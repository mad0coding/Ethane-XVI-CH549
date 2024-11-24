#ifndef _PARACONFIG_H_
#define _PARACONFIG_H_

#include "CH549.H"
#include "DEBUG.H"

#include "FlashRom.H"
#include "BasicIO.h"
#include "Diag.h"
//#include "CompositeHID.H"

//�첽��־����
#define ASYNC_FLAG_CFG		10
#define ASYNC_FLAG_LIGHT	30
#define ASYNC_FLAG_GLOB		50
#define ASYNC_FLAG_SRST		100
#define ASYNC_FLAG_BOOT		101

//�洢λ�ö���
//#define FLASH_POS_CFG		10
//#define FLASH_POS_LIGHT		10
//#define FLASH_POS_GLOB		100
//#define FLASH_POS_DIAG		150
//#define FLASH_POS_ZERO		200


//��ǰ60k(0x0000~0xEFFF)ΪCodeFlash ����10k��
//����1k(0xF000~0xF3FF)ΪDataFlash ����Լ��100k��
//���3k(0xF400~0xFFFF)ΪBootLoader��ConfigInfo �û�����ʹ��
#define DATA_CFG_BASE		0xF200//����������ʼ
#define DATA_LIGHT_BASE		0xDC00//��Ч������ʼ
#define DATA_GLOB_BASE		0xDA00//ȫ��������ʼ
#define DATA_DIAG_BASE		0xD800//���������ʼ
//��������ռCodeFlash�ĺ�3k��DataFlash��1k,ÿ��512B,�ռ�8��,Ŀǰʹ�ú�4��,�Ӻ���ǰ��
//��Ч����ռCodeFlash�ĵ�����4~5k,ÿ��256B,�ռ�8��,Ŀǰʹ��ǰ4��,��ǰ������
//ȫ������ռCodeFlash�ĵ�����6k�ĺ�һ��Ŀ�ͷ����,Ŀǰʹ��64B
//�������ռCodeFlash�ĵ�����6k��ǰһ��Ŀ�ͷ����,Ŀǰʹ��64B
//Ŀǰȫ���洢����ʹ��7k�ռ�,ʣ�����ռ�Ϊ54k

#define CFG_NUM		8	//���ø���

//��������
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

#define CFG_R_DATA_S		(CFG_THIS + 468)//ҡ��������ʼ
#define CFG_R_DATA_L		(12)//ҡ�����ݳ���
#define CFG_R_MODE(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 0))//ҡ��ģʽ������ģʽ
#define CFG_R_DIR(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 1))//ҡ�˷���
#define CFG_R_KEY(i,n)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 2 + (n)))//ҡ�˰��� ����������
#define CFG_R_FUNC(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 7))//ҡ�˹��ܼ�
#define CFG_R_DEAD(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 8))//ҡ������
#define CFG_R_PARA(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 9))//ҡ�˲���
#define CFG_R_NEER(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 10))//ҡ�˽��˲���
#define CFG_R_FAR(i)		(*(CFG_R_DATA_S + (i) * CFG_R_DATA_L + 11))//ҡ��Զ�˲���

#define CFG_E_DATA_S		(CFG_THIS + 480)//��ť������ʼ
#define CFG_E_DATA_L		(8)//��ť���ݳ���
#define CFG_E_MODE(i)		(*(CFG_E_DATA_S + (i) * CFG_E_DATA_L + 0))//��ťģʽ������ģʽ
#define CFG_E_DIR(i)		(*(CFG_E_DATA_S + (i) * CFG_E_DATA_L + 1))//��ť����
#define CFG_E_KEY(i,n)		(*(CFG_E_DATA_S + (i) * CFG_E_DATA_L + 2 + (n)))//��ť���� ����˳
#define CFG_E_FUNC(i,n)		(*(CFG_E_DATA_S + (i) * CFG_E_DATA_L + 5 + (n)))//��ť���ܼ� ����˳

