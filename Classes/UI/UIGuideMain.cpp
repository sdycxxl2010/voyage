#include "UIGuideMain.h"
#include "ui/CocosGUI.h"
#include"cocostudio/CocoStudio.h"
#include "UIGuidePalace.h"
#include "TVSceneLoader.h"
#include "UIGuidePort.h"
#include "UIGuideShipyard.h"
#include "UIGuideTarven.h"

#include "Utils.h"
#include "UICommon.h"
#include "TVLoadingScene.h"
#include "UIMain.h"
#include "UINoviceStoryLine.h"
UIGuideMain::UIGuideMain() :
t_size(0),
m_guidestage(0),
m_infoIndex(0),
m_chatcomplete(false),
m_taskflag(false),
sprite_hand(nullptr),
m_cursay(false),
m_tasklayer(nullptr),
m_rewardInfo(nullptr),
m_draw(nullptr),
m_roleLevel(0),
m_receiveexp(0),
m_receiveRp(0),
m_receiveCoin(0),
m_receiveVp(0),
m_isfirstIn(true),
m_allChatNum(0),
m_chatnum(0),
m_contentLength(0),
m_isLevelup(false),
m_lenAfter(0),
m_textTime(0),
m_taskAlreadyShow(false),
m_cityResult(nullptr),
m_chatBg(nullptr),
m_LoadingLayer(nullptr),
m_dialogLayer(nullptr)
{
	m_words.clear();
	m_dispalys.clear();
	talks.clear();
	m_dialogdatas.clear();
	m_names.clear();
	m_eUIType = UI_PORT;
	m_rewardInfo = new REWARD_INFO;
	m_bIsClickedShipyard = false;
}
UIGuideMain::~UIGuideMain()
{
	m_dispalys.clear();
	talks.clear();
	for (auto dialog : m_dialogdatas)
	{
		delete dialog;
		dialog = nullptr;
	}
	m_dialogdatas.clear();
	m_names.clear();
	m_words.clear();
	delete m_rewardInfo;
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
	
}
UIGuideMain* UIGuideMain::create()
{
	UIGuideMain * guide = new UIGuideMain;
	if (guide &&guide->init())
	{
		guide->autorelease();
		return guide;
	}
	CC_SAFE_DELETE(guide);
	return nullptr;
}
bool UIGuideMain::init()
{
	if(!Node::init())
	{
		return false;
	}
	m_LoadingLayer = UILoadingIndicator::create(this, m_eUIType);
	this->addChild(m_LoadingLayer);
	ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIGuideMain::onServerEvent, this), this);
	confirmGuideStage();
	ProtocolThread::GetInstance()->getCurrentCityData(UILoadingIndicator::create(this, m_eUIType));
	
	return true;
}

void UIGuideMain::onEnter()
{
	UIBasicLayer::onEnter();
}
void UIGuideMain::onExit()
{
	UIBasicLayer::onExit();
}

