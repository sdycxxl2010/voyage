#include "UIGuideSailManage.h"
#include "UIGuideShipyard.h"
#include "UIGuideMain.h"
#include "UIGuidePort.h"
#include "UIGuideCenterCharactorSkill.h"
#include "UINoviceStoryLine.h"
#include "TVBattleManager.h"

#include "UIPalace.h"
#include "Utils.h"
#include "TVSceneLoader.h"
#include "UISailManage.h"
#include "UIGuideTarven.h"

/*
* 剧情类 由onGuideTaskMessage()开始新手引导的剧情
* 然后传入stepGuideDialog()剧情对话段落索引
* 根据对话段落索引 创建对话层、写入剧情json文件 
* 之后创建剧情所需人物、背景等等 （调用createGuideDialogAllNeed_1()类似来创建）
* 对话层点击相应事件为noviceClickEvent()（同时判断点击翻页）
* 点击后 过滤点击步骤dialogSentenceStepByStep(int Num)
* 开始顺序播放对话 dialogSpeakOrder1_1()（对话更替为下一句和剧情是否结束和删除也在此类似方法中判断）
*/
UINoviceStoryLine::UINoviceStoryLine()
{
	guideWidget = nullptr;
	titleTxt = nullptr;
	txtTxt = nullptr;
	branchText1 = nullptr;
	branchText2 = nullptr;
	nameTxt = nullptr;

	wineHouseBg = nullptr;
	dockBg = nullptr;
	shipyardBg = nullptr;
	fightPic = nullptr;
	dialogBg = nullptr;
	AidePic = nullptr;
	HeroPic = nullptr;
	majorFigurePic = nullptr;
	ruffianAPic = nullptr;
	ruffianBPic = nullptr;
	shipyardBossPic = nullptr;
	supportingRolePic = nullptr;
	captainPic = nullptr;
	asideBg = nullptr;
	dboxLeft = nullptr;
	dboxRight = nullptr;
	anchr = nullptr;
	maskPic = nullptr;
	handPic = nullptr;
	zhixianBgPic = nullptr;

	branchButton1 = nullptr;
	branchButton2 = nullptr;
	btnSkip = nullptr;
	btn_nameOk = nullptr;
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

	skipNum = 0;

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
	m_nOneLineHeight = 0;
	m_pParent = nullptr;
	m_jsonName.clear();
	m_jsonDialog.clear();
	log("start ****************------------------");
}

UINoviceStoryLine::~UINoviceStoryLine()
{
	guideWidget = nullptr;
	titleTxt = nullptr;
	txtTxt = nullptr;
	branchText1 = nullptr;
	branchText2 = nullptr;
	nameTxt = nullptr;

	wineHouseBg = nullptr;
	dockBg = nullptr;
	shipyardBg = nullptr;
	fightPic = nullptr;
	dialogBg = nullptr;
	AidePic = nullptr;
	HeroPic = nullptr;
	majorFigurePic = nullptr;
	ruffianAPic = nullptr;
	ruffianBPic = nullptr;
	shipyardBossPic = nullptr;
	supportingRolePic = nullptr;
	captainPic = nullptr;
	asideBg = nullptr;
	dboxLeft = nullptr;
	dboxRight = nullptr;
	anchr = nullptr;
	maskPic = nullptr;
	handPic = nullptr;
	zhixianBgPic = nullptr;

	branchButton1 = nullptr;
	branchButton2 = nullptr;
	btnSkip = nullptr;
	btn_nameOk = nullptr;
	pStencil = nullptr;

	m_pParent = nullptr;

	m_dialogVector.clear();
	m_dialogNames.clear();
	m_jsonName.clear();
	m_jsonDialog.clear();
	unregisterCallBack();
	log("release ****************------------------");
}

void UINoviceStoryLine::onEnter()
{
	UIBasicLayer::onEnter();
}

void UINoviceStoryLine::onExit()
{
	UIBasicLayer::onExit();
}

UINoviceStoryLine* UINoviceStoryLine::m_pInstance=0;

