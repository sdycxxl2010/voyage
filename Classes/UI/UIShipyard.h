/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月26日
 *  Author:Sen
 *  The game ShipYard implementation class
 *
 */
#ifndef __DOCK__LAYER__
#define __DOCK__LAYER__

#include "UIBasicLayer.h"
class UIShipyardFleet;
class UIShipyardEquip;

class UIShipyard : public UIBasicLayer
{
	enum SHIPYARD_INDEX
	{
		//修理界面船坞
		SHIPYARD_REPAIR_DOCK,
		//修理界面舰队
		SHIPYARD_REPAIR_FLEET,
	};
public:
	UIShipyard();
	~UIShipyard();
	static UIShipyard* createDock();
	bool init();
	void initStaticData();

	void notifyCompleted(int index) override;
	void doNextJob(int actionIndex);

	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	/*
	*更新银币和v票并刷新界面
	*参数 gold:v票个数   coin:银币个数
	*/
	void flushCoinInfo(int64_t coin, int64_t gold);
	/*
	*设置舰队数据是否保存了
	*/
	void setFleetFinishData(bool finsh = false);
	/*
	*获得舰队数据是否保存
	*/
	bool getFleetFinishData();
	/*
	*设置舰队装备数据是否保存了
	*/
	void setShipEquipFinishData(bool finsh = false);
	/*
	*获得舰队装备数据是否保存
	*/
	bool getShipEquipFinishData();
	//所有城市船坞中托管的船只
	int  getAllCityDockShipNum(){ return m_nAll_city_dock_ship_num; };
	void saveAllCityDockShipNum(int nums){ m_nAll_city_dock_ship_num = nums; };

	//更新装备界面信息
	void flushEquipInfo(EquipmentDefine* equipInfo);

	/**
	*获取当前船只或装备所对应当前玩家技能的熟练度等级
	*/
	int getShipOrEquipmentProficiencyLevel(int proficiencyId);
	/**
	*获取当前熟练度对应的熟练度等级
	*/
	int getProficiencyLevel(int proficiencyNum);
	/**
	*获取当前船只所需的技能等级
	*/
	int getNeedProficiencyLevel(int sid);

