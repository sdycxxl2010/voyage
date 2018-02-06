#include "WarManage.h"
#include "UINationWarMap.h"
#include "UINationWarShip.h"
#include "UINationWarHUD.h"
#include "EffectManage.h"
#include "TVSceneLoader.h"
#include "UIInform.h"
#include "UICommon.h"
#include "UISailManage.h"
#include "UIWorldNotify.h"

WarManage::WarManage()
{
	m_eUIType = UI_COUNTRY_WAR;
	m_pWarMap = nullptr;
	m_pWarShip = nullptr;
	m_pWarUI = nullptr;
	m_pEffectManage = nullptr;
	m_vWarShip.clear();
	m_pResult = nullptr;
	m_bIsWarStop = false;
	m_pBattleTurnResult = nullptr;
	m_pLayer = nullptr;
	m_nBattleTureTime = 0;
}

WarManage::~WarManage()
{
	this->unschedule(schedule_selector(WarManage::updateBySecond));
	delete m_pEffectManage;
	unregisterCallBack();
}

WarManage* WarManage::createWarManage()
{
	WarManage* wm = new WarManage;
	if (wm && wm->init())
	{
		wm->setName("WarManage");
		wm->autorelease();
		return wm;
	}
	CC_SAFE_DELETE(wm);
	return nullptr;
}

bool WarManage::init()
{
	bool pRet = false;
	do
	{
		SINGLE_HERO->m_heroIsOnsea = false;
		registerCallBack();
		ProtocolThread::GetInstance()->startAttackCity(SINGLE_HERO->m_nAttackCityId, UILoadingIndicator::create(this, m_eUIType));
		playAudio();
		pRet = true;
	} while (0);

	return pRet;
}

void WarManage::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* widget = dynamic_cast<Widget*>(pSender);
		std::string name = widget->getName();
		if (isButton(button_error_yes))
		{
			return;
		}
	}
}

