#include "UIRegister.h"
#include "UILogin.h"
#include "UITips.h"
#include "UIInform.h"
#include "UICreateRole.h"
#include "TVLoadingScene.h"

#ifdef ANDROID
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif


UIRegister::UIRegister()
{
	m_inputParent = nullptr;
}

UIRegister::~UIRegister()
{
	unscheduleUpdate();
	unregisterCallBack();
}

UIRegister* UIRegister::create()
{
	UIRegister* res = new UIRegister;
	if (res && res->init())
	{
		res->autorelease();
		return res;
	}
	CC_SAFE_DELETE(res);
	return nullptr;
}
void getRandomString(char*buffer, int len);
//data init start in here
bool UIRegister::init()
{
	bool pRet = false;
	do 
	{
		registerCallBack();

		openView("cocosstudio/voyageUI_reg.csb");
		auto input_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_player_name"));
		auto input_pwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_password"));
		input_pwd->setTouchEnabled(true);

		auto t_pwd = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, "label_existing"));
		auto b_touch = Button::create();
		b_touch->setContentSize(t_pwd->getContentSize()*3);
		b_touch->setName("label_existing");
		b_touch->setTouchEnabled(true);
		b_touch->ignoreContentAdaptWithSize(false);
		b_touch->addTouchEventListener(CC_CALLBACK_2(UIRegister::menuCall_func,this));
		t_pwd->addChild(b_touch);
		b_touch->setPosition(Vec2(t_pwd->getBoundingBox().size.width/2,t_pwd->getBoundingBox().size.height/2));
		input_name->setTextVerticalAlignment(TextVAlignment::CENTER);
		input_name->setPlaceHolderColor(Color3B(116,98,71));
		input_pwd->setTextVerticalAlignment(TextVAlignment::CENTER);
		input_pwd->setPlaceHolderColor(Color3B(116,98,71));
		input_pwd->addEventListener(CC_CALLBACK_2(UIRegister::textEvent,this));
		input_name->addEventListener(CC_CALLBACK_2(UIRegister::textEvent,this));
		m_inputParent = (Helper::seekWidgetByName(m_pRoot, "image_bg_dialog"));
		m_inputOldPostion = m_inputParent->getPosition();
		input_pwd->setPasswordEnabled(false);
		input_pwd->setString("");
		
		/*
		char passwd[13];
		passwd[12] = 0;
		int pass_len = 12;
		getRandomString(passwd, pass_len);
		input_pwd->setString(passwd);
		*/
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
				input_name->setString(defaultName);
				jniInfo.env->ReleaseStringUTFChars(res, smsList);
			}else{
				input_name->setPlaceHolder("please input you Email in here !");
			}			
		}
		else
		{
			input_name->setPlaceHolder("please input you Email in here !");
		}
#else
		//input_name->setString("xijing@infolife.mobi");
#endif
		scheduleUpdate();
		pRet = true;
	} while (0);
	return pRet;
}

Scene* UIRegister::createScene()
{
	auto scene = Scene::create();
	CC_RETURN_IF(scene);
	auto res = UIRegister::create();
	scene->addChild(res);
	return scene;
}

// ---------button event ----------------------
void UIRegister::menuCall_func(Ref *pSender,Widget::TouchEventType touchType)
{
	if (touchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	
	Widget* widget = dynamic_cast<Widget*>(pSender);
	std::string name = widget->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//set music state open
	UserDefault::getInstance()->setIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO);
	UserDefault::getInstance()->setIntegerForKey(SOUND_EFFECT_KEY, OPEN_AUDIO);
	//UserDefault::getInstance()->setIntegerForKey(SERVER_LOCAL_INDEX, 0);
	UserDefault::getInstance()->flush();
	if (isButton(label_existing))
	{
		Director::getInstance()->replaceScene(UILogin::createScene());
	}else if (isButton(button_register))
	{
		
		auto userName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_player_name"));
		auto input_pwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_password"));
		auto email = userName->getString();
		/*账户格式不对*/
		if (email == ""||email.find("@") == std::string::npos || email.find("@") == 0 )
		{
			//openSuccessOrFailDialog("TIP_EMAIL_VAID");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_EAMIL_FAIL");
			return;
		}	
		auto password = input_pwd->getString();
		/*密码长度不对*/
		if (strlen(password.data()) < 8 || strlen(password.data()) > 20)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_PASSWORD_FAIL");
			return;
		}
		ProtocolThread::GetInstance()->registerUser(userName->getString().c_str(),input_pwd->getString().c_str(),UILoadingIndicator::create(this));
	}

}