#define CFG_RGB_RGB(i)		(*(CFG_THIS + 496 + (i)))//RGB�Ʊ�ǩɫֵRGB
#define CFG_RGB_SIGN(i)		(*(CFG_THIS + 499 + (i)))//RGB��ָʾɫֵRGB
#define CFG_RGB_CFG(i)		(*(CFG_THIS + 502 + (i)))//RGB��ָʾ������
#define CFG_RGB_TIME		(*(CFG_THIS + 505))//RGB�Ʊ�ǩʱ��

#define CFG_SCN_W			(*((PUINT16C)(CFG_THIS + 506)))//��Ļ���
#define CFG_SCN_H			(*((PUINT16C)(CFG_THIS + 508)))//��Ļ�߶�
#define CFG_KB_DIR			(*(CFG_THIS + 510))//���̷���
#define CFG_ALL_PRI			(*(CFG_THIS + 511))//�����ȼ�

//�������ñ���
#define CFGb_R_DIRx(i)		((CFG_R_DIR(i) >> 0) & 1)//ҡ��X����
#define CFGb_R_DIRy(i)		((CFG_R_DIR(i) >> 1) & 1)//ҡ��Y����
#define CFGb_R_DIRr(i)		((CFG_R_DIR(i) >> 2) & 1)//ҡ��90��ת��
#define CFGb_Rk_MODE(i)		(CFG_R_MODE(i) >> 4)//ҡ�˰���ģʽ
#define CFGb_R_MODE(i)		(CFG_R_MODE(i) & 0x0F)//ҡ��ģʽ
#define CFGb_Ek_MODE(i)		(CFG_E_MODE(i) >> 4)//��ť����ģʽ
#define CFGb_E_MODE(i)		(CFG_E_MODE(i) & 0x0F)//��ťģʽ

#define CFGb_RGB_DIR(i)		(CFG_RGB_CFG(i) >> 7)//RGB��ָʾ�Ʒ���
#define CFGb_RGB_MAP(i)		(CFG_RGB_CFG(i) & 0x0F)//RGB��ָʾ��ӳ��
#define CFGb_RGB_T_ON		(CFG_RGB_TIME >> 4)//RGB�Ʊ�ǩ��ʱ��
#define CFGb_RGB_T_OFF		(CFG_RGB_TIME & 0x0F)//RGB�Ʊ�ǩ�����ٶ�



//ȫ������
#define GLOB_THIS		((PUINT8C)DATA_GLOB_BASE)//ȫ��������ʼ
#define GLOB_ANA_MID1		(*((PUINT16C)(GLOB_THIS + 0)))//ҡ����λ
#define GLOB_ANA_MID2		(*((PUINT16C)(GLOB_THIS + 2)))//ҡ����λ
#define GLOB_KEY_FLT		(*(GLOB_THIS + 4))//�����˲�ֵ
#define GLOB_EC_FREQ		(*(GLOB_THIS + 5))//��ť��Ƶ
//ȫ�����ñ���
#define GLOBb_EC_FREQ1		((GLOB_EC_FREQ >> 0) & 1)//һ����ť��Ƶ
#define GLOBb_EC_FREQ2		((GLOB_EC_FREQ >> 1) & 1)//������ť��Ƶ


extern UINT8X FlashBuf[512];//���û�������
extern uint16_t keyAddr[CFG_NUM][16];//3��ÿ��16���������ݵ�ַ
extern uint16_t keyWork[16];//3��ÿ��16�����Ĺ���������
extern uint8_t keyFlag[16];//16�����ı��������
extern uint8_t keyDir[CFG_NUM];//���̷���
extern uint8_t sysCs;//������ѡ��
extern PUINT8C DATA_CFG;//������������Ϣָ��

void AsyncHandle(uint8_t flag);//�첽����
void ParaSave(uint8_t pos, uint8_t num);	//��������
uint8_t ParaWrite(uint16_t addr, uint8_t *buf, uint8_t num);	//����д��
void ParaLoad(void);					//������ȡ
void ParaUpdate(uint8_t pos);			//��������
void GlobalParaLoad(void);		//ȫ�ֲ�����ȡ
void GlobalParaUpdate(void);	//ȫ�ֲ�������


#endif



