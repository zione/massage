#include "aes.h"
#include <string.h>

#define byte unsigned char 

#define BPOLY 0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16 //!< Block size in number of bytes.4*4矩阵,AES一次只能对16字节加密

#define KEYBITS 128 //!< Use AES128.密钥宽度
#define ROUNDS 10 //!< Number of rounds.循环迭代次数
#define KEYLENGTH 16 //!< Key length in number of bytes.

byte block1[ 256 ]; //!< Workspace 1.
byte block2[ 256 ]; //!< Worksapce 2.

byte *powTbl; //!< Final location of exponentiation lookup table.
byte *logTbl; //!< Final location of logarithm lookup table.
byte *sBox; //!< Final location of s-box.
byte *sBoxInv; //!< Final location of inverse s-box.
byte *expandedKey; //!< Final location of expanded key.

//以下为加解密密码，共16字节。可以选择任意值
byte kTable[16] =
{
//	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 
//	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38

	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};
//2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c 
void CalcPowLog( byte *powTbl, byte *logTbl )
{
    byte i = 0;
    byte t = 1;

    do {
		// Use 0x03 as root for exponentiation and logarithms.
    	powTbl[i] = t;
    	logTbl[t] = i;
    	i++;

		// Muliply t by 3 in GF(2^8).
    	t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
    } while( t != 1 ); // Cyclic properties ensure that i < 255.
    
    powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.
}

//获取置换S-盒
void CalcSBox( byte *sBox )
{
	byte i, rot;
	byte temp;
	byte result;

	// Fill all entries of sBox[].
	i = 0;
	do {
		// Inverse in GF(2^8).
		if( i > 0 ) 
		{
			temp = powTbl[ 255 - logTbl[i] ];
	    } 
		else 
		{
            temp = 0;
		}

        // Affine transformation in GF(2).
        result = temp ^ 0x63; // Start with adding a vector in GF(2).
        for( rot = 0; rot < 4; rot++ ) 
		{
            // Rotate left.
			temp = (temp<<1) | (temp>>7);

			// Add rotated byte in GF(2).
			result ^= temp;
		}
		// Put result in table.
        sBox[i] = result;
	} while( ++i != 0 );
}	

//获取解谜替换S盒(用于解密)
void CalcSBoxInv( byte *sBox, byte *sBoxInv )
{
	byte i = 0;
	byte j = 0;

	// Iterate through all elements in sBoxInv using  i.
	do {
		// Search through sBox using j.
		 
		do {
			// Check if current j is the inverse of current i.
			if( sBox[ j ] == i ) 
			{
				// If so, set sBoxInc and indicate search finished.
				sBoxInv[ i ] = j;
				j = 255;
			}
		} while( ++j != 0 );
	} while( ++i != 0 );
}

void CycleLeft( byte *row )
{
	// Cycle 4 bytes in an array left once.
	byte   temp = row[0];
	row[0] = row[1];
	row[1] = row[2];
	row[2] = row[3];
	row[3] = temp;
}

//列混合反变换(用于解密)
void InvMixColumn( byte *column )
{
	byte  r0, r1, r2, r3;

	r0 = column[1] ^ column[2] ^ column[3];
	r1 = column[0] ^ column[2] ^ column[3];
	r2 = column[0] ^ column[1] ^ column[3];
	r3 = column[0] ^ column[1] ^ column[2];

 	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
 	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
   	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
   	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

	r0 ^= column[0] ^ column[1];
	r1 ^= column[1] ^ column[2];
	r2 ^= column[2] ^ column[3];
	r3 ^= column[0] ^ column[3];

   	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
   	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
   	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
   	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

	r0 ^= column[0] ^ column[2];
	r1 ^= column[1] ^ column[3];
	r2 ^= column[0] ^ column[2];
	r3 ^= column[1] ^ column[3];

   	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
   	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
   	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
   	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

	column[0] ^= column[1] ^ column[2] ^ column[3];
	r0 ^= column[0];
	r1 ^= column[0];
	r2 ^= column[0];
	r3 ^= column[0];

	column[0] = r0;
	column[1] = r1;
	column[2] = r2;
	column[3] = r3;
}

