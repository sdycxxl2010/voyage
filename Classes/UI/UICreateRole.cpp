#include "UICreateRole.h"
#include "Utils.h"

#include "UINoviceStoryLine.h"
#include "TVSceneLoader.h"
#include "TVLoadingScene.h"
#include "UITips.h"
#include "env.h"
UICreateRole::UICreateRole():
	m_nation(0),
	m_sex(0),
	m_icon(0)
{
	m_heroName = "";
	m_firstmate_hobby = 1;
	m_bFirstStep = false;
	m_bSecondStep = false;
}

UICreateRole::~UICreateRole()
{
	unregisterCallBack();
}

UICreateRole* UICreateRole::create()
{
	UICreateRole* res = new UICreateRole;
	if (res && res->init())
	{
		res->autorelease();
		return res;
	}
	CC_SAFE_DELETE(res);
	return nullptr;
}

bool UICreateRole::init()
{  
	bool pRet = false;
	do 
	{
		Utils::sendNewPlayerTrackingStep("go_into_create_role",0);
		registerCallBack();

		openView(CREATE_ROLE_RES[CREATE_ROLE_STEP_ONE]);
		m_sex = cocos2d::random() % 2 + 1;
		m_icon = cocos2d::random() % 4 + 1;
		if (m_sex == 2)
		{
			m_icon += 4;
		}

		auto p_charactor = m_pRoot->getChildByName<Widget*>("panel_customize_charactor");
		dynamic_cast<CheckBox*>(p_charactor->getChildByTag(m_sex)->getChildByTag(m_sex))->setVisible(true);
		dynamic_cast<CheckBox*>(p_charactor->getChildByTag(m_sex)->getChildByTag(m_sex))->setSelected(true);
		dynamic_cast<Button*>(p_charactor->getChildByTag(m_sex))->setTouchEnabled(false);

		auto i_avatar = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot, "image_head"));
		i_avatar->ignoreContentAdaptWithSize(false);
		i_avatar->loadTexture(getPlayerIconPath(m_icon));
		auto i_guild_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_charactor, "label_guild_name"));
		auto i_content = dynamic_cast<Text*>(Helper::seekWidgetByName(p_charactor, "label_content"));
		i_content->setPosition(Vec2(i_guild_name->getPositionX() + i_guild_name->getBoundingBox().size.width/2 + i_content->getBoundingBox().size.width/2 + 5,i_guild_name->getPositionY()));

		auto userName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_name"));
		userName->addEventListenerTextField(this, textfieldeventselector(UICreateRole::textfileEvent));
		userName->setTextVerticalAlignment(TextVAlignment::CENTER);
		userName->setPlaceHolderColor(Color3B(116,98,71));
		const std::string& defaultName = ProtocolThread::GetInstance()->getDefaultUserName();
		if (defaultName.size() > 0 && ENVIRONMENT != PRODUCTION)
		{
			userName->setString(defaultName);
		}
		userName->setVisible(false);
		ui::EditBox* editbox_name = nullptr;
		if (!editbox_name)
		{
			editbox_name = ui::EditBox::create(Size(userName->getContentSize()), "input.png");
			editbox_name->setTag(1);
			userName->getParent()->addChild(editbox_name, 9999);
			editbox_name->setPosition(userName->getPosition());
			editbox_name->setPlaceholderFontColor(Color3B(116, 98, 71));
			editbox_name->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
			editbox_name->setFontColor(Color3B(46, 29, 14));
			editbox_name->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
			editbox_name->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		}
		pRet = true;
	} while (0);
	 
	return pRet;
}

