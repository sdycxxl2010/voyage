#include "TVBattleShareObj.h"

TVBattleShareObj::TVBattleShareObj()
{
	m_nFlagShipAttackShipTag_Enemy = 0;
	m_nLastAttackFlagShipTag_Enemy = 0;
	m_nFlagShipAttackShipTag = 0;
	m_nLastAttackFlagshipTag = 0;
	m_nLockShipTag = -1;

//	m_nMorale = (int)(MAX_MORALE * 0.8);
//	m_nMorale_Enemy = (int)(MAX_MORALE * 0.8);
}

TVBattleShareObj::~TVBattleShareObj()
{
	m_vEnemys.clear();
	m_vDiedShips.clear();
	m_vMyFriends.clear();
}

void TVBattleShareObj::addSelfMorale(const int value)
{
	int addValue = 0;
	if (value > 0) 
	{
		addValue = 10;
	} else if (value < 0) 
	{
		addValue = -10;
	}else
	{
		addValue = 50;
	}
	addMoraleCore(addValue);
}

void TVBattleShareObj::addMoraleByKill(const int isFlagShip) 
{
	if (isFlagShip) 
	{
		addMoraleCore(-500);
	} else 
	{
		addMoraleCore(-300);
	}
}

void TVBattleShareObj::addMoraleCore(const int value) 
{
	m_nMorale += value;
	if(m_nMorale > MAX_MORALE)
	{
		m_nMorale = MAX_MORALE;
	}else if ( m_nMorale < 0) 
	{
		m_nMorale = 0;
	}
}

void TVBattleShareObj::addEnemyMorale(const int value) 
{
	int addValue = 0;
	if (value > 0) 
	{
		addValue = 10;
	} else if (value < 0) 
	{
		addValue = -10;
	}
	addEnemyMoraleCore(addValue);
}

void TVBattleShareObj::addEnemyMoraleByKill(const int isFlagShip)
{
	if (isFlagShip) 
	{
		addEnemyMoraleCore(-300); //enemy no flagship
	} else
	{
		addEnemyMoraleCore(-100);
	}
}

void TVBattleShareObj::addEnemyMoraleCore(const int value)
{
	m_nMorale_Enemy += value;
	if (m_nMorale_Enemy > MAX_MORALE)
	{
		m_nMorale_Enemy = MAX_MORALE;
	}else if (m_nMorale_Enemy < 0)
	{
		m_nMorale_Enemy = 0;
	}
}
