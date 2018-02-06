#include "UIBattleHUD.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
#include "TVBattleShareObj.h"
#include "TVBasicShip.h"

#include "WriterLog.h"
#include "UICommon.h"
#include "UIInform.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "TVSceneLoader.h"
#include "UINoviceStoryLine.h"
#include "MainTaskManager.h"
#include "CompanionTaskManager.h"
#include "TVBattleManager.h"
#include "UISailManage.h"
#include "UIPirate.h"
#include "TVSingle.h"
#include "UIStore.h"
#include "TVBattleMainShip.h"
#include "Utils.h"
#define  DIAMETER_LENGTH 1350.f

UIBattleHUD::UIBattleHUD():
	m_SmallMap(nullptr),
	m_bIsBattleEnd(true),
	m_pPrevTime(0)
{
	m_pWaitForLogUploading = 0;
	m_bIsFailed = false;
	m_nIconIndex = 0;
	m_RemainTime = 0;
	m_TotleTime = 0;
	m_eUIType = UI_BATTLE;
	for (int i = 0; i < 4; i++)
	{
		m_gSkillTime[i] = false;
		m_gPerSkillTimeNum[i] = 1;
		m_gSkillTimeNum[i] = 0;
	}

	m_TimeLoad = nullptr;
	m_pFight = nullptr;
	m_pShareObj = nullptr;
	m_pTimeShow = nullptr;
	m_pResult = nullptr;
	m_pDelegate = nullptr;
	m_bIsLevelUp = false;
	m_bIsPrestigeUp = false;
	m_bIsPrestigeDown = false;
	m_bIsCaptainUp = false;
	m_bIsSwitchAI = true;
	m_nSkillID=-1;
	m_pEndFightResult = nullptr;
	//对话走字
	lenNum = 0;
	lenAfter = 0;
	plusNum = 0;
	chatContent = "";
	anchPic = nullptr;
	chatTxt = nullptr;
	m_bDiaolgOver = false;
	m_bIsSendToData = true;
	m_pAttactPirateResult = nullptr;
	m_nCapturingShipId.clear();
	m_nFoodPlunderShipId.clear();
	m_sendBattleLogRetry = 2;
	m_vTips.clear();
	m_nTipIntervalTime = 0;
	m_bIsVTickesTip = false;
	m_bIsTrue = true;
	m_bIsTouch = false;
}

UIBattleHUD::~UIBattleHUD()
{
	if(m_pEndFightResult) end_fight_result__free_unpacked(m_pEndFightResult,0);

	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

UIBattleHUD* UIBattleHUD::createBattleUI(EngageInFightResult* result,TVBattleDelegate* dele)
{
	UIBattleHUD* bu = new UIBattleHUD;
	if (bu)
	{
		bu->m_pDelegate = dele;
		bu->m_pShareObj = dele->getShareObj();
		bu->init(result);
		bu->autorelease();
		return bu;
	}
	CC_SAFE_DELETE(bu);
	return nullptr;
}

void UIBattleHUD::init(EngageInFightResult* result)
{
	m_pResult = result;
	initStaticData(result);
	ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIBattleHUD::onServerEvent,this),this);
	int heroNum = result->n_myships;
	int enemyNum = result->n_enemyships;
	//左边的我方船
	for (int i = heroNum; i  < 5; i++)
	{
		std::string shipIcon_name = StringUtils::format("panel_player_%d",i+1);
		auto *w_hero = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFight, shipIcon_name));
		w_hero->setVisible(false);
	}
	//右边的敌方船
	for (int i = enemyNum; i  < 5; i++)
	{
		std::string enemy_name = StringUtils::format("panel_enemy_%d",i+1);
		auto *w_enemy = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFight, enemy_name));
		w_enemy->setVisible(false);
	}
}

void UIBattleHUD::onEnter()
{
	UIBasicLayer::onEnter();
	if (UserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO) == OPEN_AUDIO)
	{
		playAudio();
	}
}

void UIBattleHUD::onExit()
{
	UIBasicLayer::onExit();
}

void UIBattleHUD::initStaticData(EngageInFightResult* result)
{
	m_TotleTime = getFightTime(0,0);
	if (m_pDelegate->getContinueLastFight())
	{
		m_RemainTime = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(FIGHT_TIME).c_str(), 180000);
	}
	else
	{
		m_RemainTime = m_TotleTime;
	}

	auto winSize = Director::getInstance()->getVisibleSize();
	openView(MAPUI_COCOS_RES[INDEX_UI_FIGHT_UI_MAIN_CSB]);
	m_pFight = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FIGHT_UI_MAIN_CSB]);
	m_pFight->setTouchEnabled(false);

	ImageView* time_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pFight, "image_time_bg"));
	m_pTimeShow = dynamic_cast<Text*>(Helper::seekWidgetByName(time_bg, "text_time"));
	m_TimeLoad = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(time_bg, "progressbar_time"));

	// 左下小地图
	auto rate = winSize.width / DIAMETER_LENGTH;

	Widget* panel_radar = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFight, "panel_radar"));
	auto map_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_radar, "image_radar"));
	panel_radar->getChildByName<ImageView*>("image_me")->setVisible(false);

	auto map_pos = map_bg->getPosition() * m_pFight->getScaleX();
	auto map_consize = map_bg->getContentSize() * map_bg->getScale() * m_pFight->getScaleX();
	auto fakeScreen = Layer::create();
	auto fake_width = rate * map_consize.width;
	auto fake_height = winSize.height /winSize.width * fake_width;
	fakeScreen->setContentSize(Size(fake_width,fake_height));
	fakeScreen->setPosition(map_pos.x - fake_width/2,map_pos.y - fake_height/2);

	m_SmallMap = Layer::create();
	m_SmallMap->setContentSize(fakeScreen->getContentSize());
	m_SmallMap->setPosition(-fake_width,-fake_height);
	this->addChild(fakeScreen);//小地图的order
	fakeScreen->addChild(m_SmallMap);
	
	if (m_RemainTime / 1000 <= 30)
	{
		m_TimeLoad->loadTexture("ship/time_line_red.png");
	}
	std::string s_time = StringUtils::format("%d:%02d",m_RemainTime/1000/60,(m_RemainTime/1000) %60);
	m_pTimeShow->setString(s_time);
	m_TimeLoad->setPercent(100 * m_RemainTime / m_TotleTime);

	// hero ship init
	for (auto i = 0; i < result->n_myships; i++)
	{
		auto shipInfo = result->myships[i];
		std::string hero_name = StringUtils::format("panel_player_%d", i + 1);
		std::string s_index = StringUtils::format("%d", i + 1);
		std::string s_attack = StringUtils::format("%d", shipInfo->sailornum);
		Widget* w_hero = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFight, hero_name));
		auto i_ship_icon = w_hero->getChildByName<ImageView*>("image_ship");
		auto l_durable = w_hero->getChildByName<LoadingBar*>("progressbar_durable");
		auto i_position = w_hero->getChildByName<ImageView*>("image_num_1");

		i_ship_icon->ignoreContentAdaptWithSize(false);
		i_ship_icon->loadTexture(getShipIconPath(shipInfo->sid));
		i_position->loadTexture(getPositionIconPath(shipInfo->position + 1));

		auto st_sailor = StringUtils::format(MY_SHIP_SAILOR, i + 1);
		auto st_blood = StringUtils::format(MY_SHIP_BLOOD, i + 1);
		auto sailornum = shipInfo->sailornum;
		auto bloodnum = shipInfo->current_hp;
		if (m_pDelegate->getContinueLastFight())
		{
			sailornum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), 0);
			bloodnum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), 0);
		}

		if (sailornum > 0 && bloodnum > 0)
		{
			l_durable->setVisible(true);
			l_durable->setPercent((shipInfo->current_hp * 100.0) / shipInfo->max_hp);
		}
		else
		{
			l_durable->setVisible(false);
			l_durable->setPercent(0);
			setGLProgramState(i_ship_icon, true);
		}
		m_vHerosInfo.push_back(w_hero);
	}

	// enemys ship init
	for (auto i = 0; i < result->n_enemyships;i++)
	{
		auto shipInfo = result->enemyships[i];
		std::string enemy_name = StringUtils::format("panel_enemy_%d",i+1);
		Widget* w_enemy = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFight, enemy_name));
		auto i_ship_icon = w_enemy->getChildByName<ImageView*>("image_ship");
		auto l_durable   = w_enemy->getChildByName<LoadingBar*>("progressbar_durable");
		auto i_position = w_enemy->getChildByName<ImageView*>("image_num_1");

		i_ship_icon->ignoreContentAdaptWithSize(false);
		i_ship_icon->loadTexture(getShipIconPath(shipInfo->sid));
		auto st_sailor = StringUtils::format(ENEMY_SHIP_SAILOR, i + 1);
		auto st_blood = StringUtils::format(ENEMY_SHIP_BLOOD, i + 1);
		auto sailornum = shipInfo->sailornum;
		auto bloodnum = shipInfo->current_hp;
		if (m_pDelegate->getContinueLastFight())
		{
			sailornum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), 0);
			bloodnum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), 0);
		}
		if (sailornum > 0 && bloodnum > 0)
		{
			l_durable->setVisible(true);
			l_durable->setPercent((shipInfo->current_hp * 100.0) / shipInfo->max_hp);
		}
		else
		{
			l_durable->setVisible(false);
			l_durable->setPercent(0);
			setGLProgramState(i_ship_icon, true);
		}
		l_durable->setVisible(true);
		i_position->loadTexture(getPositionIconPath(i + 6));
		m_vEnemysInfo.push_back(w_enemy);
	}
	// top info init
	for (int i = 0; i < 2; i++)
	{
		std::string widgetName = StringUtils::format("panel_info_%d",i + 1);
		auto w_info = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFight, widgetName));
		auto t_name = w_info->getChildByName<Text*>("label_name");
		auto t_level = w_info->getChildByName<Text*>("label_level");
		if (0 == i)
		{
			std::string s_level = StringUtils::format("Lv. %d",SINGLE_HERO->m_iLevel);
			t_name->setString(SINGLE_HERO->m_sName);
			if (ProtocolThread::GetInstance()->isInTutorial())//新手引导
			{
				t_level->setString("Lv. 2");
			}
			else
			{
				t_level->setString(s_level);
			}		
		}else
		{
			std::string s_level = StringUtils::format("Lv. %d",result->enemyinfo->level);
			//离线委托，返回玩家名字
			if (result->fighttype == FIGHT_TYPE_ROB)
			{
				if (result->enemyinfo->name)
				{
					t_name->setString(result->enemyinfo->name);
				}
				else
				{
					t_name->setString("N/A");
				}
			}
			else
			{
				t_name->setString(SINGLE_SHOP->getBattleNpcInfo().find(result->npcid)->second.name);
			}

			if (ProtocolThread::GetInstance()->isInTutorial())//新手引导对方船
			{
				t_level->setString("Lv. 1");
			}
			else
			{
				t_level->setString(s_level);
			}			
		}
	}

	updateAnger(true);
	updateAnger(false);
	//button init 
	initMainStrategyButton(MODEL_FREE,m_bIsSwitchAI);
	initMainSkillOrPorpButton();

	onMainUI();
	
	if (m_pShareObj->m_vMyFriends.empty())
	{
		setMainShipDied();
	}
	else
	{
		if (m_pShareObj->m_vMyFriends.at(0)->getType() != MAIN_SHIP)
		{
			setMainShipDied();
		}
	}

	if (m_pDelegate->getContinueLastFight())
	{
		for (size_t i = 1; i <= 10; i++)
		{
			auto capturing_ship = StringUtils::format(CAPTURINGSHIPID, i);
			auto hex = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(capturing_ship.c_str()).c_str());
			char *st_num = 0;
			//解密
			if (hex.length() > 0)
			{
				st_num = getStringFromEncryptedHex((char*)hex.c_str());
			}
			int shipid = 0;
			if (st_num)
			{
				shipid = atoi(st_num);
				free(st_num);
			}
			if (shipid > 0)
			{
				m_nCapturingShipId.push_back(shipid);
			}

			auto food_plunder = StringUtils::format(FOODPLUNDERSHIPID, i);
			auto hex2 = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(food_plunder.c_str()).c_str());
			char *st_num2 = 0;
			//解密
			if (hex2.length() > 0)
			{
				st_num2 = getStringFromEncryptedHex((char*)hex2.c_str());
			}
			int shipid2 = 0;
			if (st_num2)
			{
				shipid2 = atoi(st_num2);
				free(st_num2);
			}
			if (shipid2 > 0)
			{
				m_nFoodPlunderShipId.push_back(shipid2);
			}
		}
	}
}

void UIBattleHUD::pushFightingCentralTips(FIGHTING_CENTER_TIP_DATA tip_data)
{
	m_vTips.push_back(tip_data);
}

