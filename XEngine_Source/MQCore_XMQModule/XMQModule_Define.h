#pragma once
/********************************************************************
//    Created:     2021/06/30  10:05:48
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule\XMQModule_Define.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule
//    File Base:   XMQModule_Define
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列导出定义
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD XMQModule_GetLastError(int *pInt_SysError = NULL);
/*************************************************************************
                        消息队列包管理导出函数
**************************************************************************/
/********************************************************************
函数名称：XMQModule_Packet_Init
函数功能：初始化消息队列包管理工具
 参数.一：lpszCommonName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入公用主题名称
 参数.二：nPacketCount
  In/Out：In
  类型：整数型
  可空：Y
  意思：输入最大允许包个数,默认0,不限制
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL XMQModule_Packet_Init(LPCTSTR lpszCommonName, int nPacketCount = 0);
/************************************************************************
函数名称：XMQModule_Packet_Destory
函数功能：销毁消息队列包管理工具
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
extern "C" BOOL XMQModule_Packet_Destory();
/********************************************************************
函数名称：XMQModule_Packet_TopicCreate
函数功能：创建一个主题
 参数.一：lpszKeyStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL XMQModule_Packet_TopicCreate(LPCTSTR lpszKeyStr);
/********************************************************************
函数名称：XMQModule_Packet_TopicDelete
函数功能：删除一个指定的主题名
 参数.一：lpszKeyStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的主题名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL XMQModule_Packet_TopicDelete(LPCTSTR lpszKeyStr);
/************************************************************************
函数名称：XMQModule_Packet_Post
函数功能：投递一个数据包到消息队列中
  参数.一：pSt_ProtocolHdr
   In/Out：In
   类型：数据结构指针
   可空：N
   意思：输入协议头数据
  参数.二：lpszMsgBuffer
   In/Out：In
   类型：常量字符指针
   可空：N
   意思：要投递的BUFF缓冲区
返回值
  类型：逻辑型
  意思：是否投递成功
备注：
************************************************************************/
extern "C" BOOL XMQModule_Packet_Post(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, LPCTSTR lpszMsgBuffer, int nMsgLen);
/************************************************************************
函数名称：XMQModule_Packet_Get
函数功能：从一个消息队列中获取一个包
  参数.一：pSt_MQProtocol
   In/Out：In
   类型：数据结构指针
   可空：N
   意思：数据协议头,协议头的序列号为可选字段
  参数.二：ptszMsgBuffer
   In/Out：Out
   类型：字符指针
   可空：N
   意思：导出获取到的数据
  参数.三：pInt_Len
   In/Out：In/Out
   类型：整数型指针
   可空：N
   意思：输入，获取数据缓冲区大小，输出真实获取到的大小
返回值
  类型：逻辑型
  意思：是否获取成功
备注：
************************************************************************/
extern "C" BOOL XMQModule_Packet_Get(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_Len);
/************************************************************************
函数名称：XMQModule_Packet_Del
函数功能：从一个队列中删除一个包
  参数.一：pSt_ProtocolHdr
   In/Out：In
   类型：数据结构指针
   可空：N
   意思：要删除的数据协议头信息
  参数.二：lpszMsgBuffer
   In/Out：In
   类型：常量字符指针
   可空：N
   意思：附加删除协议数据
返回值
  类型：逻辑型
  意思：是否删除成功
备注：
************************************************************************/
extern "C" BOOL XMQModule_Packet_Del(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol);
/********************************************************************
函数名称：XMQModule_Packet_GetCount
函数功能：获取指定主题剩余包个数
 参数.一：lpszKeyStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要获取的主题名
 参数.二：pInt_Count
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出总个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL XMQModule_Packet_GetCount(LPCTSTR lpszKeyStr, __int64x * pInt_Count);
/********************************************************************
函数名称：XMQModule_Packet_GetSerial
函数功能：获得开始和结尾序列编号
 参数.一：lpszKeyStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要获取的主题名
 参数.二：pInt_FirstNumber
  In/Out：Out
  类型：整数型指针
  可空：Y
  意思：导出头序列编号
 参数.三：pInt_LastNumber
  In/Out：Out
  类型：整数型指针
  可空：Y
  意思：导出尾序列编号
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL XMQModule_Packet_GetSerial(LPCTSTR lpszKeyStr, __int64x* pInt_FirstNumber = NULL, __int64x* pInt_LastNumber = NULL);