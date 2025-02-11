﻿#include "pch.h"
#include "SessionModule_Client.h"
/********************************************************************
//    Created:     2021/07/02  10:15:27
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Client\SessionModule_Client.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Client
//    File Base:   SessionModule_Client
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息会话服务
//    History:
*********************************************************************/
CSessionModule_Client::CSessionModule_Client()
{
}
CSessionModule_Client::~CSessionModule_Client()
{
}
///////////////////////////////////////////////////////////////////////////////
//                      公有函数
///////////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：SessionModule_Client_Init
函数功能：初始化客户端会话管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CSessionModule_Client::SessionModule_Client_Init()
{
    Session_IsErrorOccur = false;

    return true;
}
/************************************************************************
函数名称：SessionModule_Client_Destory
函数功能：销毁会话管理器
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
bool CSessionModule_Client::SessionModule_Client_Destory()
{
    Session_IsErrorOccur = false;

	st_Locker.lock();
	stl_MapSession.clear();
	st_Locker.unlock();
    return true;
}
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
bool CSessionModule_Client::SessionModule_Client_Create(LPCXSTR lpszClientAddr, XENGINE_PROTOCOL_USERINFO *pSt_UserInfo, int nNetType)
{
    Session_IsErrorOccur = false;

    if (NULL == lpszClientAddr)
    {
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return false;
    }
	XENGINE_SESSIONINFO st_SessionInfo;
	memset(&st_SessionInfo, '\0', sizeof(XENGINE_SESSIONINFO));

	st_SessionInfo.nNetType = nNetType;
	st_SessionInfo.nTimeStart = time(NULL);
	st_SessionInfo.st_UserInfo = *pSt_UserInfo;
	_tcsxcpy(st_SessionInfo.tszUserAddr, lpszClientAddr);

    st_Locker.lock();
	stl_MapSession.insert(make_pair(lpszClientAddr, st_SessionInfo));
    st_Locker.unlock();
    return true;
}
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
bool CSessionModule_Client::SessionModule_Client_Delete(LPCXSTR lpszClientAddr)
{
	Session_IsErrorOccur = false;

	st_Locker.lock();
	unordered_map<xstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
	if (stl_MapIterator != stl_MapSession.end())
	{
        stl_MapSession.erase(stl_MapIterator);
	}
	st_Locker.unlock();

	return true;
}
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
bool CSessionModule_Client::SessionModule_Client_DeleteByUser(LPCXSTR lpszClientUser)
{
	Session_IsErrorOccur = false;

	st_Locker.lock();
	for (auto stl_MapIterator = stl_MapSession.begin(); stl_MapIterator != stl_MapSession.end(); stl_MapIterator++)
	{
		if (0 == _tcsxnicmp(lpszClientUser, stl_MapIterator->second.st_UserInfo.tszUserName, _tcsxlen(lpszClientUser)))
		{
			stl_MapSession.erase(stl_MapIterator);
			break;
		}
	}
	st_Locker.unlock();

	return true;
}
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
bool CSessionModule_Client::SessionModule_Client_GetUser(LPCXSTR lpszSessionStr, XCHAR* ptszUserName /* = NULL */)
{
	Session_IsErrorOccur = false;

	if (NULL == lpszSessionStr)
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return false;
	}
	st_Locker.lock_shared();
	unordered_map<xstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszSessionStr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return false;
	}
	if (NULL != ptszUserName)
	{
		_tcsxcpy(ptszUserName, stl_MapIterator->second.st_UserInfo.tszUserName);
	}
	st_Locker.unlock_shared();
	return true;
}
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
bool CSessionModule_Client::SessionModule_Client_GetAddr(LPCXSTR lpszUserName, XCHAR* ptszUserAddr /* = NULL */)
{
	Session_IsErrorOccur = false;

	if (NULL == lpszUserName)
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return false;
	}
	bool bFound = false;
	st_Locker.lock_shared();
	for (auto stl_MapIterator = stl_MapSession.begin(); stl_MapIterator != stl_MapSession.end(); stl_MapIterator++)
	{
		if (0 == _tcsxncmp(lpszUserName, stl_MapIterator->second.st_UserInfo.tszUserName, _tcsxlen(lpszUserName)))
		{
			bFound = true;
			if (NULL != ptszUserAddr)
			{
				_tcsxcpy(ptszUserAddr, stl_MapIterator->second.tszUserAddr);
			}
			break;
		}
	}
	st_Locker.unlock_shared();

	if (!bFound)
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		return false;
	}
	return true;
}
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
bool CSessionModule_Client::SessionModule_Client_GetInfoByUser(LPCXSTR lpszUserName, XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	Session_IsErrorOccur = false;

	if (NULL == lpszUserName)
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return false;
	}
	bool bFound = false;
	st_Locker.lock_shared();
	for (auto stl_MapIterator = stl_MapSession.begin(); stl_MapIterator != stl_MapSession.end(); stl_MapIterator++)
	{
		if (0 == _tcsxncmp(lpszUserName, stl_MapIterator->second.st_UserInfo.tszUserName, _tcsxlen(lpszUserName)))
		{
			bFound = true;
			*pSt_UserInfo = stl_MapIterator->second.st_UserInfo;
			break;
		}
	}
	st_Locker.unlock_shared();

	if (!bFound)
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		return false;
	}
	return true;
}
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
bool CSessionModule_Client::SessionModule_Client_GetType(LPCXSTR lpszSessionStr, int* pInt_NetType)
{
	Session_IsErrorOccur = false;

	if (NULL == lpszSessionStr)
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return false;
	}
	st_Locker.lock_shared();
	unordered_map<xstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszSessionStr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return false;
	}
	*pInt_NetType = stl_MapIterator->second.nNetType;
	st_Locker.unlock_shared();
	return true;
}
/********************************************************************
函数名称：SessionModule_Client_GetExist
函数功能：指定客户端是否存在
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：输入客户端地址
 参数.二：lpszClientUser
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：输入客户端用户
返回值
  类型：逻辑型
  意思：是否成功
备注：不能同时为NULL,可以使用一个参数
*********************************************************************/
bool CSessionModule_Client::SessionModule_Client_GetExist(LPCXSTR lpszClientAddr, LPCXSTR lpszClientUser)
{
	Session_IsErrorOccur = false;

	if (NULL == lpszClientUser && NULL == lpszClientAddr)
	{
		Session_IsErrorOccur = true;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return false;
	}

	st_Locker.lock_shared();
	if (NULL == lpszClientAddr)
	{
		bool bFound = false;
		unordered_map<xstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.begin();
		for (int i = 0; stl_MapIterator != stl_MapSession.end(); stl_MapIterator++, i++)
		{
			if (0 == _tcsxnicmp(lpszClientUser, stl_MapIterator->second.st_UserInfo.tszUserName, _tcsxlen(stl_MapIterator->second.st_UserInfo.tszUserName)))
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			Session_IsErrorOccur = true;
			Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
			st_Locker.unlock_shared();
			return false;
		}
	}
	else
	{
		unordered_map<xstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
		if (stl_MapIterator == stl_MapSession.end())
		{
			Session_IsErrorOccur = true;
			Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
			st_Locker.unlock_shared();
			return false;
		}
	}
	st_Locker.unlock_shared();
	return true;
}
/********************************************************************
函数名称：SessionModule_Client_GetList
函数功能：获取客户端地址列表
 参数.一：ppptszClientList
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出客户端列表
 参数.二：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出列表个数
 参数.三：bAddr
  In/Out：In
  类型：逻辑型
  可空：Y
  意思：是否获取地址列表,否则用户列表
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CSessionModule_Client::SessionModule_Client_GetListAddr(XCHAR*** ppptszClientList, int* pInt_ListCount, bool bAddr)
{
	Session_IsErrorOccur = false;

	*pInt_ListCount = stl_MapSession.size();
	BaseLib_Memory_Malloc((XPPPMEM)ppptszClientList, stl_MapSession.size(), 128);

	st_Locker.lock_shared();
	unordered_map<xstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.begin();
	for (int i = 0; stl_MapIterator != stl_MapSession.end(); stl_MapIterator++, i++)
	{
		if (bAddr)
		{
			_tcsxcpy((*ppptszClientList)[i], stl_MapIterator->second.tszUserAddr);
		}
		else
		{
			_tcsxcpy((*ppptszClientList)[i], stl_MapIterator->second.st_UserInfo.tszUserName);
		}
	}
	st_Locker.unlock_shared();
	return true;
}