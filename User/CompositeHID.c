
#include "CompositeHID.H"

//USB端点缓存,必须是偶地址。EP4 的 OUT/IN 缓冲紧随 EP0，三者共用 UEP0_DMA。
static UINT8X Ep0Buffer[3*MAX_PACKET_SIZE] _at_ XBASE_EP0_BUF;
static UINT8X Ep1Buffer[MIN(64,ENDP1_OUT_SIZE+2)+MIN(64,ENDP1_IN_SIZE+2)] _at_ XBASE_EP1_BUF;	//端点1 OUT&IN
static UINT8X Ep2Buffer[64] _at_ XBASE_EP2_BUF;	//端点2 仅IN
static UINT8X Ep3Buffer[64] _at_ XBASE_EP3_BUF;	//端点3 仅IN

#define Ep4OutBuffer	(Ep0Buffer + MAX_PACKET_SIZE)
#define Ep4InBuffer	(Ep0Buffer + MAX_PACKET_SIZE * 2)


UINT8X WakeUpEnFlag = 0;	//远程唤醒使能标志
static UINT8X SetupReq;
static UINT8X UsbConfig;	//USB配置标志
static UINT16X SetupLen;
static PUINT8 pDescr;		//描述符指针 必须是通用指针
static PXUSB_SETUP_REQ  SetupReqBuf;	//暂存Setup包

/* HID 控制传输状态。Custom HID 与主 HID 各保存一份 Idle 状态。 */
static UINT8X HidIdle[USBD_MAX_NUM_INTERFACES] = {0};
static UINT8X HidProtocol = 1;                 // 1: Report protocol, 0: Boot protocol
static UINT8X HidKeyboardLedState = 0;
static UINT8X HidSetReportInterface = 0xFF;
static UINT8X HidSetReportType = 0;

static bit Ready = 0;			//USB就绪标志
static bit Endp1Busy = 0;		//传输完成控制标志
static bit Endp2Busy = 0;		//传输完成控制标志
static bit Endp3Busy = 0;		//传输完成控制标志
static bit Endp4Busy = 0;		//传输完成控制标志


#pragma  NOAREGS


/*设备描述符*/
static UINT8C DevDesc[] = {//设备描述符
	0x12,//1. 第一个字节 0x12 表示该设备描述符的长度为 18 字节。
	0x01,//2. 第二个字节 0x01 表示该描述符的类型为设备描述符 (Device Descriptor)。
	0x00,//3. 第三个字节 0x00 表示USB规范的版本号 (USB Specification Release Number) 的低字节。
	0x02,//4. 第四个字节 0x02 表示USB规范的版本号的高字节。
	0x00,//5. 第五个字节 0x00 表示该设备的设备类代码 (Device Class Code)。
	0x00,//6. 第六个字节 0x00 表示该设备的设备子类代码 (Device Subclass Code)。
	0x00,//7. 第七个字节 0x00 表示该设备的设备协议代码 (Device Protocol Code)。
	THIS_ENDP0_SIZE,//8. 第八个字节表示该设备的最大包长度 (Max Packet Size)。
	Vendor_ID & 0xFF,//9. 第九个字节表示该设备的厂商ID (Vendor ID) 的低字节。
	Vendor_ID >> 8,//10. 第十个字节表示该设备的厂商ID的高字节。
	Product_ID & 0xFF,//11. 第十一个字节表示该设备的产品ID (Product ID) 的低字节。
	Product_ID >> 8,//12. 第十二个字节表示该设备的产品ID的高字节。
	0x00,//13. 第十三个字节 0x00 表示该设备的设备版本号 (Device Release Number) 的低字节。
	0x01,//14. 第十四个字节 0x01 表示该设备的设备版本号的高字节。
	0x01,//15. 第十五个字节 0x01 表示该设备的制造商字符串描述符索引 (Manufacturer String Index)。
	0x02,//16. 第十六个字节 0x02 表示该设备的产品字符串描述符索引 (Product String Index)。
	0x03,//17. 第十七个字节 0x00 表示该设备的序列号字符串描述符索引 (Serial Number String Index)。
	0x01//18. 第十八个字节 0x01 表示该设备支持的配置数目 (Number of Configurations)。
};

/*字符串描述符*/
static UINT8C MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };//语言描述符
static UINT8C MyProdInfo[] = {22,0x03,'E',0,'t',0,'h',0,'a',0,'n',0,'e',0,'-',0,'X',0,'V',0,'I',0};//产品名称
static UINT8C MyManuInfo[] = {36,0x03,
	'L',0,'i',0,'g',0,'h',0,'t',0,'&',0,'E',0,'l',0,'e',0,'c',0,'t',0,'r',0,'i',0,'c',0,'i',0,'t',0,'y',0
};//制造者名称
UINT8X MySrNumInfo[26];//序列号字符串 初始化时加载

