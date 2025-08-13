#ifndef _ALK_FUNC_H_
#define _ALK_FUNC_H_

#include "ALK_Def.h"

#include "ParaConfig.h"

#define ALK_RPT_L_KEYBRD		22-0	// 键盘报文长度
#define ALK_RPT_L_MOUSE			5	// 鼠标报文长度
#define ALK_RPT_L_POINT			7	// 指针报文长度
#define ALK_RPT_L_VOL			2	// 媒体报文长度
#define ALK_RPT_L_DIAL			3	// 轮盘报文长度


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


extern ALK_U8 KeyBrd_data[ALK_RPT_L_KEYBRD];	// 键盘报文
extern ALK_U8 Mouse_data[ALK_RPT_L_MOUSE];		// 鼠标报文
extern ALK_U8 Point_data[ALK_RPT_L_POINT];		// 指针报文
extern ALK_U8 Vol_data[ALK_RPT_L_VOL];			// 音量报文
extern ALK_U8 Dial_data[ALK_RPT_L_DIAL];		// 轮盘报文

extern ALK_U8 KeyBrd_if_send;	// 键盘报文是否发送
extern ALK_U8 Vol_if_send;		// 音量报文是否发送
extern ALK_U8 Point_if_send;	// 指针报文是否发送
extern ALK_U8 Mouse_if_send;	// 鼠标报文是否发送
extern ALK_U8 Dial_if_send;		// 轮盘报文是否发送

extern ALK_U8 morse_key;	// 摩尔斯码按键
extern ALK_U8 morse_vol;	// 摩尔斯码音量

extern ALK_U8 clickerNum;	// 自动连点数
extern ALK_U8 mode3_key;	// 按键组按键(1-ALK_KEY_NUM)

extern ALK_U32 changeTime;	// 配置切换时间


ALK_U8 FillReport(void); // 报文填写

void CsChange(ALK_U8 change, ALK_U8 ifTmp); // 切换



#endif


