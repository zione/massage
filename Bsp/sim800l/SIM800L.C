#include <stddef.h>
#include <stdlib.h>

#include "USART1_init.h"
#include "USART2_init.h"
#include "stdio.h"
#include "delay.h"
#include "SIM800L.h"

#define SIM_BUFF_SIZEMAX 256
static char SIM_Buffer[SIM_BUFF_SIZEMAX];	//���ڽ��ջ����� 

const char *port = "1883";																	//�˿ڹ̶�Ϊ1883
const char *ipaddr = "a11fsZZPcNJ.iot-as-mqtt.cn-shanghai.aliyuncs.com";

/******************************************************************************
* ������		: Sim_ini
* ��������  	: SIM800C��ʼ��
* �������  	: 
* ������  	: 
* ����ֵ    	: 
******************************************************************************/  
void Sim_ini(void)
{
	while(1)
	{
		IGT_Lo;
		delay_ms(2500);
		IGT_Hi;   //gprsģ���ϵ�
		delay_ms(1000);
		
		while(sim800c_send_cmd("AT","OK",100))//����������Ӧ
		{
			printf("δ��⵽ģ��!!!");
			delay_ms(800);
			printf("��������ģ��...");
			delay_ms(400);  
		}
		printf("����ģ��ɹ�...");
		printf("��ʼGPRS����...");
		if(Start_Gprs_TCP() == 0)
		{
			break;
		}
		printf("GPRS����ʧ��,���¿�ʼ����...");
	}
}

/******************************************************************************
* ������		: RCC_Configuration
* ��������  	: SIM800C������
* �������  	: cmd:���͵������ַ���(����Ҫ���ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
*								ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
*								waittime:�ȴ�ʱ��(��λ:10ms)
* ������  	: 
* ����ֵ    	: 0,���ͳɹ�(�õ����ڴ���Ӧ����)  1,����ʧ��
******************************************************************************/  
u8 sim800c_send_cmd(char *cmd,char *ack,u16 waittime)
{
	u8 res=0; 
	u16 length = 0;
	if((u32)cmd<=0XFF)
	{
		while((USART2->SR&0X40)==0);//�ȴ���һ�����ݷ������  
		USART2->DR=(u32)cmd;
	}else{
		USART2_printf("%s\r\n",cmd);//��������
	}
	
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{ 
			delay_ms(10);
			length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
			if(length)//���յ��ڴ���Ӧ����
			{
				SIM_Buffer[length]=0;//���ӽ�����
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
* ������		: sim800c_send_data
* ��������  	: SIM800C��������
* �������  	: data:���͵�����
*								lenth������
* ������  	: 
* ����ֵ    	: 0,���ͳɹ�(�õ����ڴ���Ӧ����)  1,����ʧ��
******************************************************************************/
uint8_t sim800c_send_data(char *data,u8 lenth)
{
	if(sim800c_send_cmd("AT+CIPSEND",">",500)==0)		//��������
	{
		USART2_SendString(data,lenth);
		delay_ms(10);
		if(sim800c_send_cmd((char*)0X1A,"SEND OK",5000)==0)
			return 0;
		else
			return 1;
	}
	else {
		sim800c_send_cmd((char*)0X1B,0,0);	//ESC,ȡ������
		return 1;
	}
}



/******************************************************************************
* ������		: Start_Gprs_TCP
* ��������  	: SIM800C��gprs��ʼ��
* �������  	: 					
* ������  	: 
* ����ֵ    	: 0,���ͳɹ�(�õ����ڴ���Ӧ����)  1,����ʧ��
******************************************************************************/
uint8_t Start_Gprs_TCP(void)
{
/*	sim800c_send_cmd("ATE0","OK",200);										//������
	sim800c_send_cmd("AT+IFC=0,0","OK",200);
	sim800c_send_cmd("AT+CSCLK=0","OK",200);
	sim800c_send_cmd("AT+CBC","OK",200);
	sim800c_send_cmd("AT+CPIN?","OK",200);
	sim800c_send_cmd("AT+CSQ","OK",200);
	sim800c_send_cmd("AT+CFGRI=0","OK",200);
	sim800c_send_cmd("AT+CREG=1","OK",200);
	sim800c_send_cmd("AT+CGREG=1","OK",200);
	if(Creg_CK()){
		printf("\r\n=======����ע��ʧ��=======\r\n");
		return 0x01;
	}
	printf("\r\n=======����ע��ɹ�=======\r\n");
	sim800c_send_cmd("AT+CGATT=1","OK",100);	
	sim800c_send_cmd("AT+CGREG?","OK",100);	
	sim800c_send_cmd("AT+CGATT?","OK",100);	
	sim800c_send_cmd("AT+CIPRXGET=1","OK",100);		
	sim800c_send_cmd("AT+CIPSHUT","OK",100);		
	sim800c_send_cmd("AT+CSTT=\"CMNET\"","OK",100);	
	sim800c_send_cmd("AT+CIICR","OK",100);			
	sim800c_send_cmd("AT+CIFSR",0,100);			
	sim800c_send_cmd("AT+CIPQSEND=0","OK",100);*/

	
	sim800c_send_cmd("AT+CIPCLOSE=1","OK",2);	//�ر�����
  delay_ms(100);
	sim800c_send_cmd("AT+CIPSHUT","SHUT OK",2);		//�ر��ƶ�����
	sim800c_send_cmd("AT+CGCLASS=\"B\"","OK",2);//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
	sim800c_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",2);//����PDP������,��������Э��,��������Ϣ
	sim800c_send_cmd("AT+CGATT=1","OK",2);//����GPRSҵ��
	sim800c_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",2);//����ΪGPRS����ģʽ
  sim800c_send_cmd("AT+CIPMUX=0","OK",2);//����Ϊ��·����
	sim800c_send_cmd("AT+CIPMODE=1","OK",2);//��͸������
	sim800c_send_cmd("AT+CIPCCFG=4,5,200,1","OK",2);//����͸��ģʽ�������ط�����:2,���1S����һ��,ÿ�η���200���ֽ�
	Creg_CK();
	return connect_server();
}

//�ж��ƶ�����ע��״̬
//�ɹ�0��ʧ�ܣ�1
uint8_t Creg_CK(void)
{
	uint8_t i,j;
	u16 length = 0;
	i=0;
	do{	
		USART2_printf("%s\r\n","AT+CREG?");//��������
		i++;
		delay_ms(500);
		length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
		if(length)//���յ��ڴ���Ӧ����
		{
			SIM_Buffer[length]=0;//���ӽ�����
		}
	}while((strstr((const char*)SIM_Buffer,(const char*)"+CREG: 1,5")==NULL) && (strstr((const char*)SIM_Buffer,(const char*)"+CREG: 1,1")==NULL)&& i<30);
	
	j=0;
	do{
		USART2_printf("%s\r\n","AT+CGREG?");//��������
		j++;
		delay_ms(500);
		length = USART2_GetData(SIM_Buffer,SIM_BUFF_SIZEMAX);
		if(length)//���յ��ڴ���Ӧ����
		{
			SIM_Buffer[length]=0;//���ӽ�����
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
	if(0==sim800c_send_cmd(p2,"OK",500))														//��������
	{
		i=0;
		while(i < 5){
			i++;
			delay_ms(500);
			sim800c_send_cmd("AT+CIPSTATUS","OK",500);										//��ѯ����״̬
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

