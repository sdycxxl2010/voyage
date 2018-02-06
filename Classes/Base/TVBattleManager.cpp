/*
 * ok, the name is shit, it's the real battle layer!!!
 *
 */
#include "TVBattleManager.h"
#include "TVBattleShareObj.h"
#include "TVBattleShip.h"
#include "TVBattleMainShip.h"
#include "UISailManage.h"
#include "WriterLog.h"

#include "UINoviceStoryLine.h"
#include "UIGuideSailManage.h"
#include "TVSceneLoader.h"
#include "UISolo.h"
#define SOLO_TEST 0
class TouchMe : public Node
{
public:
	TouchMe(int flag):flag(flag){};
	~TouchMe(){};
	int flag;
	Sprite* m_Node;
	static TouchMe* create(int flag = 0)
	{
		TouchMe* ti = new TouchMe(flag);
		if (ti && ti->init())
		{
			ti->autorelease();
			return ti;
		}
		CC_SAFE_DELETE(ti);
		return nullptr;
	}
	bool init()
	{
		bool pRet = false;
		do 
		{
			m_Node = Sprite::create();
			m_Node->setVisible(true);
			this->addChild(m_Node);			
			pRet = true;
		} while (0);
		return pRet;
	}

	void start(Animation* ani)
	{
		this->setVisible(true);
		m_Node->stopAllActions();
		auto seq = Sequence::create(Animate::create(ani),CCCallFunc::create(CC_CALLBACK_0(TouchMe::removeIcon,this)),nullptr);
		m_Node->runAction(seq);
	}

	void removeIcon()
	{
		this->setVisible(false);
		//this->removeFromParent();
	}
};


TVBattleManager::TVBattleManager()
{
	m_SkillFlushTime = 0.f;
	_contactEvent = nullptr;
	Size mapSize = Director::getInstance()->getWinSize();
	m_Cmap = new CMap(mapSize.width * 3,mapSize.height * 3);
	m_StartGame = false;
	mActivieSkills = 0;
	mActivieSkillsNum = 0;
	for (int i = 0; i < 20; i++)
	{
		m_skillEffectTime[i] = -1;
	}

	m_SkillManage = nullptr;
	m_FightResult = nullptr;
	m_EffManage = nullptr;
	m_ShareObj = nullptr;
	m_UIlayer = nullptr;
	m_MapLayer = nullptr;
	UICamera = nullptr;
	/*
	Camera  *c  = Director::getInstance()->getRunningScene()->getCameras().front();
	c->setMoveFlag(true);
	m_cameraStartPostion = c->getPosition();
	*/
	m_atkbuffnum = 0;
	spriteatkEff = nullptr;


	/*
	//加入了转场不需要了
	AnimationCache::destroyInstance();
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	*/
}

TVBattleManager::~TVBattleManager()
{
	std::for_each(m_ShareObj->m_vMyFriends.begin(),m_ShareObj->m_vMyFriends.end(),[](Node* target)
	{
		target->release();
	});
	std::for_each(m_ShareObj->m_vEnemys.begin(),m_ShareObj->m_vEnemys.end(),[](Node* target)
	{
		target->release();
	});
	std::for_each(m_ShareObj->m_vDiedShips.begin(),m_ShareObj->m_vDiedShips.end(),[](Node* target)
	{
		target->release();
	});
	_userObject = nullptr;
	delete m_ShareObj;
	delete m_EffManage;
	delete m_SkillManage;
	delete m_Cmap;
	WriterLog::getInstance()->release();
	if (mActivieSkillsNum >= 0)
	{
		for (int i = 0; i < mActivieSkillsNum; i++)
		{
			delete mActivieSkills[i];
		}
		delete[]mActivieSkills;
	}

	if(m_FightResult) engage_in_fight_result__free_unpacked(m_FightResult,0);
}

TVBattleManager* TVBattleManager::createBattle(EngageInFightResult* result, Vec2 pos)
{
	if (!result)
	{
		return nullptr;
	}
	TVBattleManager *bm = new TVBattleManager;
	if (bm)
	{
		bm->setName("BattleManage");
		bm->m_ShipMapPos = pos;
		bm->m_FightResult = result;
		auto npcId = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
		//正常战斗(新手引导除外)
		if (npcId == result->npcid && result->fighttype == FIGHT_TYPE_NORMAL && !ProtocolThread::GetInstance()->isInTutorial())
		{
			bm->m_bContinueLastFight = true;
		}
		else
		{
			bm->m_bContinueLastFight = false;
		}
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), result->npcid);
		UserDefault::getInstance()->flush();
		bm->init();
		bm->autorelease();
		SINGLE_HERO->m_heroIsOnsea = true;
		return bm;
	}
	CC_SAFE_RELEASE(bm);
	return nullptr;
}

void TVBattleManager::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
}

bool TVBattleManager::init()
{
	m_ShareObj = new TVBattleShareObj;
	if (m_bContinueLastFight)
	{
		auto morale = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(MY_SHIP_MORALE).c_str(), 0);
		auto npcmorale = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(ENEMY_SHIP_MORALE).c_str(), 0);
		this->m_ShareObj->m_nMorale = morale;
		this->m_ShareObj->m_nMorale_Enemy = npcmorale;
	}
	else
	{
		this->m_ShareObj->m_nMorale = m_FightResult->morale;
		this->m_ShareObj->m_nMorale_Enemy = m_FightResult->npcmorale;
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(MY_SHIP_MORALE).c_str(), m_FightResult->morale);
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(ENEMY_SHIP_MORALE).c_str(), m_FightResult->npcmorale);
		UserDefault::getInstance()->flush();
	}
	m_SkillManage = new SkillManage;
	m_SkillManage->setShareObj(m_ShareObj);
	auto screenSize = Director::getInstance()->getVisibleSize() * BATTLE_SCALE_RATE;
	auto smallScreen = Director::getInstance()->getVisibleSize();
	m_MapLayer = Layout::create();
	m_MapLayer->setContentSize(screenSize);
	m_MapLayer->setScale(0.3333f);
	m_MapLayer->setPosition(Vec2(0,0));
	this->addChild(m_MapLayer,2);
	if (m_FightResult && m_FightResult->n_positiveskills > 0)
	{
		auto skillTrees = SINGLE_SHOP->getSkillTrees();
		mActivieSkillsNum = m_FightResult->n_positiveskills;
		mActivieSkills = new ActiveSkill*[mActivieSkillsNum];
		for (int i = 0; i < mActivieSkillsNum; i++)
		{
			mActivieSkills[i] = new ActiveSkill;
			mActivieSkills[i]->skillId = m_FightResult->positiveskills[i]->id;
			mActivieSkills[i]->skillLevel = m_FightResult->positiveskills[i]->level;
			auto& skillData = skillTrees[mActivieSkills[i]->skillId];
			mActivieSkills[i]->definedCdSeconds = skillData.cd - m_FightResult->positiveskills[i]->level*skillData.cdreduce_per_lv;
			mActivieSkills[i]->definedDurationSeconds = skillData.base_duration;
		}
	}
	return true;
}

