/********************************** (C) COPYRIGHT *******************************
* File Name          : GPIO.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 GPIO相关函数
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "GPIO.H"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : GPIO_Init(UINT8 PORTx,UINT8 PINx,UINT8 MODEx)
* Description    : GPIO端口初始化函数
* Input          : PORTx:0~4
*                  PINx:位域,每个位对应该Port的一个引脚
*                  MODEx:
*                        0:高阻输入模式，引脚没有上拉电阻
*                        1:推挽输出模式，具有对称驱动能力，可以输出或者吸收较大电流
*                        2:开漏输出，支持高阻输入，引脚没有上拉电阻
*                        3:准双向模式(标准 8051)，开漏输出，支持输入，引脚有上拉电阻(默认模式)
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Init(UINT8 PORTx,UINT8 PINx,UINT8 MODEx)
{
    UINT8 Px_DIR_PU,Px_MOD_OC;
    switch(PORTx)                                         //读出初始值
    {
    case PORT0:
        Px_MOD_OC = P0_MOD_OC;
        Px_DIR_PU = P0_DIR_PU;
        break;
    case PORT1:
        Px_MOD_OC = P1_MOD_OC;
        Px_DIR_PU = P1_DIR_PU;
        break;
    case PORT2:
        Px_MOD_OC = P2_MOD_OC;
        Px_DIR_PU = P2_DIR_PU;
        break;
    case PORT3:
        Px_MOD_OC = P3_MOD_OC;
        Px_DIR_PU = P3_DIR_PU;
        break;
    case PORT4:
        Px_MOD_OC = P4_MOD_OC;
        Px_DIR_PU = P4_DIR_PU;
        break;
    default :
        break;
    }
    switch(MODEx)
    {
    case MODE0:                                       //高阻输入模式，引脚没有上拉电阻
        Px_MOD_OC &= ~PINx;
        Px_DIR_PU &= ~PINx;
        break;
    case MODE1:                                       //推挽输出模式，具有对称驱动能力，可以输出或者吸收较大电流
        Px_MOD_OC &= ~PINx;
        Px_DIR_PU |= PINx;
        break;
    case MODE2:                                       //开漏输出，支持高阻输入，引脚没有上拉电阻
        Px_MOD_OC |= PINx;
        Px_DIR_PU &= ~PINx;
        break;
    case MODE3:                                       //准双向模式(标准 8051)，开漏输出，支持输入，引脚有上拉电阻
        Px_MOD_OC |= PINx;
        Px_DIR_PU |= PINx;
        break;
    default :
        break;
    }
    switch(PORTx)                                         //回写
    {
    case PORT0:
        P0_MOD_OC = Px_MOD_OC;
        P0_DIR_PU = Px_DIR_PU;
        break;
    case PORT1:
        P1_MOD_OC = Px_MOD_OC;
        P1_DIR_PU = Px_DIR_PU;
        break;
    case PORT2:
        P2_MOD_OC = Px_MOD_OC;
        P2_DIR_PU = Px_DIR_PU;
        break;
    case PORT3:
        P3_MOD_OC = Px_MOD_OC;
        P3_DIR_PU = Px_DIR_PU;
        break;
    case PORT4:
        P4_MOD_OC = Px_MOD_OC;
        P4_DIR_PU = Px_DIR_PU;
        break;
    default :
        break;
    }
}
/*******************************************************************************
* Function Name  : GPIO_INT_Init
* Description    : 可设置 RXD1_L、P15_L、P14_L、P03_L、P57_H、P46_L、RXD0_L 扩展引脚的外部中断
*                  同时还包含兼容C51的 INT1_L、INT0_L 的外部中断触发
*                  (RXD1_L、RXD0_L具体是哪个引脚取决于引脚是否映射)
* Input          : IntSrc:共9个外部中断，按位域表示，具体定义见GPIO.H
*                  Mode：0：电平模式   1：边沿模式 (注意扩展引脚的中断模式是统一配置的)
*                  NewState：0：关闭对应外部中断使能  1：开启对应外部中断
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_INT_Init( UINT16 IntSrc,UINT8 Mode,UINT8 NewState )
{
    /* 中断触发模式设置 */
    if(Mode == INT_EDGE)                                //边沿触发模式
    {
        if(IntSrc & 0x7F)                               //存在扩展中断
        {
            GPIO_IE |= bIE_IO_EDGE;
        }
        if(IntSrc&INT_INT0_L)                           //存在外部中断0
        {
            IT0 = 1;
        }
        if(IntSrc&INT_INT1_L)                           //存在外部中断1
        {
            IT1 = 1;
        }
    }
    else                                                //电平触发模式
    {
        if(IntSrc & 0x7F)                               //存在扩展中断
        {
            GPIO_IE &= ~bIE_IO_EDGE;
        }
        if(IntSrc&INT_INT0_L)                           //存在外部中断0
        {
            IT0 = 0;
        }
        if(IntSrc&INT_INT1_L)                           //存在外部中断1
        {
            IT1 = 0;
        }
    }
    /* 中断使能状态 */
    if(NewState == Enable)                              //开启外部中断
    {
        GPIO_IE |= ((UINT8)IntSrc&0x7F);
        if(IntSrc&INT_INT0_L)                           //存在外部中断0
        {
            EX0 = 1;
        }
        if(IntSrc&INT_INT1_L)                           //存在外部中断1
        {
            EX1 = 1;
        }
        if(GPIO_IE&0x7F)
        {
            IE_GPIO = 1;                                //开启扩展GPIO中断
        }
        EA = 1;                                         //开启总中断
    }
    else                                                //关闭对应外部中断
    {
        GPIO_IE &= ~((UINT8)IntSrc&0x7F);
        if(IntSrc&INT_INT0_L)                           //存在外部中断0
        {
            EX0 = 0;
        }
        if(IntSrc&INT_INT1_L)                           //存在外部中断1
        {
            EX1 = 0;
        }
        if((GPIO_IE&0x7F)== 0)
        {
            IE_GPIO = 0;                                //关闭扩展GPIO中断
        }
    }
}

