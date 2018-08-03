#include "bsp_led.h"
#include "delay.h"
#include "bsp_key.h"
#include "TIMER.h"
#include "USART1_init.h"
#include "USART2_init.h"
#include "printf_init.h"
#include "transport.h"
#include "MQTTPacket.h"
#include "SIM800L.h"
#include <string.h>

#define CLIENT_ID "xza123456|securemode=3,signmethod=hmacsha1,timestamp=789|"
#define USER_NAME "am88285&a11fsZZPcNJ"
#define PASSWORD "1f564cd72c8a7b35ff1a15b69e1f8f0abf7c115f"
#define TOPIC_STRING "/a11fsZZPcNJ/am88285/data"

void ByteToHexStr(const char* source, char* dest, int sourceLen);
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{ 
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int mysock = 0;
	unsigned char buf[256];
	int buflen = sizeof(buf);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 1;
	char* payload = "mypayload";
	int payloadlen = strlen(payload);
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
	
	RCC_Configuration();
	delay_init();																		//延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	LED_GPIO_Config();
	KEY_Init();
	
  USART1_Config();		
  USART2_Config();
	USART2_DMATxd_Init();
	Set_PrintfPort(1);		//设置DebugPf从串口1输出
	
	Sim_ini();   //初始化sim868
mqtt_connect:	
	data.clientID.cstring = CLIENT_ID; 
	data.username.cstring = USER_NAME;
	data.password.cstring = PASSWORD;
	data.keepAliveInterval = 80;   //80秒
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
			goto mqtt_connect;
		}
		printf("mqtt connect ok\n");
	}
	else{
		printf("mqtt connect fail2\n");
		goto mqtt_connect;
	}
		
	
		/* subscribe */

mqtt_subscrib:
	topicString.cstring = TOPIC_STRING;
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
	printf("-------s--------\n");
		for(;i<len;i++){
			printf("%02X ",buf[i]);
		}
	printf("-------s--------\n");
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
			printf("---------------\n");
			print_usart2_buf();
			printf("---------------\n");
			goto mqtt_subscrib;
		}
		printf("mqtt subscrib ok\n");
	}
	else{
		printf("mqtt subscrib fail2\n");
		goto mqtt_subscrib;
	}
	
	/* loop getting msgs on subscribed topic */
	topicString.cstring = TOPIC_STRING;
	while(1)
	{
		switch(KEY_Scan(0))
		{
			case KEY0_PRES:
				//printf("publishing send\n");
				//payload = "mypayload";
				//payloadlen = strlen(payload);
				//len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
				//transport_sendPacketBuffer(mysock, buf, len);
				printf("ping...\n");
				len = MQTTSerialize_pingreq(buf, buflen);
				transport_sendPacketBuffer(mysock, buf, len);
				break;
			default:
				break;
		}
		
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		msg_type = MQTTPacket_read(buf, buflen, transport_getdata);
		if (msg_type == PUBLISH)
		{
			MQTTDeserialize_publish(&dup, &qos, &retained, &pmsgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
			printf("message arrived %.*s\n", payloadlen_in, payload_in);
			
			
		}
		else if (msg_type == PINGRESP){
			printf("get Pong ack\n");
		}
	}
	
	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	transport_sendPacketBuffer(mysock, buf, len);
}


void ByteToHexStr(const char* source, char* dest, int sourceLen)

{
    short i;
    unsigned char highByte, lowByte;


    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;


        highByte += 0x30;


        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;


        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return ;
}
/* ----------------------------------------end of file------------------------------------------------ */

