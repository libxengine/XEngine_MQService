// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

#ifdef _WINDOWS
// 添加要在此处预编译的标头
#include "framework.h"
#include <tchar.h>
#endif
#endif //PCH_H
#include <string.h>
#include <memory>
#include <json/json.h>
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_Types.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Define.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Error.h>
#include <XEngine_Include/XEngine_Core/Cryption_Define.h>
#include <XEngine_Include/XEngine_Core/Cryption_Error.h>
#include <XEngine_Include/XEngine_Core/ManagePool_Define.h>
#include <XEngine_Include/XEngine_HelpComponents/DataBase_Define.h>
#include <XEngine_Include/XEngine_RfcComponents/MQTTProtocol_Define.h>
#include <XEngine_Include/XEngine_RfcComponents/MQTTProtocol_Error.h>
#include "../XQueue_ProtocolHdr.h"
#include "../MQCore_DBModule/DBModule_Define.h"
#include "Protocol_Define.h"
#include "Protocol_Error.h"
/********************************************************************
//    Created:     2021/07/01  15:44:55
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\pch.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule
//    File Base:   pch
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     公用头文件
//    History:
*********************************************************************/
extern bool Protocol_IsErrorOccur;
extern XLONG Protocol_dwErrorCode;

#ifdef _MSC_BUILD
#pragma comment(lib,"XEngine_BaseLib/XEngine_BaseLib")
#pragma comment(lib,"XEngine_Core/XEngine_Cryption")
#pragma comment(lib,"XEngine_RfcComponents/RfcComponents_MQTTProtocol.lib")
#ifdef _DEBUG
#ifdef _M_X64
#pragma comment(lib,"../x64/Debug/jsoncpp")
#elif _M_ARM64
#pragma comment(lib,"../ARM64/Debug/jsoncpp")
#elif _M_IX86
#pragma comment(lib,"../Debug/jsoncpp")
#endif
#else
#ifdef _M_X64
#pragma comment(lib,"../x64/Release/jsoncpp")
#elif _M_ARM64
#pragma comment(lib,"../ARM64/Release/jsoncpp")
#elif _M_IX86
#pragma comment(lib,"../Release/jsoncpp")
#endif
#endif
#endif