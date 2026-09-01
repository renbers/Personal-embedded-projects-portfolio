#ifndef __VISION_H_
#define __VISION_H_

#include "main.h"

typedef struct
{
    int16_t target_x;
    int16_t target_y;
    int16_t error_x;
    int16_t error_y;
    uint8_t locked;
    uint8_t updated;
} vision_target_t;

void Vision_Init(uint16_t frame_center_x, uint16_t frame_center_y);
void Vision_UpdateTarget(int16_t target_x, int16_t target_y, uint8_t locked);
uint8_t Vision_InputByte(uint8_t data);
uint8_t Vision_HasUpdate(void);
void Vision_ClearUpdate(void);

extern vision_target_t vision_target;

#endif
