#include "UITask.h"
#include "ProtocolThread.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
#include "UIVoyageSlider.h"
#include "UICenter.h"
#include "TVSingle.h"

#include "UITips.h"
#include "UISocial.h"
#include "UIEmail.h"
#include "UIMain.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "UICommon.h"
#include "UIInform.h"
#include "MainTaskManager.h"
#include "TVSceneLoader.h"
#include "Utils.h"
#include "TVSDHeaders.h"
#include "Localization.h"

static int calc_victory_percent(int my_power, int npc_power){
	int victorypercent = my_power * my_power * my_power * my_power
		/ (my_power * my_power * my_power * my_power + npc_power * npc_power * npc_power * npc_power) * 100.0;
	if (victorypercent <= 2)
		victorypercent = 0;
	if (victorypercent >= 98)
		victorypercent = 100;
	return victorypercent;
}

UITask*  UITask::m_taskLayer = nullptr; 
UITask::UITask():
	m_mainTask(false),
	m_pTempButton(nullptr),
	m_userTask(nullptr),
	m_pCurTaskDefine(nullptr),
	m_pCompanionsTaskDefine(nullptr),
	m_pMainTaskResult(nullptr),
	m_pNpcScoreResult(nullptr)
{
	m_nTaskFalg = 0;

	m_nCurrentTaskId = 0;
	m_nCurrentTaskStatus = 0;
	m_nCompletedAllTasks = 0;
	m_bIsLevelUp = false;
	m_bIsCaptainUp = false;
	m_bIsPrestigeUp = false;
	m_bIsPrayer = false;
	m_nTaskIndex = 0;
	m_nCompleteBoundId = 0;
	nomalTaskIndex = NOMAL_ROB_TASK;
	m_nTotalDays = 0;
	m_nSublineId = 0;
	m_bSublineFinish = false;
}

UITask::~UITask()
{
	unregisterCallBack();
	this->unschedule(schedule_selector(UITask::everySecondReflush));
}

bool UITask::init()
{
	if(UIBasicLayer::init())
	{
		auto mainTaskClickBoo = UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName("MAINTASKCLICKBOOL").c_str());
		if (mainTaskClickBoo)
		{
			m_mainTask = true;
		}
		ProtocolThread::GetInstance()->getUserTasks(UILoadingIndicator::createWithMask(this, m_cameraMask));
		return true;
	}
	return false;
}

void UITask::openTaskLayer(int index, Node *parent, int cameraMask)
{
	if(parent){
		parent->addChild(this,10);
	}else{
		auto currentScene = Director::getInstance()->getRunningScene();
		currentScene->addChild(this,10);
	}
	m_nTaskIndex = index;
	this->setPosition(STARTPOS);
	m_cameraMask = cameraMask;
	this->setCameraMask((unsigned short)cameraMask, true);
	registerCallBack();
	init();
}

void UITask::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
		case PROTO_TYPE_HandleTaskResult:
		{
			HandleTaskResult *result = (HandleTaskResult*)message;
			if (result->failed == 0)
			{
				if (result->actioncode == 0)
				{
					closeView();
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_PUP_GET_TASK_SUCCESS");
				}
				else if (result->actioncode == 1)
				{
					closeView();
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_PUP_GET_REWARD_SUCCESS");
				}
				else if (result->actioncode == 2)
				{
					m_pHandleTaskResult = result;
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
						}
					}
					openGetRewardDialog(result);

					auto t_title = m_pTempButton->getChildByName<Text*>("label_quest_name");
					t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SIDE_NO_TASK_NAME"]);
					auto view = getViewRoot("cocosstudio/voyageUI_quest.csb");
					auto l_taskInfo = view->getChildByName<ListView*>("listview_task_info");
					l_taskInfo->removeAllChildrenWithCleanup(true);
					auto p_name_0 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_task_name"));
					auto p_name = p_name_0->clone();
					l_taskInfo->addChild(p_name);
					auto i_task_type = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_name, "image_quest_ship"));
					i_task_type->setVisible(false);
					auto t_name = p_name->getChildByName<Text*>("label_task_name");
					auto i_div_0 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_div_1"));
					auto i_div_1 = i_div_0->clone();
					l_taskInfo->addChild(i_div_1);
					auto t_story_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_task_story"));
					auto t_story = dynamic_cast<Text*>(t_story_0->clone());
					l_taskInfo->addChild(t_story);
					//显示当前无支线任务时的说明
					t_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_SIDE_NO_TASK_TITLE"]);
					std::string str = SINGLE_SHOP->getTipsInfo()["TIP_SIDE_NO_TASK_DESC"];
					t_story->setString(str);
					t_story->setContentSize(Size(t_story->getContentSize().width, getLabelHight(str, t_story->getContentSize().width,t_story->getFontName())));
					auto b_reward = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_get_reward"));
					b_reward->setVisible(false);
					b_reward->setTouchEnabled(false);

					ProtocolThread::GetInstance()->getUserTasks(UILoadingIndicator::createWithMask(this, m_cameraMask));

					//客户端统计玩家任务行为
					//Utils::onTaskCompleted(StringUtils::format("side_task_%d", result->task->taskid));task为空
					Utils::onTaskCompleted(StringUtils::format("side_task_%d", m_nCompleteBoundId));
				}
				else
				{
					closeView();
					ProtocolThread::GetInstance()->getUserTasks(UILoadingIndicator::createWithMask(this, m_cameraMask));
				
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_PUPLAYER_GIVEUP_TASK_SUCCESS");

					//客户端统计玩家任务行为
					Utils::onTaskFailed(StringUtils::format("side_task_%d", m_pCurTaskDefine->taskid), "quit");
				}
			}
			else if (result->failed == 103)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_BAG_FULL");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
		case PROTO_TYPE_GetUserTasksResult:
		{
			GetUserTasksResult *userTask = (GetUserTasksResult*)message;
			mainId.clear();
			sideId.clear();
			nomalId.clear();
			m_userTask = userTask;
			if (userTask->failed == 0)
			{
				//当前主线任务id
				m_nCurrentTaskId = userTask->maintaskid;
				//0 not finish,1 finish
				m_nCurrentTaskStatus = userTask->maintaskstatus;
				//0 not finish,1 finish
				m_nCompletedAllTasks = userTask->completedalltasks;
				//支线任务
				m_pCurTaskDefine = userTask->sidetask;
				//小伙伴剧情任务
				m_pCompanionsTaskDefine = userTask->companionstasks;
				//
				if (m_nCurrentTaskId>=0)
				{ 
					auto completedChapter = 0;
					if (m_nCompletedAllTasks == 1)
					{
						mainId.push_back(0);
						completedChapter = SINGLE_SHOP->getMainTaskInfo()[m_nCurrentTaskId].chapter_idx;
					}
					else
					{
						mainId.push_back(m_nCurrentTaskId);
						completedChapter = SINGLE_SHOP->getMainTaskInfo()[m_nCurrentTaskId].chapter_idx - 1;
					}
					for (int i = completedChapter; i >= 1; i--)
					{
						mainId.push_back(i);
					}
				}
				else
				{
					mainId.push_back(m_nCurrentTaskId);
				}
				if (m_pCurTaskDefine || m_pCompanionsTaskDefine)
				{
					if (m_pCurTaskDefine)
					{
						int len = 100;
						int *npcId = new int[len];
						npcId[0] = 0;

						auto coreId = m_pCurTaskDefine->taskcoreid;
						if (coreId)
						{
							auto type = SINGLE_SHOP->getCoreTaskById(m_pCurTaskDefine->taskcoreid).type;
							if (type == 1)
							{
								npcId[1] = SINGLE_SHOP->getCoreTaskById(m_pCurTaskDefine->taskcoreid).finish_target_npcId[m_pCurTaskDefine->npcindex];
								ProtocolThread::GetInstance()->getNpcScore(npcId, 2, UILoadingIndicator::createWithMask(this, m_cameraMask));
							}
						}

						if (npcId)
						{
							delete[] npcId;
						}
						sideId.push_back(m_pCurTaskDefine->taskid);
					}
					if (m_pCompanionsTaskDefine && userTask->n_companionstasks)
					{
						for (int i = 0; i < userTask->n_companionstasks;i++)
						{
							sideId.push_back(m_pCompanionsTaskDefine[i]->taskid);
						}
					}
			    }
			    else
			    {
					sideId.push_back(0);
			    }

				//TODO
				m_nSublineId = 1;
				m_bSublineFinish = true;
				if (m_userTask->prayremaintime > 0)
				{
					m_bIsPrayer = false;
				}
				else
				{
					m_bIsPrayer = true;
				}
				flushLeftTaskButton();
			    everySecondReflush(0);
			    this->schedule(schedule_selector(UITask::everySecondReflush), 1.0f);
			}
			else
			{
				//TODO: 错误情况处理
				mainId.push_back(m_nCurrentTaskId);
				flushLeftTaskButton();
			}
			break;
		}
		case PROTO_TYPE_GetNpcScoreResult:
		{
			GetNpcScoreResult* result = (GetNpcScoreResult*)message;
			if (result->failed == 0)
			{
				m_pNpcScoreResult = result;
				flushLeftTaskButton();
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_FLUSH_CAPTAINLIST_FAIL");
			}
			break;
		}
		case PROTO_TYPE_GetMainTaskRewardResult:
		{
			GetMainTaskRewardResult *taskReward = (GetMainTaskRewardResult*)message;
			m_pMainTaskResult = taskReward;
			if (taskReward->failed == 0)
			{
				SINGLE_HERO->m_iExp = taskReward->exp;
				SINGLE_HERO->m_iRexp = taskReward->fame;
				if (taskReward->newlevel)
				{
					m_bIsLevelUp = true;
					SINGLE_HERO->m_iLevel = taskReward->newlevel;
				}
				if (taskReward->new_fame_level)
				{
					m_bIsPrestigeUp = true;
					SINGLE_HERO->m_iPrestigeLv = taskReward->new_fame_level;
				}
				if (taskReward->n_captains > 0)
				{
					//有船长增加的经验大于0是才显示界面
					for (int i = 0; i< taskReward->n_captains; i++)
					{
						if (taskReward->captains[i]->lootexp > 0)
						{
							m_bIsCaptainUp = true;
							break;
						}
					}
				}
				openMianTaskGetRewardDialog(taskReward);
				ProtocolThread::GetInstance()->getUserTasks(UILoadingIndicator::createWithMask(this, m_cameraMask));

				auto currentScene = Director::getInstance()->getRunningScene();
				auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
				if (mainlayer)
				{
					mainlayer->flushCionAndGold(taskReward->coins, taskReward->golds);
				}
			}
			else
			{
				//TODO: 错误情况处理
			}
			break;
		}
		//日常任务（打劫活动和赏金猎人活动奖励）
		case PROTO_TYPE_GetDailyActivitiesRewardResult:
		{
			GetDailyActivitiesRewardResult *taskReward = (GetDailyActivitiesRewardResult*)message;
			if (taskReward->failed == 0)
			{
				openNomalGetRewardDialog(taskReward);
				ProtocolThread::GetInstance()->getUserTasks(UILoadingIndicator::createWithMask(this, m_cameraMask));

				auto currentScene = Director::getInstance()->getRunningScene();
				auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
				if (mainlayer)
				{
					mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, taskReward->golds);
				}
			}
			else
			{
				//TODO: 错误情况处理
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
		case PROTO_TYPE_GetPretaskItemsResult:
		{
			GetPretaskItemsResult *preTask = (GetPretaskItemsResult*)message;
			if (preTask->failed == 0)
			{
				if (preTask->addcoin > 0 || preTask->bank > 0 || preTask->n_items > 0)
				{
					getNpcPresentItem(preTask);
				}
			}
			else
			{

			}
			break;
		}
		case PROTO_TYPE_GetPrayResult:
		{
			auto result = (GetPrayResult*)message;
			if (result->failed == 0)
			{
				openPrayDialog(result);
			}
			else
			{

			}
			break;
		}
		case PROTO_TYPE_GetAttackPirateRankInfoResult:
		{
			GetAttackPirateRankInfoResult * result = (GetAttackPirateRankInfoResult*)message;
			if (result->failed == 0)
			{
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushBossDamageRank(result, true);
			}
			break;
		}
	default:
		break;
	}
}

