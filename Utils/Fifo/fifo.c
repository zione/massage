#include "fifo.h"

MYFIFO pFifo;

//FIFO 初始化
void FifoInit(void)
{
	pFifo.FifoBytes=0;
	pFifo.FifoRead=pFifo.FifoBuf;
	pFifo.FifoWrite=pFifo.FifoBuf;
	pFifo.SyncEvent=0;
}

//向FIFO中写入数据; 注:在上次调用完成前,不能进行调用,否则出错
uint16_t PutFifoData(uint8_t *buf,uint16_t length)
{
	uint16_t i=0;

	if(pFifo.FifoBytes+length>FIFO_LENGTH)//长度大于剩余空间
	{
		return 0;
	}
	for(i=0;i<length;i++)
	{
		if(pFifo.FifoWrite==pFifo.FifoBuf+FIFO_LENGTH)//首尾循环
		{
			pFifo.FifoWrite=pFifo.FifoBuf;
		}
		//if(pFifo.FifoBytes>=FIFO_LENGTH)//没有空间了
		//{
		//	return i;
		//}
		*(pFifo.FifoWrite++)=*(buf+i);
		pFifo.FifoBytes++;
	}
	return length;
}

//从FIFO中读取数据; 注:在上次调用完成前,不能进行调用,否则出错
uint16_t ReadFifoData(uint8_t *buf,uint16_t length)
{
	uint16_t i=0;

	if(length>pFifo.FifoBytes)//长度大于数据量
	{
		return 0;
	}
	for(i=0;i<length;i++)
	{
		if(pFifo.FifoRead>=pFifo.FifoBuf+FIFO_LENGTH)//首尾循环
		{
			pFifo.FifoRead=pFifo.FifoBuf;
		}
		//if(pFifo.FifoBytes==0)//没有数据了
		//{
		//	return i;
		//}
		*(buf+i)=*(pFifo.FifoRead++);
		pFifo.FifoBytes--;
	}
	return length;
}

//查看Fifo数据
uint16_t PeekFifoData(uint8_t *buf,uint16_t length)
{
	uint16_t i=0;
	u8* lu_addr;
	
	lu_addr=pFifo.FifoRead;
	if(length>pFifo.FifoBytes)
	{
		return 0;
	}
	for(i=0;i<length;i++)
	{
		if(lu_addr>=pFifo.FifoBuf+FIFO_LENGTH)
		{
			lu_addr=pFifo.FifoBuf;
		}
		*(buf+i)=*(lu_addr++);
	}
	return length;
}

uint16_t GetFifoBytes(void)//获取FIFO中数据量
{
	return pFifo.FifoBytes;
}
