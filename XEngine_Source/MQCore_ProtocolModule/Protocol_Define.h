#pragma once
/********************************************************************
//    Created:     2021/07/01  15:45:45
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\Protocol_Define.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule
//    File Base:   Protocol_Define
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出的定义
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                        导出函数
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD Protocol_GetLastError(int* pInt_SysError = NULL);
/************************************************************************/
/*                        封装类函数                                    */
/************************************************************************/
/********************************************************************
函数名称：ProtocolModule_Packet_TCPCommon
函数功能：通用封包类
 参数.一：pSt_ProtocolHdr
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：协议头
 参数.二：pSt_MQProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：消息头
 参数.三：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：导出封装号的协议
 参数.四：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：导出封装的协议的长度
 参数.五：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：要封装的数据
 参数.六：nMsgLen
  In/Out：In
  类型：整数型
  可空：Y
  意思：要封装数据的长度
 参数.七：enPayType
  In/Out：In
  类型：枚举型
  可空：Y
  意思：消息类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer = NULL, int nMsgLen = 0, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING);
/********************************************************************
函数名称：ProtocolModule_Packet_HttpCommon
函数功能：HTTP通用封包类
 参数.一：pSt_ProtocolHdr
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：协议头
 参数.二：pSt_MQProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：消息头
 参数.三：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：导出封装号的协议
 参数.四：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：导出封装的协议的长度
 参数.五：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：要封装的数据
 参数.六：nMsgLen
  In/Out：In
  类型：整数型
  可空：Y
  意思：要封装数据的长度
 参数.七：enPayType
  In/Out：In
  类型：枚举型
  可空：Y
  意思：消息类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL ProtocolModule_Packet_HttpCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer = NULL, int nMsgLen = 0, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING);
/************************************************************************/
/*                        解析类函数                                    */
/************************************************************************/
/********************************************************************
函数名称：ProtocolModule_Parse_Http
函数功能：HTTP协议解析
 参数.一：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要解析的缓冲区
 参数.二：nMsgLen
  In/Out：In
  类型：整数型
  可空：N
  意思：输入缓冲区大小
 参数.三：pSt_MQProtocol
  In/Out：Out
  类型：数据结构指针
  可空：N
  意思：输出消息协议
 参数.四：pSt_ProtocolHdr
  In/Out：Out
  类型：数据结构指针
  可空：Y
  意思：输出解析到的头协议
 参数.五：pptszMsgBuffer
  In/Out：Out
  类型：二级指针
  可空：Y
  意思：输出消息内容,需要释放内存
 参数.六：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：Y
  意思：输出内容大小
 参数.七：pInt_Type
  In/Out：Out
  类型：整数型指针
  可空：Y
  意思：输出负载类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL ProtocolModule_Parse_Http(LPCTSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, XENGINE_PROTOCOLHDR * pSt_ProtocolHdr = NULL, TCHAR * *pptszMsgBuffer = NULL, int* pInt_MsgLen = NULL, int* pInt_Type = NULL);