#include "UINationWarShip.h"

#include "WarManage.h"
#include "UINationWarMap.h"
#include "EffectManage.h"
#include "UINationWarHUD.h"

#define filling_bg  "cocosstudio/login_ui/sailing_720/battle_filling_bg.png"
#define filling_connon  "cocosstudio/login_ui/sailing_720/battle_filling.png"

UINationWarShip::UINationWarShip()
{
	m_eUIType = UI_COUNTRY_WAR;
	m_pShipIcon = nullptr;
	m_pParent = nullptr;
	m_nRotation = 0;
	m_pShipNode = nullptr;
	m_eShipType = NO_SHIP_FLEET;
	m_nPositon = 0;
	m_pShipFleetInfo = nullptr;
	m_pConnonCD = nullptr;
	m_pName = nullptr;
	m_pNation = nullptr;
	m_bIsShowHurt = false;
	m_nDirection = 0;
}

UINationWarShip::~UINationWarShip()
{

}

void UINationWarShip::onEnter()
{
	Node::onEnter();
}

void UINationWarShip::onExit()
{
	Node::onExit();
}

UINationWarShip* UINationWarShip::createWarShip(UIBasicLayer* parent, int position, SHIP_FLEET_TYPE ship_type, int fleetIndex)
{
	UINationWarShip* ws = new UINationWarShip;
	if (ws && ws->init())
	{
		ws->m_pParent = parent;
		ws->m_nPositon = position;
		ws->m_eShipType = ship_type;
		ws->m_nFleetIndex = fleetIndex;
		ws->autorelease();
		return ws;
	}
	CC_SAFE_DELETE(ws);
	return nullptr;
}

bool UINationWarShip::init()
{
	bool pRet = false;
	do
	{
		m_pShipIcon = ImageView::create();
		this->addChild(m_pShipIcon);
		m_pShipIcon->setName("image_ship_icon");
		m_pShipIcon->setAnchorPoint(Vec2(0.5f, 0.5f));
		pRet = true;
	} while (0);

	return pRet;
}

void UINationWarShip::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* widget = dynamic_cast<Widget*>(pSender);
		std::string name = widget->getName();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	
		if (isButton(image_ship_icon) || isButton(label_play))
		{
			ProtocolThread::GetInstance()->getUserInfoById(m_pShipFleetInfo->cid/*, LoadingLayer::create(this)*/);
			return;
		}

	}
}

void UINationWarShip::updateBySecond(const float fTime)
{
	m_bIsShowHurt = false;
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(fTime), CallFunc::create(CC_CALLBACK_0(UINationWarShip::fired, this))));
}

void UINationWarShip::fired()
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FIRE_CANNON_07);
	m_pConnonCD->setVisible(false);

	std::vector<SHIP_POSITION> v_ship_position;
	auto model_id = SINGLE_SHOP->getShipData().find(m_pShipFleetInfo->ships[0]->ship_id)->second.model_id;
	auto& shipPosition = SINGLE_SHOP->getShipWarPositionInfo();
	std::map <int, SHIP_WAR_POSITION>::iterator m1_Iter;
	for (m1_Iter = shipPosition.begin(); m1_Iter != shipPosition.end(); m1_Iter++){
		auto& item = m1_Iter->second;
		if (item.model_id == model_id && item.direction == m_nDirection)
		{
			v_ship_position = item.position;
		}
	}
	int nFiredNum = 0;
	for (size_t i = 0; i < m_pShipFleetInfo->n_ships; i++)
	{
		if ((m_eShipType == GHOST_SHIP_FLEET && m_pShipFleetInfo->ships[i]->hp + m_pShipFleetInfo->ships[i]->ship_hurt  > 0)
			|| (m_eShipType == MY_SHIP_FLEET && m_pShipFleetInfo->ships[i]->hp > 0))
		{
			nFiredNum++;
			int p = nFiredNum - 1;
			
			if (p > 2)
			{
				p -= 3;
			}
			int rx = v_ship_position.at(p).x - m_pShipIcon->getContentSize().width / 2;
			int ry = m_pShipIcon->getContentSize().height / 2 - v_ship_position.at(p).y;
		
			//船只开火
			auto animation = dynamic_cast<WarManage*>(m_pParent)->getEffectManage()->getAnimation(PT_FIRE_EFFECT);
			auto animateEff = Animate::create(animation->clone());
			auto eff = Sprite::create();
			eff->runAction(Sequence::create(DelayTime::create(0.7f * (nFiredNum - 1)), animateEff, RemoveSelf::create(true), nullptr));
			eff->setPosition(rx, ry);
			m_pShipNode->addChild(eff);
			eff->setScale(0.2f);
			eff->setAnchorPoint(Vec2(0.25, 0.5));
			eff->setRotation(m_nRotation);

			this->runAction(Sequence::create(DelayTime::create(0.7f * nFiredNum), CallFunc::create(CC_CALLBACK_0(UINationWarShip::hurt, this, 0.7f)), nullptr));
		}
	}

	if (nFiredNum == 1)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FIRE_CANNON_07);
	}
	else
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FIRE_CANNON_08);
	}
}

