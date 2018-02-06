#include "UIBasicLayer.h"
#include "UIStoryLine.h"
#include "TVSceneLoader.h"
#include "TVLoadingScene.h"
#include "UIStart.h"
#include "UITips.h"
#include "UISailManage.h"
#include "UINoviceStoryLine.h"
#include "UIMain.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UITavern.h"
#include "UIPalace.h"
#include "UIShipyard.h"
#include "UIPort.h"

#include "UITask.h"
#include "MainTaskManager.h"
#include "CompanionTaskManager.h"
#include "UIGuideTarven.h"
#include "UIGuideShipyard.h"
#include "UIGuidePalace.h"
#include "UIPause.h"
#include "UISetting.h"
#include "UISetting.h"
#include "Utils.h"
#include "UIWorldNotify.h"
#include "WarManage.h"
#include "UINationWarHUD.h"
#include "TVEventManager.h"

#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif

UIBasicLayer::UIBasicLayer() :
	m_vImage(nullptr),
	m_vRoot(nullptr),
	m_eUIType(UI_NONE),
	m_HideLabel(nullptr),
	m_pRoot(nullptr)
{
	m_isFirstScrollEvent = true;
	m_listviewCallback = nullptr;
	m_keyboardListener = nullptr;
	init();
	m_nEula = 0;
	m_pAllNeedControlWidget.clear();
	m_pAllNeedControlGuideWidget.clear();
	setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
n_sailorNotEnoughTag = WARNING_NONE;
}

UIBasicLayer::~UIBasicLayer()
{
	clearListViewCallback();
	//移除返回键监听器
	if (m_keyboardListener)
	{
		Director::getInstance()->getEventDispatcher()->removeEventListener(m_keyboardListener);
		m_keyboardListener = nullptr;
	}
}

void UIBasicLayer::onEnter()
{
	Layer::onEnter();
}

void UIBasicLayer::onExit()
{
	Layer::onExit();
}

bool UIBasicLayer::init()
{
	bool pRet = false;
	do 
	{
		//键盘监听
		if (!m_keyboardListener)
		{
			m_keyboardListener = EventListenerKeyboard::create();
			m_keyboardListener->onKeyReleased = CC_CALLBACK_2(UIBasicLayer::keyReleased, this);
			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_keyboardListener, this);
		}
		CC_BREAK_IF(!CCLayer::init());
		pRet = true;
	} while (0);
	return pRet;
}

void UIBasicLayer::setUIType(const UI_VIEW_TYPE type)
{
	m_eUIType = type;
}

void UIBasicLayer::playAudio() 
{
	switch (m_eUIType) 
	{
		//log("audio type: %d", m_eUIType);
		case UI_NONE:
			break;
		case UI_PORT:
		case UI_BANK:
		case UI_DOCK:
		case UI_WHARF:
		case UI_MAIL:
		case UI_CABIN:
		case UI_CENTER:
		case UI_EXCHANGE:
			SINGLE_AUDIO->vplayMusicByType(MUSIC_CITY);
			break;
		case UI_PUB:
			SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
			break;
		case UI_MAP:
			SINGLE_AUDIO->vplayMusicByType(MUSIC_SAILING);
			break;
		case UI_PALACE:
			SINGLE_AUDIO->vplayMusicByType(MUSIC_PALACE);
			break;
		case UI_BATTLE:
			SINGLE_AUDIO->vplayMusicByType(MUSIC_BATTLE);
			break;
		case UI_START:
			SINGLE_AUDIO->vplayMusicByType(MUSIC_START);
			break;
		case UI_ATTACK:
			SINGLE_AUDIO->vplayMusicByType(MUSIC_BEFORE_BATTLE);
			break;
		case UI_COUNTRY_WAR:
			SINGLE_AUDIO->vplayMusicByType(MUSIC_BATTLE);
			break;
		default:
			break;
	}
}

void UIBasicLayer::stopAudio()
{
	SINGLE_AUDIO->vstopBGMusic();
}

void UIBasicLayer::button_callBack()
{
	//TODO: use sceneLoading to do this.
	unregisterCallBack();
	if (ProtocolThread::GetInstance()->isInTutorial())
	{						
		int city_id = SINGLE_HERO->m_iCityID;
		int born_id = SINGLE_HERO->m_iBornCity;
		int stage = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());
        if (stage >= 1 && stage <= 8)
		{
			CHANGETO(SCENE_TAG::MAINGUID_TAG);
		}
			else
		{
			CHANGETO(SCENE_TAG::MAIN_TAG);
		}

	}
    else
	{	
		CHANGETO(SCENE_TAG::MAIN_TAG);
	}
	
}

bool UIBasicLayer::isNumber(const std::string str)
{
	int len = str.length();
	for (int i = 0; i < len; i++)
	{
		char chr = str[i];
		if (chr > '9' || chr < '0')
		{
			return false;
		}
	}
	return true;
}

void UIBasicLayer::gotoStorePage()
{
#if ANDROID
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "openGoogleMarketById", "()V");
	//log("StartGoogleLogin id:%x,%x,%d", jniInfo.methodID, jniInfo.classID, id);
	int succ = 0;
	if (id)
	{
		jniInfo.env->CallStaticVoidMethod(jniInfo.classID, jniInfo.methodID);
	}
#endif
}

void UIBasicLayer::exitGame()
{
#if 0
	exit(0);
#else
	Director::getInstance()->end();
#endif
}

void UIBasicLayer::gotoLoginPage(int succ ){
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
	auto scene = TVLoadingScene::createLoadingScene();
	Director::getInstance()->replaceScene(scene);

	ProtocolThread::GetInstance()->reconnectServer();
}

void UIBasicLayer::reConnect()
{
#if 0
		if(!ProtocolThread::GetInstance()->hasValidEmailAndPassword()){
			ProtocolThread::GetInstance()->registerGoogleLoginCallback(CC_CALLBACK_1(BasicLayer::gotoLoginPage, this));
			//StartGoogleLogin
			JniMethodInfo jniInfo;
			bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","StartGoogleLogin","()I");
			log("StartGoogleLogin id:%x,%x,%d",jniInfo.methodID,jniInfo.classID,id);
			int succ = 0;
			if(id)
			{
				if(jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID) == 1){
					succ =1;
				}
			}
			if(!succ){
				gotoLoginPage(2);
			}
		}

#else
		ProtocolThread::GetInstance()->reset();
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		TVEventManager::PushEvent(TVEventType::_NET_RECONNECT);
		auto scene = UIStart::createScene();
		Director::getInstance()->replaceScene(scene);
#endif
}

