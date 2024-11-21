[中文](README.md) ||  [English](README.en.md)  
# XEngine_MQService
本仓库有开发和主分支,如果要使用,请使用master分支下的代码  
只要仓库不是在暂停状态,那么就会有人一直维护和开发,请放心使用

## 介绍
c c++消息服务 消息中间件  
c c++Message Service  

## 软件架构
此软件基于XEngine开发.采用C/C++作为开发语言  
此软件是一款简易的高性能服务,统一的API接口,让用户几乎不需要二次开发就可以直接使用  
此软件主要是提供了消息队列与消息分发引擎,可用于用户消息操作的一个API服务器.支持HTTP,WEBSOCKET,MQTT和私有协议.  
## 软件特性
此软件支持以下特性  
1. 消息序列  
2. 永久消息  
3. 消息超时  
4. 主题分区  
5. 消息回溯
6. 加密通信(planning)  
7. 负载均衡(planning)  
8. 消息次数  
9. 消息订阅  
10. 支持权限验证  
11. 支持多种协议(TCP,WEBSOCKET,MQTT)  
12. 主动投递获取模式或者被动订阅通知模块  
13. 获取顺序与开始序列号设置  
14. 不限制负载的消息类型  
15. 定时消息
16. 权限控制(planning)
17. 二进制消息
18. http接口管理
19. 消息属性
20. 点对点消息

## 安装教程

#### XEngine环境
必须安装XEngine,版本需要V8.31或者以上版本,安装XEngine可以参考其Readme文档  
GITEE:https://gitee.com/libxengine/libxengine  
GITHUB:https://github.com/libxengine/libxengine

##### 快速部署
git clone https://gitee.com/libxengine/libxengine.git 或者 git clone https://github.com/libxengine/libxengine.git  
window执行XEngine_WINEnv.bat 脚本.  
Linux执行:sudo ./XEngine_LINEnv.sh -i 3  
macos执行:./XEngine_LINEnv.sh -i 3  

##### 依赖环境
需要MYSQL服务的支持,执行XEngine_SQLFile/CreateDatabase.sql  创建数据库  
分别执行:执行XEngine_SQLFile/XEngine_MQData.sql 和 执行XEngine_SQLFile/XEngine_MQUser.sql 创建表  

#### sub module
由于依赖的子模块,在你checkout仓库后,在仓库目录下执行下面的命令拉取子模块  
git submodule init  
git submodule update  
如果github访问失败,你也可以clone该项目,在主目录下使用命令:git clone https://gitee.com/xengine/XEngine_OPenSource.git XEngine_Source/XEngine_Depend

#### Windows
使用VS打开并且编译,支持WINDOWS 7SP1以上系统  
直接运行即可

#### Linux
Linux使用Makefile编译  
UBUNTU22.04 x64或者RockyLinux9 x64均可  
在控制台运行

###### 编译命令
在XEngine_Source目录下执行命令  
make 编译  
make FLAGS=InstallAll 安装库程序  
make FLAGS=CleanAll 清理编译  
如果没有错误,你可以在XEngine_Release目录下看到编译的XEngine_MQServiceApp  
然后直接在终端运行它即可.

#### MacOS
使用makefile编译,控制台运行,需要mac 13以及以上版本  
在控制台运行

###### 编译命令
同LINUX

## 安装指导
#### 安装
修改配置文件为你需要的,然后直接运行服务即可

#### 升级
如果是升级安装,下载新版本后修改配置文件就可以直接运行了
数据库需要打开数据库匹配字段和表是否一致

## 视频教程
你也可以观看视频教程来了解我们的代码框架情况:https://www.bilibili.com/video/BV1uH4y1a7QV

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
- XEngine_SQLFile  数据库文件备份目录  

## 其他问题  
你可以参考docment目录下的文档.里面包含了API协议和服务说明.

## 测试服务器
地址:app.xyry.org 端口:TCP 5200,WEBSOCKET 5201,管理接口HTTP 5202  

## 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request  
5.  提交到develop分支

## 关注我们
如果你觉得这个软件对你有帮助,请你给我们一个START吧

## 提交问题

如果你有问题,可以在issues中提交