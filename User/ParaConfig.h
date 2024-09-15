#ifndef _PARACONFIG_H_
#define _PARACONFIG_H_

#include "CH549.H"
#include "DEBUG.H"

#include "FlashRom.H"
#include "BasicIO.h"
//#include "CompositeHID.H"

//��ǰ60k(0x0000~0xEFFF)ΪCodeFlash ����10k��
//����1k(0xF000~0xF3FF)ΪDataFlash ����Լ��100k��
//���3k(0xF400~0xFFFF)ΪBootLoader��ConfigInfo �û�����ʹ��
#define DATA_CFG_BASE		0xF200//����������ʼ
#define DATA_GLOB_BASE		0xE800//ȫ��������ʼ
#define DATA_LIGHT_BASE		0xE700//��Ч������ʼ
//��������ռCodeFlash�ĺ�1k��DataFlash��1k,ÿ��512B,�ռ�4��,Ŀǰʹ�ú�3��,�Ӻ���ǰ��
//ȫ������ռCodeFlash�ĵ�����2k�Ŀ�ͷ����,Ŀǰʹ��64B
//��Ч����ռCodeFlash�ĵ�����3k,ÿ��256B,�ռ�4��,Ŀǰʹ�ú�3��,�Ӻ���ǰ��
//Ŀǰȫ���洢����ʹ��4k�ռ�,ʣ�����ռ�Ϊ57k

#define CFG_NUM		3//���ø���

#define CFG_THIS		(DATA_CFG)//����������ʼ
#define CFG_ACS(a)			(*((PUINT8C)(a) + 0))//����ֱ�ӷ���
#define CFG_K_ID(a)			(*((PUINT8C)(a) + 0))//����ID
#define CFG_K_MODE(a)		(*((PUINT8C)(a) + 1))//����ģʽ
#define CFG_K_KEY(a)		(*((PUINT8C)(a) + 2))//������ֵ
#define CFG_K_LEN(a)		(*((PUINT8C)(a) + 2))//������ģʽʱ��������
#define CFG_K_FUNC(a)		(*((PUINT8C)(a) + 3))//�������ܼ�
#define CFG_K_M3DT(a)		(*((PUINT8C)(a) + 3))//������ģʽʱ������ʼ
#define CFG_K_X(a)			(*((PUINT16C)((a) + 2)))//���ģʽʱXֵ
#define CFG_K_Y(a)			(*((PUINT16C)((a) + 4)))//���ģʽʱYֵ
#define CFG_K_T(a)			(*((PUINT16C)((a) + 4)))//����ģʽʱ����
#define CFG_RGB_R			(*(CFG_THIS + 476))//RGB��ɫֵR
#define CFG_RGB_G			(*(CFG_THIS + 477))//RGB��ɫֵG
#define CFG_RGB_B			(*(CFG_THIS + 478))//RGB��ɫֵB
#define CFG_KB_DIR			(*(CFG_THIS + 479))//���̷���
#define CFG_SCN_W			(*((PUINT16C)(CFG_THIS + 480)))//��Ļ���
#define CFG_SCN_H			(*((PUINT16C)(CFG_THIS + 482)))//��Ļ�߶�
#define CFG_R_MODE			(*(CFG_THIS + 484))//ҡ��ģʽ������ģʽ
#define CFG_R_KEY(n)		(*(CFG_THIS + 485 + (n)))//ҡ�˰��� ����������
#define CFG_R_FUNC			(*(CFG_THIS + 490))//ҡ�˹��ܼ�
#define CFG_R_DIR			(*(CFG_THIS + 491))//ҡ�˷���
#define CFG_R_DEAD			(*(CFG_THIS + 492))//ҡ������
#define CFG_R_PARA			(*(CFG_THIS + 493))//ҡ�˲���
#define CFG_R_RSV1			(*(CFG_THIS + 494))//ҡ��rsv1
#define CFG_R_RSV2			(*(CFG_THIS + 495))//ҡ��rsv2
#define CFG_E_MODE(i)		(*(CFG_THIS + 496 + (i) * 6))//��ťģʽ������ģʽ
#define CFG_E_KEY(i,n)		(*(CFG_THIS + 497 + (i) * 6 + (n)))//��ť���� ����˳
#define CFG_E_FUNC(i)		(*(CFG_THIS + 500 + (i) * 6))//��ť���ܼ�
#define CFG_E_DIR(i)		(*(CFG_THIS + 501 + (i) * 6))//��ť����
#define CFG_RGB_CYCLE		(*(CFG_THIS + 508))//��ɫ���ںͺ�������
#define CFG_RGB_SET			(*(CFG_THIS + 509))//RGB������
#define CFG_RGB_LIGHT		(*(CFG_THIS + 510))//RGB������
#define CFG_ALL_PRI			(*(CFG_THIS + 511))//�����ȼ�

#define CFGb_R_DIRx			((CFG_R_DIR >> 0) & 1)//ҡ��X����
#define CFGb_R_DIRy			((CFG_R_DIR >> 1) & 1)//ҡ��Y����
#define CFGb_R_DIRr			((CFG_R_DIR >> 2) & 1)//ҡ��90��ת��
#define CFGb_Rk_MODE		(CFG_R_MODE >> 4)//ҡ�˰���ģʽ
#define CFGb_R_MODE			(CFG_R_MODE & 0x0F)//ҡ��ģʽ
#define CFGb_Ek_MODE(i)		(CFG_E_MODE(i) >> 4)//��ť����ģʽ
#define CFGb_E_MODE(i)		(CFG_E_MODE(i) & 0x0F)//��ťģʽ
#define CFGb_RGB_COLORFUL	(CFG_RGB_CYCLE >> 4)//��ɫ����
#define CFGb_RGB_WAVE		(CFG_RGB_CYCLE & 0x0F)//��������
#define CFGb_RGB_RK			((CFG_RGB_SET >> 7) & 1)//ҡ��ӳ��
#define CFGb_RGB_CLICKER	((CFG_RGB_SET >> 6) & 1)//����ָʾ
#define CFGb_RGB_LOOP		((CFG_RGB_SET >> 5) & 1)//ѭ��ָʾ
#define CFGb_RGB_TIME		(CFG_RGB_SET & 0x0F)//����ָʾʱ��

#define GLOB_THIS		(GLOB_CFG)//ȫ��������ʼ
#define GLOB_ANA_MID1		(*((PUINT16C)(GLOB_THIS + 0)))//ҡ����λ
#define GLOB_ANA_MID2		(*((PUINT16C)(GLOB_THIS + 2)))//ҡ����λ
#define GLOB_KEY_FLT		(*(GLOB_THIS + 4))//�����˲�ֵ


extern UINT8X FlashBuf[512];//���û�������
extern uint16_t keyAddr[CFG_NUM][16];//3��ÿ��16���������ݵ�ַ
extern uint16_t keyWork[16];//3��ÿ��16�����Ĺ���������
extern uint8_t keyFlag[16];//16�����ı��������
extern uint8_t key_turn[CFG_NUM];//���̷���
extern uint8_t sys_cs;//������ѡ��
extern PUINT8C DATA_CFG;//������������Ϣָ��


void ParaSave(uint8_t pos, uint8_t num);//��������
void ParaLoad();//������ȡ
void ParaUpdate(uint8_t pos);//��������
void GlobalParaLoad();//ȫ�ֲ�����ȡ
void GlobalParaUpdate();//ȫ�ֲ�������


#endif



