#ifndef __RDA5807M_H
#define	__RDA5807M_H

#include "stm32f10x.h"


typedef enum{EF_NULL,EF_VOL_ADD,EF_VOL_DEC,EF_SEEK_PREV,EF_SEEK_NEXT,EF_REFRESH}TOUCH_EVEN_FM;
						//���¼��������ӣ�������������Ѱ̨������Ѱ̨��ˢ����Ļ
void RDA5807_Volume_Set(uint8_t vol);
void RDA5807_Volume_Add(void);
void RDA5807_Volume_Dec(void);
void RDA5807_Seek_Prev(void);
void RDA5807_Seek_Next(void);
void RDA5807_FM_Test(void);
//void even_process(void);

#endif /* __RDA5807M_H */