UINoviceStoryLine* UINoviceStoryLine::GetInstance(){
	if(!m_pInstance){
		m_pInstance = new UINoviceStoryLine;
		m_pInstance->autorelease();
		m_pInstance->retain();
	}
	return m_pInstance;
}
//剧情播放最先调用的方法 新手引导剧情
bool UINoviceStoryLine::onGuideTaskMessage(int dialogId,int flags)
{
#if 1
	if (dialogId > 0)
	{
		stepGuideDialog(dialogId);
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
UINoviceStoryLine* UINoviceStoryLine::createNovice(Layer * parent)
{
	UINoviceStoryLine* pup = new UINoviceStoryLine;
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

bool UINoviceStoryLine::init()
{
	bool pRet = false;
	do 
	{
		pRet = true;
	} while (0);
	return pRet;
}
void UINoviceStoryLine::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	log("menuCall_func");
	Widget* button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
}

void UINoviceStoryLine::onServerEvent(struct ProtobufCMessage* message, int msgType)
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
}

/*
*主线剧情对话段落根据索引选择
*首先创建对话层
*然后判断索引是否为海上引导、战斗引导或战败引导的剧情 否则根据国家来进行主线剧情
*最后根据传进来的剧情索引创建需要的人物 对话文本和背景
*/
void UINoviceStoryLine::stepGuideDialog(int eStep)
{
	log("NoviceDialogLayer::stepDialog step:%d,--- %d", eStep, __LINE__);
	m_eStep = eStep;
	//首先创建对话层
	createLayer();
	//判断m_eStep索引是否为海上引导、战斗引导或战败引导的剧情
	//根据传进来的剧情索引创建需要的人物 对话文本和背景
	switch (SINGLE_HERO->m_iNation)
	{
	case 1:
		createGuideDialogAllNeed_1();
		break;
	case 2:
		createGuideDialogAllNeed_2();
		break;
	case 3:
		createGuideDialogAllNeed_3();
		break;
	case 4:
		createGuideDialogAllNeed_4();
		break;
	case 5:
		createGuideDialogAllNeed_5();
		break;
	default:
		break;
	}
}
//根据剧情索引创建葡萄牙主线剧情需要的人物 对话文本和背景 并开始对话
void UINoviceStoryLine::createGuideDialogAllNeed_1()
{
	switch (m_eStep)
	{
	case START_DIALOG:
		txtNum = 0;
		createDockBg();
		dockBg->setOpacity(255);
		createWineHouseBg();
		createAide();
		createHero();
		creatMajorFigure(DIALOG_PATH[HENRY_PATH]);
		createCaptain();
		createDboxLeft();
		createDboxRight();
		createAsideBg();
		createText();
		createAnchr();
		createButtonSkip();
		SINGLE_AUDIO->vplayBGMusic(audio_effect[AUDIO_EFFECT_MEW_06]);
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_ONE_DIALOG:
		txtNum = 24;
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createShipyardBoss();
		createDboxLeft();
		createDboxRight();
		createDialogBranchButton(txtNum + 9);
		createText();
		createAnchr();
		createButtonSkip();
		createNameBoat();
		SINGLE_AUDIO->vplayBGMusic(audio_effect[AUDIO_EFFECT_MEW_06]);
		m_eName = SHIPYARDBOSS;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_TWO_DIALOG:
		txtNum = 37;
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createDboxLeft();
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case TARVEN_ONE_DIALOG:
	case TARVEN_TWO_DIALOG:
	case TARVEN_THREE_DIALOG:
	case MARKET_BUY_DIALOG:
	case DOCK_ONE_DIALOG:
	case DOCK_TWO_DIALOG:
	case SAILING_GUIDE_DIALOG:
	case WAR_GUIDE_DIALOG:
	case WAR_FAIL_GUIDE_DIALOG:
	case PALACE_DIALOG:
	case MARKET_SELL_DIALOG:
		switch (m_eStep)
		{
		case TARVEN_ONE_DIALOG:
			txtNum = 38;
			break;
		case TARVEN_TWO_DIALOG:
			txtNum = 39;
			break;
		case TARVEN_THREE_DIALOG:
			txtNum = 40;
			createWineHouseBg();
			wineHouseBg->setOpacity(255);
			break;
		case MARKET_BUY_DIALOG:
			txtNum = 41;
			break;
		case DOCK_ONE_DIALOG:
			txtNum = 42;
			break;
		case DOCK_TWO_DIALOG:
			txtNum = 43;
			break;
		case SAILING_GUIDE_DIALOG:
			txtNum = 44;
			break;
		case WAR_GUIDE_DIALOG:
			txtNum = 49;
			break;
		case WAR_FAIL_GUIDE_DIALOG:
			txtNum = 55;
			break;
		case PALACE_DIALOG:
			txtNum = 56;
			break;
		case MARKET_SELL_DIALOG:
			txtNum = 57;
			break;
		default:
			break;
		}
		createAide();
		createDboxLeft();
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG)
		{
			createAsideBg();
			createButtonSkip();
		}
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG || m_eStep == WAR_FAIL_GUIDE_DIALOG)
		{
			guideWidget->setCameraMask(4);
		}
		break;
	case CENTER_ONE_DIALOG:
	case CENTER_TWO_DIALOG:
	case CENTER_THREE_DIALOG:
		switch (m_eStep)
		{
		case CENTER_ONE_DIALOG:
			txtNum = 58;
			break;
		case CENTER_TWO_DIALOG:
			txtNum = 59;
			break;
		case CENTER_THREE_DIALOG:
			txtNum = 62;
			break;
		default:
			break;
		}
		createAsideBg();
		createAsideText();
		createAnchr();
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	default:
		break;
	}
}
void UINoviceStoryLine::createGuideDialogAllNeed_2()
{
	switch (m_eStep)
	{
	case START_DIALOG:
		txtNum = 0;
		createWineHouseBg();
		wineHouseBg->setOpacity(255);
		createAide();
		createHero();
		creatMajorFigure(DIALOG_PATH[CATALINA_PATH]);
		if (SINGLE_HERO->m_iGender == 1)
		{
			createSupportingRole(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_BAR_GIRL));
		}
		creatRuffian();
		createDboxLeft();
		createDboxRight();
		createAsideBg();
		createText();
		createAnchr();
		createButtonSkip();
		createFightBg();
		SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_ONE_DIALOG:
		txtNum = 30;
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createShipyardBoss();
		createDboxLeft();
		createDboxRight();
		createDialogBranchButton(txtNum + 9);
		createText();
		createAnchr();
		createButtonSkip();
		createNameBoat();
		SINGLE_AUDIO->vplayBGMusic(audio_effect[AUDIO_EFFECT_MEW_06]);
		m_eName = SHIPYARDBOSS;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_TWO_DIALOG:
		txtNum = 43;
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createDboxLeft();
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case TARVEN_ONE_DIALOG:
	case TARVEN_TWO_DIALOG:
	case TARVEN_THREE_DIALOG:
	case MARKET_BUY_DIALOG:
	case DOCK_ONE_DIALOG:
	case DOCK_TWO_DIALOG:
	case SAILING_GUIDE_DIALOG:
	case WAR_GUIDE_DIALOG:
	case WAR_FAIL_GUIDE_DIALOG:
	case PALACE_DIALOG:
	case MARKET_SELL_DIALOG:
		switch (m_eStep)
		{
		case TARVEN_ONE_DIALOG:
			txtNum = 44;
			break;
		case TARVEN_TWO_DIALOG:
			txtNum = 45;
			break;
		case TARVEN_THREE_DIALOG:
			txtNum = 46;
			createWineHouseBg();
			wineHouseBg->setOpacity(255);
			break;
		case MARKET_BUY_DIALOG:
			txtNum = 47;
			break;
		case DOCK_ONE_DIALOG:
			txtNum = 48;
			break;
		case DOCK_TWO_DIALOG:
			txtNum = 49;
			break;
		case SAILING_GUIDE_DIALOG:
			txtNum = 50;
			break;
		case WAR_GUIDE_DIALOG:
			txtNum = 55;
			break;
		case WAR_FAIL_GUIDE_DIALOG:
			txtNum = 61;
			break;
		case PALACE_DIALOG:
			txtNum = 62;
			break;
		case MARKET_SELL_DIALOG:
			txtNum = 63;
			break;
		default:
			break;
		}
		createAide();
		createDboxLeft();
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG)
		{
			createAsideBg();
			createButtonSkip();
		}
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG || m_eStep == WAR_FAIL_GUIDE_DIALOG)
		{
			guideWidget->setCameraMask(4);
		}
		break;
	case CENTER_ONE_DIALOG:
	case CENTER_TWO_DIALOG:
	case CENTER_THREE_DIALOG:
		switch (m_eStep)
		{
		case CENTER_ONE_DIALOG:
			txtNum = 64;
			break;
		case CENTER_TWO_DIALOG:
			txtNum = 65;
			break;
		case CENTER_THREE_DIALOG:
			txtNum = 68;
			break;
		default:
			break;
		}
		createAsideBg();
		createAsideText();
		createAnchr();
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	default:
		break;
	}
}
void UINoviceStoryLine::createGuideDialogAllNeed_3()
{
	auto picNum = 0;
	switch (m_eStep)
	{
	case START_DIALOG:
		txtNum = 0;
		if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
		{
			picNum = SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_village;
		}
		else
		{
			picNum = SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_id;
		}
		createDialogBg(StringUtils::format("cities_resources/background/city_%d.jpg", picNum));
		dialogBg->setOpacity(255);
		createWineHouseBg();
		createAide();
		createHero();
		creatMajorFigure(DIALOG_PATH[DEREK_PATH]);
		createSupportingRole(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_BAR_GIRL));
		createDboxLeft();
		createDboxRight();
		createAsideBg();
		createText();
		createAnchr();
		createButtonSkip();
		SINGLE_AUDIO->vplayMusicByType(MUSIC_CITY);
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_ONE_DIALOG:
		txtNum = 23;
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createShipyardBoss();
		createDboxLeft();
		createDboxRight();
		createDialogBranchButton(txtNum + 9);
		createText();
		createAnchr();
		createButtonSkip();
		createNameBoat();
		SINGLE_AUDIO->vplayBGMusic(audio_effect[AUDIO_EFFECT_MEW_06]);
		m_eName = SHIPYARDBOSS;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_TWO_DIALOG:
		txtNum = 36;
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createDboxLeft();
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case TARVEN_ONE_DIALOG:
	case TARVEN_TWO_DIALOG:
	case TARVEN_THREE_DIALOG:
	case MARKET_BUY_DIALOG:
	case DOCK_ONE_DIALOG:
	case DOCK_TWO_DIALOG:
	case SAILING_GUIDE_DIALOG:
	case WAR_GUIDE_DIALOG:
	case WAR_FAIL_GUIDE_DIALOG:
	case PALACE_DIALOG:
	case MARKET_SELL_DIALOG:
		switch (m_eStep)
		{
		case TARVEN_ONE_DIALOG:
			txtNum = 37;
			break;
		case TARVEN_TWO_DIALOG:
			txtNum = 38;
			break;
		case TARVEN_THREE_DIALOG:
			txtNum = 39;
			createWineHouseBg();
			wineHouseBg->setOpacity(255);
			break;
		case MARKET_BUY_DIALOG:
			txtNum = 40;
			break;
		case DOCK_ONE_DIALOG:
			txtNum = 41;
			break;
		case DOCK_TWO_DIALOG:
			txtNum = 42;
			break;
		case SAILING_GUIDE_DIALOG:
			txtNum = 43;
			break;
		case WAR_GUIDE_DIALOG:
			txtNum = 48;
			break;
		case WAR_FAIL_GUIDE_DIALOG:
			txtNum = 54;
			break;
		case PALACE_DIALOG:
			txtNum = 55;
			break;
		case MARKET_SELL_DIALOG:
			txtNum = 56;
			break;
		default:
			break;
		}
		createAide();
		createDboxLeft();
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG)
		{
			createAsideBg();
			createButtonSkip();
		}
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG || m_eStep == WAR_FAIL_GUIDE_DIALOG)
		{
			guideWidget->setCameraMask(4);
		}
		break;
	case CENTER_ONE_DIALOG:
	case CENTER_TWO_DIALOG:
	case CENTER_THREE_DIALOG:
		switch (m_eStep)
		{
		case CENTER_ONE_DIALOG:
			txtNum = 57;
			break;
		case CENTER_TWO_DIALOG:
			txtNum = 58;
			break;
		case CENTER_THREE_DIALOG:
			txtNum = 61;
			break;
		default:
			break;
		}
		createAsideBg();
		createAsideText();
		createAnchr();
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	default:
		break;
	}
}
void UINoviceStoryLine::createGuideDialogAllNeed_4()
{
	switch (m_eStep)
	{
	case START_DIALOG:
		txtNum = 0;
		createWineHouseBg();
		wineHouseBg->setOpacity(255);
		createAide();
		createHero();
		creatMajorFigure(DIALOG_PATH[BARENTS_PATH]);
		createDboxLeft();
		createDboxRight();
		createAsideBg();
		createText();
		createAnchr();
		createButtonSkip();
		SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_ONE_DIALOG:
		txtNum = 18;
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createShipyardBoss();
		createDboxLeft();
		createDboxRight();
		createDialogBranchButton(txtNum + 9);
		createText();
		createAnchr();
		createButtonSkip();
		createNameBoat();
		SINGLE_AUDIO->vplayBGMusic(audio_effect[AUDIO_EFFECT_MEW_06]);
		m_eName = SHIPYARDBOSS;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_TWO_DIALOG:
		txtNum = 31;
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createDboxLeft();
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case TARVEN_ONE_DIALOG:
	case TARVEN_TWO_DIALOG:
	case TARVEN_THREE_DIALOG:
	case MARKET_BUY_DIALOG:
	case DOCK_ONE_DIALOG:
	case DOCK_TWO_DIALOG:
	case SAILING_GUIDE_DIALOG:
	case WAR_GUIDE_DIALOG:
	case WAR_FAIL_GUIDE_DIALOG:
	case PALACE_DIALOG:
	case MARKET_SELL_DIALOG:
		switch (m_eStep)
		{
		case TARVEN_ONE_DIALOG:
			txtNum = 32;
			break;
		case TARVEN_TWO_DIALOG:
			txtNum = 33;
			break;
		case TARVEN_THREE_DIALOG:
			txtNum = 34;
			createWineHouseBg();
			wineHouseBg->setOpacity(255);
			break;
		case MARKET_BUY_DIALOG:
			txtNum = 35;
			break;
		case DOCK_ONE_DIALOG:
			txtNum = 36;
			break;
		case DOCK_TWO_DIALOG:
			txtNum = 37;
			break;
		case SAILING_GUIDE_DIALOG:
			txtNum = 38;
			break;
		case WAR_GUIDE_DIALOG:
			txtNum = 43;
			break;
		case WAR_FAIL_GUIDE_DIALOG:
			txtNum = 49;
			break;
		case PALACE_DIALOG:
			txtNum = 50;
			break;
		case MARKET_SELL_DIALOG:
			txtNum = 51;
			break;
		default:
			break;
		}
		createAide();
		createDboxLeft();
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG)
		{
			createAsideBg();
			createButtonSkip();
		}
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG || m_eStep == WAR_FAIL_GUIDE_DIALOG)
		{
			guideWidget->setCameraMask(4);
		}
		break;
	case CENTER_ONE_DIALOG:
	case CENTER_TWO_DIALOG:
	case CENTER_THREE_DIALOG:
		switch (m_eStep)
		{
		case CENTER_ONE_DIALOG:
			txtNum = 52;
			break;
		case CENTER_TWO_DIALOG:
			txtNum = 53;
			break;
		case CENTER_THREE_DIALOG:
			txtNum = 56;
			break;
		default:
			break;
		}
		createAsideBg();
		createAsideText();
		createAnchr();
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	default:
		break;
	}
}
void UINoviceStoryLine::createGuideDialogAllNeed_5()
{
	switch (m_eStep)
	{
	case START_DIALOG:
		txtNum = 0;
		createWineHouseBg();
		wineHouseBg->setOpacity(255);
		createDockBg();
		createAide();
		createHero();
		creatMajorFigure(DIALOG_PATH[COLUMBUS_PATH]);
		createSupportingRole(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_BAR_GIRL));
		createDboxLeft();
		createDboxRight();
		createAsideBg();
		createText();
		createAnchr();
		createButtonSkip();
		SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_ONE_DIALOG:
		if (SINGLE_HERO->m_iGender == 1)
		{
			txtNum = 29;
		}
		else
		{
			txtNum = 24;
		}
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createShipyardBoss();
		createDboxLeft();
		createDboxRight();
		createDialogBranchButton(txtNum + 9);
		createText();
		createAnchr();
		createButtonSkip();
		createNameBoat();
		SINGLE_AUDIO->vplayBGMusic(audio_effect[AUDIO_EFFECT_MEW_06]);
		m_eName = SHIPYARDBOSS;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxRight->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case SHIPYARD_TWO_DIALOG:
		if (SINGLE_HERO->m_iGender == 1)
		{
			txtNum = 42;
		}
		else
		{
			txtNum = 37;
		}
		createShipyardBg();
		shipyardBg->setOpacity(255);
		createAide();
		createDboxLeft();
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	case TARVEN_ONE_DIALOG:
	case TARVEN_TWO_DIALOG:
	case TARVEN_THREE_DIALOG:
	case MARKET_BUY_DIALOG:
	case DOCK_ONE_DIALOG:
	case DOCK_TWO_DIALOG:
	case SAILING_GUIDE_DIALOG:
	case WAR_GUIDE_DIALOG:
	case WAR_FAIL_GUIDE_DIALOG:
	case PALACE_DIALOG:
	case MARKET_SELL_DIALOG:
		if (SINGLE_HERO->m_iGender == 1)
		{
			switch (m_eStep)
			{
			case TARVEN_ONE_DIALOG:
				txtNum = 43;
				break;
			case TARVEN_TWO_DIALOG:
				txtNum = 44;
				break;
			case TARVEN_THREE_DIALOG:
				txtNum = 45;
				createWineHouseBg();
				wineHouseBg->setOpacity(255);
				break;
			case MARKET_BUY_DIALOG:
				txtNum = 46;
				break;
			case DOCK_ONE_DIALOG:
				txtNum = 47;
				break;
			case DOCK_TWO_DIALOG:
				txtNum = 48;
				break;
			case SAILING_GUIDE_DIALOG:
				txtNum = 49;
				break;
			case WAR_GUIDE_DIALOG:
				txtNum = 54;
				break;
			case WAR_FAIL_GUIDE_DIALOG:
				txtNum = 60;
				break;
			case PALACE_DIALOG:
				txtNum = 61;
				break;
			case MARKET_SELL_DIALOG:
				txtNum = 63;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (m_eStep)
			{
			case TARVEN_ONE_DIALOG:
				txtNum = 38;
				break;
			case TARVEN_TWO_DIALOG:
				txtNum = 39;
				break;
			case TARVEN_THREE_DIALOG:
				txtNum = 40;
				createWineHouseBg();
				wineHouseBg->setOpacity(255);
				break;
			case MARKET_BUY_DIALOG:
				txtNum = 41;
				break;
			case DOCK_ONE_DIALOG:
				txtNum = 42;
				break;
			case DOCK_TWO_DIALOG:
				txtNum = 43;
				break;
			case SAILING_GUIDE_DIALOG:
				txtNum = 44;
				break;
			case WAR_GUIDE_DIALOG:
				txtNum = 49;
				break;
			case WAR_FAIL_GUIDE_DIALOG:
				txtNum = 55;
				break;
			case PALACE_DIALOG:
				txtNum = 56;
				break;
			case MARKET_SELL_DIALOG:
				txtNum = 57;
				break;
			default:
				break;
			}
		}
		createAide();
		createDboxLeft();
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG)
		{
			createAsideBg();
			createButtonSkip();
		}
		createText();
		createAnchr();
		m_eName = AIDE;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
		dboxLeft->runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::personCome, this)), DelayTime::create(0.5), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		if (m_eStep == SAILING_GUIDE_DIALOG || m_eStep == WAR_GUIDE_DIALOG || m_eStep == WAR_FAIL_GUIDE_DIALOG)
		{
			guideWidget->setCameraMask(4);
		}
		break;
	case CENTER_ONE_DIALOG:
	case CENTER_TWO_DIALOG:
	case CENTER_THREE_DIALOG:
		if (SINGLE_HERO->m_iGender == 1)
		{
			switch (m_eStep)
			{
			case CENTER_ONE_DIALOG:
				txtNum = 63;
				break;
			case CENTER_TWO_DIALOG:
				txtNum = 64;
				break;
			case CENTER_THREE_DIALOG:
				txtNum = 67;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (m_eStep)
			{
			case CENTER_ONE_DIALOG:
				txtNum = 58;
				break;
			case CENTER_TWO_DIALOG:
				txtNum = 59;
				break;
			case CENTER_THREE_DIALOG:
				txtNum = 62;
				break;
			default:
				break;
			}
		}
		createAsideBg();
		createAsideText();
		createAnchr();
		asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		break;
	default:
		break;
	}
}
/*
*创建guideWidget 设置其点击事件 并判断读取（主线对话或触发剧情）json文件
*/
void UINoviceStoryLine::createLayer()
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
        UINoviceStoryLine::noviceClickEvent();
    };

    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = callback;
	listener->onTouchEnded = callback2;
    listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,guideWidget);
	//判断读取（主线对话或触发剧情）json文件
	UINoviceStoryLine::replaceJsonString();
}
/*
*获取主线对话json内容 或触发剧情内容（dialogType为0时为主线剧情 dialogType为1时是触发剧情）
*获取到的剧情的名字存到m_jsonName 获取到的剧情的内容存到m_jsonDialog  
*遍历m_jsonName和m_jsonDialog名字替换 返回替换后的名字和内容
*/
void UINoviceStoryLine::replaceJsonString()
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
	m_jsonName.clear();
	m_jsonDialog.clear();
	int count = SINGLE_SHOP->getGuideTaskDialogInfo().size();
	log("count:%d-%d", count, __LINE__);
	map <int, GUIDETASKDIALOGINFO>::iterator m1_Iter;
	auto taskMap = SINGLE_SHOP->getGuideTaskDialogInfo();
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
}
/*
*对话层的点击事件
*layerFarmClickBoo为false时可点击
*点击时判断是否翻页
*/
void UINoviceStoryLine::noviceClickEvent()
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
					UINoviceStoryLine::dialogSentenceStepByStep(txtNum);
				}
				else
				{
					txtNum = txtNum + 1;
					if (titleTxt)
					{
						titleTxt->setString("");
					}
					txtTxt->setString("");
					lenNum = 0;
					txtTxt->setPosition(maskPic->getPositionX(), maskPic->getPositionY());
					UINoviceStoryLine::dialogSentenceStepByStep(txtNum);
				}
			}
		}
		else
		{
			this->unschedule(schedule_selector(UINoviceStoryLine::showText));
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
void UINoviceStoryLine::createWineHouseBg()
{
	wineHouseBg = Sprite::create();
	wineHouseBg->setTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_PUP_AREA));
	wineHouseBg->setScale(Director::getInstance()->getVisibleSize().height/wineHouseBg->getContentSize().height,Director::getInstance()->getVisibleSize().height/wineHouseBg->getContentSize().height);
	wineHouseBg->setAnchorPoint(Vec2(0.5,0.5));
	wineHouseBg->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(wineHouseBg);
	wineHouseBg->setOpacity(0);
}
//创建港口背景
void UINoviceStoryLine::createDockBg()
{
	dockBg = Sprite::create();
	dockBg->setTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_WHARF_AREA));
	dockBg->setScale(Director::getInstance()->getVisibleSize().height/dockBg->getContentSize().height,Director::getInstance()->getVisibleSize().height/dockBg->getContentSize().height);
	dockBg->setAnchorPoint(Vec2(0.5,0.5));
	dockBg->setPosition(Director::getInstance()->getVisibleOrigin().x+Director::getInstance()->getVisibleSize().width/2,Director::getInstance()->getVisibleOrigin().y+Director::getInstance()->getVisibleSize().height/2);
	guideWidget->addChild(dockBg);
	dockBg->setOpacity(0);
}
//船坞背景
void UINoviceStoryLine::createShipyardBg()
{
	shipyardBg = Sprite::create();
	shipyardBg->setTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_DOCK_AREA));
	shipyardBg->setScale(Director::getInstance()->getVisibleSize().height / shipyardBg->getContentSize().height, Director::getInstance()->getVisibleSize().height / shipyardBg->getContentSize().height);
	shipyardBg->setAnchorPoint(Vec2(0.5, 0.5));
	shipyardBg->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(shipyardBg);
	shipyardBg->setOpacity(0);
}
//西班牙打架
void UINoviceStoryLine::createFightBg()
{
	//fightpic 800*480
	fightPic = Sprite::create();
	fightPic->setTexture(DIALOG_PATH[FIGHT_PATH]);
	fightPic->setScale(Director::getInstance()->getVisibleSize().width / fightPic->getContentSize().width, Director::getInstance()->getVisibleSize().height / fightPic->getContentSize().height);
	fightPic->setAnchorPoint(Vec2(0.5, 0.5));
	fightPic->setPosition(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(fightPic);
	fightPic->setOpacity(0);
}
//创建对话通用背景
void UINoviceStoryLine::createDialogBg(string m_path)
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
void UINoviceStoryLine::createAide()
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
void UINoviceStoryLine::createHero()
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
void UINoviceStoryLine::creatMajorFigure(string m_path)
{
	majorFigurePic = Sprite::create();
	majorFigurePic->setTexture(m_path);
	majorFigurePic->setAnchorPoint(Vec2(0,0));
	majorFigurePic->setScale((576.0/720*Director::getInstance()->getVisibleSize().height)/majorFigurePic->getContentSize().height,(576.0/720*Director::getInstance()->getVisibleSize().height)/majorFigurePic->getContentSize().height);
	OFFSET = majorFigurePic->getBoundingBox().size.width;// + 70.0/1280*Director::getInstance()->getVisibleSize().width;
	majorFigurePic->setPosition(- majorFigurePic->getBoundingBox().size.width,0);
	guideWidget->addChild(majorFigurePic);
}
//创建西班牙流氓ab
void UINoviceStoryLine::creatRuffian()
{
	//西班牙流氓ab
	ruffianAPic = Sprite::create();
	ruffianAPic->setTexture(DIALOG_PATH[RUFFIAN_1_PATH]);
	ruffianAPic->setAnchorPoint(Vec2(0, 0));
	ruffianAPic->setFlippedX(true);
	ruffianAPic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / ruffianAPic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / ruffianAPic->getContentSize().height);
	OFFSET = ruffianAPic->getBoundingBox().size.width;// + 70.0 / 1280 * Director::getInstance()->getVisibleSize().width;
	ruffianAPic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
	guideWidget->addChild(ruffianAPic);
	ruffianBPic = Sprite::create();
	ruffianBPic->setTexture(DIALOG_PATH[RUFFIAN_2_PATH]);
	ruffianBPic->setAnchorPoint(Vec2(0, 0));
	ruffianBPic->setFlippedX(true);
	ruffianBPic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / ruffianBPic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / ruffianBPic->getContentSize().height);
	ruffianBPic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
	guideWidget->addChild(ruffianBPic);
}
//创建船坞船老板
void UINoviceStoryLine::createShipyardBoss()
{
	shipyardBossPic = Sprite::create();
	shipyardBossPic->setTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_SHIP_BOSS));
	shipyardBossPic->setAnchorPoint(Vec2(0, 0));
	shipyardBossPic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / shipyardBossPic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / shipyardBossPic->getContentSize().height);
	OFFSET = shipyardBossPic->getBoundingBox().size.width;
	shipyardBossPic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
	shipyardBossPic->setFlippedX(true);
	guideWidget->addChild(shipyardBossPic);
}
//创建西班牙酒吧老板（女郎）
void UINoviceStoryLine::createSupportingRole(std::string m_path)
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
//创建里斯本船长
void UINoviceStoryLine::createCaptain()
{
	captainPic = Sprite::create();
	captainPic->setTexture(DIALOG_PATH[CAPTAIN_PATH]);
	captainPic->setAnchorPoint(Vec2(0, 0));
	captainPic->setScale((576.0 / 720 * Director::getInstance()->getVisibleSize().height) / captainPic->getContentSize().height, (576.0 / 720 * Director::getInstance()->getVisibleSize().height) / captainPic->getContentSize().height);
	OFFSET = captainPic->getBoundingBox().size.width;
	captainPic->setPosition(-captainPic->getBoundingBox().size.width, 0);
	guideWidget->addChild(captainPic);
}
//旁白对话背景
void UINoviceStoryLine::createAsideBg()
{
	asideBg = Sprite::create();
	asideBg->setTexture(DIALOG_PATH[ASIDE_PATH]);
	asideBg->setScale(Director::getInstance()->getVisibleSize().width / asideBg->getContentSize().width,Director::getInstance()->getVisibleSize().width / asideBg->getContentSize().width);
	asideBg->setAnchorPoint(Vec2(0,0));
	guideWidget->addChild(asideBg);
	asideBg->setOpacity(0);
}
//左对话背景
void UINoviceStoryLine::createDboxLeft()
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
void UINoviceStoryLine::createDboxRight()
{
	dboxRight = Sprite::create();
	dboxRight->setTexture(DIALOG_PATH[DBOXRIGHT_PATH]);
	log("NoviceDialogLayer::createDboxRight:0x%x, path:%s",(long)dboxRight,DIALOG_PATH[DBOXRIGHT_PATH].c_str());
	dboxRight->setAnchorPoint(Vec2(0,0));
	dboxRight->setScale((742.0 / 1280 * Director::getInstance()->getVisibleSize().width) / dboxLeft->getContentSize().width, (742.0 / 1280 * Director::getInstance()->getVisibleSize().width) / dboxLeft->getContentSize().width);
	dboxRight->setPosition(Director::getInstance()->getVisibleSize().width - 95.0 / 1280 * Director::getInstance()->getVisibleSize().width - dboxLeft->getBoundingBox().size.width, 30);
	guideWidget->addChild(dboxRight);
	dboxRight->setOpacity(0);
}
//点击提示图标
void UINoviceStoryLine::createAnchr()
{
	anchr = Sprite::create();
	anchr->setTexture(DIALOG_PATH[ANCHR_PATH]);
	guideWidget->addChild(anchr);
	anchr->setOpacity(0);
	anchr->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0,10))),EaseBackOut::create(MoveBy::create(0.5, Vec2(0,-10))))));
}
//略过按钮
void UINoviceStoryLine::createButtonSkip()
{
	btnSkip = Button::create();
	btnSkip->loadTextureNormal(DIALOG_PATH[BUTTON_SKIP_PATH]);
	btnSkip->addTouchEventListener(CC_CALLBACK_2(UINoviceStoryLine::buttonSkipEvent,this));
	btnSkip->setTouchEnabled(true);
	btnSkip->setPosition(Vec2(Director::getInstance()->getVisibleOrigin().x + 1139.0 / 1280 * Director::getInstance()->getVisibleSize().width, Director::getInstance()->getVisibleOrigin().y + 665.0 / 720 * Director::getInstance()->getVisibleSize().height));
	guideWidget->addChild(btnSkip);
	auto txt = Label::createWithSystemFont("", CUSTOM_FONT_NAME_1, 24);
	txt->setColor(Color3B(255, 255, 255));
	txt->setPosition(Vec2(120, 27));
	txt->setString(SINGLE_SHOP->getTipsInfo()["TIP_BUTTON_SKIP_TEXT"]);
	btnSkip->addChild(txt);
}
//对话文本
void UINoviceStoryLine::createText()
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
//对话文本
void UINoviceStoryLine::createAsideText()
{
	ClippingNode *clippingNode = ClippingNode::create();
	clippingNode->setAlphaThreshold(0);
	clippingNode->setPosition(0, 0);
	clippingNode->setAnchorPoint(Vec2(0, 0));
	guideWidget->addChild(clippingNode);

	//文字
	txtTxt = Label::createWithSystemFont("", CUSTOM_FONT_NAME_2, FontSize);
	txtTxt->setColor(Color3B(238, 230, 202));
	txtTxt->enableShadow();
	txtTxt->setAnchorPoint(Vec2(0, 1));
	txtTxt->setWidth(865);
	txtTxt->setPosition(asideBg->getPositionX() + 225.0 / 1280 * Director::getInstance()->getVisibleSize().width, asideBg->getPositionY() + asideBg->getBoundingBox().size.height - (157 / 286.0)*asideBg->getBoundingBox().size.height);
	clippingNode->addChild(txtTxt);

	auto txt0 = Label::createWithSystemFont("", CUSTOM_FONT_NAME_2, FontSize);
	txt0->setWidth(540);
	txt0->setAnchorPoint(Vec2(0, 1));
	txt0->setString("test");
	m_nOneLineHeight = txt0->getBoundingBox().size.height;

	maskPic = Sprite::create();
	maskPic->setTexture(DIALOG_PATH[MASK_PATH]);
	maskPic->setAnchorPoint(Vec2(0, 1));
	maskPic->setPosition(txtTxt->getPositionX(), txtTxt->getPositionY() + 2);
	maskPic->setTextureRect(Rect(0, 0, txtTxt->getWidth(), m_nOneLineHeight * 3));
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
	log("textTime = %f", textTime);
}
//创建有对话支线的按钮
void UINoviceStoryLine::createDialogBranchButton(int Num)
{
	branchTxtNum = Num;
	std::string dialog1 = "";
	std::string dialog2 = "";
	std::string dialog3 = "";
	switch (m_eStep)
	{
	case SHIPYARD_ONE_DIALOG:
	{
												//"是/否"
												dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_THREE_SECTION_ONE_OPTION_ONE"];
												dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_THREE_SECTION_ONE_OPTION_TWO"];
												break;
	}
	default:
		break;
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
	branchButton1->addTouchEventListener(CC_CALLBACK_2(UINoviceStoryLine::branchClickEvent,this));
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
	branchButton2->addTouchEventListener(CC_CALLBACK_2(UINoviceStoryLine::branchClickEvent,this));
	zhixianBgPic->addChild(branchButton2);
	branchButton2->setTouchEnabled(false);
	auto startY = 152;
	branchButton1->setPosition(Vec2(265, startY));
	branchButton2->setPosition(Vec2(265, startY - 60));
}
//创建船坞给船命名界面
void UINoviceStoryLine::createNameBoat()
{
	nameListPic = Sprite::create();
	nameListPic->setTexture("login_ui/player_720/dropdialog_bg.png");
	nameListPic->setPosition(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
	guideWidget->addChild(nameListPic);
	nameListPic->setVisible(false);

	auto t_title = Label::createWithSystemFont("", CUSTOM_FONT_NAME_2, 24);
	t_title->setColor(Color3B(225, 210, 157));
	t_title->setPosition(Vec2(320, 323));
	t_title->setWidth(460);
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SHIPYARD_NAME_SHIP_TITLE"]);
	t_title->setHorizontalAlignment(TextHAlignment::CENTER);
	t_title->setVerticalAlignment(TextVAlignment::CENTER);
	nameListPic->addChild(t_title);

	auto inputPic = Sprite::create();
	inputPic->setTexture("login_ui/market_720/input_bg.png");
	inputPic->setPosition(Vec2(322, 186));
	inputPic->runAction(ScaleTo::create(0.01, 474.0 / 214,60.0/58));
	nameListPic->addChild(inputPic);

	nameTxt = ui::EditBox::create(Size(460, 36), "input.png");
	nameTxt->setTag(1);
	nameListPic->addChild(nameTxt);
	nameTxt->setText((SINGLE_SHOP->getTipsInfo()["TIP_SHIPYARD_NAME_SHIP_EXAMPLE"]).c_str());
	nameTxt->setPosition(Vec2(322, 186));
	nameTxt->ignoreContentAdaptWithSize(false);
	nameTxt->setPlaceholderFontColor(Color3B(116, 98, 71));
	nameTxt->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
	nameTxt->setFontColor(Color3B(46, 29, 14));
	nameTxt->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	nameTxt->setReturnType(ui::EditBox::KeyboardReturnType::DONE);


	LanguageType nType = LanguageType(Utils::getLanguage());
	char buffer[10];
	sprintf(buffer, "%d", ProtocolThread::GetInstance()->getIcon());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
	case cocos2d::LanguageType::CHINESE:
		nameTxt->setMaxLength(10);
		break;
	default:
		nameTxt->setMaxLength(20);
		break;
	}
	btn_nameOk = Button::create();
	btn_nameOk->loadTextureNormal("login_ui/common/yes.png");
	btn_nameOk->setPosition(Vec2(321,16));
	btn_nameOk->addTouchEventListener(CC_CALLBACK_2(UINoviceStoryLine::buttonNameOkEvent,this));
	btn_nameOk->setTouchEnabled(false);
	nameListPic->addChild(btn_nameOk);
}

//设定播放对话定时器
void UINoviceStoryLine::openText()
{
	 this->schedule(schedule_selector(UINoviceStoryLine::showText),textTime);
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
void UINoviceStoryLine::showText(float f)
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
		
		this->unschedule(schedule_selector(UINoviceStoryLine::showText));
        return;
	}
}
void UINoviceStoryLine::buttonSkipEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		if (layerFarmClickBoo == false)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			btnSkip->setTouchEnabled(false);
			this->unschedule(schedule_selector(UINoviceStoryLine::showText));
			layerFarmClickBoo = true;
			skipNum = skipNum + 1;
			len = maxLen;
			auto endNum1 = 0;
			auto endNum2 = 0;
			switch (m_eStep)
			{
			case START_DIALOG:
			case SHIPYARD_ONE_DIALOG:
			{
										switch (SINGLE_HERO->m_iNation)
										{
										case 1:
											endNum1 = 24;
											endNum2 = 29;
											break;
										case 2:
											endNum1 = 30;
											endNum2 = 35;
											break;
										case 3:
											endNum1 = 23;
											endNum2 = 28;
											break;
										case 4:
											endNum1 = 18;
											endNum2 = 23;
											break;
											break;
										case 5:
											if (SINGLE_HERO->m_iGender == 1)
											{
												endNum1 = 29;
												endNum2 = 34;
											}
											else
											{
												endNum1 = 24;
												endNum2 = 29;
											}
											break;
										default:
											break;
										}
										if (txtNum < endNum1)
										{
											//初始对话 直接跳转 TODO
											removeGuideWidget();
											CHANGETO(SCENE_TAG::MAINGUID_TAG);
											UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 1);
											UserDefault::getInstance()->flush();
											return;
										}
										else if (skipNum == 1 && txtNum < endNum2)
										{
											//直接显示命名框
											anchr->setOpacity(0);
											if (txtNum > endNum1)
											{
												AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, 100, 100, 100)));
											}
											shipyardBossPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, 100, 100, 100)));
											titleTxt->setString("");
											txtTxt->setString("");
											dboxLeft->runAction(FadeOut::create(0.01));
											dboxRight->runAction(FadeOut::create(0.01));
											nameListPic->setVisible(true);
											btnSkip->setVisible(false);
											btn_nameOk->setTouchEnabled(true);
										}
										else if (txtNum >= endNum2 || skipNum == 2)
										{
											//直接跳转船坞界面
											anchr->setOpacity(0);
											UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASHIPNAME").c_str(), nameTxt->getText());
											UserDefault::getInstance()->flush();
											registerCallBack();
											ProtocolThread::GetInstance()->setMainTaskStatus((char*)(nameTxt->getText()), 1);
											unregisterCallBack();
											//TODO 跳转船坞
											removeGuideWidget();
											auto layerDock = UIGuideShipyard::createDockLayerGuide();
											Scene*currentScene = Director::getInstance()->getRunningScene();
											auto guideDialogLayer = UINoviceStoryLine::GetInstance();
											currentScene->addChild(layerDock, 100);
											if (guideDialogLayer->getParent())
											{
												guideDialogLayer->removeFromParent();
											}
										}
			}
				break;
			case SAILING_GUIDE_DIALOG:
				createSaillingHelp();
				break;
			case WAR_GUIDE_DIALOG:
				createWarGuideHelp();
				break;
			default:
				break;
			}
		}
	}
}
void UINoviceStoryLine::buttonNameOkEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		nameListPic->setVisible(false);
		btn_nameOk->setTouchEnabled(false);
		btnSkip->setVisible(true);
		btnSkip->setTouchEnabled(true);
		if (skipNum == 1)
		{
			titleTxt->setString("");
			txtTxt->setString("");
			switch (SINGLE_HERO->m_iNation)
			{
			case 1:
				txtNum = 29;
				break;
			case 2:
				txtNum = 35;
				break;
			case 3:
				txtNum = 28;
				break;
			case 4:
				txtNum = 23;
				break;
			case 5:
				if (SINGLE_HERO->m_iGender == 1)
				{
					txtNum = 34;
				}
				else
				{
					txtNum = 29;
				}
				break;
			default:
				break;
			}
		}
		UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASHIPNAME").c_str(), nameTxt->getText());
		UserDefault::getInstance()->flush();
		registerCallBack();
		ProtocolThread::GetInstance()->setMainTaskStatus((char*)(nameTxt->getText()), 1);
		unregisterCallBack();
		shipyardBossPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
		AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}
