#include "UITavern.h"
#include "UIChatHint.h"
#include "CompanionTaskManager.h"
#include "UIVoyageSlider.h"
#include "MainTaskManager.h"

#include "Utils.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIStore.h"
#include "TimeUtil.h"

static int calc_victory_percent(int my_power,int npc_power){
	double f_my_power = (double)my_power;
	double f_npc_power = (double)npc_power;
	int victorypercent = f_my_power * f_my_power * f_my_power
					/ (f_my_power * f_my_power * f_my_power + f_npc_power * f_npc_power * f_npc_power) * 100.0;
	if (victorypercent <= 2)
		victorypercent = 0;
	if (victorypercent >= 98)
		victorypercent = 100;
	return victorypercent;
}

static int *get_nozero_int_array_from_string(char *p,char s_char,int *size)
{
	if (p && p[0])
	{
		int len = 100;
		int *ids = new int[len];
		int i = 0;
		while (*p)
		{
			long value = atoi(p);
			if (value > 0)
			{
				ids[i++] = value;
			}
			while (*p != s_char && *p != 0)
			{
				p++;
			}
			if (!*p)
			{
				break;
			}
			p++;
		}
		if (i == 0)
		{
			if (size) 
			{
				*size = 0;
			}
			delete[] ids;
			return 0;
		}
		if (size) 
		{
			*size = i;
		}
		return ids;
	}
	if (size) 
	{
		*size = 0;
	}
	return 0;
}

static int *get_int_array_from_string(char*p,char s_char,int*size)
{
	if (p && p[0])
	{
		int len = 100;
		int *ids = new int[len];
		int i=0;
		while (*p)
		{
			long value = atoi(p);
			ids[i++] = value;
			while (*p != s_char && *p != 0) 
			{
				p++;
			}
			if (!*p)
			{
				break;
			}
			p++;
		}
		if (i == 0)
		{
			if(size)
			{
				*size = 0;
			}
			delete[] ids;
			return 0;
		}
		if (size) 
		{
			*size = i;
		}
		return ids;
	}
	if (size)
	{
		*size = 0;
	}
	return 0;
}

UITavern::UITavern() :
	m_pBarInfoResult(nullptr),
	m_pTaskButton(nullptr),
	m_nFavour(0),
	m_pGiftResult(nullptr),
	m_pMainButton(nullptr),
	m_pIntelligenceResult(nullptr),
	m_pCaptainButton(nullptr),
	m_pCurTaskDefine(nullptr),
	m_pNpcScoreResult(nullptr),
	m_pCh(nullptr),
	m_nChatContentIndex(CONTENT_WELCOME)
{
	m_pSpriteForFavorAnimate = nullptr;
	m_pAvailCrewNumResult = nullptr;
	m_pHandleTaskResult = nullptr;
	m_pBarGirlButton = nullptr;
	m_pGiftButton = nullptr;
	m_BarGirlContentLabel = nullptr;
	chatTxt = nullptr;
	anchPic = nullptr;

	m_vMyCaptains.clear();
	m_vShopCaptains.clear();
	m_vTaskIds.clear();
	m_vCoreTaskIds.clear();
	m_vNpcIdx.clear();
	m_vCityIdx.clear();
	m_BonusMap.clear();

	chatContent = "";
	
	m_nCaptainFlushTime = -2;
	m_nTaskFlushTime = -2;
	m_nViewIndex = 0;
	m_nBarIndex = 0;
	m_nConfirmIndex = 0;
	m_nTaskFalg = 0;
	m_nTaskId = 0;
	m_nCheers = 0;
	m_nCurSailorNum = 0;
	lenNum = 0;
	lenAfter = 0;
	plusNum = 0;
	maxLen = 0;
	m_nToTalDialogPage = 1;
	m_nCurDialogPage = 1;
	bargirlPositionY = 0;
	triggerDialogId = 0;
	m_nlikeRecievedGift = 0;

	m_eUIType = UI_PUB;

	m_bIsBanquet = false;
	m_bIsHint = false;
	m_bIsFriend = false;
	m_bIsLevelUp = false;
	m_bIsPrestigeUp = false;
	m_bIsCaptainUp = false;
	m_bChatButtonBackClick = true;
	m_bActionRunning = false;
	m_bChiefClick = false;
	m_IsVillage = false;
	wordLenBoo = false;
	guardDiaolgOver = false;
	bargirlClickBoo = true;
	favorDegreeActionPlayed = false;
	m_vSkillDefine.clear();
	m_lastTaskid = 0;
	m_ndissmissSailors = 0;
	BarGLProgram = nullptr;
}

UITavern::~UITavern()
{
	unregisterCallBack();
	if (m_pBarInfoResult)
	{
		get_bar_info_result__free_unpacked(m_pBarInfoResult,0);
	}
	if (m_pGiftResult)
	{
		get_personal_item_result__free_unpacked(m_pGiftResult,0);
	}
	if (m_pIntelligenceResult)
	{
		get_intelligence_result__free_unpacked(m_pIntelligenceResult, 0);
	}
	if (m_pAvailCrewNumResult)
	{
		get_avail_crew_num_result__free_unpacked(m_pAvailCrewNumResult, 0);
	}
	if (m_pHandleTaskResult)
	{
		handle_task_result__free_unpacked(m_pHandleTaskResult, 0);
	}
//	if (m_pCurTaskDefine)
//	{
//		task_define__free_unpacked(m_pCurTaskDefine,0);
//	}
	if (m_pNpcScoreResult)
	{
		get_npc_score_result__free_unpacked(m_pNpcScoreResult, 0);
	}
	m_pTaskButton = nullptr;
	m_pMainButton = nullptr;
	m_pCaptainButton = nullptr;
	m_pSpriteForFavorAnimate = nullptr;
	m_pAvailCrewNumResult = nullptr;
	m_pHandleTaskResult = nullptr;
	m_pBarGirlButton = nullptr;
	m_pGiftButton = nullptr;
	m_BarGirlContentLabel = nullptr;
	chatTxt = nullptr;
	anchPic = nullptr;
	m_pCh = nullptr;

	m_vMyCaptains.clear();
	m_vShopCaptains.clear();
	m_vTaskIds.clear();
	m_vCoreTaskIds.clear();
	m_vNpcIdx.clear();
	m_vCityIdx.clear();
	m_BonusMap.clear();
	this->unschedule(schedule_selector(UITavern::everySecondReflush));
	SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon0.plist");
	SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon1.plist");
}

void UITavern::onEnter()
{
	UIBasicLayer::onEnter();
}

void UITavern::onExit()
{
	UIBasicLayer::onExit();
}

UITavern* UITavern::createPup()
{
	UITavern* pup = new UITavern;
	if (pup && pup->init())
	{
		pup->autorelease();
		return pup;
	}
	CC_SAFE_DELETE(pup);
	CCASSERT(pup,"create pup fail");
	return nullptr;
}

bool UITavern::init()
{
	bool pRet = false;
	do 
	{
		CC_BREAK_IF(!UIBasicLayer::init());
		registerCallBack();
		ProtocolThread::GetInstance()->getBarInfo(0,UILoadingIndicator::create(this,m_eUIType));
		pRet = true;
	} while (0);
	return pRet;
}
//加载酒馆初始界面和部分初始数据
void UITavern::initResult()
{
	m_nFavour = m_pBarInfoResult->bargirlprestige;
	m_nTaskFlushTime = m_pBarInfoResult->surplusrefreshtime;
	m_nCaptainFlushTime = m_pBarInfoResult->distocaptainfinish;
	this->schedule(schedule_selector(UITavern::everySecondReflush),1.0f);
	
	openView(TARVEN_COCOS_RES[TARVEN_CSB]);
	openView(TARVEN_COCOS_RES[TARVEN_TWO_CSB], 10);
	auto viewTarven = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto image_bg = viewTarven->getChildByName<ImageView*>("image_bg");
	auto bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");

	t_content->setVisible(false);
	bargirlPositionY = i_anchor->getPositionY();
	m_BarGirlContentLabel = Label::create();
	t_content->getParent()->addChild(m_BarGirlContentLabel);
	m_BarGirlContentLabel->setAnchorPoint(Vec2(0,1));
	m_BarGirlContentLabel->setSystemFontSize(t_content->getFontSize());
	m_BarGirlContentLabel->setSystemFontName(t_content->getFontName());
	m_BarGirlContentLabel->setWidth(t_content->getBoundingBox().size.width);

	auto p_content = i_dialog_bg->getChildByName<Widget*>("panel_content");
	p_content->setContentSize(Size(p_content->getContentSize().width, m_BarGirlContentLabel->getSystemFontSize() * 3));
	m_BarGirlContentLabel->setPositionX(t_content->getPositionX());
	m_BarGirlContentLabel->setPositionY(m_BarGirlContentLabel->getSystemFontSize() * 3);

	auto p_tavern = viewTarven->getChildByName<Widget*>("panel_tavern");
	p_tavern->setTouchEnabled(false);
	image_bg->loadTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID,FLAG_PUP_AREA));
	auto m_bIsNeutralCityChange = false;
	for (int j = 0; j < SINGLE_HERO->m_iNwantedforceids; j++)
	{
		if (SINGLE_HERO->m_iCurCityNation == SINGLE_HERO->m_iWantedforceids[j])
		{
			m_bIsNeutralCityChange = true;
			break;
		}
	}
	//先判断是否是苏丹国家，在判断友好度
	if (SINGLE_HERO->m_iCurCityNation == 8)
	{
		if (m_pBarInfoResult->usedsudanmask)
		{
			m_bIsFriend = true;
		}
		else
		{
			m_bIsFriend = false;
		}
	}
	else if (m_bIsNeutralCityChange)
	{
		m_bIsFriend = false;
	}
	else if (m_pBarInfoResult->friendvalue >=0)
	{
		m_bIsFriend = true;
		if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
		{
			m_bIsFriend = false;
		}
	}
	else
	{
		m_bIsFriend = false;
	}
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	view->setTouchEnabled(false);
	auto otherButton = view->getChildByName<Widget*>("panel_two_butter");
	otherButton->setVisible(false);
	otherButton->setOpacity(0);
	
	auto t_tavern = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_tavern"));
	if (UserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO) == OPEN_AUDIO)
	{
		playAudio();
	}
	showBarGirlChat(1);
	updateCoin(m_pBarInfoResult->coin,m_pBarInfoResult->gold);
	m_nCurSailorNum = m_pBarInfoResult->totalcrewnum;

	m_vMyCaptains.clear();
	m_vShopCaptains.clear();
	for (int i = 0; i < m_pBarInfoResult->n_mycaptain; i++)
	{
		m_vMyCaptains.push_back(m_pBarInfoResult->mycaptain[i]);
	}
	for (int i = 0; i < m_pBarInfoResult->n_barcaptain; i++)
	{
		m_vShopCaptains.push_back(m_pBarInfoResult->barcaptain[i]);
	}

	m_vTaskIds.clear();
	m_vCoreTaskIds.clear();
	m_vNpcIdx.clear();
	m_vCityIdx.clear();
	m_BonusMap.clear();

	int size = 0;
	int *ids = get_nozero_int_array_from_string(m_pBarInfoResult->taskids, ',', &size);
	int size1 = 0;
	int *ids_core = get_nozero_int_array_from_string(m_pBarInfoResult->coretaskids, ',', &size1);

	int npcNum = 0;
	int *npcIds = get_int_array_from_string(m_pBarInfoResult->npcindexstr, ',', &npcNum);

	int cityNum = 0;
	int *cityIds = get_int_array_from_string(m_pBarInfoResult->targetcitystr, ',', &cityNum);

	int bonusNum = 0;
	int*bonusIds = get_int_array_from_string(m_pBarInfoResult->bonusstr,',',&bonusNum);

	if (size > 0 && size == size1)
	{
		for (int i = 0; i < size; i++)
		{
			m_vTaskIds.push_back(ids[i]);
			m_vCoreTaskIds.push_back(ids_core[i]);
			m_vNpcIdx.push_back(npcIds[i]);
			m_vCityIdx.push_back(cityIds[i]);
			if(i<bonusNum)
			{
				m_BonusMap[ids_core[i]] = bonusIds[i];
			}
			else
			{
				m_BonusMap[ids_core[i]] = 0;
				log("bonus overbound.");
			}
		}
		delete[]ids;
		ids = 0;
		ids_core = 0;
	}
	
	if (ids)
	{
		delete[] ids;
	}
	if (ids_core)
	{
		delete[] ids_core;
	}
	if (npcIds)
	{
		delete[] npcIds;
	}
	if (cityIds)
	{
		delete[] cityIds;
	}

	int len = 100;
	int *npcId = new int[len];
	npcId[0] = 0;
	npcNum = 1;
	for (size_t i = 0; i < m_vTaskIds.size(); i++)
	{
		auto coreId = getCoreTaskIdFromTaskId(m_vTaskIds[i]);
		if (coreId)
		{
			auto type = SINGLE_SHOP->getCoreTaskById(coreId).type;
			if (type == 1)
			{
				npcId[npcNum] = SINGLE_SHOP->getCoreTaskById(coreId).finish_target_npcId[m_vNpcIdx[i]];
				npcNum++;
			}
		}
	}
	ProtocolThread::GetInstance()->getNpcScore(npcId, npcNum, UILoadingIndicator::create(this));
	if (npcId)
	{
		delete[] npcId;
	}
	m_pCurTaskDefine = m_pBarInfoResult->bartask;

	//聊天
	m_pCh = UIChatHint::createHint();
	this->addChild(m_pCh, 10);

	notifyCompleted(NONE);
}
/*
* 当剧情或者对话动作结束时调用该函数
* actionIndex, 当前动作的index
*/
void UITavern::notifyCompleted(int actionIndex)
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
void UITavern::doNextJob(int actionIndex)
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
			auto p_tavern = viewTarven->getChildByName<Widget*>("panel_tavern");
			auto p_bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
			addNeedControlWidgetForHideOrShow(p_tavern, true);
			addNeedControlWidgetForHideOrShow(p_bargirl_dialog, false);
			addNeedControlWidgetForHideOrShow(view, false);
			allTaskInfo(false, MAIN_STORY, 0);
			MainTaskManager::GetInstance()->notifyUIPartDelayTime(0);
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
			auto p_tavern = viewTarven->getChildByName<Widget*>("panel_tavern");
			auto p_bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
			addNeedControlWidgetForHideOrShow(p_tavern, true);
			addNeedControlWidgetForHideOrShow(p_bargirl_dialog, false);
			addNeedControlWidgetForHideOrShow(view, false);
			allTaskInfo(false, COMPANION_STORY, 0);
			CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(0);
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
			auto p_tavern = viewTarven->getChildByName<Widget*>("panel_tavern");
			auto p_bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
			auto viewQuest = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
			addNeedControlWidgetForHideOrShow(p_tavern, true);
			addNeedControlWidgetForHideOrShow(p_bargirl_dialog, false);
			addNeedControlWidgetForHideOrShow(view, false);
			addNeedControlWidgetForHideOrShow(viewQuest, false);
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

