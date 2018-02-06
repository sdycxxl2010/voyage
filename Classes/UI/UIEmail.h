/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game email implementation class
 *
 */

#ifndef __EMAIL__LAYER__
#define __EMAIL__LAYER__

#include "UIBasicLayer.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

struct MAIL_ITEM_INFO
{
	int32_t type;
	int32_t itemId;
	int64_t itemAmount;
};

class UIEmail : public UIBasicLayer, TableViewDataSource,TableViewDelegate
{
public:
	//邮件的状态
	enum INBOX_MAIL_TYPE
	{
		INBOX_MAIL_TYPE_NORMAL = 0,
		INBOX_MAIL_TYPE_UNION = 1,
		INBOX_MAIL_TYPE_SYSTEM = 1<<1,
		INBOX_MAIL_TYPE_INBOX = 1<<2,
		INBOX_MAIL_TYPE_DRAFT = 1<<3,
		INBOX_MAIL_TYPE_SENT = 1<<4,
		INBOX_MAIL_TYPE_DELETED = 1<<5,
	};

	//确认索引
	enum CONFIRM_INDEX_EMAIL
	{
		CONFIRM_INDEX_DELETE_INBOX_EMAIL,
		CONFIRM_INDEX_DELETE_SYSTEM_EMAIL,
		CONFIRM_INDEX_DELETE_FRIEND,
		CONFIRM_INDEX_ADD_FRIEND,
		CONFIRM_INDEX_BLOCK_FRIEND,
		CONFIRM_INDEX_BLOCK_STRANGER,
		CONFIRM_INDEX_EMAIL_INVITE_GUILD,
		CONFIRM_INDEX_DELETE_ALL_INBOX_EMAIL,
		CONFIRM_INDEX_DELETE_ALL_SYSTEM_EMAIL
	};
	enum EMAIL_PAGE
	{
		PAGE_INBOX_INDEX = 1,
		PAGE_SYSTEM_INDEX,
		PAGE_BATTLELOG_INDEX
	};

	UIEmail();
	~UIEmail();
	void onEnter();
	void onExit();

	static UIEmail*  getInstance()
	{
		if (m_pEmailLayer == nullptr)
		{
			m_pEmailLayer = new UIEmail;
		}
		return m_pEmailLayer;
	}
	//打开邮件界面
	void openEmailView(Node *parent = nullptr, int cameraMask = 1);  
	//默认的button相应 
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);         
	//收信箱button相应  
	void inboxButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);       
	//系统邮箱的button相应 
	void systemButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);	
	//发送邮件的button相应 	
	void composeButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);	
	//数字键盘操作的button相应
	void numPadButtonEvent(Ref *pSender, Widget::TouchEventType TouchType);

	//接收服务器信息
	void onServerEvent(struct ProtobufCMessage* message,int msgType);

	//左侧button上的邮箱里没有查看邮箱的个数
	void flushLeftButtonNum(EMAIL_PAGE nFlag);
	//收信箱界面             
	void openInboxView();  
	//系统邮箱界面                             
	void openSystemView();                              
	//战斗日志
	void openBattleLogView();

	//收信箱邮件中详情界面
	void openInboxReadView();	
	//发送邮件界面						
	void openComposeView();	
	//显示数字键盘							
	void showNumpad(); 
	//好友列表选择好友                                 
	void openChooseFirend(const GetFriendsListResult *pFriendsResult); 
	//选择物品界面   
	void openComposeEncloseView();		
	//选择物品个数界面						   
	void openCompostAmountView(const int nIndex);   
	//个数滑动条实现                      
	void ItemSilder(const int nNum);

	//发送邮件
	void sendMail();
	//向name发送邮件(如果是回复的邮件，则peername为被回复邮件的玩家名，否则为"")
	void sendEmailToPlayer(std::string name, std::string peername);

	//解析系统邮件的json文件
	void getContentParseJson(std::string &content,MailDefine *&mailDefine);  
	//系统邮件取出说明
	void systemItemTalkAllPosition(std::string &content,int type);    

	       
	//更改左侧按键的状态
	void changeMinorButtonState(Widget *target);   
	
	void moveEnd(){ m_bIsActionFlag = true; };
	//判断输入框是否为输入状态并修改输入框透明度;
	void update(float dt);   

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
	static UIEmail       *m_pEmailLayer;
	GetMailListResult       *m_pMailListResult;	
	GetPersonalItemResult   *m_pItemResult;
	//取回邮件的按钮
	Widget					*m_pTakeBtn;
	Widget                  *m_pTempButton;
	GetLootPlayerLogResult	*m_pBattleLogResult;
	//收件箱邮件信息
	std::vector<MailDefine*> m_vIntbox;    
	//系统箱邮件信息
	std::vector<MailDefine*> m_vSystem;	   

	std::vector<MailDefine*> m_vTakeAllMailId;
	
	//邮件索引
	int                      m_nEmailIndex;     
	//item 的类型
	int                      m_nItemType;       
	//item 的个数
	int						 m_nItemNum;		
	//item 的id
	int						 m_nItemId;			
	//item 的iid
	int						 m_nItemIid;
	//页数显示索引
	int                      m_nInboxPageIndex;  
	//页数显示索引
	int                      m_nSystemPageIndex;
	//记录偏移量
	Vec2                     m_nslider;
	//当前界面索引(收件箱、系统邮件)
	EMAIL_PAGE				 m_nPageIndex;
	//攻打海盗基地的银币奖励
	int					     m_nAgainstPirateRewardCoins;
	//攻打海盗基地的金币奖励
	int					     m_nAgainstPirateRewardGolds;
	//攻打海盗基地的水手币奖励
	int					     m_nAgainstPirateRewardSailorCoins;
	//是否是收件箱
	bool                     m_bIsIntbox;  	
	bool					 m_bIsTakeAll;
	//键盘控制
	bool                     m_bIsActionFlag; 
	//判断是否在村庄
	bool					 m_bIsVillage;
	//判断是否在敌对城市
	bool					 m_bIsEnemyCity;
	//确认对话索引
	CONFIRM_INDEX_EMAIL      m_eConfirmIndex; 
	//键盘输入的字符串
	std::string              m_padString;
};
#endif

