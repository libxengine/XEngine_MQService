#include "MQService_Hdr.h"

XHTHREAD XCALLBACK MessageQueue_HttpThread(XPVOID lParam)
{
	int nThreadPos = *(int*)lParam;
	nThreadPos++;
	while (bIsRun)
	{
		if (!HttpProtocol_Server_EventWaitEx(xhHTTPPacket, nThreadPos))
		{
			continue;
		}
		int nListCount = 0;
		XENGINE_MANAGEPOOL_TASKEVENT** ppSst_ListAddr;

		HttpProtocol_Server_GetPoolEx(xhHTTPPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nMsgLen = 0;
				int nHdrCount = 0;
				XCHAR** ppszHdrList;
				XCHAR* ptszMsgBuffer = NULL;
				RFCCOMPONENTS_HTTP_REQPARAM st_HTTPReqparam;

				memset(&st_HTTPReqparam, '\0', sizeof(RFCCOMPONENTS_HTTP_REQPARAM));

				if (HttpProtocol_Server_GetMemoryEx(xhHTTPPacket, ppSst_ListAddr[i]->tszClientAddr, &ptszMsgBuffer, &nMsgLen, &st_HTTPReqparam, &ppszHdrList, &nHdrCount))
				{
					MessageQueue_Http_Handle(&st_HTTPReqparam, ppSst_ListAddr[i]->tszClientAddr, ptszMsgBuffer, nMsgLen, ppszHdrList, nHdrCount);
					BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
					BaseLib_Memory_Free((XPPPMEM)&ppszHdrList, nHdrCount);
				}
			}
		}
		BaseLib_Memory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
bool MessageQueue_Http_Handle(RFCCOMPONENTS_HTTP_REQPARAM* pSt_HTTPParam, LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, XCHAR** pptszListHdr, int nHdrCount)
{
	int nPKTLen = 4096;
	XCHAR tszPKTBuffer[4096] = {};
	XCHAR tszKey[XPATH_MAX] = {};
	XCHAR tszValue[XPATH_MAX] = {};

	LPCXSTR lpszPostMethod = _X("POST");
	LPCXSTR lpszGetMethod = _X("GET");

	if (st_ServiceCfg.st_XVerification.bEnable)
	{
		int nVType = 0;
		RFCCOMPONENTS_HTTP_HDRPARAM st_HDRParam = {};

		st_HDRParam.nHttpCode = 401;
		st_HDRParam.bIsClose = true;
		st_HDRParam.bAuth = true;
		//打包验证信息
		int nHDRLen = 0;
		XCHAR tszHDRBuffer[XPATH_MAX] = {};
		if (1 == st_ServiceCfg.st_XVerification.nVType)
		{
			Verification_HTTP_BasicServerPacket(tszHDRBuffer, &nHDRLen);
		}
		else
		{
			XCHAR tszNonceStr[64] = {};
			XCHAR tszOpaqueStr[64] = {};
			Verification_HTTP_DigestServerPacket(tszHDRBuffer, &nHDRLen, tszNonceStr, tszOpaqueStr);
		}
		//后去验证方法
		if (!Verification_HTTP_GetType(pptszListHdr, nHdrCount, &nVType))
		{
			HttpProtocol_Server_SendMsgEx(xhHTTPPacket, tszPKTBuffer, &nPKTLen, &st_HDRParam, NULL, 0, tszHDRBuffer);
			NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, tszPKTBuffer, nPKTLen);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,用户验证失败,验证方式:%d,错误:%lX"), lpszClientAddr, st_ServiceCfg.st_XVerification.nVType, Verification_GetLastError());
			return false;
		}
		//验证方式是否一致
		if (st_ServiceCfg.st_XVerification.nVType != nVType)
		{
			HttpProtocol_Server_SendMsgEx(xhHTTPPacket, tszPKTBuffer, &nPKTLen, &st_HDRParam, NULL, 0, tszHDRBuffer);
			NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, tszPKTBuffer, nPKTLen);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,用户验证失败,验证方式错误,请求:%d,需求:%d"), lpszClientAddr, nVType, st_ServiceCfg.st_XVerification.nVType);
			return false;
		}
		bool bRet = false;
		int nHTTPCode = 0;
		int nMSGLen = 0;
		XCLIENT_APIHTTP st_APIHttp = {};

		XCHAR* ptszMSGBuffer = NULL;
		if (!APIClient_Http_Request(_X("GET"), st_ServiceCfg.st_XVerification.tszAuthPass, NULL, &nHTTPCode, &ptszMSGBuffer, &nMSGLen, NULL, NULL, &st_APIHttp))
		{
			st_HDRParam.nHttpCode = 500;
			HttpProtocol_Server_SendMsgEx(xhHTTPPacket, tszPKTBuffer, &nPKTLen, &st_HDRParam, NULL, 0, tszHDRBuffer);
			NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, tszPKTBuffer, nPKTLen);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,用户验证失败,GET请求验证服务:%s 失败,错误码:%lX"), lpszClientAddr, st_ServiceCfg.st_XVerification.tszAuthPass, APIClient_GetLastError());
			return false;
		}
		if (200 != nHTTPCode)
		{
			st_HDRParam.nHttpCode = 500;
			HttpProtocol_Server_SendMsgEx(xhHTTPPacket, tszPKTBuffer, &nPKTLen, &st_HDRParam, NULL, 0, tszHDRBuffer);
			NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, tszPKTBuffer, nPKTLen);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,用户验证失败,GET请求验证服务:%s 失败,错误:%d"), lpszClientAddr, st_ServiceCfg.st_XVerification.tszAuthPass, nHTTPCode);
			return false;
		}
		XENGINE_PROTOCOL_USERINFO st_UserAuth = {};
		if (!ProtocolModule_Parse_Register(ptszMSGBuffer, nMsgLen, &st_UserAuth))
		{
			st_HDRParam.nHttpCode = 500;
			HttpProtocol_Server_SendMsgEx(xhHTTPPacket, tszPKTBuffer, &nPKTLen, &st_HDRParam, NULL, 0, tszHDRBuffer);
			NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, tszPKTBuffer, nPKTLen);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,用户验证失败,返回内容:%s 错误,无法继续"), lpszClientAddr, ptszMSGBuffer);
			BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMSGBuffer);
			return false;
		}
		BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMSGBuffer);

		if (1 == nVType)
		{
			bRet = Verification_HTTP_Basic(st_UserAuth.tszUserName, st_UserAuth.tszUserPass, pptszListHdr, nHdrCount);
		}
		else if (2 == nVType)
		{
			bRet = Verification_HTTP_Digest(st_UserAuth.tszUserName, st_UserAuth.tszUserPass, pSt_HTTPParam->tszHttpMethod, pptszListHdr, nHdrCount);
		}

		if (!bRet)
		{
			HttpProtocol_Server_SendMsgEx(xhHTTPPacket, tszPKTBuffer, &nPKTLen, &st_HDRParam, NULL, 0, tszHDRBuffer);
			NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, tszPKTBuffer, nPKTLen);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,用户验证失败,验证处理错误,可能用户密码登信息不匹配,类型:%d"), lpszClientAddr, nVType);
			return false;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,HTTP验证类型:%d 通过"), lpszClientAddr, nVType);
	}
	int nUrlCount = 0;
	XCHAR** ppSt_ListUrl;
	HttpProtocol_ServerHelp_GetParament(pSt_HTTPParam->tszHttpUri, &ppSt_ListUrl, &nUrlCount);
	if (nUrlCount < 1)
	{
		ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen, ERROR_XENGINE_MESSAGE_HTTP_PARAMENT, "url parament is incorrent");
		XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求HTTP管理接口失败,参数错误:%s"), lpszClientAddr, pSt_HTTPParam->tszHttpUri);
		return false;
	}
	//判断请求
	if (0 == _tcsxncmp(lpszPostMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszPostMethod)))
	{
		BaseLib_String_GetKeyValue(ppSt_ListUrl[0], _X("="), tszKey, tszValue);
		MessageQueue_HttpTask_Post(lpszClientAddr, tszValue, lpszMsgBuffer, nMsgLen);
	}
	else if (0 == _tcsxncmp(lpszGetMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszGetMethod)))
	{
		BaseLib_String_GetKeyValue(ppSt_ListUrl[0], _X("="), tszKey, tszValue);
		MessageQueue_HttpTask_Get(lpszClientAddr, tszValue, &ppSt_ListUrl, nUrlCount);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP客户端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}