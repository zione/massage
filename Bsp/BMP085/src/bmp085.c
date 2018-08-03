
/*******************************************************************************
*GY-80
*  �������ܣ����ڷ�������  HMC5883L	+ ADXL345 + BMP085+L3G4200D
*  оƬ��
*  �����ʣ�115200
*  ���뻷����Keil uVision4
*  ��дʱ�䣺
*  ��    �ߣ�
*******************************************************************************/

#include "stm32f10x.h"
#include  <math.h>    //Keil library  
#include "USART1_init.h"
#include "BMP085.h"

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;


//************************************************************************************************/
char  test1=0; 				 //IIC�õ�

#define   uchar unsigned char
#define   uint unsigned int	
#define   OSS 0	 //	BMP085ʹ��

//����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
#define	BMP085_Addr     0xee	//��ѹ������������ַ     ����

uchar ge,shi,bai,qian,wan,shiwan;           //��ʾ����

extern unsigned char hei[];

//***BMP085ʹ��
short ac1;
short ac2; 
short ac3; 
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1; 
short b2;
short mb;
short mc;
short md;

long  temperature;
long  pressure;
int height;
//********
 //************************************++++++++++++++++++++++++++++++++
/*ģ��IIC�˿�������붨��*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7

/* �������� -----------------------------------------------*/

void Delay(u32 nTime);
void Delayms(vu32 m);  
void conversion(long temp_data);


/* �������� ----------------------------------------------*/


//***************************************************
void conversion(long temp_data)  
{     
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //ȡ������ 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //ȡ������
		qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //ȡ������
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //ȡ������
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //ȡ������
    ge=temp_data+0x30; 	
}
/*******************************************************************************
* Function Name  : I2C_GPIO_Config
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{
		
   u8 i=30; //��������Ż��ٶ�	����������͵�5����д��
   while(i) 
   { 
     i--; 
   }  
}

void delay5ms(void)
{
		
   int i=5000;  
   while(i) 
   { 
     i--; 
   }  
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
bool I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;
	I2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
bool I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
	  I2C_delay();
      return FALSE;
	}
	SCL_L;
	I2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
unsigned char I2C_RadeByte(void)  //���ݴӸ�λ����λ//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
} 
//ZRX          
//���ֽ�д��*******************************************

bool Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	if(!I2C_Start())return FALSE;
    I2C_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
    I2C_SendByte(REG_Address );   //���õ���ʼ��ַ      
    I2C_WaitAck();	
    I2C_SendByte(REG_data);
    I2C_WaitAck();   
    I2C_Stop(); 
    delay5ms();
    return TRUE;
}

//���ֽڶ�ȡ*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
	if(!I2C_Start())return FALSE;
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop();test1=1; return FALSE;}
    I2C_SendByte((u8) REG_Address);   //���õ���ʼ��ַ      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress+1);
    I2C_WaitAck();

	REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return TRUE;
	return REG_data;

}						      

/*
********************************************************************************
** �������� �� Delay(vu32 nCount)
** �������� �� ��ʱ����
** ��    ��	�� ��
** ��    ��	�� ��
** ��    ��	�� ��
********************************************************************************
*/
 void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}

/*
********************************************************************************
** �������� �� void Delayms(vu32 m)
** �������� �� ����ʱ����	 m=1,��ʱ1ms
** ��    ��	�� ��
** ��    ��	�� ��
** ��    ��	�� ��
********************************************************************************
*/
 void Delayms(vu32 m)
{
  u32 i;
  
  for(; m != 0; m--)	
       for (i=0; i<50000; i++);
}


//********************************************************************
long bmp085ReadTemp(void)//���¶�
{   short  temp_ut;
	Single_Write(BMP085_Addr,0xF4,0x2E);//ѡ����BMP085ģ�飬��д����ʵ�ַ
	Delayms(5);	// max time is 4.5ms
	temp_ut = Single_Read(BMP085_Addr,0xF6);//��ȡ���ݸ߰�λ
	temp_ut = (temp_ut<<8)| Single_Read(BMP085_Addr,0xF7);//��ȡ���ݵͰ�λ
	return (long) temp_ut ;
}
//*************************************************************

long bmp085ReadPressure(void)//��ѹǿ
{
	long pressure = 0;
	Single_Write(BMP085_Addr,0xF4,0x34);
	Delayms(5);	// max time is 4.5ms
	pressure = Single_Read(BMP085_Addr,0xF6);
	pressure = (pressure<<8)| Single_Read(BMP085_Addr,0xF7);		
	pressure &= 0x0000FFFF;	
	return pressure;	
}

 //******************

