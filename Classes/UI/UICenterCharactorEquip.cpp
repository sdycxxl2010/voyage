#include "UICenterCharactorEquip.h"
#include "UIInform.h"
#include "UICommon.h"
#include "ModeLayerManger.h"
#include "UIModelLayer.h"
UICenterCharactorEquip::UICenterCharactorEquip():
	m_pParent(nullptr),
	m_nType(-1),
	m_pTempButton(nullptr),
	m_bFinishData(true),
	m_alreadyAdd(false),
	_radius(1.0f),
	_distanceZ(1.0f),
	_center(Vec3(0, 0, 0))
{
	for (int i = 0; i < 5; i++)
	{
		m_gCurEquipId[i] = 0;
		m_gCurEquipIid[i] = 0;
		m_gIsFulshEquip[i] = true;
	}
	m_eConfirmIndex = CONFIRM_INDEX_SAVE_DATA;
	m_vAllEquip->clear();
	m_character = nullptr;
	m_viewCamera = nullptr;
	m_characterLayer = nullptr;
	m_touchBegan = Vec2(0, 0);
	for (int i = 0; i < 3;i++)
	{
		m_characterInfo[i] = 0;
	}
	//帽子
	m_manmesh.push_back("man_hats_M_100_1");
	//衣服
	m_manmesh.push_back("man_frock_M_100_1");
	m_manmesh.push_back("man_pants_M_100_1");
	//项链
	m_manmesh.push_back("man_necklace_M_100_1");
	//鞋子
	m_manmesh.push_back("man_shoe_M_100_1");

	//帽子
	m_womanmesh.push_back("woman_hats_M_100_1");
	//衣服
	m_womanmesh.push_back("woman_frock_M_100_1");
	m_womanmesh.push_back("woman_pants_M_100_1");
	//项链
	m_womanmesh.push_back("woman_necklace_M_100_1");
	//鞋子
	m_womanmesh.push_back("woman_shoe_M_100_1");
	//手套
	m_womanmesh.push_back("woman_gloves_M_100_1");
	
}

UICenterCharactorEquip::~UICenterCharactorEquip()
{

}

UICenterCharactorEquip* UICenterCharactorEquip::createEquip(UIBasicLayer *parent)
{
	UICenterCharactorEquip *ce = new UICenterCharactorEquip;
	if (ce && ce->init())
	{
		ce->m_pParent = parent;
		ce->autorelease();
		return ce;
	}
	CC_SAFE_DELETE(ce);
	return nullptr;
}

void UICenterCharactorEquip::openCenterEquip()
{
	ProtocolThread::GetInstance()->getHeroEquip(0,UILoadingIndicator::create(m_pParent));
}
void UICenterCharactorEquip::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		std::string name = target->getName();
		buttonEvent(target,name);
	}
}