void UIGuideMain::guide()
{
		m_guidestage++;
		auto i_citybg = m_vRoot->getChildByName<Widget*>("image_main_city_bg");
		auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
		auto b_task = m_vRoot->getChildByName<Widget*>("button_task");
		int guidelevel = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());
		setButtonsDisable(i_citybg);
		if (guidelevel < 5)
		{
			partOneGuide(m_guidestage);
			return;
		}
		switch (m_guidestage)
		{
		case TASK_BEGIN:
		{
						 
						   focusOnButton(b_task);
						   showMark(true);
						   m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 4;
						   b_task->setTouchEnabled(true);

		}
			break;
		case OPEN_TASK:
		{
						  showNotify(true);
						  auto b_reward = m_tasklayer->getChildByName<Button*>("button_get_reward");
						  setButtonsDisable(b_reward->getParent());
						  focusOnButton(b_reward);
						  showMark(false);
						  b_reward->setTouchEnabled(true);

		}
			break;
		case REWARD_CONFIRM:
		{
						   initRewardInfo(m_infoIndex);
						   auto view = getViewRoot(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
 						   auto b_yes = view->getChildByName<Button*>("button_result_yes");
 						   setButtonsDisable(m_vRoot);
 						   setButtonsDisable(m_tasklayer);
 						   b_yes->setTouchEnabled(true);
						   focusOnButton(b_yes);
						   auto root = getViewRoot(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());				
						   auto b_getReward = root->getChildByName<Button*>("button_get_reward");
						   b_getReward->setVisible(false);
						   showNotify(false);
					   
		}
			break;
		case GET_REWARD:
		{			
						   if (SINGLE_HERO->m_iStage == -1)
						   {
							   focusOnButton(b_task);
							   showMark(true);
							   m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 5;
							   b_task->setTouchEnabled(true);
						   }
						   else
						   {
							   int64_t a[4] = { SINGLE_HERO->m_iExp, SINGLE_HERO->m_iRexp, SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp };
							   saveUserData(a);
							   UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 6);
							   if (m_isLevelup)
							   {
								   m_isLevelup = false;
								   return;
							   }
							   guide();
						   }
		}
			break;
		case TASK_END:
		{
						 m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 5;
						 openTaskLayer(m_infoIndex);
						 auto b_close = m_tasklayer->getChildByName<Button*>("button_back");
						 setButtonsDisable(b_close->getParent());
						 focusOnButton(b_close);
						 b_close->setTouchEnabled(true);
						 auto b_getReward = m_tasklayer->getChildByName<Button*>("button_get_reward");
						 b_getReward->setVisible(false);
						 taskBlink();
		}
			break;
		case NPC_FADEIN:
		{
						   showMark(false);
						   sprite_hand->setVisible(false);
						   m_dialogLayer = UINoviceStoryLine::GetInstance();
						   this->addChild(m_dialogLayer, 10000);
						   m_dialogLayer->setVisible(true);
						   m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::PALACE_DIALOG, 0);
		}
			break;
		case OPEN_PALACE:
		{
							m_dialogLayer->removeFromParentAndCleanup(true);
							guard_dialog->setTouchEnabled(false);
							sprite_hand->setVisible(true);
							auto  palace = i_citybg->getChildByName<Button*>("button_cityhall");							
							auto cityHall = i_citybg->getChildByName<Button*>("button_palace");
							setButtonsDisable(i_citybg);
							if (palace->isVisible())
							{
								palace->setTouchEnabled(true);
								focusOnButton(palace);
							}
							else
							{
								cityHall->setTouchEnabled(true);
								focusOnButton(cityHall);
							}

		}
			break;
		case SELL_TASK_BEGIN:
		{							 
								focusOnButton(b_task);
								showMark(true);
								m_infoIndex = m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 5;;
								b_task->setTouchEnabled(true);

		}
			break;
		case OPEN_LISENCE_REWARD_TASK:
		{
		
								auto b_reward = m_tasklayer->getChildByName<Button*>("button_get_reward");
								setButtonsDisable(b_reward->getParent());
								focusOnButton(b_reward);
								showMark(false);
								b_reward->setTouchEnabled(true);
								showNotify(true);
		}
			break;
		case LISENCE_REWARD_CONFIRM:
		{
								initRewardInfo(m_infoIndex);
								auto root = getViewRoot(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());
								auto view = getViewRoot(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
								auto b_yes = view->getChildByName<Button*>("button_result_yes");
								auto b_getReward = root->getChildByName<Button*>("button_get_reward");
								b_getReward->setVisible(false);

								showNotify(false);

								setButtonsDisable(m_vRoot);
								setButtonsDisable(m_tasklayer);
							    b_yes->setTouchEnabled(true);
							    focusOnButton(b_yes);

		}
			break;
		case GET_LISEBCE_REWARD:
		{

								   if (SINGLE_HERO->m_iStage == -1)
								   {
									   focusOnButton(b_task);
									   showMark(true);
									   m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 6;
									   b_task->setTouchEnabled(true);
								   }
								   else
								   {
									   int64_t a[4] = { SINGLE_HERO->m_iExp, SINGLE_HERO->m_iRexp, SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp };
									   saveUserData(a);
									   UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 7);
									   if (m_isLevelup)
									   {
										   m_isLevelup = false;
										   return;
									   }
									   guide();
								   }
								
		}
			break;
		case LISEBCE_TASK_END:
		{
								 m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 6;
								 openTaskLayer(m_infoIndex);
								 auto b_close = m_tasklayer->getChildByName<Button*>("button_back");
								 setButtonsDisable(b_close->getParent());
								 focusOnButton(b_close);
								 b_close->setTouchEnabled(true);
								 auto b_getReward = m_tasklayer->getChildByName<Button*>("button_get_reward");
								 b_getReward->setVisible(false);
								 taskBlink();
		}
			break;
		case NPC_SELL_FADEIN:
		{
						
								showMark(false);
								sprite_hand->setVisible(false);
								m_dialogLayer = UINoviceStoryLine::GetInstance();
								this->addChild(m_dialogLayer, 10000);
								m_dialogLayer->setVisible(true);
								m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::MARKET_SELL_DIALOG, 0);
		}
			break;
		case OPEN_EXCHANGE:
		{						
							  m_dialogLayer->removeFromParentAndCleanup(true);
							  guard_dialog->setTouchEnabled(false);
							  sprite_hand->setVisible(true);
							  auto  exChange = i_citybg->getChildByName<Button*>("button_market");
							  exChange->setTouchEnabled(true);
							  focusOnButton(exChange);

		}
			break;
		case SELL_TASK_END:
		{

							  focusOnButton(b_task);
							  showMark(true);
							  m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 6;
							  b_task->setTouchEnabled(true);
		}
			break;
		case OPEN_END_SELL_TASK:
		{
							  showNotify(true);
							  auto b_reward = m_tasklayer->getChildByName<Button*>("button_get_reward");
							  setButtonsDisable(b_reward->getParent());
							  focusOnButton(b_reward);
							  showMark(false);
							  b_reward->setTouchEnabled(true);
		}
			break;
		case CONFIRM_END_SELL_REWARD:
		{
							  initRewardInfo(m_infoIndex);
							  auto view = getViewRoot(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
							  auto b_yes = view->getChildByName<Button*>("button_result_yes");
							  setButtonsDisable(m_vRoot);
							  setButtonsDisable(m_tasklayer);
							  b_yes->setTouchEnabled(true);
							  focusOnButton(b_yes);
							  auto root = getViewRoot(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());
							  auto b_getReward = root->getChildByName<Button*>("button_get_reward");
							  b_getReward->setVisible(false);
							  showNotify(false);
		}
			break;

		case GET_END_SELL_TASK_REWARD:
		{
							  int64_t a[4] = { SINGLE_HERO->m_iExp, SINGLE_HERO->m_iRexp, SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp };
							  saveUserData(a);
							  UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 8);
							  guide();
		}
			break;
		case SKILL_GUIDE_DIALOG:
		{
								   sprite_hand->setVisible(false);
								   m_dialogLayer = UINoviceStoryLine::GetInstance();
								   this->addChild(m_dialogLayer, 10000);
								   m_dialogLayer->setVisible(true);
								   m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::CENTER_ONE_DIALOG, 0);
		}
			break;
		case FOCUS_CHARACTER_CENTER:
		{
							  m_dialogLayer->removeFromParentAndCleanup(true);
							  setButtonsDisable(i_citybg);
							  auto i_head = i_citybg->getChildByName<ImageView*>("image_head");
							  i_head->setTouchEnabled(true);
							  i_head->addTouchEventListener(CC_CALLBACK_2(UIGuideMain::menuCall_func, this));
							  focusOnButton(i_head);

		}
		default:
			break;
		}
	    
}
void UIGuideMain::partOneGuide(int stage)
{
	auto view = getViewRoot("cocosstudio/voyageUI_city.csb");
	auto i_citybg = view->getChildByName<Widget*>("image_main_city_bg");
	auto guard_dialog = view->getChildByName<Widget*>("panel_dialog");
	auto b_task = view->getChildByName<Widget*>("button_task");
	switch (stage)
	{
	case NEW_TASK_BEGAN:
	{
						   focusOnButton(b_task);
						   showMark(true);
						   m_infoIndex = (SINGLE_HERO->m_iNation -1) * 6 + 1;
						   b_task->setTouchEnabled(true);
	}
		break;
	case NEW_OPEN_TASK:
	{
						  auto b_close = m_tasklayer->getChildByName<Button*>("button_back");
						  setButtonsDisable(b_close->getParent());
						  focusOnButton(b_close);
						  b_close->setTouchEnabled(true);
						  auto b_getReward = m_tasklayer->getChildByName<Button*>("button_get_reward");
						  b_getReward->setVisible(false);
						  taskBlink();
	}
		break;
	case NEW_TASK_CLOSE:
	{
						   auto b_shipyard = view->getChildByName<Button*>("button_shipyard");
						   focusOnButton(b_shipyard);
						   showMark(false);
						   b_shipyard->setTouchEnabled(true);		
	}
		break;
	case NEW_OPEN_SHIPYARD_TASK:
	{
							focusOnButton(b_task);
							showMark(true);
							m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 1;
							b_task->setTouchEnabled(true);
	}
		break;
	case NEW_GET_SHIPYARD_TASK_REWARD:
	{
							showNotify(true);
							auto b_reward = m_tasklayer->getChildByName<Button*>("button_get_reward");
							setButtonsDisable(b_reward->getParent());
							focusOnButton(b_reward);
							showMark(false);
							b_reward->setTouchEnabled(true);
	}
		break;
	case NEW_CONFIRM_GET_SHIPYARD_TASK_REWARD:
	{
							auto root = getViewRoot(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());
							auto view = getViewRoot(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
							auto b_yes = view->getChildByName<Button*>("button_result_yes");
							auto b_getReward = root->getChildByName<Button*>("button_get_reward");
							b_getReward->setVisible(false);
							initRewardInfo(m_infoIndex);
							showNotify(false);
							setButtonsDisable(m_vRoot);
							setButtonsDisable(m_tasklayer);
							b_yes->setTouchEnabled(true);
							focusOnButton(b_yes);
	}
		break;

	case NEW_SHIPYARD_TASK_END:
	{
						    if (SINGLE_HERO->m_iStage == -1)
						    {
								focusOnButton(b_task);
								showMark(true);
								m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 2;
								b_task->setTouchEnabled(true);
							}
							else
							{
								int64_t a[4] = { SINGLE_HERO->m_iExp, SINGLE_HERO->m_iRexp, SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp };
								saveUserData(a);
								UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 2);
								guide();
							}

							
	}
		break;
	case NEW_BAR_TASK_BEGIN:
	{
							   m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 2;
							   openTaskLayer(m_infoIndex);
							   auto b_close = m_tasklayer->getChildByName<Button*>("button_back");
							   setButtonsDisable(b_close->getParent());
							   focusOnButton(b_close);
							   b_close->setTouchEnabled(true);
							   auto b_getReward = m_tasklayer->getChildByName<Button*>("button_get_reward");
							   b_getReward->setVisible(false);
							   taskBlink();
	}
		break;
	case NEW_NPC_FADEIN:
	{
						   showMark(false);
						   sprite_hand->setVisible(false);
 						   this->addChild(m_dialogLayer, 10000);
 						   m_dialogLayer->setVisible(true);
						   m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::TARVEN_ONE_DIALOG, 0);
						   
	}
		break;
	case NEW_OPEN_BAR:
	{
						 m_dialogLayer->setVisible(false);
						 m_dialogLayer->removeFromParentAndCleanup(true);
						 showMark(false);
						 auto b_bar = view->getChildByName<Widget*>("button_tavern");
						 focusOnButton(b_bar);
						 b_bar->setTouchEnabled(true);
	}
		break;
	case NEW_OPEN_BAR_TASK:
	{
// 							  m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 2;
// 							  openTaskLayer(m_infoIndex);
// 							  auto b_close = m_tasklayer->getChildByName<Button*>("button_back");
// 							  setButtonsDisable(b_close->getParent());
// 							  focusOnButton(b_close);
// 							  b_close->setTouchEnabled(true);
// 							  auto b_getReward = m_tasklayer->getChildByName<Button*>("button_get_reward");
// 							  b_getReward->setVisible(false);
// 							  taskBlink();
						focusOnButton(b_task);
						showMark(true);
						m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 2;
						b_task->setTouchEnabled(true);
	}
		break;
	case NEW_GET_BAR_TASK_REWARD:
	{
						showNotify(true);
						auto b_reward = m_tasklayer->getChildByName<Button*>("button_get_reward");
						setButtonsDisable(b_reward->getParent());
						focusOnButton(b_reward);
						showMark(false);
						b_reward->setTouchEnabled(true);
	}
		break;
	case NEW_CONFIRM_GET_BAR_TASK_REWARD:
	{
							auto root = getViewRoot(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());
							auto view = getViewRoot(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
							auto b_yes = view->getChildByName<Button*>("button_result_yes");
							auto b_getReward = root->getChildByName<Button*>("button_get_reward");
							b_getReward->setVisible(false);
							initRewardInfo(m_infoIndex);
							showNotify(false);
							setButtonsDisable(m_vRoot);
							setButtonsDisable(m_tasklayer);
							b_yes->setTouchEnabled(true);
							focusOnButton(b_yes);
	}
		break;
	case NEW_BAR_TASK_END:
	{
							 if (SINGLE_HERO->m_iStage == -1)
							 {
								 focusOnButton(b_task);
								 showMark(true);
								 m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 3;
								 b_task->setTouchEnabled(true);
							 }else
							 {
								 int64_t a[4] = { SINGLE_HERO->m_iExp, SINGLE_HERO->m_iRexp, SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp };
								 saveUserData(a);
								 UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 3);
								 guide();
							 }
	}
		break;
	case NEW_EXCHANGE_TASK_BEGIN:
	{
							m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 3;
							openTaskLayer(m_infoIndex);
							auto b_close = m_tasklayer->getChildByName<Button*>("button_back");
							setButtonsDisable(b_close->getParent());
							focusOnButton(b_close);
							b_close->setTouchEnabled(true);
							auto b_getReward = m_tasklayer->getChildByName<Button*>("button_get_reward");
							b_getReward->setVisible(false);
							taskBlink();
	}
		break;
	case NEW_EXCHAGE_NPC_FADEIN:
	{
						//guide();
						    showMark(false);
							sprite_hand->setVisible(false);
							m_dialogLayer = UINoviceStoryLine::GetInstance();
							this->addChild(m_dialogLayer, 10000);
							m_dialogLayer->setVisible(true);
							m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::MARKET_BUY_DIALOG, 0);
	}
		break;
	case NEW_OPEN_EXCHANGE:
	{
					    m_dialogLayer->setVisible(false);
						m_dialogLayer->removeFromParentAndCleanup(true);
						showMark(false);
						auto b_market = view->getChildByName<Widget*>("button_market");
						focusOnButton(b_market);
						b_market->setTouchEnabled(true);
	}
		break;
	case 	NEW_OPEN_EXCHANGE_TASK:
	{
						focusOnButton(b_task);
						showMark(true);
						m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 3;
						b_task->setTouchEnabled(true);
	}
		break;
	case 	NEW_GET_EXCHANGE_TASK_REWARD:
	{
						showNotify(true);
						auto b_reward = m_tasklayer->getChildByName<Button*>("button_get_reward");
						setButtonsDisable(b_reward->getParent());
						focusOnButton(b_reward);
						showMark(false);
						b_reward->setTouchEnabled(true);
	}
		break;
	case NEW_CONFIRM_GET_EXCHANGE_TASK_REWARD:
	{
						auto root = getViewRoot(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());
						auto view = getViewRoot(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
						auto b_yes = view->getChildByName<Button*>("button_result_yes");
						auto b_getReward = root->getChildByName<Button*>("button_get_reward");
						b_getReward->setVisible(false);
						initRewardInfo(m_infoIndex);
						showNotify(false);
						setButtonsDisable(m_vRoot);
						setButtonsDisable(m_tasklayer);
						b_yes->setTouchEnabled(true);
						focusOnButton(b_yes);
	}
		break;
	case NEW_EXCHAGE_TASK_END:
	{
						if (SINGLE_HERO->m_iStage == -1)
						{
							focusOnButton(b_task);
							showMark(true);
							m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 4;
							b_task->setTouchEnabled(true);
						}
						else
						{
							int64_t a[4] = { SINGLE_HERO->m_iExp, SINGLE_HERO->m_iRexp, SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp };
							saveUserData(a);
							UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 4);
							guide();
						}

	}
		break;
	case NEW_SEA_TASK_BEGIN:
	{
						m_infoIndex = (SINGLE_HERO->m_iNation - 1) * 6 + 4;
						openTaskLayer(m_infoIndex);
						auto b_close = m_tasklayer->getChildByName<Button*>("button_back");
						setButtonsDisable(b_close->getParent());
						focusOnButton(b_close);
						b_close->setTouchEnabled(true);
						auto b_getReward = m_tasklayer->getChildByName<Button*>("button_get_reward");
						b_getReward->setVisible(false);
						if (m_isLevelup)
						{
							m_isLevelup = false;
							return;
						}
						else
						{
							focusOnButton(b_close);		
							taskBlink();
						}
	}
		break;
	case NEW_SEA_NPC_FADEIN:
	{
						showMark(false);
						sprite_hand->setVisible(false);
						m_dialogLayer = UINoviceStoryLine::GetInstance();
						this->addChild(m_dialogLayer, 10000);
						m_dialogLayer->setVisible(true);
						m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::DOCK_ONE_DIALOG, 0);
	}
		break;
	case NEW_OPEN_DOCK:
	{
						m_dialogLayer->setVisible(false);
						m_dialogLayer->removeFromParentAndCleanup(true);
						showMark(false);
						auto b_dock = view->getChildByName<Widget*>("button_dock");
						focusOnButton(b_dock);
						b_dock->setTouchEnabled(true);
//						SINGLE_HERO->m_iStage = 2;
	}
	default:
		break;
	}
}

