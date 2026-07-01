#define RS485_DE_TX()   GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define RS485_DE_RX()   GPIO_ResetBits(GPIOA, GPIO_Pin_8)

void RS485_Init(uint32_t baud);
void RS485_SendBuf(uint8_t *buf, uint16_t len);
uint8_t RS485_ReadPressure(int16_t *pressure_kpa);