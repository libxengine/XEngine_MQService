CC = g++ -Wall -std=c++17 -fPIC
RELEASE = 0
UNICODE = 0
INCLUDE = -I ./ -I ../XEngine_ThirdPart/jsoncpp
LOADBIN = -L ../XEngine_ThirdPart/jsoncpp
LIB = -ljsoncpp
LIBEX = -static-libgcc -lrt -ldl -lpthread
LOADSO = -Wl,-rpath=./,--disable-new-dtags
LIBINCLUDE = ProtocolModule_Packet.o ProtocolModule_Parse.o pch.o

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
	$(CC) $(DEBUG) $(LIBINCLUDE) -o libMQCore_ProtocolModule.so -shared -fPIC $(LOADBIN) $(LIB) $(LIBEX) $(LOADSO)

ProtocolModule_Packet.o:./ProtocolModule_Packet/ProtocolModule_Packet.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./ProtocolModule_Packet/ProtocolModule_Packet.cpp 
ProtocolModule_Parse.o:./ProtocolModule_Parse/ProtocolModule_Parse.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./ProtocolModule_Parse/ProtocolModule_Parse.cpp 
							              
pch.o:./pch.cpp                 		       	    
	$(CC) $(DEBUG) $(FLAGS) $(UNICODE) $(INCLUDE) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_ProtocolModule.so
	cp ./libMQCore_ProtocolModule.so ../../XEngine_Release/libMQCore_ProtocolModule.so


CleanAll:CleanObj CleanMk
CleanObj:                                               
	rm *.o
CleanMk:                             
	rm *.so                        
