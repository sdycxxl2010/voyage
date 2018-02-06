/*
*  CopyRight (c)
*  Created on: 2015年7月28日
*  Author:
*  description: something sailing UI Guide
*/
#ifndef __MAP_UIGUIDE_H__
#define __MAP_UIGUIDE_H__

#include "UIBasicLayer.h"
class UIStoryLine;
class TVSeaEvent;
class UIGuideSailHUD : public UIBasicLayer
{
public:
	enum CONFIRM_INDEX
	{
		CONFIRM_INDEX_INTOCITY,//是否登录城市
		CONFIRM_INDEX_NOTAUTO//提示信息，是否切断自动航行
	};
public:
	//Unreasonable_code_24;成员私有化
	bool m_bIsBlocking;//障碍物阻挡，漂浮物，海岸
	int   m_nConfirmIndex;//提示信息的标识
	int64_t m_nEventversion;//海上事件

	bool init();
	UIGuideSailHUD();
	~UIGuideSailHUD();
	static UIGuideSailHUD* create();
	void initf(float f);
	virtual void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void setSailingDay(const int days);							//显示航行天数
	void setSupplies(const int supplies, const  int maxSupplies);	//显示补给
	void setCrew(const int crewnum);					//显示水手
	void setExpAndFame(const long int exp, const long int fame);				//转换经验与声望
	void setPlayerLv(const int lv,const int repLv);					//显示主角等级
	void setShipPositon(const Vec2 pos);	//显示船的坐标信息
	void setShip(Node* ship);				//设置船对象
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;

	void buttonEvent(Widget* target, std::string name, bool isRefresh = false);
	void landCity(GetCityStatusResult* result);//点击城市,登陆信息框
	void update(float f) override;				//每秒刷新，更新雷达
	void flushMoral(Widget* view, int32_t num);				//士气显示
	void flushBattleVS(Widget* view, EngageInFightResult* result);						//进入战斗界面
	
	void mewSound(float f);											//每隔一段事件出现海鸥声音
	bool showSeaEvent(TVSeaEvent* se);							//海上事件向后端发送数据
	void shipPause(bool isPause);								//暂停船

	void findNewCity(const int cityId);		//发现新城市面板
	void findCityResult(const int addLevelexp, const int addFame);			//发现新城市增加经验，声望
	void openFightEvent();					//进入战斗
	void setForceFightNpcId(int npcId){ m_nForceFightNpcId = npcId; }		//保存战斗npc的ID
	void GuildmapNameAction();//海域名字显示动画

private:

	bool                m_bIsLevelUp;//经验升级
	bool                m_bIsPrestigeUp;//声望升级
	bool                m_bIsFailed;//战斗失败
	bool					m_bIsNewCityCauseLvUp;//发现新城市加经验引起   主角升级
	bool					m_bIsNewCityCauseFameUp;//发现新城市加声望引起   主角声望升级
	int 					m_nForceFightNpcId;//战斗npc的ID
	int			        m_nDisableCancelFight;//进入战斗界面，0可取消战斗，1为不可取消战斗
	int					m_nCurMapIndex;//地图标识
	int					m_nLandCityId;//登录城市ID
	int					m_nMaxSupply;//最大补给数量
	int					m_nMaxCrewNum;//最大水手数量
	int					m_nFindcityId;//发现新城市的ID

	Node*				m_pShip;//你的船对象
	Text	*				m_pLandName;//海域名
	Text *				m_playerLv;//主角等级
	Text *				m_playerRepLv;//主角声望等级
	ImageView*		m_pSmallMap;//雷达地图
	ImageView*		m_pShipFlag;//地图上的小船图标
	ImageView*		m_pImage_title_bg;//左上角水手和补给的背景图
	Widget*			m_pSubPanel;//整个海上容器
	Widget*			m_pPanelActionbar;//海上人物信息容器
	Widget*			m_pPanelExp;//经验容器
	Widget*			m_pPanelRep;//声望容器

	cocos2d::Rect m_MapRect[9];//海域容器
	LoadingBar*	m_pProgressbar_sailor;//船舰信息水手进度条
	LoadingBar*	m_pProgressbar_supply;//船舰信息补给进度条
	AddDiscoveredCityResult*m_pDiscoverCityResult;//发现新城市数据
	bool m_bCurrentInSafeArea;

	SafeAreaCheck * m_SafeAreaCheck;
	Text		  * m_pSafeAreaLabel;

};


#endif