void UIBattleHUD::showFightingCentralTips()
{
	if (m_vTips.empty())
	{
		return;
	}
	m_nTipIntervalTime = 3;
	auto tips = m_vTips.at(0);
	auto p_centralTip = m_pFight->getChildByName<Widget*>("panel_fudong");
	auto p_centralTip_clone = p_centralTip->clone();
	p_centralTip_clone->setPosition(Vec2(640, 360));
	m_pFight->addChild(p_centralTip_clone);
	p_centralTip_clone->setVisible(true);
	p_centralTip_clone->setAnchorPoint(Vec2(0.5, 0.5));
	p_centralTip_clone->setCameraMask(4);
	auto p_shipSunk = p_centralTip_clone->getChildByName<Widget*>("panel_destroyed");
	auto p_singled = p_centralTip_clone->getChildByName<Widget*>("panel_text");
	auto p_skill = p_centralTip_clone->getChildByName<Widget*>("panel_battleInfo");

	auto shipFail = p_shipSunk->getChildByName<ImageView*>("image_ship_bg");
	auto shipSuc = p_shipSunk->getChildByName<ImageView*>("image_ship_bg_0");
	auto shipIconSuc = shipSuc->getChildByName<ImageView*>("image_ship");
	auto shipIconFail = shipFail->getChildByName<ImageView*>("image_ship");
	auto shipNumSuc = shipSuc->getChildByName<ImageView*>("image_num");
	auto shipNumFail = shipFail->getChildByName<ImageView*>("image_num");
	auto t_shipSunk = p_shipSunk->getChildByName<Text*>("text_13");

	auto i_ship = p_skill->getChildByName<ImageView*>("image_captain_bg");
	auto i_position = i_ship->getChildByName<ImageView*>("image_num");
	auto t_skill = p_skill->getChildByName<Text*>("text_13");

	auto t_singled = p_singled->getChildByName<Text*>("text_onlytext");

	std::string str;
	std::string old_vaule;
	std::string new_vaule;
	std::string path1;
	std::string path2;

	if (tips.succ_is_captain == 2)
	{
		path1 = getPlayerIconPath(tips.succ_proto_id);
	}
	else
	{
		if (tips.succ_is_captain == 3)
		{
			path1 = getNpcIconPath(tips.succ_proto_id);
		}
		else
		{
			path1 = getCompanionIconPath(tips.succ_proto_id, tips.succ_is_captain);
		}
	}

	if (tips.fail_is_captain == 2)
	{
		path2 = getPlayerIconPath(tips.fail_proto_id);
	}
	else
	{
		if (tips.fail_is_captain == 3)
		{
			path2 = getNpcIconPath(tips.fail_proto_id);
		}
		else
		{
			path2 = getCompanionIconPath(tips.fail_proto_id, tips.fail_is_captain);
		}
	}

	switch (tips.fighting_type)
	{
	case TIP_SHIP_SUNK:
		p_shipSunk->setVisible(true);
		p_skill->setVisible(false);
		p_singled->setVisible(false);
		
		shipIconSuc->loadTexture(getShipIconPath(tips.succSid));
		shipIconFail->loadTexture(getShipIconPath(tips.failSid));
		shipNumSuc->loadTexture(getPositionIconPath(tips.succPos));
		shipNumFail->loadTexture(getPositionIconPath(tips.failPos));
		t_shipSunk->setString(SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_CENTER_TIP_DESTROYED"]);
		shipFail->setPositionX(t_shipSunk->getPositionX() - t_shipSunk->getBoundingBox().size.width / 2 - shipFail->getBoundingBox().size.width / 2 - 20);
		shipSuc->setPositionX(t_shipSunk->getPositionX() + t_shipSunk->getBoundingBox().size.width / 2 + shipSuc->getBoundingBox().size.width / 2 + 20);
		break;
	case TIP_SKILL:
		p_shipSunk->setVisible(false);
		p_skill->setVisible(true);
		p_singled->setVisible(false);
		i_ship->loadTexture(getShipIconPath(tips.succSid));
		i_position->loadTexture(getPositionIconPath(tips.succPos));
		t_skill->setString("NOT NOT NOT NOT");
		break;
	case TIP_SINGLED_START:
		p_shipSunk->setVisible(true);
		p_skill->setVisible(false);
		p_singled->setVisible(false);
		
		shipIconSuc->loadTexture(path2);
		shipIconFail->loadTexture(path1);
		shipNumSuc->loadTexture(getPositionIconPath(tips.failPos));
		shipNumFail->loadTexture(getPositionIconPath(tips.succPos));
		t_shipSunk->setString(SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_CENTER_TIP_SINGLED_START"]);
		shipFail->setPositionX(t_shipSunk->getPositionX() - t_shipSunk->getBoundingBox().size.width / 2 - shipFail->getBoundingBox().size.width / 2 - 20);
		shipSuc->setPositionX(t_shipSunk->getPositionX() + t_shipSunk->getBoundingBox().size.width / 2 + shipSuc->getBoundingBox().size.width / 2 + 20);
		break;
	case TIP_SINGLED_END:
		p_shipSunk->setVisible(true);
		p_skill->setVisible(false);
		p_singled->setVisible(false);

		shipIconSuc->loadTexture(path2);
		shipIconFail->loadTexture(path1);
		shipNumSuc->loadTexture(getPositionIconPath(tips.failPos));
		shipNumFail->loadTexture(getPositionIconPath(tips.succPos));
		if (tips.skillId == 1)
		{
			t_shipSunk->setString(SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_CENTER_TIP_SINGLED_SUCC"]);
		}
		else
		{
			t_shipSunk->setString(SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_CENTER_TIP_SINGLED_DARW"]);
		}
		shipFail->setPositionX(t_shipSunk->getPositionX() - t_shipSunk->getBoundingBox().size.width / 2 - shipFail->getBoundingBox().size.width / 2 - 20);
		shipSuc->setPositionX(t_shipSunk->getPositionX() + t_shipSunk->getBoundingBox().size.width / 2 + shipSuc->getBoundingBox().size.width / 2 + 20);
		break;
	default:
		break;
	}

	auto easeAct = MoveBy::create(2.f, Vec2(0, 280));
	p_centralTip_clone->setOpacity(0);
	auto spawn = Spawn::create(FadeIn::create(2.0f), easeAct, nullptr);
	auto seq = Sequence::create(spawn, DelayTime::create(1.0f), FadeOut::create(1.0f), CallFuncN::create([=](Ref *pSender){
		p_centralTip_clone->removeFromParent();
	}), nullptr);

	for (auto &child : p_centralTip_clone->getChildren().at(tips.fighting_type)->getChildren())
	{
		child->setCascadeOpacityEnabled(true);
	}
	p_centralTip_clone->setCascadeOpacityEnabled(true);
	p_centralTip_clone->runAction(seq);
	m_vTips.erase(m_vTips.begin());
}

void UIBattleHUD::initMainStrategyButton(int index,bool isSwitchAI)
{
	auto i_strategy = m_pFight->getChildByName<ImageView*>("image_strategy_bg");
	auto b_atk = i_strategy->getChildByName<Button*>("button_atk");
	auto i_atk_1 = b_atk->getChildByName<ImageView*>("image_atk_1");
	auto i_atk_2 = b_atk->getChildByName<ImageView*>("image_atk_2");
	auto b_guard = i_strategy->getChildByName<Button*>("button_guard");
	auto i_guard_1 = b_guard->getChildByName<ImageView*>("image_guard_1");
	auto i_guard_2 = b_guard->getChildByName<ImageView*>("image_guard_2");
	auto b_snipe = i_strategy->getChildByName<Button*>("button_snipe");
	auto i_snipe_1 = b_snipe->getChildByName<ImageView*>("image_snipe_1");
	auto i_snipe_2 = b_snipe->getChildByName<ImageView*>("image_snipe_2");
	auto t_strategy = i_strategy->getChildByName<Text*>("label_strategy");

	
	
	t_strategy->enableOutline(Color4B::BLACK, OUTLINE_MAX);


	auto infoPanelInstance = m_pFight->getChildByName<Layout*>("panel_destroyed");
	infoPanelInstance->setVisible(false);



	//auto image_captain_bg = battleInfo_panel->getChildByName<Layout*>("image_ship_bg");
	//image_captain_bg->setVisible(false);
	


	t_strategy->runAction(Sequence::create(FadeIn::create(1),DelayTime::create(3),FadeOut::create(1),nullptr));
	if (!isSwitchAI)
	{
		t_strategy->setString(SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_NOT_USE_AI"]);
		return;
	}

	b_atk->setBright(true);
	i_atk_1->setVisible(false);
	i_atk_2->setVisible(true);
	b_guard->setBright(true);
	i_guard_1->setVisible(false);
	i_guard_2->setVisible(true);
	b_snipe->setBright(true);
	i_snipe_1->setVisible(false);
	i_snipe_2->setVisible(true);
	t_strategy->setVisible(true);
	t_strategy->enableOutline(Color4B::BLACK,OUTLINE_MIN);

	switch (index)
	{
	case MODEL_FREE:
		{
			b_atk->setBright(false);
			i_atk_1->setVisible(true);
			i_atk_2->setVisible(false);
			t_strategy->setString(SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_MODEL_FREE"]);
			break;
		}
	case MODEL_PROTECT:
		{
			b_guard->setBright(false);
			i_guard_1->setVisible(true);
			i_guard_2->setVisible(false);
			t_strategy->setString(SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_MODEL_PROTECT"]);
			break;
		}
	case MODEL_FOCUS:
		{
			b_snipe->setBright(false);
			i_snipe_1->setVisible(true);
			i_snipe_2->setVisible(false);
			t_strategy->setString(SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_MODEL_FOCUS"]);
			break;
		}
	default:
		break;
	}
}

void UIBattleHUD::initMainSkillOrPorpButton()
{
	auto p_skill = m_pFight->getChildByName<Widget*>("panel_skill");
	auto p_prop = m_pFight->getChildByName<Widget*>("panel_prop");
	auto l_tip = m_pFight->getChildByName<Text*>("label_tip");
	bool bBattleSwitch = UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName("BATTLESWITCH").c_str(), true);
	if (bBattleSwitch)
	{
		p_skill->setVisible(true);
		p_prop->setVisible(false);
	}
	else
	{
		p_skill->setVisible(false);
		p_prop->setVisible(true);
	}
	
	if (l_tip != nullptr)
	{
		l_tip->setOpacity(0);
	}

	int repair_module_num = 0;
	for (int j = 0; j < m_pResult->n_props; j++)
	{
		if (m_pResult->props[j]->propid == REPAIR_MODULE_ID)
		{
			auto st_prop = StringUtils::format(PROP_NUM, REPAIR_MODULE_ID);
			if (m_pDelegate->getContinueLastFight())
			{
				auto hex = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(st_prop.c_str()).c_str());
				char *st_num = 0;
				//解密
				if (hex.length() > 0)
				{
					st_num = getStringFromEncryptedHex((char*)hex.c_str());
				}
				int propNum = 0;
				if (st_num)
				{
					propNum = atoi(st_num);
					free(st_num);
				}
				if (m_pResult->props[j]->num > propNum)
				{
					m_pResult->props[j]->num = propNum;
				}
			}
			else
			{
				//加密
				auto st_num = StringUtils::format("%d", m_pResult->props[j]->num);
				UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(st_prop.c_str()).c_str(), getEncryptedPasswordHex((char*)st_num.c_str()));
			}
			UserDefault::getInstance()->flush();
			repair_module_num = m_pResult->props[j]->num;
			break;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		std::string button_skill = StringUtils::format("button_skill_%d",i+1);
		std::string image_skill = StringUtils::format("image_skill_%d",i+1);
		std::string button_area = StringUtils::format("button_area_%d",i+1);
		std::string st_repair = StringUtils::format("text_item_skill_lv_%d", i + 1);
		auto b_skill = p_skill->getChildByName<Button*>(button_skill);
		auto i_skill = b_skill->getChildByName<ImageView*>(image_skill);
		auto b_area = b_skill->getChildByName<Button*>(button_area);
		auto t_repair = b_skill->getChildByName<Text*>(st_repair);
		m_pDelegate->getSkillManage()->pushAllSkillData(m_pResult);
		if (i < m_pResult->n_positiveskills)
		{
			int id = m_pResult->positiveskills[i]->id;
			b_area->setTag(i);
			b_area->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::skillButtonEvent,this));
			i_skill->ignoreContentAdaptWithSize(false);
			i_skill->loadTexture(getSkillIconPath(id, SKILL_TYPE_PLAYER));
			if (m_pResult->myships[0]->n_cannons < 1)
			{
				if (id == SKILL_POWWEFUL_SHOOT || id == SKILL_SALVO_SHOOT || id == SKILL_INCENDIARY_SHOOT || id == SKILL_HAMMER_SHOOT)
				{
					setGLProgramState(i_skill, true);
				}
				else
				{
					i_skill->setVisible(true);
				}
			}
			if (id == SKILL_PROGRESSIVE_REPAIR || id == SKILL_EMERGENT_REPAIR || id == SKILL_FLEET_REPAIR)
			{
				t_repair->setVisible(true);
				setTextSizeAndOutline(t_repair, repair_module_num);
				if (repair_module_num  < 1)
				{
					setGLProgramState(i_skill, true);
				}
			}
			else
			{
				t_repair->setVisible(false);
			}

			auto t_skill_time = b_skill->getChildByName<Text*>(StringUtils::format("text_time_%d", i + 1));
			auto i_cd = b_skill->getChildByName<ImageView*>(StringUtils::format("image_cd_%d", i + 1));
			auto st_skill = StringUtils::format(SKILL_NUM, id);
			if (m_pDelegate->getContinueLastFight())
			{
				m_gSkillTimeNum[i] = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_skill.c_str()).c_str(), 0);
				if (m_gSkillTimeNum[i] < 1)
				{
					m_gSkillTime[i] = false;
				}
				else
				{
					t_skill_time->setVisible(true);
					i_cd->setVisible(true);
					m_gSkillTime[i] = true;
					setTextSizeAndOutline(t_skill_time, m_gSkillTimeNum[i]);
				}
			}
			else
			{
				UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_skill.c_str()).c_str(), 0);
			}
		}else
		{
			b_skill->setTouchEnabled(false);
			i_skill->setVisible(false);
			t_repair->setVisible(false);
		}

		std::string button_prop = StringUtils::format("button_item_%d",i+1);
		std::string image_prop_bg = StringUtils::format("image_skill_%d",i+1);
		std::string image_prop = StringUtils::format("image_item_%d",i+1);
		std::string prop_num = StringUtils::format("text_item_skill_lv_%d",i+1);
		std::string prop_area = StringUtils::format("button_item_area_%d",i+1);
		auto b_prop = p_prop->getChildByName<Button*>(button_prop);
		auto prop_bg = b_prop->getChildByName<ImageView*>(image_prop_bg);
		
		int id = 0;
		int num = 0;
		switch (i)
		{
		case 0:
			{
				id = PROP_CROSS;
				break;
			}
		case 1:
			{
				id = PROP_FLEE_SMOKE;
				break;
			}
		case 2:
			{
				id = PROP_NIORD_HORN;
				break;
			}
		case 3:
			{
				id = PROP_WAR_DRUM;
				break;
			}
		default:
			break;
		}
		for (int j = 0; j < m_pResult->n_props; j++)
		{
			if (m_pResult->props[j]->propid == id)
			{
				auto st_prop = StringUtils::format(PROP_NUM, id);
				if (m_pDelegate->getContinueLastFight())
				{
					auto hex = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(st_prop.c_str()).c_str());
					char *st_num = 0;
					if (hex.length() > 0)
					{
						st_num = getStringFromEncryptedHex((char*)hex.c_str());
					}
					int propNum = 0;
					if (st_num)
					{
						propNum = atoi(st_num);
						free(st_num);
					}
					if (m_pResult->props[j]->num > propNum)
					{
						m_pResult->props[j]->num = propNum;
					}
				}
				else
				{
					//加密
					auto st_num = StringUtils::format("%d", m_pResult->props[j]->num);
					UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(st_prop.c_str()).c_str(), getEncryptedPasswordHex((char*)st_num.c_str()));
				}
				num = m_pResult->props[j]->num;
				break;
			}
		}
		UserDefault::getInstance()->flush();
		auto i_prop = prop_bg->getChildByName<ImageView*>(image_prop);
		auto t_num = prop_bg->getChildByName<Text*>(prop_num);
		auto bp_area = b_prop->getChildByName<Button*>(prop_area);
		i_prop->ignoreContentAdaptWithSize(false);
		i_prop->loadTexture(getItemIconPath(id));
		setTextSizeAndOutline(t_num,num);
		bp_area->setTag(i);
		bp_area->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::porpsButtonEvent, this));
		if (num < 1)
		{
			setGLProgramState(i_prop,true);
		}
	}
}

void UIBattleHUD::switchSkillAndPorp()
{
	auto p_skill = m_pFight->getChildByName<Widget*>("panel_skill");
	auto p_prop = m_pFight->getChildByName<Widget*>("panel_prop");
	p_prop->setVisible(p_skill->isVisible());
	p_skill->setVisible(!p_skill->isVisible());
}

void UIBattleHUD::onMainUI()
{
	int heroNums = m_pShareObj->m_vMyFriends.size();
	int enemyNums = m_pShareObj->m_vEnemys.size();

	for (int i = 0; i< heroNums; i++)
	{
		Button* friendFlag;
		if (m_pShareObj->m_vMyFriends.at(i)->getType() == MAIN_SHIP)
		{
			friendFlag = Button::create(MAINE_POS_FLAG, MAINE_POS_FLAG, MAINE_POS_FLAG);
		}
		else
		{
			friendFlag = Button::create(FRIEND_SHIP_POS_FLAG_BRI, FRIEND_SHIP_POS_FLAG, FRIEND_SHIP_POS_FLAG);
		}
		friendFlag->setAnchorPoint(Vec2(0.5f, 0.5f));
		auto t_flag = Text::create();
		t_flag->setString(StringUtils::format("%d", m_pShareObj->m_vMyFriends.at(i)->m_Info->_tag));
		t_flag->setTextColor(Color4B::WHITE);
		t_flag->setFontSize(14);
		t_flag->enableOutline(Color4B::BLACK, 2);
		t_flag->setAnchorPoint(Vec2(0.5f, 0.5f));
		friendFlag->addChild(t_flag, 1, "lable_flag");
		m_SmallMap->addChild(friendFlag);
		m_vHeroShipFlags.push_back(friendFlag);
		friendFlag->setPosition(Vec2(-2000, 0));
	}

	for (int i = 0; i < enemyNums; i++)
	{
		Button* enemyFlag = Button::create(ENEMY_SHIP_POS_FLAG_BRI, ENEMY_SHIP_POS_FLAG, ENEMY_SHIP_POS_FLAG);
		enemyFlag->setAnchorPoint(Vec2(0.5f, 0.5f));
		m_SmallMap->addChild(enemyFlag);
		auto t_flag = Text::create();
		t_flag->setString(StringUtils::format("%d", m_pShareObj->m_vEnemys.at(i)->m_Info->_tag - 5));
		t_flag->setTextColor(Color4B::WHITE);
		t_flag->setFontSize(14);
		t_flag->enableOutline(Color4B(157, 0, 0, 255), 2);
		t_flag->setAnchorPoint(Vec2(0.5f, 0.5f));
		enemyFlag->addChild(t_flag, 1, "lable_flag");
		m_vEnemyShipFlags.push_back(enemyFlag);
		enemyFlag->setPosition(Vec2(-2000, 0));
	}
}

