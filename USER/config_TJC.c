#include "config_TJC.h"
#include "config_RS232.h"
#include "config_delay.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "config_led.h"
/* printf 鏍煎紡鍖栧彂閫佹椂浣跨敤鐨勪复鏃剁紦鍐插尯闀垮害 */
#define TJC_BUF_SIZE 100
extern uint8_t sig_mode;      // SW1: 0=NO常开  1=NC常闭
extern uint8_t sig_define;    // SW0: 0=X1合格X2不合格  1=X1不合格X2合格

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
功能: 格式化发送屏命令
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

/************************************************
函数: TJC_SetSystemState
功能: 设置系统状态显示
*************************************************/
void TJC_SetSystemState(uint8_t system_ok)
{
    if (system_ok)
    {
        TJCPrintf("main.sys.val=1");
        Delay_ms(200);
        TJCPrintf("main.sys.val=1");
        //TJCPrintf("main.n1.val=1");
        LED1 = 0;
    }
    else
    {
        TJCPrintf("main.sys.val=0");
        Delay_ms(200);
        TJCPrintf("main.sys.val=0");
        //TJCPrintf("main.n1.val=0");
        LED1 = 1;
        //LED2 = 1;
    }
}

/************************************************
函数: TJC_SetProductResult
功能: 设置产品检测结果
说明:
0 = 未检测
1 = 合格
2 = 不合格
*************************************************/
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
说明:
数值返回帧以 0x71 开头，后跟 4 字节小端数据
*************************************************/
uint32_t TJC_ReadNumber(const char *cmd)
{
    uint8_t ch;
    uint8_t buf[4];
    uint8_t index = 0;
    uint8_t ff_count = 0;
    uint32_t value = 0;

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

                value = (uint32_t)buf[0]
                      | ((uint32_t)buf[1] << 8)
                      | ((uint32_t)buf[2] << 16)
                      | ((uint32_t)buf[3] << 24);

                /* 鍏抽敭锛氱户缁?璇绘帀缁撳熬 FF FF FF */
                ff_count = 0;
                while(ff_count < 3)
                {
                    if(RS232_ReadByte(&ch))
                    {
                        if(ch == 0xFF)
                            ff_count++;
                        else
                            ff_count = 0;
                    }
                }

                return value;
            }
        }
    }
}

/************************************************
函数: TJC_GetDateTime
功能: 读取屏幕当前时间字符串
返回:
1 = 读取成功
0 = 读取失败
*************************************************/
uint8_t TJC_GetDateTime(char *datetime)
{
    uint8_t ch;
    uint16_t index = 0;
    uint8_t ff_count = 0;
    uint8_t start = 0;
    uint32_t timeout = 0;

    memset(datetime, 0, 32);

    TJC_SendCmd("click getTime,1");
    Delay_ms(100);

    TJC_SendCmd("get main.date.txt");

    while(timeout < 50000)
    {
        timeout++;

        if(RS232_ReadByte(&ch))
        {
            if(ch == 0x70)
            {
                start = 1;
                index = 0;
                ff_count = 0;
                continue;
            }

            if(start == 0)
                continue;

            if(ch == 0xFF)
            {
                ff_count++;

                if(ff_count >= 3)
                {
                    datetime[index] = 0;
                    return index > 0 ? 1 : 0;
                }
            }
            else
            {
                ff_count = 0;

                if(index < 31)
                {
                    datetime[index++] = ch;
                }
            }
        }
    }

    return 0;
}