/*******************************************************************************
* Function Name  : GPIO_STD1_ISR
* Description    : INT1(P33) 引脚外部中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_STD1_ISR(void) interrupt INT_NO_INT1
{
//    mDelaymS(10);
//    printf("P33 Falling\n");
}

/*******************************************************************************
* Function Name  : GPIO_STD0_ISR & GPIO_STD3_ISR
* Description    : P22(INT0_)↓ P37(INT3)↑ 边沿中断ISR 连接编码器1
* Input          : 函数无输入 检测引脚P22 P24
* Output         : None
* Return         : None
*******************************************************************************/
UINT8D EC1old = 0;//0~3分别为A↓B0,A↓B1,A↑B0,A↑B1
UINT8D EC1val = 0;//计数值
bit EC1freq = 0;//倍频设置
#if 0
#define GPIO_STD03_ISR() \
	register UINT8 ECA = P2_4, ECB = P2_2;/*这里AB反过来*/\
	if(EC1freq){	/*若启用倍频*/\
		if(ECA && ECB || !ECA && !ECB) EC1val++;/*逆时针(只针对开发使用的编码器)*/\
		else						   EC1val--;/*顺时针(只针对开发使用的编码器)*/\
	}else{			/*若不倍频*/\
		register UINT8 ECnew = (ECA << 1) | ECB;/*更新记录状态*/\
		if(EC1old == 0 && ECnew == 3)	   EC1val++;/*逆时针(只针对开发使用的编码器)*/\
		else if(EC1old == 1 && ECnew == 2) EC1val--;/*顺时针(只针对开发使用的编码器)*/\
		EC1old = ECnew;/*更新记录状态*/\
	}
//End of #define GPIO_STD03_ISR()
#endif
#define GPIO_STD03_ISR() \
	register UINT8 ECA = P2_4, ECB = P2_2;/*这里AB反过来*/\
	if(EC1freq){	/*若启用倍频*/\
		if(ECA && ECB || !ECA && !ECB) EC1val++;/*逆时针(只针对开发使用的编码器)*/\
		else						   EC1val--;/*顺时针(只针对开发使用的编码器)*/\
	}else{			/*若不倍频*/\
		register UINT8 ECnew = (ECA << 1) | ECB;/*更新记录状态*/\
		if(/*EC1old == 0 &&*/ ECnew == 3)	   EC1val++;/*逆时针(只针对开发使用的编码器)*/\
		else if(/*EC1old == 1 &&*/ ECnew == 2) EC1val--;/*顺时针(只针对开发使用的编码器)*/\
		EC1old = ECnew;/*更新记录状态*/\
	}
//End of #define GPIO_STD03_ISR()
void GPIO_STD0_ISR(void) interrupt INT_NO_INT0
{
	GPIO_STD03_ISR();
}
void GPIO_STD3_ISR(void) interrupt INT_NO_INT3
{
    GPIO_STD03_ISR();
}

/*******************************************************************************
* Function Name  : GPIO_EXT_ISR
* Description    : P03↓ P57↑ 边沿中断ISR 连接编码器2
* Input          : 函数无输入 检测引脚P01 P03
* Output         : None
* Return         : None
*******************************************************************************/
UINT8D EC2old = 0;//0~3分别为A↓B0,A↓B1,A↑B0,A↑B1
UINT8D EC2val = 0;//计数值
bit EC2freq = 0;//倍频设置
void GPIO_EXT_ISR(void) interrupt INT_NO_GPIO
{
	register UINT8 ECA = P0_1, ECB = P0_3;//这里AB反过来
	if(EC2freq){	//若启用倍频
		if(ECA && ECB || !ECA && !ECB) EC2val--;//顺时针(只针对开发使用的编码器)
		else						   EC2val++;//逆时针(只针对开发使用的编码器)
	}else{			//若不倍频
		register UINT8 ECnew = (ECA << 1) | ECB;//更新记录状态
		if(EC2old == 0 && ECnew == 3)	   EC2val--;//顺时针(只针对开发使用的编码器)
		else if(EC2old == 1 && ECnew == 2) EC2val++;//逆时针(只针对开发使用的编码器)
		EC2old = ECnew;//更新记录状态
	}
}
