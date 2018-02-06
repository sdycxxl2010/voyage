#include "TVBattleShip.h"
#include "TVBattleManager.h"
#include "Bullet.h"

#include "SystemVar.h"
#include "WriterLog.h"
#include "Utils.h"

#define OFFSET 50.0f
#define CHECK_RADIUS 5.0f

TVBattleShip::TVBattleShip()
	:m_pEnable_touch(nullptr)
	,m_pDisable_touch(nullptr)
	,m_pPreTarget(nullptr)
	,m_TargetPos(0,0)
{
	m_NextRotation = 0;
	m_fRotationSpeed = 1.f;
	for (int i = 0; i < 5; i++)
	{
		m_gHurtPoints[i] = 0;
	}
	m_nFightType = 0;
	m_bIsChargeInitiative = true;
}

TVBattleShip::~TVBattleShip()
{
	
}

void TVBattleShip::onEnter()
{
	TVBasicShip::onEnter();
	m_pEnable_touch = Sprite::create();
	m_pEnable_touch->setTexture(ENABLE_ATK_ICON);
	m_pDisable_touch = Sprite::create();
	m_pDisable_touch->setTexture(ENABLE_NOT_ICON);
	m_pEnable_touch->setVisible(false);
	m_pDisable_touch->setVisible(false);
	_parent->addChild(m_pEnable_touch,2);
	_parent->addChild(m_pDisable_touch,2);
}

void TVBattleShip::onExit()
{
	TVBasicShip::onExit();
}

void TVBattleShip::startBattle()
{
	TVBasicShip::startBattle();
	m_TargetPos = _position;
}

