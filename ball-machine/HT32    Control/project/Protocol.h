#ifndef Protocol_H
#define Protocol_H

#include "ht32.h"

#define CmdPing                 0x01
#define CmdResetFault           0x02
#define CmdSetThrowGear         0x11
#define CmdSetServoMode         0x20
#define CmdSetChassisPosition   0x30
#define CmdSetPushRod           0x40
#define CmdQueryStatus          0x50
#define CmdEmergencyStop        0x7F

void Protocol_Process(void);

#endif