//*************************************************
void Send_BMP085()
{
USART1_SendByte('B');
USART1_SendByte('M');
USART1_SendByte('P');
USART1_SendByte('0');
USART1_SendByte('8');
USART1_SendByte('5');
USART1_SendByte(':');

USART1_SendByte('t');
USART1_SendByte('=');
conversion(temperature);
USART1_SendByte(bai); 
USART1_SendByte(shi); 
USART1_SendByte('.');
USART1_SendByte(ge);
USART1_SendByte('`'); 
USART1_SendByte('C'); 

USART1_SendByte(' ');
USART1_SendByte('p');
USART1_SendByte('=');
conversion(pressure);

USART1_SendByte(shiwan);
USART1_SendByte(wan);
USART1_SendByte(qian);
USART1_SendByte('.');
USART1_SendByte(bai); 
USART1_SendByte(shi); 
USART1_SendByte(ge); 
USART1_SendByte('K'); 
USART1_SendByte('p'); 
USART1_SendByte('a');  				
USART1_SendByte(0x0d);
USART1_SendByte(0x0a);

}
//*****************************************************

//*****************************************
void read_BMP085(void)
{

  long ut;
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;

	ut = bmp085ReadTemp();	   // ��ȡ�¶�
	up = bmp085ReadPressure();  // ��ȡѹǿ
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	temperature = (b5 + 8) >> 4;
	
	 //****************

	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = (((long)ac1 * 4 + x3) + 2)/4;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
	b7 = ((unsigned long) up - b3) * (50000 >> OSS);
	if( b7 < 0x80000000)
	     p = (b7 * 2) / b4 ;
  else  
		   p = (b7 / b4) * 2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	pressure = p + ((x1 + x2 + 3791) >> 4);
}

//****************************
 void  Init_BMP085(void)
{
	ac1 = Single_Read(BMP085_Addr,0xAA);
	ac1 = (ac1<<8)|Single_Read(BMP085_Addr,0xAB);

  ac2 = Single_Read(BMP085_Addr,0xAC);
	ac2 = (ac2<<8)| Single_Read(BMP085_Addr,0xAD);

	ac3 = Single_Read(BMP085_Addr,0xAE);
	ac3 = (ac3<<8)| Single_Read(BMP085_Addr,0xAF);

	ac4 = Single_Read(BMP085_Addr,0xB0);
	ac4 = (ac4<<8)| Single_Read(BMP085_Addr,0xB1);

	ac5 = Single_Read(BMP085_Addr,0xB2);
	ac5 = (ac5<<8)| Single_Read(BMP085_Addr,0xB3);

	ac6 = Single_Read(BMP085_Addr,0xB4);
	ac6 = (ac6<<8)| Single_Read(BMP085_Addr,0xB5);

	b1 = Single_Read(BMP085_Addr,0xB6);
	b1 = (b1<<8)| Single_Read(BMP085_Addr,0xB7);

	b2 = Single_Read(BMP085_Addr,0xB8);
	b2 = (b2<<8)| Single_Read(BMP085_Addr,0xB9);

	mb = Single_Read(BMP085_Addr,0xBA);
	mb = (mb<<8)| Single_Read(BMP085_Addr,0xBB);

	mc = Single_Read(BMP085_Addr,0xBC);
	mc = (mc<<8)| Single_Read(BMP085_Addr,0xBD);

	md = Single_Read(BMP085_Addr,0xBE);
	md = (md<<8)| Single_Read(BMP085_Addr,0xBF);

}


/********************************************************************************
** �������� �� main(void)
** �������� �� ������
** ��    ��	�� ��
** ��    ��	�� ��
** ��    ��	�� ��
********************************************************************************
*/
void BMP_INI(void)
{ 
	unsigned char ht[5];
	uint8_t i;
	
	I2C_GPIO_Config();
	Init_BMP085();
 
	read_BMP085();		     // ��ѹǿ �¶�ֵ 
	Get_H();

	sprintf(ht,"%04d",height);
	
	Send_BMP085();           // ����ѹǿ�¶�ֵ
	DebugPf("Height=%s\r\n",ht);
//�߶����ݴ���hei[]����	
	for(i=0;i<4;i++)
	{
		hei[i+3]=ht[i];
	}
	for(i=3;i<7;i++){
		if(hei[i]=='-'){
			hei[i]='0';
			hei[3]='-';
		}
	}
}

void Get_H()
{
	double ans;  
	
//1
//	ans=log10((double)((double)101325)/(double)pressure);
//	ans=ans * (double)1840000;
//	ans=ans * (double)(1+(double)temperature/2730);
//	height=(double)(ans/100);
//2
//		ans=pow((double)pressure/(double)101325,0.19029);  
//		ans=(double)44330*(1-ans);//?????????
//		height=(long)(ans*100);//??100,???long int?,??????
//3
//	ans=log10((double)((double)101325)/(double)pressure);
//	ans=ans*(double)8.51;
//	ans=ans*(double)(temperature+273.15);
//	ans=(double)(ans/9.84);
//	ans=(double)(ans/29);
//	height=(long)(ans*100000);
//4	
		ans=(double)(log(pressure)+18.2573);
		ans=(double)(ans/(double)5.25885);
		ans=(double)pow((double)2.718282,ans);
		ans=(double)((double)288.15-ans);
		ans=(double)ans/(double)0.0065;
		height=(int)(ans);
		
}
/*************����***************/
