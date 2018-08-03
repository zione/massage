/**
  ******************************************************************************
  * @file    ble.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   bleӦ�ú����ӿ�
  ******************************************************************************
  */
    
#include "BLE.h"
#include "bsp_usart3.h"
#include "bsp_led.h"
#include "flash.h"
#include <string.h>
typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

static void delay(uint16_t t);
void BLE_Print(uint8_t len);
uint8_t Check_HD(unsigned char *s);
uint8_t Check_CK(unsigned char *addr);

unsigned char CentO[2]={0x00,0x00};//���ĵ��ֵ�洢����2Byte
unsigned char *MoShi1;//ģʽ1�洢��
unsigned char *MoShi2;//ģʽ2�洢��

extern uint8_t CentL,CentR;//����������ֵ�洢��Ԫ
extern uint32_t StartAddress;
extern uint32_t M1Address;
extern uint32_t M2Address;

extern bool FLAG_BLE_CTS;

extern unsigned char BLE_HD[4];
extern unsigned char BLE_TP[1];
extern unsigned char BLE_LEN[1];
extern unsigned char BLE_CK[1];
extern unsigned char BLE_END[1];
extern unsigned char BLE_DATA[];

/*****************************

*****************************/
static void delay(uint16_t t)
{
	uint16_t i;
	while(t--)
	{
		for(i=0;i<1000;i++);
	}
}
/**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void BLE_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		/*����LED������ʱ��*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE); 

//PA8--EN2
		/*ѡ��Ҫ���Ƶ�GPIOA����--EN2*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIOA8*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
//BTEN,BTRST
		/*ѡ��Ҫ���Ƶ�GPIOC����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;	
	
		/*���ÿ⺯������ʼ��GPIOC*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
//RTS
		/*ѡ��Ҫ���Ƶ�GPIOB����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	
	
		/*���ÿ⺯������ʼ��GPIOB*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
//CTS
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   

		GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ����PE1Ϊ�ⲿ�ж�1����ͨ����EXIT1�� */
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB , GPIO_PinSource13);
		EXTI_InitStructure.EXTI_Line = EXTI_Line13;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
		EXTI_Init(&EXTI_InitStructure);
	

		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		GPIO_SetBits(GPIOC, GPIO_Pin_1);
		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
			
}

//��ʼ�����ȴ�APP����
uint8_t BLE_Init(void)
{
	uint8_t lens[2];
	char *addr;
	char *c;
	uint16_t i,j;
  uint8_t bitstatus = 0x00;
	
	BLE_ON;
	BEN_ON;
	delay(8000);
	printf("%s","BLE Init:\r\n");
//MAC	
	RTS3_OFF;
	delay(20);
	USART3_printf(USART3,"TTM:MAC-?\r\n\0");
	delay(2);
	RTS3_ON;
	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));
	clean_rebuff3();
	while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));		
	delay(20);
	while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
	addr=get_rebuff3(lens);
	for(i=0;i<strlen(addr);i++)
	{
		if(addr[i]=='M' && addr[i+1]=='A')
		{
			break;
		}
	}
	
	printf("%s",addr);

	delay(1000);
//10s�ڵȴ��ֻ�APP��������
	i=0;
	do{
		if(i%100==0){
			LED_TOGGLE;
		}
		delay(10);
		i++;
	}while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)) && i<8000);
	
//������10s��BLE���ӣ�ֱ���˳��ó���
//�������BLEͨ��ģʽ
	while((!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13))){
		clean_rebuff3();
		while((!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)));	
		delay(20);
		while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
		addr=get_rebuff3(lens);
		do{
			LED_TOGGLE;
		}while(strstr(addr,"TTM:OK")==NULL);
		printf("%s",addr);
		delay(1000);
		

