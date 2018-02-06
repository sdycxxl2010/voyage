#include "UIStoryLine.h"
#include "UIGuideSailManage.h"
#include "MainTaskManager.h"
#include "UITavern.h"
#include "CompanionTaskManager.h"
#include "UIGuideCenterCharactorSkill.h"
#include "UINoviceStoryLine.h"
#include "TVBattleManager.h"

#include "UIPalace.h"
#include "Utils.h"
#include "TVSceneLoader.h"
#include "UISailManage.h"
#include "UIMain.h"
#include "UIShipyard.h"
#include "UIPort.h"

/*
* 剧情类 由onMainTaskMessage()或onTriggerDialogMessage() 开始主线或触发的剧情
* 然后传入stepDialog()或stepTriggerDialog()剧情对话段落索引
* 根据对话段落索引 创建对话层、写入剧情json文件 
* 之后创建剧情所需人物、背景等等 （调用createDialogAllNeed_1()类似来创建）
* 对话层点击相应事件为noviceClickEvent()（同时判断点击翻页）
* 点击后 过滤点击步骤dialogSentenceStepByStep(int Num)
* 开始顺序播放对话 dialogSpeakOrder1_1()（对话更替为下一句和剧情是否结束和删除也在此类似方法中判断）
*/
UIStoryLine::UIStoryLine()
{
	guideWidget = nullptr;
	titleTxt = nullptr;
	txtTxt = nullptr;
	branchText1 = nullptr;
	branchText2 = nullptr;

	wineHouseBg = nullptr;
	seaBoatBg = nullptr;
	dockBg = nullptr;
	palaceBg = nullptr;
	svalbardBg = nullptr;
	judgePic = nullptr;
	boatteamPic = nullptr;
	cateWineHousePic = nullptr;
	cateBedroomPic = nullptr;
	CeutaBattlePic = nullptr;
	MadeiraPic = nullptr;
	CapeVerdePic = nullptr;
	RedBeardFleetVoyagePic = nullptr;
	RedBeardWar1Pic = nullptr;
	RedBeardWar2Pic = nullptr;
	dialogBg = nullptr;
	AidePic = nullptr;
	HeroPic = nullptr;
	majorFigurePic = nullptr;
	pasengerAPic = nullptr;
	pasengerBPic = nullptr;
	kingPic = nullptr;
	supportingRolePic = nullptr;
	OfficerPic = nullptr;
	asideBg = nullptr;
	dboxLeft = nullptr;
	dboxRight = nullptr;
	anchr = nullptr;
	maskPic = nullptr;
	handPic = nullptr;
	zhixianBgPic = nullptr;

	branchButton1 = nullptr;
	branchButton2 = nullptr;
	branchButton3 = nullptr;
	pStencil = nullptr;

	OFFSET = 0;
	txtNum = 0;
	branchTxtNum = 0;
	lenNum = 0;
	len = 0;
	maxLen = 0;
	moreNum = 0;
	plusNum = 0;
	addjust = 0;
	FontSize = 23;
	IllustrationNum = 0;
	dialogType = 0;

	textTime = -1;

	lenBoo = false;
	layerFarmClickBoo = true;
	directShowBoo = false;

	m_dialogVector.clear();
	m_dialogNames.clear();
	JOANNA = "";
	JOHN = "";
	mystr = "";

	m_eStep = 0;
	m_eTriggerStep = 0;
	m_ePartnerStep = 0;
	m_nOneLineHeight = 0;
	m_pParent = nullptr;
	m_jsonName.clear();
	m_jsonDialog.clear();
	log("start ****************------------------");
}

UIStoryLine::~UIStoryLine()
{
	unregisterCallBack();
	guideWidget = nullptr;
	titleTxt = nullptr;
	txtTxt = nullptr;
	branchText1 = nullptr;
	branchText2 = nullptr;

	wineHouseBg = nullptr;
	seaBoatBg = nullptr;
	dockBg = nullptr;
	palaceBg = nullptr;
	svalbardBg = nullptr;
	judgePic = nullptr;
	boatteamPic = nullptr;
	cateWineHousePic = nullptr;
	cateBedroomPic = nullptr;
	CeutaBattlePic = nullptr;
	MadeiraPic = nullptr;
	CapeVerdePic = nullptr;
	RedBeardFleetVoyagePic = nullptr;
	RedBeardWar1Pic = nullptr;
	RedBeardWar2Pic = nullptr;
	dialogBg = nullptr;
	AidePic = nullptr;
	HeroPic = nullptr;
	majorFigurePic = nullptr;
	pasengerAPic = nullptr;
	pasengerBPic = nullptr;
	kingPic = nullptr;
	supportingRolePic = nullptr;
	OfficerPic = nullptr;
	asideBg = nullptr;
	dboxLeft = nullptr;
	dboxRight = nullptr;
	anchr = nullptr;
	maskPic = nullptr;
	handPic = nullptr;
	zhixianBgPic = nullptr;

	branchButton1 = nullptr;
	branchButton2 = nullptr;
	branchButton3 = nullptr;
	pStencil = nullptr;

	m_pParent = nullptr;

	m_dialogVector.clear();
	m_dialogNames.clear();
	m_jsonName.clear();
	m_jsonDialog.clear();
	log("release ****************------------------");
}

void UIStoryLine::onEnter()
{
	UIBasicLayer::onEnter();
}

void UIStoryLine::onExit()
{
	UIBasicLayer::onExit();
}

UIStoryLine* UIStoryLine::m_pInstance=0;

UIStoryLine* UIStoryLine::GetInstance(){
	if(!m_pInstance){
		m_pInstance = new UIStoryLine;
		m_pInstance->autorelease();
		m_pInstance->retain();
	}
	return m_pInstance;
}
//剧情播放最先调用的方法 主线剧情
bool UIStoryLine::onMainTaskMessage(int dialogId,int flags)
{
#if 1
	if (dialogId > 0)
	{
		stepDialog(dialogId);
	}
	return true;
#else
	Scene*currentScene = Director::getInstance()->getRunningScene();
	if(currentScene){
		m_pParent = currentScene;
		currentScene->addChild(this,1000);
		stepDialog(PORTUGAL_TASK_THREE_GOAL_ONE_HINT2);
		return true;
	}
	return false;
#endif
}
//剧情播放最先调用的方法 触发剧情
bool UIStoryLine::onTriggerDialogMessage(int triggerDialogId, int flags)
{
#if 1
	if (triggerDialogId > 0)
	{
		stepTriggerDialog(triggerDialogId);
	}
	return true;
#else
	Scene*currentScene = Director::getInstance()->getRunningScene();
	if (currentScene){
		m_pParent = currentScene;
		currentScene->addChild(this, 1000);
		stepTriggerDialog(DISTRIBUTE_MONEY);
		return true;
	}
	return false;
#endif
}
//剧情播放最先调用的方法 小伙伴剧情
bool UIStoryLine::onPartnerDialogMessage(int partnerDialogId, int flags)
{
#if 1
	if (partnerDialogId > 0 && !guideWidget)
	{
		stepPartnerDialog(partnerDialogId);
	}
	return true;
#else
	Scene*currentScene = Director::getInstance()->getRunningScene();
	if (currentScene){
		m_pParent = currentScene;
		currentScene->addChild(this, 1000);
		stepPartnerDialog(PARTNER_RED_BEARD_ZERO);
		return true;
	}
	return false;
#endif
}
UIStoryLine* UIStoryLine::createNovice(Layer * parent)
{
	UIStoryLine* pup = new UIStoryLine;
	if (pup && pup->init())
	{
		pup->m_pParent = parent;
		pup->autorelease();
		return pup;
	}
	CC_SAFE_DELETE(pup);
	CCASSERT(pup,"create pup fail");
	return nullptr;
}

bool UIStoryLine::init()
{
	bool pRet = false;
	do 
	{
		pRet = true;
	} while (0);
	return pRet;
}
void UIStoryLine::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	log("menuCall_func");
	Widget* button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
}

