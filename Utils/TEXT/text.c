#include  "./sys/sys.h"
#include "fontupd.h"
#include "./W25QXX/w25qxx.h" 
#include "./LCD/lcd.h"
#include "text.h"	
#include "string.h"												    
#include "./usart/usart.h"
//汉字显示 驱动代码	   

//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK码
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体大小
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{
	    for(i=0;i<csize;i++)*mat++=0x00;//填充满格
	    return; //结束访问
	}
	if(ql<0x7f)ql-=0x40;//注意!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize;	//得到字库中的字节偏移量  		  
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
//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//size:字体大小
//mode:0,正常显示,1,叠加显示	   
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
	Get_HzMat(font,dzk,size);	//得到相应大小的点阵数据 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y++;
			if((y-y0)==size)	//如果字体的高度达到设置的字体高度，换列再次竖着写点
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
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
	Get_HzMat(font,dzk,size);	//得到相应大小的点阵数据 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y+=pointy;
			if((y-y0)==(size*pointy))	//如果字体的高度达到设置的字体高度，换列再次竖着写点
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
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
	Get_HzMat(font,dzk,size);	//得到相应大小的点阵数据 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)//有点需要显示时
			{
				if(shape==0)
					LCD_Fast_DrawPoint(x,y,POINT_COLOR);
				if(shape==1)
					LCD_DrawRectangle(x,y,x+5,y+12);
				if(shape==2)
					LCD_Draw_Circle(x,y,3);
			}		
//			else if(mode==0)//空白无需显示的点
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
			if((y-y0)==(size*pointy))	//如果字体的高度达到设置的字体高度，换列再次竖着写点
			{
				y=y0;
				x+=pointx;
				break;
			}
		}  	 
	}  
}
//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式    	   		   
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //字符或者中文  	    				    				  	  
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//中文 
	        else              //字符
	        {      
                if(x>(x0+width-size/2))//换行
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//越界返回      
		        if(*str==13)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//有效部分写入 
				str++; 
		        x+=size/2; //字符,为全字的一半 
	        }
        }else//中文 
        {     
            bHz=0;//有汉字库    
            if(x>(x0+width-size))//换行
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//越界返回  						     
	        Show_Font(x,y,str,size,mode); //显示这个汉字,空心显示 
	        str+=2; 
	        x+=size;//下一个汉字偏移	    
        }						 
    }   
}  			 		 
void Show_My_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode,u8 pointx,u8 pointy)
{					
	u16 x0=x;
	u16 y0=y;							  	  
  u8 bHz=0;     //字符或者中文  	    				    				  	  
  while(*str!=0)//数据未结束
  { 
    if(!bHz)
    {
	    if(*str>0x80)bHz=1;//中文 
	    else              //字符
	    {      
        if(x>(x0+width-size/2*pointx))//换行
				{				   
					y+=size*pointy;
					x=x0;	   
				}							    
		    //if(y>(y0+height-size*pointy))break;//越界返回      
		    if(*str==13)//换行符号
		    {         
		     y+=size*pointy;
				 x=x0;
		     str++; 
		    }  
		    else 
				LCD_ShowMyChar(x,y,*str,size,mode,pointx,pointy);//有效部分写入 
				str++; 
		    x+=size/2*pointx; //字符,为全字的一半 
	     }
     }
		 else//中文 
     {     
       bHz=0;//有汉字库    
       if(x>(x0+width-size*pointx))//换行
			 {	    
				y+=size*pointy;
				x=x0;		  
			 }
	     //if(y>(y0+height-size*pointy))break;//越界返回  						     
	     Show_My_Font(x,y,str,size,mode,pointx,pointy); //显示这个汉字,空心显示 
	     str+=2; 
	     x+=size*pointx;//下一个汉字偏移	    
     }						 
  }   
}  		
void Show_My_Shape_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode,u8 pointx,u8 pointy,u8 shape)
{					
	u16 x0=x;
	u16 y0=y;							  	  
  u8 bHz=0;     //字符或者中文  	    				    				  	  
  while(*str!=0)//数据未结束
  { 
    if(!bHz)
    {
	    if(*str>0x80)bHz=1;//中文 
	    else              //字符
	    {      
        if(x>(x0+width-size/2*pointx))//换行
				{				   
					y+=size*pointy;
					x=x0;	   
				}							    
		    //if(y>(y0+height-size*pointy))break;//越界返回      
		    if(*str==13)//换行符号
		    {         
		     y+=size*pointy;
				 x=x0;
		     str++; 
		    }  
		    else 
				LCD_ShowMyShapeChar(x,y,*str,size,mode,pointx,pointy,shape);//有效部分写入 
				str++; 
		    x+=size/2*pointx; //字符,为全字的一半 
	     }
     }
		 else//中文 
     {     
       bHz=0;//有汉字库    
       if(x>(x0+width-size*pointx))//换行
			 {	    
				y+=size*pointy;
				x=x0;		  
			 }
	     //if(y>(y0+height-size*pointy))break;//越界返回  						     
	     Show_My_Shape_Font(x,y,str,size,mode,pointx,pointy,shape); //显示这个汉字,空心显示 
	     str+=2; 
	     x+=size*pointx;//下一个汉字偏移	    
     }						 
  }   
}
//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度			  
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
//			//Show_My_Str_Mid(myx,myy,"旭东智能",24,240,4,6);
//			//Show_My_Str(myx,myy,480,120,"旭东智能",16,0,6,7);//16号字体放大6*7倍	48-54 x
//			//Show_My_Str(myx,myy,480,120,"旭东智能",24,0,5,5);//24号字体放大5*5倍	0-5 x
//			//Show_My_Str(myx,myy,480,120,"旭东智能",12,0,10,10);//12号字体放大10*10倍5-10x
//			Show_My_Str(myx,myy,480,120,"旭东智能",12,0,10,10);
//		}
//	}
//}
//POINT_COLOR=RED;
//Show_My_Shape_Str(22,50,480,240,"智能",16,0,7,14,1);
//delay_ms(1000);LCD_Clear(WHITE);
//Show_My_Shape_Str(25,50,480,240,"智能",16,0,7,14,2);




//void testmyshow()
//{
//	u16 myx,myy;
//	
//	POINT_COLOR=BLUE;
//	for(myx=0;myx<5;myx+=1)
//	{
//		for(myy=100;myy<116;myy+=1)
//		{
//			Show_My_Str(myx,myy,480,120,"旭东智能",24,0,5,16);
//		}
//	}
//	LCD_Clear(WHITE);
//	POINT_COLOR=RED;
//	for(myx=0;myx<5;myx+=1)
//	{
//		for(myy=100;myy<116;myy+=1)
//		{
//			Show_My_Str(myx,myy,480,120,"荣誉出品",24,0,5,16);
//		}
//	}
//	LCD_Clear(WHITE);
//	POINT_COLOR=RED;
//	Show_My_Shape_Str(22,50,480,240,"智能LOVE",16,0,7,14,1);
//}
