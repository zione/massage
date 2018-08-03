#include  "./sys/sys.h"
#include "fontupd.h"
#include "./W25QXX/w25qxx.h" 
#include "./LCD/lcd.h"
#include "text.h"	
#include "string.h"												    
#include "./usart/usart.h"
//������ʾ ��������	   

//code �ַ�ָ�뿪ʼ
//���ֿ��в��ҳ���ģ
//code �ַ����Ŀ�ʼ��ַ,GBK��
//mat  ���ݴ�ŵ�ַ (size/8+((size%8)?1:0))*(size) bytes��С	
//size:�����С
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���
	{
	    for(i=0;i<csize;i++)*mat++=0x00;//�������
	    return; //��������
	}
	if(ql<0x7f)ql-=0x40;//ע��!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize;	//�õ��ֿ��е��ֽ�ƫ����  		  
	switch(size)
	{
		case 12:
			W25QXX_Read(mat,foffset+ftinfo.f12addr,csize);
			break;
		case 16:
			W25QXX_Read(mat,foffset+ftinfo.f16addr,csize);
			break;
		case 24:
			W25QXX_Read(mat,foffset+ftinfo.f24addr,csize);
			break;
	}     												    
}  
//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//size:�����С
//mode:0,������ʾ,1,������ʾ	   
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	if(size!=12&&size!=16&&size!=24)return;	//��֧�ֵ�size
	Get_HzMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//�õ���������                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y++;
			if((y-y0)==size)	//�������ĸ߶ȴﵽ���õ�����߶ȣ������ٴ�����д��
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
}
void Show_My_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode,u8 pointx,u8 pointy)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	if(size!=12&&size!=16&&size!=24)return;	//��֧�ֵ�size
	Get_HzMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//�õ���������                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y+=pointy;
			if((y-y0)==(size*pointy))	//�������ĸ߶ȴﵽ���õ�����߶ȣ������ٴ�����д��
			{
				y=y0;
				x+=pointx;
				break;
			}
		}  	 
	}  
}
void Show_My_Shape_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode,u8 pointx,u8 pointy,u8 shape)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	if(size!=12&&size!=16&&size!=24)return;	//��֧�ֵ�size
	Get_HzMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//�õ���������                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)//�е���Ҫ��ʾʱ
			{
				if(shape==0)
					LCD_Fast_DrawPoint(x,y,POINT_COLOR);
				if(shape==1)
					LCD_DrawRectangle(x,y,x+5,y+12);
				if(shape==2)
					LCD_Draw_Circle(x,y,3);
			}		
