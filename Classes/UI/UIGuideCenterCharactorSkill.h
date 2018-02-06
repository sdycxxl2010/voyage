/*
*
*  CopyRight (c) ....
*  Created on: 2015年06月29日
*  Author:Sen
*  The game center layer implementation class
*
*/

#ifndef __SKILLGUIDE_H__
#define __SKILLGUIDE_H__

#include "UIBasicLayer.h"
class UICenterCharactor;
class UICenterItem;
class UICenterLocal;
class UINoviceStoryLine;

class UIGuideCenterCharactorSkill : public UIBasicLayer
{
public:
	//主界面索引(角色，物品，本地仓库)
	enum CENTER_PAGEINDEX
	{
		PAGE_CHARACTOR = 1,
		PAGE_ITEM,
		PAGE_LOCAL,
	};
	enum SKILL_GUIDE
	{
		FOCUS_SKILL_PAGE  = 1,
		START_DIALOG,
		FOCUS_SKILL_TREE,
		SKILL_TREE_DIALOG,
		SKILL_GUIDE_COMPLETE,
	};
	struct DIALOGDATA//展示文字
	{
		int personIdx;
		std::string dialog;
	};
public:
	UIGuideCenterCharactorSkill();
	~UIGuideCenterCharactorSkill();
	static	UIGuideCenterCharactorSkill* createCenter();
	bool	init();
	void	scrollToView(Widget *target);
	//默认的button相应 
	void	menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);	
	void	buttonEvent(Widget *target, std::string name);
	/*
	*按钮点击事件(向下传递相应事件)
	*/
	void	distributeEvent(Widget *target, std::string name);
	//接收服务器信息
	void	onServerEvent(struct ProtobufCMessage *message, int msgType);
	
	/**
	*引导
	*/
	void   guide();

	/**
	*打开旁白
	*parent : 需要放置旁白层的父节点
	*/
	void openAsideLayer(Node * parent);

	/**
	*关闭旁白
	*/
	void closeAsideLayer();

	/**
	*旁白打字机效果
	*/
	void  textShow(float dt);

	/**
	*旁白提示语裁切
	*content : 需要裁切的提示语
	*/
	void  cutChat(std::string content);

	/**
	*设置界面按钮的点击事件关闭
	*parent: 要设置点击事件的父节点
	*touchable: 开启/关闭
	*/
	void setButtonsTouch(Node * parent,bool touchable);

	/**
	*引导 小手指引
	*psender: 下一个要点击的按钮
	*/
	void focusOnButton(Widget * psender);

	/**
	*旁白层的点击事件
	*/
	void asideTouchEvent(Ref *pSender, Widget::TouchEventType TouchType);

	/**
	*解析Json文件
	*/
	void readJsonFile();

	/**
	*确定JSON文件路径
	*path: 路径
	*/
	std::string findJsonFile(std::string path);

	/**
	*判断当前国家的json文件
	*/
	std::string confirmCityJson();

	/**
	*船锚动画
	*/
	void anchorAction();
	
	/**
	*初始化新手引导下的角色信息
	*/
	void initGuideInfo();

private:
	/*
	*角色类
	*/
	UICenterLocal         *m_pLocalLayer;

	/*
	*物品类
	*/
	UICenterItem			*m_pItemLayer;

	/*
	*本地仓库类
	*/
	UICenterCharactor		*m_pCharaLayer;

	/*
	*当前的按钮--主界面索引(角色，物品，本地仓库)
	*/
	Widget				*m_pCurMainButton;

	/*
	*新手引导进度
	*/
	int                  m_guideStage;


	/*
	*旁白裁切后的字符串数组
	*/
	std::vector<std::string> m_words;

	/*
	*小手
	*/
	Sprite               *sprite_hand;

	/*
	*旁白字幕背景
	*/
	Sprite               *m_asideBg;

	/*
	*旁白字幕
	*/
	Text                 *m_aside_chat;
	
	/*
	*对话的长度
	*/
	int  t_size;

	/*
	*字体大小
	*/
	int  FontSize;

	/*
	*当前处于第几段对话
	*/
	int  m_chatnum;
	
	/*
	*当前对话是否说完
	*/
	bool m_cursay;

	/*
	*所有对话完成
	*/
	bool m_chatcomplete;
	
	/*
	*对话长度
	*/
	int  m_contentLength;
	
	/*
	*对话总数
	*/
	int  m_allChatNum;
	
	/*
	*所有对话
	*/
	std::vector<std::string>talks;
	
	/*
	*NPC对话
	*/
	std::string m_talk;
	
	/*
	*对话信息和奖励信息
	*/
	std::vector<DIALOGDATA*> m_dialogdatas;
	
	/*
	*名字信息
	*/
	std::vector<std::string> m_names;
	
	/*
	*技能点
	*/
	int m_skillpointnum;

	/**
	*
	*/
	UINoviceStoryLine * m_dialogLayer;
};

#endif
