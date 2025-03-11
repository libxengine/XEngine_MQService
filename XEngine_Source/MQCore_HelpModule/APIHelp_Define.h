#pragma once
/********************************************************************
//    Created:     2025/03/11  14:47:16
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule\APIHelp_Define.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule
//    File Base:   APIHelp_Define
//    File Ext:    h
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     导出的函数
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG APIHelp_GetLastError(int *pInt_SysError = NULL);
/*************************************************************************
                        统计计数器导出
**************************************************************************/
/********************************************************************
函数名称：MemoryCache_DBData_DataSerialSet
函数功能：设置当前序列号
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要插入的队列名称
 参数.二：nSerial
  In/Out：In
  类型：整数型
  可空：N
  意思：输入设置的队列序列号
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool APIHelp_Counter_SerialSet(LPCXSTR lpszQueueName, __int64x nSerial);
/********************************************************************
函数名称：APIHelp_Counter_SerialGet
函数功能：获取当前序列号
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的队列名称
 参数.二：pInt_Serial
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出当前队列序列号
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool APIHelp_Counter_SerialGet(LPCXSTR lpszQueueName, __int64x* pInt_Serial);
/********************************************************************
函数名称：APIHelp_Counter_SerialDel
函数功能：删除当前队列
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的队列名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool APIHelp_Counter_SerialDel(LPCXSTR lpszQueueName);