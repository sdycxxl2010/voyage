/*
 * This layer is not for loading, it's for server connection!
 */
#include "TVLoadingScene.h"
#include "ui/CocosGUI.h"
#include "ProtocolThread.h"
#include "UICreateRole.h"
#include "UILogin.h"
//#include "CCLuaEngine.h"
#include "UIRegister.h"
#include "TVSceneLoader.h"
#include "UISailManage.h"
#include "TVSailDefineVar.h"
#include "UITips.h"
#include "TVSingle.h"
#include "LuaSailScene.h"
#include "UIStart.h"

#include "UIInform.h"
#include "UIAccountSafe.h"
#include "Utils.h"
#include "UILoadingIndicator.h"
#include "UISocial.h"
#include "UISetting.h"

#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif

//测试数据
#define  GUIDE 1
TVLoadingScene::TVLoadingScene()
{
	m_bSendEmailLogin = false;
	m_userInfo = nullptr;
	n_fromAccountSafe = false;
	m_pTempWidget = nullptr;
	m_nunResult = nullptr;
}

TVLoadingScene::~TVLoadingScene()
{
	m_loading->removeFromParent();
	m_loading=nullptr;
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

Scene* TVLoadingScene::createLoadingScene(bool isAccountSafe)
{
	auto load = Scene::create();
	TVLoadingScene* ls = new TVLoadingScene();
	ls->n_fromAccountSafe = isAccountSafe;
	if (ls &&ls->init())
	{
		ls->autorelease();
		load->addChild(ls);
		return load;
	}
	CC_SAFE_DELETE(ls);
	return nullptr;
	
}
//data init start in here

bool TVLoadingScene::init()
{
	USING_NS_CC;
	using namespace ui;
	bool pRet = false;
	do 
	{
		m_loading = UILoadingIndicator::create(this,1);
		m_loading->showSelf();
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(TVLoadingScene::onServerEvent, this),this);
		
		scheduleOnce(schedule_selector(TVLoadingScene::sendEmailLogin), 0);
		pRet = true;
	} while (0);

	return pRet;
}

void TVLoadingScene::sendEmailLogin(float t)
{
	if (m_bSendEmailLogin){
		return;
	}
	m_bSendEmailLogin = true;

	const std::string & email = ProtocolThread::GetInstance()->getSavedEmail();
	const std::string & passwd = ProtocolThread::GetInstance()->getSavedPassword();
	log("LoadingScene::sendEmailLogin");
	if (ProtocolThread::GetInstance()->getGoogleAccount().length() > 0 && ProtocolThread::GetInstance()->getGoogleToken().length() > 0){
		ProtocolThread::GetInstance()->tokenLogin();
	}else if (email.size() > 0 && passwd.size() > 0){
		ProtocolThread::GetInstance()->userEmailLogin(email.c_str(), passwd.c_str());
	}
	else if(ProtocolThread::GetInstance()->isGoogleAccountChecking()){
		//android
		ProtocolThread::GetInstance()->setIsWaitingForGoogleToken(true);
		log("cocos waiting for google token");
	}else{
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		//auto scene = RegisterUserLayer::createScene();
		//Director::getInstance()->replaceScene(scene);

		//登陆与注册合并，直接跳到登录界面
		Director::getInstance()->replaceScene(UILogin::createScene());
	}
}

void TVLoadingScene::updateData(float f)
{

}

void TVLoadingScene::reConnect()
{
	//ProtocolThread::GetInstance()->reconnectServer();
	UIBasicLayer::reConnect();
}

void TVLoadingScene::exitGame()
{
#if WIN32
	Director::getInstance()->end();
#elif ANDROID
	exit(0);
#endif
}

