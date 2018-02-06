#include "UITips.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
#include "TVSingle.h"
UITips::UITips()
{
	
	m_vImage = cocos2d::Sprite::create();
	m_vImage->setTexture(ERROR_DIALOG_BG);
	addChild(m_vImage);
	auto textArea = m_vImage->getContentSize()*0.8;

	m_vText = cocos2d::ui::Text::create();
	m_vText->setTextAreaSize(textArea);
	m_vText->setFontSize(24);
	addChild(m_vText);
	
}

UITips::UITips(const char* text,int buttonNumver /* = 1*/)
{
	cocos2d::LayerColor* bgLayer = cocos2d::LayerColor::create(cocos2d::ccc4(0,0,0,90));//200,200,200,150
	bgLayer->setAnchorPoint(cocos2d::Vec2(0.5,0.5));
	auto winSize = cocos2d::Director::getInstance()->getWinSize();
	bgLayer->setPosition(-winSize.width*0.5,-winSize.height*0.5);
	bgLayer->changeWidthAndHeight(winSize.width,winSize.height);
	this->addChild(bgLayer,0);
	
	auto eventListener = cocos2d::EventListenerTouchOneByOne::create();
	eventListener->setSwallowTouches(true);
	eventListener->onTouchBegan = [](cocos2d::Touch* touch,cocos2d::Event* type){ return true;};
	
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener,bgLayer);
	
	
	m_sText = text;
	m_vImage = cocos2d::Sprite::create();
	m_vImage->setTexture(ERROR_DIALOG_BG);
	//..............................................................................................................................
	m_vImage->setScaleX(0.56);
	m_vImage->setScaleY(0.5);
	addChild(m_vImage);
	auto textArea = m_vImage->getContentSize();
	m_vText = cocos2d::ui::Text::create();
	m_vText->setContentSize(textArea);
	m_vText->setSize(cocos2d::Size(textArea.width * 0.4,150));
	m_vText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	m_vText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
#ifdef LINUX
	m_vText->setFontName("文泉驿微米黑");
#endif
	m_vText->ignoreContentAdaptWithSize(false);
	m_vText->setString(text);
	m_vText->setColor(cocos2d::ccc3(0,0,0));
	addChild(m_vText);
	if(buttonNumver == 1){
		
		positiveButton = cocos2d::ui::Button::create(DIALOG_BUTTON_IMG[0]);
		positiveButton->setScale(0.5);
		//positiveButton->setTitleText("O K");
		positiveButton->setTouchEnabled(true);
	
		positiveButton->setPosition(cocos2d::Vec2(0,-80));
		this->addChild(positiveButton);
		using namespace cocos2d::ui;
		positiveButton->addTouchEventListener(CC_CALLBACK_2(UITips::positiveCallback,this));
	}else{
		positiveButton = cocos2d::ui::Button::create(DIALOG_BUTTON_IMG[0]);
#ifdef LINUX
		positiveButton->setTitleFontName("文泉驿微米黑");
#endif
		//positiveButton->setTitleText("O K");
		positiveButton->setScale(0.5);
		positiveButton->setTouchEnabled(true);
		positiveButton->setPosition(cocos2d::Vec2(80,-80));
		this->addChild(positiveButton);
		using namespace cocos2d::ui;
		positiveButton->addTouchEventListener(CC_CALLBACK_2(UITips::positiveCallback,this));

		negativeButton = cocos2d::ui::Button::create(DIALOG_BUTTON_IMG[1]);
#ifdef LINUX
		negativeButton->setTitleFontName("文泉驿微米黑");
#endif
		//negativeButton->setTitleText("Cancel");
		negativeButton->setScale(0.5);
		negativeButton->setTouchEnabled(true);
		negativeButton->setPosition(cocos2d::Vec2(-80,-80));
		this->addChild(negativeButton);
		using namespace cocos2d::ui;
		negativeButton->addTouchEventListener(CC_CALLBACK_2(UITips::negativeCallback,this));
		//this->setCameraMask(4,true);
	}
}
UITips::UITips(const char* text,cocos2d::Size& textArea)
{
	m_sText = text;
	m_vImage = cocos2d::Sprite::create("");
	addChild(m_vImage);
	m_vText = cocos2d::ui::Text::create();
	m_vText->setString(text);
	m_vText->setSize(textArea);
#ifdef LINUX
	m_vText->setFontName("文泉驿微米黑");
#endif
	addChild(m_vText);
}

UITips::~UITips()
{

}


UITips* UITips::createTip(TVBasic* _self,int buttonNumber)
{
	UITips* tip = new UITips;
	cocos2d::Sprite* image = _self->getImage();
	
	if (!tip)
	{
		cocos2d::log("tip create fail !");
		return NULL;
	}
	tip->autorelease();
	return tip;
}
UITips* UITips::createTip(const char*  name,int buttonNumber)
{
	UITips* tip = new UITips(name,buttonNumber);
	if (!tip)
	{
		cocos2d::log("tip create fail !");
		return NULL;
	}
	auto winSize = cocos2d::Director::getInstance()->getWinSize();
	tip->setPosition(winSize.width*0.5,winSize.height*0.5);
	tip->setScaleX(1.6f);
	tip->setScaleY(1.5f);
	return tip;
}

UITips* UITips::createTip(const char*  name,cocos2d::Size& textArea,int buttonNumber)
{
	UITips* tip = new UITips(name,textArea);
	if (!tip)
	{
		cocos2d::log("tip create fail !");
		return NULL;
	}

	return tip;
}

void UITips::onEnter()
{
	Node::onEnter();
	//scheduleOnce(schedule_selector(GUITip::removeTextAndSelf),3.0f);
}
void UITips::onExit()
{
	Node::onExit();
}

void UITips::exitGame()
{
	cocos2d::Director::getInstance()->end();
}
void UITips::reconnect()
{
	//ProtocolThread::GetInstance()->reconnectServer();
}
void UITips::setPositiveText(const char* text){
	positiveButton->setTitleText(text);
}

void UITips::setNegativeText(const char* text){
	negativeButton->setTitleText(text);
}

void UITips::positiveCallback(Ref* obj,cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_vText->removeFromParentAndCleanup(true);
		this->removeFromParentAndCleanup(true);
		if(positive){
			positive();
		}
	}	
}

void UITips::negativeCallback(Ref* obj,cocos2d::ui::Widget::TouchEventType type){
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_vText->removeFromParentAndCleanup(true);
		this->removeFromParentAndCleanup(true);
		if(negative){
			negative();
		}else
		{
			exitGame();
		}
	}
}
