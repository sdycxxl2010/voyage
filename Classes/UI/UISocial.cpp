#include "UISocial.h"
#include "UIEmail.h"
#include "UICommon.h"
#include "UIInform.h"
#include "Utils.h"
#include "WarManage.h"
#include "UINationWarHUD.h"
#include "ModeLayerManger.h"
UISocial* UISocial::m_pSocialLayer = nullptr;
//最近发言的玩家个数限制
#define CHAT_LIST_PLAYER 20
#define CHAT_TIME_INTERVAL 60
#define GM_IOCN  "res/country_icon/gm.png"

UISocial::UISocial()
{
	reset();
	ProtocolThread::GetInstance()->registerChatMessageCallback(CC_CALLBACK_1(UISocial::onChatMessage, this));
	ProtocolThread::GetInstance()->listChannelUsers(m_nChannelType);
}

void UISocial::reset()
{
	m_vPrivateName.clear();
	m_pTempButton = nullptr;
	m_pPreChannelBtn = nullptr;
	m_nChannelType = 1;
	n_type = 0;
	LanguageType nType = LanguageType(Utils::getLanguage());
	if (nType == cocos2d::LanguageType::ENGLISH)
	{
		m_nLastChannelType = 1;
	}
	else
	{
		m_nLastChannelType = 0;
	}
	m_pMainButton = nullptr;
	m_nChatIndex = 0;
	m_eConfirmIndex = CONFIRM_INDEX_ACCEPT_ADDFRIEND;
	m_pFirendButton = nullptr;
	m_vBlackListName.clear();
	sendIntervalTime = 0;
	m_pFriendsResult = nullptr;
	m_bisFriend = false;
	n_type = 0;
	m_IsSendPrivate = false;
	m_IsMeSendToPrivate = false;
	m_nFirstSendToPrivate = STATE_NONE;
	for (size_t i = 0; i < 5; i++)
	{
		m_vHeroListMsg[i].clear();
		m_vTempHeroListMsg[i].clear();
	}
	m_vHeroMsg.clear();
	m_bSaveHeromsg = false;
	m_nLastTime.clear();
	for (size_t i = 0; i < 5; i++)
	{
		m_nLastTime.push_back(-1);
	}
	m_pBlackListResult = nullptr;
	m_pFriendsReqListResult = nullptr;

	m_nForGiftIndex = 0;
	m_ForReqIndex = 0;
	m_ForBlockIndex = 0;
	m_ForFriendIndex = 0;
	m_bIsFriendList = false;
	m_bIsSendGift = false;
	m_bIsReqList = false;
	m_bIsBlockList = false;
}

UISocial::~UISocial()
{
	ProtocolThread::GetInstance()->unregisterChatMessageCallback();
}

void UISocial::onEnter()
{
	UIBasicLayer::onEnter();
}

void UISocial::onExit()
{
	UIBasicLayer::onExit();
}

void UISocial::openSocialLayer()
{
	auto currentScene = Director::getInstance()->getRunningScene();
	UIInform::getInstance()->closeAutoInform(0);
	this->removeFromParentAndCleanup(true);
	auto warmange = dynamic_cast<WarManage*>(_director->getRunningScene()->getChildByName("WarManage"));
	if (warmange != nullptr)
	{
		auto * warUi = warmange->getWarUI();
		warUi->addChild(this, 1000);
	}
	else
	{
		currentScene->addChild(this, 10);
	}
	registerCallBack();
	this->scheduleUpdate();
}

void UISocial::onChatMessage(char*chatMsg)
{
	const char *listPre = ":voyagechat 353 ";
	int lenListPre = strlen(listPre);
	if (strncmp(chatMsg, listPre, lenListPre) == 0)
	{
		char *pc = chatMsg + lenListPre;
		int len = strlen(pc);
		char *newMsg = new char[len + 1];
		int i = 0;
		while (i < len && pc[i] != '\r')
		{
			newMsg[i] = pc[i];
			i++;
		}
		newMsg[i] = 0;
		m_vHeroMsg.clear();
		rapidjson::Document root;
		if (root.Parse<0>(newMsg).HasParseError()){
			delete newMsg;
			return;
		}

		auto channel = DictionaryHelper::getInstance()->getStringValue_json(root, "channel");
		auto& result = DictionaryHelper::getInstance()->getSubDictionary_json(root, "result");

		for (int i = 0; i > -1; i++)
		{
			if (DictionaryHelper::getInstance()->checkObjectExist_json(result, i))
			{
				auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(result, i);
				auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
				auto icon = DictionaryHelper::getInstance()->getIntValue_json(counts, "icon");
				auto cid = DictionaryHelper::getInstance()->getIntValue_json(counts, "cid");
				auto nation = DictionaryHelper::getInstance()->getIntValue_json(counts, "nation");
				auto is_master = DictionaryHelper::getInstance()->getIntValue_json(counts, "is_master");
				if (is_master == 1)
				{
					nation = -1;
				}
				CHATMSG heroMsg = { name, channel, icon, cid, nation };
				m_vHeroMsg.push_back(heroMsg);
			}
			else
			{
				break;
			}
		}
		delete[] newMsg;
		//私聊时当对方发来消息时，右边聊表显示对方的名字

		if (m_nChannelType >= 5)
		{
			m_IsMeSendToPrivate = false;
			m_IsSendPrivate = false;
			auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
			if (!panel_chat)
			{
				return;
			}
			auto l_chat = panel_chat->getChildByName<ListView*>("listview_chat");
			auto counts = l_chat->getChildrenCount();
			if (counts <= 0)
			{
				m_nFirstSendToPrivate = STATE_FIRST;
			}
			else
			{
				m_nFirstSendToPrivate = STATE_AFTER;
			}
		}
		else
		{
			//匹配时更新更新本地数据
			if (strcmp(channel, "#voyage_zh") == 0 && m_nChannelType == 0)
			{
				m_vTempHeroListMsg[m_nChannelType].clear();
				m_vTempHeroListMsg[m_nChannelType] = m_vHeroMsg;
				if (!m_bSaveHeromsg)
				{
					m_nChannelType = 2;
					ProtocolThread::GetInstance()->listChannelUsers(m_nChannelType);
				}
			}
			else if (strcmp(channel, "#voyage") == 0 && m_nChannelType == 1)
			{
				m_vTempHeroListMsg[m_nChannelType].clear();
				m_vTempHeroListMsg[m_nChannelType] = m_vHeroMsg;
				if (!m_bSaveHeromsg)
				{
					m_nChannelType = 0;
					ProtocolThread::GetInstance()->listChannelUsers(m_nChannelType);
				}
			}
			else if ((strcmp(channel, "#England") == 0 || strcmp(channel, "#Portugal") == 0 || strcmp(channel, "#Spain") == 0
				|| strcmp(channel, "#Netherlands") == 0 || strcmp(channel, "#Genoa") == 0) && m_nChannelType == 2)
			{
				m_vTempHeroListMsg[m_nChannelType].clear();
				m_vTempHeroListMsg[m_nChannelType] = m_vHeroMsg;
				if (!m_bSaveHeromsg)
				{
					m_nChannelType = 3;
					ProtocolThread::GetInstance()->listChannelUsers(m_nChannelType);
				}
			}
			else if ((strncmp(channel, "#CITY_", strlen("#CITY_")) == 0 || strncmp(channel, "#SEA_", strlen("#SEA_")) == 0) && m_nChannelType == 3)
			{
				m_vTempHeroListMsg[m_nChannelType].clear();
				m_vTempHeroListMsg[m_nChannelType] = m_vHeroMsg;
				if (!m_bSaveHeromsg)
				{
					if (SINGLE_HERO->m_iGuildId)
					{
						m_nChannelType = 4;
						ProtocolThread::GetInstance()->listChannelUsers(m_nChannelType);
					}
					else
					{
						m_bSaveHeromsg = true;
						m_nChannelType = -1;
					}
				}
			}
			else if (strncmp(channel, "#GUILD_", strlen("#GUILD_")) == 0 && m_nChannelType == 4)
			{
				m_vTempHeroListMsg[m_nChannelType].clear();
				m_vTempHeroListMsg[m_nChannelType] = m_vHeroMsg;
				if (!m_bSaveHeromsg)
				{
					m_bSaveHeromsg = true;
					m_nChannelType = -1;
				}
			}

			if (m_nChannelType >= 0 && m_nChannelType < 5)
			{
				bool isfind = false;
				for (size_t i = 0; i < m_vTempHeroListMsg[m_nChannelType].size(); i++)
				{
					isfind = false;
					for (size_t j = 0; j < m_vHeroListMsg[m_nChannelType].size(); j++)
					{
						if (m_vTempHeroListMsg[m_nChannelType].at(i).name == m_vHeroListMsg[m_nChannelType].at(j).name)
						{
							isfind = true;
						}

					}
					if (!isfind)
					{
						m_vHeroListMsg[m_nChannelType].push_back(m_vTempHeroListMsg[m_nChannelType].at(i));
					}
				}


				//第一个是自己
				isfind = false;
				size_t i = 0;
				for (; i < m_vHeroListMsg[m_nChannelType].size(); i++)
				{
					if (m_vHeroListMsg[m_nChannelType].at(i).cid == SINGLE_HERO->m_iID)
					{
						isfind = true;
						break;
					}
				}
				int nNation;
				if (SINGLE_HERO->m_bGameMaster)
				{
					nNation = -1;
				}
				else
				{
					nNation = SINGLE_HERO->m_iNation;
				}
				CHATMSG mychatmsg = { SINGLE_HERO->m_sName, channel, SINGLE_HERO->m_iIconidx, SINGLE_HERO->m_iID, nNation };
				if (isfind)
				{
					m_vHeroListMsg[m_nChannelType].erase(m_vHeroListMsg[m_nChannelType].begin() + i);
				}
				m_vHeroListMsg[m_nChannelType].insert(m_vHeroListMsg[m_nChannelType].begin(), mychatmsg);
			}
		}
		flushChatFirendList();
	}
	else
	{
		std::string receiver;
		std::string name;
		std::string content;
		int nation;
		int sender_id;
		std::string channel;
		int icon;

		rapidjson::Document root;
		if (root.Parse<0>(chatMsg).HasParseError()){
			return;
		}
		if (!DictionaryHelper::getInstance()->checkObjectExist_json(root, "receiver"))
		{
			return;
		}
		receiver = DictionaryHelper::getInstance()->getStringValue_json(root, "receiver");
		name = DictionaryHelper::getInstance()->getStringValue_json(root, "sender");

		for (int j = 0; j < m_vBlackListName.size(); j++)
		{
			if (strcmp(name.data(), m_vBlackListName[j].data()) == 0)
			{
				return;
			}
		}
		sender_id = DictionaryHelper::getInstance()->getIntValue_json(root, "sender_id");
		nation = DictionaryHelper::getInstance()->getIntValue_json(root, "nation");
		icon = DictionaryHelper::getInstance()->getIntValue_json(root, "icon");
		auto is_master = DictionaryHelper::getInstance()->getIntValue_json(root, "is_master");
		auto msg_type = DictionaryHelper::getInstance()->getIntValue_json(root, "msg_type");
		if (is_master == 1)
		{
			nation = -1;
		}
		//服务器定义msg_type 500:正常消息 502:世界公告 503:工会公告
		std::string content_zh = "";
		std::string content_el = "";
		std::string content_tc = "";
		bool is_notice = true;
		if (msg_type == 500)
		{
			is_notice = false;
			content = DictionaryHelper::getInstance()->getStringValue_json(root, "msg");
		}
		else
		{
			name = "";
			if (msg_type == 503)
			{
				content = DictionaryHelper::getInstance()->getStringValue_json(root, "msg");
				addGuild(name, content, nation, is_notice);
				channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_GUILD"];
				return;
			}
			else
			{
				auto& msg = DictionaryHelper::getInstance()->getSubDictionary_json(root, "msg");
				auto count = DictionaryHelper::getInstance()->getArrayCount_json(msg, "notice");
				for (int i = 0; i < count; i++)
				{
					auto&notice_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(msg, "notice", i);
					auto lang = DictionaryHelper::getInstance()->getIntValue_json(notice_counts, "lang");
					switch ((LanguageType)lang)
					{
					case LanguageType::ENGLISH:
						content_el = DictionaryHelper::getInstance()->getStringValue_json(notice_counts, "msg");
						break;
					case LanguageType::CHINESE:
						content_zh = DictionaryHelper::getInstance()->getStringValue_json(notice_counts, "msg");
						break;
					case LanguageType::TRADITIONAL_CHINESE:
						content_tc = DictionaryHelper::getInstance()->getStringValue_json(notice_counts, "msg");
						break;
					default:
						content_el = DictionaryHelper::getInstance()->getStringValue_json(notice_counts, "msg");
						break;
					}
				}

				if (msg_type == 502)
				{
					LanguageType nType = LanguageType(Utils::getLanguage());
					if (nType == cocos2d::LanguageType::CHINESE && content_zh != "")
					{
						addPublic_zh(name, content_zh, nation, is_notice);
						channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC_ZH"];
						CHAT chat = { name, content_zh, channel };
						m_vChat.push_back(chat);
					}
					else if (nType == cocos2d::LanguageType::TRADITIONAL_CHINESE && content_tc != "")
					{
						addPublic_zh(name, content_tc, nation, is_notice);
						channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC_ZH"];
						CHAT chat = { name, content_tc, channel };
						m_vChat.push_back(chat);
					}

					if (content_el != "")
					{
						addPublic(name, content_el, nation, is_notice);
						channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC"];
						CHAT chat2 = { name, content_el, channel };
						m_vChat.push_back(chat2);
					}
					return;
				}
			}
		}

		int tempChannelType;
		if (strcmp(receiver.data(), "#voyage_zh") == 0)
		{
			tempChannelType = 0;
			addPublic_zh(name, content, nation, is_notice);
			channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC_ZH"];
		}
		else if (strcmp(receiver.data(), "#voyage") == 0)
		{
			tempChannelType = 1;
			addPublic(name, content, nation, is_notice);
			channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC"];
		}
		else if (strcmp(receiver.data(), "#England") == 0 || strcmp(receiver.data(), "#Portugal") == 0 || strcmp(receiver.data(), "#Spain") == 0
			|| strcmp(receiver.data(), "#Netherlands") == 0 || strcmp(receiver.data(), "#Genoa") == 0)
		{
			tempChannelType = 2;
			addCountry(name, content, nation);
			channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_COUNTRY"];
		}
		else if (strncmp(receiver.data(), "#CITY_", strlen("#CITY_")) == 0 || strncmp(receiver.data(), "#SEA_", strlen("#SEA_")) == 0)
		{
			tempChannelType = 3;
			addZone(name, content, nation);
			channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_ZONE"];
		}
		else if (strncmp(receiver.data(), "#GUILD_", strlen("#GUILD_")) == 0)
		{
			tempChannelType = 4;
			addGuild(name, content, nation, is_notice);
			channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_GUILD"];
		}
		else
		{
			tempChannelType = 5;
			addPrivate(sender_id, name, content, nation);
			channel = SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PRIVATE"];
		}
		CHAT chat = { name, content, channel };
		//添加显示最后五条信息
		m_vChat.push_back(chat);
		if (m_vChat.size() > 5)
		{
			m_vChat.erase(m_vChat.begin());
			m_nChatIndex--;
			if (m_nChatIndex < 0)
			{
				m_nChatIndex = 0;
			}
		}


		if (tempChannelType >= 0 && tempChannelType < 5 && sender_id != SINGLE_HERO->m_iID && msg_type == 500)
		{
			bool isfind = false;
			size_t i = 1;
			int min_grid = MIN(m_vHeroListMsg[tempChannelType].size(), CHAT_LIST_PLAYER);
			for (; i < min_grid; i++)
			{
				if (m_vHeroListMsg[tempChannelType].at(i).cid == sender_id)
				{
					isfind = true;
					break;
				}
			}

			CHATMSG senderchatmsg = { name, channel, icon, sender_id, nation };
			if (isfind)
			{
				m_vHeroListMsg[tempChannelType].erase(m_vHeroListMsg[tempChannelType].begin() + i);
			}

			if (m_vHeroListMsg[tempChannelType].empty())
			{
				int nNation;
				if (SINGLE_HERO->m_bGameMaster)
				{
					nNation = -1;
				}
				else
				{
					nNation = SINGLE_HERO->m_iNation;
				}
				CHATMSG mychatmsg = { SINGLE_HERO->m_sName, channel, SINGLE_HERO->m_iIconidx, SINGLE_HERO->m_iID, nNation };
				m_vHeroListMsg[tempChannelType].push_back(mychatmsg);
			}
			m_vHeroListMsg[tempChannelType].insert(m_vHeroListMsg[tempChannelType].begin() + 1, senderchatmsg);
		}
		if (tempChannelType == m_nChannelType)
		{
			flushChatFirendList();
		}
	}
}

