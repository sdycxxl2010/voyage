/*
 * AttackPirate.cpp
 *
 *  Created on: Dec 12, 2015
 *      Author: xijing
 */
#include "UIPirate.h"
#include "ProtocolThread.h"

#include "UICommon.h"
#include "UIInform.h"

#include "UISailManage.h"
#include "TVBattleManager.h"
#include "UIChatHint.h"
#include "TVSceneLoader.h"

UIPriate::UIPriate()
{
	m_pAttactPirateResult = nullptr;
	m_pMainButton = nullptr;
	m_pView = nullptr; 
	m_bAttackMoveActionFlag = true;
	m_pBossResult = nullptr;
	m_pRankResult = nullptr;
}

UIPriate::~UIPriate()
{
	unregisterCallBack();
}


void UIPriate::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetAttackPirateBossInfoResult:
	{
		GetAttackPirateBossInfoResult * result = (GetAttackPirateBossInfoResult*)message;
		if (result->failed == 0)
		{
			m_pBossResult = result;
			if (m_pView)
			{
				changeMainButtonState(0);
			}
			else
			{
				attackPirateGuardExitScene(0);
				this->scheduleOnce(schedule_selector(UIPriate::changeMainButtonState), 0.5f);
			}
			openBoss(0);
		}
		else
		{
			m_bAttackMoveActionFlag = true;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_SERVER_DATA_FIAL");
		}
		break;
	}
	case PROTO_TYPE_GetAttackPirateRankInfoResult:
	{
		GetAttackPirateRankInfoResult * result = (GetAttackPirateRankInfoResult*)message;
		if (result->failed == 0)
		{
			m_pRankResult = result;
			if (m_pView)
			{
				changeMainButtonState(0);
			}
			else
			{
				attackPirateGuardExitScene(0);
				this->scheduleOnce(schedule_selector(UIPriate::changeMainButtonState), 0.5f);
			}
			openRanking(0);
		}
		else
		{
			m_bAttackMoveActionFlag = true;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_SERVER_DATA_FIAL");
		}
		break;
	}
	case PROTO_TYPE_EngageInFightResult:
	{
		EngageInFightResult* result = (EngageInFightResult*)message;
		if (result->failed == 0)
		{
			SINGLE_AUDIO->setMusicType();
			SINGLE_HERO->m_pBattelData = result;
			SINGLE_HERO->shippos = Vec2(0, 0);
			ProtocolThread::GetInstance()->forceFightStatus();
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			CHANGETO(SCENE_TAG::BATTLE_TAG);
		}
		else
		{
			m_bAttackMoveActionFlag = true;
			//攻击海盗基地次数已用完
			if (result->failed == 19)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_ATTACT_PIRATE_NOT_TIMES");
			}
			//攻击海盗基地时间没有到
			else if (result->failed == 15)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_ATTACT_PIRATE_NOT_IN_TIME");
			}
			//攻击海盗基地--海盗已经死
			else if (result->failed == 17)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_ATTACT_PIRATE_HAS_BEEN_BREACHED");
			}
			//舰队中没有船只
			else if (result->failed == 5)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_ACTIVITY_PIRATE_ATTACK_NOT_SHIP");
			}
			//没有水手
			else if (result->failed == 6)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_ACTIVITY_PIRATE_ATTACK_NOT_SAILOR");
			}
		}
		break;
	}
	default:
		break;
	}
}