void UIGuideMain::initf(float dt)
{
	openView("cocosstudio/voyageUI_city.csb");
	m_vRoot = getViewRoot("cocosstudio/voyageUI_city.csb");
	setButtonsDisable(m_vRoot);
	this->setUIType(UI_VIEW_TYPE::UI_PORT);
	SINGLE_AUDIO->setBackgroundMusicON(true);
	playAudio();
	auto b_task = m_vRoot->getChildByName<Widget*>("button_task");
	b_task->setTouchEnabled(true);
	sprite_hand = Sprite::create();
	sprite_hand->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	sprite_hand->setVisible(false);
	this->addChild(sprite_hand, 2, HAND_TAG);
	auto b_sail = m_vRoot->getChildByName("image_hints_sailor");
	auto b_dua = m_vRoot->getChildByName("image_hints_dua");
	b_sail->setVisible(false);
	b_dua->setVisible(false);
	showBackGround();
	updateUserInfo();
	checkCurStage();
	m_dialogLayer = UINoviceStoryLine::GetInstance();
    m_LoadingLayer->removeFromParentAndCleanup(true);

	auto l_events = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_vRoot, "listview_event"));
	auto b_events = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_events"));
	l_events->setVisible(false);
	b_events->setVisible(false);
}

void UIGuideMain::textShow(float dt)
{
	std::string  s;
	std::string  s1;
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto t_guard = guard_dialog->getChildByName<Text*>("label_guard");
	auto t_content = guard_dialog->getChildByName<Text*>("label_content");
	if (talks.size()>0)
	{
		s =  talks.at(m_allChatNum - m_chatnum);
	}
	else
	{
		s = m_talk;
	}
	if (t_size == 0)
	{
		cutChat(s);
	}
	t_size++;

	std::string	 text = t_content->getString();
	m_lenAfter =  text.length();
	std::string showT = "";
	int  plusNum = chatGetUtf8CharLen(s.c_str() + m_lenAfter);
	m_lenAfter = m_lenAfter + plusNum;
	int maxLen = s.length();
	showT = s.substr(0, m_lenAfter - 0);
	log("%s", showT.c_str());
	log("%d", plusNum);
	t_content->setString(showT);
	if (m_lenAfter >= maxLen)
	{
		t_size = 0;
		m_cursay = true;
		t_content = nullptr;
		m_chatnum--;
		anchorAction();
		unschedule(schedule_selector(UIGuideMain::textShow));
		return;
	}

	if (m_cursay)
	{
		t_content->setText(s);
		m_chatnum--;
		unschedule(schedule_selector(UIGuideMain::textShow));
		anchorAction();
		return;
	}
	
}
void UIGuideMain::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		log("-------OK----------");
		return;
	}

	Widget* widget = dynamic_cast<Widget*>(pSender);
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	callEventByName(widget, widget->getName());
}

void UIGuideMain::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
    UIBasicLayer::onServerEvent(message, msgType);
 	switch (msgType) 	
	{
	case PROTO_TYPE_GetCurrentCityDataResult:
	{
			if (message)
			{
				m_cityResult = (GetCurrentCityDataResult *)message;
				int stagelevel = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());
				if (SINGLE_HERO->m_iStage == 3 && stagelevel == 5)
				{
					SINGLE_HERO->initHeroInfo(m_cityResult->data);
					SINGLE_HERO->m_iSilver = 101450;
					SINGLE_HERO->m_iVp = 0;
					SINGLE_HERO->m_iExp = 790;
					SINGLE_HERO->m_iRexp = 109; 
				}
				else if (SINGLE_HERO->m_iStage == 0)
				{
					SINGLE_HERO->initHeroInfo(m_cityResult->data);
					SINGLE_HERO->m_iSilver = 50000;
					SINGLE_HERO->m_iVp = 0;
					SINGLE_HERO->m_iExp = 0;
					SINGLE_HERO->m_iRexp = 0;
				}
				else if (SINGLE_HERO->m_iStage == -1)
				{
					SINGLE_HERO->initHeroInfo(m_cityResult->data);
					SINGLE_HERO->m_iSilver = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASILVERNUM").c_str());
					SINGLE_HERO->m_iVp = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAGOLDNUM").c_str());
					SINGLE_HERO->m_iExp = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str());
					SINGLE_HERO->m_iRexp = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str());

				}
				initf(0);		
			}
												
	}
		break;
	case PROTO_TYPE_SetMainTaskStatusResult:
 	{
			SetMainTaskStatusResult*result = (SetMainTaskStatusResult*)message;
			if (result->failed == 0)
			{
				return;
			}
		
		
 	}
 		break;

 	default:
		break;
 	}
}

