
#include "ParaUse.h"

//******************************���ķ���******************************//
extern uint8_t KeyBrd_data[];//���̱���
extern uint8_t Mouse_data[];//��걨��
extern uint8_t Point_data[];//ָ�뱨��
extern uint8_t Vol_data[];//��������
uint8_t KeyBrd_data_old[KB_LEN];//�ϴμ��̱���
uint8_t Mouse_data_old = 0;//�ϴ���걨��
uint8_t Vol_data_old = 0;//�ϴ���������
extern uint8_t KeyBrd_if_send;//���̱����Ƿ���
extern uint8_t Vol_if_send;//���������Ƿ���
extern uint8_t Point_if_send;//ָ�뱨���Ƿ���
extern uint8_t Mouse_if_send;//��걨���Ƿ���
//********************************************************************//

extern uint8_t clickerNum;//�Զ�������
extern uint32_t changeTime;//�����л�ʱ��

//******************************�������******************************//
uint8_t mode3_key = 0;//ģʽ3����(1-16)
uint16_t mode3_i = 0;//ģʽ3Դ�����±�(����mode3_data)
uint16_t mode3_loop_count = 0;//ģʽ3ѭ������
uint8_t mode3_loop_flag = 0;//ģʽ3ѭ��������־
uint8_t mode3_pulse = 0;//ģʽ3���
uint8_t mode3_delaying = 0;//ģʽ3�Ƿ���ʱ��
//********************************************************************//
uint8_t switch_i = 0xFF, switch_count = 0;//�л���ѡ��ͼ���
uint8_t switch_key = 0, switch_func = 0;//�л�������


uint8_t fillReport(void)//������д
{
	static uint32_t oldTime = 0;//��¼ʱ��
	uint8_t mode1_num = 0, mode2_num = 0, mode7_num = 0;
	uint8_t i = 0;//������
//	struct config_key *fill_key = cfg_key[sys_cs];
	
	uint16_t x, y;
	uint8_t turn_old, turn_dif;
	int8_t auto_num;
	
	//***********************************�����ļ����ͱ�־��ʼ��***********************************//
	KeyBrd_if_send = Mouse_if_send = Point_if_send = Vol_if_send = 0;//���ͱ�־����
	
	memcpy(KeyBrd_data_old + 1, KeyBrd_data + 1, KB_LEN - 1);//��¼��һ�α���
	memset(KeyBrd_data + 1, 0, KB_LEN - 1);//������м�

	Mouse_data_old = Mouse_data[1];//��¼��һ�α���
	memset(Mouse_data + 1, 0, 4);//�����걨��

	Point_data[1] = 0x10;
	memset(Point_data + 3, 0xFF, 4);//���ָ�뱨��

	Vol_data_old = Vol_data[1];//��¼��һ�α���
	Vol_data[1] = 0;//�����������
	//********************************************************************************************//
	
	if(keyAddr[sys_cs][0] == 0) return 0xFF;//�����������ݴ������˳�
	
	//****************************************���̰�������****************************************//
	if(mode3_key){//������mode3δ���״̬
		if(keyOld[mode3_key - 1] && !keyNow[mode3_key - 1] && mode3_loop_flag < 2) mode3_loop_flag++;//�ͷ��ؼ���
		mode3_pulse = !mode3_pulse;//�����־�ȷ�ת
		if(mode3_pulse) mode3Handle();//����ת��Ҫ���Ҳ����תǰ���ü����ִ�д���
	}
	else{//����״̬
		for(i = 0; i < 16; i++){//ͳ�ư��µĸ�ģʽ����
			if(keyNow[i/*+1*/]){
				if(CFG_K_MODE(keyAddr[sys_cs][i]) == 1 && i != switch_i) mode1_num++;
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 2 && i != switch_i) mode2_num++;
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 7 && i != switch_i) mode7_num++;
			}
		}
		for(i = 0; i < 16; i++){//����16������
			if(i == switch_i){//����������Ч����ʱ�л���
				if(!keyNow[i] && !keyOld[i]){//�ͷ���֮��һ��
					switch_count++;//�����˲�
					if(switch_count > 10){
						switch_count = 0;//��������
						keyInsert(i + 3,switch_key);//�����ֵ
						csChange(switch_func);//�л�����
						switch_i = 0xFF;//��λ
					}
				}
				continue;//������ʱ�л���
			}
			if(keyNow[i]){//������
				if(CFG_K_MODE(keyAddr[sys_cs][i]) == 1){//ģʽ1:����
					keyInsert(i + 3,CFG_K_KEY(keyAddr[sys_cs][i]));//�����ֵ
				}
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 2){//ģʽ2:��ݼ�
					keyInsert(i + 3,CFG_K_KEY(keyAddr[sys_cs][i]));//�����ֵ
					KeyBrd_data[1] |= CFG_K_FUNC(keyAddr[sys_cs][i]);//���빦�ܼ�
				}
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 3){//ģʽ3:������
					if(mode1_num == 0 && mode2_num == 0 && mode7_num == 0 && !keyOld[i]){//������1,2,7ģʽ�����Ұ�����
						mode3_key = i + 1;//��¼mode3����(Ҫ+1)
						mode3_i = keyAddr[sys_cs][i] + 3;//��ȡ��ʼ�±�
						mode3_loop_count = 0;//ģʽ3ѭ����������
						mode3_loop_flag = 0;//ģʽ3ѭ��������־����
						mode3_pulse = 1;//������
						mode3Handle();
					}
				}
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 4 || CFG_K_MODE(keyAddr[sys_cs][i]) == 5){//ģʽ4:�����λ,ģʽ5:�����
					if(!keyOld[i]) keyFlag[i] = 2;//������̬
					
					x = CFG_K_X(keyAddr[sys_cs][i]) * 32768 / CFG_SCN_W;
					y = CFG_K_Y(keyAddr[sys_cs][i]) * 32768 / CFG_SCN_H;
					Point_data[3] = x & 0xFF;
					Point_data[4] = (x >> 8) & 0xFF;
					Point_data[5] = y & 0xFF;
					Point_data[6] = (y >> 8) & 0xFF;
					if(CFG_K_MODE(keyAddr[sys_cs][i]) == 5){
						Point_data[1] |= 0x01;
					}
					
					Point_data[2] = i;
				}
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 6){//ģʽ6:�л���
					if((CFG_K_FUNC(keyAddr[sys_cs][i]) & 0x08) && switch_i == 0xFF && !keyOld[i]){//��Ϊ��ʱ�л���Ϊ�����Ұ�����
						switch_key = /*fill_key[i].key*/CFG_K_KEY(keyAddr[sys_cs][i]);//�����ֵ
						switch_func = sys_cs + 1;//����ɼ���ѡ��
						/*uint8_t */turn_old = key_turn[sys_cs];//�ɼ��̷���
						csChange(CFG_K_FUNC(keyAddr[sys_cs][i]));//�л�
						/*uint8_t */turn_dif = (key_turn[sys_cs] + 4 - turn_old) % 4;//��Ծɼ��̷���
						if(turn_dif == 0) switch_i = i;
						else if(turn_dif == 1) switch_i = turnL90[i]/* - 1*/;
						else if(turn_dif == 2) switch_i = 16 - i;
						else if(turn_dif == 3) switch_i = turnR90[i]/* - 1*/;
//						switch_key = /*fill_key[i].key*/CFG_K_KEY(keyAddr[sys_cs][i]);//�����ֵ
//						switch_func = CFG_K_FUNC(keyAddr[sys_cs][i]);//�����л���ʽ
						break;//��������ѭ��
					}
				}
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 7){//ģʽ7:��������
					if(!keyOld[i] && !(CFG_K_FUNC(keyAddr[sys_cs][i]) & 0x01)){//��Ϊ�������ҷ��Զ�����
						keyFlag[i] |= 0x02;//��������
						keyWork[i] = 0;//��ʱ������
					}
				}
			}
			else if(keyOld[i]){//����̧��(�ͷ���)
				if(CFG_K_MODE(keyAddr[sys_cs][i]) == 5){//ģʽ5:�����
					if(keyOld[i]){//�ͷ���
						keyFlag[i] = 1;//��̧��̬
					}
				}
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 6){//ģʽ6:�л���
					if(!(CFG_K_FUNC(keyAddr[sys_cs][i]) & 0x08)){//��Ϊ����ʱ�л�
						switch_i = 0xFF;//ֱ�Ӹ�λ��ʱ�л�����־
						csChange(CFG_K_FUNC(keyAddr[sys_cs][i]));//�л�
					}
				}
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 7){//ģʽ7:��������
					if((CFG_K_FUNC(keyAddr[sys_cs][i]) & 0x01) && !(keyFlag[i] & 0x02)){//�Զ������ҵ�ǰδ������
						keyFlag[i] |= 0x02;//��������
						keyWork[i] = 0;//��ʱ������
					}
					else keyFlag[i] &= ~0x02;//ֹͣ����
				}
				else if(CFG_K_MODE(keyAddr[sys_cs][i]) == 8){//ģʽ8:������
					return 1;
				}
			}
		}//������16����������Ҫ����
		
		for(i = 0; i < 16; i++){//����16�������Ĵ���
			if(CFG_K_MODE(keyAddr[sys_cs][i]) == 4 || CFG_K_MODE(keyAddr[sys_cs][i]) == 5){//ģʽ4:�����λ,ģʽ5:�����
				if(CFG_K_FUNC(keyAddr[sys_cs][i]) == 1 || CFG_K_FUNC(keyAddr[sys_cs][i]) == 2){
					x = CFG_K_X(keyAddr[sys_cs][i]) * 32768 / CFG_SCN_W;
					y = CFG_K_Y(keyAddr[sys_cs][i]) * 32768 / CFG_SCN_H;
					Point_data[3] = x & 0xFF;
					Point_data[4] = (x >> 8) & 0xFF;
					Point_data[5] = y & 0xFF;
					Point_data[6] = (y >> 8) & 0xFF;
					
					Point_data[2] = i;
					
					if(CFG_K_FUNC(keyAddr[sys_cs][i]) == 2){
						if(CFG_K_MODE(keyAddr[sys_cs][i]) == 5) Point_data[1] |= 0x01;
						else Point_data[1] &= ~0x01;
						keyFlag[i] = 3;
					}
					else{
						Point_data[1] = 0x00;
						keyFlag[i] = 0;
					}
					break;
				}
			}
		}//������16�������Ĵ���
		
		/*int8_t */auto_num = 0;//����ִ���Զ�����İ�������
		for(i = 0; i < 16; i++){//����16������������
			if(CFG_K_MODE(keyAddr[sys_cs][i]) == 7){//ģʽ7:��������
				if(oldTime > Systime){//��ϵͳʱ������
					keyWork[i] = 0;//����趨ʱ��
					keyFlag[i] &= ~0x04;//��λ�����־
					keyFlag[i] &= ~0x02;//ֹͣ����
				}
				else if(keyFlag[i] & 0x02){//����������
					if(CFG_K_T(keyAddr[sys_cs][i]) == 0){//������Ϊ0�򳤰�
						keyInsert(i + 3,CFG_K_KEY(keyAddr[sys_cs][i]));//�����ֵ
					}
					else if(keyFlag[i] & 0x04){//���ϴ��ѵ��
						keyFlag[i] &= ~0x04;//��λ�����־
					}
					else{//���ϴ�δ���
						if((uint16_t)((uint16_t)Systime - keyWork[i]) >= CFG_K_T(keyAddr[sys_cs][i])){//����ʱ�ѵ�
							keyInsert(i + 3,CFG_K_KEY(keyAddr[sys_cs][i]));//�����ֵ
							keyWork[i] = Systime;//��¼����ʱ���16λ
							keyFlag[i] |= 0x04;//��λ�����־
						}
					}
					if(CFG_K_FUNC(keyAddr[sys_cs][i]) & 0x01) auto_num++;//�Զ����㰴������
				}
			}
		}//������16������������
		oldTime = Systime;//��¼ʱ�����
		clickerNum = auto_num;//�Զ�����������
	}
	//********************************************************************************************//
	
	//***********************************ҡ����ť����***********************************//
	rkHandle(0);//ҡ�˴���
	ecHandle(0);//��ť����
	if(!mode3_key) rkEcKeyHandle();//ҡ����ť��������
	//**********************************************************************************//
	
	//***********************************�жϸ������Ƿ�Ҫ����***********************************//
	for(i = 1; i < KB_LEN; i++){
		if(KeyBrd_data_old[i] != KeyBrd_data[i]){//���̱�������һ�β�ͬ����
			KeyBrd_if_send = 1;	break;
		}
	}
	if(Mouse_data[1] != Mouse_data_old) Mouse_if_send = 1;//��갴�����ϴβ�ͬ����
	else{
		for(i = 2; i < 5; i++){
			if(Mouse_data[i] != 0){//�������ƶ����������
				Mouse_if_send = 1;	break;
			}
		}
	}
	for(i = 3; i < 7; i++){
		if(Point_data[i] != 0xFF){//����������Ч��������
			Point_if_send = 1;	break;
		}
	}
	if(Vol_data[1] != Vol_data_old) Vol_if_send = 1;//ý�屨�����ϴβ�ͬ����
	//******************************************************************************************//
	
	if(switch_i != 0xFF 
		&& (KeyBrd_if_send || Mouse_if_send || Point_if_send || Vol_if_send)){
		switch_key = 0;//����ʱ�л��������ڼ�����������������л��������ֵ
	}
	
	if(mode3_delaying == 1){//��ʱ�����ͱ�־����
		KeyBrd_if_send = Mouse_if_send = Point_if_send = Vol_if_send = 0;
	}
	
	return 0;
}