void TVBattleManager::onEnter()
{
	Layer::onEnter();
	auto size = Director::getInstance()->getWinSize();
	UICamera = Camera::createOrthographic(size.width, size.height, -1024, 1024);
//	mainCamera->setPosition3D(Vec3(0,0,0));
	UICamera->setCameraFlag(CameraFlag::USER2);
	UICamera->setDepth(4);
	Camera  *c = Director::getInstance()->getRunningScene()->getCameras().front();
	c->setMoveFlag(true);
	m_cameraStartPostion = c->getPosition();

	this->setAnchorPoint(Vec2(0.5f, 0.5f));
	this->addChild(UICamera);

	loadStaticData(0);

	auto collisionListener = EventListenerPhysicsContact::create();
	collisionListener->onContactBegin = CC_CALLBACK_1(TVBattleManager::onContactBegain, this);
	collisionListener->onContactPostSolve = CC_CALLBACK_2(TVBattleManager::onContactPostSolve, this);
	collisionListener->onContactPreSolve = CC_CALLBACK_2(TVBattleManager::onContactPreSolve, this);
	collisionListener->onContactSeperate = CC_CALLBACK_1(TVBattleManager::onContactSeperate, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(collisionListener,this);
	auto listener = EventListenerTouchOneByOne::create();
	
	listener->onTouchBegan = CC_CALLBACK_2(TVBattleManager::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(TVBattleManager::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(TVBattleManager::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(TVBattleManager::onTouchCancelled, this);
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	_touchListener = listener;
	auto winSize = Director::getInstance()->getWinSize();

	CCLOG(" BattleManage onEnter 111 ");
	
	auto spa  = Spawn::createWithTwoActions(EaseQuarticActionInOut::create(MoveBy::create(2.f,Vec2(-winSize.width,-winSize.height))),EaseQuarticActionInOut::create(ScaleTo::create(2.f,1.f)));
	auto seq = Sequence::create(DelayTime::create(0.5f),spa,CCCallFunc::create(CC_CALLBACK_0(TVBattleManager::scaleEnd,this)),nullptr);
	TouchMe* ti = TouchMe::create(0);
	ti->retain();
	ti->setVisible(false);
	this->_userObject = ti;
	m_MapLayer->addChild(ti,3);
	m_MapLayer->runAction(seq);
}

void TVBattleManager::onExit()
{
	_userObject->release();
	_eventDispatcher->removeEventListener(_touchListener);
	_eventDispatcher->removeEventListener(_contactEvent);
	Layer::onExit();
}

void getRandomAnimation(int randomCount,Animation* ani)
{
	Vector<AnimationFrame*> &sprites = (Vector<AnimationFrame*>&)(ani->getFrames());
	for (int i = 0; i < randomCount; i++)
	{
		AnimationFrame* tempFrames = sprites.at(0);
		tempFrames->retain();
		sprites.erase(0);
		sprites.pushBack(tempFrames);
		tempFrames->release();
	}
}

void TVBattleManager::loadBackGround()
{
	int width = 200;
	int height = 200;
	int x = m_MapLayer->getContentSize().width / width;
	int y = m_MapLayer->getContentSize().height / height;

	Animation* animation = m_EffManage->getAnimation(PT_BATTLE_BG);
	auto animationSprites = animation->getFrames();
	for (int i = 0; i < x + 1; i++)
	{
		for (int j = 0; j < y + 1; j++)
		{
			int randomCount = cocos2d::random(0,119);
			Animation* animation_copy = animation->clone();
			getRandomAnimation(randomCount,animation_copy);
			auto battle_bg = Sprite::create();
			battle_bg->setAnchorPoint(Vec2(0,0));
			battle_bg->setPosition(Vec2(i * width,j * height));
			m_MapLayer->addChild(battle_bg,1);
			battle_bg->runAction(Animate::create(animation_copy));
		}
	}
	//m_MapLayer->scheduleUpdate();
}

void TVBattleManager::loadStaticData(float dt)
{
	auto screenSize = Director::getInstance()->getVisibleSize();
	auto x = screenSize.width;
	auto y = screenSize.height;
	//随机船只位置
	Vec2 Pos[] = { Vec2(1 * x / 2, 1 * y / 2), Vec2(1 * x / 4, 1 * y / 4), Vec2(3 * x / 4, 1 * y / 4), Vec2(1 * x / 4, 3 * y / 4), Vec2(3 * x / 4, 3 * y / 4),
		Vec2(1 * x / 2, 3 * y / 2), Vec2(1 * x / 4, 5 * y / 4), Vec2(3 * x / 4, 5 * y / 4), Vec2(1 * x / 4, 7 * y / 4), Vec2(3 * x / 4, 7 * y / 4),
		Vec2(1 * x / 2, 5 * y / 2), Vec2(1 * x / 4, 9 * y / 4), Vec2(3 * x / 4, 9 * y / 4), Vec2(1 * x / 4, 11 * y / 4), Vec2(3 * x / 4, 11 * y / 4),
		Vec2(3 * x / 2, 1 * y / 2), Vec2(5 * x / 4, 1 * y / 4), Vec2(7 * x / 4, 1 * y / 4), Vec2(5 * x / 4, 3 * y / 4), Vec2(7 * x / 4, 3 * y / 4),
		Vec2(3 * x / 2, 3 * y / 2), Vec2(5 * x / 4, 5 * y / 4), Vec2(7 * x / 4, 5 * y / 4), Vec2(5 * x / 4, 7 * y / 4), Vec2(7 * x / 4, 7 * y / 4),
		Vec2(3 * x / 2, 5 * y / 2), Vec2(5 * x / 4, 9 * y / 4), Vec2(7 * x / 4, 9 * y / 4), Vec2(5 * x / 4, 11 * y / 4), Vec2(7 * x / 4, 11 * y / 4),
		Vec2(5 * x / 2, 1 * y / 2), Vec2(9 * x / 4, 1 * y / 4), Vec2(11 * x / 4, 1 * y / 4), Vec2(9 * x / 4, 3 * y / 4), Vec2(11 * x / 4, 3 * y / 4),
		Vec2(5 * x / 2, 3 * y / 2), Vec2(9 * x / 4, 5 * y / 4), Vec2(11 * x / 4, 5 * y / 4), Vec2(9 * x / 4, 7 * y / 4), Vec2(11 * x / 4, 7 * y / 4),
		Vec2(5 * x / 2, 5 * y / 2), Vec2(9 * x / 4, 9 * y / 4), Vec2(11 * x / 4, 9 * y / 4), Vec2(9 * x / 4, 11 * y / 4), Vec2(11 * x / 4, 11 * y / 4)};

	int enemys_num = m_FightResult->n_enemyships;
	int friend_num = m_FightResult->n_myships;
	int j = 0;
	int n = 0;
	int m = 1;
	bool flag = false;
	int a[10] = {0, 1, 2, 3, 4 ,5 ,6 ,7, 8, 9};
	a[0] = 20;
	for (size_t i = 1; i < enemys_num + friend_num; i++)
	{
		flag = false;
		n = random(0,44);
		j = 0;
		while (j < m)
		{
			if (a[j] == n)
			{
				flag = true;
				i--;
				break;
			}
			else
			{
				j++;
			}
		}
		if (!flag)
		{
			a[j] = n;
			m++;
		}
	}
	
	//My
	for (int i = 0; i < friend_num; i++)
	{
		TVBasicShip* myFriend;
		auto shipInfo = m_FightResult->myships[i];
		//是否是上一场战斗没有结束
		auto sailornum = shipInfo->sailornum;
		auto bloodnum = shipInfo->current_hp;
		auto st_sailor = StringUtils::format(MY_SHIP_SAILOR, i + 1);
		auto st_blood = StringUtils::format(MY_SHIP_BLOOD, i + 1);
		if (m_bContinueLastFight)
		{
			sailornum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), 0);
			bloodnum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), 0);
		}
		else
		{
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), sailornum);
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), bloodnum);
		}
		if (sailornum > 0 && bloodnum > 0)
		{
			int type = getBattleShipType(m_FightResult->myships[i]->sid);
			if (i == 0)
			{
				myFriend = TVBattleMainShip::createShip(this, shipInfo, m_FightResult->myships[i]->position + 1, true, type, m_FightResult->fighttype, i);
				myFriend->setType(MAIN_SHIP);
				myFriend->setPosition(Pos[a[i]]);
				auto pybd = myFriend->getShipImage()->getPhysicsBody();
				if (pybd)
				{
					pybd->setCategoryBitmask(1);
					pybd->setContactTestBitmask(1);
				}
			}
			else
			{
				myFriend = TVBattleShip::createShip(this, shipInfo, m_FightResult->myships[i]->position + 1, true, type, m_FightResult->fighttype, i);
				myFriend->setType(FLOW_SHIP);
				auto pybd = myFriend->getShipImage()->getPhysicsBody();
				if (pybd)
				{
					pybd->setCategoryBitmask(1);
					pybd->setContactTestBitmask(1);
				}
				myFriend->setPosition(Pos[a[i]]);
			}
			myFriend->retain();
			myFriend->setDelegate(this);
			m_MapLayer->addChild(myFriend, 5);
			myFriend->setTag(i + 1);
			m_ShareObj->m_vMyFriends.push_back(myFriend);
		}
	}
	// Enemys
	for (int i = 0; i < enemys_num; i++)
	{
		auto enemyInfo = m_FightResult->enemyships[i];

		//是否是上一场战斗没有结束
		auto sailornum = enemyInfo->sailornum;
		auto bloodnum = enemyInfo->current_hp;
		auto st_sailor = StringUtils::format(ENEMY_SHIP_SAILOR, i + 1);
		auto st_blood = StringUtils::format(ENEMY_SHIP_BLOOD, i + 1);
		if (m_bContinueLastFight)
		{
			sailornum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), 0);
			bloodnum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), 0);
		}
		else
		{
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), sailornum);
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), bloodnum);
		}

		if (sailornum > 0 && bloodnum > 0)
		{
			int type = getBattleShipType(m_FightResult->enemyships[i]->sid);
			auto enemy = TVBattleShip::createShip(this, enemyInfo, i + 6, false, type, m_FightResult->fighttype, i);
			enemy->setChargeInitiative(m_FightResult->charge_initiative);
			auto consize = enemy->getContentSize();
			auto pybd = enemy->getShipImage()->getPhysicsBody();
			if (pybd)
			{
				pybd->setCategoryBitmask(1);
				pybd->setContactTestBitmask(1);
			}
			enemy->retain();
			enemy->setDelegate(this);
			m_MapLayer->addChild(enemy, 5);
			enemy->setTag(i + 6);
			enemy->setDirect(180.f);
			m_ShareObj->m_vEnemys.push_back(enemy);
			enemy->setPosition(Pos[a[i + friend_num]]);
			enemy->setType(NPC_SHIP);
		}
	}
	UserDefault::getInstance()->flush();
	m_EffManage = EffectManage::createEffect();
	m_UIlayer = UIBattleHUD::createBattleUI(m_FightResult, this);
	m_UIlayer->setDelegate(this);
	m_UIlayer->setTouchEnabled(false);
	m_UIlayer->setCameraMask(4);
	this->addChild(m_UIlayer, MAX_Z_ORDER + 1);
	loadBackGround();
}

void TVBattleManager::specialButtonEvent(cocos2d::Node* target,std::string name,float varValue)
{
	CCLOG("BattleManage  specialButtonEvent 111");
	if (isButton(event_unlockTarget))
	{
		//m_UIlayer->unLockFlag(target->getTag(),false);
		return;
	}
	if (isButton(event_lockTarget))
	{
		int time = -1;
		for (size_t i = 0; i < m_ShareObj->m_vEnemys.size(); i++)
		{
			if (m_ShareObj->m_vEnemys[i]->m_Info->_tag == target->getTag())
			{
				TVBattleShip* ship = dynamic_cast<TVBattleShip*>(m_ShareObj->m_vEnemys[i]);
				time = ship->getUnlockTime();
				if (time > 0)
				{
					ship->lockShip(false);
				}
				break;
			}
		}
		if (time > 0)
		{
			for (size_t i = 0; i < m_ShareObj->m_vMyFriends.size(); i++)
			{
				if (m_ShareObj->m_vMyFriends[i]->m_Info->_tag == 1)
				{
					TVBattleMainShip* ship = dynamic_cast<TVBattleMainShip*>(m_ShareObj->m_vMyFriends[i]);
					std::string str = SINGLE_SHOP->getTipsInfo()["TIP_FIGHTING_MAINSHIP_UNLOCK"];
					LabelTTF* label = LabelTTF::create(str, "", 25);
					label->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(2), FadeOut::create(0.5), RemoveSelf::create(true), nullptr));
					label->setPositionY(120);
					label->setColor(Color3B::RED);
					ship->addChild(label);
				}
			}
		}
		else
		{
			//m_UIlayer->unLockFlag(target->getTag(), true);
		}
		return;
	}
	if (isButton(event_die))
	{
		int tarTag = target->getTag();
		m_UIlayer->heroDie(tarTag);
		if (target->getTag() < 6)
		{
			for_each(m_ShareObj->m_vEnemys.begin(),m_ShareObj->m_vEnemys.end(),[&tarTag](TVBasicShip* ship)
			{
				ship->clearHateByTag(tarTag);
			});
		}else
		{
			for_each(m_ShareObj->m_vMyFriends.begin(),m_ShareObj->m_vMyFriends.end(),[&tarTag](TVBasicShip* ship)
			{
				ship->clearHateByTag(tarTag - 5);
			});
		}
		return;
	}
	if (isButton(event_attacked))
	{
		CCLOG("BattleManage specialButtonEvent event_attacked 111 ");
		m_UIlayer->heroAttacked(target->getTag(),varValue * 100);
		return;
	}
	if (isButton(event_anger))
	{
		
	}
}

