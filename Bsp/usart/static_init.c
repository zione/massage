#include "static_init.h"

//		__enable_irq();  		//���ж�
//		__disable_irq(); 		//���ж�
//		NVIC_SystemReset();		//ϵͳ������ 

/*˵���������жϲ���
	u8 x=0;
	u8 val=0;
	u8 flag=0;

	DebugPf("���ַ����ղ��Կ�ʼ\r\n");
	do
	{
		flag=USART1_GetByte_Wait(&val,72000);
		if(flag==1)
		{
			DebugPf("\r\n");
			DebugPf("-- %c --\r\n",val);
		}
		else
		{
			DebugPf("%d\r\n",x++);
		}
		if(val=='S'||val=='s')
		{
			DebugPf("�ر��ж�\r\n");
			__disable_irq();		 //�ر��ж�
		}
		if(x==20)
		{
			DebugPf("���ж�\r\n");	
			x=0;
			__enable_irq();  		//���ж�
		}
		val=0;
		Delay_S(1);	
	}while(val!=26);	//SUB CTRL+Z
*/

u8 test=0;					//�����¶ϵ���
u8 HardFault_Handler_X = 0;	//�Ŵ���

/****************************************************************************
* ��	�ƣ�void Delay(u32 nCount)
* ��	�ܣ�һ��������Ƶ����ʱ
* ��ڲ�����u32 nCount	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
//void Delay(u32 nCount)
//{
//  while(nCount--);
//}

/****************************************************************************
* ��	�ƣ�void Delay_US(u32 nCount_temp)
* ��	�ܣ�΢�뼶
* ��ڲ�����u32 nCount_temp	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay_US(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8;
	while(nCount--);
}

/****************************************************************************
* ��	�ƣ�void Delay_MS(u32 nCount_temp)
* ��	�ܣ����뼶
* ��ڲ�����u32 nCount_temp	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay_MS(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8000;
	while(nCount--);
}

/****************************************************************************
* ��	�ƣ�void Delay_S(u32 nCount_temp)
* ��	�ܣ��뼶
* ��ڲ�����u32 nCount_temp	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay_S(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8000000;
	while(nCount--);
}

/****************************************************************************
* ��	�ƣ�__asm void NOPDelay(void)
* ��	�ܣ�NOPָ�������ӳ�	   (���һ���ӳ�1΢��)
* ��ڲ�������
* ���ڲ�������
* ˵	��������һ����ʱһ��ϵͳʱ������	  
****************************************************************************/
__asm void NOPDelay(void)	  //NOPָ�����ڼ��ӳ�
{
	NOP;
}

/****************************************************************************
* ��	�ƣ�u32 Get_SyS_CLK(u8 temp)
* ��	�ܣ���ǰϵͳ����Ƶ��״̬��ȡ	   
* ��ڲ�����temp ѡ��Ƶ�ʲ���
* ���ڲ�����ʱ��Ƶ��
* ˵	������ƵƵ��(0) HCLKƵ��(1) PCLK1Ƶ��(2) PCLK2Ƶ��(3) SYSTEMƵ��(����)
			x=Get_SyS_CLK(1); //��ƵƵ��
			DebugPf("ADCCLK=%ldMHz\r\n",x);
			x=Get_SyS_CLK(2); //HCLKƵ��
			DebugPf("HCLK=%ldMHz\r\n",x);
			x=Get_SyS_CLK(3); //PCLK1Ƶ��
			DebugPf("PCLK1=%ldMHz\r\n",x);
			x=Get_SyS_CLK(4); //PCLK2Ƶ��
			DebugPf("PCLK2=%ldMHz\r\n",x);
			x=Get_SyS_CLK(5); //SYSTEMƵ�� //����
			DebugPf("SYSCLK=%ldMHz\r\n",x);			
****************************************************************************/
u32 Get_SyS_CLK(u8 temp)
{
	RCC_ClocksTypeDef RCC_ClockFreq;
	RCC_GetClocksFreq(&RCC_ClockFreq);	//Ϊʱ�ӱ�������Ƭ�ϸ�ʱ��
	switch (temp)
	{
		case 1:	return RCC_ClockFreq.ADCCLK_Frequency; 	//��ƵƵ��
		case 2:	return RCC_ClockFreq.HCLK_Frequency; 	//HCLKƵ��
		case 3:	return RCC_ClockFreq.PCLK1_Frequency; 	//PCLK1Ƶ��
		case 4:	return RCC_ClockFreq.PCLK2_Frequency; 	//PCLK2Ƶ��
		default: return RCC_ClockFreq.SYSCLK_Frequency;	//SYSTEMƵ��
	}	
}

/****************************************************************************
* ��	�ƣ�u16 StoB_2_BtoS(u16 n)				 //12ת21
* ��	�ܣ�2�ֽڵĴ�С��ת��	   
* ��ڲ�����n ת��ǰ����
* ���ڲ�����ת�������
* ˵	������ unsigned int 2�ֽڵ���
****************************************************************************/
u16 StoB_2_BtoS(u16 n)				 //12ת21
{
	char nTmp;

	nTmp = ((char*)&n)[0];
	((char*)&n)[0] = ((char*)&n)[1];
	((char*)&n)[1] = nTmp;

	return n;
}