void UIBattleHUD::updateMapPosition()
{
	auto &enemys = m_pShareObj->m_vEnemys;
	auto &friends = m_pShareObj->m_vMyFriends;
	Size winSize = Director::getInstance()->getWinSize();
	Size fakeSize = m_SmallMap->getContentSize();


	float rx = fakeSize.width*1920/ winSize.width;
	float ry = fakeSize.height*1080/ winSize.height; //d == ry
	float radarR = ry/2;
	bool hasMainShip = false;
	Vec2 radarCenterPosition;
	//float radarDistance = winSize.width*1.0;
	float radarDistance = winSize.height*1.0;

	for (auto i = 0; i < friends.size() ;i++)
	{
		if(friends[i]->getType() == MAIN_SHIP){
			hasMainShip = true;
			radarCenterPosition = friends[i]->getPosition();
			if(radarCenterPosition.x < winSize.width/2){
				radarCenterPosition.x = winSize.width/2;
			}else if(radarCenterPosition.x > winSize.width*2.5){
				radarCenterPosition.x = winSize.width*2.5;
			}

			if(radarCenterPosition.y < winSize.height/2){
				radarCenterPosition.y = winSize.height/2;
			}else if(radarCenterPosition.y > winSize.height*2.5){
				radarCenterPosition.y = winSize.height*2.5;
			}

			break;
		}
	}
	if (!hasMainShip)
	{
		Camera  *c = Director::getInstance()->getRunningScene()->getCameras().front();
		radarCenterPosition = c->getPosition() + winSize;
		if (radarCenterPosition.x < winSize.width / 2)
		{
			radarCenterPosition.x = winSize.width / 2;
		}
		else if (radarCenterPosition.x > winSize.width*2.5)
		{
			radarCenterPosition.x = winSize.width*2.5;
		}

		if (radarCenterPosition.y < winSize.height / 2)
		{
			radarCenterPosition.y = winSize.height / 2;
		}
		else if (radarCenterPosition.y > winSize.height*2.5)
		{
			radarCenterPosition.y = winSize.height*2.5;
		}
	}

	for (auto i = 0; i < enemys.size() ;i++)
	{
		auto pos = enemys[i]->getPosition();
		float dis = radarCenterPosition.distance(pos);
		float factor = dis/radarDistance;
		if(factor <=1){
			if(dis < 0.0001){
				m_vHeroShipFlags[i]->setPosition(Vec2(rx,ry));
			}else{
				float newX = (pos.x - radarCenterPosition.x)/dis*factor*radarR+rx;
				float newY = (pos.y - radarCenterPosition.y)/dis*factor*radarR+ry;
				m_vEnemyShipFlags[i]->setPosition(Vec2(newX,newY));
			}
			m_vEnemyShipFlags[i]->setBright(true);
		}else{
			m_vEnemyShipFlags[i]->setBright(false);
			auto directPos = pos - radarCenterPosition;
			directPos.normalize();
			m_vEnemyShipFlags[i]->setPosition( directPos * ry / 2 + Vec2(rx,ry));
			m_vEnemyShipFlags[i]->setRotation(-90.f - CC_RADIANS_TO_DEGREES(atan2f(directPos.y,directPos.x)));
			m_vEnemyShipFlags[i]->getChildByName<Text*>("lable_flag")->setRotation(90.f + CC_RADIANS_TO_DEGREES(atan2f(directPos.y, directPos.x)));
		}
	}

	for (auto i = 0; i < friends.size() ;i++)
	{
		auto ship = friends[i];
//		if(ship->getType() == MAIN_SHIP){
//			m_vHeroShipFlags[i]->setBright(true);
//			m_vHeroShipFlags[i]->setPosition(Vec2(rx,ry));
//			//continue;
//		}
		auto pos = friends[i]->getPosition();
		float dis = radarCenterPosition.distance(pos);
		float factor = dis/radarDistance;
		if(factor <=1){
			if(dis < 0.0001){
				m_vHeroShipFlags[i]->setPosition(Vec2(rx,ry));
			}else{
				float newX = (pos.x - radarCenterPosition.x)/dis*factor*radarR+rx;
				float newY = (pos.y - radarCenterPosition.y)/dis*factor*radarR+ry;
				m_vHeroShipFlags[i]->setPosition(Vec2(newX,newY));
			}
			m_vHeroShipFlags[i]->setBright(true);
		}else{
			m_vHeroShipFlags[i]->setBright(false);
			auto directPos = pos - radarCenterPosition;
			directPos.normalize();
			m_vHeroShipFlags[i]->setPosition( directPos * ry / 2 + Vec2(rx,ry));
			m_vHeroShipFlags[i]->setRotation(-90.f - CC_RADIANS_TO_DEGREES(atan2f(directPos.y,directPos.x)));
			m_vHeroShipFlags[i]->getChildByName<Text*>("lable_flag")->setRotation(90.f + CC_RADIANS_TO_DEGREES(atan2f(directPos.y, directPos.x)));
		}
	}

	if (m_vEnemyShipFlags.size() > enemys.size() && m_vEnemyShipFlags.size() > 0)
	{
		auto lastEnemy = m_vEnemyShipFlags[m_vEnemyShipFlags.size() - 1];
		m_vEnemyShipFlags.erase(m_vEnemyShipFlags.end() - 1);
		lastEnemy->removeFromParent();

		for (size_t i = 0; i < m_vEnemyShipFlags.size(); i++)
		{
			if (i < m_pShareObj->m_vEnemys.size() && i < m_vEnemyShipFlags.size())
			{
				m_vEnemyShipFlags[i]->getChildByName<Text*>("lable_flag")->setString(StringUtils::format("%d", m_pShareObj->m_vEnemys.at(i)->m_Info->_tag - 5));
			}
		}
	}
	if (m_vHeroShipFlags.size() > friends.size() && m_vHeroShipFlags.size() > 0)
	{
		if (friends.size() > 0 && friends[0]->getType() == MAIN_SHIP)
		{
			auto lastHero = m_vHeroShipFlags[m_vHeroShipFlags.size() - 1];
			m_vHeroShipFlags.erase(m_vHeroShipFlags.end() - 1);
			lastHero->removeFromParent();
		}else
		{
			auto lastHero = m_vHeroShipFlags[0];
			m_vHeroShipFlags.erase(m_vHeroShipFlags.begin());
			lastHero->removeFromParent();
		}

		for (size_t i = 0; i < m_vHeroShipFlags.size(); i++)
		{
			if (i < m_pShareObj->m_vMyFriends.size() && i < m_vHeroShipFlags.size())
			{
				m_vHeroShipFlags[i]->getChildByName<Text*>("lable_flag")->setString(StringUtils::format("%d", m_pShareObj->m_vMyFriends.at(i)->m_Info->_tag));
			}
		}
	}
}

void UIBattleHUD::startGame()
{
	m_bIsBattleEnd = false;
	this->scheduleUpdate();
}

void UIBattleHUD::stopGame()
{
	m_bIsBattleEnd = true;
	this->unscheduleUpdate();
}

void UIBattleHUD::stopGameByProps()
{
	m_bIsBattleEnd = true;
	sendDataToServer(BATTLE_FLEE);
}

void UIBattleHUD::specialButtonEvent(Node* target,std::string name,float varValue)
{

}

void UIBattleHUD::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		if (target)
		{
			buttonEvent(target,target->getName());
		}
	}
}

void UIBattleHUD::buttonEvent(Widget* target,std::string name)
{
	//花费V票
	if (isButton(button_v_yes))
	{
		if (m_pResult->saveshipcost <= SINGLE_HERO->m_iGold)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			ProtocolThread::GetInstance()->endFailedFightByVTicket(1, FIGHT_TYPE_NORMAL, UILoadingIndicator::createWithMask(this, 4));
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		}
		return;
	}

	if (isButton(button_v_no))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bIsTouch==true)
		{

			closeView();
			m_bIsSendToData = true;
			sendDataToServer(BATTLE_DEFEATED);
			m_bIsTouch = false;
		}
		else
		{
			m_bIsTouch = false;
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYesOrNO("TIP_RANSOM", "ARE_YOU_OK");
			
		}
		return;
	}

	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_confirm_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bIsTrue)
		{
			closeView();
			m_bIsSendToData = true;
			sendDataToServer(BATTLE_DEFEATED);
		}
		else
		{
		m_pDelegate->callbackEventByIndex(0);
}
		return;
	}
	if (isButton(button_continue_1) || isButton(button_skip))
	{
		if (!isButton(button_skip))
		{
			closeView();
		}
		
		if (m_bIsLevelUp)
		{
			m_bIsLevelUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPlayerLevelUp();
			return;
		}
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
			return;
		}
		if (m_bIsPrestigeDown)
		{
			m_bIsPrestigeDown = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelDown();
			return;
		}
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			if (!m_pEndFightResult) return;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp(CAPTAIN_EXPUI_BATTLE);
			UICommon::getInstance()->flushCaptainLevelUp(m_pEndFightResult->n_captains,m_pEndFightResult->captains);
			return;
		}
		flushBattleResult2(m_pEndFightResult->reason,m_pEndFightResult);
		return;
	}
	if (isButton(button_level_ok) || isButton(panel_levelup))
	{  
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
			return;
		}
		if (m_bIsPrestigeDown)
		{
			m_bIsPrestigeDown = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelDown();
			return;
		}
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			if (!m_pEndFightResult) return;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp(CAPTAIN_EXPUI_BATTLE);
			UICommon::getInstance()->flushCaptainLevelUp(m_pEndFightResult->n_captains,m_pEndFightResult->captains);
			return;
		}
		flushBattleResult2(m_pEndFightResult->reason,m_pEndFightResult);
		return;
	}
	if (isButton(panel_r_levelup) ||isButton(panel_r_leveldown))
	{
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			if (!m_pEndFightResult) return;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp();
			UICommon::getInstance()->flushCaptainLevelUp(m_pEndFightResult->n_captains,m_pEndFightResult->captains);
			return;
		}
		flushBattleResult2(m_pEndFightResult->reason,m_pEndFightResult);
		return;
	}
	if ( isButton(panel_captain_addexp))
	{
		if (!m_pEndFightResult) return;
		flushBattleResult2(m_pEndFightResult->reason,m_pEndFightResult);
		return;
	}
	if (isButton(button_continue_2))
	{
		if (m_bIsFailed)
		{
			closeView(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_2_CSB]);
			if (m_pResult->fighttype == FIGHT_TYPE_ROB)
			{
				openDialogFaild(-1);
			}
			else
			{
				openDialogFaild(m_pEndFightResult->lostcoin);
			}
		}else
		{
			if (m_pEndFightResult->fighttype == FIGHT_TYPE_ROB){
				if(m_pEndFightResult->shipx == 0){
					CHANGETO(SCENE_TAG::MAIN_TAG);
				}else{
					CHANGETO(SCENE_TAG::MAP_SAILING_TAG);
				}
			}else{
				m_pDelegate->callbackEventByIndex(0);
			}
		}
		return;
	}
	if (isButton(Panel_faildialog_root))
	{
		if (m_bDiaolgOver)
		{

			ProtocolThread::GetInstance()->reachCity(-1, -1, UILoadingIndicator::createWithMask(this, 4));
		}
		else
		{
			chatTxt->setString(chatContent);
			this->unschedule(schedule_selector(UIBattleHUD::showWarFailedText));
			m_bDiaolgOver = true;
			anchPic->setVisible(true);
		}
		return;
	}

	if (isButton(button_close))
	{
		closeView();
		return;
	}
	if (isButton(button_back))
	{
		ProtocolThread::GetInstance()->cancelFight(0);
		button_callBack();
		return;
	}
	if (isButton(button_fail_ok))
	{
		button_callBack();
		return;
	}
	if (isButton(button_ok))
	{
		closeView();
		m_pDelegate->callbackEventByIndex(0);
		return;
	}
	//更改AI模式
	if (isButton(button_atk) || isButton(button_guard) || isButton(button_snipe))
	{
		initMainStrategyButton(target->getTag(),m_bIsSwitchAI);
		m_pDelegate->callbackEventByIndex(target->getTag());
		return;
	}
	//技能、道具切换
	if (isButton(button_switch))
	{
		bool bBattleSwitch = UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName("BATTLESWITCH").c_str(), true);
		UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName("BATTLESWITCH").c_str(), !bBattleSwitch);
		UserDefault::getInstance()->flush();
		switchSkillAndPorp();
		return;
	}
	if (isButton(button_error_yes))//战斗数据异常
	{
		if (!m_bIsTouch)
		{
			switch (m_pResult->fighttype)
			{
			case FIGHT_TYPE_NORMAL:
			case FIGHT_TYPE_ROB:
				m_pDelegate->callbackEventByIndex(0);
				break;
			case FIGHT_TYPE_TASK:
				m_pDelegate->callbackEventByIndex(0);
				break;
			case FIGHT_TYPE_PIRATE_ATTACK:
				ProtocolThread::GetInstance()->getPirateAttackInfo(UILoadingIndicator::createWithMask(this, 4));
				break;
			case FIGHT_TYPE_ATTACK_PIRATE:
				ProtocolThread::GetInstance()->getAttackPirateInfo(SINGLE_HERO->m_nBossEventId, UILoadingIndicator::createWithMask(this, 4));
				break;
			default:
				break;
			}
		}
		return;
	}

	if (isButton(button_continue_fight))
	{
		closeView();
		ProtocolThread::GetInstance()->engageInFight(-1, -1, -1, m_pResult->fighttype, m_pResult->fightlevel, UILoadingIndicator::createWithMask(this, 4));
		return;
	}

	if (isButton(button_leave))
	{
		closeView();
		ProtocolThread::GetInstance()->finalMyExploitScore(UILoadingIndicator::createWithMask(this, 4));
		return;
	}
	if (isButton(button_result_yes))
	{
		UICommon::getInstance()->closeView();
		ProtocolThread::GetInstance()->getPirateAttackInfo(UILoadingIndicator::createWithMask(this, 4));
		return;
	}
	//海盗基地
	if (isButton(button_yes))
	{
		ProtocolThread::GetInstance()->getAttackPirateInfo(SINGLE_HERO->m_nBossEventId, UILoadingIndicator::createWithMask(this, 4));
		return;
	}

	if (isButton(image_skill_bg_2_2))
	{
		auto t_skill = target->getChildByName<Text*>("text_item_skill_lv");
		UICommon::getInstance()->openCommonView(this);
		SKILL_DEFINE skillDefine;
		skillDefine.id = target->getTag();
		skillDefine.lv = atoi(t_skill->getString().c_str());
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}
}

void UIBattleHUD::porpsButtonEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		if (m_bIsBattleEnd)
		{
			return;
		}
		if (!m_bIsSendToData)
		{
			return;
		}
		if (m_pDelegate->getShareObj()->m_vMyFriends.empty())
		{
			return;
		}

		TVBattleMainShip* ship = dynamic_cast<TVBattleMainShip*>(m_pDelegate->getShareObj()->m_vMyFriends[0]);

		Widget* target = dynamic_cast<Widget*>(pSender);
		int tag = target->getTag();
		if (!ship && tag !=1)
		{
			return;
		}

		int id = 0;
		int num = 0;
		switch (tag)
		{
			case 0:
			{
				id = PROP_CROSS;
				break;
			}
			case 1:
			{
				id = PROP_FLEE_SMOKE;
				break;
			}
			case 2:
			{
				id = PROP_NIORD_HORN;
				break;
			}
			case 3:
			{
				id = PROP_WAR_DRUM;
				break;
			}
			default:
				break;
		}
		for (int i = 0; i < m_pResult->n_props; i++)
		{
			if (m_pResult->props[i]->propid == id && m_pResult->props[i]->num > 0)
			{
				num = m_pResult->props[i]->num;
				m_pResult->props[i]->num--;
				auto st_prop = StringUtils::format(PROP_NUM, id);
				auto st_num = StringUtils::format("%d", m_pResult->props[i]->num);
				UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(st_prop.c_str()).c_str(), getEncryptedPasswordHex((char*)st_num.c_str()));
				UserDefault::getInstance()->flush();
				break;
			}
		}
		auto p_prop = m_pFight->getChildByName<Widget*>("panel_prop");
		std::string str;
		if (num < 1)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			str = SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_NOT_PROP"];
		}
		else
		{
			if (num == 1)
			{
				auto i_prop = Helper::seekWidgetByName(p_prop, StringUtils::format("image_item_%d", tag + 1));
				setGLProgramState(i_prop, true);
			}
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_USED_PROP_21);
			auto t_num = target->getParent()->getChildByName(StringUtils::format("image_skill_%d", tag + 1))->getChildByName<Text*>(StringUtils::format("text_item_skill_lv_%d", tag + 1));
			setTextSizeAndOutline(t_num, --num);
			m_pDelegate->usedProps(id);
			str = getItemName(id);
			str += ":-1";
		}
		auto t_tip = m_pFight->getChildByName<Text*>("label_tip");
		t_tip->setString(str);
		t_tip->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(1), FadeOut::create(0.5), nullptr));
	}
}