void UICenterCharactorEquip::buttonEvent(Widget *pSender,std::string name)
{
	auto button = (Button*)pSender;
	//装备--帽子
	if (isButton(button_maozi))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nType = 1;
		if (m_gIsFulshEquip[m_nType - 1])
		{
			ProtocolThread::GetInstance()->getEquipableItems(m_nType, 1, UILoadingIndicator::createWithMask(m_characterLayer,4));
		}else
		{
			flushEquipinfo();
		}
		return;
	}
	//装备--衣服
	if (isButton(button_yifu))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nType = 2;
		if (m_gIsFulshEquip[m_nType - 1])
		{
			ProtocolThread::GetInstance()->getEquipableItems(m_nType, 1, UILoadingIndicator::createWithMask(m_characterLayer, 4));
		}else
		{
			flushEquipinfo();
		}
		return;
	}
	//装备--鞋子
	if (isButton(button_xiezi))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nType = 5;
		if (m_gIsFulshEquip[m_nType - 1])
		{
			ProtocolThread::GetInstance()->getEquipableItems(m_nType, 1, UILoadingIndicator::createWithMask(m_characterLayer, 4));
		}else
		{
			flushEquipinfo();
		}
		return;
	}
	//装备--武器
	if (isButton(button_wuqi))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nType = 3;
		if (m_gIsFulshEquip[m_nType - 1])
		{
			ProtocolThread::GetInstance()->getEquipableItems(m_nType, 1, UILoadingIndicator::createWithMask(m_characterLayer, 4));
		}else
		{
			flushEquipinfo();
		}
		return;
	}
	//装备--首饰
	if (isButton(button_shoushi))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nType = 4;
		if (m_gIsFulshEquip[m_nType - 1])
		{
			ProtocolThread::GetInstance()->getEquipableItems(m_nType, 1, UILoadingIndicator::createWithMask(m_characterLayer, 4));
		}else
		{
			flushEquipinfo();
		}
		return;
	}
	/*
	if (isButton(button_finish))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_eConfirmIndex = CONFIRM_INDEX_SAVE_DATA;
		InformView::getInstance()->openInformView(m_pParent);
		InformView::getInstance()->openConfirmYesOrNO("TIP_CENTER_ROLL_EQUIP_SAVE_DATA_TITLE","TIP_CENTER_ROLL_EQUIP_SAVE_DATA");
		return;
	}
	*/
	//装备--卸载
	if (isButton(button_uninstall))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_eConfirmIndex = CONFIRM_INDEX_EQUIP;
		UIInform::getInstance()->openInformView(m_pParent);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_FLEET_CONFIRM_SAVE_TITLE","TIP_CENTER_ROLL_EQUIP_UNINSTALL");
		return;
	}

	if (isButton(button_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		int tempIid = m_gCurEquipIid[m_nType - 1];
		int tempId = m_gCurEquipId[m_nType - 1];
		m_gCurEquipIid[m_nType - 1] = m_pTempButton->getTag();
		m_gCurEquipId[m_nType - 1] = m_pTempButton->getParent()->getTag();

		bool isfind = false;
		for (auto iter = m_vAllEquip[m_nType - 1].begin(); iter != m_vAllEquip[m_nType - 1].end(); iter++)
		{
			if (tempId && tempId == iter->first)
			{
				isfind = true;
				iter->second.amount++;
				break;
			}
		}

		if (!isfind && tempId != 0)
		{
			ROLE_EQUIP item;
			item.iid = tempIid;
			item.amount = 1;
			m_vAllEquip[m_nType - 1].insert(std::pair<int, ROLE_EQUIP>(tempId, item));
		}

		for (auto iter = m_vAllEquip[m_nType - 1].begin(); iter != m_vAllEquip[m_nType - 1].end(); iter++)
		{
			if (m_gCurEquipId[m_nType - 1] = iter->first)
			{
				if (iter->second.amount <= 1)
				{
					m_vAllEquip[m_nType - 1].erase(iter);
					break;
				}
			}
		}
		
		flushEquipButton();
		m_pTempButton = nullptr;
		m_bFinishData = false;
		m_pParent->closeView();
		saveEquipHero();
		return;
	}
	//取消按钮
	if (isButton(button_cancel)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pParent->closeView();
		m_pTempButton = nullptr;
		auto instance = ModeLayerManger::getInstance();
		auto layer = instance->getModelLayer();
		if (layer != nullptr)
		{
			instance->showModelLayer(true);
			auto type = instance->getNeedShowLayerType();
			if (type != -1)
			{
				instance->operaModels(type);
			}
		}
		
		return;
	}

	if (isButton(button_equip_items))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pTempButton)
		{
			m_pTempButton->setBright(true);
		}
		m_pTempButton = button;
		m_pTempButton->setBright(false);
		auto p_addequip = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CHANGE_CSB]);
		auto b_ok = p_addequip->getChildByName<Button*>("button_ok");
		b_ok->setBright(true);
		b_ok->setTouchEnabled(true);
		updataSelectEquip(m_pTempButton->getTag());
		return;
	}
	//确认按钮
	if (isButton(button_confirm_yes))
	{
		if (m_eConfirmIndex == CONFIRM_INDEX_EQUIP)
		{
			int tempIid = m_gCurEquipIid[m_nType - 1];
			int tempId = m_gCurEquipId[m_nType - 1];
			m_gCurEquipIid[m_nType - 1] = 0;
			m_gCurEquipId[m_nType - 1] = 0;

			bool isfind = false;
			for (auto iter = m_vAllEquip[m_nType - 1].begin(); iter != m_vAllEquip[m_nType - 1].end(); iter++)
			{
				if (tempId && tempId == iter->first)
				{
					isfind = true;
					iter->second.amount++;
					break;
				}
			}
			if (!isfind && tempId != 0)
			{
				ROLE_EQUIP item;
				item.iid = tempIid;
				item.amount = 1;
				m_vAllEquip[m_nType - 1].insert(std::pair<int, ROLE_EQUIP>(tempId, item));
			}

			flushEquipButton();
			m_bFinishData = false;
			m_pTempButton = nullptr;
			m_pParent->closeView();
			saveEquipHero();
		}

		if (m_eConfirmIndex == CONFIRM_INDEX_SAVE_DATA)
		{
			ProtocolThread::GetInstance()->equipHero(m_gCurEquipId[0],m_gCurEquipId[1],m_gCurEquipId[2],m_gCurEquipId[3],m_gCurEquipId[4],UILoadingIndicator::create(m_pParent));
		}
		return;
	}
	//取消按钮
	if (isButton(button_confirm_no))
	{
		m_pTempButton = nullptr;
		return;
	}
	//装备的详情
	if (isButton(select_equip_bg) || isButton(current_equip_bg))
	{
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushItemsDetail(nullptr, button->getTag(), true);
		return;
	}

// 	if (isButton(button_plus))
// 	{
// 		auto camera = dynamic_cast<Character3DModel*>(m_character)->getCamera();
// 		camera->setPositionX(camera->getPositionX() + 0.1);
// 		log("==========X============%.2f", camera->getPosition3D().x);
// 		log("==========X============%.2f", camera->getPositionX());
// 		return;
// 	}
// 	if (isButton(button_minus))
// 	{
// 		auto camera = dynamic_cast<Character3DModel*>(m_character)->getCamera();
// 		camera->setPositionZ(camera->getPositionZ() - 0.1);
// 		log("===========Z===========%.2f", camera->getPosition3D().z);
// 		log("===========Z===========%.2f", camera->getPositionZ());
// 		return;
// 	}
// 	if (isButton(button_y))
// 	{
// 		auto camera = dynamic_cast<Character3DModel*>(m_character)->getCamera();
// 		camera->setPositionY(camera->getPositionY() - 0.1);
// 		log("==========Y============%.2f", camera->getPosition3D().y);
// 		log("==========Y============%.2f", camera->getPositionY());
// 	}

		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushImageDetail(pSender);
	
}

void UICenterCharactorEquip::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	switch (msgType)
	{
	case PROTO_TYPE_GetHeroEquipResult:
		{
			GetHeroEquipResult *pResult = (GetHeroEquipResult*)(message);
			if (pResult->failed == 0)
			{
				flushHeroEquipInfomation(pResult);
				if (pResult->hp != 0)
				{
					updteCharacterInfo(pResult->hp, pResult->defense, pResult->attack);
				}
			}else
			{
				openSuccessOrFailDialog("TIP_PLAYERCENTER_SEE_PLAYER_EQUIP_FAIL");
				//ProtocolThread::GetInstance()->getHeroEquip(LoadingLayer::create(m_pParent));
			}
			break;
		}
	case PROTO_TYPE_EquipHeroResult:
		{
			EquipHeroResult *pResult = (EquipHeroResult*)(message);
			if (pResult->failed == 0)
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->setCameraMask(4);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ROLL_EQUIP_SAVE_SUCCESS");
				updteCharacterInfo(pResult->hp, pResult->defense, pResult->attack);
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->setCameraMask(4);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_ROLL_EQUIP_SAVE_FAIL");
			}
			break;
		}
	case  PROTO_TYPE_GetEquipableItemsResult:
		{
			GetEquipableItemsResult* pResult = (GetEquipableItemsResult*)message;
			if (pResult->failed == 0)
			{
				m_nType = pResult->type;
				updateEquipData(pResult);
				flushEquipinfo();
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->setCameraMask(4);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_GET_ROLL_EQUIP_FAIL");
			}
			break;
		}
	default:
		break;
	}
}

