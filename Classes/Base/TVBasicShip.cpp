#include "TVBasicShip.h"
#include "TVBattleManager.h"
#include "Bullet.h"

#include "SystemVar.h"
#include "WriterLog.h"

#define OFFSET 50.0f
#define CHECK_RADIUS 5.0f

#define SHIP_HEAD_33 70
#define SHIP_HEAD_38 120

#define SHIP_SPEED_COEF  100.0
#define SHIP_STEER_COEF  100.0

TVBasicShip::TVBasicShip()
	:m_PreTime(0)
	,m_pBloodIcon(nullptr)
	,m_pFlagIcon(nullptr)
	,m_pDelegate(nullptr)
	,m_pShipIcon(nullptr)
	,m_pMyMainPath(nullptr)
	,m_BufferList(nullptr)
	,m_BufferTarget(nullptr)
	,m_eType(FLOW_SHIP)
	,m_eMode(MODEL_FREE)
	,m_bIsDied(false)
	,m_BattleState(false)
	,m_PrepareFire_OK(true)
	,m_ShipStatus(1)
	, m_fCDTimeBasis(10.0)
	,m_fCDTime(10.0)
	,m_fBlood(0)
	,isFired(false)
	,m_bIsMoveEnd(true)
{
	m_fMaxHp = 0;
	m_Info = new Ship_info;
	m_Info->_attack = 0;
	m_Info->_sid = 1;
	m_Info->_type = 1;
	m_Info->_activeCritSize = 0;
	m_Info->_isSlowSpeedTime = 0;
	m_Info->_isFireEfficeharm = 0;
	m_Info->_captureNum = 0;
	m_Info->_bowgun_attack = 0;
	m_fRotationSpeed = 1.f;
	m_fMoveSpeed = 1.f;
	m_fCurSpeed = 1.f;
	m_speedFactor = 1;
	m_ExpediteSpeed_flag = 0;
	m_LaunchDistance_max = 320;
	m_LaunchDistance_min = 118;
	m_LaunchDistance_mid = 224;
	m_CurLaunchDistance = 3;// not cannons
	m_pShipWave = nullptr;

	m_Launch_max = 0;
	m_Launch_mid = 0;
	m_Launch_min = 0;

	m_currentDirect = 0;
	m_ShortDefense_per = 0;
	m_ShortdAttack_per = 0;
	m_RangedAttack_per = 0;
	m_moral_per = 0;

	m_PassiveSkillsNum = 0;
	mPassiveSkills = 0;

	m_pCDTimeBg=nullptr;
	m_pResult=nullptr;
	map=nullptr;
	m_pTouchListener=nullptr;
	m_PreExpediteTime=0;
	m_RepairEnable=false;
	m_NextRotation=0;
	m_ExpediteSpeed = 0;
	m_pSailorLabel = nullptr;
	m_isSlowSpeed = false;
	m_isOnFired = false;
	m_slowSpeedTime = 0;
	m_onFiredTime = 0;
	m_SkillFlushTime = 0;
	m_firdHarm = 0;
	m_Indomitable_Will_prob = 0;
	m_Roar_Victory_prob = 0;
	m_nProgressiveRepairTime = 0;
	m_nUnlockTime = 0;
	spriteEffectSmoke = nullptr;
	spriteEffectSpalsh = nullptr;
	m_bEndGame = false;
	m_fTempSpeed = 0;
	m_fireShipSide = 0;
	m_pPosition = Vec2(0, 0);
	m_pHitPosition = Vec2(0, 0);
}

TVBasicShip::~TVBasicShip()
{
	delete m_Info;

	if (m_PassiveSkillsNum > 0)
	{
		for (int i = 0; i < m_PassiveSkillsNum; i++)
		{
			delete mPassiveSkills[i];
		}
		delete[] mPassiveSkills;
	}
}

void TVBasicShip::onEnter()
{
	Node::onEnter();
	m_pShipWave = ParticleSystemQuad::create("wave11.plist");
	m_pShipWave->setPositionType(ParticleSystem::PositionType::RELATIVE);
	m_pShipWave->setPosition(Vec2(-1000,-1000));
	m_pShipWave->setScaleX(0.6);
	_parent->addChild(m_pShipWave,2);
	Camera  *c  = Director::getInstance()->getRunningScene()->getCameras().front();
	m_cameraStartPostion = c->getPosition();
}





void TVBasicShip::onExit()
{
	if (!m_Info->_isFriend)
	{
		_eventDispatcher->removeEventListener(m_pTouchListener);
	}
	Node::onExit();
}


SkillDefine* TVBasicShip::getPassiveSkillsById(int id)
{
	SkillDefine* skillDef = nullptr;
	for (size_t i = 0; i < m_PassiveSkillsNum; i++)
	{
		if (mPassiveSkills[i]->id == id)
		{
			skillDef = mPassiveSkills[i];
		}
		
	}

	return skillDef;

}

/*
 * 返回当前的准确速度，考虑船只是否在运动。
 */
float TVBasicShip::getCurrentSpeed(){
	if(m_bIsMoveEnd){
		return 0;
	}
	return m_fCurSpeed;
}

void TVBasicShip::setPassiveSkills(FightShipInfo* sfi)
{
	m_PassiveSkillsNum = sfi->n_skills + sfi->n_flagship_companion_skills;
	if (m_PassiveSkillsNum > 0){
		mPassiveSkills = new SkillDefine *[m_PassiveSkillsNum];
		for (int i = 0; i < m_PassiveSkillsNum; i++)
		{
			mPassiveSkills[i] = new SkillDefine;
			if (i < sfi->n_skills)
			{
				memcpy(mPassiveSkills[i], sfi->skills[i], sizeof(SkillDefine));
			}
			else
			{
				memcpy(mPassiveSkills[i], sfi->flagship_companion_skills[i - sfi->n_skills], sizeof(SkillDefine));
			}
		}
	}
}



void TVBasicShip::playCaptureEffect()
{
	int num = this->m_Info->_captureNum;

	auto sprite = Sprite::create();
	sprite->setTexture("ship/battle_getsailor.png");
	sprite->setAnchorPoint(Vec2(1, 0));
	sprite->setPosition(Vec2(30, 30));
	this->addChild(sprite);

	char str[10];
	sprintf(str, "%d", num);



	TextAtlas* capNums = TextAtlas::create(str, IMAGE_FONT[3], 30, 32, "/");
	//std::string font_1 = StringUtils::format("+%d", num);
	capNums->setAnchorPoint(Vec2(0, 0));
	capNums->setString(StringUtils::format("+%d", num));
	capNums->setPosition(Vec2(40, 30));
	
	

	auto easeAct = EaseQuinticActionOut::create(MoveBy::create(2.f, Vec2(0, 30)));
	auto seq = Sequence::create(DelayTime::create(1.9f), FadeIn::create(0.1f), easeAct, CallFuncN::create([=](Ref *pSender){
		auto node = (Node*)(pSender);
		node->removeFromParent();
	}), nullptr);
	capNums->runAction(seq);
	this->addChild(capNums);

	sprite->runAction(seq->clone());




	this->m_Info->_captureNum = 0;

}

