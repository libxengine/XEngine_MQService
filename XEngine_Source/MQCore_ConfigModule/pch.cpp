﻿#include "pch.h"
#include "Config_Json/Config_Json.h"
/********************************************************************
//    Created:     2021/07/01  11:01:57
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ConfigModule\pch.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ConfigModule
//    File Base:   pch
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出函数实现
//    History:
*********************************************************************/
bool Config_IsErrorOccur = false;
XLONG Config_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////
CConfig_Json m_ConfigJson;
//////////////////////////////////////////////////////////////////////
//                        导出函数定义
//////////////////////////////////////////////////////////////////////
extern "C" XLONG Config_GetLastError(int* pInt_ErrorCode)
{
	if (pInt_ErrorCode != NULL)
	{
		*pInt_ErrorCode = errno;
	}
	return Config_dwErrorCode;
}
/************************************************************************/
/*                        配置文件读取                                  */
/************************************************************************/
extern "C" bool Config_Json_File(LPCXSTR lpszConfigFile, XENGINE_SERVERCONFIG * pSt_ServerConfig)
{
	return m_ConfigJson.Config_Json_File(lpszConfigFile, pSt_ServerConfig);
}
extern "C" bool Config_Json_VersionFile(LPCXSTR lpszConfigFile, XENGINE_SERVERCONFIG * pSt_ServerConfig)
{
	return m_ConfigJson.Config_Json_VersionFile(lpszConfigFile, pSt_ServerConfig);
}
extern "C" bool Config_Json_DBFile(LPCXSTR lpszConfigFile, MESSAGEQUEUE_DBCONFIG * pSt_DBConfig)
{
	return m_ConfigJson.Config_Json_DBFile(lpszConfigFile, pSt_DBConfig);
}