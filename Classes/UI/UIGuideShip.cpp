#include "UIGuideShip.h"
#include <math.h>
#include "UITips.h"

#include "UILoadingIndicator.h"
#include "UIGuideSailManage.h"
#include "ProtocolThread.h"
#include "UIGuideSailHUD.h"

#include "UIBasicLayer.h"
#include "UIInform.h"


#define  ACTION_TAG 10

Sprite3D* Ship3DGuide::create(const std::string &modelPath)
{
	if (modelPath.length() < 4)
		CCASSERT(false, "invalid filename for Sprite3D");

	auto sprite = new (std::nothrow) Ship3DGuide();
	if (sprite && sprite->initWithFile(modelPath))
	{
		sprite->_contentSize = sprite->getBoundingBox().size;
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Ship3DGuide::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (isVisitableByVisitingCamera()){
		Mat4 transform = parentTransform;
#if 0
		std::vector<Node*> myParents;
		Node* start = this->getParent();
		while (start){
			myParents.push_back(start);
			start = start->getParent();
		}
		std::vector<Node*>::iterator iter;
		for (iter = myParents.end() - 1; iter != myParents.begin() - 1; iter--)
		{
			Node*node = *iter;
			parentFlags = node->processParentFlags(transform, parentFlags);
			transform = node->getModelViewTransform();
		}
#endif
		Sprite3D::visit(renderer, transform, parentFlags);
	}
	else{

	}
}

UIGuideShip::UIGuideShip(int sid) :
m_pParent(nullptr),
m_MoveEnd(true),
m_IsShipMoving(0),
m_bIsCenterY(true),
m_SupplyFlag(5000),
m_bIsCenterX(true),
m_IsAutoSailing(false),
m_IsHaveCheckCity(true),
m_CityLayer(nullptr),
m_PreUpdatePos(0, 0),
_s3d(nullptr),
m_vImage(nullptr),
m_ShipDirect(0.0),
m_PauseTime(0),
m_speed(1)
{
	m_lastSailingDays = 0;
	m_useDay = 0;
	m_ptime = 1;
	m_atime = 1000;
	m_SupplyDays = 0;
	m_SailRoad = nullptr;
	m_CurrentIndex = 1;
	m_ShipId = sid;
	m_CurrentCrew = 0;
	m_DieCrew = 0;
	m_GoSeaTime = 0;
	_obj_x = nullptr;
	m_IsPause = false;
	pausedMillis = 0;
	pausedStartMillis = 0;
	m_dir = nullptr;
	m_autoSailInterrupt = false;
	m_lastUpdateMilliSec = 0;

	m_flag_index = 0;
	m_oars_index = 0;
}

UIGuideShip::~UIGuideShip()
{
	m_vImage->release();
}


bool UIGuideShip::init(int sid)
{
	bool pRet = false;
	do
	{

		m_vImage = cocos2d::Sprite::create();
		CC_BREAK_IF(!m_vImage);
		m_vImage->setContentSize(Size(100, 100));
		m_vImage->retain();
		//this->addChild(m_vImage);
		m_WinSize = Director::getInstance()->getWinSize();

		{
			_obj_x = Node::create();
			this->addChild(_obj_x);
			log("create ship3d");
			//test3d
			m_sp = Ship3DGuide::create(getShipTestModePath_3D(sid));
			addStandByPoint();
		//	_s3d = Ship3DGuide::create(getShipModePath_3D(sid));

			m_sp->setRotation3D(Vec3(0, -90.0f, 0));
			//			_s3d->setRotation3D(Vec3(-45.0f,180,0));
			_s3d->setScale(1.5f);
			_obj_x->addChild(_s3d, 50);
			_obj_x->setRotation3D(Vec3(0, 0, 0));
		}
		m_GoSeaTime = getCurrentTimeUsev();
		initCityPortPos();
		auto ship_falg = Sprite::create();
		ship_falg->setTexture("res/ship_flag.png");
		ship_falg->setPosition(Vec2(0, 65));
		_obj_x->addChild(ship_falg, 60);
		ship_falg->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0, 10))), EaseBackOut::create(MoveBy::create(0.5, Vec2(0, -10))))));

		m_dir = Sprite::create();
		m_dir->setTexture("res/direct.png");
		m_dir->setAnchorPoint(Vec2(0.5, 0.5));
		m_dir->setScale(0.25f);
		m_dir->setPositionX(-10);
		m_dir->setRotation3D(Vec3(90, 0, -180));
		_s3d->addChild(m_dir, 50);
		m_dir->setVisible(true);
		m_dir->runAction(Sequence::create(FadeIn::create(0.5f), DelayTime::create(2.0f), FadeOut::create(0.5f), Hide::create(), nullptr));

		//add 3D particles

		//auto rootps = PUParticleSystem3D::create("waveTest/scripts/lineStreak.pu", "waveTest/materials/pu_mediapack_01.material");//cocos material
		//auto rootps = PUParticleSystem3D::create("shipwave3D/scripts/wave.pu", "shipwave3D/materials/wave.material");//our material
		//rootps->setCameraMask(2);
		//rootps->startParticleSystem();
		// _s3d->addChild(rootps,0);

		pRet = true;
	} while (0);

	return pRet;
}

