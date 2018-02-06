#include "UIMap.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
#include "UITips.h"
#include "UIMain.h"
#include "TVSailDefineVar.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

//Unreasonable_code_9;纯数字应该配置化
#define MAP_NUM 19


using namespace rapidjson;
using namespace cocostudio;

UIMap::UIMap():
	m_nMapIndex(1),
	m_nLastMapIndex(-1),
	m_pResult(nullptr)
{
	m_pFindCityNum=nullptr;
	m_pTextTitle=nullptr;
	m_pButtonLook=nullptr;
	m_pCities = nullptr;
	m_pShipIcon = nullptr;
	m_nMapTag = 0;
	m_pImageMap = nullptr;
	m_pImageMapBg=nullptr;
	m_vDirectWidgets.clear();
}

UIMap::~UIMap()
{
	
	//SINGLE_SHOP->releaseLandData();
	
	m_vDirectWidgets.clear();
	m_pCities=nullptr;

}

UIMap*UIMap::createMap(int tag)
{
	UIMap* map = new UIMap;
	CC_RETURN_IF(map);
	if (map->init())
	{
		map->m_nMapTag=tag;
		map->autorelease();
		return map;
	}
	CC_SAFE_RELEASE(map);
	return nullptr;
}
bool UIMap::init()
{
	log("MapLayer init");
	if (UIBasicLayer::init())
	{
		do
		{
		CC_BREAK_IF(!UIBasicLayer::init());
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIMap::onServerEvent,this),this);
		initStaticData(0);
		//this->scheduleOnce(schedule_selector(MapLayer::initStaticData),0);
		ProtocolThread::GetInstance()->getVisitedCities(UILoadingIndicator::create(this));
		}
		while (0);
		return true;
	}
	return false;
}

void UIMap::initStaticData(float f)
{
	auto fakeRoot=dynamic_cast<Node*>(CSLoader::createNode(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[18] + ".csb").c_str()));
	m_vRoot=(Widget*)fakeRoot;
	this->addChild(m_vRoot);
	addEventForChildren(m_vRoot);	
	auto panel_map=m_vRoot->getChildByName("panel_map");
	auto panel_map_content=panel_map->getChildByName("panel_map_content");
	m_pImageMapBg=panel_map_content->getChildByName<ImageView*>("image_map_bg");
	m_pImageMap=m_pImageMapBg->getChildByName<ImageView*>("image_map_1");
	m_pImageMapBg->setVisible(false);

	const char *directName[] = {"button_on","button_left","button_under","button_right"};
	for (int i = 0; i < 4; i++)
	{
		auto btn_direct =panel_map_content->getChildByName<Button*>(directName[i]);

		m_vDirectWidgets.pushBack(btn_direct);
		m_vDirectWidgets.at(i)->setVisible(false);

	}

	auto image_title_bg=panel_map->getChildByName("image_title_bg");
	m_pTextTitle=image_title_bg->getChildByName<Text*>("label_title");

	m_pButtonLook=panel_map->getChildByName<Button*>("button_look");
	m_pButtonLook->setVisible(false);

	m_pFindCityNum=panel_map->getChildByName<Widget*>("panel_max_day");
	m_pShipIcon=ImageView::create(SHIP_ICON_PATH);
	m_pImageMap->addChild(m_pShipIcon);
	m_pShipIcon->setPosition(Vec2(1000,1000));
	//when isAutosea (true) to the land,stop Auto state
	if (m_nMapTag==MAP_TAG_MAINCITY)
	{
		SINGLE_HERO->m_bShipStateIsAuto=false;
	}
}