/****************************************************************************
* ��	�ƣ�u32 StoB_4_BtoS(u32 n)				  //1234ת4321
* ��	�ܣ�4�ֽڵĴ�С��ת��(����)	   
* ��ڲ�����n ת��ǰ����
* ���ڲ�����ת�������
* ˵	������ long int 4�ֽڵ���
****************************************************************************/
u32 StoB_4_BtoS(u32 n)				  //1234ת4321
{
	char nTmp;

	nTmp = ((char*)&n)[0];
	((char*)&n)[0] = ((char*)&n)[3];
	((char*)&n)[3] = nTmp;

	nTmp = ((char*)&n)[1];
	((char*)&n)[1] = ((char*)&n)[2];
	((char*)&n)[2] = nTmp;	

	return n;
}

/****************************************************************************
* ��	�ƣ�float StoB_F4_BtoS(float f)				   //1234ת4321
* ��	�ܣ�4�ֽڵĴ�С��ת��(������)		   
* ��ڲ�����n ת��ǰ����
* ���ڲ�����ת�������
* ˵	������ float 4�ֽڵ���
****************************************************************************/
float StoB_F4_BtoS(float f)				   //1234ת4321
{
	char nTmp;

	nTmp = ((char*)&f)[0];
	((char*)&f)[0] = ((char*)&f)[3];
	((char*)&f)[3] = nTmp;

	nTmp = ((char*)&f)[1];
	((char*)&f)[1] = ((char*)&f)[2];
	((char*)&f)[2] = nTmp;	

	return f;
}

/****************************************************************************
* ��	�ƣ�u8 AsciiToHex(char ch)
* ��	�ܣ�����ASCII��ת16����		   
* ��ڲ�����ch	'A'-'F'��'0'-'9'
* ���ڲ�����ת�����0x0A-0x0F.0x00-0x09
* ˵	����ASCII��A-F.0-9ת16����0x0A-0x0F.0x00-0x09
****************************************************************************/
u8 AsciiToHex(char ch)
{
	if(ch >= '0' && ch <= '9')	//����0~9
	{
		return (ch - 0x30);
	}
	else
	{
		if(ch >= 'A' && ch <= 'F')	//��ĸA~F
		{
			return (ch - 0x37);
		}
		else
		{
			return 0xFF;
		}
	}
}

/****************************************************************************
* ��	�ƣ�char HexToAscii(u8 nHex)
* ��	�ܣ�����16����תASCII��		   
* ��ڲ�����nHex	0x0A-0x0F.0x00-0x09
* ���ڲ�����ת�����ASCII��
* ˵	����16����0x0A-0x0F.0x00-0x09תASCII��A-F.0-9
****************************************************************************/
char HexToAscii(u8 nHex)
{
	if(nHex <= 9)	//����0~9
	{
		return (nHex + 0x30);
	}
	else
	{
		if(nHex >= 0x0A && nHex <= 0x0F)	//��ĸA~F
		{
			return (nHex + 0x37);
		}
		else
		{
			return 0xFF;
		}
	}
}

/****************************************************************************
* ��	�ƣ�void BufferAsciiToHex(u8 *pAaciiBuf, u8 *pHexBuf, u16 nAsciiLen)
* ��	�ܣ����ASCII��ת16����		   
* ��ڲ�����*pAaciiBuf	ASCII����������	   Դ
			*pHexBuf	16���ƴ������	   ���
			nAsciiLen	ת������
* ���ڲ�������
* ˵	����u8 x[4]="ABCD";
			u8 z[2]={0};		ת������{0xAB,0xCD}		 4λת2λ
****************************************************************************/
void BufferAsciiToHex(u8 *pAaciiBuf, u8 *pHexBuf, u16 nAsciiLen)
{
	u16 i;
	u8 temp;
	if(pAaciiBuf == 0)
		return;
	if(pHexBuf == 0)
		return;

	for(i=0; i<nAsciiLen; i+=2)
	{
		temp = AsciiToHex(pAaciiBuf[i]);
		if(temp == 0xFF)
			return;

		pHexBuf[i>>1] = temp;//��4λ
		pHexBuf[i>>1] <<= 4;

		temp = AsciiToHex(pAaciiBuf[i+1]);
		if(temp == 0xFF)
			return;

		pHexBuf[i>>1] += temp;//��4λ
	}
}


/****************************************************************************
* ��	�ƣ�void BufferHexToAscii(u8 *pHexBuf,u8 *pAaciiBuf , u16 nHexLen)
* ��	�ܣ����16����תASCII��	   
* ��ڲ�����*pAaciiBuf	ASCII��������	   ���
			*pHexBuf	16������������	   Դ
			nHexLen	ת������
* ���ڲ�������
* ˵	����u8 x[2]={0xAB,0XCD};
			u8 z[4]={0};		ת������{65,66,67,68}		 2λת4λ
****************************************************************************/
void BufferHexToAscii(u8 *pHexBuf,u8 *pAaciiBuf , u16 nHexLen)
{
	u16 i;
	if(pAaciiBuf == 0)
		return;
	if(pHexBuf == 0)
		return;

	for(i=0; i<nHexLen; i++)
	{
		pAaciiBuf[i<<1] = HexToAscii(pHexBuf[i]>>4);
		if(pAaciiBuf[i] == 0xFF)
			return;

		
		pAaciiBuf[(i<<1)+1] = HexToAscii(pHexBuf[i] & 0x0F);
		if(pAaciiBuf[(i<<1)+1] == 0xFF)
			return;
	}
}

