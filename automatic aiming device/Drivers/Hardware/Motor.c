#include "main.h"


uint8_t motorA_dir = 1; // 1ÎªÕý×ª 0Îª·´×ª
uint8_t motorB_dir = 1; // 1ÎªÕý×ª 0Îª·´×ª

int Encoder_count1 = 0;
int Encoder_count2 = 0;

int speed_now;


void motorA_duty(int Out1){
    
    //PB6   PB7     作为极性输出    PA12    作为PWM波输出
    if (motorA_dir){
            DL_GPIO_clearPins(GPIO_Motor_PORT, GPIO_Motor_Motor_L1_PIN);
            DL_GPIO_setPins(GPIO_Motor_PORT, GPIO_Motor_Motor_L2_PIN);
            DL_TimerG_setCaptureCompareValue(Motor_Pid_INST, Out1, DL_TIMER_CC_0_INDEX);
        }

    else{
            DL_GPIO_setPins(GPIO_Motor_PORT, GPIO_Motor_Motor_L1_PIN);
            DL_GPIO_clearPins(GPIO_Motor_PORT, GPIO_Motor_Motor_L2_PIN);
            DL_TimerG_setCaptureCompareValue(Motor_Pid_INST,Out1, DL_TIMER_CC_0_INDEX);
        }
    
}


void motorB_duty(int Out2){

    //PB13   PB14     作为极性输出    PA13    作为PWM波输出
    if (motorB_dir){
            DL_GPIO_clearPins(GPIO_Motor_PORT, GPIO_Motor_Motor_R1_PIN);
            DL_GPIO_setPins(GPIO_Motor_PORT, GPIO_Motor_Motor_R2_PIN);
            DL_TimerG_setCaptureCompareValue(Motor_Pid_INST, Out2, DL_TIMER_CC_1_INDEX);
        }

    else{
            DL_GPIO_setPins(GPIO_Motor_PORT, GPIO_Motor_Motor_R1_PIN);
            DL_GPIO_clearPins(GPIO_Motor_PORT, GPIO_Motor_Motor_R2_PIN);
            DL_TimerG_setCaptureCompareValue(Motor_Pid_INST,Out2, DL_TIMER_CC_1_INDEX);
        }
}