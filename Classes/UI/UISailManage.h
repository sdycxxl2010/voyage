/*
*  CopyRight (c) 
*  Created on: 2014年9月28日
*  Author: 
*  description: something sailing control
*/
#ifndef __MAPS_H__
#define __MAPS_H__

#include "UIBasicLayer.h"
#include "HistCheck.h"
#include "SailRoad.h"
#include "UIShip.h"
#include "UINPCShip.h"


class TVSeaEvent;
class TVSeaEventManage;
class UIShadowShipManage;

class SailScene : public Scene
{
public:
	static Scene *createWithPhysics();
	void render(Renderer* renderer);
CC_CONSTRUCTOR_ACCESS:
    bool initWithPhysics();
};

class UISailManage : public UIBasicLayer
{
public:
	UISailManage();
	~UISailManage();

	static UISailManage* create();
	static UISailManage* create(Point pos, bool seainfo = false);
	//在一定范围是否触发警戒
	bool getIfTriggerRange(){ return m_ischufa;};
	void setIfTriggerRange(bool IsTrigger){ m_ischufa = IsTrigger; };
	bool init(bool seainfo = false);
	void onEnter() override;
	void onExit() override;
	void update(float f);//海上定时器刷新
	void initStatic(float f);//初始化数据
	void initNetworkData(LeaveCityResult* result);//进入海上,初始化数据
	void notifyCompleted(int actionIndex) override;
	void doNextJob(int actionIndex);
	void addLights();//光线处理
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);//点击事件
	void buttonEvent(cocos2d::Ref* target,cocos2d::ui::Widget::TouchEventType type);//点击事件
	void buttonEventName(cocos2d::ui::Widget* target,std::string name);
	void initCityData();//海上城市按钮的初始化
	void mapUICall_func(std::string name);//点击查看舰队信息
	void GoBackCity();//切换场景，返回主城
	void openCountUI(ReachCityResult* result);//登陆结算框

	void registerTouchCallBack(float fTime);
	bool onTouchBegan1(Touch *touch, Event *unused_event);//海上点击处理
	void onTouchCancelled1(Touch *touch, Event *unused_event);
	void onTouchEnded1(Touch *touch, Event *unused_event);//海上点击任意处的点
	void onTouchMoved1(Touch *touch, Event *unused_event);
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	void addSeaEvent_nextBlock(const SeaBlockInfo *info);//更新海上事件
	void flashIconPosXWithText( Text* text, ImageView* image);//海上登录界面费用的显示
	void openDialog();//登录城市的第一个界面
	void openFailureDialog();//海上死亡提示界面
	void setAutoSailRoad(SailRoad* sr);//保存自动航行路线
	void setCityId(int8_t cityId);//保存城市ID
	void texture2DCallback_land(Texture2D* texture);//陆地图片设置
	void texture2DCallback_sea(Texture2D* texture);//海域图片设置
	void addSeaEvents();//海上事件设置(可点击，位置，tag)
	void checkGuardShip(Vec2 pos, float f);//判断是否与npc战斗
	void genenrateEvent(SeaEventDefine** tempEvent,int n_events);// 根据海上不同事件类型初始化事件
	void setSailingDays(int days) { m_nSailDays = days; };//保存航行天数
	void openfindNewCity(Widget * psender);//调取MapUI的发现新城市
	
	void showSeaEvent(TVSeaEvent* temEvent);//调取MapUI的显示海上事件
	void seaEventEnd();  //海上事件结束
	void seaEventStart(); //海上事件开始
	void addSeaEvent_leavCity();//首次进入海上，npc,海上事件的设置
	void addSeaEvent(SeaEventDefine** events,int n_events);//有海上事件发生后，更新海上事件
	inline Node* getMapUI() const { return m_pMapUI;};//获取MapUi对象
	HistCheck* getHistCheck(){ return m_pHistCheck; };
	int64_t getCurrentTimeUsev();//海上获取用户的当前时间

	bool checkFrontBlock(Point pos);//是否碰到障碍物
	void loadMapLine();//船移动，载入地图
	void loadMapLine(int direct);//根据船的方向载入地图
	void setShipDirect(Point pos);//根据点击的点，设置船的移动方向
	
	std::string getSeaCellName(int x,int y);//可能不再用，函数里的资源路径错误
	std::string getCellName(int x,int y);//获取资源
	void asyncLoadingImage(Sprite* land,Sprite* sea,std::string &landName,std::string& seaName);//加载资源
	int getLastNPCEventId(){ return m_lastNPCEventID;}
	void setLastNPCEventId(int npcEventId) { m_lastNPCEventID = npcEventId; m_npcIsActive = 30.0; };
	bool					m_bCaptainToHeroUI;//船长经验没增加，则直接显示登陆结算框
	ReachCityResult*							m_pReachCityResult;//登陆城市数据保存
	//登录城市的时候人物经验升级
	bool m_HeroExpLevelUp;
	//登录城市的时候人物声望升级
	bool m_HeroFameLevelUp;
	//海上事件是不是漂浮物
	bool getSeaEventLayerFloat();
	TVSeaEvent*getSeaFloat();

	bool getIsPause(){ return m_bIsPause; }
	
	LeaveCityResult * getLeaveCityResult(){ return m_pLeaveCityResult; };
	//瞭望的等级
	int m_captain_skill_outlookLv;
