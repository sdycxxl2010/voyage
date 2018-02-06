/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game center skill implementation class
 *
 */

#ifndef __CENTER__SKILL__LAYER__
#define __CENTER__SKILL__LAYER__

#include "UIBasicLayer.h"

class UICenterCharactorSkill : public UIBasicLayer
{
public:
	UICenterCharactorSkill();
	~UICenterCharactorSkill();
	static UICenterCharactorSkill* createSkill(UIBasicLayer *parent);
	/*
	*打开技能界面
	*/
	void openCenterSkill();
	/*
	*按钮点击事件
	*/
	void buttonEvent(Widget *pSender,std::string name);
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage *message,int msgType);
	//新手引导用到的设置技能点数
	void setSkillPoints(int skillpoints){ m_nSkillPoints = skillpoints; };
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	/*
	*更新主界面技能剩余点数和技能树界面上的技能剩余点数
	*/
	void updateMainUISkillPoint();
	/*
	*刷新技能主界面的主动技能(4个槽的信息)
	*/
	void flushMainUIALLSkill();
	/*
	*选择主线技能界面(装载4个技能槽)
	*/
	void openAddSkillView();
	/*
	*技能树界面
	*/
	void openSkillTreeView();
	/*
	*升级技能界面
	*参数 nSkill_id:当前技能的id
	*/
	void openUpgradeView(const int nSkill_id);
	/*
	*刷新所有的技能数据
	*/
	void pushResultInMap();
	/*
	*技能介绍界面
	*参数 nSkill_id:当前技能的id
	*/
	void updataSelectSkill(const int nSkill_id);
	/*
	*刷新技能树里左侧按钮上的文本
	*/
	void flushMinorButton();
	/*
	*更改按钮状态(技能树界面上方的交易和战斗按钮)
	*参数 target:当前点击的按钮
	*/
	void changeMainButtonState(Widget *target);
	/*
	*更改按钮状态(技能树界面左侧的按钮)
	*参数 target:当前点击的按钮
	*/
	void changeMinorButtonState(Widget *target);


private:
	/*
	*剩余的技能点数
	*/
	int				  m_nSkillPoints;
	/*
	*根节点(CenterLayer)
	*/
	UIBasicLayer       *m_pParent;
	/*
	*主动技能装置时记录选择的技能
	*/
	Widget			 *m_pTempButton;
	/*
	*记录技能树上方的战斗和贸易按钮
	*/
	Widget			 *m_pSkillTreeMainButton;
	/*
	*记录技能树左侧的按钮
	*/
	Widget			 *m_pSkillTreeMinorButton;
	/*
	*主动技能位置索引
	*/
	int				  m_nSoltIndex;
	/*
	*技能的详情数据
	*/
	GetSkillsDetailsResult			*m_pResult;
	/*
	*获取已经装载的主动技能数据
	*/
	GetSelectedPositiveSkillsResult *m_pSelPosResult;
	/*
	*装载主动技能的主动技能数据
	*/
	GetHeroPositiveSkillListResult  *m_pHeroSkillResult;
};

#endif
