#include "config_TJC.h"
#include "config_RS232.h"
#include <stdio.h>
#include <string.h>

/*
 * TJC X5 串口屏命令结尾：0xFF 0xFF 0xFF
 * 本模板默认 TJC 与控制板共用 USART1 RS232 总线。
 * 如果你最终采用独立串口，建议把 TJC_SendRaw 改到 UART4/USART3。
 */

static void TJC_SendRaw(const uint8_t *buf, uint16_t len)
{
    RS232_SendBuf((uint8_t *)buf, len);
}

void TJC_Init(uint32_t baud)
{
    RS232_Init(baud);
}

void TJC_SendCmd(const char *cmd)
{
    uint8_t end[3] = {0xFF, 0xFF, 0xFF};

    TJC_SendRaw((const uint8_t *)cmd, strlen(cmd));
    TJC_SendRaw(end, 3);
}

void TJC_SetText(const char *obj, const char *txt)
{
    char cmd[128];

    sprintf(cmd, "%s.txt=\"%s\"", obj, txt);
    TJC_SendCmd(cmd);
}

void TJC_SetNumber(const char *obj, int value)
{
    char cmd[64];

    sprintf(cmd, "%s.val=%d", obj, value);
    TJC_SendCmd(cmd);
}

void TJC_UpdatePressure(float pressure_kpa)
{
    char text[32];

    sprintf(text, "%.1f kPa", pressure_kpa);

    TJC_SetText("t0", text);
    TJC_SetNumber("n0", (int)(pressure_kpa * 10));
}

void TJC_AddDataRecord(float pressure_kpa, uint8_t pass)
{
    char text[32];

    sprintf(text, "%.1f kPa", pressure_kpa);
    TJC_SetText("DataLog.pressure", text);

    if (pass)
        TJC_SetText("DataLog.result", "合格");
    else
        TJC_SetText("DataLog.result", "不合格");

    /* TJC 页面内用 va0 触发插入数据记录 */
    TJC_SendCmd("va0.val=1");
}

void TJC_AddAlarm(const char *reason, const char *state)
{
    TJC_SetText("AlarmLog.alarm", reason);
    TJC_SetText("AlarmLog.state", state);

    /* TJC 页面内用 va1 触发插入报警记录 */
    TJC_SendCmd("va1.val=1");
}