void UICreateRole::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	if(msgType == PROTO_TYPE_InsertUserNameResult)
	{
		InsertUserNameResult *result = (InsertUserNameResult*)message;
		if (result->failed == 0)
		{
			//closeView();
			ProtocolThread::GetInstance()->createCharacter(m_firstmate_hobby, m_nation, UILoadingIndicator::create(this));
			
		}else
		{
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_USER_NAME_VAID_USED"].c_str());
			this->addChild(tip,1);
		}
	}else if (msgType == PROTO_TYPE_CreateCharacterFailed)
	{
		CreateCharacterFailed*failed = (CreateCharacterFailed*)message;
		log("create character failed, reason:%d",failed->reason);
	}else if(msgType == PROTO_TYPE_EmailLoginResult)
	{
		EmailLoginResult*succ = (EmailLoginResult*)message;
		if (succ->failed == 0)
		{
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			if(succ->maintaskstatus == 10)
			{
				CHANGETO(SCENE_TAG::MAIN_TAG);
			}else
			{
//				ScriptEngineManager::getInstance()->getScriptEngine()->executeScriptFile("src/main.lua");
				Scene*currentScene = Director::getInstance()->getRunningScene();
				auto guideDialogLayer = UINoviceStoryLine::GetInstance();
				if (!guideDialogLayer->getParent())
				{
					currentScene->addChild(guideDialogLayer, 10001);
				}
				guideDialogLayer->onGuideTaskMessage(1, 0);

			}
		}else
		{
			Director::getInstance()->replaceScene(UICreateRole::createScene());
		}
	}else if(msgType == -1)
	{
		UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_CONNECT_FAIL"].c_str(),2);
		this->addChild(tip,1);
		tip->setNegativeCallback(CC_CALLBACK_0(UICreateRole::exitGame,this));
		tip->setPositiveCallback(CC_CALLBACK_0(UICreateRole::reConnect,this));		
	}
}

void UICreateRole::reConnect()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);	
	auto scene = TVLoadingScene::createLoadingScene();
	Director::getInstance()->replaceScene(scene);
	ProtocolThread::GetInstance()->reconnectServer();
}

void UICreateRole::exitGame()
{
#if WIN32
	Director::getInstance()->end();
#elif ANDROID
	exit(0);
#endif
}

Scene* UICreateRole::createScene()
{
	auto scene = Scene::create();
	CC_RETURN_IF(scene);
	auto res = UICreateRole::create();
	scene->addChild(res);
	return scene;
}

// ---------button event ----------------------
void UICreateRole::menuCall_func(Ref *pSender,Widget::TouchEventType touchType)
{
	if (touchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* widget = dynamic_cast<Widget*>(pSender);
	std::string name = widget->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_setoff))
	{
		if (!m_bSecondStep)
		{
			m_bSecondStep = true;
			Utils::sendNewPlayerTrackingStep("go_into_firstmate", m_nation);
		}
		initFirstmateChoose();
		return;
	}

	if(isButton(button_left))
	{
		Utils::sendNewPlayerTrackingStep("choose_icon", m_nation);
		changePlayer(false);
		return;
	}

	if (isButton(button_right))
	{
		Utils::sendNewPlayerTrackingStep("choose_icon", m_nation);
		changePlayer(true);
		return;
	}

	if(isButton(button_male) || isButton(button_female))
	{
		auto p_charactor = m_pRoot->getChildByName<Widget*>("panel_customize_charactor");
 		dynamic_cast<CheckBox*>(p_charactor->getChildByTag(m_sex)->getChildByTag(m_sex))->setVisible(false);
		dynamic_cast<Button*>(p_charactor->getChildByTag(m_sex))->setTouchEnabled(true);
		m_sex = widget->getTag();
		dynamic_cast<CheckBox*>(p_charactor->getChildByTag(m_sex)->getChildByTag(m_sex))->setVisible(true);
		dynamic_cast<CheckBox*>(p_charactor->getChildByTag(m_sex)->getChildByTag(m_sex))->setSelected(true);
		dynamic_cast<Button*>(p_charactor->getChildByTag(m_sex))->setTouchEnabled(false);
		changePlayer(true);
		Utils::sendNewPlayerTrackingStep("choose_sex", m_nation);
		return;
	}

	if (isButton(button_next))
	{
		auto view = getViewRoot(CREATE_ROLE_RES[CREATE_ROLE_STEP_ONE]);
		auto userName = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_name"));
		ui::EditBox* editnox_name = dynamic_cast<ui::EditBox*>(userName->getParent()->getChildByTag(1));
		m_heroName = editnox_name->getText();
		auto userNameLength = strlen(m_heroName.data());
		/*现在起名限制中文2-8个 英文5-24*/
		auto minLength = 5;
		auto maxLength = 24;
		
		if (userNameLength == 0)
		{//命名无效
			Utils::sendNewPlayerTrackingStep("name_invalid", m_nation);
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_USER_NAME_VAID_INVALID"].c_str());
			this->addChild(tip, 1);
		}
		else if (userNameLength > 0 && userNameLength < minLength)
		{//命名超短
			Utils::sendNewPlayerTrackingStep("name_too_short", m_nation);
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_USER_NAME_VAID_SHORT"].c_str());
			this->addChild(tip, 1);
		}
		else if(userNameLength >= minLength && userNameLength <= maxLength)
		{
			//ProtocolThread::GetInstance()->insertUserName(userName->getString().c_str(), m_sex, m_icon, LoadingLayer::create(this));
			/*打开下一步*/
			//统计
			if (!m_bFirstStep)
			{
				m_bFirstStep = true;
				Utils::sendNewPlayerTrackingStep("name_good_go_next", m_nation);
			}

			openView(CREATE_ROLE_RES[CREATE_ROLE_STEP_TWO]);
			initSelectNation();
			view->setPosition(ENDPOS);
		}
		else
		{//命名超长
			Utils::sendNewPlayerTrackingStep("name_too_long", m_nation);
			UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_USER_NAME_VAID_LONG"].c_str());
			this->addChild(tip, 1);
		}
		return;
	}
	/*返回上一步*/
	if (isButton(button_return))
	{
		Utils::sendNewPlayerTrackingStep("back_choose_name_and_sex", m_nation);
		auto viewCreate1 = getViewRoot(CREATE_ROLE_RES[CREATE_ROLE_STEP_ONE]);
		viewCreate1->setPosition(STARTPOS);
		closeView(CREATE_ROLE_RES[CREATE_ROLE_STEP_TWO]);
		return;
	}
}

