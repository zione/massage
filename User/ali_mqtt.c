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

int preLedStatus;		//led灯的先前状态
int force_send = 0; //强制发送
int flag_server = 0;   //是否连上了服务器
int flag_conn = 0;   //是否连上mqtt
int flag_sub = 0;   //是否已经订阅
	
/******************************************************************************
* 函数名		: connect_server
* 函数描述  	: 连接阿里物联网服务器
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 0,连接成功(得到了期待的应答结果)  1,连接失败
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
* 函数名		: sent_connect_packet
* 函数描述  	: 发送mqtt的连接包
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 0,成功 1,失败
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
* 函数名		: sent_connect_packet
* 函数描述  	: 发送mqtt的订阅包
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 0,成功 1,失败
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
* 函数名		: send_publish_packet
* 函数描述  	: 发送mqtt的发布包
* 输入参数  	: buf,buf_l,dup,qos,retained,packetid,
* 输出结果  	: 
* 返回值    	: 0,成功 1,失败
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
* 函数名		: sent_connect_packet
* 函数描述  	: 发送mqtt的发布响应包
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 0,成功 1,失败
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
* 函数名		: sent_ping_packet
* 函数描述  	: 发送mqtt的ping包
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 0,成功 1,失败
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
* 函数名		: check_connect_close
* 函数描述  	: 查询是否掉线了
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 0,在线 1,掉线
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
* 函数名		: check_to_ping
* 函数描述  	: 检查是否该发送ping包
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 1,该 0,不该
******************************************************************************/ 
uint8_t check_to_ping(){
	return isPingCountEnd();
}


/******************************************************************************
* 函数名		: check_to_publish
* 函数描述  	: 检查是否该发送发布包，led状态变化就该发送，没接到ack也重发
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 1,该 0,不该
******************************************************************************/ 
uint8_t check_to_publish(){
	return force_send || preLedStatus != getLedStatus();
}

/******************************************************************************
* 函数名		: parse_payload
* 函数描述  	: 解析服务器下发的控制命令，具体业务 {value:12345}
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 
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
* 函数名		: recv_mqtt
* 函数描述  	: 接收mqtt包
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	:
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
					//阿里云暂时不支持
				}else{
					//0级不用回复
				}
			}
			break;
		case PUBACK:
			if(MQTTDeserialize_ack(&packettype,&dup,&rcv_msgid, buf, MQTT_BUFF_LEN)){
				if(rcv_msgid == msgid){ //发送的消息确认被服务器收到
					force_send = 0;
					msgid = (msgid+1)%100;
					printf("stop send\r\n");
				}else{
					force_send = 1;  //没成功继续发送
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
* 函数名		: handle_mqtt
* 函数描述  	: 处理mqtt的一切事务（连接，接收，应答等）
* 输入参数  	: 
* 输出结果  	: 
* 返回值    	: 0,连接成功(得到了期待的应答结果)  1,连接失败
******************************************************************************/  
void handle_mqtt(void){
		if(check_connect_close()){   //在循环中不断检查是否掉线
			delay_ms(1000);
			return;
		}
	
		if(flag_server == 0){    //连接服务器，连接过就不再连接，除非断线
			if(connect_server()){
				delay_ms(1000);
				return;
			}
		}
		
		if(flag_conn == 0){			//发送连接包
			if(send_connect_packet()){
				delay_ms(1000);
				return;
			}
		}
		if(flag_sub == 0){      //订阅
			if(send_subscribe_packet()){
				delay_ms(1000);
				return;
			}
			preLedStatus = getLedStatus();
			//force_send = 1;     //第一次上电初始化成功要主动上报一次
		}
		
		if(check_to_publish()){  //该发送发布包
			send_publish_packet();
		}
		
		if(check_to_ping()){   //该发送ping包
			send_ping_packet();
		}
		
		recv_mqtt();
}