//初始化购买水手的滑动条
void UITavern::initSailorDialog()
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB]);
	auto w_slider = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_select_num"));
	auto ss = dynamic_cast<UIVoyageSlider*>(getChildByTag(1));
	if (ss)
	{
		ss->removeFromParentAndCleanup(true);
	}
	ss = UIVoyageSlider::create(w_slider,100,0,true);
	this->addChild(ss,1,1);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UITavern::sliderSailor,this));
}
//初始化酒吧女郎聊天交互框
void UITavern::initBarGirlChat()
{
	m_nChatContentIndex = CONTENT_BARGIRL_CHAT;
	this->scheduleOnce(schedule_selector(UITavern::showBarGirlChat), 0);
	minorButtonMoveToLeft(0);
}
//初始化酒水交互框
void UITavern::initFoodAndDrink()
{   
	/*--init drink and eat view ---*/
	auto menu_view = getViewRoot(TARVEN_COCOS_RES[TARVEN_ORDER_CSB]);
	menu_view->setTouchEnabled(false);
	auto l_foodAndDrink = dynamic_cast<ListView*>(menu_view->getChildByName("listview_order"));
	auto item = dynamic_cast<Button*>(menu_view->getChildByName("button_item"));
	item->addTouchEventListener(CC_CALLBACK_2(UITavern::foodAndDrinkButtonEvent, this));
	auto food_view = l_foodAndDrink->getItem(0);
	auto drink_view = l_foodAndDrink->getItem(1);
	auto l_food = food_view->getChildByName<ListView*>("listview_food");
	auto l_drink = drink_view->getChildByName<ListView*>("listview_drink");
	l_food->removeAllChildrenWithCleanup(true);
	l_drink->removeAllChildrenWithCleanup(true);

	auto foodDrinkData = SINGLE_SHOP->getFoodDrinkData();
	for (int i = 0; i < m_pBarInfoResult->n_foodmenu; i++)
	{
		CC_BREAK_IF(!m_pBarInfoResult->foodmenu[i]);
		BarMenuDefine* food = m_pBarInfoResult->foodmenu[i];
		auto b_food = item->clone();
		b_food->setTag(i);
		b_food->setSwallowTouches(false);
		auto i_food = b_food->getChildByName<ImageView*>("image_item");
		auto t_food_name = b_food->getChildByName<Text*>("label_item_name");
		auto t_food_num = b_food->getChildByName<Text*>("label_silver_num");
		t_food_name->setString(foodDrinkData[food->nameid]);
		t_food_num->setString(StringUtils::format("%d",food->price));
		l_food->pushBackCustomItem(b_food);
		i_food->ignoreContentAdaptWithSize(false);
		i_food->loadTexture(StringUtils::format("res/food_icon/food_%d.png",food->nameid));
	}

	for (int i = 0; i < m_pBarInfoResult->n_drinkmenu; i++)
	{
		CC_BREAK_IF(!m_pBarInfoResult->drinkmenu[i]);
		BarMenuDefine* drink = m_pBarInfoResult->drinkmenu[i];
		auto b_drink = item->clone();;
		b_drink->setTag(START_INDEX+i);
		b_drink->setSwallowTouches(false);
		auto i_drink = b_drink->getChildByName<ImageView*>("image_item");
		auto t_drink_name = b_drink->getChildByName<Text*>("label_item_name");
		auto t_drink_num = b_drink->getChildByName<Text*>("label_silver_num");
		t_drink_name->setString(foodDrinkData[drink->nameid]);
		t_drink_num->setString(StringUtils::format("%d",drink->price));
		l_drink->pushBackCustomItem(b_drink);
		i_drink->ignoreContentAdaptWithSize(false);
		i_drink->loadTexture(StringUtils::format("res/food_icon/food_%d.png",drink->nameid));
	}
	auto image_pulldown = menu_view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	addListViewBar(l_foodAndDrink,image_pulldown);
}
//初始化水手交互框
void UITavern::initSailor()
{
	auto sailor = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
	sailor->setTouchEnabled(false);
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(sailor,"label_title"));
	auto t_content = sailor->getChildByName<Text*>("label_content");
	auto b_chat1 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailor,"button_chat_1"));
	auto b_chat2 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailor,"button_chat_2"));
	auto b_chat3 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailor, "button_chat_3"));
	auto t_chat1 = b_chat1->getChildByName<Text*>("label_1");
	auto t_content1 = b_chat1->getChildByName<Text*>("label_content");
	auto i_silver = b_chat1->getChildByName<Text*>("image_silver");
	auto t_silver = b_chat1->getChildByName<Text*>("label_silver_num");
	auto t_silver_0 = b_chat1->getChildByName<Text*>("label_silver_num_0");
	auto t_chat2 = b_chat2->getChildByName<Text*>("label_2");
	auto t_content2 = b_chat2->getChildByName<Text*>("label_content");
	auto t_chat3= b_chat3->getChildByName<Text*>("label_3");
	auto t_content3 = b_chat3->getChildByName<Text*>("label_content");
	
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_TITLE"]);
	int n = 30 * (ceil(FAME_NUM_TO_LEVEL(m_pBarInfoResult->fame) / 10.0) + 1);
	std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT"];
	std::string new_vaule = StringUtils::format("%d",n);
	std::string old_vaule = "[num]";
	repalce_all_ditinct(content,old_vaule,new_vaule);
	t_content->setString(content);
	t_chat1->setString("1.");
	t_chat2->setString("2.");
	t_chat3->setString("3.");

	
	t_content1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT2"]);
	t_silver->setString(numSegment(StringUtils::format("%d",m_pBarInfoResult->treatforgetcrewcost)));
	b_chat1->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent, this));
	b_chat2->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent, this));
	b_chat3->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent, this));
	auto i_bargirl = sailor->getChildByName<ImageView*>("image_bargirl");
	i_bargirl->ignoreContentAdaptWithSize(false);
	i_bargirl->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID,FLAG_BAR_GIRL));
	i_bargirl->setGLProgram(BarGLProgram);
	if (m_bIsBanquet)
	{
		i_silver->setVisible(false);
		t_silver->setVisible(false);
		t_silver_0->setVisible(true);
		t_content2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT3"]);
	}else
	{
		i_silver->setVisible(true);
		t_silver->setVisible(true);
		t_silver_0->setVisible(false);
		t_content2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT1"]);
	}
	t_content3->setString(SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_FIRE_SAILORS_TITLE"]);
}
//初始化船长交互框
void UITavern::initCaptain()
{
	flushMyCaptains();
}
//刷新我的船长界面
void UITavern::flushMyCaptains()
{
	auto my_captains = getViewRoot(TARVEN_COCOS_RES[TARVEN_HIRE_CSB]);
	my_captains->setTouchEnabled(false);
	auto t_title = my_captains->getChildByName<Text*>("label_title");
	auto l_captain = my_captains->getChildByName<ListView*>("listview_captain");
	auto n_captain = my_captains->getChildByName<Widget*>("panel_no_captain");
	auto b_hire = my_captains->getChildByName<Button*>("button_hire");
	b_hire->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent, this));
	std::string titleStr = StringUtils::format("%s (%d/30)", SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_CAPTAIN_HIRED"].c_str(), m_vMyCaptains.size());
	t_title->setString(titleStr.c_str());

	auto image_pulldown = my_captains->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (m_vMyCaptains.empty())
	{
		l_captain->setVisible(false);
		n_captain->setVisible(true);
		addListViewBar(l_captain, image_pulldown);
		return;
	}
	l_captain->setVisible(true);
	n_captain->setVisible(false);
	auto item = my_captains->getChildByName<Widget*>("panel_captain");
	l_captain->removeAllChildrenWithCleanup(true);
	for (int i = 0; i < m_vMyCaptains.size(); i++)
	{
		auto captainDefine = m_vMyCaptains.at(i);
		auto item_clone = item->clone();
		item_clone->setTag(i);
		auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item_clone,"image_captain_head"));
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone,"label_lv"));
		auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone,"label_name"));
		auto i_num = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item_clone,"image_num"));
		auto t_idle = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone, "label_idle"));
		auto i_silver = item_clone->getChildByName("image_silver");
		auto t_wage = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone,"label_wage"));
		auto b_fire = dynamic_cast<Button*>(Helper::seekWidgetByName(item_clone,"button_fire"));
			
		t_lv->setString(StringUtils::format("Lv. %d",captainDefine->level));
		t_name->setString(SINGLE_SHOP->getCaptainData()[captainDefine->id].name);
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(getCompanionIconPath(captainDefine->id, 1));
		i_num->ignoreContentAdaptWithSize(false);
		if (captainDefine->position > 0)
		{
			t_idle->setVisible(false);
			t_idle->setString("");
			i_num->setVisible(true);
			i_num->ignoreContentAdaptWithSize(false);
			i_num->loadTexture(getPositionIconPath(captainDefine->position));
			i_silver->setVisible(true);
			t_wage->setVisible(true);
			std::string str = numSegment(StringUtils::format("%d", captainDefine->salary));
			t_wage->setString(str + "/" + SINGLE_SHOP->getTipsInfo()["TIP_DAY"].c_str());
			t_wage->ignoreContentAdaptWithSize(true);
			t_wage->setPositionX(i_silver->getPositionX() + i_silver->getContentSize().width / 2 + t_wage->getContentSize().width / 2 + 5);
		}else
		{
			i_num->setVisible(false);
			t_idle->setVisible(true);
			t_idle->setString(SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_CAPTAIN_STATUS_IDLE"]);
			i_silver->setVisible(false);
			t_wage->setVisible(false);
		}
		b_fire->setTag(i);
		b_fire->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
		for(int j = 0; j < 3; j++)
		{
			auto i_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item_clone,StringUtils::format("my_captain_image_skill_%d",j+1)));
			i_skill->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
			if (j < captainDefine->n_skills)
			{
				i_skill->setVisible(true);
				auto t_skillLv = i_skill->getChildByName<Text*>("text_item_skill_lv");
				i_skill->ignoreContentAdaptWithSize(false);
				i_skill->loadTexture(getSkillIconPath(captainDefine->skills[j]->id, SKILL_TYPE_CAPTAIN));
				setTextSizeAndOutline(t_skillLv,captainDefine->skills[j]->level);
			}else
			{
				i_skill->setVisible(false);
			}
		}
		l_captain->pushBackCustomItem(item_clone);
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	addListViewBar(l_captain,image_pulldown);
}
//更新我的船长数据
void UITavern::addMyCaptains(const GetCaptainResult* result)
{
	m_vShopCaptains.clear();
	for (int i = 0; i < result->n_captains;i++)
	{
		m_vShopCaptains.push_back(result->captains[i]);
	}
	m_vMyCaptains.push_back(result->barcaptain);
	flushMyCaptains();
}
//更新我的船长数据
void UITavern::deleteMyCaptain(const FireCaptainResult* result)
{
	m_vMyCaptains.clear();
	for (int i = 0; i < result->n_mycaptains; i++)
	{
		m_vMyCaptains.push_back(result->mycaptains[i]);
	}
	flushMyCaptains();
}
//刷新商店里船长界面
void UITavern::flushShopCaptains()
{
	openView(TARVEN_COCOS_RES[TARVEN_HIRE_DIALOG_CSB],11);
	auto shop_captains = getViewRoot(TARVEN_COCOS_RES[TARVEN_HIRE_DIALOG_CSB]);
	auto p_title = shop_captains->getChildByName<Widget*>("panel_title");
	auto t_title = p_title->getChildByName<Button*>("label_title");
	auto b_capatin = p_title->getChildByName<Button*>("button_captain_v");
	auto image_clock = p_title->getChildByName<ImageView*>("image_clock");
	auto t_time = p_title->getChildByName<Text*>("label_time");
	auto b_cancel = shop_captains->getChildByName<Button*>("button_cancel");
	auto b_ok = shop_captains->getChildByName<Button*>("button_ok");
	b_capatin->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
	b_cancel->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
	b_ok->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
	image_clock->setPositionX(t_title->getPositionX() + t_title->getBoundingBox().size.width + image_clock->getBoundingBox().size.width/2);
	t_time->setPositionX(image_clock->getPositionX() + image_clock->getBoundingBox().size.width / 2);
	char buf[100];
	int64_t myTime = m_nCaptainFlushTime;
	snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld", myTime / 3600, (myTime % 3600) / 60, myTime % 60);
	t_time->setString(buf);

	auto l_captain = shop_captains->getChildByName<ListView*>("listview_captain");
	auto n_captain1 = shop_captains->getChildByName<Widget*>("label_no_captain_1");
	auto n_captain2 = shop_captains->getChildByName<Widget*>("label_no_captain_2");
	auto image_pulldown = shop_captains->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (m_vShopCaptains.empty())
	{
		l_captain->setVisible(false);
		n_captain1->setVisible(true);
		n_captain2->setVisible(true);
		image_pulldown->setVisible(false);
		return;
	}
	l_captain->setVisible(true);
	n_captain1->setVisible(false);
	n_captain2->setVisible(false);
	auto item = shop_captains->getChildByName<Widget*>("panel_captain");
	item->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
	l_captain->removeAllChildrenWithCleanup(true);
	for (int i = 0; i < m_vShopCaptains.size(); i++)
	{
		auto captainDefine = m_vShopCaptains.at(i);
		auto item_clone = item->clone();
		item_clone->setTag(i);
		auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item_clone,"image_captain_head"));
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone,"label_lv"));
		auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone,"label_name"));
		auto t_salary = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone, "label_salary"));
		auto i_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item_clone, "image_silver"));
		auto t_wage = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone,"label_wage"));
		t_salary->setAnchorPoint(Vec2(0,0.5));
		t_salary->setPositionX(0);
		item_clone->setTag(i);
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(getCompanionIconPath(captainDefine->id, 1));
		t_lv->setString(StringUtils::format("Lv. %d",captainDefine->level));
		t_name->setString(SINGLE_SHOP->getCaptainData()[captainDefine->id].name);
		auto str = numSegment(StringUtils::format("%d", captainDefine->salary));
		t_wage->setString(str + "/" + SINGLE_SHOP->getTipsInfo()["TIP_DAY"].c_str());
		i_silver->setPositionX(t_salary->getPositionX() + t_salary->getBoundingBox().size.width + i_silver->getContentSize().width/2 + 5);
		t_wage->setPositionX(i_silver->getPositionX() + i_silver->getContentSize().width / 2 + t_wage->getBoundingBox().size.width/2 + 5);
		for(int j = 0; j < 3; j++)
		{
			auto i_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item_clone,StringUtils::format("shop_captain_image_skill_%d",j+1)));
			i_skill->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
			if (j < captainDefine->n_skills)
			{
				i_skill->setVisible(true);
				auto t_skillLv = i_skill->getChildByName<Text*>("text_item_skill_lv");
				i_skill->ignoreContentAdaptWithSize(false);
				i_skill->loadTexture(getSkillIconPath(captainDefine->skills[j]->id, SKILL_TYPE_CAPTAIN));
				setTextSizeAndOutline(t_skillLv,captainDefine->skills[j]->level);
			}else
			{
				i_skill->setVisible(false);
			}
		}
		l_captain->pushBackCustomItem(item_clone);
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(l_captain,image_pulldown);
	everySecondReflush(0);
}
//解雇船长交互框
void UITavern::openFireCaptainsConfirm(const int nIndex)
{
	openView(TARVEN_COCOS_RES[TARVEN_HIRE_CONFIRMFIRE_CSB],11);
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_HIRE_CONFIRMFIRE_CSB]);
	auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_captain_head"));
	auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_lv"));
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_name"));
	auto i_sivler = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver"));
	auto t_wage = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_wage"));
	auto i_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_num"));
	
	auto captainDefine = m_vMyCaptains.at(nIndex);
	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture(getCompanionIconPath(captainDefine->id, 1));
	t_lv->setString(StringUtils::format("Lv. %d",captainDefine->level));
	t_name->setString(SINGLE_SHOP->getCaptainData()[captainDefine->id].name);
	t_wage->setString(StringUtils::format("%s/%s", numSegment(StringUtils::format("%d", captainDefine->salary)).c_str(), SINGLE_SHOP->getTipsInfo()["TIP_DAY"].c_str()));
	i_sivler->setPositionX(t_wage->getPositionX() - i_sivler->getContentSize().width/2 - 5);
	if (captainDefine->position > 0)
	{
		i_position->setVisible(true);
		i_position->ignoreContentAdaptWithSize(false);
		i_position->loadTexture(getPositionIconPath(captainDefine->position));
	}else
	{
		i_position->setVisible(false);
		i_sivler->setPositionX(i_position->getPositionX());
		t_wage->setPositionX(i_sivler->getPositionX() + i_sivler->getContentSize().width/2 + 5);
	}
	auto b_yes = view->getChildByName<Button*>("button_fire_yes");
	auto b_no = view->getChildByName<Button*>("button_fire_no");
	b_yes->setTag(captainDefine->uniqueid);
	b_yes->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
	b_no->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
}
//初始化赏金榜交互框
void UITavern::initTask()
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
	view->setTouchEnabled(false);
	auto b_time = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_repair_v"));
	auto b_task = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_task"));
	auto b_reward = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_get_reward"));
	auto b_about = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_abort"));
	auto b_accept = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_accept"));
	auto b_item1 = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_items_1"));
	auto b_item2 = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_items_2"));
	b_time->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent,this));
	b_task->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent, this));
	b_reward->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent, this));
	b_about->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent, this));
	b_accept->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent, this));
	b_item1->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent, this));
	b_item2->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent, this));

	m_vSkillDefine.clear();
	if (m_pBarInfoResult->skill_mission_terminator_level > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_MISSION_TERMINATOR;
		skillDefine.lv = m_pBarInfoResult->skill_mission_terminator_level;
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		m_vSkillDefine.push_back(skillDefine);
	}

	if (m_pBarInfoResult->skill_late_submission > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_LATE_SUBMISSION;
		skillDefine.lv = m_pBarInfoResult->skill_late_submission;
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		m_vSkillDefine.push_back(skillDefine);
	}

	if (m_pBarInfoResult->captain_mission_terminator_level > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_CAPTAIN_MISSION_EXPERT;
		skillDefine.lv = m_pBarInfoResult->captain_mission_terminator_level;
		if (m_pBarInfoResult->captain_mission_terminator_captain_type == 1)
		{
			skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
		}
		else
		{
			skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
		}
		skillDefine.icon_id = m_pBarInfoResult->captain_mission_terminator_captain_id;
		m_vSkillDefine.push_back(skillDefine);
	}

	if (m_pBarInfoResult->captain_skill_late_submission > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_CAPTAIN_SECRETARY;
		skillDefine.lv = m_pBarInfoResult->captain_skill_late_submission;
		if (m_pBarInfoResult->captain_skill_late_submission_captain_type == 1)
		{
			skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
		}
		else
		{
			skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
		}
		skillDefine.icon_id = m_pBarInfoResult->captain_skill_late_submission_captain_id;
		m_vSkillDefine.push_back(skillDefine);
	}

	auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_skill_bg"));
	image_skill->setVisible(false);
	auto captain_skill = SINGLE_SHOP->getCaptainSkillInfo();

	for (size_t i = 0; i < m_vSkillDefine.size(); i++)
	{
		if (i == 0)
		{
			image_skill->ignoreContentAdaptWithSize(false);
			image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill->setVisible(true);
			image_skill->setTag(i);
			image_skill->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent,this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
		}
		else
		{
			auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
			image_skill_clone->ignoreContentAdaptWithSize(false);
			image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill_clone->setVisible(true);
			image_skill_clone->setTag(i);
			image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent,this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
			view->addChild(image_skill_clone);
		}
	}

	flushTaskList();
	everySecondReflush(0);
}
//刷新任务列表
void UITavern::flushTaskList()
{
	auto w_taskDialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
	auto l_left = w_taskDialog->getChildByName<ListView*>("listview_left");	
	auto b_task = w_taskDialog->getChildByName<Button*>("button_task");	
	l_left->removeAllChildrenWithCleanup(true);
	
	auto l_taskInfo = w_taskDialog->getChildByName<ListView*>("listview_task_info");
	auto t_noTask1 = w_taskDialog->getChildByName<Widget*>("label_no_task_1");
	auto t_noTask2 = w_taskDialog->getChildByName<Widget*>("label_no_task_2");
	l_taskInfo->setVisible(true);
	t_noTask1->setVisible(false);
	t_noTask2->setVisible(false);
	if (m_vTaskIds.empty() && !m_pCurTaskDefine)
	{
		m_nTaskFalg = 0;
		auto b_task_clone = b_task->clone();
		auto t_task_name = b_task_clone->getChildByName<Text*>("label_task_name");
		auto t_task_nameNo = b_task_clone->getChildByName<Text*>("label_task_name_no");
		t_task_nameNo->setVisible(true);
		t_task_name->setVisible(false);
		l_left->pushBackCustomItem(b_task_clone);
		l_taskInfo->setVisible(false);
		t_noTask1->setVisible(true);
		t_noTask2->setVisible(true);
		b_task_clone->setBright(false);
		b_task_clone->setTouchEnabled(false);
		auto image_pulldown_left = w_taskDialog->getChildByName<ImageView*>("image_pulldown_left");
		image_pulldown_left->setVisible(false);
		auto image_pulldown = w_taskDialog->getChildByName<ImageView*>("image_pulldown");
		image_pulldown->setVisible(false);
		w_taskDialog->getChildByName<Button*>("button_get_reward")->setVisible(false);
		w_taskDialog->getChildByName<Button*>("button_abort")->setVisible(false);
		w_taskDialog->getChildByName<Button*>("button_accept")->setVisible(false);
		return;
	}

	if (m_pCurTaskDefine)
	{
		auto b_task_clone = b_task->clone();
		auto t_task_name = b_task_clone->getChildByName<Text*>("label_task_name");
		auto t_task_nameNo = b_task_clone->getChildByName<Text*>("label_task_name_no");
		t_task_nameNo->setVisible(false);
		t_task_name->setVisible(true);
		std::string text;
		text += SINGLE_SHOP->getSideTaskInfo()[m_pCurTaskDefine->taskid].title;
		text += " (";
		long now = time(NULL);
		if (m_pCurTaskDefine->surplustime > 0)
		{
			if (m_pCurTaskDefine->taskstatus)
			{
				text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_FINISH"];
				b_task_clone->getChildByName<ImageView*>("image_notify")->setVisible(true);
			}else
			{
				text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_CONTINUE"];
			}
		}else 
		{	
			text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_TIEMOUT"];
			b_task_clone->getChildByName<ImageView*>("image_notify")->setVisible(true);
		}
		text += ")";
		t_task_name->setString(text);
		t_task_name->setOpacity(255);
		b_task_clone->setTag(START_INDEX+m_pCurTaskDefine->taskid);
		b_task_clone->addTouchEventListener(CC_CALLBACK_2(UITavern::taskEvent,this));
		l_left->pushBackCustomItem(b_task_clone);
	}
		
	for (int i = 0; i < m_vTaskIds.size(); i++)
	{
		auto b_task_clone = b_task->clone();
		auto t_task_name = b_task_clone->getChildByName<Text*>("label_task_name");
		auto t_task_nameNo = b_task_clone->getChildByName<Text*>("label_task_name_no");
		t_task_nameNo->setVisible(false);
		t_task_name->setVisible(true);
		int id = m_vTaskIds[i];
		t_task_name->setString(SINGLE_SHOP->getSideTaskInfo()[id].title);
		if (m_pCurTaskDefine)
		{
			t_task_name->setOpacity(125);
		}else
		{
			t_task_name->setOpacity(255);
		}
		b_task_clone->setTag(START_INDEX+id);
		b_task_clone->addTouchEventListener(CC_CALLBACK_2(UITavern::taskEvent,this));
		l_left->pushBackCustomItem(b_task_clone);
	}
	m_pTaskButton = l_left->getItem(0);
	m_pTaskButton->setBright(false);
	m_pTaskButton->setTouchEnabled(false);

	if (m_pCurTaskDefine)
	{
		if (m_pCurTaskDefine->taskstatus)
		{
			m_nTaskFalg = 2; 
		}else
		{
			m_nTaskFalg = 1;
		}
		showTaskInfo(m_pCurTaskDefine->taskid,m_pCurTaskDefine->taskcoreid,m_pCurTaskDefine->npcindex);
	}
	else
	{
		m_pCurTaskDefine = nullptr;
		m_nTaskFalg = 0;
		showTaskInfo(m_vTaskIds[0],m_vCoreTaskIds[0],m_vNpcIdx[0]);
	}

	auto image_pulldown = w_taskDialog->getChildByName<ImageView*>("image_pulldown_left");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 0.5);
	addListViewBar(l_left,image_pulldown);
}
//赏金榜任务的详情
void UITavern::showTaskInfo(const int nTaskId,const int nCoreTaskId,const int nNpcIdx)
{
	m_nTaskId = nTaskId;
	//检查任务是否包含额外奖励
	bool hasBonus = false;
	if (m_pCurTaskDefine)
	{
		if (nTaskId == m_pCurTaskDefine->taskid)
		{
			hasBonus = m_pCurTaskDefine->hasbonus;
		}
		else
		{
			if (m_BonusMap[nCoreTaskId] == 1){
				hasBonus = true;
			}
		}
	}
	else
	{
		if (m_BonusMap[nCoreTaskId] == 1){
			hasBonus = true;
		}
	}

	auto w_taskDialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
	auto l_taskInfo = w_taskDialog->getChildByName<ListView*>("listview_task_info");
	auto p_task_name = dynamic_cast<Widget*>(l_taskInfo->getItem(0));
	auto t_task_name = dynamic_cast<Text*>(p_task_name->getChildByName("label_task_name"));
	auto i_task_type = dynamic_cast<ImageView*>(p_task_name->getChildByName("image_quest_ship"));
	auto item = l_taskInfo->getItem(1);	
	auto p_task_title = item->getChildByName<Widget*>("panel_task_title");
	Text * t_task_content = item->getChildByName<Text*>("label_task_content");
	auto t_task_time = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_task_time"));
	auto t_task_time_num = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_task_time_num"));
	auto t_task_story = dynamic_cast<Text*>(l_taskInfo->getItem(2));

	t_task_content->setFontName("nofont");
	t_task_name->setString(SINGLE_SHOP->getSideTaskInfo()[nTaskId].title);
	i_task_type->loadTexture(getTaskType(SINGLE_SHOP->getSideTaskInfo()[nTaskId].type));
	std::string content;
	std::string new_vaule;
	std::string old_vaule;
	std::string html_color_begin;
	std::string html_color_end;

#if WIN32
	html_color_begin = "";
	html_color_end = "";
#else
	html_color_begin = "<font size='24' color='#A95601'>";
	html_color_end = "</font>";
#endif

	auto  CoreTaskInfo = SINGLE_SHOP->getCoreTaskById(nCoreTaskId);
	if (CoreTaskInfo.finish_target_npcId)
	{
		if (m_nTaskFalg == 2 && nTaskId == m_pCurTaskDefine->taskid)
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_TASK"];
			new_vaule = html_color_begin;
			new_vaule += SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->completecityid].name;
			new_vaule += html_color_end;
			old_vaule = "[city]";
			repalce_all_ditinct(content,old_vaule,new_vaule);
		}
		else
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_NPC"];
			new_vaule = html_color_begin;
			if (m_nTaskFalg != 0 && nTaskId == m_pCurTaskDefine->taskid)
			{
				new_vaule += StringUtils::format("%d/%d", m_pCurTaskDefine->completeamount, CoreTaskInfo.finish_number);
			}
			else
			{
				new_vaule += StringUtils::format("%d", CoreTaskInfo.finish_number);
			}
			new_vaule += html_color_end;
			old_vaule = "[num]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = html_color_begin;
			new_vaule += SINGLE_SHOP->getBattleNpcInfo()[CoreTaskInfo.finish_target_npcId[nNpcIdx]].name;
			new_vaule += html_color_end;
			old_vaule = "[npc]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = html_color_begin;
			new_vaule += SINGLE_SHOP->getZoneInfo()[CoreTaskInfo.zone[nNpcIdx]];
			new_vaule += html_color_end;
			old_vaule = "[zone]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
		}
	}

	if (CoreTaskInfo.finish_goods_id)
	{
		if (m_nTaskFalg == 2 && nTaskId == m_pCurTaskDefine->taskid)
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_TASK"];
			new_vaule = html_color_begin;
			new_vaule += SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->completecityid].name;
			new_vaule += html_color_end;
			old_vaule = "[city]";
			repalce_all_ditinct(content,old_vaule,new_vaule);
		}
		else
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_GOODS"];
			new_vaule = html_color_begin;
			if (m_nTaskFalg == 2 && nTaskId == m_pCurTaskDefine->taskid)
			{
				new_vaule += StringUtils::format("%d/%d", CoreTaskInfo.finish_goods_amount[nNpcIdx], CoreTaskInfo.finish_goods_amount[nNpcIdx]);
			}
			else if (m_nTaskFalg == 1 && nTaskId == m_pCurTaskDefine->taskid)
			{
				if (m_pCurTaskDefine->completeamount)
				{
					new_vaule += StringUtils::format("%d/%d", m_pCurTaskDefine->completeamount, CoreTaskInfo.finish_goods_amount[nNpcIdx]);
				}
				else
				{
					new_vaule += StringUtils::format("%d/%d", 0, CoreTaskInfo.finish_goods_amount[nNpcIdx]);
				}
			}
			else
			{
				new_vaule += StringUtils::format("%d", CoreTaskInfo.finish_goods_amount[nNpcIdx]);
			}
			new_vaule += html_color_end;
			old_vaule = "[num]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = html_color_begin;
			new_vaule += SINGLE_SHOP->getGoodsData()[CoreTaskInfo.finish_goods_id[nNpcIdx]].name;
			new_vaule += html_color_end;
			old_vaule = "[good]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = html_color_begin;
			new_vaule += SINGLE_SHOP->getCitiesInfo()[getCityIdxFromTaskId(nTaskId)].name;
			new_vaule += html_color_end;
			old_vaule = "[city]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
		}
	}
	
	int taskTime = CoreTaskInfo.time;
	if (!m_pCurTaskDefine || m_pCurTaskDefine->taskid != nTaskId)
	{
		if(m_pBarInfoResult)
		{
			auto effect_per = SINGLE_SHOP->getSkillTrees()[SKILL_LATE_SUBMISSION].effect_per_lv / SKILL_DER_MAX;
			auto effect_per_captain = SINGLE_SHOP->getCaptainSkillInfo()[SKILL_CAPTAIN_SECRETARY].effect_per_lv / SKILL_DER_MAX;
			taskTime = (int)(taskTime*1.0*(1.0 + m_pBarInfoResult->skill_late_submission * effect_per + m_pBarInfoResult->captain_skill_late_submission * effect_per_captain));
		}
	}
	t_task_time->setFontSize(20);
	t_task_time->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TIME_LIMIT"]);
	t_task_time->setTextColor(Color4B(40, 25, 13, 255));
	if (m_nTaskFalg == 0)
	{
		std::string time;
		time += StringUtils::format("%d",taskTime);
		time += SINGLE_SHOP->getTipsInfo()["TIP_MINUTE"];
		t_task_time_num->setString(time);
		t_task_time_num->setTextColor(Color4B(40, 25, 13, 255));
		html_color_begin = "<font size='24' color='#2E1D0E'>";
	}else
	{
		if (nTaskId != m_pCurTaskDefine->taskid)
		{
			std::string time;
			time += StringUtils::format("%d",taskTime);
			time += SINGLE_SHOP->getTipsInfo()["TIP_MINUTE"];
			t_task_time_num->setString(time);
			t_task_time_num->setTextColor(Color4B(40, 25, 13, 255));
			html_color_begin = "<font size='24' color='#2E1D0E'>";
		}else
		{
			long now = time(NULL);
			t_task_time->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_REMAINING_TIME"]);
			if (m_pCurTaskDefine->surplustime <= 0)
			{
				m_nTaskFalg = 1;
				t_task_time_num->setTextColor(Color4B(205, 25, 25, 255));
				t_task_time_num->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_TIEMOUT"]);
				html_color_begin = "<font size='24' color='#CD1919'>";
				content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_TASK_OUT_TIME"];
			}else
			{
				char buf[100];
				int64_t myTime = m_pCurTaskDefine->surplustime;
				snprintf(buf,sizeof(buf),"%02lld:%02lld:%02lld",myTime/3600,(myTime%3600)/60,myTime%60);
				t_task_time_num->setString(buf);
				if (m_nTaskFalg == 2)
				{
					t_task_time_num->setTextColor(Color4B(46, 125, 50, 255));
					html_color_begin = "<font size='24’ color='#2E7D32'>";
				}
				else
				{
					t_task_time_num->setTextColor(Color4B(40, 25, 13, 255));
					html_color_begin = "<font size='24' color='#2E1D0E'>";
				}
			}
		}		
	}
	std::string temp_content;