void UIPriate::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto button = dynamic_cast<Widget*>(pSender);
		std::string name = button->getName();
		if (!m_bAttackMoveActionFlag)
		{
			return;
		}
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//海盗攻城--选择难度或攻击海盗基地--boss界面
		if (isButton(button_city_info))
		{
			m_bAttackMoveActionFlag = false;
			if (m_pMainButton)
			{
				m_pMainButton->setBright(true);
				m_pMainButton->setTouchEnabled(true);
				m_pMainButton->setPositionX(m_pMainButton->getPositionX() + 20);
			}
			m_pMainButton = button;
			if (m_bIsAttackPirate)
			{
				ProtocolThread::GetInstance()->getAttackPirateBossInfo(SINGLE_HERO->m_nBossEventId, UILoadingIndicator::create(this));
			}
			else
			{
				if (m_pView)
				{
					changeMainButtonState(0);
				}
				else
				{
					attackPirateGuardExitScene(0);
					this->scheduleOnce(schedule_selector(UIPriate::changeMainButtonState), 0.5f);
				}
				openChooseLevel(0);
			}
			return;
		}
		//海盗攻城--积分说明或攻击海盗基地--排行榜
		if (isButton(button_hall_of_honor))
		{
			m_bAttackMoveActionFlag = false;
			if (m_pMainButton)
			{
				m_pMainButton->setBright(true);
				m_pMainButton->setTouchEnabled(true);
				m_pMainButton->setPositionX(m_pMainButton->getPositionX() + 20);
			}
			m_pMainButton = button;

			if (m_bIsAttackPirate)
			{
				ProtocolThread::GetInstance()->getAttackPirateRankInfo(UILoadingIndicator::create(this));
			}
			else
			{
				if (m_pView)
				{
					changeMainButtonState(0);
				}
				else
				{
					attackPirateGuardExitScene(0);
					this->scheduleOnce(schedule_selector(UIPriate::changeMainButtonState), 0.5f);
				}
				openFeatsOfIntegral(0);
			}
				
			return;
		}
		//规则说明
		if (isButton(button_investment))
		{
			m_bAttackMoveActionFlag = false;
			if (m_pMainButton)
			{
				m_pMainButton->setBright(true);
				m_pMainButton->setTouchEnabled(true);
				m_pMainButton->setPositionX(m_pMainButton->getPositionX() + 20);
			}
			m_pMainButton = button;

			if (m_pView)
			{
				changeMainButtonState(0);
			}
			else
			{
				attackPirateGuardExitScene(0);
				this->scheduleOnce(schedule_selector(UIPriate::changeMainButtonState), 0.5f);
			}

			if (m_bIsAttackPirate)
			{
				openRule(0);
			}
			else
			{
				openPirateAttackRule(0);
			}
			return;
		}

		//攻击海盗基地--进入战斗界面
		if (isButton(button_attack))
		{
			if (m_bIsAttackPirate)
			{
				//0可以攻击 1没有票 2boss没有hp 3时间没到
				switch (m_pBossResult->canattack)
				{
				case 0:
				{
					ProtocolThread::GetInstance()->engageInFight(SINGLE_HERO->m_nBossEventId, -1, -1, FIGHT_TYPE_ATTACK_PIRATE, -1, UILoadingIndicator::create(this));
					break;
				}
				case 1:
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_ATTACT_PIRATE_NOT_TIMES");
					break;
				}
				case 2:
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_ATTACT_PIRATE_HAS_BEEN_BREACHED");
					break;
				}
				case 3:
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_ATTACT_PIRATE_NOT_IN_TIME");
					break;
				}
				default:
					break;
				}
			}
			else
			{
				m_bAttackMoveActionFlag = false;
				ProtocolThread::GetInstance()->engageInFight(-1, -1, -1, FIGHT_TYPE_PIRATE_ATTACK, button->getTag(), UILoadingIndicator::create(this));
			}
			return;
		}
		//界面的返回按钮
		if (isButton(button_backcity))
		{
			unregisterCallBack();
			if (m_bIsAttackPirate)
			{
				Scene* sc = Scene::create();
				auto mps = UISailManage::create(SINGLE_HERO->shippos, 1);
				mps->setTag(MAP_TAG + 100);
				sc->addChild(mps);
				Director::getInstance()->replaceScene(sc);
			}
			else
			{
				button_callBack();
			}
			return;
		}
	}
}

