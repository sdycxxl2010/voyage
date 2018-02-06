#include "UIStart.h"
#include "cocostudio/DictionaryHelper.h"
#include "ServerManager.h"
#include "Utils.h"
#include "UITips.h"
#include "TVLoadingScene.h"
#include "UISocial.h"
#include "TVEventManager.h"

#include "env.h"
#include"ui/CocosGUI.h"
#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif
UIStart::UIStart()
{
	m_pTarget = nullptr;
	m_loadingNode = nullptr;
	m_DefaultServerIndex = -1;
	m_ConfigClientVersion = 0;
	m_myClinetVersion = 0;
	m_eUIType = UI_START;
	useBack = false;
	m_viewIndex = 0;
}

UIStart::~UIStart()
{

}

Scene* UIStart::createScene()
{
	auto scene = Scene::create();
	UIStart* lg = new UIStart;

	if (lg && lg->init())
	{
		lg->autorelease();
		scene->addChild(lg);
		return scene;
	}
	CC_SAFE_DELETE(lg);
	return nullptr;
}

bool UIStart::init()
{
	bool pRet = false;
	do 
	{
		useBack = Utils::shouldUseBackServer();
        std::string triggerPath = "/sdcard/useback";
        std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(triggerPath);
        if(contentStr.length() > 0){
            useBack = true;
        }
        std::string splashPath = "cocosstudio/voyageUI_splashscr.csb";
        openView(splashPath);
        auto view = getViewRoot(splashPath);
		view->setOpacity(0);
		view->runAction(Sequence::create(FadeIn::create(0.8), DelayTime::create(1.7), FadeOut::create(0.5), CallFunc::create([=]{
			closeView();
			if (UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(HAVE_ACCPET_EULA).c_str()))
			{
				openStartPanel();
			}
			else
			{	
				openEula();
			}
			// start music
			if (UserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY) != CLOSE_AUDIO)
			{
				SINGLE_AUDIO->setMusicType();
				playAudio();
			}
			if (UserDefault::getInstance()->getIntegerForKey(SOUND_EFFECT_KEY) != CLOSE_AUDIO)
			{
				SINGLE_AUDIO->setEffectON(true);
			}
			else
			{
				SINGLE_AUDIO->setEffectON(false);
			}
		}), nullptr));
		ProtocolThread::GetInstance()->registerGoogleLoginCallback(CC_CALLBACK_1(UIStart::gotoLoginPage, this));
		pRet = true;
	} while (0);

	return pRet;
}
void UIStart::openEula()
{
	setEula(1);
	openView(SETTING_COCOS_RES[VIEW_INFO_EULA]);
	auto view = getViewRoot(SETTING_COCOS_RES[VIEW_INFO_EULA]);
	auto btn_close = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_agreement_close"));
	btn_close->setVisible(false);
	auto image_content_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_content_bg"));
	auto sizeContent = image_content_bg->getContentSize();
	m_viewIndex = VIEW_EULA;
	//webView网页控件
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		
		cocos2d::experimental::ui::WebView *siteWeb = cocos2d::experimental::ui::WebView::create();
		image_content_bg->addChild(siteWeb);
		siteWeb->setTag(100);
		siteWeb->setLocalZOrder(-1);
		if(ENVIRONMENT == PRODUCTION){
			siteWeb->loadURL("http://pubapi.voyage.piistudio.com/eula.php");
		}else{
			siteWeb->loadURL("http://pubapi-dev.voyage.piistudio.com/eula.php");
		}
		siteWeb->setAnchorPoint(Vec2(0.5, 0.5));
		siteWeb->setContentSize(sizeContent);
		siteWeb->setPosition(Vec2(image_content_bg->getContentSize().width / 2, image_content_bg->getContentSize().height / 2));
		siteWeb->setScalesPageToFit(true);
		siteWeb->setOnShouldStartLoading([](cocos2d::experimental::ui::WebView*sender, const std::string &url){
			return true;
		});
		siteWeb->setOnDidFinishLoading([](cocos2d::experimental::ui::WebView *sender, const std::string &url){
		});
		siteWeb->setOnDidFailLoading([](cocos2d::experimental::ui::WebView *sender, const std::string &url){
		});
	}
