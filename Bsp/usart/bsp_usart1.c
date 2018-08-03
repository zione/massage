#include "USART1_init.h"

#define BAUD_RATE 115200  //������

//���ջ���������
#define USART1_RX_BUFF_SIZEMAX 256
#define USART1_TX_BUFF_SIZEMAX 256

//�ڲ��ñ���
static u8 USART1_GetChar=0;							//���յ��ĵ����ַ�				�ж���ʹ��					
static u8 USART1RxBuffer[USART1_RX_BUFF_SIZEMAX];	//���ڽ��ջ����� 
static u8 USART1TxBuffer[USART1_TX_BUFF_SIZEMAX];	//���ڷ��ͻ����� 
static u8 rxValide; //���յ��������Ƿ���������һ֡
static u16 rxCount; //���յ������ݳ���

/****************************************************************************
* ��	�ƣ�void USART1_GPIO_Init(void)
* ��	�ܣ��������ų�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART1_GPIO_Init(void)						//�������ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ
	
	//�������ŷ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//���ô��� Tx (PA.09) Ϊ�����������
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					//���ڷ�������
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				//�����������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//Ƶ��50MHz
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						//��ʼ������
    
	// ���ô��� Rx (PA.10) Ϊ��������
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;					//���ڽ�������
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//��������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART1_Init(void)
* ��	�ܣ����ڳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART1_Init(void)
{		
	USART_InitTypeDef UART_InitStructure;		//���ڽṹ

	//���ڷ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//���ڳ�ʼ��
	UART_InitStructure.USART_BaudRate            = BAUD_RATE;			//������
	UART_InitStructure.USART_WordLength          = USART_WordLength_8b;				//����λ8bit
	UART_InitStructure.USART_StopBits            = USART_StopBits_1;				//ֹͣλ����
	UART_InitStructure.USART_Parity              = USART_Parity_No ;				//��������żЧ��
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//RTS��CTSʹ��(None��ʹ��)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	//���ͺͽ���ʹ��
	USART_Init(USART1, &UART_InitStructure);										//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART1_NVIC_Init(void)
* ��	�ܣ������ж��������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART1_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 						//�жϿ���������
	
	/* ѡ�����ȼ�����1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//�򿪴����ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ���ж�������
}

/****************************************************************************
* ��	�ƣ�void USART1_DMATxd_Init(void)
* ��	�ܣ�����DMA��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART1_DMATxd_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 						//�жϿ���������
	DMA_InitTypeDef DMA_InitStructure;							//DMA�ṹ

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			//ʹ��DMA1ʱ��

	/* ѡ�����ȼ�����1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//DMA�ж���������
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;	//����DMA�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//���ж�
	NVIC_Init(&NVIC_InitStructure); 

	//DMA����
	DMA_DeInit(DMA1_Channel4);  		   									//��λDMA1_Channel4ͨ��ΪĬ��ֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_BASE + 4;				//DMAͨ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART1TxBuffer;				//DMAͨ���洢������ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//DMAĿ�ĵ�	(DMA_DIR_PeripheralSRCԴ)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//��ǰ����Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//��ǰ�洢�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݿ��Ϊ�ֽ�(8λ)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//�洢�����ݿ��Ϊ�ֽ�(8λ)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//��������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;					//DMAͨ�����ȼ��ǳ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMAͨ��δ���ô洢�����洢������
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);							//�����������ó�ʼ��DMA
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);    						//����DMAͨ���ж�
}

/****************************************************************************
* ��	�ƣ�void USART1_RX_Buffer_init(void)
* ��	�ܣ����ڽ��ճ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	���������ж�����ջ�������
****************************************************************************/
void USART1_RX_Buffer_init(void)
{
	uint16_t i;
	rxValide = 0;
	rxCount = 0;										//�ռ������ݸ���
	for(i=0;i<USART1_RX_BUFF_SIZEMAX;i++)
	{
		USART1RxBuffer[i] = 0x00;
	}
}

/****************************************************************************
* ��	�ƣ�void USART1_Config(void)
* ��	�ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵	����Ĭ��Ϊ�����ݽ���					 
****************************************************************************/
void USART1_Config(void)
{
	USART1_Init();				//���ڳ�ʼ��
	USART1_GPIO_Init();			//�������ų�ʼ��
	USART1_NVIC_Init();			//�жϳ�ʼ��
//	USART1_DMATxd_Init();		//DMA���ͳ�ʼ��
	USART1_RX_Buffer_init();	//�����ж�����ջ�������

	USART_ClearITPendingBit(USART1, USART_IT_RXNE);				//����ձ�־
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				//���������ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);				//����IDLE�ж�
	
	USART_Cmd(USART1, ENABLE);  								//ʹ��ʧ�ܴ�������	
}

/****************************************************************************
* ��	�ƣ�u32 LookUSART1_GetBuffMax(void)	
* ��	�ܣ���ȡ���ջ�������С
* ��ڲ�������
* ���ڲ�������
* ˵	������			  
****************************************************************************/
u32 LookUSART1_GetBuffMax(void)
{
	return (u32)USART1_RX_BUFF_SIZEMAX;
}

