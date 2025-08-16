
#include "ALK_If.h"

#include "ALK_Def.h"
#include "ALK_Func.h"


ALK_U32 alkSystime; // ALK系统时间

ALK_U8C TURN_L90[16] = {3,	7,	11,	15,
						2,	6,	10,	14,
						1,	5,	9,	13,
						0,	4,	8,	12}; // 左旋按键映射矩阵
ALK_U8C TURN_R90[16] = {12,	8,	4,	0,
						13,	9,	5,	1,
						14,	10,	6,	2,
						15,	11,	7,	3}; // 右旋按键映射矩阵




ALK_U8 KeyiRemap(ALK_U8 keyi, ALK_U8 oldDir, ALK_U8 newDir){ // 按键位置重映射
	newDir = (newDir + 4 - oldDir) % 4; // 相对旧键盘方向
	if(newDir == 1) return TURN_L90[keyi];
	else if(newDir == 2) return 16 - keyi;
	else if(newDir == 3) return TURN_R90[keyi];
	return keyi; // 方向相同
}


void ParaUpdate(ALK_U8 pos){//参数更新
	ALK_U16 addr;
	ALK_U8 i;
	
	if(pos < ALK_CFG_NUM) addr = DATA_CFG_BASE - pos * CFG_DATA_SIZE;//计算本套配置的起始地址
	else return;
	
	if(CFG_ACS(addr + (&CFG_ALL_PRI - CFG_THIS)) == 1){//若本配置为优先配置
		sysCs = pos;//总选择为本配置
		CFG_DATA_CSC(sysCs);//更新配置数据选择
		LIGHT_DATA_CSC(sysCs);//更新灯效数据选择
		KeyRGB(1);//键盘RGB控制清零
	}
	
	for(i = 0; i < ALK_KEY_NUM; i++){
		keyAddr[pos][i] = addr;//存储地址
		if(CFG_K_ID(addr) != i + 1){//若ID不对
			keyAddr[pos][0] = 0;//置零以标记为无效
			break;
		}
		if(CFG_K_MODE(addr) == m0_none || CFG_K_MODE(addr) == m8_buzz){
			addr += 2;
		}
		else if(CFG_K_MODE(addr) == m1_button){
			addr += 3;
		}
		else if(CFG_K_MODE(addr) == m2_shortcut || CFG_K_MODE(addr) == m6_change){
			addr += 4;
		}
		else if(CFG_K_MODE(addr) == m9_morse){
			addr += 5;
		}
		else if(CFG_K_MODE(addr) == m4_move || CFG_K_MODE(addr) == m5_press || CFG_K_MODE(addr) == m7_clicker){
			addr += 6;
		}
		else if(CFG_K_MODE(addr) == m3_group){
			addr += 3 + CFG_K_LEN(addr);
		}
		else{//模式不对
			keyAddr[pos][0] = 0;//置零以标记为无效
			break;
		}
	}
}











