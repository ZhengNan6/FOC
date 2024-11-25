#include "AS5600.h"
#include "math.h"



float angle_prev=0;
int full_rotations=0; // full rotation tracking;
uint8_t data[2]={0};

float Angle;
float Angle_total;

//发送单字节时序
void AS5600_Write_Reg(uint16_t reg, unsigned char value)
{
	HAL_I2C_Mem_Write(&hi2c1, AS5600_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 50);
}


//发送多字节时序
void AS5600_Write_Regs(uint16_t reg, unsigned char *value, unsigned char len)
{
	HAL_I2C_Mem_Write(&hi2c1, AS5600_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, value, len, 50);
}



//IIC读多字节
void AS5600_Read_DMA(void) 
{
    // 启动I2C DMA接收
    if (HAL_I2C_Mem_Read_DMA(&hi2c1, AS5600_ADDRESS, Angle_Hight_Register_Addr, I2C_MEMADD_SIZE_8BIT, data, DATA_SIZE) != HAL_OK) {
        // 错误处理，例如打印错误信息
    }
}





//得到弧度制的角度，范围在0-6.28
float GetAngle_Without_Track(void)
{
	float angle_d;
	int16_t in_angle;
//	AS5600_Read_DMA( Angle_Hight_Register_Addr, data, DATA_SIZE);
	in_angle = ((int16_t)data[0] <<8) | (data[1]);
    angle_d = (float)in_angle / 4096;//angle_d为角度制，范围在0-360
//	angle_d = (float)in_angle * (2.0f*PI) / 4096;//angle_d为弧度制，范围在0-6.28
	return angle_d;
	
}

float GetAngle(void)
{
    float val = GetAngle_Without_Track();
    float d_angle = val - angle_prev;
    //计算旋转的总圈数
    //通过判断角度变化是否大于80%的一圈(0.8f*6.28318530718f)来判断是否发生了溢出
		//如果发生了，则将full_rotations增加1（如果d_angle小于0）或减少1（如果d_angle大于0）。
//    if(fabs(d_angle) > (0.8f*2.0f*PI) ) full_rotations += ( d_angle > 0 ) ? -1 : 1;
//    angle_prev = val;
//    return (float)full_rotations * 2.0f * PI + angle_prev;
    
    if(fabs(d_angle) > (0.8f*360.0f) ) full_rotations += ( d_angle > 0 ) ? -1 : 1;
    angle_prev = val;
    return (float)full_rotations + angle_prev;
}


// DMA传输完成回调函数
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
		Angle=GetAngle_Without_Track();
		Angle_total=GetAngle();

	// 重新启动DMA接收以实现连续读取
		AS5600_Read_DMA( Angle_Hight_Register_Addr, data, DATA_SIZE);
    }
}

float* GetAngleTotal_point(void){return &Angle_total;}
float* GetAngle_point(void){return &Angle;}