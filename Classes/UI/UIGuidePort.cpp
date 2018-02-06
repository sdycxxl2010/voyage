#include "UIGuidePort.h"
#include "MyDrawLine.h"
#include "SailRoad.h"
#include "MainTaskManager.h"
#include "CompanionTaskManager.h"

#include "UIStore.h"
#include "LuaSailScene.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UISailManage.h"
#include "UINoviceStoryLine.h"
#include "TVSceneLoader.h"

#define MAP_NUM 9
static Text* _flagText = nullptr;
static MyDrawLine* _myDrawLine = nullptr;


UIGuidePort::UIGuidePort():
	m_nMapIndex(1),
	m_nFirstCityId(-1),
	m_nAddSupplyAmount(-1),
	m_pResult(nullptr)
{
	m_bIsScale = false;
	m_bIsNeedScale = false;
	m_bMoveActionFlag = true;
	m_bIsAutoToSea=false;
	_flagText = Text::create();
	auto winSize = Director::getInstance()->getWinSize();
	_flagText->setPosition(Vec2(winSize.width*0.5,40));
	_flagText->setString("CUREE INDEX: 1");
#ifndef WIN32
	_flagText->setVisible(false);
#endif
	this->addChild(_flagText,50);
	_flagText->retain();
	_myDrawLine = MyDrawLine::create();
	this->addChild(_myDrawLine,50);
	_myDrawLine->retain();
	m_pSailRoad = new SailRoad;
	m_bSuppleAutoSea = false;
	m_bToseaWithoutSupply=false;
	m_bAideClick = true;
	m_nAutoDays = 0;
	m_pCities = nullptr;
	m_pSetOffButton=nullptr;
	m_pZoomOutButton=nullptr;
	m_pButton_backcity=nullptr;
	m_vDirectWidgets.clear();
	m_vCities.clear();
	m_eUIType = UI_WHARF;
	m_guideStage = 0;
	sprite_hand = nullptr;
	n_costNum = 3500;
	m_dialogLayer = nullptr;
}

UIGuidePort::~UIGuidePort()
{
	m_vDirectWidgets.clear();
	m_vCities.clear();
	_flagText->release();
	_myDrawLine->release();
	//SINGLE_SHOP->releaseLandData();
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}
UIGuidePort* UIGuidePort::create()
{
	UIGuidePort* pup = new UIGuidePort;
	CC_RETURN_IF(pup);
	if (pup->init())
	{
		pup->autorelease();
		return pup;
	}
	CC_SAFE_RELEASE(pup);
	return nullptr;
}

bool UIGuidePort::init()
{
	bool pRet = false;
	
	do 
	{
		CC_BREAK_IF(!UIBasicLayer::init());
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIGuidePort::onServerEvent,this),this);
		ProtocolThread::GetInstance()->getVisitedCities(UILoadingIndicator::create(this,m_eUIType));
		sprite_hand = Sprite::create();
		sprite_hand->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
		sprite_hand->setVisible(false);
		this->addChild(sprite_hand, 10000);
		pRet = true;
	} while (0);
	return pRet;
}


bool UIGuidePort::initStaticData()
{
	bool pRet = false;
	do 
	{
		m_pSailRoad->initSailRoadData();
		openView(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]);
		showCaptainChat();
		chatContentBySupply();
		updateRoleCoin();
		auto  view=	getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName<Widget*>("panel_chief_mate_dialog");
		view->addTouchEventListener(CC_CALLBACK_2(UIGuidePort::closeCaptainChat,this));
		auto t_dock =getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_actionbar")->getChildByName<Text*>("label_dock");

		setButtonsDisable(getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]));
		pRet = true;
		
	} while (0);
	return pRet;
}


void UIGuidePort::notifyCompleted(int actionIndex)
{
	
	switch (actionIndex){
	case MAIN_STORY:
		break;
	case COMPANION_STORY:
		break;
	case SMALL_STORY:
		break;
	default:
		break;
	}
	doNextJob(actionIndex);
}


void UIGuidePort::doNextJob(int actionIndex)
{
	actionIndex++;
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	auto viewTarven = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	switch (actionIndex)
	{
	case MAIN_STORY:
		
		if (MainTaskManager::GetInstance()->checkTaskAction())
		{
			auto view = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]);
			auto mainTask1 = view->getChildByName<Widget*>("panel_actionbar");
			addNeedControlWidgetForHideOrShow(mainTask1, true);
			allTaskInfo(false, MAIN_STORY, 0.5);
			MainTaskManager::GetInstance()->notifyUIPartDelayTime(1.0f);
		}
		else
		{
			notifyCompleted(MAIN_STORY);
		}
		break;
	case COMPANION_STORY:
		if (CompanionTaskManager::GetInstance()->checkTaskAction())
		{
			auto view = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]);
			auto mainTask1 = view->getChildByName<Widget*>("panel_actionbar");
			addNeedControlWidgetForHideOrShow(mainTask1, true);
			allTaskInfo(false, COMPANION_STORY, 0.5);
			CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(1.0f);
		}
		else
		{
			notifyCompleted(COMPANION_STORY);
		}
		break;
	case SMALL_STORY:
		
		if (SINGLE_HERO->m_nTriggerId)
		{
			addNeedControlWidgetForHideOrShow(nullptr, true);
			allTaskInfo(false, SMALL_STORY);
			notifyUIPartDelayTime(0.5);
		}
		else
		{
			notifyCompleted(SMALL_STORY);
		}
		break;
	default:
		break;
	}
}

