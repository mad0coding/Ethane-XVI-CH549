
#include "Diag.h"

void DiagRead(uint8_t *buf, uint8_t len){//诊断数据读取
	memcpy(buf, (PUINT8C)DATA_DIAG_BASE, len);
}

void DiagFormat(uint8_t setting){//诊断数据格式化
	if(setting & DIAG_FMT_LOAD) DiagRead(DIAG_THIS, 64);//诊断数据载入内存
	if(setting & DIAG_FMT_MARK) DIAG_CNT_MARK = 0;//连标志错误一同归0
	memset(DIAG_THIS + 2, 0, 56 - 2 + 2);
	DIAG_SUM = ~DIAG_CNT_MARK;//计算校验
	DIAG_MARK = DIAG_MARK_VALUE;//设置标志
	if(setting & DIAG_FMT_SAVE) ;//保存数据
}

uint16_t DiagSum(uint8_t *buf, uint8_t len){//诊断数据计算校验
	uint16_t sum = 0;
	do{ sum += buf[len--]; }while(len);
	return sum;
}

void DiagInit(void){//诊断初始化处理
	uint8_t tmp = 0;
	DiagRead(DIAG_THIS, 64);//诊断数据载入内存
	if(DIAG_MARK != DIAG_MARK_VALUE){//标志错误
		DiagFormat(DIAG_FMT_NONE);//诊断数据格式化
	}
	if(DIAG_RST_FLAG == RST_REASON_WDG){//若本次为看门狗复位
		if(DIAG_RST_LAST){//若上次也是看门狗复位
			if(DIAG_RST_CNT == 0){//若为新发生的连续看门狗复位
				//这里让连续看门狗复位计数+1
			}
			DIAG_RST_CNT_SET(7);//设置计数值
		}
		else if(DIAG_RST_CNT == 0){//若上次非看门狗复位 且 计数为0
			//这里让看门狗复位计数+1
		}
		DIAG_RST_LAST_SET(1);//记录为看门狗复位
	}
	else if(DIAG_RST_CNT > 0){//若本次非看门狗复位 且 计数非0
		DIAG_RST_CNT_DEC;//计数递减
		DIAG_RST_LAST_SET(0);//记录为非看门狗复位
	}
	
}

void DiagCountInc(uint8_t pos, uint8_t fail){//闪存擦除计数增加
	uint16_t addr = DATA_DIAG_BASE + 128 * pos;
	if(fail){//错误计数增加
		
		return;
	}
	if(*(PUINT16C)(addr + 128 - 2) != 0x5A10){//标志码不合法
		
	}
}























