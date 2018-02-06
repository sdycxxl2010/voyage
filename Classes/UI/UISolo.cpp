#include "UISolo.h"
#include "TVBattleManager.h"
std::string animationName[] =
{
	"Standby",
	"walk",
	"attack",
	"Damaged",
	"Dodge",
	"Parrying",
	"Death1",
	"Death2",
	"Rage",
};
UISolo::UISolo()
{
	m_player = nullptr;
	m_enemy = nullptr;
	m_playerAttacted = false;
	m_shipA = nullptr;
	m_shipB = nullptr;
	m_touchTimes = 0;
	playerInfo.attack = 100;
	playerInfo.defence = 20;
	playerInfo.hp = 1000;
	enemyInfo.attack = 100;
	enemyInfo.defence = 20;
	enemyInfo.hp = 1000;
	m_battleIndex = 0;
	m_battleComplete = 0;
}
UISolo::~UISolo()
{

}
bool UISolo::init()
{
	if (!UIBasicLayer::init())
	{
		return false;
	}
	registerCallBack();
#if 1
	initSoloView();
#endif
	return true;
}
UISolo * UISolo::createLayer()
{
	UISolo * solo = new UISolo;
	if (solo && solo->init())
	{
		solo->autorelease();
		return solo;
	}
	CC_SAFE_DELETE(solo);
	return nullptr;
}
Scene * UISolo::createScene()
{
	auto scene = Scene::create();
	auto layer = UISolo::createLayer();
	scene->addChild(layer);
	return scene;
}
void UISolo::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = dynamic_cast<Button*>(pSender);
	auto name = button->getName();
	if (isButton(button_ship))
	{	
		auto sc = _director->getRunningScene();
		auto manage = dynamic_cast<TVBattleManager*>(sc->getChildByName("BattleManage"));
		manage->soloResult(m_shipA, m_shipB);
		manage->startBattle(0);
		this->removeFromParentAndCleanup(true);
		return;		
//		battleAnimation(m_touchTimes);
		m_touchTimes += 1;

// 		if (m_touchTimes > 4)
// 		{
// 			auto sc = _director->getRunningScene();
// 			auto manage = dynamic_cast<BattleManage*>(sc->getChildByName("BattleManage"));
// 			manage->soloResult(m_shipA, m_shipB);
// 			manage->startBattle(0);
// 			this->removeFromParentAndCleanup(true);
// 			return;
// 		}
//		if (!m_playerAttacted)
//		{
// 		    m_player->clearTracks();
// 			m_enemy->clearTracks();
// 			m_enemy->setToSetupPose();
// 			m_player->setToSetupPose();
// 			m_enemy->setLocalZOrder(2);
// 			m_player->setLocalZOrder(1);
//			m_enemy->setMix(animationName[STANDBY],animationName[DAMAGED],1);
// 		    m_enemy->setMix(animationName[DAMAGED], animationName[STANDBY],1);
// 			m_player->addAnimation(0, animationName[DAMAGED], false, 2);
// 			m_player->addAnimation(0, animationName[STANDBY], true);
// 			m_enemy->setMix(animationName[STANDBY], animationName[ATTACK], 0);
// 			m_enemy->addAnimation(0, animationName[ATTACK], false);
// 			m_enemy->addAnimation(0, animationName[STANDBY], true);
// 			m_enemy->runAction(Sequence::create(DelayTime::create(0.5), CallFunc::create([=]{m_enemy->addAnimation(0, animationName[DAMAGED], false);
// 			}), CallFunc::create([=]{ m_enemy->addAnimation(0, animationName[STANDBY], true); }), nullptr));
// 			m_player->runAction(Sequence::create(CallFunc::create([=]{m_player->addAnimation(0, animationName[ATTACK], false); }), CallFunc::create([=]{ m_player->addAnimation(0, animationName[STANDBY], false); }), nullptr));
//			m_playerAttacted = true;
// 		}
// 		else
// 		{
// 			m_enemy->stopAllActions();
// 			m_player->stopAllActions();
// 			m_enemy->setToSetupPose();
// 			m_player->setToSetupPose();
// 			m_enemy->setLocalZOrder(2);
// 			m_player->setLocalZOrder(1);
// 			m_player->setMix(animationName[STANDBY], animationName[DAMAGED], 0);
// 			m_player->addAnimation(0, animationName[DAMAGED], false,0.5);
// 			m_player->addAnimation(0, animationName[STANDBY], true);
// 			m_enemy->setMix(animationName[STANDBY], animationName[ATTACK], 0);
// 			m_enemy->addAnimation(0, animationName[ATTACK], false);
// 			m_enemy->addAnimation(0, animationName[STANDBY], true);
// // 			m_enemy->runAction(Sequence::create(CallFunc::create([=]{m_enemy->addAnimation(0, animationName[ATTACK], false); }), CallFunc::create([=]{ m_enemy->addAnimation(0, animationName[STANDBY], true); }), nullptr));
// // 			m_player->runAction(Sequence::create(DelayTime::create(0.5), CallFunc::create([=]{m_player->addAnimation(0, animationName[DAMAGED], false);
// // 			}), CallFunc::create([=]{m_player->addAnimation(0, animationName[STANDBY], true); }), nullptr));
// 			m_playerAttacted = false;
//		}

		return;
	}
}
void UISolo::onServerEvent(struct ProtobufCMessage* message, int msgType)
{

}
void UISolo::initSoloView()
{
	openView(MAPUI_COCOS_RES[INDEX_UI_BATTLE_BOARD]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BATTLE_BOARD]);
	auto i_own = Helper::seekWidgetByName(view, "image_own");
	i_own->setVisible(false);
	auto i_enemy = Helper::seekWidgetByName(view, "image_enemy");
	i_enemy->setVisible(false);
	auto ske1 = spine::SkeletonAnimation::createWithFile("test/test.json", "test/test.atlas", 0.22);
	auto ske2 = spine::SkeletonAnimation::createWithFile("test/test.json", "test/test.atlas", 0.22);
	ske1->setCameraMask(4,true);
	ske2->setCameraMask(4, true);
	ske1->addAnimation(0, animationName[WALK], true);
	ske2->addAnimation(0, animationName[WALK], true);
	ske1->setPosition(i_own->getWorldPosition() - Vec2(0,i_own->getContentSize().height/2));
	ske2->setPosition(i_enemy->getWorldPosition() - Vec2(0,i_enemy->getContentSize().height/2));
	ske2->setScaleX(-1);
	view->addChild(ske1,2);
	view->addChild(ske2,1);
	m_player = ske1;
	m_enemy = ske2;
	auto p_enemy = view->getChildByName<Widget*>("panel_enemy");
	auto p_own = view->getChildByName<Widget*>("panel_own");
	auto l_enemy = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(p_enemy, "loadingbar_duable"));
	auto e_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_enemy, "text_name"));
	auto o_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_own, "text_name"));
	auto e_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_enemy, "image_head_16"));
	auto o_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_own, "image_head_16"));
	auto t_eLevel= dynamic_cast<Text*>(Helper::seekWidgetByName(p_enemy, "label_lv"));
	auto t_oLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(p_own, "label_lv"));
	l_enemy->setScaleX(-1);
	
