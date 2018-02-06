/*
*
*  CopyRight (c) ....
*  Created on: 2016年1月18日
*  Author:Chengyuan
* 图鉴界面
*
*/
#ifndef __HANDBOOKLAYER_H
#define __HANDBOOKLAYER_H
#include "cocos2d.h"
#include "UIBasicLayer.h"
#include "UICommon.h"
using namespace cocos2d;
class UIHandBook :public UIBasicLayer
{
	enum COMPANIE_STATUES
	{
		//当前小伙伴没有任何碎片
		COMPANIE_NONE_ITEM,
		//当前小伙伴拥有部分碎片
		COMPANIE_HAVE_ITEM,
		//小伙伴碎片已达到解锁要求
		COMPANIE_ITEM_ENOUGH,
		//小伙伴任务开启
		COMPANIE_TASK,
		//小伙伴解锁
		COMPANIE_UNLOCKED,
	};

	//左侧按钮编号
	enum BUTTON_INDEX
	{
		BUTTON_COMPANION,
		BUTTON_LANDSPACE,
		BUTTON_RELIC,
	};
public:
	UIHandBook();
	~UIHandBook();
	bool init();
	void onEnter();
	void onExit();
	static UIHandBook * create();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
private:

	//初始化界面
	void initf(float dt);
	/**
	*显示伙伴详细信息
	*tag : 要显示的伙伴索引值
	*/
	void flushCompanionInfo(int tag);

	/**
	*显示伙伴贴图
	*tag : 要显示的伙伴索引值
	*pos : 贴图所在位置
	*/
	void openCompanionCard(int tag,Vec2 pos);

	/**
	*更改选中小伙伴按钮状态
	*p_sender : 当前点击的头像
	*/
	void changeSelectButtonState(Node * p_sender);

	/**
	*切换小伙伴头像状态
	*imageHead: 当前小伙伴的头像
	*info: 小伙伴信息
	*/
	void changeHeadImageStatus(ImageView * imageHead, CompanionsStatus* info);

	/**
	*显示当前小伙伴任务进度
	*p_sender : 控件父节点
	*info : 伙伴信息
	*/
	void showStorycluse(Widget * p_sender, CompanionsStatus* info);

	/**
	*显示当前小伙伴任务进度
	*p_sender : 控件父节点
	*info : 伙伴信息
	*/
	void showTaskStatue(Widget * p_sender, CompanionsStatus* info);
	/**
	*确认v票解锁小伙伴
	*/
	void openConfirmPayView();

	/**
	*设置左侧按钮状态及点击事件
	*button : 当前点击的按钮
	*/
	void setMainButtonStage(Widget * button);
	//获取当前故事碎片不足的小伙伴解锁需要的v票
	int getUnclockVTicket();
private:
 	GetCompanionsStatusResult * m_statusResult;
	//选中的小伙伴头像按钮
	Node * m_selectedButton;
	//当前小伙伴的状态
	std::vector<COMPANIE_STATUES> m_curCompaniesStatues;
	/*
	*技能图标的表达(每个界面显示时用到要清空)
	*/
	std::vector<SKILL_DEFINE> m_vNormalSkillDefine;
	/*
	*技能图标的表达(每个界面显示时用到要清空)
	*/
	std::vector<SKILL_DEFINE> m_vSpecialSkillDefine;
	//当前伙伴可以解锁
	bool m_curCompanionCanUnlock;
	//当前小伙伴的服务器数据
	CompanionsStatus * m_curCompaionStatus;
	//mainButton 未点击的图片路径
	std::vector<std::string> m_vButtonNormalPath;
	//mainButton 点击后的图片路径
	std::vector<std::string> m_vButtonPressedPath;
	//按钮点击完毕
	bool m_bMainButtonTouched;
};

#endif
