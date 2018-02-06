#include "UIInform.h"

#include "ui/CocosGUI.h"
#include"cocostudio/CocoStudio.h"


UIInform*  UIInform::m_informView = nullptr; 
UIInform::UIInform()
{
	m_index = -1;
}

UIInform::~UIInform()
{
	m_index = -1;
}
void UIInform::openInformView(UIBasicLayer *parent)
{
	while (m_vFilePath.size() != 0)
	{
		closeView();
	}

 	this->removeFromParentAndCleanup(true);
	parent->addChild(this, 10010, INFORMVIEWTAG);
// 	if (SINGLE_HERO->m_heroIsOnsea)
// 	{
// 		this->setCameraMask(4, true);
// 	}
// 	else
// 	{
// 		this->setCameraMask(1, true);
// 	}
	auto cameras = _director->getRunningScene()->getCameras();
	if (cameras.size() >= 2)
	{
		auto flag = (unsigned short)(cameras.at(cameras.size() - 1)->getCameraFlag());
		this->setCameraMask(flag,true);
	}
	else
	{
		this->setCameraMask(1,true);
	}

	m_pParent = parent;
}
void UIInform::openGoldNotEnoughYesOrNo(int num){
	std::string title = "TIP_GOLD_NOT";
	std::string content = "TIP_GOLD_NOT_CONTENT";
	m_index = C_VIEW_CONFIRM_CSB;
	openView(INFORM_COCOS_RES[C_VIEW_CONFIRM_CSB]);
	auto view=getViewRoot(INFORM_COCOS_RES[C_VIEW_CONFIRM_CSB]);

	auto tipInfos = SINGLE_SHOP->getTipsInfo();
	std::string info = tipInfos[content];
	std::string old_value = "[num]";
	std::string new_value = StringUtils::format("%d", num);
	repalce_all_ditinct(info, old_value, new_value);
	auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
	t_content->setString(info);//内容

	std::string titlecontent=tipInfos[title];
	auto t_titleContent=dynamic_cast<Text*>(view->getChildByName("label_repair"));
	t_titleContent->setString(titlecontent);

	auto btn_yes = view->getChildByName<Button*>("button_confirm_yes");
	btn_yes->addTouchEventListener(CC_CALLBACK_2(UIInform::confirmButtonEvent,this));
	auto btn_no = view->getChildByName<Button*>("button_confirm_no");
	if (btn_no)
	{
		btn_no->addTouchEventListener(CC_CALLBACK_2(UIInform::menuCall_func,this));
	}
	this->setCameraMask(_cameraMask, true);
}
void UIInform::openConfirmYesOrNO(std::string title, std::string content, bool Changedcontent)
{
	m_index = C_VIEW_CONFIRM_CSB;
	openView(INFORM_COCOS_RES[C_VIEW_CONFIRM_CSB]);
	auto view=getViewRoot(INFORM_COCOS_RES[C_VIEW_CONFIRM_CSB]);
	view->setCameraMask(_cameraMask,true);
	auto tipInfos = SINGLE_SHOP->getTipsInfo();
	std::string info = tipInfos[content];
	if (Changedcontent)
	{
		info = content;
	}
	auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
	t_content->setString(info);//内容

	std::string titlecontent=tipInfos[title];
	auto t_titleContent=dynamic_cast<Text*>(view->getChildByName("label_repair"));
	t_titleContent->setString(titlecontent);

	auto btn_yes = view->getChildByName<Button*>("button_confirm_yes");
	btn_yes->addTouchEventListener(CC_CALLBACK_2(UIInform::confirmButtonEvent,this));
	auto btn_no = view->getChildByName<Button*>("button_confirm_no");
	if (btn_no)
	{
		btn_no->addTouchEventListener(CC_CALLBACK_2(UIInform::menuCall_func,this));
	}
	this->setCameraMask(_cameraMask, true);

}
void UIInform::openConfirmYes(std::string name)
{
	m_index = C_VIEW_ERROR_CONFIRM_CSB;
	openView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
	auto view=getViewRoot(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
	auto tipInfos = SINGLE_SHOP->getTipsInfo();
	std::string info = tipInfos[name];
	auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
	t_content->setString(info);//内容
	auto btn_yes = view->getChildByName<Button*>("button_error_yes");
	btn_yes->addTouchEventListener(CC_CALLBACK_2(UIInform::confirmButtonEvent,this));
	this->setCameraMask(_cameraMask, true);
}
void UIInform::openViewAutoClose(std::string name, float time,const int64_t addScore)
{
 		m_index = C_VIEW_OTHER_TOAST_CSB;
		openView(INFORM_COCOS_RES[C_VIEW_OTHER_TOAST_CSB]);
		auto view=getViewRoot(INFORM_COCOS_RES[C_VIEW_OTHER_TOAST_CSB]);
		auto tipInfos = SINGLE_SHOP->getTipsInfo();
		std::string info = tipInfos[name];
		if (addScore>=0)
		{
			std::string new_vaule = StringUtils::format("%lld", addScore);
			std::string old_vaule = "[addScore]";
			repalce_all_ditinct(info, old_vaule, new_vaule);
		}
		auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
		t_content->setString(info);
		auto i_bg = dynamic_cast<ImageView*>(view->getChildByName("image_bg"));
		auto act_1 = FadeIn::create(time/4);
		auto act_2 = FadeOut::create(time/4);
		auto act_3 = FadeIn::create(time/4);
		auto act_4 = FadeOut::create(time/4);
		t_content->runAction(Sequence::create(act_1,DelayTime::create(time/2),act_2,nullptr));
		i_bg->runAction(Sequence::create(act_3,DelayTime::create(time/2),act_4,nullptr));
		this->scheduleOnce(schedule_selector(UIInform::closeAutoInform),time);
		this->setCameraMask(_cameraMask, true);
}
void UIInform::closeAutoInform(float time)
{
	closeView(INFORM_COCOS_RES[C_VIEW_OTHER_TOAST_CSB]);
}

void UIInform::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	Widget* w_btn = dynamic_cast<Widget*>(pSender);
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		closeView(INFORM_COCOS_RES[m_index]);
		buttonEventByName(w_btn,w_btn->getName());
	}
}

void UIInform::confirmButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	Widget* w_btn = dynamic_cast<Widget*>(pSender);
	if (Widget::TouchEventType::ENDED == TouchType)
	{	
		closeView(INFORM_COCOS_RES[m_index]);
		m_pParent->menuCall_func(w_btn,Widget::TouchEventType::ENDED);
	}
}
void UIInform::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
}
void UIInform::buttonEventByName(Widget* target,std::string name)
{
	//sound effect
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_confirm_no))
	{
		m_pParent->menuCall_func(target, Widget::TouchEventType::ENDED);
		return;
	}
}
