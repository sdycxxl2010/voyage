/*
*  CopyRight (c) ....
*  Created on: 2015年06月25日
*  Author:Cheng yuan
*  description  新手指引
*/

#ifndef  __GuideLayer_
#define  __GuideLayer_
#include "cocos2d.h"
#include "UIBasicLayer.h"
USING_NS_CC;
class UIStoryLine;
class UIGuidePalace;
class UIMain;
class UINoviceStoryLine;
class UIGuideMain :public UIBasicLayer
{
	//任务指示精灵的Tag值
	enum SING_TAG
	{
		HAND_TAG = 111,
		MASK_TAG,
	};
	//第二部分新手引导
	enum TASK_STAGE
	{

		//任务开始
		GUIDE_TASK_BEGAN = 4,
		//宫殿任务完成
		PALACE_TASK_COMPLETE,
		//交易所任务完成
		EXCHANGE_TASK_COMPLETE,
	};
	//第一部分新手引导
	enum NEW_TASK_STAGE
	{
		//任务开始
		NEW_GUIDE_TASH_BEGAN = 0,
		//船坞任务完成
		NEW_SHIPYARD_TASK_COMPLETE,
		//酒馆任务完成
		NEW_BAR_TASK_COMPLETE,
		//交易所任务完成
		NEW_EXCHANGE_TASK_COMPLETE,
	};
    //新手引导进度
	enum GUIDE_STAGE
	{
		//起始任务
		TASK_BEGIN= 1,
		//打开任务界面
		OPEN_TASK ,
		//领取奖励确定
		REWARD_CONFIRM,
		//领取奖励
		GET_REWARD ,
		//任务结束
		TASK_END,
		//弹出NPC对话
		NPC_FADEIN,
		//打开主城界面
		OPEN_PALACE,
		//交易起始任务
		SELL_TASK_BEGIN ,
		//打开任务界面
		OPEN_LISENCE_REWARD_TASK,
		//交易许可证任务奖励确定
		LISENCE_REWARD_CONFIRM,
		//领取许可证任务奖励
		GET_LISEBCE_REWARD,
		//许可证任务结束
		LISEBCE_TASK_END,
		//弹出NPC对话
		NPC_SELL_FADEIN,
		//打开交易界面
		OPEN_EXCHANGE,
		//交易任务 
		SELL_TASK_END,
		//打开任务界面
		OPEN_END_SELL_TASK,
		//确认交易奖励
		CONFIRM_END_SELL_REWARD,
		//领取交易任务
		GET_END_SELL_TASK_REWARD,
		//技能引导任务对话
		SKILL_GUIDE_DIALOG,
		//打开个人中心界面,
		FOCUS_CHARACTER_CENTER,
	};
	//第一部分新手引导
	enum GUIDE_STAGE_1
	{
		//任务开始
		NEW_TASK_BEGAN = 1,
		//打开任务界面
		NEW_OPEN_TASK,
		//关闭任务界面
		NEW_TASK_CLOSE,
		//打开船坞领取奖励界面
		NEW_OPEN_SHIPYARD_TASK,
		//领取船坞任务奖励
		NEW_GET_SHIPYARD_TASK_REWARD,
		//确认领取奖励
		NEW_CONFIRM_GET_SHIPYARD_TASK_REWARD,
		//船坞任务结束
		NEW_SHIPYARD_TASK_END,
		//酒吧任务开始
		NEW_BAR_TASK_BEGIN,
		//弹出NPC对话
		NEW_NPC_FADEIN,
		//打开酒吧界面
		NEW_OPEN_BAR,
		//打开酒吧任务界面
		NEW_OPEN_BAR_TASK,
		//领取酒吧任务奖励,
		NEW_GET_BAR_TASK_REWARD,
		//确认领取酒吧任务奖励
		NEW_CONFIRM_GET_BAR_TASK_REWARD,
		//酒吧任务结束
		NEW_BAR_TASK_END,
		//交易所任务开始
		NEW_EXCHANGE_TASK_BEGIN,
		//弹出NPC对话
		NEW_EXCHAGE_NPC_FADEIN,
		//打开交易所界面
		NEW_OPEN_EXCHANGE,
		//打开领取交易所任务奖励界面
		NEW_OPEN_EXCHANGE_TASK,
		//领取交易所任务奖励
		NEW_GET_EXCHANGE_TASK_REWARD,
		//确认领取交易所任务奖励
		NEW_CONFIRM_GET_EXCHANGE_TASK_REWARD,
		//交易所任务结束
		NEW_EXCHAGE_TASK_END,
		//海上任务开始
		NEW_SEA_TASK_BEGIN,
		//弹出NPC对话
		NEW_SEA_NPC_FADEIN,
		//打开港口界面
		NEW_OPEN_DOCK,
		
	};
	struct DIALOGDATA//展示文字
	{
		int personIdx;
		std::string dialog;
	};
	struct REWARD_GOODS//任务奖励物品信息 
	{
		int goodsnum;//奖励物品的个数
		int type_1;//第一个奖励物品的类型
		int type_2;//第二个奖励物品的类型
	};
  struct REWARD_INFO //领取奖励信息
	{
		int exp;//经验
		int coin;//银币
		int v_p;//v票
		int r_p;//荣誉值
		REWARD_GOODS * goods;//任务奖励物品
	};



public:
	UIGuideMain();
	~UIGuideMain();
	bool init();
	void onEnter();
	void onExit();
	static UIGuideMain * create();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	//判断是否点击过船坞
	bool	m_bIsClickedShipyard;