byte Multiply( byte num, byte factor )
{
	byte mask = 1;
	byte result = 0;

	while( mask != 0 ) 
	{
		// Check bit of factor given by mask.
		if( mask & factor ) 
		{
			// Add current multiple of num in GF(2).
			result ^= num;
		}

		// Shift mask to indicate next bit.
		mask <<= 1;

		// Double num.
       	num = (num << 1) ^ (num & 0x80 ? BPOLY : 0);
	}

	return result;
}

byte DotProduct( byte *vector1, byte *vector2 )
{
	byte result = 0;

	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1  , *vector2   );

	return result;
}

void MixColumn( byte *column )
{
	byte row[8] = 
	{
		0x02, 0x03, 0x01, 0x01,
		0x02, 0x03, 0x01, 0x01
	}; // Prepare first row of matrix twice, to eliminate need for cycling.

	byte result[4];
	
	// Take dot products of each matrix row and the column vector.
	result[0] = DotProduct( row+0, column );
	result[1] = DotProduct( row+3, column );
	result[2] = DotProduct( row+2, column );
	result[3] = DotProduct( row+1, column );

	// Copy temporary result to original column.
	column[0] = result[0];
	column[1] = result[1];
	column[2] = result[2];
	column[3] = result[3];
}

//字节替换
void SubBytes( byte *bytes, byte count )
{
	do {
		*bytes = sBox[ *bytes ]; // Substitute every byte in state.
		bytes++;
	} while( --count );
}

// 用于解密的字节反替换
void InvSubBytesAndXOR( byte *bytes, byte *key, byte count )
{
	do {
		//*bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.
		*bytes = block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.
		bytes++;
		key++;
	} while( --count );
}

//用于解密的行位移反变换
void InvShiftRows( byte *state )
{
	byte temp;

	// Note: State is arranged column by column.

	// Cycle second row right one time.
	temp = state[ 1 + 3*4 ];
	state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
	state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
	state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
	state[ 1 + 0*4 ] = temp;

	// Cycle third row right two times.
	temp = state[ 2 + 0*4 ];
	state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
	state[ 2 + 2*4 ] = temp;
	temp = state[ 2 + 1*4 ];
	state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
	state[ 2 + 3*4 ] = temp;

	// Cycle fourth row right three times, ie. left once.
	temp = state[ 3 + 0*4 ];
	state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
	state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
	state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
	state[ 3 + 3*4 ] = temp;
}

//行位移变换
void ShiftRows( byte *state )
{
	byte temp;

	// Note: State is arranged column by column.

	// Cycle second row left one time.
	temp = state[ 1 + 0*4 ];
	state[ 1 + 0*4 ] = state[ 1 + 1*4 ];
	state[ 1 + 1*4 ] = state[ 1 + 2*4 ];
	state[ 1 + 2*4 ] = state[ 1 + 3*4 ];
	state[ 1 + 3*4 ] = temp;

	// Cycle third row left two times.
	temp = state[ 2 + 0*4 ];
	state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
	state[ 2 + 2*4 ] = temp;
	temp = state[ 2 + 1*4 ];
	state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
	state[ 2 + 3*4 ] = temp;

	// Cycle fourth row left three times, ie. right once.
	temp = state[ 3 + 3*4 ];
	state[ 3 + 3*4 ] = state[ 3 + 2*4 ];
	state[ 3 + 2*4 ] = state[ 3 + 1*4 ];
	state[ 3 + 1*4 ] = state[ 3 + 0*4 ];
	state[ 3 + 0*4 ] = temp;
}

//用于解密的行混合反变换
void InvMixColumns( byte *state )
{
	InvMixColumn( state + 0*4 );
	InvMixColumn( state + 1*4 );
	InvMixColumn( state + 2*4 );
	InvMixColumn( state + 3*4 );
}

//行混合变换
void MixColumns( byte *state )
{
	MixColumn( state + 0*4 );
	MixColumn( state + 1*4 );
	MixColumn( state + 2*4 );
	MixColumn( state + 3*4 );
}

void XORBytes( byte *bytes1, byte *bytes2, byte count )
{
	do {
		*bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
		bytes1++;
		bytes2++;
	} while( --count );
}



