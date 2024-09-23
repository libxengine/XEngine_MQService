RELEASE = 0
UNICODE = 0
PLATFORM=linux
FLAGS= 
#要编译的模块
BASE_THIRDPART_JSONCPP = ./XEngine_Depend/XEngine_Module/jsoncpp
BASE_THIRDPART_REPORT = ./XEngine_Depend/XEngine_Module/XEngine_InfoReport

BASE_CONFIG_PATH = ./MQCore_ConfigModule
BASE_PROTOCOL_PATH = ./MQCore_ProtocolModule
BASE_DATABASE_PATH = ./MQCore_DBModule
BASE_SESSION_PATH = ./MQCore_SessionModule

APP_SERVICE_PATH = ./XEngine_MQServiceApp

ifeq ($(PLATFORM),linux)
	FILEEXT = so
else ifeq ($(PLATFORM),mac)
	FILEEXT = dylib
endif

XENGINE_MODULES = libjsoncpp.so libXEngine_InfoReport.so          \
                libMQCore_ConfigModule.so libMQCore_ProtocolModule.so libMQCore_SessionModule.so libMQCore_DBModule.so     \
		        XEngine_MQServiceApp.exe 

.PHONY:MakeAll 
MakeAll:$(XENGINE_MODULES)


libjsoncpp.so:
ifeq ($(FLAGS), InstallAll)
	cp $(BASE_THIRDPART_JSONCPP)/libjsoncpp.$(FILEEXT) ../XEngine_Release/
else
	make -C $(BASE_THIRDPART_JSONCPP) PLATFORM=$(PLATFORM) $(FLAGS)
endif
libXEngine_InfoReport.so:
ifeq ($(FLAGS), InstallAll)
	cp $(BASE_THIRDPART_REPORT)/libXEngine_InfoReport.$(FILEEXT) ../XEngine_Release/
else
	make -C $(BASE_THIRDPART_REPORT) PLATFORM=$(PLATFORM) $(FLAGS)
endif

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