	/**
	* 引导
	*/
	void guide();
	/**
	*将所有按钮设置为不可点击
	*@param node按钮父节点
	*/
	void setButtonsDisable(Node * node);

	/**
	*引导模式引导玩家下一步需要点击那个按钮
	*@param psender下个任务需要点击的按钮
	*/
	void focusOnButton(Node * psender);

	/**
	*声望等级提升弹出界面
	*/
	void flushPrestigeLevelUp();

	/**
	*确认当前引导阶段
	*/
	void confirmGuideStage();
private:

	/**
	*当前游戏引导进度
	*/
	void checkCurStage();
	/**
	*新手引导初始化界面
	*
	*/
	void initf(float dt);

	/**
	* 对话打字机效果
	*/
	void textShow(float dt);



	/**
	*第一部分新手引导
	*/
	void partOneGuide(int stage);
	
	/**
	*按钮名对应的点击事件
	*/
	void callEventByName(Widget* psender,std::string name);

	/**
	*对话淡入
	*/
	void chatFadeIn();

	/**
	*对话淡出
	*/
	void chatFadeOut();

	/**
	*船锚动画
	*/
	void anchorAction();

	/**
	*打开宫殿
	*/
	void openPalace();

	/**
	*显示玩家信息
	*/
	void updateUserInfo();

	/**
	*新任务提示
	*flag: 新任务提示显示/隐藏
	*/
	void showMark(bool flag);

	/**
	*打开任务界面
	*/
	void openTaskLayer(int chapter);

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
	*城镇背景转换
	*/
	void showBackGround();

	/**
	*任务文字提示框闪烁
	*/
	void taskBlink();

	/**
	*任务界面的数据
	*index : 任务界面的数据索引
	*/
	void taskShow(int index);

	/**
	*打开任务奖励领取确定界面
	*/
	void openRewardConfirm();

	/**
	*关闭任务奖励领取确定界面
	*/
    void closeRewadConfirm();

	/**
	*初始化确定领取任务界面信息
	*reward: 当前任务的奖励数据
	*/
	void initConfirmRewardInfo(REWARD_INFO * reward);

	/**
	*初始化任务奖励数据
	*index : 任务奖励数据索引
	*/
	void initRewardInfo(int index);


	
	/**
	*奖励提醒提醒
	*flag: 奖励提醒显示/不显示
	*/
	void  showNotify(bool flag);

	/**
	*角色升级动画
	*level: 界面要显示的等级 
	*/
	void roleLevelUp(int level);

	/**
	*保存玩家游戏数据0 exp 1 rexp 2 coin 3. vp  
	*/
	void saveUserData(int64_t a[4]);

	/**
	*判断当前国家的json文件
	*/
	std::string confirmCityJson();
	/**
	*判断当前国家的配置文件
	*/
	std::string  confirmCityDisplayJson();

	/**
	*确定当前城镇的PalaceHall按钮的样式
	*/
	void villageButtonShow();

	/**
	*对话拆分
	*param 需要拆分的对话
	*/
	void cutChat(std::string content);

	/**
	*文本替换
	*text: 需要替换[aidename]字段的文本
	*/
	void replaceText(std::string &text);
private:
	//对话的长度
	int t_size;
	//当前处于第几段对话
	int m_chatnum;
	//对话总数
	int m_allChatNum;
	//引导进度
	int m_guidestage;
	//当前对话是否说完
	bool m_cursay;
	//所有对话完成
	bool m_chatcomplete;
	//新任务提醒
	bool m_taskflag;
	//任务奖励界面信息索引
	int  m_infoIndex;
	//角色等级
	int  m_roleLevel;
	//角色声望等级
	int  m_role_rLevel;
	//对话长度
	int  m_contentLength;
	//是否升级
	bool m_isLevelup;
	//得到的经验
	int64_t  m_receiveexp;
	//得到的声望
	int64_t  m_receiveRp;
	//得到的银币
	int64_t  m_receiveCoin;
	//得到的V票
	int64_t  m_receiveVp;
	//是否是第一次到达城镇
	bool  m_isfirstIn;
	//任务界面
	Widget * m_tasklayer;
	//所有对话
	std::vector<std::string>talks;
	//NPC对话
	std::string m_talk;
	//小手
	Sprite * sprite_hand;
	//任务提示框
	cocos2d::ui::Scale9Sprite * m_draw;
	//对话信息和奖励信息
	std::vector<DIALOGDATA*> m_dialogdatas;
	//存储json文件里对话的名字信息
	std::vector<std::string> m_names;
	//控件展示数据
	std::vector<std::string> m_dispalys;
	//任务奖励信息
	REWARD_INFO * m_rewardInfo;
	//城市信息数据
	GetCurrentCityDataResult * m_cityResult;
	//NPC提示语
	std::vector<std::string> m_words;
	//旁白界面弹出文字的背景
	Sprite* m_chatBg;
	//对话向后移动的长度
	int m_lenAfter;
	//打字机效果对话出现速度
	float m_textTime;
	//任务界面已经打开
	bool m_taskAlreadyShow;
	//加载界面
	UILoadingIndicator *m_LoadingLayer;
	//对话界面
	UINoviceStoryLine * m_dialogLayer;
};
#endif
