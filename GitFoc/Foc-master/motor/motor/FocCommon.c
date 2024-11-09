#include "FocCommon.h"

/*******************************************************************************
 * @brief    ���º���
 * @param    target Ŀ��ֵ  inc����ֵ 
 * @return   out���ֵ
 ******************************************************************************/
void RampStep(ramp_t *pHandle)
{
	 if(pHandle->target < pHandle->out)
	 {
		  pHandle->out += pHandle->inc;
			if(pHandle->out >= pHandle->target)		
			{
					pHandle->out = pHandle->target;
			}
	 }
	 else if(pHandle->target > pHandle->out)
	 {
		  pHandle->out -= pHandle->inc;
			if(pHandle->out <= pHandle->target)		
			{
					pHandle->out = pHandle->target;
			}
	 }
	 else
	 {
		 pHandle->out = pHandle->target;
	 }
}