void UIGuidePort::addMapDirectButton()
{
	const char *directName[] = {"button_on","button_left","button_under","button_right"};
	auto dockMap = getViewRoot(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB]);
	auto panel_map_content=dockMap->getChildByName<Widget*>("panel_map_content");
	for (int i = 0; i < 4; i++)
	{
		auto btn_direct =panel_map_content->getChildByName<Button*>(directName[i]);
		m_vDirectWidgets.pushBack(btn_direct);
		
	}
	
	auto landInfo = SINGLE_SHOP->getLandInfo();
 	for (auto i = 0;i < MAP_NUM;i++)
 	{
		Rect tempRect = Rect(landInfo[i + 1].left_down_x, landInfo[i + 1].left_down_y, 7518, 3727);
		Vec2 cityPos = Vec2(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].x,SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].y);
 		if(tempRect.containsPoint(cityPos))
 		{
 			m_nMapIndex = i + 1;
			m_pImageMap->loadTexture( StringUtils::format("ship/seaFiled/map_%d.jpg",m_nMapIndex));
 			_flagText->setString(String::createWithFormat("CURR INDEX:%d",m_nMapIndex)->_string);
			addCitiesForMap(m_pImageMap,tempRect);
 			updateDirectButton();
 		}
 	}
	 
}

Vec2 UIGuidePort::getMapOffset(const ImageView* view){

	const Size &contSize = view -> getContentSize();
	float offsetX = contSize.width / 894 * 34;
	float offsetY = contSize.width / 462 * 34;
	return Vec2(offsetX, offsetY);

}

void UIGuidePort::addCitiesForMap(ImageView* view,Rect& fieldSize)
{
	const Size &contSize = view->getContentSize();
	Vec2 offset = getMapOffset(view);
	float zoomFactor_x = (contSize.width - offset.x) / fieldSize.size.width;
	float zoomFactor_y = (contSize.height - offset.y) / fieldSize.size.height;

	auto citiesInfo = SINGLE_SHOP->getCitiesInfo();
	if(!m_pCities)
	m_pCities=Layout::create();
	m_pCities->setContentSize(contSize);
	view->addChild(m_pCities);
	for (int i = 0; i < m_pResult->n_city; i++)
	{
		CityDefine* cityDefine = m_pResult->city[i];
		int cityId = cityDefine->cityid;
		CITYINFO &cityInfo = citiesInfo[cityId];
		Vec2 cityPos = Vec2(cityInfo.x,cityInfo.y);
		if (fieldSize.containsPoint(cityPos))
		{
			int32_t friendvalue = cityDefine->friendvalue;
			Label* cityName = Label::createWithSystemFont(cityInfo.name,"",MAP_FONT_SIZE);
			Button* cityIcon = Button::create(CITY_ICON_PATH);
			ImageView * city_flag = ImageView::create();
		
			city_flag->loadTexture(getCountryIconPath(m_pResult->city[i]->nation));
			if (friendvalue >= 0)
			{	
				cityIcon->loadTextureNormal(CITY_ICON_PATH);
				cityName->setTextColor(MAP_FRIEND_TEXT_COLOR);
				cityName->enableOutline(MAP_FRIEND_TEXT_OUTLINE, MAP_OUTLINE_SIZE);
			
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
			else if (friendvalue < 0)
			{
				cityIcon->loadTextureNormal(EU_ENEMY_S_PATH);
				cityName->setTextColor(MAP_ENEMY_TEXR_CLOLR);
				cityName->enableOutline(MAP_ENEMY_TEXT_OUTLINE, MAP_OUTLINE_SIZE);
				if (8 == cityInfo.nation)
				{
					cityIcon->loadTextureNormal(OTTTOMAN_S_PATH);
					cityName->setTextColor(MAP_ENEMY_TEXR_CLOLR);
					cityName->enableOutline(MAP_ENEMY_TEXT_OUTLINE, MAP_OUTLINE_SIZE);
				}
			}

			cityIcon->setTag(START_INDEX + cityDefine->cityid);
			cityIcon->setLocalZOrder(10);
			cityIcon->setName("button_city_flag");
			cityIcon->addTouchEventListener(CC_CALLBACK_2(UIGuidePort::menuCall_func,this));

			if (cityDefine->cityid == SINGLE_HERO->m_iCityID)
			{
				ImageView* ship_icon = ImageView::create(SHIP_ICON_PATH);
				ship_icon->setPosition(cityIcon->getContentSize()/2);
				ship_icon->setLocalZOrder(11);
				ship_icon->setPosition(cityIcon->getContentSize() / 2);
				cityIcon->addChild(ship_icon);				
			}

			Vec2 diffPos = cityPos - fieldSize.origin;
			diffPos.setPoint((diffPos.x) * zoomFactor_x+ offset.x / 2,(diffPos.y) * zoomFactor_y + offset.y / 2);
			cityIcon->setPosition(diffPos);
			m_pCities->addChild(cityIcon, 10);

			cityName->setPosition(diffPos);
			cityName->setPositionY(cityName->getPositionY() - cityIcon->getContentSize().height / 2);
			m_pCities->addChild(cityName,12);

			city_flag->setScale(0.4);
			city_flag->setAnchorPoint(Vec2(1,0.5));
			city_flag->setPosition(cityName->getPosition());
			city_flag->setPositionX(city_flag->getPositionX() - cityName->getContentSize().width / 2);
			m_pCities->addChild(city_flag,11);

			ImageView* city_icon = ImageView::create(CURRENT_CITY_ICON_FLAG);
			city_icon ->setScale(0.5);
			cityIcon->addChild(city_icon);
			city_icon->setPosition(cityIcon->getContentSize()/2);
			m_vCities.pushBack(cityIcon);
		}
	}
}
void UIGuidePort::changeMapByIndex(MAP_DIRCT direct)
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
	//SINGLE_SHOP->loadLandData();
	auto landInfo = SINGLE_SHOP->getLandInfo();
	_flagText->setString(String::createWithFormat("CURR INDEX:%d",m_nMapIndex)->_string);
	Rect tempRect = Rect(landInfo[mapTempIndex].left_down_x,landInfo[mapTempIndex ].left_down_y,7518,3727);
	//add city
	addCitiesForMap(m_pImageMap,tempRect);
	updateDirectButton();

}
void UIGuidePort::updateDirectButton()
{
	for (int i = 0; i < 4; i++)
	{
		auto item = m_vDirectWidgets.at(i);
		if (MAP_INDEX[m_nMapIndex][i] != 0)
		{
			item->setVisible(true);
			//show map name
			auto image_title_bg=getViewRoot(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB])->getChildByName("image_title_bg");
			auto label_title=image_title_bg->getChildByName<Text*>("label_title");
			label_title->setString(SINGLE_SHOP->getLandInfo()[m_nMapIndex].name);
		}else
		{
			item->setVisible(false);
		}
	}
}