/*HID类报文描述符*/
/* 主 HID（键盘、鼠标、媒体与 Dial）使用 EP2 IN。触摸单独使用 EP3 IN。 */
static UINT8C KeyRepDesc[/*285*/] = { // HID报文描述符
	//键盘
	0x05, 0x01,					//	USAGE_PAGE (Generic Desktop)
	0x09, 0x06,					//	USAGE (Keyboard)
	0xa1, 0x01,					//	COLLECTION (Application)
	0x85, 0x01,					//		REPORT_ID (1)
	0x05, 0x07,					//		USAGE_PAGE (Keyboard)
	0x19, 0xe0,					//		USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,					//		USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,					//		LOGICAL_MINIMUM (0)
	0x25, 0x01,					//		LOGICAL_MAXIMUM (1)
	0x75, 0x01,					//		REPORT_SIZE (1)
	0x95, 0x08,					//		REPORT_COUNT (8)
	0x81, 0x02,					//		INPUT (Data,Var,Abs)
	0x95, 0x01,					//		REPORT_COUNT (1)
	0x75, 0x08,					//		REPORT_SIZE (8)
	0x81, 0x03,					//		INPUT (Cnst,Var,Abs)
	
	0x95, 0x05,					//		REPORT_COUNT (5)
	0x75, 0x01,					//		REPORT_SIZE (1)
	0x05, 0x08,					//		USAGE_PAGE (LEDs)
	0x19, 0x01,					//		USAGE_MINIMUM (Num Lock)
	0x29, 0x05,					//		USAGE_MAXIMUM (Kana)
	0x91, 0x02,					//		OUTPUT (Data,Var,Abs)
	0x95, 0x01,					//		REPORT_COUNT (1)
	0x75, 0x03,					//		REPORT_SIZE (3)
	0x91, 0x03,					//		OUTPUT (Cnst,Var,Abs)

	/* 与 Methane 的描述符顺序一致：先 LED Output，再声明 19 键数组。 */
	0x95, 0x13,					//		REPORT_COUNT (19)按键数
	0x75, 0x08,					//		REPORT_SIZE (8)
	0x15, 0x00,					//		LOGICAL_MINIMUM (0)
	0x25, 0x65,					//		LOGICAL_MAXIMUM (101)
	0x05, 0x07,					//		USAGE_PAGE (Keyboard)
	0x19, 0x00,					//		USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,					//		USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,					//		INPUT (Data,Ary,Abs)
	0xc0,						//	END_COLLECTION
	
	//鼠标
	0x05, 0x01,					//	USAGE_PAGE (Generic Desktop)
	0x09, 0x02,					//	USAGE (Mouse)
	0xa1, 0x01,					//	COLLECTION (Application)
	0x85, 0x02,					//		REPORT_ID (2)
	0x09, 0x01,					//		USAGE (Pointer)
	0xa1, 0x00,					//		COLLECTION (Physical)
	0x05, 0x09,					//			USAGE_PAGE (Button)
	0x19, 0x01,					//			USAGE_MINIMUM (Button 1)
	0x29, 0x03,					//			USAGE_MAXIMUM (Button 3)
	0x15, 0x00,					//			LOGICAL_MINIMUM (0)
	0x25, 0x01,					//			LOGICAL_MAXIMUM (1)
	0x95, 0x03,					//			REPORT_COUNT (3)
	0x75, 0x01,					//			REPORT_SIZE (1)
	0x81, 0x02,					//			INPUT (Data,Var,Abs)
	0x95, 0x01,					//			REPORT_COUNT (1)
	0x75, 0x05,					//			REPORT_SIZE (5)
	0x81, 0x03,					//			INPUT (Cnst,Var,Abs)
	0x05, 0x01,					//			USAGE_PAGE (Generic Desktop)
	0x09, 0x30,					//			USAGE (X)
	0x09, 0x31,					//			USAGE (Y)
	0x09, 0x38,					//			USAGE (Wheel)	
	0x15, 0x81,					//			LOGICAL_MINIMUM (-127)
	0x25, 0x7f,					//			LOGICAL_MAXIMUM (127)
	0x75, 0x08,					//			REPORT_SIZE (8)
	0x95, 0x03,					//			REPORT_COUNT (3)
	0x81, 0x06,					//			INPUT (Data,Var,Rel)Rel相对值,Abs绝对值
	0xc0,						//		END_COLLECTION
	0xc0,						//	END_COLLECTION
	
	//媒体控制
	0x05,0x0C,					//	USAGE_PAGE (Consumer)
	0x09,0x01,					//	USAGE (Consumer Control)
	0xA1,0x01,					//	COLLECTION (Applicatior)
	0x85,0x04,					//		REPORT_ID (4)
	0xA1,0x00,					//		COLLECTION(Physical)
	0x09,0xE9,					//			USAGE (Volume Increment)
	0x09,0xEA,					//			USAGE (Volume Decrement)
	0x09,0xE2,					//			USAGE (Mute)
	0x09,0xCD,					//			USAGE (Play/Pause)
	0x09,0xB5,					//			USAGE (Scan Next Track)
	0x09,0xB6,					//			USAGE (Scan Previous Track)
	0x35,0x00,					//			PHYSICAL_MINIMUM (0)
	0x45,0x07,					//			PHYSICAL_MAXIMUM (7)
	0x15,0x00,					//			LOGICAL_MINIMUM (0)
	0x25,0x01,					//			LOCAL_MAXIMUM (1)
	0x75,0x01,					//			REPORT_SIZE (1)
	0x95,0x06,					//			REPORT_COUNT (6)
	0x81,0x02,					//			INPUT (Data,Var,Abs)
	0x75,0x01,					//			REPORT_SIZE (1)
	0x95,0x02,					//			REPORT_COUNT (2)
	0x81,0x01,					//			INPUT (Cnst,Ary,Abs)
	0xC0,						//		END_COLLECTION
	0xC0,						//	END_COLLECTION
	
	/* Dial 保留原始描述符写法，归入不含 Touch Screen 的主 HID 接口。 */
	// Dial
	0x05,0x01,          		//	USAGE_PAGE(Generic Desktop Controls)
	0x09,0x0E,          		//	LOCAL_USAGE(Reserved)
	0xA1,0x01,          		//	COLLECTION(Applicatior)
	0x85,0x05,          		//		REPORT_ID(5)
	0x05,0x0D,          		//		USAGE_PAGE(Digitizer)
	0x09,0x21,          		//		LOCAL_USAGE(Puck)
	0xA1,0x00,          		//		MAIN_COLLECTION(Physical)
	0x05,0x09,          		//		USAGE_PAGE(Button)
	0x09,0x01,          		//		LOCAL_USAGE(Button 1 (primary/trigger))
	0x95,0x01,          		//		REPORT_COUNT(1)
	0x75,0x01,          		//		REPORT_SIZE(1)
	0x15,0x00,          		//		LOGICAL_MINIMUM(0)
	0x25,0x01,          		//		LOCAL_MAXIMUM(1)
	0x81,0x02,          		//		INPUT(data var absolute NoWrap linear) Input 0.1
	0x05,0x01,          		//		USAGE_PAGE(Generic Desktop Controls)
	0x09,0x37,          		//		LOCAL_USAGE(Dial)
	0x95,0x01,          		//		REPORT_COUNT(1)
	0x75,0x0F,          		//		REPORT_SIZE(15)
	0x55,0x0F,          		//		UNIT_EXPONENT(15)
	0x65,0x14,          		//		REPORT_UNIT(20)
	0x36,0xF0,0xF1,     		//		PHYSICAL_MINIMUM(61936)
	0x46,0x10,0x0E,     		//		PHYSICAL_MAXIMUM(3600)
	0x16,0xF0,0xF1,     		//		LOGICAL_MINIMUM(-3600)
	0x26,0x10,0x0E,     		//		LOCAL_MAXIMUM(3600)
	0x81,0x06,          		//		INPUT(data var relative NoWrap linear) Input 2.0
	0xC0,               		//		END_COLLECTION
	0xC0,               		//	END_COLLECTION
};

/* 触摸独占 HID 接口/EP3，使 Android 仅对该接口绑定 hid-multitouch。 */
static UINT8C TouchRepDesc[] = {
	0x05, 0x0d,					// USAGE_PAGE (Digitizers)
	0x09, 0x04,					// USAGE (Touch Screen)
	0xa1, 0x01,					// COLLECTION (Application)
	0x85, 0x03,					//   REPORT_ID (3)
	0x09, 0x22,					//   USAGE (Finger)
	0xa1, 0x00,					//   COLLECTION (Physical)
	0x09, 0x42,					//     USAGE (Tip Switch)
	0x09, 0x44,					//     USAGE (Barrel Switch)
	0x09, 0x3c,					//     USAGE (Invert)
	0x09, 0x45,					//     USAGE (Eraser Switch)
	0x09, 0x32,					//     USAGE (In Range)
	0x15, 0x00,
	0x25, 0x01,
	0x75, 0x01,
	0x95, 0x05,
	0x81, 0x02,
	0x95, 0x01,
	0x75, 0x03,
	0x81, 0x03,
	0x75, 0x08,
	0x09, 0x51,					//     USAGE (Contact Identifier)
	0x95, 0x01,
	0x81, 0x02,
	0x05, 0x01,					//     USAGE_PAGE (Generic Desktop)
	0x75, 0x10,
	0x26, 0xFF, 0x7F,
	0x46, 0xFF, 0x7F,
	0x09, 0x30,					//     USAGE (X)
	0x09, 0x31,					//     USAGE (Y)
	0x95, 0x02,
	0x81, 0x02,
	0xc0,
	0xc0
};

