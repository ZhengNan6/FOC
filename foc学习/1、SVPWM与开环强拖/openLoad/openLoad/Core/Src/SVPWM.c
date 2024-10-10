#include "svpwm.h"
#include "stdio.h"

float Ta;
float Tb;
float Tc;

void svpwm(float Ualpha, float Ubeta)
{
	//1.u1 u2 u3
	float u1 = Ubeta;
	float u2 = -_sqrt3_2*Ualpha - Ubeta*_1_2;
	float u3 = _sqrt3_2*Ualpha  - Ubeta*_1_2;
	
	//2.��ȡ������
	uint8_t sector = (u3 >0 ) + ((u2 > 0) << 1) + ((u1 > 0) << 2);
	
	//3.�ܽ�ʸ������ʱ���
	if(sector == 5)    //����1
	{
		float T4 = u3;
		float T6 = u1;
		
		float sum = T4+T6;
		if(sum > Ts)
		{
			float k = Ts/sum;
			
			T4 = k*T4;
			T6 = k*T6;
		}
		
		float T0 = (Ts - T4 - T6)/2;
		
		Ta = T4+T6+T0;
		Tb = T6+T0;
		Tc = T0;
	}
	else if(sector == 4)    //����2
	{
		float T2 = -u3;
		float T6 = -u2;
		
		float sum = T2+T6;
		if(sum > Ts)
		{
			float k = Ts/sum;
			
			T2 = k*T2;
			T6 = k*T6;
		}
		
		float T0 = (Ts - T2 - T6)/2;
		
		Ta = T6+T0;
		Tb = T2+T6+T0;
		Tc = T0;
	}
	else if(sector == 6)    //����3
	{
		float T2 = u1;
		float T3 = u2;
		
		float sum = T2+T3;
		if(sum > Ts)
		{
			float k = Ts/sum;
			
			T2 = k*T2;
			T3 = k*T3;
		}
		
		float T0 = (Ts - T2 - T3)/2;
		
		Ta = T0;
		Tb = T2+T3+T0;
		Tc = T3+T0;
	}
	else if(sector == 2)    //����4
	{
		float T1 = -u1;
		float T3 = -u3;
		
		float sum = T1+T3;
		if(sum > Ts)
		{
			float k = Ts/sum;
			
			T1 = k*T1;
			T3 = k*T3;
		}
		
		float T0 = (Ts - T1 - T3)/2;
		
		Ta = T0;
		Tb = T3+T0;
		Tc = T1+T3+T0;
	}
	else if(sector == 3)    //����5
	{
		float T1 = u2;
		float T5 = u3;
		
		float sum = T1+T5;
		if(sum > Ts)
		{
			float k = Ts/sum;
			
			T1 = k*T1;
			T5 = k*T5;
		}
		
		float T0 = (Ts - T1 - T5)/2;
		
		Ta = T5+T0;
		Tb = T0;
		Tc = T1+T5+T0;
	}
	else if(sector == 1)    //����6
	{
		float T4 = -u2;
		float T5 = -u1;
		
		float sum = T4+T5;
		if(sum > Ts)
		{
			float k = Ts/sum;
			
			T4 = k*T4;
			T5 = k*T5;
		}
		
		float T0 = (Ts - T4 - T5)/2;
		
		Ta = T4+T5+T0;
		Tb = T0;
		Tc = T5+T0;
	}
}
