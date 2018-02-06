#include "UISailManage.h"
#include "UILoadingIndicator.h"
#include "ProtocolThread.h"

#include "ShipPathData.h"
#include "UIShip.h"
#include "TVSailDefineVar.h"
#include "TVSingle.h"
#include "UISailHUD.h"
#include "CCLuaEngine.h"
#include "UITips.h"
#include "TVSeaEventManage.h"
#include "UINPCShip.h"
#include "UIEveryDaySign.h"
#include "UICommon.h"
#include "MainTaskManager.h"
#include "CompanionTaskManager.h"
#include "env.h"
#include "UIOffLine.h"
#include "UIShadowShipManage.h"
#include "UIShadowShip.h"
#include "Utils.h"
#include "AppHelper.h"
#define MAPLAYER_TAG 55
using namespace ui;
//点击海域类，出现小白点的
class TickIcon : public Node
{
public:
	TickIcon(int flag):flag(flag){};
	~TickIcon(){};
	int flag;
	Sprite* m_Node;
	static TickIcon* create(int flag = 0)
	{
		TickIcon* ti = new TickIcon(flag);
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
			{
				m_Node = Sprite::create();
				m_Node->setTexture("res/map_cell/cursor.png");
			}
			this->addChild(m_Node);
			this->scheduleOnce(schedule_selector(TickIcon::removeIcon),0.5f);
			pRet = true;
		} while (0);
		return pRet;
	}
	void start()
	{
		this->unscheduleAllSelectors();
		this->setVisible(true);
		this->scheduleOnce(schedule_selector(TickIcon::removeIcon),0.5f);
	}
	void removeIcon(float f)
	{
		this->setVisible(false);
		//this->removeFromParent();
	}
private:

};

void getRandomAnimations(int randomCount,Animation* ani)
{
	Vector<AnimationFrame*> &sprites = (Vector<AnimationFrame*>&)(ani->getFrames());
	for (int i = 0; i < randomCount;i++)
	{
		AnimationFrame* tempFrames = sprites.at(0);
		tempFrames->retain();
		sprites.erase(0);
		sprites.pushBack(tempFrames);
		tempFrames->release();
	}
}

UISailManage::UISailManage():
	m_MapsLayer(nullptr),
	m_pShipImage(nullptr),
	m_PrePoint(0,0),
	m_pLoadLayer(nullptr),
	m_pTouchIcon(nullptr),
	m_ScreenFirstPos(10,14),
	m_bIsBlock(false),
	m_ischufa(0),
	m_pHistCheck(nullptr),
	m_ShipPostion(-1,-1),
	m_nShipMoveTime(0),
	m_pSailRoad(nullptr),
	m_pSeaManage(nullptr)
{
	m_nSailDays = 0;
	m_pHistCheck = new HistCheck;
	m_pMapUI = nullptr;
	m_pLeaveCityResult = nullptr;
	m_pLoginFirstData=nullptr;
	m_eUIType = UI_MAP;
	m_vNPCShipsF.clear();
	m_vNPCShipsD.clear();
	m_nCityId = 0;
	m_EventLayer = nullptr;
	m_bIsCaptainUp = false;
	m_npcIsActive = 30.0;
	m_lastNPCEventID = 0;
	m_bFailedAutoFight = 0;
	m_nTaskNpcId = 0;
	m_bIsLeaveCityTosea=false;
	m_bCaptainToHeroUI=false;
	m_pReachCityResult=nullptr;
	m_mapUICamera = nullptr;
	imageSeaTaskTipBg = nullptr;
	imageSeaTask = nullptr;
	imageSeaCompanionsTaskTipBg = nullptr;
	imageSeaCompanionsTask = nullptr;
	m_dayViewOpened = true;
	m_bIsPause = false;
	m_nearbyShipManager = new UIShadowShipManage(this);
	m_HeroExpLevelUp = false;
	m_HeroFameLevelUp = false;
	m_captain_skill_outlookLv = 0;
}

UISailManage::~UISailManage()
{
	m_UnLoadImage_sea.clear();
	m_UnLoadImage_land.clear();
	m_vNPCShipsF.clear();
	m_vNPCShipsD.clear();
	delete m_pHistCheck;
	if (m_pLeaveCityResult)
	{
		leave_city_result__free_unpacked(m_pLeaveCityResult,0);
		m_pLeaveCityResult = nullptr;
	}
	m_pLoginFirstData=nullptr;
	m_pReachCityResult=nullptr;
	if (m_pSailRoad)
	{
		delete m_pSailRoad;
	}
	if (m_pSeaManage)
	{
		m_pSeaManage->release();
	}
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

UISailManage* UISailManage::create()
{
	auto* myMap = new UISailManage();
	if (myMap &&myMap->init())
	{
		myMap->autorelease();
		myMap->setName("MAPS");
		return myMap;
	}
	CC_SAFE_DELETE(myMap);
	return nullptr;
}

UISailManage* UISailManage::create(Point pos, bool seainfo)
{
	auto* myMap = new UISailManage();
	myMap->m_ShipPostion = pos;
	if (myMap && myMap->init(seainfo))
	{
		myMap->autorelease();
		myMap->setName("MAPS");
		return myMap;
	}
	CC_SAFE_DELETE(myMap);
	return nullptr;
}

void UISailManage::onEnter()
{
	UIBasicLayer::onEnter();
	auto size = Director::getInstance()->getWinSize();
	Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
	popCam->setCameraFlag(CameraFlag::USER2);
	popCam->setDepth(4);
	//popCam->setStartVisitNode(m_pMapUI);
	m_mapUICamera = popCam;
	this->setAnchorPoint(Vec2(0.5f, 0.5f));
	this->addChild(popCam);

	
}

void UISailManage::registerTouchCallBack(float fTime)
{
	auto listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = CC_CALLBACK_2(UISailManage::onTouchBegan1, this);
	listener->onTouchMoved = CC_CALLBACK_2(UISailManage::onTouchMoved1, this);
	listener->onTouchCancelled = CC_CALLBACK_2(UISailManage::onTouchCancelled1, this);
	listener->onTouchEnded = CC_CALLBACK_2(UISailManage::onTouchEnded1, this);
	listener->setSwallowTouches(true);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void UISailManage::onExit()
{
	this->unscheduleUpdate();
	TextureCache::getInstance()->unbindAllImageAsync();
	m_UnLoadImage_land.clear();
	m_UnLoadImage_sea.clear();
	_eventDispatcher->removeEventListenersForTarget(this);
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("res/test/battlewater.plist");
	UIBasicLayer::onExit();
}

bool UISailManage::init(bool seainfo)
{
	bool pRet = false;
	do 
	{
		SINGLE_HERO->m_bInSafeArea = true;
		Layer::init();
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UISailManage::onServerEvent, this),this);
		if (seainfo)
		{
			if (UserDefault::getInstance()->getIntegerForKey(SOUND_EFFECT_KEY, OPEN_AUDIO) != OPEN_AUDIO)
			{
				SINGLE_AUDIO->setEffectON(false);
			}
			ProtocolThread::GetInstance()->getSailInfo(UILoadingIndicator::create(this,4));
		}else
		{
			ProtocolThread::GetInstance()->leaveCity(UILoadingIndicator::create(this,4));
		}

		m_MapsLayer = Layer::create();
		this->addChild(m_MapsLayer,1,MAPLAYER_TAG);
		m_MapsLayer->setContentSize(Size(MAP_HEIGH * MAP_CELL_HEIGH,MAP_WIDTH * MAP_CELL_WIDTH));
		m_pLoadLayer = UILoadingIndicator::create(this,1);
		m_pLoadLayer->showSelf();
		m_EventLayer = Layer::create();
		m_MapsLayer->addChild(m_EventLayer,3);

		pRet = true;
	} while (0);
	return pRet;
}

void UISailManage::initStatic(float f)
{
	if (m_pMapUI)
	{
		return;
	}
	m_pSeaManage = TVSeaEventManage::createManage(this);
	m_pSeaManage->retain();
	m_pSeaManage->scheduleUpdate(); 
	this->addChild(m_pSeaManage);
	initCityData();
	m_pHistCheck->initLandHistTestData();	
	m_pMapUI = UISailHUD::create();
	m_pMapUI->setCameraMask(4,true);
	this->addChild(m_pMapUI,10);
	m_pMapUI->setName("MAPUI");
}

void UISailManage::addLights()
{
//	auto directionalLight_6 = DirectionLight::create(Vec3(1.0f, -1.0f, -1.0f), Color3B(255, 255, 255));
	auto directionalLight_6 = DirectionLight::create(Vec3(1.0f, 0.0f, 0.0f), Color3B(255, 255, 255));
	directionalLight_6->setCameraMask(2);
	directionalLight_6->setIntensity(1.6f);
	directionalLight_6->setLightFlag(LightFlag::LIGHT1);
	this->addChild(directionalLight_6);

//	auto directionalLight_7 = DirectionLight::create(Vec3(-1.0f, 0.0f, 0.0f), Color3B(255, 255, 255));
//	directionalLight_7->setCameraMask(2);
//	directionalLight_7->setIntensity(1.4f);
//	directionalLight_7->setLightFlag(LightFlag::LIGHT1);
//	this->addChild(directionalLight_7);

	auto _ambientLight = AmbientLight::create(Color3B(180, 180, 180));
	//_ambientLight->retain();
	_ambientLight->setCameraMask(2);
	_ambientLight->setEnabled(true);
	this->addChild(_ambientLight);
}

