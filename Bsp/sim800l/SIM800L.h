#include "stm32f10x.h"

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1

/* 带参宏，可以像内联函数一样使用 */

/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//设置为高电平		
#define digitalLo(p,i)			{p->BRR=i;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态

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
