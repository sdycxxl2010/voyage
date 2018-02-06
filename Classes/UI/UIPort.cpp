#include "UIPort.h"
#include "UIChatHint.h"
#include "MyDrawLine.h"
#include "SailRoad.h"
#include "MainTaskManager.h"
#include "CompanionTaskManager.h"

#include "UIStore.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UISailManage.h"
#include "TVSceneLoader.h"

#define MAP_NUM 19
static Text* _flagText = nullptr;
static MyDrawLine* _myDrawLine = nullptr;

UIPort::UIPort():
	m_nMapIndex(1),
	m_nFirstCityId(-1),
	m_nAutomaticCityId(-1),
	m_nAddSupplyAmount(-1),
	m_pResult(nullptr),
	m_pPanleButton(nullptr)
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
	m_bAideClick = true;
	m_nAutoDays = 0;
	m_pCities = nullptr;
	m_pSetOffButton=nullptr;
	m_pZoomOutButton=nullptr;
	m_pButton_backcity=nullptr;
	m_vDirectWidgets.clear();
	m_vCities.clear();
	m_eUIType = UI_WHARF;
	m_nFastHireCrewNum = 0;
	m_pFastHireCrewResult = nullptr;
	m_dialogTag = 0;
	m_loadingLayer = nullptr;
	m_bIsToTarven = false;
}

UIPort::~UIPort()
{
	m_vDirectWidgets.clear();
	m_vCities.clear();
	_flagText->release();
	_myDrawLine->release();
	m_pPanleButton = nullptr;
	//SINGLE_SHOP->releaseLandData();
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}
UIPort* UIPort::create()
{
	UIPort* pup = new UIPort;
	CC_RETURN_IF(pup);
	if (pup->init())
	{
		pup->autorelease();
		return pup;
	}
	CC_SAFE_RELEASE(pup);
	return nullptr;
}

bool UIPort::init()
{
	bool pRet = false;
	
	do 
	{
		CC_BREAK_IF(!UIBasicLayer::init());
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIPort::onServerEvent,this),this);
		ProtocolThread::GetInstance()->getVisitedCities(UILoadingIndicator::create(this,m_eUIType));
		m_loadingLayer = UILoadingIndicator::create(this, 0);
		m_loadingLayer->showSelf();
		pRet = true;
	} while (0);
	return pRet;
}


bool UIPort::initStaticData()
{
	bool pRet = false;
	do 
	{
		m_pSailRoad->initSailRoadData();
		openView(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]);
		//显示对话内容
		showCaptainChat();
		chatContentBySupply();
		updateRoleCoin();
		auto  view=	getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName<Widget*>("panel_chief_mate_dialog");
		view->addTouchEventListener(CC_CALLBACK_2(UIPort::closeCaptainChat,this));
		auto t_dock =getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_actionbar")->getChildByName<Text*>("label_dock");
		//聊天
		auto ch = UIChatHint::createHint();
		this->addChild(ch, 10);
		if (m_loadingLayer)
		{
		m_loadingLayer->removeSelf();
		m_loadingLayer = nullptr;
		}
		pRet = true;
	} while (0);
	return pRet;
}

/*
* 当剧情或者对话动作结束时调用该函数
* actionIndex, 当前动作的index
*/
void UIPort::notifyCompleted(int actionIndex)
{
	//扫尾工作
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

/*
* 执行下一个对话或者剧情操作
* actionIndex, 当前动作的index
*/
void UIPort::doNextJob(int actionIndex)
{
	actionIndex++;
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	auto viewTarven = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	switch (actionIndex)
	{
	case MAIN_STORY:
		//剧情（主线）
		//主线任务完成对话
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
		//小伙伴剧情
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
		//小剧情
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

void UIPort::addMapDirectButton()
{
	const char *directName[] = {"button_on","button_left","button_under","button_right"};
	auto dockMap = getViewRoot(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB]);
	auto panel_map_content=dockMap->getChildByName<Widget*>("panel_map_content");
	for (int i = 0; i < 4; i++)
	{
		auto btn_direct =panel_map_content->getChildByName<Button*>(directName[i]);
		m_vDirectWidgets.pushBack(btn_direct);
		
	}
	//获取资源
	//SINGLE_SHOP->loadLandData();
	auto landInfo = SINGLE_SHOP->getLandInfo();
 	for (auto i = 0;i < MAP_NUM;i++)
 	{
		Rect tempRect = Rect(landInfo[i + 1].left_down_x, landInfo[i + 1].left_down_y, landInfo[i + 1].right_up_x - landInfo[i + 1].left_down_x, 3727);
		Vec2 cityPos = Vec2(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].x,SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].y);
 		if(tempRect.containsPoint(cityPos))
 		{
 			m_nMapIndex = i + 1;
			m_pImageMap->loadTexture( StringUtils::format("ship/seaFiled/map_%d.jpg",m_nMapIndex));
			Rect tempRect_1 = Rect(landInfo[i + 1].left_down_x, landInfo[i + 1].left_down_y, 7518, 3727);
 			_flagText->setString(String::createWithFormat("CURR INDEX:%d",m_nMapIndex)->_string);
			addCitiesForMap(m_pImageMap, tempRect_1, m_nMapIndex);
 			updateDirectButton();
 		}
 	}
	 
}

