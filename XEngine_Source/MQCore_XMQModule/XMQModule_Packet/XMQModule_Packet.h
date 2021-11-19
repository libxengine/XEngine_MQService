#pragma once
/********************************************************************
//    Created:     2021/06/30  10:03:02
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule\XMQModule_Packet\XMQModule_Packet.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule\XMQModule_Packet
//    File Base:   XMQModule_Packet
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列服务
//    History:
*********************************************************************/
#define XENGINE_INT64X_MAX_VALUE 102400000

typedef struct
{
    XENGINE_PROTOCOL_XMQ st_XMQProtocol;                                //XMQ子协议
    time_t nStartTime;                                                  //入包时间
    int nGetNow;                                                        //当前已取得次数
    int nMsgLen;                                                        //数据消息大小
    TCHAR *ptszMsgBuffer;
}XENGINE_MQXPACKET,*LPXENGINE_MQXPACKET;
typedef struct
{
    shared_ptr<std::thread> pSTDThread;
    shared_mutex st_Locker;
    __int64x nSerialPret;                                               //第一个队列序列号
    __int64x nSerialLast;                                               //超出范围值的序列
    BOOL bIsRun;
    LPVOID lClass;
    list<XENGINE_MQXPACKET>* pStl_ListPacket;
}XENGINE_PACKETINFO, * LPXENGINE_PACKETINFO;

class CXMQModule_Packet
{
public:
    CXMQModule_Packet();
    ~CXMQModule_Packet();
public:
    BOOL XMQModule_Packet_Init(LPCTSTR lpszCommonName, int nPacketCount = 0);
    BOOL XMQModule_Packet_Destory();
    BOOL XMQModule_Packet_TopicCreate(LPCTSTR lpszKeyStr);
    BOOL XMQModule_Packet_TopicDelete(LPCTSTR lpszKeyStr);
	BOOL XMQModule_Packet_Post(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, LPCTSTR lpszMsgBuffer, int nMsgLen);
	BOOL XMQModule_Packet_Get(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_Len);
    BOOL XMQModule_Packet_Del(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol);
    BOOL XMQModule_Packet_GetCount(LPCTSTR lpszKeyStr, __int64x* pInt_Count);
    BOOL XMQModule_Packet_GetSerial(LPCTSTR lpszKeyStr, __int64x* pInt_FirstNumber = NULL, __int64x* pInt_LastNumber = NULL);
protected:
    BOOL XMQModule_Packet_Push(XENGINE_MQXPACKET *pSt_MQPacket);
protected:
    static XHTHREAD XMQModule_Packet_Thread(LPVOID lParam);
private:
    UINT m_nMaxPacket;
private:
    shared_mutex st_Locker;
private:
    unordered_map<tstring, XENGINE_PACKETINFO*> stl_MapMQPacket;
};
