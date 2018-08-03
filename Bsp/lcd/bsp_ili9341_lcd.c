#include "bsp_ili9341_lcd.h"
#include "ascii.h"
#include "bsp_sdfs_app.h"

#define DEBUG_DELAY()

void Lcd_Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/**
  * @brief  ��ʼ������LcD��IO
  * @param  ��
  * @retval ��
  */
void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* ʹ��FSMCʱ��*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* ʹ��FSMC��Ӧ��Ӧ�ܽ�ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD 
                            | RCC_APB2Periph_GPIOE 
	                          | RCC_APB2Periph_GPIOB,ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* ����LCD��λ���ƹܽ�*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 
    GPIO_Init(GPIOE, &GPIO_InitStructure);  		   
    
    /* ����FSMC���Ӧ��������,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10*/	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
		/* ����FSMC���Ӧ�Ŀ�����
		 * PD4-FSMC_NOE   :LCD-RD
		 * PD5-FSMC_NWE   :LCD-WR
		 * PD7-FSMC_NE1   :LCD-CS
		 * PD11-FSMC_A16  :LCD-DC
		*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    /* ����LCD������ƹܽ�*/
    #if 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    #elif 0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    #endif
}

 /**
  * @brief  LCD  FSMC ģʽ����
  * @param  ��
  * @retval ��
  */
void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //��ַ����ʱ��
    p.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ��
    p.FSMC_DataSetupTime = 0x05;		 //���ݽ���ʱ��
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;
    p.FSMC_AccessMode = FSMC_AccessMode_B;	 // һ��ʹ��ģʽB������LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    //FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
		FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;  
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* ʹ�� FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

/**
 * @brief  LCD ������λ
 * @param  ��
 * @retval ��
 */
void LCD_Rst(void)
{			
		GPIO_ResetBits(GPIOE, GPIO_Pin_1);	 //�͵�ƽ��λ
    Lcd_Delay(0xAFFf<<2); 					   
    GPIO_SetBits(GPIOE, GPIO_Pin_1);		 	 
    Lcd_Delay(0xAFFf<<2); 	
}
/**
 * @brief  ����lcd��ʼ���Ĵ���
 * @param  ��
 * @retval ��
 */