void UIGuideMain::callEventByName(Widget* psender, std::string name)
{
	//对话点击事件
	if (isButton(panel_dialog))
	{
		
		if (m_chatcomplete)
		{
			return;
		}
		if (m_chatnum <= 0)
		{
			m_chatcomplete = true;
			runAction(Sequence::create(CallFunc::create([=]{chatFadeOut(); }), DelayTime::create(1), CallFunc::create([=]{guide(); }),nullptr));
			return;
		}
		m_chatcomplete = false;
		if (!m_cursay)
		{
			m_cursay = true;
			return;
		}else
 		{
			if (m_chatnum)
			{
				t_size = 0;
				m_cursay = false;
				schedule(schedule_selector(UIGuideMain::textShow), m_textTime);
			}
 			
 		}
		
		return;
	}
	//任务按钮
	if (isButton(button_task))
	{		
		openTaskLayer(m_infoIndex);		
	}
	//宫殿按钮
	if (isButton(button_palace) || isButton(button_cityhall))
	{
		openPalace();
		return;
	}
	//任务界面返回
	if (isButton(button_back))
	{
		if (m_draw)
		{
			m_draw->setVisible(false);
			m_draw->removeFromParentAndCleanup(true);
			m_draw = nullptr;
		}	
		this->closeView();		
	}

	//任务界面领取奖励
	if (isButton(button_get_reward))
	{
		openRewardConfirm();
		
	}

	//任无界面领取任务奖励确认界面
	if (isButton(button_result_yes))
	{
		closeRewadConfirm();
		updateUserInfo();
		if (m_guidestage == CONFIRM_END_SELL_REWARD)
		{
			this->closeView(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());
		}
	}

	//交易所
	if (isButton(button_market))
	{
		CHANGETO(SCENE_TAG::EXCHANGEGUID_TAG);
		return;
	}

	//主角升级界面点击事件
	if (isButton(panel_levelup))
	{
		closeView(COMMOM_COCOS_RES[C_VIEW_LEVEL_UP_CSB]);
		m_guidestage -= 1;
		
	}

	//主角声望等级提升界面点击事件
	if (isButton(panel_r_levelup))
	{
		closeView(COMMOM_COCOS_RES[C_VIEW_R_LEVEL_UP_CSB]);
		m_guidestage -= 1;
	}

	//主角信息界面
	if (isButton(image_head))
	{
		CHANGETO(SCENE_TAG::SKILLGUIDE_TAG);
	}

	if (isButton(button_shipyard))
	{
		if (!m_bIsClickedShipyard)
		{
			m_bIsClickedShipyard = true;
			auto scene = Scene::create();
			auto guideDialogLayer = UINoviceStoryLine::GetInstance();
			if (!guideDialogLayer->getParent())
			{
				scene->addChild(guideDialogLayer, 10001);
			}
			guideDialogLayer->onGuideTaskMessage(UINoviceStoryLine::SHIPYARD_ONE_DIALOG, 0);
			_director->replaceScene(scene);
		}
		return;
	}
	if (isButton(button_tavern))
	{
		CHANGETO(SCENE_TAG::PUP_GUIDE_TAG);
		return;
	}
	if (isButton(button_dock))
	{
		CHANGETO(SCENE_TAG::WHARF_GUIDE_TAG);
		return;
	}
	this->guide();
}

void UIGuideMain::chatFadeIn()
{
	m_cursay = false;
	t_size = 0;
	auto tips = SINGLE_SHOP->getTipsInfo();
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto i_soldier = guard_dialog->getChildByName<ImageView*>("image_soldier");
	auto i_dialog = guard_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = guard_dialog->getChildByName<Text*>("label_guard");
	auto t_content = guard_dialog->getChildByName<Text*>("label_content");
	auto t_name = guard_dialog->getChildByName<Text*>("label_guard");
	auto i_anchor = guard_dialog->getChildByName<Text*>("image_anchor");
	if (SINGLE_HERO->m_iGender == 1)
	{
		t_name->setText(tips["TIP_MAIN_FEMAIE_AIDE"]);
	}
	else
	{
		t_name->setText(tips["TIP_MAIN_MAIE_AIDE"]);
	}
	guard_dialog->setPosition(STARTPOS);
	i_dialog->setOpacity(0);
	t_guard->setOpacity(0);
	t_content->setOpacity(0);
	t_name->setOpacity(0);
	i_anchor->setOpacity(0);
	i_soldier->loadTexture(getCheifIconPath());
	auto countAction = Sequence::createWithTwoActions(DelayTime::create(0.5), CallFunc::create([=]{
		i_dialog->runAction(FadeIn::create(0.5));
		t_guard->runAction(FadeIn::create(0.5));
		t_content->runAction(FadeIn::create(0.5));
		t_name->runAction(FadeIn::create(0.5));
		this->schedule(schedule_selector(UIGuideMain::textShow), m_textTime);
	}));
	if (m_guidestage == SKILL_GUIDE_DIALOG)
	{
		countAction = Sequence::createWithTwoActions(DelayTime::create(0.1), CallFunc::create([=]{
			i_dialog->runAction(FadeIn::create(0.5));
			t_guard->runAction(FadeIn::create(0.5));
			t_content->runAction(FadeIn::create(0.5));
			t_name->runAction(FadeIn::create(0.5));
			this->schedule(schedule_selector(UIGuideMain::textShow), m_textTime);
		}));
		i_soldier->runAction(Sequence::create(MoveBy::create(0.5, Vec2(i_soldier->getBoundingBox().size.width, 0)), countAction, nullptr));

	}
	else
	{
		i_soldier->runAction(Sequence::create(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(i_soldier->getBoundingBox().size.width, 0)), countAction, nullptr));
	}	
}
void UIGuideMain::chatFadeOut()
{
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto i_soldier = guard_dialog->getChildByName<ImageView*>("image_soldier");
	auto i_dialog = guard_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = guard_dialog->getChildByName<Text*>("label_guard");
	auto t_content = guard_dialog->getChildByName<Text*>("label_content");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");

	i_soldier->runAction(Sequence::createWithTwoActions(DelayTime::create(0.1),MoveBy::create(0.5, -Vec2(i_soldier->getBoundingBox().size.width, 0))));
	i_dialog->runAction(FadeOut::create(0.1));
	t_guard->runAction(FadeOut::create(0.01));
	t_content->runAction(FadeOut::create(0.01));
	guard_dialog->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), Place::create(ENDPOS)));
	i_anchor->runAction(FadeOut::create(0.01));
}
void UIGuideMain::anchorAction()
{
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(255);
	i_anchor->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0, 10))), EaseBackOut::create(MoveBy::create(0.5, Vec2(0, -10))))));
}
void UIGuideMain::openPalace()
{

	CHANGETO(SCENE_TAG::PALACEGUID_TAG);

}