void UIGuidePort::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	buttonEvent(button,name);
}
void UIGuidePort::buttonEvent(Widget* target,std::string name,bool isRefresh /* = false*/)
{
	if (!target){	return;}
	
	if (isButton(button_Supply_yes))
	{	
		closeView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
		auto view = getViewRoot(INFORM_COCOS_RES[C_VIEW_CONFIRM_CSB]);
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		SINGLE_HERO->m_iSilver -= n_costNum;
		updateRoleCoin();
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openViewAutoClose("TIP_WHARF_ADD_SUPPLY_SUCCESS", 1);
		guide();
		return;
	}else
	{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			
			if (isButton(button_silver))
			{
				UIStore::getInstance()->openVticketStoreLayer(m_eUIType,1);
				return;
			}
		
			if (isButton(button_gold))
			{
				UIStore::getInstance()->openVticketStoreLayer(m_eUIType,0);
				return;
			}
			
			if (isButton(button_backcity))
			{
				ProtocolThread::GetInstance()->unregisterMessageCallback(this);
				this->button_callBack();
				return;
			}
			
			if (isButton(button_left))
			{
				m_pSetOffButton->setVisible(true);
				m_pZoomOutButton->setVisible(false);
				changeMapByIndex(D_LEFT);
				scaleMaptoNomal();
				return;
			}
			if (isButton(button_right))
			{
				m_pSetOffButton->setVisible(true);
				m_pZoomOutButton->setVisible(false);
				changeMapByIndex(D_RIGHT);
				scaleMaptoNomal();
				return;
			}
			if (isButton(button_on))
			{
				m_pSetOffButton->setVisible(true);
				m_pZoomOutButton->setVisible(false);
				changeMapByIndex(D_UP);
				scaleMaptoNomal();
				return;
			}
			if (isButton(button_under))
			{
				m_pSetOffButton->setVisible(true);
				m_pZoomOutButton->setVisible(false);
				changeMapByIndex(D_DOWN);
				scaleMaptoNomal();
				return;
			}
			
			if (isButton(button_Supply_no))
			{
				closeView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
				if (m_bToseaWithoutSupply)
				{
					moveMapToStart();			
					m_bToseaWithoutSupply = false;
				}
				else
				{
					//this ->button_callBack();
				}
				return;
			}
			//Fall失败按钮
			if (isButton(button_escort_yes))
			{
				closeView(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
				return;
			}
			//返回按钮
			if (isButton(button_return))
			{
				ProtocolThread::GetInstance()->unregisterMessageCallback(this);
				this->button_callBack();
				return;
			}
			//地图放大
			if (isButton(button_zoomout))
			{
				m_pSetOffButton->setVisible(true);
				m_pZoomOutButton->setVisible(false);
				if (m_bIsScale)
				{
					scaleMaptoNomal();
				}
				return;
			}
			//出海
			if (isButton(button_setoff))
			{
				_myDrawLine->drawLineByPoint(nullptr,0,0);
				auto scene = LuaSailScene::create(17);
				Director::getInstance()->replaceScene(scene);
				return;
			}
			//出海提示界面的确定按钮
			if (isButton(button_s_yes))
			{
				closeView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
				//ֱ直接出海
				if (!m_bIsAutoToSea)
				{
					ProtocolThread::GetInstance()->unregisterMessageCallback(this);
					auto scene = LuaSailScene::create(17);
					Director::getInstance()->replaceScene(scene);

				}
				//自动航行
				else
				{
					if (m_pResult->maxsupply==m_pResult->currentsupply&&m_pResult->endurancedays < m_nAutoDays)
					{
						return;
					}
					if(m_pResult->endurancedays < m_nAutoDays)
					{
						if (1.0*m_pResult->maxsupply/(1.0*m_pResult->maxcrewnum)<m_nAutoDays)
						{			
							//m_supplyContent->setString(SINGLE_SHOP->getTipInfo()[TIP_WHARF_AUTO_CANNOT_REACH_AFTER_GIVE]);
						}

					}
					else
					{
						closeView(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]);
						closeView(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB]);
						ProtocolThread::GetInstance()->unregisterMessageCallback(this);

						Scene* scene = Scene::createWithPhysics();
						UISailManage* map = UISailManage::create();
						map->setTag(MAP_TAG + 100);
						scene->addChild(map);
						m_pSailRoad->getSailRoad(SINGLE_HERO->m_iCityID,m_nFirstCityId);
						map->setAutoSailRoad(m_pSailRoad);
						Director::getInstance()->replaceScene(scene);
					}
				}
				return;
			}
			//关闭提示框
			if (isButton(button_s_no))
			{
				m_bIsAutoToSea = false;
				closeView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
				return;
			}
			//取消
			if (isButton(button_cancel))
			{
				m_bSuppleAutoSea = false;
				closeView();
				return;
			}
			if (isButton(button_ok))
			{
				closeView();
				return;
			}
			//补给或水手不足时出现的感叹号
			if (isButton(button_caveat))
			{
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushWarning(SAILOR_NOT_ENOUGH_EFFECT_SPEED);
				return;
			}
			//地图上的城市按钮
			if (isButton(button_city_flag))
			{
				int index = target->getTag() - START_INDEX;
				m_nFirstCityId = index;

				if (index < 1 || index > CITY_NUMBER)
				{
					index = 1;
				}
				if (index == SINGLE_HERO->m_iCityID)
				{
					showFallInfomation("TIP_WHARF_SAME_CITY_INFO");
					return;
				}
				//村庄的自动航行暂不实现
				if (SINGLE_SHOP->getCitiesInfo()[index].port_type == 5)
				{
					openSuccessOrFailDialog("TIP_WHARF_VILLAGE_CANNOT_ARRIVE_AUTO");
					return;
				}
				for (int i = 0; i < (m_vCities.size() - 1); i++)
				{
					for (int j = i; j < (m_vCities.size() - 1); j++){
						int btag = m_vCities.at(j)->getTag() - START_INDEX;
						if (btag == index){
				
						}
						else
						{
							Vec2 cPos = m_vCities.at(j)->getPosition() - STARTPOS;
							float dist = (cPos.x * cPos.x + cPos.y * cPos.y - (target->getPositionX()*target->getPositionX()) - (target->getPositionY()* target->getPositionY()));

							if (!m_bIsScale && fabs(dist) < 15000)
							{
								m_bIsNeedScale = true;
								break;
							}
						}
					}
				}

				if (!m_bIsScale){

					if (m_bIsNeedScale){

						m_pSetOffButton->setVisible(false);
						m_pZoomOutButton->setVisible(true);

						m_pImageMap->runAction(ScaleTo::create(0.5, 2));

						for (int i = 0; i < m_vCities.size(); i++)
						{
							m_vCities.at(i)->runAction(ScaleTo::create(0.5, 0.5));
						}

						m_bIsScale = true;						
						m_pImageMap->setAnchorPoint(isOutofMapSide(target->getPosition(), m_pImageMap->getContentSize()));
						m_pButton_backcity->setVisible(false);
						m_bIsNeedScale = false;
						return;
					}
				}			
				if (isCanSea())
				{
					m_bIsAutoToSea = true;
					openView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
					auto autoSea = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
					auto label_content=autoSea->getChildByName<Text*>("label_content");
					auto titleAutoSea=autoSea->getChildByName<Text*>("label_repair");
					titleAutoSea->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_CSB_WHARF_AUTOSEA_TITLE"]);
					std::string text = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_SUPPLE_AUTOSEA1"];
					std::string new_value = StringUtils::format("%d",m_nAutoDays);
					std::string old_value = "[num]";
					repalce_all_ditinct(text,old_value,new_value);
					new_value = SINGLE_SHOP->getCitiesInfo()[index].name;
					old_value = "[city]";
					repalce_all_ditinct(text,old_value,new_value);
					label_content->setString(text);	
				}
			}
		}
}

