#include "pch.h"
#include "DBModule_MQData.h"
/********************************************************************
//    Created:     2022/03/17  09:46:36
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQData\DBModule_MQData.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQData
//    File Base:   DBModule_MQData
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列数据库操作
//    History:
*********************************************************************/
CDBModule_MQData::CDBModule_MQData()
{
    xhDBSQL = 0;
}
CDBModule_MQData::~CDBModule_MQData()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DBModule_MQData_Init
函数功能：初始化数据库管理器
 参数.一：pSt_DBConnector
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：数据MYSQL数据库连接信息
 参数.二：fpCall_TimePublish
  In/Out：In
  类型：回调函数
  可空：N
  意思：定时消息发布回调函数
 参数.三：lParam
  In/Out：In
  类型：无类型指针
  可空：Y
  意思：回调函数自定义参数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQData::DBModule_MQData_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH fpCall_TimePublish, LPVOID lParam /* = NULL */)
{
    DBModule_IsErrorOccur = FALSE;

    if (NULL == pSt_DBConnector)
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
        return FALSE;
    }
#ifdef _WINDOWS
    LPCTSTR lpszStrCharset = _T("gbk");
#else
    LPCTSTR lpszStrCharset = _T("utf8");
#endif
    //连接数据库
    _tcscpy(pSt_DBConnector->tszDBName, _T("XEngine_MQData"));
    if (!DataBase_MySQL_Connect(&xhDBSQL, pSt_DBConnector, 5, TRUE, lpszStrCharset))
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return FALSE;
    }
	bIsRun = TRUE;
	m_lParam = lParam;
	lpCall_TimePublish = fpCall_TimePublish;
	pSTDThread = make_shared<std::thread>(DBModule_MQData_TimeThread, this);
	if (NULL == pSTDThread)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_CREATETHREAD;
		return FALSE;
	}
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQData::DBModule_MQData_Destory()
{
    DBModule_IsErrorOccur = FALSE;
    
	bIsRun = FALSE;
	if (NULL != pSTDThread)
	{
		pSTDThread->join();
	}
	DataBase_MySQL_Close(xhDBSQL);
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_Insert
函数功能：插入消息到队列中
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要插入的数据信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQData::DBModule_MQData_Insert(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo)
{
    DBModule_IsErrorOccur = FALSE;

    if (NULL == pSt_DBInfo)
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
        return FALSE;
    }
    TCHAR tszSQLStatement[10240];
    memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf(tszSQLStatement, _T("INSERT INTO `%s` (tszQueueName,nQueueSerial,nQueueGetTime,tszQueueLeftTime,tszQueuePublishTime,tszQueueData,nDataLen,tszQueueCreateTime) VALUES('%s',%lld,%lld,'%s','%s','%s',%d,now())"), pSt_DBInfo->tszQueueName, pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial, pSt_DBInfo->nQueueGetTime, pSt_DBInfo->tszQueueLeftTime, pSt_DBInfo->tszQueuePublishTime, pSt_DBInfo->tszMsgBuffer, pSt_DBInfo->nMsgLen);
    if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return FALSE;
    }
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_Query
函数功能：查询数据
 参数.一：pSt_DBInfo
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入查询数据,输出查询到的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQData::DBModule_MQData_Query(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_DBInfo)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	//查询
	XHDATA xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf_s(tszSQLStatement, _T("SELECT * FROM `%s` WHERE nQueueSerial = %lld"), pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial);
	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	if (nllLine <= 0)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
		return FALSE;
	}
	TCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
	if (NULL != pptszResult[1])
	{
		_tcscpy(pSt_DBInfo->tszQueueName, pptszResult[1]);
	}
	if (NULL != pptszResult[2])
	{
		pSt_DBInfo->nQueueSerial = _ttoi64(pptszResult[2]);
	}
	if (NULL != pptszResult[3])
	{
		pSt_DBInfo->nQueueGetTime = _ttoi64(pptszResult[3]);
	}
	if (NULL != pptszResult[4])
	{
		_tcscpy(pSt_DBInfo->tszQueueLeftTime, pptszResult[4]);
	}
	if (NULL != pptszResult[5])
	{
		_tcscpy(pSt_DBInfo->tszQueuePublishTime, pptszResult[5]);
	}
	if (NULL != pptszResult[6])
	{
		_tcscpy(pSt_DBInfo->tszMsgBuffer, pptszResult[6]);
	}
	if (NULL != pptszResult[7])
	{
		pSt_DBInfo->nMsgLen = _ttoi(pptszResult[7]);
	}
	if (NULL != pptszResult[8])
	{
		_tcscpy(pSt_DBInfo->tszQueueCreateTime, pptszResult[8]);
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_GetSerial
函数功能：获取序列号
 参数.一：lpszName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要查找的名称
 参数.二：pInt_DBCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出统计的个数
 参数.三：pSt_DBStart
  In/Out：Out
  类型：数据结构指针
  可空：N
  意思：开始的记录
 参数.四：pSt_DBEnd
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：结尾的记录
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQData::DBModule_MQData_GetSerial(LPCTSTR lpszName, __int64x* pInt_DBCount, XENGINE_DBMESSAGEQUEUE* pSt_DBStart, XENGINE_DBMESSAGEQUEUE* pSt_DBEnd)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == lpszName)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	//查询
	XHDATA xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;
	TCHAR** pptszResult;

	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	//////////////////////////////////////////////////////////////////////////第一条
	if (NULL != pSt_DBStart)
	{
		_stprintf(tszSQLStatement, _T("SELECT * FROM `%s` ORDER BY ID ASC LIMIT 1"), lpszName);
		if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
		{
			DBModule_IsErrorOccur = TRUE;
			DBModule_dwErrorCode = DataBase_GetLastError();
			return FALSE;
		}
		if (nllLine <= 0)
		{
			DBModule_IsErrorOccur = TRUE;
			DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
			return FALSE;
		}
		pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
		if (NULL != pptszResult[1])
		{
			_tcscpy(pSt_DBStart->tszQueueName, pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			pSt_DBStart->nQueueSerial = _ttoi64(pptszResult[2]);
		}
		if (NULL != pptszResult[3])
		{
			pSt_DBStart->nQueueGetTime = _ttoi64(pptszResult[3]);
		}
		if (NULL != pptszResult[4])
		{
			_tcscpy(pSt_DBStart->tszQueueLeftTime, pptszResult[4]);
		}
		if (NULL != pptszResult[5])
		{
			_tcscpy(pSt_DBStart->tszQueuePublishTime, pptszResult[5]);
		}
		if (NULL != pptszResult[6])
		{
			_tcscpy(pSt_DBStart->tszMsgBuffer, pptszResult[6]);
		}
		if (NULL != pptszResult[7])
		{
			pSt_DBStart->nMsgLen = _ttoi(pptszResult[7]);
		}
		if (NULL != pptszResult[8])
		{
			_tcscpy(pSt_DBStart->tszQueueCreateTime, pptszResult[8]);
		}
		DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	}
	//////////////////////////////////////////////////////////////////////////最后一条
	if (NULL != pSt_DBEnd)
	{
		nllLine = 0;
		nllRow = 0;
		memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
		_stprintf(tszSQLStatement, _T("SELECT * FROM `%s` ORDER BY ID DESC LIMIT 1"), lpszName);
		if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
		{
			DBModule_IsErrorOccur = TRUE;
			DBModule_dwErrorCode = DataBase_GetLastError();
			return FALSE;
		}
		if (nllLine <= 0)
		{
			DBModule_IsErrorOccur = TRUE;
			DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
			return FALSE;
		}
		pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
		if (NULL != pptszResult[1])
		{
			_tcscpy(pSt_DBEnd->tszQueueName, pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			pSt_DBEnd->nQueueSerial = _ttoi64(pptszResult[2]);
		}
		if (NULL != pptszResult[3])
		{
			pSt_DBEnd->nQueueGetTime = _ttoi64(pptszResult[3]);
		}
		if (NULL != pptszResult[4])
		{
			_tcscpy(pSt_DBEnd->tszQueueLeftTime, pptszResult[4]);
		}
		if (NULL != pptszResult[5])
		{
			_tcscpy(pSt_DBEnd->tszQueuePublishTime, pptszResult[5]);
		}
		if (NULL != pptszResult[6])
		{
			_tcscpy(pSt_DBEnd->tszMsgBuffer, pptszResult[6]);
		}
		if (NULL != pptszResult[7])
		{
			pSt_DBEnd->nMsgLen = _ttoi(pptszResult[7]);
		}
		if (NULL != pptszResult[8])
		{
			_tcscpy(pSt_DBEnd->tszQueueCreateTime, pptszResult[8]);
		}
		DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	}
	//////////////////////////////////////////////////////////////////////////统计
	if (NULL != pInt_DBCount)
	{
		nllLine = 0;
		nllRow = 0;
		memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
		_stprintf(tszSQLStatement, _T("SELECT COUNT(*) FROM `%s`"), lpszName);
		if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
		{
			DBModule_IsErrorOccur = TRUE;
			DBModule_dwErrorCode = DataBase_GetLastError();
			return FALSE;
		}
		if (nllLine <= 0)
		{
			DBModule_IsErrorOccur = TRUE;
			DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
			return FALSE;
		}
		pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
		if (NULL != pptszResult[0])
		{
			*pInt_DBCount = _ttoi64(pptszResult[0]);
		}
		DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	}
	
	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_CreateTable
函数功能：创建表
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要创建的表名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQData::DBModule_MQData_CreateTable(LPCTSTR lpszQueueName)
{
    DBModule_IsErrorOccur = FALSE;

    TCHAR tszSQLQuery[2048];
    memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

    _stprintf_s(tszSQLQuery, _T("CREATE TABLE IF NOT EXISTS `%s` ("
        "`ID` int NOT NULL AUTO_INCREMENT,"
        "`tszQueueName` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '所属队列',"
        "`nQueueSerial` bigint NOT NULL COMMENT '消息序列',"
        "`nQueueGetTime` bigint NOT NULL COMMENT '获取次数',"
        "`tszQueueLeftTime` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '过期时间',"
        "`tszQueuePublishTime` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '发布时间',"
        "`tszQueueData` varchar(8192) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '保存数据',"
		"`nDataLen` int NOT NULL COMMENT '数据大小',"
        "`tszQueueCreateTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '插入时间',"
        "PRIMARY KEY (`ID`) USING BTREE"
        ") ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;"
    ), lpszQueueName);

    if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return FALSE;
    }
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_DeleteTable
函数功能：删除表
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQData::DBModule_MQData_DeleteTable(LPCTSTR lpszQueueName)
{
	DBModule_IsErrorOccur = FALSE;

	TCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

    _stprintf_s(tszSQLQuery, _T("DROP TABLE IF EXISTS `%s`"), lpszQueueName);

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_TimeInsert
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
BOOL CDBModule_MQData::DBModule_MQData_TimeInsert(XENGINE_DBTIMERELEASE* pSt_DBInfo)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_DBInfo)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	TCHAR tszSQLStatement[10240];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf(tszSQLStatement, _T("INSERT INTO `XEngine_TimeRelease` (tszQueueName,nIDMsg,nIDTime) VALUES('%s',%lld,%lld)"), pSt_DBInfo->tszQueueName, pSt_DBInfo->nIDMsg, pSt_DBInfo->nIDTime);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_TimeQuery
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
BOOL CDBModule_MQData::DBModule_MQData_TimeQuery(XENGINE_DBTIMERELEASE*** pppSt_DBInfo, int* pInt_ListCount)
{
	DBModule_IsErrorOccur = FALSE;

	//查询
	XHDATA xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;
	TCHAR tszSQLStatement[1024];

	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	_stprintf(tszSQLStatement, _T("SELECT * FROM `XEngine_TimeRelease` WHERE nIDTime >= %lld"), time(NULL));

	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	*pInt_ListCount = (int)nllLine;
	BaseLib_OperatorMemory_Malloc((XPPPMEM)pppSt_DBInfo, *pInt_ListCount, sizeof(XENGINE_DBTIMERELEASE));
	for (int i = 0; i < nllLine; i++)
	{
		TCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
		if (NULL != pptszResult[0])
		{
			_tcscpy((*pppSt_DBInfo)[i]->tszQueueName, pptszResult[0]);
		}
		if (NULL != pptszResult[1])
		{
			(*pppSt_DBInfo)[i]->nIDMsg = _ttoi64(pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			(*pppSt_DBInfo)[i]->nIDTime = _ttoi64(pptszResult[2]);
		}
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);

	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQData_TimeDelete
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
BOOL CDBModule_MQData::DBModule_MQData_TimeDelete(XENGINE_DBTIMERELEASE* pSt_DBInfo)
{
	DBModule_IsErrorOccur = FALSE;

	TCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	_stprintf_s(tszSQLQuery, _T("DELETE FORM `XEngine_TimeRelease` WHERE nIDMsg = %lld"), pSt_DBInfo->nIDMsg);

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//                         线程函数
//////////////////////////////////////////////////////////////////////////
XHTHREAD CALLBACK CDBModule_MQData::DBModule_MQData_TimeThread(LPVOID lParam)
{
	CDBModule_MQData* pClass_This = (CDBModule_MQData*)lParam;

	while (pClass_This->bIsRun)
	{
		int nListCount = 0;
		XENGINE_DBTIMERELEASE** ppSt_DBInfo;
		pClass_This->DBModule_MQData_TimeQuery(&ppSt_DBInfo, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			pClass_This->lpCall_TimePublish(ppSt_DBInfo[i]->tszQueueName, ppSt_DBInfo[i]->nIDMsg, ppSt_DBInfo[i]->nIDTime, pClass_This->m_lParam);
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}