void UISocial::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetFriendsListResult:
	{
											GetFriendsListResult *FriendsListResult = (GetFriendsListResult*)message;
											if (FriendsListResult->failed == 0)
											{
												m_pFriendsResult = FriendsListResult;
												if (m_bIsSendGift)
												{
													m_bIsSendGift = false;
													auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
													TableView* mTableView = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_MY_FRIEND));
													mTableView->updateCellAtIndex(m_nForGiftIndex);
													auto p_gift = view->getChildByName<Widget*>("panel_gift");
													auto t_gift_num = p_gift->getChildByName<Text*>("label_gift_num");
													t_gift_num->setString(StringUtils::format("%d", m_pFriendsResult->can_send_gift_count));
												}
												else
												{
													if (m_bIsFriendList)
													{
														m_bIsFriendList = false;
														flushFirendList(FriendsListResult);
														//改动
														auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
														TableView* mTableView = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_MY_FRIEND));
														//当前cell的高和宽

														Vec2 ss = tableCellSizeForIndex(mTableView, 0);
														if (m_pFriendsResult->n_friends > 4)
														{
															if ((m_pFriendsResult->n_friends / 3) >= 2 && m_ForFriendIndex > (m_pFriendsResult->n_friends - 3))
															{
																mTableView->setContentOffset(Vec2(m_nslider.x, 0), false);
															}
															else if (m_ForFriendIndex > 3 && m_ForFriendIndex < (m_pFriendsResult->n_friends - 3))
															{
																mTableView->setContentOffset(Vec2(m_nslider.x, 0.00 - (m_pFriendsResult->n_friends - (m_ForFriendIndex + 1))*ss.y), false);
															}
														}
													}
													else
													{
														flushFirendList(FriendsListResult);
													}
												}
											}
											else
											{
												UIInform::getInstance()->openInformView(this);
												UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_FRIEND_LIST_FAIL");
											}
											break;
	}
	case PROTO_TYPE_GetBlackListResult:
	{
										  GetBlackListResult *blackListResult = (GetBlackListResult*)message;
										  if (blackListResult->failed == 0)
										  {
											  m_pBlackListResult = blackListResult;
											  flushBlackList(blackListResult);
											  for (int i = 0; i < blackListResult->n_friends; i++)
											  {
												  m_vBlackListName.push_back(blackListResult->friends[i]->heroname);
											  }
											  if (m_bIsBlockList)
											  {
												  m_bIsBlockList = false;
												  //改动
												  auto w_friend = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
												  TableView* mTableView = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_FRIEND_BLACK));
												  //当前cell的高和宽
												  Vec2 ss = tableCellSizeForIndex(mTableView, 0);
												  if (m_pBlackListResult->n_friends > 4)
												  {
													  if ((m_pBlackListResult->n_friends / 3) >= 2 && m_ForBlockIndex > (m_pBlackListResult->n_friends - 3))
													  {
														  mTableView->setContentOffset(Vec2(m_nslider.x, 0), false);
													  }
													  else if (m_ForBlockIndex > 3 && m_ForBlockIndex < (m_pBlackListResult->n_friends - 3))
													  {
														  mTableView->setContentOffset(Vec2(m_nslider.x, 0.00 - (m_pBlackListResult->n_friends - (m_ForBlockIndex + 1))*ss.y), false);
													  }
												  }
											  }
											  else
											  {
												  m_bIsBlockList = false;
												  m_pBlackListResult = blackListResult;
												  flushBlackList(blackListResult);
											  }
										  }
										  else
										  {
											  UIInform::getInstance()->openInformView(this);
											  UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_BLACK_LIST_FAIL");
										  }
										  break;
	}
	case PROTO_TYPE_GetFriendsReqListResult:
	{
											   GetFriendsReqListResult *friendsReqListResult = (GetFriendsReqListResult*)message;
											   if (friendsReqListResult->failed == 0)
											   {
												   if (m_bIsReqList)
												   {
													   m_bIsReqList = false;
													   m_pFriendsReqListResult = friendsReqListResult;
													   flushApplication(friendsReqListResult);
													   //改动
													   auto w_friend = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
													   TableView* mTableView = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_FRIEND_APPLICATION));
													   //当前cell的高和宽
													   Vec2 ss = tableCellSizeForIndex(mTableView, 0);
													   if (m_pFriendsReqListResult->n_friends > 4)
													   {
														   if ((m_pFriendsReqListResult->n_friends / 3) >= 2 && m_ForReqIndex > (m_pFriendsReqListResult->n_friends - 3))
														   {
															   mTableView->setContentOffset(Vec2(m_nslider.x, 0), false);
														   }
														   else if (m_ForReqIndex > 3 && m_ForReqIndex < (m_pFriendsReqListResult->n_friends - 3))
														   {
															   mTableView->setContentOffset(Vec2(m_nslider.x, 0.00 - (m_pFriendsReqListResult->n_friends - (m_ForReqIndex + 1))*ss.y), false);
														   }
													   }
												   }
												   else
												   {
													   m_pFriendsReqListResult = friendsReqListResult;
													   flushApplication(friendsReqListResult);
												   }
											   }
											   else
											   {
												   UIInform::getInstance()->openInformView(this);
												   UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_FRIENDREQ_LIST_FAIL");
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
													  m_bIsReqList = true;
													  UIInform::getInstance()->openInformView(this);
													  UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADDREFUSAL_SUCCESS");
												  }
												  else if (pFriendsResult->actioncode == 3)
												  {
													  m_bIsReqList = true;
													  UIInform::getInstance()->openInformView(this);
													  UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADDED_FRIEND_SUCCESS");
												  }
												  else if (pFriendsResult->actioncode == 2)
												  {
													  m_bIsFriendList = true;
													  UIInform::getInstance()->openInformView(this);
													  UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_BLACK_LIST_SUCCESS");
												  }
												  else if (pFriendsResult->actioncode == 1)
												  {
													  m_bIsFriendList = true;
													  UIInform::getInstance()->openInformView(this);
													  UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_DETELE_FRIEND_SUCCESS");
												  }
												  else if (pFriendsResult->actioncode == 0) //加为好友
												  {
													  m_bIsFriendList = true;
													  m_bIsBlockList = true;
													  UIInform::getInstance()->openInformView(this);
													  UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_FRIEND_SUCCESS");
												  }
												  else if (pFriendsResult->actioncode == 5)//移除拉黑玩家
												  {
													  m_bIsBlockList = true;
													  UIInform::getInstance()->openInformView(this);
													  UIInform::getInstance()->openViewAutoClose("TIP_REMOVE_BLOCK_SUCCESS");
												  }

												  UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]);
												  if (m_pMainButton)
												  {
													  if (m_pMainButton->getName() == "button_firends")
													  {
														  ProtocolThread::GetInstance()->getFriendsList(UILoadingIndicator::create(this));
													  }
													  else if (m_pMainButton->getName() == "button_blacklist")
													  {
														  ProtocolThread::GetInstance()->getBlackList(UILoadingIndicator::create(this));
													  }
													  else
													  {
														  ProtocolThread::GetInstance()->getFriendsReqList(UILoadingIndicator::create(this));
													  }
												  }

											  }
											  else if (pFriendsResult->failed == 105)
											  {
												  UIInform::getInstance()->openInformView(this);
												  UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_REQUEST_ADDFRIENDED");
											  }
											  else
											  {
												  UIInform::getInstance()->openInformView(this);
												  UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_ADD_FRIENDED_FAIL");
											  }
											  break;
	}
	case PROTO_TYPE_SearchUserByNameResult:
	{
											  SearchUserByNameResult *searchResult = (SearchUserByNameResult*)message;
											  if (searchResult->failed == 0)
											  {
												  closeView();
												  showSearchUserList(searchResult);
											  }
											  else
											  {
												  UIInform::getInstance()->openInformView(this);
												  UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_HEATCHINPUT_FAIL");
											  }
											  break;
	}
	case PROTO_TYPE_GetUserInfoByIdResult:
	{
											 GetUserInfoByIdResult *result = (GetUserInfoByIdResult *)message;
											 if (result->failed == 0)
											 {
												 SINGLE_HERO->m_InvitedToGuildNation = result->nation;
												 UICommon::getInstance()->openCommonView(this);
												 UICommon::getInstance()->setCameraMask(_cameraMask);
												 if (m_pTempButton->getParent()->getTag() == SINGLE_HERO->m_iID)
												 {
													 UICommon::getInstance()->flushFriendDetail(result, true);
												 }
												 else
												 {
													 UICommon::getInstance()->flushFriendDetail(result, false);
												 }
											 }
											 else
											 {
												 UIInform::getInstance()->openInformView(this);
												 UIInform::getInstance()->setCameraMask(_cameraMask);
												 UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_PLAYER_INFO_FAIL");
											 }
											 break;
	}
	case PROTO_TYPE_InviteUserToGuildResult:
	{
											   InviteUserToGuildResult *result = (InviteUserToGuildResult *)message;
											   if (result->failed == 0)
											   {
												   UIInform::getInstance()->openInformView(this);
												   UIInform::getInstance()->openViewAutoClose("TIP_GULID_INVINT_SUCCESS");
											   }
											   else if (result->failed == 11)
											   {
												   //邀请的对象不是同一势力
												   UIInform::getInstance()->openInformView(this);
												   UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITED_FAILED_NOT_SAME_COUNTRY_CONTENT");
											   }
											   else
											   {
												   UIInform::getInstance()->openInformView(this);
												   UIInform::getInstance()->openConfirmYes("TIP_GULID_INVINT_FAIL");
											   }
											   break;
	}
	case PROTO_TYPE_SendActivitiesGiftResult:
	{
												SendActivitiesGiftResult *result = (SendActivitiesGiftResult *)message;
												if (result->failed == 0)
												{
													m_bIsSendGift = true;
													UIInform::getInstance()->openInformView(this);
													UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_GIVE_GIFT_SUCCESS");
													ProtocolThread::GetInstance()->getFriendsList();
												}
												else
												{
													UIInform::getInstance()->openInformView(this);
													UIInform::getInstance()->openConfirmYes("TIP_PUP_YOUR_GIFT_HAVE_GIVEOUT");
												}
												break;
	}
	default:
	{
			   break;
	}
	}
}

