#include "UILoadingIndicator.h"
#include "TVSingle.h"
#include "UIBasicLayer.h"
#include "ui/CocosGUI.h"
#include"cocostudio/CocoStudio.h"
using namespace cocos2d;

USING_NS_CC;
UILoadingIndicator::UILoadingIndicator()
{

}

UILoadingIndicator::~UILoadingIndicator()
{
	_eventDispatcher->removeEventListenersForTarget(this);
}

 void UILoadingIndicator::onEnter(){ 
 	Layer::onEnter(); 
 }
void UILoadingIndicator::onExit(){ Layer::onExit(); setTouchEnabled(false);}

UILoadingIndicator* UILoadingIndicator::create(cocos2d::Layer*parent) 
{ 
	UILoadingIndicator *pRet = new UILoadingIndicator(); 
	if (pRet && pRet->init()) 
	{	
		pRet->showLoading_2(0);
		pRet->autorelease(); 
		pRet->retain();
		pRet->mParent = parent;
		return pRet; 
	} 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}
/*
 * Full Screen loading with tips.
 */
UILoadingIndicator* UILoadingIndicator::create(cocos2d::Layer*parent,int ui_Type) 
{ 
	UILoadingIndicator *pRet = new UILoadingIndicator(); 
	pRet->setSwallowsTouches(true);
	if (pRet && pRet->init()) 
	{
		pRet->Tipindex=ui_Type;//diff scene,diff tip content
		pRet->showLoading_1(0);
		pRet->autorelease(); 
		pRet->retain();
		pRet->mParent = parent;
		return pRet; 
	} 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}

UILoadingIndicator* UILoadingIndicator::createWithMask(cocos2d::Layer*parent, int cameraMask)
{
	UILoadingIndicator *pRet = create(parent);
	if(pRet){
		pRet->setCameraMask((unsigned short)cameraMask);
	}
	return pRet;
}

UILoadingIndicator* UILoadingIndicator::createWithMask(cocos2d::Layer*parent,int ui_Type , int cameraMask)
{
	UILoadingIndicator *pRet = create(parent, ui_Type);
	if(pRet){
		pRet->setCameraMask((unsigned short)cameraMask);
	}
	return pRet;
}

bool UILoadingIndicator::init()
{
	bool pRet = false;
	do 
	{
		CC_BREAK_IF(!Layer::init());
		this->setTouchEnabled(true);
		auto listener1=EventListenerTouchOneByOne::create();
		listener1->setSwallowTouches(true);
		listener1->onTouchBegan = [](Touch* touch, Event* event)
		{
			return true;
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1,this);

		pRet = true;
	} while (0);
	
	return pRet;
}

void UILoadingIndicator::showSelf(int tag){
	if(mParent){
		if (this->getParent() == nullptr)
		{
			
			if (tag == 0)
			{
				mParent->addChild(this,10000,200);
			}else
			{
				mParent->addChild(this,tag,tag);
			}
			
		}
	}
}