void TVLoadingScene::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
	log("LoadingScene::onServerEvent,msg type:%d",msgType);
	
	if(msgType == PROTO_TYPE_EmailLoginResult){
		EmailLoginResult* result = (EmailLoginResult*)message;
		m_emailResult = result;
		if(result && result->userid != -1)
		{
			cocos2d::log("login succ,userid:%d",result->userid);
			if (!result->needcreatecharacter){
					m_userInfo = message;
					
					openChooseRoleView(result);
				//BasicLayer::onServerEvent(message,msgType);
			}else{
				cocos2d::log("need to create character");
				Director::getInstance()->replaceScene(UICreateRole::createScene());
			}
		}
		else if (result->failed == 2){
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
			//tip->setPosition(400,240);
			tip->setPositiveCallback(CC_CALLBACK_0(UIBasicLayer::gotoStorePage, this));
			this->addChild(tip, 10100);
		}else if (result->failed == 13)
		{
			//账户安全验证
			auto scecseaccount = UIAccountSafe::createAccountSafeLayer(n_fromAccountSafe);
			ProtocolThread::GetInstance()->connectServer();
			Director::getInstance()->replaceScene(scecseaccount);
		}
 		else if (result->failed == 99)
 		{
 			openWaitView();
 		}
		else
		{
			cocos2d::log("login failed,remaining tries.");
			Director::getInstance()->replaceScene(UILogin::createScene());
		}
	}else if(msgType == 0){
		//connect to server succeeded
		sendEmailLogin();
	}else if(msgType == -1){
		//connect to server failed		
 		UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_CONNECT_FAIL"].c_str(),2);
 		this->addChild(tip,10000);
		tip->setNegativeCallback(CC_CALLBACK_0(TVLoadingScene::exitGame,this));
		tip->setPositiveCallback(CC_CALLBACK_0(TVLoadingScene::reConnect,this));		
		//tip->setPositiveText("Retry");
	}else if(msgType == PROTO_TYPE_RegisterUserResult){
		RegisterUserResult*result = (RegisterUserResult*)message;
		if(result->sessionid != -1 && result->userid != -1){
			Director::getInstance()->replaceScene(UICreateRole::createScene());
		}else{
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);

			auto scene = UIRegister::createScene();
			Director::getInstance()->replaceScene(scene);
		}
	}
	else if (msgType == PROTO_TYPE_DeleteUserResult)
	{
		DeleteUserResult*result = (DeleteUserResult*)message;
		if (result->failed == 0){
			auto view = getViewRoot("cocosstudio/voyageUI_start_account.csb");
			auto listview_account = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_account"));
			auto no_role = dynamic_cast<Widget*>(Helper::seekWidgetByName(listview_account, "image_no_role"));
			no_role->setVisible(true);
			auto i_account = dynamic_cast<Widget*>(Helper::seekWidgetByName(listview_account, "image_account"));
			i_account->setVisible(false);
			listview_account->refreshView();
		}
		else if (result->failed == 3)
		{
			openSuccessOrFailDialog("TIP_LOGIN_DELETE_USER");
		}
		else
		{
			openSuccessOrFailDialog("TIP_CENTER_OPERATE_FAIL");
		}
	}
	else if (msgType == PROTO_TYPE_UpdateDataVersionResult)
	{
		UpdateDataVersionResult *result = (UpdateDataVersionResult*)message;
		if (result->failed == 0)
		{
			UIBasicLayer::onServerEvent(m_userInfo, PROTO_TYPE_EmailLoginResult);
		}
		else
		{
			//openSuccessOrFailDialog("TIP_CENTER_OPERATE_FAIL");
		}
	}
	else if (msgType == PROTO_TYPE_CheckDeleteCharactersNumResult)
	{
		CheckDeleteCharactersNumResult *result = (CheckDeleteCharactersNumResult*)message;
		if (result->failed == 0)
		{
			m_nunResult = result;
			SINGLE_HERO->m_iDeleteNum = result->remainnum;
			UIInform::getInstance()->openInformView(this);
		    openYesOrNO("TIP_LOADING_DEL_ROLE_TITLE", "TIP_LOADING_DEL_ROLE_CONTENT");

		}
	}
}
void TVLoadingScene::openYesOrNO(std::string title, std::string content, bool Changedcontent)
{
	openView(COMMOM_COCOS_RES[C_VIEW_MULTIPLELINES_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_MULTIPLELINES_CSB]);
	auto listview_content = view->getChildByName<ListView*>("listview_content");
	auto tipInfos = SINGLE_SHOP->getTipsInfo();
	std::string info = tipInfos[content];
	if (Changedcontent)
	{
		info = content;
	}
	auto t_content = dynamic_cast<Text*>(view->getChildByName("label_content_1"));
	t_content->setString(info);//内容
	Text* label_content = dynamic_cast<Text*>(t_content->clone());
	listview_content->pushBackCustomItem(label_content);
	t_content->setContentSize(Size(t_content->getContentSize().width, 10 + getLabelHight(t_content->getString(), t_content->getContentSize().width, t_content->getFontName(), t_content->getFontSize())));

	std::string deleteNum = SINGLE_SHOP->getTipsInfo()["TIP_LOADING_DEL_NUM"];
	std::string old_value = "[num]";
	std::string new_value = StringUtils::format("%d", SINGLE_HERO->m_iDeleteNum);;
	repalce_all_ditinct(deleteNum, old_value, new_value);
	label_content->setString(deleteNum);

	std::string titlecontent = tipInfos[title];
	auto t_titleContent = dynamic_cast<Text*>(view->getChildByName("label_repair"));
	t_titleContent->setString(titlecontent);

	auto btn_yes = view->getChildByName<Button*>("button_s_yes");
	btn_yes->addTouchEventListener(CC_CALLBACK_2(TVLoadingScene::menuCall_func, this));
	auto btn_no = view->getChildByName<Button*>("button_s_no");
	if (btn_no)
	{
		btn_no->addTouchEventListener(CC_CALLBACK_2(TVLoadingScene::menuCall_func, this));
	}
	this->setCameraMask(_cameraMask, true);

}
void TVLoadingScene::openChooseRoleView(EmailLoginResult* result)
{

	if (UserDefault::getInstance()->getBoolForKey(HAVE_OPEN_CHANGE_LOG)==true)
	{
		UISetting::getInstance()->openSettingLayer();
		UISetting::getInstance()->getChangeLogInfo(true);
	}
	
	openView("cocosstudio/voyageUI_start_account.csb");	
	auto view = getViewRoot("cocosstudio/voyageUI_start_account.csb");
	auto eamil = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title"));
	eamil->setString(ProtocolThread::GetInstance()->getLoginEmail());

	auto item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "Panel_account"));
	auto i_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_head"));
	auto i_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_flag"));
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_player_name"));
	auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_player_lv"));
	auto b_del = dynamic_cast<Button*>(Helper::seekWidgetByName(item, "button_del"));
	i_head->loadTexture(getPlayerIconPath(result->iconidx));
	i_flag->loadTexture(getCountryIconPath(result->nation));
	t_name->setString(result->heroname);
	auto i_no_role = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_no_role"));
	auto b_no = i_no_role->getChildByName<Button*>("button_head_no");
	b_no->setTouchEnabled(false);
	i_no_role->setTouchEnabled(true);
	i_no_role->addTouchEventListener(CC_CALLBACK_2(TVLoadingScene::menuCall_func, this));
	if (result->maintaskstatus < 9)
	{
		t_lv->setString(StringUtils::format("Lv. %d", EXP_NUM_TO_LEVEL(UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str(), 1))));
	}
	else
	{
		t_lv->setString(StringUtils::format("Lv. %d", EXP_NUM_TO_LEVEL(result->exp)));
	}

	b_del->setTag(result->cid);
	item->setTouchEnabled(false);

	auto winSize = Director::getInstance()->getWinSize();
	auto panel_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_bg"));
	auto width_num = panel_bg->getContentSize().width - winSize.width;
	panel_bg->runAction(RepeatForever::create(Sequence::createWithTwoActions(MoveBy::create(width_num / 10, Vec2(-width_num, 0))
		, MoveBy::create(width_num / 10, Vec2(width_num, 0)))));
	m_loading->removeFromParent();
	Utils::sendNewPlayerTrackingStep("go_into_role_list", result->nation);
}

