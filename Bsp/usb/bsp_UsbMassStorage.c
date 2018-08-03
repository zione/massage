#include "bsp_UsbMassStorage.h"
#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart1.h"	
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"
#include "bsp_led.h"  
#include "app_ui.h"

#ifdef LCD_DISPLAY
	#include "bsp_ili9341_lcd.h"
#endif

void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

void UsbMassStorage_Test(void)
{
	 /* USART1 config */
	//USART1_Config();
	
  LED_GPIO_Config();
  
	/*��ʼ��SD��*/
	Set_System();
  	
	/*����USBʱ��Ϊ48M*/
	Set_USBClock();
 	
	/*����USB�ж�(����SDIO�ж�)*/
	USB_Interrupts_Config();
 
	/*USB��ʼ��*/
 	USB_Init();
 
 	while (bDeviceState != CONFIGURED);	 //�ȴ��������
	   
	printf("\r\n Ұ�� ISO STM32 USB MASS STORAGE ʵ��\r\n");
	#ifdef LCD_DISPLAY
			LCD_Clear(20,195,206,32,BACKGROUND);			//�����Ļ
			LCD_DispEnCh(20,195,"USB ʵ����Գɹ�!",BLUE);		
	#endif
  while (1)
  {
    LED_TOGGLE;

    USB_Delay(0x0FFFFF);
  }
}
/* -------------------------------------------end of file------------------------------------------ */