void TVBattleManager::callbackEventByIndex(int tag)
{
	switch (tag)
	{
	case 0:
		{
			Director::getInstance()->getRunningScene()->getEventDispatcher()->removeEventListenersForType(EventListener::Type::TOUCH_ONE_BY_ONE);
			if (ProtocolThread::GetInstance()->isInTutorial())
			{
				Scene* sc = Scene::create();
				sc->addChild(UIGuideSailManage::create(m_ShipMapPos, true),1,MAP_TAG+1000);
				Director::getInstance()->replaceScene(sc);
			}
			else
			{
				Scene* sc = Scene::create();
				sc->addChild(UISailManage::create(m_ShipMapPos, 1),1,MAP_TAG+100);
				Director::getInstance()->replaceScene(sc);
			}
			break;
		}
	case MODEL_FREE://change mode to free
		{
			int num = m_ShareObj->m_vMyFriends.size();
			for (int i = 1; i < num; i++)
			{
				m_ShareObj->m_vMyFriends[i]->setMode(MODEL_FREE);
			}
			break;
		}
	case MODEL_PROTECT://change mode to protected
		{
			int num = m_ShareObj->m_vMyFriends.size();
			for (int i = 1; i < num; i++)
			{
				m_ShareObj->m_vMyFriends[i]->setMode(MODEL_PROTECT);
			}
			break;
		}
	case MODEL_FOCUS: // change mode to follow
		{
			int num = m_ShareObj->m_vMyFriends.size();
			for (int i = 1; i < num; i++)
			{
				m_ShareObj->m_vMyFriends[i]->setMode(MODEL_FOCUS);
			}
			break;
		}
	/*				
	case 1://change mode to free
		{
			int num = m_ShareObj->m_vMyFriends.size();
			if (num > 1)
			{
				auto type = m_ShareObj->m_vMyFriends[0]->getType();
				if (type == MAIN_SHIP)
				{
					for (int i = 1; i < num; i++)
					{
						m_ShareObj->m_vMyFriends[i]->setMode(MODEL_FREE);
					}
				}
			}
			break;
		}
	case 2://change mode to protected
		{
			int num = m_ShareObj->m_vMyFriends.size();
			if (num > 1)
			{
				auto type = m_ShareObj->m_vMyFriends[0]->getType();
				if (type == MAIN_SHIP)
				{
					for (int i = 1; i < num;i++)
					{
						m_ShareObj->m_vMyFriends[i]->setMode(MODEL_PROTECT);
					}
				}
			}
			break;
		}
	case 3: // change mode to follow
		{
			int num = m_ShareObj->m_vMyFriends.size();
			if (num > 1)
			{
				auto type = m_ShareObj->m_vMyFriends[0]->getType();
				if (type == MAIN_SHIP)
				{
					for (int i = 1; i < num;i++)
					{
						m_ShareObj->m_vMyFriends[i]->setMode(MODEL_FOCUS);
					}
				}
			}
			break;
		}
		*/
	default:
		break;
	}
}

void TVBattleManager::usedSkills(int tag)
{
	auto skill = SINGLE_SHOP->getSkillTrees().find(tag)->second;
	int index = -1;
	for (int i = 0; i < mActivieSkillsNum; i++)
	{
		if (tag == mActivieSkills[i]->skillId)
		{
			if (mActivieSkills[i]->status == 0)
			{
				index = i;
				break;
			}
		}
	}

	if (index < 0 || m_ShareObj->m_vMyFriends.empty())
	{
		return;
	}
	TVBattleMainShip* mship = dynamic_cast<TVBattleMainShip*>(m_ShareObj->m_vMyFriends[0]);
	if(!mship)
	{
		return;
	}
	auto spriteEff = Sprite::create();

	if (!spriteatkEff)
	{
		spriteatkEff = Sprite::create();
		mship->addChild(spriteatkEff);
		auto  animate = Animate::create(m_EffManage->getAnimation(PT_SPEC_EFFECT));
		spriteatkEff->runAction(RepeatForever::create(animate));
		spriteatkEff->setVisible(false);
	}
	
	mship->addChild(spriteEff, 100, 1000);
	auto myanimation_Skill_strong = Animate::create(m_EffManage->getAnimation(PT_SKILL_ATTACK_STRONG));//添加攻击技能动画
	auto myanimation_star = Animate::create(m_EffManage->getAnimation(PT_STAR_SHINE));//修复技能动画
	switch (tag)
	{
	case SKILL_POWWEFUL_SHOOT:
		{							
			mship->m_Info->_activeCritSize = skill.effect_per_lv*mActivieSkills[index]->skillLevel;
			spriteEff->runAction(Sequence::create(myanimation_Skill_strong, RemoveSelf::create(true), nullptr));
			if (m_atkbuffnum == 0)
			{
				mship->setfireComplete(false);
				m_atkbuffnum++;
				spriteatkEff->setVisible(true);
				schedule(schedule_selector(TVBattleManager::removeSpecEffect));
			}

			break;
		}
	case SKILL_SALVO_SHOOT:
		{
			 mship->battleSalvoShoot(skill.effect_per_lv*mActivieSkills[index]->skillLevel / SKILL_DER_MAX, SKILL_SALVO_SHOOT);
			spriteEff->runAction(Sequence::create(myanimation_Skill_strong, RemoveSelf::create(true), nullptr));
			break;
		}	
	case SKILL_INCENDIARY_SHOOT:
		{
			mship->m_Info->_isFireEfficeharm = skill.effect_per_lv*mActivieSkills[index]->skillLevel;
			spriteEff->runAction(Sequence::create(myanimation_Skill_strong, RemoveSelf::create(true), nullptr));			
			if (m_atkbuffnum == 0)
			{
				mship->setfireComplete(false);
				m_atkbuffnum++;
				spriteatkEff->setVisible(true);
				schedule(schedule_selector(TVBattleManager::removeSpecEffect));
			}
			break;
		}
	case SKILL_HAMMER_SHOOT:
		{
			mship->m_Info->_isSlowSpeedTime = skill.duration_per_lv*mActivieSkills[index]->skillLevel;
			spriteEff->runAction(Sequence::create(myanimation_Skill_strong, RemoveSelf::create(true), nullptr));	
			if (m_atkbuffnum == 0)
			{
				mship->setfireComplete(false);
				m_atkbuffnum++;
				spriteatkEff->setVisible(true);
				schedule(schedule_selector(TVBattleManager::removeSpecEffect));
			}
			break;
		}
	case SKILL_ASSAULT_NAVIGATION:
		{
			float num = mship->getShipMoveSpeed()*(skill.effect_per_lv*mActivieSkills[index]->skillLevel / SKILL_DER_MAX);
			//log("battle ship speed: %f, %f", mship->getShipMoveSpeed(), num);
			mship->setShipTempSpeed(num);
			m_skillEffectTime[SKILL_ASSAULT_NAVIGATION] = mActivieSkills[index]->definedDurationSeconds;
			auto speedup = Animate::create(m_EffManage->getAnimation(PT_SPEED_UP_EFFECT)->clone());
			spriteEff->runAction(Sequence::createWithTwoActions(speedup, RemoveSelf::create(true)));

			auto sp_eff_1 = Sprite::create();
			sp_eff_1->setPosition(spriteEff->getPosition());
			mship->addChild(sp_eff_1);
		
			auto cyclone = Animate::create(m_EffManage->getAnimation(PT_CYCLONE_EFFECT)->clone());
			sp_eff_1->runAction(RepeatForever::create(cyclone));
			sp_eff_1->runAction(Sequence::create(DelayTime::create(m_skillEffectTime[SKILL_ASSAULT_NAVIGATION]), RemoveSelf::create(true), nullptr));

			break;
		}
	case SKILL_SNEAK_ATTACK:
		{
			for (size_t i = 0; i < m_ShareObj->m_vMyFriends.size(); i++)
			{
				auto bship = dynamic_cast<TVBasicShip*>(m_ShareObj->m_vMyFriends[i]);
				if (bship)
				{
					bship->m_Info->_defense = bship->m_Info->_defense * (1 - 0.3);
					float num = bship->getShipMoveSpeed()*(skill.effect_per_lv*mActivieSkills[index]->skillLevel / SKILL_DER_MAX);
					bship->setShipTempSpeed(num);
					m_skillEffectTime[SKILL_SNEAK_ATTACK] = mActivieSkills[index]->definedDurationSeconds;
					auto speedup = Animate::create(m_EffManage->getAnimation(PT_SPEED_UP_EFFECT)->clone());
					spriteEff->runAction(Sequence::createWithTwoActions(speedup, RemoveSelf::create(true)));

					auto sp_eff_1 = Sprite::create();
					sp_eff_1->setPosition(spriteEff->getPosition());
					bship->addChild(sp_eff_1);

					auto cyclone = Animate::create(m_EffManage->getAnimation(PT_CYCLONE_EFFECT)->clone());
					sp_eff_1->runAction(RepeatForever::create(cyclone));
					sp_eff_1->runAction(Sequence::create(DelayTime::create(m_skillEffectTime[SKILL_SNEAK_ATTACK]), RemoveSelf::create(true), nullptr));
				}
			}
			break;
		}
	case SKILL_EMBOLON_ATTACK:
		{
			mship->setShortAttackAddition(skill.effect_per_lv*mActivieSkills[index]->skillLevel);
			m_skillEffectTime[SKILL_EMBOLON_ATTACK] = mActivieSkills[index]->definedDurationSeconds;
			auto attackup = Animate::create(m_EffManage->getAnimation(PT_CLOSE_ATTACK_UP));
			spriteEff->runAction(attackup);

			auto sp_eff_1 = Sprite::create();
			sp_eff_1->setPosition(spriteEff->getPosition());
			mship->addChild(sp_eff_1);

			auto cyclone = Animate::create(m_EffManage->getAnimation(PT_SPEC_EFFECT)->clone());
			sp_eff_1->runAction(RepeatForever::create(cyclone));
			sp_eff_1->runAction(Sequence::create(DelayTime::create(m_skillEffectTime[SKILL_EMBOLON_ATTACK]), RemoveSelf::create(true), nullptr));;
			break;
		}
	case SKILL_ATTACKING_HORN:
		{
			for (int i = 0; i < m_ShareObj->m_vMyFriends.size(); i++)
			{
				TVBasicShip* ship = dynamic_cast<TVBasicShip*>(m_ShareObj->m_vMyFriends[i]);
				if (ship)
				{
					ship->m_Info->_activeCritSize = skill.effect_per_lv*mActivieSkills[index]->skillLevel;
				}
			}
			break;
		}
	case SKILL_FORTRESS_BATTLESHIP:
		{
			mship->m_Info->_change_defense += mship->m_Info->_change_defense * skill.effect_per_lv*mActivieSkills[index]->skillLevel / SKILL_DER_MAX;
			float num = -mship->getShipMoveSpeed()* 0.4f;
			mship->setShipTempSpeed(num);
			m_skillEffectTime[SKILL_FORTRESS_BATTLESHIP] = mActivieSkills[index]->definedDurationSeconds;
			auto defence = Animate::create(m_EffManage->getAnimation(PT_DEFENCE_UP));
			spriteEff->runAction(Sequence::create(defence, RemoveSelf::create(true), nullptr));
			break;
		}
	case SKILL_FLEET_REPAIR:
	case SKILL_PROGRESSIVE_REPAIR:
	case SKILL_EMERGENT_REPAIR:
	{
			m_skillEffectTime[SKILL_PROGRESSIVE_REPAIR] = mActivieSkills[index]->definedDurationSeconds;
			
			switch (mship->m_Info->_type)
			{
			case 1:
				spriteEff->setScale(0.43);
				break;
			case 2:
				spriteEff->setScale(0.5);
				break;
			case 3:
				spriteEff->setScale(0.57);
				break;
			default:
				spriteEff->setScale(0.5);
				break;
			}
			auto progressrepair = Animate::create(m_EffManage->getAnimation(PT_RECOVER_GREEN)->clone());
			auto repair = Animate::create(m_EffManage->getAnimation(PT_REPAIR_EFFECT)->clone());
			auto sp = Sprite::create();
			sp->setPosition(spriteEff->getPosition());
			
			mship->addChild(sp);
			if (tag == SKILL_PROGRESSIVE_REPAIR)
			{
				sp->runAction(Sequence::create(repair, RemoveSelf::create(true), nullptr));
				spriteEff->runAction(RepeatForever::create(progressrepair));
				spriteEff->runAction(Sequence::create(DelayTime::create(m_skillEffectTime[SKILL_PROGRESSIVE_REPAIR]), RemoveSelf::create(true), nullptr));
			}
			else if (tag == SKILL_EMERGENT_REPAIR || tag == SKILL_FLEET_REPAIR)
			{
				sp->runAction(Sequence::create(repair, RemoveSelf::create(true), nullptr));
				spriteEff->runAction(progressrepair);
				spriteEff->runAction(Sequence::create(DelayTime::create(3),RemoveSelf::create(true), nullptr));
			}
    }
		break;
	default:
		break;
	}
}

