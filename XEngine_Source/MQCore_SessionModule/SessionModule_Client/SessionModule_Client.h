#pragma once
/********************************************************************
//    Created:     2021/07/02  10:12:38
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Client\SessionModule_Client.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Client
//    File Base:   SessionModule_Client
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息会话服务
//    History:
*********************************************************************/
typedef struct
{
    TCHAR tszKeyStr[128];                                               //主题名称
    __int64x nSerialPos;                                                //当前序列号标记
    time_t nStartTime;                                                  //时间
}XENGINE_SESSIONINFO, * LPXENGINE_SESSIONINFO;

class CSessionModule_Client
{
public:
    CSessionModule_Client();
    ~CSessionModule_Client();
public:
    BOOL SessionModule_Client_Init();
    BOOL SessionModule_Client_Destory();
    BOOL SessionModule_Client_Create(LPCTSTR lpszClientAddr);
    BOOL SessionModule_Client_Delete(LPCTSTR lpszClientAddr);
    BOOL SessionModule_Client_Get(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol);
    BOOL SessionModule_Client_Set(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol);
    BOOL SessionModule_Client_ADDSerial(LPCTSTR lpszClientAddr);
private:
    shared_mutex st_Locker;
private:
    unordered_map<tstring, XENGINE_SESSIONINFO> stl_MapSession;
};