#pragma once
/********************************************************************
//    Created:     2025/03/11  14:45:06
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule\APIHelp_Counter\APIHelp_Counter.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule\APIHelp_Counter
//    File Base:   APIHelp_Counter
//    File Ext:    h
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     帮助模块计数器
//    History:
*********************************************************************/


class CAPIHelp_Counter
{
public:
	CAPIHelp_Counter();
	~CAPIHelp_Counter();
public:
	bool APIHelp_Counter_SerialSet(LPCXSTR lpszQueueName, __int64x nSerial);
	bool APIHelp_Counter_SerialGet(LPCXSTR lpszQueueName, __int64x* pInt_Serial);
	bool APIHelp_Counter_SerialDel(LPCXSTR lpszQueueName);
protected:
private:
	std::shared_mutex st_Locker;
private:
	std::unordered_map<std::string, std::atomic<__int64x>> stl_MapSerial;
};