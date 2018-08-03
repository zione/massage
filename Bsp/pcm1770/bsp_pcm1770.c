#include "stm32f10x.h"
#include "bsp_pcm1770.h"
#include "bsp_ili9341_lcd.h"
#include <stdio.h>


uint8_t voice=28;						//������С


/**
  * @brief  Delay ������ʱ
  * @param  Delay Num
  * @retval None
  */
static void Delay(u32 Num)
{
	vu32 Count = Num*4;
	
	while (--Count);	
}



/**
  * @brief  PCM1770 Init��ʼ��PCM1770�õ������ţ�����λоƬ
  * @param  None
  * @retval None
  */
void PCM1770Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ʹ�ܿ����źŵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	Delay(50);
	
	//Ӳ����λ
	PCM_PD_SET_L;
	Delay(1000);
	PCM_PD_SET_H;  	
	Delay(100);

	PCM_CS_SET_H ;
	PCM_CLK_SET_H;
	PCM_DAT_SET_H; 	
}



/**
  * @brief  ��PCM1770д���������
  * @param  Reg:�Ĵ�������
	*					Data:Ҫд�������
  * @retval None
  */
  
void PCM_WriteData(const u8 Reg, const u8 Data)
{
	vu16 TrasferData, i; 
	
	TrasferData = Data;
	TrasferData |= (Reg<<8)&0xff00;

	PCM_CS_SET_L;			//Ƭѡ
	Delay(10);
	for (i = 0; i < 16; i++)
	{
		//����ʱMSB first
		PCM_CLK_SET_L;
		
		if (TrasferData&(0x8000>>i))
		{
		   PCM_DAT_SET_H;	
		}
		else
		{
			 PCM_DAT_SET_L;	
		}
		Delay(10);			//�������ȶ�
		PCM_CLK_SET_H;	//������д��
		Delay(10);			//�ȴ��ӻ�������		
	}		
	
	PCM_CLK_SET_H;
  PCM_DAT_SET_H;	

	PCM_CS_SET_H;			//�ͷ�Ƭѡ
	Delay(10);
}
  
/**
  * @brief  Volume_Add
  * @param  None
  * @retval None
  */
void PCM1770_VolumeSet(vu8 vol)
{
		char lcd_char[10] ;
		voice = vol;

		PCM_WriteData(0x01, vol);//���������������
		PCM_WriteData(0x02, vol);//�����Ҷ�������  

		Lcd_GramScan( 1 );
		sprintf(lcd_char,"����:%2d",voice);		//��ʾ������LCD��
		LCD_DispEnCh(155,235,lcd_char,BLUE);	

}

/**
  * @brief  Volume_Dec
  * @param  None
  * @retval None
  */
void Volume_Dec(void)
{
	char lcd_char[10] ;
	
	if (voice > 0)
	{
		voice--;
		PCM_WriteData(0x01, voice);//���������������
		PCM_WriteData(0x02, voice);//�����Ҷ�������  	
	
		Lcd_GramScan( 1 );
		sprintf(lcd_char,"����:%2d",voice);	//��ʾ������LCD��
		LCD_DispEnCh(155,235,lcd_char,BLUE);

	}	
	
}

/**
  * @brief  Volume_Add
  * @param  None
  * @retval None
  */
void Volume_Add(void)
{
	char lcd_char[10];
	if (voice < C_VOLUME_MAX)
	{
		voice++;
		PCM_WriteData(0x01, voice);//���������������
		PCM_WriteData(0x02, voice);//�����Ҷ�������  
	
		Lcd_GramScan( 1 );
		sprintf(lcd_char,"����:%2d",voice);	//��ʾ������LCD��
		LCD_DispEnCh(155,235,lcd_char,BLUE);
		
	}
}



/**
  * @brief  Volume_Add
  * @param  None
  * @retval None
  */
void PCM1770_Mute(void)
{
		PCM_WriteData(0x01, 0xc0);//���Ҷ������� 
		PCM_WriteData(0x02, 0x00);//�Ҷ�������		
}



/**
  * @brief  Loud_Speaker_ON ����������
  * @param  none
  * @retval None
  */
void Loud_Speaker_ON(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		//ʹ�ܿ����źŵ�ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);

	}

/**
  * @brief  Loud_Speaker_OFF �ر�������
  * @param  none
  * @retval None
  */
void Loud_Speaker_OFF(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		//ʹ�ܿ����źŵ�ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_SetBits(GPIOA,GPIO_Pin_8);

	}


/******************* end of file**************************/