void LCD_REG_Config(void)
{
	/*  Power control B (CFh)  */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xCF);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x81);
	LCD_ILI9341_Parameter(0x30);
	
	/*  Power on sequence control (EDh) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xED);
	LCD_ILI9341_Parameter(0x64);
	LCD_ILI9341_Parameter(0x03);
	LCD_ILI9341_Parameter(0x12);
	LCD_ILI9341_Parameter(0x81);
	
	/*  Driver timing control A (E8h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xE8);
	LCD_ILI9341_Parameter(0x85);
	LCD_ILI9341_Parameter(0x10);
	LCD_ILI9341_Parameter(0x78);
	
	/*  Power control A (CBh) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xCB);
	LCD_ILI9341_Parameter(0x39);
	LCD_ILI9341_Parameter(0x2C);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x34);
	LCD_ILI9341_Parameter(0x02);
	
	/* Pump ratio control (F7h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xF7);
	LCD_ILI9341_Parameter(0x20);
	
	/* Driver timing control B */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xEA);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x00);
	
	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xB1);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x1B);
	
	/*  Display Function Control (B6h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xB6);
	LCD_ILI9341_Parameter(0x0A);
	LCD_ILI9341_Parameter(0xA2);
	
	/* Power Control 1 (C0h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xC0);
	LCD_ILI9341_Parameter(0x35);
	
	/* Power Control 2 (C1h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xC1);
	LCD_ILI9341_Parameter(0x11);
	
	/* VCOM Control 1(C5h) */
	LCD_ILI9341_CMD(0xC5);
	LCD_ILI9341_Parameter(0x45);
	LCD_ILI9341_Parameter(0x45);
	
	/*  VCOM Control 2(C7h)  */
	LCD_ILI9341_CMD(0xC7);
	LCD_ILI9341_Parameter(0xA2);
	
	/* Enable 3G (F2h) */
	LCD_ILI9341_CMD(0xF2);
	LCD_ILI9341_Parameter(0x00);
	
	/* Gamma Set (26h) */
	LCD_ILI9341_CMD(0x26);
	LCD_ILI9341_Parameter(0x01);
	DEBUG_DELAY();
	
	/* Positive Gamma Correction */
	LCD_ILI9341_CMD(0xE0); //Set Gamma
	LCD_ILI9341_Parameter(0x0F);
	LCD_ILI9341_Parameter(0x26);
	LCD_ILI9341_Parameter(0x24);
	LCD_ILI9341_Parameter(0x0B);
	LCD_ILI9341_Parameter(0x0E);
	LCD_ILI9341_Parameter(0x09);
	LCD_ILI9341_Parameter(0x54);
	LCD_ILI9341_Parameter(0xA8);
	LCD_ILI9341_Parameter(0x46);
	LCD_ILI9341_Parameter(0x0C);
	LCD_ILI9341_Parameter(0x17);
	LCD_ILI9341_Parameter(0x09);
	LCD_ILI9341_Parameter(0x0F);
	LCD_ILI9341_Parameter(0x07);
	LCD_ILI9341_Parameter(0x00);
	
	/* Negative Gamma Correction (E1h) */
	LCD_ILI9341_CMD(0XE1); //Set Gamma
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x19);
	LCD_ILI9341_Parameter(0x1B);
	LCD_ILI9341_Parameter(0x04);
	LCD_ILI9341_Parameter(0x10);
	LCD_ILI9341_Parameter(0x07);
	LCD_ILI9341_Parameter(0x2A);
	LCD_ILI9341_Parameter(0x47);
	LCD_ILI9341_Parameter(0x39);
	LCD_ILI9341_Parameter(0x03);
	LCD_ILI9341_Parameter(0x06);
	LCD_ILI9341_Parameter(0x06);
	LCD_ILI9341_Parameter(0x30);
	LCD_ILI9341_Parameter(0x38);
	LCD_ILI9341_Parameter(0x0F);
	
	/* memory access control set */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0x36); 	
	LCD_ILI9341_Parameter(0xC8);    /*����  ���Ͻǵ�(���)�����½�(�յ�)ɨ�跽ʽ*/
	DEBUG_DELAY();
	
	/* column address control set */
	LCD_ILI9341_CMD(0X2A); 
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0xEF);
	
	/* page address control set */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0X2B); 
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x01);
	LCD_ILI9341_Parameter(0x3F);
	
	/*  Pixel Format Set (3Ah)  */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0x3a); 
	LCD_ILI9341_Parameter(0x55);
	
	/* Sleep Out (11h)  */
	LCD_ILI9341_CMD(0x11);	
	Lcd_Delay(0xAFFf<<2);
	DEBUG_DELAY();
	
	/* Display ON (29h) */
	LCD_ILI9341_CMD(0x29); 
}


/**
 * @brief  lcd��ʼ�������Ҫ�õ�lcd��һ��Ҫ�����������
 * @param  ��
 * @retval ��
 */
void LCD_Init(void)
{
	LCD_GPIO_Config();
	LCD_FSMC_Config();	
	LCD_Rst();
	LCD_REG_Config();
	LCD_Clear(0 ,0,240,320,BLACK);
}

/* ����Һ��GRAM��ɨ�跽�� 
 * �����óɲ�ͬ��ɨ��ģʽʱ, page(��x) �� column(��y) ��ֵ�ǻ�ı��
 */
