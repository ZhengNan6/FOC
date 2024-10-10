#ifndef __FOC_MATH_H__
#define __FOC_MATH_H__

#include "stdint.h"
#include "pid.h"

#define Udc 24
#define PI 3.1415926f

#define user_abs(x) ((x) > (0) ? (x) : (-(x)))
#define SQRT_3_DIV_2 0.8660254037844386f
#define SQRT3_INV 0.57735026918962576450914878050196f // 1/sqrt(3)
#define SQET_3      1.7320508075688772935274463415059f
#define SQET_3_2    0.86602540378443864676372317075294f

typedef struct
{
    float A;
    float B;
    float C;
}Phase_U_t;//ABC相电压
typedef struct
{
    float A;
    float B;
}Alpha_Beta_U_t;//α β坐标系电压
typedef struct
{
    float q;
    float d;
}q_d_U_t;//q d坐标系电压

typedef struct
{
    float A;
    float B;
    float C;
    float Alpha;
    float Beta;
    float q;
    float d;
}FOC_U_t;//FOC控制中各个坐标系的电压

typedef struct
{
    float A;
    float B;
    float C;
}Phase_I_t;//ABC相电流
typedef struct
{
    float A;
    float B;
}Alpha_Beta_I_t;//α β坐标系电流
typedef struct
{
    float q;
    float d;
}q_d_I_t;//q d坐标系电流

typedef struct
{
    float A;//指向DMA采集的三个相电流数据
    float B;
    float C;
    float Alpha;
    float Beta;
    float q;
    float d;
    uint8_t SetSimplingPhash; //目前正在电流采样的相
    uint8_t NowSimplingPhash;//目前已完成电流采样的相
}FOC_I_t;//FOC控制中各个坐标系的电流

typedef struct
{
    float A;
    float B;
    float C;
}PWM_t;

typedef struct
{
    FOC_U_t U;
    FOC_I_t I;
    PWM_t PWM;
    pid_t Uq_PI;
    pid_t Ud_PI;
    float angle;
}FOC_t;

FOC_t* GetFOC(void);
void FOC_Init(void);
void FOCSetCurrent(float SetCurrent);
void qd_to_AlphaBeta(void);
void AlphaBeta_to_Phase(void);

void Phase_to_AlphaBeta(void);
void AlphaBeta_to_qd(void);

float sin_calculate(float angle);
float cos_calculate(float angle);
#endif