void UIPriate::changeMainButtonState(const float fTime)
{
	if (m_pMainButton)
	{
		m_pMainButton->setTouchEnabled(false);
		m_pMainButton->setBright(false);
		m_pMainButton->setPositionX(m_pMainButton->getPositionX() - 20);
	}
}
/*
*初始化--根据传入的数据区分海盗基地和海盗攻城
*初始化上方标题等元素的显示以及按钮上文字-不同的背景图片
*/
bool UIPriate::init(struct _GetAttackPirateInfoResult*data, struct _GetPirateAttackInfoResult *pPirateAttactResult)
{
	bool pRet = false;
	do
	{
		setUIType(UI_ATTACK);
		playAudio();
		if (data)
		{
			m_bIsAttackPirate = true;
			m_pAttactPirateResult = data;
		}
		if (pPirateAttactResult)
		{
			m_bIsAttackPirate = false;
			m_pPirateAttactResult = pPirateAttactResult;
		}

		SINGLE_HERO->m_heroIsOnsea = false;
		registerCallBack();

		openView(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_CSB]);
		auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_CSB]);
		openView(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_TWO_CSB], 10);
		auto view2 = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_TWO_CSB]);
		attackPirateGuardEnterScene(0);

		auto panel_actionbar = view2->getChildByName<Widget*>("panel_actionbar");
		auto t_ongoing = panel_actionbar->getChildByName<Text*>("label_ongoing");
		auto t_nostart = panel_actionbar->getChildByName<Text*>("label_nostart");
		auto i_start = panel_actionbar->getChildByName<ImageView*>("image_time_start");
		auto i_nostart = panel_actionbar->getChildByName<ImageView*>("image_time_off");
		auto image_ic = panel_actionbar->getChildByName<ImageView*>("image_ic");
		auto t_event = i_nostart->getChildByName<Text*>("label_event");
		auto t_event_0 = i_nostart->getChildByName<Text*>("label_event_0_0");
		auto t_title = panel_actionbar->getChildByName<Text*>("label_title");
	
		auto panel_two_butter = view2->getChildByName<Widget*>("panel_two_butter");
		panel_two_butter->setVisible(false);
	
		auto t_attack = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_two_butter, "label_city_info"));
		auto t_glory = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_two_butter, "label_hall_of_honor"));
		auto t_rules = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_two_butter, "label_investment"));

		auto panel_two_butter2 = view->getChildByName<Widget*>("panel_two_butter");
		auto t_attack2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_two_butter2, "label_city_info"));
		auto t_glory2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_two_butter2, "label_hall_of_honor"));
		auto t_rules2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_two_butter2, "label_investment"));
		auto i_bg = view->getChildByName<ImageView*>("image_bg");
		if (m_bIsAttackPirate)
		{
			t_attack->setString(SINGLE_SHOP->getTipsInfo()["TIP_ATTACT_PIRATE_BUTTON_BOSS"]);
			t_glory->setString(SINGLE_SHOP->getTipsInfo()["TIP_ATTACT_PIRATE_BUTTON_RANKING"]);
			t_rules->setString(SINGLE_SHOP->getTipsInfo()["TIP_ATTACT_PIRATE_BUTTON_RULES"]);
			t_attack2->setString(SINGLE_SHOP->getTipsInfo()["TIP_ATTACT_PIRATE_BUTTON_BOSS"]);
			t_glory2->setString(SINGLE_SHOP->getTipsInfo()["TIP_ATTACT_PIRATE_BUTTON_RANKING"]);
			t_rules2->setString(SINGLE_SHOP->getTipsInfo()["TIP_ATTACT_PIRATE_BUTTON_RULES"]);
			t_ongoing->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_ONGOING"]);
			t_nostart->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_NOSTART"]);
			t_event->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_NOSTART_TITLE1"]);
			t_event_0->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_NOSTART_TITLE4"]);
			t_title->setString(SINGLE_SHOP->getBattleNpcInfo()[m_pAttactPirateResult->bossid].name);

			if (m_pAttactPirateResult->canattackpirate)
			{
				i_start->setVisible(true);
				t_ongoing->setVisible(true);
			}
			else
			{
				i_nostart->setVisible(true);
				t_nostart->setVisible(true);
			}
			image_ic->loadTexture("res/city_icon/boss.png");
			i_bg->loadTexture("res/activity_bg/boss_bg.jpg");
		}
		else
		{
			t_attack->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_BUTTON_ATTACK"]);
			t_glory->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_BUTTON_SCORE"]);
			t_rules->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_BUTTON_RULES"]);
			t_attack2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_BUTTON_ATTACK"]);
			t_glory2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_BUTTON_SCORE"]);
			t_rules2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_BUTTON_RULES"]);
			i_start->setVisible(true);
			t_ongoing->setVisible(true);
			t_ongoing->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_ONGOING"]);
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_ACTIVITY_PIRATE_ATTACK_TITLE"]);
			i_bg->loadTexture("res/activity_bg/pirate_1.jpg");
		}

		updateBySecond(0);
		this->schedule(schedule_selector(UIPriate::updateBySecond), 1.0f);
		

		//聊天
		auto ch = UIChatHint::createHint();
		this->addChild(ch, 10);
		pRet = true;
	} while (0);
	return pRet;
}
/*
*开始时的倒计时的实现
*/
void UIPriate::updateBySecond(const float fTime)
{
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_TWO_CSB]);

	auto panel_actionbar = view->getChildByName<Widget*>("panel_actionbar");
	auto t_ongoing = panel_actionbar->getChildByName<Text*>("label_ongoing");
	auto t_nostart = panel_actionbar->getChildByName<Text*>("label_nostart");
	auto i_start = panel_actionbar->getChildByName<ImageView*>("image_time_start");
	auto i_nostart = panel_actionbar->getChildByName<ImageView*>("image_time_off");
	auto text_time = dynamic_cast<Text*>(Helper::seekWidgetByName(i_start, "label_event_0"));
	if (m_bIsAttackPirate)
	{
		int64_t time = 0;
		if (m_pAttactPirateResult->canattackpirate)
		{
			m_pAttactPirateResult->distoend--;
			time = m_pAttactPirateResult->distoend;
			if (m_pAttactPirateResult->distoend < 0)
			{
				m_pAttactPirateResult->canattackpirate = 0;
				m_pAttactPirateResult->distostart = 5 * 24 * 3600;
				i_nostart->setVisible(true);
				i_start->setVisible(false);
				t_ongoing->setVisible(false);
				t_nostart->setVisible(true);
			}
		}
		else
		{
			m_pAttactPirateResult->distostart--;
			time = m_pAttactPirateResult->distostart;
			if (m_pAttactPirateResult->distostart < 0)
			{
				m_pAttactPirateResult->canattackpirate = 1;
				m_pAttactPirateResult->distoend = 2 * 24 * 3600;
				i_nostart->setVisible(false);
				i_start->setVisible(true);
				t_ongoing->setVisible(true);
				t_nostart->setVisible(false);
			}
		}

		int d = time / (24 * 3600);
		int h = (time % (24 * 3600)) / 3600;
		int m = (time % 3600) / 60;
		int s = time % 60;

		std::string str_content = SINGLE_SHOP->getTipsInfo()["TIP_ATTACT_PIRATE_TIME"];
		std::string old_vaule;
		char new_vaule[30];
		snprintf(new_vaule, sizeof(new_vaule), "%02d:%02d:%02d", h, m, s);
		old_vaule = "[hour]";
		repalce_all_ditinct(str_content, old_vaule, new_vaule);
		snprintf(new_vaule, sizeof(new_vaule), "%d", d);
		old_vaule = "[day]";
		repalce_all_ditinct(str_content, old_vaule, new_vaule);
		text_time->setString(str_content);
	}
	else
	{
		if (m_pPirateAttactResult->distoend == 0)
		{
			i_nostart->setVisible(true);
			i_start->setVisible(false);
			t_ongoing->setVisible(false);
			t_nostart->setVisible(true);
			t_nostart->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_NOSTART"]);
			auto t_event = i_nostart->getChildByName<Text*>("label_event");
			auto t_event_0 = i_nostart->getChildByName<Text*>("label_event_0");
			auto t_event_0_0 = i_nostart->getChildByName<Text*>("label_event_0_0");
			t_event->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_NOSTART_TITLE1"]);
			t_event_0->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_NOSTART_TITLE2"]);
			t_event_0_0->setString(SINGLE_SHOP->getTipsInfo()["TIP_PIRATE_ATTACT_NOSTART_TITLE3"]);
			this->unschedule(schedule_selector(UIPriate::updateBySecond));
			return;
		}
		m_pPirateAttactResult->distoend--;

		int h = (m_pPirateAttactResult->distoend % (24 * 3600)) / 3600;
		int m = (m_pPirateAttactResult->distoend % 3600) / 60;
		int s = m_pPirateAttactResult->distoend % 60;
		text_time->setString(StringUtils::format("%02d:%02d:%02d", h, m, s));
	}
}

