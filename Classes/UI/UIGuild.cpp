#include "UIGuild.h"
#include "UIEmail.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIMain.h"
#include "TVSceneLoader.h"

#include "UISocial.h"

UIGuild*  UIGuild::m_pGuildLayer = nullptr; 

UIGuild::UIGuild() :
m_pInputText(nullptr),
bIsEnter(false),
m_pResult(nullptr),
m_pApplyInfoResult(nullptr),
m_pTempButton(nullptr),
m_pPreButton(nullptr),
m_eConfirmIndex(CONFIRM_INDEX_JION_GUILD),
m_eTableViewIndex(TABLEVIEW_NONE),
m_eMyGuildActionIndex(MY_GUILD_ACTION_NONE),
m_nGuildIconid(-1),
m_nPageIndex(-1),
m_nPageSearchIndex(-1),
m_nGuildMemberIndex(-1),
m_pGuildListResult(nullptr),
m_pMyguildDetails(nullptr),
m_pMyguildManager(nullptr),
m_pSearchGuildListResult(nullptr)
   
{
	for (int i = 0; i < 1000; i++)
	{
		m_bChangeAdmin[i] = false;
		m_oldgCompetence[i] = 0;
	}
	m_nSameForceGuilds.clear();
	init();
}

UIGuild::~UIGuild()
{
	if (m_pApplyInfoResult)
	{
		get_apply_info_result__free_unpacked(m_pApplyInfoResult, 0);
	}

	if (m_pGuildListResult)
	{
		get_guild_list_result__free_unpacked(m_pGuildListResult, 0);
	}

	if (m_pSearchGuildListResult)
	{
		get_search_guild_list_result__free_unpacked(m_pSearchGuildListResult, 0);
	}
	m_nSameForceGuilds.clear();
	m_pMyguildDetails = nullptr;
	m_pMyguildManager = nullptr;
}

void UIGuild::onEnter()
{
	UIBasicLayer::onEnter();
}

void UIGuild::onExit()
{
	UIBasicLayer::onExit();
}

bool UIGuild::init()
{
	if(UIBasicLayer::init())
	{
		m_pInputText = ui::EditBox::create(Size(100, 100), "input.png");
		m_pInputText->setPosition(Vec2(-200, -200));
		m_pInputText->setDelegate(this);
		m_pInputText->setTag(100);
		m_pInputText->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		m_pInputText->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		this->addChild(m_pInputText,100);
		return true;
	}
	return false;
}

void UIGuild::openGuildLayer()
{
	auto currentScene = Director::getInstance()->getRunningScene();
	UIInform::getInstance()->closeAutoInform(0);
	this->removeFromParentAndCleanup(true);
	currentScene->addChild(this,10);
	
	registerCallBack();
	m_pPreButton = nullptr;
	if (SINGLE_HERO->m_iGuildId)
	{
		ProtocolThread::GetInstance()->getMyGuildDetails(UILoadingIndicator::create(this));
	}else
	{
		ProtocolThread::GetInstance()->getGuildList(UILoadingIndicator::create(this));
	}
}

