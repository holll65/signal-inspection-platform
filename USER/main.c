#include "stm32f10x.h"
#include "config_relay.h"
#include "config_delay.h"
#include "config_opt.h"
#include "config_mos.h"
#include "config_TJC.h"
#include <stdlib.h>
#include "config_protocol.h"
#include "config_rs485.h"
#include "config_led.h"
/************************************************
* 压力范围定义
* 压力单位：kPa
************************************************/
#define PRESSURE_MIN   0
#define PRESSURE_MAX   630
#define PRESSURE_INIT  300

uint8_t sig_mode = 0;     // SW1: 0=NO常开  1=NC常闭
uint8_t sig_define = 0;   // SW0: 0=X1合格X2不合格  1=X1不合格X2合格
uint8_t pass_signal = 0;
uint8_t fail_signal = 0;

uint8_t Signal_IsActive(uint8_t x)
{
    if(sig_mode == 0)
    {
        // NO常开：闭合有效，当前你的输入闭合是0
        return (x == 0);
    }
    else
    {
        // NC常闭：断开有效
        return (x == 1);
    }
}
/************************************************
Function: TJC_SendPressure
Purpose : 发送压力值到陶晶驰串口屏
          n0显示当前压力
          s0曲线实时显示压力变化
Input   : pressure_value 当前压力值
*************************************************/
void TJC_SendPressure(int16_t pressure_value)
{
    uint16_t curve_value;

    /* 数字显示当前压力 */
    TJCPrintf("main.t3.txt=\"%d\"", pressure_value);
    /* 转换为曲线坐标值
       曲线高度125像素
       压力范围0~630kPa */
    /* 负数不画到曲线下面 */
    if(pressure_value < 0)
    {
        curve_value = 0;
    }
    else if(pressure_value > 630)
    {
        curve_value = 125;
    }
    else
    {
        curve_value = pressure_value * 125 / 630;
    }

    /* 添加一个曲线点 */
    TJCPrintf("add s0.id,0,%d", curve_value);
}

