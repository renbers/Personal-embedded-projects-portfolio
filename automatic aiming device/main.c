#include "main.h"
#include "stdio.h"


uint8_t KeyGet = 0 ,KeyMode = 0 ,Task = 0 ,Debug_Mode = 0 ,circle = 1;

uint32_t system_time_10ms = 300;

void OLED_Show(void);
void Key_StateMachine(void);
static void Gimbal_UpdateTaskMode(void);

int main(void)
{
    SYSCFG_DL_init();
    SysTick_Init();

    OLED_Init();                      
    Interrupt_Init();
    Vision_Init(160, 120);
    Gimbal_Init();

    motor_pid_init(&motorA, DELTA_PID, 3, 0.2, 0.3);
    motor_pid_init(&motorB, DELTA_PID, 3, 0.2, 0.3);

    while (1) {

        Key_StateMachine();
        Gimbal_UpdateTaskMode();
        OLED_Show();

        if (Vision_HasUpdate()) {
            if (vision_target.locked) {
                Gimbal_TrackError(vision_target.error_x, vision_target.error_y);
            }
            Vision_ClearUpdate();
        }

        track();
    }
}

void OLED_Show(void)
{
    OLED_Printf(0, 0, OLED_8X16, "Debug:%d",Debug_Mode);
    OLED_Printf(70, 0, OLED_8X16, "Task:%d",Task);

    OLED_ShowFloatNum(0, 16, motorA.target, 3, 0,OLED_8X16);
    OLED_ShowFloatNum(55,16, motorB.target,3,0,OLED_8X16);

    OLED_ShowNum(0, 32, Turn_Dealy, 3, OLED_8X16);
    OLED_ShowNum(60, 32, KeyGet, 3, OLED_8X16);
    OLED_Printf(0, 48, OLED_8X16, "G:%d C:%d", gimbal.mode, circle);

    OLED_Update();
}

static void Gimbal_UpdateTaskMode(void)
{
    static uint8_t last_task = 0xFFU;

    if (Task == last_task) {
        return;
    }
    last_task = Task;

    if (Task == 2 || Task == 3) {
        // 一圈和两圈任务均要求运动期间持续照射固定靶面。
        Gimbal_SetMode(GIMBAL_AIMING);
    } else {
        Gimbal_SetMode(GIMBAL_IDLE);
    }
}



void TIMER_0_INST_IRQHandler(void)                      //1ms定时中断
{
    static uint16_t Count;
    static uint16_t GimbalCount;

    if(DL_TimerG_getPendingInterrupt(TIMER_0_INST) == DL_TIMER_IIDX_ZERO) {     //1ms读取一次按键
        Key_Tick();
    }
    Count ++;
    if (Count >= 10){
        Count = 0;
        system_time_10ms ++;
        motor_pid_control();                                              //10ms更新一次底盘速度环
    }
    GimbalCount ++;
    if (GimbalCount >= 20){
        GimbalCount = 0;
        Gimbal_UpdateSpeed(0.02f);                                        //20ms更新一次云台速度环
    }

}


void Key_StateMachine(void) {

    KeyGet = Key_GetNum();

    switch (Debug_Mode) {
    case 0:
        if (KeyGet == 1) { Debug_Mode = 1; }    //进入任务选择模式
        if (KeyGet == 2) { Debug_Mode = 2; }
        if (KeyGet == 3) { Debug_Mode = 3; }
        if (KeyGet == 4) { Debug_Mode = 4; }
        if (KeyGet == 5) { Debug_Mode = 5; }
        break;

    case 1:                                 //调试模式1——任务选择模式
        if (KeyGet == 1) { Debug_Mode = 0; }
        if (KeyGet == 2) { Task = 1; }      //任务1——小车单独循迹
        if (KeyGet == 3) { Task = 2; }      //任务2——小车搭载云台循迹一圈打靶
        if (KeyGet == 4) { Task = 3; }      //任务3——小车搭载云台循迹两圈打靶
        if (KeyGet == 5) { Task = 4; }      //任务4——小车搭载云台循迹一圈画圆
        break;

    case 2:                                 //调试模式2——左电机PID参数调整
        if (KeyGet == 1){Debug_Mode = 1;}
        if (KeyGet == 3){if(motorA.p <= 1.2){motorA.p += 10;} else{motorA.p = 0.0;}}
        if (KeyGet == 4){if(motorA.i <= 1.2){motorA.i += 10;} else{motorA.i = 0.0;}}
        if (KeyGet == 5){if(motorA.d <= 1.2){motorA.d += 10;} else{motorA.d = 0.0;}}
        break;

    case 3:                                 //调试模式3——右电机PID参数调整
        if (KeyGet == 1){Debug_Mode = 1;}
        if (KeyGet == 3){if(motorB.p <= 1.2){motorB.p += 10;} else{motorB.p = 0.0;}}
        if (KeyGet == 4){if(motorB.i <= 1.2){motorB.i += 10;} else{motorB.i = 0.0;}}
        if (KeyGet == 5){if(motorB.d <= 1.2){motorB.d += 10;} else{motorB.d = 0.0;}}
        break;

    case 4:                                 //调试模式4——循迹部分参数调整
        if (KeyGet == 1){Debug_Mode = 1;}
        if (KeyGet == 2){Turn_Dealy += 10;}
        if (KeyGet == 3){Turn_Dealy -= 10;}
        break;

    case 5:                                 //调试模式5——执行模式，即操作按键将在对应任务模式中反馈
        if (KeyGet == 1){Debug_Mode = 1;}

        if (Task == 1) {
            if(KeyGet == 2){
                if(circle < 5){circle ++;}
                else {circle = 1;}
            }
        }
        if (Task == 2) {
            circle = 1;
        }
        if (Task == 3) {
            circle = 2;
        }
        if (Task == 4) {
            circle = 1;
        }
        break;

    default:
        Debug_Mode = 0;
        break;
    }

}