void UITask::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* button = (Widget*)pSender;
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//主线任务
	if(isButton(button_title_story))
	{
		m_mainTask = !m_mainTask;
		UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName("MAINTASKCLICKBOOL").c_str(), m_mainTask);
		auto mainTaskClickIndex = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("MAINTASKCLICKINDEX").c_str(), -1);
		if (mainTaskClickIndex > 0 && mainTaskClickIndex < mainId.size())
		{
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("MAINTASKCLICKINDEX").c_str(), 0);
		}
		UserDefault::getInstance()->flush();
		flushLeftTaskButton();
		return;
	}
	//支线任务 事件任务
	if (isButton(button_main_quest))
	{
		if (m_pTempButton)
		{
			m_pTempButton->setBright(true);
		}

		m_pTempButton = button;
		m_pTempButton->setBright(false);
		
		flushTaskInfo(m_pTempButton->getTag());
		return;
	}
	//返回主城
	if (isButton(button_back))
	{
		closeView();
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		this->removeAllChildrenWithCleanup(true);
		m_taskLayer = nullptr;
		this->unschedule(schedule_selector(UITask::everySecondReflush));
		//主线任务完成对话
		if (MainTaskManager::GetInstance()->checkTaskAction())
		{
			addNeedControlWidgetForHideOrShow(nullptr, true);
			allTaskInfo(false, MAIN_STORY);
			MainTaskManager::GetInstance()->notifyUIPartDelayTime(0.5f);
		}
	}
	//关闭界面
	if (isButton(button_close))
	{
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		this->removeAllChildrenWithCleanup(true);
	}
	//任务奖励物品详情
	if (isButton(button_items_1) || isButton(button_items_2))
	{
		int type = button->getChildByName<ImageView*>("image_goods")->getTag();
		int id = button->getTag();
		openGoodInfo(nullptr, type, id);
		return;
	}
	//接受任务
	if (isButton(button_accept))
	{
		return;
	}
	//放弃任务
	if (isButton(button_abort))
	{
		openTaskConfirmDialog(button->getTag() - START_INDEX, false);
		return;
	}
	//领取奖励
	if (isButton(button_get_reward))
	{
		auto i_notify_reward = button->getChildByName<ImageView*>("image_notify_1");
		switch (m_task)
		{
		case MAIN_TASK:
		{
						  UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(MAIN_TASK_FINISH).c_str(), false);
						  UserDefault::getInstance()->flush();
						  i_notify_reward->setVisible(false);
						  auto i_notify_main = m_pTempButton->getChildByName<ImageView*>("image_notify_3");
						  i_notify_main->setVisible(false);
						  ProtocolThread::GetInstance()->getMainTaskReward(UILoadingIndicator::createWithMask(this, m_cameraMask));
		}
			break;
		case PARTNER_TASK:
		{
						  i_notify_reward->setVisible(false);
						  auto i_notify_main = m_pTempButton->getChildByName<ImageView*>("image_notify_3");
						  i_notify_main->setVisible(false);
						  //ProtocolThread::GetInstance()->getMainTaskReward(LoadingLayer::createWithMask(this, m_cameraMask));
		}
			break;
		case SIDE_TASK:
		{
						  if (m_pCurTaskDefine->completecityid == m_userTask->cityid || m_userTask->bargirlfelling >= FAVOUR_SIDE_TASK)
						  {
							  i_notify_reward->setVisible(false);
							  auto i_notify_side = m_pTempButton->getChildByName<ImageView*>("image_notify_3");
							  i_notify_side->setVisible(false);
							  m_nCompleteBoundId = button->getTag() - START_INDEX;
							  ProtocolThread::GetInstance()->handleTask(button->getTag() - START_INDEX, 2, UILoadingIndicator::createWithMask(this, m_cameraMask));
						  }
						  else
						  {
							  UIInform::getInstance()->openInformView(this);
							  UIInform::getInstance()->openConfirmYes("TIP_PUP_GET_REWARD_NOTCITY");
						  }
		}
			break;
		case NOMAL_TASK:
		{
						   i_notify_reward->setVisible(false);
						   auto i_notify_main = m_pTempButton->getChildByName<ImageView*>("image_notify_3");
						   i_notify_main->setVisible(false);
						   if (button->getTag() < 7)
						   {
							   ProtocolThread::GetInstance()->getDailyActivitiesReward(nomalTaskIndex, UILoadingIndicator::createWithMask(this, m_cameraMask));
						   } 
						   else
						   {
							   openSublineRewardDialog();
							   //TODO
						   }
						  
		}
			break;
		default:
			break;
		}
		return;
	}
	//确认领奖
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
			if (m_task == MAIN_TASK)
			{
				UICommon::getInstance()->flushCaptainLevelUp(m_pMainTaskResult->n_captains, m_pMainTaskResult->captains);
			}
			else
			{
				UICommon::getInstance()->flushCaptainLevelUp(m_pHandleTaskResult->n_captains, m_pHandleTaskResult->captains);
			}
			return;
		}

		//玩家获得的系统道具
		if (m_task == MAIN_TASK)
		{
			ProtocolThread::GetInstance()->getPretaskItems(UILoadingIndicator::createWithMask(this, m_cameraMask));
		}
		return;
	}

	//玩家升级
	if (isButton(panel_levelup))
	{
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
			if (m_task == MAIN_TASK)
			{
				UICommon::getInstance()->flushCaptainLevelUp(m_pMainTaskResult->n_captains, m_pMainTaskResult->captains);
			}
			else
			{
				UICommon::getInstance()->flushCaptainLevelUp(m_pHandleTaskResult->n_captains, m_pHandleTaskResult->captains);
			}
			return;
		}
		//玩家获得的系统道具
		if (m_task == MAIN_TASK)
		{
			ProtocolThread::GetInstance()->getPretaskItems(UILoadingIndicator::createWithMask(this, m_cameraMask));
		}
		return;
	}
	//声望升级
	if (isButton(panel_r_levelup))
	{
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp();
			if (m_task == MAIN_TASK)
			{
				UICommon::getInstance()->flushCaptainLevelUp(m_pMainTaskResult->n_captains, m_pMainTaskResult->captains);
			}
			else
			{
				UICommon::getInstance()->flushCaptainLevelUp(m_pHandleTaskResult->n_captains, m_pHandleTaskResult->captains);
			}
			return;
		}
		//玩家获得的系统道具
		if (m_task == MAIN_TASK)
		{
			ProtocolThread::GetInstance()->getPretaskItems(UILoadingIndicator::createWithMask(this, m_cameraMask));
		}
		return;
	}
	//船长升级
	if (isButton(panel_captain_addexp) || isButton(button_skip))
	{
		//玩家获得的系统道具
		if (m_task == MAIN_TASK)
		{
			ProtocolThread::GetInstance()->getPretaskItems(UILoadingIndicator::createWithMask(this, m_cameraMask));
			return;
		}
		return;
	}
	//支线任务提交
	if (isButton(button_s_yes))
	{
		if (button->getTag() < 20 * START_INDEX)
		{
			ProtocolThread::GetInstance()->handleTask(button->getTag() - START_INDEX, 0, UILoadingIndicator::createWithMask(this, m_cameraMask));
		}
		else
		{
			auto i_notify_side = m_pTempButton->getChildByName<ImageView*>("image_notify_3");
			i_notify_side->setVisible(false);
			//auto i_notify_abort = button->getChildByName<ImageView*>("image_notify_2");
			//i_notify_abort->setVisible(false);
			ProtocolThread::GetInstance()->handleTask(button->getTag() - 20 * START_INDEX, 3, UILoadingIndicator::createWithMask(this, m_cameraMask));
		}
		return;
	}
	//关闭界面
	if (isButton(button_s_no))
	{
		this->unscheduleUpdate();
		closeView();
		return;
	}
	//关闭提示框界面
	if (isButton(button_present_yes) || isButton(button_yes))
	{
		closeView();
		return;
	}
	
	if (isButton(button_pray))
	{
		ProtocolThread::GetInstance()->getPray(button->getTag(), UILoadingIndicator::createWithMask(this, m_cameraMask));
		auto i_notify_main = m_pTempButton->getChildByName<ImageView*>("image_notify_3");
		i_notify_main->setVisible(false);
		m_bIsPrayer = false;
		return;
	}

	if (isButton(button_attend))
	{
		if (m_userTask->pirateattackstatus)
		{
			ProtocolThread::GetInstance()->getPirateAttackInfo(UILoadingIndicator::createWithMask(this, m_cameraMask));
			closeView();
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			this->removeAllChildrenWithCleanup(true);
			m_taskLayer = nullptr;
			this->unschedule(schedule_selector(UITask::everySecondReflush));
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_ACTIVITY_NOT_STARTE");
		}
		return;
	}

	if (isButton(button_ranking))
	{
		ProtocolThread::GetInstance()->getAttackPirateRankInfo(UILoadingIndicator::createWithMask(this, m_cameraMask));
		return;
	}

	if (isButton(button_boss_close))
	{
		UICommon::getInstance()->closeView(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RANKING_CSB]);
		return;
	}
	if (isButton(image_silver) || isButton(image_v) ||isButton(image_r))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(button);
		return;
	}
}

void UITask::onEnter()
{
	UIBasicLayer::onEnter();
	ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UITask::onServerEvent, this),this);
}

void UITask::onExit()
{
	UIBasicLayer::onExit();
}

void UITask::flushLeftTaskButton()
{
	openView("cocosstudio/voyageUI_quest.csb");
	auto view = getViewRoot("cocosstudio/voyageUI_quest.csb");
	auto mainTaskClickIndex = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("MAINTASKCLICKINDEX").c_str(), -1);
	auto clickIndex = 0;
	if (mainTaskClickIndex > 0)
	{
		clickIndex = mainTaskClickIndex;
	}
	//显示海盗基地
	if (m_nTaskIndex == 1)
	{
		clickIndex = -1;
	}

	int index = 0;

	m_pTempButton = nullptr;
	view->setPosition(STARTPOS);
	auto l_taskButton = view->getChildByName<ListView*>("listview_left");
	auto b_mainButton = view->getChildByName<ImageView*>("button_title_story");
	auto b_taskButton = view->getChildByName<Button*>("button_main_quest");
	l_taskButton->removeAllChildrenWithCleanup(true);
	//主线任务
	auto b_item1 = b_mainButton->clone();
	b_item1->setTouchEnabled(true);
	auto t_title1 = b_item1->getChildByName<Text*>("label_quest_name");
	t_title1->setString(SINGLE_SHOP->getTipsInfo()["TIP_TASK_BUTTON_TITLE1"]);
	auto b_unfold1 = b_item1->getChildByName<Button*>("button_unfold");
	b_unfold1->setVisible(true);
	b_unfold1->setBright(m_mainTask);
	l_taskButton->pushBackCustomItem(b_item1);

	if (m_nCurrentTaskId != 0)
	{
		if (m_mainTask == false)
		{
			//显示当前主线任务 
			auto item = b_taskButton->clone();
			item->setTag(index);
			if (index == clickIndex)
			{
				m_pTempButton = item;
				item->setBright(false);
			}
			
			auto t_title = item->getChildByName<Text*>("label_quest_name");
			if (mainId[0] == 0)
			{
				auto str = StringUtils::format("TIP_MAIN_NO_TASK_NAME_%d", SINGLE_HERO->m_iNation);
				t_title->setString(SINGLE_SHOP->getTipsInfo()[str]);
			}
			else
			{
				t_title->setString(SINGLE_SHOP->getMainTaskInfo()[mainId[0]].name);
				if (m_nCurrentTaskStatus == 1)
				{
					auto i_notify_main = item->getChildByName<ImageView*>("image_notify_3");
					i_notify_main->setVisible(true);
				}
			}
			l_taskButton->pushBackCustomItem(item);
		}
		else
		{
			//显示之前做过的主线任务
			//MAINTASKSTORYINFO story = SINGLE_SHOP->getMainTaskStoryByNationAndChapter(SINGLE_SHOP->getMainTaskInfo()[mainId[0]].nation, 1);

			if (mainId.size() >= 1)
			{				
				for (int i = mainId.size() - 1; i >= 0; i--)
				{
					auto item = b_taskButton->clone();
					item->setTag(i);
					index = i;
					if (index == clickIndex)
					{
						m_pTempButton = item;
						item->setBright(false);
					}
					
					auto text = SINGLE_SHOP->getMainTaskInfo()[mainId[i]].name;
					if (i == 0)
					{
						if (mainId[i] == 0)
						{
							auto str = StringUtils::format("TIP_MAIN_NO_TASK_NAME_%d", SINGLE_HERO->m_iNation);
							text = SINGLE_SHOP->getTipsInfo()[str];
						}
						else
						{
							if (m_nCurrentTaskStatus == 1)
							{
								auto i_notify_main = item->getChildByName<ImageView*>("image_notify_3");
								i_notify_main->setVisible(true);
							}
						}
						
					}
					else
					{
						m_story = SINGLE_SHOP->getMainTaskStoryByNationAndChapter(SINGLE_HERO->m_iNation, mainId[i]);
						text = m_story.name;
						//item->setBright(true);
						text += " (";
						text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_FINISH"];
						text += ")";
					}
					auto t_title = item->getChildByName<Text*>("label_quest_name");
					t_title->setString(text);
					l_taskButton->pushBackCustomItem(item);
				}
			}
		}
	}
	else
	{
		//无
		if (m_mainTask == false)
		{
			auto item = b_taskButton->clone();
			item->setTag(index);
			if (index == clickIndex)
			{
				m_pTempButton = item;
				item->setBright(false);
			}
			auto t_title = item->getChildByName<Text*>("label_quest_name");
			auto str = StringUtils::format("TIP_MAIN_NO_TASK_NAME_%d",SINGLE_HERO->m_iNation);
			t_title->setString(SINGLE_SHOP->getTipsInfo()[str]);
			l_taskButton->pushBackCustomItem(item);
		}
		else
		{
			//显示之前做过的主线任务
			for (int i = 0; i < mainId.size(); i++)
			{
				auto item = b_taskButton->clone();
				item->setTag(i);
				index = i;
				if (index == clickIndex)
				{
					m_pTempButton = item;
					item->setBright(false);
				}
				auto str = StringUtils::format("TIP_MAIN_NO_TASK_NAME_%d", SINGLE_HERO->m_iNation);
				auto text = SINGLE_SHOP->getTipsInfo()[str];

				if (i == 0)
				{
					//m_pTempButton = item;
					//item->setBright(false);
				}
				else
				{
					m_story = SINGLE_SHOP->getMainTaskStoryByNationAndChapter(SINGLE_HERO->m_iNation, mainId[i]);
					text = m_story.name;
					//item->setBright(true);
					text += " (";
					text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_FINISH"];
					text += ")";
				}
				auto t_title = item->getChildByName<Text*>("label_quest_name");
				t_title->setString(text);
				l_taskButton->pushBackCustomItem(item);
			}
		}
	}
	//支线任务（赏金榜）及小伙伴剧情任务
	auto b_item2 = b_mainButton->clone();
	b_item2->setTouchEnabled(false);
	auto t_title2 = b_item2->getChildByName<Text*>("label_quest_name");
	t_title2->setString(SINGLE_SHOP->getTipsInfo()["TIP_TASK_BUTTON_TITLE2"]);
	auto i_unfold2 = b_item2->getChildByName<Widget*>("button_unfold");
	i_unfold2->setVisible(false);
	l_taskButton->pushBackCustomItem(b_item2);

	if (sideId.size() > 1)
	{
		for (int i = 0; i < sideId.size(); i++)
		{
			auto item = b_taskButton->clone();
			index = mainId.size() + i;
			item->setTag(index);
			if (index == clickIndex)
			{
				m_pTempButton = item;
				item->setBright(false);
			}
			auto t_title = item->getChildByName<Text*>("label_quest_name");
			auto i_notify_side = item->getChildByName<ImageView*>("image_notify_3");
			if (i == 0)
			{
				if (m_pCurTaskDefine)
				{
					std::string text;
					text += SINGLE_SHOP->getSideTaskInfo()[m_pCurTaskDefine->taskid].title;
					text += " (";
					long now = time(NULL);
					if (m_pCurTaskDefine->surplustime > 0)
					{
						if (m_pCurTaskDefine->taskstatus)
						{
							m_nTaskFalg = 2;
							text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_FINISH"];
							i_notify_side->setVisible(true);
						}
						else
						{
							m_nTaskFalg = 1;
							text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_CONTINUE"];
						}
					}
					else
					{
						//当前的支线任务超时
						m_nTaskFalg = -1;
						i_notify_side->setVisible(true);
						text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_TIEMOUT"];
					}
					text += ")";
					t_title->setString(text);
				}
				else if (m_pCompanionsTaskDefine)
				{
					t_title->setString(SINGLE_SHOP->getPartnerDialogJudgeInfo()[sideId[i]].title);
				}
			}
			else
			{
				if (m_pCompanionsTaskDefine)
				{
					t_title->setString(SINGLE_SHOP->getPartnerDialogJudgeInfo()[sideId[i]].title);
				}
			}
			l_taskButton->pushBackCustomItem(item);
		}
	}
	else
	{
		//无
		auto item = b_taskButton->clone();
		index = mainId.size();
		item->setTag(index);
		if (index == clickIndex)
		{
			m_pTempButton = item;
			item->setBright(false);
		}
		auto t_title = item->getChildByName<Text*>("label_quest_name");
		auto i_notify_side = item->getChildByName<ImageView*>("image_notify_3");
		if (m_pCurTaskDefine)
		{
			std::string text;
			text += SINGLE_SHOP->getSideTaskInfo()[m_pCurTaskDefine->taskid].title;
			text += " (";
			long now = time(NULL);
			if (m_pCurTaskDefine->surplustime > 0)
			{
				if (m_pCurTaskDefine->taskstatus)
				{
					m_nTaskFalg = 2;
					text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_FINISH"];
					i_notify_side->setVisible(true);
				}
				else
				{
					m_nTaskFalg = 1;
					text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_CONTINUE"];
				}
			}
			else
			{
				//当前的支线任务超时
				m_nTaskFalg = -1;
				i_notify_side->setVisible(true);
				text += SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_TIEMOUT"];
			}
			text += ")";
			t_title->setString(text);
		}
		else if (m_pCompanionsTaskDefine)
		{
			t_title->setString(SINGLE_SHOP->getPartnerDialogJudgeInfo()[sideId[0]].title);
		}
		else
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SIDE_NO_TASK_NAME"]);
		}
		l_taskButton->pushBackCustomItem(item);
	}
	
	//事件任务
	auto b_item3 = b_mainButton->clone();
	b_item3->setTouchEnabled(false);
	auto t_title3 = b_item3->getChildByName<Text*>("label_quest_name");
	t_title3->setString(SINGLE_SHOP->getTipsInfo()["TIP_TASK_BUTTON_TITLE3"]);
	auto i_unfold3 = b_item3->getChildByName<Widget*>("button_unfold");
	i_unfold3->setVisible(false);
	l_taskButton->pushBackCustomItem(b_item3);
	
	auto indexC = index + 1;
	int num = 7;
	int faith_index = 4;
	if (m_userTask->levelenough == 0)
	{
		num = 5;
		faith_index = 2;
	}
	
	for (int i = 0; i < num; i++)
	{
		auto item = b_taskButton->clone();
		index = indexC + i;
		item->setTag(index);
		auto t_title = item->getChildByName<Text*>("label_quest_name");
		auto i_notify_side = item->getChildByName<ImageView*>("image_notify_3");
		auto signNum = 0;
		auto countNum = 0;
		if ((m_userTask->levelenough == 1 && i == faith_index) || (m_userTask->levelenough == 0 && i == faith_index))
		{
			if (m_bIsPrayer)
			{
				i_notify_side->setVisible(true);
			}
			else
			{
				i_notify_side->setVisible(false);
			}
		}
		else
		{
			i_notify_side->setVisible(false);
		}
		if (i == 0)
		{
			signNum = m_userTask->sign_for_loot_reward;
			countNum = m_userTask->loot_count;
		}
		else if (i == 1)
		{
			signNum = m_userTask->sign_for_tavern_task_reward;
			countNum = m_userTask->tavern_task_count;
		}
		if (signNum != 1 && countNum >= 10)
		{
			i_notify_side->setVisible(true);
		}
		if (m_nTaskIndex == 1 && i == 3)
		{
			m_nTaskIndex = 0;
			clickIndex = index;
		}
		
		if (index == clickIndex)
		{
			m_pTempButton = item;
			item->setBright(false);
		}
		string st_title;
		if (m_userTask->levelenough == 0 && i >= faith_index)
		{
			st_title = StringUtils::format("TIP_EVNETS_TASK_BUTTLE_%d", 2 + i + 1);
		}
		else
		{
			st_title = StringUtils::format("TIP_EVNETS_TASK_BUTTLE_%d", i + 1);
		}

		if (i == num - 1)
		{
			SDGuideTasks *task = SDGuideTasks::GetElement(m_nSublineId);
			t_title->setString(GetLocalStr(task->getname_ids()));
		}
		else
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()[st_title]);
		}
		l_taskButton->pushBackCustomItem(item);
	}

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown_left");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 0.5);
	addListViewBar(l_taskButton, image_pulldown);
	
	flushTaskInfo(clickIndex);
	this->setCameraMask(_cameraMask, true);
}

