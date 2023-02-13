#pragma once
/********************************************************************
//    Created:     2021/07/09  16:37:59
//    File Name:   D:\XEngine_MQService\XEngine_Source\XQueue_ProtocolHdr.h
//    File Path:   D:\XEngine_MQService\XEngine_Source
//    File Base:   XQueue_ProtocolHdr
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列协议
//    History:
*********************************************************************/
#define XENGINE_MQAPP_NETTYPE_TCP 1
#define XENGINE_MQAPP_NETTYPE_HTTP 2
#define XENGINE_MQAPP_NETTYPE_WEBSOCKET 3
///////////////////////////////////////////////////////////////////////////
//                          导出的数据定义
///////////////////////////////////////////////////////////////////////////
//消息队列协议
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST 0x7000           //投递包请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST 0x7001           //投递包回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET 0x7002            //获取包请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET 0x7003            //获取包回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE 0x7004         //主题创建请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPCREATE 0x7005         //主题创建回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDELETE 0x7006         //主题删除请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDELETE 0x7007         //主题删除回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICMODIFY 0x7008    //主题名修改请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICMODIFY 0x7009    //主题名修改回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQMSGMODIFY 0x7010      //消息修改请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPMSGMODIFY 0x7011      //消息修改回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICBIND 0x7012      //请求主题绑定操作
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICBIND 0x7013      //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER 0x7014         //请求消息队列编号信息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNUMBER 0x7015         //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUNREAD 0x7016         //请求未读消息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUNREAD 0x7017         //响应未读消息
//用户协议
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERLOG 0x7020       
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERLOG 0x7021
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG 0x7022
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERREG 0x7023
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL 0x7024
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERDEL 0x7025
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSEROUT 0x7026       
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSEROUT 0x7027
//Only TCP and WEBSOCKET
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNOTIFY 0x7030         //请求订阅
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNOTIFY 0x7031         //订阅回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY 0x703A         //消息通知
///////////////////////////////////////////////////////////////////////////
//                          导出的数据结构
///////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
//消息队列服务协议
typedef struct 
{
	CHAR tszMQKey[256];                                                   //此消息的KEY，不能为空
	__int64x nSerial;                                                     //包序列号
	__int64x nPubTime;                                                    //发布时间，根据自己需求配置时区
	int nKeepTime;                                                        //保持时间,单位秒,-1 永久存在 0 一次就结束,>0 保存秒数
	int nGetTimer;                                                        //可以获取的次数
}XENGINE_PROTOCOL_XMQ, * LPXENGINE_PROTOCOL_XMQ;
typedef struct 
{
	TCHAR tszMQKey[256];                                                  //主题名		  
	__int64x nCount;                                                      //总个数
	__int64x nFirstNumber;                                                //起始编码
	__int64x nLastNumber;                                                 //末尾编号
}XENGINE_MQNUMBER, * LPXENGINE_MQNUMBER;
typedef struct 
{
	TCHAR tszMQKey[256];                                               //原始主题名		
}XENGINE_MQTOPIC, * LPXENGINE_MQTOPIC;
#pragma pack(pop)