void UIGuild::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetGuildListResult:
		{
			GetGuildListResult *pGuildList = (GetGuildListResult*)message;
			m_pResult = message;
			if (pGuildList->failed == 0)
			{
				m_pGuildListResult = pGuildList;
				m_nSameForceGuilds.clear();
				for (int i = 0; i < pGuildList->n_guilds;i++)
				{			
					m_nSameForceGuilds.push_back(pGuildList->guilds[i]);
				}
				m_nPageIndex = 1;
				flushGuildList();
				std::string name = "button_guilds";
				isButtonInListView(name, nullptr);
				SINGLE_HERO->m_iGuildId = pGuildList->guildid;
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_GET_GUILD_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetMyGuildDetailsResult:
		{
			GetMyGuildDetailsResult *myguildDetails = (GetMyGuildDetailsResult*)message;
			m_pResult = message;
			if (myguildDetails->failed == 0)
			{
				flushMyGuild(myguildDetails);
				std::string name = "button_mgguild";
				isButtonInListView(name, nullptr);
				SINGLE_HERO->m_iGuildId = myguildDetails->id;
			}
			else if (myguildDetails->failed == 3)
			{
				SINGLE_HERO->m_iGuildId = 0;
				myguildDetails->id = 0;
				flushMyGuild(myguildDetails);
				std::string name = "button_mgguild";
				isButtonInListView(name, nullptr);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_GET_MYGUILD_DATA_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetApplyInfoResult:
		{
			GetApplyInfoResult *applyInfo = (GetApplyInfoResult*)message;
			m_pApplyInfoResult = applyInfo;
			if (applyInfo->failed == 0)
			{
				flushApplication(applyInfo);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_GET_APPLY_INFO_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetGuildDetailsResult:
		{
			GetGuildDetailsResult *guildDetails = (GetGuildDetailsResult*)message;
			if (guildDetails->failed == 0)
			{
				openGuildDetailView(guildDetails);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_GET_GUILD_DETAIL_FAIL");
			}
			break;
		}
	case PROTO_TYPE_JoinGuildRequestResult:
		{
			JoinGuildRequestResult *guildDetails = (JoinGuildRequestResult*)message;
			UIInform::getInstance()->openInformView(this);
			/*
			*0:申请发送成功 2:申请已经发送 9:已经加入了公会 11：不属于同一势力不能加入
			*/
			switch (guildDetails->failed)
			{
			case 0:
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_JOIN_GUILD_SUCCESS");
				break;
			case 2:
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_REQUEST_ADDFRIENDED");
				break;
			case 9:
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_ONLG_ONE_GUILD");
				break;
			case 11:
			    UIInform::getInstance()->openConfirmYes("TIP_JOIN_GUILD_FAILED_NOT_SAME_COUNTRY");
			    break;
			default:
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_JOIN_GUILD_FAIL");
				break;
			}
			break;
		}
	case PROTO_TYPE_CreateNewGuildResult:
		{
			CreateNewGuildResult *newGuild = (CreateNewGuildResult*)message;
			UIInform::getInstance()->openInformView(this);
			/*
			*0:创建公会成功 10:公会名称不合法  COIN_NOT_FAIL:银币不足
			*/
			switch (newGuild->failed)
			{
			case 0:
				{
					closeView();
					UIInform::getInstance()->openViewAutoClose("TIP_GUILD_CREATE_GUILD_SUCCESS");
					ProtocolThread::GetInstance()->getGuildList(UILoadingIndicator::create(this));
					SINGLE_HERO->m_iGuildId = newGuild->guildid;
					SINGLE_HERO->m_iCoin = newGuild->curcoins;
					auto currentScene = Director::getInstance()->getRunningScene();
					auto mainlayer = (UIMain*)(currentScene->getChildByTag(100 + MAIN_TAG));
					if (mainlayer)
					{
						mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
					}
					break;
				}
			case 10:
				{
					UIInform::getInstance()->openConfirmYes("TIP_GUILD_GUILD_NAME_NOTILLEGAL");
					break;
				}
			case COIN_NOT_FAIL:
				{
					UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
					break;
				}
			default:
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_CREATE_GUILD_FAIL");
				break;
			}
			break;
		}
	case PROTO_TYPE_ChangeGuildIntroResult:
		{
			ChangeGuildIntroResult *changeGuild = (ChangeGuildIntroResult*)message;
			if (changeGuild->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_CHANGE_GUILD_SUCCESS");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_CHANGE_GUILD_FAIL");
			}
			break;
		}
	case PROTO_TYPE_ExpandGuildCapacityResult:
		{
			ExpandGuildCapacityResult *expandGuild = (ExpandGuildCapacityResult*)message;
			if (expandGuild->failed == 0)
			{
				flushMembersNum(expandGuild);
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_EXPAND_SUCCESS");
				SINGLE_HERO->m_iCoin = expandGuild->coins;
				SINGLE_HERO->m_iGold = expandGuild->gold;
				auto currentScene = Director::getInstance()->getRunningScene();
				auto mainlayer = (UIMain*)(currentScene->getChildByTag(100 + MAIN_TAG));
				if (mainlayer)
				{
					mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
				}
			}
			else if (expandGuild->failed == COIN_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_EXPAND_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetGuildMemberPermissionResult:
		{
			GetGuildMemberPermissionResult *gulidMember = (GetGuildMemberPermissionResult*)message;
			m_pResult = message;
			if (gulidMember->failed == 0)
			{
				m_pMyguildManager = gulidMember;
				if (m_pMyguildManager->create_id != SINGLE_HERO->m_iID)
				{
					vector<GuildMemberPermissionDefine*> vMyguildInfo;
					vMyguildInfo.clear();
					for (size_t i = 0; i < m_pMyguildManager->n_members; i++)
					{
						auto guildDefine = m_pMyguildManager->members[i];
						if (!guildDefine->admin)
						{
							vMyguildInfo.push_back(guildDefine);
						}
					}

					m_pMyguildManager->n_members = vMyguildInfo.size();
					for (size_t i = 0; i < m_pMyguildManager->n_members; i++)
					{
						m_pMyguildManager->members[i] = vMyguildInfo.at(i);
					}
				}

				openManagerView(m_pMyguildManager);
				for (int i = 0; i < gulidMember->n_members; i++)
				{
					m_oldgCompetence[i] = gulidMember->members[i]->admin;
					m_bChangeAdmin[i] = false;
				}
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_GET_PERMISSION_FAIL");
			}
			break;
		}
	case PROTO_TYPE_SetGuildMemberPermissionResult:
		{
			SetGuildMemberPermissionResult *gulidMember = (SetGuildMemberPermissionResult*)message;
			if (gulidMember->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_SET_PERMISSION_SUCCESS");
				ProtocolThread::GetInstance()->getMyGuildDetails(UILoadingIndicator::create(this));
				ProtocolThread::GetInstance()->getGuildMemberPermission(UILoadingIndicator::create(this));
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_SET_PERMISSION_FAIL");
			}
			break;
		}
	case PROTO_TYPE_DealWithGuildJoinResult:
		{
			DealWithGuildJoinResult *dealWithGuild = (DealWithGuildJoinResult*)message;
			if (dealWithGuild->failed == 0)
			{
				ProtocolThread::GetInstance()->getApplyInfo(UILoadingIndicator::create(this));
				/*0：拒绝 1:同意*/
				if (dealWithGuild->action == 1)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_GUILD_AGREE_JOIN_GUILD_SUCCESS");
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_GUILD_REFUSAL_JOIN_GUILD_SUCCESS");
				}
			}
			else
			{
				ProtocolThread::GetInstance()->getApplyInfo(UILoadingIndicator::create(this));
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_AGREE_JOINGUILD_FAIL");
			}
			break;
		}
	case PROTO_TYPE_RemoveGuildMemberResult:
		{
			RemoveGuildMemberResult *removeMember = (RemoveGuildMemberResult*)message;
			if (removeMember->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_REMOVE_MEMBER_SUCCESS");
				ProtocolThread::GetInstance()->getGuildMemberPermission(UILoadingIndicator::create(this));
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_REMOVE_MEMBER_FAIL");
			}
			break;
		}
	case PROTO_TYPE_ExitFromGuildResult:
		{
			ExitFromGuildResult *exitGuild = (ExitFromGuildResult*)message;
			if (exitGuild->failed == 0)
			{
				int num = m_vRoots.size();
				for (int i = 1; i < num; i++)
				{
					closeView();
				}
				ProtocolThread::GetInstance()->getGuildList(UILoadingIndicator::create(this));
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_EXIT_GUILD_SUCCESS");
				SINGLE_HERO->m_iGuildId = 0;
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_EXIT_GUILD_FAIL");
			}
			break;
		}
	case PROTO_TYPE_DismissGuildResult:
		{
			DismissGuildResult *dismissGuild = (DismissGuildResult*)message;
			if (dismissGuild->failed == 0)
			{
				int num = m_vRoots.size();
				for (int i = 1; i < num; i++)
				{
					closeView();
				}
				ProtocolThread::GetInstance()->getGuildList(UILoadingIndicator::create(this));
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_DISMISS_GUILD_SUCCESS");
				SINGLE_HERO->m_iGuildId = 0;
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_DISMISS_GUILD_FAIL");
			}
			break;
		}
	case PROTO_TYPE_ChangeGuildAnnouncementResult:
		{
			ChangeGuildAnnouncementResult *changeAnnouncement = (ChangeGuildAnnouncementResult*)message;
			if (changeAnnouncement->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GUILD_CHANGE_ANNOUNCEMENT_SUCCESS");
				ProtocolThread::GetInstance()->getMyGuildDetails(UILoadingIndicator::create(this));
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_CHANGE_ANNOUNCEMENT_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetSearchGuildListResult:
		{
			GetSearchGuildListResult *pGuildList = (GetSearchGuildListResult*)message;
			if (pGuildList->failed == 0)
			{
				m_nPageSearchIndex = 1;
				m_pSearchGuildListResult = pGuildList;
				closeView();
				openSearchView();
			}
			else if (pGuildList->failed == 9)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_SEARCH_FREQUENTLY");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_GET_GUILD_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetUserInfoByIdResult:
		{
			GetUserInfoByIdResult *result = (GetUserInfoByIdResult *)message;
			if (result->failed == 0)
			{
				if (m_pTempButton)
				{
					UICommon::getInstance()->openCommonView(this);
					if (m_pTempButton->getParent()->getTag() == SINGLE_HERO->m_iID)
					{
						UICommon::getInstance()->flushFriendDetail(result, true);
					}
					else
					{
						UICommon::getInstance()->flushFriendDetail(result, false);
					}
				}
			}
			break;
		}
	case PROTO_TYPE_FriendsOperationResult:
		{
			FriendsOperationResult *pFriendsResult = (FriendsOperationResult*)message;
			if (pFriendsResult->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				switch (pFriendsResult->actioncode)
				{
				case 0:
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_FRIEND_SUCCESS");
					break;
				case 1:
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_BLACK_LIST_SUCCESS");
					break;
				case 2:
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_BLACK_LIST_SUCCESS");
					break;
				case 3:
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADDED_FRIEND_SUCCESS");
					break;
				case 4:
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADDREFUSAL_SUCCESS");
					break;
				default:
					break;
				}
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
	case PROTO_TYPE_GetInvitationListResult:
		{
			GetInvitationListResult *result = (GetInvitationListResult *)message;
			if (result->failed == 0)
			{
				flushInvitation(result);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITATION_INFO_FAIL");
			}
			break;
		}
	case PROTO_TYPE_TestCreateNewGuildResult:
		{
			TestCreateNewGuildResult *result = (TestCreateNewGuildResult *)message;
			if (result->failed == 0)
			{
				openCreateGuildView();
			}
			else
			{
				openCreateGuildFailView(result);
			}
			break;
		}
	case PROTO_TYPE_DealWithInvitationResult:
		{
			DealWithInvitationResult *result = (DealWithInvitationResult *)message;
			if (result->failed == 0)
			{
				if (result->accept == 1)
				{
					closeView();
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_GUILD_ACCEPT_INVITATION_SUCCESS");
					ProtocolThread::GetInstance()->getMyGuildDetails(UILoadingIndicator::create(this));
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_GUILD_REFUST_INVITATION_SUCCESS");
					ProtocolThread::GetInstance()->getInvitationList(UILoadingIndicator::create(this));
				}
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	default:
		break;
	}
}

void UIGuild::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* button = (Widget*)pSender;
	std::string name = button->getName();
	if (m_pPreButton == button)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//工会列表
	if (isButton(button_guilds))
	{
		closeView();
		isButtonInListView(name,button);
		ProtocolThread::GetInstance()->getGuildList(UILoadingIndicator::create(this));
		return;
	}
	//自己工会
	if (isButton(button_mgguild))
	{
		closeView();
		isButtonInListView(name,button);
		ProtocolThread::GetInstance()->getMyGuildDetails(UILoadingIndicator::create(this));
		return;
	}
	//申请入会
	if (isButton(button_request))
	{
		closeView();
		isButtonInListView(name,button);
		ProtocolThread::GetInstance()->getApplyInfo(UILoadingIndicator::create(this));
		return;
	}
	//邀请入会
	if (isButton(button_invitation))
	{
		closeView();
		isButtonInListView(name,button);
		ProtocolThread::GetInstance()->getInvitationList(UILoadingIndicator::create(this));
		return;
	}
	//返回主城
	if (isButton(button_back))
	{
		closeView();
		closeView();
		unregisterCallBack();
		return;
	}
	//关闭公会管理界面
	if (isButton(b_close))
	{
		auto view = getViewRoot(GUILD_COCOS_RES[GUILD_MANAGER_CSB]);
		auto l_guild = m_pRoot->getChildByName<ListView*>("listview_guild");
		auto tableViewManager = l_guild->getParent()->getChildByTag(1025);
		if (tableViewManager)
		{
			tableViewManager->removeFromParentAndCleanup(true);
		}
		closeView();
		m_eTableViewIndex = MY_GUILD_DETAILS;
		ProtocolThread::GetInstance()->getMyGuildDetails(UILoadingIndicator::create(this));
		return;
	}
	//关闭界面
	if (isButton(button_close) || isButton(button_no))
	{
		closeView();
		return;
	}
	//工会成员上线扩充确认
	if (isButton(button_s_yes))
	{
		if (m_eConfirmIndex = CONFIRM_INDEX_EXPAND_GUILD_CAPACITY)
		{
			ProtocolThread::GetInstance()->expandGuildCapacity(UILoadingIndicator::create(this));
		}
		return;
	}
	//提示界面确定按钮
	if (isButton(button_confirm_yes))
	{
		if (m_eConfirmIndex == CONFIRM_INDEX_JION_GUILD)//确认加入
		{
			ProtocolThread::GetInstance()->joinGuildRequest(m_pTempButton->getTag(),UILoadingIndicator::create(this));
			return;
		}
		if (m_eConfirmIndex == CONFIRM_INDEX_EQIT_GUILD)//确认编辑
		{
			ProtocolThread::GetInstance()->exitFromGuild(UILoadingIndicator::create(this));
			return;
		}
		if (m_eConfirmIndex == CONFIRM_INDEX_DISMISS_GUILD)//确认解散
		{
			ProtocolThread::GetInstance()->dismissGuild(UILoadingIndicator::create(this));
			return;
		}
		if (m_eConfirmIndex == CONFIRM_INDEX_ACCEPT_JIONGUILD)//接受申请
		{
			ProtocolThread::GetInstance()->dealWithGuildJoin(1,m_pApplyInfoResult->applications[m_pTempButton->getTag()]->id,m_pApplyInfoResult->applications[m_pTempButton->getTag()]->characterid,m_pApplyInfoResult->guild_id,UILoadingIndicator::create(this));
			return;
		}
		if (m_eConfirmIndex == CONFIRM_INDEX_REFUST_JIONGUILD)//拒绝申请
		{
			ProtocolThread::GetInstance()->dealWithGuildJoin(0,m_pApplyInfoResult->applications[m_pTempButton->getTag()]->id,m_pApplyInfoResult->applications[m_pTempButton->getTag()]->characterid,m_pApplyInfoResult->guild_id,UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_SAVE_SETADMIN)//批准入会
		{
			GetGuildMemberPermissionResult *result = (GetGuildMemberPermissionResult*)m_pResult;
			//SetGuildPermissionDefine **guildPeermission = new SetGuildPermissionDefine *[result->n_members];
			int nums = 0;
			for (int i = 0; i < result->n_members;i++)
			{
				if (m_bChangeAdmin[i])
				{
					nums++;
				}				
			}
			//有成员权限改变时
			if (nums>0)
			{
				SetGuildPermissionDefine **guildPeermission = new SetGuildPermissionDefine *[nums];
				int j = 0;
				for (int i = 0; i < result->n_members; i++)
				{
					if (m_bChangeAdmin[i])
					{
						SetGuildPermissionDefine *g_Peermission = new SetGuildPermissionDefine;
						set_guild_permission_define__init(g_Peermission);
						g_Peermission->character_id = result->members[i]->character_id;
						g_Peermission->admin = m_gCompetence[i];
						g_Peermission->deposit_cash = 1;
						g_Peermission->withdraw_cash = 1;
						g_Peermission->deposit_item = 1;
						g_Peermission->withdraw_item = 1;
						guildPeermission[j++] = g_Peermission;
					}
				}
				m_eMyGuildActionIndex = MY_GUILD_SET_ADMIN;
				ProtocolThread::GetInstance()->setGuildMemberPermission(guildPeermission, nums, UILoadingIndicator::create(this));
				for (int i = 0; i < nums; i++)
				{
					delete guildPeermission[i];
				}
				delete[]guildPeermission;
			}	
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_REMOVE_GUILDMANAGR)//确认删除会员
		{
			m_eMyGuildActionIndex = MY_GUILD_DELETE_MEMBER;
			GetMyGuildDetailsResult *result = (GetMyGuildDetailsResult *)m_pResult;
			ProtocolThread::GetInstance()->removeGuildMember(result->id,m_pTempButton->getTag(),UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_FRIEND_BLOCK_GUILD || m_eConfirmIndex == CONFIRM_INDEX_STRANGER_BLOCK_GUILD)//确认拉黑
		{
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(),2,UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_REMOVE_FRIEND)//确认删除好友
		{
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(),1,UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_STRANGER_ADD_FRIEND || m_eConfirmIndex == CONFIRM_INDEX_BLOCK_ADD_FRIEND)//确认添加拉黑列表中人物为好友
		{
			ProtocolThread::GetInstance()->friendsOperation(m_pTempButton->getParent()->getTag(),0,UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_ACCEPT_INVITATION)//接受邀请
		{
			ProtocolThread::GetInstance()->dealWithInvitation(m_pTempButton->getTag(),1,UILoadingIndicator::create(this));
			return;
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_REFUST_INVITATION)//拒绝邀请
		{
			ProtocolThread::GetInstance()->dealWithInvitation(m_pTempButton->getTag(),0,UILoadingIndicator::create(this));
			return;
		}
		//发送公会邀请
		if (m_eConfirmIndex == CONFIRM_INDEX_GUILD_INVITE_GUILD)
		{
			ProtocolThread::GetInstance()->inviteUserToGuild(m_pTempButton->getTag(), UILoadingIndicator::create(this));
			return;
		}
		return;
	}
	//添加好友
	if (isButton(button_add_friend))
	{
		m_eConfirmIndex = CONFIRM_INDEX_STRANGER_ADD_FRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_ADD_FRIEND_TITLE", "TIP_SOCIAL_ADD_FRIEND");
		return;
	}
	//删除好友
	if (isButton(button_friend_delete))
	{
		m_eConfirmIndex = CONFIRM_INDEX_REMOVE_FRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_DELETE_FRIEND_TITLE", "TIP_SOCIAL_DELETE_FRIEND");
		return;
	}
	//拉黑好友
	if (isButton(button_friend_block))
	{
		m_eConfirmIndex = CONFIRM_INDEX_FRIEND_BLOCK_GUILD;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_FRIEND");
		return;
	}
	//拉黑陌生人
	if (isButton(button_stranger_block))
	{
		m_eConfirmIndex = CONFIRM_INDEX_STRANGER_BLOCK_GUILD;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_STRANGER");
		return;
	}
	//邀请
	if (isButton(button_invite))
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_GUILD_INVITE_GUILD;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_INVITE_GUILD_TITLE", "TIP_GUILD_INVINT_GUILD");
		return;
	}
	//查找 翻页效果
	if (isButton(button_pageup)) 
	{
		if (m_nPageSearchIndex > 1)
		{
			m_nPageSearchIndex--;
			openSearchView();
		}
		return;
	}
	//查找 翻页效果
	if (isButton(button_pagedown))
	{
		if (m_pSearchGuildListResult->n_guilds > m_nPageSearchIndex * PAGE_ITEM_NUM)
		{
			m_nPageSearchIndex++;
			openSearchView();
		}
		return;
	}
	//确认界面按钮
	if (isButton(button_confirm_no) || isButton(button_error_yes) || isButton(button_s_no))
	{
		return;
	}

	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushImageDetail(button);
}

void UIGuild::guildListEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* button = (Widget*)pSender;
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//详细信息
	if (isButton(panel_guild) || isButton(button_more))
	{
		ProtocolThread::GetInstance()->getGuildDetails(button->getTag(),UILoadingIndicator::create(this));
		return;
	}
	//加入工会
	if (isButton(button_add_guild) || isButton(button_join_guild)) 
	{
		m_eConfirmIndex = CONFIRM_INDEX_JION_GUILD;
		m_pTempButton = button;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_GUILD_JOIN_GUILD","TIP_GUILD_JOIN_GUILD");
		return;
	}
	//搜索
	if (isButton(button_search))
	{
		openView(GUILD_COCOS_RES[GUILD_SEARCH_CSB]);
		TextField* tf_guild_name = m_pRoot->getChildByName<TextField*>("textfield_guild_name");
		tf_guild_name->setTextVerticalAlignment(TextVAlignment::CENTER);
		tf_guild_name->setPlaceHolderColor(Color3B(116, 98, 71));
		tf_guild_name->setVisible(false);
			cocos2d::ui::EditBox* editbox_name = nullptr;
		if (!editbox_name)
		{
			editbox_name = cocos2d::ui::EditBox::create(Size(tf_guild_name->getContentSize()),"input.png");
			tf_guild_name->getParent()->addChild(editbox_name);
			editbox_name->setTag(1);
			editbox_name->setPosition(tf_guild_name->getPosition());
			editbox_name->setAnchorPoint(tf_guild_name->getAnchorPoint());
			editbox_name->setPlaceholderFontColor(Color3B(116, 98, 71));
			editbox_name->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
			editbox_name->setFontColor(Color3B(46, 29, 14));
			editbox_name->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
			editbox_name->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		}
		auto b_ok = m_pRoot->getChildByName<Button*>("button_ok");
		b_ok->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
		auto b_cancel = m_pRoot->getChildByName<Button*>("button_cancel");
		b_cancel->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
		scheduleUpdate();
		return;
	}
	//创建工会
	if (isButton(button_create))
	{
		ProtocolThread::GetInstance()->testCreateNewGuild(UILoadingIndicator::create(this));
		return;
	}
	//搜索工会确认
	if (isButton(button_ok))
	{
		TextField* tf_guild_name = m_pRoot->getChildByName<TextField*>("textfield_guild_name");
		cocos2d::ui::EditBox* editbox_name = dynamic_cast<cocos2d::ui::EditBox*>(tf_guild_name->getParent()->getChildByTag(1));
		std::string e_name = editbox_name->getText();
		auto name = (char*)(e_name.data());
		if (strlen(name) < 4 || strlen(name) > 128)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_HEATCHINPUT_FAIL");
		}else
		{
			ProtocolThread::GetInstance()->getSearchGuildList(name,UILoadingIndicator::create(this));
		}
		return;
	}
	//关闭界面
	if (isButton(button_cancel))
	{
		closeView();
		return;
	}
	//退出工会
	if (isButton(button_quit))
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_EQIT_GUILD;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_GUILD_EXIT_GUILD","TIP_GUILD_EXIT_GUILD");
		return;
	}
	//解散工会
	if (isButton(button_dismiss))
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_DISMISS_GUILD;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_GUILD_DISMISS_GUILD_TITLE","TIP_GUILD_DISMISS_GUILD");
		return;
	}
	//创建公会Icon选择
	if (isButton(button_left))  
	{
		m_nGuildIconid--;
		if (m_nGuildIconid < 1)
		{
			m_nGuildIconid = 5;
		}
		auto i_icon = m_pRoot->getChildByName<ImageView*>("image_guild_head");
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(getGuildIconPath(m_nGuildIconid));
		return;
	}
	//选择工会Icon
	if (isButton(button_right))
	{
		m_nGuildIconid++;
		if (m_nGuildIconid > 5)
		{
			m_nGuildIconid = 1;
		}
		auto i_icon = m_pRoot->getChildByName<ImageView*>("image_guild_head");
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(getGuildIconPath(m_nGuildIconid));
		return;
	}
	//创建公会
	if (isButton(button_creat))
	{
		m_pTempButton = button;
		auto t_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_guild_name"));
		auto t_slogon = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_guild_slogon"));
		auto tf_name = t_name->getParent();
		auto tf_slogon = t_slogon->getParent();
		cocos2d::ui::EditBox* editbox_name = dynamic_cast<cocos2d::ui::EditBox*>(tf_name->getChildByTag(1));
		cocos2d::ui::EditBox* editbox_slogon = dynamic_cast<cocos2d::ui::EditBox*>(tf_slogon->getChildByTag(2));
		std::string e_name = editbox_name->getText();
		std::string e_slogon = editbox_slogon->getText();
		if (e_name.length()<5 || e_name.length()>24)//公会名字长度限制
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GUILD_GUILD_NAME_TOO_LONGOR_SHORT");
			return;
		}
		if (e_slogon.length()<5 || e_slogon.length()>120)//说明内容长度限制
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GUILD_GUILD_CONTENT_TOO_LONGOR_SHORT");
			return;
		}
		auto name = (char*)(e_name.data());
		auto slogon = (char*)(e_slogon.data());
		ProtocolThread::GetInstance()->createNewGuild(name,m_nGuildIconid,slogon,UILoadingIndicator::create(this));
		return;
	}
	//关闭界面
	if (isButton(button_yes))
	{
		closeView();
		return;
	}
	//翻页效果
	if (isButton(button_pageup))  
	{
		if (m_nPageIndex > 1)
		{
			m_nPageIndex--;
			flushGuildList();
		}
		return;
	}
	//工会成员翻页
	if (isButton(button_pagedown))
	{
		//if (m_pGuildListResult->n_guilds > m_nPageIndex * PAGE_ITEM_NUM)
		if (m_nSameForceGuilds.size() > m_nPageIndex * PAGE_ITEM_NUM)
		{
			m_nPageIndex++;
			flushGuildList();
		}
		return;
	}
	//成员详情
	if (isButton(label_guild_name))
	{
		m_pTempButton = button;
		ProtocolThread::GetInstance()->getUserInfoById(button->getTag(), UILoadingIndicator::create(this));
		return;
	}
}

void UIGuild::myGuildListEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* button = (Widget*)pSender;
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//工会信息
	if (isButton(button_info))
	{
		ProtocolThread::GetInstance()->getGuildDetails(button->getTag(),UILoadingIndicator::create(this));
		return;
	}
	//工会信息编辑
	if (isButton(button_edit))
	{
		if (m_eConfirmIndex == CONFIRM_INDEX_NOT_ACCESS)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GUILD_NOT_ACCESS");
			return;
		}
		if (m_pInputText)
		{
			auto p_myguild = m_pRoot->getChildByName<Widget*>("panel_myguild");
			TableView* mTableView = dynamic_cast<TableView*>(p_myguild->getChildByTag(1024));
			if (mTableView)
			{
				auto  cell = mTableView->cellAtIndex(0);
				Widget* cellItem = dynamic_cast<Widget*>(cell->getChildByTag(0));
				auto t_inputName = dynamic_cast<Text*>(Helper::seekWidgetByName(cellItem, "text_announce"));
				m_pInputText->setText((t_inputName->getString()).c_str());
				m_pInputText->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
			}
		
		}
		return;
	}
	//扩展工会成员上限
	if (isButton(button_add))
	{
		m_eConfirmIndex = CONFIRM_INDEX_EXPAND_GUILD_CAPACITY;
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushSilverConfirmView("TIP_GUILD_EXPAND_CAPACITY_TITLE","TIP_GUILD_EXPAND_CAPACITY_CONTENT",1000000);
		return;
	}
	//工会会长信息
	if (isButton(button_manager))
	{
		auto c_box = button->getChildByName<CheckBox*>("checkbox_268");
		auto l_manager = button->getChildByName<Text*>("label_manager");
		if (c_box->isSelected())
		{
			c_box->setSelectedState(false);
			l_manager->setOpacity(127);
			m_gCompetence[c_box->getTag()] = 0;
		}else
		{
			c_box->setSelectedState(true);
			l_manager->setOpacity(255);
			m_gCompetence[c_box->getTag()] = 1;
		}
		//进行比较是否改变工会权限
		if (m_gCompetence[c_box->getTag()] == m_oldgCompetence[c_box->getTag()])
		{
			m_bChangeAdmin[c_box->getTag()] = false;
		}
		else
		{
			m_bChangeAdmin[c_box->getTag()] = true;
		}
		return;
	}
	//删除工会成员
	if (isButton(button_delete))
	{
		auto c_box = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(dynamic_cast<Widget*>(button->getParent()),"checkbox_268"));
		m_nGuildMemberIndex = c_box->getTag();		
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_REMOVE_GUILDMANAGR;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_GUILD_REMOVE_TITLE", "TIP_GUILD_REMOVE");
		return;
	}
	//我的公会 管理
	if (isButton(button_manage)) 
	{
		if (m_eConfirmIndex == CONFIRM_INDEX_NOT_ACCESS)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GUILD_NOT_ACCESS");
		}
		else
		{
			ProtocolThread::GetInstance()->getGuildMemberPermission(UILoadingIndicator::create(this));
		}
		return;
	}
	//聊天
	if (isButton(button_chat))
	{
		m_pTempButton = nullptr;

		if (button->getTag())
		{
			UISocial::getInstance()->openSocialLayer();
			UISocial::getInstance()->setChatCall(button->getParent()->getName());
			UISocial::getInstance()->showChat();
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_FRIEND_NOT_ONLINE");
		}
		return;
	}
	//邮件
	if (isButton(button_mail))
	{
		UIEmail::getInstance()->sendEmailToPlayer(button->getParent()->getName(),"");
		return;
	}
	//确认保存管理
	if (isButton(button_ok))
	{
		m_eConfirmIndex = CONFIRM_INDEX_SAVE_SETADMIN;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_GUILD_SAVE_TITLE","TIP_GUILD_SAVE");
		return;
	}
	//玩家信息
	if (isButton(button_player_info) || isButton(image_guild_head))
	{
		m_pTempButton = button;
		ProtocolThread::GetInstance()->getUserInfoById(button->getParent()->getTag(),UILoadingIndicator::create(this));
		return;
	}
	m_pTempButton = button->getChildByName<Widget*>("image_flag");
	ProtocolThread::GetInstance()->getUserInfoById(button->getTag(),UILoadingIndicator::create(this));
}

void UIGuild::applicationsEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* button = (Widget*)pSender;
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//同意入会
	if (isButton(button_accept))
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_ACCEPT_JIONGUILD;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_REQUEST_ACCEPT_TITLE","TIP_GUILD_AGREE_JOIN_GUILD");
		return;
	}
	//拒绝入会
	if (isButton(button_refuse))
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_REFUST_JIONGUILD;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_REQUEST_REFUSAL_TITLE","TIP_GUILD_REFUSAL_JOIN_GUILD");
		return;
	}
	//同意工会邀请
	if (isButton(button_guild_accept))
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_ACCEPT_INVITATION;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_REQUEST_ACCEPT_TITLE","TIP_GUILD_ACCEPT_INVITATION");
		return;
	}
	//拒绝工会邀请
	if (isButton(button_guild_refuse))
	{
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_REFUST_INVITATION;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_REQUEST_REFUSAL_TITLE","TIP_GUILD_REFUST_INVITATION");
		return;
	}
	//查看工会详情
	if (isButton(panel_request) || isButton(panel_guild))
	{
		m_pTempButton = button->getChildByName<Widget*>("image_flag");
		//公会邀请
		if (m_pTempButton->getTag() == -1)
		{
			ProtocolThread::GetInstance()->getGuildDetails(button->getTag(), UILoadingIndicator::create(this));
		}
		//管理公会申请
		else
		{
			ProtocolThread::GetInstance()->getUserInfoById(button->getTag(), UILoadingIndicator::create(this));
		}
		return;
	}
}

