#include "config_protocol.h"

static uint8_t rx_buf[64];
static uint8_t rx_index = 0;
static uint8_t rx_len = 0;
static uint8_t state = 0;

static uint8_t CalcCRC(uint8_t *buf, uint8_t len)
{
    uint8_t crc = 0;
    uint8_t i;

    for (i = 0; i < len; i++)
    {
        crc ^= buf[i];
    }

    return crc;
}

uint8_t Protocol_CheckCRC(uint8_t *buf, uint8_t len)
{
    if (len < 1)
        return 0;

    return CalcCRC(buf, len - 1) == buf[len - 1];
}

uint8_t Protocol_ParseByte(uint8_t ch, ProtocolFrame_t *frame)
{
    uint8_t i;

    switch (state)
    {
        case 0:
            if (ch == PROTOCOL_HEAD1)
                state = 1;
            break;

        case 1:
            if (ch == PROTOCOL_HEAD2)
                state = 2;
            else
                state = 0;
            break;

        case 2:
            rx_len = ch;
            rx_index = 0;
            if (rx_len > sizeof(rx_buf))
                state = 0;
            else
                state = 3;
            break;

        case 3:
            rx_buf[rx_index++] = ch;

            if (rx_index >= rx_len)
            {
                state = 0;

                if (Protocol_CheckCRC(rx_buf, rx_len))
                {
                    frame->cmd = rx_buf[0];
                    frame->len = rx_len - 2;

                    for (i = 0; i < frame->len; i++)
                    {
                        frame->data[i] = rx_buf[1 + i];
                    }

                    return 1;
                }
            }
            break;

        default:
            state = 0;
            break;
    }

    return 0;
}