void CopyBytes( byte *to, byte *from, byte count )
{
	do {
		*to = *from;
		to++;
		from++;
	} while( --count );
}

void KeyExpansion( byte *expandedKey )
{
	byte temp[4];
	byte i;
	byte Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant.
	
	byte *key = kTable;//获取我们的密钥,然后生成密钥调度表

	// Copy key to start of expanded key.
	i = KEYLENGTH;
	do {
		*expandedKey = *key;
		expandedKey++;
		key++;
	} while( --i );

	// Prepare last 4 bytes of key in temp.
	expandedKey -= 4;
	temp[0] = *(expandedKey++);
	temp[1] = *(expandedKey++);
	temp[2] = *(expandedKey++);
	temp[3] = *(expandedKey++);

	// Expand key.
	i = KEYLENGTH;
	while( i < BLOCKSIZE*(ROUNDS+1) ) 
	{
		// Are we at the start of a multiple of the key size?
		if( (i % KEYLENGTH) == 0 ) 
		{
			CycleLeft( temp ); // Cycle left once.
			SubBytes( temp, 4 ); // Substitute each byte.
			XORBytes( temp, Rcon, 4 ); // Add constant in GF(2).
			*Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
		}

		// Keysize larger than 24 bytes, ie. larger that 192 bits?
		#if KEYLENGTH > 24
		// Are we right past a block size?
		else if( (i % KEYLENGTH) == BLOCKSIZE ) 
		{
			SubBytes( temp, 4 ); // Substitute each byte.
		}
		#endif

		// Add bytes in GF(2) one KEYLENGTH away.
		XORBytes( temp, expandedKey - KEYLENGTH, 4 );

		// Copy result to current 4 bytes.
		*(expandedKey++) = temp[ 0 ];
		*(expandedKey++) = temp[ 1 ];
		*(expandedKey++) = temp[ 2 ];
		*(expandedKey++) = temp[ 3 ];

		i += 4; // Next 4 bytes.
	}	
}

//解密输入函数
void InvCipher( byte *block, byte *expandedKey )
{
	byte round = ROUNDS-1;
	expandedKey += BLOCKSIZE * ROUNDS;

	XORBytes( block, expandedKey, 16 );
	expandedKey -= BLOCKSIZE;

	do {
		InvShiftRows( block );
		InvSubBytesAndXOR( block, expandedKey, 16 );
		expandedKey -= BLOCKSIZE;
		InvMixColumns( block );
	} while( --round );

	InvShiftRows( block );
	InvSubBytesAndXOR( block, expandedKey, 16 );
}

//加密输入函数
void Cipher( byte *block, byte *expandedKey )//完成一个块(16字节，128bit)的加密
{
	byte round = ROUNDS-1;

	XORBytes( block, expandedKey, 16 );
	expandedKey += BLOCKSIZE;

	do {
		SubBytes( block, 16 );
		ShiftRows( block );
		MixColumns( block );
		XORBytes( block, expandedKey, 16 );
		expandedKey += BLOCKSIZE;
	} while( --round );

	SubBytes( block, 16 );
	ShiftRows( block );
	XORBytes( block, expandedKey, 16 );
}

void aesInit( byte *tempbuf )
{
	powTbl = block1;
	logTbl = block2;
	CalcPowLog( powTbl, logTbl );

	sBox = tempbuf;
	CalcSBox( sBox );

	expandedKey = block1;  //至此block1用来存贮密码表
	KeyExpansion( expandedKey );
	
	sBoxInv = block2; // Must be block2. block2至此开始只用来存贮SBOXINV
	CalcSBoxInv( sBox, sBoxInv );
}	

//对一个16字节块解密,参数buffer是解密缓存，chainBlock是要解密的块
void aesDecrypt( byte *buffer, byte *chainBlock )
{
	//byte   temp[ BLOCKSIZE ];

	//CopyBytes( temp, buffer, BLOCKSIZE );
	CopyBytes(buffer,chainBlock,BLOCKSIZE);
	InvCipher( buffer, expandedKey );
	//XORBytes( buffer, chainBlock, BLOCKSIZE );
	CopyBytes( chainBlock, buffer, BLOCKSIZE );
}

