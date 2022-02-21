#pragma once
/********************************************************************
//    Created:     2021/11/16  15:30:48
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Auth\SessionModule_Auth.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Auth
//    File Base:   SessionModule_Auth
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     授权验证
//    History:
*********************************************************************/

typedef struct
{
    TCHAR tszUserName[64];
    TCHAR tszUserPass[64];
}XENGINE_SESSIONAUTH, * LPXENGINE_SESSIONAUTH;

class CSessionModule_Auth
{
public:
    CSessionModule_Auth();
    ~CSessionModule_Auth();
public:
    BOOL SessionModule_Auth_Init(LPCTSTR lpszAuthFile);
    BOOL SessionModule_Auth_Destory();
    BOOL SessionModule_Auth_Exist(LPCTSTR lpszUser, LPCTSTR lpszPass);
private:
    unordered_map<tstring, XENGINE_SESSIONAUTH> stl_MapSession;
};
