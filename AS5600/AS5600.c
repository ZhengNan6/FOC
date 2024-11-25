#include "AS5600.h"
#include "math.h"



float angle_prev=0;
int full_rotations=0; // full rotation tracking;
uint8_t data[2]={0};

float Angle;
float Angle_total;

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
void AS5600_Read_DMA(void) 
{
    // ����I2C DMA����
    if (HAL_I2C_Mem_Read_DMA(&hi2c1, AS5600_ADDRESS, Angle_Hight_Register_Addr, I2C_MEMADD_SIZE_8BIT, data, DATA_SIZE) != HAL_OK) {
        // �����������ӡ������Ϣ
    }
}





//�õ������ƵĽǶȣ���Χ��0-6.28
float GetAngle_Without_Track(void)
{
	float angle_d;
	int16_t in_angle;
//	AS5600_Read_DMA( Angle_Hight_Register_Addr, data, DATA_SIZE);
	in_angle = ((int16_t)data[0] <<8) | (data[1]);
    angle_d = (float)in_angle / 4096;//angle_dΪ�Ƕ��ƣ���Χ��0-360
//	angle_d = (float)in_angle * (2.0f*PI) / 4096;//angle_dΪ�����ƣ���Χ��0-6.28
	return angle_d;
	
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
    
    if(fabs(d_angle) > (0.8f*360.0f) ) full_rotations += ( d_angle > 0 ) ? -1 : 1;
    angle_prev = val;
    return (float)full_rotations + angle_prev;
}


// DMA������ɻص�����
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
		Angle=GetAngle_Without_Track();
		Angle_total=GetAngle();

	// ��������DMA������ʵ��������ȡ
		AS5600_Read_DMA( Angle_Hight_Register_Addr, data, DATA_SIZE);
    }
}

float* GetAngleTotal_point(void){return &Angle_total;}
float* GetAngle_point(void){return &Angle;}