void UICenterCharactorEquip::flushHeroEquipInfomation(const GetHeroEquipResult *pResult)
{
	if (pResult->head)
	{
		m_gCurEquipId[0] = pResult->head->uniqueid;
		m_gCurEquipIid[0] = pResult->head->iid;
	}else
	{
		m_gCurEquipId[0] = 0;
		m_gCurEquipIid[0] = 0;
	}
	
	if (pResult->clothes)
	{
		m_gCurEquipId[1] = pResult->clothes->uniqueid;
		m_gCurEquipIid[1] = pResult->clothes->iid;
	}else
	{
		m_gCurEquipId[1] = 0;
		m_gCurEquipIid[1] = 0;
	}

	if (pResult->shoe)
	{
		m_gCurEquipId[4] = pResult->shoe->uniqueid;
		m_gCurEquipIid[4] = pResult->shoe->iid;
	}else
	{
		m_gCurEquipId[4] = 0;
		m_gCurEquipIid[4] = 0;
	}

	if (pResult->weapon)
	{
		m_gCurEquipId[2] = pResult->weapon->uniqueid;
		m_gCurEquipIid[2] = pResult->weapon->iid;
	}else
	{
		m_gCurEquipId[2] = 0;
		m_gCurEquipIid[2] = 0;
	}

	if (pResult->access)
	{
		m_gCurEquipId[3] = pResult->access->uniqueid;
		m_gCurEquipIid[3] = pResult->access->iid;
	}else
	{
		m_gCurEquipId[3] = 0;
		m_gCurEquipIid[3] = 0;
	}
	m_characterInfo[CHARACTER_HP] = pResult->hp;
	m_characterInfo[CHARACTER_ATTACK] = pResult->attack;
	m_characterInfo[CHARACTER_DEFENCE] = pResult->defense;
	flushEquipButton();
}

void UICenterCharactorEquip::flushEquipButton()
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CSB]);
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CSB]);
	view->setLocalZOrder(-1);
	auto b_head = view->getChildByName<Button*>("button_maozi");
	auto b_cloth = view->getChildByName<Button*>("button_yifu");
	auto b_shoe = view->getChildByName<Button*>("button_xiezi");
	auto b_weapon = view->getChildByName<Button*>("button_wuqi");
	auto b_access = view->getChildByName<Button*>("button_shoushi");
	auto i_head = b_head->getChildByName<ImageView*>("image_equip");	
	auto i_cloth = b_cloth->getChildByName<ImageView*>("image_equip");
	auto i_shoe = b_shoe->getChildByName<ImageView*>("image_equip");
	auto i_weapon = b_weapon->getChildByName<ImageView*>("image_equip");
	auto i_access = b_access->getChildByName<ImageView*>("image_equip");

	auto i_spe = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_speed"));
	i_spe->setTouchEnabled(true);
	i_spe->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorEquip::menuCall_func, this));
	i_spe->setTag(IMAGE_ICON_HEALTHPOWER + IMAGE_INDEX2);
	auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_atk"));
	i_atk->setTouchEnabled(true);
	i_atk->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorEquip::menuCall_func, this));
	i_atk->setTag(IMAGE_ICON_ATTACKPOWER + IMAGE_INDEX2);
	auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_def"));
	i_def->setTouchEnabled(true);
	i_def->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorEquip::menuCall_func, this));
	i_def->setTag(IMAGE_ICON_DEFENSEPOWER + IMAGE_INDEX2);
	int n_hp = 0;
	int n_attack = 0;
	int n_defence = 0;
	auto shopData = SINGLE_SHOP->getItemData();
	if (m_gCurEquipIid[0])
	{
		auto b_add = b_head->getChildByName<Button*>("button_add");
		b_add->setVisible(false);
		i_head->setVisible(true);
		i_head->loadTexture(getItemIconPath(m_gCurEquipIid[0]));
		b_head->setBright(false);
		
		auto item = shopData[m_gCurEquipIid[0]];
		auto v = getEquipAttribute(item.property2);
		n_hp += v.at(CHARACTER_HP);
		n_attack += v.at(CHARACTER_ATTACK);
		n_defence += v.at(CHARACTER_DEFENCE);
	}else
	{
		auto b_add = b_head->getChildByName<Button*>("button_add");
		b_add->setVisible(true);
		b_add->setSwallowTouches(false);
		i_head->setVisible(false);
		b_head->setBright(true);
	}

	if (m_gCurEquipIid[1])
	{
		auto b_add = b_cloth->getChildByName<Button*>("button_add");
		b_add->setVisible(false);
		i_cloth->setVisible(true);
		i_cloth->loadTexture(getItemIconPath(m_gCurEquipIid[1]));
		b_cloth->setBright(false);
		auto item = shopData[m_gCurEquipIid[1]];
		auto v = getEquipAttribute(item.property2);
		n_hp += v.at(CHARACTER_HP);
		n_attack += v.at(CHARACTER_ATTACK);
		n_defence += v.at(CHARACTER_DEFENCE);
	}else
	{
		auto b_add = b_cloth->getChildByName<Button*>("button_add");
		b_add->setSwallowTouches(false);
		b_add->setVisible(true);
		i_cloth->setVisible(false);
		b_cloth->setBright(true);
	}

	if (m_gCurEquipIid[4])
	{
		auto b_add = b_shoe->getChildByName("button_add");
		b_add->setVisible(false);
		i_shoe->setVisible(true);
		i_shoe->loadTexture(getItemIconPath(m_gCurEquipIid[4]));
		b_shoe->setBright(false);
		auto item = shopData[m_gCurEquipIid[4]];
		auto v = getEquipAttribute(item.property2);
		n_hp += v.at(CHARACTER_HP);
		n_attack += v.at(CHARACTER_ATTACK);
		n_defence += v.at(CHARACTER_DEFENCE);
	}else
	{
		auto b_add = b_shoe->getChildByName<Button*>("button_add");
		b_add->setVisible(true);
		b_add->setSwallowTouches(false);
		i_shoe->setVisible(false);
		b_shoe->setBright(true);
	}

	if (m_gCurEquipIid[2])
	{
		auto b_add = b_weapon->getChildByName("button_add");
		b_add->setVisible(false);
		i_weapon->setVisible(true);
		i_weapon->loadTexture(getItemIconPath(m_gCurEquipIid[2]));
		b_weapon->setBright(false);
		auto item = shopData[m_gCurEquipIid[2]];
		auto v = getEquipAttribute(item.property2);
		n_hp += v.at(CHARACTER_HP);
		n_attack += v.at(CHARACTER_ATTACK);
		n_defence += v.at(CHARACTER_DEFENCE);
	}else
	{
		auto b_add = b_weapon->getChildByName<Button*>("button_add");
		b_add->setVisible(true);
		b_add->setSwallowTouches(false);
		i_weapon->setVisible(false);
		b_weapon->setBright(true);
	}

	if (m_gCurEquipIid[3])
	{
		auto b_add = b_access->getChildByName("button_add");
		b_add->setVisible(false);
		i_access->setVisible(true);
		i_access->loadTexture(getItemIconPath(m_gCurEquipIid[3]));
		b_access->setBright(false);
		auto item = shopData[m_gCurEquipIid[3]];
		auto v = getEquipAttribute(item.property2);
		n_hp += v.at(CHARACTER_HP);
		n_attack += v.at(CHARACTER_ATTACK);
		n_defence += v.at(CHARACTER_DEFENCE);
	}else
	{
		auto b_add = b_access->getChildByName<Button*>("button_add");
		b_add->setSwallowTouches(false);
		b_add->setVisible(true);
		i_access->setVisible(false);
		b_access->setBright(true);
	}
	std::string path;
	if (SINGLE_HERO->m_iGender == 1)
	{
		path = getCharacterModePath(1);
	}else
	{
		path = getCharacterModePath(2);
	}
	auto i_image = Helper::seekWidgetByName(view, "image_male");
	int hp = m_characterInfo[CHARACTER_HP] + n_hp;
	int defence = m_characterInfo[CHARACTER_DEFENCE] + n_defence;
	int attack = m_characterInfo[CHARACTER_DEFENCE] + n_attack;
