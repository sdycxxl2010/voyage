#include "TVSeaEventManage.h"
#include "UISailManage.h"
#include "TVSailDefineVar.h"
#include "UISailHUD.h"
#include "Utils.h"
#include "AppHelper.h"

using namespace cocos2d;
TVSeaEventManage::TVSeaEventManage()
	:m_bIsStart(true)
	,m_nUsedDays(1)
	,m_vShipPos(0,0)
	,m_fStorm_flag(10)
	,m_fRock_flag(10)
	,m_fNpcShip_flag(3)
	,n_r(0)
{
	m_nLastPestisDay = 1;
	m_nLastHomesickDay = 1;
	m_nLastSepsisDay = 1;
	m_bIsFloat = false;
	m_flortevts = nullptr;
}
TVSeaEventManage::~TVSeaEventManage()
{
	releaseEvents();
}

TVSeaEventManage* TVSeaEventManage::createManage(Node* map)
{
	TVSeaEventManage* sem = new TVSeaEventManage;
	if (sem)
	{
		sem->autorelease();
		sem->m_pMap = map;
		return sem;
	}
	CC_SAFE_DELETE(sem);
	return nullptr;
}

bool TVSeaEventManage::geneEvents(SeaEventDefine** events,int n_events)
{
	releaseEvents();
	for (int i = 0; i < n_events; i++)
	{
		TVSeaEvent* se = TVSeaEvent::generateEvent(events[i]);
		m_vEvents.push_back(se);
	}
	return true;
}

void TVSeaEventManage::startSeaEvent()
{
	m_bIsStart = true;
}
void TVSeaEventManage::setCurrentDays_pos(int usedDays,cocos2d::Vec2 pos)
{
	m_nUsedDays = usedDays;
	m_vShipPos = pos;
}
void TVSeaEventManage::update(float f)
{
	long a = Utils::gettime();
	if (!m_bIsStart)
	{
		return;
	}
	if (m_fRock_flag >= 0)
	{
		m_fRock_flag -= f;
	}
	if (m_fStorm_flag >= 0)
	{
		m_fStorm_flag -= f;
	}
	if (m_fNpcShip_flag >= 0)
	{
		m_fNpcShip_flag -= f;
	}else{
	
		dynamic_cast<UISailManage*>(m_pMap)->setIfTriggerRange(false);
	}
	if (m_fRock_flag < 0 || m_fStorm_flag < 0 || m_fNpcShip_flag < 0){
	
		UISailHUD* mapui = dynamic_cast<UISailHUD*>(dynamic_cast<UISailManage*>(m_pMap)->getMapUI());
		for (int i = 0; i < m_vEvents.size(); i++)
		{
			if (isHappenEvent(m_vEvents[i]))
			{				
				switch (m_vEvents[i]->m_Type)
				{
				case EV_SEA_FLOAT:
				break;
				case EV_SEA_PESTIS:
				{
						//当补给小于0时，不发生鼠灾
                       if (mapui->m_nCurSupplies<=0)
                       {
						   break;
                       }
					   else
					   {
						   	m_bIsStart = false;
						   	dynamic_cast<UISailManage*>(m_pMap)->showSeaEvent(m_vEvents[i]);
					   }
					   break;
				}
				default:
				m_bIsStart = false;
				dynamic_cast<UISailManage*>(m_pMap)->showSeaEvent(m_vEvents[i]);
				break;
				}
				return;
			}
		}
	}
	long b = Utils::gettime();
	if (AppHelper::frameTime > 0.045)
	{
		log("TVSeaEventManage:%d, frameTime:%f, frameCount:%d", b - a, AppHelper::frameTime, AppHelper::frameCount);
	}
}

