#include "UIShip.h"
#include <math.h>
#include "UISailManage.h"
#include "ProtocolThread.h"
#include "UISailHUD.h"
#include "UIShadowShip.h"

UIShadowShip::UIShadowShip(){
	m_shipImage = nullptr;
	m_isMoved = false;
	m_cid = 0;
	m_isHitLand = false;
	m_stopped = true;
	m_currentDir = DIR_NONE;
	m_lastRoadTIme = 0;
	m_playerInfo = nullptr;
	m_tname = nullptr;
	m_tlevel = nullptr;
	m_countryIcon = nullptr;
}

UIShadowShip::~UIShadowShip()
{
	while (!m_qSailRoads.empty())
	{
		m_qSailRoads.pop();
	};
	delete m_playerInfo;
}
bool UIShadowShip::init(NearByPlayerItem  * playerInfo)
{
	if (!Node::init())
	{
		return false;
	}
	m_playerInfo = new NearByPlayerItem;
	m_playerInfo->sid = playerInfo->sid;
	m_playerInfo->cid = playerInfo->cid;
	m_playerInfo->lv = playerInfo->lv;
	m_playerInfo->nation = playerInfo->nation;
	m_playerInfo->name = playerInfo->name;
	m_playerInfo->roadTime = playerInfo->roadTime;
	for (size_t i = 0; i < playerInfo->points.size(); i++)
	{
		m_playerInfo->points.push_back(playerInfo->points.at(i));
	}

	m_nShipId = m_playerInfo->sid;
	loadShip();
	this->m_lastRoadTIme = playerInfo->roadTime;
	this->setCameraMask(4, true);
	return true;
}

bool UIShadowShip::resetRoad(struct NearByPlayerItem*playerInfo){


	if(m_lastRoadTIme == playerInfo->roadTime){
		return false;
	}
	m_lastRoadTIme = playerInfo->roadTime;

	for (auto item : playerInfo->points)
	{
		m_qSailRoads.push(item);
	}
	Vec2 startPoint = m_qSailRoads.front().start;
	Vec2 endPoint = m_qSailRoads.front().end;
	int64_t time = m_qSailRoads.front().millisec;
	if(time == 0) time = 100;
	double sec = (double)time / 1000;
	startPoint.y = MAP_WIDTH * MAP_CELL_WIDTH - startPoint.y;
	endPoint.y = MAP_WIDTH * MAP_CELL_WIDTH - endPoint.y;

	if (!startPoint.equals(getPosition())){
		if(m_stopped){
			shipMove(startPoint,sec);
		}else{

		}
	}
	else{
		m_qSailRoads.pop();
		shipMove(endPoint,sec);
	}
	
	return true;
}
UIShadowShip * UIShadowShip::create(NearByPlayerItem * playerInfo)
{
	auto ship = new UIShadowShip();
	if (ship && ship->init(playerInfo))
	{
		ship->autorelease();
		return ship;
	}
	CC_SAFE_DELETE(ship);
	return nullptr;
}

void UIShadowShip::start(){
	Vec2 startPoint = m_qSailRoads.front().start;
	Vec2 endPoint = m_qSailRoads.front().end;
	int64_t time = m_qSailRoads.front().millisec;
	double sec = (double)time / 1000;

	startPoint.y = MAP_WIDTH * MAP_CELL_WIDTH - startPoint.y;
	endPoint.y = MAP_WIDTH * MAP_CELL_WIDTH - endPoint.y;
	this->setPosition(startPoint);
	m_qSailRoads.pop();
	this->shipMove(endPoint,sec);
}

void UIShadowShip::shipMove(Vec2 pos,double sec)
{
	m_stopped = true;
	this->stopAllActions();
	this->setOpacity(0);
	this->runAction(FadeIn::create(0.5));
	MoveTo * move = MoveTo::create(sec, pos);
	if(pos.distance(getPosition()) > 0.01){
		m_stopped = false;
		changeDirection(confirmShipDir(pos));
	}
	auto seq = Sequence::createWithTwoActions(move, CallFunc::create([=]{moveEnd();}));
	this->runAction(seq);
}

