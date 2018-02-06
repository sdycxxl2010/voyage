/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月26日
 *  Author:Sen
 *  The game fleet of ShipYard implementation class
 *
 */
#ifndef __FLEET__VIEW__
#define __FLEET__VIEW__

#include "UIBasicLayer.h"

class UIShipyardFleet : public UIBasicLayer,ui::EditBoxDelegate
{
public:
	UIShipyardFleet(UIBasicLayer* parent);
	~UIShipyardFleet();
	static UIShipyardFleet* createFleet(UIBasicLayer* parent);
	bool init();
	/*
	*提交数据
	*/
	void sendDataToServer();
	/*
	*按钮点击事件
	*/
	void buttonEventDis(Widget* target, std::string name);
	/*
	*更新舰队数据
	*参数 result:舰队数据
	*/
	void updateFleetAndDockView(GetFleetAndDockShipsResult* result);
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
	{
		if (TouchType == Widget::TouchEventType::ENDED)
		{
			m_parent->menuCall_func(pSender,Widget::TouchEventType::ENDED);
		}	
	};
	/*
	*舰队位置交换
	*/
	void switchShipOrder(int first,int second);
	/*
	*刷新舰队船长信息
	*参数 index:位置索引 capInfo:船长信息
	*/
	void updateCaptainInfo(int index,CaptainDefine* capInfo);
	/*
	*交换船长
	*/
	void switchCaptain();
	/*
	*船长操作
	*参数 caps:船长信息 n_caps:位置索引
	*/
	void pushCaptainList(CaptainDefine** caps,int n_caps);
	/*
	*更新船坞船只
	*参数 item:船只cell ShipDefine:船只信息  tag:位置索引
	*/
	void updateDockShipInfo(Widget* item,ShipDefine* shipDefine,int tag);
	/*
	*更新舰队船只
	*参数 item:船只cell ShipDefine:船只信息  tag:位置索引
	*/
	void updateFleetShipInfo(Widget* item,ShipDefine* shipDefine,int tag);
	/*
	*更新舰队捕获船只
	*参数 item:船只cell ShipDefine:船只信息  tag:位置索引
	*/
	void updateFleetCatchShipInfo(Widget* item, ShipDefine* shipDefine, int tag);
	/*
	*船只移动--移动到舰队中
	*参数 index:位置索引
	*/
	void addShipToFleet(int index);
	/*
	*船只移动--移动到捕获槽中
	*参数 index:位置索引
	*/
	void addShipToCatch(int index);
	/*
	*船只移动--移动到船坞中
	*参数 index:位置索引
	*/
	void addShipToDock(int index);
	/*
	*船只移动--刷新舰队界面
	*/
	void flushFleetShipInfo();
	/*
	*船只移动--刷新船坞界面
	*/
	void flushDockShipInfo();
	/*
	*船只移动--更新船只托管费用
	*/
	void updateFee();
	/*
	*获取更改的船只名
	*/
	char** getAllChangeShipName();
	/*
	*更改的船只name相应
	*/
	void textFiledEvent(Ref* target, bool type);
	/*
	*获取装备信息
	*参数 id:装备id
	*/
	EquipDetailInfo* getEquipDetailInfo(int id) const;
	/*
	*获取船长信息
	*参数 id:船长id  isCaptain:是不是船长
	*/
	CaptainDefine*	getCaptainDefine(int id, int isCaptain) const;
	/*
	*获取船只信息
	*参数 id:船只id
	*/
	ShipDefine*		getShipDefine(int id) const;
	/*
	*判断船只使用强化道具后属性颜色变化
	*参数 view:船只cell  index:位置索引
	*/
	void confirmTextColor(Widget * view, int index);
	/*
	*输入完成会调用的方法
	*/
	virtual void editBoxReturn(ui::EditBox* editBox);
private:
	/*
	*船只信息
	*/
	std::vector<ShipDefine*> m_vAllShips;
	/*
	*舰队里船只id
	*/
	std::vector<int> m_vFleetShip;
	/*
	*舰队捕获船只id
	*/
	std::vector<int> m_vCatchShip;
	/*
	*船坞里船只id
	*/
	std::vector<int> m_vDockShip;		
	/*
	*船长id
	*/
	std::vector<FleetCaptain*> m_vCaptainList;
	/*
	*暂时没有的船长id
	*/
	std::vector<CaptainDefine*> m_vUnusedCaptain;
	/*
	*更改船只name的id
	*/
	std::vector<int> m_vChangeShipNameId;
	/*
	*船只索引
	*/
	unsigned int m_nGlobalIndex;
	/*
	*船长索引
	*/
	unsigned int m_nTempIndex;
	/*
	*父类DockLayer
	*/
	UIBasicLayer  *m_parent;
	/*
	*左侧按钮(舰队和装备)
	*/
	Widget      *m_pPressButton;
	/*
	*按钮索引
	*/
	Widget		*m_pTempButton;
	/*
	*舰队里--船队ListView
	*/
	ListView	*m_pFleetList;   
	/*
	*舰队里--船坞ListView
	*/
	ListView	*m_pDockList;
	/*
	*更改船只name的TextField
	*/
	ui::EditBox	*m_pInputText;
	/*
	*是否输入结束
	*/
	bool        bIsEnter;
	/*
	*更改船只name 索引
	*/
	int			 m_nInputNameIndex;
	/*
	*舰队里船只数据
	*/
	GetFleetAndDockShipsResult	*m_pFleetAndDockResult;
	/*
	*是否是第一次将舰队的船只移动到船坞里
	*/
	bool		     m_bShipToDockFormFleet;
	/*
	*记录舰队里船只的唯一id
	*/
	std::vector<int> m_vIid;

	/*
	*确认交互框索引
	*/
	enum CONFIRM_INDEX_FLEET
	{
		CONFIRM_INDEX_SHIP_TO_DOCK,
		CONFIRM_INDEX_SHIP_TO_CATCH,
	};

	/*
	* 确认索引
	*/
	CONFIRM_INDEX_FLEET	m_nConfirmIndex;
};

#endif

