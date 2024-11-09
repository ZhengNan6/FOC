#include "PIController.h"
#include "FocConfig.h"
/**
 * @brief   pi����
 * @param   targetĿ��ֵ  realʵ��ֵ
 * @return  out���ֵ
 */
void PIControllerStep(PIController_t *pi)
{
		iq_t integ,out;
		pi->err = pi->target - pi->real;
	  integ = pi->integral;
		integ += Q15Mpy(pi->ki , pi->err); 
		out = integ + Q15Mpy(pi->kp , pi->err);
		//��ֹ���ֱ��ͣ��޷�����
		if(out > pi->maxValue)
		{
			out = pi->maxValue;
		}
		else if(out < pi->minValue )	
		{
			out = pi->minValue;
		}
		else
		{
			
		}
		pi->integral = integ;
		pi->out = out;
 }