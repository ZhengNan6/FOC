#ifndef __FOC_H__
#define __FOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define PI        3.1415926535


//�޷�����
float my_limit(float *limit, float limit_max, float limit_min);
//��һ������
float normalizeAngle(float angle);

void back_park(float Uq, float Ud, float eAngle);

#ifdef __cplusplus
}
#endif

#endif /* __FOC_H__ */

