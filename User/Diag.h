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
#define DIAG_CNT(i)				(*(PUINT16X)(DIAG_THIS + 2*i))
#define DIAG_I_MARK				(0)//��־����
#define DIAG_I_SUM				(1)//У�����
#define DIAG_I_WDG				(4)//���Ź���λ
#define DIAG_I_WDGS				(5)//�������Ź���λ
#define DIAG_I_CFG				(8)//���ô洢������ʼ
#define DIAG_I_LIGHT			(16)//��Ч�洢������ʼ
#define DIAG_I_GLB				(24)//ȫ�ִ洢����
#define DIAG_LAST				(*(PUINT8X)(DIAG_THIS + 54))//������
#define DIAG_SUM				(*(PUINT16X)(DIAG_THIS + 58))//У��
#define DIAG_MARK				(*(PUINT32X)(DIAG_THIS + 60))//��־

#define DIAG_KEEP		(RESET_KEEP)//�ӹܸ�λ���ּĴ���
#define DIAG_RST_FLAG			((PCON >> 4) & 0x03)//���θ�λ��־
#define DIAG_RST_LAST			((DIAG_KEEP >> 3) & 0x01)//�ϴθ�λ��־
#define DIAG_RST_CNT			(DIAG_KEEP & 0x07)//������λ����
#define DIAG_RST_LAST_SET(x)	(DIAG_KEEP = (DIAG_KEEP & ~(0x01 << 3)) | ((x) << 3))
#define DIAG_RST_CNT_SET(x)		(DIAG_KEEP = (DIAG_KEEP & ~0x07 | (x)))
#define DIAG_RST_CNT_DEC		(DIAG_KEEP--)//�����ݼ� ʹ��ʱע�����


void DiagGet(uint8_t *buf, uint8_t len);//������ݻ�ȡ
void DiagCountInc(uint8_t setting, uint8_t pos);//��ϼ�������



#endif



