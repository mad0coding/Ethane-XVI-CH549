#ifndef _PARAUSE_H_
#define _PARAUSE_H_

#include "CH549.H"
#include "DEBUG.H"

#include "FlashRom.H"

#include "BasicIO.h"
#include "ParaConfig.h"

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

#define kv_point		237
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

#define LIMIT(x, min, max)	(MIN(MAX(x, min), max))
#define MIN(X, Y)	((X) < (Y) ? (X) : (Y))
#define MAX(X, Y)	((X) > (Y) ? (X) : (Y))
#define ABS(X)		((X) > 0 ? (X) : (-(X)))
#define SIGN(X)		(((X)<0) ? (-1) : (((X)>0) ? 1 : 0))


uint8_t Fill_report(void);//报文填写

void cs_change(uint8_t change);//切换
void key_insert(uint8_t r_i, uint8_t key_v);//单键填入
void mode3_handle(void);//mode3处理
void RK_EC_key_handle(void);//摇杆旋钮按键处理
void RK_handle(uint8_t clear);//摇杆处理
void EC_handle(uint8_t clear);//旋钮处理


//void Default_para(void);//载入默认参数



#endif


