#ifndef __MOTOR_PID_H__
#define __MOTOR_PID_H__

#include "main.h"

enum
{
    POSITION_PID = 0,  // 位置式 PID
    DELTA_PID,         // 增量式 PID
};

typedef struct
{
    float target;
    float now;
    float error[3];
    float p, i, d;
    float pout, dout, iout;
    float out;

    uint32_t pid_mode;
} motor_pid_t;

void motor_pid_init(motor_pid_t *pid, uint32_t mode, float p, float i, float d);
void motor_pid_control(void);
void motor_pid_calculate(motor_pid_t *pid);
void motor_pid_output_limit(motor_pid_t *pid);
void motor_target_set(int spe1, int spe2);
void Dir_cal(float Err);

extern motor_pid_t motorA;
extern motor_pid_t motorB;
extern motor_pid_t angle;

#endif