//对一个16字节块完成加密，参数buffer是加密缓存，chainBlock是要加密的块
void aesEncrypt( byte *buffer, byte *chainBlock )
{
	CopyBytes( buffer, chainBlock, BLOCKSIZE );
	//XORBytes( buffer, chainBlock, BLOCKSIZE );
	Cipher( buffer, expandedKey );
	CopyBytes( chainBlock, buffer, BLOCKSIZE );
}

//加解密函数，参数为加解密标志，要加解密的数据缓存起始指针，要加解密的数据长度（如果解密运算，必须是16的整数倍。）
byte aesBlockDecrypt(byte Direct,byte *ChiperDataBuf,byte DataLen)
{
    byte i;
	byte Blocks;
	byte sBoxbuf[256];
	byte tempbuf[16];
	unsigned long int   OrignLen=0;//未加密数据的原始长度

	if(Direct==0)//解密
	{
		*((byte *)&OrignLen+3)=ChiperDataBuf[0];
		*((byte *)&OrignLen+2)=ChiperDataBuf[1];
		*((byte *)&OrignLen+1)=ChiperDataBuf[2];
		*((byte *)&OrignLen)=ChiperDataBuf[3];
		DataLen=DataLen-4;
	}
	else//加密
	{
		//*memmove(void *dest, const void *src, unsigned int count);

		memmove(ChiperDataBuf+4,ChiperDataBuf,DataLen);
		OrignLen=DataLen;
		ChiperDataBuf[0]=OrignLen;
		ChiperDataBuf[1]=OrignLen>>8;
		ChiperDataBuf[2]=OrignLen>>16;
		ChiperDataBuf[3]=OrignLen>>24;
	}
	 
	aesInit(sBoxbuf);//初始化
	if(Direct==0)//解密
	{
		Blocks=DataLen/16;
		for(i=0;i<Blocks;i++)
		{
			aesDecrypt(tempbuf,ChiperDataBuf+4+16*i);
		}
		memmove(ChiperDataBuf,ChiperDataBuf+4,OrignLen);
		 
		return(OrignLen);
	}
	else//加密
	{
		if(DataLen%16!=0)
		{  
			Blocks=DataLen/16+1;
			//memset(ChiperDataBuf+4+Blocks*16-(DataLen%16),0x00,DataLen%16); //不足16字节的块补零处理
		}
		else
		{
			Blocks=DataLen/16;
		}

		for(i=0;i<Blocks;i++)
		{
			aesEncrypt(tempbuf,ChiperDataBuf+4+16*i);
		}
		 
		return(Blocks*16+4);
	}
}

/*----------------- 给外部函数调用接口-------------------*/
/************************************************************
函数名:     AesBlockDecrypt
功能描述:   Aes 加解密函数

入口参数:       Direct,*ChiperDataBuf,DataLen
入口参数的说明: 
			    Direct: 0  解密  
				        1  加密
				*ChiperDataBuf：加密解密 数据的入口
                 DataLen：数据长度 不足十六字节，直接补0 
                         
                
出口参数：      *ChiperDataBuf：加密解密 数据的出口地址
出口参数说明：  加密解密 数据的出口地址
                
返回值：        byte 类型
返回值的说明:   加密解密正常返回 16个字节，不正常 返回 0
************************************************************/
byte AesBlockDecrypt(byte Direct,byte *ChiperDataBuf,byte DataLen)
{
	byte st;
	byte i;
	byte temp[4];
	if( Direct == 0 )//解密
	{ 
		temp[0] = 0x00;
		temp[1] = 0x00;
		temp[2] = 0x00;
		temp[3] = 0x10;
		memmove(ChiperDataBuf+4,ChiperDataBuf,DataLen);
		DataLen += 4;
		for(i=0;i<4;i++)
		ChiperDataBuf[i] = temp[i];
		st = aesBlockDecrypt(Direct,ChiperDataBuf,DataLen); 
	}
	else if( Direct == 1 )//加密
	{ 
		st = aesBlockDecrypt(Direct,ChiperDataBuf,DataLen);
		memmove(ChiperDataBuf,ChiperDataBuf+4,st-4);
		st = st - 4; 
	}
    else
    {
        st = 0;
    }
    return st;
}
//#endif

