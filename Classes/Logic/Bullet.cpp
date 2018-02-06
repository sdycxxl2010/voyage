#include "Bullet.h"
#include "SystemVar.h"
#include "TVBattleShareObj.h"
#include "TVBasicShip.h"
#include "EffectManage.h"
#include "TVSingle.h"
#include "Utils.h"
#include "ParticleManager.h"

Bullet::Bullet():
	m_pIcon(nullptr),
	m_pParent(nullptr),
	m_pOwnInfo(nullptr),
    m_pPsq(nullptr)
{
	m_pOwnInfo = new Ship_info;
	m_pOwnInfo->_attack = 0;
	m_pOwnInfo->_activeCritSize = 0;
	m_pOwnInfo->_isSlowSpeedTime = 0;
	m_pOwnInfo->_isFireEfficeharm = 0;
	m_pOwnInfo->_defense = 0;
	m_pOwnInfo->_change_attack = 0;
	m_pOwnInfo->_change_defense = 0;
	m_pOwnInfo->_isFriend = 0;
	m_pOwnInfo->_sailornum = 0;
	m_pOwnInfo->_captureNum = 0;
	m_pOwnInfo->_sid = 0;
	m_pOwnInfo->_type = 0;
	m_pOwnInfo->_tag = 0;
	m_pOwnInfo->_extAttack = 0;
	m_pOwnInfo->_critSize = 0;
	m_pOwnInfo->_attack_accuracy = 0;
	m_nTargetSpeed = 0;

	m_nAttack = 0;
	m_pDelegate = nullptr;
	m_fOffsetDirect = 0;
	m_fMoveTime=0;
	m_cannonid = 0;
}

Bullet::~Bullet()
{
	delete m_pOwnInfo;
	//CC_SAFE_RELEASE(this);
}
											     
Bullet* Bullet::createBullet(Node* parent, int id, int iid)
{
	Bullet* but = new Bullet;
	if (but && but->init())
	{
		but->m_pParent = parent;
		but->retain();
		but->autorelease();
		but->m_cannonid = iid;
		return but;
	}
	CC_SAFE_DELETE(but);
	return nullptr;
}

bool Bullet::init()
{
	this->scheduleUpdate();
	return true;
}

void Bullet::onEnter()
{
	Node::onEnter();
	if (m_pOwnInfo)
	{
		auto direct = m_DesPos - _position;
//		float angle = atan2f(direct.y,direct.x);
//		auto degrees = int(CC_RADIANS_TO_DEGREES(angle) + 360.0) % 360;

		m_fMoveTime = direct.distance(Vec2(0,0)) / 200.0f;
		//根据技能计算是否要进行精准射击
		if(m_pOwnInfo->_attack_accuracy > cocos2d::random(0,999)){
			log("_attack_accuracy hit:%d",m_pOwnInfo->_attack_accuracy);

			float offset = m_nTargetSpeed * m_fMoveTime * 60;
			log("attack offset %f, speed %f", offset, m_nTargetSpeed);
			m_DesPos = m_DesPos + Vec2(cos(CC_DEGREES_TO_RADIANS(m_fOffsetDirect)),sin(CC_DEGREES_TO_RADIANS(m_fOffsetDirect))) * offset;
		}else{
			log("_attack_accuracy not hit:%d",m_pOwnInfo->_attack_accuracy);
		}
		
		//炮弹纹理图
		m_pIcon = Sprite::create();
		m_pIcon->setTexture("ship/bullet2.png");
		m_pIcon->setScale(0.58f);
		m_pIcon->setAnchorPoint(Vec2(0.5f,0.5f));
		this->addChild(m_pIcon);
		m_pIcon->setGlobalZOrder(1000);

		//炮弹的烟的粒子效果
		//auto part2_frame=SpriteFrameCache::getInstance()->getSpriteFrameByName("toonSmoke.png");
		ValueMap vm = ParticleManager::getInstance()->GetPlistData("bulletsmoke");

		if(vm.size()<1){
			m_pPsq = ParticleSystemQuad::create("bulletsmoke_1.plist");
		}else{
			m_pPsq = ParticleSystemQuad::create(vm);
		}
		m_pPsq->setAnchorPoint(Vec2(0.5f,0.5f));
		m_pPsq->setPositionType(ParticleSystem::PositionType::FREE);
		m_pPsq->setLife(m_fMoveTime + 0.3);
		m_pPsq->setDuration(m_fMoveTime);
		m_pParent->getParent()->addChild(m_pPsq);
		m_pPsq->setGlobalZOrder(999);

		auto seq = Sequence::create(MoveTo::create(m_fMoveTime,m_DesPos),CCCallFunc::create(CC_CALLBACK_0(Bullet::sendFinsh,this)),nullptr);
		this->runAction(seq);
	}
}

