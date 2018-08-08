#include "bsp_led.h"    

/**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;
		/*����LED������ʱ��*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

//PB9--LED
		/*ѡ��Ҫ���Ƶ�GPIOB����--LED*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIOB0*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);		
}

int getLedStatus(void){
	return GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9);
}

/*********************************************END OF FILE**********************/
