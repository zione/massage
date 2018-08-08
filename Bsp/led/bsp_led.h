#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//设置为高电平		
#define digitalLo(p,i)			{p->BRR=i;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


/* 定义控制IO的宏 */

#define LED_TOGGLE		digitalToggle(GPIOB,GPIO_Pin_9)
#define LED_OFF				digitalHi(GPIOB,GPIO_Pin_9)
#define LED_ON				digitalLo(GPIOB,GPIO_Pin_9)

void LED_GPIO_Config(void);
int getLedStatus(void);

#endif /* __LED_H */
