#include "UIVoyageSlider.h"
#include "UIBank.h"

UIVoyageSlider::UIVoyageSlider():
	m_slider(nullptr),
	m_vRoot(nullptr),
	m_bIsOk(false),
	m_iSumNumber(0),
	m_currentPercent(1),
	m_selectorEvent_1(nullptr),
	m_selectorEvent_2(nullptr),
	m_isOverWriter(false),
	m_isZero(false)
{
}

UIVoyageSlider::~UIVoyageSlider()
{
}
void UIVoyageSlider::onEnter()
{
	Node::onEnter();
	if (m_iSumNumber == 0)
	{
		return;
	}
	sliderEvent(nullptr,cocos2d::ui::SliderEventType::SLIDER_PERCENTCHANGED);
}
void UIVoyageSlider::onExit()
{
	Node::onExit();
}
bool UIVoyageSlider::init(cocos2d::ui::Widget* root,int sumNumbeer)
{
	bool pRet = false;
	do 
	{
		using namespace cocos2d;
		using namespace ui;
		Node::init();
		m_iSumNumber = sumNumbeer;
		m_currentPercent = sumNumbeer;
		root->setTouchEnabled(true);
		root->setVisible(true);
		root->setLocalZOrder(1);
		m_vRoot = root->getChildByName<Widget*>("panel_selectnum_bg");
		m_slider = dynamic_cast<Slider*>(m_vRoot->getChildByName<Widget*>("slider_shipment"));
		m_slider->setTouchEnabled(true);
		m_slider->addEventListenerSlider(this,sliderpercentchangedselector(UIVoyageSlider::sliderEvent));
		Vector<Node*> children = m_vRoot->getChildren();
		for(auto child : children)
		{
			Button* widget = dynamic_cast<Button*>(child);
			if(!widget) continue;
			widget->setTouchEnabled(true);
			widget->addTouchEventListener(CC_CALLBACK_2(UIVoyageSlider::buttonEvetn,this));
		}
		
		m_slider->setPercent(m_currentPercent * 100/m_iSumNumber);
		pRet = true;
	} while (0);

	return pRet;
}

bool UIVoyageSlider::init(cocos2d::ui::Widget* root,int sumNumbeer,size_t type,bool noValue)
{
	bool pRet = false;
	do 
	{
		using namespace cocos2d;
		using namespace ui;
		Node::init();
		m_isOverWriter = noValue;
		m_iSumNumber = sumNumbeer;
		m_currentPercent = sumNumbeer;
		//m_currentPercent = 0;
		m_vRoot = root;
		//m_vRoot = root->getChildByName<Widget*>("panel_selectnum_bg");
		for(auto child : m_vRoot->getChildren())
		{
			Slider *slider = dynamic_cast<Slider*>(child);
			if(slider)
			{
				m_slider = slider;
				break;
			}
		}
		if (m_iSumNumber <= 0)
		{
			m_slider->setPercent(100);
			pRet = true;
			break;
		}
		Node* parent = m_slider->getParent();
		m_slider->setTouchEnabled(true);
		m_slider->addEventListenerSlider(this,sliderpercentchangedselector(UIVoyageSlider::sliderEvent));
		
		m_slider->setPercent(m_currentPercent * 100/m_iSumNumber);

		Vector<Node*> children = m_vRoot->getChildren();
		for(auto child : children)
		{
			Button* widget = dynamic_cast<Button*>(child);
			if(!widget) continue;
			widget->setTouchEnabled(true);
			widget->addTouchEventListener(CC_CALLBACK_2(UIVoyageSlider::buttonEvetn,this));
		}
		pRet = true;
	} while (0);

	return pRet;
}

bool UIVoyageSlider::initSliderNum(int sumNumbeer,size_t type)
{
	m_iSumNumber = sumNumbeer;
	m_currentPercent = sumNumbeer;
	sliderEvent(nullptr,cocos2d::ui::SliderEventType::SLIDER_PERCENTCHANGED);
	m_slider->setPercent(m_currentPercent * 100/m_iSumNumber);
	return true;
}

void UIVoyageSlider::addSliderScrollEvent_1(selecterSliderScrollEvent_1 selectorEvent)
{
	if (selectorEvent != nullptr)
	{
		m_selectorEvent_1 = selectorEvent;
	}
}
void UIVoyageSlider::addSliderScrollEvent_2(selecterSliderScrollEvent_2 selectorEvent /* = nullptr */)
{
	if (selectorEvent != nullptr)
	{
		m_selectorEvent_2 = selectorEvent;
	}
}
UIVoyageSlider* UIVoyageSlider::create(cocos2d::ui::Widget* root,int sumNumbeer)
{
	UIVoyageSlider* slider = new UIVoyageSlider();
	if (slider && slider->init(root,sumNumbeer))
	{
		slider->autorelease();
		return slider;
	}

	CC_SAFE_DELETE(slider);
	return nullptr;
}
UIVoyageSlider* UIVoyageSlider::create(cocos2d::ui::Widget* root,int sumNumbeer,size_t icon_type,bool noValue)
{
	UIVoyageSlider* slider = new UIVoyageSlider();
	if (slider && slider->init(root,sumNumbeer,icon_type,noValue))
	{
		slider->autorelease();
		return slider;
	}

	CC_SAFE_DELETE(slider);
	return nullptr;
}
void UIVoyageSlider::close()
{
	if (this->getParent())
	{
		this->removeFromParentAndCleanup(true);
	}
	
}

