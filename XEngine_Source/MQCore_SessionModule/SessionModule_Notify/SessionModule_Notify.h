#pragma once
/********************************************************************
//    Created:     2021/07/09  13:37:33
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Notify\SessionModule_Notify.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Notify
//    File Base:   SessionModule_Notify
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息订阅服务
//    History:
*********************************************************************/
typedef struct
{
    shared_ptr<shared_mutex> st_Locker;
    list<tstring>* pStl_ListClient;
}XENGINE_SESSIONNOTIFY, * LPXENGINE_SESSIONNOTIFY;

class CSessionModule_Notify
{
public:
    CSessionModule_Notify();
    ~CSessionModule_Notify();
public:
    BOOL SessionModule_Notify_Create(LPCTSTR lpszTopicName);
    BOOL SessionModule_Notify_Destory(LPCTSTR lpszTopicName);
    BOOL SessionModule_Notify_Insert(LPCTSTR lpszTopicStr, LPCTSTR lpszClientAddr);
    BOOL SessionModule_Notify_Delete(LPCTSTR lpszTopicStr, LPCTSTR lpszClientAddr);
    BOOL SessionModule_Notify_GetList(LPCTSTR lpszTopicStr, TCHAR*** ppptszListClient, int* pInt_ListCount);
private:
    shared_mutex st_Locker;
private:
    unordered_map<tstring, XENGINE_SESSIONNOTIFY> stl_MapSession;
};
