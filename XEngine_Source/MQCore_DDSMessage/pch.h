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
#include <time.h>
#include <WinSock2.h>
#endif
#endif //PCH_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <list>
#include <shared_mutex>
#include <unordered_map>
#include <json/json.h>
using namespace std;
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_Types.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Define.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Error.h>
#include "../XQueue_ProtocolHdr.h"
#include "DDSMessage_Define.h"
#include "DDSMessage_Error.h"
/********************************************************************
//    Created:     2021/09/22  13:19:56
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage\pch.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage
//    File Base:   pch
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     公用头文件
//    History:
*********************************************************************/
extern BOOL XDDS_IsErrorOccur;
extern DWORD XDDS_dwErrorCode;

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#ifdef _MSC_BUILD
#pragma comment(lib,"XEngine_BaseLib/XEngine_BaseLib")
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/jsoncpp")
#else
#pragma comment(lib,"../Debug/jsoncpp")
#endif
#else
#ifdef _WIN64
#pragma comment(lib,"../x64/Release/jsoncpp")
#else
#pragma comment(lib,"../Release/jsoncpp")
#endif
#endif
#endif