void UIGuideShip::onEnter()
{
	Node::onEnter();
	this->scheduleUpdate();
	log("ShipGuide::onEnter");
	_s3d->setCameraMask((unsigned short)CameraFlag::USER1);
	//	_obj_x->setCameraMask((unsigned short)CameraFlag::USER1, true);
	Size s = CCDirector::getInstance()->getWinSize();
	camera = Camera::createPerspective(60, (GLfloat)s.width / s.height, 1, 1000);
	camera->setCameraFlag(CameraFlag::USER1);
	//camera->setStartVisitNode(_s3d);
	Vec3 shipPos = _s3d->getPosition3D();
	_obj_x->addChild(camera);
	camera->setPosition3D(shipPos + Vec3(0, 60 * 0.8f, 60));
	camera->lookAt(shipPos, Vec3(0, 1, 0));
	camera->setDepth(2);
	m_TargetPos = _position;
}
void UIGuideShip::onExit()
{
	Node::onExit();
}

void UIGuideShip::initCityPortPos()
{
	auto cityInfo = SINGLE_SHOP->getCitiesInfo();
	for (int i = 0; i <CITY_NUMBER; i++)
	{	
		m_CityPort[i][0] = cityInfo[i + 1].x;
		m_CityPort[i][1] = cityInfo[i + 1].y;
	}
}

void UIGuideShip::setSupplyDays(int days)
{
	m_SupplyDays = days;
}
void UIGuideShip::changeShipDirect()
{
	//random
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_LOOF_26);
	m_dir->setVisible(true);
	m_dir->stopAllActions();
	m_dir->runAction(Sequence::create(FadeIn::create(0.5f), DelayTime::create(2.0f), FadeOut::create(0.5f), Hide::create(), nullptr));
	float angle = m_ShipDirect;
	float sigle_angle = std::abs(angle);
	if (sigle_angle > M_PI_2)
	{
		sigle_angle = M_PI - sigle_angle;
	}

	if (angle < 0)
	{
		angle = M_PI * 2.0 + angle;
	}
	{
		//test3d
		auto shipPos = _s3d->getPosition3D();
		//		camera->setPosition3D(shipPos + Vec3(0, 130, 130));
		//		camera->lookAt(shipPos,Vec3(0, 1, 0));
		Vec3 rotation = _s3d->getRotation3D();

		float x = -45.0 * (std::cos(angle));
		x = std::cos(angle);
		float y = CC_RADIANS_TO_DEGREES(angle);

		_s3d->setRotation3D(Vec3(x, 180 + y, 0));
		//		float z = 60.0f * std::sin(angle);
		//		_obj_x->setRotation3D(Vec3(std::abs(z),0,0));
		//_obj_x->setRotation3D(Vec3(0,0,y));
	}
}
UIGuideShip* UIGuideShip::create(int sid)
{
	UIGuideShip* move = new UIGuideShip(sid);
	CC_RETURN_IF(move);
	if (move)
	{
		move->autorelease();
		move->init(sid);
		return move;
	}
	return nullptr;
}

void UIGuideShip::setParent(Node* parent)
{
	Node::setParent(parent);
	if (parent)
	{
		m_pParent = parent;
		m_CityLayer = m_pParent->getChildByTag(3);
	}
}