void csChange(uint8_t change)//�л�
{
	change &= 0x07;//ȡ��3λ
	if(change == 0 || change > CFG_NUM) return;
	sys_cs = change - 1;
	DATA_CFG = DATA_CFG_BASE - sys_cs * 512;//�޸ļ�������ָ�� ����
	
	keyRGB(1);//����RGB��������
	if(LIGHT_MONO != 3) DATA_LIGHT = DATA_LIGHT_BASE + sys_cs * 256;//�����ǵ�Ч���л����޸ĵ�Ч����ָ�� ����
	
	changeTime = Systime;
	
	rkHandle(1);
	ecHandle(1);
	
	memset(keyWork,0,sizeof(keyWork));
	memset(keyFlag,0,sizeof(keyFlag));
}

void keyInsert(uint8_t r_i, uint8_t key_v)//��������
{
	if(key_v == kv_wheel_down) Mouse_data[4] += -1;//��������
	else if(key_v == kv_wheel_up) Mouse_data[4] += 1;//��������
	else if(key_v == kv_vol_up) Vol_data[1] |= 0x01;//������
	else if(key_v == kv_vol_down) Vol_data[1] |= 0x02;//������
	else if(key_v == kv_vol_mute) Vol_data[1] |= 0x04;//����
	else if(key_v == kv_vol_stop) Vol_data[1] |= 0x08;//������ͣ
	else if(key_v == kv_vol_next) Vol_data[1] |= 0x10;//��һ��
	else if(key_v == kv_vol_prev) Vol_data[1] |= 0x20;//��һ��
	else if(key_v == kv_mouse_l) Mouse_data[1] |= 0x01;//������
	else if(key_v == kv_mouse_m) Mouse_data[1] |= 0x04;//����м�
	else if(key_v == kv_mouse_r) Mouse_data[1] |= 0x02;//����Ҽ�
	else if(key_v == kv_ctrl) KeyBrd_data[1] |= 0x01;//ctrl
	else if(key_v == kv_shift) KeyBrd_data[1] |= 0x02;//shift
	else if(key_v == kv_alt) KeyBrd_data[1] |= 0x04;//alt
	else if(key_v == kv_win) KeyBrd_data[1] |= 0x08;//win
	else{//��ͨ���̰���
		if(r_i == 0xFF){//��ʹ���Զ�����
			for(r_i = 3; r_i < KB_LEN; r_i++){//������λ
				if(!KeyBrd_data[r_i]) break;//���˴�Ϊ��
			}
		}
		if(r_i == 0xFF) return;//��δ�ҵ���λ���˳�
		KeyBrd_data[r_i] = key_v;//�����ֵ
	}
}