void TVBattleManager::usedProps(int tag)
{
	TVBasicShip* ship = dynamic_cast<TVBasicShip*>(m_ShareObj->m_vMyFriends[0]);

	ship->addLogAction(ACT_USE_PROPS,0,tag); // writer log
	if (!ship && tag != PROP_FLEE_SMOKE)
	{
		return;
	}
	switch (tag)
	{
	case PROP_FLEE_SMOKE:
		{
			stopBattle();
			m_UIlayer->stopGameByProps();
			return;
		}
	case PROP_NIORD_HORN:
		{
			ship->deleteBuffer(STATE_SLOWSPEED);
			break;
		}
	case PROP_WAR_DRUM:
		{
			//使用道具鼓舞士气
			getShareObj()->addMoraleCore(1500);
			m_UIlayer->updateAnger(true);
			break;
		}
	case PROP_CROSS:
		{
			ship->deleteBuffer(STATE_FIRE);
			break;
		}
	default:
		break;
	}
}

void TVBattleManager::changeShipInfo(int skill_id, int tag)
{
	auto skill = SINGLE_SHOP->getSkillTrees().find(skill_id)->second;
	int index = -1;
	for (int i = 0; i < mActivieSkillsNum; i++)
	{
		if (skill_id == mActivieSkills[i]->skillId)
		{
			if (mActivieSkills[i]->status == 0)
			{
				index = i;
				break;
			}
		}
	}
	if (index < 0 || m_ShareObj->m_vMyFriends.empty())
	{
		return;
	}
	TVBattleMainShip* mship = dynamic_cast<TVBattleMainShip*>(m_ShareObj->m_vMyFriends[0]);
	
	switch (skill_id)
	{
	case SKILL_POWWEFUL_SHOOT:
		{
			if (mship)
			{
				mship->m_Info->_activeCritSize = 0;
			}
			break;
		}
	case SKILL_INCENDIARY_SHOOT:
		{
			if (mship)
			{
				mship->m_Info->_isFireEfficeharm = 0;
			}
			break;
		}
	case SKILL_HAMMER_SHOOT:
		{
			if (mship)
			{
				mship->m_Info->_isSlowSpeedTime = 0;
			}
			break;
		}
	case SKILL_ASSAULT_NAVIGATION:
		{
			if (mship)
			{
				float num = -mship->getShipMoveSpeed() * (skill.effect_per_lv*mActivieSkills[index]->skillLevel / SKILL_DER_MAX);
				mship->setShipTempSpeed(num);
			}
			break;
		}
	case SKILL_SNEAK_ATTACK:
		{
			for (size_t i = 0; i < m_ShareObj->m_vMyFriends.size(); i++)
			{
				auto bship = dynamic_cast<TVBasicShip*>(m_ShareObj->m_vMyFriends[i]);
				if (bship)
				{
					bship->m_Info->_change_defense -= bship->m_Info->_defense * (1 - 0.3);
					float num = -bship->getShipMoveSpeed() * (skill.effect_per_lv*mActivieSkills[index]->skillLevel / SKILL_DER_MAX);
					bship->setShipTempSpeed(num);
				}
			}
			break;
		}
	case SKILL_EMBOLON_ATTACK:
		{
			if (mship)
			{
				mship->setShortAttackAddition(0);
			}
			break;
		}
	case SKILL_ATTACKING_HORN:
		{
			for (int i = 0; i < m_ShareObj->m_vMyFriends.size(); i++)
			{
				TVBasicShip* ship = dynamic_cast<TVBasicShip*>(m_ShareObj->m_vMyFriends[i]);
				if (ship && ship->m_Info->_tag == tag)
				{
					ship->m_Info->_activeCritSize = 0;
				}
			}
			break;
		}
	case SKILL_FORTRESS_BATTLESHIP:
		{
			if (mship)
			{
				mship->m_Info->_change_defense -= mship->m_Info->_defense * skill.effect_per_lv*mActivieSkills[index]->skillLevel / SKILL_DER_MAX;
				float num = mship->getShipMoveSpeed() * 0.4f;
				mship->setShipTempSpeed(num);
			}
			break;
		}
	default:
		break;
	}		
}

void TVBattleManager::startBattle(float dt)
{
	const auto &enemys = m_ShareObj->m_vEnemys;
	const auto &myFriend = m_ShareObj->m_vMyFriends;
	
	for (int i = 0; i <myFriend.size(); i++ )
	{
		if (static_cast<TVBattleMainShip*>(myFriend[i]))
		{
			static_cast<TVBattleMainShip*>(myFriend[i])->startBattle();
		}
		else
		{
			static_cast<TVBattleShip*>(myFriend[i])->startBattle();
		}
	}
	for (int i = 0; i <enemys.size(); i++ )
	{
		static_cast<TVBattleShip*>(enemys[i])->startBattle();
	}
	m_UIlayer->startGame();
	this->scheduleUpdate();
	m_StartGame = true;
	WriterLog::getInstance()->setDelegate(m_ShareObj);
	WriterLog::getInstance()->addStartStates(m_FightResult);
}

void TVBattleManager::scaleEnd()
{
	//进入战斗新手引导
	if (ProtocolThread::GetInstance()->isInTutorial())
	{
		auto pNovice = UINoviceStoryLine::createNovice(this);
		this->addChild(pNovice,MAX_Z_ORDER + 2);
		pNovice->onGuideTaskMessage(UINoviceStoryLine::WAR_GUIDE_DIALOG,0);
		pNovice->setCameraMask(4);
	}
	else
	{
		CCLOG("BattleManage scaleEnd 11 ");
		auto call = CallFuncN::create([=](Ref *pSender){
			this->startBattle(0);
		});
		//DelayTime::create(1.5f),

		this->runAction(Sequence::create(DelayTime::create(1.5f), call, nullptr));
		//this->scheduleOnce(schedule_selector(BattleManage::startBattle), 1.5f);
		//this->scheduleOnce(CC_CALLBACK_1(BattleManage::startBattle, this), 1.5f, "startBattle");
	}
}

void TVBattleManager::stopBattle()
{
	const auto &enemys = m_ShareObj->m_vEnemys;
	const auto &myFriend = m_ShareObj->m_vMyFriends;
	for (int i = 0; i <myFriend.size(); i++ )
	{
		myFriend[i]->stopBattle();
	}
	for (int i = 0; i <enemys.size(); i++ )
	{
		enemys[i]->stopBattle();
	}
	m_UIlayer->stopGame();
	this->unscheduleUpdate();
	m_StartGame = false;
}

bool TVBattleManager::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}

void TVBattleManager::onTouchEnded(Touch *touch, Event *unused_event)
{
	Vec2 pos_1 = touch->getLocation();
	int numb = m_ShareObj->m_vMyFriends.size();
	if (numb > 0)
	{
		auto ship = dynamic_cast<TVBattleMainShip*>(m_ShareObj->m_vMyFriends[0]);
		if (ship)
		{
			Camera  *c  = Director::getInstance()->getRunningScene()->getCameras().front();
			auto diff = c->getPosition() - m_cameraStartPostion;
			Vec2 pos_2 = m_MapLayer->convertToNodeSpace(pos_1+diff);
			ship->touchForTarget(pos_2);
			//海面点击效果
			dynamic_cast<TouchMe*>(_userObject)->start(m_EffManage->getAnimation(PT_TOUCH_ANIMA));
			dynamic_cast<TouchMe*>(_userObject)->setPosition(pos_2);
		}
	}
}

void TVBattleManager::onTouchCancelled(Touch *touch, Event *unused_event)
{

}

