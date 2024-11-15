
#include "ParaConfig.h"

UINT8X FlashBuf[512] _at_ XBASE_FLASH_BUF;//���û�������

uint16_t keyAddr[CFG_NUM][16];//3��ÿ��16���������ݵ�ַ
uint16_t keyWork[16];//16�����Ĺ���������
uint8_t keyFlag[16];//16�����ı��������

uint8_t keyDir[CFG_NUM];//���̷���

uint8_t sysCs = 0;//������ѡ��

PUINT8C DATA_CFG = DATA_CFG_BASE;//������������Ϣָ��
PUINT8C GLOB_CFG = DATA_GLOB_BASE;//������ȫ����Ϣָ��

void AsyncHandle(uint8_t flag){//�첽����
	uint8_t ret;
	if(flag >= ASYNC_FLAG_CFG && flag < ASYNC_FLAG_CFG + CFG_NUM){			//�������ô洢
		ret = ParaWrite((DATA_CFG_BASE - (flag - ASYNC_FLAG_CFG) * 512), FlashBuf, 8);
		ParaUpdate(flag - ASYNC_FLAG_CFG);//�������ò�������
	}
	else if(flag >= ASYNC_FLAG_LIGHT && flag < ASYNC_FLAG_LIGHT + CFG_NUM){	//��Ч���ô洢
		ret = ParaWrite((DATA_LIGHT_BASE - (flag - ASYNC_FLAG_LIGHT) * 256), FlashBuf, 4);
		KeyRGB(1);//����RGB��������
	}
	else if(flag == ASYNC_FLAG_GLOB){		//ȫ�ֲ����洢
		GlobalParaUpdate();//ȫ�ֲ�������
		ret = ParaWrite(DATA_GLOB_BASE, FlashBuf, 1);
	}
	else if(flag == ASYNC_FLAG_SRST){		//��λ
		ClearKeyRGB();//�������RGB
		WsWrite16();//��д��
		mDelaymS(100);//��ʱ����USB�������
		IE = 0;//ȫ���жϹر�
		CH549SoftReset();//��λ
	}
	else if(flag == ASYNC_FLAG_BOOT){		//BootԤ��ת
		ClearKeyRGB();//�������RGB
		WsWrite16();//��д��
		mDelaymS(100);//��ʱ����USB�������
		IE = 0;//ȫ���жϹر�
		USB_INT_FG = 0xFF;	//���жϱ�־
		USB_CTRL = 0x06;	//��λUSB���ƼĴ����������临λ�����Ĵ���
		
		Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;//��������־��λ
		Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
		
		memset(FlashBuf, 0, 64);//��һ�����ÿ� �����´��ϵ�ʱ�ܽ�Boot
		FlashBuf[62] = 0x80; FlashBuf[63] = 0xFE;//ʹ��SJMP���ָ����ǰ��2�ֽ� ʵ��while(1)��ѭ��
		ret = ParaWrite(0, FlashBuf, 1);//������д���һ����

		while(1) WDOG_COUNT = 0;//��ѭ�� ���㿴�Ź�����
	}
	else if(flag == ASYNC_FLAG_FCNT){
		
	}
}

void paraSave1(uint8_t pos, uint8_t num){//��������
	uint16_t addr;
	
	if(pos >= 1 && pos <= CFG_NUM) addr = DATA_CFG_BASE - (pos - 1) * 512;//�������ô洢λ�ü���
	else if(pos >= 51 && pos <= 50 + CFG_NUM) addr = DATA_LIGHT_BASE - (pos - 51) * 256;//��Ч���ô洢λ�ü���
	else if(pos == 100) addr = DATA_GLOB_BASE;//ȫ�ֲ�����ַ
	else if(pos == 200) addr = 0;//0��ַ
	else return;
	
	
	if(ParaWrite(addr, FlashBuf, num) == 0){//����д��
		FlashCountInc(0, 0);//��ȷ��������
	}
	else FlashCountInc(0, 1);//�����������
}

