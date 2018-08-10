#include <stddef.h>
#include <stdlib.h>

#include "USART1_init.h"
#include "USART2_init.h"
#include "stdio.h"
#include "delay.h"
#include "SIM800L.h"

#define SIM_BUFF_SIZEMAX 256
static char SIM_Buffer[SIM_BUFF_SIZEMAX];	//串口接收缓存区 

/******************************************************************************
* 函数名		: Sim_ini
* 函数描述  	: SIM800C初始化
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 
******************************************************************************/  
void Sim_ini(void)
{
	while(1)
	{
		delay_ms(2500);
		while(sim800c_send_cmd("AT","OK",100))//波特率自适应
		{
			printf("未检测到模块!!!");
			delay_ms(800);
			printf("尝试连接模块...");
			delay_ms(400);  
		}
		printf("开始GPRS配置...");
		if(Start_Gprs_TCP() == 0)
		{
			break;
		}
		printf("GPRS配置失败,重新开始配置...");
		IGT_Lo;
		delay_ms(2500);
		IGT_Hi;   //gprs模块上电
	}
}

/******************************************************************************
* 函数名		: RCC_Configuration
* 函数描述  	: SIM800C发送命
* 输入参数  	: cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
*								ack:期待的应答结果,如果为空,则表示不需要等待应答
*								waittime:等待时间(单位:10ms)
* 输出结果  	: 
* 返回值    	: 0,发送成功(得到了期待的应答结果)  1,发送失败
******************************************************************************/  
u8 sim800c_send_cmd(char *cmd,char *ack,u16 waittime)
{
	u8 res=0; 
	u16 length = 0;
	if((u32)cmd<=0XFF)
	{
		while((USART2->SR&0X40)==0);//等待上一次数据发送完成  
		USART2->DR=(u32)cmd;
	}else{
		USART2_printf("%s\r\n",cmd);//发送命令
	}
	
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{ 
			delay_ms(1);
			length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
			if(length)//接收到期待的应答结果
			{
				SIM_Buffer[length]='\0';//添加结束符
				if(strstr((const char*)SIM_Buffer,(const char*)ack)){
					break;
				}
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

/******************************************************************************
* 函数名		: sim800c_send_data
* 函数描述  	: SIM800C发送数据
* 输入参数  	: data:发送的数据
*								lenth：长度
* 输出结果  	: 
* 返回值    	: 0,发送成功(得到了期待的应答结果)  1,发送失败
******************************************************************************/
uint8_t sim800c_send_data(char *data,u8 lenth)
{
	if(sim800c_send_cmd("AT+CIPSEND",">",20)==0)		//发送数据
	{
		USART2_SendString(data,lenth);
		if(sim800c_send_cmd((char*)0X1A,"OK",500)==0){
			return 0;
		}
		else{
			return 1;
		}
	}
	else {
		sim800c_send_cmd((char*)0X1B,0,0);	//ESC,取消发送
		return 1;
	}
}
/******************************************************************************
* 函数名		: Start_Gprs_TCP
* 函数描述  	: SIM800C的gprs初始化
* 输入参数  	: 					
* 输出结果  	: 
* 返回值    	: 0,发送成功(得到了期待的应答结果)  1,发送失败
******************************************************************************/
uint8_t Start_Gprs_TCP(void)
{
	sim800c_send_cmd("ATE0","OK",20);
	sim800c_send_cmd("AT+CIPCLOSE=1","OK",20);	//关闭连接
  delay_ms(500);
	sim800c_send_cmd("AT+CIPSHUT","SHUT OK",20);		//关闭移动场景
	sim800c_send_cmd("AT+CSQ","OK",20);	//信号强度
	sim800c_send_cmd("AT+CREG=1","OK",20);
	sim800c_send_cmd("AT+CGREG=1","OK",20);
	sim800c_send_cmd("AT+CGCLASS=\"B\"","OK",20);//设置GPRS移动台类别为B,支持包交换和数据交换 
	sim800c_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",20);//设置PDP上下文,互联网接协议,接入点等信息
	sim800c_send_cmd("AT+CGATT=1","OK",20);//附着GPRS业务
	sim800c_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",20);//设置为GPRS连接模式
  sim800c_send_cmd("AT+CIPMUX=0","OK",20);//设置为单路连接
	sim800c_send_cmd("AT+CIPMODE=1","OK",20);//打开透传功能
	sim800c_send_cmd("AT+CIPCCFG=4,5,256,1","OK",20);//配置透传模式：单包重发次数:2,间隔1S发送一次,每次发送200的字节
	return wait_reg();
}

/******************************************************************************
* 函数名		: wait_reg
* 函数描述  	: 等待SIM800C注册成功
* 输入参数  	: 					
* 输出结果  	: 
* 返回值    	: 0,成功 1,失败
******************************************************************************/
uint8_t wait_reg(void)
{
	uint8_t i=0,j=0,retry_max = 30;
	u16 length;

	do{	
		USART2_printf("%s\r\n","AT+CREG?");//发送命令
		i++;
		delay_ms(1000);
		length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
		if(length)//接收到期待的应答结果
		{
			SIM_Buffer[length]='\0';//添加结束符
		}
	}while((strstr((const char*)SIM_Buffer,(const char*)"+CREG: 1,5")==NULL) && (strstr((const char*)SIM_Buffer,(const char*)"+CREG: 1,1")==NULL)&& i<retry_max);
	
	do{
		USART2_printf("%s\r\n","AT+CGREG?");//发送命令
		j++;
		delay_ms(1000);
		length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
		if(length)//接收到期待的应答结果
		{
			SIM_Buffer[length]=0;//添加结束符
		}
	}while((strstr((const char*)SIM_Buffer,(const char*)"+CGREG: 1,5")==NULL) && (strstr((const char*)SIM_Buffer,(const char*)"+CGREG: 1,1")==NULL)&& j<retry_max);
	
	if(i<retry_max && j<retry_max){
		return 0x00;
	}else{
		return 0x01;
	}
}


