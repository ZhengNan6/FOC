#ifndef __PID_H__
#define __PID_H__

typedef struct 
{
    float P;
    float I;
    float D;
    float Error;
    float Ierror;
    float IerrorLimit;
    float OutLimit;
    float Out;
}pid_t;

typedef struct 
{
    float P;
    float I;
    float D;
    float IerrorLimit;
    float OutLimit;
}pid_init_t;

void PidInit(pid_t* pid, pid_init_t* pid_init);
float PidCalc(pid_t* pid, float SetValue, float ActuallValue);
#endif