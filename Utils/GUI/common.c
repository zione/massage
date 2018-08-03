#include "./common.h"  

//模式选择列表的窗体名字
u8*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"模式选择","模式選擇","Mode Select",
}; 
//提示信息的窗体名字
u8*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"提示信息","提示信息","Remind",	 
};
//提醒保存的窗体名字
u8*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"是否保存编辑后的文件?","是否保存編輯后的文件?","Do you want to save?",
};

//创建文件失败,提示是否存在SD卡? ,200的宽度
u8*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM]=
{							 
"创建文件失败,请检查!","創建文件失敗,請檢查!","Creat File Failed,Please Check!",
}; 
//磁盘名字表
u8*const APP_DISK_NAME_TBL[3][GUI_LANGUAGE_NUM]=
{
	{"SD卡","SD卡","SD Card",},
 	{"FLASH磁盘","FLASH磁盤","FLASH Disk",},
 	{"U盘","U盤","U Disk",},
};  							 


u8*const APP_OK_PIC="1:/SYSTEM/APP/COMMON/ok.bmp";				//确认图标
u8*const APP_CANCEL_PIC="1:/SYSTEM/APP/COMMON/cancel.bmp";		//取消图标
u8*const APP_UNSELECT_PIC="1:/SYSTEM/APP/COMMON/unselect.bmp";	//未选中图标
u8*const APP_SELECT_PIC="1:/SYSTEM/APP/COMMON/select.bmp";		//选中图标

//PC2LCD2002字体取模方法:逐列式,顺向(高位在前),阴码.C51格式.																		    
//特殊字体:
//数码管字体:ASCII集+℃(' '+95)
//普通字体:ASCII集
u8*const APP_ASCII_S6030="1:/SYSTEM/APP/COMMON/fonts60.fon";	//数码管字体60*30大数字字体路径 
u8*const APP_ASCII_5427="1:/SYSTEM/APP/COMMON/font54.fon";		//普通字体54*27大数字字体路径 
u8*const APP_ASCII_3618="1:/SYSTEM/APP/COMMON/font36.fon";		//普通字体36*18大数字字体路径
u8*const APP_ASCII_2814="1:/SYSTEM/APP/COMMON/font28.fon";		//普通字体28*14大数字字体路径 

u8* asc2_s6030=0;	//数码管字体60*30大字体点阵集
u8* asc2_5427=0;	//普通字体54*27大字体点阵集
u8* asc2_3618=0;	//普通字体36*18大字体点阵集
u8* asc2_2814=0;	//普通字体28*14大字体点阵集

