#ifndef _FIFO_H
#define _FIFO_H

#include <stm32f10x.h>
#include <string.h>

#define FIFO_LENGTH (2048)

typedef struct
{
	uint8_t	FifoBuf[FIFO_LENGTH];
	uint16_t FifoBytes;
	uint8_t *FifoRead;
	uint8_t *FifoWrite;
	uint8_t SyncEvent;
}MYFIFO;

extern MYFIFO pFifo;

void FifoInit(void);
uint16_t PutFifoData(uint8_t *buf,uint16_t length);
uint16_t ReadFifoData(uint8_t *buf,uint16_t length);
uint16_t PeekFifoData(uint8_t *buf,uint16_t length);//查看Fifo数据
uint16_t GetFifoBytes(void);

#endif
