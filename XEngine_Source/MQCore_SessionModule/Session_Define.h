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
//                        导出函数
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG SessionModule_GetLastError(int* pInt_SysError = NULL);
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
extern "C" bool SessionModule_Client_Init();
/************************************************************************
函数名称：SessionModule_Client_Destory
函数功能：销毁会话管理器
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
extern "C" bool SessionModule_Client_Destory();
/********************************************************************
函数名称：SessionModule_Client_Create
函数功能：创建一个用户会话
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入客户端地址
 参数.二：pSt_UserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入用户信息
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
extern "C" bool SessionModule_Client_Create(LPCXSTR lpszClientAddr, XENGINE_PROTOCOL_USERINFO * pSt_UserInfo, int nNetType);
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
extern "C" bool SessionModule_Client_Delete(LPCXSTR lpszClientAddr);
/********************************************************************
函数名称：SessionModule_Client_DeleteByUser
函数功能：通过同户名删除会话
 参数.一：lpszClientUser
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的客户端
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool SessionModule_Client_DeleteByUser(LPCXSTR lpszClientUser);
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
extern "C" bool SessionModule_Client_GetUser(LPCXSTR lpszSessionStr, XCHAR* ptszUserName = NULL);
/********************************************************************
函数名称：SessionModule_Client_GetAddr
函数功能：获取用户地址
 参数.一：lpszUserName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
 参数.二：ptszUserAddr
  In/Out：Out
  类型：字符指针
  可空：Y
  意思：输出用户地址
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool SessionModule_Client_GetAddr(LPCXSTR lpszUserName, XCHAR* ptszUserAddr = NULL);
/********************************************************************
函数名称：SessionModule_Client_GetInfoByUser
函数功能：通过用户名获取用户信息
 参数.一：lpszUserName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
 参数.二：pSt_UserInfo
  In/Out：Out
  类型：数据结构指针
  可空：N
  意思：输出用户信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool SessionModule_Client_GetInfoByUser(LPCXSTR lpszUserName, XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
/********************************************************************
函数名称：SessionModule_Client_GetType
函数功能：通过客户端获得连接的网络类型
 参数.一：lpszSessionStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的客户端
 参数.二：pInt_NetType
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出网络类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool SessionModule_Client_GetType(LPCXSTR lpszSessionStr, int* pInt_NetType);