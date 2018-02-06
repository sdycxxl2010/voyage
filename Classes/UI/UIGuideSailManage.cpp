
#include "UIGuideSailManage.h"
#include "UILoadingIndicator.h"
#include "ProtocolThread.h"

#include "ShipPathData.h"
#include "UIGuideShip.h"
#include "TVSailDefineVar.h"
#include "TVSingle.h"
#include "UIGuideSailHUD.h"
#include "CCLuaEngine.h"
#include "UITips.h"
#include "TVGuideSeaEventManage.h"
#include "UICommon.h"

using namespace ui;
void guideGetRandomAnimations(int randomCount, Animation* ani)
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

UIGuideSailManage::UIGuideSailManage() :
m_MapsLayer(nullptr),
m_pShipImage(nullptr),
m_PrePoint(0, 0),
m_pLoadLayer(nullptr),
m_pTouchIcon(nullptr),
m_ScreenFirstPos(10, 14),
m_bIsBlock(false),
m_ischufa(0),
m_pHistCheck(nullptr),
m_ShipPostion(-1, -1),
m_nShipMoveTime(0),
m_pSailRoad(nullptr),
m_pSeaManage(nullptr)
{
	m_nSailDays = 0;
	m_pHistCheck = new HistCheck;
	m_pMapUI = nullptr;
	m_pLeaveCityResult = nullptr;
	m_eUIType = UI_MAP;
	m_vNPCShipsF.clear();
	m_vNPCShipsD.clear();
	m_nCityId = 0;
	m_EventLayer = nullptr;
	m_npcIsActive = 30.0;
	m_nNpcId = 0;
	m_bFailedAutoFight = 0;

	m_pReachCityResult = nullptr;
	saillngHandPic = nullptr;
	m_pSailRoad = new SailRoad;
	guideShipStop_1 = false;
	guideShipStop_2 = false;
	guideShipStop_3 = false;
	guideShipStop_4 = false;
	guideShipStop_5 = false;
}

UIGuideSailManage::~UIGuideSailManage()
{
	m_UnLoadImage_sea.clear();
	m_UnLoadImage_land.clear();
	m_vNPCShipsF.clear();
	m_vNPCShipsD.clear();
	delete m_pHistCheck;
	if (m_pLeaveCityResult)
	{
		leave_city_result__free_unpacked(m_pLeaveCityResult, 0);
		m_pLeaveCityResult = nullptr;
	}

	m_pReachCityResult = nullptr;
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

UIGuideSailManage* UIGuideSailManage::create()
{
	auto* myMap = new UIGuideSailManage();
	if (myMap &&myMap->init())
	{
		myMap->autorelease();
		return myMap;
	}
	CC_SAFE_DELETE(myMap);
	return nullptr;
}
UIGuideSailManage* UIGuideSailManage::create(Point pos, bool seainfo)
{
	auto* myMap = new UIGuideSailManage();
	myMap->m_ShipPostion = pos;
	if (myMap && myMap->init(seainfo))
	{
		myMap->autorelease();
		return myMap;
	}
	CC_SAFE_DELETE(myMap);
	return nullptr;
}
void UIGuideSailManage::onEnter()
{
	UIBasicLayer::onEnter();// 
	auto size = Director::getInstance()->getWinSize();
	Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
	popCam->setCameraFlag(CameraFlag::USER2);
	popCam->setDepth(4);
	//popCam->setStartVisitNode(m_pMapUI);
	this->setAnchorPoint(Vec2(0.5f, 0.5f));
	this->addChild(popCam);

	auto listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = CC_CALLBACK_2(UIGuideSailManage::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(UIGuideSailManage::onTouchMoved, this);
	listener->onTouchCancelled = CC_CALLBACK_2(UIGuideSailManage::onTouchCancelled, this);
	listener->onTouchEnded = CC_CALLBACK_2(UIGuideSailManage::onTouchEnded, this);
	listener->setSwallowTouches(true);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void UIGuideSailManage::onExit()
{
	this->unscheduleUpdate();
	TextureCache::getInstance()->unbindAllImageAsync();
	m_UnLoadImage_land.clear();
	m_UnLoadImage_sea.clear();

	_eventDispatcher->removeEventListenersForTarget(this);
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("res/test/battlewater.plist");
	//stopAudio();
	UIBasicLayer::onExit();
}
bool UIGuideSailManage::init(bool seainfo)
{
	bool pRet = false;
	do
	{
		Layer::init();

		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIGuideSailManage::onServerEvent, this), this);

		if (seainfo)//从战斗返回
		{
			if (CCUserDefault::getInstance()->getIntegerForKey(SOUND_EFFECT_KEY, OPEN_AUDIO) != OPEN_AUDIO)
			{
				SINGLE_AUDIO->setEffectON(false);
			}
			ProtocolThread::GetInstance()->getSailInfo(UILoadingIndicator::create(this, 1));
		}
		else//lua 出来进这里
		{
			if(ProtocolThread::GetInstance()->getConnectionStatus()){
				ProtocolThread::GetInstance()->leaveCity(UILoadingIndicator::create(this, 1));
			}else{
				ProtocolThread::GetInstance()->reconnectServer();
			}
		}


		m_MapsLayer = Layer::create();
		this->addChild(m_MapsLayer, 5);
		m_MapsLayer->setContentSize(Size(MAP_HEIGH * MAP_CELL_HEIGH, MAP_WIDTH * MAP_CELL_WIDTH));
		m_pLoadLayer = UILoadingIndicator::create(this, 1);
		m_pLoadLayer->showSelf();
		m_EventLayer = Layer::create();
		//m_EventLayer->retain();
		m_MapsLayer->addChild(m_EventLayer, 3);
		pRet = true;
	} while (0);
	return pRet;
}

void UIGuideSailManage::initStatic(float f)
{
	if (m_pMapUI)
	{
		return;
	}
	m_pSeaManage = TVGuideSeaEventManage::createManage(this);
	m_pSeaManage->retain();
	m_pSeaManage->scheduleUpdate();
	this->addChild(m_pSeaManage);
	initCityData();
	m_pHistCheck->initLandHistTestData();
	m_pMapUI = UIGuideSailHUD::create();
	m_pMapUI->setCameraMask(4, true);
	this->addChild(m_pMapUI, 10);
}
void UIGuideSailManage::addLights()
{
	{
		auto directionalLight_6 = DirectionLight::create(Vec3(1.0f, 0.0f, 0.0f), Color3B(255, 255, 255));

		directionalLight_6->setCameraMask(2);
		directionalLight_6->setIntensity(1.6f);
		directionalLight_6->setLightFlag(LightFlag::LIGHT1);
		this->addChild(directionalLight_6);
		auto _ambientLight = AmbientLight::create(Color3B(180, 180, 180));
		//_ambientLight->retain();
		_ambientLight->setCameraMask(2);
		_ambientLight->setEnabled(true);
		this->addChild(_ambientLight);
	}
}
void UIGuideSailManage::initNetworkData(const LeaveCityResult* result)
{
	for (int i = 0; i<result->n_discoverdcityids; i++)
	{
		SINGLE_HERO->setFindCityStatus(result->discoverdcityids[i] - 1, true);
	}
	initStatic(0);
	m_vEventFlag.assign(result->blockinfo->n_events, false);
	m_CenterPoint = Director::getInstance()->getWinSize() / 2;
	Point startPos = Vec2(result->blockinfo->x, MAP_WIDTH * MAP_CELL_WIDTH - result->blockinfo->y - result->blockinfo->height);
	Size hw = Size(result->blockinfo->width, result->blockinfo->height);
	auto shipInfo = SINGLE_SHOP->getShipData()[result->shipid];
	UIGuideShip *ship = UIGuideShip::create(shipInfo.model_id);
	m_MapsLayer->addChild(ship, 2); 

	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->setForceFightNpcId(result->fightnpcid);

	//addLights();
	ship->setParent(m_MapsLayer);
	ship->setUpdateSize(hw);
	ship->setUpdatePos(startPos);
	ship->setAnchorPoint(Vec2(0.5, 0.5));
	ship->setSupplyDays(result->endurancedays);
	ship->setCurrentCrew(result->sailorcount);

	ship->setSpeed(result->speed);
	ship->setLastSailDays(m_nSailDays);
	m_pShipImage = ship;
	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->setShip(ship);
	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->setShipPositon(m_pShipImage->getPosition());
	auto expNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str());
	auto repNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str());
	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->setExpAndFame(expNovice, repNovice);
	
	//在新手引导水手和补给都不会损失
	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->setSupplies(result->maxtotalsupply, result->maxtotalsupply);
	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->setCrew(result->maxsailorcount);
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

		ship->setPosition(Vec2(result->shipx, MAP_WIDTH * MAP_CELL_WIDTH - result->shipy));
		m_MapsLayer->setPosition(Vec2(city_x, city_y));

	}
	else
	{
		city_x = -m_ShipPostion.x + m_CenterPoint.x;
		city_y = -m_ShipPostion.y + m_CenterPoint.y;
		if (city_x > 0)
		{
			city_x = 0;
			ship->setIsCenterX(false);
		}
		if (city_y > 0)
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
		m_MapsLayer->setPosition(Vec2(city_x, city_y));
	}

	m_ScreenFirstPos = Vec2(-city_x, -city_y) / MAP_CELL_WIDTH;
	m_ScreenFirstPos = Vec2(int(m_ScreenFirstPos.x), int(m_ScreenFirstPos.y));
	m_PrePoint = Vec2(m_ScreenFirstPos.x * MAP_CELL_WIDTH, m_ScreenFirstPos.y * MAP_CELL_HEIGH);
	int firstX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM / 2;
	int firstY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM / 2;

	auto cache = AnimationCache::getInstance();
	Animation *an = cache->getAnimation("waterAnimation");
	if (an == NULL) {
		SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
		Director::getInstance()->getTextureCache()->removeUnusedTextures();

		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/test/battlewater.plist");
		Vector<SpriteFrame*> sfs(120);
		for (int i = 0; i < 120; i++) {

			SpriteFrame *sf = SpriteFrameCache::getInstance()->getSpriteFrameByName(String::createWithFormat("water%04d.png", i)->getCString());
			sfs.pushBack(sf);
		}
		an = Animation::createWithSpriteFrames(sfs, 1.f / 30, -1);	//0.15f,-1);
		cache->addAnimation(an, "waterAnimation");
	}

	for (int i = 0; i < SCREEN_HEIGH_NUM * 2; i++)
	{
		for (int j = 0; j < 2 * SCREEN_WIDTH_NUM; j++)
		{
			std::string fileName = getCellName(firstX + j, firstY + i);
			std::string seaName = getSeaCellName(firstX + j, firstY + i);
			auto mapCell = Sprite::create();
			mapCell->setTexture(fileName);
			auto bg = Sprite::create();
			bg->setAnchorPoint(Vec2(0, 0));
			int randomCount = cocos2d::random(0, 60);
			Animation* temp_an = an->clone();
			guideGetRandomAnimations(randomCount, temp_an);
			bg->runAction(Animate::create(temp_an));

			mapCell->setAnchorPoint(Vec2(0, 0));
			mapCell->addChild(bg, -2, 2);
			m_MapsLayer->addChild(mapCell, 1);

			mapCell->setPosition(MAP_CELL_WIDTH * (firstX + j), MAP_CELL_HEIGH*(firstY + i));
			m_RenderSprites.insert(std::pair<int, Sprite*>((firstX + j) * CONST_VALUE + i + firstY, mapCell));
		}
	}
	//npc初始化
	addSeaEvent_leavCity();
	this->scheduleUpdate();
	if (m_pLoadLayer)
	{
		m_pLoadLayer->removeSelf();
		m_pLoadLayer = nullptr;
	}
	m_pMapUI->setCameraMask(4);
}

