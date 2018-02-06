#include "UIAccountSafe.h"
#include "ServerManager.h"
#include "UIStart.h"
#include "UISetting.h"
#include "UIInform.h"
#include "env.h"
#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif
UIAccountSafe::UIAccountSafe()
{
	m_DefaultServerIndex = -1;
	m_myClinetVersion = 0;
	m_ConfigClientVersion = 0;
	m_loadingNode = nullptr;
	sendIntervalTime = 0;
}


UIAccountSafe::~UIAccountSafe()
{
}
Scene*UIAccountSafe::createAccountSafeLayer(bool fromLoadingScene)
{
	auto sceneAccount = Scene::create();
	UIAccountSafe* account = new UIAccountSafe();
	account->continueFail = fromLoadingScene;
	if (account &&account->init())
	{
		account->autorelease();
		sceneAccount->addChild(account);
		return sceneAccount;
	}
	CC_SAFE_DELETE(account);
	return nullptr;
}
bool UIAccountSafe::init()
{
	bool pRet = false;
	do
	{
		//getAccountConfigServer();
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIAccountSafe::onServerEvent, this), this);
		scheduleOnce(schedule_selector(UIAccountSafe::initStaticData), 0);
		pRet = true;
	} while (0);
	return pRet;
}
void UIAccountSafe::initStaticData(float f)
{
	openView(ABNORMAL_ACCOUNT_RES[VIEW_ABNOMAL_ACCOUNT]);
	auto view = getViewRoot(ABNORMAL_ACCOUNT_RES[VIEW_ABNOMAL_ACCOUNT]);
	auto panel_account = view->getChildByName<Widget*>("panel_account");
	auto i_label_account_1 = panel_account->getChildByName<Text*>("label_account_1");
	i_label_account_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_REMOTE_ACCOUNT_YOUR_ACCOUNT"]);
	auto i_label_account_2 = panel_account->getChildByName<Text*>("label_account_2");
	i_label_account_2->setString(ProtocolThread::GetInstance()->getSavedEmail());
	i_label_account_2->setPositionX(i_label_account_1->getPositionX() + i_label_account_1->getContentSize().width);
	auto i_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content"));
	i_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_ABNORMAL_ACCOUNT_CONTENT"]);
	auto panel_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_bg"));
	auto winSize = Director::getInstance()->getWinSize();
	auto width_num = panel_bg->getContentSize().width - winSize.width;
	panel_bg->runAction(RepeatForever::create(Sequence::createWithTwoActions(MoveBy::create(width_num / 10, Vec2(-width_num, 0))
		, MoveBy::create(width_num / 10, Vec2(width_num, 0)))));
	if (continueFail)
	{
		openService("TIP_REMOTE_ACCOUNT_GO_TO_ACCOUNT");
	}
}
void UIAccountSafe::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* widget = dynamic_cast<Widget*>(pSender);
	std::string name = widget->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//关闭异地安全界面
	if (isButton(button_abnormal_close))
	{
		closeView();
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		auto scenceStart = UIStart::createScene();
		Director::getInstance()->replaceScene(scenceStart);
		return;
	}
	//异地账户安全，重发
	if (isButton(button_abnormal_resend))
	{
		int timeNow = getCurrentTimeUsev() / 1000.0;
		if (timeNow - sendIntervalTime <= 60)//重发时间
		{
			openService("TIP_REMOTE_ACCOUNT_ONE_MINITE");
		}
		else
		{
			std::string  emailContent = ProtocolThread::GetInstance()->getSavedEmail();
			char*email = (char*)(emailContent.c_str());
			ProtocolThread::GetInstance()->authenticateAccount(email, 1, UILoadingIndicator::create(this));
			sendIntervalTime = getCurrentTimeUsev() / 1000.0;
		}
		
		return;
	}
	//继续,类似于开始界面的start按钮
	if (isButton(button_abnormal_continue))
	{
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		reConnect();
//#if ANDROID
//		if (m_ConfigClientVersion > m_myClinetVersion){
//			GUITip* tip = GUITip::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
//			//tip->setPosition(400,240);
//			tip->setPositiveCallback(CC_CALLBACK_0(BasicLayer::exitGame, this));
//			this->addChild(tip, 10100);
//			return;
//		}
//		if (ProtocolThread::GetInstance()->hasValidEmailAndPassword()){
//			gotoLoginPage(2);
//		}
//		else{
//			//StartGoogleLogin
//			JniMethodInfo jniInfo;
//			bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "StartGoogleLogin", "()I");
//			log("StartGoogleLogin id:%x,%x,%d", jniInfo.methodID, jniInfo.classID, id);
//			int succ = 0;
//			if (id)
//			{
//				if (jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID) == 1){
//					succ = 1;
//				}
//			}
//			if (succ){
//				m_loadingNode = LoadingLayer::create(this);
//				m_loadingNode->showSelf();
//			}
//			else{
//				gotoLoginPage(2);
//			}
//		}
//
//#else
//		gotoLoginPage();
//#endif
		return;
	}
	//确定联系客服
	if (isButton(button_confirm_yes))
	{
		UISetting::getInstance()->openSettingLayer();
		UISetting::getInstance()->customServiceFeedBack(true);
		return;
	}
	//关闭
	if (isButton(button_game_exit))
	{
		closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
		return;
	}
	//打开客服
	if (isButton(button_game_reconn))
	{
		closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
		UISetting::getInstance()->openSettingLayer();
		UISetting::getInstance()->customServiceFeedBack(true);
		return;
	}
}
void UIAccountSafe::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	switch (msgType)
	{
	case PROTO_TYPE_AuthenticateAccountResult:
	{
			AuthenticateAccountResult*result = (AuthenticateAccountResult*)message;
			if (result->forsecurity)
			{
				if (result->failed == 0 )
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_REMOTE_ACCOUNT_LOGIN");

				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_REMOTE_ACCOUNT_RESEND_RESULT");
					//客服
					UISetting::getInstance()->openSettingLayer();
					UISetting::getInstance()->customServiceFeedBack(true);
				}
			}
			
			break;
	}
	default:
	break;
	}
}
//void AccountSafeLayer::getAccountConfigServer()
//{
//	m_loadingNode = LoadingLayer::create(this);
//	cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
//	std::string urlpath;
//	if (ENVIRONMENT == PRODUCTION){
//		urlpath = "http://pubapi.voyage.piistudio.com/status.php";
//	}
//	else{
//		urlpath = "http://pubapi.voyage.piistudio.com/status.php?test=1";
//	}
//
//	request->setUrl(urlpath.c_str());
//	request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
//	request->setResponseCallback(CC_CALLBACK_2(AccountSafeLayer::onHttpRequestCompleted, this));
//
//	request->setTag("log_test_1");
//
//	cocos2d::network::HttpClient::getInstance()->send(request);
//	request->release();
//	m_loadingNode->showSelf();
//}
//void AccountSafeLayer::onHttpRequestCompleted(network::HttpClient* client, network::HttpResponse* response)
//{
//	long code = response->getResponseCode();
//	int valid = 0;
//	if (code == 200){
//		auto data = response->getResponseData();
//		std::string s_data(data->begin(), data->end());
//		rapidjson::Document root;
//		log("server config:%s", s_data.c_str());
//		root.Parse<0>(s_data.c_str());
//		m_ConfigClientVersion = DictionaryHelper::getInstance()->getIntValue_json(root, "version");
//#if ANDROID
//		//getVersionCode
//		JniMethodInfo jniInfo;
//		bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "GetVersionCode", "()I");
//		if (id)
//		{
//			m_myClinetVersion = jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID);
//		}
//
//		if (m_ConfigClientVersion > m_myClinetVersion){
//			GUITip* tip = GUITip::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
//			//tip->setPosition(400,240);
//			tip->setPositiveCallback(CC_CALLBACK_0(BasicLayer::exitGame, this));
//			this->addChild(tip, 10100);
//			return;
//		}
//#endif
//		const char* default_zone_name = DictionaryHelper::getInstance()->getStringValue_json(root, "default_zone_name");
//		if (1){
//			int version = DictionaryHelper::getInstance()->getIntValue_json(root, "version");
//			auto&zones = DictionaryHelper::getInstance()->getSubDictionary_json(root, "zones");
//			int n = (int)zones.Size();
//			for (int i = 0; i < n; i++){
//				auto server = new ServerInfo;
//				auto& item = DictionaryHelper::getInstance()->getSubDictionary_json(zones, i);
//				auto status = DictionaryHelper::getInstance()->getStringValue_json(item, "status");
//				if (status && strcmp(status, "ok") == 0)
//				{
//					server->status = 1;
//				}
//				else
//				{
//					server->status = 0;
//				}
//				auto host = DictionaryHelper::getInstance()->getStringValue_json(item, "host");
//				server->host = host;
//				auto name = DictionaryHelper::getInstance()->getStringValue_json(item, "name");
//				server->name = name;
//				server->port = DictionaryHelper::getInstance()->getIntValue_json(item,"port");
//				server->chatServer = DictionaryHelper::getInstance()->getStringValue_json(item,"chat");
//				server->chatPort = DictionaryHelper::getInstance()->getIntValue_json(item,"chat_port");
//				server->apiServer = DictionaryHelper::getInstance()->getStringValue_json(item,"api");
//				servers.push_back(server);
//				if (name && default_zone_name && strcmp(name, default_zone_name) == 0){
//					m_DefaultServerIndex = i;
//				}
//			}
//		}
//
////		std::string apiServer = DictionaryHelper::getInstance()->getStringValue_json(root, "api");
////		log("set api server %s", apiServer.c_str());
////		ServerConfig::GetInstance()->SetApiServer(apiServer);
//	}
//		//..............存储本地.....................
//		int serverIndex = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), m_DefaultServerIndex + 1);
//		if (serverIndex < 1 || serverIndex > 3){
//			serverIndex = 1;
//		}
//		//服务器状态不可用
//		if (servers[serverIndex - 1]->status)
//		{
//			setGameServer(servers[serverIndex - 1]);
//		}
//		else
//		{
//			openSuccessOrFailDialog("TIP_STARTPANEL_RESELECT_SER");
//		}
//
//		if (serverIndex > 0 && serverIndex < 4){
//			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), serverIndex);
//			UserDefault::getInstance()->flush();
//		}
//		log("---Set Server Name:%s---", servers[serverIndex - 1]->name.data());
//	   m_loadingNode->removeFromParent();
//}
//
//void AccountSafeLayer::setGameServer(struct ServerInfo*server)
//{
//	log("set server url:%s,chat url:%s,api url:%s",server->host.c_str(),server->chatServer.c_str(),server->apiServer.c_str());
//	ProtocolThread::GetInstance()->setServerUrl(server->host,server->port,server->chatServer,server->chatPort);
//	ServerConfig::GetInstance()->SetApiServer(server->apiServer);
//}
//
//void AccountSafeLayer::gotoLoginPage(int succ)
//{
//#if ANDROID
//	if (succ != 2){
//		m_loadingNode->removeFromParent();
//	}
//#endif
//	//log("gotoLoginPage code:%d",succ);
//	int serverIndex = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), m_DefaultServerIndex + 1);
//	if (servers[serverIndex - 1]->status)
//	{
//		auto scene = LoadingScene::createLoadingScene(true);
//		ProtocolThread::GetInstance()->connectServer();
//		Director::getInstance()->replaceScene(scene);
//	}
//	else
//	{
//		openSuccessOrFailDialog("TIP_STARTPANEL_RESELECT_SER");
//	}
//
//}
void  UIAccountSafe::openService(const std::string name)
{
	openView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
	view->setLocalZOrder(10000);
	std::string text = SINGLE_SHOP->getTipsInfo()[name];
	auto label_content = view->getChildByName<Text*>("label_content");
	label_content->setString(text);

	auto i_game_recc = view->getChildByName<Button*>("button_game_reconn");
	auto i_game_exit = view->getChildByName<Button*>("button_game_exit");
	i_game_recc->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_REMOTE_ACCOUNT_SERVICE"]);
	i_game_exit->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_EXIT_GAME"]);

	i_game_recc->addTouchEventListener(CC_CALLBACK_2(UIAccountSafe::menuCall_func,this));
	i_game_exit->addTouchEventListener(CC_CALLBACK_2(UIAccountSafe::menuCall_func,this));
}
