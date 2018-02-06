#include "UISetting.h"
#include "Utils.h"
#include "UILogin.h"
#include "UIInform.h"
#include "UIStart.h"

#include "env.h"
#include "ui/CocosGUI.h"
#include "UISocial.h"

#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif

UISetting*UISetting::sm_settingLayer=nullptr;

UISetting::UISetting():
	m_nBtnIndex(0)
{
	m_pTextfiled = nullptr;
	m_ncustormBtnTag = 0;
	m_pNewPassword = nullptr;
	m_pReaptPassword = nullptr;
	m_pResetAccountContent = nullptr;
	init();
}

UISetting::~UISetting()
{
	sm_settingLayer=nullptr;
}
bool UISetting::init()
{
	if (UIBasicLayer::init())
	{
		return true;
	}
	return false;
}

void UISetting::onEnter()
{
	UIBasicLayer::onEnter();
	firstSettingView();
	ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UISetting::onServerEvent, this),this);
}
void UISetting::onExit()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
	UIBasicLayer::onExit();
}
void UISetting::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType!=Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto target = dynamic_cast<Widget*>(pSender);
	std::string name=target->getName();

	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(panel_music))//背景音乐
	{	
		auto c_music = target->getChildByName<CheckBox*>("checkbox_1");
		SINGLE_AUDIO->setBackgroundMusicON(!SINGLE_AUDIO->getBackgroundMusicON());
		if (SINGLE_AUDIO->getBackgroundMusicON())
		{
			UserDefault::getInstance()->setIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO);
			c_music->setSelected(true);
			playAudio();
		}
		else
		{
			UserDefault::getInstance()->setIntegerForKey(BG_MUSIC_KEY, CLOSE_AUDIO);
			c_music->setSelected(false);
			stopAudio();
		}
		UserDefault::getInstance()->flush();
		return;
	}

	if (isButton(panel_sound_effect))//音效
	{
		auto c_sound_effect = target->getChildByName<CheckBox*>("checkbox_2");
		SINGLE_AUDIO->setEffectON(!SINGLE_AUDIO->getEffectON());
		if (SINGLE_AUDIO->getEffectON())
		{
			UserDefault::getInstance()->setIntegerForKey(SOUND_EFFECT_KEY, OPEN_AUDIO);
			c_sound_effect->setSelected(true);
		}
		else
		{
			UserDefault::getInstance()->setIntegerForKey(SOUND_EFFECT_KEY, CLOSE_AUDIO);
			c_sound_effect->setSelected(false);
		}
		UserDefault::getInstance()->flush();
		return;
	}
	
	if (isButton(button_connect_googleplay)) //googleplay
	{

		return;
	}

	if (isButton(button_connect))//连接faceBook
	{
		Application::getInstance()->openURL("https://www.facebook.com/The-Voyage-1567475376864836/");
		return;
	}

	if (isButton(button_language))//语言
	{
		showLanguage();
		return;
	}

	if (isButton(button_account))//账户
	{
		openView(SETTING_COCOS_RES[VIEW_ACCOUNT_CSB]);
		auto viewAccount = getViewRoot(SETTING_COCOS_RES[VIEW_ACCOUNT_CSB]);
		viewAccount->setVisible(false);
		ProtocolThread::GetInstance()->getEmailVerifiedInfo(UILoadingIndicator::createWithMask(this, _cameraMask));
		
		return;
	}

	if (isButton(button_support))//客服
	{
//		customServiceFeedBack();
		std::string  emailContent = ProtocolThread::GetInstance()->getLoginEmail();
				Utils::ShowFreshDesk(emailContent, SINGLE_HERO->m_sName);
		return;
	}

	if (isButton(button_help))//帮助按钮
	{
		openView(SETTING_COCOS_RES[VIEW_HELP_CSB]);
		this->setCameraMask(_cameraMask, true);
		return;
	}

	if (isButton(button_about))//关于
	{
		showInfoPanel();
		return;
	}

	if (isButton(panel_language))//语言切换
	{
		UserDefault::getInstance()->setIntegerForKey(LANGUAGE_KEY, target->getTag());
		UserDefault::getInstance()->flush();
		showLanguage();
		return;
	}
	
	//help
	if(isButton(button_cutomer_service))//客户服务
	{
		closeView();
		openCustomService();
		return;
	}

	if(isButton(button_lengend))//图鉴
	{
		closeView();
		legendItemDetail();
		return;
	}
	//Info
	if (isButton(button_credits))//制作人员
	{
		openView(SETTING_COCOS_RES[VIEW_CREDITS_CSB]);
		this->setCameraMask(_cameraMask, true);
		return;
	}
	if(isButton(button_privacy))//隐私协议
	{
		
		return;
	}
	if(isButton(button_end))//最终用户许可协议
	{
		useLicenseAgreement();
		return;
	}
	if (isButton(button_agreement_close) || isButton(button_reject))//提交问题反馈页面的拒绝和关闭按钮
	{
		closeView();
		return;
	}
	if (isButton(button_accept))//提交问题反馈页面的发送按钮
	{
		closeView();
		return;
	}
	//account
	if(isButton(button_password))//修改密码
	{
		if (SINGLE_HERO->m_iMyEmailAccountVerified)
		{
			openView(SETTING_COCOS_RES[VIEW_SET_PASSWORD]);		
			openPassword();
		}
		else
		{
			m_nBtnIndex = INDEX_CHANG_PASWORD;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SETTING_PASSWORD_TITLE", "TIP_REST_PASSWORD_NEED_VERTIFY_ACCOUNT");
		}
		
		return;
	}
	if(isButton(button_logout))//登出
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SETTING_LOGOUT_TITLE","TIP_SETTING_ACCOUNT_QUIT_GAME");
		m_nBtnIndex=INDEX_QUIT_GAME;