void TVBasicShip::addPassiveSkillsToShip(Ship_info* shipInfo)
{
	auto skills = mPassiveSkills;
	int n_skills = m_PassiveSkillsNum;
	srand(time(0));
	//在打劫时技能能是玩家的技能
	if (m_Info->_isFriend || m_nFightType == FIGHT_TYPE_ROB){
		//旗舰
		if(m_eType == MAIN_SHIP){
			
			for (int i = 0; i < n_skills; i++)
			{
				int id = skills[i]->id;
				int level = skills[i]->level;
				if (skills[i]->skilltype == SKILL_TYPE_PARTNER_SPECIAL)
				{
					//小伙伴特殊技能强力火炮
					if (id == 2)
					{
						//玩家没有使用强力火炮时
						if (shipInfo->_activeCritSize == 0)
						{
							//概率
							if (cocos2d::random(0, 100) <= 20)
							{
								shipInfo->_activeCritSize = 1000;
							}
						}
					}
				}
				else if (skills[i]->skilltype == SKILL_TYPE_PLAYER)
				{
					auto skillTrees = SINGLE_SHOP->getSkillTrees();
					SKILL_TREE skillTree = skillTrees.find(id)->second;
					switch (id)
					{
						case SKILL_GREAT_SHOOTER:
							shipInfo->_attack_accuracy += skillTree.effect_per_lv * level;
							break;
						default:
							break;
					}
				}
				else if (skills[i]->skilltype == SKILL_TYPE_COMPANION_NORMAL)
				{
					auto skillTrees = SINGLE_SHOP->getCaptainSkillInfo();
					SKILL_TREE skillTree = skillTrees.find(id)->second;
					switch (id)
					{
					case SKILL_COMPANION_NORMAL_GUN_EXPERT:
						shipInfo->_change_attack += skillTree.effect_per_lv*level;
						break;
					case SKILL_COMPANION_NORMAL_HAMMER_SHOOT:
						if (skillTree.effect_per_lv * level >= cocos2d::random(0, 100))
						{
							shipInfo->_isSlowSpeedTime = 5;
							m_isSlowSpeed = true;
							m_fTempSpeed -= m_fMoveSpeed * 0.4;
						}
						break;
					case SKILL_COMPANION_NORMAL_WEAKNESS_ATTACK:
						shipInfo->_critSize += skillTree.rate_per_lv*level;;
						break;
					case SKILL_COMPANION_NORMAL_ACCURACY:
						shipInfo->_attack_accuracy += skillTree.effect_per_lv * level;
						break;
					default:
						break;
					}
				}
				else if (skills[i]->skilltype == SKILL_TYPE_CAPTAIN)
				{
					auto skillTrees = SINGLE_SHOP->getCaptainSkillInfo();
					SKILL_TREE skillTree = skillTrees.find(id)->second;
					switch (id)
					{
					case SKILL_CAPTAIN_GUN_EXPERT:
						shipInfo->_change_attack += skillTree.effect_per_lv*level;
						break;
					case SKILL_CAPTAIN_WEAKNESS_ATTACK:
						shipInfo->_critSize += skillTree.rate_per_lv*level;
						break;
					case SKILL_CAPTAIN_HAMMER_SHOOT:
						if (skillTree.effect_per_lv * level >= cocos2d::random(0,100))
						{
							shipInfo->_isSlowSpeedTime = 5;
							m_isSlowSpeed = true;
							m_fTempSpeed -= m_fMoveSpeed * 0.4;
						}
						break;
					case SKILL_CAPTAIN_ACCURACY:
						shipInfo->_attack_accuracy += skillTree.effect_per_lv * level;
						break;
					default:
						break;
					}
				}
			}
		}else{
			for (int i = 0; i < n_skills; i++)
			{
				if (skills[i]->skilltype == SKILL_TYPE_CAPTAIN)
				{
					auto skillTrees = SINGLE_SHOP->getCaptainSkillInfo();
					int id = skills[i]->id;
					int level = skills[i]->level;
					SKILL_TREE t = skillTrees.find(id)->second;
					switch (id)
					{
					case SKILL_CAPTAIN_GUN_EXPERT:
						shipInfo->_change_attack += t.effect_per_lv*level;
						break;
					case SKILL_CAPTAIN_WEAKNESS_ATTACK:
						shipInfo->_critSize += t.rate_per_lv*level;
						break;
					case SKILL_CAPTAIN_HAMMER_SHOOT:
						if (t.effect_per_lv * level >= cocos2d::random(0, 100))
						{
							shipInfo->_isSlowSpeedTime = 5;
							m_isSlowSpeed = true;
							m_fTempSpeed -= m_fMoveSpeed * 0.4;
							
						}
						break;
					case SKILL_CAPTAIN_ACCURACY:
						shipInfo->_attack_accuracy += t.effect_per_lv * level;
						break;
					default:
						break;
					}
				}
				else if (skills[i]->skilltype == SKILL_TYPE_COMPANION_NORMAL)
				{
					auto skillTrees = SINGLE_SHOP->getCompanionNormalSkillInfo();
					int id = skills[i]->id;
					int level = skills[i]->level;
					SKILL_TREE t = skillTrees.find(id)->second;
					switch (id)
					{
					case SKILL_COMPANION_NORMAL_GUN_EXPERT:
						shipInfo->_change_attack += t.effect_per_lv*level;
						break;
					case SKILL_COMPANION_NORMAL_HAMMER_SHOOT:
						if (t.effect_per_lv * level >= cocos2d::random(0, 100))
						{
							shipInfo->_isSlowSpeedTime = 5;
							m_isSlowSpeed = true;
							m_fTempSpeed -= m_fMoveSpeed * 0.4;

						}
						break;
					case SKILL_COMPANION_NORMAL_WEAKNESS_ATTACK:
						shipInfo->_critSize += t.rate_per_lv*level;
						break;
					case SKILL_COMPANION_NORMAL_ACCURACY:
						shipInfo->_attack_accuracy += t.effect_per_lv * level;
						break;
					default:
						break;
					}
				}
				else if (skills[i]->skilltype == SKILL_TYPE_PARTNER_SPECIAL)
				{
					int id = skills[i]->id;
					//小伙伴特殊技能强力火炮
					if (id == 2)
					{
						//玩家没有使用强力火炮时
						if (shipInfo->_activeCritSize == 0)
						{
							//概率
							if (cocos2d::random(0, 100) <= 20)
							{
								shipInfo->_activeCritSize = 1000;
							}
						}
					}
				}
			}
		}
	}else{
		//敌人
		auto skillTrees = SINGLE_SHOP->getNPCSkillInfo();
		for (int i = 0; i < n_skills; i++)
		{
			int id = skills[i]->id;
			int level = skills[i]->level;
			log("skill %d , level %d", id, level);
			//敌人的skill数据是错误的，需要生成json
			SKILL_TREE t = skillTrees.find(id)->second;
			switch(id)
			{
			case 1001://Gun Expert
				shipInfo->_change_attack += t.effect_per_lv*level;
				log("enemy additional attack %d", shipInfo->_change_attack);
				break;
			case 1003:
				if (t.effect_per_lv * level >= cocos2d::random(0, 100))
				{
					shipInfo->_isSlowSpeedTime = 5;
				}
				break;
			case 1004:
				shipInfo->_critSize += t.rate_per_lv*level;
				break;
			case 1008://精准射击
				shipInfo->_attack_accuracy += t.effect_per_lv * level;
				break;
			default:
				break;
			}
		}
	}
}

void TVBasicShip::addTouchEvent(float f)
{
	if (!m_Info->_isFriend)
	{
		auto listener = EventListenerTouchOneByOne::create();

		listener->onTouchBegan = CC_CALLBACK_2(TVBasicShip::onTouchBegan, this);
		listener->onTouchEnded = CC_CALLBACK_2(TVBasicShip::onTouchEnded, this);
		listener->onTouchMoved = CC_CALLBACK_2(TVBasicShip::onTouchMoved, this);
		listener->onTouchCancelled = CC_CALLBACK_2(TVBasicShip::onTouchCancelled, this);
		listener->setSwallowTouches(true);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
		m_pTouchListener = listener;
	}
}

bool TVBasicShip::init()
{
	bool pRet = false;
	do 
	{
		auto sp = Sprite::create();
		sp->setTexture("ship/hp.png");
		auto sp_bg = ImageView::create("ship/ship_state_2.png");
		auto sp_flag = Sprite::create();
		sp_flag->setTexture("ship/gun_progress_1.png");
		sp->setTextureRect(Rect(44, 2.5, 88, 5));
		if (m_Info->_tag == 1)
		{
			m_pCDTimeBg = Sprite::create();
			m_pCDTimeBg->setTexture("ship/ship_state_2.png");
			sp_flag->setTextureRect(Rect(44, 2.5, 88, 5));
		}
		else
		{
			m_pCDTimeBg = Sprite::create();
			m_pCDTimeBg->setTexture("ship/ship_state_4.png");
			sp_flag->setTextureRect(Rect(44.5, 1, 89, 2));
		}

		ImageView* captainIcon = ImageView::create(getPositionIconPath(m_Info->_tag));

		m_pBloodIcon = ProgressTimer::create(sp);
		m_pFlagIcon = ProgressTimer::create(sp_flag);
		m_pShipIcon = Sprite::create();
		m_pShipIcon->setAnchorPoint(Vec2(0.5f,0.5f));
		m_pShipIcon->setTexture(getShipTopIconPath(m_Info->_sid));
		float shipScale = 0.5;
		//攻击海盗基地时海盗船放大（100分值数值）
		if (!m_Info->_isFriend && m_nFightType == FIGHT_TYPE_ATTACK_PIRATE)
		{
			shipScale = 0.5f * m_pResult->model_modify / 100;
		}
		m_pShipIcon->setScale(shipScale);
		
		Size ship_size = Size(SINGLE_SHOP->getShipData().find(m_Info->_sid)->second.collision_a, SINGLE_SHOP->getShipData().find(m_Info->_sid)->second.collision_b);
		m_pyBodySize = ship_size * shipScale;
		auto pyBody = PhysicsBody::createBox(Size(m_pyBodySize));
		pyBody->setGravityEnable(false);
		m_pShipIcon->setPhysicsBody(pyBody);

		int num_sailor = m_Info->_sailornum;
		std::string s_nsailor = StringUtils::format("%d", num_sailor);
		m_pSailorLabel = Text::create(s_nsailor, "", 30);
		m_pSailorLabel->enableOutline(Color4B::BLACK, OUTLINE_MIN);
		ImageView* p_sailor = ImageView::create("ship/sailor.png");
		p_sailor->setPosition(Vec2(-p_sailor->getContentSize().width / 2, m_pShipIcon->getContentSize().height / 2 + p_sailor->getContentSize().height / 2 + 5));
		m_pSailorLabel->setPosition(Vec2(m_pSailorLabel->getContentSize().width / 2, m_pShipIcon->getContentSize().height / 2 + p_sailor->getContentSize().height / 2 + 5));
		this->addChild(p_sailor,2, 114);
		this->addChild(m_pSailorLabel, 2);
		this->addChild(m_pShipIcon,2);
		this->addChild(captainIcon,3, 115);
		this->addChild(sp_bg,3,113);
		this->addChild(m_pCDTimeBg,3);
		this->addChild(m_pBloodIcon,4);
		this->addChild(m_pFlagIcon,4);

		auto shipSize = m_pShipIcon->getContentSize() * m_pShipIcon->getScale();
		this->setContentSize(shipSize - Size(10,10));
		
		m_pBloodIcon->setPosition(0,- shipSize.height);
		sp_bg->setPosition(Vec2(0, - shipSize.height));
		if (m_Info->_tag == 1)
		{
			m_pFlagIcon->setPosition(0, m_pBloodIcon->getPositionY() - sp_bg->getContentSize().height / 2 - 4);
			m_pCDTimeBg->setPosition(0, m_pBloodIcon->getPositionY() - sp_bg->getContentSize().height / 2 - 4);
		}
		else
		{
			m_pFlagIcon->setPosition(0, m_pBloodIcon->getPositionY() - sp_bg->getContentSize().height / 2 - 2);
			m_pCDTimeBg->setPosition(0, m_pBloodIcon->getPositionY() - sp_bg->getContentSize().height / 2 - 2);
		}
		

		if (m_Info->_isFriend)
		{
			captainIcon->setPosition(Vec2(-m_pBloodIcon->getContentSize().width / 2 - captainIcon->getContentSize().width / 2 - 10, -shipSize.height - m_pBloodIcon->getContentSize().height / 2 - m_pCDTimeBg->getContentSize().height / 2 + 5));
		}
		else
		{
			captainIcon->setPosition(Vec2(-m_pBloodIcon->getContentSize().width / 2 - captainIcon->getContentSize().width / 2 - 10, -shipSize.height - m_pBloodIcon->getContentSize().height / 2));
			m_pCDTimeBg->setVisible(false);
			m_pFlagIcon->setVisible(false);
		}
		
		
		m_pShipIcon->setPosition(0,0);
		m_pBloodIcon->setType(ProgressTimer::Type::BAR);
		m_pFlagIcon->setType(ProgressTimer::Type::BAR);
		m_pBloodIcon->setBarChangeRate(Vec2(1,0));
		m_pFlagIcon->setBarChangeRate(Vec2(1,0));
		m_pBloodIcon->setMidpoint(Vec2(0,0));
		m_pFlagIcon->setMidpoint(Vec2(0,0));

		m_pBloodIcon->setPercentage(100.0 * m_fBlood/m_fMaxHp);
		m_pFlagIcon->setPercentage(0.f);

		m_BufferList = ListView::create();
		this->addChild(m_BufferList);
		m_BufferList->setPosition(Vec2(0,shipSize.height * 1.5));
		m_BufferList->setDirection(ScrollView::Direction::HORIZONTAL);
		m_BufferList->setContentSize(Size(0,50));

		//没有火炮时不显示火炮cd
		if (m_pResult->n_cannons < 1 /*&& m_pResult->aftergun == nullptr*/)
		{
			m_pCDTimeBg->setVisible(false);
			m_pFlagIcon->setVisible(false);
		}

		pRet = true;
	} while (0);
	return pRet;
}

