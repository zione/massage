#include "enc28j60_test.h"

void ENC28J60_Test(void)
{
  /* ENC28J60 SPI �ӿڳ�ʼ�� */
  SPI_Enc28j60_Init();
	
	/* ENC28J60 WEB ������� */  
  Web_Server();
}

