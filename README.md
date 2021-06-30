# XEngine_MQService

## 介绍
c c++消息队列服务 消息队列中间件  
c c++Message Queue Service  

## 软件架构
此软件基于XEngine开发.采用C/C++作为开发语言  
此软件是一款简易的高性能服务,统一的API接口,让用户几乎不需要二次开发就可以直接使用  
此软件主要是提供了消息队列引擎,可用于用户消息操作的一个API服务器.支持HTTP和私有协议.  
## 软件特性
此软件支持以下特性  
1.消息序列  
2.永久消息(planning)  
3.消息超时  
4.主题分区  
5.消息回溯(planning)  
6.加密通信(planning)  
7.负载均衡(planning)  
7.主动投递获取模式或者被动订阅通知模块  

## 安装教程

#### 版本需求
支持WINDOWS 7SP1和LINUX(UBUNT20.04,CENTOS8)以上系统  
XEngine版本需要V7.15或者以上版本  
vcpkg 需要2021.05.11以上版本  

#### Windows
使用VS2019 x86(debug release) x64(release)打开并且编译
你需要按照下面的方式配置环境,不然你可能需要自己在项目中设置库目录
##### 三方库环境
需要使用VCPKG安装环境.代码地址:https://github.com/microsoft/vcpkg  
安装好后你可能需要把它集成到你的VS中  
然后命令安装环境: vcpkg.exe install jsoncpp   
集成VS环境: vcpkg integrate install  
vcpkg 主要为了方便安装jsoncpp,如果你想自己配置jsoncpp的环境,可以不使用vcpkg...  

##### XEngine环境
XEngine可以直接下载,下载完毕后添加用户环境变量,需要下面两个  
- XEngine_Include 头文件目录地址
- XEngine_Library 库文件目录地址

#### Linux
Linux使用Makefile编译  
UBUNTU20.04 x64或者CENTOS8 x64均可  

##### 三方库环境
基于不同系统,可能命令不同,比如在Ubuntu下面  
sudo apt install libjsoncpp-devel  
Centos8.x  
sudo dnf install jsoncpp-devel  

##### XEngine环境
XEngine可以通过脚本文件安装sudo XEngine_RunEnv.sh -c 3
##### 编译命令
在XEngine_Source目录下执行命令
make 编译  
make FLAGS=InstallAll 安装库程序  
make FLAGS=CleanAll 清理编译  

#### 使用说明

1.  切换到MASTER分支
2.  下载代码
3.  编译
4.  安装
5.  运行
6.  通过curl或postman 等三方工具测试消息投递和获取

## 目录结构
- XEngine_Docment  文档目录结构  
- XEngine_Release  安装目录结构  
- XEngine_Source   源代目录结构  
  

## 其他问题  
你可以参考docment目录下的文档.里面包含了API协议和服务说明.

## 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request  

## 提交问题

如果你有问题,可以在issues中提交