#ifndef __GUI_TIP__
#define __GUI_TIP__
#include "TVBasic.h"
#include "ui/CocosGUI.h"

typedef std::function<void ()> DialogBtnCallBack;

class UITips : public cocos2d::Node
{
public:
	~UITips();
	UITips();
	UITips(const char* text,int buttonNumber = 1);
	UITips(const char* text,cocos2d::Size& textArea);
	std::string getText(){return m_sText;}
	
	static UITips* createTip(TVBasic* _self,int buttonNumber = 1);
	static UITips* createTip(const char* name,int buttonNumber = 1);
	static UITips* createTip(const char* name,cocos2d::Size& textArea,int buttonNumber = 1);
	
	void onEnter() override;
	void onExit() override;
	void exitGame();
	void reconnect();
	void setTextArea(cocos2d::Size& wAndH /* width and height */ ){ m_vText->setTextAreaSize(wAndH);}
	
	void setString(const char* text){ m_sText =  text; }
	void setPositiveText(const char* text);
	void setNegativeText(const char* text);
	void setPositiveCallback(DialogBtnCallBack callback){ positive = callback; }
	void setNegativeCallback(DialogBtnCallBack callback){ negative = callback; }

private:
	
	void positiveCallback(Ref* obj,cocos2d::ui::Widget::TouchEventType type);
	void negativeCallback(Ref* obj,cocos2d::ui::Widget::TouchEventType type);

	
	std::string m_sText;
	cocos2d::ui::Text* m_vText;
	cocos2d::Sprite* m_vImage;
	cocos2d::ui::Button* positiveButton;
	cocos2d::ui::Button* negativeButton;
	DialogBtnCallBack positive;
	DialogBtnCallBack negative;
	
};




#endif // __HELLOWORLD_SCENE_H__
