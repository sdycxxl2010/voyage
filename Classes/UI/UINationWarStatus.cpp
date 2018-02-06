#include "UINationWarStatus.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
#include "UITips.h"
#include "TVSingle.h"
#include "UIInform.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "TVSceneLoader.h"
#include "UICommon.h"
#include "Utils.h"
#include "UINationWarRanking.h"

USING_NS_CC;
using namespace rapidjson;
using namespace cocostudio;
UINationWarStatus::UINationWarStatus()
{
	m_vWigetInfo.clear();
};
UINationWarStatus::~UINationWarStatus()
{
	m_vWigetInfo.clear();
	this->unschedule(schedule_selector(UINationWarStatus::updateAll));
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

UINationWarStatus*UINationWarStatus::createCountryWarStatus()
{
	auto warStatus = new UINationWarStatus;
	if (warStatus && warStatus->init())
	{
		warStatus->autorelease();
		return warStatus;
	}
	CC_SAFE_DELETE(warStatus);
	return nullptr;
}
bool UINationWarStatus::init()
{
	bool pRet = false;
	do 
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UINationWarStatus::onServerEvent, this), this);
		ProtocolThread::GetInstance()->getNationWarCityLost();
		initStatic(0);
		pRet = true;
	} while (0);
	return pRet;
}
void UINationWarStatus::initStatic(float f)
{
	openView(COUNTRY_WAR_RES[COUNTRY_WAR_STATUS_CSB], 1);
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_STATUS_CSB]);
	this->setCameraMask(4, true);
	view->setTouchEnabled(true);

	auto l_content = view->getChildByName<ListView*>("listview_content");
	auto p_score = dynamic_cast<Widget*>(l_content->getItem(0));
	//玩家分数
	auto t_score_num = p_score->getChildByName<Text*>("label_score_num");
	t_score_num->setString("0");
	//玩家银币数
	auto t_coin_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_coin_num"));
	t_coin_num->setString("0");
	auto t_icsailor_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_icsailor_num"));
	t_icsailor_num->setString("0");
	auto t_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_gold_num"));
	t_gold_num->setString("0");
	auto t_r_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_r_num"));
	t_r_num->setString("0");

	auto p_capital = dynamic_cast<Widget*>(l_content->getItem(2));
	auto p_city_own_1 = p_capital->getChildByName<Widget*>("panel_city_own_1");
	auto t_loss_num = p_city_own_1->getChildByName<Text*>("label_looted_num");
	auto i_silver_loss = p_city_own_1->getChildByName<ImageView*>("image_silver");
	//损失银币总数
	t_loss_num->setString("0");
	i_silver_loss->setPositionX(t_loss_num->getPositionX() - t_loss_num->getBoundingBox().size.width - i_silver_loss->getContentSize().width);

	auto p_city_enemy_1 = p_capital->getChildByName<Widget*>("panel_city_enemy_1");
	auto t_looted_num = p_city_enemy_1->getChildByName<Text*>("label_looted_num");
	auto i_silver_looted = p_city_enemy_1->getChildByName<ImageView*>("image_silver");
	//战利品银币总数
	t_looted_num->setString("0");
	i_silver_looted->setPositionX(t_looted_num->getPositionX() - t_looted_num->getBoundingBox().size.width - i_silver_looted->getContentSize().width);

	auto p_city_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_city_2"));
	p_city_2->setVisible(false);

	auto b_ranking = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_ship_parts"));
	b_ranking->addTouchEventListener(CC_CALLBACK_2(UINationWarStatus::menuCall_func, this));

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 3);
	addListViewBar(l_content, image_pulldown);

	this->schedule(schedule_selector(UINationWarStatus::updateAll), 3);
}
void UINationWarStatus::updateAll(const float fTime)
{
	ProtocolThread::GetInstance()->getNationWarCityLost();
}