void UIBattleHUD::skillButtonEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		if (m_bIsBattleEnd)
		{
			return;
		}
		Widget* target = dynamic_cast<Widget*>(pSender);
		int tag = target->getTag();
		if (m_gSkillTime[tag])
		{
			return;
		}
		//m_pDelegate->stopBattle();//使用技能不在暂停
		int id = m_pResult->positiveskills[tag]->id;
		m_nSkillID = id;
		if (id == SKILL_PROGRESSIVE_REPAIR || id == SKILL_EMERGENT_REPAIR || id == SKILL_FLEET_REPAIR)
		{
			//useSkillEffect(id, true);
			bool isRepair = false;
			for (int j = 0; j < m_pResult->n_props; j++)
			{
//				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				if (m_pResult->props[j]->propid == REPAIR_MODULE_ID)
				{
					isRepair = true;
					if (m_pResult->props[j]->num < 1)
					{
						isRepair = false;
						break;
					}
					else
					{
						m_pResult->props[j]->num--;
						for (size_t i = 0; i < 4; i++)
						{
							auto p_skill = m_pFight->getChildByName<Widget*>("panel_skill");
							std::string button_skill = StringUtils::format("button_skill_%d", i + 1);
							std::string image_skill = StringUtils::format("image_skill_%d", i + 1);
							std::string button_area = StringUtils::format("button_area_%d", i + 1);
							std::string st_repair = StringUtils::format("text_item_skill_lv_%d", i + 1);
							auto b_skill = p_skill->getChildByName<Button*>(button_skill);
							auto i_skill = b_skill->getChildByName<ImageView*>(image_skill);
							auto b_area = b_skill->getChildByName<Button*>(button_area);
							auto t_repair = b_skill->getChildByName<Text*>(st_repair);

							if (i < m_pResult->n_positiveskills)
							{
								int id = m_pResult->positiveskills[i]->id;
								i_skill->setVisible(true);
								
								if (id == SKILL_PROGRESSIVE_REPAIR || id == SKILL_EMERGENT_REPAIR || id == SKILL_FLEET_REPAIR)
								{
									t_repair->setVisible(true);
									setTextSizeAndOutline(t_repair, m_pResult->props[j]->num);
									if (m_pResult->props[j]->num < 1)
									{
										setGLProgramState(i_skill, true);
									}
								}
							}
						}
					
						auto st_prop = StringUtils::format(PROP_NUM, REPAIR_MODULE_ID);
						auto st_num = StringUtils::format("%d", m_pResult->props[j]->num);
						UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(st_prop.c_str()).c_str(), getEncryptedPasswordHex((char*)st_num.c_str()));
						UserDefault::getInstance()->flush();
					}
					break;
				}
			}
			if (!isRepair)
			{
				std::string content = SINGLE_SHOP->getTipsInfo()["TIP_COMBAT_REPAIR_MODULE"];
				std::string old_value = "[item]";
				std::string new_value = getItemName(REPAIR_MODULE_ID);
				repalce_all_ditinct(content, old_value, new_value);
				auto t_tip = m_pFight->getChildByName<Text*>("label_tip");
				t_tip->setString(new_value);
				t_tip->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(1), FadeOut::create(0.5), nullptr));
				return;
			}
		}
		else
		{
			//useSkillEffect(id, false);
			if (m_nSkillID == SKILL_POWWEFUL_SHOOT || m_nSkillID == SKILL_SALVO_SHOOT || m_nSkillID == SKILL_INCENDIARY_SHOOT || m_nSkillID == SKILL_HAMMER_SHOOT)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CONNON_LEAD);
			}
			else if (m_nSkillID == SKILL_ATTACKING_HORN)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUFF_BUGLE);
			}
			else if (m_nSkillID == SKILL_PROGRESSIVE_REPAIR)
			{

				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BLOOD_RETURN);
			}
		}
		auto skill = SINGLE_SHOP->getSkillTrees().find(id)->second;
		m_gSkillTimeNum[tag] = skill.cd - skill.cdreduce_per_lv*m_pResult->positiveskills[tag]->level;
		m_gSkillTime[tag] = true;
		m_pDelegate->getSkillManage()->setSkillUsed(tag, true);
		m_pDelegate->usedSkills(m_nSkillID);
	}
}

void UIBattleHUD::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_ReachCityResult:
		{
			button_callBack();
			break;
		}
	case PROTO_TYPE_EndFightResult:
		{
			EndFightResult* result = (EndFightResult*)message;
			m_pEndFightResult = result;
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
			for (size_t i = 1; i <= 10; i++)
			{
				auto capturing_ship = StringUtils::format(CAPTURINGSHIPID, i);
				auto st_ship = StringUtils::format("%d", 0);
				UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(capturing_ship.c_str()).c_str(), getEncryptedPasswordHex((char*)st_ship.c_str()));
				auto food_plunder = StringUtils::format(FOODPLUNDERSHIPID, i);
				auto st_ship2 = StringUtils::format("%d", 0);
				UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(food_plunder.c_str()).c_str(), getEncryptedPasswordHex((char*)st_ship2.c_str()));
			}
			for (size_t i = 0; i < m_pResult->n_positiveskills; i++)
			{
				auto st_skill = StringUtils::format(SKILL_NUM, m_pResult->positiveskills[i]->id);
				UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_skill.c_str()).c_str(), 0);
			}
			UserDefault::getInstance()->flush();
			//未完成战斗结束后打开离线委托界面
			if (SINGLE_HERO->m_lastBattleNotComplete)
			{
				SINGLE_HERO->m_lastBattleNotComplete = false;
				SINGLE_HERO->m_isShowOutLineResult = true;
			}
			SINGLE_HERO->m_iExp = result->totalexp;
			SINGLE_HERO->m_iRexp = result->totalfame;
			//最近离开的有许可证的城市
			SINGLE_HERO->m_iHave_licence_cityId = result->last_safe_city;
			setShipEquipBroken();//沉船的装备损坏
			SINGLE_AUDIO->vpauseBGMusic();

			//0:正常战斗结果， 1：任务战斗  2：海盗攻城 3:攻击海盗基地 4:打劫或者复仇
			switch (m_pEndFightResult->fighttype)
			{
			case FIGHT_TYPE_NORMAL:
			case FIGHT_TYPE_ROB:
				break;
			case FIGHT_TYPE_TASK:
			{
				if (result->failed != 0)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_SERVER_DATA_FIAL");
					return;
				}
				if (CompanionTaskManager::GetInstance()->m_PartsInfo && CompanionTaskManager::GetInstance()->m_PartsInfo->npc_id)
				{
					if ((CompanionTaskManager::GetInstance()->getCurrentTaskId() == 5 && CompanionTaskManager::GetInstance()->getCurrentPartId() == 2) || 
						(CompanionTaskManager::GetInstance()->getCurrentTaskId() == 7 && CompanionTaskManager::GetInstance()->getCurrentPartId() == 2))
					{
						
					}
					else
					{
						CompanionTaskManager::GetInstance()->completeCompanionTask();
						if (CompanionTaskManager::GetInstance()->getCurrentTaskId() == 9 && CompanionTaskManager::GetInstance()->getCurrentPartId() == 1)
						{
							return;
						}
					}
				}
				if (result->reason == BATTLE_WIN)
				{
					if (MainTaskManager::GetInstance()->m_PartsInfo && MainTaskManager::GetInstance()->m_PartsInfo->has_npc)
					{
						MainTaskManager::GetInstance()->completeMainTask();
						return;
					}
					else if (CompanionTaskManager::GetInstance()->getCurrentTaskId() == 7 && CompanionTaskManager::GetInstance()->getCurrentPartId() == 2)
					{
						CompanionTaskManager::GetInstance()->completeCompanionTask();
						return;
					}
				}
				else
				{
					if (CompanionTaskManager::GetInstance()->getCurrentTaskId() == 5 && CompanionTaskManager::GetInstance()->getCurrentPartId() == 2)
					{
						CompanionTaskManager::GetInstance()->completeCompanionTask();
					}
					else
					{
						openDialogFaild();
					}
					return;
				}
				break;
			}
			case FIGHT_TYPE_PIRATE_ATTACK:
			{	
				if (result->failed != 0)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_SERVER_DATA_FIAL");
				}
				{
					openViewPirateAttack();
				}
				return;
			}
			case FIGHT_TYPE_ATTACK_PIRATE:
				if (result->failed != 0)
				{
					//攻击海盗基地--海盗已经死
					if (result->failed == 15)
					{
						UIInform::getInstance()->openInformView(this);
						UIInform::getInstance()->openConfirmYes("TIP_ATTACT_PIRATE_HAS_BEEN_BREACHED");
					}
					else
					{
						UIInform::getInstance()->openInformView(this);
						UIInform::getInstance()->openConfirmYes("TIP_SERVER_DATA_FIAL");
					}
				}
				else
				{
					openViewAttackPirate();
				}
				return;
			default:
				break;
			}

			switch (result->reason)
			{
			case 0:
			case BATTLE_WIN:
				{
					SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BATTLE_WIN_27);

					/*提交log以后才出现战斗结算界面*/
					//onWin(result->reason,result);
					/*比较特殊，赢了才向服务器写log*/
					//新手引导数据，不用提交
					if (ProtocolThread::GetInstance()->isInTutorial())
					{
						flushBattleResult1Novice();
					}
					else
					{
						WriterLog::getInstance()->addGameOverStates(result);
						WriterLog::getInstance()->setCallbackHttp(CC_CALLBACK_2(UIBattleHUD::onHttpRequestCompleted, this));
						WriterLog::getInstance()->sendDataToServer();
						m_pWaitForLogUploading = UILoadingIndicator::create(this);
						this->addChild(m_pWaitForLogUploading, 1000);
					}		
					
					this->setCameraMask(4);
					break;
				}
			case BATTLE_DEFEATED:
				{
					SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BATTLE_FAIL_28);
					auto &ships = m_pDelegate->getShareObj()->m_vMyFriends;
					for (auto iter = ships.begin();iter != ships.end();iter++)
					{
						(*iter)->removeFromParent();
					}
					m_bIsFailed = true;
					ships.clear();
					onWin(result->reason,result);
					break;
				}
			
			case BATTLE_DRAW:
			case BATTLE_FLEE:
				{
					SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BATTLE_FAIL_28);
					onWin(result->reason,result);
					break;
				}
			case 5:
				{
					closeView();
					openDialogFaild(-1);
					break;
				}
			default:
				break;
			}
			break;
		}
	case PROTO_TYPE_NotifyBattleLogResult:
		{
			NotifyBattleLogResult* result = (NotifyBattleLogResult*)message;
			if (result->failed == 0)
			{
				if(!m_pEndFightResult) return;
				onWin(m_pEndFightResult->reason,m_pEndFightResult);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_BATTLEUI_BATTLE_LOG_FAIL");
			}
			break;
		}
	case PROTO_TYPE_EngageInFightResult:
	{
		EngageInFightResult*result = (EngageInFightResult*)message;
		if (result->failed == 0)
		{
			SINGLE_AUDIO->setMusicType();
			SINGLE_HERO->m_pBattelData = result;
			SINGLE_HERO->shippos = m_pDelegate->getShipPos();
			ProtocolThread::GetInstance()->forceFightStatus();
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			CHANGETO(SCENE_TAG::BATTLE_TAG);
		}
		else if (result->failed == 5)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_ACTIVITY_PIRATE_ATTACK_NOT_SHIP");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_ACTIVITY_PIRATE_ATTACK_NOT_SAILOR");
		}
		break;
	}
	case PROTO_TYPE_FinalMyExploitScoreResult:
	{
		FinalMyExploitScoreResult *result = (FinalMyExploitScoreResult*)message;
		if (result->failed == 0)
		{
			if (result->cantaken)
			{
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushPlayerAddExpOrFrame(result->exp, result->fame, result->addexp, result->addfame);

				openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
				auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
				auto t_title = view->getChildByName<Text*>("label_title");
				auto p_result = view->getChildByName<Widget*>("panel_result");
				auto l_result = p_result->getChildByName<ListView*>("listview_result");

				auto p_silver = p_result->getChildByName<Widget*>("panel_silver");
				auto i_div = p_result->getChildByName<ImageView*>("image_div_1");
				t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_REWARD_TITLE"]);

				auto p_item_clone = p_silver->clone();
				auto t_item_title = p_item_clone->getChildByName<Text*>("label_force_relation");
				auto t_num = p_item_clone->getChildByName<Text*>("label_buy_num");
				std::string itemName;
				std::string itemPath;
				t_item_title->setString(SINGLE_SHOP->getSpecialsInfo().find(10001)->second.name);
				t_num->setTextColor(Color4B(46, 125, 50, 255));
				t_num->setString(StringUtils::format("+ %lld", result->addsailorcoins));
				l_result->pushBackCustomItem(p_item_clone);
				auto i_div1 = i_div->clone();
				l_result->pushBackCustomItem(i_div1);
				view->setCameraMask(4);
			}
			else
			{
				ProtocolThread::GetInstance()->getPirateAttackInfo(UILoadingIndicator::createWithMask(this, 4));
			}
		}
		break;
	}
	case  PROTO_TYPE_GetAttackPirateInfoResult:
	{
		GetAttackPirateInfoResult *result = (GetAttackPirateInfoResult *)message;
		if (result->failed == 0)
		{
			UICommon::getInstance()->closeView();
			UIPriate*pirateLayer = new UIPriate();
			Scene* scene = Scene::createWithPhysics();
			scene->addChild(pirateLayer);
			pirateLayer->init(result, nullptr);
			Director::getInstance()->replaceScene(scene);
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_ACTIVITY_NOT_STARTE");
		}
		break;
	}
	case PROTO_TYPE_GetPirateAttackInfoResult:
	{
		GetPirateAttackInfoResult *result = (GetPirateAttackInfoResult *)message;
		if (result->failed == 0)
		{
			UIPriate*pirateLayer = new UIPriate();
			Scene* scene = Scene::createWithPhysics();
			scene->addChild(pirateLayer);
			pirateLayer->init(nullptr, result);
			Director::getInstance()->replaceScene(scene);
		}
		else
		{
			log("GetPirateAttackInfoResult Data fail !!!");
			button_callBack();
		}
		break;
	}
	case PROTO_TYPE_SaveFailedFightResult:
		{
			auto result = (SaveFailedFightResult *)message;
			if (result->failed == 0 ||m_bIsTouch==0)
			{
					openView(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
					auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
					auto t_title = view->getChildByName<Text*>("label_advanced_repair");
					auto t_content = view->getChildByName<Text*>("label_repair_content");
					auto lv_costNum = view->getChildByName<ListView*>("listview_cost");
					auto w_costNum = lv_costNum->getItem(1);
					auto t_costNum = w_costNum->getChildByName<Text*>("label_cost_num");
					std::string s_cost = numSegment(StringUtils::format("%d", m_pResult->saveshipcost));
					t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_SAVE_SHIP_TITLE"]);
					t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_SAVE_SHIP_CONTENT"]);
					t_costNum->setString(s_cost);
					auto image_silver_1 = w_costNum->getChildByName<ImageView*>("image_silver_1");
					image_silver_1->setPositionX(t_costNum->getPositionX() -
						t_costNum->getBoundingBox().size.width - image_silver_1->getBoundingBox().size.width * 0.7);
	
			}
			else
			{
				ProtocolThread::GetInstance()->saveFailedFight(UILoadingIndicator::createWithMask(this, 4));
			}
			break;
		}
	case PROTO_TYPE_EndFailedFightByVTicketResult:
		{
			auto result = (EndFailedFightByVTicketResult *)message;
			if (result->failed == 0)
			{
				SINGLE_HERO->m_iHave_licence_cityId = result->last_safe_city;
				if (m_pResult->fighttype != FIGHT_TYPE_ROB)
				{
					Utils::consumeVTicket("16", 1, result->usevticket);
					closeView();
					openDialogFaild(-1);
				}
				else
				{
					m_bIsSendToData = true;
					sendDataToServer(BATTLE_DEFEATED);
				}
			}
			else
			{
				//ProtocolThread::GetInstance()->endFailedFightByVTicket(1, m_pResult->fighttype, LoadingLayer::createWithMask(this, 4));
			}
			break;
		}
	default:
		break;
	}
}

