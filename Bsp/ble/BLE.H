#ifndef __BLE_H
#define	__BLE_H

#include "stm32f10x.h"

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1

/* ���κ꣬��������������һ��ʹ�� */
#define BLE(a)	if (a)	\
					GPIO_ResetBits(GPIOA,GPIO_Pin_8);\
					else		\
					GPIO_SetBits(GPIOA,GPIO_Pin_8)



/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)			{p->BRR=i;}				//����͵�ƽ
#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬


/* �������IO�ĺ� */
#define BLE_ON				digitalHi(GPIOA,GPIO_Pin_8)
#define BLE_OFF				digitalLo(GPIOA,GPIO_Pin_8)

#define BEN_ON				digitalLo(GPIOC,GPIO_Pin_2)
#define BEN_OFF				digitalHi(GPIOC,GPIO_Pin_2)

#define RTS3_ON				digitalHi(GPIOB,GPIO_Pin_14)
#define RTS3_OFF			digitalLo(GPIOB,GPIO_Pin_14)


void BLE_GPIO_Config(void);
uint8_t BLE_Init(void);
void BLE_TEST(void);
void Wait_Back(void);
void strclr(char *s);
uint8_t BLE_GetINI(void);
uint8_t Get_M1(void);
uint8_t Get_M2(void);
uint8_t Motor_RUN(void);
uint8_t BLE_Check(void);
uint8_t Type_Get();

#endif /* __LED_H */