void UIGuideShip::setGoldPosition(cocos2d::Vec2 pos)
{
	if (pos == m_TargetPos)
	{
		return;
	}
	m_TargetPos = pos;
	if (m_IsAutoSailing)
	{
		m_PauseTime = getCurrentTimeUsev();
		UIGuideSailManage* map = dynamic_cast<UIGuideSailManage*>(m_pParent->getParent());
		UIGuideSailHUD* mapui = dynamic_cast<UIGuideSailHUD*>(map->getMapUI());
		mapui->m_nConfirmIndex = UIGuideSailHUD::CONFIRM_INDEX_NOTAUTO;
		UIInform::getInstance()->openInformView(mapui);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_COMMON_CSB_WHARF_AUTOSEA_TITLE", "TIP_SAILING_NOTAUTO_GOSEA_ASK");
		Director::getInstance()->pause();
		return;
	}
	m_IsAutoSailing = false;

	float x = pos.x - _position.x;
	float y = pos.y - _position.y;
	m_ShipDirect = Vec2(x, y).getAngle();
	changeShipDirect();
	m_IsShipMoving = true;
}
void UIGuideShip::setAutoSailGoldPosition(Vec2 pos)
{
	if (pos == m_TargetPos)
	{
		return;
	}
	m_TargetPos = pos;

	float x = pos.x - _position.x;
	float y = pos.y - _position.y;
	m_ShipDirect = Vec2(x, y).getAngle();
	changeShipDirect();
	//m_IsShipMoving = true;
}
int64_t	UIGuideShip::getCurrentTimeUsev()
{
	timeval timeStruc;
	gettimeofday(&timeStruc, nullptr);
	int64_t sec1 = ((int64_t)timeStruc.tv_sec) * 1000;
	int64_t sec2 = timeStruc.tv_usec / 1000;

	return sec1 + sec2 - pausedMillis;
}

void UIGuideShip::setSpeed(int sp){

	m_speed = sp;
}

float UIGuideShip::getSpeed(){
	//log("speed %d", m_speed);
	float tmp = m_speed > 10 ? m_speed : 10;
	return tmp / 100.0f;
}

void UIGuideShip::stopShip(bool isStop)
{
	if (m_IsAutoSailing)
	{
		m_PauseTime = getCurrentTimeUsev();
		UIGuideSailManage* map = dynamic_cast<UIGuideSailManage*>(m_pParent->getParent());
		UIGuideSailHUD* mapui = dynamic_cast<UIGuideSailHUD*>(map->getMapUI());
		mapui->m_nConfirmIndex = UIGuideSailHUD::CONFIRM_INDEX_NOTAUTO;
		UIInform::getInstance()->openInformView(mapui);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SAILING_NOTAUTO_GOSEA_ASK", "TIP_SAILING_NOTAUTO_GOSEA_ASK");
		Director::getInstance()->pause();
		return;
	}
	m_IsShipMoving = !isStop;
}
void UIGuideShip::setUpdatePos(Point pos)
{
	m_PreUpdatePos = pos;
}
void UIGuideShip::setUpdateSize(Size wh)
{
	m_UpdateSize = wh;
}

bool UIGuideShip::isCanLandCity()
{
	UIGuideSailManage* map = dynamic_cast<UIGuideSailManage*>(m_pParent->getParent());
	for (int i = 0; i < CITY_NUMBER; i++)
	{
		float distanceP = ccpDistance(Vec2(m_CityPort[i][0], m_CityPort[i][1]), _position);
		if (std::abs(distanceP) < LAND_DISTANCE)
		{
			map->setCityId(i + 1);
			map->openDialog();
			m_IsShipMoving = false;
			m_IsHaveCheckCity = true;
			return true;
		}
	}
	return false;
}

