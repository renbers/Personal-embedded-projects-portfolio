#ifndef __GIMBAL_H_
#define __GIMBAL_H_

#include "main.h"

typedef enum
{
    GIMBAL_IDLE = 0,
    GIMBAL_AIMING,
    GIMBAL_FAULT
} gimbal_mode_t;

typedef struct
{
    float kp;
    float ki;
    float kd;
    float integral;
    float last_error;
    float out_min;
    float out_max;
} gimbal_pid_t;

typedef struct
{
    gimbal_mode_t mode;
    int32_t pan_encoder_total;
    uint16_t pan_encoder_last;
    float pan_speed;
    float pan_target_speed;
    float tilt_angle;
    float tilt_target_speed;
    uint32_t last_vision_ms;
    gimbal_pid_t pan_position_pid;
    gimbal_pid_t pan_speed_pid;
    gimbal_pid_t tilt_position_pid;
} gimbal_t;

void Gimbal_Init(void);
void Gimbal_Center(void);
void Gimbal_SetMode(gimbal_mode_t mode);
void Gimbal_TrackError(float error_x, float error_y);
void Gimbal_UpdateSpeed(float dt_s);

extern gimbal_t gimbal;

#endif
