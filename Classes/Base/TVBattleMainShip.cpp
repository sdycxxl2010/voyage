#include "TVBattleMainShip.h"
#include "TVBattleManager.h"
#include "Bullet.h"

#include "SystemVar.h"
#include "WriterLog.h"

#define OFFSET 50.0f
#define CHECK_RADIUS 5.0f

TVBattleMainShip::TVBattleMainShip()
{
	m_bIsMoveEnd = true;
	m_bIsRotationEnd = true;
	m_bZeroSpeedRotation = true;
	m_bIsCenterX = true;
	m_bIsCenterY = true;
	m_NextRotation = 0;
	m_fCurSpeed = 0;
	m_fMoveSpeed = 1.0;
	m_PreExpediteTime = 0;
	m_vRoad.clear();

	m_pAttackDistanceIcon = nullptr;
	m_pGreenCircle = nullptr;
	m_eType = MAIN_SHIP;
	m_bIsMainShipSkill = false;
	m_bIsMainShipHPHalf = false;
	slovId = 0;
}

TVBattleMainShip::~TVBattleMainShip()
{

}

TVBattleMainShip* TVBattleMainShip::createShip(TVBattleManager*manager, FightShipInfo* shipInfo, int tag, bool isFriend, int s, int fightType, int index)
{
	auto bs = new TVBattleMainShip();
	if (bs)
	{
		bs->m_nFightType = fightType;
		bs->m_Info->_tag = tag;
		bs->m_Info->_index = index;
		bs->m_Info->_type = s;
		bs->m_Info->_isFriend = isFriend;
		bs->initShipInfo(shipInfo,manager);
		bs->init();
		bs->autorelease();
		return bs;
	}
	CC_SAFE_DELETE(bs);
	return nullptr;
}

bool TVBattleMainShip::init()
{
	bool pRet = false;
	do
	{
		TVBasicShip::init();
		m_ExpediteSpeed =  m_fMoveSpeed * m_fMoveSpeed / (2 * 60.0) * 2;
		for (size_t i = 0; i < m_PassiveSkillsNum; i++)
		{
			if (mPassiveSkills[i]->skilltype == SKILL_TYPE_PLAYER && mPassiveSkills[i]->id == SKILL_REFORM_OF_HULL)
			{
				m_bIsMainShipSkill = true;
			}
		}
		pRet = true;
	} while (0);

	return pRet;
}

void TVBattleMainShip::onEnter()
{
	TVBasicShip::onEnter();
	m_pAttackDistanceIcon = Sprite::create();
	m_pAttackDistanceIcon->setAnchorPoint(Vec2(0,0));
	m_pAttackDistanceIcon->setPosition(Vec2(-1000,-1000));
	_parent->addChild(m_pAttackDistanceIcon,3);
	
	//转动动画
	m_pGreenCircle = Sprite::create();

	Vector<SpriteFrame *> frames;
	for (int i = 0; i < 32; i++)
	{
		std::string name = StringUtils::format("battle_selector__%04d.png", i);//32
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 0.05f, 1);
	auto myanimation = Animate::create(animation);
	m_pAttackDistanceIcon->addChild(m_pGreenCircle, 100, 1000);
	m_pGreenCircle->runAction(RepeatForever::create(Sequence::create(myanimation, nullptr)));

	auto isHaveRange = [&](int range)->bool
	{
		if(!m_pResult) return false;
		for (int i = 0; i < m_pResult->n_cannons;i++)
		{
			if(m_pResult->cannons[i]->range == range)
				return true;
		}
		return false;
	};
	if (isHaveRange(3))
	{
		auto attackIcon_max = Sprite::create();
		attackIcon_max->setTexture(ATTACK_RANGE_MAX);
		m_pAttackDistanceIcon->addChild(attackIcon_max,0);
	}
	if (isHaveRange(2))
	{
		auto attackIcon_mid = Sprite::create();
		attackIcon_mid->setTexture(ATTACK_RANGE_MID);
		m_pAttackDistanceIcon->addChild(attackIcon_mid,1);
	}
	if (isHaveRange(1))
	{
		auto attackIcon_min = Sprite::create();
		attackIcon_min->setTexture(ATTACK_RANGE_MIN);
		m_pAttackDistanceIcon->addChild(attackIcon_min,2);
	}

	m_shipStartPostion = this->getPosition();
}

void TVBattleMainShip::onExit()
{
	this->unscheduleUpdate();
	TVBasicShip::onExit();
}
  
void TVBattleMainShip::startBattle()
{
	TVBasicShip::startBattle();
	m_TargetPos = _position;
}