void UIBasicLayer::openReconnectDialog(const std::string name)
{
	std::string text = SINGLE_SHOP->getTipsInfo()[name];
	openView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT], 10004);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
	auto label_content = view->getChildByName<Text*>("label_content");
	label_content->setString(text);

	auto i_game_recc = view->getChildByName<Button*>("button_game_reconn");
	auto i_game_exit = view->getChildByName<Button*>("button_game_exit");
	i_game_recc->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_RECC_GAME"]);
	i_game_exit->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_EXIT_GAME"]);

	i_game_recc->addTouchEventListener(CC_CALLBACK_2(UIBasicLayer::reconnectGameOrExit,this));
	i_game_exit->addTouchEventListener(CC_CALLBACK_2(UIBasicLayer::reconnectGameOrExit,this));
	auto cameras = Director::getInstance()->getRunningScene()->getCameras();
	if (cameras.size() >= 2){
		auto flag = (unsigned short)cameras[cameras.size() - 1]->getCameraFlag();
		view->setCameraMask((unsigned short)cameras[cameras.size() - 1]->getCameraFlag(), true);
	}
}

void UIBasicLayer::openSuccessOrFailDialog(const std::string name,Node* parent/* = nullptr */,const int zorder/* =10001*/)
{
	std::string text = SINGLE_SHOP->getTipsInfo()[name];
	UITips* tip = UITips::createTip(text.c_str(),1);
	if (parent != nullptr)
	{
		parent->addChild(tip,zorder);
	}else
	{ 
		this->addChild(tip,zorder);
	}	
}

void UIBasicLayer::openSuccessOrFailDialogWithString(const std::string content,Node* parent/* = nullptr */,const int zorder/* =10001*/)
{
	UITips* tip = UITips::createTip(content.c_str(),1);
	if (parent != nullptr)
	{
		parent->addChild(tip,zorder);
	}else
	{
		this->addChild(tip,zorder);
	}
}
void UIBasicLayer::registerCallBack()
{
	ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIBasicLayer::onServerEvent, this),this);
}

void UIBasicLayer::unregisterCallBack()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

void UIBasicLayer::addEventForChildren(Widget* parent, std::string type)
{
	if (!parent)
	{
		return;
	}
	Vector<Node*> arrayRootChildren = parent->getChildren();
	for (auto obj: arrayRootChildren)
	{
		Widget* child = (Widget*)(obj);
		std::string name = child->getName();
		if (type == "button_" || type == "all")
		{
			if (name.find("button_") == 0)
			{				
				//child->addTouchEventListener(this,toucheventselector(BasicLayer::menuCall_func));
				child->addTouchEventListener(CC_CALLBACK_2(UIBasicLayer::menuCall_func,this));
				continue;
			}
		}
		if(type == "checkbox_" || type == "all")
		{
			if (name.find("checkbox_") == 0)
			{				
				//image->setTouchEnabled(true);
				CheckBox* checkbox = dynamic_cast<CheckBox*>(child);
				checkbox->addEventListenerCheckBox(this,checkboxselectedeventselector(UIBasicLayer::checkboxCall_func));
				continue;
			}
		}
		
		this->addEventForChildren(child,type);
	}
}

void UIBasicLayer::setOnlyoneDialogVisibleFromParent(Widget* parent,Widget* child)
{
	if (!parent)
	{
		return;
	}
	parent->setVisible(true);
	parent->setTouchEnabled(true);
	Vector<Node*> children = parent->getChildren();
	Widget* widget;
	for(auto obj : children)
	{
		if (dynamic_cast<Widget*>(obj) == child)
		{
			child->setVisible(true);
			setTouchabedForChildren(child,true);
			continue;
		}
		widget = dynamic_cast<Widget*>(obj);
		widget->setVisible(false);
		setTouchabedForChildren(widget,false);
	}
}

void UIBasicLayer::setTouchabedForChildren(Widget* parent,bool touchabled)
{
	if (!parent)
	{
		return;
	}
	parent->setTouchEnabled(touchabled);
	Vector<Node*> arrayRootChildren = parent->getChildren();
	
	for (auto obj : arrayRootChildren)
	{
		Widget* child = dynamic_cast<Widget*>(obj);
		std::string name = child->getName();
		if (name.find("button_") == 0 || name.find("checkbox_")== 0 || name.find("slider_")== 0)
		{
			child->setTouchEnabled(touchabled);
			continue;
		}
		if (name.find("pageview_")== 0 || name.find("listview_") == 0)
		{
			child->setTouchEnabled(touchabled);
		}
		this->setTouchabedForChildren(child,touchabled);
	}
}

int64_t	UIBasicLayer::getCurrentTimeUsev()
{
	timeval timeStruc;
	gettimeofday(&timeStruc,nullptr);
	int64_t sec1 = ((int64_t)timeStruc.tv_sec) * 1000;
	int64_t sec2 = timeStruc.tv_usec /1000;
	return sec1 + sec2;
}

void UIBasicLayer::checkboxCall_func(Ref* obj,CheckBoxEventType type)
{
	if (type == CheckBoxEventType::CHECKBOX_STATE_EVENT_SELECTED)
	{
		log("checkbox selected---");
	}else
	{
		log("checkbox unselect ----");
	}
}


void UIBasicLayer::addListViewBar(ListView* lv,ImageView* iv)
{
	Node *parent = lv->getParent();
	m_isFirstScrollEvent = true;
	lv->refreshView();
	if(parent != nullptr)
	{
		auto sroll = Helper::seekWidgetByName(iv, "button_pulldown");
		
		if (sroll != nullptr)
		{
			iv->setVisible(false);
			auto plusAll = 0;
			for (int i = 0; i < lv->getChildrenCount(); i++)
			{
				plusAll += lv->getItem(i)->getBoundingBox().size.height;
			}
			Size lv_size = lv->getContentSize();
			if (plusAll > lv_size.height)
			{
				Size innerContainer = lv->getInnerContainerSize();
				Size lv_size = lv->getContentSize();
				if (innerContainer.height < lv_size.height + 10) {
					iv->setVisible(false);
					return;
				}
				iv->setVisible(true);
			}else
			{
				iv->setVisible(false);
				return;
			}

			lv->setUserObject(sroll);
			lv->addEventListenerScrollView(this, scrollvieweventselector(UIBasicLayer::scrollEvent));
			sroll->setTouchEnabled(false);
			sroll->setAnchorPoint(Vec2(0,0));
			sroll->setPositionY(iv->getBoundingBox().size.height - sroll->getBoundingBox().size.height - 17);
		}
	}
}

void UIBasicLayer::scrollEvent(Ref* target,ScrollviewEventType type)
{
	if (type == ScrollviewEventType::SCROLLVIEW_EVENT_SCROLLING) {
		ListView *lv = dynamic_cast<ListView*>(target);
		Widget *sroll = dynamic_cast<Widget*>(lv->getUserObject());
		Size innerContainer = lv->getInnerContainerSize();
		Size lv_size = lv->getContentSize();
		if (lv->getUserObject() == nullptr) {
			lv->unscheduleAllSelectors();
			return;
		}
		
		Point innerCon_pos = lv->getInnerContainer()->getPosition();
		Point lv_pos = lv->getPosition();
		if (innerContainer.height < lv_size.height + 10) {
			lv->unscheduleAllSelectors();
			sroll->getParent()->setVisible(false);
			return;
		}
		if (!sroll->isVisible()) {
			sroll->setVisible(true);
			return;
		}
		Size img_size = sroll->getSize();
		auto len = dynamic_cast<Widget*>(sroll->getParent())->getSize().height - img_size.height - 17 * 2;
		auto factor = std::fabs(innerCon_pos.y * 1.0 / (innerContainer.height - lv_size.height));
		sroll->setPositionY(len * factor + 17);
	}
}