void UIBattleHUD::update(float delta)
{
	if (m_bIsBattleEnd)
	{
		for (auto iter = m_vEnemyShipFlags.begin(); iter != m_vEnemyShipFlags.end();iter++)
		{
			(*iter)->setVisible(false);
		}
		for (auto iter = m_vHeroShipFlags.begin(); iter != m_vHeroShipFlags.end();iter++)
		{
			(*iter)->setVisible(false);
		}
		this->unscheduleUpdate();
		return;
	}
	updateCount(delta);
	updateMapPosition();
	checkIsWin(delta);
	updateSkillTime(delta);
	if (m_nTipIntervalTime <= 0)
	{
		showFightingCentralTips();
	}
	else
	{
		m_nTipIntervalTime -= delta;
	}
}

void UIBattleHUD::updateSkillTime(float time)
{
	for (int i = 0; i < 4; i++)
	{
		if (m_gSkillTime[i])
		{
			m_gPerSkillTimeNum[i] += time;
			if (m_gPerSkillTimeNum[i] >= 1)
			{
				m_gPerSkillTimeNum[i] = 0;
				auto p_skill = m_pFight->getChildByName<Widget*>("panel_skill");
				auto b_skill = p_skill->getChildByName<Button*>(StringUtils::format("button_skill_%d",i+1));
				auto t_skill_time = b_skill->getChildByName<Text*>(StringUtils::format("text_time_%d", i + 1));
				auto i_cd = b_skill->getChildByName<ImageView*>(StringUtils::format("image_cd_%d",i+1));
				t_skill_time->setVisible(true);
				i_cd->setVisible(true);
				setTextSizeAndOutline(t_skill_time, m_gSkillTimeNum[i]);
				if (m_gSkillTimeNum[i] < 1)
				{
					t_skill_time->setVisible(false);
					i_cd->setVisible(false);
					m_gSkillTime[i] = false;
					m_gPerSkillTimeNum[i] = 1;
				}
				m_gSkillTimeNum[i]--;
				if (i < m_pResult->n_positiveskills)
				{
					auto st_skill = StringUtils::format(SKILL_NUM, m_pResult->positiveskills[i]->id);
					UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_skill.c_str()).c_str(), m_gSkillTimeNum[i]);
					UserDefault::getInstance()->flush();
				}
			} 
		}
	}
}

void UIBattleHUD::useSkillEffect(int index,bool isRestore)
{
	openView(MAPUI_COCOS_RES[INDEX_UI_FIGHT_UI_SKILL_EFFECT_CSB]);
	auto skill_effect = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FIGHT_UI_SKILL_EFFECT_CSB]);
	skill_effect->setCameraMask(4,true);
	skill_effect->setPosition(STARTPOS);

	auto p_node = skill_effect->getChildByName<Widget*>("panel_node");
	auto name_bg = p_node->getChildByName<ImageView*>("image_skill_name_bg");
	auto t_name = name_bg->getChildByName<Text*>("label_skill_name");
	auto i_skill = p_node->getChildByName<ImageView*>("image_skill");
	auto i_skill1 = p_node->getChildByName<ImageView*>("image_skillfx_light");
	auto i_skill2 = p_node->getChildByName<ImageView*>("image_skillfx_light_restore");
	auto i_skill3 = p_node->getChildByName<ImageView*>("image_skillfx_light_atk");
	i_skill->ignoreContentAdaptWithSize(false);
	i_skill->loadTexture(getSkillIconPath(index, SKILL_TYPE_PLAYER));
	t_name->setString(getSkillName(index, SKILL_TYPE_PLAYER));

	p_node->setVisible(true);
	name_bg->setVisible(true);

	name_bg->setOpacity(0);
	t_name->setOpacity(0);
	i_skill->setOpacity(255);
	i_skill1->setOpacity(255);
	i_skill2->setOpacity(0);
	i_skill3->setOpacity(0);

	p_node->setPositionY(0);
	p_node->setScale(0.1);
	i_skill1->setVisible(false); 
	i_skill1->setScale(1);
	if (isRestore)
	{
		i_skill2->setVisible(true);
		i_skill3->setVisible(false);
	}else
	{
		i_skill2->setVisible(false);
		i_skill3->setVisible(true);
	}

	auto winSize = Director::getInstance()->getWinSize();
	float up_time = 0.3;
	float l_time1 = 0.9;  
	float l_time2 = 2.0;  
	float scale_time = 0.3; 
	float delay_time = 1.8;
	float fade_time = 0.2;
	auto up_move = MoveBy::create(up_time,Vec2(0,winSize.height/2));
	auto up_scale = ScaleTo::create(up_time,0.5);
	auto spawn = Spawn::createWithTwoActions(EaseExponentialOut::create(up_scale),EaseExponentialOut::create(up_move));
	auto seq = Sequence::createWithTwoActions(spawn,ScaleTo::create(0.5,1));
	p_node->runAction(seq);

	i_skill1->runAction(RotateBy::create(l_time1,180));
	i_skill2->runAction(RotateBy::create(l_time2,360));
	i_skill3->runAction(RotateBy::create(l_time2,360));
	i_skill1->runAction(Sequence::createWithTwoActions(DelayTime::create(l_time1-scale_time),ScaleTo::create(scale_time,0.1)));
	i_skill2->runAction(FadeIn::create(l_time1));
	i_skill3->runAction(FadeIn::create(l_time1));

	i_skill->runAction(Sequence::createWithTwoActions(DelayTime::create(delay_time),FadeOut::create(0.2)));
	i_skill1->runAction(Sequence::createWithTwoActions(DelayTime::create(delay_time),FadeOut::create(0.2)));
	i_skill2->runAction(Sequence::createWithTwoActions(DelayTime::create(delay_time),FadeOut::create(0.2)));
	i_skill3->runAction(Sequence::createWithTwoActions(DelayTime::create(delay_time),FadeOut::create(0.2)));
	name_bg->runAction(Sequence::create(DelayTime::create(up_time),FadeIn::create(fade_time),DelayTime::create(delay_time-fade_time-up_time),FadeOut::create(fade_time),nullptr));
	t_name->runAction(Sequence::create(DelayTime::create(up_time),FadeIn::create(fade_time),DelayTime::create(delay_time-fade_time-up_time),FadeOut::create(fade_time),nullptr));
	skill_effect->runAction(Sequence::createWithTwoActions(DelayTime::create(l_time2),Place::create(ENDPOS)));
}

void UIBattleHUD::setBloodValue(float num,int index,bool isFriend)
{
	if (isFriend)
	{
		if (index - 1 < m_vHerosInfo.size())
		{
			auto w_hero = m_vHerosInfo[index - 1];
			auto l_durable = w_hero->getChildByName<LoadingBar*>("progressbar_durable");
			l_durable->setPercent(100 * num / m_pResult->myships[index - 1]->max_hp);
		}
	}else
	{
		if (index - 6 < m_vEnemysInfo.size())
		{
			auto w_enemy = m_vEnemysInfo[index - 6];
			auto l_durable = w_enemy->getChildByName<LoadingBar*>("progressbar_durable");
			l_durable->setPercent(100 * num / m_pResult->enemyships[index - 6]->max_hp);
		}
	}
}

void UIBattleHUD::checkIsWin(float delta)
{
	int friendNums = m_pShareObj->m_vMyFriends.size();
	int enemysNums = m_pShareObj->m_vEnemys.size();
	if (m_RemainTime < 1)
	{
		sendDataToServer(BATTLE_DRAW);
		m_bIsBattleEnd = true;
	}
	if (friendNums < 1 && enemysNums > 0)
	{
		sendDataToServer(BATTLE_DEFEATED);
		m_bIsBattleEnd = true;
		return;
	}
	if (friendNums > 0 && enemysNums < 1)
	{	
		sendDataToServer(BATTLE_WIN);
		m_bIsBattleEnd = true;
		return;
	}
	if (friendNums < 1 && enemysNums < 1)
	{
		sendDataToServer(BATTLE_DEFEATED);
		m_bIsBattleEnd = true;
		return;
	}
}

void UIBattleHUD::updateCount(float delta)
{
	if (m_bIsBattleEnd)
	{
		return;
	}
	m_pPrevTime += int(delta * 1000);
	if (m_pPrevTime > 999 && m_RemainTime > 0)
	{
		m_pPrevTime = 0;	
		m_RemainTime = (m_RemainTime > 1000 ? m_RemainTime - 1000:0);
		if (m_RemainTime / 1000 <= 30)
		{
			m_TimeLoad->loadTexture("ship/time_line_red.png");
		}
		if (m_RemainTime < 1)
		{
			checkIsWin(0);
		}
		std::string s_time = StringUtils::format("%d:%02d",m_RemainTime/1000/60,(m_RemainTime/1000)%60);
		m_pTimeShow->setString(s_time);
		m_TimeLoad->setPercent(100.0*m_RemainTime/m_TotleTime);
	}
}

void UIBattleHUD::heroAttacked(int tag,float harm)
{
	if (tag > 5)
	{
		if (m_vEnemysInfo.size() < tag - 5)
		{
			return;
		}
		auto *enemy = m_vEnemysInfo[tag - 6];
		auto image_attacked_flag = enemy->getChildByName("image_atk_bg");
		auto progressto_blood = enemy->getChildByName<LoadingBar*>("progressbar_durable");
		image_attacked_flag->setVisible(true);
		progressto_blood->setPercent(harm);
		auto seq = Sequence::createWithTwoActions(Blink::create(1.f,3),CCCallFuncN::create(CC_CALLBACK_0(UIBattleHUD::blindEnd,this,image_attacked_flag)));
		image_attacked_flag->runAction(seq);
		updateAnger(false);
		updateAnger(true);
	}else
	{
		if (m_vHerosInfo.size() < tag)
		{
			return;
		}
		auto *hero = m_vHerosInfo[tag - 1];
		auto image_attacked_flag = hero->getChildByName("image_atk_bg");
		auto progressto_blood = hero->getChildByName<LoadingBar*>("progressbar_durable");
		progressto_blood->setPercent(harm);
		image_attacked_flag->setVisible(true);
		auto seq = Sequence::createWithTwoActions(Blink::create(1.f,3),CCCallFuncN::create(CC_CALLBACK_0(UIBattleHUD::blindEnd,this,image_attacked_flag)));
		image_attacked_flag->runAction(seq);
		updateAnger(true);
		updateAnger(false);
	}
}

void UIBattleHUD::heroDie(int tag)
{
	if (tag > 5)
	{
		if (m_vEnemysInfo.size() < tag - 5)
		{
			return;
		}
		auto *enemy = m_vEnemysInfo[tag - 6];
		auto l_durable = enemy->getChildByName("progressbar_durable");
		l_durable->setVisible(false);
		ImageView* image_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(enemy, "image_ship"));
		setGLProgramState(image_ship,true);
		image_ship->setCameraMask(4);
	}else
	{
		if (m_vHerosInfo.size() < tag)
		{
			return;
		}
		auto *hero = m_vHerosInfo[tag - 1];
		auto l_durable = hero->getChildByName("progressbar_durable");
		l_durable->setVisible(false);
		ImageView* image_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(hero, "image_ship"));
		setGLProgramState(image_ship,true);
		image_ship->setCameraMask(4);
	}
}

void UIBattleHUD::updateAnger(bool isHero)
{
	if (isHero)
	{
		int hero_anger = m_pDelegate->getShareObj()->m_nMorale;
		Widget* heroInfo = (Widget*)m_pFight->getChildByName("panel_info_1");
		ImageView* image_moral_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(heroInfo, "image_moral_left"));

		for (int i = 1; i <= 5; i++)
		{
			std::string str_moral = StringUtils::format("image_moral_%d", i);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(image_moral_bg, str_moral));
			moral_image->loadTexture(getMoralIconPath(3));
		}

		int n1000 = (hero_anger / 1000) % 6;
		int n100 = hero_anger - n1000 * 1000;
		for (int i = 1; i <= n1000; i++)
		{
			std::string str_moral = StringUtils::format("image_moral_%d", i);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(image_moral_bg, str_moral));
			moral_image->loadTexture(getMoralIconPath(1));

		}
		if (n1000 < 5 && n100 >= 50)
		{
			std::string str_moral = StringUtils::format("image_moral_%d", n1000 + 1);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(image_moral_bg, str_moral));
			moral_image->loadTexture(getMoralIconPath(2));
		}
		m_pDelegate->getShareObj()->m_nMorale = hero_anger;

	}else
	{
		int enemy_anger = m_pDelegate->getShareObj()->m_nMorale_Enemy;
		Widget* npcInfo = (Widget*)m_pFight->getChildByName("panel_info_2");

		ImageView* image_moral_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(npcInfo, "image_moral_left"));

		for (int i = 1; i <= 5; i++)
		{
			std::string str_moral = StringUtils::format("image_moral_%d", i);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(image_moral_bg, str_moral));
			moral_image->loadTexture(getMoralIconPath(3));
		}

		int n1000 = (enemy_anger / 1000) % 6;
		int n100 = enemy_anger - n1000 * 1000;
		for (int i = 1; i <= n1000; i++)
		{
			std::string str_moral = StringUtils::format("image_moral_%d", i);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(image_moral_bg, str_moral));
			moral_image->loadTexture(getMoralIconPath(1));
		}
		if (n1000 < 5 && n100 >= 50)
		{
			std::string str_moral = StringUtils::format("image_moral_%d", n1000 + 1);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(image_moral_bg, str_moral));
			moral_image->loadTexture(getMoralIconPath(2));
		}
		m_pDelegate->getShareObj()->m_nMorale_Enemy = enemy_anger;
	}
}

void UIBattleHUD::blindEnd(Node* target)
{
	if (target)
	{
		target->setVisible(false);
	}
}

void UIBattleHUD::onWin(int winflag,EndFightResult* result)
{
	if (m_pEndFightResult->fighttype == FIGHT_TYPE_ROB)
	{
		flushRobBattleResult(winflag,result);
	}
	else
	{
		flushBattleResult1(winflag, result);
	}
	if (result->newlevel)
	{
		m_bIsLevelUp = true;
		SINGLE_HERO->m_iLevel = result->newlevel;
	}

	if (result->new_fame_level)
	{
		//声望降级时服务器返回new_fame_level为负值，代表降级
		if (result->new_fame_level<SINGLE_HERO->m_iPrestigeLv&&result->new_fame_level<0)
		{
			m_bIsPrestigeDown = true;
			SINGLE_HERO->m_iPrestigeLv = -result->new_fame_level;
		}
		else
		{
			m_bIsPrestigeUp = true;
			SINGLE_HERO->m_iPrestigeLv = result->new_fame_level;
		}
	}

	if (result->n_captains)
	{
		for (int i = 0; i< result->n_captains; i++)//有船长增加的经验大于0是才显示界面
		{
			if (result->captains[i]->lootexp > 0)
			{
				m_bIsCaptainUp = true;
				break;
			}else
			{
				m_bIsCaptainUp = false;
			}
		}
	}
}

