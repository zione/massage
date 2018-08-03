#include "fifo.h"

MYFIFO pFifo;

//FIFO ��ʼ��
void FifoInit(void)
{
	pFifo.FifoBytes=0;
	pFifo.FifoRead=pFifo.FifoBuf;
	pFifo.FifoWrite=pFifo.FifoBuf;
	pFifo.SyncEvent=0;
}

//��FIFO��д������; ע:���ϴε������ǰ,���ܽ��е���,�������
uint16_t PutFifoData(uint8_t *buf,uint16_t length)
{
	uint16_t i=0;

	if(pFifo.FifoBytes+length>FIFO_LENGTH)//���ȴ���ʣ��ռ�
	{
		return 0;
	}
	for(i=0;i<length;i++)
	{
		if(pFifo.FifoWrite==pFifo.FifoBuf+FIFO_LENGTH)//��βѭ��
		{
			pFifo.FifoWrite=pFifo.FifoBuf;
		}
		//if(pFifo.FifoBytes>=FIFO_LENGTH)//û�пռ���
		//{
		//	return i;
		//}
		*(pFifo.FifoWrite++)=*(buf+i);
		pFifo.FifoBytes++;
	}
	return length;
}

//��FIFO�ж�ȡ����; ע:���ϴε������ǰ,���ܽ��е���,�������
uint16_t ReadFifoData(uint8_t *buf,uint16_t length)
{
	uint16_t i=0;

	if(length>pFifo.FifoBytes)//���ȴ���������
	{
		return 0;
	}
	for(i=0;i<length;i++)
	{
		if(pFifo.FifoRead>=pFifo.FifoBuf+FIFO_LENGTH)//��βѭ��
		{
			pFifo.FifoRead=pFifo.FifoBuf;
		}
		//if(pFifo.FifoBytes==0)//û��������
		//{
		//	return i;
		//}
		*(buf+i)=*(pFifo.FifoRead++);
		pFifo.FifoBytes--;
	}
	return length;
}

//�鿴Fifo����
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

uint16_t GetFifoBytes(void)//��ȡFIFO��������
{
	return pFifo.FifoBytes;
}