void UICreateRole::nationBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto widget = dynamic_cast<Widget*>(pSender);
		int tag = widget->getTag();
		if (tag > 5)
		{
			openSuccessOrFailDialog("TIP_CREATE_HOMETOWN");
			return;
		}
		Utils::sendNewPlayerTrackingStep(StringUtils::format("choose_nation_%d", tag), m_nation);
		auto p_nation = m_pRoot->getChildByName<Widget*>("panel_choose_hometown");
		auto l_nation = p_nation->getChildByName<ListView*>("listview_30");
		dynamic_cast<CheckBox*>(l_nation->getItem((m_nation-1)*2)->getChildByTag(m_nation)->getChildByTag(m_nation))->setVisible(false);
		dynamic_cast<Widget*>(l_nation->getItem((m_nation-1)*2))->setTouchEnabled(true);
		m_nation = widget->getTag();
		dynamic_cast<CheckBox*>(l_nation->getItem((m_nation-1)*2)->getChildByTag(m_nation)->getChildByTag(m_nation))->setVisible(true);
		dynamic_cast<CheckBox*>(l_nation->getItem((m_nation-1)*2)->getChildByTag(m_nation)->getChildByTag(m_nation))->setSelected(true);
		dynamic_cast<Widget*>(l_nation->getItem((m_nation-1)*2))->setTouchEnabled(false);
	}
}

void UICreateRole::nationLeftAndRigetBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto widget = dynamic_cast<Widget*>(pSender);
		auto name = widget->getName();
		auto p_nation = m_pRoot->getChildByName<Widget*>("panel_choose_hometown");
		auto l_nation = p_nation->getChildByName<ListView*>("listview_30");
		auto b_left = p_nation->getChildByName<Button*>("button_left");
		auto b_right = p_nation->getChildByName<Button*>("button_right");
		if (isButton(button_left))
		{
			auto item = l_nation->getItem(0);
			auto item_1 = l_nation->getItem(1);
			auto l_container = l_nation->getInnerContainer();
			float offsetX = item_1->getPositionX() - item->getPositionX();
			l_container->setPositionX(l_container->getPositionX() + offsetX * 2 + 10);
			b_left->setVisible(false);
			b_right->setVisible(true);
		}else
		{
			auto item = l_nation->getItem(0);
			auto item_1 = l_nation->getItem(1);
			auto l_container = l_nation->getInnerContainer();
			float offsetX = item_1->getPositionX() - item->getPositionX();
			l_container->setPositionX(l_container->getPositionX() - (offsetX * 2+10));
			b_left->setVisible(true);
			b_right->setVisible(false);
		}
	}
}

