#include "foc.h"
#include "svpwm.h"
#include "tim.h"
#include "math.h"

float Ualpha;
float Ubeta;

//�޷�����
float my_limit(float *limit, float limit_max, float limit_min)
{
	if(*limit > limit_max)
		*limit = limit_max;
	if(*limit < limit_min)
		*limit = limit_min;
	
	return *limit;
	
}

//��һ������
//���Ƕ�������[0, 2PI]
float normalizeAngle(float angle)
{
	float a;
	a = fmod(angle, 2*PI);
	
	return a>=0?a:(a+2*PI);
}

//Park��任  svpwm���
void back_park(float Uq, float Ud, float eAngle)
{
	normalizeAngle(eAngle);
	Ualpha = Ud*cos(eAngle) - Uq*sin(eAngle);
	Ubeta  = Ud*sin(eAngle) + Uq*cos(eAngle);
	
	svpwm(Ualpha, Ubeta);
	
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, Tc*250);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, Tb*250);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, Ta*250);
}

