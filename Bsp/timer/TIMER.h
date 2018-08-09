
#ifndef __TIMER_H
#define	__TIMER_H

#include "stm32f10x.h"

#define MAX_PING_COUNTER 20
void RCC_Configuration(void);
void NVIC_Configuration(void);
void TIM2_Configuration(void);

int isPingCountEnd(void);
void resetPingCount(void);

void setSwitchTime(int value);


#endif /* __LED_H */

