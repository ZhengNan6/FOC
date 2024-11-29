#ifndef __AS5600_H__
#define __AS5600_H__

#include "main.h"
#include <stdio.h>
#include "i2c.h"

#define PI														3.14159265358979f
#define AS5600_ADDRESS                0x36<<1		//设备从地址
#define Write_Bit                 		0	   			//写标记
#define Read_Bit                  		1    			//读标记
#define Angle_Hight_Register_Addr 		0x0C 			//寄存器高位地址
#define Angle_Low_Register_Addr   		0x0D 			//寄存器低位地址
#define DATA_SIZE 										2 				// 每次读取2字节数据

void Set_Zero(void);
void AS5600_Read_Reg(uint16_t reg, unsigned char* buf, uint8_t len);
void AS5600_Write_Reg(uint16_t reg, unsigned char value);

//float GetSpeed(void);
//float GetAngle(void);
int16_t min_distance(void);
float* GetAngleTotal_point(void);
float* GetAngle_point(void);
float Get_OffSet_Angle(void);
void AS5600_Read_DMA(void);

#endif