	/**
	*获取熟练度信息
	*/
	GetProficiencyValuesResult * getProficiencyResult(){ return m_proficiencyResult; };
private:
	/*
	*按钮点击事件
	*/
	void buttonEventDis(Widget *target,std::string name);
	/*
	*船只的修理界面--跳转舰队按钮事件
	*/
	void formationEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*修理界面中的button相应
	*/
	void pageview_1_event(Widget *target,std::string name);
	/*
	*舰队界面中舰队的button相应
	*/
	void pageview_2_event_1(Widget *target,std::string name);
	/*
	*舰队界面中装备的button相应
	*/
	void pageview_2_event_2(Widget *target,std::string name);
	/*
	*工坊界面中的button相应
	*/
	void pageview_3_event(Widget *target,std::string name);
	/*
	*点击物品按钮-物品详情的相应
	*/
	void ItemEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage *message,int msgType);
	/*
	*更新修理界面
	*/
	void updatePageView_1();
	/*
	*更新舰队里的舰队界面
	*/
	void updatePageView_2_1();
	/*
	*更新舰队里的装备界面
	*参数 result;船只的数据
	*/
	void updatePageView_2_2(const GetEquipShipInfoResult *result);
	/*
	*更新工坊里的船只强化界面
	*/
	void updatePageView_3_ship();
	/*
	*更新工坊里的装备强化界面
	*/
	void updatePageView_3_equip();
	/*
	*更新工坊里的船只建造界面
	*/
	void updatePageView_3_shipBuild();
	/*
	*更新工坊里的装备建造界面
	*/
	void updatePageView_3_equipBuild();

	/*
	*修理花费界面提示
	*/
	void updateRepairShipDialog(); 
	/*
	*建造结果界面
	*参数 result;建造结果的数据
	*/
	void showBuildResult(const BuildShipResult *result);
	/*
	*建造里--显示道具详情
	*参数 nIndex;位置索引值
	*/
	void showBuildItemDetails(const int nIndex);
	/*
	*更改上方按键的状态(修理、舰队和工坊按钮)
	*参数 target:当前点击的按钮
	*/
	void changeMainButtonState(Widget *target);
	/*
	*更改左侧按键的状态
	*参数 target:当前点击的按钮
	*/
	void changeMinorButtonState(Widget *target);
	
	/*
	*船只强化界面
	*参数 root:一个元素cell  result:船只数据  i:indx索引
	*/
	void updateReinforceShip(Widget *root,const ReinforceShipDefine *result,const int i);
	/*
	*装备强化界面
	*参数 root:一个元素cell  result:船只装备数据  i:indx索引
	*/
	void updateReinforceEquip(Widget *root,const ReinforceEquipDefine *result,const int i);
	/*
	*船只建造界面
	*参数 root:一个元素cell  result:建造船只数据  i:indx索引
	*/
	void updateShipBuild(Widget *root,const BuildingShipDefine *result,const int i);         
	/*
	*装备建造界面
	*参数 root:一个元素cell  result:建造船只装备数据  i:indx索引
	*/
	void updateEquipBuild(Widget *root,const BuildingEquipmentDefine *result,const int i);
	/*
	*强化方案界面(包含所有的船只方案和装备方案)
	*参数 flag:0-表示花费银币(初次强化该槽)1-表示花费v票(更改该槽的强化)
	*/
	void flushSpecialMaterial(int flag = 0);
	/*
	*强化后的强化信息界面(重置或者替换强化方案)
	*参数 id:强化方案的id
	*/
	void updateResetAndReplaceDialog(int id);
	/*
	*图纸界面
	*参数 result:图纸数据
	*/
	void updateShipAndEquipDrawing(GetDrawingsResult *result);
	/*
	*制造界面
	*参数 index:位置索引
	*/
	void updateShipAndEquipDialog(int index);
	/*
	*建造倒计时
	*/
	void updateBySecond(const float fTime);
	/*
	*解析数据
	*参数 _src:属性字符串 arr:解析数值 n_num;个数
	*/
	int* convertToIntArr(char* _src,int* arr,int n_num);
	/*
	*设置船只强化过后的属性颜色为绿色
	*参数 root:船只cell index:属性索引 isAdd;是否是增加属性值
	*/
	void setTextGreen(Widget * root, int index, bool isAdd);
	/*
	*船坞对话相关函数
	*/
	void showChiefDialog();
	void barGirlButtonEvent(Ref *pSender, Widget::TouchEventType TouchType); 
	void mainButtonMoveToLeft(const float fTime);
	void mainButtonMoveToRight();          //button右移动

	//item名字过长时，超出部分加“...”
	std::string CutItemName(std::string itemName);


