CC = g++ -Wall -std=c++17 -fPIC
RELEASE = 1
UNICODE = 0
LOADBIN = 
LIB = 
INCLUDE = -I ./
FILECENTOS = /etc/redhat-release
LIBEX = -static-libgcc -lrt -ldl -lpthread
LOADSO = -Wl,-rpath=./,--disable-new-dtags
LIBINCLUDE = XMQService_Packet.o XMQService_Protocol.o XMQService_User.o pch.o

ifeq ($(RELEASE),1)
FLAGS = -c -O2 
DEBUG = 
else
FLAGS = -c -lc_p
DEBUG = -g -pg 
endif

ifeq ($(UNICODE),1)
UNICODE = -D _UNICODE
else
UNICODE = 
endif

all:$(LIBINCLUDE)
	$(CC) $(DEBUG) $(LIBINCLUDE) -o libMQCore_XMQService.so -shared -fPIC $(LOADBIN) $(LIB) $(LIBEX) $(LOADSO)


XMQService_Packet.o:./XMQService_Packet/XMQService_Packet.cpp                         
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./XMQService_Packet/XMQService_Packet.cpp
XMQService_Protocol.o:./XMQService_Protocol/XMQService_Protocol.cpp                         
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./XMQService_Protocol/XMQService_Protocol.cpp
XMQService_User.o:./XMQService_User/XMQService_User.cpp                         
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./XMQService_User/XMQService_User.cpp
	
pch.o:./pch.cpp                          
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./pch.cpp


InstallAll:InstallSo InstallHeader
InstallSo:./libMQCore_XMQService.so
ifeq ($(FILECENTOS), $(wildcard $(FILECENTOS)))
	cp ./libMQCore_XMQService.so ../../../XEngine_Release/XEngine_Linux/Centos/XEngine_MQCore/libMQCore_XMQService.so
else
	cp ./libMQCore_XMQService.so ../../../XEngine_Release/XEngine_Linux/Ubuntu/XEngine_MQCore/libMQCore_XMQService.so
endif
InstallHeader:./XMQService_Define.h ./XMQService_Error.h
	cp ./XMQService_Define.h ../../../XEngine_Release/XEngine_Include/XEngine_MQCore/XMQService_Define.h
	cp ./XMQService_Error.h ../../../XEngine_Release/XEngine_Include/XEngine_MQCore/XMQService_Error.h

CleanAll:CleanObj CleanMk
CleanObj:
	rm *.o                                           
CleanMk:
	rm *.so                                         
	
