#ifndef _USART2_INIT_H_	
#define _USART2_INIT_H_

#ifdef __cplusplus			//�����CPP����C����
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"	//���ڽṹ��

void USART2_Config(void);		  				//��������
void USART2_RX_Buffer_init(void);
void USART2_DMATxd_Init(void);
void USART2_SendByte(char Data);					//���ַ����ݷ���
void USART2_SendString(char* Data,u32 Len);		//���ַ�����
void USART2_printf(char* fmt,...);
void USART2_DMASendString(char* Data,u32 Len);	//DMA���ַ�����
u32 USART2_GetData(char* Data,u32 DataLen);	//��ȡ���ֽ�����

u32 LookUSART2_GetBuffMax(void);				//��ѯ���ջ�������С
u32 LookUSART2_GetBuffCount(void);				//��ѯ�������н��յ������ݸ���
void print_usart2_buf(void);
int has_rcv_closed(void);

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
