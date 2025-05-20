#pragma once
/********************************************************************
//    Created:     2025/05/20  15:18:46
//    File Name:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_HTTPTask\MQService_HTTPPost.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_HTTPTask
//    File Base:   MQService_HTTPPost
//    File Ext:    h
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     HTTP POST方法处理
//    History:
*********************************************************************/
bool MessageQueue_HttpTask_Post(LPCXSTR lpszClientAddr, LPCXSTR lpszFuncName, LPCXSTR lpszMsgBuffer, int nMsgLen);