void UIMap::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType!=Widget::TouchEventType ::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_map_close))//关闭按钮
	{
		 
		if (!SINGLE_HERO->m_bShipStateIsAuto&&(m_nMapTag==MAP_TAG_SAILING||m_nMapTag==MAP_TAG_UNKNOWN))
		{
			cocos2d::NotificationCenter::getInstance()->postNotification("mapHaveClosed_notAuto",nullptr);
		}	
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		this->removeFromParentAndCleanup(true);
		
		return;
	}
	if (isButton(button_left))//左
	{
		changeMapByIndex(D_LEFT);
		
		return;
	}
	if (isButton(button_right))//右
	{
		changeMapByIndex(D_RIGHT);
		
		return;
	}
	if (isButton(button_on))//上
	{
		changeMapByIndex(D_UP);
		
		return;
	}
	if (isButton(button_under))//下
	{
		changeMapByIndex(D_DOWN);
		return;
	}
	if (isButton(button_look))//放大镜
	{
		m_nMapTag=MAP_TAG_UNKNOWN;

	    m_pButtonLook->setVisible(false);
		this->removeAllChildrenWithCleanup(true);
		//clear vector
		m_vDirectWidgets.clear();
		UIMap::init();
		this->setCameraMask(4,true);
	
		return;
	}
}


void UIMap::updateMapsInfo()
{

	m_pImageMapBg->setVisible(true);

	//SINGLE_SHOP->loadLandData();
	auto landInfo = SINGLE_SHOP->getLandInfo();

	for (auto i = 0;i < MAP_NUM;i++)
	{
		Rect tempRect = Rect(landInfo[i + 1].left_down_x, landInfo[i + 1].left_down_y, 7518, 3727);
		Rect tempRectReal = Rect(landInfo[i + 1].left_down_x, landInfo[i + 1].left_down_y, landInfo[i + 1].right_up_x- landInfo[i + 1].left_down_x, 3727);
		Vec2 cityPos = Vec2(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].x, SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].y);
		if (m_nMapTag==MAP_TAG_UNKNOWN)
		{
			cityPos = Vec2(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iBornCity].x, SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iBornCity].y);
		}
		if (tempRectReal.containsPoint(cityPos))
			{
				m_nMapIndex = i + 1;	
				m_pImageMap->loadTexture( StringUtils::format("ship/seaFiled/map_%d.jpg",m_nMapIndex));
				addCitiesForMap(m_pImageMap, tempRect, m_nMapIndex);
				updateDirectButton();
		}
	}
}

Vec2 UIMap::getMapOffset(const ImageView* view){

	const Size &contSize = view -> getContentSize();
	float offsetX = contSize.width / 894 * 34;
	float offsetY = contSize.width / 462 * 34;

	Vec2 offset = Vec2(offsetX, offsetY);

	return offset;

}


