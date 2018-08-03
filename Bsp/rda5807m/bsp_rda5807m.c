#include "bsp_rda5807m.h"
#include "bsp_i2c_gpio.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h" 
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_touch.h"
#include "bsp_ili9341_lcd.h"
#include <stdio.h>
#include "bsp_bmp.h"

//#define delayms(ms) Delay_us(ms)

#define RDA5807M_SLAVE_ADDRESS 0x20


TOUCH_EVEN_FM touch_even_fm = EF_REFRESH;		//�¼���־

static uint8_t volume = 0;						//����
static uint16_t chan = 0;							//Ƶ��	

static void RDA5807_write_reg(void);
static void RDA5807_read_reg(uint8_t *reg_buf);
static void RDA5807_power(void);
static void RDA5807_FM_seek(void);
static void even_process(void);

// RDA5807 �Ĵ���  д
static uint8_t RDA_reg_data[8] =
{
   0xd0,0x00,  // 02H
   0x00,0x00,  // 03H
   0x00,0x40,  // 04H
   0x90,0x88,  // 05H
};

/**
  * @brief  RDA5807_write_reg ����д�Ĵ���
  * @param  ��  
  * @retval ��
  */
static void RDA5807_write_reg(void)
{
	uint8_t i;
	uint8_t ucAck;
	
	i2c_Start();
	
	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(RDA5807M_SLAVE_ADDRESS | I2C_WR);
	
	/* ���ACK */
    ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		goto err_quit;
	}
	
	// �Ĵ�������д����
	for(i=0; i<8; i++)
	{
		i2c_SendByte(RDA_reg_data[i]);
		
		/* ���ACK */
		ucAck = i2c_WaitAck();
		if (ucAck == 1)
		{
			goto err_quit;
		}
	}
	
err_quit:
	i2c_Stop();
}

/**
  * @brief  RDA5807_read_reg �������Ĵ���
  * @param  ��  
  * @retval ��
  */
void RDA5807_read_reg(uint8_t *reg_buf)
{
	uint8_t i;
	uint8_t ucAck;

	i2c_Start();

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(RDA5807M_SLAVE_ADDRESS | I2C_RD);

	/* ���ACK */
	ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		goto err_quit;
	}

	for (i = 0; i < 4; i++)
	{
		reg_buf[i] = i2c_ReadByte();
		if (i == 3)
		{
			i2c_NAck();
		}
		else
		{                        
			i2c_Ack();
		}
	}

err_quit:
	i2c_Stop();
}

/**
  * @brief  RDA5807_power ����ģ���ϵ磬��λ
  * @param  ��  
  * @retval ��
  */
static void RDA5807_power(void)
{
	 Delay_ms(50);
	
	 // ���������λָ��
   RDA_reg_data[0] = 0x00;
   RDA_reg_data[1] = 0x02;
   RDA5807_write_reg();

   Delay_ms(10);

   // ����ģ��Ĭ�ϲ���
   RDA_reg_data[0] = 0xd0;
   RDA_reg_data[1] = 0x01;
   RDA5807_write_reg();
}

/**
  * @brief  RDA5807_FM_seek �Զ�Ѱ̨
  * @param  ��  
  * @retval ��
  */
static void RDA5807_FM_seek(void)
{
   
   uint8_t  reg_data[4] = {0x00, 0x00, 0x00, 0x00};

   RDA_reg_data[3] &= ~(1 << 4);      //��г����

   // �ڲ��Զ�Ѱ̨ʹ��
   RDA_reg_data[0] |=  (1 << 0);      //SEEKλ��1
   RDA5807_write_reg();

    // �ȴ�STC ��־��λ
   while(0 == (reg_data[0] & 0x40))
   {
     Delay_ms(40);
      // ��ȡ�ڲ�״̬
     RDA5807_read_reg(reg_data);
	 
   }
    // ��ȡ��ǰ����Ƶ��
   chan = reg_data[0] & 0x03;
   chan = reg_data[1] | (chan << 8);
   chan = chan << 6;
   
   printf("\r\n Ѱ̨�ɹ� \r\n");
   printf("��ǰ����Ƶ��Ϊ��%d" , chan);

    // ���浱ǰ����Ƶ��
   RDA_reg_data[2] = (chan >> 8) & 0xff;
   RDA_reg_data[3] = (chan & 0xff);
}

/**
  * @brief  RDA5807_FM_Test ������ģ�����
  * @param  ��  
  * @retval ��
  */
void RDA5807_FM_Test(void)
{
	/* ��ʾFM����������ͼƬ */
	Lcd_show_bmp(0, 0,"/radio/ui_window.bmp");
  /* config the led */
	LED1_ON;
	LED2_ON;	

	printf("\r\n rda5807m ����ʵ�� \r\n");
	
	if ( i2c_CheckDevice(RDA5807M_SLAVE_ADDRESS) == 0 )
	{
		printf("\r\n rda5807m ���ɹ� \r\n");
	}
	else
	{
		printf("\r\n rda5807m ��ⲻ�ɹ� \r\n");
	}
	/* ϵͳ��ʱ�� 1us ��ʱ��ʼ�� */
	 SysTick_Init();	
	
	/* �ϵ縴λ */
	RDA5807_power();
	
	printf("\r\n �ϵ�ɹ� \r\n");
	
	/* ���ó�ʼ���� */
	RDA5807_Volume_Set(5);

	/* ��Ѱ��̨ */
	RDA5807_Seek_Next();
	
	while(1)
	{	
		if( Key_Scan(GPIOC,GPIO_Pin_13) == KEY_ON  )	//ɨ�谴��
		{			
			touch_even_fm = EF_SEEK_NEXT;			
			/*��ת*/
			LED1_TOGGLE;
		}  

		if( Key_Scan(GPIOA,GPIO_Pin_0) == KEY_ON  )
		{			
			touch_even_fm = EF_SEEK_PREV;			
			/*��ת*/
			LED1_TOGGLE;
		}
		
		/* �����¼� */
		even_process();	
		
  }
}