bool TVSeaEventManage::isHappenEvent(TVSeaEvent* evts)
{
#ifdef WIN32

	evts->m_Days = 5;
#endif // WIN32
	switch (evts->m_Type)
	{
	case EV_SEA_NPC:
		{
			float disTanceSeq = m_vShipPos.getDistanceSq(Vec2(evts->m_X,MAP_CELL_HEIGH * MAP_WIDTH-evts->m_Y));
			if(!evts->shouldTriggerBattle() || evts->m_Event_id == dynamic_cast<UISailManage*>(m_pMap)->getLastNPCEventId()){
				//log("distance:%f",disTanceSeq);
				if(disTanceSeq<40*40 && m_fNpcShip_flag <= 0){
					m_fNpcShip_flag = 3.0f;
					setseaEventHalfRange(1);
					return true;
				}
			}else if (disTanceSeq <= std::pow(evts->m_Range * evts->m_NPC->n, 2) && m_fNpcShip_flag <= 0/* && evts->m_IsCan*/)
			{
				int randomValue = cocos2d::random(0,100);
				if (randomValue <= evts->m_Rate)
				{
					m_fNpcShip_flag = 3.0f;
					setseaEventHalfRange(1);
					return true;
				}

			}
			break;
		}
	case EV_SEA_STORM:
		{
			float disTanceSeq = m_vShipPos.getDistanceSq(Vec2(evts->m_X,MAP_CELL_HEIGH * MAP_WIDTH-evts->m_Y));
			if (disTanceSeq <= std::pow(evts->m_Range,2) && m_fStorm_flag <= 0)
			{
				int randomValue = cocos2d::random(0,100);
				if (randomValue <= evts->m_Rate)
				{
					m_fStorm_flag = 10.f;
					return true;
				}
			}
			break;
		}
	case EV_SEA_FLOAT:

		{
				float disTanceSeq = m_vShipPos.getDistanceSq(Vec2(evts->m_X, MAP_CELL_HEIGH * MAP_WIDTH - evts->m_Y));
				auto range = evts->m_Range + dynamic_cast<UISailManage*>(m_pMap)->m_captain_skill_outlookLv;
				if (disTanceSeq <= std::pow(range, 2) && m_fNpcShip_flag <= 0)
				//if (disTanceSeq <= std::pow(150, 2) && m_fNpcShip_flag <= 0)
					{
						int randomValue = cocos2d::random(0, 100);
						if (randomValue <= evts->m_Rate)
							{
								m_fNpcShip_flag = 0.0f;
								m_bIsFloat = true;
								m_flortevts = evts;
								return true;
							 }
					}
				else
				{
					m_bIsFloat = false;
					m_flortevts = nullptr;
				}
				break;
		}

	case EV_SEA_ROCK:
		{
			float disTanceSeq = m_vShipPos.getDistanceSq(Vec2(evts->m_X,MAP_CELL_HEIGH * MAP_WIDTH-evts->m_Y));
			if (disTanceSeq <= std::pow(evts->m_Range,2))
			{
				int randomValue = cocos2d::random(0,100);
				if (randomValue <= evts->m_Rate && m_fRock_flag <= 0)
				{
					m_fRock_flag = 10.f;
					return true;
				}
				else if (m_fRock_flag <=0)
				{
					m_fRock_flag = 10.f;
				}
			}
			break;
		}
	case EV_SEA_BIG_FOG:
		break;
	case EV_SEA_BOSS:
		break;
	case EV_SEA_PESTIS:
		{
			//return false;
			if (evts->m_Days>0 && m_nLastPestisDay < m_nUsedDays && m_nUsedDays % evts->m_Days == 0)
			{
				m_nLastPestisDay = m_nUsedDays;
				int randomValue = cocos2d::random(0,100);
				if (randomValue < evts->m_Rate)
				{

					m_nLastPestisDay = m_nUsedDays;
					m_nLastHomesickDay = m_nUsedDays;
					m_nLastSepsisDay = m_nUsedDays;
					return true;
				}
			}
			break;
		}
	case EV_SEA_HOMESICK:
		{
			//return false;
			if (evts->m_Days > 0 && m_nLastHomesickDay < m_nUsedDays && m_nUsedDays % evts->m_Days == 0)
			{
				m_nLastHomesickDay = m_nUsedDays;
				int randomValue = cocos2d::random(0,100);
				if (randomValue < evts->m_Rate)
				{
					m_nLastPestisDay = m_nUsedDays;
					m_nLastHomesickDay = m_nUsedDays;
					m_nLastSepsisDay = m_nUsedDays;
					return true;
				}
			}
			break;
		}
	case EV_SEA_SEPSIS:
		{
			//return false;
			if (evts->m_Days>0 && m_nLastSepsisDay < m_nUsedDays && m_nUsedDays % evts->m_Days == 0)
			{
				m_nLastSepsisDay = m_nUsedDays;
				int randomValue = cocos2d::random(0,100);
				if (randomValue < evts->m_Rate)
				{
					m_nLastPestisDay = m_nUsedDays;
					m_nLastHomesickDay = m_nUsedDays;
					m_nLastSepsisDay = m_nUsedDays;
					return true;
				}
			}
			break;
		}
	default:
		break;
	}

	return false;
}
void TVSeaEventManage::releaseEvents()
{
	for (int i = 0; i < m_vEvents.size(); i++)
	{
		delete m_vEvents[i];
	}
	m_vEvents.clear();
}
