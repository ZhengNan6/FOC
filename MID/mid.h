#ifndef __MID_H__
#define __MID_H__
#include "stdint.h"
#define ADC_VALUE_OF_CURRENT 0.00146484375f //¡À3A 12bit 6/4096
typedef enum{
    A = 4, //0b 100
    B = 2, //0b 010
    C = 1, //0b 001
    AB = 6,//0b 110
    AC = 5,//0b 101
    BC = 3 //0b 011
}SamplingPhash_e;

void StartPhashPwm(void);
void GetSamplingCurrent(void);
void SetPhashPwm(uint16_t A, uint16_t B, uint16_t C);
void SetSamplingPhash();

#endif