//	updteCharacterInfo(hp,defence,attack);


	if (!m_alreadyAdd)
	{
		m_alreadyAdd = true;
		addCharacter3DModel(i_image, path);
	}
	else
	{
		auto instance = ModeLayerManger::getInstance();
		auto layer = instance->getModelLayer();
		if (layer != nullptr)
		{
			instance->showModelLayer(true);
		}
		auto character = ModeLayerManger::getInstance()->getModelByType(CENTER_CHARACTER);
		addEquipment(character);
	}
	
}

void UICenterCharactorEquip::flushEquipinfo()
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CHANGE_CSB]);
	auto p_addequip =  m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CHANGE_CSB]);
	auto scene = _director->getRunningScene();
	auto cameras = scene->getCameras();
	auto instance = ModeLayerManger::getInstance();
	auto layer = instance->getModelLayer();
	if (layer != nullptr)
	{
		instance->showModelLayer(false);
	}
	
	auto l_equip = p_addequip->getChildByName<ListView*>("listview_left");
	auto p_bag = l_equip->getItem(0);
	auto i_title = p_bag->getChildByName<ImageView*>("image_icon");
	auto tx_title = p_bag->getChildByName<Text*>("label_city");
	tx_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BAG"]);
	i_title->setPositionX(tx_title->getPositionX() - i_title->getBoundingBox().size.width - tx_title->getBoundingBox().size.width / 2);
	auto p_no = p_addequip->getChildByName<Widget*>("panel_no");
	p_no->setVisible(false);
	auto b_ok = p_addequip->getChildByName<Button*>("button_ok");
	b_ok->setBright(false);
	b_ok->setTouchEnabled(false);

	auto b_cur_equip = p_addequip->getChildByName<Button*>("current_equip_bg");
	auto cur_equip = b_cur_equip->getChildByName<ImageView*>("image_current_equip");
	auto b_uninstall = p_addequip->getChildByName<Button*>("button_uninstall");
	auto t_curname = p_addequip->getChildByName<Text*>("label_current_itemname");
	updataSelectEquip(-1);
	std::vector<EquipableItem*> itemsInfo;

	if (m_gCurEquipIid[m_nType - 1])
	{
		cur_equip->setVisible(true);
		cur_equip->ignoreContentAdaptWithSize(false);
		cur_equip->loadTexture(getItemIconPath(m_gCurEquipIid[m_nType - 1]));
		b_uninstall->setBright(true);
		b_uninstall->setTouchEnabled(true);
		t_curname->setString(getItemName(m_gCurEquipIid[m_nType - 1]));
		setBgButtonFormIdAndType(b_cur_equip, m_gCurEquipIid[m_nType - 1], ITEM_TYPE_ROLE_EQUIP);
		setTextColorFormIdAndType(t_curname, m_gCurEquipIid[m_nType - 1], ITEM_TYPE_ROLE_EQUIP);
		b_cur_equip->setTouchEnabled(true);
		b_cur_equip->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorEquip::menuCall_func,this));
		b_cur_equip->setTag(m_gCurEquipIid[m_nType - 1]);
	}else
	{
		cur_equip->setVisible(false);
		b_uninstall->setBright(false);
		b_uninstall->setTouchEnabled(false);
		t_curname->setString("N/A");
		b_cur_equip->setTouchEnabled(false);
	}

	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(p_addequip,"label_title_sell"));
	auto p_no1 = p_no->getChildByName<Text*>("label_no_item1"); 
	auto p_no2 = p_no->getChildByName<Text*>("label_no_item2");
	switch (m_nType) 
	{
	case 1: 
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_HEAD"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_HEAD_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_HEAD_NO2"]);
			break;
		}
	case 2:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_CLOTHES"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_CLOTHES_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_CLOTHES_NO2"]);
			break;
		}
	case 3:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_WEAPONS"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_WEAPON_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_WEAPON_NO2"]);
			break;
		}
	case 4:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_ACCESS"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_ACCESS_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_ACCESS_NO2"]);
			break;
		}
	case 5:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_SHOE"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_SHOE_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_SHOE_NO2"]);
			break;
		}
	default:
		break;
	}
	if (m_vAllEquip[m_nType - 1].empty())
	{
		p_no->setVisible(true);
		auto l_list1 = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_addequip, "listview_p"));
		auto l_list2 = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_addequip, "listview_left"));
		auto i_pulldown1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_addequip, "image_pulldown"));
		auto b_pulldown = i_pulldown1->getChildByName("button_pulldown");
		auto i_pulldown2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_addequip, "image_pulldown_0"));
		auto b_pulldown_1 = i_pulldown2->getChildByName("button_pulldown");
		b_pulldown->setPositionX(b_pulldown->getPositionX() - b_pulldown->getContentSize().width / 2);
		b_pulldown_1->setPositionX(b_pulldown_1->getPositionX() - b_pulldown_1->getContentSize().width / 2);
		addListViewBar(l_list1, i_pulldown1);
		addListViewBar(l_list2, i_pulldown2);
		return;
	}
	auto p_item = p_addequip->getChildByName<Widget*>("panel_items");
	auto iter = m_vAllEquip[m_nType - 1].begin();
	for (int i = 0; i < ceil(m_vAllEquip[m_nType - 1].size()/6.0); i++)
	{
		auto p_item_clone = p_item->clone();
		for (int j = 0; j < 6; j++)
		{
			auto item  = p_item_clone->getChildByName<Widget*>(StringUtils::format("image_items_%d",j + 1));
			auto i_goods = item->getChildByName<ImageView*>("image_goods");
			auto b_goods = item ->getChildByName<Button*>("button_equip_items");
			if (i * 6 + j < m_vAllEquip[m_nType - 1].size())
			{
				i_goods->setVisible(true);
				b_goods->setTouchEnabled(true);
				i_goods->ignoreContentAdaptWithSize(false);
				i_goods->loadTexture(getItemIconPath(iter->second.iid));
				b_goods->setTag(iter->second.iid);
				item->setTag(iter->first);
				auto t_num = i_goods->getChildByName<Text*>("text_item_num");
				setTextSizeAndOutline(t_num,iter->second.amount);
				setBgButtonFormIdAndType(item, iter->second.iid, ITEM_TYPE_ROLE_EQUIP);
				iter++;
			}else
			{
				i_goods->setVisible(false);
				b_goods->setTouchEnabled(false);
			}
		}
		l_equip->pushBackCustomItem(p_item_clone);
	}
	auto l_list1 = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_addequip, "listview_p"));
	auto l_list2 = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_addequip, "listview_left"));
	auto i_pulldown1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_addequip, "image_pulldown"));
	auto b_pulldown = i_pulldown1->getChildByName("button_pulldown");
	auto i_pulldown2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_addequip, "image_pulldown_0"));
	auto b_pulldown_1 = i_pulldown2->getChildByName("button_pulldown");
	b_pulldown->setPositionX(b_pulldown->getPositionX() - b_pulldown->getContentSize().width / 2);
	b_pulldown_1->setPositionX(b_pulldown_1->getPositionX() - b_pulldown_1->getContentSize().width / 2);
	addListViewBar(l_list1, i_pulldown1);
	addListViewBar(l_list2, i_pulldown2);
}