void TVBattleManager::onTouchMoved(Touch *touch, Event *unused_event)
{
	if (!m_StartGame)
	{
		return;
	}
	if (m_ShareObj->m_vMyFriends.empty())
	{
		return;
	}
	auto ship = m_ShareObj->m_vMyFriends[0];
	if (ship->getType() != MAIN_SHIP)
	{
		Vec2 prePos = touch->getPreviousLocation();
		Vec2 curPos = touch->getLocation();
		Vec2 diffPos = curPos - prePos;

		Camera  *c  = Director::getInstance()->getRunningScene()->getCameras().front();
		Vec2 cameraPostion = c->getPosition() - diffPos;
		Size contentSize = m_MapLayer->getContentSize();
		auto size = Director::getInstance()->getWinSize();

		if (cameraPostion.x > contentSize.width / 2)
		{
			cameraPostion.x = contentSize.width / 2;
		}
		if (cameraPostion.y > contentSize.height / 2)
		{
			cameraPostion.y = contentSize.height / 2;
		}

		if (cameraPostion.x < -contentSize.width / 2 + size.width) 
		{
			cameraPostion.x = -contentSize.width / 2 + size.width;
		}

		if(cameraPostion.y < -contentSize.height/2 + size.height)
		{
			cameraPostion.y = -contentSize.height/2 + size.height;
		}

		c->setPosition(cameraPostion);
	}
}

bool TVBattleManager::onContactBegain(PhysicsContact& contact)
{
	if (!m_StartGame)
	{
		return false;
	}
    auto node_a = contact.getShapeA()->getBody()->getNode()->getParent();
    auto node_b = contact.getShapeB()->getBody()->getNode()->getParent();
	TVBasicShip* ob_A = dynamic_cast<TVBasicShip*>(node_a);
 	TVBasicShip* ob_B = dynamic_cast<TVBasicShip*>(node_b);
	if (ob_A->m_Info->_sailornum == 0 || ob_B->m_Info->_sailornum == 0)
	{
		return false;
	}
	if (ob_A->isCloseBattle() || ob_B->isCloseBattle())
	{
		return true;
	}
	if (ob_A->m_Info->_isFriend == ob_B->m_Info->_isFriend)
	{
		return true;
	}
	int num1 = ob_A->m_Info->_sailornum;
	int num2 = ob_B->m_Info->_sailornum;
	ob_A->setCloseBattleState(true);
	ob_B->setCloseBattleState(true);
	ob_A->addLogAction(ACT_COLD_BATTLE,0,ob_B->m_Info->_tag);// writer log
	ob_B->addLogAction(ACT_COLD_BATTLE,0,ob_A->m_Info->_tag);// writer log
	//撞角攻击
	ob_A->hurt(ob_B->m_Info->_bowgun_attack - ob_A->m_Info->_defense - ob_A->m_Info->_change_defense,ob_B->m_Info->_sid);
	ob_B->hurt(ob_A->m_Info->_bowgun_attack - ob_B->m_Info->_defense - ob_B->m_Info->_change_defense,ob_A->m_Info->_sid);
#if SOLO_TEST
	if (ob_A->getBlood() > 0 && ob_B->getBlood() > 0)
	{
		stopBattle();
		auto solo = UISolo::createLayer();
		solo->setCameraMask(4, true);
		m_UIlayer->addChild(solo, 10010);
		solo->setBattleShip(ob_A, ob_B);
	}
#else 
	//先撞角攻击后白刃或者单挑
	if (ob_A->getBlood() > 0 && ob_B->getBlood() > 0)
	{
		auto seq = Sequence::create(DelayTime::create(3.5f), CCCallFuncN::create(CC_CALLBACK_0(TVBattleManager::delaySetCloseBattle, this, ob_A, ob_B)), nullptr);
		Battle_A_B objs;
		switch (singledResult(ob_A, ob_B))
		{
		case 0:
		case 3:
			objs = { 3, node_a, node_b, nullptr, nullptr };
			objs._animation = createclosefightflash(node_a, node_b);
			this->numofsailor(node_a, node_b);
			objs._sprite_bg_red = createsp_bg_red(node_a, node_b);
			objs._fonts = createFonts(node_a, node_b, num1, num2);
			objs.font1 = createfont1(node_a, node_b, num1, num2);
			objs._fonts->setVisible(false);
			objs.font1->setVisible(false);

			objs._animation->addChild(objs._sprite_bg_red, -1);
			objs._animation->addChild(objs._fonts, -2);
			objs._animation->addChild(objs.font1, -1);

			m_fightingObjs.push_back(objs);
			foodPlunderAndCapturingShip(ob_A, ob_B);
			break;
		case 1:
			if (ob_A->m_Info->_isFriend)
			{
				ob_B->m_Info->_sailornum = 0;
			}
			else
			{
				ob_A->m_Info->_sailornum = 0;
			}

			this->runAction(seq);
			foodPlunderAndCapturingShip(ob_A, ob_B);
			break;
		case 2:
			if (ob_A->m_Info->_isFriend)
			{
				ob_A->m_Info->_sailornum = 0;
			}
			else
			{
				ob_B->m_Info->_sailornum = 0;
			}

			this->runAction(seq);
			break;
		default:
			ob_A->setCloseBattleState(false);
			ob_B->setCloseBattleState(false);
			foodPlunderAndCapturingShip(ob_A, ob_B);
			break;
		}
	}
	else
	{
		auto seq = Sequence::create(DelayTime::create(2.f), CCCallFuncN::create(CC_CALLBACK_0(TVBattleManager::delaySetCloseBattle, this, ob_A, ob_B)), nullptr);
		this->runAction(seq);
	}

	this->m_UIlayer->flushUserDefault(ob_A);
	this->m_UIlayer->flushUserDefault(ob_B);
	
	//解除主舰锁定
	if (m_ShareObj->m_nLockShipTag == ob_A->m_Info->_tag || m_ShareObj->m_nLockShipTag == ob_B->m_Info->_tag)
	{

		for (size_t i = 0; i < m_ShareObj->m_vMyFriends.size(); i++)
		{
			if (m_ShareObj->m_vMyFriends[i]->m_Info->_tag == 1)
			{
				TVBattleMainShip* ship = dynamic_cast<TVBattleMainShip*>(m_ShareObj->m_vMyFriends[i]);
				std::string str = SINGLE_SHOP->getTipsInfo()["TIP_FIGHTING_MAINSHIP_UNLOCK"];
				LabelTTF* label = LabelTTF::create(str, "", 20);
				label->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(1), FadeOut::create(0.5), RemoveSelf::create(true), nullptr));
				label->setPositionY(120);
				label->setColor(Color3B::RED);
				ship->addChild(label);
				break;
			}
		}
	}

	int tag = 0;
	for (size_t i = 0; i < m_ShareObj->m_vEnemys.size(); i++)
	{
		if (m_ShareObj->m_vEnemys[i]->m_Info->_tag == tag)
		{
			TVBattleShip* ship = dynamic_cast<TVBattleShip*>(m_ShareObj->m_vEnemys[i]);
			ship->lockShip(false);
			ship->setUnlockTime(4);
			break;
		}
	}
#endif
	return true;
}

void TVBattleManager::delaySetCloseBattle(TVBasicShip*node1, TVBasicShip*node2)
{
	node1->setCloseBattleState(false);
	node2->setCloseBattleState(false);
}

Sprite*TVBattleManager::createfont1(Node*node1,Node*node2,int loatNumA,int loatNumB)
{
	auto sp = Sprite::create();
	TVBasicShip* ob_A1 = dynamic_cast<TVBasicShip*>(node1);
	TVBasicShip* ob_B1 = dynamic_cast<TVBasicShip*>(node2);	

	LabelTTF* label_1 = LabelTTF::create("","",25);
	LabelTTF* label_2 = LabelTTF::create("","",25);
	sp->addChild(label_1,1,ob_A1->m_Info->_sailornum);
	sp->addChild(label_2,1,ob_B1->m_Info->_sailornum);
	if (ob_A1->m_Info->_isFriend)
	{
		label_1->setPosition(-15,50);
		label_2->setPosition(120,50);
	}else
	{
		label_1->setPosition(120,50);
		label_2->setPosition(-15,50);
	}
	label_1->setName("label_1");
	label_2->setName("label_2");
	std::string font_1 = StringUtils::format("%d",loatNumA);
	std::string font_2 = StringUtils::format("%d",loatNumB);
	label_1->setString(font_1);
	label_2->setString(font_2);
	auto seq1 = Sequence::create(DelayTime::create(2.f),CCCallFuncN::create(CC_CALLBACK_0(TVBattleManager::fontAnimation,this,sp,node1,node2)),nullptr);
	sp->runAction(seq1);
	return sp;
}

Sprite* TVBattleManager::createsp_bg_red(Node*node1,Node*node2)
{
	auto  bg_sprite = Sprite::create();
    auto  sprite_bg_red = Sprite::create();
	sprite_bg_red->setTexture("res/melee/red.png");
	sprite_bg_red->setScale(0.6);
	sprite_bg_red->setTag(30);
	//sprite_bg_red->setScale(0.6);
	sprite_bg_red->setPosition(Vec2(50,50));
 	sprite_bg_red->setVisible(true);
	bg_sprite->addChild(sprite_bg_red);
 	auto seq = Sequence::create(DelayTime::create(0.1f),CCCallFuncN::create(CC_CALLBACK_0(TVBattleManager::sprite_bg_redAnimation,this,sprite_bg_red)),nullptr);
 	sprite_bg_red->runAction(Repeat::create(seq,30));
	return bg_sprite;
}

void TVBattleManager::sprite_bg_redAnimation(Node*sp)
{
	int current_time = sp->getTag();
	sp->setTag(current_time - 1);
	current_time--;
	if (current_time % 2 == 1)
	{
		sp->setVisible(true);
	}else
	{
	    sp->setVisible(false);
	}
}

Layer* TVBattleManager:: createclosefightflash(Node*_A_obj,Node*_B_obj)
{
	auto layer = Layer::create();
	auto animation = Sprite::create();
	animation->setTexture("res/melee/melee0000.png");
	animation->setPosition(Vec2(50,50));
	animation->setScale(0.6);
	layer->addChild(animation,10);
	TVBasicShip* ob_A1 = dynamic_cast<TVBasicShip*>(_A_obj);
	TVBasicShip* ob_B1 = dynamic_cast<TVBasicShip*>(_B_obj);
	layer->setAnchorPoint(ccp(0.5,0.5));
	layer->setPosition(Vec2((ob_A1->getPosition()+ob_B1->getPosition())/2));
	//sprite->setScale(0.6);
	m_MapLayer->addChild(layer,10000);
	animation->runAction(Animate::create(m_EffManage->getAnimation(PT_CLOSE_BATTLE)));		
	return layer;
}




