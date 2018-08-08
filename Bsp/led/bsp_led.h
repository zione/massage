#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)			{p->BRR=i;}				//����͵�ƽ
#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬


/* �������IO�ĺ� */

#define LED_TOGGLE		digitalToggle(GPIOB,GPIO_Pin_9)
#define LED_OFF				digitalHi(GPIOB,GPIO_Pin_9)
#define LED_ON				digitalLo(GPIOB,GPIO_Pin_9)

void LED_GPIO_Config(void);
int getLedStatus(void);

#endif /* __LED_H */
