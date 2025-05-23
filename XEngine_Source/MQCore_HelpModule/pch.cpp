#include "pch.h"
#include "APIHelp_Counter/APIHelp_Counter.h"
#include "APIHelp_MQHelp/APIHelp_MQHelp.h"
/********************************************************************
//    Created:     2025/03/11  14:51:51
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule\pch.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule
//    File Base:   pch
//    File Ext:    cpp
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     导出实现
//    History:
*********************************************************************/
bool APIHelp_IsErrorOccur = false;
XLONG APIHelp_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////////
CAPIHelp_Counter m_HelpCounter;
CAPIHelp_MQHelp m_MQHelp;
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG APIHelp_GetLastError(int* pInt_SysError)
{
    if (NULL != pInt_SysError)
    {
        *pInt_SysError = errno;
    }
    return APIHelp_dwErrorCode;
}
/*************************************************************************
                        统计计数器导出
**************************************************************************/
extern "C" bool APIHelp_Counter_SerialSet(LPCXSTR lpszQueueName, __int64x nSerial)
{
    return m_HelpCounter.APIHelp_Counter_SerialSet(lpszQueueName, nSerial);
}
extern "C" bool APIHelp_Counter_SerialGet(LPCXSTR lpszQueueName, __int64x* pInt_Serial)
{
    return m_HelpCounter.APIHelp_Counter_SerialGet(lpszQueueName, pInt_Serial);
}
extern "C" bool APIHelp_Counter_SerialDel(LPCXSTR lpszQueueName)
{
    return m_HelpCounter.APIHelp_Counter_SerialDel(lpszQueueName);
}
/*************************************************************************
                        通用帮助函数导出
**************************************************************************/
extern "C" bool APIHelp_MQHelp_JsonToHex(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr)
{
    return m_MQHelp.APIHelp_MQHelp_JsonToHex(pSt_ProtocolHdr);
}