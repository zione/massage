#ifndef AES_H
#define AES_H

//#include "global.h"
/*global.h 下面包含 要对输出函数(Printf)做适当的修改*/
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void aesInit( unsigned char * tempbuf );
void aesDecrypt(unsigned char *buffer, unsigned char *chainBlock);
void aesEncrypt( unsigned char * buffer, unsigned char * chainBlock );

unsigned char aesBlockDecrypt(unsigned char Direct,unsigned char *ChiperDataBuf,unsigned char DataLen);
unsigned char AesBlockDecrypt(unsigned char Direct,unsigned char *ChiperDataBuf,unsigned char DataLen);
//extern unsigned char aesBlockDecrypt1(unsigned char Direct,unsigned char *ChiperDataBuf,unsigned char DataLen);

#endif // AES_H
