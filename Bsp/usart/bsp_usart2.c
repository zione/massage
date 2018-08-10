#include "USART2_init.h"
#include "USART1_init.h"
#include "stdarg.h"	
#include "stdio.h"	 	 
#include "string.h"	


//内部用变量				
static u8 USART2RxBuffer[USART2_RX_BUFF_SIZEMAX] = {'\0'};	//串口接收缓存区 
static u8 USART2TxBuffer[USART2_TX_BUFF_SIZEMAX] = {'\0'};	//串口发送缓存区 
static u8 frameValide; //接收到的数据是否是完整的一帧
static u16 frameCount; //接收到的数据帧长度
int frameIndex;   //接收缓存区的输出下标
int rcvClosed;		//是否接收过CLOSED字符串，表示sim868断开了连接
/****************************************************************************
* 名	称：void USART2_GPIO_Init(void)
* 功	能：串口引脚初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void USART2_GPIO_Init(void)			//串口引脚初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;		//串口引脚结构
	
	//串口引脚分配时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//配置串口 Tx (PA.02) 为复用推挽输出
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;					//串口发送引脚
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				//复用推挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//频率50MHz
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						//初始化引脚
    
	// 配置串口 Rx (PA.03) 为浮空输入
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;					//串口接收引脚
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						//初始化引脚
}

/****************************************************************************
* 名	称：void USART2_Init(void)
* 功	能：串口初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void USART2_Init(void)
{		
	USART_InitTypeDef UART_InitStructure;		//串口结构

	//串口分配时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	//串口初始化
	UART_InitStructure.USART_BaudRate            = BAUD_RATE;	//波特率
	UART_InitStructure.USART_WordLength          = USART_WordLength_8b;		//数据位8bit
	UART_InitStructure.USART_StopBits            = USART_StopBits_1;		//停止位个数
	UART_InitStructure.USART_Parity              = USART_Parity_No ;		//不进行奇偶效验
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//RTS和CTS使能(None不使用)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	//发送和接收使能
	USART_Init(USART2, &UART_InitStructure);	//初始化串口
}

/****************************************************************************
* 名	称：void USART2_NVIC_Init(void)
* 功	能：串口中断向量表初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void USART2_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//中断控制器变量
	
	/* 选择优先级分组1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//设置中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//主优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//设置优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开串口中断
	NVIC_Init(&NVIC_InitStructure);								//初始化中断向量表
}

/****************************************************************************
* 名	称：void USART2_DMATxd_Init(void)
* 功	能：串口DMA初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void USART2_DMATxd_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//中断控制器变量
	DMA_InitTypeDef DMA_InitStructure;			//DMA结构

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			//使能DMA1时钟


	/* 选择优先级分组1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//DMA中断向量配置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;	//设置DMA中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//主优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//设置优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开中断
	NVIC_Init(&NVIC_InitStructure); 

	//DMA配置
	DMA_DeInit(DMA1_Channel7);  		   									//复位DMA1_Channel4通道为默认值
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_BASE + 4;				//DMA通道外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART2TxBuffer;				//DMA通道存储器基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//DMA目的地	(DMA_DIR_PeripheralSRC源)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//当前外设寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//当前存储寄存器增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为字节(8位)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//存储器数据宽度为字节(8位)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//正常缓冲模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;					//DMA通道优先级非常高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMA通道未配置存储器到存储器传输
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);							//根据上诉设置初始化DMA
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);    						//开启DMA通道中断
}

/****************************************************************************
* 名	称：void USART2_RX_Buffer_init(void)
* 功	能：串口接收初始化
* 入口参数：无
* 出口参数：无
* 说	明：接收中断与接收缓冲区绑定
****************************************************************************/
void USART2_RX_Buffer_init(void)
{
	frameValide = 0;
	frameCount = 0;										//空间里数据个数
	frameIndex = 0;
}

/****************************************************************************
* 名	称：void USART2_Config(void)
* 功	能：串口设置
* 入口参数：无
* 出口参数：无
* 说	明：默认为包数据接收					 
****************************************************************************/
void USART2_Config(void)
{
	USART2_Init();				//串口初始化
	USART2_GPIO_Init();			//串口引脚初始化
	USART2_NVIC_Init();			//中断初始化
	USART2_DMATxd_Init();		//DMA发送初始化
	USART2_RX_Buffer_init();	//接收中断与接收缓冲区绑定

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);				//清接收标志
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);				//开启接收中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);				//开启IDLE中断
	
	USART_Cmd(USART2, ENABLE);  								//使能失能串口外设	
}

/****************************************************************************
* 名	称：u32 LookUSART2_GetBuffMax(void)	
* 功	能：获取接收缓冲区大小
* 入口参数：无
* 出口参数：无
* 说	明：无			  
****************************************************************************/
u32 LookUSART2_GetBuffMax(void)
{
	return (u32)USART2_RX_BUFF_SIZEMAX;
}

/****************************************************************************
* 名	称：u32 LookUSART2_GetBuffCount(void)
* 功	能：获取缓冲区中数据个数
* 入口参数：无
* 出口参数：无
* 说	明：无					  
****************************************************************************/
u32 LookUSART2_GetBuffCount(void)
{
	return frameCount-frameIndex;
}