void UIGuideMain::setButtonsDisable(Node * node)
{
	auto children = node->getChildren();
	Vector<Widget*>btns;
	for (auto btn : children)
	{
		std::string name = btn->getName();
		if (btn->getChildren().size() >= 0 )
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

void UIGuideMain::focusOnButton(Node * psender)
{
	sprite_hand->setVisible(true);
	auto c_psender = dynamic_cast<Widget*>(psender);
	Size win = _director->getWinSize();
	Vec2 pos = c_psender->getWorldPosition();
	Size cut_hand = sprite_hand->getContentSize() / 2;
	Size cut_psendet = psender->getContentSize() / 2;
	std::string name = dynamic_cast<Widget*>(psender)->getName();
	//提示的小手在屏幕中的四个不同方位方位需要调整的角度
	if (pos.x < win.width/2 && pos.y > win.height/2)
	{
		sprite_hand->setRotation(-180);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width/2,
			c_psender->getWorldPosition().y - c_psender->getContentSize().height/2*0.6 - sprite_hand->getContentSize().height/2*0.6));
	}
	else if (pos.x > win.width/2 && pos.y > win.height/2)
	{
		sprite_hand->setRotation(-110);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
			c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));

	}else if (pos.x < win.width/2 && pos.y < win.height/2)
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
				sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width/2,
					c_psender->getWorldPosition().y + c_psender->getContentSize().height/2*0.6 + sprite_hand->getContentSize().height/2*0.6));
			}
			
		}
	
	}else if (pos.x >win.width/2 && pos.y < win.height/2)
	{
		sprite_hand->setRotation(0);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width/2,
			c_psender->getWorldPosition().y + c_psender->getContentSize().height/2*0.6 + sprite_hand->getContentSize().height/2*0.6));
	}
	sprite_hand->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255),TintTo::create(0.5, 180, 180, 180))));
}
void UIGuideMain::updateUserInfo()
{
	auto t_level = m_vRoot->getChildByName<Text*>("label_lv");
	auto lo_exp = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_exp"));
	auto lo_rep = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_rep"));
	auto t_coin = m_vRoot->getChildByName<Text*>("label_coin_num");
	auto t_gold = m_vRoot->getChildByName<Text*>("label_gold_num");
	auto image_head = m_vRoot->getChildByName<ImageView*>("image_head");
	auto t_name = m_vRoot->getChildByName<Text*>("label_city_name");
	auto t_r_level = m_vRoot->getChildByName<Text*>("label_lv_r");

	int m_coin = SINGLE_HERO->m_iSilver + m_receiveCoin;
	int m_gold = SINGLE_HERO->m_iVp + m_receiveVp;
	int m_exp = SINGLE_HERO->m_iExp + m_receiveexp;
	int m_rp = SINGLE_HERO->m_iRexp + m_receiveRp ;

	SINGLE_HERO->m_iSilver = m_coin;
	SINGLE_HERO->m_iVp = m_gold;
	SINGLE_HERO->m_iExp = m_exp;
	SINGLE_HERO->m_iRexp = m_rp;

	int m_explv = EXP_NUM_TO_LEVEL(SINGLE_HERO->m_iExp);
	int m_rplv = FAME_NUM_TO_LEVEL(SINGLE_HERO->m_iRexp);

	float temp_exp = 0;
	if (m_explv < LEVEL_MAX_NUM)
	{
		temp_exp = (m_exp - LEVEL_TO_EXP_NUM(m_explv))*1.0 / (LEVEL_TO_EXP_NUM(m_explv + 1) - LEVEL_TO_EXP_NUM(m_explv));
	}
	float temp_rep = 0;
	if (m_explv < LEVEL_MAX_NUM)
	{
		temp_rep = (m_rp - LEVEL_TO_FAME_NUM(m_rplv))*1.0 /( LEVEL_TO_FAME_NUM(m_rplv + 1) - LEVEL_TO_FAME_NUM(m_rplv));
	}
	t_level->setText(StringUtils::format("%d",m_explv));
	t_r_level->setString(StringUtils::format("%d",m_rplv));
	t_coin->setText(numSegment(StringUtils::format("%d",m_coin)));
	t_gold->setText(numSegment(StringUtils::format("%d",m_gold)));
	lo_exp->setContentSize(Size(lo_exp->getContentSize().width, 120 * (temp_exp)));
	lo_rep->setContentSize(Size(lo_rep->getContentSize().width,120*(temp_rep)));	
	if (m_isfirstIn)
	{
		m_roleLevel = m_explv;
		m_role_rLevel = m_rplv;
		image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
		t_name->setString(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name);
		m_isfirstIn = false;

	}
	else  
	{
		if (m_roleLevel < m_explv)
		{
			m_roleLevel = m_explv;
			roleLevelUp(m_roleLevel);
		}
		if (m_role_rLevel < m_rplv)
		{
			m_role_rLevel = m_rplv;
			flushPrestigeLevelUp();
		}
	}

}
void UIGuideMain::openTaskLayer(int chapter)
{

	this->openView(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());
	m_tasklayer = getViewRoot(std::string(COCOSTDIO_RES_DIR + COCOSTUDIO_RES[24] + ".csb").c_str());
	if (!m_tasklayer->getParent())
	{
		this->addChild(m_tasklayer);
	}
	auto p_quest = m_tasklayer->getChildByName<Widget*>("panel_quest");
	auto i_pulldown = m_tasklayer->getChildByName("image_pulldown");
	auto i_pulldown_1 = m_tasklayer->getChildByName("image_pulldown_left");
	i_pulldown->setVisible(false);
	i_pulldown_1->setVisible(false);
	p_quest->setOpacity(255);
	p_quest->setPosition(STARTPOS);
	setButtonsDisable(p_quest);
	auto l_leftView = m_tasklayer->getChildByName<ListView*>("listview_left");
	auto b_mainquest = m_tasklayer->getChildByName<Widget*>("button_main_quest");
	auto b_title = m_tasklayer->getChildByName<Widget*>("button_title_story");
	l_leftView->removeAllChildren();
	auto tipInfo = SINGLE_SHOP->getTipsInfo();
	for (int i = 0; i < 3; i++)
	{
		std::string name = StringUtils::format("button_title_story%d", i);
		auto title = b_title->clone();
		title->setName(name);
		name = StringUtils::format("button_main_quest%d", i);
		auto quest = b_mainquest->clone();
		if (i == 0)
		{
			dynamic_cast<Button*>(quest)->setBright(false);
		}
		
		quest->setName(name);
		if (i > 0)
		{
			auto t_questCount = quest->getChildByName<Text*>("label_quest_name");
			t_questCount->setText(tipInfo["TIP_MAIN_NO_TASK_NAME_1"]);
			title->removeChildByName("button_unfold");
		}
		l_leftView->pushBackCustomItem(title);
		l_leftView->pushBackCustomItem(quest);
	}
 	auto b_reward = m_tasklayer->getChildByName<Button*>("button_get_reward");
// 	b_reward->setTitleText(tipInfo["TIP_MAIN_NO_TASK_NAME_1"]);
	auto b_item = l_leftView->getItem(1);
	auto i_notify = b_item->getChildByName<ImageView*>("image_notify_3");
	i_notify->setVisible(false);
	i_notify->loadTexture("cocosstudio/login_ui/sailing_720");
	auto l_task = m_tasklayer->getChildByName<ListView*>("listview_task_info");
	l_task->setVisible(true);
	b_reward->setVisible(true);

	float rect_x = l_task->getBoundingBox().size.width;
	float rect_y = l_task->getBoundingBox().size.height / 6;

	m_tasklayer->getChildByName<ImageView*>("image_pulldown")->getChildByName<ImageView*>("button_pulldown")->setVisible(false);
	taskShow(chapter);
}
void UIGuideMain::showMark(bool flag)
{
	auto mark = m_vRoot->getChildByName("image_unread_4");
	mark->setVisible(flag);
}

void UIGuideMain::readJsonFile()
{
	std::string jsonpath;
	std::string name = confirmCityJson();
	jsonpath= findJsonFile(name);
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);

	rapidjson::Document root;
	if(root.Parse<0>(contentStr.c_str()).HasParseError()){
		return;
	}
	auto count = DictionaryHelper::getInstance()->getArrayCount_json(root, "pname");
	auto dialog1_count = DictionaryHelper::getInstance()->getArrayCount_json(root, "dialog1");

	for (int i = 0; i < count; i++)
	{
		const auto& dic = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root, "pname", i);
		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%d", i + 1);
		auto content = DictionaryHelper::getInstance()->getStringValue_json(dic, buffer);
		std::string str = (char*)content;
		m_names.push_back(str);
	}
	for (int i = 0; i < dialog1_count; i++)
	{
		const auto& dic = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root, "dialog1", i);
		int idx = 1;
		auto content = DictionaryHelper::getInstance()->getStringValue_json(dic, "1");
		if (!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic, "2");
			idx = 2;
		}
		if (!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic, "3");
			idx = 3;
		}
		if (!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic, "0");
			idx = 0;
		}
		std::string str = (char*)content;
		if (content != 0){
			auto item = new DIALOGDATA;
			item->dialog = str;
			item->personIdx = idx;
			m_dialogdatas.push_back(item);
		}	
	}

	std::string jsonpath1;
	std::string name1 = confirmCityDisplayJson();	
	jsonpath1 = findJsonFile(name1);
	std::string contentStr1 = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath1);
	rapidjson::Document root1;
	root1.Parse<0>(contentStr1.c_str());
	auto display_count = DictionaryHelper::getInstance()->getArrayCount_json(root1, "display");
	for (int i = 0; i < display_count; i++)
	{
		auto dic = DictionaryHelper::getInstance()->getStringValueFromArray_json(root1, "display", i);
		m_dispalys.push_back(dic);

	}
}

std::string UIGuideMain::findJsonFile(std::string path){
	std::string jsonPath;
	LanguageType nType = LanguageType(Utils::getLanguage());
	char buffer[10];
	sprintf(buffer, "%d", ProtocolThread::GetInstance()->getIcon());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
		log("language: chinese");
		jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(path + ".zh_HK.json");
		m_textTime = 0.1;
		break;
	case cocos2d::LanguageType::CHINESE:
		log("language: chinese");
	
		jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(path + ".zh_CN.json");
			m_textTime = 0.1;
		break;
	default:
		log("language: english");

		jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(path + ".json");
			m_textTime = 0.1 / 3;
		break;
	}
	return jsonPath;
}
void UIGuideMain::showBackGround()
{
	auto picNum = 0;
	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
	{
		picNum = SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_village;
	}
	else
	{
		picNum = SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_id;
	}
	std::string cityName = StringUtils::format("cities_resources/background/city_%d.jpg", picNum);
	auto i_city_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_main_city_bg"));
	i_city_bg->ignoreContentAdaptWithSize(false);
	i_city_bg->loadTexture(cityName);

	auto i_city_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_flag"));
	i_city_icon->loadTexture(getCountryIconPath(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].nation));

	Helper::seekWidgetByName(m_vRoot, "button_libao")->setVisible(false);
	Helper::seekWidgetByName(m_vRoot, "image_libao_bg")->setVisible(false);
	Helper::seekWidgetByName(m_vRoot, "label_libao_time")->setVisible(false);

	if (!m_vRoot->getParent())
	{
		this->addChild(m_vRoot);
	}
	villageButtonShow();
}