void UICenterCharactorEquip::updataSelectEquip(const int nId)
{
	auto p_addequip = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CHANGE_CSB]);
	auto t_name = p_addequip->getChildByName<Text*>("label_select_itemname");
	auto t_desc = p_addequip->getChildByName<Text*>("label_select_itemname_0");
	auto b_sel_equip = p_addequip->getChildByName<Button*>("select_equip_bg");
	auto i_sel_equip = b_sel_equip->getChildByName<ImageView*>("image_current_equip");
	if (nId == -1)
	{
		t_desc->setVisible(false);
		t_name->setString("N/A");
		b_sel_equip->setTouchEnabled(false);
		i_sel_equip->setVisible(false);
	}else
	{
		t_desc->setVisible(true);
		t_name->setString(SINGLE_SHOP->getItemData()[nId].name);
		if (SINGLE_SHOP->getItemData()[nId].type == ITEM_TYPE_SHIP_EQUIP && SINGLE_SHOP->getItemData()[nId].sub_type == SUB_TYPE_SHIP_FIGUREHEAD)
		{
			t_desc->setString(SINGLE_SHOP->getItemData()[nId].shortdesc);
		}
		else
		{
			t_desc->setString(SINGLE_SHOP->getItemData()[nId].desc);
		}
		
		setTextColorFormIdAndType(t_name, nId, ITEM_TYPE_ROLE_EQUIP);
		i_sel_equip->setVisible(true);
		i_sel_equip->loadTexture(getItemIconPath(nId));
		b_sel_equip->setTouchEnabled(true);
		b_sel_equip->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorEquip::menuCall_func,this));
		b_sel_equip->setTag(nId);
	}
}

bool UICenterCharactorEquip::getFinishData()
{
	return m_bFinishData;
}

