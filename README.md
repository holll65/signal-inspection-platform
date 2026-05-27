# 5.压力显示_TJC_X5

## 项目说明

本工程模板用于：

- STM32F103RCT6
- 485采集压力传感器
- RS232连接陶晶驰 TJC X5 串口屏
- 显示压力、状态
- 触发数据记录和报警记录

## 文件结构

```text
5.压力显示_TJC_X5/
├── .vscode/
│   ├── c_cpp_properties.json
│   ├── settings.json
│   ├── tasks.json
│   └── launch.json
├── platformio.ini
├── Makefile
├── CMSIS/
├── FWlib/
└── USER/
    ├── main.c
    ├── config_RS232.c/h
    ├── config_rs485.c/h
    ├── config_TJC.c/h
    ├── config_protocol.c/h
    ├── config_delay.c/h
    └── stm32f10x_it.c/h
```

## 硬件连接

| STM32引脚 | 连接 |
|---|---|
| PA9 USART1_TX | RS485/RS232发送 |
| PA10 USART1_RX | RS485/RS232接收 |
| PA8 | RS485 DE/RE方向控制 |
| PC10 UART4_TX | 可扩展给TJC独立串口 |
| PC11 UART4_RX | 可扩展给TJC独立串口 |

## 推荐方式

### PlatformIO

1. VSCode 安装 PlatformIO IDE 插件
2. 打开本文件夹
3. 等待识别 `platformio.ini`
4. 点击底部编译/下载

### Makefile

需要 ARM GCC：

```bash
make
make clean
```

## 注意

- 当前模板保留了你给出的原例程结构。
- `config_RS232.c` 是 USART1 PA9/PA10。
- `config_rs485.c` 用于压力采集，内部已放 Modbus CRC 框架。
- `config_TJC.c` 用于陶晶驰串口屏命令发送。
- 实际项目需要根据压力传感器寄存器地址修改 `RS485_ReadPressure()`。
git提交
git add .
git commit -m "主控板初始化"
git push
git将代码同步到本地
git pull