#if WIN32
	temp_content = content;
#else
	temp_content = "<html>";
	temp_content += html_color_begin;
	temp_content += content;
	temp_content += html_color_end;
	temp_content += "</html>";
#endif
	t_task_content->setString(temp_content);

	t_task_time_num->setPositionX(t_task_time->getPositionX() + t_task_time->getBoundingBox().size.width);
	std::string str = SINGLE_SHOP->getSideTaskInfo()[nTaskId].desc;
	if (CoreTaskInfo.finish_target_npcId)
	{
		std::string old_value1 = "[npc]";
		std::string new_value1 = SINGLE_SHOP->getBattleNpcInfo()[CoreTaskInfo.finish_target_npcId[nNpcIdx]].name;
		repalce_all_ditinct(str, old_value1, new_value1);
	}

	if(CoreTaskInfo.type == 2)
	{
		std::string old_value2 = "[city]";
		std::string new_value2 = SINGLE_SHOP->getCitiesInfo()[getCityIdxFromTaskId(nTaskId)].name;
		repalce_all_ditinct(str,old_value2,new_value2);
	}
	
	if (CoreTaskInfo.zone)
	{
		std::string old_value3 = "[zone]";
		std::string new_value3 = SINGLE_SHOP->getZoneInfo()[CoreTaskInfo.zone[nNpcIdx]];
		repalce_all_ditinct(str, old_value3, new_value3);
	}

	if(CoreTaskInfo.type == 2)
	{
		std::string old_value4 = "[goods]";
		std::string new_value4 = getGoodsName(CoreTaskInfo.finish_goods_id[nNpcIdx]);
		repalce_all_ditinct(str,old_value4,new_value4);
	}

	int easness_num = 100;
	if (CoreTaskInfo.type == 2)
	{
		easness_num = 100;
	}
	else
	{
		if (m_pNpcScoreResult && m_pNpcScoreResult->n_lowscores > nNpcIdx)
		{
			int hero_npc = (m_pNpcScoreResult->highescores[0] + m_pNpcScoreResult->lowscores[0]) / 2;
			int enemy_npc = (m_pNpcScoreResult->highescores[nNpcIdx - 1] + m_pNpcScoreResult->lowscores[nNpcIdx - 1]) / 2;
			easness_num = calc_victory_percent(hero_npc, enemy_npc);
		}
	}
	auto i_diff1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_diff_1"));
	auto i_diff2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_diff_2"));
	auto i_diff3 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_diff_3"));

	if (easness_num <= 20)
	{
		i_diff1->loadTexture("res/rarity/difficulty_1.png");
		i_diff2->loadTexture("res/rarity/difficulty_3.png");
		i_diff3->loadTexture("res/rarity/difficulty_3.png");
	}
	else if (easness_num <= 40)
	{
		i_diff1->loadTexture("res/rarity/difficulty_1.png");
		i_diff2->loadTexture("res/rarity/difficulty_2.png");
		i_diff3->loadTexture("res/rarity/difficulty_3.png");
	}
	else if (easness_num <= 60)
	{
		i_diff1->loadTexture("res/rarity/difficulty_1.png");
		i_diff2->loadTexture("res/rarity/difficulty_1.png");
		i_diff3->loadTexture("res/rarity/difficulty_3.png");
	}
	else if (easness_num <= 80)
	{
		i_diff1->loadTexture("res/rarity/difficulty_1.png");
		i_diff2->loadTexture("res/rarity/difficulty_1.png");
		i_diff3->loadTexture("res/rarity/difficulty_2.png");
	}
	else
	{
		i_diff1->loadTexture("res/rarity/difficulty_1.png");
		i_diff2->loadTexture("res/rarity/difficulty_1.png");
		i_diff3->loadTexture("res/rarity/difficulty_1.png");
	}

	t_task_story->setString(str);
	t_task_story->setContentSize(Size(t_task_story->getContentSize().width,36 + getLabelHight(str,t_task_story->getContentSize().width,t_task_story->getFontName())));
	auto w_common = item->getChildByName<Widget*>("panel_common");
	auto i_silver = w_common->getChildByName<ImageView*>("image_silver");
	auto t_silver = w_common->getChildByName<Text*>("label_silver_num");
	auto i_gold = w_common->getChildByName<ImageView*>("image_v");
	auto t_gold = w_common->getChildByName<Text*>("label_v_num");
	auto t_exp = w_common->getChildByName<Text*>("label_exp");
	auto t_expNum = w_common->getChildByName<Text*>("label_exp_num");
	auto i_prestige = w_common->getChildByName<ImageView*>("image_r");
	auto t_prestige = w_common->getChildByName<Text*>("label_r_num");
	t_silver->setString(numSegment(StringUtils::format("%d",CoreTaskInfo.rewards_coin)));
	t_gold->setString(numSegment(StringUtils::format("%d",hasBonus?CoreTaskInfo.rewards_gold + CoreTaskInfo.bonus_v_ticket : CoreTaskInfo.rewards_gold)));
	t_expNum->setString(StringUtils::format("%d",CoreTaskInfo.rewards_exp));
	t_prestige->setString(StringUtils::format("%d",CoreTaskInfo.rewards_reputation));

	auto w_force = item->getChildByName<Widget*>("panel_force_relation");
	auto i_country = w_force->getChildByName<ImageView*>("image_country");
	auto t_force = w_force->getChildByName<Text*>("label_force_relation_num");
	if (CoreTaskInfo.rewards_force_id)
	{
		w_force->setVisible(true);
		i_country->loadTexture(getCountryIconPath(CoreTaskInfo.rewards_force_id));
		t_force->setString(StringUtils::format("%d",CoreTaskInfo.rewards_force_affect));
	}
	else
	{
		w_force->setVisible(false);
	}

	auto w_equip = item->getChildByName<Widget*>("panel_equip");
	auto i_item1 = w_equip->getChildByName<ImageView*>("image_goods_bg1");
	auto b_item1 = w_equip->getChildByName<Button*>("button_items_1");
	auto i_goods1 = b_item1->getChildByName<ImageView*>("image_goods");
	auto t_good1 = i_goods1->getChildByName<Text*>("label_num");
	auto i_item2 = w_equip->getChildByName<ImageView*>("image_goods_bg2");
	auto b_item2 = w_equip->getChildByName<Button*>("button_items_2");
	auto i_goods2 = b_item2->getChildByName<ImageView*>("image_goods");
	auto t_good2 = i_goods2->getChildByName<Text*>("label_num");
 
	std::string name;
	std::string path;
	if (CoreTaskInfo.rewards_item1_id)
	{
		w_equip->setVisible(true);
		i_item1->setVisible(true);
		b_item1->setVisible(true);
		b_item1->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent,this));
		getItemNameAndPath(CoreTaskInfo.rewards_item1_type,CoreTaskInfo.rewards_item1_id,name,path);
		i_goods1->loadTexture(path);
		t_good1->enableOutline(Color4B::BLACK,OUTLINE_MIN);
		t_good1->setString(StringUtils::format("%d",CoreTaskInfo.rewards_item1_amount));
		setBgButtonFormIdAndType(i_item1, CoreTaskInfo.rewards_item1_id, CoreTaskInfo.rewards_item1_type);
	}else
	{
		i_item1->setVisible(false);
		b_item1->setVisible(false);
		w_equip->setVisible(false);
	}

	if (CoreTaskInfo.rewards_item2_id)
	{
		i_item2->setVisible(true);
		b_item2->setVisible(true);
		b_item2->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent,this));
		getItemNameAndPath(CoreTaskInfo.rewards_item2_type,CoreTaskInfo.rewards_item2_id,name,path);
		i_goods2->loadTexture(path);
		t_good2->enableOutline(Color4B::BLACK,OUTLINE_MIN);
		t_good2->setString(StringUtils::format("%d",CoreTaskInfo.rewards_item2_amount));
		setBgButtonFormIdAndType(i_item2, CoreTaskInfo.rewards_item2_id, CoreTaskInfo.rewards_item2_type);
	}else
	{
		i_item2->setVisible(false);
		b_item2->setVisible(false);
	}

	auto b_reward = w_taskDialog->getChildByName<Button*>("button_get_reward");
	auto b_about = w_taskDialog->getChildByName<Button*>("button_abort");
	auto b_accept = w_taskDialog->getChildByName<Button*>("button_accept");
	b_reward->setTag(nTaskId+START_INDEX);
	b_about->setTag(nTaskId+START_INDEX);
	b_accept->setTag(nTaskId+START_INDEX);
	
	if (m_nTaskFalg == 2 && nTaskId == m_pCurTaskDefine->taskid)
	{
		b_reward->setVisible(true);
		b_about->setTouchEnabled(false);
		b_about->setVisible(false);
		b_accept->setTouchEnabled(false);
		b_accept->setVisible(false);
		b_reward->getChildByName<ImageView*>("image_notify")->setVisible(true);
		if (m_pBarInfoResult->bartask->completecityid == SINGLE_HERO->m_iCityID || 
				m_nFavour >= FAVOUR_SIDE_TASK)
		{
			b_reward->setBright(true);
			b_reward->setTouchEnabled(true);
		}
		else
		{
			b_reward->setBright(false);
			b_reward->setTouchEnabled(false);
		}
	}
	else if (m_nTaskFalg == 1 &&  nTaskId == m_pCurTaskDefine->taskid)
	{
		b_reward->setTouchEnabled(false);
		b_reward->setVisible(false);
		b_about->setTouchEnabled(true);
		b_about->setVisible(true);
		b_accept->setTouchEnabled(false);
		b_accept->setVisible(false);
		long now = time(NULL);
		if (m_pCurTaskDefine->surplustime > 0)
		{
			b_about->getChildByName<ImageView*>("image_notify")->setVisible(false);
		}
		else
		{
			b_about->getChildByName<ImageView*>("image_notify")->setVisible(true);
		}
		
	}
	else if (m_nTaskFalg != 0 && nTaskId != m_pCurTaskDefine->taskid)
	{
		b_reward->setTouchEnabled(false);
		b_reward->setVisible(false);
		b_about->setTouchEnabled(false);
		b_about->setVisible(false);
		b_accept->setBright(false);
		b_accept->setTouchEnabled(true);
		b_accept->setVisible(true);
	}
	else
	{
		b_reward->setTouchEnabled(false);
		b_reward->setVisible(false);
		b_about->setTouchEnabled(false);
		b_about->setVisible(false);
		b_accept->setBright(true);
		b_accept->setTouchEnabled(true);
		b_accept->setVisible(true);
	}
	l_taskInfo->jumpToTop();
	l_taskInfo->refreshView();
	auto image_pulldown = w_taskDialog->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(l_taskInfo,image_pulldown);
	auto sroll = Helper::seekWidgetByName(dynamic_cast<Widget*>(l_taskInfo->getParent()), "image_pulldown");
	if (sroll != nullptr)
	{
		sroll = sroll->getChildByName<Widget*>("button_pulldown");
		Size inner_size = l_taskInfo->getInnerContainerSize();
		Size lv_size = l_taskInfo->getContentSize();

		l_taskInfo->setUserObject(sroll);
		l_taskInfo->addEventListenerScrollView(this, scrollvieweventselector(UITavern::scrollEvent));
		Size img_size = sroll->getSize();
		Vec2 anch = sroll->getAnchorPoint();
		
		Point	lv_pos = l_taskInfo->getPosition();

		auto newY = dynamic_cast<Widget*>(sroll->getParent())->getSize().height - img_size.height - 17;
	}
}

void UITavern::scrollEvent(Ref *node, ScrollviewEventType type)
{
	//BasicLayer::scrollEvent(node,type);
	if (type == ScrollviewEventType::SCROLLVIEW_EVENT_SCROLLING) {
		ListView *lv = dynamic_cast<ListView*>(node);
		if (lv->getUserObject() == nullptr) {
			lv->unscheduleAllSelectors();
			return;
		}
		Widget *sroll = dynamic_cast<Widget*>(lv->getUserObject());
		Size lv_size = lv->getContentSize();
		Size innerContainer = lv->getInnerContainerSize();
		Point innerCon_pos = lv->getInnerContainer()->getPosition();
		Point lv_pos = lv->getPosition();
		if (innerContainer.height < lv_size.height + 10) {
			lv->unscheduleAllSelectors();
			return;
		}
		if (!sroll->isVisible()) {
			sroll->setVisible(true);
			return;
		}
		Size img_size = sroll->getSize();
		auto len = dynamic_cast<Widget*>(sroll->getParent())->getSize().height - img_size.height - 17 * 2;
		auto factor = std::fabs(innerCon_pos.y * 1.0 / (innerContainer.height - lv_size.height));
		sroll->setPositionY(len * factor + 17);
	} 
	else if (type == ScrollviewEventType::SCROLLVIEW_EVENT_SCROLL_TO_TOP) 
	{
	}
}
//更新银币和V票
void UITavern::updateCoin(const int64_t nCoin,const int64_t nGold)
{
	std::string s_coin = numSegment(StringUtils::format("%lld",nCoin));
	std::string s_gold = numSegment(StringUtils::format("%lld",nGold));
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_silver_num"))->setString(s_coin);
	dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_gold_num"))->setString(s_gold);
	SINGLE_HERO->m_iCoin = nCoin;
	SINGLE_HERO->m_iGold = nGold;
	setTextFontSize(dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_silver_num")));
	setTextFontSize(dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_gold_num")));
}
//默认的button相应 
void UITavern::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	
	std::string name = button->getName();
	//商城
	if (isButton(button_silver))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType,1);
		return;
	}
	if (isButton(button_gold))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType,0);
		return;
	}
	//玩家升级
	if (isButton(panel_levelup))
	{  
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
			return;
		}
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp();
			UICommon::getInstance()->flushCaptainLevelUp(m_pHandleTaskResult->n_captains, m_pHandleTaskResult->captains);
			return;
		}
		return;
	}
	//声望升级
	if (isButton(panel_r_levelup))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp();
			UICommon::getInstance()->flushCaptainLevelUp(m_pHandleTaskResult->n_captains, m_pHandleTaskResult->captains);
			return;
		}
		return;
	}
	//船长升级
	if (isButton(panel_captain_addexp))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		return;
	}
	if (isButton(button_error_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		return;
	}
	//确认对话框
	if (isButton(button_s_yes)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		if (m_nConfirmIndex == CONFIRM_INDEX_BARGIRL_DRINK)
		{
			ProtocolThread::GetInstance()->treatBargirl(UILoadingIndicator::create(this));
			return;
		}

		if (m_nConfirmIndex == CONFIRM_INDEX_FOOD)
		{
			ProtocolThread::GetInstance()->barConsume(m_nBarIndex, 1, UILoadingIndicator::create(this));
			return;
		}
		if (m_nConfirmIndex == CONFIRM_INDEX_DRINK)
		{
			ProtocolThread::GetInstance()->barConsume(m_nBarIndex, 2, UILoadingIndicator::create(this));
			return;
		}
		if (m_nConfirmIndex == CONFIRM_INDEX_SAILOR_DRINK)
		{
			ProtocolThread::GetInstance()->barConsume(8, 3, UILoadingIndicator::create(this));
			return;
		}
		return;
	}
	//刷新确定按钮
	if (isButton(button_v_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_nConfirmIndex == CONFIRM_INDEX_CAPTAIN_FRESH)
		{
			ProtocolThread::GetInstance()->refreshCaptainList(1, UILoadingIndicator::create(this));
			return;
		}
		if (m_nConfirmIndex == CONFIRM_INDEX_TASK_FRESH)
		{
			ProtocolThread::GetInstance()->getTaskList(1, UILoadingIndicator::create(this));
			return;
		}
		return;
	}

	//回主城
	if (isButton(button_backcity)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//暂时注释，不需要提示玩家宴请信息
		/*
		if (m_bIsHint)
		{
		m_bIsHint = false;
		InformView::getInstance()->openInformView(this);
		InformView::getInstance()->openConfirmYes("TIP_PUP_EXIT_PUP");
		return;
		}*/
		button_callBack();
		return;
	}

	//进入v票商城
	if (isButton(button_confirm_yes)) 
	{
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		return;
	}
	if (isButton(button_s_no))
	{
		minorButtonChangeStatus(nullptr);
		return;
	}
	//女郎状态说明按钮
	if (isButton(button_bargirl_info))
	{
		openView(COMMOM_COCOS_RES[C_VIEW_DIALOG_INFO_CSB], 11);
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_DIALOG_INFO_CSB]);
		auto t_title = view->getChildByName<Text*>("label_title");
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_BARGIRL_STATUS_INFO_TITLE"]);
		auto l_content = view->getChildByName<ListView*>("listview_content");
		auto p_cont_1 = l_content->getItem(1);
		auto i_pic = p_cont_1->getChildByName<ImageView*>("image_pic");
		auto t_name = p_cont_1->getChildByName<Text*>("label_name");
		t_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_BARGIRL_STATUS_INFO_LEVEL1"]);
		auto t_content = p_cont_1->getChildByName<Text*>("label_content");
		t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_BARGIRL_STATUS_INFO_LEVEL1_DESC"]);
		auto wi = p_cont_1->getContentSize().width - i_pic->getBoundingBox().size.width - 20;
		t_content->setContentSize(Size(wi, getLabelHight(t_content->getString().c_str(), wi, t_content->getFontName(), t_content->getFontSize())));
		p_cont_1->setContentSize(Size(p_cont_1->getContentSize().width, p_cont_1->getBoundingBox().size.height));
		auto image_div_2 = l_content->getItem(2);

		for (int i = 2; i < 5; i++)
		{
			std::string nameStr = StringUtils::format("TIP_TARVEN_BARGIRL_STATUS_INFO_LEVEL%d", i);
			std::string descStr = StringUtils::format("TIP_TARVEN_BARGIRL_STATUS_INFO_LEVEL%d_DESC", i);
			auto item = p_cont_1->clone();
			auto i_pic = item->getChildByName<ImageView*>("image_pic");
			i_pic->ignoreContentAdaptWithSize(true);
			i_pic->loadTexture(StringUtils::format("cocosstudio/login_ui/common/info_bargirl_%d.jpg", i));
			i_pic->setPositionX(i_pic->getBoundingBox().size.width / 2);
			auto t_name = item->getChildByName<Text*>("label_name");
			t_name->setString(SINGLE_SHOP->getTipsInfo()[nameStr]);
			t_name->setPositionX(i_pic->getPositionX() + i_pic->getContentSize().width/2 + 20);
			auto t_content = item->getChildByName<Text*>("label_content");
			t_content->setString(SINGLE_SHOP->getTipsInfo()[descStr]);
			t_content->setPositionX(i_pic->getPositionX() + i_pic->getContentSize().width / 2 + 20);
			auto wi = item->getContentSize().width - i_pic->getBoundingBox().size.width - 20;
			t_content->setContentSize(Size(wi,getLabelHight(t_content->getString().c_str(), wi,t_content->getFontName(),t_content->getFontSize())));
			item->setContentSize(Size(item->getContentSize().width,item->getBoundingBox().size.height));
			i_pic->setPositionY(t_name->getPositionY() + (t_name->getBoundingBox().size.height/2 - i_pic->getBoundingBox().size.height/2));
			l_content->addChild(item);
			auto image_div = image_div_2->clone();
			l_content->addChild(image_div);
		}

		auto btn_close = view->getChildByName<Button*>("button_close");
		btn_close->setTouchEnabled(true);
		return;
	}
	//关闭女郎状态说明界面
	if (isButton(button_close))
	{
		closeView(COMMOM_COCOS_RES[C_VIEW_DIALOG_INFO_CSB]);
		return;
	}
}

