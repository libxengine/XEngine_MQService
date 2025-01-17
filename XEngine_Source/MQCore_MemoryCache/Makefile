CC = g++ -Wall -std=c++20 -fPIC
PLATFORM = linux
PLATVER = 
PLATDIR = 
FILEEXT = 
LIBFLAG = 
RELEASE = 0
LOADHDR = -I ./
LOADSO = 
LIB = -lHelpComponents_DataBase
LIBEX = 
OBJECTS = MemoryCache_DBData.o MemoryCache_DBUser.o pch.o

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
	$(CC) $(DEBUG) $(OBJECTS) -o libMQCore_MemoryCache.$(FILEEXT) $(LIBFLAG) $(LIB) $(LIBEX) $(LOADSO) $(LOADBIN)

MemoryCache_DBData.o:./MemoryCache_DBData/MemoryCache_DBData.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MemoryCache_DBData/MemoryCache_DBData.cpp 
MemoryCache_DBUser.o:./MemoryCache_DBUser/MemoryCache_DBUser.cpp             
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./MemoryCache_DBUser/MemoryCache_DBUser.cpp 

pch.o:./pch.cpp                 		       	    
	$(CC) $(DEBUG) $(FLAGS) $(PLATVER) $(LOADHDR) ./pch.cpp


InstallAll:InstallSo 
InstallSo:./libMQCore_MemoryCache.$(FILEEXT)
	cp ./libMQCore_MemoryCache.$(FILEEXT) ../../XEngine_Release/libMQCore_MemoryCache.$(FILEEXT)


CleanAll:CleanObj CleanMk
CleanObj:                                               
	rm *.o
CleanMk:                             
	rm *.$(FILEEXT)                        