#endif
	
}
void  UIStart::openStartPanel()
{
	openView("cocosstudio/voyageUI_start.csb");
	auto view_1 = getViewRoot("cocosstudio/voyageUI_start.csb");
	if(view_1){
		std::string versionName = Utils::getVersionName();
		std::string versionCode = Utils::getVersionCode();
		if(USE_BAKCUP_SERVER || useBack){
			versionName= versionName + "[beta]";
		}
		if(ENVIRONMENT == DEVELOPMENT){
			versionName=versionName+"[dev]";
		}
		versionName = versionName + "(" + versionCode + ")";
		auto label = Label::createWithTTF(versionName, "fonts/Roboto-Regular.ttf",20);
		view_1->addChild(label);
		label->setVisible(true);
		label->setAnchorPoint(Point(0,0));
		label->setPosition(Point(5,5));
	}
	auto winSize = Director::getInstance()->getWinSize();
	auto panel_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(view_1, "panel_bg"));
	auto width_num = panel_bg->getContentSize().width - winSize.width;
	panel_bg->runAction(RepeatForever::create(Sequence::createWithTwoActions(MoveBy::create(width_num / 10, Vec2(-width_num, 0))
		, MoveBy::create(width_num / 10, Vec2(width_num, 0)))));
	auto panel_start = view_1->getChildByName<Widget*>("panel_start");
	auto panel_server = view_1->getChildByName<Widget*>("panel_customize_charactor");
	panel_start->setVisible(true);
	panel_server->setVisible(false);
	getServerConfig();

	//推荐区(1:北美,2:欧洲,,3:亚太)
	//..............选择服务器本地存储.............
	int serverIndex = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), 1);
	
	std::string st_tip = StringUtils::format("TIP_START_SELECT_SERVER_%d", serverIndex);
	std::string st_tips = SINGLE_SHOP->getTipsInfo()[st_tip];
	SINGLE_SHOP->getTipsInfo()["TIP_START_SELECT_SERVER_TITLE"];
	auto t_server = panel_start->getChildByName<Text*>("text_server_1");
	t_server->setString(st_tips);
	auto i_server_status = panel_start->getChildByName<ImageView*>("image_online");
	i_server_status->setPositionX(t_server->getPositionX() - t_server->getBoundingBox().size.width - i_server_status->getBoundingBox().size.width);
	auto image_div = panel_start->getChildByName<ImageView*>("image_div");
	if (t_server->getBoundingBox().size.width > image_div->getBoundingBox().size.width)
	{
		image_div->setContentSize(Size(t_server->getBoundingBox().size.width, image_div->getBoundingBox().size.height));
	}
}
void UIStart::connectServer(float t){
	ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIStart::onServerEvent,this),this);
	ProtocolThread::GetInstance()->connectServer();
}

void UIStart::onServerEvent(struct ProtobufCMessage *message,int msgType)
{

	if(msgType == PROTO_TYPE_GetServerInfoResult){
		GetServerInfoResult*result = (GetServerInfoResult*)message;
		if(result->clientversion < result->serverversion){
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
			//tip->setPosition(400,240);
			tip->setPositiveCallback(CC_CALLBACK_0(UIBasicLayer::gotoStorePage, this));
			this->addChild(tip, 10100);
		}
	}else if (msgType == -1) {
		openReconnectDialog("TIP_CONNECT_FAIL");
		return;
	}else{
		UIBasicLayer::onServerEvent(message,msgType);
	}
}
/*
 * 参数是什么意思？0，1，2？
 */
