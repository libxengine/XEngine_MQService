#include "pch.h"
#include "APIHelp_MQHelp.h"
/********************************************************************
//    Created:     2025/05/23  14:16:56
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule\APIHelp_MQHelp\APIHelp_MQHelp.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule\APIHelp_MQHelp
//    File Base:   APIHelp_MQHelp
//    File Ext:    cpp
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     通用帮助函数
//    History:
*********************************************************************/
CAPIHelp_MQHelp::CAPIHelp_MQHelp()
{
}
CAPIHelp_MQHelp::~CAPIHelp_MQHelp()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：APIHelp_MQHelp_ProtocolToHex
函数功能：协议转二进制
 参数.一：pSt_ProtocolHdr
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：打包的协议
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CAPIHelp_MQHelp::APIHelp_MQHelp_JsonToHex(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr)
{
	APIHelp_IsErrorOccur = false;

	pSt_ProtocolHdr->wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	pSt_ProtocolHdr->byIsReply = true;
	pSt_ProtocolHdr->byVersion = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN;
	pSt_ProtocolHdr->unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	pSt_ProtocolHdr->unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	pSt_ProtocolHdr->wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	return true;
}