#if ANDROID
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","SignOut","()V");
	cocos2d::log("SignOut method id:%x,%x",jniInfo.methodID,jniInfo.classID);
	if(id)
	{
		jniInfo.env->CallStaticVoidMethod(jniInfo.classID, jniInfo.methodID);
	}
#endif
		return;
	}
	if (isButton(button_reset_no))
	{
		closeView();
		return;
	}
	if (isButton(button_reset_yes))
	{
		
		std::string newPassWord = m_pNewPassword->getString();
		std::string repeatPassWord = m_pReaptPassword->getString();
		if (newPassWord.length()<8)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_PASSWORD_FAIL");
		}
		else if (newPassWord.length()>20)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_SETTING_CHANGE_PASSWORD_TOO_LONG");
		}
		else
		{
			if (newPassWord == repeatPassWord)
			{
				closeView();
				
				char *str = (char *)newPassWord.c_str();
				ProtocolThread::GetInstance()->changePassword(str, UILoadingIndicator::create(this));
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SETTING_CHANGE_PASSWORD_FAIL");
				m_pNewPassword->setString("");
				m_pReaptPassword->setString("");
			}
		}
		return;
	}
	if (isButton(button_confirm_yes))//提示框的确定按钮
	{
		if (m_nBtnIndex==INDEX_CHANG_PASWORD)
		{
			openResetAccount();
			//ProtocolThread::GetInstance()->changePassword("password",LoadingLayer::create(this));
			return;
		}
		else if (m_nBtnIndex==INDEX_QUIT_GAME)
		{
			closeView();
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			ProtocolThread::GetInstance()->logout();
			ProtocolThread::GetInstance()->saveEmail("");
			ProtocolThread::GetInstance()->savePassword("");
			UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(HAVE_ACCPET_EULA).c_str(), true);
			UserDefault::getInstance()->flush();
			SINGLE_CHAT->reset();
			UISocial::getInstance()->reset();
			Director::getInstance()->replaceScene(UIStart::createScene());
			return;
		}
		else if (m_nBtnIndex==INDEX_DELE_ACCOUNT)
		{
			ProtocolThread::GetInstance()->deleteUser(2,UILoadingIndicator::create(this));//暂不实现
			return;
		}
		
		return;
	}
	if (isButton(button_close))//关闭子页面
	{
		closeView();
		return;
	}
	if (isButton(button_Seting_close))//关闭设置界面
	{
		closeView(SETTING_COCOS_RES[VIEW_SET_CSB]);
		return;
	}
	if (isButton(button_backcity))//返回
	{
		closeView();
		return;
	}
	//认证按钮
	if (isButton(button_verify))
	{
		openResetAccount();
		return;
	}
	//更新日志
	if (isButton(button_update_log))
	{
		getChangeLogInfo();
		return;
	}
	//关闭日志
	if (isButton(button_village_close))
	{
		closeView();
		return;
	}
}
void UISetting::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_ChangePasswordResult:
		{
			ChangePasswordResult*result=(ChangePasswordResult*)message;
			if (result->failed==0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SETTING_CHANGE_PASSWORD_SUCCESS");
				ProtocolThread::GetInstance()->savePassword(result->password);
				UserDefault::getInstance()->flush();
			}
			else
			{
				openSuccessOrFailDialog("TIP_SETTING_ACCOUNT_CHANGE_PASSWORD_FAIL");
			}
			break;
		}
	case PROTO_TYPE_DeleteUserResult:
		{
			DeleteUserResult*result=(DeleteUserResult*)message;
			if (result->failed==0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SETTING_DELE_ACCOUNT_SUCC");
				Director::getInstance()->replaceScene(UILogin::createScene());
			}
			else if (result->failed == 3)
			{
				openSuccessOrFailDialog("TIP_LOGIN_DELETE_USER");
			}
			else
			{
				openSuccessOrFailDialog("TIP_SETTING_DELE_ACCOUNT_FAIL");
			}
			break;
		}
	case PROTO_TYPE_SendCustomServiceInfoResult:
	{
			SendCustomServiceInfoResult*result = (SendCustomServiceInfoResult*)message;
			if (result->failed ==0)
			{
				
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SETTING_FEEDBACK_ANSWER");
			}
			else
			{

			}
			break;
	}
	case PROTO_TYPE_AuthenticateAccountResult:
	{
			AuthenticateAccountResult*result = (AuthenticateAccountResult*)message;
			
			if (result->failed==0&&result->forsecurity ==0)
			{
				if (result->newemail)
				{
					ProtocolThread::GetInstance()->saveEmail(result->newemail);
					UserDefault::getInstance()->flush();
				}
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_SUCEESS");
			}
			else if (result->failed == 1)
			{
				//常规错误
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_FAIL");

			}
			else if (result->failed ==2)
			{
				//格式不对
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_FORMAT_WRONG");
			}
			else if (result->failed ==3)
			{
				//已经认证过
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_HAVE_VERTIFIED");
			}
			else if (result->failed ==4)
			{
				//别人注册过
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_OTHER_HAVE_SIGN");
			}
			break;
	}
	case PROTO_TYPE_GetEmailVerifiedInfoResult:
	{
		GetEmailVerifiedInfoResult*result = (GetEmailVerifiedInfoResult*)message;
		if (result->failed == 0)
		{
			if (result->authenticated)
			{
				SINGLE_HERO->m_iMyEmailAccountVerified = true;
			}
			else
			{
				SINGLE_HERO->m_iMyEmailAccountVerified = false;
			}
			viewMyAccount();
		}
		 break;
	}
	case PROTO_TYPE_GetChangeLogResult:
	{
			GetChangeLogResult*result = (GetChangeLogResult*)message;
			if (result->failed == 0)
			{
				UserDefault::getInstance()->setBoolForKey(HAVE_OPEN_CHANGE_LOG, false);
				UserDefault::getInstance()->flush();
				openChangLogView(result);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GET_CHANGE_LOG_FAILED");
			}
			break;
	}
	default:
		break;
	}

}
void UISetting::openSettingLayer(int cameraTag)
{
	setEula(0);
	auto currentScene = Director::getInstance()->getRunningScene();
	this->removeFromParentAndCleanup(true);
	currentScene->addChild(this,10002);
	this->setPosition(STARTPOS);
	this->setCameraMask(cameraTag, true);
	
}