typedef enum tcp_conn_status_{
	TCP_WATCH_READ = 0,
	TCP_WATCH_WRITE = 1<<0,
	TCP_SHUTDOWN_AFTER_WRITE = 1<<1, //set when errer occured.
	TCP_SHUTDOWN_NO_WRITE = 1<<2, //client shutdown connection.
	TCP_HAS_TIMER = 1<<3,
	TCP_SHUTDOWN_AFTER_WRITE_REASON_MULTI_CONNECTION = 1<<4,
	TCP_SHUTDOWN_AFTER_WRITE_REASON_SYS_MAINTENANCE = 1<<5,
	TCP_SHUTDOWN_AFTER_WAIT_TIME = 1<<6,
}tcp_conn_status;


void UIBasicLayer::onServerEvent(struct ProtobufCMessage* message, int msgType) {
//	log("BasicLayer::onServerEvent type:%d",msgType);
	if (msgType == -1) {
		auto curScene = Director::getInstance()->getRunningScene();
		auto loading = curScene->getChildByTag(TAG_LAODING);
		if (loading) {
			loading->removeFromParentAndCleanup(true);
		}
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		log("network is disconnect...");
		openReconnectDialog("TIP_CONNECT_FAIL");
		return;
	}
	if (msgType == PROTO_TYPE_SysMessage) 
	{
		auto curScene = Director::getInstance()->getRunningScene();
		auto loading = curScene->getChildByTag(TAG_LAODING);
		if (loading) 
		{
			loading->removeFromParentAndCleanup(true);
		}
		SysMessage *sm = (SysMessage*) message;
		if (sm->type == 0) 
		{
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);

			auto layer = curScene->getChildByName("SysInfo");
			if (!layer)
			{
				layer = UIWorldNotify::create();
			}
			switch (sm->index)
			{
			case 0:
				openReconnectDialog("TIP_USER_LOGINED");
				break;
			case TCP_SHUTDOWN_AFTER_WRITE_REASON_MULTI_CONNECTION:
				openReconnectDialog("MULTI_CONNECTION");
				break;
			case TCP_SHUTDOWN_AFTER_WRITE_REASON_SYS_MAINTENANCE:
				openReconnectDialog("SYS_MAINTENANCE");
				break;
			case TCP_SHUTDOWN_AFTER_WAIT_TIME:
				SINGLE_HERO->m_Infoshowtime = 60.0;
				SINGLE_HERO->m_waitTime = atoi(sm->param);
				SINGLE_HERO->m_nSystemMessageType = SHUTDOWN_AFTER_WAIT_TIME;
				
				SYSTEM_IFNO s_item;
				s_item.sys_type = SHUTDOWN_AFTER_WAIT_TIME;
				s_item.nationId1 = 0;
				s_item.nationId2 = 0;
				s_item.cityId = 0;
				s_item.isWait = false;
				SINGLE_HERO->m_vSystemInfo.push_back(s_item);
				
				if (!layer->getParent())
				{
					curScene->addChild(layer, 10010);
				}
				break;
			default:
				break;
			}
			return;
		}
		else if (sm->type == 1)
		{
			rapidjson::Document root;
			root.Parse<0>(sm->param);
			
			int nation_a = 0;
			int nation_b = 0;
			int cityId = 0;
			
			switch (sm->index)
			{
			case 1:
				//国战战争结束
				nation_a = DictionaryHelper::getInstance()->getIntValue_json(root, "nation1");
				nation_b = DictionaryHelper::getInstance()->getIntValue_json(root, "nation2");
				SINGLE_HERO->m_nSystemMessageType = COUNTRY_WAR_OVER;
				break;
			case 2:
				//国战中城市沦陷
				cityId = DictionaryHelper::getInstance()->getIntValue_json(root, "city");
				SINGLE_HERO->m_nSystemMessageType = COUNTRY_WAR_CITY_FALLEN;
				break;
			case 3:
				//国战中战争发起
				nation_a = DictionaryHelper::getInstance()->getIntValue_json(root, "nation1");
				nation_b = DictionaryHelper::getInstance()->getIntValue_json(root, "nation2");
				SINGLE_HERO->m_nSystemMessageType = COUNTRY_WAR_START;
				break;
			case 4:
				//国战中进入战争阶段
				nation_a = DictionaryHelper::getInstance()->getIntValue_json(root, "nation1");
				nation_b = DictionaryHelper::getInstance()->getIntValue_json(root, "nation2");
				SINGLE_HERO->m_nSystemMessageType = COUNTRY_WAR_ING;
				break;
			default:
				break;
			}
			SINGLE_HERO->m_Infoshowtime = 60.0;
			SINGLE_HERO->m_waitTime = 0;

			SYSTEM_IFNO s_item;
			s_item.sys_type = SINGLE_HERO->m_nSystemMessageType;
			s_item.nationId1 = nation_a;
			s_item.nationId2 = nation_b;
			s_item.cityId = cityId;
			s_item.isWait = false;
			auto warManage = curScene->getChildByName("WarManage");
			if (warManage)
			{
				if (nation_a == SINGLE_HERO->m_iNation || nation_b == SINGLE_HERO->m_iNation || cityId == SINGLE_HERO->m_nAttackCityId)
				{
					s_item.isWait = true;
				}
			}
			
			SINGLE_HERO->m_vSystemInfo.push_back(s_item);

			auto layer = curScene->getChildByName("SysInfo");
			if (!layer)
			{
				layer = UIWorldNotify::create();
				curScene->addChild(layer, 10010);
			}
		}
	}
	else if (msgType == PROTO_TYPE_EmailLoginResult) 
	{
		EmailLoginResult*result = (EmailLoginResult*) message;
		if (result->shipx > 0 && result->shipy > 0) {
			int x = result->shipx;
			int y = result->shipy;
			auto scene = SailScene::createWithPhysics();
			scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
			UISailManage* map = UISailManage::create(Vec2(x, MAP_WIDTH * MAP_CELL_WIDTH - y), true);
			scene->addChild(map, 1, MAP_TAG+100);
			Director::getInstance()->replaceScene(scene);
		} else {
			Scene*currentScene = Director::getInstance()->getRunningScene();
			auto guideDialogLayer = UINoviceStoryLine::GetInstance();
			switch (result->maintaskstatus) {
			case GUIDE_START:
				if (!guideDialogLayer->getParent())
				{
					currentScene->addChild(guideDialogLayer, 10001);
				}
				guideDialogLayer->onGuideTaskMessage(1, 0);
				break;
			case GUIDE_SEA:
			case GUIDE_FIRST_CITY:		
			{
				int city_id = SINGLE_HERO->m_iCityID;
				int born_id = SINGLE_HERO->m_iBornCity;
				int m_needguide = ProtocolThread::GetInstance()->isInTutorial();
				int stage = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());
				if (stage == 0)
				{
					if (!guideDialogLayer->getParent())
					{
						currentScene->addChild(guideDialogLayer, 10001);
					}
					guideDialogLayer->onGuideTaskMessage(1, 0);
					return;
				}
                if (m_needguide && stage >= 1 && stage <= 8)
				{
					CHANGETO(SCENE_TAG::MAINGUID_TAG);
				}
				else
				{
					CHANGETO(SCENE_TAG::MAIN_TAG);
				}
			}
			break;
			default:
			// save data daily reward
			CHANGETO(SCENE_TAG::MAIN_TAG);
			break;
		 }
	 }
	}
	else if (msgType == PROTO_TYPE_CompleteDialogsResult)
	{
		auto result = (CompleteDialogsResult*)message;
		auto currentScene = Director::getInstance()->getRunningScene();
		auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
		auto mapLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
		auto puplayer = (UITavern*)(currentScene->getChildByTag(SCENE_TAG::PUPL_TAG + 100));
		auto palaceLayer = (UIPalace*)(currentScene->getChildByTag(SCENE_TAG::PALACE_TAG + 100));
		auto dockLayer = (UIShipyard*)(currentScene->getChildByTag(SCENE_TAG::DOCK_TAG + 100));
		auto wharfLayer = (UIPort*)(currentScene->getChildByTag(SCENE_TAG::WHARF_TAG + 100));
		if (result->failed == 0)
		{
			SINGLE_HERO->m_nTriggerId = 0;
			
			if (result->rewardcoin > 0 || result->rewardgold > 0)
			{
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushPlayerAddExpOrFrame(result->exp, result->fame, result->rewardexp, result->rewardfame);

				openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
				auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
				auto t_title = view->getChildByName<Text*>("label_title");
				auto p_result = view->getChildByName<Widget*>("panel_result");
				auto l_result = p_result->getChildByName<ListView*>("listview_result");
				auto p_item = p_result->getChildByName<Widget*>("panel_item_1");
				auto p_force = p_result->getChildByName<Widget*>("panel_sailor");
				auto p_coin = p_result->getChildByName<Widget*>("panel_coin");
				auto i_div = p_result->getChildByName<ImageView*>("image_div_1");
				t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_REWARD_TITLE"]);
			
				if (result->rewardcoin)
				{
					auto p_silver_clone = p_coin->clone();
					auto i_silver = p_silver_clone->getChildByName<ImageView*>("image_silver");
					auto t_silver_num = p_silver_clone->getChildByName<Text*>("label_buy_num");
					t_silver_num->setString(numSegment(StringUtils::format("+%lld", result->rewardcoin)));
					l_result->pushBackCustomItem(p_silver_clone);
					auto i_div3 = i_div->clone();
					l_result->pushBackCustomItem(i_div3);
				}
				
				if (result->rewardgold)
				{
					auto p_gold_clone = p_coin->clone();
					auto i_gold = p_gold_clone->getChildByName<ImageView*>("image_silver");
					auto t_gold_num = p_gold_clone->getChildByName<Text*>("label_buy_num");
					i_gold->ignoreContentAdaptWithSize(false);
					i_gold->loadTexture(getVticketOrCoinPath(10000, 1));
					t_gold_num->setString(numSegment(StringUtils::format("+%lld", result->rewardgold)));
					l_result->pushBackCustomItem(p_gold_clone);
					auto i_div4 = i_div->clone();
					l_result->pushBackCustomItem(i_div4);
				}
			
				auto b_yes = view->getChildByName<Button*>("button_result_yes");
				b_yes->addTouchEventListener(CC_CALLBACK_2(UIBasicLayer::closeRawardEvent, this));
				SINGLE_HERO->m_iCoin = result->coins;
				SINGLE_HERO->m_iGold = result->golds;
				
				if (mainlayer)
				{
					mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
				}
			}
			if (mainlayer)
			{
				mainlayer->notifyCompleted(SMALL_STORY);
			}
			else if (mapLayer)
			{
				mapLayer->notifyCompleted(SMALL_STORY);
			}
			else if (puplayer)
			{
				//更新好感度状态特效
				puplayer->m_nFavour = result->bargirlprestige;
				puplayer->notifyCompleted(SMALL_STORY);
			}
			else if (palaceLayer)
			{
				palaceLayer->notifyCompleted(SMALL_STORY);
			}
			else if (dockLayer)
			{
				dockLayer->notifyCompleted(SMALL_STORY);
			}
			else if (wharfLayer)
			{
				wharfLayer->notifyCompleted(SMALL_STORY);
			}
		}
		else if (result->failed == 2)
		{
			//没有能够触发的技能
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_SERVER_DATA_FIAL");
			if (mainlayer)
			{
				mainlayer->notifyCompleted(SMALL_STORY);
			}
			else if (mapLayer)
			{
				mapLayer->notifyCompleted(SMALL_STORY);
			}
			else if (puplayer)
			{
				puplayer->notifyCompleted(SMALL_STORY);
			}
			else if (palaceLayer)
			{
				palaceLayer->notifyCompleted(SMALL_STORY);
			}
			else if (dockLayer)
			{
				dockLayer->notifyCompleted(SMALL_STORY);
			}
			else if (wharfLayer)
			{
				wharfLayer->notifyCompleted(SMALL_STORY);
			}
		}
		else if (result->failed == 3)
		{
			//不符合条件
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_SERVER_DATA_FIAL");
			if (mainlayer)
			{
				mainlayer->notifyCompleted(SMALL_STORY);
			}
			else if (mapLayer)
			{
				mapLayer->notifyCompleted(SMALL_STORY);
			}
			else if (puplayer)
			{
				puplayer->notifyCompleted(SMALL_STORY);
			}
			else if (palaceLayer)
			{
				palaceLayer->notifyCompleted(SMALL_STORY);
			}
			else if (dockLayer)
			{
				dockLayer->notifyCompleted(SMALL_STORY);
			}
			else if (wharfLayer)
			{
				wharfLayer->notifyCompleted(SMALL_STORY);
			}
		}
	}
}

