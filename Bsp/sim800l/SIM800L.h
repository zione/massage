#include "stm32f10x.h"

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1

/* ���κ꣬��������������һ��ʹ�� */

/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)			{p->BRR=i;}				//����͵�ƽ
#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬

#define IGT_Hi				digitalHi(GPIOB,GPIO_Pin_4)
#define GPSEN_Hi				digitalHi(GPIOB,GPIO_Pin_8)

#define IGT_Lo				digitalLo(GPIOB,GPIO_Pin_4)
#define GPSEN_Lo				digitalLo(GPIOB,GPIO_Pin_8)

void Sim_ini(void);
u8 sim800c_send_cmd(char *cmd,char *ack,u16 waittime);
uint8_t sim800c_send_data(char *data,u8 lenth);
uint8_t Start_Gprs_TCP(void);
uint8_t wait_reg(void);
uint8_t connect_server(void);
