#include "pch.h"
#include "XMQModule_Packet.h"
/********************************************************************
//    Created:     2021/06/30  10:03:09
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule\XMQModule_Packet\XMQModule_Packet.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule\XMQModule_Packet
//    File Base:   XMQModule_Packet
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列服务
//    History:
*********************************************************************/
CXMQModule_Packet::CXMQModule_Packet()
{
    m_nMaxPacket = 0;
}
CXMQModule_Packet::~CXMQModule_Packet()
{
}
///////////////////////////////////////////////////////////////////////////////
//                      公有函数
///////////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：XMQModule_Packet_Init
函数功能：初始化消息队列包管理工具
 参数.一：lpszCommonName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入公用主题名称
 参数.二：nPacketCount
  In/Out：In
  类型：整数型
  可空：Y
  意思：输入最大允许包个数,默认0,不限制
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CXMQModule_Packet::XMQModule_Packet_Init(LPCTSTR lpszCommonName, int nPacketCount /* = 0 */)
{
    MQX_IsErrorOccur = FALSE;

	m_nMaxPacket = nPacketCount;
	//添加公有主题
    if (!XMQModule_Packet_TopicCreate(lpszCommonName))
    {
        return FALSE;
    }
    return TRUE;
}
/************************************************************************
函数名称：XMQModule_Packet_Destory
函数功能：销毁消息队列包管理工具
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
BOOL CXMQModule_Packet::XMQModule_Packet_Destory()
{
    MQX_IsErrorOccur = FALSE;

    //释放队列资源
    st_Locker.lock();
    unordered_map<tstring, XENGINE_PACKETINFO*>::iterator stl_MapIterator = stl_MapMQPacket.begin();
    for (; stl_MapIterator != stl_MapMQPacket.end(); stl_MapIterator++)
    {
        //关闭线程
		stl_MapIterator->second->bIsRun = FALSE;
		stl_MapIterator->second->pSTDThread->join();
        //清理资源
        stl_MapIterator->second->st_Locker.lock();
        list<XENGINE_MQXPACKET>::iterator stl_ListIterator = stl_MapIterator->second->pStl_ListPacket->begin();
        for (;stl_ListIterator != stl_MapIterator->second->pStl_ListPacket->end();stl_ListIterator++)
        {
            free(stl_ListIterator->ptszMsgBuffer);
            stl_ListIterator->ptszMsgBuffer = NULL;
        }
        stl_MapIterator->second->pStl_ListPacket->clear();
        stl_MapIterator->second->st_Locker.unlock();
    }
    stl_MapMQPacket.clear();
    st_Locker.unlock();
    
    return TRUE;
}
/********************************************************************
函数名称：XMQModule_Packet_TopicCreate
函数功能：创建一个主题
 参数.一：lpszKeyStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CXMQModule_Packet::XMQModule_Packet_TopicCreate(LPCTSTR lpszKeyStr)
{
    MQX_IsErrorOccur = FALSE;

    if (NULL == lpszKeyStr)
    {
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_CREATE_PARAMENT;
		return FALSE;
    }
	XENGINE_PACKETINFO* pSt_PacketInfo = new XENGINE_PACKETINFO;
	if (NULL == pSt_PacketInfo)
	{
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_CREATE_MALLOC;
		return FALSE;
	}
	pSt_PacketInfo->bIsRun = TRUE;
	pSt_PacketInfo->lClass = this;
	pSt_PacketInfo->nSerialLast = 0;
	pSt_PacketInfo->nSerialPret = 0;
	pSt_PacketInfo->pStl_ListPacket = new list<XENGINE_MQXPACKET>;
	if (NULL == pSt_PacketInfo->pStl_ListPacket)
	{
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_CREATE_MALLOC;
		return FALSE;
	}
	//创建线程
	pSt_PacketInfo->pSTDThread = make_shared<std::thread>(XMQModule_Packet_Thread, pSt_PacketInfo);
	if (!pSt_PacketInfo->pSTDThread->joinable())
	{
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_CREATE_THREAD;
		return FALSE;
	}
	//添加主题
    st_Locker.lock();
	stl_MapMQPacket.insert(make_pair(lpszKeyStr, pSt_PacketInfo));
    st_Locker.unlock();
    return TRUE;
}
/********************************************************************
函数名称：XMQModule_Packet_TopicDelete
函数功能：删除一个指定的主题名
 参数.一：lpszKeyStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的主题名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CXMQModule_Packet::XMQModule_Packet_TopicDelete(LPCTSTR lpszKeyStr)
{
	MQX_IsErrorOccur = FALSE;

	st_Locker.lock();
	unordered_map<tstring, XENGINE_PACKETINFO* >::iterator stl_MapIterator = stl_MapMQPacket.find(lpszKeyStr);
	if (stl_MapIterator != stl_MapMQPacket.end())
	{
		//关闭线程
        stl_MapIterator->second->bIsRun = FALSE;
		stl_MapIterator->second->pSTDThread->join();
		//清理资源
        stl_MapIterator->second->st_Locker.lock();
        list<XENGINE_MQXPACKET>::iterator stl_ListIterator = stl_MapIterator->second->pStl_ListPacket->begin();
        for (; stl_ListIterator != stl_MapIterator->second->pStl_ListPacket->end(); stl_ListIterator++)
        {
			free(stl_ListIterator->ptszMsgBuffer);
			stl_ListIterator->ptszMsgBuffer = NULL;
        }
        stl_MapIterator->second->pStl_ListPacket->clear();
        stl_MapMQPacket.erase(stl_MapIterator);
	}
	st_Locker.unlock();

	return TRUE;
}
/************************************************************************
函数名称：XMQModule_Packet_Post
函数功能：投递一个数据包到消息队列中
  参数.一：pSt_ProtocolHdr
   In/Out：In
   类型：数据结构指针
   可空：N
   意思：输入协议头数据
  参数.二：lpszMsgBuffer
   In/Out：In
   类型：常量字符指针
   可空：N
   意思：要投递的BUFF缓冲区
返回值
  类型：逻辑型
  意思：是否投递成功
备注：
************************************************************************/
BOOL CXMQModule_Packet::XMQModule_Packet_Post(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, LPCTSTR lpszMsgBuffer, int nMsgLen)
{
    MQX_IsErrorOccur = FALSE;

    if (NULL == lpszMsgBuffer)
    {
        MQX_IsErrorOccur = TRUE;
        MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_POST_PARAMENT;
        return FALSE;
    }
	XENGINE_MQXPACKET st_MQPacket;
	memset(&st_MQPacket, '\0', sizeof(XENGINE_MQXPACKET));

    st_MQPacket.ptszMsgBuffer = (TCHAR *)malloc(nMsgLen);
    if (NULL == st_MQPacket.ptszMsgBuffer)
    {
        MQX_IsErrorOccur = TRUE;
        MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_POST_MALLOC;
        return FALSE;
    }
    memset(st_MQPacket.ptszMsgBuffer, '\0', nMsgLen);

    st_MQPacket.nMsgLen = nMsgLen;
    st_MQPacket.nStartTime = time(NULL);
    memcpy(&st_MQPacket.st_XMQProtocol, pSt_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
	memcpy(st_MQPacket.ptszMsgBuffer, lpszMsgBuffer, nMsgLen);

	st_Locker.lock_shared();
	if (!XMQModule_Packet_Push(&st_MQPacket))
	{
        free(st_MQPacket.ptszMsgBuffer);
        st_MQPacket.ptszMsgBuffer = NULL;
        st_Locker.unlock_shared();
        return FALSE;
	}
	st_Locker.unlock_shared();
    return TRUE;
}
/************************************************************************
函数名称：XMQModule_Packet_Get
函数功能：从一个消息队列中获取一个包
  参数.一：pSt_MQProtocol
   In/Out：In
   类型：数据结构指针
   可空：N
   意思：数据协议头,协议头的序列号为可选字段
  参数.二：ptszMsgBuffer
   In/Out：Out
   类型：字符指针
   可空：N
   意思：导出获取到的数据
  参数.三：pInt_Len
   In/Out：In/Out
   类型：整数型指针
   可空：N
   意思：输入，获取数据缓冲区大小，输出真实获取到的大小
返回值
  类型：逻辑型
  意思：是否获取成功
备注：
************************************************************************/
BOOL CXMQModule_Packet::XMQModule_Packet_Get(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_Len)
{
    MQX_IsErrorOccur = FALSE;

    if ((NULL == pSt_MQProtocol) || (NULL == ptszMsgBuffer) || (NULL == pInt_Len))
    {
        MQX_IsErrorOccur = TRUE;
        MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_GET_PARAMENT;
        return FALSE;
    }
    //开始取出数据
    st_Locker.lock_shared();
    unordered_map<tstring, XENGINE_PACKETINFO*>::iterator stl_MapIterator = stl_MapMQPacket.find(pSt_MQProtocol->tszMQKey);
	if (stl_MapIterator == stl_MapMQPacket.end())
	{
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_GET_NOTFOUND;
		return FALSE;
	}
    //队列是否为空
    stl_MapIterator->second->st_Locker.lock();
    if (stl_MapIterator->second->pStl_ListPacket->empty())
    {
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_GET_EMPTY;
        stl_MapIterator->second->st_Locker.unlock();
        return FALSE;
    }
	BOOL bIsFound = FALSE;
	//如果找到了
	list<XENGINE_MQXPACKET>::iterator stl_ListIterator = stl_MapIterator->second->pStl_ListPacket->begin();
	for (; stl_ListIterator != stl_MapIterator->second->pStl_ListPacket->end(); stl_ListIterator++)
	{
		//只取序列号,那么需要判断当前是否输入了值
		if (pSt_MQProtocol->nSerial > 0)
		{
			//输入了按照匹配取
			if (pSt_MQProtocol->nSerial == stl_ListIterator->st_XMQProtocol.nSerial)
			{
				bIsFound = TRUE;
				break;
			}
		}
		else
		{
			//没有输入就取最小的一个
			bIsFound = TRUE;
			break;
		}
	}
	//判断是否找到
	if (!bIsFound)
	{
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_GET_NOSERIAL;
        stl_MapIterator->second->st_Locker.unlock();
		return FALSE;
	}
	//判断长度
	if (*pInt_Len < stl_ListIterator->nMsgLen)
	{
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_GET_LEN;
		*pInt_Len = stl_ListIterator->nMsgLen;
        stl_MapIterator->second->st_Locker.unlock();
		return FALSE;
	}
	//拷贝数据
	*pInt_Len = stl_ListIterator->nMsgLen;
	memcpy(ptszMsgBuffer, stl_ListIterator->ptszMsgBuffer, stl_ListIterator->nMsgLen);
	//判断保存时间是否等于0
	if (0 == stl_ListIterator->st_XMQProtocol.nKeepTime)
	{
		free(stl_ListIterator->ptszMsgBuffer);
		stl_ListIterator->ptszMsgBuffer = NULL;
		stl_MapIterator->second->pStl_ListPacket->erase(stl_ListIterator);    
	}
	if (stl_ListIterator->st_XMQProtocol.nGetTimer > 0)
	{
		//是否有取出次数限制
		stl_ListIterator->nGetNow++;
		if (stl_ListIterator->nGetNow >= stl_ListIterator->st_XMQProtocol.nGetTimer)
		{
			free(stl_ListIterator->ptszMsgBuffer);
			stl_ListIterator->ptszMsgBuffer = NULL;
			stl_MapIterator->second->pStl_ListPacket->erase(stl_ListIterator);
		}
	}
    stl_MapIterator->second->st_Locker.unlock();
    st_Locker.unlock_shared();
    return TRUE;
}
/************************************************************************
函数名称：XMQModule_Packet_Del
函数功能：从一个队列中删除一个包
  参数.一：pSt_ProtocolHdr
   In/Out：In
   类型：数据结构指针
   可空：N
   意思：要删除的数据协议头信息
  参数.二：lpszMsgBuffer
   In/Out：In
   类型：常量字符指针
   可空：N
   意思：附加删除协议数据
返回值
  类型：逻辑型
  意思：是否删除成功
备注：
************************************************************************/
BOOL CXMQModule_Packet::XMQModule_Packet_Del(XENGINE_PROTOCOL_XMQ* pSt_MQProtocol)
{
    MQX_IsErrorOccur = FALSE;

    if (NULL == pSt_MQProtocol)
    {
        MQX_IsErrorOccur = TRUE;
        MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_DEL_PARAMENT;
        return FALSE;
    }
	if (pSt_MQProtocol->nSerial <= 0)
	{
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_DEL_NOSERIAL;
		return FALSE;
	}
	//查找指定KEY数据
	st_Locker.lock_shared();
	unordered_map<tstring, XENGINE_PACKETINFO*>::iterator stl_MapIterator = stl_MapMQPacket.find(pSt_MQProtocol->tszMQKey);
	if (stl_MapIterator == stl_MapMQPacket.end())
	{
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_DEL_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	stl_MapIterator->second->st_Locker.lock();
	list<XENGINE_MQXPACKET>::iterator stl_ListIterator = stl_MapIterator->second->pStl_ListPacket->begin();
	for (; stl_ListIterator != stl_MapIterator->second->pStl_ListPacket->end();)
	{
		if (pSt_MQProtocol->nSerial == stl_ListIterator->st_XMQProtocol.nSerial)
		{
			//找到直接删除
			free(stl_ListIterator->ptszMsgBuffer);
			stl_ListIterator->ptszMsgBuffer = NULL;
			stl_MapIterator->second->pStl_ListPacket->erase(stl_ListIterator);
			break;
		}
	}
	stl_MapIterator->second->st_Locker.unlock();
	st_Locker.unlock_shared();
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
//                      保护函数
///////////////////////////////////////////////////////////////////////////////
/************************************************************************
函数名称：XMQModule_Packet_Push
函数功能：投递一个数据包到一个容器中
  参数一：pSt_MQPacket
   In/Out：In
   类型：数据结构指针
   可空：N
   意思：要投递的数据信息
返回值
  类型：逻辑型
  意思：是否投递成功
备注：
************************************************************************/
BOOL CXMQModule_Packet::XMQModule_Packet_Push(XENGINE_MQXPACKET *pSt_MQPacket)
{
    MQX_IsErrorOccur = FALSE;

    //判断包个数是否满了
    if (m_nMaxPacket > 0)
    {
		if (stl_MapMQPacket.size() >= m_nMaxPacket)
		{
			MQX_IsErrorOccur = TRUE;
			MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_PUSH_MAXSIZE;
			return FALSE;
		}
    }
    //开始查找这个地址是否存在
    unordered_map<tstring, XENGINE_PACKETINFO*>::iterator stl_MapIterator = stl_MapMQPacket.find(pSt_MQPacket->st_XMQProtocol.tszMQKey);
    if (stl_MapIterator == stl_MapMQPacket.end())
    {
		MQX_IsErrorOccur = TRUE;
		MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_PUSH_NOTFOUND;
		return FALSE;
    }
    BOOL bExist = FALSE;
    //大于0表示设置了序列号
    if (pSt_MQPacket->st_XMQProtocol.nSerial > 0)
    {
        //处理序列号
        stl_MapIterator->second->st_Locker.lock_shared();
        list<XENGINE_MQXPACKET>::iterator stl_ListIterator = stl_MapIterator->second->pStl_ListPacket->begin();
        for (; stl_ListIterator != stl_MapIterator->second->pStl_ListPacket->end(); stl_ListIterator++)
        {
            if (pSt_MQPacket->st_XMQProtocol.nSerial == stl_ListIterator->st_XMQProtocol.nSerial)
            {
                bExist = TRUE;
                break;
            }
        }
        stl_MapIterator->second->st_Locker.unlock_shared();
        if (bExist)
        {
			MQX_IsErrorOccur = TRUE;
			MQX_dwErrorCode = ERROR_MQ_MODULE_PACKET_PUSH_EXIST;
            return FALSE;
        }
    }
    else
    {
        //序列号越界需要重置
        if (stl_MapIterator->second->nSerialPret >= XENGINE_INT64X_MAX_VALUE)
        {
            stl_MapIterator->second->nSerialPret = 1;
            stl_MapIterator->second->nSerialLast++;
        }
        else
        {
			stl_MapIterator->second->nSerialPret++;
        }
        pSt_MQPacket->st_XMQProtocol.nSerial = stl_MapIterator->second->nSerialPret;
    }

    stl_MapIterator->second->st_Locker.lock();
    stl_MapIterator->second->pStl_ListPacket->push_back(*pSt_MQPacket);
    stl_MapIterator->second->st_Locker.unlock();
    return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
//                      线程函数
///////////////////////////////////////////////////////////////////////////////
XHTHREAD CXMQModule_Packet::XMQModule_Packet_Thread(LPVOID lParam)
{
    XENGINE_PACKETINFO* pSt_PacketInfo = (XENGINE_PACKETINFO*)lParam;

    while (pSt_PacketInfo->bIsRun)
    {
        pSt_PacketInfo->st_Locker.lock_shared();
		list<XENGINE_MQXPACKET>::iterator stl_ListIterator = pSt_PacketInfo->pStl_ListPacket->begin();
		for (; stl_ListIterator != pSt_PacketInfo->pStl_ListPacket->end(); stl_ListIterator++)
		{
			if (stl_ListIterator->st_XMQProtocol.nKeepTime > 0)
			{
				time_t nEndTime = time(NULL);
				if ((nEndTime - stl_ListIterator->nStartTime) >= stl_ListIterator->st_XMQProtocol.nKeepTime)
				{
                    pSt_PacketInfo->st_Locker.unlock_shared();
                    pSt_PacketInfo->st_Locker.lock();
                    pSt_PacketInfo->pStl_ListPacket->erase(stl_ListIterator);
                    pSt_PacketInfo->st_Locker.unlock();
                    pSt_PacketInfo->st_Locker.lock_shared();
					break;
				}
			}
		}
        pSt_PacketInfo->st_Locker.unlock_shared();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
