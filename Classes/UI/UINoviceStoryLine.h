/*
*  CopyRight (c) 
*  Created on: 2016年3月15日
*  Author: zhangting 
*  description: something about novice guide dialog
*/
//防止重定义
#ifndef __NOVICE__DIALOG__LAYER__
#define __NOVICE__DIALOG__LAYER__
#include "UIBasicLayer.h"
#include "UIStoryLine.h"

//新手引导对话索引

class UINoviceStoryLine : public UIBasicLayer
{
public:
	enum GUIDE_DIALOG
	{
		START_DIALOG = 1,
		SHIPYARD_ONE_DIALOG = START_DIALOG + 1,
		SHIPYARD_TWO_DIALOG = START_DIALOG + 2,
		TARVEN_ONE_DIALOG = START_DIALOG + 3,
		TARVEN_TWO_DIALOG = START_DIALOG + 4,
		TARVEN_THREE_DIALOG = START_DIALOG + 5,
		MARKET_BUY_DIALOG = START_DIALOG + 6,
		DOCK_ONE_DIALOG = START_DIALOG + 7,
		DOCK_TWO_DIALOG = START_DIALOG + 8,
		SAILING_GUIDE_DIALOG = START_DIALOG + 9,
		WAR_GUIDE_DIALOG = START_DIALOG + 10,
		WAR_FAIL_GUIDE_DIALOG = START_DIALOG + 11,
		PALACE_DIALOG = START_DIALOG + 12,
		MARKET_SELL_DIALOG = START_DIALOG + 13,
		CENTER_ONE_DIALOG = START_DIALOG + 14,
		CENTER_TWO_DIALOG = START_DIALOG + 15,
		CENTER_THREE_DIALOG = START_DIALOG + 16
	};
public:	
	static UINoviceStoryLine* GetInstance();
	//剧情播放最先调用的方法 新手引导剧情
	bool onGuideTaskMessage(int dialogId, int flags);

	UINoviceStoryLine();
	~UINoviceStoryLine();
	void onEnter();
	void onExit();
	static UINoviceStoryLine* createNovice(Layer * parent);
	bool init();
	//默认的button相应 
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType) override; 
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	//创建guideWidget 设置其点击事件 并读取（主线对话或触发剧情）json文件
	void createLayer();
	//主线对话段落选择
	void stepGuideDialog(int eStep);

	//新手引导对话结束时删除层并且任务提交的方法
	void removeGuideWidget();
