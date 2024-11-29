#include "AS5600.h"
#include "math.h"
#include "stm32f1xx_it.h"

extern uint64_t time2_ms;

uint16_t OffSet = 0;

uint8_t receive_Start = 0;
uint8_t data[2];
uint16_t Last_Encoder;
uint16_t Encoder = 0;
int64_t Encoder_total = 0;

float angle_prev=0;
int full_rotations=0; // full rotation tracking;

float Angle;
float Angle_total;
float Speed;//  °/s

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
void AS5600_Read_Reg(uint16_t reg, uint8_t* buf, uint8_t len)
{
	HAL_I2C_Mem_Read(&hi2c1, AS5600_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 50);
}


//float GetSpeed(void)
//{
//    return 
//}

uint8_t de;
int16_t in_angle;
uint8_t temp[2]={0};


//IIC读多字节
void AS5600_Read_DMA(void) 
{
    // 启动I2C DMA接收
    if (HAL_I2C_Mem_Read_DMA(&hi2c1, AS5600_ADDRESS, Angle_Hight_Register_Addr, I2C_MEMADD_SIZE_8BIT, data, DATA_SIZE) != HAL_OK) {
        de =1;
        // 错误处理，例如打印错误信息
    }
}





//得到弧度制的角度，范围在0-6.28
float GetAngle_Without_Track(void)
{
//	float angle_d;
//	int16_t in_angle;
////	AS5600_Read_DMA( Angle_Hight_Register_Addr, data, DATA_SIZE);
//	in_angle = ((int16_t)data[0] <<8) | (data[1]);
//    angle_d = (float)in_angle / 4096 * 360.0f;//angle_d为角度制，范围在0-360
////	angle_d = (float)in_angle * (2.0f*PI) / 4096;//angle_d为弧度制，范围在0-6.28
//	return angle_d;
	
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
    
    if(fabs(d_angle) > (0.8f*360.0f) ) 
        full_rotations += ( d_angle > 0 ) ? -1 : 1;
    
    angle_prev = val;
    return (float)full_rotations*360.0f + angle_prev;
}


//临角处理16位（对应角度正值）
int16_t angle_limiting_int16(int16_t Angl_Err, int16_t lap_encoder) {
   //|当前值 - 上一次值| > 编码器最大值/2 时说明向上溢出
   if (Angl_Err < -(lap_encoder / 2))
   {
       Angl_Err += (lap_encoder - 1);
   }
   if (Angl_Err > (lap_encoder / 2)) {
       Angl_Err -= (lap_encoder - 1);
   }
   return Angl_Err;
}

void Encoder_Deal(void)
{
    Encoder = ((uint16_t)data[0] <<8) | (data[1]);
    int16_t d_angle = Encoder - Last_Encoder;
    Last_Encoder = Encoder;
    
    volatile int erro=angle_limiting_int16(d_angle, 4095); //差值累加
   
    Encoder_total += erro;
}

// DMA传输完成回调函数
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
//		Angle=GetAngle_Without_Track();
//		Angle_total=GetAngle();
        receive_Start = 1;
        Encoder_Deal();
//        static uint8_t init = 1;
//        if (init)
//        {
//            init = 0;
//            Last_Encoder = Encoder;
//            Encoder_total = Encoder;
//        }
//        
//        Speed = (Angle_total - Angle_total_last)/(time2_ms - time2_ms_last / 1000000.0f);
//        
//        time2_ms_last = time2_ms;
//        Angle_total_last = Angle_total;
	// 重新启动DMA接收以实现连续读取
		AS5600_Read_DMA();
    }
}


/**
  *@brief 
  *@param 
  */
int16_t min_distance(void)
{
    if(abs(OffSet - Encoder) > 2048)
    {
        if(4095 - Encoder < 2048)
        {
            return 4095 - Encoder + OffSet;
        }
        else
        {
            return 4095 - Encoder + OffSet;
        }
    }
    else
    {
        return OffSet - Encoder;
    }
}


//OffSet作为0°
//＜OffSet 为 电角度正
//＞OffSet 为 电角度负
void Set_Zero(void)
{
    OffSet = Encoder;
}
float Get_OffSet_Angle(void)
{
    int16_t close = OffSet - Encoder;
    if (close > 2045)   return (close - 4095)/4095.0f*360.0f;
    if (close < -2045)  return (4095 + close)/4095.0f*360.0f;
    return close/4095.0f*360.0f;
}


float* GetAngleTotal_point(void){return &Angle_total;}
float* GetAngle_point(void){return &Angle;}