bool UIGuidePort::isCanSea()
{
	for (int i = 0; i < m_pResult->n_shipcrew; ++i)
	{
		if (m_pResult->shipcrew[i]->shiphp < 1)
		{
			showFallInfomation("TIP_WHARF_SHIP_DURABLE");	
			return false;
		}
	}

	if (m_pResult->maxcrewnum == 0)
	{
		showFallInfomation("TIP_WHARF_NOT_SHIP");
		return false;
	}else if (m_pResult->currentcrewnum <1) 
	{	
		showFallInfomation("TIP_WHARF_SHIP_NO_CREW");
		return false;
	}else if (SINGLE_HERO->m_iCoin < 0)
	{
		showFallInfomation("TIP_COIN_NOT");
		return false;
	}else
	{
		m_pSailRoad->getSailRoad(SINGLE_HERO->m_iCityID,m_nFirstCityId);
		float totalRoadNum = 0;
		for (int i = 0; i < m_pSailRoad->resultData->num -1; i++)
		{
			float x1 = m_pSailRoad->resultData->points[i]->x;
			float y1 = m_pSailRoad->resultData->points[i]->y;
			float x2 = m_pSailRoad->resultData->points[i+1]->x;
			float y2 = m_pSailRoad->resultData->points[i+1]->y;
			totalRoadNum += sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
		}
		m_nAutoDays = ceil(totalRoadNum/(m_pResult->secondspersailday*m_pResult->shipspeed*60.0/100));
		m_bIsAutoToSea = true;
		if (m_pResult->endurancedays < m_nAutoDays)
		{
			openView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
			auto autoSea = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
			auto label_repair = autoSea->getChildByName<Text*>("label_repair");
			label_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_CSB_WHARF_AUTOSEA_TITLE"]);
			auto label_content = autoSea->getChildByName<Text*>("label_content");
			std::string text;
			if (m_pResult->maxsupply<=m_pResult->currentsupply)
			{
				text = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_NOT_SUPPLE_AUTOSEA"];
				std::string new_value = StringUtils::format("%d",m_nAutoDays);
				std::string old_value = "[num]";
				repalce_all_ditinct(text,old_value,new_value);
				new_value = SINGLE_SHOP->getCitiesInfo()[m_nFirstCityId].name;
				old_value = "[city]";
				repalce_all_ditinct(text,old_value,new_value);
			}
			else
			{
				 text = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_NOT_SUPPLE_AUTOSEA"];
				std::string new_value = StringUtils::format("%d",m_nAutoDays);
				std::string old_value = "[num]";
				repalce_all_ditinct(text,old_value,new_value);
				new_value = SINGLE_SHOP->getCitiesInfo()[m_nFirstCityId].name;
				old_value = "[city]";
				repalce_all_ditinct(text,old_value,new_value);
			}

			label_content->setString(text);
			autoSea->setVisible(true);
			autoSea->setLocalZOrder(10);
			m_bSuppleAutoSea = true;
			return false;
		}
	}
	return true;
	
}
void UIGuidePort::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);

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
				initStaticData();
			}else
			{
				buttonEvent(nullptr,"button_return");
			}	
			break;
		}
	case PROTO_TYPE_AddSupplyResult:
		{
			auto result = (AddSupplyResult*)message;
		
			if (result->failed == 0)
			{
				m_pResult->currentsupply = result->currentsupply;
				m_pResult->endurancedays = result->endurancedays;
				
				SINGLE_HERO->m_iCoin = result->coin;			
				updateRoleCoin();
				moveMapToStart();
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_WHARF_ADD_SUPPLY_SUCCESS",1);
			}
			else if (result->failed ==100)
			{	
				moveMapToStart();
				showFallInfomation("TIP_COIN_NOT");
			}
			else
			{
				moveMapToStart();
				showFallInfomation("TIP_WHARF_ADD_SUPPLY_FAIL");
			}		
			break;
		}
	default:
		break;
	}
}