// 	if (m_shipA != nullptr && m_shipB != nullptr)
// 	{
// 		auto infoA = dynamic_cast<BasicShip*>(m_shipA)->getBattleShipInfo();
// 		auto infoB = dynamic_cast<BasicShip*>(m_shipB)->getBattleShipInfo();
// //		e_name->setString()
// 
// 	}
// 	else
// 	{
// 		return;
// 	}

	initAnimations();
	battleBegin();
}
void UISolo::battleBegin()
{
	float n_distance = m_enemy->getPositionX() - m_player->getPositionX();
	Vec2 v_mPoint = Vec2(_director->getWinSize().width/2, _director->getWinSize().height/2);
	float n_pMove = v_mPoint.x - m_player->getBoundingBox().size.width/2 - m_player->getPositionX() - 115;
	float n_eMove = m_enemy->getPositionX() - m_enemy->getBoundingBox().size.width / 2 - v_mPoint.x - 115;
	m_player->runAction(Sequence::createWithTwoActions(MoveBy::create(1.5, Vec2(n_pMove, 0)), CallFunc::create([=]{m_player->addAnimation(0, animationName[STANDBY], true); m_player->setCompleteListener(CC_CALLBACK_2(UISolo::skeleEndListener, this)); m_player->setStartListener(CC_CALLBACK_1(UISolo::skeleStartListener, this)); })));
	m_enemy->runAction(Sequence::createWithTwoActions(MoveBy::create(1.5, Vec2(-n_eMove, 0)), CallFunc::create([=]{m_enemy->addAnimation(0, animationName[STANDBY], true); m_enemy->setCompleteListener(CC_CALLBACK_2(UISolo::skeleEndListener, this)); m_enemy->setStartListener(CC_CALLBACK_1(UISolo::skeleStartListener, this)); })));
//	this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.5), CallFunc::create([=]{battle(0)})))
//	this->schedule(schedule_selector(SoloLayer::battle), 2.5);
}
void UISolo::battleAnimation(int index, int aniIndex)
{
// 	m_enemy->clearTracks();
// 	m_enemy->setToSetupPose();
// 	m_player->clearTracks();
// 	m_player->setToSetupPose();
	int m_index = index % 10;
	m_enemy->addAnimation(0, animationName[STANDBY],true);
	m_player->addAnimation(0, animationName[STANDBY],true);
	switch (m_index)
	{
	case ENEMY_DAMAGED:
		m_enemy->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4), CallFunc::create([=]{
			m_enemy->clearTracks();
			m_enemy->setToSetupPose();
			m_enemy->addAnimation(0, animationName[DAMAGED], false);
			m_enemy->addAnimation(0, animationName[STANDBY], true); 
//			hurt(m_enemy, aniIndex); 
		})));