private:
	/*
	*确认交互框索引
	*/
	enum CONFIRM_INDEX_DOCK
	{
		CONFIRM_INDEX_SHIP_REINFORCE,
		CONFIRM_INDEX_EQUIP_REINFORCE,
		CONFIRM_INDEX_SHIP_REINFORCE_CHANGE,
		CONFIRM_INDEX_SHIP_REINFORCE_RESET,
		CONFIRM_INDEX_EQUIP_REINFORCE_CHANGE,
		CONFIRM_INDEX_EQUIP_REINFORCE_RESET,
		CONFIRM_INDEX_SHIP_SPEEDUP,
		CONFIRM_INDEX_SHIP_CANCEL,
		CONFIRM_INDEX_EQUIP_SPEEDUP,
		CONFIRM_INDEX_EQUIP_CANCEL,
		CONFIRM_INDEX_REPAIR_SHIP,
	};
	/*
	*船只主界面
	*/
	enum MAININDEX
	{
		MAIN_REPAIR = 1,//修理
		MAIN_FLEET,		//舰队
		MAIN_WORKSHOP,	//船坊
	};
	/*
	*修理
	*/
	enum REPAIRSHIP
	{
		REPAIR_GOLD = 1,  //金币修理 一个船只
		REPAIR_GOLDS, //金币修理 所有船只
		REPAIR_COIN,  //银币修理
		REPAIR_NOT_GOLDS, //v票不足
	};
	/*
	*舰队类
	*/
	UIShipyardFleet		*m_pFleetView;
	/*
	*船只装备类
	*/
	UIShipyardEquip	*m_pShipEquipView;
	/*
	*船队数据
	*/
	GetFleetAndDockShipsResult  *m_pFleetShipResult;
	/*
	*船只图纸数据
	*/
	GetDrawingsResult		    *m_pShipDrawingsResult;
	/*
	*船只装备图纸数据
	*/
	GetDrawingsResult		    *m_pEquipDrawingsResult;
	/*
	*船只强化列表数据
	*/
	GetReinforceShipListResult	*m_pReinforceShipResult;
	/*
	*船只装备强化列表数据
	*/
	GetReinforceEquipListResult	*m_pReinforceEquipResult;
	/*
	*建造船只数据
	*/
	GetBuildingShipsResult	    *m_pShipBuildingResult;
	/*
	*建造船只装备数据
	*/
	GetBuildingEquipmentResult	*m_pEquipBuildingResult;
	/*
	*是否是全部图纸
	*/
	bool m_bIsAllDrawingDefine;
	/*
	*当前城市可以使用的船只图纸
	*/
	std::vector<ShipdrawingDefine*>  m_vCurrentCityCanShipBuild;
	/*
	*当前城市不可以使用的船只图纸
	*/
	std::vector<ShipdrawingDefine*>  m_vCurrentCityNoShipBuild;
	/*
	*当前城市可以使用的装备图纸
	*/
	std::vector<EquipdrawingDefine*> m_vCurrentCityCanEquipBuild;
	/*
	*当前城市不可以使用的装备图纸
	*/
	std::vector<EquipdrawingDefine*> m_vCurrentCityNoEquipBuild;
	/*
	*装备剩余的时间
	*/
	std::vector<int>		m_vUpdateTimeList;
	/*
	*制造的总共时间
	*/
	std::vector<int>		m_vUpdateTotleTimeList;
	/*
	*所有装备ID和IID
	*/
	std::vector<int>        m_equipmentsId;
	/*
	*记录主界面上方按钮索引(修理、舰队、工坊)
	*/
	Widget	*m_pMainPressButton;
	/*
	*记录主界面左方按钮索引
	*/
	Widget  *m_pMinorPressButton;
	/*
	*按钮索引
	*/
	Widget	*m_pTempButton;
	/*
	*按钮索引
	*/
	Widget	*m_pTempButton2;
	/*
	*装备强化索引
	*/
	Widget	*m_pEquipForce;
	/*
	*修改强化方案id
	*/
	int     m_nOptionItem;
	/*
	*强化的位置（1或2）
	*/
	int		m_nPostionFlag;
	/*
	*主标题位置（1（修理）、2（舰队）或3（工坊））
	*/
	int	m_nMainIndex;
	/*
	*强化索引（强化船只或装备）
	*/
	int		m_nGlobalIndex;
	/*
	* 强化索引(强化方案)
	*/
	int		m_nTempIndex;       
	/*
	* 是否是第一次获取装备的数据（船只装备）
	*/
	bool    m_bFirstGetEquip;
	/*
	* 确认索引
	*/
	CONFIRM_INDEX_DOCK	m_nConfirmIndex;
	/*
	* 舰队数据保存
	*/
	bool	m_bFleetFinishData;
	/*
	*船只装备数据保存
	*/
	bool    m_bShipEquipFinishData; 
	/*
	*修理索引
	*/
	REPAIRSHIP     m_nShipRepairIndex;
	//所有城市船坞中托管的船只
	int m_nAll_city_dock_ship_num;
	//当前修理界面
	SHIPYARD_INDEX m_repairView;
	//加载界面
	UILoadingIndicator * m_loadingLayer;
	//制造船或装备时材料不足
	bool m_bIsMaterialEnough;
	//制造船或装备时银币不足
	bool m_bIsCoinEnough;
	//制造船或装备时城市是否满足
	bool m_bIsRightCity;
	//当前选中的装备
	Widget * m_curSelectEquipment;
	/**
	*熟练度信息
	*/
	GetProficiencyValuesResult * m_proficiencyResult;
};
#endif