void UITask::flushTaskInfo(int index)
{
	UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("MAINTASKCLICKINDEX").c_str(), index);
	UserDefault::getInstance()->flush();
	auto view = getViewRoot("cocosstudio/voyageUI_quest.csb");
	auto i_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver"));
	i_silver->setTouchEnabled(true);
	i_silver->addTouchEventListener(CC_CALLBACK_2(UITask::menuCall_func, this));
	i_silver->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
	auto i_v = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_v"));
	i_v->setTouchEnabled(true);
	i_v->addTouchEventListener(CC_CALLBACK_2(UITask::menuCall_func, this));
	i_v->setTag(IMAGE_ICON_VTICKET + IMAGE_INDEX2);
	auto i_r = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_r"));
	i_r->setTouchEnabled(true);
	i_r->addTouchEventListener(CC_CALLBACK_2(UITask::menuCall_func, this));
	i_r->setTag(IMAGE_ICON_PRESTIGE + IMAGE_INDEX2);
	auto l_taskInfo = view->getChildByName<ListView*>("listview_task_info");
	l_taskInfo->removeAllChildrenWithCleanup(true);
	auto p_piratessiege = view->getChildByName<Widget*>("panel_event_piratessiege");
	auto p_siege = view->getChildByName<Widget*>("panel_event_siege");
	auto p_faith = view->getChildByName<Widget*>("panel_event_faith");
	auto p_daily = view->getChildByName<Widget*>("panel_daily");

	auto currentIdNum = 0;
	if (index < mainId.size())
	{
		m_task = MAIN_TASK;
		currentIdNum = mainId[index];
		l_taskInfo->setVisible(true);
		p_piratessiege->setVisible(false);
		p_siege->setVisible(false);
		p_faith->setVisible(false);
		p_daily->setVisible(false);
	}
	else
	{
		for (int i = 0; i < sideId.size(); i++)
		{
			if (m_pCurTaskDefine  && index == mainId.size())
			{
				m_task = SIDE_TASK;
				currentIdNum = m_pCurTaskDefine->taskid;
			}
			else if (m_pCompanionsTaskDefine)
			{
				m_task = PARTNER_TASK;
				currentIdNum = sideId[i];
			}
			else
			{
				m_task = SIDE_TASK;
				currentIdNum = 0;
			}
			l_taskInfo->setVisible(true);
			p_piratessiege->setVisible(false);
			p_siege->setVisible(false);
			p_faith->setVisible(false);
			p_daily->setVisible(false);
		}
		if (index > mainId.size() + sideId.size() - 1)
		{
			m_task = NOMAL_TASK;
			l_taskInfo->setVisible(false);

			currentIdNum = index - mainId.size() - sideId.size() + 1;
		}
	}

	auto p_name_0 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_task_name"));
	auto p_name = p_name_0->clone();
	auto i_task_type = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_name, "image_quest_ship"));
	l_taskInfo->addChild(p_name);
	auto t_name = p_name->getChildByName<Text*>("label_task_name");
	t_name->setPositionX(i_task_type->getBoundingBox().size.width + 4);
	auto p_difficulty_0 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_difficulty"));
	auto p_difficulty = p_difficulty_0->clone();
	l_taskInfo->addChild(p_difficulty);
	auto i_div_0 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_div_1"));
	auto i_div_1 = i_div_0->clone();
	l_taskInfo->addChild(i_div_1);
	auto p_content_0 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_task_content"));
	auto p_content = p_content_0->clone();
	l_taskInfo->addChild(p_content);
	p_content->setContentSize(Size(700,70));
	auto t_content = p_content->getChildByName<Text*>("label_task_content");
	t_content->setVisible(true);
	t_content->setPositionY(p_content->getContentSize().height / 2);
	auto p_time_0 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_time"));
	auto p_time = p_time_0->clone();
	l_taskInfo->addChild(p_time);
	p_time->setName("p_time");
	p_time->setVisible(false);
	auto t_time_label = p_time->getChildByName<Text*>("label_task_time");
	auto t_time_num = p_time->getChildByName<Text*>("label_task_time_num");

	auto p_reward_0 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_reward"));
	auto p_reward = p_reward_0->clone();
	l_taskInfo->addChild(p_reward);
	auto p_common = p_reward->getChildByName<Widget*>("panel_common");
	auto t_silver = p_common->getChildByName<Text*>("label_silver_num");
	auto t_gold = p_common->getChildByName<Text*>("label_v_num");
	auto t_exp = p_common->getChildByName<Text*>("label_exp_num");
	auto t_rep = p_common->getChildByName<Text*>("label_r_num");
	auto p_force = p_reward->getChildByName<Widget*>("panel_force relation");
	p_force->setVisible(false);
	auto i_falg = p_force->getChildByName<ImageView*>("image_country");
	auto t_force_num = p_force->getChildByName<Text*>("label_force_relation_num");
	auto p_equip = p_reward->getChildByName<Widget*>("panel_equip");
	p_equip->setVisible(false);
	auto b_item1 = p_equip->getChildByName<Button*>("button_items_1");
	auto i_itembg1 = p_equip->getChildByName<ImageView*>("image_goods_bg_1");
	auto i_item1 = b_item1->getChildByName<ImageView*>("image_goods");
	auto t_num1 = i_item1->getChildByName<Text*>("text_item_num");
	auto i_itembg2 = p_equip->getChildByName<ImageView*>("image_goods_bg_2");
	auto b_item2 = p_equip->getChildByName<Button*>("button_items_2");
	auto i_item2 = b_item2->getChildByName<ImageView*>("image_goods");
	auto t_num2 = i_item2->getChildByName<Text*>("text_item_num");
	auto i_div_2 = i_div_0->clone();
	l_taskInfo->addChild(i_div_2);
	auto t_story_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_task_story"));
	auto t_story = dynamic_cast<Text*>(t_story_0->clone());
	l_taskInfo->addChild(t_story);

	auto b_accept = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_accept"));
	b_accept->setTouchEnabled(false);
	b_accept->setVisible(false);
	auto b_abort = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_abort"));
	auto i_notify_abort = b_abort->getChildByName<ImageView*>("image_notify_2");
	i_notify_abort->setVisible(false);
	b_abort->setTouchEnabled(false);
	b_abort->setVisible(false);
	auto b_reward = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_get_reward"));
	auto i_notify_reward = b_reward->getChildByName<ImageView*>("image_notify_1");
	i_notify_reward->setVisible(false);
	b_reward->setTouchEnabled(false);
	b_reward->setVisible(false);
	b_reward->setTag(currentIdNum + START_INDEX);
	b_abort->setTag(currentIdNum + START_INDEX);
	b_accept->setTag(currentIdNum + START_INDEX);

	switch (m_task)
	{
	case MAIN_TASK:
	{
						i_task_type->loadTexture(getTaskType(3));
						if (m_nCurrentTaskId != 0 && index == 0 && m_nCompletedAllTasks != 1)
						{
							//显示当前正在做的主线任务	
							i_task_type->setVisible(true);
							l_taskInfo->removeChild(p_difficulty);
							l_taskInfo->removeChild(p_time);
							t_name->setString(SINGLE_SHOP->getMainTaskInfo()[currentIdNum].title);
							auto CoreTaskInfo = SINGLE_SHOP->getCoreTaskById(currentIdNum);
							t_content->setString(SINGLE_SHOP->getMainTaskInfo()[currentIdNum].tip);
							float height = getLabelHight(SINGLE_SHOP->getMainTaskInfo()[currentIdNum].tip, t_content->getContentSize().width, t_content->getFontName(), t_content->getFontSize());
							if (height > t_content->getContentSize().height)
							{
								t_content->setAnchorPoint(Vec2(0, 1));
								t_content->setContentSize(Size(t_content->getContentSize().width, height + t_content->getFontSize()));
								p_content->setContentSize(t_content->getContentSize());
								t_content->setPositionY(t_content->getContentSize().height);
							}

							t_silver->setString(numSegment(StringUtils::format("%d", SINGLE_SHOP->getMainTaskInfo()[currentIdNum].rewards.coin)));
							t_gold->setString(numSegment(StringUtils::format("%d", SINGLE_SHOP->getMainTaskInfo()[currentIdNum].rewards.vticket)));
							t_exp->setString(StringUtils::format("%d", SINGLE_SHOP->getMainTaskInfo()[currentIdNum].rewards.exp));
							t_rep->setString(StringUtils::format("%d", SINGLE_SHOP->getMainTaskInfo()[currentIdNum].rewards.reputation));

							std::string str = SINGLE_SHOP->getMainTaskInfo()[currentIdNum].desc;
							std::string new_value_aide;
							std::string old_value_aide = "[aidename]";
							if (SINGLE_HERO->m_iGender == 1)
							{
								new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"];
							}
							else
							{
								new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"];
							}
							repalce_all_ditinct(str, old_value_aide, new_value_aide);
							t_story->setString(str);
							t_story->setContentSize(Size(t_story->getContentSize().width, 36 + getLabelHight(str, t_story->getContentSize().width, t_story->getFontName())));

							if (SINGLE_SHOP->getMainTaskInfo()[currentIdNum].rewards.force_id > 0)
							{
								p_force->setVisible(true);
								i_falg->ignoreContentAdaptWithSize(false);
								i_falg->loadTexture(getCountryIconPath(SINGLE_SHOP->getMainTaskInfo()[currentIdNum].rewards.force_id));
								t_force_num->setString(StringUtils::format("+%d", SINGLE_SHOP->getMainTaskInfo()[currentIdNum].rewards.force_affect));
							}
							//主线任务暂时不显示items
							if (m_nCurrentTaskStatus)
							{
								b_reward->setVisible(true);
								b_reward->setTouchEnabled(true);
								i_notify_reward->setVisible(true);
							}
						}
						else
						{
							//显示主线任务历史
							i_task_type->setVisible(false);
							t_name->setPositionX(0);
							l_taskInfo->removeChild(p_content);
							l_taskInfo->removeChild(p_difficulty);
							l_taskInfo->removeChild(p_time);
							l_taskInfo->removeChild(p_reward);

							l_taskInfo->removeChild(i_div_2);
							if (m_nCurrentTaskId != 0 && index != 0)
							{
								//显示历史
								m_story = SINGLE_SHOP->getMainTaskStoryByNationAndChapter(SINGLE_HERO->m_iNation, currentIdNum);
								std::string new_value_aide;
								std::string old_value_aide = "[aidename]";
								if (SINGLE_HERO->m_iGender == 1)
								{
									new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"];
								}
								else
								{
									new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"];
								}
								repalce_all_ditinct(m_story.desc, old_value_aide, new_value_aide);
								t_story->setString(m_story.desc);
								t_name->setString(m_story.title);

								t_story->setContentSize(Size(t_story->getContentSize().width, getLabelHight(m_story.desc, t_story->getContentSize().width, t_story->getFontName())));
							}
							else
							{
								//显示当前无主线任务时的说明
								auto str = StringUtils::format("TIP_MAIN_NO_TASK_TITLE_%d", SINGLE_HERO->m_iNation);
								t_name->setString(SINGLE_SHOP->getTipsInfo()[str]);
								str = StringUtils::format("TIP_MAIN_NO_TASK_DESC_%d", SINGLE_HERO->m_iNation);
								t_story->setString(SINGLE_SHOP->getTipsInfo()[str]);
								t_story->setContentSize(Size(t_story->getContentSize().width, getLabelHight(str, t_story->getContentSize().width, t_story->getFontName())));
							}
						}
						break;
	}
	case SIDE_TASK:
	{
						if (m_pCurTaskDefine)
						{
							p_time->setVisible(true);
							t_name->setString(SINGLE_SHOP->getSideTaskInfo()[currentIdNum].title);
							std::string content;
							std::string new_vaule;
							std::string old_vaule;
							std::string html_color_begin1;
							std::string html_color_begin;
							std::string html_color_end;

#if WIN32
							html_color_begin1 = "";
							html_color_begin = "";
							html_color_end = "";
#else
							//默认深棕色
							html_color_begin1 = "<font size='24' color='#2E1D0E'>";
							html_color_begin = "<font size='24' color='#A95601'>";
							html_color_end = "</font>";
							if (currentIdNum != m_pCurTaskDefine->taskid)
							{
								//浅棕色
								html_color_begin = "<font size='24' color='#A95601'>";
							}
							else
							{
								long now = time(NULL);
								if (m_pCurTaskDefine->surplustime <= 0)
								{
									//红色
									html_color_begin = "<font size='24' color='#CD1919'>";
								}
								else
								{
									//浅棕色
									html_color_begin = "<font size='24' color='#A95601'>";
								}
							}
							if (m_nTaskFalg == 2 && currentIdNum == m_pCurTaskDefine->taskid)
							{
								//任务完成
								//绿色
								html_color_begin = "<font size='24’ color='#2E7D32'>";
							}
#endif

							auto CoreTaskInfo = SINGLE_SHOP->getCoreTaskById(m_pCurTaskDefine->taskcoreid);

							if (CoreTaskInfo.finish_target_npcId)
							{
								//"击败npc"
								i_task_type->loadTexture(getTaskType(1));
								if (m_nTaskFalg == 2 && currentIdNum == m_pCurTaskDefine->taskid || m_nTaskFalg == -1 && currentIdNum == m_pCurTaskDefine->taskid && m_pCurTaskDefine->taskstatus)
								{
									content += SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_TASK"];
									new_vaule = html_color_end;
									new_vaule += html_color_begin;
									new_vaule += SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->completecityid].name;
									new_vaule += html_color_end;
									new_vaule += html_color_begin1;
									old_vaule = "[city]";
									repalce_all_ditinct(content, old_vaule, new_vaule);
								}
								else
								{
									content += SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_NPC"];
									new_vaule = html_color_end;
									new_vaule += html_color_begin;
									if (m_nTaskFalg != 0 && currentIdNum == m_pCurTaskDefine->taskid)
									{
										new_vaule += StringUtils::format("%d/%d", m_pCurTaskDefine->completeamount, CoreTaskInfo.finish_number);
									}
									else
									{
										new_vaule += StringUtils::format("%d", CoreTaskInfo.finish_number);
									}
									new_vaule += html_color_end;
									new_vaule += html_color_begin1;
									old_vaule = "[num]";
									repalce_all_ditinct(content, old_vaule, new_vaule);
									new_vaule = html_color_end;
									new_vaule += html_color_begin;
									new_vaule += SINGLE_SHOP->getBattleNpcInfo()[CoreTaskInfo.finish_target_npcId[m_pCurTaskDefine->npcindex]].name;
									new_vaule += html_color_end;
									new_vaule += html_color_begin1;
									old_vaule = "[npc]";
									repalce_all_ditinct(content, old_vaule, new_vaule);
									new_vaule = html_color_end;
									new_vaule += html_color_begin;
									new_vaule += SINGLE_SHOP->getZoneInfo()[CoreTaskInfo.zone[m_pCurTaskDefine->npcindex]];
									new_vaule += html_color_end;
									new_vaule += html_color_begin1;
									old_vaule = "[zone]";
									repalce_all_ditinct(content, old_vaule, new_vaule);
								}
							}

							if (CoreTaskInfo.finish_goods_id)
							{
								//"运送物资"
								i_task_type->loadTexture(getTaskType(2));
								if (m_nTaskFalg == 2 && currentIdNum == m_pCurTaskDefine->taskid || m_nTaskFalg == -1 && currentIdNum == m_pCurTaskDefine->taskid && m_pCurTaskDefine->taskstatus)
								{
									content += SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_TASK"];
									new_vaule = html_color_end;
									new_vaule += html_color_begin;
									new_vaule += SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->completecityid].name;
									new_vaule += html_color_end;
									new_vaule += html_color_begin1;
									old_vaule = "[city]";
									repalce_all_ditinct(content, old_vaule, new_vaule);
								}
								else
								{
									content += SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_GOODS"];
									new_vaule = html_color_end;
									new_vaule += html_color_begin;
									if (m_nTaskFalg == 2 && currentIdNum == m_pCurTaskDefine->taskid)
									{
										new_vaule += StringUtils::format("%d/%d", CoreTaskInfo.finish_goods_amount[m_pCurTaskDefine->npcindex], CoreTaskInfo.finish_goods_amount[m_pCurTaskDefine->npcindex]);
									}
									else if (m_nTaskFalg == 1 && currentIdNum == m_pCurTaskDefine->taskid)
									{
										if (m_pCurTaskDefine->completeamount)
										{
											new_vaule += StringUtils::format("%d/%d", m_pCurTaskDefine->completeamount, CoreTaskInfo.finish_goods_amount[m_pCurTaskDefine->npcindex]);
										}
										else
										{
											new_vaule += StringUtils::format("%d/%d", 0, CoreTaskInfo.finish_goods_amount[m_pCurTaskDefine->npcindex]);
										}
									}
									else
									{
										new_vaule += StringUtils::format("%d", CoreTaskInfo.finish_goods_amount[m_pCurTaskDefine->npcindex]);
									}
									new_vaule += html_color_end;
									new_vaule += html_color_begin1;
									old_vaule = "[num]";
									repalce_all_ditinct(content, old_vaule, new_vaule);
									new_vaule = html_color_end;
									new_vaule += html_color_begin;
									new_vaule += SINGLE_SHOP->getGoodsData()[CoreTaskInfo.finish_goods_id[m_pCurTaskDefine->npcindex]].name;
									new_vaule += html_color_end;
									new_vaule += html_color_begin1;
									old_vaule = "[good]";
									repalce_all_ditinct(content, old_vaule, new_vaule);
									new_vaule = html_color_end;
									new_vaule += html_color_begin;
									new_vaule += SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->targetcity].name;
									new_vaule += html_color_end;
									new_vaule += html_color_begin1;
									old_vaule = "[city]";
									repalce_all_ditinct(content, old_vaule, new_vaule);
								}
							}

							if (currentIdNum != m_pCurTaskDefine->taskid)
							{
								std::string time;
								time += StringUtils::format("%d", CoreTaskInfo.time);
								time += SINGLE_SHOP->getTipsInfo()["TIP_MINUTE"];
								t_time_num->setString(time);
								t_time_label->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TIME_LIMIT"]);
								//棕色
								t_content->setTextColor(Color4B(40, 25, 13, 255));
								t_time_label->setTextColor(Color4B(40, 25, 13, 255));
								t_time_num->setTextColor(Color4B(40, 25, 13, 255));
								//html_color_begin = "<font size='24' color='#2E1D0E'>";
							}
							else
							{
								long now = time(NULL);
								t_time_label->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_REMAINING_TIME"]);
								if (m_pCurTaskDefine->surplustime <= 0)
								{
									m_nTaskFalg = -1;
									//红色
									t_time_num->setTextColor(Color4B(205, 25, 25, 255));
									t_content->setTextColor(Color4B(205, 25, 25, 255));
									t_time_label->setTextColor(Color4B(205, 25, 25, 255));
									t_time_num->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_TIEMOUT"]);
									//html_color_begin = "<font size='24' color='#CD1919'>";
								}
								else
								{
									char buf[100];
									int64_t myTime = m_pCurTaskDefine->surplustime;
									snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld", myTime / 3600, (myTime % 3600) / 60, myTime % 60);
									t_time_num->setString(buf);
									//棕色
									t_time_num->setTextColor(Color4B(40, 25, 13, 255));
									t_content->setTextColor(Color4B(40, 25, 13, 255));
									t_time_label->setTextColor(Color4B(40, 25, 13, 255));
									//html_color_begin = "<font size='24' color='#2E1D0E'>";
								}
							}

							t_silver->setString(numSegment(StringUtils::format("%d", CoreTaskInfo.rewards_coin)));
							t_gold->setString(numSegment(StringUtils::format("%d", m_pCurTaskDefine->hasbonus ? CoreTaskInfo.rewards_gold + CoreTaskInfo.bonus_v_ticket : CoreTaskInfo.rewards_gold)));
							t_exp->setString(StringUtils::format("%d", CoreTaskInfo.rewards_exp));
							t_rep->setString(StringUtils::format("%d", CoreTaskInfo.rewards_reputation));

							std::string str = SINGLE_SHOP->getSideTaskInfo()[currentIdNum].desc;
							if (CoreTaskInfo.finish_target_npcId)
							{
								std::string old_value1 = "[npc]";
								std::string new_value1 = SINGLE_SHOP->getBattleNpcInfo()[CoreTaskInfo.finish_target_npcId[m_pCurTaskDefine->npcindex]].name;
								repalce_all_ditinct(str, old_value1, new_value1);
							}

							if (CoreTaskInfo.type == 2)
							{
								std::string old_value2 = "[city]";
								std::string new_value2 = SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->targetcity].name;
								repalce_all_ditinct(str, old_value2, new_value2);
							}

							if (CoreTaskInfo.zone)
							{
								std::string old_value3 = "[zone]";
								std::string new_value3 = SINGLE_SHOP->getZoneInfo()[CoreTaskInfo.zone[m_pCurTaskDefine->npcindex]];
								repalce_all_ditinct(str, old_value3, new_value3);
							}

							if (CoreTaskInfo.type == 2)
							{
								std::string old_value4 = "[goods]";
								std::string new_value4 = getGoodsName(CoreTaskInfo.finish_goods_id[m_pCurTaskDefine->npcindex]);
								repalce_all_ditinct(str, old_value4, new_value4);
							}
							t_story->setString(str);
							t_story->setContentSize(Size(t_story->getContentSize().width, 36 + getLabelHight(str, t_story->getContentSize().width, t_story->getFontName())));


							int easness_num = 100;
							if (CoreTaskInfo.type == 2)
							{
								easness_num = 100;
							}
							else
							{
								if (m_pNpcScoreResult && m_pNpcScoreResult->n_lowscores > m_pCurTaskDefine->npcindex)
								{
									int hero_npc = (m_pNpcScoreResult->highescores[0] + m_pNpcScoreResult->lowscores[0]) / 2;
									int enemy_npc = (m_pNpcScoreResult->highescores[m_pCurTaskDefine->npcindex - 1] + m_pNpcScoreResult->lowscores[m_pCurTaskDefine->npcindex - 1]) / 2;
									easness_num = calc_victory_percent(hero_npc, enemy_npc);
								}
							}
							auto i_diff1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_difficulty, "image_diff_1"));
							auto i_diff2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_difficulty, "image_diff_2"));
							auto i_diff3 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_difficulty, "image_diff_3"));

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

							std::string name;
							std::string path;
							if (CoreTaskInfo.rewards_item1_id)
							{
								p_equip->setVisible(true);
								i_itembg1->setVisible(true);
								b_item1->setVisible(true);
								b_item1->setTag(CoreTaskInfo.rewards_item1_id);
								getItemNameAndPath(CoreTaskInfo.rewards_item1_type, CoreTaskInfo.rewards_item1_id, name, path);
								i_item1->loadTexture(path);
								i_item1->setTag(CoreTaskInfo.rewards_item1_type);
								setTextSizeAndOutline(t_num1, CoreTaskInfo.rewards_item1_amount);
								setBgButtonFormIdAndType(i_itembg1, CoreTaskInfo.rewards_item1_id, CoreTaskInfo.rewards_item1_type);
							}
							else
							{
								b_item1->setVisible(false);
								i_itembg1->setVisible(false);
								p_equip->setVisible(false);
							}
							if (CoreTaskInfo.rewards_item2_id)
							{
								b_item2->setVisible(true);
								i_itembg2->setVisible(true);
								b_item2->setTag(CoreTaskInfo.rewards_item2_id);
								getItemNameAndPath(CoreTaskInfo.rewards_item2_type, CoreTaskInfo.rewards_item2_id, name, path);
								i_item2->loadTexture(path);
								i_item2->setTag(CoreTaskInfo.rewards_item2_type);
								setTextSizeAndOutline(t_num2, CoreTaskInfo.rewards_item2_amount);
								setBgButtonFormIdAndType(i_itembg2, CoreTaskInfo.rewards_item1_id, CoreTaskInfo.rewards_item1_type);
							}
							else
							{
								b_item2->setVisible(false);
								i_itembg2->setVisible(false);
							}
							//按钮显示
							if (m_nTaskFalg == 2 && currentIdNum == m_pCurTaskDefine->taskid)
							{
								//任务完成
								if (m_userTask->cityid)
								{
									b_reward->setBright(true);
									b_reward->setTouchEnabled(true);
								}
								else
								{
									b_reward->setBright(false);
									b_reward->setTouchEnabled(false);
								}
								b_reward->setVisible(true);
								i_notify_reward->setVisible(true);
								b_abort->setTouchEnabled(false);
								b_abort->setVisible(false);
								//绿色
								t_time_num->setTextColor(Color4B(46, 125, 50, 255));
								t_content->setTextColor(Color4B(46, 125, 50, 255));
								t_time_label->setTextColor(Color4B(46, 125, 50, 255));
								//html_color_begin = "<font size='24’ color='#2E7D32'>";
							}
							if (m_nTaskFalg == 1 && currentIdNum == m_pCurTaskDefine->taskid)
							{
								//任务正在进行
								b_reward->setTouchEnabled(false);
								b_reward->setVisible(false);
								b_abort->setTouchEnabled(true);
								b_abort->setVisible(true);
							}
							if (m_nTaskFalg == -1)
							{
								//任务超时
								b_reward->setTouchEnabled(false);
								b_reward->setVisible(false);
								b_abort->setTouchEnabled(true);
								b_abort->setVisible(true);
								i_notify_abort->setVisible(true);
							}

							std::string temp_content;
