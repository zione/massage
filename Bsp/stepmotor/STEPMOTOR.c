
/********************************
1AI+����PE3
1AI-����PE4
1BI+����PE5
1BI-����PE6
2AI+����PB7
2AI-����PB8
2BI+����PB9
2BI-����PE0
3AI+����PC4
3AI-����PC5
3BI+����PB0
3BI-����PB1
4AI+����PA4
4AI-����PA5
4BI+����PA6
4BI-����PA7
**********************************/

#include "STEPMOTOR.h"
#include "bsp_iso_test.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include <string.h>
#include "bsp_usart1.h"


#define uint unsigned int 
#define uchar unsigned char 
	
static void delay(uint16_t t);
static void delayus(uint16_t t);

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

uint i,j,k ;
uint N;//�˶�����
uint P_C;//����ָ��
uint N_M;//ģʽ���ݳ���
//B-,B+,A-,A+ 
uchar single_pos[4]={0x01,0x04,0x02,0x08};  /*��������ת��*/// 0001 0100 0010 1000
uchar single_rev[4]={0x08,0x02,0x04,0x01};  /*�����ķ�ת��*/// 1000 0010 0100 0001
uchar double_pos[4]={0x05,0x06,0x0a,0x09};  /*��������ת��*/// 0001 0100 0010 1000
uchar double_rev[4]={0x09,0x0a,0x06,0x05};  /*��������ת��*/// 0001 0100 0010 1000
uchar eight_pos[8] ={0x01,0x05,0x04,0x06,0x02,0x0a,0x08,0x09};  /*��������ת��*/// 0001 0100 0010 1000
uchar eight_rev[8] ={0x09,0x08,0x0a,0x02,0x06,0x04,0x05,0x01};  /*��������ת��*/// 0001 0100 0010 1000

uchar pc;//��Ƭ���ĵ�λ��
uchar p1,p2,p3,p4;//1-4����ﵱǰ�����״ֵ̬
bool dl,dr;//�����۾�Ƭ���з�����1/3��Ƭ����Ϊ����1Ϊ����0��
uint sl,sr;//�����۴������ܲ���
uint STEP_L,STEP_R;//�����������в���
bool TIMOK;//��ʱ��ɱ�ʶ��1Ϊ��ɣ���ʱ�������ٶȿ���
uint v;//�����ٶȣ���λ��s/���壩������ٶ�Ϊÿ������1�룬ȡֵ��Χ��1-30
bool over_l,over_r;//ִ����ɱ�ʶ��1���
uint x;		//˿�˵ľ�������λ��,���������Ϊ���0����Զ��Ϊ�յ�



void MOTOR_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;
		/*����PWR������ʱ��*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE, ENABLE); 

//GPIOA		
		/*ѡ��Ҫ���Ƶ�����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIOA*/			
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
//GPIOB
		/*ѡ��Ҫ���Ƶ�����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;

		/*���ÿ⺯������ʼ��GPIOB*/			
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
//GPIOC
		/*ѡ��Ҫ���Ƶ�����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;

		/*���ÿ⺯������ʼ��GPIOC*/			
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
//GPIOE
		/*ѡ��Ҫ���Ƶ�����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;

		/*���ÿ⺯������ʼ��GPIOE*/			
		GPIO_Init(GPIOE, &GPIO_InitStructure);	
				
}

 
//����p1,p2,p3,p4ֵ��ȡ״̬��λ��i������ȷ����һ������ֵ
//���룺px--��ǰ״ֵ̬��dx--����ת
uchar getbypx(uchar px,bool dx)
{
	if(dx)//��ת
	{
		for(i=0;i<4;i++)
		{
			if(double_pos[i]==px){
				return i;
			}
		}
	}else//��ת
	{
		for(i=0;i<4;i++)
		{
			if(double_rev[i]==px){
				return i;
			}
		}
	}
	return 0;
}