void UISailManage::initNetworkData(LeaveCityResult* result)
{

	for (int i=0;i<result->n_discoverdcityids;i++)
	{
		SINGLE_HERO->setFindCityStatus(result->discoverdcityids[i] - 1, true);
	}
	initStatic(0);
	m_vEventFlag.assign(result->blockinfo->n_events,false);
	//航行出港时船只的信息设置
	m_CenterPoint = Director::getInstance()->getWinSize()/2;
	Point startPos = Vec2(result->blockinfo->x,MAP_WIDTH * MAP_CELL_WIDTH - result->blockinfo->y - result->blockinfo->height);
	Size hw = Size(result->blockinfo->width,result->blockinfo->height);
	auto shipInfo = SINGLE_SHOP->getShipData()[result->shipid];
	UIShip *ship = UIShip::create(shipInfo.model_id);
	m_MapsLayer->addChild(ship,4);

	ship->setParent(m_MapsLayer);
	ship->setUpdateSize(hw);
	ship->setUpdatePos(startPos);
	ship->setAnchorPoint(Vec2(0.5,0.5));
	ship->setSupplyDays(result->endurancedays);
	ship->setCurrentCrew(result->sailorcount);
	ship->setSpeed(result->speed);
	ship->setLastSailDays(m_nSailDays);
	ship->setServerConfigForShip(result->reportintervalsec,result->secondspersailday);
	m_pShipImage = ship;
//chengyuan++
	ship->m_dirChangePos.push_back(Vec2(result->shipx,MAP_WIDTH * MAP_CELL_WIDTH- result->shipy));
	ship->m_timePoints.push_back(getCurrentTimeUsev());

	//addLights();
	
	//航行UI界面的设置
	dynamic_cast<UISailHUD*>(m_pMapUI)->setShip(ship);
	dynamic_cast<UISailHUD*>(m_pMapUI)->setShipPositon(Vec2(result->shipx, MAP_WIDTH * MAP_CELL_WIDTH - result->shipy));
	dynamic_cast<UISailHUD*>(m_pMapUI)->setExpAndFame(result->exp, result->fame);
	dynamic_cast<UISailHUD*>(m_pMapUI)->setCrew(result->sailorcount,result->maxsailorcount,result->shipnum);
	dynamic_cast<UISailHUD*>(m_pMapUI)->setSupplies(result->tatalsupply, result -> maxtotalsupply);
	dynamic_cast<UISailHUD*>(m_pMapUI)->setCrew(result->sailorcount);
	dynamic_cast<UISailHUD*>(m_pMapUI)->setSkillLv(result);
	dynamic_cast<UISailHUD*>(m_pMapUI)->setSailingDay(m_nSailDays);
	dynamic_cast<UISailHUD*>(m_pMapUI)->setForceFightNpcId(result->fightnpcid);


	/*if (SINGLE_HERO->m_iDailyrewarddata){
		EveryDaySign*every = EveryDaySign::createEveryDaysign(2);
		m_pMapUI->addChild(every);
		every->setLocalZOrder(100);
    	m_dayViewOpened = false;
	}*/
	notifyCompleted(NONE);
	int city_x = 0;
	int city_y = 0;
	if (m_ShipPostion.x < 0)
	{
		city_x = -result->shipx + m_CenterPoint.x;
		city_y = result->shipy - MAP_WIDTH * MAP_CELL_WIDTH + m_CenterPoint.y;
		if (-result->shipx + m_CenterPoint.x > 0)
		{
			city_x = 0;
			ship->setIsCenterX(false);
		}
		if (result->shipy + m_CenterPoint.y > MAP_WIDTH * MAP_CELL_WIDTH)
		{
			city_y = 0;
			ship->setIsCenterY(false);
		}
		if (result->shipx + m_CenterPoint.x > MAP_HEIGH * MAP_CELL_WIDTH)
		{
			city_x = MAP_HEIGH * MAP_CELL_HEIGH - m_CenterPoint.x * 2;
			ship->setIsCenterX(false);
		}
		if (result->shipy < m_CenterPoint.y)
		{
			city_y = MAP_WIDTH * MAP_CELL_WIDTH - m_CenterPoint.y * 2;
			ship->setIsCenterY(false);
		}
		
		ship->setPosition(Vec2(result->shipx,MAP_WIDTH * MAP_CELL_WIDTH - result->shipy));
		m_MapsLayer->setPosition(Vec2(city_x,city_y));
		
	}else
	{
		city_x = -m_ShipPostion.x + m_CenterPoint.x;
		city_y = -m_ShipPostion.y + m_CenterPoint.y;
		if (city_x > 0)
		{
			city_x = 0;
			ship->setIsCenterX(false);
		}
		if ( city_y > 0)
		{
			city_y = 0;
			ship->setIsCenterY(false);
		}
		if (m_ShipPostion.x + m_CenterPoint.x > MAP_HEIGH * MAP_CELL_HEIGH)
		{
			city_x = m_CenterPoint.x * 2 - MAP_HEIGH * MAP_CELL_HEIGH;
			ship->setIsCenterX(false);
		}
		if (m_ShipPostion.y + m_CenterPoint.y > MAP_WIDTH * MAP_CELL_WIDTH)
		{
			city_y = m_CenterPoint.y * 2 - MAP_WIDTH * MAP_CELL_WIDTH;
			ship->setIsCenterY(false);
		}
		m_pShipImage->setPosition(m_ShipPostion /*+ Vec2(hw.width/2,hw.height/2)*/);
		m_MapsLayer->setPosition(Vec2(city_x,city_y));
	}
	
	
	m_ScreenFirstPos = Vec2(-city_x,-city_y)/MAP_CELL_WIDTH;
	m_ScreenFirstPos = Vec2(int(m_ScreenFirstPos.x),int(m_ScreenFirstPos.y));
	m_PrePoint = Vec2(m_ScreenFirstPos.x * MAP_CELL_WIDTH,m_ScreenFirstPos.y * MAP_CELL_HEIGH);
	int firstX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM/2;
	int firstY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM/2;

	auto cache = AnimationCache::getInstance();
	Animation *an = cache->getAnimation("waterAnimation");
	if (an == NULL) {
		SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
		Director::getInstance()->getTextureCache()->removeUnusedTextures();
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/test/battlewater.plist");
		Vector<SpriteFrame*> sfs(120);
		for (int i = 0; i < 120; i++) {

			SpriteFrame *sf =SpriteFrameCache::getInstance()->getSpriteFrameByName(String::createWithFormat("water%04d.png", i)->getCString());
			sfs.pushBack(sf);
		}
		an = Animation::createWithSpriteFrames(sfs, 1.f / 30, -1);	//0.15f,-1);
		cache->addAnimation(an,"waterAnimation");
	}

	m_pTouchIcon = TickIcon::create(0);
	this->addChild(m_pTouchIcon, 10);

	for (int i = 0;i < SCREEN_HEIGH_NUM* 2;i++)
	{
		for (int j = 0; j < 2* SCREEN_WIDTH_NUM;j++)
		{
			std::string fileName = getCellName(firstX + j,firstY + i);
			std::string seaName = getSeaCellName(firstX + j,firstY + i);
			auto mapCell = Sprite::create();
			mapCell->setTexture(fileName);
			auto bg = Sprite::create();
			bg->setAnchorPoint(Vec2(0,0));
			int randomCount = cocos2d::random(0,60);
			Animation* temp_an = an->clone();
			getRandomAnimations(randomCount,temp_an);
			bg->runAction(Animate::create(temp_an));

			mapCell->setAnchorPoint(Vec2(0,0));
			mapCell->addChild(bg,-2,2);
			m_MapsLayer->addChild(mapCell,1);

			mapCell->setPosition(MAP_CELL_WIDTH * (firstX + j),MAP_CELL_HEIGH*(firstY + i));
			m_RenderSprites.insert(std::pair<int,Sprite*>((firstX + j) * CONST_VALUE + i + firstY,mapCell));
		}
	}
	//自动航行
	if (m_pSailRoad)
	{
		ship->m_IsAutoSailing = true;
		ship->setAutoSailRoad(m_pSailRoad);
		ship->autoSail();
	}
	//npc初始化
	addSeaEvent_leavCity();
	this->scheduleUpdate();
	if (m_pLoadLayer)
	{
		m_pLoadLayer->removeSelf();
		m_pLoadLayer=nullptr;
	}
	m_pMapUI->setCameraMask(4);

	ProtocolThread::GetInstance()->getMyNationWar();

	//fix 出城后立刻进城问题(时间天数为1)
	//ship->updateUseDays(0);
	/*EmailLoginResult *  e_result = ProtocolThread::GetInstance()->getLastLoginResult();
	if (m_dayViewOpened && SINGLE_HERO->m_isShowOutLineResult &&  e_result->delegatecoins != 0)
	{
		this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create([=]{ OutLineDelegateLayer::openOutLineDelegateLayer(this, 4); }), nullptr));	
		SINGLE_HERO->m_isShowOutLineResult = false;
	}*/

	this->scheduleOnce(schedule_selector(UISailManage::registerTouchCallBack), 0.1f);
}
/*
* 当剧情或者对话动作结束时调用该函数
* actionIndex, 当前动作的index
*/
void UISailManage::notifyCompleted(int actionIndex)
{
	//扫尾工作
	switch (actionIndex){
	case DAILY_SIGN_IN:
		break;
	case OFFLINE_DELEGATE:
		break;
	case MAIN_STORY:
		break;
	case COMPANION_STORY:
		break;
	case SMALL_STORY:
		break;
	case FIRST_MATE:
		break;
	default:
		break;
	}
	doNextJob(actionIndex);
}

/*
* 执行下一个对话或者剧情操作
* actionIndex, 当前动作的index
*/
void UISailManage::doNextJob(int actionIndex)
{
	actionIndex++;
	switch (actionIndex)
	{
		case MAIN_STORY:
			notifyCompleted(MAIN_STORY);
			break;
		case SMALL_STORY:
			//小剧情(暂时没有海上触发的小剧情 所以海上还没有判断小剧情)
			if (SINGLE_HERO->m_nTriggerId)
			{
//				auto mainTask1 = m_vRoot->getChildByName<ImageView*>("image_main_city_bg");
//				addNeedControlWidgetForHideOrShow(mainTask1, true);
				addNeedControlWidgetForHideOrShow(nullptr, true);
				allTaskInfo(false, SMALL_STORY);
				notifyUIPartDelayTime(0.5);
			}
			else
			{
				notifyCompleted(SMALL_STORY);
			}
			break;
		case COMPANION_STORY:
			//小伙伴剧情
			if (CompanionTaskManager::GetInstance()->checkTaskAction())
			{
				addNeedControlWidgetForHideOrShow(nullptr, true);
				allTaskInfo(false, COMPANION_STORY);
				CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
			}
			else
			{
				notifyCompleted(COMPANION_STORY);
			}
			break;
		case DAILY_SIGN_IN:
			//每日签到
			if (SINGLE_HERO->m_iDailyrewarddata){
				UIEveryDaySign*every = UIEveryDaySign::createEveryDaysign(2);
				this->m_pMapUI->addChild(every);
				every->setLocalZOrder(100);
				m_dayViewOpened = false;
			}
			else
			{
				notifyCompleted(DAILY_SIGN_IN);
			}
			break;
		case OFFLINE_DELEGATE:
		{
			//离线委托结算
			EmailLoginResult *  e_result = ProtocolThread::GetInstance()->getLastLoginResult();
			if (m_dayViewOpened && SINGLE_HERO->m_isShowOutLineResult &&  e_result->delegatecoins != 0)
			{
				this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create([=]{ UIOffLine::openOffLineDelegateLayer(this, 4); }), nullptr));
				SINGLE_HERO->m_isShowOutLineResult = false;
			}
			else
			{
				notifyCompleted(OFFLINE_DELEGATE);
			}
			break;
		}
		case FIRST_MATE:
		default:
			break;
	}
}
void UISailManage::setAutoSailRoad(SailRoad* sr)
{
	if (sr == nullptr)
	{
		return;
	}
	m_pSailRoad = sr;
	return;
}