void UIStoryLine::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	if (msgType == PROTO_TYPE_EngageInFightForTaskResult)
	{
		EngageInFightForTaskResult*result = (EngageInFightForTaskResult*)message;
		if (result->failed == 0)
		{
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			SINGLE_HERO->m_pBattelData = result->batteldata;
			SINGLE_HERO->shippos = Vec2(0, 0);
			ProtocolThread::GetInstance()->forceFightStatus();
			CHANGETO(SCENE_TAG::BATTLE_TAG);
		}
	}
	else if (msgType == PROTO_TYPE_DeductCoinsResult)
	{
		DeductCoinsResult*result = (DeductCoinsResult*)message;
		if (result->failed == 0)
		{
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			SINGLE_HERO->m_iCoin = result->coins;
			auto currentScene = Director::getInstance()->getRunningScene();
			auto palaceLayer = (UIPalace*)(currentScene->getChildByTag(SCENE_TAG::PALACE_TAG + 100));
			if (palaceLayer)
			{
				auto viewCsb = palaceLayer->getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
				auto panel_actionbar = viewCsb->getChildByName("panel_actionbar");
				auto bntSliver = panel_actionbar->getChildByName<Button*>("button_silver");
				auto label_silvernum = bntSliver->getChildByName<Text*>("label_silver_num");
				label_silvernum->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iCoin)));
			}
		}
	}
}
/*
*触发剧情对话段落根据索引选择
*首先创建对话层
*然后根据传进来的剧情索引创建需要的人物 对话文本和背景
*/
void UIStoryLine::stepTriggerDialog(int eStep)
{
	m_eTriggerStep = eStep;
	dialogType = DIALOG_TYPE_SMALL_STORY;
	createLayer();
	switch (m_eTriggerStep)
	{
	case GIVING_GIFT_TO_BARGIRL:
	case DISTRIBUTE_MONEY:
	case OPEN_OFFLINE_COMMISION:
	{
								   createAide();
								   createHero();
								   if (m_eTriggerStep == GIVING_GIFT_TO_BARGIRL)
								   {
									   creatMajorFigure(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_BAR_GIRL));
								   }
								   if (m_eTriggerStep == DISTRIBUTE_MONEY)
								   {
									   creatMajorFigure(DIALOG_PATH[KINDNESS_MERCHANT_PATH]);
								   }
								   createDboxLeft();
								   createDboxRight();
								   createText();
								   createAnchr();
								   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
								   //每段剧情初始语句索引 txtNum为索引值
								   switch (m_eTriggerStep)
								   {
								   case GIVING_GIFT_TO_BARGIRL:
									   txtNum = 0;
									   break;
								   case DISTRIBUTE_MONEY:
									   txtNum = 12;
									   break;
								   case OPEN_OFFLINE_COMMISION:
									   txtNum = 22;
									   break;
								   default:
									   break;
								   }
								   m_eName = AIDE;
								   dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
								   break;
	}
	default:
		break;
	}
}
/*
*小伙伴红胡子剧情对话段落根据索引选择
*首先创建对话层
*然后根据传进来的剧情索引创建需要的人物 对话文本和背景
*/
void UIStoryLine::stepPartnerDialog(int eStep)
{
	m_ePartnerStep = eStep;
	dialogType = DIALOG_TYPE_PARTNER_RED_BEARD;
	createLayer();
	switch (m_ePartnerStep)
	{
	case PARTNER_RED_BEARD_ZERO:
	case PARTNER_RED_BEARD_THREE_GOAL_ONE:
	case PARTNER_RED_BEARD_FOUR:
	case PARTNER_RED_BEARD_FIVE_GOAL_ONE:
	case PARTNER_RED_BEARD_SIX_GOAL_ONE:
	case PARTNER_RED_BEARD_SEVEN_GOAL_ONE:
	case PARTNER_RED_BEARD_SEVEN_GOAL_THREE:
	case PARTNER_RED_BEARD_EIGHT_GOAL_ONE:
	{
								   createAide();
								   createHero();
								   if (m_ePartnerStep == PARTNER_RED_BEARD_FIVE_GOAL_ONE || m_ePartnerStep == PARTNER_RED_BEARD_SIX_GOAL_ONE)
								   {
									   //路人
									   creatPassenger(false);
								   }
								   else if (m_ePartnerStep == PARTNER_RED_BEARD_SEVEN_GOAL_THREE)
								   {
									   createOfficer();
								   }
								   createDboxLeft();
								   createDboxRight();
								   createText();
								   createAnchr();
								   m_eName = AIDE;
								   switch (m_ePartnerStep)
								   {
								   case PARTNER_RED_BEARD_ZERO:
									   txtNum = 0;
									   break;
								   case PARTNER_RED_BEARD_THREE_GOAL_ONE:
									   txtNum = 31;
									   break;
								   case PARTNER_RED_BEARD_FOUR:
									   txtNum = 40;
									   break;
								   case PARTNER_RED_BEARD_FIVE_GOAL_ONE:
									   txtNum = 43;
									   m_eName = PASENGERA;
									   break;
								   case PARTNER_RED_BEARD_SIX_GOAL_ONE:
									   txtNum = 56;
									   m_eName = HERO;
									   break;
								   case PARTNER_RED_BEARD_SEVEN_GOAL_ONE:
									   txtNum = 74;
									   break;
								   case PARTNER_RED_BEARD_SEVEN_GOAL_THREE:
									   txtNum = 84;
									   break;
								   case PARTNER_RED_BEARD_EIGHT_GOAL_ONE:
									   txtNum = 89;
									   m_eName = HERO;
									   break;
								   default:
									   break;
								   }
								   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
								   if (m_ePartnerStep == PARTNER_RED_BEARD_SIX_GOAL_ONE || m_ePartnerStep == PARTNER_RED_BEARD_EIGHT_GOAL_ONE)
								   {
									   dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
								   }
								   else
								   {
									   dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
								   }
								   if (m_ePartnerStep == PARTNER_RED_BEARD_FOUR || m_ePartnerStep == PARTNER_RED_BEARD_SEVEN_GOAL_THREE)
								   {
									   guideWidget->setCameraMask(4);
								   }
								   break;
	}
	case PARTNER_RED_BEARD_ONE:
	case PARTNER_RED_BEARD_THREE_GOAL_TWO:
	case PARTNER_RED_BEARD_SIX_GOAL_TWO:
	case PARTNER_RED_BEARD_EIGHT_GOAL_TWO:
	{
								   if (m_ePartnerStep == PARTNER_RED_BEARD_EIGHT_GOAL_TWO)
								   {
									   //红胡子牢房
									   createDialogBg(DIALOG_PATH[REDBEARDPRISON_PATH]);
									   //士兵
									   createSupportingRole(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_SOLDIER));
									   //红胡子
									   creatMajorFigure(DIALOG_PATH[REDBEARD_PATH]);
								   }
								   else
								   {
									   createOfficer();
								   }
								   createHero();
								   createDboxLeft();
								   createDboxRight();
								   m_eName = OFFICER;
								   switch (m_ePartnerStep)
								   {
								   case PARTNER_RED_BEARD_ONE:
									   txtNum = 8;
									   createDialogBranchButton(txtNum + 1, false);
									   break;
								   case PARTNER_RED_BEARD_THREE_GOAL_TWO:
									   txtNum = 33;
									   break;
								   case PARTNER_RED_BEARD_SIX_GOAL_TWO:
									   txtNum = 62;
									   m_eName = HERO;
									   break;
								   case PARTNER_RED_BEARD_EIGHT_GOAL_TWO:
									   txtNum = 92;
									   m_eName = SOLDIER;
									   createDialogBranchButton(txtNum + 1, true);
									   createAsideBg();
									   break;
								   default:
									   break;
								   }
								   createText();
								   createAnchr();
								   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
								   if (m_ePartnerStep == PARTNER_RED_BEARD_SIX_GOAL_TWO)
								   {
									   dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
								   }
								   else
								   {
									   dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
								   }
								   break;
	}
	case PARTNER_RED_BEARD_TWO:
	case PARTNER_RED_BEARD_SEVEN_GOAL_TWO:
	{
								  if (m_ePartnerStep == PARTNER_RED_BEARD_TWO)
								  {
									  createDockBg();
									  dockBg->setOpacity(255);
									  createRedBeardFleetVoyagePic();
									  createRedBeardWar1Pic();
									  createRedBeardWar2Pic();
									  createOfficer();
								  }
								  createHero();
								  createAide();
								  //红胡子
								  creatMajorFigure(DIALOG_PATH[REDBEARD_PATH]);
								  //水手
								  createSupportingRole(DIALOG_PATH[SAILOR_PATH]);
								  createDboxLeft();
								  createDboxRight();
								  createAsideBg();
								  createText();
								  switch (m_ePartnerStep)
								  {
								  case PARTNER_RED_BEARD_TWO:
									  txtNum = 14;
									  createDialogBranchButton(txtNum + 1, false);
									  m_eName = OFFICER;
									  break;
								  case PARTNER_RED_BEARD_SEVEN_GOAL_TWO:
									  txtNum = 76;
									  m_eName = SAILOR;
									  break;
								  default:
									  break;
								  }
								  createAnchr();
								  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
								  if (m_ePartnerStep == PARTNER_RED_BEARD_SEVEN_GOAL_TWO)
								  {
									  dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									  guideWidget->setCameraMask(4);
								  }
								  else
								  {
									  dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
								  }
								  break;
	}
	case PARTNER_RED_BEARD_FIVE_GOAL_TWO:
	case PARTNER_RED_BEARD_FIVE_GOAL_THREE:
	case PARTNER_RED_BEARD_NINE_GOAL_ONE:
	case PARTNER_RED_BEARD_NINE_GOAL_TWO_FAILED:
	case PARTNER_RED_BEARD_NINE_GOAL_THREE_WIN:
	{
								  //5_3 有海战背景图 TODO
								  //红胡子
								  if (m_ePartnerStep == PARTNER_RED_BEARD_FIVE_GOAL_TWO || m_ePartnerStep == PARTNER_RED_BEARD_FIVE_GOAL_THREE)
								  {
									  if (m_ePartnerStep == PARTNER_RED_BEARD_FIVE_GOAL_THREE)
									  {
										  createDialogBg(DIALOG_PATH[REDBEARDWAR1]);
										  dialogBg->setOpacity(255);
									  }
								  }
								  creatMajorFigure(DIALOG_PATH[REDBEARD_PATH]);
								  createHero();
								  if (m_ePartnerStep == PARTNER_RED_BEARD_NINE_GOAL_ONE)
								  {
									  //士兵
									  createSupportingRole(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_SOLDIER));
								  }
								  createDboxLeft();
								  createDboxRight();
								  createText();
								  createAnchr();
								  m_eName = MAJORFIGURE;
								  switch (m_ePartnerStep)
								  {
								  case PARTNER_RED_BEARD_FIVE_GOAL_TWO:
									  txtNum = 47;
									  break;
								  case PARTNER_RED_BEARD_FIVE_GOAL_THREE:
									  txtNum = 51;
									  break;
								  case PARTNER_RED_BEARD_NINE_GOAL_ONE:
									  txtNum = 123;
									  createDialogBranchButton(txtNum + 1, false);
									  m_eName = SOLDIER;
									  break;
								  case PARTNER_RED_BEARD_NINE_GOAL_TWO_FAILED:
									  txtNum = 131;
									  break;
								  case PARTNER_RED_BEARD_NINE_GOAL_THREE_WIN:
									  txtNum = 133;
									  break;
								  default:
									  break;
								  }
								  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
								  dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
								  if (m_ePartnerStep != PARTNER_RED_BEARD_NINE_GOAL_ONE)
								  {
									  auto currentScene = Director::getInstance()->getRunningScene();
									  auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
									  if (!mainlayer)
									  {
										  guideWidget->setCameraMask(4);
									  }
								  }
								  break;
	}
	default:
		break;
	}
}
/*
*主线剧情对话段落根据索引选择
*首先创建对话层
*然后判断索引是否为海上引导、战斗引导或战败引导的剧情 否则根据国家来进行主线剧情
*最后根据传进来的剧情索引创建需要的人物 对话文本和背景
*/
void UIStoryLine::stepDialog(int eStep)
{
	log("NoviceLayer::stepDialog step:%d,--- %d", eStep, __LINE__);
	m_eStep = eStep;
	dialogType = DIALOG_TYPE_MAIN_TASK;
	//首先创建对话层
	createLayer();
	//判断m_eStep索引是否为海上引导、战斗引导或战败引导的剧情
	if (m_eStep == SALLING_GUID || m_eStep == WAR_GUID || m_eStep == WAR_FAIL_GUID)
	{
		createAide();
		createDboxLeft();
		createAsideBg();
		createText();
		createAnchr();
		readJson();
		switch (m_eStep)
		{
		case SALLING_GUID:
			txtNum = 22;
			break;
		case WAR_GUID:
			txtNum = 23;
			break;
		case WAR_FAIL_GUID:
			txtNum = 81;
			break;
		default:
			break;
		}
		m_eName = AIDE;
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else
	{
		//根据传进来的剧情索引创建需要的人物 对话文本和背景
		switch (SINGLE_HERO->m_iNation)
		{
		case 1:
			createDialogAllNeed_1();
			break;
		case 2:
			createDialogAllNeed_2();
			break;
		case 3:
			createDialogAllNeed_3();
			break;
		case 4:
			createDialogAllNeed_4();
			break;
		case 5:
			createDialogAllNeed_5();
			break;
		default:
			break;
		}
	}
}
//根据剧情索引创建葡萄牙主线剧情需要的人物 对话文本和背景 并开始对话
void UIStoryLine::createDialogAllNeed_1()
{
	switch (m_eStep)
	{
	case PORTUGAL_TASK_THREE_GOAL_ONE_DOCK1:
	{
											   createDockBg();
											   dockBg->setOpacity(255);
											   createSeaBoatBg();
											   createCeutaBattlePic();
											   createAide();
											   createHero();
											   creatMajorFigure(DIALOG_PATH[HENRY_PATH]);
											   createDboxLeft();
											   createDboxRight();
											   createAsideBg();
											   createText();
											   txtNum = 1;
											   createDialogBranchButton(txtNum + 1,false);
											   createAnchr();
											   m_eName = AIDE;
											   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
											   dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											   break;
	}
	case PORTUGAL_TASK_THREE_GOAL_ONE_DOCK2:
	{
											   this->setCameraMask(4);
											   creatMajorFigure(DIALOG_PATH[HENRY_PATH]);
											   createDboxLeft();
											   txtNum = 5;
											   createText();
											   createAnchr();
											   m_eName = MAJORFIGURE;
											   dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											   break;
	}
	case PORTUGAL_TASK_THREE_GOAL_TWO1:
	case PORTUGAL_TASK_THREE_GOAL_THREE:
	case PORTUGAL_TASK_THREE_GOAL_FIVE:
	{
										  createAide();
										  createHero();
										  createDboxLeft();
										  createDboxRight();
										  createText();
										  createAnchr();
										  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
										  if (m_eStep == PORTUGAL_TASK_THREE_GOAL_TWO1 || m_eStep == PORTUGAL_TASK_THREE_GOAL_FIVE)
										  {
											  txtNum = 6;
											  m_eName = AIDE;
											  dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
										  }
										  else if (m_eStep == PORTUGAL_TASK_THREE_GOAL_THREE)
										  {
											  txtNum = 17;
											  m_eName = HERO;
											  dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
										  }
										  break;
	}
	case PORTUGAL_TASK_THREE_GOAL_TWO2:
	case PORTUGAL_TASK_FOUR_GOAL_ONE:
	case PORTUGAL_TASK_FIVE_GOAL_ONE:
	case PORTUGAL_TASK_FIVE_GOAL_FIVE:
	{
										 createWineHouseBg();
										 wineHouseBg->setOpacity(255);
										 createAide();
										 createHero();
										 creatMajorFigure(DIALOG_PATH[HENRY_PATH]);
										 createDboxLeft();
										 createDboxRight();
										 createText();
										 createAnchr();
										 switch (m_eStep)
										 {
										 case PORTUGAL_TASK_THREE_GOAL_TWO2:
											 txtNum = 8;
											 break;
										 case PORTUGAL_TASK_FOUR_GOAL_ONE:
											 if (SINGLE_HERO->m_iGender == 1)
											 {
												 txtNum = 23;
											 }
											 else
											 {
												 txtNum = 25;
											 }
											 break;
										 case PORTUGAL_TASK_FIVE_GOAL_ONE:
											 if (SINGLE_HERO->m_iGender == 1)
											 {
												 txtNum = 38;
											 }
											 else
											 {
												 txtNum = 42;
											 }
											 break;
										 case PORTUGAL_TASK_FIVE_GOAL_FIVE:
											 if (SINGLE_HERO->m_iGender == 1)
											 {
												 txtNum = 68;
											 }
											 else
											 {
												 txtNum = 72;
											 }
											 break;
										 default:
											 break;
										 }
										 m_eName = MAJORFIGURE;
										 SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
										 SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
										 dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
										 break;
	}
	case PORTUGAL_TASK_FOUR_GOAL_FIVE:
	case PORTUGAL_TASK_FIVE_GOAL_FOUR:
	{
										 createSeaBoatBg();
										 seaBoatBg->setOpacity(255);
										 createAide();
										 createHero();
										 createDboxLeft();
										 createDboxRight();
										 if (m_eStep == PORTUGAL_TASK_FOUR_GOAL_FIVE)
										 {
											 if (SINGLE_HERO->m_iGender == 1)
											 {
												 txtNum = 32;
												 m_eName = HERO;
												 createDialogBranchButton(txtNum + 2,false);
											 }
											 else
											 {
												 txtNum = 35;
												 m_eName = AIDE;
												 createDialogBranchButton(txtNum + 3, false);
											 }
										 }
										 else
										 {
											 createAsideBg();
											 if (SINGLE_HERO->m_iGender == 1)
											 {
												 txtNum = 51;
											 }
											 else
											 {
												 txtNum = 53;
											 }
											 createDialogBranchButton(txtNum + 6, false);
										 }
										 createText();
										 createAnchr();
										 if (m_eStep == PORTUGAL_TASK_FOUR_GOAL_FIVE)
										 {
											 SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
											 if (SINGLE_HERO->m_iGender == 1)
											 {
												 dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											 }
											 else
											 {
												 dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											 }
										 }
										 else
										 {
											 asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
										 }
										 guideWidget->setCameraMask(4);
										 break;
	}
	case PORTUGAL_TASK_FIVE_GOAL_FOUR_FINDVERDE:
	{
												   createCapeVerdePic();
												   CapeVerdePic->setOpacity(255);
												   createDialogBg("res/event_type/event_2.jpg");
												   createAide();
												   createHero();
												   createDboxLeft();
												   createDboxRight();
												   createText();
												   createAnchr();
												   if (SINGLE_HERO->m_iGender == 1)
												   {
													   txtNum = 64;
												   }
												   else
												   {
													   txtNum = 68;
												   }
												   m_eName = AIDE;
												   dboxLeft->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
												   guideWidget->setCameraMask(4);
												   break;
	}
	case PORTUGAL_TASK_THREE_GOAL_ONE_HINT1:
	case PORTUGAL_TASK_THREE_GOAL_ONE_HINT2:
	case PORTUGAL_TASK_THREE_GOAL_THREE_HINT:
	case PORTUGAL_TASK_THREE_GOAL_FOUR_HINT:
	case PORTUGAL_TASK_THREE_GOAL_FIVE_HINT:
	case PORTUGAL_TASK_FOUR_GOAL_TWO_HINT:
	case PORTUGAL_TASK_FOUR_GOAL_THREE_HINT1:
	case PORTUGAL_TASK_FOUR_GOAL_THREE_HINT2:
	case PORTUGAL_TASK_FOUR_GOAL_FOUR_HINT1:
	case PORTUGAL_TASK_FOUR_GOAL_FOUR_HINT2:
	case PORTUGAL_TASK_FOUR_GOAL_FIVE_FINDEMADEIRA:
	case PORTUGAL_TASK_FIVE_GOAL_TWO_HINT:
	case PORTUGAL_TASK_FIVE_GOAL_THREE_HINT:
	case PORTUGAL_TASK_FIVE_GOAL_FOUR_HINT:
	{
											  
											  if (m_eStep == PORTUGAL_TASK_THREE_GOAL_ONE_HINT2)
											  {
												  creatMajorFigure(DIALOG_PATH[HENRY_PATH]);
												  m_eName = MAJORFIGURE;
											  }
											  else
											  {
												  if (m_eStep == PORTUGAL_TASK_FOUR_GOAL_FIVE_FINDEMADEIRA)
												  {
													  createMadeiraPic();
													  MadeiraPic->setOpacity(255);
												  }
												  createAide();
												  m_eName = AIDE;
											  }
											  createDboxLeft();
											  createText();
											  createAnchr();
											  switch (m_eStep)
											  {
											  case PORTUGAL_TASK_THREE_GOAL_ONE_HINT1:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 71;
												  }
												  else
												  {
													  txtNum = 75;
												  }
												  break;
											  case PORTUGAL_TASK_THREE_GOAL_ONE_HINT2:
												  txtNum = 0;
												  break;
											  case PORTUGAL_TASK_THREE_GOAL_THREE_HINT:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 72;
												  }
												  else
												  {
													  txtNum = 76;
												  }
												  break;
											  case PORTUGAL_TASK_THREE_GOAL_FOUR_HINT:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 73;
												  }
												  else
												  {
													  txtNum = 77;
												  }
												  break;
											  case PORTUGAL_TASK_THREE_GOAL_FIVE_HINT:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 74;
												  }
												  else
												  {
													  txtNum = 78;
												  }
												  break;
											  case PORTUGAL_TASK_FOUR_GOAL_TWO_HINT:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 75;
												  }
												  else
												  {
													  txtNum = 79;
												  }
												  break;
											  case PORTUGAL_TASK_FOUR_GOAL_THREE_HINT1:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 76;
												  }
												  else
												  {
													  txtNum = 80;
												  }
												  break;
											  case PORTUGAL_TASK_FOUR_GOAL_THREE_HINT2:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 30;
												  }
												  else
												  {
													  txtNum = 33;
												  }
												  break;
											  case PORTUGAL_TASK_FOUR_GOAL_FOUR_HINT1:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 77;
												  }
												  else
												  {
													  txtNum = 81;
												  }
												  break;
											  case PORTUGAL_TASK_FOUR_GOAL_FOUR_HINT2:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 31;
												  }
												  else
												  {
													  txtNum = 34;
												  }
												  break;
											  case PORTUGAL_TASK_FOUR_GOAL_FIVE_FINDEMADEIRA:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 37;
												  }
												  else
												  {
													  txtNum = 41;
												  }
												  break;
											  case PORTUGAL_TASK_FIVE_GOAL_TWO_HINT:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 49;
												  }
												  else
												  {
													  txtNum = 51;
												  }
												  break;
											  case PORTUGAL_TASK_FIVE_GOAL_THREE_HINT:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 50;
												  }
												  else
												  {
													  txtNum = 52;
												  }
												  break;
											  case PORTUGAL_TASK_FIVE_GOAL_FOUR_HINT:
												  if (SINGLE_HERO->m_iGender == 1)
												  {
													  txtNum = 78;
												  }
												  else
												  {
													  txtNum = 82;
												  }
												  break;
											  default:
												  break;
											  }
											  if (m_eStep == PORTUGAL_TASK_FOUR_GOAL_FIVE_FINDEMADEIRA)
											  {
												  dboxLeft->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
												  guideWidget->setCameraMask(4);
											  }
											  else
											  {
												  dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											  }
											  break;
	}
	default:
		break;
	}
}
//根据剧情索引创建西班牙主线剧情需要的人物 对话文本和背景 并开始对话
void UIStoryLine::createDialogAllNeed_2()
{
	auto picNum = 0;
	switch (m_eStep)
	{
	case SPAIN_TASK_THREE_GOAL_ONE_HINT:
	case SPAIN_TASK_THREE_GOAL_THREE_HINT:
	case SPAIN_TASK_THREE_GOAL_FOUR_HINT:
	case SPAIN_TASK_THREE_GOAL_FIVE_HINT:
	case SPAIN_TASK_FOUR_GOAL_TWO_HINT:
	case SPAIN_TASK_FOUR_GOAL_THREE_HINT1:
	case SPAIN_TASK_FOUR_GOAL_THREE_HINT2:
	case SPAIN_TASK_FOUR_GOAL_FOUR_HINT1:
	case SPAIN_TASK_FOUR_GOAL_FOUR_HINT2:
	case SPAIN_TASK_FIVE_GOAL_TWO_HINT:
	case SPAIN_TASK_FIVE_GOAL_THREE_HINT:
	{
											createAide();
											m_eName = AIDE;
											createDboxLeft();
											createText();
											createAnchr();
											switch (m_eStep)
											{
											case SPAIN_TASK_THREE_GOAL_ONE_HINT:
												txtNum = 0;
												break;
											case SPAIN_TASK_THREE_GOAL_THREE_HINT:
												txtNum = 12;
												break;
											case SPAIN_TASK_THREE_GOAL_FOUR_HINT:
												txtNum = 22;
												break;
											case SPAIN_TASK_THREE_GOAL_FIVE_HINT:
												txtNum = 23;
												break;
											case SPAIN_TASK_FOUR_GOAL_TWO_HINT:
												txtNum = 31;
												break;
											case SPAIN_TASK_FOUR_GOAL_THREE_HINT1:
												txtNum = 32;
												break;
											case SPAIN_TASK_FOUR_GOAL_THREE_HINT2:
												txtNum = 33;
												break;
											case SPAIN_TASK_FOUR_GOAL_FOUR_HINT1:
												txtNum = 34;
												break;
											case SPAIN_TASK_FOUR_GOAL_FOUR_HINT2:
												txtNum = 35;
												break;
											case SPAIN_TASK_FIVE_GOAL_TWO_HINT:
												if (SINGLE_HERO->m_iGender == 1)
												{
													txtNum = 73;
												}
												else
												{
													txtNum = 79;
												}
												break;
											case SPAIN_TASK_FIVE_GOAL_THREE_HINT:
												if (SINGLE_HERO->m_iGender == 1)
												{
													txtNum = 74;
												}
												else
												{
													txtNum = 80;
												}
												break;
											default:
												break;
											}
											dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											break;
	}
	case SPAIN_TASK_THREE_GOAL_ONE:
	case SPAIN_TASK_THREE_GOAL_FIVE:
	case SPAIN_TASK_FIVE_GOAL_THREE:
	{
									   createAide();
									   createHero();
									   createDboxLeft();
									   createDboxRight();
									   createText();
									   createAnchr();
									   switch (m_eStep)
									   {
									   case SPAIN_TASK_THREE_GOAL_ONE:
										   txtNum = 1;
										   break;
									   case SPAIN_TASK_THREE_GOAL_FIVE:
										   txtNum = 24;
										   break;
									   case SPAIN_TASK_FIVE_GOAL_THREE:
										   if (SINGLE_HERO->m_iGender == 1)
										   {
											   txtNum = 75;
										   }
										   else
										   {
											   txtNum = 81;
										   }
										   break;
									   default:
										   break;
									   }
									   m_eName = AIDE;
									   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
									   dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									   break;
	}
	case SPAIN_TASK_THREE_GOAL_TWO:
	case SPAIN_TASK_FOUR_GOAL_ONE:
	case SPAIN_TASK_FIVE_GOAL_ONE:
	{
									 createWineHouseBg();
									 wineHouseBg->setOpacity(255);
									 if (m_eStep == SPAIN_TASK_FIVE_GOAL_ONE)
									 {
										 createCateWineHouse();
										 createCateBedroom();
									 }
									 createAide();
									 createHero();
									 creatMajorFigure(DIALOG_PATH[CATALINA_PATH]);
									 createDboxLeft();
									 createDboxRight();
									 switch (m_eStep)
									 {
									 case SPAIN_TASK_THREE_GOAL_TWO:
										 txtNum = 3;
										 break;
									 case SPAIN_TASK_FOUR_GOAL_ONE:
										 txtNum = 26;
										 break;
									 case SPAIN_TASK_FIVE_GOAL_ONE:
										 createAsideBg();
										 txtNum = 49;
										 break;
									 default:
										 break;
									 }
									 createText();
									 createAnchr();
									 SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
									 SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
									 if (m_eStep == SPAIN_TASK_FIVE_GOAL_ONE && SINGLE_HERO->m_iGender == 2)
									 {

										 m_eName = HERO;
										 dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									 }
									 else
									 {
										 m_eName = MAJORFIGURE;
										 dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									 }
									 break;
	}
	case SPAIN_TASK_THREE_GOAL_THREE:
	{
										createAide();
										createHero();
										creatPassenger(true);
										createDboxLeft();
										createDboxRight();
										createText();
										createAnchr();
										txtNum = 13;
										m_eName = HERO;
										SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
										dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
										break;
	}
	case SPAIN_TASK_FOUR_GOAL_FIVE1:
	case SPAIN_TASK_FIVE_GOAL_FOUR:
	{
									  createSeaBoatBg();
									  seaBoatBg->setOpacity(255);
									  if (m_eStep == SPAIN_TASK_FIVE_GOAL_FOUR)
									  {
										  createBoatteamBg();
										  
										  if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
										  {
											  picNum = SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_village;
										  }
										  else
										  {
											  picNum = SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_id;
										  }
										  createDialogBg(StringUtils::format("cities_resources/background/city_%d.jpg", picNum));
										  createJudgeBg();
										  createWineHouseBg();
										  createPalaceBg();
										  createKing(DIALOG_PATH[SPAIN_KING_PATH]);
										  createSupportingRole(DIALOG_PATH[MERCHANT_PATH]);
									  }
									  createAide();
									  createHero();
									  createDboxLeft();
									  createDboxRight();
									  createAsideBg();
									  createText();
									  createAnchr();
									  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
									  if (m_eStep == SPAIN_TASK_FOUR_GOAL_FIVE1)
									  {
										  txtNum = 36;
										  m_eName = AIDE;
										  dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									  }
									  else if (m_eStep == SPAIN_TASK_FIVE_GOAL_FOUR)
									  {
										  if (SINGLE_HERO->m_iGender == 1)
										  {
											  txtNum = 79;
										  }
										  else
										  {
											  txtNum = 85;
										  }
										  m_eName = HERO;
										  dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									  }
									  guideWidget->setCameraMask(4);
									  break;
	}
	case SPAIN_TASK_FOUR_GOAL_FIVE2:
	{
									   createDialogBg(DIALOG_PATH[ATTACK_SPAIN_PATH]);
									   dialogBg->setOpacity(255);
									   createAide();
									   createHero();
									   creatMajorFigure(DIALOG_PATH[CATALINA_PATH]);
									   createDboxLeft();
									   createDboxRight();
									   txtNum = 41;
									   createText();
									   createAnchr();
									   m_eName = HERO;
									   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
									   dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									   guideWidget->setCameraMask(4);
									   break;
	}
	case SPAIN_TASK_FIVE_GOAL_FIVE:
	{
									  createWineHouseBg();
									  wineHouseBg->setOpacity(255);
									  createHero();
									  creatMajorFigure(DIALOG_PATH[CATALINA_PATH]);
									  createDboxLeft();
									  createDboxRight();
									  createAsideBg();
									  createText();
									  createAnchr();
									  if (SINGLE_HERO->m_iGender == 1)
									  {
										  txtNum = 97;
									  }
									  else
									  {
										  txtNum = 103;
									  }
									  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
									  asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									  break;
	}
	default:
		break;
	}
}
//根据剧情索引创建英国主线剧情需要的人物 对话文本和背景 并开始对话
void UIStoryLine::createDialogAllNeed_3()
{
	switch (m_eStep)
	{
	case ENGLAND_TASK_THREE_GOAL_ONE_HINT:
	case ENGLAND_TASK_THREE_GOAL_THREE_HINT:
	case ENGLAND_TASK_THREE_GOAL_FOUR_HINT:
	case ENGLAND_TASK_THREE_GOAL_FIVE_HINT:
	case ENGLAND_TASK_FOUR_GOAL_TWO_HINT:
	case ENGLAND_TASK_FOUR_GOAL_THREE_HINT1:
	case ENGLAND_TASK_FOUR_GOAL_THREE_HINT2:
	case ENGLAND_TASK_FOUR_GOAL_FOUR_HINT1:
	case ENGLAND_TASK_FOUR_GOAL_FOUR_HINT2:
	case ENGLAND_TASK_FIVE_GOAL_TWO_HINT:
	case ENGLAND_TASK_FIVE_GOAL_THREE_HINT:
	{
											  createAide();
											  createDboxLeft();
											  createDboxRight();
											  createText();
											  createAnchr();
											  switch (m_eStep)
											  {
											  case ENGLAND_TASK_THREE_GOAL_ONE_HINT:
												  txtNum = 0;
												  break;
											  case ENGLAND_TASK_THREE_GOAL_THREE_HINT:
												  txtNum = 12;
												  break;
											  case ENGLAND_TASK_THREE_GOAL_FOUR_HINT:
												  txtNum = 13;
												  break;
											  case ENGLAND_TASK_THREE_GOAL_FIVE_HINT:
												  txtNum = 14;
												  break;
											  case ENGLAND_TASK_FOUR_GOAL_TWO_HINT:
												  txtNum = 23;
												  break;
											  case ENGLAND_TASK_FOUR_GOAL_THREE_HINT1:
												  txtNum = 24;
												  break;
											  case ENGLAND_TASK_FOUR_GOAL_THREE_HINT2:
												  txtNum = 25;
												  break;
											  case ENGLAND_TASK_FOUR_GOAL_FOUR_HINT1:
												  txtNum = 26;
												  break;
											  case ENGLAND_TASK_FOUR_GOAL_FOUR_HINT2:
												  txtNum = 27;
												  break;
											  case ENGLAND_TASK_FIVE_GOAL_TWO_HINT:
												  txtNum = 35;
												  break;
											  case ENGLAND_TASK_FIVE_GOAL_THREE_HINT:
												  txtNum = 36;
												  break;
											  default:
												  break;
											  }
											  m_eName = AIDE;
											  dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											  break;
	}
	case ENGLAND_TASK_THREE_GOAL_ONE:
	case ENGLAND_TASK_THREE_GOAL_FIVE:
	case ENGLAND_TASK_FIVE_GOAL_THREE:
	{
										 switch (m_eStep)
										 {
										 case ENGLAND_TASK_THREE_GOAL_ONE:
											 txtNum = 1;
											 m_eName = AIDE;
											 break;
										 case ENGLAND_TASK_THREE_GOAL_FIVE:
											 txtNum = 15;
											 m_eName = AIDE;
											 break;
										 case ENGLAND_TASK_FIVE_GOAL_THREE:
											 createPalaceBg();
											 creatMajorFigure(DIALOG_PATH[DEREK_PATH]);
											 createKing(DIALOG_PATH[ENGLAND_QUEEN_PATH]);
											 m_eName = MAJORFIGURE;
											 txtNum = 37;
											 break;
										 default:
											 break;
										 }
										 createAide();
										 createHero();
										 createDboxLeft();
										 createDboxRight();
										 createText();
										 createAnchr();
										 SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
										 dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
										 break;
	}
	case ENGLAND_TASK_THREE_GOAL_TWO:
	case ENGLAND_TASK_FOUR_GOAL_ONE:
	case ENGLAND_TASK_FIVE_GOAL_ONE:
	case ENGLAND_TASK_FIVE_GOAL_FIVE:
	{
										createWineHouseBg();
										wineHouseBg->setOpacity(255);
										createAide();
										createHero();
										creatMajorFigure(DIALOG_PATH[DEREK_PATH]);
										createDboxLeft();
										createDboxRight();
										switch (m_eStep)
										{
										case ENGLAND_TASK_THREE_GOAL_TWO:
											txtNum = 3;
											break;
										case ENGLAND_TASK_FOUR_GOAL_ONE:
											txtNum = 17;
											break;
										case ENGLAND_TASK_FIVE_GOAL_ONE:
											txtNum = 31;
											break;
										case ENGLAND_TASK_FIVE_GOAL_FIVE:
											txtNum = 59;
											break;
										default:
											break;
										}
										createText();
										createAnchr();
										m_eName = MAJORFIGURE;
										SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
										SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
										dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
										break;
	}
	case ENGLAND_TASK_FOUR_GOAL_FIVE:
	case ENGLAND_TASK_FIVE_GOAL_FOUR:
	{
										createSeaBoatBg();
										seaBoatBg->setOpacity(255);
										switch (m_eStep)
										{
										case ENGLAND_TASK_FOUR_GOAL_FIVE:
											txtNum = 28;
											createDialogBg(DIALOG_PATH[ATTACK_SPAIN_PATH]);
											break;
										case ENGLAND_TASK_FIVE_GOAL_FOUR:
											txtNum = 55;
											createDialogBg(DIALOG_PATH[ENGLAND_VS_SPAIN_PATH]);
											break;
										default:
											break;
										}
										createAide();
										createHero();
										createDboxLeft();
										createDboxRight();
										createText();
										createAnchr();
										m_eName = AIDE;
										dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
										guideWidget->setCameraMask(4);
										break;
	}
	default:
		break;
	}
}
//根据剧情索引创建荷兰主线剧情需要的人物 对话文本和背景 并开始对话
void UIStoryLine::createDialogAllNeed_4()
{
	switch (m_eStep)
	{
	case NETHERLANDS_TASK_THREE_GOAL_ONE_HINT:
	case NETHERLANDS_TASK_THREE_GOAL_THREE_HINT:
	case NETHERLANDS_TASK_THREE_GOAL_FOUR_HINT:
	case NETHERLANDS_TASK_THREE_GOAL_FIVE_HINT:
	case NETHERLANDS_TASK_FOUR_GOAL_TWO_HINT:
	case NETHERLANDS_TASK_FOUR_GOAL_THREE_HINT1:
	case NETHERLANDS_TASK_FOUR_GOAL_THREE_HINT2:
	case NETHERLANDS_TASK_FOUR_GOAL_FOUR_HINT1:
	case NETHERLANDS_TASK_FOUR_GOAL_FOUR_HINT2:
	case NETHERLANDS_TASK_FIVE_GOAL_TWO_HINT:
	case NETHERLANDS_TASK_FIVE_GOAL_THREE_HINT:
	case NETHERLANDS_TASK_FIVE_GOAL_FOUR_HINT:
	{
												 createAide();
												 createDboxLeft();
												 createDboxRight();
												 createText();
												 createAnchr();
												 switch (m_eStep)
												 {
												 case NETHERLANDS_TASK_THREE_GOAL_ONE_HINT:
													 txtNum = 0;
													 break;
												 case NETHERLANDS_TASK_THREE_GOAL_THREE_HINT:
													 txtNum = 12;
													 break;
												 case NETHERLANDS_TASK_THREE_GOAL_FOUR_HINT:
													 txtNum = 13;
													 break;
												 case NETHERLANDS_TASK_THREE_GOAL_FIVE_HINT:
													 txtNum = 14;
													 break;
												 case NETHERLANDS_TASK_FOUR_GOAL_TWO_HINT:
													 txtNum = 27;
													 break;
												 case NETHERLANDS_TASK_FOUR_GOAL_THREE_HINT1:
													 txtNum = 28;
													 break;
												 case NETHERLANDS_TASK_FOUR_GOAL_THREE_HINT2:
													 txtNum = 29;
													 break;
												 case NETHERLANDS_TASK_FOUR_GOAL_FOUR_HINT1:
													 txtNum = 30;
													 break;
												 case NETHERLANDS_TASK_FOUR_GOAL_FOUR_HINT2:
													 txtNum = 31;
													 break;
												 case NETHERLANDS_TASK_FIVE_GOAL_TWO_HINT:
													 if (SINGLE_HERO->m_iGender == 1)
													 {
														 txtNum = 57;
													 }
													 else
													 {
														 txtNum = 53;
													 }
													 break;
												 case NETHERLANDS_TASK_FIVE_GOAL_THREE_HINT:
													 if (SINGLE_HERO->m_iGender == 1)
													 {
														 txtNum = 58;
													 }
													 else
													 {
														 txtNum = 54;
													 }
													 break;
												 case NETHERLANDS_TASK_FIVE_GOAL_FOUR_HINT:
													 if (SINGLE_HERO->m_iGender == 1)
													 {
														 txtNum = 91;
													 }
													 else
													 {
														 txtNum = 86;
													 }
													 break;
												 default:
													 break;
												 }
												 m_eName = AIDE;
												 dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
												 break;
	}
	case NETHERLANDS_TASK_THREE_GOAL_ONE:
	case NETHERLANDS_TASK_THREE_GOAL_FIVE:
	case NETHERLANDS_TASK_FIVE_GOAL_FIVE:
	{
											switch (m_eStep)
											{
											case NETHERLANDS_TASK_THREE_GOAL_ONE:
											case NETHERLANDS_TASK_THREE_GOAL_FIVE:
												txtNum = 1;
												break;
											case NETHERLANDS_TASK_FIVE_GOAL_FIVE:
												if (SINGLE_HERO->m_iGender == 1)
												{
													txtNum = 89;
												}
												else
												{
													txtNum = 84;
												}
												break;
											default:
												break;
											}
											m_eName = AIDE;
											createAide();
											createHero();
											createDboxLeft();
											createDboxRight();
											createText();
											createAnchr();
											SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
											dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											break;
	}
	case NETHERLANDS_TASK_THREE_GOAL_TWO:
	case NETHERLANDS_TASK_FOUR_GOAL_ONE:
	case NETHERLANDS_TASK_FIVE_GOAL_ONE:
	{
										   createWineHouseBg();
										   wineHouseBg->setOpacity(255);
										   createAide();
										   createHero();
										   creatMajorFigure(DIALOG_PATH[BARENTS_PATH]);
										   createDboxLeft();
										   createDboxRight();
										   if (m_eStep == NETHERLANDS_TASK_FIVE_GOAL_ONE)
										   {
											   createAsideBg();
										   }
										   createText();
										   createAnchr();
										   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
										   switch (m_eStep)
										   {
										   case NETHERLANDS_TASK_THREE_GOAL_TWO:
										   case NETHERLANDS_TASK_FOUR_GOAL_ONE:
											   if (m_eStep == NETHERLANDS_TASK_THREE_GOAL_TWO)
											   {
												   txtNum = 3;
											   }
											   else
											   {
												   txtNum = 17;
											   }
											   m_eName = MAJORFIGURE;
											   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
											   dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											   break;
										   case NETHERLANDS_TASK_FIVE_GOAL_ONE:
											   txtNum = 41;
											   asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											   break;
										   default:
											   break;
										   }
										   break;
	}
	case NETHERLANDS_TASK_FOUR_GOAL_FIVE:
	case NETHERLANDS_TASK_FIVE_GOAL_FOUR:
	{
											if (m_eStep == NETHERLANDS_TASK_FOUR_GOAL_FIVE)
											{
												createSeaBoatBg();
												seaBoatBg->setOpacity(255);
												txtNum = 32;
											}
											else
											{
												createSvalbardBg();
												svalbardBg->setOpacity(255);
												if (SINGLE_HERO->m_iGender == 1)
												{
													txtNum = 59;
												}
												else
												{
													txtNum = 55;
												}
											}
											createDialogBg(DIALOG_PATH[FINDEXINDIDAO_PATH]);
											creatMajorFigure(DIALOG_PATH[BARENTS_PATH]);
											createAide();
											createHero();
											if (m_eStep == NETHERLANDS_TASK_FIVE_GOAL_FOUR)
											{
												createSupportingRole(DIALOG_PATH[SAILOR_PATH]);
											}
											createDboxLeft();
											createDboxRight();
											createAsideBg();
											createText();
											createAnchr();
											asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											guideWidget->setCameraMask(4);
											break;
	}
	default:
		break;
	}
}
//根据剧情索引创建热那亚主线剧情需要的人物 对话文本和背景 并开始对话
void UIStoryLine::createDialogAllNeed_5()
{
	switch (m_eStep)
	{
	case GENOVA_TASK_THREE_GOAL_ONE_HINT:
	case GENOVA_TASK_THREE_GOAL_THREE_HINT:
	case GENOVA_TASK_THREE_GOAL_FOUR_HINT:
	case GENOVA_TASK_THREE_GOAL_FIVE_HINT:
	case GENOVA_TASK_FOUR_GOAL_TWO_HINT:
	case GENOVA_TASK_FOUR_GOAL_THREE_HINT1:
	case GENOVA_TASK_FOUR_GOAL_THREE_HINT2:
	case GENOVA_TASK_FOUR_GOAL_FOUR_HINT:
	case GENOVA_TASK_FIVE_GOAL_TWO_HINT:
	case GENOVA_TASK_FIVE_GOAL_THREE_HINT1:
	case GENOVA_TASK_FIVE_GOAL_THREE_HINT2:
	case GENOVA_TASK_FIVE_GOAL_FOUR_HINT:
	case GENOVA_TASK_FOUR_GOAL_FIVE_HINT:
	{
											createAide();
											createDboxLeft();
											createDboxRight();
											createText();
											createAnchr();
											switch (m_eStep)
											{
											case GENOVA_TASK_THREE_GOAL_ONE_HINT:
												txtNum = 0;
												break;
											case GENOVA_TASK_THREE_GOAL_THREE_HINT:
												txtNum = 12;
												break;
											case GENOVA_TASK_THREE_GOAL_FOUR_HINT:
												txtNum = 13;
												break;
											case GENOVA_TASK_THREE_GOAL_FIVE_HINT:
												txtNum = 14;
												break;
											case GENOVA_TASK_FOUR_GOAL_TWO_HINT:
												txtNum = 28;
												break;
											case GENOVA_TASK_FOUR_GOAL_THREE_HINT1:
												txtNum = 29;
												break;
											case GENOVA_TASK_FOUR_GOAL_THREE_HINT2:
												txtNum = 30;
												break;
											case GENOVA_TASK_FOUR_GOAL_FOUR_HINT:
												txtNum = 31;
												break;
											case GENOVA_TASK_FIVE_GOAL_TWO_HINT:
												txtNum = 49;
												break;
											case GENOVA_TASK_FIVE_GOAL_THREE_HINT1:
												txtNum = 50;
												break;
											case GENOVA_TASK_FIVE_GOAL_THREE_HINT2:
												txtNum = 51;
												break;
											case GENOVA_TASK_FIVE_GOAL_FOUR_HINT:
												txtNum = 64;
												guideWidget->setCameraMask(4);
												break;
											case GENOVA_TASK_FOUR_GOAL_FIVE_HINT:
												txtNum = 65;
												break;
											default:
												break;
											}
											m_eName = AIDE;
											dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
											break;
	}
	case GENOVA_TASK_THREE_GOAL_ONE:
	case GENOVA_TASK_THREE_GOAL_FIVE:
	case GENOVA_TASK_FOUR_GOAL_FIVE:
	case GENOVA_TASK_FIVE_GOAL_FIVE:
	{
									   switch (m_eStep)
									   {
									   case GENOVA_TASK_THREE_GOAL_ONE:
										   txtNum = 1;
										   break;
									   case GENOVA_TASK_THREE_GOAL_FIVE:
										   txtNum = 15;
										   break;
									   case GENOVA_TASK_FOUR_GOAL_FIVE:
										   txtNum = 32;
										   break;
									   case GENOVA_TASK_FIVE_GOAL_FIVE:
										   txtNum = 62;
										   break;
									   default:
										   break;
									   }
									   m_eName = AIDE;
									   createAide();
									   createHero();
									   createDboxLeft();
									   createDboxRight();
									   createText();
									   createAnchr();
									   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
									   dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									   break;
	}
	case GENOVA_TASK_THREE_GOAL_TWO:
	case GENOVA_TASK_FOUR_GOAL_ONE:
	case GENOVA_TASK_FIVE_GOAL_ONE:
	{
									  createWineHouseBg();
									  wineHouseBg->setOpacity(255);
									  if (m_eStep == GENOVA_TASK_FIVE_GOAL_ONE)
									  {
										  createPalaceBg();
									  }
									  createAide();
									  createHero();
									  creatMajorFigure(DIALOG_PATH[COLUMBUS_PATH]);
									  switch (m_eStep)
									  {
									  case GENOVA_TASK_THREE_GOAL_TWO:
										  txtNum = 3;
										  break;
									  case GENOVA_TASK_FOUR_GOAL_ONE:
										  txtNum = 17;
										  createSupportingRole(DIALOG_PATH[TOPHATMAN_PATH]);
										  createAsideBg();
										  break;
									  case GENOVA_TASK_FIVE_GOAL_ONE:
										  txtNum = 34;
										  createSupportingRole(DIALOG_PATH[CHANCELLOR_PATH]);
										  createKing(DIALOG_PATH[ENGLAND_QUEEN_PATH]);
										  createAsideBg();
										  HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100)))));
										  break;
									  default:
										  break;
									  }
									  createDboxLeft();
									  createDboxRight();
									  createText();
									  createAnchr();

									  m_eName = MAJORFIGURE;
									  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
									  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
									  dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									  break;
	}
	case GENOVA_TASK_FIVE_GOAL_FOUR:
	{
									   createSeaBoatBg();
									   seaBoatBg->setOpacity(255);
									   createDialogBg(DIALOG_PATH[FINDFODEJIAO_PATH]);
									   creatMajorFigure(DIALOG_PATH[COLUMBUS_PATH]);
									   createAide();
									   createHero();
									   createDboxLeft();
									   createDboxRight();
									   createAsideBg();
									   createText();
									   createAnchr();
									   txtNum = 52;
									   asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
									   guideWidget->setCameraMask(4);
									   break;
	}
	default:
		break;
	}
}
/*
*创建guideWidget 设置其点击事件 并判断读取（主线对话或触发剧情）json文件
*/
void UIStoryLine::createLayer()
{
	if (!guideWidget)
	{
		  guideWidget = LayerColor::create(Color4B(0,0,0,150));
		  this->addChild(guideWidget);
	}
	//this->addChild(guideWidget);
	Size size(Director::getInstance()->getVisibleSize().width,Director::getInstance()->getVisibleSize().height);
	guideWidget->setContentSize(size);
	guideWidget->setAnchorPoint(Vec2(0,0));
	guideWidget->setTouchEnabled(true);

	auto callback = [](Touch * ,Event *)
    {
        return true;
    };
	auto callback2 = [=](Touch* touch, Event* event)
    {
        UIStoryLine::noviceClickEvent();
    };

    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = callback;
	listener->onTouchEnded = callback2;
    listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,guideWidget);
	//判断读取（主线对话或触发剧情）json文件
	switch (dialogType)
	{
	case DIALOG_TYPE_MAIN_TASK:
		if (m_eStep != SALLING_GUID && m_eStep != WAR_GUID && m_eStep != WAR_FAIL_GUID)
		{
			UIStoryLine::replaceJsonString();
		}
		break;
	case DIALOG_TYPE_SMALL_STORY:
	case DIALOG_TYPE_PARTNER_RED_BEARD:
		UIStoryLine::replaceJsonString();
		break;
	default:
		break;
	}
}
/*
*获取主线对话json内容 或触发剧情内容（dialogType为0时为主线剧情 dialogType为1时是触发剧情）
*获取到的剧情的名字存到m_jsonName 获取到的剧情的内容存到m_jsonDialog  
*遍历m_jsonName和m_jsonDialog名字替换 返回替换后的名字和内容
*/
void UIStoryLine::replaceJsonString()
{
	std::string name;
	std::string dialog;
	std::string old_value_hero = "[heroname]";
	std::string new_value_hero = SINGLE_HERO->m_sName;
	std::string old_value_aide = "[aidename]";
	std::string new_value_aide; 
	if(SINGLE_HERO->m_iGender == 1)
	{
		new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"];
	}
	else
	{
		new_value_aide = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"];
	}
	std::string old_value_bargirl = "[bargirl name]";
	std::string new_value_bargirl = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].bargirl;
	m_jsonName.clear();
	m_jsonDialog.clear();
	switch (dialogType)
	{
	case DIALOG_TYPE_MAIN_TASK:
	{
									int count = SINGLE_SHOP->getTaskDialogInfo().size();
									log("count:%d-%d", count, __LINE__);
									map <int, TASKDIALOGINFO>::iterator m1_Iter;
									auto taskMap = SINGLE_SHOP->getTaskDialogInfo();
									for (m1_Iter = taskMap.begin(); m1_Iter != taskMap.end(); m1_Iter++)
									{
										auto& item = m1_Iter->second;
										name = item.name;
										dialog = item.dialog;
										//需要替换 玩家名称和副官名称
										repalce_all_ditinct(name, old_value_hero, new_value_hero);
										repalce_all_ditinct(name, old_value_aide, new_value_aide);
										repalce_all_ditinct(dialog, old_value_hero, new_value_hero);
										repalce_all_ditinct(dialog, old_value_aide, new_value_aide);
										//log("name = %s new_value_hero = %s", name.c_str(), new_value_hero.c_str());
										m_jsonName.push_back(name);
										m_jsonDialog.push_back(dialog);
									}

									break;
	}
	case DIALOG_TYPE_SMALL_STORY:
	{
									int count = SINGLE_SHOP->getTriggerDialogInfo().size();
									log("count:%d-%d", count, __LINE__);
									map <int, TRIGGERDIALOGINFO>::iterator m1_Iter;
									auto taskMap = SINGLE_SHOP->getTriggerDialogInfo();
									for (m1_Iter = taskMap.begin(); m1_Iter != taskMap.end(); m1_Iter++)
									{
										auto& item = m1_Iter->second;
										name = item.name;
										dialog = item.dialog;
										//需要替换 玩家名称和副官名称、酒吧女郎名称
										repalce_all_ditinct(name, old_value_hero, new_value_hero);
										repalce_all_ditinct(name, old_value_aide, new_value_aide);
										repalce_all_ditinct(dialog, old_value_hero, new_value_hero);
										repalce_all_ditinct(dialog, old_value_aide, new_value_aide);
										repalce_all_ditinct(name, old_value_bargirl, new_value_bargirl);
										repalce_all_ditinct(dialog, old_value_bargirl, new_value_bargirl);
										//log("name = %s new_value_hero = %s", name.c_str(), new_value_hero.c_str());
										m_jsonName.push_back(name);
										m_jsonDialog.push_back(dialog);
									}

									break;
	}
	case DIALOG_TYPE_PARTNER_RED_BEARD:
	{
									int count = SINGLE_SHOP->getTriggerDialogInfo().size();
									log("count:%d-%d", count, __LINE__);
									map <int, PARTNERREDBEARDDIALOGINFO>::iterator m1_Iter;
									auto taskMap = SINGLE_SHOP->getPartnerRedBeardDialogInfo();
									for (m1_Iter = taskMap.begin(); m1_Iter != taskMap.end(); m1_Iter++)
									{
										auto& item = m1_Iter->second;
										name = item.name;
										dialog = item.dialog;
										//需要替换 玩家名称和副官名称
										repalce_all_ditinct(name, old_value_hero, new_value_hero);
										repalce_all_ditinct(name, old_value_aide, new_value_aide);
										repalce_all_ditinct(dialog, old_value_hero, new_value_hero);
										repalce_all_ditinct(dialog, old_value_aide, new_value_aide);
										log("name = %s new_value_hero = %s", name.c_str(), new_value_hero.c_str());
										m_jsonName.push_back(name);
										m_jsonDialog.push_back(dialog);
									}

									break;
	}
	default:
		break;
	}
}
/*
*对话层的点击事件
*layerFarmClickBoo为false时可点击
*点击时判断是否翻页
*/
void UIStoryLine::noviceClickEvent()
{
	if (layerFarmClickBoo == false)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		layerFarmClickBoo = true;
		//对话点击提示是否显示 显示则判断继续翻页或是播放下一句 未显示则直接显示整句然后显示点击提示
		if (anchr->getOpacity() == 255)
		{
			//如果直接显示内容的翻页为true 则继续翻页 直到剩余高度moreNum不足翻页 则继续后面的剧情
			if (directShowBoo == true)
			{
				if (moreNum > 0)
				{
					moreNum = moreNum - m_nOneLineHeight * 3;
					txtTxt->setPositionY(txtTxt->getPositionY() + m_nOneLineHeight * 3 + 3);
					if (moreNum <= 0)
					{
						directShowBoo = false;
					}
				}
				layerFarmClickBoo = false;
			}
			else
			{
				//len和maxLen的比较来判断是否在逐字显示的翻页
				if (len < maxLen)
				{
					UIStoryLine::dialogSentenceStepByStep(txtNum);
				}
				else
				{
					if (dialogType == DIALOG_TYPE_MAIN_TASK)
					{
						//SALLING_GUID和WAR_GUID为海上引导和战斗引导的剧情 json为lua的json 内容顺序特殊
						if (m_eStep == SALLING_GUID)
						{
							if (txtNum == 22)
							{
								//后面是航行教学
								txtNum = 64;
							}
						}
						if (m_eStep == WAR_GUID)
						{
							if (txtNum == 23)
							{
								//后面是战斗教学
								txtNum = 67;
							}
						}
					}
					
					txtNum = txtNum + 1;
					titleTxt->setString("");
					txtTxt->setString("");
					lenNum = 0;
					txtTxt->setPosition(maskPic->getPositionX(), maskPic->getPositionY());
					UIStoryLine::dialogSentenceStepByStep(txtNum);
				}
			}
		}
		else
		{
			this->unschedule(schedule_selector(UIStoryLine::showText));
			txtTxt->setString(mystr);
			anchr->setOpacity(255);
			if (asideBg)
			{
				if (asideBg->getOpacity() == 255)
				{
					anchr->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width - 80, Director::getInstance()->getVisibleOrigin().y + 40);
				}
			}
			if (dboxLeft)
			{
				if (dboxLeft->getOpacity() == 255)
				{
					anchr->setPosition(dboxLeft->getPositionX() + dboxLeft->getBoundingBox().size.width - 83.0, dboxLeft->getPositionY() + 36.0);
				}
			}
			if (dboxRight)
			{
				if (dboxRight->getOpacity() == 255)
				{
					anchr->setPosition(dboxRight->getPositionX() + dboxRight->getBoundingBox().size.width - 83.0, dboxRight->getPositionY() + 36.0);
				}
			}
			layerFarmClickBoo = false;
			len = 0;
			maxLen = 0;
			//判断直接显示的内容文本是否需要翻页
			if (txtTxt->getBoundingBox().size.height > m_nOneLineHeight * 3)
			{
				//计算文本超出部分的长度
				moreNum = txtTxt->getBoundingBox().size.height - m_nOneLineHeight * 3;
				directShowBoo = true;
			}
		}
	}
}
//创建酒馆背景
void UIStoryLine::createWineHouseBg()
{
	wineHouseBg = Sprite::create();
	wineHouseBg->setTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_PUP_AREA));
	wineHouseBg->setScale(Director::getInstance()->getVisibleSize().height/wineHouseBg->getContentSize().height,Director::getInstance()->getVisibleSize().height/wineHouseBg->getContentSize().height);
	wineHouseBg->setAnchorPoint(Vec2(0.5,0.5));
	wineHouseBg->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(wineHouseBg);
	wineHouseBg->setOpacity(0);
}
//创建海上背景
void UIStoryLine::createSeaBoatBg()
{
	seaBoatBg = Sprite::create();
	seaBoatBg->setTexture(DIALOG_PATH[SEABOAT_PATH]);
	seaBoatBg->setScale(Director::getInstance()->getVisibleSize().height/seaBoatBg->getContentSize().height,Director::getInstance()->getVisibleSize().height/seaBoatBg->getContentSize().height);
	seaBoatBg->setAnchorPoint(Vec2(0.5,0.5));
	seaBoatBg->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(seaBoatBg);
	seaBoatBg->setOpacity(0);
}
//创建港口背景
void UIStoryLine::createDockBg()
{
	dockBg = Sprite::create();
	dockBg->setTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_WHARF_AREA));
	dockBg->setScale(Director::getInstance()->getVisibleSize().height/dockBg->getContentSize().height,Director::getInstance()->getVisibleSize().height/dockBg->getContentSize().height);
	dockBg->setAnchorPoint(Vec2(0.5,0.5));
	dockBg->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(dockBg);
	dockBg->setOpacity(0);
}
//欧洲王宫背景
void UIStoryLine::createPalaceBg()
{
	palaceBg = Sprite::create();
	palaceBg->setTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_PALACE_AREA));
	palaceBg->setScale(Director::getInstance()->getVisibleSize().height / palaceBg->getContentSize().height, Director::getInstance()->getVisibleSize().height / palaceBg->getContentSize().height);
	palaceBg->setAnchorPoint(Vec2(0.5, 0.5));
	palaceBg->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(palaceBg);
	palaceBg->setOpacity(0);
}
//荷兰对话中 斯瓦尔巴特群岛插画背景
void UIStoryLine::createSvalbardBg()
{
	svalbardBg = Sprite::create();
	svalbardBg->setTexture(DIALOG_PATH[FINDSVALBARD_PATH]);
	svalbardBg->setScale(Director::getInstance()->getVisibleSize().height / svalbardBg->getContentSize().height, Director::getInstance()->getVisibleSize().height / svalbardBg->getContentSize().height);
	svalbardBg->setAnchorPoint(Vec2(0.5, 0.5));
	svalbardBg->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(svalbardBg);
	svalbardBg->setOpacity(0);
}
//审判卡特琳娜插画
void UIStoryLine::createJudgeBg()
{
	judgePic = Sprite::create();
	judgePic->setTexture(DIALOG_PATH[JUDGE_BG_PATH]);
	judgePic->setScale(Director::getInstance()->getVisibleSize().height / judgePic->getContentSize().height, Director::getInstance()->getVisibleSize().height / judgePic->getContentSize().height);
	judgePic->setAnchorPoint(Vec2(0.5, 0.5));
	judgePic->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(judgePic);
	judgePic->setOpacity(0);
}
//舰队插画
void UIStoryLine::createBoatteamBg()
{
	boatteamPic = Sprite::create();
	boatteamPic->setTexture(DIALOG_PATH[BOATTEAM_PATH]);
	boatteamPic->setScale(Director::getInstance()->getVisibleSize().height / boatteamPic->getContentSize().height, Director::getInstance()->getVisibleSize().height / boatteamPic->getContentSize().height);
	boatteamPic->setAnchorPoint(Vec2(0.5, 0.5));
	boatteamPic->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(boatteamPic);
	boatteamPic->setOpacity(0);
}
//卡特琳娜酒馆插画
void UIStoryLine::createCateWineHouse()
{
	cateWineHousePic = Sprite::create();
	cateWineHousePic->setTexture(DIALOG_PATH[CATEWINEHOUSE_PATH]);
	cateWineHousePic->setScale(Director::getInstance()->getVisibleSize().height / cateWineHousePic->getContentSize().height, Director::getInstance()->getVisibleSize().height / cateWineHousePic->getContentSize().height);
	cateWineHousePic->setAnchorPoint(Vec2(0.5, 0.5));
	cateWineHousePic->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(cateWineHousePic);
	cateWineHousePic->setOpacity(0);
}
//卡特琳娜卧室插画
void UIStoryLine::createCateBedroom()
{
	cateBedroomPic = Sprite::create();
	cateBedroomPic->setTexture(DIALOG_PATH[CATEBEDROOM_PATH]);
	cateBedroomPic->setScale(Director::getInstance()->getVisibleSize().height / cateBedroomPic->getContentSize().height, Director::getInstance()->getVisibleSize().height / cateBedroomPic->getContentSize().height);
	cateBedroomPic->setAnchorPoint(Vec2(0.5, 0.5));
	cateBedroomPic->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(cateBedroomPic);
	cateBedroomPic->setOpacity(0);
}
//创建发现马德拉插画
void UIStoryLine::createCeutaBattlePic()
{
	CeutaBattlePic = Sprite::create();
	CeutaBattlePic->setTexture(DIALOG_PATH[CEUTABATTLE_PATH]);
	CeutaBattlePic->setScale(Director::getInstance()->getVisibleSize().height/CeutaBattlePic->getContentSize().height,Director::getInstance()->getVisibleSize().height/CeutaBattlePic->getContentSize().height);
	CeutaBattlePic->setAnchorPoint(Vec2(0.5,0.5));
	CeutaBattlePic->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(CeutaBattlePic);
	CeutaBattlePic->setOpacity(0);
}
//创建发现佛得角插画
void UIStoryLine::createMadeiraPic()
{
	MadeiraPic = Sprite::create();
	MadeiraPic->setTexture(DIALOG_PATH[FINDMADELA_PATH]);
	MadeiraPic->setScale(Director::getInstance()->getVisibleSize().height/MadeiraPic->getContentSize().height,Director::getInstance()->getVisibleSize().height/MadeiraPic->getContentSize().height);
	MadeiraPic->setAnchorPoint(Vec2(0.5,0.5));
	MadeiraPic->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(MadeiraPic);
	MadeiraPic->setOpacity(0);
}
//创建发现佛得角插画
void UIStoryLine::createCapeVerdePic()
{
	CapeVerdePic = Sprite::create();
	CapeVerdePic->setTexture(DIALOG_PATH[FINDFODEJIAO_PATH]);
	CapeVerdePic->setScale(Director::getInstance()->getVisibleSize().height/CapeVerdePic->getContentSize().height,Director::getInstance()->getVisibleSize().height/CapeVerdePic->getContentSize().height);
	CapeVerdePic->setAnchorPoint(Vec2(0.5,0.5));
	CapeVerdePic->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(CapeVerdePic);
	CapeVerdePic->setOpacity(0);
}
//创建小伙伴红胡子剧情中航海舰队插画
void UIStoryLine::createRedBeardFleetVoyagePic()
{
	RedBeardFleetVoyagePic = Sprite::create();
	RedBeardFleetVoyagePic->setTexture(DIALOG_PATH[REDBEARDFLEETVOYAGE_PATH]);
	RedBeardFleetVoyagePic->setScale(Director::getInstance()->getVisibleSize().height / RedBeardFleetVoyagePic->getContentSize().height, Director::getInstance()->getVisibleSize().height / RedBeardFleetVoyagePic->getContentSize().height);
	RedBeardFleetVoyagePic->setAnchorPoint(Vec2(0.5, 0.5));
	RedBeardFleetVoyagePic->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(RedBeardFleetVoyagePic);
	RedBeardFleetVoyagePic->setOpacity(0);
}
//创建小伙伴红胡子剧情中海战1插画
void UIStoryLine::createRedBeardWar1Pic()
{
	RedBeardWar1Pic = Sprite::create();
	RedBeardWar1Pic->setTexture(DIALOG_PATH[REDBEARDWAR1]);
	RedBeardWar1Pic->setScale(Director::getInstance()->getVisibleSize().height / RedBeardWar1Pic->getContentSize().height, Director::getInstance()->getVisibleSize().height / RedBeardWar1Pic->getContentSize().height);
	RedBeardWar1Pic->setAnchorPoint(Vec2(0.5, 0.5));
	RedBeardWar1Pic->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(RedBeardWar1Pic);
	RedBeardWar1Pic->setOpacity(0);
}
//创建小伙伴红胡子剧情中海战2插画
void UIStoryLine::createRedBeardWar2Pic()
{
	RedBeardWar2Pic = Sprite::create();
	RedBeardWar2Pic->setTexture(DIALOG_PATH[REDBEARDWAR2]);
	RedBeardWar2Pic->setScale(Director::getInstance()->getVisibleSize().height / RedBeardWar2Pic->getContentSize().height, Director::getInstance()->getVisibleSize().height / RedBeardWar2Pic->getContentSize().height);
	RedBeardWar2Pic->setAnchorPoint(Vec2(0.5, 0.5));
	RedBeardWar2Pic->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(RedBeardWar2Pic);
	RedBeardWar2Pic->setOpacity(0);
}
//创建对话通用背景
void UIStoryLine::createDialogBg(string m_path)
{
	dialogBg = Sprite::create();
	dialogBg->setTexture(m_path);
	dialogBg->setScale(Director::getInstance()->getVisibleSize().height/dialogBg->getContentSize().height,Director::getInstance()->getVisibleSize().height/dialogBg->getContentSize().height);
	dialogBg->setAnchorPoint(Vec2(0.5,0.5));
	dialogBg->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(dialogBg);
	dialogBg->setOpacity(0);
}
//创建副官
void UIStoryLine::createAide()
{
	AidePic = Sprite::create();
	AidePic->setTexture(getCheifIconPath());
	AidePic->setAnchorPoint(Vec2(0,0));
	AidePic->setScale((576.0/720*Director::getInstance()->getVisibleSize().height)/AidePic->getContentSize().height,(576.0/720*Director::getInstance()->getVisibleSize().height)/AidePic->getContentSize().height);
	OFFSET = AidePic->getBoundingBox().size.width;//+ 70.0/1280*Director::getInstance()->getVisibleSize().width;
	AidePic->setPosition(- AidePic->getBoundingBox().size.width,0);
	guideWidget->addChild(AidePic);
}
//创建玩家
void UIStoryLine::createHero()
{
	HeroPic = Sprite::create();
	HeroPic->setTexture(StringUtils::format("res/npc/player/icon_%d.png", ProtocolThread::GetInstance()->getIcon()));
	HeroPic->setAnchorPoint(Vec2(0,0));
	HeroPic->setScale((576.0/720*Director::getInstance()->getVisibleSize().height)/HeroPic->getContentSize().height,(576.0/720*Director::getInstance()->getVisibleSize().height)/HeroPic->getContentSize().height);
	OFFSET = HeroPic->getBoundingBox().size.width;//+ 70.0/1280*Director::getInstance()->getVisibleSize().width;
	HeroPic->setPosition(Director::getInstance()->getVisibleSize().width,0);
	guideWidget->addChild(HeroPic);
	HeroPic->setFlippedX(true);
}
//创建每个国家代表人物（亨利、卡特琳娜、德雷克、巴伦支、哥伦布）
void UIStoryLine::creatMajorFigure(string m_path)
{
	majorFigurePic = Sprite::create();
	majorFigurePic->setTexture(m_path);
	majorFigurePic->setAnchorPoint(Vec2(0,0));
	majorFigurePic->setScale((576.0/720*Director::getInstance()->getVisibleSize().height)/majorFigurePic->getContentSize().height,(576.0/720*Director::getInstance()->getVisibleSize().height)/majorFigurePic->getContentSize().height);
	OFFSET = majorFigurePic->getBoundingBox().size.width;// + 70.0/1280*Director::getInstance()->getVisibleSize().width;
	majorFigurePic->setPosition(- majorFigurePic->getBoundingBox().size.width,0);
	guideWidget->addChild(majorFigurePic);
}
//创建路人甲乙 (showB为是否创建路人乙)
void UIStoryLine::creatPassenger(bool showB)
{
	//西班牙路人甲乙
	pasengerAPic = Sprite::create();
	pasengerAPic->setTexture(DIALOG_PATH[PASENGER_A_PATH]);
	pasengerAPic->setAnchorPoint(Vec2(0, 0));
	pasengerAPic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / pasengerAPic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / pasengerAPic->getContentSize().height);
	OFFSET = pasengerAPic->getBoundingBox().size.width;// + 70.0 / 1280 * Director::getInstance()->getVisibleSize().width;
	pasengerAPic->setPosition(-pasengerAPic->getBoundingBox().size.width, 0);
	guideWidget->addChild(pasengerAPic);
	if (showB)
	{
		pasengerBPic = Sprite::create();
		pasengerBPic->setTexture(DIALOG_PATH[PASENGER_B_PATH]);
		pasengerBPic->setAnchorPoint(Vec2(0, 0));
		pasengerBPic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / pasengerBPic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / pasengerBPic->getContentSize().height);
		pasengerBPic->setPosition(-pasengerBPic->getBoundingBox().size.width, 0);
		guideWidget->addChild(pasengerBPic);
	}
}
//创建国王或王后
void UIStoryLine::createKing(string m_path)
{
	kingPic = Sprite::create();
	kingPic->setTexture(m_path);
	kingPic->setAnchorPoint(Vec2(0, 0));
	kingPic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / kingPic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / kingPic->getContentSize().height);
	OFFSET = kingPic->getBoundingBox().size.width;// + 70.0 / 1280 * Director::getInstance()->getVisibleSize().width;
	kingPic->setPosition(-kingPic->getBoundingBox().size.width, 0);
	guideWidget->addChild(kingPic);
}
//创建富商甲（西班牙人物）、荷兰剧情和红胡子剧情的水手、热那亚高帽男、热那亚剧情中的西班牙王宫大臣、红胡子剧情的守卫
void UIStoryLine::createSupportingRole(std::string m_path)
{
	supportingRolePic = Sprite::create();
	supportingRolePic->setTexture(m_path);
	supportingRolePic->setAnchorPoint(Vec2(0, 0));
	supportingRolePic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / supportingRolePic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / supportingRolePic->getContentSize().height);
	OFFSET = supportingRolePic->getBoundingBox().size.width;// + 70.0 / 1280 * Director::getInstance()->getVisibleSize().width;
	supportingRolePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
	supportingRolePic->setFlippedX(true);
	guideWidget->addChild(supportingRolePic);
}
//创建军官
void UIStoryLine::createOfficer()
{
	OfficerPic = Sprite::create();
	OfficerPic->setTexture(getNpcPath(2, FLAG_CHIEF_OFFICER));
	OfficerPic->setAnchorPoint(Vec2(0, 0));
	OfficerPic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / OfficerPic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / OfficerPic->getContentSize().height);
	OFFSET = OfficerPic->getBoundingBox().size.width;//+ 70.0/1280*Director::getInstance()->getVisibleSize().width;
	OfficerPic->setPosition(-OfficerPic->getBoundingBox().size.width, 0);
	guideWidget->addChild(OfficerPic);
}
//旁白对话背景
void UIStoryLine::createAsideBg()
{
	asideBg = Sprite::create();
	asideBg->setTexture(DIALOG_PATH[ASIDE_PATH]);
	asideBg->setScale(Director::getInstance()->getVisibleSize().width / asideBg->getContentSize().width,Director::getInstance()->getVisibleSize().width / asideBg->getContentSize().width);
	asideBg->setAnchorPoint(Vec2(0,0));
	guideWidget->addChild(asideBg);
	asideBg->setOpacity(0);
}
//左对话背景
void UIStoryLine::createDboxLeft()
{
	dboxLeft = Sprite::create();
	dboxLeft->setTexture(DIALOG_PATH[DBOXLEFT_PATH]);
	dboxLeft->setAnchorPoint(Vec2(0,0));
	dboxLeft->setScale((742.0 / 1280 * Director::getInstance()->getVisibleSize().width) / dboxLeft->getContentSize().width, (742.0 / 1280 * Director::getInstance()->getVisibleSize().width) / dboxLeft->getContentSize().width);
	dboxLeft->setPosition(95.0 / 1280 * Director::getInstance()->getVisibleSize().width, 30.0);
	guideWidget->addChild(dboxLeft);
	dboxLeft->setOpacity(0);
}
//右对话背景
void UIStoryLine::createDboxRight()
{
	dboxRight = Sprite::create();
	dboxRight->setTexture(DIALOG_PATH[DBOXRIGHT_PATH]);
	log("NoviceLayer::createDboxRight:0x%x, path:%s",(long)dboxRight,DIALOG_PATH[DBOXRIGHT_PATH].c_str());
	dboxRight->setAnchorPoint(Vec2(0,0));
	dboxRight->setScale((742.0 / 1280 * Director::getInstance()->getVisibleSize().width) / dboxLeft->getContentSize().width, (742.0 / 1280 * Director::getInstance()->getVisibleSize().width) / dboxLeft->getContentSize().width);
	dboxRight->setPosition(Director::getInstance()->getVisibleSize().width - 95.0 / 1280 * Director::getInstance()->getVisibleSize().width - dboxLeft->getBoundingBox().size.width, 30);
	guideWidget->addChild(dboxRight);
	dboxRight->setOpacity(0);
}
//点击提示图标
void UIStoryLine::createAnchr()
{
	anchr = Sprite::create();
	anchr->setTexture(DIALOG_PATH[ANCHR_PATH]);
	anchr->setPosition(dboxLeft->getPositionX() + dboxLeft->getBoundingBox().size.width - 83.0,dboxLeft->getPositionY() + 36.0);
	guideWidget->addChild(anchr);
	anchr->setOpacity(0);
	anchr->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0,10))),EaseBackOut::create(MoveBy::create(0.5, Vec2(0,-10))))));
}
//对话文本
void UIStoryLine::createText()
{
	ClippingNode *clippingNode = ClippingNode::create();
	clippingNode->setAlphaThreshold(0);
	clippingNode->setPosition(0,0);
	clippingNode->setAnchorPoint(Vec2(0,0));
	guideWidget->addChild(clippingNode);
    
	//文字
	titleTxt = Label::createWithSystemFont("",CUSTOM_FONT_NAME_3,30);
	titleTxt->setColor(Color3B(231, 192, 125));
	titleTxt->setAnchorPoint(Vec2(0,0));
	titleTxt->enableShadow();
	titleTxt->setWidth(540);
	titleTxt->setPosition(dboxLeft->getPositionX() + 88.0,dboxLeft->getPositionY() + dboxLeft->getBoundingBox().size.height - 76.0);
	guideWidget->addChild(titleTxt);

	txtTxt = Label::createWithSystemFont("",CUSTOM_FONT_NAME_2,FontSize);
	txtTxt->setColor(Color3B(238, 230, 202));
	txtTxt->setPosition(dboxLeft->getPositionX() + 88.0,dboxLeft->getPositionY() + dboxLeft->getBoundingBox().size.height - 86.0);
	txtTxt->enableShadow();
	txtTxt->setWidth(540);
	txtTxt->setAnchorPoint(Vec2(0,1));
	clippingNode->addChild(txtTxt);
    
	auto txt0 = Label::createWithSystemFont("", CUSTOM_FONT_NAME_2, FontSize);
	txt0->setWidth(540);
	txt0->setAnchorPoint(Vec2(0, 1));
	txt0->setString("test");
	m_nOneLineHeight = txt0->getBoundingBox().size.height;

	maskPic = Sprite::create();
	maskPic->setTexture(DIALOG_PATH[MASK_PATH]);
	maskPic->setAnchorPoint(Vec2(0,1));
	maskPic->setPosition(txtTxt->getPositionX(),txtTxt->getPositionY() + 2);
	maskPic->setTextureRect(Rect(0,0,txtTxt->getWidth(),m_nOneLineHeight * 3));
	clippingNode->setStencil(maskPic);

	//why use language related time? --语言不同 逐字显示的间隔不同
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
	case cocos2d::LanguageType::CHINESE:
		textTime = 0.1;
		break;
	default:
		textTime = 0.1 / 3;
		break;
	}
	//textTime = 0.1/3;
	log("textTime = %f",textTime);
}
//创建有对话支线的按钮
void UIStoryLine::createDialogBranchButton(int Num,bool threeBranch)
{
	branchTxtNum = Num;
	std::string dialog1 = "";
	std::string dialog2 = "";
	std::string dialog3 = "";
	if (SINGLE_HERO->m_iNation == 1 && dialogType == DIALOG_TYPE_MAIN_TASK)
	{
		switch (m_eStep)
		{
		case PORTUGAL_TASK_THREE_GOAL_ONE_DOCK1:
		{
												 //"是/否"
												 dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_THREE_SECTION_ONE_OPTION_ONE"];
												 dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_THREE_SECTION_ONE_OPTION_TWO"];
												 break;
		}
		case PORTUGAL_TASK_FOUR_GOAL_FIVE:
		{
												 if (SINGLE_HERO->m_iGender == 1)
												 {
													 dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_FOUR_SECTION_FIVE_OPTION_ONE"];
													 dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_FOUR_SECTION_FIVE_OPTION_TWO"];
												 }
												 else
												 {
													 dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_FOUR_SECTION_FIVE_LADY_OPTION_ONE"];
													 dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_FOUR_SECTION_FIVE_LADY_OPTION_TWO"];
												 }
												 break;
		}
		case PORTUGAL_TASK_FIVE_GOAL_FOUR:
		{
												dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_FIVE_SECTION_FOUR_OPTION_ONE"];
												dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_FIVE_SECTION_FOUR_OPTION_TWO"];
												break;
		}
		default:
			break;
		}
	}
	else if (dialogType == DIALOG_TYPE_PARTNER_RED_BEARD)
	{
		switch (m_ePartnerStep)
		{
		case PARTNER_RED_BEARD_ONE:
		case PARTNER_RED_BEARD_TWO:
		{
											   //"是/否"
											   dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_THREE_SECTION_ONE_OPTION_ONE"];
											   dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_THREE_SECTION_ONE_OPTION_TWO"];
											   break;
		}
		case PARTNER_RED_BEARD_EIGHT_GOAL_TWO:
		{
												dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_RED_BEARD_EIGHT_TWO_OPTION_ONE"];
												dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_RED_BEARD_EIGHT_TWO_OPTION_TWO"];
												dialog3 = SINGLE_SHOP->getTipsInfo()["TIP_RED_BEARD_EIGHT_TWO_OPTION_THREE"];
												break;
		}
		case PARTNER_RED_BEARD_NINE_GOAL_ONE:
		{
												dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_RED_BEARD_NINE_ONE_OPTION_ONE"];
												dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_RED_BEARD_NINE_ONE_OPTION_TWO"];
												break;
		}
		default:
			break;
		}
	}

	zhixianBgPic = Sprite::create();
	zhixianBgPic->setTexture("login_ui/start_720/option_bg.png");
	zhixianBgPic->setPosition(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(zhixianBgPic);
	zhixianBgPic->setVisible(false);

	branchButton1 = Button::create();
	branchButton1->setName("branchButton1");
	//branchButton1->setTitleFontName("fonts/LBRITEDI.TTF");
	branchButton1->setTitleFontSize(24);
	branchButton1->setTitleColor(Color3B(215, 190, 51));
	branchButton1->setTitleText(dialog1);
	branchButton1->ignoreContentAdaptWithSize(false);
	branchButton1->setContentSize(Size(534, 60));
	branchButton1->ignoreAnchorPointForPosition(false);
	branchButton1->addTouchEventListener(CC_CALLBACK_2(UIStoryLine::branchClickEvent,this));
	zhixianBgPic->addChild(branchButton1);
	branchButton1->setTouchEnabled(false);

	branchButton2 = Button::create();
	branchButton2->setName("branchButton2");
	//branchButton2->setTitleFontName("fonts/LBRITEDI.TTF");
	branchButton2->setTitleFontSize(24);
	branchButton2->setTitleColor(Color3B(215, 190, 51));
	branchButton2->setTitleText(dialog2);
	branchButton2->ignoreContentAdaptWithSize(false);
	branchButton2->setContentSize(Size(534, 60));
	branchButton2->ignoreAnchorPointForPosition(false);
	branchButton2->addTouchEventListener(CC_CALLBACK_2(UIStoryLine::branchClickEvent,this));
	zhixianBgPic->addChild(branchButton2);
	branchButton2->setTouchEnabled(false);
	auto startY = 152;
	if (threeBranch)
	{
		startY = 182;
		branchButton3 = Button::create();
		branchButton3->setName("branchButton3");
		//branchButton3->setTitleFontName("fonts/LBRITEDI.TTF");
		branchButton3->setTitleFontSize(24);
		branchButton3->setTitleColor(Color3B(215, 190, 51));
		branchButton3->setTitleText(dialog3);
		branchButton3->ignoreContentAdaptWithSize(false);
		branchButton3->setContentSize(Size(534, 60));
		branchButton3->ignoreAnchorPointForPosition(false);
		branchButton3->setPosition(Vec2(265, startY - 120));
		branchButton3->addTouchEventListener(CC_CALLBACK_2(UIStoryLine::branchClickEvent,this));
		zhixianBgPic->addChild(branchButton3);
		branchButton3->setTouchEnabled(false);
	}
	branchButton1->setPosition(Vec2(265, startY));
	branchButton2->setPosition(Vec2(265, startY - 60));
}
//设定播放对话定时器
void UIStoryLine::openText()
{
	 this->schedule(schedule_selector(UIStoryLine::showText),textTime);
}
//获取每个字的字节大小
static int getUtf8CharLen(const char*utf8){
	int plusNum = 0;
	if (utf8 && utf8[0]){
		plusNum = 1;
		unsigned char byte = (unsigned char)utf8[0];
		if (byte > 0xf0 && byte <= 0xf7) {
			plusNum = 4;
		}
		else if (byte > 0xe0 && byte <= 0xef)
		{
			plusNum = 3;
		}
		else if (byte > 0xb0 && byte <= 0xcf)
		{
			plusNum = 2;
		}
	}
	return plusNum;
}
//播放对话的定时器调用的方法，用来逐字显示对话
void UIStoryLine::showText(float f)
{
	std::string	text = txtTxt->getString();
    if (lenBoo == true)
	{
        lenBoo = false;
        len = len - plusNum;
        //addjust = 1;
        lenNum = len + addjust;
	}
    else
	{
		len = lenNum + text.length() - addjust;
	}
    if(len < 0)
    	return;
	plusNum = getUtf8CharLen(mystr.c_str() + len);
	std::string showT;
	std::string	nextC = mystr.substr(len,1);
	
    len = len + plusNum;
	maxLen = mystr.length();
	showT = mystr.substr(lenNum,len - lenNum);
    txtTxt->setString(showT);
	
	if (len >= maxLen || txtTxt->getBoundingBox().size.height > m_nOneLineHeight * 3)
	{
        addjust = 0;
        //显示点击提示
        anchr->setOpacity(255);
		if(asideBg)
		{
		   if (asideBg->getOpacity() == 255) 
			{
			   anchr->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width - 80, Director::getInstance()->getVisibleOrigin().y + 40);
			}
		}
		if(dboxLeft)
		{
			if (dboxLeft->getOpacity() == 255) 
			{
				anchr->setPosition(dboxLeft->getPositionX() + dboxLeft->getBoundingBox().size.width - 83.0,dboxLeft->getPositionY() + 36.0);
			}
		}
		if(dboxRight)
		{
			if (dboxRight->getOpacity() == 255) 
			{
				anchr->setPosition(dboxRight->getPositionX() + dboxRight->getBoundingBox().size.width - 83.0,dboxRight->getPositionY() + 36.0);
			}
		}
		//判断是否需要翻页
        if (txtTxt->getBoundingBox().size.height > m_nOneLineHeight * 3)
		{
            lenBoo = true;
			showT = mystr.substr(lenNum,len - plusNum - lenNum);
            txtTxt->setString(showT);
		}
		
		this->unschedule(schedule_selector(UIStoryLine::showText));
        return;
	}
}
//读取lua的json文件 （海上引导、战斗引导、战斗失败引导对话用到的）
void UIStoryLine::readJson()
{
	std::string jsonpath = findJsonFile("res_lua/lua_config/Lisben_Scene");
	//葡萄牙
	if (SINGLE_HERO->m_iNation ==1)
	{
		 jsonpath = findJsonFile("res_lua/lua_config/Lisben_Scene");
	}
	//西班牙
	else if (SINGLE_HERO->m_iNation == 2)
	{
		jsonpath = findJsonFile("res_lua/lua_config/Xibanya_Scene");
	}
	//英国
	else if (SINGLE_HERO->m_iNation == 3)
	{
		jsonpath = findJsonFile("res_lua/lua_config/England_Scene");
	}
	//荷兰
	else if (SINGLE_HERO->m_iNation == 4)
	{
		jsonpath = findJsonFile("res_lua/lua_config/Helan_Scene");
	}
	//热那亚
	else if (SINGLE_HERO->m_iNation == 5)
	{
		jsonpath = findJsonFile("res_lua/lua_config/Renaya_Scene");
	}
	
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
	rapidjson::Document root;
	if(root.Parse<0>(contentStr.c_str()).HasParseError()){
		return;
	}
	auto count = DictionaryHelper::getInstance()->getArrayCount_json(root,"pname");
	
	for (int i = 0; i < count;i++)
	{
		const auto& dic = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root,"pname",i);
		char buffer[32];
		snprintf(buffer,sizeof(buffer),"%d",i+1);
		auto content = DictionaryHelper::getInstance()->getStringValue_json(dic,buffer);
		std::string str = (char*)content;
		std::string old_value = "[heroname]";
		std::string new_value = JOHN;
		repalce_all_ditinct(str,old_value,new_value);
		old_value = "[aidename]";
		new_value = JOANNA;
		repalce_all_ditinct(str,old_value,new_value);
		m_dialogNames.push_back(str);
		//log("******** %s ***********", m_dialogNames[i].c_str());
	}
	
	auto dialog1_count = DictionaryHelper::getInstance()->getArrayCount_json(root,"dialog1");
	for (int i = 0; i < dialog1_count;i++)
	{
		const auto& dic = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root,"dialog1",i);
		int idx = 1;
		auto content = DictionaryHelper::getInstance()->getStringValue_json(dic,"1");
		if(!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic,"2");
			idx = 2;
		}
		if(!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic,"3");
			idx = 3;
		}
		if(!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic,"0");
			idx = 0;
		}
		std::string str = (char*)content;
		std::string old_value = "[heroname]";
		std::string new_value = JOHN;
		repalce_all_ditinct(str,old_value,new_value);
		old_value = "[aidename]";
		new_value = JOANNA;
		repalce_all_ditinct(str,old_value,new_value);
		if(content != 0){
			auto item = new DialogData;
			item->dialog = str;
			
			item->personIdx = idx;
			m_dialogVector.push_back(item);
		}
		//log("------------%d",i);
		//log("////////////%d",m_dialogVector[i]->personIdx);
		//log("********%s***********%s",m_dialogVector[i]->dialog.c_str(),JOANNA.c_str());
	}
	//if(m_dialogVector[0]->personIdx > 0)
	//m_dialogNames[m_dialogVector[0]->personIdx-1];	 m_dialogVector[0]->dialog;
}
//返回当前用到的lua json
std::string UIStoryLine::findJsonFile(std::string name){
	std::string jsonPath;
	LanguageType nType = LanguageType(Utils::getLanguage());
	char buffer[10];
	sprintf(buffer,"%d",ProtocolThread::GetInstance()->getIcon());
	JOHN = ProtocolThread::GetInstance()->getHeroName();
	if (SINGLE_HERO->m_iGender == 1)
	{
		JOANNA = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"];
	}
	else
	{
		JOANNA = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"];
	}
	switch (nType)
	{
	case cocos2d::LanguageType::CHINESE:
		if (cocos2d::CCFileUtils::getInstance()->isFileExist(name + ".zh_CN" + ".json"))
		{
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".zh_CN" + ".json");

		}
		else
		{
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".json");
		}
		break;
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
		if (cocos2d::CCFileUtils::getInstance()->isFileExist(name + ".zh_HK" + ".json"))
		{
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".zh_HK" + ".json");

		}
		else
		{
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".json");
		}
		break;
	default:
		jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".json");
		break;
	}
	return jsonPath;
}
//显示旁白背景
void UIStoryLine::showAside()
{
	asideBg->runAction(FadeIn::create(0.2));
}
//显示海上背景
void UIStoryLine::showSeaBoatBg()
{
	seaBoatBg->runAction(FadeIn::create(0.3));
}
//对话中支线的点击事件
void UIStoryLine::branchClickEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		zhixianBgPic->setVisible(false);
		auto target = (Widget*)pSender;
		auto name = target->getName();
		target->setTouchEnabled(false);
		if (name == "branchButton1")
		{
			txtNum = branchTxtNum;
		}
		else if (name == "branchButton2")
		{
			txtNum = branchTxtNum + 1;
		}
		else if (name == "branchButton3")
		{
			txtNum = branchTxtNum + 2;
		}
		if (SINGLE_HERO->m_iNation == 1 && dialogType == DIALOG_TYPE_MAIN_TASK)
		{
			if (SINGLE_HERO->m_iGender == 2 && branchTxtNum == 59)
			{
				dboxLeft->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (branchTxtNum == 2)
				{
					m_eName = HERO;
					dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxRight->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
		}
		else
		{
			switch (branchTxtNum)
			{
			case 9:
			case 15:
			case 93:
			case 124:
			{
					  m_eName = HERO;
					  if (branchTxtNum == 93)
					  {
						  if (name == "branchButton2")
						  {
							  txtNum = branchTxtNum + 4;
						  }
						  else if (name == "branchButton3")
						  {
							  txtNum = branchTxtNum + 9;
						  }
					  }
					  dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			default:
				break;
			}
		}
	}
}
//显示插画时定时器调用的函数
void UIStoryLine::showIllustration(float f)
{
	IllustrationNum = IllustrationNum + 1;
	if (IllustrationNum == 2)
	{
		this->unschedule(schedule_selector(UIStoryLine::showIllustration));
		if(txtNum == 5)
		{
			SINGLE_AUDIO->vplayMusicByType(MUSIC_AFTER_BATTLE);
			majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5),MoveBy::create(0.5, Vec2(OFFSET, 0))));
			dboxLeft->runAction(Sequence::create(DelayTime::create(1),FadeIn::create(0.3),CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog,this)),nullptr));
		}
		if(txtNum == 66 || txtNum == 70)
		{
			AidePic->runAction(MoveBy::create(0.5,Vec2(OFFSET,0)));
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.5),FadeIn::create(0.3),CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog,this)),nullptr));
		}
	}
}

