CC = g++ -Wall -std=c++20 -fPIC
PLATFORM = linux
PLATVER = 
PLATDIR = 
FILEEXT = 
LIBFLAG = 
RELEASE = 0
LOADHDR = -I ./ -I ../XEngine_Depend/XEngine_Module/jsoncpp
LOADSO = -L ../XEngine_Depend/XEngine_Module/jsoncpp
LIB = -ljsoncpp -lXEngine_BaseLib -lXEngine_Cryption -lRfcComponents_MQTTProtocol
LIBEX = 
OBJECTS = ProtocolModule_Packet.o ProtocolModule_Parse.o pch.o

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
	FILEEXT = so
	LIBFLAG = -shared
	LIBEX = -lpthread -lrt -ldl
	LOADBIN = -Wl,-rpath=./,--disable-new-dtags
else ifeq ($(PLATFORM),mac)
	CC += -Wno-deprecated-declarations
	PLATVER = -D __MACOS__
	PLATDIR = XEngine_Mac
	FILEEXT = dylib
	LIBFLAG = -dynamiclib
	LIBEX = -lpthread -ldl
	LOADBIN = -Wl,-rpath,@loader_path/./
endif


all:$(OBJECTS)
	$(CC) $(DEBUG) $(OBJECTS) -o libMQCore_ProtocolModule.$(FILEEXT) $(LIBFLAG) $(LIB) $(LIBEX) $(LOADSO) $(LOADBIN)

ProtocolModule_Packet.o:./ProtocolModule_Packet/ProtocolModule_Packet.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./ProtocolModule_Packet/ProtocolModule_Packet.cpp 
ProtocolModule_Parse.o:./ProtocolModule_Parse/ProtocolModule_Parse.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./ProtocolModule_Parse/ProtocolModule_Parse.cpp 
							              
pch.o:./pch.cpp                 		       	    
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_ProtocolModule.$(FILEEXT)
	cp ./libMQCore_ProtocolModule.$(FILEEXT) ../../XEngine_Release/libMQCore_ProtocolModule.$(FILEEXT)


CleanAll:CleanObj CleanMk
CleanObj:                                               
	rm *.o
CleanMk:                             
	rm *.$(FILEEXT)                        
