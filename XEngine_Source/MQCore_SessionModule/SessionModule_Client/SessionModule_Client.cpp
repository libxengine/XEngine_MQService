#include "pch.h"
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
BOOL CSessionModule_Client::SessionModule_Client_Init()
{
    Session_IsErrorOccur = FALSE;

    return TRUE;
}
/************************************************************************
函数名称：SessionModule_Client_Destory
函数功能：销毁会话管理器
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
BOOL CSessionModule_Client::SessionModule_Client_Destory()
{
    Session_IsErrorOccur = FALSE;

    return TRUE;
}
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
BOOL CSessionModule_Client::SessionModule_Client_Create(LPCTSTR lpszClientAddr)
{
    Session_IsErrorOccur = FALSE;

    if (NULL == lpszClientAddr)
    {
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return FALSE;
    }
	XENGINE_SESSIONINFO st_SessionInfo;
	memset(&st_SessionInfo, '\0', sizeof(XENGINE_SESSIONINFO));

	st_SessionInfo.bOrder = TRUE;
	st_SessionInfo.nSerialPos = 1;
	st_SessionInfo.nStartTime = time(NULL);

    st_Locker.lock();
	stl_MapSession.insert(make_pair(lpszClientAddr, st_SessionInfo));
    st_Locker.unlock();
    return TRUE;
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
BOOL CSessionModule_Client::SessionModule_Client_Delete(LPCTSTR lpszClientAddr)
{
	Session_IsErrorOccur = FALSE;

	st_Locker.lock();
	unordered_map<tstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
	if (stl_MapIterator != stl_MapSession.end())
	{
        stl_MapSession.erase(stl_MapIterator);
	}
	st_Locker.unlock();

	return TRUE;
}
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
BOOL CSessionModule_Client::SessionModule_Client_Get(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol)
{
    Session_IsErrorOccur = FALSE;

    if ((NULL == lpszClientAddr) || (NULL == pSt_MQProtocol))
    {
        Session_IsErrorOccur = TRUE;
        Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
        return FALSE;
    }
	st_Locker.lock_shared();
	unordered_map<tstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	pSt_MQProtocol->nSerial = stl_MapIterator->second.nSerialPos;
	_tcscpy(pSt_MQProtocol->tszMQKey, stl_MapIterator->second.tszKeyStr);
	st_Locker.unlock_shared();
    return TRUE;
}
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
BOOL CSessionModule_Client::SessionModule_Client_Set(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol)
{
    Session_IsErrorOccur = FALSE;

    if ((NULL == lpszClientAddr) || (NULL == pSt_MQProtocol))
    {
        Session_IsErrorOccur = TRUE;
        Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
        return FALSE;
    }
    //开始取出数据
    st_Locker.lock_shared();
    unordered_map<tstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	stl_MapIterator->second.nSerialPos = pSt_MQProtocol->nSerial;
	_tcscpy(stl_MapIterator->second.tszKeyStr, pSt_MQProtocol->tszMQKey);
    st_Locker.unlock_shared();
    return TRUE;
}
/********************************************************************
函数名称：SessionModule_Client_SetOrder
函数功能：设置客户端队列读取顺序
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：要设置的客户端
 参数.二：lpszKeyStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：要设置的客户端
 参数.三：bOrder
  In/Out：In
  类型：逻辑型
  可空：N
  意思：真为顺序读取,假为倒序
 参数.四：nMQSerial
  In/Out：In
  类型：整数型
  可空：N
  意思：消息队列位置
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Client::SessionModule_Client_SetOrder(LPCTSTR lpszClientAddr, LPCTSTR lpszKeyStr, BOOL bOrder, __int64x nMQSerial)
{
	Session_IsErrorOccur = FALSE;

	if (NULL == lpszClientAddr)
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return FALSE;
	}
	//开始取出数据
	st_Locker.lock_shared();
	unordered_map<tstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	stl_MapIterator->second.bOrder = bOrder;
	stl_MapIterator->second.nSerialPos = nMQSerial;
	_tcscpy(stl_MapIterator->second.tszKeyStr, lpszKeyStr);
	st_Locker.unlock_shared();
	return TRUE;
}
/********************************************************************
函数名称：SessionModule_Client_ADDDelSerial
函数功能：序列号自加自减
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
BOOL CSessionModule_Client::SessionModule_Client_ADDDelSerial(LPCTSTR lpszClientAddr)
{
	Session_IsErrorOccur = FALSE;

	if (NULL == lpszClientAddr)
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return FALSE;
	}

	st_Locker.lock_shared();
	unordered_map<tstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	if (stl_MapIterator->second.bOrder)
	{
		stl_MapIterator->second.nSerialPos++;
	}
	else
	{
		stl_MapIterator->second.nSerialPos--;
	}
	st_Locker.unlock_shared();
	return TRUE;
}