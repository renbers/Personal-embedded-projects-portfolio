#ifndef __Servo_GPTM1_H
#define __Servo_GPTM1_H


//-----------------------------------------------------------------------------
#include "ht32.h"

//-----------------------------------------------------------------------------
void Servo_PWM_Init(void);
void Servo_Mode_Choose(uint8_t KeyNum);
void Servo_SetAngle(uint8_t AngleValue);

#endif