//ALIENTEK logo 图标(18*24大小)
//PCtoLCD2002取模方式:阴码,逐行式,顺向
const u8 APP_ALIENTEK_ICO1824[]=
{
0x03,0xF0,0x00,0x0F,0xF8,0x00,0x1F,0xFC,0x00,0x3F,0xFE,0x00,0x7F,0xFF,0x00,0x7F,
0xFF,0x00,0xFF,0xFF,0x80,0xFF,0xFF,0x80,0xFF,0xFF,0x80,0xFF,0xFF,0x80,0xFF,0xFF,
0x80,0xFF,0xFF,0x80,0xCF,0xF9,0x80,0xC7,0xF1,0x80,0x43,0xE1,0x00,0x61,0xC3,0x00,
0x31,0xC6,0x00,0x39,0xCE,0x00,0x1F,0xFC,0x00,0x0F,0xF8,0x00,0x07,0xF0,0x00,0x03,
0xE0,0x00,0x01,0xC0,0x00,0x00,0x80,0x00
};
//ALIENTEK logo 图标(24*32大小)
//PCtoLCD2002取模方式:阴码,逐行式,顺向
const u8 APP_ALIENTEK_ICO2432[]=
{
0x00,0x7F,0x00,0x01,0xFF,0xC0,0x03,0xFF,0xE0,0x07,0xFF,0xF0,0x0F,0xFF,0xF8,0x1F,
0xFF,0xFC,0x1F,0xFF,0xFC,0x3F,0xFF,0xFE,0x3F,0xFF,0xFE,0x7F,0xFF,0xFF,0x7F,0xFF,
0xFF,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,
0x73,0xFF,0xE7,0x70,0xFF,0x87,0x30,0xFF,0x86,0x30,0x7F,0x06,0x38,0x3E,0x0E,0x1C,
0x3E,0x1C,0x1E,0x1C,0x3C,0x0F,0x1C,0x78,0x07,0xFF,0xF0,0x07,0xFF,0xF0,0x03,0xFF,
0xE0,0x01,0xFF,0xC0,0x00,0xFF,0x80,0x00,0x7F,0x00,0x00,0x3E,0x00,0x00,0x1C,0x00,
};
//ALIENTEK logo 图标(36*48大小)
//PCtoLCD2002取模方式:阴码,逐行式,顺向
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
////////////////////////////////伪随机数产生办法////////////////////////////////
u32 random_seed=1;
void app_srand(u32 seed)
{
	random_seed=seed;
}
//获取伪随机数
//可以产生0~RANDOM_MAX-1的随机数
//seed:种子
//max:最大值	  		  
//返回值:0~(max-1)中的一个值 		
u32 app_get_rand(u32 max)
{			    	    
	random_seed=random_seed*22695477+1;
	return (random_seed)%max; 
}   
//读取背景色
//x,y,width,height:背景色读取范围
//ctbl:背景色存放指针
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl)
{
	u32	x0,y0,ccnt;
	ccnt=0;
	for(y0=y;y0<y+height;y0++)
	{
		for(x0=x;x0<x+width;x0++)
		{
			ctbl[ccnt]=gui_phy.read_point(x0,y0);//读取颜色
			ccnt++;
		}
	}
}  
//恢复背景色
//x,y,width,height:背景色还原范围
//ctbl:背景色存放指针
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl)
{
	u32 x0,y0,ccnt;
	ccnt=0;
	for(y0=y;y0<y+height;y0++)
	{
		for(x0=x;x0<x+width;x0++)
		{
			gui_phy.draw_point(x0,y0,ctbl[ccnt]);//读取颜色
			ccnt++;
		}
	}
}
//2色条
//x,y,width,height:坐标及尺寸.
//mode:	设置分界线
//	    [3]:右边分界线
//		[2]:左边分界线
//		[1]:下边分界线
//		[0]:上边分界线
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode)
{
 	u16 halfheight=height/2;
 	gui_fill_rectangle(x,y,width,halfheight,LIGHTBLUE);  			//填充底部颜色(浅蓝色)	
 	gui_fill_rectangle(x,y+halfheight,width,halfheight,GRAYBLUE); 	//填充底部颜色(灰蓝色)
	if(mode&0x01)gui_draw_hline(x,y,width,DARKBLUE);
	if(mode&0x02)gui_draw_hline(x,y+height-1,width,DARKBLUE);
	if(mode&0x04)gui_draw_vline(x,y,height,DARKBLUE);
	if(mode&0x08)gui_draw_vline(x+width-1,y,width,DARKBLUE);
} 
//得到数字的位数
//num:数字
//dir:0,从高位到低位.1,从低位到高位.
//返回值:数字的位数.(最大为10位)
//注:0,的位数为0位.
u8 app_get_numlen(long long num,u8 dir)
{
#define MAX_NUM_LENTH		10		//最大的数字长度
	u8 i=0,j;
	u8 temp=0;  
	if(dir==0)//从高到底
	{
		i=MAX_NUM_LENTH-1;
		while(1)
		{
			temp=(num/gui_pow(10,i))%10;
			if(temp||i==0)break;
			i--;
		}
	}else	//从低到高
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


//显示单色图标
//x,y,width,height:坐标及尺寸.
//icosbase:点整位置
//color:画点的颜色.
//bkcolor:背景色
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor)
{
	u16 rsize;
	u8 i,j;
	u8 temp;
	u8 t=0;
	u16 x0=x;//保留x的位置
	rsize=width/8+((width%8)?1:0);//每行的字节数
 	for(i=0;i<rsize*height;i++)
	{
		temp=icosbase[i];
		for(j=0;j<8;j++)
		{
	        if(temp&0x80)gui_phy.draw_point(x,y,color);  
			else gui_phy.draw_point(x,y,bkcolor);  	 
			temp<<=1;
			x++;
			t++;			//宽度计数器
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
//显示一个浮点数(支持负数)
//注意:这里的坐标是从右到左显示的.
//x,y:开始显示的坐标
//num:数字	   
//flen:小数的位数.0,没有小数部分;1,有一位小数;2,有两位小数;其他以此类推.
//clen:要清除的位数(清除前一次的痕迹)
//font:字体
//color:字体颜色.
//bkcolor:背景色
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor)
{			   
	u8 offpos=1;
	u8 ilen=0;						//整数部分长度和小数部分的长度
	u8 negative=0;					//负数标记
	u16 maxlen=(u16)clen*(font/2);	//清除的长度
	gui_fill_rectangle(x-maxlen,y,maxlen,font,bkcolor);//清除之前的痕迹
	if(num<0) 
	{
		num=-num;
  		negative=1;
	}
	ilen=app_get_numlen(num,0);	//得到总位数
	gui_phy.back_color=bkcolor;
	if(flen)
	{
 		gui_show_num(x-(font/2)*flen,y,flen,color,font,num,0X80);//显示小数部分
		gui_show_ptchar(x-(font/2)*(flen+1),y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,font,'.',0);//显示小数点
		offpos=2+flen;
	} 
	if(ilen<=flen)ilen=1;//至少要有一位整数部分
 	else 
	{
		if(flen)offpos=ilen+1;
		else offpos=ilen;
		ilen=ilen-flen;	//得到整数部分的长度.
	}
	num=num/gui_pow(10,flen);//得到整数部分
	gui_show_num(x-(font/2)*offpos,y,ilen,color,font,num,0X80);	//填充0显示 
	if(negative)gui_show_ptchar(x-(font/2)*(offpos+1),y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,font,'-',0);//显示符号		 
} 			  
//文件浏览横条显示
//topname:浏览的时候要显示的名字	 
//mode:
//[0]:0,不显示上方色条;1,显示上方色条
//[1]:0,不显示下方色条;1,显示下方色条
//[2]:0,不显示名字;1,显示名字
//[3~7]:保留
//返回值:无	 						  
void app_filebrower(u8 *topname,u8 mode)
{		
  	if(mode&0X01)app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);								//下分界线
	if(mode&0X02)app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);//上分界线
	if(mode&0X04)gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,topname);	  
} 
//在一个区域中间显示数字
//x,y,width,height:区域
//num:要显示的数字
//len:位数
//size:字体尺寸
//ptcolor,bkcolor:画笔颜色以及背景色   
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor)
{
	u16 numlen;
	u8 xoff,yoff;
	numlen=(size/2)*len;//数字长度
	if(numlen>width||size>height)return;
	xoff=(width-numlen)/2;
	yoff=(height-size)/2;
	POINT_COLOR=ptcolor;
	BACK_COLOR=bkcolor;
	LCD_ShowxNum(x+xoff,y+yoff,num,len,size,0X80);//显示这个数字
}
//画一条平滑过渡的彩色线(或矩形)
//以中间为间隔,两边展开
//x,y,width,height:线条的坐标尺寸
//sergb,mrgb:起止颜色和中间颜色
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb)
{	  
	gui_draw_smooth_rectangle(x,y,width/2,height,sergb,mrgb);	   		//前半段渐变
	gui_draw_smooth_rectangle(x+width/2,y,width/2,height,mrgb,sergb);   //后半段渐变
}      

//判断触摸屏当前值是不是在某个区域内
//tp:触摸屏
//x,y,width,height:坐标和尺寸 
//返回值:0,不在区域内;1,在区域内.
u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height)
{						 	 
	if(tp->x[0]<=(x+width)&&tp->x[0]>=x&&tp->y[0]<=(y+height)&&tp->y[0]>=y)return 1;
	else return 0;							 	
}

//得到STM32的序列号
//sn0,sn1,sn2:3个固有序列号
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2)
{
	*sn0=*(vu32*)(0x1FFFF7E8);
	*sn1=*(vu32*)(0x1FFFF7EC);
	*sn2=*(vu32*)(0x1FFFF7F0);     
}
//打印SN
void app_usmart_getsn(void)
{
	u32 sn0,sn1,sn2;
	app_getstm32_sn(&sn0,&sn1,&sn2);
	printf("\r\nSerial Number:%X%X%X\r\n",sn0,sn1,sn2);
}