void TVBasicShip::initShipInfo(FightShipInfo* sfi,TVBattleManager*manager)
{

	m_battleManager = manager;
	m_pResult = sfi;
	float tempTime = 0;
	for (int i = 0; i < sfi->n_cannons; i++)
	{
		if (sfi->cannons[i]->range == 3)
		{
			m_Launch_max += sfi->cannons[i]->attack;
		}else if (sfi->cannons[i]->range == 2)
		{
			m_Launch_mid += sfi->cannons[i]->attack;
		}else
		{
			m_Launch_min += sfi->cannons[i]->attack;
		}
		ITEM_RES item = SINGLE_SHOP->getItemData()[sfi->cannons[i]->iid];
		int b = item.property4;
		float tempTime2 = 0;
		switch (b)
		{
		case 1:{
				   tempTime2 = m_fCDTimeBasis * 1.0;
				   break;
		}
		case 2:{
				   tempTime2 = m_fCDTimeBasis * 1.2;
				   break;
		}
		case 3:{
				   tempTime2 = m_fCDTimeBasis * 2.0;
				   break;
		}
		case 4:{
				   tempTime2 = m_fCDTimeBasis * 0.5;
				   break;
		}
		default:
			break;
		}
		if (tempTime < tempTime2)
		{
			tempTime = tempTime2;
		}
	}
	m_fCDTimeBasis = tempTime;
	m_fCDTime = tempTime;
	if (m_eType == MAIN_SHIP)
	{
		log("main_ship:m_fCDTimeBasis:%0.2f,m_fCDTime:%0.2f", m_fCDTimeBasis, m_fCDTime);
	}
	if (sfi->bowiconiid == 33)  // 中级船首相战神
	{
		m_RangedAttack_per += SHIP_HEAD_33;
	}else if(sfi->bowiconiid == 38) // 高级船首相战神
	{
		m_RangedAttack_per += SHIP_HEAD_38;
	}
	
	
	setCurLaunch();
	m_Info->_sid = sfi->sid;
	m_Info->_defense = sfi->defense;
	if (m_battleManager->getContinueLastFight())
	{
		std::string  st_sailor;
		std::string  st_blood;
		if (m_Info->_isFriend)
		{
			st_sailor = StringUtils::format(MY_SHIP_SAILOR, m_Info->_tag);
			st_blood = StringUtils::format(MY_SHIP_BLOOD, m_Info->_tag);
		}
		else
		{
			st_sailor = StringUtils::format(ENEMY_SHIP_SAILOR, m_Info->_tag - 5);
			st_blood = StringUtils::format(ENEMY_SHIP_BLOOD, m_Info->_tag - 5);
		}
		auto sailornum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_sailor.c_str()).c_str(), 0);
		auto bloodnum = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_blood.c_str()).c_str(), 0);
		m_Info->_sailornum = sailornum;
		m_fBlood = bloodnum;
	}
	else
	{
		m_Info->_sailornum = sfi->sailornum;
		m_fBlood = sfi->current_hp;
	}
	
	m_Info->_sailBeginNum = sfi->sailornum;
	m_Info->_extAttack = 0;
	m_Info->_critSize = 0;
	m_Info->_change_attack = 0;
	m_Info->_change_defense = 0;
	if (sfi->bowgun)
	{
		m_Info->_bowgun_attack = sfi->bowgun->attack;
	}
	
	m_fMoveSpeed = sfi->speed / SHIP_SPEED_COEF;
	m_fCurSpeed = m_fMoveSpeed;
//	log("battle speed: %f", m_fMoveSpeed);
	m_fRotationSpeed = sfi->steer_speed / SHIP_STEER_COEF;
	m_fMaxHp = sfi->max_hp;
	generateSkills(sfi->skills,sfi->n_skills);
	generateSkills(sfi->flagship_companion_skills, sfi->n_flagship_companion_skills);
	setPassiveSkills(sfi);
}

void TVBasicShip::setCurLaunch()
{
	auto isHaveRange = [&](int range)->bool
	{
		for (unsigned int i = 0; i < m_pResult->n_cannons;i++)
		{
			if(m_pResult->cannons[i]->range == range)
				return true;
		}
		return false;
	};

	if (isHaveRange(3))
	{
		m_CurLaunchDistance = m_LaunchDistance_max;
		return;
	}
	if (isHaveRange(2))
	{
		m_CurLaunchDistance = m_LaunchDistance_mid;
		return;
	}
	if (isHaveRange(1))
	{
		m_CurLaunchDistance = m_LaunchDistance_min;
	}
}

void TVBasicShip::startBattle()
{
	this->scheduleUpdate();
	if (!m_pMyMainPath)
	{
		addTouchEvent(0);
		m_pMyMainPath = MyDrawLine::create();
		_parent->addChild(m_pMyMainPath, 2);
		m_pDelegate->specialButtonEvent(this, "event_attacked", m_fBlood / m_fMaxHp);
	}
}

void TVBasicShip::stopBattle()
{
	this->unscheduleUpdate();
}

void TVBasicShip::setDelegate(TVBattleDelegate *pDelegate)
{
	m_pDelegate = pDelegate;
	map = m_pDelegate->getMap();
}

void TVBasicShip::fired(TVBasicShip* target)
{
	if (m_bIsDied)
	{
		return;
	}

#ifdef WIN32  // test fire 
	switch (m_Info->_tag)
	{
	case 1:
		log("1__%d",this->_tag);
		break;
	case 2:
		log("2__%d",this->_tag);
		break;
	case 3:
		log("3__%d",this->_tag);
		break;
	case 4:
		log("4__%d",this->_tag);
		break;
	case 5:
		log("5__%d",this->_tag);
		break;
	case 6:
		log("6__%d",this->_tag);
		break;
	case 7:
		log("7__%d",this->_tag);
		break;
	case 8:
		log("8__%d",this->_tag);
		break;
	case 9:
		log("9__%d",this->_tag);
		break;
	case 10:
		log("10__%d",this->_tag);
		break;
	default:
		break;
	}
	
#endif

	m_BulltIndex.clear();
	int bulletNum = m_Cannons.size();
	
	for (int i = 0; i < bulletNum; i++)
	{
		auto bullet = Bullet::createBullet(_parent, m_Info->_sid, m_pResult->cannons[i]->cannonid);
		m_Info->_attack = m_Cannons[i];

		//get factor only.
		m_Info->_extAttack = getRangeAttackFactor();
		//默认给所有船只30%的准确命中率
		m_Info->_attack_accuracy = 300;

		bullet->setBulletInfo(m_Info);
		bullet->setAttack(m_Cannons[i]);
		if(m_eType == MAIN_SHIP){
			log("bullet to:(%f,%f)",target->getPosition().x,target->getPosition().y);
		}
		bullet->setDestination(target->getPosition());//被击打的位置
		bullet->setOffsetDirect(target->getDirect());

		bullet->setTargetSpeed(target->getCurrentSpeed());
		
		bullet->setDelegate(m_pDelegate);
		bullet->setAnchorPoint(Vec2(0.5f,0.5f));
		//被动技能加成
		addPassiveSkillsToShip(bullet->getBulletInfo());
		m_BulltIndex.pushBack(bullet);

		m_pHitPosition = target->getPosition();
		m_pPosition =  bullet->getPosition();
	}
	
	m_ShipStatus = SHIP_FIRED;

	//log("battle cannon:%d", bulletNum);
	if(bulletNum ==1){
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FIRE_CANNON_07);
	}else{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FIRE_CANNON_08);
	}

//bullets should have fire delay time!!!!!!!!!! Don't use the stupid code here!
#if 1
	for (int i = 0; i < bulletNum; i++)
	{
		float time = 0.05f * i;
		runAction(Sequence::createWithTwoActions(DelayTime::create(time),CCCallFunc::create(CC_CALLBACK_0(TVBasicShip::fireCallback,this,i==bulletNum))));
	}
