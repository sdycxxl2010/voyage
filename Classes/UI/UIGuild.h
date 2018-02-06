/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game guild implementation class
 *
 */

#ifndef __GUILD__LAYER__
#define __GUILD__LAYER__

#include "UIBasicLayer.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

class UIGuild : public UIBasicLayer, TableViewDataSource, TableViewDelegate, extension::EditBoxDelegate
{
public:
	UIGuild();
	~UIGuild();
	void onExit();
	void onEnter();
	bool init();

	static UIGuild*  getInstance()
	{
		if (m_pGuildLayer == nullptr)
		{
			m_pGuildLayer = new UIGuild;
		}
		return m_pGuildLayer;
	}
	/*
	*公会
	*/
	void openGuildLayer();
	/*
	*重载判断的方法
	*/
	virtual void editBoxReturn(ui::EditBox* editBox);
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)override;
	/*
	*管理公会权限事件
	*/
	void checkboxCall_func(Ref *pSender,CheckBoxEventType TouchType);
	/*
	*公会签名事件
	*/
	void textFiledEvent(Ref *target, bool type);
	/*
	*公会列表界面button相应
	*/
	void guildListEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*我的公会界面button相应
	*/
	void myGuildListEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*公会申请界面或者公会邀请button相应
	*/
	void applicationsEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*更改按钮状态(界面左侧按钮 公会列表、我的公会、公会申请、公会要求)
	*/
	bool isButtonInListView(std::string& name,Widget *target);       
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	/*
	*刷新左侧按钮
	*参数 bHasGuild:是否有公会 bIsadm:是否是管理员 nNum:未处理的信息个数
	*/
	void flushLeftButton(const bool bHasGuild,const bool bIsadm,const int nNum);
	/*
	*刷新公会列表界面
	*/
	void flushGuildList();
	/*
	*刷新我的公会界面
	*参数 pMyGulidDetailsResult:我的公会数据
	*/
	void flushMyGuild(GetMyGuildDetailsResult *pMyGulidDetailsResult);
	/*
	*刷新公会的申请界面
	*参数 pApplyInfoResult:公会的申请数据
	*/
	void flushApplication(const GetApplyInfoResult *pApplyInfoResult);
	/*
	*刷新公会的邀请界面
	*参数 pInvitInfoResult:公会的邀请数据
	*/
	void flushInvitation(const GetInvitationListResult *pInvitInfoResult);
	/*
	*查找公会输入公会名称界面
	*/
	void openSearchView();
	/*
	*公会的详情界面
	*参数 pGuildDetails:公会的数据
	*/
	void openGuildDetailView(const GetGuildDetailsResult *pGuildDetails);
	/*
	*管理公会的界面
	*参数 pGulidMember:公会成员权限数据
	*/
	void openManagerView(const GetGuildMemberPermissionResult *pGulidMember);
	/*
	*创建公会时条件满足时--界面
	*/
	void openCreateGuildView();
	/*
	*创建公会时条件不满时--界面
	*参数 pResult:创建公会数据
	*/
	void openCreateGuildFailView(const TestCreateNewGuildResult *pResult);
	/*
	*扩展公会时刷新界面
	*参数 pExpandGuildResult:扩展公会数据
	*/
	void flushMembersNum(const ExpandGuildCapacityResult *pExpandGuildResult);
	/*
	*退出公会的相应
	*/
	void quitGuildEvent(Ref *pSender, Widget::TouchEventType TouchType);
//chengyuan++
	void update(float delta);
	/* 必须实现的函数 */
	// 当滑动tableview时触发该方法 参数为当前的tableview对象
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view);
	// 当tableview被放大或缩小时触发该方法  参数为当前tableview对象
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {};
	// 当cell被点击时调用该方法 参数为当前的tableview对象与被点击的cell对象
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);
	// 设置tableview的Cell大小
	virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
	// 获取编号为idx的cell
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	// 设置tableview中cell的个数
	virtual ssize_t numberOfCellsInTableView(TableView *table);