void UIGuideMain::taskBlink()
{
	m_draw = cocos2d::ui::Scale9Sprite::create("cocosstudio/login_ui/start_720/select.png");
	m_draw->setAnchorPoint(Vec2(0, 0));
	m_draw->setContentSize(Size(862, 195));
	m_draw->setInsetBottom(80);
	m_draw->setInsetLeft(80);
	m_draw->setInsetRight(80);
	m_draw->setInsetTop(80);
	m_draw->setVisible(true);
	sprite_hand->setVisible(false);
	auto btn_close = m_tasklayer->getChildByName<Button*>("button_back");
	btn_close->setTouchEnabled(false);
	auto  l_taskText = m_tasklayer->getChildByName<ListView*>("listview_task_info");
	auto pos = l_taskText->getWorldPosition();
	float x = l_taskText->getPositionX();
	float y = 0;
	for (int i = 0; i < 3; i++)
	{
		y += l_taskText->getItem(i)->getContentSize().height;
	}
	m_draw->setPosition(Vec2(pos.x,l_taskText->getContentSize().height - y) + Vec2(-60,15));
	this->addChild(m_draw, sprite_hand->getZOrder()-1);
	m_draw->runAction(Sequence::create(FadeIn::create(0.5),FadeOut::create(0.5), FadeIn::create(0.5),nullptr));
	runAction(Sequence::createWithTwoActions(DelayTime::create(1), CallFunc::create([=]{sprite_hand->setVisible(true); btn_close->setTouchEnabled(true); })));
}

void UIGuideMain::taskShow(int index)
{
	//任务列表
	auto l_leftlist = m_tasklayer->getChildByName("listview_left");
 	auto text1 = l_leftlist->getChildByName<Button*>("button_title_story0")->getChildByName<Text*>("label_quest_name");
 	auto text2 = l_leftlist->getChildByName<Button*>("button_main_quest0")->getChildByName<Text*>("label_quest_name");
 	auto text3 = l_leftlist->getChildByName<Button*>("button_title_story1")->getChildByName<Text*>("label_quest_name");
 	auto text4 = l_leftlist->getChildByName<Button*>("button_main_quest1")->getChildByName<Text*>("label_quest_name");
 	auto text5 = l_leftlist->getChildByName<Button*>("button_title_story2")->getChildByName<Text*>("label_quest_name");
 	auto text6 = l_leftlist->getChildByName<Button*>("button_main_quest2")->getChildByName<Text*>("label_quest_name");

	//任务内容
	auto t_silver_num = m_tasklayer->getChildByName<Text*>("label_silver_num");
	auto t_V_num = m_tasklayer->getChildByName<Text*>("label_v_num");
	auto t_exp_num = m_tasklayer->getChildByName<Text*>("label_exp_num");
	auto t_r_num = m_tasklayer->getChildByName<Text*>("label_r_num");
	auto l_taskname = m_tasklayer->getChildByName<Text*>("label_task_name");
	auto l_content = m_tasklayer->getChildByName<Text*>("label_task_content");

	auto b_item1 = m_tasklayer->getChildByName<Button*>("button_items_1");
	auto b_item2 = m_tasklayer->getChildByName<Button*>("button_items_2");
	auto i_itembg1 = m_tasklayer->getChildByName<ImageView*>("image_goods_bg_1");
	auto i_itembg2 = m_tasklayer->getChildByName<ImageView*>("image_goods_bg_2");

	b_item1->setVisible(false);
	b_item2->setVisible(false);
	i_itembg1->setVisible(false);
	i_itembg2->setVisible(false);

	auto tipInfo = SINGLE_SHOP->getTipsInfo();
	text1->setText(tipInfo["TIP_TASK_BUTTON_TITLE1"]);
	text3->setText(tipInfo["TIP_TASK_BUTTON_TITLE2"]);
	text5->setText(tipInfo["TIP_TASK_BUTTON_TITLE3"]);
	auto i_item1 = m_tasklayer->getChildByName<Widget*>("panel_silver");
	auto i_item2 = m_tasklayer->getChildByName<Widget*>("panel_v");

	auto taskInfo = SINGLE_SHOP->getMaintaskGuide();
	MAINTASKGUIDEINFO guideTask = taskInfo[index];
	//剧情任务按钮
	text2->setText(guideTask.name);
	//任务描述 章节
	l_taskname->setText(guideTask.title);
	l_content->setTextColor(Color4B(169,86,1,255));
	//任务描述
	l_content->setText(guideTask.tip);
	//任务奖励银币
	t_silver_num->setText(numSegment(StringUtils::format("%d",guideTask.rewards.coin)));
	//V票
	t_V_num->setText(numSegment(StringUtils::format("%d", guideTask.rewards.vticket)));
	//经验
	t_exp_num->setText(StringUtils::format("%d", guideTask.rewards.exp));
	//声望
	t_r_num->setText(StringUtils::format("%d", guideTask.rewards.reputation));

	text4->setText(tipInfo["TIP_MAIN_NO_TASK_NAME_1"]);
	text6->setText(tipInfo["TIP_MAIN_NO_TASK_NAME_1"]);

	auto l_taskInfo = m_tasklayer->getChildByName<ListView*>("listview_task_info");
	l_taskInfo->removeAllChildrenWithCleanup(true);
	auto p_taskName = m_tasklayer->getChildByName<Layout*>("panel_task_name")->clone();
	auto i_div = m_tasklayer->getChildByName<ImageView*>("image_div_1")->clone();
	auto p_content = m_tasklayer->getChildByName<Layout*>("panel_task_content")->clone();
	auto p_reward = m_tasklayer->getChildByName<Layout*>("panel_reward")->clone();
	auto t_taskStory = m_tasklayer->getChildByName<Text*>("label_task_story")->clone();	
	std::string str = "";
	str = guideTask.desc;
	replaceText(str);
	dynamic_cast<Text*>(t_taskStory)->setString(str);
	/*
	if (TASK_BEGIN == m_guidestage)
	{
		str = m_dispalys.at(68);
		replaceText(str);
		dynamic_cast<Text*>(t_taskStory)->setString(str);
	}
	else if (SELL_TASK_BEGIN == m_guidestage || GET_REWARD == m_guidestage )
	{
		str = m_dispalys.at(69);
		replaceText(str);
		dynamic_cast<Text*>(t_taskStory)->setString(str);
	}
	else if (GET_LISEBCE_REWARD == m_guidestage ||SELL_TASK_END == m_guidestage)
	{
		str = m_dispalys.at(70);
		replaceText(str);
		dynamic_cast<Text*>(t_taskStory)->setString(str);
	}
	*/
	l_taskInfo->pushBackCustomItem(p_taskName);
	l_taskInfo->pushBackCustomItem(i_div->clone());
	l_taskInfo->pushBackCustomItem(p_content);
	l_taskInfo->pushBackCustomItem(p_reward);
	l_taskInfo->pushBackCustomItem(i_div->clone());
	l_taskInfo->pushBackCustomItem(t_taskStory);

}

