
#include "BasicIO.h"

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

uint8_t keyRaw[KP_num];//����ԭʼ����
uint8_t keyFlt[KP_num];//�����˲����
uint8_t fltOld[KP_num];//�����˲��õľ�ֵ
uint8_t fltCount[KP_num];//�����˲�����

uint8_t keyNow[KP_num];//����ӳ����
uint8_t keyOld[KP_num];//����ӳ������ֵ

void arrayInit(){//�����ʼ��
//	uint8_t seed;	srand(seed);//��������
	memset(keyFlt,0,KP_num);
	memset(fltOld,0,KP_num);
	memset(fltCount,0,KP_num);
	memset(keyNow,0,KP_num);
	memset(keyOld,0,KP_num);
}

void adcRead(uint8_t ch){//ҡ�˶�ȡ
	if(ADC_CTRL&bADC_IF){		//��ADC�������
		ADC_CTRL = bADC_IF;			//���־
		adcValue[!ch] = ADC_DAT;	//��¼����ֵ
		ADC_ExChannelSelect(ch);	//ѡ��ͨ��ch
		ADC_StartSample();			//����һ��ADC����
	}
}

void keyRead(){//������ȡ
	keyRaw[0] = !KP_1;		keyRaw[1] = !KP_2;		keyRaw[2] = !KP_3;		keyRaw[3] = !KP_4;
	keyRaw[4] = !KP_5;		keyRaw[5] = !KP_6;		keyRaw[6] = !KP_7;		keyRaw[7] = !KP_8;
	keyRaw[8] = !KP_9;		keyRaw[9] = !KP_10;		keyRaw[10] = !KP_11;	keyRaw[11] = !KP_12;
	keyRaw[12] = !KP_13;	keyRaw[13] = !KP_14;	keyRaw[14] = !KP_15;	keyRaw[15] = !KP_16;
	keyRaw[16] = !KP_E1;	keyRaw[17] = !KP_E2;
	keyRaw[18] = !KP_R;
}