Vec2 UIPort::getMapOffset(const ImageView* view){

	const Size &contSize = view -> getContentSize();
	float offsetX = contSize.width / 894 * 34;
	float offsetY = contSize.width / 462 * 34;
	return Vec2(offsetX, offsetY);

}

void UIPort::addCitiesForMap(ImageView* view,Rect& fieldSize,int index)
{
	auto landInfo = SINGLE_SHOP->getLandInfo();
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
			cityIcon->addTouchEventListener(CC_CALLBACK_2(UIPort::menuCall_func,this));

			if (cityDefine->cityid == SINGLE_HERO->m_iCityID)
			{
				ImageView* ship_icon = ImageView::create(SHIP_ICON_PATH);
				ship_icon->setPosition(cityIcon->getContentSize()/2);
				ship_icon->setLocalZOrder(11);
				ship_icon->setPosition(cityIcon->getContentSize() / 2);
				cityIcon->addChild(ship_icon);				
			}

			Vec2 diffPos = cityPos - fieldSize.origin;
			if (index == 10)//加勒比海东
			{
				diffPos.setPoint(diffPos.x  * zoomFactor_x + offset.x / 2, (diffPos.y) * zoomFactor_y + offset.y / 2);
			}
			else
			{
				diffPos.setPoint((diffPos.x + 7518 - (landInfo[index].right_up_x - landInfo[index].left_down_x)) * zoomFactor_x + offset.x / 2, (diffPos.y) * zoomFactor_y + offset.y / 2);
			}
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
void UIPort::changeMapByIndex(MAP_DIRCT direct)
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
	addCitiesForMap(m_pImageMap, tempRect, mapTempIndex);
	updateDirectButton();

}
void UIPort::updateDirectButton()
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