void UITavern::buttonEvent(Widget *target,std::string name)
{
	//确认对话框
	if (isButton(button_s_yes)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		if (m_nConfirmIndex == CONFIRM_INDEX_BARGIRL_DRINK)
		{
			ProtocolThread::GetInstance()->treatBargirl(UILoadingIndicator::create(this));
			return;
		}

		if (m_nConfirmIndex == CONFIRM_INDEX_FOOD)
		{
			ProtocolThread::GetInstance()->barConsume(m_nBarIndex,1,UILoadingIndicator::create(this));
			return;
		}
		if (m_nConfirmIndex == CONFIRM_INDEX_DRINK)
		{
			ProtocolThread::GetInstance()->barConsume(m_nBarIndex,2,UILoadingIndicator::create(this));
			return;
		}
		if (m_nConfirmIndex == CONFIRM_INDEX_SAILOR_DRINK)
		{
			ProtocolThread::GetInstance()->barConsume(8,3,UILoadingIndicator::create(this));
			return;
		}
		return;
	}
	//刷新确定按钮
	if (isButton(button_v_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_nConfirmIndex == CONFIRM_INDEX_CAPTAIN_FRESH)
		{
			ProtocolThread::GetInstance()->refreshCaptainList(1,UILoadingIndicator::create(this));
			return;
		}
		if (m_nConfirmIndex == CONFIRM_INDEX_TASK_FRESH)
		{
			ProtocolThread::GetInstance()->getTaskList(1,UILoadingIndicator::create(this));
			return;
		}
		return;
	}
	//回主城
	if (isButton(button_backcity)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//暂时注释，不需要提示玩家宴请信息
		/*
		if (m_bIsHint)
		{
			m_bIsHint = false;
			InformView::getInstance()->openInformView(this);
			InformView::getInstance()->openConfirmYes("TIP_PUP_EXIT_PUP");
			return;
		}*/
		button_callBack();
		return;
	}
	//进入v票商城
	if (isButton(button_confirm_yes)) 
	{
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		return;
	}
	if (isButton(button_s_no))
	{
		minorButtonChangeStatus(nullptr);
		return;
	}
}
//右侧大主题的按钮相应
void UITavern::mainButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();

	if (m_bActionRunning)
	{
		return;
	}
	auto otherButton = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB])->getChildByName("panel_two_butter");
	button = otherButton->getChildByName<Button*>(name);
	auto b_chat2 = otherButton->getChildByName<Button*>("button_chat");

	//sound effect
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//聊天
	if(isButton(button_chat))
	{
		m_bChatButtonBackClick = true;
		if (m_nViewIndex != UI_CHAT)
		{
			if (m_bIsFriend)
			{
				m_bActionRunning = true;
				if (otherButton->getOpacity() == 0)
				{
					mainButtonMoveToRight();
					this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.5);
				}
				else
				{
					this->scheduleOnce(schedule_selector(UITavern::mainButtonTavern2CsbHide), 0);
					this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 1);
				}
				mainButtonChangeStatus(button);
				openBarGirlDialog(button);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				if (m_IsVillage)
				{
					UIInform::getInstance()->openConfirmYes("TIP_MAIN_VILLAGE_NOT_OPEN");
				}
				else
				{
					UIInform::getInstance()->openConfirmYes("TIP_PUP_NOT_FIREND");
				}
			
			}
		}
		return;
	}
	//酒水
	if(isButton(button_order))
	{
		if (m_nViewIndex != UI_ORDER)
		{
			if (m_bIsFriend)
			{
				m_bActionRunning = true;
				if (otherButton->getOpacity() == 0)
				{
					mainButtonMoveToRight();
					this->scheduleOnce(schedule_selector(UITavern::openOtherMainButton), 0.4);
					this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.9);
				}
				else
				{
					this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.5);
				}
				openFoodAndDrinkDialog(button);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				if (m_IsVillage)
				{
					UIInform::getInstance()->openConfirmYes("TIP_MAIN_VILLAGE_NOT_OPEN");
				}
				else
				{
					UIInform::getInstance()->openConfirmYes("TIP_PUP_NOT_FIREND");
				}

			}
		}
		return;
	}
	//雇佣水手
	if(isButton(button_recruit_sailor))
	{
		if (m_nViewIndex != UI_RECRUIT_SAILOR)
		{
			m_bActionRunning = true;
			if (otherButton->getOpacity() == 0)
			{
				mainButtonMoveToRight();
				this->scheduleOnce(schedule_selector(UITavern::openOtherMainButton), 0.4);
				this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.5);
			}
			else
			{
				this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.9);
			}
			openSailorDialog(button);
		}
		return;
	}
	//雇佣船长
	if(isButton(button_hire_captain))
	{
		if (m_nViewIndex != UI_HIRE_CAPTAIN)
		{
			if (m_bIsFriend)
			{
				m_bActionRunning = true;
				if (otherButton->getOpacity() == 0)
				{
					mainButtonMoveToRight();
					this->scheduleOnce(schedule_selector(UITavern::openOtherMainButton), 0.4);
					this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.5);
				}
				else
				{
					this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.9);
				}
				openCaptainDialog(button);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				if (m_IsVillage)
				{
					UIInform::getInstance()->openConfirmYes("TIP_MAIN_VILLAGE_NOT_OPEN");
				}
				else
				{
					UIInform::getInstance()->openConfirmYes("TIP_PUP_NOT_FIREND");
				}

			}
		}
		return;
	}
	//赏金榜
	if(isButton(button_bounty_board))
	{
		if (m_nViewIndex != UI_BOUNTY_BOARD)
		{
			if (m_bIsFriend)
			{
				m_bActionRunning = true;
				if (otherButton->getOpacity() == 0)
				{
					mainButtonMoveToRight();
					this->scheduleOnce(schedule_selector(UITavern::openOtherMainButton), 0.4);
					this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.5);
				}
				else
				{
					this->scheduleOnce(schedule_selector(UITavern::actionRunningEnd), 0.9);
				}
				openTaskDialog(button);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				if (m_IsVillage)
				{
					UIInform::getInstance()->openConfirmYes("TIP_MAIN_VILLAGE_NOT_OPEN");
				}
				else
				{
					UIInform::getInstance()->openConfirmYes("TIP_PUP_NOT_FIREND");
				}
			}
		}
		return;
	}
}
//聊天交互里的按钮相应
void UITavern::barGirlButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	//sound effect
	//请酒吧女郎喝酒
	if (isButton(button_drink))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		favorDegreeActionPlayed = false;
		//接受喝酒
		if (m_nCheers < 5) 
		{
			m_nConfirmIndex = CONFIRM_INDEX_BARGIRL_DRINK;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushSilverConfirmView("TIP_PUP_DRINK_CONFIRM_TITLE","TIP_PUP_DRINK_CONFIRM_CONTENT",m_pBarInfoResult->bargirldrinkcost);
		}
		//不接受喝酒（喝醉）
		else
		{
			m_nChatContentIndex = CONTENT_REFUSE_DRINK;
			this->scheduleOnce(schedule_selector(UITavern::showBarGirlChat),0);
		}
		return;
	}
	//送礼物
	if (isButton(button_gift)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		favorDegreeActionPlayed = false;
		ProtocolThread::GetInstance()->getPersonalItems(3,0,UILoadingIndicator::create(this));
		return;
	}
	//情报
	if (isButton(button_gossip))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->getIntelligence(UILoadingIndicator::create(this));
		return;
	}
	//返回
	if (isButton(button_back))
	{
		if (m_bChatButtonBackClick)
		{
			m_bChatButtonBackClick = false;
			m_nViewIndex = 0;
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			this->unschedule(schedule_selector(UITavern::showDialogWordText));
			mainButtonChangeStatus(nullptr);
			m_pBarGirlButton = nullptr;
			m_nChatContentIndex = CONTENT_WELCOME;
			minorButtonMoveToRight(0);
			this->scheduleOnce(schedule_selector(UITavern::showBarGirlChat), 0);
		}
		return;
	}
	//酒吧女郎(只在聊天界面可以点击)
	if (isButton(panel_bargirl_dialog))
	{
		//log("点击之前 m_nToTalDialogPage = %d m_nCurDialogPage = %d", m_nToTalDialogPage, m_nCurDialogPage);
		if (bargirlClickBoo == false)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			bargirlClickBoo = true;
			auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
			auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
			auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
			auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
			auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
			this->unschedule(schedule_selector(UITavern::showDialogWordText));
			if (i_anchor->getOpacity() == 255)
			{
				//提示符显示表示要翻页
				if (guardDiaolgOver == false)
				{
					//对话需翻页 对话继续后面部分
					m_BarGirlContentLabel->setString("");
					UITavern::showDialogWordSchedule();
					bargirlClickBoo = false;
					i_anchor->runAction(FadeOut::create(0.01));
					m_nCurDialogPage++;
				}
				else
				{
					lenNum = 0;
					m_nCurDialogPage++;
					if (m_nCurDialogPage <= m_nToTalDialogPage)
					{
						//对话未结束 需直接翻页显示内容
						m_BarGirlContentLabel->runAction(Sequence::create(FadeOut::create(0.5), Place::create(Vec2(m_BarGirlContentLabel->getPositionX(), m_nCurDialogPage * (m_BarGirlContentLabel->getSystemFontSize() * 3))), FadeIn::create(0.5), nullptr));
						if (m_nCurDialogPage < m_nToTalDialogPage)
						{
							//需提示翻页
							this->scheduleOnce(schedule_selector(UITavern::anchorAction), 0);
							bargirlClickBoo = false;
						}
						else
						{
							if (m_nCurDialogPage == m_nToTalDialogPage)
							{
								if (favorDegreeActionPlayed == false)
								{
									//与酒吧女郎友好度增加时的特效
									if (m_nChatContentIndex == CONTENT_ACCEPT_DRINK || m_nChatContentIndex == CONTENT_ACCEPT_GIFT)
									{
										UITavern::showFavorDegreeAction();
									}
								}
								i_anchor->runAction(FadeOut::create(0.01));
								if (m_nViewIndex == UI_CHAT)
								{
									favorDegreeActionPlayed = false;
									bargirlClickBoo = true;
								}
							}
						}
					}
				}
			}
			else
			{
				//提示符不显示 点击表示不逐字显示 直接显示内容
				guardDiaolgOver = true;
				m_BarGirlContentLabel->setString(chatContent);
				m_BarGirlContentLabel->stopAllActions();
				m_BarGirlContentLabel->setOpacity(255);
				wordLenBoo = false;
				lenNum = 0;
				m_nCurDialogPage = 1;
				if (m_nToTalDialogPage > 1)
				{
					//需提示翻页
					this->scheduleOnce(schedule_selector(UITavern::anchorAction), 0);
					bargirlClickBoo = false;
				}
				else
				{
					if (favorDegreeActionPlayed == false)
					{
						//与酒吧女郎友好度增加时的特效
						if (m_nChatContentIndex == CONTENT_ACCEPT_DRINK || m_nChatContentIndex == CONTENT_ACCEPT_GIFT)
						{
							UITavern::showFavorDegreeAction();
						}
					}
					if (m_nViewIndex == UI_CHAT)
					{
						favorDegreeActionPlayed = false;
						bargirlClickBoo = true;
					}
				}
			}
		}
		//log("m_nToTalDialogPage = %d m_nCurDialogPage = %d", m_nToTalDialogPage, m_nCurDialogPage);
		return;
	}
	//聊天送礼界面的礼物
	if (isButton(button_item))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pGiftButton)
		{
			m_pGiftButton->setBright(true);
		}
		m_pGiftButton = button;
		m_pGiftButton->setBright(false);
		return;
	}
	//点击礼物，赠送礼物
	if (isButton(button_fire_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pGiftButton)
		{
			ProtocolThread::GetInstance()->giveBargirlGift(m_pGiftButton->getTag(), m_pGiftResult->itemtype, UILoadingIndicator::create(this));
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_PUP_GIFT_CHOOSE");
		}
		return;
	}
	//关闭送礼物界面
	if (isButton(button_fire_no))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		minorButtonChangeStatus(nullptr);
		m_pGiftButton = nullptr;
		closeView();
		return;
	}
}
//酒水交互里的按钮相应
void UITavern::foodAndDrinkButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	//sound effect
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//点击酒水
	if (isButton(button_item)) 
	{
		//水手数为0时
		if (m_nCurSailorNum == 0) 
		{
			openSuccessOrFailDialog("TIP_PUP_NOT_SAILOR");
			return;
		}
		int cost = 0;
		//食物
		if (button->getTag() < START_INDEX) 
		{
			m_nBarIndex = m_pBarInfoResult->foodmenu[button->getTag()]->id;
			cost = m_pBarInfoResult->foodmenu[button->getTag()]->price * m_nCurSailorNum;
			m_nConfirmIndex = CONFIRM_INDEX_FOOD;
		}
		//酒水
		else 
		{
			m_nBarIndex = m_pBarInfoResult->drinkmenu[button->getTag()-START_INDEX]->id;
			cost = m_pBarInfoResult->drinkmenu[button->getTag()-START_INDEX]->price * m_nCurSailorNum;
			m_nConfirmIndex = CONFIRM_INDEX_DRINK;
		}
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushSilverConfirmView("TIP_PUP_FOODANDDRINK_TITLE","TIP_PUP_FOODANDDRINK_CONTENT",cost);
		return;
	}
	//关闭士气提示界面
	if (isButton(button_yes))
	{
		closeView();
		return;
	}
}
//水手交互里的按钮相应
void UITavern::sailorButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();

	//雇佣水手确认
	if (isButton(button_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		UIVoyageSlider *ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
		ProtocolThread::GetInstance()->getCrew(ss->getCurrentNumber(),UILoadingIndicator::create(this));
		return;
		//退出宴请时的副官对话
	}
	else if (isButton(panel_bargirl_dialog))
	{
		if (m_nChatContentIndex == CONFIRM_INDEX_CHIEF_CHAT)
		{
			return;
		}
		if (m_bChiefClick == false)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			m_bChiefClick = true;
			closeBarChiefChat();
			auto p_button = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB])->getChildByName("panel_two_butter");
			p_button->setVisible(true);
			m_nViewIndex = 0;
			openSailorDialog(nullptr);
		}
		return;
	}
	else
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//宴请
		if (isButton(button_chat_1))  
		{
			//村庄初级不实现宴请
			if (m_IsVillage)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_MAIN_VILLAGE_NOT_OPEN");
				return;
			}
			//再次宴请时提示
			if (m_bIsBanquet) 
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_NOT_FETE");
			}
			else
			{
				m_nConfirmIndex = CONFIRM_INDEX_SAILOR_DRINK;
				ProtocolThread::GetInstance()->getAvailCrewNum(UILoadingIndicator::create(this));
			}
			return;
		}
		//雇佣水手
		if (isButton(button_chat_2)) 
		{
			m_nConfirmIndex = CONFIRM_INDEX_NOT_DRINK;
			ProtocolThread::GetInstance()->getAvailCrewNum(UILoadingIndicator::create(this));
			return;
		}
		//解雇水手
		if (isButton(button_chat_3))
		{
			if (m_nCurSailorNum<=0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_TARVEN_FIRE_SAILORS_TOO_LITTLE");
			}
			else
			{
				initDismissSailors();
			}			
			return;
		}
		//雇佣水手取消
		if (isButton(button_cancel))
		{
			closeView();
			return;
		}
		//雇佣水手结果界面按键
		if (isButton(button_result_yes))
		{
			closeView();
			if (m_pAvailCrewNumResult->currentcrewnum == m_pAvailCrewNumResult->maxcrewnum)
			{
				openSuccessOrFailDialog("TIP_PUP_SHIP_NOT_CAP");
				return;
			}
			ProtocolThread::GetInstance()->getAvailCrewNum(UILoadingIndicator::create(this));
			return;
		}
		//雇佣水手界面花费解释
		if (isButton(button_income_info))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushInfoBtnView("TIP_PUP_SAILORS_INCOME_TITLE","TIP_PUP_SAILORS_INCOME_CONTENT");
			return;
		}
		//雇佣水手界面最大航行天数解释
		if (isButton(button_maxday_info))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushInfoBtnView("TIP_PUP_SAILORS_MAXDAY_TITLE","TIP_PUP_SAILORS_MAXDAY_CONTENT");
			return;
		}
		//水手数没有达到最低要求时的解释
		if (isButton(button_caveat))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushWarning(n_sailorNotEnoughTag);
			return;
		}
	}
}
//船长交互里的按钮相应
void UITavern::captainButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	
	//船长商店确定雇佣按钮
	if (isButton(button_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		ProtocolThread::GetInstance()->getCaptain(m_pCaptainButton->getTag(),UILoadingIndicator::create(this));
		return;
	}
	else
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//雇佣船长（去船长商店的按钮）
		if (isButton(button_hire)) 
		{
			flushShopCaptains();
			auto b_ok = getViewRoot(TARVEN_COCOS_RES[TARVEN_HIRE_DIALOG_CSB])->getChildByName<Button*>("button_ok");
			b_ok->setBright(false);
			b_ok->setTouchEnabled(false);	
			return;
		}
		//雇佣船长船长商店界面取消按钮
		if (isButton(button_cancel))
		{
			if (m_pCaptainButton)
			{
				m_pCaptainButton = nullptr;
			}
			closeView();
			return;
		}
		//刷新船船长列表
		if (isButton(button_captain_v))
		{
			//刷新船长列表
			if (m_pBarInfoResult->refreshcaptaincost <= SINGLE_HERO->m_iGold) 
			{
				m_nConfirmIndex = CONFIRM_INDEX_CAPTAIN_FRESH;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushVConfirmView("TIP_PUP_FLUSH_CAPTAIN_TITLE", "TIP_PUP_FLUSH_CAPTAIN_CONTENT", m_pBarInfoResult->refreshcaptaincost);
			}
			//v 票不足
			else 
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(m_pBarInfoResult->refreshcaptaincost);
			}
			return;
		}
		//关闭雇佣船长成功提示按钮
		if (isButton(button_captain_yes))
		{
			closeView();
			return;
		}
		//选择商店里的船长
		if (isButton(panel_captain))
		{
			if (m_pCaptainButton)
			{
				m_pCaptainButton->getChildByName("image_press")->setVisible(false);
			}
			auto b_ok = getViewRoot(TARVEN_COCOS_RES[TARVEN_HIRE_DIALOG_CSB])->getChildByName<Button*>("button_ok");
			b_ok->setBright(true);
			b_ok->setTouchEnabled(true);
			m_pCaptainButton = button;
			m_pCaptainButton->getChildByName("image_press")->setVisible(true);
			return;
		}
		//解雇船长确认
		if (isButton(button_fire_yes))
		{
			ProtocolThread::GetInstance()->fireCaptain(button->getTag(),UILoadingIndicator::create(this));
			return;
		}
		//取消解雇船长
		if (isButton(button_fire_no))
		{
			closeView();
			return;
		}
		//解雇船长界面
		if (isButton(button_fire))
		{
			openFireCaptainsConfirm(button->getTag());	
			return;
		}
		//我的船长技能详情
		if (isButton(my_captain_image_skill))
		{
			UICommon::getInstance()->openCommonView(this);
			auto captainInfo = m_vMyCaptains.at(button->getParent()->getTag());
			SKILL_DEFINE skillDefine;
			skillDefine.id = captainInfo->skills[button->getTag() - 1]->id;
			skillDefine.lv = captainInfo->skills[button->getTag() - 1]->level;
			skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
			skillDefine.icon_id = captainInfo->id;
			UICommon::getInstance()->flushSkillView(skillDefine);
			return;
		}
		//商店船长技能详情
		if (isButton(shop_captain_image_skill))
		{
			UICommon::getInstance()->openCommonView(this);
			auto captainInfo = m_vShopCaptains.at(button->getParent()->getTag());
			SKILL_DEFINE skillDefine;
			skillDefine.id = captainInfo->skills[button->getTag() - 1]->id;
			skillDefine.lv = captainInfo->skills[button->getTag() - 1]->level;
			skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
			skillDefine.icon_id = captainInfo->id;
			UICommon::getInstance()->flushSkillView(skillDefine);
			return;
		}
	}
	
}
//赏金榜交互里的按钮相应
void UITavern::taskButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	//sound effect
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//任务刷新按钮
	if (isButton(button_repair_v)) 
	{
		//任务刷新
		if (m_pBarInfoResult->refreshtaskcost <= SINGLE_HERO->m_iGold)
		{
			m_nConfirmIndex = CONFIRM_INDEX_TASK_FRESH;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushVConfirmView("TIP_PUPLAYER_FLUSH_TASKLIST_TITLE", "TIP_PUPLAYER_FLUSH_TASKLIST_CONTENT", m_pBarInfoResult->refreshtaskcost);
		}
		//v 票不足
		else 
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(m_pBarInfoResult->refreshtaskcost);
		}
		
		return;
	}
	//接受任务按钮
	if (isButton(button_accept))
	{
		if (m_nTaskFalg)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_NOT_ACCEPT_TASK");
		}else
		{
			ProtocolThread::GetInstance()->handleTask(button->getTag() - START_INDEX, 0, UILoadingIndicator::create(this));
		}
		return;
	}
	//放弃任务按钮
	if (isButton(button_abort))
	{
		openTaskConfirmDialog(button->getTag()-START_INDEX,false);
		return;
	}
	//领取奖励按钮
	if (isButton(button_get_reward))
	{
		if (m_pCurTaskDefine->completecityid == SINGLE_HERO->m_iCityID || m_nFavour >= FAVOUR_SIDE_TASK)
		{
			ProtocolThread::GetInstance()->handleTask(button->getTag()-START_INDEX,2,UILoadingIndicator::create(this));
		}else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_GET_REWARD_NOTCITY");
		}
		return;
	}
	//放弃任务确定按钮
	if (isButton(button_s_yes))
	{
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
		auto w_taskConfirm = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
		auto b_no = w_taskConfirm->getChildByName<Button*>("button_s_no");
		b_no->setTouchEnabled(false);
		ProtocolThread::GetInstance()->handleTask(button->getTag()-20*START_INDEX,3,UILoadingIndicator::create(this));
		return;
	}
	//放弃任务取消按钮
	if (isButton(button_s_no))
	{
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
		auto w_taskConfirm = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
		auto b_yes = w_taskConfirm->getChildByName<Button*>("button_s_yes");
		b_yes->setTouchEnabled(false);
		closeView();
		return;
	}
	//领取奖励后是否有升级界面
	if (isButton(button_result_yes))
	{
		closeView();
		UICommon::getInstance()->closeView();
		if (m_bIsLevelUp)
		{
			m_bIsLevelUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPlayerLevelUp();
			return;
		}
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
			return;
		}
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp();
			UICommon::getInstance()->flushCaptainLevelUp(m_pHandleTaskResult->n_captains, m_pHandleTaskResult->captains);
			return;
		}
		return;
	}
	//奖励物品的详情
	if (isButton(button_items_1))
	{
		int taskId = m_pTaskButton->getTag()-START_INDEX;
		int taskCoreId = getCoreTaskIdFromTaskId(taskId);
		int type = SINGLE_SHOP->getCoreTaskById(taskCoreId).rewards_item1_type;
		int id = SINGLE_SHOP->getCoreTaskById(taskCoreId).rewards_item1_id;
		openGoodInfo(nullptr,type,id);
		return;
	}
	//奖励物品的详情
	if (isButton(button_items_2))
	{
		int taskId = m_pTaskButton->getTag()-START_INDEX;
		int taskCoreId = getCoreTaskIdFromTaskId(taskId);
		int type = SINGLE_SHOP->getCoreTaskById(taskCoreId).rewards_item2_type;
		int id = SINGLE_SHOP->getCoreTaskById(taskCoreId).rewards_item2_id;
		openGoodInfo(nullptr,type,id);
		return;
	}
	//技能的详情
	if (isButton(image_skill_bg))
	{
		UICommon::getInstance()->openCommonView(this);
		auto index = button->getTag();
		UICommon::getInstance()->flushSkillView(m_vSkillDefine.at(index));
		return;
	}
}
//赏金榜任务列表的任务切换
void UITavern::taskEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	if (!button->isBright())
	{
		return;
	}
	if (m_pTaskButton != nullptr)
	{
		m_pTaskButton->setBright(true);
		m_pTaskButton->setTouchEnabled(true);
	}
	m_pTaskButton = button;
	m_pTaskButton->setBright(false);
	m_pTaskButton->setTouchEnabled(false);
	int taskId = m_pTaskButton->getTag()-START_INDEX;
	int taskCoreId = getCoreTaskIdFromTaskId(taskId);
	if (taskCoreId > 0)
	{
		showTaskInfo(taskId,taskCoreId,getNpcIdxFromTaskId(taskId));
	}
}

int UITavern::getCoreTaskIdFromTaskId(const int nTaskId)
{
	if (m_pCurTaskDefine && m_pCurTaskDefine->taskid == nTaskId)
	{
		return m_pCurTaskDefine->taskcoreid;
	}
	int nTaskCoreId = 0;
	for (auto i = 0; i < m_vTaskIds.size(); i++)
	{
		if (m_vTaskIds[i] == nTaskId)
		{
			nTaskCoreId = m_vCoreTaskIds[i];
			break;
		}
	}
	return nTaskCoreId;
}

int UITavern::getNpcIdxFromTaskId(const int nTaskId)
{
	if (m_pCurTaskDefine && m_pCurTaskDefine->taskid == nTaskId)
	{
		return m_pCurTaskDefine->npcindex;
	}	
	int nTaskCoreId = 0;
	for (auto i = 0; i < m_vTaskIds.size(); i++)
	{
		if (m_vTaskIds[i] == nTaskId)
		{
			nTaskCoreId = m_vNpcIdx[i];
			break;
		}
	}
	return nTaskCoreId;
}

