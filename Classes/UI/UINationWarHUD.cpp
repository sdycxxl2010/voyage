#include "UINationWarHUD.h"
#include "EnumList.h"
#include "UICommon.h"
#include "UIInform.h"

#include "UISocial.h"
#include "WarManage.h"
#include "EffectManage.h"
#include "TVSceneLoader.h"
#include "UISailManage.h"
#include "UINationWarMap.h"
#include "TimeUtil.h"

UINationWarHUD::UINationWarHUD()
{
	m_eUIType = UI_COUNTRY_WAR;
	m_nCountryWarTime = 0;
	m_bIsFirstChat = true;
	m_pMainChat = nullptr;
	m_nShowChatTime = 0;
	m_nChatItem = 0;
	m_pResult = nullptr;
	m_pCityInfo = nullptr;
	m_pLastShipFleetInfo = nullptr;
	m_pShipFleetInfo = nullptr;
	m_eConfirm = CONFIRM_INDEX_NONE;
	m_nUserCid = 0;
	m_nLoadingNum = 0;
}

UINationWarHUD::~UINationWarHUD()
{
	this->unschedule(schedule_selector(UINationWarHUD::updateBySecond));
}

UINationWarHUD* UINationWarHUD::createWarUI(UIBasicLayer* parent, StartAttackCityResult  *pResult)
{
	UINationWarHUD* wu = new UINationWarHUD;
	if (wu && wu->init(pResult, parent))
	{
		wu->autorelease();
		return wu;
	}
	CC_SAFE_DELETE(wu);
	return nullptr;
}

bool UINationWarHUD::init(StartAttackCityResult  *pResult, UIBasicLayer* parent)
{
	bool pRet = false;
	do
	{
		m_pParent = parent;
		m_pResult = pResult;
		setCityAndShipFleetInfo(m_pResult->city_status, m_pResult->status);
		m_nCountryWarTime = 180;
		openView(MAPUI_COCOS_RES[COUNTRY_WAR_UI_CSB]);
		auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_UI_CSB]);
		view->setTouchEnabled(false);
		auto t_cityname = view->getChildByName<Text*>("text_cityname");
		t_cityname->setString(SINGLE_SHOP->getCitiesInfo().at(SINGLE_HERO->m_nAttackCityId).name);
		auto t_time = view->getChildByName<Text*>("text_retreat_time");
		t_time->setString(StringUtils::format("%d:%02d", m_nCountryWarTime / 60, m_nCountryWarTime % 60));
		auto t_duable = view->getChildByName<Text*>("Text_1");
		auto l_duable = view->getChildByName<LoadingBar*>("loadingbar_duable");
		t_duable->setString(StringUtils::format("%d/%d", m_pCityInfo->start_hp, m_pCityInfo->max_hp));
		l_duable->setPercent(100.0 * m_pCityInfo->start_hp / m_pCityInfo->max_hp);
		auto warMap = dynamic_cast<WarManage*>(m_pParent)->getWarMapLayer();
		warMap->updateCityButton(m_pCityInfo->start_hp, m_pCityInfo->max_hp);
		auto i_light = view->getChildByName<ImageView*>("image_progress_duable_light");
		i_light->setVisible(false);

		auto p_fleet = view->getChildByName<Widget*>("panel_fleet");
		auto p_ship = p_fleet->getChildByName<Widget*>("panel_ship_1");
		int n_gird = m_pShipFleetInfo->n_ships;
		p_fleet->setPositionX(p_fleet->getPositionX() + (5 - n_gird) * 100);

		for (size_t i = 0; i < n_gird; i++)
		{
			auto shipInfo = m_pShipFleetInfo->ships[i];
			Widget *p_item;
			if (i < 1)
			{
				p_item = p_ship;
			}
			else
			{
				p_item = p_ship->clone();
				p_fleet->addChild(p_item);
				p_item->setName(StringUtils::format("panel_ship_%d", i + 1));
			}

			auto i_bg = p_item->getChildByName<ImageView*>("image_material_bg");
			auto i_ship = i_bg->getChildByName<ImageView*>("image_ship");
			auto i_position = i_bg->getChildByName<ImageView*>("image_num_2");
			i_ship->loadTexture(getShipIconPath(shipInfo->ship_id));
			i_position->loadTexture(getPositionIconPath(i + 1));
			auto t_durable = p_item->getChildByName<Text*>("label_ship_durable_num_1");
			auto l_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(p_item, "progressbar_durable"));
			t_durable->setString(StringUtils::format("%d/%d", shipInfo->hp, shipInfo->max_hp));
			l_durable->setPercent(100.0 * shipInfo->hp / shipInfo->max_hp);
			setBgButtonFormIdAndType(i_bg, shipInfo->ship_id, ITEM_TYPE_SHIP);
			p_item->setPositionX(i * 200);
		}
		flushMainChat();

		m_nLoadingNum = cocos2d::random(1, 20);
		openView(COMMOM_COCOS_RES[C_VIEW_NATION_WAR_LOADING]);
		auto view1 = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NATION_WAR_LOADING]);
		auto i_loading_1 = view1->getChildByName<ImageView*>("image_loading_1");
		auto i_loading_2 = view1->getChildByName<ImageView*>("image_loading_2");
		auto LoadBgSmalllrepeat = RepeatForever::create(RotateBy::create(1, 180));
		i_loading_1->runAction(LoadBgSmalllrepeat);
		auto  LoadBglrepeat = RepeatForever::create(RotateBy::create(1, -180));
		i_loading_2->runAction(LoadBglrepeat);
		auto t_content = view1->getChildByName<Text*>("label_content");
		t_content->runAction(RepeatForever::create(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINationWarHUD::changeLabel, this)), FadeIn::create(0.8f), DelayTime::create(3.2f), FadeOut::create(0.8f), nullptr)));
		
		this->schedule(schedule_selector(UINationWarHUD::updateBySecond), 1);
		pRet = true;
	} while (0);

	return pRet;
}