void UIGuideShip::isContinueAutoSailing(bool isBreak)
{
	int64_t curTime = getCurrentTimeUsev();
	m_PauseTime = curTime - m_PauseTime;
	Director::getInstance()->resume();
	if (m_PauseTime < 0)
	{
		m_PauseTime = 0;
	}
	m_GoSeaTime += m_PauseTime;
	m_PauseTime = 0;
	if (isBreak)
	{
		this->stopAllActionsByTag(ACTION_TAG);
		m_pParent->stopAllActionsByTag(ACTION_TAG);
		m_IsAutoSailing = false;
		m_IsShipMoving = false;
	}
	else
	{
		m_IsAutoSailing = true;
	}
}
void UIGuideShip::update(float f)
{
	//struct timeval start,end;
	//gettimeofday(&start,0);

	int64_t curTime = getCurrentTimeUsev();
	if (!m_IsShipMoving){
		m_lastUpdateMilliSec = 0;
	}
	if (!pausedStartMillis)//time_val > SHIP_MOVE_VAL)
	{

		m_SupplyFlag += int(f * 1000);
		if (m_SupplyFlag > SECONDS_PER_DAY)
		{
			m_SupplyFlag = 0;
			updateUseDays(curTime);
		}

		if (m_IsShipMoving && !m_IsAutoSailing)
		{

			UIGuideSailManage* map = dynamic_cast<UIGuideSailManage*>(m_pParent->getParent());
			for (int i = 0; i < CITY_NUMBER; i++)
			{
				
				float distanceP = ccpDistance(Vec2(m_CityPort[i][0], m_CityPort[i][1]), _position);
				if (std::abs(distanceP) < FIND_NEWCITY_DISTANCE&&!SINGLE_HERO->getFindCityStatus(i))
				{		
					if ((i!= 30)&&(i!=26))//比萨,休达
					{
						SINGLE_HERO->setFindCityStatus(i, true);
						SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_USED_PROP_21);
						map->setCityId(i + 1);
						map->openfindNewCity();
						map->initCityData();
						startPauseTime();
					}
				}
				else
				{
					if (!m_IsHaveCheckCity && std::abs(distanceP) < LAND_DISTANCE_1)// before LAND_DISTANCE
					{
						log("Have City Login :%d", i);
						m_IsShipMoving = false;
						m_IsHaveCheckCity = true;
						map->setCityId(i + 1);
						if ((i != 30) && (i != 26))//比萨,休达
						{
							map->openDialog();
						}					
						return;
					}
					else if (std::abs(distanceP) < LAND_DISTANCE_1)// before LAND_DISTANCE
					{
						break;
					}
					else if (i == CITY_NUMBER - 1)
					{
						m_IsHaveCheckCity = false;
					}
				}

			}
			float x_speed = 0;//120+x/ 45
			float y_speed = 0;

			float  factor = f / (1.0 / 60);
			if (factor >= 3.5){
				factor = 1;
			}
			x_speed = factor*getSpeed() * cos(m_ShipDirect);//120+x/ 45
			y_speed = factor*getSpeed() * sin(m_ShipDirect);
			/*
			if (!m_lastUpdateMilliSec){
				x_speed = getSpeed() * cos(m_ShipDirect);//120+x/ 45
				y_speed = getSpeed() * sin(m_ShipDirect);
			}
			else if (curTime > m_lastUpdateMilliSec){
				float factor = (curTime - m_lastUpdateMilliSec)*1.0 / (1000.0 / 60);
				x_speed = factor*getSpeed() * cos(m_ShipDirect);//120+x/ 45
				y_speed = factor*getSpeed() * sin(m_ShipDirect);
			}
			*/
			m_lastUpdateMilliSec = curTime;
			Point map_pos = m_pParent->getPosition();
			Point ship_pos = _position;
			//x direct
			if (true)//std::abs(x) > std::abs(x_speed) - 0.1)
			{
				if (m_bIsCenterX)
				{
					map_pos.x -= x_speed;// * (x/std::abs(x));
				}
				else
				{
					float diff = ship_pos.x + map_pos.x;
					if (diff < m_WinSize.width / 2 && std::abs(map_pos.x) > 100.0)
					{
						map_pos.x += (m_WinSize.width / 2 - diff);
						m_bIsCenterX = true;
					}
					if (diff > m_WinSize.width / 2 && std::abs(map_pos.x) < 10.0)
					{
						map_pos.x -= (diff - m_WinSize.width / 2);
						m_bIsCenterX = true;
					}
				}
				ship_pos.x += x_speed;//* (x/std::abs(x));
			}
			//y direct
			if (true)//std::abs(y) > std::abs(y_speed) - 0.1)
			{
				if (m_bIsCenterY)
				{
					map_pos.y -= y_speed;//* (y/std::abs(y));
				}
				else
				{
					float diff = ship_pos.y + map_pos.y;
					if (diff < m_WinSize.height / 2 && std::abs(map_pos.y) > 100.0)
					{
						map_pos.y += (m_WinSize.height / 2 - diff);
						m_bIsCenterY = true;
					}
					if (diff > m_WinSize.height / 2 && std::abs(map_pos.y) < 10.0)
					{
						map_pos.y -= (diff - m_WinSize.height / 2);
						m_bIsCenterY = true;
					}
				}
				ship_pos.y += y_speed;// * (y/std::abs(y));
			}
			else
			{
				ship_pos.y = m_TargetPos.y;
				if (m_bIsCenterY)
				{
					//map_pos.y -= y;
				}
			}
			if (ship_pos.x < 0.1 || ship_pos.y < 0.1 || ship_pos.x > MAP_HEIGH * MAP_CELL_WIDTH - 1 || ship_pos.y > MAP_WIDTH * MAP_CELL_HEIGH - 1)
			{
				m_IsShipMoving = false;
				return;
			}
			//int index_x = (ship_pos.x + 20.0)/MAP_CELL_WIDTH;
			//int index_y = MAP_WIDTH - int((ship_pos.y + 20.0)/MAP_CELL_HEIGH) - 1;


			bool hitText = map->getHistCheck()->landHitTest(ship_pos.x, MAP_WIDTH * MAP_CELL_HEIGH - ship_pos.y, 20);
			if (hitText)
			{
				m_IsShipMoving = false;
				return;
			}

			if (map_pos.x > 0)
			{
				m_bIsCenterX = false;
				map_pos.x = 0;
			}
			if (map_pos.y > 0)
			{
				m_bIsCenterY = false;
				map_pos.y = 0;
			}
			if (std::abs(map_pos.x) > (MAP_HEIGH - SCREEN_WIDTH_NUM) * MAP_CELL_WIDTH)
			{
				m_bIsCenterX = false;
				map_pos.x = -(MAP_HEIGH - SCREEN_WIDTH_NUM) * MAP_CELL_WIDTH;
			}
			if (std::abs(map_pos.y) > MAP_WIDTH * MAP_CELL_HEIGH - m_WinSize.height)
			{
				m_bIsCenterY = false;
				map_pos.y = -(MAP_WIDTH * MAP_CELL_HEIGH - m_WinSize.height);
			}
			this->setPosition(ship_pos);
			m_pParent->setPosition(map_pos);
			if (ship_pos.x > m_PreUpdatePos.x + m_UpdateSize.width || ship_pos.y > m_PreUpdatePos.y + m_UpdateSize.height)
			{
				TextureCache::getInstance()->removeUnusedTextures();
				m_PreUpdatePos = Vec2(int(ship_pos.x / m_UpdateSize.width) * m_UpdateSize.width, int(ship_pos.y / m_UpdateSize.height) * m_UpdateSize.height);
				ProtocolThread::GetInstance()->getNextMapBlock(makeShipRoutine(), m_useDay);
				log("request one ..");
			}
			else if (ship_pos.x < m_PreUpdatePos.x || ship_pos.y < m_PreUpdatePos.y)
			{

				TextureCache::getInstance()->removeUnusedTextures();
				m_PreUpdatePos = Vec2(int(ship_pos.x / m_UpdateSize.width) * m_UpdateSize.width, int(ship_pos.y / m_UpdateSize.height) * m_UpdateSize.height);
				ProtocolThread::GetInstance()->getNextMapBlock(makeShipRoutine(), m_useDay);
				log("request one ..");
			}
		}
	}
	else{
		m_lastUpdateMilliSec = 0;
	}
	//gettimeofday(&end,0);
	//log(" ship::update : %d",(end.tv_usec - start.tv_usec + (end.tv_sec-start.tv_sec)*1000000));
}

