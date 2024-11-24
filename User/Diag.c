
#include "Diag.h"

void DiagGet(uint8_t *buf, uint8_t len){//诊断数据获取 对外接口
	memcpy(buf, (PUINT8C)DATA_DIAG_BASE, len);
}

static void DiagRead(uint8_t *buf){//诊断数据读取
	memcpy(buf, (PUINT8C)DATA_DIAG_BASE, 64);
}

static void DiagWrite(uint8_t *buf){//诊断数据写入
	ParaWrite(DATA_DIAG_BASE, buf, 1);//参数写入
}

static uint16_t DiagSum(uint8_t *buf, uint8_t len){//诊断数据计算校验
	uint16_t sum = 0;
	do{ sum += buf[len--]; }while(len);
	return ~sum;//取反
}

static void DiagFormat(uint8_t setting){//诊断数据格式化
	if(setting & DIAG_FMT_LOAD) DiagRead(DIAG_THIS);//诊断数据载入内存
	if(setting & DIAG_FMT_MARK) DIAG_CNT(DIAG_I_MARK) = 0;//连标志错误一同归0
	else DIAG_CNT(DIAG_I_MARK)++;//标志错误计数+1
	memset(DIAG_THIS + 2, 0, 56 - 2 + 2);//清零
	DIAG_LAST = DIAG_I_MARK;//记录最后错误
	DIAG_MARK = DIAG_MARK_VALUE;//设置标志
	DIAG_SUM = DiagSum(DIAG_THIS, 56);//更新校验
	if(setting & DIAG_FMT_SAVE) DiagWrite(DIAG_THIS);//保存数据
}

void DiagCountInc(uint8_t setting, uint8_t pos){//诊断计数增加
//	if(pos >= 28) return;//越界
//	if(setting & DIAG_FMT_LOAD) DiagRead(DIAG_THIS);//诊断数据载入内存
//	DIAG_CNT(pos)++;//计数+1
//	DIAG_LAST = pos;//记录最后错误
//	DIAG_SUM = DiagSum(DIAG_THIS, 56);//更新校验
//	if(setting & DIAG_FMT_SAVE) DiagWrite(DIAG_THIS);//保存数据
}

void DiagInit(void){//诊断初始化处理
	uint8_t needSave = 0;
	DiagRead(DIAG_THIS);//诊断数据载入内存
	if(DIAG_MARK != DIAG_MARK_VALUE){//标志错误
		DiagFormat(DIAG_FMT_NONE);//诊断数据格式化
		needSave = 1;
	}
	if(DIAG_SUM != DiagSum(DIAG_THIS, 56)){//校验错误
		DIAG_SUM = DiagSum(DIAG_THIS, 56);//更新校验
		DiagCountInc(DIAG_FMT_NONE, DIAG_I_SUM);//诊断计数增加
		needSave = 1;
	}
	if(DIAG_RST_FLAG == RST_REASON_WDG){//若本次为看门狗复位
		if(DIAG_RST_LAST){//若上次也是看门狗复位
			if(DIAG_RST_CNT == 0){//若为新发生的连续看门狗复位
				DiagCountInc(DIAG_FMT_NONE, DIAG_I_WDGS);//诊断计数增加
				needSave = 1;
			}
			DIAG_RST_CNT_SET(7);//设置计数值
		}
		else if(DIAG_RST_CNT == 0){//若上次非看门狗复位 且 计数为0
			DiagCountInc(DIAG_FMT_NONE, DIAG_I_WDG);//诊断计数增加
			needSave = 1;
		}
		DIAG_RST_LAST_SET(1);//记录为看门狗复位
	}
	else{//若本次非看门狗复位
		if(DIAG_RST_CNT > 0) DIAG_RST_CNT_DEC;//计数非0则计数递减
		DIAG_RST_LAST_SET(0);//记录为非看门狗复位
	}
	if(needSave) DiagWrite(DIAG_THIS);//诊断数据写入闪存
}

