#else
	runAction(Repeat::create(Sequence::create(DelayTime::create(0.05f),CCCallFunc::create(CC_CALLBACK_0(TVBasicShip::fireCallback,this)),nullptr),bulletNum));
#endif


	if (m_Info->_tag < 6)
	{
		m_pDelegate->changeShipInfo(SKILL_ATTACKING_HORN,m_Info->_tag);
	}
	
	if (m_Info->_tag == 1)
	{
		m_pDelegate->changeShipInfo(SKILL_POWWEFUL_SHOOT);
		m_pDelegate->changeShipInfo(SKILL_INCENDIARY_SHOOT);
		m_pDelegate->changeShipInfo(SKILL_HAMMER_SHOOT);
	}
	isFired = true;
	Vec2 pos = target->convertToWorldSpace(Point(0, 0));

	Vec2 pos2 = m_pShipIcon->convertToNodeSpace(pos);
	log("degree%.2f", CC_RADIANS_TO_DEGREES(pos2.getAngle()));
	//m_pPosition = m_pShipIcon->getPosition();
	if (m_fireShipSide != 2)
	{
		if (0 < CC_RADIANS_TO_DEGREES(pos2.getAngle()))
		{
			m_fireShipSide = 0;
		}
		else if (0 > CC_RADIANS_TO_DEGREES(pos2.getAngle()))
		{
			m_fireShipSide = 1;
		}
		
	}

	fireeffective(bulletNum);
}

void TVBasicShip::firedByDistance(TVBasicShip* target,int distance)
{
	if (m_pResult->n_cannons < 1)
	{
		return;
	}

	m_Cannons.clear();
	float offset = 0;
	if (distance <= m_LaunchDistance_min + offset)
	{
		//m_Info->_attack = m_Launch_min + m_Launch_mid + m_Launch_max;
		for (int i = 0; i < m_pResult->n_cannons;i++)
		{
			if(m_pResult->cannons[i]->range == 1)
				m_Cannons.push_back(m_pResult->cannons[i]->attack);
		}
	}
	if(distance <= m_LaunchDistance_mid + offset)
	{
		//m_Info->_attack = m_Launch_mid + m_Launch_max;
		for (int i = 0; i < m_pResult->n_cannons;i++)
		{
			if(m_pResult->cannons[i]->range == 2)
				m_Cannons.push_back(m_pResult->cannons[i]->attack);
		}
	}
	if(distance <= m_LaunchDistance_max + offset)
	{
		//m_Info->_attack = m_Launch_max;
		for (int i = 0; i < m_pResult->n_cannons;i++)
		{
			if(m_pResult->cannons[i]->range == 3)
				m_Cannons.push_back(m_pResult->cannons[i]->attack);
		}
	}

	if (m_Cannons.size()==0)
	{
		return;
	}
	fired(target);
}

void TVBasicShip::fireCallback(bool isLastOne)
{
	int bullet_nums = m_BulltIndex.size() - 1;
	if (bullet_nums < 0)
	{
		return;
	}
	int index = cocos2d::random(0,bullet_nums);
	Bullet *bullet = dynamic_cast<Bullet*>(m_BulltIndex.at(index));
	m_BulltIndex.erase(index);
	int a = (bullet_nums+1) %3;

	Size shipContent = Size(m_pShipIcon->getContentSize().width * cos(CC_DEGREES_TO_RADIANS(m_currentDirect)),m_pShipIcon->getContentSize().height * sin(CC_DEGREES_TO_RADIANS(m_currentDirect))) * m_pShipIcon->getScale();
	Vec2 desPoss[] = {Vec2(shipContent.width/4,shipContent.height/4),Vec2(0,0),Vec2(-shipContent.width/4,-shipContent.height/4)};
	Size targetContent = m_pyBodySize*0.5f;//shipContent * 0.5f;

	Vec2 desPoss2[] = {Vec2(targetContent.width/4,targetContent.height/4),Vec2(0,0),Vec2(-targetContent.width/4,-targetContent.height/4)};
	bullet->setPosition(_parent->convertToNodeSpace(convertToWorldSpaceAR(m_pShipIcon->getPosition())) + desPoss[a]);

	Vec2 direct = desPoss2[a];
	//direct.x=desPoss2[a].x*cos(CC_DEGREES_TO_RADIANS(bullet->getTargetDirect())) - desPoss2[a].y*sin(CC_DEGREES_TO_RADIANS(bullet->getTargetDirect()));
	//direct.y=desPoss2[a].x*sin(CC_DEGREES_TO_RADIANS(bullet->getTargetDirect())) + desPoss2[a].y*cos(CC_DEGREES_TO_RADIANS(bullet->getTargetDirect()));

	if(desPoss[a].x*direct.x<0){//保持方向一直
		direct.x = - direct.x;
	}

	if(desPoss[a].y*direct.y<0){
		direct.y = -direct.y;
	}

	bullet->setDestination(bullet->getDestination() + direct);
	bullet->show();
	bullet->release();
}

std::vector<TVBasicShip*>* TVBasicShip::getEnemyShips(int* attFlagship)
{
	std::vector<TVBasicShip*>* EnemyShips;
	int attackTag;
	if (m_Info->_isFriend)
	{
		EnemyShips = &(m_pDelegate->getShareObj()->m_vEnemys);
		attackTag = m_pDelegate->getShareObj()->m_nLastAttackFlagshipTag;
	}else
	{
		EnemyShips = &(m_pDelegate->getShareObj()->m_vMyFriends);
		attackTag = m_pDelegate->getShareObj()->m_nLastAttackFlagShipTag_Enemy;
	}
	if (attFlagship)
	{
		*attFlagship = attackTag;
	}
	return EnemyShips;
}

std::vector<TVBasicShip*>* TVBasicShip::getFriendShips()
{
	std::vector<TVBasicShip*>* Ships;
	//int attackTag;
	if (m_Info->_isFriend)
	{
		Ships = &(m_pDelegate->getShareObj()->m_vMyFriends);
		//attackTag = m_pDelegate->getShareObj()->m_nLastAttackFlagshipTag;
	}else
	{
		Ships = &(m_pDelegate->getShareObj()->m_vEnemys);
		//attackTag = m_pDelegate->getShareObj()->m_nLastAttackFlagShipTag_Enemy;
	}
	return Ships;
}

void TVBasicShip::update(float f)
{
	m_speedFactor = f / (1.0 / 60);
	if (m_speedFactor >= 2.5){
		m_speedFactor = 1;
	}
	if (m_vRoad.size()>0 && m_vRoad[0].turnR != 0){
		m_bIsTurning = true;
	}else{
		m_bIsTurning = false;
	}

	correctPysics();
	correctWave();
	checkShipDied();
	repairShip();
	checkExpediteSpeed(f);

	m_SkillFlushTime += f;
	if (m_SkillFlushTime > 0.999f)
	{
		m_SkillFlushTime = 0;
		if (m_isOnFired)
		{
			m_onFiredTime--;
			if (m_onFiredTime < 1)
			{
				m_isOnFired = false;
				deleteBuffer(STATE_FIRE);
			}else
			{
				float num = m_fMaxHp*m_firdHarm/(SKILL_DER_MAX*20);
				m_fBlood -= num;
				setBloodIconValue(m_fBlood);
			}
		}
		if (m_isSlowSpeed)
		{
			m_slowSpeedTime--;
			if (m_slowSpeedTime < 1)
			{
				deleteBuffer(STATE_SLOWSPEED);
				m_isSlowSpeed = false;
				m_fTempSpeed += m_fMoveSpeed *0.4;
			}
		}

		m_nUnlockTime--;
	}
}


void TVBasicShip::checkExpediteSpeed(float delt)
{
	float tempMaxSpeed;
	if(m_bIsTurning){ //turning.
		tempMaxSpeed = m_fLimitMoveSpeed;// + m_fTempSpeed;
		m_fCurSpeed = tempMaxSpeed;
		m_PreExpediteTime = 0;
	}else{
		tempMaxSpeed= m_fMoveSpeed + m_fTempSpeed;
	
		m_PreExpediteTime += int(delt * 1000);
		if (m_PreExpediteTime > 180) //180毫秒加速一次
		{
			if (m_fCurSpeed < tempMaxSpeed)
			{
				log("increase speed %f",m_ExpediteSpeed * 1000 /m_PreExpediteTime);
				m_fCurSpeed += m_ExpediteSpeed * 1000 /m_PreExpediteTime;
				if(m_fCurSpeed > tempMaxSpeed){
					m_fCurSpeed = tempMaxSpeed;
				}
			}else
			{
				//减速
				m_fCurSpeed = tempMaxSpeed;
			}
			m_PreExpediteTime = 0;
		}
	}


//	if (m_ExpediteSpeed_flag == 1)
//	{
//
//	}
//	if (m_ExpediteSpeed_flag == 2)
//	{
//		m_PreExpediteTime += int(delt * 1000);
//		if (m_PreExpediteTime > 180)
//		{
//			if (m_fCurSpeed > 0)
//			{
//				m_fCurSpeed -= m_ExpediteSpeed * 1000 /m_PreExpediteTime;
//			}else
//			{
//				m_fCurSpeed = 0;
//				m_ExpediteSpeed_flag = 0;
//			}
//			m_PreExpediteTime = 0;
//		}
//	}
//
//	if (m_ExpediteSpeed_flag == 0)
//	{
//		m_PreExpediteTime += int(delt * 1000);
//
//		if (m_PreExpediteTime > 180)
//		{
//			if (m_fCurSpeed > tempMaxSpeed)
//			{
//				m_fCurSpeed -= m_ExpediteSpeed * 1000 / m_PreExpediteTime;
//			}
//			else
//			{
//				m_fCurSpeed = tempMaxSpeed;
//			}
//			m_PreExpediteTime = 0;
//		}
//	}
}