struct _ShipRoutine*UIGuideShip::makeShipRoutine(){
	ShipRoutine*routine = (ShipRoutine*)malloc(sizeof(ShipRoutine));
	ship_routine__init(routine);
	routine->n_roads = 1;
	routine->roads = (ShipPoint**)malloc(sizeof(ShipPoint*)*routine->n_roads);
	int num = 0;

	ShipPoint*define = (ShipPoint*)malloc(sizeof(ShipPoint));
	ship_point__init(define);

	define->millisec = 0;
	define->x = (int)_position.x;
	define->y = (int)(MAP_WIDTH * MAP_CELL_HEIGH - _position.y);

	routine->roads[num++] = define;
	return routine;
}

void UIGuideShip::autoSail()
{
	if (m_SailRoad == nullptr)
	{
		return;
	}
	if (!m_IsAutoSailing)
		return;
	UIGuideSailManage* map = dynamic_cast<UIGuideSailManage*>(m_pParent->getParent());
	for (int i = 0; i<CITY_NUMBER; i++)
	{
		float distanceP = ccpDistance(Vec2(m_CityPort[i][0], m_CityPort[i][1]), _position);
		if (std::abs(distanceP) < FIND_NEWCITY_DISTANCE&&!SINGLE_HERO->getFindCityStatus(i))
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_USED_PROP_21);
			SINGLE_HERO->setFindCityStatus(i, true);
			if ((i != 26) && (i != 16))
			{
				map->setCityId(i + 1);
				map->openfindNewCity();
				map->initCityData();
			}
			m_autoSailInterrupt = true;
			setSeaAccident(true);
			startPauseTime();

			m_IsAutoSailing = false;
			return;
			//stopShip(true);
		}
	}
	if (m_CurrentIndex >= m_SailRoad->resultData->num || m_CurrentIndex < 0)
	{
		m_IsAutoSailing = false;
		isCanLandCity();
		return;
	}
	if (m_SailRoad->isNormal)
	{
		float x = m_SailRoad->resultData->points[m_CurrentIndex]->x;
		float y = MAP_WIDTH * MAP_CELL_WIDTH - m_SailRoad->resultData->points[m_CurrentIndex]->y;
		float useTime = _position.getDistance(Vec2(x, y)) / (60 * getSpeed());
		auto shipMove = MoveTo::create(useTime, Vec2(x, y));
		auto mapMove = MoveBy::create(useTime, Vec2(_position.x - x, _position.y - y));
		auto seq = Sequence::create(shipMove, CallFunc::create(this, callfunc_selector(UIGuideShip::MoveEnd)), nullptr);
		auto seq_map = Sequence::create(mapMove, nullptr);
		seq_map->setTag(ACTION_TAG);
		seq->setTag(ACTION_TAG);
		setAutoSailGoldPosition(Vec2(x, y));
		m_pParent->runAction(seq_map);
		this->runAction(seq);
	}
	else
	{
		float x = m_SailRoad->resultData->points[m_SailRoad->resultData->num - m_CurrentIndex - 1]->x;
		float y = MAP_WIDTH * MAP_CELL_WIDTH - m_SailRoad->resultData->points[m_SailRoad->resultData->num - m_CurrentIndex - 1]->y;
		float useTime = _position.getDistance(Vec2(x, y)) / (60 * getSpeed());
		auto shipMove = MoveTo::create(useTime, Vec2(x, y));
		auto mapMove = MoveBy::create(useTime, Vec2(_position.x - x, _position.y - y));
		auto seq = Sequence::create(shipMove, CallFunc::create(this, callfunc_selector(UIGuideShip::MoveEnd)), nullptr);
		auto seq_map = Sequence::create(mapMove, nullptr);
		seq_map->setTag(ACTION_TAG);
		seq->setTag(ACTION_TAG);
		setAutoSailGoldPosition(Vec2(x, y));
		m_pParent->runAction(seq_map);
		this->runAction(seq);
	}
	m_CurrentIndex++;
}
void UIGuideShip::MoveEnd()
{
	autoSail();
}
void UIGuideShip::updateUseDays(int64_t curTime)
{
	UIGuideSailManage* map = dynamic_cast<UIGuideSailManage*>(m_pParent->getParent());
	if (m_CurrentCrew <= m_DieCrew)
	{
		this->unscheduleUpdate();
		return;
	}
	int64_t	useTime = curTime - m_GoSeaTime;
	//if(!ProtocolThread::GetInstance()->isInTutorial() ||  m_useDay==0){
	//m_useDay = m_lastSailingDays + useTime / SECONDS_PER_DAY + 1;  //开始多一天
	m_useDay = 1;//新手引导为一天
	map->setSailingDays(m_useDay);
	//}
	ProtocolThread::GetInstance()->getNextMapBlock(makeShipRoutine(), m_useDay);
	auto mapUI = dynamic_cast<UIGuideSailHUD*>(map->getMapUI());
	mapUI->setSailingDay(m_useDay);
}