//剧情对话人物出场 为动作进行先后而调用
void UIStoryLine::personCome()
{
	switch (m_eName)
	{
	case AIDE:
		AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case HERO:
		HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case MAJORFIGURE:
		majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case OFFICER:
		OfficerPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case PASENGERA:
		pasengerAPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case SAILOR:
		supportingRolePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case SOLDIER:
		supportingRolePic->setPosition(-supportingRolePic->getBoundingBox().size.width, 0);
		supportingRolePic->setFlippedX(false);
		supportingRolePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	default:
		break;
	}
}
//剧情对话人物对话显示 为动作进行先后而调用
void UIStoryLine::personSpeak()
{
	titleTxt->setOpacity(255);
	txtTxt->setOpacity(255);
	
	if(m_eStep == SALLING_GUID || m_eStep == WAR_GUID)
	{
		titleTxt->setString(m_dialogNames[m_dialogVector[txtNum]->personIdx-1]);
		mystr = m_dialogVector[txtNum]->dialog.c_str();
	}
	else
	{
		titleTxt->setString(m_jsonName[txtNum]);
		mystr = m_jsonDialog[txtNum];
	}
	txtTxt->setString("");
	lenNum = 0;
	openText(); 
	layerFarmClickBoo = false;
}

/*
 * 对话句子按照点击继续下一句 直到结束删除对话层 传给服务器结束剧情
 * 包括主线剧情、触发剧情
 * 主线剧情时，先排除是否为海上引导、战斗引导和战败引导的索引，其他根据国家来走剧情
 */
