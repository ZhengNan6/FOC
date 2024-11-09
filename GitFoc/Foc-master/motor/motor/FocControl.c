#include "FocControl.h"
#include "FocConfig.h"
#include "FocObserver.h"
#include "FocComponentsSvpwm.h"
#include "arm_math.h" 
#include "PIController.h"
#include <string.h>
#include "tim.h"
#include "ADC.h"
#include "FocSample.h"
#include "FocStartUp.h"
#include "focDebug.h"
PIController_t gIDLoop;   //d�������
PIController_t gIQLoop;   //q�������
PIController_t gSpeedLoop;//�ٶȻ�
ComponentsSvpwm_t gSvpwm; //svpwm�ṹ��
FocSample_t gAdcSample;   //���������ṹ��
ifStartup_t gIfStartup;   //if�����ṹ��
ObserverSmc_t gObSmc;     //�۲����ṹ��
control_t gFocMachine;    //foc״̬�� 


void CurrentLoopTest(void)
{
		static float time;
		time += FOC_PWM_PERIOD;
		if(time < 5.0f)
		{
			 gIfStartup.iq = 0.05f;
		}
		else if(time >= 5.0f && time < 10.0f)
		{
			gIfStartup.iq = 0.5f;
		}
		else
		{
			time = 0;
		}
}
/*******************************************************************************
 * @brief   foc������
 * @param   
 * @return  
 ******************************************************************************/
void FocMain(void)
{
	 BspInit();
	gFocMachine.sw = 1;
	gFocMachine.target = 0.5f;
	 for(;;)
		{
			DoControl(&gFocMachine);
			VofaGraphSend(2);
		}
}
/*******************************************************************************
 * @brief   �����ʼ��
 * @param   
 * @return  
 ******************************************************************************/
void BspInit(void)
{
	  TIM1->CCR4 = 4100;
		CurrentSampleInit(&gAdcSample);
}
/*******************************************************************************
 * @brief   �Ը���ģ����г�ʼ��
 * @param   
 * @return  
 ******************************************************************************/
void ControlInit(void)
{
	memset(&gIDLoop,0,sizeof(gIDLoop));
	memset(&gIQLoop,0,sizeof(gIQLoop));
	memset(&gSvpwm ,0,sizeof(gSvpwm));
	memset(&gIfStartup ,0,sizeof(gIfStartup));
	memset(&gObSmc ,0,sizeof(gObSmc));
	//D�������ʼ��
	gIDLoop.kp = Q15(CURRENT_ID_KP / R_BASE);
	gIDLoop.ki = Q15(CURRENT_ID_KI * FOC_PWM_PERIOD / R_BASE);
	gIDLoop.maxValue = Q15(CURRENT_ID_OUT_MAX / U_BASE);
	gIDLoop.minValue = Q15(CURRENT_ID_OUT_MIN / U_BASE);
	//Q�������ʼ��
	gIQLoop.kp = Q15(CURRENT_IQ_KP / R_BASE);
	gIQLoop.ki = Q15(CURRENT_ID_KI * FOC_PWM_PERIOD / R_BASE);
	gIQLoop.maxValue = Q15(CURRENT_IQ_OUT_MAX / U_BASE);
	gIQLoop.minValue = Q15(CURRENT_IQ_OUT_MIN / U_BASE);
	//�۲�����ʼ��
	ObserverSmcInit(&gObSmc);
	//svpwm��ʼ��
	gSvpwm.vBus = Q15(12.0f / U_BASE);
	
	#if MOTOR_CONTORL_MODE == MOTOR_CONTORL_VOLTAGE_OPEN_LOOP //��ѹ�������� id����
	gIDLoop.maxValue = 24 * 0.5;
	gIDLoop.ki = 14 * 0.0005f;
	gIDLoop.kp = 0.010f * 0.0005f;
	gIDLoop.kc = 0;
	gSvpwm.vBus = 24.0;
  #endif 
}
/*******************************************************************************
 * @brief   ADC�жϣ�focִ������
 * @param   
 * @return  
 ******************************************************************************/
