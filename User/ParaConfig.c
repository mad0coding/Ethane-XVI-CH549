
#include "ParaConfig.h"

UINT8X FlashBuf[512] _at_ XBASE_FLASH_BUF;//配置缓存数组

uint16_t keyAddr[CFG_NUM][16];//3组每组16按键的数据地址
uint16_t keyWork[16];//16按键的工作用数组
uint8_t keyFlag[16];//16按键的标记用数组

uint8_t keyDir[CFG_NUM];//键盘方向

uint8_t sysCs = 0;//总配置选择

PUINT8C DATA_CFG = DATA_CFG_BASE;//闪存区配置信息指针
PUINT8C GLOB_CFG = DATA_GLOB_BASE;//闪存区全局信息指针

void AsyncHandle(uint8_t flag){//异步处理
	uint8_t ret;
	if(flag >= ASYNC_FLAG_CFG && flag < ASYNC_FLAG_CFG + CFG_NUM){			//键盘配置存储
		ret = ParaWrite((DATA_CFG_BASE - (flag - ASYNC_FLAG_CFG) * 512), FlashBuf, 8);
		ParaUpdate(flag - ASYNC_FLAG_CFG);//键盘配置参数更新
	}
	else if(flag >= ASYNC_FLAG_LIGHT && flag < ASYNC_FLAG_LIGHT + CFG_NUM){	//灯效配置存储
		ret = ParaWrite((DATA_LIGHT_BASE - (flag - ASYNC_FLAG_LIGHT) * 256), FlashBuf, 4);
		KeyRGB(1);//键盘RGB控制清零
	}
	else if(flag == ASYNC_FLAG_GLOB){		//全局参数存储
		GlobalParaUpdate();//全局参数更新
		ret = ParaWrite(DATA_GLOB_BASE, FlashBuf, 1);
	}
	else if(flag == ASYNC_FLAG_SRST){		//软复位
		ClearKeyRGB();//清除键盘RGB
		WsWrite16();//灯写入
		mDelaymS(100);//延时以让USB发送完成
		IE = 0;//全局中断关闭
		CH549SoftReset();//软复位
	}
	else if(flag == ASYNC_FLAG_BOOT){		//Boot预跳转
		ClearKeyRGB();//清除键盘RGB
		WsWrite16();//灯写入
		mDelaymS(100);//延时以让USB发送完成
		IE = 0;//全局中断关闭
		USB_INT_FG = 0xFF;	//清中断标志
		USB_CTRL = 0x06;	//复位USB控制寄存器并利用其复位其他寄存器
		
		Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;//保护检查标志置位
		Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
		
		memset(FlashBuf, 0, 64);//第一扇区置空 以在下次上电时能进Boot
		FlashBuf[62] = 0x80; FlashBuf[63] = 0xFE;//使用SJMP汇编指令向前跳2字节 实现while(1)死循环
		ret = ParaWrite(0, FlashBuf, 1);//擦除并写入第一扇区

		while(1) WDOG_COUNT = 0;//死循环 清零看门狗计数
	}
	else if(flag == ASYNC_FLAG_FCNT){
		
	}
}

void paraSave1(uint8_t pos, uint8_t num){//参数保存
	uint16_t addr;
	
	if(pos >= 1 && pos <= CFG_NUM) addr = DATA_CFG_BASE - (pos - 1) * 512;//键盘配置存储位置计算
	else if(pos >= 51 && pos <= 50 + CFG_NUM) addr = DATA_LIGHT_BASE - (pos - 51) * 256;//灯效配置存储位置计算
	else if(pos == 100) addr = DATA_GLOB_BASE;//全局参数地址
	else if(pos == 200) addr = 0;//0地址
	else return;
	
	
	if(ParaWrite(addr, FlashBuf, num) == 0){//参数写入
		FlashCountInc(0, 0);//正确计数增加
	}
	else FlashCountInc(0, 1);//错误计数增加
}

