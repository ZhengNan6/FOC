#ifndef _FOCCONFIG_H
#define _FOCCONFIG_H
#define  M_2PI                          6.2831853072f
#define  PWM_PERIOD              				(4199)
#define  FOC_PWM_PERIOD          				(0.00005f)
//???????????
#define  MOTOR_PHASE_RESISTANCE  				(3.5f)
#define  MOTOR_PHASE_INDUCTANCE  				(0.0010f)
#define  MOTOR_PLOES             				(2.0f)
#define  MOTOR_KE                				(4.2f)
//????????
#define  ADC_SAMPLE_RESISTANCE  				(0.010f)//
#define  ADC_AMP                				(5.0f)
#define  VBUS_RATE               				(21.0f)

//????????????????
#define OB_SMC_GAIN                     2.0f
#define OB_SMC_LPF_K                    0.2f
#define OB_SMC_PLL_KP                   (40.0f)
#define OB_SMC_PLL_KI                   (400.0f)
//???¦Ë???
#define ALGIN_THETA                    (15.0f / 360.0f * 6.28f - 90.0 / 360.0f * 6.28f)
//if????????
#define IF_STARTUP_BEGIN_THETA         (280.0f / 360.0f * 6.28f)
#define IF_STARTUP_IQ_MIN              (0.5f)  //?????§³????
#define IF_STARTUP_IQ                  (0.7f)  //???????
#define IF_STARTUP_IQ_KP               (0.1f)  //???????????
#define IF_STARTUP_OMEGA               (50.0f * 6.28f) //?????????????
#define IF_STARTUP_OMEGA_ACC           (50.0f * 6.28f)//??????
#define IF_STARTUP_THETA_DIFF          (0.10f * 6.28f)//?????§³???
#define IF_STARTUP_MAX_TIME            (3.0f)//??????????
//???????
#define  MOTOR_CONTORL_VOLTAGE_OPEN_LOOP 0 
//???????
#define  MOTOR_CONTORL_CURRENT_LOOP      1 
//????+??????
#define  MOTOR_CONTORL_SPEED_LOOP        2 
//?????????
#define  MOTOR_CONTORL_MODE              MOTOR_CONTORL_CURRENT_LOOP


//??????????
#define CURRENT_IQ_KP                    (MOTOR_PHASE_INDUCTANCE * 1024) 
#define CURRENT_IQ_KI                    (MOTOR_PHASE_RESISTANCE * 1024) 
#define CURRENT_IQ_OUT_MAX               (0.4f * 12.0f)
#define CURRENT_IQ_OUT_MIN               (-0.4f * 12.0f)

//??????????
#define CURRENT_ID_KP                    (MOTOR_PHASE_INDUCTANCE * 1024) 
#define CURRENT_ID_KI                    (MOTOR_PHASE_RESISTANCE * 1024) 
#define CURRENT_ID_OUT_MAX               (0.4f * 12.0f)
#define CURRENT_ID_OUT_MIN               (-0.4f * 12.0f)
#endif
