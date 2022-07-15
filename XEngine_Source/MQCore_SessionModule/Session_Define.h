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
//                        导出回调函数
//////////////////////////////////////////////////////////////////////////
typedef void(CALLBACK* CALLBACK_MESSAGEQUEUE_SESSIONMODULE_CLIENT_TIMEOUT)(LPCSTR lpszClientAddr, LPCTSTR lpszClientUser, ENUM_MQCORE_SESSION_CLIENT_TYPE enClientType, LPVOID lParam);
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
 参数.一：nSessionTime
  In/Out：In
  类型：整数型
  可空：N
  意思：输入会话超时时间,单位秒
 参数.二：fpCall_Timeout
  In/Out：In/Out
  类型：回调函数
  可空：N
  意思：设置会话超时回调
 参数.三：lParam
  In/Out：In/Out
  类型：无类型指针
  可空：Y
  意思：回调函数自定义参数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Client_Init(int nSessionTime, CALLBACK_MESSAGEQUEUE_SESSIONMODULE_CLIENT_TIMEOUT fpCall_Timeout, LPVOID lParam = NULL);
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
  意思：输入客户端地址
 参数.二：lpszUserName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
 参数.三：nNetType
  In/Out：In
  类型：整数型
  可空：N
  意思：客户端的网络连接类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Client_Create(LPCTSTR lpszClientAddr, LPCTSTR lpszUserName, int nNetType);
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
函数名称：SessionModule_Client_GetUser
函数功能：通过会话ID获取用户
 参数.一：lpszSessionStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入会话ID
 参数.二：ptszUserName
  In/Out：Out
  类型：字符指针
  可空：Y
  意思：输出对应的用户名
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
extern "C" BOOL SessionModule_Client_GetUser(LPCTSTR lpszSessionStr, TCHAR* ptszUserName = NULL);
/********************************************************************
函数名称：SessionModule_Client_Heart
函数功能：触发一次心跳
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要触发的客户端
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL SessionModule_Client_Heart(LPCTSTR lpszClientAddr);
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