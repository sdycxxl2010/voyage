/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月30日
 *  Author:Sen
 *  battle ui
 *
 */

#ifndef __BATTLE__UI__
#define __BATTLE__UI__

#include "UIBasicLayer.h"
#include "TVBattleDelegate.h"
#include "network/HttpClient.h"
#include "TVBasicShip.h"

class UIBattleHUD : public UIBasicLayer,public TVBattleDelegate
{
public:
	UIBattleHUD();
	~UIBattleHUD();
	static UIBattleHUD* createBattleUI(EngageInFightResult* result,TVBattleDelegate* dele);
	void init(EngageInFightResult* result);
	void onEnter();
	void onExit();
	void initStaticData(EngageInFightResult* result);  //初始化UI
	void initMainStrategyButton(int index, bool isSwitchAI = true);			   //初始化战斗模式
	void initMainSkillOrPorpButton();				   //初始化技能和道具技能
	void onMainUI();								   //初始化
	/*
	*设置代理
	*/
	void setDelegate(TVBattleDelegate* _delegate)
	{
		m_pDelegate = _delegate;
	};

	void startGame(); //开始战斗
	void stopGame();  //结束战斗
	void stopGameByProps(); //使用脱离战斗道具结束战斗

	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);  //按钮响应
	void buttonEvent(Widget* target,std::string name);
	void porpsButtonEvent(Ref *pSender, Widget::TouchEventType TouchType); //道具使用按钮响应
	void skillButtonEvent(Ref *pSender, Widget::TouchEventType TouchType); //技能使用按钮响应
	void specialButtonEvent(Node* target,std::string name,float varValue); //
	
	void heroDie(int tag);						//船只死亡界面更新
	void heroAttacked(int tag,float harm);		//船只受到攻击界面更新
	void blindEnd(Node* target);                //船只受到攻击时闪烁完成后隐藏

	void onServerEvent(struct ProtobufCMessage* message,int msgType);  //服务器数据
	
	void update(float delta);
	void updateMapPosition();				//更新导航上船只的位置
	void updateCount(float delta);			//更新上方战斗的时间
	void checkIsWin(float delta);			//检测战斗是否结束
	void sendDataToServer(int reson);		//提交战斗数据

	void flushBattleResult1(int winflag, EndFightResult* result);	//战斗结算界面1
	void flushBattleResult2(int winflag, EndFightResult* result);	//战斗结算界面2
	unsigned int getFightTime(float myFighting,float enemyFighting);//战斗的时间

	void switchSkillAndPorp();	//技能与道具切换
	void updateSkillTime(float time);  //技能冷却
	void useSkillEffect(int index,bool isRestore);//使用技能效果
	void setBloodValue(float num,int index,bool isFriend);  //设置界面上的船只血条
	
	void updateAnger(bool isHero);  //更新界面士气
	void onWin(int winflag,EndFightResult* result);  //战斗结果
	void onHttpRequestCompleted(network::HttpClient* client, network::HttpResponse* response);
	//物品详情相应
	void showItemInfo(Ref* pSender, Widget::TouchEventType TouchType);	
	//获取船只详情相应
	void showShipInfo(Ref* pSender, Widget::TouchEventType TouchType);

	void setMainShipDied();  //旗舰沉没后，设置技能不可用和AI模式
	void callUserskillids();//技能动画播放完后恢复定时器及实现skill
	void setShipEquipBroken();//打完仗装备破损提示存储本地
	void openDialogFaild(int64_t lostCoin = 0);//失败对走字
	void showWarFailedText(float t);//显示对话
	void flushBattleResult1Novice();//新手引导结算
	//显示活动海盗攻城结算界面
	void openViewPirateAttack();
	//显示攻击海盗基地的结算界面
	void openViewAttackPirate();
	//捕获船只的id
	std::vector<int> getCapturingShipId(){ return m_nCapturingShipId; };
	void setCapturingShipId(int shipId)
	{
		m_nCapturingShipId.push_back(shipId); 
		auto capturing_ship = StringUtils::format(CAPTURINGSHIPID, m_nCapturingShipId.size());
		//加密
		auto st_num = StringUtils::format("%d", shipId);
		UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(capturing_ship.c_str()).c_str(), getEncryptedPasswordHex((char*)st_num.c_str()));
		UserDefault::getInstance()->flush();
	};
	//所掠夺船只的id
	void setFoodPlunderShipId(int shipId)
	{
		m_nFoodPlunderShipId.push_back(shipId); 
		auto food_plunder = StringUtils::format(FOODPLUNDERSHIPID, m_nFoodPlunderShipId.size());
		//加密
		auto st_num = StringUtils::format("%d", shipId);
		UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(food_plunder.c_str()).c_str(), getEncryptedPasswordHex((char*)st_num.c_str()));
		UserDefault::getInstance()->flush();
	};
	/*
	*屏幕中央的浮动效果，将浮动信息压的堆栈中
	*/
	void pushFightingCentralTips(FIGHTING_CENTER_TIP_DATA tip_data);
	/*
	*屏幕中央的浮动效果
	*/
	void showFightingCentralTips();
	/*
	*保存战斗数据
	*/
	void flushUserDefault(TVBasicShip *ob_A);
	//打劫结算界面 
	void flushRobBattleResult(int winflag, EndFightResult* result);	

