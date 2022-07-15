CC = g++ -Wall -std=c++17 -fPIC
SHAREFLAG = -shared
SHAREDLL = so
RELEASE = 0
UNICODE = 0
INCLUDE = -I ./
LOADBIN = 
LIB = 
LIBEX = -ldl -lpthread
LOADSO = -Wl,-rpath=
LIBINCLUDE = SessionModule_Client.o SessionModule_Notify.o pch.o

ifeq ($(shell uname),Darwin)
	SHAREDLL = dylib
	SHAREFLAG = -dynamiclib
	LOADSO = 
else
	SHAREDLL = so
	SHAREFLAG = -shared
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
	$(CC) $(DEBUG) $(LIBINCLUDE) -o libMQCore_SessionModule.$(SHAREDLL) $(SHAREFLAG) -fPIC $(LOADBIN) $(LIB) $(LIBEX) $(LOADSO)

SessionModule_Client.o:./SessionModule_Client/SessionModule_Client.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./SessionModule_Client/SessionModule_Client.cpp 
SessionModule_Notify.o:./SessionModule_Notify/SessionModule_Notify.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./SessionModule_Notify/SessionModule_Notify.cpp

pch.o:./pch.cpp                 		       	    
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_SessionModule.$(SHAREDLL)
	cp ./libMQCore_SessionModule.$(SHAREDLL) ../../XEngine_Release/libMQCore_SessionModule.$(SHAREDLL)


CleanAll:CleanObj CleanMk
CleanObj:                                               
	rm *.o
CleanMk:                             
	rm *.$(SHAREDLL)                        