void Lcd_GramScan( uint16_t option )
{	
	switch(option)
	{
		case 1:
		{/* ���Ͻ�->���½�      ��ʾ��Ӣ��ʱ�õ�������ģʽ */
			LCD_ILI9341_CMD(0x36); 
			LCD_ILI9341_Parameter(0xC8);   
			LCD_ILI9341_CMD(0X2A); 
			LCD_ILI9341_Parameter(0x00);	/* x start */	
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);  /* x end */	
			LCD_ILI9341_Parameter(0xEF);

			LCD_ILI9341_CMD(0X2B); 
			LCD_ILI9341_Parameter(0x00);	/* y start */  
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x01);	/* y end */   
			LCD_ILI9341_Parameter(0x3F);
					
		}break;
		case 2:
		{/* ���½�->���Ͻ�      ��ʾ����ͷͼ��ʱ�õ�������ģʽ */	
			LCD_ILI9341_CMD(0x36); 
			LCD_ILI9341_Parameter(0x68);	
			LCD_ILI9341_CMD(0X2A); 
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x01);
			LCD_ILI9341_Parameter(0x3F);	

			LCD_ILI9341_CMD(0X2B); 
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0xEF);			
		}break;
		case 3:
		{/* ���½�->���Ͻ�      ��ʾBMPͼƬʱ�õ�������ģʽ */
			LCD_ILI9341_CMD(0x36); 
			LCD_ILI9341_Parameter(0x28);	
			LCD_ILI9341_CMD(0X2A); 
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x01);
			LCD_ILI9341_Parameter(0x3F);	

			LCD_ILI9341_CMD(0X2B); 
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0xEF);			
		}break;
	}
		
	/* write gram start */
	LCD_ILI9341_CMD(0x2C);
}

/*                                      ����ΪLCD�ײ㺯��
 *------------------------------------------------------------------------------------------------------
 *----------------------------------------���Ƿָ���----------------------------------------------------|
 *------------------------------------------------------------------------------------------------------
 *                                      ����ΪLCDӦ�ú���
 */
void LCD_Clear(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
	uint32_t i = 0;
	
	/* column address control set */
	LCD_ILI9341_CMD(0X2A);
	LCD_ILI9341_Parameter( x >> 8 );	 /* �ȸ�8λ��Ȼ���8λ */
	LCD_ILI9341_Parameter( x & 0xff );	         /* column start   */ 
	LCD_ILI9341_Parameter( (x+width-1) >> 8 );   /* column end   */
	LCD_ILI9341_Parameter( (x+width-1) & 0xff );
	
	/* page address control set */	
  LCD_ILI9341_CMD(0X2B); 			     
	LCD_ILI9341_Parameter( y >> 8 );			/* page start   */
	LCD_ILI9341_Parameter( y & 0xff );
	LCD_ILI9341_Parameter( (y+height-1) >> 8);  /* page end     */
	LCD_ILI9341_Parameter( (y+height-1) & 0xff);
	
	/* memory write */
	LCD_ILI9341_CMD(0x2c);	
		
	for( i=0; i < width*height; i++ )
	{
		LCD_WR_Data( color );
		//Delay(0x0FFf);
	}	
}

void LCD_SetCursor(uint16_t x, uint16_t y)	
{	
	LCD_ILI9341_CMD(0X2A); 				 /* ����X���� */
	LCD_ILI9341_Parameter(x>>8);	 /* �ȸ�8λ��Ȼ���8λ */
	LCD_ILI9341_Parameter(x&0xff);	 /* ������ʼ��ͽ�����*/
	LCD_ILI9341_Parameter(x>>8);
	LCD_ILI9341_Parameter(x&0xff);

    LCD_ILI9341_CMD(0X2B); 			     /* ����Y����*/
	LCD_ILI9341_Parameter(y>>8);
	LCD_ILI9341_Parameter(y&0xff);
	LCD_ILI9341_Parameter(y>>8);
	LCD_ILI9341_Parameter(y&0xff);		     
}

//  _ _ _ _ _ _
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
//  - - - - - -
void LCD_OpenWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{	
	LCD_ILI9341_CMD(0X2A); 				 /* ����X���� */
	LCD_ILI9341_Parameter( x >> 8 );	 /* �ȸ�8λ��Ȼ���8λ */
	LCD_ILI9341_Parameter( x & 0xff );	 /* ������ʼ��ͽ�����*/
	LCD_ILI9341_Parameter( (x+width-1) >> 8 );
	LCD_ILI9341_Parameter( (x+width-1) & 0xff );

	LCD_ILI9341_CMD(0X2B); 			     /* ����Y����*/
	LCD_ILI9341_Parameter( y >> 8 );
	LCD_ILI9341_Parameter( y & 0xff );
	LCD_ILI9341_Parameter( (y+height-1) >> 8);
	LCD_ILI9341_Parameter( (y+height-1) & 0xff);	
		
	/* write gram start */
	LCD_ILI9341_CMD(0x2C);
}