void mode3Handle(void)//mode3����(�����鴦��)
{
	static uint32_t setTime = 0, oldTime = 0;//�趨ʱ�估��¼ʱ��
	static uint16_t loopStart = 0xFFFF;//ѭ����ʼ��ַ
	uint8_t report_i = 3;//����д��λ��
	
	uint16_t x, y;
	uint16_t end_i;
	uint8_t check_i;
//	uint8_t mode3_key = 0;//ģʽ3����(1-16)
//	uint16_t mode3_i = 0;//ģʽ3Դ�����±�(����mode3_data)
//	uint16_t mode3_loop_count = 0xFFFF;//ģʽ3ѭ������
//	uint8_t mode3_loop_flag = 0;//ģʽ3ѭ��������־
	
	if(oldTime > Systime) setTime = 0;//��ϵͳʱ����������ֹ��ʱ
	oldTime = Systime;//��¼ʱ�����
	if(setTime > Systime){//��ʱδ����
		mode3_delaying = 1;//��ʱ��־��λ
		mode3_pulse = 0;//������ͣ��־
		return;//�˳��ȴ�
	}
	mode3_delaying = 0;//��ʱ��־����
	
	/*uint16_t */end_i = 0;//����λ��
	if(mode3_key) end_i = keyAddr[sys_cs][mode3_key - 1] + 3 + CFG_K_LEN(keyAddr[sys_cs][mode3_key - 1]);
	
	for(;report_i < KB_LEN;){//������δ����
		if(mode3_i >= end_i){//����������
			mode3_key = 0;
			break;
		}
		if(CFG_ACS(mode3_i) == kv_report){//���Ŀ���
			mode3_pulse = CFG_ACS(mode3_i + 1) & 0x01;//�������λ��ͣ��־
			if(CFG_ACS(mode3_i + 1) & 0x02){//��Ϊ������ֹ
				mode3_i += 2;
				break;//��ռ���α���
			}
			else mode3_i++;
		}
		else if(CFG_ACS(mode3_i) == kv_loop){//����ѭ��
			if(CFG_ACS(mode3_i + 1) == 0x02){//��Ϊ��ʼ��
				loopStart = mode3_i + 4;//ѭ����ʼ��ַ
				mode3_i += 4;
				continue;
			}
			mode3_loop_count++;//ѭ����������
			x = ((uint16_t)CFG_ACS(mode3_i + 2) << 8) | CFG_ACS(mode3_i + 3);//����x�洢ѭ������
			if(CFG_ACS(mode3_i + 1) && mode3_loop_flag == 2 || !CFG_ACS(mode3_i + 1) && mode3_loop_flag
				|| x && mode3_loop_count >= x){//���½��ش������������λ
				mode3_loop_flag = 3;//��Ϊ������־
			}
			if(mode3_loop_flag == 3){//����ѭ��
				mode3_i += 3;//�±�����ѭ����Ԫ
				mode3_loop_flag = mode3_loop_count = 0;//������־�ͼ���������
				loopStart = 0xFFFF;//ѭ����ʼ��ַ��λ
			}
			else{//����ѭ��
				if(loopStart != 0xFFFF) mode3_i = loopStart;//�±���ת��ѭ����ʼ��ַ
				else mode3_i = keyAddr[sys_cs][mode3_key - 1] + 3;//�±���ת�ؿ�ͷ
				break;//��ռ���α���
			}
		}
		else if(CFG_ACS(mode3_i) == kv_delay){//������ʱ
			uint16_t delayTime = (CFG_ACS(mode3_i + 1) << 8) | CFG_ACS(mode3_i + 2);
			setTime = Systime + delayTime;
//			mode3_pulse = 0;//������ͣ��־
			mode3_i += 2;
			if(++mode3_i == end_i) mode3_key = 0;//����������
			break;//��ռ���α���
		}
		else if(CFG_ACS(mode3_i) == kv_shortcut){//���п�ݼ�
			if(report_i > 3 || Mouse_data[1] != 0) break;//�����α��������������˳�����һ��
			keyInsert(report_i,CFG_ACS(++mode3_i));//�����ֵ
			KeyBrd_data[1] = CFG_ACS(++mode3_i);//���빦�ܼ�
			if(++mode3_i == end_i) mode3_key = 0;//����������
			break;//��ռ���α���
		}
		else if(CFG_ACS(mode3_i) == kv_point){//���й����λ
			if(report_i > 3 || Mouse_data[1] != 0) break;//�����α��������������˳�����һ��
			x = (CFG_ACS(mode3_i + 1) << 8) | CFG_ACS(mode3_i + 2);
			y = (CFG_ACS(mode3_i + 3) << 8) | CFG_ACS(mode3_i + 4);
			x = x * 32768 / CFG_SCN_W;
			y = y * 32768 / CFG_SCN_H;
			Point_data[3] = x & 0xFF;
			Point_data[4] = (x >> 8) & 0xFF;
			Point_data[5] = y & 0xFF;
			Point_data[6] = (y >> 8) & 0xFF;
			if(1){
				Point_data[1] |= 0x01;//�����
			}
			mode3_i += 4;
			if(++mode3_i == end_i) mode3_key = 0;//����������
			break;//��ռ���α���
		}
		else if(CFG_ACS(mode3_i) == kv_shift){//����shift
			check_i = 3;
			for(check_i = 3; check_i < report_i; check_i++){//����Ƿ�������ͬ��ֵ
				if(KeyBrd_data[check_i] == CFG_ACS(mode3_i)){
					check_i = 0;
					break;
				}
			}
			if(!check_i) break;//��������ͬ��ֵ���˳�����һ��
			if(Mouse_data[1] != 0) break;//����걨�������������˳�����һ��
			if(KeyBrd_data[1]){//�������Ѻ�shift
				keyInsert(report_i++,CFG_ACS(++mode3_i));//�����ֵ
			}
			else if(report_i == 3){//������ʼ
				keyInsert(report_i++,CFG_ACS(++mode3_i));//�����ֵ
				KeyBrd_data[1] |= 0x02;//����shift
			}
			else break;//��ǰ���ų�shift���޷�����
		}
		else{//����shift
			check_i = 3;
			for(check_i = 3; check_i < report_i; check_i++){//����Ƿ�������ͬ��ֵ
				if(KeyBrd_data[check_i] == CFG_ACS(mode3_i)){
					check_i = 0;
					break;
				}
			}
			if(!check_i) break;//��������ͬ��ֵ���˳�����һ��
			if(Mouse_data[1] != 0) break;//����걨�������������˳�����һ��
			if(!KeyBrd_data[1]){//������δ��shift
				keyInsert(report_i++,CFG_ACS(mode3_i));//�����ֵ
			}
			else if(report_i == 3){//������ʼ
				keyInsert(report_i++,CFG_ACS(mode3_i));//�����ֵ
			}
			else break;//��ǰ������shift���޷�����
		}
		mode3_i++;
//		if(mode3_i >= end_i){//����������
//			mode3_key = 0;
//			break;
//		}
	}
}