void ADC_IRQHandler()
{
	
	if(__HAL_ADC_GET_FLAG(&hadc2,ADC_FLAG_JEOC) == SET)
	{
		__HAL_ADC_CLEAR_FLAG(&hadc2,ADC_FLAG_JEOC);
		TIM1->CCR4 = 4160;
		//�������� 
		CurrentSampleStep(&gAdcSample,gSvpwm.sin_cos);
		//���������
		gIDLoop.target = 0;
		gIDLoop.real = gAdcSample.iD;
//�ٶȱջ�ģʽ
#if  MOTOR_CONTORL_SPEED_LOOP == MOTOR_CONTORL_MODE
		
#endif
//		gSpeedLoop.target = gFocMachine.target;
//		gSpeedLoop.real   =  gObSmc.oemga;
//		PIControllerStep(&gSpeedLoop);

		gIQLoop.target = gIfStartup.iq;
		gIQLoop.real = gAdcSample.iQ;		
		
		PIControllerStep(&gIQLoop);
		PIControllerStep(&gIDLoop);
		//����Ƕ�
		ObserverSmcStep(&gObSmc,gSvpwm.vAlpha,gSvpwm.vBeta,gAdcSample.iAlpha,gAdcSample.iBeta);
		//if����
		IfStartupStep(&gIfStartup,gObSmc.theta,&gSvpwm.sin_cos[0]);
		//gSvpwm.vBus = Q16(24.0f / U_BASE);
		gSvpwm.vd = gIDLoop.out;
		gSvpwm.vq = gIQLoop.out;
		SvpwmStep(&gSvpwm);
		//AdcSampleTest();
		//svpwmʱ�丳ֵ����ʱ��
		TIM1->CCR1 = gSvpwm.ta;
		TIM1->CCR2 = gSvpwm.tb;
		TIM1->CCR3 = gSvpwm.tc;
		float graph[3];
		graph[0] = gSvpwm.ta;
		graph[1] = gAdcSample.aCurrent;

////		graph[0] = gObSmc.alphaEMF;
////		graph[1] = gObSmc.betaEMF;
//		//graph[1] = gSvpwm.ta ;
//		graph[0] = Q16ToF(gAdcSample.aCurrent) * CURRENT_BASE;
//		graph[1] = gSvpwm.ta;
//		graph[2] = 0;
  		GraphDataCopy(graph,3);

		
	}
}
/*******************************************************************************
 * @brief   PWM��ʼ
 * @param   
 * @return  
 ******************************************************************************/
void PwmStart(void)
{
	TIM1->CNT = 0;
	TIM1->CR1 |= 0x01; 
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,0);
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,0);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	
	TIM1->CCR4 = 4199 - 120;
	//__HAL_TIM_ENABLE_IT
	__HAL_TIM_ENABLE(&htim1);
	HAL_TIM_Base_Start(&htim1);	
}
/*******************************************************************************
 * @brief   PWMֹͣ
 * @param   
 * @return  
 ******************************************************************************/
void PwmStop(void)
{
	TIM1->CR1 &=~0x01; 
	__HAL_TIM_DISABLE(&htim1);
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,0);
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,0);
	HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_3);;	
}
/*******************************************************************************
 * @brief   �������״̬��
 * @param   pHandle����ʵ��
 * @return  
 ******************************************************************************/
void DoControl(control_t *pHandle)
{

	switch(pHandle->machineState)
	{
#if MOTOR_CONTORL_MODE == MOTOR_CONTORL_VOLTAGE_OPEN_LOOP //��ѹ�������� id����
#define OLTAGE_OPEN_LOOP_OMEGA             (10.0f * 6.28f)   //����ʱ�ٶ� 
#define OLTAGE_OPEN_LOOP_ID                (0.3A)   //����ʱD�����
		case FOC_STATE_IDLE:
			if(pHandle->sw)
			{
				ControlInit();
				pHandle->machineState = FOC_STATE_ALIGN;
				pHandle->openVoltageTheta = 0;
				PwmStart();
			}
			break;
		
		case FOC_STATE_ALIGN:	//����Ƕ�0
		for(float i = 0.001f;i < 0.3f;i += 0.001f)
		{
			 gIDLoop.target = i;
		}
		pHandle->machineState = FOC_STATE_RUN;
			break;
		
		case FOC_STATE_RUN://����λ�ú󿪻��϶��������
			gSvpwm.vd = 2.0f;
		  gSvpwm.vq = 0.0f;
		  pHandle->openVoltageTheta += OLTAGE_OPEN_LOOP_OMEGA * FOC_PWM_PERIOD;
			if(pHandle->openVoltageTheta > 6.28f)
			{
				 pHandle->openVoltageTheta -= 6.28f;
			}
			gSvpwm.sin_cos[0] = arm_sin_f32(pHandle->openVoltageTheta);
			gSvpwm.sin_cos[1] = arm_sin_f32(pHandle->openVoltageTheta);
			break;
		case FOC_STATE_STOP:
			break;
#elif MOTOR_CONTORL_MODE == MOTOR_CONTORL_CURRENT_LOOP //������ģʽ
		case FOC_STATE_IDLE:
			if(pHandle->sw)
			{
				ControlInit();
				pHandle->machineState = FOC_STATE_RUN;
				pHandle->openVoltageTheta = 0;
				PwmStart();
			}
			break;
		
		case FOC_STATE_ALIGN:	//����Ƕ�0
		for(float i = 0.001f;i < 0.3f;i += 0.001f)
		{
			 gIQLoop.target = i;
			
		}
		pHandle->machineState = FOC_STATE_RUN;
		break;	
		case FOC_STATE_RUN://ֱ�ӽ���ջ�
			//gIQLoop.target = gIfStartup.iq;
			break;
		case FOC_STATE_STOP:
			ControlInit();
			break;
#endif
	}
}