void UISocial::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	Widget *button = (Widget*)pSender;
	std::string name = button->getName();

	if (isButton(button_firends)) //好友列表
	{
		changeMainButton(button);
		m_nPageIndex = PAGE_MY_FRIEND;
		openFaceBookInvite(false);
		ProtocolThread::GetInstance()->getFriendsList(UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_blacklist)) //黑名单
	{
		changeMainButton(button);
		m_nPageIndex = PAGE_FRIEND_BLACK;
		openFaceBookInvite(false);
		ProtocolThread::GetInstance()->getBlackList(UILoadingIndicator::create(this));
		return;
	}

	if (isButton(button_request)) //好友请求
	{
		changeMainButton(button);
		m_nPageIndex = PAGE_FRIEND_APPLICATION;
		openFaceBookInvite(false);
		ProtocolThread::GetInstance()->getFriendsReqList(UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_addfacebook_Friend)) //faceBook邀请好友
	{
		changeMainButton(button);
		openFaceBookInvite(true);
		auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
		auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown"));
		image_pulldown->setVisible(false);
		auto button_search = dynamic_cast<Button*>(view->getChildByName<Widget*>("button_search"));
		button_search->setVisible(false);
		auto friends_list = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_friend"));
		friends_list->removeAllChildrenWithCleanup(true);
		return;
	}
	if (isButton(button_addfacebook_invite)) //发送faceBook邀请好码
	{
		Utils::ShowFacebookInviteDialog(ProtocolThread::GetInstance()->getInviteCode());
		return;
	}

	if (isButton(button_chat_back)) //关闭
	{
		m_nLastChannelType = m_nChannelType;
		m_nChannelType = -1;
		m_pPreChannelBtn = nullptr;
		closeView();
		this->unschedule(schedule_selector(UISocial::getLatestSpeakers));
		auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
		if (!view)
		{
			unregisterCallBack();
		}
		auto instance = ModeLayerManger::getInstance();
		auto layer = instance->getModelLayer();
		auto type = instance->getNeedShowLayerType();
		if (layer != nullptr)
		{
//			instance->showModelLayer(true);
			if (type == CENTER_CHARACTER)
			{
				instance->showModelLayer(true);
				instance->operaModels(type);
			}
			else
			{
				int tag = layer->getTag();
				if (tag == 1)
				{
					instance->showModelLayer(true);
					instance->operaModels(type);
				}
			}

		}
		return;
	}

	if (isButton(button_back)) //关闭好友主界面
	{
		openFaceBookInvite(false);
		closeView();
		unregisterCallBack();
		this->unschedule(schedule_selector(UISocial::getLatestSpeakers));
		return;
	}

	if (isButton(button_chat_close))//关闭聊天
	{
		LanguageType nType = LanguageType(Utils::getLanguage());
		if (nType == cocos2d::LanguageType::ENGLISH)
		{
			m_nChannelType = 1;
		}
		else
		{
			m_nChannelType = 0;
		}
		auto index = button->getTag() - 5;
		SINGLE_CHAT->m_private_unread_falg[m_vPrivateName[index].data()] = false;
		m_vPrivateName.erase(m_vPrivateName.begin() + index);
		flushChatButton();
		m_nLastTime.erase(m_nLastTime.begin() + index);
		flushChatList();
		return;
	}

	if (isButton(button_close) || isButton(button_cancel))//关闭界面
	{
		closeView();
		return;
	}

	if (isButton(button_send)) //发送信息
	{
		auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
		auto t_chat = dynamic_cast<TextField*>(Helper::seekWidgetByName(panel_chat, "textfield_chat"));
		ui::EditBox* edit_chat = dynamic_cast<ui::EditBox*>(t_chat->getParent()->getChildByTag(1));
		if (strcmp(edit_chat->getText(), "") == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_INPUT_SEND_TEXT");
		}
		else
		{
			int timeNow = getCurrentTimeUsev() / 1000.0;
			if (timeNow - sendIntervalTime < 1)//控制刷屏时间
			{
				openSuccessOrFailDialog("TIP_CHAT_SPEAK_TOO_FAST");
			}
			else
			{
				std::string chat = edit_chat->getText();
				auto chatlength = chat.length();
				if (chatlength>40 * 3)//输入长度限制
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ROLE_PERSONAL_SIGN_TOO_LONG");
				}
				else
				{
					sendChat();
					sendIntervalTime = getCurrentTimeUsev() / 1000.0;
				}
			}
		}
		return;
	}

	if (isButton(button_mail)) //发送邮件
	{
		UIEmail::getInstance()->sendEmailToPlayer(button->getParent()->getName(), "");
		return;
	}

	if (isButton(button_chat)) //打开聊天界面
	{
		bool isfind = false;
		for (size_t i = 0; i < m_vPrivateName.size(); i++)
		{
			if (m_vPrivateName[i] == button->getParent()->getName())
			{
				isfind = true;
				m_nChannelType = i + 5;
				break;
			}
		}
		if (!isfind)
		{
			m_vPrivateName.push_back(button->getParent()->getName());
			m_nChannelType = m_vPrivateName.size() + 4;
			m_nLastTime.push_back(-1);
		}
		flushChatButton();
		flushChatList();
		showChat();
		return;
	}

	if (isButton(button_more)) //更多
	{
		ProtocolThread::GetInstance()->getUserInfoById(button->getParent()->getTag(), UILoadingIndicator::create(this));
		return;
	}

	if (isButton(button_confirm_yes))//确认界面确认按钮点击事件
	{
		if (m_eConfirmIndex == CONFIRM_INDEX_FRIEND_BLOCK || m_eConfirmIndex == CONFIRM_INDEX_STRANGER_BLOCK)//添加黑名单内好友
		{
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(), 2, UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_REMOVEFRIEND)//删除好友
		{
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(), 1, UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_REFUSAL_ADDFRIEND)//拒绝添加
		{
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(), 4, UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_ACCEPT_ADDFRIEND)//同意好友申请
		{
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(), 3, UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_STRANGER_ADDFRIEND)//添加陌生人
		{
			if (m_pTempButton->getParent()->getTag() == SINGLE_HERO->m_iID)
			{
				return;
			}
			m_pTempButton->setBright(false);
			auto i_add = m_pTempButton->getChildByName<ImageView*>("image_add");
			setGLProgramState(i_add, true);
			m_pTempButton->setTag(0);
			if (UICommon::getInstance()->getViewRoot(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]) && getViewRoot(SOCIAL_COCOS_RES[SOCIAL_SEARCH_RESULT_CSB]))
			{
				m_pFirendButton->setBright(false);
				auto i_add = m_pFirendButton->getChildByName<ImageView*>("image_add");
				setGLProgramState(i_add, true);
				m_pFirendButton->setTag(0);
			}
			m_pFirendButton = nullptr;
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(), 0, UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_SOCIAL_INVITE_GUILD) //公会发送邀请
		{
			ProtocolThread::GetInstance()->inviteUserToGuild(m_pTempButton->getTag(), UILoadingIndicator::create(this));
			return;
		}
		if (m_eConfirmIndex == CONFIRM_INDEX_FRIEND_NOT_ONLINE)//好友不在线
		{
			return;
		}
		if (m_eConfirmIndex == CONFIRM_INDEX_BLOCK_REMOVE)//
		{
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(), 5, UILoadingIndicator::create(this));

			return;
		}
	}

	if (isButton(button_confirm_no) || isButton(button_error_yes))//提示信息确定
	{
		return;
	}

	if (isButton(button_player))//刷新聊天界面
	{
		bool isfind = false;
		for (size_t i = 0; i < m_vPrivateName.size(); i++)
		{
			if (m_vPrivateName[i] == name)
			{
				isfind = true;
				m_nChannelType = i + 5;
				break;
			}
		}
		if (!isfind)
		{
			m_vPrivateName.push_back(name);
			m_nChannelType = m_vPrivateName.size() + 4;
			m_nLastTime.push_back(-1);
		}
		flushChatButton();
		flushChatList();
		return;
	}

	if (isButton(button_friend_delete)) //删除好友
	{
		m_bIsFriendList = true;
		m_eConfirmIndex = CONFIRM_INDEX_REMOVEFRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_DELETE_FRIEND_TITLE", "TIP_SOCIAL_DELETE_FRIEND");
		return;
	}

	if (isButton(button_friend_block)) //拉黑
	{
		m_bIsFriendList = true;
		m_eConfirmIndex = CONFIRM_INDEX_FRIEND_BLOCK;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_FRIEND");
		return;
	}

	if (isButton(button_stranger_block)) //拉黑陌生人
	{
		m_eConfirmIndex = CONFIRM_INDEX_STRANGER_BLOCK;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_STRANGER");
		return;
	}

	if (isButton(button_invite)) //公会邀请
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_SOCIAL_INVITE_GUILD;
		if (SINGLE_HERO->m_InvitedToGuildNation == SINGLE_HERO->m_iNation)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_INVITE_GUILD_TITLE", "TIP_GUILD_INVINT_GUILD");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITED_FAILED_NOT_SAME_COUNTRY_CONTENT");
		}
		return;
	}

	if (isButton(button_add_friend)) //加为好友
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_STRANGER_ADDFRIEND;
		if (button->getParent()->getTag() == SINGLE_HERO->m_iID)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_NOT_ADD_FRIENDED");
		}
		else if (button->getTag() == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_AGAIN_ADD_FRIENDS");
		}
		else if (button->getTag() == 1)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_ALREADY_GOOD_FRIEND");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_ADD_FRIEND_TITLE", "TIP_SOCIAL_ADD_FRIEND");
		}
		return;
	}
	if (isButton(button_stranger_remove) || isButton(button_accept_0))//移除黑名单
	{
		m_bIsBlockList = true;
		m_eConfirmIndex = CONFIRM_INDEX_BLOCK_REMOVE;
		m_pTempButton = button;
		m_ForBlockIndex = button->getTag();
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_REMOVE_BLOCK_TITLE", "TIP_REMOVE_BLOCK");
		return;
	}

	bool isfind = false;
	for (size_t i = 0; i < m_vPrivateName.size(); i++)
	{
		if (m_vPrivateName[i] == name)
		{
			isfind = true;
			m_nChannelType = i + 5;
			break;
		}
	}
	if (!isfind)
	{
		m_vPrivateName.push_back(name);
		m_nChannelType = m_vPrivateName.size() + 4;
		m_nLastTime.push_back(-1);
	}
	flushChatButton();
	flushChatList();
}

void UISocial::friendListButtonEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget *button = (Widget*)pSender;
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

	if (isButton(button_mail))//好友列表发送邮件
	{
		UIEmail::getInstance()->sendEmailToPlayer(button->getParent()->getName(), "");
		return;
	}
	if (isButton(button_chat))//发起好友聊天
	{
		if (button->getTag())
		{
			openView(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
			LanguageType nType = LanguageType(Utils::getLanguage());
			auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
			auto l_chat = panel_chat->getChildByName<ListView*>("listView_chatbutton");
			if (nType == cocos2d::LanguageType::ENGLISH)
			{
				l_chat->removeItem(0);
			}

			bool isfind = false;
			for (size_t i = 0; i < m_vPrivateName.size(); i++)
			{
				if (m_vPrivateName[i] == button->getParent()->getName())
				{
					isfind = true;
					m_nChannelType = i + 5;
					break;
				}
			}
			if (!isfind)
			{
				m_vPrivateName.push_back(button->getParent()->getName());
				m_nChannelType = m_vPrivateName.size() + 4;
				m_nLastTime.push_back(-1);
			}
			flushChatButton();
			flushChatList();
			auto t_chat = panel_chat->getChildByName<TextField*>("textfield_chat");
			t_chat->setVisible(false);
			ui::EditBox* edit_chat = nullptr;
			if (!edit_chat)
			{
				edit_chat = ui::EditBox::create(Size(t_chat->getContentSize()), "input.png");
				edit_chat->setTag(1);
				t_chat->getParent()->addChild(edit_chat);
				edit_chat->setDelegate(this);
				edit_chat->setAnchorPoint(t_chat->getAnchorPoint());
				edit_chat->setPlaceholderFontColor(Color3B(255, 255, 255));
				edit_chat->setPosition(t_chat->getPosition());
				edit_chat->setFontColor(Color3B(255, 255, 255));
				edit_chat->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
				edit_chat->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
				edit_chat->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
			}
			showChat();
		}
		else
		{
			m_eConfirmIndex = CONFIRM_INDEX_FRIEND_NOT_ONLINE;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_FRIEND_NOT_ONLINE");
		}
		return;
	}
	if (isButton(button_more) || isButton(image_head_bg_1))//好友信息
	{
		m_bIsFriendList = true;
		m_pTempButton = button;
		m_ForFriendIndex = m_pTempButton->getTag();
		ProtocolThread::GetInstance()->getUserInfoById(button->getParent()->getTag(), UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_search)) //搜索按钮
	{
		openView(SOCIAL_COCOS_RES[SOCIAL_SEARCH_CSB]);
		auto w_search = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_SEARCH_CSB]);
		TextField* text_search = w_search->getChildByName<TextField*>("textfield_friend_name");
		text_search->setTextVerticalAlignment(TextVAlignment::CENTER);
		text_search->setVisible(false);
		ui::EditBox* editbox_search = nullptr;
		if (!editbox_search)
		{
			editbox_search = ui::EditBox::create(Size(text_search->getContentSize()), "input.png");
			editbox_search->setTag(1);
			text_search->getParent()->addChild(editbox_search);
			editbox_search->setPosition(text_search->getPosition());
			editbox_search->setText("");
			editbox_search->setPlaceholderFontColor(Color3B(116, 98, 71));
			editbox_search->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
			editbox_search->setFontColor(Color3B(46, 29, 14));
			editbox_search->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
			editbox_search->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		}

		w_search->getChildByName<Button*>("button_ok")->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));
		scheduleUpdate();
		return;
	}
	if (isButton(button_ok)) //进行搜索
	{
		auto w_search = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_SEARCH_CSB]);
		TextField* text_name = w_search->getChildByName<TextField*>("textfield_friend_name");
		ui::EditBox* edit_name = dynamic_cast<ui::EditBox*>(text_name->getParent()->getChildByTag(1));
		std::string e_name = edit_name->getText();
		auto name = (char*)e_name.data();
		ProtocolThread::GetInstance()->searchUserByName(name, UILoadingIndicator::create(this));
		return;
	}

	if (isButton(button_gift_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_SOCIAL_GIVE_GIFT_TITLE", "TIP_SOCIAL_GIVE_GIFT_CONTENT");
		return;
	}

	if (isButton(button_gift))
	{
		if (!m_bIsSendGift)
		{
			int cur_cid = button->getParent()->getTag();
			m_nForGiftIndex = button->getTag();
			bool isCan = false;
			for (size_t i = 0; i < m_pFriendsResult->n_can_not_send_cid; i++)
			{
				if (cur_cid == m_pFriendsResult->can_not_send_cid[i])
				{
					isCan = true;
					break;
				}
			}

			if (isCan)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_GIVE_GIFT_FAIL");
			}
			else
			{
				ProtocolThread::GetInstance()->sendActivitiesGift(cur_cid, UILoadingIndicator::create(this));
			}
		}
		return;
	}

	if (getViewRoot(SOCIAL_COCOS_RES[SOCIAL_SEARCH_RESULT_CSB]))
	{
		m_pFirendButton = button->getChildByName<Widget*>("button_add_friend");
	}
	m_pTempButton = button->getChildByName<Widget*>("label_player_name");
	ProtocolThread::GetInstance()->getUserInfoById(button->getTag(), UILoadingIndicator::create(this));
}

void UISocial::blockListAndRequestButtonEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget *button = (Widget*)pSender;
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

	if (isButton(button_more) || isButton(image_head_bg_1) || isButton(image_head_bg))//拉黑人员详细信息
	{
		m_bIsBlockList = true;
		m_bIsReqList = true;
		m_pTempButton = button;
		m_ForBlockIndex = m_pTempButton->getTag();
		m_ForReqIndex = m_pTempButton->getTag();
		ProtocolThread::GetInstance()->getUserInfoById(button->getParent()->getTag(), UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_accept)) //同意
	{
		m_bIsReqList = true;
		m_pTempButton = button;
		m_ForReqIndex = m_pTempButton->getTag();
		m_eConfirmIndex = CONFIRM_INDEX_ACCEPT_ADDFRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_REQUEST_ACCEPT_TITLE", "TIP_SOCIAL_REQUEST_ACCEPT");
		return;
	}
	if (isButton(button_delete_0)) //拒绝
	{
		m_bIsReqList = true;
		m_pTempButton = button;
		m_ForReqIndex = m_pTempButton->getTag();
		m_eConfirmIndex = CONFIRM_INDEX_REFUSAL_ADDFRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_REQUEST_REFUSAL_TITLE", "TIP_SOCIAL_REQUEST_REFUSAL");
		return;
	}

	m_pTempButton = button->getChildByName<Widget*>("label_player_name");
	ProtocolThread::GetInstance()->getUserInfoById(button->getTag(), UILoadingIndicator::create(this));
}

void UISocial::chatEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget *button = (Widget*)pSender;
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	m_nFirstSendToPrivate = STATE_NONE;
	if (isButton(button_chat_public) || isButton(button_chat_public_zh) || isButton(button_chat_country) || isButton(button_chat_zone)
		|| isButton(button_chat_gulid))//聊天界面上方按钮点击事件
	{
		m_nChannelType = button->getTag();
		switch (m_nChannelType)
		{
		case 0:
			SINGLE_CHAT->m_public_unread_falg_zh = false;
			break;
		case 1:
			SINGLE_CHAT->m_public_unread_falg = false;
			break;
		case 2:
			SINGLE_CHAT->m_country_unread_falg = false;
			break;
		case 3:
			SINGLE_CHAT->m_zone_unread_falg = false;
			break;
		case 4:
			SINGLE_CHAT->m_guild_unread_falg = false;
			break;
		default:
			break;
		}
		flushChatButton();
		flushChatList();
		return;
	}
	if (isButton(button_chat_private))
	{
		m_nChannelType = button->getTag();
		SINGLE_CHAT->m_private_unread_falg[m_vPrivateName[m_nChannelType - 5].data()] = false;
		flushChatButton();
		flushChatList();
		auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
		if (!panel_chat)
		{
			return;
		}
		auto l_chat = panel_chat->getChildByName<ListView*>("listview_chat");
		auto counts = l_chat->getChildrenCount();
		if (counts <= 0)
		{
			m_nFirstSendToPrivate = STATE_FIRST;
		}
		else
		{
			m_nFirstSendToPrivate = STATE_AFTER;
		}

		return;
	}

	if (isButton(button_more))//查看详情
	{
		m_pTempButton = button;
		m_bIsBlockList = true;
		m_ForBlockIndex = m_pTempButton->getTag();
		ProtocolThread::GetInstance()->getUserInfoById(button->getParent()->getTag(), UILoadingIndicator::createWithMask(this, _cameraMask));
		return;
	}
	if (isButton(button_chat))
	{
		if (m_bisFriend == false)
		{
			m_bisFriend = true;
			auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
			auto t_chat = dynamic_cast<TextField*>(Helper::seekWidgetByName(panel_chat, "textfield_chat"));
			ui::EditBox* edit_chat = dynamic_cast<ui::EditBox*>(t_chat->getParent()->getChildByTag(1));
			edit_chat->setPosition(Vec2(11, 39));
			edit_chat->setContentSize(Size(848, 60));
			edit_chat->setMaxLength(480);
		}
		else
		{
			m_bisFriend = false;
			auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
			auto t_chat = dynamic_cast<TextField*>(Helper::seekWidgetByName(panel_chat, "textfield_chat"));
			ui::EditBox* edit_chat = dynamic_cast<ui::EditBox*>(t_chat->getParent()->getChildByTag(1));
			edit_chat->setPosition(Vec2(11, 39));
			edit_chat->setContentSize(Size(506, 60));
			edit_chat->setMaxLength(240);
		}
		flushChatList();
		flushChatFirendList();
		return;
	}

	if (name == SINGLE_HERO->m_sName)
	{
		return;
	}
	bool isfind = false;
	for (size_t i = 0; i < m_vPrivateName.size(); i++)
	{
		if (m_vPrivateName[i] == name)
		{
			isfind = true;
			m_nChannelType = i + 5;
			break;
		}
	}
	if (!isfind)
	{
		m_vPrivateName.push_back(name);
		m_nChannelType = m_vPrivateName.size() + 4;
		m_nFirstSendToPrivate = STATE_FIRST;
		m_nLastTime.push_back(-1);
	}
	flushChatButton();
	flushChatList();
}

