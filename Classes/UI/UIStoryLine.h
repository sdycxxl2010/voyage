/*
*  CopyRight (c) 
*  Created on: 2015年6月26日
*  Author: zhangting 
*  description: something about novice guide
*/
//防止重定义
#ifndef __NOVICE__LAYER__
#define __NOVICE__LAYER__
#include "UIBasicLayer.h"
//葡萄牙主线任务索引
enum PATH_DIALOG
{
	SEABOAT_PATH,
	FINDMADELA_PATH,
	FINDFODEJIAO_PATH,
	CEUTABATTLE_PATH,
	ATTACK_SPAIN_PATH,
	ENGLAND_VS_SPAIN_PATH,
	JUDGE_BG_PATH,
	BOATTEAM_PATH,
	CATEWINEHOUSE_PATH,
	CATEBEDROOM_PATH,
	FINDEXINDIDAO_PATH,
	FINDSVALBARD_PATH,
	REDBEARDFLEETVOYAGE_PATH,
	REDBEARDWAR1,
	REDBEARDWAR2,
	REDBEARDPRISON_PATH,
	FIGHT_PATH,

	HENRY_PATH,
	CATALINA_PATH,
	DEREK_PATH,
	BARENTS_PATH,
	COLUMBUS_PATH,
	SPAIN_KING_PATH,
	ENGLAND_QUEEN_PATH,
	PASENGER_A_PATH,
	PASENGER_B_PATH,
	MERCHANT_PATH,
	SAILOR_PATH,
	TOPHATMAN_PATH,
	CHANCELLOR_PATH,
	KINDNESS_MERCHANT_PATH,
	REDBEARD_PATH,
	CAPTAIN_PATH,
	RUFFIAN_1_PATH,
	RUFFIAN_2_PATH,

	ASIDE_PATH,
	DBOXLEFT_PATH,
	DBOXRIGHT_PATH,
	ANCHR_PATH,
	BUTTON_SKIP_PATH,
	MASK_PATH,
	HANDPIC_PATH,
	BRANCH_BG_PATH,
	GIFT_PATH
};
enum PERSON_NAME
{
	AIDE,
	HERO,
	MAJORFIGURE,
	OFFICER,
	PASENGERA,
	SAILOR,
	SOLDIER,
	SHIPYARDBOSS
};
static const std::string DIALOG_PATH[] = {
								"res_lua/seaboat.jpg",
								"res_lua/findMadela.jpg",
								"res_lua/findFodejiao.jpg",
								"res_lua/CeutaBattle.jpg",
								"res_lua/attackXibanya.jpg",
								"res_lua/yinxiWar.jpg",
								"res_lua/shenpan.jpg",
								"res_lua/boatteam.jpg",
								"res_lua/catlinnaWineHouse.jpg",
								"res_lua/catlinnaBedroom.jpg",
								"res_lua/findXindidao.jpg",
								"res_lua/findSvalbard.jpg",
								"res_lua/redBeardFleetVoyage.jpg",
								"res_lua/redBeardWar1.jpg",
								"res_lua/redBeardWar2.jpg",
								"res_lua/redBeardPrison.jpg",
								"res_lua/fight.png",

								"res/npc/drama/Henry.png",
								"res/npc/drama/Catalina.png",
								"res/npc/drama/Derek.png",
								"res/npc/drama/Barents.png",
								"res/npc/drama/Columbus.png",
								"res/npc/king/Spain.png",
								"res/npc/king/Elizabeth.png",
								"res/npc/chief/civilian_1.png",
								"res/npc/chief/civilian_2.png",
								"res/npc/drama/Merchant.png",
								"res/npc/drama/Sailor.png",
								"res/npc/drama/TopHatMan.png",
								"res/npc/drama/Chancellor.png",
								//好心的商人 暂时用副官代替
								"res/npc/chief/npc_6_1.png",
								"res/npc/partner/red_beard.png",
								"res/npc/drama/Captain.png",
								"res/npc/chief/ruffian_1.png",
								"res/npc/chief/ruffian_2.png",

								"login_ui/start_720/aside_bg.png",
								"res_lua/dboxLeft.png",
								"res_lua/dboxRight.png",
								"res_lua/anchr.png",
								"login_ui/start_720/skip_btn.png",
								"res_lua/mask.png",
								"login_ui/start_720/hand_icon.png",
								"res_lua/dialogzhixian.png",
								"login_ui/city_720/libao.png"
};

