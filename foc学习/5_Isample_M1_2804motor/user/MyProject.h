

#ifndef MYPROJECT_H
#define MYPROJECT_H

/* Includes ------------------------------------------------------------------*/

#include "stm32f10x_it.h" 
#include "usart.h"
#include "delay.h"
#include "timer.h"
#include "adc.h"
#include "iic.h"
#include "spi2.h"

#include "MagneticSensorI2C.h" 
#include "MagneticSensorSPI.h" 
#include "Sensor.h" 
#include "foc_utils.h"
#include "FOCMotor.h"
#include "BLDCmotor.h" 
#include "lowpass_filter.h" 
#include "pid.h"
#include "InlineCurrentSense.h"
#include "CurrentSense.h"

#define M1_Enable    GPIO_SetBits(GPIOB,GPIO_Pin_9);          //高电平使能
#define M1_Disable   GPIO_ResetBits(GPIOB,GPIO_Pin_9);        //低电平失能

//设置使用的编码器为1，不使用的为0
#define M1_AS5600    1   //编码器类型，只能选一
#define M1_AS5047P   0
#define M1_TLE5012B  0
#define M1_MA730     0

#endif

