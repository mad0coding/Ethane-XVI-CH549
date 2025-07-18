
//全开理论灯功耗576，实测总功耗604，全关实测总功耗18，故得全开灯功耗604-18=590
#include "RgbDrv.h"

uint8_t rgbHidFlag = 0; // RGB通信标志
// bit7:新帧标志 bit6~4:模式 bit3~0:预留

PUINT8C DATA_LIGHT = DATA_LIGHT_BASE; // 闪存区灯效信息指针

static UINT8C LED_CURVE[] = {//LED非线性校正表
	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,
	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  1,  1,  1,   1,  1,  1,  1,
	  1,  1,  1,  1,   1,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  3,   3,  3,  3,  3,
	  3,  3,  3,  4,   4,  4,  4,  4,   4,  4,  4,  5,   5,  5,  5,  5,   5,  6,  6,  6,
	  6,  6,  6,  7,   7,  7,  7,  7,   7,  8,  8,  8,   8,  8,  9,  9,   9,  9,  9,  9,
	 10, 10, 10, 10,  11, 11, 11, 11,  11, 12, 12, 12,  12, 13, 13, 13,  13, 13, 14, 14,
	 14, 14, 15, 15,  15, 15, 16, 16,  16, 16, 17, 17,  17, 18, 18, 18,  18, 19, 19, 19,
	 19, 20, 20, 20,  21, 21, 21, 22,  22, 22, 22, 23,  23, 23, 24, 24,  24, 25, 25, 25,
	 26, 26, 26, 27,  27, 27, 28, 28,  28, 29, 29, 29,  30, 30, 30, 31,  31, 31, 32, 32,
	 33, 33, 33, 34,  34, 34, 35, 35,  36, 36, 36, 37,  37, 37, 38, 38,  39, 39, 39, 40,
	 40, 41, 41, 42,  42, 42, 43, 43,  44, 44, 44, 45,  45, 46, 46, 47,  47, 48, 48, 48,
	 49, 49, 50, 50,  51, 51, 52, 52,  53, 53, 53, 54,  54, 55, 55, 56,  56, 57, 57, 58,
	 58, 59, 59, 60,  60, 61, 61, 62,  62, 63, 63, 64,  64, 65, 65, 66,  66, 67, 67, 68,
	 68, 69, 70, 70,  71, 71, 72, 72,  73, 73, 74, 74,  75, 76, 76, 77,  77, 78, 78, 79,
	 79, 80, 81, 81,  82, 82, 83, 84,  84, 85, 85, 86,  86, 87, 88, 88,  89, 89, 90, 91,
	 91, 92, 93, 93,  94, 94, 95, 96,  96, 97, 98, 98,  99, 99,100,101, 101,102,103,103,
	104,105,105,106, 107,107,108,109, 109,110,111,111, 112,113,113,114, 115,115,116,117,
	117,118,119,120, 120,121,122,122, 123,124,124,125, 126,127,127,128, 129,129,130,131,
	132,132,133,134, 135,135,136,137, 138,138,139,140, 141,141,142,143, 144,144,145,146,
	147,148,148,149, 150,151,151,152, 153,154,155,155, 156,157,158,159, 159,160,161,162,
	163,164,164,165, 166,167,168,168, 169,170,171,172, 173,173,174,175, 176,177,178,179,
	179,180,181,182, 183,184,185,185, 186,187,188,189, 190,191,192,193, 193,194,195,196,
	197,198,199,200, 201,201,202,203, 204,205,206,207, 208,209,210,211, 212,213,213,214,
	215,216,217,218, 219,220,221,222, 223,224,225,226, 227,228,229,230, 231,232,233,234,
	235,235,236,237, 238,239,240,241, 242,243,244,245, 246,247,248,249, 250,251,252,253,
	255,
};
static UINT8C ON_CURVE[] = {//升亮度观感校正表
	  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22,
	 22, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 46, 46, 47,
	 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 75,
	 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,
	107,108,109,110,111,112,113,114,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,136,137,138,139,140,
	141,142,143,144,145,146,148,149,150,151,152,153,154,155,157,158,159,160,161,162,163,164,166,167,168,169,170,171,173,174,175,176,
	177,178,180,181,182,183,184,186,187,188,189,190,192,193,194,195,196,198,199,200,201,203,204,205,206,207,209,210,211,212,214,215,
	216,217,219,220,221,223,224,225,226,228,229,230,231,233,234,235,237,238,239,241,242,243,245,246,247,248,250,251,252,253,254,255,
};
static UINT8C NBHD_T[] = {//十字邻域表
	1,4,255,255,	0,2,5,255,		1,3,6,255,		2,7,255,255,
	0,5,8,255,		1,4,6,9,		2,5,7,10,		3,6,11,255,
	4,9,12,255,		5,8,10,13,		6,9,11,14,		7,10,15,255,
	8,13,255,255,	9,12,14,255,	10,13,15,255,	11,14,255,255,
};
static UINT8C NBHD_X[] = {//X形邻域表
	5,255,255,255,	4,6,255,255,	5,7,255,255,	6,255,255,255,
	1,9,255,255,	0,2,8,10,		1,3,9,11,		2,10,255,255,
	5,13,255,255,	4,6,12,14,		5,7,13,15,		6,14,255,255,
	9,255,255,255,	8,10,255,255,	9,11,255,255,	10,255,255,255,
};
static UINT8C INX_TABLE[] = {//预置灯效下标表
	0,0,0,0,	 1,1,1,1,	 2,2,2,2,	 3,3,3,3,//↓(波动)
	0,1,2,3,	 1,2,3,4,	 2,3,4,5,	 3,4,5,6,//↘
	0,1,2,3,	 0,1,2,3,	 0,1,2,3,	 0,1,2,3,//→
	3,4,5,6,	 2,3,4,5,	 1,2,3,4,	 0,1,2,3,//↗
	3,3,3,3,	 2,2,2,2,	 1,1,1,1,	 0,0,0,0,//↑
	6,5,4,3,	 5,4,3,2,	 4,3,2,1,	 3,2,1,0,//↖
	3,2,1,0,	 3,2,1,0,	 3,2,1,0,	 3,2,1,0,//←
	3,2,1,0,	 4,3,2,1,	 5,4,3,2,	 6,5,4,3,//↙
	0,1,2,3,	 4,5,6,7,	 8,9,10,11,	 12,13,14,15,//→↓(行列)
	0,4,8,12,	 1,5,9,13,	 2,6,10,14,	 3,7,11,15,//↓→
	3,7,11,15,	 2,6,10,14,	 1,5,9,13,	 0,4,8,12,//↑→
	12,13,14,15, 8,9,10,11,	 4,5,6,7,	 0,1,2,3,//→↑
	15,14,13,12, 11,10,9,8,	 7,6,5,4,	 3,2,1,0,//←↑
	15,11,7,3,	 14,10,6,2,	 13,9,5,1,	 12,8,4,0,//↑←
	12,8,4,0,	 13,9,5,1,	 14,10,6,2,	 15,11,7,3,//↓←
	3,2,1,0,	 7,6,5,4,	 11,10,9,8,	 15,14,13,12,//←↓
	0,1,2,3,	 7,6,5,4,	 8,9,10,11,	 15,14,13,12,//→↓(蛇行)
	0,7,8,15,	 1,6,9,14,	 2,5,10,13,	 3,4,11,12,//↓→
	3,4,11,12,	 2,5,10,13,	 1,6,9,14,	 0,7,8,15,//↑→
	15,14,13,12, 8,9,10,11,	 7,6,5,4,	 0,1,2,3,//→↑
	12,13,14,15, 11,10,9,8,	 4,5,6,7,	 3,2,1,0,//←↑
	12,11,4,3,	 13,10,5,2,	 14,9,6,1,	 15,8,7,0,//↑←
	15,8,7,0,	 14,9,6,1,	 13,10,5,2,	 12,11,4,3,//↓←
	3,2,1,0,	 4,5,6,7,	 11,10,9,8,	 12,13,14,15,//←↓
	0,1,2,3,	 11,12,13,4, 10,15,14,5, 9,8,7,6,//→↓(涡旋)
	0,11,10,9,	 1,12,15,8,	 2,13,14,7,	 3,4,5,6,//↓→
	3,4,5,6,	 2,13,14,7,	 1,12,15,8,	 0,11,10,9,//↑→
	9,8,7,6,	 10,15,14,5, 11,12,13,4, 0,1,2,3,//→↑
	6,7,8,9,	 5,14,15,10, 4,13,12,11, 3,2,1,0,//←↑
	6,5,4,3,	 7,14,13,2,	 8,15,12,1,	 9,10,11,0,//↑←
	9,10,11,0,	 8,15,12,1,	 7,14,13,2,	 6,5,4,3,//↓←
	3,2,1,0,	 4,13,12,11, 5,14,15,10, 6,7,8,9,//←↓
};