void STEPMOTOR_Init()
{
	vu16  capture = 0; 			/* ��ǰ�������ֵ�ֲ����� */
	extern vu16 CCR1_Val;

	MOTOR_GPIO_Config();
	p1=0x05;		//״̬��ֵ
	p2=0x05;		//״̬��ֵ
	p3=0x05;		//״̬��ֵ
	p4=0x05;		//״̬��ֵ
	dl=dr=1;		//�����ֵ
	STEP_L=STEP_R==0;	//�����в�����ֵ
	sl=sr=0;			//�����ܲ�����ֵ
	N_M=0;				//ģʽ��������
	P_C=0;				//�˶�ģʽ���ݱ�ָ�룬���ڻ�ȡ��ǰҪִ�е�����
	N=0;					//�˶���������ֵ0
	v=1;//���������ٶ�Ĭ��ֵΪ1s/��
	
	OPoint();	//У׼��Ƭλ��
	LastPoint();//���ϴιػ�ʱ���е������
//	N_M=getN_M();//��ȡģʽ���ݳ���
//
		GetConfigL();//��ȡҪִ�е�����
		CalConfigL();//�������㣺�������ò�����������з���d,����s,�ٶ�v
		GetConfigR();//��ȡҪִ�е�����
		CalConfigR();//�������㣺�������ò�����������з���d,����s,�ٶ�v
						
		sl=20;
		over_l=0;
		sr=20;
		over_r=0;
//�ٶȽ��Ķ�ʱ����ʼ��	
	CCR1_Val=(uint16_t)(1000*v);
	
	TIMOK=0;	//��ʱ����ɱ�ʶ��ֵ	
	/* ������ǰ����ֵ */
	capture = TIM_GetCapture1(TIM2);
	/* ���ݵ�ǰ����ֵ�����������Ĵ��� */
	TIM_SetCompare1(TIM2, capture + CCR1_Val);
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
}
//��Ƭ��ԭ�㣬У׼
void OPoint()
{
	uint8_t i;
	
//	MOTOR_GPIO_Config();

	LED(ON);
	i=0;
	while(GetKey1L() && i<255)
	{
		p1=double_rev[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
		Pin1Set(p1);				//��������ţ��������
		delay(30);
		PinRSTL();
		delay(150);
		i++;								//ָ����һ������					
	}	
	delay(1000);
	i=0;
	while(i<128)
	{
		p1=double_pos[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
		Pin1Set(p1);				//��������ţ��������
		delay(30);
		PinRSTL();
		delay(150);
		i++;								//ָ����һ������					
	}	
	LED(OFF);
	delay(1000);
	LED(ON);
	i=0;
	
	while(GetKey2L() && i<255)
	{
		p2=double_rev[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
		Pin2Set(p2);				//��������ţ��������
		delay(30);
		PinRSTL();
		delay(150);
		i++;								//ָ����һ������					
	}	
	delay(1000);
	i=0;
	while(i<131)
	{
		p2=double_pos[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
		Pin2Set(p2);				//��������ţ��������
		delay(30);
		PinRSTL();
		delay(150);
		i++;								//ָ����һ������					
	}	

	LED(OFF);
	delay(1000);
	LED(ON);

	i=0;
	while(GetKey1R() && i<255)
	{
		p3=double_rev[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
		Pin3Set(p3);				//��������ţ��������
		delay(30);
		PinRSTR();					//���ŵ�ƽ���ߣ����͹���
		delay(150);
		i++;								//ָ����һ������					
	}			

	delay(1000);
	i=0;
	while(i<128)
	{
		p3=double_pos[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
		Pin3Set(p3);				//��������ţ��������
		delay(30);
		PinRSTR();
		delay(150);
		i++;								//ָ����һ������					
	}	
	
	LED(OFF);
	delay(1000);
	LED(ON);	
	i=0;
	while(GetKey2R() && i<255)
	{
		p4=double_rev[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
		Pin4Set(p4);				//��������ţ��������
		delay(30);
		PinRSTR();
		delay(150);
		i++;								//ָ����һ������					
	}		
	
	delay(1000);
	i=0;
	while(i<131)
	{
		p4=double_pos[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
		Pin4Set(p4);				//��������ţ��������
		delay(30);
		PinRSTR();
		delay(150);
		i++;								//ָ����һ������					
	}		
 		
	LED(OFF);

	x=0;
}
//��Ƭ��ͫ�������
void EyePoidt()
{
	
}

//��Ƭ���ϴιػ�ʱ���е�����㣬���ޣ���ͫ���
void LastPoint()
{
	
}


//��ȡ������ǰ�����򣬲������ٶ�
void GetConfigL()
{
	STEP_L=0;
}
//��ȡ������ǰ�����򣬲������ٶ�
void GetConfigR()
{
	STEP_R=0;
}

//�������㣺�������ò�����������з���d,����s,�ٶ�v
void CalConfigL()
{
	
	
}

//�������㣺�������ò�����������з���d,����s,�ٶ�v
void CalConfigR()
{
	
	
}

//��Flash�˶�ģʽ���ݱ��л�ȡ���д���
uint8_t getN(uint8_t x)
{
	uint8_t n;
	n=x+1;
	return n;
}

void STEPMOTOR_Test()
 { 
//		STEPMOTOR_Init();
		N_M=3;
	 	if(P_C<N_M){					
			 
			if(N>0){
				if(over_l && over_r){	////ִ����ɱ�־λ��ֵΪ1����ʾ��ɣ�������������
					GetConfigL();//��ȡҪִ�е�����
					CalConfigL();//�������㣺�������ò�����������з���d,����s,�ٶ�v
					GetConfigR();//��ȡҪִ�е�����
					CalConfigR();//�������㣺�������ò�����������з���d,����s,�ٶ�v
									
					sl=20;
					over_l=0;
					sr=20;
					over_r=0;
					
					N--;
				}
				
				RunN(); 					
			
			}else{		//��ִ�е�����β
				N=getN(P_C);//��ȡ��ǰ�˶������е��˶����������Ѿ�ִ�е�����β��N����0
				N=2*N+1;	//���˶�����ֵ����2����ʾһ�����ڣ��˶��������أ�Ϊ1��
				printf("\r\nP_C=%d\r\n",P_C);
				P_C++;
				dl=1;
				dr=1;
			}						
		}else{
				
			P_C=0;	//����ָ��ָ��ͷ
			
			while(0)
			{
				delay(8000);
				LED_TOGGLE;
				delay(8000);
				printf("\r\n%s","Test OVER\r\n");
			};
		}
}

/*****************************�����������*************************************/  
//������ǰ��s��
//���أ���ǰ�������p
void RunN()
{
		uchar i;
		uint16_t j;
		uint16_t n;
	
		if(TIMOK)							//��ʱ��ɺ������һ��	     
		{  
			TIMOK=0;						//����
//									
			if(dl){
				i=getbypx(p1,dl);			//��ȡ��ǰλ�ö�Ӧ�������±�i
				i++;								//ָ����һ������
				p1=double_pos[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
				
				i=getbypx(p2,!dl);	//��ȡ��ǰλ�ö�Ӧ�������±�i
				i++;								//ָ����һ������
				p2=double_rev[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ		
														
			}else if(!dl){
				i=getbypx(p1,dl);			//��ȡ��ǰλ�ö�Ӧ�������±�i
				i++;								//ָ����һ������
				p1=double_rev[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
				
				i=getbypx(p2,!dl);	//��ȡ��ǰλ�ö�Ӧ�������±�i
				i++;								//ָ����һ������
				p2=double_pos[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
						
			}
			
			STEP_L++;
			if(STEP_L<sl){	
				over_l=0;//��ǰ����δ���				
			}else{
				over_l=1;//��ǰ���������
				dl=!dl;
			}
					
//			
			if(dr){
				i=getbypx(p3,dr);			//��ȡ��ǰλ�ö�Ӧ�������±�i
				i++;								//ָ����һ������
				p3=double_pos[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
				
				i=getbypx(p4,!dr);	//��ȡ��ǰλ�ö�Ӧ�������±�i
				i++;								//ָ����һ������
				p4=double_rev[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
									
			}else if(!dr){
				i=getbypx(p3,dr);			//��ȡ��ǰλ�ö�Ӧ�������±�i
				i++;								//ָ����һ������
				p3=double_rev[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ			
				
				i=getbypx(p4,!dr);	//��ȡ��ǰλ�ö�Ӧ�������±�i
				i++;								//ָ����һ������
				p4=double_pos[i%4];	//�õ�����ֵ����ֵ��������ţ�������pֵ							
				
			}
			
			STEP_R++;
				
			if(STEP_R<sr){				
				over_r=0;//��ǰ����δ���				
			}else{
				over_r=1;//��ǰ���������
				dr=!dr;
			}
//���µ��λ��
				Pin1Set(p1);					//��������ţ��������	
				delay(30);						//�ʵ���ʱ����֤ת��ת��ʱ�䣬��ʱ���̻ᶪ��				
				PinRSTL();
				Pin2Set(p2);					//��������ţ��������
				delay(30);						//�ʵ���ʱ����֤ת��ת��ʱ�䣬��ʱ���̻ᶪ��				
				PinRSTL();
				Pin3Set(p3);					//��������ţ��������
				delay(30);						//�ʵ���ʱ����֤ת��ת��ʱ�䣬��ʱ���̻ᶪ��				
				PinRSTR();
				Pin4Set(p4);					//��������ţ��������			
				delay(30);						//�ʵ���ʱ����֤ת��ת��ʱ�䣬��ʱ���̻ᶪ��				
				PinRSTR();
				

//			
//			n=4;
//			j=1;
//			do{
//				Pin1Set(p1);					//��������ţ��������			
//				Pin2Set(p2);					//��������ţ��������
//				delayus(j);					//�ʵ���ʱ����֤ת��ת��ʱ�䣬��ʱ���̻ᶪ��		
//				
//				PinRSTL();						
//				delayus(200-j);
//	
//				j=j+40;
//		
//			}while(n--);
//						
//			n=4;
//			j=1;
//			do{
//	
//				Pin3Set(p3);					//��������ţ��������			
//				Pin4Set(p4);					//��������ţ��������
//				delayus(j);					//�ʵ���ʱ����֤ת��ת��ʱ�䣬��ʱ���̻ᶪ��				
//				
//				PinRSTR();				
//				delayus(200-j);
//	
//				j=j+40;
//		
//			}while(n--);
//				
		}
}

//���Ĳ������������
void PinSet(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
		
			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
		
			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
		
			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
		
			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		default:
			PinRST();//
			break;
	}
}
			
//���Ĳ������������
void Pin1Set(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+		
			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+	
			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-		
			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+		
			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			break;
		default:
//			PinRST();//
			break;
	}
}

//���Ĳ������������
void Pin2Set(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			break;
		default:
//			PinRST();//
			break;
	}
}


//���Ĳ������������
void Pin3Set(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			break;
		default:
//			PinRST();//
			break;
	}
}


//���Ĳ������������
void Pin4Set(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		default:
//			PinRST();//
			break;
	}
}

////���ŵ�ƽ���ߣ����͹���
//void PinRST()
//{
//		/* �ر�����*/
//		GPIO_SetBits(GPIOA, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
//		GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
//		GPIO_SetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_5);
//		GPIO_SetBits(GPIOE, GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);	
//}

//���ŵ�ƽ���ߣ����͹���
void PinRST(void)
{
		/* �ر�����*/
		GPIO_ResetBits(GPIOA, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
		GPIO_ResetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
		GPIO_ResetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_5);
		GPIO_ResetBits(GPIOE, GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);	
}

//���ŵ�ƽ���ߣ����͹���
void PinRSTL(void)
{
		/* �ر�����*/
		GPIO_ResetBits(GPIOE, GPIO_Pin_3);//1A+		
		GPIO_ResetBits(GPIOE, GPIO_Pin_5);//1B+
		GPIO_ResetBits(GPIOE, GPIO_Pin_4);//1A-		
		GPIO_ResetBits(GPIOE, GPIO_Pin_6);//1B-	
	
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);//2A+
		GPIO_ResetBits(GPIOB, GPIO_Pin_9);//2B+
		GPIO_ResetBits(GPIOB, GPIO_Pin_8);//2A-
		GPIO_ResetBits(GPIOE, GPIO_Pin_0);//2B-
		
}

//���ŵ�ƽ���ߣ����͹���
void PinRSTR(void)
{
		/* �ر�����*/
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);//3A+
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);//3B+
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);//3A-
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);//3B-

		GPIO_ResetBits(GPIOA, GPIO_Pin_4);//4A+
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);//4B+
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);//4A-
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);//4B-
}

void TEST_M()
{	
	uint i,j;
	uchar xxx;
	MOTOR_GPIO_Config();
	PinRST();
	
		for(i=0;i<4;i++)//ǰ��160�����壬��10Ȧ
		{
			for(j=0;j<4;j++)
			{

				xxx=double_pos[j];
				PinSet(xxx);
				delay(50);
				PinRST();
				delay(100);
			}
			
		}
		for(i=0;i<4;i++)//����160�����壬��10Ȧ
		{
			for(j=0;j<4;j++)
			{

				xxx=double_rev[j];
				PinSet(xxx);
				delay(50);
				PinRST();
				delay(100);
			}
			
		}
}

static void delay(uint16_t t)
{
	uint16_t i;
	while(t--)
	{
		for(i=0;i<1000;i++);
	}
}

static void delayus(uint16_t t)
{
	uint16_t i;
	while(t--)
	{
		for(i=0;i<200;i++);
	}
}