/****************************************************************************
* ��	�ƣ�void GetCpuIdHex32(u32 *HCpuId)
* ��	�ܣ���ȡCPUоƬIDֵ32λ   
* ��ڲ�������
* ���ڲ�������
* ˵	����3��u32��ֵ���CPUID
			u32 x[3]={0};
			0x22A6FF32
			0x32375931
			0x43097342
****************************************************************************/
void GetCpuIdHex32(u32 *HCpuId)
{
	HCpuId[0] = (*(u32*)(0x1FFFF7E8));
	HCpuId[1] = (*(u32*)(0x1FFFF7EC));
	HCpuId[2] = (*(u32*)(0x1FFFF7F0));
}

/****************************************************************************
* ��	�ƣ�void GetCpuIdHex16(u16 *HCpuId)
* ��	�ܣ���ȡCPUоƬIDֵ16λ   
* ��ڲ�������
* ���ڲ�������
* ˵	����6��u16��ֵ���CPUID
			u16 x[6]={0};
			0xFF32
			0x22A6
			0x5931
			0x3237
			0x7342
			0x4309
****************************************************************************/
void GetCpuIdHex16(u16 *HCpuId)
{
	*&HCpuId[0] = (*(u32*)(0x1FFFF7E8));
	*&HCpuId[1] = (*(u32*)(0x1FFFF7EA));
	*&HCpuId[2] = (*(u32*)(0x1FFFF7EC));
	*&HCpuId[3] = (*(u32*)(0x1FFFF7EE));
	*&HCpuId[4] = (*(u32*)(0x1FFFF7F0));
	*&HCpuId[5] = (*(u32*)(0x1FFFF7F2));
}

/****************************************************************************
* ��	�ƣ�void GetCpuIdHex8(u8* HCpuId)
* ��	�ܣ���ȡCPUоƬIDֵ8λ   
* ��ڲ�������
* ���ڲ�������
* ˵	����12��u8��ֵ���CPUID
			u8 x[12]={0};
			0x22 0xA6 0xFF 0x32 0x32 0x37 0x59 0x31 0x43 0x09 0x73 0x42
****************************************************************************/
void GetCpuIdHex8(u8* HCpuId)
{
	u32 CpuId[3]={0};

	CpuId[0] = (*(u32*)(0x1FFFF7E8));
	CpuId[1] = (*(u32*)(0x1FFFF7EC));
	CpuId[2] = (*(u32*)(0x1FFFF7F0));

   	HCpuId[0] = (u8)(CpuId[0]>>24);
	HCpuId[1] = (u8)(CpuId[0]>>16);
	HCpuId[2] = (u8)(CpuId[0]>>8);
	HCpuId[3] = (u8)(CpuId[0]);
	HCpuId[4] = (u8)(CpuId[1]>>24); 
	HCpuId[5] = (u8)(CpuId[1]>>16);
	HCpuId[6] = (u8)(CpuId[1]>>8);
	HCpuId[7] = (u8)(CpuId[1]);
	HCpuId[8] = (u8)(CpuId[2]>>24); 
	HCpuId[9] = (u8)(CpuId[2]>>16);
	HCpuId[10] = (u8)(CpuId[2]>>8);
	HCpuId[11] = (u8)(CpuId[2]);
	
	
		
}


/****************************************************************************
* ��	�ƣ�void L_From_B(float* Val,u32 size)   //ð��������С����
* ��	�ܣ�����������������С��������
* ��ڲ�����float* Val		��Ҫ���������
			u32 size 		�������
* ���ڲ�������
* �㷨˵������������С��������
****************************************************************************/
void L_From_B(float* Val,u32 size)   //ð��������С����
{ 
	float temp;	//��ʱ���
	u32 i=0;  	//�Ա���
	u32 j=0;   	//�Ա���

	for(;i<size-1;++i)  
	{  
		for(j=0;j<size-i-1;++j)  
		{   
			if(Val[j]>Val[j+1])  
			{
				//��������ֵ
				temp=Val[j];
				Val[j]=Val[j+1];
				Val[j+1]=temp;
			}	
		}  
	} 
}