TVBattleShip* TVBattleShip::createShip(TVBattleManager*manager, FightShipInfo* shipInfo, int tag, bool isFriend, int s, int fightType, int index)
{
	auto bs = new TVBattleShip();
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

bool TVBattleShip::init()
{
	bool pRet = false;
	do 
	{
		TVBasicShip::init();
		//m_ExpediteSpeed = m_fMoveSpeed * m_fMoveSpeed / (2 * 60.0) * 2;
		m_ExpediteSpeed = 0.01f; //每毫秒的加速度
		this->setAnchorPoint(Vec2(0,0));
		pRet = true;
	} while (0);
	return pRet;
}

void TVBattleShip::callbackEventByIndex(int tag)
{

}

void TVBattleShip::specialButtonEvent(Node* target,std::string name,float varValue)
{

}

void TVBattleShip::update(float delate)
{

	TVBasicShip::update(delate);
	checkSelected();
	if (!m_PrepareFire_OK)
	{
		return;
	}
	if (m_BattleState)
	{
		return;
	}
	changeStrategy();

	m_PreTime += int(delate * 1000);
	if ( m_PreTime > 500)
	{
		m_PreTime = 0;
		if(SHIP_FIRED == m_ShipStatus)
		{
			m_ShipStatus = SHIP_LOADING;
			m_pFlagIcon->runAction(ProgressTo::create(0.3f,0.f));
		}else if(m_pFlagIcon->getPercentage() > 99.9)
		{
			m_ShipStatus = SHIP_READY;

		}else if(m_ShipStatus == SHIP_LOADING)
		{
			if (isFired)
			{
				isFired = false;
			}
			m_pFlagIcon->setPercentage(m_pFlagIcon->getPercentage() + 100.0f / (2 * m_fCDTime));
		}
	}
}

void TVBattleShip::changeStrategy()
{
	flowShipDispense();
}

void TVBattleShip::flowShipDispense()
{
	int buffer = getBufferState();
	if (buffer > 0)
	{
		bufferStateDisponse(buffer);
		return;
	}
	switch (m_eMode)
	{
	case MODEL_FREE:
		battleFree();
		break;
	case MODEL_FOCUS:
		battleFocus();
		break;
	case MODEL_PROTECT:
		battleProtected();
		break;
	default:
		break;
	}
}

void TVBattleShip::battleFree()
{
	TVBasicShip* nearShip = nullptr;
	float distance = 99999.0;
	std::vector<TVBasicShip*>* EnemyShips = getEnemyShips(nullptr);
	if(!EnemyShips)
		return;
	int points[5];
	getEnemyHurtPoints(points);
	int extValue = 0;
	if (m_Info->_isFriend)
	{
		extValue = 5;
	}else
	{
//		log("enemys");
	}
	//根据仇恨寻找目标
	int sortedTargetTags[5] = {0,0,0,0,0};	

	for (int i = 0;i < 5; i++)
	{		
		int value = 0;
		for (int j = i; j < 5; j++)
		{
			if (value < points[j])
			{
				value = points[j];
				sortedTargetTags[i] = j + 1;
				points[j] = points[i];
				points[i] = value;
			}
		}
		if (sortedTargetTags[i] == 0)
		{
			break;
		}else
		{
			points[sortedTargetTags[i] - 1] = 0;
		}
	}

	for (int i = 0; i < 5; i++)
	{	
		for (auto item = EnemyShips->begin(); item != EnemyShips->end(); item++)
		{
			if((*item)->isDied()){
				continue;
			}
			if ((*item)->getTag() == sortedTargetTags[i] + extValue)
			{
				if ((*item)->isCloseBattle())
				{
					continue;
				}
				distance = (*item)->getPosition().distance(_position);
				nearShip = *item;
				//log("find target %d", nearShip->getTag());
				break;
			}
		}
		if (nearShip)
		{
			break;
		}
	}

	//寻找最近的船只作为目标
	if(!nearShip)
	{
		distance = 99999.0;
		for (auto item = EnemyShips->begin();item != EnemyShips->end(); item++)
		{
			if((*item)->isDied()){
					continue;
			}
			float tempDis = (*item)->getPosition().distance(_position);
			if (tempDis < distance)
			{
				distance = tempDis;
				nearShip = *item;
				//log("find target by distance %d", nearShip->getTag());
			}
		}
	}

	if(!nearShip){
		log("no nearship!");
	}else{
//		log("battle %d target %d", this->getTag(), nearShip->getTag());
		distance = std::abs(nearShip->getPosition().distance(_position));
	}
	/*
	 *  如果对方距离在炮击范围，炮击，并且继续移动。
	 *  如果水手数是目标1,5倍 或者舰队水手数是对方的1.5倍同时自己船只不比敌人少，启动白刃战。
	 *
	 */

	if (m_bIsChargeInitiative && nearShip && (nearShip->m_Info->_sailornum * 1.5f <= this->m_Info->_sailornum ||
			(nearShip->m_Info->_sailornum <= this->m_Info->_sailornum && getEnemySailors() * 1.5f < getFriendSailors() ))){
		//先判断是否要攻击
		auto targetPos = nearShip->getPosition();
		auto diffPos = targetPos - _position;
		float fireRange = CC_RADIANS_TO_DEGREES(Vec2::angle(diffPos,Vec2(cos(CC_DEGREES_TO_RADIANS(m_currentDirect)),sin(CC_DEGREES_TO_RADIANS(m_currentDirect)))));
		if ((fireRange >= 35.f && fireRange <= 145.f ) && nearShip && SHIP_READY == m_ShipStatus && !nearShip->isCloseBattle())
		{
			//log("battle fire with moving %d", this->getTag());
			firedByDistance(nearShip,distance);
		}
		//然后再逼宫白刃战
		log("battle bayonet %d", this->getTag());
		moveAndRotation(nearShip->getPosition());
	}else{
		if (nearShip && distance <= m_CurLaunchDistance)
			{
//				log("battle in distance %d", this->getTag());
				m_bIsMoveEnd = true; // npc stop
				auto targetPos = nearShip->getPosition();
				auto diffPos = targetPos - _position;
				if (m_pPreTarget != nearShip)
				{
					//log("battle change target %d", this->getTag());
					m_pPreTarget = nearShip;
					addLogAction(ACT_CHANGE_TARGET,0,m_pPreTarget->m_Info->_tag);
				}
				float fireRange = CC_RADIANS_TO_DEGREES(Vec2::angle(diffPos,Vec2(cos(CC_DEGREES_TO_RADIANS(m_currentDirect)),sin(CC_DEGREES_TO_RADIANS(m_currentDirect)))));
				if (fireRange < 35.f || fireRange > 145.f )
				{
					//log("battle adjust angle %d",this->getTag());
					m_PrepareFire_OK = false;
					m_currentDirect -= 90.0;
					auto seq = Sequence::create(RotateBy::create(1.0f,90.f),CCCallFunc::create(CC_CALLBACK_0(TVBattleShip::adjustAngleFinish,this)),nullptr);
					m_pShipIcon->runAction(seq);
				}else
				{
					//如果没有装载好，或者敌船处于白刃战，就等待。
					if (nearShip && SHIP_READY == m_ShipStatus && !nearShip->isCloseBattle())
					{
						//log("battle fire %d", this->getTag());
						firedByDistance(nearShip,distance);
					}else{
						//log("battle fire not ready, hold up, %d", this->getTag());
					}
					//fired(nearShip->getPosition() + Vec2(10.f,10.f));
				}
				return;
			}else if (distance > m_CurLaunchDistance  && nearShip)
			{
//				log("battle move to fire positon %d", this->getTag());
				moveToFirePosition(nearShip->getPosition());
			}
	}

}


/*
 * 获得敌方所有船只水手
 */
int TVBattleShip::getEnemySailors(){
	int count = 0;
	std::vector<TVBasicShip*>* EnemyShips = getEnemyShips(nullptr);

	for (auto item = EnemyShips->begin(); item != EnemyShips->end(); item++){
		TVBasicShip* Ship = *item;
		count += Ship->m_Info->_sailornum;
	}

	return count;
}

/*
 * 获得友方所有船只水手
 */
int TVBattleShip::getFriendSailors(){
	int count = 0;
	std::vector<TVBasicShip*>* Ships = getFriendShips();

	for (auto item = Ships->begin(); item != Ships->end(); item++){
		TVBasicShip* Ship = *item;
		count += Ship->m_Info->_sailornum;
	}

	return count;
}

/*
 * 1秒钟，如果目标移动了，重新计算一次路径
 */
void TVBattleShip::moveAndRotation(Vec2 targetPos)
{
	m_bIsMoveEnd = false;
	float diffDistance = targetPos.getDistance(m_TargetPos);
	
	if (Utils::gettime() - last_cal_time < 1000 && m_vRoad.size() > 0)
	{
		npcMove();
		npcRotation();
	}else
	{
		long start = Utils::gettime();
		Size winSize = Director::getInstance()->getWinSize();
		if (targetPos.x > winSize.width * 3 || targetPos.x < 0 || targetPos.y < 0 || targetPos.y > winSize.height * 3)
		{
			return;
		}
		m_vRoad.clear();
		map->removeAllWallShip();
		const auto& ememys = m_pDelegate->getShareObj()->m_vEnemys;
		const auto& friends = m_pDelegate->getShareObj()->m_vMyFriends;

		for (int i = 0; i < ememys.size(); i++ )
		{
			auto &bs = ememys.at(i);
			auto box = bs->getHitTestPointsNineScreen();
			if (bs == this)
			{
				map->addSelfPoints(box[0],box[1],box[2],box[3]);
				continue;
			}
			map->addWallShip(box[0],box[1],box[2],box[3]);
		}
		for (int i = 0 ; i< friends.size(); i++)
		{
			auto &bs = friends.at(i);
			auto box = bs->getHitTestPointsNineScreen();
			if (bs == this)
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
			log("turn radius %f", R);
			float distance = sqrt(targetPos.getDistanceSq(_position));
//			if( distance < R*2){
//				Vec2 diffPos = targetPos - _position;
//				float angle = std::abs(CC_RADIANS_TO_DEGREES(Vec2::angle(diffPos,Vec2(cos(CC_DEGREES_TO_RADIANS(m_currentDirect)),sin(CC_DEGREES_TO_RADIANS(m_currentDirect))))));
//				if(angle == 180.0f || angle == -180.0f){
//					R = tan(CC_DEGREES_TO_RADIANS(angle)) * distance;
//				}else {
//					R = distance/2/std::abs(sin(CC_DEGREES_TO_RADIANS(angle)));
//				}
//				R = distance /2;
//				m_fLimitMoveSpeed = 2*3.1415f*R/(360 / atan(m_fMoveSpeed / m_fRotationSpeed));
//			}else{
//				m_fLimitMoveSpeed = m_fMoveSpeed;
//			}
			if( distance < R*2){
				//短距离目标需要考虑减小速度和转向半径。
				R = distance / 2;
				m_fLimitMoveSpeed = 2*3.1415f*R/(360 / atan2f(m_fMoveSpeed, m_fRotationSpeed));
			}else{
				m_fLimitMoveSpeed = m_fMoveSpeed;
			}
		}
		m_vRoad = map->getSmoothRoad(Vec2(cos(CC_DEGREES_TO_RADIANS(m_currentDirect)),sin(CC_DEGREES_TO_RADIANS(m_currentDirect))),_position,targetPos,R);
		if(!m_vRoad.empty())
		{
#if defined(LINUX) || defined(WIN32)
			m_pMyMainPath->drawPathByVector(m_vRoad);
#endif	
			m_TargetPos = targetPos;
			m_ExpediteSpeed_flag = 1;
			addLogAction(ACT_CHANGE_POS,0,0,targetPos); //writer log
			if (m_vRoad.size() > 0)
			{
				if(m_vRoad[0].angle != 0 && m_NextRotation == 0)//m_vRoad.size() > 1)
				{

					m_NextRotation = CC_RADIANS_TO_DEGREES(m_vRoad[0].angle);
				}
				if (m_vRoad[0].angle != 0 && m_vRoad[0].turnR != 0)
				{
					float rad = m_fCurSpeed/m_vRoad[0].turnR/2;
					if(rad <= 1 && rad >= -1){
						m_fRoateAngle = CC_RADIANS_TO_DEGREES(2 * asin(rad));
					}
				}
			}
		}
		long cost = Utils::gettime() - start;
		last_cal_time = Utils::gettime();
		log("calculate using %ld", cost);
	}
}
/*
 *  保护旗舰模式
 */
void TVBattleShip::battleProtected()
{
	std::vector<TVBasicShip*>* EnemyShips = getEnemyShips(nullptr);
	int attackFlagshipTag = m_pDelegate->getShareObj()->m_nLastAttackFlagshipTag;
	if (attackFlagshipTag > 0)
	{
		auto iter = find_if(EnemyShips->begin(),EnemyShips->end(),[attackFlagshipTag](TVBasicShip* target)
		{ 
			if (target->getTag() == attackFlagshipTag)
			{
					return true;
				}else
				{
					return false;
			}
		});
		if (iter != EnemyShips->end())
		{
			int lanchDistance = (*iter)->getPosition().getDistance(_position);
			if ((*iter)->m_Info->_sailornum <= this->m_Info->_sailornum / 2 && m_bIsChargeInitiative){
					moveAndRotation((*iter)->getPosition());
			}else	if (lanchDistance < m_CurLaunchDistance)
			{
				m_bIsMoveEnd = true;
				if (SHIP_READY == m_ShipStatus)
				{
					firedByDistance(*iter,lanchDistance);
				}
			}else
			{
				moveToFirePosition((*iter)->getPosition());
			}
			
		}else
		{
			m_pDelegate->getShareObj()->m_nLastAttackFlagshipTag = -1;
		}
	}else
	{
		//旗舰没有受到攻击时按仇恨模式
		battleFree();
	}
}

/*
 * 集中火力模式
 */
void TVBattleShip::battleFocus() {
	std::vector<TVBasicShip*>* EnemyShips = getEnemyShips(nullptr);
	int index = m_pDelegate->getShareObj()->m_nLockShipTag;
	TVBasicShip* targetShip = nullptr;
	if (index > 0) {
		for (auto iter = EnemyShips->begin(); iter != EnemyShips->end();
				iter++) {
			if ((*iter)->getTag() == index) {
				targetShip = *iter;
				break;
			}
		}
		if (targetShip != nullptr) {
			int lanchDistance = (targetShip)->getPosition().getDistance(_position);
			if ((targetShip)->m_Info->_sailornum <= this->m_Info->_sailornum / 2 && m_bIsChargeInitiative) {
				moveAndRotation((targetShip)->getPosition());
			}else if (lanchDistance < m_CurLaunchDistance) {
				m_bIsMoveEnd = true;
				if (SHIP_READY == m_ShipStatus) {
					firedByDistance(targetShip, lanchDistance);
				}
			} else {
				moveToFirePosition((targetShip)->getPosition());
			}

		} else {
			m_pDelegate->getShareObj()->m_nFlagShipAttackShipTag = -1;
		}
	} else {
		//旗舰没有攻击目标时按自由仇恨模式
		battleFree();
	}
}

/*
 * Move to a proper position so it can fire to target.
 */
void TVBattleShip::moveToFirePosition(Vec2 targetPosition){
	Vec2 diffVec = targetPosition - this->getPosition();
	float angle = diffVec.getAngle();
	//add 5 to get it a little bit nearby.
	int distance = m_CurLaunchDistance - 5;
	Vec2 firePosition = Vec2(targetPosition.x - distance * cos(angle), targetPosition.y - distance* sin(angle));
	moveAndRotation(firePosition);
}

void TVBattleShip::hurt(Node* bullet)
{
	auto *bul = dynamic_cast<Bullet*>(bullet);
	auto *bulletInfo = bul->getBulletInfo();

	TVBasicShip::hurt(bullet);
	int tag = bulletInfo->_tag;
	if (m_Info->_isFriend != bulletInfo->_isFriend)
	{
		if (tag > 5)
		{
			tag %= 6;
			m_gHurtPoints[tag] += m_Info->_attack;
		}else
		{
			m_gHurtPoints[tag - 1] += m_Info->_attack;
		}
	}
	//animation
	//船被子弹打到产生碎片的动画
 //	auto shipBlash = Sprite::create();
 //	shipBlash->setScale(0.5);
 //	shipBlash->setAnchorPoint(Vec2(0.5,0.3));//爆炸效果移到船中央
 //	this->addChild(shipBlash, 100, 1000);
	//auto animation = m_pDelegate->getEffective()->getAnimation(PT_SHIP_ATTACKED);
	//auto myanimation = Animate::create(animation);
	//shipBlash->runAction(myanimation);
}

void TVBattleShip::getEnemyHurtPoints(int points[5])
{
	memcpy(points,m_gHurtPoints,5*sizeof(int));
}

void TVBattleShip::clearHateByTag(int tag)
{
	if (tag > 0)
	{
		m_gHurtPoints[tag-1] = 0;
	}
}

void TVBattleShip::shipDied()
{
	if (m_bIsDied)
	{
		return;
	}
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
		if (!m_Info->_isFriend)
		{
			bool IsVisible_1 = m_pEnable_touch->isVisible();
			bool IsVisible_2 = m_pDisable_touch->isVisible();
			if (IsVisible_1 || IsVisible_2)
			{
				lockShip(false);
			}
		}
		m_bIsDied = true;
		m_pDelegate->specialButtonEvent(this,"event_die");
		m_pDelegate->getShareObj()->m_vDiedShips.push_back(*iter);
		friends.erase(iter);
	}
	if (m_pShipWave)
	{
		m_pShipWave->removeFromParent();
	}
	//adjust morale
	if(m_Info->_isFriend)
	{
		if (m_Info->_tag == 1)
		{
			m_pDelegate->getShareObj()->addMoraleByKill(1);
		}else
		{
			m_pDelegate->getShareObj()->addMoraleByKill(0);
		}
		m_pDelegate->getBattleUI()->updateAnger(true);
	}else
	{
		m_pDelegate->getShareObj()->addEnemyMoraleByKill(0);
		m_pDelegate->getBattleUI()->updateAnger(false);
	}
	addLogAction(ACT_DIED);
	this->unscheduleUpdate();
	float time = 0;
	if (m_Info->_sailornum < 1)
	{
		time = 4;
	}	
	int friendNums = m_pDelegate->getShareObj()->m_vMyFriends.size();
	int enemysNums = m_pDelegate->getShareObj()->m_vEnemys.size();
	if (friendNums < 1 || enemysNums < 1)
	{
		m_bEndGame = true;
	}
	
	//auto actionSeq = Sequence::create(DelayTime::create(time),Blink::create(1.f,4),CallFunc::create(CC_CALLBACK_0(BattleShip::shipDiedAnimationEnd,this)),nullptr);
	auto actionSeq = Sequence::create(DelayTime::create(time), CallFunc::create(this, callfunc_selector(TVBattleShip::shipDieAnimation)), DelayTime::create(2.9f),
 			CallFunc::create(CC_CALLBACK_0(TVBattleShip::shipDiedAnimationEnd, this)), nullptr);
	this->runAction(actionSeq);
	this->removeFromPhysicsWorld();
}
void TVBattleShip::shipDieAnimation()
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

	auto seq = Sequence::create(DelayTime::create(0.55f), CallFunc::create(CC_CALLBACK_0(TVBattleShip::setShipAndBooldIconVisible, this)),
		DelayTime::create(0.75f), myanimationSink,nullptr);
	shipSprite1->runAction(myanimationFire);
	shipSprite2->runAction(seq);
}
void TVBattleShip::shipDiedAnimationEnd()
{
	this->removeFromParent();
	if (m_bEndGame)
	{
		m_pDelegate->getBattleUI()->checkIsWin(0);
	}
}