void TVBasicShip::correctPysics()
{
	auto shipIconPos = m_pShipIcon->getPosition();
	auto pb = m_pShipIcon->getPhysicsBody();
	m_pShipIcon->setPosition(Vec2(0,0));
	this->setPosition(_position + shipIconPos);
}

void TVBasicShip::correctWave()
{
	m_pShipWave->setPosition(_position);
	m_pShipWave->setStartSpin(-m_currentDirect + 90.0);
	m_pShipWave->setEndSpin(-m_currentDirect + 90.0);
}

void TVBasicShip::checkShipDied()
{
	if (m_fBlood <= 0.f)
	{
		//船沉了，参数reason：1 炮打沉，2水手死光，3，单挑，
		//isMyShip,如果是自己的船设置为1
		shipDied();
		if (m_Info->_isFriend)
		{
			WriterLog::getInstance()->addShipCrashBattleLogItem(m_Info->_sid,1,1);
		}
		else
		{
            WriterLog::getInstance()->addShipCrashBattleLogItem(m_Info->_sid,0,1);
		}	
	}
	else if (m_Info->_sailornum < 1)
	{
		shipDied();
		if (m_Info->_isFriend)
		{
			WriterLog::getInstance()->addShipCrashBattleLogItem(m_Info->_sid, 1, 2);
		}
		else
		{
			WriterLog::getInstance()->addShipCrashBattleLogItem(m_Info->_sid, 0, 2);
		}
	}
}

void TVBasicShip::repairShip()
{
	for (size_t i = 0; i < m_pDelegate->getSkillManage()->m_vAllSkillInfo.size(); i++)
	{
		auto &skillInfo = m_pDelegate->getSkillManage()->m_vAllSkillInfo[i];
		auto &skill = SINGLE_SHOP->getSkillTrees().find(skillInfo.id)->second;
		switch (skillInfo.id)
		{
		case SKILL_EMERGENT_REPAIR:
			if (m_Info->_isFriend && skillInfo.used[m_Info->_tag-1])
			{
				skillInfo.used[m_Info->_tag-1] = false;
				auto addNum = m_fMaxHp*skill.effect_per_lv*skillInfo.lv / SKILL_DER_MAX;
				setBloodIconValue(addNum + m_fBlood);
				repairShipEffice(ceil(addNum));
			}
			break;
		case SKILL_PROGRESSIVE_REPAIR:
			if (m_Info->_isFriend && skillInfo.used[m_Info->_tag-1])
			{
				skillInfo.used[m_Info->_tag-1] = false;
				m_nProgressiveRepairTime = skill.base_duration;
				skill.cur_skill_level = skillInfo.lv;
				updataProgressiveRepair();
			}
			break;
		case SKILL_FLEET_REPAIR:
			if (m_Info->_isFriend &&  skillInfo.used[m_Info->_tag-1])
			{
				skillInfo.used[m_Info->_tag-1] = false;
				auto addNum = m_fMaxHp*skill.effect_per_lv*skillInfo.lv / SKILL_DER_MAX;
				setBloodIconValue(addNum + m_fBlood);
				repairShipEffice(ceil(addNum));
			}
			break;
		default:
			break;
		}
	}
}

void TVBasicShip::shipDied()
{
	//TODO
	m_battleManager->getBattleUILayer()->flushUserDefault(this);
}

void TVBasicShip::moveToTaraget(Vec2 targetPos)
{
	if (m_bIsDied)
	{
		return;
	}
}

void TVBasicShip::setDirect(float angle)
{
	m_currentDirect = angle;
	normalDriect();
	m_pShipIcon->setRotation(m_currentDirect);
}

void TVBasicShip::normalDriect()
{
	if (std::abs(m_currentDirect) >= 360.f)
	{
		float tempAngle = m_currentDirect - int(m_currentDirect);
		m_currentDirect = int(m_currentDirect) % 360 + tempAngle;
	}
	if (m_currentDirect < - 180.0)
	{
		m_currentDirect = 360 - abs(m_currentDirect);
	}
	if (m_currentDirect > 180.0)
	{
		m_currentDirect = m_currentDirect - 360.f;
	}
}

bool TVBasicShip::onTouchBegan(Touch *touch, Event *unused_event)
{
	Camera  *c  = Director::getInstance()->getRunningScene()->getCameras().front();
	auto diff = c->getPosition() - m_cameraStartPostion;
	auto pos = convertToNodeSpace(touch->getLocation()+diff);
	Rect bb;
	bb.origin = Vec2(-_contentSize.width/2,-_contentSize.height/2);
	bb.size = _contentSize;
	if(bb.containsPoint(pos))
	{
		return true;
	}
	return false;
}

void TVBasicShip::onTouchEnded(Touch *touch, Event *unused_event)
{

}

bool TVBasicShip::hitTest(const Vec2 &pt)
{
	Vec2 nsp = m_pShipIcon->convertToNodeSpaceAR(pt);
	auto oldP = nsp;
	auto oldP1 = nsp;
	auto oldP2 = nsp;
	//nsp.rotate(this->getAnchorPoint(),CC_DEGREES_TO_RADIANS(-this->getDirect()));
	//换算到船本身的坐标系
	oldP2.x=oldP.x*cos(CC_DEGREES_TO_RADIANS(this->getDirect())) - oldP.y*sin(CC_DEGREES_TO_RADIANS(this->getDirect()));
	oldP2.y=oldP.x*sin(CC_DEGREES_TO_RADIANS(this->getDirect())) + oldP.y*cos(CC_DEGREES_TO_RADIANS(this->getDirect()));
	nsp = oldP2/2.0f;

	Rect bb;
	bb.size = m_pyBodySize;
	bb.origin = Vec2(-bb.size.width/2,-bb.size.height/2);

	if (bb.containsPoint(nsp))
	{	
		return true;
	}

	return false;
}

Point (&TVBasicShip::getHitTestPoints())[4]
{
	auto consize = m_pShipIcon->getContentSize();
	auto ak = m_pShipIcon->getAnchorPoint();

	auto scaleContent = consize/1.5;
	consize = scaleContent;
	Vec2 v1 = m_pShipIcon->convertToWorldSpaceAR(Vec2(-consize.width*ak.x,consize.height*(1-ak.y)));
	Vec2 v2 = m_pShipIcon->convertToWorldSpaceAR(Vec2(consize.width*(1.0-ak.x),consize.height*(1-ak.y)));
	Vec2 v3 = m_pShipIcon->convertToWorldSpaceAR(Vec2(consize.width*(1.0-ak.x),-consize.height*ak.y));
	Vec2 v4 = m_pShipIcon->convertToWorldSpaceAR(Vec2(-consize.width*ak.x,-consize.height*ak.y));

	mHitTestPoints[0] = v1;
	mHitTestPoints[1] = v2;
	mHitTestPoints[2] = v3;
	mHitTestPoints[3] = v4;

	return mHitTestPoints;
};

Point (&TVBasicShip::getHitTestPointsNineScreen())[4]
{
	auto points = getHitTestPoints();
	for (int i = 0;i < 4;i++)
	{
		mHitTestPointsNineScreen[i].x = points[i].x - _parent->getPosition().x;
		mHitTestPointsNineScreen[i].y = points[i].y - _parent->getPosition().y;
	}
	return mHitTestPointsNineScreen;
}

