#ifndef __CONFIG_RELAY_H
#define __CONFIG_RELAY_H

#include "sys.h"

#define relay1 PCout(13)
#define relay2 PDout(2)
#define relay3 PCout(12)
#define relay4 PAout(15)

void GPIO_relay_Config(void);

#endif