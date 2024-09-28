
#include "BasicIO.h"

extern uint8_t KeyBrd_data[];//���̱���

UINT8C turnL90[16] = {	3,	7,	11,	15,
						2,	6,	10,	14,
						1,	5,	9,	13,
						0,	4,	8,	12};//��������ӳ�����
UINT8C turnR90[16] = {	12,	8,	4,	0,
						13,	9,	5,	1,
						14,	10,	6,	2,
						15,	11,	7,	3};//��������ӳ�����

uint32_t Systime = 0;//ϵͳʱ��

uint8_t DebugBuf[64];

uint16_t adcValue[2] = {2048,2048};
uint16_t ANA_MID_SET[2] = {2048,2048};
UINT8D keyFltNum = 2;//�����˲�����

uint8_t keyRaw[KP_NUM];//����ԭʼ����
uint8_t keyFlt[KP_NUM];//�����˲����
uint8_t fltOld[KP_NUM];//�����˲��õľ�ֵ
uint8_t fltCount[KP_NUM];//�����˲�����

uint8_t keyNow[KP_NUM];//����ӳ����
uint8_t keyOld[KP_NUM];//����ӳ������ֵ

void arrayInit(void){//�����ʼ��
	//srand(*(PUINT16X)(2048 - 2));//��������
	memset(KeyBrd_data + 1, 0, 21);//��ʼ�����̱�������
	memset(DebugBuf, 0, 64);
	memset(keyFlt,0,KP_NUM);
	memset(fltOld,0,KP_NUM);
	memset(fltCount,0,KP_NUM);
	memset(keyNow,0,KP_NUM);
	memset(keyOld,0,KP_NUM);
}

void adcRead(void){//ҡ�˶�ȡ
	static uint8_t ch = 1;	//0��1 ����ͨ��0��2
	if(ADC_CTRL & bADC_IF){		//��ADC�������
		ADC_CTRL = bADC_IF;			//���־
		//adcValue[!ch] = ADC_DAT;	//��¼����ֵ
		//adcValue[!ch] = (ADC_DAT + adcValue[!ch]) >> 1;	//��¼����ֵ ���˲�
		adcValue[ch] += ((int16_t)ADC_DAT - (int16_t)adcValue[ch]) >> 2;	//��¼����ֵ ���˲�
		ch = !ch;					//�л�ѡ��
		ADC_ExChannelSelect(ch*2);	//ѡ��ͨ��ch 0��2
		ADC_StartSample();			//������һ��ADC����
	}
}

void keyRead(void){//������ȡ
	keyRaw[0] = !KP_1;		keyRaw[1] = !KP_2;		keyRaw[2] = !KP_3;		keyRaw[3] = !KP_4;
	keyRaw[4] = !KP_5;		keyRaw[5] = !KP_6;		keyRaw[6] = !KP_7;		keyRaw[7] = !KP_8;
	keyRaw[8] = !KP_9;		keyRaw[9] = !KP_10;		keyRaw[10] = !KP_11;	keyRaw[11] = !KP_12;
	keyRaw[12] = !KP_13;	keyRaw[13] = !KP_14;	keyRaw[14] = !KP_15;	keyRaw[15] = !KP_16;
	keyRaw[16] = !KP_E1;	keyRaw[17] = !KP_E2;
	keyRaw[18] = !KP_R;
}

void keyFilter(uint8_t ts){//�����˲�
	uint8_t i;
	for(i = 0; i < KP_NUM; i++){
		if(ts == 2){//��Ϊ�˲����׶�
			if(fltCount[i]) fltCount[i]--;//���˲�����δ������ݼ�
			if(fltOld[i] == keyRaw[i] && keyFlt[i] != keyRaw[i]
					&& fltCount[i] == 0){//������ͬ����֮ǰ��ͬ���˲������ѹ���
				keyFlt[i] = keyRaw[i];//�����˲����ֵ
				fltCount[i] = keyFltNum;//�����˲�����ֵ
			}
		}
		fltOld[i] = keyRaw[i];//�����˲���ֵ
	}
}

