#ifndef __AES_H
#define __AES_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void aesInit( unsigned char * tempbuf );
void aesDecrypt(unsigned char *buffer, unsigned char *chainBlock);
void aesEncrypt( unsigned char * buffer, unsigned char * chainBlock );

unsigned char aesBlockDecrypt(unsigned char Direct,unsigned char *ChiperDataBuf,unsigned char DataLen);
unsigned char AesBlockDecrypt(unsigned char Direct,unsigned char *ChiperDataBuf,unsigned char DataLen);//�ⲿ���ýӿ�,���ݳ��Ȳ���16�ֽ�,ֱ�Ӳ�0
extern unsigned char kTable[16];

#endif
