/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game center charactor implementation class
 *
 */

#ifndef __CENTER__CHARACTOR__
#define __CENTER__CHARACTOR__

#include "UIBasicLayer.h"

class UICenterCharactorRole;
class UICenterCharactorEquip;
class UICenterCharactorSkill;

class UICenterCharactor : public UIBasicLayer
{
public:
	//熟练度
	struct PROFICIENCY
	{
		PROFICIENCY_INFO pro;
		int proValue;
	};
	UICenterCharactor();
	~UICenterCharactor();
	static	UICenterCharactor* createCharactor(UIBasicLayer* parent);
	bool	init();
	/*
	*获取人物装备类
	*/
	UICenterCharactorEquip* getCenterEquip();
	/*
	*打开角色界面
	*/
	void    openCenterCharactor();
	/*
	*按钮点击事件
	*/
	void	buttonEvent(Widget *target, std::string name);
	/*
	*服务器返回数据
	*/
	void	onServerEvent(struct ProtobufCMessage *message, int msgType);
	//获得技能layer
	UICenterCharactorSkill * getSkillLayer(){ return m_pSkillLayer; };
	/**
	*获取熟练度信息
	*/
	GetProficiencyValuesResult * getProficiencyResult(){ return m_proficiencyResult; };
private:
	/*
	*按钮点击事件(默认)
	*/
	void	menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	/*
	*按钮点击事件(左面的个人信息，人物装备，技能按钮)
	*/
	void	scrollToView(Widget *target);
	/**
	*打开熟练度界面
	*/
	void    flushProficiencyView(int page);

	/**
	*打开学习熟练度界面
	*/
	void    flushLearnProficiencyView(Widget * p_sender, GetProficiencyBookResult * bookInfo);

	/**
	*获取熟练度id
	*/
	void    getProficiencyInfo(GetProficiencyValuesResult * result);
	/**
	*获取熟练度等级
	*/
	int    getProficiencyLevel(int value);

	/**
	*获取熟练等级对应的经验
	*/
	int    getLevelProficiency(int level);

	/**
	*当前等级获取的经验
	*/
	int    getCurlevelProficiency(int value);
	//滑动条滑动事件
	void usingItemsSliderEvent(Ref* obj, cocos2d::ui::Slider::EventType type);
	/**
	*刷新使用道具界面
	*/
	void flushLearnView();
	/**
	*滑动条事件
	*/
	void updateForNumber(int num);

	/*
	*左侧滚动提示
	*/
	void scrollButtonEvent(Ref *pSender, cocos2d::ui::ScrollView::EventType type);
private:	
	/*
	*角色索引(个人信息，人物装备，技能)
	*/
	enum CHARACTOR_PAGEINDEX
	{
		PAGE_ROLE = 1,
		PAGE_EQUIP,
		PAGE_SKILL,
	};
	enum PROFICIENCY_PAGE
	{
		PAGE_SHIP = 1,
		PAGE_EQUIPMENT = 2,
	};
	/*
	*人物装备类
	*/
	UICenterCharactorEquip				*m_pEquipLayer; 
	/*
	*技能类
	*/
	UICenterCharactorSkill		*m_pSkillLayer;
	/*
	*个人信息类
	*/
	UICenterCharactorRole				*m_pRoleLayer;
	/*
	*当前的按钮--界面索引(个人信息，人物装备，技能)
	*/
	Widget					*m_pMinorButton;
	/*
	*父类(CenterLayer)
	*/
	UIBasicLayer				*m_pParent;
	/**
	*当前熟练度界面
	*/
	int  m_curPage;
	/**
	*熟练度信息
	*/
	GetProficiencyValuesResult * m_proficiencyResult;
	/**
	*船只熟练度Id
	*/
	std::vector<PROFICIENCY> m_shipProficiency;
	/**
	*装备熟练度
	*/
	std::vector<PROFICIENCY> m_equimrntProficiency;
	/**
	*当前操作的item
	*/
	Widget * m_curOperateItem;
	/**
	*当前使用的道具数量
	*/
	int m_curItemMaxNum;

	/**
	*使用物品的个数
	*/
	int m_nUseItemNum;
	/**
	*滑动条
	*/
	Slider * m_pSlider;

	/**
	*使用的道具数量
	*/
	Text * m_pLabelNum;

	/**
	*熟练度增加
	*/
	Text * m_taddProficiency;

	/**
	*熟练度增加进度条
	*/
	LoadingBar * m_loadingbar;

	/**
	*当前熟练度进度
	*/
	float m_curPercent;

	/**
	*使用物品个数
	*/
	Text *  t_proNum;

	/**
	*当前操作的数据
	*/
	PROFICIENCY m_operaInfo;

	/**
	*当前技能的熟练度
	*/
	int m_curProficiency;

	/**
	*当前技能等级
	*/
	Text * m_curLevel;
};

#endif