void UIBasicLayer::closeRawardEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		auto button = static_cast<Widget*>(pSender);
		std::string name = button->getName();
		closeView();
		UICommon::getInstance()->closeView();
		auto currentScene = Director::getInstance()->getRunningScene();
		auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
		auto mapLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
		if (mainlayer)
		{
			mainlayer->notifyCompleted(SMALL_STORY);
		}
		else if (mapLayer)
		{
			mapLayer->notifyCompleted(SMALL_STORY);
		}
		return;
	}
}

void UIBasicLayer::getItemNameAndPath(const int itemtype,const int itemid,std::string& name,std::string& path)
{
	if (itemtype == ITEM_TYPE_GOOD)
	{
		name = getGoodsName(itemid);
		path = getGoodsIconPath(itemid,IMAGE_ICON_OUTLINE);
	}else if(itemtype == ITEM_TYPE_SHIP)
	{
		name = getShipName(itemid);
		path = getShipIconPath(itemid,IMAGE_ICON_OUTLINE);
	}else
	{
		name = getItemName(itemid);
		path = getItemIconPath(itemid,IMAGE_ICON_OUTLINE);
	}
}

void UIBasicLayer::openGoodInfo(const GetItemsDetailInfoResult *result,const int type,const int id)
{
	switch (type)
	{
	case ITEM_TYPE_GOOD://goods
		{
			UICommon::getInstance()->openCommonView(this);	
			if(result)
			{
				UICommon::getInstance()->flushGoodsDetail(result->goods,result->itemid,true);
			}else
			{
				UICommon::getInstance()->flushGoodsDetail(nullptr,id,true);
			}
			break;
		}
	case ITEM_TYPE_SHIP://ship
		{
			UICommon::getInstance()->openCommonView(this);
			if (result)
			{
				UICommon::getInstance()->flushShipDetail(result->ship,result->itemid,true);
			}else
			{
				UICommon::getInstance()->flushShipDetail(nullptr,id,true);
			}
			break;
		}
	case ITEM_TYPE_SHIP_EQUIP://equip
		{
			UICommon::getInstance()->openCommonView(this);
			if (result)
			{
				UICommon::getInstance()->flushEquipsDetail(result->equipment,result->itemid,true);
			}else
			{
				UICommon::getInstance()->flushEquipsDetail(nullptr,id,true);
			}
			break;
		}
	case ITEM_TYPE_PORP://prop
		{
			UICommon::getInstance()->openCommonView(this);
			if (result)
			{
				UICommon::getInstance()->flushItemsDetail(result->equipment,result->itemid,true);
			}else
			{
				UICommon::getInstance()->flushItemsDetail(nullptr,id,true);
			}
			break;
		}
	case ITEM_TYPE_ROLE_EQUIP:// role equip
		{
			UICommon::getInstance()->openCommonView(this);
			if (result)
			{
				UICommon::getInstance()->flushItemsDetail(result->equipment,result->itemid,true);
			}else
			{
				UICommon::getInstance()->flushItemsDetail(nullptr,id,true);
			}
			break;
		}
	case ITEM_TYPE_DRAWING:// drawing
		{
			UICommon::getInstance()->openCommonView(this);
			if (result)
			{
				UICommon::getInstance()->flushDrawingDetail(result->drawing,result->itemid,true);
			}else
			{
				UICommon::getInstance()->flushDrawingDetail(nullptr,id,true);
			}
			break;
		}
	case ITEM_TYPE_SPECIAL:// special
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushSpecialDetail(id,true);
			break;
		}
	default:
		{
			break;
		}
	}
}

