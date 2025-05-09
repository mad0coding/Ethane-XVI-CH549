/********************************** (C) COPYRIGHT *******************************
* File Name          : DEBUG.H
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH5XX DEBUG Interface
                     (1)、主频设置;
                     (2)、us\ms基本延时函数;										 
                     (3)、串口0输出打印信息，波特率可调;            				   
                     (4)、看门狗初始化和赋值函数;     										 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef	__DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <string.h>

//定义函数返回值
#ifndef  SUCCESS
#define  SUCCESS  1
#endif
#ifndef  FAIL
#define  FAIL    0
#endif

//定义定时器起始
#ifndef  START
#define  START  1
#endif
#ifndef  STOP
#define  STOP    0
#endif

//#ifndef  DE_PRINTF                       //调试开关
//#define  DE_PRINTF     1
//#endif

#define	 FREQ_SYS			24000000	//系统主频24MHz。注: 48MHz仅对定制芯片使用
#define  OSC_EN_XT			0			//外部晶振使能，默认开启内部晶振
#define  MCU_WORK_AT_3V3    0			//MCU工作电压：1-3.3V;0-5V. 注意: 请根据MCU实际工作电压进行设置

#ifndef  UART0BUAD
#define  UART0BUAD          115200
#endif

//#ifndef MIN
//#define MIN(a,b)                (((a) <= (b)) ? (a) : (b))
//#endif


//******************************自定义类型别名******************************//
#define u8d UINT8D//100/29.9us
#define u8i UINT8I//100/29.9us
#define u8p UINT8P//100/25.8us
#define u8x UINT8X//100/51.2us

#define uint8_t		UINT8
#define uint16_t	UINT16
#define uint32_t	UINT32
#define int8_t		char
#define int16_t		short
#define int32_t		long

#ifndef INT8D
typedef char  data             INT8D;
#endif
#ifndef INT16D
typedef short data             INT16D;
#endif
#ifndef INT32D
typedef long  data             INT32D;
#endif
#ifndef UINT8P
typedef unsigned char  pdata            UINT8P;
#endif
#ifndef UINT16P
typedef unsigned short pdata            UINT16P;
#endif
#ifndef UINT32P
typedef unsigned long  pdata            UINT32P;
#endif
#ifndef UINT16C
typedef const unsigned short code        UINT16C;
#endif
#ifndef PUINT16C
typedef const unsigned short code      *PUINT16C;
#endif
#ifndef PUINT32C
typedef const unsigned long code      *PUINT32C;
#endif
//**************************************************************************//


void CfgFsys( );                        // CH549时钟选择和配置
void mDelayuS( UINT16 n );              // 以uS为单位延时
void mDelaymS( UINT16 n );              // 以mS为单位延时
void CH559UART0Alter();                 // CH549串口0引脚映射到P0.2/P0.3
void mInitSTDIO( );                     // T1作为波特率发生器
void CH549SoftReset( );                 // CH549软复位
void CH549WDTModeSelect(UINT8 mode);	// 看门狗模式选择
void CH549WDTFeed(UINT8 tim);           // 喂狗
	
#endif

