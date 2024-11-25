
//ȫ�����۵ƹ���576��ʵ���ܹ���604��ȫ��ʵ���ܹ���18���ʵ�ȫ���ƹ���604-18=590
#include "RgbDrv.h"

PUINT8C DATA_LIGHT = DATA_LIGHT_BASE;//��������Ч��Ϣָ��

static UINT8C LED_CURVE[] = {//LED������У����
	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,
	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  1,  1,  1,   1,  1,  1,  1,
	  1,  1,  1,  1,   1,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  3,   3,  3,  3,  3,
	  3,  3,  3,  4,   4,  4,  4,  4,   4,  4,  4,  5,   5,  5,  5,  5,   5,  6,  6,  6,
	  6,  6,  6,  7,   7,  7,  7,  7,   7,  8,  8,  8,   8,  8,  9,  9,   9,  9,  9,  9,
	 10, 10, 10, 10,  11, 11, 11, 11,  11, 12, 12, 12,  12, 13, 13, 13,  13, 13, 14, 14,
	 14, 14, 15, 15,  15, 15, 16, 16,  16, 16, 17, 17,  17, 18, 18, 18,  18, 19, 19, 19,
	 19, 20, 20, 20,  21, 21, 21, 22,  22, 22, 22, 23,  23, 23, 24, 24,  24, 25, 25, 25,
	 26, 26, 26, 27,  27, 27, 28, 28,  28, 29, 29, 29,  30, 30, 30, 31,  31, 31, 32, 32,
	 33, 33, 33, 34,  34, 34, 35, 35,  36, 36, 36, 37,  37, 37, 38, 38,  39, 39, 39, 40,
	 40, 41, 41, 42,  42, 42, 43, 43,  44, 44, 44, 45,  45, 46, 46, 47,  47, 48, 48, 48,
	 49, 49, 50, 50,  51, 51, 52, 52,  53, 53, 53, 54,  54, 55, 55, 56,  56, 57, 57, 58,
	 58, 59, 59, 60,  60, 61, 61, 62,  62, 63, 63, 64,  64, 65, 65, 66,  66, 67, 67, 68,
	 68, 69, 70, 70,  71, 71, 72, 72,  73, 73, 74, 74,  75, 76, 76, 77,  77, 78, 78, 79,
	 79, 80, 81, 81,  82, 82, 83, 84,  84, 85, 85, 86,  86, 87, 88, 88,  89, 89, 90, 91,
	 91, 92, 93, 93,  94, 94, 95, 96,  96, 97, 98, 98,  99, 99,100,101, 101,102,103,103,
	104,105,105,106, 107,107,108,109, 109,110,111,111, 112,113,113,114, 115,115,116,117,
	117,118,119,120, 120,121,122,122, 123,124,124,125, 126,127,127,128, 129,129,130,131,
	132,132,133,134, 135,135,136,137, 138,138,139,140, 141,141,142,143, 144,144,145,146,
	147,148,148,149, 150,151,151,152, 153,154,155,155, 156,157,158,159, 159,160,161,162,
	163,164,164,165, 166,167,168,168, 169,170,171,172, 173,173,174,175, 176,177,178,179,
	179,180,181,182, 183,184,185,185, 186,187,188,189, 190,191,192,193, 193,194,195,196,
	197,198,199,200, 201,201,202,203, 204,205,206,207, 208,209,210,211, 212,213,213,214,
	215,216,217,218, 219,220,221,222, 223,224,225,226, 227,228,229,230, 231,232,233,234,
	235,235,236,237, 238,239,240,241, 242,243,244,245, 246,247,248,249, 250,251,252,253,
	255,
};
static UINT8C ON_CURVE[] = {//�����ȹ۸�У����
	  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22,
	 22, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 46, 46, 47,
	 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 75,
	 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,
	107,108,109,110,111,112,113,114,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,136,137,138,139,140,
	141,142,143,144,145,146,148,149,150,151,152,153,154,155,157,158,159,160,161,162,163,164,166,167,168,169,170,171,173,174,175,176,
	177,178,180,181,182,183,184,186,187,188,189,190,192,193,194,195,196,198,199,200,201,203,204,205,206,207,209,210,211,212,214,215,
	216,217,219,220,221,223,224,225,226,228,229,230,231,233,234,235,237,238,239,241,242,243,245,246,247,248,250,251,252,253,254,255,
};
static UINT8C NBHD_T[] = {//ʮ�������
	1,4,255,255,	0,2,5,255,		1,3,6,255,		2,7,255,255,
	0,5,8,255,		1,4,6,9,		2,5,7,10,		3,6,11,255,
	4,9,12,255,		5,8,10,13,		6,9,11,14,		7,10,15,255,
	8,13,255,255,	9,12,14,255,	10,13,15,255,	11,14,255,255,
};
static UINT8C NBHD_X[] = {//X�������
	5,255,255,255,	4,6,255,255,	5,7,255,255,	6,255,255,255,
	1,9,255,255,	0,2,8,10,		1,3,9,11,		2,10,255,255,
	5,13,255,255,	4,6,12,14,		5,7,13,15,		6,14,255,255,
	9,255,255,255,	8,10,255,255,	9,11,255,255,	10,255,255,255,
};
static UINT8C INX_TABLE[] = {//Ԥ�õ�Ч�±��
	0,0,0,0,	 1,1,1,1,	 2,2,2,2,	 3,3,3,3,//��(����)
	0,1,2,3,	 1,2,3,4,	 2,3,4,5,	 3,4,5,6,//�K
	0,1,2,3,	 0,1,2,3,	 0,1,2,3,	 0,1,2,3,//��
	3,4,5,6,	 2,3,4,5,	 1,2,3,4,	 0,1,2,3,//�J
	3,3,3,3,	 2,2,2,2,	 1,1,1,1,	 0,0,0,0,//��
	6,5,4,3,	 5,4,3,2,	 4,3,2,1,	 3,2,1,0,//�I
	3,2,1,0,	 3,2,1,0,	 3,2,1,0,	 3,2,1,0,//��
	3,2,1,0,	 4,3,2,1,	 5,4,3,2,	 6,5,4,3,//�L
	0,1,2,3,	 4,5,6,7,	 8,9,10,11,	 12,13,14,15,//����(����)
	0,4,8,12,	 1,5,9,13,	 2,6,10,14,	 3,7,11,15,//����
	3,7,11,15,	 2,6,10,14,	 1,5,9,13,	 0,4,8,12,//����
	12,13,14,15, 8,9,10,11,	 4,5,6,7,	 0,1,2,3,//����
	15,14,13,12, 11,10,9,8,	 7,6,5,4,	 3,2,1,0,//����
	15,11,7,3,	 14,10,6,2,	 13,9,5,1,	 12,8,4,0,//����
	12,8,4,0,	 13,9,5,1,	 14,10,6,2,	 15,11,7,3,//����
	3,2,1,0,	 7,6,5,4,	 11,10,9,8,	 15,14,13,12,//����
	0,1,2,3,	 7,6,5,4,	 8,9,10,11,	 15,14,13,12,//����(����)
	0,7,8,15,	 1,6,9,14,	 2,5,10,13,	 3,4,11,12,//����
	3,4,11,12,	 2,5,10,13,	 1,6,9,14,	 0,7,8,15,//����
	15,14,13,12, 8,9,10,11,	 7,6,5,4,	 0,1,2,3,//����
	12,13,14,15, 11,10,9,8,	 4,5,6,7,	 3,2,1,0,//����
	12,11,4,3,	 13,10,5,2,	 14,9,6,1,	 15,8,7,0,//����
	15,8,7,0,	 14,9,6,1,	 13,10,5,2,	 12,11,4,3,//����
	3,2,1,0,	 4,5,6,7,	 11,10,9,8,	 12,13,14,15,//����
	0,1,2,3,	 11,12,13,4, 10,15,14,5, 9,8,7,6,//����(����)
	0,11,10,9,	 1,12,15,8,	 2,13,14,7,	 3,4,5,6,//����
	3,4,5,6,	 2,13,14,7,	 1,12,15,8,	 0,11,10,9,//����
	9,8,7,6,	 10,15,14,5, 11,12,13,4, 0,1,2,3,//����
	6,7,8,9,	 5,14,15,10, 4,13,12,11, 3,2,1,0,//����
	6,5,4,3,	 7,14,13,2,	 8,15,12,1,	 9,10,11,0,//����
	9,10,11,0,	 8,15,12,1,	 7,14,13,2,	 6,5,4,3,//����
	3,2,1,0,	 4,13,12,11, 5,14,15,10, 6,7,8,9,//����
};

