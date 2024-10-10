#include "pid.h"
#include <string.h>
#define VALUE_LIMIT(VALUE, MIN, MAX)  (((VALUE) < (MIN)) ? (MIN) : ((VALUE) > (MAX) ? (MAX) : (VALUE)))

void PidInit(pid_t* pid, pid_init_t* pid_init)
{
    memset(pid, 0, sizeof(pid_t));
    pid->P = pid_init->P;
    pid->I = pid_init->I;
    pid->D = pid_init->D;
    pid->IerrorLimit = pid_init->IerrorLimit;
    pid->OutLimit = pid_init->OutLimit;
}

float PidCalc(pid_t* pid, float SetValue, float ActuallValue)
{
    pid->Error = SetValue - ActuallValue;
    pid->Ierror += pid->Error;
    VALUE_LIMIT(pid->Ierror, -pid->IerrorLimit, pid->IerrorLimit);

    pid->Out = pid->P * pid->Error + pid->I * pid->Ierror;
    VALUE_LIMIT(pid->Out, -pid->OutLimit, pid->OutLimit);

    return pid->Out;
}