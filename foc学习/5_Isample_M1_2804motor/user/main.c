
#include "stm32f10x.h"
#include <stdlib.h>
#include "MyProject.h"

/************************************************
SimpleMotor开发板
电流采样  演示
=================================================
本程序仅供学习，引用代码请标明出处
使用教程：https://blog.csdn.net/loop222/article/details/120497425
         《SimpleFOC移植STM32(五) —— 电流采样及其变换》
创建日期：20230208
作    者：loop222 @郑州
************************************************/
/******************************************************************************/
#define LED_blink    GPIOC->ODR^=(1<<13)
/******************************************************************************/
void commander_run(void);
/******************************************************************************/
float target;
long timecntr_pre=0;
long time_cntr=0;
/******************************************************************************/
int timecount(void)
{
	long Ts,now_us;
	
	now_us = SysTick->VAL;
	if(now_us<timecntr_pre)Ts =(timecntr_pre - now_us)/9;   //us
	else
		Ts = (0xFFFFFF - now_us + timecntr_pre)/9;
	if(Ts > 500000) Ts = 0;	// >0.5S 第一次上电或错误
	timecntr_pre = now_us;
	
	return Ts;
}
/******************************************************************************/
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);//使能GPIOA,GPIOB,GPIOC,AFIO;
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;         //PC13是LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  //速度
	GPIO_Init(GPIOC, &GPIO_InitStructure);             //对选中管脚初始化
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);                 //上电点亮LED
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          //PB9是motor1的使能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);                  //低电平解除,Motor_init()中使能
}
/******************************************************************************/
int main(void)
{
	GPIO_Config();
	uart_init(115200);
	TIM2_PWM_Init();
	
	delay_ms(1000);            //Wait for the system to stabilize
	MagneticSensor_Init();     //初始化编码器参数 和 I2C或者SPI
	InlineCurrentSense(0.001,50,ADC_Channel_3,ADC_Channel_4,NOT_SET);    //SimpleMotor//采样电阻阻值，运放倍数，A相，B相，C相
	InlineCurrentSense_Init(); //ADC初始化和偏置电压校准
	LPF_init();                //LPF参数初始化
	PID_init();                //PID参数初始化
	
	voltage_power_supply=12;   //V 电源电压
	pole_pairs=7;              //电机极对数，按照实际设置，虽然可以上电检测但有失败的概率
	voltage_sensor_align=2;    //V alignSensor() use it，航模电机设置的值小一点比如0.5-1，云台电机设置的大一点比如2-3
	voltage_limit=6;           //V，主要为限制电机最大电流，最大值需小于12/1.732=6.9
	velocity_limit=20;         //rad/s 角度模式时限制最大转速，力矩模式和速度模式不起作用
	current_limit=50;          //A，foc_current和dc_current模式限制电流，不能为0。速度模式和位置模式起作用
	torque_controller=Type_voltage;  //Type_dc_current;//  Type_foc_current;  //Type_voltage;
	controller=Type_velocity;  //Type_torque;  //Type_velocity;  //Type_angle; 
	PID_current_d.P=0.6;       //电流环PI参数，可以进入 PID_init() 函数中修改其它参数
	PID_current_d.I=0;         //电流环I参数不太好调试，设置为0只用P参数也可以
	PID_current_q.P=0.6;
	PID_current_q.I=0;
	PID_velocity.P=0.1;        //0.5, 速度环PI参数，只用P参数方便快速调试
	PID_velocity.I=1;
	P_angle.P=20;              //位置环参数，只需P参数
	PID_velocity.output_ramp=50; //速度爬升斜率，如果不需要可以设置为0
	LPF_velocity.Tf=0.0001;      //Tf设置小一点，配合爬升斜率设置，速度切换更平稳；如果没有爬升模式的斜率限制，Tf太小电机容易抖动。
	target=0;
	
	Motor_init();
	Motor_initFOC(0,UNKNOWN);  //(2.2,CW);(0,UNKNOWN);  //如果填入零点偏移角度和方向，将跳过上电检测。电机极对数要设置正确。 
  printf("Motor ready.\r\n");
	
	systick_CountMode();   //systick时钟开启1ms中断模式，不能再调用delay_us()和delay_ms()函数
	
	while(1)
	{
		time_cntr +=timecount();
		if(time_cntr>=500000)  //us
		{
			time_cntr=0;
			LED_blink;
			//printf("V=%.4f\r\n",shaft_velocity);  //当前速度，打印会影响代码的执行效率
			//printf("A=%.4f\r\n",shaft_angle);     //当前绝对角度
		}
		move(target);
		loopFOC();
		commander_run();
	}
}
/******************************************************************************/
//通信协议只有简单的设置速度，不能与官方上位机对接
void commander_run(void)
{
	if((USART_RX_STA&0x8000)!=0)
	{
		switch(USART_RX_BUF[0])
		{
			case 'A':   //打印当前机械角度
				printf("A=%.4f\r\n",shaft_angle);
				break;
			case 'H':
				printf("Hello World!\r\n");
				break;
			case 'T':   //T6.28
				target=atof((const char *)(USART_RX_BUF+1));
				printf("T=%.4f\r\n", target);
				break;
			case 'P':   //P,设置速度模式的P参数,例如设置P=0.8，发送指令P0.8
				PID_velocity.P=atof((const char *)(USART_RX_BUF+1));
				printf("P=%.4f!\r\n",PID_velocity.P);
				break;
			case 'I':   //I,设置速度模式的I参数,例如设置I=0.8，发送指令I0.8
				PID_velocity.I=atof((const char *)(USART_RX_BUF+1));
				printf("I=%.4f!\r\n",PID_velocity.I);
				break;
		}
		USART_RX_STA=0;
	}
}
/******************************************************************************/



