#include "USART2_init.h"
#include "USART1_init.h"
#include "stdarg.h"	
#include "stdio.h"	 	 
#include "string.h"	


//�ڲ��ñ���				
static u8 USART2RxBuffer[USART2_RX_BUFF_SIZEMAX] = {'\0'};	//���ڽ��ջ����� 
static u8 USART2TxBuffer[USART2_TX_BUFF_SIZEMAX] = {'\0'};	//���ڷ��ͻ����� 
static u8 frameValide; //���յ��������Ƿ���������һ֡
static u16 frameCount; //���յ�������֡����
int frameIndex;   //���ջ�����������±�
int rcvClosed;		//�Ƿ���չ�CLOSED�ַ�������ʾsim868�Ͽ�������
/****************************************************************************
* ��	�ƣ�void USART2_GPIO_Init(void)
* ��	�ܣ��������ų�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART2_GPIO_Init(void)			//�������ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ
	
	//�������ŷ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//���ô��� Tx (PA.02) Ϊ�����������
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;					//���ڷ�������
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				//�����������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//Ƶ��50MHz
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						//��ʼ������
    
	// ���ô��� Rx (PA.03) Ϊ��������
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;					//���ڽ�������
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//��������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART2_Init(void)
* ��	�ܣ����ڳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART2_Init(void)
{		
	USART_InitTypeDef UART_InitStructure;		//���ڽṹ

	//���ڷ���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	//���ڳ�ʼ��
	UART_InitStructure.USART_BaudRate            = BAUD_RATE;	//������
	UART_InitStructure.USART_WordLength          = USART_WordLength_8b;		//����λ8bit
	UART_InitStructure.USART_StopBits            = USART_StopBits_1;		//ֹͣλ����
	UART_InitStructure.USART_Parity              = USART_Parity_No ;		//��������żЧ��
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//RTS��CTSʹ��(None��ʹ��)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	//���ͺͽ���ʹ��
	USART_Init(USART2, &UART_InitStructure);	//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART2_NVIC_Init(void)
* ��	�ܣ������ж��������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART2_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//�жϿ���������
	
	/* ѡ�����ȼ�����1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//�򿪴����ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ���ж�������
}

/****************************************************************************
* ��	�ƣ�void USART2_DMATxd_Init(void)
* ��	�ܣ�����DMA��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART2_DMATxd_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//�жϿ���������
	DMA_InitTypeDef DMA_InitStructure;			//DMA�ṹ

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			//ʹ��DMA1ʱ��


	/* ѡ�����ȼ�����1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//DMA�ж���������
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;	//����DMA�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//���ж�
	NVIC_Init(&NVIC_InitStructure); 

	//DMA����
	DMA_DeInit(DMA1_Channel7);  		   									//��λDMA1_Channel4ͨ��ΪĬ��ֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_BASE + 4;				//DMAͨ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART2TxBuffer;				//DMAͨ���洢������ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//DMAĿ�ĵ�	(DMA_DIR_PeripheralSRCԴ)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//��ǰ����Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//��ǰ�洢�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݿ��Ϊ�ֽ�(8λ)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//�洢�����ݿ��Ϊ�ֽ�(8λ)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//��������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;					//DMAͨ�����ȼ��ǳ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMAͨ��δ���ô洢�����洢������
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);							//�����������ó�ʼ��DMA
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);    						//����DMAͨ���ж�
}

/****************************************************************************
* ��	�ƣ�void USART2_RX_Buffer_init(void)
* ��	�ܣ����ڽ��ճ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	���������ж�����ջ�������
****************************************************************************/
void USART2_RX_Buffer_init(void)
{
	frameValide = 0;
	frameCount = 0;										//�ռ������ݸ���
	frameIndex = 0;
}

