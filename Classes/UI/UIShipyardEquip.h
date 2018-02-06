/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月26日
 *  Author:Sen
 *  The game ship equip of ShipYard implementation class
 *
 */
#ifndef __SHIP__EQUIP__VIEW__
#define __SHIP__EQUIP__VIEW__

#include "UIBasicLayer.h"

typedef std::vector<EquipableItem*> vector_equip_type;

class UIShipyardEquip : public UIBasicLayer
{
public:
	UIShipyardEquip();
	~UIShipyardEquip();
	static UIShipyardEquip* createShipEquip(UIBasicLayer *parent);
	/*
	*按钮点击事件
	*/
	void buttonEventDis(Widget *target, std::string name);
	/*
	*设置船只装备里的数据
	*/
	void setShipView(const GetEquipShipInfoResult* result, bool firse = false);
	/*
	*提交装备数据
	*/
	void submitDataToserver();
	/*
	*formation提交装备数据
	*/
	bool m_bIsFormationButtonSubmite;
	/*
	*f刷新船只装备的icon
	*/
	void reflushShipInfo();
	/*
	*设置装备数据
	*/
	void setEquipView(GetEquipableItemsResult *result);

	/**
	*装备修理完成后刷新界面
	*/
	void flushRepairEquipmentView(EquipmentDefine* equipmentInfo);

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);

	void resetModel();
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*装备左侧舰队icon的显示 
	*/
	void showShipsList();
	/*
	*刷新船只数据
	*/
	void flushShipInfo();
	/*
	*船只装备数据处理
	*/
	void pushAllEquipIdAndNum();  
	/*
	*显示装备
	*/
	void showEquipInformation();
	/*
	*刷新选择装备的信息显示
	*/
	void flushEquipInfo();
	/*
	*更改装备的数据
	*/
	void changeVectorContent(bool bIsAddEquip = true);    
	/*
	*更改船只icon按钮的状态
	*参数 target:当前点击的按钮
	*/
	void changePressButtonState(Widget *target);    
	/*
	*获取装备的信息
	*参数 nId:装备的id
	*/
	EquipDetailInfo* getEquipDetailInfo(const int nId);
	/*
	*获取船只上所有装备的各个属性和
	*/
	EquipShipInfo* getEquipDetailSumInfo();
	/*
	*获取装备icon的id
	*参数 nId:装备的唯一id
	*/
	int getEquipIID(const int nId);
	/*
	*字符串转化成一组数
	*参数 _src:字符串  _des:数
	*/
	void convertToVector(std::string &_src,std::vector<int> &_des);
	/*
	*一组数转化成字符串
	*参数 _src:一组数字
	*/
    std::string	convertToString(std::vector<int> &_src);
	/*
	*判断船只使用强化道具强化过的属性并改变颜色为绿色
	*/
	void confirmTextColor(Widget * view, int index);
	/**
	*显示船装备 模型
	*showmodel true 显示模型 false 显示装备
	*/
	void showShipModelOrEquipment(int modelId,bool showmodel = false);
	/**
	*显示船只模型
	*/
	void showShipModel(int modelId);
private:
	/*
	*船只icon按钮(舰队和装备)
	*/
	Widget		*m_PressButton;
	/*
	*父类DockLayer
	*/
	UIBasicLayer	*m_pParent;
	/*
	*选择装备时处理
	*/
	Widget		*m_pTempButton;
	/*
	*记录装备icon id
	*/
	Widget		*m_pEquipIcon;
	/*
	*左侧船只icon
	*/
	ListView	*m_pShipsList;
	/*
	*当前船首相的id
	*/
	int m_nCurFirstH_id;
	/*
	*当前船首炮的id
	*/
	int m_nCurGun1_id;
	/*
	*当前船尾炮的id
	*/
	int m_nCurGun2_id;
	/*
	*第一次装备 船首相的id
	*/
	int m_nFirst_FirstH_id;
	/*
	*第一次装备 船首炮的id
	*/
	int m_nFirst_Gun1_id;
	/*
	*第一次装备 船尾炮的id
	*/
	int m_nFirst_Gun2_id;

	/*
	*记下刷新后第一次装备情况  船帆
	*/
	std::vector<int> m_vFirst_CurSails_id;
	/*
	*记下刷新后第一次装备情况  火炮
	*/
	std::vector<int> m_vFirst_CurGuns_id;
	/*
	*记下刷新后第一次装备情况  装甲
	*/
	std::vector<int> m_vFirst_CurArmors_id;
	// 本地多次操作后的现在装备情况（没有提交网络）
	/*
	*当前装备的 船帆
	*/
	std::vector<int> m_vCurSails_id;
	/*
	*当前装备的 火炮
	*/
	std::vector<int> m_vCurGuns_id;     
	/*
	*当前装备的 装甲
	*/
	std::vector<int> m_vCurArmors_id;
	/*
	*装备的数目
	*/
	std::vector<int> m_vEquips_num;
	/*
	*装备的数据
	*/
	std::vector<EquipableItem**>	m_vAllEquipItem;
	/*
	*当前船只的所有信息
	*/
	GetEquipShipInfoResult         *m_pShipInfoResult;  
	/*
	*装备的信息
	*/
	GetEquipableItemsResult        *m_pEquipItemResult[6];
	/*
	*装备信息id--number
	*/
	std::map<int,int>	m_vAllEquips;
	/*
	*装备个数
	*/
	int                 m_nEquipDetailsNum;
	/*
	*船只索引
	*/
	int                 m_nMainPressIndex;
	/*
	*装备索引
	*/
	int                 m_nGlobalIndex;
	/*
	*装备详情
	*/
	EquipDetailInfo	  **m_pCurEquipdetails;
	//当前的装备 或者替换的装备
	Widget * m_curItem;
	//当前模型
	Sprite3D * m_operaModel;
	//当前模型的modelid
	int m_curModelId;
	//
	Vec2 m_touchBegan;
};
#endif