void UIGuild::checkboxCall_func(Ref*pSender,CheckBoxEventType TouchType)
{
	auto l_manager = ((Widget*)pSender)->getParent()->getChildByName<Text*>("label_manager");
	int a = ((Widget*)pSender)->getTag();
	int c = 0;
	if(((CheckBox*)pSender)->isSelected())
	{
		c = 1;
		l_manager->setOpacity(255);
	}else
	{
		c = 0;
		l_manager->setOpacity(127);
	}

	m_gCompetence[a] = c;
	//进行比较是否改变工会权限
	if (m_gCompetence[a] == m_oldgCompetence[a])
	{
		m_bChangeAdmin[a] = false;
	}
	else
	{
		m_bChangeAdmin[a] = true;
	}
}

void UIGuild::flushLeftButton(const bool bHasGuild,const bool bIsadm,const int nNum)
{
	auto left_view = getViewRoot(GUILD_COCOS_RES[GUILD_CSB]);
	auto l_button = dynamic_cast<ListView*>(Helper::seekWidgetByName(left_view,"listview_two_butter"));
	auto b_item = l_button->getItem(5);
	auto t_item = b_item->getChildByName<Text*>("label_applications");
	auto i_unread = b_item->getChildByName<ImageView*>("image_unread");
	if(bHasGuild)
	{
		if (bIsadm)
		{
			b_item->setVisible(true);
			l_button->getItem(6)->setVisible(true);
			b_item->setName("button_request");
			t_item->setString(SINGLE_SHOP->getTipsInfo()["TIP_GULID_BUTTON_APPLICATIONS"]);
			if (nNum)
			{
				i_unread->setVisible(true);
			}else
			{
				i_unread->setVisible(false);
			}
		}else
		{
			b_item->setVisible(false);
			l_button->getItem(6)->setVisible(false);
		}
	}else
	{
		b_item->setVisible(true);
		l_button->getItem(6)->setVisible(true);
		b_item->setName("button_invitation");
		t_item->setString(SINGLE_SHOP->getTipsInfo()["TIP_GULID_BUTTON_INVITATIONS"]);
		if (nNum)
		{
			i_unread->setVisible(true);
		}else
		{
			i_unread->setVisible(false);
		}
	}
}

