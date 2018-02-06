/*
 * MainTaskManager.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: xijing
 */

#include "MainTaskManager.h"
#include "ProtocolThread.h"
#include "ShopData.h"
#include "TVSingle.h"
#include "UIStoryLine.h"
#include "TVSceneLoader.h"
#include "TVSailDefineVar.h"

#include "CompanionTaskManager.h"

MainTaskManager::MainTaskManager() 
{
	// TODO Auto-generated constructor stub
	m_currentTaskId = 0;
	m_currentPartId = 0;
	m_isCurrentTaskCompleted = false;
	m_PartsInfo = nullptr;
	m_currentCityId = 0;
	m_currentCityArea = AREA__MAINCITY;
	m_seaX = 0;
	m_seaY = 0;
	notifyDialogId = 0;
	//m_isCurrentTaskFinihed = false;
	m_bShowChat = false;
	m_sailDays = -1;
	m_lastSendXYTime = 0;
	m_isTriggering = false;
}

MainTaskManager* MainTaskManager::m_pInstance=0;
MainTaskManager* MainTaskManager::GetInstance()
{
	if(!m_pInstance)
	{
		m_pInstance = new MainTaskManager;
	}
	return m_pInstance;
}

MainTaskManager::~MainTaskManager() 
{
	// TODO Auto-generated destructor stub
}

PARTSINFO* MainTaskManager::getPartInfo(int taskId,int partId)
{
	if(partId <= 0)
		return 0;
	std::map<int, MAINTASKINFO>& tasksInfo = SINGLE_SHOP->getMainTaskInfo();
	MAINTASKINFO& task = tasksInfo[taskId];
	if(task.trigger.part_num >= partId)
	{
		auto& parts = task.trigger.parts;
		std::vector<PARTSINFO>::iterator it = parts.end();
		for (auto i = parts.begin(); i != parts.end(); i++)
		{
			auto& item = *i;
			if(item.part == partId)
			{
				return &item;
			}
		}
	}
	return 0;
}

void MainTaskManager::reportSeaXY(int x,int y)
{
	if(x>0 && y >0)
	{
		m_currentCityId = 0;
		m_seaX = x;
		m_seaY = y;
	}
}

void MainTaskManager::reportSeaXY(int x,int y,int sailDays)
{
	if(!m_PartsInfo)
		return;
	Vec2 position1(x,y);
	Vec2 position2(m_PartsInfo->x,m_PartsInfo->y);
	float dis = position1.distance(position2);
	timeval timeStruc;
	gettimeofday(&timeStruc,nullptr);
	int64_t sec1 = timeStruc.tv_sec;
	if(m_lastSendXYTime + 3 < sec1)
	{
		//log("gotcha!!!");
		m_lastSendXYTime = sec1;
#if 0
		if(dis <= m_PartsInfo->r)
		{
			ProtocolThread::GetInstance()->getNextMapBlock(x,MAP_WIDTH * MAP_CELL_HEIGH - y,sailDays);
		}
#endif
		reportSeaXY(x,y);
		m_sailDays = sailDays;
	}
	//log("-------------------------------------------------------------------distance:%.f---", dis);
}

void MainTaskManager::reportCityArea(int cityId,CityAreaType area)
{
	if(cityId > 0)
	{
		m_currentCityId = cityId;
		m_currentCityArea = area;

		m_seaX = m_seaY = 0;
	}
}

bool MainTaskManager::matchTaskPosition()
{
	if(m_currentCityId > 0)
	{
		return matchCityTaskPosition(m_currentCityId,m_currentCityArea);
	}
	else
	{
		return matchSeaTaskPosition(m_seaX,m_seaY);
	}
}

bool MainTaskManager::matchSeaTaskPosition(int x,int y)
{
	if(m_PartsInfo)
	{
		if(!m_PartsInfo->has_position)
		{
			return true;
		}
		else
		{
			if(!m_PartsInfo->sea)
				return false;

			if (m_PartsInfo->x == 0 && m_PartsInfo->y == 0)
			{
				return true;
			}
			Vec2 position1(x,y);
			Vec2 position2(m_PartsInfo->x,m_PartsInfo->y);
			if(position1.distance(position2) <= m_PartsInfo->r)
			{
				return true;
			}
		}
	}
	return false;
}

