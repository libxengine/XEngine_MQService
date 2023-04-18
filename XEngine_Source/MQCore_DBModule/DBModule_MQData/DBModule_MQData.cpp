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
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQData::DBModule_MQData_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector)
{
    DBModule_IsErrorOccur = false;

    if (NULL == pSt_DBConnector)
    {
        DBModule_IsErrorOccur = true;
        DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
        return false;
    }
#ifdef _WINDOWS
    LPCXSTR lpszStrCharset = _T("gbk");
#else
    LPCXSTR lpszStrCharset = _T("utf8");
#endif
    //连接数据库
    _tcscpy(pSt_DBConnector->tszDBName, _T("XEngine_MQData"));
    if (!DataBase_MySQL_Connect(&xhDBSQL, pSt_DBConnector, 5, true, lpszStrCharset))
    {
        DBModule_IsErrorOccur = true;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return false;
    }
    return true;
}
/********************************************************************
函数名称：DBModule_MQData_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQData::DBModule_MQData_Destory()
{
    DBModule_IsErrorOccur = false;
    
	DataBase_MySQL_Close(xhDBSQL);
    return true;
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
bool CDBModule_MQData::DBModule_MQData_Insert(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo)
{
    DBModule_IsErrorOccur = false;

    if (NULL == pSt_DBInfo)
    {
        DBModule_IsErrorOccur = true;
        DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
        return false;
    }
    TCHAR tszSQLStatement[10240];
    memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf(tszSQLStatement, _T("INSERT INTO `%s` (tszUserName,tszQueueName,nQueueSerial,nQueueGetTime,tszQueueLeftTime,tszQueuePublishTime,tszQueueData,nDataLen,nDataType,tszQueueCreateTime) VALUES('%s','%s',%lld,%lld,'%s','%s','%s',%d,%d,now())"), pSt_DBInfo->tszQueueName, pSt_DBInfo->tszUserName, pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial, pSt_DBInfo->nQueueGetTime, pSt_DBInfo->tszQueueLeftTime, pSt_DBInfo->tszQueuePublishTime, pSt_DBInfo->tszMsgBuffer, pSt_DBInfo->nMsgLen, pSt_DBInfo->byMsgType);
    if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
    {
        DBModule_IsErrorOccur = true;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return false;
    }
    return true;
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
bool CDBModule_MQData::DBModule_MQData_Query(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_DBInfo)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	//查询
	XNETHANDLE xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf_s(tszSQLStatement, _T("SELECT * FROM `%s` WHERE nQueueSerial = %lld"), pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial);
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
	TCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);

	if (NULL != pptszResult[1])
	{
		_tcscpy(pSt_DBInfo->tszUserName, pptszResult[1]);
	}
	if (NULL != pptszResult[2])
	{
		_tcscpy(pSt_DBInfo->tszQueueName, pptszResult[2]);
	}
	if (NULL != pptszResult[3])
	{
		pSt_DBInfo->nQueueSerial = _ttoi64(pptszResult[3]);
	}
	if (NULL != pptszResult[4])
	{
		pSt_DBInfo->nQueueGetTime = _ttoi64(pptszResult[4]);
	}
	if (NULL != pptszResult[5])
	{
		_tcscpy(pSt_DBInfo->tszQueueLeftTime, pptszResult[5]);
	}
	if (NULL != pptszResult[6])
	{
		_tcscpy(pSt_DBInfo->tszQueuePublishTime, pptszResult[6]);
	}
	if (NULL != pptszResult[7])
	{
		_tcscpy(pSt_DBInfo->tszMsgBuffer, pptszResult[7]);
	}
	if (NULL != pptszResult[8])
	{
		pSt_DBInfo->nMsgLen = _ttoi(pptszResult[8]);
	}
	if (NULL != pptszResult[9])
	{
		pSt_DBInfo->byMsgType = _ttoi(pptszResult[9]);
	}
	if (NULL != pptszResult[10])
	{
		_tcscpy(pSt_DBInfo->tszQueueCreateTime, pptszResult[10]);
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return true;
}
/********************************************************************
函数名称：DBModule_MQData_Modify
函数功能：数据修改
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要操作的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQData::DBModule_MQData_Modify(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pSt_DBInfo)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	TCHAR tszSQLStatement[10240];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf(tszSQLStatement, _T("UPDATE `%s` SET nQueueGetTime = %lld,tszQueueLeftTime = '%s',tszQueuePublishTime = '%s',tszQueueData = '%s',nDataLen = %d,nDataType = %d WHERE tszUserName = '%s' AND tszQueueName = '%s' AND nQueueSerial = %lld"), pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueGetTime, pSt_DBInfo->tszQueueLeftTime, pSt_DBInfo->tszQueuePublishTime, pSt_DBInfo->tszMsgBuffer, pSt_DBInfo->nMsgLen, pSt_DBInfo->byMsgType, pSt_DBInfo->tszUserName, pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial);

	__int64u nRows = 0;
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement, &nRows))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	if (nRows <= 0)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_NOTFOUND;
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQData_List
函数功能：枚举指定主题序列号后的数据
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要处理的主题
 参数.二：nSerial
  In/Out：In
  类型：整数型
  可空：N
  意思：输入主题序列号
 参数.三：pppSt_DBMessage
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出数据队列信息
 参数.四：pInt_ListCount
  In/Out：Out
  类型：整数型
  可空：N
  意思：输出数据队列大小
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQData::DBModule_MQData_List(LPCXSTR lpszQueueName, __int64x nSerial, XENGINE_DBMESSAGEQUEUE*** pppSt_DBMessage, int* pInt_ListCount)
{
	DBModule_IsErrorOccur = false;

	if (NULL == lpszQueueName)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	//查询
	XNETHANDLE xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	//名称为,消息名为必填
	_stprintf_s(tszSQLStatement, _T("SELECT * FROM `%s` WHERE nQueueSerial > %lld"), lpszQueueName, nSerial);
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
	BaseLib_OperatorMemory_Malloc((XPPPMEM)pppSt_DBMessage, (int)nllLine, sizeof(XENGINE_DBMESSAGEQUEUE));
	for (__int64u i = 0; i < nllLine; i++)
	{
		TCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);

		if (NULL != pptszResult[1])
		{
			_tcscpy((*pppSt_DBMessage)[i]->tszUserName, pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			_tcscpy((*pppSt_DBMessage)[i]->tszQueueName, pptszResult[2]);
		}
		if (NULL != pptszResult[3])
		{
			(*pppSt_DBMessage)[i]->nQueueSerial = _ttoi64(pptszResult[3]);
		}
		if (NULL != pptszResult[4])
		{
			(*pppSt_DBMessage)[i]->nQueueGetTime = _ttoi64(pptszResult[4]);
		}
		if (NULL != pptszResult[5])
		{
			_tcscpy((*pppSt_DBMessage)[i]->tszQueueLeftTime, pptszResult[5]);
		}
		if (NULL != pptszResult[6])
		{
			_tcscpy((*pppSt_DBMessage)[i]->tszQueuePublishTime, pptszResult[6]);
		}
		if (NULL != pptszResult[7])
		{
			_tcscpy((*pppSt_DBMessage)[i]->tszMsgBuffer, pptszResult[7]);
		}
		if (NULL != pptszResult[8])
		{
			(*pppSt_DBMessage)[i]->nMsgLen = _ttoi(pptszResult[8]);
		}
		if (NULL != pptszResult[9])
		{
			(*pppSt_DBMessage)[i]->byMsgType = _ttoi(pptszResult[9]);
		}
		if (NULL != pptszResult[10])
		{
			_tcscpy((*pppSt_DBMessage)[i]->tszQueueCreateTime, pptszResult[10]);
		}
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return true;
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
bool CDBModule_MQData::DBModule_MQData_GetSerial(LPCXSTR lpszName, __int64x* pInt_DBCount, XENGINE_DBMESSAGEQUEUE* pSt_DBStart, XENGINE_DBMESSAGEQUEUE* pSt_DBEnd)
{
	DBModule_IsErrorOccur = false;

	if (NULL == lpszName)
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return false;
	}
	//查询
	XNETHANDLE xhTable = 0;
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
		pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);

		if (NULL != pptszResult[1])
		{
			_tcscpy(pSt_DBStart->tszUserName, pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			_tcscpy(pSt_DBStart->tszQueueName, pptszResult[2]);
		}
		if (NULL != pptszResult[3])
		{
			pSt_DBStart->nQueueSerial = _ttoi64(pptszResult[3]);
		}
		if (NULL != pptszResult[4])
		{
			pSt_DBStart->nQueueGetTime = _ttoi64(pptszResult[4]);
		}
		if (NULL != pptszResult[5])
		{
			_tcscpy(pSt_DBStart->tszQueueLeftTime, pptszResult[5]);
		}
		if (NULL != pptszResult[6])
		{
			_tcscpy(pSt_DBStart->tszQueuePublishTime, pptszResult[6]);
		}
		if (NULL != pptszResult[7])
		{
			_tcscpy(pSt_DBStart->tszMsgBuffer, pptszResult[7]);
		}
		if (NULL != pptszResult[8])
		{
			pSt_DBStart->nMsgLen = _ttoi(pptszResult[8]);
		}
		if (NULL != pptszResult[9])
		{
			pSt_DBStart->byMsgType = _ttoi(pptszResult[9]);
		}
		if (NULL != pptszResult[10])
		{
			_tcscpy(pSt_DBStart->tszQueueCreateTime, pptszResult[10]);
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
		pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
		
		if (NULL != pptszResult[1])
		{
			_tcscpy(pSt_DBEnd->tszUserName, pptszResult[1]);
		}
		if (NULL != pptszResult[2])
		{
			_tcscpy(pSt_DBEnd->tszQueueName, pptszResult[2]);
		}
		if (NULL != pptszResult[3])
		{
			pSt_DBEnd->nQueueSerial = _ttoi64(pptszResult[3]);
		}
		if (NULL != pptszResult[4])
		{
			pSt_DBEnd->nQueueGetTime = _ttoi64(pptszResult[4]);
		}
		if (NULL != pptszResult[5])
		{
			_tcscpy(pSt_DBEnd->tszQueueLeftTime, pptszResult[5]);
		}
		if (NULL != pptszResult[6])
		{
			_tcscpy(pSt_DBEnd->tszQueuePublishTime, pptszResult[6]);
		}
		if (NULL != pptszResult[7])
		{
			_tcscpy(pSt_DBEnd->tszMsgBuffer, pptszResult[7]);
		}
		if (NULL != pptszResult[8])
		{
			pSt_DBEnd->nMsgLen = _ttoi(pptszResult[8]);
		}
		if (NULL != pptszResult[9])
		{
			pSt_DBEnd->byMsgType = _ttoi(pptszResult[9]);
		}
		if (NULL != pptszResult[10])
		{
			_tcscpy(pSt_DBEnd->tszQueueCreateTime, pptszResult[10]);
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
		pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
		if (NULL != pptszResult[0])
		{
			*pInt_DBCount = _ttoi64(pptszResult[0]);
		}
		DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	}
	
	return true;
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
bool CDBModule_MQData::DBModule_MQData_CreateTable(LPCXSTR lpszQueueName)
{
    DBModule_IsErrorOccur = false;

    TCHAR tszSQLQuery[2048];
    memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

    _stprintf_s(tszSQLQuery, _T("CREATE TABLE IF NOT EXISTS `%s` ("
        "`ID` int NOT NULL AUTO_INCREMENT,"
		"`tszUserName` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '谁发布的消息',"
        "`tszQueueName` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '所属队列',"
        "`nQueueSerial` bigint NOT NULL COMMENT '消息序列',"
        "`nQueueGetTime` bigint NOT NULL COMMENT '获取次数',"
        "`tszQueueLeftTime` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '过期时间',"
        "`tszQueuePublishTime` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '发布时间',"
        "`tszQueueData` varchar(8192) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '保存数据',"
		"`nDataLen` int NOT NULL COMMENT '数据大小',"
		"`nDataType` tinyint NOT NULL COMMENT '数据类型',"
        "`tszQueueCreateTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '插入时间',"
        "PRIMARY KEY (`ID`) USING BTREE"
        ") ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;"
    ), lpszQueueName);

    if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
    {
        DBModule_IsErrorOccur = true;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return false;
    }
    return true;
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
bool CDBModule_MQData::DBModule_MQData_DeleteTable(LPCXSTR lpszQueueName)
{
	DBModule_IsErrorOccur = false;

	TCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

    _stprintf_s(tszSQLQuery, _T("DROP TABLE IF EXISTS `%s`"), lpszQueueName);

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	return true;
}
/********************************************************************
函数名称：DBModule_MQData_ModifyTable
函数功能：修改表名称
 参数.一：lpszSrcTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要修改的表名
 参数.二：lpszDstTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入修改到的名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQData::DBModule_MQData_ModifyTable(LPCXSTR lpszSrcTable, LPCXSTR lpszDstTable)
{
	DBModule_IsErrorOccur = false;

	TCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	_stprintf(tszSQLQuery, _T("UPDATE `%s` SET tszQueueName = '%s' WHERE tszQueueName = '%s'"), lpszSrcTable, lpszDstTable, lpszSrcTable);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));
	_stprintf_s(tszSQLQuery, _T("RENAME TABLE `%s` TO `%s`"), lpszSrcTable, lpszDstTable);

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	
	return true;
}