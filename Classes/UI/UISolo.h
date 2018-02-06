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
		//��������
		STANDBY,
		//��·
		WALK,
		//����
		ATTACK,
		//����
		DAMAGED,
		//����
		DODGE,
		//��
		PARRYING,
		//����1
		DEATH_1,
		//����2
		DEATH_2,
		//����
		ULTIMATE,
	};
	enum ATTACK_INDEX
	{
//�з�������
		ENEMY_DAMAGED,
		ENEMY_DODGE,
		ENEMY_PARRYING,
		ENEMY_DEATH,
		ENEMY_BEATEN,
//��ұ�����
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
	//��ʼ����������
	void initSoloView();
	//������ʼ����
	void battleBegin();
	//�򶷶���
	void battleAnimation(int index,int aniIndex);
	//��ײ�Ĵ�ֻ
	void setBattleShip(Node * shipA,Node * shipB);
	//�����ص�
	void animationStateEvent();
	//���˳���
	void hurt(Node * psender,int index);
	//��ʼ�������б�
	void initAnimations();
	//��
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
	//key ����index
	std::map<int, std::vector<int>> m_actions;
	ROLE_INFO playerInfo;
	ROLE_INFO enemyInfo;
	int  m_battleIndex;
	int  m_battleComplete;
	
};
#endif