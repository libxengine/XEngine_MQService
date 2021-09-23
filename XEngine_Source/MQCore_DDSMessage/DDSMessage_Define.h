#pragma once
/********************************************************************
//    Created:     2021/09/22  13:19:01
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage\DDSMessage_Define.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage
//    File Base:   DDSMessage_Define
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息分发服务导出的函数
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD DDSMessage_GetLastError(int *pInt_SysError = NULL);
/*************************************************************************
                        订阅者与发布者公有函数
**************************************************************************/
/********************************************************************
函数名称：DDSMessage_CommApi_Init
函数功能：初始化DDS服务
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DDSMessage_CommApi_Init();
/************************************************************************
函数名称：DDSMessage_CommApi_Destory
函数功能：销毁DDS服务
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
extern "C" BOOL DDSMessage_CommApi_Destory();
/********************************************************************
函数名称：DDSMessage_CommApi_TopicCreate
函数功能：创建一个主题
 参数.一：pSt_DDSProtocol
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入主题信息,输出床边的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DDSMessage_CommApi_TopicCreate(XENGINE_PROTOCOL_XDDS * pSt_DDSProtocol);
/************************************************************************
函数名称：DDSMessage_CommApi_TopicDelete
函数功能：删除一个主题
 参数.一：pSt_DDSProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要删除的主题信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
extern "C" BOOL DDSMessage_CommApi_TopicDelete(XENGINE_PROTOCOL_XDDS * pSt_DDSProtocol);
/********************************************************************
函数名称：DDSMessage_CommApi_TopicGet
函数功能：获得主题关联信息
 参数.一：lpszTopicName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要获取的主题名称
 参数.二：pppSt_DDSProtocol
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出主题关联列表,这个值需要释放内存
 参数.三：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出获取的个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DDSMessage_CommApi_TopicGet(LPCTSTR lpszTopicName, XENGINE_PROTOCOL_XDDS * **pppSt_DDSProtocol, int* pInt_ListCount);
