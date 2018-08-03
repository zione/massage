/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   adc1 应用bsp / DMA 模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "bsp_adc.h"
#include "USART1_init.h"
#include "bsp_led.h"
#include "bsp_iso_test.h"
#include "SIM800L.h"

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

static void delay(uint16_t t);

extern bool flag_rck;//ADC转换的电量再次检测，避免错误

#ifdef LCD_DISPLAY
	#include "bsp_ili9341_lcd.h"
#endif

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t ADC_ConvertedValue;
//__IO u16 ADC_ConvertedValueLocal;

/**
  * @brief  使能ADC1和DMA1的时钟，初始化PC.01
  * @param  无
  * @retval 无
  */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
	
	/* Configure PC.0  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);				// PC0,输入时不用设置速率
}

/**
  * @brief  配置ADC1的工作模式为MDA模式
  * @param  无
  * @retval 无
  */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 			//ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  				//内存地址固定
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 				//禁止扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 								//要转换的通道数目1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*配置ADC时钟，为PCLK2的8分频，即9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	/*配置ADC1的通道10为55.	5个采样周期，序列为1 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
	
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/*复位校准寄存器 */   
	ADC_ResetCalibration(ADC1);
	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* ADC校准 */
	ADC_StartCalibration(ADC1);
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1));
	
	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  ADC1初始化
  * @param  无
  * @retval 无
  */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}

/*-----------------------------------------------------------------------------------------------------*/


// 软件延时
void ADC_Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

void ADC_Test(void)
{
  // 局部变量，用于保存转换计算后的电压值 	 
  float ADC_ConvertedValueLocal; 
  
  /* enable adc1 and config adc1 to dma mode */
	ADC1_Init();
	
	ADC_Delay(720000);  	

	ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3*2; // 读取转换的AD值
	if(ADC_ConvertedValueLocal<3.4)
	{
		DebugPf("\r\n 电源电压V= %f(v)，自动关机\r\n",ADC_ConvertedValueLocal); 
		LED4(ON);
		delay(2000);
		LED4(OFF);
		delay(2000);
		LED4(ON);
		delay(2000);
		LED4(OFF);
		delay(2000);		
		LED4(ON);
		delay(2000);
		LED4(OFF);
		delay(2000);
		MCU_Sleep();
	}
		DebugPf("\r\n The ADC_Channel10 value = 0x%04X \r\n", ADC_ConvertedValue); 
		DebugPf("\r\n 电源电压 V= %f(v) \r\n",ADC_ConvertedValueLocal); 
}

void BATCK(void)
{
	  // 局部变量，用于保存转换计算后的电压值 	 
  float ADC_ConvertedValueLocal; 
	if(0x745<ADC_ConvertedValue<0x0AE8){
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3*2; // 读取转换的AD值
		if(ADC_ConvertedValueLocal<3.4)
		{
			if(flag_rck==1){
				if(!GetKeySTA()){//如果Status引脚拉低，则模块已经关机
					DebugPf("\r\n 电源电压V= %f(v)，自动关机\r\n",ADC_ConvertedValueLocal); 
					MCU_Sleep();
				}
			}else{
				flag_rck=1;
				DebugPf("\r\n 电源电压V= %f(v)，再次检测\r\n",ADC_ConvertedValueLocal); 
			}
		}else{
			DebugPf("\r\n 电源电压 V= %f(v) \r\n",ADC_ConvertedValueLocal); 
			flag_rck=0;
		}
	}
}

static void delay(uint16_t t)
{
	uint16_t i;
	while(t--)
	{
		for(i=0;i<1000;i++);
	}
}

/*********************************************END OF FILE**********************/