void UIStart::gotoLoginPage(int succ)
{
#if ANDROID
	if(succ !=2 && m_loadingNode){
		m_loadingNode->removeFromParent();
	}
#endif
	//log("gotoLoginPage code:%d",succ);
	int serverIndex = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), m_DefaultServerIndex + 1);
	serverIndex = serverIndex - 1;
	if (serverIndex >= 0 && servers.size() > serverIndex && servers[serverIndex]->status)
	{
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		auto scene = TVLoadingScene::createLoadingScene();
		if(!scene){ //not sure why it will be nullptr, memory problem? try to retry it here.
			scene = TVLoadingScene::createLoadingScene();
		}
		ProtocolThread::GetInstance()->connectServer();
		Director::getInstance()->replaceScene(scene);
	}
	else
	{
		openSuccessOrFailDialog("TIP_STARTPANEL_RESELECT_SER");
	}
}


void UIStart::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	auto widget = dynamic_cast<Widget*>(pSender);
	auto name = widget->getName();

		if (name.compare("button_start") == 0)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_LOOF_26);
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		}
	if (isButton(button_server))//服务器区域选择按钮
	{
		Utils::sendNewPlayerTrackingStep("go_into_choose_server", 0);
		auto view = getViewRoot("cocosstudio/voyageUI_start.csb");
		auto panel_start = view->getChildByName<Widget*>("panel_start");
		auto panel_server = view->getChildByName<Widget*>("panel_customize_charactor");
		panel_start->setVisible(false);
		panel_server->setVisible(true);
		return;
	}

	if (isButton(panel_server_))//选择具体的服务器区域
	{
		if (m_pTarget)
		{
			m_pTarget->setTouchEnabled(true);
			m_pTarget->getChildByName<Widget*>("checkbox_1")->setVisible(false);
		}
		m_pTarget = widget;
		m_pTarget->setTouchEnabled(false);
		m_pTarget->getChildByName<Widget*>("checkbox_1")->setVisible(true);
		m_DefaultServerIndex = m_pTarget->getTag() - 1;
	
		Utils::sendNewPlayerTrackingStep(StringUtils::format("choose_server_%d", m_DefaultServerIndex + 1), 0);
		return;
	}

	if (isButton(button_ok))//确定选择此服务器
	{
		Utils::sendNewPlayerTrackingStep("choose_server_ok", 0);
		auto view = getViewRoot("cocosstudio/voyageUI_start.csb");
		auto panel_start = view->getChildByName<Widget*>("panel_start");
		auto panel_server = view->getChildByName<Widget*>("panel_customize_charactor");
		panel_server->setVisible(false);
		panel_start->setVisible(true);
		m_DefaultServerIndex = m_pTarget->getTag() - 1;

		auto hosts = servers.at(m_DefaultServerIndex)->host;
	
		int index;
		if (servers.at(m_DefaultServerIndex)->name == "na")
		{
			index = 1;
		}
		else if (servers.at(m_DefaultServerIndex)->name == "eu")
		{
			index = 3;
		}
		else
		{
			index = 2;
		}
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), index);
		UserDefault::getInstance()->flush();

		std::string st_tip = StringUtils::format("TIP_START_SELECT_SERVER_%d", index);
		std::string st_tips = SINGLE_SHOP->getTipsInfo()[st_tip];
		auto t_server = panel_start->getChildByName<Text*>("text_server_1");
		t_server->setString(st_tips);
		auto image_div = panel_start->getChildByName<ImageView*>("image_div");
		if (t_server->getBoundingBox().size.width > image_div->getBoundingBox().size.width)
		{
			image_div->setContentSize(Size(t_server->getBoundingBox().size.width, image_div->getBoundingBox().size.height));
		}
		auto i_server_status = panel_start->getChildByName<ImageView*>("image_online");
		switch (servers[m_DefaultServerIndex]->status)
		{
		case 0:
			i_server_status->loadTexture("cocosstudio/login_ui/social_720/offline.png");
			break;
		case 1:
			setGameServer(servers[m_DefaultServerIndex]);
			i_server_status->loadTexture("cocosstudio/login_ui/social_720/online.png");
			break;
		case 2:
			i_server_status->loadTexture("cocosstudio/login_ui/social_720/maintain.png");
			break;
		default:
			log("server status data error !!!");
			break;
		}
		i_server_status->setPositionX(t_server->getPositionX() - t_server->getBoundingBox().size.width - i_server_status->getBoundingBox().size.width);

		//log("---Set Server Name:%s---", servers[m_DefaultServerIndex]->name.data());
		return;
	}

	if (isButton(button_start))//开始
	{
		Utils::sendNewPlayerTrackingStep("start_game", 0);
		std::string st_status;
		std::string old_value;
		std::string new_value;
		time_t myTime = atoll(servers[m_DefaultServerIndex]->maintainEnd.c_str());
		tm *nowTime = localtime(&myTime);
		//0 服务器关闭状态，1 正常状态，2 维护状态
		switch (servers[m_DefaultServerIndex]->status)
		{
		case 0:
			openSuccessOrFailDialog("TIP_START_SERVER_CLOSE");
			break;
		case 1:
			if (!ProtocolThread::GetInstance()->hasValidEmailAndPassword())
			{
				openAccountType(true);
			}
			else
			{
#if ANDROID
				if (m_ConfigClientVersion > m_myClinetVersion){
					UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
					//tip->setPosition(400,240);
					tip->setPositiveCallback(CC_CALLBACK_0(UIBasicLayer::gotoStorePage, this));
					this->addChild(tip, 10100);
					return;
				}
				if (ProtocolThread::GetInstance()->hasValidEmailAndPassword()){
					gotoLoginPage(2);
				}
				else{
					//StartGoogleLogin
					JniMethodInfo jniInfo;
					bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "StartGoogleLogin", "()I");
					log("StartGoogleLogin id:%x,%x,%d", jniInfo.methodID, jniInfo.classID, id);
					int succ = 0;
					if (id)
					{
						if (jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID) == 1){
							succ = 1;
						}
					}
					if (succ){
						m_loadingNode = UILoadingIndicator::create(this);
						m_loadingNode->showSelf();
					}
					else{
						gotoLoginPage(2);
					}
				}
#else
				gotoLoginPage();
#endif
			}
			UISocial::getInstance();
			break;
		case 2:
			st_status = SINGLE_SHOP->getTipsInfo()["TIP_START_SERVER_MAINTAIN"];
			old_value = "[time]";
			new_value = StringUtils::format("%d/%d/%d %02d:%02d", nowTime->tm_mon + 1, nowTime->tm_mday, nowTime->tm_year + 1900, nowTime->tm_hour, nowTime->tm_min);
			repalce_all_ditinct(st_status, old_value, new_value);
			openView(COMMOM_COCOS_RES[C_VIEW_SERVER_MAINTAIN]);
			break;
		default:
			log("server status data error !!!");
			break;
		}
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SERVER_MAINTAIN]);
		if (view)
		{
			auto t_content = view->getChildByName<Text*>("label_content");
			t_content->setString(st_status);
		}
		return;
	}

	

	//google账户按钮
	if (isButton(button_google))
	{
		Utils::sendNewPlayerTrackingStep("choose_google_account", 0);
		openAccountType(false);
#if ANDROID
		if (m_ConfigClientVersion > m_myClinetVersion)
		{
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
			//tip->setPosition(400,240);
			tip->setPositiveCallback(CC_CALLBACK_0(UIBasicLayer::gotoStorePage, this));
			this->addChild(tip, 10100);
			return;
		}

		//StartGoogleLogin
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "StartGoogleLogin", "()I");
		log("StartGoogleLogin id:%x,%x,%d", jniInfo.methodID, jniInfo.classID, id);
		int succ = 0;
		if (id)
		{
			if (jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID) == 1){
				succ = 1;
			}
		}
		if (succ){
			ProtocolThread::GetInstance()->savePassword("");
			m_loadingNode = UILoadingIndicator::create(this);
			m_loadingNode->showSelf();
		}
		else{
			gotoLoginPage(2);
		}
