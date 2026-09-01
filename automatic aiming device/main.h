#ifndef _MAIN_H_
#define _MAIN_H_

extern unsigned int system_time_10ms;
extern unsigned char Key_Num ,circle;

#include <stdio.h>
#include <string.h>

#include "clock.h"
#include "interrupt.h"


#include "ti_msp_dl_config.h"
#include "oled.h"
#include "Key.h"
#include "Motor.h"
#include "motor_pid.h"
#include "gray_track.h"
#include "gimbal.h"
#include "vision.h"


#endif  /* #ifndef _MAIN_H_ */
