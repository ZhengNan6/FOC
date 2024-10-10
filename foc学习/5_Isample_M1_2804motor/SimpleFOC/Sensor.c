

#include "MyProject.h"


/******************************************************************************/
MagnetSensor_Para  AS5047P_para,TLE5012B_para,MA730_para;
/******************************************************************************/
long  cpr;
float full_rotation_offset;
long  angle_data_prev;
float angle_prev;
unsigned long velocity_calc_timestamp;
/******************************************************************************/
unsigned short getRawCount(void);
/******************************************************************************/
//初始化三种SPI接口的编码器的参数, 初始化I2C接口或者SPI接口
void MagneticSensor_Init(void)
{ 
	AS5047P_para.mag_cpol=SPI_CPOL_Low;
	AS5047P_para.mag_cpha=SPI_CPHA_2Edge;
	AS5047P_para.mag_BrtPre=SPI_BaudRatePrescaler_4;
	
	TLE5012B_para.mag_cpol=SPI_CPOL_Low;
	TLE5012B_para.mag_cpha=SPI_CPHA_2Edge;
	TLE5012B_para.mag_BrtPre=SPI_BaudRatePrescaler_4;
	
	MA730_para.mag_cpol=SPI_CPOL_High;
	MA730_para.mag_cpha=SPI_CPHA_2Edge;
	MA730_para.mag_BrtPre=SPI_BaudRatePrescaler_4;
	
#if M1_AS5600
	I2C_Init_();                  //AS5600
	cpr = AS5600_CPR;
	printf("AS5600\r\n");
#elif M1_AS5047P
	SPI2_Init_(&AS5047P_para);    //AS5047P
	cpr = AS5047P_CPR;
	printf("AS5047P\r\n");
#elif M1_TLE5012B
	SPI2_Init_(&TLE5012B_para);   //TLE5012B
	cpr = TLE5012B_CPR;
	printf("TLE5012B\r\n");
#elif M1_MA730
  SPI2_Init_(&MA730_para);      //MA730
	cpr = MA730_CPR;
	printf("MA730\r\n");
#endif
	
	delay_ms(10);
	
	angle_data_prev = getRawCount();
	full_rotation_offset = 0;
	velocity_calc_timestamp=0;//SysTick->VAL;
	delay_ms(5);
	angle_prev = getAngle();
}
/******************************************************************************/
unsigned short getRawCount(void)  //获取编码器的原始值
{
	unsigned short val;
	
#if M1_AS5600
	val = I2C_getRawCount(I2C1); 
#elif M1_AS5047P 
	val = ReadAS5047P(AS5047P_Angle)&0x3FFF;
#elif M1_TLE5012B
	val = ReadTLE5012B(TLE5012B_Angle)&0x7FFF;
#elif M1_MA730
	val = ReadMA730(MA730_Angle);   //高位在前，低位补0
#endif
	
	return val;
}
/******************************************************************************/
float getAngle(void)
{
	long angle_data,d_angle;
	
	angle_data = getRawCount();
	
	// tracking the number of rotations 
	// in order to expand angle range form [0,2PI] to basically infinity
	d_angle = angle_data - angle_data_prev;
	// if overflow happened track it as full rotation
	if(fabs(d_angle) > (0.8*cpr) ) full_rotation_offset += (d_angle > 0) ? -_2PI : _2PI; 
	// save the current angle value for the next steps
	// in order to know if overflow happened
	angle_data_prev = angle_data;
	
	
	if(full_rotation_offset >= ( _2PI*1000)) //转动圈数过多后浮点数精度下降，并导致堵转，每隔一千圈归零一次
	{                                        //这个问题针对电机长时间连续转动；如果不是长时间一个方向转动也可以屏蔽掉这几句
		full_rotation_offset=0;
		angle_prev = angle_prev - _2PI*1000;
	}
	if(full_rotation_offset <= (-_2PI*1000))
	{
		full_rotation_offset=0;
		angle_prev = angle_prev + _2PI*1000;
	}
	
	
	// return the full angle 
	// (number of full rotations)*2PI + current sensor angle 
	return (full_rotation_offset + ( (float)angle_data / cpr) * _2PI);
}
/******************************************************************************/
// Shaft velocity calculation
float getVelocity(void)
{
	unsigned long now_us;
	float Ts, angle_now, vel;

	// calculate sample time
	now_us = SysTick->VAL; //_micros();
	if(now_us<velocity_calc_timestamp)Ts = (float)(velocity_calc_timestamp - now_us)/9*1e-6;
	else
		Ts = (float)(0xFFFFFF - now_us + velocity_calc_timestamp)/9*1e-6;
	// quick fix for strange cases (micros overflow)
	if(Ts == 0 || Ts > 0.5) Ts = 1e-3;

	// current angle
	angle_now = getAngle();
	// velocity calculation
	vel = (angle_now - angle_prev)/Ts;

	// save variables for future pass
	angle_prev = angle_now;
	velocity_calc_timestamp = now_us;
	return vel;
}
/******************************************************************************/