#if WIN32
							temp_content = content;
#else
							temp_content = "<html>";
							temp_content += html_color_begin1;
							temp_content += content;
							temp_content += html_color_end;
							temp_content += "</html>";
#endif
							//log("temp_content = %s", temp_content.c_str());
							t_content->setString(temp_content);
						}
						else
						{
							//t_name->setPosition(Vec2(i_task_type->getPositionX(), i_task_type->getPositionY()));
							i_task_type->setVisible(false);
							t_name->setPositionX(0);
							l_taskInfo->removeChild(p_content);
							l_taskInfo->removeChild(p_difficulty);
							l_taskInfo->removeChild(p_time);
							l_taskInfo->removeChild(p_reward);
							l_taskInfo->removeChild(i_div_2);

							//显示当前无支线任务时的说明
							t_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_SIDE_NO_TASK_TITLE"]);
							std::string str = SINGLE_SHOP->getTipsInfo()["TIP_SIDE_NO_TASK_DESC"];
							t_story->setString(str);
							t_story->setContentSize(Size(t_story->getContentSize().width, getLabelHight(str, t_story->getContentSize().width, t_story->getFontName())));
						}
						break;
	}
	case PARTNER_TASK:
	{
						 i_task_type->loadTexture(getTaskType(3));
						 //显示当前正在做的小伙伴任务	
						 i_task_type->setVisible(true);
						 l_taskInfo->removeChild(p_difficulty);
						 t_name->setString(SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].title);
						 auto CoreTaskInfo = SINGLE_SHOP->getCoreTaskById(currentIdNum);
						 t_content->setString(SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].tip);
						 float height = getLabelHight(SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].tip, t_content->getContentSize().width, t_content->getFontName(), t_content->getFontSize());
						 if (height > t_content->getContentSize().height)
						 {
							 t_content->setAnchorPoint(Vec2(0, 1));
							 t_content->setPosition(t_content->getPosition() + Vec2(0, t_content->getContentSize().height / 2));
							 t_content->setContentSize(Size(t_content->getContentSize().width, height + t_content->getFontSize()));
						 }
						 t_time_label->setString(StringUtils::format("%d", currentIdNum));
						 t_time_num->setString(StringUtils::format("%d", currentIdNum));

						 t_silver->setString(numSegment(StringUtils::format("%d", SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].rewards.coin)));
						 t_gold->setString(numSegment(StringUtils::format("%d", SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].rewards.vticket)));
						 t_exp->setString(StringUtils::format("%d", SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].rewards.exp));
						 t_rep->setString(StringUtils::format("%d", SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].rewards.reputation));

						 std::string str = SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].desc;
						 std::string new_value_aide;
						 std::string old_value_aide = "[aidename]";
						 if (SINGLE_HERO->m_iGender == 1)
						 {
							 new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"];
						 }
						 else
						 {
							 new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"];
						 }
						 repalce_all_ditinct(str, old_value_aide, new_value_aide);
						 t_story->setString(str);
						 t_story->setContentSize(Size(t_story->getContentSize().width, 36 + getLabelHight(str, t_story->getContentSize().width, t_story->getFontName())));

						 if (SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].rewards.force_id > 0)
						 {
							 p_force->setVisible(true);
							 i_falg->ignoreContentAdaptWithSize(false);
							 i_falg->loadTexture(getCountryIconPath(SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].rewards.force_id));
							 t_force_num->setString(StringUtils::format("+%d", SINGLE_SHOP->getPartnerDialogJudgeInfo()[currentIdNum].rewards.force_affect));
						 }
						 //小伙伴剧情任务暂时不显示items
						 /*if (m_nCurrentTaskStatus)
						 {
							 b_reward->setVisible(true);
							 b_reward->setTouchEnabled(true);
							 i_notify_reward->setVisible(true);
						 }*/
						 break;
	}
	case NOMAL_TASK:
	{
						if (m_userTask->levelenough == 0 && currentIdNum >= 3)
						{
							currentIdNum += 2;
						}
						b_reward->setTag(currentIdNum);
						switch (currentIdNum)
						{
						case 1:
						case 2:
						{
								  l_taskInfo->setVisible(true);
								  i_task_type->setVisible(false);
								  t_name->setPositionX(0);
								  auto st_title = StringUtils::format("TIP_EVNETS_TASK_BUTTLE_%d", currentIdNum);
								  t_name->setString(SINGLE_SHOP->getTipsInfo()[st_title].c_str());
								  b_item1->setVisible(false);
								  i_itembg1->setVisible(false);
								  p_equip->setVisible(false);
								  l_taskInfo->removeChild(p_difficulty);
								  
								  auto signNum = 0;
								  auto countNum = 0;
								  if (currentIdNum == 1)
								  {
									  signNum = m_userTask->sign_for_loot_reward;
									  countNum = m_userTask->loot_count;
									  nomalTaskIndex = NOMAL_ROB_TASK;
								  }
								  else if (currentIdNum == 2)
								  {
									  signNum = m_userTask->sign_for_tavern_task_reward;
									  countNum = m_userTask->tavern_task_count;
									  nomalTaskIndex = NOMAL_SIDE_TASK;
								  }
								  if (signNum == 1)
								  {
									  l_taskInfo->removeChild(p_content);
									  l_taskInfo->removeChild(p_difficulty);
									  l_taskInfo->removeChild(p_time);
									  l_taskInfo->removeChild(p_reward);
									  l_taskInfo->removeChild(i_div_2);

									  //显示今日任务已完成的说明
									  std::string str;
									  if (currentIdNum == 1)
									  {
										  str = SINGLE_SHOP->getTipsInfo()["TIP_EVENT_ROB_TASK_FINISH_DESC"];
									  }
									  else if (currentIdNum == 2)
									  {
										  str = SINGLE_SHOP->getTipsInfo()["TIP_EVENT_TEN_SIDE_TASKS_FINISH_DESC"];
									  }
									  t_story->setString(str);
									  t_story->setContentSize(Size(t_story->getContentSize().width, getLabelHight(str, t_story->getContentSize().width, t_story->getFontName())));
								  }
								  else
								  {
									  p_time->setVisible(true);
									  char buf[100];
									  int64_t myTime = m_userTask->dis_to_oneday_finish;
									  snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld", myTime / 3600, (myTime % 3600) / 60, myTime % 60);
									  t_time_num->setString(buf);
									  t_time_label->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_REMAINING_TIME"]);
									  //棕色
									  t_time_label->setTextColor(Color4B(40, 25, 13, 255));
									  t_time_num->setTextColor(Color4B(40, 25, 13, 255));

									  std::string content;
									  std::string new_vaule;
									  std::string old_vaule;
									  std::string html_color_begin1;
									  std::string html_color_begin;
									  std::string html_color_end;

#if WIN32
									  html_color_begin1 = "";
									  html_color_begin = "";
									  html_color_end = "";
#else
									  //默认深棕色
									  html_color_begin1 = "<font size='24' color='#2E1D0E'>";
									  html_color_begin = "<font size='24' color='#A95601'>";
									  html_color_end = "</font>";
									  //浅棕色
									  html_color_begin = "<font size='24' color='#A95601'>";
									  if (countNum >= 10)
									  {
										  //任务完成
										  //绿色
										  html_color_begin = "<font size='24’ color='#2E7D32'>";
									  }
#endif
									  std::string goalStr;
									  std::string descStr;
									  if (currentIdNum == 1)
									  {
										  goalStr = SINGLE_SHOP->getTipsInfo()["TIP_EVENT_ROB_TASK_UNFINISH_GOAL"];
										  descStr = SINGLE_SHOP->getTipsInfo()["TIP_EVENT_ROB_TASK_UNFINISH_DESC"];
									  }
									  else if (currentIdNum == 2)
									  {
										  goalStr = SINGLE_SHOP->getTipsInfo()["TIP_EVENT_TEN_SIDE_TASKS_UNFINISH_GOAL"];
										  descStr = SINGLE_SHOP->getTipsInfo()["TIP_EVENT_TEN_SIDE_TASKS_UNFINISH_DESC"];
									  }
									  content += goalStr;
									  new_vaule = html_color_end;
									  new_vaule += html_color_begin;
									  if (countNum > 0)
									  {
										  new_vaule += StringUtils::format("%d/%d", countNum, 10);
									  }
									  else
									  {
										  new_vaule += StringUtils::format("%d", 10);
									  }
									  new_vaule += html_color_end;
									  new_vaule += html_color_begin1;
									  old_vaule = "[num]";
									  repalce_all_ditinct(content, old_vaule, new_vaule);

									  t_silver->setString(numSegment(StringUtils::format("%d", 0)));
									  t_gold->setString(numSegment(StringUtils::format("%d", 10)));
									  t_exp->setString(StringUtils::format("%d", 0));
									  t_rep->setString(StringUtils::format("%d", 0));

									  
									  t_story->setString(descStr);
									  t_story->setContentSize(Size(t_story->getContentSize().width, 36 + getLabelHight(descStr, t_story->getContentSize().width, t_story->getFontName())));

									  //按钮显示
									  if (countNum >= 10)
									  {
										  //任务完成
										  b_reward->setBright(true);
										  b_reward->setTouchEnabled(true);
										  b_reward->setVisible(true);
										  i_notify_reward->setVisible(true);
										  //绿色
										  t_time_num->setTextColor(Color4B(46, 125, 50, 255));
										  t_content->setTextColor(Color4B(46, 125, 50, 255));
										  t_time_label->setTextColor(Color4B(46, 125, 50, 255));
										  //html_color_begin = "<font size='24’ color='#2E7D32'>";
									  }
									  else
									  {
										  //任务正在进行
										  b_reward->setTouchEnabled(false);
										  b_reward->setVisible(false);
									  }

									  std::string temp_content;
#if WIN32
									  temp_content = content;
#else
									  temp_content = "<html>";
									  temp_content += html_color_begin1;
									  temp_content += content;
									  temp_content += html_color_end;
									  temp_content += "</html>";
#endif
									  //log("temp_content = %s", temp_content.c_str());
									  t_content->setString(temp_content);
								  }
								  break;
						}
						case 3:
						{
									p_piratessiege->setVisible(true);
									p_siege->setVisible(false);
									p_faith->setVisible(false);
									p_daily->setVisible(false);
									auto t_feats_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_feats_num"));
									auto text_gp_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_gp_num_1"));
									auto text_gp_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_gp_num_2"));
									auto text_gp_num_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_gp_num_3"));
									auto text_gp_num_1_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_gp_num_1_0"));
									auto text_gp_num_2_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_gp_num_2_0"));
									auto text_gp_num_3_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_gp_num_3_0"));
									auto l_feats_bar = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(p_piratessiege, "loadingbar_feats"));

									t_feats_num->setString(StringUtils::format("%d", m_userTask->myexploitscore));
									text_gp_num_1->setString("1");
									text_gp_num_2->setString("2");
									text_gp_num_3->setString("3");
									text_gp_num_1_0->setString(StringUtils::format("%d", m_userTask->exploitsection[0]));
									text_gp_num_2_0->setString(StringUtils::format("%d", m_userTask->exploitsection[1]));
									text_gp_num_3_0->setString(StringUtils::format("%d", m_userTask->exploitsection[2]));
									float num = 100.0 * m_userTask->myexploitscore / m_userTask->exploitsection[2];
									l_feats_bar->setPercent(num);
									auto l_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_piratessiege, "listview_content"));
									auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_1"));
									t_content->ignoreContentAdaptWithSize(false);
									t_content->setContentSize(Size(t_content->getContentSize().width, getLabelHight(t_content->getString(), t_content->getContentSize().width, t_content->getFontName(),t_content->getFontSize())));
									auto image_pulldown = p_piratessiege->getChildByName<ImageView*>("image_pulldown_0");
									auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
									button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
									addListViewBar(l_content, image_pulldown);

									auto image_div = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_piratessiege, "image_div_2"));
									auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_time"));
									auto t_time_content = dynamic_cast<Text*>(Helper::seekWidgetByName(p_piratessiege, "text_time_1"));
									t_time->setPositionX(image_div->getPositionX() - (t_time_content->getBoundingBox().size.width + t_time->getBoundingBox().size.width) / 2);
									t_time_content->setPositionX(t_time->getPositionX() + t_time->getBoundingBox().size.width);

									auto b_attend = dynamic_cast<Button*>(Helper::seekWidgetByName(p_piratessiege, "button_attend"));
									b_attend->setVisible(true);
									if (m_userTask->pirateattackstatus && !SINGLE_HERO->m_heroIsOnsea)
									{
										b_attend->setBright(true);
										b_attend->setTouchEnabled(true);
									}
									else
									{
										b_attend->setBright(false);
										b_attend->setTouchEnabled(false);
									}
									break;
						}
						case 4:
						{
									p_piratessiege->setVisible(false);
									p_siege->setVisible(true);
									p_faith->setVisible(false);
									p_daily->setVisible(false);
									auto t_times = dynamic_cast<Text*>(Helper::seekWidgetByName(p_siege, "text_num_2"));
									t_times->setString(StringUtils::format("%d", m_userTask->attacktimes));
									if (m_userTask->bossinfo)
									{
										auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_siege, "image_head"));
										auto t_npcName = dynamic_cast<Text*>(Helper::seekWidgetByName(p_siege, "text_bossname"));
										auto t_npcPosition = dynamic_cast<Text*>(Helper::seekWidgetByName(p_siege, "text_coordinate"));
										t_npcName->setString(SINGLE_SHOP->getBattleNpcInfo()[m_userTask->bossinfo->bossid].name);
										t_npcPosition->setString(StringUtils::format("%d,%d", m_userTask->bossinfo->positionx, int(MAP_CELL_HEIGH * MAP_WIDTH - m_userTask->bossinfo->positiony)));
										image_head->loadTexture("res/city_icon/boss.png");

										auto image_write = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_siege, "image_write"));
										if (SINGLE_HERO->m_iLevel < 10)
										{
											setGLProgramState(image_write, true);
										}

										if (m_userTask->boss_alive == 0)
										{
											auto i_feats_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_siege, "image_feats_bg"));
											auto i_time_bg = i_feats_bg->getChildByName<ImageView*>("image_time_bg");
											auto t_num = i_feats_bg->getChildByName<Text*>("text_num_1");
											auto t_num2 = i_feats_bg->getChildByName<Text*>("text_num_2");
											auto i_gp_dot = i_feats_bg->getChildByName<ImageView*>("image_gp_dot_3_0");
											auto t_time = i_feats_bg->getChildByName<Text*>("text_time");
											auto i_time_bg2 = i_feats_bg->getChildByName<ImageView*>("image_time_bg_2");
											auto t_boss_die = i_feats_bg->getChildByName<Text*>("text_dea");
											i_time_bg->setVisible(false);
											t_num->setVisible(false);
											t_num2->setVisible(false);
											i_gp_dot->setVisible(false);
											t_time->setVisible(false);
											i_time_bg2->setVisible(true);
											t_boss_die->setVisible(true);
											i_time_bg->setVisible(false);
										}
									}
									else
									{
										auto panel_target = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_siege, "panel_target"));
										auto image_div_4 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_siege, "image_div_4"));
										if (panel_target) panel_target->removeFromParentAndCleanup(true);
										if (image_div_4) image_div_4->removeFromParentAndCleanup(true);
									}

									auto l_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_siege, "listView_siege"));
									auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(p_siege, "text_1"));
									t_content->ignoreContentAdaptWithSize(false);
									t_content->setContentSize(Size(t_content->getContentSize().width, getLabelHight(t_content->getString(), t_content->getContentSize().width, t_content->getFontName(), t_content->getFontSize())));
									auto image_pulldown = p_siege->getChildByName<ImageView*>("image_pulldown_0");
									auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
									button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
									addListViewBar(l_content, image_pulldown);
									break;
						}
						case 5:
						{
									p_piratessiege->setVisible(false);
									p_siege->setVisible(false);
									p_faith->setVisible(true);
									p_daily->setVisible(false);
									auto panel_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_faith, "panel_1"));
									auto panel_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_faith, "panel_2"));
									if (m_userTask->prayremaintime == 0)
									{
										panel_1->setVisible(true);
										panel_2->setVisible(false);
										auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_1, "text_1"));
										t_content->ignoreContentAdaptWithSize(false);
										t_content->setContentSize(Size(t_content->getContentSize().width, getLabelHight(t_content->getString(), t_content->getContentSize().width, t_content->getFontName(), t_content->getFontSize())));
									}
									else
									{
										panel_1->setVisible(false);
										panel_2->setVisible(true);
										auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_2, "text_name"));
										auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_2, "text_2"));
										auto i_belief = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_2, "image_belief"));

										std::string name = StringUtils::format("TIP_FAITH_NAME_%d", m_userTask->lastpreyindex);
										std::string content = StringUtils::format("TIP_FAITH_CONTENT_%d", m_userTask->lastpreyindex);
										t_name->setString(SINGLE_SHOP->getTipsInfo()[name]);
										t_content->setString(SINGLE_SHOP->getTipsInfo()[content]);
										i_belief->ignoreContentAdaptWithSize(false);
										i_belief->loadTexture(StringUtils::format("cocosstudio/login_ui/activity_720/belief_%d.png", m_userTask->lastpreyindex));
									}
									break;
						}
						case 6:
						{
								  p_piratessiege->setVisible(false);
								  p_siege->setVisible(false);
								  p_faith->setVisible(false);
								  p_daily->setVisible(true);
								  int totalSignDays = 0;
								  if (SINGLE_HERO->m_iDailyrewarddata)
								  {
									  m_nTotalDays = SINGLE_HERO->m_iDailyrewarddata[0]->rewardindex;//0-29
								  }
								  if (SINGLE_HERO->m_iNdailyrewarddata <= 0)
								  {
									  totalSignDays = m_nTotalDays + 1;
								  }
								  else
								  {
									  totalSignDays = m_nTotalDays + SINGLE_HERO->m_iNdailyrewarddata;
									  if (totalSignDays>30)
									  {
										  totalSignDays = totalSignDays - 30;
									  }
								  }
								  auto label_task_content_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_daily, "label_task_content_0"));
								  label_task_content_0->setString(SINGLE_SHOP->getTipsInfo()["TIP_TASK_DAILYREWARD_KEEP_PLAYING"]);
								  auto label_task_content_0_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_daily, "label_task_content_0_0"));
								  label_task_content_0_0->setString(SINGLE_SHOP->getTipsInfo()["TIP_TASK_DAILYREWARD_NUMBER"]);
								  auto label_next = dynamic_cast<Text*>(Helper::seekWidgetByName(p_daily, "label_next"));
								  label_next->setString(SINGLE_SHOP->getTipsInfo()["TIP_TASK_DAILYREWARD_LOGIN"]);
								  auto label_next_num = p_daily->getChildByName<Text*>("label_next_num");
								  label_next_num->setContentSize(Size(label_next_num->getContentSize().width, 34));
								  auto image_goods = p_daily->getChildByName<ImageView*>("image_goods");
								  auto listView_dayPanel = p_daily->getChildByName<ListView*>("listView_dayPanel");
								  listView_dayPanel->setDirection(ListView::Direction::VERTICAL);
								  listView_dayPanel->removeAllChildrenWithCleanup(true);
								  //item name
								  std::string youGet = " ";

								  if (totalSignDays == 0)
								  {
									  image_goods->setVisible(false);
									  label_next_num->setVisible(false);
								  }
								  else
								  {
									  image_goods->setVisible(true);
									  label_next_num->setVisible(true);
									  if (totalSignDays == 30)
									  {
										  if (SINGLE_HERO->m_iDailyrewarddata[0]->sid)
										  {
											  youGet = getShipName(SINGLE_HERO->m_iDailyrewarddata[0]->sid);
											  image_goods->loadTexture(getShipIconPath(SINGLE_HERO->m_iDailyrewarddata[0]->sid, IMAGE_ICON_SHADE));
										  }
										  else
										  {
											  youGet = getShipName(27);
											  image_goods->loadTexture(getShipIconPath(27, IMAGE_ICON_SHADE));
										  }

										  youGet += StringUtils::format(" x%d", SINGLE_SHOP->getDailyRewardInfo()[30].ships_num);
										  label_next_num->setString(youGet);
									  }
									  else
									  {
										  if (SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].items_id)
										  {
											  image_goods->loadTexture(getItemIconPath(SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].items_id, IMAGE_ICON_SHADE));
											  youGet = SINGLE_SHOP->getItemData()[SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].items_id].name;
											  youGet = youGet + " x" + StringUtils::format("%d", SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].items_num);
											  label_next_num->setString(youGet);
										  }
										  else if (SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].coins)
										  {
											  image_goods->loadTexture(getItemIconPath(10002, IMAGE_ICON_SHADE));
											  youGet = SINGLE_SHOP->getTipsInfo()["TIP_BANK_SILVER"];
											  youGet = youGet + " x" + numSegment(StringUtils::format("%d", SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].coins));
											  label_next_num->setString(youGet);
										  }
										  else
										  {
											  image_goods->loadTexture(getItemIconPath(10000, IMAGE_ICON_SHADE));
											  youGet = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_GOLD"];
											  youGet = youGet + " x" + numSegment(StringUtils::format("%d", SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].v_tickets));
											  label_next_num->setString(youGet);
										  }
									  }
								  }
								  //right panel
								  auto panel_items_1 = p_daily->getChildByName("panel_items_1");
								  auto image_items_1 = panel_items_1->getChildByName<Widget*>("image_items_1");//clone
								  //30 day
								  auto ships = SINGLE_SHOP->getDailyRewardInfo()[ALLDAYS].ships_id;
								  break;
						}
						case 7:
						{
								  if (m_nSublineId > 0)
								  {
									  SDGuideTasks *task = SDGuideTasks::GetElement(m_nSublineId);
									  SDGuideTasksReward * taskReward = SDGuideTasksReward::GetElement(m_nSublineId);
									  l_taskInfo->setVisible(true);
									  i_task_type->setVisible(false);
									  t_name->setPositionX(0);
									  l_taskInfo->removeChild(p_difficulty);
									  p_time->setVisible(false);

									  auto image_sailor = p_common->getChildByName<ImageView*>("image_sailor");
									  auto label_sailor_num = p_common->getChildByName<Text*>("label_sailor_num");
									  image_sailor->setVisible(true);
									  label_sailor_num->setVisible(true);
									  label_sailor_num->setString(numSegment(StringUtils::format("%d", taskReward->getv_badges())));
									  t_silver->setString(numSegment(StringUtils::format("%d", taskReward->getcoins())));
									  t_gold->setString(numSegment(StringUtils::format("%d", taskReward->getv_tickets())));
									  t_exp->setString(StringUtils::format("%d", 0));
									  t_rep->setString(StringUtils::format("%d", 0));
									  t_name->setString(GetLocalStr(task->gettitle_ids()));
									  t_content->setString(GetLocalStr(task->gettip_ids()));
									  t_story->setString(SINGLE_SHOP->getTipsInfo()["TIP_TASK_SUBLINE_DSC"]);

									  string name = "";
									  string path = "";
									  if (taskReward->getitem1id() && taskReward->getitem1amount() > 0)
									  {
										  p_equip->setVisible(true);
										  i_itembg1->setVisible(true);
										  b_item1->setVisible(true);
										  b_item1->setTag(taskReward->getitem1id());
										  getItemNameAndPath(taskReward->getitem1type(), taskReward->getitem1id(), name, path);
										  i_item1->loadTexture(path);
										  setTextSizeAndOutline(t_num1, taskReward->getitem1amount());
										  i_item1->setTag(taskReward->getitem1type());
										  setBgButtonFormIdAndType(i_itembg1, taskReward->getitem1id(), taskReward->getitem1type());
									  }
									  else
									  {
										  b_item1->setVisible(false);
										  i_itembg1->setVisible(false);
										  p_equip->setVisible(false);
									  }
									  if (taskReward->getitem2id() && taskReward->getitem2amount() > 0)
									  {
										  b_item2->setVisible(true);
										  i_itembg2->setVisible(true);
										  b_item2->setTag(taskReward->getitem2id());
										  getItemNameAndPath(taskReward->getitem2type(), taskReward->getitem2id(), name, path);
										  i_item2->loadTexture(path);
										  setTextSizeAndOutline(t_num2, taskReward->getitem2amount());
										  i_item2->setTag(taskReward->getitem2type());
										  setBgButtonFormIdAndType(i_itembg2, taskReward->getitem2id(), taskReward->getitem2type());
									  }
									  else
									  {
										  b_item2->setVisible(false);
										  i_itembg2->setVisible(false);
									  }

									  if (m_bSublineFinish)
									  {
										  //任务完成
										  b_reward->setBright(true);
										  b_reward->setTouchEnabled(true);
										  b_reward->setVisible(true);
										  i_notify_reward->setVisible(true);
									  }
									  else
									  {
										  b_reward->setTouchEnabled(false);
										  b_reward->setVisible(false);
										  i_notify_reward->setVisible(false);
									  }
								  } 
								  else
								  {
									  //副线结束
									  l_taskInfo->setVisible(true);
									  i_task_type->setVisible(false);
									  t_name->setPositionX(0);
									  l_taskInfo->removeChild(p_content);
									  l_taskInfo->removeChild(p_difficulty);
									  l_taskInfo->removeChild(p_time);
									  l_taskInfo->removeChild(p_reward);
									  l_taskInfo->removeChild(i_div_2);

									  auto str = StringUtils::format("TIP_MAIN_NO_TASK_TITLE_%d", SINGLE_HERO->m_iNation);
									  t_name->setString(SINGLE_SHOP->getTipsInfo()[str]);
									  str = StringUtils::format("TIP_MAIN_NO_TASK_DESC_%d", SINGLE_HERO->m_iNation);
									  t_story->setString(SINGLE_SHOP->getTipsInfo()[str]);
									  t_story->setContentSize(Size(t_story->getContentSize().width, getLabelHight(str, t_story->getContentSize().width, t_story->getFontName())));
								  }
								  break;
						}
						default:
							break;
						}
						break;
	}
	default:
		break;
	}
	t_time_num->setPositionX(t_time_label->getPositionX() + t_time_label->getBoundingBox().size.width);
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_taskInfo, image_pulldown);
	this->setCameraMask(_cameraMask, true);
}

