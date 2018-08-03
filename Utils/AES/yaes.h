#ifndef __AES_H
#define __AES_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void aesInit( unsigned char * tempbuf );
void aesDecrypt(unsigned char *buffer, unsigned char *chainBlock);
void aesEncrypt( unsigned char * buffer, unsigned char * chainBlock );

unsigned char aesBlockDecrypt(unsigned char Direct,unsigned char *ChiperDataBuf,unsigned char DataLen);
unsigned char AesBlockDecrypt(unsigned char Direct,unsigned char *ChiperDataBuf,unsigned char DataLen);//外部调用接口,数据长度不足16字节,直接补0
extern unsigned char kTable[16];

#endif