void UISailManage::initCityData()
{
	Layer* cityLayer = Layer::create();
	cityLayer->retain();
	auto citysInfo = SINGLE_SHOP->getCitiesInfo();
	std::string path;
	
	for (int i = 0; i < CITY_NUMBER; i++)
	{
		auto picNum = 0;
		if (SINGLE_SHOP->getCitiesInfo()[i + 1].port_type == 5)
		{
			picNum = SINGLE_SHOP->getCityAreaResourceInfo()[i + 1].icon_village;
		}
		else
		{
			picNum = SINGLE_SHOP->getCityAreaResourceInfo()[i + 1].icon_id;
		}
		path = StringUtils::format("cities_resources/icon/city_%d.png", picNum);
		auto city = Button::create(path);
		city->setPosition(Vec2(citysInfo[i+ 1].x,citysInfo[i + 1].y));
		city->setAnchorPoint(Vec2(0.5,0.5));
		city->setName("button_city");
		auto i_bg = ImageView::create("res/city_icon/cityname_bg.png");
		i_bg->setPosition(Vec2(citysInfo[i+ 1].x,citysInfo[i + 1].y-city->getContentSize().height*0.4));
		auto t_city = Text::create();
		t_city->setString(citysInfo[i+ 1].name);
		t_city->setPosition(Vec2(i_bg->getContentSize().width/2,i_bg->getContentSize().height*0.4));
		t_city->setColor(Color3B(255,191,38));
		t_city->setFontSize(20);
		i_bg->addChild(t_city);
		cityLayer->addChild(city,1,i + CONST_VALUE);
		cityLayer->addChild(i_bg,2);
		city->setVisible(false);
		i_bg->setVisible(false);
		if (SINGLE_HERO->getFindCityStatus(i))
		{
			city->setVisible(true);		
			i_bg->setVisible(true);
		}
//chengyuan++	发现城市的特效	
		if (i == m_nCityId - 1 && !SINGLE_HERO->getFindCityStatus(i))
		{
			SINGLE_HERO->setFindCityStatus(i, true);
			city->setVisible(true);
			i_bg->setVisible(true);
			city->setTouchEnabled(false);
			auto city_bglight = Sprite::create();
			city_bglight->setTexture("cocosstudio/login_ui/sailing_720/light_3.png");
			city_bglight->runAction(RepeatForever::create(RotateBy::create(3, 180)));
			city_bglight->setPosition(city->getContentSize()/2);
			city_bglight->setTag(211);
			city_bglight->setLocalZOrder(-1);
			city_bglight->setOpacity(0);
			city->addChild(city_bglight);
			m_MapsLayer->addChild(cityLayer, 8, 212);
			openfindNewCity(city);
		}
//
		if (SINGLE_HERO->m_iBornCity==i+1)
		{
			city->setVisible(true);		
			i_bg->setVisible(true);
		}

		city->addTouchEventListener(CC_CALLBACK_2(UISailManage::buttonEvent,this));
	}

	cityLayer->setPosition(0,0);
	if (!cityLayer->getParent())
	{
		m_MapsLayer->addChild(cityLayer, 3);
	}
	cityLayer->release();
}

std::string UISailManage::getSeaCellName(int x,int y)
{
	return "res/map_cell/sea/1.png";
	y = MAP_WIDTH - y - 1;
	if (x > MAP_HEIGH -1 || x<0)
	{
		return StringUtils::format("res/map_cell/sea/1.png");
	}
	if (y > MAP_WIDTH - 1 || y < 0)
	{
		return StringUtils::format("res/map_cell/sea/1.png");
	}

	return StringUtils::format("res/map_cell/sea/%d.png",MAP_SEA_INDEX[y][x]);
}

std::string UISailManage::getCellName(int x,int y)
{
	y = MAP_WIDTH - y - 1;
	if (x > MAP_HEIGH -1 || x < 0)
	{
		return StringUtils::format("res/map_cell/land/1.png");
	}
	if (y > MAP_WIDTH - 1 || y < 0)
	{
		return StringUtils::format("res/map_cell/land/1.png");
	}
	int index = MAPINDEX[y][x];
	return StringUtils::format("res/map_cell/land/%d.png",MAPINDEX[y][x]);
}

void UISailManage::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* w_button = dynamic_cast<Widget*>(pSender);
		std::string name = w_button->getName();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
// 		if (isButton(panel_levelup))//船长升级界面
// 		{  
// 			if (m_bIsCaptainUp)
// 			{
// 				m_bIsCaptainUp = false;
// 				CommonView::getInstance()->openCommonView(m_pMapUI);
// 				CommonView::getInstance()->getCaptainAddexp();
// 			}
// 			return;
// 		}
	}
}

void UISailManage::buttonEvent(Ref* target,Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* w_button = dynamic_cast<Widget*>(target);
	std::string name = w_button->getName();
	buttonEventName(w_button,name);
}

void UISailManage::buttonEventName(Widget* target,std::string name)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_ok))//确认回城
	{
		if(m_bIsBlock){
			return;
		}
		m_bIsBlock = true;
		auto portsInfo = SINGLE_SHOP->getCitiesInfo();
		auto portInfo = portsInfo[m_nCityId];
		
		ProtocolThread::GetInstance()->reachCity(portInfo.dock_x,portInfo.dock_y,UILoadingIndicator::create(this,4));
		return;
	}
	if (isButton(button_cancel))//关闭界面
	{
		auto parent = target->getParent()->getParent();
		parent->removeFromParent();
		return;
	}
	
	if (isButton(button_city))//地图上的城市按钮
	{
		int cityId = target->getTag() - CONST_VALUE;
		auto portsInfo = SINGLE_SHOP->getCitiesInfo();
		auto portInfo = portsInfo[cityId + 1];
		int city_x = portInfo.x;// portInfo.dock_x
		int city_y = portInfo.y;//portInfo.dock_y
		if (m_pShipImage->getPosition().getDistance(Vec2(city_x,city_y)) < LAND_DISTANCE)
		{
			m_nCityId = cityId + 1;
			openDialog();
		}else
		{
			if (m_pShipImage)
			{
				UIShip* ship = dynamic_cast<UIShip*>(m_pShipImage);
				ship->setGoldPosition(target->getPosition());
				ship->m_IsAutoSailing = false;
			}
		}
		return;
	}

	if (isButton(button_boss))
	{
		dynamic_cast<UISailHUD*>(m_pMapUI)->openAttactPirate(target->getTag());
		return;
	}
}

