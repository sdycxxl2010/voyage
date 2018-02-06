/*
 * CompanionTaskManager.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: xijing
 */
#include "CompanionTaskManager.h"
#include "ProtocolThread.h"
#include "ShopData.h"
#include "TVSingle.h"
#include "UIStoryLine.h"
#include "TVSceneLoader.h"
#include "TVSailDefineVar.h"

CompanionTaskManager::CompanionTaskManager()
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

	m_sailDays = -1;
	m_lastSendXYTime = 0;
	m_isTriggering = false;
	missionFailed = 0;
}

CompanionTaskManager* CompanionTaskManager::m_pInstance=0;
CompanionTaskManager* CompanionTaskManager::GetInstance()
{
	if(!m_pInstance)
	{
		m_pInstance = new CompanionTaskManager;
	}
	return m_pInstance;
}

CompanionTaskManager::~CompanionTaskManager()
{

}

PARTSINFO* CompanionTaskManager::getPartInfo(int taskId,int partId)
{
	if(partId <= 0)
		return 0;
	std::map<int, PARTNERDIALOGJUDGEINFO>& tasksInfo = SINGLE_SHOP->getPartnerDialogJudgeInfo();
	PARTNERDIALOGJUDGEINFO& task = tasksInfo[taskId];
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

void CompanionTaskManager::reportSeaXY(int x,int y)
{
	if(x>0 && y >0)
	{
		m_currentCityId = 0;
		m_seaX = x;
		m_seaY = y;
	}
}

void CompanionTaskManager::reportSeaXY(int x,int y,int sailDays)
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

void CompanionTaskManager::reportCityArea(int cityId,CityAreaType area)
{
	if(cityId > 0)
	{
		m_currentCityId = cityId;
		m_currentCityArea = area;

		m_seaX = m_seaY = 0;
	}
}

void CompanionTaskManager::reportCityArea(CityAreaType area)
{
	m_currentCityArea = area;
}

bool CompanionTaskManager::matchTaskPosition()
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

bool CompanionTaskManager::matchSeaTaskPosition(int x,int y)
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
			//log("--distance:%.f x = %d y = %d m_PartsInfo->x = %d m_PartsInfo->y = %d", position1.distance(position2), x, y, m_PartsInfo->x, m_PartsInfo->y);
			int min = 100;
			if(m_PartsInfo->r > min)
				min = m_PartsInfo->r;
			if(position1.distance(position2) <= min)
			{
				return true;
			}
		}
	}
	return false;
}

bool CompanionTaskManager::matchCityTaskPosition(int cityId,int cityArea)
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
			if ((m_PartsInfo->city == 0 || m_PartsInfo->city == cityId) && (m_PartsInfo->cityarea == cityArea/* || m_PartsInfo->cityarea == 0*/))
				return true;
		}
	}
	return false;
}