void UIGuideMain::replaceText(std::string &text)
{
	if (text.find("[aidename]"))
	{
		std::string oldValue = "[aidename]";
		std::string new_value_aide;
		if (SINGLE_HERO->m_iGender == 1)
		{
			new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"];
		}
		else
		{
			new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"];
		}
		repalce_all_ditinct(text, oldValue, new_value_aide);
		log("%s", text.c_str());
	}
	
}
void UIGuideMain::openRewardConfirm()
{
	//任务确定界面
	openView(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
	openView(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);  
}
void UIGuideMain::closeRewadConfirm()
{
	closeView(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
	closeView(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);
}

void UIGuideMain::initConfirmRewardInfo(REWARD_INFO * reward)
{
	Vector<Widget*> m_widgets;
	auto view = getViewRoot(COMMOM_COCOS_RES[COMMON_LIST::C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view_1 = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);
	auto  l_relist = view->getChildByName<ListView*>("listview_result");
	auto  t_title = view->getChildByName<Text*>("label_title");
	auto p_result = view->getChildByName<Widget*>("panel_result");
	auto i_div = view->getChildByName<ImageView*>("image_div_1");
	auto lo_exp = dynamic_cast<Widget*>(Helper::seekWidgetByName(view_1, "panel_exp"));
	auto lo_rep = dynamic_cast<Widget*>(Helper::seekWidgetByName(view_1, "panel_rep"));
	auto c_item = dynamic_cast<Layout*>(Helper::seekWidgetByName(p_result, "panel_coin"));
	auto tipInfo = SINGLE_SHOP->getTipsInfo();	
	t_title->setText(tipInfo["TIP_PUP_REWARD_TITLE"]);
	if (reward)
	{
		auto c_silver = c_item->clone();
		auto t_coinnum = c_silver->getChildByName<Text*>("label_buy_num");
		t_coinnum->setString(numSegment(StringUtils::format("+%d", reward->coin)));
		l_relist->pushBackCustomItem(c_silver);
		l_relist->pushBackCustomItem(i_div->clone());

		if (reward->v_p > 0)
		{
			auto c_glod = c_item->clone();
			auto i_gold = c_glod->getChildByName<ImageView*>("image_silver");
			auto t_goldnum = c_glod->getChildByName<Text*>("label_buy_num");
			i_gold->loadTexture("cocosstudio/login_ui/store_720/v.png");
			t_goldnum->setString(numSegment(StringUtils::format("+%d", reward->v_p)));
			l_relist->pushBackCustomItem(c_glod);
			l_relist->pushBackCustomItem(i_div->clone());
		}
	}

	m_receiveCoin = reward->coin;
	m_receiveVp = reward->v_p;
	m_receiveexp = reward->exp;
	m_receiveRp = reward->r_p;

	auto expview = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);
	auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(expview, "image_head"));
	auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(expview, "label_lv"));
	auto t_rlv = dynamic_cast<Text*>(Helper::seekWidgetByName(expview, "label_lv_r"));
	i_icon->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));

	int m_coin = SINGLE_HERO->m_iSilver;
	int m_gold = SINGLE_HERO->m_iVp;
	int m_exp = SINGLE_HERO->m_iExp;
	int m_rp = SINGLE_HERO->m_iRexp;
	int m_explv = EXP_NUM_TO_LEVEL(SINGLE_HERO->m_iExp);
	int m_rplv = FAME_NUM_TO_LEVEL(SINGLE_HERO->m_iRexp);
	float temp_exp = 0;
	if (m_explv < LEVEL_MAX_NUM)
	{
		temp_exp = (m_exp - LEVEL_TO_EXP_NUM(m_explv))*1.0 / (LEVEL_TO_EXP_NUM(m_explv + 1) - LEVEL_TO_EXP_NUM(m_explv));
	}
	float temp_rep = 0;
	if (m_explv < LEVEL_MAX_NUM)
	{
		temp_rep = (m_rp - LEVEL_TO_FAME_NUM(m_rplv))*1.0 / (LEVEL_TO_FAME_NUM(m_rplv + 1) - LEVEL_TO_FAME_NUM(m_rplv));
	}
	auto p_exp = expview->getChildByName<Widget*>("panel_exp");
	auto p_rep = expview->getChildByName<Widget*>("panel_r");
	auto t_exp = p_exp->getChildByName<Widget*>("label_exp");
	auto i_rep = p_rep->getChildByName<Widget*>("image_r");
	auto label_exp_num = p_exp->getChildByName<Text*>("label_exp_num");
	label_exp_num->setString(StringUtils::format("+ %d", reward->exp));
	auto label_r_num = p_rep->getChildByName<Text*>("label_r_num");
	label_r_num->setString(StringUtils::format("+ %d", reward->r_p));
	t_exp->setPositionX(p_exp->getBoundingBox().size.width / 2 - label_exp_num->getBoundingBox().size.width / 2 - 10);
	label_exp_num->setPositionX(t_exp->getPositionX() + t_exp->getBoundingBox().size.width / 2 + 20);
	i_rep->setPositionX(p_rep->getBoundingBox().size.width / 2 - label_r_num->getBoundingBox().size.width / 2 - 10);
	label_r_num->setPositionX(i_rep->getPositionX() + i_rep->getBoundingBox().size.width / 2 + 20);

	t_lv->setString(StringUtils::format("%d", m_explv));
	t_rlv->setString(StringUtils::format("%d", m_rplv));
	lo_exp->setContentSize(Size(lo_exp->getContentSize().width, 120 * (temp_exp)));
	lo_rep->setContentSize(Size(lo_rep->getContentSize().width, 120 * (temp_rep)));
}

void UIGuideMain::initRewardInfo(int index)
{

	auto guideInfo = SINGLE_SHOP->getMaintaskGuide();
	auto reward = guideInfo[index];
	m_rewardInfo->coin = reward.rewards.coin;
	m_rewardInfo->v_p = reward.rewards.vticket;
	m_rewardInfo->exp = reward.rewards.exp;
	m_rewardInfo->r_p = reward.rewards.reputation;
	m_rewardInfo->goods = nullptr;
	initConfirmRewardInfo(m_rewardInfo);
}

void UIGuideMain::checkCurStage()
{
	int  stage = 0;
	int stagelevel = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());
	if (stagelevel > 4)
	{
		if (SINGLE_HERO->m_iStage == -1)
		{
			int stagelevel = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());
			switch (stagelevel)
			{
			case 6:
				stage = REWARD_CONFIRM;
				break;
			case  7:
				stage = LISENCE_REWARD_CONFIRM;
				break;
			case 8:
				stage = GET_END_SELL_TASK_REWARD;
				break;
			default:
				break;
			}
		}

		int a = SINGLE_HERO->m_iStage;
		switch (SINGLE_HERO->m_iStage + 1)
		{
		case GUIDE_TASK_BEGAN:
			stage = 0;
			break;
		case PALACE_TASK_COMPLETE:
			stage = OPEN_PALACE;
			break;
		case EXCHANGE_TASK_COMPLETE:
			stage = OPEN_EXCHANGE;
			break;
		default:
			break;
		}


	}
	else
	{
		int a = SINGLE_HERO->m_iStage;
		switch (SINGLE_HERO->m_iStage)
		{
		case NEW_GUIDE_TASH_BEGAN:
			stage = 0;
			break;
		case NEW_SHIPYARD_TASK_COMPLETE:
			stage = NEW_TASK_CLOSE;
			break;
		case NEW_BAR_TASK_COMPLETE:
			stage = NEW_OPEN_BAR;
			break;
		case NEW_EXCHANGE_TASK_COMPLETE:
			stage = NEW_OPEN_EXCHANGE;
			break;
		default:
			break;
		}
		if (SINGLE_HERO->m_iStage == -1)
		{
			int stagelevel = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());
			switch (stagelevel)
			{
			case NEW_OPEN_TASK:
				stage = NEW_CONFIRM_GET_SHIPYARD_TASK_REWARD;
				break;
			case NEW_TASK_CLOSE:
				stage = NEW_CONFIRM_GET_BAR_TASK_REWARD;
				break;
			case NEW_OPEN_SHIPYARD_TASK:
				stage = NEW_CONFIRM_GET_EXCHANGE_TASK_REWARD;
				break;
			default:
				break;
			}
		}

	}


	m_guidestage = stage;
//	readJsonFile();
	guide();
}

void UIGuideMain::showNotify(bool flag)
{
	auto l_leftView = m_tasklayer->getChildByName<ListView*>("listview_left");
	auto b_item = l_leftView->getItem(1);
	auto i_notify = b_item->getChildByName<ImageView*>("image_notify_3");
	i_notify->setVisible(flag);	
}
void UIGuideMain::roleLevelUp(int level)
{

	UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(MAIN_CITY_NEW_POINT).c_str(), true);
	UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(SKILL_NEW_POINT).c_str(), true);
	UserDefault::getInstance()->flush();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_PALYER_UPGRAD_23);
	openView(COMMOM_COCOS_RES[C_VIEW_LEVEL_UP_CSB]);
	auto level_up = getViewRoot(COMMOM_COCOS_RES[C_VIEW_LEVEL_UP_CSB]);
	level_up->setCameraMask(_cameraMask, true);
	level_up->setLocalZOrder(50);
	level_up->setPosition(STARTPOS);
	if (level_up)
	{
		level_up->addTouchEventListener(CC_CALLBACK_2(UIGuideMain::menuCall_func, this));
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(level_up, "label_level"));
		t_lv->setString(StringUtils::format("%d", level));
		t_lv->enableOutline(Color4B::BLACK, OUTLINE_MIN);
		auto node1 = level_up->getChildByName<ImageView*>("image_node1");
		auto node2 = level_up->getChildByName<ImageView*>("image_node2");
		node1->setVisible(true);
		node2->setVisible(false);
		node1->setScale(0.1);
		node1->runAction(ScaleTo::create(0.3, 1));
		auto i_light = node1->getChildByName<ImageView*>("image_light");
		auto image_body_light = node1->getChildByName<ImageView*>("image_body_light");
		i_light->runAction(RotateBy::create(2, 180));
		i_light->runAction(Sequence::createWithTwoActions(DelayTime::create(1.7), FadeOut::create(0.3)));
		image_body_light->runAction(RepeatForever::create(Sequence::createWithTwoActions(FadeTo::create(0.5, 255), FadeTo::create(0.5, 153))));
		Vector<Node*> arrayRootChildren = node1->getChildren();
		for (auto obj : arrayRootChildren)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(FadeIn::create(0.2));
		}
		node2->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7), Show::create()));
		Vector<Node*> arrayRootChildren1 = node2->getChildren();
		for (auto obj : arrayRootChildren1)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7), FadeIn::create(0.3)));
		}

		auto level_up = node2->getChildByName<ImageView*>("image_levelup");
		level_up->ignoreContentAdaptWithSize(true);
		level_up->loadTexture(LEVEL_UP[SINGLE_SHOP->L_TYPE]);
	}

	if (!m_isLevelup)
	{
		m_isLevelup = true;
	}
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.2), CallFunc::create([=]{sprite_hand->setVisible(false);})));

}
void UIGuideMain::saveUserData(int64_t a[4])
{
	auto U_instance = UserDefault::getInstance();
	U_instance->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str(), a[0]);
	U_instance->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str(), a[1]);
	U_instance->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASILVERNUM").c_str(), a[2]);
	U_instance->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAGOLDNUM").c_str(), a[3]);
	U_instance->flush();
}