class UIStoryLine : public UIBasicLayer
{
public:
	enum PORTUGAL_TASK
	{
		SALLING_GUID = 1,
		WAR_FAIL_GUID = SALLING_GUID - 1,
		WAR_GUID = SALLING_GUID + 1,
		PORTUGAL_TASK_THREE_GOAL_ONE_HINT1 = SALLING_GUID + 2,
		PORTUGAL_TASK_THREE_GOAL_ONE_HINT2 = SALLING_GUID + 3,
		PORTUGAL_TASK_THREE_GOAL_ONE_DOCK1 = SALLING_GUID + 4,
		PORTUGAL_TASK_THREE_GOAL_ONE_DOCK2 = SALLING_GUID + 5,
		PORTUGAL_TASK_THREE_GOAL_TWO1 = SALLING_GUID + 6,
		PORTUGAL_TASK_THREE_GOAL_TWO2 = SALLING_GUID + 7,
		PORTUGAL_TASK_THREE_GOAL_THREE_HINT = SALLING_GUID + 8,
		PORTUGAL_TASK_THREE_GOAL_THREE = SALLING_GUID + 9,
		PORTUGAL_TASK_THREE_GOAL_FOUR_HINT = SALLING_GUID + 10,
		PORTUGAL_TASK_THREE_GOAL_FIVE_HINT = SALLING_GUID + 11,
		PORTUGAL_TASK_THREE_GOAL_FIVE = SALLING_GUID + 12,
		PORTUGAL_TASK_FOUR_GOAL_ONE = SALLING_GUID + 13,
		PORTUGAL_TASK_FOUR_GOAL_TWO_HINT = SALLING_GUID + 14,
		PORTUGAL_TASK_FOUR_GOAL_THREE_HINT1 = SALLING_GUID + 15,
		PORTUGAL_TASK_FOUR_GOAL_THREE_HINT2 = SALLING_GUID + 16,
		PORTUGAL_TASK_FOUR_GOAL_FOUR_HINT1 = SALLING_GUID + 17,
		PORTUGAL_TASK_FOUR_GOAL_FOUR_HINT2 = SALLING_GUID + 18,
		PORTUGAL_TASK_FOUR_GOAL_FIVE = SALLING_GUID + 19,
		PORTUGAL_TASK_FOUR_GOAL_FIVE_FINDEMADEIRA = SALLING_GUID + 20,
		PORTUGAL_TASK_FIVE_GOAL_ONE = SALLING_GUID + 21,
		PORTUGAL_TASK_FIVE_GOAL_TWO_HINT = SALLING_GUID + 22,
		PORTUGAL_TASK_FIVE_GOAL_THREE_HINT = SALLING_GUID + 23,
		PORTUGAL_TASK_FIVE_GOAL_FOUR_HINT = SALLING_GUID + 24,
		PORTUGAL_TASK_FIVE_GOAL_FOUR = SALLING_GUID + 25,
		PORTUGAL_TASK_FIVE_GOAL_FOUR_FINDVERDE = SALLING_GUID + 26,
		PORTUGAL_TASK_FIVE_GOAL_FIVE = SALLING_GUID + 27
	};
public:	
	static UIStoryLine* GetInstance();
	//剧情播放最先调用的方法 主线剧情
	bool onMainTaskMessage(int dialogId, int flags);
	//剧情播放最先调用的方法 触发剧情
	bool onTriggerDialogMessage(int triggerDialogId, int flags);
	//剧情播放最先调用的方法 小伙伴剧情
	bool onPartnerDialogMessage(int partnerDialogId, int flags);

