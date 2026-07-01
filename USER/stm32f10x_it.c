#include "stm32f10x_it.h"

/*
 * 中断模板：
 * USART1_IRQHandler 可用于接收控制板协议或 TJC 回传事件。
 * 如果使用 485 采集压力，建议使用 DMA + IDLE 中断。
 */