bool UIMap::addCitiesForMap(ImageView* view,Rect& fieldSize,int index)
{
	
	auto landInfo = SINGLE_SHOP->getLandInfo();
	const Size &contSize = view->getContentSize();

	Vec2 offset = getMapOffset(view);

	float zoomFactor_x = (contSize.width - offset.x) / fieldSize.size.width;
	float zoomFactor_y = (contSize.height - offset.y) / fieldSize.size.height;

	if (!m_pResult)
	{
		return false;
	}

	auto citiesInfo = SINGLE_SHOP->getCitiesInfo();

	if(!m_pCities)
	{
		m_pCities=Layout::create();
		m_pCities->setCameraMask(_cameraMask);
		view->addChild(m_pCities);
	}
	m_pCities->setContentSize(contSize);
	bool isInMap = false;
	for (int i = 0; i < m_pResult->n_city; i++)
	{
		CityDefine* cityDefine = m_pResult->city[i];
		int cityId = cityDefine->cityid;
		CITYINFO &cityInfo = citiesInfo[cityId];
		Vec2 cityPos = Vec2(cityInfo.x,cityInfo.y);
		Rect fieldRectReal = Rect(landInfo[index].left_down_x,landInfo[index].left_down_y, landInfo[index].right_up_x - landInfo[index].left_down_x, 3721);
		if (fieldRectReal.containsPoint(cityPos))
		{
			Label* cityName = nullptr;
			cityName = Label::createWithSystemFont(cityInfo.name, "", MAP_FONT_SIZE);
			Button* cityIcon = Button::create(CITY_ICON_PATH);
			ImageView * city_flag = ImageView::create();
			city_flag->loadTexture(getCountryIconPath(m_pResult->city[i]->nation));
			cityName->setTextColor(MAP_FRIEND_TEXT_COLOR);
			cityName->enableOutline(MAP_FRIEND_TEXT_OUTLINE,MAP_OUTLINE_SIZE);
			int32_t friendvalue = cityDefine->friendvalue;
			cityIcon->setTouchEnabled(false);//不可点击
			cityIcon->setCameraMask(_cameraMask);
			cityName->setCameraMask(_cameraMask);
			if (friendvalue >= 0){

				cityIcon->loadTextureNormal(CITY_ICON_PATH);
				cityName->setTextColor(MAP_FRIEND_TEXT_COLOR);
				//中立国家变敌对
				for (int j=0;j<m_pResult->n_wantedforceids;j++)
				{
					if (m_pResult->city[i]->nation==m_pResult->wantedforceids[j])
					{
						cityIcon->loadTextureNormal(EU_ENEMY_S_PATH);
						cityName->setTextColor(MAP_ENEMY_TEXR_CLOLR);
						cityName->enableOutline(MAP_ENEMY_TEXT_OUTLINE, MAP_OUTLINE_SIZE);
					}
					
				}

			}
			else if (friendvalue < 0){

				cityIcon->loadTextureNormal(EU_ENEMY_S_PATH);
				cityName->setTextColor(MAP_ENEMY_TEXR_CLOLR);
				cityName->enableOutline(MAP_ENEMY_TEXT_OUTLINE, MAP_OUTLINE_SIZE);
				if (8 == cityInfo.nation){
				
					cityIcon->loadTextureNormal(OTTTOMAN_S_PATH);
					cityName->setTextColor(MAP_ENEMY_TEXR_CLOLR);
					cityName->enableOutline(MAP_ENEMY_TEXT_OUTLINE, MAP_OUTLINE_SIZE);
				}
			}
			cityIcon->setTag(START_INDEX + cityDefine->cityid);
			cityIcon->setLocalZOrder(10);
			cityIcon->setName("button_city_flag");
			cityIcon->addTouchEventListener(CC_CALLBACK_2(UIMap::menuCall_func, this));

			Vec2 diffPos = cityPos - fieldSize.origin;
			if (index ==10)//加勒比海东
			{
				diffPos.setPoint(diffPos.x * zoomFactor_x + offset.x / 2, (diffPos.y) * zoomFactor_y + offset.y / 2);
			}
			else
			{
				diffPos.setPoint((diffPos.x + 7518 - (landInfo[index].right_up_x - landInfo[index].left_down_x)) * zoomFactor_x + offset.x / 2, (diffPos.y) * zoomFactor_y + offset.y / 2);
			}
		
			cityIcon->setPosition(diffPos);
			
			cityName->setPosition(diffPos);
			cityName->setPositionY(cityName->getPositionY() - cityIcon->getContentSize().height /2);


			city_flag->setScale(0.4);
			city_flag->setAnchorPoint(Vec2(1, 0.5));
			city_flag->setPosition(cityName->getPosition());
			city_flag->setPositionX(city_flag->getPositionX() - cityName->getContentSize().width / 2);
		

			if (m_nMapTag == MAP_TAG_MAINCITY) 
			{
				if (cityDefine->cityid == SINGLE_HERO->m_iCityID)
				{

					m_pShipIcon->setLocalZOrder(11);
					m_pShipIcon->setPosition(diffPos);

					isInMap = true;
				}
			}
			m_pCities->addChild(cityIcon,10);
			m_pCities->addChild(cityName,12);
			m_pCities->addChild(city_flag,11);
		}
	}
	if(m_nMapTag == MAP_TAG_MAINCITY)
	{
		m_pShipIcon->setVisible(isInMap);
	}
	return true;
}