/****************************************************************************
* ��	�ƣ�u32 LookUSART1_GetBuffCount(void)
* ��	�ܣ���ȡ�������н��յ������ݸ���
* ��ڲ�������
* ���ڲ�������
* ˵	������					  
****************************************************************************/
u32 LookUSART1_GetBuffCount(void)
{
	return rxCount;
}

/****************************************************************************
* ��	�ƣ�u8 USART1_WaitTime_GetString(u8* Data,u32 DataLen,u32 TimeLater)	
* ��	�ܣ�����ʱ֮��ʼ����ָ���������µ�����
* ��ڲ�����u8* Data ���յ�������
			u32 DataLen		ϣ�����յ����ݸ���
			u32 TimeLater	���յȴ�ʱ�� 7=1us
* ���ڲ�����u32 ���յ����ݸ���	0û�н��յ�����
* ˵	������
***************************************************************************/
u32 USART1_GetData(u8* Data,u32 DataLen)
{
	u16 i;
	u16 count = 0;
	
	if(rxValide && rxCount){
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);				//�رս����жϣ���ֹ��ȡ�����е����ݱ仯
		for(i=0;i<rxCount && i< DataLen;i++)
		{
			Data[i] = USART1RxBuffer[i];
		}
		count = rxCount;
		rxCount = 0;
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				//���������ж�
	}	

	return count;
}

/****************************************************************************
* ��	�ƣ�void USART1_SendByte(u8 Data)
* ��	�ܣ����ַ�����
* ��ڲ�����Data 	���͵��ַ�����
* ���ڲ�������
* ˵	������				   
****************************************************************************/
void USART1_SendByte(u8 Data)		   //���ַ��������
{
	USART_SendData(USART1, Data);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/****************************************************************************
* ��	�ƣ�void USART1_SendString(u8* Data,u32 Len)
* ��	�ܣ����ַ����
* ��ڲ�����Data 	����ĵ��ַ�����
			Len		�ַ�����
* ���ڲ�������
* ˵	������					 
****************************************************************************/
void USART1_SendString(u8* Data,u32 Len)		   //���ַ����
{
	u32 i=0;
	for(i=0;i<Len;i++)
    {    
		USART_SendData(USART1, Data[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}

/****************************************************************************
* ��	�ƣ�void USART1_DMASendString(u8* Data,u32 Len)
* ��	�ܣ�DMA��ʽ���ַ����
* ��ڲ�����Data 	����ĵ��ַ�����
			Len		�ַ�����
* ���ڲ�������
* ˵	����������USART1_DMATxd_Init��ʼ��֮�����ʹ��
			DMA����CPU���� �� ��CPU���� ���з��ͳ�ͻ	 
****************************************************************************/
void USART1_DMASendString(u8* Data,u32 Len)		   //���ַ����
{
	memcpy(USART1TxBuffer, Data, Len);			   //�������ݵ����ͻ�����
	DMA1_Channel4->CNDTR = Len;					   //�����ֽ�����
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //����
	DMA_Cmd(DMA1_Channel4, ENABLE);				   //ʼ��DMAͨ��
}

/****************************************************************************
* ��	�ƣ�void USART1_IRQHandler(void)	
* ��	�ܣ��жϻ���
* ��ڲ�������
* ���ڲ�������
* ˵	�������յ������ݴ�����ջ�����
	 USART_GetITStatus		���ָ����USART�жϷ������
	 USART_GetFlagStatus	���ָ����USART��־λ�������
****************************************************************************/
void USART1_IRQHandler(void)
{
	u8 clear=clear;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)				// ���ڽ������ݴ����ж�
	{
		if(rxValide != 0){
			rxCount = 0;
			rxValide = 0;
		}
		
		if(rxCount < USART1_RX_BUFF_SIZEMAX)			//����ռ�δ��,���ձ������� 
		{
			USART1_GetChar = USART_ReceiveData(USART1);					//���յ����ַ�����
			USART1RxBuffer[rxCount]= USART1_GetChar;
			rxCount++;								//�ռ������ݸ�������
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);				//��ս����жϱ�־
	}
	else if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE) == SET)   //���յ�������һ֡����
	{					
			clear=USART1->SR;
			clear=USART1->DR;			
			rxValide = 1;
	}

	else if(USART_GetFlagStatus(USART1, USART_IT_ORE) == SET)		//����Ƿ��н������
	{
		USART_ReceiveData(USART1);									//����������־��ֻ���ö����ݵķ�ʽ���������־
	}

	else if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)				//���ڷ������ݴ����ж�
	{
		;
	}
}

/****************************************************************************
* ��	�ƣ�void DMA1_Channel4_IRQHandler(void)	
* ��	�ܣ�DMA�жϻ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC4)) //����������
	{
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);    //�ر�DMA����
		DMA_Cmd(DMA1_Channel4, DISABLE);	       			//�ر�DMAͨ��  
	}

	DMA_ClearFlag(DMA1_FLAG_GL4| DMA1_FLAG_TC4 | DMA1_FLAG_HT4 | DMA1_FLAG_TE4);  //���DMA��ر�־
}
