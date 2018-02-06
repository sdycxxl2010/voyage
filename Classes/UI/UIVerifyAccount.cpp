#include "UIVerifyAccount.h"
#include "TVSceneLoader.h"
#include "UIInform.h"
#include "UIMain.h"

UIVerifyAccount::UIVerifyAccount()
{
	m_pEmailContent = nullptr;
	m_pGetEmailVerifyInfo = nullptr;
}


UIVerifyAccount::~UIVerifyAccount()
{
	m_pEmailContent = nullptr;
	m_pGetEmailVerifyInfo = nullptr;
}
UIVerifyAccount*UIVerifyAccount::createVertifyLayer()
{
	UIVerifyAccount*verify = new UIVerifyAccount;
	CC_RETURN_IF(verify);
	if (verify->init())
	{
		verify->autorelease();
		return verify;
	}
	CC_SAFE_RELEASE(verify);
	return nullptr;
}
bool UIVerifyAccount::init()
{
	bool pRet = false;
	do
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIVerifyAccount::onServerEvent, this), this);
		ProtocolThread::GetInstance()->getEmailVerifiedInfo(UILoadingIndicator::create(this));
		pRet = true;

	} while (0);
	return pRet;

}
void UIVerifyAccount::initStaticData(float f)
{
	openView(VERIFIY_ACCOUNT_RES[VIEW_VERIFIY_ACCOUNT]);
	auto view = getViewRoot(VERIFIY_ACCOUNT_RES[VIEW_VERIFIY_ACCOUNT]);
	//未验证的panel
	auto panel_nopass = view->getChildByName<Widget*>("panel_nopass");
	panel_nopass->setVisible(true);
	auto i_content_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_nopass, "label_vertify_content_1"));
	i_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_VERTIFY_NO_PASS_CONTENT_1"]);
	auto i_content_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_nopass, "label_vertify_content_2"));
	i_content_2->setString(SINGLE_SHOP->getTipsInfo()["TIP_VERTIFY_NO_PASS_CONTENT_2"]);
	auto i_reward_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_nopass, "label_reward_num"));
	i_reward_num->setString(StringUtils::format("x %lld", m_pGetEmailVerifyInfo->addgolds));
	//已验证过panel 
	auto panel_pass = view->getChildByName<Widget*>("panel_pass");
	panel_pass->setVisible(false);
	auto i_content_get = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_pass, "label_content_1"));
	i_content_get->setString(SINGLE_SHOP->getTipsInfo()["TIP_VERTIFY_PASS_GET_REWARD_CONTENT"]);
	auto i_verify_reward = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_pass, "label_vertify_num"));
	i_verify_reward->setString(StringUtils::format("x %d", m_pGetEmailVerifyInfo->addgolds));

	if (m_pGetEmailVerifyInfo->authenticated)
	{
		panel_pass->setVisible(true);
		panel_nopass->setVisible(false);
	}
	else
	{
		panel_pass->setVisible(false);
		panel_nopass->setVisible(true);
	}
}
void UIVerifyAccount::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//关闭
	if (isButton(button_verfic_close))
	{
		closeView();
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		return;
	}
	//领取奖励
	if (isButton(button_verify_reward))
	{
		ProtocolThread::GetInstance()->getEmailVerifiedReward(UILoadingIndicator::create(this));
		closeView(VERIFIY_ACCOUNT_RES[VIEW_VERIFIY_ACCOUNT]);
			
		return;
	}
	//打开账户
	if (isButton(button_verfi_your_account))
	{
		openResetEmail();
		return;
	}
	//确定修改账户
	if (isButton(button_verific_yes))
	{

		std::string emailContent = m_pEmailContent->getString();
		char*email = (char*)(emailContent.c_str());
		ProtocolThread::GetInstance()->authenticateAccount(email,0, UILoadingIndicator::create(this));
		closeView();
		closeView(VERIFIY_ACCOUNT_RES[VIEW_VERIFIY_ACCOUNT]);
		return;
	}
	if (isButton(button_verific_no))
	{
		closeView();
		return;
	}
}
void UIVerifyAccount::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetEmailVerifiedInfoResult:
	{
		GetEmailVerifiedInfoResult*result = (GetEmailVerifiedInfoResult*)message;
		m_pGetEmailVerifyInfo = result;
		if (result->failed == 0)
		{
			initStaticData(0);
		}
		break;
	}
	case PROTO_TYPE_GetEmailVerifiedRewardResult:
	{
			GetEmailVerifiedRewardResult*result = (GetEmailVerifiedRewardResult*)message;
			if (result->failed == 0)
			{
				auto currentScene = Director::getInstance()->getRunningScene();
				auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
				if (mainlayer)
				{
					mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin,result->golds);
					mainlayer->showVerify(false);

				}
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_VERTIFY_ACCOUNT_TO_GETREWARD");
				
			}
			else if (result->failed == 2)
			{
				openSuccessOrFailDialog("TIP_VERTIFY_ACCOUNT_HAVE_GETREWARD");
			}
			else
			{
				openSuccessOrFailDialog("TIP_CENTER_OPERATE_FAIL");
			}
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			break;
	}
	case PROTO_TYPE_AuthenticateAccountResult:
	{
		 AuthenticateAccountResult*result = (AuthenticateAccountResult*)message;
		 if (result->forsecurity==1)
		 {
			 return;
		 }
			if (result->failed == 0)
			{
				if (result->newemail)
				{
					ProtocolThread::GetInstance()->saveEmail(result->newemail);
					UserDefault::getInstance()->flush();
				}
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_SUCEESS");
			}
			else if (result->failed == 1)
			{
				//常规错误
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_FAIL");

			}
			else if (result->failed == 2)
			{
				//格式不对
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_FORMAT_WRONG");
			}
			else if (result->failed == 3)
			{
				//已经认证过
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_HAVE_VERTIFIED");
			}
			else if (result->failed == 4)
			{
				//别人注册过
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_VERIFY_ACCOUNT_OTHER_HAVE_SIGN");
			}
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			break;
	}
	default:
	break;
	}
}
void UIVerifyAccount::openResetEmail()
{
	openView(SETTING_COCOS_RES[VIEW_ACCOUNT_EMAIL]);
	auto viewEmail = getViewRoot(SETTING_COCOS_RES[VIEW_ACCOUNT_EMAIL]);
	auto i_no = dynamic_cast<Button*>(Helper::seekWidgetByName(viewEmail, "button_verific_no"));

	auto i_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(viewEmail, "button_verific_yes"));
	auto i_account_tip = dynamic_cast<Text*>(Helper::seekWidgetByName(viewEmail, "label_account_tip"));
	i_account_tip->setString(SINGLE_SHOP->getTipsInfo()["TIP_VERIFY_ACCOUNT_EMAIL"]);

	auto i_textfield_account = dynamic_cast<TextField*>(Helper::seekWidgetByName(viewEmail, "textfield_email_account"));
	m_pEmailContent = i_textfield_account;
	i_textfield_account->setTextVerticalAlignment(TextVAlignment::CENTER);
	i_textfield_account->setString(ProtocolThread::GetInstance()->getSavedEmail());
}