private:	
	
	struct DialogData{
		int personIdx;
		std::string dialog;
	};

	LayerColor		*guideWidget;
	Label		*titleTxt;
	Label		*txtTxt;
	Label		*branchText1;
	Label		*branchText2;
	ui::EditBox	*nameTxt;

	Sprite		*wineHouseBg;
	Sprite		*dockBg;
	Sprite		*shipyardBg;
	Sprite		*fightPic;
	Sprite		*dialogBg;
	Sprite		*AidePic;
	Sprite		*HeroPic;
	Sprite		*majorFigurePic;
	Sprite		*ruffianAPic;
	Sprite		*ruffianBPic;
	Sprite		*shipyardBossPic;
	Sprite		*supportingRolePic;
	Sprite		*captainPic;
	Sprite		*asideBg;
	Sprite		*dboxLeft;
	Sprite		*dboxRight;
	//对话点击提示
	Sprite		*anchr;
	Sprite		*maskPic;
	Sprite		*handPic;
	Sprite		*zhixianBgPic;
	Sprite		*nameListPic;

	Button		*branchButton1;
	Button		*branchButton2;
	Button		*btnSkip;
	Button		*btn_nameOk;
	DrawNode		*pStencil;
	//当前层的父类层
	Layer *m_pParent;
	static UINoviceStoryLine* m_pInstance;

	int		OFFSET;
	int		txtNum;
	int		branchTxtNum;
	int		lenNum;
	int		len;
	int		maxLen;
	//需要翻页时 当前文本需要翻页的剩余高度
	int		moreNum;
	int		plusNum;
	int		addjust;
	int		FontSize;
	int		IllustrationNum;
	//剧情对话索引
	int	  m_eStep;
	//对话文本实际一行的高度
	int		m_nOneLineHeight;

	int		skipNum;

	float	textTime;

	bool	lenBoo;
	//对话层是否可以点击 true不可点击 false可点击
	bool	layerFarmClickBoo;
	//当前点击直接显示的内容是否需要翻页 true翻页 false不翻页（翻页判断有两种 一种是直接显示内容然后翻页 另一种是逐字显示到翻页 逐字显示的翻页到下一页的时候还是逐字显示）
	bool	directShowBoo;

	//存储新手引导的json对话
	std::vector<DialogData*> m_dialogVector;
	std::vector<std::string> m_dialogNames;
	std::string	JOANNA;
	std::string	JOHN;
	std::string	mystr;
	PERSON_NAME m_eName;
	//存储当前(包括主线和触发剧情)对话json文件的逐条对话的名字
	std::vector<std::string> m_jsonName;
	//存储当前(包括主线和触发剧情)对话json文件的逐条对话的内容
	std::vector<std::string> m_jsonDialog;

	//解决切换场景崩溃bug
	void stopAllAction();

	//获取主线对话json内容 或触发剧情内容 以及名字替换
	void replaceJsonString();
	//创建酒馆背景
	void createWineHouseBg();
	//创建港口背景
	void createDockBg();

	//船坞背景
	void createShipyardBg();
	//西班牙打架
	void createFightBg();
	
	//创建对话通用背景
	void createDialogBg(std::string m_path);
	//创建副官
	void createAide();
	//创建玩家
	void createHero();
	//创建每个国家代表人物（亨利、卡特琳娜、德雷克、巴伦支、哥伦布）
	void creatMajorFigure(std::string m_path);

	//创建西班牙俩流氓
	void creatRuffian();
	//创建船坞船老板
	void createShipyardBoss();
	//创建富商甲（西班牙人物）、荷兰剧情和红胡子剧情的水手、热那亚高帽男、热那亚剧情中的西班牙王宫大臣、红胡子剧情的守卫
	void createSupportingRole(std::string m_path);
	//创建里斯本船长
	void createCaptain();
	
	//旁白对话背景
	void createAsideBg();
	//左对话背景
	void createDboxLeft();
	//右对话背景
	void createDboxRight();
	//点击提示图标
	void createAnchr();
	//新手引导剧情略过按钮
	void createButtonSkip();
	//对话文本
	void createText();
	//只有旁白时的对话文本
	void createAsideText();
	//创建有对话支线的按钮
	void createDialogBranchButton(int Num);
	//创建船坞给船命名界面
	void createNameBoat();

	//根据剧情索引创建每个国家主线剧情需要的人物 对话文本和背景 并开始对话
	void createGuideDialogAllNeed_1();
	void createGuideDialogAllNeed_2();
	void createGuideDialogAllNeed_3();
	void createGuideDialogAllNeed_4();
	void createGuideDialogAllNeed_5();

	//屏幕点击的方法
	void	noviceClickEvent();
	
	void buttonSkipEvent(Ref *pSender, Widget::TouchEventType TouchType);
	//命名完成按钮
	void buttonNameOkEvent(Ref *pSender, Widget::TouchEventType TouchType);
	//对话中支线的点击事件
	void branchClickEvent(Ref *pSender, Widget::TouchEventType TouchType);
	//战斗引导
	void createWarGuideHelp();
	//战斗引导点击事件
	void warStartEvent(Ref *pSender, Widget::TouchEventType TouchType);
	//海上引导
	void createSaillingHelp();
	//海上引导点击事件
	void startSailling(Ref *pSender, Widget::TouchEventType TouchType);

	//过滤点击步骤 
	void	dialogSentenceStepByStep(int Num);

	//每个国家男女版对话播放顺序 对话更替为下一句和直到结束删除对话层 传给服务器结束剧情
	void dialogSpeakOrder1_0();
	void dialogSpeakOrder2_1();
	void dialogSpeakOrder2_2();
	void dialogSpeakOrder3_0();
	void dialogSpeakOrder4_0();
	void dialogSpeakOrder5_1();
	void dialogSpeakOrder5_2();


	
	//设定播放对话定时器
	void	openText();
	//播放对话的定时器调用的方法，用来逐字显示对话
	void	showText(float f);

	//显示对话支线
	void showBranch();
	//显示对话
	void showDialog();
	
	//显示插画时定时器调用的函数
	void showIllustration(float f);
	
	//剧情对话人物出场 (为动作进行先后而调用)
	void personCome();

	//变量重置
	void reset();
};
#endif
