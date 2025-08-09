/*******************************************************************************
* File Name          : CompositeHID.C
* Date               : 2023/12/30
* Description        : CH549模拟CustomHID+HID复合设备
********************************************************************************/
#include "CH549.H"
#include "DEBUG.H"

#include "BasicIO.h"
#include "ParaConfig.h"
#include "ALK_Func.h"

#define HID_EPIN_ADDR						0x81U // HID使用上行端点1
#define HID_EPOUT_ADDR						0x01U // HID使用下行端点1
#define CUSTOM_HID_EPIN_ADDR				0x82U // 自定义HID使用上行端点2
#define CUSTOM_HID_EPOUT_ADDR				0x02U // 自定义HID使用下行端点2
#define RGB_HID_EPIN_ADDR					0x83U // RGB自定义HID使用上行端点3
#define RGB_HID_EPOUT_ADDR					0x03U // RGB自定义HID使用下行端点3

#define THIS_ENDP0_SIZE         64 // 端点0数据包大小
#define ENDP1_IN_SIZE           22 // HID端点IN数据包大小
#define ENDP1_OUT_SIZE          64 // HID端点OUT数据包大小
#define ENDP2_IN_SIZE           64 // 自定义HID端点IN数据包大小
#define ENDP2_OUT_SIZE          64 // 自定义HID端点OUT数据包大小
#define ENDP3_IN_SIZE           64 // RGB自定义HID端点IN数据包大小
#define ENDP3_OUT_SIZE          64 // RGB自定义HID端点OUT数据包大小

#define USBD_MAX_NUM_INTERFACES				3 // 接口个数
#define USBD_HID_INTERFACE					0 // HID接口编号
#define USBD_CUSTOM_HID_INTERFACE			1 // 自定义HID接口编号
#define USBD_RGB_HID_INTERFACE				2 // RGB自定义HID接口编号

#define HID_FS_BINTERVAL			0x1 // HID轮询间隔
#define CUSTOM_HID_FS_BINTERVAL		0x1 // 自定义HID轮询间隔
#define RGB_HID_FS_BINTERVAL		0x1 // RGB自定义HID轮询间隔

#define Vendor_ID		0x1A86	// 厂商ID
#define Product_ID		22660	// 产品ID


#define UsbSetupBuf		((PUSB_SETUP_REQ)Ep0Buffer)

#pragma  NOAREGS


extern UINT8C FIRMWARE_VERSION[4]; // 固件版本
extern uint8_t asyncFlag; // 异步操作标志
extern UINT8X WakeUpEnFlag; // 远程唤醒使能标志
extern UINT8X MySrNumInfo[]; // 序列号


void Enp1IntIn(UINT8 *buf, UINT8 len);
void Enp2IntIn(UINT8 *buf, UINT8 len);

void CH554USBDevWakeup();
void USBDeviceInit();