void UIPriate::attackPirateGuardEnterScene(const float fTime)
{
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_CSB]);
	auto panel_adjutant_dialog = view->getChildByName<Widget*>("panel_adjutant_dialog");
	panel_adjutant_dialog->setPosition(STARTPOS);
	auto i_soldier = panel_adjutant_dialog->getChildByName<ImageView*>("image_bargirl");

	auto i_dialog = panel_adjutant_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = i_dialog->getChildByName<Text*>("label_bargirl");
	auto t_content = i_dialog->getChildByName<Text*>("label_content");
	auto i_anchor = i_dialog->getChildByName<ImageView*>("image_anchor");

	i_dialog->setOpacity(0);
	t_guard->setOpacity(0);
	t_content->setOpacity(0);
	i_anchor->setVisible(false);
	i_soldier->loadTexture(getCheifIconPath());
	i_soldier->runAction(MoveBy::create(0.5, Vec2(i_soldier->getBoundingBox().size.width, 0)));
	auto tips = SINGLE_SHOP->getTipsInfo();
	if (SINGLE_HERO->m_iGender == 1)
	{
		t_guard->setString(tips["TIP_MAIN_FEMAIE_AIDE"]);
	}
	else
	{
		t_guard->setString(tips["TIP_MAIN_MAIE_AIDE"]);
	}
	if (m_bIsAttackPirate)
	{
		t_content->setString(tips["TIP_ATTACT_PIRATE_AIDE_CONTENT"]);
	}
	else
	{
		t_content->setString(tips["TIP_PIRATE_ATTACT_AIDE_CONTENT"]);
	}
	i_dialog->setPositionX(i_dialog->getPositionX() + i_soldier->getBoundingBox().size.width);
	i_dialog->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), FadeIn::create(0.5f)));
	t_guard->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), FadeIn::create(0.5f)));
	t_content->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), FadeIn::create(0.5f)));
	//i_anchor->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), FadeIn::create(0.5f)));
}

