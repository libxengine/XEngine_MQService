#pragma once
/********************************************************************
//    Created:     2021/07/01  10:46:24
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ConfigModule\Config_Define.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ConfigModule
//    File Base:   Config_Define
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出的定义
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                        导出的数据结构
//////////////////////////////////////////////////////////////////////////
typedef struct tag_XEngine_ServerConfig
{
    TCHAR tszIPAddr[128];
    TCHAR tszTopic[128];
    BOOL bDeamon;
    int nTCPPort;
    int nHttpPort;
    int nWSPort;
    int nBroadPort;
    struct
    {
        int nMaxClient;
        int nMaxQueue;
        int nIOThread;
        int nTCPThread;
        int nHttpThread;
        int nWSThread;
    }st_XMax;
    struct
    {
        BOOL bHBTime;
        int nDBMonth;
        int nTimeCheck;
        int nTCPTimeOut;
        int nHttpTimeOut;
        int nWSTimeOut;
    }st_XTime;
    struct
    {
        int nMaxSize;
        int nMaxCount;
        int nLogLeave;
    }st_XLog;
    struct
    {
        TCHAR tszSQLAddr[128];
        TCHAR tszSQLUser[128];
        TCHAR tszSQLPass[128];
        TCHAR tszDBName[128];                                                
        int nSQLPort;
    }st_XSql;
    struct
    {
        list<tstring> *pStl_ListStorage;
    }st_XVer;
}XENGINE_SERVERCONFIG;
//////////////////////////////////////////////////////////////////////////
//                        导出函数定义
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD Config_GetLastError(int *pInt_ErrorCode = NULL);
/************************************************************************/
/*                        文件配置读取                                  */
/************************************************************************/
extern "C" BOOL Config_Json_File(LPCTSTR lpszConfigFile,XENGINE_SERVERCONFIG *pSt_ServerConfig);