static uint8_t fracM[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,};//主效果比例
static uint8_t fracUD[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,};//按动效果比例

static uint8_t inXi[16] = {0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15,};

uint8_t FrameRaw[16*3] = {//原始帧缓存
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
};//RGB
static UINT8I FrameBuf[16*3] = {//帧缓存
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
};//GRB

void ClearKeyRGB(void){//清除键盘RGB
	memset(FrameBuf, 0, sizeof(FrameBuf));
}

void KeyRGB(uint8_t clear){ // 键盘RGB控制
	static uint32_t dTime = 0, leaveTime = -65535;//时间记录 离开时间
	static uint16_t randTime = 0;//间隔随机时间
	static uint8_t taskTick = 0;//0~3bit:0为启动沿,1为动作期,2为结束沿,3为间隔期;bit7:呼吸模式用
	static uint8_t eTime = 0, INXi = 0, fracSHLD = 0;//时间记录,自定义下标,屏蔽效果比例
	static uint8_t listMode = -1, lightMode = 0;//在列表选中的模式, 真正的模式
	uint8_t /*lightMode = 0, */ifINX = 0, leftSHLD = 255;//模式,是否有自定义标记,屏蔽剩余
	UINT8D i, j;//按键循环变量,RGB循环变量
	INT16D tool16 = 0;//工具变量
	uint16_t h, s, v;//HSV色值
	uint8_t r, g, b;//RGB色值
	
	if(clear){
		dTime = Systime;//重置时间记录
		leaveTime = Systime - 65535;//重置离开时间
		/*if(LIGHT_MONO <= 0) */memset(fracM, 0, 16);//快切换则清空主效果比例
		/*if(LIGHT_MONO <= 1) */taskTick = 0;//快、缓切换则清零任务节拍
		return;
	}
	
	if(rgbHidFlag & 0x70){ // 非硬件模式
		if(rgbHidFlag & 0x80){ // 有新帧
			rgbHidFlag &= ~0x80; // 清除新帧标志
			// 唯一的goto语句 暂为避免套if或大改逻辑或重新封装函数
			goto MAP_GRB_TO_RGB; // 跳过所有硬件灯效 直接输出帧
		}
		else return; // 无新帧则退出
	}
	
	if((uint8_t)((uint8_t)Systime - eTime) < 20) return;//以20ms周期处理灯效
	eTime += 20;//时间跟进

	if(LIGHT_T_WAIT){//若启用等待时间
		for(i = 0; i < 16; i++){//检查是否有按键
			if(keyNow[i]){//若有按下
				leaveTime = Systime;//不断更新离开时间
				break;
			}
		}
	}
	
	if((taskTick & 0x0F) == 2){//若为结束沿
		taskTick = 3;//进入间隔期
		dTime = Systime;//记录间隔期起始时间
		INXi = 255;
	}
	if((taskTick & 0x0F) == 3 && (uint32_t)(Systime - dTime) > (LIGHT_T_GAP + (uint32_t)randTime)//若为间隔期 且 间隔时间已到
		&& (!LIGHT_T_WAIT || (uint32_t)(Systime - leaveTime) > LIGHT_T_WAIT * 10UL)){//且 无等待或等待时间已到
		randTime = LIGHT_T_RAND ? ((rand() ^ (TL0 << 8)) % LIGHT_T_RAND) : 0;//更新间隔随机时间
		taskTick = 0;//进入启动沿
	}
	if((taskTick & 0x0F) == 0){//若为启动沿
		taskTick = 1;//进入动作期
		dTime = Systime;//记录动作期起始时间
		INXi = 0;
		if(LIGHT_WAVE == 0){//呼吸
			lightMode = 200;
		}
		else if(LIGHT_WAVE <= 4){//波动,行列,蛇行,涡旋
			lightMode = (LIGHT_WAVE - 1) * 8 + LIGHT_DIR;
		}
		else if(LIGHT_WAVE == 5){//自定义
			lightMode = 100;
		}
		else{//循环 或 随机
			for(i = j = 0; i < (LIGHT_COUNT + 1) && i < LIGHT_LIST_MAX; i++){//借用j统计列表总个数
				if(LIGHT_LIST(i) && LIGHT_LIST(i) <= 4) j += 8;
				else j++;
			}
			if(LIGHT_WAVE == 6){//循环
				if(++listMode >= j) listMode = 0;//自增或转一圈回来
			}
			else{//随机
				do{ i = (rand() ^ TL0) % j; }while(i == listMode && j > 1);//借用i存储掺入时间的随机数 直到与现在不同
				listMode = i;
			}
			
			for(i = j = 0; i <= (LIGHT_COUNT + 1) && i <= LIGHT_LIST_MAX; i++){//在列表查找
				if(j > listMode){
					if(LIGHT_LIST(i - 1) && LIGHT_LIST(i - 1) <= 4) j = listMode + 8 - j;//借用j记录方向
					i = LIGHT_LIST(i - 1);//记录类型
					break;
				}
				if(LIGHT_LIST(i) && LIGHT_LIST(i) <= 4) j += 8;
				else j++;
			}
			if(i == 0){//呼吸
				lightMode = 200;
			}
			else if(i <= 4){//波动,行列,蛇行,涡旋
				lightMode = (i - 1) * 8 + j;
			}
			else{//自定义
				lightMode = 100;
			}
		}
	}
	if((taskTick & 0x0F) == 1 && (uint16_t)((uint16_t)Systime - dTime) > LIGHT_T_ACT && lightMode != 200){//若为动作期且动作时间已到且非呼吸模式
		dTime = Systime;//记录新动作起始时间
		INXi++;
	}
	
	if(lightMode < 32) memcpy(inXi, INX_TABLE + 16*lightMode, 16);//32种预置灯效
	else if(lightMode == 100) memcpy(inXi, &LIGHT_IDX(0), 16);//自定义
	
	memcpy(FrameRaw, &LIGHT_UP(0), 16*3);//将上配色载入原始帧缓存
	
	for(i = 0; i < 16; i++){//16键处理开始
		if(LIGHT_COLOR_T && (LIGHT_COLOR_S & 0x01)){//若启用上配色色彩变化
			Rgb2Hsv(FrameRaw[3*i+0], FrameRaw[3*i+1], FrameRaw[3*i+2], &h, &s, &v);
			h += (Systime / LIGHT_COLOR_T) % (COLOR_ANGLE * 6);//加入色环变化
			if(h >= COLOR_ANGLE * 6) h -= COLOR_ANGLE * 6;//防止越界
			Hsv2Rgb(h, s, v, &FrameRaw[3*i+0], &FrameRaw[3*i+1], &FrameRaw[3*i+2]);
		}
		
		//主效果
		if(lightMode == 200 && LIGHT_T_ACT){//若为动作期且为呼吸模式
			v = ((uint16_t)((uint16_t)Systime - dTime) / (MAX(LIGHT_T_ACT,1000) / 1000)) % (COLOR_ANGLE * 6);//用动作时间控制呼吸周期
			if(v >= COLOR_ANGLE * 3){//下降段
				v = COLOR_ANGLE * 6 - 1 - v;//0~500
				tool16 = LIGHT_G_OFF % 3;//用灭渐变字节来选择下降段曲线
				taskTick |= 0x80;//置位最高位
			}else{//上升段
				tool16 = LIGHT_G_ON % 3;//用亮渐变字节来选择上升段曲线
				if(taskTick == 0x81){//若为动作期并第二次进入上升段则代表一次循环已完
					if(i == 16 - 1) taskTick = 2;//清除最高位并进入结束沿(必须在最后一键处理)
					v = 0;//清零亮度
				}
			}
			if((taskTick & 0x0F) != 1) v = 0;//若非动作期则清零亮度
			//加入呼吸效果
			if(tool16 == 0){//纯线性
				for(j = 0; j < 3; j++) FrameRaw[3*i+j] = (uint32_t)FrameRaw[3*i+j] * v / 500;
			}else if(tool16 == 1){//二次函数校正
				v = ON_CURVE[(uint32_t)v * 255 / 500];//获取校正的亮度值(借用v存储)
				for(j = 0; j < 3; j++) FrameRaw[3*i+j] = (uint16_t)FrameRaw[3*i+j] * v / 255;
			}else{//纯平方校正
				v = LED_CURVE[v];//获取校正的亮度值(借用v存储)
				for(j = 0; j < 3; j++) FrameRaw[3*i+j] = (uint16_t)FrameRaw[3*i+j] * v / 255;
			}
			ifINX = 1;//自定义标记置位以防止触发非呼吸模式的结束沿进入代码
		}
		else if(/*(taskTick & 0x0F) == 1 && */lightMode != 200){//若为动作期但为其他模式
			if(inXi[i] == INXi){//上升段
				ifINX = 1;//自定义标记置位
//				tool16 = (255 - fracM[i]) / 50;
//				if((255 - fracM[i]) && tool16 == 0) tool16 = 1;
//				fracM[i] += tool16;
				
				tool16 = 31 - LIGHT_G_ON;
				if(tool16 > 10) tool16 = (tool16 - 10) * 10 + 10;
				if(255 - fracM[i] > tool16) fracM[i] += tool16;//亮渐变
				else fracM[i] = 255;
				for(j = 0; j < 3; j++){//加入升主效果
					FrameRaw[3*i+j] = (uint16_t)FrameRaw[3*i+j] * ON_CURVE[fracM[i]] / 255;
				}
			}else{//下降段
//				tool16 = (fracSHLD - leftSHLD) / (LIGHT_M_OFF + 1);//屏蔽灭渐变
//				if(tool16 == 0) tool16 = 1;
//				fracSHLD -= tool16;
				
				tool16 = fracM[i] / (LIGHT_G_OFF + 1);//灭渐变
				if(fracM[i] && tool16 == 0) tool16 = 1;
				fracM[i] -= tool16;
				for(j = 0; j < 3; j++){//加入降主效果
					FrameRaw[3*i+j] = (uint16_t)FrameRaw[3*i+j] * fracM[i] / 255;
				}
			}
		}
		
		//计算屏蔽效果,获取屏蔽需求,计算按键效果
		if(keyNow[i]){//若按下
			tool16 = (255 - fracUD[i]) / (LIGHT_T_D(i) + 1);//31
			if((255 - fracUD[i]) && tool16 == 0) tool16 = 1;
			fracUD[i] += tool16;
			if(leftSHLD > LIGHT_SHLD(i)) leftSHLD = LIGHT_SHLD(i);//若按下且需压低屏蔽
		}else{//若抬起
			tool16 = fracUD[i] / (LIGHT_T_U(i) + 1);//31
			if(fracUD[i] && tool16 == 0) tool16 = 1;
			fracUD[i] -= tool16;
			for(j = 0; j < 3; j++){//加入屏蔽效果
				FrameRaw[3*i+j] = FrameRaw[3*i+j] * fracSHLD / 255;
			}
		}

		//加入按动效果
		if(LIGHT_COLOR_T && (LIGHT_COLOR_S & 0x02)){//若启用下配色色彩变化
			Rgb2Hsv(LIGHT_DOWN(3*i+0), LIGHT_DOWN(3*i+1), LIGHT_DOWN(3*i+2), &h, &s, &v);
			h += (Systime / LIGHT_COLOR_T) % (COLOR_ANGLE * 6);//加入色环变化
			if(h >= COLOR_ANGLE * 6) h -= COLOR_ANGLE * 6;//防止越界
			Hsv2Rgb(h, s, v, &r, &g, &b);
			FrameRaw[3*i+0] += ((int16_t)r - FrameRaw[3*i+0]) * fracUD[i] / 255;
			FrameRaw[3*i+1] += ((int16_t)g - FrameRaw[3*i+1]) * fracUD[i] / 255;
			FrameRaw[3*i+2] += ((int16_t)b - FrameRaw[3*i+2]) * fracUD[i] / 255;
		}
		else{
			FrameRaw[3*i+0] += ((int16_t)LIGHT_DOWN(3*i+0) - FrameRaw[3*i+0]) * fracUD[i] / 255;
			FrameRaw[3*i+1] += ((int16_t)LIGHT_DOWN(3*i+1) - FrameRaw[3*i+1]) * fracUD[i] / 255;
			FrameRaw[3*i+2] += ((int16_t)LIGHT_DOWN(3*i+2) - FrameRaw[3*i+2]) * fracUD[i] / 255;
		}
	}//16键处理结束
	
	if(ifINX == 0 && (taskTick & 0x0F) == 1//无匹配且为动作期
		|| LIGHT_T_WAIT && leaveTime == Systime){//或 启用等待时间且当前有按键
		taskTick = 2;//进入结束沿
	}
	
	//计算全局屏蔽效果供下一次使用
	if(leftSHLD > fracSHLD){//若屏蔽剩余值大于当前屏蔽比例
//		tool16 = (255 - fracSHLD) / 31;//(LIGHT_M_ON + 1);
//		if((255 - fracSHLD) && tool16 == 0) tool16 = 1;
//		fracSHLD += tool16;
		tool16 = 31 - LIGHT_M_ON;
		if(tool16 > 10) tool16 = (tool16 - 10) * 10 + 10;
		if(leftSHLD - fracSHLD > tool16) fracSHLD += tool16;//5;//屏蔽亮渐变
		else fracSHLD = leftSHLD;
	}else if(leftSHLD < fracSHLD){//若屏蔽剩余值小于当前屏蔽比例
		tool16 = (fracSHLD - leftSHLD) / (LIGHT_M_OFF + 1);//屏蔽灭渐变
		if(tool16 == 0) tool16 = 1;
		fracSHLD -= tool16;
	}
	
	MAP_GRB_TO_RGB: // 跳过所有硬件灯效 直接执行映射并写入帧缓存
	
	//GRB换位和旋转映射
	if(CFG_KB_DIR == 0){//正常方向
		for(i = 0; i < 16; i++){	FrameBuf[i*3+1] = FrameRaw[i*3+0];
			FrameBuf[i*3+0] = FrameRaw[i*3+1];FrameBuf[i*3+2] = FrameRaw[i*3+2];	}
	}else if(CFG_KB_DIR == 3){//左旋90度(此处与按键读取相反)
		for(i = 0; i < 16; i++){	FrameBuf[i*3+1] = FrameRaw[TURN_R90[i]*3+0];
			FrameBuf[i*3+0] = FrameRaw[TURN_R90[i]*3+1];FrameBuf[i*3+2] = FrameRaw[TURN_R90[i]*3+2];	}
	}else if(CFG_KB_DIR == 2){//旋转180度
		for(i = 0; i < 16; i++){	FrameBuf[i*3+1] = FrameRaw[(16 - i)*3+0];
			FrameBuf[i*3+0] = FrameRaw[(16 - i)*3+1];FrameBuf[i*3+2] = FrameRaw[(16 - i)*3+2];	}
	}else if(CFG_KB_DIR == 1){//右旋90度(此处与按键读取相反)
		for(i = 0; i < 16; i++){	FrameBuf[i*3+1] = FrameRaw[TURN_L90[i]*3+0];
			FrameBuf[i*3+0] = FrameRaw[TURN_L90[i]*3+1];FrameBuf[i*3+2] = FrameRaw[TURN_L90[i]*3+2];	}
	}
}

