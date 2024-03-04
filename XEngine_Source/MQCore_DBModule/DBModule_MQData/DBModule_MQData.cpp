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
    //连接数据库
    _tcsxcpy(pSt_DBConnector->tszDBName, _X("XEngine_MQData"));
    if (!DataBase_MySQL_Connect(&xhDBSQL, pSt_DBConnector))
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
	XCHAR tszSQLStatement[10240];
	XCHAR tszSQLCoder[8192];

    memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	memset(tszSQLCoder, '\0', sizeof(tszSQLCoder));

	DataBase_MySQL_Coder(xhDBSQL, pSt_DBInfo->tszMsgBuffer, tszSQLCoder, &pSt_DBInfo->nMsgLen);
	__int64u nRet = _xstprintf(tszSQLStatement, _X("INSERT INTO `%s` (tszUserName,tszUserBelong,tszQueueName,nQueueSerial,tszQueueLeftTime,tszQueuePublishTime,tszQueueData,nDataType,nDataAttr,tszQueueCreateTime) VALUES('%s','%s','%s',%lld,'%s','%s','"), pSt_DBInfo->tszQueueName, pSt_DBInfo->tszUserName, pSt_DBInfo->tszUserBelong, pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial, pSt_DBInfo->tszQueueLeftTime, pSt_DBInfo->tszQueuePublishTime);
	memcpy(tszSQLStatement + nRet, tszSQLCoder, pSt_DBInfo->nMsgLen);
	nRet += pSt_DBInfo->nMsgLen;

	memset(tszSQLCoder, '\0', sizeof(tszSQLCoder));
	int nLen = _xstprintf(tszSQLCoder, _X("',%d,%d,now())"), pSt_DBInfo->byMsgType, pSt_DBInfo->nMsgAttr);
	memcpy(tszSQLStatement + nRet, tszSQLCoder, nLen);
	nRet += nLen;

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement, &nRet))
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

	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_xstprintf(tszSQLStatement, _X("SELECT * FROM `%s` WHERE nQueueSerial = %lld"), pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial);
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
	XLONG* pInt_Length = DataBase_MySQL_GetLength(xhDBSQL, xhTable);

	int nPos = 1;
	if (NULL != pptszResult[nPos])
	{
		_tcsxcpy(pSt_DBInfo->tszUserName, pptszResult[nPos]);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		_tcsxcpy(pSt_DBInfo->tszUserBelong, pptszResult[nPos]);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		_tcsxcpy(pSt_DBInfo->tszQueueName, pptszResult[nPos]);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		pSt_DBInfo->nQueueSerial = _ttxoll(pptszResult[nPos]);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		_tcsxcpy(pSt_DBInfo->tszQueueLeftTime, pptszResult[nPos]);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		_tcsxcpy(pSt_DBInfo->tszQueuePublishTime, pptszResult[nPos]);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		pSt_DBInfo->nMsgLen = pInt_Length[nPos];
		memcpy(pSt_DBInfo->tszMsgBuffer, pptszResult[nPos], pSt_DBInfo->nMsgLen);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		pSt_DBInfo->byMsgType = _ttxoi(pptszResult[nPos]);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		pSt_DBInfo->nMsgAttr = _ttxoi(pptszResult[nPos]);
	}
	nPos++;
	if (NULL != pptszResult[nPos])
	{
		_tcsxcpy(pSt_DBInfo->tszQueueCreateTime, pptszResult[nPos]);
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
	XCHAR tszSQLStatement[10240];
	XCHAR tszSQLCoder[8192];

	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	memset(tszSQLCoder, '\0', sizeof(tszSQLCoder));

	DataBase_MySQL_Coder(xhDBSQL, pSt_DBInfo->tszMsgBuffer, tszSQLCoder, &pSt_DBInfo->nMsgLen);
	__int64u nRet = _xstprintf(tszSQLStatement, _X("UPDATE `%s` SET tszUserBelong = '%s',tszQueueLeftTime = '%s',tszQueuePublishTime = '%s',tszQueueData = '"), pSt_DBInfo->tszQueueName, pSt_DBInfo->tszUserBelong, pSt_DBInfo->tszQueueLeftTime, pSt_DBInfo->tszQueuePublishTime);

	memcpy(tszSQLStatement + nRet, tszSQLCoder, pSt_DBInfo->nMsgLen);
	nRet += pSt_DBInfo->nMsgLen;

	memset(tszSQLCoder, '\0', sizeof(tszSQLCoder));
	int nLen = _xstprintf(tszSQLCoder, _X("',nDataType = %d,nDataAttr = %d WHERE tszUserName = '%s' AND tszQueueName = '%s' AND nQueueSerial = %lld"), pSt_DBInfo->byMsgType, pSt_DBInfo->nMsgAttr, pSt_DBInfo->tszUserName, pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial);
	memcpy(tszSQLStatement + nRet, tszSQLCoder, nLen);
	nRet += nLen;

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement, &nRet))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	if (nRet <= 0)
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

	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	//名称为,消息名为必填
	_xstprintf(tszSQLStatement, _X("SELECT * FROM `%s` WHERE nQueueSerial >= %lld"), lpszQueueName, nSerial);
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
		XCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
		XLONG* pInt_Length = DataBase_MySQL_GetLength(xhDBSQL, xhTable);

		int nPos = 1;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy((*pppSt_DBMessage)[i]->tszUserName, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy((*pppSt_DBMessage)[i]->tszQueueName, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			(*pppSt_DBMessage)[i]->nQueueSerial = _ttxoll(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy((*pppSt_DBMessage)[i]->tszQueueLeftTime, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy((*pppSt_DBMessage)[i]->tszQueuePublishTime, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			(*pppSt_DBMessage)[i]->nMsgLen = pInt_Length[nPos];
			memcpy((*pppSt_DBMessage)[i]->tszMsgBuffer, pptszResult[nPos], (*pppSt_DBMessage)[i]->nMsgLen);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			(*pppSt_DBMessage)[i]->byMsgType = _ttxoi(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			(*pppSt_DBMessage)[i]->nMsgAttr = _ttxoi(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy((*pppSt_DBMessage)[i]->tszQueueCreateTime, pptszResult[nPos]);
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
	XCHAR** pptszResult;

	XCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	//////////////////////////////////////////////////////////////////////////第一条
	if (NULL != pSt_DBStart)
	{
		_xstprintf(tszSQLStatement, _X("SELECT * FROM `%s` ORDER BY ID ASC LIMIT 1"), lpszName);
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
		XLONG* pInt_Length = DataBase_MySQL_GetLength(xhDBSQL, xhTable);

		int nPos = 1;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBStart->tszUserName, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBStart->tszUserBelong, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBStart->tszQueueName, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			pSt_DBStart->nQueueSerial = _ttxoll(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBStart->tszQueueLeftTime, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBStart->tszQueuePublishTime, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			pSt_DBStart->nMsgLen = pInt_Length[nPos];
			memcpy(pSt_DBStart->tszMsgBuffer, pptszResult[nPos], pSt_DBStart->nMsgLen);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			pSt_DBStart->byMsgType = _ttxoi(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			pSt_DBStart->nMsgAttr = _ttxoi(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBStart->tszQueueCreateTime, pptszResult[nPos]);
		}
		DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	}
	//////////////////////////////////////////////////////////////////////////最后一条
	if (NULL != pSt_DBEnd)
	{
		nllLine = 0;
		nllRow = 0;
		memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
		_xstprintf(tszSQLStatement, _X("SELECT * FROM `%s` ORDER BY ID DESC LIMIT 1"), lpszName);
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
		XLONG* pInt_Length = DataBase_MySQL_GetLength(xhDBSQL, xhTable);
		
		int nPos = 1;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBEnd->tszUserName, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBEnd->tszUserBelong, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBEnd->tszQueueName, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			pSt_DBEnd->nQueueSerial = _ttxoll(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBEnd->tszQueueLeftTime, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBEnd->tszQueuePublishTime, pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			pSt_DBEnd->nMsgLen = pInt_Length[nPos];
			memcpy(pSt_DBEnd->tszMsgBuffer, pptszResult[nPos], pSt_DBEnd->nMsgLen);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			pSt_DBEnd->byMsgType = _ttxoi(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			pSt_DBEnd->nMsgAttr = _ttxoi(pptszResult[nPos]);
		}
		nPos++;
		if (NULL != pptszResult[nPos])
		{
			_tcsxcpy(pSt_DBEnd->tszQueueCreateTime, pptszResult[nPos]);
		}
		DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	}
	//////////////////////////////////////////////////////////////////////////统计
	if (NULL != pInt_DBCount)
	{
		nllLine = 0;
		nllRow = 0;
		memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
		_xstprintf(tszSQLStatement, _X("SELECT COUNT(*) FROM `%s`"), lpszName);
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
			*pInt_DBCount = _ttxoll(pptszResult[0]);
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

    XCHAR tszSQLQuery[2048];
	XCHAR tszUTFQuery[2048];

    memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));
	memset(tszUTFQuery, '\0', sizeof(tszUTFQuery));

    _xstprintf(tszSQLQuery, _X("CREATE TABLE IF NOT EXISTS `%s` ("
        "`ID` int NOT NULL AUTO_INCREMENT,"
		"`tszUserName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '谁发布的消息',"
		"`tszUserBelong` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '所属用户',"
        "`tszQueueName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '所属队列',"
        "`nQueueSerial` bigint NOT NULL COMMENT '消息序列',"
        "`tszQueueLeftTime` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '过期时间',"
        "`tszQueuePublishTime` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '发布时间',"
        "`tszQueueData` longblob NOT NULL COMMENT '保存数据',"
		"`nDataType` tinyint NOT NULL COMMENT '数据类型',"
		"`nDataAttr` tinyint NULL DEFAULT NULL COMMENT '消息属性',"
        "`tszQueueCreateTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '插入时间',"
        "PRIMARY KEY (`ID`) USING BTREE"
        ") ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;"
    ), lpszQueueName);

#ifdef _MSC_BUILD
	int nUTFLen = 0;
	BaseLib_OperatorCharset_AnsiToUTF(tszSQLQuery, tszUTFQuery, &nUTFLen);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszUTFQuery))
#else
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
#endif
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

	XCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

    _xstprintf(tszSQLQuery, _X("DROP TABLE IF EXISTS `%s`"), lpszQueueName);

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

	XCHAR tszSQLQuery[2048];
	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));

	_xstprintf(tszSQLQuery, _X("UPDATE `%s` SET tszQueueName = '%s' WHERE tszQueueName = '%s'"), lpszSrcTable, lpszDstTable, lpszSrcTable);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}

	memset(tszSQLQuery, '\0', sizeof(tszSQLQuery));
	_xstprintf(tszSQLQuery, _X("RENAME TABLE `%s` TO `%s`"), lpszSrcTable, lpszDstTable);

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLQuery))
	{
		DBModule_IsErrorOccur = true;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return false;
	}
	
	return true;
}
/********************************************************************
函数名称：DBModule_MQData_ShowTable
函数功能：获取所有表名
 参数.一：pppszTableName
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出表名列表
 参数.二：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出列表个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQData::DBModule_MQData_ShowTable(XCHAR*** pppszTableName, int* pInt_ListCount)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pInt_ListCount)
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
	_xstprintf(tszSQLStatement, _X("SHOW TABLES"));
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
	BaseLib_OperatorMemory_Malloc((XPPPMEM)pppszTableName, (int)nllLine, sizeof(XENGINE_DBMESSAGEQUEUE));
	for (__int64u i = 0; i < nllLine; i++)
	{
		XCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);

		if (NULL != pptszResult[0])
		{
			_tcsxcpy((*pppszTableName)[i], pptszResult[0]);
		}
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return true;
}
/********************************************************************
函数名称：DBModule_MQData_GetLeftCount
函数功能：获取剩余个数
 参数.一：lpszTableName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入表名称
 参数.二：nSerial
  In/Out：In
  类型：整数型
  可空：N
  意思：输入开始的序列号
参数.三：pInt_Count
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出统计信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CDBModule_MQData::DBModule_MQData_GetLeftCount(LPCXSTR lpszTableName, int nSerial, int* pInt_Count)
{
	DBModule_IsErrorOccur = false;

	if (NULL == pInt_Count)
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

	_xstprintf(tszSQLStatement, _X("SELECT COUNT(*) FROM %s WHERE serial > %d"), lpszTableName, nSerial);
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

	if (NULL != pptszResult[0])
	{
		*pInt_Count = _ttxoi(pptszResult[0]);
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return true;
}