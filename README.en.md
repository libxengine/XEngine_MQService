[中文](README.md) ||  [English](README.en.md)  
# XEngine_MQService
This repository has a development and master branch. If you want to use it, please use the master branch  
As long as the repository is not in suspended state, we will maintain and develop it all the time, please use it with confidence

#### Description
c c++消息服务 消息中间件
c c++Message Queue Service

#### Software Architecture
the software developed based on xengine,using c/c++ development  
the software is a simple and high performance server,Allow users to use it directly without secondary development  
the software mainly provides a message queue and dds engine,support http,WEBSOCKET and private protocol api server  

## Software features
this software support following features  
1. message serial  
2. Permanent message(planning)  
3. message timeout  
4. topic Partition  
5. message tracking(planning)  
6. encrypt Communication(planning)  
7. load balanc(planning)  
8. message get time  
9. message subcribe  
10. support Permission Validation  
11. support multi protocol(TCP,HTTP,WEBSOCKET)  
12. Active delivery acquisition mode or passive subscription notification module  
13. get order and start serial pos setting  
14. Unlimited load message types  
15. timed message
16. Access control(planning)

## install

#### XEngine Evn
you must install XEngine,need V7.46 or above,install XEngine can be refer to xengine Readme docment  
GITEE:https://gitee.com/xyry/libxengine  
GITHUB:https://github.com/libxengine/xengine

##### fast to deployment 
git clone https://gitee.com/xyry/libxengine.git or git clone https://github.com/libxengine/xengine.git  
window Exection XEngine_WINEnv.bat   
Linux Exection:sudo ./XEngine_LINEnv.sh -i 3  
Macos Exection:./XEngine_LINEnv.sh -i 3

##### dependent environment
need to mysql service support,exection XEngine_SQLFile/CreateDatabase.sql to create database  
and exection:XEngine_SQLFile/XEngine_MQData.sql and XEngine_SQLFile/XEngine_MQUser.sql to create table  

#### Windows
use vs open and complie,support windows 7sp1 and above   
Just Run it.

#### Linux
Linux use Makefile to complie  
supproted ubuntu x64 or centos x64  

#### MacOS
Linux use Makefile to complie,terminal exection,Requires mac 12 and above  

##### complie
execute command in XEngine_Source path
make complie  
make FLAGS=InstallAll install  
make FLAGS=CleanAll clear  

#### use

1.  Switch to the MASTER branch
2.  download code
3.  complie
4.  install
5.  run
6.  run example code  

## directory struct
- XEngine_Docment  docment directory  
- XEngine_Release  install directory  
- XEngine_Source   code    directory  
- XEngine_Apps     Client code directory 
- XEngine_SQLFile  database file back dir 

## other problems   
You can refer to the document under the docment directory. It contains API protocol and service description.  

## Test Server Address
Address:app.xyry.org Port:TCP 5200,HTTP 5201,WEBSOCKET 5202  

## Participate in contribution

1. Fork this code
2. Create new Feat_xxx branch
3. Submit the code
4. New Pull Request

## Follow us
If you think this software is helpful to you, please give us a START

## post issues

if you have eny quest.post issues...
