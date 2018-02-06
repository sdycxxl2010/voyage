#include "UIWorldNotify.h"

UIWorldNotify::UIWorldNotify()
{
	m_nSysIndex = -1;
}

UIWorldNotify::~UIWorldNotify()
{
	SINGLE_HERO->m_vSystemInfo.clear();
}

bool UIWorldNotify::init()
{
	if (!UIBasicLayer::init())
	{
		return false;
	}	

	this->setName("SysInfo");
	openSysInfoView(0);
	return true;
}
void UIWorldNotify::onServerEvent(struct ProtobufCMessage* message, int msgType)
{

}

void UIWorldNotify::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
}

void UIWorldNotify::openSysInfoView(int time)
{
	if (m_nSysIndex != -1)
	{
		return;
	}
	bool is_find = false;
	SYSTEM_IFNO s_item;
	for (size_t i = 0; i < SINGLE_HERO->m_vSystemInfo.size(); i++)
	{
		auto item = SINGLE_HERO->m_vSystemInfo.at(i);
		if (!item.isWait)
		{
			s_item = item;
			is_find = true;
			m_nSysIndex = i;
			break;
		}
	}
	if (is_find)
	{
		openView(COMMOM_COCOS_RES[C_VIEW_SERVER_NOTIFICATION]);
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SERVER_NOTIFICATION]);
		view->setSwallowTouches(false);
		auto t_import = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "text_importantnotify"));
		auto i_bg = view->getChildByName<ImageView*>("image_i_bg");
		i_bg->stopAllActions();
		t_import->stopAllActions();

		std::string content;
		std::string old_value;
		std::string new_value;

		switch (s_item.sys_type)
		{
		case SHUTDOWN_AFTER_WAIT_TIME:
			t_import->enableOutline(Color4B(78, 0, 0, 255), 4);
			i_bg->loadTexture("cocosstudio/login_ui/sailing_720/importantnotify_bg.png");

			content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTERM_INFO"];
			old_value = "[time]";
			new_value = StringUtils::format("%d", SINGLE_HERO->m_waitTime);
			repalce_all_ditinct(content, old_value, new_value);
			break;
		case COUNTRY_WAR_OVER:
			t_import->enableOutline(Color4B(102, 62, 0, 255), 4);
			i_bg->loadTexture("cocosstudio/login_ui/sailing_720/importantnotify_bg_2.png");

			content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_MESSAGE_COUNTRY_WAR_END"];
			old_value = "[nationname1]";
			new_value = SINGLE_SHOP->getNationInfo()[s_item.nationId1];
			repalce_all_ditinct(content, old_value, new_value);
			old_value = "[nationname2]";
			new_value = SINGLE_SHOP->getNationInfo()[s_item.nationId2];
			repalce_all_ditinct(content, old_value, new_value);
			break;
		case COUNTRY_WAR_CITY_FALLEN:
			t_import->enableOutline(Color4B(102, 62, 0, 255), 4);
			i_bg->loadTexture("cocosstudio/login_ui/sailing_720/importantnotify_bg_2.png");

			content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_MESSAGE_CITY_FALLEN"];
			old_value = "[cityname]";
			new_value = SINGLE_SHOP->getCitiesInfo()[s_item.cityId].name;
			repalce_all_ditinct(content, old_value, new_value);
			break;
		case COUNTRY_WAR_START:
			t_import->enableOutline(Color4B(102, 62, 0, 255), 4);
			i_bg->loadTexture("cocosstudio/login_ui/sailing_720/importantnotify_bg_2.png");

			content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_MESSAGE_COUNTRY_WAR_START"];
			old_value = "[nationname1]";
			new_value = SINGLE_SHOP->getNationInfo()[s_item.nationId1];
			repalce_all_ditinct(content, old_value, new_value);
			old_value = "[nationname2]";
			new_value = SINGLE_SHOP->getNationInfo()[s_item.nationId2];
			repalce_all_ditinct(content, old_value, new_value);
			break;
		case COUNTRY_WAR_ING:
			t_import->enableOutline(Color4B(102, 62, 0, 255), 4);
			i_bg->loadTexture("cocosstudio/login_ui/sailing_720/importantnotify_bg_2.png");

			content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_MESSAGE_COUNTRY_WAR_ING"];
			old_value = "[nationname1]";
			new_value = SINGLE_SHOP->getNationInfo()[s_item.nationId1];
			repalce_all_ditinct(content, old_value, new_value);
			old_value = "[nationname2]";
			new_value = SINGLE_SHOP->getNationInfo()[s_item.nationId2];
			repalce_all_ditinct(content, old_value, new_value);
			break;
		default:
			break;
		}
		//log("nationId1 = %d nationId2 = %d", s_item.nationId1, s_item.nationId2);
		t_import->setString(content);
		i_bg->setOpacity(0);

		i_bg->runAction(FadeIn::create(0.8));
		i_bg->setSwallowTouches(false);
		t_import->setSwallowTouches(false);
		auto panel = view->getChildByName<Widget*>("panel_1");
		panel->setTouchEnabled(false);
		float weight = t_import->getContentSize().width;
		t_import->setPositionX(490);
		
		t_import->runAction(Sequence::create(DelayTime::create(0.8f), MoveBy::create(10.0, Vec2(-weight - panel->getContentSize().width, 0)), CallFunc::create([=]{t_import->setPositionX(490); }), CallFunc::create(CC_CALLBACK_0(UIWorldNotify::timeOff, this, 0)), nullptr));

		if (SINGLE_HERO->m_heroIsOnsea)
		{
			view->setCameraMask(4);
		}
		else
		{
			view->setCameraMask(1);
		}
		/*
		auto cameras = Director::getInstance()->getRunningScene()->getCameras();
		if (cameras.size() >= 2){
			auto flag = (unsigned short)cameras[cameras.size() - 1]->getCameraFlag();
			view->setCameraMask((unsigned short)cameras[cameras.size() - 1]->getCameraFlag(), true);
		}
		*/
	}
}
void UIWorldNotify::timeOff(float dt)
{
	SINGLE_HERO->m_vSystemInfo.erase(SINGLE_HERO->m_vSystemInfo.begin() + m_nSysIndex);
	m_nSysIndex = -1;
	if (SINGLE_HERO->m_vSystemInfo.empty())
	{
		this->removeFromParentAndCleanup(true);
	}
	else
	{
		openSysInfoView(0);
	}
}