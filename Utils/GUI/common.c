#include "./common.h"  

//ģʽѡ���б�Ĵ�������
u8*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"ģʽѡ��","ģʽ�x��","Mode Select",
}; 
//��ʾ��Ϣ�Ĵ�������
u8*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"��ʾ��Ϣ","��ʾ��Ϣ","Remind",	 
};
//���ѱ���Ĵ�������
u8*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"�Ƿ񱣴�༭����ļ�?","�Ƿ񱣴澎݋����ļ�?","Do you want to save?",
};

//�����ļ�ʧ��,��ʾ�Ƿ����SD��? ,200�Ŀ��
u8*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM]=
{							 
"�����ļ�ʧ��,����!","�����ļ�ʧ��,Ո�z��!","Creat File Failed,Please Check!",
}; 
//�������ֱ�
u8*const APP_DISK_NAME_TBL[3][GUI_LANGUAGE_NUM]=
{
	{"SD��","SD��","SD Card",},
 	{"FLASH����","FLASH�űP","FLASH Disk",},
 	{"U��","U�P","U Disk",},
};  							 


u8*const APP_OK_PIC="1:/SYSTEM/APP/COMMON/ok.bmp";				//ȷ��ͼ��
u8*const APP_CANCEL_PIC="1:/SYSTEM/APP/COMMON/cancel.bmp";		//ȡ��ͼ��
u8*const APP_UNSELECT_PIC="1:/SYSTEM/APP/COMMON/unselect.bmp";	//δѡ��ͼ��
u8*const APP_SELECT_PIC="1:/SYSTEM/APP/COMMON/select.bmp";		//ѡ��ͼ��

//PC2LCD2002����ȡģ����:����ʽ,˳��(��λ��ǰ),����.C51��ʽ.																		    
//��������:
//���������:ASCII��+��(' '+95)
//��ͨ����:ASCII��
u8*const APP_ASCII_S6030="1:/SYSTEM/APP/COMMON/fonts60.fon";	//���������60*30����������·�� 
u8*const APP_ASCII_5427="1:/SYSTEM/APP/COMMON/font54.fon";		//��ͨ����54*27����������·�� 
u8*const APP_ASCII_3618="1:/SYSTEM/APP/COMMON/font36.fon";		//��ͨ����36*18����������·��
u8*const APP_ASCII_2814="1:/SYSTEM/APP/COMMON/font28.fon";		//��ͨ����28*14����������·�� 

u8* asc2_s6030=0;	//���������60*30���������
u8* asc2_5427=0;	//��ͨ����54*27���������
u8* asc2_3618=0;	//��ͨ����36*18���������
u8* asc2_2814=0;	//��ͨ����28*14���������

//ALIENTEK logo ͼ��(18*24��С)
//PCtoLCD2002ȡģ��ʽ:����,����ʽ,˳��
const u8 APP_ALIENTEK_ICO1824[]=
{
0x03,0xF0,0x00,0x0F,0xF8,0x00,0x1F,0xFC,0x00,0x3F,0xFE,0x00,0x7F,0xFF,0x00,0x7F,
0xFF,0x00,0xFF,0xFF,0x80,0xFF,0xFF,0x80,0xFF,0xFF,0x80,0xFF,0xFF,0x80,0xFF,0xFF,
0x80,0xFF,0xFF,0x80,0xCF,0xF9,0x80,0xC7,0xF1,0x80,0x43,0xE1,0x00,0x61,0xC3,0x00,
0x31,0xC6,0x00,0x39,0xCE,0x00,0x1F,0xFC,0x00,0x0F,0xF8,0x00,0x07,0xF0,0x00,0x03,
0xE0,0x00,0x01,0xC0,0x00,0x00,0x80,0x00
};
//ALIENTEK logo ͼ��(24*32��С)
//PCtoLCD2002ȡģ��ʽ:����,����ʽ,˳��
const u8 APP_ALIENTEK_ICO2432[]=
{
0x00,0x7F,0x00,0x01,0xFF,0xC0,0x03,0xFF,0xE0,0x07,0xFF,0xF0,0x0F,0xFF,0xF8,0x1F,
0xFF,0xFC,0x1F,0xFF,0xFC,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x7F,0xFF,0xFF,0x7F,0xFF,
0xFF,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,
0x73,0xFF,0xE7,0x70,0xFF,0x87,0x30,0xFF,0x86,0x30,0x7F,0x06,0x38,0x3E,0x0E,0x1C,
0x3E,0x1C,0x1E,0x1C,0x3C,0x0F,0x1C,0x78,0x07,0xFF,0xF0,0x07,0xFF,0xF0,0x03,0xFF,
0xE0,0x01,0xFF,0xC0,0x00,0xFF,0x80,0x00,0x7F,0x00,0x00,0x3E,0x00,0x00,0x1C,0x00,
};
//ALIENTEK logo ͼ��(36*48��С)
//PCtoLCD2002ȡģ��ʽ:����,����ʽ,˳��
const u8 APP_ALIENTEK_ICO3648[]=
{
0x00,0x01,0xFC,0x00,0x00,0x00,0x0F,0xFF,0x80,0x00,0x00,0x1F,0xFF,0xC0,0x00,0x00,
0x7F,0xFF,0xF0,0x00,0x00,0xFF,0xFF,0xF8,0x00,0x01,0xFF,0xFF,0xFC,0x00,0x03,0xFF,
0xFF,0xFE,0x00,0x07,0xFF,0xFF,0xFF,0x00,0x07,0xFF,0xFF,0xFF,0x80,0x0F,0xFF,0xFF,
0xFF,0x80,0x0F,0xFF,0xFF,0xFF,0xC0,0x1F,0xFF,0xFF,0xFF,0xC0,0x1F,0xFF,0xFF,0xFF,
0xC0,0x1F,0xFF,0xFF,0xFF,0xC0,0x1F,0xFF,0xFF,0xFF,0xE0,0x1F,0xFF,0xFF,0xFF,0xE0,
0x3F,0xFF,0xFF,0xFF,0xE0,0x3F,0xFF,0xFF,0xFF,0xE0,0x3F,0xFF,0xFF,0xFF,0xE0,0x3F,
0xFF,0xFF,0xFF,0xE0,0x3F,0xFF,0xFF,0xFF,0xE0,0x3F,0xFF,0xFF,0xFF,0xE0,0x3F,0xFF,
0xFF,0xFF,0xE0,0x3F,0xFF,0xFF,0xFF,0xE0,0x3C,0xFF,0xFF,0xFD,0xE0,0x38,0x3F,0xFF,
0xF0,0xE0,0x18,0x1F,0xFF,0xC0,0xE0,0x1C,0x0F,0xFF,0x80,0xC0,0x1C,0x07,0xFF,0x01,
0xC0,0x0C,0x03,0xFF,0x01,0xC0,0x0E,0x03,0xFE,0x03,0x80,0x07,0x01,0xFC,0x07,0x80,
0x07,0x81,0xFC,0x07,0x00,0x03,0xC0,0xFC,0x0F,0x00,0x03,0xE0,0xF8,0x3E,0x00,0x01,
0xF0,0xF8,0x7E,0x00,0x01,0xFE,0xFD,0xFC,0x00,0x00,0xFF,0xFF,0xFC,0x00,0x00,0x7F,
0xFF,0xF8,0x00,0x00,0x3F,0xFF,0xF0,0x00,0x00,0x3F,0xFF,0xE0,0x00,0x00,0x1F,0xFF,
0xC0,0x00,0x00,0x0F,0xFF,0x80,0x00,0x00,0x07,0xFF,0x00,0x00,0x00,0x03,0xFE,0x00,
0x00,0x00,0x01,0xFC,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
////////////////////////////////α����������취////////////////////////////////
u32 random_seed=1;
void app_srand(u32 seed)
{
	random_seed=seed;
}
//��ȡα�����
//���Բ���0~RANDOM_MAX-1�������
//seed:����
//max:���ֵ	  		  
//����ֵ:0~(max-1)�е�һ��ֵ 		
u32 app_get_rand(u32 max)
{			    	    
	random_seed=random_seed*22695477+1;
	return (random_seed)%max; 
}   
//��ȡ����ɫ
//x,y,width,height:����ɫ��ȡ��Χ
//ctbl:����ɫ���ָ��
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl)
{
	u32	x0,y0,ccnt;
	ccnt=0;
	for(y0=y;y0<y+height;y0++)
	{
		for(x0=x;x0<x+width;x0++)
		{
			ctbl[ccnt]=gui_phy.read_point(x0,y0);//��ȡ��ɫ
			ccnt++;
		}
	}
}  
//�ָ�����ɫ
//x,y,width,height:����ɫ��ԭ��Χ
//ctbl:����ɫ���ָ��
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl)
{
	u32 x0,y0,ccnt;
	ccnt=0;
	for(y0=y;y0<y+height;y0++)
	{
		for(x0=x;x0<x+width;x0++)
		{
			gui_phy.draw_point(x0,y0,ctbl[ccnt]);//��ȡ��ɫ
			ccnt++;
		}
	}
}
//2ɫ��
//x,y,width,height:���꼰�ߴ�.
//mode:	���÷ֽ���
//	    [3]:�ұ߷ֽ���
//		[2]:��߷ֽ���
//		[1]:�±߷ֽ���
//		[0]:�ϱ߷ֽ���
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode)
{
 	u16 halfheight=height/2;
 	gui_fill_rectangle(x,y,width,halfheight,LIGHTBLUE);  			//���ײ���ɫ(ǳ��ɫ)	
 	gui_fill_rectangle(x,y+halfheight,width,halfheight,GRAYBLUE); 	//���ײ���ɫ(����ɫ)
	if(mode&0x01)gui_draw_hline(x,y,width,DARKBLUE);
	if(mode&0x02)gui_draw_hline(x,y+height-1,width,DARKBLUE);
	if(mode&0x04)gui_draw_vline(x,y,height,DARKBLUE);
	if(mode&0x08)gui_draw_vline(x+width-1,y,width,DARKBLUE);
} 
//�õ����ֵ�λ��
//num:����
//dir:0,�Ӹ�λ����λ.1,�ӵ�λ����λ.
//����ֵ:���ֵ�λ��.(���Ϊ10λ)
//ע:0,��λ��Ϊ0λ.
u8 app_get_numlen(long long num,u8 dir)
{
#define MAX_NUM_LENTH		10		//�������ֳ���
	u8 i=0,j;
	u8 temp=0;  
	if(dir==0)//�Ӹߵ���
	{
		i=MAX_NUM_LENTH-1;
		while(1)
		{
			temp=(num/gui_pow(10,i))%10;
			if(temp||i==0)break;
			i--;
		}
	}else	//�ӵ͵���
	{
		j=0;
 		while(1)
		{
			if(num%10)
			{
				i=app_get_numlen(num,0);    
				return i;
			}
			if(j==(MAX_NUM_LENTH-1))break;	 
			num/=10;
			j++;
		}
	}
	if(i)return i+1;
	else if(temp)return 1;
	else return 0;	
}