int main(void)
{
    /* 系统状态
       0 = 未就绪
       1 = 已就绪 */
    uint8_t system_ok = 0;
   
    /* 上一次系统状态
       用于避免重复刷新屏幕 */
    uint8_t last_system_ok = 255;

    /* 产品检测结果
       0 = 未检测
       1 = 合格
       2 = 不合格 */
    uint8_t product_result = 0;

    /* 上一次检测结果 */
    uint8_t last_product_result = 255;

    /* 当前压力值 */
    int16_t pressure_value = 0;
   

    /* 压力持续计时(ms) */
    uint16_t pressure_time = 0;

    /* 压力发送计时(ms) */
    uint16_t pressure_send_time = 0;

    /* 日志写入标志
       防止一次检测重复记录 */
    uint8_t log_written = 0;
    /// RS485通讯异常标志
    uint8_t X3_AlarmFlag = 0;
    uint16_t power_on_time = 0;       // 开机计时
    uint8_t power_on_alarm_enable = 0; // 0=开机屏蔽报警 1=允许报警记录
    uint8_t device_in;
    uint8_t pass_in;
    uint8_t result_latch = 0;  // 检测结果锁存标志
    static uint16_t heart = 0;
    static uint16_t heart_time = 0;

    // uint16_t sig_cfg_time = 0;
    // uint32_t save_cfg = 0;
    /****************************************
     * 硬件初始化
     ****************************************/
    Delay_Init();          // 延时模块
    GPIO_relay_Config();   // 继电器输出
    GPIO_MOS_Config();     // MOS输出
    GPIO_OPT_Config();     // 光耦输入

    TJC_Init(9600);        // 串口屏初始化

    GPIO_LED_Config();     // LED初始化
    /* RS485初始化 */
   
    RS485_Init(9600);
    srand(1234);

    /* 上电初始化界面 */
    TJC_SetSystemState(0);
    TJC_SetProductResult(0);
   
   
    Delay_ms(500);

     
    while(1)
    {
        /* 开机前5秒不记录压力仪表通讯故障 */
        if(power_on_alarm_enable == 0)
        {
            if(power_on_time < 5000)
            {
                power_on_time += 50;
            }
            else
            {
               power_on_alarm_enable = 1;
            }
        }
        TJC_ReceiveTask();
        /****************************************
         * 1. 每秒刷新一次压力值
         ****************************************/
        pressure_send_time += 50;

        if(pressure_send_time >= 1000)
        {
            pressure_send_time = 0;

            /* 读取压力 */
            pressure_value = Pressure_GetValue();

            /* 发送到串口屏 */
            TJC_SendPressure(pressure_value);

            // TJCPrintf("add s0.id,0,62");
        }
        
      
        heart_time += 50;

        if(heart_time >= 1000)
        {
            heart_time = 0;

            heart++;

            TJCPrintf("heart.val=%u", heart);
        }

        

        /****************************************
         * 3. 系统状态变化时更新屏幕
         ****************************************/
        if(system_ok != last_system_ok)
        {
            TJC_SetSystemState(system_ok);
            last_system_ok = system_ok;
        }
       /****************************************
         * 信号配置页面处理
         * 500ms读取一次saveCfg
         ****************************************/
        /****************************************
         * 4. 状态机逻辑（X1/X2 + 红绿灯）
         * relay1 = 绿灯
         * relay2 = 红灯
         ****************************************/

        

        /* 信号定义切换 */
        if(sig_define == 0)
        {
            device_in = Signal_IsActive(X1);
            pass_in   = Signal_IsActive(X2);
        }
        else
        {
            device_in = Signal_IsActive(X2);
            pass_in   = Signal_IsActive(X1);
        }

        /****************************************
         * 状态1：未放置
         ****************************************/
        if(device_in == 0 && pass_in == 0)
        {
            system_ok = 0;

            relay1 = 0;   // 绿灯灭
            relay2 = 0;   // 红灯灭

            product_result = 0;
        }

        /****************************************
         * 状态2：已放置
         ****************************************/
        else if(device_in == 1 && pass_in == 0)
        {
            system_ok = 1;

            relay1 = 0;   // 绿灯灭
            relay2 = 1;   // 红灯亮

            product_result = 0;
        }

        /****************************************
         * 状态3：合格
         ****************************************/
        else if(pass_in == 1)
        {
            system_ok = 1;

            relay1 = 1;   // 绿灯亮
            relay2 = 0;   // 红灯灭

            product_result = 1;
        }

        /****************************************
         * 异常状态（防止两路同时异常）
         ****************************************/
        else
        {
            relay1 = 0;
            relay2 = 0;

            system_ok = 0;
            product_result = 0;
        }
       
           
        
       
        /****************************************
         * 4. 系统就绪后开始判定
         ****************************************/
        // if(system_ok == 1)
        // {
        //     /* X1触发 → 合格 */
        //     if(X1 == 0)
        //     {
        //         relay1 = 1;
        //         relay2 = 0;

        //         product_result = 1;
        //     }
        //     /* X2触发 → 不合格 */
        //     else if(X2 == 0)
        //     {
        //         relay1 = 0;
        //         relay2 = 1;

        //         product_result = 2;
        //     }
        //     else
        //     {
        //         relay1 = 0;
        //         relay2 = 0;

        //         product_result = 0;
        //     }
        // }
        // else
        // {
        //     product_result = 0;
        // }

        /****************************************
         * 5. 检测结果变化时更新屏幕
         ****************************************/
        if(product_result != last_product_result)
        {
            TJC_SetProductResult(product_result);
            last_product_result = product_result;
        }

       /****************************************
         * 6. 检测日志（只记录一次）
         ****************************************/

        // 进入合格或不合格时触发一次
        if(product_result == 1 && result_latch == 0)
        {
            TJC_AddDataLog(pressure_value, "合格");
            result_latch = 1;
        }
        else if(product_result == 2 && result_latch == 0)
        {
            TJC_AddDataLog(pressure_value, "不合格");
            result_latch = 1;
        }

        /****************************************
         * 7. 复位条件（新一轮开始）
         ****************************************/

        // 只有设备完全回到“未放置”才允许下一次记录
        if(system_ok == 0)
        {
            result_latch = 0;
        }
      /****************************************
     * 8. RS485压力仪表通讯状态监测
     ****************************************/
    if(Pressure_CommOK() == 0)
    {
        if(X3_AlarmFlag == 0)
        {
            X3_AlarmFlag = 1;
        }

        if(power_on_alarm_enable == 1 && X3_AlarmFlag == 1)
        {
            TJC_AddAlarmLog("压力仪表通讯故障","故障");

            // 防止重复写
            X3_AlarmFlag = 2;
        }
    }
    else
    {
        if(X3_AlarmFlag != 0)
        {
            if(power_on_alarm_enable == 1)
            {
                TJC_AddAlarmLog("压力仪表通讯故障","复位");
            }

            X3_AlarmFlag = 0;
        }
    }
        /* 主循环周期50ms */
        Delay_ms(50);
    }
}