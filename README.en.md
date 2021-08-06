# XEngine_MQService

#### Description
c c++消息队列服务 消息队列中间件
c c++Message Queue Service

#### Software Architecture
the software developed based on xengine,using c/c++ development  
the software is a simple and high performance server,Allow users to use it directly without secondary development  
the software mainly provides a message queue engine,support http,WEBSOCKET and private protocol api server  

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
10. support Permission Validation(planning)  
11. support multi protocol(TCP,HTTP,WEBSOCKET)  
12. Active delivery acquisition mode or passive subscription notification module  
13. get with Reverse order(planning)  
14. Unlimited load message types  

## install

#### Requirements  
support system above windows 7sp1 and linux(ubuntu20.04,centos8)  
XEngine need V7.18 or above  
vcpkg need 2021.05.11 or above  

#### Windows
use vs2019 x86(debug or release) x64(only release) open and complie  
You need to configure the environment in the following way, otherwise you may need to set the library directory in the project yourself  

##### install Dependent library
vcpkg:https://github.com/microsoft/vcpkg   
vcpkg.exe install jsoncpp  
vcpkg integrate install  
you can not use vcpkg and config jsoncpp development for youself   

##### XEngine
XEngine can be download with mine repository,whe you downloaded xengine,you have to add value to you user environment  
- XEngine_Include header file path
- XEngine_Library library file path

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

## Participate in contribution

1. Fork this code
2. Create new Feat_xxx branch
3. Submit the code
4. New Pull Request

## Follow us
If you think this software is helpful to you, please give us a START

## post issues

if you have eny quest.post issues...