/****************************************************************************
* �� �� ����ţ�ٲ�ֵ
* ��	�ƣ�float Ca_NewTon(void)
* ��	�ܣ�ţ�ٲ�ֵ����
* ��ڲ�������
* ���ڲ�����0��		�д����ȱ����Ч�궨���� 
			����:	ת�����
* �㷨˵����
	WeighParam_Ten	����ֵ
	ADCAverage		ģ��ֵ

	u8 NewTon_Num(u16 flag);	//���ұ궨����
	void NewTon_Num_init(CALIBRATION* CS,u8 num_Temp);	//����ţ�ٲ���

* ���ԣ�
	CALIBRATION CalibrationSave={15,{8.75F,47.35F,107.85F,146.1},{1.035F,1.404F,1.979F,2.342}};
	u8 num;
	float weight;		  //127.0
	num=NewTon_Num(CalibrationSave.ValidFlag);	//���ҵ�ǰ�궨ʹ�ø���
	NewTon_Num_init(&CalibrationSave,num);		//ţ�ٲ��̼���
	weight=Ca_NewTon(2.161F,&CalibrationSave,num);	//ţ�ٲ�ֵ����
****************************************************************************/
CALIBRATION CalibrationSave={15,			  //�궨����
		{8.75F,47.35F,107.85F,146.1F},		  //����ֵ
		{1.035F,1.404F,1.979F,2.342F}};		  //ģ��ֵ

//���ҵ�ǰ�궨ʹ�ø���
//����	u16 flag	�궨������¼ 	������
//����	u8			�궨���� 		ʮ����
u8 NewTon_Num(u16 flag)
{
	u8 number=0;
	for(;( flag & 0x01==1);)
	{
		number++;
		flag=flag>>1;
	}
	return number;
}

//ţ�ٲ��̼���
//����	CALIBRATION* CS	�궨
//		u8 num_Temp		�궨���� ʮ����
void NewTon_Num_init(CALIBRATION* CS,u8 num_Temp)
{
	float sum;			   //�����
	float mul;			   //�����
	float temp[10]={0.0F};	   //��ʱ�����ŵ�
	u8 k;	//�ⲿ�Ա���
	u8 i;	//�궨�±���
	u8 j;	//�ڲ��Ա���

	for (i = 0; i < num_Temp; i++)
	{
		for (sum = 0.0F, k = 0; k <= i; k++)
		{
			for (mul = 1.0F, j = 0; j <= i; j++)
			{
				if (j != k)
				{
					mul = mul * (CS->VotageVal[k] - CS->VotageVal[j]);
				}
			}
			sum = sum + CS->WeightVal[k] / mul;
		}
		temp[i] = sum;
	}
	memcpy(CS->WeightVal,temp,4*10);
}

//ţ�ٲ�ֵ����
//����	float ADC_temp	��ǰģ��ֵ
//		CALIBRATION* CS	�궨
//		u8 x			�궨ʹ�ø���	ʮ����
float Ca_NewTon(float ADC_temp,CALIBRATION* CS,u8 x)
{
    u8 i,j;
	float OutYP,mul;

	for (OutYP = CS->WeightVal[0], i = 1; i < x; i++)
	{
		for (mul = CS->WeightVal[i], j = 0; j < i; j++)
		{
			mul = mul * (ADC_temp - CS->VotageVal[j]);
		}	
		OutYP = OutYP + mul;
	}
    return OutYP;
}

/****************************************************************************
* ��	�ƣ�long Sample_Wave(float* AD_New,float* AD_Old,long ADOld_Len,float Wave_Temp)
* ��	�ܣ�����˲�
* ��ڲ�����float* AD_New	����˲���ɺ����������
			float* AD_Old	�����δ�����˲�����������
			long ADOld_Len	δ�˲������ݸ���
			float Wave_Temp	��������ֵ��ƫ�Χ
* ���ڲ�����long	�˲������ݸ���
* �㷨˵����x[10]
x[1]��x[0]�Ƚϣ�����ƫ��ֵ��Ҫ��δ������ȡ��
x[2]��x[1]�Ƚϣ�����
���ԣ�
#define old_len 100		  //δ�˲������ݸ���
float Wave=1000.0F;		  //��������ֵ��ƫ�Χ
float old[100]={0.0F};	  //�����δ�����˲�����������
float new[100]={0.0F};	  //����˲���ɺ����������
long i=0;	  //�ۼ���
long x=0;	  //�˲������ݸ���
	for(i=0;i<100;i++)
		old[i]=i*2;
	old[10]=1000.0F;
	old[20]=2000.0F;
	old[30]=3000.0F;
	old[40]=4000.0F;	  //׼����������

	x=Sample_Wave(new,old,old_len,Wave);	//�˲�
****************************************************************************/
long Sample_Wave(float* AD_New,float* AD_Old,long ADOld_Len,float Wave_Temp)
{
	long ko=1; //δ�����˲��������±�
	long kn=1; //�˲����������±�

	AD_New[0]=AD_Old[0];		  //��һ��ֱ�Ӹ�ֵ

	for(ko=1;ko<ADOld_Len;ko++)	  //δ�˲������ͷ��β����
	{
		if(fabs(AD_Old[ko]-AD_New[kn-1])<=Wave_Temp)	  //�����������ľ��Բ�ֵ������ֵ���бȽ�
		{
			AD_New[kn]=AD_Old[ko];	//δ�����ͽ���ȡֵ
			kn++;	//�˲������±�����
		}
	}
	return kn;
}

