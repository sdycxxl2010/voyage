#include "UILogin.h"
#include "ServerManager.h"
#include "UITips.h"
#include "Utils.h"
#include "UICreateRole.h"
#include "UISocial.h"
#include "UICommon.h"
#include "UIInform.h"

#include "TVLoadingScene.h"
#include "UIStart.h"
#include "UIAccountSafe.h"

#ifdef ANDROID
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif

UILogin::UILogin()
{
	tryNumber = 100;
	m_inputParent = nullptr;
	m_userInfo = nullptr;
	m_nBtnIndex = -1;
	m_pTempWidget = nullptr;
	bIsEnter = false;
	m_nunResult = nullptr;
}

UILogin::~UILogin()
{
	unscheduleUpdate();
	unregisterCallBack();
}

bool UILogin::init()
{
	registerCallBack();
	openView("cocosstudio/voyageUI_login.csb");
	const std::string & email = ProtocolThread::GetInstance()->getSavedEmail();
	auto tf_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_player_name"));
	auto tf_passwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_password"));

    //tf_name->attachWithIME();

	auto t_passwd = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, "label_forget_password"));
	auto b_touch = Button::create();
	b_touch->setContentSize(t_passwd->getContentSize()*3);
	b_touch->setName("label_forget_password");
	b_touch->setTouchEnabled(true);
	b_touch->ignoreContentAdaptWithSize(false);
	b_touch->addTouchEventListener(CC_CALLBACK_2(UILogin::menuCall_func, this));
	t_passwd->addChild(b_touch);
	b_touch->setPosition(Vec2(t_passwd->getBoundingBox().size.width/2,t_passwd->getBoundingBox().size.height/2));
	m_inputParent = (Helper::seekWidgetByName(m_pRoot, "image_bg_dialog"));
	m_inputOldPostion = m_inputParent->getPosition();

	tf_passwd->setVisible(false);
	tf_name->setVisible(false);
	auto size_name = tf_name->getContentSize();
	cocos2d::ui::EditBox* editbox_name = nullptr;
	auto textfield_name = tf_name->getParent();
	if (!editbox_name)
	{
		editbox_name = cocos2d::ui::EditBox::create(Size(size_name), "input.png");
		textfield_name->addChild(editbox_name);
		editbox_name->setTag(1);
		editbox_name->setPosition(tf_name->getPosition());
		editbox_name->setAnchorPoint(tf_name->getAnchorPoint());
		editbox_name->setPlaceholderFontColor(Color3B(116,98,71));
		editbox_name->setInputMode(cocos2d::ui::EditBox::InputMode::EMAIL_ADDRESS);
		editbox_name->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		editbox_name->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
		editbox_name->setFontColor(Color3B(46, 29, 14));
		editbox_name->setName("editbox_name");
		editbox_name->setDelegate(this);
	
	}

	auto size_pwd = tf_passwd->getContentSize();
	cocos2d::ui::EditBox* editbox_password = nullptr;
	auto textfield_password = tf_passwd->getParent();
	if (!editbox_password)
	{
		editbox_password = cocos2d::ui::EditBox::create(Size(size_pwd), "input.png"); 
		textfield_password->addChild(editbox_password);
		editbox_password->setTag(2);
		editbox_password->setPosition(tf_passwd->getPosition());
		editbox_password->setAnchorPoint(tf_passwd->getAnchorPoint());
		editbox_password->setPlaceholderFontColor(Color3B(116,98,71));
		editbox_password->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
		editbox_password->setFontColor(Color3B(46, 29, 14));
		editbox_password->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		editbox_password->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		editbox_password->setName("editbox_password");
		editbox_password->setDelegate(this);
	}
#ifdef ANDROID
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","getDefaultAccount","()Ljava/lang/String;");
	//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
	if(id)
	{			
		jstring res = (jstring)jniInfo.env->CallStaticObjectMethod(jniInfo.classID, jniInfo.methodID);
		if(res){
			const char* smsList = jniInfo.env->GetStringUTFChars(res, NULL);			
			std::string defaultName(smsList);
			tf_name->setString(defaultName);
			jniInfo.env->ReleaseStringUTFChars(res, smsList);
		}else{
			tf_name->setPlaceHolder("please input you Email in here !");
		}			
	}
	else
	{
		tf_name->setPlaceHolder("please input you Email in here !");
	}
#endif

	if(email.size() > 0){
		editbox_name->setText(email.c_str());
	}
	const std::string & passwd = ProtocolThread::GetInstance()->getSavedPassword();
	if(passwd.size() > 0){
		editbox_password->setText(passwd.c_str());
	}else
	{
		editbox_name->setText("");
	}
	//tf_passwd->setPasswordEnabled(true);
	//tf_passwd->setPasswordStyleText("*");
	scheduleUpdate();
    return true;
}