// 		m_enemy->addAnimation(0, animationName[STANDBY], true, 0.1);
// 		m_enemy->addAnimation(0, animationName[DAMAGED], false);
// 		m_enemy->addAnimation(0, animationName[STANDBY], true);
		break;
	case ENEMY_DODGE:
		m_enemy->runAction(Sequence::createWithTwoActions(DelayTime::create(0.2), CallFunc::create([=]{
			m_enemy->clearTracks();
			m_enemy->setToSetupPose();
			m_enemy->addAnimation(0, animationName[DODGE], false);
			m_enemy->addAnimation(0, animationName[STANDBY], true);
		})));
// 		m_enemy->addAnimation(0, animationName[STANDBY], true, 0.1);
// 		m_enemy->addAnimation(0, animationName[DODGE], false);
// 		m_enemy->addAnimation(0, animationName[STANDBY], true);
		break;
	case ENEMY_PARRYING:
		m_enemy->runAction(Sequence::createWithTwoActions(DelayTime::create(0.2), CallFunc::create([=]{
			m_enemy->clearTracks();
			m_enemy->setToSetupPose();
			m_enemy->addAnimation(0, animationName[PARRYING], false);
			m_enemy->addAnimation(0, animationName[STANDBY], true);
//			hurt(m_enemy, aniIndex);
		})));
// 		m_enemy->addAnimation(0, animationName[STANDBY], true, 0.1);
// 		m_enemy->addAnimation(0, animationName[PARRYING], false);
// 		m_enemy->addAnimation(0, animationName[STANDBY], true);
		break;
	case ENEMY_DEATH:
		m_enemy->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4), CallFunc::create([=]{
			m_enemy->clearTracks();
			m_enemy->setToSetupPose();
			m_enemy->addAnimation(0, animationName[DEATH_2], false);
//			hurt(m_enemy, aniIndex); 
		})));
// 		m_enemy->addAnimation(0, animationName[STANDBY], true, 0.1);
// 		m_enemy->addAnimation(0, animationName[DEATH_2], false);
		break;
	case ENEMY_BEATEN:
		m_enemy->addAnimation(0, animationName[STANDBY], true);
		m_enemy->runAction(Sequence::createWithTwoActions(DelayTime::create(2), CallFunc::create([=]{
			m_enemy->clearTracks();
			m_enemy->setToSetupPose();
			m_enemy->addAnimation(0, animationName[DEATH_2], false);
		})));

// 		m_enemy->addAnimation(0, animationName[STANDBY], true, 3.8);
// 		m_enemy->addAnimation(0, animationName[DEATH_2], false);
		break;
	case PLAYER_DAMAGED:
		m_player->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4), CallFunc::create([=]{
			m_player->clearTracks();
			m_player->setToSetupPose();
			m_player->addAnimation(0, animationName[DAMAGED], false); 
			m_player->addAnimation(0, animationName[STANDBY], true); 
//			hurt(m_player, aniIndex); 
		})));
// 		m_player->addAnimation(0, animationName[STANDBY], true, 0.1);
// 		m_player->addAnimation(0, animationName[DAMAGED], false);
// 		m_player->addAnimation(0, animationName[STANDBY], true);
		break;
	case PLAYER_DODGE:
		m_player->runAction(Sequence::createWithTwoActions(DelayTime::create(0.2), CallFunc::create([=]{
			m_player->clearTracks();
			m_player->setToSetupPose();
			m_player->addAnimation(0, animationName[DODGE], false); 
			m_player->addAnimation(0, animationName[STANDBY], true);
		})));
