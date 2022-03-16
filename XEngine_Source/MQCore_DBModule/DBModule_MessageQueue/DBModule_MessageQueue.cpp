#include "pch.h"
#include "DBModule_MessageQueue.h"
/********************************************************************
//    Created:     2022/03/15  13:08:11
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MessageQueue\DBModule_MessageQueue.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MessageQueue
//    File Base:   DBModule_MessageQueue
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列数据库操作
//    History:
*********************************************************************/
CDBModule_MessageQueue::CDBModule_MessageQueue()
{
    xhDBSQL = 0;
}
CDBModule_MessageQueue::~CDBModule_MessageQueue()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DBModule_MessageQueue_Init
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
BOOL CDBModule_MessageQueue::DBModule_MessageQueue_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector)
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
    _tcscpy(pSt_DBConnector->tszDBName, _T("XEngine_MessageQueue"));
    if (!DataBase_MySQL_Connect(&xhDBSQL, pSt_DBConnector, 5, TRUE, lpszStrCharset))
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return FALSE;
    }
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MessageQueue_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MessageQueue::DBModule_MessageQueue_Destory()
{
    DBModule_IsErrorOccur = FALSE;
    DataBase_MySQL_Close(xhDBSQL);
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MessageQueue_Insert
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
BOOL CDBModule_MessageQueue::DBModule_MessageQueue_Insert(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo)
{
    DBModule_IsErrorOccur = FALSE;

    if (NULL == pSt_DBInfo)
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
        return FALSE;
    }
    TCHAR tszSQLStatement[2048];
    memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

    _stprintf(tszSQLStatement, _T("INSERT INTO `%s` (tszQueueName,nQueueSerial,nQueueGetTime,tszQueueLeftTime,tszQueuePublishTime,tszQueueData,tszQueueCreateTime) VALUES('%s',%lld,%lld,'%s','%s','%s',now())"), pSt_DBInfo->tszQueueName, pSt_DBInfo->tszQueueName, pSt_DBInfo->nQueueSerial, pSt_DBInfo->nQueueGetTime, pSt_DBInfo->tszQueueLeftTime, pSt_DBInfo->tszMsgBuffer, pSt_DBInfo->tszQueuePublishTime);
    if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return FALSE;
    }
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MessageQueue_CreateTable
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
BOOL CDBModule_MessageQueue::DBModule_MessageQueue_CreateTable(LPCTSTR lpszQueueName)
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