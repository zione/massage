#ifndef __COMMON_H
#define __COMMON_H 	
#include "./sys/sys.h"
#include "./TOUCH/touch.h"
#include "./gui.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APPͨ�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/2/16
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//V1.1 20140216
//�����Ը��ֱַ���LCD��֧��.
//V1.2 20140727
//�޸�app_show_float������һ��bug

////////////////////////////////////////////////////////////////////////////////////////////
//��ͼ��/ͼƬ·��
extern u8*const APP_OK_PIC;			//ȷ��ͼ��
extern u8*const APP_CANCEL_PIC;		//ȡ��ͼ��
extern u8*const APP_UNSELECT_PIC;	//δѡ��ͼ��
extern u8*const APP_SELECT_PIC;		//ѡ��ͼ��

extern u8*const APP_ASCII_S6030;	//����ܴ�����·��
extern u8*const APP_ASCII_5427;		//��ͨ������·��
extern u8*const APP_ASCII_3618;		//��ͨ������·��
extern u8*const APP_ASCII_2814;		//��ͨ������·��

extern u8* asc2_s6030;				//���������60*30���������
extern u8* asc2_5427;				//��ͨ����54*27���������
extern u8* asc2_3618;				//��ͨ����36*18���������
extern u8* asc2_2814;				//��ͨ����28*14���������


//app��Ҫ���ܽ������

extern u8*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_DELETE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];
//ƽ���ߵ���ֹ��ɫ����
#define WIN_SMOOTH_LINE_SEC	0XB1FFC4	//��ֹ��ɫ
#define WIN_SMOOTH_LINE_MC	0X1600B1	//�м���ɫ

//��������ѡ����Ŀ��������Ϣ
#define APP_ITEM_BTN1_WIDTH		60	  		//��2������ʱ�Ŀ��
#define APP_ITEM_BTN2_WIDTH		100			//ֻ��1������ʱ�Ŀ��
#define APP_ITEM_BTN_HEIGHT		30			//�����߶�
#define APP_ITEM_ICO_SIZE		32			//ICOͼ��ĳߴ�

#define APP_ITEM_SEL_BKCOLOR	0X0EC3		//ѡ��ʱ�ı���ɫ
#define APP_WIN_BACK_COLOR	 	0XC618		//���屳��ɫ

//��ֵ����
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
