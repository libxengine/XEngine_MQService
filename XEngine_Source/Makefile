RELEASE = 0
UNICODE = 0

FLAGS = RELEASE=$(RELEASE)

BASE_CONFIG_PATH = ./MQCore_ConfigModule
BASE_MQCORE_PATH = ./MQCore_XMQModule

APP_SERVICE_PATH = ./XEngine_MQServiceApp

NETENGINE_LIB = libMQCore_ConfigModule.so libMQCore_XMQModule.so                 \
		 XEngine_MQServiceApp.exe 

.PHONY:MakeAll 
MakeAll:$(NETENGINE_LIB)

libMQCore_ConfigModule.so:
	make -C $(BASE_CONFIG_PATH) $(FLAGS)
libMQCore_XMQModule.so:
	make -C $(BASE_MQCORE_PATH) $(FLAGS)

XEngine_MQServiceApp.exe:
	make -C $(APP_SERVICE_PATH) $(FLAGS)


SetFlags_Install:
	export FLAGS=InstallAll
InstallAll:SetFlags_Install $(NETENGINE_LIB) 
 
SetFlags_Clean:
	export FLAGS=CleanAll
CleanAll:SetFlags_Clean $(NETENGINE_LIB) 
	
	
	