void UIPriate::attackPirateGuardExitScene(const float fTime)
{
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_CSB]);
	auto panel_adjutant_dialog = view->getChildByName<Widget*>("panel_adjutant_dialog");
	panel_adjutant_dialog->setPosition(STARTPOS);
	auto i_soldier = panel_adjutant_dialog->getChildByName<ImageView*>("image_bargirl");

	auto i_dialog = panel_adjutant_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = i_dialog->getChildByName<Text*>("label_bargirl");
	auto t_content = i_dialog->getChildByName<Text*>("label_content");
	auto i_anchor = i_dialog->getChildByName<ImageView*>("image_anchor");

	i_dialog->stopAllActions();
	t_guard->stopAllActions();
	t_content->stopAllActions();
	//i_anchor->stopAllActions();

	i_dialog->runAction(FadeOut::create(0.5f));
	t_guard->runAction(FadeOut::create(0.5f));
	t_content->runAction(FadeOut::create(0.5f));
	//i_anchor->runAction(FadeOut::create(0.5f));

	auto view2 = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_TWO_CSB]);
	auto panel_two_butter2 = view2->getChildByName<Widget*>("panel_two_butter");
	panel_two_butter2->runAction(Sequence::createWithTwoActions(DelayTime::create(0.8f), Show::create()));
	m_pMainButton = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_two_butter2, m_pMainButton->getName()));

	auto panel_two_butter = view->getChildByName<Widget*>("panel_two_butter");
	auto b_attack = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_two_butter, "button_city_info"));
	auto b_glory = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_two_butter, "button_hall_of_honor"));
	auto b_rules = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_two_butter, "button_investment"));
	auto attack1 = DelayTime::create(0.5f);
	auto attack2 = EaseBackOut::create(MoveBy::create(0.2f, Vec2(120, 0)));
	auto seq1 = Sequence::create(attack1, attack2, nullptr);
	b_attack->runAction(seq1);

	auto glory1 = DelayTime::create(0.51f);
	auto glory2 = EaseBackOut::create(MoveBy::create(0.2f, Vec2(120, 0)));
	auto seq2 = Sequence::create(glory1, glory2, nullptr);
	b_glory->runAction(seq2);

	auto rules1 = DelayTime::create(0.52f);
	auto rules2 = EaseBackOut::create(MoveBy::create(0.2f, Vec2(120, 0)));
	auto seq3 = Sequence::create(rules1, rules2, nullptr);
	b_rules->runAction(seq3);
	panel_two_butter->runAction(Sequence::createWithTwoActions(DelayTime::create(0.8f), Hide::create()));
	i_soldier->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), EaseBackOut::create(MoveBy::create(0.3, Vec2(-i_soldier->getBoundingBox().size.width, 0)))));
}

void UIPriate::openBoss(const float fTime)
{
	openView(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_BOSS_CSB]);
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_BOSS_CSB]);
	view->setPosition(ENDPOS3);
	auto b_attack = view->getChildByName<Button*>("button_attack");
	auto t_boss_name = view->getChildByName<Text*>("label_boss_name");
	t_boss_name->setString(SINGLE_SHOP->getBattleNpcInfo()[m_pBossResult->bossid].name);
	auto t_icsailor_num = view->getChildByName<Text*>("label_icsailor_num");
	t_icsailor_num->setString(StringUtils::format("%d", m_pBossResult->pirateflags));
	if (!m_pBossResult->canattack)
	{
		b_attack->setBright(true);
	}
	else
	{
		b_attack->setBright(false);
	}

	if (m_pBossResult->bosscurrenthp)
	{
		auto image_durable = view->getChildByName<ImageView*>("image_durable");
		image_durable->setVisible(true);
		auto label_ship_durable_num = image_durable->getChildByName<Text*>("label_ship_durable_num");
		label_ship_durable_num->setString(StringUtils::format("%d/%d", m_pBossResult->bosscurrenthp, m_pBossResult->bossmaxhp));
		auto progressbar_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(image_durable, "progressbar_durable"));
		progressbar_durable->setPercent(100.0 * m_pBossResult->bosscurrenthp / m_pBossResult->bossmaxhp);
	}
	else
	{
		auto panel_player = view->getChildByName<Widget*>("panel_player");
		panel_player->setVisible(true);
		auto image_flag = panel_player->getChildByName<ImageView*>("image_flag");
		auto t_name = panel_player->getChildByName<Text*>("label_name");
		image_flag->loadTexture(getCountryIconPath(m_pBossResult->killernation));
		if (m_pBossResult->killername)
		{
			t_name->setString(m_pBossResult->killername);
		}
		else
		{
			t_name->setString("N/A");
		}
		image_flag->setPositionX(t_name->getPositionX() - t_name->getBoundingBox().size.width - image_flag->getBoundingBox().size.height);
		t_icsailor_num->setTextColor(TEXT_RED);
	}
	
	
	int num = 1;
	if (m_pBossResult->bossid >= 2401 && m_pBossResult->bossid <= 2405)
	{
		num = 2;
	}
	else if (m_pBossResult->bossid >= 2406 && m_pBossResult->bossid <= 2410)
	{
		num = 3;
	}
	else
	{
		num = 1;
	}
	std::string str_content = StringUtils::format("TIP_ATTACT_PIRATE_BOSS_CONTENT_%d", num);
	auto listview_content = view->getChildByName<ListView*>("listview_content");
	auto t_boss_story = dynamic_cast<Text*>(listview_content->getItem(0));
	t_boss_story->setString(SINGLE_SHOP->getTipsInfo()[str_content]);
	t_boss_story->setContentSize(Size(t_boss_story->getContentSize().width, 10 + getLabelHight(t_boss_story->getString(), t_boss_story->getContentSize().width, t_boss_story->getFontName(), t_boss_story->getFontSize())));
	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto  button_pulldown = view->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(i_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listview_content, i_pulldown);

	float time = 1.0f;
	if (m_pView)
	{
		time = 0.5f;
		m_pView->runAction(EaseBackOut::create(MoveTo::create(0.5f, ENDPOS3)));
	}

	view->runAction(Sequence::createWithTwoActions(DelayTime::create(time), EaseBackOut::create(MoveTo::create(0.5f, STARTPOS))));
	m_pView = view;
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), CallFunc::create(CC_CALLBACK_0(UIPriate::attackMoveEnd, this))));

}

