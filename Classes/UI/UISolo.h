#ifndef __SOLOLATER_H
#define __SOLOLATER_H
#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>
#include "spine/spine.h"
#include "UIBasicLayer.h"
class UISolo :public UIBasicLayer
{
	enum ANI_INDEX
	{
		//待机动画
		STANDBY,
		//走路
		WALK,
		//攻击
		ATTACK,
		//受伤
		DAMAGED,
		//闪避
		DODGE,
		//格挡
		PARRYING,
		//死亡1
		DEATH_1,
		//死亡2
		DEATH_2,
		//大招
		ULTIMATE,
	};
	enum ATTACK_INDEX
	{
//敌方被攻击
		ENEMY_DAMAGED,
		ENEMY_DODGE,
		ENEMY_PARRYING,
		ENEMY_DEATH,
		ENEMY_BEATEN,
//玩家被攻击
		PLAYER_DAMAGED,
		PLAYER_DODGE,
		PLAYER_PARRYING,
		PLAYER_DEATH,
		PLAYER_BEATEN,
	};
	struct ROLE_INFO
	{
		int hp;
		int defence;
		int attack;
	};
public:
	UISolo();
	~UISolo();
	bool init();
	static UISolo * createLayer();
	static Scene * createScene();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	//初始化单挑界面
	void initSoloView();
	//单挑开始动画
	void battleBegin();
	//打斗动画
	void battleAnimation(int index,int aniIndex);
	//碰撞的船只
	void setBattleShip(Node * shipA,Node * shipB);
	//动画回调
	void animationStateEvent();
	//受伤出字
	void hurt(Node * psender,int index);
	//初始化动作列表
	void initAnimations();
	//打斗
	void battle(float dt);
	void skeleEndListener(int trackIndex, int loopCount);
	void skeleStartListener(int trackIndex);
	void battleWin();
protected:
	spine::SkeletonAnimation * m_player;
	spine::SkeletonAnimation * m_enemy;
	Node * m_shipA;
	Node * m_shipB;
	bool m_playerAttacted;
	int m_battleResult;
	int m_touchTimes;
	//key 动画index
	std::map<int, std::vector<int>> m_actions;
	ROLE_INFO playerInfo;
	ROLE_INFO enemyInfo;
	int  m_battleIndex;
	int  m_battleComplete;
	
};
#endif