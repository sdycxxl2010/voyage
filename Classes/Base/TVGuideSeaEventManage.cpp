#include "TVGuideSeaEventManage.h"
#include "UIGuideSailManage.h"
#include "TVSailDefineVar.h"

using namespace cocos2d;
TVGuideSeaEventManage::TVGuideSeaEventManage()
:m_bIsStart(true)
, m_nUsedDays(1)
, m_vShipPos(0, 0)
, m_fNpcShip_flag(3)
, n_r(0)
{
}
TVGuideSeaEventManage::~TVGuideSeaEventManage()
{
	releaseEvents();
}

TVGuideSeaEventManage* TVGuideSeaEventManage::createManage(Node* map)
{
	TVGuideSeaEventManage* sem = new TVGuideSeaEventManage;
	if (sem && sem->init())
	{
		sem->autorelease();
		sem->m_pMap = map;
		return sem;
	}
	CC_SAFE_DELETE(sem);
	return nullptr;
}
bool TVGuideSeaEventManage::init()
{
	Node::init();
	bool pRet = false;
	do
	{
		pRet = true;
	} while (0);
	return pRet;
}
bool TVGuideSeaEventManage::geneEvents(SeaEventDefine** events, int n_events)
{
	releaseEvents();
	for (int i = 0; i < n_events; i++)
	{
		TVSeaEvent* se = TVSeaEvent::generateEvent(events[i]);
		m_vEvents.push_back(se);
	}
	return true;
}

void TVGuideSeaEventManage::startSeaEvent()
{
	m_bIsStart = true;
}
void TVGuideSeaEventManage::setCurrentDays_pos(int usedDays, cocos2d::Vec2 pos)
{
	m_nUsedDays = usedDays;
	m_vShipPos = pos;
}
void TVGuideSeaEventManage::update(float f)
{
	if (!m_bIsStart)
	{
		return;
	}
	if (m_fNpcShip_flag >= 0)
	{
		m_fNpcShip_flag -= f;
	}
	else{

		//dynamic_cast<MapsGuide*>(m_pMap)->m_ischufa = false;
	}

	if ( m_fNpcShip_flag < 0){

		for (int i = 0; i < m_vEvents.size(); i++)
		{
			if (isHappenEvent(m_vEvents[i]))
			{
				m_bIsStart = false;
				auto typeEvent = m_vEvents[i]->m_Type;
				dynamic_cast<UIGuideSailManage*>(m_pMap)->showSeaEvent(m_vEvents[i]);
				return;
			}
		}
	}

}

bool TVGuideSeaEventManage::isHappenEvent(TVSeaEvent* evts)
{
#ifdef WIN32

	evts->m_Days = 5;
#endif // WIN32
	switch (evts->m_Type)
	{
	case EV_SEA_NPC:
	{
			//不同的国家，海盗船坐标不同
			 float disTanceSeq = 99999.0;
			 Vec2 enmeyPos = dynamic_cast<UIGuideSailManage*>(m_pMap)->getGuideEnemyShipPos();
			 disTanceSeq = m_vShipPos.getDistanceSq(enmeyPos);
			evts->m_Range = 50;
			if (disTanceSeq <= std::pow(evts->m_Range * n_r, 2) && m_fNpcShip_flag <= 0 )
			{
				int randomValue = cocos2d::random(0, 100);
				if (randomValue <= evts->m_Rate)
				{
					m_fNpcShip_flag = 3.0f;
					setGuideSeaEventHalfRange(1);
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
void TVGuideSeaEventManage::releaseEvents()
{
	for (int i = 0; i < m_vEvents.size(); i++)
	{
		delete m_vEvents[i];
	}
	m_vEvents.clear();
}
