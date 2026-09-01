#include "main.h"

#define VISION_FRAME_HEADER_1  0xAA
#define VISION_FRAME_HEADER_2  0x55
#define VISION_PAYLOAD_SIZE    5

typedef enum
{
    VISION_RX_WAIT_HEADER_1 = 0,
    VISION_RX_WAIT_HEADER_2,
    VISION_RX_READ_PAYLOAD,
    VISION_RX_READ_CRC
} vision_rx_state_t;

vision_target_t vision_target;

static uint16_t vision_center_x;
static uint16_t vision_center_y;

static vision_rx_state_t vision_rx_state = VISION_RX_WAIT_HEADER_1;
static uint8_t vision_payload[VISION_PAYLOAD_SIZE];
static uint8_t vision_payload_index;

static uint8_t Vision_Crc8Update(uint8_t crc, uint8_t data)
{
    uint8_t i;

    crc ^= data;
    for (i = 0; i < 8; i++) {
        if (crc & 0x80) {
            crc = (uint8_t)((crc << 1) ^ 0x07);
        } else {
            crc <<= 1;
        }
    }

    return crc;
}

static uint8_t Vision_CalculateCrc8(const uint8_t *data, uint8_t length)
{
    uint8_t crc = 0x00;
    uint8_t i;

    for (i = 0; i < length; i++) {
        crc = Vision_Crc8Update(crc, data[i]);
    }

    return crc;
}

static void Vision_ResetReceiver(void)
{
    vision_rx_state = VISION_RX_WAIT_HEADER_1;
    vision_payload_index = 0;
}

void Vision_Init(uint16_t frame_center_x, uint16_t frame_center_y)
{
    vision_center_x = frame_center_x;
    vision_center_y = frame_center_y;
    Vision_ResetReceiver();
    Vision_UpdateTarget(frame_center_x, frame_center_y, 0);
    Vision_ClearUpdate();
}

void Vision_UpdateTarget(int16_t target_x, int16_t target_y, uint8_t locked)
{
    vision_target.target_x = target_x;
    vision_target.target_y = target_y;
    vision_target.error_x = target_x - (int16_t)vision_center_x;
    vision_target.error_y = target_y - (int16_t)vision_center_y;
    vision_target.locked = locked;
    vision_target.updated = 1;
}

uint8_t Vision_InputByte(uint8_t data)
{
    uint8_t crc_data[VISION_PAYLOAD_SIZE + 2];
    int16_t target_x;
    int16_t target_y;

    switch (vision_rx_state) {
    case VISION_RX_WAIT_HEADER_1:
        if (data == VISION_FRAME_HEADER_1) {
            vision_rx_state = VISION_RX_WAIT_HEADER_2;
        }
        break;

    case VISION_RX_WAIT_HEADER_2:
        if (data == VISION_FRAME_HEADER_2) {
            vision_payload_index = 0;
            vision_rx_state = VISION_RX_READ_PAYLOAD;
        } else if (data != VISION_FRAME_HEADER_1) {
            Vision_ResetReceiver();
        }
        break;

    case VISION_RX_READ_PAYLOAD:
        vision_payload[vision_payload_index++] = data;
        if (vision_payload_index >= VISION_PAYLOAD_SIZE) {
            vision_rx_state = VISION_RX_READ_CRC;
        }
        break;

    case VISION_RX_READ_CRC:
        crc_data[0] = VISION_FRAME_HEADER_1;
        crc_data[1] = VISION_FRAME_HEADER_2;
        for (vision_payload_index = 0; vision_payload_index < VISION_PAYLOAD_SIZE; vision_payload_index++) {
            crc_data[vision_payload_index + 2] = vision_payload[vision_payload_index];
        }

        Vision_ResetReceiver();
        if (data != Vision_CalculateCrc8(crc_data, VISION_PAYLOAD_SIZE + 2)) {
            break;
        }

        target_x = (int16_t)((uint16_t)vision_payload[0] | ((uint16_t)vision_payload[1] << 8));
        target_y = (int16_t)((uint16_t)vision_payload[2] | ((uint16_t)vision_payload[3] << 8));
        Vision_UpdateTarget(target_x, target_y, vision_payload[4]);
        return 1;

    default:
        Vision_ResetReceiver();
        break;
    }

    return 0;
}

uint8_t Vision_HasUpdate(void)
{
    return vision_target.updated;
}

void Vision_ClearUpdate(void)
{
    vision_target.updated = 0;
}
