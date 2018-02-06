#include "UINewStart.h"
#include "Utils.h"
#include "UIManager.h"
#include "env.h"
#include "TVEventManager.h"
#include "ServerManager.h"
#include "UIHelper.h"

UINewStart::UINewStart()
{
	m_ConfigClientVersion = 0;
	m_DefaultServerIndex = 0;
}

UINewStart::~UINewStart()
{
}


void UINewStart::OnInit()
{

}

void UINewStart::OnShow()
{

	Vector<Node*> allBtn;
	UIHelper::GetWigetsByType(m_pRootWidget, WidgetType::_UIButton, allBtn);
	Widget::ccWidgetTouchCallback call = CC_CALLBACK_2(UINewStart::MenuCallback, this);
	UIHelper::ForeachSetCallbackForButton(allBtn, call);
	

	//////////////
	auto view_1 = m_pRootWidget;

	if (view_1){
		std::string versionName = Utils::getVersionName();
		std::string versionCode = Utils::getVersionCode();
		if (USE_BAKCUP_SERVER){
			versionName = versionName + "[beta]";
		}
		if (ENVIRONMENT == DEVELOPMENT){
			versionName = versionName + "[dev]";
		}
		versionName = versionName + "(" + versionCode + ")";
		auto label = Label::createWithTTF(versionName, "fonts/Roboto-Regular.ttf", 20);
		view_1->addChild(label);
		label->setVisible(true);
		label->setAnchorPoint(Point(0, 0));
		label->setPosition(Point(5, 5));
	}
	auto winSize = Director::getInstance()->getWinSize();
	auto panel_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(view_1, "panel_bg"));
	auto width_num = panel_bg->getContentSize().width - winSize.width;
	panel_bg->runAction(RepeatForever::create(Sequence::createWithTwoActions(MoveBy::create(width_num / 10, Vec2(-width_num, 0))
		, MoveBy::create(width_num / 10, Vec2(width_num, 0)))));
	auto panel_start = view_1->getChildByName<Widget*>("panel_start");
	auto panel_server = view_1->getChildByName<Widget*>("panel_customize_charactor");
	panel_start->setVisible(true);
	panel_server->setVisible(false);
	
	ServerManager::getInstance()->RequestServerList();

	//推荐区(1:北美,2:欧洲,,3:亚太)
	//..............选择服务器本地存储.............
	int serverIndex = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(SERVER_LOCAL_INDEX).c_str(), 1);
	std::string st_tip = StringUtils::format("TIP_START_SELECT_SERVER_%d", serverIndex);
	std::string st_tips = SINGLE_SHOP->getTipsInfo()[st_tip];
	SINGLE_SHOP->getTipsInfo()["TIP_START_SELECT_SERVER_TITLE"];
	auto t_server = panel_start->getChildByName<Text*>("text_server_1");
	t_server->setString(st_tips);
	auto i_server_status = panel_start->getChildByName<ImageView*>("image_online");
	i_server_status->setPositionX(t_server->getPositionX() - t_server->getBoundingBox().size.width - i_server_status->getBoundingBox().size.width);
	auto image_div = panel_start->getChildByName<ImageView*>("image_div");
	if (t_server->getBoundingBox().size.width > image_div->getBoundingBox().size.width)
	{
		image_div->setContentSize(Size(t_server->getBoundingBox().size.width, image_div->getBoundingBox().size.height));
	}
}

void UINewStart::OnUpdate()
{

}

void UINewStart::OnHide()
{

}


void UINewStart::MenuCallback(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	auto widget = dynamic_cast<Widget*>(pSender);
	auto name = widget->getName();

}