uint8_t ParaWrite(uint16_t addr, uint8_t *buf, uint8_t num){//参数写入
	uint16_t i;
	
	Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;//保护检查标志置位
	Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
	
	for(i = 0; i < num; i++){
		if(FlashErasePage(addr + i * 64)){//若擦除失败
			mDelaymS(1);
			FlashErasePage(addr + i * 64);//再次擦除
		}
		if(FlashProgPage(addr + i * 64, buf + i * 64, 64)){//若编程失败
			mDelaymS(1);
			FlashProgPage(addr + i * 64, buf + i * 64, 64);//再次编程
		}
	}
	
	Flash_Op_Check_Byte1 = 0;//保护检查标志复位
	Flash_Op_Check_Byte2 = 0;
	
	if(num == 4) buf[255]++;//错误注入 测试代码
	
	for(i = 0; i < num * 64; i++){//数据正确性检验
		if(*(PUINT8C)(addr + i) != buf[i]) return 1;//数据错误
	}
	return 0;
}

void ParaLoad(void){//参数读取
	GlobalParaLoad();//全局参数读取
	ParaUpdate(2);
	ParaUpdate(1);
	ParaUpdate(0);
	memset(keyWork, 0, sizeof(keyWork));
	memset(keyFlag, 0, sizeof(keyFlag));
}

void ParaUpdate(uint8_t pos){//参数更新
	uint16_t addr;
	uint8_t i;
	
	if(pos < CFG_NUM) addr = DATA_CFG_BASE - pos * 512;//计算本套配置的起始地址
	else return;
	
	keyDir[pos] = CFG_ACS(addr + (&CFG_KB_DIR - CFG_THIS));//读取键盘方向
	
	if(CFG_ACS(addr + (&CFG_ALL_PRI - CFG_THIS)) == 1){//若本配置为优先配置
		sysCs = pos;//总选择为本配置
		DATA_CFG = (PUINT8C)addr;//指针指向本配置
		DATA_LIGHT = DATA_LIGHT_BASE - sysCs * 256;//修改灯效配置指针
	}
	
	for(i = 0; i < 16; i++){
		keyAddr[pos][i] = addr;//存储地址
		if(CFG_K_ID(addr) != i + 1){//若ID不对
			keyAddr[pos][0] = 0;//置零以标记为无效
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
			keyAddr[pos][0] = 0;//置零以标记为无效
			break;
		}
	}
}

void GlobalParaLoad(void){//全局参数读取
	Adc_Mid_Set[0] = GLOB_ANA_MID1;//摇杆中位
	Adc_Mid_Set[1] = GLOB_ANA_MID2;
	if(!Adc_Mid_Set[0] || Adc_Mid_Set[0] >= 4095) Adc_Mid_Set[0] = 2048;//非法数据检查
	if(!Adc_Mid_Set[1] || Adc_Mid_Set[1] >= 4095) Adc_Mid_Set[1] = 2048;
	keyFltNum = GLOB_KEY_FLT;//按键滤波参数
	EC1freq = GLOBb_EC_FREQ1;//旋钮倍频参数
	EC2freq = GLOBb_EC_FREQ2;
}

void GlobalParaUpdate(void){//全局参数更新
	((uint16_t*)(FlashBuf))[0] = Adc_Mid_Set[0];//摇杆中位
	((uint16_t*)(FlashBuf))[1] = Adc_Mid_Set[1];
	FlashBuf[4] = keyFltNum;//按键滤波参数
	FlashBuf[5] = (((uint8_t)EC2freq) << 1) | ((uint8_t)EC1freq);//旋钮倍频参数
//	ParaSave(100, 1);//参数保存
}

void FlashCountInc(uint8_t pos, uint8_t fail){//闪存擦除计数增加
	uint16_t addr = DATA_DIAG_BASE + 128 * pos;
	if(fail){//错误计数增加
		
		return;
	}
	if(*(PUINT16C)(addr + 128 - 2) != 0x5A10){//标志码不合法
		
	}
}

uint32_t FlashCountGet(uint8_t pos, uint8_t fail){//闪存擦除计数获取
	uint32_t flashCount;
	uint16_t addr = DATA_DIAG_BASE + 128 * pos;
	if(fail) return *(PUINT16C)(addr + 128 - 4);//返回错误计数
	flashCount = *(PUINT32C)(addr + 128 - 8) * 120UL;//正确计数的进位部分
	//for(addr)
	
	return flashCount;
}




















