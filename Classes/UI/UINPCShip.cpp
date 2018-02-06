#include "UINPCShip.h"
#include "TVSingle.h"

UINPCShip::UINPCShip():
i_id(0),
i_country(0),
i_type(0),
i_direction(NPCSHIP_DIRECT::RIGHT),
m_country(nullptr),
m_shipTexture(nullptr),
w_topWidget(nullptr),
t_name(nullptr),
t_level(nullptr),
s_shipTexture(""),
m_guardCircle(nullptr),
m_attackCircle(nullptr),
m_isFriendImage(nullptr),
m_isFridend(false),
m_isActiveAttack(false),
m_isScale(false),
i_eventId(0),
i_npcType(0),
n(0)
{
	setTouchEnabled(true);
	supply_2day = nullptr;
	m_eventData = nullptr;
}

UINPCShip::~UINPCShip(){

}

UINPCShip* UINPCShip::create(){

	UINPCShip* npcShip = new UINPCShip();

	do
	{
		CC_BREAK_IF(!npcShip);
		CC_BREAK_IF(!npcShip->init());

		npcShip->autorelease();
		return npcShip;

	} while (false);

	CC_SAFE_DELETE(npcShip);
	return nullptr;
}

bool UINPCShip::init(){

	bool pRet = false;

	do
	{

		CC_BREAK_IF(!Widget::init());

		pRet = true;
	} while (false);

	return pRet;
}

void UINPCShip::setCameraMask(unsigned short mask, bool applyChildren)
{
	Widget::setCameraMask(mask,applyChildren);
}

void UINPCShip::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	Widget::visit(renderer,parentTransform,parentFlags);
}

UINPCShip* UINPCShip::create(NPCShipInfo npcShipInfo){

	UINPCShip* npcShip = new UINPCShip();

	do
	{
		CC_BREAK_IF(!npcShip);
		CC_BREAK_IF(!npcShip->init(npcShipInfo));

		npcShip->autorelease();
		return npcShip;

	} while (false);

	CC_SAFE_DELETE(npcShip);
	return nullptr;
}

bool UINPCShip::init(NPCShipInfo npcShipInfo){

	bool pRet = false;

	do
	{

		CC_BREAK_IF(!Widget::init());

		CC_BREAK_IF(!flushInit(npcShipInfo));

		pRet = true;
	} while (false);

	return pRet;
}