//1:����/��ݼ�,2:�����л���
void rkEcKeyHandle(void)//ҡ����ť��������
{
	uint8_t keyM[3], keyV[3], keyF[3];//����ģʽ,��ֵ,���ܼ�
	uint8_t i;
	
	keyM[0] = CFGb_Ek_MODE(0);	keyV[0] = CFG_E_KEY(0,0);	keyF[0] = CFG_E_FUNC(0,0);
	keyM[1] = CFGb_Ek_MODE(1);	keyV[1] = CFG_E_KEY(1,0);	keyF[1] = CFG_E_FUNC(1,0);
	keyM[2] = CFGb_Rk_MODE(0);	keyV[2] = CFG_R_KEY(0,0);	keyF[2] = CFG_R_FUNC(0);
	
	for(i = 0; i < 3; i++){//2����ť����1��ҡ�˰���
		if(keyNow[i + 16]){//��Ϊ����״̬
			if(keyM[i] == 1){//����/��ݼ�
				if(keyV[i]) keyInsert(0xFF, keyV[i]);//�����ֵ
				KeyBrd_data[1] |= keyF[i];//���빦�ܼ�
			}
		}
		else if(keyOld[i + 16]){//��Ϊ�ͷ���
			if(keyM[i] == 2){//�����л���
				switch_i = 0xFF;//ֱ�Ӹ�λ��ʱ�л�����־
				csChange(keyV[i] - kv_orig_1 + 1);//�л� �ú������в������ �˴�USB��ֵ�����ּ�1�ļ�ֵ��ʼ��Ч
			}
		}
	}
}