void UIGuild::flushGuildList()
{
	openView(GUILD_COCOS_RES[GUILD_CSB]);
	auto p_no = m_pRoot->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(false);

	openView(GUILD_COCOS_RES[GUILD_LIST_CSB]);
	m_pRoot->setTouchEnabled(false);
	auto l_guilds = m_pRoot->getChildByName<ListView*>("listview_friend");
	l_guilds->removeAllChildrenWithCleanup(true);

	int num = m_pGuildListResult->invitationnum;
	if (m_pGuildListResult->guildid)
	{
		num = m_pGuildListResult->applicationnum;
	}

	flushLeftButton(m_pGuildListResult->guildid,m_pGuildListResult->memberstatus,num);
	
	auto b_create = m_pRoot->getChildByName<Button*>("button_create");
	b_create->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	auto b_search = m_pRoot->getChildByName<Button*>("button_search");
	b_search->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));

	auto p_page = m_pRoot->getChildByName<Widget*>("panel_page");
	auto b_up = p_page->getChildByName<Button*>("button_pageup");
	auto b_down = p_page->getChildByName<Button*>("button_pagedown");
	auto t_curPage = p_page->getChildByName<Text*>("label_page");
	t_curPage->setString(StringUtils::format("%d",m_nPageIndex));
	b_up->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	b_down->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	if (m_nPageIndex == 1)
	{
		b_up->setBright(false);
		b_up->setTouchEnabled(false);
	}else
	{
		b_up->setBright(true);
		b_up->setTouchEnabled(true);
	}

	if (m_nSameForceGuilds.size()> m_nPageIndex * PAGE_ITEM_NUM)
	{
		b_down->setBright(true);
		b_down->setTouchEnabled(true);
	}else
	{
		b_down->setBright(false);
		b_down->setTouchEnabled(false);
	}

	//if (m_pGuildListResult->n_guilds < 1)
	if (m_nSameForceGuilds.size()<1)
	{
		p_no->setVisible(true);
		p_no->getChildByName<Text*>("label_no_found_1_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_GUILDLIST_NO1"]);
		p_no->getChildByName<Text*>("label_no_found_2_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_GUILDLIST_NO2"]);
		return;
	}

	auto item = m_pRoot->getChildByName<Button*>("panel_guild");
	item->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	for (int i = (m_nPageIndex - 1) * PAGE_ITEM_NUM; i < m_nPageIndex * PAGE_ITEM_NUM && i < m_nSameForceGuilds.size(); i++)
	{
		    auto guildInfo = m_nSameForceGuilds.at(i);
			auto buttonGuild = item->clone();
			buttonGuild->setTag(guildInfo->id);
			auto i_guild_icon = buttonGuild->getChildByName<ImageView*>("image_guild_head");
			auto i_falg_icon = buttonGuild->getChildByName<ImageView*>("image_flag");
			auto t_guild_name = buttonGuild->getChildByName<Text*>("label_guild_name");
			auto t_admin_name = dynamic_cast<Text*>(Helper::seekWidgetByName(buttonGuild, "label_plauer_name"));
			auto t_num = dynamic_cast<Text*>(Helper::seekWidgetByName(buttonGuild, "label_guild_num"));
			auto b_more = buttonGuild->getChildByName<Text*>("button_more");
			b_more->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
			b_more->setTag(guildInfo->id);
			i_guild_icon->ignoreContentAdaptWithSize(false);
			i_guild_icon->loadTexture(getGuildIconPath(guildInfo->guildicon));
			i_falg_icon->ignoreContentAdaptWithSize(false);
			i_falg_icon->loadTexture(getCountryIconPath(guildInfo->nation));
			t_guild_name->setString(guildInfo->name);
			t_admin_name->setString(guildInfo->adminname);
			t_num->setString(StringUtils::format("(%d/%d)", guildInfo->membernum, guildInfo->maxmemnum));
			t_num->setPositionX(t_admin_name->getBoundingBox().size.width + t_admin_name->getPositionX() + 10);
			l_guilds->pushBackCustomItem(buttonGuild);
	
	}
	l_guilds->jumpToTop();
	m_pTempButton = nullptr;
}

