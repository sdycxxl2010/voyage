/*
*  CopyRight (c) 
*  Created on: 2014年9月28日
*  Author: 
*  description: something sailing UI
*/
#ifndef __MAP_UI_H__
#define __MAP_UI_H__

#include "UIBasicLayer.h"
#include "TVSeaEvent.h"
#include "EffectManage.h"
#include "UICommon.h"
#include "SafeAreaCheck.h"
#include "UINationWarLand.h"

class UIStoryLine;
class TVSeaEvent;
class UISailHUD : public UIBasicLayer
{
public:
	enum CONFIRM_INDEX
	{
		//初始状态
		CONFIRM_INDEX_NONE = 0,
		//是否登录城市
		CONFIRM_INDEX_INTOCITY,
		//提示信息，是否切断自动航行
		CONFIRM_INDEX_NOTAUTO,
		//提示信息，打捞中
		CONFIRM_INDEX_SALVAGING,
		//提示信息，是否切断打捞
		CONFIRM_INDEX_INTERRUPT_SALVAGE,
		//提示信息,是否打劫
		CONFIRM_INDEX_START_ROB,
		//提示信息,查看玩家信息
		CONFIRM_INDEX_GET_PLAYERINFO,
		//战斗死亡时，取消付款提示
		CONFIRM_INDEX_CANCEL_V,
		//个人背包已满，扩容取消付款提示
		CONFIRM_INDEX_CANCEL_OVER_WEIGHT,
	};
	enum CONFIRM_INDEX_MAPUI
	{
		//当前好友操作索引初始化
		CONFIRM_INDEX_ACCEPT_ADDFRIEND,
		//拉黑陌生人
		CONFIRM_INDEX_STRANGER_ADDFRIEND,
		//添加拉黑玩家好友
		CONFIRM_INDEX_BLOCK_ADDFRIEND,
		//删除好友
		CONFIRM_INDEX_REMOVEFRIEND,
		//拉黑玩家
		CONFIRM_INDEX_FRIEND_BLOCK,
		//拉黑陌生人
		CONFIRM_INDEX_STRANGER_BLOCK,
		//邀请玩家加入工会
		CONFIRM_INDEX_SOCIAL_INVITE_GUILD,
		//移除黑名单
		CONFIRM_INDEX_BLOCK_REMOVE,

	};
public:
	bool init();
	UISailHUD();
	~UISailHUD();
	static UISailHUD* create();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	virtual void onServerEvent(struct ProtobufCMessage* message, int msgType);

	/*
	*海上暂停
	*isPause:海上事件开始/暂停
	*/
	void shipPause(bool isPause);

	/*
	*发现新城市界面
	*cityId: 城市Id
	*psender:地图上的城市按钮
	*pos:偏移坐标
	*/
	void findNewCity(const int cityId, Widget * psender, Point pos);

	/*
	*得到现在水手数量
	*/
	void getCurrSailorsNum(int sailorNums = 0);

	/*
	*获取当前打捞状态
	*/
	bool getIsSalvaging(){ return m_bIsSalvage; }

	/*
	*漂浮物点击和碰到触礁
	*/
	void onSeaEvent_float(Ref *pSender, Widget::TouchEventType TouchType);

	/*
	*海上npc的点击事件
	*/
	void onSeaEvent_npc(Ref *pSender, Widget::TouchEventType TouchType);

	/*
	*攻击海盗基地
	*npcId: npc的Id
	*/
	void openAttactPirate(const int npcId);

	/*
	*是否停止打捞
	*/
	void openIsStopSalvage();

	/*
	* 海上死亡和快速战斗时的屏幕走字
	*tag:快速战斗结算类型 1 正常结算 2 新手保护 3:强制退出战斗  花费V票失败走字效果
	4:强制退出战斗  未花费V票失败走字效果
	*lostcoin : 损失的银币
	*/
	void openSaillingDialog(int tag, int64_t lostcoin = 0);

	/*
	*显示技能
	*result : 出海数据包含技能信息
	*/
	void setSkillLv(LeaveCityResult* result);

	/*
	*显示航行天数
	*days : 当前行驶天数
	*/
	void setSailingDay(const int days);

	/*
	*显示补给
	*supplies: 当前补给量
	*maxSupplies: 最大补给量
	*/
	void setSupplies(const int supplies, const  int maxSupplies);

