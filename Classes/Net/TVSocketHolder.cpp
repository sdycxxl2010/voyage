#include "TVSocketHolder.h"
#include "cocos2d.h"
#include "ProtocolThread.h"
#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif

USING_NS_CC;
int TVSocketHolder::start(){    
	log("SocketThread::start");

	return 0;
} 

void TVSocketHolder::setConnectionStatus(bool isConnect){
	m_connected = isConnect;
}

bool TVSocketHolder::isConnected(){
	return m_connected;
}

bool TVSocketHolder::isGoodConnection()
{
	return csocket.isGoodConnection();
}

bool TVSocketHolder::isConnecting(){
	return m_isConnecting;
}

int TVSocketHolder::Connect()   { 
	log("SocketThread::startSocketThread");
#if ANDROID
	//JniHelper::getJavaVM()->DetachCurrentThread();
#endif
	csocket.Init();	


	//const char* serverAddr = "192.168.3.29"; //
#if 0
	//$(call import-add-path, $(COCOS2D)/external/libmobclickcpp/include)
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","GetAccountToken","(Ljava/lang/String;)V");
	cocos2d::log("GetAccountToken method id:%x,%x",jniInfo.methodID,jniInfo.classID);
	if(id)
	{
		jstring jdbName = jniInfo.env->NewStringUTF("test@gmail.com");
		jniInfo.env->CallStaticVoidMethod(jniInfo.classID, jniInfo.methodID,jdbName);
	}
#endif

	int useName = 0;
#if 0
	//const char* serverAddr = "192.168.3.120"; //用office 2
	const char* serverAddr = "54.199.117.92";
	//const char* serverAddr = "127.0.0.1";//"54.248.230.228";"54.254.96.81"//
	//const char* serverAddr = "54.178.104.249";  //server
	//const char *serverAddr = "127.0.0.1";
	//const char* serverAddr = "192.168.2.6";  //office
	//const char* serverAddr = "192.168.3.109";  //server
#else
	//test information should not be in product.
	const char* serverAddr; // "testfronter.voyage.piistudio.com";  //server
	useName = 1;
	m_isConnecting = true;
	if(customIp.length() && useName) {
		serverAddr = customIp.c_str();
	}
#endif

	int port = 8081;
	if(m_port>0 && useName){
		port = m_port;
	}
	if(csocket.Create(AF_INET,SOCK_STREAM,0)){
		if(useName){
			m_connected=csocket.ConnectWithName(serverAddr,port);
		}else{
			m_connected=csocket.Connect(serverAddr,port);
		}
	}	
	m_isConnecting = false;
//	if(!iscon){
//	   //serverAddr = "54.250.92.87";  //
//	   if(csocket.Create(AF_INET,SOCK_STREAM,0)){
//		iscon=csocket.Connect(serverAddr,port);
//	   }
//	}
	if(m_connected){
		CCLOG("connected to server:%s:%d",serverAddr,port);
		//ProtocolThread::GetInstance()->clearQueueMsg();
		ProtocolThread::GetInstance()->broadcastConnectionSucceeded();
		CCLOG("disconnected by server:%s:%d",serverAddr,port);
		//m_connected = false;

	}else{
		ProtocolThread::GetInstance()->broadcastConnectionFailed();
		CCLOG("connect to server:%s:%d failed",serverAddr,port);
	}	
	if(m_chatServerConnected && chatSocket){
		chatSocket->Close(__FILE__,__LINE__);
	}
	return 0;
}


int TVSocketHolder::SelectSocket()
{
	ProtocolThread::GetInstance()->processSocketMessage();
	return 0;
}

int TVSocketHolder::Disconnect()
{
	ProtocolThread::GetInstance()->broadcastConnectionDisconnected();
	return 0;
}


int TVSocketHolder::StartChatThread(){    
	int errCode = 0;


	auto socket = new ODSocket;
	socket->Create(AF_INET,SOCK_STREAM,0);
	if(chatSocket){
		chatSocket->Close(__FILE__,__LINE__);
	}
	chatSocket = socket;

	chatThreadInstance = new std::thread(CC_CALLBACK_0(TVSocketHolder::chatThread,this));
	return errCode;
} 

void* TVSocketHolder::chatThread(){
	//remove server name info.
	const char* serverAddr;//"testchat.voyage.piistudio.com";
	int useName = 1;
	if(customChatIp.length() && useName) {
		serverAddr = customChatIp.c_str();
	}

	//serverAddr = "192.168.3.120";
	//const char* serverAddr = "192.168.10.127";
	int port = 9001;
	if(m_chatPort > 0){
		m_chatPort = port;
	}
	bool connected = false;
	if(1){
		if(useName){
			m_chatServerConnected = chatSocket->ConnectWithName(serverAddr, port);
		}else{
			m_chatServerConnected = chatSocket->Connect(serverAddr, port);
		}
	}
	if (m_chatServerConnected){
		log("chat connected.");
		ProtocolThread::GetInstance()->processChatMessage();
		m_chatServerConnected = false;
	}
	if(m_connected && ProtocolThread::GetInstance()->getLastLoginResult()){//异常退出，重连
		StartChatThread();
	}
#if ANDROID
	JniHelper::getJavaVM()->DetachCurrentThread();
#endif
	return 0;
}

ODSocket TVSocketHolder::getSocket(){
	return this->csocket;
}

ODSocket& TVSocketHolder::getChatSocket(){
	return *chatSocket;
}






TVSocketHolder::TVSocketHolder(void)
{
	m_connected = 0;
	//customIp = 0;
	chatThreadInstance = 0;
	m_isConnecting = false;
	m_chatServerConnected = false;
	chatSocket = 0;
	m_port = 0;
	m_chatPort = 0;
	m_vBackendRspQueue;
}


TVSocketHolder::~TVSocketHolder(void)
{
	
}
