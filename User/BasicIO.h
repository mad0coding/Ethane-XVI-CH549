#ifndef _BASICIO_H_
#define _BASICIO_H_

#include "CH549.H"
#include "DEBUG.H"

#include "ADC.H"
#include "PWM.H"
#include "Timer.H"

#include "CompositeHID.H"
#include "RgbDrv.h"
#include "ParaUse.h"

#define KB_LEN		22//���̱��ĳ���
#define KP_NUM		19//��������

#define KP_1	P1_3
#define KP_2	P3_0
#define KP_3	P3_3
#define KP_4	P4_6
#define KP_5	P1_4
#define KP_6	P4_5
#define KP_7	P3_4
#define KP_8	P4_2
#define KP_9	P1_6
#define KP_10	P4_4
#define KP_11	P3_5
#define KP_12	P4_3
#define KP_13	P1_7
#define KP_14	P3_1
#define KP_15	P3_2
#define KP_16	P3_6

#define KP_E1	P2_6
#define KP_E2	P0_0
#define KP_R	P1_5

#define KP_E1A	P2_4
#define KP_E1B	P2_2//INT0_
#define KP_E1C	P3_7//INT3
#define KP_E2A	P0_1
#define KP_E2B	P0_3
#define KP_E2C	P5_7

#define BUZZ_G		P2_5//����������դ��
#define BUZZ_PWM	PWM_DATA0//����������ռ�ձ�

#define ANA_MID_0	ANA_MID_SET[0]
#define ANA_MID_1	ANA_MID_SET[1]

extern uint32_t Systime;//ϵͳʱ��

extern UINT8C turnL90[16];//��������ӳ�����
extern UINT8C turnR90[16];//��������ӳ�����
extern UINT16D toneTimValue;//������ʱ������ֵ
extern UINT16D buzzTimVol;//������ʱ����ʱֵ

extern UINT8D EC1val, EC2val;
extern bit EC1freq, EC2freq;

extern uint16_t adcValue[];
extern uint16_t ANA_MID_SET[];
extern UINT8D keyFltNum;//�����˲�����

extern uint8_t keyNow[KP_NUM];//����ӳ����
extern uint8_t keyOld[KP_NUM];//����ӳ������ֵ
//extern uint8_t keyFlt[KP_NUM];
extern uint8_t DebugBuf[];


void arrayInit(void);//�����ʼ��
void adcRead(void);//ҡ�˶�ȡ
void keyRead(void);//������ȡ
void keyFilter(uint8_t ts);//�����˲�

void getTime(void);//ʱ���ȡ

void LL_test(void);

void multiFunc(void);//���ܼ��Ϻ���


#endif




