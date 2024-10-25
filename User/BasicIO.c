
#include "BasicIO.h"

extern uint8_t KeyBrd_data[];//键盘报文

UINT8C TURN_L90[16] = {	3,	7,	11,	15,
						2,	6,	10,	14,
						1,	5,	9,	13,
						0,	4,	8,	12};//左旋按键映射矩阵
UINT8C TURN_R90[16] = {	12,	8,	4,	0,
						13,	9,	5,	1,
						14,	10,	6,	2,
						15,	11,	7,	3};//右旋按键映射矩阵

uint32_t Systime = 0;//系统时间

uint8_t debugBuf[64];

uint16_t adcValue[2] = {2048,2048};
uint16_t Adc_Mid_Set[2] = {2048,2048};
UINT8D keyFltNum = 2;//按键滤波参数

static uint8_t keyRaw[KP_NUM] _at_ XBASE_KEY_RAW;//按键原始采样
static uint8_t keyFlt[KP_NUM] _at_ XBASE_KEY_FLT;//按键滤波结果
static uint8_t fltOld[KP_NUM] _at_ XBASE_FLT_OLD;//按键滤波用的旧值
static uint8_t fltCount[KP_NUM] _at_ XBASE_FLT_CNT;//按键滤波计数

uint8_t keyNow[KP_NUM] _at_ XBASE_KEY_NOW;//按键映射结果
uint8_t keyOld[KP_NUM] _at_ XBASE_KEY_OLD;//按键映射结果旧值

void ArrayInit(void){//数组初始化
	adcValue[0] = adcValue[1] = Adc_Mid_Set[0] = Adc_Mid_Set[1] = 2048;
	//srand(*(PUINT16X)(2048 - 2));//填入种子
	memset(KeyBrd_data + 1, 0, 21);//初始化键盘报文数组
	memset(debugBuf, 0, 64);
	memset(keyFlt, 0, KP_NUM);
	memset(fltOld, 0, KP_NUM);
	memset(fltCount, 0, KP_NUM);
	memset(keyNow, 0, KP_NUM);
	memset(keyOld, 0, KP_NUM);
	
	//读芯片唯一ID 并填入USB序列号字符串
//	*(PUINT32X)(MySrNumInfo + 18) = FlashReadOTPword(0x14);//17H,16H,15H,14H 高两字节为校验 不使用
//	*(PUINT32X)(MySrNumInfo + 22) = FlashReadOTPword(0x10);//13H,12H,11H,10H
	MySrNumU16[0] = FlashReadOTPword(0x14);//17H,16H,15H,14H 高两字节为校验 不使用
	*(uint32_t*)&MySrNumU16[1] = FlashReadOTPword(0x10);//13H,12H,11H,10H
	
//	sprintf(MySrNumInfo + 2, "%04x%04x%04x", *(PUINT16X)(MySrNumInfo + 20), 
//				*(PUINT16X)(MySrNumInfo + 22), *(PUINT16X)(MySrNumInfo + 24));//把ID打印为12个连续字节
	sprintf(MySrNumInfo + 2, "%04x%04x%04x", MySrNumU16[0], MySrNumU16[1], MySrNumU16[2]);//把ID打印为12个连续字节
	
	for(MySrNumInfo[0] = 0; MySrNumInfo[0] < 12; MySrNumInfo[0]++){//借用MySrNumInfo[0]计数
		MySrNumInfo[24 - 2 * MySrNumInfo[0]] = MySrNumInfo[12 + 2 - 1 - MySrNumInfo[0]];//把连续字节间隔放置
		MySrNumInfo[25 - 2 * MySrNumInfo[0]] = 0;//以0间隔
	}
	MySrNumInfo[0] = 26; MySrNumInfo[1] = 0x03;//长度 类型
}

void AdcRead(void){//摇杆读取
	static uint8_t ch = 1;	//0或1 代表通道0或2
	if(ADC_CTRL & bADC_IF){		//若ADC采样完成
		ADC_CTRL = bADC_IF;			//清标志
		//adcValue[!ch] = ADC_DAT;	//记录采样值
		//adcValue[!ch] = (ADC_DAT + adcValue[!ch]) >> 1;	//记录采样值 有滤波
		adcValue[ch] += ((int16_t)ADC_DAT - (int16_t)adcValue[ch]) >> 2;	//记录采样值 有滤波
		ch = !ch;					//切换选择
		ADC_ExChannelSelect(ch*2);	//选择通道ch 0或2
		ADC_StartSample();			//启动下一次ADC采样
	}
}