float UIBasicLayer::getLabelHight(const std::string content, const float width, const std::string foneName, const int fontSize)
{
	if (!m_HideLabel)
	{
		m_HideLabel = Label::create();
		this->addChild(m_HideLabel);
		m_HideLabel->setVisible(false);
	}
	m_HideLabel->setSystemFontSize(fontSize);
	m_HideLabel->setSystemFontName(foneName);
	m_HideLabel->setString(content);
	m_HideLabel->setWidth(width);
	return m_HideLabel->getBoundingBox().size.height;
}

void UIBasicLayer::setGLProgramState(Widget* target,const bool isGrey)
{
	auto fragmentFullPath = FileUtils::getInstance()->fullPathForFilename("example_greyScale.fsh");
	auto fragSource = FileUtils::getInstance()->getStringFromFile(fragmentFullPath);
	GLProgram* glprogram;
	if (isGrey)
	{
		glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert,fragSource.c_str());
	}else
	{
		glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert,ccPositionTextureColor_noMVP_frag);
	}
	auto glprogramstate = GLProgramState::getOrCreateWithGLProgram(glprogram);
	((ImageView*)target)->setScale9Enabled(false);
	((cocos2d::ui::Scale9Sprite*)target->getVirtualRenderer())->getSprite()->setGLProgramState(glprogramstate);
}

void UIBasicLayer::setTextProperty(TextAtlas *target, const int num, const std::string& charMapFile, int itemWidth, int itemHeight, const std::string& startCharMap)
{
	std::string st_num = StringUtils::format("%d",num);
	target->setProperty(st_num,charMapFile,itemWidth,itemHeight,startCharMap);
}

void UIBasicLayer::setTextColorFormIdAndType(Text* target, const int itemId, const int itemType, bool iswhite)
{
	int rarity = 1;
	switch (itemType)
	{
	case ITEM_TYPE_GOOD:
	case ITEM_TYPE_SPECIAL:
		rarity = 1;
		break;
	case ITEM_TYPE_SHIP:
		rarity = SINGLE_SHOP->getShipData()[itemId].rarity;
		break;
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
	case ITEM_TYPE_DRAWING:
		rarity = SINGLE_SHOP->getItemData()[itemId].rarity;
		break;
	default:
		break;
	}

	Color4B Text_Color;
	switch (rarity)
	{
	case 1://普通
		if (iswhite)
		{
			Text_Color = Color4B(255, 255, 255, 255);
		}
		else
		{
			Text_Color = Color4B(40, 25, 13, 255);
		}
		break;
	case 2://高级
		Text_Color = Color4B(40, 160, 45, 255);
		break;
	case 3://稀有
		Text_Color = Color4B(42, 117, 169, 255);
		break;
	case 4://传奇
		Text_Color = Color4B(154, 71, 218, 255);
		break;
	case 5://史诗
		Text_Color = Color4B(195, 101, 6, 255);
		break;
	default:
		break;
	}
	if (target)
	{
		target->setTextColor(Text_Color);
	}
}

void UIBasicLayer::setBgButtonFormIdAndType(Widget* target,const int itemId,const int itemType)
{
	int rarity = 1;
	switch (itemType)
	{
	case ITEM_TYPE_GOOD:
	case ITEM_TYPE_SPECIAL:
		rarity = 1;
		break;
	case ITEM_TYPE_SHIP:
		rarity = SINGLE_SHOP->getShipData()[itemId].rarity;
		break;
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
	case ITEM_TYPE_DRAWING:
		rarity = SINGLE_SHOP->getItemData()[itemId].rarity;
		break;
	default:
		break;
	}
	target->setVisible(true);
	auto i_target = dynamic_cast<ImageView*>(target);
	if (i_target)
	{
		i_target->loadTexture(StringUtils::format("res/rarity/bgButton_%d.png",rarity));
	}
	
	auto b_target = dynamic_cast<Button*>(target);
	if (b_target)
	{
		b_target->loadTextureNormal(StringUtils::format("res/rarity/bgButton_%d.png",rarity));
	}
}