void UILoadingIndicator::removeSelf()
{
	
	if ( mParent)
	{
		this->removeFromParentAndCleanup(true);
	}else
	{
		delete this;
	}
	this->release();
}
void UILoadingIndicator::showLoading_1(float loadDiffTag)
{
	auto winSize = Director::getInstance()->getWinSize();
	auto image_bg = ImageView::create(StringUtils::format("res/loading/%d.jpg", SINGLE_SHOP->Loading_bg));
	image_bg->setTouchEnabled(true);
	image_bg->setContentSize(winSize);
	image_bg->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(image_bg);

	auto image_content_bg = ImageView::create("res/loading/content_bg.png");
	image_content_bg->setContentSize(Size(968, 144));
	image_content_bg->setPosition(Vec2(639, 135));
	this->addChild(image_content_bg);

	auto t_content = Text::create();
	t_content->setContentSize(Size(770, 144));
	t_content->setPosition(Vec2(639, 135));
	t_content->ignoreContentAdaptWithSize(false);
	t_content->setAnchorPoint(Vec2(0.5, 0.5));
	t_content->setTextHorizontalAlignment(TextHAlignment::CENTER);
	t_content->setTextVerticalAlignment(TextVAlignment::CENTER);
	t_content->setFontSize(24);
	t_content->setTextColor(Color4B(255, 255, 255, 255));
	this->addChild(t_content);
	int indextext = cocos2d::random(1, 15);
	t_content->setString(SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_LOADING_TEXT_%d", indextext)]);

	auto image_load_bg = ImageView::create("res/loading/load_bg.png");
	image_load_bg->setContentSize(Size(60, 60));
	image_load_bg->setPosition(Vec2(1050, 46));
	image_load_bg->ignoreContentAdaptWithSize(false);
	image_load_bg->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(image_load_bg);
	auto  LoadBglrepeat = RepeatForever::create(RotateBy::create(1, -180));
	image_load_bg->runAction(LoadBglrepeat);

	auto image_load = ImageView::create("res/loading/load.png");
	image_load->setContentSize(Size(36, 36));
	image_load->setPosition(Vec2(1050, 46));
	image_load->ignoreContentAdaptWithSize(false);
	image_load->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(image_load);
	auto LoadBgSmalllrepeat = RepeatForever::create(RotateBy::create(1, 180));
	image_load->runAction(LoadBgSmalllrepeat);

	auto image_text_bg = ImageView::create("res/loading/text_bg.png");
	image_text_bg->setContentSize(Size(188, 46));
	image_text_bg->setPosition(Vec2(1186, 45));
	this->addChild(image_text_bg);

	auto label_loading = Text::create();
	label_loading->setContentSize(Size(180, 40));
	label_loading->setPosition(Vec2(1178, 45));
	label_loading->setTextHorizontalAlignment(TextHAlignment::LEFT);
	label_loading->setTextVerticalAlignment(TextVAlignment::CENTER);
	label_loading->setFontSize(22);
	label_loading->setTextColor(Color4B(255, 255, 255, 255));
	this->addChild(label_loading);
	label_loading->setString(SINGLE_SHOP->getTipsInfo()["TIP_LOADING_TEXT"]);
	auto act_1 = FadeOut::create(1.0f);
	auto act_2 = FadeIn::create(1.0f);
	auto sequence = Sequence::create(act_1, act_2, NULL);
	auto labelrepeat = RepeatForever::create(sequence);
	label_loading->runAction(labelrepeat);
}
void UILoadingIndicator::showLoading_2(int loadDiffTag)
{
	auto winSize = Director::getInstance()->getWinSize();
	auto p_bg = Layout::create();
	p_bg->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
	p_bg->setBackGroundColor(Color3B(0, 0, 0));
	p_bg->setBackGroundColorOpacity(125);
	p_bg->setTouchEnabled(true);
	p_bg->setContentSize(winSize);
	p_bg->setPosition(Vec2(0, 0));
	this->addChild(p_bg);

	auto image_load_bg = ImageView::create("res/loading/load_bg.png");
	image_load_bg->setContentSize(Size(80, 80));
	image_load_bg->setPosition(Vec2(640, 359));
	image_load_bg->ignoreContentAdaptWithSize(false);
	image_load_bg->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(image_load_bg);
	auto  LoadBglrepeat = RepeatForever::create(RotateBy::create(1, -180));
	image_load_bg->runAction(LoadBglrepeat);

	auto image_load = ImageView::create("res/loading/load.png");
	image_load->setContentSize(Size(48, 48));
	image_load->setPosition(Vec2(640, 359));
	image_load->ignoreContentAdaptWithSize(false);
	image_load->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(image_load);
	auto LoadBgSmalllrepeat = RepeatForever::create(RotateBy::create(1, 180));
	image_load->runAction(LoadBgSmalllrepeat);

	auto label_loading = Text::create();
	label_loading->setContentSize(Size(500, 45));
	label_loading->setPosition(Vec2(640, 292));
	label_loading->setTextHorizontalAlignment(TextHAlignment::CENTER);
	label_loading->setTextVerticalAlignment(TextVAlignment::CENTER);
	label_loading->setFontSize(22);
	label_loading->setTextColor(Color4B(255, 255, 255, 255));
	this->addChild(label_loading);
	label_loading->setString(SINGLE_SHOP->getTipsInfo()["TIP_LOADING_TEXT"]);
	auto act_1 = FadeOut::create(1.0f);
	auto act_2 = FadeIn::create(1.0f);
	auto sequence = Sequence::create(act_1, act_2, NULL);
	auto labelrepeat = RepeatForever::create(sequence);
	label_loading->runAction(labelrepeat);
}