void UINationWarHUD::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* widget = dynamic_cast<Widget*>(pSender);
		std::string name = widget->getName();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

		if (isButton(button_retreat_info))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_WAR_RANKING_TITLE", "TIP_PALACE_WAR_RANKING_CONTENET");
			return;
		}
		//退出按钮
		if (isButton(button_retreat))
		{
			if (m_nCountryWarTime < 1)
			{
				m_eConfirm = CONFIRM_INDEX_QUIT;
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYesOrNO("TIP_COUNTRY_WAR_RETREAT_TITLE", "TIP_COUNTRY_WAR_RETREAT_CONTENT");		
			}
			else
			{
				m_eConfirm = CONFIRM_INDEX_NOT_QUIT;
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYesOrNO("TIP_COUNTRY_WAR_NOT_RETREAT_TITLE", "TIP_COUNTRY_WAR_NOT_RETREAT_CONTENT");
			}
			return;
		}

		if (isButton(button_confirm_yes))
		{
			switch (m_eConfirm)
			{
			case UINationWarHUD::CONFIRM_INDEX_NONE:
				break;
			case UINationWarHUD::CONFIRM_INDEX_DIED:
				button_callBack();
				break;
			case UINationWarHUD::CONFIRM_INDEX_CITY_BREACHED:
			case UINationWarHUD::CONFIRM_INDEX_WAR_END:
			case UINationWarHUD::CONFIRM_INDEX_QUIT:
			case UINationWarHUD::CONFIRM_INDEX_NOT_QUIT:
			case UINationWarHUD::CONFIRM_INDEX_OVERTIME:
				ProtocolThread::GetInstance()->endAttackCity(SINGLE_HERO->m_nAttackCityId, UILoadingIndicator::create(this));
				break;
			case UINationWarHUD::CONFIRM_INDEX_REMOVEFRIEND:
				m_eConfirm = CONFIRM_INDEX_NONE;
				ProtocolThread::GetInstance()->friendsOperation(m_nUserCid, 1, UILoadingIndicator::create(this));
				break;
			case UINationWarHUD::CONFIRM_INDEX_FRIEND_BLOCK:
			case UINationWarHUD::CONFIRM_INDEX_STRANGER_BLOCK:
				m_eConfirm = CONFIRM_INDEX_NONE;
				ProtocolThread::GetInstance()->friendsOperation(m_nUserCid, 2, UILoadingIndicator::create(this));
				break;
			case UINationWarHUD::CONFIRM_INDEX_SOCIAL_INVITE_GUILD:
				m_eConfirm = CONFIRM_INDEX_NONE;
				ProtocolThread::GetInstance()->inviteUserToGuild(m_nUserCid, UILoadingIndicator::create(this));
				break;
			case UINationWarHUD::CONFIRM_INDEX_STRANGER_ADDFRIEND:
				m_eConfirm = CONFIRM_INDEX_NONE;
				ProtocolThread::GetInstance()->friendsOperation(m_nUserCid, 0, UILoadingIndicator::create(this));
				break;
			case UINationWarHUD::CONFIRM_INDEX_BLOCK_REMOVE:
				m_eConfirm = CONFIRM_INDEX_NONE;
				ProtocolThread::GetInstance()->friendsOperation(m_nUserCid, 5, UILoadingIndicator::create(this));
				break;
			default:
				break;
			}
			return;
		}

		if (isButton(button_error_yes))
		{
			switch (m_eConfirm)
			{
			case UINationWarHUD::CONFIRM_INDEX_NONE:
				break;
			case UINationWarHUD::CONFIRM_INDEX_DIED:
				button_callBack();
				break;
			case UINationWarHUD::CONFIRM_INDEX_CITY_BREACHED:
			case UINationWarHUD::CONFIRM_INDEX_WAR_END:
			case UINationWarHUD::CONFIRM_INDEX_QUIT:
			case UINationWarHUD::CONFIRM_INDEX_NOT_QUIT:
			case UINationWarHUD::CONFIRM_INDEX_OVERTIME:
				ProtocolThread::GetInstance()->endAttackCity(SINGLE_HERO->m_nAttackCityId, UILoadingIndicator::create(this));
				break;
			default:
				break;
			}
			return;
		}

		if (isButton(button_chat))
		{
			UISocial::getInstance()->openSocialLayer();
			UISocial::getInstance()->setChatCall();
			UISocial::getInstance()->showChat();
			return;
		}

		if (isButton(button_friend_delete)) //删除好友
		{
			m_nUserCid = widget->getParent()->getTag();
			m_eConfirm = CONFIRM_INDEX_REMOVEFRIEND;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_DELETE_FRIEND_TITLE", "TIP_SOCIAL_DELETE_FRIEND");
			return;
		}

		if (isButton(button_friend_block)) //拉黑
		{
			m_nUserCid = widget->getParent()->getTag();
			m_eConfirm = CONFIRM_INDEX_FRIEND_BLOCK;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_FRIEND");
			return;
		}

		if (isButton(button_stranger_block)) //拉黑陌生人
		{
			m_nUserCid = widget->getParent()->getTag();
			m_eConfirm = CONFIRM_INDEX_STRANGER_BLOCK;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_STRANGER");
			return;
		}

		if (isButton(button_invite)) //公会邀请
		{
			m_nUserCid = widget->getTag();
			m_eConfirm = CONFIRM_INDEX_SOCIAL_INVITE_GUILD;
			if (SINGLE_HERO->m_InvitedToGuildNation == SINGLE_HERO->m_iNation)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_INVITE_GUILD_TITLE", "TIP_GUILD_INVINT_GUILD");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITED_FAILED_NOT_SAME_COUNTRY_CONTENT");
			}
			return;
		}

		if (isButton(button_add_friend)) //加为好友
		{
			m_nUserCid = widget->getParent()->getTag();
			m_eConfirm = CONFIRM_INDEX_STRANGER_ADDFRIEND;
			if (widget->getTag() == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_AGAIN_ADD_FRIENDS");
			}
			else if (widget->getTag() == 1)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_ALREADY_GOOD_FRIEND");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_ADD_FRIEND_TITLE", "TIP_SOCIAL_ADD_FRIEND");
			}
			return;
		}
		if (isButton(button_stranger_remove) || isButton(button_accept_0))//移除黑名单
		{
			m_nUserCid = widget->getParent()->getTag();
			m_eConfirm = CONFIRM_INDEX_BLOCK_REMOVE;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_REMOVE_BLOCK_TITLE", "TIP_REMOVE_BLOCK");
			return;
		}
	}
}