void TVBasicShip::hurt(Node* bullt)
{
	auto bul = dynamic_cast<Bullet*>(bullt);
	auto bulletInfo = bul->getBulletInfo();
	
	if (bulletInfo)
	{
		//sound effect ship is hit,maybe random 
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CANNON_HIT_29);
	
		Vec2 bullPos = bul->getPosition();
		Vec2 bullWorldPos = _parent->convertToWorldSpace(bullPos);
		Vec2 bullLocalPos = m_pShipIcon->convertToNodeSpaceAR(bullWorldPos);
		float distance = bullLocalPos.x - m_pShipIcon->getPosition().x;
		int attack = 0;
		int c = 1;
		if (std::abs(distance) < m_pShipIcon->getContentSize().width * m_pShipIcon->getScale()/3)
		{
			attack = bul->getAttack() * (1.f + bulletInfo->_activeCritSize / 1000.f + bulletInfo->_change_attack/1000.0 + bulletInfo->_extAttack/1000.0);
			c = 1;
		}else if(distance > m_pShipIcon->getContentSize().width * m_pShipIcon->getScale()/6)
		{
			//计算所有的技能和buf以后再叠加爆击伤害
			attack = bul->getAttack() * (1.f + bulletInfo->_activeCritSize / 1000.f + m_Info->_critSize / 1000.f + bulletInfo->_change_attack/1000.0 + bulletInfo->_extAttack/1000.0) * 2.0f ;
			c = 2;
		}else
		{
			//计算所有的技能和buf以后再叠加爆击伤害
			attack = bul->getAttack() * (1.f + bulletInfo->_activeCritSize / 1000.f + m_Info->_critSize / 1000.f + bulletInfo->_change_attack/1000.0 + bulletInfo->_extAttack/1000.0) * 2.0f;
			c = 2;
		}
		//log("critical:%d, bullet attack %d, attack %d, defense:%d, change_attack:%d, ext_attack:%d, activeCritSize:%d", c, bul->getAttack(), attack, m_Info->_defense + m_Info->_change_defense, bulletInfo->_change_attack, bulletInfo->_extAttack, bulletInfo->_activeCritSize);
		int defense = m_Info->_defense + m_Info->_change_defense;
		attack = attack * 3 /(1+ defense * 0.02f) ;  // 减防御
		if (attack < 0)
		{
			attack = 0;
		}
		else
		{
			//子弹打到船上，船碎片效果
			auto spriteSpalsh = Sprite::create();
			spriteSpalsh->setScale(2.0f);
			spriteSpalsh->setLocalZOrder(1000);
			spriteSpalsh->setPosition(m_pShipIcon->getContentSize() / 2);

			m_pShipIcon->addChild(spriteSpalsh);
			auto firAmimation = m_pDelegate->getEffective()->getAnimation(PT_SHIP_ATTACKED);
			spriteSpalsh->runAction(Sequence::create(Animate::create(firAmimation), RemoveSelf::create(), nullptr));
		}
		//连续命中时延长减速时间
		if (m_isSlowSpeed)
		{
			m_slowSpeedTime += bulletInfo->_isSlowSpeedTime;
		}
		//skill_id 3,4  add buffer
		if (!m_isSlowSpeed && bulletInfo->_isSlowSpeedTime > 0)
		{
			m_isSlowSpeed = true;
			m_slowSpeedTime = bulletInfo->_isSlowSpeedTime;
			m_fTempSpeed -= m_fMoveSpeed * 0.4;
			addBuffer(STATE_SLOWSPEED);//链球炮弹
		}			
		if (!m_isOnFired && bulletInfo->_isFireEfficeharm > 0)
		{
			m_firdHarm = bulletInfo->_isFireEfficeharm;
			m_isOnFired = true;
			m_onFiredTime = 20;
			addBuffer(STATE_FIRE);//燃烧弹
		}
		//士气影响
		if (bulletInfo->_isFriend && !m_Info->_isFriend)
		{
			m_pDelegate->getShareObj()->addSelfMorale(attack);
			m_pDelegate->getShareObj()->addEnemyMorale(-attack);
			if (bulletInfo->_tag == 1) //falg ship  skillid : 12
			{
				int prob = cocos2d::random(1,100);
				if (prob <= m_Roar_Victory_prob)
				{
					m_pDelegate->getShareObj()->addSelfMorale(0);
				}
			}
		}
		if(!bulletInfo->_isFriend && m_Info->_isFriend)
		{
			m_pDelegate->getShareObj()->addEnemyMorale(attack);
			if (m_Info->_tag == 1) //falg ship skillid : 11
			{
				int prob = cocos2d::random(1,100);
				if (prob > m_Indomitable_Will_prob)
				{
					m_pDelegate->getShareObj()->addSelfMorale(-attack);
				}
			}else
			{
				m_pDelegate->getShareObj()->addSelfMorale(-attack);
			}
		}
		//isSelfVictim 如果是敌人对己方造成的伤害，设置为true
		//火炮造成的伤害，参数依次为：己方shipid ，敌人shipid，伤害值，造成伤害的火炮iid，己方ship位置，敌人ship位置
		//void addCannonBattleLogItem(int myShipId, int enemyShipId, int hurt, int cannonItemid, Vec2 myShipPos, Vec2 enemyShipPos, bool isSelfVictim);

		if (bulletInfo->_isFriend)
		{
			WriterLog::getInstance()->addCannonBattleLogItem(bulletInfo->_sid, m_Info->_sid, attack, bul->getCannonid(), m_pPosition, m_pHitPosition, false);
		}
		else
		{
			WriterLog::getInstance()->addCannonBattleLogItem(m_Info->_sid, bulletInfo->_sid, attack, bul->getCannonid(), m_pHitPosition, m_pPosition, true);
		}
		//更新UI相应的船只血量
		m_fBlood -= attack;
		if (m_fBlood <= 0)
		{
			m_fBlood = 0;
			FIGHTING_CENTER_TIP_DATA tips = { bulletInfo->_sid, bulletInfo->_tag, m_Info->_sid, m_Info->_tag, 0, 0, 0, 0, 0, TIP_SHIP_SUNK };
			this->m_battleManager->getBattleUILayer()->pushFightingCentralTips(tips);
		}
		
		m_pBloodIcon->runAction(ProgressTo::create(0.2f, m_fBlood * 100 / m_fMaxHp));
		m_pDelegate->specialButtonEvent(this,"event_attacked",m_fBlood/m_fMaxHp);

		//减少血量(根据资源情况用'/'代表'-')
		int height = 60;
		int randomCount = cocos2d::random(-15,15);
		const char* formatString = attack > 0 ? "/%d":"%d";
		std::string s_capNums = StringUtils::format(formatString,attack);
		TextAtlas *capNums;
		if (c == 1)
		{
			capNums = TextAtlas::create(s_capNums, IMAGE_FONT[c], 20, 26, "/");
		}
		else
		{
			capNums = TextAtlas::create(s_capNums, IMAGE_FONT[c], 30, 32, "/");
		}
		capNums->setPositionX(randomCount);
		this->addChild(capNums, 50);
		auto seq_1 = Sequence::create(DelayTime::create(1.8f),FadeOut::create(0.3f),nullptr);
		auto easeAct = EaseQuinticActionOut::create(Spawn::createWithTwoActions(MoveBy::create(1.5f,Vec2(0,height)),seq_1));
		auto seq = Sequence::createWithTwoActions(easeAct,RemoveSelf::create(true));
		capNums->runAction(seq);

		m_battleManager->getBattleUILayer()->flushUserDefault(this);
	}
}

void TVBasicShip::hurt(int attack, int myId)
{
	if (attack < 0)
	{
		attack = 0;
	}
	m_fBlood -= attack;
	m_pBloodIcon->runAction(ProgressTo::create(0.2f, m_fBlood * 100 / m_fMaxHp));
	m_pDelegate->specialButtonEvent(this, "event_attacked", m_fBlood / m_fMaxHp);
	int height = 60;
	const char* formatString = attack > 0 ? "/%d" : "%d";
	std::string s_capNums = StringUtils::format(formatString, attack);
	TextAtlas *capNums;
	int randomCount = cocos2d::random(-15, 15);
	capNums = TextAtlas::create(s_capNums, IMAGE_FONT[1], 20, 26, "/");
	capNums->setPositionX(randomCount);
	this->addChild(capNums, 50);

	auto seq_1 = Sequence::create(DelayTime::create(1.8f), FadeOut::create(0.3f), nullptr);
	auto easeAct = EaseQuinticActionOut::create(Spawn::createWithTwoActions(MoveBy::create(1.5f, Vec2(0, height)), seq_1));
	auto seq = Sequence::createWithTwoActions(easeAct, RemoveSelf::create(true));
	capNums->runAction(seq);
	m_battleManager->getBattleUILayer()->flushUserDefault(this);
	//放技能造成的伤害，int skillId 技能id,int skillOwnerId 放技能的captainid或者玩家，victimShipIds 受伤的ship id 数组
	int a[5];
	int* enemys = a;
	for (int i = 0; i < m_pDelegate->getShareObj()->m_vEnemys.size(); i++)
	{
		enemys[i] = m_pDelegate->getShareObj()->m_vEnemys[i]->m_Info->_sid;
	}
	WriterLog::getInstance()->addSkillBattleLogItem(2, myId, attack, enemys, m_pDelegate->getShareObj()->m_vEnemys.size(),false);
}

void TVBasicShip::generateSkills(SkillDefine** skills,int n_skills)
{
	int n_skillType;
	for (int i = 0; i < n_skills;i++)
	{
		n_skillType = skills[i]->skilltype;
		int id = skills[i]->id;
		int level = skills[i]->level;
		if (n_skillType == SKILL_TYPE_PLAYER)
		{
			auto skillTrees = SINGLE_SHOP->getSkillTrees();
			switch(id)
			{
			case SKILL_FAST_RELOAD://Fast Reload
				m_fCDTime -= m_fCDTimeBasis * skillTrees[id].effect_per_lv*1.0*level / 1000;
				break;
			case SKILL_WEAKNESS_ATTACK://Weakness Attack
				m_Info->_critSize = skillTrees[id].rate_per_lv*level;
				break;
			case SKILL_INDOMITABLE_WILL://Indomitable Will
				m_Indomitable_Will_prob = skillTrees[id].effect_per_lv*level;
				break;
			case SKILL_ROAR_OF_VICTORY://Roar of Victory
				m_Roar_Victory_prob = skillTrees[id].rate_per_lv*level;
				break;
			case SKILL_ARMOUR_OPTIMIZATION://Armour Optimization
				m_Info->_change_defense += static_cast<int>(m_Info->_defense*1.0*(1.0 + skillTrees[id].effect_per_lv*level*1.0 / 1000));
				break;
			case SKILL_REFORM_OF_HULL://Reform of Hull
				//技能已改
				//m_fBlood = m_fBlood*(1.0+skillTrees[id].effect_per_lv*1.0*level/1000);
				//m_fMaxHp = m_fMaxHp*(1.0+skillTrees[id].effect_per_lv*1.0*level/1000);
				break;
			default:
				break;
			}
		}
		else if (n_skillType == SKILL_TYPE_CAPTAIN)
		{
			auto skillTrees = SINGLE_SHOP->getCaptainSkillInfo();
			switch(id)
			{
			case SKILL_CAPTAIN_ARMOR_DEFENSE://Armor Defense
				m_Info->_change_defense += static_cast<int>(m_Info->_defense*1.0*(1.0 + skillTrees[id].effect_per_lv*1.0*level / 1000));
				break;
			case SKILL_CAPTAIN_UNBEATABLE_WILL:
				m_Indomitable_Will_prob = skillTrees[id].rate_per_lv*level;
				break;
			case SKILL_CAPTAIN_ROAR_OF_VICTORY:
				m_Roar_Victory_prob = skillTrees[id].rate_per_lv*level;
				break;
			default:
				break;
			}
		}
		else if (n_skillType == SKILL_TYPE_NPC)
		{
			auto skillTrees = SINGLE_SHOP->getNPCSkillInfo();
			switch(id)
			{		
			case 1005://Armor Defense
				m_Info->_change_defense += static_cast<int>(m_Info->_defense*1.0*(1.0 + skillTrees[id].effect_per_lv*1.0*level / 1000));
				break;
			case 1006:
				m_Indomitable_Will_prob = skillTrees[id].rate_per_lv*level;
				break;
			case 1007:
				m_Roar_Victory_prob = skillTrees[id].rate_per_lv*level;
				break;
			default:
				break;
			}
		}
		else if (n_skillType == SKILL_TYPE_COMPANION_NORMAL)
		{
			auto skillTrees = SINGLE_SHOP->getCompanionNormalSkillInfo();
			switch (id)
			{
			case SKILL_COMPANION_NORMAL_ARMOR_DEFENSE://Armor Defense
				m_Info->_change_defense += static_cast<int>(m_Info->_defense*1.0*(1.0 + skillTrees[id].effect_per_lv*1.0*level / 1000));
				break;
			case SKILL_COMPANION_NORMAL_UNBEATABLE_WILL:
				m_Indomitable_Will_prob = skillTrees[id].rate_per_lv*level;
				break;
			case SKILL_COMPANION_NORMAL_ROAR_OF_VICTORY:
				m_Roar_Victory_prob = skillTrees[id].rate_per_lv*level;
				break;
			default:
				break;
			}
		}
		else if (n_skillType == SKILL_TYPE_PARTNER_SPECIAL)
		{
			//数据都是固定的不需要查表
			switch (id)
			{
			case 1:
				m_fCDTime -= m_fCDTimeBasis * 5 / 100;
				break;
			default:
				break;
			}
		}
	}	
}