void UISocial::changeMainButton(Widget *target)
{
	if (!target)
	{
		auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
		target = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_firends"));
	}
	if (m_pMainButton)
	{
		m_pMainButton->setBright(true);
		m_pMainButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pMainButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_pMainButton = target;
	m_pMainButton->setBright(false);
	m_pMainButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pMainButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
}

void UISocial::flushFirendList(const GetFriendsListResult *pFriendsResult)
{
	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto t_title = view->getChildByName<Text*>("label_title");
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_FIREND_TITLE"]);
	auto image_div = view->getChildByName<ImageView*>("image_div_2");
	t_title->setPositionX(image_div->getPositionX() - image_div->getBoundingBox().size.width / 2 + 20);
	auto p_gift = view->getChildByName<Widget*>("panel_gift");
	p_gift->setVisible(true);
	auto t_gift_num = p_gift->getChildByName<Text*>("label_gift_num");
	auto b_gift = p_gift->getChildByName<Button*>("button_gift_info");
	auto t_gift = p_gift->getChildByName<Text*>("label_gift");
	t_gift_num->setString(StringUtils::format("%d", pFriendsResult->can_send_gift_count));
	t_gift_num->setPositionX(b_gift->getPositionX() - b_gift->getBoundingBox().size.width / 2 - t_gift_num->getBoundingBox().size.width - 10);
	t_gift->setPositionX(t_gift_num->getPositionX() - t_gift->getBoundingBox().size.width / 2 - 10);
	b_gift->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));

	auto b_search = view->getChildByName<Button*>("button_search");
	b_search->setVisible(true);
	b_search->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));
	auto friends_list = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_friend"));
	friends_list->removeAllChildrenWithCleanup(true);
	friends_list->setVisible(false);
	auto p_no = view->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(false);
	flushApplicationNum(pFriendsResult->friendsrequestnum);

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	TableView* mTableView_black = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_FRIEND_BLACK));
	if (mTableView_black)
	{
		mTableView_black->removeFromParentAndCleanup(true);
	}
	TableView* mTableView_application = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_FRIEND_APPLICATION));
	if (mTableView_application)
	{
		mTableView_application->removeFromParentAndCleanup(true);
	}
	if (pFriendsResult->n_friends < 1)
	{
		auto t_no1 = p_no->getChildByName<Text*>("label_no_found_1_1");
		auto t_no2 = p_no->getChildByName<Text*>("label_no_found_2_1");
		t_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_FIRENDLIST_NO1"]);
		t_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_FIRENDLIST_NO2"]);
		p_no->setVisible(true);
		image_pulldown->setVisible(false);
		TableView* mTableView = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_MY_FRIEND));
		if (mTableView)
		{
			mTableView->removeFromParentAndCleanup(true);
		}
		return;
	}
	//使用tableview
	TableView* mTableView = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_MY_FRIEND));
	if (mTableView)
	{
		mTableView->reloadData();
		return;
	}
	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(friends_list->getContentSize().width, friends_list->getContentSize().height));
		view->addChild(mTableView, 100);
	}
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(friends_list->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(10240 + PAGE_MY_FRIEND);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	mTableView->setBounceable(true);
	this->setCameraMask(_cameraMask, true);
}

void UISocial::flushBlackList(const GetBlackListResult *pBlackResult)
{
	auto w_friend = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto t_title = w_friend->getChildByName<Text*>("label_title");
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_BLACK_TITLE"]);
	auto image_div = w_friend->getChildByName<ImageView*>("image_div_2");
	t_title->setPositionX(image_div->getPositionX() - t_title->getBoundingBox().size.width / 2);
	auto p_gift = w_friend->getChildByName<Widget*>("panel_gift");
	p_gift->setVisible(false);
	auto b_search = w_friend->getChildByName<Button*>("button_search");
	b_search->setVisible(false);
	auto friends_list = dynamic_cast<ListView*>(w_friend->getChildByName<Widget*>("listview_friend"));
	friends_list->setVisible(false);

	auto item = w_friend->getChildByName<Widget*>("panel_block");
	friends_list->removeAllChildrenWithCleanup(true);

	auto p_no = w_friend->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(false);
	auto image_pulldown = w_friend->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	TableView* mTableView_friend = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_MY_FRIEND));
	if (mTableView_friend)
	{
		mTableView_friend->removeFromParentAndCleanup(true);
	}
	TableView* mTableView_application = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_FRIEND_APPLICATION));
	if (mTableView_application)
	{
		mTableView_application->removeFromParentAndCleanup(true);
	}
	if (pBlackResult->n_friends < 1)
	{
		auto t_no1 = p_no->getChildByName<Text*>("label_no_found_1_1");
		auto t_no2 = p_no->getChildByName<Text*>("label_no_found_2_1");
		t_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_BLACKLIST_NO"]);
		t_no2->setString("");
		p_no->setVisible(true);
		image_pulldown->setVisible(false);
		TableView* mTableView = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_FRIEND_BLACK));
		if (mTableView)
		{
			mTableView->removeFromParentAndCleanup(true);
		}
		return;
	}
	//使用tableview

	TableView* mTableView = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_FRIEND_BLACK));
	if (mTableView)
	{
		mTableView->reloadData();
		return;
	}
	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(friends_list->getContentSize().width, friends_list->getContentSize().height));
		w_friend->addChild(mTableView, 100);
	}

	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(friends_list->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(10240 + PAGE_FRIEND_BLACK);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	mTableView->setBounceable(true);
	this->setCameraMask(_cameraMask, true);
}

void UISocial::flushApplication(const GetFriendsReqListResult *pFriendsReqResult)
{
	auto w_friend = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto t_title = w_friend->getChildByName<Text*>("label_title");
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_REQUEST_TITLE"]);
	auto image_div = w_friend->getChildByName<ImageView*>("image_div_2");
	t_title->setPositionX(image_div->getPositionX() - t_title->getBoundingBox().size.width / 2);
	auto p_gift = w_friend->getChildByName<Widget*>("panel_gift");
	p_gift->setVisible(false);
	auto b_search = w_friend->getChildByName<Button*>("button_search");
	b_search->setVisible(false);
	auto friends_list = dynamic_cast<ListView*>(w_friend->getChildByName<Widget*>("listview_friend"));
	friends_list->setVisible(false);

	auto item = w_friend->getChildByName<Widget*>("panel_request");
	friends_list->removeAllChildrenWithCleanup(true);
	flushApplicationNum(pFriendsReqResult->n_friends);
	auto p_no = w_friend->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(false);
	auto image_pulldown = w_friend->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	TableView* mTableView_friend = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_MY_FRIEND));
	if (mTableView_friend)
	{
		mTableView_friend->removeFromParentAndCleanup(true);
	}
	TableView* mTableView_black = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_FRIEND_BLACK));
	if (mTableView_black)
	{
		mTableView_black->removeFromParentAndCleanup(true);
	}
	if (pFriendsReqResult->n_friends < 1)
	{
		auto t_no1 = p_no->getChildByName<Text*>("label_no_found_1_1");
		auto t_no2 = p_no->getChildByName<Text*>("label_no_found_2_1");
		t_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_REQUESTLIST_NO"]);
		t_no2->setString("");
		p_no->setVisible(true);
		image_pulldown->setVisible(false);
		TableView* mTableView = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_FRIEND_APPLICATION));
		if (mTableView)
		{
			mTableView->removeFromParentAndCleanup(true);
		}
		return;
	}
	//使用tableview

	TableView* mTableView = dynamic_cast<TableView*>(w_friend->getChildByTag(10240 + PAGE_FRIEND_APPLICATION));
	if (mTableView)
	{
		mTableView->reloadData();
		return;
	}
	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(friends_list->getContentSize().width, friends_list->getContentSize().height));
		w_friend->addChild(mTableView, 100);
	}

	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(friends_list->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(10240 + PAGE_FRIEND_APPLICATION);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	mTableView->setBounceable(true);
	this->setCameraMask(_cameraMask, true);

	/*for(int i = 0; i < pFriendsReqResult->n_friends; ++i)
	{
	auto friendsItem = item->clone();
	auto img_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(friendsItem,"image_head"));
	auto img_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(friendsItem,"image_flag"));
	auto t_name = dynamic_cast<Text*>(friendsItem->getChildByName<Widget*>("label_player_name"));
	auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(friendsItem,"label_lv"));

	friendsItem->setTag(pFriendsReqResult->friends[i]->characterid);
	friendsItem->setName(pFriendsReqResult->friends[i]->heroname);
	img_icon->ignoreContentAdaptWithSize(false);
	img_icon->loadTexture(getPlayerIconPath(pFriendsReqResult->friends[i]->iconidx));
	img_flag->ignoreContentAdaptWithSize(false);
	img_flag->loadTexture(getCountryIconPath(pFriendsReqResult->friends[i]->nation));
	t_name->setString(pFriendsReqResult->friends[i]->heroname);
	t_lv->setString(String::createWithFormat("Lv. %ld",pFriendsReqResult->friends[i]->level)->_string);

	friends_list->pushBackCustomItem(friendsItem);
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(friends_list,image_pulldown);*/
}

void UISocial::flushApplicationNum(const int nNum)
{
	auto w_friend = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto i_unread = dynamic_cast<ImageView*>(Helper::seekWidgetByName(w_friend, "image_unread"));
	if (nNum)
	{
		i_unread->setVisible(true);
	}
	else
	{
		i_unread->setVisible(false);
	}
}

void UISocial::showChat(const int cameraMask)
{
	this->setCameraMask(cameraMask, true);
}

