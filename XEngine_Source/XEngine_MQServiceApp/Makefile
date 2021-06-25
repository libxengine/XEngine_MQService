CC = g++ -Wall
RELEASE = 0
UNICODE = 0  
LOADBIN = -L ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_BaseLib -L ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_Core -L ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_HelpComponents -L ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_MQCore
LIB = -lXEngine_BaseLib -lXEngine_Algorithm -lXEngine_Core -lXEngine_ManagePool -lHelpComponents_XLog -lHelpComponents_Packets -lMQCore_XMQService 
LIBEX = -static-libgcc -ldl -lrt -lpthread
LOADSO = -Wl,-rpath=./,--disable-new-dtags
LIBINCLUDE = MQService_Net.o MQService_Config.o MQService_Task.o XEngine_MQService.o

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
MQService_Task.o:./MQService_Task.cpp
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./MQService_Task.cpp
XEngine_MQService.o:./XEngine_MQService.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./XEngine_MQService.cpp   


InstallAll:InstallApp
InstallApp:./XEngine_MQServiceApp
	cp -f ./XEngine_MQServiceApp ./XEngine_MQService/XEngine_MQServiceApp
InstallSo:
	cp -f ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_BaseLib/libXEngine_BaseLib.so ./XEngine_MQService/libXEngine_BaseLib.so
	cp -f ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_BaseLib/libXEngine_Algorithm.so ./XEngine_MQService/libXEngine_Algorithm.so
	cp -f ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_Core/libXEngine_Core.so ./XEngine_MQService/libXEngine_Core.so
	cp -f ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_Core/libXEngine_ManagePool.so ./XEngine_MQService/libXEngine_ManagePool.so
	cp -f ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_HelpComponents/libHelpComponents_XLog.so ./XEngine_MQService/libHelpComponents_XLog.so
	cp -f ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_HelpComponents/libHelpComponents_Packets.so ./XEngine_MQService/libHelpComponents_Packets.so
	cp -f ../../XEngine/XEngine_Release/XEngine_Linux/Ubuntu/XEngine_MQCore/libMQCore_XMQService.so ./XEngine_MQService/libMQCore_XMQService.so

CleanAll:CleanObj CleanMk
CleanObj:                                              
	rm *.o
CleanMk:                                  
	rm XEngine_MQServiceApp     
CleanSo:
	rm ./XEngine_MQService/XEngine_MQServiceApp
	       