void UISetting::firstSettingView()
{
	openView(SETTING_COCOS_RES[VIEW_SET_CSB]);
	auto viewSettingMain = getViewRoot(SETTING_COCOS_RES[VIEW_SET_CSB]);
	auto panel_music = viewSettingMain->getChildByName<Widget*>("panel_music");
	panel_music->addTouchEventListener(CC_CALLBACK_2(UISetting::menuCall_func,this));
	auto c_music = panel_music->getChildByName<CheckBox*>("checkbox_1");
	c_music->setTouchEnabled(false);

	auto panel_sound_effect = viewSettingMain->getChildByName<Widget*>("panel_sound_effect");
	panel_sound_effect->addTouchEventListener(CC_CALLBACK_2(UISetting::menuCall_func,this));
	auto c_sound_effect = panel_sound_effect->getChildByName<CheckBox*>("checkbox_2");
	c_sound_effect->setTouchEnabled(false);

	if (CCUserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO) == OPEN_AUDIO)
	{
		c_music->setSelected(true);
	}
	else
	{
		c_music->setSelected(false);
	}
	if (CCUserDefault::getInstance()->getIntegerForKey(SOUND_EFFECT_KEY, OPEN_AUDIO) == OPEN_AUDIO)
	{
		c_sound_effect->setSelected(true);
	}
	else
	{
		c_sound_effect->setSelected(false);
	}
	this->setCameraMask(_cameraMask, true);
}

