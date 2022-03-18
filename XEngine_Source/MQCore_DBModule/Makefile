CC = g++ -Wall -std=c++17 -fPIC
RELEASE = 0
UNICODE = 0
INCLUDE = -I ./ 
LOADBIN = -L /usr/local/lib/XEngine_Release/XEngine_HelpComponents
LIB = -lHelpComponents_DataBase
LIBEX = -static-libgcc -lrt -ldl -lpthread
LOADSO = -Wl,-rpath=./,--disable-new-dtags
LIBINCLUDE = DBModule_MQData.o DBModule_MQUser.o pch.o

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
	$(CC) $(DEBUG) $(LIBINCLUDE) -o libMQCore_DBModule.so -shared -fPIC $(LOADBIN) $(LIB) $(LIBEX) $(LOADSO)

DBModule_MQData.o:./DBModule_MQData/DBModule_MQData.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./DBModule_MQData/DBModule_MQData.cpp 
DBModule_MQUser.o:./DBModule_MQUser/DBModule_MQUser.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./DBModule_MQUser/DBModule_MQUser.cpp 

pch.o:./pch.cpp                 		       	    
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_DBModule.so
	cp ./libMQCore_DBModule.so ../../XEngine_Release/libMQCore_DBModule.so


CleanAll:CleanObj CleanMk
CleanObj:                                               
	rm *.o
CleanMk:                             
	rm *.so                        