	/*
	*显示水手
	*crewnum: 当前水手数量
	*/
	void setCrew(const int crewnum);

	/*
	*船信息显示水手
	*crewnum: 当前水手数量
	*maxCrewNum: 最大水手数量
	*fleetShipNum: 船只数量
	*/
	void setCrew(const int crewnum, const int maxCrewNum, const int fleetShipNum);

	/*
	*将当前的声望和经验转换成为声望等级
	*exp: 当前经验数值
	*fame: 当前声望数值
	*/
	void setExpAndFame(const long int exp, const long int fame);

	/*
	*保存战斗npc的ID
	*npcId: 当前发生战斗的NPC ID
	*/
	void setForceFightNpcId(int npcId){ m_nForceFightNpcId = npcId; }

	/*
	*显示主角等级
	*lv: 角色等级
	*p_lv: 角色声望等级
	*/
	void setPlayerLv(const int lv, const int p_lv);

	/*
	*显示船的坐标信息
	*pos : 当前船只的坐标
	*/
	void setShipPositon(const Vec2 pos);

	/*
	*存储服务器返回信息
	*result: 服务器返回的信息
	*/
	void setResult(GetFleetAndDockShipsResult* result){ m_pResult_fleet = result; };

	/*
	*设置船对象
	*ship: 船对象
	*/
	void setShip(Node* ship);

	/*
	*快速战斗装备破损提示存储本地
	*nBrokenEquips : 损坏装备的个数
	*brokenEquipsItems: 损坏装备信息
	*/
	void setShipEquipBrokenLocal(size_t nBrokenEquips, OutOfDurableItemDefine**brokenEquipsItems);

	/*
	*打劫按钮显示
	*show : 显示/不显示
	*/
	void showLootButton(bool show);

	/*
	*海上事件向后端发送数据
	*se:
	*/
	bool showSeaEvent(TVSeaEvent* se);

	/*
	*刷新当前邮件信息
	*/
	void updateMailNumber(const float fTime);

	/*
	*显示海上事件
	*type:海上事件类型
	*viewFlag:并没有什么用
	*result:触发的海上事件数据
	*/
	void updateSeaEventView(const int type, const int viewFlag, TriggerSeaEventResult* result);
	//当前查看的海上单位的cid
	CC_SYNTHESIZE(int, playerInfoId, PlayerInfoId);

	inline void setDisableCancelFightFlag(int value){ m_nDisableCancelFight = value; }
	inline int getDisableCancelFightFlag(){ return m_nDisableCancelFight; }

	inline int getCancelFightCost(){ return m_nCancelfightcost; }
	inline int getSaveShipCost(){ return m_nSaveshipcost; }

	/*
	*显示背包超重图标
	*/
	void showBagOverWeight();
private:
	/*
	*初始化
	*/
	void initf(float f);

	/*
	*快速战斗结果显示
	*result: 战斗信息
	*/
	void autofightevent(const EndFightResult* result);			

	/*
	*海上事件界面的点击事件
	*/
	void onSeaEvent(Ref *pSender,Widget::TouchEventType TouchType);


	/*
	*海上场景UI的按钮点击事件
	*/
	void buttonEvent(Widget* target,std::string name,bool isRefresh = false);

	/*
	*点击城市, 登陆信息框
	*result: 登陆信息
	*/
	void landCity(GetCityStatusResult* result);

	/*
	*点击城市, 国战时登陆信息框
	*result: 国战登陆信息 暂时使用GetCityStatusResult
	*/
	void countryWarLandCity(GetCityStatusResult* result);

	/*
	* 每秒刷新，更新雷达
	*/
	void update(float f) override;		

	/*
	*海上走马灯
	*/
	void flushSeaChat();	

	/*
	*舰队信息显示
	*result
	*/
	void flushPanelFleetInfo(GetFleetAndDockShipsResult* result);	

	/*
	*海上事件数据显示
	*result: 海上事件数据
	*title: 海上事件标题
	*/
	void flushSeaEventView(const TriggerSeaEventResult* result, std::string title);		

	/*
	*海上npc数据显示
	*view: npc信息UI
	*result: npc信息数据
	*/
	void flushFleetinfoNpc(Widget* view, EngageInFightResult* result);	