void UISailManage::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_CompleteMainTaskResult:
		{
			CompleteMainTaskResult* result = (CompleteMainTaskResult*)message;
			if(result->failed == 0 && result->shouldreachcity){
				ProtocolThread::GetInstance()->reachCity(-1,-1,UILoadingIndicator::createWithMask(this,4));
			}
			break;
		}
	case PROTO_TYPE_GetSailInfoResult:
		{
			GetSailInfoResult*sailInfo = (GetSailInfoResult *)message;
			LeaveCityResult *result = sailInfo->seadata;
			if (result && sailInfo->failed == 0)
			{
				m_captain_skill_outlookLv = result->captain_skill_outlook;
				m_nearbyShipManager->reportShipPosition(result->players,result->n_players);

				m_bIsLeaveCityTosea=true;
				m_npcIsActive = 30.0;
				m_lastNPCEventID = sailInfo->lastfighteventid;
				m_pLeaveCityResult = result;
				//从个人中心回来，算作一天
				if (SINGLE_HERO->m_centerTosea)
				{
					SINGLE_HERO->m_centerTosea = false;
					m_nSailDays = sailInfo->saildays + 1;
				}
				else
				{
					m_nSailDays = sailInfo->saildays;
				}
			
				SINGLE_HERO->m_iNwantedforceids = sailInfo->n_wantedforceids;
				SINGLE_HERO->m_iWantedforceids = sailInfo->wantedforceids;
				SINGLE_HERO->m_heroIsOnsea = true;
				SINGLE_HERO->m_forceNPCId = result->fightnpcid;
				if (result->fightnpcid>0)
				{
					SINGLE_HERO->m_lastBattleNotComplete = true;
					SINGLE_HERO->m_isShowOutLineResult = false;
				}
				initNetworkData(result);
		
				if (UserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO) == OPEN_AUDIO)
				{
					playAudio();
				}
				//add mew sound
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_MEW_06);
				dynamic_cast<UISailHUD*>(m_pMapUI)->m_nEventversion = result->blockinfo->eventversion;
				dynamic_cast<UISailHUD*>(m_pMapUI)->showLootButton(result->delegateactive);
				dynamic_cast<UISailHUD*>(m_pMapUI)->onServerEvent(message, msgType);
			}
			else
			{
				if (result)
				{
					openSuccessOrFailDialog("TIP_LEAVE_CITY_FAIL");
				}
			}
			break;
		}
	case PROTO_TYPE_LeaveCityResult:
		{
			LeaveCityResult *result = (LeaveCityResult *)message;
			if (result && result->failed == 0)
			{
				m_captain_skill_outlookLv = result->captain_skill_outlook;
				m_nearbyShipManager->reportShipPosition(result->players,result->n_players);
				m_bIsLeaveCityTosea=true;
				m_pLeaveCityResult = result;
				SINGLE_HERO->m_iNwantedforceids = result->n_wantedforceids;
				SINGLE_HERO->m_iWantedforceids = result->wantedforceids;
				SINGLE_HERO->m_heroIsOnsea = true;

				initNetworkData(result);
				if (UserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO) == OPEN_AUDIO)
				{
					playAudio();
				}
 			
				//add mew sound
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_MEW_06);
				dynamic_cast<UISailHUD*>(m_pMapUI)->m_nEventversion = result->blockinfo->eventversion;
				dynamic_cast<UISailHUD*>(m_pMapUI)->showLootButton(result->delegateactive);
			}else
			{
				if (result)
				{
					openSuccessOrFailDialog("TIP_LEAVE_CITY_FAIL");
				}
			}
			break;
		}
	case PROTO_TYPE_ReachCityResult:
		{
			ReachCityResult *result = (ReachCityResult*)message;
			if (result && result->failed == 0)
			{		
				m_pReachCityResult=result;
				SINGLE_HERO->m_iExp = result->exp;
				SINGLE_HERO->m_iRexp = result->fame;
			
				if (result->newlevel)
				{
					m_HeroExpLevelUp = true;
					SINGLE_HERO->m_iLevel = result->newlevel;						
				}
				if (result->newfamelevel)
				{
					m_HeroFameLevelUp = true;
					SINGLE_HERO->m_iPrestigeLv = result->newfamelevel;								
				}
				dynamic_cast<UISailHUD*>(m_pMapUI)->setPlayerLv(SINGLE_HERO->m_iLevel, SINGLE_HERO->m_iPrestigeLv);
				dynamic_cast<UISailHUD*>(m_pMapUI)->setExpAndFame(result->exp, result->failed);
				ProtocolThread::GetInstance()->unregisterMessageCallback(this);
				if(m_bFailedAutoFight || result->maintaskflag)
				{
					this->button_callBack();
				}else
				{
					if (result->addedexp>=0)
					{
						if (result->n_captains)//有船长
						{
							for (int i=0;i<result->n_captains;i++)
							{
								if (result->captains[i]->lootexp>0)
								{
									m_bIsCaptainUp = true;
									break;
								}
								else
								{
									m_bIsCaptainUp = false;
								}
							}
							if (m_bIsCaptainUp)//显示船长增加经验界面
							{
								m_bIsCaptainUp = false;
								m_bCaptainToHeroUI=true;
								UICommon::getInstance()->openCommonView(m_pMapUI);
								UICommon::getInstance()->getCaptainAddexp();
								UICommon::getInstance()->flushCaptainLevelUp(result->n_captains,result->captains);
							}
							else
							{
								openCountUI(result);
							}
						}
						else//没船长
						{
							if (m_HeroExpLevelUp)
							{
								m_HeroExpLevelUp = false;
								UICommon::getInstance()->openCommonView(m_pMapUI);
								UICommon::getInstance()->flushPlayerLevelUp();
							}
							if (m_HeroFameLevelUp)
							{							
								if (!m_HeroExpLevelUp)
								{
									m_HeroFameLevelUp = false;
									UICommon::getInstance()->openCommonView(m_pMapUI);
									UICommon::getInstance()->flushPrestigeLevelUp();
								}
							}
							openCountUI(result);
						}				
					}
					else
					{
						button_callBack();
					}
				}		
			}else
			{
				openSuccessOrFailDialog("TIP_VIST_CITY_FAIL");
				dynamic_cast<UISailHUD*>(m_pMapUI)->shipPause(false);
			}
			m_bIsBlock = false;
			break;
		}
	case PROTO_TYPE_GetNextMapBlockResult:
		{
			GetNextMapBlockResult* result = (GetNextMapBlockResult*)message;
			if (result && result->failed == 0)
			{
				m_nearbyShipManager->reportShipPosition(result->players,result->n_players);
				//最近离开的有许可证的城市
				SINGLE_HERO->m_iHave_licence_cityId = result->last_safe_city;
				int curday = result->days;
				int sailors = result->totalsailors;
				int supply = result->currentsupply;
				int maxSupply = result->maxsupply;
				int brokeEquipnum = result->n_brokenequips;				
				auto mapUI = dynamic_cast<UISailHUD*>(m_pMapUI);

				mapUI->setShipPositon(m_pShipImage->getPosition());
				mapUI->setExpAndFame(result->exp, result->fame);
				m_pShipImage->setSpeed(result->sail_speed);
				
				//当补给耗尽水手开始被消耗时，显示每秒损失的水手
				if (mapUI->m_nCurSupplies <= 0)
				{
					if (mapUI->m_nCurCrewNum != sailors)
					{
						int deidSailors = mapUI->m_nCurCrewNum - sailors;
						dynamic_cast<UIShip*>(m_pShipImage)->sailorsDieAnimation(deidSailors);
					}
					
				}
				mapUI->getCurrSailorsNum(sailors);
				mapUI->setCrew(sailors);
				if (sailors < 1)
				{
					openFailureDialog();
					mapUI->setShipEquipBrokenLocal(result->n_brokenequips,result->brokenequips);
					dynamic_cast<UIShip*>(m_pShipImage)->stopShip(true);
					m_pShipImage->unscheduleUpdate();
					m_MapsLayer->stopAllActions();
					m_pShipImage->stopAllActions();				
				}
				
				if (result->eventchanged)
				{
					m_EventLayer->removeAllChildren();
					dynamic_cast<UISailHUD*>(m_pMapUI)->m_nEventversion = result->blockinfo->eventversion;
					addSeaEvent_nextBlock(result->blockinfo);
				}
				//当补给为0时，服务器会把最大水手和最大补给改为0
				if (0 == result->maxsailornum){
					//mapUI->m_nCurSupplies = 0;
				}
				else{
					mapUI->setSupplies(supply,maxSupply);
				}

				if (result->newlevel)
				{
					SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->fame);
					mapUI->setPlayerLv(result->newlevel, SINGLE_HERO->m_iPrestigeLv);
					mapUI->setExpAndFame(result->exp,result->fame);
					SINGLE_HERO->m_iLevel = result->newlevel;
					UICommon::getInstance()->openCommonView(m_pMapUI);
					UICommon::getInstance()->flushPlayerLevelUp();
				}
				
				//主线任务完成对话
				if (MainTaskManager::GetInstance()->m_PartsInfo)
				{
					auto taskPosX = MainTaskManager::GetInstance()->m_PartsInfo->x;
					auto taskPosY = MainTaskManager::GetInstance()->m_PartsInfo->y;
					if (taskPosX&&taskPosY)
					{
						Vec2 shipPos(m_pShipImage->getPosition());
						Vec2 taskPos(taskPosX, taskPosY);
						auto TaskTipDistance = shipPos.distance(taskPos);
						auto a = MainTaskManager::GetInstance()->m_PartsInfo->r;
						if (TaskTipDistance<800 && TaskTipDistance> MainTaskManager::GetInstance()->m_PartsInfo->r)
						{
							if ((!m_EventLayer->getChildByName("imageSeaTaskTipBg")) && (!m_EventLayer->getChildByName("imageSeaTask")))
							{		
								imageSeaTaskTipBg = Sprite::create();
								imageSeaTaskTipBg->setTexture("res/position_icon/quest_bg.png");
								imageSeaTask = Sprite::create();
								imageSeaTask->setTexture("res/position_icon/main_quest.png");
								imageSeaTaskTipBg->setVisible(false);
								imageSeaTask->setVisible(false);
								imageSeaTaskTipBg->setName("imageSeaTaskTipBg");
								imageSeaTask->setName("imageSeaTask");
								m_EventLayer->addChild(imageSeaTaskTipBg);
								m_EventLayer->addChild(imageSeaTask);
								imageSeaTaskTipBg->setVisible(true);
								imageSeaTask->setVisible(true);
								imageSeaTaskTipBg->setPosition(taskPos);
								imageSeaTask->setPosition(taskPos);
								imageSeaTaskTipBg->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1.0f), FadeIn::create(1.0f), nullptr)));
								imageSeaTask->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1.0f), FadeIn::create(1.0f), nullptr)));
							}
						}
						
					}

					if (MainTaskManager::GetInstance()->checkTaskAction() && !MainTaskManager::GetInstance()->getIsTriggering())
					{
						if (imageSeaTaskTipBg)
						{
							m_EventLayer->removeChildByName("imageSeaTaskTipBg");
							imageSeaTaskTipBg = nullptr;
						}

						if (imageSeaTask)
						{
							m_EventLayer->removeChildByName("imageSeaTask");
							imageSeaTask = nullptr;
						}

						MainTaskManager::GetInstance()->setIsTriggering(true);
						addNeedControlWidgetForHideOrShow(nullptr, true);
						allTaskInfo(false, MAIN_STORY);
						MainTaskManager::GetInstance()->notifyUIPartDelayTime(0);
						dynamic_cast<UISailHUD*>(m_pMapUI)->shipPause(true);
					}
				}

				
				//小伙伴剧情任务完成对话
				if (CompanionTaskManager::GetInstance()->m_PartsInfo)
				{
					auto taskPosX = CompanionTaskManager::GetInstance()->m_PartsInfo->x;
					auto taskPosY = CompanionTaskManager::GetInstance()->m_PartsInfo->y;
					if (taskPosX&&taskPosY)
					{
						imageSeaCompanionsTaskTipBg = Sprite::create();
						imageSeaCompanionsTaskTipBg->setTexture("res/position_icon/quest_bg.png");
						imageSeaCompanionsTask = Sprite::create();
						imageSeaCompanionsTask->setTexture("res/position_icon/main_quest.png");
						imageSeaCompanionsTaskTipBg->setVisible(false);
						imageSeaCompanionsTask->setVisible(false);
						imageSeaCompanionsTaskTipBg->setName("imageSeaCompanionsTaskTipBg");
						imageSeaCompanionsTask->setName("imageSeaCompanionsTask");

						Vec2 shipPos(m_pShipImage->getPosition());
						Vec2 taskPos(taskPosX, taskPosY);
						auto TaskTipDistance = shipPos.distance(taskPos);
						auto a = CompanionTaskManager::GetInstance()->m_PartsInfo->r;
						if (TaskTipDistance<800 && TaskTipDistance> CompanionTaskManager::GetInstance()->m_PartsInfo->r)
						{
							if ((!m_EventLayer->getChildByName("imageSeaCompanionsTaskTipBg")) && (!m_EventLayer->getChildByName("imageSeaCompanionsTask")))
							{
								m_EventLayer->addChild(imageSeaCompanionsTaskTipBg);
								m_EventLayer->addChild(imageSeaCompanionsTask);
								imageSeaCompanionsTaskTipBg->setVisible(true);
								imageSeaCompanionsTask->setVisible(true);
								imageSeaCompanionsTaskTipBg->setPosition(taskPos);
								imageSeaCompanionsTask->setPosition(taskPos);
								imageSeaCompanionsTaskTipBg->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1.0f), FadeIn::create(1.0f), nullptr)));
								imageSeaCompanionsTask->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1.0f), FadeIn::create(1.0f), nullptr)));
							}
						}
						
					}

					if (CompanionTaskManager::GetInstance()->checkTaskAction() && !CompanionTaskManager::GetInstance()->getIsTriggering())
					{
						CompanionTaskManager::GetInstance()->setIsTriggering(true);
						if (imageSeaCompanionsTaskTipBg)
						{
							m_EventLayer->removeChildByName("imageSeaCompanionsTaskTipBg");
							imageSeaCompanionsTaskTipBg = nullptr;
						}

						if (imageSeaCompanionsTask)
						{
							m_EventLayer->removeChildByName("imageSeaCompanionsTask");
							imageSeaCompanionsTask = nullptr;
						}
						addNeedControlWidgetForHideOrShow(nullptr, true);
						allTaskInfo(false, COMPANION_STORY);
						CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(0);
						dynamic_cast<UISailHUD*>(m_pMapUI)->shipPause(true);
					}
				}
			}else
			{
				log("Update Position fail:%d",result->failed);
			}
			break;
		}
	case PROTO_TYPE_EndFightResult:
		{
			m_pMapUI->onServerEvent(message,msgType);
			EndFightResult* result = (EndFightResult*)message;
			if(result->failed == 0 && result->isautofight){
				if(result->reason == 1){
					addSeaEvent_nextBlock(result->seainfo);
				}else if(result->reason == 2){
					m_bFailedAutoFight = true;
				}
			}
			break;
		}
	case PROTO_TYPE_TriggerSeaEventResult:
		{
			TriggerSeaEventResult* result = (TriggerSeaEventResult*)message;
			if (result->failed == 0)
			{
				if(result->eventchanged)
				{
					addSeaEvent(result->seainfo->events,result->seainfo->n_events);
				}
				if (result->type == EV_SEA_PESTIS)
				{
					if (dynamic_cast<UISailHUD*>(m_pMapUI)->m_nCurSupplies <= 0)
					{
						dynamic_cast<UISailHUD*>(m_pMapUI)->m_bIsBlocking = false;
						seaEventEnd();
						break;
					}
					else
					{
						dynamic_cast<UISailHUD*>(m_pMapUI)->updateSeaEventView(result->type, INDEX_UI_FLOAT_CSB, result);
					}			
				}
				else
				{
					dynamic_cast<UISailHUD*>(m_pMapUI)->updateSeaEventView(result->type, INDEX_UI_FLOAT_CSB, result);
				}
				m_pShipImage->setSpeed(result->sail_speed);
			}else
			{
				seaEventEnd();
			}
			break;
		}
	case PROTO_TYPE_EngageInFightResult:
	case PROTO_TYPE_GetFleetAndDockShipsResult:
	case PROTO_TYPE_GetCityStatusResult:
	case PROTO_TYPE_AddDiscoveredCityResult:
	case PROTO_TYPE_StartSalvageResult:
	case PROTO_TYPE_SalvageResult:
	case PROTO_TYPE_GetSalvageConditionResult:
	case PROTO_TYPE_CheckMailBoxResult:
	case PROTO_TYPE_GetAttackPirateInfoResult:
	case PROTO_TYPE_FindLootPlayerResult:
	case PROTO_TYPE_LootPlayerResult:
	case PROTO_TYPE_InviteUserToGuildResult:
	case PROTO_TYPE_GetUserInfoByIdResult:
	case PROTO_TYPE_FriendsOperationResult:
	case PROTO_TYPE_CancelFightResult:
	case PROTO_TYPE_EndFailedFightByVTicketResult:
	case PROTO_TYPE_ExpandPackageSizeResult:
	case PROTO_TYPE_GetMyNationWarResult:
		{
			dynamic_cast<UISailHUD*>(m_pMapUI)->onServerEvent(message, msgType);
			break;
		}
	default:
		break;
	}
}