void UIGuideSailManage::initCityData()
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
		city->setPosition(Vec2(citysInfo[i + 1].x, citysInfo[i + 1].y));
		city->setAnchorPoint(Vec2(0.5, 0.5));
		city->setName("button_city");
		auto i_bg = ImageView::create("res/city_icon/cityname_bg.png");
		i_bg->setPosition(Vec2(citysInfo[i + 1].x, citysInfo[i + 1].y - city->getContentSize().height*0.4));
		auto t_city = Text::create();
		t_city->setString(citysInfo[i + 1].name);
		t_city->setPosition(Vec2(i_bg->getContentSize().width / 2, i_bg->getContentSize().height*0.4));
		t_city->setColor(Color3B(255, 191, 38));
		t_city->setFontSize(20);
		i_bg->addChild(t_city);
		cityLayer->addChild(city, 1, i + CONST_VALUE);
		cityLayer->addChild(i_bg, 2);
		city->setVisible(false);
		i_bg->setVisible(false);
		if (SINGLE_HERO->getFindCityStatus(i))
		{
			city->setVisible(true);
			i_bg->setVisible(true);
		}

		if (SINGLE_HERO->m_iBornCity == i + 1)
		{
			city->setVisible(true);
			i_bg->setVisible(true);
			city->setTouchEnabled(false);//引导中不可点击
		}
		if (i == 16 || i == 14||i == 22 || i == 8||i==20)//法鲁,巴塞罗那，鹿特丹，那不勒斯，南安普顿
		{
			city->setTouchEnabled(false);
		}

		city->addTouchEventListener(CC_CALLBACK_2(UIGuideSailManage::buttonEvent,this));
	}
	cityLayer->setPosition(0, 0);
	m_MapsLayer->addChild(cityLayer, 8);
	cityLayer->release();
}
std::string UIGuideSailManage::getSeaCellName(int x, int y)
{
	return "res/map_cell/sea/1.png";
	y = MAP_WIDTH - y - 1;
	if (x > MAP_HEIGH - 1 || x<0)
	{
		return StringUtils::format("res/map_cell/sea/1.png");
	}
	if (y > MAP_WIDTH - 1 || y < 0)
	{
		return StringUtils::format("res/map_cell/sea/1.png");
	}
	return StringUtils::format("res/map_cell/sea/%d.png", MAP_SEA_INDEX[y][x]);
}
std::string UIGuideSailManage::getCellName(int x, int y)
{
	y = MAP_WIDTH - y - 1;
	if (x > MAP_HEIGH - 1 || x < 0)
	{
		return StringUtils::format("res/map_cell/land/1.png");
	}
	if (y > MAP_WIDTH - 1 || y < 0)
	{
		return StringUtils::format("res/map_cell/land/1.png");
	}
	int index = MAPINDEX[y][x];
	return StringUtils::format("res/map_cell/land/%d.png", MAPINDEX[y][x]);
}

