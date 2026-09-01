#include "main.h"


void motorA_duty(int duty);
void motorB_duty(int duty);
void Motor2_Calculate(int8_t Direction);


extern int Encoder_count1, Encoder_count2;
extern int speed_now;
extern uint8_t motorA_dir, motorB_dir;