void UIBasicLayer::setBgImageColorFormIdAndType(ImageView* target,const int itemId,const int itemType)
{
	int rarity = 1;
	switch (itemType)
	{
	case ITEM_TYPE_GOOD:
	case ITEM_TYPE_SPECIAL:
		rarity = 1;
		break;
	case ITEM_TYPE_SHIP:
		rarity = SINGLE_SHOP->getShipData()[itemId].rarity;
		break;
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
	case ITEM_TYPE_DRAWING:
		rarity = SINGLE_SHOP->getItemData()[itemId].rarity;
		break;
	default:
		break;
	}
	if (target)
	{
		target->loadTexture(StringUtils::format("res/rarity/bgImage_%d.png",rarity));
		target->setVisible(false);
		/*
		if (rarity == 1)
		{
			target->setVisible(false);
		}else
		{
			target->setVisible(true);
		}
		*/
	}
}

void UIBasicLayer::addStrengtheningIcon(Widget* target)
{
	if (target)
	{
		auto flag = ImageView::create("res/rarity/goods_intensify.png");
		flag->setName("goods_intensify");
		flag->ignoreContentAdaptWithSize(false);
		flag->setContentSize(Size(target->getContentSize().width * 26.0 / 78, target->getContentSize().height * 26.0 / 78));
		flag->setPosition(Vec2(flag->getContentSize().width / 2, flag->getContentSize().height / 2));
		target->addChild(flag, 10000);
	}
}

void UIBasicLayer::openView(const std::string filePath,const int zOrder)
{
	bool isfind = false;
	int i = 0;
	for (; i < m_vFilePath.size(); i++)
	{
		if (m_vFilePath.at(i) == filePath)
		{
			isfind = true;
			break;
		}
	}
	
	if (isfind)
	{
		m_pRoot = dynamic_cast<Widget*>(m_vRoots.at(i)->getChildren().at(0));
	}else
	{
		auto fakeRoot = CSLoader::createNode(filePath);
		m_pRoot = dynamic_cast<Widget*>(fakeRoot->getChildren().at(0));
		m_vFilePath.push_back(filePath);
		fakeRoot->removeFromParentAndCleanup(true);
		this->addChild(fakeRoot,zOrder);
		if (SINGLE_HERO->m_heroIsOnsea)
		{
			fakeRoot->setCameraMask(4);
		}
		else
		{
			fakeRoot->setCameraMask(1);
		}
		addEventForChildren(m_pRoot,"all");
		m_vRoots.push_back(fakeRoot);
	}
}

void UIBasicLayer::closeView()
{
	if (m_vRoots.empty())
	{
//		log("Check m_vRoots error!!!");
		return;
	}
	auto curRoot = m_vRoots.at(m_vRoots.size() - 1);
	m_vRoots.erase(m_vRoots.end()-1);
	m_vFilePath.erase(m_vFilePath.end()-1);
	curRoot->removeFromParentAndCleanup(true);
	if (!m_vRoots.empty())
	{
		m_pRoot = dynamic_cast<Widget*>(m_vRoots.at(m_vRoots.size() - 1)->getChildren().at(0));
	}
}

void UIBasicLayer::closeView(const std::string filePath,const float fTime)
{
	bool isfind = false;
	int i = 0;
	for (; i < m_vFilePath.size(); i++)
	{
		if (m_vFilePath.at(i) == filePath)
		{
			isfind = true;
			break;
		}
	}
	if (isfind)
	{
		auto curRoot = m_vRoots.at(i);
		m_vRoots.erase(m_vRoots.begin()+i);
		m_vFilePath.erase(m_vFilePath.begin()+i);	
		if (fTime == 0)
		{
			curRoot->removeFromParentAndCleanup(true);
		}
		else
		{
			curRoot->runAction(Sequence::createWithTwoActions(DelayTime::create(fTime), CallFuncN::create(CC_CALLBACK_0(UIBasicLayer::removeView, this, curRoot))));
		}
		
		if (!m_vRoots.empty())
		{
			m_pRoot = dynamic_cast<Widget*>(m_vRoots.at(m_vRoots.size() - 1)->getChildren().at(0));
		}
	}else
	{
//		log("check m_vRoots error!!!");
	}
}

void UIBasicLayer::removeView(Node *view)
{
	view->removeFromParentAndCleanup(true);
}

Widget* UIBasicLayer::getViewRoot(const std::string filePath)
{
	Widget* curRoot = nullptr;
	for (int i = 0; i < m_vFilePath.size(); i++)
	{
		if (m_vFilePath.at(i) == filePath)
		{
			curRoot = dynamic_cast<Widget*>(m_vRoots.at(i)->getChildren().at(0));
			break;
		}
	}
	return curRoot;
}

void UIBasicLayer::setViewZOrder(const std::string filePath,const int zOrder)
{
	for (int i = 0; i < m_vFilePath.size(); i++)
	{
		if (m_vFilePath.at(i) == filePath)
		{
			m_vRoots.at(i)->setLocalZOrder(zOrder);
			break;
		}
	}
}

Node* UIBasicLayer::getView(const std::string filePath)
{
	Node* fakeRoot = nullptr;
	for (int i = 0; i < m_vFilePath.size(); i++)
	{
		if (m_vFilePath.at(i) == filePath)
		{
			fakeRoot = m_vRoots.at(i);
			break;
		}
	}
	return fakeRoot;
}
int UIBasicLayer::chatGetUtf8CharLen(const char*utf8)
{
	int plusNum = 0;
	if (utf8 && utf8[0]){
		plusNum = 1;
		unsigned char byte = (unsigned char)utf8[0];
		if (byte > 0xf0 && byte <= 0xf7) {
			plusNum = 4;
		}
		else if (byte > 0xe0 && byte <= 0xef)
		{
			plusNum = 3;
		}
		else if (byte > 0xb0 && byte <= 0xcf)
		{
			plusNum = 2;
		}
	}
	return plusNum;
}
std::string UIBasicLayer::numSegment(std::string numstr)
{
	std::vector<std::string> sub_str;
	std::string change_str = "";
	int comma_in = numstr.find('.');
	int minus_in = numstr.find('-');
	bool isminus = false;
	if (minus_in != -1)
	{
		numstr = numstr.substr(minus_in + 1, std::string::npos);
		isminus = true;
		if (numstr.size() <= 3 && comma_in == -1)
		{
			return "-" + numstr;
		}
	}

	if (comma_in > 0)
	{
		int pos = numstr.find_first_of(".");
		if (pos > 3)
		{
			std::string str = numstr.substr(0, pos);
			std::string str_1 = numstr.substr(pos, (numstr.size() - pos));
			int b = str.size();
			for (int i = 0; i < pos / 3; i++)
			{

				sub_str.push_back(str.substr(b - 3, 3));
				b -= 3;
			}
			if (pos % 3 > 0)
			{
				sub_str.push_back(str.substr(0, str.size() % 3));
			}
			for (int i = sub_str.size() - 1; i >= 0; i--)
			{
				change_str += sub_str.at(i);
				if (i != 0)
				{
					change_str += ",";
				}
			}
			if (!isminus)
			{
				return  change_str += str_1;
			}
			else
			{
				return "-" + change_str += str_1;
			}

		}
		else
		{
			if (!isminus)
			{
				return numstr;
			}
			else
			{
				return "-" + numstr;
			}
		}

	}
	else
	{
		if (numstr.size() > 3)
		{
			int b = numstr.size();
			for (int i = 0; i < numstr.size() / 3; i++)
			{

				sub_str.push_back(numstr.substr(b - 3, 3));
				b -= 3;
			}
			if (numstr.size() % 3 > 0)
			{
				sub_str.push_back(numstr.substr(0, numstr.size() % 3));
			}

			for (int i = sub_str.size() - 1; i >= 0; i--)
			{
				change_str += sub_str.at(i);
				if (i != 0)
				{
					change_str += ",";
				}

			}
			if (!isminus)
			{
				return change_str;
			}
			else
			{
				return "-" + change_str;
			}

		}

	}

	return numstr;
}
void UIBasicLayer::addequipBrokenIcon(Widget*target)
{
	if (target)
	{
		auto flag = ImageView::create("res/rarity/equip_broken.png");
		flag->setName("equip_broken_icon");
		flag->ignoreContentAdaptWithSize(false);
		//flag->setContentSize(Size(target->getContentSize().width * 26.0 / 78, target->getContentSize().height * 26.0 / 78));
		target->addChild(flag, 10000);
		flag->setPosition(Vec2(target->getContentSize().width-flag->getContentSize().width / 2, target->getContentSize().height-flag->getContentSize().height / 2));
		
	}
}
void UIBasicLayer::setBrokenEquipRed(Widget* target,bool isRed)
{
	auto fragmentFullPath = FileUtils::getInstance()->fullPathForFilename("color.fsh");
	auto fragSource = FileUtils::getInstance()->getStringFromFile(fragmentFullPath);
	GLProgram* glprogram;
	if (isRed)
	{
		glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
	}
	else
	{
		glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, ccPositionTextureColor_noMVP_frag);
	}
	auto glprogramstate = GLProgramState::getOrCreateWithGLProgram(glprogram);
	((ImageView*)target)->setScale9Enabled(false);
	((cocos2d::ui::Scale9Sprite*)target->getVirtualRenderer())->getSprite()->setGLProgramState(glprogramstate);
}

