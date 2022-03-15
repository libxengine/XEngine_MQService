#include "pch.h"
#include "DBModule_DDSMessage.h"
/********************************************************************
//    Created:     2022/02/23  16:13:25
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_DDSMessage\DBModule_DDSMessage.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_DDSMessage
//    File Base:   DBModule_DDSMessage
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     DDS数据库操作
//    History:
*********************************************************************/
CDBModule_DDSMessage::CDBModule_DDSMessage()
{
    xhDBSQL = 0;
}
CDBModule_DDSMessage::~CDBModule_DDSMessage()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DBModule_DDSMessage_Init
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
BOOL CDBModule_DDSMessage::DBModule_DDSMessage_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector)
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
    _tcscpy(pSt_DBConnector->tszDBName, _T("XEngine_DDService"));
    if (!DataBase_MySQL_Connect(&xhDBSQL, pSt_DBConnector, 5, TRUE, lpszStrCharset))
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return FALSE;
    }
    return TRUE;
}
/********************************************************************
函数名称：DBModule_DDSMessage_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_DDSMessage::DBModule_DDSMessage_Destory()
{
    DBModule_IsErrorOccur = FALSE;
    DataBase_MySQL_Close(xhDBSQL);
    return TRUE;
}