/****************************************************************************
* ��	�ƣ�u32 BeiNumber(u32 num_temp,u8 len_temp)
* ��	�ܣ�10����
* ��ڲ�����u32 num_temp	��Ҫ��������
			u8 len_temp	�����ٱ� ��1������ٸ�0
* ���ڲ�����u32 ������ı���ֵ
* ���Դ��룺
	u32 x=2;
	x=BeiNumber(x,3);
	//x=2000;
****************************************************************************/
u32 BeiNumber(u32 num_temp,u8 len_temp)
{
     u32 Bei=1;		  //Ĭ�ϱ���ֵ
	 	
     while(len_temp--)//���������з���
          Bei*=10;	  //��������
     return Bei*num_temp;	  //���ر���
}

/****************************************************************************
* ��	�ƣ�u32 ASCIItoNumber(u8* data_temp,u8 len_temp)
* ��	�ܣ���ASCII������ת����Ϊu32������
* ��ڲ�����u8* data_temp	ASCII������
			u8 len_temp		ASCII���������ָ���
* ���ڲ�����u32 unsigned int������
* ���Դ��룺
	u32 num=0;
	num=ASCIItoNumber("1234",4);
	//num=1234;

	num = ASCIItoNumber("123.45678",3);
	num = 123;
	num = ASCIItoNumber("35.124",2);
	num = 35;
	num = ASCIItoNumber("123.458",3);
	num = 123;
	num = ASCIItoNumber("3.14159",1);
	num = 3;
****************************************************************************/
u32 ASCIItoNumber(u8* data_temp,u8 len_temp)
{
    u8 i = 0;					//����
    u32 val=0; 
    
    while(1)
    {
        val = val * 10 + data_temp[i] - 0x30;     //ȡ���� 
        
        i++;				   //����ĸ�������
        if(i >= len_temp)	   //����ĸ�������ϣ������ĸ���
           return val;
    } 
}

/****************************************************************************
* ��	�ƣ�double ASCIItoStarNumber(u8* data_temp,u8 len_temp,u8 RealLen)
* ��	�ܣ���ASCII������ת��С��
* ��ڲ�����u8* data_temp	ASCII������
			u8 len_temp		С������
			u8 RealLen		ʵ�������ܳ��ȸ���
* ���ڲ�����double	С��
* ���Դ��룺
	double num = 0.0F;

	num = ASCIItoStarNumber("123.45678",5,9);
	//num = 0.45678;
	num = ASCIItoStarNumber("35.124",3,6);
	//num = 0.124;
	num = ASCIItoStarNumber("123.458",3,7);
	//num = 0.458;
	num = ASCIItoStarNumber("3.14159",5,7);
	//num = 0.14159
****************************************************************************/
double ASCIItoStarNumber(u8* data_temp,u8 len_temp,u8 RealLen)
{
    u8 i = RealLen - 1;		//����
	u8 fi = 0;				//�����˵ĸ���

    double val = 0.0F;

    while(1)
    {
		val = val * 0.1F + (data_temp[i] - 0x30) * 0.1F; 	  //�������С��
		
		i--;					  //�±�λ�ñ仯
		fi++;					  //����ĸ�������

		if(fi >= len_temp)		  //����ĸ�������ϣ������ĸ���
		   return (double)val;
    } 
}

/****************************************************************************
* ��	�ƣ�u8 JZ_chack(void)
* ��	�ܣ�8M������������ 
* ��ڲ�������
* ���ڲ�����u8	1����	0û����
* ˵    ����OSC_IN(5,12,23)	
			OSC_OUT(6,13,24)
* ���Դ���			
	flag=JZ_chack();
	if(flag==1)		//����
		flag=9;
	else			//û����
		flag=9;
****************************************************************************/
u8 JZ_chack(void)
{
	u8 flag=0;		 //������������ʱ

	RCC_DeInit();			 		//������ RCC�Ĵ�������Ϊȱʡֵ
	RCC_HSEConfig(RCC_HSE_ON);		//�����ⲿ8M���پ���(HSE)
									//RCC_HSE_OFF  		HSE 	���� OFF 
									//RCC_HSE_ON  		HSE 	���� ON 
									//RCC_HSE_Bypass  	HSE 	�����ⲿʱ����·
	flag = RCC_WaitForHSEStartUp();	//�ȴ� HSE ���񣬸ú������ȴ�ֱ�� HSE �����������ڳ�ʱ��������˳� 
	if(flag == SUCCESS)
		return 1;
	else
		return 0;
}