void UINationWarStatus::flushWarScoreAndLossAndLooted(GetNationWarCityLostResult* result)
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_STATUS_CSB]);
	auto l_content = view->getChildByName<ListView*>("listview_content");
	auto p_score = dynamic_cast<Widget*>(l_content->getItem(0));
	//玩家分数
	auto t_score_num = p_score->getChildByName<Text*>("label_score_num");
	t_score_num->setString(StringUtils::format("%lld",result->my_score));
	//玩家银币数
	auto t_coin_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_coin_num"));
	t_coin_num->setString(StringUtils::format("%lld", result->rob_coins));
	auto t_icsailor_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_icsailor_num"));
	t_icsailor_num->setString(StringUtils::format("%lld", result->rob_sailor_coins));
	auto t_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_gold_num"));
	t_gold_num->setString(StringUtils::format("%lld", result->rob_golds));
	auto t_r_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_r_num"));
	t_r_num->setString(StringUtils::format("%lld", result->rob_prestige));

	if (result->n_my_city_lost > 0 || result->n_enemy_city_loss > 0)
	{
		auto p_capital = dynamic_cast<Widget*>(l_content->getItem(2));
		auto p_city_own_1 = p_capital->getChildByName<Widget*>("panel_city_own_1");
		//损失银币总数
		auto t_loss_num = p_city_own_1->getChildByName<Text*>("label_looted_num");
		auto i_silver_loss = p_city_own_1->getChildByName<ImageView*>("image_silver");

		auto p_city_enemy_1 = p_capital->getChildByName<Widget*>("panel_city_enemy_1");
		//战利品银币总数
		auto t_looted_num = p_city_enemy_1->getChildByName<Text*>("label_looted_num");
		auto i_silver_looted = p_city_enemy_1->getChildByName<ImageView*>("image_silver");

		auto p_city_2 = dynamic_cast<Widget*>(l_content->getItem(3));
		p_city_2->setVisible(true);
		auto p_city_own_2 = p_city_2->getChildByName<Widget*>("panel_city_own_2");
		auto p_city_enemy_2 = p_city_2->getChildByName<Widget*>("panel_city_enemy_2");

		std::string fallenCityName;
		int coinNum = 0;
		if (result->n_my_city_lost > 0)
		{
			p_city_own_2->setVisible(true);
			auto t_loss_city_name = p_city_own_2->getChildByName<Text*>("label_looted");
			auto t_loss_city_coin = p_city_own_2->getChildByName<Text*>("label_looted_num");
			auto i_silver_loss = p_city_own_2->getChildByName<ImageView*>("image_silver");

			t_loss_city_name->setString(SINGLE_SHOP->getCitiesInfo()[result->my_city_lost[0]->city_id].name);
			t_loss_city_coin->setString(StringUtils::format("%lld", result->my_city_lost[0]->robbed_coins));
			i_silver_loss->setPositionX(t_loss_city_coin->getPositionX() - t_loss_city_coin->getBoundingBox().size.width - i_silver_loss->getContentSize().width);
			for (int i = 0; i < result->n_my_city_lost; i++)
			{
				coinNum += result->my_city_lost[i]->robbed_coins;
			}
		}
		else
		{
			p_city_own_2->setVisible(false);
			coinNum = 0;
		}
		t_loss_num->setString(StringUtils::format("%d",coinNum));
		i_silver_loss->setPositionX(t_loss_num->getPositionX() - t_loss_num->getBoundingBox().size.width - i_silver_loss->getContentSize().width);

		if (result->n_enemy_city_loss > 0)
		{
			p_city_enemy_2->setVisible(true);
			auto i_flag = p_city_enemy_2->getChildByName<ImageView*>("image_flag");
			auto t_looted_city_name = p_city_enemy_2->getChildByName<Text*>("label_looted");
			auto t_looted_city_coin = p_city_enemy_2->getChildByName<Text*>("label_looted_num");
			auto i_silver_looted = p_city_enemy_2->getChildByName<ImageView*>("image_silver");

			i_flag->loadTexture(getCountryIconPath(result->enemy_city_loss[0]->nation_id));
			fallenCityName = SINGLE_SHOP->getCitiesInfo()[result->enemy_city_loss[0]->city_id].name;
			fallenCityName += SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_STATUS_CITYNAME_PLUS_FALLEN"];
			t_looted_city_name->setString(fallenCityName);
			t_looted_city_coin->setString(StringUtils::format("%lld", result->enemy_city_loss[0]->robbed_coins));
			i_silver_looted->setPositionX(t_looted_city_coin->getPositionX() - t_looted_city_coin->getBoundingBox().size.width - i_silver_looted->getContentSize().width);
			for (int i = 0; i < result->n_enemy_city_loss; i++)
			{
				coinNum += result->enemy_city_loss[i]->robbed_coins;
			}
		}
		else
		{
			p_city_enemy_2->setVisible(false);
			coinNum = 0;
		}
		t_looted_num->setString(StringUtils::format("%d", coinNum));
		i_silver_looted->setPositionX(t_looted_num->getPositionX() - t_looted_num->getBoundingBox().size.width - i_silver_looted->getContentSize().width);

		for (int i = 0; i < m_vWigetInfo.size();i++)
		{
			m_vWigetInfo[i]->removeFromParentAndCleanup(true);
		}
		m_vWigetInfo.clear();
		if (result->n_my_city_lost > 1 && result->n_my_city_lost >= result->n_enemy_city_loss)
		{
			for (int i = 1; i < result->n_my_city_lost; i++)
			{
				auto p_city_clone = p_city_2->clone();
				l_content->pushBackCustomItem(p_city_clone);
				m_vWigetInfo.push_back(p_city_clone);
				p_city_clone->setCameraMask(4);
				auto p_city_own_2 = p_city_clone->getChildByName<Widget*>("panel_city_own_2");
				auto p_city_enemy_2 = p_city_clone->getChildByName<Widget*>("panel_city_enemy_2");
				p_city_own_2->setVisible(true);
				p_city_enemy_2->setVisible(true);

				auto t_loss_city_name = p_city_own_2->getChildByName<Text*>("label_looted");
				auto t_loss_city_coin = p_city_own_2->getChildByName<Text*>("label_looted_num");
				auto i_silver_loss = p_city_own_2->getChildByName<ImageView*>("image_silver");

				t_loss_city_name->setString(SINGLE_SHOP->getCitiesInfo()[result->my_city_lost[i]->city_id].name);
				t_loss_city_coin->setString(StringUtils::format("%lld",result->my_city_lost[i]->robbed_coins));
				i_silver_loss->setPositionX(t_loss_city_coin->getPositionX() - t_loss_city_coin->getBoundingBox().size.width - i_silver_loss->getContentSize().width);
				if (i >= result->n_enemy_city_loss)
				{
					p_city_enemy_2->setVisible(false);
				}
				else
				{
					auto i_flag = p_city_enemy_2->getChildByName<ImageView*>("image_flag");
					auto t_looted_city_name = p_city_enemy_2->getChildByName<Text*>("label_looted");
					auto t_looted_city_coin = p_city_enemy_2->getChildByName<Text*>("label_looted_num");
					auto i_silver_looted = p_city_enemy_2->getChildByName<ImageView*>("image_silver");

					i_flag->loadTexture(getCountryIconPath(result->enemy_city_loss[i]->nation_id));
					fallenCityName = SINGLE_SHOP->getCitiesInfo()[result->enemy_city_loss[i]->city_id].name;
					fallenCityName += SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_STATUS_CITYNAME_PLUS_FALLEN"];
					t_looted_city_name->setString(fallenCityName);
					t_looted_city_coin->setString(StringUtils::format("%lld", result->enemy_city_loss[i]->robbed_coins));
					i_silver_looted->setPositionX(t_looted_city_coin->getPositionX() - t_looted_city_coin->getBoundingBox().size.width - i_silver_looted->getContentSize().width);
				}
			}
		}
		else if (result->n_enemy_city_loss > 1 && result->n_enemy_city_loss >= result->n_my_city_lost)
		{
			for (int i = 1; i < result->n_enemy_city_loss; i++)
			{
				auto p_city_clone = p_city_2->clone();
				l_content->pushBackCustomItem(p_city_clone);
				m_vWigetInfo.push_back(p_city_clone);
				p_city_clone->setCameraMask(4);
				auto p_city_own_2 = p_city_clone->getChildByName<Widget*>("panel_city_own_2");
				auto p_city_enemy_2 = p_city_clone->getChildByName<Widget*>("panel_city_enemy_2");
				p_city_own_2->setVisible(true);
				p_city_enemy_2->setVisible(true);

				auto i_flag = p_city_enemy_2->getChildByName<ImageView*>("image_flag");
				auto t_looted_city_name = p_city_enemy_2->getChildByName<Text*>("label_looted");
				auto t_looted_city_coin = p_city_enemy_2->getChildByName<Text*>("label_looted_num");
				auto i_silver_looted = p_city_enemy_2->getChildByName<ImageView*>("image_silver");

				i_flag->loadTexture(getCountryIconPath(result->enemy_city_loss[i]->nation_id));
				fallenCityName = SINGLE_SHOP->getCitiesInfo()[result->enemy_city_loss[i]->city_id].name;
				fallenCityName += SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_STATUS_CITYNAME_PLUS_FALLEN"];
				t_looted_city_name->setString(fallenCityName);
				t_looted_city_coin->setString(StringUtils::format("%lld", result->enemy_city_loss[i]->robbed_coins));
				i_silver_looted->setPositionX(t_looted_city_coin->getPositionX() - t_looted_city_coin->getBoundingBox().size.width - i_silver_looted->getContentSize().width);

				if (i >= result->n_my_city_lost)
				{
					p_city_own_2->setVisible(false);
				}
				else
				{
					auto t_loss_city_name = p_city_own_2->getChildByName<Text*>("label_looted");
					auto t_loss_city_coin = p_city_own_2->getChildByName<Text*>("label_looted_num");
					auto i_silver_loss = p_city_own_2->getChildByName<ImageView*>("image_silver");

					t_loss_city_name->setString(SINGLE_SHOP->getCitiesInfo()[result->my_city_lost[i]->city_id].name);
					t_loss_city_coin->setString(StringUtils::format("%lld", result->my_city_lost[i]->robbed_coins));
					i_silver_loss->setPositionX(t_loss_city_coin->getPositionX() - t_loss_city_coin->getBoundingBox().size.width - i_silver_loss->getContentSize().width);
				}
			}
		}
	}
}
void UINationWarStatus::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto button = dynamic_cast<Widget*>(pSender);
		std::string name = button->getName();
		buttonEvent(button, name);
	}
}
void UINationWarStatus::buttonEvent(Widget* target, std::string name, bool isRefresh)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

	if (isButton(button_close))
	{
		closeView();
		this->removeFromParentAndCleanup(true);
		return;
	}
	//国战排行榜
	if (isButton(button_ship_parts))
	{
		auto layer = UINationWarRanking::createCountryWarRanking();
		this->addChild(layer, 3);
		return;
	}

}
void UINationWarStatus::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	switch (msgType)
	{
	// 战斗中城市被掠夺的情况
	case PROTO_TYPE_GetNationWarCityLostResult:
	{
		GetNationWarCityLostResult *result = (GetNationWarCityLostResult*)message;
		if (result->failed == 0)
		{
			flushWarScoreAndLossAndLooted(result);
		}
		else
		{

		}

		break;
	}
	default:
		break;
	}
}