void UIGuidePort::showCaptainChat()
{
	auto viewCsb = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]);
	auto view = viewCsb->getChildByName("panel_chief_mate_dialog");

	auto image_bg = viewCsb->getChildByName<ImageView*>("image_bg");
	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].nation == 8)
	{
		image_bg->loadTexture(WHARF_BG[1]);
	}
	else
	{
		image_bg->loadTexture(WHARF_BG[0]);
	}

	auto  image_chief_mate=view->getChildByName<ImageView*>("image_chief_mate");
	auto image_dialog_bg=view->getChildByName("image_dialog_bg");
	auto label_bargirl=view->getChildByName<Text*>("label_bargirl");
	auto label_content_supply_all=view->getChildByName<Text*>("label_content_supply_all");
	auto image_anchor=view->getChildByName("image_anchor");

	image_dialog_bg->setOpacity(0);
	label_bargirl->setOpacity(0);
	label_content_supply_all->setOpacity(0);
	image_anchor->setOpacity(0);
	image_chief_mate->ignoreContentAdaptWithSize(false);
	image_chief_mate->loadTexture(getCheifIconPath());
	if (SINGLE_HERO->m_iGender == 1)
	{
		label_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"]);
	}else
	{
		label_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"]);
	}
	image_chief_mate->runAction(MoveTo::create(1,STARTPOS));
	image_dialog_bg->runAction(Sequence::createWithTwoActions(DelayTime::create(1),FadeIn::create(0.5)));
	label_bargirl->runAction(Sequence::createWithTwoActions(DelayTime::create(1.5), FadeIn::create(0.5)));
	label_content_supply_all->runAction(Sequence::createWithTwoActions(DelayTime::create(1.5), FadeIn::create(0.5)));
	this->scheduleOnce(schedule_selector(UIGuidePort::anchorAction),2);
}

void UIGuidePort::anchorAction(float time)
{
	auto guard_dialog = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_chief_mate_dialog");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(255);
	i_anchor->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5,Vec2(0,10))),EaseBackOut::create(MoveBy::create(0.5, Vec2(0,-10))))));
	m_bAideClick = false;
}

