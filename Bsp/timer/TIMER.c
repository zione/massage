
#include "TIMER.h"
#include "bsp_led.h"

/* �Զ���ͬ��ؼ���    --------------------------------------------------------*/

/* �Զ��������        --------------------------------------------------------*/

/* �Զ��庯����        --------------------------------------------------------*/

/* �Զ���ȫ�ֱ���          ----------------------------------------------------*/

vu16 CCR1_Val = 10000;			
vu16 CCR2_Val = 500;			
vu16 CCR3_Val = 100;			
vu16 CCR4_Val = 1000;			

uint8_t FLAG_TEST;

int ping_time = 0;    //��������ʱ�������
int switch_time = 0;    //���ص�ʱ�������
/* �Զ��庯������      --------------------------------------------------------*/

/******************************************************************************
* ������		: RCC_Configuration
* ��������  	: ����ϵͳ������ʱ��
* �������  	: ��
* ������  	: ��
* ����ֵ    	: ��
******************************************************************************/
void RCC_Configuration(void)
{
	/* ����ö�����ͱ��� HSEStartUpStatus */
	ErrorStatus HSEStartUpStatus;

  	/* ��λϵͳʱ������ */
  	RCC_DeInit();
  	/* ���� HSE */
  	RCC_HSEConfig(RCC_HSE_ON);
			
  	/* �ȴ� HSE �����ȶ� */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* �ж� HSE ���Ƿ���ɹ����������if()�ڲ� */
  	if(HSEStartUpStatus == SUCCESS)
  	{
    	/* ѡ�� HCLK��AHB��ʱ��ԴΪSYSCLK 1��Ƶ */
    	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    	/* ѡ�� PCLK2 ʱ��ԴΪ HCLK��AHB�� 1��Ƶ */
    	RCC_PCLK2Config(RCC_HCLK_Div1); 
    	/* ѡ�� PCLK1 ʱ��ԴΪ HCLK��AHB�� 2��Ƶ */
    	RCC_PCLK1Config(RCC_HCLK_Div2);
			/* ����FLASH��ʱ������Ϊ2 */
    	FLASH_SetLatency(FLASH_Latency_2);
    	/* ʹ��FLASHԤȡ���� */
    	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		  /* ѡ�����໷��PLL��ʱ��ԴΪHSE 1��Ƶ����Ƶ��Ϊ3�������Ƶ��Ϊ72MHz */
    	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    	/* ʹ�� PLL */ 
    	RCC_PLLCmd(ENABLE);
    	/* �ȴ� PLL ����ȶ� */
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    	/* ѡ�� SYSCLK ʱ��ԴΪ PLL */
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    	/* �ȴ� PLL ��Ϊ SYSCLK ʱ��Դ */
    	while(RCC_GetSYSCLKSource() != 0x08);
			FLAG_TEST=0;
  	}else{
			FLAG_TEST=1;
		}
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//����JTAG,�ͷ�PB3/4

}

/*******************************************************************************
* ������  		: TIM2_Configuration
* ��������    	: һ��һ���ж�
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/

void TIM2_Configuration(void)
{
	/* ���� TIM_TimeBase ��ʼ���ṹ�� TIM_TimeBaseStructure */
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
* ������  		: NVIC_Configuration
* ��������    	: ����NVIC����
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/

void NVIC_Configuration(void)
{								
	/* ���� NVIC ��ʼ���ṹ�� */
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ѡ�����ȼ�����1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* ���� TIM2 �ж�, 0����ռ���ȼ���0����ռ���ȼ� */	
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

//�Ƿ��Ѿ���ʱ��10s
int isPingCountEnd(){
	return ping_time == MAX_PING_COUNTER?0:1;
}

void resetPingCount(){
	ping_time = 0;
}

//���ÿ��رպ϶�����
void setSwitchTime(int value){
	switch_time = value;
	if(value > 0){
		LED_ON;
	}else{
		LED_OFF;
	}
}