/* 两个自定义 HID 报告描述符。 */
static UINT8C ComRepDesc[/*34*/] = { // 自定义HID报文描述符
	0x06, 0x00, 0xff, 	// Usage page Vendor defined ( FF00H )
	0x09, 0x01, 		// Local usage 1
	0xa1, 0x01, 		// Collation Application
	0x09, 0x02, 		// Local usage 2
	0x15, 0x00, 		// Logical min ( 0H )
	0x26, 0xff, 0x00,	// Logical max ( FFH )
	0x75, 0x08,  		// Report size ( 08H )
	0x95, 0x40, 		// Report count ( 40H )
	0x81, 0x06,  		// Input ( Data, Relative, Wrap )
	0x09, 0x02, 		// Local usage 2
	0x15, 0x00,  		// Logical min ( 0H )
	0x26, 0xff, 0x00,	// Logical max ( FFH )
	0x75, 0x08, 		// Report size ( 08H )
	0x95, 0x40, 		// Report count ( 40H )
	0x91, 0x06, 		// Output ( Data, Relative, Wrap )
	0xc0,				// END_COLLECTION
};
static UINT8C RgbRepDesc[/*34*/] = { // RGB自定义HID报文描述符
	0x06, 0x01, 0xff, 	// Usage page Vendor defined ( FF01H )
	0x09, 0x03, 		// Local usage 3
	0xa1, 0x01, 		// Collation Application
	0x09, 0x04, 		// Local usage 4
	0x15, 0x00, 		// Logical min ( 0H )
	0x26, 0xff, 0x00,	// Logical max ( FFH )
	0x75, 0x08,  		// Report size ( 08H )
	0x95, 0x40, 		// Report count ( 40H )
	0x81, 0x06,  		// Input ( Data, Relative, Wrap )
	0x09, 0x04, 		// Local usage 4
	0x15, 0x00,  		// Logical min ( 0H )
	0x26, 0xff, 0x00,	// Logical max ( FFH )
	0x75, 0x08, 		// Report size ( 08H )
	0x95, 0x40, 		// Report count ( 40H )
	0x91, 0x06, 		// Output ( Data, Relative, Wrap )
	0xc0,				// END_COLLECTION
};

/* 四个 HID class descriptor，亦用于主机单独请求 0x21 描述符。 */
static UINT8C CustomHidDesc[] = { 0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(ComRepDesc)&0xFF,sizeof(ComRepDesc)>>8 };
static UINT8C RgbHidDesc[]    = { 0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(RgbRepDesc)&0xFF,sizeof(RgbRepDesc)>>8 };
static UINT8C MainHidDesc[]   = { 0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(KeyRepDesc)&0xFF,sizeof(KeyRepDesc)>>8 };
static UINT8C TouchHidDesc[]  = { 0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(TouchRepDesc)&0xFF,sizeof(TouchRepDesc)>>8 };

/*
 * 接口0: 键盘、鼠标、媒体、Dial/EP2；接口1: 触摸/EP3；
 * 接口2: RGB HID/EP4（SignalRGB 以此接口号匹配）；接口3: Custom HID/EP1。
 * 触摸必须是独立接口，使 Android 只对接口1 绑定 hid-multitouch。
 */
static UINT8C CfgDesc[] = {//配置描述符
	0x09,	// 1. 第一个字节 0x09 表示该配置描述符的长度为 9 字节
	0x02,	// 2. 第二个字节 0x02 表示该描述符的类型为配置描述符 (Configuration Descriptor)
	9+25+25+32+32,	// 配置 + 主 HID + 触摸 HID + RGB HID + Custom HID = 123字节
	0x00,	// 4. 第四个字节 0x00 表示配置描述符的总长度的高字节
	USBD_MAX_NUM_INTERFACES,	// 5. 四个接口
	0x01,	// 6. 第六个字节 0x01 表示配置描述符的标识符 (Configuration Value)
	0x00,	// 7. 第七个字节 0x00 表示该配置的描述字符串索引 (Configuration String Index)
	0xA0,	// 8. 第八个字节 0xA0 表示该配置的特性标志 (Attributes)，A0=0b10100000，bit7必须1，bit6为是否自供电，bit5为是否远程唤醒
	250,	// 9. 第九个字节 0x32 表示该配置的最大功率 (Max Power)，这里的值 0x32 表示设备使用的最大电流为 (250 * 2 mA) = 500 mA

	0x09,0x04,USBD_HID_INTERFACE,0x00,1,0x03,0x00,0x00,0x00,							// 主 HID 接口0
	0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(KeyRepDesc)&0xFF,sizeof(KeyRepDesc)>>8,	// HID类描述符
	0x07,0x05,HID_EPIN_ADDR,0x03,ENDP2_IN_SIZE,0x00,HID_FS_BINTERVAL,					// 端点2 IN

	0x09,0x04,USBD_TOUCH_HID_INTERFACE,0x00,1,0x03,0x00,0x00,0x00,					// 触摸 HID 接口1
	0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(TouchRepDesc)&0xFF,sizeof(TouchRepDesc)>>8,	// HID类描述符
	0x07,0x05,TOUCH_HID_EPIN_ADDR,0x03,ENDP3_IN_SIZE,0x00,HID_FS_BINTERVAL,				// 端点3 IN

	0x09,0x04,USBD_RGB_HID_INTERFACE,0x00,2,0x03,0x00,0x00,0x00,						// RGB Custom HID 接口2
	0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(RgbRepDesc)&0xFF,sizeof(RgbRepDesc)>>8,	// RGB HID类描述符
	0x07,0x05,RGB_HID_EPIN_ADDR,0x03,ENDP4_IN_SIZE,0x00,RGB_HID_FS_BINTERVAL,				// 端点4 IN
	0x07,0x05,RGB_HID_EPOUT_ADDR,0x03,ENDP4_OUT_SIZE,0x00,RGB_HID_FS_BINTERVAL,			// 端点4 OUT

	0x09,0x04,USBD_CUSTOM_HID_INTERFACE,0x00,2,0x03,0x00,0x00,0x00,						// Custom HID 接口3
	0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(ComRepDesc)&0xFF,sizeof(ComRepDesc)>>8,	// Custom HID类描述符
	0x07,0x05,CUSTOM_HID_EPIN_ADDR,0x03,ENDP1_IN_SIZE,0x00,CUSTOM_HID_FS_BINTERVAL,	// 端点1 IN
	0x07,0x05,CUSTOM_HID_EPOUT_ADDR,0x03,ENDP1_OUT_SIZE,0x00,CUSTOM_HID_FS_BINTERVAL,	// 端点1 OUT
};


