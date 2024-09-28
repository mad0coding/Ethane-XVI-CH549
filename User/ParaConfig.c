
#include "ParaConfig.h"

UINT8X FlashBuf[512] _at_ (1024+512);//配置缓存数组

uint16_t keyAddr[CFG_NUM][16];//3组每组16按键的数据地址
uint16_t keyWork[16];//16按键的工作用数组
uint8_t keyFlag[16];//16按键的标记用数组

uint8_t key_turn[CFG_NUM];//键盘方向

uint8_t sys_cs = 0;//总配置选择

PUINT8C DATA_CFG = DATA_CFG_BASE;//闪存区配置信息指针
PUINT8C GLOB_CFG = DATA_GLOB_BASE;//闪存区全局信息指针


void ParaSave(uint8_t pos, uint8_t num){//参数保存
	uint8_t i;
	uint16_t addr;
	if(pos >= 1 && pos <= CFG_NUM) addr = DATA_CFG_BASE - (pos - 1) * 512;//键盘配置存储位置计算
	else if(pos >= 51 && pos <= 50 + CFG_NUM) addr = DATA_LIGHT_BASE - (pos - 51) * 256;//灯效配置存储位置计算
	else if(pos == 100) addr = DATA_GLOB_BASE;//全局参数地址
	else return;
	
	Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;//保护检查标志置位
	Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
	
	for(i = 0; i < num; i++){
		if(FlashErasePage(addr + i * 64)){//若擦除失败
			mDelaymS(1);
			FlashErasePage(addr + i * 64);//再次擦除
		}
		if(FlashProgPage(addr + i * 64, FlashBuf + i * 64, 64)){//若编程失败
			mDelaymS(1);
			FlashProgPage(addr + i * 64, FlashBuf + i * 64, 64);//再次编程
		}
	}
	
	Flash_Op_Check_Byte1 = 0;//保护检查标志复位
	Flash_Op_Check_Byte2 = 0;
}

void ParaLoad(void){//参数读取
	GlobalParaLoad();//全局参数读取
	ParaUpdate(3);
	ParaUpdate(2);
	ParaUpdate(1);
	memset(keyWork,0,sizeof(keyWork));
	memset(keyFlag,0,sizeof(keyFlag));
}

void ParaUpdate(uint8_t pos){//参数更新
	uint16_t addr;
	uint8_t i;
	
	if(pos >= 1 && pos <= CFG_NUM) addr = DATA_CFG_BASE - (pos - 1) * 512;//计算本套配置的起始地址
	else if(pos >= 51 && pos <= 50 + CFG_NUM){
		keyRGB(1);//键盘RGB控制清零
		return;
	}
	else return;
	
	key_turn[pos - 1] = CFG_ACS(addr + (&CFG_KB_DIR - CFG_THIS));//读取键盘方向
	
	if(CFG_ACS(addr + (&CFG_ALL_PRI - CFG_THIS)) == 1){//若本配置为优先配置
		sys_cs = pos - 1;//总选择为本配置
		DATA_CFG = (PUINT8C)addr;//指针指向本配置
		DATA_LIGHT = DATA_LIGHT_BASE - sys_cs * 256;//修改灯效配置指针
	}
	
	for(i = 0; i < 16; i++){
		keyAddr[pos - 1][i] = addr;//存储地址
		if(CFG_K_ID(addr) != i + 1){//若ID不对
			keyAddr[pos - 1][0] = 0;//置零以标记为无效
			break;
		}
		if(CFG_K_MODE(addr) == 0 || CFG_K_MODE(addr) == 8){
			addr += 2;
		}
		else if(CFG_K_MODE(addr) == 1){
			addr += 3;
		}
		else if(CFG_K_MODE(addr) == 2 || CFG_K_MODE(addr) == 6){
			addr += 4;
		}
		else if(CFG_K_MODE(addr) == 4 || CFG_K_MODE(addr) == 5 || CFG_K_MODE(addr) == 7){
			addr += 6;
		}
		else if(CFG_K_MODE(addr) == 3){
			addr += 3 + CFG_K_LEN(addr);
		}
		else{//模式不对
			keyAddr[pos - 1][0] = 0;//置零以标记为无效
			break;
		}
	}
}

void GlobalParaLoad(void){//全局参数读取
	ANA_MID_SET[0] = GLOB_ANA_MID1;//摇杆中位
	ANA_MID_SET[1] = GLOB_ANA_MID2;
	keyFltNum = GLOB_KEY_FLT;//按键滤波参数
	EC1freq = GLOBb_EC_FREQ1;//旋钮倍频参数
	EC2freq = GLOBb_EC_FREQ2;
}

void GlobalParaUpdate(void){//全局参数更新
	((uint16_t*)(FlashBuf))[0] = ANA_MID_SET[0];//摇杆中位
	((uint16_t*)(FlashBuf))[1] = ANA_MID_SET[1];
	FlashBuf[4] = keyFltNum;//按键滤波参数
	FlashBuf[5] = (((uint8_t)EC2freq) << 1) | ((uint8_t)EC1freq);//旋钮倍频参数
	ParaSave(100, 1);//参数保存
}





