void UIGuideSailManage::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* w_button = dynamic_cast<Widget*>(pSender);
		std::string name = w_button->getName();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	}
}

void UIGuideSailManage::buttonEvent(Ref* target, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* w_button = dynamic_cast<Widget*>(target);
	std::string name = w_button->getName();
	buttonEventName(w_button, name);
}

void UIGuideSailManage::buttonEventName(Widget* target, std::string name)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_ok))
	{
		if (m_bIsBlock){
			return;
		}
		m_bIsBlock = true;
		auto portsInfo = SINGLE_SHOP->getCitiesInfo();
		auto portInfo = portsInfo[m_nCityId];

		ProtocolThread::GetInstance()->reachCity(portInfo.dock_x, portInfo.dock_y, UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_cancel))
	{
		auto parent = target->getParent()->getParent();
		parent->removeFromParent();
		return;
	}
	if (isButton(button_yes))
	{
		if (m_bIsBlock){
			return;
		}
		m_bIsBlock = true;
		ProtocolThread::GetInstance()->reachCity(-1, -1);
		dynamic_cast<UIBasicLayer*>(this->getParent())->button_callBack();
		return;
	}
	//the big city button.
	//if (isButton(button_city))
	//{
	//	int cityId = target->getTag() - CONST_VALUE;
	//	auto portsInfo = SINGLE_SHOP->getCitiesInfo();
	//	auto portInfo = portsInfo[cityId + 1];
	//	int city_x = portInfo.x;// portInfo.dock_x
	//	int city_y = portInfo.y;//portInfo.dock_y
	//	if (m_pShipImage->getPosition().getDistance(Vec2(city_x, city_y)) < LAND_DISTANCE)
	//	{
	//		m_nCityId = cityId + 1;
	//		openDialog();
	//	}
	//	else
	//	{
	//		if (m_pShipImage)
	//		{
	//			ShipGuide* ship = dynamic_cast<ShipGuide*>(m_pShipImage);
	//			ship->setGoldPosition(target->getPosition());
	//			ship->m_IsAutoSailing = false;

	//		}
	//	}
	//	return;
	//}
}