void TVBattleMainShip::deleteBuffer(SkillData& skilldata)
{
	TVBasicShip::deleteBuffer(skilldata);
	m_bIsMoveEnd = true;
	m_bIsRotationEnd = true;
}

void TVBattleMainShip::callbackEventByIndex(int tag)
{

}

void TVBattleMainShip::specialButtonEvent(Node* target,std::string name,float varValue)
{

}

void TVBattleMainShip::update(float delate)
{
	if (m_bIsDied)
	{
		//log("Main ship die-----");
		return;
	}

	TVBasicShip::update(delate);

	if (!m_PrepareFire_OK)
	{
		return;
	}
	if (m_BattleState)
	{
		//log("cocos : close battle fighting...");
		return;
	}
	
 	m_PreTime += int(delate * 1000);
	if ( m_PreTime > 500)
	{
		m_PreTime = 0;
		if (SHIP_FIRED == m_ShipStatus)
		{
			m_ShipStatus = SHIP_LOADING;
			m_pFlagIcon->runAction(ProgressTo::create(0.3f,0.f));
		}else if (m_pFlagIcon->getPercentage() > 99.9)
		{
			m_ShipStatus = SHIP_READY;
		}else if (m_ShipStatus == SHIP_LOADING)
		{
			m_pFlagIcon->setPercentage(m_pFlagIcon->getPercentage() + 100.0f / (2 * m_fCDTime));
			if (isFired)
			{
				isFired = false;
			}
			//log("Ship tag %d: %f",this->getTag(),m_pFlagIcon->getPercentage());
		}
	}
	
	changeStrategy();
	checkAttackDistanceIcon();

	//舰体改良技能的实现
	if (m_bIsMainShipSkill && !m_bIsMainShipHPHalf)
	{
		if (m_fBlood < m_fMaxHp / 2)
		{
			m_bIsMainShipHPHalf = true;
			for (size_t i = 0; i < m_PassiveSkillsNum; i++)
			{
				if (mPassiveSkills[i]->skilltype == SKILL_TYPE_PLAYER && mPassiveSkills[i]->id == SKILL_REFORM_OF_HULL)
				{
					auto skillTrees = SINGLE_SHOP->getSkillTrees();
					m_Info->_change_defense += m_Info->_defense * skillTrees[SKILL_REFORM_OF_HULL].effect_per_lv*1.0*mPassiveSkills[i]->level / 1000;
					m_bIsMainShipSkill = true;
					break;
				}
			}
		}
	}
	
}
/*
void BattleMainShip::checkExpediteSpeed(float delt)
{
	if (m_ExpediteSpeed_flag == 1)
	{
		m_PreExpediteTime += int(delt * 1000);
		if (m_PreExpediteTime > 180)
		{
			if (m_fCurSpeed < m_fMoveSpeed)
			{
				m_fCurSpeed += m_ExpediteSpeed * 1000 / m_PreExpediteTime;
			}else
			{
				m_fCurSpeed = m_fMoveSpeed;
				m_ExpediteSpeed_flag = 0;
			}
			m_PreExpediteTime = 0;
		}
	}
	if (m_ExpediteSpeed_flag == 2)
	{
		m_PreExpediteTime += int(delt * 1000);
		if (m_PreExpediteTime > 180)
		{
			if (m_fCurSpeed > 0)
			{
				m_fCurSpeed -= m_ExpediteSpeed * 1000 / m_PreExpediteTime;
			}else
			{
				m_fCurSpeed = 0;
				m_ExpediteSpeed_flag = 0;
			}
			m_PreExpediteTime = 0;
		}
	}
}
*/
void TVBattleMainShip::changeStrategy()
{
	int state = getBufferState();
	if (state > 0)
	{
		//log("cocos : butffer %d",state);
		bufferStateDisponse(state);
		return;
	}
	
	if (m_vRoad.size() > 0)
	{
		if(m_vRoad[0].angle != 0 && m_NextRotation == 0)//m_vRoad.size() > 1)
		{
			
			m_bIsRotationEnd = false;
			m_NextRotation = CC_RADIANS_TO_DEGREES(m_vRoad[0].angle);
		}
		if (m_vRoad[0].angle != 0 && m_vRoad[0].turnR != 0)
		{
			m_fRoateAngle = CC_RADIANS_TO_DEGREES(2 * asin(m_fLimitMoveSpeed/m_vRoad[0].turnR/2));
		}
	
		shipRotation(m_TargetPos);
		shipMove(m_TargetPos);
	}
	
	battleFree();
}

void TVBattleMainShip::moveAndRotation(Vec2 targetPos)
{
	//autoMoveAndRotation(m_TargetPos);
	moveToTaraget(targetPos);
}