void UIBattleHUD::sendDataToServer(int reason)
{
	if (!m_bIsSendToData)
	{
		return;
	}
	m_bIsSendToData = false;
	m_pDelegate->stopBattle();
	if (ProtocolThread::GetInstance()->isInTutorial())
	{
		if (reason == BATTLE_DEFEATED || reason == BATTLE_DRAW)
		{
			auto pNovice = UINoviceStoryLine::createNovice(this);
			this->addChild(pNovice, MAX_Z_ORDER + 2);
			pNovice->onGuideTaskMessage(UINoviceStoryLine::WAR_FAIL_GUIDE_DIALOG,0);
			pNovice->setCameraMask(4);
			return;
		}
	}
	//能赎回时才进行判断，不能赎回时直接死亡
	//增加战斗失败后花费V票提示
	if (m_pResult->supportransom == 1 && reason == BATTLE_DEFEATED && !m_bIsVTickesTip && m_pResult->fighttype == FIGHT_TYPE_NORMAL && m_pResult->userlv >= 10)
	{
		m_bIsVTickesTip = true;
		SINGLE_HERO->m_iGold = m_pResult->vtickets;
		ProtocolThread::GetInstance()->saveFailedFight(UILoadingIndicator::createWithMask(this, 4));
		return;
	}

	if (reason == BATTLE_DEFEATED && !m_bIsVTickesTip && m_pResult->fighttype == FIGHT_TYPE_ROB)
	{
		m_bIsVTickesTip = true;
		ProtocolThread::GetInstance()->endFailedFightByVTicket(1, FIGHT_TYPE_ROB, UILoadingIndicator::createWithMask(this, 4));
		return;
	}


	auto &myShips = m_pDelegate->getShareObj()->m_vMyFriends;
	int shipsNum = myShips.size();

	//攻打海盗基地时用到
	int enemyHurt = 0;
	int enemySailor = 0;
	if (m_pResult->fighttype == FIGHT_TYPE_ATTACK_PIRATE)
	{	
		if (m_pDelegate->getShareObj()->m_vEnemys.size())
		{
			auto &enemyShips = m_pDelegate->getShareObj()->m_vEnemys[0];
			if (enemyShips)
			{
				enemyHurt = m_pResult->enemyships[0]->current_hp - enemyShips->getBlood();
				enemySailor = m_pResult->enemyships[0]->sailornum - enemyShips->m_Info->_sailornum;
			}
		}
		else
		{
			enemyHurt = m_pResult->enemyships[0]->current_hp;
			enemySailor = m_pResult->enemyships[0]->sailornum;
		}
	}
	
	if (shipsNum > 0)
	{
		int *hps = new int[shipsNum];
		int *sailors = new int[shipsNum];
		int *shipId = new int[shipsNum];
		int capturingShipNum = m_nCapturingShipId.size(); 
		int *catchshipids = new int[capturingShipNum];
		int foodPlunderShipNum = m_nFoodPlunderShipId.size();
		int *foodplundershipids = new int[foodPlunderShipNum];
		for (int i = 0;i < shipsNum;i++)
		{
			auto *info = myShips[i]->m_Info;
			//预防越界处理
			if (m_pResult->n_myships <= info->_index)
			{
				return;
			}
			//排除战斗中使用修理技能后最好结算血量会增加的问题
			if (myShips[i]->getBlood() > m_pResult->myships[info->_index]->current_hp)
			{
				hps[i] = m_pResult->myships[info->_index]->current_hp;
				//myShips[i]->setBlood(m_pResult->myships[info->_index]->current_hp);
			}
			else
			{
				hps[i] = myShips[i]->getBlood();
				//myShips[i]->setBlood(m_pResult->myships[info->_index]->current_hp);
			}
			sailors[i] = info->_sailornum;
			shipId[i] = m_pResult->myships[info->_index]->shipid;
		}

		for (size_t i = 0; i < capturingShipNum; i++)
		{
			catchshipids[i] = m_nCapturingShipId[i];
		}

		for (size_t i = 0; i < foodPlunderShipNum; i++)
		{
			foodplundershipids[i] = m_nFoodPlunderShipId[i];
		}
		int t = m_pDelegate->getShareObj()->m_nMorale;
		ProtocolThread::GetInstance()->endFight(reason, shipsNum, hps, sailors, shipId, m_pResult->props, m_pResult->n_props, t, m_pResult->fighttype, m_RemainTime / 1000, enemyHurt, enemySailor, capturingShipNum, catchshipids, foodPlunderShipNum, foodplundershipids, (m_TotleTime - m_RemainTime) / 1000,nullptr, UILoadingIndicator::createWithMask(this, 4));

		delete [] hps;
		delete [] shipId;
		delete [] sailors;
		delete [] catchshipids;
		delete [] foodplundershipids;
	}else
	{
		int t = m_pDelegate->getShareObj()->m_nMorale;
		ProtocolThread::GetInstance()->endFight(reason, 0, nullptr, nullptr, nullptr, m_pResult->props, m_pResult->n_props, t, m_pResult->fighttype, m_RemainTime / 1000, enemyHurt, enemySailor, 0, nullptr, 0, nullptr, (m_TotleTime - m_RemainTime) / 1000,nullptr, UILoadingIndicator::createWithMask(this, 4));
	}
}

void UIBattleHUD::onHttpRequestCompleted(network::HttpClient* client, network::HttpResponse* response)
{
	long code = response->getResponseCode();
	log("upload: code %ld", code);
	auto data = response->getResponseData();
	std::string s_data(data->begin(),data->end());
	log("upload:%s",s_data.c_str());
	if (s_data == "ok")
	{
		if (m_pWaitForLogUploading)
		{
			m_pWaitForLogUploading->removeSelf();
			m_pWaitForLogUploading = 0;
		}

		ProtocolThread::GetInstance()->notifyBattleLog(m_pResult->fighttype,UILoadingIndicator::createWithMask(this,4));
		log("upload:ok ....");
	}else
	{
		auto buff = response->getErrorBuffer();
		log("upload: buffer %s",buff);
		log("upload:fail");
		//提交log失败，发送失败原因 3 代表游戏问题
		std::string failedStr = StringUtils::format("http code:%ld  server return:%s  error buffer:%s", code, s_data.c_str(), buff);
		char* failreason = (char*)failedStr.c_str();
		if (m_sendBattleLogRetry > 0)
		{
			m_sendBattleLogRetry--;
			WriterLog::getInstance()->sendDataToServer();
		}else 
		{
			if (m_pWaitForLogUploading)
			{
				m_pWaitForLogUploading->removeSelf();
				m_pWaitForLogUploading = 0;
			}
			ProtocolThread::GetInstance()->sendCustomServiceInfo(3, failreason, 0, UILoadingIndicator::create(this));
			ProtocolThread::GetInstance()->notifyBattleLog(m_pResult->fighttype, UILoadingIndicator::createWithMask(this, 4));
		}
	}
}

void UIBattleHUD::flushBattleResult1(int winflag, EndFightResult* result){

	SINGLE_HERO->m_iLevel = EXP_NUM_TO_LEVEL(result->totalexp);
	SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->totalfame);
	SINGLE_HERO->m_iCoin = result->coins;
	SINGLE_HERO->m_iGold = result->golds;

	openView(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_1_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_1_CSB]);

	auto panel_addexp = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_addexp"));

	auto image_head_player_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_addexp, "image_head_player_bg"));
	auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_head_player_bg, "image_head"));
	auto label_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv"));
	auto label_lv_r = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv_r"));
	auto panel_coin = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted"));
	auto label_coin_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_coin, "label_hosted_num"));
	auto i_skill = panel_coin->getChildByName<ImageView*>("image_skill_bg_2_2");
	auto t_skill = i_skill->getChildByName<Text*>("text_item_skill_lv");

	auto panel_supply = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_2"));
	auto label_supply_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_supply, "label_hosted_num"));

	auto image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_bg"));
	auto image_result = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_bg, "image_title_victory"));
	std::string path;
	switch (winflag)
	{
	case BATTLE_WIN:
		path = WAR_VICTORY[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DEFEATED:
		path = WAR_DEFEATED[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DRAW:
		path = WAR_DRAW[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_FLEE:
		path = WAR_FLEE[SINGLE_SHOP->L_TYPE];
		break;
	default:
		break;
	}
	image_result->loadTexture(path);
	
	image_head->ignoreContentAdaptWithSize(false);
	image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));

	float temp_exp = 0;
	if (result->explv < LEVEL_MAX_NUM)
	{
		temp_exp = (result->totalexp - LEVEL_TO_EXP_NUM(result->explv))*1.0/(LEVEL_TO_EXP_NUM(result->explv+1)-LEVEL_TO_EXP_NUM(result->explv));
	}
	float temp_rep = 0;
	if (result->famelv < LEVEL_MAX_NUM)
	{
		temp_rep = (result->totalfame - LEVEL_TO_FAME_NUM(result->famelv))*1.0/(LEVEL_TO_FAME_NUM(result->famelv+1)-LEVEL_TO_FAME_NUM(result->famelv));
	}
	Widget* panel_exp = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_addexp, "panel_exp"));
	panel_exp->setContentSize(Size(panel_exp->getContentSize().width, 120 * temp_exp));
	Widget* panel_rep = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_addexp, "panel_rep"));
	panel_rep->setContentSize(Size(panel_rep->getContentSize().width, 120 * temp_rep));

	std::string s_supply = StringUtils::format("+%d", result->supply + result->lootenemysupply);
	std::string s_coin = StringUtils::format("+%lld", result->lootcoins + result->extralootcoins);
	label_lv->setString(String::createWithFormat("%d", SINGLE_HERO->m_iLevel)->getCString());
	label_lv_r->setString(StringUtils::format("%d", SINGLE_HERO->m_iPrestigeLv));

	//add exp and rep
	auto p_exp = panel_addexp->getChildByName<Widget*>("panel_exp");
	auto p_rep = panel_addexp->getChildByName<Widget*>("panel_r");
	auto t_exp = p_exp->getChildByName<Widget*>("label_exp");
	auto i_rep = p_rep->getChildByName<Widget*>("image_r");
	auto label_exp_num = p_exp->getChildByName<Text*>("label_exp_num");
	if (result->lootexp >= 0)
	{
		label_exp_num->setString(StringUtils::format("+%lld", result->lootexp));
	}
	else
	{
		label_exp_num->setString(StringUtils::format("%lld", result->lootexp));
	}

	auto label_r_num = p_rep->getChildByName<Text*>("label_r_num");
	if (result->lootfame < 0)
	{
		label_r_num->setString(StringUtils::format("%lld", result->lootfame));
	}
	else
	{
		label_r_num->setString(StringUtils::format("+%lld", result->lootfame));
	}
	
	t_exp->setPositionX(p_exp->getBoundingBox().size.width / 2 - label_exp_num->getBoundingBox().size.width / 2 - 10);
	label_exp_num->setPositionX(t_exp->getPositionX() + t_exp->getBoundingBox().size.width / 2 + 20);
	i_rep->setPositionX(p_rep->getBoundingBox().size.width / 2 - label_r_num->getBoundingBox().size.width / 2 - 10);
	label_r_num->setPositionX(i_rep->getPositionX() + i_rep->getBoundingBox().size.width / 2 + 20);

	label_coin_num->setString(s_coin);
	label_supply_num->setString(s_supply);

	if (result->skill_plunder_master > 0 || result->skill_stealing_from_thieves > 0)
	{
		i_skill->setVisible(true);
		i_skill->setTouchEnabled(true);
		i_skill->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::menuCall_func, this));
		if (result->skill_plunder_master > 0)
		{
			i_skill->loadTexture(getSkillIconPath(SKILL_PREDATORY_MASTE, SKILL_TYPE_PLAYER));
			i_skill->setTag(SKILL_PREDATORY_MASTE);
			setTextSizeAndOutline(t_skill, result->skill_plunder_master);
		}
		else
		{
			i_skill->loadTexture(getSkillIconPath(SKILL_BLACK_EAT_BLACK, SKILL_TYPE_PLAYER));
			i_skill->setTag(SKILL_BLACK_EAT_BLACK);
			setTextSizeAndOutline(t_skill, result->skill_stealing_from_thieves);
		}
		label_coin_num->setPositionX(i_skill->getPositionX() -  i_skill->getBoundingBox().size.width / 2);
	}
	else
	{
		i_skill->setVisible(false);
	}

	auto image_item = view->getChildByName<ImageView*>("image_items");
	image_item->setTouchEnabled(true);
	auto panel_ship = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_3"));
	auto l_captured = panel_ship->getChildByName<ListView*>("list_captured");
	auto t_no = panel_ship->getChildByName<Text*>("text_content");
	if (result->n_catchshipdefine < 1)
	{
		t_no->setVisible(true);
	}
		
	for (int i = 0; i < result->n_catchshipdefine; i++)
	{
		auto item = image_item->clone();
		auto i_item = item->getChildByName<ImageView*>("image_goods");
		auto t_num = item->getChildByName<Text*>("text_item_num");
		item->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::showShipInfo, this));
		t_num->setVisible(false);
		item->setTag(i);
		i_item->setTag(result->catchshipdefine[i]->sid);
		i_item->loadTexture(getShipIconPath(result->catchshipdefine[i]->sid));
		setBgButtonFormIdAndType(item, result->catchshipdefine[i]->sid, ITEM_TYPE_SHIP);
		l_captured->pushBackCustomItem(item);
	}
	l_captured->setPositionX(l_captured->getPositionX() + (5 - result->n_catchshipdefine) * 63);
	

	auto l_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_content"));
	auto panel_items = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_4"));
	int n_gird = ceil(result->n_lootitemsarray / 9.0);
	auto t_no2 = panel_items->getChildByName<Text*>("text_content");
	if (result->n_lootitemsarray < 1)
	{
		t_no2->setVisible(true);
	}
	for (int i = 0; i < n_gird; i++)
	{
		Widget* p_item;
		if (i < 1)
		{
			p_item = panel_items;
		}
		else
		{
			p_item = panel_items->clone();
			l_content->pushBackCustomItem(p_item);
			p_item->getChildByName<ImageView*>("image_loot")->setVisible(false);
			p_item->getChildByName<Text*>("label_loots")->setVisible(false);
		}
		auto l_item = p_item->getChildByName<ListView*>("list_item");
		l_item->removeAllItems();
		size_t j = 0;
		for (; j < 9; j++)
		{
			if (i * 9 + j < result->n_lootitemsarray)
			{
				auto item = image_item->clone();
				auto i_item = item->getChildByName<ImageView*>("image_goods");
				auto t_num = item->getChildByName<Text*>("text_item_num");
				item->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::showItemInfo, this));
				std::string name;
				std::string path;
				getItemNameAndPath(result->lootitemsarray[i * 9 + j]->itemtype, result->lootitemsarray[i * 9 + j]->itemid, name, path);
				i_item->setVisible(true);
				t_num->setVisible(true);
				item->setTag(result->lootitemsarray[i * 9 + j]->itemtype);
				i_item->loadTexture(path);
				i_item->setTag(result->lootitemsarray[i * 9 + j]->itemid);
				setTextSizeAndOutline(t_num, result->lootitemsarray[i * 9 + j]->count);
				setBgButtonFormIdAndType(item, result->lootitemsarray[i * 9 + j]->itemid, result->lootitemsarray[i * 9 + j]->itemtype);
				l_item->pushBackCustomItem(item);
			}
			else
			{
				break;
			}
		}
		l_item->setPositionX(l_item->getPositionX() + (9 - j) * 63);
	}

	l_content->setClippingEnabled(true);
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 3);
	addListViewBar(l_content, image_pulldown);
	view->setCameraMask(4);

//chengyuan++ 战斗结束后损坏装备提示框
	if (result->brokenequips && result->n_brokenequips > 0)
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushEquipBrokenView(result->brokenequips, result->n_brokenequips);
	}
}