static uint8_t fracM[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,};//��Ч������
static uint8_t fracUD[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,};//����Ч������

static uint8_t inXi[16] = {0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15,};

static uint8_t FrameRaw[16*3] = {//ԭʼ֡����
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
};//RGB
static UINT8I FrameBuf[16*3] = {//֡����
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
	0,0,0,	0,0,0,	0,0,0,	0,0,0,
};//GRB

void ClearKeyRGB(void){//�������RGB
	memset(FrameBuf, 0, sizeof(FrameBuf));
}

void KeyRGB(uint8_t clear){//����RGB����
	static uint16_t dTime = 0;//���ʱ���¼
	static uint8_t taskTick = 0;//0~3bit:0Ϊ������,1Ϊ������,2Ϊ������,3Ϊ�����;bit7:����ģʽ��
	static uint8_t eTime = 0, INXi = 0, fracSHLD = 0;//ʱ���¼,�Զ����±�,����Ч������
	static uint8_t lightMode = 0;
	uint8_t /*lightMode = 0, */ifINX = 0, leftSHLD = 255;//ģʽ,�Ƿ����Զ�����,����ʣ��
	UINT8D i, j;//����ѭ������,RGBѭ������
	int16_t tool16 = 0;//���߱���
	uint16_t h, s, v;//HSVɫֵ
	
	if(clear){
		if(LIGHT_MONO <= 0) memset(fracM, 0, 16);//���л��������Ч������
		if(LIGHT_MONO <= 1) taskTick = 0;//�졢���л��������������
	}
	
	if((uint8_t)((uint8_t)Systime - eTime) < 20) return;//��20ms���ڴ����Ч
	eTime += 20;//ʱ�����
	
	if((taskTick & 0x0F) == 2){//��Ϊ������
		taskTick = 3;//��������
		dTime = Systime;//��¼�������ʼʱ��
		INXi = 255;
	}
	if((taskTick & 0x0F) == 3 && (uint16_t)((uint16_t)Systime - dTime) > LIGHT_D2WAVE){//��Ϊ������Ҽ��ʱ���ѵ�
		taskTick = 0;//����������
	}
	if((taskTick & 0x0F) == 0){//��Ϊ������
		taskTick = 1;//���붯����
		dTime = Systime;//��¼��������ʼʱ��
		INXi = 0;
		//lightMode = (lightMode + 1) % 32;
		if(LIGHT_WAVE == 0){//����
			lightMode = 200;
		}else if(LIGHT_WAVE <= 4){//����,����,����,����
			lightMode = (LIGHT_WAVE - 1) * 8 + LIGHT_DIR;
		}else if(LIGHT_WAVE == 5){//�Զ���
			lightMode = 100;
			memcpy(inXi, &LIGHT_IDX(0), 16);
		}else if(LIGHT_WAVE == 6){//ѭ��1
			lightMode = (lightMode + 1) % 32;
		}else if(LIGHT_WAVE == 7){//ѭ��2
			lightMode += 8;
			if(lightMode >= 39) lightMode = 0;
			else if(lightMode >= 32) lightMode -= 31;
		}else if(LIGHT_WAVE == 8){//���
			do{ j = (rand() ^ TL0) % /*26*/32; }while(lightMode == j);//����j�洢����ʱ��������,ֱ�������ڲ�ͬ
			lightMode = j;
		}
		//if(lightMode >= 32) lightMode = 200;
	}
	if((taskTick & 0x0F) == 1 && (uint16_t)((uint16_t)Systime - dTime) > LIGHT_D1WAVE && lightMode != 200){//��Ϊ�������Ҷ���ʱ���ѵ��ҷǺ���ģʽ
		dTime = Systime;//��¼�¶�����ʼʱ��
		INXi++;
	}
	
	if(lightMode < 32) memcpy(inXi, INX_TABLE + 16*lightMode, 16);//32��Ԥ�õ�Ч
	
	memcpy(FrameRaw, &LIGHT_UP(0), 16*3);//������ɫ����ԭʼ֡����
	
	for(i = 0; i < 16; i++){//16������ʼ
		if(LIGHT_COLORFUL){//������ɫ�ʱ仯
			Rgb2Hsv(FrameRaw[3*i+0], FrameRaw[3*i+1], FrameRaw[3*i+2], &h, &s, &v);
			h += (Systime / LIGHT_COLORFUL) % (COLOR_ANGLE * 6);//����ɫ���仯
			if(h >= COLOR_ANGLE * 6) h -= COLOR_ANGLE * 6;//��ֹԽ��
			Hsv2Rgb(h, s, v, &FrameRaw[3*i+0], &FrameRaw[3*i+1], &FrameRaw[3*i+2]);
		}
		
		//��Ч��
		if(lightMode == 200 && LIGHT_D1WAVE){//��Ϊ��������Ϊ����ģʽ
			v = ((uint16_t)((uint16_t)Systime - dTime) / (MAX(LIGHT_D1WAVE,1000) / 1000)) % (COLOR_ANGLE * 6);//�ö���ʱ����ƺ�������
			if(v >= COLOR_ANGLE * 3){//�½���
				v = COLOR_ANGLE * 6 - 1 - v;//0~500
				tool16 = LIGHT_T2WAVE;//�����ӳ��ֽ���ѡ���½�������
				taskTick |= 0x80;//��λ���λ
			}else{//������
				tool16 = LIGHT_T1WAVE;//�����ӳ��ֽ���ѡ������������
				if(taskTick == 0x81){//��Ϊ�����ڲ��ڶ��ν��������������һ��ѭ������
					if(i == 16 - 1) taskTick = 2;//������λ�����������(���������һ������)
					v = 0;//��������
				}
			}
			if((taskTick & 0x0F) != 1) v = 0;//���Ƕ���������������
			//�������Ч��
			if(tool16 == 0){//������
				for(j = 0; j < 3; j++) FrameRaw[3*i+j] = (uint32_t)FrameRaw[3*i+j] * v / 500;
			}else if(tool16 == 1){//���κ���У��
				v = ON_CURVE[(uint32_t)v * 255 / 500];//��ȡУ��������ֵ(����v�洢)
				for(j = 0; j < 3; j++) FrameRaw[3*i+j] = (uint16_t)FrameRaw[3*i+j] * v / 255;
			}else{//��ƽ��У��
				v = LED_CURVE[v];//��ȡУ��������ֵ(����v�洢)
				for(j = 0; j < 3; j++) FrameRaw[3*i+j] = (uint16_t)FrameRaw[3*i+j] * v / 255;
			}
			ifINX = 1;//�Զ�������λ�Է�ֹ�����Ǻ���ģʽ�Ľ����ؽ������
		}
		else if(/*(taskTick & 0x0F) == 1 && */lightMode != 200){//��Ϊ�����ڵ�Ϊ����ģʽ
			if(inXi[i] == INXi){//������
				ifINX = 1;//�Զ�������λ
//				tool16 = (255 - fracM[i]) / 50;
//				if((255 - fracM[i]) && tool16 == 0) tool16 = 1;
//				fracM[i] += tool16;
				
				tool16 = 31 - LIGHT_T1WAVE;
				if(tool16 > 10) tool16 = (tool16 - 10) * 10 + 10;
				if(255 - fracM[i] > tool16) fracM[i] += tool16;//���ӳ�
				else fracM[i] = 255;
				for(j = 0; j < 3; j++){//��������Ч��
					FrameRaw[3*i+j] = (uint16_t)FrameRaw[3*i+j] * ON_CURVE[fracM[i]] / 255;
				}
			}else{//�½���
//				tool16 = (fracSHLD - leftSHLD) / (LIGHT_T2SYS + 1);//�������ӳ�
//				if(tool16 == 0) tool16 = 1;
//				fracSHLD -= tool16;
				
				tool16 = fracM[i] / (LIGHT_T2WAVE + 1);//���ӳ�
				if(fracM[i] && tool16 == 0) tool16 = 1;
				fracM[i] -= tool16;
				for(j = 0; j < 3; j++){//���뽵��Ч��
					FrameRaw[3*i+j] = (uint16_t)FrameRaw[3*i+j] * fracM[i] / 255;
				}
			}
		}
		
		//��������Ч��,��ȡ��������,���㰴��Ч��
		if(keyNow[i]){//������
			tool16 = (255 - fracUD[i]) / (LIGHT_T1(i) + 1);//31
			if((255 - fracUD[i]) && tool16 == 0) tool16 = 1;
			fracUD[i] += tool16;
			if(leftSHLD > LIGHT_SHLD(i)/*i*16*/) leftSHLD = LIGHT_SHLD(i)/*i*16*/;//����������ѹ������
		}else{//��̧��
			tool16 = fracUD[i] / (LIGHT_T2(i) + 1);//31
			if(fracUD[i] && tool16 == 0) tool16 = 1;
			fracUD[i] -= tool16;
			for(j = 0; j < 3; j++){//��������Ч��
				FrameRaw[3*i+j] = FrameRaw[3*i+j] * fracSHLD / 255;
			}
		}
		for(j = 0; j < 3; j++){//���밴��Ч��
			FrameRaw[3*i+j] += ((int16_t)LIGHT_DOWN(3*i+j) - FrameRaw[3*i+j]) * fracUD[i] / 255;
		}
	}//16���������
	
	if(ifINX == 0 && (taskTick & 0x0F) == 1) taskTick = 2;//��ƥ����Ϊ����������������
	
	//����ȫ������Ч������һ��ʹ��
	if(leftSHLD > fracSHLD){//������ʣ��ֵ���ڵ�ǰ���α���
//		tool16 = (255 - fracSHLD) / 31;//(LIGHT_T1SYS + 1);
//		if((255 - fracSHLD) && tool16 == 0) tool16 = 1;
//		fracSHLD += tool16;
		tool16 = 31 - LIGHT_T1SYS;
		if(tool16 > 10) tool16 = (tool16 - 10) * 10 + 10;
		if(leftSHLD - fracSHLD > tool16) fracSHLD += tool16;//5;//�������ӳ�
		else fracSHLD = leftSHLD;
	}else if(leftSHLD < fracSHLD){//������ʣ��ֵС�ڵ�ǰ���α���
		tool16 = (fracSHLD - leftSHLD) / (LIGHT_T2SYS + 1);//�������ӳ�
		if(tool16 == 0) tool16 = 1;
		fracSHLD -= tool16;
	}
	
	//GRB��λ����תӳ��
	if(CFG_KB_DIR == 0){//��������
		for(i = 0; i < 16; i++){	FrameBuf[i*3+1] = FrameRaw[i*3+0];
			FrameBuf[i*3+0] = FrameRaw[i*3+1];FrameBuf[i*3+2] = FrameRaw[i*3+2];	}
	}else if(CFG_KB_DIR == 3){//����90��(�˴��밴����ȡ�෴)
		for(i = 0; i < 16; i++){	FrameBuf[i*3+1] = FrameRaw[TURN_R90[i]*3+0];
			FrameBuf[i*3+0] = FrameRaw[TURN_R90[i]*3+1];FrameBuf[i*3+2] = FrameRaw[TURN_R90[i]*3+2];	}
	}else if(CFG_KB_DIR == 2){//��ת180��
		for(i = 0; i < 16; i++){	FrameBuf[i*3+1] = FrameRaw[(16 - i)*3+0];
			FrameBuf[i*3+0] = FrameRaw[(16 - i)*3+1];FrameBuf[i*3+2] = FrameRaw[(16 - i)*3+2];	}
	}else if(CFG_KB_DIR == 1){//����90��(�˴��밴����ȡ�෴)
		for(i = 0; i < 16; i++){	FrameBuf[i*3+1] = FrameRaw[TURN_L90[i]*3+0];
			FrameBuf[i*3+0] = FrameRaw[TURN_L90[i]*3+1];FrameBuf[i*3+2] = FrameRaw[TURN_L90[i]*3+2];	}
	}
}