	/*
	*进入战斗界面
	*view: 进入战斗的UI
	*result: 战斗数据
	*/
	void flushBattleVS(Widget* view, EngageInFightResult* result);

	/*
	*士气显示
	*view: 士气图案所在的父节点
	*num: 士气
	*/
	void flushMoral(Widget* view, int32_t num);	


	/*
	*舰队信息中单个船的信息
	*tag: 船只编号
	*/
	void initShipDetails(const int tag);										
	
	/*
	*每隔一段事件出现海鸥声音
	*/
	void mewSound(float f);		

	/*
	*部分海上事件播放完动画向后端发送数据
	*psend:触摸层
	*se:海上事件数据
	*/
	void playAnimationEventEnd(Node* psend,TVSeaEvent* se);				
	
	/*
	*初始化舰队每条船的信息
	*item:需要加载船只信息的按钮
	*shipDefine:船只信息
	*tag:船的编号
	*/
	void initFleetShipInfo(Button* item, ShipDefine* shipDefine, int tag);		

	/*
	*海上事件丢失水手
	*num: 失去的水手
	*/
	void lossCrewNum(const int num);

	/*
	*损失的供给
	*num: 损失的补给
	*/
	void lossSupply(const int num);
	
	/*
	*海上事件减少耐久
	*shipid: 船只ID
	*lossHp:当前损失耐久
	*lossMaxhp:损失的最大耐久
	*hp:当前最大耐久
	*maxHp:最大耐久
	*shipname:船只名字
	*order:船只位置
	*/
	void lossDurable(const int shipid, int lossHp, int lossMaxhp, int hp, int maxHp, std::string shipname, int order,int shipCounts);		
	
	/*
	*海上事件降低最大耐久
	*shipid: 船只Id
	*num:损失的耐久
	*shipname:船只名字
	*/
	void lossMaxDurable(const int shipid, const int num, std::string shipname);	

	/*
	*海上事件丢失物品和碰到漂浮物增加物品
	*id:物品Id
	*num:物品数量
	*/
	void addOrLossItem(const int id, const int num);				
	
	/*
	*海上事件丢失银币和碰到漂浮物增加银币
	*num: 银币数量
	*/
	void addOrLossCoin(const int num);		

	/*
	*根据船长id获得船长信息
	*id:船长id
	*/
	CaptainDefine* getCaptainDefine(int id) const;		

	/*
	*海上界面查看物品详细信息的点击事件
	*/
	void showItemInfo(Ref* pSender, Widget::TouchEventType TouchType);	
	/*
	*海上查看船只信息的点击事件
	*/
	void showShipInfo(Ref* pSender, Widget::TouchEventType TouchType);		

	/*
	*战斗结果结算面板带人物头像
	*reslut: 战斗数据
	*/
	void flushBattleResult1(const EndFightResult* result);	
	/*
	*战斗结果结算船只信息面板
	*reslut: 战斗数据
	*/
	void flushBattleResult2(const EndFightResult* result);				
	
	/*
	*关闭地图,恢复船状态
	*/
	void closeMap(Ref*);



	/*
	*发现新城市增加经验，声望
	*addLevelexp: 增长的经验
	*addFame:增长的声望
	*/
	void findCityResult(const int addLevelexp,const int addFame);	
	
	/*
	*海上事件，当有道具时，减少道具数量
	*itemId: 道具Id
	*/
	void lossProps(const int itemId);
	
	/*
	*进入战斗
	*/
	void openFightEvent();	


	/*
	*重写visit函数
	*/
	void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
	
	/*
	*进入不同海域 海域名字显示动画
	*/
	void mapNameAction();

	/*
	 * 进入危险海域警告
	 */
	void WarningAction(std::string content);
	/*
	*船的名字显示限制，汉字3个，英文8个
	*itemName: 船只名字
	*/
	std::string changeShipName(std::string itemName);



	/*
	*显示战斗失败文本
	*/
	void showSaillingFailedText(float t);

	/*
	*打捞结果
	*/
	void openSalvage(const float fTime);

	/*
	*打捞结果
	*result:打捞信息
	*/
	void openSalvage(SalvageResult *result);
	