void UICreateRole::changePlayer(bool isAdd)
{
	if (isAdd)
	{
		m_icon++;
	}else
	{
		m_icon--;
	}

	int min = 1;
	int max = 4;
	if (m_sex == 2)
	{
		min = 5;
		max = 8;
	}

	if (m_icon < min)
	{
		m_icon = max;
	}

	if (m_icon > max)
	{
		m_icon = min;
	}

	auto i_avatar = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"image_head"));
	i_avatar->ignoreContentAdaptWithSize(false);
	i_avatar->loadTexture(getPlayerIconPath(m_icon));
}

void UICreateRole::initSelectNation()
{
	auto p_nation = m_pRoot->getChildByName<Widget*>("panel_choose_hometown");
	auto l_nation = p_nation->getChildByName<ListView*>("listview_30");
	auto b_left = p_nation->getChildByName<Button*>("button_left");
	auto b_right = p_nation->getChildByName<Button*>("button_right");

	b_left->setVisible(false);
	b_left->addTouchEventListener(CC_CALLBACK_2(UICreateRole::nationLeftAndRigetBtnEvent,this));
	b_right->addTouchEventListener(CC_CALLBACK_2(UICreateRole::nationLeftAndRigetBtnEvent,this));
	m_nation = cocos2d::random() % 5 + 1;
	p_nation->getChildByName<CheckBox*>("checkbox_1")->setVisible(false);
	dynamic_cast<CheckBox*>(l_nation->getItem((m_nation - 1) * 2)->getChildByTag(m_nation)->getChildByTag(m_nation))->setVisible(true);
	dynamic_cast<CheckBox*>(l_nation->getItem((m_nation - 1) * 2)->getChildByTag(m_nation)->getChildByTag(m_nation))->setSelected(true);
	dynamic_cast<Widget*>(l_nation->getItem((m_nation - 1) * 2))->setTouchEnabled(false);
//chengyuan ++
	l_nation->setDirection(SCROLLVIEW_DIR_NONE);
	for (int i = 0; i < 7; i++)
	{
		auto p_item = l_nation->getItem(2 * i);
		p_item->setTag(i + 1);
		p_item->addTouchEventListener(CC_CALLBACK_2(UICreateRole::nationBtnEvent,this));
		auto children = p_item->getChildren();
		for (auto child : children)
		{
			auto name  = dynamic_cast<Button*> (child)->getName();
			if (name.find("button_flag_")==0)
			{
				child->setTag(i + 1);
				dynamic_cast<Button*> (child)->addTouchEventListener(CC_CALLBACK_2(UICreateRole::nationBtnEvent,this));
			}
			dynamic_cast<Button*>(child)->loadTextureNormal(getCountryIconPath(100+i));
		}
	}
	dynamic_cast<Text*>(Helper::seekWidgetByName(p_nation, "label_lisbon"))->setString(SINGLE_SHOP->getNationInfo()[1]);
	dynamic_cast<Text*>(Helper::seekWidgetByName(p_nation, "label_sevilla"))->setString(SINGLE_SHOP->getNationInfo()[2]);
	dynamic_cast<Text*>(Helper::seekWidgetByName(p_nation, "label_london"))->setString(SINGLE_SHOP->getNationInfo()[3]);
	dynamic_cast<Text*>(Helper::seekWidgetByName(p_nation, "label_amsterdam"))->setString(SINGLE_SHOP->getNationInfo()[4]);
	dynamic_cast<Text*>(Helper::seekWidgetByName(p_nation, "label_genoa"))->setString(SINGLE_SHOP->getNationInfo()[5]);
	dynamic_cast<Text*>(Helper::seekWidgetByName(p_nation, "label_venice"))->setString(SINGLE_SHOP->getNationInfo()[6]);
	dynamic_cast<Text*>(Helper::seekWidgetByName(p_nation, "label_swedish"))->setString(SINGLE_SHOP->getNationInfo()[7]);
}
void UICreateRole::textfileEvent(Ref* target, TextFiledEventType type)
{
	auto text = dynamic_cast<TextField*>(target);
	auto str = text->getString();
	if (str.length() == 0)
	{
		text->setOpacity(255 / 2);
	}
	else
	{
		text->setOpacity(255);
	}
}
void UICreateRole::initFirstmateChoose()
{
	openView(CREATE_ROLE_RES[CREATE_ROLE_STEP_THREE]);
	auto view = getViewRoot(CREATE_ROLE_RES[CREATE_ROLE_STEP_THREE]);
	auto i_done_firstmate = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_done_firstmate"));
	auto i_previous_firstmate = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_previous_firstmate"));
	auto i_panel_battle = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_battle"));
	auto i_panel_business = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_business"));
	auto checkbox_battle = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(view, "checkbox_battle"));
	checkbox_battle->setSelected(true);

	auto label_battle = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_battle"));
	auto label_business = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_business"));
	label_battle->setString(SINGLE_SHOP->getTipsInfo()["TIP_CREATROLE_FIRSTMATE_BATTLE_CONTENT"]);
	label_business->setString(SINGLE_SHOP->getTipsInfo()["TIP_CREATROLE_FIRSTMATE_BUSSINESS_CONTENT"]);
	i_done_firstmate->addTouchEventListener(CC_CALLBACK_2(UICreateRole::myFirstmateChooseBtn,this));
	i_previous_firstmate->addTouchEventListener(CC_CALLBACK_2(UICreateRole::myFirstmateChooseBtn,this));
	i_panel_battle->addTouchEventListener(CC_CALLBACK_2(UICreateRole::myFirstmateChooseBtn,this));
	i_panel_business->addTouchEventListener(CC_CALLBACK_2(UICreateRole::myFirstmateChooseBtn,this));
	if (m_sex == ISMAN)//男性，其副官是女的
	{
		m_firstmate_hobby = WOMAN_BATTEL;
	}
	else
	{
		m_firstmate_hobby = MAN_BATTEL;
	}
}
void UICreateRole::myFirstmateChooseBtn(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto widget = dynamic_cast<Widget*>(pSender);
	int tag = widget->getTag();
	auto name = widget->getName();

	auto view = getViewRoot(CREATE_ROLE_RES[CREATE_ROLE_STEP_THREE]);
	auto i_panel_battle = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_battle"));
	auto i_panel_business = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_business"));
	auto checkbox_battle = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(view, "checkbox_battle"));
	auto checkbox_buss = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(view, "checkbox_buss"));
	//选择偏好战斗
	if (isButton(panel_battle))
	{
		checkbox_battle->setSelected(true);
		checkbox_buss->setSelected(false);
		if (m_sex == ISMAN)
		{
			m_firstmate_hobby = WOMAN_BATTEL;
		}
		else
		{
			m_firstmate_hobby = MAN_BATTEL;
		}
		Utils::sendNewPlayerTrackingStep("choose_battle", m_nation);
		return;
	}
	//选择偏好经济
	if (isButton(panel_business))
	{
		checkbox_battle->setSelected(false);
		checkbox_buss->setSelected(true);
		if (m_sex == ISMAN)
		{
			m_firstmate_hobby = WOMAN_BUSSNIESS;
		}
		else
		{
			m_firstmate_hobby = MAN_BUSSNIESS;
		}
		Utils::sendNewPlayerTrackingStep("choose_business", m_nation);
		return;
	}
	//返回上一步，重新选择国家
	if (isButton(button_previous_firstmate))
	{
		Utils::sendNewPlayerTrackingStep("go_into_game", m_nation);
		closeView();
		return;
	}
	//设置完成
	if (isButton(button_done_firstmate))
	{
		Utils::sendNewPlayerTrackingStep("back_choose_nation", m_nation);
		ProtocolThread::GetInstance()->insertUserName(m_heroName.c_str(), m_sex, m_icon, UILoadingIndicator::create(this));
		return;
	}
}
