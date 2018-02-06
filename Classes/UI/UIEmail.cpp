#include "UIEmail.h"

#include "UICommon.h"
#include "UIInform.h"
#include "UIMain.h"
#include "UISailManage.h"
#include "UISailHUD.h"
#include "TVSceneLoader.h"
#include "UIVoyageSlider.h"
#include "TVBattleManager.h"
#include "Utils.h"
#include "TimeUtil.h"
//#include "ProtocolThread.h"

UIEmail*  UIEmail::m_pEmailLayer = nullptr; 

UIEmail::UIEmail() :
	m_pMailListResult(nullptr),
	m_pItemResult(nullptr),
	m_pTempButton(nullptr),
	m_nItemNum(0),
	m_nItemId(0),
	m_nItemType(0),
	m_nItemIid(0),
	m_nEmailIndex(0),
    m_nslider(0,0),
	m_eConfirmIndex(CONFIRM_INDEX_DELETE_INBOX_EMAIL),
	m_bIsIntbox(true),
	m_bIsTakeAll(false),
	m_nInboxPageIndex(1),
	m_nSystemPageIndex(1),
	m_bIsActionFlag(true)
{	
	m_pTakeBtn = nullptr;
	m_pBattleLogResult = nullptr;
	
	m_eUIType = UI_MAIL;
	
	m_nPageIndex = PAGE_SYSTEM_INDEX;
	m_nAgainstPirateRewardCoins = 0;
	m_nAgainstPirateRewardGolds = 0;
	m_nAgainstPirateRewardSailorCoins = 0;

	m_vIntbox.clear();
	m_vSystem.clear();
	m_vTakeAllMailId.clear();

	m_padString = "";
	m_bIsVillage = false;
	m_bIsEnemyCity = false;
}

UIEmail::~UIEmail()
{
	if (m_pMailListResult)
	{
		get_mail_list_result__free_unpacked(m_pMailListResult,0);
	}
	if (m_pItemResult)
	{
		get_personal_item_result__free_unpacked(m_pItemResult,0);
	}
	m_pEmailLayer = nullptr;
	m_pTempButton = nullptr;
	m_pTakeBtn = nullptr;

	m_vIntbox.clear();
	m_vSystem.clear();
}

void UIEmail::onEnter()
{
	UIBasicLayer::onEnter();
}

void UIEmail::onExit()
{
	UIBasicLayer::onExit();
}

void UIEmail::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	//玩家邮箱
	if (isButton(button_inbox)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nPageIndex = PAGE_INBOX_INDEX;
		openInboxView();
		return;
	}
	//系统邮箱
	if (isButton(button_system)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nPageIndex = PAGE_SYSTEM_INDEX;
		openSystemView();
		return;
	}
	//战斗日志
	if (isButton(button_battlelog)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nPageIndex = PAGE_BATTLELOG_INDEX;
		ProtocolThread::GetInstance()->getLootPlayerLog(0,1, UILoadingIndicator::createWithMask(this, _cameraMask));
		return;
	}
	//复仇
	if (isButton(button_revenge))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->startRevengeOnLootPlayer(button->getTag(), UILoadingIndicator::createWithMask(this, _cameraMask));
		return;
	}
	//查看邮件里船只详情
	if (isButton(button_ship_equipment))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->getPersonalItems(ITEM_TYPE_SHIP_EQUIP, 1, UILoadingIndicator::createWithMask(this, _cameraMask));
		changeMinorButtonState(button);
		return;
	}
	//查看邮件里装备详情
	if (isButton(button_human_equipment))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->getPersonalItems(ITEM_TYPE_ROLE_EQUIP, 1, UILoadingIndicator::createWithMask(this, _cameraMask));
		changeMinorButtonState(button);
		return;
	}
	//查看邮件里图纸详情
	if (isButton(button_drawings))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->getPersonalItems(ITEM_TYPE_DRAWING, 1, UILoadingIndicator::createWithMask(this, _cameraMask));
		changeMinorButtonState(button);
		return;
	} 
	//查看邮件里的道具详情
	if (isButton(button_miscs))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->getPersonalItems(ITEM_TYPE_PORP, 1, UILoadingIndicator::createWithMask(this, _cameraMask));
		changeMinorButtonState(button);
		return;
	}
	//查看邮件物品详情
	if (isButton(button_items) || isButton(b_item_0) || isButton(b_item_1) || isButton(b_item_2) || 
		isButton(image_items_bg_1)||isButton(image_items_bg_2)||isButton(image_items_bg_3))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ImageView*i_goods_bg = nullptr;
		auto i_goods_bg_1 = button->getChildByName<ImageView*>("image_goods_bg");
		auto image_item = button->getChildByName<ImageView*>("image_item");
		if (i_goods_bg_1)
		{
			i_goods_bg = i_goods_bg_1;
		}
		else if (image_item)
		{
			i_goods_bg = image_item;
		}
		if (i_goods_bg)
		{
			openGoodInfo(nullptr, i_goods_bg->getTag(), button->getTag());
		}
				return;
	}
	//返回主城
	if(isButton(button_back)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);	
		if (this->getChildByTag(INFORMVIEWTAG))
		{
			auto view = this->getChildByTag(INFORMVIEWTAG);
			dynamic_cast<UIInform*>(view)->closeAutoInform(0);
		}
		closeView();
		auto currentScene = Director::getInstance()->getRunningScene();
		auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
		auto mapLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
		if (mainlayer)
		{
			mainlayer->updateMailNumber(0);
		}
		else if (mapLayer)
		{
			auto mapUI = dynamic_cast<UISailHUD*>(mapLayer->getChildByName("MAPUI"));
			mapUI->updateMailNumber(0);
		}
		unregisterCallBack();
		int* unReadMail = new int[m_vSystem.size()];
		for (int i = 0; i < m_vSystem.size(); i++)
		{
			MailDefine *mailDefine = m_vSystem.at(i);
			unReadMail[i] = m_vSystem.at(i)->mailid;
		}

		ProtocolThread::GetInstance()->markMailAsRead(unReadMail, m_vSystem.size());
		delete[]unReadMail;
		return;
	}

	//关闭提示框界面
	if (isButton(button_salvage_no))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	//删除好友
	if (isButton(button_friend_delete)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_eConfirmIndex = CONFIRM_INDEX_DELETE_FRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_DELETE_FRIEND_TITLE","TIP_SOCIAL_DELETE_FRIEND");
		return;
	}
	//好友拉黑
	if (isButton(button_friend_block)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_eConfirmIndex = CONFIRM_INDEX_BLOCK_FRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE","TIP_SOCIAL_BLOCK_FRIEND");
		return;
	}
	//陌生人拉黑
	if (isButton(button_stranger_block)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_eConfirmIndex = CONFIRM_INDEX_BLOCK_STRANGER;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE","TIP_SOCIAL_BLOCK_STRANGER");
		return;
	}
	//加好友
	if (isButton(button_add_friend)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_eConfirmIndex = CONFIRM_INDEX_ADD_FRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_ADD_FRIEND_TITLE","TIP_SOCIAL_ADD_FRIEND");
		return;
	}
	//邀请
	if (isButton(button_invite)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pTempButton = button;
		m_eConfirmIndex = CONFIRM_INDEX_EMAIL_INVITE_GUILD;
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
	//确认对话
	if (isButton(button_confirm_yes))  
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		switch (m_eConfirmIndex)
		{
		case UIEmail::CONFIRM_INDEX_DELETE_INBOX_EMAIL:
			//删除收信界面邮件
			{
				MailDefine* mailDefine = m_vIntbox[m_nEmailIndex];
				ProtocolThread::GetInstance()->deleteMail(&mailDefine->mailid, 1, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
			break;
		case UIEmail::CONFIRM_INDEX_DELETE_SYSTEM_EMAIL:
			//删除系统邮件
			{
				MailDefine* mailDefine = m_vSystem[m_nEmailIndex];
				ProtocolThread::GetInstance()->deleteMail(&mailDefine->mailid, 1, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
			break;
		case UIEmail::CONFIRM_INDEX_DELETE_FRIEND:
			//删除好友邮件
			{
				ProtocolThread::GetInstance()->friendsOperation(m_vIntbox.at(m_nEmailIndex)->peerid, 1, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
			break;
		case UIEmail::CONFIRM_INDEX_ADD_FRIEND:
			//添加好友
			{
				ProtocolThread::GetInstance()->friendsOperation(m_vIntbox.at(m_nEmailIndex)->peerid, 0, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
			break;
		case UIEmail::CONFIRM_INDEX_BLOCK_FRIEND:
			//拉黑好友
			{
				ProtocolThread::GetInstance()->friendsOperation(m_vIntbox.at(m_nEmailIndex)->peerid, 2, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
			break;
		case UIEmail::CONFIRM_INDEX_BLOCK_STRANGER:
			//拉黑陌生人
			{
				ProtocolThread::GetInstance()->friendsOperation(m_vIntbox.at(m_nEmailIndex)->peerid, 2, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
			break;
		case UIEmail::CONFIRM_INDEX_EMAIL_INVITE_GUILD:
			//邀请好友
			{
				ProtocolThread::GetInstance()->inviteUserToGuild(m_pTempButton->getTag(), UILoadingIndicator::createWithMask(this, _cameraMask));
			}
			break;
		case UIEmail::CONFIRM_INDEX_DELETE_ALL_INBOX_EMAIL:
			//删除全部收信界面邮件
			{
				int *mailId = new int[m_vIntbox.size()];
				for (int i = 0; i <m_vIntbox.size(); i++)
				{
					mailId[i] = m_vIntbox.at(i)->mailid;
				}
				
				ProtocolThread::GetInstance()->deleteMail(mailId, m_vIntbox.size(), UILoadingIndicator::createWithMask(this, _cameraMask));
				delete []mailId;
			}
			break;
		case UIEmail::CONFIRM_INDEX_DELETE_ALL_SYSTEM_EMAIL:
			//删除全部系统邮件
			{
				int *mailId = new int[m_vSystem.size()];
				for (int i = 0; i <m_vSystem.size(); i++)
				{
					mailId[i] = m_vSystem.at(i)->mailid;
				}

				ProtocolThread::GetInstance()->deleteMail(mailId, m_vSystem.size(), UILoadingIndicator::createWithMask(this, _cameraMask));
				delete[]mailId;
			}
			break;
		default:
			break;
		}
		return;
	}
	//关闭提示框
	if (isButton(button_last_widget))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	//发送邮件失败提示框
	if (isButton(button_error_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		return;
	}
	//邮箱问号
	if (isButton(button_mail_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_MAIL_INFO_TITLE", "TIP_MAIL_INFO_CONTENT");
		return;
	}
	//一键删除所有邮件
	if (isButton(button_deleteall))
	{
		auto unTakenBoo = false;
		std::string titleStr;
		switch (m_nPageIndex)
		{
		case PAGE_INBOX_INDEX:
			m_eConfirmIndex = CONFIRM_INDEX_DELETE_ALL_INBOX_EMAIL;
			for (int i = 0; i <m_vIntbox.size(); i++)
			{
				if (m_vIntbox.at(i)->cantakeattachment == 1)
				{
					unTakenBoo = true;
				}
			}
			titleStr = "TIP_EMAIL_DELETE_ALL_INBOX_EMAIL_TITLE";
			break;
		case PAGE_SYSTEM_INDEX:
			m_eConfirmIndex = CONFIRM_INDEX_DELETE_ALL_SYSTEM_EMAIL;
			for (int i = 0; i < m_vSystem.size(); i++)
			{
				if (m_vSystem.at(i)->cantakeattachment == 1)
				{
					unTakenBoo = true;
				}
			}
			titleStr = "TIP_EMAIL_DELETE_ALL_SYSTEM_EMAIL_TITLE";
			break;
		default:
			break;
		}
		if (unTakenBoo)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO(titleStr, "TIP_MAIL_DELETEALL_HAVE_SOMETHING_UNTAKEN");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO(titleStr, "TIP_MAIL_DELETEALL_CONFIRM");
		}
		return;
	}
	//一件领取所有附件
	if (isButton(button_takeall))
	{
		m_bIsTakeAll = true;
		int *takeAllMailId;
		int num = 0;
		m_vTakeAllMailId.clear();
		auto currentScene = Director::getInstance()->getRunningScene();
		auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
		auto mapLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
		auto mapSailingLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_SAILING_TAG + 100));
		
		switch (m_nPageIndex)
		{
		case PAGE_INBOX_INDEX:
			takeAllMailId = new int[m_vIntbox.size()];
			for (int i = 0; i <m_vIntbox.size(); i++)
			{
				if (m_vIntbox.at(i)->cantakeattachment == 1)
				{
					m_vTakeAllMailId.push_back(m_vIntbox.at(i));
					takeAllMailId[num] = m_vIntbox.at(i)->mailid;
					num++;
				}
			}
			ProtocolThread::GetInstance()->markMailStatus(takeAllMailId, m_vTakeAllMailId.size(), 5, UILoadingIndicator::createWithMask(this, _cameraMask));
			delete[]takeAllMailId;
			break;
		case PAGE_SYSTEM_INDEX:
			takeAllMailId = new int[m_vSystem.size()];
			m_bIsVillage = false;
			m_bIsEnemyCity = false;
			for (int i = 0; i <m_vSystem.size(); i++)
			{
				if (m_vSystem.at(i)->cantakeattachment == 1)
				{
					if (mainlayer)
					{
						if (m_vSystem.at(i)->attachtype == ITEM_TYPE_SHIP && (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5 || mainlayer->getIsEnemyCity()))
						{
							if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
							{
								m_bIsVillage = true;
							}
							else if (mainlayer->getIsEnemyCity())
							{
								m_bIsEnemyCity = true;
							}
						}
						else
						{
							m_vTakeAllMailId.push_back(m_vSystem.at(i));
							takeAllMailId[num] = m_vSystem.at(i)->mailid;
							num++;
						}
					}
					else if (mapLayer || mapSailingLayer)
					{
						if (m_vSystem.at(i)->attachtype == ITEM_TYPE_SHIP)
						{

						}
						else
						{
							m_vTakeAllMailId.push_back(m_vSystem.at(i));
							takeAllMailId[num] = m_vSystem.at(i)->mailid;
							num++;
						}
					}
				}
			}
			if (m_vTakeAllMailId.size())
			{
				ProtocolThread::GetInstance()->markMailStatus(takeAllMailId, m_vTakeAllMailId.size(), 5, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				if (m_bIsVillage)
				{
					UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_FAILED_WITHOUT_SHIP_AT_VILLAGE");
				}
				else if (m_bIsEnemyCity)
				{
					UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_FAILED_WITHOUT_SHIP_AT_ENEMY_CITY");
				}
				else if (mapLayer || mapSailingLayer)
				{
					UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_FAILED_WITHOUT_SHIP_AT_SEA");
				}
			}
			delete[]takeAllMailId;
			break;
		default:
			break;
		}

		return;
	}
}

void UIEmail::inboxButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	//玩家取回
	if (isButton(button_takeall)) 
	{
		button->setTouchEnabled(false);
		m_pTakeBtn = button;
		m_bIsIntbox = true;
		MailDefine* mailDefine = m_vIntbox.at(m_nEmailIndex);
		if (mailDefine->coinnum > 0)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		}
		ProtocolThread::GetInstance()->markMailStatus(&mailDefine->mailid, 1, 5, UILoadingIndicator::createWithMask(this, _cameraMask));
		return;
	}
	else
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//删除玩家邮件
		if (isButton(button_delete)) 
		{
			if (button->getTag() < 100000)
			{
				m_nEmailIndex = button->getTag();
			}
			m_eConfirmIndex = CONFIRM_INDEX_DELETE_INBOX_EMAIL;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_EMAIL_DELETE_EMAIL_TITLE","TIP_EMAIL_DELETE_EMAIL");
			return;
		}
		//写邮件
		if (isButton(button_write)) 
		{
			openComposeView();
			return;
		}
		//恢复邮件
		if (isButton(button_reply))
		{
			sendEmailToPlayer(m_vIntbox.at(m_nEmailIndex)->peername, m_vIntbox.at(m_nEmailIndex)->peername);
			return;
		}
		//个人信息
		if (isButton(button_info))  
		{
			ProtocolThread::GetInstance()->getUserInfoById(button->getTag(), UILoadingIndicator::createWithMask(this, _cameraMask));
			return;
		}
		//发送邮件输入数字
		if (isButton(label_silver_num)) 
		{
			if (m_bIsActionFlag)
			{
				showNumpad();
			}
			return;
		}
		//翻页效果
		if (isButton(button_pageup))
		{
			m_nInboxPageIndex--;
			openInboxView();
			return;
		}
		//翻页效果
		if (isButton(button_pagedown))
		{
			m_nInboxPageIndex++;
			openInboxView();
			return;
		}
		//退出数字键盘
		if (isButton(button_numpad_yes) || isButton(panel_numpad_mark)) 
		{
			if (m_bIsActionFlag)
			{
				m_bIsActionFlag = false;
				auto view = getViewRoot(MAIL_COCOS_RES[MAIL_WRITE_CSB]);
				auto p_numpad_mark = view->getChildByName<Widget*>("panel_numpad_mark");
				p_numpad_mark->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4f), Place::create(ENDPOS)));
				auto num_root = view->getChildByName<Widget*>("panel_numpad");
				num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, ENDPOS4),
					CallFunc::create(this, callfunc_selector(UIEmail::moveEnd))));
				auto t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_silver_num"));
				std::string str = t_silver->getString();
				if (str.empty())
				{
					t_silver->setString("0");
				}
			}
			return;
		}
		//关闭
		if (isButton(button_close))
		{
			closeView();
			MailDefine* mailDefine = m_vIntbox.at(m_nEmailIndex);
			if (mailDefine->status == 0)
			{
				ProtocolThread::GetInstance()->markMailAsRead(&mailDefine->mailid, 1);
			}
			auto l_mail = m_pRoot->getChildByName<ListView*>("listview_mail");
			TableView* mTableView = dynamic_cast<TableView*>(l_mail->getParent()->getChildByTag(1000));
			mTableView->updateCellAtIndex(m_nEmailIndex);
			return;
		}
	}
}
//系统邮箱的button相应 
void UIEmail::systemButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	//取回
	if (isButton(button_take))  
	{
		button->setTouchEnabled(false);
		m_nEmailIndex = button->getTag();
		m_pTakeBtn = button;
		m_bIsIntbox = false;
		MailDefine* mailDefine = m_vSystem.at(m_nEmailIndex);
		if (mailDefine->coinnum>0)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		}
		
		auto currentScene = Director::getInstance()->getRunningScene();
		auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
		auto mapLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
		auto mapSailingLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_SAILING_TAG + 100));
		if (mainlayer)
		{
			if (mailDefine->attachtype == ITEM_TYPE_SHIP)
			{
				if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKE_SHIP_FAILED_AT_VILLAGE");
					button->setTouchEnabled(true);
				}
				else if (mainlayer->getIsEnemyCity())
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKE_SHIP_FAILED_AT_ENEMY_CITY");
					button->setTouchEnabled(true);
				}
				else
				{
					ProtocolThread::GetInstance()->markMailStatus(&mailDefine->mailid, 1, 5, UILoadingIndicator::createWithMask(this, _cameraMask));
				}
			}
			else
			{
				ProtocolThread::GetInstance()->markMailStatus(&mailDefine->mailid, 1, 5, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
		}
		else if (mapLayer || mapSailingLayer)
		{
			if (mailDefine->attachtype == ITEM_TYPE_SHIP)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKE_SHIP_FAILED_AT_SEA");
				button->setTouchEnabled(true);
			}
			else
			{
				ProtocolThread::GetInstance()->markMailStatus(&mailDefine->mailid, 1, 5, UILoadingIndicator::createWithMask(this, _cameraMask));
			}
		}
		
		return;
	}
	else
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (isButton(button_getgift_reply) || isButton(label_reply))//回赠礼物
		{
			auto indexGift = button->getTag();
			MailDefine* mailDefine = m_vSystem.at(indexGift);
			auto content = mailDefine->message;
			rapidjson::Document root;
			if (root.Parse<0>(mailDefine->message).HasParseError()){
				return;
			}
			if (root.IsNull())
			{
				return;
			}
			if (strcmp(mailDefine->title, "12") == 0)
			{
				auto name = DictionaryHelper::getInstance()->getStringValue_json(root, "sender_name");
				auto sender_cid = DictionaryHelper::getInstance()->getIntValue_json(root, "sender_cid");
				ProtocolThread::GetInstance()->sendActivitiesGift(sender_cid, UILoadingIndicator::create(this));
			}
			return;
		}
		//删除系统邮件
		if (isButton(button_delete)) 
		{
			if (button->getTag() < 100000)
			{
				m_nEmailIndex = button->getTag();
			}
			m_eConfirmIndex = CONFIRM_INDEX_DELETE_SYSTEM_EMAIL;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_EMAIL_DELETE_EMAIL_TITLE","TIP_EMAIL_DELETE_EMAIL");
			return;
		}
	}
}
//发送邮件的button相应 
void UIEmail::composeButtonEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//发送时添加物品
	if (isButton(button_items)) 
	{
		ProtocolThread::GetInstance()->getPersonalItems(ITEM_TYPE_SHIP_EQUIP, 1, UILoadingIndicator::createWithMask(this,_cameraMask));
		return;
	}
	//选择好友
	if (isButton(button_choose))  
	{
		ProtocolThread::GetInstance()->getFriendsList(UILoadingIndicator::createWithMask(this,_cameraMask));
		return;
	}
	//重新编辑
	if (isButton(button_rewrite)) 
	{
		openComposeView();
		m_padString.clear();
		return;
	}
	//发送
	if (isButton(button_send)) 
	{
		sendMail();		
		return;
	}
	//发送邮件附带物品
	if (isButton(panel_itemdetails))
	{
		openCompostAmountView(button->getTag());
		return;
	}
	//确认 选择物品
	if (isButton(button_yes)) 
	{
		UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
		m_nItemType = m_pItemResult->itemtype;
		m_nItemNum = ss->getCurrentNumber();
		m_nItemId = button->getTag();
		std::string name;
		std::string path;
		getItemNameAndPath(m_nItemType,m_nItemIid,name,path);
		auto view = getViewRoot(MAIL_COCOS_RES[MAIL_WRITE_CSB]);
		auto i_bgImage = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_goods_bg"));
		auto i_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_item_bg_lv"));
		auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_goods"));
		auto t_num = i_icon->getChildByName<Text*>("text_item_num");
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(path);
		t_num->setVisible(true);
		setTextSizeAndOutline(t_num, m_nItemNum);
		setBgButtonFormIdAndType(i_bgImage, m_nItemIid, m_nItemType);
		setBgImageColorFormIdAndType(i_bg_lv, m_nItemIid, m_nItemType);

		auto i_intensify = Helper::seekWidgetByName(i_bgImage, "goods_intensify");
		if (i_intensify)
		{
			i_intensify->removeFromParentAndCleanup(true);
		}
		for (size_t i = 0; i < m_pItemResult->n_equipments; i++)
		{
			if (m_nItemId == m_pItemResult->equipments[i]->uniqueid)
			{
				if (m_pItemResult->equipments[i]->optionalid > 0)
				{
					addStrengtheningIcon(i_bgImage);
				}
				break;
			}	
		}
		
		closeView();
		closeView();
		return;
	}
	//选择邮寄物品界面关闭按钮
	if (isButton(button_no))
	{
		closeView();
		return;
	}
	//选择某好友
	if (isButton(panel_friend))  
	{
		closeView();
		std::string name = button->getChildByName<Text*>("label_player_name")->getString();
		TextField* t_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(getViewRoot(MAIL_COCOS_RES[MAIL_WRITE_CSB]),"textfield_player_name"));
		t_name->setVisible(false);
		ui::EditBox* edit_name = dynamic_cast<ui::EditBox*>(t_name->getParent()->getChildByTag(1));
		edit_name->setText(name.c_str());
		return;
	}
	if (isButton(button_friend_choose_close))
	{
		closeView();
		return;
	}
	//关闭写邮件界面
	if (isButton(button_compose_close)) 
	{
		closeView();
		//chengyuan++
		unscheduleUpdate();
		m_padString.clear();
		//
		auto view = getViewRoot(MAIL_COCOS_RES[MAIL_CSB]);
		if (!view)
		{
			unregisterCallBack();
		}
		return;
	}
	//关闭
	if (isButton(button_close))
	{
		closeView();
		if (m_nPageIndex == PAGE_INBOX_INDEX)
		{
			auto l_mail = m_pRoot->getChildByName<ListView*>("listview_mail");
			TableView* mTableView = dynamic_cast<TableView*>(l_mail->getParent()->getChildByTag(1000));
			mTableView->updateCellAtIndex(m_nEmailIndex);
		}
		return;
	}
}
//数字键盘操作的button相应
void UIEmail::numPadButtonEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto target = dynamic_cast<Widget*>(pSender);
		std::string name = target->getName();
		auto view = getViewRoot(MAIL_COCOS_RES[MAIL_WRITE_CSB]);
		auto t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_silver_num"));
		//数字键盘删除操作
		if (isButton(button_del))
		{
			if (!m_padString.empty())
			{
				m_padString.erase(m_padString.end() - 1);
			}
		}
		else
		{
			int tag = target->getTag();
	
			if (m_padString.size() < 2 && m_padString == "0")
			{
				m_padString.erase(m_padString.end() - 1);
			}
			m_padString += StringUtils::format("%d", tag);
		}
		t_silver->setString(numSegment(m_padString));
	}
}

