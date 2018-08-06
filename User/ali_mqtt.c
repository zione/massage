#include "ali_mqtt.h"
#include "stdio.h"
#include "delay.h"
#include <stddef.h>
#include <stdlib.h>
#include "SIM800L.h"
#include "MQTTPacket.h"
#include <string.h>
#include "transport.h"
 
MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
int mysock = 0;
unsigned char buf[256];
int buflen = sizeof(buf);
int msgid = 1;
MQTTString topicString = MQTTString_initializer;
int req_qos = 1;
char* payload = "mypayload";
int payloadlen = 0;
int len = 0;
int msg_type = 0;
int i=0;

unsigned char dup;
int qos;
unsigned char retained;
unsigned short pmsgid;
int payloadlen_in;
unsigned char* payload_in;
MQTTString receivedTopic;

int flag_server = 0;   //�Ƿ������˷�����
int flag_conn = 0;   //�Ƿ�����mqtt
int flag_sub = 0;   //�Ƿ��Ѿ�����
	
/******************************************************************************
* ������		: connect_server
* ��������  	: ���Ӱ���������������
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,���ӳɹ�(�õ����ڴ���Ӧ����)  1,����ʧ��
******************************************************************************/ 
uint8_t connect_server(void){
	char p2[100];
	uint8_t i;
	
	sprintf(p2,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"","TCP",IP_ADDR,PORT);
	if(0==sim800c_send_cmd(p2,"OK",500))														//��������
	{
		i=0;
		while(i < 5){
			i++;
			delay_ms(500);
			if(sim800c_send_cmd("AT+CIPSTATUS","CONNECT OK",500) == 0)										//��ѯ����״̬
			{
				printf("Connect server OK\r\n");
				flag_server = 1;
				return 0;
			}
		}
		return 1;
	}else{
			return 1;
	}
}

/******************************************************************************
* ������		: sent_connect_packet
* ��������  	: ����mqtt�����Ӱ�
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,�ɹ� 1,ʧ��
******************************************************************************/ 
uint8_t send_connect_packet(void)
{
	data.clientID.cstring = CLIENT_ID; 
	data.username.cstring = USER_NAME;
	data.password.cstring = PASSWORD;
	data.keepAliveInterval = ALIVE_TIME;
	data.cleansession = 1; 
	
	len = MQTTSerialize_connect(buf, buflen, &data);
	transport_sendPacketBuffer(mysock, buf, len);
	printf("mqtt send connect\n");
		/* wait for connack */
	if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
			printf("Unable to connect, return code %d\n", connack_rc);
			printf("mqtt connect fail1\n");
			return 1;
		}
		flag_conn = 1;
		return 0;
	}
	else{
		printf("mqtt connect fail2\n");
		return 1;
	}
}


/******************************************************************************
* ������		: sent_connect_packet
* ��������  	: ����mqtt�Ķ��İ�
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,�ɹ� 1,ʧ��
******************************************************************************/ 
uint8_t send_subscribe_packet(void)
{
	topicString.cstring = TOPIC_STRING;
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
	transport_sendPacketBuffer(mysock, buf, len);
	printf("mqtt send subscrib\n");
	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 1)
		{
			printf("granted qos != 0, %d\n", granted_qos);
			return 1;
		}
		flag_sub = 1;
		return 0;
	}
	else{
		printf("mqtt subscrib fail2\n");
		return 1;
	}
}


/******************************************************************************
* ������		: send_publish_packet
* ��������  	: ����mqtt�ķ�����
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,�ɹ� 1,ʧ��
******************************************************************************/ 
uint8_t send_publish_packet(void)
{
	payload = "mypayload";
	payloadlen = strlen(payload);
	len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	transport_sendPacketBuffer(mysock, buf, len);
}

/******************************************************************************
* ������		: sent_connect_packet
* ��������  	: ����mqtt�ķ�����Ӧ��
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,�ɹ� 1,ʧ��
******************************************************************************/ 
uint8_t send_puback_packet(int mid)
{
	len = MQTTSerialize_puback(buf, buflen,mid);
	transport_sendPacketBuffer(mysock, buf, len);
}

/******************************************************************************
* ������		: sent_ping_packet
* ��������  	: ����mqtt��ping��
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,�ɹ� 1,ʧ��
******************************************************************************/ 
uint8_t send_ping_packet(void)
{
	printf("ping...\n");
	len = MQTTSerialize_pingreq(buf, buflen);
	transport_sendPacketBuffer(mysock, buf, len);
}

/******************************************************************************
* ������		: handle_mqtt
* ��������  	: ����mqtt��һ���������ӣ����գ�Ӧ��ȣ�
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,���ӳɹ�(�õ����ڴ���Ӧ����)  1,����ʧ��
******************************************************************************/  
void handle_mqtt(void){
		if(flag_server == 0){
			if(connect_server()){
				delay_ms(1000);
				return;
			}
		}
		
		if(flag_conn == 0){
			if(send_connect_packet())
				delay_ms(1000);
				return;
			}
		}
		if(flag_sub == 0){
			if(send_subscribe_packet()){
				delay_ms(1000);
				return;
			}
		}
	
		
		msg_type = MQTTPacket_read(buf, buflen, transport_getdata);
		if (msg_type == PUBLISH)
		{
			
		}
		else if (msg_type == PINGRESP){
			printf("get Pong ack\n");
		}
}