void UIVoyageSlider::sliderEvent(Ref* obj,cocos2d::ui::SliderEventType type)
{
	if (m_iSumNumber < 1)
	{
		return;
	}
	if(!obj){
		if (!m_isOverWriter)
		{
			dynamic_cast<cocos2d::ui::Text*>(m_vRoot->getChildByName("label_num"))->setString(cocos2d::String::createWithFormat("%d",
				m_currentPercent)->getCString());
		}
		
		m_slider->setPercent(m_currentPercent* 100/m_iSumNumber);
		
	}else
	{
		
		m_currentPercent = m_slider->getPercent() * m_iSumNumber /100;
		if (m_currentPercent <1)
		{
			m_currentPercent = 1;
			m_slider->setPercent(m_currentPercent* 100/m_iSumNumber);
		}
		if(!m_isOverWriter)
		{
			dynamic_cast<cocos2d::ui::Text*>(m_vRoot->getChildByName("label_num"))->setString(cocos2d::String::createWithFormat("%d",
				m_iSumNumber* m_slider->getPercent()/100)->getCString());
		}
	}
	
	if (m_selectorEvent_1 != nullptr)
	{
		m_selectorEvent_1(m_currentPercent);
	}
	if (m_selectorEvent_2 != nullptr)
	{
		m_selectorEvent_2(this->getTag(),m_currentPercent);
	}
}

void UIVoyageSlider::setEnableZero(bool isZero)
{
	m_isZero = isZero;
}
void UIVoyageSlider::buttonEvetn(Ref* obj ,cocos2d::ui::Widget::TouchEventType type)
{
	if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
	{
		return;
	}
	std::string name = dynamic_cast<cocos2d::ui::Widget*>(obj)->getName();
	if (name.compare("button_plus")==0)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_currentPercent < m_iSumNumber)
		{
			++m_currentPercent;
			sliderEvent(nullptr,cocos2d::ui::SliderEventType::SLIDER_PERCENTCHANGED);
		}
		
	}else if(name.compare("button_minus")==0)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if ((m_currentPercent > 1 || m_isZero) && m_currentPercent >0)
		{
			--m_currentPercent;
			sliderEvent(nullptr,cocos2d::ui::SliderEventType::SLIDER_PERCENTCHANGED);
		}
		
	}else if(name.compare("button_ok")==0)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bIsOk = true;
		
		cocos2d::NotificationCenter::getInstance()->postNotification("sliderfinsh",this);
	}else if(name.compare("button_cancel")==0)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bIsOk = false;
		cocos2d::NotificationCenter::getInstance()->postNotification("sliderfinsh",this);
	}
	else if (name.compare("button_Slider_no")==0)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//m_vRoot->setPosition(ENDPOS);
		cocos2d::NotificationCenter::getInstance()->postNotification("close_palace_Slider",this);

	}
	else if (name.compare("button_Slider_yes")==0)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		cocos2d::NotificationCenter::getInstance()->postNotification("button_Slider_yes",this);
	}
	else if(name.compare("button_no")==0)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_vRoot->setPosition(ENDPOS);
	}
	else if (name.compare("button_provide_yes")==0)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		cocos2d::NotificationCenter::getInstance()->postNotification("button_provideSlider_yes",this);
	}

}
int UIVoyageSlider::getExchangeNumber() const
{
	if (m_bIsOk)
	{
		return m_currentPercent;
	}
	return 0;
	
}
int UIVoyageSlider::getCurrentNumber() const
{
	return m_currentPercent;
}

void UIVoyageSlider::setCurrentNumber(int num)
{
	if (m_iSumNumber > 0)
	{
		if(num > m_iSumNumber)
			num = m_iSumNumber;
		m_currentPercent = num;
		m_slider->setPercent((int)100*num/m_iSumNumber);
	}
}

int UIVoyageSlider::getMaxPercent() const
{
	return m_iSumNumber;
}

void UIVoyageSlider::setMaxPercent(int maxValue)
{
	if (maxValue > 0)
	{
		m_iSumNumber = maxValue;
		if (m_currentPercent > maxValue)
		{
			m_currentPercent = maxValue;
		}
		setCurrentNumber(m_currentPercent);
	}
}