//1 mark as read, 2 move to dustbin , 3 delete permanently ,4 restore,5 fetch attachment
void UIEmail::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetMailListResult:
		{
			GetMailListResult *mailList = (GetMailListResult*)message;
			if (!mailList->failed)
			{
				m_pMailListResult = mailList;
				m_vIntbox.clear();
				m_vSystem.clear();
				for (int i = 0; i < m_pMailListResult->n_inbox; i++)
				{
					m_vIntbox.push_back(m_pMailListResult->inbox[i]);
				}
				for (int i = 0; i < m_pMailListResult->n_system; i++)
				{
					m_vSystem.push_back(m_pMailListResult->system[i]);
				}
				m_nPageIndex = PAGE_SYSTEM_INDEX;
				openSystemView();
			}
			break;
		}
	case PROTO_TYPE_MarkMailStatusResult:
		{
			MarkMailStatusResult *result = (MarkMailStatusResult*)message;
			if (result->failed == 0)
			{
				switch (result->action)
				{
				case 1:
				{
						  // mark as read is success
						  ProtocolThread::GetInstance()->checkMailBox();
						  break;
				}
				case 2:
				{
						  //Move To Dustbin success
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_MOVE_EMAIL_SUCCESS");
						  break;
				}
				case 3:
				{
						  //delete success
						  switch (m_eConfirmIndex)
						  {
						  case UIEmail::CONFIRM_INDEX_DELETE_INBOX_EMAIL:
						  {
							  m_vIntbox.erase(m_vIntbox.begin() + m_nEmailIndex);
							  openInboxView();
						     auto l_mail = m_pRoot->getChildByName<ListView*>("listview_mail");
							 TableView* mTableView = dynamic_cast<TableView*>(l_mail->getParent()->getChildByTag(1000));
							 //mTableView->removeCellAtIndex(m_nEmailIndex);
							 mTableView->reloadData();
							   break;
						  }
						  case UIEmail::CONFIRM_INDEX_DELETE_SYSTEM_EMAIL:
						  {
							  m_vSystem.erase(m_vSystem.begin() + m_nEmailIndex);
                              openSystemView();
                            //改动
							auto l_mail = m_pRoot->getChildByName<ListView*>("listview_mail");
							TableView* mTableView = dynamic_cast<TableView*>(l_mail->getParent()->getChildByTag(999));
							//当前cell的高和宽
							Vec2 ss = tableCellSizeForIndex(mTableView, 0);
							if (m_vSystem.size() > 2)
							{
								if (m_nEmailIndex == m_vSystem.size()|| m_nEmailIndex == m_vSystem.size()-1)
								{
									mTableView->setContentOffset(Vec2(m_nslider.x, 0), false);
								}
								else if (m_nEmailIndex == 0||m_nEmailIndex == 1)
								{
									mTableView->setContentOffset(Vec2(m_nslider.x, 0.00 -(( m_vSystem.size()-2)*ss.y)-24), false);
								}
								else
								{
									mTableView->setContentOffset(Vec2(m_nslider.x, 0.00 - (m_vSystem.size() - m_nEmailIndex )*ss.y), false);
								}
							}
							else if (m_vSystem.size() == 2)
							{
								mTableView->setContentOffset(Vec2(m_nslider.x, 0.00 - ((m_vSystem.size() - 2)*ss.y) - 24), false);
							}
							  break;
					       }
						  case UIEmail::CONFIRM_INDEX_DELETE_ALL_INBOX_EMAIL:
							  m_vIntbox.clear();
							  openInboxView();
							  break;
						  case UIEmail::CONFIRM_INDEX_DELETE_ALL_SYSTEM_EMAIL:
							  m_vSystem.clear();
							  openSystemView();
							  break;
						  default:
							  break;
						  }

						  closeView(MAIL_COCOS_RES[MAIL_VIEW_PLAYER_CSB]);
						  closeView(MAIL_COCOS_RES[MAIL_VIEW_SYSTEM_CSB]);
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_DELETE_EMAIL_SUCCESS");
						  break;
				}
				case 5:
				{
						  //take all success
						  if (m_bIsTakeAll)
						  {
							  m_bIsTakeAll = false;
							  for (int i = 0; i < m_vTakeAllMailId.size();i++)
							  {
								  m_vTakeAllMailId[i]->cantakeattachment = 0;
								  m_vTakeAllMailId[i]->status = 1;
								  if (m_vTakeAllMailId[i]->status == 0)
								  {
									  ProtocolThread::GetInstance()->markMailAsRead(&m_vTakeAllMailId[i]->mailid, 1);
								  }

							  }
							  if (m_nPageIndex == PAGE_INBOX_INDEX)
							  {
								  auto l_mail = m_pRoot->getChildByName<ListView*>("listview_mail");
								  if(l_mail){
									  TableView* mTableView = dynamic_cast<TableView*>(l_mail->getParent()->getChildByTag(1000));
									  mTableView->reloadData();
								  }
								  flushLeftButtonNum(PAGE_INBOX_INDEX);
							  }
							  else if (m_nPageIndex == PAGE_SYSTEM_INDEX)
							  {
								  auto l_notification = m_pRoot->getChildByName<ListView*>("listview_notification");
								  TableView* mTableView = dynamic_cast<TableView*>(l_notification->getParent()->getChildByTag(999));
								  mTableView->updateCellAtIndex(m_nEmailIndex);
								  mTableView->reloadData();
								  flushLeftButtonNum(PAGE_SYSTEM_INDEX);
							  }
							  m_vTakeAllMailId.clear();
							  SINGLE_HERO->m_iCoin = result->usercoins;
							  SINGLE_HERO->m_iGold = result->usergolds;
							  SINGLE_HERO->m_iLevel = EXP_NUM_TO_LEVEL(result->exp);
							  SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->fame);
							  auto currentScene = Director::getInstance()->getRunningScene();
							  auto mainlayer = (UIMain*)(currentScene->getChildByTag(100 + MAIN_TAG));
							  auto mapLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
							  auto mapSailingLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_SAILING_TAG + 100));
							  if (mainlayer)
							  {
								  mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
								  mainlayer->flushExpAndRepLv();
							  }
							  UIInform::getInstance()->openInformView(this);
							  if (m_bIsVillage)
							  {
								  m_bIsVillage = false;
								  UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_SUCCESS_WITHOUT_SHIP_AT_VILLAGE");
							  }
							  else if (m_bIsEnemyCity)
							  {
								  m_bIsEnemyCity = false;
								  UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_SUCCESS_WITHOUT_SHIP_AT_ENEMY_CITY");
							  }
							  else if (mapLayer || mapSailingLayer)
							  {
								  UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_SUCCESS_WITHOUT_SHIP_AT_SEA");
							  }
							  else
							  {
								  UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_SUCCESS");
							  }
						  }
						  else
						  {
							  if (m_bIsIntbox)
							  {
								  UIInform::getInstance()->openInformView(this);
								  UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_SUCCESS");
								  SINGLE_HERO->m_iCoin = result->usercoins;
								  SINGLE_HERO->m_iGold = result->usergolds;
								  SINGLE_HERO->m_iLevel = EXP_NUM_TO_LEVEL(result->exp);
								  SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->fame);
								  auto currentScene = Director::getInstance()->getRunningScene();
								  auto mainlayer = (UIMain*)(currentScene->getChildByTag(100 + MAIN_TAG));
								  if (mainlayer)
								  {
									  mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
									  mainlayer->flushExpAndRepLv();
								  }
								  MailDefine* mailDefine = m_vIntbox.at(m_nEmailIndex);
								  mailDefine->cantakeattachment = 0;
								  openInboxReadView();
							  }
							  else
							  {
								  MailDefine* mailDefine = m_vSystem.at(m_nEmailIndex);
								  if (mailDefine->status == 0)
								  {
									  ProtocolThread::GetInstance()->markMailAsRead(&mailDefine->mailid, 1);
								  }
								  mailDefine->cantakeattachment = 0;
								  mailDefine->status = 1;
								  auto l_notification = m_pRoot->getChildByName<ListView*>("listview_notification");
								  TableView* mTableView = dynamic_cast<TableView*>(l_notification->getParent()->getChildByTag(999));
								  mTableView->updateCellAtIndex(m_nEmailIndex);
								  flushLeftButtonNum(PAGE_SYSTEM_INDEX);

								  if (strcmp(mailDefine->title, "12") == 0 || strcmp(mailDefine->title, "13") == 0 || strcmp(mailDefine->title, "14") == 0)
								  {
									  openView(COMMOM_COCOS_RES[C_VIEW_SALVAGE_RESULT]);
									  auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SALVAGE_RESULT]);
									  auto t_title = view->getChildByName<Text*>("label_result");
									  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EVNETS_FAITH_RESULT"]);
									  int n = 1;
									  //礼物中物品和银币只可能有一个
									  if (strcmp(mailDefine->title, "12") == 0)
									  {
										  if (result->item || result->giftaddcoins > 0)
										  {
											  n++;
										  }
									  }
									  //收礼物反馈有可能有银币
									  if (strcmp(mailDefine->title, "13") == 0)
									  {
										  if (result->giftaddcoins > 0)
										  {
											  n++;
										  }
									  }
									  //攻打海盗基地给予的奖励 一定有三种水手币、金币、银币
									  if (strcmp(mailDefine->title, "14") == 0)
									  {
										  n = 3;
									  }
									  for (size_t i = 0; i < 3; i++)
									  {
										  auto item = view->getChildByName<ImageView*>(StringUtils::format("image_items_bg_%d", i + 1));
										  item->setTouchEnabled(true);
										  item->addTouchEventListener(CC_CALLBACK_2(UIEmail::menuCall_func,this));
										  if (i < n)
										  {
											  auto i_icon = item->getChildByName<ImageView*>("image_item");
											  auto t_num = item->getChildByName<Text*>("label_item_num");
											  i_icon->ignoreContentAdaptWithSize(false);
											  if (i == 0)
											  {
												  item->setTag(10001);
												  i_icon->setTag(ITEM_TYPE_SPECIAL);
												  i_icon->loadTexture(getItemIconPath(10001));
												  if (strcmp(mailDefine->title, "14") == 0)
												  {
													  setTextSizeAndOutline(t_num, m_nAgainstPirateRewardSailorCoins);
												  }
												  else
												  {
													  setTextSizeAndOutline(t_num, result->addsailorcoins);
												  }
											  }
											  else
											  {
												  if (strcmp(mailDefine->title, "14") == 0)
												  {
													  if (i == 1)
													  {
														  item->setTag(10000);
														  i_icon->setTag(ITEM_TYPE_SPECIAL);
														  i_icon->loadTexture(getVticketOrCoinPath(10000, 1));
														  setTextSizeAndOutline(t_num, m_nAgainstPirateRewardGolds);
													  }
													  else
													  {
														  item->setTag(10002);
														  i_icon->setTag(ITEM_TYPE_SPECIAL);
														  i_icon->loadTexture(getVticketOrCoinPath(10001, 1));
														  setTextSizeAndOutline(t_num, m_nAgainstPirateRewardCoins);
													  }
												  }
												  else
												  {
													  if (result->giftaddcoins > 0)
													  {
														  item->setTag(10002);
														  i_icon->setTag(ITEM_TYPE_SPECIAL);
														  i_icon->loadTexture(getVticketOrCoinPath(10001, 1));
														  setTextSizeAndOutline(t_num, result->giftaddcoins);
													  }
													  else
													  {
														  item->setTag(result->item->itemid);
														  auto shopData = SINGLE_SHOP->getItemData();
														  std::map<int, ITEM_RES>::iterator m1_Iter;
														  for (m1_Iter = shopData.begin(); m1_Iter != shopData.end(); m1_Iter++)
														  {
															  if (m1_Iter->second.id == result->item->itemid)
															  {
																  i_icon->setTag(m1_Iter->second.type);								 
																  break;
															  }
														  }
														  i_icon->loadTexture(getItemIconPath(result->item->itemid));
														  setTextSizeAndOutline(t_num, result->item->itemcount);
													  }
												  }
											  }
											  item->setVisible(true);
										  }
										  else
										  {
											  item->setVisible(false);
										  }
										  //位置微调
										  if (n == 1)
										  {
											  if (i == 0)
											  {
												  item->setPositionX(item->getPositionX() + item->getBoundingBox().size.width*1.3);
											  }
										  }
										  else if (n == 2)
										  {
											  if (i == 0 || i == 1)
											  {
												  item->setPositionX(item->getPositionX() + item->getBoundingBox().size.width*0.8);
											  }
										  }
									  }
								  }
								  else
								  {
									  UIInform::getInstance()->openInformView(this);
									  UIInform::getInstance()->openViewAutoClose("TIP_EAMIL_TAKEALL_ITEM_SUCCESS");
								  }

								  SINGLE_HERO->m_iCoin = result->usercoins;
								  SINGLE_HERO->m_iGold = result->usergolds;
								  SINGLE_HERO->m_iLevel = EXP_NUM_TO_LEVEL(result->exp);
								  SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->fame);
								  auto currentScene = Director::getInstance()->getRunningScene();
								  auto mainlayer = (UIMain*)(currentScene->getChildByTag(100 + MAIN_TAG));
								  if (mainlayer)
								  {
									  mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
									  mainlayer->flushExpAndRepLv();
								  }
							  }
						  }
						  break;
				}
				default:
					break;
				}
			}
			else if (result->failed == 104)
			{
				if (m_bIsTakeAll)
				{
					m_bIsTakeAll = false;
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_MAIL_DELETEALL_EXCREATE_BAG_FULL");
					
					if (result->n_succrecvmailids)
					{
						for (int i = 0; i < result->n_succrecvmailids; i++)
						{
							if (m_nPageIndex == PAGE_INBOX_INDEX)
							{
								for (int j = 0; j < m_vIntbox.size(); j++)
								{
									if (m_vIntbox.at(j)->mailid == result->succrecvmailids[i])
									{
										m_vIntbox.at(j)->cantakeattachment = 0;
									}
								}
							}
							else if (m_nPageIndex == PAGE_SYSTEM_INDEX)
							{
								for (int j = 0; j < m_vSystem.size(); j++)
								{
									if (m_vSystem.at(j)->mailid == result->succrecvmailids[i])
									{
										m_vSystem.at(j)->cantakeattachment = 0;
									}
								}
							}
						}
					}
					for (int i = 0; i < m_vTakeAllMailId.size(); i++)
					{
						m_vTakeAllMailId[i]->status = 1;
						if (m_vTakeAllMailId[i]->status == 0)
						{
							ProtocolThread::GetInstance()->markMailAsRead(&m_vTakeAllMailId[i]->mailid, 1);
						}
					}
					if (m_nPageIndex == PAGE_INBOX_INDEX)
					{
						auto l_mail = m_pRoot->getChildByName<ListView*>("listview_mail");
						TableView* mTableView = dynamic_cast<TableView*>(l_mail->getParent()->getChildByTag(1000));
						mTableView->reloadData();
						flushLeftButtonNum(PAGE_INBOX_INDEX);
					}
					else if (m_nPageIndex == PAGE_SYSTEM_INDEX)
					{
						auto l_notification = m_pRoot->getChildByName<ListView*>("listview_notification");
						TableView* mTableView = dynamic_cast<TableView*>(l_notification->getParent()->getChildByTag(999));
						mTableView->reloadData();
						flushLeftButtonNum(PAGE_SYSTEM_INDEX);
					}
					m_vTakeAllMailId.clear();
					SINGLE_HERO->m_iCoin = result->usercoins;
					SINGLE_HERO->m_iGold = result->usergolds;
					auto currentScene = Director::getInstance()->getRunningScene();
					auto mainlayer = (UIMain*)(currentScene->getChildByTag(100 + MAIN_TAG));
					if (mainlayer)
					{
						mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
					}
				}
				else
				{
					if (m_pTakeBtn)
					{
						m_pTakeBtn->setTouchEnabled(true);
					}
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_BAG_FULL");
				}
			}
			else
			{
				if (m_bIsTakeAll)
				{
					m_bIsTakeAll = false;
				}
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	case PROTO_TYPE_SendMailResult:
		{
			SendMailResult* result = (SendMailResult*)message;
			switch (result->failed)
			{
			case 0:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openViewAutoClose("TIP_EMAIL_SENDTO_EMAIL_SUCCESS");
					  closeView();
					  m_padString.clear();
					  auto view = getViewRoot(MAIL_COCOS_RES[MAIL_CSB]);
					  if (!view)
					  {
						  unregisterCallBack();
					  }
					  SINGLE_HERO->m_iCoin = result->usercoins;
					  SINGLE_HERO->m_iGold = result->usergolds;
					  auto currentScene = Director::getInstance()->getRunningScene();
					  auto mainlayer = (UIMain*)(currentScene->getChildByTag(100 + MAIN_TAG));
					  if (mainlayer)
					  {
						  mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
					  }
					  break;
			}
			case 2:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_EMAIL_SENDTO_EMAIL_FAIL");
					  break;
			}
			case 6:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_EMAIL_SEND_EQUIPS_COMMINT_PREVENT");
					  break;
			}
			case 7:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_EMAIL_SEND_COIN_COMMINT_PREVENT");
					  break;
			}
			case COIN_NOT_FAIL:
			{
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
					  break;
			}
			default:
				break;
			}
			break;
		}
	case PROTO_TYPE_GetPersonalItemResult:
		{
			GetPersonalItemResult *result = (GetPersonalItemResult *)message;
			m_pItemResult = result;
			if (result->failed == 0)
			{
				m_pItemResult = result;
				openComposeEncloseView();
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_GET_ROLL_EQUIP_FAIL");
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
				UICommon::getInstance()->flushFriendDetail(result);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_PLAYER_INFO_FAIL");
			}
			break;
		}
	case PROTO_TYPE_FriendsOperationResult:
		{
			FriendsOperationResult *pFriendsResult = (FriendsOperationResult*)message;
			if (pFriendsResult->failed == 0)
			{
				switch (pFriendsResult->actioncode)
				{
				case 0:
				{
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_FRIEND_SUCCESS");
						  break;
				}
				case 1:
				case 2:
				{
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_BLACK_LIST_SUCCESS");
						  break;
				}
				default:
					break;
				}
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetFriendsListResult:
		{
			GetFriendsListResult *FriendsListResult = (GetFriendsListResult*)message;
			if (FriendsListResult->failed == 0)
			{
				openChooseFirend(FriendsListResult);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_FRIEND_LIST_FAIL");
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
			else if (result->failed ==11)
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
	case PROTO_TYPE_GetItemsDetailInfoResult:
		{
			GetItemsDetailInfoResult *result = (GetItemsDetailInfoResult*)message;
			if (result->failed == 0)
			{
				openGoodInfo(result, result->itemtype, result->itemid);
			}
			break;
		}
	case PROTO_TYPE_GetLootPlayerLogResult:
		{
			GetLootPlayerLogResult * result = (GetLootPlayerLogResult*)message;
			if (result->failed == 0)
			{
				m_pBattleLogResult = result;
				openBattleLogView();
			}
			break;
		}
	case PROTO_TYPE_FindLootPlayerResult:
		{
			FindLootPlayerResult * result = (FindLootPlayerResult *)message;
			switch (result->failed)
			{
			case 0:
			{
					  if (result->info->lootforbid)
					  {
						  //当前玩家不可以被复仇提示信息
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openConfirmYes("TIP_CANNOT_LOOT_PLAYER_INFO");
					  }
					  else
					  {
						  SINGLE_HERO->m_pBattelData = result->battledata;
						  SINGLE_HERO->shippos = Vec2(0, 0);
						  ProtocolThread::GetInstance()->forceFightStatus();
						  unregisterCallBack();
						  CHANGETO(SCENE_TAG::BATTLE_TAG);
						  break;
					  }
			}
			case 1:
			{
					  //当前被打劫的玩家没有船船只
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_LOOT_PLAYER_HAVE_NO_SHIP");
					  break;
			}
			case 2:
			{
					  //当前没有可打劫的玩家提示信息
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_LOOT_PLAYER_INFO");
					  break;
			}
			case 3:
			{
					  //当前玩家不可以被复仇提示信息
					  UIInform::getInstance()->openInformView(this);
					  UIInform::getInstance()->openConfirmYes("TIP_CANNOT_LOOT_PLAYER_INFO");
					  break;
			}
			default:
				break;
			}
			break;
		}
	case PROTO_TYPE_SendActivitiesGiftResult:
	{
			SendActivitiesGiftResult *result = (SendActivitiesGiftResult *)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_GIVE_GIFT_SUCCESS");
			//	ProtocolThread::GetInstance()->getFriendsList();
			}
			else if (result->failed == 101)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_GIVE_GIFT_FAIL");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PUP_YOUR_GIFT_HAVE_GIVEOUT");
			}
			break;
	}
	default:
		break;
	}
}
//左侧button上的邮箱里没有查看邮箱的个数
void UIEmail::flushLeftButtonNum(EMAIL_PAGE nFlag)
{
	auto p_root = getViewRoot(MAIL_COCOS_RES[MAIL_CSB]);
	auto b_inbox = dynamic_cast<Button*>(Helper::seekWidgetByName(p_root,"button_inbox"));
	auto b_system = dynamic_cast<Button*>(Helper::seekWidgetByName(p_root,"button_system"));
	auto b_battlelog = dynamic_cast<Button*>(Helper::seekWidgetByName(p_root, "button_battlelog"));
	auto b_takeall = dynamic_cast<Button*>(Helper::seekWidgetByName(p_root, "button_takeall"));
	auto b_deleteall = dynamic_cast<Button*>(Helper::seekWidgetByName(p_root, "button_deleteall"));
	auto b_write = dynamic_cast<Button*>(Helper::seekWidgetByName(p_root, "button_write"));
	auto t_inbox = b_inbox->getChildByName<Text*>("label_inbox");
	auto i_inbox_unRead = b_inbox->getChildByName<ImageView*>("image_unread");
	auto t_system = b_system->getChildByName<Text*>("label_system");
	auto i_system_unRead = b_system->getChildByName<ImageView*>("image_unread");
	auto t_battlelog = b_battlelog->getChildByName<Text*>("label_battlelog");

	b_deleteall->setVisible(true);
	b_takeall->setVisible(true);
	b_deleteall->setBright(true);
	b_takeall->setBright(true);
	b_deleteall->setTouchEnabled(true);
	b_takeall->setTouchEnabled(true);

	std::string inbox;
	int n = 0;
	for (int i = 0; i < m_vIntbox.size(); i++)
	{
		if (m_vIntbox.at(i)->status == 0)
		{
			n++;
		}
	}
	inbox += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_INBOX_BUTTON_TEXT"];
	if (n > 0)
	{
		i_inbox_unRead->setVisible(true);
		inbox += "(";
		inbox += StringUtils::format("%d",n);
		inbox += ")";
	}else
	{
		i_inbox_unRead->setVisible(false);
	}
	t_inbox->setString(inbox);

	std::string system;
	n = 0;
	for (int i = 0; i < m_vSystem.size(); i++)
	{
		if(m_vSystem.at(i)->status == 0)
		{
			n++;
		}
	}
	system += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_BUTTON_TEXT"];
	if (n > 0)
	{
		i_system_unRead->setVisible(true);
		system += "(";
		system += StringUtils::format("%d",n);
		system += ")";
	}else
	{
		i_system_unRead->setVisible(false);
	}
	t_system->setString(system);

	auto t_title = p_root->getChildByName<Text*>("label_title");
	std::vector<MailDefine*> mailBox;
	switch (nFlag)
	{
		case PAGE_INBOX_INDEX:
		{
				  b_inbox->setBright(false);
				  b_inbox->setTouchEnabled(false);
				  b_system->setBright(true);
				  b_system->setTouchEnabled(true);
				  b_battlelog->setBright(true);
				  b_battlelog->setTouchEnabled(true);
				  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_INBOX_TITLE"]);
				  t_inbox->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
				  t_system->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
				  t_battlelog->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
				  b_takeall->setPositionX(b_deleteall->getPositionX() + b_deleteall->getContentSize().width/2 + b_takeall->getContentSize().width/2 + 26);

				  mailBox = m_vIntbox;
				  break;
		}
		case PAGE_SYSTEM_INDEX:
		{
				  b_inbox->setBright(true);
				  b_inbox->setTouchEnabled(true);
				  b_system->setBright(false);
				  b_system->setTouchEnabled(false);
				  b_battlelog->setBright(true);
				  b_battlelog->setTouchEnabled(true);
				  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_TITLE"]);
				  t_inbox->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
				  t_system->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
				  t_battlelog->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
				  b_takeall->setPositionX(b_write->getPositionX());

				  mailBox = m_vSystem;
				  break;
		}
		default:
		{
				   b_inbox->setBright(true);
				   b_inbox->setTouchEnabled(true);
				   b_system->setBright(true);
				   b_system->setTouchEnabled(true);
				   b_battlelog->setBright(false);
				   b_battlelog->setTouchEnabled(false);
				   t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BATTLE_LOG_TITLE"]);
				   t_inbox->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
				   t_system->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
				   t_battlelog->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
				   b_deleteall->setVisible(false);
				   b_takeall->setVisible(false);

				   break;
		}
	}
	if (mailBox.size() <= 0)
	{
		b_deleteall->setBright(false);
		b_takeall->setBright(false);
		b_deleteall->setTouchEnabled(false);
		b_takeall->setTouchEnabled(false);
	}
	else
	{
		auto takeAllBoo = false;
		for (int i = 0; i <mailBox.size(); i++)
		{
			if (mailBox.at(i)->cantakeattachment == 1)
			{
				takeAllBoo = true;
			}
		}
		if (takeAllBoo)
		{
			takeAllBoo = false;
		}
		else
		{
			b_takeall->setBright(false);
			b_takeall->setTouchEnabled(false);
		}
	}
	mailBox.clear();
}
//收信箱界面   
void UIEmail::openInboxView()
{
	openView(MAIL_COCOS_RES[MAIL_CSB]);
	flushLeftButtonNum(PAGE_INBOX_INDEX);
	auto b_mail_info = m_pRoot->getChildByName<Button*>("button_mail_info");
	b_mail_info->setTouchEnabled(true);
	b_mail_info->setVisible(true);
	auto b_write = m_pRoot->getChildByName<Button*>("button_write");
	b_write->setVisible(true);
	b_write->setLocalZOrder(10000);
	b_write->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent, this));
	auto l_notification = m_pRoot->getChildByName<ListView*>("listview_notification");
	l_notification->setVisible(false);
	l_notification->removeAllChildrenWithCleanup(true);
	auto l_mail = m_pRoot->getChildByName<ListView*>("listview_mail");
	l_mail->setVisible(true);
	l_mail->removeAllChildrenWithCleanup(true);
	auto tableViewSystem = l_notification->getParent()->getChildByTag(999);
	if (tableViewSystem)
	{
		tableViewSystem->removeFromParentAndCleanup(true);
	}
	auto tableViewBattleLog = l_notification->getParent()->getChildByTag(998);
	if (tableViewBattleLog)
	{
		tableViewBattleLog->removeFromParentAndCleanup(true);
	}
	auto w_content = m_pRoot->getChildByName<Widget*>("panel_centent_no");
	w_content->setVisible(false);
	auto image_pulldown = m_pRoot->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	image_pulldown->setVisible(false);
	TableView* mTableView = dynamic_cast<TableView*>(l_mail->getParent()->getChildByTag(1000));
	if (m_vIntbox.empty())
	{
		w_content->getChildByName<Text*>("label_no_found_1_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_INBOXLIST_NO1"]);
		w_content->getChildByName<Text*>("label_no_found_2_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_INBOXLIST_NO2"]);
		w_content->setVisible(true);
		if (mTableView)
		{
			mTableView->reloadData();
		}
		return;
	}
	
	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(l_mail->getContentSize().width, l_mail->getContentSize().height));
		l_mail->getParent()->addChild(mTableView);
	}
	mTableView->setVisible(true);
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(l_mail->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(1000);
	mTableView->setTouchEnabled(true);
	mTableView->setPositionX(l_mail->getPositionX() - 18);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	this->setCameraMask(_cameraMask, true);
}

