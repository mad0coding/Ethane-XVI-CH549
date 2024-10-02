
#include "ParaConfig.h"

UINT8X FlashBuf[512] _at_ (1024+512);//���û�������

uint16_t keyAddr[CFG_NUM][16];//3��ÿ��16���������ݵ�ַ
uint16_t keyWork[16];//16�����Ĺ���������
uint8_t keyFlag[16];//16�����ı��������

uint8_t key_turn[CFG_NUM];//���̷���

uint8_t sys_cs = 0;//������ѡ��

PUINT8C DATA_CFG = DATA_CFG_BASE;//������������Ϣָ��
PUINT8C GLOB_CFG = DATA_GLOB_BASE;//������ȫ����Ϣָ��

void asyncHandle(uint8_t flag){//�첽����
	uint8_t ret;
	if(flag >= 10 && flag < 10 + CFG_NUM){		//�������ô洢
		ret = paraWrite((DATA_CFG_BASE - (flag - 10) * 512), FlashBuf, 8);
		paraUpdate(flag - 10);//�������ò�������
	}
	else if(flag >= 20 && flag < 20 + CFG_NUM){	//��Ч���ô洢
		ret = paraWrite((DATA_LIGHT_BASE - (flag - 20) * 256), FlashBuf, 4);
		keyRGB(1);//����RGB��������
	}
	else if(flag == 30){						//ȫ�ֲ����洢
		globalParaUpdate();//ȫ�ֲ�������
		ret = paraWrite(DATA_GLOB_BASE, FlashBuf, 1);
	}
	else if(flag == 100){						//��λ
		clearKeyRGB();//�������RGB
		wsWrite16();//��д��
		mDelaymS(100);//��ʱ����USB�������
		IE = 0;//ȫ���жϹر�
		CH549SoftReset();//��λ
	}
	else if(flag == 101){						//BootԤ��ת
		clearKeyRGB();//�������RGB
		wsWrite16();//��д��
		mDelaymS(100);//��ʱ����USB�������
		IE = 0;//ȫ���жϹر�
		USB_INT_FG = 0xFF;	//���жϱ�־
		USB_CTRL = 0x06;	//��λUSB���ƼĴ����������临λ�����Ĵ���
		Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;//��������־��λ
		Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
		FlashErasePage(0);//����FLASH��ͷ �����´��ϵ�ʱ�ܽ�Boot
		while(1) WDOG_COUNT = 0;//��ѭ�� ���㿴�Ź�����
	}
	else if(flag == 102){
		
	}
}

void paraSave1(uint8_t pos, uint8_t num){//��������
	uint16_t addr;
	
	if(pos >= 1 && pos <= CFG_NUM) addr = DATA_CFG_BASE - (pos - 1) * 512;//�������ô洢λ�ü���
	else if(pos >= 51 && pos <= 50 + CFG_NUM) addr = DATA_LIGHT_BASE - (pos - 51) * 256;//��Ч���ô洢λ�ü���
	else if(pos == 100) addr = DATA_GLOB_BASE;//ȫ�ֲ�����ַ
	else if(pos == 200) addr = 0;//0��ַ
	else return;
	
	
	if(paraWrite(addr, FlashBuf, num) == 0){//����д��
		flashCountInc(0, 0);//��ȷ��������
	}
	else flashCountInc(0, 1);//�����������
}

uint8_t paraWrite(uint16_t addr, uint8_t *buf, uint8_t num){//����д��
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

void paraLoad(void){//������ȡ
	globalParaLoad();//ȫ�ֲ�����ȡ
	paraUpdate(2);
	paraUpdate(1);
	paraUpdate(0);
	memset(keyWork, 0, sizeof(keyWork));
	memset(keyFlag, 0, sizeof(keyFlag));
}

void paraUpdate(uint8_t pos){//��������
	uint16_t addr;
	uint8_t i;
	
	if(pos < CFG_NUM) addr = DATA_CFG_BASE - pos * 512;//���㱾�����õ���ʼ��ַ
	else return;
	
	key_turn[pos] = CFG_ACS(addr + (&CFG_KB_DIR - CFG_THIS));//��ȡ���̷���
	
	if(CFG_ACS(addr + (&CFG_ALL_PRI - CFG_THIS)) == 1){//��������Ϊ��������
		sys_cs = pos;//��ѡ��Ϊ������
		DATA_CFG = (PUINT8C)addr;//ָ��ָ������
		DATA_LIGHT = DATA_LIGHT_BASE - sys_cs * 256;//�޸ĵ�Ч����ָ��
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

void globalParaLoad(void){//ȫ�ֲ�����ȡ
	ANA_MID_SET[0] = GLOB_ANA_MID1;//ҡ����λ
	ANA_MID_SET[1] = GLOB_ANA_MID2;
	keyFltNum = GLOB_KEY_FLT;//�����˲�����
	EC1freq = GLOBb_EC_FREQ1;//��ť��Ƶ����
	EC2freq = GLOBb_EC_FREQ2;
}

void globalParaUpdate(void){//ȫ�ֲ�������
	((uint16_t*)(FlashBuf))[0] = ANA_MID_SET[0];//ҡ����λ
	((uint16_t*)(FlashBuf))[1] = ANA_MID_SET[1];
	FlashBuf[4] = keyFltNum;//�����˲�����
	FlashBuf[5] = (((uint8_t)EC2freq) << 1) | ((uint8_t)EC1freq);//��ť��Ƶ����
//	paraSave(100, 1);//��������
}

void flashCountInc(uint8_t pos, uint8_t fail){//���������������
	uint16_t addr = DATA_COUNT_BASE + 128 * pos;
	if(fail){//�����������
		
		return;
	}
	if(*(PUINT16C)(addr + 128 - 2) != 0x5A10){//��־�벻�Ϸ�
		
	}
}

uint32_t flashCountGet(uint8_t pos, uint8_t fail){//�������������ȡ
	uint32_t flashCount;
	uint16_t addr = DATA_COUNT_BASE + 128 * pos;
	if(fail) return *(PUINT16C)(addr + 128 - 4);//���ش������
	flashCount = *(PUINT32C)(addr + 128 - 8) * 120UL;//��ȷ�����Ľ�λ����
	//for(addr)
	
	return flashCount;
}




