//对话中支线的点击事件
void UINoviceStoryLine::branchClickEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		zhixianBgPic->setVisible(false);
		auto target = (Widget*)pSender;
		auto name = target->getName();
		target->setTouchEnabled(false);
		if (m_eStep == SHIPYARD_ONE_DIALOG)
		{
			if (name == "branchButton1")
			{
				txtNum = branchTxtNum;
			}
			else if (name == "branchButton2")
			{
				txtNum = branchTxtNum + 2;
			}
		}
		dboxLeft->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}
//显示插画时定时器调用的函数
void UINoviceStoryLine::showIllustration(float f)
{
	IllustrationNum = IllustrationNum + 1;
	if (IllustrationNum == 2)
	{
		this->unschedule(schedule_selector(UINoviceStoryLine::showIllustration));
		fightPic->runAction(FadeOut::create(0.1));
		wineHouseBg->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (SINGLE_HERO->m_iGender == 1)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
			majorFigurePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
			dboxLeft->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
}

//剧情对话人物出场 为动作进行先后而调用
void UINoviceStoryLine::personCome()
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
		captainPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case PASENGERA:
		ruffianAPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case SAILOR:
		supportingRolePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case SOLDIER:
		supportingRolePic->setPosition(-supportingRolePic->getBoundingBox().size.width, 0);
		supportingRolePic->setFlippedX(false);
		supportingRolePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
		break;
	case SHIPYARDBOSS:
		shipyardBossPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)))));
	default:
		break;
	}
}