#else

		gotoLoginPage();
#endif
		return;
	}
	//email账户按钮
	if (isButton(button_email))
	{
		Utils::sendNewPlayerTrackingStep("choose_email_account", 0);
		openAccountType(false);
#if ANDROID
		if (m_ConfigClientVersion > m_myClinetVersion)
		{
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
			//tip->setPosition(400,240);
			tip->setPositiveCallback(CC_CALLBACK_0(UIBasicLayer::gotoStorePage, this));
			this->addChild(tip, 10100);
			return;
		}
		else
		{
			gotoLoginPage(2);
		}
#else

		gotoLoginPage();
#endif
		return;
	}
	if (isButton(button_accept))//用户许可协议接受按钮,或提示框的确定按钮
	{
		if (m_viewIndex == VIEW_EULA)
		{
			Utils::sendNewPlayerTrackingStep("user_accept_protocol", 0);
			closeView(SETTING_COCOS_RES[VIEW_INFO_EULA]);
			UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(HAVE_ACCPET_EULA).c_str(), true);
			UserDefault::getInstance()->flush();
			openStartPanel();
		}else if (m_viewIndex == VIEW_UPDATE)
		{
			//联系客服
			std::string  emailContent = ProtocolThread::GetInstance()->getLoginEmail();
			Utils::ShowFreshDesk(emailContent, SINGLE_HERO->m_sName);
		}

		return;
	}
	if (isButton(button_reject))//用户许可协议取消按钮
	{
		if (m_viewIndex == VIEW_EULA)
		{
			Utils::sendNewPlayerTrackingStep("user_reject_protocol", 0);
			closeView(SETTING_COCOS_RES[VIEW_INFO_EULA]);
			UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(HAVE_ACCPET_EULA).c_str(), false);
			UserDefault::getInstance()->flush();
			Director::getInstance()->end();//退出游戏
		}else if (m_viewIndex == VIEW_UPDATE)
		{
			//清理缓存
			Utils::clearCache();
		}
 

		return;
	}
	//取消选择账户界面
	if (isButton(button_cancle))
	{
		Utils::sendNewPlayerTrackingStep("cancle_choose_account", 0);
		openAccountType(false);
		return;
	}

	if (isButton(button_captain_yes))
	{
		closeView(COMMOM_COCOS_RES[C_VIEW_SERVER_MAINTAIN]);
		return;
	}
	//游戏升级
	if (isButton(button_game_exit))
	{
		UserDefault::getInstance()->setBoolForKey(HAVE_OPEN_CHANGE_LOG, true);
		UserDefault::getInstance()->flush();
		gotoStorePage();
		return;
	}
	//我有问题
	if (isButton(button_game_reconn))
	{
		openWebView();
		return;
	}
	if (isButton(button_agreement_close))
	{
		if (m_viewIndex == VIEW_UPDATE)
		{
			closeView(SETTING_COCOS_RES[VIEW_INFO_EULA]);
		}
		return;
	}
}

