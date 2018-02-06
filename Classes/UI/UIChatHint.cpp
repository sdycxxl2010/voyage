#include "UIChatHint.h"
#include "UISocial.h"
#include "ModeLayerManger.h"
UIChatHint::UIChatHint()
{
	i_unRead = nullptr;
}

UIChatHint::~UIChatHint()
{
	this->unscheduleUpdate();
}

UIChatHint* UIChatHint::createHint()
{
	auto ch = new UIChatHint;
	if (ch && ch->init())
	{
		ch->autorelease();
		return ch;
	}
	CC_SAFE_DELETE(ch);
	return nullptr;
}

bool UIChatHint::init()
{
	UISocial::getInstance()->showChat(1);
	auto b_chat = Button::create("cocosstudio/login_ui/common/chat_btn.png");
	b_chat->setName("button_chat");
	b_chat->setPosition(Vec2(b_chat->getBoundingBox().size.width/2, 360));
	b_chat->addTouchEventListener(CC_CALLBACK_2(UIChatHint::menuCall_func,this));
	this->addChild(b_chat);
	i_unRead = ImageView::create("cocosstudio/login_ui/social_720/unread.png");
	i_unRead->setPosition(Vec2(34, 50));
	i_unRead->setScale(0.6);
	i_unRead->setVisible(false);
	b_chat->addChild(i_unRead);
	this->scheduleUpdate();
	return true;
}

void UIChatHint::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		auto target = dynamic_cast<Widget*>(pSender);
		auto name = target->getName();
		if (isButton(button_chat))
		{
			auto instance = ModeLayerManger::getInstance();
			auto layer = instance->getModelLayer();
			if (layer != nullptr)
			{
				instance->showModelLayer(false);
			}
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			UISocial::getInstance()->openSocialLayer();
			UISocial::getInstance()->setChatCall();
			return;
		}
	}
}

void UIChatHint::update(float delta)
{
	bool unread = false;
	if (SINGLE_CHAT->m_public_unread_falg_zh)
	{
		unread = true;
	}
	if (SINGLE_CHAT->m_public_unread_falg)
	{
		unread = true;
	}
	if (SINGLE_CHAT->m_country_unread_falg)
	{
		unread = true;
	}
	if (SINGLE_CHAT->m_zone_unread_falg)
	{
		unread = true;
	}
	if (SINGLE_CHAT->m_guild_unread_falg)
	{
		unread = true;
	}

	std::map<std::string, bool>::iterator m1_Iter;
	for (m1_Iter = SINGLE_CHAT->m_private_unread_falg.begin(); m1_Iter != SINGLE_CHAT->m_private_unread_falg.end(); m1_Iter++)
	{
		if (m1_Iter->second)
		{
			unread = true;
		}
	}

	if (unread)
	{
		i_unRead->setVisible(true);
	}
	else
	{
		i_unRead->setVisible(false);
	}
}