void UIGuidePort::closeCaptainChat(Ref*pSender,Widget::TouchEventType TouchType)
{
	if (TouchType!=Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* chatPanel = static_cast<Widget*>(pSender);
	
	if (strcmp(chatPanel->getName().data(),"panel_chief_mate_dialog")==0)
	{	
		if (m_bAideClick == false)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			if (!m_pResult) return;
			auto image_chief_mate = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_chief_mate_dialog")->getChildByName("image_chief_mate");
			if (image_chief_mate->getPosition() != STARTPOS)
			{
				return;
			}
			else
			{
				captainChatOut();
			}
		
			if ((m_pResult->currentpackagesize <= m_pResult->maxpackagesize) && (m_pResult->currentgoodsweight <= m_pResult->maxgoodsweight))
			{
			
				if (m_pResult->n_shipcrew > 0)
				{
					
					if (m_pResult->currentsupply >= m_pResult->maxsupply)
					{
						moveMapToStart();
					}
					else
					{
						showSupplyPanelData();
						m_bToseaWithoutSupply = true;
					}
				}
				else
				{
					this->button_callBack();
				}
				guide();
				return;
			}
			else
			{
				this->button_callBack();
				return;
			}
		}
	}
}
void UIGuidePort::updateRoleCoin()
{
	auto viewBar=getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_actionbar");
	auto buttonSilver=viewBar->getChildByName("button_silver");
	auto label_silver_num=buttonSilver->getChildByName<Text*>("label_silver_num");
	label_silver_num->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iSilver)));
	auto buttonGold=viewBar->getChildByName("button_gold");
	auto label_gold_num=buttonGold->getChildByName<Text*>("label_gold_num");
	label_gold_num->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iVp)));
	setTextFontSize(label_gold_num);
	setTextFontSize(label_silver_num);
}
void UIGuidePort::chatContentBySupply()
{	
	if (!m_pResult)
	{
		return;
	}
	auto view=	getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_chief_mate_dialog");
	auto label_content_supply_all=view->getChildByName<Text*>("label_content_supply_all");
	std::string content;

	if ((m_pResult->currentpackagesize<=m_pResult->maxpackagesize)&&(m_pResult->currentgoodsweight<=m_pResult->maxgoodsweight))
	{
		
		if (m_pResult->n_shipcrew > 0)
		{
			
			m_pResult->currentsupply = 0;
			if (m_pResult->currentsupply >= m_pResult->maxsupply)
			{
				content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_CAPTAINCHAT_SUPPLY_FULL"];
			}
			else
			{
				content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_CAPTAINCHAT_SUPPLY_NOT_FULL"];
			}
		}	
		else
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_NOT_SHIP"];
		}
	}
	else
	{
		if (m_pResult->currentpackagesize>m_pResult->maxpackagesize)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_BAG_OVERWEIGHT"];
		}
		else
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_CABIN_CAPACITY_FULL"];
		}
		
	}
	
	std::string new_vaule = SINGLE_HERO->m_sName;
	std::string old_vaule = "[heroname]";
	repalce_all_ditinct(content,old_vaule,new_vaule);
	label_content_supply_all->setString(content);
}
void UIGuidePort::showDockMapData(const GetVisitedCitiesResult*result)
{
	if (!result){ return; }
	auto view=getViewRoot(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB]);
	//auto image_sailor=view->getChildByName("image_sailor");
	//auto ship_sailor_num=image_sailor->getChildByName<Text*>("label_ship_sailor_num_1");
	//auto ship_sailor_num_2=image_sailor->getChildByName<Text*>("label_ship_sailor_num_2");
	
	//ship_sailor_num->setString(StringUtils::format("%d",35));
	//ship_sailor_num_2->setString(StringUtils::format("/%d",35));
	//auto progressbar_sailor_Bg=image_sailor->getChildByName("image_progressbar_sailor");
	//auto button_caveat=image_sailor->getChildByName<Button*>("button_caveat");
	//button_caveat->addTouchEventListener(CC_CALLBACK_2(WharfGuide::menuCall_func,this));
	//auto progressbar_sailor=progressbar_sailor_Bg->getChildByName<LoadingBar*>("progressbar_durable");
	//progressbar_sailor->setPercent(100.0*(result->currentcrewnum/(result->maxcrewnum*1.0)));

	//auto image_supply=view->getChildByName("image_supply");
	//auto label_ship_supply_num=image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	
	//label_ship_supply_num->setString(StringUtils::format("%d/%d",350,350));
	
	//auto image_progressbar_supply=image_supply->getChildByName("image_progressbar_supply");
	//auto progressbar_supply=image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
//	progressbar_supply->setPercent(100.0*(350/(350*1.0)));
//	if (result->sailorsisenough)
//	{
	//	ship_sailor_num->setColor(Color3B(0,0,0));
	//	button_caveat->setVisible(false);
	//}
	//else
	//{
	//	ship_sailor_num->setColor(Color3B(255,0,0));
	//	button_caveat->setVisible(true);
		//这里的设计是水手少会影响航速，从而给出提示，而不是直接告诉你水手不够
	//	button_caveat->loadTextureNormal("cocosstudio/login_ui/sailing_720/supply_over.png");
	//}
	auto panel_max_day=view->getChildByName("panel_max_day");
	auto label_max_day_num=panel_max_day->getChildByName<Text*>("label_max_day_num");
	label_max_day_num->setString(StringUtils::format("%d",m_pResult->endurancedays));
//chengyuan++
	//label_ship_supply_num->setTextHorizontalAlignment(TextHAlignment::CENTER);
