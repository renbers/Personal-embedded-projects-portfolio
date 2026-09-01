#ifndef __gray_track_h_
#define __gray_track_h_
#include "main.h"

#define R3 digtal(1)
#define R2 digtal(2)
#define R1 digtal(3)
#define M  digtal(4)
#define L1 digtal(5)
#define L2 digtal(6)
#define L3 digtal(7)

typedef struct
{
    uint32_t start_time;
    uint8_t executing_flag;
}track_time;

extern uint16_t Turn_Dealy;
extern uint8_t Turn_Count;

void track(void);
void track_turn (void);
void track_line (void);
unsigned char digtal(unsigned char channel);
void track2(void);

#endif
