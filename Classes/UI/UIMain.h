/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月03日
 *  Author:Sen
 *  主城
 *
 */

#ifndef __MAIN__LAYER__
#define __MAIN__LAYER__

#include "UIBasicLayer.h"
#include "EffectManage.h"

class UIMain : public UIBasicLayer
{
public:
	//对话索引
	enum GUARD_INDEX
	{
		NONE = 0,
		BATTLE_NPC_FAIL,	//战斗失败提示
		SIDE_TASK_BATTLE_NPC,	//任务:攻打NPC
		//SIDE_TASK_GOODS,		//任务:运送货物
		ENEMY_CITY_CHAT,		//敌对城市
		DISGUISE_CITY_CAHT,		//使用伪装道具	
		NEUTRAL_CHANGETO_ENEMY_CHAT,//中立城市提示
		//POPULAR_ITEM,			//流行品提示
		GAME_EVENT,				//事件
		GAME_NEWBIE_GUIDE_END1,	//新手引导结束
		GAME_NEWBIE_GUIDE_END2	//新手引导结束
	};
	//点击图片提示
	enum TIP_IMAGE_INDEX
	{
		IMAGE_TIP_NONE = 0,
		//水手不足的提示
		IMAGE_TIP_LACK_SAILORS,
		//船只损坏的提示
		IMAGE_TIP_LACK_SHIP_DURE,
		//银行保险
		IMAGE_TIP_BANK_INSURANCE,
	};
	

public:
	UIMain();
	~UIMain();
	bool init() override;
	void onEnter()override;
	void onExit() override;
	static UIMain* createMainLayer();
	void initRes(const float fTime);

	void showBarData();			//加载数据
	void loadBackground();		//加载城市背景
	void loadCityInfo();		//加载城市的信息
	void loadOtherInfo();		//加载其它信息
	void notifyCompleted(int index) override;
	void doNextJob(int actionIndex);
	void flushMainChat();		//跑马灯实现
	void flushCionAndGold(const int64_t nCoin, const int64_t nGold);	//刷新银币和V票
	void flushVActivityBonus();
	void flushSudanMaskNum(const int nNum);			//刷新伪装道具
	void setMainLayerButton(const bool isTouch);		//敌对城市设置
	void bgMusicAndSound();						//背景音效

	void openGuardChat(int index);    //对话索引
	void guardEnterScene(const float fTime);	 //副官进入场景
	void guardExitScene(const float fTime);	 //副官退出场景
	void chatFadeIn(const float fTime);		 //对话内容渐入
	void chatFadeOut(const float fTime);		 //对话内容渐出
	void anchorAction(const float fTime);		 //对话的动画(锚)
	std::string getChiefContent();		 //对话的内容
	std::string getCityItemName();		 //流行品城市的名字
	void moveEnd(){ m_bMoveActionFlag = true;};

	void menuCall_func(Ref *pSender,Widget::TouchEventType touchType) override;
	void callEventByName(std::string& name,Node* node);

	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	
	void getAllUnreadImage(const CheckMailBoxResult *pResult);  //提示
    void getMainTaskUnreadImage();//当前主线任务完成提示
	void checkBountyBoard();
	void showMaincityInfo(const GetPalaceInfoResult *pResult, const GetCityProducesResult *proudceResult, const GetFriendValueResult * friendInfo);	 //城市信息
	void updateMailNumber(const float fTime);						 //检查更新
	void villageButtonShow();//当进入村庄第一阶段时
	void mainCityEscortTip();//护送成功的信息

	void showCityGuardText(float t);//显示对话
	void showLimitPackTime(float f);//显示礼包倒计时
	void showGiftPackBg(bool isShow);//等级礼包的背景显示
    void showRaindrops(float dt);
	void buttonImageNotifyShow();//大按钮的感叹号

	void newbieGuideEnd(); //新手引导结束
	//验证按钮显示
	void showVerify(bool isShow);
	//水手或船只耐久低于某个值的提示
	void showSailorOrShipDur();
	//关闭主城下雨效果
	void closeRainEffect();
	//刷新主城的级别和声望等级
	void flushExpAndRepLv();
	//有保险时显示保险图标
	void showInsuranceHints(CheckMailBoxResult*result);
	//faceBook被邀请的图标 isShow：是否显示
	void showFaceBookInvitedHints(bool isShow);
	//领取faceBook被邀请奖励
	void openGetFaceBookReward();
	//faceBook领取完奖励，邀请他人
	void openFacebookInviteOthers();