void TVBattleShip::onTouchEnded(Touch *touch, Event *unused_event)
{
	if (m_bIsDied)
	{
		return;
	}
	auto &ships = m_pDelegate->getShareObj()->m_vMyFriends;
	if (ships.size() < 1)
	{
		return;
	}
	if (ships[0]->getType() != MAIN_SHIP)
	{
		return;
	}
	if (!m_Info->_isFriend)
	{
		bool IsVisible_1 = m_pEnable_touch->isVisible();
		bool IsVisible_2 = m_pDisable_touch->isVisible();
		if (IsVisible_1 || IsVisible_2)
		{
			lockShip(false);
		}else
		{
			auto &Ememys = m_pDelegate->getShareObj()->m_vEnemys;
			for (auto enemy : Ememys)
			{
				dynamic_cast<TVBattleShip*>(enemy)->lockShip(false);
			}
			lockShip(true);
		}
	}
}

void TVBattleShip::lockShip(bool isLock)
{
	bool IsVisible_1 = m_pEnable_touch->isVisible();
	bool IsVisible_2 = m_pDisable_touch->isVisible();
	
	if (isLock == (IsVisible_1 || IsVisible_2))
	{
		return;
	}
	if (!isLock)
	{
		m_pEnable_touch->setVisible(false);
		m_pDisable_touch->setVisible(false);
		m_pDelegate->getShareObj()->m_nLockShipTag = -1;
		m_pDelegate->specialButtonEvent(this,"event_unlockTarget");
		addLogAction(ACT_UNLOCK);
	}else
	{
		m_pEnable_touch->setVisible(false);
		m_pDisable_touch->setVisible(true);
		m_pDelegate->getShareObj()->m_nLockShipTag = m_Info->_tag;
		m_pDelegate->specialButtonEvent(this,"event_lockTarget");
		addLogAction(ACT_LOCK);
	}
}