void TVLoadingScene::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* widget = dynamic_cast<Widget*>(pSender);
	std::string name = widget->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//开始游戏
	if (isButton(button_play))
	{
		auto view = getViewRoot("cocosstudio/voyageUI_start_account.csb");
		auto no_role = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "image_no_role"));
		if (!no_role->isVisible())
		{	
			EmailLoginResult*result = (EmailLoginResult*)m_userInfo;
			if (result->n_popuserselectversions > 0 && result->currentcharacterdataversion < result->currentserverdataversion)
			{
				popUserSelectVersions(result->popuserselectversions[0]);				
			}else
			{
				UIBasicLayer::onServerEvent(m_userInfo, PROTO_TYPE_EmailLoginResult);
			}
			Utils::sendNewPlayerTrackingStep("play_game", result->nation);
		}
		else
		{
			Director::getInstance()->replaceScene(UICreateRole::createScene());
		}
		return;
	}
	//登出
	if (isButton(button_logout))
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SETTING_LOGOUT_TITLE", "TIP_SETTING_ACCOUNT_QUIT_GAME");
		m_nBtnIndex = INDEX_LOGOUT;
		return;
	}
	//删除角色
	if (isButton(button_del))
	{
		ProtocolThread::GetInstance()->checkDeleteCharactersNum(UILoadingIndicator::create(this));

		
		m_nBtnIndex = INDEX_DEL_ROLE;
		return;
	}
	//提示框的确定按钮
	if (isButton(button_confirm_yes) || isButton(button_s_yes))
	{
		if (m_nBtnIndex == INDEX_DEL_ROLE)
		{
			ProtocolThread::GetInstance()->deleteUser(2,UILoadingIndicator::create(this));
#if 0
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "SignOut", "()V");
		cocos2d::log("SignOut method id:%x,%x", jniInfo.methodID, jniInfo.classID);
		if (id)
		{
			jniInfo.env->CallStaticVoidMethod(jniInfo.classID, jniInfo.methodID);
		}
#endif
		closeView(COMMOM_COCOS_RES[C_VIEW_MULTIPLELINES_CSB]);
			return;
		}
		else if (m_nBtnIndex == INDEX_LOGOUT)
		{
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			ProtocolThread::GetInstance()->logout();
			ProtocolThread::GetInstance()->saveEmail("");
			ProtocolThread::GetInstance()->savePassword("");
			UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(HAVE_ACCPET_EULA).c_str(), true);
			UserDefault::getInstance()->purgeSharedUserDefault();
			UserDefault::getInstance()->flush();
			SINGLE_CHAT->reset();
			UISocial::getInstance()->reset();
			Director::getInstance()->replaceScene(UIStart::createScene());