void UIGuild::flushMyGuild(GetMyGuildDetailsResult *pMyGulidDetailsResult)
{
	openView(GUILD_COCOS_RES[GUILD_CSB]);
	auto p_no = m_pRoot->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(false);

	m_eTableViewIndex = MY_GUILD_DETAILS;

	openView(GUILD_COCOS_RES[GUILD_MYGUILD_CSB]);	
	m_pRoot->setTouchEnabled(false);
	int num = pMyGulidDetailsResult->invitationnum;
	if (pMyGulidDetailsResult->id)
	{
		num = pMyGulidDetailsResult->applicationnum;
	}
	flushLeftButton(pMyGulidDetailsResult->id,pMyGulidDetailsResult->mystatus,num);
	auto i_title = m_pRoot->getChildByName<ImageView*>("image_title_bg");
	i_title->setVisible(true);
	auto l_myguild = m_pRoot->getChildByName<ListView*>("listview_myguild_info");
	l_myguild->removeAllChildrenWithCleanup(true);

	auto image_pulldown = m_pRoot->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2+9);
	
	if (!pMyGulidDetailsResult->id)
	{
		i_title->setVisible(false);
		p_no->setVisible(true);
		p_no->getChildByName<Text*>("label_no_found_1_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_MYGUILD_NO1"]);
		p_no->getChildByName<Text*>("label_no_found_2_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_MYGUILD_NO2"]);
		l_myguild->setVisible(false);
		image_pulldown->setVisible(false);
		return;
	}
	//公会的名字
	auto i_icon = i_title->getChildByName<ImageView*>("image_guild_head");
	auto i_flag = i_title->getChildByName<ImageView*>("image_flag");
	auto t_name = i_title->getChildByName<Text*>("label_guild_name");
	auto b_info = i_title->getChildByName<Button*>("button_info");
	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture(getGuildIconPath(pMyGulidDetailsResult->guildicon));
	i_flag->ignoreContentAdaptWithSize(false);
	i_flag->loadTexture(getCountryIconPath(pMyGulidDetailsResult->nation));
	t_name->setFontName("");
	t_name->setString(pMyGulidDetailsResult->name);
	b_info->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
	b_info->setTag(pMyGulidDetailsResult->id);

	//公会成员的一些排序
	//排序等级：创建者、管理员（自己优先其次在线优先）、普通成员（自己优先其次在线优先）
	std::vector<int>creatorIndex;
	std::vector<int>secondIndex;
	std::vector<int>thirdIndex;
	std::vector<int>selfIndex;
	auto selfIndexNum = 0;
	//记录所有成员初始详细信息
	std::vector<GuildPersonalInfoDefine *>totalMemberDetails;

	bool find_self = pMyGulidDetailsResult->creatorid == SINGLE_HERO->m_iID ? true : false;
	for (int i = 0; i < pMyGulidDetailsResult->n_members; i++)
	{
		auto myGulidInfo = pMyGulidDetailsResult->members[i];
		totalMemberDetails.push_back(pMyGulidDetailsResult->members[i]);
		if (myGulidInfo->characterid == pMyGulidDetailsResult->creatorid)
		{
			creatorIndex.push_back(i);
		}
		else if (myGulidInfo->memberstatus == 1)
		{
			if (myGulidInfo->online)
			{
				if (!find_self && myGulidInfo->characterid == SINGLE_HERO->m_iID)
				{
					selfIndexNum = 2;
					selfIndex.push_back(i);
					find_self = true;
				}
				else
				{
					secondIndex.insert(secondIndex.begin(), i);
				}
			}
			else
			{
				secondIndex.push_back(i);
			}
		}
		else
		{
			if (myGulidInfo->online)
			{
				if (!find_self && myGulidInfo->characterid == SINGLE_HERO->m_iID)
				{
					selfIndexNum = 3;
					selfIndex.push_back(i);
					find_self = true;
				}
				else
				{
					thirdIndex.insert(thirdIndex.begin(), i);
				}
			}
			else
			{
				thirdIndex.push_back(i);
			}
		}
	}
	//将自己放到相应的等级顺序中
	switch (selfIndexNum)
	{
	case 2:
		secondIndex.insert(secondIndex.begin(), selfIndex[0]);
		break;
	case 3:
		thirdIndex.insert(thirdIndex.begin(), selfIndex[0]);
		break;
	default:
		break;
	}
	//顺序赋值相应成员信息
	auto numIndex = 0;
	for (int i = 0; i < pMyGulidDetailsResult->n_members; i++)
	{
		if (i == 0)
		{
			numIndex = creatorIndex[i];
		}
		else if (i <= secondIndex.size())
		{
			numIndex = secondIndex[i - creatorIndex.size()];
		}
		else
		{
			numIndex = thirdIndex[i - (creatorIndex.size() + secondIndex.size())];
		}
		pMyGulidDetailsResult->members[i] = totalMemberDetails[numIndex];
	}

	creatorIndex.clear();
	secondIndex.clear();
	thirdIndex.clear();
	selfIndex.clear();
	totalMemberDetails.clear();
	m_pMyguildDetails = pMyGulidDetailsResult;
	auto m_p = l_myguild->getParent();
	//添加tableView
	TableView* mTableView = dynamic_cast<TableView*>(m_p->getChildByTag(1024));
	if (!mTableView){
		mTableView = TableView::create(this, Size(l_myguild->getContentSize().width, l_myguild->getContentSize().height));
		m_p->addChild(mTableView, 100);
	}
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(l_myguild->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(1024);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	//mTableView->setBounceable(true);
	this->setCameraMask(_cameraMask, true);
	
	m_pTempButton = nullptr;

}

void UIGuild::flushApplication(const GetApplyInfoResult *pApplyInfoResult)
{
	openView(GUILD_COCOS_RES[GUILD_CSB]);
	auto p_no = m_pRoot->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(false);
	if (pApplyInfoResult->n_applications)
	{
		dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"image_unread"))->setVisible(true);
		
	}else
	{
		dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"image_unread"))->setVisible(false);
	}


	openView(GUILD_COCOS_RES[GUILD_APPLICATIONS_CSB]);	
	m_pRoot->setTouchEnabled(false);

	auto l_guilds = m_pRoot->getChildByName<ListView*>("listview_friend");
	l_guilds->removeAllChildrenWithCleanup(true);
	auto image_pulldown = m_pRoot->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (pApplyInfoResult->n_applications < 1)
	{
		p_no->setVisible(true);
		p_no->getChildByName<Text*>("label_no_found_1_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_APPLICATION_NO1"]);
		p_no->getChildByName<Text*>("label_no_found_2_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_APPLICATION_NO2"]);
		image_pulldown->setVisible(false);
		return;
	}
	auto item = m_pRoot->getChildByName<Widget*>("panel_request");
	item->addTouchEventListener(CC_CALLBACK_2(UIGuild::applicationsEvent,this));
	for (int i = 0; i < pApplyInfoResult->n_applications; i++)
	{
		auto guildinfo = pApplyInfoResult->applications[i];
		auto buttonGuild = item->clone();
		buttonGuild->setTag(guildinfo->characterid);
		auto i_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(buttonGuild,"image_head"));
		auto i_falg = buttonGuild->getChildByName<ImageView*>("image_flag");
		auto t_name = buttonGuild->getChildByName<Text*>("label_player_name");
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(buttonGuild,"label_lv"));
		i_head->ignoreContentAdaptWithSize(false);
		i_head->loadTexture(getPlayerIconPath(guildinfo->iconid));
		i_falg->ignoreContentAdaptWithSize(false);
		i_falg->loadTexture(getCountryIconPath(guildinfo->nationid));
		i_falg->setTag(-2);
		t_name->setString(guildinfo->heroname);
		t_lv->setString(String::createWithFormat("Lv. %d",guildinfo->level)->_string);

		buttonGuild->getChildByName<Button*>("button_accept")->addTouchEventListener(CC_CALLBACK_2(UIGuild::applicationsEvent,this));
		buttonGuild->getChildByName<Button*>("button_refuse")->addTouchEventListener(CC_CALLBACK_2(UIGuild::applicationsEvent, this));
		buttonGuild->getChildByName<Button*>("button_accept")->setTag(i);
		buttonGuild->getChildByName<Button*>("button_refuse")->setTag(i);
		l_guilds->pushBackCustomItem(buttonGuild);
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(l_guilds,image_pulldown);
	m_pTempButton = nullptr;
}

void UIGuild::flushInvitation(const GetInvitationListResult *pInvitInfoResult)
{
	openView(GUILD_COCOS_RES[GUILD_CSB]);
	auto p_no = m_pRoot->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(false);
	if (pInvitInfoResult->n_invitations)
	{
		dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"image_unread"))->setVisible(true);
		
	}else
	{
		dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"image_unread"))->setVisible(false);
	}


	openView(GUILD_COCOS_RES[GUILD_INVITATIONS_CSB]);
	m_pRoot->setTouchEnabled(false);
	auto l_guilds = m_pRoot->getChildByName<ListView*>("listview_friend");
	l_guilds->removeAllChildrenWithCleanup(true);

	auto image_pulldown = m_pRoot->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (pInvitInfoResult->n_invitations < 1)
	{
		p_no->setVisible(true);
		p_no->getChildByName<Text*>("label_no_found_1_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_INVITATIONS_NO1"]);
		p_no->getChildByName<Text*>("label_no_found_2_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_INVITATIONS_NO2"]);
		image_pulldown->setVisible(false);
		return;
	}
	auto item = m_pRoot->getChildByName<Widget*>("panel_guild");
	item->addTouchEventListener(CC_CALLBACK_2(UIGuild::applicationsEvent,this));
	for (int i = 0; i < pInvitInfoResult->n_invitations; i++)
	{
		auto guildinfo = pInvitInfoResult->invitations[i];
		auto buttonGuild = item->clone();
		buttonGuild->setTag(guildinfo->guildid);
		auto i_head = buttonGuild->getChildByName<ImageView*>("image_guild_head");
		auto i_falg = buttonGuild->getChildByName<ImageView*>("image_flag");
		auto t_name = buttonGuild->getChildByName<Text*>("label_guild_name");
		auto t_adm = dynamic_cast<Text*>(Helper::seekWidgetByName(buttonGuild,"label_plauer_name"));
		auto t_num = dynamic_cast<Text*>(Helper::seekWidgetByName(buttonGuild,"label_guild_num"));
		
		i_head->ignoreContentAdaptWithSize(false);
		i_head->loadTexture(getGuildIconPath(guildinfo->iconid));
		i_falg->ignoreContentAdaptWithSize(false);
		i_falg->loadTexture(getCountryIconPath(guildinfo->invitornationid));
		i_falg->setTag(-1);
		t_name->setString(guildinfo->name);
		t_adm->setString(guildinfo->invitorname);
		t_num->setString(StringUtils::format("(%d/%d)",guildinfo->guildcurnum,guildinfo->guildmaxnum));
		t_num->setPositionX(t_adm->getBoundingBox().size.width + t_adm->getPositionX() + 10);
		buttonGuild->getChildByName<Button*>("button_guild_accept")->addTouchEventListener(CC_CALLBACK_2(UIGuild::applicationsEvent,this));
		buttonGuild->getChildByName<Button*>("button_guild_refuse")->addTouchEventListener(CC_CALLBACK_2(UIGuild::applicationsEvent,this));
		buttonGuild->getChildByName<Button*>("button_guild_accept")->setTag(guildinfo->invitationid);
		buttonGuild->getChildByName<Button*>("button_guild_refuse")->setTag(guildinfo->invitationid);
		l_guilds->pushBackCustomItem(buttonGuild);
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(l_guilds,image_pulldown);
	m_pTempButton = nullptr;
}

void UIGuild::textFiledEvent(Ref* target, bool type)
{
	{
		std::string value = m_pInputText->getText();	
		auto p_myguild = m_pRoot->getChildByName<Widget*>("panel_myguild");
		TableView* mTableView = dynamic_cast<TableView*>(p_myguild->getChildByTag(1024));
		Text*l_signature = nullptr;
		if (mTableView)
		{
			auto  cell = mTableView->cellAtIndex(0);
			Widget* cellItem = dynamic_cast<Widget*>(cell->getChildByTag(0));
			l_signature = dynamic_cast<Text*>(Helper::seekWidgetByName(cellItem, "text_announce"));
		}
		else
		{
			return;
		}
		if (illegal_character_check(value))
		{
			value = "";
			m_pInputText->setText(value.c_str());
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
			l_signature->setString(value);
			return;
		}
		
		auto Signheight = getLabelHight(value, l_signature->getBoundingBox().size.width, l_signature->getFontName());
		if (type)
		{
			bIsEnter = false;
			if (Signheight>120)
			{
				l_signature->setString("");
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ROLE_PERSONAL_SIGN_TOO_LONG");
			}
			else
			{
				l_signature->setString(value);
				char *str = (char *)value.c_str();
				ProtocolThread::GetInstance()->changeGuildIntro(str, UILoadingIndicator::create(this));
			}		
		}
	}
}