void UIPriate::openChooseLevel(const float fTime)
{
	openView(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_ATTACK_CSB]);
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_ATTACK_CSB]);
	view->setPosition(ENDPOS3);
	for (size_t i = 0; i < 3; i++)
	{
		auto b_attack = view->getChildByName<Button*>(StringUtils::format("button_attack_%d", i + 1));
		b_attack->setTag(i + 1);
	}
	float time = 1.0f;
	if (m_pView)
	{
		time = 0.5f;
		m_pView->runAction(EaseBackOut::create(MoveTo::create(0.5f, ENDPOS3)));
	}
	view->runAction(Sequence::createWithTwoActions(DelayTime::create(time), EaseBackOut::create(MoveTo::create(0.5f, STARTPOS))));
	m_pView = view;
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), CallFunc::create(CC_CALLBACK_0(UIPriate::attackMoveEnd, this))));
}

void UIPriate::openRanking(const float fTime)
{
	openView(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RANKING_CSB]);
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RANKING_CSB]);
	auto panel_title = view->getChildByName<Widget*>("panel_title");

	auto t_damageRanking = panel_title->getChildByName<Text*>("label_forces_relationship");
	auto t_rankingLv = panel_title->getChildByName<Text*>("label_forces_relationship_0");
	t_rankingLv->setString(StringUtils::format("(Lv. %d -- Lv. %d)", m_pRankResult->levelstart, m_pRankResult->levelend));

	t_damageRanking->setPositionX((panel_title->getBoundingBox().size.width - t_damageRanking->getBoundingBox().size.width -
		t_rankingLv->getBoundingBox().size.width - 20) / 2 + t_damageRanking->getBoundingBox().size.width / 2);
	t_rankingLv->setPositionX(t_damageRanking->getBoundingBox().size.width / 2 + t_rankingLv->getBoundingBox().size.width / 2 + t_damageRanking->getPositionX() + 10);

	auto panel_list = view->getChildByName<Widget*>("panel_list");
	auto l_ranking = view->getChildByName<ListView*>("listview_ranking");
	l_ranking->removeAllChildrenWithCleanup(true);
	for (size_t i = 0; i < m_pRankResult->n_rankinfo; i++)
	{
		auto item = panel_list->clone();
		auto t_rank = item->getChildByName<Text*>("label_rank");
		auto i_trophy = item->getChildByName<ImageView*>("image_trophy");
		auto i_nation = item->getChildByName<ImageView*>("image_country");
		auto t_name = item->getChildByName<Text*>("label_name");
		auto t_lv = item->getChildByName<Text*>("label_lv");
		auto t_admage = item->getChildByName<Text*>("label_silver_num");
		t_rank->setString(StringUtils::format("%d", i + 1));
		if (i < 3)
		{
			if (i + 1 == 1)
			{
				i_trophy->loadTexture(RANK_GOLD);
			}
			else if (i + 1 == 2)
			{
				i_trophy->loadTexture(RANK_SILVER);
			}
			else
			{
				i_trophy->loadTexture(RANK_COPPER);
			}
		}
		else
		{
			i_trophy->setVisible(false);
			t_rank->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		}
		i_nation->loadTexture(getCountryIconPath(m_pRankResult->rankinfo[i]->nation));
		t_name->setString(m_pRankResult->rankinfo[i]->username);
		t_lv->setString(StringUtils::format("Lv. %d", m_pRankResult->rankinfo[i]->level));
		t_admage->setString(StringUtils::format("%d", m_pRankResult->rankinfo[i]->hurt));
		l_ranking->pushBackCustomItem(item);
	}

	auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_head_16"));
	auto t_rank_title = view->getChildByName<Text*>("label_you_rank");
	auto t_rank_num = view->getChildByName<Text*>("label_you_rank_num");
	auto t_damage_title = view->getChildByName<Text*>("label_you_total_invest");
	auto t_damage_num = view->getChildByName<Text*>("label_you_total_invest_num");
	if (m_pRankResult->myrank)
	{
		t_rank_num->setString(StringUtils::format("%d", m_pRankResult->myrank));
	}
	else
	{
		t_rank_num->setString("N/A");
	}

	t_rank_num->setPositionX(t_rank_title->getBoundingBox().size.width + t_rank_title->getPositionX() + 10);
	t_damage_num->setString(StringUtils::format("%d", m_pRankResult->mydamage));
	t_damage_num->setPositionX(t_damage_title->getBoundingBox().size.width + t_damage_title->getPositionX() + 10);
	image_head->loadTexture(getPlayerIconPath(m_pRankResult->headicon));
	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	addListViewBar(l_ranking, i_pulldown);
	auto t_no_ranking = view->getChildByName<Text*>("label_no_ranking");
	if (m_pRankResult->n_rankinfo > 0)
	{
		t_no_ranking->setVisible(false);
	}
	else
	{
		t_no_ranking->setVisible(true);
	}
	view->setPosition(ENDPOS3);
	float time = 1.0f;
	if (m_pView)
	{
		time = 0.5f;
		m_pView->runAction(EaseBackOut::create(MoveTo::create(0.5f, ENDPOS3)));
	}

	view->runAction(Sequence::createWithTwoActions(DelayTime::create(time), EaseBackOut::create(MoveTo::create(0.5f, STARTPOS))));
	m_pView = view;
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), CallFunc::create(CC_CALLBACK_0(UIPriate::attackMoveEnd, this))));
}


