#include <stddef.h>
#include <stdlib.h>

#include "USART1_init.h"
#include "USART2_init.h"
#include "stdio.h"
#include "delay.h"
#include "SIM800L.h"

#define SIM_BUFF_SIZEMAX 256
static char SIM_Buffer[SIM_BUFF_SIZEMAX];	//串口接收缓存区 

const char *port = "1883";																	//端口固定为1883
const char *ipaddr = "a11fsZZPcNJ.iot-as-mqtt.cn-shanghai.aliyuncs.com";

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
		IGT_Lo;
		delay_ms(2500);
		IGT_Hi;   //gprs模块上电
		delay_ms(1000);
		
		while(sim800c_send_cmd("AT","OK",100))//波特率自适应
		{
			printf("未检测到模块!!!");
			delay_ms(800);
			printf("尝试连接模块...");
			delay_ms(400);  
		}
		printf("连接模块成功...");
		printf("开始GPRS配置...");
		if(Start_Gprs_TCP() == 0)
		{
			break;
		}
		printf("GPRS配置失败,重新开始配置...");
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
			delay_ms(10);
			length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
			if(length)//接收到期待的应答结果
			{
				SIM_Buffer[length]=0;//添加结束符
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
	if(sim800c_send_cmd("AT+CIPSEND",">",500)==0)		//发送数据
	{
		USART2_SendString(data,lenth);
		delay_ms(10);
		if(sim800c_send_cmd((char*)0X1A,"SEND OK",5000)==0)
			return 0;
		else
			return 1;
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
/*	sim800c_send_cmd("ATE0","OK",200);										//不回显
	sim800c_send_cmd("AT+IFC=0,0","OK",200);
	sim800c_send_cmd("AT+CSCLK=0","OK",200);
	sim800c_send_cmd("AT+CBC","OK",200);
	sim800c_send_cmd("AT+CPIN?","OK",200);
	sim800c_send_cmd("AT+CSQ","OK",200);
	sim800c_send_cmd("AT+CFGRI=0","OK",200);
	sim800c_send_cmd("AT+CREG=1","OK",200);
	sim800c_send_cmd("AT+CGREG=1","OK",200);
	if(Creg_CK()){
		printf("\r\n=======网络注册失败=======\r\n");
		return 0x01;
	}
	printf("\r\n=======网络注册成功=======\r\n");
	sim800c_send_cmd("AT+CGATT=1","OK",100);	
	sim800c_send_cmd("AT+CGREG?","OK",100);	
	sim800c_send_cmd("AT+CGATT?","OK",100);	
	sim800c_send_cmd("AT+CIPRXGET=1","OK",100);		
	sim800c_send_cmd("AT+CIPSHUT","OK",100);		
	sim800c_send_cmd("AT+CSTT=\"CMNET\"","OK",100);	
	sim800c_send_cmd("AT+CIICR","OK",100);			
	sim800c_send_cmd("AT+CIFSR",0,100);			
	sim800c_send_cmd("AT+CIPQSEND=0","OK",100);*/

	
	sim800c_send_cmd("AT+CIPCLOSE=1","OK",2);	//关闭连接
  delay_ms(100);
	sim800c_send_cmd("AT+CIPSHUT","SHUT OK",2);		//关闭移动场景
	sim800c_send_cmd("AT+CGCLASS=\"B\"","OK",2);//设置GPRS移动台类别为B,支持包交换和数据交换 
	sim800c_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",2);//设置PDP上下文,互联网接协议,接入点等信息
	sim800c_send_cmd("AT+CGATT=1","OK",2);//附着GPRS业务
	sim800c_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",2);//设置为GPRS连接模式
  sim800c_send_cmd("AT+CIPMUX=0","OK",2);//设置为单路连接
	sim800c_send_cmd("AT+CIPMODE=1","OK",2);//打开透传功能
	sim800c_send_cmd("AT+CIPCCFG=4,5,200,1","OK",2);//配置透传模式：单包重发次数:2,间隔1S发送一次,每次发送200的字节
	Creg_CK();
	return connect_server();
}

//判断移动网络注册状态
//成功0，失败：1
uint8_t Creg_CK(void)
{
	uint8_t i,j;
	u16 length = 0;
	i=0;
	do{	
		USART2_printf("%s\r\n","AT+CREG?");//发送命令
		i++;
		delay_ms(500);
		length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
		if(length)//接收到期待的应答结果
		{
			SIM_Buffer[length]=0;//添加结束符
		}
	}while((strstr((const char*)SIM_Buffer,(const char*)"+CREG: 1,5")==NULL) && (strstr((const char*)SIM_Buffer,(const char*)"+CREG: 1,1")==NULL)&& i<30);
	
	j=0;
	do{
		USART2_printf("%s\r\n","AT+CGREG?");//发送命令
		j++;
		delay_ms(500);
		length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
		if(length)//接收到期待的应答结果
		{
			SIM_Buffer[length]=0;//添加结束符
		}
	}while((strstr((const char*)SIM_Buffer,(const char*)"+CGREG: 1,5")==NULL) && (strstr((const char*)SIM_Buffer,(const char*)"+CGREG: 1,1")==NULL)&& i<30);
	
	if(i<30 && j<30){
		return 0x00;
	}else{
		return 0x01;
	}
}

uint8_t connect_server(void){
	char p2[100];
	uint8_t i;
	
	sprintf((char*)p2,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"","TCP",ipaddr,port);
	if(0==sim800c_send_cmd(p2,"OK",500))														//发起连接
	{
		i=0;
		while(i < 5){
			i++;
			delay_ms(500);
			sim800c_send_cmd("AT+CIPSTATUS","OK",500);										//查询连接状态
			if(strstr((const char*)SIM_Buffer,"CONNECT OK"))
			{
				printf("SIM800C TCP Connect server OK\r\n");
				return 0;
			}
		}
		return 1;
	}else{
			return 1;
	}
}