// 		m_player->addAnimation(0, animationName[STANDBY], true, 0.1);
// 		m_player->addAnimation(0, animationName[DODGE], false);
// 		m_player->addAnimation(0, animationName[STANDBY], true);
		break;
	case PLAYER_PARRYING:
		m_player->runAction(Sequence::createWithTwoActions(DelayTime::create(0.2), CallFunc::create([=]{
			m_player->clearTracks();
			m_player->setToSetupPose();
			m_player->addAnimation(0, animationName[PARRYING], false);
			m_player->addAnimation(0, animationName[STANDBY], true); 
//			hurt(m_player,aniIndex);
		})));
// 		m_player->addAnimation(0, animationName[STANDBY], true, 0.1);
// 		m_player->addAnimation(0, animationName[PARRYING], false);
// 		m_player->addAnimation(0, animationName[STANDBY], true);
		break;
	case PLAYER_DEATH:
		m_player->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4), CallFunc::create([=]{
			m_player->clearTracks();
			m_player->setToSetupPose();
			m_player->addAnimation(0, animationName[DEATH_2], false); 
//			hurt(m_player, aniIndex);
		})));
// 		m_player->addAnimation(0, animationName[STANDBY], true, 0.1);
// 		m_player->addAnimation(0, animationName[DEATH_2], false);
		break;
	case PLAYER_BEATEN:
		m_player->addAnimation(0, animationName[STANDBY], true);
    	m_player->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0), CallFunc::create([=]{
			m_player->clearTracks();
			m_player->setToSetupPose();
			m_player->addAnimation(0, animationName[DEATH_2], false);
		})));
// 		m_player->clearTracks();
// 		m_player->setToSetupPose();
// 		m_player->addAnimation(0, animationName[STANDBY], true, 3.8);
// 		m_player->addAnimation(0, animationName[DEATH_2], false);
		break;
	default:
		break;
	}
	if (m_index <= 4)
	{
		m_enemy->setLocalZOrder(1);
		m_player->setLocalZOrder(2);
		m_player->clearTracks();
		m_player->setToSetupPose();
		if (m_index == 4)
		{
			m_player->addAnimation(0, animationName[ULTIMATE], false);
		}
		else
		{
			m_player->addAnimation(0, animationName[ATTACK], false);
		}
		m_player->addAnimation(0, animationName[STANDBY], true);
	}
	else
	{
		m_enemy->clearTracks();
		m_enemy->setToSetupPose();
		m_enemy->setLocalZOrder(2);
		m_player->setLocalZOrder(1);
//		m_enemy->setMix(animationName[STANDBY], animationName[ATTACK], 0);
		if (m_index == 9)
		{
			m_enemy->addAnimation(0, animationName[ULTIMATE], false);
		}
		else
		{
			m_enemy->addAnimation(0, animationName[ATTACK], false);
		}
		m_enemy->addAnimation(0, animationName[STANDBY], true);
	}

}
void UISolo::setBattleShip(Node * shipA, Node * shipB)
{
	m_shipA = shipA;
	m_shipB = shipB;
//	initSoloView();
}
void UISolo::animationStateEvent()
{

}
void UISolo::hurt(Node * psender, int index)
{
	auto aniIndex = m_actions[m_battleIndex -1];
	log("BattleIndex::%d", index);
	log("aniIndex:: %d", aniIndex.at(1));
	log("aniHP:: %d", aniIndex.at(0));
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BATTLE_BOARD]);
	auto p_own = view->getChildByName<Widget*>("panel_own");
	auto p_enemy = view->getChildByName<Widget*>("panel_enemy");
	auto t_damage = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "text_damage"));
	if (aniIndex.at(1) >= PLAYER_DAMAGED)
	{
		auto l_own = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(p_own, "loadingbar_duable"));
		float a = aniIndex.at(0) * 1.0 / 1000;
		l_own->setPercent(a * 100);
	}
	else
	{
		auto l_enemy = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(p_enemy, "loadingbar_duable"));
		float b = aniIndex.at(0) * 1.0 / 1000;
		l_enemy->setPercent(b * 100);
	}
}

