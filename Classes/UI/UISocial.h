/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game social implementation class
 *
 */

#ifndef __SOCIAL__LAYER__
#define __SOCIAL__LAYER__

#include "UIBasicLayer.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

class UISocial : public UIBasicLayer, TableViewDataSource, TableViewDelegate,ui::EditBoxDelegate
{
public:
	struct CHATMSG
	{
		std::string name;
		std::string channel;
		int64_t iconId;
		int64_t cid;
		int64_t nation;
	};
	
	struct CHAT
	{
		std::string name;
		std::string content;
		std::string channel;
	};

	/*
	*确认索引
	*/
	enum CONFIRM_INDEX_SOCIALLAYER
	{
		CONFIRM_INDEX_ACCEPT_ADDFRIEND,
		CONFIRM_INDEX_REFUSAL_ADDFRIEND,
		CONFIRM_INDEX_STRANGER_ADDFRIEND,
		CONFIRM_INDEX_BLOCK_ADDFRIEND,
		CONFIRM_INDEX_REMOVEFRIEND,
		CONFIRM_INDEX_FRIEND_BLOCK,
		CONFIRM_INDEX_STRANGER_BLOCK,
		CONFIRM_INDEX_FRIEND_NOT_ONLINE,
		CONFIRM_INDEX_SOCIAL_INVITE_GUILD,
		CONFIRM_INDEX_BLOCK_REMOVE,
	};
	/*
	*私聊的状态
	*/
	enum SENT_MESSAGE_TO_PRIVATE
	{
		//没开始私聊
		STATE_NONE,
		//点开列表,第一次私聊
		STATE_FIRST,
		//多次私聊以后的状态
		STATE_AFTER,
	};
	enum FRIEND_PAGE
	{
		//我的好友
		PAGE_MY_FRIEND = 1,
		//好友拉黑
		PAGE_FRIEND_BLACK,
		//好友申请
		PAGE_FRIEND_APPLICATION,
	};
public:
	UISocial();
	~UISocial();
	void onEnter();
	void onExit();
	
	static UISocial* getInstance()
	{
		if (m_pSocialLayer == nullptr)
		{
			m_pSocialLayer = new UISocial;
		}
		return m_pSocialLayer;
	}
	void reset();
	void openSocialLayer();
	/*
	*调出聊天窗口
	*/
	void setChatCall(std::string name = "");
	/*
	*调出好友列表窗口
	*/
	void setFirendCall();
	/*
	*海上界面显示设置
	*/
	void showChat(const int cameraMask = 1);
	//跳转到Facebook邀请界面
	void setFaceBookInvite();
	/*
	*聊天内容
	*/
	std::vector<CHAT> getSocialChat(){ return m_vChat; };
	
