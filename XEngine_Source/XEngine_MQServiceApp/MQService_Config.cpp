#include "MQService_Hdr.h"

bool MQ_Service_Parament(int argc, char** argv, XENGINE_SERVERCONFIG* pSt_StartlParam)
{
    LPCXSTR lpszConfigFile = _T("./XEngine_Config/XEngine_Config.json");

	if (!Config_Json_File(lpszConfigFile, pSt_StartlParam))
	{
		printf("解析配置文件失败,Config_Json_File:%lX\n", Config_GetLastError());
		return false;
	}

    for (int i = 0;i < argc;i++)
    {
        if ((0 == _tcscmp("-h",argv[i])) || (0 == _tcscmp("-H",argv[i])))
        {
            MQ_Service_ParamentHelp();
            return false;
        }
        if ((0 == _tcscmp("-v",argv[i])) || (0 == _tcscmp("-V",argv[i])))
        {
            printf("Version：V1.1.0\n");
            return false;
        }
        else if (0 == _tcscmp("-TP",argv[i]))
        {
            pSt_StartlParam->nTCPPort = _ttoi(argv[i + 1]);
        }
        else if (0 == _tcscmp("-HP",argv[i]))
        {
            pSt_StartlParam->nHttpPort = _ttoi(argv[i + 1]);
        }
        else if (0 == _tcscmp("-d",argv[i]))
        {
            pSt_StartlParam->bDeamon = _ttoi(argv[i + 1]);
        }
    }

    return true;
}

void MQ_Service_ParamentHelp()
{
    printf(_T("--------------------------启动参数帮助开始--------------------------\n"));
    printf(_T("网络消息队列服务启动参数：程序 参数 参数值，参数是区分大小写的。如果不指定将会加载默认的ini配置文件里面的参数\n"));
    printf(_T("-h or -H：启动参数帮助提示信息\n"));
    printf(_T("-TP：设置消息队列TCP服务端口号\n"));
    printf(_T("-HP：设置消息队列HTTP服务端口号\n"));
    printf(_T("-d：1 启用守护进程，2不启用\n"));
    printf(_T("--------------------------启动参数帮助结束--------------------------\n"));
}
