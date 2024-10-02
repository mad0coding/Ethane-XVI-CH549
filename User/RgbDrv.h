#ifndef _RGBDRV_H_
#define _RGBDRV_H_

#include "CH549.H"
#include "DEBUG.H"
#include <intrins.h>
#include <stdlib.h>

#include "BasicIO.h"
#include "ParaUse.h"


#define WS_DOUT		P2_7
#define PWM_R		PWM_DATA5
#define PWM_G		PWM_DATA2
#define PWM_B		PWM_DATA4

#define colorAngle	167//色环的1/6

#define LIGHT_THIS		(DATA_LIGHT)

//#define LIGHT_ACS(a)		(*((PUINT8C)(a) + 0))

#define LIGHT_UP(n)			(*(LIGHT_THIS + (n) + 0))//按键上配色
#define LIGHT_DOWN(n)		(*(LIGHT_THIS + (n) + 3*16))//按键下配色
#define LIGHT_T1(n)			(*(LIGHT_THIS + (n) + 6*16))//按键按下灯延迟
#define LIGHT_T2(n)			(*(LIGHT_THIS + (n) + 7*16))//按键抬起灯延迟
#define LIGHT_SHLD(n)		(*(LIGHT_THIS + (n) + 8*16))//按键屏蔽剩余
#define LIGHT_IDX(n)		(*(LIGHT_THIS + (n) + 9*16))//按键自定义下标
#define LIGHT_MONO			(*(LIGHT_THIS + 244))//配置独占
#define LIGHT_DIR			(*(LIGHT_THIS + 245))//动作方向设置
#define LIGHT_WAVE			(*(LIGHT_THIS + 246))//动作模式设置
#define LIGHT_COLORFUL		(*(LIGHT_THIS + 247))//变色周期(s)
#define LIGHT_D1WAVE		(*((PUINT16C)(LIGHT_THIS + 248)))//动作时间(ms)
#define LIGHT_D2WAVE		(*((PUINT16C)(LIGHT_THIS + 250)))//间隔时间(ms)
#define LIGHT_T1WAVE		(*(LIGHT_THIS + 252))//亮延迟
#define LIGHT_T2WAVE		(*(LIGHT_THIS + 253))//灭延迟
#define LIGHT_T1SYS			(*(LIGHT_THIS + 254))//屏蔽亮延迟
#define LIGHT_T2SYS			(*(LIGHT_THIS + 255))//屏蔽灭延迟


extern PUINT8C DATA_LIGHT;//闪存区灯效信息指针

void clearKeyRGB(void);//清除键盘RGB
void keyRGB(uint8_t clear);//键盘RGB控制

void rgbToHSV(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV);//RGB转HSV
void hsvToRGB(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB);//HSV转RGB

void wsWrite16(void);//写入16个灯

void sysRGB();//系统RGB控制





#endif

