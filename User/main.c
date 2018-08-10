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
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{ 
	RCC_Configuration();
	delay_init();																		//延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	LED_GPIO_Config();
	KEY_Init();
	
  USART1_Config();		
  USART2_Config();
	Set_PrintfPort(1);		//设置DebugPf从串口1输出
	TIM2_Configuration();
	
	Sim_ini();   //初始化sim868
	
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