/************************************************
函数: TJC_AddDataLog
功能: 添加检测日志
*************************************************/
void TJC_AddDataLog(uint16_t pressure, char *result)
{
    char datetime[32];
    char cmd[200];
    char id[20];

    /* 1. 鍏堣?诲彇灞忓綋鍓嶆椂闂? */
    if(TJC_GetDateTime(datetime) == 0)
    {
        strcpy(datetime, "0000/00/00 00:00:00");
    }

     /* 鐢熸垚鏃堕棿鎴矷D */
    sprintf(id,
            "%.2s%.2s%.2s%.2s%.2s%.2s",
            &datetime[2],   /* 骞村悗涓や綅 */
            &datetime[5],   /* 鏈? */
            &datetime[8],   /* 鏃? */
            &datetime[11],  /* 鏃? */
            &datetime[14],  /* 鍒? */
            &datetime[17]); /* 绉? */

    /* 鎻掑叆璁板綍 */
    sprintf(cmd,
            "DataLog.data0.insert(\"%s^%s^%u^%s\")",
            id,
            datetime,
            pressure,
            result);

    TJC_SendCmd(cmd);
}
/************************************************
函数: TJC_AddAlarmLog
功能: 添加报警日志
*************************************************/
  void TJC_AddAlarmLog(char *reason,char *status)
{
    char datetime[32];
    char cmd[200];
    char id[20];

    /* 鑾峰彇鏃堕棿 */
    if(TJC_GetDateTime(datetime) == 0)
    {
        strcpy(datetime,"0000/00/00 00:00:00");
    }

    /* 鏃堕棿鎴矷D */
    sprintf(id,
            "%.2s%.2s%.2s%.2s%.2s%.2s",
            &datetime[2],
            &datetime[5],
            &datetime[8],
            &datetime[11],
            &datetime[14],
            &datetime[17]);

    /* 鎻掑叆鎶ヨ?﹁?板綍 */
    sprintf(cmd,
            "AlarmLog.data0.insert(\"%s^%s^%s^%s\")",
            id,
            datetime,
            reason,
            status);

    TJC_SendCmd(cmd);
}  

/************************************************
函数: TJC_UpdateSigLogicCfg
功能: 更新信号逻辑配置页面
说明:
sw0 显示信号定义
sw1 显示 NO/NC 模式
t0  显示当前模式文字
*************************************************/
void TJC_UpdateSigLogicCfg(void)
{
    TJCPrintf("SigLogicCfg.sw0.val=%d", sig_define);
    TJCPrintf("SigLogicCfg.sw1.val=%d", sig_mode);

    if(sig_mode == 0)
    {
        TJCPrintf("SigLogicCfg.t0.txt=\"常开（NO）\"");
    }
    else
    {
        TJCPrintf("SigLogicCfg.t0.txt=\"常闭（NC）\"");
    }
}

/************************************************
函数: TJC_ReceiveTask
功能: 接收并解析屏幕发送的二进制协议
协议:
AA 55 01          读取当前配置
AA 55 02 SW0 SW1  保存当前配置
*************************************************/
void TJC_ReceiveTask(void)
{
    static uint8_t step = 0;
    static uint8_t cmd = 0;
    static uint8_t data1 = 0;
    static uint8_t data2 = 0;
    uint8_t ch;

    while(RS232_ReadByte(&ch))
    {
        switch(step)
        {
            case 0:
                if(ch == 0xAA) step = 1;
                break;

            case 1:
                if(ch == 0x55) step = 2;
                else step = 0;
                break;

            case 2:
                cmd = ch;

                if(cmd == 0x01)
                {
                    TJC_UpdateSigLogicCfg();
                    step = 0;
                }
                else if(cmd == 0x02)
                {
                    step = 3;
                }
                else
                {
                    step = 0;
                }
                break;

            case 3:
                data1 = ch;
                step = 4;
                break;

            case 4:
                data2 = ch;

                sig_define = data1;
                sig_mode   = data2;

                if(sig_define > 1) sig_define = 0;
                if(sig_mode > 1) sig_mode = 0;

                TJC_UpdateSigLogicCfg();

                TJCPrintf("SigLogicCfg.t1.txt=\"配置已保存\"");
                TJCPrintf("vis SigLogicCfg.t1,1");


                step = 0;
                break;

            default:
                step = 0;
                break;
        }
    }
}