void UISailManage::openCountUI(ReachCityResult* result)
{
	//iLoongV 登录结算画面
	dynamic_cast<UISailHUD*>(m_pMapUI)->openView(MAPUI_COCOS_RES[INDEX_UI_BACK_CITY_CSB]);
	Widget* panel_back_result = dynamic_cast<UISailHUD*>(m_pMapUI)->getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BACK_CITY_CSB]);
	panel_back_result->setCameraMask(4,true);//摄像机

	Widget* panel_addexp = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_addexp"));

	Widget* panel_total = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_total"));
	Widget* panel_warehouse = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_warehouse"));
	Widget* panel_hosted = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_hosted"));
	Widget* panel_captain = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_captain"));

	Widget* panel_exp = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_exp"));
	Widget* panel_rep = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_rep"));

	ImageView* image_silver_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_total, "image_silver_1"));
	ImageView* image_silver_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_warehouse, "image_silver_2"));
	ImageView* image_silver_3 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_hosted, "image_silver_3"));
	ImageView* image_silver_4 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_captain, "image_silver_4"));

	
	Text* label_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv"));
	ImageView* image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_addexp, "image_head"));
	Widget* panel_fee = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_fee"));
	Text* label_total_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fee, "label_total_num"));
	Text* label_warehouse_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fee, "label_warehouse_num"));
	Text* label_hosted_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fee, "label_hosted_num"));
	Text* label_daily_expenses_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fee, "label_daily_expenses_num"));
	auto  t_rep_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv_r"));

	Widget* panel_ship_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_1"));
	Widget* panel_ship_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_2"));
	Widget* panel_ship_3 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_3"));
	Widget* panel_ship_4 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_4"));
	Widget* panel_ship_5 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_5"));


	//int sailDay = m_SailDays;
	int sailDay =result->sailingdays ;
	int wareFee = sailDay * result->warehousedailycost;
	int captainFee = sailDay * result->captainsdailycost;
	int dockFee = sailDay * result->dockdailycost;
	int totalFee = wareFee + captainFee + dockFee;
	long int exp = result->exp;
	int levle = EXP_NUM_TO_LEVEL(exp);
	long int fame=result->fame;
	int fameLevel = FAME_NUM_TO_LEVEL(fame);

	float temp_exp = 0;
	if (levle < LEVEL_MAX_NUM)
	{
		temp_exp = (exp - LEVEL_TO_EXP_NUM(levle))*1.0/(LEVEL_TO_EXP_NUM(levle+1)-LEVEL_TO_EXP_NUM(levle));
	}
	float temp_rep = 0;
	if (fameLevel < LEVEL_MAX_NUM)
	{
		temp_rep = (fame - LEVEL_TO_FAME_NUM(fameLevel))*1.0/(LEVEL_TO_FAME_NUM(fameLevel+1)-LEVEL_TO_FAME_NUM(fameLevel));
	}

	panel_exp->setContentSize(Size(panel_exp->getContentSize().width, 120 * temp_exp));
	panel_rep->setContentSize(Size(panel_rep->getContentSize().width, 120 * temp_rep));
	
	//add exp and rep
	auto p_exp = panel_addexp->getChildByName<Widget*>("panel_exp");
	auto p_rep = panel_addexp->getChildByName<Widget*>("panel_r");
	auto t_exp = p_exp->getChildByName<Widget*>("label_exp");
	auto i_rep = p_rep->getChildByName<Widget*>("image_r");
	auto label_exp_num = p_exp->getChildByName<Text*>("label_exp_num");
	label_exp_num->setString(StringUtils::format("+%d", result->addedexp));
	auto label_r_num = p_rep->getChildByName<Text*>("label_r_num");
	label_r_num->setString(StringUtils::format("+%d", result->addedfame));
	t_exp->setPositionX(p_exp->getBoundingBox().size.width / 2 - label_exp_num->getBoundingBox().size.width / 2 - 10);
	label_exp_num->setPositionX(t_exp->getPositionX() + t_exp->getBoundingBox().size.width / 2 + 20);
	i_rep->setPositionX(p_rep->getBoundingBox().size.width / 2 - label_r_num->getBoundingBox().size.width / 2 - 10);
	label_r_num->setPositionX(i_rep->getPositionX() + i_rep->getBoundingBox().size.width / 2 + 20);

	std::string str_lv = StringUtils::format("%d ", levle);

	std::string iconPath = StringUtils::format("res/player_icon/icon_%d.png", SINGLE_HERO->m_iIconidx);
	image_head->loadTexture(iconPath);
	std::string str_total_num = StringUtils::format("%d", totalFee);
	label_total_num->setString(numSegment(str_total_num));

	std::string str_warehouse_num = StringUtils::format("%d", wareFee);
	label_warehouse_num->setString(numSegment(str_warehouse_num));

	std::string str_hosted_num = StringUtils::format("%d", dockFee);
	label_hosted_num->setString(numSegment(str_hosted_num));

	std::string str_daily_expenses_num = StringUtils::format("%d", captainFee);
	label_daily_expenses_num->setString(numSegment(str_daily_expenses_num));

	std::string str_rep_lv = StringUtils::format("%d", fameLevel);
	label_lv->setString(str_lv);
	t_rep_lv->setString(str_rep_lv);
	flashIconPosXWithText(label_total_num, image_silver_1);
	flashIconPosXWithText(label_warehouse_num, image_silver_2);
	flashIconPosXWithText(label_hosted_num, image_silver_3);
	flashIconPosXWithText(label_daily_expenses_num, image_silver_4);

	for (int i = 1; i <= 5; i++){
	
		std::string ship_name = StringUtils::format("panel_ship_%d", i);
		Widget* panel_ship = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, ship_name));
		panel_ship->setVisible(false);
	
	}

	for (int i = 1; i <= result->n_fleetships; i++)
	{
		ShipDefine* sd = result->fleetships[i-1];

		std::string ship_name = StringUtils::format("panel_ship_%d", i);
		Widget* panel_ship = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, ship_name));
		auto image_ship_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_ship, "image_ship_bg"));
		ImageView* image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_ship_bg, "image_item_bg_lv"));
		ImageView* image_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "image_ship"));
		ImageView* image_num = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "image_num"));
		std::string num_name = getPositionIconPath(i);
		image_num->loadTexture(num_name);
		std::string filePath = getShipIconPath(sd->sid);
		image_ship->loadTexture(filePath);
		image_ship->setCameraMask(4,true);
		Text* label_ship_durable_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_1"));
		Text* label_ship_durable_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_2"));
		Text* label_ship_durable_num_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_3"));
		LoadingBar* progressbar_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_ship, "progressbar_durable"));
		std::string str_durable_num_1 = StringUtils::format("%d",sd->hp);
		std::string str_durable_num_3 = StringUtils::format("%d",sd->current_hp_max);
		std::string str_durable_num;

		label_ship_durable_num_1->setText(str_durable_num_1);
		label_ship_durable_num_2->setText("/");
		label_ship_durable_num_3->setText(str_durable_num_3);
		label_ship_durable_num_1->setPositionX(label_ship_durable_num_2->getPositionX() - label_ship_durable_num_1->getBoundingBox().size.width / 2 - 5);
		label_ship_durable_num_3->setPositionX(label_ship_durable_num_2->getPositionX() + label_ship_durable_num_3->getBoundingBox().size.width / 2 + 5);

		setBgButtonFormIdAndType(image_ship_bg, sd->sid, ITEM_TYPE_SHIP);
		setBgImageColorFormIdAndType(image_item_bg_lv, sd->sid, ITEM_TYPE_SHIP);
		if (0 == sd->current_hp_max){
		
			progressbar_durable->setPercent(0);
		
		}
		else{
			
			progressbar_durable->setPercent(sd->hp * 100 / sd->hp_max);
		
		}

		ImageView* Image_17 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "Image_17"));
		int chp = sd->current_hp_max;
		if (chp == sd->hp_max){
		
/*			str_durable_num = str_durable_num_1 + "/" + str_durable_num_3;*/
			Image_17->setVisible(false);
		}
		else
		{		
				Image_17->setPositionX(progressbar_durable->getSize().width * chp / sd->hp_max);
				Image_17->setVisible(true);
				Image_17->setLocalZOrder(4);
// 				str_durable_num = str_durable_num_1 + "/" + str_durable_num_3;
//         		str_durable_num = "<html>" + str_durable_num_1 + "/" + "<font size ='20' color ='#B71C1C'>" + str_durable_num_3 + "</font></html>";
				label_ship_durable_num_3->setTextColor(Color4B(183, 28, 28, 255));
		}

		Text* label_ship_sailor_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_sailor_num_1"));
		Text* label_ship_sailor_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_sailor_num_2"));
		Text* label_ship_sailor_num_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_sailor_num_3"));
		LoadingBar* progressbar_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_ship, "progressbar_sailor"));

		if (0 == sd->max_crew_num){

			progressbar_sailor->setPercent(0);

		}
		else{

			progressbar_sailor->setPercent(sd->crew_num * 100 / sd->max_crew_num);

		}
		std::string str_sailor_num_1 = StringUtils::format("%d", sd->crew_num);
		std::string str_sailor_num_3 = StringUtils::format("%d", sd->max_crew_num);
