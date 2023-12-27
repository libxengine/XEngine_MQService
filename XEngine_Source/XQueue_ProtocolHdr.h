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
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER 0x7004         //请求消息队列编号信息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNUMBER 0x7005         //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUNREAD 0x7006         //请求未读消息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUNREAD 0x7007         //响应未读消息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICCREATE 0x7010    //主题创建请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICCREATE 0x7011    //主题创建回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICDELETE 0x7012    //主题删除请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICDELETE 0x7013    //主题删除回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICMODIFY 0x7014    //主题名修改请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICMODIFY 0x7015    //主题名修改回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICBIND 0x7016      //请求主题绑定操作
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICBIND 0x7017      //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICUNBIND 0x7018    //请求主题解绑操作
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICUNBIND 0x7019    //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQMSGMODIFY 0x7020      //消息修改请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPMSGMODIFY 0x7021      //消息修改回复
//用户协议
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERLOG 0x7020        //用户登录
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERLOG 0x7021        
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG 0x7022        //用户注册
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERREG 0x7023
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL 0x7024        //用户删除
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERDEL 0x7025
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSEROUT 0x7026        //用户注销
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSEROUT 0x7027
//Only TCP and WEBSOCKET
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY 0x70A0         //消息通知
///////////////////////////////////////////////////////////////////////////
//                          导出的数据结构
///////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
typedef struct
{
	XBYTE byAttrAll : 1;                                                 //通知所有
	XBYTE byAttrSelf : 1;                                                //自己也能接受
	XBYTE byAttrReply : 1;                                               //对方必须回复
	XBYTE byAttrResver3 : 1;
	XBYTE byAttrResver4 : 1;
	XBYTE byAttrResver5 : 1;
	XBYTE byAttrResver6 : 1;
	XBYTE byAttrResver7 : 1;
	XBYTE byAttrResver;
}XENGINE_PROTOCOL_MSGATTR, * LPXENGINE_PROTOCOL_MSGATTR;
//消息队列服务协议
typedef struct 
{
	XCHAR tszMQKey[MAX_PATH];                                             //此消息的KEY，不能为空
	XCHAR tszMQUsr[MAX_PATH];                                             //此消息接受用户
	__int64x nSerial;                                                     //包序列号
	__int64x nPubTime;                                                    //发布时间，根据自己需求配置时区,-1表示投递的消息通过用户主动获取,0表示即时发布消息给当前订阅的在线用户,>0表示发布的日期
	int nKeepTime;                                                        //可用时间,单位秒,>0 超时秒数
	XENGINE_PROTOCOL_MSGATTR st_MSGAttr;                                  //消息属性
}XENGINE_PROTOCOL_XMQ, * LPXENGINE_PROTOCOL_XMQ;
typedef struct 
{
	XCHAR tszMQKey[256];                                                  //主题名		  
	__int64x nCount;                                                      //总个数
	__int64x nFirstNumber;                                                //起始编码
	__int64x nLastNumber;                                                 //末尾编号
}XENGINE_MQNUMBER, * LPXENGINE_MQNUMBER;
typedef struct 
{
	XCHAR tszMQKey[256];                                               //原始主题名		
}XENGINE_MQTOPIC, * LPXENGINE_MQTOPIC;
#pragma pack(pop)