void TVBattleMainShip::battleFree()
{
	float distance = 999999.0;
	if(2 != m_ShipStatus)
	{
		return;
	}

	std::vector<TVBasicShip*>* EnemyShips = getEnemyShips(nullptr);
	auto target_index = m_pDelegate->getShareObj()->m_nLockShipTag;
	if (target_index != -1 && EnemyShips->size() > 0)
	{
		auto iter = find_if(EnemyShips->begin(),EnemyShips->end(),[target_index](TVBasicShip* ship){ return target_index == ship->m_Info->_tag;});
		if (iter != EnemyShips->end())
		{
			if ((*iter)->isCloseBattle())
			{
				return;
			}
			auto targetPos = (*iter)->getPosition();
			auto diffPos = targetPos - _position;
			distance = (*iter)->getPosition().distance(_position);
			float fireRange = CC_RADIANS_TO_DEGREES(Vec2::angle(diffPos,Vec2(cos(CC_DEGREES_TO_RADIANS(m_currentDirect)),sin(CC_DEGREES_TO_RADIANS(m_currentDirect)))));
			
			if (distance < m_CurLaunchDistance && fireRange > 35.f && fireRange < 145.f )
			{
//				log("cocos : mainship fired %d");
				firedByDistance((*iter),distance);
			}	
		}
	}
}