void TVBattleShip::checkSelected()
{
	if(m_Info->_isFriend)
	{
		return;
	}
	bool IsVisible_1 = m_pEnable_touch->isVisible();
	bool IsVisible_2 = m_pDisable_touch->isVisible();
	if (m_pDelegate->getShareObj()->m_vMyFriends.size() < 1)
	{
		return;
	}

	auto mainShip = m_pDelegate->getShareObj()->m_vMyFriends[0];
	if (mainShip->getType() != MAIN_SHIP)
	{
		m_pEnable_touch->setVisible(false);
		m_pDisable_touch->setVisible(false);
		return;
	}
	int curLanunchDis = mainShip->getCurLanunchDistance();
	if (IsVisible_1 || IsVisible_2)
	{
		auto shareObj = m_pDelegate->getShareObj();
		m_pEnable_touch->setPosition(_position);
		m_pDisable_touch->setPosition(_position);
		if (shareObj->m_vMyFriends.size() > 0)
		{
			auto mainShip = dynamic_cast<TVBasicShip*>(shareObj->m_vMyFriends[0]);
			if (mainShip->getType() == MAIN_SHIP)
			{
				float distanse = _position.distance(mainShip->getPosition());
				if (distanse < curLanunchDis && IsVisible_2)
				{
					m_pEnable_touch->setVisible(true);
					m_pDisable_touch->setVisible(false);
				}else if(distanse > curLanunchDis && IsVisible_1)
				{
					m_pEnable_touch->setVisible(false);
					m_pDisable_touch->setVisible(true);
				}
			}
		}
	}
}