	int                     m_nChatIndex;
private:
	std::vector<CHAT>		m_vChat;
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*好友列表的button相应 
	*/
	void friendListButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*好友黑名单的button相应 
	*/
	void blockListAndRequestButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*聊天窗口的button相应
	*/
	void chatEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*聊天窗口左右箭头显示
	*/
	void chatButtonEvent(Ref *pSender,ScrollviewEventType type);
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage *message,int msgType);
	/*
	*接收聊天服务器信息数据
	*/
	void onChatMessage(char *chatMsg);
	/*
	*刷新朋友列表界面
	*参数 pFriendsResult:好友列表数据
	*/
	void flushFirendList(const GetFriendsListResult *pFriendsResult);
	/*
	*刷新黑名单列表界面
	*参数 pBlackResult:黑名单列表数据
	*/
	void flushBlackList(const GetBlackListResult *pBlackResult);
	/*
	*刷新好友申请列表界面
	*参数 pFriendsReqResult:好友申请列表数据
	*/
	void flushApplication(const GetFriendsReqListResult *pFriendsReqResult);
	/*
	*刷新好友申请button的个数
	*参数 nNum:未处理的申请个数
	*/
	void flushApplicationNum(const int nNum);
	/*
	*更改按钮的状态()
	*参数  target:当前点击的按钮
	*/
	void changeMainButton(Widget *target);
	/*
	*刷新频道聊天内容
	*/
	void flushChatList();
	/*
	*刷新频道内的玩家列表
	*/
	void flushChatFirendList();
	/*
	*频道内发送信息
	*/
	void sendChat();
	/*
	*刷新聊天窗口的上方的按钮
	*/
	void flushChatButton();
	/*
	*刷新左右按键的显示
	*/
	void flushChatLifeAndRight(int nIndex);
	/*
	*刷新频道内的最近发言的玩家
	*/
	void getLatestSpeakers(const float fTime);

	//对聊天本地存储的操作
	void addPublic_zh(std::string name, std::string content, int nation, bool is_notice);
	void addPublic(std::string name, std::string content, int nation, bool is_notice);
	void addCountry(std::string name, std::string content, int nation);
	void addZone(std::string name, std::string content, int nation);
	void addGuild(std::string name, std::string content, int nation, bool is_notice);
	void addPrivate(int id, std::string name, std::string content, int nation);
	/*
	* 输入框编辑时会调用的
	*/
	virtual void editBoxReturn(ui::EditBox* editBox);
	virtual void editBoxEditingDidBegin(ui::EditBox* editBox);
	virtual void editBoxEditingDidEnd(ui::EditBox* editBox);
	virtual void editBoxTextChanged(ui::EditBox* editBox, const std::string& text);
	/*
	*设置聊天窗口是否有公会
	*/
	void setGuildChatButton();
	/*
	*查找好友
	*/
	void showSearchUserList(SearchUserByNameResult *searchResult);
	/*
	*输入框点击事件
	*/
	void textFiledEvent(Ref *target, int type);
	//私聊的数据
	std::vector<std::string>m_vPrivateName;
	/*
	*输入框占位字符透明度变化
	*/
	void update(float delta);
	//打开faceBook邀请  isShow：是否显示该界面
	void openFaceBookInvite(bool isShow =false);

	/* 好友列表 */
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
	static UISocial     *m_pSocialLayer;

	Widget					*m_pMainButton;
	Widget					*m_pTempButton;
	Widget					*m_pPreChannelBtn;
	/*
	*频道
	*/
	int                     m_nChannelType;
	/*
	*上一次关闭聊天框时所作频道
	*/
	int                     m_nLastChannelType;
	/*
	*记录是否收起好友列表
	*/
	bool                    m_bisFriend;

	//为送礼记录cell的index
	int						m_nForGiftIndex;
	bool                    m_bIsSendGift;

    //为记录cell的index
	int                     m_ForReqIndex;
	bool               	    m_bIsReqList;

	//为黑名单的cell记录index
	int                     m_ForBlockIndex;
	bool                    m_bIsBlockList;

	//为好友的cell记录index
	int                     m_ForFriendIndex;
	bool                    m_bIsFriendList;
	//记录偏移量
	Vec2                     m_nslider;
	/*
	*最近发送的消息信息时间
	*/
	std::vector<float>	m_nLastTime;
	/*
	*确认索引
	*/
	CONFIRM_INDEX_SOCIALLAYER  m_eConfirmIndex;
	/*
	*玩家信息
	*/
	std::vector<CHATMSG>    m_vHeroMsg;
	/*
	*黑名单玩家
	*/
	std::vector<std::string> m_vBlackListName;
	/*
	*发送的信息间隔
	*/
	int64_t sendIntervalTime;
	/*
	*记录类型
	*/
	int n_type;
	/*
	*好友
	*/
	Widget					*m_pFirendButton;
	/*
	*好友列表
	*/
	GetFriendsListResult	*m_pFriendsResult;
	/*
	*私聊的标志
	*/
	bool m_IsSendPrivate;
	/*
	*私聊时我向对方发送
	*/
	bool m_IsMeSendToPrivate;
	/*
	*第一次私聊打开窗口,右边列表清空，直到有一方发送消息时，列表显示信息
	*/
	int m_nFirstSendToPrivate;
	/*
	*玩家信息(做缓存cache)
	*/
	std::vector<CHATMSG>    m_vHeroListMsg[5];
	/*
	*玩家信息(做缓存cache)
	*/
	std::vector<CHATMSG>    m_vTempHeroListMsg[5];
	/*
	*是否缓存了信息
	*/
	bool   m_bSaveHeromsg;
	/*
	*好友界面左侧按钮的标志
	*/
	int m_nPageIndex;
	//拉黑列表
	GetBlackListResult *m_pBlackListResult;
	//申请列表
	GetFriendsReqListResult *m_pFriendsReqListResult;
};

#endif