void UIGuideSailManage::onServerEvent(struct ProtobufCMessage* message, int msgType) {
	//BasicLayer::onServerEvent(message, msgType);
	switch (msgType) {
		case 0:{//连接服务器成功
			const std::string & email = ProtocolThread::GetInstance()->getSavedEmail();
			const std::string & passwd = ProtocolThread::GetInstance()->getSavedPassword();
			log("LoadingScene::sendEmailLogin");
			if (ProtocolThread::GetInstance()->getGoogleAccount().length() > 0 && ProtocolThread::GetInstance()->getGoogleToken().length() > 0){
				ProtocolThread::GetInstance()->tokenLogin();
			}else if (email.size() > 0 && passwd.size() > 0){
				ProtocolThread::GetInstance()->userEmailLogin(email.c_str(), passwd.c_str());
			}else{
				openReconnectDialog("TIP_CONNECT_FAIL");
			}
		}
			break;
		case PROTO_TYPE_EmailLoginResult:{
			EmailLoginResult *result = (EmailLoginResult*)message;
			if(result->failed == 0){
				ProtocolThread::GetInstance()->leaveCity();
			}else{
				openReconnectDialog("TIP_CONNECT_FAIL");
			}

		}
			break;
		case PROTO_TYPE_GetSailInfoResult: {
			pNovice = UINoviceStoryLine::createNovice(this);
			this->addChild(pNovice, 100);
			pNovice->setCameraMask(4);
			saillngHandPic = Sprite::create();
			saillngHandPic->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
			saillngHandPic->setRotation(-180.0f);
			m_EventLayer->addChild(saillngHandPic, 200, 200);
			saillngHandPic->runAction(
					RepeatForever::create(
							Sequence::createWithTwoActions(
									TintTo::create(0.5, 255, 255, 255),
									TintTo::create(0.5, 180, 180, 180))));
			setAfterWarHandPic();	//小手的位置
			GetSailInfoResult*sailInfo = (GetSailInfoResult *) message;
			LeaveCityResult *result = sailInfo->seadata;
			if (result && sailInfo->failed == 0) {
				m_npcIsActive = 30.0;
				m_nNpcId = sailInfo->lastfighteventid;
				m_pLeaveCityResult = result;
				m_nSailDays = sailInfo->saildays;
				initNetworkData(result);
				for (int i = 0; i < m_vNPCShipsD.size(); i++) {
					if (m_vNPCShipsD.at(i)->getNpcId() == m_nNpcId) {
						m_vNPCShipsD.at(i)->setAciveAttack(false);
						break;
					}
				}
				playAudio();
				//add mew sound
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_MEW_06);
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->m_nEventversion =
						result->blockinfo->eventversion;
			} else {
				if (result) {
					openSuccessOrFailDialog("TIP_LEAVE_CITY_FAIL");
				}
			}
			break;
		}
			break;
		case PROTO_TYPE_LeaveCityResult: {
			SINGLE_HERO->m_heroIsOnsea = true;
			pNovice = UINoviceStoryLine::createNovice(this);
			this->addChild(pNovice, 100);
			pNovice->onGuideTaskMessage(UINoviceStoryLine::SAILING_GUIDE_DIALOG, 0);
			pNovice->setCameraMask(4);
			LeaveCityResult *result = (LeaveCityResult *)message;
			if (result && result->failed == 0)
			{
				m_pLeaveCityResult = result;
				initNetworkData(result);
				playAudio();
				//add mew sound
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_MEW_06);
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->m_nEventversion = result->blockinfo->eventversion;
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
		case PROTO_TYPE_ReachCityResult:
		{
			ReachCityResult *result = (ReachCityResult*)message;
			if (result && result->failed == 0)
			{
				m_pReachCityResult = result;
				ProtocolThread::GetInstance()->unregisterMessageCallback(this);
				if (m_bFailedAutoFight)
				{
					this->button_callBack();
				}
				else
				{
					openCountUI(result);
				}
			}
			else
			{
				openSuccessOrFailDialog("TIP_VIST_CITY_FAIL");
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(false);
			}
			m_bIsBlock = false;
			break;
		}
		case PROTO_TYPE_GetNextMapBlockResult:
		{
			GetNextMapBlockResult* result = (GetNextMapBlockResult*)message;
			if (result && result->failed == 0)
			{
				auto mapUI = dynamic_cast<UIGuideSailHUD*>(m_pMapUI);
				mapUI->setShipPositon(m_pShipImage->getPosition());
				if (result->eventchanged)
				{
					m_EventLayer->removeAllChildren();
					dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->m_nEventversion = result->blockinfo->eventversion;
					addSeaEvent_nextBlock(result->blockinfo);
				}
			}
			break;
		}
		case PROTO_TYPE_EngageInFightResult:
		{
			m_pMapUI->onServerEvent(message, msgType);
			break;
		}
		case PROTO_TYPE_EndFightResult:
		{
			m_pMapUI->onServerEvent(message, msgType);
			EndFightResult* result = (EndFightResult*)message;
			if (result->failed == 0 && result->isautofight) {
				if (result->reason == 1) {
					addSeaEvent_nextBlock(result->seainfo);
				}
				else if (result->reason == 2) {
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
				if (result->eventchanged)
				{
					addSeaEvent(result->seainfo->events, result->seainfo->n_events);
				}
			}
			else
			{
				seaEventEnd();
			}
			break;
		}
		case PROTO_TYPE_GetCityStatusResult:
		{
			dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->onServerEvent(message, msgType);
			break;
		}
		case PROTO_TYPE_AddDiscoveredCityResult:
		{
			dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->onServerEvent(message, msgType);
			break;
		}
		default:
			UIBasicLayer::onServerEvent(message, msgType);
		break;
	}
}

void UIGuideSailManage::openCountUI(ReachCityResult* result)
{

	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->openView(MAPUI_COCOS_RES[INDEX_UI_BACK_CITY_CSB]);
	Widget* panel_back_result = dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BACK_CITY_CSB]);
	panel_back_result->setCameraMask(4, true);//摄像机

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

	Text* label_exp_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_exp_num"));
	Text* label_r_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_r_num"));
	Text* label_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv"));
	Text* label_lv_r = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv_r"));

	ImageView* image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_addexp, "image_head"));
	Widget* panel_fee = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_fee"));
	Text* label_total_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fee, "label_total_num"));
	Text* label_warehouse_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fee, "label_warehouse_num"));
	Text* label_hosted_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fee, "label_hosted_num"));
	Text* label_daily_expenses_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fee, "label_daily_expenses_num"));

	Widget* panel_ship_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_1"));
	Widget* panel_ship_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_2"));
	Widget* panel_ship_3 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_3"));
	Widget* panel_ship_4 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_4"));
	Widget* panel_ship_5 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, "panel_ship_5"));

	//引导的小手
	auto button_landCity = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_back_result, "button_landCity"));
	auto handpic = Sprite::create();
	handpic->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	handpic->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
	button_landCity->addChild(handpic, 10);
	handpic->setCameraMask(4, true);
	handpic->setLocalZOrder(1000);
	//小手在按钮左上角
	handpic->setPosition(Vec2(button_landCity->getContentSize().width / 2 - handpic->getContentSize().width / 2, button_landCity->getContentSize().height / 2 * 1.6 + handpic->getContentSize().height / 2 * 0.6));

	int sailDay = result->sailingdays;
	int wareFee = sailDay * result->warehousedailycost;
	int captainFee = sailDay * result->captainsdailycost;
	int dockFee = sailDay * result->dockdailycost;
	int totalFee = wareFee + captainFee + dockFee;

	auto expNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str());
	auto repNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str());
	auto expLv = EXP_NUM_TO_LEVEL(expNovice);
	auto repLv = FAME_NUM_TO_LEVEL(repNovice);

	float temp_exp = 0;
	float temp_rep = 0;
	if (expLv < LEVEL_MAX_NUM)
	{
		temp_exp = (expNovice - LEVEL_TO_EXP_NUM(expLv))*1.0 / (LEVEL_TO_EXP_NUM(expLv + 1) - LEVEL_TO_EXP_NUM(expLv));
	}
	
	if (repLv < LEVEL_MAX_NUM)
	{
		temp_rep = (repNovice - LEVEL_TO_FAME_NUM(repLv))*1.0 / (LEVEL_TO_FAME_NUM(repLv + 1) - LEVEL_TO_FAME_NUM(repLv));
	}

	panel_exp->setContentSize(Size(panel_exp->getContentSize().width, 120 * temp_exp));
	panel_rep->setContentSize(Size(panel_rep->getContentSize().width, 120 * temp_rep));
	
	std::string str_rrep_num = StringUtils::format(" + %d ", 0);
	label_r_num->setString(str_rrep_num);

	std::string str_lv = StringUtils::format("%d ", expLv);

	std::string str_exp_num = StringUtils::format("+ %d ", 0);//新手引导此处不加经验
	label_exp_num->setString(str_exp_num);
	std::string iconPath = StringUtils::format("res/player_icon/icon_%d.png", SINGLE_HERO->m_iIconidx);
	image_head->loadTexture(iconPath);
	std::string str_total_num = StringUtils::format(" %d ", totalFee);
	label_total_num->setString(str_total_num);

	std::string str_warehouse_num = StringUtils::format(" %d ", wareFee);
	label_warehouse_num->setString(str_warehouse_num);

	std::string str_hosted_num = StringUtils::format(" %d ", dockFee);
	label_hosted_num->setString(str_hosted_num);

	std::string str_daily_expenses_num = StringUtils::format(" %d ", captainFee);
	label_daily_expenses_num->setString(str_daily_expenses_num);

	label_lv->setString(str_lv);
	label_lv_r->setString(StringUtils::format(" %d ", repLv));
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

		ShipDefine* sd = result->fleetships[i - 1];

		std::string ship_name = StringUtils::format("panel_ship_%d", i);
		Widget* panel_ship = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_back_result, ship_name));
		auto image_ship_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_ship, "image_ship_bg"));
		ImageView* image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_ship_bg, "image_item_bg_lv"));
		ImageView* image_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "image_ship"));
		ImageView* image_num = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "image_num"));
		std::string num_name = getPositionIconPath(i);
		image_num->loadTexture(num_name);
		if (sd->sid<1||sd->sid>28)
		{
			sd->sid = 2;
		}
		std::string filePath = getShipIconPath(sd->sid);
		image_ship->loadTexture(filePath);
		image_ship->setCameraMask(4, true);
		Text* label_ship_durable_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_1"));
		Text* label_ship_durable_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_2"));
		Text* label_ship_durable_num_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_3"));
		LoadingBar* progressbar_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_ship, "progressbar_durable"));
		std::string str_durable_num_1 = StringUtils::format(" %d", sd->hp);
		std::string str_durable_num_3 = StringUtils::format("  %d ", sd->hp_max);
		setBgButtonFormIdAndType(image_ship_bg, sd->sid, ITEM_TYPE_SHIP);
		setBgImageColorFormIdAndType(image_item_bg_lv, sd->sid, ITEM_TYPE_SHIP);

		if (0 == sd->hp_max){
			progressbar_durable->setPercent(0);
		}
		else{
			progressbar_durable->setPercent(sd->hp * 100 / sd->hp_max);
		}

		ImageView* Image_17 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "Image_17"));
		int chp = sd->current_hp_max;
		if (chp >= sd->hp_max){
			Image_17->setVisible(false);
		}
		else
		{
			if (0 != sd->hp_max){

				Image_17->setPositionX(progressbar_durable->getSize().width * chp / sd->hp_max);
				Image_17->setVisible(true);

			}
		}
		label_ship_durable_num_1->setString(str_durable_num_1);
		label_ship_durable_num_2->setString(" / ");
		label_ship_durable_num_3->setString(str_durable_num_3);

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
		std::string str_sailor_num_1 = StringUtils::format(" %d", sd->crew_num);
		std::string str_sailor_num_3 = StringUtils::format("  %d ", sd->max_crew_num);
		label_ship_sailor_num_1->setString(str_sailor_num_1);
		label_ship_sailor_num_2->setString(" / ");
		label_ship_sailor_num_3->setString(str_sailor_num_3);

		panel_ship->setVisible(true);
	}
}
void UIGuideSailManage::GoBackCity()
{
	this->button_callBack();
}