Sprite* TVBattleManager::createFonts(Node*node1,Node*node2,int loatNumA,int loatNumB)
{
	Color3B c4;
	c4=Color3B(250,54,54);
	auto sp = Sprite::create();
	TVBasicShip* ob_A1 = dynamic_cast<TVBasicShip*>(node1);
	TVBasicShip* ob_B1 = dynamic_cast<TVBasicShip*>(node2);
	TextAtlas * label_1 = TextAtlas::create("", IMAGE_FONT[1], 20, 26, "/");
	TextAtlas * label_2 = TextAtlas::create("", IMAGE_FONT[1], 20, 26, "/");

	label_1->setColor(c4);
	label_2->setColor(c4);
//	sp->addChild(sprite_bg,-10);

	ob_A1->addChild(label_1, 10, ob_A1->m_Info->_sailornum);
	ob_B1->addChild(label_2, 10, ob_B1->m_Info->_sailornum);
// 	if (ob_A1->m_Info->_isFriend)
// 	{
// 		label_1->setPosition(-15,80);
// 		label_2->setPosition(120,80);
// 	}else
// 	{
// 		label_1->setPosition(120,80);
// 		label_2->setPosition(-15,80);
// 	}
	std::string font_1 = StringUtils::format("/-%d",loatNumA-ob_A1->m_Info->_sailornum);
	std::string font_2 = StringUtils::format("/-%d",loatNumB-ob_B1->m_Info->_sailornum);
	label_1->setString(font_1);
	label_2->setString(font_2);
	label_1->setName("label_1");
	label_2->setName("label_2");
	label_2->setOpacity(0);
	label_1->setOpacity(0);
	auto sp_sailor1 = Sprite::create();
	auto sp_sailor2 = Sprite::create();
	sp_sailor1->setTexture("ship/battle_losesailor.png");
	sp_sailor2->setTexture("ship/battle_losesailor.png");
	sp_sailor1->setOpacity(0);
	sp_sailor2->setOpacity(0);
	sp_sailor1->setPosition(Vec2(-sp_sailor1->getContentSize().width / 2, label_1->getContentSize().height/2));
	sp_sailor2->setPosition(Vec2(-sp_sailor2->getContentSize().width / 2, label_2->getContentSize().height/2));
	label_1->addChild(sp_sailor1, 1);
	label_2->addChild(sp_sailor2, 1);
	//auto seq_1 = Sequence::create(DelayTime::create(2.f),FadeOut::create(0.1f),nullptr);
	auto easeAct = EaseQuinticActionOut::create(MoveBy::create(2.f,Vec2(0,30)));
	sp_sailor1->runAction(Sequence::create(DelayTime::create(1.9f), FadeIn::create(0.1f), nullptr));
	sp_sailor2->runAction(Sequence::create(DelayTime::create(1.9f), FadeIn::create(0.1f), nullptr));
	auto seq = Sequence::create(DelayTime::create(1.9f),FadeIn::create(0.1f),easeAct,CCCallFuncN::create(CC_CALLBACK_0(TVBattleManager::deleteHarmNumsA,this,label_1,label_2)),nullptr);
	label_1->runAction(seq);
	label_2->runAction(seq->clone());
	return sp;
}

void TVBattleManager::fontAnimation(Node* label,Node* node1,Node* node2)
{
	auto L_1 = dynamic_cast<LabelTTF*>(label->getChildByName("label_1"));
	auto L_2 = dynamic_cast<LabelTTF*>(label->getChildByName("label_2"));	
	
	std::string font_11 = StringUtils::format("%d",L_1->getTag());
 	std::string font_12 = StringUtils::format("%d",L_2->getTag());
    L_1->setString(font_11);
	L_2->setString(font_12);

	TVBasicShip* ob_A1 = dynamic_cast<TVBasicShip*>(node1);
	TVBasicShip* ob_B1 = dynamic_cast<TVBasicShip*>(node2);	
	ob_A1->updataSailorLabel();
	ob_B1->updataSailorLabel();
}

void TVBattleManager::numofsailor(Node*node1,Node*node2)
{
	bool isASuc = false;
	bool isBSuc = false;

	float att_factor = 0.7f;
	float def_factor = 0.5f;

	TVBasicShip* ob_A1 = dynamic_cast<TVBasicShip*>(node1);
	TVBasicShip* ob_B1 = dynamic_cast<TVBasicShip*>(node2);

	int sailer_A = ob_A1->m_Info->_sailornum;
	int sailer_B = ob_B1->m_Info->_sailornum;


 	int extlost_A = 0;
	int extlost_B = 0;

	if (ob_B1->m_Info->_isFriend)
	{
		int moral_a = m_ShareObj->m_nMorale_Enemy;
		int moral_b = m_ShareObj->m_nMorale;

		extlost_A = sailer_B * (ob_B1->getMoralAddition(moral_b) - 500)/5000;  //士气影响
		extlost_B = sailer_A * (ob_A1->getMoralAddition(moral_a) - 500)/5000;
	}else
	{
		int moral_a = m_ShareObj->m_nMorale;
		int moral_b = m_ShareObj->m_nMorale_Enemy;
		extlost_A = sailer_B * (ob_B1->getMoralAddition(moral_b) - 500)/5000;
		extlost_B = sailer_A * (ob_A1->getMoralAddition(moral_a) - 500)/5000;
	}

	//技能增加的是战斗后的伤害
	float loatNumAf = (sailer_B * (att_factor - def_factor) + extlost_A) * (1 + ob_B1->getShortAttackAddition());
	float loatNumBf = (sailer_A * (att_factor - def_factor) + extlost_B) * (1 + ob_A1->getShortAttackAddition());

	int loatNumA = ceil(loatNumAf / 2);
	int loatNumB= ceil(loatNumBf / 2);

	//严阵以待
	SkillDefine*  skillReadyA = ob_A1->getPassiveSkillsById(SKILL_COMBAT_READY);
	SkillDefine*  skillReadyB = ob_B1->getPassiveSkillsById(SKILL_COMBAT_READY);
	//严阵以待的条件

	if (skillReadyA)
	{

		loatNumB = ceil(loatNumB*(1 + 0.01* skillReadyA->level));
	}

	if (skillReadyB)
	{
		loatNumA = ceil(loatNumA*(1 + 0.01*skillReadyB->level));
	}

	if (ob_A1->m_Info->_sailornum < loatNumA)
	{
		ob_A1->m_Info->_sailornum = 0;
		isBSuc = true;
		
	}else
	{
		ob_A1->m_Info->_sailornum=sailer_A - loatNumA;
	}
	if (ob_B1->m_Info->_sailornum < loatNumB)
	{
		ob_B1->m_Info->_sailornum = 0;
		isASuc = true;
	}else
	{
		ob_B1->m_Info->_sailornum=sailer_B - loatNumB;
	}	



	if (isASuc)
	{
		SkillDefine*  skillSurA = ob_A1->getPassiveSkillsById(SKILL_INDUCE_TO_CAPITULATE);
		if (skillSurA)
		{
			//计算几率
			float rand = CCRANDOM_0_1() * 100;

			int condition = skillSurA->level * 2;
			if (rand < condition)
			{
				ob_A1->m_Info->_captureNum = ceil(ob_B1->m_Info->_sailBeginNum / 2);
				int max = ob_A1->getMaxSoliderNum();

			
				if (ob_A1->m_Info->_sailornum + ob_A1->m_Info->_captureNum> max)
				{
					ob_A1->m_Info->_captureNum = max - ob_A1->m_Info->_sailornum;
					ob_A1->m_Info->_sailornum = max;
				}
				else
				{
					ob_A1->m_Info->_sailornum += ob_A1->m_Info->_captureNum;
				}

			
				

				ob_A1->runAction(Sequence::create(DelayTime::create(1.2f), CallFuncN::create([=](Ref *pSender){
					ob_A1->playCaptureEffect();
				}), nullptr));
				
			}

		}
		FIGHTING_CENTER_TIP_DATA tips = { ob_A1->m_Info->_sid, ob_A1->m_Info->_tag, ob_B1->m_Info->_sid, ob_B1->m_Info->_tag, 0, 0, 0, 0, 0, TIP_SHIP_SUNK };
		this->m_UIlayer->pushFightingCentralTips(tips);
 
	}else if(isBSuc)
	{
		SkillDefine*  skillSurB = ob_B1->getPassiveSkillsById(SKILL_INDUCE_TO_CAPITULATE);
		if (skillSurB)
		{			
			//计算几率
			float rand = CCRANDOM_0_1() * 100;
			int condition = skillSurB->level * 2;
			if (rand < condition)
			{
				ob_B1->m_Info->_captureNum = ceil(ob_A1->m_Info->_sailBeginNum / 2);
				int max = ob_B1->getMaxSoliderNum();

				if (ob_B1->m_Info->_sailornum + ob_B1->m_Info->_captureNum> max)
				{
					ob_B1->m_Info->_captureNum = max - ob_B1->m_Info->_sailornum;
					ob_B1->m_Info->_sailornum = max;
				}
				else
				{
					ob_B1->m_Info->_sailornum += ob_B1->m_Info->_captureNum;
				}
				

				ob_B1->runAction(Sequence::create(DelayTime::create(1.2f), CallFuncN::create([=](Ref *pSender){
					ob_B1->playCaptureEffect();
				}), nullptr));
				
			}
		}
		FIGHTING_CENTER_TIP_DATA tips = { ob_B1->m_Info->_sid, ob_B1->m_Info->_tag, ob_A1->m_Info->_sid, ob_A1->m_Info->_tag, 0, 0, 0, 0, 0, TIP_SHIP_SUNK };
		this->m_UIlayer->pushFightingCentralTips(tips);
	}
	//白刃战造成的水手死亡，参数依次为：己方shipid ，敌人shipid，敌人死亡的水手，己方死亡的水手
//	void addSailorBattlelogItem(int myShipId, int enemyShipId, int killedEnemySailorNum, int killedSelfSailorNum);
	if (ob_A1->m_Info->_isFriend)
	{
		WriterLog::getInstance()->addSailorBattlelogItem(ob_A1->m_Info->_sid, ob_B1->m_Info->_sid, loatNumB, loatNumA);
	}
	else
	{
		WriterLog::getInstance()->addSailorBattlelogItem(ob_B1->m_Info->_sid, ob_A1->m_Info->_sid, loatNumA, loatNumB);
	}
	
}

void TVBattleManager::deleteHarmNumsA(Node*node1,Node*node2)
{	
	node1->removeFromParent();
	node2->removeFromParent();
}

void TVBattleManager::onContactPostSolve(PhysicsContact& contact,const PhysicsContactPostSolve& solve)
{
	//log("on contact postsolve");
}

bool TVBattleManager::onContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
	//log("on contact presolve");
	return true;
}

