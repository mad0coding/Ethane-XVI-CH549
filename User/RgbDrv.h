#ifndef _RGBDRV_H_
#define _RGBDRV_H_

#include "CH549.H"
#include "DEBUG.H"
#include <intrins.h>
#include <stdlib.h>

#include "BasicIO.h"
#include "ParaUse.h"


#define WS_DOUT		P2_7
#define PWM_R		PWM_DATA5
#define PWM_G		PWM_DATA2
#define PWM_B		PWM_DATA4

#define colorAngle	167//ɫ����1/6

#define LIGHT_THIS		(DATA_LIGHT)

//#define LIGHT_ACS(a)		(*((PUINT8C)(a) + 0))

#define LIGHT_UP(n)			(*(LIGHT_THIS + (n) + 0))//��������ɫ
#define LIGHT_DOWN(n)		(*(LIGHT_THIS + (n) + 3*16))//��������ɫ
#define LIGHT_T1(n)			(*(LIGHT_THIS + (n) + 6*16))//�������µ��ӳ�
#define LIGHT_T2(n)			(*(LIGHT_THIS + (n) + 7*16))//����̧����ӳ�
#define LIGHT_SHLD(n)		(*(LIGHT_THIS + (n) + 8*16))//��������ʣ��
#define LIGHT_IDX(n)		(*(LIGHT_THIS + (n) + 9*16))//�����Զ����±�
#define LIGHT_MONO			(*(LIGHT_THIS + 244))//���ö�ռ
#define LIGHT_DIR			(*(LIGHT_THIS + 245))//������������
#define LIGHT_WAVE			(*(LIGHT_THIS + 246))//����ģʽ����
#define LIGHT_COLORFUL		(*(LIGHT_THIS + 247))//��ɫ����(s)
#define LIGHT_D1WAVE		(*((PUINT16C)(LIGHT_THIS + 248)))//����ʱ��(ms)
#define LIGHT_D2WAVE		(*((PUINT16C)(LIGHT_THIS + 250)))//���ʱ��(ms)
#define LIGHT_T1WAVE		(*(LIGHT_THIS + 252))//���ӳ�
#define LIGHT_T2WAVE		(*(LIGHT_THIS + 253))//���ӳ�
#define LIGHT_T1SYS			(*(LIGHT_THIS + 254))//�������ӳ�
#define LIGHT_T2SYS			(*(LIGHT_THIS + 255))//�������ӳ�


extern PUINT8C DATA_LIGHT;//��������Ч��Ϣָ��

void clearKeyRGB(void);//�������RGB
void keyRGB(uint8_t clear);//����RGB����

void rgbToHSV(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV);//RGBתHSV
void hsvToRGB(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB);//HSVתRGB

void wsWrite16(void);//д��16����

void sysRGB();//ϵͳRGB����





#endif