void UICenterCharactorEquip::setFinishData(const bool bFinish)
{
	m_bFinishData = bFinish;
}
void UICenterCharactorEquip::saveEquipHero()
{
	ProtocolThread::GetInstance()->equipHero(m_gCurEquipId[0], m_gCurEquipId[1], m_gCurEquipId[2], m_gCurEquipId[3], m_gCurEquipId[4], /*LoadingLayer::createWithMask(m_characterLayer, 4)*/nullptr);
}
void UICenterCharactorEquip::updateEquipData(const GetEquipableItemsResult *pResult)
{
	int index = pResult->type - 1;
	m_gIsFulshEquip[index] = false;
	int id = 0;
	ROLE_EQUIP item;
	bool iscan = false;
	for (int i = 0; i < pResult->n_items; i++)
	{
		id = pResult->items[i]->id;
		item.iid = pResult->items[i]->iid;
		item.amount = pResult->items[i]->num;
		if (m_gCurEquipId[index] != id || (m_gCurEquipId[index] == id && item.amount-- > 1))
		{
			m_vAllEquip[index].insert(std::pair<int, ROLE_EQUIP>(id, item));
		}
	}
	/*
	if (!iscan && m_gCurEquipIid[index])
	{
		item.iid = m_gCurEquipIid[index];
		item.amount = 1;
		m_vAllEquip[index].insert(std::pair<int,ROLE_EQUIP>(m_gCurEquipId[index],item));
	}
	*/
}

void UICenterCharactorEquip::addCharacter3DModel(Node * parent, const std::string path)
{
// 	auto sp_character = Character3DModel::create(path);
// 	auto sextant = Sprite3D::create("Character3D/sextant.c3b");
// 	sextant->setCameraMask(2, true);
// 	sextant->setVisible(false);
// 	auto node = Layer::create();
// 	auto characterNode = Node::create();
// 	node->setName("characterlayer");
// 	characterNode->setName("characterNode");
// 	m_characterLayer = node;
// 	auto animation = Animation3D::create(path);
// 	auto animate = Animate3D::create(animation);
// 	sp_character->runAction(RepeatForever::create(animate));
// 	sp_character->setScale(1.0);
// 	sp_character->setVisible(true);
// 	sp_character->setForceDepthWrite(true);
// 
// 	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CSB]);
// 	m_character = sp_character;
// 	auto pos = dynamic_cast<Widget*>(parent)->getWorldPosition();
// 	characterNode->setPosition(pos);
// 	_center = sp_character->getPosition3D();
// 
// 	
// 	auto scene = _director->getRunningScene();
// 	characterNode->addChild(sp_character);
// 	m_characterLayer->addChild(characterNode);
// 	scene->addChild(m_characterLayer, 1000);
// 
// 	auto listener = EventListenerTouchOneByOne::create();
// 	listener->onTouchBegan = CC_CALLBACK_2(UICenterCharactorEquip::onTouchBegan,this);
// 	listener->onTouchMoved = CC_CALLBACK_2(UICenterCharactorEquip::onTouchMoved,this);
// 	listener->onTouchEnded = CC_CALLBACK_2(UICenterCharactorEquip::onTouchEnded,this);
// 	_director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, m_characterLayer);
// 
// //界面Camera 管理loadingLayer
// 	auto size = Director::getInstance()->getWinSize();
// 	Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
// 	popCam->setCameraFlag(CameraFlag::USER2);
// 	popCam->setDepth(4);
// 	m_viewCamera = popCam;
// 	m_characterLayer->addChild(popCam);
// 
// //角色camera 管理角色
// 	Size s = CCDirector::getInstance()->getWinSize();
// 	auto m_camera = Camera::createPerspective(60, (GLfloat)s.width / s.height, 1, 1000);
// 	m_camera->setCameraFlag(CameraFlag::USER1);
// 	Vec3 characterPos = getPosition3D();
// 	m_camera->setPosition3D(characterPos + Vec3(0, 20, 20));
// 	m_camera->lookAt(characterPos, Vec3(0, 1, 0));
// 	m_camera->setRotation3D(Vec3(0, 0, 0));
// 	m_camera->setPosition3D(Vec3(0.4, 2.4, 6.5));
// 	m_camera->setDepth(2);
// 	characterNode->addChild(m_camera);
// 	auto attach = sp_character->getAttachNode("joint_hem_LM_100_1");
// 	attach->addChild(sextant);
	auto instance = ModeLayerManger::getInstance();
//	addEquipment(sp_character);
	CharacterEquiment info;
	info.clothId = m_gCurEquipIid[EQUIP_CLOTH];
	info.hatId = m_gCurEquipIid[EQUIP_HAT];
	info.necklaceId = m_gCurEquipIid[EQUIP_ACCESS];
	info.shoesId = m_gCurEquipIid[EQUIP_SHOES];
	info.weaponId = m_gCurEquipIid[EQUIP_WEAPON];
	info.gender = SINGLE_HERO->m_iGender;
	info.iconindex = SINGLE_HERO->m_iIconidx;
	instance->openModelLayer();
	instance->addCharacter(dynamic_cast<Widget*>(parent), CENTER_CHARACTER, info);
}
void UICenterCharactorEquip::updteCharacterInfo(int hp, int defence, int attack)
{
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CSB]);
	auto t_hp = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_speed_num"));
	auto t_defence = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_defnum"));
	auto t_attack = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_atk_num"));
	t_hp->setString(StringUtils::format("%d", hp));
	t_defence->setString(StringUtils::format("%d",defence));
	t_attack->setString(StringUtils::format("%d", attack));
}

