#ifndef __SMO_H__
#define __SMO_H__
#include "stdint.h"
typedef struct
{
    float A;//离散化关系下 I(k) = A*I(k-1)+B*(Z) 
    float B;//离散化关系下 I(k) = A*I(k-1)+B*(Z) 
    float vAlpha;//电机电压
    float vBeta;//电机电压
    float realAlphaCurrent;//实际电流
    float realBetaCurrent;//实际电流
    float AlphaCurreantHat;//观测电流
    float BetaCurreantHat;//观测电流
    float ErrAlphaCurrent;//观测电流误差
    float ErrBetaCurrent;//观测电流误差
    float alphaZ;//观测反电动势
    float betaZ;//观测反电动势
    float H;//滑膜增益 
    float lpfK;//低通滤波系数
    float Etheta;//电角度
}Observer_t;
void SMO_observer(void);
#endif