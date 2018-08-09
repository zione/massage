#ifndef _USART2_INIT_H_	
#define _USART2_INIT_H_

#ifdef __cplusplus			//定义对CPP进行C处理
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"	//串口结构体

void USART2_Config(void);		  				//串口配置
void USART2_RX_Buffer_init(void);
void USART2_DMATxd_Init(void);
void USART2_SendByte(char Data);					//单字符数据发送
void USART2_SendString(char* Data,u32 Len);		//多字符发送
void USART2_printf(char* fmt,...);
void USART2_DMASendString(char* Data,u32 Len);	//DMA多字符发送
u32 USART2_GetData(char* Data,u32 DataLen);	//获取多字节数据

u32 LookUSART2_GetBuffMax(void);				//查询接收缓冲区大小
u32 LookUSART2_GetBuffCount(void);				//查询缓冲区中接收到的数据个数
void print_usart2_buf(void);
int has_rcv_closed(void);

#ifdef __cplusplus		   //定义对CPP进行C处理 //结束部分
}
#endif

#endif