void WsWrite16(void){//д��16����
	UINT8D i, iBit;
	EA = 0;//���ж�
	for(i = 0; i < 16*3; i++){//GRB
		for(iBit = 7; iBit < 8; iBit--){
			if((FrameBuf[i] >> iBit) & 0x01){//1��
				WS_DOUT = 1;//P2 |= 0x80;
				_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
				WS_DOUT = 0;//P2 &= ~0x80;
			}else{//0��
				WS_DOUT = 1;
				_nop_();
				WS_DOUT = 0;
			}
		}
	}
	EA = 1;//���ж�
}

//H:0~COLOR_ANGLE*6,S:0~100(����delta����),V:0~255
void Rgb2Hsv(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV){//RGBתHSV
    uint8_t max = MAX(MAX(vR,vG),vB), min = MIN(MIN(vR,vG),vB);
    uint8_t delta = max - min;
    if(delta == 0) *pH = 0;
    else if(max == vR) *pH = (uint16_t)COLOR_ANGLE*(vG-vB)/delta;
    else if(max == vG) *pH = (uint16_t)COLOR_ANGLE*(vB-vR)/delta + COLOR_ANGLE*2;
    else if(max == vB) *pH = (uint16_t)COLOR_ANGLE*(vR-vG)/delta + COLOR_ANGLE*4;
    if(*pH > COLOR_ANGLE * 6) *pH += COLOR_ANGLE * 6;
    if(max == 0) *pS = 0;
    else *pS = delta;//100 * delta / max;//ע��˴�Sֱ����delta����,�ʺ�����ֱ���޸�V���Ϸ�
    *pV = max;
}
void Hsv2Rgb(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB){//HSVתRGB
	uint8_t hi = (uint16_t)(vH / COLOR_ANGLE) % 6;
    uint16_t f = vH - hi * COLOR_ANGLE;
    uint8_t p = vV - vS;
    uint8_t q = vV - (uint16_t)vS * f / COLOR_ANGLE;
    uint8_t t = vV - (uint16_t)vS * (COLOR_ANGLE - f) / COLOR_ANGLE;
    if(hi == 0)     {*pR = vV;   *pG = t;    *pB = p;}
    else if(hi == 1){*pR = q;    *pG = vV;   *pB = p;}
    else if(hi == 2){*pR = p;    *pG = vV;   *pB = t;}
    else if(hi == 3){*pR = p;    *pG = q;    *pB = vV;}
    else if(hi == 4){*pR = t;    *pG = p;    *pB = vV;}
    else if(hi == 5){*pR = vV;   *pG = p;    *pB = q;}
}

