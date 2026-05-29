#include "config_TJC.h"
#include "config_RS232.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define TJC_BUF_SIZE 100

void TJC_Init(uint32_t baud)
{
    RS232_Init(baud);
}

void TJC_SendEnd(void)
{
    RS232_SendByte(0xFF);
    RS232_SendByte(0xFF);
    RS232_SendByte(0xFF);
}

void TJC_SendCmd(const char *cmd)
{
    RS232_SendBuf((uint8_t *)cmd, strlen(cmd));
    TJC_SendEnd();
}

void TJCPrintf(const char *fmt, ...)
{
    char buf[TJC_BUF_SIZE];
    va_list args;
    int len;

    va_start(args, fmt);
    len = vsnprintf(buf, TJC_BUF_SIZE, fmt, args);
    va_end(args);

    if (len <= 0)
        return;

    if (len >= TJC_BUF_SIZE)
        len = TJC_BUF_SIZE - 1;

    RS232_SendBuf((uint8_t *)buf, len);
    TJC_SendEnd();
}

/*
 * 系统状态变量发送给屏
 * 1 = 系统正常
 * 0 = 系统异常
 *
 * 屏里变量名：sys
 */
void TJC_SetSystemState(uint8_t system_ok)
{
    if (system_ok)
    {
        TJCPrintf("sys.val=1");
    }
    else
    {
        TJCPrintf("sys.val=0");
    }
}

/*
 * 产品测试结果
 *
 * 0 = 未测试
 * 1 = 合格
 * 2 = 不合格
 */
void TJC_SetProductResult(uint8_t result)
{
    if(result > 2)
    {
        result = 0;
    }

    TJCPrintf("result.val=%d", result);
}
// void TJC_SetMainT0(const char *txt)
// {
//     TJCPrintf("main.t0.txt=\"%s\"", txt);
// }
uint32_t TJC_ReadNumber(const char *cmd)
{
    uint8_t ch;
    uint8_t buf[4];
    uint8_t index = 0;
    uint8_t ff_count = 0;

    TJC_SendCmd(cmd);

    while(1)
    {
        if(RS232_ReadByte(&ch))
        {
            if(ch == 0x71)
            {
                index = 0;

                while(index < 4)
                {
                    if(RS232_ReadByte(&buf[index]))
                    {
                        index++;
                    }
                }

                return (uint32_t)buf[0]
                     | ((uint32_t)buf[1] << 8)
                     | ((uint32_t)buf[2] << 16)
                     | ((uint32_t)buf[3] << 24);
            }

            if(ch == 0xFF)
            {
                ff_count++;
                if(ff_count >= 3)
                {
                    break;
                }
            }
        }
    }

    return 0;
}
