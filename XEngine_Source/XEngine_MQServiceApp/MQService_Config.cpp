#include "MQService_Hdr.h"

bool MQ_Service_Parament(int argc, char** argv, XENGINE_SERVERCONFIG* pSt_StartlParam)
{
    LPCXSTR lpszConfigFile = _X("./XEngine_Config/XEngine_Config.json");
    LPCXSTR lpszVersionFile = _X("./XEngine_Config/XEngine_VerConfig.json");

	if (!Config_Json_File(lpszConfigFile, pSt_StartlParam))
	{
		printf("解析配置文件失败,Config_Json_File:%lX\n", Config_GetLastError());
		return false;
	}
	if (!Config_Json_VersionFile(lpszVersionFile, pSt_StartlParam))
	{
		printf("解析配置文件失败,Config_Json_VerFile:%lX\n", Config_GetLastError());
		return false;
	}

    for (int i = 0;i < argc;i++)
    {
        if (0 == _tcsxicmp("-h",argv[i]))
        {
            MQ_Service_ParamentHelp();
            return false;
        }
        else if (0 == _tcsxicmp("-v",argv[i]))
        {
            printf("Version：V1.1.0\n");
            return false;
        }
        else if (0 == _tcsxicmp("-tp",argv[i]))
        {
            pSt_StartlParam->nTCPPort = _ttxoi(argv[++i]);
        }
        else if (0 == _tcsxicmp("-hp",argv[i]))
        {
            pSt_StartlParam->nHttpPort = _ttxoi(argv[++i]);
        }
		else if (0 == _tcsxicmp("-wp", argv[i]))
		{
			pSt_StartlParam->nWSPort = _ttxoi(argv[++i]);
		}
		else if (0 == _tcsxicmp("-mp", argv[i]))
		{
			pSt_StartlParam->nMQTTPort = _ttxoi(argv[++i]);
		}
        else if (0 == _tcsxicmp("-d",argv[i]))
        {
            pSt_StartlParam->bDeamon = _ttxoi(argv[++i]);
        }
		else if (0 == _tcsxicmp("-t", argv[i]))
		{
            bIsTest = true;
		}
		else if (0 == _tcsxicmp("-lt", argv[i]))
		{
			pSt_StartlParam->st_XLog.nLogType = _ttxoi(argv[++i]);
		}
		else if (0 == _tcsxcmp("-l", argv[i]))
		{
			LPCXSTR lpszLogLevel = argv[++i];
			if (0 == _tcsxcmp("debug", lpszLogLevel))
			{
				pSt_StartlParam->st_XLog.nLogLeave = XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_DETAIL;
			}
			else if (0 == _tcsxcmp("detail", lpszLogLevel))
			{
				pSt_StartlParam->st_XLog.nLogLeave = XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_DETAIL;
			}
			else if (0 == _tcsxcmp("info", lpszLogLevel))
			{
				pSt_StartlParam->st_XLog.nLogLeave = XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO;
			}
		}
    }

    return true;
}

void MQ_Service_ParamentHelp()
{
    printf(_X("--------------------------启动参数帮助开始--------------------------\n"));
    printf(_X("网络消息队列服务启动参数：程序 参数 参数值，参数是区分大小写的。如果不指定将会加载默认的ini配置文件里面的参数\n"));
    printf(_X("-h or -H：启动参数帮助提示信息\n"));
    printf(_X("-TP：设置消息队列TCP服务端口号\n"));
    printf(_X("-HP：设置消息队列HTTP服务端口号\n"));
    printf(_X("-d：1 启用守护进程，2不启用\n"));
    printf(_X("--------------------------启动参数帮助结束--------------------------\n"));
}