void UISocial::sendChat()
{
	auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	auto t_chat = dynamic_cast<TextField*>(Helper::seekWidgetByName(panel_chat, "textfield_chat"));
	ui::EditBox* edit_chat = dynamic_cast<ui::EditBox*>(t_chat->getParent()->getChildByTag(1));
	std::string m_vChat = edit_chat->getText();
	std::string html_color_begin;
	std::string html_color_end;
#if WIN32
	html_color_begin = "";
	html_color_end = "";
#else
	html_color_begin = "<html><font size='24' color='#2E1D0E'>";
	html_color_end = "</font></html>";
#endif
	std::string new_vChat = "";
	if (illegal_character_check(m_vChat))
	{
		auto lengBegin = html_color_begin.length();
		auto lengEnd = html_color_end.length();

		auto posbegin = m_vChat.find(html_color_begin, 0);

		auto str_1 = m_vChat.erase(posbegin, lengBegin);
		auto posend = str_1.find(html_color_end, 0);
		auto newContent = str_1.erase(posend, lengEnd);
		m_vChat = "";
		m_vChat = newContent;
	}

	if (m_nChannelType < 5)
	{
		ProtocolThread::GetInstance()->sendChat(m_nChannelType, m_vChat.c_str());
	}
	else
	{
		m_IsSendPrivate = true;
		m_IsMeSendToPrivate = true;
		m_nFirstSendToPrivate = STATE_AFTER;
		ProtocolThread::GetInstance()->sendPrivateChat((((Button *)m_pPreChannelBtn)->getTitleText()).c_str(), m_vChat.c_str());
	}
	int64_t curTime = getCurrentTimeUsev() / 1000;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	int nation;
	if (SINGLE_HERO->m_bGameMaster)
	{
		nation = -1;
	}
	else
	{
		nation = SINGLE_HERO->m_iNation;
	}
	switch (m_nChannelType)
	{
	case 0:
	{
			  if (m_nLastTime.at(0) < 0)
			  {
				  SINGLE_CHAT->addPublic_zh(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
			  }
			  SINGLE_CHAT->addPublic_zh(SINGLE_HERO->m_sName.c_str(), m_vChat.c_str(), nation);
			  break;
	}
	case 1:
	{
			  if (m_nLastTime.at(1) < 0)
			  {
				  SINGLE_CHAT->addPublic(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
			  }
			  SINGLE_CHAT->addPublic(SINGLE_HERO->m_sName.c_str(), m_vChat.c_str(), nation);
			  break;
	}
	case 2:
	{
			  if (m_nLastTime.at(2) < 0)
			  {
				  SINGLE_CHAT->addCountry(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
			  }
			  SINGLE_CHAT->addCountry(SINGLE_HERO->m_sName.c_str(), m_vChat.c_str(), nation);
			  break;
	}
	case 3:
	{
			  if (m_nLastTime.at(3) < 0)
			  {
				  SINGLE_CHAT->addZone(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
			  }
			  SINGLE_CHAT->addZone(SINGLE_HERO->m_sName.c_str(), m_vChat.c_str(), nation);
			  break;
	}
	case  4:
	{
			   if (m_nLastTime.at(4) < 0)
			   {
				   SINGLE_CHAT->addGuild(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
			   }
			   SINGLE_CHAT->addGuild(SINGLE_HERO->m_sName.c_str(), m_vChat.c_str(), nation);
			   break;
	}
	default:
	{
			   if (m_nLastTime.at(m_nChannelType) < 0)
			   {
				   SINGLE_CHAT->addPrivate(((Button *)m_pPreChannelBtn)->getTitleText(), String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
			   }
			   SINGLE_CHAT->addPrivate(((Button *)m_pPreChannelBtn)->getTitleText(), SINGLE_HERO->m_sName.c_str(), m_vChat.c_str(), nation);
			   flushChatFirendList();
			   break;
	}
	}
	m_nLastTime.at(m_nChannelType) = CHAT_TIME_INTERVAL;
	edit_chat->setText("");
	flushChatList();
}

void UISocial::flushChatList()
{
	auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	if (!panel_chat)
	{
		return;
	}
	auto panel_chatlist_1 = dynamic_cast<Layout*>(panel_chat->getChildByName("panel_chatlist_1"));
	auto image_chatlist_bg = dynamic_cast<ImageView*>(panel_chat->getChildByName("image_chatlist_bg"));
	auto image_div = panel_chatlist_1->getChildByName<ImageView*>("image_div");
	auto l_chat = panel_chat->getChildByName<ListView*>("listview_chat");
	auto t_time = dynamic_cast<Text *>(panel_chat->getChildByName("label_time"));
	auto w_other = dynamic_cast<Widget*>(panel_chat->getChildByName("panel_other"));
	auto w_own = dynamic_cast<Widget*>(panel_chat->getChildByName("panel_own"));
	auto button_send = dynamic_cast<Button *>(panel_chat->getChildByName("button_send"));
	auto image_input_bg = panel_chat->getChildByName<TextField*>("image_input_bg");
	l_chat->removeAllChildrenWithCleanup(true);

	auto f_w = w_own->getBoundingBox().size.width;
	auto p_h = t_time->getBoundingBox().size.height;
	if (m_bisFriend)
	{
		image_input_bg->setPosition(Vec2(34, 58));
		image_input_bg->setAnchorPoint(Vec2(0.0, 0.5));
		image_input_bg->setContentSize(Size(874, 72));
		image_chatlist_bg->setContentSize(Size(1104, 532));
		image_chatlist_bg->setPosition(Vec2(553, 268));
		image_div->setContentSize(Size(1044, 6));
		image_div->setPosition(Vec2(550.86, 106));
		l_chat->setContentSize(Size(1006, 395));
		t_time->setContentSize(Size(1006, 35));
		w_other->setContentSize(Size(1006, 50));
		w_own->setContentSize(Size(1006, 50));
		button_send->setPositionX(1000);
		f_w = w_own->getBoundingBox().size.width;
		p_h = t_time->getBoundingBox().size.height;
	}
	else
	{
		image_input_bg->setPosition(Vec2(298, 59));
		image_input_bg->setAnchorPoint(Vec2(0.5, 0.5));
		image_input_bg->setContentSize(Size(532, 72));
		image_chatlist_bg->setContentSize(Size(742, 532));
		image_chatlist_bg->setPosition(Vec2(372.43, 268));
		image_div->setContentSize(Size(682, 6));
		image_div->setPosition(Vec2(376.86, 107));
		l_chat->setContentSize(Size(664, 395));
		t_time->setContentSize(Size(650, 35));
		w_other->setContentSize(Size(650, 50));
		w_own->setContentSize(Size(650, 50));
		button_send->setPositionX(647);
		f_w = w_own->getBoundingBox().size.width;
		p_h = t_time->getBoundingBox().size.height;
	}

	std::string path;
	switch (m_nChannelType)
	{
	case 0:
	{
			  for (int i = 0; i < SINGLE_CHAT->m_public_name_zh.size(); i++)
			  {
				  if (strcmp(SINGLE_CHAT->m_public_content_zh[i].c_str(), "") == 0)
				  {
					  auto item = t_time->clone();
					  dynamic_cast<Text *>(item)->setString(SINGLE_CHAT->m_public_name_zh[i]);
					  l_chat->pushBackCustomItem(item);
				  }
				  else if (strcmp(SINGLE_CHAT->m_public_name_zh[i].c_str(), "") == 0)
				  {
					  std::string speaker;
					  std::string content;
					  RichElementText*  t_content;
					  RichElementText* t_speaker;
					  speaker = "[";
					  speaker += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_SYSTEM_NOTICE_ZH"];
					  speaker += "]:";
					  t_speaker = RichElementText::create(1, Color3B(46, 125, 50), 255, speaker, "Helvetica", 26, true);

					  content = SINGLE_CHAT->m_public_content_zh[i];
					  t_content = RichElementText::create(1, Color3B(46, 125, 50), 255, content, "Helvetica", 26, true);

					  auto mainChat = RichText::create();
					  mainChat->ignoreContentAdaptWithSize(false);
					  mainChat->setContentSize(Size(f_w, 26 + 10));
					  mainChat->pushBackElement(t_speaker);
					  mainChat->pushBackElement(t_content);
					  mainChat->setVerticalSpace(5.0f);
					  mainChat->formatText();
					  l_chat->pushBackCustomItem(mainChat);
				  }
				  else
				  {
					  std::string speaker;
					  std::string content;

					  int nation = SINGLE_CHAT->m_public_nation_zh[i];
					  if (nation > 0)
					  {
						  path = getCountryIconPath(nation);
					  }
					  else
					  {
						  path = GM_IOCN;
					  }
					  RichElementImage* i_nation = RichElementImage::create(1, Color3B(255, 255, 255), 0, path);
					  RichElementText*  t_content;
					  RichElementText* t_speaker;
					  if (strcmp(SINGLE_CHAT->m_public_name_zh[i].c_str(), SINGLE_HERO->m_sName.c_str()) == 0)
					  {
						  speaker = "[";
						  speaker += SINGLE_CHAT->m_public_name_zh[i];
						  speaker += "]:";
						  t_speaker = RichElementText::create(1, Color3B(0x4F, 0x4F, 0x4F), 255, speaker, "Helvetica", 26, true);

						  content = SINGLE_CHAT->m_public_content_zh[i];
						  t_content = RichElementText::create(1, Color3B(0x58, 0x58, 0x58), 255, content, "Helvetica", 26, true);
					  }
					  else
					  {
						  speaker = "[";
						  speaker += SINGLE_CHAT->m_public_name_zh[i];
						  speaker += "]:";
						  t_speaker = RichElementText::create(1, Color3B(0x00, 0x57, 0xCE), 255, speaker, "Helvetica", 26, true);

						  content = SINGLE_CHAT->m_public_content_zh[i];
						  t_content = RichElementText::create(1, Color3B(0x2E, 0x1D, 0x0E), 255, content, "Helvetica", 26, true);
					  }
					  auto mainChat = RichText::create();
					  mainChat->ignoreContentAdaptWithSize(false);
					  mainChat->setContentSize(Size(f_w, 26 + 10));
					  mainChat->pushBackElement(i_nation);
					  mainChat->pushBackElement(t_speaker);
					  mainChat->pushBackElement(t_content);
					  mainChat->setVerticalSpace(5.0f);
					  mainChat->formatText();
					  l_chat->pushBackCustomItem(mainChat);
				  }
			  }
			  break;
	}
	case 1:
	{
			  for (int i = 0; i < SINGLE_CHAT->m_public_name.size(); i++)
			  {
				  if (strcmp(SINGLE_CHAT->m_public_content[i].c_str(), "") == 0)
				  {
					  auto item = t_time->clone();
					  dynamic_cast<Text *>(item)->setString(SINGLE_CHAT->m_public_name[i]);
					  l_chat->pushBackCustomItem(item);
				  }
				  else if (strcmp(SINGLE_CHAT->m_public_name[i].c_str(), "") == 0)
				  {
					  std::string speaker;
					  std::string content;
					  RichElementText*  t_content;
					  RichElementText* t_speaker;
					  speaker = "[";
					  speaker += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_SYSTEM_NOTICE"];
					  speaker += "]:";
					  t_speaker = RichElementText::create(1, Color3B(46, 125, 50), 255, speaker, "Helvetica", 26, true);

					  content = SINGLE_CHAT->m_public_content[i];
					  t_content = RichElementText::create(1, Color3B(46, 125, 50), 255, content, "Helvetica", 26, true);

					  auto mainChat = RichText::create();
					  mainChat->ignoreContentAdaptWithSize(false);
					  mainChat->setContentSize(Size(f_w, 26 + 10));
					  mainChat->pushBackElement(t_speaker);
					  mainChat->pushBackElement(t_content);
					  mainChat->setVerticalSpace(5.0f);
					  mainChat->formatText();
					  l_chat->pushBackCustomItem(mainChat);
				  }
				  else
				  {
					  std::string speaker;
					  std::string content;

					  int nation = SINGLE_CHAT->m_public_nation[i];
					  if (nation > 0)
					  {
						  path = getCountryIconPath(nation);
					  }
					  else
					  {
						  path = GM_IOCN;
					  }
					  RichElementImage* i_nation = RichElementImage::create(1, Color3B(255, 255, 255), 0, path);
					  RichElementText*  t_content;
					  RichElementText* t_speaker;
					  if (strcmp(SINGLE_CHAT->m_public_name[i].c_str(), SINGLE_HERO->m_sName.c_str()) == 0)
					  {
						  speaker = "[";
						  speaker += SINGLE_CHAT->m_public_name[i];
						  speaker += "]:";
						  t_speaker = RichElementText::create(1, Color3B(0x4F, 0x4F, 0x4F), 255, speaker, "Helvetica", 26, true);

						  content = SINGLE_CHAT->m_public_content[i];
						  t_content = RichElementText::create(1, Color3B(0x58, 0x58, 0x58), 255, content, "Helvetica", 26, true);
					  }
					  else
					  {
						  speaker = "[";
						  speaker += SINGLE_CHAT->m_public_name[i];
						  speaker += "]:";
						  t_speaker = RichElementText::create(1, Color3B(0x00, 0x57, 0xCE), 255, speaker, "Helvetica", 26, true);

						  content = SINGLE_CHAT->m_public_content[i];
						  t_content = RichElementText::create(1, Color3B(0x2E, 0x1D, 0x0E), 255, content, "Helvetica", 26, true);
					  }
					  auto mainChat = RichText::create();
					  mainChat->ignoreContentAdaptWithSize(false);
					  mainChat->setContentSize(Size(f_w, 26 + 10));
					  mainChat->pushBackElement(i_nation);
					  mainChat->pushBackElement(t_speaker);
					  mainChat->pushBackElement(t_content);
					  mainChat->setVerticalSpace(5.0f);
					  mainChat->formatText();
					  l_chat->pushBackCustomItem(mainChat);
				  }
			  }
			  break;
	}
	case 2:
	{
			  for (int i = 0; i < SINGLE_CHAT->m_country_name.size(); i++)
			  {
				  if (strcmp(SINGLE_CHAT->m_country_content[i].c_str(), "") == 0)
				  {
					  auto item = t_time->clone();
					  dynamic_cast<Text *>(item)->setString(SINGLE_CHAT->m_country_name[i]);
					  l_chat->pushBackCustomItem(item);
				  }
				  else
				  {
					  std::string speaker;
					  std::string content;
					  int nation = SINGLE_CHAT->m_country_nation[i];
					  if (nation > 0)
					  {
						  path = getCountryIconPath(nation);
					  }
					  else
					  {
						  path = GM_IOCN;
					  }
					  RichElementImage* i_nation = RichElementImage::create(1, Color3B(255, 255, 255), 0, path);
					  RichElementText*  t_content;
					  RichElementText* t_speaker;
					  if (strcmp(SINGLE_CHAT->m_country_name[i].c_str(), SINGLE_HERO->m_sName.c_str()) == 0)
					  {
						  speaker = "[";
						  speaker += SINGLE_CHAT->m_country_name[i];
						  speaker += "]:";
						  t_speaker = RichElementText::create(1, Color3B(0x4F, 0x4F, 0x4F), 255, speaker, "Helvetica", 26, true);
						  content += SINGLE_CHAT->m_country_content[i];
						  t_content = RichElementText::create(1, Color3B(0x58, 0x58, 0x58), 255, content, "Helvetica", 26, true);
					  }
					  else
					  {
						  speaker = "[";
						  speaker += SINGLE_CHAT->m_country_name[i];
						  speaker += "]:";
						  t_speaker = RichElementText::create(1, Color3B(0x00, 0x57, 0xCE), 255, speaker, "Helvetica", 26, true);
						  content += SINGLE_CHAT->m_country_content[i];
						  t_content = RichElementText::create(1, Color3B(0x2E, 0x1D, 0x0E), 255, content, "Helvetica", 26, true);
					  }
					  auto mainChat = RichText::create();
					  mainChat->ignoreContentAdaptWithSize(false);
					  mainChat->setContentSize(Size(f_w, 26 + 10));
					  mainChat->pushBackElement(i_nation);
					  mainChat->pushBackElement(t_speaker);
					  mainChat->pushBackElement(t_content);
					  mainChat->setVerticalSpace(5.0f);
					  mainChat->formatText();
					  l_chat->pushBackCustomItem(mainChat);
				  }
			  }
			  break;
	}
	case 3:
	{
			  for (int i = 0; i < SINGLE_CHAT->m_zone_name.size(); i++)
			  {
				  if (strcmp(SINGLE_CHAT->m_zone_content[i].c_str(), "") == 0)
				  {
					  auto item = t_time->clone();
					  dynamic_cast<Text *>(item)->setString(SINGLE_CHAT->m_zone_name[i].c_str());
					  l_chat->pushBackCustomItem(item);
				  }
				  else
				  {
					  std::string speaker;
					  std::string content;
					  int nation = SINGLE_CHAT->m_zone_nation[i];
					  if (nation > 0)
					  {
						  path = getCountryIconPath(nation);
					  }
					  else
					  {
						  path = GM_IOCN;
					  }
					  RichElementImage* i_nation = RichElementImage::create(1, Color3B(255, 255, 255), 0, path);
					  RichElementText*  t_content;
					  RichElementText* t_speaker;
					  if (strcmp(SINGLE_CHAT->m_zone_name[i].c_str(), SINGLE_HERO->m_sName.c_str()) == 0)
					  {

						  speaker = "[";
						  speaker += SINGLE_CHAT->m_zone_name[i];
						  speaker += "]:";
						  t_speaker = RichElementText::create(1, Color3B(0x4F, 0x4F, 0x4F), 255, speaker, "Helvetica", 26, true);
						  content = SINGLE_CHAT->m_zone_content[i];
						  t_content = RichElementText::create(1, Color3B(0x58, 0x58, 0x58), 255, content, "Helvetica", 26, true);
					  }
					  else
					  {
						  speaker = "[";
						  speaker += SINGLE_CHAT->m_zone_name[i];
						  speaker += "]:";
						  t_speaker = RichElementText::create(1, Color3B(0x00, 0x57, 0xCE), 255, speaker, "Helvetica", 26, true);
						  content += SINGLE_CHAT->m_zone_content[i];
						  t_content = RichElementText::create(1, Color3B(0x2E, 0x1D, 0x0E), 255, content, "Helvetica", 26, true);
					  }
					  auto mainChat = RichText::create();
					  mainChat->ignoreContentAdaptWithSize(false);
					  mainChat->setContentSize(Size(f_w, 26 + 10));
					  mainChat->pushBackElement(i_nation);
					  mainChat->pushBackElement(t_speaker);
					  mainChat->pushBackElement(t_content);
					  mainChat->setVerticalSpace(5.0f);
					  mainChat->formatText();
					  l_chat->pushBackCustomItem(mainChat);
				  }
			  }
			  break;
	}
	case  4:
	{
			   for (int i = 0; i < SINGLE_CHAT->m_guild_name.size(); i++)
			   {
				   if (strcmp(SINGLE_CHAT->m_guild_content[i].c_str(), "") == 0)
				   {
					   auto item = t_time->clone();
					   dynamic_cast<Text *>(item)->setString(SINGLE_CHAT->m_guild_name[i].c_str());
					   l_chat->pushBackCustomItem(item);
				   }
				   else if (strcmp(SINGLE_CHAT->m_guild_name[i].c_str(), "") == 0)
				   {
					   std::string speaker;
					   std::string content;
					   RichElementText*  t_content;
					   RichElementText* t_speaker;
					   speaker = "[";
					   speaker += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_GUILD_NOTICE"];
					   speaker += "]:";
					   t_speaker = RichElementText::create(1, Color3B(46, 125, 50), 255, speaker, "Helvetica", 26, true);

					   content = SINGLE_CHAT->m_guild_content[i];
					   t_content = RichElementText::create(1, Color3B(46, 125, 50), 255, content, "Helvetica", 26, true);

					   auto mainChat = RichText::create();
					   mainChat->ignoreContentAdaptWithSize(false);
					   mainChat->setContentSize(Size(f_w, 26 + 10));
					   mainChat->pushBackElement(t_speaker);
					   mainChat->pushBackElement(t_content);
					   mainChat->setVerticalSpace(5.0f);
					   mainChat->formatText();
					   l_chat->pushBackCustomItem(mainChat);
				   }
				   else
				   {
					   std::string speaker;
					   std::string content;
					   int nation = SINGLE_CHAT->m_guild_nation[i];
					   if (nation > 0)
					   {
						   path = getCountryIconPath(nation);
					   }
					   else
					   {
						   path = GM_IOCN;
					   }
					   RichElementImage* i_nation = RichElementImage::create(1, Color3B(255, 255, 255), 0, path);
					   RichElementText*  t_content;
					   RichElementText* t_speaker;
					   if (strcmp(SINGLE_CHAT->m_guild_name[i].c_str(), SINGLE_HERO->m_sName.c_str()) == 0)
					   {
						   speaker = "[";
						   speaker += SINGLE_CHAT->m_guild_name[i];
						   speaker += "]:";
						   t_speaker = RichElementText::create(1, Color3B(0x4F, 0x4F, 0x4F), 255, speaker, "Helvetica", 26, true);
						   content = SINGLE_CHAT->m_guild_content[i];
						   t_content = RichElementText::create(1, Color3B(0x58, 0x58, 0x58), 255, content, "Helvetica", 26, true);
					   }
					   else
					   {
						   speaker = "[";
						   speaker += SINGLE_CHAT->m_guild_name[i];
						   speaker += "]:";
						   t_speaker = RichElementText::create(1, Color3B(0x00, 0x57, 0xCE), 255, speaker, "Helvetica", 26, true);
						   content = SINGLE_CHAT->m_guild_content[i];
						   t_content = RichElementText::create(1, Color3B(0x2E, 0x1D, 0x0E), 255, content, "Helvetica", 26, true);
					   }
					   auto mainChat = RichText::create();
					   mainChat->ignoreContentAdaptWithSize(false);
					   mainChat->setContentSize(Size(f_w, 26 + 10));
					   mainChat->pushBackElement(i_nation);
					   mainChat->pushBackElement(t_speaker);
					   mainChat->pushBackElement(t_content);
					   mainChat->setVerticalSpace(5.0f);
					   mainChat->formatText();
					   l_chat->pushBackCustomItem(mainChat);
				   }
			   }
			   break;
	}
	default:
	{
			   for (int i = 0; i < SINGLE_CHAT->m_private_name[((Button *)m_pPreChannelBtn)->getTitleText()].size(); i++)
			   {
				   if (strcmp(SINGLE_CHAT->m_private_content[((Button *)m_pPreChannelBtn)->getTitleText()][i].c_str(), "") == 0)
				   {
					   auto item = t_time->clone();
					   dynamic_cast<Text *>(item)->setString(SINGLE_CHAT->m_private_name[((Button *)m_pPreChannelBtn)->getTitleText()][i]);
					   l_chat->pushBackCustomItem(item);
				   }
				   else
				   {
					   std::string speaker;
					   std::string content;
					   int nation = SINGLE_CHAT->m_private_nation[((Button *)m_pPreChannelBtn)->getTitleText()][i];
					   if (nation > 0)
					   {
						   path = getCountryIconPath(nation);
					   }
					   else
					   {
						   path = GM_IOCN;
					   }
					   RichElementImage* i_nation = RichElementImage::create(1, Color3B(255, 255, 255), 0, path);
					   RichElementText*  t_content;
					   RichElementText* t_speaker;
					   if (strcmp(SINGLE_CHAT->m_private_name[((Button *)m_pPreChannelBtn)->getTitleText()][i].c_str(), SINGLE_HERO->m_sName.c_str()) == 0)
					   {
						   speaker = "[";
						   speaker += SINGLE_CHAT->m_private_name[((Button *)m_pPreChannelBtn)->getTitleText()][i];
						   speaker += "]:";
						   t_speaker = RichElementText::create(1, Color3B(0x4F, 0x4F, 0x4F), 255, speaker, "Helvetica", 26, true);
						   content = SINGLE_CHAT->m_private_content[((Button *)m_pPreChannelBtn)->getTitleText()][i];
						   t_content = RichElementText::create(1, Color3B(0x58, 0x58, 0x58), 255, content, "Helvetica", 26, true);
					   }
					   else
					   {
						   speaker = "[";
						   speaker += SINGLE_CHAT->m_private_name[((Button *)m_pPreChannelBtn)->getTitleText()][i];
						   speaker += "]:";
						   t_speaker = RichElementText::create(1, Color3B(0x00, 0x57, 0xCE), 255, speaker, "Helvetica", 26, true);
						   content = SINGLE_CHAT->m_private_content[((Button *)m_pPreChannelBtn)->getTitleText()][i];
						   t_content = RichElementText::create(1, Color3B(0x2E, 0x1D, 0x0E), 255, content, "Helvetica", 26, true);
					   }
					   auto mainChat = RichText::create();
					   mainChat->ignoreContentAdaptWithSize(false);
					   mainChat->setContentSize(Size(f_w, 26 + 10));
					   mainChat->pushBackElement(i_nation);
					   mainChat->pushBackElement(t_speaker);
					   mainChat->pushBackElement(t_content);
					   mainChat->setVerticalSpace(5.0f);
					   mainChat->formatText();
					   l_chat->pushBackCustomItem(mainChat);
				   }
			   }
			   break;
	}
	}

	l_chat->jumpToBottom();
	auto image_pulldown = panel_chat->getChildByName<ImageView*>("image_pulldown_1");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_chat, image_pulldown);
	l_chat->jumpToPercentVertical(100);
	this->setCameraMask(_cameraMask, true);

	if (m_bisFriend)
	{
		image_pulldown->setPosition(Vec2(1055, 316));
	}
	else
	{
		image_pulldown->setPosition(Vec2(735, 316));
	}
}
void UISocial::flushChatFirendList()
{
	auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	if (!panel_chat)
	{
		return;
	}
	else
	{
		if (m_nChannelType == -1)
		{
			if (SINGLE_HERO->m_iGuildId)
			{
				m_nChannelType = 4;
			}
			else
			{
				m_nChannelType = 3;
			}
		}
	}
	auto panel_playerlist = panel_chat->getChildByName<Layout*>("panel_playerlist");
	auto playerlist = panel_playerlist->getChildByName<ListView*>("listview_chat_playerlist");
	playerlist->setVisible(true);
	playerlist->removeAllChildrenWithCleanup(true);
	auto t_speakers = panel_playerlist->getChildByName<Text*>("label_latest_speakers");
	auto image_div = panel_playerlist->getChildByName<ImageView*>("image_div");
	auto image_pulldown = panel_playerlist->getChildByName<ImageView*>("image_pulldown_2");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	auto b_list = panel_chat->getChildByName<Button*>("button_chat");
	auto i_player = panel_chat->getChildByName<Widget*>("panel_player");
	i_player->addTouchEventListener(CC_CALLBACK_2(UISocial::chatEvent, this));
	auto b_more = i_player->getChildByName<Button*>("button_more");
	b_more->addTouchEventListener(CC_CALLBACK_2(UISocial::chatEvent, this));
	b_list->addTouchEventListener(CC_CALLBACK_2(UISocial::chatEvent, this));
	std::string path;
	if (m_nChannelType < 5)
	{
		t_speakers->setVisible(true);
		image_div->setVisible(true);
		playerlist->setPositionY(1);
		int min_grid = MIN(m_vHeroListMsg[m_nChannelType].size(), CHAT_LIST_PLAYER);
		for (int i = 0; i < min_grid; i++)
		{
			if (m_vHeroListMsg[m_nChannelType][i].nation > 0)
			{
				path = getCountryIconPath(m_vHeroListMsg[m_nChannelType][i].nation);
			}
			else
			{
				path = GM_IOCN;
			}

			auto item_clone = i_player->clone();
			item_clone->setVisible(true);
			auto t_name = item_clone->getChildByName<Text*>("label_name");
			auto i_flag = item_clone->getChildByName<ImageView*>("image_flag");
			i_flag->ignoreContentAdaptWithSize(false);

			item_clone->setTag(m_vHeroListMsg[m_nChannelType][i].cid);
			t_name->setString(m_vHeroListMsg[m_nChannelType][i].name);
			item_clone->setName(m_vHeroListMsg[m_nChannelType][i].name);
			i_flag->loadTexture(path);
			playerlist->pushBackCustomItem(item_clone);
		}
	}
	else
	{
		t_speakers->setVisible(false);
		image_div->setVisible(false);
		playerlist->setPositionY(66);
		//私聊时只显示对方信息
		int index;
		if (m_vHeroMsg.empty())
		{
			index = -1;
		}
		else
		{
			if (m_vHeroMsg[0].cid == SINGLE_HERO->m_iID && m_vHeroMsg.size() > 1)
			{
				index = 1;
			}
			else
			{
				index = 0;
			}
			if (m_vHeroMsg[index].name != m_vPrivateName.at(m_nChannelType - 5))
			{
				index = -1;
			}
		}

		auto item_clone = i_player->clone();
		item_clone->setVisible(true);
		auto t_name = item_clone->getChildByName<Text*>("label_name");
		auto i_flag = item_clone->getChildByName<ImageView*>("image_flag");
		i_flag->ignoreContentAdaptWithSize(false);
		//对方不在线时显示自己（暂时处理）
		if (index == -1)
		{

			if (SINGLE_HERO->m_bGameMaster)
			{
				path = GM_IOCN;
			}
			else
			{
				path = getCountryIconPath(SINGLE_HERO->m_iNation);
			}
			item_clone->setTag(SINGLE_HERO->m_iID);
			t_name->setString(SINGLE_HERO->m_sName);
			item_clone->setName(SINGLE_HERO->m_sName);
			i_flag->loadTexture(path);
		}
		else
		{

			if (m_vHeroMsg[index].nation > 0)
			{
				path = getCountryIconPath(m_vHeroMsg[index].nation);
			}
			else
			{
				path = GM_IOCN;
			}
			item_clone->setTag(m_vHeroMsg[index].cid);
			t_name->setString(m_vHeroMsg[index].name);
			item_clone->setName(m_vHeroMsg[index].name);
			i_flag->loadTexture(path);
		}

		playerlist->pushBackCustomItem(item_clone);
	}
	image_pulldown->setPosition(Vec2(350, 256));
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(playerlist, image_pulldown);
	playerlist->refreshView();
	this->setCameraMask(_cameraMask, true);

	if (m_bisFriend)
	{
		panel_playerlist->setVisible(false);
		b_list->setBright(false);
		b_list->setTouchEnabled(true);
		b_list->setPosition(Vec2(1187, 364));
	}
	else
	{
		b_list->setPosition(Vec2(828, 364));
		b_list->setBright(true);
		b_list->setTouchEnabled(true);
		panel_playerlist->setPosition(Vec2(834, 66));
		panel_playerlist->setContentSize(Size(374, 524));
		panel_playerlist->setVisible(true);
	}
}

void UISocial::chatButtonEvent(Ref *pSender, ScrollviewEventType type)
{
	if (type == SCROLLVIEW_EVENT_SCROLLING)
	{
		auto l_chat = dynamic_cast<ListView*>(pSender);
		auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
		auto i_left_more = panel_chat->getChildByName<ImageView*>("image_chat_more_left");
		auto i_right_more = panel_chat->getChildByName<ImageView*>("image_chat_more_right");
		auto i_left = panel_chat->getChildByName<ImageView*>("image_left");
		auto i_right = panel_chat->getChildByName<ImageView*>("image_right");

		int n = l_chat->getInnerContainer()->getPositionX() * 100 / (l_chat->getContentSize().width - l_chat->getInnerContainer()->getContentSize().width);
		if (l_chat->getChildrenCount() == 4)
		{
			i_left_more->setVisible(false);
			i_right_more->setVisible(false);
			i_left->setVisible(false);
			i_right->setVisible(false);
			return;
		}
		if (n == 0)
		{
			i_left_more->setVisible(false);
			i_right_more->setVisible(true);
			i_left->setVisible(false);
			i_right->setVisible(true);
		}
		else if (n == 100)
		{
			i_left_more->setVisible(true);
			i_right_more->setVisible(false);
			i_left->setVisible(true);
			i_right->setVisible(false);
		}
		else
		{
			i_left_more->setVisible(true);
			i_right_more->setVisible(true);
			i_left->setVisible(true);
			i_right->setVisible(true);
		}
	}
}

void UISocial::flushChatButton()
{
	auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	if (!panel_chat)
	{
		return;
	}
	auto l_chat = panel_chat->getChildByName<ListView*>("listView_chatbutton");
	l_chat->addEventListenerScrollView(this, scrollvieweventselector(UISocial::chatButtonEvent));
	auto b_item = panel_chat->getChildByName<Button*>("button_chat_private");
	b_item->addTouchEventListener(CC_CALLBACK_2(UISocial::chatEvent, this));
	for (size_t i = l_chat->getChildrenCount(); i > 0; i--)
	{
		auto tag = l_chat->getItem(i - 1)->getTag();
		if (tag <= 4)
		{
			auto b_chat_1 = dynamic_cast<Button*>(l_chat->getItem(i - 1));
			b_chat_1->addTouchEventListener(CC_CALLBACK_2(UISocial::chatEvent, this));
			b_chat_1->setTag(tag);
			b_chat_1->setTitleColor(Color3B(141, 114, 71));
			b_chat_1->setBright(true);
		}
		else
		{
			l_chat->removeItem(i - 1);
		}
	}

	for (size_t i = 0; i < m_vPrivateName.size(); i++)
	{
		auto b_chat_private = (Button*)(b_item->clone());
		b_chat_private->setTag(i + 5);
		b_chat_private->setTitleText(m_vPrivateName[i]);
		b_chat_private->setTitleFontName("");
		b_chat_private->setTitleColor(Color3B(141, 114, 71));
		auto b_chatClose = b_chat_private->getChildByName<Button*>("button_chat_close");
		b_chatClose->addTouchEventListener(CC_CALLBACK_2(UISocial::menuCall_func, this));
		b_chatClose->setVisible(true);
		b_chatClose->setTag(i + 5);
		l_chat->insertCustomItem(b_chat_private, 0);
	}
	setGuildChatButton();
	for (size_t i = 0; i < l_chat->getChildrenCount(); i++)
	{
		if (l_chat->getItem(i)->getTag() == m_nChannelType)
		{
			m_pPreChannelBtn = l_chat->getItem(i);
		}
	}
	m_pPreChannelBtn->setBright(false);
	((Button*)m_pPreChannelBtn)->setTitleColor(Color3B(241, 200, 129));
	flushChatLifeAndRight(m_nChannelType);
	getLatestSpeakers(0);
}

void UISocial::getLatestSpeakers(const float fTime)
{
	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	if (view)
	{
		auto playerlist = view->getChildByName<ListView*>("listview_chat_playerlist");
		playerlist->setVisible(false);
		flushChatFirendList();
		if (m_nChannelType < 5)
		{
			ProtocolThread::GetInstance()->listChannelUsers(m_nChannelType);
		}
		else
		{
			ProtocolThread::GetInstance()->getPrivateUserInfo((char*)m_vPrivateName[m_nChannelType - 5].data());
		}
	}
}

void UISocial::addPublic_zh(std::string name, std::string content, int nation, bool is_notice)
{
	int64_t curTime = getCurrentTimeUsev() / 1000;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	if (strcmp(name.data(), SINGLE_HERO->m_sName.data()) != 0)
	{
		if (m_nLastTime.at(0) < 0)
		{
			SINGLE_CHAT->addPublic_zh(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
		}
		SINGLE_CHAT->addPublic_zh(name, content, nation);
		if (m_nChannelType != 0 && !is_notice)
		{
			SINGLE_CHAT->m_public_unread_falg_zh = true;
		}
	}
	if (m_nChannelType == 0)
	{
		flushChatList();
	}
	m_nLastTime.at(0) = CHAT_TIME_INTERVAL;
}

void UISocial::addPublic(std::string name, std::string content, int nation, bool is_notice)
{
	int64_t curTime = getCurrentTimeUsev() / 1000;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	if (strcmp(name.data(), SINGLE_HERO->m_sName.data()) != 0)
	{
		if (m_nLastTime.at(1) < 0)
		{
			SINGLE_CHAT->addPublic(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
		}
		SINGLE_CHAT->addPublic(name, content, nation);
		if (m_nChannelType != 1 && !is_notice)
		{
			SINGLE_CHAT->m_public_unread_falg = true;
		}
	}
	if (m_nChannelType == 1)
	{
		flushChatList();
	}
	m_nLastTime.at(1) = CHAT_TIME_INTERVAL;
}

void UISocial::addCountry(std::string name, std::string content, int nation)
{
	int64_t curTime = getCurrentTimeUsev() / 1000;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	if (strcmp(name.data(), SINGLE_HERO->m_sName.data()) != 0)
	{
		if (m_nLastTime.at(2) < 0)
		{
			SINGLE_CHAT->addCountry(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
		}
		SINGLE_CHAT->addCountry(name, content, nation);
		if (m_nChannelType != 2)
		{
			SINGLE_CHAT->m_country_unread_falg = true;
		}
	}
	if (m_nChannelType == 2)
	{
		flushChatList();
	}
	m_nLastTime.at(2) = CHAT_TIME_INTERVAL;
}

void UISocial::addZone(std::string name, std::string content, int nation)
{
	int64_t curTime = getCurrentTimeUsev() / 1000;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	if (strcmp(name.data(), SINGLE_HERO->m_sName.data()) != 0)
	{
		if (m_nLastTime.at(3) < 0)
		{
			SINGLE_CHAT->addZone(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
		}
		SINGLE_CHAT->addZone(name, content, nation);
		if (m_nChannelType != 3)
		{
			SINGLE_CHAT->m_zone_unread_falg = true;
		}
	}

	if (m_nChannelType == 3)
	{
		flushChatList();
	}

	m_nLastTime.at(3) = CHAT_TIME_INTERVAL;
}

void UISocial::addGuild(std::string name, std::string content, int nation, bool is_notice)
{
	int64_t curTime = getCurrentTimeUsev() / 1000;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	if (strcmp(name.data(), SINGLE_HERO->m_sName.data()) != 0)
	{
		if (m_nLastTime.at(4) < 0)
		{
			SINGLE_CHAT->addGuild(String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
		}
		SINGLE_CHAT->addGuild(name, content, nation);
		if (m_nChannelType != 4 && !is_notice)
		{
			SINGLE_CHAT->m_guild_unread_falg = true;
		}
	}

	if (m_nChannelType == 4)
	{
		flushChatList();
	}
	m_nLastTime.at(4) = CHAT_TIME_INTERVAL;
}

void UISocial::addPrivate(int id, std::string name, std::string content, int nation)
{
	int64_t curTime = getCurrentTimeUsev() / 1000;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	if (strcmp(name.data(), SINGLE_HERO->m_sName.data()) != 0)
	{
		if (m_nChannelType > m_nLastTime.size() || m_nLastTime.at(m_nChannelType) < 0)
		{
			SINGLE_CHAT->addPrivate(name, String::createWithFormat("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec)->_string, "", 0);
		}
		SINGLE_CHAT->addPrivate(name, name, content, nation);
	}

	bool isfind = false;
	size_t i = 0;
	for (; i < m_vPrivateName.size(); i++)
	{
		if (m_vPrivateName[i] == name)
		{
			isfind = true;
			break;
		}
	}

	if (isfind)
	{
		m_nLastTime.at(i + 5) = CHAT_TIME_INTERVAL;
		flushChatButton();
		if (m_nChannelType - 5 == i)
		{
			flushChatList();
		}
	}
	else
	{
		m_nLastTime.push_back(CHAT_TIME_INTERVAL);
		m_vPrivateName.push_back(name);
		flushChatButton();
	}

	if ((m_nChannelType > 4 && strcmp(name.data(), m_vPrivateName[m_nChannelType - 5].data()) != 0) || m_nChannelType <= 4)
	{
		SINGLE_CHAT->m_private_unread_falg[name] = true;
	}
}

void UISocial::flushChatLifeAndRight(int nIndex)
{
	auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	auto l_chat = panel_chat->getChildByName<ListView*>("listView_chatbutton");
	auto i_left_more = panel_chat->getChildByName<ImageView*>("image_chat_more_left");
	auto i_right_more = panel_chat->getChildByName<ImageView*>("image_chat_more_right");
	auto i_left = panel_chat->getChildByName<ImageView*>("image_left");
	auto i_right = panel_chat->getChildByName<ImageView*>("image_right");
	int total = l_chat->getChildrenCount();

	int n_gird = 4;
	LanguageType nType = LanguageType(Utils::getLanguage());
	if (nType == cocos2d::LanguageType::ENGLISH)
	{
		n_gird = 4;
	}
	else
	{
		n_gird = 5;
		nIndex++;
	}

	if (total > n_gird && nIndex > n_gird)
	{
		nIndex -= n_gird;
	}
	else if (total > n_gird && nIndex <= n_gird)
	{
		nIndex += total - n_gird;
	}

	float temp = 100.0 * nIndex / total;
	l_chat->refreshView();
	l_chat->jumpToPercentHorizontal(temp);
	if (nIndex == total - n_gird)
	{
		temp = 0;
		l_chat->jumpToPercentHorizontal(temp);
	}

	if (nIndex <= 5 && nType != cocos2d::LanguageType::ENGLISH)
	{
		if (nIndex - 1 == total - n_gird)
		{
			temp = 0;
			l_chat->jumpToPercentHorizontal(temp);
		}
	}

	if (total == n_gird && nType == cocos2d::LanguageType::ENGLISH)
	{
		i_left_more->setVisible(false);
		i_right_more->setVisible(false);
		i_left->setVisible(false);
		i_right->setVisible(false);
	}
	else if (temp <= 0)
	{
		i_left_more->setVisible(false);
		i_right_more->setVisible(true);
		i_left->setVisible(false);
		i_right->setVisible(true);
	}
	else if (temp >= 98)
	{
		i_left_more->setVisible(true);
		i_right_more->setVisible(false);
		i_right->setVisible(false);
		i_left->setVisible(true);
	}
	else
	{
		i_left_more->setVisible(true);
		i_right_more->setVisible(true);
		i_right->setVisible(true);
		i_left->setVisible(true);
	}
}

void UISocial::setChatCall(std::string name)
{
	if (!ProtocolThread::GetInstance()->isChatServerConnected())
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_CHAT_SERVER_NOT_CONNECTION");
		return;
	}
	openView(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	LanguageType nType = LanguageType(Utils::getLanguage());
	auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	auto l_chat = panel_chat->getChildByName<ListView*>("listView_chatbutton");
	if (nType == cocos2d::LanguageType::ENGLISH)
	{
		l_chat->removeItem(0);
	}

	m_bSaveHeromsg = true;

	m_pMainButton = nullptr;
	if (name != "")
	{
		bool isfind = false;
		for (size_t i = 0; i < m_vPrivateName.size(); i++)
		{
			if (m_vPrivateName[i] == name)
			{
				isfind = true;
				m_nChannelType = i + 5;
				SINGLE_CHAT->m_private_unread_falg[m_vPrivateName[m_nChannelType - 5].data()] = false;
				break;
			}
		}

		if (!isfind)
		{
			m_vPrivateName.push_back(name);
			m_nChannelType = m_vPrivateName.size() + 4;
			m_nLastTime.push_back(-1);
		}
	}
	else
	{
		m_nChannelType = m_nLastChannelType;
		switch (m_nChannelType)
		{
		case 0:
			SINGLE_CHAT->m_public_unread_falg_zh = false;
			break;
		case 1:
			SINGLE_CHAT->m_public_unread_falg = false;
			break;
		case 2:
			SINGLE_CHAT->m_country_unread_falg = false;
			break;
		case 3:
			SINGLE_CHAT->m_zone_unread_falg = false;
			break;
		case 4:
			SINGLE_CHAT->m_guild_unread_falg = false;
			break;
		default:
			if (m_nChannelType - 5 < m_vPrivateName.size())
			{
				SINGLE_CHAT->m_private_unread_falg[m_vPrivateName[m_nChannelType - 5].data()] = false;
			}
			break;
		}
	}
	flushChatButton();
	flushChatList();
	auto t_chat = panel_chat->getChildByName<TextField*>("textfield_chat");
	t_chat->setVisible(false);
	ui::EditBox* edit_chat = nullptr;
	if (!edit_chat)
	{
		edit_chat = ui::EditBox::create(Size(t_chat->getContentSize()), "input.png");
		edit_chat->setTag(1);
		edit_chat->setPosition(t_chat->getPosition());
		t_chat->getParent()->addChild(edit_chat);
		edit_chat->setDelegate(this);
		edit_chat->setAnchorPoint(t_chat->getAnchorPoint());
		edit_chat->setPlaceholderFontColor(Color3B(255, 255, 255));
		edit_chat->setFontColor(Color3B(255, 255, 255));
		edit_chat->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
		edit_chat->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		edit_chat->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	}
}

void UISocial::setFirendCall()
{
	openView(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	m_pMainButton = nullptr;
	changeMainButton(nullptr);
	m_nPageIndex = PAGE_MY_FRIEND;
	ProtocolThread::GetInstance()->getFriendsList(UILoadingIndicator::create(this));

	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto t_title = view->getChildByName<Text*>("label_title");
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_FIREND_TITLE"]);
	auto p_no = view->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(false);
}

void UISocial::setGuildChatButton()
{
	auto panel_chat = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	auto b_guildChat = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_chat, "button_chat_gulid"));
	if (SINGLE_HERO->m_iGuildId)
	{
		b_guildChat->setTouchEnabled(true);
		b_guildChat->setTitleColor(Color3B(141, 114, 71));
	}
	else
	{
		b_guildChat->setTouchEnabled(false);
		b_guildChat->setTitleColor(Color3B(47, 40, 25));
	}
}

void UISocial::showSearchUserList(SearchUserByNameResult *searchResult)
{
	openView(SOCIAL_COCOS_RES[SOCIAL_SEARCH_RESULT_CSB]);
	auto p_search_friend = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_SEARCH_RESULT_CSB]);
	auto b_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_search_friend, "panel_friend"));
	auto w_no = p_search_friend->getChildByName<Widget*>("panel_search_no");
	auto l_friend = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_search_friend, "listview_friend"));
	l_friend->removeAllChildrenWithCleanup(true);
	w_no->setVisible(false);
	auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_search_friend, "image_pulldown"));
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (searchResult->n_friends < 1)
	{
		w_no->setVisible(true);
		image_pulldown->setVisible(false);
		return;
	}
	auto b_add = b_item->getChildByName<Button*>("button_add_friend");
	b_item->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));

	for (int i = 0; i < searchResult->n_friends; i++)
	{
		auto item = b_item->clone();
		auto img_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_head"));
		auto img_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_flag"));
		auto t_name = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_player_name"));
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_lv"));

		item->setTag(searchResult->friends[i]->characterid);
		img_icon->ignoreContentAdaptWithSize(false);
		img_icon->loadTexture(getPlayerIconPath(searchResult->friends[i]->iconidx));
		img_flag->ignoreContentAdaptWithSize(false);
		img_flag->loadTexture(getCountryIconPath(searchResult->friends[i]->nation));
		t_name->setString(searchResult->friends[i]->heroname);
		t_lv->setString(String::createWithFormat("Lv. %ld", searchResult->friends[i]->level)->_string);

		l_friend->pushBackCustomItem(item);

		auto b_add = item->getChildByName<Button*>("button_add_friend");
		b_add->setTag(searchResult->friends[i]->status);

		auto i_add = b_add->getChildByName<ImageView*>("image_add");
		if (searchResult->friends[i]->status == 0 || searchResult->friends[i]->status == 1)
		{
			b_add->setBright(false);
			setGLProgramState(i_add, true);
		}
		else
		{
			b_add->setBright(true);
			setGLProgramState(i_add, false);
		}
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_friend, image_pulldown);
}

void UISocial::update(float delta)
{
	std::string html_color_begin;
	std::string html_color_end;
#if WIN32
	html_color_begin = "";
	html_color_end = "";
#else
	html_color_begin = "<html><font size='24' color='#2E1D0E'>";
	html_color_end = "</font></html>";
#endif
	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_SEARCH_CSB]);
	if (view)
	{
		auto t_search = view->getChildByName<TextField*>("textfield_friend_name");
		ui::EditBox* edit_search = dynamic_cast<ui::EditBox*>(t_search->getParent()->getChildByTag(1));
		std::string search = edit_search->getText();
		if (search.size()>0)
		{
			edit_search->setOpacity(255);
		}
		else
		{
			edit_search->setOpacity(127);
		}

		if (illegal_character_check(search))
		{
			edit_search->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
	}
	else
	{
		view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
		if (view)
		{
			auto t_chat = view->getChildByName<TextField*>("textfield_chat");
			ui::EditBox* edit_chat = dynamic_cast<ui::EditBox*>(t_chat->getParent()->getChildByTag(1));
			std::string  chat = edit_chat->getText();
			if (chat.size() >0)
			{
				edit_chat->setOpacity(255);
				std::string new_vaule = "[";
				std::string old_vaule = "<";
				repalce_all_ditinct(chat, old_vaule, new_vaule);
				new_vaule = "]";
				old_vaule = ">";
				repalce_all_ditinct(chat, old_vaule, new_vaule);
				edit_chat->setText(chat.c_str());
			}
			else
			{
				edit_chat->setOpacity(127);
			}
		}
	}

	//聊天未读标记处理
	{
		view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
		if (view)
		{
			auto l_chat = view->getChildByName<ListView*>("listView_chatbutton");
			for (size_t i = 0; i < l_chat->getChildrenCount(); i++)
			{
				auto item_button = l_chat->getItem(i);
				auto i_unread = item_button->getChildByName<ImageView*>("image_unread");
				bool unread_falg = false;
				switch (item_button->getTag())
				{
				case 0:
					unread_falg = SINGLE_CHAT->m_public_unread_falg_zh;
					break;
				case 1:
					unread_falg = SINGLE_CHAT->m_public_unread_falg;
					break;
				case 2:
					unread_falg = SINGLE_CHAT->m_country_unread_falg;
					break;
				case 3:
					unread_falg = SINGLE_CHAT->m_zone_unread_falg;
					break;
				case 4:
					unread_falg = SINGLE_CHAT->m_guild_unread_falg;
					break;
				default:
					unread_falg = SINGLE_CHAT->m_private_unread_falg[m_vPrivateName[item_button->getTag() - 5].data()];
					break;
				}
				if (unread_falg)
				{
					i_unread->setVisible(true);
				}
				else
				{
					i_unread->setVisible(false);
				}
			}
		}
	}
	for (size_t i = 1; i < m_nLastTime.size(); i++)
	{
		m_nLastTime.at(i) -= delta;
	}
}
void UISocial::textFiledEvent(Ref *target, int type)
{
	auto fieldContent = (ui::EditBox*)(target);
	std::string strContent = "";

	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CHAT_CSB]);
	if (view)
	{
		TextField* text = view->getChildByName<TextField*>("textfield_chat");
		fieldContent = dynamic_cast<ui::EditBox*>(text->getParent()->getChildByTag(1));
		strContent = fieldContent->getText();
	}
	std::string html_color_begin;
	std::string html_color_end;