void UIPriate::openRule(const float fTime)
{
	openView(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RULES_CSB]);
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RULES_CSB]);
	auto listview_rules = view->getChildByName<ListView*>("listview_rules");
	view->setPosition(ENDPOS3);
	for (size_t i = 0; i < 5; i++)
	{
		auto item = dynamic_cast<Text*>(listview_rules->getItem(i));
		std::string str_content = StringUtils::format("TIP_ATTACT_PIRATE_RULES_%d", i + 1);
		item->setString(SINGLE_SHOP->getTipsInfo()[str_content]);
		item->setContentSize(Size(item->getContentSize().width, 10 + getLabelHight(item->getString(), item->getContentSize().width, item->getFontName())));
	}

	auto panel_item = view->getChildByName<Widget*>("panel_list");
	auto boss_awards = SINGLE_SHOP->getBossAwards();
	int index = 0;
	switch (m_pAttactPirateResult->strength)
	{
	case 1:
		index = 1;
		break;
	case 2:
		index = 14;
		break;
	case 3:
		index = 27;
		break;
	case 4:
		index = 40;
		break;
	case 5:
		index = 53;
		break;
	default:
		break;
	}
	
	for (size_t i = 0; i < 10; i++, index++)
	{
		auto item = panel_item->clone();
		auto i_trophy = item->getChildByName<ImageView*>("image_trophy");
		auto t_rank = item->getChildByName<Text*>("label_rank");
		auto i_sailor = item->getChildByName<ImageView*>("image_sailor");
		auto t_sailor = item->getChildByName<Text*>("label_icsailor_num");
		auto i_coin = item->getChildByName<ImageView*>("image_coin");
		auto t_coin = item->getChildByName<Text*>("label_icsailor_num_0");
		auto i_gold = item->getChildByName<ImageView*>("image_gold");
		auto t_gold = item->getChildByName<Text*>("label_icsailor_num_0_0");
		t_rank->setString(StringUtils::format("%d", i + 1));
		if (i < 3)
		{
			if (i + 1 == 1)
			{
				i_trophy->loadTexture(RANK_GOLD);
			}
			else if (i + 1 == 2)
			{
				i_trophy->loadTexture(RANK_SILVER);
			}
			else
			{
				i_trophy->loadTexture(RANK_COPPER);
			}
		}
		else
		{
			i_trophy->setVisible(false);
			t_rank->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		}
		t_sailor->setString(StringUtils::format("%d", boss_awards[index].badge));
		t_coin->setString(StringUtils::format("%d", boss_awards[index].coin));
		t_gold->setString(StringUtils::format("%d", boss_awards[index].v_ticket));
		i_sailor->setPositionX(t_sailor->getPositionX() - t_sailor->getBoundingBox().size.width - i_sailor->getBoundingBox().size.width / 2 - 10);
		i_coin->setPositionX(t_coin->getPositionX() - t_coin->getBoundingBox().size.width - i_coin->getBoundingBox().size.width / 2 - 10);
		i_gold->setPositionX(t_gold->getPositionX() - t_gold->getBoundingBox().size.width - i_gold->getBoundingBox().size.width / 2 - 10);
		listview_rules->pushBackCustomItem(item);
	}
	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto  button_pulldown = view->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(i_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listview_rules, i_pulldown);

	float time = 1.0f;
	if (m_pView)
	{
		time = 0.5f;
		m_pView->runAction(EaseBackOut::create(MoveTo::create(0.5f, ENDPOS3)));
	}

	view->runAction(Sequence::createWithTwoActions(DelayTime::create(time), EaseBackOut::create(MoveTo::create(0.5f, STARTPOS))));
	m_pView = view;
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), CallFunc::create(CC_CALLBACK_0(UIPriate::attackMoveEnd, this))));
}


