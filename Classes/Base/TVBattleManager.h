/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月01日
 *  Author:Sen
 *  battle manage （战斗的管理）
 *
 */

#ifndef __BATTLE__MANAGE__
#define __BATTLE__MANAGE__

#include "TVBasic.h"
#include "UIBattleHUD.h"
#include "TVBattleDelegate.h"
#include "EffectManage.h"
#include "TVBasicShip.h"

USING_NS_CC;

class TVBattleManager : public UIBasicLayer,public TVBattleDelegate
{
public:
	//
	struct Battle_A_B
	{
		int _CD;
		Node *_A_obj;
		Node *_B_obj;
		Layer *_animation;
		Sprite *_fonts;
		Sprite *_sprite_bg_red;
		Sprite *font1;

	};

	class ActiveSkill
	{
	public:
		ActiveSkill()
		{
			skillId = 0;
			skillLevel = 0;
			definedCdSeconds = 0;
			definedDurationSeconds = 0;
			CDRemainSeconds = 0;
			consumedSeconds = 0;
			status = 0;
		};
		int skillId;
		int skillLevel;
		int definedCdSeconds;
		int definedDurationSeconds;
		int CDRemainSeconds;
		int consumedSeconds;
		int status; //0,ready 1, fired, 2 CD-ing
	};

public:
	TVBattleManager();
	~TVBattleManager();

	static TVBattleManager* createBattle(EngageInFightResult *result,Vec2 pos);
	void onEnter() override;
	void onExit() override;
	bool init();
	void loadStaticData(float dt);//双方船的信息初始化,加入主界面
	void loadBackGround();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);//默认button相应


	//近战船只特效
	Sprite *createfont1(Node *node1,Node *node2,int num1,int num2);  //水手数字
	Layer *createclosefightflash(Node *node1,Node *node2);  //近战战斗的特效 
	void  numofsailor(Node *node1,Node *node2);			  //两个近战船只剩余的水手数
	Sprite* createFonts(Node *node1,Node *node2,int num1,int num2); //减少的水手数字
	void deleteHarmNumsA(Node *node1,Node *node2);		  //删除水手数字
	Sprite* createsp_bg_red(Node *node1,Node *node2);     //背景红的特效
	void fontAnimation(Node* label,Node* node1,Node* node2);//水手数字 
	void sprite_bg_redAnimation(Node *sp);      //背景红的特效
	
	/*
	*延迟设置近战结束
	*/
	void delaySetCloseBattle(TVBasicShip*node1, TVBasicShip*node2);

	//BattleDelegate
	void specialButtonEvent(cocos2d::Node* target,std::string name,float varValue) override;
	void callbackEventByIndex(int tag) override;
	TVBattleShareObj* getShareObj() override { return m_ShareObj; } ;
	EffectManage* getEffective() { return m_EffManage;};
	SkillManage* getSkillManage() { return m_SkillManage;}
	UIBattleHUD* getBattleUI() {return m_UIlayer;}
	CMap* getMap(){ return m_Cmap;};
	
	void usedSkills(int tag);	//使用技能
	void usedProps(int tag);	//使用道具
	void changeShipInfo(int skill_id, int tag = 1);  //更改船只的数据
	void update(float df) override;
	void scaleEnd();				//进入战斗后，画面由远及近 开始战斗
	void startBattle(float dt);		//开始战斗
	void stopBattle();				//结束战斗
	void checkBattle_A_B();			//船只碰撞

	bool onTouchBegan(Touch *touch, Event *unused_event) override;
	void onTouchMoved(Touch *touch, Event *unused_event) override;
	void onTouchEnded(Touch *touch, Event *unused_event) override;
	void onTouchCancelled(Touch *touch, Event *unused_event) override;

	//船只碰撞
	bool onContactBegain(PhysicsContact& contact);
	void onContactPostSolve(PhysicsContact& contact,const PhysicsContactPostSolve& solve);
	bool onContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve);
	void onContactSeperate(PhysicsContact& contact);
	
	void soloResult(Node * objA,Node * objB);

	void removeSpecEffect(float dt);

	Vec2 getShipPos() { return m_ShipMapPos; }
	EngageInFightResult *getFightResult(){ return m_FightResult; }

	bool getIsPause() { return !m_StartGame; }
	//船体牵引技能 参数shipId:可能要捕获的船只的id
	bool isCapturingShip(int shipId);
	//食物掠夺技能 参数shipId:可能要捕获的船只的id
	void foodPlunderAndCapturingShip(TVBasicShip *ob_A, TVBasicShip *ob_B);
	UIBattleHUD* getBattleUILayer(){ return m_UIlayer;};
	/*
	*海上战斗近战时单挑结果
	*return 1:己方胜利 2:敌方胜利 3:平局(继续白刃战) 0:没有处罚单挑
	*/
	int singledResult(TVBasicShip *ob_A, TVBasicShip *ob_B);
	/*
	*上次战斗是否结束了
	*/
	bool getContinueLastFight(){ return m_bContinueLastFight; }

	//有时候需要保存这个指针给下一场战斗，reset避免被free
	void resetEngageInFightData(){m_FightResult=nullptr;};
private:
	ActiveSkill**mActivieSkills;
	int mActivieSkillsNum;
	Vec2 m_ShipMapPos;
	CMap	*m_Cmap;
	Layout	*m_MapLayer;
	UIBattleHUD* m_UIlayer;
	TVBattleShareObj* m_ShareObj;
	SkillManage* m_SkillManage;
	EffectManage* m_EffManage;
	EngageInFightResult* m_FightResult;
	EventListenerPhysicsContact* _contactEvent;
	bool m_StartGame;
	std::vector<Battle_A_B> m_fightingObjs;
	float m_SkillFlushTime;

	Point m_cameraStartPostion;
	int  m_skillEffectTime[20];
	Camera *UICamera;
	int  m_atkbuffnum;//攻击buff数量
	Sprite * spriteatkEff;//蓄力特效
	/*
	*是否继续上次战斗
	*/
	bool	m_bContinueLastFight;
};

#endif
