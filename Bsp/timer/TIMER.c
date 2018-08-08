
#include "TIMER.h"
#include "bsp_led.h"

/* 自定义同义关键字    --------------------------------------------------------*/

/* 自定义参数宏        --------------------------------------------------------*/

/* 自定义函数宏        --------------------------------------------------------*/

/* 自定义全局变量          ----------------------------------------------------*/

vu16 CCR1_Val = 10000;			
vu16 CCR2_Val = 500;			
vu16 CCR3_Val = 100;			
vu16 CCR4_Val = 1000;			

uint8_t FLAG_TEST;

int ping_time = 0;    //心跳包的时间计数器
int switch_time = 0;    //开关的时间计数器
/* 自定义函数声明      --------------------------------------------------------*/

/******************************************************************************
* 函数名		: RCC_Configuration
* 函数描述  	: 设置系统各部分时钟
* 输入参数  	: 无
* 输出结果  	: 无
* 返回值    	: 无
******************************************************************************/
void RCC_Configuration(void)
{
	/* 定义枚举类型变量 HSEStartUpStatus */
	ErrorStatus HSEStartUpStatus;

  	/* 复位系统时钟设置 */
  	RCC_DeInit();
  	/* 开启 HSE */
  	RCC_HSEConfig(RCC_HSE_ON);
			
  	/* 等待 HSE 起振并稳定 */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* 判断 HSE 起是否振成功，是则进入if()内部 */
  	if(HSEStartUpStatus == SUCCESS)
  	{
    	/* 选择 HCLK（AHB）时钟源为SYSCLK 1分频 */
    	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    	/* 选择 PCLK2 时钟源为 HCLK（AHB） 1分频 */
    	RCC_PCLK2Config(RCC_HCLK_Div1); 
    	/* 选择 PCLK1 时钟源为 HCLK（AHB） 2分频 */
    	RCC_PCLK1Config(RCC_HCLK_Div2);
			/* 设置FLASH延时周期数为2 */
    	FLASH_SetLatency(FLASH_Latency_2);
    	/* 使能FLASH预取缓存 */
    	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		  /* 选择锁相环（PLL）时钟源为HSE 1分频，倍频数为3，则输出频率为72MHz */
    	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    	/* 使能 PLL */ 
    	RCC_PLLCmd(ENABLE);
    	/* 等待 PLL 输出稳定 */
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    	/* 选择 SYSCLK 时钟源为 PLL */
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    	/* 等待 PLL 成为 SYSCLK 时钟源 */
    	while(RCC_GetSYSCLKSource() != 0x08);
			FLAG_TEST=0;
  	}else{
			FLAG_TEST=1;
		}
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//禁用JTAG,释放PB3/4

}

/*******************************************************************************
* 函数名  		: TIM2_Configuration
* 函数描述    	: 一秒一次中断
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/

void TIM2_Configuration(void)
{
	/* 定义 TIM_TimeBase 初始化结构体 TIM_TimeBaseStructure */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_DeInit(TIM2);
	
	TIM_TimeBaseStructure.TIM_Period=2000-1; 
	TIM_TimeBaseStructure.TIM_Prescaler=(36000-1); 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);  
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_Configuration();
	
	TIM_Cmd(TIM2,ENABLE);        
}

/*******************************************************************************
* 函数名  		: NVIC_Configuration
* 函数描述    	: 设置NVIC参数
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/

void NVIC_Configuration(void)
{								
	/* 定义 NVIC 初始化结构体 */
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 选择优先级分组1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* 开启 TIM2 中断, 0级先占优先级，0级后占优先级 */	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		if(ping_time < MAX_PING_COUNTER){
			ping_time++;
		}else{
			
		}
		
		if(switch_time > 0){
			switch_time--;
		}
		
		if(switch_time == 0){
			LED_OFF;
		}
	}
}

//是否已经计时了10s
int isPingCountEnd(){
	return ping_time == MAX_PING_COUNTER?0:1;
}

void resetPingCount(){
	ping_time = 0;
}

//设置开关闭合多少秒
void setSwitchTime(int value){
	switch_time = value;
	if(value > 0){
		LED_ON;
	}else{
		LED_OFF;
	}
}
