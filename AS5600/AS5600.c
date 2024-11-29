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
float Speed;//  ��/s

//���͵��ֽ�ʱ��
void AS5600_Write_Reg(uint16_t reg, unsigned char value)
{
	HAL_I2C_Mem_Write(&hi2c1, AS5600_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 50);
}


//���Ͷ��ֽ�ʱ��
void AS5600_Write_Regs(uint16_t reg, unsigned char *value, unsigned char len)
{
	HAL_I2C_Mem_Write(&hi2c1, AS5600_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, value, len, 50);
}


//IIC�����ֽ�
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


//IIC�����ֽ�
void AS5600_Read_DMA(void) 
{
    // ����I2C DMA����
    if (HAL_I2C_Mem_Read_DMA(&hi2c1, AS5600_ADDRESS, Angle_Hight_Register_Addr, I2C_MEMADD_SIZE_8BIT, data, DATA_SIZE) != HAL_OK) {
        de =1;
        // �����������ӡ������Ϣ
    }
}





//�õ������ƵĽǶȣ���Χ��0-6.28
float GetAngle_Without_Track(void)
{
//	float angle_d;
//	int16_t in_angle;
////	AS5600_Read_DMA( Angle_Hight_Register_Addr, data, DATA_SIZE);
//	in_angle = ((int16_t)data[0] <<8) | (data[1]);
//    angle_d = (float)in_angle / 4096 * 360.0f;//angle_dΪ�Ƕ��ƣ���Χ��0-360
////	angle_d = (float)in_angle * (2.0f*PI) / 4096;//angle_dΪ�����ƣ���Χ��0-6.28
//	return angle_d;
	
}

float GetAngle(void)
{
    float val = GetAngle_Without_Track();
    float d_angle = val - angle_prev;
    //������ת����Ȧ��
    //ͨ���жϽǶȱ仯�Ƿ����80%��һȦ(0.8f*6.28318530718f)���ж��Ƿ��������
		//��������ˣ���full_rotations����1�����d_angleС��0�������1�����d_angle����0����
//    if(fabs(d_angle) > (0.8f*2.0f*PI) ) full_rotations += ( d_angle > 0 ) ? -1 : 1;
//    angle_prev = val;
//    return (float)full_rotations * 2.0f * PI + angle_prev;
    
    if(fabs(d_angle) > (0.8f*360.0f) ) 
        full_rotations += ( d_angle > 0 ) ? -1 : 1;
    
    angle_prev = val;
    return (float)full_rotations*360.0f + angle_prev;
}


//�ٽǴ���16λ����Ӧ�Ƕ���ֵ��
int16_t angle_limiting_int16(int16_t Angl_Err, int16_t lap_encoder) {
   //|��ǰֵ - ��һ��ֵ| > ���������ֵ/2 ʱ˵���������
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
    
    volatile int erro=angle_limiting_int16(d_angle, 4095); //��ֵ�ۼ�
   
    Encoder_total += erro;
}

// DMA������ɻص�����
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
	// ��������DMA������ʵ��������ȡ
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


//OffSet��Ϊ0��
//��OffSet Ϊ ��Ƕ���
//��OffSet Ϊ ��Ƕȸ�
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