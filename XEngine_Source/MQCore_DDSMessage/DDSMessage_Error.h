#pragma once
/********************************************************************
//    Created:     2021/09/22  13:19:10
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage\DDSMessage_Error.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage
//    File Base:   DDSMessage_Error
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出的错误
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                       DDS服务公有API导出错误
//////////////////////////////////////////////////////////////////////////
#define ERROR_XENGINE_MQCORE_XDDS_COMMAPI_TOPICCREATE_PARAMENT 0x160A1010   //参数错误，无法继续
#define ERROR_XENGINE_MQCORE_XDDS_COMMAPI_TOPICGET_PARAMENT 0x160A1021      //参数错误
#define ERROR_XENGINE_MQCORE_XDDS_COMMAPI_TOPICGET_NOTTOPIC 0x160A1022      //没有找到
//////////////////////////////////////////////////////////////////////////
//                       发布者错误表
//////////////////////////////////////////////////////////////////////////
#define ERROR_XENGINE_MQCORE_XDDS_PUBLISH_CREATE_PARAMENT 0x160A2001      //参数错误，无法创建发布者
#define ERROR_XENGINE_MQCORE_XDDS_PUBLISH_CREATE_NOTPORTRES 0x160A2003    //没有可用的资源了
#define ERROR_XENGINE_MQCORE_XDDS_PUBLISH_SEND_PARAMENT 0x160A2010        //发送数据失败，参数错误
#define ERROR_XENGINE_MQCORE_XDDS_PUBLISH_SEND_NOTPUB 0x160A2011          //没有找到发布者
//////////////////////////////////////////////////////////////////////////
//                       订阅者错误表
//////////////////////////////////////////////////////////////////////////
#define ERROR_XENGINE_MQCORE_XDDS_SUBSCRIBE_CREATE_PARAMENT 0x1602A01     //参数错误，创建失败
#define ERROR_XENGINE_MQCORE_XDDS_SUBCRIBE_INSERT_PARAMENT 0x1602A10      //参数错误
#define ERROR_XENGINE_MQCORE_XDDS_SUBCRIBE_INSERT_NOTFOUND 0x1602A11      //没有找到
#define ERROR_XENGINE_MQCORE_XDDS_SUBCRIBE_INSERT_NOTMATCH 0x1602A12      //不匹配,不需要加入
#define ERROR_XENGINE_MQCORE_XDDS_SUBSCRIBE_READ_PARAMENT 0x1602A20       //读取数据失败，参数错误
#define ERROR_XENGINE_MQCORE_XDDS_SUBSCRIBE_READ_NOTSUB 0x1602A21         //没有找到订阅者
#define ERROR_XENGINE_MQCORE_XDDS_SUBSCRIBE_READ_NOMSG 0x1602A23          //没有消息可以读取

