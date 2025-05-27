/*******************************************************************************
* File Name          : main.c
* Author             : Light&Electricity
* Version            : V1.5
* Date               : 2025/5/3
* Description        : Ethane-XVI
********************************************************************************/
#include "CH549.H"
#include "DEBUG.H"
#include "GPIO.H"
#include "ADC.H"
#include "PWM.H"
#include "Timer.H"
#include "CompositeHID.H"
#include "BasicIO.h"

#pragma  NOAREGS

UINT8C FIRMWARE_VERSION[4] = {1,5,3,0}; // 固件版本

uint8_t asyncFlag = 0;//异步操作标志

uint8_t reportSendTime = 0;//报文发送时间
uint8_t All_if_send = 0;//总发送标志
uint8_t KeyBrd_if_send = 0;//键盘报文是否发送
uint8_t Mouse_if_send = 0;//鼠标报文是否发送
uint8_t Point_if_send = 0;//指针报文是否发送
uint8_t Vol_if_send = 0;//媒体报文是否发送
uint8_t Dial_if_send = 0;//轮盘报文是否发送

uint8_t KeyBrd_data[KB_LEN] = {1,0,0,0};//编号1,功能键,保留0,其他按键
//功能键:bit7-bit0分别为为右win alt shift ctrl,左win alt shift ctrl

uint8_t Mouse_data[5] = {2,0,0,0,0};//编号2,功能键,x,y,滚轮
//功能键:bit0为左键,bit1为右键,bit2为中键,bit6为x是否溢出,bit7为y是否溢出

uint8_t Point_data[7] = {3,0x10,1,0,0,0,0};//编号3,功能键,id,x_L,x_H,y_L,y_H
//功能键:bit0为Tip Switch,bit1为Barrel Switch,bit2为Invert,bit3为Eraser Switch,bit4为In Range

uint8_t Vol_data[2] = {4,0};//编号4,功能键
//功能键:bit0音量加,bit1音量减,bit2静音,bit3播放暂停

uint8_t Dial_data[3] = {5,0,0};//编号5,功能键+dial_L,dial_H
//功能键:bit0按键,bit1~7为轮盘低7bit