void UINationWarHUD::updateBySecond(const float fTime)
{
	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_UI_CSB]);
	if (view)
	{
		//倒计时
		if (!getViewRoot(COMMOM_COCOS_RES[C_VIEW_NATION_WAR_LOADING]))
		{
			m_nCountryWarTime--;
			auto t_time = view->getChildByName<Text*>("text_retreat_time");
			t_time->setString(timeUtil(m_nCountryWarTime, TIME_UTIL::_TU_MIN_SEC, TIME_TYPE::_TT_GM));
			if (m_nCountryWarTime == 0)
			{
				t_time->setVisible(false);
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_WAR_RETREAT_SAFELY");
			}
		}

		//聊天提示红点
		auto b_unread = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_68"));
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
			b_unread->setVisible(true);
		}
		else
		{
			b_unread->setVisible(false);
		}


		m_pResult->left_time--;
		if (m_pResult->left_time <= 0)
		{
			this->unschedule(schedule_selector(UINationWarHUD::updateBySecond));
			m_eConfirm = CONFIRM_INDEX_WAR_END;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_TIME_OVER");
		}
	}
}

void UINationWarHUD::cityAttackShip(int attack, int position)
{
	m_pShipFleetInfo->ships[position]->hp -= attack;

	if (m_pShipFleetInfo->ships[position]->hp <= 0)
	{
		m_pShipFleetInfo->ships[position]->hp = 0;
	}
	updateShipDuable(position);
}

