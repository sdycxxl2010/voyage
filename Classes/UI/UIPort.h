/*
*  CopyRight (c) 
*  Created on: 2014年5月28日
*  Author:
*  description: something about wharf,sailing,supply
*/
#ifndef __SHIP_YARD_SCENE_H__
#define __SHIP_YARD_SCENE_H__
#include "UIBasicLayer.h"
class SailRoad;
class UIPort : public UIBasicLayer
{
public:
	//地图的方向
	enum MAP_DIRCT
	{
		D_UP,
		D_LEFT,
		D_DOWN,
		D_RIGHT,
	};
	//补给和招募水手
	enum SLIDER_INDEX
	{
		STEP_NONE,
		STEP_SUPPLY,
		STEP_FAST_RECRUIT_SAILORS,
	};

public:
	UIPort();
	~UIPort();
	bool init();
	static UIPort* create();
	//初始化数据
	bool initStaticData();

	void notifyCompleted(int index) override;
	void doNextJob(int actionIndex);

	void onServerEvent(struct ProtobufCMessage* message,int msgType);

	//默认点击事件
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	void buttonEvent(Widget* target, std::string name, bool isRefresh = false);
	
	//方向按钮的处理
	void updateDirectButton();

	//方向按钮变化后，更新地图内容  direct：方向
	void changeMapByIndex(MAP_DIRCT direct);

	//进入动画
	void showCaptainChat();
	void anchorAction(float time);

	//点击界面退出对话
	void closeCaptainChat(Ref*pSender,Widget::TouchEventType TouchType);

	//更新银币V票
	void updateRoleCoin();

	//不同的情况，不同的对话
	void chatContentBySupply();

	//地图数据初始化  result：进入港口时接口数据
	void showDockMapData(const GetVisitedCitiesResult*result);

	//补给数据
	void showSupplyPanelData();

	//失败提示信息  name：提示内容
	void showFallInfomation(std::string name);

	//出现地图
	void moveMapToStart();

	//动画退出
	void captainChatOut();

	//恢复地图大小
	void scaleMaptoNomal();

	//出海条件
	bool isCanSea();

	//自动航行出海条件
	bool isCanAutomatic();

	//补给滑动条事件
	void supplySliderChange(Ref* obj,cocos2d::ui::SliderEventType type);

	//缩小地图  view：当前的地图
	Vec2 getMapOffset(const ImageView* view);
	
	//存储地图的四个按钮
	void addMapDirectButton();

	//不同的地图显示不同的城市  view：当前的地图，fieldSize：设置地图的大小
	void addCitiesForMap(ImageView* view,Rect& fieldSize,int index);

	//是否超出地图图片边界  targetPos：在地图上点击的位置，mapSize：地图的大小
	Vec2 isOutofMapSide(Vec2 targetPos, Size mapSize);
	//显示快速招募水手
	void showFastHirePanelData();
	//水手快速补给滑动条事件
	void fastHireCrewSliderChange(Ref* obj, cocos2d::ui::SliderEventType type);
	//不同的情况，不同的对话
	void chatfastHireContent();
	//显示可自动航行的国家
	void openAutopilotView();
private:
	bool						m_bSuppleAutoSea;//供给不足不能自动航行
	bool						m_bIsScale;//地图缩小
	bool						m_bIsNeedScale;//地图是否需要缩小
	bool						m_bMoveActionFlag;//动画移动标志
	bool                    m_bIsAutoToSea;//自动航行
	int						m_nAutoDays;//自动航行天数
	int						m_nAutomaticDays;//自动航行出海界面天数
	int						m_nFirstCityId;//自动航行终点
	int						m_nAutomaticCityId;//自动航行出海城市终点
	int						m_nMapIndex;//地图标识
	int						m_nAddSupplyAmount;//补给数量
	
	ImageView*			m_pImageMap;//地图对象
	Button*				m_pSetOffButton;//出海按钮
	Button*				m_pZoomOutButton;//缩小按钮

	SailRoad*				m_pSailRoad;//自动航行路线
	GetVisitedCitiesResult*  m_pResult;//保存数据
	Button*							m_pButton_backcity;//回城
	Layout*							m_pCities;//地图上添加城市的层
	Vector<Widget*>			m_vDirectWidgets;//保存方向按钮
	Vector<Button*>			m_vCities;//保存每个地图的城市
	//副官说话是否可以点击
	bool					m_bAideClick;
	//快速招募水手的数量
	int m_nFastHireCrewNum;
	//快速招募水手的结果
	CalFastHireCrewResult *m_pFastHireCrewResult;
	//对话顺序
	int m_dialogTag;
	//加载层
	UILoadingIndicator*m_loadingLayer;
	//雇佣水手
	bool m_bIsToTarven;
	Widget *m_pPanleButton;
};


#endif
