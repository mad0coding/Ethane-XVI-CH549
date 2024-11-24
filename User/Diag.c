
#include "Diag.h"

void DiagGet(uint8_t *buf, uint8_t len){//������ݻ�ȡ ����ӿ�
	memcpy(buf, (PUINT8C)DATA_DIAG_BASE, len);
}

static void DiagRead(uint8_t *buf){//������ݶ�ȡ
	memcpy(buf, (PUINT8C)DATA_DIAG_BASE, 64);
}

static void DiagWrite(uint8_t *buf){//�������д��
	ParaWrite(DATA_DIAG_BASE, buf, 1);//����д��
}

static uint16_t DiagSum(uint8_t *buf, uint8_t len){//������ݼ���У��
	uint16_t sum = 0;
	do{ sum += buf[len--]; }while(len);
	return ~sum;//ȡ��
}

static void DiagFormat(uint8_t setting){//������ݸ�ʽ��
	if(setting & DIAG_FMT_LOAD) DiagRead(DIAG_THIS);//������������ڴ�
	if(setting & DIAG_FMT_MARK) DIAG_CNT(DIAG_I_MARK) = 0;//����־����һͬ��0
	else DIAG_CNT(DIAG_I_MARK)++;//��־�������+1
	memset(DIAG_THIS + 2, 0, 56 - 2 + 2);//����
	DIAG_LAST = DIAG_I_MARK;//��¼������
	DIAG_MARK = DIAG_MARK_VALUE;//���ñ�־
	DIAG_SUM = DiagSum(DIAG_THIS, 56);//����У��
	if(setting & DIAG_FMT_SAVE) DiagWrite(DIAG_THIS);//��������
}

void DiagCountInc(uint8_t setting, uint8_t pos){//��ϼ�������
//	if(pos >= 28) return;//Խ��
//	if(setting & DIAG_FMT_LOAD) DiagRead(DIAG_THIS);//������������ڴ�
//	DIAG_CNT(pos)++;//����+1
//	DIAG_LAST = pos;//��¼������
//	DIAG_SUM = DiagSum(DIAG_THIS, 56);//����У��
//	if(setting & DIAG_FMT_SAVE) DiagWrite(DIAG_THIS);//��������
}

void DiagInit(void){//��ϳ�ʼ������
	uint8_t needSave = 0;
	DiagRead(DIAG_THIS);//������������ڴ�
	if(DIAG_MARK != DIAG_MARK_VALUE){//��־����
		DiagFormat(DIAG_FMT_NONE);//������ݸ�ʽ��
		needSave = 1;
	}
	if(DIAG_SUM != DiagSum(DIAG_THIS, 56)){//У�����
		DIAG_SUM = DiagSum(DIAG_THIS, 56);//����У��
		DiagCountInc(DIAG_FMT_NONE, DIAG_I_SUM);//��ϼ�������
		needSave = 1;
	}
	if(DIAG_RST_FLAG == RST_REASON_WDG){//������Ϊ���Ź���λ
		if(DIAG_RST_LAST){//���ϴ�Ҳ�ǿ��Ź���λ
			if(DIAG_RST_CNT == 0){//��Ϊ�·������������Ź���λ
				DiagCountInc(DIAG_FMT_NONE, DIAG_I_WDGS);//��ϼ�������
				needSave = 1;
			}
			DIAG_RST_CNT_SET(7);//���ü���ֵ
		}
		else if(DIAG_RST_CNT == 0){//���ϴηǿ��Ź���λ �� ����Ϊ0
			DiagCountInc(DIAG_FMT_NONE, DIAG_I_WDG);//��ϼ�������
			needSave = 1;
		}
		DIAG_RST_LAST_SET(1);//��¼Ϊ���Ź���λ
	}
	else{//�����ηǿ��Ź���λ
		if(DIAG_RST_CNT > 0) DIAG_RST_CNT_DEC;//������0������ݼ�
		DIAG_RST_LAST_SET(0);//��¼Ϊ�ǿ��Ź���λ
	}
	if(needSave) DiagWrite(DIAG_THIS);//�������д������
}

























