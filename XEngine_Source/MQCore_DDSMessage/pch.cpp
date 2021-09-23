#include "pch.h"
#include "DDSMessage_CommApi/DDSMessage_CommApi.h"
/********************************************************************
//    Created:     2021/09/22  13:23:28
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage\pch.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage
//    File Base:   pch
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出的函数实现
//    History:
*********************************************************************/
BOOL XDDS_IsErrorOccur = FALSE;
DWORD XDDS_dwErrorCode = 0;
///////////////////////////////////////////////////////////////////////////////
CDDSMessage_CommApi m_DDSCommApi;
///////////////////////////////////////////////////////////////////////////////
//                      导出函数实现
///////////////////////////////////////////////////////////////////////////////
extern "C" DWORD DDSMessage_GetLastError(int* pInt_SysError)
{
	if (NULL != pInt_SysError)
	{
		*pInt_SysError = errno;
	}
	return XDDS_dwErrorCode;
}
/*************************************************************************
						   订阅者与发布者公有函数
**************************************************************************/
extern "C" BOOL DDSMessage_CommApi_Init()
{
	return m_DDSCommApi.DDSMessage_CommApi_Init();
}
extern "C" BOOL DDSMessage_CommApi_Destory()
{
	return m_DDSCommApi.DDSMessage_CommApi_Destory();
}
extern "C" BOOL DDSMessage_CommApi_TopicCreate(XENGINE_PROTOCOL_XDDS * pSt_DDSProtocol)
{
	return m_DDSCommApi.DDSMessage_CommApi_TopicCreate(pSt_DDSProtocol);
}
extern "C" BOOL DDSMessage_CommApi_TopicDelete(XENGINE_PROTOCOL_XDDS * pSt_DDSProtocol)
{
	return m_DDSCommApi.DDSMessage_CommApi_TopicDelete(pSt_DDSProtocol);
}
extern "C" BOOL DDSMessage_CommApi_TopicGet(LPCTSTR lpszTopicName, XENGINE_PROTOCOL_XDDS * **pppSt_DDSProtocol, int* pInt_ListCount)
{
	return m_DDSCommApi.DDSMessage_CommApi_TopicGet(lpszTopicName, pppSt_DDSProtocol, pInt_ListCount);
}