bool CompanionTaskManager::matchTaskConidition()
{
	if(m_PartsInfo)
	{
		if(!m_PartsInfo->has_condition)
		{
			return true;
		}

		switch(m_PartsInfo->type)
		{
		case TYPE__BEAT_NPC:
		{
			if(m_PartsInfo->target_id == m_targetData.m_targetId && m_targetData.m_targetAmount >= m_PartsInfo->target_amount){
				return true;
			}else{
				return false;
			}
		}
			break;
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

void CompanionTaskManager::reset()
{
	m_currentCityId = 0;
	m_currentCityArea = AREA__MAINCITY;
	m_seaX = 0;
	m_seaY = 0;
	m_isTriggering = false;
	//m_hadSendXY = false;
	notifyDialogId = 0;
	m_boughtItems.clear();
	missionFailed = 0;
	//m_targetData.m_targetAmount = 0;
	//m_targetData.m_targetCity = 0;
	//m_targetData.m_targetId = 0;
}

void CompanionTaskManager::notifyUIPartDelayTime(float time)
{
	Director::getInstance()->getScheduler()->schedule(schedule_selector(CompanionTaskManager::notifyUIPartCompleted), this, 0.1f, 0.1f, time, false);
}

void CompanionTaskManager::notifyUIPartCompleted(float t)
{
	Scene*currentScene = Director::getInstance()->getRunningScene();
	if(currentScene){
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(CompanionTaskManager::notifyUIPartCompleted), this);
		auto dialogLayer = UIStoryLine::GetInstance();
		if (!dialogLayer->getParent())
		{
			currentScene->addChild(dialogLayer, 10001);
		}
		dialogLayer->onPartnerDialogMessage(notifyDialogId, 0);
	}
}

void CompanionTaskManager::completeCompanionTask()
{
	if (m_PartsInfo && m_PartsInfo->part == m_currentPartId)
	{
		setIsTriggering(false);
		ProtocolThread::GetInstance()->completeCompanionTask(m_currentTaskId,m_currentPartId,missionFailed);
		missionFailed = 0;
	}
}

void CompanionTaskManager::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	switch (msgType) {

	case PROTO_TYPE_GetSailInfoResult:
	{
		GetSailInfoResult *result = (GetSailInfoResult*) message;
		if (result->failed)
			return;
		if (result->companiontaskinfo && result->companiontaskinfo->n_tasks > 0)
		{
			if (result->companiontaskinfo->tasks[0]->currentpartid == 0)
			{ //task completed
				m_currentTaskId = result->companiontaskinfo->tasks[0]->taskid;
				m_currentPartId = -1;
				m_isCurrentTaskCompleted = true;
				//m_isCurrentTaskFinihed = true;
			}
			else
			{// if (m_currentTaskId != result->maintaskinfo->taskid || m_currentPartId != result->maintaskinfo->completedpartnum + 1) {
				reset();
				m_currentTaskId = result->companiontaskinfo->tasks[0]->taskid;
				m_currentPartId = result->companiontaskinfo->tasks[0]->currentpartid;
				m_PartsInfo = getPartInfo(m_currentTaskId, m_currentPartId);
				if(m_PartsInfo && m_PartsInfo->type == TYPE__BEAT_NPC){
					if(m_targetData.m_targetId != m_PartsInfo->target_id){
						m_targetData.m_targetId = m_PartsInfo->target_id;
						m_targetData.m_targetAmount = result->companiontaskinfo->tasks[0]->beatnpcnum;
					}
				}else{
					m_targetData.m_targetAmount = 0;
					m_targetData.m_targetId = 0;
				}
				m_isCurrentTaskCompleted = false;
				//m_isCurrentTaskFinihed = false;
//				for (int i = 0; i < result->maintaskinfo->n_items; i++)
//				{
//					int m_itemtype, m_itemid, m_itemamount;
//					m_itemtype = result->maintaskinfo->items[i]->taskitemtype;
//					m_itemid = result->maintaskinfo->items[i]->taskitemid;
//					m_itemamount = result->maintaskinfo->items[i]->taskitemamount;
//
//					struct BoughtItem item = { m_itemtype, m_itemid, m_itemamount };
//					m_boughtItems.push_back(item);
//				}
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
		if (result->companiontaskinfo && result->companiontaskinfo->n_tasks > 0)
		{
			if(result->companiontaskinfo->tasks[0]->currentpartid == 0)
			{//task completed
				m_currentTaskId = result->companiontaskinfo->tasks[0]->taskid;
				m_currentPartId = -1;
				m_isCurrentTaskCompleted = true;
				//m_isCurrentTaskFinihed = true;
			}
			else
			{// if (m_currentTaskId != result->maintaskinfo->taskid || m_currentPartId != result->maintaskinfo->completedpartnum+1) {
				reset();
				m_currentTaskId = result->companiontaskinfo->tasks[0]->taskid;
				m_currentPartId = result->companiontaskinfo->tasks[0]->currentpartid;
				m_PartsInfo = getPartInfo(m_currentTaskId, m_currentPartId);
				if(m_PartsInfo && m_PartsInfo->type == TYPE__BEAT_NPC){
					if(m_targetData.m_targetId != m_PartsInfo->target_id){
						m_targetData.m_targetId = m_PartsInfo->target_id;
						m_targetData.m_targetAmount = result->companiontaskinfo->tasks[0]->beatnpcnum;
					}
				}else{
					m_targetData.m_targetAmount = 0;
					m_targetData.m_targetId = 0;
				}
				m_isCurrentTaskCompleted = false;
				//m_isCurrentTaskFinihed = false;

//				for(int i=0;i<result->maintaskinfo->n_items;i++)
//				{
//					int m_itemtype,m_itemid,m_itemamount;
//					m_itemtype = result->maintaskinfo->items[i]->taskitemtype;
//					m_itemid = result->maintaskinfo->items[i]->taskitemid;
//					m_itemamount = result->maintaskinfo->items[i]->taskitemamount;
//
//					struct BoughtItem item = {m_itemtype,m_itemid,m_itemamount};
//					m_boughtItems.push_back(item);
//				}
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
	case PROTO_TYPE_UnlockCompanyResult:
	{
		UnlockCompanyResult*result = (UnlockCompanyResult*)message;
		if(result->failed == 0){
			//ProtocolThread::GetInstance()->getUserTasks();
		}
	}
		break;
	case PROTO_TYPE_GetUserTasksResult:
	{
		GetUserTasksResult *result = (GetUserTasksResult*) message;
		if (result->failed)
			return;

		if (result->n_companionstasks < 1)
		{ //completed,none of your business
			m_currentTaskId = 0;
			m_currentPartId = 0;
			m_isCurrentTaskCompleted = true;
			//m_isCurrentTaskFinihed = true;
		}
		else
		{
			m_currentTaskId = result->companionstasks[0]->taskid;
			m_currentPartId = result->companionstasks[0]->currentpartid;
			m_PartsInfo = getPartInfo(m_currentTaskId, m_currentPartId);
			if(m_PartsInfo && m_PartsInfo->type == TYPE__BEAT_NPC){
				if(m_targetData.m_targetId != m_PartsInfo->target_id){
					m_targetData.m_targetId = m_PartsInfo->target_id;
					m_targetData.m_targetAmount = result->companionstasks[0]->beatnpcnum;
				}
			}else{
				m_targetData.m_targetAmount = 0;
				m_targetData.m_targetId = 0;
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
		if (result->failed == 0)
		{
			//result->reason = 1;
			if(result->fighttype == 1 && m_PartsInfo && m_PartsInfo->has_npc && m_PartsInfo->npc_id == result->npcid/* && m_PartsInfo->has_next_part*/){
				if(result->reason == 1){
					missionFailed = 0;
				}else{
					missionFailed = 1;
				}
			}else if(result->reason == 1){
				if(m_PartsInfo && m_PartsInfo->type == TYPE__BEAT_NPC && result->npcid == m_targetData.m_targetId){
					m_targetData.m_targetAmount ++;
//					log("m_targetAmount = %d", m_targetData.m_targetAmount);
				}
			}
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
	case PROTO_TYPE_CompleteCompanionTaskResult:
	{
		CompleteCompanionTaskResult* result = (CompleteCompanionTaskResult*)message;
		if (result->failed == 0)
		{
			if (m_currentTaskId == 9 && m_currentPartId == 2)
			{
				//终极一战战败回城
				CHANGETO(SCENE_TAG::MAIN_TAG);
			}

			notifyDialogId = 0;
			m_boughtItems.clear();
			m_targetData.m_targetAmount = 0;
			m_targetData.m_targetCity = 0;
			m_targetData.m_targetId = 0;

			m_currentTaskId = result->currenttaskid;
			m_currentPartId = result->currentpartid;
			m_PartsInfo = getPartInfo(m_currentTaskId, m_currentPartId);
			if(m_PartsInfo && m_PartsInfo->type == TYPE__BEAT_NPC){
				if(m_targetData.m_targetId != m_PartsInfo->target_id){
					m_targetData.m_targetId = m_PartsInfo->target_id;
				}
			}else{
				m_targetData.m_targetAmount = 0;
				m_targetData.m_targetId = 0;
			}
			if ((m_currentTaskId == 5 && m_currentPartId == 3) || (m_currentTaskId == 7 && m_currentPartId == 3) 
				|| (m_currentTaskId == 9 && m_currentPartId == 2 )|| (m_currentTaskId == 9 && m_currentPartId == 3))
			{
				notifyDialogId = m_PartsInfo->action;
				CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
			}
			else if ((m_currentTaskId == 8 && m_currentPartId == 1))
			{
				CHANGETO(SCENE_TAG::MAP_TAG);
			}
			else if ((m_currentTaskId == 6 && m_currentPartId == 1) || (m_currentTaskId == 0 && m_currentPartId == 0))
			{
				//必输的战斗回城
				CHANGETO(SCENE_TAG::MAIN_TAG);
			}
		}
		else
		{
			log("complete companion task error !!!");
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

int CompanionTaskManager::checkTaskAction()
{
	if(matchTaskPosition() && matchTaskConidition())
	{
		if(m_PartsInfo)
		{
			notifyDialogId = m_PartsInfo->action;
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




