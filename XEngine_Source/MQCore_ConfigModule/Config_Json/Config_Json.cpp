#include "pch.h"
#include "Config_Json.h"
/********************************************************************
//    Created:     2021/07/01  11:02:26
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ConfigModule\Config_Json\Config_Json.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ConfigModule\Config_Json
//    File Base:   Config_Json
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     文件配置信息操作
//    History:
*********************************************************************/
CConfig_Json::CConfig_Json()
{

}
CConfig_Json::~CConfig_Json()
{

}
//////////////////////////////////////////////////////////////////////////
//                        公用函数
//////////////////////////////////////////////////////////////////////////
BOOL CConfig_Json::Config_Json_File(LPCTSTR lpszConfigFile,XENGINE_SERVERCONFIG *pSt_ServerConfig)
{
    Config_IsErrorOccur = FALSE;

    if ((NULL == lpszConfigFile) || (NULL == pSt_ServerConfig))
    {
        Config_IsErrorOccur = TRUE;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARAMENT;
        return FALSE;
    }
    JSONCPP_STRING st_JsonError;
    Json::Value st_JsonRoot;
    Json::CharReaderBuilder st_JsonBuilder;

    FILE* pSt_File = _tfopen(lpszConfigFile, _T("rb"));
    if (NULL == pSt_File)
    {
        Config_IsErrorOccur = TRUE;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARAMENT;
        return FALSE;
    }
    int nCount = 0;
    TCHAR tszMsgBuffer[4096];
    while (1)
    {
        int nRet = fread(tszMsgBuffer + nCount, 1, 2048, pSt_File);
        if (nRet <= 0)
        {
            break;
        }
        nCount += nRet;
    }
    fclose(pSt_File);

    std::unique_ptr<Json::CharReader> const pSt_JsonReader(st_JsonBuilder.newCharReader());
    if (!pSt_JsonReader->parse(tszMsgBuffer, tszMsgBuffer + nCount, &st_JsonRoot, &st_JsonError))
    {
        Config_IsErrorOccur = TRUE;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARSE;
        return FALSE;
    }
    _tcscpy(pSt_ServerConfig->tszIPAddr,st_JsonRoot["tszIPAddr"].asCString());
    _tcscpy(pSt_ServerConfig->tszTopic, st_JsonRoot["tszTopic"].asCString());
    pSt_ServerConfig->bDeamon = st_JsonRoot["bDeamon"].asInt();
    pSt_ServerConfig->nTCPPort = st_JsonRoot["nTCPPort"].asInt();
    pSt_ServerConfig->nHttpPort = st_JsonRoot["nHttpPort"].asInt();
    pSt_ServerConfig->nWSPort = st_JsonRoot["nWSPort"].asInt();
    pSt_ServerConfig->nBroadRVPort = st_JsonRoot["nBroadRVPort"].asInt();
    pSt_ServerConfig->nBroadSDPort = st_JsonRoot["nBroadSDPort"].asInt();

    if (st_JsonRoot["XMax"].empty() || (6 != st_JsonRoot["XMax"].size()))
    {
        Config_IsErrorOccur = TRUE;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XMAX;
        return FALSE;
    }
    Json::Value st_JsonXMax = st_JsonRoot["XMax"];
    pSt_ServerConfig->st_XMax.nMaxClient = st_JsonXMax["nMaxClient"].asInt();
    pSt_ServerConfig->st_XMax.nMaxQueue = st_JsonXMax["nMaxQueue"].asInt();
    pSt_ServerConfig->st_XMax.nIOThread = st_JsonXMax["nIOThread"].asInt();
    pSt_ServerConfig->st_XMax.nTCPThread = st_JsonXMax["nTCPThread"].asInt();
    pSt_ServerConfig->st_XMax.nHttpThread = st_JsonXMax["nHttpThread"].asInt();
    pSt_ServerConfig->st_XMax.nWSThread = st_JsonXMax["nWSThread"].asInt();

    if (st_JsonRoot["XTime"].empty() || (6 != st_JsonRoot["XTime"].size()))
    {
        Config_IsErrorOccur = TRUE;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XTIME;
        return FALSE;
    }
    Json::Value st_JsonXTime = st_JsonRoot["XTime"];
    pSt_ServerConfig->st_XTime.bHBTime = st_JsonXTime["bHBTime"].asInt();
    pSt_ServerConfig->st_XTime.nDBMonth = st_JsonXTime["nDBMonth"].asInt();
    pSt_ServerConfig->st_XTime.nTimeCheck = st_JsonXTime["nTimeCheck"].asInt();
    pSt_ServerConfig->st_XTime.nTCPTimeOut = st_JsonXTime["nTCPTimeOut"].asInt();
    pSt_ServerConfig->st_XTime.nHttpTimeOut = st_JsonXTime["nHttpTimeOut"].asInt();
    pSt_ServerConfig->st_XTime.nWSTimeOut = st_JsonXTime["nWSTimeOut"].asInt();
    
    if (st_JsonRoot["XLog"].empty() || (3 != st_JsonRoot["XLog"].size()))
    {
        Config_IsErrorOccur = TRUE;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XLOG;
        return FALSE;
    }
    Json::Value st_JsonXLog = st_JsonRoot["XLog"];
    pSt_ServerConfig->st_XLog.nMaxSize = st_JsonXLog["MaxSize"].asInt();
    pSt_ServerConfig->st_XLog.nMaxCount = st_JsonXLog["MaxCount"].asInt();
    pSt_ServerConfig->st_XLog.nLogLeave = st_JsonXLog["LogLeave"].asInt();

    if (st_JsonRoot["XSql"].empty() || (4 != st_JsonRoot["XSql"].size()))
    {
        Config_IsErrorOccur = TRUE;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XSQL;
        return FALSE;
    }
    Json::Value st_JsonXSql = st_JsonRoot["XSql"];
    pSt_ServerConfig->st_XSql.nSQLPort = st_JsonXSql["SQLPort"].asInt();
    _tcscpy(pSt_ServerConfig->st_XSql.tszSQLAddr,st_JsonXSql["SQLAddr"].asCString());
    _tcscpy(pSt_ServerConfig->st_XSql.tszSQLUser,st_JsonXSql["SQLUser"].asCString());
    _tcscpy(pSt_ServerConfig->st_XSql.tszSQLPass,st_JsonXSql["SQLPass"].asCString());

	if (st_JsonRoot["XSql"].empty() || (3 != st_JsonRoot["XAuth"].size()))
	{
		Config_IsErrorOccur = TRUE;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XAUTH;
		return FALSE;
	}
	Json::Value st_JsonXAuth = st_JsonRoot["XAuth"];
	pSt_ServerConfig->st_XAuth.nAuth = st_JsonXAuth["nAuth"].asInt();
	_tcscpy(pSt_ServerConfig->st_XAuth.tszAuthUser, st_JsonXAuth["AuthUser"].asCString());
	_tcscpy(pSt_ServerConfig->st_XAuth.tszAuthHttp, st_JsonXAuth["AuthHttp"].asCString());

	if (st_JsonRoot["XVer"].empty())
	{
		Config_IsErrorOccur = TRUE;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XVER;
		return FALSE;
	}
	Json::Value st_JsonXVer = st_JsonRoot["XVer"];
    pSt_ServerConfig->st_XVer.pStl_ListStorage = new list<tstring>;

    for (unsigned int i = 0; i < st_JsonXVer["VersionList"].size(); i++)
    {
        pSt_ServerConfig->st_XVer.pStl_ListStorage->push_back(st_JsonXVer["VersionList"][i].asCString());
    }
    return TRUE;
}