void UINationWarHUD::updateShipDuable(int position)
{
	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_UI_CSB]);
	if (view)
	{
		auto p_fleet = view->getChildByName<Widget*>("panel_fleet");
		auto p_ship = p_fleet->getChildByName<Widget*>(StringUtils::format("panel_ship_%d", position + 1));
		auto t_durable = p_ship->getChildByName<Text*>("label_ship_durable_num_1");
		auto l_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(p_ship, "progressbar_durable"));
		t_durable->setString(StringUtils::format("%d/%d", m_pShipFleetInfo->ships[position]->hp, m_pShipFleetInfo->ships[position]->max_hp));
		l_durable->setPercent(100.0 * m_pShipFleetInfo->ships[position]->hp / m_pShipFleetInfo->ships[position]->max_hp);

		if (m_pShipFleetInfo->ships[position]->hp <= 0)
		{
			auto i_bg = p_ship->getChildByName<ImageView*>("image_material_bg");
			auto i_ship = i_bg->getChildByName<ImageView*>("image_ship");
			setGLProgramState(i_ship, true);
		}

		bool is_die = true;
		for (size_t i = 0; i < m_pShipFleetInfo->n_ships; i++)
		{
			if (m_pShipFleetInfo->ships[i]->hp > 0)
			{
				is_die = false;
			}
		}
		//玩家自己死亡
		if (is_die)
		{
			dynamic_cast<WarManage*>(m_pParent)->countryWarStop();
			m_eConfirm = CONFIRM_INDEX_DIED;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_PLAYER_DIED");
		}
	}
}

void UINationWarHUD::shipAttackCity(int attack)
{
	m_pCityInfo->start_hp -= attack;
	updateCityDuable(true);
}