void UIPort::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	buttonEvent(button,name);
}
void UIPort::buttonEvent(Widget* target,std::string name,bool isRefresh /* = false*/)
{
	if (!target){	return;}
	//补给界面的确定按钮
	if (isButton(button_Supply_yes))
	{	
		closeView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
		switch (m_dialogTag)
		{
		case STEP_FAST_RECRUIT_SAILORS:
		       if (m_nFastHireCrewNum > 0)
			   {
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
				   ProtocolThread::GetInstance()->getCrew(m_nFastHireCrewNum, UILoadingIndicator::create(this), 1);
			   }
		break;
		default:
				//小于单位补给的花费的银币
				if (SINGLE_HERO->m_iCoin < m_pResult->costpersupply)
				{
					SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
					moveMapToStart();
					showFallInfomation("TIP_COIN_NOT");
				}
				else
				{
					SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
					ProtocolThread::GetInstance()->addSupply(m_nAddSupplyAmount, UILoadingIndicator::create(this));
				}
				break;
		}
		
		return;
	}else
	{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			//银币按钮
			if (isButton(button_silver))
			{
				UIStore::getInstance()->openVticketStoreLayer(m_eUIType,1);
				return;
			}
			//V票
			if (isButton(button_gold))
			{
				UIStore::getInstance()->openVticketStoreLayer(m_eUIType,0);
				return;
			}
			//返回主城按钮
			if (isButton(button_backcity))
			{
				ProtocolThread::GetInstance()->unregisterMessageCallback(this);
				this->button_callBack();
				return;
			}
			//地图的的方向按钮
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
			//补给界面取消
			if (isButton(button_Supply_no))
			{
				closeView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
				if (m_dialogTag == STEP_FAST_RECRUIT_SAILORS)
				{
					moveMapToStart();
				}
				else
				{
					if (m_pResult->sailorsisenough)
					{
						moveMapToStart();
					}
					else
					{
						showCaptainChat();
						chatfastHireContent();
					}
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
			//地图放大镜
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
				for (int i = 0; i < m_pResult->n_shipcrew; ++i)
				{
					if (m_pResult->shipcrew[i]->shiphp < 1)
					{
						showFallInfomation("TIP_WHARF_SHIP_DURABLE");
						return;
					}
				}
	
				if (m_pResult->maxcrewnum == 0)
				{
						showFallInfomation("TIP_WHARF_NOT_SHIP");
				}else if (m_pResult->currentcrewnum <1) 
				{
						showFallInfomation("TIP_WHARF_SHIP_NO_CREW");

				}
 				else if (m_pResult->currentsupply<=0)
				{
					showFallInfomation("TIP_WHARF_WITHOUT_SUPPLY_CANNOTSEA");
				}
 				else if (m_pResult->endurancedays<1&&m_pResult->currentsupply>0)
 				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_CANCEL_DOCK_FORM_FLEET_TITLE", "TIP_WHARF_SUPPLY_A_DAY_DANGEROUS");
 				}
				else if (SINGLE_HERO->m_iCoin < 0)
				{
					showFallInfomation("TIP_COIN_NOT");
				}else
				{
					//不是自动航行
					m_bIsAutoToSea = false;
					openView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
					auto viewCommon = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
					auto label_content=viewCommon->getChildByName<Text*>("label_content");
					auto label_title = viewCommon->getChildByName<Text*>("label_repair");
					label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_WHARF_GO_SEA"]);
					label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_WHARF_LEAVE_CITY"]);	
				}
				return;
			}
			//出海提示界面的确定按钮
			if (isButton(button_s_yes))
			{
				closeView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
				//直接出海
				if (!m_bIsAutoToSea)
				{
					if (m_pResult->maxcrewnum == 0 ||SINGLE_HERO->m_iCoin < 0)
					{
						return;
					}
					if (m_pResult->currentcrewnum < 1)
					{
						return;
					}else
					{

					}
					for (int i = 0; i < m_pResult->n_shipcrew; ++i)
					{
						if (m_pResult->shipcrew[i]->shiphp < 1)
						{
							return;
						}
					}
					ProtocolThread::GetInstance()->unregisterMessageCallback(this);
					//auto scene = LuaSailScene::create(17);
					//Director::getInstance()->replaceScene(scene);
					SINGLE_HERO->m_londonWeather = 0;
					CHANGETO(MAP_TAG);
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
			if (isButton(button_autopilot))
			{
				openAutopilotView();
				return;
			}
			//关闭提示框
			if (isButton(button_s_no))
			{
				m_bIsAutoToSea = false;
				closeView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
				m_bSuppleAutoSea = false;
				return;
			}
			//取消
			if (isButton(button_cancel))
			{
				m_bSuppleAutoSea = false;
				if (m_pPanleButton)
				{
					m_pPanleButton = nullptr;
				}
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
				UICommon::getInstance()->flushWarning(n_sailorNotEnoughTag);
				return;
			}
			//银币不足
			if (isButton(button_error_yes))
			{
				if (m_bIsToTarven)
				{
					CHANGETO(SCENE_TAG::PUPL_TAG);
				}
				else
				{
					button_callBack();
				}
				
				return;
			}
			//地图上的城市按钮
			if (isButton(button_city_flag) || isButton(button_yes_1))
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
				if (SINGLE_SHOP->getCitiesInfo()[index].port_type == 5 || SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type==5)
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
				return;
			}
		}
		if (isButton(panel_city_1))
		{
			if (m_pPanleButton)
			{
				m_pPanleButton->getChildByName("image_press")->setVisible(false);
			}
			m_pPanleButton = target;
			m_pPanleButton->getChildByName("image_press")->setVisible(true);
			int index = target->getTag() - START_INDEX;
			m_nAutomaticCityId = index;
			m_pSailRoad->getSailRoad(SINGLE_HERO->m_iCityID, m_nAutomaticCityId);

			auto autopilot = getViewRoot(DOCK_COCOS_RES[VIEW_AUTOPILOT_CSB]);
			auto button_yes_1 = autopilot->getChildByName<Button*>("button_yes_1");
			
			button_yes_1->setTag(START_INDEX + m_nAutomaticCityId);
			if (!m_pSailRoad || !m_pSailRoad->resultData)
			{
				showFallInfomation("TIP_WHARF_AUTO_SAILING_FAILED");
			}
		}

}

bool UIPort::isCanSea()
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
		if (!m_pSailRoad||!m_pSailRoad->resultData)
		{
			showFallInfomation("TIP_WHARF_AUTO_SAILING_FAILED");
			return false;
		}
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
				m_bSuppleAutoSea = false;
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
				m_bSuppleAutoSea = true;
			}

			label_content->setString(text);
			autoSea->setVisible(true);
			autoSea->setLocalZOrder(10);
			return false;
		}
	}
	return true;
	
}