/*
 * 谁能告诉我这函数干嘛的？参数干嘛的？
 */
void UIBasicLayer::addNeedControlWidgetForHideOrShow(Widget* target,bool isClear)
{
	if (isClear)
	{
		m_pAllNeedControlWidget.clear();
		UIStoryLine::GetInstance()->m_pAllNeedControlWidget.clear();
		UITask::getInstance()->m_pAllNeedControlWidget.clear();
	}
	if (target)
	{
		m_pAllNeedControlWidget.push_back(target);
		UIStoryLine::GetInstance()->m_pAllNeedControlWidget.push_back(target);
		UITask::getInstance()->m_pAllNeedControlWidget.push_back(target);
	}
}
void UIBasicLayer::guideAddNeedControlWidgetForHideOrShow(Widget* target, bool isClear)
{
	if (isClear)
	{
		m_pAllNeedControlGuideWidget.clear();
		UINoviceStoryLine::GetInstance()->m_pAllNeedControlWidget.clear();
	}
	if (target)
	{
		m_pAllNeedControlGuideWidget.push_back(target);
		UINoviceStoryLine::GetInstance()->m_pAllNeedControlWidget.push_back(target);
	}
}
void UIBasicLayer::reconnectGameOrExit(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto target = dynamic_cast<Widget*>(pSender);
	std::string name = target->getName();
	closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
	if (isButton(button_game_reconn))
	{
		reConnect();
		return;
	}
	if (isButton(button_game_exit))
	{
		exitGame();
		return;
	}
}
void UIBasicLayer::allTaskInfo(bool isShow, JobIndex index, float time,bool isCommit)
{
	if (isCommit)
	{
		switch (index)
		{
		case UIBasicLayer::NONE:
			break;
		case UIBasicLayer::MAIN_STORY:
			MainTaskManager::GetInstance()->completeMainTask();
			break;
		case UIBasicLayer::COMPANION_STORY:
			CompanionTaskManager::GetInstance()->completeCompanionTask();
			break;
		case UIBasicLayer::SMALL_STORY:
			ProtocolThread::GetInstance()->completeDialogs(SINGLE_HERO->m_nTriggerId);
			break;
		default:
			break;
		}
	}
	auto currentScene = Director::getInstance()->getRunningScene();
	auto pupLayer = (UITavern*)(currentScene->getChildByTag(SCENE_TAG::PUPL_TAG + 100));
	auto dockLayer = (UIShipyard*)(currentScene->getChildByTag(SCENE_TAG::DOCK_TAG + 100));
	auto palaceLayer = (UIPalace*)(currentScene->getChildByTag(SCENE_TAG::PALACE_TAG + 100));
	std::mutex mt;
	mt.lock();
	for (size_t i = 0; i < m_pAllNeedControlWidget.size(); i++)
	{
		if (m_pAllNeedControlWidget[i] && m_pAllNeedControlWidget[i]->getParent())
		{
			Vector<Node*> arrayRootChildren = m_pAllNeedControlWidget[i]->getChildren();
			
			for (auto obj : arrayRootChildren)
			{
				Widget* child = dynamic_cast<Widget*>(obj);
				if (isShow)
				{
					child->runAction(Sequence::createWithTwoActions(DelayTime::create(time), FadeIn::create(0.5f)));
					if (pupLayer)
					{
						if (pupLayer->m_nViewIndex != pupLayer->UI_BOUNTY_BOARD)
						{
							auto viewTarven = pupLayer->getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
							auto p_bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
							if (p_bargirl_dialog)
							{
								p_bargirl_dialog->setVisible(true);
							}
							auto view = pupLayer->getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
							auto otherButton = view->getChildByName<Widget*>("panel_two_butter");
							otherButton->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), FadeOut::create(0.0001)));
						}
						if (pupLayer->m_pCh)
						{
							pupLayer->m_pCh->setVisible(true);
						}
					}
					else if (dockLayer)
					{
						auto view = dockLayer->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
						auto p_bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
						if (p_bargirl_dialog)
						{
							p_bargirl_dialog->setVisible(true);
						}
					}
					else if (palaceLayer)
					{
						auto viewCsb = palaceLayer->getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
						auto panel_palace = viewCsb->getChildByName<Widget*>("panel_palace");
						auto panel_two_butter = palaceLayer->getChildByName<Widget*>("panel_two_butter");
						panel_palace->setVisible(true);
						panel_two_butter->setVisible(true);
					}
				}
				else
				{
					child->runAction(Sequence::createWithTwoActions(DelayTime::create(time), FadeOut::create(0.5f)));
					if (pupLayer)
					{
						auto viewTarven = pupLayer->getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
						auto p_bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
						if (p_bargirl_dialog)
						{
							p_bargirl_dialog->setVisible(false);
						}
						if (pupLayer->m_pCh)
						{
							pupLayer->m_pCh->setVisible(false);
						}
					}
					else if (dockLayer)
					{
						auto view = dockLayer->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
						auto p_bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
						if (p_bargirl_dialog)
						{
							p_bargirl_dialog->setVisible(false);
						}
					}
					else if (palaceLayer)
					{
						auto viewCsb = palaceLayer->getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
						auto panel_palace = viewCsb->getChildByName<Widget*>("panel_palace");
						auto panel_two_butter = palaceLayer->getChildByName<Widget*>("panel_two_butter");
						panel_palace->setVisible(false);
						panel_two_butter->setVisible(false);
					}
				}
			}
		}
	}
	mt.unlock();
}
void UIBasicLayer::allGuideTaskInfo(bool isShow, float time)
{
	auto currentScene = Director::getInstance()->getRunningScene();
	auto pupLayer = (UIGuideTarven*)(currentScene->getChildByTag(SCENE_TAG::PUP_GUIDE_TAG + 100));
	auto dockLayer = (UIGuideShipyard*)(currentScene->getChildByTag(SCENE_TAG::DOCK_GUIDE_TAG + 100));
	auto palaceLayer = (UIGuidePalace*)(currentScene->getChildByTag(SCENE_TAG::PALACEGUID_TAG + 100));

	for (size_t i = 0; i < m_pAllNeedControlGuideWidget.size(); i++)
	{
		if (m_pAllNeedControlGuideWidget[i])
		{
			Vector<Node*> arrayRootChildren = m_pAllNeedControlGuideWidget[i]->getChildren();
			for (auto obj : arrayRootChildren)
			{
				Widget* child = dynamic_cast<Widget*>(obj);
				if (isShow)
				{
					child->runAction(Sequence::createWithTwoActions(DelayTime::create(time), FadeIn::create(0.5f)));
					if (pupLayer)
					{
					
							auto viewTarven = pupLayer->getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
							auto p_bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
							p_bargirl_dialog->setVisible(true);
							auto view = pupLayer->getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
							auto otherButton = view->getChildByName<Widget*>("panel_two_butter");
							otherButton->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), FadeOut::create(0.0001)));
					}
					else if (dockLayer)
					{
						auto view = dockLayer->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
						auto p_bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
						p_bargirl_dialog->setVisible(true);
					}
					else if (palaceLayer)
					{
						auto viewCsb = palaceLayer->getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
						auto panel_palace = viewCsb->getChildByName<Widget*>("panel_palace");
						auto panel_two_butter = palaceLayer->getChildByName<Widget*>("panel_two_butter");
						panel_palace->setVisible(true);
						panel_two_butter->setVisible(true);
					}
				}
				else
				{
					child->runAction(Sequence::createWithTwoActions(DelayTime::create(time), FadeOut::create(0.5f)));
					if (pupLayer)
					{
						auto viewTarven = pupLayer->getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
						auto p_bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
						p_bargirl_dialog->setVisible(false);
					}
					else if (dockLayer)
					{
						auto view = dockLayer->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
						auto p_bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
						p_bargirl_dialog->setVisible(false);
					}
					else if (palaceLayer)
					{
						auto viewCsb = palaceLayer->getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
						auto panel_palace = viewCsb->getChildByName<Widget*>("panel_palace");
						auto panel_two_butter = palaceLayer->getChildByName<Widget*>("panel_two_butter");
						panel_palace->setVisible(false);
						panel_two_butter->setVisible(false);
					}
				}
			}
		}
	}
}
void UIBasicLayer::notifyUIPartDelayTime(float time)
{
	Director::getInstance()->getScheduler()->schedule(schedule_selector(UIBasicLayer::notifyUIPartCompleted), this, 0.1f, 0.1f, time, false);
}