bool UIGuild::isButtonInListView(std::string& name,Widget *target)
{
	auto left_view = getViewRoot(GUILD_COCOS_RES[GUILD_CSB]);
	if (target == nullptr)
	{
		target = Helper::seekWidgetByName(left_view,name.data());
	}
	if (target == m_pPreButton)
	{
		return false;
	}

	if (!target)
	{
		return false;
	}

	if(target != nullptr)
	{
		target->setBright(false);
		dynamic_cast<Text*>(target->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
	}
	if (m_pPreButton != nullptr)
	{
		m_pPreButton->setBright(true);
		dynamic_cast<Text*>(m_pPreButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}

	m_pPreButton = target;
	return true;
}

void UIGuild::openSearchView()
{
	openView(GUILD_COCOS_RES[GUILD_SEARCH_RESULT_CSB]);
	auto l_guild = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_pRoot,"listview_friend"));
	l_guild->removeAllChildrenWithCleanup(true);
	auto p_no = m_pRoot->getChildByName<Widget*>("panel_search_no");
	p_no->setVisible(false);

	auto p_page = m_pRoot->getChildByName<Widget*>("panel_page");
	auto b_up = p_page->getChildByName<Button*>("button_pageup");
	auto b_down = p_page->getChildByName<Button*>("button_pagedown");
	auto t_curPage = p_page->getChildByName<Text*>("label_page");
	t_curPage->setString(StringUtils::format("%d", m_nPageSearchIndex));
	b_up->addTouchEventListener(CC_CALLBACK_2(UIGuild::menuCall_func,this));
	b_down->addTouchEventListener(CC_CALLBACK_2(UIGuild::menuCall_func,this));
	if (m_nPageSearchIndex == 1)
	{
		b_up->setBright(false);
		b_up->setTouchEnabled(false);
	}else
	{
		b_up->setBright(true);
		b_up->setTouchEnabled(true);
	}

	if (m_pSearchGuildListResult->n_guilds > m_nPageSearchIndex * PAGE_ITEM_NUM)
	{
		b_down->setBright(true);
		b_down->setTouchEnabled(true);
	}else
	{
		b_down->setBright(false);
		b_down->setTouchEnabled(false);
	}

	if (m_pSearchGuildListResult->n_guilds < 1)
	{
		p_no->setVisible(true);
		return;
	}

	auto item = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pRoot,"panel_guild"));
	item->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	for (int i = (m_nPageSearchIndex - 1) * PAGE_ITEM_NUM; i < m_nPageSearchIndex * PAGE_ITEM_NUM && i < m_pSearchGuildListResult->n_guilds; i++)
	{
		    auto guildInfo = m_pSearchGuildListResult->guilds[i];
			auto buttonGuild = item->clone();
			buttonGuild->setTag(guildInfo->id);
			auto i_guild_icon = buttonGuild->getChildByName<ImageView*>("image_guild_head");
			auto i_falg_icon = buttonGuild->getChildByName<ImageView*>("image_flag");
			auto t_guild_name = buttonGuild->getChildByName<Text*>("label_guild_name");
			auto t_admin_name = dynamic_cast<Text*>(Helper::seekWidgetByName(buttonGuild, "label_plauer_name"));
			auto t_num = dynamic_cast<Text*>(Helper::seekWidgetByName(buttonGuild, "label_guild_num"));
			auto b_guild = buttonGuild->getChildByName<Text*>("button_add_guild");
			b_guild->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
			b_guild->setTag(guildInfo->id);
			i_guild_icon->ignoreContentAdaptWithSize(false);
			i_guild_icon->loadTexture(getGuildIconPath(guildInfo->guildicon));
			i_falg_icon->ignoreContentAdaptWithSize(false);
			i_falg_icon->loadTexture(getCountryIconPath(guildInfo->nation));
			t_guild_name->setString(guildInfo->name);
			t_admin_name->setString(guildInfo->adminname);
			t_num->setString(StringUtils::format("(%d/%d)", guildInfo->membernum, guildInfo->maxmemnum));
			t_num->setPositionX(t_admin_name->getBoundingBox().size.width + t_admin_name->getPositionX() + 10);
			l_guild->pushBackCustomItem(buttonGuild);
	}
}

void UIGuild::openGuildDetailView(const GetGuildDetailsResult *pGuildDetails)
{
	openView(COMMOM_COCOS_RES[C_VIEW_GUILD_CSB]);
	auto panel_detail = getViewRoot(COMMOM_COCOS_RES[C_VIEW_GUILD_CSB]);
	auto l_guilds = panel_detail->getChildByName<ListView*>("listview_guild_info");
	l_guilds->jumpToTop();

	auto p_bio = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_detail,"panel_bio_info"));
	auto i_icon = p_bio->getChildByName<ImageView*>("image_guild_head");
	auto i_flag = p_bio->getChildByName<ImageView*>("image_flag");
	auto t_name = p_bio->getChildByName<Text*>("label_guild_name");
	auto t_content = p_bio->getChildByName<Text*>("label_bio_content");
	auto b_edit = p_bio->getChildByName<Button*>("button_edit");
	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture(getGuildIconPath(pGuildDetails->guildicon));
	i_flag->ignoreContentAdaptWithSize(false);
	i_flag->loadTexture(getCountryIconPath(pGuildDetails->nation));
	i_flag->setTouchEnabled(true);
	i_flag->addTouchEventListener(CC_CALLBACK_2(UIGuild::menuCall_func,this));
	i_flag->setTag(pGuildDetails->nation+IMAGE_ICON_CLOCK+IMAGE_INDEX2);
	t_name->setString(pGuildDetails->name);
	if (pGuildDetails->intro)
	{
		if (strcmp(pGuildDetails->intro, "") != 0)
		{
			t_content->setString(pGuildDetails->intro);
		}
	}
	b_edit->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	
	auto l_guild = dynamic_cast<ListView*>(Helper::seekWidgetByName(panel_detail,"listview_guild_name_num"));
	auto l_leader = l_guild->getItem(1);
	auto l_member = l_guild->getItem(3);
	auto t_leader_name = dynamic_cast<Text*>(Helper::seekWidgetByName(l_leader,"label_guild_name"));
	auto i_flag_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(l_leader,"image_guild_icon"));
	auto t_member = l_member->getChildByName<Text*>("label_time");
	i_flag_icon->ignoreContentAdaptWithSize(false);
	i_flag_icon->loadTexture(getCountryIconPath(pGuildDetails->nation));
	i_flag_icon->setTouchEnabled(true);
	i_flag_icon->addTouchEventListener(CC_CALLBACK_2(UIGuild::menuCall_func,this));
	i_flag_icon->setTag(pGuildDetails->nation+IMAGE_ICON_CLOCK+IMAGE_INDEX2);
	t_leader_name->setString(pGuildDetails->creatorname);
	t_leader_name->setTouchEnabled(true);
	t_leader_name->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	t_leader_name->setTag(pGuildDetails->creatorid);
	t_member->setString(StringUtils::format("%d/%d",pGuildDetails->currentmemnum,pGuildDetails->maxmembernum));
	i_flag_icon->setPositionX(t_leader_name->getPositionX() - t_leader_name->getBoundingBox().size.width - i_flag_icon->getBoundingBox().size.width/2 - 10);
	//公会不在具有自己的友好度
#if 0
	for (int i = 1; i <  pGuildDetails->n_relations; i++)
	{
		for (int j = 0; j < pGuildDetails->n_relations-i; j++)
		{
			if (pGuildDetails->relations[j]->forceid > pGuildDetails->relations[j+1]->forceid)
			{
				auto temp = pGuildDetails->relations[j];
				pGuildDetails->relations[j] = pGuildDetails->relations[j+1];
				pGuildDetails->relations[j+1] = temp;
			}
		}
	}
	auto personal_friend = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_detail,"image_country_friendliness"));
	ImageView *p_friend = nullptr;
	int p_friend_num = 0;
	for(int i = 1; i <= 7; i++)
	{
		switch(i)
		{
		case 1:
			p_friend = personal_friend->getChildByName<ImageView *>("image_portugal");
			p_friend->setTag(IMAGE_ICON_PORTUGAL+IMAGE_INDEX2);
			break;
		case 2:
			p_friend = personal_friend->getChildByName<ImageView *>("image_spain");
			p_friend->setTag(IMAGE_ICON_SPAIN+IMAGE_INDEX2);
			break;
		case 3:
			p_friend = personal_friend->getChildByName<ImageView *>("image_england");
			p_friend->setTag(IMAGE_ICON_ENGLAND+IMAGE_INDEX2);
			break;
		case 4:
			p_friend = personal_friend->getChildByName<ImageView *>("image_venice");
			p_friend->setTag(IMAGE_ICON_NETHERLANDS+IMAGE_INDEX2);
			break;
		case 5:
			p_friend = personal_friend->getChildByName<ImageView *>("image_france");
			p_friend->setTag(IMAGE_ICON_GENOVA+IMAGE_INDEX2);
			break;
		case 6:
			p_friend = personal_friend->getChildByName<ImageView *>("image_holland");
			p_friend->setTag(IMAGE_ICON_VENICE+IMAGE_INDEX1);
			break;
		case 7:
			p_friend = personal_friend->getChildByName<ImageView *>("image_sweden");
			p_friend->setTag(IMAGE_ICON_SWEDEN+IMAGE_INDEX1);
			break;
		default:
			break;
		}
		p_friend->loadTexture(getCountryIconPath(i));
		p_friend->addTouchEventListener(CC_CALLBACK_2(GuildLayer::menuCall_func, this));
		p_friend->setTouchEnabled(true);
		if (p_friend != nullptr)
		{
			if (pGuildDetails->relations[i - 1]->forcerelationvalue < 0 && pGuildDetails->relations[i - 1]->forcerelationvalue >= -1000)
			{
				p_friend_num = -1;
			}
			else if (pGuildDetails->relations[i - 1]->forcerelationvalue < -1000)
			{
				p_friend_num = pGuildDetails->relations[i - 1]->forcerelationvalue / 1000.0 - 0.5;
			}
			else
			{
				p_friend_num = pGuildDetails->relations[i - 1]->forcerelationvalue / 1000.0 + 0.5;
			}

			p_friend->loadTexture(getCountryIconPath(pGuildDetails->relations[i-1]->forceid));
			auto  i_friend = p_friend->getChildByName<ImageView*>("image_friendliness_status");
			if (p_friend_num >= 0)
			{
				i_friend->loadTexture(ICON_FRIEND_NPC);
			}else
			{
				i_friend->loadTexture(ICON_ENEMY_NPC);
			}
			Text *t_num = p_friend->getChildByName<Text*>("label_num");
			t_num->setString(String::createWithFormat("%d",p_friend_num)->_string);
		}
	}
#endif
	auto b_quit = panel_detail->getChildByName<Button*>("button_quit");
	auto b_join = panel_detail->getChildByName<Button*>("button_join_guild");
	auto b_miss = panel_detail->getChildByName<Button*>("button_dismiss");
	b_quit->setTag(pGuildDetails->id);
	b_join->setTag(pGuildDetails->id);
	b_miss->setTag(pGuildDetails->id);
	if (pGuildDetails->myguildid)
	{
		if (pGuildDetails->myguildid == pGuildDetails->id)
		{
			b_join->setVisible(false);
			if (pGuildDetails->mystatus == 2)
			{
				b_miss->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
				b_quit->setVisible(false);
				b_miss->setVisible(true);
			}else
			{
				b_quit->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
				b_quit->setVisible(true);
				b_miss->setVisible(false);
			}
		}else
		{
			b_join->setVisible(false);
			b_quit->setVisible(false);
			b_miss->setVisible(false);
		}
		
	}else
	{
		b_join->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
		b_join->setVisible(true);
		b_quit->setVisible(false);
		b_miss->setVisible(false);
	}
	l_guilds->refreshView();
	auto image_pulldown = panel_detail->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(l_guilds,image_pulldown);
}

