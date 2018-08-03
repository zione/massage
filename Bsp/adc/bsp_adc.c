/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   adc1 Ӧ��bsp / DMA ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
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

extern bool flag_rck;//ADCת���ĵ����ٴμ�⣬�������

#ifdef LCD_DISPLAY
	#include "bsp_ili9341_lcd.h"
#endif

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t ADC_ConvertedValue;
//__IO u16 ADC_ConvertedValueLocal;

/**
  * @brief  ʹ��ADC1��DMA1��ʱ�ӣ���ʼ��PC.01
  * @param  ��
  * @retval ��
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
	GPIO_Init(GPIOC, &GPIO_InitStructure);				// PC0,����ʱ������������
}

/**
  * @brief  ����ADC1�Ĺ���ģʽΪMDAģʽ
  * @param  ��
  * @retval ��
  */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 			//ADC��ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  				//�ڴ��ַ�̶�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//����
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//����ADCģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 				//��ֹɨ��ģʽ��ɨ��ģʽ���ڶ�ͨ���ɼ�
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//��������ת��ģʽ������ͣ�ؽ���ADCת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//��ʹ���ⲿ����ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//�ɼ������Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 								//Ҫת����ͨ����Ŀ1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	/*����ADC1��ͨ��10Ϊ55.	5���������ڣ�����Ϊ1 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
	
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/*��λУ׼�Ĵ��� */   
	ADC_ResetCalibration(ADC1);
	/*�ȴ�У׼�Ĵ�����λ��� */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* ADCУ׼ */
	ADC_StartCalibration(ADC1);
	/* �ȴ�У׼���*/
	while(ADC_GetCalibrationStatus(ADC1));
	
	/* ����û�в����ⲿ����������ʹ���������ADCת�� */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  ADC1��ʼ��
  * @param  ��
  * @retval ��
  */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}

/*-----------------------------------------------------------------------------------------------------*/


// �����ʱ
void ADC_Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

void ADC_Test(void)
{
  // �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
  float ADC_ConvertedValueLocal; 
  
  /* enable adc1 and config adc1 to dma mode */
	ADC1_Init();
	
	ADC_Delay(720000);  	

	ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3*2; // ��ȡת����ADֵ
	if(ADC_ConvertedValueLocal<3.4)
	{
		DebugPf("\r\n ��Դ��ѹV= %f(v)���Զ��ػ�\r\n",ADC_ConvertedValueLocal); 
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
		DebugPf("\r\n ��Դ��ѹ V= %f(v) \r\n",ADC_ConvertedValueLocal); 
}

void BATCK(void)
{
	  // �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
  float ADC_ConvertedValueLocal; 
	if(0x745<ADC_ConvertedValue<0x0AE8){
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3*2; // ��ȡת����ADֵ
		if(ADC_ConvertedValueLocal<3.4)
		{
			if(flag_rck==1){
				if(!GetKeySTA()){//���Status�������ͣ���ģ���Ѿ��ػ�
					DebugPf("\r\n ��Դ��ѹV= %f(v)���Զ��ػ�\r\n",ADC_ConvertedValueLocal); 
					MCU_Sleep();
				}
			}else{
				flag_rck=1;
				DebugPf("\r\n ��Դ��ѹV= %f(v)���ٴμ��\r\n",ADC_ConvertedValueLocal); 
			}
		}else{
			DebugPf("\r\n ��Դ��ѹ V= %f(v) \r\n",ADC_ConvertedValueLocal); 
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