void UISetting::showLanguage()
{
	openView(SETTING_COCOS_RES[VIEW_LANGUAGE]);
	auto view = getViewRoot(SETTING_COCOS_RES[VIEW_LANGUAGE]);
	auto cur_language = view->getChildByName<Text*>("label_1_2");
	auto l_language = view->getChildByName<ListView*>("listview_language");
	auto panel_english = dynamic_cast<Widget*>(l_language->getItem(0));
	panel_english->addTouchEventListener(CC_CALLBACK_2(UISetting::menuCall_func,this));
	panel_english->setTag((int)LanguageType::ENGLISH);
	auto c_english = panel_english->getChildByName<CheckBox*>("checkbox_1");
	c_english->setTouchEnabled(false);
	c_english->setSelected(false);
	auto t_english = panel_english->getChildByName<Text*>("label_Language1");
	t_english->setString(SINGLE_SHOP->getTipsInfo()["TIP_CHANGE_LANG_EN"]);
	auto panel_chinese_cn = dynamic_cast<Widget*>(l_language->getItem(1));
	panel_chinese_cn->addTouchEventListener(CC_CALLBACK_2(UISetting::menuCall_func,this));
	panel_chinese_cn->setTag((int)LanguageType::CHINESE);
	auto c_chinese_cn = panel_chinese_cn->getChildByName<CheckBox*>("checkbox_1");
	c_chinese_cn->setTouchEnabled(false);
	c_chinese_cn->setSelected(false);
	auto t_chinese_cn = panel_chinese_cn->getChildByName<Text*>("label_Language1");
	t_chinese_cn->setString(SINGLE_SHOP->getTipsInfo()["TIP_CHANGE_LANG_CN"]);
	auto panel_chinese_hk = dynamic_cast<Widget*>(l_language->getItem(2));
	panel_chinese_hk->addTouchEventListener(CC_CALLBACK_2(UISetting::menuCall_func,this));
	panel_chinese_hk->setTag((int)LanguageType::TRADITIONAL_CHINESE);
	auto c_chinese_hk = panel_chinese_hk->getChildByName<CheckBox*>("checkbox_1");
	c_chinese_hk->setTouchEnabled(false);
	c_chinese_hk->setSelected(false);
	auto t_chinese_hk = panel_chinese_hk->getChildByName<Text*>("label_Language1");
	t_chinese_hk->setString(SINGLE_SHOP->getTipsInfo()["TIP_CHANGE_LANG_HK"]);
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::ENGLISH:
		c_english->setSelected(true);
		cur_language->setString(SINGLE_SHOP->getTipsInfo()["TIP_CHANGE_LANG_EN"]);
		break;
	case cocos2d::LanguageType::CHINESE:
		c_chinese_cn->setSelected(true);
		cur_language->setString(SINGLE_SHOP->getTipsInfo()["TIP_CHANGE_LANG_CN"]);
		break;
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
		c_chinese_hk->setSelected(true);
		cur_language->setString(SINGLE_SHOP->getTipsInfo()["TIP_CHANGE_LANG_HK"]);
		break;
	default:
		c_english->setSelected(true);
		cur_language->setString(SINGLE_SHOP->getTipsInfo()["TIP_CHANGE_LANG_EN"]);
		break;
	}
	this->setCameraMask(_cameraMask, true);
}