void TVBattleManager::onContactSeperate(PhysicsContact& contact)
{
	//log("on contact seperate");
	if (!m_StartGame)
	{
		return;
	}
	auto node_a = contact.getShapeA()->getBody()->getNode()->getParent();
	auto node_b = contact.getShapeB()->getBody()->getNode()->getParent();
	TVBasicShip* ob_A = dynamic_cast<TVBattleMainShip*>(node_a);
	TVBasicShip* ob_B = dynamic_cast<TVBattleMainShip*>(node_b);
	if (ob_A)
	{
		ob_A->findTargetAgain();
	}
	if (ob_B)
	{
		ob_B->findTargetAgain();
	}
}

void TVBattleManager::checkBattle_A_B()
{
	for (auto iter = m_fightingObjs.begin(); iter != m_fightingObjs.end();)
	{
		TVBasicShip* _obj_a = dynamic_cast<TVBasicShip*>((*iter)._A_obj);
		TVBasicShip* _obj_b =	dynamic_cast<TVBasicShip*>((*iter)._B_obj);
		if ((*iter)._CD < 1)
		{
			if (_obj_a)
			{
				_obj_a->setCloseBattleState(false);
			}
			if (_obj_b)
			{
				_obj_b->setCloseBattleState(false);
			}
			
			(*iter)._animation->removeFromParent();
			iter = m_fightingObjs.erase(iter);
		}else
		{
			if ((*iter)._CD < 3)
			{
				(*iter).font1->setVisible(false);
			}else if ((*iter)._CD < 2)
			{
				(*iter)._animation->stopAllActions();							
			}
			(*iter)._CD = (*iter)._CD - 1;
			iter++;
		}
	}
}

void TVBattleManager::update(float df)
{
	m_SkillFlushTime += df;
	if (m_SkillFlushTime > 0.999f)
	{
		m_SkillFlushTime = 0;
		m_SkillManage->checkBufferState();
		checkBattle_A_B(); //1秒检测一次碰撞有瞬间穿过的可能
		for (int i = 0; i < 20; i++)
		{
			m_skillEffectTime[i]--;
			if (i == SKILL_PROGRESSIVE_REPAIR && m_skillEffectTime[SKILL_PROGRESSIVE_REPAIR] > 0)
			{
				changeShipInfo(SKILL_PROGRESSIVE_REPAIR);
			}
			if (m_skillEffectTime[i] == 0)
			{
				changeShipInfo(i);
			}
		}
	}
 }

void TVBattleManager::removeSpecEffect(float dt)
{
	if (m_ShareObj->m_vMyFriends.empty())
	{
		return;
	}
	TVBattleMainShip* mship = dynamic_cast<TVBattleMainShip*>(m_ShareObj->m_vMyFriends[0]);
	if (mship && mship->getfireComplete() && m_atkbuffnum > 0)
	{
		mship->setfireComplete(false);
		m_atkbuffnum = 0;
		spriteatkEff->setVisible(false);
		this->unschedule(schedule_selector(TVBattleManager::removeSpecEffect));
	}
}