void UIEmail::scrollViewDidScroll(cocos2d::extension::ScrollView* view){
	auto pos = view->getContentOffset();
	auto viewSize = view->getViewSize();
	auto contentSize = view->getContainer()->getContentSize();
	
	float bottom = viewSize.height-contentSize.height;
	float top = 0;
	float totalLen = top - bottom;
	float currentLen = top - pos.y;
    m_nslider = pos;
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
		button_pulldown->setPositionY(len * factor + allowHeight*2 + allowHeight);
	}
}

void UIEmail::tableCellTouched(TableView* table, TableViewCell* cell)
{
	if (m_nPageIndex == PAGE_INBOX_INDEX)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nEmailIndex = cell->getTag();
		MailDefine* mailDefine = m_vIntbox.at(m_nEmailIndex);
		if (mailDefine->status == 0)
		{
			ProtocolThread::GetInstance()->markMailAsRead(&mailDefine->mailid, 1);
		}
		flushLeftButtonNum(PAGE_INBOX_INDEX);
		openInboxReadView();
	}
}

Size UIEmail::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	switch (m_nPageIndex)
	{
	case PAGE_SYSTEM_INDEX:
	{
							  if (m_vSystem.size() > 0)
							  {
								  auto old_item = m_pRoot->getChildByName<Widget*>("panel_system");
								  Size s = old_item->getContentSize();
								  return s;
							  }
							  break;
	}
	case PAGE_INBOX_INDEX:
	{
							 if (m_vIntbox.size() > 0)
							 {
								 auto old_item = m_pRoot->getChildByName<Widget*>("panel_player");
								 Size s = old_item->getContentSize();
								 return s;
							 }
							 break;
	}
	case PAGE_BATTLELOG_INDEX:
	{
								 if (!m_pBattleLogResult)
								 {
									 ProtocolThread::GetInstance()->getLootPlayerLog(0,1, UILoadingIndicator::createWithMask(this, _cameraMask));
									 return Size(0, 0);
								 }
								 if (m_pBattleLogResult->n_log > 0)
								 {
									 auto old_item = m_pRoot->getChildByName<Widget*>("image_log_1");
									 Size s = old_item->getContentSize();
									 return s;
								 }
								 break;
	}
	default:
		break;
	}
	return Size(0,0);
}

