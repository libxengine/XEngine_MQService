#include "pch.h"
#include "DBModule_MQUser.h"
/********************************************************************
//    Created:     2022/03/17  09:47:36
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQUser\DBModule_MQUser.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQUser
//    File Base:   DBModule_MQUser
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列用户信息操作
//    History:
*********************************************************************/
CDBModule_MQUser::CDBModule_MQUser()
{
    xhDBSQL = 0;
}
CDBModule_MQUser::~CDBModule_MQUser()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DBModule_MQUser_Init
函数功能：初始化数据库管理器
 参数.一：pSt_DBConnector
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：数据MYSQL数据库连接信息
 参数.二：bMemoryQuery
  In/Out：In
  类型：逻辑型
  可空：N
  意思：是否启用高速缓存查询
 参数.三：fpCall_TimePublish
  In/Out：In
  类型：回调函数
  可空：N
  意思：定时消息发布回调函数
 参数.四：lParam
  In/Out：In
  类型：无类型指针
  可空：Y
  意思：回调函数自定义参数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector, bool bMemoryQuery, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH fpCall_TimePublish, XPVOID lParam /* = NULL */)
{
    DBModule_IsErrorOccur = false;

    if (NULL == pSt_DBConnector)
    {
        DBModule_IsErrorOccur = true;
        DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
        return false;
    }
    //连接数据库
    _tcsxcpy(pSt_DBConnector->tszDBName, _X("XEngine_MQUser"));
    if (!DataBase_MySQL_Connect(&xhDBSQL, pSt_DBConnector))
    {
        DBModule_IsErrorOccur = true;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return false;
    }

	m_bMemoryQuery = bMemoryQuery;
	bIsRun = true;
	m_lParam = lParam;
	lpCall_TimePublish = fpCall_TimePublish;
	pSTDThread = make_shared<std::thread>(DBModule_MQUser_TimeThread, this);
	if (NULL == pSTDThread)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_CREATETHREAD;
		return false;
	}
    return true;
}
/********************************************************************
函数名称：DBModule_MQUser_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_Destory()
{
    DBModule_IsErrorOccur = false;

	bIsRun = false;
	if (NULL != pSTDThread)
	{
		pSTDThread->join();
	}
    DataBase_MySQL_Close(xhDBSQL);
    return true;
}
/********************************************************************
函数名称：DBModule_MQUser_UserInsert
函数功能：插入用户
 参数.一：pSt_UserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要插入的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_UserInsert(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
    DBModule_IsErrorOccur = false;

	if (NULL == pSt_UserInfo)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	XCHAR tszSQLStatement[2048];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

    _xstprintf(tszSQLStatement, _X("INSERT INTO `UserInfo` (tszUserName,tszUserPass,tszEMailAddr,nPhoneNumber,nIDNumber,nUserState,nUserLevel,tszCreateTime) VALUES('%s','%s','%s',%lld,%lld,%d,%d,now())"), pSt_UserInfo->tszUserName, pSt_UserInfo->tszUserPass, pSt_UserInfo->tszEMailAddr, pSt_UserInfo->nPhoneNumber, pSt_UserInfo->nIDNumber, pSt_UserInfo->nUserState, pSt_UserInfo->nUserLevel);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

    return true;
}
/********************************************************************
函数名称：DBModule_MQUser_UserQuery
函数功能：查询用户
 参数.一：pSt_UserInfo
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要查询的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_UserQuery(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_UserInfo)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	if (m_bMemoryQuery)
	{
		if (MemoryCache_DBUser_DataQuery(pSt_UserInfo))
		{
			return true;
		}
	}
	//查询
	XNETHANDLE xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("SELECT * FROM `UserInfo` WHERE tszUserName = '%s' AND tszUserPass = '%s'"), pSt_UserInfo->tszUserName, pSt_UserInfo->tszUserPass);
	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	if (nllLine <= 0)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
		return false;
	}
	XCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
	if (NULL != pptszResult[3])
	{
		_tcsxcpy(pSt_UserInfo->tszEMailAddr, pptszResult[3]);
	}
	if (NULL != pptszResult[4])
	{
		pSt_UserInfo->nPhoneNumber = _ttxoll(pptszResult[4]);
	}
	if (NULL != pptszResult[5])
	{
		pSt_UserInfo->nIDNumber = _ttxoll(pptszResult[5]);
	}
	if (NULL != pptszResult[6])
	{
		pSt_UserInfo->nUserState = _ttxoi(pptszResult[6]);
	}
	if (NULL != pptszResult[7])
	{
		pSt_UserInfo->nUserLevel = _ttxoi(pptszResult[7]);
	}
	if (NULL != pptszResult[8])
	{
		_tcsxcpy(pSt_UserInfo->tszLoginTime, pptszResult[8]);
	}
	if (NULL != pptszResult[9])
	{
		_tcsxcpy(pSt_UserInfo->tszCreateTime, pptszResult[9]);
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);

	if (m_bMemoryQuery)
	{
		MemoryCache_DBUser_DataInsert(pSt_UserInfo);
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_UserDelete
函数功能：删除用户
 参数.一：pSt_UserInfo
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要删除的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_UserDelete(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_UserInfo)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("DELETE FROM `UserInfo` WHERE tszUserName = '%s' AND tszUserPass = '%s' AND nIDNumber = '%lld'"), pSt_UserInfo->tszUserName, pSt_UserInfo->tszUserPass, pSt_UserInfo->nIDNumber);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	if (m_bMemoryQuery)
	{
		MemoryCache_DBUser_DataDelete(pSt_UserInfo);
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_UserUPDate
函数功能：更新用户
 参数.一：pSt_UserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要处理的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_UserUPDate(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_UserInfo)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	if (1 == pSt_UserInfo->nUserState)
	{
		_xstprintf(tszSQLStatement, _X("UPDATE `UserInfo` SET nUserState = %d,tszLoginTime = now() WHERE tszUserName = '%s'"), pSt_UserInfo->nUserState, pSt_UserInfo->tszUserName);
	}
	else
	{
		_xstprintf(tszSQLStatement, _X("UPDATE `UserInfo` SET nUserState = %d WHERE tszUserName = '%s'"), pSt_UserInfo->nUserState, pSt_UserInfo->tszUserName);
	}
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

	if (m_bMemoryQuery)
	{
		MemoryCache_DBUser_DataDelete(pSt_UserInfo);
		MemoryCache_DBUser_DataInsert(pSt_UserInfo);
	}

	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_UserList
函数功能：获取用户列表
 参数.一：pppSt_UserInfo
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出用户表信息
 参数.二：pInt_ListCount
  In/Out：Out
  类型：整数型
  可空：N
  意思：输出用户表个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_UserList(XENGINE_PROTOCOL_USERINFO*** pppSt_UserInfo, int* pInt_ListCount)
{
	DBModule_IsErrorOccur = false;

	//查询
	XNETHANDLE xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	XCHAR tszSQLStatement[256];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("SELECT * FROM `UserInfo`"));
	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	if (nllLine <= 0)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
		return false;
	}
	*pInt_ListCount = (int)nllLine;
	BaseLib_Memory_Malloc((XPPPMEM)pppSt_UserInfo, (int)nllLine, sizeof(XENGINE_PROTOCOL_USERINFO));
	for (__int64u i = 0; i < nllLine; i++)
	{
		XCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);

		if (NULL != pptszResult[1])
		{
			_tcsxcpy((*pppSt_UserInfo)[i]->tszUserName, pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			_tcsxcpy((*pppSt_UserInfo)[i]->tszUserPass, pptszResult[2]);
		}
		if (NULL != pptszResult[3])
		{
			_tcsxcpy((*pppSt_UserInfo)[i]->tszEMailAddr, pptszResult[3]);
		}
		if (NULL != pptszResult[4])
		{
			(*pppSt_UserInfo)[i]->nPhoneNumber = _ttxoll(pptszResult[4]);
		}
		if (NULL != pptszResult[5])
		{
			(*pppSt_UserInfo)[i]->nIDNumber = _ttxoll(pptszResult[5]);
		}
		if (NULL != pptszResult[6])
		{
			(*pppSt_UserInfo)[i]->nUserState = _ttxoi(pptszResult[6]);
		}
		if (NULL != pptszResult[7])
		{
			(*pppSt_UserInfo)[i]->nUserLevel = _ttxoi(pptszResult[7]);
		}
		if (NULL != pptszResult[8])
		{
			_tcsxcpy((*pppSt_UserInfo)[i]->tszLoginTime, pptszResult[8]);
		}
		if (NULL != pptszResult[9])
		{
			_tcsxcpy((*pppSt_UserInfo)[i]->tszCreateTime, pptszResult[9]);
		}
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return true;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DBModule_MQUser_KeyInsert
函数功能：插入一个绑定的用户消息
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要插入的内容
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_KeyInsert(XENGINE_DBUSERKEY* pSt_UserKey)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_UserKey)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	if (DBModule_MQUser_KeyQuery(pSt_UserKey))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EXIST;
		return false;
	}
	XCHAR tszSQLStatement[2048];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("INSERT INTO `UserKey` (tszKeyUser,tszKeyName,nKeySerial,tszUPTime,tszCreateTime) VALUES('%s','%s',%lld,now(),now())"), pSt_UserKey->tszUserName, pSt_UserKey->tszKeyName, pSt_UserKey->nKeySerial);

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_KeyQuery
函数功能：通过用户查询绑定的消息信息
 参数.一：pSt_UserKey
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要查询的信息,输出查询到的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_KeyQuery(XENGINE_DBUSERKEY* pSt_UserKey)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_UserKey)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	//查询
	XNETHANDLE xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	//名称为,消息名为必填
	_xstprintf(tszSQLStatement, _X("SELECT * FROM `UserKey` WHERE tszKeyUser = '%s' AND tszKeyName = '%s'"), pSt_UserKey->tszUserName, pSt_UserKey->tszKeyName);
	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	if (nllLine <= 0)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
		return false;
	}
	XCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
	if (NULL != pptszResult[3])
	{
		pSt_UserKey->nKeySerial = _ttxoll(pptszResult[3]);
	}
	if (NULL != pptszResult[4])
	{
		_tcsxcpy(pSt_UserKey->tszUPTime, pptszResult[4]);
	}
	if (NULL != pptszResult[5])
	{
		_tcsxcpy(pSt_UserKey->tszCreateTime, pptszResult[5]);
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_KeyList
函数功能：枚举用户关联的列表
 参数.一：lpszUser
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
 参数.二：lpszKeyName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要查询的KEY名称
 参数.三：pppSt_UserKey
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出用户列表信息
 参数.四：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出列表个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_KeyList(LPCXSTR lpszUser, LPCXSTR lpszKeyName, XENGINE_DBUSERKEY*** pppSt_UserKey, int* pInt_ListCount)
{
	DBModule_IsErrorOccur = false;

	if (NULL == lpszKeyName)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	//查询
	XNETHANDLE xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	//名称为,消息名为必填
	if (NULL != lpszUser && _tcsxlen(lpszKeyName) > 0)
	{
		_xstprintf(tszSQLStatement, _X("SELECT * FROM `UserKey` WHERE tszKeyUser = '%s' AND tszKeyName = '%s'"), lpszUser, lpszKeyName);
	}
	else if (NULL != lpszUser && (_tcsxlen(lpszKeyName) == 0))
	{
		_xstprintf(tszSQLStatement, _X("SELECT * FROM `UserKey` WHERE tszKeyUser = '%s'"), lpszUser);
	}
	else if (NULL == lpszUser && (_tcsxlen(lpszKeyName) > 0))
	{
		_xstprintf(tszSQLStatement, _X("SELECT * FROM `UserKey` WHERE tszKeyName = '%s'"), lpszKeyName);
	}

	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	if (nllLine <= 0)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
		return false;
	}
	*pInt_ListCount = (int)nllLine;
	BaseLib_Memory_Malloc((XPPPMEM)pppSt_UserKey, (int)nllLine, sizeof(XENGINE_DBUSERKEY));
	for (__int64u i = 0; i < nllLine; i++)
	{
		XCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);

		if (NULL != pptszResult[1])
		{
			_tcsxcpy((*pppSt_UserKey)[i]->tszUserName, pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			_tcsxcpy((*pppSt_UserKey)[i]->tszKeyName, pptszResult[2]);
		}
		if (NULL != pptszResult[3])
		{
			(*pppSt_UserKey)[i]->nKeySerial = _ttxoll(pptszResult[3]);
		}
		if (NULL != pptszResult[4])
		{
			_tcsxcpy((*pppSt_UserKey)[i]->tszUPTime, pptszResult[4]);
		}
		if (NULL != pptszResult[5])
		{
			_tcsxcpy((*pppSt_UserKey)[i]->tszCreateTime, pptszResult[5]);
		}
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_KeyDelete
函数功能：删除绑定的消息队列
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要删除的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_KeyDelete(XENGINE_DBUSERKEY* pSt_UserKey)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_UserKey)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	if ((_tcsxlen(pSt_UserKey->tszKeyName) > 0) && (_tcsxlen(pSt_UserKey->tszUserName) > 0))
	{
		_xstprintf(tszSQLStatement, _X("DELETE FROM `UserKey` WHERE tszKeyUser = '%s' AND tszKeyName = '%s'"), pSt_UserKey->tszUserName, pSt_UserKey->tszKeyName);
	}
	else if ((_tcsxlen(pSt_UserKey->tszKeyName) > 0) && (_tcsxlen(pSt_UserKey->tszUserName) <= 0))
	{
		_xstprintf(tszSQLStatement, _X("DELETE FROM `UserKey` WHERE tszKeyName = '%s'"), pSt_UserKey->tszKeyName);
	}
	else if ((_tcsxlen(pSt_UserKey->tszKeyName) <= 0) && (_tcsxlen(pSt_UserKey->tszUserName) > 0))
	{
		_xstprintf(tszSQLStatement, _X("DELETE FROM `UserKey` WHERE tszKeyUser = '%s'"), pSt_UserKey->tszUserName);
	}
	else
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_KeyUPDate
函数功能：更新用户绑定的消息
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要更新的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_KeyUPDate(XENGINE_DBUSERKEY* pSt_UserKey)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_UserKey)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("UPDATE `UserKey` SET nKeySerial = %lld WHERE tszKeyUser = '%s' AND tszKeyName = '%s'"), pSt_UserKey->nKeySerial, pSt_UserKey->tszUserName, pSt_UserKey->tszKeyName);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_KeyTopicUPDate
函数功能：更新队列信息表
 参数.一：lpszSourceTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入原始表名
 参数.二：lpszDestTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入目标表名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_KeyTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable)
{
	DBModule_IsErrorOccur = false;

	XCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	_xstprintf(tszSQLQuery, _X("UPDATE `UserKey` SET tszKeyName = '%s' WHERE tszKeyName = '%s'"), lpszDestTable, lpszSourceTable);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DBModule_MQUser_TimeInsert
函数功能：定时发布插入
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_TimeInsert(XENGINE_DBTIMERELEASE* pSt_DBInfo)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_DBInfo)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	XCHAR tszSQLStatement[10240];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("INSERT INTO `UserTime` (tszQueueName,nIDMsg,nIDTime,bActive,tszCreateTime) VALUES('%s',%lld,%lld,0,now())"), pSt_DBInfo->tszQueueName, pSt_DBInfo->nIDMsg, pSt_DBInfo->nIDTime);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_TimeQuery
函数功能：定时发布查询
 参数.一：pppSt_DBInfo
  In/Out：Out
  类型：三级指针
  可空：N
  意思：导出的列表
 参数.二：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：导出的个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_TimeQuery(XENGINE_DBTIMERELEASE*** pppSt_DBInfo, int* pInt_ListCount)
{
	DBModule_IsErrorOccur = false;

	//查询
	XNETHANDLE xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;
	XCHAR tszSQLStatement[1024];

	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
#ifdef _MSC_BUILD
	_xstprintf(tszSQLStatement, _X("SELECT * FROM `UserTime` WHERE nIDTime <= %lld AND bActive = 0"), time(NULL));
#else
	_xstprintf(tszSQLStatement, _X("SELECT * FROM `UserTime` WHERE nIDTime <= %ld AND bActive = 0"), time(NULL));
#endif

	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	*pInt_ListCount = (int)nllLine;
	BaseLib_Memory_Malloc((XPPPMEM)pppSt_DBInfo, *pInt_ListCount, sizeof(XENGINE_DBTIMERELEASE));
	for (__int64u i = 0; i < nllLine; i++)
	{
		XCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
		if (NULL != pptszResult[0])
		{
			_tcsxcpy((*pppSt_DBInfo)[i]->tszQueueName, pptszResult[0]);
		}
		if (NULL != pptszResult[1])
		{
			(*pppSt_DBInfo)[i]->nIDMsg = _ttxoll(pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			(*pppSt_DBInfo)[i]->nIDTime = _ttxoll(pptszResult[2]);
		}
		if (NULL != pptszResult[3])
		{
			(*pppSt_DBInfo)[i]->bActive = _ttxoi(pptszResult[3]);
		}
		if (NULL != pptszResult[4])
		{
			_tcsxcpy((*pppSt_DBInfo)[i]->tszCreateTime, pptszResult[4]);
		}
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);

	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_TimeDelete
函数功能：定时发布删除
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_TimeDelete(XENGINE_DBTIMERELEASE* pSt_DBInfo)
{
	DBModule_IsErrorOccur = false;

	XCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	if (pSt_DBInfo->nIDMsg > 0)
	{
		_xstprintf(tszSQLQuery, _X("DELETE FROM `UserTime` WHERE nIDMsg = %lld"), pSt_DBInfo->nIDMsg);
	}
	else if (_tcsxlen(pSt_DBInfo->tszQueueName) > 0)
	{
		_xstprintf(tszSQLQuery, _X("DELETE FROM `UserTime` WHERE tszQueueName = '%s'"), pSt_DBInfo->tszQueueName);
	}
	else
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_TimeUPDate
函数功能：更新定时发布表
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要更新的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_TimeUPDate(XENGINE_DBTIMERELEASE* pSt_DBInfo)
{
	DBModule_IsErrorOccur = false;

	XCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	_xstprintf(tszSQLQuery, _X("UPDATE `UserTime` SET bActive = 1 WHERE nIDMsg = %lld"), pSt_DBInfo->nIDMsg);

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_TimeClaer
函数功能：清理超时通知
 参数.一：nTime
  In/Out：In
  类型：整数型
  可空：Y
  意思：输入要清理的日期
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_TimeClaer(time_t nTime /* = 0 */)
{
	DBModule_IsErrorOccur = false;

	XCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	if (0 == nTime)
	{
		nTime = time(NULL);
	}
#ifdef _MSC_BUILD
	_xstprintf(tszSQLQuery, _X("DELETE FROM `UserTime` WHERE nIDTime <= %lld"), nTime);
#else
	_xstprintf(tszSQLQuery, _X("DELETE FROM `UserTime` WHERE nIDTime <= %ld"), nTime);
#endif

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_TimeTopicUPDate
函数功能：更新定时发布表
 参数.一：lpszSourceTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入原始表名
 参数.二：lpszDestTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入目标表名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_TimeTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable)
{
	DBModule_IsErrorOccur = false;

	XCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	_xstprintf(tszSQLQuery, _X("UPDATE `UserTime` SET tszQueueName = '%s' WHERE tszQueueName = '%s'"), lpszDestTable, lpszSourceTable);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_OwnerInsert
函数功能：主题所有者插入
 参数.一：pSt_DBOwner
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_OwnerInsert(XENGINE_DBTOPICOWNER* pSt_DBOwner)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_DBOwner)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	XCHAR tszSQLStatement[10240];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("INSERT IGNORE INTO `KeyOwner` (tszUserName,tszKeyName,tszCreateTime) VALUES('%s','%s',now())"), pSt_DBOwner->tszUserName, pSt_DBOwner->tszQueueName);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_OwnerDelete
函数功能：主题所有者删除
 参数.一：pSt_DBOwner
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_OwnerDelete(XENGINE_DBTOPICOWNER* pSt_DBOwner)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_DBOwner)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	__int64u nAffectRow = 0;
	XCHAR tszSQLStatement[10240];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	if ((_tcsxlen(pSt_DBOwner->tszUserName) > 0) && (_tcsxlen(pSt_DBOwner->tszQueueName) > 0))
	{
		_xstprintf(tszSQLStatement, _X("DELETE FROM `KeyOwner` WHERE tszUserName = '%s' AND tszKeyName = '%s'"), pSt_DBOwner->tszUserName, pSt_DBOwner->tszQueueName);
		if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement, &nAffectRow))
		{
			DBModule_IsErrorOccur = true;
			DBModule_dwErrorCode = DataBase_GetLastError();
			return false;
		}
		if (nAffectRow <= 0)
		{
			DBModule_IsErrorOccur = true;
			DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_NOTFOUND;
			return false;
		}
	}
	else
	{
		_xstprintf(tszSQLStatement, _X("DELETE FROM `KeyOwner` WHERE tszUserName = '%s'"), pSt_DBOwner->tszUserName);
		if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
		{
			DBModule_IsErrorOccur = true;
			DBModule_dwErrorCode = DataBase_GetLastError();
			return false;
		}
	}
	
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_OwnerQuery
函数功能：主题所有者查询
 参数.一：pSt_DBOwner
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_OwnerQuery(XENGINE_DBTOPICOWNER* pSt_DBOwner)
{
	DBModule_IsErrorOccur = false;

	//查询
	XNETHANDLE xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;
	XCHAR tszSQLStatement[1024];

	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("SELECT * FROM `KeyOwner` WHERE tszUserName = '%s' AND tszKeyName = '%s'"), pSt_DBOwner->tszUserName, pSt_DBOwner->tszQueueName);

	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	if (nllLine <= 0)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQUser_OwnerTopicUPDate
函数功能：更新所有者表
 参数.一：lpszSourceTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入原始表名
 参数.二：lpszDestTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入目标表名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQUser::DBModule_MQUser_OwnerTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable)
{
	DBModule_IsErrorOccur = false;

	XCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	_xstprintf(tszSQLQuery, _X("UPDATE `KeyOwner` SET tszKeyName = '%s' WHERE tszKeyName = '%s'"), lpszDestTable, lpszSourceTable);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
//                         线程函数
//////////////////////////////////////////////////////////////////////////
XHTHREAD CALLBACK CDBModule_MQUser::DBModule_MQUser_TimeThread(XPVOID lParam)
{
	CDBModule_MQUser* pClass_This = (CDBModule_MQUser*)lParam;

	while (pClass_This->bIsRun)
	{
		int nListCount = 0;
		XENGINE_DBTIMERELEASE** ppSt_DBInfo = NULL;
		pClass_This->DBModule_MQUser_TimeQuery(&ppSt_DBInfo, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			pClass_This->lpCall_TimePublish(ppSt_DBInfo[i], pClass_This->m_lParam);
		}
		BaseLib_Memory_Free((XPPPMEM)&ppSt_DBInfo, nListCount);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}