void UISolo::initAnimations()
{
	int index = 0;
	bool playerAttacked = true;
	srand(time(0));
	while (playerInfo.hp > 0  && enemyInfo.hp > 0 )
	{	
		int a = 0;
		if (playerAttacked)
		{
			a = rand() % 30;
			playerAttacked = false;
		}
		else
		{
			a = rand() % 30 + 30;
			playerAttacked = true;
		}
		if (a <10)
		{

			enemyInfo.hp -= playerInfo.attack;
			std::vector<int> v_action;
			v_action.push_back(enemyInfo.hp);
			if (enemyInfo.hp <= 0)
			{
				v_action.push_back(ENEMY_BEATEN);
			}
			else{
				v_action.push_back(ENEMY_DAMAGED);
			}
			m_actions.insert(std::pair<int, std::vector<int>>(index, v_action));
		}else if (a <20)
		{
			std::vector<int> v_action;
			v_action.push_back(enemyInfo.hp);
			v_action.push_back(ENEMY_DODGE);
			m_actions.insert(std::pair<int, std::vector<int>>(index, v_action));
		}
		else if (a <30)
		{
			enemyInfo.hp -= playerInfo.attack / 2; 
			std::vector<int> v_action;
			v_action.push_back(enemyInfo.hp);
			if (enemyInfo.hp <= 0)
			{
				v_action.push_back(ENEMY_BEATEN);
			}
			else{
				v_action.push_back(ENEMY_PARRYING);
			}
			m_actions.insert(std::pair<int, std::vector<int>>(index, v_action));
		}
		else if (a <40)
		{
			playerInfo.hp -= enemyInfo.attack;
			std::vector<int> v_action;
			v_action.push_back(playerInfo.hp);
			if (playerInfo.hp <= 0)
			{
				v_action.push_back(PLAYER_BEATEN);
			}
			else{
				v_action.push_back(PLAYER_DAMAGED);
			}
			m_actions.insert(std::pair<int, std::vector<int>>(index, v_action));
		}
		else if (a <50)
		{
			std::vector<int> v_action;
			v_action.push_back(enemyInfo.hp);
			v_action.push_back(PLAYER_DODGE);
			m_actions.insert(std::pair<int, std::vector<int>>(index, v_action));
		}
		else if (a < 60)
		{
			playerInfo.hp -= enemyInfo.attack / 2;
			std::vector<int> v_action;
			v_action.push_back(playerInfo.hp);
			if (playerInfo.hp <= 0)
			{
				v_action.push_back(PLAYER_BEATEN);
			}
			else{
				v_action.push_back(PLAYER_PARRYING);
			}
			m_actions.insert(std::pair<int, std::vector<int>>(index, v_action));
		}
		index++;
	}

}
void UISolo::battle(float dt)
{
	if (m_battleIndex <= m_actions.size()-1)
	{
		battleAnimation(m_actions[m_battleIndex].at(1),m_battleIndex);
		m_battleIndex++;
	}
}
void UISolo::skeleEndListener(int trackIndex, int loopCount)
{
//	log("trackIndex::%d , loopCount::%d", trackIndex, loopCount);
// 	m_battleComplete++;
// 	if (m_battleIndex == m_actions.size() -1)
// 	{
// 		log("hehe");
// 	}
// 	if (m_battleComplete == 2 && m_battleIndex < m_actions.size() && trackIndex )
// 	{
// //		battle(0);
// 		m_battleComplete = 0;
// 	}
	spTrackEntry*  p_current = m_player->getCurrent();
	spTrackEntry*  e_current = m_enemy->getCurrent();
	if (p_current == nullptr || e_current == nullptr)
	{
		return;
	}
	auto p_aniName = p_current->animation->name;
	auto e_aniName = e_current->animation->name;
	if (strcmp(p_aniName, animationName[DAMAGED].c_str()) == 0 || strcmp(p_aniName, animationName[PARRYING].c_str()) == 0 || strcmp(p_aniName, animationName[DEATH_2].c_str()) == 0)
	{
		hurt(m_player, m_battleIndex);
	}
	if (strcmp(e_aniName, animationName[DAMAGED].c_str()) == 0 || strcmp(e_aniName, animationName[PARRYING].c_str()) == 0 || strcmp(e_aniName, animationName[DEATH_2].c_str()) == 0)
	{
		hurt(m_enemy, m_battleIndex);
	}
}

void UISolo::skeleStartListener(int trackIndex)
{
// 	log("trackIndex::%d , loopCount::%d", trackIndex, loopCount);
// 	m_battleComplete++;
// 	if (m_battleIndex == m_actions.size() - 1)
// 	{
// 		log("hehe");
// 	}
// 	if (m_battleComplete == 2 && m_battleIndex < m_actions.size() && trackIndex)
// 	{
// 		//		battle(0);
// 		m_battleComplete = 0;
// 	}
	spTrackEntry*  p_current = m_player->getCurrent();
	spTrackEntry*  e_current = m_enemy->getCurrent();
	if (strcmp(p_current->animation->name, e_current->animation->name) == 0)
	{
		battle(0);
	}
}