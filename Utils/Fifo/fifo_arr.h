#ifndef _FIFO_ARR_H
#define _FIFO_ARR_H

#include <stm32f10x.h>
#include <string.h>

#define FIFO_LENGTH (2048)
#define FIFO_COUNT  (3)

typedef struct
{
	uint8_t	FifoBuf[FIFO_LENGTH];
	uint16_t FifoBytes;
	uint8_t *FifoRead;
	uint8_t *FifoWrite;
	uint8_t SyncEvent;
}MYFIFO;

extern MYFIFO pFifo[FIFO_COUNT]; 

void Fifo_Init(void);
void FiFoClear(uint16_t FifoNum);
uint16_t PutFifoData(uint16_t FifoNum,uint8_t *buf,uint16_t length);
uint16_t ReadFifoData(uint16_t FifoNum,uint8_t *buf,uint16_t length);
uint16_t GetFifoBytes(uint16_t FifoNum);

#endif
