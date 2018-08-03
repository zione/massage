/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : fsmc_nand.h
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Header for fsmc_nand.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_NAND_H
#define __BSP_NAND_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint8_t Maker_ID;
  uint8_t Device_ID;
  uint8_t Third_ID;
  uint8_t Fourth_ID;
}NAND_IDTypeDef;

typedef struct 
{
  uint16_t Zone;
  uint16_t Block;
  uint16_t Page;
} NAND_ADDRESS_T;

/* NAND Flash �ͺ� */
typedef enum
{
	HY27UF081G2A = 0,
	K9F1G08U0A,
	K9F1G08U0B,
	NAND_UNKNOW
}NAND_TYPE_E;

#define NAND_TYPE	HY27UF081G2A

/* 	
	������Ч�� NAND ID
	HY27UF081G2A  	= 0xAD 0xF1 0x80 0x1D
	K9F1G08U0A		= 0xEC 0xF1 0x80 0x15
	K9F1G08U0B		= 0xEC 0xF1 0x00 0x95
*/
#define NAND_MAKER_ID	0xAD
#define NAND_DEVICE_ID	0xF1
#define NAND_THIRD_ID 	0x80
#define NAND_FOURTH_ID	0x1D

#define HY27UF081G2A	0xADF1801D
#define K9F1G08U0A		0xECF18015
#define K9F1G08U0B		0xECF10095

/* Exported constants --------------------------------------------------------*/
/* NAND Area definition  for STM3210E-EVAL Board RevD */
#define CMD_AREA                   (uint32_t)(1<<16)  /* A16 = CLE  high */
#define ADDR_AREA                  (uint32_t)(1<<17)  /* A17 = ALE high */
#define DATA_AREA                  ((uint32_t)0x00000000) 

/* FSMC NAND memory command */
#define	NAND_CMD_AREA_A            ((uint8_t)0x00)
#define	NAND_CMD_AREA_B            ((uint8_t)0x01)
#define NAND_CMD_AREA_C            ((uint8_t)0x50)
#define NAND_CMD_AREA_TRUE1        ((uint8_t)0x30)

#define NAND_CMD_WRITE0            ((uint8_t)0x80)
#define NAND_CMD_WRITE_TRUE1       ((uint8_t)0x10)
	
#define NAND_CMD_ERASE0            ((uint8_t)0x60)
#define NAND_CMD_ERASE1            ((uint8_t)0xD0)  

#define NAND_CMD_READID            ((uint8_t)0x90)	

#define NAND_CMD_LOCK_STATUS       ((uint8_t)0x7A)
#define NAND_CMD_RESET             ((uint8_t)0xFF)

/* NAND memory status */
#define NAND_BUSY                  ((uint8_t)0x00)
#define NAND_ERROR                 ((uint8_t)0x01)
#define NAND_READY                 ((uint8_t)0x40)
#define NAND_TIMEOUT_ERROR         ((uint8_t)0x80)

/* FSMC NAND memory parameters */
/* ����HY27UF081G2A    K9F1G08 */
#if NAND_TYPE == HY27UF081G2A
	#define NAND_PAGE_SIZE             ((uint16_t)0x0800) /* 2 * 1024 bytes per page w/o Spare Area */
	#define NAND_BLOCK_SIZE            ((uint16_t)0x0040) /* 64 pages per block */
	#define NAND_ZONE_SIZE             ((uint16_t)0x0400) /* 1024 Block per zone */
	#define NAND_SPARE_AREA_SIZE       ((uint16_t)0x0040) /* last 64 bytes as spare area */
	#define NAND_MAX_ZONE              ((uint16_t)0x0001) /* 1 zones of 1024 block */

	/* ������붨�� */
	#define NAND_CMD_COPYBACK_A			((uint8_t)0x00)		/* PAGE COPY-BACK �������� */
	#define NAND_CMD_COPYBACK_B			((uint8_t)0x35)	
	#define NAND_CMD_COPYBACK_C			((uint8_t)0x85)	
	#define NAND_CMD_COPYBACK_D			((uint8_t)0x10)	
	
	#define NAND_CMD_STATUS				((uint8_t)0x70)		/* ��NAND Flash��״̬�� */

	#define MAX_PHY_BLOCKS_PER_ZONE  1024	/* ÿ�������������� */
	#define MAX_LOG_BLOCKS_PER_ZONE  1000	/* ÿ��������߼���� */
	
	#define NAND_BLOCK_COUNT			1024 /* ����� */
	#define NAND_PAGE_TOTAL_SIZE		(NAND_PAGE_SIZE + NAND_SPARE_AREA_SIZE)	/* ҳ���ܴ�С */
	
	
	
