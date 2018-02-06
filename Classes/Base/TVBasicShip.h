/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月30日
 *  Author:Sen
 *  basic ship 
 *
 */

#ifndef __BASIC__SHIP__
#define __BASIC__SHIP__

#include "TVBasic.h"
#include "TVBattleDelegate.h"
#include "TVBattleShareObj.h"
#include "MyDrawLine.h"
#include "login.pb-c.h"
#include "TVBattleSkill.h"

using namespace ui;

#define SHIP_LOADING  1		//船只炮弹cd中
#define SHIP_READY  2		//船只炮弹装备
#define SHIP_FIRED 3		//船只开火
class TVBattleManager;

class TVBasicShip : public Node,public TVBattleDelegate
{
public:
	TVBasicShip();
	virtual ~TVBasicShip();

	/* node class */
	virtual void onEnter() override;
	virtual void onExit() override;
	void initShipInfo(FightShipInfo* sfi,TVBattleManager*manager); //初始化数据
	bool init();   //初始化界面
	
	/* Battleship delegate */
	virtual void startBattle(); //开始战斗
	virtual void stopBattle();	//停止战斗
	void setDelegate(TVBattleDelegate *pDelegate);

	Sprite* getShipImage()   //获取船只
	{
		return m_pShipIcon;
	};

	virtual void findTargetAgain(){};
	virtual void clearHateByTag(int tag){};

	//炮弹的cd时间
	inline void setCDTime(float sec) 
	{
		m_fCDTime = sec;
	};
	inline float getCDTime()
	{
		return m_fCDTime;
	};
	//船只的类型
	inline void setType(SHIP_TYPE type)
	{
		m_eType = type; 
	};	
	inline SHIP_TYPE getType() 
	{
		return m_eType;
	};
	//船只的战斗AI模式（三种：自由攻击、保护旗舰、集中火力）
	inline void setMode(SHIP_ACTION_MODEL mode) 
	{
		m_eMode = mode;
	};
	inline SHIP_ACTION_MODEL getMode() 
	{
		return m_eMode;
	};
	//是否死亡
	inline bool isDied() 
	{
		return m_bIsDied;
	};

	// s = 1 small ship,s = 2 midle ship,s = 3 large ship
	inline int setShipSize(int s)
	{
		m_Info->_type = s;
	}; 

	float getCurrentSpeed();

	//获取的是当前船只正常时的最大速度
	virtual float getShipMoveSpeed()
	{
		return m_fMoveSpeed;
	};
	//获取的是当前船只变化速度(技能影响)
	virtual void setShipTempSpeed(float num)
	{
		m_fTempSpeed += num;
	};

	//更新船只的水手数
	void updataSailorLabel() 
	{
		m_pSailorLabel->setString(StringUtils::format("%d",m_Info->_sailornum));
	};
	//获取距离
	int	getCurLanunchDistance() 
	{
		return m_CurLaunchDistance; 
	};
	//方向
	void setDirect(float angle);
	float getDirect()
	{
		return m_currentDirect;
	};
	//血量
	void setBloodIconValue(float blood);
	float getBlood()
	{ 
		return m_fBlood; 
	};
	//设置血量
	void setBlood(float hp){ m_fBlood = hp; };
	float getMaxHp()
	{
		return m_fMaxHp;
	}
	//近战是锁定目标倒计时
	void setUnlockTime(int unlockTime)
	{
		m_nUnlockTime = unlockTime;
	}
	int getUnlockTime()
	{
		return m_nUnlockTime;
	}

	bool hitTest(const Vec2 &pt);    //炮弹是否击中船只
	virtual void firedByDistance(TVBasicShip* target,int distances); //开火
	virtual void fired(TVBasicShip* target);   //开火
	virtual void hurt(Node* bullt);			//炮弹击中船只伤害计算
	//技能造成伤害计算
	virtual void hurt(int attack, int myId);
	virtual void moveToTaraget(Vec2 targetPos);  //移动到目标船只处
	