uint8_t ParaWrite(uint16_t addr, uint8_t *buf, uint8_t num){//����д��
	uint16_t i;
	
	Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;//��������־��λ
	Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
	
	for(i = 0; i < num; i++){
		if(FlashErasePage(addr + i * 64)){//������ʧ��
			mDelaymS(1);
			FlashErasePage(addr + i * 64);//�ٴβ���
		}
		if(FlashProgPage(addr + i * 64, buf + i * 64, 64)){//�����ʧ��
			mDelaymS(1);
			FlashProgPage(addr + i * 64, buf + i * 64, 64);//�ٴα��
		}
	}
	
	Flash_Op_Check_Byte1 = 0;//��������־��λ
	Flash_Op_Check_Byte2 = 0;
	
	if(num == 4) buf[255]++;//����ע�� ���Դ���
	
	for(i = 0; i < num * 64; i++){//������ȷ�Լ���
		if(*(PUINT8C)(addr + i) != buf[i]) return 1;//���ݴ���
	}
	return 0;
}

void ParaLoad(void){//������ȡ
	GlobalParaLoad();//ȫ�ֲ�����ȡ
	ParaUpdate(2);
	ParaUpdate(1);
	ParaUpdate(0);
	memset(keyWork, 0, sizeof(keyWork));
	memset(keyFlag, 0, sizeof(keyFlag));
}

void ParaUpdate(uint8_t pos){//��������
	uint16_t addr;
	uint8_t i;
	
	if(pos < CFG_NUM) addr = DATA_CFG_BASE - pos * 512;//���㱾�����õ���ʼ��ַ
	else return;
	
	keyDir[pos] = CFG_ACS(addr + (&CFG_KB_DIR - CFG_THIS));//��ȡ���̷���
	
	if(CFG_ACS(addr + (&CFG_ALL_PRI - CFG_THIS)) == 1){//��������Ϊ��������
		sysCs = pos;//��ѡ��Ϊ������
		DATA_CFG = (PUINT8C)addr;//ָ��ָ������
		DATA_LIGHT = DATA_LIGHT_BASE - sysCs * 256;//�޸ĵ�Ч����ָ��
	}
	
	for(i = 0; i < 16; i++){
		keyAddr[pos][i] = addr;//�洢��ַ
		if(CFG_K_ID(addr) != i + 1){//��ID����
			keyAddr[pos][0] = 0;//�����Ա��Ϊ��Ч
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
		else{//ģʽ����
			keyAddr[pos][0] = 0;//�����Ա��Ϊ��Ч
			break;
		}
	}
}

void GlobalParaLoad(void){//ȫ�ֲ�����ȡ
	Adc_Mid_Set[0] = GLOB_ANA_MID1;//ҡ����λ
	Adc_Mid_Set[1] = GLOB_ANA_MID2;
	if(!Adc_Mid_Set[0] || Adc_Mid_Set[0] >= 4095) Adc_Mid_Set[0] = 2048;//�Ƿ����ݼ��
	if(!Adc_Mid_Set[1] || Adc_Mid_Set[1] >= 4095) Adc_Mid_Set[1] = 2048;
	keyFltNum = GLOB_KEY_FLT;//�����˲�����
	EC1freq = GLOBb_EC_FREQ1;//��ť��Ƶ����
	EC2freq = GLOBb_EC_FREQ2;
}

void GlobalParaUpdate(void){//ȫ�ֲ�������
	((uint16_t*)(FlashBuf))[0] = Adc_Mid_Set[0];//ҡ����λ
	((uint16_t*)(FlashBuf))[1] = Adc_Mid_Set[1];
	FlashBuf[4] = keyFltNum;//�����˲�����
	FlashBuf[5] = (((uint8_t)EC2freq) << 1) | ((uint8_t)EC1freq);//��ť��Ƶ����
//	ParaSave(100, 1);//��������
}

void FlashCountInc(uint8_t pos, uint8_t fail){//���������������
	uint16_t addr = DATA_DIAG_BASE + 128 * pos;
	if(fail){//�����������
		
		return;
	}
	if(*(PUINT16C)(addr + 128 - 2) != 0x5A10){//��־�벻�Ϸ�
		
	}
}

uint32_t FlashCountGet(uint8_t pos, uint8_t fail){//�������������ȡ
	uint32_t flashCount;
	uint16_t addr = DATA_DIAG_BASE + 128 * pos;
	if(fail) return *(PUINT16C)(addr + 128 - 4);//���ش������
	flashCount = *(PUINT32C)(addr + 128 - 8) * 120UL;//��ȷ�����Ľ�λ����
	//for(addr)
	
	return flashCount;
}




