int TVBasicShip::getRangeAttackFactor()
{
	int MORALE_HALF = 2500;
	int morale = 0;
	if (m_Info->_isFriend)
	{
		morale = m_pDelegate->getShareObj()->m_nMorale; //士气
	}else
	{
		morale = m_pDelegate->getShareObj()->m_nMorale_Enemy; //士气
	}
	int morale_attack = (100 * (morale - MORALE_HALF))/MORALE_HALF;
	
	//m_rangeAttack should /10
	//船首相和士气影响
	//log("attack m_RangedAttack_per %d morale %d", m_RangedAttack_per, morale);
	int extAttackFactor =  m_RangedAttack_per +  morale_attack;
	return extAttackFactor;
}

void TVBasicShip::setShortAttackAddition(int add_per)
{
	m_ShortdAttack_per = add_per;
}

float TVBasicShip::getShortAttackAddition()
{
	//小伙伴的被动技能的影响
	auto temp = 0;
	for (int i = 0; i < m_PassiveSkillsNum; i++)
	{
		int type = mPassiveSkills[i]->skilltype;
		int id = mPassiveSkills[i]->id;

		if (type == SKILL_TYPE_CAPTAIN && id == SKILL_CAPTAIN_RAM_EXPERT)
		{
			auto skill = SINGLE_SHOP->getCaptainSkillInfo().find(id)->second;
			temp = skill.effect_per_lv * mPassiveSkills[i]->level;
			break;
		}
		else if (type == SKILL_TYPE_COMPANION_NORMAL && id == SKILL_COMPANION_NORMAL_RAM_EXPERT)
		{
			auto skill = SINGLE_SHOP->getCompanionNormalSkillInfo().find(id)->second;
			temp = skill.effect_per_lv * mPassiveSkills[i]->level;
			break;
		}
		else if (type == SKILL_TYPE_NPC && id == 1002)
		{
			auto skill = SINGLE_SHOP->getNPCSkillInfo().find(id)->second;
			temp = skill.effect_per_lv * mPassiveSkills[i]->level;
			break;
		}
	}

	return (m_ShortdAttack_per + temp) / 1000.f;
}

float TVBasicShip::getShortDefenseAddition()
{
	return m_ShortDefense_per/1000.f;
}

int TVBasicShip::getMoralAddition(int curMoral)
{
	return int(curMoral * (1.f + m_moral_per/1000.f));
}

void TVBasicShip::operatorSkill(int index)
{
	
}

void TVBasicShip::setBloodIconValue(float blood)
{
	m_fBlood = blood;
	if (m_fBlood > m_fMaxHp)
	{
		m_fBlood = m_fMaxHp;
	}

	if (m_fBlood <= 0)
	{
		m_fBlood = 0;
	}

	m_pBloodIcon->setPercentage(100 * m_fBlood / m_fMaxHp);
	m_pDelegate->getBattleUI()->setBloodValue(m_fBlood, m_Info->_tag, m_Info->_isFriend);
}

void TVBasicShip::deleteBuffer(int type)
{
	if (m_BufferList)
	{
		auto& buffers = m_BufferList->getItems();
		int nums =  buffers.size() - 1;
		if (nums < 0)
		{
			return;
		}

		for (auto &item : buffers)
		{
			if(item->getTag() == type)
			{
				int index = m_BufferList->getIndex(item);
				auto contentSize = item->getContentSize();
				m_BufferList->removeItem(index);	
				m_BufferList->setContentSize(Size(nums * contentSize.width,contentSize.height));
				addLogAction(3,0,type); // writer log
				break;
			}
		}

		for (auto buffer1 = m_vBuffer.begin();buffer1 != m_vBuffer.end(); buffer1++)
		{
			if ((*buffer1) == type)
			{
				m_vBuffer.erase(buffer1);
				break;
			}
		}

		switch (type)
		{
		case STATE_SLOWSPEED:
			{
				m_slowSpeedTime = 0;
				if (spriteEffectSpalsh)
				{
					spriteEffectSpalsh->removeFromParentAndCleanup(true);
					spriteEffectSpalsh = nullptr;
				}
				break;
			}
		case STATE_FIRE:
			{
				m_onFiredTime = 0;
				if (spriteEffectSmoke)
				{
					spriteEffectSmoke->removeFromParentAndCleanup(true);
					spriteEffectSmoke = nullptr;
				}
				break;
			}
		default:
			break;
		}
	}
}

void TVBasicShip::deleteBuffer(SkillData& skilldata)
{
	int type = skilldata.skill_type;
	deleteBuffer(type);
	for (auto buffer = m_vBuffer.begin();buffer != m_vBuffer.end(); buffer++)
	{
		if ((*buffer) == type)
		{
			m_vBuffer.erase(buffer);
			/*
			if (type == STATE_SLOWSPEED)
			{
				m_fMoveSpeed = m_fMoveSpeed / (1 + skilldata.skill_effect/100.f);
			}
			*/
			break;
		}
	}
}

void TVBasicShip::addBuffer(int type)
{
	addLogAction(2,0,type); // writer log
	m_vBuffer.push_back(type);

	switch (type)
	{
	case STATE_SLOWSPEED:
		{
			slowSpeedState_Buffer();
			break;
		}
	case STATE_FIRE:
		{
			fireState_Buffer();
			break;
		}
	default:
		break;
	}
	
	if (m_BufferList)
	{
		int nums = m_BufferList->getItems().size() + 1;
		std::string image_file = StringUtils::format(BUFFER_4.c_str(),int(type));
		ImageView* buffer = ImageView::create(image_file);
		buffer->setVisible(false);
		auto contentSize = buffer->getContentSize();
		m_BufferList->setContentSize(Size(nums * contentSize.width,contentSize.height));
		buffer->setTag(type);
		m_BufferList->pushBackCustomItem(buffer);
	}
}

void TVBasicShip::addBuffer(SkillData& skilldata)
{
	auto type = skilldata.skill_type;
	addLogAction(2,0,type); // writer log
	m_vBuffer.push_back(skilldata.skill_type);
	if (type == STATE_SLOWSPEED)
	{
		m_fMoveSpeed = m_fMoveSpeed * (1 - skilldata.skill_effect/100.f);
	}
	if (type == STATE_DISORDER)
	{
		m_BufferTarget = nullptr;
	}
	
	if (m_BufferList)
	{
		int nums = m_BufferList->getItems().size() + 1;
		std::string image_file = StringUtils::format(BUFFER_4.c_str(),int(skilldata.skill_type));
		ImageView* buffer = ImageView::create(image_file);
		auto contentSize = buffer->getContentSize();
		m_BufferList->setContentSize(Size(nums * contentSize.width,contentSize.height));
		buffer->setTag(type);
		m_BufferList->pushBackCustomItem(buffer);
	}
}

int TVBasicShip::getBufferState()
{
	for (auto iter : m_vBuffer)
	{
		//return (iter < 3 && iter > 0) ? iter: -1;
		return (iter == STATE_FEAR) ? iter : -1;
	}
	return -1;
}

void TVBasicShip::bufferStateDisponse(int bufferState)
{
	switch (bufferState)
	{
	case STATE_DISORDER:
		{
			disorderState_Buffer();
			break;
		}
	case STATE_FEAR:
		{
			fearState_Buffer();
			break;
		}
	case STATE_SLOWSPEED:
		{
			//slowSpeedState_Buffer();
			break;
		}
	case STATE_FREE:
		{
			//fireState_Buffer();
			break;
		}
	default:
		break;
	}
}