//		label_ship_sailor_num_1->setString(str_sailor_num_1);
		label_ship_sailor_num_2->setString(str_sailor_num_1 + "/" + str_sailor_num_3);
//		label_ship_sailor_num_3->setString(str_sailor_num_3);

		panel_ship->setVisible(true);
		auto btn = Helper::seekWidgetByName(panel_back_result,"button_landCity");
		btn->setTouchEnabled(false);
		btn->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), CallFunc::create([=]{btn->setTouchEnabled(true); })));
	}
}

void UISailManage::GoBackCity()
{
	this->button_callBack();
}

void UISailManage::loadMapLine()
{
	if (!m_MapsLayer)
	{
		return;
	}
	Point curPos = m_MapsLayer->getPosition();
	Point diffPos = ccpAdd(curPos,m_PrePoint);
	
	// map direct is opposite
	if (diffPos.x > MAP_CELL_WIDTH)
	{
		loadMapLine(UIShip::LEFT);
	}
	if (diffPos.x < -MAP_CELL_WIDTH)
	{
		loadMapLine(UIShip::RIGHT);
	}
	if (diffPos.y > MAP_CELL_HEIGH)
	{
		loadMapLine(UIShip::DOWN);
	}
	if (diffPos.y < -MAP_CELL_HEIGH)
	{
		loadMapLine(UIShip::UP);
	}
	m_PrePoint = Vec2(m_ScreenFirstPos.x * MAP_CELL_WIDTH,m_ScreenFirstPos.y * MAP_CELL_HEIGH);
}

void UISailManage::loadMapLine(int direct)
{
	std::string loadFileName;
	switch (direct)
	{
	case UIShip::UP:
		{
			m_ScreenFirstPos.y++;
			
			int tempY = m_ScreenFirstPos.y + 3 * SCREEN_HEIGH_NUM /2;
			int tempX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM/2;
			int lastY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM/2 - 1;
			int lastX = tempX;
			for (int i = 0; i < SCREEN_WIDTH_NUM * 2; i++)
			{
				auto iter = m_RenderSprites.find((lastX + i) * CONST_VALUE + lastY);
				if (iter != m_RenderSprites.end())
				{
					auto cell = iter->second;
					auto seaCell = dynamic_cast<Sprite*>(cell->getUserObject());
					std::string fileName = getCellName(tempX + i,tempY);
					std::string seaName = getSeaCellName(tempX + i,tempY);
					asyncLoadingImage(cell,seaCell,fileName,seaName);
					cell->setPosition((tempX + i) *MAP_CELL_WIDTH,tempY * MAP_CELL_HEIGH);
					m_RenderSprites.insert(std::pair<int,Sprite*>((tempX + i) * CONST_VALUE + tempY,cell));
					m_RenderSprites.erase(iter);
					
				}else
				{
					std::string fileName = getCellName(tempX,tempY + i);
					log("Up,not find:%d,%s",i,fileName.c_str());
				}	
			}
			
			break;
		}
	case UIShip::DOWN:
		{
			m_ScreenFirstPos.y--;
			int tempY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM /2;
			int tempX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM/2;
			int lastY = m_ScreenFirstPos.y + 3 * SCREEN_HEIGH_NUM /2 + 1;
			int lastX = tempX;
			for (int i = 0; i < SCREEN_WIDTH_NUM * 2; i++)
			{	
				auto iter = m_RenderSprites.find((lastX + i) * CONST_VALUE + lastY);
				if (iter != m_RenderSprites.end())
				{
					auto cell = iter->second;
					auto seaCell = dynamic_cast<Sprite*>(cell->getUserObject());
					std::string fileName = getCellName(tempX + i,tempY);
					std::string seaName = getSeaCellName(tempX + i,tempY);
					
					asyncLoadingImage(cell,seaCell,fileName,seaName);
					cell->setPosition((tempX + i) *MAP_CELL_WIDTH,tempY * MAP_CELL_HEIGH);
					m_RenderSprites.insert(std::pair<int,Sprite*>((tempX + i) * CONST_VALUE + tempY,cell));
					m_RenderSprites.erase(iter);
					//auto label = dynamic_cast<Label*>(seaCell->getChildByTag(1));
					//label->setString(seaName.substr(seaName.find_last_of('/')));
				}else
				{
					std::string fileName = getCellName(tempX,tempY + i);
					log("Down,not find:%d,Name:%s",i,fileName.c_str());
				}
			}
			
			break;
		}
	case UIShip::LEFT:
		{
			m_ScreenFirstPos.x--;

			int tempY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM /2;
			int tempX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM/2;
			int lastY = tempY;
			int lastX = m_ScreenFirstPos.x + 3 * SCREEN_WIDTH_NUM/2;
			for (int i = 0; i < SCREEN_HEIGH_NUM * 2; i++)
			{
				auto iter = m_RenderSprites.find(lastX * CONST_VALUE + lastY + i);
				if (iter != m_RenderSprites.end())
				{
					auto cell = iter->second;
					auto seaCell = dynamic_cast<Sprite*>(cell->getUserObject());
					std::string fileName = getCellName(tempX,tempY + i);
					std::string seaName = getSeaCellName(tempX,tempY + i);
					asyncLoadingImage(cell,seaCell,fileName,seaName);
					m_RenderSprites.insert(std::pair<int,Sprite*>(tempX * CONST_VALUE + i + tempY,cell));
					m_RenderSprites.erase(iter);
					
					cell->setPosition(tempX *MAP_CELL_WIDTH,(tempY + i) * MAP_CELL_HEIGH);
					//auto label = dynamic_cast<Label*>(seaCell->getChildByTag(1));
					//label->setString(seaName.substr(seaName.find_last_of('/')));
				}else
				{
					std::string fileName = getCellName(tempX,tempY + i);
					log("Left,not find:%d,Name:%s",i,fileName.c_str());
					
				}
			}
			
			break;
		}
	case UIShip::RIGHT:
		{
			m_ScreenFirstPos.x++;
			
			int tempY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM /2;
			int tempX = m_ScreenFirstPos.x + 3 * SCREEN_WIDTH_NUM/2 - 1;
			int lastY = tempY;
			int lastX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM/2 - 1;
			for (int i = 0; i < SCREEN_HEIGH_NUM * 2; i++)
			{	
				auto iter = m_RenderSprites.find(lastX * CONST_VALUE + lastY + i);
				if (iter != m_RenderSprites.end())
				{
					auto cell = iter->second;
					auto seaCell = dynamic_cast<Sprite*>(cell->getUserObject());
					
					std::string fileName = getCellName(tempX,tempY + i);
					std::string seaName = getSeaCellName(tempX,tempY + i);
					
					asyncLoadingImage(cell,seaCell,fileName,seaName);
					auto texture = cell->getTexture();
					cell->setPosition(tempX *MAP_CELL_WIDTH,(tempY + i) * MAP_CELL_HEIGH);
					m_RenderSprites.insert(std::pair<int,Sprite*>(tempX * CONST_VALUE + i + tempY,cell));
					m_RenderSprites.erase(iter);
					//auto label = dynamic_cast<Label*>(seaCell->getChildByTag(1));
					//label->setString(seaName.substr(seaName.find_last_of('/')));
					
				}else
				{
					std::string fileName = getCellName(tempX,tempY + i);
					log("Right,not find:%d,Name:%s",i,fileName.c_str());
				}
			}
			
			break;
		}
	default:
		break;
	}
}

void UISailManage::asyncLoadingImage(Sprite* land,Sprite* sea,std::string& landName,std::string& seaName)
{

	for (auto iter = m_UnLoadImage_sea.begin();iter != m_UnLoadImage_sea.end();)
	{
		if((*iter)->getTag() == -10)
		{
			(*iter)->setTag(-100);
			iter = m_UnLoadImage_sea.erase(iter);
		}else
		{
			iter++;
		}
	}
	for (auto iter = m_UnLoadImage_land.begin();iter != m_UnLoadImage_land.end();)
	{
		if((*iter)->getTag() == -10)
		{
			(*iter)->setTag(-100);
			iter = m_UnLoadImage_land.erase(iter);
		}else
		{
			iter++;
		}
	}
	int land_tag = atoi(landName.substr(landName.find_last_of('/') + 1,landName.find('.') - landName.find_last_of('/') - 1).c_str());
	int sea_tag = atoi(seaName.substr(seaName.find_last_of('/') + 1,seaName.find('.') - seaName.find_last_of('/') - 1).c_str());
	land->setTag(land_tag);
	auto landTexture = TextureCache::getInstance()->getTextureForKey(landName);
	
	if (landTexture)
	{
		land->setTexture(landTexture);
	}else
	{
		m_UnLoadImage_land.push_back(land);
		TextureCache::getInstance()->addImageAsync(landName,CC_CALLBACK_1(UISailManage::texture2DCallback_land,this));	
	}
#if 0
	auto seaTexture = TextureCache::getInstance()->getTextureForKey(seaName);
	if (seaTexture)
	{
		//sea->setTexture(seaTexture);
	}else
	{
		//m_UnLoadImage_sea.push_back(sea);
		
		//TextureCache::getInstance()->addImageAsync(seaName,CC_CALLBACK_1(Maps::texture2DCallback_sea,this));
	}
#endif
	//sea->setTexture(seaName);
	//land->setTexture(landName);
}

void UISailManage::texture2DCallback_land(Texture2D* texture)
{
	for (auto iter = m_UnLoadImage_land.begin();iter != m_UnLoadImage_land.end();)
	{
		if ((*iter)->getTag() == -10 || (*iter)->getTag() == -100)
		{
			(*iter)->setTag(-100);
			iter = m_UnLoadImage_land.erase(iter);
		}
		else
		{
			std::string landName = StringUtils::format("res/map_cell/land/%d.png", (*iter)->getTag());
			auto _texture = TextureCache::getInstance()->getTextureForKey(landName);
			if (_texture)
			{
				(*iter)->setTexture(_texture);
				(*iter)->setTag(-10);
				(*iter)->setDirty(true);
			}
			iter++;
		}
	}
}
void UISailManage::texture2DCallback_sea(Texture2D* texture)
{
	/*for (auto iter = m_UnLoadImage_sea.begin();iter != m_UnLoadImage_sea.end();iter++)
	{

	std::string seaName = StringUtils::format("res/map_cell/sea/%d.png",(*iter)->getTag());
	auto _texture = TextureCache::getInstance()->getTextureForKey(seaName);
	if (_texture)
	{
	(*iter)->setTexture(_texture);
	(*iter)->setTag(-10);
	}
	}*/
}