void LCD_SetPoint(uint16_t x , uint16_t y , uint16_t color)	
{	
	LCD_SetCursor(x, y);
	LCD_ILI9341_CMD(0x2c);	         /* д���� */
	LCD_WR_Data(color);
}

uint16_t LCD_RD_data(void)	
{	
	uint16_t R=0, G=0, B=0 ;

	R = *(__IO uint16_t *)Bank1_LCD_D; 	  /*FIRST READ OUT DUMMY DATA*/
	R = *(__IO uint16_t *)Bank1_LCD_D;  	/*READ OUT RED DATA  */
	B = *(__IO uint16_t *)Bank1_LCD_D;  	/*READ OUT BLACK DATA*/
	G = *(__IO uint16_t *)Bank1_LCD_D;  	/*READ OUT GREEN DATA*/
	
    return (((R>>11)<<11) | ((G>>10)<<5) | (B>>11));
}

uint16_t LCD_GetPoint(uint16_t x , uint16_t y)
{ 
	uint16_t temp;

	LCD_SetCursor(x, y);
	LCD_ILI9341_CMD(0x2e);         /* ������ */
	temp=LCD_RD_data();
	return (temp);
}

/*-------------------------------------------------------------------------------------------------------*/
/*    column 240
      _ _ _ _ _ _
 page|           |
     |           |
     |           |
     |           |
     |           |
     |           |  320        
     |           |
     |           |
     |           |
     |           |
     |           |
     |           |
      - - - - - -
*/
/* ��ʾһ���ַ� 
 * ��СΪ 12�����ȣ�* 6���߶ȣ�
 */
 
 #define COLUMN_WINDOW 230
void LCD_DispChar(uint16_t x, uint16_t y, uint8_t ascii, uint16_t color)
{
	uint16_t page, column, temp, i;
	i = ascii - ' ';
	
	LCD_OpenWindow(x, y, STR_WIDTH, STR_HEIGHT);
	LCD_ILI9341_CMD(0X2C);	
	
	for( page=0; page < STR_HEIGHT; page++ )
	{
		temp = asc2_1206[i][page];
		for( column=0; column < STR_WIDTH; column++ )
		{
			if( temp & 0x01 )
			{
				LCD_WR_Data( color );
			}
			else
			{
				LCD_WR_Data( BACKGROUND );								
			}
			temp >>= 1;		
		}/* һ��д�� */
	}/* ȫ��д�� */
}

/* ��ʾ�ַ��� */
void LCD_DispStr(uint16_t x, uint16_t y, uint8_t *pstr, uint16_t color)
{
	while( *pstr != '\0' )
	{
		if( x > (COLUMN_WINDOW-STR_WIDTH) )
		{
			x = 20;
			y += STR_HEIGHT;
		}
		if( y > (PAGE-STR_HEIGHT) )
		{
			x = 0;
			y = 0;
		}
		LCD_DispChar(x, y, *pstr, color);
		x += STR_WIDTH;
		pstr++;
	}
}

//  temp = 345   length = 0
//  34           1
//  3            2
//  0            3


//  0    6    12
//  ____ ____ ____

/* ��ʾ���� 
 * ��СΪ 12�����ȣ�* 6���߶ȣ�
 */
void LCD_DisNum(uint16_t x, uint16_t y, uint32_t num, uint16_t color)
{
	uint32_t length = 0, temp = 0;
	temp = num;
	
	if( temp == 0 )
	{
		LCD_DispChar(x, y, '0', color);
		return;
	}
	
	while( temp )
	{// �õ�num�ĳ���
		temp /= 10;
		length ++;
	}
	
	while( num )
	{
//		if( x > (COLUMN_WINDOW-STR_WIDTH) )
//		{
//			x = 20;
//			y += STR_HEIGHT;
//		}
//		if( y > (PAGE-STR_HEIGHT) )
//		{
//			x = 0;
//			y = 0;
//		}
		/* �Ӹ�λ��ʼ��ʾ */
		LCD_DispChar((x+STR_WIDTH*(length--)-STR_WIDTH), y, (num%10)+'0', color);
		num /= 10;
	}	
}
/*------------------------------------------------------------------------------------------------------*/
/*    column 240
      _ _ _ _ _ _
 page|           |
     |           |
     |           |
     |           |
     |           |
     |           |  320        
     |           |
     |           |
     |           |
     |           |
     |           |
     |           |
      - - - - - -
*/