void UISetting::showInfoPanel()
{
	openView(SETTING_COCOS_RES[VIEW_INFO_CSB]);
	auto viewInfo = getViewRoot(SETTING_COCOS_RES[VIEW_INFO_CSB]);
	auto label_gamename=viewInfo->getChildByName<Text*>("label_gamename");
	auto label_copyright=viewInfo->getChildByName<Text*>("label_copyright");
	
	auto panel_web=viewInfo->getChildByName("panel_web");
	auto label_web_1=panel_web->getChildByName<Text*>("label_web_1");
	label_gamename->setString("Voyage 1.1.336");
	label_copyright->setString("Copyright @ Pii Studio 2015 All Rights Reserved");
	label_web_1->setString("https://www.facebook.com/The-Voyage-1567475376864836/");
	//label_web_1->setAnchorPoint(Vec2(0.5,0.5));
	label_web_1->setTouchEnabled(true);
	label_web_1->addTouchEventListener(CC_CALLBACK_2(UISetting::WebSiteTouch,this));
	this->setCameraMask(_cameraMask, true);
}
void UISetting::WebSiteTouch(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType!=Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto website=dynamic_cast<Widget*>(pSender);
	std ::string name=website->getName();
	Size winSize = Director::getInstance()->getVisibleSize();
	if (isButton(label_web_1))
	{
		Application::getInstance()->openURL("https://www.facebook.com/The-Voyage-1567475376864836/");
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
// 		{
// 		}
// #endif
		return;
	}
}
void UISetting::openCustomService()
{
	openView(SETTING_COCOS_RES[VIEW_CUSTOMERSERVICE]);
	auto viewCustom = getViewRoot(SETTING_COCOS_RES[VIEW_CUSTOMERSERVICE]);
	auto i_button_close = viewCustom->getChildByName<Button*>("button_close");
	auto i_button_account = viewCustom->getChildByName<Button*>("button_account");
	auto i_button_billing = viewCustom->getChildByName<Button*>("button_billing");
	auto i_button_bug = viewCustom->getChildByName<Button*>("button_bug");
	auto i_button_feedback = viewCustom->getChildByName<Button*>("button_feedback");
	i_button_account->setTag(1);
	i_button_billing->setTag(2);
	i_button_bug->setTag(3);
	i_button_feedback->setTag(4);
	i_button_close->addTouchEventListener(CC_CALLBACK_2(UISetting::customServiceButton,this));
	i_button_account->addTouchEventListener(CC_CALLBACK_2(UISetting::customServiceButton, this));
	i_button_billing->addTouchEventListener(CC_CALLBACK_2(UISetting::customServiceButton, this));
	i_button_bug->addTouchEventListener(CC_CALLBACK_2(UISetting::customServiceButton, this));
	i_button_feedback->addTouchEventListener(CC_CALLBACK_2(UISetting::customServiceButton, this));
	this->setCameraMask(_cameraMask, true);
}
void UISetting::customServiceButton(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto target = dynamic_cast<Widget*>(pSender);
	std::string name = target->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_close))
	{
		closeView(SETTING_COCOS_RES[VIEW_CUSTOMERSERVICE]);
		return;
	}
	if (isButton(button_account) || isButton(button_billing) || isButton(button_bug) || isButton(button_feedback))
	{
		m_ncustormBtnTag = target->getTag();
		//closeView();	
		customServiceFeedBack();
		return;
	}
	if (isButton(button_send))
	{
		std::string strContent = m_pTextfiled->getString();
		if (illegal_character_check(strContent))
		{
			m_pTextfiled->setString("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
		else
		{
			char *str = (char *)strContent.c_str();
			if (strContent != "")
			{
				std::string  emailContent = ProtocolThread::GetInstance()->getSavedEmail();
				char*email = (char*)(emailContent.c_str());
				ProtocolThread::GetInstance()->sendCustomServiceInfo(m_ncustormBtnTag, str, email, UILoadingIndicator::create(this));
				closeView();
			}
			else
			{
				openSuccessOrFailDialog("TIP_SETTING_CUSTOMSERVICE_CONTENT_EMPTY");
			}
		}
		return;
	}
}
void UISetting::customServiceFeedBack(bool closeMainSet)
{
	if (closeMainSet)
	{
		auto viewMain = getViewRoot(SETTING_COCOS_RES[VIEW_SET_CSB]);
		if (viewMain)
		{
			closeView(SETTING_COCOS_RES[VIEW_SET_CSB]);
		}
	}
	openView(SETTING_COCOS_RES[VIEW_CUSTOMERSERVICE_WRITE]);
	auto viewFeedBack = getViewRoot(SETTING_COCOS_RES[VIEW_CUSTOMERSERVICE_WRITE]);
	auto i_button_send = dynamic_cast<Button*>(Helper::seekWidgetByName(viewFeedBack,"button_send"));
	i_button_send->addTouchEventListener(CC_CALLBACK_2(UISetting::customServiceButton,this));
	auto i_textfield_content = dynamic_cast<TextField*>(Helper::seekWidgetByName(viewFeedBack,"textfield_content"));
	m_pTextfiled = i_textfield_content;
	i_textfield_content->setPlaceHolderColor(Color4B(46,29,14,128));
	i_textfield_content->setPlaceHolder(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_CUSTOMSERVICE_FEEDBACK_CONTENT"]);

	auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewFeedBack, "image_pulldown"));
	image_pulldown->setVisible(false);
	this->setCameraMask(_cameraMask, true);
}
void UISetting::useLicenseAgreement()
{
	setEula(2);
	openView(SETTING_COCOS_RES[VIEW_INFO_EULA]);
	auto view = getViewRoot(SETTING_COCOS_RES[VIEW_INFO_EULA]);
	auto image_content_bg = view->getChildByName<ImageView*>("image_content_bg");//在此范围内显示内容
	auto sizeContent = image_content_bg->getContentSize();
	auto button_reject = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_reject"));
	auto button_accept = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_accept"));
	button_reject->setVisible(false);
	button_accept->setVisible(false);
	//webView网页控件
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		cocos2d::experimental::ui::WebView *siteWeb = cocos2d::experimental::ui::WebView::create();
		image_content_bg->addChild(siteWeb);
		siteWeb->setTag(100);
		siteWeb->setLocalZOrder(-1);
		siteWeb->loadURL("http://pubapi.voyage.piistudio.com/eula.php");
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

	this->setCameraMask(_cameraMask, true);
}
void UISetting::openPassword()
{
	auto view = getViewRoot(SETTING_COCOS_RES[VIEW_SET_PASSWORD]);
	auto new_password = view->getChildByName<TextField*>("textfield_new_password");
	auto repeat_password = view->getChildByName<TextField*>("textfield_repeat_password");
	m_pNewPassword = new_password;
	m_pReaptPassword = repeat_password;
	new_password->setTextVerticalAlignment(TextVAlignment::CENTER);
	repeat_password->setTextVerticalAlignment(TextVAlignment::CENTER);

	new_password->setPasswordStyleText("*");
	new_password->setPasswordEnabled(true);
	repeat_password->setPasswordStyleText("*");
	repeat_password->setPasswordEnabled(true);
	auto checkbox_show = view->getChildByName<CheckBox*>("checkbox_show");
	checkbox_show->addEventListenerCheckBox(this, checkboxselectedeventselector(UISetting::checkboxPassword));
}
void UISetting::checkboxPassword(Ref* target, CheckBoxEventType type)
{
	auto view = getViewRoot(SETTING_COCOS_RES[VIEW_SET_PASSWORD]);
	auto i_new_password = view->getChildByName<TextField*>("textfield_new_password");
	auto i_repeat_password = view->getChildByName<TextField*>("textfield_repeat_password");
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (type == CheckBoxEventType::CHECKBOX_STATE_EVENT_SELECTED)
	{		
		
		i_new_password->setPasswordEnabled(false);
		i_new_password->setString(i_new_password->getString());
		i_repeat_password->setPasswordEnabled(false);
		i_repeat_password->setString(i_repeat_password->getString());
		
	}
	else
	{
		i_new_password->setPasswordEnabled(true);
		i_new_password->setString(i_new_password->getString());
		i_repeat_password->setPasswordEnabled(true);
		i_repeat_password->setString(i_repeat_password->getString());
	}
}
void UISetting::viewMyAccount()
{
	auto view = getViewRoot(SETTING_COCOS_RES[VIEW_ACCOUNT_CSB]);
	if (view)
	{
		view->setVisible(true);
		auto cur_account = view->getChildByName<Text*>("label_1_2");
		auto hero_name = view->getChildByName<Text*>("label_2_2");
		cur_account->setString(ProtocolThread::GetInstance()->getLoginEmail());
		//cur_account->setString(ProtocolThread::GetInstance()->getSavedEmail());
		hero_name->setString(SINGLE_HERO->m_sName);

		auto cur_account_title = view->getChildByName<Text*>("label_1_1");
		auto cur_account_name_title = view->getChildByName<Text*>("label_2_1");
		//验证部分
		auto i_account_not_vertify = view->getChildByName<Text*>("label_1_3");
		auto i_btn_vertify = view->getChildByName<Button*>("button_verify");
		//是否验证成功
		if (SINGLE_HERO->m_iMyEmailAccountVerified)
		{

			hero_name->setVisible(true);
			cur_account_name_title->setVisible(true);
			i_account_not_vertify->setVisible(false);
			i_btn_vertify->setVisible(false);
		}
		else
		{

			hero_name->setVisible(false);
			cur_account_name_title->setVisible(false);
			i_account_not_vertify->setVisible(true);
			i_btn_vertify->setVisible(true);
		}
		this->setCameraMask(_cameraMask, true);
	}
	
}
void UISetting::verifyAccountBtnEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto target = dynamic_cast<Widget*>(pSender);
	std::string name = target->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	
	//确定修改账户
	if (isButton(button_verific_yes))
	{
	
		std::string emailContent = m_pResetAccountContent->getString();
		char*email = (char*)(emailContent.c_str());
		ProtocolThread::GetInstance()->authenticateAccount(email,0, UILoadingIndicator::createWithMask(this, _cameraMask));
		closeView();
		closeView(SETTING_COCOS_RES[VIEW_ACCOUNT_CSB]);
		return;
	}
	//关闭修改账户
	if (isButton(button_verific_no))
	{
		closeView();
		return;
	}
}
void UISetting::openResetAccount()
{
	openView(SETTING_COCOS_RES[VIEW_ACCOUNT_EMAIL]);
	auto viewEmail = getViewRoot(SETTING_COCOS_RES[VIEW_ACCOUNT_EMAIL]);
	auto i_no = dynamic_cast<Button*>(Helper::seekWidgetByName(viewEmail,"button_verific_no"));
	i_no->addTouchEventListener(CC_CALLBACK_2(UISetting::verifyAccountBtnEvent,this));
	auto i_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(viewEmail, "button_verific_yes"));
	i_yes->addTouchEventListener(CC_CALLBACK_2(UISetting::verifyAccountBtnEvent,this));
	auto i_account_tip = dynamic_cast<Text*>(Helper::seekWidgetByName(viewEmail, "label_account_tip"));
	i_account_tip->setString(SINGLE_SHOP->getTipsInfo()["TIP_VERIFY_ACCOUNT_EMAIL"]);

	auto i_textfield_account = dynamic_cast<TextField*>(Helper::seekWidgetByName(viewEmail,"textfield_email_account"));
	i_textfield_account->setTextVerticalAlignment(TextVAlignment::CENTER);
	//i_textfield_account->setString(ProtocolThread::GetInstance()->getLoginEmail());
	i_textfield_account->setString(ProtocolThread::GetInstance()->getSavedEmail());
	m_pResetAccountContent = i_textfield_account;
	this->setCameraMask(_cameraMask, true);
}
void UISetting::legendItemDetail()
{
	openView(SETTING_COCOS_RES[VIEW_LEGEND_CSB]);
	auto viewlegend = getViewRoot(SETTING_COCOS_RES[VIEW_LEGEND_CSB]);
	auto listview_content = dynamic_cast<ListView*>(viewlegend->getChildByName<ListView*>("listview_content"));
	auto panel_content = dynamic_cast<Widget*>(viewlegend->getChildByName("panel_content"));
	auto label_title_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_content,"label_title_1"));//分类标题
	auto image_div_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_content, "image_div_1"));//分隔线
	auto panel_legend_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_content, "panel_legend_1"));//属性部分
	auto panel_legend_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_content, "panel_legend_11"));//势力 部分，其它部分
	label_title_1->setTouchEnabled(true);
	listview_content->removeAllChildren();
	auto settingInfo = SINGLE_SHOP->getSettingLengend();


	std::vector<int>property_Ids;
	std::vector<int>force_Ids;
	std::vector<int>normal_Ids;
	std::map<int, SETTING_LENGEND>::iterator m1_Iter;
	property_Ids.clear();
	force_Ids.clear();
	normal_Ids.clear();
	for (m1_Iter = settingInfo.begin(); m1_Iter != settingInfo.end(); m1_Iter++)
	{
		if (m1_Iter->second.type== 0)
		{
			property_Ids.push_back(m1_Iter->first);
		}
		else if (m1_Iter->second.type == 1)
		{
			force_Ids.push_back(m1_Iter->first);
		}
		else if (m1_Iter->second.type == 2)
		{
			normal_Ids.push_back(m1_Iter->first);
		}
	}
	//加载属性图标
	auto label_title_property = dynamic_cast<Text*>(label_title_1->clone());
	label_title_property->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_LENGEND_TITLE_1"]);
	label_title_property->setSwallowTouches(false);
	listview_content->pushBackCustomItem(label_title_property);
	int lines_1 = (property_Ids.size() % 2 == 0) ? (property_Ids.size() / 2) : (property_Ids.size() / 2+ 1);
	for (int i = 0; i < lines_1;i++)
	{
		ListView *item_layer_1 = ListView::create();
		item_layer_1->setDirection(ListView::Direction::HORIZONTAL);
		item_layer_1->setContentSize(Size(1070, 100));	
		item_layer_1->setSwallowTouches(false);
		Widget*panel_pro = nullptr;
		for (int k = 0; k < 2;++k)
		{
			if (2 * i + k<property_Ids.size())
			{
				panel_pro= dynamic_cast<Widget*>(panel_legend_1->clone());
				panel_pro->setSwallowTouches(false);
				auto image_legend = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_pro,"image_legend"));
				auto label_total = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_pro, "label_total"));
				image_legend->ignoreContentAdaptWithSize(false);
				image_legend->loadTexture(settingInfo[property_Ids.at(2* i + k)].path); 
				if (settingInfo[property_Ids.at(2 * i + k)].id >= 7 && settingInfo[property_Ids.at(2 * i + k)].id<=10)
				{
					image_legend->setColor(Color3B(40,25,13));
				}
				label_total->setString(settingInfo[property_Ids.at(2 * i + k)].name);
				item_layer_1->pushBackCustomItem(panel_pro);
			}
			
		}
		listview_content->pushBackCustomItem(item_layer_1);
	}
	//加载势力图标
	auto image_div_1_clone = dynamic_cast<ImageView*>(image_div_1->clone());
	auto label_title_force = dynamic_cast<Text*>(label_title_1->clone());
	label_title_force->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_LENGEND_TITLE_2"]);
	label_title_force->setSwallowTouches(false);
	listview_content->pushBackCustomItem(image_div_1_clone);
	listview_content->pushBackCustomItem(label_title_force);
	int lines_2 = (force_Ids.size() % 2 == 0) ? (force_Ids.size() / 2) : (force_Ids.size() / 2 + 1);

	for (int i = 0; i < lines_2; i++)
	{
		ListView *item_layer_1 = ListView::create();
		item_layer_1->setDirection(ListView::Direction::HORIZONTAL);
		item_layer_1->setContentSize(Size(1070, 100));	
		item_layer_1->setSwallowTouches(false);
		Widget*panel_pro = nullptr;
		for (int k = 0; k < 2; ++k)
		{
			if (2 * i + k < force_Ids.size())
			{
				panel_pro = dynamic_cast<Widget*>(panel_legend_2->clone());
				panel_pro->setSwallowTouches(false);
				auto image_legend = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_pro, "image_legend"));
				auto label_total = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_pro, "label_total"));
				image_legend->ignoreContentAdaptWithSize(true);
				image_legend->loadTexture(settingInfo[force_Ids.at(2 * i + k)].path);
				label_total->setString(settingInfo[force_Ids.at(2 * i + k)].name);
				item_layer_1->pushBackCustomItem(panel_pro);
			}			
		}
		listview_content->pushBackCustomItem(item_layer_1);
	}

	//加载普通图标
	auto image_div_2_clone = dynamic_cast<ImageView*>(image_div_1->clone());
	auto label_title_normal = dynamic_cast<Text*>(label_title_1->clone());
	label_title_normal->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_LENGEND_TITLE_3"]);
	label_title_normal->setSwallowTouches(false);
	listview_content->pushBackCustomItem(image_div_2_clone);
	listview_content->pushBackCustomItem(label_title_normal);
	int lines_3 = (normal_Ids.size() % 2 == 0) ? (normal_Ids.size() / 2) : (normal_Ids.size() / 2 + 1);

	for (int i = 0; i < lines_3; i++)
	{
		ListView *item_layer_1 = ListView::create();
		item_layer_1->setDirection(ListView::Direction::HORIZONTAL);
		item_layer_1->setContentSize(Size(1070, 100));
		
		item_layer_1->setSwallowTouches(false);
		Widget*panel_pro = nullptr;
		for (int k = 0; k < 2; ++k)
		{
			if (2* i + k < normal_Ids.size())
			{
				panel_pro = dynamic_cast<Widget*>(panel_legend_2->clone());
				panel_pro->setSwallowTouches(false);
				auto image_legend = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_pro, "image_legend"));
				auto label_total = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_pro, "label_total"));
				image_legend->ignoreContentAdaptWithSize(true);
				image_legend->loadTexture(settingInfo[normal_Ids.at(2* i + k)].path);
				label_total->setString(settingInfo[normal_Ids.at(2* i + k)].name);
				item_layer_1->pushBackCustomItem(panel_pro);
			}
		}
		listview_content->pushBackCustomItem(item_layer_1);
	}

	auto image_pulldown = viewlegend->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listview_content, image_pulldown);
	this->setCameraMask(_cameraMask, true);
}
void UISetting::openChangLogView(GetChangeLogResult*logResult)
{
	openView(ABNORMAL_ACCOUNT_RES[VIEW_CHANGE_LOG]);
	auto view = getViewRoot(ABNORMAL_ACCOUNT_RES[VIEW_CHANGE_LOG]);

	auto image_titile_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_titile_bg"));
	auto label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(image_titile_bg, "label_title"));
	label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_GET_CHANGE_LOG_CONTENT_TITLE"]);
	auto listView_1 = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listView_1"));
	listView_1->removeAllChildrenWithCleanup(true);
	auto labe_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "labe_content"));

	rapidjson::Document root;
	if (root.Parse<0>(logResult->changelog_str).HasParseError()){
		return;
	}
	if (root.IsNull())
		return;
	if (DictionaryHelper::getInstance()->checkObjectExist_json(root))
	{
		auto versionName = DictionaryHelper::getInstance()->getStringValue_json(root, "version_name");
		auto t_change_title = dynamic_cast<Text*>(labe_content->clone());
		t_change_title->setString(versionName);
		t_change_title->setContentSize(Size(labe_content->getContentSize().width, getLabelHight(
			t_change_title->getString(), labe_content->getContentSize().width, labe_content->getFontName()) + 20));
		listView_1->pushBackCustomItem(t_change_title);

		auto changesCount = DictionaryHelper::getInstance()->getArrayCount_json(root, "changes");
		for (int i = 0; i < changesCount;i++)
		{
			std::string changeLogContent = DictionaryHelper::getInstance()->getStringValueFromArray_json(root, "changes", i);
			auto t_change_clone = dynamic_cast<Text*>(labe_content->clone());
			
			t_change_clone->setString("* " + changeLogContent);
			t_change_clone->setContentSize(Size(labe_content->getContentSize().width, getLabelHight(
				t_change_clone->getString(), labe_content->getContentSize().width, labe_content->getFontName()) + 10));
			listView_1->pushBackCustomItem(t_change_clone);
		}
	}
	listView_1->refreshView();
	this->setCameraMask(_cameraMask, true);
}
void UISetting::getChangeLogInfo(bool closeMainSet)
{
	if (closeMainSet)
	{
		auto viewMain = getViewRoot(SETTING_COCOS_RES[VIEW_SET_CSB]);
		if (viewMain)
		{
			closeView(SETTING_COCOS_RES[VIEW_SET_CSB]);
		}
	}
	int nType = (int)LanguageType(Utils::getLanguage());
	int osType = 1;
#if ANDROID
	osType = 1;
#endif
#if CC_TARGET_OS_IPHONE
	osType = 2;
#endif

	ProtocolThread::GetInstance()->getVersionChangeLog(osType, nType, UILoadingIndicator::create(this));
}