void TVBattleShip::npcMove()
{
	this->setLocalZOrder(MAX_Z_ORDER - _position.y);

	Size winsize = Director::getInstance()->getWinSize();
	Vec2 ship_pos = _position;
//	log("current speed %f", m_fCurSpeed);
	float tmpy = m_fCurSpeed*m_speedFactor * sin(CC_DEGREES_TO_RADIANS(m_currentDirect));
	float tmpx = m_fCurSpeed*m_speedFactor * cos(CC_DEGREES_TO_RADIANS(m_currentDirect));
	float nDistance = 0.0f;

	//直线修正
	if(!m_bIsTurning && m_vRoad.size()>0){
		Vec2 tmpDiff = m_vRoad[0].pt2 - m_vRoad[0].pt1;
		nDistance = m_vRoad[0].pt2.distance(ship_pos);
		if(std::abs(tmpDiff.x) > 0.1f && std::abs(tmpDiff.y) > 0.1f && tmpy > 0.1f){ //极小值时应该避免。
			float sign = 1.0f;
			if(tmpx < 0){
				sign = -1.0f;
			}
			tmpx = sign * std::abs(tmpy/tmpDiff.y * tmpDiff.x);
		}

	}
	ship_pos.y += tmpy;
	ship_pos.x += tmpx;
//	log("battleship tag:%d, tmp y:%f, x:%f m_speedFactor: %f m_currentDirect %f m_fCurSpeed %f distance:%f, %s", this->getTag(), tmpy, tmpx, m_speedFactor, m_currentDirect, m_fCurSpeed, nDistance, m_bIsTurning?"Turning" : "Direct");
	//log("m_speedFactor:%f", m_speedFactor);

	if (ship_pos.x < 0.1 || ship_pos.y < 0.1 || ship_pos.x > (winsize.width * 3 -1) || ship_pos.y >  (winsize.height *3 -1))
	{
		//log("npcMove x:y-%f:%f",ship_pos.x,ship_pos.y);
		m_fCurSpeed = 1;
		Vec2 tempPos = m_vRoad[0].pt2 - ship_pos;
		float degrees = CC_RADIANS_TO_DEGREES(atan2f(tempPos.y,tempPos.x));
		m_pShipIcon->setRotation(-degrees);
		m_currentDirect = degrees;
		m_vRoad.clear();
		return;
	}

	Vec2 diff = m_vRoad[0].pt2 - ship_pos;
	if (pow(diff.x,2) + pow(diff.y,2) <= 2 * pow(m_fCurSpeed,2))
	{
		ship_pos = m_vRoad[0].pt2;
		m_vRoad.erase(m_vRoad.begin());

		if (m_vRoad.size() < 1)
		{
			m_fCurSpeed = 1;
		}else
		{
			Vec2 tempPos = m_vRoad[0].pt2 - ship_pos;
//			float degrees = CC_RADIANS_TO_DEGREES(atan2f(tempPos.y,tempPos.x));
			float degrees = CC_RADIANS_TO_DEGREES(tempPos.getAngle());
			if(degrees < 0){
				degrees = degrees + 360;
			}
			m_pShipIcon->setRotation(-degrees);
			m_currentDirect = degrees;
		}
	}
	this->setPosition(ship_pos);
}

