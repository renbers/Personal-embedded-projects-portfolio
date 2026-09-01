#include "main.h"

motor_pid_t motorA;
motor_pid_t motorB;
motor_pid_t angle;

void motor_pid_init(motor_pid_t *pid, uint32_t mode, float p, float i, float d)
{
    pid->pid_mode = mode;
    pid->p = p;
    pid->i = i;
    pid->d = d;
}

void motor_target_set(int spe1, int spe2)
{
    if (spe1 >= 0) {
        motorA_dir = 1;
        motorA.target = spe1;
    } else {
        motorA_dir = 0;
        motorA.target = -spe1;
    }

    if (spe2 >= 0) {
        motorB_dir = 1;
        motorB.target = spe2;
    } else {
        motorB_dir = 0;
        motorB.target = -spe2;
    }
}

void motor_pid_control(void)
{
    // 编码器计数换算为当前速度，并清零等待下一次采样。
    motorA.now = Encoder_count1 * 4;
    motorB.now = Encoder_count2 * 4;
    Encoder_count1 = 0;
    Encoder_count2 = 0;

    // 左右电机分别执行增量式速度环 PID。
    motor_pid_calculate(&motorA);
    motor_pid_calculate(&motorB);
    motor_pid_output_limit(&motorA);
    motor_pid_output_limit(&motorB);

    motorA_duty(motorA.out);
    motorB_duty(motorB.out);
}

void motor_pid_calculate(motor_pid_t *pid)
{
    pid->error[0] = pid->target - pid->now;

    if (pid->pid_mode == DELTA_PID) {
        pid->pout = pid->p * (pid->error[0] - pid->error[1]);
        pid->iout = pid->i * pid->error[0];
        pid->dout = pid->d * (pid->error[0] - 2 * pid->error[1] + pid->error[2]);
        pid->out += pid->pout + pid->iout + pid->dout;
    } else if (pid->pid_mode == POSITION_PID) {
        pid->pout = pid->p * pid->error[0];
        pid->iout += pid->i * pid->error[0];
        pid->dout = pid->d * (pid->error[0] - pid->error[1]);
        pid->out = pid->pout + pid->iout + pid->dout;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
}

void motor_pid_output_limit(motor_pid_t *pid)
{
    if (pid->out >= 80) {
        pid->out = 80;
    }
    if (pid->out <= 0) {
        pid->out = 0;
    }
}

void Dir_cal(float Err)
{
    /*
    if (Err <= 1.5 && Err >= -1.5) { motor_target_set(30, 30); }
    if ((Err <= 5 && Err >= 1.5) || (Err >= -5 && Err <= -1.5)) { motor_target_set(35, 30); }
    if ((Err <= 10 && Err > 5) || (Err >= -10 && Err < -5)) { motor_target_set(40, 30); }
    if ((Err <= 30 && Err > 10) || (Err >= -30 && Err < -10)) { motor_target_set(45, 20); }
    if (Err > 30 && Err < -30) {
        motor_pid_calculate(&angle);
        motor_target_set(-angle.out, angle.out);
    }
    */
}
