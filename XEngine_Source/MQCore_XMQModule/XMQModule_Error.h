#pragma once
/********************************************************************
//    Created:     2021/06/30  14:30:55
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule\XMQModule_Error.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule
//    File Base:   XMQModule_Error
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列导出错误
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                       消息队列错误
//////////////////////////////////////////////////////////////////////////
#define ERROR_MQ_MODULE_PACKET_CREATE_PARAMENT 0x0010000              //参数错误
#define ERROR_MQ_MODULE_PACKET_CREATE_MALLOC 0x0010001                //申请内存失败
#define ERROR_MQ_MODULE_PACKET_CREATE_THREAD 0x0010002                //初始化失败，创建线程失败
#define ERROR_MQ_MODULE_PACKET_POST_PARAMENT 0x0010010                //参数错误，投递失败
#define ERROR_MQ_MODULE_PACKET_POST_MALLOC 0x0010011                  //申请内存失败，无法继续
#define ERROR_MQ_MODULE_PACKET_PUSH_MAXSIZE  0x0010020                //最大包个数，无法继续
#define ERROR_MQ_MODULE_PACKET_PUSH_NOTFOUND 0x0010021                //没有找到
#define ERROR_MQ_MODULE_PACKET_PUSH_EXIST 0x0010022                   //序列已经存在
#define ERROR_MQ_MODULE_PACKET_GET_PARAMENT 0x0010030                 //参数错误，无法继续
#define ERROR_MQ_MODULE_PACKET_GET_NOTFOUND 0x0010031                 //找不到主题
#define ERROR_MQ_MODULE_PACKET_GET_EMPTY 0x0010032                    //获取数据失败，KEY容器列表为空
#define ERROR_MQ_MODULE_PACKET_GET_NOSERIAL 0x0010033                 //没有找到指定序列号
#define ERROR_MQ_MODULE_PACKET_GET_LEN 0x0010034                      //数据长度不够
#define ERROR_MQ_MODULE_PACKET_DEL_PARAMENT 0x0010040                 //删除失败，参数错误
#define ERROR_MQ_MODULE_PACKET_DEL_NOSERIAL 0x0010041                 //序列号错误,无法继续
#define ERROR_MQ_MODULE_PACKET_DEL_NOTFOUND 0x0010042                 //没有找到主题
