#ifndef _RGBDRV_H_
#define _RGBDRV_H_

#include "CH549.H"
#include "DEBUG.H"
#include <intrins.h>
#include <stdlib.h>

#include "BasicIO.h"
#include "ParaUse.h"

// NOP延时
#define WS_NOP_BIT0		_nop_();
#define WS_NOP_BIT1		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();

#define WS_DOUT		P2_7
#define PWM_R		PWM_DATA5
#define PWM_G		PWM_DATA2
#define PWM_B		PWM_DATA4

#define COLOR_ANGLE		167//色环的1/6
#define LIGHT_LIST_MAX	9//列表上限


#define LIGHT_ADDR_CSC(i)	(DATA_LIGHT_BASE + (i) * 256)//灯效数据地址 正序
#define LIGHT_DATA_CSC(i)	(DATA_LIGHT = LIGHT_ADDR_CSC(i))//灯效数据选择

#define LIGHT_THIS		(DATA_LIGHT)

//#define LIGHT_ACS(a)		(*((PUINT8C)(a) + 0))

#define LIGHT_UP(n)			(*(LIGHT_THIS + (n) + 0))//按键上配色
#define LIGHT_DOWN(n)		(*(LIGHT_THIS + (n) + 3*16))//按键下配色
#define LIGHT_T_D(n)		(*(LIGHT_THIS + (n) + 6*16))//按键按下灯渐变
#define LIGHT_T_U(n)		(*(LIGHT_THIS + (n) + 7*16))//按键抬起灯渐变
#define LIGHT_SHLD(n)		(*(LIGHT_THIS + (n) + 8*16))//按键屏蔽剩余
#define LIGHT_IDX(n)		(*(LIGHT_THIS + (n) + 9*16))//按键自定义下标
#define LIGHT_LIST(n)		(*(LIGHT_THIS + 224 + (n)))//列表
#define LIGHT_COUNT			(*(LIGHT_THIS + 235))//列表计数
#define LIGHT_MONO			(*(LIGHT_THIS + 238))//配置独占
#define LIGHT_DIR			(*(LIGHT_THIS + 240))//动作方向设置
#define LIGHT_WAVE			(*(LIGHT_THIS + 241))//动作模式设置
#define LIGHT_COLOR_S		(*(LIGHT_THIS + 242))//变色设置
#define LIGHT_COLOR_T		(*(LIGHT_THIS + 243))//变色周期(s)
#define LIGHT_T_WAIT		(*((PUINT16C)(LIGHT_THIS + 244)))//等待时间(10ms)
#define LIGHT_T_ACT			(*((PUINT16C)(LIGHT_THIS + 246)))//动作时间(ms)
#define LIGHT_T_GAP			(*((PUINT16C)(LIGHT_THIS + 248)))//间隔时间(ms)
#define LIGHT_T_RAND		(*((PUINT16C)(LIGHT_THIS + 250)))//间隔随机时间(ms)
#define LIGHT_G_ON			(*(LIGHT_THIS + 252))//亮渐变
#define LIGHT_G_OFF			(*(LIGHT_THIS + 253))//灭渐变
#define LIGHT_M_ON			(*(LIGHT_THIS + 254))//屏蔽亮渐变
#define LIGHT_M_OFF			(*(LIGHT_THIS + 255))//屏蔽灭渐变


extern PUINT8C DATA_LIGHT;//闪存区灯效信息指针


void ClearKeyRGB(void);//清除键盘RGB
void KeyRGB(uint8_t clear);//键盘RGB控制

void Rgb2Hsv(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV);//RGB转HSV
void Hsv2Rgb(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB);//HSV转RGB

void WsWrite16(void);//写入16个灯

void SysRGB();//系统RGB控制





#endif