#if ANDROID
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "SignOut", "()V");
		cocos2d::log("SignOut method id:%x,%x", jniInfo.methodID, jniInfo.classID);
		if (id)
		{
			jniInfo.env->CallStaticVoidMethod(jniInfo.classID, jniInfo.methodID);
		}
#endif
			return;
		}
		return;
	}
	//创建角色
	if (isButton(image_no_role))
	{
		Director::getInstance()->replaceScene(UICreateRole::createScene());
		return;
	}
	//关闭异地安全界面
	if (isButton(button_abnormal_close))
	{
		closeView();
		return;
	}
	//异地账户安全，重发
	if (isButton(button_abnormal_resend))
	{
		return;
	}
	//继续
	if (isButton(button_abnormal_continue))
	{
		return;
	}

	if (isButton(button_done_firstmate))
	{
		popUserSelectVersions(2);
		return;
	}

	//选择偏好战斗
	if (isButton(panel_battle))
	{
		m_pTempWidget->setTouchEnabled(true);
		auto checkbox_buss = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_pTempWidget, "checkbox_buss"));
		checkbox_buss->setSelected(false);
		m_pTempWidget = dynamic_cast<Widget*>(pSender);
		m_pTempWidget->setTouchEnabled(false);
		auto checkbox_battle = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_pTempWidget, "checkbox_battle"));
		checkbox_battle->setSelected(true);
		return;
	}
	//选择偏好经济
	if (isButton(panel_business))
	{
		m_pTempWidget->setTouchEnabled(true);
		auto checkbox_battle = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_pTempWidget, "checkbox_battle"));
		checkbox_battle->setSelected(false);
		m_pTempWidget = dynamic_cast<Widget*>(pSender);
		m_pTempWidget->setTouchEnabled(false);
		auto checkbox_buss = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_pTempWidget, "checkbox_buss"));
		checkbox_buss->setSelected(true);
		return;
	}
	if (isButton(button_s_no))
	{
		closeView();
		return;
	}
}
void  TVLoadingScene::openAbnormalAccount()
{
	openView(ABNORMAL_ACCOUNT_RES[VIEW_ABNOMAL_ACCOUNT]);
	auto view = getViewRoot(ABNORMAL_ACCOUNT_RES[VIEW_ABNOMAL_ACCOUNT]);
	auto i_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_content"));
	i_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_ABNORMAL_ACCOUNT_CONTENT"]);
	//m_loading->removeFromParent();
}