//
}
void UIGuidePort::showSupplyPanelData()
{
	if (!m_pResult)
	{
		return;
	}
	openView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto supplyView = getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	
	auto slider_dropitem_num=supplyView->getChildByName<Slider*>("slider_dropitem_num");
	slider_dropitem_num->addEventListenerSlider(this,sliderpercentchangedselector(UIGuidePort::supplySliderChange));

	auto image_supply=supplyView->getChildByName("image_supply");
	auto label_ship_supply_num_1=image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	auto label_ship_supply_num_2 = image_supply->getChildByName<Text*>("label_ship_supply_num_2");
	auto image_progressbar_supply=image_supply->getChildByName("image_progressbar_supply");
	auto progressbar_supply=image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
	label_ship_supply_num_1->setString(StringUtils::format("%d",m_pResult->maxsupply));
	std::string  str = label_ship_supply_num_1->getString();
	int n = atoi(str.c_str());
	if (n == m_pResult->maxsupply)
	{
		label_ship_supply_num_1->setTextColor(Color4B(46, 125, 50 ,255));
	}
	else
	{
		label_ship_supply_num_1->setTextColor(Color4B(40, 25, 13,255));
	}
	label_ship_supply_num_2->setString(StringUtils::format("/%d", m_pResult->maxsupply));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width);
	progressbar_supply->setPercent(100.0*(m_pResult->currentsupply*1.0/(m_pResult->maxsupply*1.0)));

	//progressbar_supply_add
	auto progressbar_supply_add=image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply_add");
	progressbar_supply_add->setPercent(100);
	m_nAddSupplyAmount=m_pResult->maxsupply-m_pResult->currentsupply;

	auto panel_salary=supplyView->getChildByName("panel_salary");
	auto label_cost_num=panel_salary->getChildByName<Text*>("label_cost_num");
	n_costNum = 3500;
	label_cost_num->setString(numSegment(StringUtils::format("%d",n_costNum)));
	label_cost_num->ignoreContentAdaptWithSize(true);
	label_cost_num->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	auto i_silver = supplyView->getChildByName("image_silver_1");
	i_silver->setPositionX(label_cost_num->getPositionX() - label_cost_num->getContentSize().width - i_silver->getContentSize().width / 2);
//
}
void UIGuidePort::showFallInfomation(std::string name)
{
	
	openView(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
	auto viewFall = getViewRoot(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
	viewFall->setLocalZOrder(100);
	auto label_content=viewFall->getChildByName<Text*>("label_content");
	label_content->setString(SINGLE_SHOP->getTipsInfo()[name]);
	//标题
	auto label_result = viewFall->getChildByName<Text*>("label_result");
	label_result->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_OPERATE_FAIL"]); 
}
void UIGuidePort::moveMapToStart()
{
	if (m_bMoveActionFlag)
	{
		m_bMoveActionFlag=false;

		openView(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB]);
		auto panel_dock_map = getViewRoot(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB]);
		auto panel_map_content=panel_dock_map->getChildByName("panel_map_content");
		Layout *panel_map = panel_map_content->getChildByName<Layout*>("panel_map");
		m_pImageMap = panel_map->getChildByName<ImageView*>("image_map_1");
		m_pButton_backcity = panel_dock_map->getChildByName<Button*>("button_backcity_1");

		panel_dock_map->runAction(MoveTo::create(1,STARTPOS));
		
		auto button_zoomout=panel_dock_map->getChildByName<Button*>("button_zoomout");
		button_zoomout->setVisible(false);
		auto button_setoff=panel_dock_map->getChildByName<Button*>("button_setoff");
		button_setoff->setVisible(true);
		m_pSetOffButton=button_setoff;
		m_pZoomOutButton=button_zoomout;

		showDockMapData(m_pResult);	
		addMapDirectButton();
		updateDirectButton();
		
//		notifyCompleted(NONE);
	}
	else
	{
		return;
	}
}
void UIGuidePort::captainChatOut()
{
	m_bAideClick = true;
	auto view=	getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_chief_mate_dialog");
	auto image_chief_mate=view->getChildByName("image_chief_mate");
	auto image_dialog_bg=view->getChildByName("image_dialog_bg");
	auto label_bargirl=view->getChildByName("label_bargirl");
	auto label_content_supply_all=view->getChildByName<Text*>("label_content_supply_all");
	auto image_anchor=view->getChildByName("image_anchor");

	image_chief_mate->runAction(Sequence::createWithTwoActions(DelayTime::create(0.3),MoveBy::create(1, ENDPOS3)));
	image_dialog_bg->runAction(FadeOut::create(0.3));
	label_bargirl->runAction(FadeOut::create(0.01));
	label_content_supply_all->runAction(FadeOut::create(0.01));
	image_anchor->setOpacity(0);
	image_anchor->runAction(FadeOut::create(0.01));
}

void UIGuidePort::scaleMaptoNomal(){

	m_pImageMap->setAnchorPoint(Vec2(0.5, 0.5));
	m_pImageMap->runAction(ScaleTo::create(0.5, 1));
	for (int i = 0; i < m_vCities.size(); i++)
	{
		m_vCities.at(i)->runAction(ScaleTo::create(0.5, 1));
	}
	m_pButton_backcity->setVisible(true);
	m_bIsScale = false;

}
void UIGuidePort::supplySliderChange(Ref* obj,cocos2d::ui::SliderEventType type)
{
	if (type!=SliderEventType::SLIDER_PERCENTCHANGED)
	{
		return;
	}
	if (!m_pResult) return;
	auto supplyView=getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto image_supply=supplyView->getChildByName("image_supply");
	auto label_ship_supply_num_1=image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	auto label_ship_supply_num_2 = image_supply->getChildByName<Text*>("label_ship_supply_num_2");
	auto image_progressbar_supply=image_supply->getChildByName("image_progressbar_supply");
	auto progressbar_supply=image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
	
	progressbar_supply->setPercent(100.0*(m_pResult->currentsupply*1.0/(m_pResult->maxsupply*1.0)));

	auto slider_dropitem_num=supplyView->getChildByName<Slider*>("slider_dropitem_num");

	auto progressbar_supply_add=image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply_add");
	auto perChange=(100.0-progressbar_supply->getPercent())*slider_dropitem_num->getPercent()/100.0+progressbar_supply->getPercent();
	progressbar_supply_add->setPercent(perChange);
	
	auto panel_salary=supplyView->getChildByName("panel_salary");
	auto label_cost_num=panel_salary->getChildByName<Text*>("label_cost_num");

	int32_t supplyChange=(m_pResult->maxsupply-m_pResult->currentsupply)*slider_dropitem_num->getPercent()/100.0;

	if (supplyChange==0)
	{
		supplyChange=1;
	}
	m_nAddSupplyAmount=supplyChange;
	label_cost_num->setString(numSegment(StringUtils::format("%d",supplyChange*m_pResult->costpersupply)));
	label_ship_supply_num_1->setString(StringUtils::format("%d",m_pResult->currentsupply+perChange));
	label_ship_supply_num_2->setString(StringUtils::format("/%d", m_pResult->maxsupply));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width);
	std::string  str = label_ship_supply_num_1->getString();
	int n = atoi(str.c_str());
	if (n == m_pResult->maxsupply)
	{
		label_ship_supply_num_1->setTextColor(Color4B(46, 125, 50,255));
	}
	else
	{
		label_ship_supply_num_1->setTextColor(Color4B(40, 25, 13,255));
	}
}

