
#include "ParaUse.h"

//******************************报文发送******************************//
extern uint8_t KeyBrd_data[];//键盘报文
extern uint8_t Mouse_data[];//鼠标报文
extern uint8_t Point_data[];//指针报文
extern uint8_t Vol_data[];//音量报文
uint8_t KeyBrd_data_old[KB_LEN];//上次键盘报文
uint8_t Mouse_data_old = 0;//上次鼠标报文
uint8_t Vol_data_old = 0;//上次音量报文
extern uint8_t KeyBrd_if_send;//键盘报文是否发送
extern uint8_t Vol_if_send;//音量报文是否发送
extern uint8_t Point_if_send;//指针报文是否发送
extern uint8_t Mouse_if_send;//鼠标报文是否发送
//********************************************************************//

uint8_t clickerNum = 0;//自动连点数
uint32_t changeTime = -10000*0;//配置切换时间

//******************************数据相关******************************//
uint8_t mode3_key = 0;//模式3按键(1-16)
uint16_t mode3_i = 0;//模式3源数据下标(访问mode3_data)
uint16_t mode3_loop_count = 0;//模式3循环计数
uint8_t mode3_loop_flag = 0;//模式3循环操作标志
uint8_t mode3_pulse = 0;//模式3间隔
uint8_t mode3_delaying = 0;//模式3是否延时中
//********************************************************************//
uint8_t switch_i = 0xFF, switch_count = 0;//切换键选择和计数
uint8_t switch_key = 0, switch_func = 0;//切换键缓存