/*******************************************************************************
* Function Name  : CH554USBDevWakeup()
* Description    : USB设备模式唤醒主机，发送K信号
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH554USBDevWakeup( ){
	UDEV_CTRL |= bUD_LOW_SPEED;//先变为低速
	mDelaymS(2);
	UDEV_CTRL &= ~bUD_LOW_SPEED;//再变回全速
}

/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : USB设备模式配置，设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceInit(){
	IE_USB = 0;
	USB_CTRL = 0x00;				//先设定USB设备模式
	UDEV_CTRL = bUD_PD_DIS;			//禁止DP/DM下拉电阻
	
	UDEV_CTRL &= ~bUD_LOW_SPEED;	//选择全速12M模式，默认方式
	USB_CTRL &= ~bUC_LOW_SPEED;
	
	UEP1_T_LEN = 0;
	UEP2_T_LEN = 0;
	UEP3_T_LEN = 0;
	UEP4_T_LEN = 0;

	/* EP4 与 EP0 共享 UEP0_DMA 的三个连续 64B 缓冲区，见 BasicIO.h。 */
	UEP0_DMA = Ep0Buffer;							//端点0、端点4数据传输地址
	UEP1_DMA = Ep1Buffer;							//端点1数据传输地址
	UEP2_DMA = Ep2Buffer;							//端点2数据传输地址
	UEP3_DMA = Ep3Buffer;							//端点3数据传输地址

	/* EP1/EP4 为双向 Custom HID；EP2 为主 HID IN；EP3 为触摸 HID IN。 */
	UEP4_1_MOD &= ~(bUEP1_BUF_MOD | bUEP4_RX_EN | bUEP4_TX_EN);
	UEP4_1_MOD |= bUEP1_TX_EN | bUEP1_RX_EN | bUEP4_TX_EN | bUEP4_RX_EN;
	UEP2_3_MOD &= ~(bUEP3_RX_EN | bUEP3_BUF_MOD | bUEP2_RX_EN | bUEP2_BUF_MOD);
	UEP2_3_MOD |= bUEP3_TX_EN | bUEP2_TX_EN;
	/* 即使主机尚未来得及发总线复位，各端点也从确定的 DATA0/NAK 状态开始。 */
	UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
	UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
	UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
	UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
	UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
	
	USB_DEV_AD = 0x00;
	USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;			//启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
	UDEV_CTRL |= bUD_PORT_EN;										//允许USB端口
	USB_INT_FG = 0xFF;												//清中断标志
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
	IE_USB = 1;
}

/*******************************************************************************
* Function Name  : Enp1IntIn
* Description    : USB设备模式端点1的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp1IntIn(UINT8 *buf, UINT8 len){
	memcpy(Ep1Buffer+MAX_PACKET_SIZE, buf, len);	//加载上传数据
	if(Ready && !Endp1Busy){						//USB就绪且端点1空闲
		UEP1_T_LEN = len;												//设置发送长度
		UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;		//有数据时上传数据并应答ACK
		Endp1Busy = 1;
	}
}

/*******************************************************************************
* Function Name  : Enp2IntIn()
* Description    : USB设备模式端点2的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp2IntIn(UINT8 *buf, UINT8 len){
	memcpy(Ep2Buffer, buf, len);	//端点2仅 TX，DMA 从基地址读取
	if(Ready && !Endp2Busy){						//USB就绪且端点2空闲
		UEP2_T_LEN = len;												//设置发送长度
		UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;		//有数据时上传数据并应答ACK
		Endp2Busy = 1;
	}
}

/*******************************************************************************
* Function Name  : Enp3IntIn()
* Description    : USB设备模式端点3的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp3IntIn(UINT8 *buf, UINT8 len){
	memcpy(Ep3Buffer, buf, len);	//端点3仅 IN，DMA 从基地址读取
	if(Ready && !Endp3Busy){						//USB就绪且端点3空闲
		UEP3_T_LEN = len;												//设置实际触摸报告长度
		UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;		//有数据时上传数据并应答ACK
		Endp3Busy = 1;
	}
}

/*******************************************************************************
* Function Name  : Enp4IntIn()
* Description    : USB设备模式端点4（RGB Custom HID）的中断上传
*******************************************************************************/
void Enp4IntIn(UINT8 *buf, UINT8 len){
	memcpy(Ep4InBuffer, buf, len);
	if(Ready && !Endp4Busy){
		UEP4_T_LEN = len;
		UEP4_CTRL = UEP4_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
		Endp4Busy = 1;
	}
}