void UIStart::reConnectServer()
{
	getServerConfig();
}

void UIStart::connectErrorDialog(const std::string name)
{
	std::string text = SINGLE_SHOP->getTipsInfo()[name];
	UITips* tip = UITips::createTip(text.c_str(),2);
	this->addChild(tip,100);

	tip->setNegativeCallback(CC_CALLBACK_0(UIStart::exitGame,this));
	tip->setPositiveCallback(CC_CALLBACK_0(UIStart::reConnectServer,this));
}

void UIStart::setGameServer(ServerInfo*server)
{
	log("set server url:%s,chat url:%s,api url:%s",server->host.c_str(),server->chatServer.c_str(),server->apiServer.c_str());
	ProtocolThread::GetInstance()->setServerUrl(server->host,server->port,server->chatServer,server->chatPort);
	ServerManager::CurSelectedServer()->apiServer = server->apiServer;
	ServerManager::CurSelectedServer()->name = server->name;
	TVEventManager::PushEvent(TVEventType::_NET_SELECT_SERVER);
}

void UIStart::onHttpRequestCompleted(network::HttpClient* client, network::HttpResponse* response)
{
	long code = response->getResponseCode();
	int valid = 0;
	if(code == 200){
		auto data = response->getResponseData();
		std::string s_data(data->begin(),data->end());
		rapidjson::Document root;
		log("server config:%s",s_data.c_str());
		if(root.Parse<0>(s_data.c_str()).HasParseError()){
			return;
		}
		m_ConfigClientVersion = DictionaryHelper::getInstance()->getIntValue_json(root, "version");
#if ANDROID
		//getVersionCode
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","GetVersionCode","()I");
		if(id)
		{
			m_myClinetVersion = jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID);
		}

		if(m_ConfigClientVersion > m_myClinetVersion){
// 			GUITip* tip = GUITip::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
// 			//tip->setPosition(400,240);
// 			tip->setPositiveCallback(CC_CALLBACK_0(BasicLayer::gotoStorePage, this));
// 			this->addChild(tip, 10100);
			openUpdateView();
			return;
		}
#endif
		const char* default_zone_name = DictionaryHelper::getInstance()->getStringValue_json(root,"default_zone_name");
		if(1){
			int version = DictionaryHelper::getInstance()->getIntValue_json(root,"version");
			auto&zones = DictionaryHelper::getInstance()->getSubDictionary_json(root,"zones");
			int n = (int)zones.Size();
			for(int i=0;i<n;i++){
				auto server = new ServerInfo;
				auto& item = DictionaryHelper::getInstance()->getSubDictionary_json(zones,i);
				auto status = DictionaryHelper::getInstance()->getStringValue_json(item,"status");
				if(status){
					if(strcmp(status,"ok") == 0)
					{
						server->status = 1;
					}else if(strcmp(status,"maintain") == 0){
						if(FORCE_CONNECT){
							server->status = 1;
						}else{
							server->status = 2;
						}
					}
					else
					{
						server->status = 0;
					}
				}
				auto host = DictionaryHelper::getInstance()->getStringValue_json(item,"host");
				server->host = host;
				auto name = DictionaryHelper::getInstance()->getStringValue_json(item,"name");
				server->name = name;
				server->port = DictionaryHelper::getInstance()->getIntValue_json(item,"port");
				server->chatServer = DictionaryHelper::getInstance()->getStringValue_json(item,"chat");
				server->chatPort = DictionaryHelper::getInstance()->getIntValue_json(item,"chat_port");
				server->apiServer = DictionaryHelper::getInstance()->getStringValue_json(item,"api");
				auto maintain = DictionaryHelper::getInstance()->getStringValue_json(item,"maintain_end");
				if(maintain){
					server->maintainEnd = maintain;
				}
				servers.push_back(server);

				ServerManager::getInstance()->AddServer(server);

				if(name && default_zone_name && strcmp(name,default_zone_name) == 0){
					m_DefaultServerIndex = i;
					ServerManager::getInstance()->SelectServerByName(name);
				}
			}
		}

//		std::string apiServer = DictionaryHelper::getInstance()->getStringValue_json(root,"api");
//		log("set api server %s", apiServer.c_str());
//		ServerConfig::GetInstance()->SetApiServer(apiServer);
	}

	if(m_DefaultServerIndex < 0){
		connectErrorDialog("TIP_CONNECT_FAIL");
	}else{
		auto view = getViewRoot("cocosstudio/voyageUI_start.csb");
		auto panel_start = view->getChildByName<Widget*>("panel_start");
		auto panel_server = view->getChildByName<Widget*>("panel_customize_charactor");
		int index = 0;
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i]->name == "na")
			{
				index = 1;
			}
			else if (servers[i]->name == "eu")
			{
				index = 3;
			}
			else
			{
				index = 2;
			}
			auto b_server = panel_server->getChildByName<Widget*>(StringUtils::format("panel_server_%d", (int)i + 1));
			b_server->addTouchEventListener(CC_CALLBACK_2(UIStart::menuCall_func, this));
			auto i_server_status = b_server->getChildByName<ImageView*>("image_status");
			auto t_server_status = b_server->getChildByName<Text*>("label_server_1");

			b_server->getChildByName<Text*>("label_server")->setString(SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_START_SELECT_SERVER_%d", index)]);
			b_server->setTag(i + 1);
			std::string st_status;
			std::string old_value;
			std::string new_value;
			time_t myTime = atoll(servers[i]->maintainEnd.c_str());
			tm *nowTime = localtime(&myTime);
			//0 服务器关闭状态，1 正常状态，2 维护状态
			switch (servers[i]->status)
			{
			case 0:
				i_server_status->loadTexture("cocosstudio/login_ui/social_720/offline.png");
				st_status = SINGLE_SHOP->getTipsInfo()["TIP_START_SERVER_STATUS_CLOSE"];
				break;
			case 1:
				i_server_status->loadTexture("cocosstudio/login_ui/social_720/online.png");
				st_status = SINGLE_SHOP->getTipsInfo()["TIP_START_SERVER_STATUS_OPEN"];
				break;
			case 2:
				i_server_status->loadTexture("cocosstudio/login_ui/social_720/maintain.png");
				st_status = SINGLE_SHOP->getTipsInfo()["TIP_START_SERVER_STATUS_MAINTAIN"];
				old_value = "[time]";
				new_value = new_value = StringUtils::format("%d/%d/%d %02d:%02d", nowTime->tm_mon + 1, nowTime->tm_mday, nowTime->tm_year + 1900, nowTime->tm_hour, nowTime->tm_min);
				repalce_all_ditinct(st_status, old_value, new_value);
				break;
			default:
				log("server status data error !!!");
				break;
			}
			t_server_status->setString(st_status);
		}

		
		//..............存储本地.....................
		int serverIndex  = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), m_DefaultServerIndex+1);
		if(serverIndex < 1 || serverIndex > 3){
			serverIndex = 1;
		}
		m_DefaultServerIndex = serverIndex - 1;