/****************************************************************************
* ��	�ƣ�void RCC_USEHSI(void)
* ��	�ܣ�����STM32�ڲ�RC����
* ��ڲ�������
* ���ڲ�������
* ˵	����
		���ʹ���ڲ�RC��������ʹ���ⲿ�����밴�����淽������
		1 ����100�Ż�144�ŵĲ�Ʒ��OSC_INӦ�ӵأ�OSC_OUTӦ���ա�
		2 ��������100�ŵĲ�Ʒ����2�ֽӷ���
			2.1 OSC_IN��OSC_OUT�ֱ�ͨ��10K����ӵء��˷��������EMC���ܡ�
			2.2 �ֱ���ӳ��OSC_IN��OSC_OUT��PD0��PD1��������PD0��PD1Ϊ������������'0'���˷������Լ�С���Ĳ�(�������2.1)��ʡ2���ⲿ���衣
* ��    �ԣ�
	u32 x=0;
	while(1)
	{
		DebugPf("--- %d ---\r\n",x);
		x++;
		Delay_S(1);
	}
****************************************************************************/
void RCC_USEHSI(void)
{
	GPIO_InitTypeDef X;

	//PD0 PD1����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE );	//�Զ˿�D����APB2����ʱ�ӣ��������˿�	  ��ӳ��OSC_IN(PD0) OSC_OUT(PD1)

	//PD0_OSC_IN
	X.GPIO_Pin = GPIO_Pin_0;			//��Ҫʹ�õĶ˿������趨
  	X.GPIO_Mode = GPIO_Mode_Out_PP;		//��Ҫʹ�õĶ˿�����ģʽ�趨PPΪ�������
  	X.GPIO_Speed = GPIO_Speed_50MHz;	//��Ҫʹ�õĶ˿�����Ƶ���趨50MHz
  	GPIO_Init(GPIOD, &X);				//��ʼ��GPIOD�Ĵ���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE );	//�Զ˿�D����APB2����ʱ�ӣ��������˿�
	
	//PD1_OSC_OUT
	X.GPIO_Pin = GPIO_Pin_1;			//��Ҫʹ�õĶ˿������趨
  	X.GPIO_Mode = GPIO_Mode_Out_PP;		//��Ҫʹ�õĶ˿�����ģʽ�趨PPΪ�������
  	X.GPIO_Speed = GPIO_Speed_50MHz;	//��Ҫʹ�õĶ˿�����Ƶ���趨50MHz
  	GPIO_Init(GPIOD, &X);				//��ʼ��GPIOD�Ĵ���

	GPIO_ResetBits(GPIOD, GPIO_Pin_0);		//OSC_IN
	GPIO_ResetBits(GPIOD, GPIO_Pin_1);		//OSC_OUT
		
	GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);			//��ӳ��OSC_IN OSC_OUT

	RCC_DeInit();						//������ RCC�Ĵ�������Ϊȱʡֵ 
	RCC_HSICmd(ENABLE);					//ʹ�ܻ���ʧ���ڲ����پ���(HSI) 
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET){}	  //HSI �������	HSI�ڲ����� HSE�ⲿ����
	
	if(1)	   //ʼ��ִ�У�����رգ��ڲ�ʱ������
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);		//ʹ��Ԥȡָ����
		FLASH_SetLatency(FLASH_Latency_2);		   	//���ô�����ʱֵΪ2����ʱ���� 
		RCC_HCLKConfig(RCC_SYSCLK_Div1);		   	//���� AHB ʱ��(HCLK)
		RCC_PCLK1Config(RCC_HCLK_Div2);			   	//���õ��� AHB ʱ��(PCLK1)
		RCC_PCLK2Config(RCC_HCLK_Div1);			   	//���ø��� AHB ʱ��(PCLK2)
													//RCC_HCLK_Div1   APB1 ʱ�� = HCLK 
													//RCC_HCLK_Div2   APB1 ʱ�� = HCLK / 2 
													//RCC_HCLK_Div4   APB1 ʱ�� = HCLK / 4 
													//RCC_HCLK_Div8   APB1 ʱ�� = HCLK / 8 
													//RCC_HCLK_Div16  APB1 ʱ�� = HCLK / 16 
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_2);	 //���� PLL ʱ��Դ����Ƶϵ��
													//RCC_PLLSource_HSI_Div2  PLL ������ʱ�� = HSIʱ��Ƶ�ʳ��� 2 
													//RCC_PLLSource_HSE_Div1  PLL ������ʱ�� = HSEʱ��Ƶ�� 
													//RCC_PLLSource_HSE_Div2  PLL ������ʱ�� = HSE ʱ��Ƶ�ʳ��� 2 
													//RCC_PLLMul_2  PLL ����ʱ�� x 2 
													//RCC_PLLMul_3  PLL ����ʱ�� x 3 
													//RCC_PLLMul_4  PLL ����ʱ�� x 4 
													//RCC_PLLMul_5  PLL ����ʱ�� x 5 
													//RCC_PLLMul_6  PLL ����ʱ�� x 6 
													//RCC_PLLMul_7  PLL ����ʱ�� x 7 
													//RCC_PLLMul_8  PLL ����ʱ�� x 8 
													//RCC_PLLMul_9  PLL ����ʱ�� x 9 
													//RCC_PLLMul_10  PLL ����ʱ�� x 10 
													//RCC_PLLMul_11  PLL ����ʱ�� x 11 
													//RCC_PLLMul_12  PLL ����ʱ�� x 12 
													//RCC_PLLMul_13  PLL ����ʱ�� x 13 
													//RCC_PLLMul_14  PLL ����ʱ�� x 14 
													//RCC_PLLMul_15  PLL ����ʱ�� x 15 
													//RCC_PLLMul_16  PLL ����ʱ�� x 16 
		RCC_PLLCmd(ENABLE);//���PLL������ϵͳʱ��,��ô�����ܱ�ʧ��	  //ʹ�ܻ���ʧ�� PLL,�����������ȡ��ENABLE����DISABLE
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}	 //�ȴ�ָ���� RCC ��־λ���óɹ� �ȴ�PLL��ʼ���ɹ�
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);	//����ϵͳʱ��(SYSCLK)����PLLΪϵͳʱ��Դ
		while(RCC_GetSYSCLKSource() != 0x08){}		//�ȴ�PLL�ɹ�������ϵͳʱ�ӵ�ʱ��Դ
													// 0x00��HSI ��Ϊϵͳʱ��
													// 0x04��HSE ��Ϊϵͳʱ��
													// 0x08��PLL ��Ϊϵͳʱ��
	}
}

