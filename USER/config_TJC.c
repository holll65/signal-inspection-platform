#include "config_TJC.h"
#include "config_RS232.h"
#include "config_delay.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* printf 格式化发送时使用的临时缓冲区长度 */
#define TJC_BUF_SIZE 100

/************************************************
函数: TJC_Init
功能: 初始化陶晶驰串口屏所使用的 RS232 串口
*************************************************/
void TJC_Init(uint32_t baud)
{
    RS232_Init(baud);
}

/************************************************
函数: TJC_SendEnd
功能: 发送串口屏命令结束符 0xFF 0xFF 0xFF
说明: Nextion/TJC 指令结尾固定需要 3 个 0xFF
*************************************************/
void TJC_SendEnd(void)
{
    RS232_SendByte(0xFF);
    RS232_SendByte(0xFF);
    RS232_SendByte(0xFF);
}

/************************************************
函数: TJC_SendCmd
功能: 发送一条完整屏命令
*************************************************/
void TJC_SendCmd(const char *cmd)
{
    RS232_SendBuf((uint8_t *)cmd, strlen(cmd));
    TJC_SendEnd();
}

/************************************************
函数: TJCPrintf
功能: 通过格式化字符串拼接屏命令并发送
*************************************************/
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

/************************************************
函数: TJC_ReadNumber
功能: 发送 get 命令并读取屏返回的 32 位数值
说明: 数值返回帧以 0x71 开头，后跟 4 字节小端数据
*************************************************/
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

/************************************************
函数: TJC_GetDateTime
功能: 读取 main.tRtc.txt 文本内容
返回:
1 = 读取成功
0 = 超时失败
说明: 字符串返回帧以 0x70 开头，以 3 个 0xFF 结尾
*************************************************/
uint8_t TJC_GetDateTime(char *datetime)
{
    uint8_t ch;
    uint16_t index = 0;
    uint8_t ff_count = 0;
    uint32_t timeout = 3000;

    memset(datetime, 0, 32);

    /* 清空当前串口接收缓冲，避免上次残留数据影响本次读取 */
    RS232_ClearRxBuffer();

    TJC_SendCmd("get main.tRtc.txt");

    while(timeout--)
    {
        if(RS232_ReadByte(&ch))
        {
            if(ch == 0x70)
                continue;

            if(ch == 0xFF)
            {
                ff_count++;

                if(ff_count >= 3)
                {
                    datetime[index] = '\0';
                    return 1;
                }
            }
            else
            {
                ff_count = 0;

                if(index < 31)
                    datetime[index++] = ch;
            }
        }

        Delay_ms(1);
    }

    return 0;
}

/************************************************
函数: TJC_AddDataLog
功能: 生成一条日志并写入屏 SD 卡文件 sd0/1.data
参数:
pressure - 当前压力值
result   - 当前测试结果字符串
说明: 日志格式为 ID,时间,压力,结果\r\n
*************************************************/
void TJC_AddDataLog(uint16_t pressure, char *result)
{
    char datetime[32];
    char cmd[200];
    uint32_t log_id;

    /* 1. 从屏变量 logCount.val 读取上一次保存的日志编号 */
    log_id = TJC_ReadNumber("get DataLog.data0.qty");

    if(log_id == 0)
        log_id = 1;
    else
        log_id++;

    /* 2. 读取屏当前 RTC 字符串，失败时使用默认占位时间 */
    if(TJC_GetDateTime(datetime) == 0)
    {
        strcpy(datetime, "0000-00-00 00:00:00");
    }

    /* 3. 将新的日志编号写回屏变量，供下次追加时继续递增 */
    sprintf(cmd, "logCount.val=%lu", log_id);
    TJC_SendCmd(cmd);

    /* 4. 使用 twfile 指令把一条 CSV 记录追加到 SD 文件 */
    sprintf(cmd,
            "twfile \"sd0/1.data\",\"%lu,%s,%u,%s\\r\\n\"",
            log_id,
            datetime,
            pressure,
            result);

    TJC_SendCmd(cmd);
}
    