//		log("server index %d", serverIndex);
		auto panel_server_choose = panel_server->getChildByName<Widget*>(StringUtils::format("panel_server_%d", serverIndex));
		panel_server_choose->getChildByName<Widget*>("checkbox_1")->setVisible(true);
		panel_server_choose->setTouchEnabled(false);
		m_pTarget = panel_server_choose;
		//...................................................
		std::string st_tips = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_START_SELECT_SERVER_%d", serverIndex)];
		auto t_server = panel_start->getChildByName<Text*>("text_server_1");
		t_server->setString(st_tips);
		auto image_div = panel_start->getChildByName<ImageView*>("image_div");
		if (t_server->getBoundingBox().size.width > image_div->getBoundingBox().size.width)
		{
			image_div->setContentSize(Size(t_server->getBoundingBox().size.width, image_div->getBoundingBox().size.height));
		}
		//服务器状态不可用
		auto i_server_status = panel_start->getChildByName<ImageView*>("image_online");
		//0 服务器关闭状态，1 正常状态，2 维护状态
		switch (servers[serverIndex - 1]->status)
		{
		case 0:
			i_server_status->loadTexture("cocosstudio/login_ui/social_720/offline.png");
			break;
		case 1:
			setGameServer(servers[serverIndex - 1]);
			i_server_status->loadTexture("cocosstudio/login_ui/social_720/online.png");
			break;
		case 2:
			i_server_status->loadTexture("cocosstudio/login_ui/social_720/maintain.png");
			break;
		default:
			log("server status data error !!!");
			break;
		}
	
		if(serverIndex > 0 && serverIndex < 4){
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), serverIndex);
			UserDefault::getInstance()->flush();
		}