UILogin* UILogin::create()
{
	auto login = new UILogin();
	if (login && login->init())
	{
		login->autorelease();
		return login;
	}
	CC_SAFE_DELETE(login);
	return nullptr;
}

void UILogin::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	std::string name = dynamic_cast<Widget*>(pSender)->getName();
	//sound
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//登陆
	if (isButton(button_login))
	{
		TextField* tf_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_name"));
		auto text_name = tf_name->getParent();
		cocos2d::ui::EditBox* editbox_name = dynamic_cast<cocos2d::ui::EditBox*>(text_name->getChildByTag(1));
		std::string name = editbox_name->getText();

		TextField* tf_pw = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_password"));
		auto text_pw = tf_pw->getParent();
		cocos2d::ui::EditBox* editbox_password = dynamic_cast<cocos2d::ui::EditBox*>(text_pw->getChildByTag(2));
		std::string passwd = editbox_password->getText();
		cocos2d::log("-- this is name and passwd :%s ,%s",name.c_str(),passwd.c_str());
		int idx1 = name.find("@");
		int idx2 = name.rfind(".");
		int len = name.length();
		if (idx1 == -1 || idx2 == -1 || idx2 < idx1 || name.length() < 6/* || idx2 >= len - 1*/)
		{
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_VAID"].c_str());
			this->addChild(tip,1);
		}else if(passwd.compare("Password") == 0)
		{
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_VAID"].c_str());
			this->addChild(tip,1);
		}else
		{
			Utils::sendNewPlayerTrackingStep("user_email_login", 0);
			log("LoginLayer::menuCall_func send userEmailLogin");
			ProtocolThread::GetInstance()->saveEmail(name.c_str());
			ProtocolThread::GetInstance()->savePassword(passwd.c_str());
			ProtocolThread::GetInstance()->userEmailLogin(name.c_str(),passwd.c_str(),UILoadingIndicator::create(this));
		}
	}
	//注册
	else if(isButton(button_register))
	{
		//Director::getInstance()->replaceScene(RegisterUserLayer::createScene());

		UserDefault::getInstance()->setIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO);
		UserDefault::getInstance()->setIntegerForKey(SOUND_EFFECT_KEY, OPEN_AUDIO);
		UserDefault::getInstance()->flush();
		registerUserAccount();
	}else if(isButton(label_forget_password))
	{
		//ProtocolThread::GetInstance()->changePassword(LoadingLayer::create(this));
		auto api = ServerManager::CurSelectedServer()->GetServerAddr();
		api += "/resetpwd.php";
		const char* apiStr = api.c_str();
#if ANDROID
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","OpenURL","(Ljava/lang/String;)V");
		log("OpenUrl id:%x,%x,%d",jniInfo.methodID,jniInfo.classID,id);
		if(id)
		{
			jstring url = jniInfo.env->NewStringUTF(apiStr);
			jniInfo.env->CallStaticVoidMethod(jniInfo.classID, jniInfo.methodID,url);
		}
#endif
	}
	//创建角色
	if (isButton(image_no_role))
	{
		Director::getInstance()->replaceScene(UICreateRole::createScene());
		return;
	}
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
			}
			else
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
		//InformView::getInstance()->openInformView(this);
		//InformView::getInstance()->openConfirmYesOrNO("TIP_LOADING_DEL_ROLE_TITLE", "TIP_LOADING_DEL_ROLE_CONTENT");
		m_nBtnIndex = INDEX_DEL_ROLE;
		return;
	}
	//确认
	if (isButton(button_confirm_yes) || isButton(button_s_yes))
	{
		if (m_nBtnIndex == INDEX_DEL_ROLE)
		{
			ProtocolThread::GetInstance()->deleteUser(2, UILoadingIndicator::create(this));
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

void UILogin::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
    if (msgType == PROTO_TYPE_EmailLoginResult)
	{
        EmailLoginResult* result = (EmailLoginResult*)message;
        //log("EmailLoginResult %d %d", result->failed, result->userid);
        if (result && result->userid != -1 && result->failed == 0)
		{
			//ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			if(!result->needcreatecharacter)
			{
				m_userInfo = message;
				openChooseRoleView(result);
				//BasicLayer::onServerEvent(message,msgType);

			}else
			{
				ProtocolThread::GetInstance()->unregisterMessageCallback(this);
				Director::getInstance()->replaceScene(UICreateRole::createScene());
			}
		}
		else if (result->failed == 13)
		{
			auto scene = UIAccountSafe::createAccountSafeLayer();
			Director::getInstance()->replaceScene(scene);
		}
		else
		{
			tryNumber = 100;
			if (result->failed == 1)
			{
				openSuccessOrFailDialog("TIP_LOGIN_NOT_USER_OR_PASSWD");
			}else if (result->failed == 2)
			{
				tryNumber = 0;
				//openSuccessOrFailDialog("TIP_LOGIN_UPDATE_CLIENT");
				std::string text = SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"];
				UITips* tip = UITips::createTip(text.c_str(),1);
				tip->setPositiveCallback(CC_CALLBACK_0(UIBasicLayer::gotoStorePage, this));
				this->addChild(tip,10001);
			}else if (result->failed == 3)
			{
				openSuccessOrFailDialog("TIP_LOGIN_ACCOUNT_DISABLED");
			}else if (result->failed == 4)
			{
				openSuccessOrFailDialog("TIP_LOGIN_WRONG_PASSWORD");
			}else if (result->failed == 5)
			{
				openSuccessOrFailDialog("TIP_LOGIN_NOT_EXIST_USER");
			}else if (result->failed == 6)
			{
				openSuccessOrFailDialog("TIP_LOGIN_NOT_EXIST_USER");
			}else if (result->failed == 9)
			{
				openSuccessOrFailDialog("TIP_LOGIN_GET_CHARACTER_ERROR");
			}else if (result->failed == 10)
			{
				openSuccessOrFailDialog("TIP_LOGIN_GET_CAEGO_ERROR");
			}else if (result->failed == 11)
			{
				openSuccessOrFailDialog("TIP_LOGIN_GET_SHIP_ERROR");
			}else if (result->failed == 12)
			{
				openSuccessOrFailDialog("TIP_LOGIN_KICKED_OUT");
			}else if (result->failed == 13)
			{
				openSuccessOrFailDialog("TIP_LOGIN_GET_CITY_DATA_ERROR");
			}else{
				ProtocolThread::GetInstance()->reconnectServer();
			}
			
			if (tryNumber > 0)
			{
				//ProtocolThread::GetInstance()->reconnectServer();
			}else{
				//ProtocolThread::GetInstance()->unregisterMessageCallback(this);				
			}
        }
    }else if (msgType == -1)
	{
		openReconnectDialog("TIP_CONNECT_FAIL");
	}else if (msgType == PROTO_TYPE_SysMessage)
	{
		SysMessage *sm = (SysMessage*)message;
		if(sm->index == 0 && sm->type == 0)
		{
			openReconnectDialog("TIP_USER_LOGINED");
			return;
		}else
		{

		}
	}else if (msgType == PROTO_TYPE_ChangePasswordResult)
	{
		ChangePasswordResult* result = (ChangePasswordResult*)message;
		if (result->failed == 0)
		{
			openSuccessOrFailDialog("TIP_LOGIN_SEND_EMAIL_SUCCESS");
		}else
		{
			openSuccessOrFailDialog("TIP_LOGIN_SEND_EMAIL_FAIL");
		}
	}
	else if (msgType == PROTO_TYPE_DeleteUserResult)
	{
		DeleteUserResult*result = (DeleteUserResult*)message;
		if (result->failed == 0){
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
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
	else if (msgType == PROTO_TYPE_RegisterUserResult)
	{
		RegisterUserResult* result = (RegisterUserResult*)message;
		if (result && result->userid != -1)
		{
			Utils::sendNewPlayerTrackingStep("register_user_succ", 0);
			Director::getInstance()->replaceScene(UICreateRole::createScene());
		}
		else
		{
			if (result->reason == 2)
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_REGISTED"].c_str());
				this->addChild(tip, 1);
			}
			else if (result->reason == 1)
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_VAID"].c_str());
				this->addChild(tip, 1);
			}
			else if (result->reason == 5)
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
				this->addChild(tip, 1);
			}
			else if (result->reason == 10)
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_REGOSTER_USER"].c_str());
				this->addChild(tip, 1);
			}
			else
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_REGISTED_FAIL"].c_str());
				this->addChild(tip, 1);
			}
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
void UILogin::openYesOrNO(std::string title, std::string content, bool Changedcontent)
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
	btn_yes->addTouchEventListener(CC_CALLBACK_2(UILogin::menuCall_func, this));
	auto btn_no = view->getChildByName<Button*>("button_s_no");
	if (btn_no)
	{
		btn_no->addTouchEventListener(CC_CALLBACK_2(UILogin::menuCall_func, this));
	}
	this->setCameraMask(_cameraMask, true);
}
Scene* UILogin::createScene()
{
	Scene* scene = Scene::create();
	if (scene)
	{
		UILogin* layer= UILogin::create();
		scene->addChild(layer);
		return scene;
	}
	return nullptr;
}