private:
	UILoadingIndicator *m_pWaitForLogUploading;  //上传log的Loading
	bool m_bIsFailed;  //战斗是否失败
	TVBattleDelegate *m_pDelegate;
	TVBattleShareObj *m_pShareObj;
	std::vector<Widget*> m_vEnemysInfo; //UI上敌舰
	std::vector<Widget*> m_vHerosInfo;	//UI上舰队
	std::vector<Button*> m_vHeroShipFlags;
	std::vector<Button*> m_vEnemyShipFlags;

	//技能时间
	bool	m_gSkillTime[4];
	int		m_gSkillTimeNum[4];
	float	m_gPerSkillTimeNum[4];

	EngageInFightResult* m_pResult;//进入战斗信息保存

	Widget	*m_pFight;			//战斗主界面
	Text *m_pTimeShow;		//战斗的倒计时
	LoadingBar *m_TimeLoad;		//战斗的倒计loadingbar
	int64_t m_pPrevTime;		
	unsigned int m_RemainTime;
	unsigned int m_TotleTime; 

	Layer	*m_SmallMap;		//层
	bool	m_bIsBattleEnd;		//战斗是否结束

	int m_nSkillID;				//保存技能的Id；

	int					m_nIconIndex;  //结算界面索引
	bool                m_bIsLevelUp;	//玩家等级是否升级
	bool                m_bIsPrestigeUp;//玩家声望是否升级
	bool                m_bIsPrestigeDown;//玩家声望是否降级
	bool                m_bIsCaptainUp;	//玩家船上船长是否升级
	bool                m_bIsSwitchAI; //旗舰死亡后不可切换AI模式
	EndFightResult*m_pEndFightResult;//战斗结束结果保存

	int lenNum;//当前显示的长度
	int lenAfter;//对话向后移动的长度
	int plusNum;//对话的字节长度
	bool m_bDiaolgOver;//对话是否播完
	Text		*chatTxt;//对话文本框
	std::string chatContent;//对话文本内容
	ImageView*anchPic;//对话箭头
	//提交战斗数据（防止重复发送结果数据）
	bool   m_bIsSendToData;
	//围攻海盗基地
	GetAttackPirateInfoResult *m_pAttactPirateResult;
	//被捕后的船只id
	std::vector<int> m_nCapturingShipId;
	//掠夺食物船只id
	std::vector<int> m_nFoodPlunderShipId;
	//发送战斗日志失败重发次数
	int m_sendBattleLogRetry;
	/*
	*中央浮动信息提醒
	*/
	vector<FIGHTING_CENTER_TIP_DATA> m_vTips;
	float m_nTipIntervalTime;
	/*
	*失败时是否提示过增加V票提示
	*/
	bool  m_bIsVTickesTip;
	bool  m_bIsTouch;
	bool  m_bIsTrue;
};

#endif 