//1:�����İ���,2:�����İ���,3:�ٶ����,4:���λ��
void rkHandle(uint8_t clear)//ҡ�˴���
{
	static int16_t RK_pulse = 0;//�����־
	static int16_t x_pic = 0, y_pic = 0;
	static int16_t dx = 0, dy = 0;
	int16_t x, y;
	int16_t equal_r;//��Ч�뾶
	
	if(clear){//���
		RK_pulse = x_pic = y_pic = 0;
		return;
	}
	
	x = (CFGb_R_DIRx(0)*2 - 1);//��������
	y = (CFGb_R_DIRy(0)*2 - 1);
	
	if(CFGb_R_DIRr(0)){//��ת90��
		if(adcValue[0] < ANA_MID_0) x *= ((int16_t)adcValue[0] - (int16_t)ANA_MID_0) * 4096L / ANA_MID_0;
		else x *= ((int16_t)adcValue[0] - (int16_t)ANA_MID_0) * 4096L / (4095 - ANA_MID_0);//�Ŵ�����4096
		
		if(adcValue[1] < ANA_MID_1) y *= ((int16_t)adcValue[1] - (int16_t)ANA_MID_1) * 4096L / ANA_MID_1;
		else y *= ((int16_t)adcValue[1] - (int16_t)ANA_MID_1) * 4096L / (4095 - ANA_MID_1);
	}
	else{
		if(adcValue[0] < ANA_MID_0) y *= ((int16_t)adcValue[0] - (int16_t)ANA_MID_0) * 4096L / ANA_MID_0;//����Ϊ��
		else y *= ((int16_t)adcValue[0] - (int16_t)ANA_MID_0) * 4096L / (4095 - ANA_MID_0);//�Ŵ�����4096
		
		if(adcValue[1] < ANA_MID_1) x *= -((int16_t)adcValue[1] - (int16_t)ANA_MID_1) * 4096L / ANA_MID_1;//����Ϊ��
		else x *= -((int16_t)adcValue[1] - (int16_t)ANA_MID_1) * 4096L / (4095 - ANA_MID_1);
	}
	
	equal_r = MAX(ABS(x), ABS(y)) - (uint16_t)CFG_R_DEAD(0) * 21;//�����Ч�뾶����ȥ����
	if(equal_r <= 0){//��������
		dx = dy = 0;
		x_pic = y_pic = 0;
		return;//�˳�
	}
	
	switch(CFGb_R_MODE(0)){
		case 1:{//�����İ���
			if(RK_pulse){//���ϴ��ѷ����򱾴μ��
				RK_pulse--;//��־�ݼ�
				break;
			}
			
			if(CFG_R_NEER(0) == 0 && CFG_R_FAR(0) == 0) RK_pulse = 0;//����
			else{//�ǳ���������
				equal_r = ((uint16_t)CFG_R_NEER(0) << 1) + (((int8_t)(CFG_R_FAR(0) - CFG_R_NEER(0)) * (equal_r >> 3)) >> 8);//�����ٶ� ����equal_r�洢

				if(equal_r <= ABS(dx)) dx = equal_r;//����ʱֱ�Ӹ�ֵ��ʹ�ù���
				else{//����ʱʹ�ù���
					equal_r = (equal_r - dx) / (CFG_R_PARA(0) + 1);//����equal_r�洢
					if(equal_r == 0) dx += 1;//����1��Ϊ1
					else dx += equal_r;
					equal_r = dx;
				}
				
				RK_pulse = 100 - equal_r;
				if(RK_pulse <= 0) RK_pulse = 1;
			}
			
			if(y > x){
				if(y > -x) keyInsert(0xFF, CFG_R_KEY(0,1));//�����ϼ�ֵ
				else keyInsert(0xFF, CFG_R_KEY(0,3));//�������ֵ
			}
			else{
				if(y > -x) keyInsert(0xFF, CFG_R_KEY(0,4));//�����Ҽ�ֵ
				else keyInsert(0xFF, CFG_R_KEY(0,2));//�����¼�ֵ
			}
			break;
		}
		case 2:{//�����İ���
			if(RK_pulse){//���ϴ��ѷ����򱾴μ��
				RK_pulse--;//��־�ݼ�
				break;
			}
			
			if(CFG_R_NEER(0) == 0 && CFG_R_FAR(0) == 0) RK_pulse = 0;//����
			else{
				equal_r = ((uint16_t)CFG_R_NEER(0) << 1) + (((int8_t)(CFG_R_FAR(0) - CFG_R_NEER(0)) * (equal_r >> 3)) >> 8);//�����ٶ� ����equal_r�洢
				
				if(equal_r <= ABS(dx)) dx = equal_r;//����ʱֱ�Ӹ�ֵ��ʹ�ù���
				else{//����ʱʹ�ù���
					equal_r = (equal_r - dx) / (CFG_R_PARA(0) + 1);//����equal_r�洢
					if(equal_r == 0) dx += 1;//����1��Ϊ1
					else dx += equal_r;
					equal_r = dx;
				}
				
				RK_pulse = 100 - equal_r;
				if(RK_pulse <= 0) RK_pulse = 1;
			}
			
			//atan(5/12)=22.6�� atan(3/7)=23.2��
			if(y*7 > x*3 && y*7 > -x*3){//��
				keyInsert(0xFF, CFG_R_KEY(0,1));//�����ֵ
			}
			else if(y*7 < x*3 && y*7 < -x*3){//��
				keyInsert(0xFF, CFG_R_KEY(0,2));//�����ֵ
			}
			if(y*3 > x*7 && y*3 < -x*7){//��
				keyInsert(0xFF, CFG_R_KEY(0,3));//�����ֵ
			}
			else if(y*3 < x*7 && y*3 > -x*7){//��
				keyInsert(0xFF, CFG_R_KEY(0,4));//�����ֵ
			}
			break;
		}
		case 3:{//�ٶ����
//			x = x > 0 ? MAX(0, x - (int16_t)CFG_R_DEAD(0) * 21) : MIN(0, x + (int16_t)CFG_R_DEAD(0) * 21);//Ӧ������
//			y = y > 0 ? MAX(0, y - (int16_t)CFG_R_DEAD(0) * 21) : MIN(0, y + (int16_t)CFG_R_DEAD(0) * 21);//Ӧ������

			x = ((int8_t)CFG_R_FAR(0) * (x >> 3)) >> 4;//�����ٶ� ����x�洢
			y = ((int8_t)CFG_R_FAR(0) * (y >> 3)) >> 4;//�����ٶ� ����y�洢
			//-1600~1600
			if(ABS(x) <= ABS(dx)) dx = x;//����ʱֱ�Ӹ�ֵ��ʹ�ù���
			else if(ABS(dx) > 800) dx = x;//���ٽ׶β�ʹ�ù���
			else if(ABS(dx) > CFG_R_NEER(0) * 10){//���ٽ׶�ʹ�õ͹���
				equal_r = (x - dx) / (CFG_R_PARA(0) + 1);//����equal_r�洢
				if(equal_r == 0) dx += SIGN(x);//����1��Ϊ1
				else dx += equal_r;
			}
			else{//���ٽ׶�ʹ�ø߹���
				equal_r = (x - dx) / (CFG_R_PARA(0) * 10 + 1);//����equal_r�洢
				if(equal_r == 0) dx += SIGN(x);//����1��Ϊ1
				else dx += equal_r;
			}
			if(ABS(y) <= ABS(dy)) dy = y;
			else if(ABS(dy) > 800) dy = y;
			else if(ABS(dy) > CFG_R_NEER(0) * 10){
				equal_r = (y - dy) / (CFG_R_PARA(0) + 1);
				if(equal_r == 0) dy += SIGN(y);
				else dy += equal_r;
			}
			else{
				equal_r = (y - dy) / (CFG_R_PARA(0) * 10 + 1);
				if(equal_r == 0) dy += SIGN(y);
				else dy += equal_r;
			}

			x_pic += dx;//����ʽ �Ը���������������
			y_pic += dy;

			x_pic = LIMIT(x_pic, -127 << 6, 127 << 6);//�޷�
			y_pic = LIMIT(y_pic, -127 << 6, 127 << 6);
			Mouse_data[2] = (int8_t)(x_pic >> 6);//��������
			Mouse_data[3] = -(int8_t)(y_pic >> 6);
			x_pic -= (int8_t)Mouse_data[2] << 6;//��ȥ�������
			y_pic += (int8_t)Mouse_data[3] << 6;
			break;
		}
		case 4:{//���λ��
			equal_r = (x - x_pic) >> 2;//�����˲� ����equal_r�洢
			if(equal_r == 0) x_pic += SIGN(x);
			else x_pic += equal_r;
			equal_r = (y - y_pic) >> 2;
			if(equal_r == 0) y_pic += SIGN(y);
			else y_pic += equal_r;
			
			if(x > 0) dx = CFG_R_KEY(0,4);	//��
			else dx = CFG_R_KEY(0,3);		//��
			if(y > 0) dy = CFG_R_KEY(0,1);	//��
			else dy = CFG_R_KEY(0,2);		//��
			
			if(dx >= kv_orig_1 && dx <= kv_orig_0){//1~9�Լ�0
				dx = (dx - kv_orig_1) * 2 + 2;//2~20 ���2
			}
			else if(dx >= kv_num_1 && dx <= kv_num_0){//NUM1~NUM9
				dx = (dx - kv_num_1) * 2 + 1;//1~19 ���2
			}
			else dx = 0;
			
			if(dy >= kv_orig_1 && dy <= kv_orig_0){//1~9�Լ�0
				dy = (dy - kv_orig_1) * 2 + 2;//2~20 ���2
			}
			else if(dy >= kv_num_1 && dy <= kv_num_0){//NUM1~NUM9
				dy = (dy - kv_num_1) * 2 + 1;//1~19 ���2
			}
			else dy = 0;
			
			x = LIMIT(x, -4095, 4095);
			y = LIMIT(y, -4095, 4095);
			
			//20*4095*50/125
			dx = (int32_t)dx * x_pic * CFG_R_FAR(0) / 125;//����ƫ����
			dy = -(int32_t)dy * y_pic * CFG_R_FAR(0) / 125;
			
			x = CFG_R_PARA(0) * 32767L / 50;//��������λ��
			y = CFG_R_NEER(0) * 32767L / 50;
			
			if(dx > 0 && (int32_t)x + dx > 32767) x = 32767;//��ֹ�����
			else if(dx < 0 && (int32_t)x + dx < 0) x = 0;//��ֹ�����
			else x += dx;//û�����
			
			if(dy > 0 && (int32_t)y + dy > 32767) y = 32767;
			else if(dy < 0 && (int32_t)y + dy < 0) y = 0;
			else y += dy;

			Point_data[3] = x & 0xFF;
			Point_data[4] = (x >> 8) & 0xFF;
			Point_data[5] = y & 0xFF;
			Point_data[6] = (y >> 8) & 0xFF;
			
			Point_data[2] = 17;//�ṩһ���Ͱ�����һ����ID
			
			RK_pulse = !RK_pulse;//����RK_pulse�洢һ��01�������
			Mouse_data[2] = RK_pulse * 2 - 1;//�������� ��x������ά���������ʾ
			
			break;
		}
		default:break;
	}
}

