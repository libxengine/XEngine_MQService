CC = g++ -Wall -std=c++17
PLATFORM = linux
PLATVER = 
PLATDIR = 
RELEASE = 0
LOADHDR = -I ./ 
LOADSO = -L ../MQCore_ConfigModule -L ../MQCore_ProtocolModule -L ../MQCore_SessionModule -L ../MQCore_DBModule \
		 -L ../XEngine_Depend/XEngine_Module/jsoncpp -L ../XEngine_Depend/XEngine_Module/XEngine_InfoReport
LIB = -lXEngine_BaseLib -lXEngine_Algorithm -lXEngine_Core -lXEngine_ManagePool -lXClient_APIHelp -lXEngine_SystemApi -lHelpComponents_XLog -lHelpComponents_Packets -lRfcComponents_WSProtocol -lRfcComponents_HttpProtocol -lRfcComponents_MQTTProtocol \
      -lMQCore_ConfigModule -lMQCore_ProtocolModule -lMQCore_SessionModule -lMQCore_DBModule \
	  -ljsoncpp -lXEngine_InfoReport
LIBEX = 
OBJECTS = MQService_Net.o MQService_Config.o MQService_TCPTask.o MQService_HttpTask.o MQService_WSTask.o MQService_Task.o MQService_MQTTTask.o XEngine_MQServiceApp.o

ifeq ($(RELEASE),1)
FLAGS = -c 
DEBUG = 
else
	ifeq ($(PLATFORM),mac)
		FLAGS = -c
		DEBUG = -g 
	else
		FLAGS = -c -lc_p
		DEBUG = -g -pg 
	endif
endif

ifeq ($(PLATFORM),linux)
	ifeq (/etc/redhat-release,$(wildcard /etc/redhat-release))
		PLATVER = -D __CENTOS__
		PLATDIR = XEngine_Linux/Centos
	else
		PLATVER = -D __UBUNTU__
		PLATDIR = XEngine_Linux/Ubuntu
	endif
	LIBEX = -lpthread -lrt -ldl
	LOADBIN = -Wl,-rpath=./,--disable-new-dtags
else ifeq ($(PLATFORM),mac)
	CC += -Wno-deprecated-declarations
	PLATVER = -D __MACOS__
	PLATDIR = XEngine_Mac
	LIBEX = -lpthread -ldl
	LOADBIN = -Wl,-rpath,@loader_path/./
endif

all:$(OBJECTS)
	$(CC) $(DEBUG) $(OBJECTS) -o XEngine_MQServiceApp $(LIB) $(LIBEX) $(LOADSO) $(LOADBIN)
  
MQService_Net.o:./MQService_Net.cpp
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MQService_Net.cpp
MQService_Config.o:./MQService_Config.cpp
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MQService_Config.cpp 
MQService_TCPTask.o:./MQService_TCPTask.cpp
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MQService_TCPTask.cpp
MQService_HttpTask.o:./MQService_HttpTask.cpp
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MQService_HttpTask.cpp
MQService_WSTask.o:./MQService_WSTask.cpp
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MQService_WSTask.cpp
MQService_Task.o:./MQService_Task.cpp
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MQService_Task.cpp
MQService_MQTTTask.o:./MQService_MQTTTask.cpp
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MQService_MQTTTask.cpp

XEngine_MQServiceApp.o:./XEngine_MQServiceApp.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./XEngine_MQServiceApp.cpp   


InstallAll:InstallApp
InstallApp:./XEngine_MQServiceApp
	cp -f ./XEngine_MQServiceApp ../../XEngine_Release/XEngine_MQServiceApp

CleanAll:CleanObj CleanMk
CleanObj:                                              
	rm *.o
CleanMk:                                  
	rm XEngine_MQServiceApp     
	       