	UIStoryLine();
	~UIStoryLine();
	void onEnter();
	void onExit();
	static UIStoryLine* createNovice(Layer * parent);
	bool init();
	//默认的button相应 
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType) override; 
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	//创建guideWidget 设置其点击事件 并读取（主线对话或触发剧情）json文件
	void createLayer();
	//主线对话段落选择
	void stepDialog(int eStep);
	//触发剧情对话段落选择
	void stepTriggerDialog(int eStep);
	//小伙伴剧情对话段落选择
	void stepPartnerDialog(int eStep);

	//解决切换场景崩溃bug
	void stopAllAction();
private:	
	
	//西班牙主线任务索引
	enum SPAIN_TASK
	{
		SPAIN_TASK_THREE_GOAL_ONE_HINT = SALLING_GUID + 2,
		SPAIN_TASK_THREE_GOAL_ONE = SALLING_GUID + 3,
		SPAIN_TASK_THREE_GOAL_TWO = SALLING_GUID + 4,
		SPAIN_TASK_THREE_GOAL_THREE_HINT = SALLING_GUID + 5,
		SPAIN_TASK_THREE_GOAL_THREE = SALLING_GUID + 6,
		SPAIN_TASK_THREE_GOAL_FOUR_HINT = SALLING_GUID + 7,
		SPAIN_TASK_THREE_GOAL_FIVE_HINT = SALLING_GUID + 8,
		SPAIN_TASK_THREE_GOAL_FIVE = SALLING_GUID + 9,
		SPAIN_TASK_FOUR_GOAL_ONE = SALLING_GUID + 10,
		SPAIN_TASK_FOUR_GOAL_TWO_HINT = SALLING_GUID + 11,
		SPAIN_TASK_FOUR_GOAL_THREE_HINT1 = SALLING_GUID + 12,
		SPAIN_TASK_FOUR_GOAL_THREE_HINT2 = SALLING_GUID + 13,
		SPAIN_TASK_FOUR_GOAL_FOUR_HINT1 = SALLING_GUID + 14,
		SPAIN_TASK_FOUR_GOAL_FOUR_HINT2 = SALLING_GUID + 15,
		SPAIN_TASK_FOUR_GOAL_FIVE1 = SALLING_GUID + 16,
		SPAIN_TASK_FOUR_GOAL_FIVE2 = SALLING_GUID + 17,
		SPAIN_TASK_FIVE_GOAL_ONE = SALLING_GUID + 18,
		SPAIN_TASK_FIVE_GOAL_TWO_HINT = SALLING_GUID + 19,
		SPAIN_TASK_FIVE_GOAL_THREE_HINT = SALLING_GUID + 20,
		SPAIN_TASK_FIVE_GOAL_THREE = SALLING_GUID + 21,
		SPAIN_TASK_FIVE_GOAL_FOUR = SALLING_GUID + 22,
		SPAIN_TASK_FIVE_GOAL_FIVE = SALLING_GUID + 23
	};
	//英国主线任务索引
	enum ENGLAND_TASK
	{
		ENGLAND_TASK_THREE_GOAL_ONE_HINT = SALLING_GUID + 2,
		ENGLAND_TASK_THREE_GOAL_ONE = SALLING_GUID + 3,
		ENGLAND_TASK_THREE_GOAL_TWO = SALLING_GUID + 4,
		ENGLAND_TASK_THREE_GOAL_THREE_HINT = SALLING_GUID + 5,
		ENGLAND_TASK_THREE_GOAL_FOUR_HINT = SALLING_GUID + 6,
		ENGLAND_TASK_THREE_GOAL_FIVE_HINT = SALLING_GUID + 7,
		ENGLAND_TASK_THREE_GOAL_FIVE = SALLING_GUID + 8,
		ENGLAND_TASK_FOUR_GOAL_ONE = SALLING_GUID + 9,
		ENGLAND_TASK_FOUR_GOAL_TWO_HINT = SALLING_GUID + 10,
		ENGLAND_TASK_FOUR_GOAL_THREE_HINT1 = SALLING_GUID + 11,
		ENGLAND_TASK_FOUR_GOAL_THREE_HINT2 = SALLING_GUID + 12,
		ENGLAND_TASK_FOUR_GOAL_FOUR_HINT1 = SALLING_GUID + 13,
		ENGLAND_TASK_FOUR_GOAL_FOUR_HINT2 = SALLING_GUID + 14,
		ENGLAND_TASK_FOUR_GOAL_FIVE = SALLING_GUID + 15,
		ENGLAND_TASK_FIVE_GOAL_ONE = SALLING_GUID + 16,
		ENGLAND_TASK_FIVE_GOAL_TWO_HINT = SALLING_GUID + 17,
		ENGLAND_TASK_FIVE_GOAL_THREE_HINT = SALLING_GUID + 18,
		ENGLAND_TASK_FIVE_GOAL_THREE = SALLING_GUID + 19,
		ENGLAND_TASK_FIVE_GOAL_FOUR = SALLING_GUID + 20,
		ENGLAND_TASK_FIVE_GOAL_FIVE = SALLING_GUID + 21
	};
	//荷兰主线任务索引
	enum NETHERLANDS_TASK
	{
		NETHERLANDS_TASK_THREE_GOAL_ONE_HINT = SALLING_GUID + 2,
		NETHERLANDS_TASK_THREE_GOAL_ONE = SALLING_GUID + 3,
		NETHERLANDS_TASK_THREE_GOAL_TWO = SALLING_GUID + 4,
		NETHERLANDS_TASK_THREE_GOAL_THREE_HINT = SALLING_GUID + 5,
		NETHERLANDS_TASK_THREE_GOAL_FOUR_HINT = SALLING_GUID + 6,
		NETHERLANDS_TASK_THREE_GOAL_FIVE_HINT = SALLING_GUID + 7,
		NETHERLANDS_TASK_THREE_GOAL_FIVE = SALLING_GUID + 8,
		NETHERLANDS_TASK_FOUR_GOAL_ONE = SALLING_GUID + 9,
		NETHERLANDS_TASK_FOUR_GOAL_TWO_HINT = SALLING_GUID + 10,
		NETHERLANDS_TASK_FOUR_GOAL_THREE_HINT1 = SALLING_GUID + 11,
		NETHERLANDS_TASK_FOUR_GOAL_THREE_HINT2 = SALLING_GUID + 12,
		NETHERLANDS_TASK_FOUR_GOAL_FOUR_HINT1 = SALLING_GUID + 13,
		NETHERLANDS_TASK_FOUR_GOAL_FOUR_HINT2 = SALLING_GUID + 14,
		NETHERLANDS_TASK_FOUR_GOAL_FIVE = SALLING_GUID + 15,
		NETHERLANDS_TASK_FIVE_GOAL_ONE = SALLING_GUID + 16,
		NETHERLANDS_TASK_FIVE_GOAL_TWO_HINT = SALLING_GUID + 17,
		NETHERLANDS_TASK_FIVE_GOAL_THREE_HINT = SALLING_GUID + 18,
		NETHERLANDS_TASK_FIVE_GOAL_FOUR_HINT = SALLING_GUID + 19,
		NETHERLANDS_TASK_FIVE_GOAL_FOUR = SALLING_GUID + 20,
		NETHERLANDS_TASK_FIVE_GOAL_FIVE = SALLING_GUID + 21
	};
	//热那亚主线任务索引
	enum GENOVA_TASK
	{
		GENOVA_TASK_THREE_GOAL_ONE_HINT = SALLING_GUID + 2,
		GENOVA_TASK_THREE_GOAL_ONE = SALLING_GUID + 3,
		GENOVA_TASK_THREE_GOAL_TWO = SALLING_GUID + 4,
		GENOVA_TASK_THREE_GOAL_THREE_HINT = SALLING_GUID + 5,
		GENOVA_TASK_THREE_GOAL_FOUR_HINT = SALLING_GUID + 6,
		GENOVA_TASK_THREE_GOAL_FIVE_HINT = SALLING_GUID + 7,
		GENOVA_TASK_THREE_GOAL_FIVE = SALLING_GUID + 8,
		GENOVA_TASK_FOUR_GOAL_ONE = SALLING_GUID + 9,
		GENOVA_TASK_FOUR_GOAL_TWO_HINT = SALLING_GUID + 10,
		GENOVA_TASK_FOUR_GOAL_THREE_HINT1 = SALLING_GUID + 11,
		GENOVA_TASK_FOUR_GOAL_THREE_HINT2 = SALLING_GUID + 12,
		GENOVA_TASK_FOUR_GOAL_FOUR_HINT = SALLING_GUID + 13,
		GENOVA_TASK_FOUR_GOAL_FIVE = SALLING_GUID + 14,
		GENOVA_TASK_FIVE_GOAL_ONE = SALLING_GUID + 15,
		GENOVA_TASK_FIVE_GOAL_TWO_HINT = SALLING_GUID + 16,
		GENOVA_TASK_FIVE_GOAL_THREE_HINT1 = SALLING_GUID + 17,
		GENOVA_TASK_FIVE_GOAL_THREE_HINT2 = SALLING_GUID + 18,
		GENOVA_TASK_FIVE_GOAL_FOUR_HINT = SALLING_GUID + 19,
		GENOVA_TASK_FIVE_GOAL_FOUR = SALLING_GUID + 20,
		GENOVA_TASK_FIVE_GOAL_FIVE = SALLING_GUID + 21,
		GENOVA_TASK_FOUR_GOAL_FIVE_HINT = SALLING_GUID + 22
	};
	
	enum TRIGGER_DIALOG
	{
		//首次接受赏金榜任务
		GIVING_GIFT_TO_BARGIRL = 1,
		//救济金
		DISTRIBUTE_MONEY = GIVING_GIFT_TO_BARGIRL + 1,
		//离线委托
		OPEN_OFFLINE_COMMISION = GIVING_GIFT_TO_BARGIRL + 2
	};
	enum DIALOG_TYPE
	{
		DIALOG_TYPE_MAIN_TASK = 0,
		DIALOG_TYPE_SMALL_STORY,
		DIALOG_TYPE_PARTNER_RED_BEARD
	};
	enum PARTNER_RED_BEARD_DIALOG
	{
		PARTNER_RED_BEARD_ZERO = 1,
		PARTNER_RED_BEARD_ONE = PARTNER_RED_BEARD_ZERO + 1,
		PARTNER_RED_BEARD_TWO = PARTNER_RED_BEARD_ZERO + 2,
		PARTNER_RED_BEARD_THREE_GOAL_ONE = PARTNER_RED_BEARD_ZERO + 3,
		PARTNER_RED_BEARD_THREE_GOAL_TWO = PARTNER_RED_BEARD_ZERO + 4,
		PARTNER_RED_BEARD_FOUR = PARTNER_RED_BEARD_ZERO + 5,
		PARTNER_RED_BEARD_FIVE_GOAL_ONE = PARTNER_RED_BEARD_ZERO + 6,
		PARTNER_RED_BEARD_FIVE_GOAL_TWO = PARTNER_RED_BEARD_ZERO + 7,
		PARTNER_RED_BEARD_FIVE_GOAL_THREE = PARTNER_RED_BEARD_ZERO + 8,
		PARTNER_RED_BEARD_SIX_GOAL_ONE = PARTNER_RED_BEARD_ZERO + 9,
		PARTNER_RED_BEARD_SIX_GOAL_TWO = PARTNER_RED_BEARD_ZERO + 10,
		PARTNER_RED_BEARD_SEVEN_GOAL_ONE = PARTNER_RED_BEARD_ZERO + 11,
		PARTNER_RED_BEARD_SEVEN_GOAL_TWO = PARTNER_RED_BEARD_ZERO + 12,
		PARTNER_RED_BEARD_SEVEN_GOAL_THREE = PARTNER_RED_BEARD_ZERO + 13,
		PARTNER_RED_BEARD_EIGHT_GOAL_ONE = PARTNER_RED_BEARD_ZERO + 14,
		PARTNER_RED_BEARD_EIGHT_GOAL_TWO = PARTNER_RED_BEARD_ZERO + 15,
		PARTNER_RED_BEARD_NINE_GOAL_ONE = PARTNER_RED_BEARD_ZERO + 16,
		PARTNER_RED_BEARD_NINE_GOAL_TWO_FAILED = PARTNER_RED_BEARD_ZERO + 17,
		PARTNER_RED_BEARD_NINE_GOAL_THREE_WIN = PARTNER_RED_BEARD_ZERO + 18,
	};
	struct DialogData{
		int personIdx;
		std::string dialog;
	};

	LayerColor		*guideWidget;
	Label		*titleTxt;
	Label		*txtTxt;
	Label		*branchText1;
	Label		*branchText2;

	Sprite		*wineHouseBg;
	Sprite		*seaBoatBg;
	Sprite		*dockBg;
	Sprite		*palaceBg;
	Sprite		*svalbardBg;
	Sprite		*judgePic;
	Sprite		*boatteamPic;
	Sprite		*cateWineHousePic;
	Sprite		*cateBedroomPic;
	Sprite		*CeutaBattlePic;
	Sprite		*MadeiraPic;
	Sprite		*CapeVerdePic;
	Sprite		*RedBeardFleetVoyagePic;
	Sprite		*RedBeardWar1Pic;
	Sprite		*RedBeardWar2Pic;
	Sprite		*dialogBg;
	Sprite		*AidePic;
	Sprite		*HeroPic;
	Sprite		*majorFigurePic;
	Sprite		*pasengerAPic;
	Sprite		*pasengerBPic;
	Sprite		*kingPic;
	Sprite		*supportingRolePic;
	Sprite		*OfficerPic;
	Sprite		*asideBg;
	Sprite		*dboxLeft;
	Sprite		*dboxRight;
	//对话点击提示
	Sprite		*anchr;
	Sprite		*maskPic;
	Sprite		*handPic;
	Sprite		*zhixianBgPic;

	Button		*branchButton1;
	Button		*branchButton2;
	Button		*branchButton3;
	DrawNode		*pStencil;
	//当前层的父类层
	Layer *m_pParent;
	static UIStoryLine* m_pInstance;

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
	//对话类型 默认0为主线剧情对话 1为触发剧情对话
	int dialogType;
	//主线剧情对话索引
	int	  m_eStep;
	//触发剧情对话索引
	int	  m_eTriggerStep;
	//小伙伴剧情对话索引
	int	  m_ePartnerStep;
	//对话文本实际一行的高度
	int		m_nOneLineHeight;

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
	PERSON_NAME	m_eName;
	//存储当前(包括主线和触发剧情)对话json文件的逐条对话的名字
	std::vector<std::string> m_jsonName;
	//存储当前(包括主线和触发剧情)对话json文件的逐条对话的内容
	std::vector<std::string> m_jsonDialog;

	

	//返回当前用到的lua json
	std::string findJsonFile(std::string name);
	//读取lua的json文件 （海上引导、战斗引导、战斗失败引导对话用到的）
	void	readJson();

	//获取主线对话json内容 或触发剧情内容 以及名字替换
	void replaceJsonString();
	//创建酒馆背景
	void createWineHouseBg();
	//创建海上背景
	void createSeaBoatBg();
	//创建港口背景
	void createDockBg();
	//欧洲王宫背景
	void createPalaceBg();

	//荷兰对话中 斯瓦尔巴特群岛插画背景
	void createSvalbardBg();
	//审判卡特琳娜插画
	void createJudgeBg();
	//舰队插画
	void createBoatteamBg();
	//卡特琳娜酒馆插画
	void createCateWineHouse();
	//卡特琳娜卧室插画
	void createCateBedroom();
	//创建休达战斗插画
	void createCeutaBattlePic();
	//创建发现马德拉插画
	void createMadeiraPic();
	//创建发现佛得角插画
	void createCapeVerdePic();
	//创建小伙伴红胡子剧情中航海舰队插画
	void createRedBeardFleetVoyagePic();
	//创建小伙伴红胡子剧情中海战1插画
	void createRedBeardWar1Pic();
	//创建小伙伴红胡子剧情中海战2插画
	void createRedBeardWar2Pic();
	
	//创建对话通用背景
	void createDialogBg(std::string m_path);
	//创建副官
	void createAide();
	//创建玩家
	void createHero();
	//创建每个国家代表人物（亨利、卡特琳娜、德雷克、巴伦支、哥伦布）
	void creatMajorFigure(std::string m_path);
	//创建路人甲乙 
	void creatPassenger(bool showB);
	//创建国王或王后
	void createKing(std::string m_path);
	//创建富商甲（西班牙人物）、荷兰剧情和红胡子剧情的水手、热那亚高帽男、热那亚剧情中的西班牙王宫大臣、红胡子剧情的守卫
	void createSupportingRole(std::string m_path);
	//创建军官
	void createOfficer();
	
	//旁白对话背景
	void createAsideBg();
	//左对话背景
	void createDboxLeft();
	//右对话背景
	void createDboxRight();
	//点击提示图标
	void createAnchr();
	//对话文本
	void createText();
	//创建有对话支线的按钮
	void createDialogBranchButton(int Num, bool threeBranch);

	//根据剧情索引创建每个国家主线剧情需要的人物 对话文本和背景 并开始对话
	void createDialogAllNeed_1();
	void createDialogAllNeed_2();
	void createDialogAllNeed_3();
	void createDialogAllNeed_4();
	void createDialogAllNeed_5();

	//屏幕点击的方法
	void	noviceClickEvent();
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
	void dialogSpeakOrder1_1();
	void dialogSpeakOrder1_2();
	void dialogSpeakOrder2_1();
	void dialogSpeakOrder2_2();
	void dialogSpeakOrder3_0();
	void dialogSpeakOrder4_1();
	void dialogSpeakOrder4_2();
	void dialogSpeakOrder5_0();
	//触发剧情对话播放顺序
	void triggerDialogSpeakOrder();
	//小伙伴红胡子剧情对话播放顺序
	void partnerRedBeardDialogSpeakOrder();

	//主线对话结束时删除层并且主线任务提交的方法
	void removeGuideWidget();
	//对话结束时只是删除层的方法
	void removeGuideWidgetOnly();
	//小剧情结束时删除层并且小剧情提交的方法
	void removeGuideWidgetTrigger();
	//小伙伴红胡子结束时删除层并且小剧情提交的方法
	void removeGuideWidgetPartnerRedBeard();
	
	//设定播放对话定时器
	void	openText();
	//播放对话的定时器调用的方法，用来逐字显示对话
	void	showText(float f);

	//显示对话支线
	void showBranch();
	//显示对话
	void showDialog();
	//显示旁白背景
	void showAside();
	//显示海上背景
	void showSeaBoatBg();
	
	//显示插画时定时器调用的函数
	void showIllustration(float f);
	
	//剧情对话人物出场 (为动作进行先后而调用)
	void personCome();
	//剧情对话人物对话显示 (为动作进行先后而调用)
	void personSpeak();
	   
	//西班牙对话中 酒馆背景改变(为动作进行先后而调用)
	void changeWineHouseBg();
	//西班牙对话中 显示插画酒馆中的卡特(为动作进行先后而调用)
	void showCateWineHousePic();
	//西班牙对话中 天黑再转到天亮(为动作进行先后而调用)
	void dayAndNight();
	//西班牙对话中 改变酒馆中的卡特插画 换成酒馆背景(为动作进行先后而调用)
	void changeToWineHouseBg();
	//西班牙对话中 改变酒馆背景 换成卧室的卡特插画(为动作进行先后而调用)
	void changeCateBedroomPic();
	//西班牙对话中 显示插画卧室的卡特(为动作进行先后而调用)
	void showCateBedroomPic();
	//改变海上背景(为动作进行先后而调用)
	void changeSeaBoatBg();
	//改变战争背景(为动作进行先后而调用)
	void changeToBoatteamBg();
	//改变当前对话背景(为动作进行先后而调用)
	void changeDialogBg();
	//改变当前对话背景 换成插画审判(为动作进行先后而调用)
	void changeToJudgeBg();

	//变量重置
	void reset();
};
#endif
