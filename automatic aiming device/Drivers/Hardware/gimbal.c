#include "main.h"

static const float GimbalDefaultDt = 0.02;
static const float PanEncoderCountsPerRevolution = 12800.0;
static const float SearchSpeedDegPerSec = 120.0;
static const uint32_t VisionLostTimeoutMs = 100;
static const float TiltCenterAngle = 90.0;
static const float TiltMinAngle = 20.0;
static const float TiltMaxAngle = 160.0;
static const float TiltAngleRange = 180.0;
static const uint16_t ServoMinPulseUs = 500;
static const uint16_t ServoMaxPulseUs = 2500;
static const uint16_t PanPwmMax = 1000;

gimbal_t gimbal;

static float Gimbal_Limit(float value, float lower, float upper)
{
    if (value < lower) {
        return lower;
    }
    if (value > upper) {
        return upper;
    }
    return value;
}

static float Gimbal_PidCalculate(gimbal_pid_t *pid, float error, float dt_s)
{
    float derivative;
    float output;

    pid->integral += error * dt_s;
    derivative = (error - pid->last_error) / dt_s;
    output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
    pid->last_error = error;

    return Gimbal_Limit(output, pid->out_min, pid->out_max);
}

static void Gimbal_SetLaser(uint8_t enabled)
{
    if (enabled) {
        DL_GPIO_setPins(Laser_Enable_PORT, Laser_Enable_EN_PIN);
    } else {
        DL_GPIO_clearPins(Laser_Enable_PORT, Laser_Enable_EN_PIN);
    }
}

static void Gimbal_SetPanDuty(float duty)
{
    uint16_t compare;

    duty = Gimbal_Limit(duty, -100.0f, 100.0f);
    if (duty < 0.0f) {
        DL_GPIO_clearPins(Pan_MotorDirection_PORT, Pan_MotorDirection_PH_PIN);
        duty = -duty;
    } else {
        DL_GPIO_setPins(Pan_MotorDirection_PORT, Pan_MotorDirection_PH_PIN);
    }

    compare = (uint16_t)(duty * PanPwmMax / 100.0f + 0.5f);
    DL_TimerG_setCaptureCompareValue(
        Pan_MotorPWM_INST, compare, GPIO_Pan_MotorPWM_C0_IDX);
}

static void Gimbal_SetTiltAngle(float angle)
{
    uint16_t pulse;

    gimbal.tilt_angle = Gimbal_Limit(angle, TiltMinAngle, TiltMaxAngle);
    pulse = (uint16_t)(ServoMinPulseUs +
        gimbal.tilt_angle / TiltAngleRange *
            (ServoMaxPulseUs - ServoMinPulseUs) + 0.5f);
    DL_TimerG_setCaptureCompareValue(
        Tilt_ServoPWM_INST, pulse, GPIO_Tilt_ServoPWM_C1_IDX);
}

void Gimbal_Init(void)
{
    gimbal.pan_position_pid.kp = 1.20f;
    gimbal.pan_position_pid.ki = 0.0f;
    gimbal.pan_position_pid.kd = 0.05f;
    gimbal.pan_position_pid.out_min = -240.0f;
    gimbal.pan_position_pid.out_max = 240.0f;

    gimbal.pan_speed_pid.kp = 0.08f;
    gimbal.pan_speed_pid.ki = 0.02f;
    gimbal.pan_speed_pid.kd = 0.0f;
    gimbal.pan_speed_pid.out_min = -100.0f;
    gimbal.pan_speed_pid.out_max = 100.0f;

    gimbal.tilt_position_pid.kp = 0.035f;
    gimbal.tilt_position_pid.ki = 0.0f;
    gimbal.tilt_position_pid.kd = 0.002f;
    gimbal.tilt_position_pid.out_min = -80.0f;
    gimbal.tilt_position_pid.out_max = 80.0f;

    Gimbal_Center();
    Gimbal_SetMode(GIMBAL_IDLE);
}

void Gimbal_Center(void)
{
    gimbal.pan_encoder_last = DL_TimerG_getTimerCount(Pan_Encoder_INST);
    gimbal.pan_encoder_total = 0;
    gimbal.pan_speed = 0.0f;
    gimbal.pan_target_speed = 0.0f;
    gimbal.tilt_target_speed = 0.0f;
    Gimbal_SetTiltAngle(TiltCenterAngle);
    Gimbal_SetPanDuty(0.0f);
}

void Gimbal_SetMode(gimbal_mode_t mode)
{
    gimbal.mode = mode;

    if (mode == GIMBAL_IDLE || mode == GIMBAL_FAULT) {
        gimbal.pan_target_speed = 0.0f;
        gimbal.tilt_target_speed = 0.0f;
        Gimbal_SetPanDuty(0.0f);
        Gimbal_SetLaser(0);
        return;
    }

    gimbal.pan_target_speed = SearchSpeedDegPerSec;
    gimbal.tilt_target_speed = 0.0f;
    Gimbal_SetLaser(1);
}

void Gimbal_TrackError(float error_x, float error_y)
{
    if (gimbal.mode != GIMBAL_AIMING) {
        return;
    }

    gimbal.pan_target_speed = Gimbal_PidCalculate(
        &gimbal.pan_position_pid, -error_x, GimbalDefaultDt);
    gimbal.tilt_target_speed = Gimbal_PidCalculate(
        &gimbal.tilt_position_pid, error_y, GimbalDefaultDt);
    gimbal.last_vision_ms = tick_ms;
}

void Gimbal_UpdateSpeed(float dt_s)
{
    uint16_t encoder_now;
    int16_t encoder_delta;
    float pan_duty;

    if (gimbal.mode == GIMBAL_IDLE || gimbal.mode == GIMBAL_FAULT) {
        return;
    }

    if (DL_GPIO_readPins(Pan_MotorFault_PORT, Pan_MotorFault_nFault_PIN) == 0) {
        Gimbal_SetMode(GIMBAL_FAULT);
        return;
    }

    encoder_now = DL_TimerG_getTimerCount(Pan_Encoder_INST);
    encoder_delta = (int16_t)(encoder_now - gimbal.pan_encoder_last);
    gimbal.pan_encoder_last = encoder_now;
    gimbal.pan_encoder_total += encoder_delta;
    gimbal.pan_speed = encoder_delta * 360.0f /
        PanEncoderCountsPerRevolution / dt_s;

    if (tick_ms - gimbal.last_vision_ms > VisionLostTimeoutMs) {
        gimbal.pan_target_speed = SearchSpeedDegPerSec;
        gimbal.tilt_target_speed = 0.0f;
    }

    pan_duty = Gimbal_PidCalculate(&gimbal.pan_speed_pid,
        gimbal.pan_target_speed - gimbal.pan_speed, dt_s);
    Gimbal_SetPanDuty(pan_duty);
    Gimbal_SetTiltAngle(gimbal.tilt_angle + gimbal.tilt_target_speed * dt_s);
}
