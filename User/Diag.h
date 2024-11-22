#ifndef _DIAG_H_
#define _DIAG_H_


#include "ParaConfig.h"

//复位原因
#define RST_REASON_SW		0x00//软复位
#define RST_REASON_PWR		0x01//电源复位
#define RST_REASON_WDG		0x02//看门狗复位
#define RST_REASON_PIN		0x03//引脚复位

//格式化设置
#define DIAG_FMT_NONE		0x00//普通格式化
#define DIAG_FMT_LOAD		0x01//格式化前先加载数据
#define DIAG_FMT_SAVE		0x02//格式化后保存数据
#define DIAG_FMT_MARK		0x10//格式化标志错误计数

#define DIAG_MARK_VALUE		0xEA16C549//诊断数据标志

#define DIAG_THIS		(FlashBuf)//诊断数据起始 使用缓存地址
#define DIAG_CNT(i)			(*(PUINT16X)(DIAG_THIS + 2*i))
#define DIAG_CNT_MARK			(*(PUINT16X)(DIAG_THIS + 0))
#define DIAG_CNT_SUM			(*(PUINT16X)(DIAG_THIS + 2))
#define DIAG_CNT_WDG			(*(PUINT16X)(DIAG_THIS + 8))
#define DIAG_CNT_WDGS			(*(PUINT16X)(DIAG_THIS + 10))
#define DIAG_SUM				(*(PUINT16X)(DIAG_THIS + 58))
#define DIAG_MARK				(*(PUINT32X)(DIAG_THIS + 60))

#define DIAG_KEEP		(RESET_KEEP)//接管复位保持寄存器
#define DIAG_RST_FLAG			((PCON >> 4) & 0x03)//本次复位标志
#define DIAG_RST_LAST			((DIAG_KEEP >> 3) & 0x01)//上次复位标志
#define DIAG_RST_CNT			(DIAG_KEEP & 0x07)//连续复位标志
#define DIAG_RST_LAST_SET(x)	(DIAG_KEEP = (DIAG_KEEP & ~(0x01 << 3)) | ((x) << 3))
#define DIAG_RST_CNT_SET(x)		(DIAG_KEEP = (DIAG_KEEP & ~0x07 | (x)))
#define DIAG_RST_CNT_DEC		(DIAG_KEEP--)//计数递减 使用时注意溢出



void FlashCountInc(uint8_t pos, uint8_t fail);		//闪存擦除计数增加
uint32_t FlashCountGet(uint8_t pos, uint8_t fail);	//闪存擦除计数获取

void DiagRead(uint8_t *buf, uint8_t len);//诊断数据读取


#endif