#else
	#define NAND_PAGE_SIZE             ((uint16_t)0x0200) /* 512 bytes per page w/o Spare Area */
	#define NAND_BLOCK_SIZE            ((uint16_t)0x0020) /* 32x512 bytes pages per block */
	#define NAND_ZONE_SIZE             ((uint16_t)0x0400) /* 1024 Block per zone */
	#define NAND_SPARE_AREA_SIZE       ((uint16_t)0x0010) /* last 16 bytes as spare area */
	#define NAND_MAX_ZONE              ((uint16_t)0x0004) /* 4 zones of 1024 block */
#endif

#define NAND_BAD_BLOCK_FLAG			0x00	/* ���ڵ�1��page�������ĵ�1���ֽ�д���0xFF���ݱ�ʾ���� */
#define NAND_USED_BLOCK_FLAG		0xF0	/* ���ڵ�1��page�������ĵ�2���ֽ�д���0xFF���ݱ�ʾ���� */

#define BI_OFFSET				0		/* ���ڵ�1��page�������ĵ�1���ֽ��ǻ����־ */
#define USED_OFFSET				1		/* ���ڵ�1��page�������ĵ�1���ֽ������ñ�־ */
#define LBN0_OFFSET				2		/* ���ڵ�1��page�������ĵ�3���ֽڱ�ʾ�߼���ŵ�8bit */
#define LBN1_OFFSET				3		/* ���ڵ�1��page�������ĵ�4���ֽڱ�ʾ�߼���Ÿ�8bit */
#define VALID_SPARE_SIZE		4		/* ʵ��ʹ�õı�������С,���ں����ڲ��������ݻ�������С */

/* FSMC NAND memory address computation */
#define ADDR_1st_CYCLE(ADDR)       (uint8_t)((ADDR)& 0xFF)               /* 1st addressing cycle */
#define ADDR_2nd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
#define ADDR_3rd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define ADDR_4th_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#define NAND_OK   0
#define NAND_FAIL 1

#define FREE_BLOCK  (1 << 12 )
#define BAD_BLOCK   (1 << 13 )
#define VALID_BLOCK (1 << 14 )
#define USED_BLOCK  (1 << 15 )

/*
		LUT[]�ĸ�ʽ��
		uint16_t usGoodBlockFirst;				 // ��1���ÿ�
		uint16_t usDataBlockCount;	             // ���������ݴ洢�Ŀ����, �ӵ�2���ÿ鿪ʼ
		uint16_t usBakBlockStart;				 // ���ݿ���ʼ���
		uint32_t usPhyBlockNo[ulDataBlockCount]; // ����������顣���ֽ���ǰ�����ֽ��ں�		
*/		
#define DATA_BLOCK_PERCENT		98	/* ���ݿ�ռ����Ч�����İٷֱ� */
#define LUT_FIRST_GOOD_BLOCK	0	/* LUT[] ��1����Ԫ���ڴ洢��1����Ч��� */
#define LUT_DATA_BLOCK_COUNT	1	/* LUT[] ��2����Ԫ���ڴ洢����Ч��Ÿ��� */
#define LUT_BAK_BLOCK_START		2	/* LUT[] ��3����Ԫ���ڱ�������ʼ��� */
#define LUT_GOOD_BLOCK_START	3	/* LUT[] ��4����Ԫ������������ʼ��� */


/* Private Structures---------------------------------------------------------*/
typedef struct __SPARE_AREA {
	uint16_t LogicalIndex;
	uint16_t DataStatus;
	uint16_t BlockStatus;
} SPARE_AREA;	

typedef enum {
  WRITE_IDLE = 0,
  POST_WRITE,
  PRE_WRITE,
  WRITE_CLEANUP,
  WRITE_ONGOING  
}WRITE_STATE;  

typedef enum {
  OLD_BLOCK = 0,
  UNUSED_BLOCK
}BLOCK_STATE; 

/* Private macro --------------------------------------------------------------*/
//#define WEAR_LEVELLING_SUPPORT		ĥ��ƽ��֧��
#define WEAR_DEPTH         10			/* ĥ����� */
#define PAGE_TO_WRITE      (Transfer_Length/512)

#define BAD_BALOK_TEST_CYCLE	5		/* �б𻵿��㷨���ظ���д����  */

/* Private variables ----------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
/* exported functions ---------------------------------------------------------*/
uint8_t NAND_Init(void);
uint8_t NAND_Write(uint32_t _ulMemAddr, uint32_t *_pWriteBuf, uint16_t _usSize);
uint8_t NAND_Read(uint32_t _ulMemAddr, uint32_t *_pReadBuf, uint16_t _usSize);
uint8_t NAND_Format(void);
void NAND_DispBadBlockInfo(void);
uint8_t NAND_ScanBlock(uint32_t _ulPageNo);
uint32_t NAND_FormatCapacity(void);
uint32_t NAND_ReadID(void);

void NANDFLASH_Test(void);

#endif /* __FSMC_NAND_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/