void KeyRead(void){//按键读取
	keyRaw[0] = !KP_1;		keyRaw[1] = !KP_2;		keyRaw[2] = !KP_3;		keyRaw[3] = !KP_4;
	keyRaw[4] = !KP_5;		keyRaw[5] = !KP_6;		keyRaw[6] = !KP_7;		keyRaw[7] = !KP_8;
	keyRaw[8] = !KP_9;		keyRaw[9] = !KP_10;		keyRaw[10] = !KP_11;	keyRaw[11] = !KP_12;
	keyRaw[12] = !KP_13;	keyRaw[13] = !KP_14;	keyRaw[14] = !KP_15;	keyRaw[15] = !KP_16;
	keyRaw[16] = !KP_E1;	keyRaw[17] = !KP_E2;
	keyRaw[18] = !KP_R;
}

void KeyFilter(uint8_t ts){//按键滤波
	uint8_t i;
	for(i = 0; i < KP_NUM; i++){
		if(ts == 2){//若为滤波二阶段
			if(fltCount[i]) fltCount[i]--;//若滤波计数未归零则递减
			if(fltOld[i] == keyRaw[i] && keyFlt[i] != keyRaw[i]
					&& fltCount[i] == 0){//两次相同且与之前不同且滤波计数已归零
				keyFlt[i] = keyRaw[i];//更新滤波后的值
				fltCount[i] = keyFltNum;//设置滤波计数值
			}
		}
		fltOld[i] = keyRaw[i];//更新滤波旧值
	}
}

void KeyTurn(void){//按键旋转映射
	uint8_t i;
	memcpy(keyOld, keyNow, KP_NUM);//存储旧值
	
	if(CFG_KB_DIR == 0){//正常方向
		memcpy(keyNow, keyFlt, 16);
	}
	else if(CFG_KB_DIR == 1){//右旋90度
		for(i = 0; i < 16; i++) keyNow[i] = keyFlt[TURN_R90[i]];
	}
	else if(CFG_KB_DIR == 2){//旋转180度
		for(i = 0; i < 16; i++) keyNow[i] = keyFlt[16 - i];
	}
	else if(CFG_KB_DIR == 3){//左旋90度
		for(i = 0; i < 16; i++) keyNow[i] = keyFlt[TURN_L90[i]];
	}
	memcpy(keyNow + 16, keyFlt + 16, 3);//摇杆旋钮按键直接拷贝
}

void GetTime(void){//时间获取
	static UINT16D THTL0_old = 0;//计时器旧值
	UINT16D THTL0;//计时器16位计数值
	UINT8D incMs;//增加的毫秒数
	THTL0 = (TH0 << 8) | TL0;//读取计时器
	if((THTL0 & 0x0F) < 6){//若疑似发生进位
		TR0 = STOP;//暂停定时器
		THTL0 = (TH0 << 8) | TL0;//重新读取计时器
		TR0 = START;//启动定时器
	}
	incMs = (uint16_t)(THTL0 - THTL0_old) >> 11 /*2000*/;//计算增加的毫秒数
//	incMs = (uint16_t)(THTL0 - THTL0_old) / 2000;//计算增加的毫秒数
	Systime += incMs;//系统时间更新
	THTL0_old += (uint16_t)incMs * 2000;//计时器旧值跟进
	//测试结果: /2000*2000:8960  /2000<<11:9174  >>11<<11:9177  >>11*2000:8952
}

UINT16C TONE_TIM[] = {//声调定时器计数值表
	15296,14421,13611,12864,12139,11456,10816,10197,9621,9088,8576,8107,
	7637,7211,6805,6421,6059,5717,	5397,5099,4821,4544,4288,4053,
	3819,3605,3413,3221,3029,2859,2709,2560,2411,2283,2155,2027,
	1920,1813,1707,1600,1515,1429,1344,1280,1195,1131,1067,1003,
};
UINT8C TONE_PWM[] = {//声调PWM分频值表
	253,239,226,213,201,190,
	179,169,160,151,142,134,127,120,113,107,101,95,
	90,85,80,75,71,67,63,60,56,53,50,47,	45,
};
UINT8C TONE_KEY[] = {29,31,33,35,255,24,26,28, 17,19,21,23,255,12,14,16,};//键位音符映射表

UINT16D buzzTimVol = 10, TONE_TIMValue = 10000;//声调定时器计数值,延时值