/* ��ʾһ������ 
 * ��СΪ 16�����ȣ�* 16���߶ȣ�����32���ֽ�
 * ȡģ˳��Ϊ����λ��ǰ����λ�ں�
 */
void LCD_DispCH(uint16_t x, uint16_t y, const uint8_t *pstr, uint16_t color)
{
	uint8_t page , column;
	uint8_t buffer[32];		
	uint16_t tmp_char=0;
	
	LCD_OpenWindow(x, y, CH_WIDTH, CH_HEIGHT);
	LCD_ILI9341_CMD(0X2C);
	
	GetGBKCode_from_sd(buffer,pstr);	 /* ȡ��ģ���� */
	
	for(page=0; page< CH_HEIGHT; page++)
	{
		tmp_char=buffer[page*2];
		tmp_char=(tmp_char<<8);
		tmp_char|=buffer[2*page+1];
		
		for (column=0; column< CH_WIDTH; column++)
		{			
			if ( tmp_char & (0x01<<15) )
			{				
				LCD_WR_Data(color);
			}
			else
			{			
				LCD_WR_Data(BACKGROUND);
			}
			tmp_char <<= 1;
		}
	}
}

/*
 * ��ʾһ������
 */
void LCD_DispStrCH(uint16_t x, uint16_t y, const uint8_t *pstr, uint16_t color) 
{
	while( *pstr != '\0' )
	{
		if( x > (COLUMN-CH_WIDTH) )
		{
			x = 0;
			y += CH_HEIGHT;
		}
		if( y > (PAGE-CH_HEIGHT) )
		{
			x = 0;
			y = 0;
		}		
		LCD_DispCH(x, y, pstr, color);
		pstr +=2;
		x += CH_WIDTH;		
	}	   
}

/* 
 * ��Ӣ�Ļ����ʾ��������ʾ���ı�����  
 * ���Ĵ�С��16*16
 * Ӣ�Ĵ�С��12*6
 */

void LCD_DispEnCh(uint16_t x, uint16_t y, const uint8_t *pstr, uint16_t color)
{
	while(*pstr != '\0')
	{
		if(*pstr <= 126)		/* �ַ��� */
		{
			if( x > (COLUMN_WINDOW-STR_WIDTH) )
			{
				x = 20;
				y += STR_HEIGHT;
			}
			if( y > (PAGE-STR_HEIGHT) )
			{
				x = 0;
				y = 0;
			}
			LCD_DispChar(x, y, *pstr, color);
			x += STR_WIDTH;
			pstr++;
		}
		else	/* ���� */
		{
			if( x > (COLUMN_WINDOW-CH_WIDTH) )
			{
				x = 20;
				y += CH_HEIGHT;
			}
			if( y > (PAGE-CH_HEIGHT) )
			{
				x = 0;
				y = 0;
			}		
			LCD_DispCH(x, y, pstr, color);
			pstr +=2;
			x += CH_WIDTH;
    }
  }
} 




void LCD_Test(void)
{
  uint32_t n;
  //LCD_Init();  
  GPIO_ResetBits(GPIOB, GPIO_Pin_1);
  
  LCD_Clear(0, 0, 240, 320, BACKGROUND);  
	LCD_DispChar(60, 60, 'A', RED);   
  
	LCD_DispStr(0, 10, (uint8_t *)"welcome to use wildfire ISO stm32 development board", RED);	
	LCD_DispStr(0, 150, (uint8_t *)"0123456789~!@#$%^&*()_+", RED);
	LCD_DispStr(40, 100, (uint8_t *)"count:", RED);  
  
	for( n=0; n<50000; n++ )
	{
		LCD_DisNum(100, 100, n, RED);
		Lcd_Delay(0xAFFf>>4);
	}
  while(1);
}