	std::vector<TVBasicShip*>* getEnemyShips(int* attFlagship);
	std::vector<TVBasicShip*>* getFriendShips();
	Point (&getHitTestPoints())[4];
	Point (&getHitTestPointsNineScreen())[4];

	virtual void deleteBuffer(int type);				//删除buffer
	virtual void deleteBuffer(SkillData& skilldata);
	virtual void addBuffer(int type);					//增加buffer
	virtual void addBuffer(SkillData& skilldata);
	void	operatorSkill(int index);
	void	generateSkills(SkillDefine** kills,int n_skills);
	int		getBufferState();
	void	bufferStateDisponse(int bufferState);
	void	fearState_Buffer();
	void	disorderState_Buffer();

	void    fireState_Buffer();
	void    slowSpeedState_Buffer();


	void playCaptureEffect();
	///close batttle 
	bool isCloseBattle()
	{ 
		return m_BattleState;
	};
	void setCloseBattleState(bool battleState) 
	{
		m_BattleState = battleState;
	};

	virtual void addLogAction(int type,float arg_1 = 0,int arg_2 = 0,Vec2 pos = Vec2(0,0)){}; //log
	

	void    setShortAttackAddition(int add_per);
	float	getShortAttackAddition();
	float	getShortDefenseAddition();
	int		getMoralAddition(int curMoral);   //士气
	int		getRangeAttackFactor();		//额外攻击
	
	std::vector<int> m_vBuffer;
	Ship_info* m_Info;
	
	void setPassiveSkills(FightShipInfo* sfi);
	/*
	*被动技能过程作用时技能特效
	*/
	void addPassiveSkillsToShip(Ship_info* bulletInfo);
	void generatePassiveSkill(int id,int level); //no


	SkillDefine* getPassiveSkillsById(int id);//获取当前船上的技能 返回值可能为null


	void checkExpediteSpeed(float delt);  //加速
	void checkShipDied();		//检测船只死亡
	void correctPysics();		//修正船只位置
	void correctWave();			//修正浪花位置
	void setCurLaunch();		//设置距离
	virtual void shipDied();
	virtual void moveAndRotation(Vec2 targetPos) {};
	virtual void addTouchEvent(float f);
	virtual void update(float f);
	
	void fireCallback(bool isLastOne=false);  //发射炮弹
	void normalDriect();

	virtual void adjustAngleFinish(){ m_PrepareFire_OK = true;};

	void repairShip(); //使用的主动关于修理的技能
	void updataProgressiveRepair(); //缓慢修改每秒调用
	void repairShipEffice(int num); //恢复的数字
	
	void fireeffective(int bulletNum = 0);//开炮特效
	CC_SYNTHESIZE(bool, isFired, fireComplete);
	//隐藏船只和血条（沉船效果时使用）
	void  setShipAndBooldIconVisible();

	inline int getMaxSoliderNum(){ return m_pResult->maxsailornum; }

	//获得被动技能的个数
	int getPassiveSkillsNum(){ return m_PassiveSkillsNum; }
	//被动技能数据
	SkillDefine **getPassiveSkills() { return mPassiveSkills; }
	//船只单挑的信息
	BattleCaptainInfo *getBattleCaptainInfo() { return m_pResult->battle_info; };
	FightShipInfo * getBattleShipInfo(){ return m_pResult; };
protected: 
	//船只点击事件
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	virtual void onTouchCancelled(Touch *touch, Event *unused_event){};
	virtual void onTouchMoved(Touch *touch, Event *unused_event){};

	size_t m_PassiveSkillsNum;
	SkillDefine **mPassiveSkills; //passive skills

	TVBattleDelegate	*m_pDelegate;		//战斗的代理
	MyDrawLine		*m_pMyMainPath;  //旗舰的路线
	std::vector<RoadNode> m_vRoad; // road path vector
	
	CMap		*map;
	Vector<Node*> m_BulltIndex;
	std::vector<int> m_Cannons;