void TVLoadingScene::openWaitView()
{
	openView("cocosstudio/voyageUI_start_offline.csb");
	auto view = getViewRoot("cocosstudio/voyageUI_start_offline.csb");
	auto t_time =dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_time"));
	auto panel_bg = Helper::seekWidgetByName(view, "panel_bg");
	auto winSize = Director::getInstance()->getWinSize();
	auto width_num = panel_bg->getContentSize().width - winSize.width;
//	EmailLoginResult*result = ProtocolThread::GetInstance()->getLastLoginResult();
	m_waitTime = m_emailResult->waitforbattleseconds;
	int min = m_waitTime / 60;
	int sec = m_waitTime % 60;
	auto str = StringUtils::format("%d%d:%d%d",min/10,min%10,sec/10,sec%10);
	t_time->setString(str);
	panel_bg->runAction(RepeatForever::create(Sequence::createWithTwoActions(MoveBy::create(width_num / 10, Vec2(-width_num, 0))
		, MoveBy::create(width_num / 10, Vec2(width_num, 0)))));
	schedule(schedule_selector(TVLoadingScene::waitTime), 1);
	m_loading->removeFromParent();
}
void TVLoadingScene::waitTime(float dt)
{
	auto view = getViewRoot("cocosstudio/voyageUI_start_offline.csb");
	auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_time"));
	EmailLoginResult*result = (EmailLoginResult*)m_userInfo;
	m_waitTime--;
	int min = m_waitTime / 60;
	int sec = m_waitTime % 60;
	auto str = StringUtils::format("%d%d:%d%d", min / 10, min % 10, sec / 10, sec % 10);
	t_time->setString(str);
	if (m_waitTime <= 0)
	{
		unschedule(schedule_selector(TVLoadingScene::waitTime));
		const std::string & email = ProtocolThread::GetInstance()->getSavedEmail();
		const std::string & passwd = ProtocolThread::GetInstance()->getSavedPassword();
		ProtocolThread::GetInstance()->userEmailLogin(email.c_str(), passwd.c_str());
	}
}

void TVLoadingScene::popUserSelectVersions(int step)
{
	if (step == 1)
	{
		openView(CREATE_ROLE_RES[CREATE_ROLE_STEP_THREE]);
		auto view = getViewRoot(CREATE_ROLE_RES[CREATE_ROLE_STEP_THREE]);
		auto i_done_firstmate = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_done_firstmate"));
		auto i_previous_firstmate = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_previous_firstmate"));
		auto i_panel_battle = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_battle"));
		auto i_panel_business = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_business"));
		auto checkbox_battle = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(view, "checkbox_battle"));
		checkbox_battle->setSelected(true);
		m_pTempWidget = i_panel_battle;
		m_pTempWidget->setTouchEnabled(false);
	
		auto label_battle = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_battle"));
		auto label_business = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_business"));
		label_battle->setString(SINGLE_SHOP->getTipsInfo()["TIP_CREATROLE_FIRSTMATE_BATTLE_CONTENT"]);
		label_business->setString(SINGLE_SHOP->getTipsInfo()["TIP_CREATROLE_FIRSTMATE_BUSSINESS_CONTENT"]);
		i_done_firstmate->setPositionX(view->getBoundingBox().size.width / 2);
		i_previous_firstmate->setVisible(false);
		i_panel_battle->addTouchEventListener(CC_CALLBACK_2(TVLoadingScene::menuCall_func,this));
		i_panel_business->addTouchEventListener(CC_CALLBACK_2(TVLoadingScene::menuCall_func,this));
	}
	else
	{
		int hobby;
		if (m_pTempWidget->getName() == "panel_battle")
		{
			hobby = 1;
		}
		else
		{
			hobby = 2;
		}
		UpdateUserSelection*array[1];
		UpdateUserSelection select = UPDATE_USER_SELECTION__INIT;
		int choices[1];
		choices[0] = hobby;
		array[0] = &select;
		select.n_choices = 1;
		select.choices = choices;
		select.dataversion = 1;
		ProtocolThread::GetInstance()->updateDataVersion(array, 1, UILoadingIndicator::create(this));
	}
}
