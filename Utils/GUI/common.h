#ifndef __COMMON_H
#define __COMMON_H 	
#include "./sys/sys.h"
#include "./TOUCH/touch.h"
#include "./gui.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP通用 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/2/16
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//V1.1 20140216
//新增对各种分辨率LCD的支持.
//V1.2 20140727
//修改app_show_float函数的一个bug

////////////////////////////////////////////////////////////////////////////////////////////
//各图标/图片路径
extern u8*const APP_OK_PIC;			//确认图标
extern u8*const APP_CANCEL_PIC;		//取消图标
extern u8*const APP_UNSELECT_PIC;	//未选中图标
extern u8*const APP_SELECT_PIC;		//选中图标

extern u8*const APP_ASCII_S6030;	//数码管大字体路径
extern u8*const APP_ASCII_5427;		//普通大字体路径
extern u8*const APP_ASCII_3618;		//普通大字体路径
extern u8*const APP_ASCII_2814;		//普通大字体路径

extern u8* asc2_s6030;				//数码管字体60*30大字体点阵集
extern u8* asc2_5427;				//普通字体54*27大字体点阵集
extern u8* asc2_3618;				//普通字体36*18大字体点阵集
extern u8* asc2_2814;				//普通字体28*14大字体点阵集


//app主要功能界面标题

extern u8*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_DELETE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];
//平滑线的起止颜色定义
#define WIN_SMOOTH_LINE_SEC	0XB1FFC4	//起止颜色
#define WIN_SMOOTH_LINE_MC	0X1600B1	//中间颜色

//弹出窗口选择条目的设置信息
#define APP_ITEM_BTN1_WIDTH		60	  		//有2个按键时的宽度
#define APP_ITEM_BTN2_WIDTH		100			//只有1个按键时的宽度
#define APP_ITEM_BTN_HEIGHT		30			//按键高度
#define APP_ITEM_ICO_SIZE		32			//ICO图标的尺寸

#define APP_ITEM_SEL_BKCOLOR	0X0EC3		//选择时的背景色
#define APP_WIN_BACK_COLOR	 	0XC618		//窗体背景色

//π值定义
#define	app_pi	3.1415926535897932384626433832795 
/////////////////////////////////////////////////////////////////////////
																			    
u32  app_get_rand(u32 max);
void app_srand(u32 seed);
void app_set_lcdsize(u8 mode);
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode);
u8 app_get_numlen(long long num,u8 dir);
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor);
void app_filebrower(u8 *topname,u8 mode);
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor);
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb);

u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height);
void app_show_items(u16 x,u16 y,u16 itemwidth,u16 itemheight,u8*name,u8*icopath,u16 color,u16 bkcolor);
u8 * app_get_icopath(u8 mode,u8 *selpath,u8 *unselpath,u8 selx,u8 index);
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *selx,u8 mode,u8*caption);
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize);
void app_muti_remind_msg(u16 x,u16 y,u16 width,u16 height,u8*caption,u8 *str);
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor);

#endif