	/* 1 = ship_load_not_full,2 = ship_load_full,3 = ship_bullet_have_launch */
	int m_ShipStatus; 
	
	bool m_PrepareFire_OK; 
	bool m_BattleState;	  // true is Battling
	bool m_RepairEnable;  
	int m_RangedAttack_per;
	int m_ShortdAttack_per;
	int m_ShortDefense_per;
	int m_moral_per;

	//距离（远中近）
	int m_LaunchDistance_max;
	int m_LaunchDistance_min;
	int m_LaunchDistance_mid;
	int m_CurLaunchDistance;  //距离
	
	//攻击（远中近）
	int m_Launch_max;
	int m_Launch_mid;
	int m_Launch_min;
	
	float m_currentDirect;
	int m_PreTime;
	ListView* m_BufferList;
	TVBasicShip* m_BufferTarget;

	FightShipInfo *m_pResult;
	EventListener *m_pTouchListener;

	Point mHitTestPoints[4];
	Point mHitTestPointsNineScreen[4];

	float m_SkillFlushTime;
	//skill_id 3,4
	bool m_isSlowSpeed; //是否减速
	bool m_isOnFired;   //是否燃烧
	int m_slowSpeedTime; //减速的时间
	int m_onFiredTime;  //燃烧的时间
	int m_firdHarm;

	int m_Indomitable_Will_prob; //skill id 11
	int m_Roar_Victory_prob;     //skill id 12

	Sprite			*m_pCDTimeBg;  //火炮CD bg
	Sprite			*m_pShipIcon;  //船只顶视图
	ProgressTimer	*m_pBloodIcon;  //船只血条
	ProgressTimer	*m_pFlagIcon;   //船只火炮CD
	ParticleSystemQuad *m_pShipWave; //船只下方浪花
	Text			*m_pSailorLabel; //船只水手个数

	/*
	*火炮的cd基数
	*/
	float               m_fCDTimeBasis;
	float				m_fCDTime;  //CD fire time
	SHIP_TYPE			m_eType;		//船只类型
	SHIP_ACTION_MODEL	m_eMode;		//AI模式
	bool				m_bIsDied;      //阵亡
	//最大速度
	float				m_fMoveSpeed;
	//是否正在转向
	bool				m_bIsTurning;
	//转向限速，线速度，根据船只的最大速度和最大转向速度来计算，最大速度越快，惯性越大
	float				m_fLimitMoveSpeed;
	//最大转向速度,线速度
	float				m_fRotationSpeed;
	//每次路径计算，船只转的角度
	float				m_fRoateAngle;
	//最大血量
	float				m_fMaxHp;
	//当前血量
	float				m_fBlood;

	//当前的船只速度(一直没有变？？？)
	float m_fCurSpeed;
	//速度因子防止严重掉帧，由于引擎底层的计算方法（skip frame），计算帧率应该使用固定的60。
	float m_speedFactor;
	//0:匀速 1:加速 2:减速 （加速）
	int m_ExpediteSpeed_flag;
	//下面两个决定加速度，起步和减速时使用
	float m_ExpediteSpeed;
	int m_PreExpediteTime;
	//技能等影响的速度
	float m_fTempSpeed;

	float m_NextRotation;


	Point				m_cameraStartPostion;

	int m_nProgressiveRepairTime; //缓慢修理时间
	int m_nUnlockTime; //近战时不能锁定目标

	int m_fireShipSide;//炮弹是从船的哪一侧发出的
	Sprite*spriteEffectSmoke;//用于播放烟雾动画
	Sprite*spriteEffectSpalsh;//用于播放碎片动画

	//判断游戏是否结束
	bool  m_bEndGame;
	//战斗的类型0(正常战斗)1(任务战斗)2(海盗攻城)3(攻击海盗基地)
	int m_nFightType;

	Size m_pyBodySize;

	bool	m_bIsMoveEnd;	   //移动结束

	TVBattleManager*m_battleManager;

	Vec2    m_pPosition;
	Vec2    m_pHitPosition;
};

#endif
