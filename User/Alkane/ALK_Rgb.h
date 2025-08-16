#ifndef _ALK_RGB_H_
#define _ALK_RGB_H_

#include "ALK_Func.h"


#define COLOR_ANGLE		    167 // 色环的1/6
#define LIGHT_LIST_MAX	    9 // 列表上限

#define LIGHT_DATA_SIZE		256 // 灯效配置数据长度

#define LIGHT_ADDR_CSC(i)	(DATA_LIGHT_BASE + (i) * LIGHT_DATA_SIZE) // 灯效数据地址 正序
#define LIGHT_DATA_CSC(i)	(DATA_LIGHT = LIGHT_ADDR_CSC(i)) // 灯效数据选择

#define LIGHT_THIS		(DATA_LIGHT)

#define LIGHT_UP(n)			(*(LIGHT_THIS + (n) + 0))//按键上配色
#define LIGHT_DOWN(n)		(*(LIGHT_THIS + (n) + 3*ALK_KEY_NUM))//按键下配色
#define LIGHT_T_D(n)		(*(LIGHT_THIS + (n) + 6*ALK_KEY_NUM))//按键按下灯渐变
#define LIGHT_T_U(n)		(*(LIGHT_THIS + (n) + 7*ALK_KEY_NUM))//按键抬起灯渐变
#define LIGHT_SHLD(n)		(*(LIGHT_THIS + (n) + 8*ALK_KEY_NUM))//按键屏蔽剩余
#define LIGHT_IDX(n)		(*(LIGHT_THIS + (n) + 9*ALK_KEY_NUM))//按键自定义下标
#define LIGHT_LIST(n)		(*(LIGHT_THIS + 224 + (n)))//列表
#define LIGHT_COUNT			(*(LIGHT_THIS + 235))//列表计数
#define LIGHT_MONO			(*(LIGHT_THIS + 238))//配置独占
#define LIGHT_DIR			(*(LIGHT_THIS + 240))//动作方向设置
#define LIGHT_WAVE			(*(LIGHT_THIS + 241))//动作模式设置
#define LIGHT_COLOR_S		(*(LIGHT_THIS + 242))//变色设置
#define LIGHT_COLOR_T		(*(LIGHT_THIS + 243))//变色周期(s)
#define LIGHT_T_WAIT		(*((ALK_U16C*)(LIGHT_THIS + 244)))//等待时间(10ms)
#define LIGHT_T_ACT			(*((ALK_U16C*)(LIGHT_THIS + 246)))//动作时间(ms)
#define LIGHT_T_GAP			(*((ALK_U16C*)(LIGHT_THIS + 248)))//间隔时间(ms)
#define LIGHT_T_RAND		(*((ALK_U16C*)(LIGHT_THIS + 250)))//间隔随机时间(ms)
#define LIGHT_G_ON			(*(LIGHT_THIS + 252))//亮渐变
#define LIGHT_G_OFF			(*(LIGHT_THIS + 253))//灭渐变
#define LIGHT_M_ON			(*(LIGHT_THIS + 254))//屏蔽亮渐变
#define LIGHT_M_OFF			(*(LIGHT_THIS + 255))//屏蔽灭渐变



extern ALK_U8 rgbHidFlag; // RGB通信标志
extern ALK_U8_LIGHT *DATA_LIGHT; // 闪存区灯效信息指针

extern ALK_U8 FrameRaw[ALK_KEY_NUM*3 + 3]; // 原始帧缓存

extern ALK_U8 numLock, capsLock, scrollLock; // 数字锁定 大写锁定 滚动锁定


void KeyRGB(ALK_U8 clear); // 键盘RGB控制
void SysRGB(); // 系统RGB控制




#endif

