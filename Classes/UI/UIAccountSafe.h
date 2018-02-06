/*
*  description: 异地账户安全验证
*/
#ifndef __ACCOUNTSAFE_LAYER_H__
#define __ACCOUNTSAFE_LAYER_H__
#include "UIBasicLayer.h"
#include "network/HttpClient.h"
class UIAccountSafe : public UIBasicLayer
{
public:
//	struct ServerInfo{
//		std::string name;
//		std::string host;
//		int port;
//		std::string chatServer;
//		int chatPort;
//		std::string apiServer;
//		int status; //1 when valid
//	};
	UIAccountSafe();
	~UIAccountSafe();
	bool init();
	void initStaticData(float f);
	static Scene* createAccountSafeLayer(bool fromLoadingScene =false);
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	//void getAccountConfigServer();
	//void setGameServer(struct ServerInfo*server);
	//void onHttpRequestCompleted(network::HttpClient* client, network::HttpResponse* response);
	UILoadingIndicator*m_loadingNode;
	//void gotoLoginPage(int succ = 1);
	void openService(const std::string name);
private:

	UILoadingIndicator*m_loading;
	//std::vector<ServerInfo*> servers;

	int m_DefaultServerIndex;
	int m_ConfigClientVersion;
	int m_myClinetVersion;
	int sendIntervalTime;
	bool continueFail;

};
#endif