/**
  * @brief  ��������ֵ
  * @param  vol:	0~15  
  * @retval ��
  */

void RDA5807_Volume_Set(uint8_t vol)
{
	volume = vol ;
	RDA_reg_data[7] = (RDA_reg_data[7] & 0xF0)|(volume & 0x0F);			// 05H�Ĵ�������bit �ı�����
	RDA5807_write_reg();
	printf("��ǰ����Ϊ%d" , volume);

}

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
void RDA5807_Volume_Add(void)
{
	if(volume>= 0x0F)
		volume=0x0F;
	else
		volume++;
	
	RDA_reg_data[7] = (RDA_reg_data[7] & 0xF0)|(volume & 0x0F);			// 05H�Ĵ�������bit �ı�����
	RDA5807_write_reg();
	printf("��ǰ����Ϊ%d" , volume);
}


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
void RDA5807_Volume_Dec(void)
{
	if(volume<= 0)
		volume=0;
	else
		volume--;
	
	RDA_reg_data[7] = (RDA_reg_data[7] & 0xF0)|(volume & 0x0F);			// 05H�Ĵ�������bit �ı�����
	RDA5807_write_reg();
	printf("��ǰ����Ϊ%d" , volume);
}

/**
  * @brief  ����Ѱ̨ seek up ��Ƶ�����ӣ�
  * @param  ��  
  * @retval ��
  */
void RDA5807_Seek_Next(void)
{
	RDA_reg_data[0] |= (1 << 1); 	     //������̨	
	RDA5807_FM_seek();                 //�Զ�Ѱ̨
}

/**
  * @brief  ����Ѱ̨ seek down��Ƶ���С��
  * @param  ��  
  * @retval ��
  */
void RDA5807_Seek_Prev(void)
{
	RDA_reg_data[0] &= ~(1 << 1); 	    //������̨
	RDA5807_FM_seek();                  //�Զ�Ѱ̨
}


/**
  * @brief  �����¼�
  * @param  ��  
  * @retval ��
  */
static void even_process(void)
{
	char lcd_char[20];
	
	switch(touch_even_fm)
	{
		case EF_VOL_ADD:
			RDA5807_Volume_Add();			
		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"����:%2d",volume);		//��ʾ������LCD��
			LCD_DispEnCh(20,200,lcd_char,BLUE);
		
			touch_even_fm = EF_NULL;
		
			break;
		
		case EF_VOL_DEC:
			RDA5807_Volume_Dec();
			
		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"����:%2d",volume);		//��ʾ������LCD��
			LCD_DispEnCh(20,200,lcd_char,BLUE);
		
			touch_even_fm = EF_NULL;
		
			break;
				
		case EF_SEEK_PREV:													//����Ѱ̨
			RDA5807_Seek_Prev();
		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"����Ƶ��:%6d",chan);	//��ʾƵ�㵽LCD��
			LCD_DispEnCh(20,150,lcd_char,BLUE);
		
			touch_even_fm = EF_NULL;
		
			break;
		
		case EF_SEEK_NEXT:													//����Ѱ̨			
			RDA5807_Seek_Next();
		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"����Ƶ��:%6d",chan);	//��ʾƵ�㵽LCD��
			LCD_DispEnCh(20,150,lcd_char,BLUE);
		
			touch_even_fm = EF_NULL;
		
			break;
		
		case EF_REFRESH:														//ˢ����Ļ		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"����Ƶ��:%6d",chan);	//��ʾ����Ƶ��
			LCD_DispEnCh(20,150,lcd_char,BLUE);
		
			sprintf(lcd_char,"����:%2d",volume);		//��ʾ������LCD��
			LCD_DispEnCh(20,200,lcd_char,BLUE);
		
			touch_even_fm = EF_NULL;
		
		default:
			break;
	
		}

}

/**
  * @brief  touch_process�жϷ����������ݴ���λ�ø��´�����־
  * @param  ��  
  * @retval ��
  */
void touch_process_fm (void)
{
		 if(touch_flag == 1)															/*������ʰ�����*/
    {		
      /* ��ȡ������� */
      if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
      {		
        LED1_TOGGLE;   																//LED��ת	 
			//	printf("\r\n x=%d,y=%d",display.x,display.y);
				
				if(display.x>=46 && display.x<=78)						//�������ķ�Χ
				{
						if(display.y>=18 && display.y<=73)				//����Ѱ̨
						{
								touch_even_fm = EF_SEEK_PREV;
								printf("\r\n prev");
							}
						else if(display.y>=84 && display.y<=139)	//����Ѱ̨	
						{
								touch_even_fm = EF_SEEK_NEXT;
								printf("\r\n next");
						}
						else if(display.y>=156 && display.y<=181)	//������
						{
								touch_even_fm = EF_VOL_ADD;
								printf("\r\n vol add");
						}
						else if(display.y>=185 && display.y<=210)	//������
						{
								touch_even_fm = EF_VOL_DEC;
								printf("\r\n vol dec");
						}
				}
			}				
		}

}



/*********************************************END OF FILE**********************/