void UIBasicLayer::notifyUIPartCompleted(float t)
{
	Scene*currentScene = Director::getInstance()->getRunningScene();
	if (currentScene){
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(UIBasicLayer::notifyUIPartCompleted), this);
		auto dialogLayer = UIStoryLine::GetInstance();
		if (!dialogLayer->getParent())
		{
			currentScene->addChild(dialogLayer, 10001);
		}
		//回调设置，函数写死在basicLayer了。
		//dialogLayer->setCallBackLayer(this);
		dialogLayer->onTriggerDialogMessage(SINGLE_HERO->m_nTriggerId, 0);
	}
}

void UIBasicLayer::notifyCompleted(int actionIndex){

}

//键盘响应
void UIBasicLayer::keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	switch (keyCode)
	{
#ifdef LINUX
	case EventKeyboard::KeyCode::KEY_SPACE:
#endif
	case EventKeyboard::KeyCode::KEY_BACK:
		{
			auto curScene = Director::getInstance()->getRunningScene();
			auto warManage = dynamic_cast<WarManage*>(curScene->getChildByName("WarManage"));
			if (warManage)
			{
				auto warUI = warManage->getWarUI();
				if (warUI)
				{
					warUI->openTipsView(3);
				}
			}
			else
			{
				//用于特殊处理协议与暂停界面的层级关系 0：正常情况下 1:游戏开始时 2：设置中
				switch (m_nEula)
				{
				case 0:
				{
					UIPause::getInstance()->openGamePauseLayer();
					break;
				}
				case 1:
				{
					exitGame();
					break;
				}
				case 2:
				{
					for (size_t i = 0; i < UISetting::getInstance()->m_vFilePath.size(); i++)
					{
						UISetting::getInstance()->closeView();
					}
					UIPause::getInstance()->openGamePauseLayer();
					break;
				}
				default:
					break;
				}
			}

		
			break;
		}
	default:
		break;
	}
}

void UIBasicLayer::setTextSizeAndOutline(Text *target, int vlaue)
{
	if (target)
	{
		target->setString(StringUtils::format("%d", vlaue));
		target->enableOutline(Color4B::BLACK, 2);
		int index = 1;
		int cur_temp = vlaue;
		while ((cur_temp /= 10))
		{
			index++;
		}
		if (index > 4)
		{
			target->setFontSize(18);
		}
	}
	else
	{
		log("Text erorr !!!");
	}
}
int UIBasicLayer::my_strlen_utf8_c(char *s) {
	int i = 0, j = 0;
	while (s[i]) {
		if ((s[i] & 0xc0) != 0x80) j++;
		i++;
	}
	return j;
}
std::string UIBasicLayer::apostrophe(std::string content, int len)
{
	auto nameLen = my_strlen_utf8_c((char*)content.data());
	if (nameLen > len)
	{
		content = content.substr(0, len) + "...";
	}
	return content;
}

bool UIBasicLayer::isChineseCharacterIn(std::string str)
{
	int index = 0;

	while (index < str.length())
	{
		char ch = str.c_str()[index];

		if (ch > -127 && ch < 0)
		{
			return true;
		}
		else
		{
			index++;
		}
	}
	return false;
}

void UIBasicLayer::setTextFontSize(Text* target)
{
	if (target)
	{
		auto testTarget = Text::create(target->getString(),target->getFontName(),target->getFontSize());
		if (testTarget->getBoundingBox().size.width > target->getBoundingBox().size.width)
		{
			target->setFontSize((int)floor(target->getFontSize()* target->getBoundingBox().size.width / testTarget->getBoundingBox().size.width)-2);
		}
	}
}