private:
	bool					m_bIsBlock;//是否阻挡
	bool					m_bFailedAutoFight;//自动战斗失败
	bool					m_bIsLeaveCityTosea;//是否从城市进入的海
	
	bool					m_bIsCaptainUp;//船长经验是否增加
	int					m_nSailDays;//航行天数
	int					m_lastNPCEventID; //最新战斗的npc event id
	int					m_nTaskNpcId; //任务npc
	int8_t				m_nCityId;//城市ID
	int64_t				m_nShipMoveTime;//船移动的时间
	bool m_ischufa;//在一定范围是否触发警戒
	/*
	*使用道具后npc主动攻击剩余的时间
	*/
	float				m_npcIsActive;
	Layer*				m_EventLayer;//海上世界层
	Layer*				m_MapsLayer;//当前的海域层
	
	Point				m_PrePoint;//移动之前的位置
	Point				m_ScreenFirstPos;
	Point				m_CurTouchPos;//当前点击的位置
	Point				m_CenterPoint;//屏幕中间的位置
	Point				m_ShipPostion;//船位置
	UILoadingIndicator*				m_pLoadLayer;//加载层
	Node*				m_pTouchIcon;//点击海上出现的小白点
	UIShip	*				m_pShipImage;//船图标

	UIBasicLayer*		m_pMapUI;//MapUi对象
	HistCheck*		m_pHistCheck;//点击海上，保存点击的点
	SailRoad*			m_pSailRoad;//航行路线
	
	std::map<int,cocos2d::Sprite*> m_RenderSprites;//存储海上背景图片
	std::deque<Sprite*>					m_UnLoadImage_land;//陆地
	std::deque<Sprite*>					m_UnLoadImage_sea;//海域
	std::vector<bool>						m_vEventFlag;//存储海上事件的状态
	std::vector<UINPCShip*>				m_vNPCShipsF;
	std::vector<UINPCShip*>				m_vNPCShipsD;//存储npc
	LeaveCityResult*							m_pLeaveCityResult;//保存数据
	LoginFirstData *							m_pLoginFirstData;
	TVSeaEventManage*						m_pSeaManage;//海上事件管理类对象
	
	Camera* 							m_mapUICamera;
	Sprite*imageSeaTaskTipBg;
	Sprite*imageSeaTask;

	Sprite*imageSeaCompanionsTaskTipBg;
	Sprite*imageSeaCompanionsTask;

	//每日奖励界面是否需要打开
	bool m_dayViewOpened;

	//海上是否暂停
	bool m_bIsPause;

	UIShadowShipManage*m_nearbyShipManager;
};


#endif