void UIShadowShip::changeDirection(SHIP_DIRECTION dir)
{
	if (dir == m_currentDir)
	{
		return;
	}
	switch (dir)
	{
	case UIShadowShip::UP:
		m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 5));
		break;
	case UIShadowShip::DOWN:
		m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 1));
		break;
	case UIShadowShip::LEFT:
		m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 3));
		break;
	case UIShadowShip::RIGHT:
		m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 7));
		break;
	case UIShadowShip::L_UP:
		m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 4));
		break;
	case UIShadowShip::L_DOWN:
		m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 2));
		break;
	case UIShadowShip::R_UP:
		m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 6));
		break;
	case UIShadowShip::R_DOWN:
		m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 8));
		break;
	default:
		break;
	}
	m_currentDir = dir;
}
void UIShadowShip::loadShip()
{
	m_tlevel = Text::create();
	m_tname = Text::create();
	m_countryIcon = ImageView::create();
	std::string namestr = m_playerInfo->name;
	std::string lvstr = StringUtils::format("Lv. %d",m_playerInfo->lv);
	std::string e_country = StringUtils::format("res/country_icon/flag_%d.png",m_playerInfo->nation);
	m_shipImage = Sprite::create();
	m_shipImage->setTexture(getShipSkewIconPath(m_nShipId, 7));
	m_tname->setString(namestr);
	m_tlevel->setString(lvstr);
	m_countryIcon->loadTexture(e_country);
	//m_shipImage->setScale(0.6);
	m_countryIcon->setScale(0.5);
	Rect rec = m_shipImage->getBoundingBox();
	m_countryIcon->setPosition(Vec2(-rec.size.width /2,rec.size.height/2));
	float x = m_countryIcon->getPositionX() + m_countryIcon->getBoundingBox().size.width/2 + m_tname->getBoundingBox().size.width/2 + 10;
	float y = m_countryIcon->getPositionY();
	m_tname->setPosition(Vec2(x,y));
	x = m_tname->getPositionX() + m_tname->getBoundingBox().size.width / 2 + m_tlevel->getBoundingBox().size.width / 2 + 10;
	y = m_tname->getPositionY();
	m_tlevel->setPosition(Vec2(x,y));
	addChild(m_shipImage);
	addChild(m_tname);
	addChild(m_tlevel);
	addChild(m_countryIcon);
	m_shipImage->setTag(m_playerInfo->cid);
	setTag(m_playerInfo->cid);
	m_cid = m_playerInfo->cid;
	setContentSize(m_shipImage->getBoundingBox().size);

	for (auto item : m_playerInfo->points)
	{
		m_qSailRoads.push(item);
	}
}
UIShadowShip::SHIP_DIRECTION UIShadowShip::confirmShipDir(Vec2 pos)
{
	Vec2 vec = pos - getPosition();
	float roat = vec.getAngle();
	float degree = CC_RADIANS_TO_DEGREES(roat);
	if ((degree< 0 &&degree >= -22.5) || (degree>0 && degree < 22.5)|| degree == 0 )
	{
		return RIGHT;
	}else if (degree >= 22.5 && degree < 67.5)
	{
		return R_UP;
	}else if (degree >= 67.5 && degree < 112.5)
	{
		return UP;
	}else if (degree >= 112.5 && degree < 157.5)
	{
		return L_UP;
	}else if (((degree >= 157.5 && degree<180)  || (degree > -180&& degree < -157.5))|| degree == 180 )
	{
		return LEFT;
	}else if (degree >= -157.5 && degree < -112.5)
	{
		return L_DOWN;
	}else if (degree >= -112.5 && degree < -67.5)
	{
		return DOWN;
	}
	else if (degree > -67.5 && degree < -22.5)
	{
		return R_DOWN;
	}
	return RIGHT;
}

void UIShadowShip::removeShip()
{
	auto seq = Sequence::createWithTwoActions(FadeOut::create(0.5), CallFunc::create([=]{this->removeFromParentAndCleanup(true); }));
	this->runAction(seq);

}

void UIShadowShip::update(float dt)
{

}
void UIShadowShip::moveEnd()
{
	if (m_qSailRoads.size() > 0)
	{
		Vec2 startPoint = m_qSailRoads.front().start;
		Vec2 endPoint = m_qSailRoads.front().end;
		int64_t time = m_qSailRoads.front().millisec;
		double sec = (double)time / 1000;	
		startPoint.y = MAP_WIDTH * MAP_CELL_WIDTH - startPoint.y;
		endPoint.y = MAP_WIDTH * MAP_CELL_WIDTH - endPoint.y;
		this->setPosition(startPoint);
		m_qSailRoads.pop();
		this->shipMove(endPoint,sec);
	}
	else
	{
		m_stopped = true;
		return;
	}

}