void UILogin::update(float dt)
{
	auto view = getViewRoot("cocosstudio/voyageUI_login.csb");

	if (view)
	{
		TextField* tf_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_player_name"));
		cocos2d::ui::EditBox* editbox_name = dynamic_cast<cocos2d::ui::EditBox*>(tf_name->getParent()->getChildByTag(1));
		std::string name = editbox_name->getText();

		TextField* tf_pw = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_player_password"));
		cocos2d::ui::EditBox* editbox_password = dynamic_cast<cocos2d::ui::EditBox*>(tf_pw->getParent()->getChildByTag(2));
		std::string passwd = editbox_password->getText();

		TrimAll(name);
		TrimAll(passwd);
		editbox_name->setText(name.c_str());
		editbox_password->setText(passwd.c_str());
		if (name.size() > 0)
		{
			editbox_name->setOpacity(255);
		}
		else
		{
			editbox_name->setOpacity(127);
		}

		if (illegal_character_check(name))
		{
			editbox_name->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}

		if (passwd.size() > 0)
		{
			editbox_password->setOpacity(255);
		}
		else
		{
			editbox_password->setOpacity(127);
		}

		if (illegal_character_check(passwd))
		{
			editbox_password->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
	}
}
void UILogin::textEvent(Ref*obj, bool type)
{
	if (type){
		bIsEnter = false;
		auto target = dynamic_cast<ui::EditBox*>(obj);
		if (target->getName()== "editbox_password")
		{
			std::string password = target->getText();
			if (strlen(password.data()) < 8 || strlen(password.data()) > 20)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_PASSWORD_FAIL");
			}
		}
		else if (target ->getName()== "editbox_name")
		{
			std::string email = target->getText();
			if (email == "" || email.find("@") == std::string::npos || email.find("@") == 0)
			{
				target->runAction(Sequence::create(TintTo::create(0.2f, Color3B(255, 0, 0)), DelayTime::create(0.5f), TintTo::create(0.2f, Color3B(46, 29, 14)), nullptr));
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_EAMIL_FAIL");
			}
		}
	}
}