//��ʾ��ɫͼ��
//x,y,width,height:���꼰�ߴ�.
//icosbase:����λ��
//color:�������ɫ.
//bkcolor:����ɫ
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor)
{
	u16 rsize;
	u8 i,j;
	u8 temp;
	u8 t=0;
	u16 x0=x;//����x��λ��
	rsize=width/8+((width%8)?1:0);//ÿ�е��ֽ���
 	for(i=0;i<rsize*height;i++)
	{
		temp=icosbase[i];
		for(j=0;j<8;j++)
		{
	        if(temp&0x80)gui_phy.draw_point(x,y,color);  
			else gui_phy.draw_point(x,y,bkcolor);  	 
			temp<<=1;
			x++;
			t++;			//��ȼ�����
			if(t==width)
			{
				t=0;
				x=x0;
				y++;  
				break;
			}
		}
	}				
} 
//��ʾһ��������(֧�ָ���)
//ע��:����������Ǵ��ҵ�����ʾ��.
//x,y:��ʼ��ʾ������
//num:����	   
//flen:С����λ��.0,û��С������;1,��һλС��;2,����λС��;�����Դ�����.
//clen:Ҫ�����λ��(���ǰһ�εĺۼ�)
//font:����
//color:������ɫ.
//bkcolor:����ɫ
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor)
{			   
	u8 offpos=1;
	u8 ilen=0;						//�������ֳ��Ⱥ�С�����ֵĳ���
	u8 negative=0;					//�������
	u16 maxlen=(u16)clen*(font/2);	//����ĳ���
	gui_fill_rectangle(x-maxlen,y,maxlen,font,bkcolor);//���֮ǰ�ĺۼ�
	if(num<0) 
	{
		num=-num;
  		negative=1;
	}
	ilen=app_get_numlen(num,0);	//�õ���λ��
	gui_phy.back_color=bkcolor;
	if(flen)
	{
 		gui_show_num(x-(font/2)*flen,y,flen,color,font,num,0X80);//��ʾС������
		gui_show_ptchar(x-(font/2)*(flen+1),y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,font,'.',0);//��ʾС����
		offpos=2+flen;
	} 
	if(ilen<=flen)ilen=1;//����Ҫ��һλ��������
 	else 
	{
		if(flen)offpos=ilen+1;
		else offpos=ilen;
		ilen=ilen-flen;	//�õ��������ֵĳ���.
	}
	num=num/gui_pow(10,flen);//�õ���������
	gui_show_num(x-(font/2)*offpos,y,ilen,color,font,num,0X80);	//���0��ʾ 
	if(negative)gui_show_ptchar(x-(font/2)*(offpos+1),y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,font,'-',0);//��ʾ����		 
} 			  
//�ļ����������ʾ
//topname:�����ʱ��Ҫ��ʾ������	 
//mode:
//[0]:0,����ʾ�Ϸ�ɫ��;1,��ʾ�Ϸ�ɫ��
//[1]:0,����ʾ�·�ɫ��;1,��ʾ�·�ɫ��
//[2]:0,����ʾ����;1,��ʾ����
//[3~7]:����
//����ֵ:��	 						  
void app_filebrower(u8 *topname,u8 mode)
{		
  	if(mode&0X01)app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);								//�·ֽ���
	if(mode&0X02)app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);//�Ϸֽ���
	if(mode&0X04)gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,topname);	  
} 
//��һ�������м���ʾ����
//x,y,width,height:����
//num:Ҫ��ʾ������
//len:λ��
//size:����ߴ�
//ptcolor,bkcolor:������ɫ�Լ�����ɫ   
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor)
{
	u16 numlen;
	u8 xoff,yoff;
	numlen=(size/2)*len;//���ֳ���
	if(numlen>width||size>height)return;
	xoff=(width-numlen)/2;
	yoff=(height-size)/2;
	POINT_COLOR=ptcolor;
	BACK_COLOR=bkcolor;
	LCD_ShowxNum(x+xoff,y+yoff,num,len,size,0X80);//��ʾ�������
}
//��һ��ƽ�����ɵĲ�ɫ��(�����)
//���м�Ϊ���,����չ��
//x,y,width,height:����������ߴ�
//sergb,mrgb:��ֹ��ɫ���м���ɫ
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb)
{	  
	gui_draw_smooth_rectangle(x,y,width/2,height,sergb,mrgb);	   		//ǰ��ν���
	gui_draw_smooth_rectangle(x+width/2,y,width/2,height,mrgb,sergb);   //���ν���
}      

//�жϴ�������ǰֵ�ǲ�����ĳ��������
//tp:������
//x,y,width,height:����ͳߴ� 
//����ֵ:0,����������;1,��������.
u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height)
{						 	 
	if(tp->x[0]<=(x+width)&&tp->x[0]>=x&&tp->y[0]<=(y+height)&&tp->y[0]>=y)return 1;
	else return 0;							 	
}

//�õ�STM32�����к�
//sn0,sn1,sn2:3���������к�
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2)
{
	*sn0=*(vu32*)(0x1FFFF7E8);
	*sn1=*(vu32*)(0x1FFFF7EC);
	*sn2=*(vu32*)(0x1FFFF7F0);     
}
//��ӡSN
void app_usmart_getsn(void)
{
	u32 sn0,sn1,sn2;
	app_getstm32_sn(&sn0,&sn1,&sn2);
	printf("\r\nSerial Number:%X%X%X\r\n",sn0,sn1,sn2);
}
