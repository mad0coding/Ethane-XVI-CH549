
#include "ALK_If.h"


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














