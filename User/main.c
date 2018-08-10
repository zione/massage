#include "bsp_led.h"
#include "delay.h"
#include "bsp_key.h"
#include "TIMER.h"
#include "USART1_init.h"
#include "USART2_init.h"
#include "printf_init.h"
#include "SIM800L.h"
#include <string.h>
#include "ali_mqtt.h"

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{ 
	RCC_Configuration();
	delay_init();																		//��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	LED_GPIO_Config();
	KEY_Init();
	
  USART1_Config();		
  USART2_Config();
	Set_PrintfPort(1);		//����DebugPf�Ӵ���1���
	TIM2_Configuration();
	
	Sim_ini();   //��ʼ��sim868
	
	while(1)
	{
		switch(KEY_Scan(0))
		{
			case KEY0_PRES:
				LED_TOGGLE;
				break;
			default:
				break;
		}
		handle_mqtt();
	}
}
/* ----------------------------------------end of file------------------------------------------------ */

