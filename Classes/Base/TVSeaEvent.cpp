#include "TVSeaEvent.h"
#include "TVSingle.h"
#include "EnumList.h"

TVSeaEvent::TVSeaEvent()
	//:m_isCan(false)
{
	m_Event_id = -1;
	m_Days = 0;
	m_Range = 0;
	m_Type = -1;
	m_X = 0;
	m_Y = 0;
	m_Rate = 0;
	m_NPC = nullptr;
	m_Wanted = false;
	m_IsFriend = false;
	m_NPCLevel = 0;
}
TVSeaEvent::~TVSeaEvent()
{

}

TVSeaEvent* TVSeaEvent::generateEvent(SeaEventDefine* eventSea)
{
	TVSeaEvent* se = new TVSeaEvent;
	if (se && se->initEvent(eventSea))
	{
		return se;
	}
	delete se;
	return nullptr;
}

bool TVSeaEvent::shouldTriggerBattle(){
	bool res = false;
	if(m_Wanted || (!SINGLE_HERO->m_bInSafeArea  && !m_IsFriend)){
		if(m_NPCLevel > SINGLE_HERO->m_iLevel - 20){
			res = true;
		}
	}
	return res;
}

bool TVSeaEvent::initEvent(SeaEventDefine* eventSea)
{
	bool pRet = false;
	do 
	{
		switch (eventSea->type)
		{
		case EV_SEA_NPC:
			{
				m_Event_id = eventSea->eventid;
				m_Type = eventSea->type;
				m_X = eventSea->x;
				m_Y = eventSea->y;
				m_Range = eventSea->r;
				m_Rate = eventSea->rate;
				int npc_type = SINGLE_SHOP->getBattleNpcInfo()[eventSea->npcid].type;

				for (int i=0;i<SINGLE_HERO->m_iNwantedforceids;i++)
				{
					if (eventSea->npcforceid==SINGLE_HERO->m_iWantedforceids[i])
					{
						//npc_type = NPC_TYPE_PIRATE;
						//eventSea->isfriendnpc = 0;
						m_Wanted = true;
						break;
					}			
				}
				m_IsFriend = eventSea->isfriendnpc;
				m_NPCLevel = eventSea->npclevel;
				if (npc_type == NPC_TYPE_CARAVAN)//商队船只不会主动攻击
				{
					m_IsFriend = true;
					m_Wanted = false;
				}
//				if (eventSea->isfriendnpc || npc_type == NPC_TYPE_CARAVAN)
//				{
//					m_isCan = false;
//				}else
//				{
//					if (eventSea->npclevel >= (SINGLE_HERO->m_iLevel - 10)){
//						m_isCan = true;
//					}else if ((eventSea->npclevel <= (SINGLE_HERO->m_iLevel-10)) && (eventSea->npclevel >= (SINGLE_HERO->m_iLevel - 20))){
//						int rate = 100 - (SINGLE_HERO->m_iLevel-eventSea->npclevel-10)*10;
//						int randomValue = cocos2d::random(0,100);
//						if (randomValue < rate)
//						{
//							m_isCan = true;
//						}else
//						{
//							m_isCan = false;
//						}
//					}else if (eventSea->npclevel <= (SINGLE_HERO->m_iLevel - 20)){
//						m_isCan = false;
//					}
//				}
				break;
			}
		case EV_SEA_STORM:
		case EV_SEA_FLOAT:
		case EV_SEA_ROCK:
			{
				m_Event_id = eventSea->eventid;
				m_Type = eventSea->type;
				m_X = eventSea->x;
				m_Y = eventSea->y;
				m_Range = eventSea->r;
				m_Rate = eventSea->rate;				
				break;
			}
		case EV_SEA_BIG_FOG:
			break;
		case EV_SEA_BOSS:
			break;
		case EV_SEA_PESTIS:
		case EV_SEA_HOMESICK:
		case EV_SEA_SEPSIS:
			{
				m_Event_id = eventSea->eventid;
				m_Type = eventSea->type;
				m_Days = eventSea->days;
				m_Range = eventSea->r;
				m_Rate = eventSea->rate;
				break;
			}
		default:
			break;
		}
		pRet = true;

	} while (0);

	return pRet;
}
