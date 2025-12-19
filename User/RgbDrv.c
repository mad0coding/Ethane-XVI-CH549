
//全开理论灯功耗576，实测总功耗604，全关实测总功耗18，故得全开灯功耗604-18=590
#include "RgbDrv.h"


static UINT8I FrameBuf[ALK_KEY_NUM*3] = {//帧缓存
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
};//GRB


void ClearKeyRGB(void){ // 清除键盘RGB
	memset(FrameBuf, 0, sizeof(FrameBuf));
}

void RgbMap(void){ // 键盘RGB映射
	uint8_t i;
	// GRB换位和旋转映射
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















