#ifndef _USART1_INIT_H_	
#define _USART1_INIT_H_

#ifdef __cplusplus			//定义对CPP进行C处理
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"	//串口结构体

void USART1_Config(void);		  				//串口配置
void USART1_RX_Buffer_init(void);
void USART1_DMATxd_Init(void);
void USART1_SendByte(u8 Data);					//单字符数据发送
void USART1_SendString(u8* Data,u32 Len);		//多字符发送
void USART1_DMASendString(u8* Data,u32 Len);	//DMA多字符发送
u32 USART1_GetData(u8* Data,u32 DataLen);	//获取多字节数据

u32 LookUSART1_GetBuffMax(void);				//查询接收缓冲区大小
u32 LookUSART1_GetBuffCount(void);				//查询缓冲区中接收到的数据个数


#ifdef __cplusplus		   //定义对CPP进行C处理 //结束部分
}
#endif

#endif