void UITask::everySecondReflush(float fTime)
{
	char buf[100];
	long now = time(NULL);
	auto view = getViewRoot("cocosstudio/voyageUI_quest.csb");
	if (view)
	{
		auto l_taskInfo = view->getChildByName<ListView*>("listview_task_info");
		auto p_time = dynamic_cast<Widget*>(Helper::seekWidgetByName(l_taskInfo, "p_time"));
		if (p_time )
		{
			auto t_time = p_time->getChildByName<Text*>("label_task_time_num");

			switch (m_task)
			{
			case SIDE_TASK:
				if (m_pCurTaskDefine)
				{
					if (m_pCurTaskDefine->surplustime <= 0)
					{
						t_time->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_TIEMOUT"]);
					}
					else
					{
						int64_t myTime = m_pCurTaskDefine->surplustime;
						m_pCurTaskDefine->surplustime--;
						snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld", myTime / 3600, (myTime % 3600) / 60, myTime % 60);
						t_time->setString(buf);
					}
				}
				break;
			case NOMAL_TASK:
				if (m_userTask->dis_to_oneday_finish)
				{
					int64_t myTime = m_userTask->dis_to_oneday_finish;
					m_userTask->dis_to_oneday_finish--;
					snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld", myTime / 3600, (myTime % 3600) / 60, myTime % 60);
					t_time->setString(buf);
				}
				break;
			default:
				break;
			}
		}

		auto p_siege = view->getChildByName<Widget*>("panel_event_siege");
		auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(p_siege, "text_time"));
		auto i_time_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_siege, "image_time_bg"));
		m_userTask->distozero--;
		if (m_userTask->distozero == 0)
		{
			if (m_userTask->attackstatus == 1)
			{
				m_userTask->attackstatus = 0;
				m_userTask->distozero = 5 * 24 * 3600;
			}
			else
			{
				m_userTask->attackstatus = 1;
				m_userTask->distozero = 2 * 24 * 3600;
			}
		}

		std::string str_content;
		std::string old_vaule;
		if (m_userTask->attackstatus == 0)
		{
			i_time_bg->setVisible(false);
			str_content = SINGLE_SHOP->getTipsInfo()["TIP_EVENTS_SIEGE_START"];
		}
		else
		{
			if (m_userTask->boss_alive == 1)
			{
				i_time_bg->setVisible(true);
			}
			str_content = SINGLE_SHOP->getTipsInfo()["TIP_EVENTS_SIEGE_END"];
		}

		int d = m_userTask->distozero / (24 * 3600);
		int h = (m_userTask->distozero % (24 * 3600)) / 3600;
		int m = (m_userTask->distozero % 3600) / 60;
		int s = m_userTask->distozero % 60;

		char new_vaule[30];
		snprintf(new_vaule, sizeof(new_vaule), "%02d:%02d:%02d", h, m, s);
		old_vaule = "[hour]";
		repalce_all_ditinct(str_content, old_vaule, new_vaule);
		snprintf(new_vaule, sizeof(new_vaule), "%d", d);
		old_vaule = "[day]";
		repalce_all_ditinct(str_content, old_vaule, new_vaule);
		t_time->setString(str_content);

		auto p_faith = view->getChildByName<Widget*>("panel_event_faith");
		auto panel_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_faith, "panel_1"));
		auto panel_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_faith, "panel_2"));
		
		if (m_userTask->prayremaintime > 0)
		{
			m_userTask->prayremaintime--;
			panel_1->setVisible(false);
			panel_2->setVisible(true);
			auto l_time = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_2, "loadingbar_1"));
			auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_2, "text_time"));

			l_time->setPercent(100.0 * m_userTask->prayremaintime / m_userTask->praycollingtime);
			int h = (m_userTask->prayremaintime % (24 * 3600)) / 3600;
			int m = (m_userTask->prayremaintime % 3600) / 60;
			int s = m_userTask->prayremaintime % 60;
			t_time->setString(StringUtils::format("%02d:%02d:%02d", h, m, s));
			m_bIsPrayer = false;
		}
		else 
		{
			if (!m_bIsPrayer)
			{
				panel_1->setVisible(true);
				panel_2->setVisible(false);
				m_bIsPrayer = true;
				flushLeftTaskButton();
			} 
		}
	}
}
void UITask::openGetRewardDialog(const HandleTaskResult* result)
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
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_item_clone, "image_item"));
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
		//棕色
		t_num->setTextColor(Color4B(40, 25, 13, 255));
		l_result->pushBackCustomItem(p_force_clone);
		auto i_div2 = i_div->clone();
		l_result->pushBackCustomItem(i_div2);
	}

	if (result->rewardcoin)
	{
		auto p_silver_clone = p_coin->clone();
		auto i_silver = p_silver_clone->getChildByName<ImageView*>("image_silver");
		auto t_silver_num = p_silver_clone->getChildByName<Text*>("label_buy_num");
		std::string str = numSegment((StringUtils::format("%lld", result->rewardcoin)));
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
		std::string str = numSegment((StringUtils::format("%lld", result->rewardgold)));
		t_gold_num->setString(StringUtils::format("+%s", str.c_str()));
		l_result->pushBackCustomItem(p_gold_clone);
		auto i_div4 = i_div->clone();
		l_result->pushBackCustomItem(i_div4);
	}
	
	this->setCameraMask(_cameraMask, true);

	SINGLE_HERO->m_iCoin = result->coin;
	SINGLE_HERO->m_iGold = result->gold;
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	if (mainlayer)
	{
		mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
		mainlayer->flushExpAndRepLv();
	}
}
void UITask::openTaskConfirmDialog(const int nIndex, const bool bIsAccept)
{
	openView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB], 11);
	auto w_taskConfirm = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
	auto b_yes = w_taskConfirm->getChildByName<Button*>("button_s_yes");
	auto b_no = w_taskConfirm->getChildByName<Button*>("button_s_no");
	auto t_title = w_taskConfirm->getChildByName<Text*>("label_repair");
	auto t_content = w_taskConfirm->getChildByName<Text*>("label_content");
	if (bIsAccept)
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_ACCEPT_TITLE"]);
		t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_CONTENT"]);
		b_yes->setTag(nIndex + START_INDEX);
	}
	else
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_GIVEUP_TITLE"]);
		t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_TASK_CONTENT"]);
		b_yes->setTag(nIndex + 20 * START_INDEX);
	}
	this->setCameraMask(_cameraMask, true);
}

