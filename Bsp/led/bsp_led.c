#include "bsp_led.h"    

/**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;
		/*开启LED的外设时钟*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

//PB9--LED
		/*选择要控制的GPIOB引脚--LED*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOB0*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);		
}

int getLedStatus(void){
	return GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9);
}

/*********************************************END OF FILE**********************/
