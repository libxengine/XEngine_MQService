RELEASE = 0
UNICODE = 0
PLATFORM=linux
FLAGS= 
#要编译的模块
BASE_THIRDPART_JSONCPP = ./XEngine_ThirdPart/jsoncpp

BASE_CONFIG_PATH = ./MQCore_ConfigModule
BASE_PROTOCOL_PATH = ./MQCore_ProtocolModule
BASE_DATABASE_PATH = ./MQCore_DBModule
BASE_SESSION_PATH = ./MQCore_SessionModule

APP_SERVICE_PATH = ./XEngine_MQServiceApp

XENGINE_MODULES = libjsoncpp.so               \
                libMQCore_ConfigModule.so libMQCore_ProtocolModule.so libMQCore_SessionModule.so libMQCore_DBModule.so     \
		        XEngine_MQServiceApp.exe 

.PHONY:MakeAll 
MakeAll:$(XENGINE_MODULES)

libjsoncpp.so:
	make -C $(BASE_THIRDPART_JSONCPP) PLATFORM=$(PLATFORM) $(FLAGS)

libMQCore_ConfigModule.so:
	make -C $(BASE_CONFIG_PATH) PLATFORM=$(PLATFORM) $(FLAGS)
libMQCore_ProtocolModule.so:
	make -C $(BASE_PROTOCOL_PATH) PLATFORM=$(PLATFORM) $(FLAGS)
libMQCore_SessionModule.so:
	make -C $(BASE_SESSION_PATH) PLATFORM=$(PLATFORM) $(FLAGS)
libMQCore_DBModule.so:
	make -C $(BASE_DATABASE_PATH) PLATFORM=$(PLATFORM) $(FLAGS)

XEngine_MQServiceApp.exe:
	make -C $(APP_SERVICE_PATH) PLATFORM=$(PLATFORM) $(FLAGS)
	
	
	