void TVBasicShip::fearState_Buffer()
{
	auto skillManage = m_pDelegate->getSkillManage();
	std::string ky = StringUtils::format("%d_%d",this->getTag(),STATE_FEAR);
	auto skilldata = skillManage->getUsedBufferInfo(ky);
	if (skilldata == nullptr)
	{
		for (auto iter = m_vBuffer.begin(); iter != m_vBuffer.end();iter++)
		{
			if ((*iter) == STATE_FEAR)
			{
				deleteBuffer(*iter);
				break;
			}
		}
		//log("skill not find ..%s.",ky.c_str());
		return;
	}
	auto shareObj = m_pDelegate->getShareObj();
	int targetId = skilldata->skill_own_id ;
	Vec2 shipPos = _position + Vec2(200,200);
	if (targetId > 5)
	{
		for (auto iter = shareObj->m_vEnemys.begin();iter != shareObj->m_vEnemys.end();iter++)
		{
			if ((*iter)->getTag() == targetId)
			{
				shipPos = (*iter)->getPosition();
			}
		}
	}else
	{
		for (auto iter = shareObj->m_vMyFriends.begin();iter != shareObj->m_vMyFriends.end();iter++)
		{
			if ((*iter)->getTag() == targetId)
			{
				shipPos = (*iter)->getPosition();
			}
		}
	}
	auto diffPos = shipPos- _position;
	auto targetPos = _position - diffPos;
	moveAndRotation(targetPos);
}

void TVBasicShip::disorderState_Buffer()
{
	m_BufferTarget = nullptr;
	if (m_BufferTarget == nullptr || m_BufferTarget->isDied())
	{
		m_BufferTarget = nullptr;
		const auto &enemys = m_pDelegate->getShareObj()->m_vEnemys;
		const auto &friends = m_pDelegate->getShareObj()->m_vMyFriends;

		std::vector<TVBasicShip*> avaliableAttackShips;
		for(auto item = enemys.begin();item != enemys.end(); item++){

			if ((*item) == this)
			{
				continue;
			}
			float tempDis = (*item)->getPosition().distance(_position);
			
			if(tempDis <= m_CurLaunchDistance){
				
				avaliableAttackShips.push_back((*item));
			}
		}
		for(auto item = friends.begin();item != friends.end(); item++){

			if ((*item) == this)
			{
				continue;
			}
			float tempDis = (*item)->getPosition().distance(_position);
			if(tempDis <= m_LaunchDistance_max){	
				avaliableAttackShips.push_back((*item));
			}
		}

		if (avaliableAttackShips.size() > 0)
		{
			int maxSize = avaliableAttackShips.size() - 1;
			int randomValue = cocos2d::random(0,maxSize);
			
			m_BufferTarget = avaliableAttackShips[randomValue];
			if (SHIP_READY == m_ShipStatus)
			{
				auto diffPos = m_BufferTarget->getPosition() - _position;
				float distance = diffPos.getLength();
				float fireRange = CC_RADIANS_TO_DEGREES(Vec2::angle(diffPos,Vec2(cos(CC_DEGREES_TO_RADIANS(m_currentDirect)),sin(CC_DEGREES_TO_RADIANS(m_currentDirect)))));
				if ( fireRange > 35.f && fireRange < 145.f )
				{
					firedByDistance(avaliableAttackShips[randomValue],distance);
					return;
				}
			}
		
		}
	}
}

void TVBasicShip::slowSpeedState_Buffer()
{
	//add slow speed effect
	if (!spriteEffectSpalsh)
	{
		spriteEffectSpalsh = Sprite::create();
//		spriteEffectSpalsh->setScale(0.5f);
//		spriteEffectSpalsh->setAnchorPoint(Vec2(0.5, 0.3));
//		spriteEffectSpalsh->setLocalZOrder(1000);
		spriteEffectSpalsh->setPosition(m_pShipIcon->getContentSize() / 2);
		spriteEffectSpalsh->setLocalZOrder(-1);
		spriteEffectSpalsh->setScale(this->getScale());
		m_pShipIcon->addChild(spriteEffectSpalsh);
	}

	auto firAmimation = m_pDelegate->getEffective()->getAnimation(PT_SPEED_DOWN);
	spriteEffectSpalsh->runAction(RepeatForever::create(Animate::create(firAmimation)));

}

void TVBasicShip::fireState_Buffer()
{
	//add fire effect
	if (!spriteEffectSmoke)
	{
		spriteEffectSmoke = Sprite::create();
		spriteEffectSmoke->setScale(0.5f);
		spriteEffectSmoke->setLocalZOrder(1000);
		this->addChild(spriteEffectSmoke);
	}
	auto firAmimation = m_pDelegate->getEffective()->getAnimation(PT_SMOKE_EFFECT);
	spriteEffectSmoke->runAction(RepeatForever::create(Animate::create(firAmimation)));
}

void TVBasicShip::updataProgressiveRepair()
{
	auto skill = SINGLE_SHOP->getSkillTrees().find(SKILL_PROGRESSIVE_REPAIR)->second;
	auto addSubNum = m_fMaxHp * skill.effect_per_lv * skill.cur_skill_level / SKILL_DER_MAX;
	setBloodIconValue(addSubNum / skill.base_duration + m_fBlood);
	repairShipEffice(ceil(addSubNum / skill.base_duration));
	m_nProgressiveRepairTime--;
	if (m_nProgressiveRepairTime > 0)
	{
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(1),
			CallFunc::create(CC_CALLBACK_0(TVBasicShip::updataProgressiveRepair, this))));
	}
}

void TVBasicShip::repairShipEffice(int num)
{
	//(根据资源情况用'/'代表'+')
	std::string st_hp = StringUtils::format("/%d", num);
	auto add_hp = TextAtlas::create(st_hp, IMAGE_FONT[3], 30, 32, "/");
	auto easeAct = EaseQuinticActionOut::create(MoveBy::create(0.8f, Vec2(0, 40)));
	add_hp->setOpacity(0);
	auto seq = Sequence::create(FadeIn::create(0.2f), easeAct, RemoveSelf::create(), nullptr);
	add_hp->runAction(seq);
	this->addChild(add_hp, 100);
}

void TVBasicShip::fireeffective(int bulletNum)
{ 
	int n_effectCount = 0;
	if (m_fireShipSide < 2)
	{
		n_effectCount = bulletNum;//(bulletNum < 4)?bulletNum : 4 ;
	}
	else
	{
		n_effectCount = 8;
	}

	for (int i = 0; i < n_effectCount; i++)
	{
		auto sp = Sprite::create();
		auto sp_1 = Sprite::create();
		auto sp_3 = Sprite::create();
		sp_3->setContentSize(Size(350, 350));
		sp->setContentSize(sp_3->getContentSize());
		sp->setScale(0.40);
		sp->setAnchorPoint(Vec2(0, 0.5));
		float offsetX = 0;
		float widthNum = m_pShipIcon->getContentSize().width;
		float b = 3.0 * widthNum / 9;
		float c = 1.0 / (n_effectCount + 1);
		offsetX = b * c;
		if (m_fireShipSide == 0)
		{
			sp->setPosition(Size(3 * ((m_pShipIcon->getContentSize().width) / 9) + offsetX * (i+1), m_pShipIcon->getContentSize().height / 2) - Size(0, 20));
			sp->setRotation(-90);
			sp_1->setRotation(-90);
			sp_1->setPositionY(sp_1->getPositionY() - sp->getContentSize().width / 6);
		}
		else if (m_fireShipSide == 1)
		{
			sp->setPosition(Size(3 * ((m_pShipIcon->getContentSize().width) / 9) + offsetX* (i + 1), m_pShipIcon->getContentSize().height / 2) + Size(0, 20));
			sp->setRotation(90);
			sp_1->setRotation(90);
			sp_1->setPositionY(sp_1->getPositionY() + sp->getContentSize().width / 2);
		}
		else
		{
			//两侧同时开炮
			if (m_fireShipSide == 2 && i < 4)
			{
				sp->setPosition(Size((i + 3) * ((m_pShipIcon->getContentSize().width) / (8 + 1)), m_pShipIcon->getContentSize().height / 2) - Size(0, 20));
				sp->setRotation(-90);
				sp_1->setRotation(-90);
				sp_1->setPositionY(sp_1->getPositionY() - sp->getContentSize().width / 6);
			}
			else
			{
				sp->setPosition(Size((i - 4 + 3) * ((m_pShipIcon->getContentSize().width) / (8 + 1)), m_pShipIcon->getContentSize().height / 2) + Size(0, 20));
				sp->setRotation(90);
				sp_1->setRotation(90);
				sp_1->setPositionY(sp_1->getPositionY() + sp->getContentSize().width / 2);
			}
		}

		sp->setLocalZOrder(100);		
		sp_1->setPositionX(sp->getPositionX());

		float a = sp->getContentSize().width;	
		sp_1->setAnchorPoint(Vec2(0, 0.5));
		auto animate = Animate::create(m_pDelegate->getEffective()->getAnimation(PT_FIRE_EFFECT)->clone());
		auto animate_1 = Animate::create(m_pDelegate->getEffective()->getAnimation(PT_FIRE_SMOKE_EFFECT)->clone());
		sp->runAction(Sequence::create(animate, CallFunc::create([=]{sp->setVisible(false); sp_1->removeFromParentAndCleanup(true); }), nullptr));
		sp_1->runAction(Sequence::create(DelayTime::create(5 * (1.0f / 24)), animate_1, CallFunc::create([=]{sp_1->setVisible(false); sp_1->removeFromParentAndCleanup(true); }), nullptr));
		m_pShipIcon->addChild(sp);
		m_pShipIcon->addChild(sp_1);
	}
	m_fireShipSide = 0;
}

void  TVBasicShip::setShipAndBooldIconVisible()
{
	m_pShipIcon->setVisible(false);
	m_pCDTimeBg->setVisible(false);
	m_pSailorLabel->setVisible(false);
	this->getChildByTag(113)->setVisible(false);
	this->getChildByTag(114)->setVisible(false);
	this->getChildByTag(115)->setVisible(false);
	m_pFlagIcon->setVisible(false);
	m_pBloodIcon->setVisible(false);
}