void TVBattleMainShip::battleSalvoShoot(float num,int tag)
{
	slovId = tag;
	int cannonNum_min = 0;
	int cannonNum_mid = 0; 
	int cannonNum_max = 0;
	int cannonLaunch_min = 0;
	int cannonLaunch_mid = 0;
	int cannonLaunch_max = 0;
	
	//技能只有火炮学可以影响攻击(小伙伴或者雇佣兵)
	float skill_num = m_Info->_change_attack / 1000.0;
	float ext_num = getRangeAttackFactor() / 1000.0;
	for (int i = 0; i < m_pResult->n_cannons;i++)
	{
		if(m_pResult->cannons[i]->range == 1)
		{
			cannonNum_min++;
			cannonLaunch_min += m_pResult->cannons[i]->attack*(1 + num + skill_num + ext_num);
		}else if(m_pResult->cannons[i]->range == 2)
		{
			cannonNum_mid++;
			cannonLaunch_mid += m_pResult->cannons[i]->attack*(1 + num + skill_num + ext_num);
		}else if(m_pResult->cannons[i]->range == 3)	
		{
			cannonNum_max++;
			cannonLaunch_max += m_pResult->cannons[i]->attack*(1 + num + skill_num + ext_num);
		}
	}
	if (cannonNum_min)
	{
		cannonLaunch_min /= cannonNum_min;
	}
	if (cannonNum_mid)
	{
		cannonLaunch_mid /= cannonNum_mid;
	}
	if (cannonNum_max)
	{
		cannonLaunch_max /= cannonNum_max;
	}
	

	std::vector<int> shipTag_min;
	std::vector<int> shipTag_mid;
	std::vector<int> shipTag_max;
	shipTag_min.clear();
	shipTag_mid.clear();
	shipTag_max.clear();
	float distance = 999999.0;
	
	for (int i = 0; i < m_pDelegate->getShareObj()->m_vEnemys.size(); i++)
	{
		auto ship = m_pDelegate->getShareObj()->m_vEnemys[i];
		auto targetPos = ship->getPosition();
		auto diffPos = targetPos - _position;
		distance = ship->getPosition().distance(_position);
			
		if (distance < m_LaunchDistance_min)
		{
			if (cannonLaunch_min <= 0 && cannonLaunch_mid <=0 && cannonNum_max!= 0)
			{
				shipTag_max.push_back(ship->m_Info->_tag);
			}
			else if (cannonLaunch_min <= 0 && cannonLaunch_mid > 0)
			{
				shipTag_mid.push_back(ship->m_Info->_tag);
			}
			else if (cannonNum_min > 0)
			{
				shipTag_min.push_back(ship->m_Info->_tag);
			}
		}else if(distance < m_LaunchDistance_mid)
		{
			if (cannonNum_mid <=0 && cannonNum_max >0)
			{
				shipTag_max.push_back(ship->m_Info->_tag);
			}else if (cannonNum_mid > 0)
			{
				shipTag_mid.push_back(ship->m_Info->_tag);
			}	
		}else if(distance < m_LaunchDistance_max)
		{
			if (cannonNum_max > 0)
			{
				shipTag_max.push_back(ship->m_Info->_tag);
			}	
		}
	}
	
	if (cannonNum_mid !=0 && cannonNum_max != 0 && cannonNum_min !=0)
	{
		cannonLaunch_min += cannonLaunch_max + cannonLaunch_mid;
		cannonLaunch_mid += cannonLaunch_max; 
	}
	else if (cannonNum_mid != 0 && cannonNum_max != 0 && cannonNum_min == 0)
	{
		cannonLaunch_mid = cannonLaunch_max + cannonLaunch_mid;
		cannonLaunch_min = 0;
	}
	else if (cannonNum_mid != 0 && cannonNum_max == 0 && cannonNum_min == 0)
	{
		cannonLaunch_max = 0;
		cannonLaunch_min = 0;
	}
	else if (cannonNum_mid == 0 && cannonNum_max != 0 && cannonNum_min != 0)
	{
		cannonLaunch_min += cannonLaunch_max;
		cannonLaunch_mid = 0;
	}
	else if (cannonNum_mid != 0 && cannonNum_max == 0 && cannonNum_min != 0)
	{
		cannonLaunch_min += cannonLaunch_mid;
		cannonLaunch_max = 0;
	}
	else if (cannonNum_mid == 0 && cannonNum_max == 0 && cannonNum_min != 0)
	{
		cannonLaunch_max = 0;
		cannonLaunch_mid = 0;
	}
	else if (cannonNum_mid == 0 && cannonNum_max != 0 && cannonNum_min == 0)
	{
		cannonLaunch_mid = 0;
		cannonLaunch_min = 0;
	}
	else if (cannonNum_mid == 0 && cannonNum_max == 0 && cannonNum_min == 0)
	{
		cannonLaunch_min = 0;
		cannonLaunch_max = 0;
		cannonLaunch_min = 0;
	}
	
	m_BulltIndex.clear();
	for (int i = 0; i < m_pDelegate->getShareObj()->m_vEnemys.size(); i++)
	{
		auto ship = m_pDelegate->getShareObj()->m_vEnemys[i];
		int tag = ship->m_Info->_tag;
		int defense = ship->m_Info->_defense;
		int myId = m_Info->_sid;
		for (int j = 0; j  < shipTag_max.size(); j ++)
		{
			if (tag == shipTag_max[j])
			{

				int attack = ceil((float)cannonLaunch_max / sqrt(shipTag_max.size()));
				attack = attack * 3 / (1+ defense * 0.02f);
				log("attack :: %d, defence :: %d", attack, defense);


				if (attack < 0)
				{
					attack = 0;
				}
				attack = attack * cannonNum_max;
				ship->hurt(attack,myId);	
				auto eff = Sprite::create();
				PARTICLE_TYPE eAnimationIndex = PT_EXPLOSION;
				eff->setScale(1.0f);
				eAnimationIndex = PT_EXPLOSION;
				auto animation = m_pDelegate->getEffective()->getAnimation(eAnimationIndex);
				eff->runAction(Sequence::create(Animate::create(animation->clone()), CCCallFunc::create([=]{eff->stopAllActions(); eff->removeFromParent(); }), nullptr));
				ship->addChild(eff,1000);;
				continue;
			}
		}
		for (int j = 0; j  < shipTag_mid.size(); j ++)
		{
			if (tag == shipTag_mid[j])
			{
				int attack = ceil((float)cannonLaunch_mid / sqrt(shipTag_mid.size()));
				attack = attack * 3 / (1+ defense * 0.02f);
				//log("attack :: %d, defence :: %d", attack, defense);
				if (attack < 0)
				{
					attack = 0;
				}
				
				attack = attack * cannonNum_mid;
				ship->hurt(attack, myId);
				auto eff = Sprite::create();
				PARTICLE_TYPE eAnimationIndex = PT_EXPLOSION;
				eff->setScale(1.0f);
				eAnimationIndex = PT_EXPLOSION;
				auto animation = m_pDelegate->getEffective()->getAnimation(eAnimationIndex);
				eff->runAction(Sequence::create(Animate::create(animation->clone()), CCCallFunc::create([=]{eff->stopAllActions(); eff->removeFromParent(); }), nullptr));
				ship->addChild(eff, 1000);
				continue;
			}
		}
		for (int j = 0; j  < shipTag_min.size(); j ++)
		{
			if (tag == shipTag_min[j])
			{
				int attack = ceil((float)cannonLaunch_min / sqrt(shipTag_min.size()));
				attack = attack * 3 / (1+ defense * 0.02f);
				//log("attack :: %d, defence :: %d", attack, defense);
				if (attack < 0)
				{
					attack = 0;
				}
				attack = attack * cannonNum_min;

				ship->hurt(attack, myId);
				auto eff = Sprite::create();
				PARTICLE_TYPE eAnimationIndex = PT_EXPLOSION;
				eff->setScale(1.0f);
				eAnimationIndex = PT_EXPLOSION;
				auto animation = m_pDelegate->getEffective()->getAnimation(eAnimationIndex);
				eff->runAction(Sequence::create(Animate::create(animation->clone()), CCCallFunc::create([=]{eff->stopAllActions(); eff->removeFromParent(); }), nullptr));
				ship->addChild(eff, 1000);
				continue;
			}
		}
	}

	m_fireShipSide = 2;
	fireeffective();

	//TODO　增加齐射的特效
}