//			else if(mode==0)//�հ�������ʾ�ĵ�
//			{
//				if(shape==0)
//					LCD_Fast_DrawPoint(x,y,BACK_COLOR);
//				if(shape==1)
//					LCD_DrawRectangle(x,y,x+3,y+3);
//				if(shape==2)
//					LCD_Draw_Circle(x,y,2);
//			}
			temp<<=1;
			y+=pointy;
			if((y-y0)==(size*pointy))	//�������ĸ߶ȴﵽ���õ�����߶ȣ������ٴ�����д��
			{
				y=y0;
				x+=pointx;
				break;
			}
		}  	 
	}  
}
//��ָ��λ�ÿ�ʼ��ʾһ���ַ���	    
//֧���Զ�����
//(x,y):��ʼ����
//width,height:����
//str  :�ַ���
//size :�����С
//mode:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ    	   		   
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //�ַ���������  	    				    				  	  
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {      
                if(x>(x0+width-size/2))//����
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//��Ч����д�� 
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ�    
            if(x>(x0+width-size))//����
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//Խ�緵��  						     
	        Show_Font(x,y,str,size,mode); //��ʾ�������,������ʾ 
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}  			 		 
void Show_My_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode,u8 pointx,u8 pointy)
{					
	u16 x0=x;
	u16 y0=y;							  	  
  u8 bHz=0;     //�ַ���������  	    				    				  	  
  while(*str!=0)//����δ����
  { 
    if(!bHz)
    {
	    if(*str>0x80)bHz=1;//���� 
	    else              //�ַ�
	    {      
        if(x>(x0+width-size/2*pointx))//����
				{				   
					y+=size*pointy;
					x=x0;	   
				}							    
		    //if(y>(y0+height-size*pointy))break;//Խ�緵��      
		    if(*str==13)//���з���
		    {         
		     y+=size*pointy;
				 x=x0;
		     str++; 
		    }  
		    else 
				LCD_ShowMyChar(x,y,*str,size,mode,pointx,pointy);//��Ч����д�� 
				str++; 
		    x+=size/2*pointx; //�ַ�,Ϊȫ�ֵ�һ�� 
	     }
     }
		 else//���� 
     {     
       bHz=0;//�к��ֿ�    
       if(x>(x0+width-size*pointx))//����
			 {	    
				y+=size*pointy;
				x=x0;		  
			 }
	     //if(y>(y0+height-size*pointy))break;//Խ�緵��  						     
	     Show_My_Font(x,y,str,size,mode,pointx,pointy); //��ʾ�������,������ʾ 
	     str+=2; 
	     x+=size*pointx;//��һ������ƫ��	    
     }						 
  }   
}  		
void Show_My_Shape_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode,u8 pointx,u8 pointy,u8 shape)
{					
	u16 x0=x;
	u16 y0=y;							  	  
  u8 bHz=0;     //�ַ���������  	    				    				  	  
  while(*str!=0)//����δ����
  { 
    if(!bHz)
    {
	    if(*str>0x80)bHz=1;//���� 
	    else              //�ַ�
	    {      
        if(x>(x0+width-size/2*pointx))//����
				{				   
					y+=size*pointy;
					x=x0;	   
				}							    
		    //if(y>(y0+height-size*pointy))break;//Խ�緵��      
		    if(*str==13)//���з���
		    {         
		     y+=size*pointy;
				 x=x0;
		     str++; 
		    }  
		    else 
				LCD_ShowMyShapeChar(x,y,*str,size,mode,pointx,pointy,shape);//��Ч����д�� 
				str++; 
		    x+=size/2*pointx; //�ַ�,Ϊȫ�ֵ�һ�� 
	     }
     }
		 else//���� 
     {     
       bHz=0;//�к��ֿ�    
       if(x>(x0+width-size*pointx))//����
			 {	    
				y+=size*pointy;
				x=x0;		  
			 }
	     //if(y>(y0+height-size*pointy))break;//Խ�緵��  						     
	     Show_My_Shape_Font(x,y,str,size,mode,pointx,pointy,shape); //��ʾ�������,������ʾ 
	     str+=2; 
	     x+=size*pointx;//��һ������ƫ��	    
     }						 
  }   
}
//��ָ����ȵ��м���ʾ�ַ���
//����ַ����ȳ�����len,����Show_Str��ʾ
//len:ָ��Ҫ��ʾ�Ŀ��			  
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	u16 strlenth=0;
  strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,lcddev.width,lcddev.height,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	  Show_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1);
	}
}  
void Show_My_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len,u8 pointx,u8 pointy)
{
	u16 strlenth=0;
  strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_My_Str(x,y,lcddev.width,lcddev.height,str,size,1,pointx,pointy);
	else
	{
		strlenth=(len-strlenth)/2;
	  Show_My_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1,pointx,pointy);
	}
}  
void Show_My_Shape_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len,u8 pointx,u8 pointy,u8 shape)
{
	u16 strlenth=0;
  strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)
		Show_My_Shape_Str(x,y,lcddev.width,lcddev.height,str,size,1,pointx,pointy,shape);
	else
	{
		strlenth=(len-strlenth)/2;
	  Show_My_Shape_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1,pointx,pointy,shape);
	}
}  

//void my_font_test()
//{
//	u16 myx,myy;
//	POINT_COLOR=BLUE;
//	for(myx=5;myx<10;myx+=1)
//	{
//		for(myy=100;myy<110;myy+=2)
//		{
//			//Show_My_Str_Mid(myx,myy,"������",24,240,4,6);
//			//Show_My_Str(myx,myy,480,120,"������",16,0,6,7);//16������Ŵ�6*7��	48-54 x
//			//Show_My_Str(myx,myy,480,120,"������",24,0,5,5);//24������Ŵ�5*5��	0-5 x
//			//Show_My_Str(myx,myy,480,120,"������",12,0,10,10);//12������Ŵ�10*10��5-10x
//			Show_My_Str(myx,myy,480,120,"������",12,0,10,10);
//		}
//	}
//}
//POINT_COLOR=RED;
//Show_My_Shape_Str(22,50,480,240,"����",16,0,7,14,1);
//delay_ms(1000);LCD_Clear(WHITE);
//Show_My_Shape_Str(25,50,480,240,"����",16,0,7,14,2);




//void testmyshow()
//{
//	u16 myx,myy;
//	
//	POINT_COLOR=BLUE;
//	for(myx=0;myx<5;myx+=1)
//	{
//		for(myy=100;myy<116;myy+=1)
//		{
//			Show_My_Str(myx,myy,480,120,"������",24,0,5,16);
//		}
//	}
//	LCD_Clear(WHITE);
//	POINT_COLOR=RED;
//	for(myx=0;myx<5;myx+=1)
//	{
//		for(myy=100;myy<116;myy+=1)
//		{
//			Show_My_Str(myx,myy,480,120,"������Ʒ",24,0,5,16);
//		}
//	}
//	LCD_Clear(WHITE);
//	POINT_COLOR=RED;
//	Show_My_Shape_Str(22,50,480,240,"����LOVE",16,0,7,14,1);
//}