void UITask::openMianTaskGetRewardDialog(const GetMainTaskRewardResult *taskReward)
{
	auto rewardInfo = SINGLE_SHOP->getMainTaskInfo()[m_nCurrentTaskId].rewards;
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushPlayerAddExpOrFrame(taskReward->exp, taskReward->fame, taskReward->rewardexp, taskReward->rewardfame);

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
	for (size_t i = 0; i < rewardInfo.items.size(); i++)
	{
		auto p_item_clone = p_item->clone();
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_item_clone, "image_item"));
		auto t_item = p_item_clone->getChildByName<Text*>("label_items_name");
		auto t_num = p_item_clone->getChildByName<Text*>("label_num");
		std::string itemName;
		std::string itemPath;
		getItemNameAndPath(rewardInfo.items[i].item_type, rewardInfo.items[i].item_id, itemName, itemPath);
		int amount = rewardInfo.items[i].amount;
		i_item->ignoreContentAdaptWithSize(false);
		i_item->loadTexture(itemPath);
		t_item->setString(itemName);
		t_num->setString(StringUtils::format("+ %d", amount));
		l_result->pushBackCustomItem(p_item_clone);
		auto i_div1 = i_div->clone();
		l_result->pushBackCustomItem(i_div1);
	}

	if (taskReward->forceid)
	{
		auto p_force_clone = p_force->clone();
		auto t_title = p_force_clone->getChildByName<Text*>("label_force_relation");
		auto i_force = p_force_clone->getChildByName<ImageView*>("image_silver_2");
		auto t_num = p_force_clone->getChildByName<Text*>("label_buy_num");
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_REWARD_FORCE_RELATION"]);
		i_force->ignoreContentAdaptWithSize(false);
		i_force->loadTexture(getCountryIconPath(taskReward->forceid));
		t_num->setString(StringUtils::format("+ %d", taskReward->forceaffect));
		//棕色
		t_num->setTextColor(Color4B(40, 25, 13, 255));
		l_result->pushBackCustomItem(p_force_clone);
		auto i_div2 = i_div->clone();
		l_result->pushBackCustomItem(i_div2);
	}

	if (taskReward->rewardcoin)
	{
		auto p_silver_clone = p_coin->clone();
		auto i_silver = p_silver_clone->getChildByName<ImageView*>("image_silver");
		auto t_silver_num = p_silver_clone->getChildByName<Text*>("label_buy_num");
		t_silver_num->setString(numSegment(StringUtils::format("+%lld", taskReward->rewardcoin)));
		l_result->pushBackCustomItem(p_silver_clone);
		auto i_div3 = i_div->clone();
		l_result->pushBackCustomItem(i_div3);
	}
	
	if (taskReward->rewardgold)
	{
		auto p_gold_clone = p_coin->clone();
		auto i_gold = p_gold_clone->getChildByName<ImageView*>("image_silver");
		auto t_gold_num = p_gold_clone->getChildByName<Text*>("label_buy_num");
		i_gold->ignoreContentAdaptWithSize(false);
		i_gold->loadTexture(getVticketOrCoinPath(10000, 1));
		t_gold_num->setString(numSegment(StringUtils::format("+%lld", taskReward->rewardgold)));
		l_result->pushBackCustomItem(p_gold_clone);
		auto i_div4 = i_div->clone();
		l_result->pushBackCustomItem(i_div4);
	}
	this->setCameraMask(_cameraMask, true);

	SINGLE_HERO->m_iCoin = taskReward->coins;
	SINGLE_HERO->m_iGold = taskReward->golds;
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	if (mainlayer)
	{
		mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
	}
}
void UITask::openNomalGetRewardDialog(const GetDailyActivitiesRewardResult *result)
{
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushPlayerAddExpOrFrame(SINGLE_HERO->m_iExp, SINGLE_HERO->m_iRexp, 0, 0);

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
	if (result->reward_golds)
	{
		auto p_gold_clone = p_coin->clone();
		auto i_gold = p_gold_clone->getChildByName<ImageView*>("image_silver");
		auto t_gold_num = p_gold_clone->getChildByName<Text*>("label_buy_num");
		i_gold->ignoreContentAdaptWithSize(false);
		i_gold->loadTexture(getVticketOrCoinPath(10000, 1));
		std::string str = numSegment((StringUtils::format("%lld", result->reward_golds)));
		t_gold_num->setString(StringUtils::format("+%s", str.c_str()));
		l_result->pushBackCustomItem(p_gold_clone);
		auto i_div4 = i_div->clone();
		l_result->pushBackCustomItem(i_div4);
	}

	this->setCameraMask(_cameraMask, true);

	SINGLE_HERO->m_iGold = result->golds;
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	if (mainlayer)
	{
		mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
		mainlayer->flushExpAndRepLv();
	}
}
void UITask::getNpcPresentItem(const GetPretaskItemsResult *preTask)
{
	SINGLE_HERO->m_iCoin = preTask->coin;
	SINGLE_HERO->m_iGold = preTask->golds;
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	if (mainlayer)
	{
		mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
	}

	openView(COMMOM_COCOS_RES[C_VIEW_GETITEM_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_GETITEM_CSB]);
	auto l_result = view->getChildByName<ListView*>("listview_result");
	auto p_silver = l_result->getItem(0);
	auto p_bank = l_result->getItem(1);
	auto p_item = l_result->getItem(2);
	
	auto coinBoo = false;
	if (preTask->addcoin > 0)
	{
		auto t_silver = p_silver->getChildByName<Text*>("label_buy_num");
		std::string numString = numSegment(StringUtils::format("%lld", preTask->addcoin));
		t_silver->setString(StringUtils::format("+ %s", numString.c_str()));
		coinBoo = true;
	}

	if (preTask->addgolds > 0)
	{
		auto p_golds = p_silver->clone();
		l_result->addChild(p_golds);
		auto i_golds = p_golds->getChildByName<ImageView*>("image_silver");
		auto t_silver = p_golds->getChildByName<Text*>("label_buy_num");
		std::string numString = numSegment(StringUtils::format("%lld", preTask->addgolds));
		t_silver->setString(StringUtils::format("+ %s", numString.c_str()));
		i_golds->ignoreContentAdaptWithSize(false);
		i_golds->loadTexture(getVticketOrCoinPath(10000, 1));
	}
	
	if (preTask->bank > 0)
	{
		auto t_silver = p_bank->getChildByName<Text*>("label_buy_num");
		std::string numString = numSegment(StringUtils::format("%lld", preTask->bank));
		t_silver->setString(StringUtils::format("+ %s", numString.c_str()));
	}
	else
	{
		l_result->removeChild(p_bank);
	}

	if (!coinBoo)
	{
		l_result->removeChild(p_silver);
	}

	if (preTask->n_items < 1)
	{
		l_result->removeChild(p_item);
		return;
	}

	std::string name;
	std::string path;
	for (size_t i = 0; i < preTask->n_items; i++)
	{
		if (i == 0)
		{
			auto i_item_bg = p_item->getChildByName<ImageView*>("image_material_bg");
			auto i_item = i_item_bg->getChildByName<ImageView*>("image_item");
			auto t_item_name = p_item->getChildByName<Text*>("label_items_name");
			auto t_item_num = p_item->getChildByName<Text*>("label_num");
			i_item_bg->addTouchEventListener(CC_CALLBACK_2(UITask::goodCall_func, this));
			i_item_bg->setTouchEnabled(true);
			i_item_bg->setTag(preTask->items[i]->itemid);
			getItemNameAndPath(preTask->items[i]->itemtype,preTask->items[i]->itemid,name,path);
			i_item->ignoreContentAdaptWithSize(false);
			i_item->loadTexture(path);
			i_item->setTag(preTask->items[i]->itemtype);
			auto nMaxStringNum = 0;
			auto languageTypeNum = 0;
			if (isChineseCharacterIn(name.c_str()))
			{
				languageTypeNum = 1;
			}
			if (languageTypeNum)
			{
				nMaxStringNum = 22;
			}
			else
			{
				nMaxStringNum = 40;
			}
			t_item_name->setString(apostrophe(name, nMaxStringNum));
			t_item_name->ignoreContentAdaptWithSize(true);
			//t_item_name->setString(name);
			t_item_num->setString(StringUtils::format("+ %d",preTask->items[i]->itemamount));
			setBgButtonFormIdAndType(i_item_bg, preTask->items[i]->itemid, preTask->items[i]->itemtype);
			setTextColorFormIdAndType(t_item_num, preTask->items[i]->itemid, preTask->items[i]->itemtype);
		}
		else
		{
			auto p_item_clone = p_item->clone();
			auto i_item_bg = p_item_clone->getChildByName<ImageView*>("image_material_bg");
			auto i_item = i_item_bg->getChildByName<ImageView*>("image_item");
			auto t_item_name = p_item_clone->getChildByName<Text*>("label_items_name");
			auto t_item_num = p_item_clone->getChildByName<Text*>("label_num");
			i_item_bg->addTouchEventListener(CC_CALLBACK_2(UITask::goodCall_func, this));
			i_item_bg->setTouchEnabled(true);
			i_item_bg->setTag(preTask->items[i]->itemid);
			getItemNameAndPath(preTask->items[i]->itemtype, preTask->items[i]->itemid, name, path);
			i_item->ignoreContentAdaptWithSize(false);
			i_item->loadTexture(path);
			i_item->setTag(preTask->items[i]->itemtype);
			auto nMaxStringNum = 0;
			auto languageTypeNum = 0;
			if (isChineseCharacterIn(name.c_str()))
			{
				languageTypeNum = 1;
			}
			if (languageTypeNum)
			{
				nMaxStringNum = 22;
			}
			else
			{
				nMaxStringNum = 40;
			}
			t_item_name->setString(apostrophe(name, nMaxStringNum));
			t_item_name->ignoreContentAdaptWithSize(true);
			//t_item_name->setString(name);
			t_item_num->setString(StringUtils::format("+ %d", preTask->items[i]->itemamount));
			setBgButtonFormIdAndType(i_item_bg, preTask->items[i]->itemid, preTask->items[i]->itemtype);
			setTextColorFormIdAndType(t_item_num, preTask->items[i]->itemid, preTask->items[i]->itemtype);
			l_result->pushBackCustomItem(p_item_clone);
		}
	}
	auto b_yes = view->getChildByName<Button*>("button_result_yes");
	b_yes->setName("button_present_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UITask::menuCall_func,this));
	
	this->setCameraMask(_cameraMask, true);
}

void UITask::openPrayDialog(const GetPrayResult *pPrayResult)
{
	openView(COMMOM_COCOS_RES[C_VIEW_GET_REWARD]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_GET_REWARD]);
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title"));
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EVNETS_FAITH_RESULT"]);
	if (SINGLE_HERO->m_heroIsOnsea)
	{
		view->setCameraMask(4, true);
	}
	else
	{
		view->setCameraMask(1, true);
	}

	int n = 1;
	//物品和银币只可能有一个
	if (pPrayResult->item || pPrayResult->add_coins > 0)
	{
		n++;
	}
	for (size_t i = 0; i < 3; i++)
	{
		auto item = view->getChildByName<ImageView*>(StringUtils::format("image_items_bg_%d", i + 1));
		item->addTouchEventListener(CC_CALLBACK_2(UITask::goodCall_func, this));
		item->setTouchEnabled(true);
		if (i < n)
		{
			auto i_icon = item->getChildByName<ImageView*>("image_item");
			auto t_num = item->getChildByName<Text*>("label_item_num");
			i_icon->ignoreContentAdaptWithSize(false);

			if (i == 0)
			{
				i_icon->loadTexture(getItemIconPath(10001));
				setTextSizeAndOutline(t_num, pPrayResult->add_sailer_coins);
				setBgButtonFormIdAndType(item, 10001, ITEM_TYPE_SPECIAL);
				item->setTag(10001);
				i_icon->setTag(ITEM_TYPE_SPECIAL);
			}
			else
			{
				if (pPrayResult->add_coins > 0)
				{
					i_icon->loadTexture(getItemIconPath(10002));
					setTextSizeAndOutline(t_num, pPrayResult->add_coins);
					setBgButtonFormIdAndType(item, 10002, ITEM_TYPE_SPECIAL);
					item->setTag(10002);
					i_icon->setTag(ITEM_TYPE_SPECIAL);
				}
				else
				{
					i_icon->loadTexture(getItemIconPath(pPrayResult->item->itemid));
					setTextSizeAndOutline(t_num, pPrayResult->item->itemcount);
					int type = SINGLE_SHOP->getItemData()[pPrayResult->item->itemid].type;
					setBgButtonFormIdAndType(item, pPrayResult->item->itemid, type);
					item->setTag(pPrayResult->item->itemid);
					i_icon->setTag(type);
				}
			}
			item->setVisible(true);
		}
		else
		{
			item->setVisible(false);
		}
		//位置微调
		if (n == 1)
		{
			if (i == 0)
			{
				item->setPositionX(item->getPositionX() + item->getBoundingBox().size.width*1.3);
			}
		}
		else if (n == 2)
		{
			if (i == 0 || i == 1)
			{
				item->setPositionX(item->getPositionX() + item->getBoundingBox().size.width*0.8);
			}
		}
	}

	

	SINGLE_HERO->m_iCoin = pPrayResult->coins;
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	if (mainlayer)
	{
		mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
	}

	m_userTask->prayremaintime = pPrayResult->remain_time;
	m_userTask->praycollingtime = pPrayResult->praycollingtime;
	view = getViewRoot("cocosstudio/voyageUI_quest.csb");
	auto p_piratessiege = view->getChildByName<Widget*>("panel_event_piratessiege");
	auto p_faith = view->getChildByName<Widget*>("panel_event_faith");
	auto panel_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_faith, "panel_1"));
	auto panel_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_faith, "panel_2"));
	panel_1->setVisible(false);
	panel_2->setVisible(true);

	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_2, "text_name"));
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_2, "text_2"));
	auto i_belief = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_2, "image_belief"));

	std::string name = StringUtils::format("TIP_FAITH_NAME_%d", pPrayResult->lastpreyindex);
	std::string content = StringUtils::format("TIP_FAITH_CONTENT_%d", pPrayResult->lastpreyindex);
	t_name->setString(SINGLE_SHOP->getTipsInfo()[name]);
	t_content->setString(SINGLE_SHOP->getTipsInfo()[content]);
	i_belief->ignoreContentAdaptWithSize(false);
	i_belief->loadTexture(StringUtils::format("cocosstudio/login_ui/activity_720/belief_%d.png", pPrayResult->lastpreyindex));
}