void UILogin::openChooseRoleView(EmailLoginResult* result)
{
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
	i_no_role->addTouchEventListener(CC_CALLBACK_2(UILogin::menuCall_func, this));
	if (result->maintaskstatus < 10)
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

	Utils::sendNewPlayerTrackingStep("go_into_role_list", result->nation);
}
void UILogin::registerUserAccount()
{
	auto view = getViewRoot("cocosstudio/voyageUI_login.csb");
	TextField* userName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_name"));
	auto text_name = userName->getParent();
	ui::EditBox* editbox_name = dynamic_cast<cocos2d::ui::EditBox*>(text_name->getChildByTag(1));
	std::string email = editbox_name->getText();

	TextField* input_pwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_password"));
	auto text_pw = input_pwd->getParent();
	ui::EditBox* editbox_password = dynamic_cast<cocos2d::ui::EditBox*>(text_pw->getChildByTag(2));
	std::string password = editbox_password->getText();
	/*账号不合法*/
	if (email == "" || email.find("@") == std::string::npos || email.find("@") == 0)
	{
		//openSuccessOrFailDialog("TIP_EMAIL_VAID");
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_EAMIL_FAIL");
		Utils::sendNewPlayerTrackingStep("account_eamil_fail", 0);
		return;
	}
	/*密码不合法*/
	if (strlen(password.data()) < 8 || strlen(password.data()) > 20)
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_PASSWORD_FAIL");
		Utils::sendNewPlayerTrackingStep("account_password_fail", 0);
		return;
	}
	Utils::sendNewPlayerTrackingStep("account_register_user", 0);
	ProtocolThread::GetInstance()->registerUser(editbox_name->getText(), editbox_password->getText(), UILoadingIndicator::create(this));
}

void UILogin::popUserSelectVersions(int step)
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
		i_panel_battle->addTouchEventListener(CC_CALLBACK_2(TVLoadingScene::menuCall_func, dynamic_cast<TVLoadingScene*>(this)));
		i_panel_business->addTouchEventListener(CC_CALLBACK_2(TVLoadingScene::menuCall_func, dynamic_cast<TVLoadingScene*>(this)));
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
void UILogin::editBoxReturn(ui::EditBox* editBox)
{
	bIsEnter = true;
	this->textEvent(editBox, bIsEnter);
}
