#ifndef _ALK_FUNC_H_
#define _ALK_FUNC_H_

#include "ALK_Def.h"

#include "CH549.H"
#include "DEBUG.H"

#include "FlashRom.H"

#include "BasicIO.h"
#include "ParaConfig.h"


#define m0_none			0
#define m1_button		1
#define m2_shortcut		2
#define m3_group		3
#define m4_move			4
#define m5_press		5
#define m6_change		6
#define m7_clicker		7
#define m8_buzz			8
#define m9_morse		9


#define kv_orig_1		30
#define kv_orig_2		31
#define kv_orig_3		32
#define kv_orig_4		33
#define kv_orig_5		34
#define kv_orig_6		35
#define kv_orig_7		36
#define kv_orig_8		37
#define kv_orig_9		38
#define kv_orig_0		39

//#define kv_space		44

#define kv_num_1		89
#define kv_num_2		90
#define kv_num_3		91
#define kv_num_4		92
#define kv_num_5		93
#define kv_num_6		94
#define kv_num_7		95
#define kv_num_8		96
#define kv_num_9		97
#define kv_num_0		98

#define kv_report		234
#define kv_loop			235
#define kv_move			236
#define kv_press		237
#define kv_vol_next		238
#define kv_vol_prev		239
#define kv_wheel_up		240
#define kv_wheel_down	241
#define kv_vol_up		242
#define kv_vol_down		243
#define kv_vol_mute		244
#define kv_vol_stop		245
#define kv_mouse_l		246
#define kv_mouse_m		247
#define kv_mouse_r		248
#define kv_ctrl			249
#define kv_shift		250
#define kv_alt			251
#define kv_win			252
#define kv_shortcut		253
#define kv_delay		254

#define LIMIT(_X, _min, _max)   (MIN(MAX(_X, _min), _max))
#define MIN(_X, _Y)		        ((_X) < (_Y) ? (_X) : (_Y))
#define MAX(_X, _Y)		        ((_X) > (_Y) ? (_X) : (_Y))
#define ABS(_X)			        ((_X) > 0 ? (_X) : (-(_X)))
#define SIGN(_X)		        (((_X)<0) ? (-1) : (((_X)>0) ? 1 : 0))


ALK_U8 FillReport(void); // 报文填写

void CsChange(ALK_U8 change, ALK_U8 ifTmp); // 切换
void KeyInsert(ALK_U8 r_i, ALK_U8 key_v); // 单键填入
void Mode3Handle(void); // mode3处理
void RkEcKeyHandle(void); // 摇杆旋钮按键处理
void RkHandle(ALK_U8 clear); // 摇杆处理
void EcHandle(ALK_U8 clear); // 旋钮处理
void MorseHandle(void); // 摩尔斯码处理

//void Default_para(void);//载入默认参数



#endif