void UIMap::changeMapByIndex(MAP_DIRCT direct)
{
	int mapTempIndex = MAP_INDEX[m_nMapIndex][direct];
	if (mapTempIndex < 1)
	{
		return;
	}
	if(m_pCities)
	{
		m_pCities->removeFromParent();
		m_pCities = nullptr;
	}
	m_pImageMap->loadTexture(StringUtils::format("ship/seaFiled/map_%d.jpg",mapTempIndex ));
	m_nMapIndex = mapTempIndex;
	
	auto landInfo = SINGLE_SHOP->getLandInfo();
	Rect tempRect = Rect(landInfo[mapTempIndex].left_down_x, landInfo[mapTempIndex].left_down_y,7518,3727);
	Rect tempshipPos = Rect(landInfo[mapTempIndex].left_down_x, landInfo[mapTempIndex].left_down_y, landInfo[mapTempIndex].right_up_x - landInfo[mapTempIndex].left_down_x, 3727);
	log("land: %d %d", landInfo[mapTempIndex].left_down_x, landInfo[mapTempIndex].left_down_y);
	//add city
	addCitiesForMap(m_pImageMap, tempRect, mapTempIndex);
	updateDirectButton();
	//mapTag ==2 means palyers is on the sea.
	if(m_nMapTag==MAP_TAG_SAILING)
	{
		if (tempshipPos.containsPoint(SINGLE_HERO->shippos))
		{
			m_pShipIcon->setVisible(true);
		}else{
			m_pShipIcon->setVisible(false);
		}
	}
}


void UIMap::updateDirectButton()
{

	m_pTextTitle->setString(SINGLE_SHOP->getLandInfo()[m_nMapIndex].name);
	for (int i = 0; i < 4; i++)
	{
		auto item = m_vDirectWidgets.at(i);
		if (MAP_INDEX[m_nMapIndex][i] != 0)
		{
			item->setVisible(true);
		}else
		{
			item->setVisible(false);
		}
	}
}
void UIMap::showMyVisitedCities(const GetVisitedCitiesResult*result)
{

	if (!result) return;
	auto panel_map=m_vRoot->getChildByName("panel_map");
	auto panel_max_day=panel_map->getChildByName("panel_max_day");
	auto label_max_day_num=panel_max_day->getChildByName<Text*>("label_max_day_num");
	label_max_day_num->setString(StringUtils::format("%d", result->visitedcitynum));

}
void UIMap::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent( message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetVisitedCitiesResult:
		{
			auto result = (GetVisitedCitiesResult*)message;
			if (result->failed == 0)
			{
				if (m_pResult)
				{
					get_visited_cities_result__free_unpacked(m_pResult,0);
				}
				m_pResult = result;
				if (m_nMapTag==MAP_TAG_MAINCITY||m_nMapTag==MAP_TAG_UNKNOWN)//main city map
				{
					updateMapsInfo();

				}
				else if (m_nMapTag==MAP_TAG_SAILING)//sail map
				{
					m_pImageMapBg->setVisible(true);
					if (SINGLE_HERO->m_bShipStateIsAuto)
					{
						hideButton(true);
						this->schedule(schedule_selector(UIMap::updateShipPos), 0.3f);
					}
					else
					{	
						
						shipPositionChange();
						updateDirectButton();
						if (m_nMapIndex==-1)//no map zone
						{
							hideButton(true);
							m_pImageMap->loadTexture("ship/seaFiled/map_no.jpg");
						    m_pShipIcon->setPosition(Vec2(m_pImageMap->getContentSize().width/2,m_pImageMap->getContentSize().height/2));
							m_pFindCityNum->setVisible(false);
							m_pButtonLook->setVisible(true);
							m_pTextTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_UNKNOW_SEA"]);
						}
						else
							{	
							m_pButtonLook->setVisible(false);
							m_pFindCityNum->setVisible(true);
							
							}
						
					}
				}

				showMyVisitedCities(result);
			}else
			{

			}	
			break;
		}
	default:
		break;
	}
}

