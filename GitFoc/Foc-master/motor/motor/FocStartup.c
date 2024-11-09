#include "FocStartup.h"
#include "FocConfig.h"
#include "FocDataBase.h"
#include "string.h"
#include "arm_math.h"

/*******************************************************************************
 * @brief  ��ʼ������
 * @param   
 * @return  
 ******************************************************************************/
void IfStartupInit(ifStartup_t *pHandle)
{
	memset(pHandle,0,sizeof(ifStartup_t));
}

iq_t Q15AngleMod(iq_t angle)
{
	 if(angle > 32768)
	 {
		 angle -= 65536;
	 }
}
/*******************************************************************************
 * @brief   if����
 * @param   pHandle ifʵ�� obTheta�۲����Ƕ�
 * @return  
 ******************************************************************************/
void IfStartupStep(ifStartup_t *pHandle,iq_t obTheta,iq_t sin_cos[])
{
	iq_t theta;
	switch(pHandle->state)
	{
		case 0://Ԥ��λ�׶�
			if(pHandle->iq < Q15(0.3f / CURRENT_BASE))
			{
				 pHandle->iq += Q15(0.001f/ CURRENT_BASE );
				 pHandle->theta = Q15AngleMod(Q15(ALGIN_THETA / THETA_BASE));
				 theta = pHandle->theta; 
			}
			else
			{
				pHandle->state = 1;
				theta = Q15AngleMod(Q15(ALGIN_THETA / THETA_BASE) + Q15(0.25f * 6.28f / THETA_BASE));
				pHandle->theta = theta;
			}
			break;
		case 1://���ٽ׶�
			pHandle->iq = Q15(IF_STARTUP_IQ / CURRENT_BASE);
		   //������ٶ�
			if(pHandle->omega < Q15(IF_STARTUP_OMEGA / OMEGA_BASE))
			{
					pHandle->omega += (iq_t)Q15(IF_STARTUP_OMEGA_ACC / OMEGA_BASE / SPEED_DIV_FOC_RATE);
					
			}
			else
			{
				pHandle->omega = Q15(IF_STARTUP_OMEGA / OMEGA_BASE );
				pHandle->state = 2;
			}
			//�����ٶ�
			pHandle->theta += pHandle->omega / SPEED_DIV_FOC_RATE;
			pHandle->theta = Q15AngleMod(pHandle->theta);
			pHandle->time += Q15(FOC_PWM_PERIOD);
			theta = pHandle->theta;
			break;
		case 2://ƥ��׶�
			pHandle->theta += pHandle->omega / SPEED_DIV_FOC_RATE;
			pHandle->theta = Q15AngleMod(pHandle->theta);
			pHandle->thetaDiff = obTheta - pHandle->theta;
			if(pHandle->thetaDiff > 0)
			{
				pHandle->thetaDiff = Q15(M_2PI / THETA_BASE) - pHandle->thetaDiff;
			}
			//��ȡ����ֵ
			float error = pHandle->thetaDiff ;
			if(error < 0)
			{
				error = -error;
			}
			
			pHandle->iq -= Q15Mpy(error ,(IF_STARTUP_IQ_KP * M_2PI / CURRENT_BASE));
			if(pHandle->iq < Q15(IF_STARTUP_IQ_MIN / CURRENT_BASE))
			{
				pHandle->iq = Q15(IF_STARTUP_IQ_MIN / CURRENT_BASE);
			}
			if(error <= IF_STARTUP_THETA_DIFF)
			{
				pHandle->diffCnt++;
			}
			else
			{
				pHandle->diffCnt = 0;
			}
			pHandle->time +=  Q15(FOC_PWM_PERIOD);
			//
			if(pHandle->diffCnt > 10000 || pHandle->time >= Q15(IF_STARTUP_MAX_TIME))
			{
					pHandle->state = 3;
			}
			theta = pHandle->theta;
			break;
		case 3://����۲���
			pHandle->theta = obTheta - pHandle->thetaDiff ;
			if(pHandle->thetaDiff < -Q15(0.1f / THETA_BASE))
			{
				pHandle->thetaDiff += Q15(0.1f / THETA_BASE);
			}
			else if(pHandle->thetaDiff > Q15(0.1f / THETA_BASE))
			{
				pHandle->thetaDiff -= Q15(0.1f / THETA_BASE);
			}
			else
			{
				pHandle->thetaDiff = 0;
			}
			theta = obTheta;
			break;
	}
	pHandle->err = obTheta - pHandle->theta;

	if(pHandle->err < -Q15MAX / 2.0)
	{
		pHandle->err += Q15MAX;
	}
	else	if(pHandle->err > Q15MAX / 2.0)
	{
		pHandle->err-=Q15MAX;
	}
	Q15SinCos(theta,sin_cos);
}