void UITask::goodCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		Widget* button = (Widget*)pSender;
		std::string name = button->getName();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

		auto i_icon = button->getChildByName<ImageView*>("image_item");
		openGoodInfo(nullptr, i_icon->getTag(), button->getTag());
	}
}

void UITask::openSublineRewardDialog()
{
	UICommon::getInstance()->openCommonView(this);
//	UICommon::getInstance()->flushPlayerAddExpOrFrame(result->exp, result->fame, result->rewardexp, result->rewardfame);
	UICommon::getInstance()->flushPlayerAddExpOrFrame(9999, 9999, 0, 0);

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
	SDGuideTasks *task = SDGuideTasks::GetElement(m_nSublineId);
	SDGuideTasksReward * taskReward = SDGuideTasksReward::GetElement(m_nSublineId);

	if (taskReward->getitem1amount() > 0 && taskReward->getitem1id())
	{
		auto p_item_clone = p_item->clone();
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_item_clone, "image_item"));
		auto t_item = p_item_clone->getChildByName<Text*>("label_items_name");
		auto t_num = p_item_clone->getChildByName<Text*>("label_num");
		std::string itemName;
		std::string itemPath;
		getItemNameAndPath(taskReward->getitem1type(), taskReward->getitem1id(), itemName, itemPath);
		int amount = taskReward->getitem1amount();
		i_item->ignoreContentAdaptWithSize(false);
		i_item->loadTexture(itemPath);
		t_item->setString(itemName);
		t_num->setString(StringUtils::format("+ %d", amount));
		l_result->pushBackCustomItem(p_item_clone);
		auto i_div1 = i_div->clone();
		l_result->pushBackCustomItem(i_div1);
		if (taskReward->getitem2amount() > 0 && taskReward->getitem2id())
		{
			auto p_item_clone = p_item->clone();
			auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_item_clone, "image_item"));
			auto t_item = p_item_clone->getChildByName<Text*>("label_items_name");
			auto t_num = p_item_clone->getChildByName<Text*>("label_num");
			getItemNameAndPath(taskReward->getitem2type(), taskReward->getitem2id(), itemName, itemPath);
			int amount = taskReward->getitem2amount();
			i_item->ignoreContentAdaptWithSize(false);
			i_item->loadTexture(itemPath);
			t_item->setString(itemName);
			t_num->setString(StringUtils::format("+ %d", amount));
			l_result->pushBackCustomItem(p_item_clone);
			auto i_div1 = i_div->clone();
			l_result->pushBackCustomItem(i_div1);
		}
	}

	if (taskReward->getcoins())
	{
		auto p_silver_clone = p_coin->clone();
		auto i_silver = p_silver_clone->getChildByName<ImageView*>("image_silver");
		auto t_silver_num = p_silver_clone->getChildByName<Text*>("label_buy_num");
		std::string str = numSegment((StringUtils::format("%d", taskReward->getcoins())));
		t_silver_num->setString(StringUtils::format("+%s", str.c_str()));
		l_result->pushBackCustomItem(p_silver_clone);
		auto i_div3 = i_div->clone();
		l_result->pushBackCustomItem(i_div3);
	}

	if (taskReward->getv_tickets())
	{
		auto p_gold_clone = p_coin->clone();
		auto i_gold = p_gold_clone->getChildByName<ImageView*>("image_silver");
		auto t_gold_num = p_gold_clone->getChildByName<Text*>("label_buy_num");
		i_gold->ignoreContentAdaptWithSize(false);
		i_gold->loadTexture(getVticketOrCoinPath(10000, 1));
		std::string str = numSegment((StringUtils::format("%d", taskReward->getv_tickets())));
		t_gold_num->setString(StringUtils::format("+%s", str.c_str()));
		l_result->pushBackCustomItem(p_gold_clone);
		auto i_div4 = i_div->clone();
		l_result->pushBackCustomItem(i_div4);
	}

	if (taskReward->getv_badges())
	{
		auto p_badges_clone = p_coin->clone();
		auto i_badges = p_badges_clone->getChildByName<ImageView*>("image_silver");
		auto t_badges_num = p_badges_clone->getChildByName<Text*>("label_buy_num");
		i_badges->ignoreContentAdaptWithSize(false);
		i_badges->loadTexture(getItemIconPath(10001));
		std::string str = numSegment((StringUtils::format("%d", taskReward->getv_badges())));
		t_badges_num->setString(StringUtils::format("+%s", str.c_str()));
		l_result->pushBackCustomItem(p_badges_clone);
		auto i_div5 = i_div->clone();
		l_result->pushBackCustomItem(i_div5);
	}
	
//	SINGLE_HERO->m_iCoin = result->coin;
//	SINGLE_HERO->m_iGold = result->gold;

	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	if (mainlayer)
	{
		mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
		mainlayer->flushExpAndRepLv();
	}
	
	m_nSublineId++;
	m_bSublineFinish = true;
	flushLeftTaskButton();
}