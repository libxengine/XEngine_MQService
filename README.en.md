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
the software mainly provides a message queue and dds engine,support http,WEBSOCKET,MQTT and private protocol api server  

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
11. support multi protocol(TCP,WEBSOCKET,MQTT)  
12. Active delivery acquisition mode or passive subscription notification module  
13. get order and start serial pos setting  
14. Unlimited load message types  
15. timed message
16. Access control(planning)
17. binary message
18. http interface management
19. message attritube
20. point to point message
21. Memory Database

## install

#### XEngine Evn
you must install XEngine,need V8.31 or above,install XEngine can be refer to xengine Readme docment  
GITEE:https://gitee.com/libxengine/libxengine  
GITHUB:https://github.com/libxengine/libxengine

##### fast to deployment 
git clone https://gitee.com/libxengine/libxengine.git or git clone https://github.com/libxengine/libxengine.git  
window Exection XEngine_WINEnv.bat   
Linux Exection:sudo ./XEngine_LINEnv.sh -i 3  
Macos Exection:./XEngine_LINEnv.sh -i 3

##### Depends on environment
Requires MYSQL service support, execute XEngine_SQLFile/CreateDatabase.sql to create database  
Execute XEngine_SQLFile/XEngine_MQData.sql and XEngine_SQLFile/XEngine_MQUser.sql to create the table  

#### sub module
Due to the dependent sub-modules, after you checkout the warehouse, execute the following command in the warehouse directory to pull the sub-modules  
git submodule init  
git submodule update  

#### Windows
use vs open and complie,support windows 7sp1 and above   
Just Run it.

#### Linux
Linux use Makefile to complie  
supproted ubuntu22.04 x64 or RockyLinux9 x64  

###### complie
execute command in XEngine_Source path
make complie  
make FLAGS=InstallAll install  
make FLAGS=CleanAll clear  
If there is no error.you can see complied XEngine_MQServiceApp file in XEngine_Release  
You can running at terminal.

#### MacOS
Linux use Makefile to complie,terminal exection,Requires mac 13 and above  
###### complie
same linux

## Installation Instructions
#### Install
Modify the configuration file to what you need, and then run the service directly

#### upgrade
If it is an upgrade installation, after downloading the new version and modifying the configuration file, it can be run directly
database you need to open the database matching fields and tables are consistent

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
Address:app.xyry.org Port:TCP 5200,WEBSOCKET 5201,HTTP Management Interface 5202  

## Participate in contribution

1. Fork this code
2. Create new Feat_xxx branch
3. Submit the code
4. New Pull Request
5. Submit develop branch

## Follow us
If you think this software is helpful to you, please give us a START

## post issues

if you have eny quest.post issues...