/*
 * TODO:增加注释
 */
void TVBattleMainShip::shipMove(Vec2 targetPos)
{
	//move
	this->setLocalZOrder(MAX_Z_ORDER - _position.y);
	if(!m_bIsMoveEnd)
	{
		Size winsize = Director::getInstance()->getWinSize();
		Vec2 ship_pos = _position;
		Point map_pos = _parent->getPosition();
		bool isTurning = false;
		//转向移动，使用转向限速
		if (m_vRoad[0].turnR != 0 && m_fLimitMoveSpeed > 0.0f)
		{
			//Vec2 diffVec2 = ship_pos - m_vRoad[0].centerPoint;
			isTurning = true;
			m_fCurSpeed = m_fLimitMoveSpeed;
		}
		//log("my current speed %f", m_fCurSpeed);
		float tmpy = m_fCurSpeed*m_speedFactor * sin(CC_DEGREES_TO_RADIANS(m_currentDirect));
		float tmpx = m_fCurSpeed*m_speedFactor * cos(CC_DEGREES_TO_RADIANS(m_currentDirect));
		//直线修正
		if(!isTurning){
			Vec2 tmpDiff = m_vRoad[0].pt2 - ship_pos;
			if(tmpDiff.y != 0.0f && tmpDiff.x != 0.0f){
				tmpx = tmpy/tmpDiff.y * tmpDiff.x;
			}
		}
		//log("tmp y: %f, tmp x:%f y/x:%f", tmpy, tmpx, tmpy/tmpx);
		if (m_bIsCenterY)
		{
			map_pos.y -= tmpy;
		}else
		{
			float diff = ship_pos.y + map_pos.y;
			if (diff < winsize.height/2 && std::abs(map_pos.y) > winsize.height)
			{
				map_pos.y += (winsize.height/2 - diff);
				m_bIsCenterY = true;
			}
			if (diff > winsize.height/2 && std::abs(map_pos.y) < 10.0)
			{
				map_pos.y -= (diff - winsize.height/2);
				m_bIsCenterY = true;
			}
		}

		if (m_bIsCenterX)
		{
			map_pos.x -= tmpx;
		}else
		{
			float diff = ship_pos.x + map_pos.x;
			if (diff < winsize.width/2 && std::abs(map_pos.x) > winsize.width)
			{
				map_pos.x += (winsize.width/2 - diff);
				m_bIsCenterX = true;
			}
			if (diff > winsize.width/2 && std::abs(map_pos.x) < 10.0)
			{
				map_pos.x -= (diff - winsize.width/2);
				m_bIsCenterX = true;
			}
		}
		//log("before ship_pos x:%f y:%f", ship_pos.x, ship_pos.y);

		ship_pos.y += tmpy;
		ship_pos.x += tmpx;


		//log("after ship_pos x:%f y:%f", ship_pos.x, ship_pos.y);
		if (ship_pos.x < 0.1 || ship_pos.y < 0.1 || ship_pos.x > (winsize.width * 3 -1) || ship_pos.y >  (winsize.height *3 -1))
		{
			m_bIsMoveEnd = true;
			m_fCurSpeed = 0;
			return;
		}

		Vec2 diff = m_vRoad[0].pt2 - ship_pos;
		//log("distance :%f, angle:%f, m_currentDirect:%f, tan:%f", diff.y / diff.x, CC_RADIANS_TO_DEGREES(diff.getAngle()), m_currentDirect, tan(CC_DEGREES_TO_RADIANS(m_currentDirect)));
		if (pow(diff.x,2) + pow(diff.y,2) <= 2 * pow(m_fCurSpeed,2))
		{
			ship_pos = m_vRoad[0].pt2;
			map_pos = Vec2(winsize.width/2,winsize.height/2) - m_vRoad[0].pt2;
			m_vRoad.erase(m_vRoad.begin());
			m_bIsRotationEnd = true;
			
			if (m_vRoad.size() < 1)
			{
				m_bIsMoveEnd = true;
				m_fCurSpeed = 0;
			}else
			{
				Vec2 tempPos = m_vRoad[0].pt2 - ship_pos;
				float degrees = CC_RADIANS_TO_DEGREES(tempPos.getAngle());//CC_RADIANS_TO_DEGREES(atan2f(tempPos.y,tempPos.x));
				m_pShipIcon->setRotation(-degrees);
				m_currentDirect = degrees;
			}
		}

		Camera  *c  = Director::getInstance()->getRunningScene()->getCameras().front();
		if(c)
		{
			auto diff2 = ship_pos - m_shipStartPostion;//m_cameraStartPostion
			Size contentSize = _parent->getContentSize();
			auto size = Director::getInstance()->getWinSize();
			auto cameraPostion = m_cameraStartPostion+diff2;

			if(cameraPostion.x > contentSize.width / 2)
			{
				cameraPostion.x = contentSize.width / 2;
			}
			if(cameraPostion.y > contentSize.height / 2)
			{
				cameraPostion.y = contentSize.height / 2;
			}

			if (cameraPostion.x < -contentSize.width / 2 + size.width) 
			{
				cameraPostion.x = -contentSize.width / 2 + size.width;
			}

			if(cameraPostion.y < -contentSize.height / 2 + size.height)
			{
				cameraPostion.y = -contentSize.height / 2 + size.height;
			}

			c->setPosition(cameraPostion);
			this->setPosition(ship_pos);
		}
	}
}

