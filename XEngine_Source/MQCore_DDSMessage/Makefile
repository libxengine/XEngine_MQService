CC = g++ -Wall -std=c++17 -fPIC
RELEASE = 0
UNICODE = 0
INCLUDE = -I ./ -I ../XEngine_ThirdPart/jsoncpp
FILECENTOS = /etc/redhat-release
LOADBIN = -L /usr/local/lib/XEngine_Release/XEngine_BaseLib -L ../XEngine_ThirdPart/jsoncpp
LIB = -lXEngine_BaseLib 
LIBEX = -static-libgcc -lrt -ldl -lpthread
LOADSO = -Wl,-rpath=./,--disable-new-dtags
LIBINCLUDE = DDSMessage_CommApi.o pch.o

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

ifeq ($(FILECENTOS), $(wildcard $(FILECENTOS)))
	UNICODE = -D _CENTOS
else
	UNICODE = -D _UBUNTU
endif

all:$(LIBINCLUDE)
	$(CC) $(DEBUG) $(LIBINCLUDE) -o libMQCore_DDSMessage.so -shared -fPIC $(LOADBIN) $(LIB) $(LIBEX) $(LOADSO)

DDSMessage_CommApi.o:./DDSMessage_CommApi/DDSMessage_CommApi.cpp                         
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./DDSMessage_CommApi/DDSMessage_CommApi.cpp

pch.o:./pch.cpp                          
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_DDSMessage.so
	cp ./libMQCore_DDSMessage.so ../../XEngine_Release/libMQCore_DDSMessage.so


CleanAll:CleanObj CleanMk
CleanObj:
	rm *.o                                           
CleanMk:
	rm *.so                                         
	
