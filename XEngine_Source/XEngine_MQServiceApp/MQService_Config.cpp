#include "MQService_Hdr.h"

BOOL MQ_Service_Parament(int argc,char **argv,NETENGIEN_MQSERVICECFG *pSt_StartlParam)
{
    LPCTSTR lpszAuthCfg = _T("./XEngine_Config/XEngine_Config.ini");

    pSt_StartlParam->nDeamon = GetPrivateProfileInt(_T("ServiceConfig"),_T("AutoStart"),0,lpszAuthCfg);
    pSt_StartlParam->nThreadCount = GetPrivateProfileInt(_T("ServiceConfig"), _T("ThreadCount"), 0, lpszAuthCfg);

    pSt_StartlParam->st_XMQ.nTCPPort = GetPrivateProfileInt(_T("XMQConfig"),_T("TCPPort"),5655,lpszAuthCfg);
    pSt_StartlParam->st_XMQ.nUDPPort = GetPrivateProfileInt(_T("XMQConfig"),_T("UDPPort"),5656,lpszAuthCfg);

    for (int i = 0;i < argc;i++)
    {
        if ((0 == _tcscmp("-h",argv[i])) || (0 == _tcscmp("-H",argv[i])))
        {
            MQ_Service_ParamentHelp();
            return FALSE;
        }
        if ((0 == _tcscmp("-v",argv[i])) || (0 == _tcscmp("-V",argv[i])))
        {
            printf("Version：V1.1.0\n");
            return FALSE;
        }
        else if (0 == _tcscmp("-TP",argv[i]))
        {
            pSt_StartlParam->st_XMQ.nTCPPort = _ttoi(argv[i + 1]);
        }
        else if (0 == _tcscmp("-UP",argv[i]))
        {
            pSt_StartlParam->st_XMQ.nUDPPort = _ttoi(argv[i + 1]);
        }
        else if (0 == _tcscmp("-d",argv[i]))
        {
            pSt_StartlParam->nDeamon = _ttoi(argv[i + 1]);
        }
    }

    return TRUE;
}

void MQ_Service_ParamentHelp()
{
    printf(_T("--------------------------启动参数帮助开始--------------------------\n"));
    printf(_T("网络消息队列服务启动参数：程序 参数 参数值，参数是区分大小写的。如果不指定将会加载默认的ini配置文件里面的参数\n"));
    printf(_T("-h or -H：启动参数帮助提示信息\n"));
    printf(_T("-TP：设置消息队列TCP服务端口号\n"));
    printf(_T("-UP：设置消息队列UDP服务端口号\n"));
    printf(_T("-d：1 启用守护进程，2不启用\n"));
    printf(_T("--------------------------启动参数帮助结束--------------------------\n"));
}
