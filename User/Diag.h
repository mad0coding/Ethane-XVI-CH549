#ifndef _DIAG_H_
#define _DIAG_H_


#include "ParaConfig.h"

//��λԭ��
#define RST_REASON_SW		0x00//��λ
#define RST_REASON_PWR		0x01//��Դ��λ
#define RST_REASON_WDG		0x02//���Ź���λ
#define RST_REASON_PIN		0x03//���Ÿ�λ

//��ʽ������
#define DIAG_FMT_NONE		0x00//��ͨ��ʽ��
#define DIAG_FMT_LOAD		0x01//��ʽ��ǰ�ȼ�������
#define DIAG_FMT_SAVE		0x02//��ʽ���󱣴�����
#define DIAG_FMT_MARK		0x10//��ʽ����־�������

#define DIAG_MARK_VALUE		0xEA16C549//������ݱ�־

#define DIAG_THIS		(FlashBuf)//���������ʼ ʹ�û����ַ
#define DIAG_CNT(i)			(*(PUINT16X)(DIAG_THIS + 2*i))
#define DIAG_CNT_MARK			(*(PUINT16X)(DIAG_THIS + 0))
#define DIAG_CNT_SUM			(*(PUINT16X)(DIAG_THIS + 2))
#define DIAG_CNT_WDG			(*(PUINT16X)(DIAG_THIS + 8))
#define DIAG_CNT_WDGS			(*(PUINT16X)(DIAG_THIS + 10))
#define DIAG_SUM				(*(PUINT16X)(DIAG_THIS + 58))
#define DIAG_MARK				(*(PUINT32X)(DIAG_THIS + 60))

#define DIAG_KEEP		(RESET_KEEP)//�ӹܸ�λ���ּĴ���
#define DIAG_RST_FLAG			((PCON >> 4) & 0x03)//���θ�λ��־
#define DIAG_RST_LAST			((DIAG_KEEP >> 3) & 0x01)//�ϴθ�λ��־
#define DIAG_RST_CNT			(DIAG_KEEP & 0x07)//������λ��־
#define DIAG_RST_LAST_SET(x)	(DIAG_KEEP = (DIAG_KEEP & ~(0x01 << 3)) | ((x) << 3))
#define DIAG_RST_CNT_SET(x)		(DIAG_KEEP = (DIAG_KEEP & ~0x07 | (x)))
#define DIAG_RST_CNT_DEC		(DIAG_KEEP--)//�����ݼ� ʹ��ʱע�����



void FlashCountInc(uint8_t pos, uint8_t fail);		//���������������
uint32_t FlashCountGet(uint8_t pos, uint8_t fail);	//�������������ȡ

void DiagRead(uint8_t *buf, uint8_t len);//������ݶ�ȡ


#endif



