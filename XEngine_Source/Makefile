RELEASE = 0
UNICODE = 0

FLAGS = RELEASE=$(RELEASE)

BASE_THIRDPART_JSONCPP = ./XEngine_ThirdPart/jsoncpp

BASE_CONFIG_PATH = ./MQCore_ConfigModule
BASE_MQCORE_PATH = ./MQCore_XMQModule
BASE_DDSMESSAGE_PATH = ./MQCore_DDSMessage
BASE_PROTOCOL_PATH = ./MQCore_ProtocolModule
BASE_SESSION_PATH = ./MQCore_SessionModule

APP_SERVICE_PATH = ./XEngine_MQServiceApp

NETENGINE_LIB = libjsoncpp.so               \
                libMQCore_ConfigModule.so libMQCore_XMQModule.so libMQCore_DDSMessage.so libMQCore_ProtocolModule.so libMQCore_SessionModule.so      \
		        XEngine_MQServiceApp.exe 

.PHONY:MakeAll 
MakeAll:$(NETENGINE_LIB)

libjsoncpp.so:
	make -C $(BASE_THIRDPART_JSONCPP) $(FLAGS)

libMQCore_ConfigModule.so:
	make -C $(BASE_CONFIG_PATH) $(FLAGS)
libMQCore_XMQModule.so:
	make -C $(BASE_MQCORE_PATH) $(FLAGS)
libMQCore_DDSMessage.so:
	make -C $(BASE_DDSMESSAGE_PATH) $(FLAGS)
libMQCore_ProtocolModule.so:
	make -C $(BASE_PROTOCOL_PATH) $(FLAGS)
libMQCore_SessionModule.so:
	make -C $(BASE_SESSION_PATH) $(FLAGS)
	
XEngine_MQServiceApp.exe:
	make -C $(APP_SERVICE_PATH) $(FLAGS)


SetFlags_Install:
	export FLAGS=InstallAll
InstallAll:SetFlags_Install $(NETENGINE_LIB) 
 
SetFlags_Clean:
	export FLAGS=CleanAll
CleanAll:SetFlags_Clean $(NETENGINE_LIB) 
	
	
	