void UINationWarShip::hurt(const float fTime)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CANNON_HIT_29);
	auto warMap = dynamic_cast<WarManage*>(m_pParent)->getWarMapLayer();
	auto b_city = warMap->getCityImage();

	int rx = cocos2d::random(int(b_city->getContentSize().width / 2 - 30), int(b_city->getContentSize().width / 2 + 30));
	int ry = cocos2d::random(int(b_city->getContentSize().width / 2 - 30), int(b_city->getContentSize().height / 2 + 30));
	auto animation_explosion = dynamic_cast<WarManage*>(m_pParent)->getEffectManage()->getAnimation(PT_EXPLOSION);
	auto animateEff = Animate::create(animation_explosion->clone());
	auto eff = Sprite::create();
	b_city->addChild(eff);
	eff->setScale(0.5f);
	eff->setAnchorPoint(Vec2(0.5, 0.5));
	eff->setPosition(Vec2(rx, ry));
	eff->runAction(Sequence::createWithTwoActions(animateEff, RemoveSelf::create(true)));

	if (!m_bIsShowHurt)
	{
		m_bIsShowHurt = true;
		int height = 60;
		int attack = 0;
		int n_ship = 0;
		for (size_t i = 0; i < m_pShipFleetInfo->n_ships; i++)
		{
			if ((m_eShipType == GHOST_SHIP_FLEET && m_pShipFleetInfo->ships[i]->hp + m_pShipFleetInfo->ships[i]->ship_hurt  > 0)
				|| (m_eShipType == MY_SHIP_FLEET && m_pShipFleetInfo->ships[i]->hp > 0))
			{
				attack += m_pShipFleetInfo->ships[i]->city_hurt;
				n_ship++;
			}
		}
		const char* formatString = attack > 0 ? "/%d" : "%d";
		std::string s_capNums = StringUtils::format(formatString, attack);
		TextAtlas *capNums = TextAtlas::create(s_capNums, IMAGE_FONT[1], 20, 26, "/");

		capNums->setPosition(Vec2(b_city->getContentSize().width / 2, b_city->getContentSize().height * 0.4f));
		b_city->addChild(capNums, 50);
		auto seq_1 = Sequence::create(DelayTime::create(1.8f), FadeOut::create(0.3f), nullptr);
		auto easeAct = EaseQuinticActionOut::create(Spawn::createWithTwoActions(MoveBy::create(1.5f, Vec2(0, height)), seq_1));
		float time = (n_ship - 2) * 0.7;
		if (time < 0)
		{
			time = 0;
		}
		auto seq = Sequence::create(DelayTime::create(time), FadeIn::create(0.5f), easeAct, RemoveSelf::create(true), nullptr);
		capNums->runAction(seq);
		capNums->setOpacity(0);

		auto warUI = dynamic_cast<WarManage*>(m_pParent)->getWarUI();
		warUI->shipAttackCity(attack);
		m_pConnonCD->setVisible(true);
	}
}

ImageView* UINationWarShip::getShipIcon()
{
	return m_pShipIcon;
}