void TVBattleMainShip::shipDied()
{
	TVBasicShip::shipDied();
	auto func_0 = [&]()->std::vector<TVBasicShip*>& 
	{
		if (m_Info->_isFriend)
		{
			return m_pDelegate->getShareObj()->m_vMyFriends;
		}else
		{
			return m_pDelegate->getShareObj()->m_vEnemys;
		}
	};

	std::vector<TVBasicShip*>& friends = func_0();
	auto iter = find_if(friends.begin(),friends.end(),[&](Node* node)
	{
		if (node->getTag() == this->getTag())
		{
			return true;
		}else
		{
			return false;
		}
	});
	if (iter != friends.end())
	{
		m_pDelegate->specialButtonEvent(this,"event_die");
		this->unscheduleUpdate();
		m_pDelegate->getShareObj()->m_vDiedShips.push_back(*iter);
		friends.erase(iter);

	}
	if (m_pShipWave)
	{
		m_pShipWave->removeFromParent();
	}
	//adjust morale
	if (m_Info->_isFriend)
	{
		m_pDelegate->getShareObj()->addEnemyMoraleByKill(1);
	}else
	{
		m_pDelegate->getShareObj()->addMoraleByKill(1);
	}
	m_bIsDied = true;
	m_pAttackDistanceIcon->removeFromParent();
	m_pAttackDistanceIcon = nullptr;

	int friendNums = m_pDelegate->getShareObj()->m_vMyFriends.size();
	int enemysNums = m_pDelegate->getShareObj()->m_vEnemys.size();
	if (friendNums < 1 || enemysNums < 1)
	{
		m_bEndGame = true;
	}
	auto actionSeq = Sequence::create(CallFunc::create(this, callfunc_selector(TVBattleMainShip::shipMianDieAnimation)), DelayTime::create(2.9f), CallFunc::create(CC_CALLBACK_0(TVBattleMainShip::shipDiedAnimationEnd, this)), nullptr);
	this->runAction(actionSeq);
	this->removeFromPhysicsWorld();
	m_pDelegate->getBattleUI()->setMainShipDied();
}
void TVBattleMainShip::shipMianDieAnimation()
{
	auto shipSprite1 = Sprite::create();

	float shipScale = m_pyBodySize.width / 200;
	shipSprite1->setScale(shipScale);
	this->addChild(shipSprite1);
	shipSprite1->setLocalZOrder(1001);
	shipSprite1->setAnchorPoint(Vec2(0.5, 0.27));

	auto shipSprite2 = Sprite::create();
	shipSprite2->setScale(shipScale);
	this->addChild(shipSprite2);
	shipSprite2->setLocalZOrder(1000);

	shipSprite1->setRotation(m_pShipIcon->getRotation());
	shipSprite2->setRotation(m_pShipIcon->getRotation());

	//11帧后船只消失，26帧后沉船效果
	auto animationFire = m_pDelegate->getEffective()->getAnimation(PT_SHIP_FIREING);
	auto myanimationFire = Animate::create(animationFire);

	auto animationSink = m_pDelegate->getEffective()->getAnimation(PT_SHIP_SINKING);
	auto myanimationSink = Animate::create(animationSink);

	auto seq = Sequence::create(DelayTime::create(0.55f), CallFunc::create(CC_CALLBACK_0(TVBattleMainShip::setShipAndBooldIconVisible, this)),
		DelayTime::create(0.75f), myanimationSink, nullptr);
	shipSprite1->runAction(myanimationFire);
	shipSprite2->runAction(seq);
}
void TVBattleMainShip::shipDiedAnimationEnd()
{
	this->removeFromParent();
	if (m_bEndGame)
	{
		m_pDelegate->getBattleUI()->checkIsWin(0);
	}
}