void UIGuideSailManage::loadMapLine()
{
	if (!m_MapsLayer)
	{
		return;
	}
	Point curPos = m_MapsLayer->getPosition();
	Point diffPos = ccpAdd(curPos, m_PrePoint);


	// map direct is opposite
	if (diffPos.x > MAP_CELL_WIDTH)
	{
		loadMapLine(UIGuideShip::LEFT);
	}
	if (diffPos.x < -MAP_CELL_WIDTH)
	{
		loadMapLine(UIGuideShip::RIGHT);
	}
	if (diffPos.y > MAP_CELL_HEIGH)
	{
		loadMapLine(UIGuideShip::DOWN);
	}
	if (diffPos.y < -MAP_CELL_HEIGH)
	{
		loadMapLine(UIGuideShip::UP);
	}
	m_PrePoint = Vec2(m_ScreenFirstPos.x * MAP_CELL_WIDTH, m_ScreenFirstPos.y * MAP_CELL_HEIGH);
}
void UIGuideSailManage::loadMapLine(int direct)
{
	std::string loadFileName;
	switch (direct)
	{
	case UIGuideShip::UP:
	{
						  m_ScreenFirstPos.y++;
						  int tempY = m_ScreenFirstPos.y + 3 * SCREEN_HEIGH_NUM / 2;
						  int tempX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM / 2;
						  int lastY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM / 2 - 1;
						  int lastX = tempX;
						  for (int i = 0; i < SCREEN_WIDTH_NUM * 2; i++)
						  {
							  auto iter = m_RenderSprites.find((lastX + i) * CONST_VALUE + lastY);
							  if (iter != m_RenderSprites.end())
							  {
								  auto cell = iter->second;
								  auto seaCell = dynamic_cast<Sprite*>(cell->getUserObject());
								  std::string fileName = getCellName(tempX + i, tempY);
								  std::string seaName = getSeaCellName(tempX + i, tempY);
								  asyncLoadingImage(cell, seaCell, fileName, seaName);
								  cell->setPosition((tempX + i) *MAP_CELL_WIDTH, tempY * MAP_CELL_HEIGH);
								  m_RenderSprites.insert(std::pair<int, Sprite*>((tempX + i) * CONST_VALUE + tempY, cell));
								  m_RenderSprites.erase(iter);

							  }
							  else
							  {
								  std::string fileName = getCellName(tempX, tempY + i);
								  log("Up,not find:%d,%s", i, fileName.c_str());
							  }
						  }

						  break;
	}
	case UIGuideShip::DOWN:
	{
					   m_ScreenFirstPos.y--;
					   int tempY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM / 2;
					   int tempX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM / 2;
					   int lastY = m_ScreenFirstPos.y + 3 * SCREEN_HEIGH_NUM / 2 + 1;
					   int lastX = tempX;
					   for (int i = 0; i < SCREEN_WIDTH_NUM * 2; i++)
					   {
						   auto iter = m_RenderSprites.find((lastX + i) * CONST_VALUE + lastY);
						   if (iter != m_RenderSprites.end())
						   {
							   auto cell = iter->second;
							   auto seaCell = dynamic_cast<Sprite*>(cell->getUserObject());
							   std::string fileName = getCellName(tempX + i, tempY);
							   std::string seaName = getSeaCellName(tempX + i, tempY);

							   asyncLoadingImage(cell, seaCell, fileName, seaName);
							   cell->setPosition((tempX + i) *MAP_CELL_WIDTH, tempY * MAP_CELL_HEIGH);
							   m_RenderSprites.insert(std::pair<int, Sprite*>((tempX + i) * CONST_VALUE + tempY, cell));
							   m_RenderSprites.erase(iter);
							   //auto label = dynamic_cast<Label*>(seaCell->getChildByTag(1));
							   //label->setString(seaName.substr(seaName.find_last_of('/')));
						   }
						   else
						   {
							   std::string fileName = getCellName(tempX, tempY + i);
							   log("Down,not find:%d,Name:%s", i, fileName.c_str());
						   }
					   }

					   break;
	}
	case UIGuideShip::LEFT:
	{
					   m_ScreenFirstPos.x--;

					   int tempY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM / 2;
					   int tempX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM / 2;
					   int lastY = tempY;
					   int lastX = m_ScreenFirstPos.x + 3 * SCREEN_WIDTH_NUM / 2;
					   for (int i = 0; i < SCREEN_HEIGH_NUM * 2; i++)
					   {
						   auto iter = m_RenderSprites.find(lastX * CONST_VALUE + lastY + i);
						   if (iter != m_RenderSprites.end())
						   {
							   auto cell = iter->second;
							   auto seaCell = dynamic_cast<Sprite*>(cell->getUserObject());
							   std::string fileName = getCellName(tempX, tempY + i);
							   std::string seaName = getSeaCellName(tempX, tempY + i);
							   asyncLoadingImage(cell, seaCell, fileName, seaName);
							   m_RenderSprites.insert(std::pair<int, Sprite*>(tempX * CONST_VALUE + i + tempY, cell));
							   m_RenderSprites.erase(iter);

							   cell->setPosition(tempX *MAP_CELL_WIDTH, (tempY + i) * MAP_CELL_HEIGH);
							   //auto label = dynamic_cast<Label*>(seaCell->getChildByTag(1));
							   //label->setString(seaName.substr(seaName.find_last_of('/')));
						   }
						   else
						   {
							   std::string fileName = getCellName(tempX, tempY + i);
							   log("Left,not find:%d,Name:%s", i, fileName.c_str());

						   }
					   }

					   break;
	}
	case UIGuideShip::RIGHT:
	{
						m_ScreenFirstPos.x++;

						int tempY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM / 2;
						int tempX = m_ScreenFirstPos.x + 3 * SCREEN_WIDTH_NUM / 2 - 1;
						int lastY = tempY;
						int lastX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM / 2 - 1;
						for (int i = 0; i < SCREEN_HEIGH_NUM * 2; i++)
						{
							auto iter = m_RenderSprites.find(lastX * CONST_VALUE + lastY + i);
							if (iter != m_RenderSprites.end())
							{
								auto cell = iter->second;
								auto seaCell = dynamic_cast<Sprite*>(cell->getUserObject());

								std::string fileName = getCellName(tempX, tempY + i);
								std::string seaName = getSeaCellName(tempX, tempY + i);

								asyncLoadingImage(cell, seaCell, fileName, seaName);
								auto texture = cell->getTexture();
								cell->setPosition(tempX *MAP_CELL_WIDTH, (tempY + i) * MAP_CELL_HEIGH);
								m_RenderSprites.insert(std::pair<int, Sprite*>(tempX * CONST_VALUE + i + tempY, cell));
								m_RenderSprites.erase(iter);
								//auto label = dynamic_cast<Label*>(seaCell->getChildByTag(1));
								//label->setString(seaName.substr(seaName.find_last_of('/')));

							}
							else
							{
								std::string fileName = getCellName(tempX, tempY + i);
								log("Right,not find:%d,Name:%s", i, fileName.c_str());
							}
						}

						break;
	}
	default:
		break;
	}
}

