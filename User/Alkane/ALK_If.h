#ifndef _ALK_IF_H_
#define _ALK_IF_H_

#include "ALK_Def.h"

#include "ParaConfig.h"



extern ALK_U8C TURN_L90[16]; // 左旋按键映射矩阵
extern ALK_U8C TURN_R90[16]; // 右旋按键映射矩阵



ALK_U8 KeyiRemap(ALK_U8 keyi, ALK_U8 oldDir, ALK_U8 newDir); // 按键位置重映射



#endif