void WarManage::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_StartAttackCityResult:
		{
			auto result = (StartAttackCityResult *)message;
			if (result->failed == 0)
			{
				m_pResult = result;
				initf();
				this->schedule(schedule_selector(WarManage::updateBySecond), 1);
			}
			else
			{
				auto sc = Scene::create();
				auto mps = UISailManage::create(SINGLE_HERO->shippos, 1);
				mps->setTag(MAP_TAG + 100);
				sc->addChild(mps);
				Director::getInstance()->replaceScene(sc);
			}
			break;
		}
	case PROTO_TYPE_NationWarBattleTurnResult:
		{
			auto result = (NationWarBattleTurnResult *)message;
			if (result->failed == 0 || result->failed == 3)
			{
				//战斗结束
				if (m_bIsWarStop)
				{
					return;
				}
				m_nBattleTureTime = 0;
				auto view = m_pWarUI->getViewRoot(COMMOM_COCOS_RES[C_VIEW_NATION_WAR_LOADING]);
				if (view)
				{
					m_pWarUI->closeView(COMMOM_COCOS_RES[C_VIEW_NATION_WAR_LOADING]);
					m_pWarShip->setShipConnonCD(true);
				} 

				int my_index = 0;
				//处理玩家退出
				for (size_t i = 0; i < m_vWarShip.size(); i++)
				{
					bool is_find = false;
					for (size_t j = 0; j < result->n_status; j++)
					{
						auto shipInfo = result->status[j];
						if (m_vWarShip.at(i)->getShipFleetInfo()->cid == shipInfo->cid)
						{
							if (i == 0)
							{
								my_index = j;
							}
							is_find = true;
							break;
						}
					}
					if (!is_find && i != 0)
					{
						m_vWarShip.at(i)->shipDied();
					}
				}
				
				auto ship_position = SINGLE_SHOP->getCityWarPositionInfo().find(SINGLE_HERO->m_nAttackCityId)->second.position;
				
				//记录该位置是否有舰队
				int p[5] = {0,0,0,0,0};
				for (size_t i = 0; i < m_vWarShip.size(); i++)
				{
					p[m_vWarShip.at(i)->getShipPosition()] = 1;
				}
	
				int s_index = 0;
				switch (result->n_status - my_index)
				{
				case 1:
					s_index = my_index - 4;
					break;
				case 2:
					s_index = my_index - 3;
					break;
				case 3:
					s_index = my_index - 2;
					break;
				case 4:
					s_index = my_index - 1;
					break;
				case 5:
					s_index = my_index - 0;
					break;
				default:
					s_index = my_index;
					break;
				}
				if (s_index < 0)
				{
					s_index = 0;
				}

				for (size_t i = s_index; i < result->n_status && m_vWarShip.size() < 5; i++)
				{
					auto shipInfo = result->status[i];
					//是否已有影子单位
					bool is_find = false;

					for (size_t j = 0; j < m_vWarShip.size(); j++)
					{
						if (shipInfo->cid == m_vWarShip.at(j)->getShipFleetInfo()->cid)
						{
							is_find = true;
							break;
						}
					}
					//创建影子单位
					if (!is_find && shipInfo->n_ships > 0)
					{
						int position = 1;
						for (size_t k = 0; k < 5; k++)
						{
							if (p[k] == 0)
							{
								p[k] = 1;
								position = k;
								break;
							}
						}
						auto warShip = UINationWarShip::createWarShip(this, position, GHOST_SHIP_FLEET, m_vWarShip.size());
						warShip->setPosition(Vec2(ship_position.at(position).x, ship_position.at(position).y));
						m_pWarMap->addChild(warShip, 1);
						m_vWarShip.push_back(warShip);
						warShip->setShipFleetInfo(shipInfo);
						warShip->setShipIcon(shipInfo->ships[0]->ship_id, ship_position.at(position).r);
					}
				}
				
		
				for (size_t i = 0; i < m_vWarShip.size(); i++)
				{
					auto warShip = getWarShip(i);
					for (size_t j = 0; j < result->n_status; j++)
					{
						if (warShip->getShipFleetInfo()->cid == result->status[j]->cid)
						{
							warShip->setShipFleetInfo(result->status[j]);
							warShip->updateBySecond(5 * (warShip->getShipFleetIndex() + 1));
							if (warShip->getShipFleetType() == MY_SHIP_FLEET)
							{
								m_pWarUI->setCityAndShipFleetInfo(result->city_status, result->status[j]);
							}
						}
					}
				}
				m_pWarMap->updateBySecond((m_vWarShip.size() + 1) * 5);


				if (m_pBattleTurnResult)
				{
					int offset_hp = m_pBattleTurnResult->city_status->start_hp - result->city_status->start_hp;
					if (offset_hp < 0)
					{
						m_pWarUI->updateCityDuable(false);
					}
				}
				m_pBattleTurnResult = result;
			}
			//国战结束(时间到了)
			else if (result->failed == 2)
			{
				m_pWarUI->openTipsView(1);
			}
			//服务器认为自己已经挂了
			else if (result->failed == 3)
			{
				//m_pWarUI->openTipsView(2);
			}
			break;
		}
	case PROTO_TYPE_EndAttackCityResult:
		{
			auto result = (EndAttackCityResult *)message;
			//国战主动退出结算
			if (result->failed == 0)
			{
				if (3 == result->reason)
				{
					button_callBack();
				}
				else
				{
					auto sc = Scene::create();
					auto mps = UISailManage::create(SINGLE_HERO->shippos, 1);
					mps->setTag(MAP_TAG + 100);
					sc->addChild(mps);
					Director::getInstance()->replaceScene(sc);
				}
			}
			//国战倒计时没有为0
			else if (result->failed == 1)
			{
				auto sc = Scene::create();
				auto mps = UISailManage::create(SINGLE_HERO->shippos, 1);
				mps->setTag(MAP_TAG + 100);
				sc->addChild(mps);
				Director::getInstance()->replaceScene(sc);
			}
			break;
		}
	case PROTO_TYPE_GetCurrentCityDataResult:
		{
			GetCurrentCityDataResult *result = (GetCurrentCityDataResult*)message;
			if (result->failed == 0)
			{
				button_callBack();
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_SERVER_DATA_FIAL");
			}
			break;
		}
	case PROTO_TYPE_GetUserInfoByIdResult:
		{
			GetUserInfoByIdResult *result = (GetUserInfoByIdResult *)message;
			if (result->failed == 0)
			{
				m_pWarUI->openUserView(result);
			}
			break;
		}
	case PROTO_TYPE_FriendsOperationResult:
		{
			FriendsOperationResult *pFriendsResult = (FriendsOperationResult*)message;
			if (pFriendsResult->failed == 0)
			{
				if (pFriendsResult->actioncode == 4)
				{
					UIInform::getInstance()->openInformView(m_pWarUI);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADDREFUSAL_SUCCESS");
				}
				else if (pFriendsResult->actioncode == 3)
				{
					UIInform::getInstance()->openInformView(m_pWarUI);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADDED_FRIEND_SUCCESS");
				}
				else if (pFriendsResult->actioncode == 2)
				{
					UIInform::getInstance()->openInformView(m_pWarUI);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_BLACK_LIST_SUCCESS");
				}
				else if (pFriendsResult->actioncode == 1)
				{
					UIInform::getInstance()->openInformView(m_pWarUI);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_DETELE_FRIEND_SUCCESS");
				}
				else if (pFriendsResult->actioncode == 0) //加为好友
				{
					UIInform::getInstance()->openInformView(m_pWarUI);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_FRIEND_SUCCESS");
				}
				else if (pFriendsResult->actioncode == 5)//移除拉黑玩家
				{
					UIInform::getInstance()->openInformView(m_pWarUI);
					UIInform::getInstance()->openViewAutoClose("TIP_REMOVE_BLOCK_SUCCESS");
				}
			}
			else if (pFriendsResult->failed == 105)
			{
				UIInform::getInstance()->openInformView(m_pWarUI);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_REQUEST_ADDFRIENDED");
			}
			else
			{
				UIInform::getInstance()->openInformView(m_pWarUI);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_ADD_FRIENDED_FAIL");
			}
			break;
		}
	case PROTO_TYPE_InviteUserToGuildResult:
		{
			InviteUserToGuildResult *result = (InviteUserToGuildResult *)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(m_pWarUI);
				UIInform::getInstance()->openViewAutoClose("TIP_GULID_INVINT_SUCCESS");
			}
			else if (result->failed == 11)
			{
				//邀请的对象不是同一势力
				UIInform::getInstance()->openInformView(m_pWarUI);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITED_FAILED_NOT_SAME_COUNTRY_CONTENT");
			}
			else
			{
				UIInform::getInstance()->openInformView(m_pWarUI);
				UIInform::getInstance()->openConfirmYes("TIP_GULID_INVINT_FAIL");
			}
			break;
		}
	default:
		break;
	}
}