private:
	static UIGuild     *m_pGuildLayer;
	/*
	*确认索引
	*/
	enum CONFIRM_INDEX_GUILDLAYER
	{
		CONFIRM_INDEX_JION_GUILD,
		CONFIRM_INDEX_EQIT_GUILD,
		CONFIRM_INDEX_DISMISS_GUILD,
		CONFIRM_INDEX_NOT_ACCESS,
		CONFIRM_INDEX_ACCESS,
		CONFIRM_INDEX_ACCEPT_JIONGUILD,
		CONFIRM_INDEX_REFUST_JIONGUILD,
		CONFIRM_INDEX_SAVE_SETADMIN,
		CONFIRM_INDEX_EXPAND_GUILD_CAPACITY,
		CONFIRM_INDEX_REMOVE_GUILDMANAGR,
		CONFIRM_INDEX_BLOCK_ADD_FRIEND,
		CONFIRM_INDEX_STRANGER_ADD_FRIEND,
		CONFIRM_INDEX_REMOVE_FRIEND,
		CONFIRM_INDEX_FRIEND_BLOCK_GUILD,
		CONFIRM_INDEX_STRANGER_BLOCK_GUILD,
		CONFIRM_INDEX_GUILD_INVITE_GUILD,
		CONFIRM_INDEX_ACCEPT_INVITATION,
		CONFIRM_INDEX_REFUST_INVITATION,
	};
	/*
	* tableview的index，目前有两个地方有tableview
	*/
	enum TABLEVIEW_INDEX
	{
		TABLEVIEW_NONE,
		MY_GUILD_DETAILS,
		MY_GUILD_MANAGER
	};

	enum MY_GUILD_ACTION_INDEX
	{
		MY_GUILD_ACTION_NONE,
		MY_GUILD_DELETE_MEMBER,
		MY_GUILD_SET_ADMIN
	};

	/*
	*退出工会按钮Tag值
	*/
	enum QUIT_BUTTON_TAG
	{
		QUIT_GUILD = 100,
		DISMISS_GUID,
	};
	/*
	*个性签名TextField
	*/
	ui::EditBox  *m_pInputText;
	/*
	* 记录用户是否输入完成
	*/
	bool         bIsEnter;
	/*
	*数据
	*/
	ProtobufCMessage      *m_pResult;
	/*
	*公会申请列表数据
	*/
	GetApplyInfoResult    *m_pApplyInfoResult;
	/*
	*记录当前操作的索引值
	*/
	Widget                *m_pTempButton;
	/*
	*记录主界面索引按钮(公会列表、我的公会、公会申请、公会邀请)
	*/
	Widget                *m_pPreButton;
	/*
	*确认索引
	*/
	CONFIRM_INDEX_GUILDLAYER m_eConfirmIndex;
	/*
	*tableview的索引
	*/
	TABLEVIEW_INDEX m_eTableViewIndex;

	MY_GUILD_ACTION_INDEX m_eMyGuildActionIndex;
	/*
	*管理数据
	*/
	//Unreasonable_code_3;越界或浪费内存
	int                    m_gCompetence[1000];
	/*
	*保存管理权限数据
	*/
	//Unreasonable_code_4;越界或浪费内存
	int                    m_oldgCompetence[1000];
	/*
	*是否改变了该成员的管理权限
	*/
	//Unreasonable_code_5;越界或浪费内存
	bool                m_bChangeAdmin[1000];
	/*
	*创建公会Iconid
	*/
	int                    m_nGuildIconid;
	/*
	*公会列表页数显示索引
	*/
	int                    m_nPageIndex;
	/*
	*搜索页面公会列表显示索引
	*/
	int					   m_nPageSearchIndex;
	int					   m_nGuildMemberIndex;

	/*
	*公会列表的数据
	*/
	GetGuildListResult    *m_pGuildListResult;
	/*
	*搜索公会的公会列表的数据
	*/
	GetSearchGuildListResult *m_pSearchGuildListResult;
	//保存同势力的公会
	std::vector<GuildInfoDefine *>m_nSameForceGuilds;
	//我的公会信息
	GetMyGuildDetailsResult*m_pMyguildDetails;
	//我的公会管理信息
	GetGuildMemberPermissionResult*m_pMyguildManager;
};

#endif