void UISailManage::openDialog()
{
	ProtocolThread::GetInstance()->getCityStatus(m_nCityId,UILoadingIndicator::createWithMask(this,4));
}

void UISailManage::openFailureDialog()
{
	if (m_bIsBlock){
		return;
	}
	m_bIsBlock = true;
	seaEventStart();
	dynamic_cast<UISailHUD*>(m_pMapUI)->openSaillingDialog(1);
}

void UISailManage::setCityId(int8_t cityId)
{
	m_nCityId = cityId;
}

bool UISailManage::onTouchBegan1(Touch *touch, Event *unused_event)
{
	Vec2 pos = m_MapsLayer->convertToNodeSpace(touch->getLocation());
	//log("touchPos:%f,%f",pos.x,pos.y);
	TextureCache::getInstance()->removeUnusedTextures();
	return true;
}

void UISailManage::onTouchCancelled1(Touch *touch, Event *unused_event)
{
	
}

void UISailManage::onTouchEnded1(Touch *touch, Event *unused_event)
{
	if (dynamic_cast<UISailHUD*>(m_pMapUI)->getIsSalvaging())
	{
		dynamic_cast<UISailHUD*>(m_pMapUI)->openIsStopSalvage();
		return;
	}
	
	Point touchPos = this->convertTouchToNodeSpace(touch);
	Point touchPosInMaps = m_MapsLayer->convertToNodeSpace(touchPos);
	dynamic_cast<TickIcon*>(m_pTouchIcon)->start();
	m_pTouchIcon->setPosition(touchPos);
	setShipDirect(touchPos);
	m_CurTouchPos = touchPos;
	if (m_pShipImage)
	{
		UIShip* ship = dynamic_cast<UIShip*>(m_pShipImage);
		ship->setGoldPosition(touchPosInMaps);
	}


	std::vector<UIShadowShip*> ships = m_nearbyShipManager->getGhostShips();
	if (ships.size() > 0)
	{
		for (auto ship : ships)
		{
			auto pos = ship->getPosition();
			auto rect = ship->getBoundingBox();
			auto size = ship->getContentSize();
			rect.origin.x -= size.width / 2;
			rect.origin.y -= size.height / 2;


			if (rect.containsPoint(touchPosInMaps))
			{
				if (dynamic_cast<UIShip*>(m_pShipImage)->m_IsAutoSailing)
				{
					dynamic_cast<UISailHUD*>(m_pMapUI)->shipPause(true);
					dynamic_cast<UIShip*>(m_pShipImage)->stopShip(true);
					dynamic_cast<UISailHUD*>(m_pMapUI)->m_nConfirmIndex = UISailHUD::CONFIRM_INDEX::CONFIRM_INDEX_GET_PLAYERINFO;
					int cid = ship->getCID();
					dynamic_cast<UISailHUD*>(m_pMapUI)->setPlayerInfoId(cid);
				}
				else
				{
					dynamic_cast<UISailHUD*>(m_pMapUI)->shipPause(true);
					int cid = ship->getCID();
					ProtocolThread::GetInstance()->getUserInfoById(cid, UILoadingIndicator::create(this));
				}
			}
		}
	}
}

void UISailManage::onTouchMoved1(Touch *touch, Event *unused_event)
{
	Point pos = this->convertTouchToNodeSpace(touch);
	
	Point diffPos = ccpSub(pos,pos);
	
	Point  tempPoint = m_MapsLayer->getPosition() + diffPos;
	if (tempPoint.x > 0)
	{
		tempPoint.x = 0;
	}
	if (tempPoint.y > 0)
	{
		tempPoint.y = 0;
	}
	if ( std::abs(tempPoint.x) > (MAP_WIDTH - SCREEN_WIDTH_NUM) * MAP_CELL_WIDTH)
	{
		tempPoint.x = -(MAP_WIDTH - SCREEN_WIDTH_NUM) * MAP_CELL_WIDTH;
	}
	if (std::abs(tempPoint.y) > (MAP_HEIGH - SCREEN_HEIGH_NUM) * MAP_CELL_HEIGH)
	{
		tempPoint.y = -(MAP_HEIGH - SCREEN_HEIGH_NUM) * MAP_CELL_HEIGH;
	}

}

void UISailManage::setShipDirect(Point pos)
{
	Point ship_pos = m_pShipImage->getPosition();
	float angle = ship_pos.getAngle(pos);
//	log("touch Pos Angle:%f",angle);
}

bool UISailManage::checkFrontBlock(Point pos)
{
	int x = pos.x/ MAP_CELL_WIDTH;
	int y = pos.y/ MAP_CELL_HEIGH;
	if (MAPINDEX[x][y] == 1)
	{
		return false;
	}
	return true;
}

int64_t UISailManage::getCurrentTimeUsev()
{
	timeval timeStruc;
	gettimeofday(&timeStruc,nullptr);
	int64_t sec = int64_t(timeStruc.tv_sec) * 1000;
	return sec + timeStruc.tv_usec/1000;
}

int countNodes(Node*node,int flag){
	if(node->getCameraMask() != flag){
		return 0;
	}

	if(node->getChildrenCount() == 0)
		return 1;
	int res = 0;
	auto & children = node->getChildren();
	for(auto & tmp:children){
		auto num = countNodes(tmp,flag);
		res += num;
	}
	return res;
}

void UISailManage::update(float f)
{
	long a = Utils::gettime();
	struct timeval start,end;
	gettimeofday(&start,0);

	int64_t curTime = getCurrentTimeUsev();
	int64_t time_val = curTime - m_nShipMoveTime;
	m_nearbyShipManager->update(f);
	m_pSeaManage->setCurrentDays_pos(m_nSailDays,m_pShipImage->getPosition());
	if (time_val > SHIP_MOVE_VAL)
	{
		m_nShipMoveTime = curTime;
		loadMapLine();
		Vec2 ship_pos = m_pShipImage->getPosition();
		checkGuardShip(ship_pos, f);
	}
	if (m_lastNPCEventID != 0)
	{
		m_npcIsActive -= f;
		if (m_npcIsActive < 0)
		{
			m_lastNPCEventID = 0;
		}
	}
	gettimeofday(&end,0);
	//log(" Maps::update : %d,mapui child count:%d",(end.tv_usec - start.tv_usec + (end.tv_sec-start.tv_sec)*1000000),countNodes(m_pMapUI));
	long b = Utils::gettime();
	//log("updata before:%d, updata end:%d, updata:%d", a, b, b - a);
	if (AppHelper::frameTime > 0.045)
	{
		log("UISailManage:%d, frameTime:%f, frameCount:%d", b - a, AppHelper::frameTime, AppHelper::frameCount);
	}
}

void UISailManage::addSeaEvents()
{
	if (m_pLeaveCityResult->blockinfo->n_events < 1)
	{
		return;
	}
	SeaEventDefine** events = m_pLeaveCityResult->blockinfo->events;
	
	Button* event_1 = Button::create(getShipTopIconPath(1));
	m_MapsLayer->addChild(event_1,10,100);
	event_1->setName("button_event_1");
	event_1->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, dynamic_cast<UISailHUD*>(m_pMapUI)));
	int x = m_pLeaveCityResult->blockinfo->events[0]->x;
	int y = MAP_CELL_HEIGH * MAP_WIDTH - m_pLeaveCityResult->blockinfo->events[0]->y;
	event_1->setTag(m_pLeaveCityResult->blockinfo->events[0]->eventid);
	event_1->setPosition(Vec2(x,y));
}