bool UIPort::isCanAutomatic()
{
	m_pSailRoad->getSailRoad(SINGLE_HERO->m_iCityID, m_nAutomaticCityId);
	float totalRoadNum = 0;
	if (!m_pSailRoad || !m_pSailRoad->resultData)
	{
	//	showFallInfomation("TIP_WHARF_AUTO_SAILING_FAILED");
		return false;
	}
	for (int i = 0; i < m_pSailRoad->resultData->num - 1; i++)
	{
		float x1 = m_pSailRoad->resultData->points[i]->x;
		float y1 = m_pSailRoad->resultData->points[i]->y;
		float x2 = m_pSailRoad->resultData->points[i + 1]->x;
		float y2 = m_pSailRoad->resultData->points[i + 1]->y;
		totalRoadNum += sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
	}
 	m_nAutomaticDays = ceil(totalRoadNum / (m_pResult->secondspersailday*m_pResult->shipspeed*60.0 / 100));
	m_bIsAutoToSea = true;
	if (m_pResult->endurancedays < m_nAutomaticDays)
	{
		//openView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
		//auto autoSea = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
		//auto label_repair = autoSea->getChildByName<Text*>("label_repair");
		//label_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_CSB_WHARF_AUTOSEA_TITLE"]);
	//	auto label_content = autoSea->getChildByName<Text*>("label_content");
		std::string text;
		if (m_pResult->maxsupply <= m_pResult->currentsupply)
		{
			text = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_NOT_SUPPLE_AUTOSEA"];
			std::string new_value = StringUtils::format("%d", m_nAutomaticDays);
			std::string old_value = "[num]";
			repalce_all_ditinct(text, old_value, new_value);
			new_value = SINGLE_SHOP->getCitiesInfo()[m_nFirstCityId].name;
			old_value = "[city]";
			repalce_all_ditinct(text, old_value, new_value);
		}
		else
		{
			text = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_NOT_SUPPLE_AUTOSEA"];
			std::string new_value = StringUtils::format("%d", m_nAutomaticDays);
			std::string old_value = "[num]";
			repalce_all_ditinct(text, old_value, new_value);
			new_value = SINGLE_SHOP->getCitiesInfo()[m_nFirstCityId].name;
			old_value = "[city]";
			repalce_all_ditinct(text, old_value, new_value);
		}
	}
	return true;
}
void UIPort::onServerEvent(struct ProtobufCMessage* message,int msgType)
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
					get_visited_cities_result__free_unpacked(m_pResult, 0);
					m_pResult = result;
					updateRoleCoin();
					moveMapToStart();
				}
				else
				{
					m_pResult = result;
					initStaticData();
				}
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
				
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_WHARF_ADD_SUPPLY_SUCCESS",1);
				if (m_pResult->sailorsisenough)
				{			
					moveMapToStart();
				}
				else
				{
					showCaptainChat();
					chatfastHireContent();
				}	   
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
	case PROTO_TYPE_CalFastHireCrewResult:
	{
			CalFastHireCrewResult *result = (CalFastHireCrewResult *)message;
			if (result->failed == 0)
			{
				m_pFastHireCrewResult = result;
				UIInform::getInstance()->openInformView(this);
				if (SINGLE_HERO->m_iCoin < 1100 && SINGLE_HERO->m_iCoin >= 600)
				{
					m_bIsToTarven = true;
					UIInform::getInstance()->openConfirmYes("TIP_MAINCITY_SAILOR_HIRE_COIN_VERY_LITTLE");
					break;
				}
				else if (SINGLE_HERO->m_iCoin < 600)
				{
					m_bIsToTarven = false;
					UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
					break;
				}
				showFastHirePanelData();
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CREW_FAIL");
			}
			break;
	}
	case PROTO_TYPE_GetCrewResult:
	{
									 GetCrewResult *result = (GetCrewResult *)message;
									 if (result->failed == 0)
									 {
										 SINGLE_HERO->m_iCoin = result->coin;
										 SINGLE_HERO->m_iGold = result->gold;
										 /*
										 //当水手足够时
										 if (result->needcrewnum <= 0)
										 {
											 m_pResult->sailorsisenough = 1;
										 }
										 m_pResult->currentcrewnum += result->crewnum;
										 updateRoleCoin();
										 moveMapToStart();
										 */
										 ProtocolThread::GetInstance()->getVisitedCities();
										 UIInform::getInstance()->openInformView(this);
										 UIInform::getInstance()->openViewAutoClose("TIP_PUP_SAILOR_RECRUIT_SUCCESS",1);

									 }
									 else
									 {
										 UIInform::getInstance()->openInformView(this);
										 UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CREW_FAIL");
									 }
									 break;
	}
	default:
		break;
	}
}


void UIPort::showCaptainChat()
{
	auto viewCsb = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB]);
	auto view = viewCsb->getChildByName("panel_chief_mate_dialog");


	//背景图片
	auto image_bg = viewCsb->getChildByName<ImageView*>("image_bg");
	image_bg->loadTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_WHARF_AREA));

	auto  image_chief_mate=view->getChildByName<ImageView*>("image_chief_mate");
	auto image_dialog_bg=view->getChildByName("image_dialog_bg");
	auto label_bargirl=view->getChildByName<Text*>("label_bargirl");
	auto label_content_supply_all=view->getChildByName<Text*>("label_content_supply_all");
	auto image_anchor=view->getChildByName("image_anchor");
	image_chief_mate->stopAllActions();
	image_dialog_bg->setOpacity(0);
	label_bargirl->setOpacity(0);
	label_content_supply_all->setOpacity(0);
	image_anchor->setOpacity(0);
	image_chief_mate->ignoreContentAdaptWithSize(false);
	image_chief_mate->loadTexture(getCheifIconPath());
	image_chief_mate->setVisible(true);
	if (SINGLE_HERO->m_iGender == 1)
	{
		label_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"]);
	}else
	{
		label_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"]);
	}
	image_chief_mate->runAction(MoveTo::create(1, STARTPOS));
	image_dialog_bg->runAction(Sequence::createWithTwoActions(DelayTime::create(1),FadeIn::create(0.5)));
	label_bargirl->runAction(Sequence::createWithTwoActions(DelayTime::create(1.5), FadeIn::create(0.5)));
	label_content_supply_all->runAction(Sequence::createWithTwoActions(DelayTime::create(1.5), FadeIn::create(0.5)));
	this->scheduleOnce(schedule_selector(UIPort::anchorAction),2);
}

void UIPort::anchorAction(float time)
{
	auto guard_dialog = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_chief_mate_dialog");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(255);
	i_anchor->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5,Vec2(0,10))),EaseBackOut::create(MoveBy::create(0.5, Vec2(0,-10))))));
	m_bAideClick = false;
}