//�ȴ�APP��������CONNECT
		i=0;
		do{
			memset(addr,0,strlen(addr));
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));
			clean_rebuff3();
			while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
			delay(20);
			while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
			addr=get_rebuff3(lens);
			LED_TOGGLE;
			delay(100);	
			LED_TOGGLE;	
			delay(100);	
		}while((strstr(addr,"CN")==NULL));
		
		printf("%s",addr);
		
		RTS3_OFF;
		delay(20);
		BLE_TP[0]=0x01;
		BLE_LEN[0]=0x02;
		BLE_DATA[0]='O';
		BLE_DATA[1]='K';
		BLE_CK[0]=BLE_DATA[0] + BLE_DATA[1];
		BLE_Print(BLE_LEN[0]);
		delay(2);
		RTS3_ON;

		Wait_Back();//wait for feedback
		
		LED_TOGGLE;
		
		return bitstatus=0xff;
	}
	return bitstatus;
}
//���BLE�յ������ݣ�У��������Ч��
//��Ч������0x02
//δ������ɣ�����0x01
//��Ч������0x00
uint8_t BLE_Check(void)
{
	uint8_t lens[1];
	unsigned char *addr;
	uint16_t i,j;

	addr=get_rebuff3(lens);
	if(lens[0]>5){
		if(Check_HD(addr)==0x01)
		{
			LED_TOGGLE;
			if(lens[0]>=addr[5]+8)
			{
				if(Check_CK(addr)==0x01){				
					
					
					return 0x02;//��⵽�Ϸ�����
				
				}
			}else{
				return 0x01;//��⵽������С��ʵ�ʳ��ȣ����ּ������
			}		
		}
	}
	
	return 0x00;//��ͷ��Ч���������μ�⣬��մ��ڻ��嵥Ԫ
}

//����У�飺��ȷ����1�����󷵻�0
uint8_t Check_CK(unsigned char *addr)
{
	uint8_t i,j;
	unsigned char s=0x00;
	j=addr[5];//����
	for(i=0;i<j;i++)
	{
		s=s+addr[i+6];		
	}
	if(s==addr[6+j])
	{
		return 0x01;//У����ȷ
	}
	
	return 0x00;
}

uint8_t Check_HD(unsigned char *addr)
{

	if(addr[0]==0x24 && addr[1]==0x44 && addr[2]==0x54 && addr[3]==0x3A){
		return 0x01;
	}
	clean_rebuff3();
	return 0x00;
}

//��ȡ��������
uint8_t Type_Get()
{
	
	uint8_t lens[1];
	unsigned char *addr;
	uint16_t i,j;

	addr=get_rebuff3(lens);
	
	return addr[4];
	
}

//��ȡ�۾���ʼ����������������ֵ
uint8_t BLE_GetINI(void)
{
		uint8_t lens[2];
		char *addr;
		char *c;
		uint16_t i;
		uint8_t bitstatus = 0x00;
	
		do{
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));
			clean_rebuff3();
			while((!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)));	
			delay(20);
			while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
			addr=get_rebuff3(lens);
			LED_TOGGLE;
		}while(strstr(addr,"INI:")==NULL);
//�ж�CentL��CentRֵ��Ч�ԣ�������96����12mm		
		if((addr[4] <0x61) && (addr[5] < 0x61))
		{
			CentL=addr[4];
			CentR=addr[5];
			c="INI OK\r\n\0";
			for(i=0;i<lens[0];i++){
				USART_SendData(USART1, addr[i]);
				while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
			}
			bitstatus=0xff;
		}else
		{
			for(i=0;i<lens[0];i++){
				USART_SendData(USART1, addr[i]);
				while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
			}
			printf("%s","\r\n");
			c="Invalid Data!\r\n\0";
			for(i=0;i<strlen(c);i++){
				USART_SendData(USART1, c[i]);
				while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
			}
			bitstatus=0;
		}	

		delay(1000);
//Send BYTE to BLE		
		RTS3_OFF;
		delay(20);	
		for(i=0;i<strlen(c);i++){
			USART_SendData(USART3, c[i]);
			while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
		}
		delay(2);
		RTS3_ON;
//		
		LED_TOGGLE;
//�ȴ����ջ������ݲ����		
		Wait_Back();

		return bitstatus;
}

//��ȡģʽ1����
uint8_t Get_M1(void)
{
		uint8_t lens[2];
		char *addr;
		char *c;
		uint8_t ck;
		uint8_t i,j;
		uint8_t bitstatus = 0x00;
	
		do{
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));
			clean_rebuff3();
			while((!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)));	
			delay(10);
			while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
			addr=get_rebuff3(lens);
			LED_TOGGLE;
		}while((strstr(addr,"M1:")==NULL));
//
		c=strstr(addr,"M1:");
//��ȡ���ݳ��ȣ����ݳ���ָ�۾��������ֵ����ݳ���
		i=c[3];
