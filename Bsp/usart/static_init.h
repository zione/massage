#ifndef _STATIC_INIT_H_	
#define _STATIC_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f10x.h"
#include "String.h"				//memcpy
#include <stdio.h>   			//printf


#define DebugPf			printf			//����printf���

typedef struct
{
	u32 ValidFlag;				//�궨����
	float  WeightVal[10];		//����ֵ
	float  VotageVal[10];		//ģ��ֵ
} CALIBRATION;					//ţ�ٲ�ֵ�궨

typedef struct
{
	u32			*QStart;		        //��ʼλ��
	u32			*QEnd;		            //����λ��
	u32			*QIn;				    //����λ��
	u32			*QOut;				    //���λ��
	u32			Count;				    //����
} FBuff;

extern u8 test;						//�����¶ϵ���
extern CALIBRATION CalibrationSave;	//ţ�ٲ�ֵ�궨

void NOPDelay(void);					//NOPָ�������ӳ�
void L_From_B(float* Val,u32 size);	//ð����С����

u32 Get_SyS_CLK	(u8 temp);			//��ǰϵͳ����Ƶ��״̬��ȡ	   ��ƵƵ��(0) HCLKƵ��(1) PCLK1Ƶ��(2) PCLK2Ƶ��(3) SYSTEMƵ��(����)
u16 StoB_2_BtoS(u16 n);				//12ת21
u32 StoB_4_BtoS(u32 n);				//1234ת4321 ����
float StoB_F4_BtoS(float f);			//1234ת4321 ������

u8 AsciiToHex(char ch);				//����ASCII��A-F.0-9ת16����0x0A-0x0F.0x00-0x09
char HexToAscii(u8 nHex);			//����16����0x0A-0x0F.0x00-0x09תASCII��A-F.0-9
void BufferAsciiToHex(u8 *pAaciiBuf, u8 *pHexBuf, u16 nAsciiLen);	 	//���ASCII��ת16����	 4λת2λ
void BufferHexToAscii(u8 *pHexBuf,u8 *pAaciiBuf , u16 nHexLen);			//���16����תASCII��	 2λת4λ

void GetCpuIdHex8(u8* HCpuId);		//��ȡCPUоƬIDֵ8λ u8 x[12];
void GetCpuIdHex16(u16 *HCpuId);		//��ȡCPUоƬIDֵ16λ u16 x[6];
void GetCpuIdHex32(u32 *HCpuId);		//��ȡCPUоƬIDֵ32λ u32 x[3];

u8 NewTon_Num(u16 flag);									//���ҵ�ǰ�궨ʹ�ø���
void NewTon_Num_init(CALIBRATION* CS,u8 num_Temp);		//ţ�ٲ��̼���
float Ca_NewTon(float ADC_temp,CALIBRATION* CS,u8 x);	//ţ�ٲ�ֵ����
long Sample_Wave(float* AD_New,float* AD_Old,long ADOld_Len,float Wave_Temp);	//����˲�
u32 ASCIItoNumber(u8* data_temp,u8 len_temp);			//ASCIIת����
double ASCIItoStarNumber(u8* data_temp,u8 len_temp,u8 RealLen);	//ASCIIתС��
u32 BeiNumber(u32 num_temp,u8 len_temp);					//���������λ�����з���
u8 JZ_chack(void);  										//8M������������ OSC_IN(5,12,23)	OSC_OUT(6,13,24)
void RCC_USEHSI(void);									//ʹ��STM32�ڲ�RCʱ�� ����ȷ ��
void System_Reset(void);									//ϵͳ����
u32 AddKey(u32 Data);			//����Կ
u32 SubKey(u32 Data);			//����Կ
u32 Fsave(FBuff* FQ,u32 val,u32 BuffLen);				//�����ȳ���
u8 Fget(u32* val,FBuff* FQ);				//����ȡ
u8 FindBo(u32 val,u32* GetBoVal);			//�Ҳ���
char* FindString(char* Data,u32 DataLen,char* StringVal,u32 StringLen);	//�ַ�������
u32 FindNoZero(u8* Data,u32 DataLen);		//�ҷ������

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
