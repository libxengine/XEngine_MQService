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
#define XENGINE_MQAPP_NETTYPE_MQTT 4
///////////////////////////////////////////////////////////////////////////
//                          导出的数据定义
///////////////////////////////////////////////////////////////////////////
//消息队列协议
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST 0x7000           //投递包请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST 0x7001           //投递包回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET 0x7002            //获取包请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET 0x7003            //获取包回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDELETE 0x7004         //消息删除请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDELETE 0x7005         //消息删除回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQMODIFY 0x7006         //消息修改请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPMODIFY 0x7007         //消息修改回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER 0x7008         //请求消息队列编号信息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNUMBER 0x7009         //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUNREAD 0x7010         //请求未读消息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUNREAD 0x7011         //响应未读消息
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICCREATE 0x7100    //主题创建请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICCREATE 0x7101    //主题创建回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICDELETE 0x7102    //主题删除请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICDELETE 0x7103    //主题删除回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICMODIFY 0x7104    //主题名修改请求
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICMODIFY 0x7105    //主题名修改回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICBIND 0x7106      //请求主题绑定操作
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICBIND 0x7107      //回复
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICUNBIND 0x7108    //请求主题解绑操作
#define XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICUNBIND 0x7109    //回复
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
//                          协议错误定义
///////////////////////////////////////////////////////////////////////////
#define ERROR_XENGINE_MESSAGE_AUTH_LOGGED 0x001                           //已经登陆过了
#define ERROR_XENGINE_MESSAGE_AUTH_HTTP 0x002                             //HTTP PASS验证失败
#define ERROR_XENGINE_MESSAGE_AUTH_USERPASS 0x003                         //用户或者密码错误
#define ERROR_XENGINE_MESSAGE_AUTH_NOTLOGIN 0x004                         //没有登陆,禁止操作

#define ERROR_XENGINE_MESSAGE_XMQ_EXISTED 0x101                           //消息存在
#define ERROR_XENGINE_MESSAGE_XMQ_INSERT 0x102                            //插入数据库失败
#define ERROR_XENGINE_MESSAGE_XMQ_MSGQUERY 0x103                          //查询消息失败
#define ERROR_XENGINE_MESSAGE_XMQ_BELONG 0x104                            //指定用户消息,无权使用
#define ERROR_XENGINE_MESSAGE_XMQ_TIMEOUT 0x105                           //消息超时,无权使用
#define ERROR_XENGINE_MESSAGE_XMQ_PUBTIME 0x106                           //定时消息,无权使用
#define ERROR_XENGINE_MESSAGE_XMQ_KEYQUERY 0x107                          //查询绑定消息失败
#define ERROR_XENGINE_MESSAGE_XMQ_SERIAL 0x108                            //消息序号错误
#define ERROR_XENGINE_MESSAGE_XMQ_CREATEKEY 0x109                         //创建消息KEY失败
#define ERROR_XENGINE_MESSAGE_XMQ_BINDOWNER 0x110                         //绑定所有者失败
#define ERROR_XENGINE_MESSAGE_XMQ_DELOWNER 0x111                          //删除所有者失败
#define ERROR_XENGINE_MESSAGE_XMQ_KEYUP 0x112                             //更新KEY失败
#define ERROR_XENGINE_MESSAGE_XMQ_BINDKEY 0x113                           //绑定KEY失败
#define ERROR_XENGINE_MESSAGE_XMQ_DELKEY 0x114                            //删除KEY失败
#define ERROR_XENGINE_MESSAGE_XMQ_QUERYOWNER 0x115                        //查询所有者失败
#define ERROR_XENGINE_MESSAGE_XMQ_MODIFYTOPIC 0x116                       //修改主题失败
#define ERROR_XENGINE_MESSAGE_XMQ_MODIFYMSG 0x117                         //修改消息失败

#define ERROR_XENGINE_MESSAGE_HTTP_PARAMENT 0x201                         //HTTP请求参数错误
#define ERROR_XENGINE_MESSAGE_HTTP_AUTHORIZE 0x202                        //验证失败没有权限
#define ERROR_XENGINE_MESSAGE_HTTP_NOTFOUND 0x203                         //没有找到请求的数据
#define ERROR_XENGINE_MESSAGE_HTTP_PARSE 0x204                            //解析失败,负载协议错误
#define ERROR_XENGINE_MESSAGE_HTTP_SERVICE 0x205                          //服务器错误
#define ERROR_XENGINE_MESSAGE_HTTP_APIREG 0x206                           //HTTP 远程注册失败
#define ERROR_XENGINE_MESSAGE_HTTP_EXISTED 0x207                          //用户已经存在
#define ERROR_XENGINE_MESSAGE_HTTP_REGISTER 0x208                         //用户注册失败,数据错误
#define ERROR_XENGINE_MESSAGE_HTTP_APIDEL 0x209                           //HTTP API删除失败
#define ERROR_XENGINE_MESSAGE_HTTP_DELETE 0x210                           //本地删除失败
///////////////////////////////////////////////////////////////////////////
//                          导出的数据结构
///////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
typedef struct
{
	XBYTE byAttrAll : 1;                                                 //通知所有,不限主题,仅在线列表有效
	XBYTE byAttrSelf : 1;                                                //自己也能接受
	XBYTE byAttrReply : 1;                                               //对方必须回复
	XBYTE byAttrActive : 1;                                              //主动获取不受限制,比如超时的,不是自己发给自己的
	XBYTE byAttrResver4 : 1;
	XBYTE byAttrResver5 : 1;
	XBYTE byAttrResver6 : 1;
	XBYTE byAttrResver7 : 1;
	XBYTE byAttrResver;
}XENGINE_PROTOCOL_MSGATTR, * LPXENGINE_PROTOCOL_MSGATTR;
//消息队列服务协议
typedef struct 
{
	XCHAR tszMQKey[XPATH_MAX];                                             //此消息的KEY，不填就是 XEngine_CommKey
	XCHAR tszMQUsr[XPATH_MAX];                                             //此消息接受用户
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