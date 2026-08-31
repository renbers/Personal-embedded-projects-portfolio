#include "Protocol.h"
#include "GPIO.h"
#include "Motor1.h"
#include "Motor2.h"
#include "USART0.h"

#define Sof0             0xAA
#define Sof1             0x55
#define MaxPayload       4
#define InvalidPayload   0xFF
#define AckPayloadLen    2

#define CmdAck           0x80
#define CmdNack          0x81

#define StatusOk         0x00
#define ErrCrc           0x01
#define ErrUnknownCmd    0x03
#define ErrParam         0x05

/*
 * Frame format:
 *   AA 55  Seq  Cmd  Payload...  CRC_L CRC_H
 *
 * The CRC16-Modbus range is Seq + Cmd + Payload only.
 * SOF(AA 55) and CRC bytes themselves are not included.
 */
extern volatile int8_t KeyMotor;
extern volatile int8_t KeyServo;
extern volatile u8 ThrowLeftSpeed;
extern volatile u8 ThrowRightSpeed;

/* Receive buffer stores Seq + Cmd + Payload + CRC. The SOF is consumed by state machine. */
static u8 s_rx_buf[2 + MaxPayload + 2];
static u8 s_rx_index = 0;
static u8 s_payload_len = 0;
static u8 s_push_state = 0;

typedef enum
{
  RxSof0 = 0,     /* Wait for 0xAA */
  RxSof1,         /* Wait for 0x55 */
  RxFixed,        /* Read Seq + Cmd */
  RxPayload,      /* Read command payload */
  RxCrc0,         /* Read CRC low byte */
  RxCrc1          /* Read CRC high byte and verify */
} RxState;

static RxState s_rx_state = RxSof0;

/* Payload length is fixed by Cmd, so the frame does not need a length byte. */
static u8 GetPayloadLen(u8 cmd)
{
  switch (cmd)
  {
    case CmdPing:
    case CmdResetFault:
    case CmdQueryStatus:
    case CmdEmergencyStop:
      return 0;

    case CmdSetThrowGear:
    case CmdSetServoMode:
    case CmdSetChassisPosition:
    case CmdSetPushRod:
      return 1;

    default:
      return InvalidPayload;
  }
}

static u16 Crc16(const u8 *data, u16 len)
{
  u16 crc = 0xFFFF;
  u16 i;
  u8 bit;

  for (i = 0; i < len; i++)
  {
    crc ^= data[i];
    for (bit = 0; bit < 8; bit++)
    {
      if ((crc & 0x0001) != 0)
      {
        crc = (crc >> 1) ^ 0xA001;
      }
      else
      {
        crc >>= 1;
      }
    }
  }

  return crc;
}

/* Build and send a complete frame. CRC is appended in little-endian order. */
static void SendFrame(u8 seq, u8 cmd, const u8 *payload, u8 payload_len)
{
  u8 frame[2 + 2 + MaxPayload + 2];
  u16 crc;
  u8 index = 0;
  u8 i;

  if (payload_len > MaxPayload)
  {
    return;
  }

  frame[index++] = Sof0;
  frame[index++] = Sof1;
  frame[index++] = seq;
  frame[index++] = cmd;

  for (i = 0; i < payload_len; i++)
  {
    frame[index++] = payload[i];
  }

  crc = Crc16(&frame[2], (u16)(2 + payload_len));
  frame[index++] = (u8)(crc & 0xFF);
  frame[index++] = (u8)((crc >> 8) & 0xFF);

  USART0_tx_data(frame, index);
}

static void SendAck(u8 seq, u8 cmd)
{
  u8 payload[AckPayloadLen];

  /* ACK payload: original Cmd + status code. */
  payload[0] = cmd;
  payload[1] = StatusOk;
  SendFrame(seq, CmdAck, payload, AckPayloadLen);
}

static void SendNack(u8 seq, u8 cmd, u8 error_code)
{
  u8 payload[AckPayloadLen];

  /* NACK payload: original Cmd + error code. */
  payload[0] = cmd;
  payload[1] = error_code;
  SendFrame(seq, CmdNack, payload, AckPayloadLen);
}

/* Gear mode maps the old key-style motor states to two PWM percentages. */
static void SetThrowGear(u8 gear)
{
  KeyMotor = (int8_t)gear;

  switch (gear)
  {
    case 0:
      ThrowLeftSpeed = 0;
      ThrowRightSpeed = 0;
      Motor_SetPWM(0);
      Motor2_SetPWM(0);
      break;

    case 1:
      ThrowLeftSpeed = 70;
      ThrowRightSpeed = 70;
      break;

    case 2:
      ThrowLeftSpeed = 85;
      ThrowRightSpeed = 85;
      break;

    case 3:
      ThrowLeftSpeed = 100;
      ThrowRightSpeed = 100;
      break;

    default:
      break;
  }
}

static void ClearBoardTriggers(void)
{
  GPIO_ClearOutBits(HT_GPIOD, GPIO_PIN_3);
  GPIO_ClearOutBits(HT_GPIOC, GPIO_PIN_10);
  GPIO_ClearOutBits(HT_GPIOC, GPIO_PIN_11);
  GPIO_ClearOutBits(HT_GPIOC, GPIO_PIN_12);
  GPIO_ClearOutBits(HT_GPIOC, GPIO_PIN_14);
  GPIO_ClearOutBits(HT_GPIOC, GPIO_PIN_15);
}