void UINationWarHUD::updateCityDuable(bool isAtt)
{
	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_UI_CSB]);
	if (view)
	{
		if (m_pCityInfo->start_hp <= 0)
		{
			m_pCityInfo->start_hp = 0;
			dynamic_cast<WarManage*>(m_pParent)->countryWarStop();
			m_eConfirm = CONFIRM_INDEX_CITY_BREACHED;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_CITY_BREACHED");
		}

		auto t_duable = view->getChildByName<Text*>("Text_1");
		auto l_duable = view->getChildByName<LoadingBar*>("loadingbar_duable");
		t_duable->setString(StringUtils::format("%d/%d", m_pCityInfo->start_hp, m_pCityInfo->max_hp));
		l_duable->setPercent(100.0 * m_pCityInfo->start_hp / m_pCityInfo->max_hp);

		if (isAtt)
		{
			//闪烁特效
			auto i_light = view->getChildByName<ImageView*>("image_progress_duable_light");
			i_light->setVisible(true);
			i_light->runAction(Sequence::createWithTwoActions(Blink::create(0.8f, 2), Hide::create()));
		}
		else
		{
			auto animation = dynamic_cast<WarManage*>(m_pParent)->getEffectManage()->getAnimation(PT_RECOVER_GREEN);
			auto animateEff = Animate::create(animation->clone());
			auto eff = Sprite::create();
			view->addChild(eff);
			eff->setScale(0.5f);
			eff->setAnchorPoint(Vec2(0.5, 0.5));
			auto winSize = Director::getInstance()->getWinSize();
			eff->setPosition(Vec2(winSize.width / 2, winSize.height/ 2));
			eff->runAction(Sequence::createWithTwoActions(animateEff, RemoveSelf::create(true)));

		}

		auto warMap = dynamic_cast<WarManage*>(m_pParent)->getWarMapLayer();
		warMap->updateCityButton(m_pCityInfo->start_hp, m_pCityInfo->max_hp);
	}
}


/*
* 刷新聊天室窗口信息
*/
void UINationWarHUD::flushMainChat()
{

	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_UI_CSB]);
	if (view)
	{
		auto i_chat = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "image_chat"));
		auto p_chat = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_chat"));
		auto seaChat = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_chat_content"));

		if (!m_pMainChat)
		{
			m_pMainChat = RichText::create();
			p_chat->addChild(m_pMainChat);
		}
		if (m_nChatItem)
		{
			m_pMainChat->removeElement(1);
			m_pMainChat->removeElement(0);
			m_nChatItem = 0;
		}

		if (UISocial::getInstance()->getSocialChat().empty())
		{
			m_bIsFirstChat = false;
			i_chat->setVisible(false);
			m_pMainChat->runAction(Sequence::createWithTwoActions(DelayTime::create(1),
				CallFunc::create(CC_CALLBACK_0(UINationWarHUD::flushMainChat, this))));
		}
		else
		{
			m_nChatItem = 2;
			if (UISocial::getInstance()->getSocialChat().size() - 1 > UISocial::getInstance()->m_nChatIndex)
			{
				UISocial::getInstance()->m_nChatIndex++;
				m_nShowChatTime = 0;
			}
			m_nShowChatTime++;
			if (m_bIsFirstChat)
			{
				m_bIsFirstChat = false;
				m_nShowChatTime = 5;
			}
			std::string name;
			name += "[";
			name += UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).channel;
			name += "] ";
			std::string content;
			content += UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).name;
			if (content == "")
			{
				if (UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).channel == SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC"])
				{
					content += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_SYSTEM_NOTICE"];
				}
				else if (UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).channel == SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC_ZH"])
				{
					content += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_SYSTEM_NOTICE_ZH"];
				}
				else
				{
					content += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_GUILD_NOTICE"];
				}
			}
			content += ":";
			content += UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).content;
			auto t_name = RichElementText::create(0, Color3B(154, 248, 255), 255, name, "Helvetica", 24);
			auto t_content = RichElementText::create(1, Color3B(255, 255, 255), 255, content, "Helvetica", 24);
			m_pMainChat->pushBackElement(t_name);
			m_pMainChat->pushBackElement(t_content);

			m_pMainChat->formatText();
			m_pMainChat->setPosition(Vec2(m_pMainChat->getContentSize().width*0.45 + i_chat->getContentSize().width, seaChat->getPositionY()));
			float t_tatol = m_pMainChat->getContentSize().width + i_chat->getContentSize().width;
			float time = t_tatol / 100;

			if (m_nShowChatTime < 4)
			{
				i_chat->setVisible(true);
				m_pMainChat->runAction(Sequence::createWithTwoActions(MoveTo::create(time, Vec2(-m_pMainChat->getContentSize().width*0.55, m_pMainChat->getPositionY())),
					CallFunc::create(CC_CALLBACK_0(UINationWarHUD::flushMainChat, this))));
			}
			else
			{
				i_chat->setVisible(false);
				m_pMainChat->runAction(Sequence::createWithTwoActions(DelayTime::create(1),
					CallFunc::create(CC_CALLBACK_0(UINationWarHUD::flushMainChat, this))));
			}
		}
	}
}