void WarManage::initf()
{
	m_pEffectManage = EffectManage::createEffect();
	m_pWarMap = UINationWarMap::createWarMapLayer(this);
	this->addChild(m_pWarMap, 1);

	m_pLayer = Layer::create();
	m_pWarMap->addChild(m_pLayer, 99);


	auto ship_position = SINGLE_SHOP->getCityWarPositionInfo().find(SINGLE_HERO->m_nAttackCityId)->second.position;
	m_pWarShip = UINationWarShip::createWarShip(this, 0, MY_SHIP_FLEET, 0);
	m_pWarShip->setPosition(Vec2(ship_position.at(0).x, ship_position.at(0).y));
	m_pWarMap->addChild(m_pWarShip, 1);
	m_vWarShip.push_back(m_pWarShip);
	m_pWarShip->setShipFleetInfo(m_pResult->status);
	m_pWarShip->setShipIcon(m_pResult->status->ships[0]->ship_id, ship_position.at(0).r);

	m_pWarUI = UINationWarHUD::createWarUI(this, m_pResult);
	this->addChild(m_pWarUI, 100);
	//检查一下城市是否死亡
	m_pWarUI->cityDied();
}

UINationWarMap *WarManage::getWarMapLayer()
{
	return m_pWarMap;
}

UINationWarHUD *WarManage::getWarUI()
{
	return m_pWarUI;
}

UINationWarShip	*WarManage::getWarShip(int tag)
{
	if (m_vWarShip.size() > tag)
	{
		return m_vWarShip.at(tag);
	}
	else
	{
		return m_pWarShip;
	}
}

EffectManage *WarManage::getEffectManage()
{
	return m_pEffectManage;
}

void WarManage::countryWarStop()
{
	for (size_t i = 0; i < m_vWarShip.size(); i++)
	{
		m_vWarShip.at(i)->countryWarStop();
	}
	m_pWarMap->countryWarStop();
	m_pWarUI->countryWarStop();
	m_bIsWarStop = true;
	this->unschedule(schedule_selector(WarManage::updateBySecond));
	auto curScene = Director::getInstance()->getRunningScene();
	auto sysInfo = dynamic_cast<UIWorldNotify*>(curScene->getChildByName("SysInfo"));
	if (sysInfo)
	{
		for (size_t i = 0; i < SINGLE_HERO->m_vSystemInfo.size(); i++)
		{
			auto &item = SINGLE_HERO->m_vSystemInfo.at(i);
			item.isWait = false;
		}
		sysInfo->openSysInfoView(0);
	}
}

Layer* WarManage::getLayer()
{
	return m_pLayer;
}

void WarManage::updateBySecond(const float fTime)
{
	m_nBattleTureTime++;
	if (m_nBattleTureTime > 45)
	{
		countryWarStop();
		m_pWarUI->openTipsView(4);
	}
}

NationWarBattleTurnResult * WarManage::getBattleTurnResult()
{
	return m_pBattleTurnResult;
}