uint8_t FillReport(void)//报文填写
{
	static uint32_t oldTime = 0;//记录时间
	uint8_t mode1_num = 0, mode2_num = 0, mode7_num = 0;
	uint8_t i = 0;//公共用
//	struct config_key *fill_key = cfg_key[sysCs];
	
	uint16_t x, y;
	uint8_t turn_old, turn_dif;
	int8_t auto_num;
	
	//***********************************各报文及发送标志初始化***********************************//
	KeyBrd_if_send = Mouse_if_send = Point_if_send = Vol_if_send = 0;//发送标志置零
	
	memcpy(KeyBrd_data_old + 1, KeyBrd_data + 1, KB_LEN - 1);//记录上一次报文
	memset(KeyBrd_data + 1, 0, KB_LEN - 1);//清除所有键

	Mouse_data_old = Mouse_data[1];//记录上一次报文
	memset(Mouse_data + 1, 0, 4);//清除鼠标报文

	Point_data[1] = 0x10;
	memset(Point_data + 3, 0xFF, 4);//清除指针报文

	Vol_data_old = Vol_data[1];//记录上一次报文
	Vol_data[1] = 0;//清除音量报文
	//********************************************************************************************//
	
	if(keyAddr[sysCs][0] == 0) return 0xFF;//若本配置数据错误则退出
	
	//****************************************键盘按键处理****************************************//
	if(mode3_key){//若处于mode3未完成状态
		if(keyOld[mode3_key - 1] && !keyNow[mode3_key - 1] && mode3_loop_flag < 2) mode3_loop_flag++;//释放沿计数
		mode3_pulse = !mode3_pulse;//间隔标志先翻转
		if(mode3_pulse) Mode3Handle();//若翻转后要间隔也即翻转前不用间隔则执行处理
	}
	else{//空闲状态
		for(i = 0; i < 16; i++){//统计按下的各模式数量
			if(keyNow[i/*+1*/]){
				if(CFG_K_MODE(keyAddr[sysCs][i]) == 1 && i != switch_i) mode1_num++;
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 2 && i != switch_i) mode2_num++;
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 7 && i != switch_i) mode7_num++;
			}
		}
		for(i = 0; i < 16; i++){//对于16个按键
			if(i == switch_i){//若有正在生效的临时切换键
				if(!keyNow[i] && !keyOld[i]){//释放沿之后一拍
					switch_count++;//计数滤波
					if(switch_count > 10){
						switch_count = 0;//计数清零
						KeyInsert(i + 3,switch_key);//填入键值
						CsChange(switch_func);//切换回来
						switch_i = 0xFF;//复位
					}
				}
				continue;//屏蔽临时切换键
			}
			if(keyNow[i]){//若按下
				if(CFG_K_MODE(keyAddr[sysCs][i]) == 1){//模式1:单键
					KeyInsert(i + 3,CFG_K_KEY(keyAddr[sysCs][i]));//填入键值
				}
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 2){//模式2:快捷键
					KeyInsert(i + 3,CFG_K_KEY(keyAddr[sysCs][i]));//填入键值
					KeyBrd_data[1] |= CFG_K_FUNC(keyAddr[sysCs][i]);//填入功能键
				}
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 3){//模式3:按键组
					if(mode1_num == 0 && mode2_num == 0 && mode7_num == 0 && !keyOld[i]){//不存在1,2,7模式按键且按下沿
						mode3_key = i + 1;//记录mode3按键(要+1)
						mode3_i = keyAddr[sysCs][i] + 3;//读取起始下标
						mode3_loop_count = 0;//模式3循环计数清零
						mode3_loop_flag = 0;//模式3循环操作标志清零
						mode3_pulse = 1;//插入间隔
						Mode3Handle();
					}
				}
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 4 || CFG_K_MODE(keyAddr[sysCs][i]) == 5){//模式4:光标移位,模式5:光标点击
					if(!keyOld[i]) keyFlag[i] = 2;//待按下态
					
					x = CFG_K_X(keyAddr[sysCs][i]) * 32768 / CFG_SCN_W;
					y = CFG_K_Y(keyAddr[sysCs][i]) * 32768 / CFG_SCN_H;
					Point_data[3] = x & 0xFF;
					Point_data[4] = (x >> 8) & 0xFF;
					Point_data[5] = y & 0xFF;
					Point_data[6] = (y >> 8) & 0xFF;
					if(CFG_K_MODE(keyAddr[sysCs][i]) == 5){
						Point_data[1] |= 0x01;
					}
					
					Point_data[2] = i;
				}
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 6){//模式6:切换键
					if((CFG_K_FUNC(keyAddr[sysCs][i]) & 0x80) && switch_i == 0xFF && !keyOld[i]){//若为临时切换且为独有且按下沿
						switch_key = CFG_K_KEY(keyAddr[sysCs][i]);//缓存键值
						switch_func = sysCs + 1;//缓存旧键盘选择
						turn_old = keyDir[sysCs];//旧键盘方向
						CsChange(CFG_K_FUNC(keyAddr[sysCs][i]));//切换
						turn_dif = (keyDir[sysCs] + 4 - turn_old) % 4;//相对旧键盘方向
						if(turn_dif == 0) switch_i = i;
						else if(turn_dif == 1) switch_i = TURN_L90[i];
						else if(turn_dif == 2) switch_i = 16 - i;
						else if(turn_dif == 3) switch_i = TURN_R90[i];
						break;//跳出本次循环
					}
				}
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 7){//模式7:按键连点
					if(!keyOld[i] && !(CFG_K_FUNC(keyAddr[sysCs][i]) & 0x01)){//若为按下沿且非自动连点
						keyFlag[i] |= 0x02;//启动连点
						keyWork[i] = 0;//计时器清零
					}
				}
			}
			else if(keyOld[i]){//若刚抬起(释放沿)
				if(CFG_K_MODE(keyAddr[sysCs][i]) == 5){//模式5:光标点击
					if(keyOld[i]){//释放沿
						keyFlag[i] = 1;//待抬起态
					}
				}
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 6){//模式6:切换键
					if(!(CFG_K_FUNC(keyAddr[sysCs][i]) & 0x80)){//若为非临时切换
						switch_i = 0xFF;//直接复位临时切换键标志
						CsChange(CFG_K_FUNC(keyAddr[sysCs][i]));//切换
					}
				}
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 7){//模式7:按键连点
					if((CFG_K_FUNC(keyAddr[sysCs][i]) & 0x01) && !(keyFlag[i] & 0x02)){//自动连点且当前未在连点
						keyFlag[i] |= 0x02;//启动连点
						keyWork[i] = 0;//计时器清零
					}
					else keyFlag[i] &= ~0x02;//停止连点
				}
				else if(CFG_K_MODE(keyAddr[sysCs][i]) == 8){//模式8:蜂鸣器
					return 1;
				}
			}
		}//处理完16个按键的主要内容
		
		for(i = 0; i < 16; i++){//对于16个按键的触摸
			if(CFG_K_MODE(keyAddr[sysCs][i]) == 4 || CFG_K_MODE(keyAddr[sysCs][i]) == 5){//模式4:光标移位,模式5:光标点击
				if(CFG_K_FUNC(keyAddr[sysCs][i]) == 1 || CFG_K_FUNC(keyAddr[sysCs][i]) == 2){
					x = CFG_K_X(keyAddr[sysCs][i]) * 32768 / CFG_SCN_W;
					y = CFG_K_Y(keyAddr[sysCs][i]) * 32768 / CFG_SCN_H;
					Point_data[3] = x & 0xFF;
					Point_data[4] = (x >> 8) & 0xFF;
					Point_data[5] = y & 0xFF;
					Point_data[6] = (y >> 8) & 0xFF;
					
					Point_data[2] = i;
					
					if(CFG_K_FUNC(keyAddr[sysCs][i]) == 2){
						if(CFG_K_MODE(keyAddr[sysCs][i]) == 5) Point_data[1] |= 0x01;
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
		}//处理完16个按键的触摸
		
		/*int8_t */auto_num = 0;//正在执行自动连点的按键数量
		for(i = 0; i < 16; i++){//对于16个按键的连点
			if(CFG_K_MODE(keyAddr[sysCs][i]) == 7){//模式7:按键连点
				if(oldTime > Systime){//若系统时间重置
					keyWork[i] = 0;//清空设定时间
					keyFlag[i] &= ~0x04;//复位点击标志
					keyFlag[i] &= ~0x02;//停止连点
				}
				else if(keyFlag[i] & 0x02){//若连点启用
					if(CFG_K_T(keyAddr[sysCs][i]) == 0){//若周期为0则长按
						KeyInsert(i + 3,CFG_K_KEY(keyAddr[sysCs][i]));//填入键值
					}
					else if(keyFlag[i] & 0x04){//若上次已点击
						keyFlag[i] &= ~0x04;//复位点击标志
					}
					else{//若上次未点击
						if((uint16_t)((uint16_t)Systime - keyWork[i]) >= CFG_K_T(keyAddr[sysCs][i])){//若定时已到
							KeyInsert(i + 3,CFG_K_KEY(keyAddr[sysCs][i]));//填入键值
							keyWork[i] = Systime;//记录发送时间低16位
							keyFlag[i] |= 0x04;//置位点击标志
						}
					}
					if(CFG_K_FUNC(keyAddr[sysCs][i]) & 0x01) auto_num++;//自动连点按键计数
				}
			}
		}//处理完16个按键的连点
		oldTime = Systime;//记录时间更新
		clickerNum = auto_num;//自动连点数更新
	}
	//********************************************************************************************//
	
	//***********************************摇杆旋钮处理***********************************//
	RkHandle(0);//摇杆处理
	EcHandle(0);//旋钮处理
	if(!mode3_key) RkEcKeyHandle();//摇杆旋钮按键处理
	//**********************************************************************************//
	
	//***********************************判断各报文是否要发送***********************************//
	for(i = 1; i < KB_LEN; i++){
		if(KeyBrd_data_old[i] != KeyBrd_data[i]){//键盘报文与上一次不同则发送
			KeyBrd_if_send = 1;	break;
		}
	}
	if(Mouse_data[1] != Mouse_data_old) Mouse_if_send = 1;//鼠标按键与上次不同则发送
	else{
		for(i = 2; i < 5; i++){
			if(Mouse_data[i] != 0){//鼠标存在移动或滚动则发送
				Mouse_if_send = 1;	break;
			}
		}
	}
	for(i = 3; i < 7; i++){
		if(Point_data[i] != 0xFF){//触摸存在有效坐标则发送
			Point_if_send = 1;	break;
		}
	}
	if(Vol_data[1] != Vol_data_old) Vol_if_send = 1;//媒体报文与上次不同则发送
	//******************************************************************************************//
	
	if(switch_i != 0xFF 
		&& (KeyBrd_if_send || Mouse_if_send || Point_if_send || Vol_if_send)){
		switch_key = 0;//若临时切换键按下期间有其他操作则清除切换键自身键值
	}
	
	if(mode3_delaying == 1){//延时中则发送标志置零
		KeyBrd_if_send = Mouse_if_send = Point_if_send = Vol_if_send = 0;
	}
	
	return 0;
}

void CsChange(uint8_t change)//切换
{
	change &= 0x0F;//取低4位
	if(change == 0 || change > CFG_NUM) return;
	sysCs = change - 1;
	DATA_CFG = DATA_CFG_BASE - sysCs * 512;//修改键盘配置指针 逆序
	
	KeyRGB(1);//键盘RGB控制清零
	if(LIGHT_MONO != 3) DATA_LIGHT = DATA_LIGHT_BASE + sysCs * 256;//若不是灯效不切换则修改灯效配置指针 正序
	
	changeTime = Systime;
	
	RkHandle(1);
	EcHandle(1);
	
	memset(keyWork,0,sizeof(keyWork));
	memset(keyFlag,0,sizeof(keyFlag));
}

void KeyInsert(uint8_t r_i, uint8_t key_v)//单键填入
{
	if(key_v == kv_wheel_down) Mouse_data[4] += -1;//滚轮向下
	else if(key_v == kv_wheel_up) Mouse_data[4] += 1;//滚轮向上
	else if(key_v == kv_vol_up) Vol_data[1] |= 0x01;//音量加
	else if(key_v == kv_vol_down) Vol_data[1] |= 0x02;//音量减
	else if(key_v == kv_vol_mute) Vol_data[1] |= 0x04;//静音
	else if(key_v == kv_vol_stop) Vol_data[1] |= 0x08;//播放暂停
	else if(key_v == kv_vol_next) Vol_data[1] |= 0x10;//下一个
	else if(key_v == kv_vol_prev) Vol_data[1] |= 0x20;//上一个
	else if(key_v == kv_mouse_l) Mouse_data[1] |= 0x01;//鼠标左键
	else if(key_v == kv_mouse_m) Mouse_data[1] |= 0x04;//鼠标中键
	else if(key_v == kv_mouse_r) Mouse_data[1] |= 0x02;//鼠标右键
	else if(key_v == kv_ctrl) KeyBrd_data[1] |= 0x01;//ctrl
	else if(key_v == kv_shift) KeyBrd_data[1] |= 0x02;//shift
	else if(key_v == kv_alt) KeyBrd_data[1] |= 0x04;//alt
	else if(key_v == kv_win) KeyBrd_data[1] |= 0x08;//win
	else{//普通键盘按键
		if(r_i == 0xFF){//若使用自动填入
			for(r_i = 3; r_i < KB_LEN; r_i++){//搜索空位
				if(!KeyBrd_data[r_i]) break;//若此处为空
			}
		}
		if(r_i == 0xFF) return;//若未找到空位则退出
		KeyBrd_data[r_i] = key_v;//填入键值
	}
}

void Mode3Handle(void)//mode3处理(按键组处理)
{
	static uint32_t setTime = 0, oldTime = 0;//设定时间及记录时间
	static uint16_t loopStart = 0xFFFF;//循环起始地址
	uint8_t report_i = 3;//报文写入位置
	
	uint16_t x, y;
	uint16_t end_i;
	uint8_t check_i;
//	uint8_t mode3_key = 0;//模式3按键(1-16)
//	uint16_t mode3_i = 0;//模式3源数据下标(访问mode3_data)
//	uint16_t mode3_loop_count = 0xFFFF;//模式3循环计数
//	uint8_t mode3_loop_flag = 0;//模式3循环操作标志
	
	if(oldTime > Systime) setTime = 0;//若系统时间重置则终止延时
	oldTime = Systime;//记录时间更新
	if(setTime > Systime){//延时未结束
		mode3_delaying = 1;//延时标志置位
		mode3_pulse = 0;//清零暂停标志
		return;//退出等待
	}
	mode3_delaying = 0;//延时标志清零
	
	end_i = 0;//结束位置
	if(mode3_key) end_i = keyAddr[sysCs][mode3_key - 1] + 3 + CFG_K_LEN(keyAddr[sysCs][mode3_key - 1]);
	
	for(;report_i < KB_LEN;){//当报文未填满
		if(mode3_i >= end_i){//当读完数据
			mode3_key = 0;
			break;
		}
		if(CFG_ACS(mode3_i) == kv_report){//报文控制
			mode3_pulse = CFG_ACS(mode3_i + 1) & 0x01;//清零或置位暂停标志
			if(CFG_ACS(mode3_i + 1) & 0x02){//作为报文中止
				mode3_i += 2;
				break;//独占本次报文
			}
			else mode3_i++;
		}
		else if(CFG_ACS(mode3_i) == kv_loop){//若有循环
			if(CFG_ACS(mode3_i + 1) == 0x02){//若为起始符
				loopStart = mode3_i + 4;//循环起始地址
				mode3_i += 4;
				continue;
			}
			mode3_loop_count++;//循环计数递增
			x = ((uint16_t)CFG_ACS(mode3_i + 2) << 8) | CFG_ACS(mode3_i + 3);//借用x存储循环次数
			if(CFG_ACS(mode3_i + 1) && mode3_loop_flag == 2 || !CFG_ACS(mode3_i + 1) && mode3_loop_flag
				|| x && mode3_loop_count >= x){//若下降沿次数达标或计数到位
				mode3_loop_flag = 3;//置为结束标志
			}
			if(mode3_loop_flag == 3){//结束循环
				mode3_i += 3;//下标跳过循环单元
				mode3_loop_flag = mode3_loop_count = 0;//操作标志和计数都清零
				loopStart = 0xFFFF;//循环起始地址复位
			}
			else{//继续循环
				if(loopStart != 0xFFFF) mode3_i = loopStart;//下标跳转回循环起始地址
				else mode3_i = keyAddr[sysCs][mode3_key - 1] + 3;//下标跳转回开头
				break;//独占本次报文
			}
		}
		else if(CFG_ACS(mode3_i) == kv_delay){//若有延时
			uint16_t delayTime = (CFG_ACS(mode3_i + 1) << 8) | CFG_ACS(mode3_i + 2);
			setTime = Systime + delayTime;
//			mode3_pulse = 0;//清零暂停标志
			mode3_i += 2;
			if(++mode3_i == end_i) mode3_key = 0;//当读完数据
			break;//独占本次报文
		}
		else if(CFG_ACS(mode3_i) == kv_shortcut){//若有快捷键
			if(report_i > 3 || Mouse_data[1] != 0) break;//若本次报文已有内容则退出等下一次
			KeyInsert(report_i,CFG_ACS(++mode3_i));//填入键值
			KeyBrd_data[1] = CFG_ACS(++mode3_i);//填入功能键
			if(++mode3_i == end_i) mode3_key = 0;//当读完数据
			break;//独占本次报文
		}
		else if(CFG_ACS(mode3_i) == kv_point){//若有光标移位
			if(report_i > 3 || Mouse_data[1] != 0) break;//若本次报文已有内容则退出等下一次
			x = (CFG_ACS(mode3_i + 1) << 8) | CFG_ACS(mode3_i + 2);
			y = (CFG_ACS(mode3_i + 3) << 8) | CFG_ACS(mode3_i + 4);
			x = x * 32768 / CFG_SCN_W;
			y = y * 32768 / CFG_SCN_H;
			Point_data[3] = x & 0xFF;
			Point_data[4] = (x >> 8) & 0xFF;
			Point_data[5] = y & 0xFF;
			Point_data[6] = (y >> 8) & 0xFF;
			if(1){
				Point_data[1] |= 0x01;//光标点击
			}
			mode3_i += 4;
			if(++mode3_i == end_i) mode3_key = 0;//当读完数据
			break;//独占本次报文
		}
		else if(CFG_ACS(mode3_i) == kv_shift){//若有shift
			check_i = 3;
			for(check_i = 3; check_i < report_i; check_i++){//检查是否已有相同键值
				if(KeyBrd_data[check_i] == CFG_ACS(mode3_i)){
					check_i = 0;
					break;
				}
			}
			if(!check_i) break;//若已有相同键值则退出等下一次
			if(Mouse_data[1] != 0) break;//若鼠标报文已有内容则退出等下一次
			if(KeyBrd_data[1]){//若报文已含shift
				KeyInsert(report_i++,CFG_ACS(++mode3_i));//填入键值
			}
			else if(report_i == 3){//当是起始
				KeyInsert(report_i++,CFG_ACS(++mode3_i));//填入键值
				KeyBrd_data[1] |= 0x02;//填入shift
			}
			else break;//因前面排斥shift而无法填入
		}
		else{//若无shift
			check_i = 3;
			for(check_i = 3; check_i < report_i; check_i++){//检查是否已有相同键值
				if(KeyBrd_data[check_i] == CFG_ACS(mode3_i)){
					check_i = 0;
					break;
				}
			}
			if(!check_i) break;//若已有相同键值则退出等下一次
			if(Mouse_data[1] != 0) break;//若鼠标报文已有内容则退出等下一次
			if(!KeyBrd_data[1]){//若报文未含shift
				KeyInsert(report_i++,CFG_ACS(mode3_i));//填入键值
			}
			else if(report_i == 3){//当是起始
				KeyInsert(report_i++,CFG_ACS(mode3_i));//填入键值
			}
			else break;//因前面已有shift而无法填入
		}
		mode3_i++;
//		if(mode3_i >= end_i){//当读完数据
//			mode3_key = 0;
//			break;
//		}
	}
}

//1:按键/快捷键,2:永久切换键
void RkEcKeyHandle(void)//摇杆旋钮按键处理
{
	uint8_t keyM[3], keyV[3], keyF[3];//按键模式,键值,功能键
	uint8_t i;
	
	keyM[0] = CFGb_Ek_MODE(0);	keyV[0] = CFG_E_KEY(0,0);	keyF[0] = CFG_E_FUNC(0,0);
	keyM[1] = CFGb_Ek_MODE(1);	keyV[1] = CFG_E_KEY(1,0);	keyF[1] = CFG_E_FUNC(1,0);
	keyM[2] = CFGb_Rk_MODE(0);	keyV[2] = CFG_R_KEY(0,0);	keyF[2] = CFG_R_FUNC(0);
	
	for(i = 0; i < 3; i++){//2个旋钮按键1个摇杆按键
		if(keyNow[i + 16]){//若为按下状态
			if(keyM[i] == 1){//按键/快捷键
				if(keyV[i]) KeyInsert(0xFF, keyV[i]);//填入键值
				KeyBrd_data[1] |= keyF[i];//填入功能键
			}
		}
		else if(keyOld[i + 16]){//若为释放沿
			if(keyM[i] == 2){//永久切换键
				switch_i = 0xFF;//直接复位临时切换键标志
				CsChange(keyV[i] - kv_orig_1 + 1);//切换 该函数内有参数检查 此处USB键值从数字键1的键值开始有效
			}
		}
	}
}

//1:四向四按键,2:八向四按键,3:速度鼠标,4:光标位置
void RkHandle(uint8_t clear)//摇杆处理
{
	static int16_t RK_pulse = 0;//间隔标志
	static int16_t x_pic = 0, y_pic = 0;
	static int16_t dx = 0, dy = 0;
	int16_t x, y;
	int16_t equal_r;//等效半径
	
	if(clear){//清除
		RK_pulse = x_pic = y_pic = 0;
		return;
	}
	
	x = (CFGb_R_DIRx(0)*2 - 1);//决定方向
	y = (CFGb_R_DIRy(0)*2 - 1);
	
	if(CFGb_R_DIRr(0)){//若转90度
		if(adcValue[0] < ANA_MID_0) x *= ((int16_t)adcValue[0] - (int16_t)ANA_MID_0) * 4096L / ANA_MID_0;
		else x *= ((int16_t)adcValue[0] - (int16_t)ANA_MID_0) * 4096L / (4095 - ANA_MID_0);//放大到正负4096
		
		if(adcValue[1] < ANA_MID_1) y *= ((int16_t)adcValue[1] - (int16_t)ANA_MID_1) * 4096L / ANA_MID_1;
		else y *= ((int16_t)adcValue[1] - (int16_t)ANA_MID_1) * 4096L / (4095 - ANA_MID_1);
	}
	else{
		if(adcValue[0] < ANA_MID_0) y *= ((int16_t)adcValue[0] - (int16_t)ANA_MID_0) * 4096L / ANA_MID_0;//向上为正
		else y *= ((int16_t)adcValue[0] - (int16_t)ANA_MID_0) * 4096L / (4095 - ANA_MID_0);//放大到正负4096
		
		if(adcValue[1] < ANA_MID_1) x *= -((int16_t)adcValue[1] - (int16_t)ANA_MID_1) * 4096L / ANA_MID_1;//向右为正
		else x *= -((int16_t)adcValue[1] - (int16_t)ANA_MID_1) * 4096L / (4095 - ANA_MID_1);
	}
	
	equal_r = MAX(ABS(x), ABS(y)) - (uint16_t)CFG_R_DEAD(0) * 21;//计算等效半径并减去死区
	if(equal_r <= 0){//在死区内
		dx = dy = 0;
		x_pic = y_pic = 0;
		return;//退出
	}
	
	switch(CFGb_R_MODE(0)){
		case 1:{//四向四按键
			if(RK_pulse){//若上次已发送则本次间隔
				RK_pulse--;//标志递减
				break;
			}
			
			if(CFG_R_NEER(0) == 0 && CFG_R_FAR(0) == 0) RK_pulse = 0;//长按
			else{//非长按则间隔按
				equal_r = ((uint16_t)CFG_R_NEER(0) << 1) + (((int8_t)(CFG_R_FAR(0) - CFG_R_NEER(0)) * (equal_r >> 3)) >> 8);//计算速度 借用equal_r存储

				if(equal_r <= ABS(dx)) dx = equal_r;//减速时直接赋值不使用惯性
				else{//加速时使用惯性
					equal_r = (equal_r - dx) / (CFG_R_PARA(0) + 1);//借用equal_r存储
					if(equal_r == 0) dx += 1;//不足1则为1
					else dx += equal_r;
					equal_r = dx;
				}
				
				RK_pulse = 100 - equal_r;
				if(RK_pulse <= 0) RK_pulse = 1;
			}
			
			if(y > x){
				if(y > -x) KeyInsert(0xFF, CFG_R_KEY(0,1));//填入上键值
				else KeyInsert(0xFF, CFG_R_KEY(0,3));//填入左键值
			}
			else{
				if(y > -x) KeyInsert(0xFF, CFG_R_KEY(0,4));//填入右键值
				else KeyInsert(0xFF, CFG_R_KEY(0,2));//填入下键值
			}
			break;
		}
		case 2:{//八向四按键
			if(RK_pulse){//若上次已发送则本次间隔
				RK_pulse--;//标志递减
				break;
			}
			
			if(CFG_R_NEER(0) == 0 && CFG_R_FAR(0) == 0) RK_pulse = 0;//长按
			else{
				equal_r = ((uint16_t)CFG_R_NEER(0) << 1) + (((int8_t)(CFG_R_FAR(0) - CFG_R_NEER(0)) * (equal_r >> 3)) >> 8);//计算速度 借用equal_r存储
				
				if(equal_r <= ABS(dx)) dx = equal_r;//减速时直接赋值不使用惯性
				else{//加速时使用惯性
					equal_r = (equal_r - dx) / (CFG_R_PARA(0) + 1);//借用equal_r存储
					if(equal_r == 0) dx += 1;//不足1则为1
					else dx += equal_r;
					equal_r = dx;
				}
				
				RK_pulse = 100 - equal_r;
				if(RK_pulse <= 0) RK_pulse = 1;
			}
			
			//atan(5/12)=22.6度 atan(3/7)=23.2度
			if(y*7 > x*3 && y*7 > -x*3){//上
				KeyInsert(0xFF, CFG_R_KEY(0,1));//填入键值
			}
			else if(y*7 < x*3 && y*7 < -x*3){//下
				KeyInsert(0xFF, CFG_R_KEY(0,2));//填入键值
			}
			if(y*3 > x*7 && y*3 < -x*7){//左
				KeyInsert(0xFF, CFG_R_KEY(0,3));//填入键值
			}
			else if(y*3 < x*7 && y*3 > -x*7){//右
				KeyInsert(0xFF, CFG_R_KEY(0,4));//填入键值
			}
			break;
		}
		case 3:{//速度鼠标
//			x = x > 0 ? MAX(0, x - (int16_t)CFG_R_DEAD(0) * 21) : MIN(0, x + (int16_t)CFG_R_DEAD(0) * 21);//应用死区
//			y = y > 0 ? MAX(0, y - (int16_t)CFG_R_DEAD(0) * 21) : MIN(0, y + (int16_t)CFG_R_DEAD(0) * 21);//应用死区

			x = ((int8_t)CFG_R_FAR(0) * (x >> 3)) >> 4;//计算速度 借用x存储
			y = ((int8_t)CFG_R_FAR(0) * (y >> 3)) >> 4;//计算速度 借用y存储
			//-1600~1600
			if(ABS(x) <= ABS(dx)) dx = x;//减速时直接赋值不使用惯性
			else if(ABS(dx) > 800) dx = x;//高速阶段不使用惯性
			else if(ABS(dx) > CFG_R_NEER(0) * 10){//中速阶段使用低惯性
				equal_r = (x - dx) / (CFG_R_PARA(0) + 1);//借用equal_r存储
				if(equal_r == 0) dx += SIGN(x);//不足1则为1
				else dx += equal_r;
			}
			else{//低速阶段使用高惯性
				equal_r = (x - dx) / (CFG_R_PARA(0) * 10 + 1);//借用equal_r存储
				if(equal_r == 0) dx += SIGN(x);//不足1则为1
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

			x_pic += dx;//增量式 以改善余数舍弃问题
			y_pic += dy;

			x_pic = LIMIT(x_pic, -127 << 6, 127 << 6);//限幅
			y_pic = LIMIT(y_pic, -127 << 6, 127 << 6);
			Mouse_data[2] = (int8_t)(x_pic >> 6);//报文填入
			Mouse_data[3] = -(int8_t)(y_pic >> 6);
			x_pic -= (int8_t)Mouse_data[2] << 6;//减去已填入的
			y_pic += (int8_t)Mouse_data[3] << 6;
			break;
		}
		case 4:{//光标位置
			equal_r = (x - x_pic) >> 2;//防抖滤波 借用equal_r存储
			if(equal_r == 0) x_pic += SIGN(x);
			else x_pic += equal_r;
			equal_r = (y - y_pic) >> 2;
			if(equal_r == 0) y_pic += SIGN(y);
			else y_pic += equal_r;
			
			if(x > 0) dx = CFG_R_KEY(0,4);	//右
			else dx = CFG_R_KEY(0,3);		//左
			if(y > 0) dy = CFG_R_KEY(0,1);	//上
			else dy = CFG_R_KEY(0,2);		//下
			
			if(dx >= kv_orig_1 && dx <= kv_orig_0){//1~9以及0
				dx = (dx - kv_orig_1) * 2 + 2;//2~20 间隔2
			}
			else if(dx >= kv_num_1 && dx <= kv_num_0){//NUM1~NUM9
				dx = (dx - kv_num_1) * 2 + 1;//1~19 间隔2
			}
			else dx = 0;
			
			if(dy >= kv_orig_1 && dy <= kv_orig_0){//1~9以及0
				dy = (dy - kv_orig_1) * 2 + 2;//2~20 间隔2
			}
			else if(dy >= kv_num_1 && dy <= kv_num_0){//NUM1~NUM9
				dy = (dy - kv_num_1) * 2 + 1;//1~19 间隔2
			}
			else dy = 0;
			
			x = LIMIT(x, -4095, 4095);
			y = LIMIT(y, -4095, 4095);
			
			//20*4095*50/125
			dx = (int32_t)dx * x_pic * CFG_R_FAR(0) / 125;//计算偏移量
			dy = -(int32_t)dy * y_pic * CFG_R_FAR(0) / 125;
			
			x = CFG_R_PARA(0) * 32767L / 50;//计算中心位置
			y = CFG_R_NEER(0) * 32767L / 50;
			
			if(dx > 0 && (int32_t)x + dx > 32767) x = 32767;//防止正溢出
			else if(dx < 0 && (int32_t)x + dx < 0) x = 0;//防止负溢出
			else x += dx;//没有溢出
			
			if(dy > 0 && (int32_t)y + dy > 32767) y = 32767;
			else if(dy < 0 && (int32_t)y + dy < 0) y = 0;
			else y += dy;

			Point_data[3] = x & 0xFF;
			Point_data[4] = (x >> 8) & 0xFF;
			Point_data[5] = y & 0xFF;
			Point_data[6] = (y >> 8) & 0xFF;
			
			Point_data[2] = 17;//提供一个和按键不一样的ID
			
			RK_pulse = !RK_pulse;//借用RK_pulse存储一个01交替的量
			Mouse_data[2] = RK_pulse * 2 - 1;//报文填入 用x抖动来维持鼠标光标显示
			
			break;
		}
		default:break;
	}
}

//1:按键/快捷键
void EcHandle(uint8_t clear)//旋钮处理
{
//	static uint32_t oldTime = 0;//记录时间
//	static int TIM_old = 0;//编码器旧计数
	static int8_t TIM_count[2] = {0,0};//编码器计数
	static int8_t EC_count[2] = {0,0};//执行计数
	static uint8_t EC_pulse[2] = {0,0};//间隔标志
	uint8_t EC_flag = 0;//执行标志
	
	uint8_t i;
	
	if(clear){//清除
		TIM_count[0] = TIM_count[1] = EC_count[0] = EC_count[2] = 0;
		EC_pulse[0] = EC_pulse[1] = EC_flag = 0;
		return;
	}

	TIM_count[0] -= (CFG_E_DIR(0) * 2 - 1) * (int8_t)(EC1val)/**4*/;//编码器计数读取
	EC1val = 0;//编码器清零
	TIM_count[1] -= (CFG_E_DIR(0) * 2 - 1) * (int8_t)(EC2val)/**4*/;//编码器计数读取
	EC2val = 0;//编码器清零
	
//	if(TIM_old != TIM_count){
//		TIM_old = TIM_count;
//		oldTime = Systime;//若计数值不一致才更新时间
//	}
//	if(/*Systime - oldTime > 5000*/0){//若长时间编码器无动作
//		TIM_old = TIM_count = EC_count = EC_flag = 0;
//		oldTime = Systime;
//	}
	
	for(i = 0; i < 2; i++){//处理每个旋钮
		if(TIM_count[i] > EC_count[i]) EC_flag = 1;
		else if(TIM_count[i] < EC_count[i]) EC_flag = 2;
		else EC_flag = 0;
		
		switch(CFGb_E_MODE(i)){
			case 1:{//按键/快捷键
				if(EC_flag){
					if(EC_pulse[i]){//若上次已发送则本次间隔
						EC_pulse[i] = 0;//清空标志
						EC_count[i] -= EC_flag * 2 - 3;//计数跟进
					}
					else{
						if(CFG_E_KEY(i, EC_flag)) KeyInsert(0xFF, CFG_E_KEY(i, EC_flag));//填入键值
						KeyBrd_data[1] |= CFG_E_FUNC(i, EC_flag);//填入功能键
						EC_pulse[i] = 1;//本次已发送则下次间隔
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








