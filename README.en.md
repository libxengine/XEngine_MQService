[中文](README.md) ||  [English](README.en.md)  
# XEngine_MQService
This repository has a development and master branch. If you want to use it, please use the master branch  

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
15. data distribution service(DDS),Support WAN and LAN

## install

#### Requirements  
support system above windows 7sp1 and linux(ubuntu20.04,centos8)  
XEngine need V7.23 or above  
vcpkg need 2021.05.11 or above  

#### Windows
use vs2019 x86(debug or release) x64(only release) open and complie  
You need to configure the environment in the following way, otherwise you may need to set the library directory in the project yourself  

##### install Dependent library
vcpkg:https://github.com/microsoft/vcpkg   
vcpkg.exe install jsoncpp  
vcpkg integrate install  
you can not use vcpkg and config jsoncpp development for youself   

#### Linux
Linux use Makefile to complie  
supproted ubuntu x64 or centos x64     

##### install Dependent library
install jsoncpp to your system  
ubuntu20.04  
sudo apt install libjsoncpp-devel  
Centos8.x  
sudo dnf install jsoncpp-devel  

##### XEngine Install
you can install xengine env to your system by shell   
like this:sudo XEngine_RunEnv.sh -i 3  
##### complie
execute command in XEngine_Source path
make complie  
make FLAGS=InstallAll install  
make FLAGS=CleanAll clear  

#### XEngine Env
install XEngine can be refer to xengine Readme docment  
GITEE:https://gitee.com/xyry/libxengine  
GITHUB:https://github.com/libxengine/xengine  

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

## other problems   
You can refer to the document under the docment directory. It contains API protocol and service description.  

## Test Server Address
Address:app.xyry.org or 159.75.200.173  
Port:TCP 5200,HTTP 5201,WEBSOCKET 5202  

## Participate in contribution

1. Fork this code
2. Create new Feat_xxx branch
3. Submit the code
4. New Pull Request

## Follow us
If you think this software is helpful to you, please give us a START

## post issues

if you have eny quest.post issues...
