﻿// pch.h: 这是预编译标头文件。
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
#include <json/json.h>
#else
#ifdef _CENTOS
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif
#endif
#endif //PCH_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <list>
using namespace std;
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_Types.h>
#include "Config_Define.h"
#include "Config_Error.h"
/********************************************************************
//    Created:     2021/07/01  10:47:37
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ConfigModule\pch.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ConfigModule
//    File Base:   pch
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     公用头文件
//    History:
*********************************************************************/
extern BOOL Config_IsErrorOccur;
extern DWORD Config_dwErrorCode;