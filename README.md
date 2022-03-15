[中文](README.md) ||  [English](README.en.md)  
# XEngine_MQService
本仓库有开发和主分支,如果要使用,请使用master分支下的代码  

## 介绍
c c++消息服务 消息中间件  
c c++Message Service  

## 软件架构
此软件基于XEngine开发.采用C/C++作为开发语言  
此软件是一款简易的高性能服务,统一的API接口,让用户几乎不需要二次开发就可以直接使用  
此软件主要是提供了消息队列与消息分发引擎,可用于用户消息操作的一个API服务器.支持HTTP,WEBSOCKET和私有协议.  
## 软件特性
此软件支持以下特性  
1. 消息序列  
2. 永久消息(planning)  
3. 消息超时  
4. 主题分区  
5. 消息回溯(planning)  
6. 加密通信(planning)  
7. 负载均衡(planning)  
8. 消息次数  
9. 消息订阅  
10. 支持权限验证  
11. 支持多种协议(TCP,HTTP,WEBSOCKET)  
12. 主动投递获取模式或者被动订阅通知模块  
13. 获取顺序与开始序列号设置  
14. 不限制负载的消息类型  
15. 消息分发服务(DDS),支持广域网和局域网

## 安装教程

#### 版本需求
支持WINDOWS 7SP1和LINUX(UBUNT20.04,CENTOS8)以上系统  
XEngine版本需要V7.23或者以上版本  

#### Windows
使用VS打开并且编译
你需要按照下面的方式配置环境,不然你可能需要自己在项目中设置库目录

#### Linux
Linux使用Makefile编译  
UBUNTU20.04 x64或者CENTOS8 x64均可  

##### 编译命令
在XEngine_Source目录下执行命令
make 编译  
make FLAGS=InstallAll 安装库程序  
make FLAGS=CleanAll 清理编译  

#### XEngine环境
安装XEngine可以参考其Readme文档  
GITEE:https://gitee.com/xyry/libxengine  
GITHUB:https://github.com/libxengine/xengine  

#### 使用说明

1.  切换到MASTER分支
2.  下载代码
3.  编译
4.  安装
5.  运行
6.  运行演示代码

## 目录结构
- XEngine_Docment  文档目录结构  
- XEngine_Release  安装目录结构  
- XEngine_Source   源代目录结构  
- XEngine_Apps     演示客户端目录  

## 其他问题  
你可以参考docment目录下的文档.里面包含了API协议和服务说明.

## 测试服务器
地址:app.xyry.org 或者 159.75.200.173  
端口:TCP 5200,HTTP 5201,WEBSOCKET 5202  

## 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request  

## 关注我们
如果你觉得这个软件对你有帮助,请你给我们一个START吧

## 提交问题

如果你有问题,可以在issues中提交