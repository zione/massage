

#include "bsp_iso_test.h"
#include "bsp_adc.h"
#include "stm32f10x.h"
#include "bsp_led.h" 
#include "bsp_SysTick.h"
#include "TIMER.h"
#include "USART1_init.h"
#include "USART2_init.h"
#include "printf_init.h"


extern volatile uint8_t rec_cmd;

static void delay(uint16_t t);

/*
 * ϵͳ�����λ
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* �ر������ж� */  
  NVIC_SystemReset();   /* ϵͳ��λ */
}

void MCU_Sleep(void)
{
	/* �ر�����led��	*/
	GPIO_SetBits(GPIOB, GPIO_Pin_3|GPIO_Pin_4);
			
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);//standby?? 
//	PWR_EnterSTANDBYMode();//standby?? 
}

static void delay(uint16_t t)
{
	uint16_t i;
	while(t--)
	{
		for(i=0;i<1000;i++);
	}
}

/*
 * ��ʼ��Һ�������ڣ���������flash��sd�� * 
 */
void ISO_Init(void)
{
	uint8_t k;
  
  /* ����USART1 */
  USART1_Config();		
	/* ����USART2 */
  USART2_Config();
	/* ���� TIM */
	TIM2_Configuration();

	USART2_DMATxd_Init();
	
	Set_PrintfPort(1);		//����DebugPf�Ӵ���1���
}




/* ---------------------------------------end of file------------------------------------- */