void UIMap::updateShipPos(float f)
{
	shipPositionChange();
}
void UIMap::hideButton(const bool hide)
{
	auto panel_map1=m_vRoot->getChildByName("panel_map");
	auto panel_map_content1=panel_map1->getChildByName("panel_map_content");
	
	auto panel_map=m_vRoot->getChildByName<Widget*>("panel_map");
	auto panel_map_content=panel_map->getChildByName("panel_map_content");
	auto button_right=panel_map_content->getChildByName<Button*>("button_right");
	auto button_left=panel_map_content->getChildByName<Button*>("button_left");
	auto button_on=panel_map_content->getChildByName<Button*>("button_on");
	auto button_under=panel_map_content->getChildByName<Button*>("button_under");

	if (hide)
	{
		button_right->setVisible(false);
		button_left->setVisible(false);
		button_on->setVisible(false);
		button_under->setVisible(false);
		
	}
}
void UIMap::shipPositionChange()
{
	m_shipPosition=SINGLE_HERO->shippos;
	//SINGLE_SHOP->loadLandData();
	auto landInfo = SINGLE_SHOP->getLandInfo();
	bool b_noMap_zone = true;
	for (auto i = 1;i <= MAP_NUM;i++)
	{
		m_nMapIndex = i;
		cocos2d::Rect seaRect = cocos2d::Rect(landInfo[i].left_down_x, landInfo[i].left_down_y, landInfo[i].right_up_x - landInfo[i].left_down_x, 3727);
		if (seaRect.containsPoint(SINGLE_HERO->shippos))
		{
			m_pImageMap->setVisible(true);
			Rect tempRect = Rect(landInfo[i].left_down_x,landInfo[i].left_down_y,7518,3727);
			if(m_nLastMapIndex != m_nMapIndex){
				log("changeland %d %d",landInfo[i].left_down_x,landInfo[i].left_down_y);
				m_pImageMap->loadTexture(StringUtils::format("ship/seaFiled/map_%d.jpg",m_nMapIndex));
				m_pTextTitle->setString(SINGLE_SHOP->getLandInfo()[m_nMapIndex].name);
				if(m_pResult){
					if(m_pCities)
					{
						m_pCities->removeFromParent();
						m_pCities = nullptr;
					}
					addCitiesForMap(m_pImageMap,tempRect,i);
					m_nLastMapIndex = m_nMapIndex;
				}
			}
			
			Vec2 curZeroPos = Vec2(SmallMaps[m_nMapIndex-1].getMinX(),SmallMaps[m_nMapIndex-1].getMinY());
			Vec2 diffpos=m_shipPosition-curZeroPos;
			Size mapsize=m_pImageMap->getContentSize();
			int border = 18;
			if (i==10)
			{
				diffpos.x = (diffpos.x - border * 2)*(mapsize.width / SmallMaps[m_nMapIndex - 1].size.width);
			}
			else
			{
				diffpos.x = (diffpos.x + 7518 - (landInfo[i].right_up_x - landInfo[i].left_down_x) - border * 2)*(mapsize.width / SmallMaps[m_nMapIndex - 1].size.width);
			}
			
			diffpos.y=(diffpos.y-border*2)*(mapsize.height/SmallMaps[m_nMapIndex-1].size.height);
			if (diffpos.x < 0)
			{
				diffpos.x = 0;

			}else if(diffpos.x > SmallMaps[m_nMapIndex-1].getMaxX())
			{
				diffpos.x = SmallMaps[m_nMapIndex-1].getMaxX();
			}
			if (diffpos.y < 0)
			{
				diffpos.y = 0;
			}else if(diffpos.y > SmallMaps[m_nMapIndex-1].getMaxY())
			{
				diffpos.y = SmallMaps[m_nMapIndex-1].getMaxY();
			}

			m_pShipIcon->setLocalZOrder(10);
			m_pShipIcon->setPosition(diffpos);
			b_noMap_zone = false;
			break;
		}
	}
	//unknown sea zone
	if (b_noMap_zone)
	{
		m_nMapIndex = -1;
		m_nMapTag = MAP_TAG_UNKNOWN;
		m_pImageMap->loadTexture("ship/seaFiled/map_no.jpg");
		m_pTextTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_UNKNOW_SEA"]);
		if (m_pCities)
		{
			m_pCities->removeFromParent();
			m_pCities = nullptr;
		}
		if (m_pShipIcon)
		{
			m_pShipIcon->setLocalZOrder(11);
			m_pShipIcon->setVisible(true);
			m_pShipIcon->setPosition(Vec2(m_pImageMap->getContentSize().width / 2, m_pImageMap->getContentSize().height / 2));
		}
		
	}
}