void Bullet::onExit()
{
	Node::onExit();
}

void Bullet::update(float delta)
{
	m_pPsq->setPosition(m_pParent->getParent()->convertToNodeSpace(convertToWorldSpaceAR(m_pIcon->getPosition())));
}


void Bullet::show()
{
	if (m_pParent)
	{
		m_pParent->addChild(this,MAX_Z_ORDER);
	}
}

void Bullet::setIconByTag(int id)
{
	if (m_pIcon)
	{
		m_pIcon->setTexture("ship/bullet.png");
	}
}

void Bullet::setBulletInfo(Ship_info* info)
{
	if(m_pOwnInfo) delete m_pOwnInfo;
	m_pOwnInfo = new Ship_info;
	m_pOwnInfo->_attack = info->_attack;
	m_pOwnInfo->_change_attack = info->_change_attack;
	m_pOwnInfo->_change_defense = info->_change_defense;
	m_pOwnInfo->_activeCritSize = info->_activeCritSize;
	m_pOwnInfo->_isSlowSpeedTime = info->_isSlowSpeedTime;
	m_pOwnInfo->_isFireEfficeharm = info->_isFireEfficeharm;
	m_pOwnInfo->_defense = info->_defense;
	m_pOwnInfo->_isFriend = info->_isFriend;
	m_pOwnInfo->_sailornum = info->_sailornum;
	m_pOwnInfo->_sid = info->_sid;
	m_pOwnInfo->_type = info->_type;
	m_pOwnInfo->_tag = info->_tag;
	m_pOwnInfo->_extAttack = info->_extAttack;
	m_pOwnInfo->_critSize = info->_critSize;
	m_pOwnInfo->_attack_accuracy = info->_attack_accuracy;

	//m_nAttack = info->_attack;
}

void Bullet::setAttack(int attack){
	m_nAttack = attack;
}

void Bullet::sendFinsh()
{
	TVBasicShip *hurtShip;
	bool isHire = false;
	auto shareObj = m_pDelegate->getShareObj();
	Vec2 curPos = _parent->convertToWorldSpace(_position);
	if (m_pOwnInfo->_isFriend)
	{
		for (auto item = shareObj->m_vEnemys.begin(); item != shareObj->m_vEnemys.end(); item++)
		{
			auto bs = (*item);
			hurtShip = bs;
			if (bs->hitTest(curPos))
			{
				if (!(bs->isDied()))
				{
					bs->hurt(this);
					isHire = true;
					break;
				}
			}
		}
	}else
	{
		for (auto item = shareObj->m_vMyFriends.begin(); item != shareObj->m_vMyFriends.end(); item++)
		{
			auto bs = (*item);
			hurtShip = bs;
			if (bs->hitTest(curPos))
			{
				if (!(bs->isDied()))
				{
					bs->hurt(this);
					isHire = true;
					break;
				}
			}
		}
	}

	this->stopAllActions();
	this->unscheduleUpdate();
	m_pIcon->removeFromParent();

	//炮弹落点粒子效果
	auto eff = Sprite::create();
	PARTICLE_TYPE eAnimationIndex = PT_EXPLOSION;
	if (!isHire)
	{
		eff->setScale(0.2f);
		eAnimationIndex = PT_DROP;
		this->addChild(eff);
	}else
	{
		eff->setScale(1.0f);
		eAnimationIndex = PT_EXPLOSION;
		hurtShip->addChild(eff);
	}
	auto animation = m_pDelegate->getEffective()->getAnimation(eAnimationIndex);
	eff->runAction(Sequence::create(Animate::create(animation->clone()), CCCallFuncN::create(CC_CALLBACK_1(Bullet::removeEffective, this)), RemoveSelf::create(true), nullptr));
}

void Bullet::removeEffective(Node* target)
{
	this->stopAllActions();
	this->removeFromParent();
}

void Bullet::setOffsetDirect(float dircent)
{
	m_fOffsetDirect = dircent; 
}

void Bullet::setTargetSpeed(float speed)
{
	m_nTargetSpeed = speed; 
}

void Bullet::setDestination(Vec2 des) 
{
	m_DesPos = des;
}

Vec2 Bullet::getDestination()
{
	return m_DesPos;
}

int Bullet::getCannonid()
{
	return  m_cannonid;
}
void Bullet::setDelegate(TVBattleDelegate *pDelegate) 
{
	m_pDelegate = pDelegate; 
}

int	Bullet::getAttack() 
{
	return m_nAttack;
}

Ship_info* Bullet::getBulletInfo() 
{
	return m_pOwnInfo;
}
