#ifndef _USART1_INIT_H_	
#define _USART1_INIT_H_

#ifdef __cplusplus			//�����CPP����C����
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"	//���ڽṹ��

void USART1_Config(void);		  				//��������
void USART1_RX_Buffer_init(void);
void USART1_DMATxd_Init(void);
void USART1_SendByte(u8 Data);					//���ַ����ݷ���
void USART1_SendString(u8* Data,u32 Len);		//���ַ�����
void USART1_DMASendString(u8* Data,u32 Len);	//DMA���ַ�����
u32 USART1_GetData(u8* Data,u32 DataLen);	//��ȡ���ֽ�����

u32 LookUSART1_GetBuffMax(void);				//��ѯ���ջ�������С
u32 LookUSART1_GetBuffCount(void);				//��ѯ�������н��յ������ݸ���


#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
