#ifndef _PARACONFIG_H_
#define _PARACONFIG_H_

#include "CH549.H"
#include "DEBUG.H"

#include "FlashRom.H"
#include "BasicIO.h"
#include "Diag.h"
//#include "CompositeHID.H"

#include "ALK_Def.h"

//异步标志定义
#define ASYNC_FLAG_CFG		10
#define ASYNC_FLAG_LIGHT	30
#define ASYNC_FLAG_GLOB		50
#define ASYNC_FLAG_SRST		100
#define ASYNC_FLAG_BOOT		101
#define ASYNC_FLAG_CSC		102
#define ASYNC_FLAG_BUZZ		103



//全局配置
#define GLOB_THIS		((PUINT8C)DATA_GLOB_BASE)//全局配置起始
#define GLOB_ANA_MIN(i)		(*((PUINT16C)(GLOB_THIS + 0 + 2*i)))//摇杆下限
#define GLOB_ANA_MAX(i)		(*((PUINT16C)(GLOB_THIS + 4 + 2*i)))//摇杆上限
#define GLOB_ANA_MID(i)		(*((PUINT16C)(GLOB_THIS + 8 + 2*i)))//摇杆中位
#define GLOB_KEY_FLT		(*(GLOB_THIS + 16))//按键滤波值
#define GLOB_EC_FREQ		(*(GLOB_THIS + 20))//旋钮倍频
//全局配置比特
#define GLOBb_EC_FREQ1		((GLOB_EC_FREQ >> 0) & 1)//一号旋钮倍频
#define GLOBb_EC_FREQ2		((GLOB_EC_FREQ >> 1) & 1)//二号旋钮倍频


extern UINT8X FlashBuf[512];//配置缓存数组


void AsyncHandle(uint8_t flag);//异步处理
void ParaSave(uint8_t pos, uint8_t num);	//参数保存
uint8_t ParaWrite(uint16_t addr, uint8_t *buf, uint8_t num);	//参数写入
void ParaLoad(void);					//参数读取
void ParaUpdate(uint8_t pos);			//参数更新
void GlobalParaLoad(void);		//全局参数读取
void GlobalParaUpdate(void);	//全局参数更新


#endif



