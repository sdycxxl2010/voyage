/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月30日
 *  Author:Sen
 *  battle share
 *
 */

#ifndef __BATTLE__SHAREOBJ__H__
#define __BATTLE__SHAREOBJ__H__

#include "cocos2d.h"
#define MAX_MORALE 5000

class TVBasicShip;
class TVBattleShareObj
{
public:
	TVBattleShareObj();
	~TVBattleShareObj();

	/*
	 * Add Morale when get attacked.
	 * value: the value of damage
	 */
	void addSelfMorale(const int value);
	/*
	 * Add Morale when kill.
	 * isFlagShip: flag ship(!0）
	 */
	void addMoraleByKill(const int isFlagShip);
	void addMoraleCore(const int value);

	/*
	 * Add Enemy Morale when get attacked.
	 * value: the value of damage
	 */
	void addEnemyMorale(const int value);
	/*
	 * Add Enemy Morale when kill.
	 * isFlagShip: flag ship (!0）
	 */
	void addEnemyMoraleByKill(const int isFlagShip);
	void addEnemyMoraleCore(const int value);

	int m_nMorale;
	int m_nMorale_Enemy;

	std::vector<TVBasicShip*> m_vEnemys;
	std::vector<TVBasicShip*> m_vMyFriends;
	std::vector<TVBasicShip*> m_vDiedShips;

	int m_nLastAttackFlagshipTag;
	int m_nFlagShipAttackShipTag;
	int m_nLockShipTag;
	int m_nLastAttackFlagShipTag_Enemy;
	int m_nFlagShipAttackShipTag_Enemy;
};

#endif