void TVBattleMainShip::shipRotation(Vec2 targetPos)
{
	//rotation
	if (!m_bIsRotationEnd && m_NextRotation != 0)
	{
		normalDriect();
	
		if (std::abs(m_NextRotation) <= m_fRoateAngle)
		{
			m_currentDirect += m_NextRotation;
			
			m_NextRotation = 0;
			m_bIsRotationEnd = true;
			m_pShipIcon->setRotation(-m_currentDirect);
			return;
		}
		if (m_NextRotation > 0)
		{
			m_NextRotation -= m_fRoateAngle;
			m_currentDirect += m_fRoateAngle;
			m_pShipIcon->setRotation(-m_currentDirect);
		}else
		{
			m_NextRotation += m_fRoateAngle;
			m_currentDirect -= m_fRoateAngle;
			m_pShipIcon->setRotation(-m_currentDirect);
		}	
	}
}

void TVBattleMainShip::hurt(Node* bullet)
{
	if (m_bIsDied)
	{
		return;
	}
	
	TVBasicShip::hurt(bullet);
	if (bullet)
	{
		Bullet* bllt = dynamic_cast<Bullet*>(bullet);
		m_pDelegate->getShareObj()->m_nLastAttackFlagshipTag = bllt->getBulletInfo()->_tag;
	}
}

/*
 * 设置玩家点击的位置作为目的地。
 */
void TVBattleMainShip::touchForTarget(Vec2 targetPos)
{
	if (m_bIsDied || !m_pMyMainPath)
	{
		return;
	}
	
	int state = getBufferState();
	if (state > 0)
	{
		return;
	}
	float distanceDiff = targetPos.getDistanceSq(_position);
	if (distanceDiff < 5.f)
	{
		return;
	}
	m_TargetPos = targetPos;

	if (m_BulltIndex.empty())
	{
		//this->stopAllActions();
	}
	m_vRoad.clear();
	map->removeAllWallShip();
	const auto& ememys = m_pDelegate->getShareObj()->m_vEnemys;
	const auto& friends = m_pDelegate->getShareObj()->m_vMyFriends;


	for (int i = 0; i < ememys.size(); i++ )
	{
		auto &bs = ememys.at(i);
		auto box = bs->getHitTestPointsNineScreen();
		map->addWallShip(box[0],box[1],box[2],box[3]);
	}
	for (int i = 0 ; i< friends.size(); i++)
	{
		auto &bs = friends.at(i);
		auto box = bs->getHitTestPointsNineScreen();
		if (i == 0 && bs->getType() == MAIN_SHIP)
		{
			map->addSelfPoints(box[0],box[1],box[2],box[3]);
			continue;
		}
		map->addWallShip(box[0],box[1],box[2],box[3]);
	}
	m_currentDirect = -m_pShipIcon->getRotation();
	normalDriect();
	float R = 100; // set a default value to avoid crash.
	if(m_fMoveSpeed == 0 || m_fRotationSpeed == 0){
		//should log here. weird.
	}else{
		R = (360 / atan2f(m_fMoveSpeed, m_fRotationSpeed)) * m_fMoveSpeed / 2 / 3.1415f;
		log("my turn radius %f", R);
		float distance = sqrt(targetPos.getDistanceSq(_position));
		if( distance < R*2){
			//短距离目标需要考虑减小速度和转向半径。
			R = distance / 2;
			m_fLimitMoveSpeed = 2*3.1415f*R/(360 / atan2f(m_fMoveSpeed, m_fRotationSpeed));
		}else{
			m_fLimitMoveSpeed = m_fMoveSpeed;
		}
//		m_fLimitMoveSpeed = m_fMoveSpeed;
		log("my limit speed %f", m_fLimitMoveSpeed);
	}
	m_vRoad = map->getSmoothRoad(Vec2(cos(CC_DEGREES_TO_RADIANS(m_currentDirect)),sin(CC_DEGREES_TO_RADIANS(m_currentDirect))),_position,targetPos,R, true);
	if(!m_vRoad.empty())
	{	 
#if defined(LINUX) || defined(WIN32)
		m_pMyMainPath->drawPathByVector(m_vRoad);
#endif
		m_NextRotation = 0;
		m_bIsMoveEnd = false;
		m_ExpediteSpeed_flag = 1;
	}
}


void TVBattleMainShip::findTargetAgain()
{
	if (m_bIsMoveEnd && m_bIsRotationEnd)
	{
		return;
	}
	touchForTarget(m_TargetPos);
}