void keyTurn(void){//������תӳ��
	uint8_t i;
	memcpy(keyOld, keyNow, KP_NUM);//�洢��ֵ
	
	if(CFG_KB_DIR == 0){//��������
		memcpy(keyNow, keyFlt, 16);
	}
	else if(CFG_KB_DIR == 1){//����90��
		for(i = 0; i < 16; i++) keyNow[i] = keyFlt[turnR90[i]];
	}
	else if(CFG_KB_DIR == 2){//��ת180��
		for(i = 0; i < 16; i++) keyNow[i] = keyFlt[16 - i];
	}
	else if(CFG_KB_DIR == 3){//����90��
		for(i = 0; i < 16; i++) keyNow[i] = keyFlt[turnL90[i]];
	}
	memcpy(keyNow + 16, keyFlt + 16, 3);//ҡ����ť����ֱ�ӿ���
}

void GetTime(void){//ʱ���ȡ
	static UINT16D THTL0_old = 0;//��ʱ����ֵ
	UINT16D THTL0;//��ʱ��16λ����ֵ
	UINT8D incMs;//���ӵĺ�����
	THTL0 = (TH0 << 8) | TL0;//��ȡ��ʱ��
	if((THTL0 & 0x0F) < 6){//�����Ʒ�����λ
		TR0 = STOP;//��ͣ��ʱ��
		THTL0 = (TH0 << 8) | TL0;//���¶�ȡ��ʱ��
		TR0 = START;//������ʱ��
	}
	incMs = (uint16_t)(THTL0 - THTL0_old) >> 11 /*2000*/;//�������ӵĺ�����
//	incMs = (uint16_t)(THTL0 - THTL0_old) / 2000;//�������ӵĺ�����
	Systime += incMs;//ϵͳʱ�����
	THTL0_old += (uint16_t)incMs * 2000;//��ʱ����ֵ����
	//���Խ��: /2000*2000:8960  /2000<<11:9174  >>11<<11:9177  >>11*2000:8952
}

UINT16C toneTIM[] = {//������ʱ������ֵ��
	15296,14421,13611,12864,12139,11456,10816,10197,9621,9088,8576,8107,
	7637,7211,6805,6421,6059,5717,	5397,5099,4821,4544,4288,4053,
	3819,3605,3413,3221,3029,2859,2709,2560,2411,2283,2155,2027,
	1920,1813,1707,1600,1515,1429,1344,1280,1195,1131,1067,1003,
};
UINT8C tonePWM[] = {//����PWM��Ƶֵ��
	253,239,226,213,201,190,
	179,169,160,151,142,134,127,120,113,107,101,95,
	90,85,80,75,71,67,63,60,56,53,50,47,	45,
};
UINT8C toneTABLE[] = {29,31,33,35,255,24,26,28, 17,19,21,23,255,12,14,16,};//��λ����ӳ���

UINT16D buzzTimVol = 10, toneTimValue = 10000;//������ʱ������ֵ,��ʱֵ