	/*
	*设置提示图标
	*pResult: 判断数据
	*/
	void setAllUnreadImage(const CheckMailBoxResult *pResult);
	/*
	*副官对话
	*/
	void openCheifChat();

	
	/*
	*弹出副官界面的点击事件
	*/
	void SalvageClickEvent();

	/*
	*打捞状态下的点击事件
	*/
	void branchClickEvent(Ref *pSender, Widget::TouchEventType TouchType);



	/*
	*打开打劫界面
	*battledata: 打劫数据
	*/
	void openLootView(EngageInFightResult *battledata);



	/*
	*在海上事件动画出现前用于吞噬触摸，不可点击其它button
	*/
	void addLayerForSeaEvent();

	/*
	*移除掉用于吞噬触摸的层
	*/
	void removeLayerForSeaEvent();
	
	/*
	*躲过触礁的动画
	*/
	void avoidSeaEventTipAction(std::string name);
	/*
	*船首像发挥作用
	*/
	void shipStatueAvoidSeaEvent(const std::string name, const int iconiid, const int type);
	/*
	*判断生效的船首像ID
	*/
	int confirmShipBowId(ShipBowIconInfo **bowicons, int bowNum, SEA_EVENT_TYPE type);
	//打捞漂浮物结果
	void salvageFloatResult(const TriggerSeaEventResult* result);

	/*
	*倒计时
	*/
	void updateBySecond(const float fTime);
	/*
	*国战提示倒计时
	*/
	void updateNationWarBySecond(const float fTime);
public:
	//判断是否点击NPC
	bool m_bIsNpc;
	//判断是否点击城市
	bool m_bIsCity;
	//判断是否点击打劫
	bool m_bIsLoot;
	//接收npc
	Ref *m_pNpc;
	//接受城市信息
	GetCityStatusResult *m_pCityResult;
	//是否拥有打劫功能
	bool m_CanLoot;
	//障碍物阻挡，漂浮物，海岸
	bool m_bIsBlocking;
	//提示信息的标识
	int   m_nConfirmIndex;
	//海上事件
	int64_t m_nEventversion;
	//海上事件结算列表
	ListView* m_pListview_result;
	//当前补给数量
	int					m_nCurSupplies;
	//当前水手数量
	int					m_nCurCrewNum;
	//判断是否国战期间
	bool		m_bIsWar;
private:
	//经验升级
	bool                m_bIsLevelUp;
	//声望升级
	bool                m_bIsPrestigeUp;
	//声望降级
	bool                m_bIsPrestigeDown;
	//船长增加经验
	bool                m_bIsCaptainUp;
	//战斗失败
	bool                m_bIsFailed;
	//发现新城市加经验引起   主角升级
	bool					m_bIsNewCityCauseLvUp;
	//发现新城市加声望引起   主角声望升级
	bool					m_bIsNewCityCauseFameUp;
	//战斗加经验引起   主角升级
	bool					m_bIsFightCauseLvUp; 
	//战斗引起声望等级变化，升级或降级
	bool					m_bIsFightCauseFameLvChange;
	//发现新城市增加船长经验
	bool                m_pIsNewCityAddCaptainExp;
	//战斗增加船长经验
	bool                m_pIsFightAddCaptainExp;
	//战斗npc的ID
	int 					m_nForceFightNpcId;
	//进入战斗界面，0可取消战斗，1、2为不可取消战斗但可以使用道具或V票取消 1是道具 2是V票-足够3是V票-不足够
	//4:战斗失败后 V票足够时 5:战斗失败后 V票不足够时
	int			        m_nDisableCancelFight;
	//地图标识
	int					m_nCurMapIndex;
	//登录城市ID
	int					m_nLandCityId;
	//m_mainChat中Item个数
	int					m_nChatItem; 
	//跑马灯显示
	bool                m_bIsFirstChat;  
	//走马灯显示的时间
	int					m_nShowChatTime;
	//最大补给数量
	int					m_nMaxSupply;
	//最大水手数量
	int					m_nMaxCrewNum;
	//当前船只数量
	int					m_nFleetShipNum; 
	//发现新城市的ID
	int					m_nFindcityId;
	//你的船对象
	Node*				m_pShip;
	//海域名
	Text	*				m_pLandName;
	//主角等级
	Text *				m_playerLv;
	//声望等级
	Text *				m_prestigeLv;
	//雷达地图
	ImageView*		m_pSmallMap;
	//地图上的小船图标
	ImageView*		m_pShipFlag;
	//左上角水手和补给的背景图
	ImageView*		m_pImage_title_bg;
	//船队信息
	Button*			m_pButtonShipinfo;
	//补给警告图标
	Button*			m_pButton_warning_1;
	//水手警告图标
	Button*			m_pButton_warning_2;
	//整个海上容器
	Widget*			m_pSubPanel;
	//海上人物信息容器
	Widget*			m_pPanelActionbar;
	//经验容器
	Widget*			m_pPanelExp;
	//声望容器
	Widget*			m_pPanelRep;
	//海上事件结算面板
	Widget*			m_pFloatResultview;
	//海域容器
	cocos2d::Rect m_MapRect[19];
	//船舰信息列表容器
	ui::ScrollView*		m_pFleetScrollView;
	//走马灯
	RichText *		m_mainChat;
	//船舰信息面板
	Widget*			m_pPanel_fleet_info;
	//船舰信息主角信息容器
	ImageView*		m_pFleet_image_title_bg;
	//船舰信息水手进度条
	LoadingBar*	m_pProgressbar_sailor;
	//船舰信息补给进度条
	LoadingBar*	m_pProgressbar_supply;
	//保存返回数据
	GetFleetAndDockShipsResult	*m_pResult_fleet;
	//存储船长信息
	std::vector<FleetCaptain*> m_vCaptainList;
	Vector<Widget*> m_vOpenViews;
	//进入战斗时数据
	EngageInFightResult*m_pFightResult;
	//战斗结束是的数据
	EndFightResult*m_pEndFightResult;   
	//海上事件数据
	TriggerSeaEventResult*m_pTriggerSeaResult;
	//海上事件标题
	std::string seaEventTitle;
	//发现新城市数据
	AddDiscoveredCityResult*m_pDiscoverCityResult;
	//对话走字
	//当前显示的长度
	int lenNum;
	//对话向后移动的长度
	int lenAfter;
	//对话的字节长度
	int plusNum;
	//对话是否播完
	bool m_bSaillingDiaolgOver;
	//对话文本框
	Text		*chatTxt;
	//对话文本内容
	std::string chatContent;
	//对话箭头
	ImageView*anchPic;
	//海上打捞
	bool  m_bIsSalvage;
	//海上进入个人中心
	bool m_bAutoToPersonCenter;
	//打捞花费
	int m_nSalvageCost;
	//围攻海盗基地
	GetAttackPirateInfoResult *m_pAttactPirateResult;