/****************************************************************************
* ��	�ƣ�void USART2_Config(void)
* ��	�ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵	����Ĭ��Ϊ�����ݽ���					 
****************************************************************************/
void USART2_Config(void)
{
	USART2_Init();				//���ڳ�ʼ��
	USART2_GPIO_Init();			//�������ų�ʼ��
	USART2_NVIC_Init();			//�жϳ�ʼ��
	USART2_DMATxd_Init();		//DMA���ͳ�ʼ��
	USART2_RX_Buffer_init();	//�����ж�����ջ�������

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);				//����ձ�־
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);				//���������ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);				//����IDLE�ж�
	
	USART_Cmd(USART2, ENABLE);  								//ʹ��ʧ�ܴ�������	
}

/****************************************************************************
* ��	�ƣ�u32 LookUSART2_GetBuffMax(void)	
* ��	�ܣ���ȡ���ջ�������С
* ��ڲ�������
* ���ڲ�������
* ˵	������			  
****************************************************************************/
u32 LookUSART2_GetBuffMax(void)
{
	return (u32)USART2_RX_BUFF_SIZEMAX;
}

/****************************************************************************
* ��	�ƣ�u32 LookUSART2_GetBuffCount(void)
* ��	�ܣ���ȡ�����������ݸ���
* ��ڲ�������
* ���ڲ�������
* ˵	������					  
****************************************************************************/
u32 LookUSART2_GetBuffCount(void)
{
	return frameCount-frameIndex;
}

/****************************************************************************
* ��	�ƣ�u8 USART2_GetData(u8* Data,u32 DataLen,u32 TimeLater)	
* ��	�ܣ�����Ч����֡��ʼ����ָ���������µ����ݣ�����������ݸ���
* ��ڲ�����u8* Data ���յ�������
			u32 DataLen		ϣ�����յ����ݸ���
* ���ڲ�����u32 ���յ����ݸ���	0û�н��յ�����
* ˵	������
***************************************************************************/
u32 USART2_GetData(char* Data,u32 DataLen)
{
	u16 count = 0;
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);				//�رս����жϣ���ֹ��ȡ�����е����ݱ仯
	if(frameValide && LookUSART2_GetBuffCount()){
		for(;frameIndex<frameCount && count< DataLen;frameIndex++)
		{
			Data[count] = USART2RxBuffer[frameIndex];
			count++;
		}
	}	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);				//���������ж�
	return count;
}

/****************************************************************************
* ��	�ƣ�void print_usart2_buf(void)
* ��	�ܣ���ӡ����2�Ľ��ջ�����
* ��ڲ�����
* ���ڲ�������
* ˵	������				   
****************************************************************************/
void print_usart2_buf(void){
	int i = 0;
	printf("\r\n");
	if(frameValide && frameCount){
		for(;i<frameCount;i++){
			printf("%02X ",USART2RxBuffer[i]);
		}
	}else{
		printf("usart2 buf null");
	}
	printf("usart2 buf :%d-%d-%d\r\n",frameValide,frameCount,frameIndex);
}

/****************************************************************************
* ��	�ƣ�int has_rcv_closed(void)
* ��	�ܣ���ѯ�Ƿ���չ���CLOSED��,��ѯ�������
* ��ڲ�����
* ���ڲ���: 1 ���չ� 0 δ���չ�
* ˵	������				   
****************************************************************************/
int has_rcv_closed(void){
	if(rcvClosed){
		rcvClosed = 0;
		return 1;
	}else{
		return 0;
	}
}

