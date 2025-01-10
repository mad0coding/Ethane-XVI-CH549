#ifndef _PARACONFIG_H_
#define _PARACONFIG_H_

#include "CH549.H"
#include "DEBUG.H"

#include "FlashRom.H"
#include "BasicIO.h"
#include "Diag.h"
//#include "CompositeHID.H"

//异步标志定义
#define ASYNC_FLAG_CFG		10
#define ASYNC_FLAG_LIGHT	30
#define ASYNC_FLAG_GLOB		50
#define ASYNC_FLAG_SRST		100
#define ASYNC_FLAG_BOOT		101


//靠前60k(0x0000~0xEFFF)为CodeFlash 寿命10k次
//靠后1k(0xF000~0xF3FF)为DataFlash 寿命约能100k次
//最后3k(0xF400~0xFFFF)为BootLoader和ConfigInfo 用户不可使用
#define DATA_CFG_BASE		0xF200//配置数据起始
#define DATA_LIGHT_BASE		0xDC00//灯效数据起始
#define DATA_GLOB_BASE		0xDA00//全局数据起始
#define DATA_DIAG_BASE		0xD800//诊断数据起始
//配置数据占CodeFlash的后3k和DataFlash的1k,每套512B,空间8套,目前使用后4套,从后往前排
//灯效数据占CodeFlash的倒数第4~5k,每套256B,空间8套,目前使用前4套,从前往后排
//全局数据占CodeFlash的倒数第6k的后一半的开头部分,目前使用64B
//诊断数据占CodeFlash的倒数第6k的前一半的开头部分,目前使用64B
//目前全部存储数据使用7k空间,剩余代码空间为54k

#define CFG_NUM		8	//配置个数

//键盘配置
#define CFG_THIS		(DATA_CFG)//键盘配置起始
#define CFG_ACS(a)			(*((PUINT8C)(a) + 0))//按键直接访问
#define CFG_K_ID(a)			(*((PUINT8C)(a) + 0))//按键ID
#define CFG_K_MODE(a)		(*((PUINT8C)(a) + 1))//按键模式
#define CFG_K_KEY(a)		(*((PUINT8C)(a) + 2))//按键键值
#define CFG_K_LEN(a)		(*((PUINT8C)(a) + 2))//按键组模式时按键长度
#define CFG_K_FUNC(a)		(*((PUINT8C)(a) + 3))//按键功能键
#define CFG_K_M3DT(a)		(*((PUINT8C)(a) + 3))//按键组模式时数据起始
#define CFG_K_X(a)			(*((PUINT16C)((a) + 2)))//光标模式时X值
#define CFG_K_Y(a)			(*((PUINT16C)((a) + 4)))//光标模式时Y值
#define CFG_K_T(a)			(*((PUINT16C)((a) + 4)))//连点模式时周期

#define CFG_R_DATA_S		(CFG_THIS + 468)//摇杆数据起始
#define CFG_R_DATA_L		(12)//摇杆数据长度
#define CFG_R_MODE(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 0))//摇杆模式及按键模式
#define CFG_R_DIR(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 1))//摇杆方向
#define CFG_R_KEY(i,n)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 2 + (n)))//摇杆按键 中上下左右
#define CFG_R_FUNC(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 7))//摇杆功能键
#define CFG_R_DEAD(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 8))//摇杆死区
#define CFG_R_PARA(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 9))//摇杆参数
#define CFG_R_NEER(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 10))//摇杆近端参数
#define CFG_R_FAR(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 11))//摇杆远端参数

#define CFG_E_DATA_S		(CFG_THIS + 480)//旋钮数据起始
#define CFG_E_DATA_L		(8)//旋钮数据长度
#define CFG_E_MODE(i)		(*(CFG_E_DATA_S + (i) * CFG_E_DATA_L + 0))//旋钮模式及按键模式
#define CFG_E_DIR(i)		(*(CFG_E_DATA_S + (i) * CFG_E_DATA_L + 1))//旋钮方向
#define CFG_E_KEY(i,n)		(*(CFG_E_DATA_S + (i) * CFG_E_DATA_L + 2 + (n)))//旋钮按键 中逆顺
#define CFG_E_FUNC(i,n)		(*(CFG_E_DATA_S + (i) * CFG_E_DATA_L + 5 + (n)))//旋钮功能键 中逆顺

#define CFG_RGB_RGB(i)		(*(CFG_THIS + 496 + (i)))//RGB灯标签色值RGB
#define CFG_RGB_SIGN(i)		(*(CFG_THIS + 499 + (i)))//RGB灯指示色值RGB
#define CFG_RGB_CFG(i)		(*(CFG_THIS + 502 + (i)))//RGB灯指示灯配置
#define CFG_RGB_TIME		(*(CFG_THIS + 505))//RGB灯标签时间

#define CFG_SCN_W			(*((PUINT16C)(CFG_THIS + 506)))//屏幕宽度
#define CFG_SCN_H			(*((PUINT16C)(CFG_THIS + 508)))//屏幕高度
#define CFG_KB_DIR			(*(CFG_THIS + 510))//键盘方向
#define CFG_ALL_PRI			(*(CFG_THIS + 511))//总优先级

//键盘配置比特
#define CFGb_R_DIRx(i)		((CFG_R_DIR(i) >> 0) & 1)//摇杆X反向
#define CFGb_R_DIRy(i)		((CFG_R_DIR(i) >> 1) & 1)//摇杆Y反向
#define CFGb_R_DIRr(i)		((CFG_R_DIR(i) >> 2) & 1)//摇杆90度转向
#define CFGb_Rk_MODE(i)		(CFG_R_MODE(i) >> 4)//摇杆按键模式
#define CFGb_R_MODE(i)		(CFG_R_MODE(i) & 0x0F)//摇杆模式
#define CFGb_Ek_MODE(i)		(CFG_E_MODE(i) >> 4)//旋钮按键模式
#define CFGb_E_MODE(i)		(CFG_E_MODE(i) & 0x0F)//旋钮模式

#define CFGb_RGB_DIR(i)		(CFG_RGB_CFG(i) >> 7)//RGB灯指示灯方向
#define CFGb_RGB_MAP(i)		(CFG_RGB_CFG(i) & 0x0F)//RGB灯指示灯映射
#define CFGb_RGB_T_ON		(CFG_RGB_TIME >> 4)//RGB灯标签亮时间
#define CFGb_RGB_T_OFF		(CFG_RGB_TIME & 0x0F)//RGB灯标签渐灭速度



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
extern uint16_t keyAddr[CFG_NUM][16];//3组每组16按键的数据地址
extern uint16_t keyWork[16];//3组每组16按键的工作用数组
extern uint8_t keyFlag[16];//16按键的标记用数组
extern uint8_t keyDir[CFG_NUM];//键盘方向
extern uint8_t sysCs;//总配置选择
extern PUINT8C DATA_CFG;//闪存区配置信息指针

void AsyncHandle(uint8_t flag);//异步处理
void ParaSave(uint8_t pos, uint8_t num);	//参数保存
uint8_t ParaWrite(uint16_t addr, uint8_t *buf, uint8_t num);	//参数写入
void ParaLoad(void);					//参数读取
void ParaUpdate(uint8_t pos);			//参数更新
void GlobalParaLoad(void);		//全局参数读取
void GlobalParaUpdate(void);	//全局参数更新


#endif