//�Ա��۾����ݳ����봮�ڽ������ݳ��ȣ������������˵��δ�������
		j=0;
		do
		{
			delay(10);
			addr=get_rebuff3(lens);
			j++;
		}while(((i+6)!=(lens[0])) && (j<200));

//���۾��������ֵ����ݽ���У�飬У���������ck		
		ck=0x00;
		for(j=5;j<i+5;j++)
		{
			ck=ck+c[j];	
//			USART_SendData(USART1, c[j]);
//			while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
		}
//��У���������������һ�ֽ�У��λ���бȽ�
//����ͬ������к�������
		if(ck==c[i+5])
		{
			Writeflash(M1Address,c+3,i+2);
			c="M1 OK\r\n\0";
			bitstatus=0xff;
			
		}else
		{
			c="M1 ERR\r\n\0";
			bitstatus=0x00;
		}	

		delay(1000);
//Send BYTE to BLE		
		RTS3_OFF;
		delay(20);	
		for(i=0;i<strlen(c);i++){
			USART_SendData(USART3, c[i]);
			while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
		}
		delay(2);
		RTS3_ON;
//		
		LED_TOGGLE;
//�ȴ����ջ������ݲ����		
		Wait_Back();

		return bitstatus;
	
}

//��ȡģʽ2����
uint8_t Get_M2(void)
{
			uint8_t lens[2];
		char *addr;
		char *c;
		uint8_t ck;
		uint8_t i,j;
		uint8_t bitstatus = 0x00;
	
		do{
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));
			clean_rebuff3();
			while((!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)));	
			delay(20);
			while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
			addr=get_rebuff3(lens);
			LED_TOGGLE;
		}while(strstr(addr,"M2:")==NULL);
//
		c=strstr(addr,"M2:");
//��ȡ���ݳ��ȣ����ݳ���ָ�۾��������ֵ����ݳ���
		i=c[3];
//���۾��������ֵ����ݽ���У�飬У���������ck		
		ck=0x00;
		for(j=5;j<i+5;j++)
		{
			ck=ck+c[j];			
		}
//��У���������������һ�ֽ�У��λ���бȽ�
//����ͬ������к�������
		if(ck==c[i+5])
		{
			Writeflash(M2Address,c+3,i+2);
			c="M2 OK\r\n\0";
			bitstatus=0xff;
			
		}else
		{
			c="M2 ERR\r\n\0";
			bitstatus=0x00;
		}	

		delay(1000);
//Send BYTE to BLE		
		RTS3_OFF;
		delay(20);	
		for(i=0;i<strlen(c);i++){
			USART_SendData(USART3, c[i]);
			while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
		}
		delay(2);
		RTS3_ON;
//		
		LED_TOGGLE;
//�ȴ����ջ������ݲ����		
		Wait_Back();

		return bitstatus;	
}

//RUN
uint8_t Motor_RUN(void)
{
		
}
//BLE���ԣ��������ݲ�������APP
void BLE_TEST(void){
	
	uint8_t lens[2];
	char *addr;
	unsigned char i;
	
	while(1){
		strclr(addr);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));
		clean_rebuff3();
		while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
		delay(20);
		while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
		addr=get_rebuff3(lens);
		
		for(i=0;i<lens[0];i++){
			USART_SendData(USART1, addr[i]);
			while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
		}
//Send BYTE to BLE		
		RTS3_OFF;
		delay(20);	
		for(i=0;i<lens[0];i++){
			USART_SendData(USART3, addr[i]);
			while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
		}
		delay(2);
		RTS3_ON;
//		
		LED_TOGGLE;
//�ȴ����ջ������ݲ����		
		Wait_Back();
		
	}
}


void Wait_Back(void)
{
	uint8_t lens[2];
	char *addr;
	
	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));
	clean_rebuff3();
	while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
	delay(20);
	while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13));	
	addr=get_rebuff3(lens);
	delay(20);	
}

void strclr(char *s)
{
	unsigned char i;
	for(i=0;i<strlen(s);i++)
	{
		s=0x00;
		s++;
	}
}

void BLE_Print(uint8_t len)
{
	BLE_Send(BLE_HD,4);
	BLE_Send(BLE_TP,1);
	BLE_Send(BLE_LEN,1);
	BLE_Send(BLE_DATA,len);
	BLE_Send(BLE_CK,1);
	BLE_Send(BLE_END,1);
}