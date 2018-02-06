/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月03日
 *  Author:Sen
 *  开始界面
 *
 */

#ifndef __START__SCENE__
#define __START__SCENE__

#include "UIBasicLayer.h"
#include "network/HttpClient.h"
#include "ServerManager.h"

class UILoadingIndicator;
class UIStart : public UIBasicLayer 
{
	enum VIEWINDEX
	{
		VIEW_EULA = 1,
		VIEW_UPDATE,
	};
public:
	
	UIStart();
	~UIStart();
	bool init();
	static Scene* createScene();
	void onServerEvent(struct ProtobufCMessage *message,int msgType);
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	void connectServer(float t=0);

	void onHttpRequestCompleted(network::HttpClient* client, network::HttpResponse* response);
	void getServerConfig();
	UILoadingIndicator*m_loadingNode;
	void setGameServer(ServerInfo*server);
	void openEula();//用户许可协议
	void openStartPanel();//进入开始页面
	void gotoLoginPage(int succ = 1);
	//打开登录方式
	void openAccountType(bool isShow = false);
	//打开游戏升级界面
	void openUpdateView();
	//打开webView
	void openWebView();
private:
	Widget *m_pTarget;
	void connectErrorDialog(const std::string name);
	void reConnectServer();
	std::vector<ServerInfo*> servers;
	int m_DefaultServerIndex;
	int m_ConfigClientVersion;
	int m_myClinetVersion;
	bool useBack;
	//当前WebVew编号
	int m_viewIndex;
};

#endif
