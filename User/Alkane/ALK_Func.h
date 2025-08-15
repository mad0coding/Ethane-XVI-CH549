#ifndef _ALK_FUNC_H_
#define _ALK_FUNC_H_

#include "ALK_Def.h"

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



#define CFG_DATA_SIZE		512 // 键盘配置数据长度

#define CFG_ADDR_CSC(i)		(DATA_CFG_BASE - (i) * CFG_DATA_SIZE)//配置数据地址 逆序
#define CFG_DATA_CSC(i)		(DATA_CFG = CFG_ADDR_CSC(i))//配置数据选择

//键盘配置
#define CFG_THIS		(DATA_CFG)//键盘配置起始
#define CFG_ACS(a)			(*((PUINT8C)(a) + 0))//按键直接访问
#define CFG_K_ID(a)			(*((PUINT8C)(a) + 0))//按键ID
#define CFG_K_MODE(a)		(*((PUINT8C)(a) + 1))//按键模式
#define CFG_K_KEY(a)		(*((PUINT8C)(a) + 2))//按键键值
#define CFG_K_LEN(a)		(*((PUINT8C)(a) + 2))//按键组模式时按键长度
#define CFG_K_MORSE(a)		(*((PUINT8C)(a) + 2))//摩尔斯码设置
#define CFG_K_MTGAP(a)		(*((PUINT8C)(a) + 3))//摩尔斯码tGap
#define CFG_K_FUNC(a)		(*((PUINT8C)(a) + 3))//按键功能键
#define CFG_K_M3DT(a)		(*((PUINT8C)(a) + 3))//按键组模式时数据起始
#define CFG_K_MTLONG(a)		(*((PUINT8C)(a) + 4))//摩尔斯码tLong
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
#define CFGb_K_MKEY(a)		(CFG_K_MORSE(a) & 0x03)//摩尔斯码按键
#define CFGb_K_MVOL(a)		(CFG_K_MORSE(a) >> 4)//摩尔斯码音量

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



#define LIMIT(_X, _min, _max)   (MIN(MAX(_X, _min), _max))
#define MIN(_X, _Y)             ((_X) < (_Y) ? (_X) : (_Y))
#define MAX(_X, _Y)             ((_X) > (_Y) ? (_X) : (_Y))
#define ABS(_X)                 ((_X) > 0 ? (_X) : (-(_X)))
#define SIGN(_X)                (((_X)<0) ? (-1) : (((_X)>0) ? 1 : 0))


extern ALK_U8_CFG *DATA_CFG; // 闪存区配置信息指针

extern ALK_U8 sysCs; // 总配置选择

extern ALK_U8 keyDir[ALK_CFG_NUM]; // 键盘方向

extern ALK_U8 keyNow[ALK_BUTTON_NUM]; // 应用层按键状态
extern ALK_U8 keyOld[ALK_BUTTON_NUM]; // 应用层按键旧值

extern ALK_U16 keyAddr[ALK_CFG_NUM][ALK_KEY_NUM]; // 每组全部按键的数据地址


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


