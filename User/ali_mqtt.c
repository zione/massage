#include "ali_mqtt.h"
#include "stdio.h"
#include "delay.h"
#include <stddef.h>
#include <stdlib.h>
#include "TIMER.h"
#include "bsp_led.h"
#include "SIM800L.h"
#include "MQTTPacket.h"
#include "USART2_init.h"
#include <string.h>
#include "transport.h"
 
unsigned char buf[MQTT_BUFF_LEN];
unsigned short msgid = 1;

int sub_qos[] = {1};
MQTTString sub_topics[] = {{SUB_TOPIC_STRING, {0, NULL}}};
MQTTString send_topics = {SEND_TOPIC_STRING, {0, NULL}};

int preLedStatus;		//led�Ƶ���ǰ״̬
int force_send = 0; //ǿ�Ʒ���
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
	if(0==sim800c_send_cmd(p2,"OK",500))													
	{
		i=0;
		while(i < 5){
			i++;
			delay_ms(500);
			if(sim800c_send_cmd("AT+CIPSTATUS","CONNECT OK",500) == 0)				
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
	int len = 0;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID.cstring = CLIENT_ID; 
	data.username.cstring = USER_NAME;
	data.password.cstring = PASSWORD;
	data.keepAliveInterval = ALIVE_TIME;
	data.cleansession = 1; 
	
	printf("mqtt send connect\n");
	len = MQTTSerialize_connect(buf, MQTT_BUFF_LEN, &data);
	transport_sendPacketBuffer(0, buf, len);
	
		/* wait for connack */
	if (MQTTPacket_read(buf, MQTT_BUFF_LEN, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, MQTT_BUFF_LEN) != 1 || connack_rc != 0)
		{
			printf("Unable to connect, return code %d\n", connack_rc);
			return 1;
		}
		flag_conn = 1;
		printf("mqtt connect ok\n");
		return 0;
	}
	else{
		printf("mqtt connect fail\n");
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
	int len = 0;
	printf("mqtt send subscrib\n");
	len = MQTTSerialize_subscribe(buf, MQTT_BUFF_LEN, 0, msgid, sizeof(sub_qos)/sizeof(sub_qos[0]), sub_topics, sub_qos);
	transport_sendPacketBuffer(0, buf, len);
	
	if (MQTTPacket_read(buf, MQTT_BUFF_LEN, transport_getdata) == SUBACK) 
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		MQTTDeserialize_suback(&submsgid, sizeof(sub_qos)/sizeof(sub_qos[0]), &subcount, &granted_qos, buf, MQTT_BUFF_LEN);
		if (granted_qos != 1)
		{
			printf("granted qos != 0, %d\n", granted_qos);
			return 1;
		}
		flag_sub = 1;
		printf("mqtt subscrib ok\n");
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
* �������  	: buf,buf_l,dup,qos,retained,packetid,
* ������  	: 
* ����ֵ    	: 0,�ɹ� 1,ʧ��
******************************************************************************/ 
uint8_t send_publish_packet(void)
{
	int len;
	char* payload = STATUS_ON;
	len = MQTTSerialize_publish(buf, MQTT_BUFF_LEN, 0, 1, 0, msgid, send_topics, (unsigned char*)payload, strlen(payload));
	transport_sendPacketBuffer(0, buf, len);
	resetPingCount();
	printf("send pub...\r\n");
	return 0;
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
	int len;
	printf("send pub ack\r\n");
	len = MQTTSerialize_puback(buf, MQTT_BUFF_LEN,mid);
	transport_sendPacketBuffer(0, buf, len);
	resetPingCount();
	return 0;
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
	int len;
	printf("ping...\r\n");
	len = MQTTSerialize_pingreq(buf, MQTT_BUFF_LEN);
	transport_sendPacketBuffer(0, buf, len);
	resetPingCount();
	return 0;
}


/******************************************************************************
* ������		: check_connect_close
* ��������  	: ��ѯ�Ƿ������
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,���� 1,����
******************************************************************************/ 
uint8_t check_connect_close(void)
{
	if(has_rcv_closed()){
		printf("disconnect...");
		flag_server = 0; 
		flag_conn = 0;  
		flag_sub = 0; 
		return 1;
	}else{
		return 0;
	}
}

/******************************************************************************
* ������		: check_to_ping
* ��������  	: ����Ƿ�÷���ping��
* �������  	: 
* ������  	: 
* ����ֵ    	: 1,�� 0,����
******************************************************************************/ 
uint8_t check_to_ping(){
	return isPingCountEnd();
}


/******************************************************************************
* ������		: check_to_publish
* ��������  	: ����Ƿ�÷��ͷ�������led״̬�仯�͸÷��ͣ�û�ӵ�ackҲ�ط�
* �������  	: 
* ������  	: 
* ����ֵ    	: 1,�� 0,����
******************************************************************************/ 
uint8_t check_to_publish(){
	return force_send || preLedStatus != getLedStatus();
}

/******************************************************************************
* ������		: parse_payload
* ��������  	: �����������·��Ŀ����������ҵ�� {value:12345}
* �������  	: 
* ������  	: 
* ����ֵ    	: 
******************************************************************************/ 
void parse_payload(unsigned char* payload,int length){
	int value = -1;
	char* start = 0;
	char* end = 0;
	int len = 0;
	char arr[32];
	memcpy(arr,payload,length);
	arr[length] = '\0';
	printf("recv: %d %s\r\n",length,arr);
	if(strlen(arr) > 0){
		start = strstr(arr,":")+1;
		end = strstr(arr,"}");
		len = end - start;
		strncpy(arr,start,len);
		value = atoi(arr);
	}
	if(value >= 0){
		setSwitchTime(value);
	}
}
/******************************************************************************
* ������		: recv_mqtt
* ��������  	: ����mqtt��
* �������  	: 
* ������  	: 
* ����ֵ    	:
******************************************************************************/ 
void recv_mqtt(){
	int qos;
	int payloadlen_in;
	unsigned char* payload_in;
	unsigned char dup;
	unsigned char retained;
	unsigned short rcv_msgid;
	MQTTString receivedTopic;
	unsigned char packettype;
	
	switch(MQTTPacket_read(buf, MQTT_BUFF_LEN, transport_getdata)){
		case PUBLISH:
			if(MQTTDeserialize_publish(&dup, &qos, &retained, &rcv_msgid, &receivedTopic,
							&payload_in, &payloadlen_in, buf, MQTT_BUFF_LEN)){
				parse_payload(payload_in,payloadlen_in);
				if(qos == 1){
					send_puback_packet(rcv_msgid);
				}else if(qos == 2){
					//��������ʱ��֧��
				}else{
					//0�����ûظ�
				}
			}
			break;
		case PUBACK:
			if(MQTTDeserialize_ack(&packettype,&dup,&rcv_msgid, buf, MQTT_BUFF_LEN)){
				if(rcv_msgid == msgid){ //���͵���Ϣȷ�ϱ��������յ�
					force_send = 0;
					msgid = (msgid+1)%100;
					printf("stop send\r\n");
				}else{
					force_send = 1;  //û�ɹ���������
				}
			}
			printf("rcv pub ack\r\n");
			break;
		case PINGRESP:
			printf("rcv ping ack\r\n");
			break;
		default:
			printf("rcv no\r\n");
			break;
	}
}


/******************************************************************************
* ������		: handle_mqtt
* ��������  	: ����mqtt��һ���������ӣ����գ�Ӧ��ȣ�
* �������  	: 
* ������  	: 
* ����ֵ    	: 0,���ӳɹ�(�õ����ڴ���Ӧ����)  1,����ʧ��
******************************************************************************/  
void handle_mqtt(void){
		if(check_connect_close()){   //��ѭ���в��ϼ���Ƿ����
			delay_ms(1000);
			return;
		}
	
		if(flag_server == 0){    //���ӷ����������ӹ��Ͳ������ӣ����Ƕ���
			if(connect_server()){
				delay_ms(1000);
				return;
			}
		}
		
		if(flag_conn == 0){			//�������Ӱ�
			if(send_connect_packet()){
				delay_ms(1000);
				return;
			}
		}
		if(flag_sub == 0){      //����
			if(send_subscribe_packet()){
				delay_ms(1000);
				return;
			}
			preLedStatus = getLedStatus();
			//force_send = 1;     //��һ���ϵ��ʼ���ɹ�Ҫ�����ϱ�һ��
		}
		
		if(check_to_publish()){  //�÷��ͷ�����
			send_publish_packet();
		}
		
		if(check_to_ping()){   //�÷���ping��
			send_ping_packet();
		}
		
		recv_mqtt();
}