void UIPort::closeCaptainChat(Ref*pSender,Widget::TouchEventType TouchType)
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


			//判断顺序1、有船 2、判断船只速度等属性 3、判断背包超重 4、货物 5、判断补给
			if (m_pResult->n_shipcrew < 1)
			{
				this->button_callBack();
			}
			else
			{
				if (m_pResult->shipspeed < 0 || m_pResult->min_defense < 0 || m_pResult->min_attack < 0 || m_pResult->min_steer_speed < 0)
				{
					this->button_callBack();
				}
				else
				{
					if (m_pResult->currentpackagesize > m_pResult->maxpackagesize)
					{
						this->button_callBack();
					}
					else
					{
						if (m_pResult->currentgoodsweight > m_pResult->maxgoodsweight)
						{
							this->button_callBack();
						}
						else
						{
							//判断补给
							if (m_pResult->currentsupply >= m_pResult->maxsupply)
							{
								//水手是否足够
								if (m_pResult->sailorsisenough)
								{
									moveMapToStart();
								}
								else
								{
									ProtocolThread::GetInstance()->calFastHireCrew(UILoadingIndicator::create(this));
								}
							}
							else
							{
								if (m_dialogTag == STEP_FAST_RECRUIT_SAILORS)
								{
									ProtocolThread::GetInstance()->calFastHireCrew(UILoadingIndicator::create(this));
								}
								else
								{
									showSupplyPanelData();
								}
							}
						}
					}
				}
			}
		}
	}
}
void UIPort::updateRoleCoin()
{
	auto viewBar=getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_actionbar");
	auto buttonSilver=viewBar->getChildByName("button_silver");
	auto label_silver_num=buttonSilver->getChildByName<Text*>("label_silver_num");
	label_silver_num->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iCoin)));
	auto buttonGold=viewBar->getChildByName("button_gold");
	auto label_gold_num=buttonGold->getChildByName<Text*>("label_gold_num");
	label_gold_num->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iGold)));
	setTextFontSize(label_silver_num);
	setTextFontSize(label_gold_num);
}
void UIPort::chatContentBySupply()
{	
	if (!m_pResult)
	{
		return;
	}
	auto view=	getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_chief_mate_dialog");
	auto label_content_supply_all=view->getChildByName<Text*>("label_content_supply_all");
	std::string content;
	//判断顺序1、有船 2、判断船只速度等属性 3、判断背包超重 4、货物 5、判断补给
	if (m_pResult->n_shipcrew < 1)
	{
		content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_NOT_SHIP"];
	}
	else
	{
		if (m_pResult->shipspeed < 0 || m_pResult->min_defense < 0 || m_pResult->min_attack < 0 || m_pResult->min_steer_speed < 0)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_SHIP_NOT_SPEED"];
		}
		else
		{
			if (m_pResult->currentpackagesize > m_pResult->maxpackagesize)
			{
				content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_BAG_OVERWEIGHT"];
			}
			else
			{
				if (m_pResult->currentgoodsweight > m_pResult->maxgoodsweight)
				{
					content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_CABIN_CAPACITY_FULL"];
				}
				else
				{
					//判断补给
					if (m_pResult->currentsupply >= m_pResult->maxsupply)
					{		
						//水手是否足够
						if (m_pResult->sailorsisenough)
						{
							content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_CAPTAINCHAT_SUPPLY_FULL"];
						}
						else
						{
							content = SINGLE_SHOP->getTipsInfo()["TIP_LACK_URGNET_SAILOR_EFFECT_SAIL_SPEED_CONTENT"];
						}
					}
					else
					{
						content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_CAPTAINCHAT_SUPPLY_NOT_FULL"];
					}
				}
			}
		}
	}	
	std::string new_vaule = SINGLE_HERO->m_sName;
	std::string old_vaule = "[heroname]";
	repalce_all_ditinct(content,old_vaule,new_vaule);
	label_content_supply_all->setString(content);
}
void UIPort::showDockMapData(const GetVisitedCitiesResult*result)
{
	if (!result){ return; }
	auto view=getViewRoot(DOCK_COCOS_RES[VIEW_DOCK_MAP_CSB]);
	auto button_ott = view->getChildByName<Button*>("button_city_otttoman");
	button_ott->setTouchEnabled(false);
	auto button_eou = view->getChildByName<Button*>("button_city_eourp");
	button_eou->setTouchEnabled(false);
	//auto image_sailor=view->getChildByName("image_sailor");
	//auto ship_sailor_num=image_sailor->getChildByName<Text*>("label_ship_sailor_num_1");
	//auto ship_sailor_num_2=image_sailor->getChildByName<Text*>("label_ship_sailor_num_2");
	
	//ship_sailor_num->setString(StringUtils::format("%d",result->currentcrewnum));
	//ship_sailor_num_2->setString(StringUtils::format("/%d",result->maxcrewnum));
	//auto progressbar_sailor_Bg=image_sailor->getChildByName("image_progressbar_sailor");
	//auto button_caveat=image_sailor->getChildByName<Button*>("button_caveat");
//	button_caveat->addTouchEventListener(CC_CALLBACK_2(WharfLayer::menuCall_func,this));
	//auto progressbar_sailor=progressbar_sailor_Bg->getChildByName<LoadingBar*>("progressbar_durable");
	//progressbar_sailor->setPercent(100.0*(result->currentcrewnum/(result->maxcrewnum*1.0)));

	//auto image_supply=view->getChildByName("image_supply");
	//auto label_ship_supply_num=image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	
	//label_ship_supply_num->setString(StringUtils::format("%d/%d",result->currentsupply,result->maxsupply));
	
	//auto image_progressbar_supply=image_supply->getChildByName("image_progressbar_supply");
	//auto progressbar_supply=image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
	//progressbar_supply->setPercent(100.0*(result->currentsupply/(result->maxsupply*1.0)));
//当前水手小于最大水手的10%时，显示红色警告图片，当影响船速时显示橙色警告
//	if (result->sailorsisenough)
	//{
	//	ship_sailor_num->setTextColor(Color4B(0, 0, 0,255));
	//	button_caveat->setVisible(false);
//	}
//	else
//	{
	//	ship_sailor_num->setTextColor(Color4B(255, 0, 0,255));
	//	button_caveat->setVisible(true);
	//	if (result->currentcrewnum <= (result->maxcrewnum*0.1))
//		{
//			button_caveat->loadTextureNormal("cocosstudio/login_ui/common/hints_sailor_2.png");		
	//		n_sailorNotEnoughTag = SAILOR_URGENT_WARNING;
//		}
	//	else
	//	{
//			button_caveat->loadTextureNormal("cocosstudio/login_ui/common/hints_sailor_1.png");
		//	n_sailorNotEnoughTag = SAILOR_NOT_ENOUGH_EFFECT_SPEED;
//		}
//	}

	auto panel_max_day=view->getChildByName("panel_max_day");
	auto label_max_day_num=panel_max_day->getChildByName<Text*>("label_max_day_num");
	label_max_day_num->setString(StringUtils::format("%d",m_pResult->endurancedays));
//chengyuan++
	//label_ship_supply_num->setTextHorizontalAlignment(TextHAlignment::CENTER);
   
}
void UIPort::showSupplyPanelData()
{
	m_dialogTag = STEP_SUPPLY;
	if (!m_pResult)
	{
		return;
	}
	openView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto supplyView = getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	
	auto slider_dropitem_num=supplyView->getChildByName<Slider*>("slider_dropitem_num");
	slider_dropitem_num->addEventListenerSlider(this,sliderpercentchangedselector(UIPort::supplySliderChange));

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
		label_ship_supply_num_1->setTextColor(Color4B(46, 125, 50,255));
	}
	else
	{
		label_ship_supply_num_1->setTextColor(Color4B(40, 25, 13,255));
	}
	label_ship_supply_num_2->setString(StringUtils::format("/%d", m_pResult->maxsupply));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width );
	progressbar_supply->setPercent(100.0*(m_pResult->currentsupply*1.0/(m_pResult->maxsupply*1.0)));

	//progressbar_supply_add
	auto progressbar_supply_add=image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply_add");
	progressbar_supply_add->setPercent(100);
	m_nAddSupplyAmount=m_pResult->maxsupply-m_pResult->currentsupply;

	auto panel_salary=supplyView->getChildByName("panel_salary");
	auto label_cost_num=panel_salary->getChildByName<Text*>("label_cost_num");
	label_cost_num->setString(numSegment(StringUtils::format("%d",(m_pResult->maxsupply-m_pResult->currentsupply)*m_pResult->costpersupply)));
