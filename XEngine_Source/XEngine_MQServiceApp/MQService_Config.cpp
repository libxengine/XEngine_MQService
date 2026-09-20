#include "MQService_Hdr.h"

// 解析并应用服务启动参数。
// 处理流程：
// 1) 先加载默认 JSON 配置与版本配置；
// 2) 再按命令行参数覆盖对应字段；
// 3) 遇到帮助/版本参数时输出信息并返回 false（表示不继续启动）。
bool MQ_Service_Parament(int argc, char** argv, XENGINE_SERVERCONFIG* pSt_StartlParam)
{
    // 默认配置文件路径（未通过命令行指定时使用）
    LPCXSTR lpszConfigFile = _X("./XEngine_Config/XEngine_Config.json");
    LPCXSTR lpszVersionFile = _X("./XEngine_Config/XEngine_VerConfig.json");

    // 加载基础服务配置
	if (!Config_Json_File(lpszConfigFile, pSt_StartlParam))
	{
		printf("解析配置文件失败,Config_Json_File:%lX\n", Config_GetLastError());
		return false;
	}
    // 加载版本相关配置
	if (!Config_Json_VersionFile(lpszVersionFile, pSt_StartlParam))
	{
		printf("解析配置文件失败,Config_Json_VerFile:%lX\n", Config_GetLastError());
		return false;
	}

    // 遍历命令行参数并覆盖默认配置
    for (int i = 0;i < argc;i++)
    {
        if (0 == _tcsxcmp("-h",argv[i]))
        {
            MQ_Service_ParamentHelp();
            return false;
        }
        else if (0 == _tcsxcmp("-v",argv[i]))
        {
            printf("Version：%s\n", st_ServiceCfg.st_XVer.pStl_ListStorage->front().c_str());
            return false;
        }
        else if (0 == _tcsxcmp("-tp",argv[i]))
        {
            // -tp 需要一个紧随其后的端口值参数
			if (i + 1 >= argc)
			{
				MQ_Service_ParamentHelp();
				return false;
			}
            // 消费下一个 argv 作为 TCP 端口
            pSt_StartlParam->nTCPPort = _ttxoi(argv[++i]);
        }
        else if (0 == _tcsxcmp("-hp",argv[i]))
        {
			if (i + 1 >= argc)
			{
				MQ_Service_ParamentHelp();
				return false;
			}
            pSt_StartlParam->nHttpPort = _ttxoi(argv[++i]);
        }
		else if (0 == _tcsxcmp("-wp", argv[i]))
		{
			if (i + 1 >= argc)
			{
				MQ_Service_ParamentHelp();
				return false;
			}
			pSt_StartlParam->nWSPort = _ttxoi(argv[++i]);
		}
		else if (0 == _tcsxcmp("-mp", argv[i]))
		{
			if (i + 1 >= argc)
			{
				MQ_Service_ParamentHelp();
				return false;
			}
			pSt_StartlParam->nMQTTPort = _ttxoi(argv[++i]);
		}
        else if (0 == _tcsxcmp("-d",argv[i]))
        {
			if (i + 1 >= argc)
			{
				MQ_Service_ParamentHelp();
				return false;
			}
            pSt_StartlParam->bDeamon = _ttxoi(argv[++i]);
        }
		else if (0 == _tcsxcmp("-t", argv[i]))
		{
            bIsTest = true;
		}
		else if (0 == _tcsxcmp("-lt", argv[i]))
		{
			pSt_StartlParam->st_XLog.nLogType = _ttxoi(argv[++i]);
		}
		else if (0 == _tcsxcmp("-l", argv[i]))
		{
			if (i + 1 >= argc)
			{
				MQ_Service_ParamentHelp();
				return false;
			}
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

    // 参数解析完成且未触发帮助/版本提前退出，可继续启动服务
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
