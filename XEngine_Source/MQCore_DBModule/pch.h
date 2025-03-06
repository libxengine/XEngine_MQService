// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

#ifdef _MSC_BUILD
// 添加要在此处预编译的标头
#include "framework.h"
#include <tchar.h>
#endif
#endif //PCH_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <thread>
#include <memory>
using namespace std;
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_Types.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Define.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Error.h>
#include <XEngine_Include/XEngine_HelpComponents/DataBase_Define.h>
#include <XEngine_Include/XEngine_HelpComponents/DataBase_Error.h>
#include "DBModule_Define.h"
#include "DBModule_Error.h"
#include "../MQCore_MemoryCache/MemoryCache_Define.h"
#include "../MQCore_MemoryCache/MemoryCache_Error.h"
/********************************************************************
//    Created:     2022/02/23  16:14:35
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\pch.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule
//    File Base:   pch
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     通用公共头文件
//    History:
*********************************************************************/
extern bool DBModule_IsErrorOccur;
extern XLONG DBModule_dwErrorCode;

#ifdef _MSC_BUILD
#pragma comment(lib,"XEngine_BaseLib/XEngine_BaseLib.lib")
#pragma comment(lib,"XEngine_HelpComponents/HelpComponents_DataBase.lib")
#ifdef _DEBUG
#ifdef _M_X64
#pragma comment(lib,"../x64/Debug/MQCore_MemoryCache.lib")
#elif _M_ARM64
#pragma comment(lib,"../ARM64/Debug/MQCore_MemoryCache.lib")
#elif _M_IX86
#pragma comment(lib,"../Debug/MQCore_MemoryCache.lib")
#endif
#else
#ifdef _M_X64
#pragma comment(lib,"../x64/Release/MQCore_MemoryCache.lib")
#elif _M_ARM64
#pragma comment(lib,"../ARM64/Release/MQCore_MemoryCache.lib")
#elif _M_IX86
#pragma comment(lib,"../Release/MQCore_MemoryCache.lib")
#endif
#endif
#endif