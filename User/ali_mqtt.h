#include "stm32f10x.h"

#define MQTT_BUFF_LEN 256
#define CLOSE_STR "CLOSED"
#define CLIENT_ID "xza123456|securemode=3,signmethod=hmacsha1,timestamp=789|"
#define USER_NAME "am88285&a11fsZZPcNJ"
#define PASSWORD "1f564cd72c8a7b35ff1a15b69e1f8f0abf7c115f"
#define TOPIC_STRING "/a11fsZZPcNJ/am88285/data"
#define PORT "1883"																	//¶Ë¿Ú¹Ì¶¨Îª1883
#define IP_ADDR "a11fsZZPcNJ.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define ALIVE_TIME 80   //Ãë

#define STATUS_ON "STATUS:ON"
#define STATUS_OFF "STATUS:OFF"

void handle_mqtt(void);
uint8_t connect_server(void);
uint8_t send_connect_packet(void);
uint8_t send_subscribe_packet(void);
uint8_t send_publish_packet(void);
uint8_t check_to_ping(void);
void recv_mqtt(void);
void parse_payload(unsigned char* payload,int length);

