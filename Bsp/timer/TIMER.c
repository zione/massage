
#include "TIMER.h"


/* �Զ���ͬ��ؼ���    --------------------------------------------------------*/

/* �Զ��������        --------------------------------------------------------*/

/* �Զ��庯����        --------------------------------------------------------*/

/* �Զ���ȫ�ֱ���          ----------------------------------------------------*/

vu16 CCR1_Val = 10000;			
vu16 CCR2_Val = 500;			
vu16 CCR3_Val = 100;			
vu16 CCR4_Val = 1000;			

uint8_t FLAG_TEST;


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
	
  	/* �� TIM2 ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//����JTAG,�ͷ�PB3/4

}

/*******************************************************************************
* ������  		: TIM2_Configuration
* ��������    	: ����TIM��ͨ��
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/

void TIM2_Configuration(void)
{
	/* ���� TIM_TimeBase ��ʼ���ṹ�� TIM_TimeBaseStructure */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/* ���� TIM_OCInit ��ʼ���ṹ�� TIM_OCInitStructure */
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* 	100us��ֵ
	*  	��������ֵΪ65535
	*  	Ԥ��ƵֵΪ(7199 + 1 = 7200)
	*  	ʱ�ӷָ�0
	*  	���ϼ���ģʽ
	*/
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInit(TIM2 , &TIM_TimeBaseStructure);
	
	/* ����Ԥ��Ƶֵ��������װ�� */
	TIM_PrescalerConfig(TIM2 , 7200-1 , TIM_PSCReloadMode_Immediate);

	/* 	
	*	���� OC1,OC2,OC3,OC4 ͨ��
	*  	����ģʽΪ������ģʽ
	*  	ʹ�ܱȽ�ƥ���������
	*  	ʱ�ӷָ�0
	*  	���ϼ���ģʽ
	*/

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;	
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
		
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	/* ��ֹԤװ�ؼĴ��� */
	TIM_OC1PreloadConfig(TIM2 , TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM2 , TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM2 , TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM2 , TIM_OCPreload_Disable);
		
	/* ʹ�� TIM �ж� */
	TIM_ITConfig(TIM2 , TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 , ENABLE);
	
	/* ���� TIM ���� */
	TIM_Cmd(TIM2 , ENABLE);	
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
