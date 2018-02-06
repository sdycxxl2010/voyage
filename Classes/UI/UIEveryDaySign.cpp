#include "UIEveryDaySign.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
#include "UITips.h"
#include "TVSingle.h"

#include "UIInform.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "UIMain.h"
#include "TVSceneLoader.h"
#include "UIOffLine.h"
#include "UISailManage.h"
USING_NS_CC;
using namespace rapidjson;
using namespace cocostudio;
UIEveryDaySign::UIEveryDaySign():
	m_nTotalDays(0)
{
	for (int i=0;i<ALLDAYS;i++)
	{
	signYesOrNoFlag[i]=CAN_SIGN;
	}
	m_nBtnTag=0;
	m_nMainCityOrSeaTag=-1;
	buttonClose=nullptr;
};
UIEveryDaySign::~UIEveryDaySign()
{

}
/*
 * tag :1出现在主城；2出现在海上
 */
UIEveryDaySign*UIEveryDaySign::createEveryDaysign(int tag)
{
	auto everySign=new UIEveryDaySign;
	everySign->m_nMainCityOrSeaTag=tag;
	if (everySign&&everySign->init())
	{
		everySign->autorelease();
		return everySign;
	}
	CC_SAFE_DELETE(everySign);
	return nullptr;
}
bool UIEveryDaySign::init()
{
	bool pRet = false;
	do 
	{
		this->scheduleOnce(schedule_selector(UIEveryDaySign::initStatic),0);
		pRet = true;
	} while (0);
	return pRet;
}
void UIEveryDaySign::initStatic(float f)
{

	openView(EVERYDAYSIGN_RES[VIEW_EVERYDAY_SIGN_PANEL]);
	auto viewSign = getViewRoot(EVERYDAYSIGN_RES[VIEW_EVERYDAY_SIGN_PANEL]);
	viewSign->setScale(0);
	viewSign->setOpacity(0);
	viewSign->setCascadeOpacityEnabled(true);
	viewSign->setOpacity(0);
	viewSign->runAction(Spawn::createWithTwoActions(FadeIn::create(0.5), Sequence::create(ScaleTo::create(0.5, 1), nullptr)));
	if (SINGLE_HERO->m_iDailyrewarddata)
	{
		m_nTotalDays = SINGLE_HERO->m_iDailyrewarddata[0]->rewardindex;//0-29
	}
	for (int i=0;i<ALLDAYS;i++)
	{
		if (i<m_nTotalDays)
		{
			signYesOrNoFlag[i] = HAVE_SIGNED;
		}
		else
		{
			signYesOrNoFlag[i] = CAN_SIGN;
		}
	}
	buttonClose = viewSign->getChildByName<Button*>("button_close");
	buttonClose->setTouchEnabled(false);
	updateDaysPanel();
	this->scheduleOnce(schedule_selector(UIEveryDaySign::todayAnimation),1.5f);
	if (m_nMainCityOrSeaTag==2)
	{
		this->setCameraMask(4, true);
	}
	
}
void UIEveryDaySign::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED!=TouchType)
	{
		return;
	}
	auto target = dynamic_cast<Widget*>(pSender);
	std::string name=target->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_close))
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openViewAutoClose("TIP_DAILYREWARD_SIGNUP_SUCCESS");//
		//clear by hand
		if (SINGLE_HERO->m_iDailyrewarddata)
		{
			SINGLE_HERO->m_iDailyrewarddata=nullptr;
			SINGLE_HERO->m_iNdailyrewarddata=0;
		}
		auto currentScene = Director::getInstance()->getRunningScene();
		auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
		auto maplayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
		if (mainlayer)//主城
		{
			mainlayer->notifyCompleted(DAILY_SIGN_IN);
		}
		else if (maplayer)
		{
			maplayer->notifyCompleted(DAILY_SIGN_IN);
		}
		this->removeFromParent();
		return;
	}
}
void UIEveryDaySign::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent( message, msgType);
}
void UIEveryDaySign::updateDaysPanel()
{
	int totalSignDays=0;
	
	if (SINGLE_HERO->m_iNdailyrewarddata<=0)
	{
		totalSignDays=m_nTotalDays+1;
	}else
	{
		totalSignDays=m_nTotalDays+SINGLE_HERO->m_iNdailyrewarddata;
		if (totalSignDays>30)
		{
			totalSignDays=totalSignDays-30;
		}
	}
	
	auto dayPanel = getViewRoot(EVERYDAYSIGN_RES[VIEW_EVERYDAY_SIGN_PANEL]);
	auto label_content=dayPanel->getChildByName<Text*>("label_content");
	label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_DAILYREWARD_KEEP_PLAYING"]);
	auto listView_Panel=dayPanel->getChildByName<ListView*>("ListView_dayPanel");
	listView_Panel->setDirection(ListView::Direction::VERTICAL);
	listView_Panel->removeAllChildrenWithCleanup(true);
	//left panel
	auto label_time=dayPanel->getChildByName<Text*>("label_time");
	label_time->setString(StringUtils::format("(%d/30)",totalSignDays));
	auto image_ship=dayPanel->getChildByName<ImageView*>("image_ship");
	auto label_you_get_item=dayPanel->getChildByName<Text*>("label_you_get_item");
	label_you_get_item->setContentSize(Size(label_you_get_item->getContentSize().width,34));
	//name,count
	std::string youGet=" ";
	
	if (totalSignDays==0)
	{
		image_ship->setVisible(false);
		label_you_get_item->setVisible(false);
	}
	else 
	{
		image_ship->setVisible(true);
		label_you_get_item->setVisible(true);
		if (totalSignDays==30)
		{
			if (SINGLE_HERO->m_iDailyrewarddata[0]->sid)
			{
				youGet = getShipName(SINGLE_HERO->m_iDailyrewarddata[0]->sid);
				image_ship->loadTexture(getShipIconPath(SINGLE_HERO->m_iDailyrewarddata[0]->sid, IMAGE_ICON_SHADE));
			}
			else
			{
				youGet = getShipName(27);
				image_ship->loadTexture(getShipIconPath(27, IMAGE_ICON_SHADE));
			}
			
			youGet+=StringUtils::format(" x%d",SINGLE_SHOP->getDailyRewardInfo()[30].ships_num);
			label_you_get_item->setString(youGet);
		}
		else
		{
			if (SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].items_id)
			{
				image_ship->loadTexture(getItemIconPath(SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].items_id,IMAGE_ICON_SHADE));
				youGet=SINGLE_SHOP->getItemData()[SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].items_id].name;
				youGet = youGet+" x"+StringUtils::format("%d",SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].items_num);
				label_you_get_item->setString(youGet);
			}else if (SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].coins)
			{
				image_ship->loadTexture(getItemIconPath(10002, IMAGE_ICON_SHADE));
				youGet = SINGLE_SHOP->getTipsInfo()["TIP_BANK_SILVER"];
				youGet = youGet+" x"+numSegment(StringUtils::format("%d",SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].coins));
				label_you_get_item->setString(youGet);
			}
			else
			{
				image_ship->loadTexture(getItemIconPath(10000, IMAGE_ICON_SHADE));
				youGet = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_GOLD"];
				youGet = youGet+" x"+numSegment(StringUtils::format("%d",SINGLE_SHOP->getDailyRewardInfo()[totalSignDays].v_tickets));
				label_you_get_item->setString(youGet);
			}		
		}
	}
	
	//right panel
	auto panel_items_1=dayPanel->getChildByName("panel_items_1");
	auto image_items_1=panel_items_1->getChildByName<Widget*>("image_items_1");//clone
	//30 day
	auto ships=SINGLE_SHOP->getDailyRewardInfo()[ALLDAYS].ships_id;

	for (int j=0;j<5;j++)
	{
		ListView*item_layer=ListView::create();
		item_layer->setDirection(ListView::Direction::HORIZONTAL);
		item_layer->setItemsMargin(0.5);
		item_layer->setContentSize(Size(480,76));
		Widget*single_btn;
		for (int i=0;i<6;i++)
		{
			if (6*j+i<30)
			{
				single_btn=(Widget*)image_items_1->clone();
				single_btn->setVisible(true);

				auto image_goods=single_btn->getChildByName<ImageView*>("image_goods");//icon
				auto button_equip_items=single_btn->getChildByName<Button*>("button_equip_items");//btn
				auto image_cd=single_btn->getChildByName<ImageView*>("image_cd");//have sign
				auto image_right=single_btn->getChildByName<ImageView*>("image_right");
				//tag
				single_btn->setTag(6*j+i+1+ALLDAYS);//31-60
				button_equip_items->setTag(6*j+i+1);//1-30
				button_equip_items->setTouchEnabled(false);
				if (6*j+i+1==ALLDAYS)
				{
					if (SINGLE_HERO->m_iDailyrewarddata[0]->sid)
					{
						image_goods->loadTexture(getShipIconPath(SINGLE_HERO->m_iDailyrewarddata[0]->sid));
					}
					else
					{
						image_goods->loadTexture(getShipIconPath(27));
					}							
				}
				else
				{
					if (SINGLE_SHOP->getDailyRewardInfo()[6*j+i+1].items_id)
					{
						image_goods->loadTexture(getItemIconPath(SINGLE_SHOP->getDailyRewardInfo()[6*j+i+1].items_id));//item					
					}
					else if(SINGLE_SHOP->getDailyRewardInfo()[6*j+i+1].coins)
					{
						image_goods->loadTexture(getItemIconPath(10002));//coin
					}
					else
					{
						image_goods->loadTexture(getItemIconPath(10000));//v					
					}
				}
				
				image_goods->ignoreContentAdaptWithSize(false);
				image_goods->setVisible(true);
				// sign or not
				if (signYesOrNoFlag[6*j+i]==HAVE_SIGNED)
				{
					image_cd->setVisible(true);
					image_right->setVisible(true);
				}
				else
				{
					image_cd->setVisible(false);
					image_right->setVisible(false);
				}
				//button_equip_items->addTouchEventListener(this,toucheventselector(EveryDaySign::goodItem_callfunc));
				item_layer->pushBackCustomItem(single_btn);
			}
		}
		listView_Panel->pushBackCustomItem(item_layer);
	}
}
void UIEveryDaySign::goodItem_callfunc(Ref *pSender,Widget::TouchEventType TouchType)
{
		if (TouchType!=Widget::TouchEventType::ENDED)
		{
			return;
		}
		auto btnDay=dynamic_cast<Widget*>(pSender);
		std ::string name=btnDay->getName();
		m_nBtnTag=btnDay->getTag();
		auto imageCd=btnDay->getParent()->getChildByName<ImageView*>("image_cd");//have sign
		auto imageRight=btnDay->getParent()->getChildByName<ImageView*>("image_right");
		if (isButton(button_equip_items))
		{
			imageCd->setVisible(true);
			imageRight->setVisible(true);
			signYesOrNoFlag[m_nBtnTag-1]=HAVE_SIGNED;
			return;
		}
}
void UIEveryDaySign::todayAnimation(float f)
{
	auto dayPanel = getViewRoot(EVERYDAYSIGN_RES[VIEW_EVERYDAY_SIGN_PANEL]);
	auto listView_Panel=dayPanel->getChildByName<ListView*>("ListView_dayPanel");
	
	if(SINGLE_HERO->m_iNdailyrewarddata==0)
	{
		buttonClose->setTouchEnabled(true);
		return;
	}
	if (m_nTotalDays+SINGLE_HERO->m_iNdailyrewarddata>30)//for a few days  continue
	{
		int startindex=m_nTotalDays+SINGLE_HERO->m_iNdailyrewarddata-30;
		for (int j=startindex;j<31;j++)
		{
			auto imageHaveSignBtn=dynamic_cast<Widget*>(Helper::seekWidgetByTag(listView_Panel,j+ALLDAYS));
			auto image_cdHaveSign=imageHaveSignBtn->getChildByName<ImageView*>("image_cd");
			auto image_rightHaveSign=imageHaveSignBtn->getChildByName<ImageView*>("image_right");
			image_cdHaveSign->setVisible(false);
			image_rightHaveSign->setVisible(false);
		}
	}
	for (int i=0;i<SINGLE_HERO->m_iNdailyrewarddata;i++)
	{
		auto imageBtn=dynamic_cast<Widget*>(Helper::seekWidgetByTag(listView_Panel,(m_nTotalDays+i)%30+1+ALLDAYS));
		auto image_right=imageBtn->getChildByName<ImageView*>("image_right");
		
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_USED_PROP_21);
		image_right->setVisible(true);
		image_right->setOpacity(0);
		image_right->setScale(0);
		image_right->runAction(Spawn::createWithTwoActions(FadeIn::create(0),Sequence::createWithTwoActions(ScaleTo::create(0.3,1.3),ScaleTo::create(0.3,1))));
	}
	
	buttonClose->setTouchEnabled(true);
	
}