void UIGuideSailManage::asyncLoadingImage(Sprite* land, Sprite* sea, std::string& landName, std::string& seaName)
{

	for (auto iter = m_UnLoadImage_sea.begin(); iter != m_UnLoadImage_sea.end();)
	{
		if ((*iter)->getTag() == -10)
		{
			(*iter)->setTag(-100);
			iter = m_UnLoadImage_sea.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	for (auto iter = m_UnLoadImage_land.begin(); iter != m_UnLoadImage_land.end();)
	{
		if ((*iter)->getTag() == -10)
		{
			(*iter)->setTag(-100);
			iter = m_UnLoadImage_land.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	int land_tag = atoi(landName.substr(landName.find_last_of('/') + 1, landName.find('.') - landName.find_last_of('/') - 1).c_str());
	int sea_tag = atoi(seaName.substr(seaName.find_last_of('/') + 1, seaName.find('.') - seaName.find_last_of('/') - 1).c_str());
	land->setTag(land_tag);
	auto landTexture = TextureCache::getInstance()->getTextureForKey(landName);

	if (landTexture)
	{
		land->setTexture(landTexture);
	}
	else
	{
		m_UnLoadImage_land.push_back(land);
		TextureCache::getInstance()->addImageAsync(landName, CC_CALLBACK_1(UIGuideSailManage::texture2DCallback_land, this));
	}
}
void UIGuideSailManage::texture2DCallback_land(Texture2D* texture)
{
	for (auto iter = m_UnLoadImage_land.begin(); iter != m_UnLoadImage_land.end();)
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
void UIGuideSailManage::openDialog()
{
	ProtocolThread::GetInstance()->getCityStatus(m_nCityId, UILoadingIndicator::createWithMask(this, 4));
}

void UIGuideSailManage::setCityId(int8_t cityId)
{
	m_nCityId = cityId;
}

bool UIGuideSailManage::onTouchBegan(Touch *touch, Event *unused_event)
{
	Vec2 pos = m_MapsLayer->convertToNodeSpace(touch->getLocation());
	//log("touchPos:%f,%f",pos.x,pos.y);
	TextureCache::getInstance()->removeUnusedTextures();
	return true;
}
void UIGuideSailManage::onTouchCancelled(Touch *touch, Event *unused_event)
{

}

void UIGuideSailManage::onTouchEnded(Touch *touch, Event *unused_event)
{

	Point touchPos = this->convertTouchToNodeSpace(touch);//屏幕坐标

	if (saillngHandPic)
	{
		Point touchPosInMaps = m_MapsLayer->convertToNodeSpace(touchPos);//海上
		if (saillngHandPic->getBoundingBox().containsPoint(touchPosInMaps))
		{		
			setShipDirect(touchPos);
 			if (m_pShipImage)
 			{
 				UIGuideShip* ship = dynamic_cast<UIGuideShip*>(m_pShipImage);
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(false);
				ship->setGoldPosition(saillngHandPic->getPosition());
 			} 		
				saillngHandPic->setVisible(false);
				saillngHandPic->setPosition(Vec2(0, 0));
		}
	}
}
void UIGuideSailManage::onTouchMoved(Touch *touch, Event *unused_event)
{
	Point pos = this->convertTouchToNodeSpace(touch);

	Point diffPos = ccpSub(pos, pos);

	Point  tempPoint = m_MapsLayer->getPosition() + diffPos;
	if (tempPoint.x > 0)
	{
		tempPoint.x = 0;
	}
	if (tempPoint.y > 0)
	{
		tempPoint.y = 0;
	}
	if (std::abs(tempPoint.x) > (MAP_WIDTH - SCREEN_WIDTH_NUM) * MAP_CELL_WIDTH)
	{
		tempPoint.x = -(MAP_WIDTH - SCREEN_WIDTH_NUM) * MAP_CELL_WIDTH;
	}
	if (std::abs(tempPoint.y) > (MAP_HEIGH - SCREEN_HEIGH_NUM) * MAP_CELL_HEIGH)
	{
		tempPoint.y = -(MAP_HEIGH - SCREEN_HEIGH_NUM) * MAP_CELL_HEIGH;
	}
}

void UIGuideSailManage::setShipDirect(Point pos)
{
	Point ship_pos = m_pShipImage->getPosition();
	float angle = ship_pos.getAngle(pos);
	log("touch Pos Angle:%f", angle);
}
bool UIGuideSailManage::checkFrontBlock(Point pos)
{
	int x = pos.x / MAP_CELL_WIDTH;
	int y = pos.y / MAP_CELL_HEIGH;
	if (MAPINDEX[x][y] == 1)
	{
		return false;
	}
	return true;
}
int64_t UIGuideSailManage::getCurrentTimeUsev()
{
	timeval timeStruc;
	gettimeofday(&timeStruc, nullptr);
	int64_t sec = int64_t(timeStruc.tv_sec) * 1000;
	return sec + timeStruc.tv_usec / 1000;
}

void UIGuideSailManage::update(float f)
{
	struct timeval start, end;
	gettimeofday(&start, 0);

	int64_t curTime = getCurrentTimeUsev();
	int64_t time_val = curTime - m_nShipMoveTime;
	m_pSeaManage->setCurrentDays_pos(m_nSailDays, m_pShipImage->getPosition());
		if (time_val > SHIP_MOVE_VAL)
		{
			m_nShipMoveTime = curTime;
			loadMapLine();

			setHandPicPosByNationId();
			Vec2 ship_pos = m_pShipImage->getPosition();
			if (SINGLE_HERO->shipGuideEnemy)
			{
				checkGuardShip(ship_pos, f);
			}
		}
		if (m_nNpcId != 0)
		{
			m_npcIsActive -= f;
			if (m_npcIsActive < 0)
			{
				for (int i = 0; i < m_vNPCShipsD.size(); i++)
				{
					if (m_vNPCShipsD.at(i)->getNpcId() == m_nNpcId)
					{
						m_vNPCShipsD.at(i)->setAciveAttack(true);
						m_nNpcId = 0;
						break;
					}
				}
			}
		}
	gettimeofday(&end, 0);
}
void UIGuideSailManage::addSeaEvents()
{
	if (m_pLeaveCityResult->blockinfo->n_events < 1)
	{
		return;
	}
	SeaEventDefine** events = m_pLeaveCityResult->blockinfo->events;

	Button* event_1 = Button::create(getShipTopIconPath(1));
	m_MapsLayer->addChild(event_1, 10, 100);
	event_1->setName("button_event_1");
	event_1->addTouchEventListener(CC_CALLBACK_2(UIGuideSailHUD::menuCall_func, dynamic_cast<UIGuideSailHUD*>(m_pMapUI)));
	int x = m_pLeaveCityResult->blockinfo->events[0]->x;
	int y = MAP_CELL_HEIGH * MAP_WIDTH - m_pLeaveCityResult->blockinfo->events[0]->y;
	event_1->setTag(m_pLeaveCityResult->blockinfo->events[0]->eventid);
	event_1->setPosition(Vec2(x, y));
}

void UIGuideSailManage::genenrateEvent(SeaEventDefine** tempEvent, int n_events)
{
	m_vNPCShipsD.clear();
	m_vNPCShipsF.clear();
	for (int i = 0; i < n_events; i++)
	{
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
			npcshipInfo.IsNeutralChange = false;
		
			UINPCShip* npcShip = UINPCShip::create(npcshipInfo);
			//npcShip->addTouchEventListener(m_pMapUI, toucheventselector(MapUIGuide::onSeaEvent_npc));
			int x = 0;
			int y = 0;
			if (tempEvent[i]->x == 0 && tempEvent[i]->y == 0)
			{
				int x = getGuideEnemyShipPos().x;
				int y = getGuideEnemyShipPos().y;
			}	
			if (SINGLE_HERO->shipGuideEnemy)
			{
				m_EventLayer->addChild(npcShip, 10, 100);
				npcShip->setPosition(getGuideEnemyShipPos());		
			}
#ifdef _DEBUG
			std::string s_pos = StringUtils::format("(%d - %d)", x, y);
			Text* t_pos = Text::create(s_pos, "", 26);
			t_pos->setPosition(Vec2(100, 100));
			t_pos->setScale(0.5f);
			npcShip->addChild(t_pos);
#endif
			m_vNPCShipsD.push_back(npcShip);
		}
	}
	m_EventLayer->setCameraMask(4, true);
	m_EventLayer->setSubNodeMaxCameraMask(4);
}

void UIGuideSailManage::showSeaEvent(TVSeaEvent* temEvent)
{
	if (dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->m_bIsBlocking){
		return;
	}
	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->m_bIsBlocking = true;

	dynamic_cast<UIGuideShip*>(m_pShipImage)->setSeaAccident(true);
	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->showSeaEvent(temEvent);
	dynamic_cast<UIGuideShip*>(m_pShipImage)->startPauseTime();
}

void UIGuideSailManage::seaEventEnd()
{
	if (m_pSeaManage == nullptr)
	{
		return;
	}
	m_pSeaManage->startSeaEvent();
	dynamic_cast<UIGuideShip*>(m_pShipImage)->setSeaAccident(false);
	dynamic_cast<UIGuideShip*>(m_pShipImage)->stopPauseTime();
}
void UIGuideSailManage::seaEventStart()
{
	m_pSeaManage->pauseSeaEvent();
	dynamic_cast<UIGuideShip*>(m_pShipImage)->setSeaAccident(true);
	dynamic_cast<UIGuideShip*>(m_pShipImage)->startPauseTime();
}
void UIGuideSailManage::addSeaEvent_leavCity()
{
	m_pSeaManage->geneEvents(m_pLeaveCityResult->blockinfo->events, m_pLeaveCityResult->blockinfo->n_events);
	genenrateEvent(m_pLeaveCityResult->blockinfo->events, m_pLeaveCityResult->blockinfo->n_events);
}

void UIGuideSailManage::addSeaEvent_nextBlock(const SeaBlockInfo *info)
{
	if (!info) return;
	m_pSeaManage->geneEvents(info->events, info->n_events);
	genenrateEvent(info->events, info->n_events);

}

void UIGuideSailManage::addSeaEvent(SeaEventDefine** events, int n_events)
{
	m_pSeaManage->geneEvents(events, n_events);
	genenrateEvent(events, n_events);
}
void UIGuideSailManage::flashIconPosXWithText(Text* text, ImageView* image){

	float textPosX = text->getPositionX();
	float textSizeX = text->getSize().width;
	float imageX = image->getSize().width;

	image->setPositionX(textPosX - textSizeX - imageX - 10);

}
//判断是否与npc战斗
void UIGuideSailManage::checkGuardShip(Vec2 pos, float f)
{
	//	m_NPCShips
	for (auto &m_npcShip : m_vNPCShipsD)
	{
		Vec2 npc_pos = m_npcShip->getPosition();
		double dis = fabs((pos.x - npc_pos.x) * (pos.x - npc_pos.x) + (pos.y - npc_pos.y) * (pos.y - npc_pos.y));
		UINPCShip* npcShip = dynamic_cast<UINPCShip*>(m_npcShip);
		if (!npcShip->getIsActiveAttack())
		{
			continue;
		}
		else
		{
 			if (dis < 50000)
 			{
				npcShip->isShowGuardCircle(true, f);
				if (dis < 20000)
				{
					if (!m_ischufa)
					{
						m_ischufa = true;
						m_pSeaManage->setGuideSeaEventHalfRange(npcShip->n);
						m_pSeaManage->isHappenEvent(m_pSeaManage->getEvents()[0]);
					}				
				}
			}
			else
			{
				npcShip->isShowGuardCircle(false, f);
			}
		}
	}
}
void UIGuideSailManage::openfindNewCity()
{
	dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->findNewCity(m_nCityId);
}
Vec2 UIGuideSailManage::guideHandPosFirst()
{
	Vec2  point = Vec2(0, 0);
	if (SINGLE_HERO->m_iNation==1)
	{
		point = Vec2(SAIILNG_GUILD_HANDPIC_POS_LISBON[0][0], SAIILNG_GUILD_HANDPIC_POS_LISBON[0][1]);
	}
	else if (SINGLE_HERO->m_iNation == 2)
	{
		point = Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[0][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[0][1]);
	}
	else if (SINGLE_HERO->m_iNation == 3)
	{
		point = Vec2(SAIILNG_GUILD_HANDPIC_POS_LONDON[0][0], SAIILNG_GUILD_HANDPIC_POS_LONDON[0][1]);
	}
	else if (SINGLE_HERO->m_iNation ==4)
	{
		point = Vec2(SAIILNG_GUILD_HANDPIC_POS_NETHERLANDS[0][0], SAIILNG_GUILD_HANDPIC_POS_NETHERLANDS[0][1]);
	}
	else if (SINGLE_HERO->m_iNation == 5)
	{
		point = Vec2(SAIILNG_GUILD_HANDPIC_POS_GENOA[0][0], SAIILNG_GUILD_HANDPIC_POS_GENOA[0][1]);
	}
	return point;
}
Vec2 UIGuideSailManage::getGuideEnemyShipPos()
{
	Vec2  point = Vec2(0, 0);
	if (SINGLE_HERO->m_iNation == 1)
	{
		point = Vec2(SAIILNG_GUILD_ENEMYSHIP[0][0], SAIILNG_GUILD_ENEMYSHIP[0][1]);
	}
	else if (SINGLE_HERO->m_iNation == 2)
	{
		point = Vec2(SAIILNG_GUILD_ENEMYSHIP[1][0], SAIILNG_GUILD_ENEMYSHIP[1][1]);
	}
	else if (SINGLE_HERO->m_iNation == 3)
	{
		point = Vec2(SAIILNG_GUILD_ENEMYSHIP[2][0], SAIILNG_GUILD_ENEMYSHIP[2][1]);
	}
	else if (SINGLE_HERO->m_iNation == 4)
	{
		point = Vec2(SAIILNG_GUILD_ENEMYSHIP[3][0], SAIILNG_GUILD_ENEMYSHIP[3][1]);
	}
	else if (SINGLE_HERO->m_iNation == 5)
	{
		point = Vec2(SAIILNG_GUILD_ENEMYSHIP[4][0], SAIILNG_GUILD_ENEMYSHIP[4][1]);
	}
	return point;
}
void UIGuideSailManage::setHandPicPosByNationId()
{
	Vec2 ship_pos = m_pShipImage->getPosition();
	if (SINGLE_HERO->m_iNation == 1)//里斯本
	{
	}
	else if (SINGLE_HERO->m_iNation == 2)//西班牙
	{
		//碰撞第1次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[0][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[0][1])) < 20)
		{
			if (!guideShipStop_1)
			{
				guideShipStop_1 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{
					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[1][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[1][1]));
				}
			}
		}
		//碰撞第2次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[1][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[1][1])) < 20)
		{
			if (!guideShipStop_2)
			{
				guideShipStop_2 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{
					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[2][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[2][1]));
				}
			}
		}
		//碰撞第3次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[2][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[2][1])) < 20)
		{
			if (!guideShipStop_3)
			{
				guideShipStop_3 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{
					
					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[3][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[3][1]));
				}
			}
		}
		//碰撞第4次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[4][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[4][1])) < 20)
		{
			if (!guideShipStop_4)
			{
				guideShipStop_4 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{

					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[5][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[5][1]));
				}
			}
		}
		//碰撞第5次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[5][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[5][1])) < 20)
		{
			if (!guideShipStop_5)
			{
				guideShipStop_5 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{

					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[6][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[6][1]));
				}
			}
		}
	}
	else if (SINGLE_HERO->m_iNation == 3)//英国
	{	
		//碰撞第1次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_LONDON[0][0], SAIILNG_GUILD_HANDPIC_POS_LONDON[0][1])) < 20)
		{
			if (!guideShipStop_1)
			{
				guideShipStop_1 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{
					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_LONDON[1][0], SAIILNG_GUILD_HANDPIC_POS_LONDON[1][1]));
				}
			}
		}
		//第二次为战斗
		//碰撞第3次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_LONDON[2][0], SAIILNG_GUILD_HANDPIC_POS_LONDON[2][1])) < 20)
		{
			if (!guideShipStop_2)
			{
				guideShipStop_2 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				//assert(0);
				if (saillngHandPic)
				{
					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_LONDON[3][0] + saillngHandPic->getContentSize().width/2,
						SAIILNG_GUILD_HANDPIC_POS_LONDON[3][1] + 64 * 0.6 - saillngHandPic->getContentSize().height / 2 * 0.3));
				}
			}
		}
	}
	else if (SINGLE_HERO->m_iNation == 4)//荷兰
	{
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_NETHERLANDS[1][0], SAIILNG_GUILD_HANDPIC_POS_NETHERLANDS[1][1])) < 20)
		{
			if (!guideShipStop_1)
			{
				guideShipStop_1 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{

					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_NETHERLANDS[2][0], SAIILNG_GUILD_HANDPIC_POS_NETHERLANDS[2][1]));
				}
			}
		}
	}
	else if (SINGLE_HERO->m_iNation == 5)//热那亚
	{
		//碰撞第1次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_GENOA[0][0], SAIILNG_GUILD_HANDPIC_POS_GENOA[0][1])) < 20)
		{
			if (!guideShipStop_1)
			{
				guideShipStop_1 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{
					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_GENOA[1][0], SAIILNG_GUILD_HANDPIC_POS_GENOA[1][1]));
				}
			}
		}
		//第二次碰海盗
		//碰撞第3次小手
		if (ship_pos.getDistanceSq(Vec2(SAIILNG_GUILD_HANDPIC_POS_GENOA[2][0], SAIILNG_GUILD_HANDPIC_POS_GENOA[2][1])) < 20)
		{
			if (!guideShipStop_2)
			{
				guideShipStop_2 = true;
				dynamic_cast<UIGuideSailHUD*>(m_pMapUI)->shipPause(true);
				if (saillngHandPic)
				{
					saillngHandPic->setVisible(true);
					saillngHandPic->setPosition(Vec2(SAIILNG_GUILD_HANDPIC_POS_GENOA[3][0], SAIILNG_GUILD_HANDPIC_POS_GENOA[3][1]));
				}
			}
		}
	}
}
void UIGuideSailManage::setAfterWarHandPic()
{
	if (SINGLE_HERO->m_iNation == 1)
	{
		saillngHandPic->setPosition(SAIILNG_GUILD_HANDPIC_POS_LISBON[0][0], SAIILNG_GUILD_HANDPIC_POS_LISBON[0][1]);
	}
	else if (SINGLE_HERO->m_iNation == 2)
	{
		saillngHandPic->setPosition(SAIILNG_GUILD_HANDPIC_POS_SEVILLA[4][0], SAIILNG_GUILD_HANDPIC_POS_SEVILLA[4][1]);
	}
	else if (SINGLE_HERO->m_iNation == 3)
	{
		saillngHandPic->setPosition(SAIILNG_GUILD_HANDPIC_POS_LONDON[2][0], SAIILNG_GUILD_HANDPIC_POS_LONDON[2][1]);
	}
	else if (SINGLE_HERO->m_iNation == 4)
	{
		saillngHandPic->setPosition(SAIILNG_GUILD_HANDPIC_POS_NETHERLANDS[1][0], SAIILNG_GUILD_HANDPIC_POS_NETHERLANDS[1][1]);
	}
	else if (SINGLE_HERO->m_iNation == 5)
	{
		saillngHandPic->setPosition(SAIILNG_GUILD_HANDPIC_POS_GENOA[2][0], SAIILNG_GUILD_HANDPIC_POS_GENOA[2][1]);
	}
}
bool UIGuideSailManage::isFarFromEnemy()
{

	auto npcship = dynamic_cast<UINPCShip*>(m_EventLayer->getChildByTag(100));
	if (npcship != nullptr)
	{
		 Vec2 pos1 = m_pShipImage->getPosition();
		 Vec2 pos2 = npcship->getPosition();
		 float distance = pos1.distance(pos2);
//		 log("%f", distance);
		 if (distance < 100)
		 {
			 return false;
		 }
	}
	else
	{
		return true;
	}
	return true;
}