void UIGuild::openManagerView(const GetGuildMemberPermissionResult *pGulidMember)
{
	m_eTableViewIndex = MY_GUILD_MANAGER;

	openView(GUILD_COCOS_RES[GUILD_MANAGER_CSB]);
	auto l_guild = m_pRoot->getChildByName<ListView*>("listview_guild");
	TableView* mTableView = dynamic_cast<TableView*>(l_guild->getParent()->getChildByTag(1025));
	Vec2 ss = tableCellSizeForIndex(mTableView, 0);
	switch (m_eMyGuildActionIndex)
	{
	case UIGuild::NONE:
		break;
	case UIGuild::MY_GUILD_DELETE_MEMBER:
		//mTableView->cellAtIndex(m_nGuildMemberIndex)->removeFromParentAndCleanup(true);
		mTableView->reloadData();
		m_eMyGuildActionIndex = MY_GUILD_ACTION_NONE;
		return;
		break;
	case UIGuild::MY_GUILD_SET_ADMIN:
		for (int i = 0; i < pGulidMember->n_members; i++)
		{
			if (m_bChangeAdmin[i])
			{
				mTableView->updateCellAtIndex(m_bChangeAdmin[i]);
			}
		}
		m_eMyGuildActionIndex = MY_GUILD_ACTION_NONE;
		return;
		break;
	default:
		break;
	}

	auto button_close = m_pRoot->getChildByName<Button*>("button_close");
	button_close->setName("b_close");
	button_close->addTouchEventListener(CC_CALLBACK_2(UIGuild::menuCall_func, this));
	l_guild->removeAllChildrenWithCleanup(true);
	auto b_player = m_pRoot->getChildByName<Widget*>("panel_player");
	b_player->setTouchEnabled(false);
	m_pRoot->getChildByName<Button*>("button_ok")->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent,this));
	m_pRoot->getChildByName<Button*>("button_ok")->setVisible(true);
	auto p_no = m_pRoot->getChildByName<Widget*>("panel_search_no");
	p_no->setVisible(false);
	auto image_pulldown = m_pRoot->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	image_pulldown->setVisible(false);
	if (pGulidMember->n_members < 1)
	{
		m_pRoot->getChildByName<Button*>("button_ok")->setVisible(false);
		p_no->setVisible(true);
		return;
	}
	auto b_manager = b_player->getChildByName<Widget*>("button_manager");
	if (pGulidMember->create_id == SINGLE_HERO->m_iID)
	{
		b_manager->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent,this));
		b_manager->setVisible(true);
	}else
	{
		b_manager->setVisible(false);
		m_pRoot->getChildByName<Button*>("button_ok")->setVisible(false);
	}
	
	//添加tableView
	
	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(l_guild->getContentSize().width, l_guild->getContentSize().height));
		l_guild->getParent()->addChild(mTableView, 100);
	}
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(l_guild->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(1025);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	mTableView->setBounceable(true);
	this->setCameraMask(_cameraMask, true);

}

void UIGuild::openCreateGuildView()
{
	openView(GUILD_COCOS_RES[GUILD_CREAT_CSB]);
	srand((unsigned)time(NULL));
	m_nGuildIconid = cocos2d::random() % 5+1;
	auto i_icon = m_pRoot->getChildByName<ImageView*>("image_guild_head");
	auto t_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_guild_name"));
	auto t_slogon = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_guild_slogon"));
	auto p_slogon = m_pRoot->getChildByName<Widget*>("panel_slogon");
	auto label_slogon = p_slogon->getChildByName<Text*>("label_slogon");
	auto label_content = p_slogon->getChildByName<Text*>("label_content");
	label_content->setPositionX(label_slogon->getPositionX() + label_slogon->getContentSize().width);
	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture(getGuildIconPath(m_nGuildIconid));
	t_name->setString("");
	t_slogon->setString("");
	t_name->setTextVerticalAlignment(TextVAlignment::CENTER);
	t_slogon->setFontName("");
	t_name->setPlaceHolderColor(Color3B(116,98,71));
	m_pRoot->getChildByName<Button*>("button_left")->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	m_pRoot->getChildByName<Button*>("button_right")->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	m_pRoot->getChildByName<Button*>("button_creat")->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	auto t_price = m_pRoot->getChildByName<Button*>("button_creat")->getChildByName<Text*>("label_price");
	t_price->setString(numSegment("1000000"));
	if (SINGLE_HERO->m_iCoin < 1000000)
	{
		t_price->setTextColor(Color4B(255, 48, 0, 255));
	}else
	{
		t_price->setTextColor(Color4B(255, 255, 255, 255));
	}
	t_name->setVisible(false);
	t_slogon->setVisible(false);
	auto size_name = t_name->getContentSize();
	auto size_slogon = t_slogon->getContentSize();
	auto tf_name = t_name->getParent();
	auto tf_slogon = t_slogon->getParent();
	cocos2d::ui::EditBox* editbox_name = nullptr;
	cocos2d::ui::EditBox* editbox_slogon = nullptr;
	if (!editbox_name)
	{
		editbox_name = cocos2d::ui::EditBox::create(Size(size_name), "input.png");
		tf_name->addChild(editbox_name,9999);
		editbox_name->setTag(1);
		editbox_name->setPosition(t_name->getPosition());
		editbox_name->setAnchorPoint(tf_name->getAnchorPoint());
		editbox_name->setPlaceholderFontColor(Color3B(116, 98, 71));
		editbox_name->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
		editbox_name->setFontColor(Color3B(46, 29, 14));
		editbox_name->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		editbox_name->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	}
	if (!editbox_slogon)
	{
		editbox_slogon = cocos2d::ui::EditBox::create(Size(size_slogon), "input.png");
		tf_slogon->addChild(editbox_slogon,10000);
		editbox_slogon->setTag(2);
		editbox_slogon->setPosition(t_slogon->getPosition());
		editbox_slogon->setAnchorPoint(tf_slogon->getAnchorPoint());
		editbox_slogon->setPlaceholderFontColor(Color3B(116, 98, 71));
		editbox_slogon->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
		editbox_slogon->setFontColor(Color3B(46, 29, 14));
		editbox_slogon->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		editbox_slogon->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	}
	scheduleUpdate();
}

void UIGuild::openCreateGuildFailView(const TestCreateNewGuildResult *pResult)
{
	openView(GUILD_COCOS_RES[GUILD_CREAT_FAILED_CSB]);
	m_pRoot->getChildByName<Button*>("button_yes")->addTouchEventListener(CC_CALLBACK_2(UIGuild::guildListEvent,this));
	m_pRoot->getChildByName<Button*>("button_yes")->setTag(1);
	auto t_no = m_pRoot->getChildByName<Widget*>("label_no");
	auto t_require = m_pRoot->getChildByName<Widget*>("label_require");
	auto i_exp_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(t_require,"label_lv"));
	auto i_fame_require = dynamic_cast<Text*>(Helper::seekWidgetByName(t_require, "label_silver"));
	i_exp_lv->setString(SINGLE_SHOP->getTipsInfo()["TIP_CREATE_GUILD_REQUIRE_HERO_EXP_LV"] + StringUtils::format("    Lv.%d", pResult->reqlv));
	i_fame_require->setString(SINGLE_SHOP->getTipsInfo()["TIP_CREATE_GUILD_REQUIRE_FAMES"] + StringUtils::format("    %d", pResult->reqfame));
	if (SINGLE_HERO->m_iGuildId)
	{
		t_require->setVisible(false);
		t_no->setVisible(true);
		m_pRoot->getChildByName<Button*>("button_yes")->setTag(0);
		return;
	}else
	{
		t_require->setVisible(true);
		t_no->setVisible(false);
	}
	bool isCan = true;
	auto i_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"image_right"));
	auto i_fame = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"image_wrong"));
	if (pResult->reqlv > pResult->curlv)
	{
		i_lv->loadTexture("cocosstudio/login_ui/guild_720/wrong.png");
		m_pRoot->getChildByName<Button*>("button_yes")->setTag(0);
		isCan = false;
	}else
	{
		i_lv->loadTexture("cocosstudio/login_ui/guild_720/right.png");
	}
	if (pResult->reqfame > pResult->curfame)
	{
		i_fame->loadTexture("cocosstudio/login_ui/guild_720/wrong.png");
		m_pRoot->getChildByName<Button*>("button_yes")->setTag(0);
		isCan = false;
	}else
	{
		i_fame->loadTexture("cocosstudio/login_ui/guild_720/right.png");
	}
	if (isCan)
	{
		closeView();
		openCreateGuildView();
	}
}

void UIGuild::flushMembersNum(const ExpandGuildCapacityResult *pExpandGuildResult)
{
	auto p_myguild = getViewRoot(GUILD_COCOS_RES[GUILD_MYGUILD_CSB]);
	auto image_pulldown = p_myguild->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 9);
	auto l_myguild = p_myguild->getChildByName<ListView*>("listview_myguild_info");
	m_pMyguildDetails->currentmemnum = pExpandGuildResult->curmembernum;
	m_pMyguildDetails->maxmembernum = pExpandGuildResult->newcapacity;
	TableView* mTableView = dynamic_cast<TableView*>(l_myguild->getParent()->getChildByTag(1024));
	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(l_myguild->getContentSize().width, l_myguild->getContentSize().height));
		l_myguild->getParent()->addChild(mTableView, 100);
	}
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(l_myguild->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(1024);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	mTableView->setBounceable(true);
	this->setCameraMask(_cameraMask, true);
}

void UIGuild::quitGuildEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = (Button*)pSender;
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_quit))
	{
		 
		if (button->getTag() == QUIT_GUILD)
		{
			m_pTempButton = button;
			m_eConfirmIndex = CONFIRM_INDEX_EQIT_GUILD;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_GUILD_EXIT_GUILD", "TIP_GUILD_EXIT_GUILD");
		}
		else if (button->getTag() == DISMISS_GUID)
		{
			m_pTempButton = button;
			m_eConfirmIndex = CONFIRM_INDEX_DISMISS_GUILD;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_GUILD_DISMISS_GUILD_TITLE", "TIP_GUILD_DISMISS_GUILD");
		}

		return;
	}
}