void WsWrite16(void){ // 写入16个灯
	UINT8D i, iBit;
	EA = 0; // 关中断
	for(i = 0; i < 16*3; i++){//GRB
		for(iBit = 7; iBit < 8; iBit--){
			if((FrameBuf[i] >> iBit) & 0x01){ // 1码
				WS_DOUT = 1; // P2 |= 0x80;
				WS_NOP_BIT1; // 延时
				WS_DOUT = 0; // P2 &= ~0x80;
			}else{ // 0码
				WS_DOUT = 1;
				WS_NOP_BIT0; // 延时
				WS_DOUT = 0;
			}
		}
	}
	EA = 1; // 开中断
}

//H:0~COLOR_ANGLE*6,S:0~100(已用delta代替),V:0~255
void Rgb2Hsv(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV){ // RGB转HSV
    UINT8D max = MAX(MAX(vR,vG),vB), min = MIN(MIN(vR,vG),vB);
    UINT8D delta = max - min;
    if(delta == 0) *pH = 0;
    else if(max == vR){
		if(vG >= vB) *pH = ((uint16_t)vG-vB)*COLOR_ANGLE/delta;
		else *pH = COLOR_ANGLE*6 - ((uint16_t)vB-vG)*COLOR_ANGLE/delta;
	}
    else if(max == vG){
		if(vB > vR) *pH = COLOR_ANGLE*2 + ((uint16_t)vB-vR)*COLOR_ANGLE/delta;
		else *pH = COLOR_ANGLE*2 - ((uint16_t)vR-vB)*COLOR_ANGLE/delta;
	}
    else if(max == vB){
		if(vR > vG) *pH = COLOR_ANGLE*4 + ((uint16_t)vR-vG)*COLOR_ANGLE/delta;
		else *pH = COLOR_ANGLE*4 - ((uint16_t)vG-vR)*COLOR_ANGLE/delta;
	}
    if(max == 0) *pS = 0;
    else *pS = delta;//100 * delta / max;//注意此处S直接用delta代替,故函数外直接修改V不合法
    *pV = max;
}
void Hsv2Rgb(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB){ // HSV转RGB
	UINT8D hi = (uint16_t)(vH / COLOR_ANGLE) % 6;
    UINT16D f = vH - hi * COLOR_ANGLE;
    UINT8D p = vV - vS;
    UINT8D q = vV - (uint16_t)vS * f / COLOR_ANGLE;
    UINT8D t = vV - (uint16_t)vS * (COLOR_ANGLE - f) / COLOR_ANGLE;
    if(hi == 0)     {*pR = vV;   *pG = t;    *pB = p;}
    else if(hi == 1){*pR = q;    *pG = vV;   *pB = p;}
    else if(hi == 2){*pR = p;    *pG = vV;   *pB = t;}
    else if(hi == 3){*pR = p;    *pG = q;    *pB = vV;}
    else if(hi == 4){*pR = t;    *pG = p;    *pB = vV;}
    else if(hi == 5){*pR = vV;   *pG = p;    *pB = q;}
}