void UIPriate::closeOtherView(const float fTime)
{
	auto view1 = getViewRoot(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RULES_CSB]);
	if (m_pView != view1)
	{
		closeView(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RULES_CSB]);
	}
	auto view2 = getViewRoot(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RANKING_CSB]);
	if (m_pView != view2)
	{
		closeView(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RANKING_CSB]);
	}
	auto view3 = getViewRoot(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_BOSS_CSB]);
	if (m_pView != view3)
	{
		closeView(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_BOSS_CSB]);
	}
	auto view4 = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_ATTACK_CSB]);
	if (m_pView != view4)
	{
		closeView(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_ATTACK_CSB]);
	}
	auto view5 = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_SCORE_CSB]);
	if (m_pView != view5)
	{
		closeView(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_SCORE_CSB]);
	}
	auto view6 = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_RULES_CSB]);
	if (m_pView != view6)
	{
		closeView(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_RULES_CSB]);
	}
}

void UIPriate::openFeatsOfIntegral(const float fTime)
{
	openView(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_SCORE_CSB]);
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_SCORE_CSB]);
	view->setPosition(ENDPOS3);
	auto panel_glorypoints = view->getChildByName<Widget*>("panel_glorypoints");
	auto t_gp_num = panel_glorypoints->getChildByName<Text*>("text_gp_num");
	auto l_feats = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_glorypoints, "loadingbar_feats"));
	auto t_gp_num_1 = panel_glorypoints->getChildByName<Text*>("text_gp_num_1");
	auto t_gp_num_2 = panel_glorypoints->getChildByName<Text*>("text_gp_num_2");
	auto t_gp_num_3 = panel_glorypoints->getChildByName<Text*>("text_gp_num_3");

	auto t_gp_num_1_0 = panel_glorypoints->getChildByName<Text*>("text_gp_num_1_0");
	auto t_gp_num_2_0 = panel_glorypoints->getChildByName<Text*>("text_gp_num_2_0");
	auto t_gp_num_3_0 = panel_glorypoints->getChildByName<Text*>("text_gp_num_3_0");

	t_gp_num->setString(StringUtils::format("%d", m_pPirateAttactResult->myexploitscore));
	float num = 100.0 * m_pPirateAttactResult->myexploitscore / m_pPirateAttactResult->exploitsection[2];
	l_feats->setPercent(num);
	t_gp_num_1->setString("1");
	t_gp_num_2->setString("2");
	t_gp_num_3->setString("3");
	t_gp_num_1_0->setString(StringUtils::format("%d", m_pPirateAttactResult->exploitsection[0]));
	t_gp_num_2_0->setString(StringUtils::format("%d", m_pPirateAttactResult->exploitsection[1]));
	t_gp_num_3_0->setString(StringUtils::format("%d", m_pPirateAttactResult->exploitsection[2]));

	auto text_1 = view->getChildByName<Text*>("text_explanation_1");
	auto text_2 = view->getChildByName<Text*>("text_explanation_1_0");
	text_1->setContentSize(Size(text_1->getContentSize().width, 10 + getLabelHight(text_1->getString(), text_1->getContentSize().width, text_1->getFontName())));
	text_2->setContentSize(Size(text_2->getContentSize().width, 10 + getLabelHight(text_2->getString(), text_2->getContentSize().width, text_2->getFontName())));
	text_2->setPositionY(text_1->getPositionY() - text_1->getBoundingBox().size.height);

	float time = 1.0f;
	if (m_pView)
	{
		time = 0.5f;
		m_pView->runAction(EaseBackOut::create(MoveTo::create(0.5f, ENDPOS3)));
	}
	view->runAction(Sequence::createWithTwoActions(DelayTime::create(time), EaseBackOut::create(MoveTo::create(0.5f, STARTPOS))));
	m_pView = view;
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), CallFunc::create(CC_CALLBACK_0(UIPriate::attackMoveEnd, this))));
}

void UIPriate::openPirateAttackRule(const float fTime)
{
	openView(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_RULES_CSB]);
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_ATTACK_PIRATES_RULES_CSB]);
	view->setPosition(ENDPOS3);
	auto listview_rules = view->getChildByName<ListView*>("listview_rules");
	for (size_t i = 0; i < listview_rules->getChildrenCount(); i++)
	{
		auto item = dynamic_cast<Text*>(listview_rules->getItem(i));
		std::string str_content = StringUtils::format("TIP_PIRATE_ATTACT_RULES_%d", i + 1);
		item->setString(SINGLE_SHOP->getTipsInfo()[str_content]);
		item->setContentSize(Size(item->getContentSize().width, 10 + getLabelHight(item->getString(), item->getContentSize().width, item->getFontName())));
	}

	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto  button_pulldown = view->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(i_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listview_rules, i_pulldown);

	float time = 1.0f;
	if (m_pView)
	{
		time = 0.5f;
		m_pView->runAction(EaseBackOut::create(MoveTo::create(0.5f, ENDPOS3)));
	}

	view->runAction(Sequence::createWithTwoActions(DelayTime::create(time), EaseBackOut::create(MoveTo::create(0.5f, STARTPOS))));
	m_pView = view;
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), CallFunc::create(CC_CALLBACK_0(UIPriate::attackMoveEnd, this))));
}