void UIGuild::update(float delta)
{
	auto view = getViewRoot(GUILD_COCOS_RES[GUILD_CREAT_CSB]);
	if (view)
	{
		TextField* tf_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_guild_name"));
		cocos2d::ui::EditBox* editbox_name = dynamic_cast<cocos2d::ui::EditBox*>(tf_name->getParent()->getChildByTag(1));
		std::string name = editbox_name->getText();

		TextField* t_slogon = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_guild_slogon"));
		cocos2d::ui::EditBox* editbox_slogon = dynamic_cast<cocos2d::ui::EditBox*>(t_slogon->getParent()->getChildByTag(2));
		std::string slogon = editbox_slogon->getText();

		if (name.size() >0)
		{
			editbox_name->setOpacity(255);
		}
		else
		{
			editbox_name->setOpacity(127);
		}
		if (slogon.size() > 0)
		{
			editbox_slogon->setOpacity(255);
		}
		else
		{
			editbox_slogon->setOpacity(127);
		}

		if (illegal_character_check(name))
		{
			editbox_name->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
		if (illegal_character_check(slogon))
		{
			editbox_slogon->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
	}
	auto view1 = getViewRoot(GUILD_COCOS_RES[GUILD_SEARCH_CSB]);
	if (view1)
	{
		auto t_name = view1->getChildByName<TextField*>("textfield_guild_name");
		cocos2d::ui::EditBox* editbox_name = dynamic_cast<cocos2d::ui::EditBox*>(t_name->getParent()->getChildByTag(1));
		std::string name = editbox_name->getText();
		if (name.size() >0)
		{
			editbox_name->setOpacity(255);
		}
		else
		{
			editbox_name->setOpacity(127);
		}

		if (illegal_character_check(name))
		{
			editbox_name->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
	}	
}
void UIGuild::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
	auto pos = view->getContentOffset();
	auto viewSize = view->getViewSize();
	auto contentSize = view->getContainer()->getContentSize();

	float bottom = viewSize.height - contentSize.height;
	float top = 0;
	float totalLen = top - bottom;
	float currentLen = top - pos.y;

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
void UIGuild::tableCellTouched(TableView* table, TableViewCell* cell)
{
}
Size UIGuild::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	switch (m_eTableViewIndex)
	{
	case UIGuild::MY_GUILD_DETAILS:
	{
		auto view = getViewRoot(GUILD_COCOS_RES[GUILD_MYGUILD_CSB]);
		auto listviews = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_myguild_info"));
		auto panel_myguild_info = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_myguild_info"));
		auto panel_myguild_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_myguild_item"));
		if (idx == 0)
		{
			return panel_myguild_info->getContentSize();
		}
		else
		{
			return panel_myguild_item->getContentSize();
		}
	}
		break;
	case UIGuild::MY_GUILD_MANAGER:
	{
		auto view = getViewRoot(GUILD_COCOS_RES[GUILD_MANAGER_CSB]);
		auto b_player = m_pRoot->getChildByName<Widget*>("panel_player");
		return b_player->getContentSize();
	}
		break;
	default:
		break;
	}
	
	return Size(0, 0);
}
TableViewCell * UIGuild::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell *cell = table->dequeueCell();
	Widget* cell_item;
	cell = new TableViewCell();
	cell->autorelease();
	switch (m_eTableViewIndex)
	{
	case UIGuild::MY_GUILD_DETAILS:
	{
											auto view = getViewRoot(GUILD_COCOS_RES[GUILD_MYGUILD_CSB]);
											auto listviews = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_myguild_info"));
											auto panel_myguild_info = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_myguild_info"));
											auto panel_myguild_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_myguild_item"));
											if (idx == 0)
											{
												cell_item = panel_myguild_info->clone();
											}
											else
											{
												cell_item = panel_myguild_item->clone();
											}

											break;
	}
	case  UIGuild::MY_GUILD_MANAGER:
	{
											auto view = getViewRoot(GUILD_COCOS_RES[GUILD_MANAGER_CSB]);
											auto l_guild = m_pRoot->getChildByName<ListView*>("listview_guild");
											l_guild->removeAllChildrenWithCleanup(true);
											auto b_player = m_pRoot->getChildByName<Widget*>("panel_player");
											cell_item = b_player->clone();
											break;
	}
	default:
		break;
	}
	cell->addChild(cell_item, 1);
	cell_item->setPosition(Vec2(0, 0));
	cell_item->setCameraMask(_cameraMask);
	cell->setVisible(true);
	cell->setTag(idx);

	cell_item->setTouchEnabled(false);
	cell_item->setVisible(true);
	cell_item->setSwallowTouches(false);

	switch (m_eTableViewIndex)
	{
	case UIGuild::MY_GUILD_DETAILS:
	{
		if (idx == 0)
		{
			cell_item->setTag(0);
			auto b_edit = cell_item->getChildByName<Button*>("button_edit");
			auto b_manage = cell_item->getChildByName<Button*>("button_manage");
			auto t_announce = cell_item->getChildByName<Text*>("text_announce");

			b_edit->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			b_manage->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			t_announce->setString(m_pMyguildDetails->intro);
			if (m_pMyguildDetails->mystatus)
			{
				b_edit->setVisible(true);
				b_manage->setVisible(true);
				b_edit->setBright(true);
				b_manage->setBright(true);
				m_eConfirmIndex = CONFIRM_INDEX_ACCESS;
			}
			else
			{
				b_edit->setVisible(false);
				b_manage->setVisible(false);
				m_eConfirmIndex = CONFIRM_INDEX_NOT_ACCESS;
			}

			auto w_title = cell_item->getChildByName<Widget*>("panel_title_member");
			auto b_add = w_title->getChildByName<Button*>("button_add");
			auto t_num = w_title->getChildByName<Text*>("label_num");
			auto t_title_member = w_title->getChildByName<Text*>("label_title_member");

			t_num->setString(StringUtils::format("(%d/%d)", m_pMyguildDetails->n_members, m_pMyguildDetails->maxmembernum));
			b_add->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			b_add->setTag(m_pMyguildDetails->id);
			if (m_pMyguildDetails->mystatus == 1 && strcmp(m_pMyguildDetails->creatorname, SINGLE_HERO->m_sName.c_str()) == 0)
			{
				b_add->setVisible(true);
				b_add->setTouchEnabled(true);
				b_add->setBright(true);
			}
			else
			{
				b_add->setVisible(true);
				b_add->setTouchEnabled(false);
				b_add->setBright(false);
			}
			t_title_member->ignoreContentAdaptWithSize(true);
			t_num->ignoreContentAdaptWithSize(true);
			t_num->setPositionX(t_title_member->getPositionX() + t_title_member->getContentSize().width);
			b_add->setPositionX(t_num->getPositionX() + t_num->getContentSize().width + b_add->getContentSize().width / 2 + 5);
		}
		else
		{
			
			cell_item->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			auto myGulidInfo = m_pMyguildDetails->members[idx - 1];
			cell_item->setTag(myGulidInfo->characterid);
			cell_item->setName(myGulidInfo->heroname);
			auto i_play_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_guild_head"));
			i_play_head->setTouchEnabled(true);
			auto i_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_head"));
			auto i_flag = cell_item->getChildByName<ImageView*>("image_flag");
			auto t_name = cell_item->getChildByName<Text*>("label_player_name");
			auto i_admin = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_player_icon"));
			auto t_admin = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_plauer_name"));
			auto b_online = dynamic_cast<Button*>(Helper::seekWidgetByName(cell_item, "button_line"));
			auto b_quit = cell_item->getChildByName<Button*>("button_quit");
			i_head->ignoreContentAdaptWithSize(false);
			i_head->loadTexture(getPlayerIconPath(myGulidInfo->iconid));
			i_flag->ignoreContentAdaptWithSize(false);
			i_flag->loadTexture(getCountryIconPath(myGulidInfo->nationid));
			t_name->setString(myGulidInfo->heroname);
			if (myGulidInfo->memberstatus)
			{
				i_admin->setVisible(true);
				t_admin->setVisible(true);

				if (myGulidInfo->memberstatus == 1)
				{
					i_admin->loadTexture("res/shipAttIcon/admin.png");
					t_admin->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_GUILD_ADM"]);
					b_quit->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_QUIT"]);
					b_quit->setTag(QUIT_GUILD);
				}
				else
				{
					i_admin->loadTexture("res/shipAttIcon/personal.png");
					t_admin->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_GUILD_ONWER"]);
					b_quit->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_DISMISS"]);
					b_quit->setTag(DISMISS_GUID);
				}
			}
			else
			{
				i_admin->setVisible(false);
				t_admin->setVisible(false);
				t_admin->setString(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_GUILD_ADM"]);
				b_quit->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_GUILD_QUIT"]);
				b_quit->setTag(QUIT_GUILD);
			}
			if (myGulidInfo->online)
			{
				b_online->setBright(true);
			}
			else
			{
				b_online->setBright(false);
			}
			auto b_chat = cell_item->getChildByName<Button*>("button_chat");
			auto b_mail = cell_item->getChildByName<Button*>("button_mail");
			auto b_info = cell_item->getChildByName<Button*>("button_player_info");
			b_chat->setTag(myGulidInfo->online);
			if (myGulidInfo->characterid == SINGLE_HERO->m_iID)
			{
				b_mail->setVisible(false);
				b_chat->setVisible(false);
				b_quit->setVisible(true);
			}
			else
			{
				b_mail->setVisible(true);
				b_chat->setVisible(true);
				b_quit->setVisible(false);
			}
			b_mail->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			b_chat->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			i_play_head->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			b_info->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			b_quit->addTouchEventListener(CC_CALLBACK_2(UIGuild::quitGuildEvent, this));
		}
	}
		break;
	case UIGuild::MY_GUILD_MANAGER:
	{
		if (m_pMyguildManager && m_pMyguildManager->members)
		{
			auto guildDefine = m_pMyguildManager->members[idx];
			auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cell_item, "image_head"));
			auto i_flag = cell_item->getChildByName<ImageView*>("image_flag");
			auto t_name = cell_item->getChildByName<Text*>("label_player_name");
			auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_lv"));
			auto b_manager = cell_item->getChildByName<Button*>("button_manager");
			auto c_box = b_manager->getChildByName<CheckBox*>("checkbox_268");
			auto l_manager = b_manager->getChildByName<Text*>("label_manager");
			auto b_delete = cell_item->getChildByName<Button*>("button_delete");

			i_icon->ignoreContentAdaptWithSize(false);
			i_icon->loadTexture(getPlayerIconPath(guildDefine->iconidx));
			i_flag->ignoreContentAdaptWithSize(false);
			i_flag->loadTexture(getCountryIconPath(guildDefine->nation));
			t_name->setString(guildDefine->heroname);
			t_lv->setString(StringUtils::format("Lv. %d", guildDefine->level));
			log("idx = %d name = %s", idx, guildDefine->heroname);
			c_box->addEventListenerCheckBox(this, checkboxselectedeventselector(UIGuild::checkboxCall_func));
			c_box->setTag(idx);

			if (guildDefine->admin)
			{
				c_box->setSelectedState(true);
				l_manager->setOpacity(255);
				m_gCompetence[idx] = 1;
			}
			else
			{
				c_box->setSelectedState(false);
				l_manager->setOpacity(127);
				m_gCompetence[idx] = 0;
			}

			b_delete->addTouchEventListener(CC_CALLBACK_2(UIGuild::myGuildListEvent, this));
			b_delete->setTag(guildDefine->character_id);
			b_manager->setTag(guildDefine->character_id);
		}
	}
		break;
	default:
		break;
	}
	
	return cell;
}
ssize_t UIGuild::numberOfCellsInTableView(TableView *table)
{
	switch (m_eTableViewIndex)
	{
	case UIGuild::MY_GUILD_DETAILS:
		if (m_pMyguildDetails)
		{
			return (m_pMyguildDetails->n_members + 1);
		}
		else
		{
			return 0;
		}
		break;
	case UIGuild::MY_GUILD_MANAGER:
		if (m_pMyguildManager)
		{
			return (m_pMyguildManager->n_members);
		}
		else
		{
			return 0;
		}
		break;
	default:
		return 0;
		break;
	}
	
}
void UIGuild::editBoxReturn(ui::EditBox* editBox)
{
	if (editBox->getTag() == m_pInputText->getTag())
	{
		bIsEnter = true;
		this->textFiledEvent(m_pInputText, bIsEnter);
	}
}