bool UINPCShip::flushInit(NPCShipInfo npcShipInfo){

	bool pRet = false;

	do
	{
		m_shipTexture = ImageView::create(npcShipInfo.shipTexture);
		addChild(m_shipTexture);
		m_shipTexture->setLocalZOrder(10);
		setContentSize(m_shipTexture->getContentSize());
		m_shipTexture->setPosition(Vec2(getContentSize().width / 2, getContentSize().height / 2));
		w_topWidget = Widget::create(); 
		w_topWidget->setScale(0.5f);
		w_topWidget->setPosition(Vec2(m_shipTexture->getPositionX(), m_shipTexture->getSize().height + TopWidgetOffset));
		addChild(w_topWidget);
		i_eventId = (npcShipInfo.i_eventId);
		m_guardCircle = ImageView::create(NPC_SHIPICON[1]);
		m_guardCircle->setVisible(false);
		m_guardCircle->setPosition(m_shipTexture->getPosition());
		addChild(m_guardCircle);
		i_id = npcShipInfo.i_id;
		i_country=npcShipInfo.i_country;
		if (npcShipInfo.i_level <= (SINGLE_HERO->m_iLevel)){

			n = 1;
		}
		if ((npcShipInfo.i_level > SINGLE_HERO->m_iLevel) && (npcShipInfo.i_level < (SINGLE_HERO->m_iLevel + 10))){

			n = 2;
		}
		if (npcShipInfo.i_level >= (SINGLE_HERO->m_iLevel + 10)){

			n = 3;
		}

		int npc_type = SINGLE_SHOP->getBattleNpcInfo()[npcShipInfo.i_id].type;
		i_npcType = npc_type;
		if (npcShipInfo.IsNeutralChange)
		{
			npcShipInfo.b_isFridend = 0;
		}
		if (npcShipInfo.b_isFridend || npc_type == NPC_TYPE_CARAVAN)
		{
				npcShipInfo.b_isActiveAttack = false;

		}else
		{
			if (npcShipInfo.i_level >= (SINGLE_HERO->m_iLevel - 10)){
				npcShipInfo.b_isActiveAttack = true;
			}else if ((npcShipInfo.i_level <= (SINGLE_HERO->m_iLevel-10)) && (npcShipInfo.i_level >= (SINGLE_HERO->m_iLevel - 20))){
				int rate = 100 - (SINGLE_HERO->m_iLevel-npcShipInfo.i_level-10)*10;
				int randomValue = cocos2d::random(0,100);
				if (randomValue < rate)
				{
					npcShipInfo.b_isActiveAttack = true;
				}else
				{
					npcShipInfo.b_isActiveAttack = false;
				}
			}else if (npcShipInfo.i_level <= (SINGLE_HERO->m_iLevel - 20)){
				npcShipInfo.b_isActiveAttack = false;
			}
		}
		//npcShipInfo.b_isActiveAttack = true;
		log("attack png:%s",NPC_SHIPICON[n + 1].c_str());
		m_attackCircle = ImageView::create(NPC_SHIPICON[n + 1]);
		m_attackCircle->setVisible(false);
		m_attackCircle->setPosition(m_shipTexture->getPosition());
		addChild(m_attackCircle);
		
		std::string type_index = StringUtils::format("TIP_SAILING_NPC_TYPE%d", npc_type);
		std::string type_name = SINGLE_SHOP->getTipsInfo()[type_index];
		t_name = Text::create(type_name,"" , 20);
		w_topWidget->addChild(t_name);

		m_country = ImageView::create();

		m_country->loadTexture(getCountryIconPath(npcShipInfo.i_country));
		m_country->setPositionX(-(t_name->getSize().width / 2) - 5 * TopWidgetOffset);
		w_topWidget->addChild(m_country);

		m_isActiveAttack = npcShipInfo.b_isActiveAttack;
		m_isFridend = npcShipInfo.b_isFridend;
		m_level = npcShipInfo.i_level;
		std::string s_level = StringUtils::format("Lv. %d", npcShipInfo.i_level);
		t_level = Text::create(s_level,"", 20);
		t_level->setPositionX(t_name->getSize().width / 2 + 5 * TopWidgetOffset);
		w_topWidget->addChild(t_level);

		if (npcShipInfo.b_isActiveAttack){
			t_name->setTextColor(Color4B::RED);
			t_level->setTextColor(Color4B::RED);
		}
		else{
			t_name->setTextColor(Color4B::WHITE);
			t_level->setTextColor(Color4B::WHITE);
		}

		supply_2day = ImageView::create("ship/supply_2day.png");
		supply_2day->setPosition(Vec2(m_shipTexture->getPositionX(), m_guardCircle->getContentSize().height * 4 / 10));
		supply_2day->setVisible(false);
		supply_2day->setScale(1.0);
		addChild(supply_2day);

		if (npcShipInfo.b_isTaskNpc)
		{ 
			auto taskFlag = ImageView::create(getTaskType(1));
			taskFlag->setContentSize(Size(42,42));
			taskFlag->setPositionX(((t_level->getSize().width) + 8 * TopWidgetOffset));
			w_topWidget->addChild(taskFlag);
		}

		pRet = true;
	} while (false);

	return pRet;

}

void UINPCShip::isShowGuardCircle(bool isTure, float f){

	m_guardCircle->setVisible(isTure);
	m_attackCircle->setVisible(isTure);
	supply_2day->setVisible(isTure);

	if (isTure){

		if (!m_isScale){
		
//			m_guardCircle->setScale(m_guardCircle->getScale() + f / 10);
			supply_2day->setScale(supply_2day->getScale() + 0.05);

			if (supply_2day->getScale() >= 1.2){
				m_isScale = true;
			}

		}
		else{
		
//			m_guardCircle->setScale(m_guardCircle->getScale() - f / 10);
			supply_2day->setScale(supply_2day->getScale() - 0.05);

			if (supply_2day->getScale() <= 1.0){
				m_isScale = false;

			}
		
		}

	}else{
		supply_2day->setScale(1.0);
	}

}

bool UINPCShip::AttackCirclehitTest(const Vec2 &pt)
{
	ImageView* attackCircle = getAttackCircle();
	Vec2 nsp = attackCircle->convertToNodeSpaceAR(pt);
	Rect bb;

	//bb.size = m_pShipIcon->getContentSize()/2;
	bb.size = attackCircle->getSize();
	bb.origin = Vec2(-bb.size.width / 2, -bb.size.height / 2);
	if (bb.containsPoint(nsp))
	{
		return true;
	}
	return false;
}

int UINPCShip::getEventID(){

	return i_eventId;

}
