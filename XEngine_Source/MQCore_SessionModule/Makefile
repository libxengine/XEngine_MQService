CC = g++ -Wall -std=c++17 -fPIC
PLATFORM = linux
PLATVER = 
PLATDIR = 
FILEEXT = 
LIBFLAG = 
RELEASE = 0
LOADHDR = -I ./ 
LOADSO = 
LIB = -lXEngine_BaseLib
LIBEX = 
OBJECTS = SessionModule_Client.o pch.o

ifeq ($(RELEASE),1)
FLAGS = -c 
DEBUG = 
else
FLAGS = -c -lc_p
DEBUG = -g -pg 
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
	$(CC) $(DEBUG) $(OBJECTS) -o libMQCore_SessionModule.$(FILEEXT) $(LIBFLAG) $(LIB) $(LIBEX) $(LOADSO) $(LOADBIN)

SessionModule_Client.o:./SessionModule_Client/SessionModule_Client.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./SessionModule_Client/SessionModule_Client.cpp 

pch.o:./pch.cpp                 		       	    
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_SessionModule.$(FILEEXT)
	cp ./libMQCore_SessionModule.$(FILEEXT) ../../XEngine_Release/libMQCore_SessionModule.$(FILEEXT)


CleanAll:CleanObj CleanMk
CleanObj:                                               
	rm *.o
CleanMk:                             
	rm *.$(FILEEXT)                        