void UIBattleHUD::flushBattleResult2(int winflag, EndFightResult* result)
{
	openView(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_2_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_2_CSB]);
	view->setPosition(STARTPOS);
	view->setCameraMask(4,true);
	view->setLocalZOrder(100);

	ImageView* image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_bg"));
	auto image_result = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_bg, "image_title_victory"));
	std::string path;
	switch (winflag)
	{
	case BATTLE_WIN:
		path = WAR_VICTORY[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DEFEATED:
		path = WAR_DEFEATED[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DRAW:
		path = WAR_DRAW[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_FLEE:
		path = WAR_FLEE[SINGLE_SHOP->L_TYPE];
		break;
	default:
		break;
	}
	image_result->loadTexture(path);

	for (int i = 0; i < 5; i++)
	{
		auto ship_bg = view->getChildByName<Widget*>(StringUtils::format("image_ship_bg_%d",i+1));
		if (i < m_pResult->n_myships)
		{
			ship_bg->setVisible(true);
			auto image_ship_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(ship_bg, "image_ship_bg"));
			auto image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(ship_bg, "image_item_bg_lv"));

			auto ship_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(ship_bg,"image_ship"));
			auto ship_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(ship_bg,"image_num"));
			ship_icon->ignoreContentAdaptWithSize(false);
			ship_icon->loadTexture(getShipIconPath(m_pResult->myships[i]->sid));
			ship_position->ignoreContentAdaptWithSize(false);
			ship_position->loadTexture(getPositionIconPath(i+1));
			
			//稀有度
			setBgButtonFormIdAndType(image_ship_bg, m_pResult->myships[i]->sid, ITEM_TYPE_SHIP);
			setBgImageColorFormIdAndType(image_item_bg_lv, m_pResult->myships[i]->sid, ITEM_TYPE_SHIP);

			auto panel_durable = dynamic_cast<Widget*>(Helper::seekWidgetByName(ship_bg,"panel_durable"));
			auto progressbar_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_durable,"progressbar_durable"));
			auto image_17 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_durable,"Image_17"));
			auto progressbar_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(ship_bg,"progressbar_sailor"));
			auto image_dead = ship_bg->getChildByName<ImageView*>("image_dead");
			auto panel_ship_content = ship_bg->getChildByName<Widget*>("panel_ship_content");
			auto image_durable = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship_content,"image_durable"));
			auto text_max = dynamic_cast<Text*>(Helper::seekWidgetByName(image_durable, "text_max"));
			text_max->setString("MAX");
			auto image_div_4 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship_content,"image_div_4"));
			auto goods_desc = panel_ship_content->getChildByName<Text*>("label_sailors_num");
			goods_desc->setContentSize(goods_desc->getContentSize() + Size(0, 200));

			if (winflag == BATTLE_DEFEATED)
			{
				image_17->setVisible(false);
				progressbar_durable->setPercent(0);
				progressbar_sailor->setPercent(0);
				image_dead->setVisible(true);
				image_durable->setVisible(false);
				image_div_4->setVisible(false);
				setGLProgramState(ship_icon,true);
				goods_desc->setString(SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_SHIP_SINK"]);
			}else
			{
				image_dead->setVisible(false);
				image_durable->setVisible(true);
				image_div_4->setVisible(true);
				setGLProgramState(ship_icon,false);

				auto panel_ship_durable_num_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_ship_content,"panel_ship_durable_num_1"));
				auto label_ship_durable_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship_durable_num_1,"label_ship_durable_num_1"));
				auto label_ship_durable_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship_durable_num_1,"label_ship_durable_num_2"));
				
				auto getShipInfo = [&](int tag)->TVBasicShip*
				{
					auto &myShips = m_pDelegate->getShareObj()->m_vMyFriends;
					for (int i = 0; i < myShips.size(); i++)
					{
						if (myShips[i]->m_Info->_tag == tag)
						return myShips[i];
					}
					return nullptr;
				};

				auto ship = getShipInfo(i + 1);
				int cur_hp = 0;
				int cur_sailors = 0;
				float temp = 0;
				int lost_goods = 0;
				if (ship)
				{
					cur_hp = ship->getBlood();
					cur_sailors = ship->m_Info->_sailornum;
				}else
				{
					cur_hp = 1;
					cur_sailors = 0;
					if (winflag == BATTLE_WIN)
					{
						lost_goods = 5;
					}else
					{
						lost_goods = 10;
					}
				}

				if(m_pResult->myships[i]->current_max_hp < m_pResult->myships[i]->max_hp)
				{   //not normal
					image_17->setVisible(true);
					if (m_pResult->myships[i]->current_max_hp != 0)
					{
						temp = (cur_hp*1.0)/( m_pResult->myships[i]->current_max_hp*1.0);
					}
					progressbar_durable->setPercent(87 * temp);
				}else
				{
					image_17->setVisible(false);
					if (m_pResult->myships[i]->max_hp != 0)
					{
						temp = (cur_hp*1.0)/(m_pResult->myships[i]->max_hp*1.0);
					}
					progressbar_durable->setPercent(100 * temp);
				}

				if (0 == m_pResult->myships[i]->sailornum)
				{
					progressbar_sailor->setPercent(0);
				}else
				{
					progressbar_sailor->setPercent(100.0*cur_sailors / m_pResult->myships[i]->sailornum);
				}

				int lost_hp = m_pResult->myships[i]->current_hp-cur_hp;
				int lost_max_hp = 0;
				for (int j = 0; j < result->n_lostships; j++)
				{
					if (m_pResult->myships[i]->shipid == result->lostships[j]->shipid)
					{
						lost_max_hp = result->lostships[j]->lostmaxhp;
						break;
					}
				}
				//排除战斗中使用修理技能后最好结算血量会增加的问题
				if (lost_hp < 0)
				{
					lost_hp = 0;
				}
				std::string s_hp_max = StringUtils::format("-%d", lost_max_hp);
				if (lost_max_hp == 0)
				{
					s_hp_max = "0";
				}
				std::string s_goods = SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_NOT_LOST_GOODS"];
				if (lost_goods > 0)
				{
					s_goods = SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_LOST_GOODS"];
					std::string old_value = "[num]";
					std::string new_value = StringUtils::format("%d",lost_goods);
					repalce_all_ditinct(s_goods,old_value,new_value);
				}
			
				label_ship_durable_num_2->setString(s_hp_max);
				goods_desc->setString(s_goods);
			}
		}else
		{
			ship_bg->setVisible(false);
		}
	}

	if (ProtocolThread::GetInstance()->isInTutorial())
	{
		//引导的指示
		auto warResult = view->getChildByName<Button*>("button_continue_2");
		auto handpic = Sprite::create();
		handpic->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
		handpic->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
		warResult->addChild(handpic, 10);
		handpic->setCameraMask(4, true);
		handpic->setLocalZOrder(1000);
		//小手在按钮左上角
		handpic->setPosition(Vec2(warResult->getContentSize().width / 2 - handpic->getContentSize().width / 2, warResult->getContentSize().height / 2 * 1.6 + handpic->getContentSize().height / 2 * 0.6));
	}
}

unsigned int UIBattleHUD::getFightTime(float myFighting,float enemyFighting)
{
	//TODO
	int time = 3 * 60 * 1000;
	if (m_pResult->battle_time > 0)
	{
		time = m_pResult->battle_time * 1000;
	}
	return time;
}

void UIBattleHUD::showItemInfo(Ref* pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender); 
		if (target)
		{
			auto i_good = target->getChildByName<ImageView*>("image_goods");
			openGoodInfo(nullptr, target->getTag(), i_good->getTag());
		}
	}
}

void UIBattleHUD::showShipInfo(Ref* pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		if (target)
		{
			auto i_good = target->getChildByName<ImageView*>("image_goods");
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushShipDetail(m_pEndFightResult->catchshipdefine[target->getTag()], i_good->getTag(), true);
		}
	}
}

void UIBattleHUD::setMainShipDied()
{
	initMainStrategyButton(MODEL_FREE,m_bIsSwitchAI);
	m_pDelegate->callbackEventByIndex(MODEL_FREE);
	m_bIsSwitchAI = false;

	auto p_skill = m_pFight->getChildByName<Widget*>("panel_skill");
	auto p_prop = m_pFight->getChildByName<Widget*>("panel_prop");
	for (int i = 0; i < 4; i++)
	{
		std::string button_skill = StringUtils::format("button_skill_%d",i+1);
		std::string button_prop = StringUtils::format("button_item_%d", i + 1);
		std::string image_skill = StringUtils::format("image_skill_%d",i+1);
		std::string button_area = StringUtils::format("button_area_%d",i+1);
		std::string button_item_area = StringUtils::format("button_item_area_%d", i + 1);
		std::string image_prop = StringUtils::format("image_item_%d", i + 1);
		
		auto b_skill = p_skill->getChildByName<Button*>(button_skill);
		auto i_skill = b_skill->getChildByName<ImageView*>(image_skill);
		auto b_area = b_skill->getChildByName<Button*>(button_area);
		auto b_item = p_prop->getChildByName<Button*>(button_prop);
		auto i_prop_bg = b_item->getChildByName<ImageView*>(image_skill);
		auto i_prop = i_prop_bg->getChildByName<ImageView*>(image_prop);
		auto b_prop = b_item->getChildByName<Button*>(button_item_area);
		b_area->setTouchEnabled(false);
		if(i!=1)b_prop->setTouchEnabled(false);
		setGLProgramState(i_skill,true);
		if(i!=1)setGLProgramState(i_prop, true);
		
		//去除倒计时
		m_gSkillTimeNum[i] = 0;
		m_gPerSkillTimeNum[i] = 1;
	}

	//海盗攻城活动旗舰死亡结束战斗
	if (m_pResult->fighttype == FIGHT_TYPE_PIRATE_ATTACK && m_bIsSendToData)
	{
		m_bIsSendToData = false;
		m_pDelegate->stopBattle(); 
		int t = m_pDelegate->getShareObj()->m_nMorale;
		ProtocolThread::GetInstance()->endFight(BATTLE_DEFEATED, 0, nullptr, nullptr, nullptr, m_pResult->props, m_pResult->n_props, t, m_pResult->fighttype, m_RemainTime / 1000, 0, 0, 0, nullptr, 0, nullptr, (m_TotleTime - m_RemainTime) / 1000,nullptr, UILoadingIndicator::createWithMask(this, 4));
	}

	//攻打海盗基地时用到
	if (m_pResult->fighttype == FIGHT_TYPE_ATTACK_PIRATE && m_bIsSendToData)
	{
		m_bIsSendToData = false;
		m_pDelegate->stopBattle();
		int enemyHurt = 0;
		int enemySailor = 0;
		if (m_pResult->fighttype == 3)
		{
			if (m_pDelegate->getShareObj()->m_vEnemys.size())
			{
				auto &enemyShips = m_pDelegate->getShareObj()->m_vEnemys[0];
				if (enemyShips)
				{
					enemyHurt = m_pResult->enemyships[0]->current_hp - enemyShips->getBlood();
					enemySailor = m_pResult->enemyships[0]->sailornum - enemyShips->m_Info->_sailornum;
				}
			}
			else
			{
				enemyHurt = m_pResult->enemyships[0]->current_hp;
				enemySailor = m_pResult->enemyships[0]->sailornum;
			}
		}
		int t = m_pDelegate->getShareObj()->m_nMorale;
		ProtocolThread::GetInstance()->endFight(BATTLE_DEFEATED, 0, nullptr, nullptr, nullptr, m_pResult->props, m_pResult->n_props, t, m_pResult->fighttype, m_RemainTime / 1000, enemyHurt, enemySailor, 0, nullptr, 0, nullptr, (m_TotleTime - m_RemainTime) / 1000, nullptr, UILoadingIndicator::createWithMask(this, 4));
	}
}

void UIBattleHUD::callUserskillids()
{
	m_pDelegate->startBattle(0);
	m_pDelegate->usedSkills(m_nSkillID);
}
void UIBattleHUD::setShipEquipBroken()
{
	if (m_pEndFightResult->brokenequips&&m_pEndFightResult->n_brokenequips)
	{
		for (int i = 0; i<m_pEndFightResult->n_brokenequips; i++)
		{
			if (m_pEndFightResult->brokenequips[i]->durability>0)
			{
				std::string st_ship_position = StringUtils::format(SHIP_POSTION_EQUIP_BROKEN, m_pEndFightResult->brokenequips[i]->position + 1);
				UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), true);
			}
		}
		UserDefault::getInstance()->flush();
	}
}
void UIBattleHUD::openDialogFaild(int64_t lostCoin)
{
	UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
	for (size_t i = 1; i <= 10; i++)
	{
		auto capturing_ship = StringUtils::format(CAPTURINGSHIPID, i);
		auto st_ship = StringUtils::format("%d", 0);
		UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(capturing_ship.c_str()).c_str(), getEncryptedPasswordHex((char*)st_ship.c_str()));
		auto food_plunder = StringUtils::format(FOODPLUNDERSHIPID, i);
		auto st_ship2 = StringUtils::format("%d", 0);
		UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(food_plunder.c_str()).c_str(), getEncryptedPasswordHex((char*)st_ship2.c_str()));
	}
	for (size_t i = 0; i < m_pResult->n_positiveskills; i++)
	{
		auto st_skill = StringUtils::format(SKILL_NUM, m_pResult->positiveskills[i]->id);
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_skill.c_str()).c_str(), 0);
	}
	UserDefault::getInstance()->flush();

	openView(MAPUI_COCOS_RES[INDEX_UI_FIGHT_DEFEATED_DIALOG_CSB]);
	auto viewfieldDiaog = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FIGHT_DEFEATED_DIALOG_CSB]);
	viewfieldDiaog->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::menuCall_func, this));
	viewfieldDiaog->setCameraMask(4, true);
	auto panel_defeat = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewfieldDiaog,"panel_defeat"));
	panel_defeat->setVisible(true);
	auto i_text_content = panel_defeat->getChildByName<Text*>("text_content");
	i_text_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_FAILED_DIALOG"]);

	anchPic = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewfieldDiaog, "image_anchor"));
	anchPic->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0, 10))), EaseBackOut::create(MoveBy::create(0.5, Vec2(0, -10))))));
	anchPic->setVisible(false);
	//花费了V票后
	if (lostCoin == -1)
	{
		//打劫时失败系统免V票
		if (m_pResult->fighttype == FIGHT_TYPE_ROB)
		{
			chatContent = SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_NO_VTICKES_FAILED_DIALOG"];
		}
		else
		{
			chatContent = SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_USE_VTICKES_FAILED_DIALOG"];
		}
		std::string new_vaule = SINGLE_SHOP->getCitiesInfo().find(SINGLE_HERO->m_iHave_licence_cityId)->second.name;
		std::string old_vaule = "[last_city]";
		repalce_all_ditinct(chatContent, old_vaule, new_vaule);
	}
	else
	{
		//主线任务战斗失败
		if (m_pEndFightResult->fighttype == FIGHT_TYPE_TASK)
		{
			chatContent = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_TASK_BATTLE_FAIL"];
		}
		else
		{
			//新手保护机制
			if (m_pEndFightResult->isprotect == 1)
			{
				chatContent = SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_FAILED_DIALOG_FOR_NEWPLAYER"];
			}
			else
			{
				chatContent = SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_FAILED_DIALOG"];//对话内容
			}
			std::string new_vaule = SINGLE_SHOP->getCitiesInfo().find(SINGLE_HERO->m_iHave_licence_cityId)->second.name;
			std::string old_vaule = "[lastCity]";
			repalce_all_ditinct(chatContent, old_vaule, new_vaule);
			std::string lossCoin = "[10000]";
			std::string newLossCoin = StringUtils::format("%lld", lostCoin);
			repalce_all_ditinct(chatContent, lossCoin, newLossCoin);
		}
	}
	
	
	i_text_content->setString("");
	chatTxt = i_text_content;
	this->schedule(schedule_selector(UIBattleHUD::showWarFailedText), 0.1 / 3);
}
void UIBattleHUD::showWarFailedText(float t)
{
	std::string	 text = chatTxt->getString();
	lenAfter = lenNum + text.length();
	std::string showT = "";
	plusNum = chatGetUtf8CharLen(chatContent.c_str()+lenAfter);
	lenAfter = lenAfter + plusNum;
	int maxLen = chatContent.length() + 1;
	showT = chatContent.substr(lenNum, lenAfter - lenNum);
	chatTxt->setString(showT);
	if (lenAfter >= maxLen-1)
	{
		m_bDiaolgOver = true;
		anchPic->setVisible(true);
		chatTxt = nullptr;
		this->unschedule(schedule_selector(UIBattleHUD::showWarFailedText));
	}
	else
	{
		m_bDiaolgOver = false;
	}
}