void main()
{
	P2_0 = P2_1 = P2_3 = 0;			//RGB灯GPIO置低
    CfgFsys( );						//CH549时钟选择配置
    mDelaymS(5);					//修改主频等待内部晶振稳定,必加
//    mInitSTDIO( );					//串口0初始化
	if(!(PCON & bRST_FLAG0)) mDelaymS(50);	//若为软复位或看门狗复位 则额外追加延时
	
	ArrayInit();	//数组初始化
	DiagInit();		//诊断初始化处理
	
    USBDeviceInit();				//USB设备模式初始化
    EA = 1;							//总中断允许

	WS_DOUT = 0;					//WS2812数据线置低
	GPIO_Init(PORT2, PIN7, MODE1);	//WS2812数据线初始化为推挽
	
	GPIO_Init(PORT2, PIN0, MODE1);	//初始化为推挽
	GPIO_Init(PORT2, PIN1, MODE1);	//初始化为推挽
	GPIO_Init(PORT2, PIN3, MODE1);	//初始化为推挽
	
	SetPWMClkDiv(32);			//PWM时钟分频配置,FREQ_SYS/32 = 768kHz
    SetPWMCycle256Clk();		//PWM周期,FREQ_SYS/32/256 = 3kHz
	SetPWM2Dat(0);				//LEDG初始占空比配置
    SetPWM4Dat(0);				//LEDB初始占空比配置
	SetPWM5Dat(0);				//LEDR初始占空比配置
	PWM_SEL_CHANNEL(PWM_CH2, Enable);		//启动通道2输出使能
	PWM_SEL_CHANNEL(PWM_CH4, Enable);		//启动通道4输出使能
	PWM_SEL_CHANNEL(PWM_CH5, Enable);		//启动通道5输出使能
	
	ADC_ExInit(0);					//ADC初始化,选择最慢采样时钟
	ADC_ChSelect(0);				//初始化通道0并选择
	ADC_ChSelect(2);				//初始化通道2并选择
	ADC_StartSample();				//启动采样
	
//	GPIO_INT_Init((INT_P03_L|INT_P57_H|INT_INT0_L), INT_EDGE, Enable); //使能3个中断
//	PIN_FUNC |= bINT0_PIN_X;		//INT0使用P22
//	GPIO_Init(PORT3, PIN7, MODE0);	//初始化为高阻
//	
//	INTX |= bIX3 | bIT3;	//INT3选择高电平和边沿触发(即上升沿触发)
//	IE_INT3 = 1;			//允许INT3中断
	
	PWM0OutPolarLowAct();				//PWM0反极性
	SetPWMClkDiv(32);					//设置频率
	PWM_SEL_CHANNEL(PWM_CH0, Enable);	//PWM0输出使能
	BUZZ_PWM = 0;						//占空比为0
	
	mTimer0Clk12DivFsys();			//T0定时器时钟设置 FREQ_SYS/12
    mTimer_x_ModInit(0, 1);			//T0定时器模式设置 模式1 16位定时器
    mTimer_x_SetData(0, 0);			//T0定时器赋值 一直16位循环 不使用中断
    mTimer0RunCTL(1);				//T0定时器启动
//    ET0 = 1;						//T0定时器中断开启

//	CH549WDTModeSelect(1);		//启动看门狗
	
	ParaLoad();		//参数读取
	
    while(1){
//		P2_0 = !KP_E2A;
//		P2_1 = !KP_E2B;
//		P2_3 = !KP_E2C;
		PWM_R = 30 * !KP_E2A;
		PWM_G = 10 * !KP_E2B;
		PWM_B = 10 * !KP_E2C;
//		WDOG_COUNT = 0;//清零看门狗计数
////		if(!KP_E2) WDOG_COUNT = 0xFF;//按下旋钮2则触发看门狗 测试代码！！！！！！！！！！
//		/********************基本IO********************/
//		GetTime();//时间获取
//		
//		KeyRead();//读取按键
//		KeyFilter(1);//滤波一阶段
//		AdcRead();//摇杆ADC读取一个通道
//		
//        WsWrite16();//灯写入
//		
//		AdcRead();//摇杆ADC读取另一个通道
//		KeyRead();//再次读取按键
//		KeyFilter(2);//滤波二阶段
//		/**********************************************/
//		
//		if(asyncFlag & 0x80) continue;//若USB正在接收数据则跳过HID发送
//		else if(asyncFlag){//有需要的异步操作
//			AsyncHandle(asyncFlag);//异步处理
//			asyncFlag = 0;//清除标志
//			continue;
//		}
//		
//		if(All_if_send == 0){ // 若总发送标志已清空
//			MultiFunc(); // 处理各种功能
//			
//			All_if_send = KeyBrd_if_send | (Mouse_if_send << 1) | (Point_if_send << 2) | (Vol_if_send << 3) 
//						| (Dial_if_send << 4); // 生成总发送标志
//		}
//		
//		if((uint8_t)((uint8_t)Systime - reportSendTime) < 8) continue;	//延时未到则跳过发送
//		if(All_if_send != 0) reportSendTime = Systime;	//若有要发送的则记录发送时间低8位
//		
//		if(All_if_send && WakeUpEnFlag == 0x03){//若要发送且主机已休眠
//			CH554USBDevWakeup();//唤醒
//			mDelaymS(50);
//			CH554USBDevWakeup();//唤醒
//			mDelaymS(50);
//		}
//		
//		if(All_if_send & 0x01){		//键盘
//			All_if_send &= ~0x01;	//清除bit0
//			Enp1IntIn(KeyBrd_data, sizeof(KeyBrd_data));
//		}
//		else if(All_if_send & 0x02){//鼠标
//			All_if_send &= ~0x02;	//清除bit1
//			Enp1IntIn(Mouse_data, sizeof(Mouse_data));
//		}
//		else if(All_if_send & 0x04){//指针
//			All_if_send &= ~0x04;	//清除bit2
//			Enp1IntIn(Point_data, sizeof(Point_data));
//		}
//		else if(All_if_send & 0x08){//媒体
//			All_if_send &= ~0x08;	//清除bit3
//			Enp1IntIn(Vol_data, sizeof(Vol_data));
//		}
//		else if(All_if_send & 0x10){//轮盘
//			All_if_send &= ~0x10;	//清除bit4
//			Enp1IntIn(Dial_data, sizeof(Dial_data));
//		}
    }
}