void UICenterCharactorEquip::addEquipment(Sprite3D * character)
{
	if (character == nullptr)
	{
		return;
	}
	std::vector<std::string>meshes;
	character->setVisible(true);
// 	if (SINGLE_HERO->m_iGender == 1)
// 	{
// 		meshes = m_manmesh;
// 	}
// 	else
// 	{
// 		meshes = m_womanmesh;
// 	}

//	auto skeleton = character->getSkeleton();

	//添加武器

//		auto sp_weapon = Sprite3D::create("Character3D/weaponA.c3b");
// 		for (int i = 0; i < skeleton->getBoneCount(); i++)
// 		{	
// 			auto bone = skeleton->getBoneByIndex(i);
// 			if (bone->getName().compare("joint_weapon_R_100_1") == 0)
// 			{
// 				auto attach = character->getAttachNode(bone->getName());
// 				//添加武器
// 				if (attach->getChildren().size()== 0)
// 				{					
// 					sp_weapon->setCameraMask(2, true);
// 					attach->addChild(sp_weapon);
// 				}
// 				//更换武器
// 				else
// 				{
// 
// 				}
// 			}
// 		}
	dynamic_cast<CharacterModel*>(character)->showArmors(m_gCurEquipIid[EQUIP_WEAPON],CharacterModel::MESH_TYPE_WEAPON);
	dynamic_cast<CharacterModel*>(character)->showArmors(m_gCurEquipIid[EQUIP_CLOTH], CharacterModel::MESH_TYPE_CLOTHES);
	dynamic_cast<CharacterModel*>(character)->showArmors(m_gCurEquipIid[EQUIP_HAT], CharacterModel::MESH_TYPE_HATS);
	dynamic_cast<CharacterModel*>(character)->showArmors(m_gCurEquipIid[EQUIP_ACCESS], CharacterModel::MESH_TYPE_NECKLACE);
	dynamic_cast<CharacterModel*>(character)->showArmors(m_gCurEquipIid[EQUIP_SHOES], CharacterModel::MESH_TYPE_SHOES);

// 		//卸载武器
// 		for (int i = 0; i < skeleton->getBoneCount(); i++)
// 		{
// 			auto bone = skeleton->getBoneByIndex(i);
// 			if (bone->getName().compare("joint_weapon_R_100_1") == 0)
// 			{
// 				auto attach = character->getAttachNode(bone->getName());
// 				if (attach->getChildren().size() >0)
// 				{		
// 					auto child = attach->getChildren().at(0);
// 					attach->removeChild(child, true);
// 				}
// 				 
// 			}
// 		}
// 	
// 	//衣服
// 	if (m_gCurEquipIid[EQUIP_CLOTH] != 0)
// 	{
// 		auto mesh1 = character->getMeshByName(meshes.at(MESH_CLOTHES));
// 		auto mesh2 = character->getMeshByName(meshes.at(MESH_PANTS));
// 		auto attach = character->getAttachNode("joint_hem_LM_100_1");
// 		if (SINGLE_HERO->m_iGender == 2)
// 		{
// 			auto mesh3 = character->getMeshByName(meshes.at(MESH_GLOVES));
// 			mesh3->setVisible(true);
// 		}
// 		mesh1->setVisible(true);
// 		mesh2->setVisible(true);
// 		if (attach->getChildren().at(0)!= nullptr)
// 		{
// 			auto sextant = attach->getChildren().at(0);
// 			sextant->setVisible(true);
// 		}
// 	}
// 	else
// 	{
// 		auto mesh1 = character->getMeshByName(meshes.at(MESH_CLOTHES));
// 		auto mesh2 = character->getMeshByName(meshes.at(MESH_PANTS));
// 		auto attach = character->getAttachNode("joint_hem_LM_100_1");
// 		if (SINGLE_HERO->m_iGender == 2)
// 		{
// 			auto mesh3 = character->getMeshByName(meshes.at(MESH_GLOVES));
// 			mesh3->setVisible(false);
// 		}
// 		mesh1->setVisible(false);
// 		mesh2->setVisible(false);
// 		if (attach->getChildren().at(0) != nullptr)
// 		{
// 			auto sextant = attach->getChildren().at(0);
// 			sextant->setVisible(false);
// 		}
// 	}
// 	//鞋子
// 	if (m_gCurEquipIid[EQUIP_SHOES] != 0)
// 	{
// 		auto mesh1 = character->getMeshByName(meshes.at(MESH_SHOES));
// 		mesh1->setVisible(true);
// 
// 	}
// 	else
// 	{
// 		auto mesh1 = character->getMeshByName(meshes.at(MESH_SHOES));
// 		mesh1->setVisible(false);
// 	}
// 
// 	//饰品
// 	if (m_gCurEquipIid[EQUIP_ACCESS] != 0)
// 	{
// 		auto mesh = character->getMeshByName(meshes.at(MESH_NECKLACE));
// 		mesh->setVisible(true);
// 	}
// 	else
// 	{
// 		auto mesh = character->getMeshByName(meshes.at(MESH_NECKLACE));
// 		mesh->setVisible(false);
// 	}
// 
// 	//帽子
// 	if (m_gCurEquipIid[EQUIP_HAT] != 0)
// 	{
// 		auto mesh = character->getMeshByName(meshes.at(MESH_HATS));
// 		mesh->setVisible(true);
// 	}
// 	else
// 	{
// 		auto mesh = character->getMeshByName(meshes.at(MESH_HATS));
// 		mesh->setVisible(false);
// 	}
}

bool UICenterCharactorEquip::onTouchBegan(Touch *touch, Event *unused_event)
{
	if (touch->getLocation().x >= 420 && touch->getLocation().x <=770 && touch->getLocation().y >= 100 && touch->getLocation().y<=530 )
	{
		m_touchBegan = touch->getLocation();
	}
	return true;
}
void UICenterCharactorEquip::onTouchMoved(Touch *touch, Event *unused_event)
{
	Point p1 = touch->getLocation();
	if (m_touchBegan != Vec2(0,0))
	{
		if (p1.x - m_touchBegan.x > 0)
		{
			m_character->setRotation3D(m_character->getRotation3D() + Vec3(0,5,0));
		}
		else if (p1.x - m_touchBegan.x < 0)
		{
			m_character->setRotation3D(m_character->getRotation3D() + Vec3(0,-5,0));
		}
		m_touchBegan = p1;
	}
}
void UICenterCharactorEquip::onTouchEnded(Touch *touch, Event *unused_event)
{
	m_touchBegan = Vec2(0, 0);
}

