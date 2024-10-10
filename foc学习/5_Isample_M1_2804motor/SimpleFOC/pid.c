

#include "MyProject.h"

/************************************************
本程序仅供学习，未经作者许可，不得用于其它任何用途
PID参数初始化、角度PID、速度PID、电流PID
使用教程：https://blog.csdn.net/loop222/article/details/119220638
创建日期：20211206
************************************************/
/******************************************************************************/
PIDController  PID_current_q,PID_current_d,PID_velocity,P_angle;
/******************************************************************************/
//所有PID参数，对于云台电机适当大一点，对于航模电机适当小一点
void PID_init(void)
{
	PID_velocity.P=0.5;  //0.5
	PID_velocity.I=0;    //0.5
	PID_velocity.D=0;
	PID_velocity.output_ramp=0;    //限制转速变化速率，
	//PID_velocity.limit=0;        //Motor_init()函数已经对limit初始化，此处无需处理
	PID_velocity.error_prev=0;
	PID_velocity.output_prev=0;
	PID_velocity.integral_prev=0;
	PID_velocity.timestamp_prev=0;
	
	P_angle.P=20;
	P_angle.I=0;
	P_angle.D=0;
	P_angle.output_ramp=0;
	//P_angle.limit=0;
	P_angle.error_prev=0;
	P_angle.output_prev=0;
	P_angle.integral_prev=0;
	P_angle.timestamp_prev=0;
	
	PID_current_q.P=0.5;  //航模电机，速度闭环，不能大于1，否则容易失控
	PID_current_q.I=0;    //电流环I参数不太好调试，只用P参数也可以
	PID_current_q.D=0;
	PID_current_q.output_ramp=0;
	//PID_current_q.limit=0;
	PID_current_q.error_prev=0;
	PID_current_q.output_prev=0;
	PID_current_q.integral_prev=0;
	PID_current_q.timestamp_prev=0;
	
	PID_current_d.P=0.5;  //0.5
	PID_current_d.I=0;
	PID_current_d.D=0;
	PID_current_d.output_ramp=0;
	//PID_current_d.limit=0;
	PID_current_d.error_prev=0;
	PID_current_d.output_prev=0;
	PID_current_d.integral_prev=0;
	PID_current_d.timestamp_prev=0;
}
/******************************************************************************/
// PID controller function
float PIDoperator(PIDController* PID,float error)
{
	unsigned long now_us;
	float Ts;
	float proportional,integral,derivative,output;
	float output_rate;
	
	now_us = SysTick->VAL;
	if(now_us<PID->timestamp_prev)Ts = (float)(PID->timestamp_prev - now_us)/9*1e-6f;
	else
		Ts = (float)(0xFFFFFF - now_us + PID->timestamp_prev)/9*1e-6f;
	PID->timestamp_prev = now_us;
	if(Ts == 0 || Ts > 0.5) Ts = 1e-3f;
	
	// u(s) = (P + I/s + Ds)e(s)
	// Discrete implementations
	// proportional part
	// u_p  = P *e(k)
	proportional = PID->P * error;
	// Tustin transform of the integral part
	// u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
	integral = PID->integral_prev + PID->I*Ts*0.5f*(error + PID->error_prev);
	// antiwindup - limit the output
	integral = _constrain(integral, -PID->limit, PID->limit);
	// Discrete derivation
	// u_dk = D(ek - ek_1)/Ts
	derivative = PID->D*(error - PID->error_prev)/Ts;
	
	// sum all the components
	output = proportional + integral + derivative;
	// antiwindup - limit the output variable
	output = _constrain(output, -PID->limit, PID->limit);
	
	// if output ramp defined
	if(PID->output_ramp > 0)
	{
		// limit the acceleration by ramping the output
		output_rate = (output - PID->output_prev)/Ts;
		if(output_rate > PID->output_ramp)output = PID->output_prev + PID->output_ramp*Ts;
		else if(output_rate < -PID->output_ramp)output = PID->output_prev - PID->output_ramp*Ts;
	}
	
	// saving for the next pass
	PID->integral_prev = integral;
	PID->output_prev = output;
	PID->error_prev = error;
	
	return output;
}
/******************************************************************************/

