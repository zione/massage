#include "stm32f10x.h"

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

void I2C_GPIO_Config(void);
void I2C_delay(void);
//void delay5ms(void);
bool I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
bool I2C_WaitAck(void); 	 //����Ϊ:=1��ACK,=0��ACK
void I2C_SendByte(u8 SendByte); //���ݴӸ�λ����λ//
unsigned char I2C_RadeByte(void);  //���ݴӸ�λ����λ//
bool Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);		     //void
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address);
void Delayms(vu32 m);

 //************************************++++++++++++++++++++++++++++++++
/*ģ��IIC�˿�������붨��*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7