void TVBattleMainShip::moveToTaraget(Vec2 targetPos)
{
	m_TargetPos = targetPos;
	m_NextRotation = 0;
	m_fCurSpeed = 0;
	m_bIsMoveEnd = false;
	m_bIsRotationEnd = false;
	m_ExpediteSpeed_flag = 1;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	shipRotation(m_TargetPos);
	shipMove(m_TargetPos);
#endif
}

/*
 * 原地转向
 */
void TVBattleMainShip::zeroSpeedRotaion()
{
	if (m_vRoad.size() < 1)
	{
		return;
	}
	m_pShipIcon->stopAllActions();
	m_bZeroSpeedRotation = true;
	m_currentDirect = -m_pShipIcon->getRotation();
	normalDriect();

	m_pShipIcon->setRotation(-m_currentDirect);
	
	float fps = 60;
	Vec2 diffPos = m_vRoad[0].pt1 - _position;//error
	float nextAngle = CC_RADIANS_TO_DEGREES(atan2f(diffPos.y,diffPos.x));
	float diffAngle = std::abs(nextAngle - m_currentDirect);
	diffAngle = diffAngle > 180.f ? 360.f - diffAngle:diffAngle;
	Sequence* seq = nullptr;
	if (m_currentDirect < 90.f && m_currentDirect > -90.f)
	{
		if (diffPos.y - diffPos.x * tan(CC_DEGREES_TO_RADIANS(m_currentDirect)) > 0) 
		{
			seq = Sequence::create(RotateBy::create(diffAngle/(m_fRotationSpeed * fps),-diffAngle),CCCallFunc::create(CC_CALLBACK_0(TVBattleMainShip::zeroRotationEnd,this)),nullptr);
			m_currentDirect += diffAngle;
		}else
		{
			seq = Sequence::create(RotateBy::create(diffAngle/(m_fRotationSpeed * fps),diffAngle),CCCallFunc::create(CC_CALLBACK_0(TVBattleMainShip::zeroRotationEnd,this)),nullptr);
			m_currentDirect -= diffAngle;
		}
	}else
	{
		if (diffPos.y - diffPos.x * tan(CC_DEGREES_TO_RADIANS(m_currentDirect)) > 0) 
		{
			seq = Sequence::create(RotateBy::create(diffAngle/(m_fRotationSpeed * fps),diffAngle),CCCallFunc::create(CC_CALLBACK_0(TVBattleMainShip::zeroRotationEnd,this)),nullptr);
			m_currentDirect -= diffAngle;
		}else
		{
			seq = Sequence::create(RotateBy::create(diffAngle/(m_fRotationSpeed * fps),-diffAngle),CCCallFunc::create(CC_CALLBACK_0(TVBattleMainShip::zeroRotationEnd,this)),nullptr);
			m_currentDirect += diffAngle;
		}

	}
	m_pShipIcon->runAction(seq);
}

void TVBattleMainShip::zeroRotationEnd()
{
	normalDriect();
	m_bZeroSpeedRotation = false;
}

void TVBattleMainShip::checkAttackDistanceIcon()
{
	if (m_pAttackDistanceIcon && m_pShipIcon)
	{
		m_pAttackDistanceIcon->setPosition(_position);
		m_pAttackDistanceIcon->setRotation(m_pShipIcon->getRotation());
	}
}

void TVBattleMainShip::addLogAction(int type,float arg_1 /* = 0 */,int arg_2 /* = 0 */,Vec2 pos /* = Vec2 */)
{
	ShipAction_FLAG info = {};
	info.curId = m_Info->_tag;
	info.action_type = type;
	switch (type)
	{
	case ACT_CHANGE_TARGET:
		{
			info.targetId = arg_2;
			break;
		}
	case ACT_CHANGE_POS:
		{
			info.pos = pos;
			break;
		}
	case ACT_CHANGE_STRATEGY:
		{
			info.propsId = arg_2;
			break;
		}
	case ACT_LOCK:
		{
			info.propsId = 1;
			break;
		}
	case ACT_UNLOCK:
		{
			info.propsId = 0;
			break;
		}
	case ACT_USE_PROPS:
		{
			info.propsId = arg_2;
			break;
		}
	case ACT_ADD_BUFFER:
		{
			info.propsId = arg_2;
			break;
		}
	case ACT_REMOVE_BUFFER:
		{
			info.propsId = arg_2;
			break;
		}
	case ACT_DIED:
		{
			break;
		}
	case ACT_COLD_BATTLE:
		{
			info.targetId = arg_2;
			break;
		}
	default:
		break;
	}
	WriterLog::getInstance()->addActionLog(info);
}
/*
float BattleMainShip::getShipMoveSpeed()
{
	if (m_bIsMoveEnd)
	{
		return 0;
	}
	return m_fMoveSpeed;
}
*/