bool UICenterCharactorEquip::init()
{
	if (!UIBasicLayer::init())
	{
		return false;
	}

	return true;
}

void UICenterCharactorEquip::onEnter()
{
	UIBasicLayer::onEnter();
}

std::vector<int> UICenterCharactorEquip::getEquipAttribute(const std::string attributeStr)
{
	auto substr = attributeStr;
	std::vector<int> valuevector;

	if (attributeStr.size() == 0 || attributeStr.find_first_not_of(","))
	{
		valuevector.push_back(0);
		valuevector.push_back(0);
		valuevector.push_back(0);
		valuevector.push_back(0);
		valuevector.push_back(0);
		return valuevector;
	}
	while (substr.find(",")!= -1 )
	{
		int a = substr.find_first_of(",");		
		int value = atoi(substr.substr(0, a).c_str());
		substr = substr.substr(a + 1,std::string::npos);
		valuevector.push_back(value);
	}
	int a = atoi(substr.c_str());
	valuevector.push_back(a);
	return valuevector;
}
void UICenterCharactorEquip::calculateArcBall(cocos2d::Vec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y)
{
	Mat4 rotation_matrix;
	Mat4::createRotation(_rotationQuat, &rotation_matrix);
	//rotation y
	Vec3 uv = rotation_matrix * Vec3(0.0f, 1.0f, 0.0f); 
	//rotation x
	Vec3 sv = rotation_matrix * Vec3(1.0f, 0.0f, 0.0f); 
	//rotation z
	Vec3 lv = rotation_matrix * Vec3(0.0f, 0.0f, -1.0f);
	//start point screen transform to 3d
	Vec3 p1 = sv * p1x + uv * p1y - lv * projectToSphere(_radius, p1x, p1y);
	//end point screen transform to 3d
	Vec3 p2 = sv * p2x + uv * p2y - lv * projectToSphere(_radius, p2x, p2y); 
	//calculate rotation axis
	Vec3::cross(p2, p1, &axis);  
	axis.normalize();

	float t = (p2 - p1).length() / (2.0 * _radius);
	//clamp -1 to 1
	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	//rotation angle
	angle = asin(t);           
}
void UICenterCharactorEquip::updateCameraTransform()
{
	Mat4 trans, rot, center;
	Mat4::createTranslation(Vec3(0.0f, 10.0f, _distanceZ), &trans);
	Mat4::createRotation(_rotationQuat, &rot);
	Mat4::createTranslation(_center, &center);
	Mat4 result = center * rot * trans;
	auto camera = dynamic_cast<Character3DModel*>(m_character)->getCamera();
	camera->setNodeToParentTransform(result);
}

float UICenterCharactorEquip::projectToSphere(float r, float x, float y)
{
	float d, t, z;
	d = sqrt(x*x + y*y);
	if (d < r * 0.70710678118654752440)//inside sphere
	{
		z = sqrt(r*r - d*d);
	}
	else                               //on hyperbola
	{
		t = r / 1.41421356237309504880;
		z = t*t / d;
	}
	return z;
}

void UICenterCharactorEquip::confirmCharacterFaceAndHair(Sprite3D * character)
{
	std::string face;
	std::string hair;
	if (SINGLE_HERO->m_iGender == 1)
	{
		face = "man_face_M_100";
		hair = "man_hair_M_100";
	}
	else if (SINGLE_HERO->m_iGender == 2)
	{
		face = "woman_face_M_100";
		hair = "woman_hair_M_100";
	}

	for (int i = 0; i < character->getMeshCount(); i++)
	{
		auto mesh = character->getMeshByIndex(i);
		if (mesh->getName().find(face) == 0)
		{
			std::string str_face = StringUtils::format("%s_%lld", face.c_str(), SINGLE_HERO->m_iIconidx);
			log("Mesh name:%s", str_face.c_str());
			if (mesh->getName().find(str_face) == 0)
			{
				mesh->setVisible(true);
				log("LOGTEST:head is show");
				log("Mesh name:%s", str_face.c_str());
			}
			else
			{
				mesh->setVisible(false);
			}
		}
		if (mesh->getName().find(hair) == 0)
		{
			std::string str_hair = StringUtils::format("%s_%lld", hair.c_str(), SINGLE_HERO->m_iIconidx);
			log("Mesh name:%s", str_hair.c_str());
			if (mesh->getName().find(str_hair) == 0)
			{
				mesh->setVisible(true);
				log("LOGTEST:head is show");
				log("Mesh name:%s",str_hair.c_str());
			}
			else
			{
				mesh->setVisible(false);
			}
		}
	}
}

Character3DModel::Character3DModel()
{
	m_camera = nullptr;
}
Character3DModel::~Character3DModel()
{

}
Sprite3D *Character3DModel::create(const std::string &modelPath)
{
	if (modelPath.length() < 4)
		CCASSERT(false, "invalid filename for Sprite3D");

	auto sprite = new (std::nothrow) Character3DModel();
	if (sprite && sprite->initWithFile(modelPath))
	{
		sprite->_contentSize = sprite->getBoundingBox().size;
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Character3DModel::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (isVisitableByVisitingCamera()){
		Mat4 transform = parentTransform;
#if 0
		std::vector<Node*> myParents;
		Node* start = this->getParent();
		while (start){
			myParents.push_back(start);
			start = start->getParent();
		}
		std::vector<Node*>::iterator iter;
		for (iter = myParents.end() - 1; iter != myParents.begin() - 1; iter--)
		{
			Node*node = *iter;
			parentFlags = node->processParentFlags(transform, parentFlags);
			transform = node->getModelViewTransform();
		}
#endif
		Sprite3D::visit(renderer, transform, parentFlags);
	}
	else{

	}
}
void Character3DModel::onEnter()
{
	Sprite3D::onEnter();
	this->setCameraMask((unsigned short)CameraFlag::USER1,true);
	//	_obj_x->setCameraMask((unsigned short)CameraFlag::USER1, true);
}
