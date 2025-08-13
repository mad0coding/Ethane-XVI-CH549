#ifndef _RGBDRV_H_
#define _RGBDRV_H_

#include "CH549.H"
#include "DEBUG.H"
#include <intrins.h>
#include <stdlib.h>

#include "ALK_Rgb.h"

// NOP延时
#define WS_NOP_BIT0		_nop_();
#define WS_NOP_BIT1		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();

#define WS_DOUT		P2_7
#define PWM_R		PWM_DATA5
#define PWM_G		PWM_DATA2
#define PWM_B		PWM_DATA4


extern ALK_U8 FrameRaw[ALK_KEY_NUM*3];//原始帧缓存
extern UINT8I FrameBuf[ALK_KEY_NUM*3];//帧缓存


void ClearKeyRGB(void); // 清除键盘RGB
void RgbMap(void); // 键盘RGB映射
void WsWrite16(void); // 写入16个灯



#endif