void UINationWarShip::setShipIcon(int shipId, int rotation)
{
	switch (rotation)
	{
	case 1:
		m_nRotation = 0;
		break;
	case 2:
		m_nRotation = 45;
		break;
	case 3:
		m_nRotation = 90;
		break;
	case 4:
		m_nRotation = 135;
		break;
	case 5:
		m_nRotation = 180;
		break;
	case 6:
		m_nRotation = -135;
		break;
	case 7:
		m_nRotation = -90;
		break;
	case 8:
		m_nRotation = -45;
		break;
	default:
		m_nRotation = 0;
		break;
	}
	
	m_nDirection = rotation;
	m_pShipIcon->loadTexture(getShipSkewIconPath(shipId, rotation));
	
	m_pShipNode = Node::create();
	m_pShipNode->setPosition(Vec2(m_pShipIcon->getContentSize().width * 0.5f, m_pShipIcon->getContentSize().height * 0.5f));
	m_pShipIcon->addChild(m_pShipNode);

	auto layer = dynamic_cast<WarManage*>(m_pParent)->getLayer();

	m_pName = Text::create();
	std::string name = m_pShipFleetInfo->heroname;
	if (m_pShipFleetInfo->cid < 0)
	{
		name = SINGLE_SHOP->getBattleNpcInfo().find(atoi(name.c_str()))->second.name;
	}
	m_pName->setString(name);
	m_pName->setFontSize(24);
	m_pName->setTextColor(Color4B::WHITE);
	m_pName->enableOutline(Color4B::BLACK, 2);
	m_pName->setAnchorPoint(Vec2(1, 1));
	m_pName->setPosition(Vec2(this->getPositionX() + 20 + m_pName->getContentSize().width / 2, this->getPositionY() + m_pShipIcon->getContentSize().height * 0.5));
	m_pNation = ImageView::create(getCountryIconPath(m_pShipFleetInfo->nation));
	m_pNation->ignoreContentAdaptWithSize(false);
	m_pNation->setContentSize(Size(26, 28));
	m_pNation->setAnchorPoint(Vec2(1, 1));
	m_pNation->setPosition(Vec2(m_pName->getPositionX() - m_pName->getContentSize().width - 6, m_pName->getPositionY()));
	layer->addChild(m_pNation);
	layer->addChild(m_pName);

	m_pConnonCD = Widget::create();
	m_pConnonCD->setContentSize(Size(60, 60));
	m_pConnonCD->setPosition(Vec2(this->getPositionX(), m_pName->getPositionY() + 30));
	layer->addChild(m_pConnonCD);
	m_pConnonCD->setGlobalZOrder(99);

	auto i_filling = ImageView::create(filling_bg);
	i_filling->setPosition(Vec2(30, 30));
	m_pConnonCD->addChild(i_filling);
	i_filling->runAction(RepeatForever::create(RotateBy::create(2, 180)));

	auto i_connon = ImageView::create(filling_connon);
	i_connon->setPosition(Vec2(30, 30));
	m_pConnonCD->addChild(i_connon);
	
	if (m_eShipType != MY_SHIP_FLEET)
	{
		m_pShipIcon->setOpacity(0);
		m_pShipIcon->runAction(FadeIn::create(0.8f));
		//m_pShipIcon->setTouchEnabled(true);
		//m_pShipIcon->addTouchEventListener(CC_CALLBACK_2(WarShip::menuCall_func, this));
		
		m_pName->setName("label_play");
		m_pName->addTouchEventListener(CC_CALLBACK_2(UINationWarShip::menuCall_func, this));
		m_pConnonCD->setVisible(true);
		if (m_pShipFleetInfo->cid < 0)
		{
			m_pName->setTouchEnabled(false);
		}
		else
		{
			m_pName->setTouchEnabled(true);
		}
	}
	else
	{
		//m_pShipIcon->setTouchEnabled(false);
		m_pName->setTouchEnabled(false);
		m_pConnonCD->setVisible(false);
	}
}

int UINationWarShip::getShipRotation()
{
	return m_nRotation;
}

SHIP_FLEET_TYPE UINationWarShip::getShipFleetType()
{
	return m_eShipType;
}

int UINationWarShip::getShipPosition()
{
	return m_nPositon;
}

void UINationWarShip::setShipFleetInfo(NationWarCharacterStatus *shipFleetInfo)
{
	m_pShipFleetInfo = shipFleetInfo;
}

NationWarCharacterStatus *UINationWarShip::getShipFleetInfo()
{
	return m_pShipFleetInfo;
}

void UINationWarShip::countryWarStop()
{
	this->stopAllActions();
	m_pConnonCD->setVisible(false);
}

void UINationWarShip::shipDied()
{
	auto &vWarShip = dynamic_cast<WarManage*>(m_pParent)->m_vWarShip;
	vWarShip.erase(vWarShip.begin() + m_nFleetIndex);
	
	for (size_t i = 0; i < vWarShip.size(); i++)
	{
		auto warShip = dynamic_cast<WarManage*>(m_pParent)->getWarShip(i);
		warShip->setShipFleetIndex(i);
	}
	this->stopAllActions();
	
	//船只死亡动画
	m_pShipIcon->runAction(Sequence::createWithTwoActions(Blink::create(0.8f, 2), FadeIn::create(0.8f)));
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(1.6f), RemoveSelf::create(true)));
	m_pConnonCD->removeFromParentAndCleanup(true);
	m_pName->runAction(Sequence::createWithTwoActions(DelayTime::create(1.6f), RemoveSelf::create(true)));
	m_pNation->runAction(Sequence::createWithTwoActions(DelayTime::create(1.6f), RemoveSelf::create(true)));
}

void UINationWarShip::setShipFleetIndex(int index)
{
	m_nFleetIndex = index;
}

int UINationWarShip::getShipFleetIndex()
{
	return m_nFleetIndex;
}

void UINationWarShip::setShipConnonCD(const bool isVisible)
{
	m_pConnonCD->setVisible(isVisible);
}