/****************************************************************************
* ��	�ƣ�void USART2_SendByte(u8 Data)
* ��	�ܣ����ַ�����
* ��ڲ�����Data 	���͵��ַ�����
* ���ڲ�������
* ˵	������				   
****************************************************************************/
void USART2_SendByte(char Data)	
{
	USART_SendData(USART2, Data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/****************************************************************************
* ��	�ƣ�void USART2_SendString(u8* Data,u32 Len)
* ��	�ܣ����ַ����
* ��ڲ�����Data 	����ĵ��ַ�����
			Len		�ַ�����
* ���ڲ�������
* ˵	������					 
****************************************************************************/
void USART2_SendString(char* Data,u32 Len)
{
	u32 i=0;
	for(i=0;i<Len;i++)
    {    
		USART_SendData(USART2, Data[i]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	}
}

/****************************************************************************
* ��	�ƣ�void USART2_printf(u8* Data,u32 Len)
* ��	�ܣ����ַ����
* ��ڲ�����Data 	����ĵ��ַ�����
			Len		�ַ�����
* ���ڲ�������
* ˵	������					 
****************************************************************************/
void USART2_printf(char* fmt,...)  
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2TxBuffer,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2TxBuffer);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2,USART2TxBuffer[j]); 
	} 
}

/****************************************************************************
* ��	�ƣ�void USART2_DMASendString(u8* Data,u32 Len)
* ��	�ܣ�DMA��ʽ���ַ����
* ��ڲ�����Data 	����ĵ��ַ�����
			Len		�ַ�����
* ���ڲ�������
* ˵	����������USART2_DMATxd_Init��ʼ��֮�����ʹ��
			DMA����CPU���� �� ��CPU���� ���з��ͳ�ͻ	 
****************************************************************************/
void USART2_DMASendString(char* Data,u32 Len)		   //���ַ����
{
	memcpy(USART2TxBuffer, Data, Len);			   //�������ݵ����ͻ�����
	DMA1_Channel7->CNDTR = Len;					   //�����ֽ�����
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //����
	DMA_Cmd(DMA1_Channel7, ENABLE);				   //ʼ��DMAͨ��
}

/****************************************************************************
* ��	�ƣ�void USART2_IRQHandler(void)	
* ��	�ܣ��жϻ���
* ��ڲ�������
* ���ڲ�������
* ˵	�������յ������ݴ�����ջ�����
	USART_GetITStatus		���ָ����USART�жϷ������
	USART_GetFlagStatus	���ָ����USART��־λ�������
****************************************************************************/
void USART2_IRQHandler(void)
{
	u8 data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)				// ���ڽ��յ�һ���ַ�
	{
		if(frameValide != 0){   //������һ֡���ݡ�
			frameCount = 0;
			frameValide = 0;
			frameIndex = 0;
		}
		
		data = USART_ReceiveData(USART2);		
		USART1_SendByte(data);
		if(frameCount < USART2_RX_BUFF_SIZEMAX)			//����ռ�δ��,���ձ������� 
		{
			USART2RxBuffer[frameCount]= data;
			frameCount++;								//�ռ������ݸ�������
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);				//��ս����жϱ�־
	}
	else if(USART_GetFlagStatus(USART2,USART_FLAG_IDLE) == SET)   //���յ�������һ֡����
	{					
			data=USART2->SR;
			data=USART2->DR;			
			frameValide = 1;
			frameIndex = 0;
			USART2RxBuffer[frameCount]= '\0';  //��ĩβ��һ��������
		
			if(strstr((char *)USART2RxBuffer,"CLOSED")){
				rcvClosed = 1;
			}else{
				rcvClosed = 0;
			}
	}

	else if(USART_GetFlagStatus(USART2, USART_IT_ORE) == SET)		//����Ƿ��н������
	{
		USART_ReceiveData(USART2);									//�����ݵķ�ʽ���������־
	}
}

/****************************************************************************
* ��	�ƣ�void DMA1_Channel7_IRQHandler(void)	
* ��	�ܣ�DMA�жϻ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC7)) //����������
	{
		USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);    //�ر�DMA����
		DMA_Cmd(DMA1_Channel7, DISABLE);	       			//�ر�DMAͨ��  
	}

	DMA_ClearFlag(DMA1_FLAG_GL7| DMA1_FLAG_TC7 | DMA1_FLAG_HT7 | DMA1_FLAG_TE7);  //���DMA��ر�־
}