void UIRegister::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
	if(msgType == PROTO_TYPE_RegisterUserResult)
	{
		RegisterUserResult* result = (RegisterUserResult*)message;
		if (result && result->userid != -1)
		{
			Director::getInstance()->replaceScene(UICreateRole::createScene());
		}else
		{
			if (result->reason == 2)
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_REGISTED"].c_str());
				this->addChild(tip,1);
			}else if (result->reason == 1)
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_VAID"].c_str());
				this->addChild(tip,1);
			}else if (result->reason == 5)
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_LOGIN_UPDATE_CLIENT"].c_str());
				tip->setPositiveCallback(CC_CALLBACK_0(UIBasicLayer::gotoStorePage, this));
				this->addChild(tip, 1);
			}else
			{
				UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_REGISTED_FAIL"].c_str());
				this->addChild(tip,1);
			}
		}
	}else if(msgType == -1)
	{
		//connect to server failed		
		UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_CONNECT_FAIL"].c_str(),2);
		this->addChild(tip,1);
		tip->setNegativeCallback(CC_CALLBACK_0(UIRegister::exitGame,this));
		tip->setPositiveCallback(CC_CALLBACK_0(UIRegister::reConnect,this));
	}
}

void UIRegister::reConnect()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);	
	auto scene = TVLoadingScene::createLoadingScene();
	Director::getInstance()->replaceScene(scene);
	ProtocolThread::GetInstance()->reconnectServer();
}

void UIRegister::exitGame()
{
	Director::getInstance()->end();
}

void UIRegister::update(float dt)
{
	auto view = getViewRoot("cocosstudio/voyageUI_reg.csb");

	if (view)
	{
		auto input_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_player_name"));
		auto name = input_name->getString();
		TrimAll(name);
		input_name->setString(name);
		if (name.size() > 0)
		{
			input_name->setOpacity(255);
		}
		else
		{
			input_name->setOpacity(127);
		}

		if (illegal_character_check(name))
		{
			input_name->setString("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}

		auto input_password = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_player_password"));
		auto password = input_password->getString();
		TrimAll(password);
		input_password->setString(password);
		if (password.size() > 0)
		{
			input_password->setOpacity(255);
		}
		else
		{
			input_password->setOpacity(127);
		}
	}
}

void UIRegister::textEvent(Ref*obj, TextField::EventType type)
{
	log("RegisterUserLayer::textEvent:%d",type);
	if(type == TextField::EventType::ATTACH_WITH_IME){
		//Vec2 newPos = m_inputOldPostion;
		m_inputParent->setPosition(Vec2(m_inputOldPostion.x,720-m_inputParent->getContentSize().height/2));
	}else if(type == TextField::EventType::DETACH_WITH_IME){
		m_inputParent->setPosition(m_inputOldPostion);
		auto target = dynamic_cast<TextField*>(obj);
		if (target->getName() == "textfield_player_password")
		{
			auto password = target->getString();
			if (strlen(password.data()) < 8 || strlen(password.data()) > 20)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_PASSWORD_FAIL");
			}
		}
		else
		{
			auto email = target->getString();
			if (email == "" || email.find("@") == std::string::npos || email.find("@") == 0)
			{
				target->runAction(Sequence::create(TintTo::create(0.2f, Color3B(255, 0, 0)),DelayTime::create(0.5f),TintTo::create(0.2f, Color3B(46, 29, 14)),nullptr));
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CREATE_ACCOUNT_EAMIL_FAIL");
			}
		}
	}
}