void UIGuideShip::setCurrentCrew(int crewnum)
{
	m_CurrentCrew = crewnum;
}

void UIGuideShip::setSeaAccident(bool isHave)
{
	if (isHave)
	{
		this->pause();
		//		this->setVisible(false);
		m_pParent->pause();
		m_PauseTime = getCurrentTimeUsev();
		m_IsShipMoving = false;
	}
	else
	{
		//		this->setVisible(true);
		this->resume();
		m_pParent->resume();
		if (m_PauseTime < 1)
		{
			return;
		}
		auto curTime = getCurrentTimeUsev();
		m_PauseTime = curTime - m_PauseTime;
		if (m_PauseTime < 1)
		{
			m_PauseTime = 0;
		}
		m_GoSeaTime += m_PauseTime;
		m_PauseTime = 0;
	}
}
std::vector<std::string> UIGuideShip::analysisName(std::string name)
{
	std::string str = name;
	std::vector<std::string> nameInfos;
	while (str.find_first_of("_") != -1)
	{
		int a = str.find_first_of("_");
		std::string str1 = str.substr(0, a);
		nameInfos.push_back(str1);
		str = str.substr(a + 1, str.npos);
		if (str.find_first_of("_") == -1)
		{
			nameInfos.push_back(str);
		}
	}
	return nameInfos;
}
void UIGuideShip::addStandByPoint()
{
	auto winsize = _director->getWinSize();
	m_standPoint = Sprite3D::create("Sprite3DTest/standbyanimation.c3b");
	auto animation = Animation3D::create("Sprite3DTest/standbyanimation.c3b");
	auto mate = Animate3D::create(animation);
	m_standPoint->runAction(RepeatForever::create(mate));
	m_standPoint->setCameraMask(2);
	auto attach = m_standPoint->getAttachNode("standbypoint_ship_M_100_1");
	attach->addChild(m_sp);
	_s3d = m_standPoint;
	addSail(m_sp);
}