/* Chassis is still controlled by HT32 -> STM32 GPIO trigger pulses. */
static void TriggerChassisPosition(u8 position_id)
{
  if (position_id == 4)
  {
    GPIO_SetOutBits(HT_GPIOC, GPIO_PIN_10);
  }
  else if (position_id == 1)
  {
    GPIO_SetOutBits(HT_GPIOC, GPIO_PIN_11);
  }
  else if (position_id == 5)
  {
    GPIO_SetOutBits(HT_GPIOC, GPIO_PIN_12);
  }
}

static void TriggerPushRod(u8 action)
{
  if (action == 0)
  {
    GPIO_SetOutBits(HT_GPIOC, GPIO_PIN_15);
  }
  else if (action == 1)
  {
    GPIO_SetOutBits(HT_GPIOD, GPIO_PIN_3);
  }
  else if (action == 2)
  {
    GPIO_SetOutBits(HT_GPIOC, GPIO_PIN_14);
  }

  s_push_state = action;
}

/* Emergency stop only handles local HT32 state and available GPIO trigger lines. */
static void EmergencyStop(void)
{
  KeyServo = 0;
  SetThrowGear(0);
  ClearBoardTriggers();
  TriggerPushRod(0);
}

static void SendStatus(u8 seq)
{
  u8 payload[MaxPayload];

  /* Status payload: MotorGear, ServoMode, PushState, FaultCode. */
  payload[0] = (u8)KeyMotor;
  payload[1] = (u8)KeyServo;
  payload[2] = s_push_state;
  payload[3] = 0;

  SendFrame(seq, CmdQueryStatus, payload, MaxPayload);
}

static void ExecuteFrame(const u8 *frame)
{
  u8 seq = frame[0];
  u8 cmd = frame[1];
  const u8 *payload = &frame[2];

  switch (cmd)
  {
    case CmdPing:
    case CmdResetFault:
      SendAck(seq, cmd);
      break;

    case CmdSetThrowGear:
      if (payload[0] > 3)
      {
        SendNack(seq, cmd, ErrParam);
        return;
      }
      SetThrowGear(payload[0]);
      SendAck(seq, cmd);
      break;

    case CmdSetServoMode:
      if (payload[0] > 3)
      {
        SendNack(seq, cmd, ErrParam);
        return;
      }
      /* Servo mode controls launch interval: 0 stop, 1 slow, 2 medium, 3 fast. */
      KeyServo = (int8_t)payload[0];
      SendAck(seq, cmd);
      break;

    case CmdSetChassisPosition:
      if (payload[0] != 1 && payload[0] != 4 && payload[0] != 5)
      {
        SendNack(seq, cmd, ErrParam);
        return;
      }
      TriggerChassisPosition(payload[0]);
      SendAck(seq, cmd);
      break;

    case CmdSetPushRod:
      if (payload[0] > 2)
      {
        SendNack(seq, cmd, ErrParam);
        return;
      }
      TriggerPushRod(payload[0]);
      SendAck(seq, cmd);
      break;

    case CmdQueryStatus:
      SendStatus(seq);
      break;

    case CmdEmergencyStop:
      EmergencyStop();
      SendAck(seq, cmd);
      break;

    default:
      SendNack(seq, cmd, ErrUnknownCmd);
      break;
  }
}

static void ResetRx(void)
{
  s_rx_state = RxSof0;
  s_rx_index = 0;
  s_payload_len = 0;
}

/* Feed one byte into the frame parser. It resyncs by searching AA 55 continuously. */
static void InputByte(u8 data)
{
  u8 cmd;
  u16 rx_crc;
  u16 calc_crc;

  switch (s_rx_state)
  {
    case RxSof0:
      if (data == Sof0)
      {
        s_rx_state = RxSof1;
      }
      break;

    case RxSof1:
      if (data == Sof1)
      {
        s_rx_index = 0;
        s_rx_state = RxFixed;
      }
      else if (data != Sof0)
      {
        s_rx_state = RxSof0;
      }
      /* If another 0xAA arrives here, keep waiting for 0x55 without losing sync. */
      break;

    case RxFixed:
      s_rx_buf[s_rx_index++] = data;
      if (s_rx_index >= 2)
      {
        cmd = s_rx_buf[1];
        s_payload_len = GetPayloadLen(cmd);
        if (s_payload_len > MaxPayload)
        {
          /* Cmd is invalid before payload is known, so reject early. */
          SendNack(s_rx_buf[0], cmd, ErrUnknownCmd);
          ResetRx();
        }
        else if (s_payload_len == 0)
        {
          s_rx_state = RxCrc0;
        }
        else
        {
          s_rx_state = RxPayload;
        }
      }
      break;

    case RxPayload:
      s_rx_buf[s_rx_index++] = data;
      if (s_rx_index >= (2 + s_payload_len))
      {
        s_rx_state = RxCrc0;
      }
      break;

    case RxCrc0:
      s_rx_buf[s_rx_index++] = data;
      s_rx_state = RxCrc1;
      break;

    case RxCrc1:
      s_rx_buf[s_rx_index++] = data;
      /* Received CRC is stored little-endian: low byte first, high byte second. */
      rx_crc = (u16)s_rx_buf[2 + s_payload_len] |
               ((u16)s_rx_buf[2 + s_payload_len + 1] << 8);
      calc_crc = Crc16(s_rx_buf, (u16)(2 + s_payload_len));

      if (rx_crc == calc_crc)
      {
        ExecuteFrame(s_rx_buf);
      }
      else
      {
        SendNack(s_rx_buf[0], s_rx_buf[1], ErrCrc);
      }

      ResetRx();
      break;

    default:
      ResetRx();
      break;
  }
}

void Protocol_Process(void)
{
  u8 data;

  /* USART interrupt fills the ring buffer; main loop drains it and parses frames here. */
  while (USART0_read_byte(&data))
  {
    InputByte(data);
  }
}