/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH549 USB中断处理函数
*******************************************************************************/
void DeviceInterrupt( void ) interrupt INT_NO_USB using 1				//USB中断服务程序,使用寄存器组1
{
	UINT8X errflag;//错误标志
	UINT16X len;
	
	/* 总线复位必须抢占旧传输完成标志，避免快速重连时 EP0 沿用旧事务状态。 */
	if(UIF_TRANSFER && !UIF_BUS_RST){			//USB传输完成标志
		switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
		{
		case UIS_TOKEN_IN | 3:							//端点3上传
			UEP3_T_LEN = 0;                                                     //预使用发送长度一定要清空
			UEP3_CTRL ^= bUEP_T_TOG;                                            //手动翻转同步标志位
			Endp3Busy = 0;
			UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
			break;
		case UIS_TOKEN_IN | 4:							//端点4（RGB Custom HID）上传
			UEP4_T_LEN = 0;
			UEP4_CTRL ^= bUEP_T_TOG;
			Endp4Busy = 0;
			UEP4_CTRL = UEP4_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;
			break;
		case UIS_TOKEN_OUT | 4:							//端点4（RGB Custom HID）下传
			if(U_TOG_OK){
				len = USB_RX_LEN;
				UEP4_CTRL ^= bUEP_R_TOG;
				/* EP4 的 OUT 为 UEP0_DMA+64，IN 为 UEP0_DMA+128。 */
				memcpy(Ep4InBuffer, Ep4OutBuffer, len);
				UEP4_T_LEN = len;
				Endp4Busy = 1;
				UEP4_CTRL = UEP4_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
				if(Ep4OutBuffer[0] == 0xA5 && Ep4OutBuffer[1] == 0x5A
					&& Ep4OutBuffer[2] == 3){ // Sender: SignalRGB
					if(Ep4OutBuffer[3] == 0){
						rgbHidFlag = (rgbHidFlag & ~0xF0) | 0x80 | 0x20;
						memcpy(FrameRaw, Ep4OutBuffer + 4, 16*3);
					}
					else if(Ep4OutBuffer[3] == 2){
						rgbHidFlag &= ~0xF0;
					}
				}
			}
			break;
		case UIS_TOKEN_IN | 1:							//端点1（Custom HID）上传
			UEP1_T_LEN = 0;                                                     //预使用发送长度一定要清空
			UEP1_CTRL ^= bUEP_T_TOG;                                            //手动翻转同步标志位
			Endp1Busy = 0;
			UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
			break;
		case UIS_TOKEN_OUT | 1:							//端点1（Custom HID）下传
			/* 以下原有 Custom HID 处理逻辑整体迁移到端点1。 */
#define UEP2_CTRL UEP1_CTRL
#define UEP2_T_LEN UEP1_T_LEN
#define Ep2Buffer Ep1Buffer
			if(U_TOG_OK){														//不同步的数据包将丢弃
				UEP2_CTRL ^= bUEP_R_TOG;									    //手动翻转同步标志位
				len = USB_RX_LEN;                                               //接收数据长度，数据从Ep2Buffer首地址开始存放
				UEP2_T_LEN = len;												//设置发送长度
/**************************************************以下CustomHID通信部分独立缩进**************************************************/
#define Buf		Ep2Buffer
#define Offset	MAX_PACKET_SIZE
#define count	Buf[Offset+63]//此字节在通信中不会修改,故借用
#define packs	Buf[Offset+62]//此字节在通信中不会修改,故借用
#define index	Buf[Offset+61]//此字节在通信中不会修改,故借用
if(asyncFlag & 0x80){//若已经在接收状态 则接收数据包
	memcpy(FlashBuf + ((UINT16X)count << 6), Buf, 64);//数据包拷贝
	Buf[Offset+0] = 'R'; Buf[Offset+1] = 'D';//填入响应字节
	Buf[Offset+2] = count++;//填入序号
	if(count >= packs){//全部数据包接收完毕
		count = 0;//防止越界
		asyncFlag &= ~0x80;//正在接收标志位清除
	}
	UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;//启动上传响应主机
}
else{//若未在接收状态 则监听各种命令
	Buf[Offset+0] = 'R'; Buf[Offset+1] = Buf[1]; Buf[Offset+2] = Buf[2]; Buf[Offset+3] = Buf[3];//预填入响应字节
	if((Buf[0] == 'D' && Buf[1] == 'K' && Buf[2] == 'B' && Buf[3] == 'C' 
		|| Buf[0] == 'D' && Buf[1] == 'L' && Buf[2] == 'T' && Buf[3] == 'C') 
		&& Buf[4] >= '1' && Buf[4] <= '0' + ALK_CFG_NUM){//连接命令
		if(Buf[1] == 'K'){	//键盘配置
			packs = 8;
			asyncFlag = Buf[4] - '1' + ASYNC_FLAG_CFG;//确定键盘存储位置
		}else{				//灯效配置
			packs = 4;
			asyncFlag = Buf[4] - '1' + ASYNC_FLAG_LIGHT;//确定灯效存储位置
		}
		count = 0;//计数置零
		asyncFlag |= 0x80;//正在接收标志位置位
	}
	else if(Buf[0] == 'C' && Buf[1] == 'K' && Buf[2] == 'Y' && Buf[3] == 'T'){//修改按键消抖参数命令
		Buf[Offset+4] = keyFltNum;//把旧参数上报
		keyFltNum = Buf[4];//修改参数
		Buf[Offset+5] = keyFltNum;//把新参数环回
		asyncFlag = ASYNC_FLAG_GLOB;//异步标志置位
	}
	else if(Buf[0] == 'C' && Buf[1] == 'R' && Buf[2] == 'K' && Buf[3] == 'M'){//摇杆中位校正命令
		*(PUINT16X)&Buf[Offset+4] = ANA_MID(0);//填入旧中位值
		*(PUINT16X)&Buf[Offset+6] = ANA_MID(1);
		ANA_MID(0) = LIMIT(adcValue[0], ANA_MIN(0) + 1, ANA_MAX(0) - 1);//将当前摇杆采样值钳位后作为摇杆中位值
		ANA_MID(1) = LIMIT(adcValue[1], ANA_MIN(1) + 1, ANA_MAX(1) - 1);
		*(PUINT16X)&Buf[Offset+8] = ANA_MID(0);//填入新中位值
		*(PUINT16X)&Buf[Offset+10] = ANA_MID(1);
		asyncFlag = ASYNC_FLAG_GLOB;//异步标志置位
	}
	else if(Buf[0] == 'C' && Buf[1] == 'R' && Buf[2] == 'K' && Buf[3] == 'Z'){//摇杆范围设置命令
		*(PUINT16X)&Buf[Offset+4] = ANA_MIN(0);//填入旧范围值
		*(PUINT16X)&Buf[Offset+6] = ANA_MAX(0);
		*(PUINT16X)&Buf[Offset+8] = ANA_MIN(1);
		*(PUINT16X)&Buf[Offset+10] = ANA_MAX(1);
		if(*(PUINT16X)&Buf[4] < ANA_MID(0) && *(PUINT16X)&Buf[6] > ANA_MID(0) 
		&& *(PUINT16X)&Buf[8] < ANA_MID(1) && *(PUINT16X)&Buf[10] > ANA_MID(1)){//接受
			ANA_MIN(0) = *(PUINT16X)&Buf[4];//接收新范围值
			ANA_MAX(0) = *(PUINT16X)&Buf[6];
			ANA_MIN(1) = *(PUINT16X)&Buf[8];
			ANA_MAX(1) = *(PUINT16X)&Buf[10];
			*(PUINT16X)&Buf[Offset+12] = ANA_MIN(0);//填入新范围值
			*(PUINT16X)&Buf[Offset+14] = ANA_MAX(0);
			*(PUINT16X)&Buf[Offset+16] = ANA_MIN(1);
			*(PUINT16X)&Buf[Offset+18] = ANA_MAX(1);
			asyncFlag = ASYNC_FLAG_GLOB;//异步标志置位
		}
		else{//不接受
			*(PUINT16X)&Buf[Offset+12] = 0xFFFF;//填入拒绝标志值
		}
	}
	else if(Buf[0] == 'C' && Buf[1] == 'E' && Buf[2] == 'C' && Buf[3] == 'D'){//修改旋钮倍频命令
		Buf[Offset+4] = EC1freq;//填入旧倍频参数
		Buf[Offset+5] = EC2freq;
		EC1freq = Buf[4];//更新旋钮倍频参数
		EC2freq = Buf[5];
		Buf[Offset+6] = EC1freq;//把新参数的采纳值环回
		Buf[Offset+7] = EC2freq;
		asyncFlag = ASYNC_FLAG_GLOB;//异步标志置位
	}
	else if(Buf[0] == 'C' && Buf[1] == 'E' && Buf[2] == 'C' && Buf[3] == 'F'){//修改旋钮滤波参数命令(暂未实现)
		//TimFilterValue = Buf[4];//更新旋钮滤波参数
	}
	else if(Buf[0] == 'B' && Buf[1] == 'F' && Buf[2] == 'W' && Buf[3] == 'V'){//固件版本读取命令
		memcpy(&Buf[Offset+4], FIRMWARE_VERSION, 4);//填入固件版本
	}
	else if(Buf[0] == 'B' && Buf[1] == 'U' && Buf[2] == 'I' && Buf[3] == 'D'){//序列号读取命令
		for(index = 0; index < 12; index++){//读取12个字符并转为16进制的一位数
			Buf[Offset+4+index] = MySrNumInfo[2+index*2];//读取字符
			if(Buf[Offset+4+index] >= '0' && Buf[Offset+4+index] <= '9') Buf[Offset+4+index] -= '0';
			else if(Buf[Offset+4+index] >= 'a' && Buf[Offset+4+index] <= 'f') Buf[Offset+4+index] -= ('a' - 10);
			else if(Buf[Offset+4+index] >= 'A' && Buf[Offset+4+index] <= 'F') Buf[Offset+4+index] -= ('A' - 10);
			else Buf[Offset+4+index] = 0;
		}
		for(index = 0; index < 6; index++){//12个16进制的一位数转为6个字节
			Buf[Offset+4+index] = (Buf[Offset+4+index*2] << 4) | Buf[Offset+5+index*2];
		}
	}
	else if(Buf[0] == 'B' && Buf[1] == 'I' && Buf[2] == 'P' && Buf[3] == 'T'){ // 输入读取命令
		*(PUINT16X)&Buf[Offset+4] = adcValue[0]; // 摇杆值
		*(PUINT16X)&Buf[Offset+6] = adcValue[1];
		*(PUINT16X)&Buf[Offset+8] = 0; // 保留
		Buf[Offset+10] = ecValue[0]; // 旋钮值
		Buf[Offset+11] = ecValue[1];
		*(PUINT32X)&Buf[Offset+12] = 0; // 按键值
		for(index = 0; index < ALK_BUTTON_NUM; index++){ // 按键值
			*(PUINT32X)&Buf[Offset+12] |= (uint32_t)keyNow[index] << index;
		}
	}
	else if(Buf[0] == 'B' && Buf[1] == 'G' && Buf[2] == 'P' && Buf[3] == 'M'){//参数读取命令
		memcpy(&Buf[Offset+4], (PUINT8C)DATA_GLOB_BASE, 56);//全局参数读取
	}
	else if(Buf[0] == 'B' && Buf[1] == 'D' && Buf[2] == 'G' && Buf[3] == 'C'){//诊断数据读取命令
		DiagGet(&Buf[Offset+4], 56);//诊断数据获取
	}
	else if(Buf[0] == 'B' && Buf[1] == 'R' && Buf[2] == 'S' && Buf[3] == 'T'){//软复位命令
		memset(&Buf[Offset+4], ' ', 64 - 4);//后面全置为空格
		asyncFlag = ASYNC_FLAG_SRST;//异步标志置位
	}
	else if(Buf[0] == 'B' && Buf[1] == 'B' && Buf[2] == 'O' && Buf[3] == 'T'){//Boot预跳转命令
		memset(&Buf[Offset+4], ' ', 64 - 4);//后面全置为空格
		asyncFlag = ASYNC_FLAG_BOOT;//异步标志置位
	}
	else if(Buf[0] == 'B' && Buf[1] == 'C' && Buf[2] == 'S' && Buf[3] == 'C'){//配置切换命令
		memset(&Buf[Offset+4], ' ', 64 - 4);//后面全置为空格
		Buf[Offset+4] = sysCs + '1';//把旧选择上报
		if(Buf[4] >= '1' && Buf[4] <= '0' + ALK_CFG_NUM){//新选择合法
			Buf[Offset+5] = Buf[4];//把新选择环回
			FlashBuf[0] = Buf[4] - '1';//用FlashBuf暂存新选择
			asyncFlag = ASYNC_FLAG_CSC;//异步标志置位
		}
		else Buf[Offset+5] = 0xFF;//填入拒绝标志
	}
	else if(Buf[0] == 'B' && Buf[1] == 'B' && Buf[2] == 'U' && Buf[3] == 'Z'){//蜂鸣器模式命令
		asyncFlag = ASYNC_FLAG_BUZZ;//异步标志置位
	}
	else if(Buf[0] == 'B' && Buf[1] == 'B' && Buf[2] == 'T' && Buf[3] == 'V'){ // 蜂鸣器音符命令
		if(Buf[4] <= 48 && Buf[5] && Buf[5] <= 10){ // 音符音量合法
			cmdBuzzTone = Buf[4];	// 接受音符
			cmdBuzzVol = Buf[5];	// 接受音量
		}
		else cmdBuzzTone = 0xFF;    // 清除音符
	}
	else{//非法命令
		Buf[Offset+0] = 'R'; Buf[Offset+1] = 'I'; Buf[Offset+2] = 'N'; Buf[Offset+3] = 'V';//非法命令响应字节
	}
	UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;//启动上传响应主机
}
/**************************************************以上CustomHID通信部分独立缩进**************************************************/
#undef index
#undef packs
#undef count
#undef Offset
#undef Buf
#undef Ep2Buffer
#undef UEP2_T_LEN
#undef UEP2_CTRL
			}
			break;
		case UIS_TOKEN_IN | 2:							//端点2（主HID）上传
#define UEP1_CTRL UEP2_CTRL
#define UEP1_T_LEN UEP2_T_LEN
#define Endp1Busy Endp2Busy
			UEP1_T_LEN = 0;														//预使用发送长度一定要清空
			UEP1_CTRL ^= bUEP_T_TOG;											//手动翻转
			Endp1Busy = 0;
			UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			//默认应答NAK
			break;
#undef Endp1Busy
#undef UEP1_T_LEN
#undef UEP1_CTRL
		/* 主 HID 没有 OUT endpoint；仅在异常事务时消费切换位。 */
		case UIS_TOKEN_OUT | 2:
			if(U_TOG_OK) UEP2_CTRL ^= bUEP_R_TOG;
			break;
		case UIS_TOKEN_SETUP | 0:						//SETUP事务
			UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;	//预置NAK,防止stall之后不及时清除响应方式
			len = USB_RX_LEN;
			if(len == (sizeof(USB_SETUP_REQ))){
				SetupLen = ((UINT16)UsbSetupBuf->wLengthH<<8) + UsbSetupBuf->wLengthL;
				errflag = len = 0;													//默认为成功并且上传0长度
				SetupReq = UsbSetupBuf->bRequest;
				HidSetReportInterface = 0xFF;
				HidSetReportType = 0;
				if((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS){//HID类命令
					if((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) != USB_REQ_RECIP_INTERF
						|| UsbSetupBuf->wIndexH != 0
						|| UsbSetupBuf->wIndexL >= USBD_MAX_NUM_INTERFACES){
						errflag = 0xFF;
						break;
					}
					switch( SetupReq )
					{
					case HID_GET_REPORT:
						if((UsbSetupBuf->bRequestType & USB_REQ_TYP_IN) == 0){
							errflag = 0xFF;
							break;
						}
						if(UsbSetupBuf->wIndexL == USBD_HID_INTERFACE && UsbSetupBuf->wValueH == 1){
							switch(UsbSetupBuf->wValueL){
							case 1: pDescr = KeyBrd_data; len = ALK_RPT_L_KEYBRD; break;
							case 2: pDescr = Mouse_data;  len = ALK_RPT_L_MOUSE;  break;
							case 4: pDescr = Vol_data;    len = ALK_RPT_L_VOL;    break;
							case 5: pDescr = Dial_data;   len = ALK_RPT_L_DIAL;   break;
							default: errflag = 0xFF; break;
							}
							if(errflag == 0 && SetupLen < len) len = SetupLen;
							if(errflag == 0) memcpy(Ep0Buffer, pDescr, len);
						}
						else if(UsbSetupBuf->wIndexL == USBD_TOUCH_HID_INTERFACE
							&& UsbSetupBuf->wValueH == 1 && UsbSetupBuf->wValueL == 3){
							pDescr = Point_data;
							len = ALK_RPT_L_POINT;
							if(SetupLen < len) len = SetupLen;
							memcpy(Ep0Buffer, pDescr, len);
						}
						else if((UsbSetupBuf->wIndexL == USBD_CUSTOM_HID_INTERFACE
							|| UsbSetupBuf->wIndexL == USBD_RGB_HID_INTERFACE)
							&& (UsbSetupBuf->wValueH == 1 || UsbSetupBuf->wValueH == 2)
							&& UsbSetupBuf->wValueL == 0){
							len = SetupLen > THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;
							memset(Ep0Buffer, 0, len);
						}
						else errflag = 0xFF;
						break;
					case HID_GET_IDLE:
						if((UsbSetupBuf->bRequestType & USB_REQ_TYP_IN) == 0 || SetupLen != 1){
							errflag = 0xFF;
							break;
						}
						Ep0Buffer[0] = HidIdle[UsbSetupBuf->wIndexL];
						len = 1;
						break;
					case HID_GET_PROTOCOL:
						if(UsbSetupBuf->wIndexL != USBD_HID_INTERFACE
							|| (UsbSetupBuf->bRequestType & USB_REQ_TYP_IN) == 0 || SetupLen != 1){
							errflag = 0xFF;
							break;
						}
						Ep0Buffer[0] = HidProtocol;
						len = 1;
						break;
					case HID_SET_REPORT:
						if((UsbSetupBuf->bRequestType & USB_REQ_TYP_IN) != 0 || SetupLen > THIS_ENDP0_SIZE){
							errflag = 0xFF;
							break;
						}
						HidSetReportInterface = UsbSetupBuf->wIndexL;
						HidSetReportType = UsbSetupBuf->wValueH;
						break;
					case HID_SET_IDLE:
						if((UsbSetupBuf->bRequestType & USB_REQ_TYP_IN) != 0 || SetupLen != 0){
							errflag = 0xFF;
							break;
						}
						HidIdle[UsbSetupBuf->wIndexL] = UsbSetupBuf->wValueH;
						break;
					case HID_SET_PROTOCOL:
						if(UsbSetupBuf->wIndexL != USBD_HID_INTERFACE
							|| (UsbSetupBuf->bRequestType & USB_REQ_TYP_IN) != 0
							|| SetupLen != 0 || UsbSetupBuf->wValueH != 0 || UsbSetupBuf->wValueL > 1){
							errflag = 0xFF;
							break;
						}
						HidProtocol = UsbSetupBuf->wValueL;
						break;
					default:							//不支持的命令或者出错
						errflag = 0xFF;
						break;
					}
				}
				else if((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD){
					errflag = 0xFF;
				}
				else{	//标准请求
					switch(SetupReq)		//请求码
					{
					case USB_GET_DESCRIPTOR:		//请求获取描述符
						switch(UsbSetupBuf->wValueH)
						{
						case 1:								//设备描述符
							pDescr = DevDesc;						//给出设备描述符指针
							len = sizeof(DevDesc);					//给出设备描述符长度
							break;
						case 2:								//配置描述符
							pDescr = CfgDesc;						//给出配置描述符指针
							len = sizeof(CfgDesc);					//给出配置描述符长度
							break;
						case 3:								//字符串描述符
							switch( UsbSetupBuf->wValueL )
							{
							case 0:								//语言信息
								pDescr = (PUINT8)( &MyLangDescr[0] );
								len = sizeof( MyLangDescr );
								break;
							case 1:								//厂商信息
								pDescr = (PUINT8)( &MyManuInfo[0] );
								len = sizeof( MyManuInfo );
								break;
							case 2:								//产品信息
								pDescr = (PUINT8)( &MyProdInfo[0] );
								len = sizeof( MyProdInfo );
								break;
							case 3:								//序列号信息
								pDescr = (PUINT8)( &MySrNumInfo[0] );
								len = sizeof( MySrNumInfo );
								break;
							default:							//不支持的字符串描述符
								errflag = 0xFF;
								break;
							}
							break;
						case 0x21:							// HID class descriptor
							if(UsbSetupBuf->wIndexL == USBD_CUSTOM_HID_INTERFACE){
								pDescr = CustomHidDesc;
								len = sizeof(CustomHidDesc);
							}
							else if(UsbSetupBuf->wIndexL == USBD_RGB_HID_INTERFACE){
								pDescr = RgbHidDesc;
								len = sizeof(RgbHidDesc);
							}
							else if(UsbSetupBuf->wIndexL == USBD_HID_INTERFACE){
								pDescr = MainHidDesc;
								len = sizeof(MainHidDesc);
							}
							else if(UsbSetupBuf->wIndexL == USBD_TOUCH_HID_INTERFACE){
								pDescr = TouchHidDesc;
								len = sizeof(TouchHidDesc);
							}
							else errflag = 0xFF;
							break;
						case 0x22:							// report descriptor
							if(UsbSetupBuf->wIndexL == USBD_CUSTOM_HID_INTERFACE){
								pDescr = ComRepDesc;
								len = sizeof(ComRepDesc);
							}
							else if(UsbSetupBuf->wIndexL == USBD_RGB_HID_INTERFACE){
								pDescr = RgbRepDesc;
								len = sizeof(RgbRepDesc);
							}
							else if(UsbSetupBuf->wIndexL == USBD_HID_INTERFACE){
								pDescr = KeyRepDesc;
								len = sizeof(KeyRepDesc);
							}
							else if(UsbSetupBuf->wIndexL == USBD_TOUCH_HID_INTERFACE){
								pDescr = TouchRepDesc;
								len = sizeof(TouchRepDesc);
							}
							else errflag = 0xFF;
							break;
						default:							//不支持的命令或者出错
							errflag = 0xFF;
							break;
						}
						if(SetupLen > len){
							SetupLen = len;	//限制总长度
						}
						len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; //本次传输长度
						memcpy(Ep0Buffer,pDescr,len);                        //加载上传数据
						SetupLen -= len;
						pDescr += len;
						break;
					case USB_SET_ADDRESS:			//请求设置USB设备地址
						SetupLen = UsbSetupBuf->wValueL;	//暂存USB设备地址
						break;
					case USB_GET_CONFIGURATION:
						Ep0Buffer[0] = UsbConfig;
						if(SetupLen >= 1) len = 1;
						break;
					case USB_SET_CONFIGURATION:
						UsbConfig = UsbSetupBuf->wValueL;
						if(UsbConfig){
							Ready = 1;	//SetConfig命令一般代表USB枚举完成的标志
						}
						break;
					case USB_GET_INTERFACE:
						if(UsbConfig == 0 || UsbSetupBuf->wIndexH != 0
							|| UsbSetupBuf->wIndexL >= USBD_MAX_NUM_INTERFACES || SetupLen != 1){
							errflag = 0xFF;
							break;
						}
						Ep0Buffer[0] = 0;	// 当前全部接口只支持 alternate setting 0
						len = 1;
						break;
					case USB_SET_INTERFACE:
						if(UsbConfig == 0 || UsbSetupBuf->wIndexH != 0
							|| UsbSetupBuf->wIndexL >= USBD_MAX_NUM_INTERFACES
							|| UsbSetupBuf->wValueH != 0 || UsbSetupBuf->wValueL != 0 || SetupLen != 0){
							errflag = 0xFF;
						}
						break;
					case USB_CLEAR_FEATURE:			//Clear Feature
						if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )//端点
						{
							if(UsbSetupBuf->wValueH != 0 || UsbSetupBuf->wValueL != 0){
								errflag = 0xFF;
								break;
							}
							switch( UsbSetupBuf->wIndexL )
							{
							case 0x84:
								UEP4_CTRL = UEP4_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
								break;
							case 0x04:
								UEP4_CTRL = UEP4_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
								break;
							case 0x83:
								UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
								break;
							case 0x03:
								UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
								break;
							case 0x82:
								UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
								break;
							case 0x02:
								UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
								break;
							case 0x81:
								UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
								break;
							case 0x01:
								UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
								break;
							default:		//其他不支持的端点
								errflag = 0xFF;
								break;
							}
						}
						else if((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE){//设备
							if(UsbSetupBuf->wValueH != 0 || UsbSetupBuf->wValueL != 1){
								errflag = 0xFF;
								break;
							}
							WakeUpEnFlag &= ~0x01;
						}
						else errflag = 0xFF;                                                //不是端点不支持
						break;
					case USB_SET_FEATURE:                                               /* Set Feature */
						if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )              /* 设置设备 */
						{
							if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
							{
								if( CfgDesc[ 7 ] & 0x20 ){
									WakeUpEnFlag |= 0x01;                                   /* 设置唤醒使能标志 */
								}
								else errflag = 0xFF;                                        /* 操作失败 */
							}
							else errflag = 0xFF;                                            /* 操作失败 */
						}
						else if( (UsbSetupBuf->bRequestType & 0x1F) == 0x02 )        /* 设置端点 */
						{
							if( ( ( (UINT16)UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
							{
								switch( ( (UINT16)UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
								{
								case 0x84:
									UEP4_CTRL = UEP4_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点4 IN STALL */
									break;
								case 0x04:
									UEP4_CTRL = UEP4_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点4 OUT STALL */
									break;
								case 0x83:
									UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点3 IN STALL */
									break;
								case 0x03:
									UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点3 OUT Stall */
									break;
								case 0x82:
									UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
									break;
								case 0x02:
									UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT Stall */
									break;
								case 0x81:
									UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点1 IN STALL */
									break;
								default:
									errflag = 0xFF;					//操作失败
									break;
								}
							}
							else errflag = 0xFF;					//操作失败
						}
						else errflag = 0xFF;						//操作失败
						break;
					case USB_GET_STATUS:
						if(SetupLen != 2){
							errflag = 0xFF;
							break;
						}
						Ep0Buffer[0] = Ep0Buffer[1] = 0x00;
						if((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE){
							if(CfgDesc[7] & 0x40) Ep0Buffer[0] |= 0x01;	// Self powered
							if(WakeUpEnFlag & 0x01) Ep0Buffer[0] |= 0x02;	// Remote wakeup enabled
						}
						else if((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_INTERF){
							if(UsbSetupBuf->wIndexH != 0 || UsbSetupBuf->wIndexL >= USBD_MAX_NUM_INTERFACES)
								errflag = 0xFF;
						}
						else if((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) != USB_REQ_RECIP_ENDP)
							errflag = 0xFF;
						len = 2;
						break;
					default:
						errflag = 0xFF;								//操作失败
						break;
					}
				}
			}
			else{
				errflag = 0xFF;										//包长度错误
			}
			if(errflag == 0xFF){
				UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
			}
			else if(len){                                                //上传数据或者状态阶段返回0长度包
				UEP0_T_LEN = len;
				UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
			}
			else{
				UEP0_T_LEN = 0;  //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
				UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1,返回应答ACK
			}
			break;
		case UIS_TOKEN_IN | 0:							//端点0上传
			switch(SetupReq)
			{
			case USB_GET_DESCRIPTOR:
				len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;    //本次传输长度
				memcpy( Ep0Buffer, pDescr, len );                            //加载上传数据
				SetupLen -= len;
				pDescr += len;
				UEP0_T_LEN = len;
				UEP0_CTRL ^= bUEP_T_TOG;                                     //同步标志位翻转
				break;
			case USB_SET_ADDRESS:
				USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
				UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
				break;
			default:
				UEP0_T_LEN = 0;                                              //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
				UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
				break;
			}
			break;
		case UIS_TOKEN_OUT | 0:							//端点0下传
			len = USB_RX_LEN;
			if(SetupReq == HID_SET_REPORT
				&& HidSetReportInterface == USBD_HID_INTERFACE
				&& HidSetReportType == 2 && len != 0){
				/* 键盘 Output report 的末字节为 LED bitmap；同时兼容报告 ID 在/不在数据段的主机。 */
				HidKeyboardLedState = Ep0Buffer[len - 1];
				numLock = !!(HidKeyboardLedState & 0x01);
				capsLock = !!(HidKeyboardLedState & 0x02);
				scrollLock = !!(HidKeyboardLedState & 0x04);
			}
			HidSetReportInterface = 0xFF;
			HidSetReportType = 0;
			UEP0_CTRL ^= bUEP_R_TOG;							//同步标志位翻转
			break;
		default:
			break;
		}
		UIF_TRANSFER = 0;										//写0清空中断
	}
	else if(UIF_BUS_RST){		//设备模式USB总线复位中断
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP0_T_LEN = 0;
		UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP1_T_LEN = 0;
		UEP2_T_LEN = 0;
		UEP3_T_LEN = 0;
		UEP4_T_LEN = 0;
		USB_DEV_AD = 0x00;
		UIF_SUSPEND = 0;
		UIF_TRANSFER = 0;
		UsbConfig = 0;
		SetupReq = 0;
		SetupLen = 0;
		pDescr = 0;
		Ready = 0;
		Endp1Busy = 0;
		Endp2Busy = 0;
		Endp3Busy = 0;
		Endp4Busy = 0;
		memset(HidIdle, 0, sizeof(HidIdle));
		HidProtocol = 1;
		HidKeyboardLedState = 0;
		HidSetReportInterface = 0xFF;
		HidSetReportType = 0;
		WakeUpEnFlag = 0;
		UIF_BUS_RST = 0;                                                 //清中断标志
	}
	else if(UIF_SUSPEND){		//USB总线挂起/唤醒完成
		UIF_SUSPEND = 0;
		if( USB_MIS_ST & bUMS_SUSPEND ){		//挂起
			WakeUpEnFlag |= 0x02;
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                              //USB或者RXD0有信号时可被唤醒
//             PCON |= PD;                                                          //睡眠
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = 0x00;
		}
		else{									//唤醒
			WakeUpEnFlag &= ~0x02;
		}
	}
	else{										//意外的中断,不可能发生的情况
		USB_INT_FG = 0xFF;						//清中断标志
	}
}