void buzzHandle(void){//����������
	uint8_t i;//ѭ������
	uint8_t count = 0, effective = 0xFF;//���°�������,��Ч����
	uint8_t buzzTone = 0xFF, buzzToneOld = 0xFF;//����
	int8_t buzzVol = 1;//����
	
	PWM0OutPolarLowAct();//PWM0������
	
	while((!keyOld[18] || keyNow[18]) && (!keyOld[17] || keyNow[17]) && (!keyOld[16] || keyNow[16])){//ҡ�˻���ť���ͷ����˳�
		keyRead();//��ȡ����
		keyFilter(1);//�˲�һ�׶�
		keyRead();//�ٴζ�ȡ����
		keyFilter(2);//�˲����׶�
		keyTurn();//������תӳ��
		
		buzzVol += EC1val;//������1��������
		EC1val = EC2val = 0;
		if(buzzVol < 1){//��С����
			PWM_R = 255;//���ָʾ
			buzzVol = 1;
			mDelaymS(2);
			PWM_R = 0;
		}
		else if(buzzVol > 10){//�������
			PWM_G = 255;//�̵�ָʾ
			buzzVol = 10;
			mDelaymS(2);
			PWM_G = 0;
		}
		
		count = 0;
		for(i = 0; i < 16; i++){
			if(i == 4 || i == 12) continue;//�������˶ȼ�����������
			if(keyNow[i] && !keyOld[i]) effective = i;//�����ؿ�ʼ����
			if(keyNow[i]) count++;//���°�������
		}
		if(count == 0) effective = 0xFF;//ȫ���������ͷź������Ч����
		if(effective != 0xFF){//������Ч����
			if(!keyNow[effective]){//���˼��ѱ��ͷ�
				for(i = 0; i < 16; i++){//����ɨ��
					if(i == 4 || i == 12) continue;//�������˶ȼ�����������
					if(keyNow[i]) effective = i;//���¼�����
				}
			}
			buzzTone = toneTABLE[effective] + keyNow[12];//�Ӳ��ұ���Ϻڼ���������
			if(keyNow[4] && effective < 8) buzzTone += 12;//�ϰ벿���˶�
			else if(keyNow[4]) buzzTone -= 12;//�°벿���˶�
		}
		else buzzTone = 0xFF;//�������
		if(buzzToneOld == buzzTone) continue;//������δ�ı�������
		if(buzzTone < 18){//��Ƶ�ɶ�ʱ��ʵ��
			toneTimValue = toneTIM[buzzTone];
			buzzTimVol = (((uint32_t)toneTimValue * buzzVol) >> 8) / 3;
			PWM_SEL_CHANNEL(PWM_CH0, Disable);//PWM0���ʧ��
			ET0 = 1;//��ʱ��0�ж�ʹ��
		}
		else if(buzzTone <= 48){//��Ƶ��PWMʵ��
			SetPWMClkDiv(tonePWM[buzzTone - 18]);//PWMʱ�ӷ�Ƶ����
			BUZZ_PWM = buzzVol;
			ET0 = 0;//��ʱ��0�ж�ʧ��
			PWM_SEL_CHANNEL(PWM_CH0, Enable);//PWM0���ʹ��
		}
		else{//ֹͣ����
			ET0 = BUZZ_PWM = 0;//�ض�ʱ���жϼ�����PWMռ�ձ�
			PWM_SEL_CHANNEL(PWM_CH0, Disable);//PWM0���ʧ��
		}
		buzzToneOld = buzzTone;//��¼����
	}
	ET0 = BUZZ_PWM = 0;//�ض�ʱ���жϼ�����PWMռ�ձ�
	PWM_SEL_CHANNEL(PWM_CH0, Disable);//PWM0���ʧ��
	SetPWMClkDiv(32);//�ָ�ԭPWMʱ�ӷ�Ƶ
}

//uint8_t keyOldTest[] = {1,1,1};
uint32_t oldTime = 0;

void LL_test(void){
	static uint16_t i;
	i++;
	if(Systime - oldTime >= 1000){//�˵�2��ӡ���
		oldTime += 1000;
		memset(DebugBuf, ' ', 64);
		sprintf(DebugBuf, "%d	%d	%u\n", adcValue[0], adcValue[1], (uint16_t)Systime);
		
		Enp2IntIn(DebugBuf, 64);
	}

//	mDelaymS(10);
}

void multiFunc(void){//���ܼ��Ϻ���
//	LL_test();//���Դ���
	keyTurn();//������תӳ��
	
	if(Fill_report() == 1){//������д �����ط�����ģʽ
		clearKeyRGB();//�������RGB
		WS_Write_16();//��д��
		buzzHandle();//����������
	}
	
	sysRGB();//ϵͳRGB����
	keyRGB(0);//����RGB����
//	if(WakeUpEnFlag & 1) PWM_R = 100;
//	else PWM_R = 0;
//	if(WakeUpEnFlag & 2) PWM_G = 100;
//	else PWM_G = 0;
}













