#ifndef __CONFIG_PROTOCOL_H
#define __CONFIG_PROTOCOL_H

#include "stm32f10x.h"
#include <stdint.h>

/*
 * 自定义二进制协议：
 * 帧头 AA 55 + 长度 + 命令 + 数据 + CRC
 */

#define PROTOCOL_HEAD1      0xAA
#define PROTOCOL_HEAD2      0x55

typedef enum
{
    CMD_PRESSURE_DATA = 0x01,
    CMD_START_TEST    = 0x10,
    CMD_STOP_TEST     = 0x11,
    CMD_ALARM         = 0x20
} ProtocolCmd_t;

typedef struct
{
    uint8_t cmd;
    uint8_t data[32];
    uint8_t len;
} ProtocolFrame_t;

uint8_t Protocol_ParseByte(uint8_t ch, ProtocolFrame_t *frame);
uint8_t Protocol_CheckCRC(uint8_t *buf, uint8_t len);

#endif
