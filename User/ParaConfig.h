#ifndef _PARACONFIG_H_
#define _PARACONFIG_H_

#include "CH549.H"
#include "DEBUG.H"

#include "FlashRom.H"
#include "BasicIO.h"
//#include "CompositeHID.H"

//靠前60k(0x0000~0xEFFF)为CodeFlash 寿命10k次
//靠后1k(0xF000~0xF3FF)为DataFlash 寿命约能100k次
//最后3k(0xF400~0xFFFF)为BootLoader和ConfigInfo 用户不可使用
#define DATA_CFG_BASE		0xF200//配置数据起始
#define DATA_GLOB_BASE		0xE800//全局数据起始
#define DATA_LIGHT_BASE		0xE700//灯效数据起始
//配置数据占CodeFlash的后1k和DataFlash的1k,每套512B,空间4套,目前使用后3套,从后往前排
//全局数据占CodeFlash的倒数第2k的开头部分,目前使用64B
//灯效数据占CodeFlash的倒数第3k,每套256B,空间4套,目前使用后3套,从后往前排
//目前全部存储数据使用4k空间,剩余代码空间为57k

#define CFG_NUM		3//配置个数

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
#define CFG_RGB_R			(*(CFG_THIS + 476))//RGB灯色值R
#define CFG_RGB_G			(*(CFG_THIS + 477))//RGB灯色值G
#define CFG_RGB_B			(*(CFG_THIS + 478))//RGB灯色值B
#define CFG_KB_DIR			(*(CFG_THIS + 479))//键盘方向
#define CFG_SCN_W			(*((PUINT16C)(CFG_THIS + 480)))//屏幕宽度
#define CFG_SCN_H			(*((PUINT16C)(CFG_THIS + 482)))//屏幕高度
#define CFG_R_MODE			(*(CFG_THIS + 484))//摇杆模式及按键模式
#define CFG_R_KEY(n)		(*(CFG_THIS + 485 + (n)))//摇杆按键 中上下左右
#define CFG_R_FUNC			(*(CFG_THIS + 490))//摇杆功能键
#define CFG_R_DIR			(*(CFG_THIS + 491))//摇杆方向
#define CFG_R_DEAD			(*(CFG_THIS + 492))//摇杆死区
#define CFG_R_PARA			(*(CFG_THIS + 493))//摇杆参数
#define CFG_R_RSV1			(*(CFG_THIS + 494))//摇杆rsv1
#define CFG_R_RSV2			(*(CFG_THIS + 495))//摇杆rsv2
#define CFG_E_MODE(i)		(*(CFG_THIS + 496 + (i) * 6))//旋钮模式及按键模式
#define CFG_E_KEY(i,n)		(*(CFG_THIS + 497 + (i) * 6 + (n)))//旋钮按键 中逆顺
#define CFG_E_FUNC(i)		(*(CFG_THIS + 500 + (i) * 6))//旋钮功能键
#define CFG_E_DIR(i)		(*(CFG_THIS + 501 + (i) * 6))//旋钮方向
#define CFG_RGB_CYCLE		(*(CFG_THIS + 508))//变色周期和呼吸周期
#define CFG_RGB_SET			(*(CFG_THIS + 509))//RGB灯设置
#define CFG_RGB_LIGHT		(*(CFG_THIS + 510))//RGB灯亮度
#define CFG_ALL_PRI			(*(CFG_THIS + 511))//总优先级

#define CFGb_R_DIRx			((CFG_R_DIR >> 0) & 1)//摇杆X反向
#define CFGb_R_DIRy			((CFG_R_DIR >> 1) & 1)//摇杆Y反向
#define CFGb_R_DIRr			((CFG_R_DIR >> 2) & 1)//摇杆90度转向
#define CFGb_Rk_MODE		(CFG_R_MODE >> 4)//摇杆按键模式
#define CFGb_R_MODE			(CFG_R_MODE & 0x0F)//摇杆模式
#define CFGb_Ek_MODE(i)		(CFG_E_MODE(i) >> 4)//旋钮按键模式
#define CFGb_E_MODE(i)		(CFG_E_MODE(i) & 0x0F)//旋钮模式
#define CFGb_RGB_COLORFUL	(CFG_RGB_CYCLE >> 4)//变色周期
#define CFGb_RGB_WAVE		(CFG_RGB_CYCLE & 0x0F)//呼吸周期
#define CFGb_RGB_RK			((CFG_RGB_SET >> 7) & 1)//摇杆映射
#define CFGb_RGB_CLICKER	((CFG_RGB_SET >> 6) & 1)//连点指示
#define CFGb_RGB_LOOP		((CFG_RGB_SET >> 5) & 1)//循环指示
#define CFGb_RGB_TIME		(CFG_RGB_SET & 0x0F)//配置指示时间

#define GLOB_THIS		(GLOB_CFG)//全局配置起始
#define GLOB_ANA_MID1		(*((PUINT16C)(GLOB_THIS + 0)))//摇杆中位
#define GLOB_ANA_MID2		(*((PUINT16C)(GLOB_THIS + 2)))//摇杆中位
#define GLOB_KEY_FLT		(*(GLOB_THIS + 4))//按键滤波值


extern UINT8X FlashBuf[512];//配置缓存数组
extern uint16_t keyAddr[CFG_NUM][16];//3组每组16按键的数据地址
extern uint16_t keyWork[16];//3组每组16按键的工作用数组
extern uint8_t keyFlag[16];//16按键的标记用数组
extern uint8_t key_turn[CFG_NUM];//键盘方向
extern uint8_t sys_cs;//总配置选择
extern PUINT8C DATA_CFG;//闪存区配置信息指针


void ParaSave(uint8_t pos, uint8_t num);//参数保存
void ParaLoad();//参数读取
void ParaUpdate(uint8_t pos);//参数更新
void GlobalParaLoad();//全局参数读取
void GlobalParaUpdate();//全局参数更新


#endif



