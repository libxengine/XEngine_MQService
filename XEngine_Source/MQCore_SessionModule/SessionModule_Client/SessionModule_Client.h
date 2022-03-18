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
    TCHAR tszUserName[128];                                             //登录的用户名
    __int64x nSerialPos;                                                //当前序列号标记
    int nNetType;                                                       //网络类型
    BOOL bAuth;                                                         //是否通过验证
    BOOL bOrder;                                                        //顺序还是倒序
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
    BOOL SessionModule_Client_Create(LPCTSTR lpszClientAddr, int nNetType);
    BOOL SessionModule_Client_Delete(LPCTSTR lpszClientAddr);
    BOOL SessionModule_Client_Get(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, BOOL *pbAuth);
    BOOL SessionModule_Client_Set(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol);
    BOOL SessionModule_Client_SetOrder(LPCTSTR lpszClientAddr, LPCTSTR lpszKeyStr, BOOL bOrder, __int64x nMQSerial);
    BOOL SessionModule_Client_ADDDelSerial(LPCTSTR lpszClientAddr);
    BOOL SessionModule_Client_SetAuth(LPCTSTR lpszClientAddr, LPCTSTR lpszUserName, BOOL bAuth = TRUE);
    BOOL SessionModule_Client_GetAuth(LPCTSTR lpszClientAddr, TCHAR* ptszUserName);
private:
    shared_mutex st_Locker;
private:
    unordered_map<tstring, XENGINE_SESSIONINFO> stl_MapSession;
};