#if WIN32
	html_color_begin = "";
	html_color_end = "";
#else
	html_color_begin = "<html><font size='24' color='#2E1D0E'>";
	html_color_end = "</font></html>";
#endif
	if (strContent.size() <= 0)
	{
		return;
	}
	if (type == 1)
	{
		//当打开输入法时去除HTML标签
		if (illegal_character_check(strContent))
		{
			auto lengBegin = html_color_begin.length();
			auto lengEnd = html_color_end.length();

			auto posbegin = strContent.find(html_color_begin, 0);

			auto str_1 = strContent.erase(posbegin, lengBegin);
			auto posend = str_1.find(html_color_end, 0);
			auto newContent = str_1.erase(posend, lengEnd);

			fieldContent->setText(newContent.c_str());
		}
	}
	/*	else if (type == 2)
	{
	if (!illegal_character_check(strContent))
	{
	std::string content = html_color_begin;
	content += strContent;
	content += html_color_end;
	fieldContent->setText(content.c_str());
	}
	else
	{
	auto lengBegin = html_color_begin.length();
	auto lengEnd = html_color_end.length();

	auto posbegin = strContent.find(html_color_begin, 0);

	auto str_1 = strContent.erase(posbegin, lengBegin);
	auto posend = str_1.find(html_color_end, 0);
	auto newContent = str_1.erase(posend, lengEnd);

	std::string content = html_color_begin;
	content += newContent;
	content += html_color_end;
	fieldContent->setText(content.c_str());
	}
	}
	*/
	else if (type == 3)
	{
		//当插入字体时去除HTML标签
		if (illegal_character_check(strContent))
		{
			auto lengBegin = html_color_begin.length();
			auto lengEnd = html_color_end.length();

			auto posbegin = strContent.find(html_color_begin, 0);

			auto str_1 = strContent.erase(posbegin, lengBegin);
			auto posend = str_1.find(html_color_end, 0);
			auto newContent = str_1.erase(posend, lengEnd);

			fieldContent->setText(newContent.c_str());
		}
	}
}
void UISocial::openFaceBookInvite(bool isShow)
{
	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto panel_invited_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_invited_content"));
	panel_invited_content->setVisible(isShow);
	auto panel_gift = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_gift"));
	panel_gift->setVisible(false);

	auto text_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_invited_content, "text_1"));
	text_1->setString("x5");
	auto text_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_invited_content, "text_2"));
	text_2->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_FACEBOOK_CONTENT_2"]);
	text_2->setTextVerticalAlignment(TextVAlignment::TOP);
	text_2->setContentSize(Size(text_2->getContentSize().width,68));
	auto text_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_invited_content, "text_3"));
	text_3->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_FACEBOOK_CONTENT_3"]);
	auto t_title = view->getChildByName<Text*>("label_title");
	t_title->setString("");
	if (isShow)
	{
		TableView* mTableView_application = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_FRIEND_APPLICATION));
		if (mTableView_application)
		{
			mTableView_application->removeFromParentAndCleanup(true);
		}
		TableView* mTableView_friend_block = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_FRIEND_BLACK));
		if (mTableView_friend_block)
		{
			mTableView_friend_block->removeFromParentAndCleanup(true);
		}
		TableView* mTableView_my_friend = dynamic_cast<TableView*>(view->getChildByTag(10240 + PAGE_MY_FRIEND));
		if (mTableView_my_friend)
		{
			mTableView_my_friend->removeFromParentAndCleanup(true);
		}
	}

}
void UISocial::setFaceBookInvite()
{
	openView(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	m_pMainButton = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "button_addfacebook_Friend"));
	auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown"));
	image_pulldown->setVisible(false);
	changeMainButton(m_pMainButton);
	openFaceBookInvite(true);
}
void UISocial::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
	auto pos = view->getContentOffset();
	auto viewSize = view->getViewSize();
	auto contentSize = view->getContainer()->getContentSize();

	float bottom = viewSize.height - contentSize.height;
	float top = 0;
	float totalLen = top - bottom;
	float currentLen = top - pos.y;
	m_nslider = pos;
	log("qqqq%f,%f", m_nslider.x, m_nslider.y);
	Button*button_pulldown = dynamic_cast<Button*>(view->getUserObject());
	if (!button_pulldown)
		return;
	float allowHeight = 16;
	float len = dynamic_cast<Widget*>(button_pulldown->getParent())->getSize().height - button_pulldown->getSize().height - allowHeight * 2;
	if (contentSize.height <= viewSize.height)
	{
		button_pulldown->setVisible(false);
		button_pulldown->getParent()->setVisible(false);
	}
	else
	{
		button_pulldown->setVisible(true);
		button_pulldown->getParent()->setVisible(true);
		auto factor = (currentLen / totalLen);
		if (factor > 1.0) factor = 1.0;
		if (factor < 0) factor = 0;
		button_pulldown->setPositionY(len * factor + allowHeight * 2 + allowHeight);
	}
}
void UISocial::tableCellTouched(TableView* table, TableViewCell* cell)
{
}
Size UISocial::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto item_friend = view->getChildByName<Widget*>("panel_friend");
	auto item_black = view->getChildByName<Widget*>("panel_block");
	auto item_application = view->getChildByName<Widget*>("panel_request");
	auto itemSize = item_friend->getContentSize();
	switch (m_nPageIndex)
	{
	case PAGE_MY_FRIEND:
	{
						   if (item_friend)
						   {
							   itemSize = item_friend->getContentSize();
						   }
						   break;
	}
	case PAGE_FRIEND_BLACK:
	{
							  if (item_black)
							  {
								  itemSize = item_black->getContentSize();
							  }
							  break;
	}
	case PAGE_FRIEND_APPLICATION:
	{
									if (item_application)
									{
										itemSize = item_application->getContentSize();
									}
									break;
	}
	default:
		break;
	}
	return itemSize;
}
TableViewCell * UISocial::tableCellAtIndex(TableView *table, ssize_t idx)
{
	auto view = getViewRoot(SOCIAL_COCOS_RES[SOCIAL_CSB]);
	auto panel_friend = view->getChildByName<Widget*>("panel_friend");
	auto item_black = view->getChildByName<Widget*>("panel_block");
	auto item_application = view->getChildByName<Widget*>("panel_request");
	TableViewCell *cell = table->dequeueCell();
	Widget* cell_item;
	if (!cell)
	{
		cell = new TableViewCell();
		cell->autorelease();
		switch (m_nPageIndex)
		{
		case PAGE_MY_FRIEND:
		{
							   if (panel_friend)
							   {
								   cell_item = panel_friend->clone();
							   }
							   break;
		}
		case PAGE_FRIEND_BLACK:
		{
								  if (item_black)
								  {
									  cell_item = item_black->clone();
								  }
								  break;
		}
		case PAGE_FRIEND_APPLICATION:
		{
										if (item_application)
										{
											cell_item = item_application->clone();
										}
										break;
		}
		default:
			break;
		}
		cell->addChild(cell_item, 1);
		cell_item->setPosition(Vec2(0, 0));
		cell_item->setCameraMask(_cameraMask);
	}
	else
	{
		cell_item = dynamic_cast<Widget*>(cell->getChildren().at(0));
	}

	cell->setTag(idx);
	cell->setVisible(true);
	cell_item->setSwallowTouches(false);

	switch (m_nPageIndex)
	{
	case PAGE_MY_FRIEND:
	{
						   cell_item->setTouchEnabled(false);
						   auto img_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_head"));
						   auto img_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_flag"));
						   auto t_name = dynamic_cast<Text*>(cell_item->getChildByName<Widget*>("label_player_name"));
						   auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_lv"));
						   auto b_chat = cell_item->getChildByName<Button*>("button_chat");
						   auto b_online = cell_item->getChildByName<Button*>("button_line");
						   auto i_button_gift = cell_item->getChildByName<Button*>("button_gift");
						   auto b_mail = cell_item->getChildByName<Button*>("button_mail");
						   auto b_more = cell_item->getChildByName<Button*>("button_more");
						   auto image_head_bg_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_head_bg_1"));
						   image_head_bg_1->setTouchEnabled(true);
						   image_head_bg_1->setTag(idx);
						   b_more->setTag(idx);
						   b_chat->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));
						   b_mail->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));
						   b_more->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));
						   image_head_bg_1->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));
						   i_button_gift->addTouchEventListener(CC_CALLBACK_2(UISocial::friendListButtonEvent, this));

						   cell_item->setTag(m_pFriendsResult->friends[idx]->characterid);
						   cell_item->setName(m_pFriendsResult->friends[idx]->heroname);
						   img_icon->ignoreContentAdaptWithSize(false);
						   img_icon->loadTexture(getPlayerIconPath(m_pFriendsResult->friends[idx]->iconidx));
						   img_flag->ignoreContentAdaptWithSize(false);
						   img_flag->loadTexture(getCountryIconPath(m_pFriendsResult->friends[idx]->nation));
						   b_chat->setTag(m_pFriendsResult->friends[idx]->online);
						   t_name->setString(m_pFriendsResult->friends[idx]->heroname);
						   t_lv->setString(StringUtils::format("Lv. %ld", m_pFriendsResult->friends[idx]->level));
						   if (m_pFriendsResult->friends[idx]->online)
						   {
							   b_online->setBright(true);
						   }
						   else
						   {
							   b_online->setBright(false);
						   }
						   bool is_gift = false;
						   for (int k = 0; k < m_pFriendsResult->n_can_not_send_cid; k++)
						   {
							   if (m_pFriendsResult->friends[idx]->characterid == m_pFriendsResult->can_not_send_cid[k])
							   {
								   is_gift = true;
								   break;
							   }
						   }
						   if (is_gift)
						   {
							   i_button_gift->setBright(false);
						   }
						   else
						   {
							   i_button_gift->setBright(true);
						   }
						   i_button_gift->setTag(idx);
						   break;
	}
	case PAGE_FRIEND_BLACK:
	{
							  cell_item->setTouchEnabled(false);
							  auto img_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_head"));
							  auto img_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_flag"));
							  auto t_name = dynamic_cast<Text*>(cell_item->getChildByName<Widget*>("label_player_name"));
							  auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_lv"));
							  //改动
							  auto bt_remove = cell_item->getChildByName<Button*>("button_accept_0");
							  bt_remove->setTag(idx);

							  cell_item->setTag(m_pBlackListResult->friends[idx]->characterid);
							  cell_item->setName(m_pBlackListResult->friends[idx]->heroname);
							  auto image_head_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_head_bg_1"));
							  auto b_more = cell_item->getChildByName<Button*>("button_more");
							  b_more->addTouchEventListener(CC_CALLBACK_2(UISocial::blockListAndRequestButtonEvent, this));
							  image_head_bg->setTouchEnabled(true);
							  image_head_bg->addTouchEventListener(CC_CALLBACK_2(UISocial::blockListAndRequestButtonEvent, this));
							  image_head_bg->setTag(idx);
							  img_icon->ignoreContentAdaptWithSize(false);
							  img_icon->loadTexture(getPlayerIconPath(m_pBlackListResult->friends[idx]->iconidx));
							  img_flag->ignoreContentAdaptWithSize(false);
							  img_flag->loadTexture(getCountryIconPath(m_pBlackListResult->friends[idx]->nation));
							  t_name->setString(m_pBlackListResult->friends[idx]->heroname);
							  t_lv->setString(StringUtils::format("Lv. %ld", m_pBlackListResult->friends[idx]->level));
							  bt_remove->addTouchEventListener(CC_CALLBACK_2(UISocial::menuCall_func, this));
							  break;
	}
	case PAGE_FRIEND_APPLICATION:
	{
									cell_item->setTouchEnabled(false);
									auto img_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_head"));
									auto img_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_flag"));
									auto t_name = dynamic_cast<Text*>(cell_item->getChildByName<Widget*>("label_player_name"));
									auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_lv"));


									cell_item->setTag(m_pFriendsReqListResult->friends[idx]->characterid);
									cell_item->setName(m_pFriendsReqListResult->friends[idx]->heroname);
									auto b_accept = cell_item->getChildByName<Button*>("button_accept");
									auto b_delete = cell_item->getChildByName<Button*>("button_delete_0");
									b_accept->addTouchEventListener(CC_CALLBACK_2(UISocial::blockListAndRequestButtonEvent, this));
									b_delete->addTouchEventListener(CC_CALLBACK_2(UISocial::blockListAndRequestButtonEvent, this));
									auto image_head_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_head_bg"));
									image_head_bg->setTouchEnabled(true);
									image_head_bg->addTouchEventListener(CC_CALLBACK_2(UISocial::blockListAndRequestButtonEvent, this));
									image_head_bg->setTag(idx);
									img_icon->ignoreContentAdaptWithSize(false);
									img_icon->loadTexture(getPlayerIconPath(m_pFriendsReqListResult->friends[idx]->iconidx));
									img_flag->ignoreContentAdaptWithSize(false);
									img_flag->loadTexture(getCountryIconPath(m_pFriendsReqListResult->friends[idx]->nation));
									t_name->setString(m_pFriendsReqListResult->friends[idx]->heroname);
									t_lv->setString(StringUtils::format("Lv. %ld", m_pFriendsReqListResult->friends[idx]->level));

									b_accept->setTag(idx);
									b_delete->setTag(idx);
									break;
	}
	default:
		break;
	}
	return cell;
}
ssize_t UISocial::numberOfCellsInTableView(TableView *table)
{
	switch (m_nPageIndex)
	{
	case PAGE_MY_FRIEND:
	{
						   return m_pFriendsResult->n_friends;
						   break;
	}
	case PAGE_FRIEND_BLACK:
	{
							  return m_pBlackListResult->n_friends;
							  break;
	}
	case PAGE_FRIEND_APPLICATION:
	{
									return m_pFriendsReqListResult->n_friends;
									break;
	}
	default:
		return 0;
		break;
	}

}
void UISocial::editBoxReturn(ui::EditBox* editBox)
{
	n_type = 2;
	this->textFiledEvent(editBox, n_type);
}
void UISocial::editBoxEditingDidBegin(ui::EditBox* editBox)
{
	n_type = 1;
	this->textFiledEvent(editBox, n_type);
}
void UISocial::editBoxEditingDidEnd(ui::EditBox* editBox)
{
	// = 2;
	//this->textFiledEvent(editBox, n_type);
}
void UISocial::editBoxTextChanged(ui::EditBox* editBox, const std::string& text)
{
	n_type = 3;
	this->textFiledEvent(editBox, n_type);
}
