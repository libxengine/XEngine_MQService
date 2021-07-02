CC = g++ -Wall -std=c++17 -fPIC
RELEASE = 1
UNICODE = 0
LOADBIN = 
LIB = 
INCLUDE = -I ./
FILECENTOS = /etc/redhat-release
LIBEX = -static-libgcc -lrt -ldl -lpthread
LOADSO = -Wl,-rpath=./,--disable-new-dtags
LIBINCLUDE = XMQModule_Packet.o pch.o

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
	$(CC) $(DEBUG) $(LIBINCLUDE) -o libMQCore_XMQModule.so -shared -fPIC $(LOADBIN) $(LIB) $(LIBEX) $(LOADSO)


XMQModule_Packet.o:./XMQModule_Packet/XMQModule_Packet.cpp                         
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./XMQModule_Packet/XMQModule_Packet.cpp
	
pch.o:./pch.cpp                          
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_XMQModule.so
	cp ./libMQCore_XMQModule.so ../../XEngine_Release/libMQCore_XMQModule.so

CleanAll:CleanObj CleanMk
CleanObj:
	rm *.o                                           
CleanMk:
	rm *.so                                         
	
