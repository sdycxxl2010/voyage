#ifndef __SHIP_SLIDER_H__
#define __SHIP_SLIDER_H__
#include "cocos2d.h"
#include "ui/CocosGUI.h"

typedef std::function<void (int)> selecterSliderScrollEvent_1;
typedef std::function<void (int,int)> selecterSliderScrollEvent_2;
class UIVoyageSlider : public cocos2d::Node
{
public:
	bool	init(cocos2d::ui::Widget*,int sumNumbeer);
	bool	init(cocos2d::ui::Widget*,int sumNumbeer,size_t type,bool noValue);
	void	close();
	void	onEnter();
	void	onExit();
	void	setEnableZero(bool isZero);
	void	setMaxPercent(int);
	int		getMaxPercent() const;
	bool	initSliderNum(int sumNumbeer,size_t type);
	int		getExchangeNumber() const;
	int		getCurrentNumber() const;
	void	setCurrentNumber(int num);
	void	addSliderScrollEvent_1(selecterSliderScrollEvent_1 selectorEvent = nullptr);
	void	addSliderScrollEvent_2(selecterSliderScrollEvent_2 selectorEvent = nullptr);
	bool   m_bIsOk;
	static UIVoyageSlider* create(cocos2d::ui::Widget*,int sumNumbeer);
	static UIVoyageSlider* create(cocos2d::ui::Widget*,int sumNumbeer,size_t icon_type,bool noValue);
private:
	
	
	int    m_iSumNumber;
	size_t m_currentPercent;
	bool	m_isZero;
	bool	m_isOverWriter;
	
	void sliderEvent(Ref*,cocos2d::ui::SliderEventType);
	void buttonEvetn(Ref*,cocos2d::ui::Widget::TouchEventType);
	
	~UIVoyageSlider();
	UIVoyageSlider();
	selecterSliderScrollEvent_1 m_selectorEvent_1;
	selecterSliderScrollEvent_2 m_selectorEvent_2;
	cocos2d::ui::Slider* m_slider;
	cocos2d::ui::Widget* m_vRoot;
};

#endif