void UINationWarHUD::setCityAndShipFleetInfo(CityStatus *pCityInfo, NationWarCharacterStatus *pShipFleetInfo)
{
	m_pCityInfo = pCityInfo;
	if (m_pLastShipFleetInfo)
	{
		m_pLastShipFleetInfo = m_pShipFleetInfo;
	}
	else
	{
		m_pLastShipFleetInfo = pShipFleetInfo;
	}
	m_pShipFleetInfo = pShipFleetInfo;
	
	for (size_t i = 0; i < m_pShipFleetInfo->n_ships; i++)
	{
		m_pShipFleetInfo->ships[i]->hp = m_pLastShipFleetInfo->ships[i]->hp;
	}
}

void UINationWarHUD::countryWarStop()
{
	this->unschedule(schedule_selector(UINationWarHUD::updateBySecond));
}

void UINationWarHUD::cityDied()
{
	//城市被攻破
	if (m_pCityInfo->start_hp <= 0)
	{
		m_eConfirm = CONFIRM_INDEX_CITY_BREACHED;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_CITY_BREACHED");
		dynamic_cast<WarManage*>(m_pParent)->countryWarStop();
	}
}

void UINationWarHUD::openUserView(GetUserInfoByIdResult *result)
{
	SINGLE_HERO->m_InvitedToGuildNation = result->nation;
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushFriendDetail(result, false);
}

void UINationWarHUD::openTipsView(int index)
{
	switch (index)
	{
	case 1:
		m_eConfirm = CONFIRM_INDEX_WAR_END;
		UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_TIME_OVER");
		break;
	case 2:
		m_eConfirm = CONFIRM_INDEX_DIED;
		UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_PLAYER_DIED");
		break;
	case 3:
		if (m_nCountryWarTime < 1)
		{
			m_eConfirm = CONFIRM_INDEX_QUIT;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_COUNTRY_WAR_RETREAT_TITLE", "TIP_COUNTRY_WAR_NOT_RETREAT_CONTENT");
		}
		else
		{
			m_eConfirm = CONFIRM_INDEX_NOT_QUIT;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_COUNTRY_WAR_NOT_RETREAT_TITLE", "TIP_COUNTRY_WAR_NOT_RETREAT_CONTENT");
		}
		break;
	case 4:
		openReconnectDialog("TIP_CONNECT_FAIL");
		break;
	default:
		break;
	}
}

CityStatus *UINationWarHUD::getCityStatus()
{
	return m_pCityInfo;
}

void UINationWarHUD::changeLabel()
{
	auto view1 = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NATION_WAR_LOADING]);
	auto t_content = view1->getChildByName<Text*>("label_content");
	m_nLoadingNum++;
	if (m_nLoadingNum > 20)
	{
		m_nLoadingNum = 1;
	}
	std::string st_content = StringUtils::format("TIP_COUNTRY_WAR_LOADING_%d", m_nLoadingNum);
	t_content->setString(SINGLE_SHOP->getTipsInfo()[st_content]);
}