void keyFilter(uint8_t ts){//�����˲�
	uint8_t i;
	for(i = 0; i < KP_num; i++){
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
	memcpy(keyOld, keyNow, KP_num);//�洢��ֵ
	
	if(CFG_KB_DIR == 0){//��������
		memcpy(keyNow, keyFlt, 16);
	}
	else if(CFG_KB_DIR == 1){//����90��
		for(i = 0; i < 16; i++){
			keyNow[i] = keyFlt[turnR90[i]];
		}
	}
	else if(CFG_KB_DIR == 2){//��ת180��
		for(i = 0; i < 16; i++){
			keyNow[i] = keyFlt[16 - i];
		}
	}
	else if(CFG_KB_DIR == 3){//����90��
		for(i = 0; i < 16; i++){
			keyNow[i] = keyFlt[turnL90[i]];
		}
	}
	memcpy(keyNow + 16, keyFlt + 16, 3);
}

void GetTime(){//ʱ���ȡ
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
	Systime += incMs;//ϵͳʱ�����
	THTL0_old += (uint16_t)incMs * 2000;//��ʱ����ֵ����
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

void buzzHandle(){//����������
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
			buzzTone = toneTABLE[effective] + keyNow[12];//�Ӳ��ұ�����Ϻڼ���������
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

#if 0
uint8_t Buzz_handle()//������(4���˶ȵĵ�����)
{
	static uint8_t effective = 0;//��Ч����
	static int8_t vol = 10;//����
	uint8_t count = 0;//���°�������
	uint8_t ratio_up = 4, ratio_down = 2;//���������˶ȵı�Ƶֵ
	uint16_t tone_arr = 0;
	
	if(!keyNow[17] && keyOld[17]){
		TIM_Cmd(TIM1, DISABLE);//�ر�TIM1������
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);//�ر�TIM1ʱ��
		return 0;//ҡ�˰����ͷ����˳�
	}
	
	if(vol < 30) vol += EC_monitor(0);//��������
	else if(vol < 70) vol += EC_monitor(0) * 2;
	else vol += EC_monitor(0) * 3;
	if(vol < 0){//��С����
		vol = 0;
		Light_handle(2,10);
	}
	else if(vol > 100){//�������
		vol = 100;
		Light_handle(3,10);
	}
	
	if(keyNow[5]){//�������˶�
		ratio_up = 8;//���������˶�
		ratio_down = 1;//���������˶�
	}
	for(uint8_t i = 1; i <= 16; i++){
		if(i == 5 || i == 13) continue;//�������˶ȼ�����������
		if(key_state[i] && !key_old[i]) effective = i;//�����ؿ�ʼ����
		if(key_state[i]) count++;//���°�������
	}
	if(count == 0) effective = 0;//ȫ���������ͷź������Ч����
	if(effective == 0) TIM1->CCR1 = 0;//����Ч������ֹͣ����
	else{
		if(key_state[13]){//�ڼ�(ȫ��������)
			switch(effective){
				case 1:tone_arr = tone[6]/ratio_up;break;//+4#
				case 2:tone_arr = tone[8]/ratio_up;break;//+5#
				case 3:tone_arr = tone[10]/ratio_up;break;//+6#
				case 4:tone_arr = tone[0]/ratio_up/2;break;//+7#(++1)
				case 6:tone_arr = tone[1]/ratio_up;break;//+1#
				case 7:tone_arr = tone[3]/ratio_up;break;//+2#
				case 8:tone_arr = tone[5]/ratio_up;break;//+3#(+4)
				case 9:tone_arr = tone[6]/ratio_down;break;//4#
				case 10:tone_arr = tone[8]/ratio_down;break;//5#
				case 11:tone_arr = tone[10]/ratio_down;break;//6#
				case 12:tone_arr = tone[0]/ratio_down/2;break;//7#(+1)
				case 14:tone_arr = tone[1]/ratio_down;break;//1#
				case 15:tone_arr = tone[3]/ratio_down;break;//2#
				case 16:tone_arr = tone[5]/ratio_down;break;//3#(4)
				default:tone_arr = 0;break;
			}
		}
		else{//�׼�
			switch(effective){
				case 1:tone_arr = tone[5]/ratio_up;break;//+4
				case 2:tone_arr = tone[7]/ratio_up;break;//+5
				case 3:tone_arr = tone[9]/ratio_up;break;//+6
				case 4:tone_arr = tone[11]/ratio_up;break;//+7
				case 6:tone_arr = tone[0]/ratio_up;break;//+1
				case 7:tone_arr = tone[2]/ratio_up;break;//+2
				case 8:tone_arr = tone[4]/ratio_up;break;//+3
				case 9:tone_arr = tone[5]/ratio_down;break;//4
				case 10:tone_arr = tone[7]/ratio_down;break;//5
				case 11:tone_arr = tone[9]/ratio_down;break;//6
				case 12:tone_arr = tone[11]/ratio_down;break;//7
				case 14:tone_arr = tone[0]/ratio_down;break;//1
				case 15:tone_arr = tone[2]/ratio_down;break;//2
				case 16:tone_arr = tone[4]/ratio_down;break;//3
				default:tone_arr = 0;break;
			}
		}
//		if(tone_arr > 0){
//			TIM1->ARR = tone_arr - 1;
//			TIM1->CCR1 = tone_arr * vol / 1000;
//		}
//		else TIM1->CCR1 = 0;
	}
	
	return 3;
}
#endif
//uint8_t keyOldTest[] = {1,1,1};
uint32_t oldTime = 0;
uint16_t THTL0old,THTL0;
//PUINT8C DATA_CFG1 = DATA_CFG_BASE;

void LL_test(){
	static uint16_t i;
	i++;
	if(Systime - oldTime >= 1000){//�˵�2��ӡ���
		oldTime += 1000;
//		memset(DebugBuf,0,64);
		DebugBuf[0] = TH0;
		DebugBuf[1] = TL0;
//		GetTime();
//		keyRead();
//		keyFilter(2);
//		WS_Write_16();
		DebugBuf[2] = TH0;
		DebugBuf[3] = TL0;
//		DebugBuf[4] = oldTime >> 24;
//		DebugBuf[5] = oldTime >> 16;
//		DebugBuf[6] = oldTime >> 8;
//		DebugBuf[7] = oldTime;
//		DebugBuf[0] = adcValue[0] >> 8;
//		DebugBuf[1] = adcValue[0];
//		DebugBuf[2] = adcValue[1] >> 8;
//		DebugBuf[3] = adcValue[1];
		THTL0 = CFG_SCN_W;
		DebugBuf[5] = keyAddr[0][0] >> 8;
		DebugBuf[6] = keyAddr[0][0];
		DebugBuf[7] = keyAddr[1][0] >> 8;
		DebugBuf[8] = keyAddr[1][0];
		DebugBuf[9] = i >> 8;
		DebugBuf[10] = i;
		
		Enp2IntIn(DebugBuf,64);
	}

//	mDelaymS(10);
}

void multiFunc(){//���ܼ��Ϻ���

	keyTurn();//������תӳ��
	
	if(Fill_report() == 1){//������д
		buzzHandle();//����������
	}
	
	sysRGB();//ϵͳRGB����
	keyRGB(0);//����RGB����
//	if(WakeUpEnFlag & 1) PWM_R = 100;
//	else PWM_R = 0;
//	if(WakeUpEnFlag & 2) PWM_G = 100;
//	else PWM_G = 0;
}












