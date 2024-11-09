#ifndef _FocCommon_H
#define _FocCommon_H
#include "FocMath.h"
/*******************************************************************************
 * @brief    һ�׵�ͨ�˲���
 * @param    oldValue ��Ҫ���ֵ  newValue��ֵ  k �˲�ϵ��
 * @return   
 ******************************************************************************/
#define LPFStep(oldValue,newValue,k)   {oldValue = (oldValue +  k * (newValue - oldValue));}

typedef struct
{
	 iq_t out;
	 iq_t target;
	 iq_t inc;
}ramp_t;
#endif