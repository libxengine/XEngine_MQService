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
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDEL 0x7004            //删除包请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDEL 0x7005            //删除包回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE 0x7006         //主题创建请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPCREATE 0x7007         //主题创建回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDELETE 0x7008         //主题删除请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDELETE 0x7009         //主题删除回复
//Only TCP WEBSOCKET
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNOTIFY 0x7010         //请求订阅
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNOTIFY 0x7011         //订阅回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQSERIAL 0x7012         //请求序列号操作
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPSERIAL 0x7013         //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER 0x7014         //请求消息队列编号信息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNUMBER 0x7015         //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY 0x70A0         //消息通知
//用户协议
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERLOG 0x7020       
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERLOG 0x7021
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG 0x7022
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERREG 0x7023
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL 0x7024
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERDEL 0x7025
//消息分发服务
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REQTOPICCREATE 0x8001   //创建主题请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REPTOPICCREATE 0x8002   //创建主题回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REQTOPICDELETE 0x8003   //主题被删除
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REPTOPICDELETE 0x8004   //主题被删除
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REQTOPICQUERY 0x8005    //主题请求是否存在
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REPTOPICQUERY 0x8006    //回复
///////////////////////////////////////////////////////////////////////////
//                          导出的数据结构
///////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
//消息队列服务协议
typedef struct tag_XEngine_ProtocolXmq
{
	CHAR tszMQKey[256];                                                   //此消息的KEY，不能为空
	__int64x nSerial;                                                     //包序列号
	__int64x nPubTime;                                                    //发布时间，根据自己需求配置时区
	int nKeepTime;                                                        //保持时间,单位秒,-1 永久存在 0 一次就结束,>0 保存秒数
	int nGetTimer;                                                        //可以获取的次数
}XENGINE_PROTOCOL_XMQ, * LPXENGINE_PROTOCOL_XMQ;
typedef struct tag_XEngine_MQNumber
{
	TCHAR tszMQKey[256];                                                  //主题名		  
	__int64x nCount;                                                      //总个数
	__int64x nFirstNumber;                                                //起始编码
	__int64x nLastNumber;                                                 //末尾编号
}XENGINE_MQNUMBER, * LPXENGINE_MQNUMBER;
//数据分发服务子协议
typedef struct tag_XEngine_ProtocolXdds
{
	CHAR tszTopic[MAX_PATH];                                              //主题
	CHAR tszDDSAddr[64];                                                  //分发地址
	BOOL bCreater;                                                        //是否是创建者
	BOOL bTcp;                                                            //是否启用TCP，默认UDP
	int nPort;                                                            //端口
}XENGINE_PROTOCOL_XDDS, * LPXENGINE_PROTOCOL_XDDS;
#pragma pack(pop)