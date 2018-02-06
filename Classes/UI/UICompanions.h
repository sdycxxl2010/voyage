/*
*
*  CopyRight (c) ....
*  Created on: 2016年01月13日
*  Author:Sen
*  The game CompanionLayer implementation class
*
*/

#ifndef __COMPANION_LAYER__
#define __COMPANION_LAYER__

#include "UIBasicLayer.h"
#include "UICommon.h"

class UICompanions : public UIBasicLayer
{
public:
	UICompanions();
	~UICompanions();
	bool init() override;
	void onEnter()override;
	void onExit() override;

	static UICompanions* createCompanionLayer();

private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	/*
	*小伙伴列表
	*/
	void openCompanionList();
	/*
	*小伙伴的详情
	*/
	void openCompanionDetails();
	/*
	*小伙伴的装备
	*/
	void updataEquipDetails();
	/*
	*增加小伙伴装备
	*/
	void flushEquipView();
	/*
	*更新选中的显示
	*/
	void updataSelectEquip();
	/*
	*获取装备的数据
	*/
	std::vector<int> getEquipAttribute(const std::string attributeStr);
private:
	/*
	*小伙伴的类型;服役的小伙伴,没有服役的小伙伴，没有服役的船长
	*/
	enum COMPANION_TYPE
	{
		INFLEET_COMPANION,
		IDLE_COMPANION,
		IDLE_CAPTAIN,
	};
	/*
	*m_nIndex:索引值
	*/
	GetCompaniesResult *m_pCompaniesResult;
	/*
	*索引值
	*/
	int m_nIndex;
	/*
	*小伙伴的类型
	*/
	int m_nCompaionType;
	/*
	*技能图标的表达(每个界面显示时用到要清空)
	*/
	std::vector<SKILL_DEFINE> m_vNormalSkillDefine;
	/*
	*技能图标的表达(每个界面显示时用到要清空)
	*/
	std::vector<SKILL_DEFINE> m_vSpecialSkillDefine;
	/*
	*小伙伴装备数据
	*/
	EquipableItem m_curEquipItem[6];
	/*
	*小伙伴装备数据
	*/
	std::vector<EquipableItem> m_vEquipableItem[6];
	/*
	*每个小伙伴只刷新一次数据
	*/
	bool m_bFirstFlush[6];
	/*
	*选中的装备
	*/
	Widget * m_pTempButton;
	/*
	*装备的类型
	*/
	ROLE_EQUIP_SUB_TYPE  m_eType;
};
#endif
