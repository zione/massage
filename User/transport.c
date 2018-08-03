#include "SIM800L.h"
#include "USART2_init.h"
#include "delay.h"
/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	return sim800c_send_data(buf,buflen);
}


int transport_getdata(unsigned char* buf, int count)
{
	int len = 0;
	int timeout = 0;
	
	while(1){
		len = USART2_GetData(buf,count);
		if(len > 0 || timeout > 25){
			break;
		}
		timeout++;
		delay_ms(200);
	}
	return len;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{
	return 0;
}

int transport_close(int sock)
{
	return 0;
}