/*
 * 负责处理转向。
 */
void TVBattleShip::npcRotation()
{
	float turnSpeed = m_fRoateAngle;
//	log("turnSpeed %f", turnSpeed);
	if (m_NextRotation != 0)
	{
		m_bIsTurning = true;
		normalDriect();
		if (std::abs(m_NextRotation) <= turnSpeed)
		{
			m_currentDirect += m_NextRotation;
			
			m_NextRotation = 0;
			m_pShipIcon->setRotation(-m_currentDirect);
			return;
		}
		if (m_NextRotation > 0)
		{
			m_NextRotation -= turnSpeed;
			m_currentDirect += turnSpeed;
			m_pShipIcon->setRotation(-m_currentDirect);
		}else
		{
			m_NextRotation += turnSpeed;
			m_currentDirect -= turnSpeed;
			m_pShipIcon->setRotation(-m_currentDirect);
		}
	}else{
		m_bIsTurning = false;
	}
}

void TVBattleShip::findTargetAgain()
{
	m_TargetPos = _position;
}

void TVBattleShip::addLogAction(int type,float arg_1 /* = 0 */,int arg_2 /* = 0 */,Vec2 pos /* = Vec2 */)
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
float BattleShip::getShipMoveSpeed()
{
	if (m_bIsMoveEnd)
	{
		return 0;
	}
	return m_fMoveSpeed;
}
*/

void TVBattleShip::setChargeInitiative(bool isCharge)
{
	m_bIsChargeInitiative = isCharge;
}