bool TVBattleManager::isCapturingShip(int shipId)
{
	if (m_UIlayer->getCapturingShipId().size() >= m_FightResult->cancatchshipnum)
	{
		return false;
	}

	for (size_t i = 0; i < m_FightResult->n_enemyships; i++)
	{
		if (m_FightResult->enemyships[i]->shipid == shipId)
		{
			if (m_FightResult->enemyships[i]->can_catch)
			{
				m_UIlayer->setCapturingShipId(shipId);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;

	/*
	if (m_FightResult->fighttype != FIGHT_TYPE_NORMAL)
	{
		return false;
	}
	if (m_UIlayer->getCapturingShipId().size() >= m_FightResult->cancatchshipnum)
	{
		return false;
	}
	
	int hull_traction = 0;
	for (size_t i = 0; i < m_FightResult->myships[0]->n_skills; i++)
	{
		if (m_FightResult->myships[0]->skills[i]->id == SKILL_HULL_TRACTION)
		{
			auto skillTrees = SINGLE_SHOP->getSkillTrees().find(SKILL_HULL_TRACTION)->second;
			hull_traction = m_FightResult->myships[0]->skills[i]->level*skillTrees.effect_per_lv;
			break;
		}
	}

	int ship_require_level = SINGLE_SHOP->getShipData().find(shipId)->second.require_level;
	int ship_capture_lv = SINGLE_SHOP->getShipData().find(shipId)->second.capture_lv;
	int ship_capture_rate = SINGLE_SHOP->getShipData().find(shipId)->second.capture_rate;
	//测试数据
	//ship_capture_rate = 1000;
	//有可能被捕获满足条件
	if (ship_require_level - SINGLE_HERO->m_iLevel <= 5 && hull_traction > ship_capture_lv)
	{	//捕获的概率
		if (ship_capture_rate > cocos2d::random(0, 999))
		{
			m_UIlayer->setCapturingShipId(shipId);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	*/
}

void TVBattleManager::foodPlunderAndCapturingShip(TVBasicShip *ob_A, TVBasicShip *ob_B)
{
	//都是常规战斗中
	//敌船是否被捕获(添加捕获特效)条件1、旗舰 2、白刃战胜利 3、概率
	if (m_FightResult->fighttype != FIGHT_TYPE_NORMAL)
	{
		return;
	}
	if ((ob_A->getType() == MAIN_SHIP && ob_B->m_Info->_sailornum == 0) || (ob_B->getType() == MAIN_SHIP && ob_A->m_Info->_sailornum == 0))
	{
		int enemyShipId = 0;
		TVBasicShip* mainShip = nullptr;
		SkillDefine *skillReady = nullptr;
		if (ob_A->getType() == MAIN_SHIP)
		{
			skillReady = ob_A->getPassiveSkillsById(SKILL_FOOD_LOOTING);
			enemyShipId = ob_B->m_Info->_sid;
			mainShip = ob_A;
		}
		else
		{
			skillReady = ob_B->getPassiveSkillsById(SKILL_FOOD_LOOTING);
			enemyShipId = ob_A->m_Info->_sid;
			mainShip = ob_B;
		}

		//捕获特效
		if (mainShip)
		{
			float time = 4.0f;
			//有食物掠夺技能就可以掠夺，只是掠夺多少不同
			if (skillReady)
			{
				auto skillTrees = SINGLE_SHOP->getSkillTrees().find(SKILL_FOOD_LOOTING)->second;
				auto shipInfo = SINGLE_SHOP->getShipData().find(enemyShipId)->second;
				int food_looting = skillReady->level*skillTrees.effect_per_lv;
				int loot_supply = ceil(1.0 * shipInfo.supply_max * food_looting / SKILL_DER_MAX);
				//std::string st_supply = StringUtils::format("/%d", loot_supply);
				//auto t_supply = TextAtlas::create(st_supply, IMAGE_FONT[3], 30, 32, "/");
				auto i_supply = ImageView::create("ship/battle_getsupply.png");

				std::string st_supply = StringUtils::format("+%d", loot_supply);
				Label* t_supply = Label::create(st_supply, "", 24);
				t_supply->setColor(Color3B(0, 230, 108));
				t_supply->enableOutline(Color4B::BLACK, OUTLINE_MIN);

				i_supply->setPosition(Vec2(-20, 30));
				t_supply->setPosition(Vec2(20, 30));
				//i_supply->setScale(0.85);
				//t_supply->setScale(0.85);
				i_supply->setOpacity(0);
				t_supply->setOpacity(0);
				auto seq_1 = Sequence::create(DelayTime::create(1.2f), FadeOut::create(0.3f), nullptr);
				auto easeAct = EaseQuinticActionOut::create(Spawn::createWithTwoActions(MoveBy::create(1.0f, Vec2(0, 50)), seq_1));
				auto seq = Sequence::create(DelayTime::create(4.0f), FadeIn::create(0.3f), easeAct, RemoveSelf::create(true), nullptr);
				t_supply->runAction(seq);
				i_supply->runAction(seq->clone());
				mainShip->addChild(t_supply,101);
				mainShip->addChild(i_supply,101);
				time = 3.0f;
				m_UIlayer->setFoodPlunderShipId(enemyShipId);
			}

			//船只被捕获特效
			if (isCapturingShip(enemyShipId))
			{
				std::string str = SINGLE_SHOP->getTipsInfo()["TIP_FIGHTING_MAINSHIP_CAPTURED"];
				Label* label = Label::create(str, "", 24);
				label->setPositionY(30);
				label->setOpacity(0);
				label->setColor(Color3B(0, 230, 108));
				label->enableOutline(Color4B::BLACK, OUTLINE_MIN);
				auto seq_1 = Sequence::create(DelayTime::create(1.2f), FadeOut::create(0.3f), nullptr);
				auto easeAct = EaseQuinticActionOut::create(Spawn::createWithTwoActions(MoveBy::create(1.0f, Vec2(0, 50)), seq_1));
				auto seq = Sequence::create(DelayTime::create(time), FadeIn::create(0.3f), easeAct, RemoveSelf::create(true), nullptr);
				label->runAction(seq);
				mainShip->addChild(label, 101);
			}
		}
	}
}
/*
*
胜利算法（暂行） 
*我方 攻击：AP1；防御：DP1；生命HP1
*敌方 攻击：AP2；防御：DP2；生命HP2 
*我方分数 = (AP1 - DP2)RANDOM(80 - 120) / HP2 
*敌方分数 = (AP2 - DP1)RANDOM(80 - 120) / HP1 
*分数高者获得单挑胜利
*/
int TVBattleManager::singledResult(TVBasicShip *ob_A, TVBasicShip *ob_B)
{
	//先屏蔽单挑
	if (false)
	{
		return 0;
	}
	//屏蔽新手引导时单挑
	if (ProtocolThread::GetInstance()->isInTutorial())
	{
		return 0;
	}
	//屏蔽海盗基地时单挑
	if (m_FightResult->fighttype == FIGHT_TYPE_ATTACK_PIRATE)
	{
		return 0;
	}
	//屏蔽主线、小伙伴任务战斗时单挑
	if (m_FightResult->fighttype == FIGHT_TYPE_TASK)
	{
		return 0;
	}
	//两船相撞（发生白刃战）的瞬间，有3%概率进入一场单挑战斗
	//小伙伴的特殊技能可以增加白刃战的几率
#if SOLO_TEST
	int odds = 100;
	int attacks = 0;
#else
	int odds = 3;
	int attacks = 0;
#endif
	if (ob_A->m_Info->_isFriend)
	{
		auto captainInfo = ob_A->getBattleCaptainInfo();
		if (captainInfo)
		{
			odds += ob_A->getBattleCaptainInfo()->add_prob;
		}
	}
	else
	{
		auto captainInfo = ob_B->getBattleCaptainInfo();
		if (captainInfo)
		{
			odds += ob_B->getBattleCaptainInfo()->add_prob;
		}		
	}

	if (cocos2d::random(0, 100) > odds)
	{
		return 0;
	}
	TVBasicShip *my_A;
	TVBasicShip *enemy_B;

	if (ob_A->m_Info->_isFriend)
	{
		my_A = ob_A;
		enemy_B = ob_B;
	}
	else
	{
		my_A = ob_B;
		enemy_B = ob_A;
	}
	auto AP1 = 100;
	auto DP1 = 80;
	auto HP1 = 200;
	auto AP2 = 100;
	auto DP2 = 80;
	auto HP2 = 200;
	int my_proto_id = 1;
	int my_is_captain = 1;
	int enemy_proto_id = 1;
	int enemy_is_captain = 1;
	if (my_A->getBattleCaptainInfo())
	{
		AP1 = my_A->getBattleCaptainInfo()->attack;
		DP1 = my_A->getBattleCaptainInfo()->defense;
		HP1 = my_A->getBattleCaptainInfo()->hp;
		my_proto_id = my_A->getBattleCaptainInfo()->proto_id;
		my_is_captain = my_A->getBattleCaptainInfo()->is_captain;
	}
	if (enemy_B->getBattleCaptainInfo())
	{
		AP2 = enemy_B->getBattleCaptainInfo()->attack;
		DP2 = enemy_B->getBattleCaptainInfo()->defense;
		HP2 = enemy_B->getBattleCaptainInfo()->hp;
		if (m_FightResult->fighttype == FIGHT_TYPE_ROB)
		{
			enemy_proto_id = enemy_B->getBattleCaptainInfo()->proto_id;
			enemy_is_captain = enemy_B->getBattleCaptainInfo()->is_captain;
		}
		else
		{
			enemy_proto_id = SINGLE_SHOP->getBattleNpcInfo().find(m_FightResult->npcid)->second.type;
			if (enemy_proto_id > 4)
			{
				enemy_proto_id = 4;
			}
			enemy_is_captain = 3;
		}
	}
#if SOLO_TEST
	auto myScore = 200;//(AP1 - DP2) * cocos2d::random(80, 120) / HP2;
	auto enemyScore = 100;//(AP2 - DP1) * cocos2d::random(80, 120) / HP1;
#else
	auto myScore = (AP1 - DP2) * cocos2d::random(80, 120) / HP2;
	auto enemyScore = (AP2 - DP1) * cocos2d::random(80, 120) / HP1;
#endif
	if (myScore > enemyScore)
	{
		//单挑取胜，isSelfVictim 敌人取胜则设置1
		WriterLog::getInstance()->addCaptainVsBattleLogItem(my_A->m_Info->_sid, enemy_B->m_Info->_sid, my_is_captain, false);

		FIGHTING_CENTER_TIP_DATA tips3 = { my_A->m_Info->_sid, my_A->m_Info->_tag, enemy_B->m_Info->_sid, enemy_B->m_Info->_tag, 1, my_proto_id, my_is_captain, enemy_proto_id, enemy_is_captain, TIP_SINGLED_START };
		this->m_UIlayer->pushFightingCentralTips(tips3);
		FIGHTING_CENTER_TIP_DATA tips1 = { my_A->m_Info->_sid, my_A->m_Info->_tag, enemy_B->m_Info->_sid, enemy_B->m_Info->_tag, 1, my_proto_id, my_is_captain, enemy_proto_id, enemy_is_captain, TIP_SINGLED_END };
		this->m_UIlayer->pushFightingCentralTips(tips1);
		FIGHTING_CENTER_TIP_DATA tips2 = { my_A->m_Info->_sid, my_A->m_Info->_tag, enemy_B->m_Info->_sid, enemy_B->m_Info->_tag, 0, 0, 0, 0, 0, TIP_SHIP_SUNK };
		this->m_UIlayer->pushFightingCentralTips(tips2);

		WriterLog::getInstance()->addShipCrashBattleLogItem(enemy_B->m_Info->_sid, 0, 3);
		return 1;
	}
	else if (myScore < enemyScore)
	{
		WriterLog::getInstance()->addCaptainVsBattleLogItem(my_A->m_Info->_sid, enemy_B->m_Info->_sid, my_is_captain, true);
		FIGHTING_CENTER_TIP_DATA tips3 = { my_A->m_Info->_sid, my_A->m_Info->_tag, enemy_B->m_Info->_sid, enemy_B->m_Info->_tag, 1, my_proto_id, my_is_captain, enemy_proto_id, enemy_is_captain, TIP_SINGLED_START };
		this->m_UIlayer->pushFightingCentralTips(tips3);
		FIGHTING_CENTER_TIP_DATA tips1 = { enemy_B->m_Info->_sid, enemy_B->m_Info->_tag, my_A->m_Info->_sid, my_A->m_Info->_tag, 1, enemy_proto_id, enemy_is_captain, my_proto_id, my_is_captain, TIP_SINGLED_END };
		this->m_UIlayer->pushFightingCentralTips(tips1);
		FIGHTING_CENTER_TIP_DATA tips2 = { enemy_B->m_Info->_sid, enemy_B->m_Info->_tag, my_A->m_Info->_sid, my_A->m_Info->_tag, 0, 0, 0, 0, 0, TIP_SHIP_SUNK };
		this->m_UIlayer->pushFightingCentralTips(tips2);
		WriterLog::getInstance()->addShipCrashBattleLogItem(my_A->m_Info->_sid, 1, 3);
		return 2;
	}
	else
	{
		FIGHTING_CENTER_TIP_DATA tips3 = { my_A->m_Info->_sid, my_A->m_Info->_tag, enemy_B->m_Info->_sid, enemy_B->m_Info->_tag, 1, my_proto_id, my_is_captain, enemy_proto_id, enemy_is_captain, TIP_SINGLED_START };
		this->m_UIlayer->pushFightingCentralTips(tips3);
		FIGHTING_CENTER_TIP_DATA tips1 = { my_A->m_Info->_sid, my_A->m_Info->_tag, enemy_B->m_Info->_sid, enemy_B->m_Info->_tag, 0, my_proto_id, my_is_captain, enemy_proto_id, enemy_is_captain, TIP_SINGLED_END };
		this->m_UIlayer->pushFightingCentralTips(tips1);
		return 3;
	}
}
void TVBattleManager::soloResult(Node * objA, Node * objB)
{
	TVBasicShip* ob_A = dynamic_cast<TVBasicShip*>(objA);
	TVBasicShip* ob_B = dynamic_cast<TVBasicShip*>(objB);
	int num1 = ob_A->m_Info->_sailornum;
	int num2 = ob_B->m_Info->_sailornum;
	if (ob_A->getBlood() > 0 && ob_B->getBlood() > 0)
	{
		auto seq = Sequence::create(DelayTime::create(3.5f), CCCallFuncN::create(CC_CALLBACK_0(TVBattleManager::delaySetCloseBattle, this, ob_A, ob_B)), nullptr);
		Battle_A_B objs;
		switch (singledResult(ob_A, ob_B))
		{
		case 0:
		case 3:
			objs = { 3, objA, objB, nullptr, nullptr };
			objs._animation = createclosefightflash(objA, objB);
			this->numofsailor(objA, objB);
			objs._sprite_bg_red = createsp_bg_red(objA, objB);
			objs._fonts = createFonts(objA, objB, num1, num2);
			objs.font1 = createfont1(objA, objB, num1, num2);
			objs._fonts->setVisible(false);
			objs.font1->setVisible(false);

			objs._animation->addChild(objs._sprite_bg_red, -1);
			objs._animation->addChild(objs._fonts, -2);
			objs._animation->addChild(objs.font1, -1);

			m_fightingObjs.push_back(objs);
			foodPlunderAndCapturingShip(ob_A, ob_B);
			break;
		case 1:
			if (ob_A->m_Info->_isFriend)
			{
				ob_B->m_Info->_sailornum = 0;
			}
			else
			{
				ob_A->m_Info->_sailornum = 0;
			}

			this->runAction(seq);
			foodPlunderAndCapturingShip(ob_A, ob_B);
			break;
		case 2:
			if (ob_A->m_Info->_isFriend)
			{
				ob_A->m_Info->_sailornum = 0;
			}
			else
			{
				ob_B->m_Info->_sailornum = 0;
			}

			this->runAction(seq);
			break;
		default:
			ob_A->setCloseBattleState(false);
			ob_B->setCloseBattleState(false);
			foodPlunderAndCapturingShip(ob_A, ob_B);
			break;
		}
	}
	else
	{
		auto seq = Sequence::create(DelayTime::create(2.f), CCCallFuncN::create(CC_CALLBACK_0(TVBattleManager::delaySetCloseBattle, this, ob_A, ob_B)), nullptr);
		this->runAction(seq);
	}

	this->m_UIlayer->flushUserDefault(ob_A);
	this->m_UIlayer->flushUserDefault(ob_B);

	//解除主舰锁定
	if (m_ShareObj->m_nLockShipTag == ob_A->m_Info->_tag || m_ShareObj->m_nLockShipTag == ob_B->m_Info->_tag)
	{

		for (size_t i = 0; i < m_ShareObj->m_vMyFriends.size(); i++)
		{
			if (m_ShareObj->m_vMyFriends[i]->m_Info->_tag == 1)
			{
				TVBattleMainShip* ship = dynamic_cast<TVBattleMainShip*>(m_ShareObj->m_vMyFriends[i]);
				std::string str = SINGLE_SHOP->getTipsInfo()["TIP_FIGHTING_MAINSHIP_UNLOCK"];
				LabelTTF* label = LabelTTF::create(str, "", 20);
				label->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(1), FadeOut::create(0.5), RemoveSelf::create(true), nullptr));
				label->setPositionY(120);
				label->setColor(Color3B::RED);
				ship->addChild(label);
				break;
			}
		}
	}

	int tag = 0;
	for (size_t i = 0; i < m_ShareObj->m_vEnemys.size(); i++)
	{
		if (m_ShareObj->m_vEnemys[i]->m_Info->_tag == tag)
		{
			TVBattleShip* ship = dynamic_cast<TVBattleShip*>(m_ShareObj->m_vEnemys[i]);
			ship->lockShip(false);
			ship->setUnlockTime(4);
			break;
		}
	}
}
