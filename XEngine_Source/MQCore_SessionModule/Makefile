CC = g++ -Wall -std=c++17 -fPIC
RELEASE = 0
UNICODE = 0
INCLUDE = -I ./
LOADBIN = 
LIB = 
LIBEX = -static-libgcc -lrt -ldl -lpthread
LOADSO = -Wl,-rpath=
LIBINCLUDE = SessionModule_Client.o SessionModule_Notify.o pch.o

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
	$(CC) $(DEBUG) $(LIBINCLUDE) -o libMQCore_SessionModule.so -shared -fPIC $(LOADBIN) $(LIB) $(LIBEX) $(LOADSO)

SessionModule_Client.o:./SessionModule_Client/SessionModule_Client.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./SessionModule_Client/SessionModule_Client.cpp 
SessionModule_Notify.o:./SessionModule_Notify/SessionModule_Notify.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./SessionModule_Notify/SessionModule_Notify.cpp

pch.o:./pch.cpp                 		       	    
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_SessionModule.so
	cp ./libMQCore_SessionModule.so ../../XEngine_Release/libMQCore_SessionModule.so


CleanAll:CleanObj CleanMk
CleanObj:                                               
	rm *.o
CleanMk:                             
	rm *.so                        