//chengyuan++
	label_cost_num->ignoreContentAdaptWithSize(true);
	label_cost_num->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	auto i_silver = supplyView->getChildByName("image_silver_1");
	i_silver->setPositionX(label_cost_num->getPositionX() - label_cost_num->getContentSize().width - i_silver->getContentSize().width / 2);
//
}
void UIPort::showFallInfomation(std::string name)
{
	
	openView(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
	auto viewFall = getViewRoot(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
	viewFall->setLocalZOrder(100);
	auto label_content=viewFall->getChildByName<Text*>("label_content");
	label_content->setString(SINGLE_SHOP->getTipsInfo()[name]);
	//标题
	auto label_result = viewFall->getChildByName<Text*>("label_result");
	label_result->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_CANCEL_DOCK_FORM_FLEET_TITLE"]); 
}
void UIPort::moveMapToStart()
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
		//目前检测主线和红胡子剧情任务
		notifyCompleted(NONE);
	}
	else
	{
		return;
	}
}
void UIPort::captainChatOut()
{
	m_bAideClick = true;
	auto view=	getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_chief_mate_dialog");
	auto image_chief_mate=view->getChildByName("image_chief_mate");
	auto image_dialog_bg=view->getChildByName("image_dialog_bg");
	auto label_bargirl=view->getChildByName("label_bargirl");
	auto label_content_supply_all=view->getChildByName<Text*>("label_content_supply_all");
	auto image_anchor=view->getChildByName("image_anchor");

	image_chief_mate->runAction(Sequence::createWithTwoActions(DelayTime::create(0.3),MoveBy::create(1, Vec2(-800,0))));
	image_dialog_bg->runAction(FadeOut::create(0.3));
	label_bargirl->runAction(FadeOut::create(0.01));
	label_content_supply_all->runAction(FadeOut::create(0.01));
	image_anchor->setOpacity(0);
	image_anchor->runAction(FadeOut::create(0.01));
}

