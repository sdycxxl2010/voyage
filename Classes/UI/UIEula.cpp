#include "UIEula.h"
#include "Utils.h"
#include "env.h"

UIEula::UIEula()
{
}

UIEula::~UIEula()
{
}


void UIEula::OnInit()
{

}

void UIEula::OnShow()
{
	Widget* view = m_pRootWidget;
	auto btn_close = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_agreement_close"));
	btn_close->setVisible(false);
	auto image_content_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_content_bg"));
	auto sizeContent = image_content_bg->getContentSize();
	//m_viewIndex = VIEW_EULA;
	//webViewÍøÒ³¿Ø¼þ
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{

		cocos2d::experimental::ui::WebView *siteWeb = cocos2d::experimental::ui::WebView::create();
		image_content_bg->addChild(siteWeb);
		siteWeb->setTag(100);
		siteWeb->setLocalZOrder(-1);
		if (ENVIRONMENT == PRODUCTION){
			siteWeb->loadURL("http://pubapi.voyage.piistudio.com/eula.php");
		}
		else{
			siteWeb->loadURL("http://pubapi-dev.voyage.piistudio.com/eula.php");
		}
		siteWeb->setAnchorPoint(Vec2(0.5, 0.5));
		siteWeb->setContentSize(sizeContent);
		siteWeb->setPosition(Vec2(image_content_bg->getContentSize().width / 2, image_content_bg->getContentSize().height / 2));
		siteWeb->setScalesPageToFit(true);
		siteWeb->setOnShouldStartLoading([](cocos2d::experimental::ui::WebView*sender, const std::string &url){
			return true;
		});
		siteWeb->setOnDidFinishLoading([](cocos2d::experimental::ui::WebView *sender, const std::string &url){
		});
		siteWeb->setOnDidFailLoading([](cocos2d::experimental::ui::WebView *sender, const std::string &url){
		});
	}
#endif
}

void UIEula::OnUpdate()
{

}

void UIEula::OnHide()
{

}