void UISailManage::genenrateEvent(SeaEventDefine** tempEvent,int n_events)
{
	m_pSeaManage->releaseEvents();
	m_vNPCShipsD.clear();
	m_vNPCShipsF.clear();
	for (int i = 0; i < n_events; i++)
	{
		TVSeaEvent* se = TVSeaEvent::generateEvent(tempEvent[i]);
		m_pSeaManage->m_vEvents.push_back(se);
		if (tempEvent[i]->type == 1)
		{
			struct UINPCShip::NPCShipInfo npcshipInfo = {};
			npcshipInfo.i_eventId = tempEvent[i]->eventid;
			npcshipInfo.i_country = tempEvent[i]->npcforceid;
			npcshipInfo.i_id = tempEvent[i]->npcid;
			npcshipInfo.i_shipLevel = SINGLE_HERO->m_iLevel;
			npcshipInfo.i_level = tempEvent[i]->npclevel;
			npcshipInfo.b_isFridend = tempEvent[i]->isfriendnpc;
			npcshipInfo.shipTexture = NPC_SHIPICON[0];
			npcshipInfo.s_name = SINGLE_SHOP->getBattleNpcInfo()[tempEvent[i]->npcid].name;
			npcshipInfo.IsNeutralChange=false;
			//中立变敌对
			if (m_bIsLeaveCityTosea)
			{
				for (int j=0;j<m_pLeaveCityResult->n_wantedforceids;j++)
				{
					if ( tempEvent[i]->npcforceid==m_pLeaveCityResult->wantedforceids[j])
					{
						npcshipInfo.IsNeutralChange=true;
					}
				}
			}
			else
			{
				for (int j=0;j<m_pLoginFirstData->n_wantedforceids;j++)
				{
					if ( tempEvent[i]->npcforceid==m_pLoginFirstData->wantedforceids[j])
					{
						npcshipInfo.IsNeutralChange=true;
					}
				}
			}
			
			if (m_nTaskNpcId == npcshipInfo.i_id || (CompanionTaskManager::GetInstance()->m_PartsInfo &&
					(CompanionTaskManager::GetInstance()->m_PartsInfo->npc_id == npcshipInfo.i_id ||
							(CompanionTaskManager::GetInstance()->m_PartsInfo->type == TYPE__BEAT_NPC && CompanionTaskManager::GetInstance()->m_PartsInfo->target_id == npcshipInfo.i_id))))
			{
				npcshipInfo.b_isTaskNpc = true;
			}else
			{
				npcshipInfo.b_isTaskNpc = false;
			}

			UINPCShip* npcShip = UINPCShip::create(npcshipInfo);
			npcShip->addTouchEventListener(CC_CALLBACK_2(UISailHUD::onSeaEvent_npc, dynamic_cast<UISailHUD*>(m_pMapUI)));
			int x = tempEvent[i]->x;
			int y = MAP_CELL_HEIGH * MAP_WIDTH - tempEvent[i]->y;
			npcShip->setPosition(Vec2(x, y));
			m_EventLayer->addChild(npcShip, 10, 100);
			se->m_NPC = npcShip;
			npcShip->m_eventData = se;
#ifdef _DEBUG
			std::string s_pos = StringUtils::format("(%d - %d)", x, y);
			Text* t_pos = Text::create(s_pos, "", 26);
			t_pos->setPosition(Vec2(100,100));
			t_pos->setScale(0.5f);
			npcShip->addChild(t_pos);
#endif
			//为什么要分开？
// 			if (tempEvent[i]->isfriendnpc){
// 				
// 				m_NPCShipsF.push_back(npcShip);
// 			}
// 			else{
// 			
// 				m_NPCShipsD.push_back(npcShip);
// 			}

			m_vNPCShipsD.push_back(npcShip);
		}else if(tempEvent[i]->type == 4) // 触礁
		{
			Button* event_4 = Button::create();
			event_4->setTouchEnabled(false);
			m_EventLayer->addChild(event_4,10,100);
			
			event_4->addTouchEventListener(CC_CALLBACK_2(UISailHUD::onSeaEvent_float, dynamic_cast<UISailHUD*>(m_pMapUI)));
			int x = tempEvent[i]->x;
			int y = MAP_CELL_HEIGH * MAP_WIDTH - tempEvent[i]->y;
			event_4->setTag(tempEvent[i]->eventid);
			event_4->setPosition(Vec2(x,y));
		}
		else if (tempEvent[i]->type == 3) //float
		{
			Button* event_3 = Button::create("res/event_float.png");
            event_3->setTouchEnabled(false);
			m_EventLayer->addChild(event_3, 10, 100);

			event_3->addTouchEventListener(CC_CALLBACK_2(UISailHUD::onSeaEvent_float, dynamic_cast<UISailHUD*>(m_pMapUI)));
			int x = tempEvent[i]->x;
			int y = MAP_CELL_HEIGH * MAP_WIDTH - tempEvent[i]->y;
			event_3->setTag(tempEvent[i]->eventid);
			event_3->setPosition(Vec2(x, y));
		}
		else if (tempEvent[i]->type == 6)
		{
			auto boss = Button::create("res/city_icon/boss.png");
			boss->setScale(0.5f);
			boss->setPosition(Vec2(tempEvent[i]->x, MAP_CELL_HEIGH * MAP_WIDTH - tempEvent[i]->y));
			boss->setAnchorPoint(Vec2(0.5, 0.5));
			boss->setName("button_boss");
				
			m_EventLayer->addChild(boss);
			boss->addTouchEventListener(CC_CALLBACK_2(UISailManage::buttonEvent,this));
			boss->setTag(tempEvent[i]->npcid);
			SINGLE_HERO->m_nBossEventId = tempEvent[i]->eventid;
			auto i_bg = ImageView::create("res/city_icon/cityname_bg.png");
			i_bg->setPosition(Vec2(tempEvent[i]->x, MAP_CELL_HEIGH * MAP_WIDTH - tempEvent[i]->y - boss->getBoundingBox().size.height*0.4));
			auto t_boss = Text::create();
			t_boss->setString(SINGLE_SHOP->getBattleNpcInfo()[tempEvent[i]->npcid].name);
			t_boss->setPosition(Vec2(i_bg->getBoundingBox().size.width / 2, i_bg->getBoundingBox().size.height*0.4));
			t_boss->setColor(Color3B(255, 191, 38));
			t_boss->setFontSize(20);
			i_bg->addChild(t_boss);
			m_EventLayer->addChild(i_bg);
		}
	}
	m_EventLayer->setCameraMask(4, true);
	m_EventLayer->setSubNodeMaxCameraMask(4);
}

void UISailManage::showSeaEvent(TVSeaEvent* temEvent)
{
	//鼠疫并且当前供给为0时跳过
	if (temEvent->m_Type == EV_SEA_PESTIS && dynamic_cast<UISailHUD*>(m_pMapUI)->m_nCurSupplies <= dynamic_cast<UISailHUD*>(m_pMapUI)->m_nCurCrewNum * CARW_ONE_DAY_SUPPLY)
	{
		return;
	}
	
	if(dynamic_cast<UISailHUD*>(m_pMapUI)->m_bIsBlocking){
		return;
	}
	dynamic_cast<UISailHUD*>(m_pMapUI)->m_bIsBlocking = true;

	dynamic_cast<UIShip*>(m_pShipImage)->setSeaAccident(true);
	dynamic_cast<UISailHUD*>(m_pMapUI)->showSeaEvent(temEvent);
	dynamic_cast<UIShip*>(m_pShipImage)->startPauseTime();
}

void UISailManage::seaEventEnd()
{
	if (m_pSeaManage == nullptr)
	{
		return;
	}
	m_bIsPause = false;
	m_pSeaManage->startSeaEvent();
	dynamic_cast<UIShip*>(m_pShipImage)->setSeaAccident(false);
	dynamic_cast<UIShip*>(m_pShipImage)->stopPauseTime();
}

void UISailManage::seaEventStart()
{
	if (m_pSeaManage == nullptr)
	{
		return;
	}
	m_bIsPause = true;
	m_pSeaManage->pauseSeaEvent();
	dynamic_cast<UIShip*>(m_pShipImage)->setSeaAccident(true);
	dynamic_cast<UIShip*>(m_pShipImage)->startPauseTime();
}

void UISailManage::addSeaEvent_leavCity()
{
	m_EventLayer->removeAllChildren();
	//m_pSeaManage->geneEvents(m_pLeaveCityResult->blockinfo->events,m_pLeaveCityResult->blockinfo->n_events);
	m_nTaskNpcId = m_pLeaveCityResult->blockinfo->tasknpcid;
	genenrateEvent(m_pLeaveCityResult->blockinfo->events,m_pLeaveCityResult->blockinfo->n_events);
}

void UISailManage::addSeaEvent_nextBlock(const SeaBlockInfo *info)
{
	if(!info) return;
	m_EventLayer->removeAllChildren();
	m_nTaskNpcId = info->tasknpcid;
	//m_pSeaManage->geneEvents(info->events, info->n_events);
	genenrateEvent(info->events, info->n_events);
	
}

void UISailManage::addSeaEvent(SeaEventDefine** events,int n_events)
{
	m_EventLayer->removeAllChildren();
	//m_pSeaManage->geneEvents(events,n_events);
	genenrateEvent(events,n_events);
}

void UISailManage::mapUICall_func(std::string name){

	if (isButton(button_fleetinfo))
	{
		ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::createWithMask(this,4));
	}
}

void UISailManage::flashIconPosXWithText(Text* text, ImageView* image){

	float textPosX = text->getPositionX();
	float textSizeX = text->getSize().width;
	float imageX = image->getSize().width;

	image->setPositionX(textPosX - textSizeX - imageX - 10);

}
//判断是否与npc战斗
void UISailManage::checkGuardShip(Vec2 pos, float f)
{
//	m_NPCShips
	for (auto &m_npcShip : m_vNPCShipsD)
	{
		Vec2 npc_pos = m_npcShip->getPosition();
		double dis = fabs((pos.x - npc_pos.x) * (pos.x - npc_pos.x) + (pos.y - npc_pos.y) * (pos.y - npc_pos.y));
		UINPCShip* npcShip = dynamic_cast<UINPCShip*>(m_npcShip);
		if (!npcShip->m_eventData->shouldTriggerBattle() || npcShip->getEventID() == m_lastNPCEventID)
		{
			npcShip->isShowGuardCircle(false, f);
			continue;
		}
		else
		{
			if (dis < 50000)
			{
				npcShip->isShowGuardCircle(true, f);
				if (dis < 20000)
				{
					if (!getIfTriggerRange())
					{
						setIfTriggerRange(true);
						m_pSeaManage->setseaEventHalfRange(npcShip->n);
						//m_pSeaManage->isHappenEvent(m_pSeaManage->getEvents()[1]);
					}
				}

			}else
			{
				npcShip->isShowGuardCircle(false, f);
			}
		}
	}
}

void UISailManage::openfindNewCity(Widget * psender)
{
	Point pos = this->convertToNodeSpace(Point(0, 0));
	Point pos1 = m_MapsLayer->convertToNodeSpace(pos);
	auto bg_light = psender->getChildByTag(211);
	bg_light->runAction(FadeIn::create(1.5));
	this->runAction(Sequence::create(DelayTime::create(2), CallFunc::create([=]{	
		dynamic_cast<UISailHUD*>(m_pMapUI)->findNewCity(m_nCityId, psender, pos1);
		bg_light->setVisible(false);
		psender->setTouchEnabled(true);
	}), nullptr));
}

bool SailScene::initWithPhysics()
{
	return Scene::initWithPhysics();
}

Scene *SailScene::createWithPhysics()
{
	SailScene *ret = new (std::nothrow) SailScene();
	if (ret && ret->initWithPhysics())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

static bool camera_cmp(const Camera* a, const Camera* b)
{
    return a->getDepth() < b->getDepth();
}

void SailScene::render(Renderer* renderer)
{
#if 1
	Scene::render(renderer);
#else
	auto director = Director::getInstance();
	Camera* defaultCamera = nullptr;
	const auto& transform = getNodeToParentTransform();
	if (_cameraOrderDirty)
	{
		stable_sort(_cameras.begin(), _cameras.end(), camera_cmp);
		_cameraOrderDirty = false;
	}
	//std::vector<float> timeConsume;
	for (const auto& camera : _cameras)
	{
		struct timeval start,end;
		if (!camera->isVisible())
			continue;
		gettimeofday(&start,0);
		Camera::setVisitingCamera(camera);

		director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
		director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, camera->getViewProjectionMatrix());

//		if((int)camera->getCameraFlag() == 4){
//			break;
//		}
		struct timeval start1,end1;
		gettimeofday(&start1,0);
		//visit the scene
		Node*root = camera->getStartVisitNode();
		if(root){
			root->visit(renderer, transform, 0);
		}else{
			visit(renderer, transform, 0);
			root = this;
		}
		gettimeofday(&end1,0);
		float visitTime = (end1.tv_sec-start1.tv_sec)*1000+ (end1.tv_usec - start1.tv_usec)*0.001;
		gettimeofday(&start1,0);
		renderer->render();
		gettimeofday(&end1,0);
		float renderTime = (end1.tv_sec-start1.tv_sec)*1000+ (end1.tv_usec - start1.tv_usec)*0.001;

		director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
		gettimeofday(&end,0);
		if(_cameras.size() == 3){
			float millSec = (end.tv_sec-start.tv_sec)*1000+ (end.tv_usec - start.tv_usec)*0.001;
			//log("Scene::render,c mask:%d,millisec:%f,visit time:%f,render time:%f,node count:%d\n",camera->getCameraFlag(),millSec,visitTime,renderTime,countNodes(root,(int)camera->getCameraFlag()));
		}
	}

	Camera::setVisitingCamera(nullptr);
#endif
}

bool UISailManage::getSeaEventLayerFloat()
{
	return m_pSeaManage->getSeaEventFLoat();
}

TVSeaEvent * UISailManage::getSeaFloat()
{
	return m_pSeaManage->SeaFloart();
}
