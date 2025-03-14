﻿#pragma once
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
typedef struct 
{
    XCHAR tszIPAddr[128];
    XCHAR tszTopic[128];
    bool bDeamon;
    int nTCPPort;
    int nHttpPort;
    int nWSPort;
    int nMQTTPort;
    struct
    {
        int nMaxClient;
        int nMaxQueue;
        int nIOThread;
        int nTCPThread;
        int nHttpThread;
        int nWSThread;
        int nMQTTThread;
    }st_XMax;
    struct
    {
        XCHAR tszLOGFile[MAX_PATH];
        int nMaxSize;
        int nMaxCount;
        int nLogLeave;
        int nLogType;
    }st_XLog;
    struct
    {
        XCHAR tszSQLAddr[128];
        XCHAR tszSQLUser[128];
        XCHAR tszSQLPass[128];
        XCHAR tszDBName[128];                                                
        int nSQLPort;
    }st_XSql;
    struct  
    {
        XCHAR tszPassRegister[MAX_PATH];
        XCHAR tszPassUNReg[MAX_PATH];
        XCHAR tszPassLogin[MAX_PATH];
        XCHAR tszPassLogout[MAX_PATH];
        int nTimeout;
    }st_XPass;
    struct  
    {
        bool bDataQueryEnable;
        bool bDataInsertEnable;
        bool bUserQueryEnable;
        int nTimeLast;
        int nTimeCount;
    }st_XMemory;
    struct  
    {
		bool bHTTPAuth;
		XCHAR tszHTTPPass[MAX_PATH];
		XCHAR tszToken[MAX_PATH];
    }st_XAuthorize;
	struct
	{
		bool bEnable;
		XCHAR tszAPIUrl[MAX_PATH];
		XCHAR tszServiceName[128];
	}st_XReport;
    struct
    {
        list<xstring> *pStl_ListStorage;
    }st_XVer;
}XENGINE_SERVERCONFIG;
typedef struct
{
    struct  
    {
        struct  
        {
            bool bPubClear;
        }st_UserTime;
    }st_MQUser;
    struct  
    {
        int nDBMonth;
        bool bCommSub;
    }st_MQData;
}MESSAGEQUEUE_DBCONFIG;
//////////////////////////////////////////////////////////////////////////
//                        导出函数定义
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG Config_GetLastError(int *pInt_ErrorCode = NULL);
/************************************************************************/
/*                        文件配置读取                                  */
/************************************************************************/
extern "C" bool Config_Json_File(LPCXSTR lpszConfigFile,XENGINE_SERVERCONFIG *pSt_ServerConfig);
extern "C" bool Config_Json_VersionFile(LPCXSTR lpszConfigFile, XENGINE_SERVERCONFIG* pSt_ServerConfig);
extern "C" bool Config_Json_DBFile(LPCXSTR lpszConfigFile, MESSAGEQUEUE_DBCONFIG* pSt_DBConfig);