std::string UIGuideMain::confirmCityJson()
{
	
	int city_id = SINGLE_HERO->m_iCityID;
	std::string str;
	switch (city_id)
	{
	case 17:
		str = "Lisben_";
		break;
	case 15:
		str = "Xibanya_";
		break;
	case 21:
		str = "England_";
		break;
	case 23:
		str = "Helan_";
		break;
	case 9:
		str = "Renaya_";
		break;
	default:
		break;
	}
	return StringUtils::format("res_lua/lua_config/%sScene",str.c_str());
}

std::string UIGuideMain::confirmCityDisplayJson()
{
	int city_id = SINGLE_HERO->m_iCityID;
	std::string str;
	switch (city_id)
	{
	case 17:
		str = "Lisben_";
		break;
	case 15:
		str = "Xibanya_";
		break;
	case 21:
		str = "England_";
		break;
	case 23:
		str = "Helan_";
		break;
	case 9:
		str = "Renaya_";
		break;
	default:
		break;
	}
	return StringUtils::format("res_lua/lua_config/%sdisplay", str.c_str());

}
void UIGuideMain::villageButtonShow()
{
	if (!m_vRoot) return;
	auto button_Main_cityInfo = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_city_name_bg"));
	auto btn_market = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_market"));
	auto btn_shipyard = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_shipyard"));
	auto btn_tavern = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_tavern"));
	auto btn_palace = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_palace"));
	auto btn_bank = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_bank"));
	auto btn_cityhall = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_cityhall"));
	auto btn_btndock = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_dock"));
	auto btn_village_cityhall = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_village_cityhall"));
	auto btn_village_tavern = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_village_tavern"));

	auto image_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_flag"));
	auto image_village = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_village"));
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
		btn_market->loadTextureNormal("cocosstudio/login_ui/city_720/market_zh_hk.png");
		btn_shipyard->loadTextureNormal("cocosstudio/login_ui/city_720/shipyard_zh_hk.png");
		btn_tavern->loadTextureNormal("cocosstudio/login_ui/city_720/tavern_zh_hk.png");
		btn_palace->loadTextureNormal("cocosstudio/login_ui/city_720/palace_zh_hk.png");
		btn_bank->loadTextureNormal("cocosstudio/login_ui/city_720/bank_zh_hk.png");
		btn_cityhall->loadTextureNormal("cocosstudio/login_ui/city_720/cityhall_zh_hk.png");
		btn_btndock->loadTextureNormal("cocosstudio/login_ui/city_720/dock_zh_hk.png");
		break;
	case cocos2d::LanguageType::CHINESE:
		btn_market->loadTextureNormal("cocosstudio/login_ui/city_720/market_zh_cn.png");
		btn_shipyard->loadTextureNormal("cocosstudio/login_ui/city_720/shipyard_zh_cn.png");
		btn_tavern->loadTextureNormal("cocosstudio/login_ui/city_720/tavern_zh_cn.png");
		btn_palace->loadTextureNormal("cocosstudio/login_ui/city_720/palace_zh_cn.png");
		btn_bank->loadTextureNormal("cocosstudio/login_ui/city_720/bank_zh_cn.png");
		btn_cityhall->loadTextureNormal("cocosstudio/login_ui/city_720/cityhall_zh_cn.png");
		btn_btndock->loadTextureNormal("cocosstudio/login_ui/city_720/dock_zh_cn.png");
		break;
	default:
		break;
	}

	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)//村庄
	{
		btn_market->setVisible(false);
		btn_shipyard->setVisible(false);
		btn_tavern->setVisible(false);
		btn_palace->setVisible(false);
		btn_bank->setVisible(false);
		btn_cityhall->setVisible(false);
		//王宫隐藏
		btn_village_cityhall->setVisible(false);
		btn_village_tavern->setVisible(true);
		image_flag->setVisible(false);
		image_village->setVisible(true);

		button_Main_cityInfo->setTouchEnabled(false);
	}
	else
	{
		btn_market->setVisible(true);
		btn_shipyard->setVisible(true);
		btn_tavern->setVisible(true);
		btn_palace->setVisible(true);
		btn_bank->setVisible(true);
		btn_cityhall->setVisible(true);
		//市政厅，王宫的判断
		if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].palace_type == 1)
		{
			btn_cityhall->setVisible(false);
			btn_palace->setVisible(true);
		}
		else
		{
			btn_palace->setVisible(false);
			btn_cityhall->setVisible(true);
		}
		btn_village_cityhall->setVisible(false);
		btn_village_tavern->setVisible(false);
		image_flag->setVisible(true);
		image_village->setVisible(false);

		button_Main_cityInfo->setTouchEnabled(true);
	}
}

void UIGuideMain::cutChat(std::string content )
{
	int i = 0;
	m_words.clear();
	while (i < content.length())
	{
		char ch = content.c_str()[i];
		
		if (ch > -127 && ch < 0)
		{
			i += 3;
		}
		else
		{
			i++;
		}
		std::string a;
		a = content.substr(0, i);
		m_words.push_back(a);
		m_contentLength++;
	}
}
void UIGuideMain::flushPrestigeLevelUp()
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_PALYER_UPGRAD_23);
	openView(COMMOM_COCOS_RES[C_VIEW_R_LEVEL_UP_CSB]);
	auto prestige = getViewRoot(COMMOM_COCOS_RES[C_VIEW_R_LEVEL_UP_CSB]);
	prestige->setCameraMask(_cameraMask, true);
	if (prestige)
	{
		prestige->addTouchEventListener(CC_CALLBACK_2(UIGuideMain::menuCall_func, this));
		auto node1 = prestige->getChildByName<Widget*>("image_node1");
		auto node2 = prestige->getChildByName<Widget*>("image_node2");
		node1->setVisible(true);
		node2->setVisible(false);
		node1->setScale(0.1);
		node1->runAction(ScaleTo::create(0.5, 1));
		auto image_light = node1->getChildByName<ImageView*>("image_light");
		auto image_light1 = node1->getChildByName<ImageView*>("image_light_1");
		auto button_rlvok = node2->getChildByName<Button*>("button_rlv_ok");
		button_rlvok->setVisible(false);
		image_light->runAction(RotateBy::create(2, 180));
		image_light1->runAction(RepeatForever::create(RotateBy::create(2, 90)));
		image_light->runAction(Sequence::createWithTwoActions(DelayTime::create(1.7), FadeOut::create(0.3)));
		image_light1->runAction(RepeatForever::create(Sequence::createWithTwoActions(FadeTo::create(0.5, 255), FadeTo::create(0.5, 153))));
		Vector<Node*> arrayRootChildren = node1->getChildren();
		for (auto obj : arrayRootChildren)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(FadeIn::create(0.3));
		}
		node2->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7), Show::create()));
		Vector<Node*> arrayRootChildren1 = node2->getChildren();
		for (auto obj : arrayRootChildren1)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7), FadeIn::create(0.3)));
		}
		auto t_lv = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(prestige, "atlasLabel_lv"));
		t_lv->setString(StringUtils::format("%d", FAME_NUM_TO_LEVEL(SINGLE_HERO->m_iRexp)));
		auto level_up = node2->getChildByName<ImageView*>("image_levelup");
		level_up->ignoreContentAdaptWithSize(true);
		level_up->loadTexture(LEVEL_UP[SINGLE_SHOP->L_TYPE]);
	}
}
void UIGuideMain::confirmGuideStage()
{
	if (UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str()) == 1 && SINGLE_HERO->m_iStage == -1)
	{
		ProtocolThread::GetInstance()->setMainTaskStatus(nullptr, GUIDE_FIRST_CITY, UILoadingIndicator::create(this, m_eUIType));
		SINGLE_HERO->m_iStage = 0;
	}
	if (UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str()) == 5 && SINGLE_HERO->m_iStage == -1)
	{
		ProtocolThread::GetInstance()->setMainTaskStatus(nullptr, GUIDE_FIRST_CITY, UILoadingIndicator::create(this, m_eUIType));
		SINGLE_HERO->m_iStage = 3;
	}
}
