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
    TCHAR tszUserAddr[128];                                             //用户地址
    TCHAR tszUserName[128];                                             //登录的用户名
    time_t nStartTime;                                                  //时间
    int nNetType;                                                       //网络类型
}XENGINE_SESSIONINFO, * LPXENGINE_SESSIONINFO;

class CSessionModule_Client
{
public:
    CSessionModule_Client();
    ~CSessionModule_Client();
public:
    BOOL SessionModule_Client_Init();
    BOOL SessionModule_Client_Destory();
    BOOL SessionModule_Client_Create(LPCTSTR lpszClientAddr, LPCTSTR lpszUserName, int nNetType);
    BOOL SessionModule_Client_Delete(LPCTSTR lpszClientAddr);
    BOOL SessionModule_Client_GetAuth(LPCTSTR lpszClientAddr, TCHAR* ptszUserName = NULL);
private:
    shared_mutex st_Locker;
private:
    unordered_map<tstring, XENGINE_SESSIONINFO> stl_MapSession;
};
