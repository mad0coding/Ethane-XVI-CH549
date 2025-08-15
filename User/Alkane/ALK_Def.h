#ifndef _ALK_DEF_H_
#define _ALK_DEF_H_


#define ALK_CFG_NUM			8 // 配置个数

#define ALK_KEY_NUM			16
#define ALK_RK_NUM			1
#define ALK_EC_NUM			2

#define ALK_BUTTON_NUM		(ALK_KEY_NUM + ALK_RK_NUM + ALK_EC_NUM) // 广义按钮个数

//******************** Report length ********************//
#define ALK_RPT_L_KEYBRD		22-0	// 键盘报文长度
#define ALK_RPT_L_MOUSE			5	// 鼠标报文长度
#define ALK_RPT_L_POINT			7	// 指针报文长度
#define ALK_RPT_L_VOL			2	// 媒体报文长度
#define ALK_RPT_L_DIAL			3	// 轮盘报文长度
//*******************************************************//

//******************** Data type ********************//
// pointer
// #define ALK_PU8C		PUINT8C
// config data type
#define ALK_U8_CFG		UINT8C
#define ALK_U8_LIGHT	UINT8C
// const
#define ALK_U8C			UINT8C
#define ALK_U16C		UINT16C
// unsigned
#define ALK_U8			uint8_t
#define ALK_U16			uint16_t
#define ALK_U32			uint32_t
// signed
#define ALK_S8			int8_t
#define ALK_S16			int16_t
#define ALK_S32			int32_t
// some platform support fast type
#define ALK_U8F			UINT8D
#define ALK_U16F		UINT16D
#define ALK_S16F		INT16D
// #define ALK_U8F			uint8_t
// #define ALK_U16F		uint16_t
// #define ALK_S16F		int16_t
//***************************************************//





//靠前60k(0x0000~0xEFFF)为CodeFlash 寿命10k次
//靠后1k(0xF000~0xF3FF)为DataFlash 寿命约能100k次
//最后3k(0xF400~0xFFFF)为BootLoader和ConfigInfo 用户不可使用
#define DATA_CFG_BASE		0xF200//配置数据起始
#define DATA_LIGHT_BASE		0xDC00//灯效数据起始
#define DATA_GLOB_BASE		0xDA00//全局数据起始
#define DATA_DIAG_BASE		0xD800//诊断数据起始
//配置数据占CodeFlash的后3k和DataFlash的1k,每套512B,空间8套,目前使用8套,从后往前排
//灯效数据占CodeFlash的倒数第4~5k,每套256B,空间8套,目前使用8套,从前往后排
//全局数据占CodeFlash的倒数第6k的后一半的开头部分,目前使用64B
//诊断数据占CodeFlash的倒数第6k的前一半的开头部分,目前使用64B
//目前全部存储数据使用7k空间,剩余代码空间为54k



#define ALK_Systime		Systime


#endif