static UINT16C RGB_DELAY[8] = {0,1,10,50, 100,500,1000,2000};//RGB��ʱ��(��λms)

extern bit bitNUM, bitCAPS, bitSCROLL;//�������� ��д���� ��������
extern uint8_t clickerNum;//�Զ�������
extern uint8_t mode3_key;//ģʽ3����(1-16)

extern uint32_t changeTime;//�����л�ʱ��

void SysRGB(){//ϵͳRGB����
	uint8_t i;
	static uint8_t rgbWeight = 0;//ϵͳRGBָʾ��Ȩ��
	uint8_t rgbOutput[3];//ϵͳRGBָʾ�����
	uint8_t rgbInput[6];//ϵͳRGBָʾ������
	rgbInput[0] = 0;
	rgbInput[1] = bitNUM;
	rgbInput[2] = bitCAPS;
	rgbInput[3] = bitSCROLL;
	rgbInput[4] = !!clickerNum;
	rgbInput[5] = !!mode3_key;
	
	if(CFGb_RGB_T_ON && (Systime - changeTime) < RGB_DELAY[CFGb_RGB_T_ON]){//�����л������趨ʱ��֮��
		rgbWeight = 255;
	}
	else if(CFGb_RGB_T_OFF){//�����趨ʱ�� �� ��������Ϊ����
		if(CFGb_RGB_T_OFF >= 4) rgbWeight = 0;//����Ϊ������
		else if(rgbWeight >= (1 << (CFGb_RGB_T_OFF - 1))) rgbWeight -= (1 << (CFGb_RGB_T_OFF - 1));
		else rgbWeight = 0;
	}

	for(i = 0; i < 3; i++){//��������R G B
		rgbOutput[i] = rgbInput[CFGb_RGB_MAP(i)] ^ CFGb_RGB_DIR(i);//ӳ�� ����ָʾ��״̬
		if(rgbOutput[i] && !CFGb_RGB_T_OFF){//��ָʾ���� �� ��ǩ����Ϊ����
			rgbOutput[i] *= CFG_RGB_SIGN(i);//ָʾ����
		}
		else{
			rgbOutput[i] = (CFG_RGB_SIGN(i) * (255 - rgbWeight) * rgbOutput[i]
							+ CFG_RGB_RGB(i) * rgbWeight) / 255;//��Ȩƽ��
		}
//		rgbOutput[i] = (CFG_RGB_SIGN(i) * (255 - rgbWeight) * (rgbInput[CFGb_RGB_MAP(i)] ^ CFGb_RGB_DIR(i)) 
//						+ CFG_RGB_RGB(i) * rgbWeight) / 255;//ӳ�� ��Ȩƽ��
	}
	PWM_R = rgbOutput[0];
	PWM_G = rgbOutput[1];
	PWM_B = rgbOutput[2];
}