void UIPort::scaleMaptoNomal(){

	m_pImageMap->setAnchorPoint(Vec2(0.5, 0.5));
	m_pImageMap->runAction(ScaleTo::create(0.5, 1));
	for (int i = 0; i < m_vCities.size(); i++)
	{
		m_vCities.at(i)->runAction(ScaleTo::create(0.5, 1));
	}
	m_pButton_backcity->setVisible(true);
	m_bIsScale = false;

}
void UIPort::supplySliderChange(Ref* obj,cocos2d::ui::SliderEventType type)
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
	//增加补给
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
	label_ship_supply_num_1->setString(StringUtils::format("%d",m_pResult->currentsupply +supplyChange));
	label_ship_supply_num_2->setString(StringUtils::format("/%d",m_pResult->maxsupply));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width);
	//label_ship_supply_num_1->setString(StringUtils::format("%d/%d",m_pResult->currentsupply+supplyChange,m_pResult->maxsupply));
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

Vec2 UIPort::isOutofMapSide(Vec2 targetPos, Size mapSize)
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
void UIPort::showFastHirePanelData()
{
	m_dialogTag = STEP_FAST_RECRUIT_SAILORS;
	if (!m_pFastHireCrewResult)
	{
		return;
	}
	openView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto supplyView = getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto slider_dropitem_num = supplyView->getChildByName<Slider*>("slider_dropitem_num");
	slider_dropitem_num->addEventListenerSlider(this, sliderpercentchangedselector(UIPort::fastHireCrewSliderChange));
	auto label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(supplyView, "label_title"));
	label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SAILOR_QUICH_HIRE_BUTTON_CONTENT"]);

	auto image_supply = dynamic_cast<ImageView*>(supplyView->getChildByName("image_supply"));
	image_supply->loadTexture("cocosstudio/login_ui/common/sailor.png");

	auto image_progressbar_supply = image_supply->getChildByName("image_progressbar_supply");
	auto progressbar_supply = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
	progressbar_supply->setVisible(false);
	//progressbar_supply->setPercent(100.0*(m_pFastHireCrewResult->owncrewnum*1.0 / (m_pFastHireCrewResult->availmaxcrewnum*1.0)));
	auto label_ship_supply_num_1 = image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	label_ship_supply_num_1->setString(StringUtils::format("%d", m_pFastHireCrewResult->availmaxcrewnum));
	auto label_ship_supply_num_2 = image_supply->getChildByName<Text*>("label_ship_supply_num_2");
	label_ship_supply_num_2->setString(StringUtils::format("/%d", m_pFastHireCrewResult->availmaxcrewnum));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width );
	//progressbar_supply_add
	auto progressbar_supply_add = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply_add");
	progressbar_supply_add->setPercent(100);
	m_nFastHireCrewNum = m_pFastHireCrewResult->availmaxcrewnum;

	auto panel_salary = supplyView->getChildByName("panel_salary");
	auto label_cost_num = panel_salary->getChildByName<Text*>("label_cost_num");
	//花费金币数
	int coinNums = (ceil(m_nFastHireCrewNum * 1.0 / m_pFastHireCrewResult->hirecrewnumonce) + 1) * (m_pFastHireCrewResult->hirecrewfee) + m_pFastHireCrewResult->crewprice* m_nFastHireCrewNum;

	label_cost_num->setString(numSegment(StringUtils::format("%d", coinNums)));
	label_cost_num->ignoreContentAdaptWithSize(true);
	label_cost_num->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	auto i_silver = supplyView->getChildByName("image_silver_1");
	i_silver->setPositionX(label_cost_num->getPositionX() - label_cost_num->getContentSize().width - i_silver->getContentSize().width / 2);

}
void UIPort::fastHireCrewSliderChange(Ref* obj, cocos2d::ui::SliderEventType type)
{
	if (type != SliderEventType::SLIDER_PERCENTCHANGED)
	{
		return;
	}
	if (!m_pFastHireCrewResult){ return; }
	auto supplyView = getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto image_supply = supplyView->getChildByName("image_supply");
	auto label_ship_supply_num_1 = image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	auto label_ship_supply_num_2 = image_supply->getChildByName<Text*>("label_ship_supply_num_2");
	auto image_progressbar_supply = image_supply->getChildByName("image_progressbar_supply");

	auto progressbar_supply = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
	//progressbar_supply->setPercent(100.0*(m_pFastHireCrewResult->owncrewnum*1.0 / (m_pFastHireCrewResult->availmaxcrewnum*1.0)));
	progressbar_supply->setVisible(false);
	auto slider_dropitem_num = supplyView->getChildByName<Slider*>("slider_dropitem_num");

	//增加补给
	auto progressbar_supply_add = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply_add");
	auto perChange = slider_dropitem_num->getPercent();
	progressbar_supply_add->setPercent(perChange);

	auto panel_salary = supplyView->getChildByName("panel_salary");
	auto label_cost_num = panel_salary->getChildByName<Text*>("label_cost_num");

	int32_t numChange = m_pFastHireCrewResult->availmaxcrewnum*slider_dropitem_num->getPercent() / 100.0;
	if (numChange == 0)
	{
		numChange = 1;
	}
	m_nFastHireCrewNum = numChange;
	int coinNums = (ceil(m_nFastHireCrewNum * 1.0 / m_pFastHireCrewResult->hirecrewnumonce) + 1) * (m_pFastHireCrewResult->hirecrewfee) + m_pFastHireCrewResult->crewprice* m_nFastHireCrewNum;

	label_cost_num->setString(numSegment(StringUtils::format("%d", coinNums)));
	label_ship_supply_num_1->setString(StringUtils::format("%d", numChange));
	label_ship_supply_num_2->setString(StringUtils::format("/%d", m_pFastHireCrewResult->availmaxcrewnum));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width );
}
void UIPort::chatfastHireContent()
{
	if (!m_pResult)
	{
		return;
	}
	m_dialogTag = STEP_FAST_RECRUIT_SAILORS;
	auto view = getViewRoot(DOCK_COCOS_RES[VIEW_CHIEF_DIALOG_CSB])->getChildByName("panel_chief_mate_dialog");
	auto label_content_supply_all = view->getChildByName<Text*>("label_content_supply_all");
	std::string content;
	if (m_pResult->sailorsisenough)
	{
		content = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_CAPTAINCHAT_SUPPLY_FULL"];
	}
	else
	{
		content = SINGLE_SHOP->getTipsInfo()["TIP_LACK_URGNET_SAILOR_EFFECT_SAIL_SPEED_CONTENT"];
	}
	std::string new_vaule = SINGLE_HERO->m_sName;
	std::string old_vaule = "[heroname]";
	repalce_all_ditinct(content, old_vaule, new_vaule);
	label_content_supply_all->setString(content);
}
void UIPort::openAutopilotView()
{
	openView(DOCK_COCOS_RES[VIEW_AUTOPILOT_CSB]);
	auto autopilot = getViewRoot(DOCK_COCOS_RES[VIEW_AUTOPILOT_CSB]);
	auto listview_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(autopilot, "listview_content"));
	auto panel_city_1 = autopilot->getChildByName<Layout*>("panel_city_1");
	auto image_div_2 = autopilot->getChildByName<ImageView*>("image_div_2");
	auto image_div_1 = autopilot->getChildByName<ImageView*>("image_div_1");
	auto button_yes_1 = autopilot->getChildByName<Button*>("button_yes_1");
	auto text_no = autopilot->getChildByName<Text*>("text_no");
	text_no->setVisible(true);
	panel_city_1->setTouchEnabled(true);
	panel_city_1->addTouchEventListener(CC_CALLBACK_2(UIPort::menuCall_func, this));
	int num = m_pResult->n_city;
	CityDefine** citydefine = m_pResult->city;
	for (int i = 0; i < num; i++)
		{
			auto l_clone = panel_city_1->clone();
			CityDefine* cityDefine = m_pResult->city[i];
			if (cityDefine->cityid == SINGLE_HERO->m_iCityID)
			{
			}
			else
			{
				l_clone->setTag(START_INDEX + cityDefine->cityid);

				auto image_flag = l_clone->getChildByName<ImageView*>("image_flag");
				auto label_city = l_clone->getChildByName<Text*>("label_city");
				auto image_press = l_clone->getChildByName<ImageView*>("image_press");
				auto text_no_1 = l_clone->getChildByName<Text*>("text_no");
				image_flag->loadTexture(getCountryIconPath(citydefine[i]->nation));
				label_city->setString(SINGLE_SHOP->getCitiesInfo()[citydefine[i]->cityid].name);
				listview_content->pushBackCustomItem(l_clone);
				l_clone->setTouchEnabled(true);
				l_clone->addTouchEventListener(CC_CALLBACK_2(UIPort::menuCall_func, this));

				if (i == num - 1)
				{
					auto l_clonediv1 = image_div_1->clone();
					listview_content->pushBackCustomItem(l_clonediv1);
				}
				else
				{
					auto l_clonediv2 = image_div_2->clone();
					listview_content->pushBackCustomItem(l_clonediv2);
				}
				m_nFirstCityId = citydefine[i]->cityid;
				m_nAutomaticCityId = citydefine[i]->cityid;
				if (isCanAutomatic() == false)
				{
					setGLProgramState(image_flag, true);
					text_no_1->setVisible(false);
				}
				std::string text = SINGLE_SHOP->getTipsInfo()["TIP_WHARF_AUTOSEA1"];
				std::string new_value = StringUtils::format("%d", m_nAutomaticDays);
				std::string old_value = "[num]";
				repalce_all_ditinct(text, old_value, new_value);
				text_no_1->setString(text);
			}
	}
	listview_content->removeItem(1);
	listview_content->removeItem(1);
	listview_content->removeItem(1);
	listview_content->removeItem(1);
	auto image_pulldown = autopilot->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listview_content, image_pulldown);
}
