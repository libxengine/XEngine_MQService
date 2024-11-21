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
bool CConfig_Json::Config_Json_File(LPCXSTR lpszConfigFile,XENGINE_SERVERCONFIG *pSt_ServerConfig)
{
    Config_IsErrorOccur = false;

    if ((NULL == lpszConfigFile) || (NULL == pSt_ServerConfig))
    {
        Config_IsErrorOccur = true;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARAMENT;
        return false;
    }
    JSONCPP_STRING st_JsonError;
    Json::Value st_JsonRoot;
    Json::CharReaderBuilder st_JsonBuilder;

    FILE* pSt_File = _xtfopen(lpszConfigFile, _X("rb"));
    if (NULL == pSt_File)
    {
        Config_IsErrorOccur = true;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARAMENT;
        return false;
    }
    int nCount = 0;
    XCHAR tszMsgBuffer[4096];
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
        Config_IsErrorOccur = true;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARSE;
        return false;
    }
    _tcsxcpy(pSt_ServerConfig->tszIPAddr,st_JsonRoot["tszIPAddr"].asCString());
    _tcsxcpy(pSt_ServerConfig->tszTopic, st_JsonRoot["tszTopic"].asCString());
    pSt_ServerConfig->bDeamon = st_JsonRoot["bDeamon"].asInt();
    pSt_ServerConfig->nTCPPort = st_JsonRoot["nTCPPort"].asInt();
    pSt_ServerConfig->nHttpPort = st_JsonRoot["nHttpPort"].asInt();
    pSt_ServerConfig->nWSPort = st_JsonRoot["nWSPort"].asInt();
	pSt_ServerConfig->nMQTTPort = st_JsonRoot["nMQTTPort"].asInt();

    if (st_JsonRoot["XMax"].empty() || (7 != st_JsonRoot["XMax"].size()))
    {
        Config_IsErrorOccur = true;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XMAX;
        return false;
    }
    Json::Value st_JsonXMax = st_JsonRoot["XMax"];
    pSt_ServerConfig->st_XMax.nMaxClient = st_JsonXMax["nMaxClient"].asInt();
    pSt_ServerConfig->st_XMax.nMaxQueue = st_JsonXMax["nMaxQueue"].asInt();
    pSt_ServerConfig->st_XMax.nIOThread = st_JsonXMax["nIOThread"].asInt();
    pSt_ServerConfig->st_XMax.nTCPThread = st_JsonXMax["nTCPThread"].asInt();
    pSt_ServerConfig->st_XMax.nHttpThread = st_JsonXMax["nHttpThread"].asInt();
    pSt_ServerConfig->st_XMax.nWSThread = st_JsonXMax["nWSThread"].asInt();
	pSt_ServerConfig->st_XMax.nMQTTThread = st_JsonXMax["nMQTTThread"].asInt();

    if (st_JsonRoot["XLog"].empty() || (4 != st_JsonRoot["XLog"].size()))
    {
        Config_IsErrorOccur = true;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XLOG;
        return false;
    }
    Json::Value st_JsonXLog = st_JsonRoot["XLog"];
    pSt_ServerConfig->st_XLog.nMaxSize = st_JsonXLog["MaxSize"].asInt();
    pSt_ServerConfig->st_XLog.nMaxCount = st_JsonXLog["MaxCount"].asInt();
    pSt_ServerConfig->st_XLog.nLogLeave = st_JsonXLog["LogLeave"].asInt();
	_tcsxcpy(pSt_ServerConfig->st_XLog.tszLOGFile, st_JsonXLog["tszLOGFile"].asCString());

    if (st_JsonRoot["XSql"].empty() || (4 != st_JsonRoot["XSql"].size()))
    {
        Config_IsErrorOccur = true;
        Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XSQL;
        return false;
    }
    Json::Value st_JsonXSql = st_JsonRoot["XSql"];
    pSt_ServerConfig->st_XSql.nSQLPort = st_JsonXSql["SQLPort"].asInt();
    _tcsxcpy(pSt_ServerConfig->st_XSql.tszSQLAddr,st_JsonXSql["SQLAddr"].asCString());
    _tcsxcpy(pSt_ServerConfig->st_XSql.tszSQLUser,st_JsonXSql["SQLUser"].asCString());
    _tcsxcpy(pSt_ServerConfig->st_XSql.tszSQLPass,st_JsonXSql["SQLPass"].asCString());

	if (st_JsonRoot["XPass"].empty() || (5 != st_JsonRoot["XPass"].size()))
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XPASS;
		return false;
	}
	Json::Value st_JsonXPass = st_JsonRoot["XPass"];
    pSt_ServerConfig->st_XPass.nTimeout = st_JsonXPass["nTimeout"].asInt();
    _tcsxcpy(pSt_ServerConfig->st_XPass.tszPassLogin, st_JsonXPass["tszPassLogin"].asCString());
	_tcsxcpy(pSt_ServerConfig->st_XPass.tszPassLogout, st_JsonXPass["tszPassLogout"].asCString());
	_tcsxcpy(pSt_ServerConfig->st_XPass.tszPassRegister, st_JsonXPass["tszPassRegister"].asCString());
	_tcsxcpy(pSt_ServerConfig->st_XPass.tszPassUNReg, st_JsonXPass["tszPassUNReg"].asCString());

	if (st_JsonRoot["XMemory"].empty() || (3 != st_JsonRoot["XMemory"].size()))
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XMEMORY;
		return false;
	}
	Json::Value st_JsonXMemory = st_JsonRoot["XMemory"];
	pSt_ServerConfig->st_XMemory.bEnable = st_JsonXMemory["bEnable"].asBool();
	pSt_ServerConfig->st_XMemory.nTimeLast = st_JsonXMemory["nTimeLast"].asInt();
	pSt_ServerConfig->st_XMemory.nTimeStart = st_JsonXMemory["nTimeStart"].asInt();

	if (st_JsonRoot["XReport"].empty() || (3 != st_JsonRoot["XReport"].size()))
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XREPORT;
		return false;
	}
	Json::Value st_JsonXReport = st_JsonRoot["XReport"];
	pSt_ServerConfig->st_XReport.bEnable = st_JsonXReport["bEnable"].asBool();
	_tcsxcpy(pSt_ServerConfig->st_XReport.tszAPIUrl, st_JsonXReport["tszAPIUrl"].asCString());
	_tcsxcpy(pSt_ServerConfig->st_XReport.tszServiceName, st_JsonXReport["tszServiceName"].asCString());
    return true;
}
bool CConfig_Json::Config_Json_VersionFile(LPCXSTR lpszConfigFile, XENGINE_SERVERCONFIG* pSt_ServerConfig)
{
	Config_IsErrorOccur = false;

	if ((NULL == lpszConfigFile) || (NULL == pSt_ServerConfig))
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARAMENT;
		return false;
	}
	JSONCPP_STRING st_JsonError;
	Json::Value st_JsonRoot;
	Json::CharReaderBuilder st_JsonBuilder;

	FILE* pSt_File = _xtfopen(lpszConfigFile, _X("rb"));
	if (NULL == pSt_File)
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARAMENT;
		return false;
	}
	int nCount = 0;
	XCHAR tszMsgBuffer[4096];
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
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARSE;
		return false;
	}

	if (st_JsonRoot["XVer"].empty())
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XVER;
		return false;
	}
	Json::Value st_JsonXVer = st_JsonRoot["XVer"];
	pSt_ServerConfig->st_XVer.pStl_ListStorage = new list<tstring>;

	for (unsigned int i = 0; i < st_JsonXVer.size(); i++)
	{
		pSt_ServerConfig->st_XVer.pStl_ListStorage->push_back(st_JsonXVer[i].asCString());
	}
	return true;
}
bool CConfig_Json::Config_Json_DBFile(LPCXSTR lpszConfigFile, MESSAGEQUEUE_DBCONFIG* pSt_DBConfig)
{
	Config_IsErrorOccur = false;

	if ((NULL == lpszConfigFile) || (NULL == pSt_DBConfig))
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARAMENT;
		return false;
	}
	JSONCPP_STRING st_JsonError;
	Json::Value st_JsonRoot;
	Json::CharReaderBuilder st_JsonBuilder;

	FILE* pSt_File = _xtfopen(lpszConfigFile, _X("rb"));
	if (NULL == pSt_File)
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARAMENT;
		return false;
	}
	int nCount = 0;
	XCHAR tszMsgBuffer[4096];
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
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_PARSE;
		return false;
	}

	if (st_JsonRoot["MQUser"].empty())
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XMAX;
		return false;
	}
	Json::Value st_JsonUser = st_JsonRoot["MQUser"];
	if (st_JsonUser["UserTime"].empty())
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XMAX;
		return false;
	}
	Json::Value st_JsonUserTime = st_JsonUser["UserTime"];
	pSt_DBConfig->st_MQUser.st_UserTime.bPubClear = st_JsonUserTime["bPubClear"].asBool();

	if (st_JsonRoot["MQData"].empty() || (2 != st_JsonRoot["MQData"].size()))
	{
		Config_IsErrorOccur = true;
		Config_dwErrorCode = ERROR_MQ_MODULE_CONFIG_JSON_XTIME;
		return false;
	}
	Json::Value st_JsonMQData = st_JsonRoot["MQData"];
	pSt_DBConfig->st_MQData.nDBMonth = st_JsonMQData["nDBMonth"].asInt();
	pSt_DBConfig->st_MQData.bCommSub = st_JsonMQData["bCommSub"].asBool();
	return true;
}