//		log("---Set Server Name:%s---", servers[serverIndex - 1]->name.data());
	}

	m_loadingNode->removeFromParent();
}

void UIStart::getServerConfig()
{
	m_loadingNode = UILoadingIndicator::create(this);
	cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
	std::string urlpath;

	if (ENVIRONMENT == PRODUCTION){
		urlpath = "http://pubapi.voyage.piistudio.com/status.php";
		if(USE_BAKCUP_SERVER || useBack){
				urlpath = "http://pubapi.voyage.piistudio.com/status.php?useback=1";
		}
	}else{
		urlpath = "http://pubapi-dev.voyage.piistudio.com/status.php";
	}
	request->setUrl(urlpath.c_str());
	request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(UIStart::onHttpRequestCompleted,this));


	request->setTag("log_test");

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
	m_loadingNode->showSelf();
}


void UIStart::openAccountType(bool isShow)
{
	auto view = getViewRoot("cocosstudio/voyageUI_start.csb");
	auto viewChooseAccount = view->getChildByName<Widget*>("panel_account_types");
	viewChooseAccount->setVisible(isShow);
}
void UIStart::openUpdateView()
{
	openView(COMMOM_COCOS_RES[C_VIEW_UPDATE_GAME],10100);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_UPDATE_GAME]);
	auto t_content = view->getChildByName<Text*>("label_content");
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title"));
	auto tip = SINGLE_SHOP->getTipsInfo();
	t_title->setString(tip["TIP_GAME_UPDATE_TITLE"]);
	t_content->setString(tip["TIP_GAME_UPDATE_CONTENT"]);
}
void UIStart::openWebView()
{
	openView(SETTING_COCOS_RES[VIEW_INFO_EULA], 10101);
	auto view = getViewRoot(SETTING_COCOS_RES[VIEW_INFO_EULA]);
	auto tip = SINGLE_SHOP->getTipsInfo();
	auto image_content_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_content_bg"));
	auto sizeContent = image_content_bg->getContentSize();
	m_viewIndex = VIEW_UPDATE;
	auto b_reject = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_reject"));
	auto b_accept = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_accept"));
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title_eula"));
	b_reject->setTitleText(tip["TIP_WEB_VIEW_BUTTON_1"]);
	b_accept->setTitleText(tip["TIP_WEB_VIEW_BUTTON_2"]);
	t_title->setString(tip["TIP_WEB_VIEW_TITLE"]);
	//webView网页控件
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{		
		cocos2d::experimental::ui::WebView *siteWeb = cocos2d::experimental::ui::WebView::create();
		image_content_bg->addChild(siteWeb);
		siteWeb->setTag(100);
		siteWeb->setLocalZOrder(-1);
		if (ENVIRONMENT == PRODUCTION){
			siteWeb->loadURL("http://pubapi.voyage.piistudio.com/updatehelp.php");
		}
		else{
			siteWeb->loadURL("http://pubapi-dev.voyage.piistudio.com/updatehelp.php");
		}
		siteWeb->setAnchorPoint(Vec2(0.5, 0.5));
		siteWeb->setContentSize(sizeContent);
		siteWeb->setPosition(Vec2(image_content_bg->getContentSize().width / 2, image_content_bg->getContentSize().height / 2));
		siteWeb->setScalesPageToFit(true);
		siteWeb->setOnShouldStartLoading([](cocos2d::experimental::ui::WebView*sender, const std::string &url){
			return true;
		});
		siteWeb->setOnDidFinishLoading([](cocos2d::experimental::ui::WebView *sender, const std::string &url){
		});
		siteWeb->setOnDidFailLoading([](cocos2d::experimental::ui::WebView *sender, const std::string &url){
		});
	}
#endif
}