/****************************************************************************
* ��	�ƣ�void System_Reset(void)
* ��	�ܣ�ϵͳ������
* ��ڲ�������
* ���ڲ�������
* ˵	����
		__enable_irq()  //���ж�
		__disable_irq() //���ж� 
* ��    �ԣ�
//������ʽ1��
	u8 x=0;
	DebugPf("START\r\n");
	while(1)
	{
		DebugPf("%d\r\n",x++);
		if(x==5)				//����10����
		{
			System_Reset();
			while(1){;}			//�����λʧ������������
		}
		Delay_S(1);
	}
	//__asm	���ָ��

//������ʽ2��	
	//NVIC_SystemReset();	//�̼����Դ���������
	u8 x=0;
	DebugPf("START\r\n");
	while(1)
	{
		DebugPf("%d\r\n",x++);
		if(x==5)				//����10����
		{
			__disable_irq();	//��ֹһ���жϲ��� 
			NVIC_SystemReset();
			while(1){;}			//�����λʧ������������
		}
		Delay_S(1);
	}
****************************************************************************/
__asm void System_Reset(void) 
{ 
	MOV R0, #1           //  
	MSR FAULTMASK, R0    // ���FAULTMASK ��ֹһ���жϲ��� 
	LDR R0, =0xE000ED0C  // 
	LDR R1, =0x05FA0004  //  
	STR R1, [R0]         // ϵͳ�����λ    
}

/****************************************************************************
* ��	�ƣ�u32 AddCode(u32 Data)
* ��	�ܣ���ԭʼ����������Կ 
* ��ڲ�����u32 Data	ԭʼ����
* ���ڲ�����u32	����Կ�������
* ˵    ������
****************************************************************************/
u32 AddKey(u32 Data)
{
	Data=rotr(Data,13);	  //ѭ������13λ
	return Data;
}

/****************************************************************************
* ��	�ƣ�u32 SubKey(u32 Data)
* ��	�ܣ�������Կ�����ݼ�ȥ��Կ 
* ��ڲ�����u32 Data	����Կ�������
* ���ڲ�����u32	ԭʼ����
* ˵    ������
****************************************************************************/
u32 SubKey(u32 Data)
{
	Data=rotl(Data,13);	  //ѭ������13λ
	return Data;
}

/*
	u32 srcval=0xABCDEF12;
	u32 srcval_T=0;

	srcval_T = AddKey(srcval);
	DebugPf("Src = %X\t A = %X\r\n",srcval,srcval_T);
	srcval_T = SubKey(srcval);	
	DebugPf("Src = %X\t S = %X\r\n",srcval,srcval_T);
	DebugPf("Src = %X\t S = %X\r\n",srcval,AddKey(srcval_T));
*/

/****************************************************************************
* ��	�ƣ�void FIFO(u32* x,u32 len, u32 val)
* ��	�ܣ��Ƚ��ȳ�����	����
* ��ڲ�����u32* x			��Ż�����
			u32 len			����������
			u32 val			����ֵ
* ���ڲ�������
* ˵	������
****************************************************************************/
void FIFO(u32* x,u32 len, u32 val)
{
	u32 i;
	for(i = 0;i < (len - 1);i++) 
	{
		x[i] = x[i + 1];
	}
	x[len - 1] = val;   
}

/****************************************************************************
* ��	�ƣ�u32 Fsave(FBuff* FQ,u32 val,u32 BuffLen)
* ��	�ܣ����д�ֵ
* ��ڲ�����FBuff* FQ	��Ŷ���
			u32* val	�����ֵ
			u32 BuffLen	���������
* ���ڲ�����u32			��ǰ������еĸ���
* ˵	���������ȳ���	ָ��
****************************************************************************/
u32 Fsave(FBuff* FQ,u32 val,u32 BuffLen)
{
	*(FQ->QIn) = val;			//����λ�ô���
	FQ->QIn++;					//����λ������
	if(FQ->QIn > FQ->QEnd)		//�����ַ���������ַ
		FQ->QIn = FQ->QStart;	//������ʼ��ַ
	if(FQ->Count < BuffLen)		//����ûװ���������
		FQ->Count++;	 			//����װ
	return FQ->Count;			//���ظ���
}