	//打劫的玩家信息
	FindLootPlayerResult * m_lootPlayerInfo;
	//打劫玩家的战斗数据
	LootPlayerResult * m_lootBattleInfo;
	//在海上事件动画出现前用于吞噬触摸，不可点击其它button
	Layer*m_touchEventLayer;
	//当前点击的控件Tag值 用于添加/删除好友操作
	int                 m_operateWidgetTag;
	//邀请加入工会玩家的ID
	int                 m_nInviteGuildTag;
	//当前好友操作的类型
	CONFIRM_INDEX_MAPUI m_confirmType;
	//解决打捞连续点击问题
	bool         m_bSalvageClick;
	//打劫保护
	bool         m_robforbid;

	/*
	*技能图标的表达(每个界面显示时用到要清空)
	*/
	std::vector<SKILL_DEFINE> m_vSkillDefine;
	//邀请加入公会
	bool m_bInvitedToguildFailed;

	bool m_bCurrentInSafeArea;

	SafeAreaCheck * m_SafeAreaCheck;
	Text		  * m_pSafeAreaLabel;

	//取消战斗所需v票
	int m_nCancelfightcost;
	//战斗失败后保存船只所需v票
	int m_nSaveshipcost;


	//战斗警告类型
	enum BATTLE_WARNING
	{
		//不支持交赎金
		NOT_PAY_RANSON,
		//支持交赎金
		PAY_RANSON,
		//特殊战斗没有损失
		NOT_LOSE,
	};


	LeaveCityResult* m_pLeaveCityResult;
	/*
	*我的国战数据
	*/
	GetMyNationWarResult *m_pMyNationWarResult;
};


#endif
