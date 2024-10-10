#ifndef SENSOR_LIB_H
#define SENSOR_LIB_H

/******************************************************************************/
typedef struct 
{
	unsigned short mag_cpol;
	unsigned short mag_cpha;
	unsigned short mag_BrtPre;
}MagnetSensor_Para;
/******************************************************************************/
extern long  cpr;
extern float angle_prev;
/******************************************************************************/
void MagneticSensor_Init(void);
float getAngle(void);
float getVelocity(void);
/******************************************************************************/

#endif