void UIGuideShip::addSail(Sprite3D * p_Sender)
{
	auto skele = p_Sender->getSkeleton();
	if (skele)
	{
		int boneNum = skele->getBoneCount();
		for (int i = 0; i < boneNum; i++)
		{
			auto bone = skele->getBoneByIndex(i);
			auto attach = p_Sender->getAttachNode(bone->getName());
			attach->addChild(confirmSail(bone));
		}
	}
}

Sprite3D * UIGuideShip::confirmSail(Bone3D * bone)
{
	auto name = bone->getName();
	std::vector<std::string> nameInfos;
	nameInfos = analysisName(name);
	Sprite3D * sp = Sprite3D::create();
	int a = atoi(nameInfos.at(3).c_str());
	float scaleValue = float(a) / 100;
	if (nameInfos.at(1).find("flag") != -1)
	{

		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		sp->setName(nameInfos.at(0) + "_" + nameInfos.at(1));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));
		}
		for (int a = 0; a < sp->getMeshCount(); a++)
		{
			auto mesh = sp->getMeshByIndex(a);
			if (a == m_flag_index)
			{
				mesh->setVisible(true);
				continue;
			}
			mesh->setVisible(false);
		}
	}
	else if (nameInfos.at(1).find("sail") != -1)
	{
		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));
		}
	}
	else if (nameInfos.at(1).find("oar") != -1)
	{
		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));

		}
		sp->setName(nameInfos.at(0) + "_" + nameInfos.at(1));
		/*		m_oars.pushBack(sp);*/
		for (int a = 0; a < sp->getMeshCount(); a++)
		{
			auto mesh = sp->getMeshByIndex(a);
			if (a == m_oars_index)
			{
				mesh->setVisible(true);
				continue;
			}
			mesh->setVisible(false);
		}
	}
	//	sp->setTag(2);
	sp->setCameraMask(2);
	return sp;

}