/****************************************************************************
* ��	�ƣ�u8 Fget(u32* val,FBuff* FQ)
* ��	�ܣ�����ȡֵ
* ��ڲ�����u32* val	ȡ����ֵ
			FBuff* FQ	���ݶ���
* ���ڲ�����u8	0		ȡֵʧ��
				1		ȡֵ�ɹ�
* ˵	����������ȡ	ָ��
****************************************************************************/
u8 Fget(u32* val,FBuff* FQ)
{
	if(FQ->Count > 0)				//����������
	{
		FQ->QIn --;					//����λ�ú���
		if(FQ->QIn < FQ->QStart)
			FQ->QIn = FQ->QEnd;	//������ڱȵ�һ��λ�û�С��λ����Ϊ��ʼλ��
		FQ->QOut = FQ->QIn;		//�õ����һ��ֵ
		*val = *(FQ->QOut);			//ȡֵ
		FQ->Count--;					//����ĸ�������
		return 1;						//ȡֵ�ɹ�
	}
	else								//������û��
		return 0;						//ȡֵʧ��
}

/* ���г�ʼ������
#define maxADlen 5				//���ݴ������С
static u32 DownData[maxADlen]={0};	//���ݴ����
FBuff DownAD = 
{
	DownData,				//��ʼλ��	Start
	DownData+(maxADlen-1),		//����λ��	End
	DownData,				//����λ��	In
	DownData,				//���λ��	Out	��ʱ
	0,						//����
};
*/

/****************************************************************************
* ��	�ƣ�u8 FindBo(u32 val,u32* GetBoVal)
* ��	�ܣ��Ҳ���
* ��ڲ�����u32 val			����ADֵ
			u32* GetBoVal	�������ֵ
* ���ڲ�����u8 �Ƿ��в����γ�	1�γ�	0û�γ�
* ˵	�����м����������߼�Ϊ����
****************************************************************************/
static u32 BoBuff[3]={0};
static u8 i=0;
FBuff ADB = 
{
	BoBuff,				//��ʼλ��	Start
	BoBuff+(3-1),		//����λ��	End
	BoBuff,				//����λ��	In
	BoBuff,				//���λ��	Out	��ʱ
	0,						//����
};
u8 FindBo(u32 val,u32* GetBoVal)
{
	if(i<2)	//������ʼ��	װ��ǰ2������
	{
		Fsave(&ADB,val,3);			//װ����
		i++;				
		return 0;			//û�õ�����
	}

	Fsave(&ADB,val,3);			//װ����

	if(BoBuff[1] > BoBuff[0] && BoBuff[1] > BoBuff[2])
	{
		*GetBoVal = BoBuff[1];	//�õ�����ֵ
		return 1;				//�õ�����
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�u8* FindString(u8* Data,u32 DataLen,u8* StringVal,u32 StringLen)
* ��	�ܣ����ַ��������ַ�������λ
* ��ڲ�����u8* Data		����ַ���������Դ
			u32 DataLen		����Դ������
			u8* StringVal	Ҫ���ҵ��ַ���
			u32 StringLen	Ҫ���ҵ��ַ�������
* ���ڲ�����u8*	��λ�ַ���������Դ�е�λ�ã���λ�ַ�����λ��	û�ҵ�����ָ��0��ַ��ָ��
* ˵	����
u8 a[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};	//26
u8 b[1] = 'a';								//1
u8* f = 0;
f = FindString(a,sizeof(a),b,sizeof(b));
****************************************************************************/
char* FindString(char* Data,u32 DataLen,char* StringVal,u32 StringLen)
{
	char* s = 0;		 //Դ��ַָ��
	char*	p = 0;		 //�Ƚ����ݵ�ַָ��
	u32 i = 0;		 //�Ƚ����ݸ���

	if(DataLen == 0 || StringLen == 0 || StringVal == 0 || Data == 0)	 //���ҵ����ݺ�����ԴΪ0����Ϊ��
		return p;

	s = Data;		 //ָ������Դ
	p = StringVal;	 //ָ���������

	while(DataLen > 0)		//Դ����û�Ƚ���
	{
		if(*s != *p)	  	//û����������
		{
			DataLen--;		//�ȽϹ�������
			p = StringVal;	//ָ���������
			if(i != 0)		//ǰ���м��αȽϳɹ��˵�
			{
				s -= (StringLen - 1);		//�Գɹ����ĵڶ�����ʼ���±Ƚ�		abcabcd	�Ƚ�abcd
				i = 0;				//�Ƚϳɹ���������
			} 
		}
		else			  	//�ҵ���������
		{
			i++;			//�Ƚϳɹ���������

			if(i >= StringLen)				//�ҵ�
				return s - (StringLen - 1);	//�Ƶ��Ƚϵ����ַ�λ��

			p++;			//�Ƚ����ݺ���һ��
		}
		s++;				//Դ����λ��
	}

	p = 0;			  //û�ҵ�
	return p;
}

/****************************************************************************
* ��	�ƣ�u32 FindNoZero(u8* Data,u32 DataLen)
* ��	�ܣ��������в��ҷ������
* ��ڲ�����u8* Data		����
			u32 DataLen		����������
* ���ڲ�����u32				�������
* ˵	�����������˳��������;Ϊ0����;�˳�
****************************************************************************/
u32 FindNoZero(u8* Data,u32 DataLen)
{
	u32 i = 0;	//�ۼ���

	if(Data == 0)
		return 0;

	while(Data[i] != 0x00)	//���ݲ�Ϊ0�����ѭ��
	{	
		i++;
		if(i >= DataLen)	//������������˳�ѭ��
			break;
	}	
	return i;
}
