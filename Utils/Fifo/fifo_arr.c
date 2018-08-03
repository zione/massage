#include "fifo_arr.h"

MYFIFO pFifo[FIFO_COUNT];

//FIFO ��ʼ��
void Fifo_Init(void)
{
	uint16_t i=0;
	for(i=0;i<FIFO_COUNT;i++)
	{
		pFifo[i].FifoBytes=0;
		pFifo[i].FifoRead=pFifo[i].FifoBuf;
		pFifo[i].FifoWrite=pFifo[i].FifoBuf;
		pFifo[i].SyncEvent=0;
	}
}

//���FIFO
void FiFoClear(uint16_t FifoNum)
{
	if(FifoNum>=FIFO_COUNT)
	{
		return;
	}
	pFifo[FifoNum].FifoBytes=0;
	pFifo[FifoNum].FifoRead=pFifo[FifoNum].FifoBuf;
	pFifo[FifoNum].FifoWrite=pFifo[FifoNum].FifoBuf;
	pFifo[FifoNum].SyncEvent=0;	
}

//��FIFO��д������; ע:���ϴε������ǰ,���ܽ��е���,�������
uint16_t PutFifoData(uint16_t FifoNum,uint8_t *buf,uint16_t length)
{
	uint16_t i=0;
	if(FifoNum>=FIFO_COUNT)
	{
		return 0;
	}
	if(pFifo[FifoNum].FifoBytes+length>FIFO_LENGTH)
	{
		return 0;
	}
	for(i=0;i<length;i++)
	{
		if(pFifo[FifoNum].FifoWrite==pFifo[FifoNum].FifoBuf+FIFO_LENGTH)
		{
			pFifo[FifoNum].FifoWrite=pFifo[FifoNum].FifoBuf;
		}
		if(pFifo[FifoNum].FifoBytes>=FIFO_LENGTH)
		{
			return i;
		}
		*(pFifo[FifoNum].FifoWrite++)=*(buf+i);
		pFifo[FifoNum].FifoBytes++;
	}
	return length;
}

//��FIFO�ж�ȡ����; ע:���ϴε������ǰ,���ܽ��е���,�������
uint16_t ReadFifoData(uint16_t FifoNum,uint8_t *buf,uint16_t length)
{
	uint16_t i=0;
	if(FifoNum>=FIFO_COUNT)
	{
		return 0;	
	}
	if(length>pFifo[FifoNum].FifoBytes)
	{
		return 0;
	}
	for(i=0;i<length;i++)
	{
		if(pFifo[FifoNum].FifoRead>=pFifo[FifoNum].FifoBuf+FIFO_LENGTH)
		{
			pFifo[FifoNum].FifoRead=pFifo[FifoNum].FifoBuf;
		}
		if(pFifo[FifoNum].FifoBytes==0)
		{
			return i;
		}
		*(buf+i)=*(pFifo[FifoNum].FifoRead++);
		pFifo[FifoNum].FifoBytes--;
	}
	return length;
}

//��ȡFIFO��������
uint16_t GetFifoBytes(uint16_t FifoNum)
{
	return pFifo[FifoNum].FifoBytes;
}
