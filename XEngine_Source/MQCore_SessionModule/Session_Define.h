#pragma once
/********************************************************************
//    Created:     2021/07/02  10:39:14
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\Session_Define.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule
//    File Base:   Session_Define
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出的定义
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                        导出枚举类型
//////////////////////////////////////////////////////////////////////////
typedef enum
{
	ENUM_MQCORE_SESSION_CLIENT_TYPE_UNKNOW = 0,
	ENUM_MQCORE_SESSION_CLIENT_TYPE_TCP = 1,
	ENUM_MQCORE_SESSION_CLIENT_TYPE_WEBSOCKET = 2,
	ENUM_MQCORE_SESSION_CLIENT_TYPE_HTTP = 3
}ENUM_MQCORE_SESSION_CLIENT_TYPE;
//////////////////////////////////////////////////////////////////////////
//                        导出的数据结构
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	TCHAR tszNotifyAddr[128];
	ENUM_MQCORE_SESSION_CLIENT_TYPE enClientType;
}SESSION_NOTIFYCLIENT, * LPSESSION_NOTIFYCLIENT;
//////////////////////////////////////////////////////////////////////////
//                        导出函数
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD SessionModule_GetLastError(int* pInt_SysError = NULL);
/************************************************************************/
/*                        客户端会话                                    */
/************************************************************************/
/********************************************************************
函数名称：SessionModule_Client_Init
函数功能：初始化客户端会话管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Client_Init();
/************************************************************************
函数名称：SessionModule_Client_Destory
函数功能：销毁会话管理器
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
extern "C" BOOL SessionModule_Client_Destory();
/********************************************************************
函数名称：SessionModule_Client_Create
函数功能：创建一个用户会话
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Client_Create(LPCTSTR lpszClientAddr);
/********************************************************************
函数名称：SessionModule_Client_Delete
函数功能：删除一个用户
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Client_Delete(LPCTSTR lpszClientAddr);
/************************************************************************
函数名称：SessionModule_Client_Get
函数功能：获得客户端对应消息信息
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入客户端信息
 参数.二：pSt_MQProtocol
  In/Out：Out
  类型：数据结构指针
  可空：N
  意思：输出消息内容
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
extern "C" BOOL SessionModule_Client_Get(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol);
/************************************************************************
函数名称：SessionModule_Client_Set
函数功能：设置客户端信息
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入客户端信息
 参数.二：pSt_MQProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入消息内容
返回值
  类型：逻辑型
  意思：是否获取成功
备注：
************************************************************************/
extern "C" BOOL SessionModule_Client_Set(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol);
/********************************************************************
函数名称：SessionModule_Client_ADDSerial
函数功能：序列号自加
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入客户端地址
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Client_ADDSerial(LPCTSTR lpszClientAddr);
/************************************************************************/
/*                        订阅                                          */
/************************************************************************/
/********************************************************************
函数名称：SessionModule_Notify_Create
函数功能：创建主题
 参数.一：lpszTopicName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Notify_Create(LPCTSTR lpszTopicName);
/********************************************************************
函数名称：SessionModule_Notify_Destory
函数功能：销毁一个主题
 参数.一：lpszTopicName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Notify_Destory(LPCTSTR lpszTopicName);
/************************************************************************
函数名称：SessionModule_Notify_Insert
函数功能：插入一个订阅客户端到指定主题
 参数.一：lpszTopicStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
 参数.二：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要插入的客户端
 参数.三：enClientType
  In/Out：In
  类型：枚举型
  可空：N
  意思：客户端类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
extern "C" BOOL SessionModule_Notify_Insert(LPCTSTR lpszTopicStr, LPCTSTR lpszClientAddr, ENUM_MQCORE_SESSION_CLIENT_TYPE enClientType);
/************************************************************************
函数名称：SessionModule_Notify_Delete
函数功能：从一个主题中删除一个订阅客户端
 参数.一：lpszTopicStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
 参数.二：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要删除的客户端
 参数.三：enClientType
  In/Out：In
  类型：枚举型
  可空：N
  意思：客户端类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
extern "C" BOOL SessionModule_Notify_Delete(LPCTSTR lpszTopicStr, LPCTSTR lpszClientAddr, ENUM_MQCORE_SESSION_CLIENT_TYPE enClientType);
/********************************************************************
函数名称：SessionModule_Notify_GetList
函数功能：获取主题对应的订阅列表
 参数.一：lpszTopicStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
 参数.二：pppSt_ListClient
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出客户端地址列表
 参数.三：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出列表个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Notify_GetList(LPCTSTR lpszTopicStr, SESSION_NOTIFYCLIENT *** pppSt_ListClient, int* pInt_ListCount);