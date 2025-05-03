#ifndef _BASICIO_H_
#define _BASICIO_H_

#include "CH549.H"
#include "DEBUG.H"

#include "ADC.H"
#include "PWM.H"
#include "Timer.H"

#include "CompositeHID.H"
#include "RgbDrv.h"
#include "ParaUse.h"

#define KB_LEN		22//键盘报文长度
#define KP_NUM		19//按键个数

//XDATA地址分配
#define XBASE_EP0_BUF		(XBASE_FLASH_BUF - 64*7)
#define XBASE_EP1_BUF		(XBASE_EP0_BUF + 64*1)
#define XBASE_EP2_BUF		(XBASE_EP0_BUF + 64*3)
#define XBASE_EP3_BUF		(XBASE_EP0_BUF + 64*5)
#define XBASE_FLASH_BUF		(1024+512)

//IO定义
#define KP_1	P1_3
#define KP_2	P3_0
#define KP_3	P3_3
#define KP_4	P4_6
#define KP_5	P1_4
#define KP_6	P4_5
#define KP_7	P3_4
#define KP_8	P4_2
#define KP_9	P1_6
#define KP_10	P4_4
#define KP_11	P3_5
#define KP_12	P4_3
#define KP_13	P1_7
#define KP_14	P3_1
#define KP_15	P3_2
#define KP_16	P3_6

#define KP_E1	P2_6
#define KP_E2	P0_0
#define KP_R	P1_5

#define KP_E1A	P2_4
#define KP_E1B	P2_2//INT0_
#define KP_E1C	P3_7//INT3
#define KP_E2A	P0_1
#define KP_E2B	P0_3
#define KP_E2C	P5_7

#define BUZZ_G		P2_5//蜂鸣器驱动栅极
#define BUZZ_PWM	PWM_DATA0//蜂鸣器驱动占空比


#define ANA_MID(i)		Adc_Set_Mid[0 + (i)]
#define ANA_UP(i)		Adc_Set_Mid[2 + (i)]
#define ANA_DOWN(i)		Adc_Set_Mid[4 + (i)]
#define ANA_MAX(i)		Adc_Set_Mid[6 + (i)]
#define ANA_MIN(i)		Adc_Set_Mid[8 + (i)]

//时间监测 测试代码
#define TICK_SOURSE			((TH0 << 8) | TL0)
#define TICK_START(t)		do{ tickStart = (t); }while(0)
#define TICK_END(t)			do{ tickEnd = (t); tickDif = tickEnd - tickStart; }while(0)
#define TICK_END_TH(t, th)	do{ tickEnd = (t) - tickStart; if(tickEnd > th) tickDif = tickEnd; }while(0)
extern UINT16D tickStart, tickEnd, tickDif;

extern uint32_t Systime;//系统时间

extern UINT8C TURN_L90[16];//左旋按键映射矩阵
extern UINT8C TURN_R90[16];//右旋按键映射矩阵
extern UINT16D TONE_TIMValue;//声调定时器计数值
extern UINT16D buzzTimVol;//声调定时器延时值
extern uint8_t cmdBuzzTone, cmdBuzzVol; // 命令音符 命令音量

extern UINT8D EC1val, EC2val;
extern bit EC1freq, EC2freq;

extern uint16_t adcValue[];
extern uint16_t Adc_Set_Mid[];
extern UINT8D keyFltNum;//按键滤波参数

extern uint8_t keyNow[KP_NUM];//按键映射结果
extern uint8_t keyOld[KP_NUM];//按键映射结果旧值
//extern uint8_t keyFlt[KP_NUM];
extern uint8_t debugBuf[];


void ArrayInit(void);//数组初始化
void AdcRead(void);//摇杆读取
void KeyRead(void);//按键读取
void KeyFilter(uint8_t ts);//按键滤波

void GetTime(void);//时间获取

void BuzzHandle(void);//蜂鸣器处理

void MultiFunc(void);//功能集合函数


#endif