//1:����/��ݼ�
void ecHandle(uint8_t clear)//��ť����
{
//	static uint32_t oldTime = 0;//��¼ʱ��
//	static int TIM_old = 0;//�������ɼ���
	static int8_t TIM_count[2] = {0,0};//����������
	static int8_t EC_count[2] = {0,0};//ִ�м���
	static uint8_t EC_pulse[2] = {0,0};//�����־
	uint8_t EC_flag = 0;//ִ�б�־
	
	uint8_t i;
	
	if(clear){//���
		TIM_count[0] = TIM_count[1] = EC_count[0] = EC_count[2] = 0;
		EC_pulse[0] = EC_pulse[1] = EC_flag = 0;
		return;
	}

	TIM_count[0] -= (CFG_E_DIR(0) * 2 - 1) * (int8_t)(EC1val)/**4*/;//������������ȡ
	EC1val = 0;//����������
	TIM_count[1] -= (CFG_E_DIR(0) * 2 - 1) * (int8_t)(EC2val)/**4*/;//������������ȡ
	EC2val = 0;//����������
	
//	if(TIM_old != TIM_count){
//		TIM_old = TIM_count;
//		oldTime = Systime;//������ֵ��һ�²Ÿ���ʱ��
//	}
//	if(/*Systime - oldTime > 5000*/0){//����ʱ��������޶���
//		TIM_old = TIM_count = EC_count = EC_flag = 0;
//		oldTime = Systime;
//	}
	
	for(i = 0; i < 2; i++){//����ÿ����ť
		if(TIM_count[i] > EC_count[i]) EC_flag = 1;
		else if(TIM_count[i] < EC_count[i]) EC_flag = 2;
		else EC_flag = 0;
		
		switch(CFGb_E_MODE(i)){
			case 1:{//����/��ݼ�
				if(EC_flag){
					if(EC_pulse[i]){//���ϴ��ѷ����򱾴μ��
						EC_pulse[i] = 0;//��ձ�־
						EC_count[i] -= EC_flag * 2 - 3;//��������
					}
					else{
						if(CFG_E_KEY(i, EC_flag)) keyInsert(0xFF, CFG_E_KEY(i, EC_flag));//�����ֵ
						KeyBrd_data[1] |= CFG_E_FUNC(i, EC_flag);//���빦�ܼ�
						EC_pulse[i] = 1;//�����ѷ������´μ��
					}
				}
				break;
			}
			default:{
				EC_count[i] = TIM_count[i]/*/4*/;
				break;
			}
		}
	}
}








