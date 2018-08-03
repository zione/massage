#include "aes.h"
#include <string.h>

#define byte unsigned char 

#define BPOLY 0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16 //!< Block size in number of bytes.4*4����,AESһ��ֻ�ܶ�16�ֽڼ���

#define KEYBITS 128 //!< Use AES128.��Կ���
#define ROUNDS 10 //!< Number of rounds.ѭ����������
#define KEYLENGTH 16 //!< Key length in number of bytes.

byte block1[ 256 ]; //!< Workspace 1.
byte block2[ 256 ]; //!< Worksapce 2.

byte *powTbl; //!< Final location of exponentiation lookup table.
byte *logTbl; //!< Final location of logarithm lookup table.
byte *sBox; //!< Final location of s-box.
byte *sBoxInv; //!< Final location of inverse s-box.
byte *expandedKey; //!< Final location of expanded key.

//����Ϊ�ӽ������룬��16�ֽڡ�����ѡ������ֵ
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

//��ȡ�û�S-��
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

//��ȡ�����滻S��(���ڽ���)
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

//�л�Ϸ��任(���ڽ���)
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

//�ֽ��滻
void SubBytes( byte *bytes, byte count )
{
	do {
		*bytes = sBox[ *bytes ]; // Substitute every byte in state.
		bytes++;
	} while( --count );
}

// ���ڽ��ܵ��ֽڷ��滻
void InvSubBytesAndXOR( byte *bytes, byte *key, byte count )
{
	do {
		//*bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.
		*bytes = block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.
		bytes++;
		key++;
	} while( --count );
}

//���ڽ��ܵ���λ�Ʒ��任
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

//��λ�Ʊ任
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

//���ڽ��ܵ��л�Ϸ��任
void InvMixColumns( byte *state )
{
	InvMixColumn( state + 0*4 );
	InvMixColumn( state + 1*4 );
	InvMixColumn( state + 2*4 );
	InvMixColumn( state + 3*4 );
}

//�л�ϱ任
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
	
	byte *key = kTable;//��ȡ���ǵ���Կ,Ȼ��������Կ���ȱ�

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

//�������뺯��
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

//�������뺯��
void Cipher( byte *block, byte *expandedKey )//���һ����(16�ֽڣ�128bit)�ļ���
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

	expandedKey = block1;  //����block1�������������
	KeyExpansion( expandedKey );
	
	sBoxInv = block2; // Must be block2. block2���˿�ʼֻ��������SBOXINV
	CalcSBoxInv( sBox, sBoxInv );
}	

//��һ��16�ֽڿ����,����buffer�ǽ��ܻ��棬chainBlock��Ҫ���ܵĿ�
void aesDecrypt( byte *buffer, byte *chainBlock )
{
	//byte   temp[ BLOCKSIZE ];

	//CopyBytes( temp, buffer, BLOCKSIZE );
	CopyBytes(buffer,chainBlock,BLOCKSIZE);
	InvCipher( buffer, expandedKey );
	//XORBytes( buffer, chainBlock, BLOCKSIZE );
	CopyBytes( chainBlock, buffer, BLOCKSIZE );
}

//��һ��16�ֽڿ���ɼ��ܣ�����buffer�Ǽ��ܻ��棬chainBlock��Ҫ���ܵĿ�
void aesEncrypt( byte *buffer, byte *chainBlock )
{
	CopyBytes( buffer, chainBlock, BLOCKSIZE );
	//XORBytes( buffer, chainBlock, BLOCKSIZE );
	Cipher( buffer, expandedKey );
	CopyBytes( chainBlock, buffer, BLOCKSIZE );
}

//�ӽ��ܺ���������Ϊ�ӽ��ܱ�־��Ҫ�ӽ��ܵ����ݻ�����ʼָ�룬Ҫ�ӽ��ܵ����ݳ��ȣ�����������㣬������16������������
byte aesBlockDecrypt(byte Direct,byte *ChiperDataBuf,byte DataLen)
{
    byte i;
	byte Blocks;
	byte sBoxbuf[256];
	byte tempbuf[16];
	unsigned long int   OrignLen=0;//δ�������ݵ�ԭʼ����

	if(Direct==0)//����
	{
		*((byte *)&OrignLen+3)=ChiperDataBuf[0];
		*((byte *)&OrignLen+2)=ChiperDataBuf[1];
		*((byte *)&OrignLen+1)=ChiperDataBuf[2];
		*((byte *)&OrignLen)=ChiperDataBuf[3];
		DataLen=DataLen-4;
	}
	else//����
	{
		//*memmove(void *dest, const void *src, unsigned int count);

		memmove(ChiperDataBuf+4,ChiperDataBuf,DataLen);
		OrignLen=DataLen;
		ChiperDataBuf[0]=OrignLen;
		ChiperDataBuf[1]=OrignLen>>8;
		ChiperDataBuf[2]=OrignLen>>16;
		ChiperDataBuf[3]=OrignLen>>24;
	}
	 
	aesInit(sBoxbuf);//��ʼ��
	if(Direct==0)//����
	{
		Blocks=DataLen/16;
		for(i=0;i<Blocks;i++)
		{
			aesDecrypt(tempbuf,ChiperDataBuf+4+16*i);
		}
		memmove(ChiperDataBuf,ChiperDataBuf+4,OrignLen);
		 
		return(OrignLen);
	}
	else//����
	{
		if(DataLen%16!=0)
		{  
			Blocks=DataLen/16+1;
			//memset(ChiperDataBuf+4+Blocks*16-(DataLen%16),0x00,DataLen%16); //����16�ֽڵĿ鲹�㴦��
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

/*----------------- ���ⲿ�������ýӿ�-------------------*/
/************************************************************
������:     AesBlockDecrypt
��������:   Aes �ӽ��ܺ���

��ڲ���:       Direct,*ChiperDataBuf,DataLen
��ڲ�����˵��: 
			    Direct: 0  ����  
				        1  ����
				*ChiperDataBuf�����ܽ��� ���ݵ����
                 DataLen�����ݳ��� ����ʮ���ֽڣ�ֱ�Ӳ�0 
                         
                
���ڲ�����      *ChiperDataBuf�����ܽ��� ���ݵĳ��ڵ�ַ
���ڲ���˵����  ���ܽ��� ���ݵĳ��ڵ�ַ
                
����ֵ��        byte ����
����ֵ��˵��:   ���ܽ����������� 16���ֽڣ������� ���� 0
************************************************************/
byte AesBlockDecrypt(byte Direct,byte *ChiperDataBuf,byte DataLen)
{
	byte st;
	byte i;
	byte temp[4];
	if( Direct == 0 )//����
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
	else if( Direct == 1 )//����
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

