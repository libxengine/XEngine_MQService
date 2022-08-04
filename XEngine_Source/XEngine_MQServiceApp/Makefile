CC = g++ -Wall -std=c++17
RELEASE = 0
UNICODE = 0  
LOADBIN = 0
LIB = -lXEngine_BaseLib -lXEngine_Algorithm -lXEngine_Core -lXEngine_ManagePool -lXEngine_SystemApi -lHelpComponents_XLog -lHelpComponents_Packets -lRfcComponents_WSProtocol -lRfcComponents_HttpServer \
      -lMQCore_ConfigModule -lMQCore_ProtocolModule -lMQCore_SessionModule -lMQCore_DBModule -ljsoncpp
LIBEX = -ldl -lpthread
LOADSO = -Wl,-rpath=./,--disable-new-dtags
LIBINCLUDE = MQService_Net.o MQService_Config.o MQService_TCPTask.o MQService_HttpTask.o MQService_WSTask.o MQService_Task.o XEngine_MQServiceApp.o

ifeq ($(shell uname),Darwin)
	LOADBIN = -L ../MQCore_ConfigModule -L ../MQCore_ProtocolModule -L ../MQCore_SessionModule -L ../MQCore_DBModule -L ../XEngine_ThirdPart/jsoncpp
	LOADSO = 
else
	LOADBIN = -L /usr/local/lib/XEngine_Release/XEngine_BaseLib -L /usr/local/lib/XEngine_Release/XEngine_Core -L /usr/local/lib/XEngine_Release/XEngine_SystemSdk -L /usr/local/lib/XEngine_Release/XEngine_HelpComponents -L /usr/local/lib/XEngine_Release/XEngine_RfcComponents  \
	          -L ../MQCore_ConfigModule -L ../MQCore_ProtocolModule -L ../MQCore_SessionModule -L ../MQCore_DBModule -L ../XEngine_ThirdPart/jsoncpp
endif

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
	$(CC) $(DEBUG) $(LIBINCLUDE) -o XEngine_MQServiceApp $(LOADBIN) $(LIB) $(LIBEX) $(LOADSO)
  
MQService_Net.o:./MQService_Net.cpp
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./MQService_Net.cpp
MQService_Config.o:./MQService_Config.cpp
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./MQService_Config.cpp 
MQService_TCPTask.o:./MQService_TCPTask.cpp
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./MQService_TCPTask.cpp
MQService_HttpTask.o:./MQService_HttpTask.cpp
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./MQService_HttpTask.cpp
MQService_WSTask.o:./MQService_WSTask.cpp
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./MQService_WSTask.cpp
MQService_Task.o:./MQService_Task.cpp
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./MQService_Task.cpp

XEngine_MQServiceApp.o:./XEngine_MQServiceApp.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./XEngine_MQServiceApp.cpp   


InstallAll:InstallApp
InstallApp:./XEngine_MQServiceApp
	cp -f ./XEngine_MQServiceApp ../../XEngine_Release/XEngine_MQServiceApp

CleanAll:CleanObj CleanMk
CleanObj:                                              
	rm *.o
CleanMk:                                  
	rm XEngine_MQServiceApp     
	       