	//打开首充v票活动界面
	void openVActivityBonus();
	/*
	*控制主城右上方按钮的显示与隐藏动画
	*
	*/
	void showAndHideCityButton();
	void showAndHideEnd(){ m_bShowAndHideAction = true; };
	//水手快速招募
	void showFastHirePanelData();
	//水手快速补给滑动条事件
	void fastHireCrewSliderChange(Ref* obj, cocos2d::ui::SliderEventType type);
	

	//是否是使用道具再次回到主城
	void setUsePropBackToCity(bool isUse){ isUsePropBackToCity = isUse; };
	bool getUsePropBackToCity(){ return isUsePropBackToCity; };
	

	//是否是敌对城市
	void setIsEnemyCity(bool isInEnemy){ m_bIsEnemyCity = isInEnemy; };
	bool getIsEnemyCity(){ return m_bIsEnemyCity; };


	/*
	*新手引导后奖励30000
	*/
	void getNpcPresentItem(const GetPretaskItemsResult *preTask);
	/*
	*显示掉落服务图标
	*/
	void showDorpservice();
	/*
	*更新掉落服务的时间
	*/
	void updataDorpserviceTime(int sec, int type);
	/*
	*倒计时
	*/
	void updateBySecond(const float fTime);

	/**
	*显示友好度信息
	*/
	void friendValueFlag(Ref *pSender, Widget::TouchEventType TouchType);

	/**
	*获取港口类型或者港口信息
	*int port 获取的内容 0港口类型  1港口信息 2港口类型对应json文件的key值
	*/
	std::string getPortTypeOrInfo(int port = 0);

	/*
	*显示背包超重图标
	*/
	void showBagOverWeight();
	//打开国战海报
	void openCountryWarPoster(const GetCityPrepareStatusResult *result);
	void textEvent(Ref* target, Widget::TouchEventType type);
private:
	//是否是敌对城市
	bool	m_bIsEnemyCity;
	//是否是使用道具再次回到主城
	bool isUsePropBackToCity;
	int isNewLanding;
	bool m_bIsGuardShowFromSea;//从海上到城市对话的标识
	RichText   *m_pMainChat; //跑马灯 
	bool  m_bIsFirstChat;     //跑马灯显示
	int   m_nShowChatTime;  //跑马灯时间
	int   m_nChatItem;  //m_mainChat中Item个数

	GetCurrentCityDataResult *m_pCityDataResult;
	CheckMailBoxResult	*m_pCheckResult;

	int		    m_nCompletedAllTasks;//当前主线任务是否全部完成

	//当前的主线任务是否完成
	int         m_nCurMainTaskComplete;
	
	bool		m_bIsGuardExit;
	bool		m_bIsShowLibao;
	
	TaskDefine		*m_pCurTaskDefine;
    
	bool        m_bMoveActionFlag;
	bool        m_bisFirstShow;	//是否是第一次对话
	int			m_nGuardIndex;	//对话索引
	int			m_nLastGuardTag;	//索引
	bool		m_bIsNeutralCityChange;//中立势力是否变成敌对
	int         m_nToTalGuardPage;//对话的总页数
	int         m_nCurGuardPage;//当前对话页
	
	int lenNum;//当前显示的长度
	int lenAfter;//对话向后移动的长度
	int plusNum;//对话的字节长度
	bool guardDiaolgOver;//对话是否播完
	Text		*chatTxt;//对话文本框
	std::string chatContent;//对话文本内容
	ImageView*anchPic;//对话箭头
	bool m_bPopItemFlag;//在被通缉的城市点击按钮防止出现流行品提示

    std::vector<Sprite*>* m_raindrops;

	/*
	*执行动作是否执行完毕
	*/
	bool m_bShowAndHideAction;

	//简报信息
	GetPalaceInfoResult*m_palaceInfoResult;
	//小提示图片
	int m_nTipImageIndex;
	//快速招募水手的结果
	CalFastHireCrewResult *m_pFastHireCrewResult;
	int m_nFastHireCrewNum;
	/*
	*是否显示新手引导领取奖励弹框
	*/
	bool   m_bNewbieGuidePretask;
	/**
	*简报商品信息
	*/
	GetCityProducesResult * m_producesResult;
	/**
	*简报势力友好度
	*/
	GetFriendValueResult * m_friendValueResult;
	//用于判断首充v成功
	bool	m_bIsChargeVSuccees;
	//加载层
	UILoadingIndicator*m_loadingLayer;
	//是否有可领取奖励
	int m_bReceive;
	//祈福是否可以进行
	int m_bClifford;

	//是否是修理
	bool  m_bIsRepair;
	int   m_nRepairCost;
};

#endif