Vec2 UIGuidePort::isOutofMapSide(Vec2 targetPos, Size mapSize)
{
	Vec2 anchorPoint = Vec2(targetPos.x / mapSize.width, targetPos.y / mapSize.height);
	float x = anchorPoint.x;
	float y = anchorPoint.y;
	Vec2 compare = Vec2(0.5, 0.5) / 2;

	if (x < compare.x)
	{
		x = compare.x;
	}else if (x > 3 * compare.x)
	{
		x = 3 * compare.x;
	}
 
	if (y < compare.y )
	{
		y = compare.y;
	}else if (y > 3 * compare.y)
	{
		y = 3 * compare.y;
	}

	return Vec2(x, y);
}
void UIGuidePort::guide()
{
	m_guideStage++;
	auto view = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]);
	setButtonsDisable(view);
	switch (m_guideStage)
	{

	case OPEN_BUY_SPULLIES_VIEW:
	{
						auto view = getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
						setButtonsDisable(view);
						auto btn_yes = Helper::seekWidgetByName(view, "button_Supply_yes");
						btn_yes->setTouchEnabled(true);
						focusOnButton(btn_yes);
	}
		break;

	case NPC_FIDE_IN:
	{
						sprite_hand->setVisible(false);
						m_dialogLayer = UINoviceStoryLine::GetInstance();
						this->addChild(m_dialogLayer, 10000);
						m_dialogLayer->setVisible(true);
						m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::DOCK_ONE_DIALOG, 0);
	}
		break;
	case LAUNCH_OUT:
	{
					   sprite_hand->setVisible(false);
					   moveMapToStart();
					   auto view = getViewRoot(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB]);
					   setButtonsDisable(view);
					   auto btn_setoff = Helper::seekWidgetByName(view, "button_setoff");
					   this->runAction(Sequence::createWithTwoActions(DelayTime::create(1.0), CallFunc::create([=]{
						   focusOnButton(btn_setoff);
						   btn_setoff->setTouchEnabled(true);
					   })));
	}
		break;
	default:
		break;
	}

}
void UIGuidePort::focusOnButton(Widget * p_sender)
{
	sprite_hand->setVisible(true);
	auto c_psender = dynamic_cast<Widget*>(p_sender);
	Size win = _director->getWinSize();
	Vec2 pos = c_psender->getWorldPosition();
	Size cut_hand = sprite_hand->getContentSize() / 2;
	Size cut_psendet = p_sender->getContentSize() / 2;
	std::string name = dynamic_cast<Widget*>(p_sender)->getName();
	//小手位置
	if (pos.x < win.width / 2 && pos.y > win.height / 2)
	{
		sprite_hand->setRotation(-180);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
			c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
	}
	else if (pos.x > win.width / 2 && pos.y > win.height / 2)
	{
		sprite_hand->setRotation(-110);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
			c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));

	}
	else if (pos.x < win.width / 2 && pos.y < win.height / 2)
	{
		if (name.compare("button_result_yes") == 0)
		{
			sprite_hand->setRotation(-180);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
		}
		else
		{
			if (name.compare("") == 0)
			{

			}
			else
			{
				sprite_hand->setRotation(70);
				sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
					c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6));
			}

		}

	}
	else if (pos.x > win.width / 2 && pos.y < win.height / 2)
	{
		if (name.compare("button_Supply_yes") == 0)
		{
			sprite_hand->setRotation(180);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
		}
		else
		{
			sprite_hand->setRotation(0);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6) + Vec2(10, -10));
		}

	}
	sprite_hand->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
}
void UIGuidePort::setButtonsDisable(Node * node)
{
	auto children = node->getChildren();
	Vector<Widget*>btns;
	for (auto btn : children)
	{
		std::string name = btn->getName();
		if (btn->getChildren().size() >= 0)
		{
			if (btn->getName().find("button_") == 0)
			{
				auto c_btn = dynamic_cast<Widget*>(btn);
				setButtonsDisable(btn);
			}
			else
			{
				setButtonsDisable(btn);
			}
		}
		if (btn->getName().find("button_") == 0)
		{
			auto c_btn = dynamic_cast<Widget*>(btn);
			c_btn->setTouchEnabled(false);
		}
	}
}