//返回cell的个数
ssize_t UIEmail::numberOfCellsInTableView(TableView *table)
{
	switch (m_nPageIndex)
	{
	case PAGE_SYSTEM_INDEX:
	{
							  return m_vSystem.size();
	}
	case PAGE_INBOX_INDEX:
	{
							  return m_vIntbox.size();
	}
	case PAGE_BATTLELOG_INDEX:
	{
							  if (!m_pBattleLogResult)
							  {
								  ProtocolThread::GetInstance()->getLootPlayerLog(0,1, UILoadingIndicator::createWithMask(this, _cameraMask));
								  return 0;
							  }
							  return m_pBattleLogResult->n_log;
	}
	default:
		break;
	}
	return 0;
}

TableViewCell * UIEmail::tableCellAtIndex(TableView *table, ssize_t idx)
{
	// 在table中取一个cell，判断是否为空，为空则创建新的。
	TableViewCell *cell = table->dequeueCell();
	Widget* cell_item;
	if (!cell)
	{
		cell = new TableViewCell();
		cell->autorelease();
		switch (m_nPageIndex)
		{
		case PAGE_SYSTEM_INDEX:
		{
								  auto old_item = m_pRoot->getChildByName<Widget*>("panel_system");
								  cell_item = old_item->clone();
								  break;
		}
		case PAGE_INBOX_INDEX:
		{
								  auto old_item = m_pRoot->getChildByName<Widget*>("panel_player");
								  cell_item = old_item->clone();
								  break;
		}
		case PAGE_BATTLELOG_INDEX:
		{
								  auto old_item = m_pRoot->getChildByName<Widget*>("image_log_1");
								  cell_item = old_item->clone();
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
	case PAGE_SYSTEM_INDEX:
	{
							  cell_item->setTouchEnabled(false);
							  MailDefine *mailDefine = m_vSystem.at(idx);
							  std::string content;
							  getContentParseJson(content, mailDefine);

							  auto panel_content = cell_item->getChildByName<Widget*>("panel_content");
							  auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_content, "label_subject"));
							  auto i_unRead = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_content, "image_unread"));
							  auto t_time = panel_content->getChildByName<Text*>("label_mail_time");
							  auto b_delete = panel_content->getChildByName<Button*>("button_delete");
							  auto t_content = panel_content->getChildByName<Text*>("label_content");
							  //t_content->ignoreContentAdaptWithSize(false);

							  auto panel_attach = cell_item->getChildByName<Widget*>("panel_attach");
							  auto b_item = panel_attach->getChildByName<Button*>("button_items");
							  auto t_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_silver_num"));
							  auto i_silver = panel_attach->getChildByName<ImageView*>("image_silver");
							  auto b_take = panel_attach->getChildByName<Button*>("button_take");
							  auto i_gift = panel_attach->getChildByName<ImageView*>("image_gift");
							  auto t_gift = panel_attach->getChildByName<Text*>("label_gift");
							  auto b_getgift_reply = panel_attach->getChildByName<Button*>("button_getgift_reply");
							  b_getgift_reply->addTouchEventListener(CC_CALLBACK_2(UIEmail::systemButtonEvent, this));
							  b_getgift_reply->setTag(idx);
							  b_getgift_reply->setVisible(false);
							  auto label_reply = dynamic_cast<Text*>(Helper::seekWidgetByName(b_getgift_reply, "label_reply"));
							  label_reply->setTouchEnabled(true);
							  label_reply->addTouchEventListener(CC_CALLBACK_2(UIEmail::systemButtonEvent, this));
							  label_reply->setTag(idx);
							  std::string html_color_begin="";
							  std::string html_color_end="";
                              #if WIN32
							  html_color_begin = "";
							  html_color_end = "";
                             #else
							  //下划线
							 // html_color_begin = "<html><font size='24' color='#2E1D0E'><u >";
							  //html_color_end = "</u></font></html>";
                             #endif
							  std::string line_content;
							  line_content += html_color_begin;
							  line_content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_FRIEND_GIFT_BACK_TITLE"];
							  line_content += html_color_end;
							  label_reply->setString(line_content);


							  /*
							  t_title->setTextHorizontalAlignment(TextHAlignment::LEFT);
							  t_title->setTextVerticalAlignment(TextVAlignment::CENTER);
							  t_title->setContentSize(Size(378,t_title->getFontSize()*1.5));
							  */
							  i_silver->setPositionX(b_item->getPositionX() + 88.9);

							  //解决无法滑动的问题，工程更新以后可以删除。
							  panel_content->setTouchEnabled(false);
							  panel_content->setSwallowTouches(false);
							  panel_attach->setTouchEnabled(false);
							  panel_attach->setSwallowTouches(false);

							  i_gift->setVisible(false);
							  t_gift->setVisible(false);
							  b_take->setVisible(true);
							  int titleIndex = atoi(mailDefine->title);
							  switch (titleIndex){
							  case 1:
							  case 2:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_ORDER_INFO"]);
								  break;
							  case 3:
							  case 10:
							  case 15:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_GUILD_INFO"]);
								  break;
							  case 11:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_GUILD_INFO"]);
								  content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_GUILD_DISSOLVE"];
								  break;
							  case 4:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_MAYOR_INFO1"]);
								  content += "\n";
								  content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_INFO1"];
								  break;
							  case 5:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_MAYOR_INFO2"]);
								  content += "\n";
								  content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_INFO2"];
								  break;
							  case 6:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_MAYOR_INFO3"]);
								  content += "\n";
								  content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_INFO3"];
								  break;
							  case 7:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_RESCUE_EMAIL"]);
								  break;
							  case 8:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_LOGIN_REWARD"]);
								  break;
							  case 9:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BULID_FINISH"]);
								  break;
							  case 12:
								  //收到礼物的提示文本及内容
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_RECEIVE_GIFT_TITLE"]);
								  break;
							  case 13:
								  //反馈收礼方取走礼物的提示文本及内容
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_GIFT_GIVEN_BE_TAKE_AWAY_TITLE"]);
								  break;
							  case 14:
								  //攻打海盗基地给予的奖励文本及内容
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_AGAINST_PIRATE_BASES_REWARD_TITLE"]);
								  break;
							  case 16:
								  //保险系统赔付
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_INSURANCE_COMPENSATION"]);
								  content += "\n";
								  content += SINGLE_SHOP->getTipsInfo()["TIP_BANK_INSURANCE_COMPENSATION_CONTENT"];
								  break;
							  case 17:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_LEVEL_UP_REWARD_TITLE"]);
								  break;
							  case 18:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_LOTTO_NOTICE_TITLE"]);
								  //邀请玩家奖励邮件
							  case 19:
							      t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_FACEBOOK_INVITE_OTHERS_TITLE"]);
								  break;
								  //首充v奖励邮件
							  case 20:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_V_ACTIVITY_BONUS_TITLE"]);
								  break;
							  case 21:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_BOSS_DEFEATED_TITLE"]);
								  break;
								  //城市沦陷，所有抢到钱的会收到一份邮件
							  case 22:
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_CITY_FALLEN_HAS_MONEY_TITLE"]);
								  break;
							  case 23:
								  //城市沦陷，本国玩家会收到一个邮件
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_CITY_FALLEN_NO_MONEY_TITLE"]);
								  break;
							  case 24:
								  //国战结束，双方国家收到邮件
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_COUNTRY_WAR_END_TITLE"]);
								  break;
							  case 25:
								  //国战排行版奖励，排行版前若干名玩家，有奖励的，会收到奖励邮件
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_COUNTRY_WAR_RANKING_REWARD_TITLE"]);
								  break;
							  case 26:
								  //两国宣战
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_COUNTRY_WAR_START_TITLE"]);
								  break;
							  case 27:
								  //两国进入战争阶段
								  t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_COUNTRY_WAR_ING_TITLE"]);
								  break;
							  default:
								  //当前系统邮件不匹配任何类型
								  t_title->setString(mailDefine->title);
								  content = mailDefine->message;
								  break;
							  }
							  
							  t_content->setString(content);
							  t_content->setContentSize(t_content->getContentSize() + Size(0, t_content->getFontSize()));

							  b_item->setVisible(false);
							  std::vector<MAIL_ITEM_INFO> v_itemInfo; 
							  v_itemInfo.clear();
							  for (int i = 0; i<3; i++)
							  {
								  auto buttonName = StringUtils::format("b_item_%d",i);
								  auto b_item_clone = panel_attach->getChildByName<Button*>(buttonName);
								  if (b_item_clone)
								  {
									  b_item_clone->removeFromParentAndCleanup(true);
								  }
							  }

							  if (mailDefine->attachid > 0 || mailDefine->coinnum > 0 || mailDefine->fame > 0 || mailDefine->sailor_coin > 0 || mailDefine->cantakeattachment == 1)
							  {
								  panel_attach->setVisible(true);
								  if (mailDefine->coinnum > 0)
								  {
									  i_silver->setVisible(true);
									  t_silver_num->setVisible(true);
									  
									  t_silver_num->setString(numSegment(StringUtils::format("%lld", mailDefine->coinnum)));
								  }
								  else
								  {
									  i_silver->setVisible(false);
									  t_silver_num->setVisible(false);
								  }
								  if (mailDefine->attachid > 0 || mailDefine->fame > 0 || mailDefine->sailor_coin > 0)
								  {
									  std::string name;
									  std::string path;
									  int cloneNum = 0;
									 
									  if (mailDefine->attachid > 0)
									  {
										  MAIL_ITEM_INFO m_item;
										  cloneNum += 1;
										  m_item.type = mailDefine->attachtype;
										  m_item.itemId = mailDefine->attachid;
										  m_item.itemAmount = mailDefine->attachamount;
										  v_itemInfo.push_back(m_item);
									  }
									  if (mailDefine->fame > 0)
									  {
										  MAIL_ITEM_INFO m_item;
										  cloneNum += 1;
										  m_item.type = ITEM_TYPE_SPECIAL;
										  m_item.itemId = 10003;
										  m_item.itemAmount = mailDefine->fame;
										  v_itemInfo.push_back(m_item);
									  }
									  if (mailDefine->sailor_coin > 0)
									  {
										  MAIL_ITEM_INFO m_item;
										  cloneNum += 1;
										  m_item.type = ITEM_TYPE_SPECIAL;
										  m_item.itemId = 10001;
										  m_item.itemAmount = mailDefine->sailor_coin;
										  v_itemInfo.push_back(m_item);
									  }
									  for (int i = 0; i < cloneNum; i++)
									  {
										  getItemNameAndPath(v_itemInfo[i].type, v_itemInfo[i].itemId, name, path);
										  auto b_item_0 = b_item->clone();
										  b_item->getParent()->addChild(b_item_0);
										  b_item_0->setCameraMask(_cameraMask);
										  b_item_0->setPosition(Vec2(b_item->getPositionX() + i * (b_item->getBoundingBox().size.width + 2),b_item->getPositionY()));
										  b_item_0->setVisible(true);
										  b_item_0->setTouchEnabled(true);
										  b_item_0->setTag(v_itemInfo[i].itemId);
										  b_item_0->setName(StringUtils::format("b_item_%d",i));
										  auto i_goods_bg = b_item_0->getChildByName<ImageView*>("image_goods_bg");
										  auto i_goods = b_item_0->getChildByName<ImageView*>("image_goods");
										  auto t_num = i_goods->getChildByName<Text*>("text_item_skill_lv");
										  i_goods_bg->setTag(v_itemInfo[i].type);
										  i_goods->ignoreContentAdaptWithSize(false);
										  i_goods->loadTexture(path);
										  //i_goods->setTag(mailDefine->uniqueid);
										  setTextSizeAndOutline(t_num, v_itemInfo[i].itemAmount);
										  setBgButtonFormIdAndType(i_goods_bg, v_itemInfo[i].itemId, v_itemInfo[i].type);

										  if (mailDefine->cantakeattachment == 1)
										  {
											  t_num->setColor(Color3B(255, 255, 255));
											  setGLProgramState(i_goods, false);
											  setGLProgramState(i_goods_bg, false);
										  }
										  else
										  {
											  t_num->setColor(Color3B(161, 161, 161));
											  setGLProgramState(i_goods, true);
											  setGLProgramState(i_goods_bg, true);
										  }
									  }
								  }
								  else
								  {
									  if (mailDefine->coinnum > 0)
									  {
										  i_silver->setPositionX(b_item->getPositionX());
									  }
								  }
								  t_silver_num->setPositionX(i_silver->getPositionX() + i_silver->getContentSize().width / 2 + 5);
								  if (mailDefine->cantakeattachment == 1)
								  {
									  b_take->setTouchEnabled(true);
									  b_take->setBright(true);
									  b_take->addTouchEventListener(CC_CALLBACK_2(UIEmail::systemButtonEvent, this));
									  t_silver_num->setOpacity(255);
									  i_silver->setOpacity(255);
									  if (strcmp(mailDefine->title, "12") == 0)//收到礼物
									  {
										  if (b_getgift_reply)
										  {
											  b_getgift_reply->setVisible(false);
											  b_take->setVisible(true);
										  }					
									  }
								  }
								  else
								  {
									  b_take->setTouchEnabled(false);
									  b_take->setBright(false);
									  t_silver_num->setOpacity(128);
									  i_silver->setOpacity(128);
									  if (strcmp(mailDefine->title, "12") == 0)//收到礼物
									  {
										  if (b_getgift_reply)
										  {
											  b_getgift_reply->setVisible(true);
											  if (true)//回赠礼物成功
											  {
											  }
											  b_take->setVisible(false);
										  }
									  }
								  }							
								  if (strcmp(mailDefine->title, "12") == 0 || strcmp(mailDefine->title, "13") == 0 || strcmp(mailDefine->title, "14") == 0)
								  {
									  //12收礼物 13收礼反馈 14攻打海盗基地给予的奖励
									  i_gift->setVisible(true);
									  setGLProgramState(i_gift, false);
									  t_gift->setVisible(true);
									  b_item->setVisible(false);
									  i_silver->setVisible(false);
									  t_silver_num->setVisible(false);
								  }
								  panel_content->setPositionY(panel_attach->getPositionY() + panel_attach->getContentSize().height + 5);
							  }
							  else
							  {
								  if (strcmp(mailDefine->title, "12") == 0 || strcmp(mailDefine->title, "13") == 0 || strcmp(mailDefine->title, "14") == 0)
								  {
									  //礼物领取
									  panel_attach->setVisible(true);
									  i_gift->setVisible(true);
									  t_gift->setVisible(true);
									  b_item->setVisible(false);
									  i_silver->setVisible(false);
									  t_silver_num->setVisible(false);
									  setGLProgramState(i_gift, true);
									  b_take->setTouchEnabled(false);
									  b_take->setBright(false);
									  if (strcmp(mailDefine->title, "12") == 0)//收到礼物
									  {
										  if (b_getgift_reply)
										  {
											  b_getgift_reply->setVisible(true);
										
											  if (true)//回赠礼物成功
											  {
											  }
											  b_take->setVisible(false);
										  }
									  }
									  else
									  {
										  if (b_getgift_reply)
										  {
											  b_getgift_reply->setVisible(false);
										  }
									  }
								  }
								  else
								  {
									  panel_attach->setVisible(false);
									  panel_content->setPositionY(panel_attach->getPositionY() + panel_attach->getContentSize().height + 5);
								  }
							  }
							  t_time->setString(timeUtil(mailDefine->sendtime, TIME_UTIL::_TU_ALL, TIME_TYPE::_TT_LOCAL));
							  t_time->setPositionX(b_delete->getPositionX() - t_time->getContentSize().width - 30);
							  if (mailDefine->status == 0)
							  {
								  i_unRead->setVisible(true);
								  t_title->setTextColor(Color4B(169, 86, 1, 255));
								  i_unRead->setPositionX(t_title->getPositionX() + t_title->getBoundingBox().size.width + i_unRead->getContentSize().width / 2);
							  }
							  else
							  {
								  i_unRead->setVisible(false);
								  t_title->setTextColor(Color4B(40, 25, 13, 255));
							  }
							  cell_item->setTag(idx);
							  b_take->setTag(idx);

							  b_delete->setTag(idx);
							  b_delete->addTouchEventListener(CC_CALLBACK_2(UIEmail::systemButtonEvent, this));

							  break;
	}
	case PAGE_INBOX_INDEX:
	{
							 cell_item->setTouchEnabled(true);
							 auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_subject"));
							 auto i_unRead = dynamic_cast<Widget*>(Helper::seekWidgetByName(cell_item, "image_unread"));
							 auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_player_name"));
							 auto t_time = cell_item->getChildByName<Text*>("label_mail_time");
							 auto b_delete = cell_item->getChildByName<Button*>("button_delete");
							 MailDefine* mailDefine = m_vIntbox.at(idx);
							 t_title->setContentSize(Size(620, 25));
							 t_title->setTextHorizontalAlignment(TextHAlignment::LEFT);
							 t_title->setTextVerticalAlignment(TextVAlignment::CENTER);
							 t_title->setString(mailDefine->title);
							 if (t_name)
							 {
								 t_name->setString(mailDefine->peername);
							 }
							 t_time->setString(timeUtil(mailDefine->sendtime, TIME_UTIL::_TU_ALL, TIME_TYPE::_TT_LOCAL));
							 if (mailDefine->status == 0)
							 {
								 i_unRead->setVisible(true);
								 t_title->setTextColor(Color4B(169, 86, 1, 255));
								 i_unRead->setPositionX(t_title->getPositionX() + t_title->getBoundingBox().size.width + i_unRead->getContentSize().width / 2);
							 }
							 else
							 {
								 i_unRead->setVisible(false);
								 t_title->setTextColor(Color4B(40, 25, 13, 255));
							 }
							 cell_item->setTag(idx);
							 b_delete->setTag(idx);
							 b_delete->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent, this));

							 break;
	}
	case PAGE_BATTLELOG_INDEX:
	{
								 if (!m_pBattleLogResult)
								 {
									 ProtocolThread::GetInstance()->getLootPlayerLog(0,1, UILoadingIndicator::createWithMask(this, _cameraMask));
									 return nullptr;
								 }
								 auto playerInfo = m_pBattleLogResult->log[idx];
								 cell_item->setTouchEnabled(true);
								 auto i_flag = cell_item->getChildByName<ImageView*>("image_flag");
								 auto t_name = cell_item->getChildByName<Text*>("label_name");
								 auto t_lv = cell_item->getChildByName<Text*>("label_lv");
								 auto t_loot = cell_item->getChildByName<Text*>("label_looted_num");
								 auto t_time = cell_item->getChildByName<Text*>("label_time");
								 auto t_won = cell_item->getChildByName<Text*>("label_won");
								 auto t_lose = cell_item->getChildByName<Text*>("label_lose");
								 auto b_raplay = cell_item->getChildByName<Button*>("button_replay");
								 auto b_revenge = cell_item->getChildByName<Button*>("button_revenge");
								 i_flag->loadTexture(getCountryIconPath(playerInfo->peerinfo->nation));
								 t_name->setString(playerInfo->peerinfo->heroname);
								 t_lv->setString(StringUtils::format("Lv. %d", playerInfo->peerinfo->level));
								 t_loot->setString(StringUtils::format("%lld", playerInfo->coins));

								 std::string content;
								 int time = playerInfo->secondsago / (60.0 * 60.0 * 24);
								 if (time > 0)
								 {
									 content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BATTLE_LOG_DATS_AGO"];
								 }
								 else
								 {
									 time = playerInfo->secondsago / (60.0 * 60.0);
									 if (time > 0)
									 {
										 content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BATTLE_LOG_HOURS_AGO"];
									 }
									 else
									 {
										 time = ceil(playerInfo->secondsago / 60.0);
										 content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BATTLE_LOG_MINS_AGO"];
									 }
								 }

								 std::string new_value = StringUtils::format("%d", time);
								 std::string old_value = "[time]";
								 repalce_all_ditinct(content, old_value, new_value);
								 t_time->setString(content);
								 //1:失败 2:胜利 3:平局 4:逃跑
								 if (playerInfo->result == 1)
								 {
									 t_won->setVisible(false);
									 t_lose->setVisible(true);
								 }
								 else
								 {
									 t_won->setVisible(true);
									 t_lose->setVisible(false);
								 }
								 b_revenge->setTag(playerInfo->id);
								 if (playerInfo->peerinfo->lootforbid == 1)
								 {
				
									 b_revenge->setBright(false);
									 b_revenge->setTouchEnabled(false);
								 }
								 else
								 {
									 b_revenge->setTouchEnabled(true);
									 b_revenge->setBright(true);
								 }
								 //TODO
								 b_raplay->setVisible(false);

								 break;
	}
	default:
		break;
	}

	return cell;
}

