
#include "ParaConfig.h"

UINT8X FlashBuf[512] _at_ (1024+512);//���û�������

uint16_t keyAddr[CFG_NUM][16];//3��ÿ��16���������ݵ�ַ
uint16_t keyWork[16];//16�����Ĺ���������
uint8_t keyFlag[16];//16�����ı��������

uint8_t key_turn[CFG_NUM];//���̷���

uint8_t sys_cs = 0;//������ѡ��

PUINT8C DATA_CFG = DATA_CFG_BASE;//������������Ϣָ��
PUINT8C GLOB_CFG = DATA_GLOB_BASE;//������ȫ����Ϣָ��


void ParaSave(uint8_t pos, uint8_t num){//��������
	uint8_t i;
	uint16_t addr;
	if(pos >= 1 && pos <= CFG_NUM) addr = DATA_CFG_BASE - (pos - 1) * 512;//�������ô洢λ�ü���
	else if(pos >= 51 && pos <= 50 + CFG_NUM) addr = DATA_LIGHT_BASE - (pos - 51) * 256;//��Ч���ô洢λ�ü���
	else if(pos == 100) addr = DATA_GLOB_BASE;//ȫ�ֲ�����ַ
	else return;
	
	Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;//��������־��λ
	Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
	
	for(i = 0; i < num; i++){
		if(FlashErasePage(addr + i * 64)){//������ʧ��
			mDelaymS(1);
			FlashErasePage(addr + i * 64);//�ٴβ���
		}
		if(FlashProgPage(addr + i * 64, FlashBuf + i * 64, 64)){//�����ʧ��
			mDelaymS(1);
			FlashProgPage(addr + i * 64, FlashBuf + i * 64, 64);//�ٴα��
		}
	}
	
	Flash_Op_Check_Byte1 = 0;//��������־��λ
	Flash_Op_Check_Byte2 = 0;
}

void ParaLoad(void){//������ȡ
	GlobalParaLoad();//ȫ�ֲ�����ȡ
	ParaUpdate(3);
	ParaUpdate(2);
	ParaUpdate(1);
	memset(keyWork,0,sizeof(keyWork));
	memset(keyFlag,0,sizeof(keyFlag));
}

void ParaUpdate(uint8_t pos){//��������
	uint16_t addr;
	uint8_t i;
	
	if(pos >= 1 && pos <= CFG_NUM) addr = DATA_CFG_BASE - (pos - 1) * 512;//���㱾�����õ���ʼ��ַ
	else if(pos >= 51 && pos <= 50 + CFG_NUM){
		keyRGB(1);//����RGB��������
		return;
	}
	else return;
	
	key_turn[pos - 1] = CFG_ACS(addr + (&CFG_KB_DIR - CFG_THIS));//��ȡ���̷���
	
	if(CFG_ACS(addr + (&CFG_ALL_PRI - CFG_THIS)) == 1){//��������Ϊ��������
		sys_cs = pos - 1;//��ѡ��Ϊ������
		DATA_CFG = (PUINT8C)addr;//ָ��ָ������
		DATA_LIGHT = DATA_LIGHT_BASE - sys_cs * 256;//�޸ĵ�Ч����ָ��
	}
	
	for(i = 0; i < 16; i++){
		keyAddr[pos - 1][i] = addr;//�洢��ַ
		if(CFG_K_ID(addr) != i + 1){//��ID����
			keyAddr[pos - 1][0] = 0;//�����Ա��Ϊ��Ч
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
			keyAddr[pos - 1][0] = 0;//�����Ա��Ϊ��Ч
			break;
		}
	}
}

void GlobalParaLoad(void){//ȫ�ֲ�����ȡ
	ANA_MID_SET[0] = GLOB_ANA_MID1;//ҡ����λ
	ANA_MID_SET[1] = GLOB_ANA_MID2;
	keyFltNum = GLOB_KEY_FLT;//�����˲�����
	EC1freq = GLOBb_EC_FREQ1;//��ť��Ƶ����
	EC2freq = GLOBb_EC_FREQ2;
}

void GlobalParaUpdate(void){//ȫ�ֲ�������
	((uint16_t*)(FlashBuf))[0] = ANA_MID_SET[0];//ҡ����λ
	((uint16_t*)(FlashBuf))[1] = ANA_MID_SET[1];
	FlashBuf[4] = keyFltNum;//�����˲�����
	FlashBuf[5] = (((uint8_t)EC2freq) << 1) | ((uint8_t)EC1freq);//��ť��Ƶ����
	ParaSave(100, 1);//��������
}





















