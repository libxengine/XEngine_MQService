#pragma once
/********************************************************************
//    Created:     2021/09/22  13:17:58
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage\DDSMessage_CommApi\DDSMessage_CommApi.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage\DDSMessage_CommApi
//    File Base:   DDSMessage_CommApi
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     服务器类公有API类
//    History:
*********************************************************************/

class CDDSMessage_CommApi
{
public:
    CDDSMessage_CommApi();
    ~CDDSMessage_CommApi();
public:
    BOOL DDSMessage_CommApi_Init();
    BOOL DDSMessage_CommApi_Destory();
    BOOL DDSMessage_CommApi_TopicCreate(XENGINE_PROTOCOL_XDDS* pSt_DDSProtocol);
    BOOL DDSMessage_CommApi_TopicDelete(XENGINE_PROTOCOL_XDDS* pSt_DDSProtocol);
    BOOL DDSMessage_CommApi_TopicGet(LPCTSTR lpszTopicName, XENGINE_PROTOCOL_XDDS*** pppSt_DDSProtocol, int* pInt_ListCount);
private:
    shared_mutex st_Locker;
private:
    unordered_map<tstring, list<XENGINE_PROTOCOL_XDDS> > stl_MapDDSClient;
};
