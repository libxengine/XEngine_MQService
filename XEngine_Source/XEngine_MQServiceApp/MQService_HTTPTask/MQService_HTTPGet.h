#pragma once
/********************************************************************
//    Created:     2025/05/20  16:33:07
//    File Name:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_HTTPTask\MQService_HTTPGet.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_HTTPTask
//    File Base:   MQService_HTTPGet
//    File Ext:    h
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     HTTP GET方法处理
//    History:
*********************************************************************/
bool MessageQueue_HttpTask_Get(LPCXSTR lpszClientAddr, LPCXSTR lpszFuncName, XCHAR*** pptszListHdr, int nUrlCount);