static UINT16C RGB_DELAY[8] = {0,1,10,50, 100,500,1000,2000};//RGB延时表(单位ms)

extern bit bitNUM, bitCAPS, bitSCROLL;//数字锁定 大写锁定 滚动锁定
extern uint8_t clickerNum;//自动连点数
extern uint8_t mode3_key;//模式3按键(1-16)

extern uint32_t changeTime;//配置切换时间

void SysRGB(void){ // 系统RGB控制
	uint8_t i;
	static uint8_t rgbWeight = 0;//系统RGB指示灯权重
	uint8_t rgbOutput[3];//系统RGB指示灯输出
	uint8_t rgbInput[6];//系统RGB指示灯输入
	rgbInput[0] = 0;
	rgbInput[1] = bitNUM;
	rgbInput[2] = bitCAPS;
	rgbInput[3] = bitSCROLL;
	rgbInput[4] = !!clickerNum;
	rgbInput[5] = !!mode3_key;
	
	if((CFGb_RGB_T_ON && (Systime - changeTime) < RGB_DELAY[CFGb_RGB_T_ON]) || !CFGb_RGB_T_OFF){//配置切换后在设定时间之内 或 不灭
		rgbWeight = 255;
	}
	else if(CFGb_RGB_T_OFF){//超过设定时间 且 不是配置为不灭
		if(CFGb_RGB_T_OFF >= 4) rgbWeight = 0;//配置为立刻灭
		else if(rgbWeight >= (1 << (CFGb_RGB_T_OFF - 1))) rgbWeight -= (1 << (CFGb_RGB_T_OFF - 1));
		else rgbWeight = 0;
	}

	for(i = 0; i < 3; i++){//轮流处理R G B
		rgbOutput[i] = rgbInput[CFGb_RGB_MAP(i)] ^ CFGb_RGB_DIR(i);//映射 计算指示灯状态
		if(rgbOutput[i] && !CFGb_RGB_T_OFF){//若指示灯亮 且 标签配置为不灭
			rgbOutput[i] *= CFG_RGB_SIGN(i);//指示灯亮
		}
		else{
			rgbOutput[i] = (CFG_RGB_SIGN(i) * (255 - rgbWeight) * rgbOutput[i]
							+ CFG_RGB_RGB(i) * rgbWeight) / 255;//加权平均
		}
//		rgbOutput[i] = (CFG_RGB_SIGN(i) * (255 - rgbWeight) * (rgbInput[CFGb_RGB_MAP(i)] ^ CFGb_RGB_DIR(i)) 
//						+ CFG_RGB_RGB(i) * rgbWeight) / 255;//映射 加权平均
	}
	PWM_R = rgbOutput[0];
	PWM_G = rgbOutput[1];
	PWM_B = rgbOutput[2];
}