void buzzHandle(void){//蜂鸣器处理
	uint8_t i;//循环变量
	uint8_t count = 0, effective = 0xFF;//按下按键计数,有效按键
	uint8_t buzzTone = 0xFF, buzzToneOld = 0xFF;//音符
	int8_t buzzVol = 1;//音量
	
	PWM0OutPolarLowAct();//PWM0反极性
	
	while((!keyOld[18] || keyNow[18]) && (!keyOld[17] || keyNow[17]) && (!keyOld[16] || keyNow[16])){//摇杆或旋钮的释放沿退出
		WDOG_COUNT = 0;//清零看门狗计数
		KeyRead();//读取按键
		KeyFilter(1);//滤波一阶段
		KeyRead();//再次读取按键
		KeyFilter(2);//滤波二阶段
		KeyTurn();//按键旋转映射
		
		buzzVol += EC1val;//编码器1调节音量
		EC1val = EC2val = 0;
		if(buzzVol < 1){//最小音量
			PWM_R = 255;//红灯指示
			buzzVol = 1;
			mDelaymS(2);
			PWM_R = 0;
		}
		else if(buzzVol > 10){//最大音量
			PWM_G = 255;//绿灯指示
			buzzVol = 10;
			mDelaymS(2);
			PWM_G = 0;
		}
		
		count = 0;
		for(i = 0; i < 16; i++){
			if(i == 4 || i == 12) continue;//跳过跳八度键和升半音键
			if(keyNow[i] && !keyOld[i]) effective = i;//按下沿开始发音
			if(keyNow[i]) count++;//按下按键计数
		}
		if(count == 0) effective = 0xFF;//全部发音键释放后空置有效按键
		if(effective != 0xFF){//若有有效按键
			if(!keyNow[effective]){//若此键已被释放
				for(i = 0; i < 16; i++){//重新扫描
					if(i == 4 || i == 12) continue;//跳过跳八度键和升半音键
					if(keyNow[i]) effective = i;//按下即发音
				}
			}
			buzzTone = TONE_KEY[effective] + keyNow[12];//从查找表并结合黑键计算音符
			if(keyNow[4] && effective < 8) buzzTone += 12;//上半部升八度
			else if(keyNow[4]) buzzTone -= 12;//下半部降八度
		}
		else buzzTone = 0xFF;//清空音符
		if(buzzToneOld == buzzTone) continue;//若音符未改变则跳过
		if(buzzTone < 18){//低频由定时器实现
			TONE_TIMValue = TONE_TIM[buzzTone];
			buzzTimVol = (((uint32_t)TONE_TIMValue * buzzVol) >> 8) / 3;
			PWM_SEL_CHANNEL(PWM_CH0, Disable);//PWM0输出失能
			ET0 = 1;//定时器0中断使能
		}
		else if(buzzTone <= 48){//高频由PWM实现
			SetPWMClkDiv(TONE_PWM[buzzTone - 18]);//PWM时钟分频配置
			BUZZ_PWM = buzzVol;
			ET0 = 0;//定时器0中断失能
			PWM_SEL_CHANNEL(PWM_CH0, Enable);//PWM0输出使能
		}
		else{//停止发音
			ET0 = BUZZ_PWM = 0;//关定时器中断及清零PWM占空比
			PWM_SEL_CHANNEL(PWM_CH0, Disable);//PWM0输出失能
		}
		buzzToneOld = buzzTone;//记录音符
	}
	ET0 = BUZZ_PWM = 0;//关定时器中断及清零PWM占空比
	PWM_SEL_CHANNEL(PWM_CH0, Disable);//PWM0输出失能
	SetPWMClkDiv(32);//恢复原PWM时钟分频
}

//uint8_t keyOldTest[] = {1,1,1};
uint32_t oldTime = 0;
uint16_t sCount = 0, fCount = 0;

void LL_test(void){
	static uint16_t i;
	i++;
	if(Systime - oldTime >= 1000){//端点2打印输出
		oldTime += 1000;
//		memset(debugBuf, ' ', 64);
//		sprintf(debugBuf, "%d	%d	%u\n", sCount, fCount, (uint16_t)Systime);
		
		Enp2IntIn(debugBuf, 64);
	}

//	mDelaymS(10);
}

void MultiFunc(void){//功能集合函数
//	LL_test();//测试代码
	KeyTurn();//按键旋转映射
	
	if(FillReport() == 1){//报文填写 若返回蜂鸣器模式
		ClearKeyRGB();//清除键盘RGB
		WsWrite16();//灯写入
		buzzHandle();//蜂鸣器处理
	}
	
	SysRGB();//系统RGB控制
	KeyRGB(0);//键盘RGB控制
//	if(WakeUpEnFlag & 1) PWM_R = 100;
//	else PWM_R = 0;
//	if(WakeUpEnFlag & 2) PWM_G = 100;
//	else PWM_G = 0;
}