/*
 * 显示系统通知邮件，服务器返回数据以后调用。
 * 目前耗时很长，需要优化。
 */
//系统邮箱界面  
void UIEmail::openSystemView()
{
	long a = Utils::gettime();
	openView(MAIL_COCOS_RES[MAIL_CSB]);
	flushLeftButtonNum(PAGE_SYSTEM_INDEX);
	//log("mid openSystemView:%d", Utils::gettime()-a );
	auto b_mail_info = m_pRoot->getChildByName<Button*>("button_mail_info");
	b_mail_info->setTouchEnabled(true);
	b_mail_info->setVisible(true);
	auto image_div = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot, "image_div"));
	image_div->setVisible(false);
	auto b_write = m_pRoot->getChildByName<Button*>("button_write");
	b_write->setVisible(false);
	auto l_mail = m_pRoot->getChildByName<ListView*>("listview_mail");
	l_mail->setVisible(false);
	l_mail->removeAllChildrenWithCleanup(true);
	auto l_notification = m_pRoot->getChildByName<ListView*>("listview_notification");
	l_notification->setVisible(false);
	l_notification->removeAllChildrenWithCleanup(true);
	auto tableViewInbox = l_mail->getParent()->getChildByTag(1000);
	if (tableViewInbox)
	{
		tableViewInbox->removeFromParentAndCleanup(true);
	}
	auto tableViewBattleLog = l_notification->getParent()->getChildByTag(998);
	if (tableViewBattleLog)
	{
		tableViewBattleLog->removeFromParentAndCleanup(true);
	}
	auto w_content = m_pRoot->getChildByName<Widget*>("panel_centent_no");
	w_content->setVisible(false);
	auto image_pulldown = m_pRoot->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	image_pulldown->setVisible(false);
	TableView* mTableView = dynamic_cast<TableView*>(l_notification->getParent()->getChildByTag(999));
	if (m_vSystem.empty())
	{
		w_content->getChildByName<Text*>("label_no_found_1_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEMLIST_NO"]);
		w_content->getChildByName<Text*>("label_no_found_2_1")->setString("");
		w_content->setVisible(true);
		if (mTableView)
		{
			mTableView->reloadData();
		}
		return;
	}

	//log("mid openSystemView:%d", Utils::gettime()-a );
	//log("table: %f %f", l_notification->getContentSize().width, l_notification->getContentSize().height);
	
	if(!mTableView){
		mTableView = TableView::create(this, CCSizeMake(l_notification->getContentSize().width,l_notification->getContentSize().height));
		l_notification->getParent()->addChild(mTableView);
	}
	mTableView->setVisible(true);
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(l_notification->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(999);
	mTableView->setTouchEnabled(true);
	image_pulldown->setLocalZOrder(10001);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	//log("mid openSystemView:%d", Utils::gettime()-a );
	this->setCameraMask(_cameraMask, true);
	//log("end openSystemView:%d", Utils::gettime()-a );
}
//战斗日志
void UIEmail::openBattleLogView()
{
	openView(MAIL_COCOS_RES[MAIL_CSB]);
	flushLeftButtonNum(PAGE_BATTLELOG_INDEX);
	auto view = getViewRoot(MAIL_COCOS_RES[MAIL_CSB]);
	auto b_mail_info = view->getChildByName<Button*>("button_mail_info");
	b_mail_info->setTouchEnabled(false);
	b_mail_info->setVisible(false);
	auto image_div = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_div"));
	image_div->setVisible(false);
	auto b_write = view->getChildByName<Button*>("button_write");
	b_write->setVisible(false);
	auto l_mail = view->getChildByName<ListView*>("listview_mail");
	l_mail->setVisible(false);
	l_mail->removeAllChildrenWithCleanup(true);
	auto l_notification = view->getChildByName<ListView*>("listview_notification");
	l_notification->setVisible(false);
	l_notification->removeAllChildrenWithCleanup(true);
	auto tableViewBattleSystem = l_notification->getParent()->getChildByTag(999);
	if (tableViewBattleSystem)
	{
		tableViewBattleSystem->removeFromParentAndCleanup(true);
	}
	auto tableViewInbox = l_mail->getParent()->getChildByTag(1000);
	if (tableViewInbox)
	{
		tableViewInbox->removeFromParentAndCleanup(true);
	}
	auto w_content = view->getChildByName<Widget*>("panel_centent_no");
	w_content->setVisible(false);
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	image_pulldown->setVisible(false);
	TableView* mTableView = dynamic_cast<TableView*>(l_notification->getParent()->getChildByTag(998));
	if (!m_pBattleLogResult)
	{
		ProtocolThread::GetInstance()->getLootPlayerLog(0, 1,UILoadingIndicator::createWithMask(this, _cameraMask));
		return;
	}
	if (m_pBattleLogResult->n_log < 1)
	{
		w_content->getChildByName<Text*>("label_no_found_1_1")->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEMLIST_NO"]);
		w_content->getChildByName<Text*>("label_no_found_2_1")->setString("");
		w_content->setVisible(true);
		if (mTableView)
		{
			mTableView->reloadData();
		}
		return;
	}

	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(l_notification->getContentSize().width, l_notification->getContentSize().height));
		l_notification->getParent()->addChild(mTableView);
	}
	mTableView->setVisible(true);
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(l_notification->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(998);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	this->setCameraMask(_cameraMask, true);
}
//收信箱邮件中详情界面
void UIEmail::openInboxReadView()
{
	openView(MAIL_COCOS_RES[MAIL_VIEW_PLAYER_CSB]);
	auto i_palyer = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"image_head"));
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot,"label_title"));
	auto b_info = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot,"button_info"));
	auto t_subject = m_pRoot->getChildByName<Text*>("label_subject");
	auto t_time = m_pRoot->getChildByName<Text*>("label_subject_time");

	auto image_mail_content_bg = m_pRoot->getChildByName<ImageView*>("image_mail_content_bg");
	auto t_content = image_mail_content_bg->getChildByName<Text*>("label_mail_content");
	auto image_pulldown = image_mail_content_bg->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	auto image_div = m_pRoot->getChildByName<ImageView*>("image_div");
	auto image_div_2 = m_pRoot->getChildByName<ImageView*>("image_div_2");
	auto panel_attach = m_pRoot->getChildByName<Widget*>("panel_attach");
	auto b_item = panel_attach->getChildByName<Button*>("button_items");
	auto i_goods = b_item->getChildByName<ImageView*>("image_goods");
	auto t_num = i_goods->getChildByName<Text*>("text_item_num");
	auto b_silver = panel_attach->getChildByName<Button*>("button_silver");
	auto i_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot, "image_silver"));
	auto t_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, "label_silver_num"));

	auto image_mail_content_bg_2 = m_pRoot->getChildByName<ImageView*>("image_mail_content_bg_2");
	auto t_content_2 = image_mail_content_bg_2->getChildByName<Text*>("label_mail_content");
	auto image_pulldown_2 = image_mail_content_bg_2->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown_2 = image_pulldown_2->getChildByName<Button*>("button_pulldown");

	auto b_take = panel_attach->getChildByName<Button*>("button_takeall");
	auto b_delete = m_pRoot->getChildByName<Button*>("button_delete");
	auto b_reply = m_pRoot->getChildByName<Button*>("button_reply");
	auto b_close = m_pRoot->getChildByName<Button*>("button_close");
	auto i_goods_bg = b_item->getChildByName<ImageView*>("image_goods_bg");

	auto mailDefine = m_vIntbox[m_nEmailIndex];
	i_palyer->ignoreContentAdaptWithSize(false);
	i_palyer->loadTexture(getPlayerIconPath(mailDefine->peericon));
	t_name->setString(mailDefine->peername);
	t_subject->setString(mailDefine->title);
	t_time->setString(timeUtil(mailDefine->sendtime, TIME_UTIL::_TU_ALL, TIME_TYPE::_TT_LOCAL));
	if (mailDefine->optionalitemnum > 0)
	{
		addStrengtheningIcon(i_goods_bg);
	}
	auto i_intensify = Helper::seekWidgetByName(i_goods_bg, "goods_intensify");

	if (mailDefine->attachid > 0 || mailDefine->coinnum > 0)
	{
		image_mail_content_bg->setVisible(true);
		image_mail_content_bg_2->setVisible(false);
		image_div->setVisible(true);
		image_div_2->setVisible(true);
		panel_attach->setVisible(true);
		if (mailDefine->attachid > 0)
		{
			std::string name;
			std::string path;
			getItemNameAndPath(mailDefine->attachtype, mailDefine->attachid, name, path);
			b_item->setVisible(true);
			b_item->setTouchEnabled(true);
			b_item->setTag(mailDefine->attachid);
			i_goods_bg->setTag(mailDefine->attachtype);
			i_goods->ignoreContentAdaptWithSize(false);
			i_goods->loadTexture(path);
			i_goods->setTag(mailDefine->uniqueid);
			setTextSizeAndOutline(t_num, mailDefine->attachamount);
			setBgButtonFormIdAndType(i_goods_bg, mailDefine->attachid, mailDefine->attachtype);
		}
		else
		{
			b_item->setVisible(false);
			b_silver->setPositionX(b_item->getPositionX());
		}
		if (mailDefine->coinnum > 0)
		{
			b_silver->setVisible(true);
		}
		else
		{
			b_silver->setVisible(false);
		}
		t_silver_num->setString(numSegment(StringUtils::format("%d", mailDefine->coinnum)));
		t_content->setString(mailDefine->message);

		image_pulldown->setVisible(false);
		
		if (mailDefine->cantakeattachment == 1)
		{
			b_take->setTouchEnabled(true);
			b_take->setBright(true);
			b_take->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_TAKE_ALL"]);
			b_take->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent,this));
			b_take->setTitleColor(Color3B(255, 255, 255));
			t_num->setColor(Color3B(255, 255, 255));
			t_silver_num->setOpacity(255);
			setGLProgramState(i_goods, false);
			setGLProgramState(i_goods_bg, false);
			if (i_intensify)
			{
				setGLProgramState(i_intensify, false);
			}
			i_silver->setOpacity(255);
		}
		else
		{
			b_take->setTouchEnabled(false);
			b_take->setBright(false);
			b_take->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_TAKEED_ALL"]);
			b_take->setTitleColor(Color3B(161, 161, 161));
			t_num->setColor(Color3B(161, 161, 161));
			t_silver_num->setOpacity(128);
			setGLProgramState(i_goods, true);
			setGLProgramState(i_goods_bg, true);
			if (i_intensify)
			{
				setGLProgramState(i_intensify, true);
			}
			i_silver->setOpacity(128);
		}
	}
	else
	{
		image_mail_content_bg->setVisible(false);
		image_mail_content_bg_2->setVisible(true);
		image_div->setVisible(false);
		image_div_2->setVisible(false);
		panel_attach->setVisible(false);
		t_content_2->setString(mailDefine->message);
		//TODO
		image_pulldown_2->setVisible(false);
	}

	b_delete->setTag(100000);
	b_delete->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent,this));
	b_reply->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent,this));
	b_close->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent, this));
	b_info->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent,this));
	b_info->setTag(mailDefine->peerid);
	mailDefine->status = 1;
	flushLeftButtonNum(PAGE_INBOX_INDEX);
}
//发送邮件界面	
void UIEmail::openComposeView()
{
	openView(MAIL_COCOS_RES[MAIL_WRITE_CSB]);
	auto p_numpad_mark = m_pRoot->getChildByName<Widget*>("panel_numpad_mark");
	p_numpad_mark->setPosition(ENDPOS);
	auto t_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_player_name"));
	auto t_subject = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_subject"));
	auto t_content = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot,"textfield_content"));
	t_name->setVisible(false);
	t_subject->setVisible(false);
	t_content->setVisible(false);
	ui::EditBox* edit_name = nullptr;
	ui::EditBox* edit_subject = nullptr;
	ui::EditBox* edit_conten = nullptr;
	if (!edit_name)
	{
		    edit_name = cocos2d::ui::EditBox::create(Size(t_name->getContentSize()), "input.png");
		    t_name->getParent()->addChild(edit_name);
			edit_name->setTag(1);
			edit_name->setPosition(t_name->getPosition());
			edit_name->setAnchorPoint(t_name->getAnchorPoint());
			edit_name->setPlaceholderFontColor(Color3B(116, 98, 71));
			edit_name->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
			edit_name->setFontColor(Color3B(46, 29, 14));
			edit_name->setOpacity(127);
			edit_name->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
			edit_name->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	}
	if (!edit_subject)
	{
		edit_subject = cocos2d::ui::EditBox::create(Size(t_subject->getContentSize()), "input.png");
		t_subject->getParent()->addChild(edit_subject);
		edit_subject->setTag(2);
		edit_subject->setPosition(t_subject->getPosition());
		edit_subject->setAnchorPoint(t_subject->getAnchorPoint());
		edit_subject->setPlaceholderFontColor(Color3B(116, 98, 71));
		edit_subject->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
		edit_subject->setFontColor(Color3B(46, 29, 14));
		edit_subject->setMaxLength(40);
		edit_subject->setOpacity(127);
		edit_subject->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		edit_subject->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	}
	if (!edit_conten)
	{
		edit_conten = cocos2d::ui::EditBox::create(Size(t_content->getContentSize()), "input.png");
		t_content->getParent()->addChild(edit_conten);
		edit_conten->setTag(3);
		edit_conten->setPosition(t_content->getPosition());
		edit_conten->setAnchorPoint(t_content->getAnchorPoint());
		edit_conten->setPlaceholderFontColor(Color3B(116, 98, 71));
		edit_conten->setFont(CUSTOM_FONT_NAME_1.c_str(), 26);
		edit_conten->setFontColor(Color3B(46, 29, 14));
		edit_conten->setOpacity(127);
		edit_conten->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		edit_conten->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	}
	auto t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot,"label_silver_num"));
	auto b_item = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot,"button_items"));
	//不让发送物品
	b_item->setVisible(false);
	auto i_bgImage = b_item->getChildByName<ImageView*>("image_goods_bg");
	auto i_icon = b_item->getChildByName<ImageView*>("image_goods");
	auto t_num = i_icon->getChildByName<Text*>("text_item_num");
	auto b_choose = m_pRoot->getChildByName<Button*>("button_choose");
	auto b_rewrite = m_pRoot->getChildByName<Button*>("button_rewrite");
	auto b_send = m_pRoot->getChildByName<Button*>("button_send");
	auto b_compose_close = m_pRoot->getChildByName<Button*>("button_compose_close");
	auto p_silver = Helper::seekWidgetByName(m_pRoot, "panel_silver");
	auto t_silver_1 = p_silver->getChildByName<Text*>("label_silver_num");
	auto t_gold = p_silver->getChildByName<Text*>("label_gold_num");
	t_silver->setString("0");
	t_silver->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent,this));
	t_num->setVisible(false);
	i_bgImage->ignoreContentAdaptWithSize(false);
	i_bgImage->loadTexture("cocosstudio/login_ui/player_720/goods_warehouse_bg.png");
	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture("cocosstudio/login_ui/mail_720/add.png");
	b_item->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent,this));
	b_choose->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent,this));
	b_rewrite->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent,this));
	b_send->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent,this));
	b_compose_close->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent, this));
	b_choose->setVisible(true);
	m_nItemId = 0;
	m_nItemIid = 0;
	m_nItemNum = 0;
	m_nItemType = 0;
	this->setCameraMask(_cameraMask, true);
	t_silver->setOpacity(127);
	scheduleUpdate();

	t_silver_1->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iCoin)));
	t_gold->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iGold)));
	t_silver_1->setContentSize(Size(220, 34));
	t_gold->setContentSize(Size(220, 34));
	setTextFontSize(t_silver_1);
	setTextFontSize(t_gold);
	auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot, "image_pulldown"));
	image_pulldown->setVisible(false);
}
//显示数字键盘	
void UIEmail::showNumpad()
{
	m_bIsActionFlag = false;
	auto view = getViewRoot(MAIL_COCOS_RES[MAIL_WRITE_CSB]);
	auto p_numpad_mark = view->getChildByName<Widget*>("panel_numpad_mark");
	p_numpad_mark->setPosition(STARTPOS);
	auto num_root = view->getChildByName<Widget*>("panel_numpad");
	num_root->setPosition(ENDPOS4);
	num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, STARTPOS),
		CallFunc::create(this, callfunc_selector(UIEmail::moveEnd))));
	auto b_yes = num_root->getChildByName<Button*>("button_numpad_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent,this));
	auto b_del = num_root->getChildByName<Button*>("button_del");
	b_del->addTouchEventListener(CC_CALLBACK_2(UIEmail::numPadButtonEvent,this));
	p_numpad_mark->addTouchEventListener(CC_CALLBACK_2(UIEmail::inboxButtonEvent,this));

	m_padString.clear();
	auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, "label_silver_num"));
	if (label_silver_num)
	{
		label_silver_num->setString("0");
		label_silver_num->setOpacity(127);

	}

	for (size_t i = 0; i < 10; i++)
	{
		auto b_num = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot, StringUtils::format("button_%d", i)));
		b_num->setTag(i);
		b_num->addTouchEventListener(CC_CALLBACK_2(UIEmail::numPadButtonEvent,this));
	}
}
//好友列表选择好友
void UIEmail::openChooseFirend(const GetFriendsListResult *pFriendsResult)
{
	openView(MAIL_COCOS_RES[MAIL_CHOOSE_FRIEND_CSB]);
	auto w_no = m_pRoot->getChildByName<Widget*>("panel_search_no");
	auto l_friend = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_pRoot,"listview_friend"));
	l_friend->removeAllChildrenWithCleanup(true);
	w_no->setVisible(false);
	auto b_close = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pRoot, "button_close"));
	b_close->setName("button_friend_choose_close");
	b_close->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent, this));
	auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot, "image_pulldown"));
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (pFriendsResult->n_friends < 1)
	{
		w_no->setVisible(true);
		image_pulldown->setVisible(false);
		button_pulldown->setVisible(false);
		return;
	}
	auto b_friend = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pRoot,"panel_friend"));
	b_friend->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent,this));
	for (int i = 0; i < pFriendsResult->n_friends; i++)
	{
		auto item = b_friend->clone();
		auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item,"image_head"));
		auto i_flag = item->getChildByName<ImageView*>("image_flag");
		auto t_name = item->getChildByName<Text*>("label_player_name");
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_lv"));
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(getPlayerIconPath(pFriendsResult->friends[i]->iconidx));
		i_flag->ignoreContentAdaptWithSize(false);
		i_flag->loadTexture(getCountryIconPath(pFriendsResult->friends[i]->nation));
		t_name->setString(pFriendsResult->friends[i]->heroname);
		t_lv->setString(StringUtils::format("Lv. %d",pFriendsResult->friends[i]->level));
		l_friend->pushBackCustomItem(item);
	}
	
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(l_friend,image_pulldown);
	this->setCameraMask(_cameraMask, true);
}
//选择物品界面 
void UIEmail::openComposeEncloseView()
{
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
	if (!view)
	{
		openView(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
		view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
	
		m_pTempButton = nullptr;
		changeMinorButtonState(nullptr);
	}
	auto panel_buy = view->getChildByName<Widget*>("panel_buy");
	auto l_item = panel_buy->getChildByName<ListView*>("listview_item");
	l_item->removeAllChildrenWithCleanup(true);
	auto p_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_itemdetails"));
	p_item->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent,this));
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title_create_sell_order"));
	auto t_subtitle = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title_select_item"));
	auto &tips = SINGLE_SHOP->getTipsInfo();
	t_title->setString(tips["TIP_EMAIL_SEND_ITEM_TITLE"]);
	t_subtitle->setString(tips["TIP_EMAIL_SEND_ITEM_SUBTITLE"]);

	std::string itemName;
	std::string itemPath;
	
	switch (m_pItemResult->itemtype)
	{
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
		{
			for (int i = 0; i < m_pItemResult->n_equipments; i++)
			{
				getItemNameAndPath(m_pItemResult->itemtype, m_pItemResult->equipments[i]->equipmentid, itemName, itemPath);
				auto item = p_item->clone();
				item->setTag(i);
				auto b_bgButton = item->getChildByName<Button*>("button_good_bg");
				auto i_bgImage = b_bgButton->getChildByName<ImageView*>("image_item_bg_lv");
				auto i_icon = b_bgButton->getChildByName<ImageView*>("image_goods");
				auto t_name = item->getChildByName<Text*>("label_goods_name");
				auto t_num = item->getChildByName<Text*>("label_goods_num");
				i_icon->ignoreContentAdaptWithSize(false);
				i_icon->loadTexture(itemPath);
				t_name->setString(itemName);
				t_num->setString(StringUtils::format("x %d", m_pItemResult->equipments[i]->amount));
				l_item->pushBackCustomItem(item);

				setBgButtonFormIdAndType(b_bgButton, m_pItemResult->equipments[i]->equipmentid, m_pItemResult->itemtype);
				setBgImageColorFormIdAndType(i_bgImage, m_pItemResult->equipments[i]->equipmentid, m_pItemResult->itemtype);
				setTextColorFormIdAndType(t_name, m_pItemResult->equipments[i]->equipmentid, m_pItemResult->itemtype);

				if (m_pItemResult->equipments[i]->optionalid > 0)
				{
					addStrengtheningIcon(b_bgButton);
				}
			}
			break;
		}
	case ITEM_TYPE_DRAWING:
		{
			for (int i = 0; i < m_pItemResult->n_drawings; i++)
			{
				getItemNameAndPath(m_pItemResult->itemtype, m_pItemResult->drawings[i]->iid, itemName, itemPath);
				auto item = p_item->clone();
				item->setTag(i);
				auto b_bgButton = item->getChildByName<Button*>("button_good_bg");
				auto i_bgImage = b_bgButton->getChildByName<ImageView*>("image_item_bg_lv");
				auto i_icon = b_bgButton->getChildByName<ImageView*>("image_goods");
				auto t_name = item->getChildByName<Text*>("label_goods_name");
				auto t_num = item->getChildByName<Text*>("label_goods_num");
				i_icon->ignoreContentAdaptWithSize(false);
				i_icon->loadTexture(itemPath);
				t_name->setString(itemName);
				t_num->setString(StringUtils::format("x %d", m_pItemResult->drawings[i]->amount));
				l_item->pushBackCustomItem(item);

				setBgButtonFormIdAndType(b_bgButton, m_pItemResult->drawings[i]->iid, m_pItemResult->itemtype);
				setBgImageColorFormIdAndType(i_bgImage, m_pItemResult->drawings[i]->iid, m_pItemResult->itemtype);
				setTextColorFormIdAndType(t_name, m_pItemResult->drawings[i]->iid, m_pItemResult->itemtype);
			}
			break;
		}
	default:
		break;
	}

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_item, image_pulldown);
	this->setCameraMask(_cameraMask, true);
}
//选择物品个数界面	
void UIEmail::openCompostAmountView(const int nIndex)
{
	openView(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto i_item_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_item_bg"));
	auto i_item_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_item_bg_lv"));
	auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_item"));
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_dropitem"));
	auto t_subTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_dropitem_tiptext"));
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SEND_ITEM_AMOUNT_TITLE"]);
	t_subTitle->setString("");
	std::string name;
	std::string path;
	int amount = 0;
	int id = 0;
	switch (m_pItemResult->itemtype)
	{
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
		{
			getItemNameAndPath(m_pItemResult->itemtype,m_pItemResult->equipments[nIndex]->equipmentid,name,path);
			amount = m_pItemResult->equipments[nIndex]->amount;
			id = m_pItemResult->equipments[nIndex]->uniqueid;
			m_nItemIid = m_pItemResult->equipments[nIndex]->equipmentid;
			if (m_pItemResult->equipments[nIndex]->optionalid > 0)
			{
				addStrengtheningIcon(i_item_bg);
			}
			break;
		}
	case ITEM_TYPE_DRAWING:
		{
			getItemNameAndPath(m_pItemResult->itemtype,m_pItemResult->drawings[nIndex]->iid,name,path);
			amount = m_pItemResult->drawings[nIndex]->amount;
			id = m_pItemResult->drawings[nIndex]->uniqueid;
			m_nItemIid = m_pItemResult->drawings[nIndex]->iid;
			break;
		}
	default:
		break;
	}

	i_item->ignoreContentAdaptWithSize(false);
	i_item->loadTexture(path);
	setBgButtonFormIdAndType(i_item_bg, m_nItemIid, m_pItemResult->itemtype);
	setBgImageColorFormIdAndType(i_item_lv, m_nItemIid, m_pItemResult->itemtype);

	auto t_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_dropitem_num"));
	t_num->setString(StringUtils::format("x %d",amount));
	UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
	if (ss)
	{
		ss->removeFromParentAndCleanup(true);
	}
	auto w_slider = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_slider"));
	ss = UIVoyageSlider::create(w_slider, amount, 0, true);
	this->addChild(ss,1,100);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIEmail::ItemSilder, this));
	ss->setMaxPercent(amount);
	ss->setCurrentNumber(amount);
	view->getChildByName<Button*>("button_no")->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent,this));
	view->getChildByName<Button*>("button_yes")->addTouchEventListener(CC_CALLBACK_2(UIEmail::composeButtonEvent,this));
	view->getChildByName<Button*>("button_yes")->setTag(id);
	this->setCameraMask(_cameraMask, true);
}
//解析系统邮件的json文件
void UIEmail::getContentParseJson(std::string &content,MailDefine* &mailDefine)
{
	std::string new_value;
	std::string old_value;
	rapidjson::Document root;
	if(root.Parse<0>(mailDefine->message).HasParseError()){
		return;
	}
	if(root.IsNull())
		return;

	if (strcmp(mailDefine->title, "1") == 0 || strcmp(mailDefine->title, "2") == 0)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, "order_type"))
		{
			auto order_type = DictionaryHelper::getInstance()->getStringValue_json(root, "order_type");
			if (strcmp(order_type, "sell") == 0)
			{
				if (strcmp(mailDefine->title, "2") == 0)
				{
					content += SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_SELL_TITLE"];
					content += "\n";
					content += SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_SELL_CONTENT"];
					auto name = DictionaryHelper::getInstance()->getStringValue_json(root, "buyer");
					new_value = name;
					old_value = "[buyer]";
					repalce_all_ditinct(content, old_value, new_value);
					auto iid = DictionaryHelper::getInstance()->getIntValue_json(root, "item_id");
					auto type = DictionaryHelper::getInstance()->getIntValue_json(root, "item_type");
					std::string itemName;
					std::string itemPath;
					getItemNameAndPath(type, iid, itemName, itemPath);
					new_value = itemName;
					old_value = "[item]";
					repalce_all_ditinct(content, old_value, new_value);
					auto amount = DictionaryHelper::getInstance()->getIntValue_json(root, "amount");
					auto tax = DictionaryHelper::getInstance()->getIntValue_json(root, "tax");
					auto total = DictionaryHelper::getInstance()->getIntValue_json(root, "total");
					auto price = DictionaryHelper::getInstance()->getIntValue_json(root, "price");
					auto coin = DictionaryHelper::getInstance()->getIntValue_json(root, "get_coin");
					new_value = StringUtils::format("%d", amount);
					old_value = "[amount]";
					repalce_all_ditinct(content, old_value, new_value);
					new_value = numSegment(StringUtils::format("%d", total));
					old_value = "[total]";
					repalce_all_ditinct(content, old_value, new_value);
					new_value = numSegment(StringUtils::format("%d", coin));
					old_value = "[income]";
					repalce_all_ditinct(content, old_value, new_value);
					new_value = numSegment(StringUtils::format("%d", price));
					old_value = "[price]";
					repalce_all_ditinct(content, old_value, new_value);
					new_value = numSegment(StringUtils::format("%d", tax));
					old_value = "[tax]";
					repalce_all_ditinct(content, old_value, new_value);
				}
				else
				{
					content += SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_SELL_OUT_TITLE"];
					content += "\n";
					content += SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_SELL_OUT_CONTENT"];
					auto iid = DictionaryHelper::getInstance()->getIntValue_json(root, "item_id");
					auto type = DictionaryHelper::getInstance()->getIntValue_json(root, "item_type");
					std::string itemName;
					std::string itemPath;
					getItemNameAndPath(type, iid, itemName, itemPath);
					new_value = itemName;
					old_value = "[item]";
					repalce_all_ditinct(content, old_value, new_value);
					auto amount = DictionaryHelper::getInstance()->getIntValue_json(root, "amount");
					new_value = StringUtils::format("%d", amount);
					old_value = "[amount]";
					repalce_all_ditinct(content, old_value, new_value);
					std::string position;
					//V票不是放在背包里
					if (iid !=10000)
					{
						systemItemTalkAllPosition(position, type);
					}				
					content += position;
				}

			}
			else
			{
				if (strcmp(mailDefine->title, "2") == 0)
				{
					content += SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_BUY_TITLE"];
					content += "\n";
					content += SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_BUY_CONTENT"];
					auto name = DictionaryHelper::getInstance()->getStringValue_json(root, "seller");
					new_value = name;
					old_value = "[seller]";
					repalce_all_ditinct(content, old_value, new_value);
					auto iid = DictionaryHelper::getInstance()->getIntValue_json(root, "item_id");
					auto type = DictionaryHelper::getInstance()->getIntValue_json(root, "item_type");
					std::string itemName;
					std::string itemPath;
					getItemNameAndPath(type, iid, itemName, itemPath);
					new_value = itemName;
					old_value = "[item]";
					repalce_all_ditinct(content, old_value, new_value);
					auto amount = DictionaryHelper::getInstance()->getIntValue_json(root, "amount");
					auto tax = DictionaryHelper::getInstance()->getIntValue_json(root, "tax");
					auto total = DictionaryHelper::getInstance()->getIntValue_json(root, "total");
					auto price = DictionaryHelper::getInstance()->getIntValue_json(root, "price");
					new_value = StringUtils::format("%d", amount);
					old_value = "[amount]";
					repalce_all_ditinct(content, old_value, new_value);
					new_value = StringUtils::format("%d", total);
					old_value = "[total]";
					repalce_all_ditinct(content, old_value, new_value);
					new_value = StringUtils::format("%d", price);
					old_value = "[price]";
					repalce_all_ditinct(content, old_value, new_value);
					new_value = StringUtils::format("%d", tax);
					old_value = "[tax]";
					repalce_all_ditinct(content, old_value, new_value);
					std::string position;
					//V票不是放在背包里
					if (iid!=10000)
					{
						systemItemTalkAllPosition(position, type);
					}			
					content += position;
				}
				else
				{
					content += SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_BUY_OUT_TITLE"];
					content += "\n";
					content += SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_BUY_OUT_CONTENT"];
					auto iid = DictionaryHelper::getInstance()->getIntValue_json(root, "item_id");
					auto type = DictionaryHelper::getInstance()->getIntValue_json(root, "item_type");
					std::string itemName;
					std::string itemPath;
					getItemNameAndPath(type, iid, itemName, itemPath);
					new_value = itemName;
					old_value = "[item]";
					repalce_all_ditinct(content, old_value, new_value);
					auto amount = DictionaryHelper::getInstance()->getIntValue_json(root, "amount");
					new_value = StringUtils::format("%d", amount);
					old_value = "[amount]";
					repalce_all_ditinct(content, old_value, new_value);
				}
			}
			return;
		}
	}
	else if (strcmp(mailDefine->title, "3") == 0)
	{
		auto name = DictionaryHelper::getInstance()->getStringValue_json(root, "name");
		content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_CREATE_GUILD_TITLE"];
		content += "\n";
		content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_CREATE_GUILD_CONTENT"];
		new_value = name;
		old_value = "[name]";
		repalce_all_ditinct(content, old_value, new_value);
		return;
	}
	else if (strcmp(mailDefine->title, "4") == 0 || strcmp(mailDefine->title, "5") == 0 || strcmp(mailDefine->title, "6") == 0)
	{
		auto id = DictionaryHelper::getInstance()->getIntValue_json(root, "city_id");
		content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_CITY"];
		new_value = SINGLE_SHOP->getCitiesInfo()[id].name;
		old_value = "[city]";
		repalce_all_ditinct(content, old_value, new_value);
		return;
	}
	else if (strcmp(mailDefine->title, "7") == 0)
	{
		auto coins = DictionaryHelper::getInstance()->getIntValue_json(root, "coins");
		content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_RESCUE_EMAIL_CONTENT"];
		std::string old_value1 = "[num1]";
		std::string new_value1 =numSegment(StringUtils::format("%d", coins));
		repalce_all_ditinct(content, old_value1, new_value1);
		return;
	}
	else if (strcmp(mailDefine->title, "8") == 0)
	{
		auto coins = DictionaryHelper::getInstance()->getIntValue_json(root, "coins");
		auto days = DictionaryHelper::getInstance()->getIntValue_json(root, "days");
		auto item_id = DictionaryHelper::getInstance()->getIntValue_json(root, "item_id");
		auto item_num = DictionaryHelper::getInstance()->getIntValue_json(root, "item_num");
		auto ship_id = DictionaryHelper::getInstance()->getIntValue_json(root, "ship_id");
		auto ship_num = DictionaryHelper::getInstance()->getIntValue_json(root, "ship_num");
		auto gold = DictionaryHelper::getInstance()->getIntValue_json(root, "gold");
		content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_LOGIN_REWARD_CONTENT1"];
		std::string old_value1 = "[num1]";
		std::string old_value2 = "[num2]";
		std::string old_value3 = "[num3]";
		std::string old_value4 = "[num4]";
		std::string old_value5 = "[num5]";
		std::string new_value1 = numSegment(StringUtils::format("%d", coins));
		std::string new_value2 = StringUtils::format("%d", days);
		std::string new_value3 = SINGLE_SHOP->getItemData()[item_id].name;
		std::string new_value4 = StringUtils::format("%d", item_num);
		std::string new_value5 = SINGLE_SHOP->getShipData()[ship_id].name;
		std::string new_value6 = StringUtils::format("%d", ship_num);
		std::string new_value7 = numSegment(StringUtils::format("%d", gold));
		repalce_all_ditinct(content, old_value2, new_value2);
		if (coins > 0)
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_LOGIN_REWARD_CONTENT2"];
			content += ",";
			repalce_all_ditinct(content, old_value1, new_value1);
		}

		if (gold > 0)
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_LOGIN_REWARD_CONTENT3"];
			content += ",";
			repalce_all_ditinct(content, old_value5, new_value7);
		}

		if (item_id > 0)
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_LOGIN_REWARD_CONTENT4"];
			content += ",";
			repalce_all_ditinct(content, old_value3, new_value3);
			repalce_all_ditinct(content, old_value4, new_value4);
		}
		if (ship_id > 0)
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_LOGIN_REWARD_CONTENT4"];
			content += ",";
			repalce_all_ditinct(content, old_value3, new_value5);
			repalce_all_ditinct(content, old_value4, new_value6);
		}
		content.erase(content.end() - 1);
		content += ".";
		return;
		
	}
	else if (strcmp(mailDefine->title, "9") == 0)
	{
		auto type = DictionaryHelper::getInstance()->getIntValue_json(root, "type");
		auto id = DictionaryHelper::getInstance()->getIntValue_json(root, "id");
		auto city_id = DictionaryHelper::getInstance()->getIntValue_json(root, "city_id");
		content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BULID_SHIP_FINISH_CONTENT"];
		if (type == 1)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BULID_EQUIP_FINISH_CONTENT"];
		}
		std::string old_value1 = "[num1]";
		std::string new_value1 = SINGLE_SHOP->getCitiesInfo()[city_id].name;
		std::string old_value2 = "[num2]";
		std::string new_value2 = SINGLE_SHOP->getShipData()[id].name;
		if (type == 1)
		{
			new_value2 = SINGLE_SHOP->getItemData()[id].name;
		}
		repalce_all_ditinct(content, old_value1, new_value1);
		repalce_all_ditinct(content, old_value2, new_value2);
		return;
	}
	else if (strcmp(mailDefine->title, "10") == 0)
	{
		auto name = DictionaryHelper::getInstance()->getStringValue_json(root, "name");
		auto result = DictionaryHelper::getInstance()->getIntValue_json(root, "result");

		if (result == 1)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_GUILD_AGREE_APPLICATION"];
		}
		else
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_GUILD_REFUSE_APPLICATION"];
		}
		
		old_value = "[guildname]";
		new_value = name;
		repalce_all_ditinct(content, old_value, new_value);
		return;	
	}
	else if (strcmp(mailDefine->title, "12") == 0 || strcmp(mailDefine->title, "13") == 0)
	{
		auto name = DictionaryHelper::getInstance()->getStringValue_json(root, "sender_name");
		auto sender_cid = DictionaryHelper::getInstance()->getIntValue_json(root, "sender_cid");
		if (strcmp(mailDefine->title, "12") == 0)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_RECEIVE_GIFT_CONTENT"];
		}
		if (strcmp(mailDefine->title, "13") == 0)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_GIFT_GIVEN_BE_TAKE_AWAY_CONTENT"];
		}
		new_value = name;
		old_value = "[FriendName]";
		repalce_all_ditinct(content, old_value, new_value);
		return;
	}
	else if (strcmp(mailDefine->title, "14") == 0)
	{
		auto type = DictionaryHelper::getInstance()->getIntValue_json(root, "type");
		m_nAgainstPirateRewardCoins = DictionaryHelper::getInstance()->getIntValue_json(root, "coins");
		m_nAgainstPirateRewardGolds = DictionaryHelper::getInstance()->getIntValue_json(root, "golds");
		m_nAgainstPirateRewardSailorCoins = DictionaryHelper::getInstance()->getIntValue_json(root, "sailor_coins");
		auto rank = DictionaryHelper::getInstance()->getIntValue_json(root, "rank");
	
		switch (type)
		{
		case 1:
			//排名奖励
			new_value = StringUtils::format("%d", rank);
			old_value = "[ranking]";
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_AGAINST_PIRATE_BASES_REWARD_CONTENT_RANKING"];
			repalce_all_ditinct(content, old_value, new_value);
			break;
		case 2:
			//补刀者奖励
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_AGAINST_PIRATE_BASES_REWARD_CONTENT_LAST_HIT"];
			break;
		case 3:
			//胜利奖励
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_AGAINST_PIRATE_BASES_REWARD_CONTENT_WIN"];
			break;
		case 4:
			//失败奖励
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_AGAINST_PIRATE_BASES_REWARD_CONTENT_FAIL"];
			break;
		default:
			break;
		}
		return;
	}
	else if (strcmp(mailDefine->title, "15") == 0)
	{
		auto guild_name = DictionaryHelper::getInstance()->getStringValue_json(root, "guild_name");
		auto type = DictionaryHelper::getInstance()->getIntValue_json(root, "type");

		switch (type)
		{
		case 1:
			//被公会踢出
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_BE_KICKED_OUT_OF_GUILD"];
			break;
		case 2:
			//被任命为公会管理员
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_BE_ADMINISTRATOR_OF_GUILD"];
			break;
		case 3:
			//被取消管理员资格
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_SYSTEM_BE_CANCELED_ADMINISTRATOR_OF_GUILD"];
			break;
		default:
			break;
		}
		old_value = "[guildname]";
		new_value = guild_name;
		repalce_all_ditinct(content, old_value, new_value);
		return;
	}
	else if (strcmp(mailDefine->title, "17") == 0)
	{
		auto levelNum = DictionaryHelper::getInstance()->getIntValue_json(root, "level");
		content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_LEVEL_UP_REWARD_CONTENT"];
		old_value = "[levelnum]";
		new_value = StringUtils::format("%d", levelNum); 
		repalce_all_ditinct(content, old_value, new_value);
	}
	else if (strcmp(mailDefine->title, "18") == 0)
	{
		auto player = DictionaryHelper::getInstance()->getStringValue_json(root, "player");
		if(player){
			content = SINGLE_SHOP->getTipsInfo()["TIP_EAMIL_LOTTO_NOTICE_CONTENT"];
			old_value = "[player]";
			new_value = player;
			repalce_all_ditinct(content, old_value, new_value);
			auto count = DictionaryHelper::getInstance()->getArrayCount_json(root, "reward");
			for (size_t i = 0; i < count; i++)
			{
				auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root, "reward", i);
				auto lang = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "lang");
				if (lang == Utils::getLanguage())
				{
					auto reward_text = DictionaryHelper::getInstance()->getStringValue_json(items_counts, "reward_text");
					old_value = "[lotto_reward]";
					new_value = reward_text;
					repalce_all_ditinct(content, old_value, new_value);
					break;
				}
			}
		}
	}
	else if (strcmp(mailDefine->title, "19") == 0)//邀请好友
	{
		content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_FACEBOOK_INVITE_OTHERS_CONTENT"];
		auto invited_name = DictionaryHelper::getInstance()->getStringValue_json(root, "invited_name:");
		if (invited_name)
		{
			old_value = "[player]";
			new_value = invited_name;
			repalce_all_ditinct(content, old_value, new_value);
		}
	}
	else if (strcmp(mailDefine->title, "20") == 0)
	{
		auto gold = DictionaryHelper::getInstance()->getIntValue_json(root, "gold");
		content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_V_ACTIVITY_BONUS_CONTENT"];
		std::string old_value1 = "[num]";
		std::string new_value1 = numSegment(StringUtils::format("%d", gold));
		repalce_all_ditinct(content, old_value1, new_value1);
		return;
	}
	else if (strcmp(mailDefine->title, "21") == 0)
	{
		auto name = DictionaryHelper::getInstance()->getStringValue_json(root, "name");
		auto level = DictionaryHelper::getInstance()->getIntValue_json(root, "level");

		content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_BOSS_DEFEATED_CONTENT"];
		std::string old_value1 = "[name]";
		std::string new_value1 = name;
		repalce_all_ditinct(content, old_value1, new_value1);
		old_value1 = "[level]";
		new_value1 = StringUtils::format("(Lv. %d -- Lv. %d)", level - 9, level);
		repalce_all_ditinct(content, old_value1, new_value1);
	}
	else if (strcmp(mailDefine->title, "22") == 0)
	{
		auto cityId = DictionaryHelper::getInstance()->getIntValue_json(root, "city_id");
		auto rob_coins = DictionaryHelper::getInstance()->getIntValue_json(root, "rob_coins");

		content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_CITY_FALLEN_HAS_MONEY_CONTENT"];
		old_value = "[cityname]";
		new_value = SINGLE_SHOP->getCitiesInfo()[cityId].name;
		repalce_all_ditinct(content, old_value, new_value);
		old_value = "[coinnums]";
		new_value = StringUtils::format("%d", rob_coins);
		repalce_all_ditinct(content, old_value, new_value);
	}
	else if (strcmp(mailDefine->title, "23") == 0)
	{
		auto cityId = DictionaryHelper::getInstance()->getIntValue_json(root, "city_id");
		content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_CITY_FALLEN_NO_MONEY_CONTENT"];
		old_value = "[cityname]";
		new_value = SINGLE_SHOP->getCitiesInfo()[cityId].name;;
		repalce_all_ditinct(content, old_value, new_value);
		return;
	}
	else if (strcmp(mailDefine->title, "24") == 0 || strcmp(mailDefine->title, "26") == 0 || strcmp(mailDefine->title, "27") == 0)
	{
		auto nation_a = DictionaryHelper::getInstance()->getIntValue_json(root, "nation_a");
		auto nation_b = DictionaryHelper::getInstance()->getIntValue_json(root, "nation_b");

		if (strcmp(mailDefine->title, "24") == 0)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_COUNTRY_WAR_END_CONTENT"];
		}
		else if (strcmp(mailDefine->title, "26") == 0)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_COUNTRY_WAR_START_CONTENT"];
		}
		else if (strcmp(mailDefine->title, "27") == 0)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_COUNTRY_WAR_ING_CONTENT"];
		}
		
		old_value = "[nationname1]";
		new_value = SINGLE_SHOP->getNationInfo()[nation_a];
		repalce_all_ditinct(content, old_value, new_value);
		old_value = "[nationname2]";
		new_value = SINGLE_SHOP->getNationInfo()[nation_b];
		repalce_all_ditinct(content, old_value, new_value);
	}
	else if (strcmp(mailDefine->title, "25") == 0)
	{
		auto golds = DictionaryHelper::getInstance()->getIntValue_json(root, "golds");
		auto sailor_coins = DictionaryHelper::getInstance()->getIntValue_json(root, "sailor_coins");
		auto fame = DictionaryHelper::getInstance()->getIntValue_json(root, "fame");

		content = SINGLE_SHOP->getTipsInfo()["TIP_SYSTEM_COUNTRY_WAR_RANKING_REWARD_CONTENT"];
		old_value = "[goldsNum]";
		new_value = StringUtils::format("%d", golds);
		repalce_all_ditinct(content, old_value, new_value);
		old_value = "[sailorCoinsNum]";
		new_value = StringUtils::format("%d", sailor_coins);
		repalce_all_ditinct(content, old_value, new_value);
		old_value = "[fameNum]";
		new_value = StringUtils::format("%d", fame);
		repalce_all_ditinct(content, old_value, new_value);
	}
	else 
	{

	}
}
//系统邮件取出说明
void UIEmail::systemItemTalkAllPosition(std::string &content,int type)
{
	content += "(";
	switch (type)
	{
	case 0:
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_TAKEALL_GOODS"];
			break;
		}
	case 1:
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_TAKEALL_SHIPS"];
			break;
		}
	default:
		{
			content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_TAKEALL_GOODS"];
			//content += SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_TAKEALL_OTHER"];
			break;
		}
	}
	content += ")";
}
//发送邮件
void UIEmail::sendMail()
{
	TextField* t_to = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_name"));
	TextField* t_title = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_subject"));
	TextField* t_content = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_content"));
	ui::EditBox* e_to = dynamic_cast<ui::EditBox*>(t_to->getParent()->getChildByTag(1));
	ui::EditBox* e_title = dynamic_cast<ui::EditBox*>(t_title->getParent()->getChildByTag(2));
	ui::EditBox* e_content = dynamic_cast<ui::EditBox*>(t_content->getParent()->getChildByTag(3));

	std::string s_to = e_to->getText();
	std::string s_title = e_title->getText();
	std::string s_content = e_content->getText();

	if (s_to == "")
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_EMAIL_SEND_TO_NULL");
		return;
	}
	else if (s_to == SINGLE_HERO->m_sName)
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_EMAIL_SEND_TO_MYSELF");
		return;
	}
	if (s_title == "")
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_EMAIL_SEND_TITLE_NULL");
		return;
	}
	auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, "label_silver_num"));
	auto i_goodsNum = atoll(m_padString.c_str());
	//当等级小于10级时，限制发送银币
	if (i_goodsNum>0)
	{
		if (SINGLE_HERO->m_iLevel<10)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EMAIL_SEND_COIN_THE_LOW_LEVEL");
			m_padString.clear();
			label_silver_num->setString("0");
			return;
		}
	}
	ProtocolThread::GetInstance()->sendMail((char*)s_to.c_str(),(char*)s_title.c_str(),(char*)s_content.c_str(),m_nItemId,m_nItemNum,m_nItemType,0,i_goodsNum,UILoadingIndicator::createWithMask(this,_cameraMask));
}
//个数滑动条实现   
void UIEmail::ItemSilder(const int nNum)
{
	auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto t_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_dropitem_num"));
	UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
	ss->setCurrentNumber(nNum);
	t_num->setString(StringUtils::format("%d/%d", nNum, ss->getMaxPercent()));
}
//向name发送邮件
void UIEmail::sendEmailToPlayer(std::string name, std::string peername)
{
	//删除ShipSlider
	this->removeChildByTag(100); 
	this->removeFromParentAndCleanup(true);
	auto currentScene = Director::getInstance()->getRunningScene();
	currentScene->addChild(this,10);
	if (this->getChildByTag(INFORMVIEWTAG))
	{
		auto view = this->getChildByTag(INFORMVIEWTAG);
		dynamic_cast<UIInform*>(view)->closeAutoInform(0);
	}

	registerCallBack();
	openComposeView();
	TextField* t_name = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_player_name"));
	t_name->setVisible(false);
	ui::EditBox* edit_name = dynamic_cast<ui::EditBox*>(t_name->getParent()->getChildByTag(1));
	edit_name->setText(name.c_str());
	if (strcmp(peername.data(), "") != 0)
	{
		auto t_subject = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_pRoot, "textfield_subject"));
		t_subject->setVisible(false);
		ui::EditBox* edit_subject = dynamic_cast<ui::EditBox*>(t_subject->getParent()->getChildByTag(2));
		std::string replyText = SINGLE_SHOP->getTipsInfo()["TIP_MAIL_REPLY_TITLE"].c_str();
		std::string old_value = "[replytitle]";
		std::string new_value = peername;
		repalce_all_ditinct(replyText, old_value, new_value);
		edit_subject->setText(replyText.c_str());
	}
	dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot,"button_choose"))->setVisible(false);
}
//打开邮件界面
void UIEmail::openEmailView(Node* parent, int CameraMask)
{
	//删除ShipSlider
	this->removeChildByTag(100); 
	this->removeFromParentAndCleanup(true);
	if (parent)
	{
		parent->addChild(this,10);
	}else
	{
		auto currentScene = Director::getInstance()->getRunningScene();
		currentScene->addChild(this,10);
	}
	this->setCameraMask((unsigned short)CameraMask,true);
	
	registerCallBack();
	ProtocolThread::GetInstance()->getMailList(UILoadingIndicator::createWithMask(this,_cameraMask));
}
//更改左侧按键的状态
void UIEmail::changeMinorButtonState(Widget *target)
{
	if (!target)
	{
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
		target = view->getChildByName<Widget*>("button_ship_equipment");
		auto l_butter = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_two_butter"));
		l_butter->removeItem(14);
		l_butter->removeItem(13);
		l_butter->removeItem(4);
		l_butter->removeItem(3);
		l_butter->removeItem(2);
		l_butter->removeItem(1);
	}

	if (m_pTempButton)
	{
		m_pTempButton->setBright(true);
		m_pTempButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pTempButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_pTempButton = target;
	m_pTempButton->setBright(false);
	m_pTempButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pTempButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
}
//判断输入框是否为输入状态并修改输入框透明度;
void UIEmail::update(float dt)
{
	auto view = getViewRoot(MAIL_COCOS_RES[MAIL_WRITE_CSB]);
	
	if (view)
	{
		TextField* t_to = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_player_name"));
		TextField* t_title = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_subject"));
		TextField* t_content = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_content"));
		t_to->setVisible(false);
		t_title->setVisible(false);
		t_content->setVisible(false);
		ui::EditBox* e_to = dynamic_cast<ui::EditBox*>(t_to->getParent()->getChildByTag(1));
		ui::EditBox* e_title = dynamic_cast<ui::EditBox*>(t_title->getParent()->getChildByTag(2));
		ui::EditBox* e_content = dynamic_cast<ui::EditBox*>(t_content->getParent()->getChildByTag(3));

		std::string name = e_to->getText();
		std::string subject = e_title->getText();
		std::string content = e_content->getText();
		auto t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_silver_num"));
		setTextFontSize(t_silver);
		if (name.size()> 0)
		{
			e_to->setOpacity(255);
		}
		else
		{
			e_to->setOpacity(127);
		}

		if (subject.size()> 0)
		{
			e_title->setOpacity(255);
		}
		else
		{
			e_title->setOpacity(127);
		}

		if (content.size()> 0)
		{
			e_content->setOpacity(255);
		}
		else
		{
			e_content->setOpacity(127);
		}

		if (t_silver->getString().compare("0") == 0)
		{
			t_silver->setOpacity(127);
		}
		else
		{
			t_silver->setOpacity(255);
		}

		if (illegal_character_check(name))
		{
			e_to->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
		if (illegal_character_check(subject))
		{
			e_title->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
		if (illegal_character_check(content))
		{
			e_content->setText("");
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		}
	}
}