/*
 * 对话句子按照点击继续下一句 直到结束删除对话层 传给服务器结束剧情
 * 包括主线剧情、触发剧情
 * 主线剧情时，先排除是否为海上引导、战斗引导和战败引导的索引，其他根据国家来走剧情
 */
void UINoviceStoryLine::dialogSentenceStepByStep(int Num)
{
	txtNum = Num;
	this->unschedule(schedule_selector(UINoviceStoryLine::showText));
	anchr->setOpacity(0);
	switch (SINGLE_HERO->m_iNation)
	{
	case 1:
		UINoviceStoryLine::dialogSpeakOrder1_0();
		break;
	case 2:
		if (SINGLE_HERO->m_iGender == 1)
		{
			UINoviceStoryLine::dialogSpeakOrder2_1();
		}
		else
		{
			UINoviceStoryLine::dialogSpeakOrder2_2();
		}
		break;
	case 3:
		UINoviceStoryLine::dialogSpeakOrder3_0();
		break;
	case 4:
		UINoviceStoryLine::dialogSpeakOrder4_0();
		break;
	case 5:
		if (SINGLE_HERO->m_iGender == 1)
		{
			UINoviceStoryLine::dialogSpeakOrder5_1();
		}
		else
		{
			UINoviceStoryLine::dialogSpeakOrder5_2();
		}
		break;
	default:
		break;
	}
}
//葡萄牙对话播放顺序
void UINoviceStoryLine::dialogSpeakOrder1_0()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		{
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  captainPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				  break;
		}
		case 2:
		case 4:
		case 9:
		{
				  if (txtNum == 2 || txtNum == 9)
				  {
						SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
						HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  }
				  else
				  {
					  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  }
				  break;
		}
		case 3:
		{
				  //旁白
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CRASH_18);
				  captainPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  break;
		}
		case 5:
		case 10:
		{
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 8:
		{
				  //旁白 转场酒馆 加开门音效
				  SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
				  majorFigurePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				  dockBg->runAction(TintTo::create(0.5, 10, 10, 10));
				  wineHouseBg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5),FadeIn::create(0.5)));
				  break;
		}
		case 21:
		{
				   majorFigurePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 22:
		{
				   majorFigurePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255))));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 24:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 49:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 62:
		case 65:
		{
				   //剧情结束处理
				   if (txtNum == 24 || txtNum == 41)
				   {
					   if (txtNum == 24)
					   {
						   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
						   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					   }
					   else if (txtNum == 41)
					   {
						   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   }
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
					   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   SINGLE_AUDIO->vstopBGMusic();
				   }
				   else if (txtNum == 37 || txtNum == 38)
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   shipyardBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   }
				   else if (txtNum == 49)
				   {
					   createSaillingHelp();
					   return;
				   }
				   else if (txtNum == 55)
				   {
					   createWarGuideHelp();
					   return;
				   }
				   else if (txtNum == 56)
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
					   return;
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
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UINoviceStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 25:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 29:
		{
				   //人物退出 显示船命名界面
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   shipyardBossPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   break;
		}
		case 32:
		{
				   shipyardBossPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 35:
		{
				   txtNum = 36;
				   break;
		}
		case 45:
		case 50:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	if (titleTxt)
	{
		titleTxt->setString("");
	}
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[0] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[5] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[21] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && captainPic)
		{
			captainPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[24] && shipyardBossPic)
		{
			shipyardBossPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
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
	if (m_jsonName[txtNum] == m_jsonName[0] && asideBg)
	{
		if (txtNum > 45 && txtNum != 50)
		{
			asideBg->runAction(Sequence::create(FadeIn::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && captainPic)
	{
		captainPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 1)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[5] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[21] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 29)
		{
			//显示船命名界面
			nameListPic->setVisible(true);
			nameTxt->setTouchEnabled(true);
			btn_nameOk->setTouchEnabled(true);
			btnSkip->setVisible(false);
		}
		else if (txtNum == 33)
		{
			//显示分支
			if (len >= maxLen)
			{
				UINoviceStoryLine::showBranch();
			}
			else
			{
				dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 21 || txtNum == 25)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum > 33 && txtNum < 37)
				{
					AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
					dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 2 ||txtNum == 9)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[24] && shipyardBossPic)
	{
		shipyardBossPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}

//西班牙男版对话播放顺序
void UINoviceStoryLine::dialogSpeakOrder2_1()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		{
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  supportingRolePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 2:
		case 4:
		{
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 3:
		{
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  ruffianAPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 5:
		{
				  //旁白 
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  ruffianAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 6:
		{
				  majorFigurePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				  ruffianAPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 7:
		case 12:
		{
				  ruffianBPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  ruffianAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 9:
		{
				  ruffianBPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  ruffianAPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 11:
		case 25:
		{
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
				  HeroPic->setFlippedX(false);
				  if (txtNum == 11)
				  {
						SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
						HeroPic->runAction(Spawn::create(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)), nullptr));
				  }
				  else
				  {
					  HeroPic->runAction(Spawn::create(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)), nullptr));
				  }
				  break;
		}
		case 13:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   ruffianBPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_TUSSLE_04);
				   fightPic->runAction(FadeIn::create(0.01));
				   fightPic->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.1, 255, 255, 255), TintTo::create(0.1, 180, 180, 180))));
				   this->schedule(schedule_selector(UINoviceStoryLine::showIllustration), 1);
				   break;
		}
		case 14:
		{
				   HeroPic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   HeroPic->setFlippedX(true);
				   HeroPic->runAction(Spawn::create(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)), nullptr));
				   break;
		}
		case 22:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(Sequence::create(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)), nullptr));
				   break;
		}
		case 28:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   majorFigurePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   majorFigurePic->setFlippedX(true);
				   majorFigurePic->runAction(Spawn::create(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)), nullptr));
				   break;
		}
		case 30:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 55:
		case 61:
		case 62:
		case 63:
		case 64:
		case 65:
		case 68:
		case 71:
		{
				   //剧情结束处理
				   if (txtNum == 30 || txtNum == 47)
				   {
					   if (txtNum == 30)
					   {
						   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
						   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   }
					   else if (txtNum == 47)
					   {
						   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   }
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
					   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   SINGLE_AUDIO->vstopBGMusic();
				   }
				   else if (txtNum == 43 || txtNum == 44)
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   shipyardBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   }
				   else if (txtNum == 55)
				   {
					   createSaillingHelp();
					   return;
				   }
				   else if (txtNum == 61)
				   {
					   createWarGuideHelp();
					   return;
				   }
				   else if (txtNum == 62)
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
					   return;
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
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UINoviceStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 31:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 35:
		{
				   //人物退出 显示船命名界面
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   shipyardBossPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   break;
		}
		case 38:
		{
				   shipyardBossPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 41:
		{
				   txtNum = 42;
				   break;
		}
		case 51:
		case 56:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	if (titleTxt)
	{
		titleTxt->setString("");
	}
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[0] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[22] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[11] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[30] && shipyardBossPic)
		{
			shipyardBossPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[3] && ruffianAPic)
		{
			ruffianAPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[7] && ruffianBPic)
		{
			ruffianBPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
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
	if (m_jsonName[txtNum] == m_jsonName[0] && asideBg)
	{
		if (txtNum > 51 && txtNum != 56)
		{
			asideBg->runAction(Sequence::create(FadeIn::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && supportingRolePic)
	{
		supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 1)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum != 13)
		{
			if (txtNum == 28)
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[22] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 35)
		{
			//显示船命名界面
			nameListPic->setVisible(true);
			nameTxt->setTouchEnabled(true);
			btn_nameOk->setTouchEnabled(true);
			btnSkip->setVisible(false);
		}
		else if (txtNum == 39)
		{
			//显示分支
			if (len >= maxLen)
			{
				UINoviceStoryLine::showBranch();
			}
			else
			{
				dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 31)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum > 39 && txtNum < 43)
				{
					AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
					dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					if (txtNum == 22 || txtNum == 24 || txtNum == 26)
					{
						if (txtNum == 22)
						{
							dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
						}
						else
						{
							dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
						}
					}
					else
					{
						dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
					}
					
				}
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[11] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 11 || txtNum == 25 || txtNum == 27 || txtNum == 29)
		{
			if (txtNum == 11)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[30] && shipyardBossPic)
	{
		shipyardBossPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[3] && ruffianAPic)
	{
		ruffianAPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 3)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[7] && ruffianBPic)
	{
		ruffianBPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}
//西班牙女版对话播放顺序
void UINoviceStoryLine::dialogSpeakOrder2_2()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		{
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  ruffianAPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 2:
		{
				  HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
				  HeroPic->setFlippedX(false);
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 3:
		{
				  //旁白 
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  ruffianAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 4:
		{
				  //旁白 
				  HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				  ruffianAPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 5:
		case 10:
		{
				   ruffianBPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   ruffianAPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 7:
		{
				  ruffianBPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  ruffianAPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 9:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 11:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   ruffianBPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_TUSSLE_04);
				   fightPic->runAction(FadeIn::create(0.01));
				   fightPic->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.1, 255, 255, 255), TintTo::create(0.1, 180, 180, 180))));
				   this->schedule(schedule_selector(UINoviceStoryLine::showIllustration), 1);
				   break;
		}
		case 12:
		{
				   HeroPic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   HeroPic->setFlippedX(true);
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 13:
		{
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 22:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(Sequence::create(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)), nullptr));
				   break;
		}
		case 25:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
				   HeroPic->setFlippedX(false);
				   HeroPic->runAction(Spawn::create(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)), nullptr));
				   break;
		}
		case 28:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   majorFigurePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   majorFigurePic->setFlippedX(true);
				   majorFigurePic->runAction(Spawn::create(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(255, 255, 255)), nullptr));
				   break;
		}
		case 30:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 55:
		case 61:
		case 62:
		case 63:
		case 64:
		case 65:
		case 68:
		case 71:
		{
				   //剧情结束处理
				   if (txtNum == 30 || txtNum == 47)
				   {
					   if (txtNum == 30)
					   {
						   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
						   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   }
					   else if (txtNum == 47)
					   {
						   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   }
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
					   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   SINGLE_AUDIO->vstopBGMusic();
				   }
				   else if (txtNum == 43 || txtNum == 44)
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   shipyardBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   }
				   else if (txtNum == 55)
				   {
					   createSaillingHelp();
					   return;
				   }
				   else if (txtNum == 61)
				   {
					   createWarGuideHelp();
					   return;
				   }
				   else if (txtNum == 62)
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
					   return;
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
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UINoviceStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 31:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 35:
		{
				   //人物退出 显示船命名界面
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   shipyardBossPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   break;
		}
		case 38:
		{
				   shipyardBossPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 41:
		{
				   txtNum = 42;
				   break;
		}
		case 51:
		case 56:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	if (titleTxt)
	{
		titleTxt->setString("");
	}
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[0] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[9] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[22] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[30] && shipyardBossPic)
		{
			shipyardBossPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && ruffianAPic)
		{
			ruffianAPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[5] && ruffianBPic)
		{
			ruffianBPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
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
	if (m_jsonName[txtNum] == m_jsonName[0] && asideBg)
	{
		if (txtNum != 11)
		{
			if (txtNum > 51 && txtNum != 56)
			{
				asideBg->runAction(Sequence::create(FadeIn::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[9] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 28)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[22] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 35)
		{
			//显示船命名界面
			nameListPic->setVisible(true);
			nameTxt->setTouchEnabled(true);
			btn_nameOk->setTouchEnabled(true);
			btnSkip->setVisible(false);
		}
		else if (txtNum == 39)
		{
			//显示分支
			if (len >= maxLen)
			{
				UINoviceStoryLine::showBranch();
			}
			else
			{
				dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 31)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum > 39 && txtNum < 43)
				{
					AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
					dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					if (txtNum == 22 || txtNum == 24 || txtNum == 26 || txtNum == 27)
					{
						if (txtNum == 22)
						{
							dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
						}
						else
						{
							dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
						}
					}
					else
					{
						dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
					}

				}
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 2 || txtNum == 6 || txtNum == 8 || txtNum == 25 || txtNum == 27 || txtNum == 29)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[30] && shipyardBossPic)
	{
		shipyardBossPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && ruffianAPic)
	{
		ruffianAPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 1)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[5] && ruffianBPic)
	{
		ruffianBPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}
//英国男女版对话播放顺序
void UINoviceStoryLine::dialogSpeakOrder3_0()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		{
				  dialogBg->runAction(TintTo::create(0.5, 10, 10, 10));
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_OPEN_DOOR_32);
				  wineHouseBg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.3),FadeIn::create(0.3)));
				  SINGLE_AUDIO->vplayMusicByType(MUSIC_TAVERN);
				  majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				  break;
		}
		case 2:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 4:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  supportingRolePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 5:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 17:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				  AidePic->setFlippedX(true);
				  AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 19:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
				   HeroPic->setFlippedX(false);
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 21:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 23:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 48:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 61:
		case 64:
		{
				   //剧情结束处理
				   if (txtNum == 23 || txtNum == 40)
				   {
					   if (txtNum == 23)
					   {
						   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
						   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					   }
					   else if (txtNum == 40)
					   {
						   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   }
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
					   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   SINGLE_AUDIO->vstopBGMusic();
				   }
				   else if (txtNum == 36 || txtNum == 37)
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   shipyardBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   }
				   else if (txtNum == 48)
				   {
					   createSaillingHelp();
					   return;
				   }
				   else if (txtNum == 54)
				   {
					   createWarGuideHelp();
					   return;
				   }
				   else if (txtNum == 55)
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
					   return;
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
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UINoviceStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 24:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 28:
		{
				   //人物退出 显示船命名界面
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   shipyardBossPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   break;
		}
		case 31:
		{
				   shipyardBossPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 34:
		{
				   txtNum = 35;
				   break;
		}
		case 44:
		case 49:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	if (titleTxt)
	{
		titleTxt->setString("");
	}
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[0] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[17] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[4] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[23] && shipyardBossPic)
		{
			shipyardBossPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
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
	if (m_jsonName[txtNum] == m_jsonName[0] && asideBg)
	{
		if (txtNum > 44 && txtNum != 49)
		{
			asideBg->runAction(Sequence::create(FadeIn::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 1)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[17] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 28)
		{
			//显示船命名界面
			nameListPic->setVisible(true);
			nameTxt->setTouchEnabled(true);
			btn_nameOk->setTouchEnabled(true);
			btnSkip->setVisible(false);
		}
		else if (txtNum == 32)
		{
			//显示分支
			if (len >= maxLen)
			{
				UINoviceStoryLine::showBranch();
			}
			else
			{
				dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 24)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum > 32 && txtNum < 36)
				{
					AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
					dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					if (txtNum == 17 || txtNum == 20 || txtNum == 22)
					{
						dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
					}
					else
					{
						dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
					}
				}
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 19)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[23] && shipyardBossPic)
	{
		shipyardBossPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[4] && supportingRolePic)
	{
		supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}
//荷兰男版对话播放顺序
void UINoviceStoryLine::dialogSpeakOrder4_0()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		{
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 2:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 11:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   AidePic->setFlippedX(true);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				   break;
		}
		case 13:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   HeroPic->setPosition(-HeroPic->getBoundingBox().size.width, 0);
				   HeroPic->setFlippedX(false);
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 15:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 16:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   HeroPic->setPosition(Director::getInstance()->getVisibleSize().width, 0);
				   HeroPic->setFlippedX(true);
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 18:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 43:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 56:
		case 59:
		{
				   //剧情结束处理
				   if (txtNum == 18 || txtNum == 35)
				   {
					   if (txtNum == 18)
					   {
						   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
						   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					   }
					   else if (txtNum == 35)
					   {
						   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   }
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
					   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   SINGLE_AUDIO->vstopBGMusic();
				   }
				   else if (txtNum == 31 || txtNum == 32)
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   shipyardBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   }
				   else if (txtNum == 43)
				   {
					   createSaillingHelp();
					   return;
				   }
				   else if (txtNum == 49)
				   {
					   createWarGuideHelp();
					   return;
				   }
				   else if (txtNum == 50)
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
					   return;
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
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UINoviceStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 19:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 23:
		{
				   //人物退出 显示船命名界面
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   shipyardBossPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   break;
		}
		case 26:
		{
				   shipyardBossPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 29:
		{
				   txtNum = 30;
				   break;
		}
		case 39:
		case 44:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	if (titleTxt)
	{
		titleTxt->setString("");
	}
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[0] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[11] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[18] && shipyardBossPic)
		{
			shipyardBossPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
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
	if (m_jsonName[txtNum] == m_jsonName[0] && asideBg)
	{
		if (txtNum > 39 && txtNum != 44)
		{
			asideBg->runAction(Sequence::create(FadeIn::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[11] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 23)
		{
			//显示船命名界面
			nameListPic->setVisible(true);
			nameTxt->setTouchEnabled(true);
			btn_nameOk->setTouchEnabled(true);
			btnSkip->setVisible(false);
		}
		else if (txtNum == 27)
		{
			//显示分支
			if (len >= maxLen)
			{
				UINoviceStoryLine::showBranch();
			}
			else
			{
				dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 19)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum > 27 && txtNum < 31)
				{
					AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
					dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					if (txtNum == 11 || txtNum == 14)
					{
						if (txtNum == 11)
						{
							dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
						}
						else
						{
							dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
						}
					}
					else
					{
						dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
					}
				}
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 13)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			if (txtNum == 1)
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[18] && shipyardBossPic)
	{
		shipyardBossPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}

//热那亚男版对话播放顺序
void UINoviceStoryLine::dialogSpeakOrder5_1()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		{
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 2:
		{
				  supportingRolePic->setPosition(-supportingRolePic->getBoundingBox().size.width, 0);
				  supportingRolePic->setFlippedX(false);
				  supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 5:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  supportingRolePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 6:
		case 14:
		case 24:
		{
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				  if (txtNum == 24)
				  {
					  HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				  }
				  break;
		}
		case 7:
		case 15:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 13:
		case 23:
		case 28:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  if (txtNum == 23)
				  {
						wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
						SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
						dockBg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.3), FadeIn::create(0.3)));
						SINGLE_AUDIO->vplayBGMusic(audio_effect[AUDIO_EFFECT_MEW_06]);
				  }
				  break;
		}
		case 25:
		{
				  majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 26:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 29:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
		case 49:
		case 54:
		case 60:
		case 61:
		case 62:
		case 63:
		case 64:
		case 67:
		case 70:
		{
				   //剧情结束处理
				   if (txtNum == 29 || txtNum == 46)
				   {
					   if (txtNum == 29)
					   {
						   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
						   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					   }
					   else if (txtNum == 46)
					   {
						   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
						   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
						   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   }
					   SINGLE_AUDIO->vstopBGMusic();
				   }
				   else if (txtNum == 42 || txtNum == 43)
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   shipyardBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   }
				   else if (txtNum == 54)
				   {
					   createSaillingHelp();
					   return;
				   }
				   else if (txtNum == 60)
				   {
					   createWarGuideHelp();
					   return;
				   }
				   else if (txtNum == 61)
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
					   return;
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
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UINoviceStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 30:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 34:
		{
				   //人物退出 显示船命名界面
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   shipyardBossPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   break;
		}
		case 37:
		{
				   shipyardBossPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 40:
		{
				   txtNum = 41;
				   break;
		}
		case 50:
		case 55:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	if (titleTxt)
	{
		titleTxt->setString("");
	}
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[0] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[6] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[25] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[29] && shipyardBossPic)
		{
			shipyardBossPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
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
	if (m_jsonName[txtNum] == m_jsonName[0] && asideBg)
	{
		if (txtNum > 50 && txtNum != 55)
		{
			asideBg->runAction(Sequence::create(FadeIn::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[6] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 6 || txtNum == 14 || txtNum == 24)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[25] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 34)
		{
			//显示船命名界面
			nameListPic->setVisible(true);
			nameTxt->setTouchEnabled(true);
			btn_nameOk->setTouchEnabled(true);
			btnSkip->setVisible(false);
		}
		else if (txtNum == 38)
		{
			//显示分支
			if (len >= maxLen)
			{
				UINoviceStoryLine::showBranch();
			}
			else
			{
				dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 30)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum > 38 && txtNum < 42)
				{
					AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
					dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 1)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[29] && shipyardBossPic)
	{
		shipyardBossPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && supportingRolePic)
	{
		supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}
//热那亚男版对话播放顺序
void UINoviceStoryLine::dialogSpeakOrder5_2()
{
	if (len >= maxLen)
	{
		//剧情中每句的人物动作、背景、声音的处理
		switch (txtNum)
		{
		case 1:
		{
				  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				  HeroPic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-OFFSET, 0))));
				  break;
		}
		case 2:
		{
				  supportingRolePic->setPosition(-supportingRolePic->getBoundingBox().size.width, 0);
				  supportingRolePic->setFlippedX(false);
				  supportingRolePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  break;
		}
		case 5:
		{
				  HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				  supportingRolePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				  break;
		}
		case 6:
		case 18:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   majorFigurePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   if (txtNum == 18)
				   {
					   HeroPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   }
				   break;
		}
		case 7:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 17:
		case 23:
		{
				   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   if (txtNum == 17)
				   {
						wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
						SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
						dockBg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.3), FadeIn::create(0.3)));
						SINGLE_AUDIO->vplayBGMusic(audio_effect[AUDIO_EFFECT_MEW_06]);
				   }
				   break;
		}
		case 19:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 20:
		{
				   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		case 24:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 49:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 62:
		case 65:
		{
				   //剧情结束处理
				   if (txtNum == 24 || txtNum == 41)
				   {
					   if (txtNum == 24)
					   {
						   majorFigurePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
						   HeroPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
					   }
					   else if (txtNum == 41)
					   {
						   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
						   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CLOSE_DOOR_33);
						   wineHouseBg->runAction(TintTo::create(0.5, 10, 10, 10));
					   }
					   SINGLE_AUDIO->vstopBGMusic();
				   }
				   else if (txtNum == 37 || txtNum == 38)
				   {
					   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
					   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_GO_35);
					   shipyardBg->runAction(TintTo::create(0.5, 10, 10, 10));
				   }
				   else if (txtNum == 49)
				   {
					   createSaillingHelp();
					   return;
				   }
				   else if (txtNum == 55)
				   {
					   createWarGuideHelp();
					   return;
				   }
				   else if (txtNum == 56)
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
					   return;
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
				   guideWidget->runAction(Sequence::create(Spawn::createWithTwoActions(TintTo::create(0.5f, Color3B(0, 0, 0)), FadeIn::create(0.5f)), CallFunc::create(this, callfunc_selector(UINoviceStoryLine::removeGuideWidget)), nullptr));

				   return;
		}
		case 25:
		{
				   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FOOTSTEPS_COME_34);
				   AidePic->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(OFFSET, 0))));
				   break;
		}
		case 29:
		{
				   //人物退出 显示船命名界面
				   AidePic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(-OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   shipyardBossPic->runAction(Spawn::createWithTwoActions(MoveBy::create(0.5, Vec2(OFFSET, 0)), TintTo::create(0.5, Color3B(100, 100, 100))));
				   break;
		}
		case 32:
		{
				   shipyardBossPic->runAction(MoveBy::create(0.5, Vec2(OFFSET, 0)));
				   break;
		}
		case 35:
		{
				   txtNum = 36;
				   break;
		}
		case 45:
		case 50:
		{
				   AidePic->runAction(MoveBy::create(0.5, Vec2(-OFFSET, 0)));
				   break;
		}
		default:
			break;
		}
	}
	if (titleTxt)
	{
		titleTxt->setString("");
	}
	txtTxt->setString("");
	//检测上一句说话的人物 颜色转暗
	if (txtNum > 0)
	{
		if (m_jsonName[txtNum - 1] == m_jsonName[0] && asideBg)
		{
			asideBg->runAction(FadeOut::create(0.01));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[6] && majorFigurePic)
		{
			majorFigurePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[25] && AidePic)
		{
			AidePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[1] && HeroPic)
		{
			HeroPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[2] && supportingRolePic)
		{
			supportingRolePic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
		}
		else if (m_jsonName[txtNum - 1] == m_jsonName[24] && shipyardBossPic)
		{
			shipyardBossPic->runAction(TintTo::create(0.5, Color3B(100, 100, 100)));
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
	if (m_jsonName[txtNum] == m_jsonName[0] && asideBg)
	{
		if (txtNum > 45 && txtNum != 50)
		{
			asideBg->runAction(Sequence::create(FadeIn::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			asideBg->runAction(Sequence::create(FadeIn::create(1), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[6] && majorFigurePic)
	{
		majorFigurePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 6 || txtNum == 18)
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[25] && AidePic)
	{
		AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 29)
		{
			//显示船命名界面
			nameListPic->setVisible(true);
			nameTxt->setTouchEnabled(true);
			btn_nameOk->setTouchEnabled(true);
			btnSkip->setVisible(false);
		}
		else if (txtNum == 33)
		{
			//显示分支
			if (len >= maxLen)
			{
				UINoviceStoryLine::showBranch();
			}
			else
			{
				dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
		}
		else
		{
			if (txtNum == 25)
			{
				dboxLeft->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
			}
			else
			{
				if (txtNum > 33 && txtNum < 37)
				{
					AidePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
					dboxLeft->runAction(Sequence::create(FadeIn::create(0.01), DelayTime::create(0.01), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
				else
				{
					dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
				}
			}
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[1] && HeroPic)
	{
		HeroPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		if (txtNum == 1)
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.8), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
		else
		{
			dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
		}
	}
	else if (m_jsonName[txtNum] == m_jsonName[24] && shipyardBossPic)
	{
		shipyardBossPic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxRight->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
	else if (m_jsonName[txtNum] == m_jsonName[2] && supportingRolePic)
	{
		supportingRolePic->runAction(TintTo::create(0.5, Color3B(255, 255, 255)));
		dboxLeft->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5), DelayTime::create(0.3), CallFunc::create(CC_CALLBACK_0(UINoviceStoryLine::showDialog, this)), nullptr));
	}
}

//主线对话结束时删除层并且主线任务提交
void UINoviceStoryLine::removeGuideWidget()
{
	auto stageNum = 0;
	auto endNum = 0;
	switch (SINGLE_HERO->m_iNation)
	{
	case 1:
		endNum = 24;
		break;
	case 2:
		endNum = 30;
		break;
	case 3:
		endNum = 23;
		break;
	case 4:
		endNum = 18;
		break;
	case 5:
		if (SINGLE_HERO->m_iGender == 1)
		{
			endNum = 29;
		}
		else
		{
			endNum = 24;
		}
		break;
	default:
		break;
	}
	if (txtNum == endNum || txtNum == (endNum + 14) || txtNum == (endNum + 17))
	{
		//TODO 初始对话结束 跳转
		if (txtNum == endNum)
		{
			stageNum = 1;
		}
		//TODO 跳转主城界面
		CHANGETO(SCENE_TAG::MAINGUID_TAG);
	}
	else if (txtNum == (endNum + 13))
	{
		//TODO 跳转船坞界面

		
		Scene*currentScene = Director::getInstance()->getRunningScene();
		auto guideDialogLayer = UINoviceStoryLine::GetInstance();
		if (guideDialogLayer->getParent())
		{
			guideDialogLayer->removeFromParent();
		}
		auto layerDock = UIGuideShipyard::createDockLayerGuide();
		currentScene->addChild(layerDock,100);
	}
	guideWidget->removeFromParentAndCleanup(true);
	reset();
	//TODO 记录阶段
	if (stageNum)
	{
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), stageNum);
		UserDefault::getInstance()->flush();
	}
	if (SINGLE_HERO->m_bDialogToTarven)
	{
		SINGLE_HERO->m_bDialogToTarven = false;
		auto currentScene = Director::getInstance()->getRunningScene();
		auto pupGuideLayre = (UIGuideTarven*)(currentScene->getChildByTag(SCENE_TAG::PUP_GUIDE_TAG + 100));
		pupGuideLayre->showBarGirlChat(0);
	}
	auto parent = this->getParent();
    if (dynamic_cast<UIGuideMain*>(parent) != nullptr)
    {
		dynamic_cast<UIGuideMain*>(parent)->guide();
    }
	if (dynamic_cast<UIGuidePort*>(parent) != nullptr)
	{
		dynamic_cast<UIGuidePort*>(parent)->guide();
	}
	if (dynamic_cast<UIGuideCenterCharactorSkill*>(parent) != nullptr)
	{
		dynamic_cast<UIGuideCenterCharactorSkill*>(parent)->guide();
	}
//	CHANGETO(SCENE_TAG::MAINGUID_TAG);
	allGuideTaskInfo(true,0.5f);
}

//显示对话支线
void UINoviceStoryLine::showBranch()
{
	if (zhixianBgPic && branchButton1 && branchButton2)
	{
		zhixianBgPic->setVisible(true);
		branchButton1->setTouchEnabled(true);
		branchButton2->setTouchEnabled(true);
	}
}
//显示对话(逐字显示播放对话内容)
void UINoviceStoryLine::showDialog()
{
	txtTxt->setString("");
	if (titleTxt)
	{
		titleTxt->setString("");
	}
	std::string name;
	std::string dialog;
	name = m_jsonName[txtNum];
	dialog = m_jsonDialog[txtNum];
	
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
	maskPic->setPosition(txtTxt->getPositionX(), txtTxt->getPositionY() + 2);
	lenNum = 0;
	UINoviceStoryLine::openText();
	layerFarmClickBoo = false;
}
//战斗引导	
void UINoviceStoryLine::createWarGuideHelp()
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
	i_warHelp->addTouchEventListener(CC_CALLBACK_2(UINoviceStoryLine::warStartEvent,this));
	i_warHelp->runAction(FadeIn::create(1));
}
//战斗引导点击事件
void UINoviceStoryLine::warStartEvent(Ref *pSender, Widget::TouchEventType TouchType)
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
void UINoviceStoryLine::createSaillingHelp()
{
	guideWidget->removeFromParentAndCleanup(true);
	reset();
	auto i_SaillingHelp = ImageView::create(GUIDE_SAILING[SINGLE_SHOP->L_TYPE]);
	i_SaillingHelp->setScale(Director::getInstance()->getVisibleSize().height / i_SaillingHelp->getContentSize().height, Director::getInstance()->getVisibleSize().height / i_SaillingHelp->getContentSize().height);
	i_SaillingHelp->setAnchorPoint(Vec2(0.5, 0.5));
	i_SaillingHelp->setPosition(Vec2(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleOrigin().y + Director::getInstance()->getVisibleSize().height / 2));
	this->addChild(i_SaillingHelp);
	i_SaillingHelp->setCameraMask(4);
	i_SaillingHelp->setTouchEnabled(true);
	i_SaillingHelp->addTouchEventListener(CC_CALLBACK_2(UINoviceStoryLine::startSailling,this));
}
//海上引导点击事件
void UINoviceStoryLine::startSailling(Ref *pSender, Widget::TouchEventType TouchType)
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
//变量重置
void UINoviceStoryLine::reset()
{
	guideWidget = nullptr;
	titleTxt = nullptr;
	txtTxt = nullptr;
	branchText1 = nullptr;
	branchText2 = nullptr;
	nameTxt = nullptr;

	wineHouseBg = nullptr;
	dockBg = nullptr;
	shipyardBg = nullptr;
	fightPic = nullptr;
	dialogBg = nullptr;
	AidePic = nullptr;
	HeroPic = nullptr;
	majorFigurePic = nullptr;
	ruffianAPic = nullptr;
	ruffianBPic = nullptr;
	shipyardBossPic = nullptr;
	supportingRolePic = nullptr;
	captainPic = nullptr;
	asideBg = nullptr;
	dboxLeft = nullptr;
	dboxRight = nullptr;
	anchr = nullptr;
	maskPic = nullptr;
	handPic = nullptr;
	zhixianBgPic = nullptr;

	branchButton1 = nullptr;
	branchButton2 = nullptr;
	btnSkip = nullptr;
	btn_nameOk = nullptr;
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

	skipNum = 0;

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
	m_nOneLineHeight = 0;
	m_jsonName.clear();
	m_jsonDialog.clear();
	log("reset ****************------------------");
}

void UINoviceStoryLine::stopAllAction()
{
	if (guideWidget)
	{
		guideWidget->removeFromParentAndCleanup(true);
	}
	reset();
}
