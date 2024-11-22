
#include "Diag.h"

void DiagRead(uint8_t *buf, uint8_t len){//������ݶ�ȡ
	memcpy(buf, (PUINT8C)DATA_DIAG_BASE, len);
}

void DiagFormat(uint8_t setting){//������ݸ�ʽ��
	if(setting & DIAG_FMT_LOAD) DiagRead(DIAG_THIS, 64);//������������ڴ�
	if(setting & DIAG_FMT_MARK) DIAG_CNT_MARK = 0;//����־����һͬ��0
	memset(DIAG_THIS + 2, 0, 56 - 2 + 2);
	DIAG_SUM = ~DIAG_CNT_MARK;//����У��
	DIAG_MARK = DIAG_MARK_VALUE;//���ñ�־
	if(setting & DIAG_FMT_SAVE) ;//��������
}

uint16_t DiagSum(uint8_t *buf, uint8_t len){//������ݼ���У��
	uint16_t sum = 0;
	do{ sum += buf[len--]; }while(len);
	return sum;
}

void DiagInit(void){//��ϳ�ʼ������
	uint8_t tmp = 0;
	DiagRead(DIAG_THIS, 64);//������������ڴ�
	if(DIAG_MARK != DIAG_MARK_VALUE){//��־����
		DiagFormat(DIAG_FMT_NONE);//������ݸ�ʽ��
	}
	if(DIAG_RST_FLAG == RST_REASON_WDG){//������Ϊ���Ź���λ
		if(DIAG_RST_LAST){//���ϴ�Ҳ�ǿ��Ź���λ
			if(DIAG_RST_CNT == 0){//��Ϊ�·������������Ź���λ
				//�������������Ź���λ����+1
			}
			DIAG_RST_CNT_SET(7);//���ü���ֵ
		}
		else if(DIAG_RST_CNT == 0){//���ϴηǿ��Ź���λ �� ����Ϊ0
			//�����ÿ��Ź���λ����+1
		}
		DIAG_RST_LAST_SET(1);//��¼Ϊ���Ź���λ
	}
	else if(DIAG_RST_CNT > 0){//�����ηǿ��Ź���λ �� ������0
		DIAG_RST_CNT_DEC;//�����ݼ�
		DIAG_RST_LAST_SET(0);//��¼Ϊ�ǿ��Ź���λ
	}
	
}

void DiagCountInc(uint8_t pos, uint8_t fail){//���������������
	uint16_t addr = DATA_DIAG_BASE + 128 * pos;
	if(fail){//�����������
		
		return;
	}
	if(*(PUINT16C)(addr + 128 - 2) != 0x5A10){//��־�벻�Ϸ�
		
	}
}