/****************************************************************************
* 名	称：u8 USART2_GetData(u8* Data,u32 DataLen,u32 TimeLater)	
* 功	能：有有效数据帧后开始接收指定个数以下的数据，最后重置数据个数
* 入口参数：u8* Data 接收到的数据
			u32 DataLen		希望接收的数据个数
* 出口参数：u32 接收到数据个数	0没有接收到数据
* 说	明：无
***************************************************************************/
u32 USART2_GetData(char* Data,u32 DataLen)
{
	u16 count = 0;
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);				//关闭接收中断，防止读取过程中的数据变化
	if(frameValide && LookUSART2_GetBuffCount()){
		for(;frameIndex<frameCount && count< DataLen;frameIndex++)
		{
			Data[count] = USART2RxBuffer[frameIndex];
			count++;
		}
	}	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);				//开启接收中断
	return count;
}

/****************************************************************************
* 名	称：void print_usart2_buf(void)
* 功	能：打印串口2的接收缓冲区
* 入口参数：
* 出口参数：无
* 说	明：无				   
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
* 名	称：int has_rcv_closed(void)
* 功	能：查询是否接收过“CLOSED”,查询完后重置
* 入口参数：
* 出口参数: 1 接收过 0 未接收过
* 说	明：无				   
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
* 名	称：void USART2_SendByte(u8 Data)
* 功	能：单字符发送
* 入口参数：Data 	发送单字符数据
* 出口参数：无
* 说	明：无				   
****************************************************************************/
void USART2_SendByte(char Data)	
{
	USART_SendData(USART2, Data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/****************************************************************************
* 名	称：void USART2_SendString(u8* Data,u32 Len)
* 功	能：多字符输出
* 入口参数：Data 	输出的单字符数据
			Len		字符个数
* 出口参数：无
* 说	明：无					 
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
* 名	称：void USART2_printf(u8* Data,u32 Len)
* 功	能：多字符输出
* 入口参数：Data 	输出的单字符数据
			Len		字符个数
* 出口参数：无
* 说	明：无					 
****************************************************************************/
void USART2_printf(char* fmt,...)  
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2TxBuffer,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2TxBuffer);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2,USART2TxBuffer[j]); 
	} 
}

/****************************************************************************
* 名	称：void USART2_DMASendString(u8* Data,u32 Len)
* 功	能：DMA方式多字符输出
* 入口参数：Data 	输出的单字符数据
			Len		字符个数
* 出口参数：无
* 说	明：必须在USART2_DMATxd_Init初始化之后才能使用
			DMA无需CPU发送 和 用CPU发送 会有发送冲突	 
****************************************************************************/
void USART2_DMASendString(char* Data,u32 Len)		   //多字符输出
{
	memcpy(USART2TxBuffer, Data, Len);			   //拷贝数据到发送缓冲区
	DMA1_Channel7->CNDTR = Len;					   //发送字节数量
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //开启
	DMA_Cmd(DMA1_Channel7, ENABLE);				   //始能DMA通道
}

/****************************************************************************
* 名	称：void USART2_IRQHandler(void)	
* 功	能：中断机制
* 入口参数：无
* 出口参数：无
* 说	明：接收到的数据存入接收缓冲区
	USART_GetITStatus		检查指定的USART中断发生与否
	USART_GetFlagStatus	检查指定的USART标志位设置与否
****************************************************************************/
void USART2_IRQHandler(void)
{
	u8 data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)				// 串口接收到一个字符
	{
		if(frameValide != 0){   //又来新一帧数据。
			frameCount = 0;
			frameValide = 0;
			frameIndex = 0;
		}
		
		data = USART_ReceiveData(USART2);		
		USART1_SendByte(data);
		if(frameCount < USART2_RX_BUFF_SIZEMAX)			//如果空间未满,接收保存数据 
		{
			USART2RxBuffer[frameCount]= data;
			frameCount++;								//空间里数据个数增加
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);				//清空接收中断标志
	}
	else if(USART_GetFlagStatus(USART2,USART_FLAG_IDLE) == SET)   //接收到完整的一帧数据
	{					
			data=USART2->SR;
			data=USART2->DR;			
			frameValide = 1;
			frameIndex = 0;
			USART2RxBuffer[frameCount]= '\0';  //在末尾加一个结束符
		
			if(strstr((char *)USART2RxBuffer,"CLOSED")){
				rcvClosed = 1;
			}else{
				rcvClosed = 0;
			}
	}

	else if(USART_GetFlagStatus(USART2, USART_IT_ORE) == SET)		//检测是否有接收溢出
	{
		USART_ReceiveData(USART2);									//读数据的方式来清溢出标志
	}
}

/****************************************************************************
* 名	称：void DMA1_Channel7_IRQHandler(void)	
* 功	能：DMA中断机制
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC7)) //如果发送完成
	{
		USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);    //关闭DMA发送
		DMA_Cmd(DMA1_Channel7, DISABLE);	       			//关闭DMA通道  
	}

	DMA_ClearFlag(DMA1_FLAG_GL7| DMA1_FLAG_TC7 | DMA1_FLAG_HT7 | DMA1_FLAG_TE7);  //清除DMA相关标志
}