void UIStoryLine::dialogSentenceStepByStep(int Num)
{
	log("dialogSentenceStepByStep %d %d", Num, dialogType);
	txtNum = Num;
	this->unschedule(schedule_selector(UIStoryLine::showText));
	anchr->setOpacity(0);
	switch (dialogType)
	{
	case DIALOG_TYPE_MAIN_TASK:
	{
			  //主线剧情时，先排除是否为海上引导、战斗引导和战败引导的索引
			  if (m_eStep == SALLING_GUID || m_eStep == WAR_GUID || m_eStep == WAR_FAIL_GUID)
			  {
				  switch (m_eStep)
				  {
				  case SALLING_GUID:
					  if (txtNum > 67)
					  {
						  //this->removeChild(guideWidget);
						  guideWidget->removeFromParentAndCleanup(true);
						  reset();
						  //log("******** %d ********** %d ************ %d", SINGLE_HERO->m_iNation, CITY_POSITION[17 - 1][0], CITY_POSITION[17 - 1][1]);
						  //different nation ,mask and handPic have different position
						  createSaillingHelp();
					  }
					  else
					  {
						  if (txtNum == 65)
						  {
							  dboxLeft->setOpacity(0);
							  titleTxt->setString("");
							  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
							  AidePic->runAction(Sequence::create(MoveBy::create(0.5, Vec2(-OFFSET, 0)), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showAside, this)), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
						  }
						  else
						  {
							  UIStoryLine::showDialog();
						  }
					  }
					  break;
				  case WAR_GUID:
					  if (txtNum > 72)
					  {
						  UIStoryLine::createWarGuideHelp();
					  }
					  else
					  {
						  if (txtNum == 68)
						  {
							  dboxLeft->setOpacity(0);
							  titleTxt->setString("");
							  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
							  AidePic->runAction(Sequence::create(MoveBy::create(0.5, Vec2(-OFFSET, 0)), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showAside, this)), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
						  }
						  else
						  {
							  UIStoryLine::showDialog();
						  }
					  }
					  break;
				  case WAR_FAIL_GUID:
					  if (txtNum == 82)
					  {
						  //海上战斗
						  if (guideWidget->getCameraMask() == 4)
						  {
							  auto curScene = Director::getInstance()->getRunningScene();
							  auto bm = dynamic_cast<TVBattleManager*>(curScene->getChildByName("BattleManage"));
							  if (bm)
							  {
									SINGLE_HERO->m_pBattelData = bm->getFightResult();
									SINGLE_HERO->shippos = bm->getShipPos();
									ProtocolThread::GetInstance()->forceFightStatus();
									CHANGETO(SCENE_TAG::BATTLE_TAG);
									bm->resetEngageInFightData();
							  }
						  }
						  guideWidget->removeFromParentAndCleanup(true);
						  reset();
					  }
					  else
					  {
						  UIStoryLine::showDialog();
					  }
					  break;
				  default:
					  break;
				  }
			  }
			  else
			  {
				  switch (SINGLE_HERO->m_iNation)
				  {
				  case 1:
					  if (SINGLE_HERO->m_iGender == 1)
					  {
						  UIStoryLine::dialogSpeakOrder1_1();
					  }
					  else
					  {
						  UIStoryLine::dialogSpeakOrder1_2();
					  }
					  break;
				  case 2:
					  if (SINGLE_HERO->m_iGender == 1)
					  {
						  UIStoryLine::dialogSpeakOrder2_1();
					  }
					  else
					  {
						  UIStoryLine::dialogSpeakOrder2_2();
					  }
					  break;
				  case 3:
					  UIStoryLine::dialogSpeakOrder3_0();
					  break;
				  case 4:
					  if (SINGLE_HERO->m_iGender == 1)
					  {
						  UIStoryLine::dialogSpeakOrder4_1();
					  }
					  else
					  {
						  UIStoryLine::dialogSpeakOrder4_2();
					  }
					  break;
				  case 5:
					  UIStoryLine::dialogSpeakOrder5_0();
					  break;
				  default:
					  break;
				  }
			  }
			  break;
	}
	case DIALOG_TYPE_SMALL_STORY:
	{
			 //触发剧情对话播放顺序
			 UIStoryLine::triggerDialogSpeakOrder();
			 break;
	}
	case DIALOG_TYPE_PARTNER_RED_BEARD:
	{
			 //小伙伴红胡子剧情对话播放顺序
			 UIStoryLine::partnerRedBeardDialogSpeakOrder();
			 break;
	}
	default:
		break;
	}
}
//葡萄牙男版对话播放顺序
void UIStoryLine::dialogSpeakOrder1_1()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 7:
		case 24:
		case 39:
		case 53:
		case 65:
		case 67:
		case 69:
		{
				   if (txtNum == 53)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   }
				   else
				   {
					   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   }
				   break;
		}
		case 4:
		{
				  //选项二 返回主城
				  guideWidget->setCascadeOpacityEnabled(true);
				  guideWidget->setCascadeColorEnabled(true);
				  guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidgetOnly)), nullptr));
				  return;
		}
		case 3:
		{
				  //people back and change the background 选项一
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				  AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  dboxRight->runAction(FadeOut::create(0.01));
				  txtNum = 4;
				  SINGLE_AUDIO->vplayMusicByType(MUSIC_SAILING);
				  dockBg->runAction(Sequence::create(TintTo::create(0.5, 10, 10, 10), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showSeaBoatBg, this)), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showAside, this)),
					  DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				  break;
		}
		case 66:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   dboxRight->runAction(FadeOut::create(0.01));
				   CapeVerdePic->runAction(TintTo::create(0.4, 10, 10, 10));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_STORM_02);
				   if (dialogBg)
					   dialogBg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4), FadeIn::create(0.3)));
				   this->schedule(schedule_selector(UIStoryLine::showIllustration), 1);
				   break;
		}
		case 1:
		case 5:
		case 6:
		case 8:
		case 17:
		case 22:
		case 30:
		case 31:
		case 32:
		case 37:
		case 38:
		case 49:
		case 50:
		case 51:
		case 64:
		case 68:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		{
				   //剧情结束处理
				   if (txtNum == 5)
				   {
					   if (MainTaskManager::GetInstance()->m_PartsInfo && MainTaskManager::GetInstance()->m_PartsInfo->has_npc)
					   {
						   guideWidget->removeFromParentAndCleanup(true);
						   reset();
						   int npc_id = MainTaskManager::GetInstance()->m_PartsInfo->npc_id;
						   registerCallBack();
						   ProtocolThread::GetInstance()->engageInFightForTask(npc_id, UILoadingIndicator::create(this));
						   return;
					   }
				   }
				   auto curScene = Director::getInstance()->getRunningScene();
				   //海上取消暂停
				   if (guideWidget->getCameraMask() == 4)
				   {
					   auto curScene = Director::getInstance()->getRunningScene();
					   auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
					   if (maps)
					   {
						   maps->seaEventEnd();
					   }
				   }
				   guideWidget->setCascadeOpacityEnabled(true);
				   guideWidget->setCascadeColorEnabled(true);
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 9:
		case 42:
		{
				   if (txtNum == 42)
				   {
					   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 12:
		case 15:
		case 44:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 14:
		case 46:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 18:
		case 33:
		case 52:
		{
				   if (txtNum != 52)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   }
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 16:
		case 25:
		case 28:
		{
				   if (txtNum == 16)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   AidePic->setPosition(-AidePic->getBoundingBox().size.width, 0);
					   AidePic->setFlippedX(false);
				   }
				   if (txtNum == 28)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 26:
		case 29:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 35:
		{
				   txtNum = 36;
				   break;
		}
		case 47:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
				   HeroPic->setFlippedX(false);
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 48:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 58:
		{
				   txtNum = 59;
				   break;
		}
		default:
			break;
		}
	}
	titleTxt->setString("");
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[4] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[0] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[0] && majorFigurePic)
	{
		if (txtNum != 5)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 48)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 9 || txtNum == 11 || txtNum == 14 || txtNum == 42 || txtNum == 46)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (txtNum != 66)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 2 || txtNum == 34 || txtNum == 57)
		{
			//show branch
			UIStoryLine::showBranch();
		}
		else
		{
			if (txtNum != 3)
			{
				if (txtNum == 47)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					if (txtNum == 53)
					{
						dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
					else
					{
						dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
				}
			}
		}
	}
}
//葡萄牙女版对话播放顺序
void UIStoryLine::dialogSpeakOrder1_2()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 7:
		case 9:
		case 24:
		case 26:
		case 36:
		case 54:
		case 69:
		case 71:
		case 73:
		{
				   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 4:
		{
				  //选项二 返回主城
				  guideWidget->setCascadeOpacityEnabled(true);
				  guideWidget->setCascadeColorEnabled(true);
				  guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidgetOnly)), nullptr));
				  return;
		}
		case 3:
		{
				  //people back and change the background
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  dboxRight->runAction(FadeOut::create(0.01));
				  txtNum = 4;
				  SINGLE_AUDIO->vplayMusicByType(MUSIC_SAILING);
				  dockBg->runAction(Sequence::create(TintTo::create(0.5, 10, 10, 10), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showSeaBoatBg, this)), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showAside, this)), 
					  DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				  break;
		}
		case 70:
		{
				   //暴风雨插画显示
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   dboxRight->runAction(FadeOut::create(0.01));
				   CapeVerdePic->runAction(TintTo::create(0.4, 10, 10, 10));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_STORM_02);
				   if (dialogBg)
					   dialogBg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4), FadeIn::create(0.3)));
				   this->schedule(schedule_selector(UIStoryLine::showIllustration), 1);
				   break;
		}
		case 1:
		case 5:
		case 6:
		case 8:
		case 17:
		case 23:
		case 33:
		case 34:
		case 35:
		case 41:
		case 42:
		case 51:
		case 52:
		case 53:
		case 68:
		case 72:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
		{
				   //剧情结束处理
				   if (txtNum == 5)
				   {
					   if (MainTaskManager::GetInstance()->m_PartsInfo && MainTaskManager::GetInstance()->m_PartsInfo->has_npc)
					   {
						   guideWidget->removeFromParentAndCleanup(true);
						   reset();
						   int npc_id = MainTaskManager::GetInstance()->m_PartsInfo->npc_id;
						   registerCallBack();
						   ProtocolThread::GetInstance()->engageInFightForTask(npc_id, UILoadingIndicator::create(this));
						   return;
					   }
				   }
				   auto curScene = Director::getInstance()->getRunningScene();
				   //海上取消暂停
				   if (guideWidget->getCameraMask() == 4)
				   {
					   auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
					   if (maps)
					   {
						   maps->seaEventEnd();
					   }
				   }
				   guideWidget->setCascadeOpacityEnabled(true);
				   guideWidget->setCascadeColorEnabled(true);
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 12:
		case 43:
		{
				   if (txtNum == 12)
				   {
					   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 15:
		case 46:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 16:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->setPosition(-AidePic->getBoundingBox().size.width, 0);
				   AidePic->setFlippedX(false);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 18:
		case 55:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 30:
		case 49:
		{
				   if (txtNum == 49)
				   {
					   AidePic->setPosition(-AidePic->getBoundingBox().size.width, 0);
				   }
				   else
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 31:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 39:
		{
				   txtNum = 40;
				   break;
		}
		case 50:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 60:
		{
				   txtNum = 61;
				   break;
		}
		default:
			break;
		}
	}
	titleTxt->setString("");
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[4] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[0] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[0] && majorFigurePic)
	{
		if (txtNum == 5 || txtNum == 41)
		{
		}
		else
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 50)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 12 || txtNum == 14 || txtNum == 43 || txtNum == 45)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (txtNum != 70)
			{
				if (txtNum == 59)
				{
					UIStoryLine::showBranch();
				}
				else
				{
					if (txtNum == 16)
					{
						dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
					else
					{
						dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
				}
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 2 || txtNum == 38)
		{
			//show branch
			UIStoryLine::showBranch();
		}
		else
		{
			if (txtNum == 80)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
}
//西班牙男版对话播放顺序
void UIStoryLine::dialogSpeakOrder2_1()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 2:
		case 4:
		case 25:
		case 27:
		case 37:
		case 39:
		case 50:
		case 54:
		case 58:
		case 62:
		case 76:
		case 84:
		case 87:
		case 93:
		case 96:
		case 99:
		{
				   if (txtNum == 96)
				   {
					   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 1:
		case 3:
		case 12:
		case 13:
		case 22:
		case 23:
		case 24:
		case 26:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 41:
		case 49:
		case 73:
		case 74:
		case 75:
		case 79:
		case 97:
		case 102:
		{
					//剧情结束处理

					auto curScene = Director::getInstance()->getRunningScene();
					//海上取消暂停
					if (guideWidget->getCameraMask() == 4)
					{
						auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
						if (maps)
						{
							maps->seaEventEnd();
						}
					}
					guideWidget->setCascadeOpacityEnabled(true);
					guideWidget->setCascadeColorEnabled(true);
					guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidget)), nullptr));

					return;
		}
		case 6:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				  AidePic->setFlippedX(true);
				  AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  break;
		}
		case 7:
		{
				  AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 9:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 11:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 14:
		case 40:
		case 42:
		case 80:
		case 83:
		case 86:
		{
				   if (txtNum == 86)
				   {
					   SINGLE_AUDIO->vpasueAllEffect();
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
					   judgePic->runAction(FadeOut::create(0.01));
					   wineHouseBg->runAction(FadeIn::create(0.3));
					   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   else
				   {
					   if (txtNum == 83)
					   {
						   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
						   SINGLE_AUDIO->vplayMusicByType(MUSIC_CITY);
						   boatteamPic->runAction(FadeOut::create(0.01));
						   dialogBg->runAction(FadeIn::create(0.3));
					   }
					   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 16:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   pasengerAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 17:
		case 19:
		{
				   pasengerAPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   pasengerBPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 18:
		{
				   pasengerAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   pasengerBPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 21:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   pasengerBPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 38:
		case 52:
		case 82:
		case 85:
		case 92:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   switch (txtNum)
				   {
				   case 85:
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   break;
				   case 92:
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   SINGLE_AUDIO->vplayMusicByType(MUSIC_PALACE);
					   wineHouseBg->runAction(FadeOut::create(0.01));
					   palaceBg->runAction(FadeIn::create(0.3));
					   kingPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
					   break;
				   default:
					   break;
				   }
				   break;
		}
		case 43:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 51:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 53:
		case 98:
		{
				   if (txtNum == 98)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 57:
		{
				   //旁白 人物退场
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 59:
		case 94:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   if (txtNum == 94)
				   {
					   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   }
				   break;
		}
		case 61:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   cateBedroomPic->runAction(FadeOut::create(0.1));
				   wineHouseBg->runAction(Spawn::createWithTwoActions(FadeIn::create(0.1), TintTo::create(0.1, 255, 255, 255)));
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	titleTxt->setString("");
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[38] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[16] && pasengerAPic)
		{
			pasengerAPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[17] && pasengerBPic)
		{
			pasengerBPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[92] && kingPic)
		{
			kingPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[94] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[38] && asideBg)
	{
		if (txtNum == 57)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeWineHouseBg, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else if (txtNum == 59)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::dayAndNight,this)),FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else if (txtNum == 60)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeCateBedroomPic, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else if (txtNum == 82)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeSeaBoatBg, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else if (txtNum == 85)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeDialogBg, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[3] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 6 || txtNum == 9 || txtNum == 11)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (txtNum == 86)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[16] && pasengerAPic)
	{
		pasengerAPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[17] && pasengerBPic)
	{
		pasengerBPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[92] && kingPic)
	{
		kingPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 92)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.3), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.3), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[94] && supportingRolePic)
	{
		supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.3), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
}
//西班牙女版对话播放顺序
void UIStoryLine::dialogSpeakOrder2_2()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 2:
		case 4:
		case 25:
		case 27:
		case 37:
		case 39:
		case 60:
		case 64:
		case 68:
		case 82:
		case 90:
		case 93:
		case 99:
		case 102:
		case 105:
		{
					if (txtNum == 102)
					{
						supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					}
					HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
					break;
		}
		case 1:
		case 3:
		case 12:
		case 13:
		case 22:
		case 23:
		case 24:
		case 26:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 41:
		case 49:
		case 79:
		case 80:
		case 81:
		case 85:
		case 103:
		case 108:
		{
					//剧情结束处理

					auto curScene = Director::getInstance()->getRunningScene();
					//海上取消暂停
					if (guideWidget->getCameraMask() == 4)
					{
						auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
						if (maps)
						{
							maps->seaEventEnd();
						}
					}
					guideWidget->setCascadeOpacityEnabled(true);
					guideWidget->setCascadeColorEnabled(true);
					guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidget)), nullptr));

					return;
		}
		case 6:
		{
					HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
					AidePic->setFlippedX(true);
					AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
					break;
		}
		case 7:
		{
				  AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 9:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 11:
		{
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 14:
		case 40:
		case 42:
		case 86:
		case 89:
		case 92:
		{
				   if (txtNum == 92)
				   {
					   SINGLE_AUDIO->vpasueAllEffect();
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
					   judgePic->runAction(FadeOut::create(0.01));
					   wineHouseBg->runAction(FadeIn::create(0.3));
					   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   else
				   {
					   if (txtNum == 89)
					   {
						   SINGLE_AUDIO->vplayMusicByType(MUSIC_CITY);
						   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
						   boatteamPic->runAction(FadeOut::create(0.01));
						   dialogBg->runAction(FadeIn::create(0.3));
					   }
					   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 16:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   pasengerAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 17:
		case 19:
		{
				   pasengerAPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   pasengerBPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 18:
		{
				   pasengerAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   pasengerBPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 21:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   pasengerBPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 38:
		case 88:
		case 91:
		case 98:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   if (txtNum == 98)
				   {
					   SINGLE_AUDIO->vplayMusicByType(MUSIC_PALACE);
					   wineHouseBg->runAction(FadeOut::create(0.01));
					   palaceBg->runAction(FadeIn::create(0.3));
					   kingPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   break;
		}
		case 43:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 50:
		case 59:
		case 104:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 58:
		case 63:
		{
				   //旁白 人物退场
				   if (txtNum == 58)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					break;
		}
		case 65:
		case 100:
		{
					HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					if (txtNum == 100)
					{
						supportingRolePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					}
					else
					{
						SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					}
				   break;
		}
		case 67:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   cateBedroomPic->runAction(FadeOut::create(0.1));
				   wineHouseBg->runAction(Spawn::createWithTwoActions(FadeIn::create(0.1), TintTo::create(0.1, 255, 255, 255)));
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	titleTxt->setString("");
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[38] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[16] && pasengerAPic)
		{
			pasengerAPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[17] && pasengerBPic)
		{
			pasengerBPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[98] && kingPic)
		{
			kingPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[100] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[38] && asideBg)
	{
		if (txtNum == 63)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeWineHouseBg, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else if (txtNum == 65)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::dayAndNight, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else if (txtNum == 66)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeCateBedroomPic, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else if (txtNum == 88)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeSeaBoatBg, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else if (txtNum == 91)
		{
			asideBg->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeDialogBg, this)), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[3] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 6 || txtNum == 9 || txtNum == 11)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (txtNum == 92)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[16] && pasengerAPic)
	{
		pasengerAPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[17] && pasengerBPic)
	{
		pasengerBPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[98] && kingPic)
	{
		kingPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 98)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[100] && supportingRolePic)
	{
		supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
}
//英国男女版对话播放顺序
void UIStoryLine::dialogSpeakOrder3_0()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 2:
		case 4:
		case 16:
		case 18:
		case 30:
		case 32:
		case 52:
		case 54:
		case 56:
		case 58:
		case 60:
		{
				   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 1:
		case 3:
		case 12:
		case 13:
		case 14:
		case 15:
		case 17:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 31:
		case 35:
		case 36:
		case 37:
		case 55:
		case 59:
		case 62:
		{
				   //剧情结束处理

				   auto curScene = Director::getInstance()->getRunningScene();
				   //海上取消暂停
				   if (guideWidget->getCameraMask() == 4)
				   {
					   auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
					   if (maps)
					   {
						   maps->seaEventEnd();
					   }
				   }

				   guideWidget->setCascadeOpacityEnabled(true);
				   guideWidget->setCascadeColorEnabled(true);
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 6:
		case 38:
		{
				   if (txtNum == 6)
				   {
					   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 7:
		case 42:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 9:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 11:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 29:
		case 57:
		{
				   //海上背景换成突袭舰队插画
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   seaBoatBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   dialogBg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4), FadeIn::create(0.3)));
				   AidePic->runAction(Sequence::create(MoveBy::create(0.3, Vec2(-OFFSET, 0)), DelayTime::create(1), MoveBy::create(0.5, Vec2(OFFSET, 0)), nullptr));
				   if (txtNum == 57)
				   {
					   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 40:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   palaceBg->runAction(FadeIn::create(0.3));
				   kingPic->runAction(Sequence::create(DelayTime::create(0.6), MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100)), nullptr));
				   majorFigurePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   majorFigurePic->setFlippedX(true);
				   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(1), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   break;
		}
		case 41:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 44:
		case 53:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   if (txtNum == 53)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   AidePic->setPosition(-AidePic->getBoundingBox().size.width, 0);
					   AidePic->setFlippedX(false);
					   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   break;
		}
		case 51:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   kingPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   palaceBg->runAction(Sequence::create(TintTo::create(0.5, Color3B(100, 100, 100)), FadeOut::create(0.2), nullptr));
				   majorFigurePic->setPosition(-majorFigurePic->getBoundingBox().size.width, 0);
				   majorFigurePic->setFlippedX(false);
				   majorFigurePic->runAction(Sequence::create(DelayTime::create(1), MoveBy::create(0.5, Vec2(OFFSET, 0)), nullptr));
				   break;
		}
		default:
			break;
		}
	}
	titleTxt->setString("");
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[43] && kingPic)
		{
			kingPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[3] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 40 || txtNum == 44 || txtNum == 46 || txtNum == 48 || txtNum == 50)
		{
			if (txtNum == 40)
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(1.5), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 51)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(1.5), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[0] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 6 || txtNum == 9 || txtNum == 11 || txtNum == 38 || txtNum == 41)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (txtNum == 29 ||txtNum == 53 || txtNum == 57)
			{
				if (txtNum == 29)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(1.5), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[43] && kingPic)
	{
		kingPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
}
//荷兰男版对话播放顺序
void UIStoryLine::dialogSpeakOrder4_1()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 2:
		case 4:
		case 18:
		case 33:
		case 36:
		case 43:
		case 61:
		case 64:
		case 68:
		case 80:
		case 90:
		{
				   if (txtNum == 33 || txtNum == 90)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   }
				   else
				   {
					   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   }
				   break;
		}
		case 1:
		case 3:
		case 12:
		case 13:
		case 14:
		case 15:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 41:
		case 57:
		case 58:
		case 59:
		case 89:
		case 91:
		case 92:
		{
				   //剧情结束处理

				   auto curScene = Director::getInstance()->getRunningScene();
				   //海上取消暂停
				   if (guideWidget->getCameraMask() == 4)
				   {
					   auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
					   if (maps)
					   {
						   maps->seaEventEnd();
					   }
				   }
				   guideWidget->setCascadeOpacityEnabled(true);
				   guideWidget->setCascadeColorEnabled(true);
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 6:
		case 19:
		case 48:
		case 84:
		{
				   if (txtNum == 6 || txtNum == 19 || txtNum == 84)
				   {
					   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   if (txtNum == 48)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   }
				   else
				   {
					   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   }
				   break;
		}
		case 7:
		case 21:
		case 24:
		case 56:
		case 85:
		{
				   if (txtNum == 56)
				   {
					   HeroPic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
					   HeroPic->setFlippedX(true);
				   }
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 9:
		case 22:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 11:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 34:
		case 37:
		case 60:
		case 73:
		{
				   if (txtNum != 37)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   else
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 35:
		{
				   //海上背景换成新地岛插画 对话为旁白
				   seaBoatBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   //if (dialogBg)
					   dialogBg->runAction(FadeIn::create(0.3));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 40:
		case 62:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 42:
		case 49:
		case 65:
		case 69:
		case 75:
		case 81:
		{
				   if (txtNum == 42 || txtNum == 75)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   else
				   {
					   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 47:
		case 52:
		case 63:
		case 67:
		case 71:
		case 76:
		case 88:
		{
				   switch (txtNum)
				   {
				   case 52:
					   HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
					   HeroPic->setFlippedX(false);
					   break;
				   case 67:
					   svalbardBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   //if (dialogBg)
						   dialogBg->runAction(FadeIn::create(0.3));
					   break;
				   case 76:
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
					   break;
				   case 88:
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   break;
				   default:
					   break;
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 53:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 72:
		{
				   //水手入场
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   supportingRolePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   break;
		}
		case 74:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 77:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}

	titleTxt->setString("");
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[72] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[32] && asideBg)
	{
		if (txtNum == 35 || txtNum == 67)
		{
			asideBg->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (asideBg->getOpacity() == 255)
			{
				asideBg->runAction(CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)));
			}
			else
			{
				asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else
	{
		if (asideBg)
		{
			asideBg->runAction(FadeOut::create(0.1));
		}
		if (m_jsonName[txtNum] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 40 || txtNum == 42 || txtNum == 62 || txtNum == 75)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 6 || txtNum == 9 || txtNum == 11 || txtNum == 19 || txtNum == 22 || txtNum == 48 || txtNum == 50 || txtNum == 54 || txtNum == 84)
			{
				if (txtNum == 48)
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
			else
			{
				if (txtNum == 14 || txtNum == 34 || txtNum == 60 || txtNum == 76)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 52)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum == 33 || txtNum == 90)
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[72] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
}
//荷兰女版对话播放顺序
void UIStoryLine::dialogSpeakOrder4_2()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 2:
		case 4:
		case 18:
		case 33:
		case 36:
		case 43:
		case 48:
		case 57:
		case 60:
		case 64:
		case 76:
		case 85:
		{
				   if (txtNum == 33 || txtNum == 85)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   }
				   else
				   {
					   if (txtNum == 48)
					   {
						   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
						   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
					   }
					   if (txtNum == 76)
					   {
						   HeroPic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
						   HeroPic->setFlippedX(true);
					   }
					   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   }
				   break;
		}
		case 1:
		case 3:
		case 12:
		case 13:
		case 14:
		case 15:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 41:
		case 53:
		case 54:
		case 55:
		case 84:
		case 86:
		case 87:
		{
				   //剧情结束处理

				   auto curScene = Director::getInstance()->getRunningScene();
				   //海上取消暂停
				   if (guideWidget->getCameraMask() == 4)
				   {
					   auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
					   if (maps)
					   {
						   maps->seaEventEnd();
					   }
				   }
				   guideWidget->setCascadeOpacityEnabled(true);
				   guideWidget->setCascadeColorEnabled(true);
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 6:
		case 19:
		{
				   if (txtNum == 6 || txtNum == 19)
				   {
					   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 7:
		case 21:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 9:
		case 52:
		{
				   if (txtNum == 52)
				   {
					   HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
					   HeroPic->setFlippedX(false);
				   }
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 11:
		case 22:
		{
				   if (txtNum == 11)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   }
				   else
				   {
					   AidePic->setPosition(-AidePic->getBoundingBox().size.width, 0);
					   AidePic->setFlippedX(false);
					   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 34:
		case 37:
		case 56:
		{
				   if (txtNum != 37)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   else
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 35:
		{
				   //海上背景换成新地岛插画 对话为旁白
				   seaBoatBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   dialogBg->runAction(FadeIn::create(0.3));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 23:
		case 40:
		case 58:
		{
				   if (txtNum != 23)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   else
				   {
					   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 42:
		case 61:
		case 65:
		case 71:
		case 77:
		{
				   if (txtNum == 42)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   }
				   else
				   {
					   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 47:
		case 49:
		case 59:
		case 63:
		case 67:
		case 83:
		{
				   switch (txtNum)
				   {
				   case 49:
					   majorFigurePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
					   majorFigurePic->setFlippedX(true);
					   break;
				   case 63:
					   svalbardBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   dialogBg->runAction(FadeIn::create(0.3));
					   break;
				   case 83:
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   break;
				   default:
					   break;
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 68:
		{
				   //水手入场
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   supportingRolePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   break;
		}
		case 69:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
				   HeroPic->setFlippedX(false);
				   HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 70:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(false);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 72:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 73:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}

	titleTxt->setString("");
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[68] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[32] && asideBg)
	{
		if (txtNum == 35 || txtNum == 63)
		{
			asideBg->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (asideBg->getOpacity() == 255)
			{
				asideBg->runAction(CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)));
			}
			else
			{
				asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else
	{
		if (asideBg)
		{
			asideBg->runAction(FadeOut::create(0.1));
		}
		if (m_jsonName[txtNum] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 49 || txtNum == 51)
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum == 40 || txtNum == 42 || txtNum == 58)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 6 || txtNum == 9 || txtNum == 11 || txtNum == 19 || txtNum == 44 || txtNum == 46 || txtNum == 70)
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum == 14 || txtNum == 34 || txtNum == 56)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 52 || txtNum == 69 || txtNum == 72)
			{
				if (txtNum == 69 || txtNum == 72)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
			else
			{
				if (txtNum == 33 || txtNum == 85)
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[68] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
}
//热那亚男女版对话播放顺序
void UIStoryLine::dialogSpeakOrder5_0()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 2:
		case 4:
		case 16:
		case 18:
		case 33:
		case 54:
		case 59:
		case 63:
		{
				   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 1:
		case 3:
		case 12:
		case 13:
		case 14:
		case 15:
		case 17:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 34:
		case 49:
		case 50:
		case 51:
		case 52:
		case 62:
		case 64:
		case 65:
		case 66:
		{
				   //剧情结束处理

				   auto curScene = Director::getInstance()->getRunningScene();
				   //海上取消暂停
				   if (guideWidget->getCameraMask() == 4)
				   {
					   auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
					   if (maps)
					   {
						   maps->seaEventEnd();
					   }
				   }
				   guideWidget->setCascadeOpacityEnabled(true);
				   guideWidget->setCascadeColorEnabled(true);
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 6:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 7:
		{
				  AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 9:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 11:
		case 35:
		case 38:
		{
				   if (txtNum == 35)
				   {
					   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   palaceBg->runAction(FadeIn::create(0.3));
					   SimpleAudioEngine::getInstance()->stopBackgroundMusic();
					   SINGLE_AUDIO->vplayMusicByType(MUSIC_PALACE);
				   }
				   else if (txtNum == 38)
				   {
					   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   else
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 53:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 21:
		case 58:
		case 61:
		{
				   if (txtNum == 61)
				   {
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   }
				   else if (txtNum == 58)
				   {
					   seaBoatBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   dialogBg->runAction(FadeIn::create(0.3));
				   }
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 22:
		case 37:
		{
				   //高帽男或大臣入场 
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   if (txtNum == 37)
				   {
					   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   supportingRolePic->runAction(Sequence::create(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0)), nullptr));
				   break;
		}
		case 23:
		case 45:
		case 46:
		case 57:
		case 60:
		{
				   switch (txtNum)
				   {
				   case 45:
					   dboxRight->runAction(FadeOut::create(0.01));
					   palaceBg->runAction(FadeOut::create(0.01));
					   wineHouseBg->runAction(Spawn::createWithTwoActions(FadeIn::create(0.3), TintTo::create(0.3, 255, 255, 255)));
					   SimpleAudioEngine::getInstance()->stopBackgroundMusic();
					   SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
					   kingPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   HeroPic->runAction(Sequence::create(DelayTime::create(1), MoveBy::create(0.5, Vec2(-OFFSET, 0)), nullptr));
					   break;
				   case 46:
					   majorFigurePic->setPosition(-majorFigurePic->getBoundingBox().size.width, 0);
					   majorFigurePic->setFlippedX(false);
					   break;
				   case 57:
					   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
					   majorFigurePic->runAction(Sequence::create(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0)), nullptr));
					   break;
				   default:
					   break;
				   }
				   if (txtNum != 57)
				   {
					   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 36:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   kingPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   majorFigurePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   majorFigurePic->setFlippedX(true);
				   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(1), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   break;
		}
		default:
			break;
		}
	}

	titleTxt->setString("");
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[22] || m_jsonName[txtNum - 1] == m_jsonName[37])
		{
			//高帽男和大臣
			if (supportingRolePic)
			{
				supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
			}
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[39] && kingPic)
		{
			kingPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[21] && asideBg)
	{
		if (txtNum == 35 || txtNum == 58)
		{
			asideBg->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (asideBg->getOpacity() == 255)
			{
				asideBg->runAction(CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)));
			}
			else
			{
				asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else
	{
		if (asideBg)
		{
			asideBg->runAction(FadeOut::create(0.1));
		}
		if (m_jsonName[txtNum] == m_jsonName[3] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 36 || txtNum == 38 || txtNum == 40 || txtNum == 42 || txtNum == 44)
			{
				if (txtNum == 36)
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
			else
			{
				if (txtNum == 57)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 6 || txtNum == 9 || txtNum == 11)
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum == 14 || txtNum == 53)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			if (txtNum == 45)
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[22] || m_jsonName[txtNum] == m_jsonName[37])
		{
			if (supportingRolePic)
			{
				supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			}
			if (txtNum == 22 || txtNum == 37)
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
		else if (m_jsonName[txtNum] == m_jsonName[39] && kingPic)
		{
			kingPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
}
//触发剧情对话播放顺序
void UIStoryLine::triggerDialogSpeakOrder()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		case 13:
		case 23:
		{
				   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 12:
		case 22:
		case 27:
		{
				   //剧情结束处理

				   auto curScene = Director::getInstance()->getRunningScene();
				   //海上取消暂停
				   if (guideWidget->getCameraMask() == 4)
				   {
					   auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
					   if (maps)
					   {
						   maps->seaEventEnd();
					   }
				   }
				   guideWidget->setCascadeOpacityEnabled(true);
				   guideWidget->setCascadeColorEnabled(true);
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidgetTrigger)), nullptr));
				   return;
		}
		case 5:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   majorFigurePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   majorFigurePic->setFlippedX(true);
				   break;
		}
		case 6:
		{
				  //酒吧女郎入场
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 10:
		{
				   //礼物飞入动画 暂时用AUDIO_EFFECT_USED_PROP_21音效
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_USED_PROP_21);
				   auto giftPic = Sprite::create();
				   giftPic->setTexture(DIALOG_PATH[GIFT_PATH]);
				   auto range = Director::getInstance()->getVisibleSize().width / 2 + giftPic->getContentSize().width / 2;
				   giftPic->setPosition(-giftPic->getContentSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
				   guideWidget->addChild(giftPic);
				   giftPic->setOpacity(0);
				   auto easeAction1 = EaseExponentialOut::create(Spawn::createWithTwoActions(FadeIn::create(0.5), MoveBy::create(0.5, Vec2(range, 0))));
				   auto easeAction2 = EaseExponentialIn::create(Spawn::createWithTwoActions(FadeOut::create(0.5), MoveBy::create(0.5, Vec2(range, 0))));
				   auto actionCome = Spawn::createWithTwoActions(easeAction1, ScaleBy::create(0.5, 2));
				   auto actionGo = Spawn::createWithTwoActions(easeAction2, ScaleBy::create(0.5, 0.5));
				   giftPic->runAction(Sequence::create(actionCome, DelayTime::create(0.7), actionGo, nullptr));
				   break;
		}
		case 11:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 14:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   //好心商人入场
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 17:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 18:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 20:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->setPosition(-AidePic->getBoundingBox().size.width, 0);
				   AidePic->setFlippedX(false);
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}

	titleTxt->setString("");
	titleTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[6] || m_jsonName[txtNum - 1] == m_jsonName[14])
		{
			//有酒吧女郎、好心的商人
			if (majorFigurePic)
			{
				majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
			}
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}
	
	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[6] || m_jsonName[txtNum] == m_jsonName[14])
	{
		if (majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		}
		if (txtNum == 6 || txtNum == 8 || txtNum == 10)
		{
			if (txtNum == 6 || txtNum == 10)
			{
				if (txtNum == 10)
				{
					//礼物退出舞台后显示对话
					dboxRight->runAction(Sequence::create(DelayTime::create(1.5), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
			else
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 14)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[0] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 17)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
	}
}
//小伙伴剧情对话播放顺序
void UIStoryLine::partnerRedBeardDialogSpeakOrder()
{
	if (len >= maxLen)
	{
		//log("txtNum = %d",txtNum);
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		case 25:
		case 29:
		case 32:
		case 34:
		case 41:
		case 44:
		case 48:
		case 52:
		case 75:
		case 86:
		case 119:
		case 132:
		case 134:
		{
				   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   break;
		}
		case 11:
		case 17:
		case 97:
		case 102:
		case 103:
		case 126:
		{
				  //选择"否" 直接关闭对话 //97 玩家选择贿赂 但是银币不足 //102 玩家选择威慑 但是声望不够 //103 玩家选择离开
				  guideWidget->setCascadeOpacityEnabled(true);
				  guideWidget->setCascadeColorEnabled(true);
				  guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidgetOnly)), nullptr));
				  return;
		}
		case 8:
		case 14:
		case 31:
		case 33:
		case 40:
		case 43:
		case 47:
		case 51:
		case 56:
		case 62:
		case 72:
		case 76:
		case 82:
		case 89:
		case 92:
		case 123:
		case 131:
		case 133:
		case 138:
		{
				   //剧情结束处理

				   auto curScene = Director::getInstance()->getRunningScene();
				   //海上取消暂停
				   if (guideWidget->getCameraMask() == 4)
				   {
					   auto maps = dynamic_cast<UISailManage*>(curScene->getChildByName("MAPS"));
					   if (maps)
					   {
						   maps->seaEventEnd();
					   }
				   }
				   if (txtNum == 51 || txtNum == 82 || txtNum == 131)
				   {
					   //5_2 7_2 9_1 进入战斗 TODO
					   if (CompanionTaskManager::GetInstance()->m_PartsInfo && CompanionTaskManager::GetInstance()->m_PartsInfo->has_npc)
					   {
						   guideWidget->removeFromParentAndCleanup(true);
						   reset();
						   int npc_id = CompanionTaskManager::GetInstance()->m_PartsInfo->npc_id;
						   registerCallBack();
						   if (txtNum == 131)
						   {
							   ProtocolThread::GetInstance()->engageInFightForTask(npc_id, UILoadingIndicator::create(this));
						   }
						   else
						   {
							   ProtocolThread::GetInstance()->engageInFightForTask(npc_id, UILoadingIndicator::createWithMask(this, 4));
						   }
						   return;
					   }
				   }
				   else
				   {
					   guideWidget->setCascadeOpacityEnabled(true);
					   guideWidget->setCascadeColorEnabled(true);
					   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UIStoryLine::removeGuideWidgetPartnerRedBeard)), nullptr));
				   }
				   return;
		}
		case 10:
		case 16:
		case 125:
		{
				   //选择"是" 继续剧情
				   txtNum = txtNum + 1;
				   if (txtNum == 17)
				   {
					   OfficerPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					   dockBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   RedBeardFleetVoyagePic->runAction(FadeIn::create(0.3));
					   AidePic->runAction(Sequence::create(DelayTime::create(0.6), MoveBy::create(0.5, Vec2(OFFSET, 0)), nullptr));
					   HeroPic->runAction(Sequence::create(DelayTime::create(0.6), MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100)), nullptr));
				   }
				   else if (txtNum == 126)
				   {
					   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   }
				   break;
		}
		case 21:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   RedBeardFleetVoyagePic->runAction(TintTo::create(0.5, 10, 10, 10));
				   RedBeardWar1Pic->runAction(FadeIn::create(0.3));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   supportingRolePic->runAction(Sequence::create(DelayTime::create(0.6), MoveBy::create(0.5, Vec2(-OFFSET, 0)), nullptr));
				   break;
		}
		case 22:
		case 63:
		{
				   OfficerPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 24:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   OfficerPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 26:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   RedBeardWar1Pic->runAction(TintTo::create(0.5, 10, 10, 10));
				   RedBeardWar2Pic->runAction(FadeIn::create(0.3));
				   //炮击声
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FIRE_CANNON_10);
				   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 27:
		{
				   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.6), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 28:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   OfficerPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 45:
		case 60:
		{
				   pasengerAPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 57:
		case 77:
		case 90:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 59:
		{
				   AidePic->runAction(Sequence::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   pasengerAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 78:
		{
				   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 79:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 85:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   OfficerPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 96:
		case 100:
		{
				   //96 贿赂成功//100 威慑成功
				   txtNum = 103;
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
				   supportingRolePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   dialogBg->runAction(FadeIn::create(0.3));
				   majorFigurePic->runAction(Sequence::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   break;
		}
		case 95:
		{
				   //判断玩家银币是否足够80万
				   if (SINGLE_HERO->m_iCoin < 800000)
				   {
					   txtNum = 96;
				   }
				   else
				   {
					   registerCallBack();
					   ProtocolThread::GetInstance()->deductCoins(800000, UILoadingIndicator::create(this));
				   }
				   break;
		}
		case 98:
		{
				   //判断玩家声望是否15级以上
				   if (SINGLE_HERO->m_iPrestigeLv < 40)
				   {
					   txtNum = 100;
				   }
				   break;
		}
		case 117:
		{
					//旁白
					majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					break;
		}
		case 118:
		{
					majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					break;
		}
		default:
			break;
		}
	}

	titleTxt->setString("");
	titleTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[23])
		{
			//红胡子巴巴罗萨
			if (majorFigurePic)
			{
				majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
			}
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[0] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[8] && OfficerPic)
		{
			OfficerPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if ((m_jsonName[txtNum - 1] == m_jsonName[21] || m_jsonName[txtNum - 1] == m_jsonName[123]) && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[27] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[43] && pasengerAPic)
		{
			pasengerAPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
	}

	if (dboxLeft)
	{
		dboxLeft->runAction(FadeOut::create(0.01));
	}
	if (dboxRight)
	{
		dboxRight->runAction(FadeOut::create(0.01));
	}
	//检测当前说话的人物 颜色变亮 并说话
	if (m_jsonName[txtNum] == m_jsonName[24] && asideBg)
	{
		if (asideBg->getOpacity() == 255)
		{
			asideBg->runAction(CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
		}
	}
	else
	{
			if (asideBg)
			{
				asideBg->runAction(FadeOut::create(0.1));
			}
			if (m_jsonName[txtNum] == m_jsonName[27] && majorFigurePic)
			{
				//红胡子巴巴罗萨
				majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
				if (txtNum == 27)
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
			}
			else if (m_jsonName[txtNum] == m_jsonName[0] && AidePic)
			{
				AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
				if (txtNum == 10000)
				{
					dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					if (txtNum == 17)
					{
						dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
					else
					{
						dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
				}
			}
			else if (m_jsonName[txtNum] == m_jsonName[1] && HeroPic)
			{
				HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
				if (txtNum == 9 || txtNum == 15 || txtNum == 93 || txtNum == 124)
				{
					UIStoryLine::showBranch();
				}
				else
				{
					if (txtNum == 103)
					{
						dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
					else
					{
						dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
				}
			}
			else if (m_jsonName[txtNum] == m_jsonName[8] && OfficerPic)
			{
				OfficerPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
			else if ((m_jsonName[txtNum] == m_jsonName[21] || m_jsonName[txtNum] == m_jsonName[123]) && supportingRolePic)
			{
				supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
				if (m_jsonName[txtNum] == m_jsonName[123])
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					if (txtNum == 21)
					{
						dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
					else
					{
						dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
					}
				}
			}
			else if (m_jsonName[txtNum] == m_jsonName[43] && pasengerAPic)
			{
				pasengerAPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showDialog, this)), nullptr));
			}
	}
}
//主线对话结束时删除层并且主线任务提交
void UIStoryLine::removeGuideWidget()
{
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	auto mapLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
	auto pupLayer = (UITavern*)(currentScene->getChildByTag(SCENE_TAG::PUPL_TAG + 100));
	auto palaceLayer = (UIPalace*)(currentScene->getChildByTag(SCENE_TAG::PALACE_TAG + 100));
	auto dockLayer = (UIShipyard*)(currentScene->getChildByTag(SCENE_TAG::DOCK_TAG + 100));
	auto wharfLayer = (UIPort*)(currentScene->getChildByTag(SCENE_TAG::WHARF_TAG + 100));
	if (mainlayer)
	{
		mainlayer->notifyCompleted(MAIN_STORY);
	}
	else if (mapLayer)
	{
		mapLayer->notifyCompleted(MAIN_STORY);
	}
	else if (pupLayer)
	{
		pupLayer->notifyCompleted(MAIN_STORY);
	}
	else if (palaceLayer)
	{
		palaceLayer->notifyCompleted(MAIN_STORY);
	}
	else if (dockLayer)
	{
		dockLayer->notifyCompleted(MAIN_STORY);
	}
	else if (wharfLayer)
	{
		wharfLayer->notifyCompleted(MAIN_STORY);
	}
	guideWidget->removeFromParentAndCleanup(true);
	reset();
	allTaskInfo(true, MAIN_STORY, 0.5f, true);
}
//对话结束时只是删除层
void UIStoryLine::removeGuideWidgetOnly()
{
	guideWidget->removeFromParentAndCleanup(true);
	reset();
	switch (dialogType)
	{
	case DIALOG_TYPE_MAIN_TASK:
		allTaskInfo(true, MAIN_STORY, 0.5f);
		break;
	case DIALOG_TYPE_SMALL_STORY:
		allTaskInfo(true, SMALL_STORY, 0.5f);
		break;
	case DIALOG_TYPE_PARTNER_RED_BEARD:
		allTaskInfo(true, COMPANION_STORY, 0.5f);
		break;
	default:
		break;
	}
}
/*
 * 小剧情结束退出 小剧情结束时删除层并且小剧情提交
 */
void UIStoryLine::removeGuideWidgetTrigger()
{
	//小剧情的callback 需要在判断是否有奖励处添加
	guideWidget->removeFromParentAndCleanup(true);
	reset();
	allTaskInfo(true, SMALL_STORY, 0.5f,true);
}
/*
* 小剧情结束退出 小剧情结束时删除层并且小剧情提交
*/
void UIStoryLine::removeGuideWidgetPartnerRedBeard()
{
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	auto mapLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
	auto pupLayer = (UITavern*)(currentScene->getChildByTag(SCENE_TAG::PUPL_TAG + 100));
	auto palaceLayer = (UIPalace*)(currentScene->getChildByTag(SCENE_TAG::PALACE_TAG + 100));
	auto dockLayer = (UIShipyard*)(currentScene->getChildByTag(SCENE_TAG::DOCK_TAG + 100));
	auto wharfLayer = (UIPort*)(currentScene->getChildByTag(SCENE_TAG::WHARF_TAG + 100));
	if (mainlayer && txtNum != 8)
	{
		mainlayer->notifyCompleted(COMPANION_STORY);
	}
	else if (mapLayer)
	{
		mapLayer->notifyCompleted(COMPANION_STORY);
	}
	else if (pupLayer)
	{
		pupLayer->notifyCompleted(COMPANION_STORY);
	}
	else if (palaceLayer)
	{
		palaceLayer->notifyCompleted(COMPANION_STORY);
	}
	else if (dockLayer)
	{
		dockLayer->notifyCompleted(COMPANION_STORY);
	}
	else if (wharfLayer)
	{
		wharfLayer->notifyCompleted(COMPANION_STORY);
	}
	guideWidget->removeFromParentAndCleanup(true);
	
	if (txtNum == 43 || txtNum == 133 || txtNum == 138)
	{
		reset();
		CompanionTaskManager::GetInstance()->completeCompanionTask();
	}
	else
	{
		reset();
		allTaskInfo(true, COMPANION_STORY, 0.5f, true);
	}
}
//显示对话支线
void UIStoryLine::showBranch()
{
	if (zhixianBgPic && branchButton1 && branchButton2)
	{
		zhixianBgPic->setVisible(true);
		branchButton1->setTouchEnabled(true);
		branchButton2->setTouchEnabled(true);
		if (branchButton3)
		{
			branchButton3->setTouchEnabled(true);
		}
	}
}
//显示对话(逐字显示播放对话内容)
void UIStoryLine::showDialog()
{
	txtTxt->setString("");
	titleTxt->setString("");
	std::string name;
	std::string dialog;
	if (dialogType == DIALOG_TYPE_MAIN_TASK)
	{
		if (m_eStep != SALLING_GUID && m_eStep != WAR_GUID && m_eStep != WAR_FAIL_GUID)
		{
			name = m_jsonName[txtNum];
			dialog = m_jsonDialog[txtNum];
		}
	}
	else
	{
		name = m_jsonName[txtNum];
		dialog = m_jsonDialog[txtNum];
	}
	
	if (dboxLeft)
	{
		if (dboxLeft->getOpacity() == 255)
		{
			txtTxt->setWidth(540);
			maskPic->setTextureRect(Rect(0, 0, txtTxt->getWidth(), m_nOneLineHeight * 3));
			titleTxt->setPosition(dboxLeft->getPositionX() + 88.0, dboxLeft->getPositionY() + dboxLeft->getBoundingBox().size.height - 76.0);
			txtTxt->setPosition(dboxLeft->getPositionX() + 88.0, dboxLeft->getPositionY() + dboxLeft->getBoundingBox().size.height - 86.0);
			titleTxt->setString(name);
		}
	}
	if (dboxRight)
	{
		if (dboxRight->getOpacity() == 255)
		{
			txtTxt->setWidth(540);
			maskPic->setTextureRect(Rect(0, 0, txtTxt->getWidth(), m_nOneLineHeight * 3));
			titleTxt->setPosition(dboxRight->getPositionX() + 88.0, dboxRight->getPositionY() + dboxRight->getBoundingBox().size.height - 76.0);
			txtTxt->setPosition(dboxRight->getPositionX() + 88.0, dboxRight->getPositionY() + dboxRight->getBoundingBox().size.height - 86.0);
			titleTxt->setString(name);
		}
	}
	if (asideBg)
	{
		if (asideBg->getOpacity() == 255)
		{
			txtTxt->setWidth(865);
			maskPic->setTextureRect(Rect(0, 0, txtTxt->getWidth(), m_nOneLineHeight * 3));
			txtTxt->setPosition(asideBg->getPositionX() + 225.0 / 1280 * Director::getInstance()->getVisibleSize().width, asideBg->getPositionY() + asideBg->getBoundingBox().size.height - (157 / 286.0)*asideBg->getBoundingBox().size.height);
		}
	}
	mystr = dialog;
	if (dialogType == DIALOG_TYPE_MAIN_TASK)
	{
		if (m_eStep == SALLING_GUID || m_eStep == WAR_GUID || m_eStep == WAR_FAIL_GUID)
		{
			if (m_dialogVector[txtNum]->personIdx > 0)
			{
				titleTxt->setString(m_dialogNames[m_dialogVector[txtNum]->personIdx - 1]);
			}
			mystr = m_dialogVector[txtNum]->dialog.c_str();
		}
	}
	maskPic->setPosition(txtTxt->getPositionX(), txtTxt->getPositionY() + 2);
	lenNum = 0;
	UIStoryLine::openText();
	layerFarmClickBoo = false;
}
//战斗引导	
void UIStoryLine::createWarGuideHelp()
{
	guideWidget->removeFromParentAndCleanup(true);
	reset();
	auto i_warHelp = ImageView::create(GUIDE_WAR[SINGLE_SHOP->L_TYPE]);
	i_warHelp->setScale(Director::getInstance()->getVisibleSize().height / i_warHelp->getContentSize().height, Director::getInstance()->getVisibleSize().height / i_warHelp->getContentSize().height);
	i_warHelp->setAnchorPoint(Vec2(0.5, 0.5));
	i_warHelp->setPosition(Vec2(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2));
	this->addChild(i_warHelp);
	i_warHelp->setCameraMask(4);
	i_warHelp->setTouchEnabled(true);
	i_warHelp->addTouchEventListener(CC_CALLBACK_2(UIStoryLine::warStartEvent,this));
	i_warHelp->runAction(FadeIn::create(1));
}
//战斗引导点击事件
void UIStoryLine::warStartEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto target = dynamic_cast<Widget*>(pSender);
		if (target->getOpacity() == 255)
		{
			dynamic_cast<TVBattleManager*>(m_pParent)->startBattle(0);
			this->removeFromParentAndCleanup(true);
		}
	}
}
//海上引导
void UIStoryLine::createSaillingHelp()
{
	auto i_SaillingHelp = ImageView::create(GUIDE_SAILING[SINGLE_SHOP->L_TYPE]);
	i_SaillingHelp->setScale(Director::getInstance()->getVisibleSize().height / i_SaillingHelp->getContentSize().height, Director::getInstance()->getVisibleSize().height / i_SaillingHelp->getContentSize().height);
	i_SaillingHelp->setAnchorPoint(Vec2(0.5, 0.5));
	i_SaillingHelp->setPosition(Vec2(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2));
	this->addChild(i_SaillingHelp);
	i_SaillingHelp->setCameraMask(4);
	i_SaillingHelp->setTouchEnabled(true);
	i_SaillingHelp->addTouchEventListener(CC_CALLBACK_2(UIStoryLine::startSailling,this));
}
//海上引导点击事件
void UIStoryLine::startSailling(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto handPicSpr = Sprite::create();
		handPicSpr->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
		dynamic_cast<UIGuideSailManage*>(m_pParent)->setsaillingHandPic(handPicSpr);
		dynamic_cast<UIGuideSailManage*>(m_pParent)->getsaillingHandPic()->setRotation(-180.0f);
		dynamic_cast<UIGuideSailManage*>(m_pParent)->getEventLayer()->addChild(dynamic_cast<UIGuideSailManage*>(m_pParent)->getsaillingHandPic(), 12, 200);
		dynamic_cast<UIGuideSailManage*>(m_pParent)->getsaillingHandPic()->setPosition(dynamic_cast<UIGuideSailManage*>(m_pParent)->guideHandPosFirst());
		dynamic_cast<UIGuideSailManage*>(m_pParent)->getsaillingHandPic()->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
		this->removeFromParentAndCleanup(true);
	}
}
//西班牙对话中 酒馆背景改变 (为动作进行先后而调用)
void UIStoryLine::changeWineHouseBg()
{
	wineHouseBg->runAction(Sequence::create(TintTo::create(0.5, 10, 10, 10), DelayTime::create(0.2), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showCateWineHousePic, this)), nullptr));
}
//西班牙对话中 显示插画酒馆中的卡特 (为动作进行先后而调用)
void UIStoryLine::showCateWineHousePic()
{
	cateWineHousePic->runAction(FadeIn::create(0.3));
}
//西班牙对话中 天黑再转到天亮 (为动作进行先后而调用)
void UIStoryLine::dayAndNight()
{
	cateWineHousePic->runAction(Sequence::create(TintTo::create(0.5, 10, 10, 10), DelayTime::create(0.2), CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeToWineHouseBg, this)), nullptr));
}
//西班牙对话中 改变酒馆中的卡特插画 换成酒馆背景 (为动作进行先后而调用)
void UIStoryLine::changeToWineHouseBg()
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OVER_NIGHT_20);
	cateWineHousePic->runAction(FadeOut::create(0.01));
	wineHouseBg->runAction(Spawn::createWithTwoActions(FadeIn::create(0.3), TintTo::create(0.3, 255, 255, 255)));
}
//西班牙对话中 改变酒馆背景 换成卧室的卡特插画 (为动作进行先后而调用)
void UIStoryLine::changeCateBedroomPic()
{
	wineHouseBg->runAction(Sequence::create(TintTo::create(0.5, 10, 10, 10), DelayTime::create(0.2), CallFunc::create(CC_CALLBACK_0(UIStoryLine::showCateBedroomPic, this)), nullptr));
}
//西班牙对话中 显示插画卧室的卡特 (为动作进行先后而调用)
void UIStoryLine::showCateBedroomPic()
{
	cateBedroomPic->runAction(FadeIn::create(0.3));
}
//改变海上背景(为动作进行先后而调用)
void UIStoryLine::changeSeaBoatBg()
{
	seaBoatBg->runAction(Sequence::create(TintTo::create(0.5, 10, 10, 10), DelayTime::create(0.2), CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeToBoatteamBg, this)), nullptr));
}
//改变战争背景(为动作进行先后而调用)
void UIStoryLine::changeToBoatteamBg()
{
	boatteamPic->runAction(FadeIn::create(0.3));
}
//改变当前对话背景(为动作进行先后而调用)
void UIStoryLine::changeDialogBg()
{
	dialogBg->runAction(Sequence::create(TintTo::create(0.5, 10, 10, 10), DelayTime::create(0.2), CallFunc::create(CC_CALLBACK_0(UIStoryLine::changeToJudgeBg, this)), nullptr));
}
//改变当前对话背景 换成插画审判(为动作进行先后而调用)
void UIStoryLine::changeToJudgeBg()
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_JUDGE_CATE_36);
	judgePic->runAction(FadeIn::create(0.3));
}
//变量重置
void UIStoryLine::reset()
{
	guideWidget = nullptr;
	titleTxt = nullptr;
	txtTxt = nullptr;
	branchText1 = nullptr;
	branchText2 = nullptr;

	wineHouseBg = nullptr;
	seaBoatBg = nullptr;
	dockBg = nullptr;
	palaceBg = nullptr;
	svalbardBg = nullptr;
	judgePic = nullptr;
	boatteamPic = nullptr;
	cateWineHousePic = nullptr;
	cateBedroomPic = nullptr;
	CeutaBattlePic = nullptr;
	MadeiraPic = nullptr;
	CapeVerdePic = nullptr;
	RedBeardFleetVoyagePic = nullptr;
	RedBeardWar1Pic = nullptr;
	RedBeardWar2Pic = nullptr;
	dialogBg = nullptr;
	AidePic = nullptr;
	HeroPic = nullptr;
	majorFigurePic = nullptr;
	pasengerAPic = nullptr;
	pasengerBPic = nullptr;
	kingPic = nullptr;
	supportingRolePic = nullptr;
	OfficerPic = nullptr;
	asideBg = nullptr;
	dboxLeft = nullptr;
	dboxRight = nullptr;
	anchr = nullptr;
	maskPic = nullptr;
	handPic = nullptr;
	zhixianBgPic = nullptr;

	branchButton1 = nullptr;
	branchButton2 = nullptr;
	branchButton3 = nullptr;
	pStencil = nullptr;

	OFFSET = 0;
	txtNum = 0;
	branchTxtNum = 0;
	lenNum = 0;
	len = 0;
	maxLen = 0;
	moreNum = 0;
	plusNum = 0;
	addjust = 0;
	FontSize = 23;
	IllustrationNum = 0;
	dialogType = 0;

	textTime = -1;

	lenBoo = false;
	layerFarmClickBoo = true;
	directShowBoo = false;

	m_dialogVector.clear();
	m_dialogNames.clear();
	JOANNA = "";
	JOHN = "";
	mystr = "";

	m_eStep = 0;
	m_eTriggerStep = 0;
	m_ePartnerStep = 0;
	m_nOneLineHeight = 0;
	m_jsonName.clear();
	m_jsonDialog.clear();
	log("reset ****************------------------");
}

void UIStoryLine::stopAllAction()
{
	if (guideWidget)
	{
		guideWidget->removeFromParentAndCleanup(true);
	}
	reset();
}