void UIBattleHUD::flushBattleResult1Novice()
{
	auto expNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str());
	auto repNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str());
	auto coinNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASILVERNUM").c_str());
	int noviceTotalExp = expNovice + 80;//新手引导经验加80
	int noviceTotalRep = repNovice + 9;//声望 9
	int noviceTotalCoin = coinNovice + 3000;//银币3000
	//UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str(), noviceTotalExp);
	//UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str(), noviceTotalRep);
	//UserDefault::getInstance()->flush();
	int levle = EXP_NUM_TO_LEVEL(noviceTotalExp);//经验等级
	int repLv = FAME_NUM_TO_LEVEL(noviceTotalRep);//声望等级

	auto winsize = Director::getInstance()->getWinSize();

	openView(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_1_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_1_CSB]);
	view->setPosition(STARTPOS);
	view->setCameraMask(4, true);
	view->setLocalZOrder(100);

	Widget* panel_addexp = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_addexp"));
	Text* label_exp_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_exp_num"));
	Text* label_r_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_r_num"));

	ImageView* image_head_player_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_addexp, "image_head_player_bg"));
	ImageView* image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_head_player_bg, "image_head"));
	Text* label_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv"));
	Text* label_lv_r = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv_r"));
	Widget* panel_coin = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted"));
	Text* label_coin_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_coin, "label_hosted_num"));
	auto i_skill = panel_coin->getChildByName<ImageView*>("image_skill_bg_2_2");
	i_skill->setVisible(false);
	Widget* panel_supply = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_2"));
	Text* label_supply_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_supply, "label_hosted_num"));

	ImageView* image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_bg"));
	auto image_result = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_bg, "image_title_victory"));
	std::string path = WAR_VICTORY[SINGLE_SHOP->L_TYPE];//新手引导必胜

	image_result->loadTexture(path);
	image_head->ignoreContentAdaptWithSize(false);
	image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));

	float temp_exp = 0;
	if (levle < LEVEL_MAX_NUM)
	{
		temp_exp = (noviceTotalExp - LEVEL_TO_EXP_NUM(levle))*1.0 / (LEVEL_TO_EXP_NUM(levle + 1) - LEVEL_TO_EXP_NUM(levle));
	}
	float temp_rep = 0;
	if (repLv < LEVEL_MAX_NUM)
	{
		temp_rep = (noviceTotalRep - LEVEL_TO_FAME_NUM(repLv))*1.0 / (LEVEL_TO_FAME_NUM(repLv + 1) - LEVEL_TO_FAME_NUM(repLv));
	}
	Widget* panel_exp = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_addexp, "panel_exp"));
	panel_exp->setContentSize(Size(panel_exp->getContentSize().width, 120 * temp_exp));
	Widget* panel_rep = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_addexp, "panel_rep"));
	panel_rep->setContentSize(Size(panel_rep->getContentSize().width, 120 * temp_rep));

	std::string s_exp = StringUtils::format("%+d", 240);//加的经验
	std::string s_supply = StringUtils::format("%+d", 0);
	std::string s_coin = StringUtils::format("%+d", 3000);
	std::string gift_name = "button_equip_items";
	label_lv->setString(StringUtils::format("%d", levle));
	label_lv_r->setString(StringUtils::format("%d", repLv));
	label_r_num->setString(StringUtils::format("%+d", 9));//加的声望值
	label_exp_num->setString(s_exp);
	label_coin_num->setString(s_coin);
	label_supply_num->setString(s_supply);

	auto image_item = view->getChildByName<ImageView*>("image_items");
	auto panel_ship = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_3"));
	auto t_no = panel_ship->getChildByName<Text*>("text_content");
	t_no->setVisible(true);

	auto l_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_content"));
	auto panel_items = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_4"));
	auto t_no2 = panel_items->getChildByName<Text*>("text_content");
	t_no2->setVisible(false);
	auto l_item = panel_items->getChildByName<ListView*>("list_item");
	auto item = image_item->clone();
	auto i_item = item->getChildByName<ImageView*>("image_goods");
	auto t_num = item->getChildByName<Text*>("text_item_num");
	i_item->setVisible(true);
	t_num->setVisible(true);
	int goodid = 66;
	i_item->loadTexture(getItemIconPath(goodid));
	i_item->setTag(goodid);
	setTextSizeAndOutline(t_num, 1);
	setBgButtonFormIdAndType(item, goodid, ITEM_TYPE_PORP);
	l_item->pushBackCustomItem(item);
	l_item->setPositionX(l_item->getPositionX() + 8 * 63);

	//引导的指示
	auto warResult = view->getChildByName<Button*>("button_continue_1");
	auto handpic = Sprite::create();
	handpic->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	handpic->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
	warResult->addChild(handpic, 10);
	handpic->setCameraMask(4, true);
	handpic->setLocalZOrder(1000);
	//小手在按钮左上角
	handpic->setPosition(Vec2(warResult->getContentSize().width / 2 - handpic->getContentSize().width / 2, warResult->getContentSize().height / 2 * 1.6 + handpic->getContentSize().height / 2 * 0.6));

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 3);
	addListViewBar(l_content, image_pulldown);
	view->setCameraMask(4);
}

void UIBattleHUD::openViewPirateAttack()
{
	openView(MAPUI_COCOS_RES[INDEX_UI_BATTLERRESULT_SCENE_PIRATA_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BATTLERRESULT_SCENE_PIRATA_CSB]);
	auto i_victory = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_title_victory"));
	auto t_feat = view->getChildByName<Text*>("label_loots_0_0");
	auto panel_baltic = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_baltic"));
	auto t_times = panel_baltic->getChildByName<Text*>("label_cost_num");
	auto panel_fleet = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_fleet"));
	auto t_ship_num = panel_fleet->getChildByName<Text*>("label_ship_num");
	auto t_crew_num = panel_fleet->getChildByName<Text*>("label_silver_num");
	t_feat->setString(StringUtils::format("%d", m_pEndFightResult->presentscore));
	t_times->setString(StringUtils::format("%d", m_pEndFightResult->defensepiratetimes));
	t_ship_num->setString(StringUtils::format("%d", m_pEndFightResult->n_survivedships));
	t_crew_num->setString(StringUtils::format("%d", m_pEndFightResult->sailorcount));

	auto b_continue = view->getChildByName<Button*>("button_continue_fight");
	auto b_leave = view->getChildByName<Button*>("button_leave");
	auto image_bg = view->getChildByName<ImageView*>("image_bg");
	auto listview_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "listview_content"));
	auto image_div_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "image_div_2"));
	listview_content->setTouchEnabled(false);
	std::string path;
	switch (m_pEndFightResult->reason)
	{
	case BATTLE_WIN:
		path = WAR_VICTORY[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DEFEATED:
		path = WAR_DEFEATED[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DRAW:
		path = WAR_DRAW[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_FLEE:
		path = WAR_FLEE[SINGLE_SHOP->L_TYPE];
		break;
	default:
		break;
	}
	i_victory->loadTexture(path);

	if (m_pEndFightResult->reason != BATTLE_WIN)
	{
		panel_fleet->setVisible(false);
		b_continue->setVisible(false);
		b_leave->setPositionX(image_bg->getPositionX());
		image_div_2->setVisible(false);
		listview_content->setPositionY(listview_content->getPositionY() - panel_baltic->getBoundingBox().size.height);
	}
}

void UIBattleHUD::openViewAttackPirate()
{
	openView(MAPUI_COCOS_RES[INDEX_UI_BATTLERRESULT_SCENE_SIEGE_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BATTLERRESULT_SCENE_SIEGE_CSB]);
	auto t_damages_num = view->getChildByName<Text*>("label_damages_num");
	t_damages_num->setString(StringUtils::format("%d", m_pEndFightResult->hurtpirateallvalue));
}

void UIBattleHUD::flushUserDefault(TVBasicShip *ob_A)
{
	if (ob_A && ob_A->m_Info->_isFriend)
	{
		auto st_sailor = StringUtils::format(MY_SHIP_SAILOR, ob_A->m_Info->_tag);
		auto st_blood = StringUtils::format(MY_SHIP_BLOOD, ob_A->m_Info->_tag);
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), ob_A->m_Info->_sailornum);
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), int(ob_A->getBlood()));
	}

	if (ob_A && !ob_A->m_Info->_isFriend)
	{
		auto st_sailor = StringUtils::format(ENEMY_SHIP_SAILOR, ob_A->m_Info->_tag - 5);
		auto st_blood = StringUtils::format(ENEMY_SHIP_BLOOD, ob_A->m_Info->_tag - 5);
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), ob_A->m_Info->_sailornum);
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), int(ob_A->getBlood()));
	}
	UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(MY_SHIP_MORALE).c_str(), m_pDelegate->getShareObj()->m_nMorale);
	UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(ENEMY_SHIP_MORALE).c_str(), m_pDelegate->getShareObj()->m_nMorale_Enemy);
	UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(FIGHT_TIME).c_str(), m_RemainTime);
	UserDefault::getInstance()->flush();
}
void UIBattleHUD::flushRobBattleResult(int winflag, EndFightResult* result)
{

	openView(MAPUI_COCOS_RES[INDEX_UI_ROB_BATTLE_RESULT_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_ROB_BATTLE_RESULT_CSB]);
	auto panel_title = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_title"));
	auto image_title_victory_0 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title, "image_title_victory_0"));
	std::string path;
	switch (winflag)
	{
	case BATTLE_WIN:
	path = ROB_WAR_VICTORY[SINGLE_SHOP->L_TYPE];
	break;
	case BATTLE_DEFEATED:
	path = ROB_WAR_DEFEATED[SINGLE_SHOP->L_TYPE];
	break;
	case BATTLE_DRAW:
	path = ROB_WAR_DRAW[SINGLE_SHOP->L_TYPE];
	break;
	case BATTLE_FLEE:
	path = ROB_WAR_FLEE[SINGLE_SHOP->L_TYPE];
	break;
	default:
	break;
	}
	image_title_victory_0->loadTexture(path);

	for (int i = 0; i<3;i++)
	{
		auto btn_star = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_title, StringUtils::format("button_star_%d", i)));
		btn_star->setBright(false);
	}
	for (int i = 0; i<result->starforloot; i++)
	{
		auto btn_star = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_title, StringUtils::format("button_star_%d", i)));
		btn_star->setBright(true);
	}
	//银币
	auto panel_coin = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted"));
	auto label_coin_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_coin, "label_hosted_num"));
	auto label_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_coin, "label_hosted"));
	label_coin->setString(SINGLE_SHOP->getTipsInfo()["TIP_ROB_BATTLE_COIN_TITLE"]);
	auto i_skill = panel_coin->getChildByName<ImageView*>("image_skill_bg_2_2");
	auto t_skill = i_skill->getChildByName<Text*>("text_item_skill_lv");
	if (result->skill_plunder_master > 0 || result->skill_stealing_from_thieves > 0)
	{
		i_skill->setVisible(true);
		i_skill->setTouchEnabled(true);
		i_skill->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::menuCall_func, this));
		if (result->skill_plunder_master > 0)
		{
			i_skill->loadTexture(getSkillIconPath(SKILL_PREDATORY_MASTE, SKILL_TYPE_PLAYER));
			i_skill->setTag(SKILL_PREDATORY_MASTE);
			setTextSizeAndOutline(t_skill, result->skill_plunder_master);
		}
		else
		{
			i_skill->loadTexture(getSkillIconPath(SKILL_BLACK_EAT_BLACK, SKILL_TYPE_PLAYER));
			i_skill->setTag(SKILL_BLACK_EAT_BLACK);
			setTextSizeAndOutline(t_skill, result->skill_stealing_from_thieves);
		}
		label_coin_num->setPositionX(i_skill->getPositionX() - i_skill->getBoundingBox().size.width / 2);
	}
	else
	{
		i_skill->setVisible(false);
	}
	std::string s_coin = StringUtils::format("+%lld", result->lootcoins + result->extralootcoins);
	label_coin_num->setString(s_coin);
	//水手币
	auto panel_sailorcoins = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_0"));
	auto label_hosted = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_sailorcoins, "label_hosted"));
	label_hosted->setString(SINGLE_SHOP->getTipsInfo()["TIP_ROB_BATTLE_SAILORCOINS_TITLE"]);
	auto label_supply_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_sailorcoins, "label_hosted_num"));
	std::string m_sailorcoins = StringUtils::format("+%d", result->addsailorcoinsforloot);
	label_supply_num->setString(m_sailorcoins);

	struct lootItem
	{
		int id;
		int type;
		int count;
	};
	std::vector<lootItem> lootItems;
	for (int i = 0; i < result->n_lootitemsarray; i++){
		lootItem good;
		good.id = result->lootitemsarray[i]->itemid;
		good.type = result->lootitemsarray[i]->itemtype;
		good.count = result->lootitemsarray[i]->count;
		lootItems.push_back(good);
	}

	auto panel_items = view->getChildByName<Widget*>("panel_items");
	for (int i = 0; i < 6; i++)
	{
		std::string st_item = StringUtils::format("image_items_%d", i + 1);
		auto i_item_bg = panel_items->getChildByName<ImageView*>(st_item);
		auto i_item = i_item_bg->getChildByName<ImageView*>("image_goods");
		auto t_num = i_item_bg->getChildByName<Text*>("text_item_num");
		i_item_bg->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::showItemInfo, this));
		if (i < lootItems.size())
		{
			std::string name;
			std::string path;
			getItemNameAndPath(lootItems[i].type, lootItems[i].id, name, path);
			i_item->setVisible(true);
			t_num->setVisible(true);
			i_item_bg->setTouchEnabled(true);
			i_item_bg->setTag(lootItems[i].type);
			i_item->loadTexture(path);
			i_item->setTag(lootItems[i].id);
			setTextSizeAndOutline(t_num, lootItems[i].count);
			setBgButtonFormIdAndType(i_item_bg, lootItems[i].id, lootItems[i].type);
		}
		else
		{
			i_item->setVisible(false);
			t_num->setVisible(false);
			i_item_bg->setTouchEnabled(false);
		}
	}

	auto panel_ship = view->getChildByName<Widget*>("panel_ship");
	for (int i = 0; i < 5; i++)
	{
		std::string st_item = StringUtils::format("image_items_%d", i + 1);
		auto i_item_bg = panel_ship->getChildByName<ImageView*>(st_item);
		auto i_item = i_item_bg->getChildByName<ImageView*>("image_goods");
		auto t_num = i_item_bg->getChildByName<Text*>("text_item_num");
		t_num->setVisible(false);
		i_item_bg->addTouchEventListener(CC_CALLBACK_2(UIBattleHUD::showShipInfo, this));
		if (i < result->n_catchshipdefine)
		{
			i_item->setVisible(true);
			i_item_bg->setTouchEnabled(true);
			i_item_bg->setTag(i);
			i_item->setTag(result->catchshipdefine[i]->sid);
			i_item->loadTexture(getShipIconPath(result->catchshipdefine[i]->sid));
			setBgButtonFormIdAndType(i_item_bg, result->catchshipdefine[i]->sid, ITEM_TYPE_SHIP);
		}
		else
		{
			i_item->setVisible(false);
			i_item_bg->setTouchEnabled(false);
			setGLProgramState(i_item_bg, true);
		}
	}

	//chengyuan++ 战斗结束后损坏装备提示框
	if (result->brokenequips && result->n_brokenequips > 0)
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushEquipBrokenView(result->brokenequips, result->n_brokenequips);
	}
}