int UITavern::getCityIdxFromTaskId(const int nTaskId)
{
	if (m_pCurTaskDefine && m_pCurTaskDefine->taskid == nTaskId)
	{
		return m_pCurTaskDefine->targetcity;
	}
	int nTaskCoreId = 0;
	for (auto i = 0; i < m_vTaskIds.size(); i++)
	{
		if (m_vTaskIds[i] == nTaskId)
		{
			nTaskCoreId = m_vCityIdx[i];
			break;
		}
	}
	return nTaskCoreId;
}
//接收服务器信息
void UITavern::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	//获取酒吧信息.
	case PROTO_TYPE_GetBarInfoResult:
		{
			m_pBarInfoResult  = (GetBarInfoResult *)message;
			if (m_pBarInfoResult->failed == 0)
			{
				if(m_pBarInfoResult->bartask){
					m_lastTaskid = m_pBarInfoResult->bartask->taskid;
				}
				//进入村庄酒吧
				if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
				{
					m_IsVillage = true;
				}
				else
				{
					m_IsVillage = false;
				}
				m_nCheers = m_pBarInfoResult->drinktimes;
				initResult();
			}else
			{
				ProtocolThread::GetInstance()->getBarInfo(0,UILoadingIndicator::create(this));
			}
			break;
		}
	//获取可以卖出的物品. itemType : 物品类型
	case PROTO_TYPE_GetPersonalItemResult:
		{
			m_pGiftResult = (GetPersonalItemResult *)message;
			if (m_pGiftResult->failed == 0)
			{
				openGiftView();
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_GET_ROLL_EQUIP_FAIL");
			}
			break;
		}
	//酒吧消费操作.
	case PROTO_TYPE_BarConsumeResult:
		{
			BarConsumeResult* result = (BarConsumeResult *)message;
			switch (result->failed)
			{
			case 0:
			{
					  updateCoin(result->coin, result->gold);
					  if (result->actioncode == 3)
					  {
						  auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
						  m_bIsBanquet = true;
						  m_bIsHint = true;
						  Helper::seekWidgetByName(view, "image_silver")->setVisible(false);
						  Helper::seekWidgetByName(view, "label_silver_num")->setVisible(false);
						  Helper::seekWidgetByName(view, "label_silver_num_0")->setVisible(true);
						  auto b_chat2 = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_chat_2"));
						  b_chat2->getChildByName<Text*>("label_content")->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT3"]);
						  view->runAction(MoveTo::create(0.4f, ENDPOS3));
						  m_nChatContentIndex = CONFIRM_INDEX_CHIEF_CHAT;
						  auto p_button = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB])->getChildByName("panel_two_butter");
						  p_button->setVisible(false);
						  this->scheduleOnce(schedule_selector(UITavern::showBarChiefChat), 0.4);
					  }
					  else
					  {
						  //吃酒水
						  openView(TARVEN_COCOS_RES[TARVEN_ORDER_RESULT_CSB], 11);
						  m_pRoot->getChildByName<Button*>("button_yes")->addTouchEventListener(CC_CALLBACK_2(UITavern::foodAndDrinkButtonEvent,this));
					  }
					  break;
			}
			case COIN_NOT_FAIL:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
					  break;
			}
			default:
			{
					   UIInform::getInstance()->openInformView(this);
					   UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
					   break;
			}
			}
			break;
		}
	//和酒吧女郎聊天 请她喝一杯
	case PROTO_TYPE_TreatBargirlResult:
		{
			TreatBargirlResult* result = (TreatBargirlResult *)message;
			switch (result->failed)
			{
			case 0:
			{
					  updateCoin(result->coin, result->gold);
					  m_nFavour = result->prestige;
					  m_nCheers++;
					  m_nChatContentIndex = CONTENT_ACCEPT_DRINK;
					  this->scheduleOnce(schedule_selector(UITavern::showBarGirlChat), 0);
					  break;
			}
			case COIN_NOT_FAIL:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
					  break;
			}
			case 13:
			{
					   //喝酒次数 多于5次时提示，（在出城后重置）
					  m_nChatContentIndex = CONTENT_REFUSE_DRINK;
					  this->scheduleOnce(schedule_selector(UITavern::showBarGirlChat), 0);
					  break;
			}
			default:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_DRINK_GIRL_FAIL");
					  break;
			}
			}
			break;
		}
	//获取可以招募的水手
	case PROTO_TYPE_GetAvailCrewNumResult:
		{
			GetAvailCrewNumResult* result = (GetAvailCrewNumResult *)message;
			switch (result->failed)
			{
			case 0:
			{
					  if (result->currentcrewnum == result->maxcrewnum)
					  {
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openConfirmYes("TIP_PUP_CREW_FULL");
					  }
					  else
					  {
						  //no offer drink
						  if (m_nConfirmIndex == CONFIRM_INDEX_NOT_DRINK)
						  {
							  m_pAvailCrewNumResult = result;
							  if (m_pAvailCrewNumResult->maxcrewnum <= m_pAvailCrewNumResult->currentcrewnum)
							  {
								  //crew full
								  UIInform::getInstance()->openInformView(this);
								  UIInform::getInstance()->openConfirmYes("TIP_PUP_SHIP_NOT_CAP");
							  }
							  else
							  {
								  openSailorConfirm(result);
							  }
						  }
						  else
						  {
							  m_pAvailCrewNumResult = result;
							  if (m_pAvailCrewNumResult->maxcrewnum <= m_pAvailCrewNumResult->currentcrewnum)
							  {
								  //crew full
								  UIInform::getInstance()->openInformView(this);
								  UIInform::getInstance()->openConfirmYes("TIP_PUP_SHIP_NOT_CAP");
							  }
							  else
							  {
								  if (!m_bIsBanquet)
								  {
									  UICommon::getInstance()->openCommonView(this);
									  UICommon::getInstance()->flushSilverConfirmView("TIP_PUP_FOODANDDRINK_TITLE", "TIP_PUP_FOODANDDRINK_CONTENT", m_pBarInfoResult->treatforgetcrewcost);
								  }
								  else
								  {
									  openSailorConfirm(result);
								  }
							  }
						  }
					  }
					  break;
			}
			case COIN_NOT_FAIL:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
					  break;
			}
			case 3:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_HAVENOSHIP_HIRE_CREWS");
					  break;
			}
			default:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_CREW_FULL");
					  break;
			}
			}
			break;
		}
	//招募水手. count : 招募数量
	case PROTO_TYPE_GetCrewResult:
		{
			GetCrewResult *result = (GetCrewResult *)message;
			switch (result->failed)
			{
			case 0:
			{
					  m_nCurSailorNum += result->crewnum;
					  int sailorNum = 0;
					  for (int i = 0; i < result->n_shipcrew; i++)
					  {
						  sailorNum += result->shipcrew[i]->crewmax;
					  }

					  if (m_nCurSailorNum == sailorNum)
					  {
						  m_bIsHint = false;
					  }
					  closeView();
					  auto tips = SINGLE_SHOP->getTipsInfo();
					  openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB], 11);
					  auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
					  view->getChildByName<Text*>("label_title")->setString(tips["TIP_PUP_SAILOR_RECRUIT_SUCCESS"]);
					  auto p_result = view->getChildByName<Widget*>("panel_result");
					  auto l_result = view->getChildByName<ListView*>("listview_result");
					  auto image_div = p_result->getChildByName<ImageView*>("image_div_1");
					  auto p_item = p_result->getChildByName<Widget*>("panel_silver_2");
					  auto p_sailor = p_item->clone();
					  p_sailor->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_TITLE"]);
					  p_sailor->getChildByName<Text*>("label_buy_num")->setString(StringUtils::format("%d", result->crewnum));
					  p_sailor->getChildByName<Text*>("label_buy_num")->setTextColor(Color4B(40, 25, 13, 255));
					  p_sailor->getChildByName<ImageView*>("image_silver")->loadTexture("cocosstudio/login_ui/common/sailor_2.png");
					  p_sailor->getChildByName<ImageView*>("image_silver")->setPositionX(p_sailor->getChildByName<Text*>("label_buy_num")->getPositionX() - p_sailor->getChildByName<Text*>("label_buy_num")->getBoundingBox().size.width
						  - p_sailor->getChildByName<ImageView*>("image_silver")->getBoundingBox().size.width / 2 - 5);
					  l_result->pushBackCustomItem(p_sailor);
					  auto image_div_1 = image_div->clone();
					  l_result->pushBackCustomItem(image_div_1);

					  auto p_cost = p_item->clone();
					  p_cost->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_FEE"]);
					  p_cost->getChildByName<Text*>("label_buy_num")->setString(numSegment(StringUtils::format("%d", m_pBarInfoResult->hirecrewfee)));
					  p_cost->getChildByName<Text*>("label_buy_num")->setTextColor(Color4B(40, 25, 13, 255));
					  p_cost->getChildByName<ImageView*>("image_silver")->loadTexture("cocosstudio/login_ui/common/silver.png");
					  p_cost->getChildByName<ImageView*>("image_silver")->setPositionX(p_cost->getChildByName<Text*>("label_buy_num")->getPositionX() - p_cost->getChildByName<Text*>("label_buy_num")->getBoundingBox().size.width
						  - p_cost->getChildByName<ImageView*>("image_silver")->getBoundingBox().size.width / 2 - 5);
					  l_result->pushBackCustomItem(p_cost);
					  auto image_div_2 = image_div->clone();
					  l_result->pushBackCustomItem(image_div_2);

					  auto p_fee = p_item->clone();
					  p_fee->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_COST"]);
					  p_fee->getChildByName<Text*>("label_buy_num")->setString(numSegment(StringUtils::format("%d", result->cost)));
					  p_fee->getChildByName<Text*>("label_buy_num")->setTextColor(Color4B(40, 25, 13, 255));
					  p_fee->getChildByName<ImageView*>("image_silver")->loadTexture("cocosstudio/login_ui/common/silver.png");
					  p_fee->getChildByName<ImageView*>("image_silver")->setPositionX(p_fee->getChildByName<Text*>("label_buy_num")->getPositionX() - p_fee->getChildByName<Text*>("label_buy_num")->getBoundingBox().size.width
						  - p_fee->getChildByName<ImageView*>("image_silver")->getBoundingBox().size.width / 2 - 5);
					  l_result->pushBackCustomItem(p_fee);
					  auto image_div_3 = image_div->clone();
					  l_result->pushBackCustomItem(image_div_3);

					  view->getChildByName<Button*>("button_result_yes")->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent,this));
					  updateCoin(result->coin, result->gold);
					  break;
			}
			case 1:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_SHIP_NOT_CAP");
					  break;
			}
			case COIN_NOT_FAIL:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
					  break;
			}
			default:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CREW_FAIL");
					  break;
			}
			}
			break;
		}
	//赠送礼物给酒吧女郎
	case PROTO_TYPE_GiveBargirlGiftResult:
		{
			GiveBargirlGiftResult *result = (GiveBargirlGiftResult *)message;
			if (result->failed == 0)
			{
				closeView();
				m_pGiftButton = nullptr;
				m_nFavour = result->prestige;
				if (result->addprestige == 10)
				{
					m_nlikeRecievedGift = 1;
				}
				m_nChatContentIndex = CONTENT_ACCEPT_GIFT;
				showBarGirlChat(0);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_SEND_GIFT_FAIL");
			}
			break;
		}
	//获取酒馆任务信息. refresh : 刷新可以接的任务的列表(1为刷新任务列表)
	case PROTO_TYPE_GetTaskListResult:
		{
			GetTaskListResult *result = (GetTaskListResult *)message;
			switch (result->failed)
			{
			case 0:
			{
						if(result->currunttask){
							m_lastTaskid = result->currunttask->taskid;
						}
					  m_pCurTaskDefine = result->currunttask;
					  m_vTaskIds.clear();
					  m_vCoreTaskIds.clear();
					  m_vNpcIdx.clear();
					  m_vCityIdx.clear();
					  m_BonusMap.clear();
					  int size = 0;
					  int*ids = get_nozero_int_array_from_string(result->taskids, ',', &size);
					  int size1 = 0;
					  int*ids_core = get_nozero_int_array_from_string(result->coretaskids, ',', &size1);
					  int npcNum = 0;
					  int*npcIds = get_int_array_from_string(result->npcindexstr, ',', &npcNum);
					  int cityNum = 0;
					  int*cityIds = get_int_array_from_string(result->targetcitystr, ',', &cityNum);
					  int bonusNum = 0;
					  int*bonusIds = get_int_array_from_string(result->bonusstr, ',', &bonusNum);

					  if (size > 0 && size == size1)
					  {
						  for (int i = 0; i < size; i++)
						  {
							  m_vTaskIds.push_back(ids[i]);
							  m_vCoreTaskIds.push_back(ids_core[i]);
							  m_vNpcIdx.push_back(npcIds[i]);
							  m_vCityIdx.push_back(cityIds[i]);
							  m_BonusMap[ids_core[i]] = bonusNum>i ? bonusIds[i] : 0;
						  }
						  delete[]ids;
						  ids = 0;
						  ids_core = 0;
					  }
					  if (bonusIds) delete[] bonusIds;
					  if (ids) delete[] ids;
					  if (ids_core) delete[] ids_core;

					  int len = 100;
					  int *npcId = new int[len];
					  npcId[0] = 0;
					  npcNum = 1;
					  for (size_t i = 0; i < m_vTaskIds.size(); i++)
					  {
						  auto coreId = getCoreTaskIdFromTaskId(m_vTaskIds[i]);
						  if (coreId)
						  {
							  auto type = SINGLE_SHOP->getCoreTaskById(coreId).type;
							  if (type == 1)
							  {
								  npcId[npcNum] = SINGLE_SHOP->getCoreTaskById(coreId).finish_target_npcId[m_vNpcIdx[i]];
								  npcNum++;
							  }
						  }
					  }
					  ProtocolThread::GetInstance()->getNpcScore(npcId, npcNum, UILoadingIndicator::create(this));
					  if (npcId)
					  {
						  delete[] npcId;
					  }

					  flushTaskList();
					  m_nTaskFlushTime = result->surplusrefreshtime;
					  updateCoin(result->coin, result->gold);
					  if (result->cost > 0)
					  {
						  Utils::consumeVTicket("8", 1, result->cost);
					  }
					  break;
			}
			case GOLD_NOT_FAIL:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openGoldNotEnoughYesOrNo(result->gold);

					  break;
			}
			default:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_FLUSH_TASK_FAIL");
					  break;
			}
			}
			break;
		}
	//情报
	case PROTO_TYPE_GetIntelligenceResult:
		{
			GetIntelligenceResult *result = (GetIntelligenceResult *)message;
			switch (result->failed)
			{
			case 0:
			{
					  m_pIntelligenceResult = result;
					  m_nChatContentIndex = CONTENT_INFO_RMATION;
					  showBarGirlChat(0);
					  break;
			}
			case 2:
			{
					  m_nChatContentIndex = CONTENT_NO_INTELLIGENCE;
					  showBarGirlChat(0);
					  break;
			}
			default:
			{
					   UIInform::getInstance()->openInformView(this);
					   UIInform::getInstance()->openConfirmYes("TIP_PUP_GET_INTELLIGENCE_FAIL");
					   break;
			}
			}
			break;
		}
	//雇佣船长. index : 选择雇佣的序号
	case PROTO_TYPE_GetCaptainResult:
		{
			GetCaptainResult* result = (GetCaptainResult*)message;
			switch (result->failed)
			{
			case 0:
			{
					  m_pCaptainButton = nullptr;
					  closeView();
					  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
					  updateCoin(result->coin, result->gold);
					  addMyCaptains(result);
					  openView(COMMOM_COCOS_RES[C_VIEW_HIRE_RESULT_CSB]);
					  auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_HIRE_RESULT_CSB]);
					  auto costNum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_num"));
					  costNum->ignoreContentAdaptWithSize(true);
					  costNum->setString(numSegment(StringUtils::format("%d", result->cost)));
					  auto i_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver_1"));
					  i_silver->setPositionX(costNum->getPositionX() - costNum->getBoundingBox().size.width - i_silver->getContentSize().width / 2 - 5);
					  view->getChildByName<Button*>("button_captain_yes")->addTouchEventListener(CC_CALLBACK_2(UITavern::captainButtonEvent,this));
					  break;
			}
			case 1:
			{
					  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CAPATAIN_FAIL");
					  break;
			}
			case COIN_NOT_FAIL:
			{
					  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
					  break;
			}
			default:
			{
					  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CAPATAIN_FAIL");
					  break;
			}
			}
			break;
		}
	//对任务进行的操作. id : 任务id, actionCode : 操作方式(0:get task, 1:submit task,2 get reward,3 give up task)
	case PROTO_TYPE_HandleTaskResult:
		{
			HandleTaskResult* result = (HandleTaskResult*) message;
			switch (result->failed)
			{
			case 0:
			{
					  triggerDialogId = result->dialogid;
					  ProtocolThread::GetInstance()->getTaskList(0, UILoadingIndicator::create(this));
					  switch (result->actioncode)
					  {
					  case 0:
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openViewAutoClose("TIP_PUP_GET_TASK_SUCCESS");
						  if (triggerDialogId)
						  {
							  auto viewTarven = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
							  auto p_tavern = viewTarven->getChildByName<Widget*>("panel_tavern");
							  auto p_bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
							  auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
							  auto viewQuest = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
							  addNeedControlWidgetForHideOrShow(p_tavern, true);
							  addNeedControlWidgetForHideOrShow(p_bargirl_dialog, false);
							  addNeedControlWidgetForHideOrShow(view, false);
							  addNeedControlWidgetForHideOrShow(viewQuest, false);
							  SINGLE_HERO->m_nTriggerId = triggerDialogId;
							  allTaskInfo(false, SMALL_STORY);
							  notifyUIPartDelayTime(0.5);
						  }
						  //客户端统计玩家任务行为
						  if(result->task){
							  Utils::onTaskBegin(StringUtils::format("side_task_%d",result->task->taskid));
						  }
						  break;
					  case 1:
						  closeView();
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openViewAutoClose("TIP_PUP_GET_REWARD_SUCCESS");
						  break;
					  case 2:
						  m_pHandleTaskResult = result;
						  SINGLE_HERO->m_iExp = result->exp;
						  SINGLE_HERO->m_iRexp = result->fame;
						  if (result->newlevel)
						  {
							  m_bIsLevelUp = true;
							  SINGLE_HERO->m_iLevel = result->newlevel;
						  }
						  if (result->new_fame_level)
						  {
							  m_bIsPrestigeUp = true;
							  SINGLE_HERO->m_iPrestigeLv = result->new_fame_level;
						  }
						  if (result->n_captains > 0)
						  {
							  //有船长增加的经验大于0是才显示界面
							  for (int i = 0; i< result->n_captains; i++)
							  {
								  if (result->captains[i]->lootexp > 0)
								  {
									  m_bIsCaptainUp = true;
									  break;
								  }
								  else
								  {
									  m_bIsCaptainUp = false;
								  }
							  }
						  }
						  openGetRewardDialog(result);
						  //客户端统计玩家任务行为
						  //Utils::onTaskCompleted(StringUtils::format("side_task_%d", result->task->taskid));
						  Utils::onTaskCompleted(StringUtils::format("side_task_%d", m_lastTaskid));
						  m_lastTaskid = 0;

						  break;
					  default:
						  closeView();
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openViewAutoClose("TIP_PUPLAYER_GIVEUP_TASK_SUCCESS");
						  //客户端统计玩家任务行为
						  if(m_pCurTaskDefine){
							  Utils::onTaskFailed(StringUtils::format("side_task_%d", m_pCurTaskDefine->taskid), "quit");
						  }
						  break;
					  }
					  break;
			}
			case 103:
			{
					  //not cargo
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_BAG_FULL");
					  break;
			}
			default:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
					  break;
			}
			}
			break;
		}
	//刷新酒馆可以雇佣的船长列表. useGold : 花费Ｖ票数量
	case PROTO_TYPE_RefreshCaptainListResult:
		{
			RefreshCaptainListResult* result = (RefreshCaptainListResult*)message;
			switch (result->failed)
			{
			case 0:
			{
					  if (m_pCaptainButton)
					  {
						  m_pCaptainButton = nullptr;
					  }
					  m_nCaptainFlushTime = result->distocaptainfinish;
					  m_vShopCaptains.clear();
					  for (int i = 0; i < result->n_captains; i++)
					  {
						  m_vShopCaptains.push_back(result->captains[i]);
					  }
					  flushShopCaptains();
					  updateCoin(result->coins, result->gold);
					  if (result->costgold > 0)
					  {
						  Utils::consumeVTicket("9", 1, result->costgold);
					  }
					  break;
			}
			case GOLD_NOT_FAIL:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openGoldNotEnoughYesOrNo(result->costgold);
					  break;
			}
			default:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_PUP_FLUSH_CAPTAINLIST_FAIL");
					  break;
			}
			}
			break;
		}
	//解雇船长. captainId : 船长id
	case PROTO_TYPE_FireCaptainResult:
		{
			FireCaptainResult* result = (FireCaptainResult*)message;
			if (result->failed == 0)
			{
				closeView();
				deleteMyCaptain(result);
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PUP_FIRE_CAPTAIN_SUCCESS");
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_FLUSH_CAPTAINLIST_FAIL");
			}
			break;
		}
	//获取npc的战斗力区间和玩家的战斗力. ids : npc id(值为0代表玩家), num : npc数量
	case PROTO_TYPE_GetNpcScoreResult:
	{
		GetNpcScoreResult* result = (GetNpcScoreResult*)message;
		if (result->failed == 0)
		{
			m_pNpcScoreResult = result;
			auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
			if (view)
			{
				flushTaskList();
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_FLUSH_CAPTAINLIST_FAIL");
		}
		break;
	}
	case PROTO_TYPE_FireCrewResult:
	{
		FireCrewResult*result = (FireCrewResult*)message;
		if (result->failed ==0)
		{
			m_nCurSailorNum = result->crew_num;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_TARVEN_FIRE_SAILORS_SUCCESS");
		}
		else
		{

		}
		break;
	}

	default:
		break;
	}
}
//放弃任务交互框
void UITavern::openTaskConfirmDialog(const int nIndex,const bool bIsAccept)
{
	openView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB], 11);
	auto w_taskConfirm = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
	auto b_yes = w_taskConfirm->getChildByName<Button*>("button_s_yes");
	auto b_no = w_taskConfirm->getChildByName<Button*>("button_s_no");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent,this));
	b_no->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent,this));
	auto t_title = w_taskConfirm->getChildByName<Text*>("label_repair");
	auto t_content = w_taskConfirm->getChildByName<Text*>("label_content");
	if (bIsAccept)
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_ACCEPT_TITLE"]);
		t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_CONTENT"]);
		b_yes->setTag(nIndex + START_INDEX);		
	}else
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_GIVEUP_TITLE"]);
		t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_CONTENT"]);
		b_yes->setTag(nIndex + 20 * START_INDEX);
	}
}
//雇佣水手界面
void UITavern::openSailorConfirm(const GetAvailCrewNumResult* result)
{
	openView(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB],11);
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB]);
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_content"));
	initSailorDialog();
	int nNum = 30 * (ceil(FAME_NUM_TO_LEVEL(m_pBarInfoResult->fame) / 10.0) + 1);
	if (m_bIsBanquet)
	{
		nNum *= 3;
	}
	std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_HIRE_SAILORS_CONTENT"];
	std::string new_vaule = StringUtils::format("%d",nNum);
	std::string old_vaule = "[num]";
	repalce_all_ditinct(content,old_vaule,new_vaule);
	t_content->setString(content);

	auto t_selnum = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_count"));
	auto t_cost = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_income_num"));
	auto liew_sailor = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_ship_sailor_num"));
	auto t_sailor1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_durable_num_1"));
	auto t_sailor2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_durable_num_2"));
	auto liew_days = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_maxday"));
	auto t_days = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_maxday_num"));
	auto liew_required = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_required"));
	auto t_required = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_required_num"));
	auto l_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_durable"));
	auto l_sailor_add = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_durable_add"));
	auto b_caveat = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_caveat"));

	UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	if (m_pAvailCrewNumResult->availcrewnum == 0)
	{
		dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_plus"))->setTouchEnabled(false);
		dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_minus"))->setTouchEnabled(false);
		dynamic_cast<Slider*>(Helper::seekWidgetByName(view,"slider_goods_num"))->setTouchEnabled(false);	
	}
	ss->setMaxPercent(m_pAvailCrewNumResult->availcrewnum);
	ss->setCurrentNumber(m_pAvailCrewNumResult->availcrewnum);
	t_selnum->setString(StringUtils::format("%d/%d",m_pAvailCrewNumResult->availcrewnum,m_pAvailCrewNumResult->availcrewnum));
	t_cost->setString(numSegment(StringUtils::format("%d",m_pAvailCrewNumResult->crewprice * ss->getCurrentNumber() + m_pBarInfoResult->hirecrewfee)));
	t_sailor1->setString(StringUtils::format("%d",m_pAvailCrewNumResult->currentcrewnum+m_pAvailCrewNumResult->availcrewnum));
	t_sailor2->setString(StringUtils::format("/%d",m_pAvailCrewNumResult->maxcrewnum));

	if(m_pAvailCrewNumResult->onecrewsupply != 0){
		t_days->setString(StringUtils::format("%d",m_pAvailCrewNumResult->totalsupply / (m_pAvailCrewNumResult->onecrewsupply *
				(m_pAvailCrewNumResult->currentcrewnum + m_pAvailCrewNumResult->availcrewnum))));
	}else{
		t_days->setString(StringUtils::format("%d",10000));
	}

	t_required->setString(StringUtils::format("%d",m_pAvailCrewNumResult->totalrequired));
	l_sailor->setPercent(100.0 * (m_pAvailCrewNumResult->currentcrewnum) / m_pAvailCrewNumResult->maxcrewnum);
	l_sailor_add->setPercent(100.0 * (m_pAvailCrewNumResult->currentcrewnum + m_pAvailCrewNumResult->availcrewnum) / m_pAvailCrewNumResult->maxcrewnum);
	b_caveat->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent,this));