bool MainTaskManager::matchCityTaskPosition(int cityId,int cityArea)
{
	if(m_PartsInfo)
	{
		if(!m_PartsInfo->has_position)
		{
			return true;
		}
		else
		{
			if(m_PartsInfo->sea)
				return false;
			if ((m_PartsInfo->city == 0 || m_PartsInfo->city == cityId) && (m_PartsInfo->cityarea == cityArea || m_PartsInfo->cityarea == 0))
				return true;
		}
	}
	return false;
}

bool MainTaskManager::matchTaskConidition()
{
	if(m_PartsInfo)
	{
		if(!m_PartsInfo->has_condition)
		{
			return true;
		}

		switch(m_PartsInfo->type)
		{
		case TYPE__MAKE_SHIP:
		{
			if (m_PartsInfo->target_id ==  0 || (m_PartsInfo->target_id != 0 && 
				m_PartsInfo->target_id == m_targetData.m_targetId))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		case TYPE__HIRE_CAPTAIN:
		{
			if (m_PartsInfo->target_id == 0 || (m_PartsInfo->target_id != 0 && 
				m_PartsInfo->target_id == m_targetData.m_targetId))
			{
				return !!m_targetData.m_targetId;
			}
			else
			{
				return false;
			}
			break;
		}
		case TYPE__ASSIGN_CAPTAIN:
		{						
			return !!m_targetData.m_targetId;

		}
			break;
		case TYPE__REINFORCE_SHIP:
		{
			return !!m_targetData.m_targetId;
		}
			break;
		case TYPE__WITHDRAW_MONEY:
		{
			return m_targetData.m_targetAmount>0;
		}
			break;
		case TYPE__INVEST_PALACE:
		{
//			if (m_PartsInfo->target_id == m_targetCity.m_targetId)
//			{
//				return m_isCurrentTaskFinihed;
//			}
//			else
//			{
//				return false;
//			}
			return m_targetData.m_targetAmount>0;
		}
			break;
		case TYPE__BUY_ITEM:
		{
			if(m_PartsInfo->items.size() == 0)
			{
				return true;
			}
			if(m_boughtItems.size() == 0)
				return false;
			for(unsigned int i=0;i<m_PartsInfo->items.size();i++)
			{
				auto needItem = m_PartsInfo->items[i];
				bool found = 0;
				for(unsigned int j=0;j<m_boughtItems.size();j++)
				{
					auto hasItem = m_boughtItems[j];
					if(needItem.item_type == hasItem.m_itemtype && hasItem.m_itemid == needItem.item_id && needItem.amount <= hasItem.m_itemamount)
					{
						found = 1;
						break;
					}
				}
				if(!found)
					return false;
			}
			return true;
//			if(m_itemtype == m_PartsInfo->items[0].item_type && m_itemid == m_PartsInfo->items[0].item_id && m_PartsInfo->items[0].amount <= m_itemamount){
//				return true;
//			}
		}
			break;
		case TYPE__HAS_SHIP:
		{
			if (m_PartsInfo->target_id == m_targetData.m_targetId && m_PartsInfo->target_city == m_targetData.m_targetCity
				&& m_PartsInfo->target_amount <= m_targetData.m_targetAmount)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		default:
			break;
		}
	}
	return false;
}

void MainTaskManager::reset()
{
	m_currentCityId = 0;
	m_currentCityArea = AREA__MAINCITY;
	m_seaX = 0;
	m_seaY = 0;
	m_isTriggering = false;
	m_bShowChat = false;
	//m_hadSendXY = false;
	notifyDialogId = 0;
	m_boughtItems.clear();
	//m_targetData.m_targetAmount = 0;
	//m_targetData.m_targetCity = 0;
	//m_targetData.m_targetId = 0;
}

void MainTaskManager::notifyUIPartDelayTime(float time)
{
	Director::getInstance()->getScheduler()->schedule(schedule_selector(MainTaskManager::notifyUIPartCompleted), this, 0.1f, 0.1f, time, false);
}

void MainTaskManager::notifyUIPartCompleted(float t)
{
	Scene*currentScene = Director::getInstance()->getRunningScene();
	if(currentScene){
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(MainTaskManager::notifyUIPartCompleted), this);
		auto dialogLayer = UIStoryLine::GetInstance();
		if (!dialogLayer->getParent())
		{
			currentScene->addChild(dialogLayer, 10001);
		}
		dialogLayer->onMainTaskMessage(notifyDialogId, 0);
	}
}

void MainTaskManager::completeMainTask()
{
	if (m_PartsInfo && m_PartsInfo->part == m_currentPartId)
	{
		setIsTriggering(false);
		ProtocolThread::GetInstance()->completeMainTask(m_currentPartId);
	}
}

void MainTaskManager::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	CompanionTaskManager::GetInstance()->onServerEvent(message,msgType);
	switch (msgType) {

	case PROTO_TYPE_GetSailInfoResult: 
	{
		GetSailInfoResult *result = (GetSailInfoResult*) message;
		if (result->failed)
			return;
		if (result->maintaskinfo) 
		{
			if (result->maintaskinfo->completedpartnum == -1) 
			{ //task completed
				m_currentTaskId = result->maintaskinfo->taskid;
				m_currentPartId = -1;
				m_isCurrentTaskCompleted = true;
				//m_isCurrentTaskFinihed = true;
			} 
			else
			{// if (m_currentTaskId != result->maintaskinfo->taskid || m_currentPartId != result->maintaskinfo->completedpartnum + 1) {
				reset();
				m_currentTaskId = result->maintaskinfo->taskid;
				m_currentPartId = result->maintaskinfo->completedpartnum + 1;
				m_PartsInfo = getPartInfo(m_currentTaskId, m_currentPartId);
				m_isCurrentTaskCompleted = false;
				//m_isCurrentTaskFinihed = false;
				for (int i = 0; i < result->maintaskinfo->n_items; i++) 
				{
					int m_itemtype, m_itemid, m_itemamount;
					m_itemtype = result->maintaskinfo->items[i]->taskitemtype;
					m_itemid = result->maintaskinfo->items[i]->taskitemid;
					m_itemamount = result->maintaskinfo->items[i]->taskitemamount;

					struct BoughtItem item = { m_itemtype, m_itemid, m_itemamount };
					m_boughtItems.push_back(item);
				}
			}
		} 
		else 
		{
			m_currentTaskId = 0;
			m_currentPartId = 0;
			m_isCurrentTaskCompleted = true;
			//m_isCurrentTaskFinihed = true;
		}
		reportSeaXY(result->seadata->shipx, MAP_WIDTH * MAP_CELL_WIDTH - result->seadata->shipy);
	}
		break;
	case PROTO_TYPE_GetCurrentCityDataResult: 
	{
		GetCurrentCityDataResult *result = (GetCurrentCityDataResult*) message;
		if (result->failed)
			return;
		if (result->maintaskinfo) 
		{
			if(result->maintaskinfo->completedpartnum == -1)
			{//task completed
				m_currentTaskId = result->maintaskinfo->taskid;
				m_currentPartId = -1;
				m_isCurrentTaskCompleted = true;
				//m_isCurrentTaskFinihed = true;
			}
			else
			{// if (m_currentTaskId != result->maintaskinfo->taskid || m_currentPartId != result->maintaskinfo->completedpartnum+1) {
				reset();
				m_currentTaskId = result->maintaskinfo->taskid;
				m_currentPartId = result->maintaskinfo->completedpartnum+1;
				m_PartsInfo = getPartInfo(m_currentTaskId, m_currentPartId);
				m_isCurrentTaskCompleted = false;
				//m_isCurrentTaskFinihed = false;

				for(int i=0;i<result->maintaskinfo->n_items;i++)
				{
					int m_itemtype,m_itemid,m_itemamount;
					m_itemtype = result->maintaskinfo->items[i]->taskitemtype;
					m_itemid = result->maintaskinfo->items[i]->taskitemid;
					m_itemamount = result->maintaskinfo->items[i]->taskitemamount;

					struct BoughtItem item = {m_itemtype,m_itemid,m_itemamount};
					m_boughtItems.push_back(item);
				}
			}
		}
		else 
		{
			m_currentTaskId = 0;
			m_currentPartId = 0;
			m_isCurrentTaskCompleted = true;
			//m_isCurrentTaskFinihed = true;
		}
		reportCityArea(result->data->lastcity->cityid, AREA__MAINCITY);
	}
		break;
	case PROTO_TYPE_GetUserTasksResult: 
	{
		GetUserTasksResult *result = (GetUserTasksResult*) message;
		if (result->failed)
			return;

		if (result->maintaskstatus == 1) 
		{ //completed,none of your business
			m_currentTaskId = 0;
			m_currentPartId = 0;
			m_isCurrentTaskCompleted = true;
			//m_isCurrentTaskFinihed = true;
		}
		else 
		{
			if (m_currentTaskId != result->maintaskid || m_currentPartId != result->maintaskcompletedpartid + 1) 
			{
				log("fix me");
				m_currentTaskId = result->maintaskid;
				m_currentPartId = result->maintaskcompletedpartid + 1;
				m_PartsInfo = getPartInfo(m_currentTaskId, m_currentPartId);
			}
			m_isCurrentTaskCompleted = false;
			//m_isCurrentTaskFinihed = false;
		}
	}
		break;
	case PROTO_TYPE_BuyVTicketMarketItemResult: 
	{

	}
		break;
	case PROTO_TYPE_GetHiredCaptainsResult: 
	{
		auto result = (GetHiredCaptainsResult *)message;
		if (result->failed == 0)
		{
//			reportCityArea(m_currentCityId, AREA__TARVEN);
//			for (size_t i = 0; i < result->n_mycaptains; i++)
//			{
//				if (result->mycaptains[i]->position >= 0)
//				{
//					m_isCurrentTaskFinihed = true;
//				}
//			}
		}
	}
		break;
	case PROTO_TYPE_EndFightResult: 
	{
		EndFightResult *result = (EndFightResult*) message;
		if (result->failed == 0 && result->reason == 1) 
		{
			//result->npcid;
		}
	}
		break;
	case PROTO_TYPE_GetBankInfoResult: 
	{
		GetBankInfoResult *result = (GetBankInfoResult*) message;
		if (result->failed == 0) 
		{
			reportCityArea(m_currentCityId, AREA__BANK);
			if(result->myinsurance && result->myinsurance->leftamount > 0 && result->myinsurance->lefttimeinseconds > 0)
			{
				if (m_PartsInfo && m_PartsInfo->type == TYPE__WITHDRAW_MONEY && matchTaskPosition())
				{
					m_targetData.m_targetAmount = 1;//result->coin;
				}
			}
		}
	}
		break;
	case PROTO_TYPE_BuyInsuranceResult:
	{
		BuyInsuranceResult*result = (BuyInsuranceResult*)message;
		if(result->failed == 0)
		{
			reportCityArea(m_currentCityId, AREA__BANK);
			//m_isCurrentTaskFinihed = true;
			if (m_PartsInfo && m_PartsInfo->type == TYPE__WITHDRAW_MONEY && matchTaskPosition())
			{
				m_targetData.m_targetAmount = result->coin;
			}
		}
	}
		break;
	case PROTO_TYPE_WithdrawMoneyResult: 
	{
		WithdrawMoneyResult *result = (WithdrawMoneyResult*)message;
		if (result->failed == 0)
		{
			reportCityArea(m_currentCityId, AREA__BANK);
			//m_isCurrentTaskFinihed = true;
			if (m_PartsInfo && m_PartsInfo->type == TYPE__WITHDRAW_MONEY && matchTaskPosition())
			{
				m_targetData.m_targetAmount = result->coin;
			}
		}
	}
		break;
	case PROTO_TYPE_GetPalaceInfoResult: 
	{
		GetPalaceInfoResult *result = (GetPalaceInfoResult*) message;
		if (result->failed == 0)
		{
			reportCityArea(m_currentCityId, AREA__PALACE);
		}
	}
		break;
	case PROTO_TYPE_ReachCityResult: 
	{
		ReachCityResult *result = (ReachCityResult*) message;
		if (result->failed == 0) 
		{
			reportCityArea(result->currentcityid, AREA__NOWHERE);
		}
	}
		break;
	case PROTO_TYPE_LeaveCityResult: 
	{
		LeaveCityResult *result = (LeaveCityResult*) message;
		if (result->failed == 0)
		{
			reportSeaXY(result->shipx, MAP_WIDTH * MAP_CELL_WIDTH - result->shipy);
		}
	}
		break;
	case PROTO_TYPE_GetNextMapBlockResult:
	{
		GetNextMapBlockResult* result = (GetNextMapBlockResult*)message;
		if (result->failed == 0)
		{
			//log("X = %.f    Y = %.f", result->blockinfo->x, result->blockinfo->y);
			//reportSeaXY(result->blockinfo->x, MAP_WIDTH * MAP_CELL_WIDTH - result->blockinfo->y);
		}
	}
		break;
	case PROTO_TYPE_GetVisitedCitiesResult: 
	{
		GetVisitedCitiesResult *result = (GetVisitedCitiesResult*) message;
		if (result->failed == 0 && result->incity) {
			reportCityArea(m_currentCityId, AREA__DOCK);
		}
	}
		break;
	case PROTO_TYPE_GetCaptainResult:
	{
		GetCaptainResult* result = (GetCaptainResult*)message;
		if (result->failed == 0)
		{
			reportCityArea(m_currentCityId, AREA__TARVEN);
			//TODO
			if(m_PartsInfo && m_PartsInfo->type == TYPE__HIRE_CAPTAIN)
			{
				if (result->barcaptain)
				{
					m_targetData.m_targetId = result->barcaptain->id;
				}
			}
		}
	}
		break;
	case PROTO_TYPE_GetBarInfoResult:
	{
		GetBarInfoResult *result = (GetBarInfoResult*) message;
		if (result->failed == 0) {
			reportCityArea(m_currentCityId, AREA__TARVEN);
		}
	}
		break;
	case PROTO_TYPE_GetPersonalItemResult: 
	{
		GetPersonalItemResult *result = (GetPersonalItemResult*)message;
		if (result->failed == 0)
		{
			reportCityArea(m_currentCityId, AREA__SHIPYARD_FLEET);
		}
	}
		break;
	case PROTO_TYPE_GetItemsBeingSoldResult: 
	{
		GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult*) message;
		if (result->failed == 0) {
			reportCityArea(m_currentCityId, AREA__MARKET);
		}
	}
		break;
	case PROTO_TYPE_GetFleetAndDockShipsResult:
	{
		GetFleetAndDockShipsResult *result = (GetFleetAndDockShipsResult*) message;
		if (result->failed == 0 && result->incity) 
		{
			if (m_PartsInfo && m_PartsInfo->type == TYPE__HAS_SHIP)
			{
				m_targetData.m_targetAmount = 0;
				for (size_t i = 0; i < result->n_dockships; i++)
				{
					if (result->dockships[i]->sid == m_PartsInfo->target_id)
					{
						m_targetData.m_targetId = m_PartsInfo->target_id;
						m_targetData.m_targetAmount++;
						m_targetData.m_targetCity = m_currentCityId;
					}
				}

//				for (size_t i = 0; i < result->n_fleetships; i++)
//				{
//					if (result->fleetships[i]->sid == m_PartsInfo->target_id)
//					{
//						m_targetCity.m_targetId = m_PartsInfo->target_id;
//						m_targetCity.m_targetAmount++;
//						m_targetCity.m_targetCity = m_currentCityId;
//					}
//				}
			}
			reportCityArea(m_currentCityId, AREA__SHIPYARD_FLEET);
		}
		break;
	}
	case PROTO_TYPE_BuyItemResult:
	{
		BuyItemResult *result = (BuyItemResult*) message;
		if (result->failed == 0 && m_PartsInfo && m_PartsInfo->has_condition && m_PartsInfo->type == TYPE__BUY_ITEM) 
		{
			int m_itemtype = result->itemtype;
			int m_itemid = result->itemid;
			int m_itemamount = result->totalcount;
			int found = 0;
			for(unsigned int i =0;i<m_boughtItems.size();i++)
			{
				if(m_boughtItems[i].m_itemid == m_itemid && m_boughtItems[i].m_itemtype == m_itemtype)
				{
					m_boughtItems[i].m_itemamount += m_itemamount;
					found = 1;
					break;
				}
			}
			if(!found)
			{
				struct BoughtItem item = {m_itemtype,m_itemid,m_itemamount};
				m_boughtItems.push_back(item);
			}
		}
		break;
	}
	case PROTO_TYPE_BuildShipResult:
	{
		BuildShipResult *result = (BuildShipResult*)message;
		if (result->failed == 0)
		{
			if (result->action == 0)
			{
				if (m_PartsInfo && m_PartsInfo->type == TYPE__MAKE_SHIP && m_PartsInfo->city == m_currentCityId)
				{
					m_targetData.m_targetId = result->itemid;
				}
				reportCityArea(m_currentCityId, AREA__SHIPYARD_FLEET);
				//m_isCurrentTaskFinihed = true;
			}
		}
		break;
	}
	case PROTO_TYPE_ReinforceShipResult:
	{
		ReinforceShipResult* result = (ReinforceShipResult*)message;
		if (result->failed == 0)
		{
			if (m_PartsInfo && m_PartsInfo->type == TYPE__REINFORCE_SHIP)
			{
				if(result->shipinfo)
				{
					m_targetData.m_targetId = result->shipinfo->shipid;
				}
			}
			reportCityArea(m_currentCityId, AREA__SHIPYARD_FLEET);
			//m_isCurrentTaskFinihed = true;
		}
		break;
	}
	case PROTO_TYPE_CompleteMainTaskResult:
	{
		CompleteMainTaskResult* result = (CompleteMainTaskResult*)message;
		if (result->failed == 0)
		{
			notifyDialogId = 0;
			m_boughtItems.clear();
			m_targetData.m_targetAmount = 0;
			m_targetData.m_targetCity = 0;
			m_targetData.m_targetId = 0;

			m_currentTaskId = result->currenttaskid;
			m_currentPartId = result->nextpartid;
			m_PartsInfo = getPartInfo(m_currentTaskId, m_currentPartId); 
			if (m_currentPartId == -1)
			{
				UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(MAIN_TASK_FINISH).c_str(), true);
				UserDefault::getInstance()->flush();
			}
			
			if (m_currentTaskId == 2 && m_currentPartId == 3)
			{
				CHANGETO(SCENE_TAG::MAIN_TAG);
			}
		}
		else
		{
			log("complete main task error !!!");
		}
		break;
	}
	case PROTO_TYPE_InvestCityResult: 
	{
		auto result = (InvestCityResult *)message;
		if (result->failed == 0)
		{
			reportCityArea(m_currentCityId, AREA__PALACE);
			if (m_PartsInfo && m_PartsInfo->type == TYPE__INVEST_PALACE && matchTaskPosition()){
				m_targetData.m_targetAmount = result->investcoin;
			}
		}
	}
		break;
	case PROTO_TYPE_FleetFormationResult:
	{
		FleetFormationResult*result = (FleetFormationResult*)message;
		if(result->failed == 0){
			if(m_PartsInfo && m_PartsInfo->type == TYPE__ASSIGN_CAPTAIN)
			{
				for (size_t i = 0; i < result->n_captainids; i++)
				{
					if (result->captainids[i]->captainid != 0)
					{
						m_targetData.m_targetId = result->captainids[i]->captainid;
						break;
					}
				}
			}
			reportCityArea(m_currentCityId, AREA__SHIPYARD_FLEET);
		}
	}
		break;
	default:
		break;
	}
//	if(matchTaskPosition() && matchTaskConidition()){
//		if(m_PartsInfo){
//			notifyDialogId = m_PartsInfo->action;
//			m_bShowChat = true;
//		}
//
//		//m_currentTaskId = 0;
//		//m_currentPartId = 0;
//		//m_isCurrentTaskCompleted = true;
//	}
}

int MainTaskManager::checkTaskAction()
{
	if(matchTaskPosition() && matchTaskConidition())
	{
		if(m_PartsInfo)
		{
			notifyDialogId = m_PartsInfo->action;
			m_bShowChat = true;
			/*
			//配合对话特殊处理
			if (SINGLE_HERO->m_iNation == 1 && notifyDialogId == 6)
			{
				return 0;
			}
			*/
			return notifyDialogId;
		}
	}
	return 0;
}