//当前水手小于最大水手的10%时，显示红色警告图片，当影响船速时显示橙色警告
	if (m_pAvailCrewNumResult->totalrequired <= m_pAvailCrewNumResult->currentcrewnum + m_pAvailCrewNumResult->availcrewnum)
	{
		t_sailor1->setTextColor(Color4B(40, 25, 13, 255));
		b_caveat->setVisible(false);
	}
	else
	{
		b_caveat->setVisible(true);
		t_sailor1->setTextColor(Color4B(255, 0, 0, 255));
		if (m_pAvailCrewNumResult->currentcrewnum + m_pAvailCrewNumResult->availcrewnum <= m_pAvailCrewNumResult->maxcrewnum*0.1)
		{
			b_caveat->loadTextureNormal("cocosstudio/login_ui/common/hints_sailor_2.png");
			n_sailorNotEnoughTag = SAILOR_URGENT_WARNING;
		}
		else
		{
			b_caveat->loadTextureNormal("cocosstudio/login_ui/common/hints_sailor_1.png");
			n_sailorNotEnoughTag = SAILOR_NOT_ENOUGH_EFFECT_SPEED;
		}
	}
	auto b_cancel = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_cancel"));
	auto b_ok = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_ok"));
	b_cancel->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent,this));
	b_ok->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent,this));

	auto b_income = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_income_info"));
	auto b_maxday = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_maxday_info"));
	b_income->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent,this));
	b_maxday->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent,this));
	liew_sailor->refreshView();
	liew_days->refreshView();
	liew_required->refreshView();
}
//雇佣水手时滑动条变动更新界面显示 
void UITavern::sliderSailor(const int nNum)
{
	UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB]);
	auto t_selnum = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_count"));
	auto t_cost = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_income_num"));
	auto liew_sailor = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_ship_sailor_num"));
	auto t_sailor1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_durable_num_1"));
	auto t_sailor2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_durable_num_2"));
	auto liew_days = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_maxday"));
	auto t_days = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_maxday_num"));
	auto l_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_durable"));
	auto l_sailor_add = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_durable_add"));
	auto b_caveat = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_caveat"));
	
	t_selnum->setString(StringUtils::format("%d/%d",ss->getCurrentNumber(),m_pAvailCrewNumResult->availcrewnum));
	t_cost->setString(numSegment(StringUtils::format("%d",m_pAvailCrewNumResult->crewprice * ss->getCurrentNumber() + m_pBarInfoResult->hirecrewfee)));
	t_sailor1->setString(StringUtils::format("%d",m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber()));
	t_sailor2->setString(StringUtils::format("/%d",m_pAvailCrewNumResult->maxcrewnum));
	if(m_pAvailCrewNumResult->onecrewsupply != 0){
		t_days->setString(StringUtils::format("%d",m_pAvailCrewNumResult->totalsupply / (m_pAvailCrewNumResult->onecrewsupply *
				(m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber()))));
	}else{
		t_days->setString(StringUtils::format("%d",10000));
	}

	l_sailor->setPercent(100.0 * (m_pAvailCrewNumResult->currentcrewnum) / m_pAvailCrewNumResult->maxcrewnum);
	l_sailor_add->setPercent(100.0 * (m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber()) / m_pAvailCrewNumResult->maxcrewnum);
	if (m_pAvailCrewNumResult->totalrequired <= m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber())
	{
		t_sailor1->setTextColor(Color4B(40,25,13,255));
		b_caveat->setVisible(false);
	}else
	{

		b_caveat->setVisible(true);
		t_sailor1->setTextColor(Color4B(255, 0, 0, 255));
		if (m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber() <= m_pAvailCrewNumResult->maxcrewnum*0.1)
		{
			b_caveat->loadTextureNormal("cocosstudio/login_ui/common/hints_sailor_2.png");
			n_sailorNotEnoughTag = SAILOR_URGENT_WARNING;
		}
		else
		{
			b_caveat->loadTextureNormal("cocosstudio/login_ui/common/hints_sailor_1.png");
			n_sailorNotEnoughTag = SAILOR_NOT_ENOUGH_EFFECT_SPEED;
		}	
		
	}
	liew_sailor->refreshView();
	liew_days->refreshView();
}
//刷新界面 
void UITavern::everySecondReflush(const float fTime)
{
	if (m_nCaptainFlushTime > -1)
	{
		if (m_nCaptainFlushTime <= 0)
		{	
			ProtocolThread::GetInstance()->refreshCaptainList(0,UILoadingIndicator::create(this));
		}else
		{
			auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_HIRE_DIALOG_CSB]);
			int64_t myTime = m_nCaptainFlushTime;
			m_nCaptainFlushTime--;
			if (view)
			{
				auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_time"));
				t_time->setString(timeUtil(myTime, TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM));
			}
		}
	}
	if (m_nTaskFlushTime > -1)
	{
		if (m_nTaskFlushTime <= 0)
		{
			ProtocolThread::GetInstance()->getTaskList(0,UILoadingIndicator::create(this));
		}else
		{
			auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
			int64_t myTime = m_nTaskFlushTime;
			m_nTaskFlushTime--;
			if (view)
			{
				auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_time"));
				t_time->setString(timeUtil(myTime, TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM));
			}
		}
	}

	if (m_nTaskFalg && m_pCurTaskDefine->taskid == m_nTaskId)
	{
		auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
		auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_task_time_num"));
		if (m_pCurTaskDefine->surplustime <= 0)
		{
			if (view)
			{
				t_time->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_TIEMOUT"]);
			}
		}else
		{
			m_pCurTaskDefine->surplustime--;
			int64_t myTime = m_pCurTaskDefine->surplustime;
			if (view)
			{
				t_time->setString(timeUtil(myTime, TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM));
			}
		}
	}
}
//酒吧女郎入场
void UITavern::showBarGirlChat(const float fTime)
{

	auto fragmentFullPath = FileUtils::getInstance()->fullPathForFilename("specialShader.vsh");
	auto fragSource = FileUtils::getInstance()->getStringFromFile(fragmentFullPath);
	auto fragmentFullPath1 = FileUtils::getInstance()->fullPathForFilename("specialShader.fsh");
	auto fragSource2 = FileUtils::getInstance()->getStringFromFile(fragmentFullPath1);
	BarGLProgram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource2.c_str());
	auto glprogramstate = GLProgramState::getOrCreateWithGLProgram(BarGLProgram);
	//auto fileUtils = FileUtils::getInstance();
	//std::string vertexSource = fileUtils->getStringFromFile(FileUtils::getInstance()->fullPathForFilename("specialShader.vsh"));
	//std::string fragmentSource = fileUtils->getStringFromFile(FileUtils::getInstance()->fullPathForFilename("specialShader.fsh"));
	//log("qqqqqq:%s",vertexSource.c_str());
	//log("qqqq:%s", fragmentSource.c_str());
	//BarGLProgram = GLProgram::createWithFilenames("specialShader.vsh", "specialShader.fsh");
	BarGLProgram->link();
	BarGLProgram->use();
	BarGLProgram->setUniformsForBuiltins();
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	if (m_nChatContentIndex != CONTENT_WELCOME)
	{
		bargirl_dialog->setTouchEnabled(true);
		view->setTouchEnabled(true);
		view->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent, this));
		bargirlClickBoo = false;
	}
	else
	{
		bargirl_dialog->setTouchEnabled(false);
		bargirlClickBoo = true;
	}
	bargirl_dialog->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent, this));
	m_bChiefClick = false;
	bargirl_dialog->setOpacity(255);
	bargirl_dialog->setVisible(true);
	bargirl_dialog->setPosition(Vec2(0, 0));
	if (m_pSpriteForFavorAnimate)
	{
		m_pSpriteForFavorAnimate->removeFromParentAndCleanup(true);
		m_pSpriteForFavorAnimate = nullptr;
	}
	showFavorStatus();
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	i_bargirl->ignoreContentAdaptWithSize(false);
	i_bargirl->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_BAR_GIRL));
	i_bargirl->setScale9Enabled(false);
	//i_bargirl->setGLProgramState(glprogramstate);
	i_bargirl->setGLProgram(BarGLProgram);
	bargirl_dialog->setPosition(STARTPOS);
	auto i_heart = i_bargirl->getChildByName<ImageView*>("image_f");
	i_heart->setVisible(true);
	i_heart->setOpacity(0);
	i_heart->setScale(0.01, 0.01);
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto t_bargirl = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(0);
	auto intervalTime = 0;
	this->unschedule(schedule_selector(UITavern::showDialogWordText));
	this->scheduleOnce(schedule_selector(UITavern::changeBarGirlChat), 0);
	if (CONTENT_WELCOME == m_nChatContentIndex || CONTENT_BARGIRL_CHAT == m_nChatContentIndex)
	{
		if (i_bargirl->getPositionX() < 0)
		{
			m_BarGirlContentLabel->setOpacity(0);
			i_dialog_bg->setOpacity(0);
			t_bargirl->setOpacity(0);
			i_bargirl->setPositionX(-i_bargirl->getContentSize().width / 2);
			i_bargirl->runAction(MoveBy::create(0.5, Vec2(i_bargirl->getBoundingBox().size.width, 0)));
			i_dialog_bg->setPositionX(0.63*i_dialog_bg->getBoundingBox().size.width);
			i_dialog_bg->runAction(Sequence::create(DelayTime::create(0.1), FadeIn::create(0.5), nullptr));
			t_bargirl->runAction(Sequence::create(DelayTime::create(0.1), FadeIn::create(0.5), nullptr));
			if (CONTENT_WELCOME == m_nChatContentIndex)
			{
				m_BarGirlContentLabel->runAction(Sequence::create(DelayTime::create(0.1), FadeIn::create(0.5), nullptr));
			}
		}
	}
	if (m_nChatContentIndex == CONTENT_WELCOME)
	{
		this->scheduleOnce(schedule_selector(UITavern::mainButtonMoveToLeft), 0);
	}
}
//改变酒吧女郎聊天对话内容
void UITavern::changeBarGirlChat(const float fTime)
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto t_bargirl = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");

	std::string chat_content;
	int n = cocos2d::random() % 2 + 1;
	int f = 1;
	if (m_nFavour <= FAVOUR_MIN)
	{
		f = 1;
	}
	else if (m_nFavour <= FAVOUR_MID)
	{
		f = 2;
	}
	else if (m_nFavour < FAVOUR_MAX)
	{
		f = 3;
	}
	else
	{
		f = 4;
	}
	std::string gender[] = { "", "MAIE", "FEMAIE" };
	std::string favour[] = { "", "MIN", "MID", "MAX", "LAR" };
	std::string chatIndex[] = { "WOLCOME", "ACCEPT_DRINK", "REFUSE_DRINK", "ACCEPT_GIFT" };
	std::string tip_index;
	auto tips = SINGLE_SHOP->getTipsInfo();
	switch (m_nChatContentIndex)
	{
	case CONTENT_WELCOME:
	{
		if (m_nFavour >= FAVOUR_SIDE_TASK && m_pBarInfoResult->bartask && m_pBarInfoResult->bartask->taskstatus && m_pBarInfoResult->bartask->completecityid != SINGLE_HERO->m_iCityID)
		{
			tip_index = "TIP_PUP_BARGIRL_HELP_TO_COLLECT";
		}
		else
		{
			tip_index = StringUtils::format("TIP_PUP_BARGIRL_%s_%s%d", chatIndex[m_nChatContentIndex].c_str(), favour[f].c_str(), n);
		}
		chat_content = tips[tip_index];
		break;
	}
	case CONTENT_ACCEPT_DRINK:
	{
		tip_index = StringUtils::format("TIP_PUP_BARGIRL_%s_%s_%s%d", gender[SINGLE_HERO->m_iGender].c_str(), chatIndex[m_nChatContentIndex].c_str(), favour[f].c_str(), n);
		chat_content = tips[tip_index];
		break;
	}
	case CONTENT_REFUSE_DRINK:
	{
		tip_index = StringUtils::format("TIP_PUP_BARGIRL_%s_%s", chatIndex[m_nChatContentIndex].c_str(), favour[f].c_str());
		chat_content = tips[tip_index];
		break;
	}
	case CONTENT_ACCEPT_GIFT:
	{
		tip_index = StringUtils::format("TIP_PUP_BARGIRL_%s_%s%d", chatIndex[m_nChatContentIndex].c_str(), favour[f].c_str(), n);
		chat_content = tips[tip_index];
		break;
	}
	case CONTENT_INFO_RMATION:
	{
		parseIntelligence(chat_content);
		break;
	}
	case CONTENT_BARGIRL_CHAT:
	{
		tip_index = StringUtils::format("TIP_PUP_BARGIRL_CHAT");
		chat_content = tips[tip_index];
		break;
	}
	case CONTENT_NO_INTELLIGENCE:
	{
		tip_index = StringUtils::format("TIP_PUP_BARGIRL_NO_INTELLIGENCE");
		chat_content = tips[tip_index];
		break;
	}
	default:
		break;
	}
	if (!m_bIsFriend)
	{
		if (m_IsVillage)
		{
			//村庄的友好度暂为0
			chat_content = SINGLE_SHOP->getTipsInfo()["TIP_VILLAGE_TAVERN_DIALOG"];
		}
		else
		{
			chat_content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_CHAT_NOT_FIREND"];
		}
	}
	
	std::string new_vaule = SINGLE_HERO->m_sName;
	std::string old_vaule = "[heroname]";
	repalce_all_ditinct(chat_content, old_vaule, new_vaule);
	m_BarGirlContentLabel->setOpacity(255);
	m_BarGirlContentLabel->setPositionY(m_BarGirlContentLabel->getSystemFontSize() * 3);
	m_BarGirlContentLabel->setString(chat_content);

	i_anchor->setPositionY(bargirlPositionY);
	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
	{
		//村庄为酒吧老板
		t_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_VILLAGE_TAVERN_BOSS_NAME"]);
	}
	else
	{
		t_bargirl->setString(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].bargirl);
	}
	
	if (m_nChatContentIndex != CONTENT_WELCOME)
	{
		//chatContent为记录聊天界面逐字显示的内容
		chatContent = chat_content;
		chatTxt = m_BarGirlContentLabel;
		m_BarGirlContentLabel->setString("");
		lenNum = 0;
		wordLenBoo = false;
		guardDiaolgOver = false;
		auto fontSize = m_BarGirlContentLabel->getSystemFontSize();
		auto dialogHeight = getLabelHight(chat_content, t_content->getBoundingBox().size.width, m_BarGirlContentLabel->getSystemFontName(), fontSize);
		
		auto leftNum = 0;
		if (dialogHeight / (fontSize * 3) > 1)
		{
			auto floorNum = floor(dialogHeight / (fontSize * 3));
			leftNum = dialogHeight - ((fontSize + 4.6) * floorNum);
		}
		else
		{
			leftNum = 0;
		}
		if (leftNum > fontSize)
		{
			m_nToTalDialogPage = ceil(dialogHeight / (fontSize * 3));
		}
		else
		{
			if (dialogHeight / (fontSize * 3) > 1)
			{
				m_nToTalDialogPage = floor(dialogHeight / (fontSize * 3));
			}
			else
			{
				m_nToTalDialogPage = 1;
			}
		}
		//log("m_nToTalDialogPage = %d dialogHeight = %f leftNum = %d floorNum = %lf chat_content = %s", m_nToTalDialogPage, dialogHeight, leftNum, floor(dialogHeight / (m_BarGirlContentLabel->getSystemFontSize() * 3)), chat_content.c_str());
		m_nCurDialogPage = 1;
		UITavern::showDialogWordSchedule();
		bargirlClickBoo = false;
	}
}
//解析情报 
void UITavern::parseIntelligence(std::string &text)
{
	rapidjson::Document root;
	std::string new_value;
	std::string old_value;
	if (!m_pIntelligenceResult->n_json)
	{
		int index = cocos2d::random(1, 15);
		//本地tip
		text = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_LOADING_TEXT_%d", index)];
	}
	for (int i = 0; i < m_pIntelligenceResult->n_json; i++)
	{
		root.Parse<0>(m_pIntelligenceResult->json[i]);
		auto type = DictionaryHelper::getInstance()->getIntValue_json(root,"type");
		//1-8服务器决定type，10好感度不足50
		switch (type)
		{
		case 1:
			{
				//漂浮物
				int x = DictionaryHelper::getInstance()->getIntValue_json(root,"x");
				int  y = MAP_WIDTH * MAP_CELL_WIDTH - DictionaryHelper::getInstance()->getIntValue_json(root, "y");
				new_value = StringUtils::format("(%d,%d)",x,y);
				old_value = "[position]";
				text = SINGLE_SHOP->getTipsInfo()["TIP_PUP_INTELLIGENCE_POSITION"];
				repalce_all_ditinct(text,old_value,new_value);
				break;
			}
		case 2:
			{
				//流行品
				auto city_id = DictionaryHelper::getInstance()->getIntValue_json(root,"city_id");
				auto count = DictionaryHelper::getInstance()->getArrayCount_json(root,"goods");
				new_value = SINGLE_SHOP->getCitiesInfo()[city_id].name;
				old_value = "[city]";
				text = SINGLE_SHOP->getTipsInfo()["TIP_PUP_INTELLIGENCE_GOODS"];
				repalce_all_ditinct(text,old_value,new_value);
				new_value = "";
				old_value = "[goods]";
				for (int i = 0; i < count; i++)
				{
					auto goods = DictionaryHelper::getInstance()->getIntValueFromArray_json(root,"goods",i);
					new_value += SINGLE_SHOP->getGoodsData()[goods].name;
					if (i < count-1)
					{
						new_value += ",";
					}
				}
				repalce_all_ditinct(text,old_value,new_value);
				break;
			}
		case 3:
			{
				//npc
				int city_id = DictionaryHelper::getInstance()->getIntValue_json(root,"city_id");
				int city_x = DictionaryHelper::getInstance()->getIntValue_json(root,"city_x");
				int city_y = MAP_WIDTH * MAP_CELL_WIDTH - DictionaryHelper::getInstance()->getIntValue_json(root, "city_y");
				int npc_x = DictionaryHelper::getInstance()->getIntValue_json(root,"npc_x");
				int npc_y = MAP_WIDTH * MAP_CELL_WIDTH - DictionaryHelper::getInstance()->getIntValue_json(root, "npc_y");
				int npc_id = DictionaryHelper::getInstance()->getIntValue_json(root, "npc_id");

				text = SINGLE_SHOP->getTipsInfo()["TIP_PUP_INTELLIGENCE_NPC"];
				
				new_value = SINGLE_SHOP->getBattleNpcInfo()[npc_id].name;
				old_value = "[npc]";
				repalce_all_ditinct(text, old_value, new_value);

				new_value = StringUtils::format("(%d,%d)",npc_x,npc_y);
				old_value = "[position1]";
				repalce_all_ditinct(text,old_value,new_value);
				new_value = SINGLE_SHOP->getCitiesInfo()[city_id].name;
				old_value = "[city]";
				repalce_all_ditinct(text,old_value,new_value);
				break;
			}
		case 4:
			{
				   //送货任务，购买goods
				   int good_id = DictionaryHelper::getInstance()->getIntValue_json(root, "good_id");
				   auto count = DictionaryHelper::getInstance()->getArrayCount_json(root, "city_id");//有此物品的城市数量
				   text = SINGLE_SHOP->getTipsInfo()["TIP_PUP_INTELLIGENCE_TASK_BUY_GOODS"];
				   old_value = "[goods]";
				   new_value = SINGLE_SHOP->getGoodsData()[good_id].name;
				   repalce_all_ditinct(text, old_value, new_value);
				   new_value = "";
				   old_value = "[goods_city]";
				   for (int i = 0; i < count; i++)
				   {
					   auto city = DictionaryHelper::getInstance()->getIntValueFromArray_json(root, "city_id", i);
					   new_value += SINGLE_SHOP->getCitiesInfo()[city].name;
					   if (i < count - 1)
					   {
						   new_value += ",";
					   }
				   }
				   repalce_all_ditinct(text, old_value, new_value);
				  break;
			}
		case 5:
		{
				   //普通游戏提示
				   int indexNormal = cocos2d::random(1, 8);
				   text = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PUP_NO_USE_MESSAGE_%d", indexNormal)];
				   break;
		}
		case 6:
		{
				   //女郎呓语
				   int indexDream = cocos2d::random(1, 4);
				   text = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PUP_BARGIRL_DREAM_WORDS_%d", indexDream)];
				   break;
		}
		case 7:
		{
				   //进阶游戏提示
				   int indexAdvanceGame = cocos2d::random(1, 2);
				   text = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PUP_BARGIRL_ADVANCED_GAME_CONTENT_%d", indexAdvanceGame)];
				   break;
		}
		case 8:
		{
				   //女郎偏好礼物
				   auto gift_id = DictionaryHelper::getInstance()->getIntValue_json(root, "good_id");
				   auto countCity = DictionaryHelper::getInstance()->getArrayCount_json(root, "city_id");//有此物品的城市数量
				   int index = cocos2d::random(1, 3);
				   text = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PUP_BARGIRL_PREFERENCE_GOOD_CONTENT_%d", index)];
				   if (index == 1 || index == 2)
				   {
					   old_value = "[present]";
					   new_value = SINGLE_SHOP->getItemData()[gift_id].name;
					   repalce_all_ditinct(text, old_value, new_value);
				   }
				   if (index == 3)
				   {
					   old_value = "[present]";
					   new_value = SINGLE_SHOP->getItemData()[gift_id].name;
					   repalce_all_ditinct(text, old_value, new_value);
					   new_value = "";
					   old_value = "[city]";
					   for (int i = 0; i < countCity; i++)
					   {
						   auto city = DictionaryHelper::getInstance()->getIntValueFromArray_json(root, "city_id", i);
						   new_value += SINGLE_SHOP->getCitiesInfo()[city].name;
						   if (i < countCity - 1)
						   {
							   new_value += ",";
						   }
					   }
					   repalce_all_ditinct(text, old_value, new_value);
				   }
				   break;
		}
		case 10:
		{
				    //好感度不足50时候按概率出现普通游戏提示，女郎偏好礼物，女郎呓语
					int rate = 0;
					auto gift_id = DictionaryHelper::getInstance()->getIntValue_json(root, "good_id");
					if (gift_id)
					{
						rate = cocos2d::random(1, 100);
					}
					else
					{
						rate = cocos2d::random(1, 50);
					}
					if (rate <= 50)
					{
						if (rate <= 10)
						{
							//女郎呓语
							int indexDream = cocos2d::random(1, 4);
							text = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PUP_BARGIRL_DREAM_WORDS_%d", indexDream)];
						}
						else
						{						
							//普通信息
							int indexNormal = cocos2d::random(1, 8);
							text = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PUP_NO_USE_MESSAGE_%d", indexNormal)];
						}
					}
					else
					{
						//偏好礼物
						
						//有此物品的城市数量
						auto countCity = DictionaryHelper::getInstance()->getArrayCount_json(root, "city_id");
						int index = cocos2d::random(1, 3);
						text = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PUP_BARGIRL_PREFERENCE_GOOD_CONTENT_%d", index)];
						if (index == 1 || index == 2)
						{
							old_value = "[present]";
							new_value = SINGLE_SHOP->getItemData()[gift_id].name;
							repalce_all_ditinct(text, old_value, new_value);
						}
						if (index == 3)
						{
							old_value = "[present]";
							new_value = SINGLE_SHOP->getItemData()[gift_id].name;
							repalce_all_ditinct(text, old_value, new_value);
							new_value = "";
							old_value = "[city]";
							for (int i = 0; i < countCity; i++)
							{
								auto city = DictionaryHelper::getInstance()->getIntValueFromArray_json(root, "city_id", i);
								new_value += SINGLE_SHOP->getCitiesInfo()[city].name;
								if (i < countCity - 1)
								{
									new_value += ",";
								}
							}
							repalce_all_ditinct(text, old_value, new_value);
						}
					}

					break;
		}
		default:
			break;
		}
			
	}
}
//副官入场(雇佣水手界面请喝酒后)
void UITavern::showBarChiefChat(const float fTime)
{
	if (m_pSpriteForFavorAnimate)
	{
		m_pSpriteForFavorAnimate->removeFromParentAndCleanup(true);
		m_pSpriteForFavorAnimate = nullptr;
	}
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto chief_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	chief_dialog->setTouchEnabled(true);
	chief_dialog->addTouchEventListener(CC_CALLBACK_2(UITavern::sailorButtonEvent,this));
	auto i_chief = chief_dialog->getChildByName<ImageView*>("image_bargirl");
	i_chief->setPositionX(-i_chief->getBoundingBox().size.width/2);
	i_chief->ignoreContentAdaptWithSize(false);
	i_chief->loadTexture(getCheifIconPath());
	chief_dialog->setVisible(true);
	chief_dialog->setPosition(STARTPOS);
	auto i_dialog_bg = chief_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto t_barChief = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
	i_dialog_bg->setOpacity(0);
	m_BarGirlContentLabel->setOpacity(0);
	t_barChief->setOpacity(0);
	i_anchor->setOpacity(0);
	i_chief->runAction(MoveBy::create(0.5, Vec2(i_chief->getBoundingBox().size.width, 0)));
	i_dialog_bg->setPositionX(0.63*i_dialog_bg->getBoundingBox().size.width);
	auto act_1 = FadeIn::create(0.5);
	auto act_2 = FadeIn::create(0.5);
	auto act_3 = FadeIn::create(0.5);
	
	m_BarGirlContentLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_CHEIF_ACCEPT_DRINK"]);
	if (SINGLE_HERO->m_iGender == 1)
	{
		t_barChief->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"]);
	}else
	{
		t_barChief->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"]);
	}
	m_BarGirlContentLabel->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), act_1));
	i_dialog_bg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5),act_2));
	t_barChief->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5),act_3));
	this->scheduleOnce(schedule_selector(UITavern::anchorAction),1);
}
//对话的点击提示图标运动
void UITavern::anchorAction(const float fTime)
{
	bargirlClickBoo = false;
	auto guard_dialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB])->getChildByName("panel_bargirl_dialog");
	if (m_nChatContentIndex == CONFIRM_INDEX_CHIEF_CHAT)
	{
		m_nChatContentIndex++;
	}
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_dialog_bg")->getChildByName<ImageView*>("image_anchor");
	i_anchor->stopAllActions();
	i_anchor->setPosition(Vec2(642,39));
	i_anchor->setOpacity(255);
	i_anchor->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5,Vec2(0,10))),EaseBackOut::create(MoveBy::create(0.5, Vec2(0,-10))))));
}
/*
*关闭女郎对话
*/
void UITavern::closeBarGirlChat()
{
	auto bargirl_dialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB])->getChildByName<Widget*>("panel_bargirl_dialog");
	bargirl_dialog->setTouchEnabled(true);
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto t_batgirl = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
	i_anchor->stopAllActions();
	i_anchor->setOpacity(0);
	bargirl_dialog->stopAllActions();
	if (CONTENT_WELCOME == m_nChatContentIndex)
	{
		t_content->runAction(FadeOut::create(0.1));
		i_dialog_bg->runAction(FadeOut::create(0.1));
		t_batgirl->runAction(FadeOut::create(0.1));
		i_bargirl->runAction(Sequence::createWithTwoActions(DelayTime::create(0), MoveBy::create(0.3, Vec2(-i_bargirl->getBoundingBox().size.width, 0))));
		bargirl_dialog->runAction(Sequence::createWithTwoActions(DelayTime::create(0), MoveTo::create(0, ENDPOS)));
	}
	else if (CONTENT_BARGIRL_CHAT == m_nChatContentIndex)
	{

	}
	else
	{
		t_content->runAction(Sequence::create(FadeOut::create(0.2), DelayTime::create(0.3), FadeIn::create(0.2), nullptr));
		i_dialog_bg->runAction(Sequence::create(FadeOut::create(0.2), Place::create(Vec2(0.63*i_dialog_bg->getBoundingBox().size.width, i_dialog_bg->getPositionY())), DelayTime::create(0.3), FadeIn::create(0.2), nullptr));
		t_batgirl->runAction(Sequence::create(FadeOut::create(0.2), DelayTime::create(0.3), FadeIn::create(0.2), nullptr));
		i_bargirl->runAction(Sequence::createWithTwoActions(DelayTime::create(0.2), MoveBy::create(0.3, Vec2(-i_bargirl->getBoundingBox().size.width*0.34, 0))));
		m_nChatContentIndex = CONTENT_BARGIRL_CHAT;
		this->scheduleOnce(schedule_selector(UITavern::changeBarGirlChat), 0);
	}
	bargirl_dialog->removeChildByTag(101);
}
/*
* 关闭副官对话
*/
void UITavern::closeBarChiefChat()
{
	auto barchief_dialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB])->getChildByName<Widget*>("panel_bargirl_dialog");
	auto i_dialog_bg = barchief_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto t_barchief = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_barchief = barchief_dialog->getChildByName<ImageView*>("image_bargirl");
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
	auto act_1 = FadeOut::create(0.5);
	auto act_2 = FadeOut::create(0.5);
	auto act_3 = FadeOut::create(0.5);
	i_anchor->stopAllActions();
	i_anchor->setOpacity(0);
	m_BarGirlContentLabel->runAction(act_1);
	i_dialog_bg->runAction(act_2);
	t_barchief->runAction(act_3);
	i_barchief->runAction(Sequence::createWithTwoActions(DelayTime::create(0), MoveBy::create(0.5, Vec2(-i_barchief->getBoundingBox().size.width, 0))));
	barchief_dialog->runAction(Sequence::createWithTwoActions(DelayTime::create(0),MoveTo::create(0,ENDPOS)));
}
/*
* 酒馆主界面tavern_csb所有按钮向左移动
*/
void UITavern::mainButtonMoveToLeft(const float fTime)
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto p_tavern = view->getChildByName<Widget*>("panel_tavern");
	p_tavern->setPosition(STARTPOS);
	auto b_chat = p_tavern->getChildByName<Button*>("button_chat");
	auto b_order = p_tavern->getChildByName<Button*>("button_order");
	auto b_sailor = p_tavern->getChildByName<Button*>("button_recruit_sailor");
	auto b_captain = p_tavern->getChildByName<Button*>("button_hire_captain");
	auto b_bounty = p_tavern->getChildByName<Button*>("button_bounty_board");
	auto b_pos_x = b_chat->getPositionX();
	auto winsize = Director::getInstance()->getWinSize();
	b_chat->setPositionX(winsize.width+b_chat->getBoundingBox().size.width/2);
	b_order->setPositionX(winsize.width+b_order->getBoundingBox().size.width/2);
	b_sailor->setPositionX(winsize.width+b_sailor->getBoundingBox().size.width/2);
	b_captain->setPositionX(winsize.width+b_captain->getBoundingBox().size.width/2);
	b_bounty->setPositionX(winsize.width+b_bounty->getBoundingBox().size.width/2);
	b_chat->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent,this));
	b_order->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent, this));
	b_sailor->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent, this));
	b_captain->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent, this));
	b_bounty->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent, this));
	b_chat->runAction(Sequence::create(MoveBy::create(0.2,Vec2(-b_chat->getBoundingBox().size.width-100,0)),MoveBy::create(0.1,Vec2(50,0)),nullptr));
	b_order->runAction(Sequence::create(DelayTime::create(0.05),MoveBy::create(0.2,Vec2(-b_order->getBoundingBox().size.width-100,0)),MoveBy::create(0.1,Vec2(50,0)),nullptr));
	b_sailor->runAction(Sequence::create(DelayTime::create(0.1),MoveBy::create(0.2,Vec2(-b_sailor->getBoundingBox().size.width-100,0)),MoveBy::create(0.1,Vec2(50,0)),nullptr));
	b_captain->runAction(Sequence::create(DelayTime::create(0.15),MoveBy::create(0.2,Vec2(-b_captain->getBoundingBox().size.width-100,0)),MoveBy::create(0.1,Vec2(50,0)),nullptr));
	b_bounty->runAction(Sequence::create(DelayTime::create(0.2),MoveBy::create(0.2,Vec2(-b_bounty->getBoundingBox().size.width-100,0)),MoveBy::create(0.1,Vec2(50,0)),nullptr));
	if (m_IsVillage)
	{
		b_chat->setBright(false);
		b_order->setBright(false);
		b_captain->setBright(false);
		b_bounty->setBright(false);
	}
}
/*
* 酒馆主界面tavern_csb所有按钮向右移动
*/
void UITavern::mainButtonMoveToRight()
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto p_tavern = view->getChildByName<Widget*>("panel_tavern");
	p_tavern->setPosition(STARTPOS);
	auto b_chat = p_tavern->getChildByName<Button*>("button_chat");
	auto b_order = p_tavern->getChildByName<Button*>("button_order");
	auto b_sailor = p_tavern->getChildByName<Button*>("button_recruit_sailor");
	auto b_captain = p_tavern->getChildByName<Button*>("button_hire_captain");
	auto b_bounty = p_tavern->getChildByName<Button*>("button_bounty_board");
	auto winsize = Director::getInstance()->getWinSize();
	b_chat->runAction(Sequence::create(MoveTo::create(0.2, Vec2(b_chat->getBoundingBox().size.width / 2 + winsize.width, b_chat->getPositionY())), nullptr));
	b_order->runAction(Sequence::create(DelayTime::create(0.05), MoveTo::create(0.2, Vec2(b_order->getBoundingBox().size.width / 2 + winsize.width, b_order->getPositionY())), nullptr));
	b_sailor->runAction(Sequence::create(DelayTime::create(0.1), MoveTo::create(0.2, Vec2(b_sailor->getBoundingBox().size.width / 2 + winsize.width, b_sailor->getPositionY())), nullptr));
	b_captain->runAction(Sequence::create(DelayTime::create(0.15), MoveTo::create(0.2, Vec2(b_captain->getBoundingBox().size.width / 2 + winsize.width, b_captain->getPositionY())), nullptr));
	b_bounty->runAction(Sequence::create(DelayTime::create(0.2), MoveTo::create(0.2, Vec2(b_bounty->getBoundingBox().size.width / 2 + winsize.width, b_bounty->getPositionY())), nullptr));
}
/*
* 酒馆主界面显示tavern_2_csb
*/
void UITavern::openOtherMainButton(const float fTime)
{
	auto p_tavern2 = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	p_tavern2->setVisible(true);
	auto otherButton = p_tavern2->getChildByName<Widget*>("panel_two_butter");
	otherButton->setVisible(true);
	otherButton->setOpacity(255);
	auto b_chat2 = otherButton->getChildByName<Button*>("button_chat");
	auto b_order2 = otherButton->getChildByName<Button*>("button_order");
	auto b_sailor2 = otherButton->getChildByName<Button*>("button_recruit_sailor");
	auto b_captain2 = otherButton->getChildByName<Button*>("button_hire_captain");
	auto b_bounty2 = otherButton->getChildByName<Button*>("button_bounty_board");
	if (m_IsVillage)
	{
		b_chat2->loadTextureNormal("res/wharfRes/btn_off_black.png");
		b_order2->loadTextureNormal("res/wharfRes/btn_off_black.png");
		b_captain2->loadTextureNormal("res/wharfRes/btn_off_black.png");
		b_bounty2->loadTextureNormal("res/wharfRes/btn_off_black.png");
	}
	b_chat2->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent,this));
	b_order2->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent,this));
	b_sailor2->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent,this));
	b_captain2->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent,this));
	b_bounty2->addTouchEventListener(CC_CALLBACK_2(UITavern::mainButtonEvent,this));
}
/*
* 酒馆主界面tavern_2_csb所有按钮隐藏
*/
void UITavern::mainButtonTavern2CsbHide(const float fTime)
{
	auto p_tavern2 = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	p_tavern2->setVisible(true);
	auto otherButton = p_tavern2->getChildByName<Widget*>("panel_two_butter");
	otherButton->setVisible(false);
	otherButton->setOpacity(0);
}
//按钮选中的状态改变
void UITavern::mainButtonChangeStatus(Widget* target)
{
	if (m_pMainButton)
	{
		m_pMainButton->runAction(MoveBy::create(0.2,Vec2(20,0)));
		m_pMainButton->setBright(true);
		m_pMainButton->setTouchEnabled(true);
	}
	m_pMainButton = target;
	if (m_pMainButton)
	{
		m_pMainButton->runAction(MoveBy::create(0.2, Vec2(-20, 0)));
		m_pMainButton->setBright(false);
		m_pMainButton->setTouchEnabled(false);
	}
}
//酒吧女郎聊天按钮选中的状态改变
void UITavern::minorButtonChangeStatus(Widget* target)
{
	if (m_pBarGirlButton)
	{
		m_pBarGirlButton->runAction(MoveBy::create(0.2, Vec2(20, 0)));
		m_pBarGirlButton->setBright(true);
		m_pBarGirlButton->setTouchEnabled(true);
	}
	m_pBarGirlButton = target;
	if (m_pBarGirlButton)
	{
		m_pBarGirlButton->runAction(MoveBy::create(0.2, Vec2(-20, 0)));
		m_pBarGirlButton->setBright(false);
		m_pBarGirlButton->setTouchEnabled(false);
	}
}
/*
* 酒吧聊天界面的按钮 向左移动
*/
void UITavern::minorButtonMoveToLeft(const float fTime)
{
	auto barGirl = getViewRoot(TARVEN_COCOS_RES[TARVEN_CHAT_CSB]);
	auto b_drink = dynamic_cast<Button*>(Helper::seekWidgetByName(barGirl, "button_drink"));
	auto b_gift = dynamic_cast<Button*>(Helper::seekWidgetByName(barGirl, "button_gift"));
	auto b_gossip = dynamic_cast<Button*>(Helper::seekWidgetByName(barGirl, "button_gossip"));
	auto b_back = dynamic_cast<Button*>(Helper::seekWidgetByName(barGirl, "button_back"));
	b_drink->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent, this));
	b_gift->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent, this));
	b_gossip->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent, this));
	b_back->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent, this));
	float delayTime = fTime;
	b_drink->runAction(Sequence::create(DelayTime::create(delayTime + 0), MoveBy::create(0.2, Vec2(-b_drink->getBoundingBox().size.width - 30, 0)), MoveBy::create(0.1, Vec2(20, 0)), nullptr));
	b_gift->runAction(Sequence::create(DelayTime::create(delayTime + 0.05), MoveBy::create(0.2, Vec2(-b_gift->getBoundingBox().size.width - 30, 0)), MoveBy::create(0.1, Vec2(20, 0)), nullptr));
	b_gossip->runAction(Sequence::create(DelayTime::create(delayTime + 0.1), MoveBy::create(0.2, Vec2(-b_gossip->getBoundingBox().size.width - 30, 0)), MoveBy::create(0.1, Vec2(20, 0)), nullptr));
	b_back->runAction(Sequence::create(DelayTime::create(delayTime + 0.15), MoveBy::create(0.2, Vec2(-b_back->getBoundingBox().size.width - 30, 0)), MoveBy::create(0.1, Vec2(20, 0)), nullptr));
	tarvenShowOrHide(delayTime, true);
	closeBarGirlChat();
}
/*
* 酒吧女郎聊天button右移动
*/
void UITavern::minorButtonMoveToRight(const float fTime)
{
	auto barGirl = getViewRoot(TARVEN_COCOS_RES[TARVEN_CHAT_CSB]);
	auto b_drink = dynamic_cast<Button*>(Helper::seekWidgetByName(barGirl, "button_drink"));
	auto b_gift = dynamic_cast<Button*>(Helper::seekWidgetByName(barGirl, "button_gift"));
	auto b_gossip = dynamic_cast<Button*>(Helper::seekWidgetByName(barGirl, "button_gossip"));
	auto b_back = dynamic_cast<Button*>(Helper::seekWidgetByName(barGirl, "button_back"));
	
	b_drink->runAction(Sequence::create(DelayTime::create(fTime + 0), MoveBy::create(0.2, Vec2(b_drink->getBoundingBox().size.width + 10, 0)), nullptr));
	b_gift->runAction(Sequence::create(DelayTime::create(fTime + 0.05), MoveBy::create(0.2, Vec2(b_gift->getBoundingBox().size.width + 10, 0)), nullptr));
	b_gossip->runAction(Sequence::create(DelayTime::create(fTime + 0.1), MoveBy::create(0.2, Vec2(b_gossip->getBoundingBox().size.width + 10, 0)), nullptr));
	b_back->runAction(Sequence::create(DelayTime::create(fTime + 0.15), MoveBy::create(0.2, Vec2(b_back->getBoundingBox().size.width + 10, 0)), nullptr));
	if (m_nChatContentIndex != CONTENT_WELCOME)
	{
		tarvenShowOrHide(fTime, false);
	}

}
/*
* 酒吧标题的显示隐藏
*/
void UITavern::tarvenShowOrHide(const float fTime, const bool isShow)
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	auto p_actionbar = view->getChildByName<Widget*>("panel_actionbar");

	Vector<Node*> arrayRootChildren = p_actionbar->getChildren();
	for (auto obj : arrayRootChildren)
	{
		Widget* child = dynamic_cast<Widget*>(obj);
		if (isShow)
		{
			if (child->getOpacity() == 0)
			{
				child->runAction(Sequence::createWithTwoActions(DelayTime::create(fTime), Spawn::createWithTwoActions(FadeIn::create(0.4f), MoveBy::create(0.4f, Vec2(p_actionbar->getBoundingBox().size.width, 0)))));
			}
		}else
		{
			if (child->getOpacity() == 255)
			{
				child->runAction(Sequence::createWithTwoActions(DelayTime::create(fTime), Spawn::createWithTwoActions(FadeOut::create(0.4f), MoveBy::create(0.4f, Vec2(-p_actionbar->getBoundingBox().size.width, 0)))));
			}
		}
	}
}
//动作结束 更改动作判定状态
void UITavern::actionRunningEnd(const float fTime)
{
	m_bActionRunning = false;
}
//打开聊天交互
void UITavern::openBarGirlDialog(Widget* button)
{
	openView(TARVEN_COCOS_RES[TARVEN_CHAT_CSB]);
	auto barGirl = getViewRoot(TARVEN_COCOS_RES[TARVEN_CHAT_CSB]);
	barGirl->setTouchEnabled(false);
	initBarGirlChat();
	mainButtonChangeStatus(button);
	closeDialog();
	m_nViewIndex = UI_CHAT;
}
//打开酒水交互
void UITavern::openFoodAndDrinkDialog(Widget* button)
{
	openView(TARVEN_COCOS_RES[TARVEN_ORDER_CSB]);
	auto foodAndDrink = getViewRoot(TARVEN_COCOS_RES[TARVEN_ORDER_CSB]);
	moveTo(foodAndDrink, STARTPOS);
	initFoodAndDrink();
	mainButtonChangeStatus(button);
	closeDialog();
	m_nViewIndex = UI_ORDER;
}
//打开水手交互
void UITavern::openSailorDialog(Widget* button)
{
	openView(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
	auto sailors = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
	auto button_chat_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailors, "button_chat_1"));
	if (m_IsVillage)
	{
		button_chat_1->setOpacity(125);
	}
	else
	{
		button_chat_1->setOpacity(255);
	}
	moveTo(sailors,STARTPOS);
	initSailor();
	if(button)
	{
		mainButtonChangeStatus(button);
	}
	closeDialog();
	m_nViewIndex = UI_RECRUIT_SAILOR;
}
//打开船长交互
void UITavern::openCaptainDialog(Widget* button)
{
	openView(TARVEN_COCOS_RES[TARVEN_HIRE_CSB]);
	auto captain = getViewRoot(TARVEN_COCOS_RES[TARVEN_HIRE_CSB]);
	moveTo(captain,STARTPOS);
	initCaptain();
	mainButtonChangeStatus(button);
	closeDialog();
	m_nViewIndex = UI_HIRE_CAPTAIN;
}
//打开赏金榜交互
void UITavern::openTaskDialog(Widget* button)
{
	openView(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
	auto w_taskDialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_QUEST_CSB]);
	auto panel_title = w_taskDialog->getChildByName<Widget*>("panel_title");
	auto label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title,"label_title"));
	auto label_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title, "label_time"));
	label_time->setString("00::00::00");
	auto button_repair_v = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_title, "button_repair_v"));
	moveTo(w_taskDialog,STARTPOS);
	initTask();
	mainButtonChangeStatus(button);
	closeDialog();
	m_nViewIndex = UI_BOUNTY_BOARD;
}
//关闭当前view	
void UITavern::closeDialog()
{
	std::string filePath;
	switch (m_nViewIndex)
	{
	case 1:
		//filePath = TARVEN_COCOS_RES[TARVEN_CHAT_CSB];
		break;
	case 2:
		filePath = TARVEN_COCOS_RES[TARVEN_ORDER_CSB];
		break;
	case 3:
		filePath = TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB];
		break;
	case 4:
		filePath = TARVEN_COCOS_RES[TARVEN_HIRE_CSB];
		break;
	case 5:
		filePath = TARVEN_COCOS_RES[TARVEN_QUEST_CSB];
		break;
	default:
		break;
	}
	auto view = getViewRoot(filePath);
	if (view)
	{
		view->runAction(MoveTo::create(0.4f,ENDPOS3));
		closeView(filePath,0.4f);
	}
}
//显示礼物界面
void UITavern::openGiftView()
{
	openView(TARVEN_COCOS_RES[TARVEN_CHAT_GIFT_CSB], 11);
	auto giftView = getViewRoot(TARVEN_COCOS_RES[TARVEN_CHAT_GIFT_CSB]);

	auto l_gift = dynamic_cast<ListView*>(Helper::seekWidgetByName(giftView, "listview_food"));
	auto b_gift = dynamic_cast<Button*>(Helper::seekWidgetByName(giftView, "button_item"));
	b_gift->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent,this));
	std::string name;
	std::string path;
	int j = 0;
	ListView *l_item;
	for (int i = 0; i < m_pGiftResult->n_equipments; i++)
	{
		int sub_type = SINGLE_SHOP->getItemData()[m_pGiftResult->equipments[i]->equipmentid].sub_type;
		if (sub_type == SUB_TYPE_GIRL_GIFT)
		{
			j++;
		}
		else
		{
			continue;
		}
		auto item_clone = b_gift->clone();
		item_clone->setTag(m_pGiftResult->equipments[i]->equipmentid);
		item_clone->setSwallowTouches(false);
		auto i_goods = item_clone->getChildByName<ImageView*>("image_item");
		auto t_name = item_clone->getChildByName<Text*>("label_item_name");
		auto t_num = item_clone->getChildByName<Text*>("label_item_num");
		i_goods->ignoreContentAdaptWithSize(false);
		i_goods->loadTexture(getItemIconPath(m_pGiftResult->equipments[i]->equipmentid, IMAGE_ICON_SHADE));
		t_name->setString(getItemName(m_pGiftResult->equipments[i]->equipmentid));
		t_name->setContentSize(Size(190, getLabelHight(t_name->getString(), 190, t_name->getFontName())));
		t_num->enableOutline(Color4B(0,0,0,255),2);
		t_num->setString(StringUtils::format("%d",m_pGiftResult->equipments[i]->amount));

		if (j % 4 == 1)
		{
			l_item = ListView::create();
			l_item->setTouchEnabled(false);
			l_item->setSize(Size(4 * b_gift->getContentSize().width, b_gift->getContentSize().height));
			l_item->setDirection(ListView::Direction::HORIZONTAL);
			l_item->pushBackCustomItem(item_clone);
			l_gift->pushBackCustomItem(l_item);
		}
		else
		{
			l_item->pushBackCustomItem(item_clone);
		}
	}

	if (l_gift->getChildrenCount() < 1)
	{
		auto p_no = dynamic_cast<Widget*>(Helper::seekWidgetByName(giftView, "panel_no"));
		p_no->setVisible(true);
	}
	auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(giftView,"image_pulldown"));
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	addListViewBar(l_gift, image_pulldown);
	auto b_fire_no = giftView->getChildByName<Button*>("button_fire_no");
	b_fire_no->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent,this));
	auto b_fire_yes = giftView->getChildByName<Button*>("button_fire_yes");
	b_fire_yes->addTouchEventListener(CC_CALLBACK_2(UITavern::barGirlButtonEvent,this));
}
//领取奖励界面
void UITavern::openGetRewardDialog(const HandleTaskResult* result)
{
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushPlayerAddExpOrFrame(result->exp, result->fame, result->rewardexp, result->rewardfame);

	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto t_title = view->getChildByName<Text*>("label_title");
	auto p_result = view->getChildByName<Widget*>("panel_result");
	auto l_result = p_result->getChildByName<ListView*>("listview_result");
	auto p_item = p_result->getChildByName<Widget*>("panel_item_1");
	auto p_force = p_result->getChildByName<Widget*>("panel_sailor");
	auto p_coin = p_result->getChildByName<Widget*>("panel_coin");
	auto i_div = p_result->getChildByName<ImageView*>("image_div_1");

	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_REWARD_TITLE"]);
	if (result->n_rewarditems > 0 && result->rewarditems[0]->itemid)
	{
		auto p_item_clone = p_item->clone();
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_item_clone,"image_item"));
		auto t_item = p_item_clone->getChildByName<Text*>("label_items_name");
		auto t_num = p_item_clone->getChildByName<Text*>("label_num");
		std::string itemName;
		std::string itemPath;
		getItemNameAndPath(result->rewarditems[0]->itemtype, result->rewarditems[0]->itemid, itemName, itemPath);
		int amount = result->rewarditems[0]->amount;
		i_item->ignoreContentAdaptWithSize(false);
		i_item->loadTexture(itemPath);
		t_item->setString(itemName);
		t_num->setString(StringUtils::format("+ %d", amount));
		l_result->pushBackCustomItem(p_item_clone);
		auto i_div1 = i_div->clone();
		l_result->pushBackCustomItem(i_div1);
		if (result->n_rewarditems > 1 && result->rewarditems[1]->itemid)
		{
			auto p_item_clone = p_item->clone();
			auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_item_clone, "image_item"));
			auto t_item = p_item_clone->getChildByName<Text*>("label_items_name");
			auto t_num = p_item_clone->getChildByName<Text*>("label_num");
			getItemNameAndPath(result->rewarditems[0]->itemtype, result->rewarditems[0]->itemid, itemName, itemPath);
			int amount = result->rewarditems[0]->amount;
			i_item->ignoreContentAdaptWithSize(false);
			i_item->loadTexture(itemPath);
			t_item->setString(itemName);
			t_num->setString(StringUtils::format("+ %d", amount));
			l_result->pushBackCustomItem(p_item_clone);
			auto i_div1 = i_div->clone();
			l_result->pushBackCustomItem(i_div1);
		}
	}

	if (result->forceid)
	{
		auto p_force_clone = p_force->clone();
		auto t_title = p_force_clone->getChildByName<Text*>("label_force_relation");
		auto i_force = p_force_clone->getChildByName<ImageView*>("image_silver_2");
		auto t_num = p_force_clone->getChildByName<Text*>("label_buy_num");
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_REWARD_FORCE_RELATION"]);
		i_force->ignoreContentAdaptWithSize(false);
		i_force->loadTexture(getCountryIconPath(result->forceid));
		t_num->setString(StringUtils::format("+ %d", result->forceaffect));
		t_num->setTextColor(Color4B(40, 25, 13, 255));
		l_result->pushBackCustomItem(p_force_clone);
		auto i_div2 = i_div->clone();
		i_force->setPositionX(t_num->getPositionX() - t_num->getContentSize().width - i_force->getContentSize().width / 2 - 16);
		l_result->pushBackCustomItem(i_div2);
	}

	if (result->rewardcoin)
	{
		auto p_silver_clone = p_coin->clone();
		auto i_silver = p_silver_clone->getChildByName<ImageView*>("image_silver");
		auto t_silver_num = p_silver_clone->getChildByName<Text*>("label_buy_num");
		std::string str = numSegment(StringUtils::format("%lld", result->rewardcoin));
		t_silver_num->setString(StringUtils::format("+%s", str.c_str()));
		l_result->pushBackCustomItem(p_silver_clone);
		auto i_div3 = i_div->clone();
		l_result->pushBackCustomItem(i_div3);
	}
	
	if (result->rewardgold)
	{
		auto p_gold_clone = p_coin->clone();
		auto i_gold = p_gold_clone->getChildByName<ImageView*>("image_silver");
		auto t_gold_num = p_gold_clone->getChildByName<Text*>("label_buy_num");
		i_gold->ignoreContentAdaptWithSize(false);
		i_gold->loadTexture(getVticketOrCoinPath(10000, 1));
		std::string str = numSegment(StringUtils::format("%lld", result->rewardgold));
		t_gold_num->setString(StringUtils::format("+%s",str.c_str()));
		l_result->pushBackCustomItem(p_gold_clone);
		auto i_div4 = i_div->clone();
		l_result->pushBackCustomItem(i_div4);
	}
	auto b_yes = view->getChildByName<Button*>("button_result_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UITavern::taskButtonEvent,this));
}
//交互框移动
bool UITavern::moveTo(Node* target,const Point& pos)
{
	closeBarGirlChat();
	float time = 0;
	target->setPosition(ENDPOS3);
	target->runAction(Sequence::create(DelayTime::create(time),MoveTo::create(0.4f,pos),nullptr));
	return true;
}
//逐字显示文本的方法
void UITavern::showDialogWordText(float t)
{
	std::string	 text = chatTxt->getString();
	std::string showT = "";
	plusNum = chatGetUtf8CharLen(chatContent.c_str() + lenAfter);
	
	if (wordLenBoo == true)
	{
		wordLenBoo = false;
		lenAfter = lenNum + text.length() - plusNum;
	}
	else
	{
		lenAfter = lenNum + text.length();
	}
	
	lenAfter = lenAfter + plusNum;
	maxLen = chatContent.length() + 1;
	showT = chatContent.substr(lenNum, lenAfter - lenNum);
	chatTxt->setString(showT);
	//log("height = %f size*3 = %d", getLabelHight(showT, chatTxt->getBoundingBox().size.width, chatTxt->getFontName(), chatTxt->getFontSize()), chatTxt->getFontSize() * 3);
	if (lenAfter >= maxLen - 1 || getLabelHight(showT, chatTxt->getBoundingBox().size.width, chatTxt->getSystemFontName(), chatTxt->getSystemFontSize()) > chatTxt->getSystemFontSize() * 3)
	{
		this->unschedule(schedule_selector(UITavern::showDialogWordText));
		if (getLabelHight(showT, chatTxt->getBoundingBox().size.width, chatTxt->getSystemFontName(), (chatTxt->getSystemFontSize() + 1)) > chatTxt->getSystemFontSize() * 3 && lenAfter < maxLen - 1)
		{
			this->scheduleOnce(schedule_selector(UITavern::anchorAction), 0);
			wordLenBoo = true;
			showT = chatContent.substr(lenNum, lenAfter - lenNum - plusNum);
			chatTxt->setString(showT);
			lenNum = lenAfter - lenNum - plusNum;
			//log("showT = %s lenNum = %d lenAfter = %d",showT.c_str(),lenNum,lenAfter);
		}
		else
		{
			lenNum = 0;
			chatTxt = nullptr;
			guardDiaolgOver = true; 
			m_nCurDialogPage = m_nToTalDialogPage;
			
			//与酒吧女郎友好度增加时的特效
			if (m_nChatContentIndex == CONTENT_ACCEPT_DRINK || m_nChatContentIndex == CONTENT_ACCEPT_GIFT)
			{
				UITavern::showFavorDegreeAction();
			}
			if (m_nViewIndex == UI_CHAT)
			{
				favorDegreeActionPlayed = false;
				bargirlClickBoo = true;
			}
		}
	}
}
//逐字显示文本的定时器
void UITavern::showDialogWordSchedule()
{
	this->schedule(schedule_selector(UITavern::showDialogWordText), 0.1 / 3);
}
//好感度动作
void UITavern::showFavorDegreeAction()
{
	favorDegreeActionPlayed = true;
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_USED_PROP_21);
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	auto i_heart = i_bargirl->getChildByName<ImageView*>("image_f");
	i_heart->setScale(1, 1);
	i_heart->setPosition(Vec2(270,204));
	auto originalHeight = i_heart->getBoundingBox().size.height;
	auto moveDistance = 0;
	if (m_nChatContentIndex == CONTENT_ACCEPT_DRINK)
	{
		i_heart->setScale(0.6,0.6);
		auto y = 204 - (originalHeight - i_heart->getBoundingBox().size.height);
		i_heart->setPositionY(y);
		moveDistance = i_bargirl->getContentSize().height - i_heart->getPositionY() + i_heart->getBoundingBox().size.height / 3;
		i_heart->runAction(EaseSineIn::create(Sequence::create(Spawn::createWithTwoActions(FadeIn::create(0.5), MoveBy::create(1.2, Vec2(0, moveDistance * 0.5))), Spawn::createWithTwoActions(FadeOut::create(0.6), MoveBy::create(1.2, Vec2(0, moveDistance * 0.5))), nullptr)));
	}
	else if (m_nChatContentIndex == CONTENT_ACCEPT_GIFT)
	{
		if (m_nlikeRecievedGift == 1)
		{
			//收到合心意的礼物
			m_nlikeRecievedGift = 0;
			i_heart->setScale(0.5, 0.5);
			moveDistance = i_bargirl->getContentSize().height - i_heart->getPositionY() + i_heart->getBoundingBox().size.height / 3; 
			i_heart->setVisible(false);
			int offset = 50;
			Node * heart_parent = i_heart->getParent();
			for (int i = 0; i < 9; i++)
			{
				auto new_heart = i_heart->clone();
				new_heart->setVisible(true);
				heart_parent->addChild(new_heart);
				int tmp = cocos2d::random(0, 2);
				int num = (i + 1) / 2;
				if (i == 0){
				}
				else{
					if (i % 2 == 0){
						new_heart->setPositionX(new_heart->getPositionX() + offset * num);
					}
					else{
						new_heart->setPositionX(new_heart->getPositionX() - offset * num);
					}
				}

				auto action1 = EaseSineIn::create(Sequence::create(Spawn::createWithTwoActions(FadeIn::create(0.5), MoveBy::create(1.2, Vec2(0, moveDistance * 0.5))), Spawn::createWithTwoActions(FadeOut::create(0.6), MoveBy::create(1.2, Vec2(0, moveDistance * 0.5))), nullptr));
				new_heart->runAction(Sequence::createWithTwoActions(DelayTime::create(0.3 * i), action1));
			}
		}
		else
		{
			//收到不合心意的礼物
			i_heart->setScale(0.5, 0.5);
			moveDistance = i_bargirl->getContentSize().height - i_heart->getPositionY() + i_heart->getBoundingBox().size.height / 3;
			i_heart->setVisible(false);
			int offset = 60;
			Node * heart_parent = i_heart->getParent();
			for (int i = 0; i < 5; i++)
			{
				auto new_heart = i_heart->clone();
				new_heart->setVisible(true);
				heart_parent->addChild(new_heart);
				int tmp = cocos2d::random(0, 2);
				int num = (i + 1) / 2;
				if (i == 0){
				}
				else{
					if (i % 2 == 0){
						new_heart->setPositionX(new_heart->getPositionX() + offset * num);
					}
					else{
						new_heart->setPositionX(new_heart->getPositionX() - offset * num);
					}
				}
				
				auto action1 = EaseSineIn::create(Sequence::create(Spawn::createWithTwoActions(FadeIn::create(0.5), MoveBy::create(1.2, Vec2(0, moveDistance * 0.5))), Spawn::createWithTwoActions(FadeOut::create(0.6), MoveBy::create(1.2, Vec2(0, moveDistance * 0.5))), nullptr));
				new_heart->runAction(Sequence::createWithTwoActions(DelayTime::create(0.3 * i), action1));
			}
		}
	}
	//判断更新好感度状态特效
	if (m_pSpriteForFavorAnimate)
	{
		m_pSpriteForFavorAnimate->removeFromParentAndCleanup(true);
		m_pSpriteForFavorAnimate = nullptr;
	}
	showFavorStatus();
}
//进入聊天界面时 好感度状态等级显示
void UITavern::showFavorStatus()
{
	int favorStatus = 1;
	if (m_nFavour < FAVOUR_MIN)
	{
		//0-49
		favorStatus = 1;
	}
	else if (m_nFavour < FAVOUR_SIDE_TASK)
	{
		//50-199
		favorStatus = 2;
	}
	else if (m_nFavour < FAVOUR_MAX)
	{
		//200-299
		favorStatus = 3;
	}
	else
	{
		//300
		favorStatus = 4;
	}
	//log("m_nFavour = %d favorStatus = %d", m_nFavour,favorStatus);
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	
	if (!m_pSpriteForFavorAnimate)
	{
		m_pSpriteForFavorAnimate = Sprite::create();
		i_bargirl->addChild(m_pSpriteForFavorAnimate, -1);
	}
	Sprite *i_bargirl_glow;
	if (favorStatus >2)
	{
		i_bargirl_glow = Sprite::create();
		i_bargirl_glow->setTexture("cocosstudio/login_ui/tarven_720/bagirl_glow.png");
		m_pSpriteForFavorAnimate->addChild(i_bargirl_glow);
		i_bargirl_glow->setScale(720/i_bargirl_glow->getContentSize().height);
		i_bargirl_glow->setPosition(Vec2(330,338));
	}
	
	//好感度状态特效
	switch (favorStatus)
	{
		case 1:
			//无特殊效果
			break;
		case 2:
			UITavern::createStarForBargirlFavor(4);
			break;
		case 3:
			UITavern::createStarForBargirlFavor(4);
			i_bargirl_glow->setOpacity(0);
			i_bargirl_glow->runAction(RepeatForever::create(Sequence::create(FadeTo::create(5, 125), FadeTo::create(5, 51), nullptr)));
			break;
		case 4:
			UITavern::createStarForBargirlFavor(9);
			i_bargirl_glow->setOpacity(0);
			i_bargirl_glow->runAction(RepeatForever::create(Sequence::create(FadeTo::create(5, 255), FadeTo::create(5, 51), nullptr)));
			break;
		default:
			break;
	}
}
//好感度状态特效--星星动画
void UITavern::createStarForBargirlFavor(int count)
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon0.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon1.plist");
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 23; i++)
	{
		std::string name = StringUtils::format("blinkstar/eff_blinkstar_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	auto animation_starflush = Animation::createWithSpriteFrames(frames, 1.0 / 20);
	auto animateEff = Animate::create(animation_starflush);
	animation_starflush->setDelayPerUnit(1.0f / 24);
	animation_starflush->setRestoreOriginalFrame(true);
	for (int i = 0; i < count; i++)
	{
		auto new_m_pSpriteAnimate = Sprite::create();
		m_pSpriteForFavorAnimate->addChild(new_m_pSpriteAnimate, -1, i);
		switch (i)
		{
		case 0:
			new_m_pSpriteAnimate->setScale(2.8);
			new_m_pSpriteAnimate->setPosition(Vec2(208, 538));
			break;
		case 1:
			new_m_pSpriteAnimate->setScale(2.2);
			new_m_pSpriteAnimate->setPosition(Vec2(345, 511));
			break;
		case 2:
			new_m_pSpriteAnimate->setScale(2.2);
			new_m_pSpriteAnimate->setPosition(Vec2(45, 417));
			break;
		case 3:
			new_m_pSpriteAnimate->setScale(2.2);
			new_m_pSpriteAnimate->setPosition(Vec2(395, 397));
			break;
		case 4:
			new_m_pSpriteAnimate->setScale(2.8);
			new_m_pSpriteAnimate->setPosition(Vec2(93, 493));
			break;
		case 5:
			new_m_pSpriteAnimate->setScale(2.1);
			new_m_pSpriteAnimate->setPosition(Vec2(382, 452));
			break;
		case 6:
			new_m_pSpriteAnimate->setScale(2.2);
			new_m_pSpriteAnimate->setPosition(Vec2(5, 327));
			break;
		case 7:
			new_m_pSpriteAnimate->setScale(2.2);
			new_m_pSpriteAnimate->setPosition(Vec2(42, 97));
			break;
		case 8:
			new_m_pSpriteAnimate->setScale(1.15);
			new_m_pSpriteAnimate->setPosition(Vec2(483, 320));
			break;
		default:
			break;
		}
		int tmp = cocos2d::random(1, count);
		new_m_pSpriteAnimate->setOpacity(255);
		new_m_pSpriteAnimate->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.2 * tmp), FadeIn::create(0.5f), animateEff, FadeOut::create(0.5f), nullptr)));
	}
}
void UITavern::initDismissSailors()
{
	openView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB],11);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	auto image_item_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_item_bg"));
	auto label_dropitem_tiptext = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_dropitem_tiptext"));
	auto label_title_deposit_or_take = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title_deposit_or_take"));
	auto label_dropitem_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_dropitem_num"));//水手数
	auto button_minus = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_minus"));
	auto button_plus = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_plus"));
	auto button_Slider_no = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_Slider_no"));
	auto button_Slider_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_Slider_yes"));
	auto slider_dropitem_num = dynamic_cast<Slider*>(Helper::seekWidgetByName(view, "slider_dropitem_num"));
	label_dropitem_num->setString(StringUtils::format("X %d", m_nCurSailorNum));
	button_minus->addTouchEventListener(CC_CALLBACK_2(UITavern::dismissSailorButtonEvent, this));
	button_plus->addTouchEventListener(CC_CALLBACK_2(UITavern::dismissSailorButtonEvent, this));
	button_Slider_no->addTouchEventListener(CC_CALLBACK_2(UITavern::dismissSailorButtonEvent, this));
	button_Slider_yes->addTouchEventListener(CC_CALLBACK_2(UITavern::dismissSailorButtonEvent, this));
	slider_dropitem_num->addEventListener(CC_CALLBACK_2(UITavern::dismissSliderChange, this));
	image_item_bg->setVisible(false);
	label_dropitem_tiptext->setVisible(true);
	label_title_deposit_or_take->setString(SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_FIRE_SAILORS_TITLE"]);
	label_dropitem_tiptext->setString(SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_FIRE_SAILORS_CONTENTS"]);
	m_ndissmissSailors = m_nCurSailorNum;
}
void UITavern::dismissSailorButtonEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	auto viewDismiss = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	if (!viewDismiss)
	{
		return;
	}

	auto slider_dropitem_num = dynamic_cast<Slider*>(Helper::seekWidgetByName(viewDismiss, "slider_dropitem_num"));
	auto label_dropitem_num = dynamic_cast<Text*>(Helper::seekWidgetByName(viewDismiss, "label_dropitem_num"));//水手数
	if (!viewDismiss)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//增加
	if (isButton(button_plus))
	{
		if (m_ndissmissSailors >= m_nCurSailorNum)
		{
			return;
		}
		m_ndissmissSailors++;
		slider_dropitem_num->setPercent(m_ndissmissSailors*100.0 / m_nCurSailorNum);
		label_dropitem_num->setString(StringUtils::format("X %d", m_ndissmissSailors));
		return;
	}
	//减少
	if (isButton(button_minus))
	{
		if (m_ndissmissSailors <= 1)
		{
			return;
		}
		m_ndissmissSailors--;
		slider_dropitem_num->setPercent(m_ndissmissSailors*100.0 / m_nCurSailorNum);
		label_dropitem_num->setString(StringUtils::format("X %d", m_ndissmissSailors));
		return;
	}
	//确定解雇
	if (isButton(button_Slider_yes))
	{
		ProtocolThread::GetInstance()->tarvenFireSailors(m_ndissmissSailors, UILoadingIndicator::create(this));
		closeView();
		return;
	}
	if (isButton(button_Slider_no))
	{
		closeView();
		return;
	}
}
void UITavern::dismissSliderChange(Ref* obj, cocos2d::ui::Slider::EventType type)
{
	if (type != cocos2d::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		return;
	}
	auto viewDismiss = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	if (!viewDismiss)
	{
		return;
	}
	auto slider_dropitem_num = dynamic_cast<Slider*>(Helper::seekWidgetByName(viewDismiss, "slider_dropitem_num"));
	auto label_dropitem_num = dynamic_cast<Text*>(Helper::seekWidgetByName(viewDismiss, "label_dropitem_num"));//水手数
	m_ndissmissSailors = m_nCurSailorNum*slider_dropitem_num->getPercent()/100.0;
	label_dropitem_num->setString(StringUtils::format("X %d", m_ndissmissSailors));
	if (m_ndissmissSailors <= 1)//最少数量为1
	{
		label_dropitem_num->setString("x 1");
		m_ndissmissSailors = 1;
	}
}