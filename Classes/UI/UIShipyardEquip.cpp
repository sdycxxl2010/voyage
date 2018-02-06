#include "UIShipyardEquip.h"
#include "UIShipyardFleetCabin.h"
#include "UIShipyard.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UISocial.h"
#include "ModeLayerManger.h"
enum SHIP_EQUIP_FLAG
{
	FIRSTHEAD,
	GUN_1,
	GUN_ALL,
	SAIL_ALL,
	ARMOR_ALL,		
	GUN_TAIL,
};

UIShipyardEquip::UIShipyardEquip():
	m_PressButton(nullptr),
	m_pParent(nullptr),
	m_pTempButton(nullptr),
	m_pEquipIcon(nullptr),
	m_pShipsList(nullptr),
	m_pShipInfoResult(nullptr),
	m_pCurEquipdetails(nullptr),
	m_nGlobalIndex(-1),
	m_nEquipDetailsNum(-1)
{
	m_nCurFirstH_id = -1;
	m_nCurGun1_id = -1;
	m_nCurGun2_id = -1;
	m_nFirst_FirstH_id = -1;
	m_nFirst_Gun1_id = -1;
	m_nFirst_Gun2_id = -1;
	m_vCurSails_id.clear();
	m_vCurGuns_id.clear();
	m_vCurArmors_id.clear();
	m_vFirst_CurSails_id.clear();
	m_vFirst_CurGuns_id.clear();
	m_vFirst_CurArmors_id.clear();
	m_vEquips_num.clear();
	m_vAllEquipItem.clear();
	m_vAllEquips.clear();
	m_nMainPressIndex = -1;
	m_bIsFormationButtonSubmite = false;
	m_curItem = nullptr;
	m_operaModel = nullptr;
	m_touchBegan = Vec2(0, 0);
}

UIShipyardEquip::~UIShipyardEquip()
{
	
}

UIShipyardEquip* UIShipyardEquip::createShipEquip(UIBasicLayer* parent)
{
	UIShipyardEquip* sv = new UIShipyardEquip;
	if (sv)
	{
		sv->m_pParent = parent;
		sv->autorelease();
		return sv;
	}
	CC_SAFE_RELEASE(sv);
	return nullptr;
}

void UIShipyardEquip::convertToVector(std::string &_src,std::vector<int> &_des)
{
	if (_src.empty())
	{
		return;
	}
	if (_des.empty())
	{
		return;
	}
	int prePos = 0;
	int flag = 0;
	for (size_t i = 0; i != std::string::npos; i++,flag++)
	{
		prePos = i;
		i = _src.find(",",prePos);
		if (i == std::string::npos || i == _src.length() - 1)
		{
			break;
		}
		int id = atoi(_src.substr(prePos,i).c_str());
		_des[flag] = id;
	}
	_des[flag] = (atoi(_src.substr(prePos).c_str()));
}

std::string UIShipyardEquip::convertToString(std::vector<int> &_src)
{
	std::string _des("");
	if (_src.empty())
	{
		return _des;
	}
	for(auto iter = _src.begin();iter != _src.end();iter++)
	{
		_des += StringUtils::format("%d",*iter);
		_des.append(",");
	}
	_des.erase(_des.length() - 1);
	return _des;
}

void UIShipyardEquip::changePressButtonState(Widget* target)
{
	if (target == nullptr)
	{
		return;
	}
	
	if (m_PressButton)
	{
		auto image_btn_on = m_PressButton->getChildByName<ImageView*>("image_btn_on");
		if (image_btn_on)
		{
			image_btn_on->setVisible(false);
			m_PressButton->setTouchEnabled(true);
		}
	}
	target->setTouchEnabled(false);
	m_PressButton = target;
	auto image_btn_on = m_PressButton->getChildByName<ImageView*>("image_btn_on");
	image_btn_on->setVisible(true);
    auto view = m_pParent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	auto i_selectBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_select")); 
	if (i_selectBg->getParent())
	{
		i_selectBg->removeFromParent();
		i_selectBg->retain();
		i_selectBg->setPosition(target->getContentSize() / 2);
		i_selectBg->setVisible(true);
		target->addChild(i_selectBg);
	}
}

void UIShipyardEquip::showShipsList()
{
	for (int i = 0; i < 5; ++i)
	{
		auto item = m_pShipsList->getItem(i);
		auto i_ship = item->getChildByName<ImageView*>("image_ship");
		auto i_num = item->getChildByName<ImageView*>("image_num");
		if (i < m_pShipInfoResult->n_sidlist)
		{
			item->setVisible(true);
			item->setTouchEnabled(true);
			item->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,dynamic_cast<UIShipyard*>(m_pParent)));
			item->setTag(i + START_INDEX);
			item->setBright(true);
			i_ship->loadTexture(getShipIconPath(m_pShipInfoResult->sidlist[i]));
			i_num->loadTexture(getPositionIconPath(i + 1));
			setBgButtonFormIdAndType(item, m_pShipInfoResult->sidlist[i], ITEM_TYPE_SHIP);

			auto image_notify = Helper::seekWidgetByName(item, "image_notify");
		
			if (m_pShipInfoResult->optionitemcount[i] > 0)
			{
				addStrengtheningIcon(item);
			}

			std::string st_ship_position = StringUtils::format(SHIP_POSTION_EQUIP_BROKEN, i + 1);
			if (i == 0)
			{
				UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), false);
				UserDefault::getInstance()->flush();
			}
			else
			{
				auto b_notify =  UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), false);
				if (image_notify)
				{
					image_notify->setVisible(b_notify);
				}
			}
			
		}else
		{
			std::string st_ship_position = StringUtils::format(SHIP_POSTION_EQUIP_BROKEN, i + 1);
			UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), false);
			UserDefault::getInstance()->flush();
			item->setVisible(false);
		}
	}
}

void UIShipyardEquip::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto button = dynamic_cast<Widget*>(pSender);
		buttonEventDis(button,button->getName());
	}
}

void UIShipyardEquip::buttonEventDis(Widget* target, std::string name)
{
	//旗舰的船舱配置小伙伴
	if (isButton(button_cabin))
	{
		if (!SINGLE_HERO->m_bClickTouched)
		{
			SINGLE_HERO->m_bClickTouched = true;
			auto instance = ModeLayerManger::getInstance();
			auto layer = instance->getModelLayer();
			if (layer != nullptr)
			{
				instance->showModelLayer(false);
			}
			auto fc = UIShipyardFleetCabin::createFleetCabin(target->getTag() - 1000);
			this->addChild(fc);
		}
		return;
	}
	//船只切换
	if (isButton(button_ship_))
	{
		if (!dynamic_cast<UIShipyard *>(m_pParent)->getShipEquipFinishData())
		{
			submitDataToserver();
		}

		m_nMainPressIndex = target->getTag() - START_INDEX;
		auto imageNotify = dynamic_cast<ImageView*>(Helper::seekWidgetByName(target, "image_notify"));

		std::string st_ship_position = StringUtils::format(SHIP_POSTION_EQUIP_BROKEN, m_nMainPressIndex + 1);
		UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), false);
		UserDefault::getInstance()->flush();
		if (imageNotify)
		{
			imageNotify->setVisible(false);
		}
		auto scene = _director->getRunningScene();
		auto layer = dynamic_cast<Layer*>(scene->getChildByName("shipLayer"));
		if (layer == nullptr)
		{
			ProtocolThread::GetInstance()->getEquipShipInfo(m_pShipInfoResult->shiplist[m_nMainPressIndex], UILoadingIndicator::create(m_pParent));
		}
		else
		{
			ProtocolThread::GetInstance()->getEquipShipInfo(m_pShipInfoResult->shiplist[m_nMainPressIndex], UILoadingIndicator::createWithMask(m_pParent,4));
		}
		return;
	}
	//卸载装备
	if (isButton(button_uninstall))
	{
		UIInform::getInstance()->openInformView(m_pParent);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_CENTER_SHIP_EQUIP_UNINSTALL_TITLE", "TIP_CENTER_SHIP_EQUIP_UNINSTALL");
		return;
	}
	//选择装备
	if (isButton(button_items))
	{
		if (m_pTempButton)
		{
			m_pTempButton->setBright(true);
		}
		m_pTempButton = target;
		m_pTempButton->setBright(false);
		flushEquipInfo();
		return;
	}
	//选择装备确定按钮
	if (isButton(button_ok))
	{
		int tag = target->getTag();
		if (tag < 40)
		{
			if (tag == 10 || tag == 20)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_EUQIP_LEVEL_NOT_ENOUGH");
			}else if (tag == 30)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_EUQIP_PROFICIENCY_NOT_ENOUGH");
			}
			return;
		}

		changeVectorContent();
		auto veiw = m_pParent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
		auto w_gear = dynamic_cast<Widget*>(veiw->getChildByName("panel_gear"));
		
		m_pTempButton = nullptr;
		dynamic_cast<UIShipyard*>(m_pParent)->closeView();
		submitDataToserver();//TODO..........................................not good
		return;
	}
	//取消
	if (isButton(button_cancel))
	{
		m_pTempButton = nullptr;
		dynamic_cast<UIShipyard*>(m_pParent)->closeView();
		return;
	}
	//卸载装备确认按钮
	if (isButton(button_confirm_yes))
	{
		changeVectorContent(false);
		m_pTempButton = nullptr;
		dynamic_cast<UIShipyard*>(m_pParent)->closeView();
		submitDataToserver();//TODO................................
		return;
	}
	//取消
	if (isButton(button_confirm_no))
	{
		m_pTempButton = nullptr;
		return;
	}
	//装备的详情
	if (isButton(select_equip_bg) || isButton(current_equip_bg))
	{
		ProtocolThread::GetInstance()->getItemsDetailInfo(target->getTag(), ITEM_TYPE_SHIP_EQUIP, target->getChildren().at(0)->getTag(), UILoadingIndicator::create(m_pParent));
		if (isButton(select_equip_bg))
	    {
			m_curItem = target->getChildByName<Widget*>("image_select_equip");
		}
		else
		{
			m_curItem = target->getChildByName<Widget*>("image_current_equip");
		}

		return;
	}
	if (isButton(image_durable))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_sailor))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_weight))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_supply))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_atk))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_def))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_speed))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_steering))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(button_switch))
	{
		int tag = target->getTag();
		if (tag == 0 || tag > 1)
		{
			target->setTag(1);
			showShipModelOrEquipment(m_curModelId, true);
		}
		else
		{
			target->setTag(0);
			showShipModelOrEquipment(m_curModelId, false);
		}
		return;
	}
	int index = 999999;
	if (isButton(button_firsthead) || isButton(button_add_head))//首相
	{
		index = 0;
		m_pEquipIcon = target;
	}
	if (isButton(button_spur_1) || isButton(button_spur_add))//撞角
	{
		index = 1;
		m_pEquipIcon = target;
	}
	
	if (isButton(button_sail_) || isButton(button_add_sail))//船帆
	{
		index = 3;
		m_pEquipIcon = target;
	}
	if (isButton(button_armor_)||isButton(button_add_armor))//甲板
	{
		index = 4;
		m_pEquipIcon = target;
	}
	if (isButton(button_gun_) || isButton(button_add_gun))//火炮
	{
		index = 2;
		m_pEquipIcon = target;
	}
	//int index = target->getTag() - 30;
	if (index < 20)
	{
		m_pTempButton =nullptr;
		m_nGlobalIndex = index;
		switch (m_nGlobalIndex)
		{
		case FIRSTHEAD:
			{
				ProtocolThread::GetInstance()->getEquipableItems(1,0,UILoadingIndicator::create(m_pParent));
				break;
			}
		case GUN_1:
			{
				ProtocolThread::GetInstance()->getEquipableItems(2,0,UILoadingIndicator::create(m_pParent));
				break;
			}
		case GUN_ALL:
			{
				ProtocolThread::GetInstance()->getEquipableItems(7,0,UILoadingIndicator::create(m_pParent));
				break;
			}
		case SAIL_ALL:
			{
				ProtocolThread::GetInstance()->getEquipableItems(4,0,UILoadingIndicator::create(m_pParent));	
				break;
			}
		case ARMOR_ALL:
			{
				ProtocolThread::GetInstance()->getEquipableItems(6,0,UILoadingIndicator::create(m_pParent));	
				break;
			}	
		default:
			break;
		}
	}

}

void UIShipyardEquip::reflushShipInfo()
{
	if (!m_bIsFormationButtonSubmite)
	{
		m_bIsFormationButtonSubmite = false;
		ProtocolThread::GetInstance()->getEquipShipInfo(m_pShipInfoResult->shiplist[m_nMainPressIndex], UILoadingIndicator::create(m_pParent));
	}
	else
	{

	}
}

EquipDetailInfo* UIShipyardEquip::getEquipDetailInfo(const int nId)
{
	if (nId == 0 || !m_pShipInfoResult)
	{
		return nullptr;
	}
	for (auto i = 0; i < m_pShipInfoResult->n_equipdetails;i++)
	{
		if (nId == m_pShipInfoResult->equipdetails[i]->id)
		{
			return m_pShipInfoResult->equipdetails[i];
		}
	}
	for (auto i = 0; i < m_nEquipDetailsNum; i++)
	{
		if(m_pCurEquipdetails[i]->id == nId)
			return m_pCurEquipdetails[i];
	}
	return nullptr;
}

void UIShipyardEquip::flushShipInfo()
{
	auto view = m_pParent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	auto i_dur = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_durable"));
	i_dur->setTouchEnabled(true);
	i_dur->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_dur->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);
	auto i_sai = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_sailor"));
	i_sai->setTouchEnabled(true);
	i_sai->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_sai->setTag(IMAGE_ICON_SAILOR + IMAGE_INDEX2);
	auto i_wei = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_weight"));
	i_wei->setTouchEnabled(true);
	i_wei->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_wei->setTag(IMAGE_ICON_CARGO + IMAGE_INDEX2);
	auto i_sup = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_supply"));
	i_sup->setTouchEnabled(true);
	i_sup->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_sup->setTag(IMAGE_ICON_SUPPLY + IMAGE_INDEX2);
	auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_atk"));
	i_atk->setTouchEnabled(true);
	i_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_atk->setTag(IMAGE_ICON_ATTACK + IMAGE_INDEX2);
	auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_def"));
	i_def->setTouchEnabled(true);
	i_def->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_def->setTag(IMAGE_ICON_DEFENCE + IMAGE_INDEX2);
	auto i_spe = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_speed"));
	i_spe->setTouchEnabled(true);
	i_spe->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_spe->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
	auto i_ste = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_steering"));
	i_ste->setTouchEnabled(true);
	i_ste->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_ste->setTag(IMAGE_ICON_STEERING + IMAGE_INDEX2);


	auto w_gear = dynamic_cast<Widget*>(view->getChildByName("panel_gear"));

	auto t_name = dynamic_cast<Text*>(w_gear->getChildByName<Widget*>("label_ship_name"));
	auto l_durable = dynamic_cast<Widget*>(Helper::seekWidgetByName(w_gear,"panel_ship_durable_num_1"));
	auto t_durable1 = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_ship_durable_num_1"));
	auto t_durable2 = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_ship_durable_num_2"));

	auto t_sailor = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_sailors_num"));
	auto t_sailor_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear, "label_sailors_num_1"));

	auto t_weight = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_capacity_num"));
	auto t_weight_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear, "label_capacity_num_1"));

	auto t_supply = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_suppy_num"));
	auto t_supply_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear, "label_suppy_num_1"));

	auto t_attack = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_attack_num"));
	auto t_defense = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_defense_num"));
	auto t_speed = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_speed_num"));
	auto t_steer = dynamic_cast<Text*>(Helper::seekWidgetByName(w_gear,"label_steering_num"));
	//new ui.....................................
	auto btn_firstHead = dynamic_cast<Button*>(w_gear->getChildByName<Widget*>("button_firsthead"));
	auto i_firstHead = btn_firstHead->getChildByName<ImageView*>("image_equip");
	auto btn_head_add = btn_firstHead->getChildByName<Button*>("button_add_head");
	auto btn_spur = dynamic_cast<Button*>(w_gear->getChildByName<Widget*>("button_spur_1"));
	auto i_gun_1 = btn_spur->getChildByName<ImageView*>("image_equip");
	auto btn_spur_add = btn_spur->getChildByName<Button*>("button_spur_add");

	auto equipDetailSum = getEquipDetailSumInfo();
	std::string s_name;
	if (equipDetailSum->shipname)
	{
		s_name = equipDetailSum->shipname;
		if (s_name.empty() || s_name == " ")
		{
			s_name = getShipName(equipDetailSum->sid);
		}
	}else
	{
		s_name = getShipName(equipDetailSum->sid);
	}
//获取船只ModelId
	m_curModelId = SINGLE_SHOP->getShipData()[equipDetailSum->sid].model_id;
	t_name->setString(s_name);
	setTextColorFormIdAndType(t_name, equipDetailSum->sid, ITEM_TYPE_SHIP);
	t_sailor->setTextColor(Color4B(40, 25, 13, 255));
	t_weight->setTextColor(Color4B(40, 25, 13, 255));
	t_supply->setTextColor(Color4B(40, 25, 13, 255));
	//判断船只属性是否为强化过后的 并将其颜色更改为绿色
	if (equipDetailSum->optional_value)
	{
		std::string  value_optional = equipDetailSum->optional_value;
		std::string str_1 = value_optional.substr(0, value_optional.find("-"));
		std::string str_2 = value_optional.substr(value_optional.find("-") + 1, std::string::npos);

		int value_1 = -1;
		int index_1 = -1;
		int value_2 = -1;
		int index_2 = -1;
		UICommon::getInstance()->getNotZeroFromString(str_1, value_1, index_1, value_2, value_2);
		UICommon::getInstance()->getNotZeroFromString(str_2, value_2, index_2, value_1, value_1);

		if (value_1 >= 0 && index_1 >= 0)
		{
			confirmTextColor(w_gear, index_1 + 1);
		}
		if (value_2 >= 0 && index_2 >= 0)
		{
			confirmTextColor(w_gear, index_2 + 1);
		}

	}


	if (equipDetailSum->maxdurable <  equipDetailSum->definedmaxdurable)
	{   //not normal
		t_durable1->setString(StringUtils::format("%d /",equipDetailSum->durable));
		t_durable2->setString(StringUtils::format(" %d",equipDetailSum->maxdurable));
		t_durable2->setTextColor(Color4B(198,2,5,255));
//chengyuan++
		t_durable1->ignoreContentAdaptWithSize(true);
		t_durable2->ignoreContentAdaptWithSize(true);
		t_durable1->setPositionX(t_durable2->getPositionX() - t_durable2->getContentSize().width);
//
	}else
	{   
		t_durable1->setString(StringUtils::format("%d /",equipDetailSum->durable));
		t_durable2->setString(StringUtils::format(" %d",equipDetailSum->definedmaxdurable));
		t_durable2->setTextColor(Color4B(40,25,13,255));
//chengyuan++
		t_durable1->ignoreContentAdaptWithSize(true);
		t_durable2->ignoreContentAdaptWithSize(true);
		t_durable1->setPositionX(t_durable2->getPositionX() - t_durable2->getContentSize().width);
//
	}
	dynamic_cast<Layout*>(l_durable)->requestDoLayout();
	t_sailor->setString(StringUtils::format(" %d",equipDetailSum->maxsails));
	t_sailor_1->setString(StringUtils::format("%d /", equipDetailSum->sails));
	t_sailor_1->setPositionX(t_sailor->getPositionX() - t_sailor->getContentSize().width);

	t_weight->setString(StringUtils::format(" %d",equipDetailSum->maxcapacity/100));
	t_weight_1->setString(StringUtils::format("%d /",equipDetailSum->capacity / 100));
	t_weight_1->setPositionX(t_weight->getPositionX() - t_weight->getContentSize().width);

	t_supply->setString(StringUtils::format(" %d",equipDetailSum->maxsupply));
	t_supply_1->setString(StringUtils::format("%d /", equipDetailSum->supply));
	t_supply_1->setPositionX(t_supply->getPositionX() - t_supply->getContentSize().width);

	t_attack->setString(String::createWithFormat("%d",equipDetailSum->attack)->_string);
	t_defense->setString(String::createWithFormat("%d",equipDetailSum->defense)->_string);
	t_steer->setString(String::createWithFormat("%d",equipDetailSum->steering)->_string);
	t_speed->setString(String::createWithFormat("%d",equipDetailSum->speed)->_string);

	setBrokenEquipRed(i_firstHead, false);
	auto icon = btn_firstHead->getChildByName("equip_broken_icon");
	if (icon != nullptr)
	{
		icon->removeFromParentAndCleanup(true);
	}
	std::string itemPath;
	if (m_pShipInfoResult->maxbowiconnum < 1)
	{
		btn_firstHead->setTouchEnabled(false);
		btn_firstHead->setVisible(false);
	}else
	{
		btn_firstHead->setTag(m_nCurFirstH_id + START_INDEX);
		btn_firstHead->setVisible(true);
		btn_firstHead->setTouchEnabled(true);
		if(m_nCurFirstH_id != 0)
		{
			itemPath = getItemIconPath(getEquipIID(m_nCurFirstH_id));
			btn_head_add->setVisible(false);
			i_firstHead->setVisible(true);
			i_firstHead->loadTexture(itemPath);
			setBgButtonFormIdAndType(btn_firstHead, getEquipIID(m_nCurFirstH_id), ITEM_TYPE_SHIP_EQUIP);
			if (getEquipDetailInfo(m_nCurFirstH_id)->optionalitemnum > 0)
			{
				addStrengtheningIcon(btn_firstHead);
			}
			if (getEquipDetailInfo(m_nCurFirstH_id)->durable <= SHIP_EQUIP_BROKEN ||
				getEquipDetailInfo(m_nCurFirstH_id)->durable <= getEquipDetailInfo(m_nCurFirstH_id)->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
			{
				addequipBrokenIcon(btn_firstHead);
				setBrokenEquipRed(i_firstHead);
			}
			else
			{
				auto icon = btn_firstHead->getChildByName("equip_broken_icon");
				if (icon != nullptr)
				{
					icon->removeFromParentAndCleanup(true);
					setBrokenEquipRed(i_firstHead, false);
				}
			}
		}
		else
		{
			btn_firstHead->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_firsthead.png");
			btn_head_add->setVisible(true);
			btn_head_add->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func,this));
			btn_head_add->setTag(m_nCurFirstH_id + START_INDEX);
			i_firstHead->setVisible(false);
			auto icon = btn_firstHead->getChildByName("equip_broken_icon");
			if (icon != nullptr)
			{
				icon->removeFromParentAndCleanup(true);
			}
		}
	}

	setBrokenEquipRed(i_gun_1, false);
	auto icon1 = btn_spur->getChildByName("equip_broken_icon");
	if (icon1 != nullptr)
	{
		icon1->removeFromParentAndCleanup(true);
	}
	if (m_pShipInfoResult->maxbowgunnum < 1)
	{
		btn_spur->setTouchEnabled(false);
		btn_spur->setVisible(false);
	
	}else
	{
		btn_spur->setTag(m_nCurGun1_id + START_INDEX);
		btn_spur->setTouchEnabled(true);
		btn_spur->setVisible(true);
		if (m_nCurGun1_id != 0)
		{
			btn_spur_add->setVisible(false);
			itemPath = getItemIconPath(getEquipIID(m_nCurGun1_id));
			i_gun_1->loadTexture(itemPath);
			i_gun_1->setVisible(true);
			setBgButtonFormIdAndType(btn_spur, getEquipIID(m_nCurGun1_id), ITEM_TYPE_SHIP_EQUIP);
			if (getEquipDetailInfo(m_nCurGun1_id)->optionalitemnum > 0)
			{
				addStrengtheningIcon(btn_spur);
			}
			if (getEquipDetailInfo(m_nCurGun1_id)->durable <= SHIP_EQUIP_BROKEN ||
				getEquipDetailInfo(m_nCurGun1_id)->durable <= getEquipDetailInfo(m_nCurGun1_id)->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
			{
				addequipBrokenIcon(btn_spur);
				setBrokenEquipRed(i_gun_1);
			}
			else
			{
				auto icon = btn_spur->getChildByName("equip_broken_icon");
				if (icon != nullptr)
				{
					icon->removeFromParentAndCleanup(true);
					setBrokenEquipRed(i_gun_1, false);
				}
			}
		}
		else
		{
			btn_spur->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_ram.png");
			btn_spur_add->setVisible(true);
			btn_spur_add->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func,this));
			btn_spur_add->setTag(m_nCurGun1_id + START_INDEX);
			i_gun_1->setVisible(false);

			auto icon = btn_spur->getChildByName("equip_broken_icon");
			if (icon != nullptr)
			{
				icon->removeFromParentAndCleanup(true);
			}
		}
	}
	//test
	for (int j = 0; j < 3;j++)//船帆隐藏
	{
		auto btnSails = dynamic_cast<Button*>(Helper::seekWidgetByName(w_gear, (StringUtils::format("button_sail_%d", j + 1))));
		auto i_intensify = Helper::seekWidgetByName(btnSails, "goods_intensify");
		auto image_equip = btnSails->getChildByName<ImageView*>("image_equip");
		setBrokenEquipRed(image_equip, false);
		if (i_intensify)
		{
			i_intensify->removeFromParentAndCleanup(true);
		}
		auto equip_broken_icon = Helper::seekWidgetByName(btnSails, "equip_broken_icon");
		if (equip_broken_icon)
		{
			equip_broken_icon->removeFromParentAndCleanup(true);
		}
		btnSails->setVisible(false);
	}
	for (int j = 0; j < 3;j++)//甲板隐藏
	{
		auto btnArmor = dynamic_cast<Button*>(Helper::seekWidgetByName(w_gear, (StringUtils::format("button_armor_%d", j + 1))));
		auto i_intensify = Helper::seekWidgetByName(btnArmor, "goods_intensify");
		auto image_equip = btnArmor->getChildByName<ImageView*>("image_equip");
		setBrokenEquipRed(image_equip, false);
		if (i_intensify)
		{
			i_intensify->removeFromParentAndCleanup(true);
		}
		auto equip_broken_icon = Helper::seekWidgetByName(btnArmor, "equip_broken_icon");
		if (equip_broken_icon)
		{
			equip_broken_icon->removeFromParentAndCleanup(true);
		}
		btnArmor->setVisible(false);
	}
	for (int j = 0; j < 8; j++)//火炮隐藏
	{
		auto btnGuns = dynamic_cast<Button*>(Helper::seekWidgetByName(w_gear, (StringUtils::format("button_gun_%d",j+1))));
		auto i_intensify = Helper::seekWidgetByName(btnGuns, "goods_intensify");
		auto image_equip = btnGuns->getChildByName<ImageView*>("image_equip");
		setBrokenEquipRed(image_equip, false);
		if (i_intensify)
		{
			i_intensify->removeFromParentAndCleanup(true);
		}
		auto equip_broken_icon = Helper::seekWidgetByName(btnGuns, "equip_broken_icon");
		if (equip_broken_icon)
		{
			equip_broken_icon->removeFromParentAndCleanup(true);
		}
		btnGuns->setVisible(false);
		auto w_range = btnGuns->getChildByName<Widget*>("panel_range");
		for (int j = 0; j < 3; j++)
		{
			auto b_range = w_range->getChildByName<Button*>(StringUtils::format("button_range_%d", j + 1));
			b_range->setBright(true);
		}
	}
	//帆船
	for (int i = 0; i < m_vCurSails_id.size();i++)
	{
		auto btnSails = dynamic_cast<Button*>(Helper::seekWidgetByName(w_gear, (StringUtils::format("button_sail_%d", i+1))));
		btnSails->setTag(m_vCurSails_id[i] + START_INDEX);
		btnSails->setVisible(true);
		auto button_add_sail = btnSails->getChildByName<Button*>("button_add_sail");
		auto image_equip = btnSails->getChildByName<ImageView*>("image_equip");
		if (m_vCurSails_id[i]!=0)
		{			
			button_add_sail->setVisible(false);
			image_equip->setVisible(true);
			itemPath = getItemIconPath(getEquipIID(m_vCurSails_id[i]));
			image_equip->loadTexture(itemPath);
			setBgButtonFormIdAndType(btnSails, getEquipIID(m_vCurSails_id[i]), ITEM_TYPE_SHIP_EQUIP);
			if (getEquipDetailInfo(m_vCurSails_id[i])->optionalitemnum > 0)
			{
				addStrengtheningIcon(btnSails);
			}
			if (getEquipDetailInfo(m_vCurSails_id[i])->durable <= SHIP_EQUIP_BROKEN ||
				getEquipDetailInfo(m_vCurSails_id[i])->durable <= getEquipDetailInfo(m_vCurSails_id[i])->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
			{
				addequipBrokenIcon(btnSails);
				setBrokenEquipRed(image_equip);
			}
		}
		else
		{
			btnSails->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_sails.png");
			button_add_sail->setVisible(true);
			button_add_sail->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func,this));
			button_add_sail->setTag(m_vCurSails_id[i] + START_INDEX);
			image_equip->setVisible(false);

			auto icon = btnSails->getChildByName("equip_broken_icon");
			if (icon != nullptr)
			{
				icon->removeFromParentAndCleanup(true);
			}
		}

	}
	//甲板
	for (int i = 0; i < m_vCurArmors_id.size();i++)
	{
		auto btnArmor = dynamic_cast<Button*>(Helper::seekWidgetByName(w_gear, (StringUtils::format("button_armor_%d", i+1))));
		btnArmor->setTag(m_vCurArmors_id[i] + START_INDEX);
		btnArmor->setVisible(true);
		auto button_add_armor = btnArmor->getChildByName<Button*>("button_add_armor");
		auto image_equip = btnArmor->getChildByName<ImageView*>("image_equip");
		if (m_vCurArmors_id[i] != 0)
		{	
			button_add_armor->setVisible(false);
			image_equip->setVisible(true);
			itemPath = getItemIconPath(getEquipIID(m_vCurArmors_id[i]));
			image_equip->loadTexture(itemPath);
			setBgButtonFormIdAndType(btnArmor, getEquipIID(m_vCurArmors_id[i]), ITEM_TYPE_SHIP_EQUIP);
			if (getEquipDetailInfo(m_vCurArmors_id[i])->optionalitemnum > 0)
			{
				addStrengtheningIcon(btnArmor);
			}
			if (getEquipDetailInfo(m_vCurArmors_id[i])->durable <= SHIP_EQUIP_BROKEN ||
				getEquipDetailInfo(m_vCurArmors_id[i])->durable <= getEquipDetailInfo(m_vCurArmors_id[i])->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
			{
				addequipBrokenIcon(btnArmor);
				setBrokenEquipRed(image_equip);
			}
		}
		else
		{
			btnArmor->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_armor.png");
			button_add_armor->setVisible(true);
			button_add_armor->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func,this));
			button_add_armor->setTag(m_vCurArmors_id[i] + START_INDEX);
			image_equip->setVisible(false);

			auto icon = btnArmor->getChildByName("equip_broken_icon");
			if (icon != nullptr)
			{
				icon->removeFromParentAndCleanup(true);
			}
		}
	}
	//火炮
	for (int i = 0; i < m_vCurGuns_id.size();i++)
	{
		auto btnGuns = dynamic_cast<Button*>(Helper::seekWidgetByName(w_gear, (StringUtils::format("button_gun_%d", i+1))));
		btnGuns->setTag(m_vCurGuns_id[i] + START_INDEX);
		btnGuns->setVisible(true);
		auto button_add_gun = btnGuns->getChildByName<Button*>("button_add_gun");
		auto image_equip = btnGuns->getChildByName<ImageView*>("image_equip");
		if (m_vCurGuns_id[i] != 0)
		{		
			button_add_gun->setVisible(false);
			image_equip->setVisible(true);
			itemPath = getItemIconPath(getEquipIID(m_vCurGuns_id[i]));
			image_equip->ignoreContentAdaptWithSize(false);
			image_equip->loadTexture(itemPath);
			auto w_range = btnGuns->getChildByName<Widget*>("panel_range");
			for (int j = 0; j < 3; j++)
			{
				auto b_range = w_range->getChildByName<Button*>(StringUtils::format("button_range_%d", j + 1));
				if (j < SINGLE_SHOP->getItemData()[getEquipIID(m_vCurGuns_id[i])].property1)
				{
					b_range->setBright(false);
				}
				else
				{
					b_range->setBright(true);
				}
			}
			setBgButtonFormIdAndType(btnGuns, getEquipIID(m_vCurGuns_id[i]), ITEM_TYPE_SHIP_EQUIP);
			if (getEquipDetailInfo(m_vCurGuns_id[i])->optionalitemnum > 0)
			{
				addStrengtheningIcon(btnGuns);
			}
			if (getEquipDetailInfo(m_vCurGuns_id[i])->durable <= SHIP_EQUIP_BROKEN ||
				getEquipDetailInfo(m_vCurGuns_id[i])->durable <= getEquipDetailInfo(m_vCurGuns_id[i])->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
			{
				addequipBrokenIcon(btnGuns);
				setBrokenEquipRed(image_equip);
			}
		}
		else
		{
			btnGuns->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_gun.png");
			button_add_gun->setVisible(true);
			button_add_gun->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func,this));
			button_add_gun->setTag(m_vCurGuns_id[i] + START_INDEX);
			image_equip->setVisible(false);

			auto icon = btnGuns->getChildByName("equip_broken_icon");
			if (icon != nullptr)
			{
				icon->removeFromParentAndCleanup(true);
			}
		}
	
	}
	showShipModelOrEquipment(m_curModelId);
	delete equipDetailSum;
}

void UIShipyardEquip::showEquipInformation()
{
	m_pParent->openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAT_CHANGE_CSB]);
	auto view = m_pParent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAT_CHANGE_CSB]);
	auto n_items = m_vEquips_num.at(m_nGlobalIndex);
	auto items = m_vAllEquipItem.at(m_nGlobalIndex);

	auto cur_equip_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "current_equip_bg"));
	auto cur_image = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_current_equip"));
	auto cur_name = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_current_itemname"));
	auto b_uninstall = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_uninstall"));
	auto b_ok = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_ok"));
	b_ok->setBright(false);
	b_ok->setTouchEnabled(false);
	
	int id = m_pEquipIcon->getTag() - START_INDEX;
	int iid = getEquipIID(id);

	if (iid)
	{
		cur_image->setVisible(true);
		cur_image->loadTexture(getItemIconPath(iid));
		cur_name->setString(apostrophe(getItemName(iid), 6));
		b_uninstall->setBright(true);
		b_uninstall->setTouchEnabled(true);
		setBgButtonFormIdAndType(cur_equip_bg, iid, ITEM_TYPE_SHIP_EQUIP);
		setTextColorFormIdAndType(cur_name, iid, ITEM_TYPE_SHIP_EQUIP);
		cur_equip_bg->setTouchEnabled(true);
		cur_equip_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func,this));
		cur_equip_bg->setTag(iid);
		cur_image->setTag(id);
		auto info = getEquipDetailInfo(id);
		if (getEquipDetailInfo(id)->optionalitemnum > 0)
		{
			addStrengtheningIcon(cur_equip_bg);
		}
		if (getEquipDetailInfo(id)->durable <= SHIP_EQUIP_BROKEN ||getEquipDetailInfo(id)->durable <= getEquipDetailInfo(id)->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
		{
			addequipBrokenIcon(cur_equip_bg);
			setBrokenEquipRed(cur_image);
		}
	}
	else
	{
		cur_image->setVisible(false);
		cur_name->setString("N/A");
		b_uninstall->setBright(false);
		b_uninstall->setTouchEnabled(false);
		cur_equip_bg->setTouchEnabled(false);
	}
	auto t_attackNum = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_attack_num"));
	auto t_defenseNum = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_defense_num"));
	auto lv_left = view->getChildByName<ListView*>("listview_left");
	auto p_bag = lv_left->getItem(0);
	auto i_title = p_bag->getChildByName<ImageView*>("image_icon");
	auto t_title = p_bag->getChildByName<Text*>("label_city");
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BAG"]);
	i_title->setPositionX(t_title->getPositionX() - i_title->getBoundingBox().size.width - t_title->getBoundingBox().size.width / 2);

	auto p_no = view->getChildByName<Widget*>("panel_no");
	if (m_vAllEquips.size() < 1)
	{
		p_no->setVisible(true);
	}else
	{
		p_no->setVisible(false);
	}
	auto w_items = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_items"));

	auto iter = m_vAllEquips.begin();
	for (int i = 0; i < ceil(m_vAllEquips.size()/6.0); i++)
	{
		auto item = w_items->clone();
		for (int j = 0; j < 6; j++)
		{		
			auto i_items = item->getChildByName<ImageView*>(StringUtils::format("image_items_%d",j+1));
			auto i_icon = i_items->getChildByName<ImageView*>("image_goods");
			auto b_item = i_items->getChildByName<Button*>("button_items");
			auto i_item_bg = i_items->getChildByName<ImageView*>("image_item_bg_lv");
			if (i * 6 + j < m_vAllEquips.size())
			{
				if (iter->second < 1)
				{
					continue;
				}
				b_item->setTouchEnabled(true);
				b_item->setTag(iter->first + START_INDEX);
				i_icon->setVisible(true);
				i_icon->loadTexture(getItemIconPath(getEquipIID(iter->first)));
				auto t_num = i_icon->getChildByName<Text*>("text_item_num");
				setTextSizeAndOutline(t_num,iter->second);
				setBgButtonFormIdAndType(i_items, getEquipIID(iter->first), ITEM_TYPE_SHIP_EQUIP);
				setBgImageColorFormIdAndType(i_item_bg, getEquipIID(iter->first), ITEM_TYPE_SHIP_EQUIP);
				if (getEquipDetailInfo(iter->first)->optionalitemnum > 0)
				{
					addStrengtheningIcon(i_items);
				}
				if (getEquipDetailInfo(iter->first)->durable <= SHIP_EQUIP_BROKEN || getEquipDetailInfo(iter->first)->durable <= getEquipDetailInfo(iter->first)->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
				{
					addequipBrokenIcon(i_icon);
					setBrokenEquipRed(i_icon);
				}
				iter++;
			}else
			{
				b_item->setTouchEnabled(false);
				i_icon->setVisible(false);
			}
		}
		lv_left->pushBackCustomItem(item);
	}

	flushEquipInfo();
}

void UIShipyardEquip::flushEquipInfo()
{
	auto view = m_pParent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAT_CHANGE_CSB]);
	auto b_sel_equip = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "select_equip_bg"));
	auto i_sel_equip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_select_equip"));
	auto t_selectName = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_select_itemname"));
	auto i_durable = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_durable"));
	auto b_switch = Helper::seekWidgetByName(view, "button_switch");
	i_durable->setTouchEnabled(true);
	i_durable->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);
	i_durable->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	auto t_durable_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_1"));
	auto t_durable_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_2"));
	
	auto i_att = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_atk"));
	auto t_att = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_attack_num"));
	auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_def"));
	auto t_def = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_defense_num"));

	auto i_pro1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_down"));
	auto t_pro1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_down_num"));
	auto i_pro2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_up"));
	auto t_pro2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_up_num"));
	
	
	auto p_property1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_property_1"));
	auto i_property1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_property_1"));
	auto w_property1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_down"));
	auto p_property2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_property_2"));
	auto i_property2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_property_2"));
	auto w_property2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_up"));
	auto w_range = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_range"));
	auto w_require = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_require"));
	auto t_require = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_require_lv"));
	auto b_ok = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_ok"));
	auto t_deac = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_select_itemname_0"));
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title_sell"));
	
	auto listview_sel = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_p"));
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = view->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	image_pulldown->setVisible(false);
	listview_sel->setVisible(false);
	t_deac->setVisible(false);
	t_deac->setFontSize(22);
	t_deac->setContentSize(Size(340,210));

	switch (m_nGlobalIndex)
	{
	case 0:
	case 1:
		{
			int id = 0;
			if (m_pTempButton)
			{
				id = m_pTempButton->getTag() - START_INDEX;
			}else
			{
				//id = m_pEquipIcon->getTag() - START_INDEX;
			}
			if (id)
			{
				t_deac->setVisible(true);
				if (SINGLE_SHOP->getItemData()[getEquipIID(id)].type == ITEM_TYPE_SHIP_EQUIP && SINGLE_SHOP->getItemData()[getEquipIID(id)].sub_type == SUB_TYPE_SHIP_FIGUREHEAD)
				{
					t_deac->setString(SINGLE_SHOP->getItemData()[getEquipIID(id)].shortdesc);
				}
				else
				{
					t_deac->setString(SINGLE_SHOP->getItemData()[getEquipIID(id)].desc);
				}
			}
			if (m_nGlobalIndex == 0)
			{
				t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_EQUIP_TYPT1"]);
			}
			else
			{
				t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_EQUIP_TYPT2"]);
			}
			break;
		}
	case 2://火炮
		{
			int cur_id = m_pEquipIcon->getTag() - START_INDEX;
			auto curItemDetail = getEquipDetailInfo(cur_id);
			int cur_att = 0;
			//是否有强化影响的属性
			bool isForce = false;
			int cur_spe = 0;
			if (curItemDetail)
			{
				cur_att = curItemDetail->attack;
				cur_spe = curItemDetail->speed;
				if (cur_spe != 0)
				{
					isForce = true;
				}
			}

			int sel_id = 0;
			if (m_pTempButton)
			{
				sel_id = m_pTempButton->getTag() - START_INDEX;
			}
			auto selItemDetail = getEquipDetailInfo(sel_id);
			int sel_att = 0;
			int sel_spe = 0;
			i_att->loadTexture(PROPERTY_ICON_PTAH[ICON_ATTACK]);
			i_def->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
			i_att->setTouchEnabled(true);
			i_att->setTag(ICON_ATTACK +1 + IMAGE_INDEX2);
			i_att->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
			i_def->setTouchEnabled(true);
			i_def->setTag(ICON_SPEED +1 + IMAGE_INDEX2);
			i_def->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
			if (selItemDetail)
			{
				listview_sel->setVisible(true);
				sel_att = selItemDetail->attack;
				sel_spe = selItemDetail->speed;
				if (sel_spe != 0)
				{
					isForce = true;
				}
				int temp_att = sel_att - cur_att;
				int temp_spe = sel_spe - cur_spe;
				t_att->setString(StringUtils::format("%d",sel_att));
				t_def->setString(StringUtils::format("%d", sel_spe));
				if (sel_spe < 0)
				{
					t_def->setTextColor(TEXT_RED);
				}
				
				if (temp_att > 0)
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/up.png");
					t_pro1->setString(StringUtils::format("%d",temp_att));
					t_pro1->setTextColor(Color4B(53, 134, 0, 255));
				}else if (temp_att < 0)
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/down.png");
					t_pro1->setString(StringUtils::format("%d",-temp_att));
					t_pro1->setTextColor(Color4B(254, 21, 25, 255));
				}else
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/same.png");
					t_pro1->setString("0");
					t_pro1->setTextColor(Color4B(106, 106, 106, 255));
				}

				if (temp_spe > 0)
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/up.png");
					t_pro2->setString(StringUtils::format("%d", temp_spe));
					t_pro2->setTextColor(Color4B(53, 134, 0, 255));
				}
				else if (temp_spe < 0)
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/down.png");
					t_pro2->setString(StringUtils::format("%d", -temp_spe));
					t_pro2->setTextColor(Color4B(254, 21, 25, 255));
				}
				else
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/same.png");
					t_pro2->setString("0");
					t_pro2->setTextColor(Color4B(106, 106, 106, 255));
				}
				if (isForce)
				{
					p_property2->setVisible(true);
					listview_sel->setTouchEnabled(false);
					CC_SAFE_RETAIN(p_property2);
					CC_SAFE_RETAIN(w_range);
					listview_sel->removeLastItem();
					listview_sel->removeLastItem();
					listview_sel->pushBackCustomItem(p_property2);
					listview_sel->pushBackCustomItem(w_range);
					CC_SAFE_RELEASE(p_property2);
					CC_SAFE_RELEASE(w_range);
					addListViewBar(listview_sel, image_pulldown);
				}
				else
				{
					p_property2->setVisible(false);
					image_pulldown->setVisible(false);
					listview_sel->setTouchEnabled(false);
					CC_SAFE_RETAIN(p_property2);
					CC_SAFE_RETAIN(w_range);
					listview_sel->removeLastItem();
					listview_sel->removeLastItem();
					listview_sel->pushBackCustomItem(w_range);
					listview_sel->pushBackCustomItem(p_property2);
					CC_SAFE_RELEASE(p_property2);
					CC_SAFE_RELEASE(w_range);
				}
			}

			int sel_range = 0;
			if (m_pTempButton)
			{
				sel_range = SINGLE_SHOP->getItemData()[getEquipIID(sel_id)].property1;
			}
			int cur_iid = getEquipIID(m_pEquipIcon->getTag()-START_INDEX);
			int cur_range = 0;
			if (cur_iid)
			{
				cur_range = SINGLE_SHOP->getItemData()[cur_iid].property1;
			}
			int max_range = sel_range;
			int min_range = cur_range;
			bool range_bool = true;
			if (sel_range < cur_range)
			{
				range_bool = false;
				max_range = cur_range;
				min_range = sel_range;
			}
			if (sel_range == 0)
			{
				min_range = max_range;
			}
			for (int i = 0; i < 3; i++)
			{
				auto range	= dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,StringUtils::format("image_range_%d",i+1)));
				if (i < min_range)
				{
					range->loadTexture("cocosstudio/login_ui/common/range_1.png");
				}else if (i < max_range)
				{
					if (range_bool)
					{
						range->loadTexture("cocosstudio/login_ui/common/range_4.png");
					}else
					{
						range->loadTexture("cocosstudio/login_ui/common/range_3.png");
					}	
				}else
				{
					range->loadTexture("cocosstudio/login_ui/common/range_2.png");
				}
			}
		
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_EQUIP_TYPT3"]);
			break;
		}
	case 3:
		{
			w_range->setVisible(false);
			int cur_id = m_pEquipIcon->getTag() - START_INDEX;
			auto curItemDetail = getEquipDetailInfo(cur_id);
			int cur_att = 0;
			int cur_def = 0;
			if (curItemDetail)
			{
				cur_att = curItemDetail->speed;
				cur_def = curItemDetail->steer_speed;
			}

			int sel_id = 0;
			if (m_pTempButton)
			{
				sel_id = m_pTempButton->getTag() - START_INDEX;
			}
			auto selectItemDetail = getEquipDetailInfo(sel_id);
			int sel_att = 0;
			int sel_def = 0;
			i_att->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
			i_def->loadTexture(PROPERTY_ICON_PTAH[ICON_STEERING]);
			i_att->setTouchEnabled(true);
			i_att->setTag(ICON_SPEED +1 + IMAGE_INDEX2);
			i_att->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));

			i_def->setTouchEnabled(true);
			i_def->setTag(ICON_STEERING +1 + IMAGE_INDEX2);
			i_def->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));

			if (selectItemDetail)
			{
				listview_sel->setVisible(true);
				sel_att = selectItemDetail->speed;
				sel_def = selectItemDetail->steer_speed;
				w_property1->setVisible(true);
				w_property2->setVisible(true);
				int temp_att = sel_att - cur_att;
				t_att->setString(StringUtils::format("%d",sel_att));
				if (temp_att > 0)
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/up.png");
					t_pro1->setString(StringUtils::format("%d",temp_att));
					t_pro1->setTextColor(Color4B(53,134,0,255));
				}else if (temp_att < 0)
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/down.png");
					t_pro1->setString(StringUtils::format("%d",-temp_att));
					t_pro1->setTextColor(Color4B(254,21,25,255));
				}else
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/same.png");
					t_pro1->setString("0");
					t_pro1->setTextColor(Color4B(106, 106, 106, 255));
				}

				int temp_def = sel_def - cur_def;
				t_def->setString(StringUtils::format("%d",sel_def));
				if (temp_def > 0)
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/up.png");
					t_pro2->setString(StringUtils::format("%d",temp_def));
					t_pro2->setTextColor(Color4B(53,134,0,255));
				}else if (temp_def < 0)
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/down.png");
					t_pro2->setString(StringUtils::format("%d",-temp_def));
					t_pro2->setTextColor(Color4B(254,21,25,255));
				}else
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/same.png");
					t_pro2->setString("0");
					t_pro2->setTextColor(Color4B(106, 106, 106, 255));
				}
			}else
			{
				w_property1->setVisible(false);
				t_att->setString(StringUtils::format("%d",cur_att));
				w_property2->setVisible(false);
				t_def->setString(StringUtils::format("%d",cur_def));
			}
			
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_EQUIP_TYPT4"]);
			break;
		}
	case 4:
		{
			w_range->setVisible(false);

			int cur_id = m_pEquipIcon->getTag() - START_INDEX;
			auto curItemDetail = getEquipDetailInfo(cur_id);
			int cur_att = 0;
			//是否有强化影响的属性
			bool isForce = false;
			int cur_spe = 0;
			if (curItemDetail)
			{
				cur_att = curItemDetail->defense;
				cur_spe = curItemDetail->speed;
				if (cur_spe != 0)
				{
					isForce = true;
				}
			}
			int sel_id = 0;
			if (m_pTempButton)
			{
				sel_id = m_pTempButton->getTag() - START_INDEX;
			}
			auto selectItemDetail = getEquipDetailInfo(sel_id);
			int sel_att = 0;
			int sel_spe = 0;
			i_att->loadTexture(PROPERTY_ICON_PTAH[ICON_DEFENSE]);
			i_def->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
			i_att->setTouchEnabled(true);
			i_att->setTag(ICON_DEFENSE +1 + IMAGE_INDEX2);
			i_att->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));

			i_def->setTouchEnabled(true);
			i_def->setTag(ICON_SPEED + 1 + IMAGE_INDEX2);
			i_def->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
			if (selectItemDetail)
			{
				listview_sel->setVisible(true);
				sel_att = selectItemDetail->defense;
				sel_spe = selectItemDetail->speed;
				if (sel_spe != 0)
				{
					isForce = true;
				}
				w_property1->setVisible(true);
				int temp_att = sel_att - cur_att;
				int temp_spe = sel_spe - cur_spe;
				t_att->setString(StringUtils::format("%d",sel_att));
				t_def->setString(StringUtils::format("%d", temp_spe));
				if (sel_spe < 0)
				{
					t_def->setTextColor(TEXT_RED);
				}
				if (temp_att > 0)
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/up.png");
					t_pro1->setString(StringUtils::format("%d",temp_att));
					t_pro1->setTextColor(Color4B(53,134,0,255));
				}else if (temp_att < 0)
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/down.png");
					t_pro1->setString(StringUtils::format("%d",-temp_att));
					t_pro1->setTextColor(Color4B(254,21,25,255));
				}else
				{
					i_pro1->setVisible(true);
					i_pro1->loadTexture("cocosstudio/login_ui/market_720/same.png");
					t_pro1->setString("0");
					t_pro1->setTextColor(Color4B(106, 106, 106, 255));
				}

				if (temp_spe > 0)
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/up.png");
					t_pro2->setString(StringUtils::format("%d", temp_spe));
					t_pro2->setTextColor(Color4B(53, 134, 0, 255));
				}
				else if (temp_spe < 0)
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/down.png");
					t_pro2->setString(StringUtils::format("%d", -temp_spe));
					t_pro2->setTextColor(Color4B(254, 21, 25, 255));
				}
				else
				{
					i_pro2->setVisible(true);
					i_pro2->loadTexture("cocosstudio/login_ui/market_720/same.png");
					t_pro2->setString("0");
					t_pro2->setTextColor(Color4B(106, 106, 106, 255));
				}
			}
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_EQUIP_TYPT5"]);
			listview_sel->setTouchEnabled(false);
			image_pulldown->setVisible(false);
			break;
		}
	default:
		break;
	}

	if (m_pTempButton)
	{
		auto i_intensify = Helper::seekWidgetByName(b_sel_equip, "goods_intensify");
		if (i_intensify)
		{
			i_intensify->removeFromParentAndCleanup(true);
		}
		auto equip_broken_icon = Helper::seekWidgetByName(b_sel_equip, "equip_broken_icon");
		if (equip_broken_icon)
		{
			equip_broken_icon->removeFromParentAndCleanup(true);
		}
		setBrokenEquipRed(i_sel_equip,false);
		w_require->setVisible(true);
		int id = m_pTempButton->getTag()-START_INDEX;
		auto selectItemDetail = getEquipDetailInfo(id);
		t_selectName->setString(apostrophe(getItemName(getEquipIID(id)), 6));
		t_require->setString(StringUtils::format("Lv. %d",selectItemDetail->requiredlv));
		auto parent = dynamic_cast<UIShipyard*>(this->getParent());
		auto equipInfo = SINGLE_SHOP->getItemData()[getEquipIID(id)];
		int n_PcurLevel = parent->getShipOrEquipmentProficiencyLevel(equipInfo.poficiency);
		int n_PneedLevel = 0;
		if (equipInfo.poficiency == 0)
		{
			n_PcurLevel = 0;
		}
		else
		{
			n_PneedLevel = equipInfo.rarity;
		}
		//经验 熟练度 都不够
		if (selectItemDetail->requiredlv > SINGLE_HERO->m_iLevel  && n_PneedLevel > n_PcurLevel)
		{
			b_ok->setBright(false);
			b_ok->setTouchEnabled(true);
			t_require->setTextColor(Color4B(198, 2, 5, 255));
			b_ok->setTag(10);
		}
		//经验不够 熟练够
		else if (selectItemDetail->requiredlv >  SINGLE_HERO->m_iLevel && n_PneedLevel <= n_PcurLevel)
		{
			b_ok->setBright(false);
			b_ok->setTouchEnabled(true);
			t_require->setTextColor(Color4B(198,2,5,255));
			b_ok->setTag(20);
		}
		//经验够 熟练度不够
		else if (selectItemDetail->requiredlv <= SINGLE_HERO->m_iLevel && n_PneedLevel > n_PcurLevel)
		{
			b_ok->setBright(false);
			b_ok->setTouchEnabled(true);
			t_require->setTextColor(Color4B(198, 2, 5, 255));
			b_ok->setTag(30);
		}
		//经验声望都满足
		else if (selectItemDetail->requiredlv <= SINGLE_HERO->m_iLevel && n_PneedLevel <= n_PcurLevel)
		{
			b_ok->setBright(true);
			b_ok->setTouchEnabled(true);
			t_require->setTextColor(Color4B(40, 25, 13, 255));
			b_ok->setTag(40);
		}
		b_sel_equip->setTouchEnabled(true);
		b_sel_equip->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func,this));
		b_sel_equip->setTag(getEquipIID(id));
		i_sel_equip->setVisible(true);
		i_sel_equip->loadTexture(getItemIconPath(getEquipIID(id)));
		i_sel_equip->setTag(id);

		i_durable->setVisible(true);
		t_durable_num_1->setString(StringUtils::format("%d / ", getEquipDetailInfo(id)->durable));
		t_durable_num_2->setString(StringUtils::format("%d", getEquipDetailInfo(id)->maxdurable));
		t_durable_num_1->setPositionX(t_durable_num_2->getPositionX() - t_durable_num_2->getBoundingBox().size.width);

		setBgButtonFormIdAndType(b_sel_equip, getEquipIID(id), ITEM_TYPE_SHIP_EQUIP);
		setTextColorFormIdAndType(t_selectName, getEquipIID(id), ITEM_TYPE_SHIP_EQUIP);

		if (getEquipDetailInfo(id)->optionalitemnum > 0)
		{
			addStrengtheningIcon(b_sel_equip);
		}
		if (getEquipDetailInfo(id)->durable <= SHIP_EQUIP_BROKEN ||
			getEquipDetailInfo(id)->durable <= getEquipDetailInfo(id)->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
		{
			addequipBrokenIcon(b_sel_equip);
			setBrokenEquipRed(i_sel_equip);
		}
	}else
	{
		w_require->setVisible(false);
		t_selectName->setString("N/A");
		b_sel_equip->setTouchEnabled(false);
		i_sel_equip->setVisible(false);
		i_durable->setVisible(false);
	}
}

void UIShipyardEquip::setEquipView(GetEquipableItemsResult* result)
{
	m_pEquipItemResult[m_nGlobalIndex] = result;

	m_vAllEquipItem.at(m_nGlobalIndex) = result->items;
	m_vEquips_num.at(m_nGlobalIndex) = result->n_items;
	
	m_pCurEquipdetails = result->equipdetails;
	m_nEquipDetailsNum = result->n_equipdetails;
	
	pushAllEquipIdAndNum();
	m_pTempButton = nullptr;
	showEquipInformation();
}

void UIShipyardEquip::setShipView(const GetEquipShipInfoResult* result,bool firse)
{
	auto view = m_pParent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	if (!view)
	{
		return;
	}
	dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "button_image_gear_equip_bg"))->setTouchEnabled(false);
	m_pShipsList = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_ships"));
	m_pShipInfoResult = (GetEquipShipInfoResult*)result;

	if (firse)
	{
		m_nMainPressIndex = 0;
		m_PressButton = nullptr;
	}
	auto b_cabin = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_cabin"));
	if (m_nMainPressIndex == 0)
	{
		showShipsList();
		b_cabin->setVisible(true);
		b_cabin->setTag(m_pShipInfoResult->sidlist[0] + 1000);
	}
	else
	{
		b_cabin->setVisible(false);
	}
	changePressButtonState(m_pShipsList->getItem(m_nMainPressIndex));

	m_vCurSails_id.clear();
	m_vCurArmors_id.clear();
	m_vCurGuns_id.clear();
	m_vAllEquipItem.clear();
	m_vFirst_CurArmors_id.clear();
	m_vFirst_CurGuns_id.clear();
	m_vFirst_CurSails_id.clear();
	m_vAllEquipItem.assign(6,nullptr);
	m_vEquips_num.assign(6,0);
	m_vCurSails_id.assign(result->maxrignum,0);
	m_vCurArmors_id.assign(result->maxarmornum,0);
	m_vCurGuns_id.assign(result->maxcannonnum,0);
	m_pCurEquipdetails = nullptr;
	m_nEquipDetailsNum = 0;

	std::string gunIds;
	std::string sailIds;
	std::string armorIds;
	if (result->cannonids == nullptr)
	{
		gunIds = "";
	}else
	{
		gunIds += result->cannonids;
	}
	if (result->spinnakerids == nullptr)
	{
		sailIds = "";
	}else
	{
		sailIds += result->spinnakerids;
	}
	if (result->armorids == nullptr)
	{
		armorIds = "";
	}else
	{
		armorIds += result->armorids;
	}
	
	
	convertToVector(gunIds,m_vCurGuns_id);
	convertToVector(sailIds,m_vCurSails_id);
	convertToVector(armorIds,m_vCurArmors_id);

	m_vFirst_CurSails_id.assign(m_vCurSails_id.begin(),m_vCurSails_id.end());
	m_vFirst_CurGuns_id.assign(m_vCurGuns_id.begin(),m_vCurGuns_id.end());
	m_vFirst_CurArmors_id.assign(m_vCurArmors_id.begin(),m_vCurArmors_id.end());

	m_nCurFirstH_id = m_pShipInfoResult->bowiconid;
	m_nCurGun1_id = m_pShipInfoResult->bowgunid;
	m_nCurGun2_id = m_pShipInfoResult->aftgunid;

	m_nFirst_FirstH_id = m_pShipInfoResult->bowiconid;
	m_nFirst_Gun1_id = m_pShipInfoResult->bowgunid;
	m_nFirst_Gun2_id = m_pShipInfoResult->aftgunid;
	flushShipInfo();
}

void UIShipyardEquip::pushAllEquipIdAndNum()
{
	m_vAllEquips.clear();
	auto func = [&](int first,int curr)
	{
		if (curr == first)
		{
			return;
		}
		if (first != 0)
		{
			auto iter_first = m_vAllEquips.find(first);
			if (iter_first == m_vAllEquips.end())
			{
				m_vAllEquips[first] = 1;
			}else
			{
				m_vAllEquips[first]++;
			}
		}
		if (curr == 0)
		{
			return;
		}
		auto iter = m_vAllEquips.find(curr);
		if (iter != m_vAllEquips.end())
		{
			m_vAllEquips[curr]--;
			if (m_vAllEquips[curr] < 1)
			{
				m_vAllEquips.erase(curr);
			}
		}else
		{
			log("error ! find equip..");
			return;
		}
		
	};

	auto func_2 = [&](std::vector<int>& first,std::vector<int>& curr){
		for (auto i = 0;i < first.size();i++)
		{
			func(first[i],curr[i]);
		}
	};
	// Not equipped with equipment ,storage 
	for (auto i = 0; i < m_pEquipItemResult[m_nGlobalIndex]->n_items; i++)
	{
		m_vAllEquips[m_pEquipItemResult[m_nGlobalIndex]->items[i]->id] = m_pEquipItemResult[m_nGlobalIndex]->items[i]->num;
	}
	
	switch (m_nGlobalIndex)
	{
	case FIRSTHEAD:
		{
			func(m_nFirst_FirstH_id,m_nCurFirstH_id);
			break;
		}
	case GUN_1:
		{
			func(m_nFirst_Gun1_id,m_nCurGun1_id);
			break;
		}
	case GUN_ALL:
		{
			func_2(m_vFirst_CurGuns_id,m_vCurGuns_id);
			break;
		}
	case SAIL_ALL:
		{
			func_2(m_vFirst_CurSails_id,m_vCurSails_id);
			break;
		}
	case ARMOR_ALL:
		{
			func_2(m_vFirst_CurArmors_id,m_vCurArmors_id);
			break;
		}
	case GUN_TAIL:
		{
			func(m_nFirst_Gun2_id,m_nCurGun2_id);
			break;
		}
	default:
		break;
	}	
}

int UIShipyardEquip::getEquipIID(const int nId)
{

	for (auto i = 0; i < m_pShipInfoResult->n_equipdetails;i++)
	{
		if (nId == m_pShipInfoResult->equipdetails[i]->id)
		{
			return m_pShipInfoResult->equipdetails[i]->iid;
		}
	}
	if (!m_pEquipItemResult[m_nGlobalIndex])
	{
		return 0;
	}
	int sum = m_pEquipItemResult[m_nGlobalIndex]->n_items;
	auto items = m_pEquipItemResult[m_nGlobalIndex]->items;
	for (int i = 0; i < sum; i++)
	{
		if (nId == items[i]->id)
		{
			return items[i]->iid;
		}
	}
	return 0;
}

void UIShipyardEquip::submitDataToserver()
{
	int index = 0;
	if (m_PressButton)
	{
		index = m_pShipsList->getIndex(m_PressButton);
	}else
	{
		//openSuccessOrFailDialog("Submit Data Error!",this->m_pParent);
		return;
	}
	int shipId = m_pShipInfoResult->shiplist[index];
	std::string gunIds = convertToString(m_vCurGuns_id);
	std::string sailIds = convertToString(m_vCurSails_id);
	std::string armorIds = convertToString(m_vCurArmors_id);
	std::string allIds = StringUtils::format("%d,%d,%d,",m_nCurFirstH_id,m_nCurGun1_id,m_nCurGun2_id) +","+ sailIds +","+ armorIds + ","+ gunIds;
	ProtocolThread::GetInstance()->equipShip(shipId,m_nCurFirstH_id,m_nCurGun1_id,m_nCurGun2_id,
		(char*)sailIds.c_str(),(char*)armorIds.c_str(),(char*)gunIds.c_str(),UILoadingIndicator::create(m_pParent));
}

EquipShipInfo* UIShipyardEquip::getEquipDetailSumInfo()
{
	auto equipDetailSum = new EquipShipInfo;
	memcpy(equipDetailSum,m_pShipInfoResult->shipinfo,sizeof(EquipShipInfo));

	auto itemEquip = getEquipDetailInfo(m_nCurFirstH_id);
	if (itemEquip)
	{
		equipDetailSum->attack += itemEquip->attack;
		equipDetailSum->defense += itemEquip->defense;
		equipDetailSum->speed += itemEquip->speed;
		equipDetailSum->steering += itemEquip->steer_speed;
	}
	itemEquip = getEquipDetailInfo(m_nFirst_FirstH_id);
	if (itemEquip)
	{
		equipDetailSum->attack -= itemEquip->attack;
		equipDetailSum->defense -= itemEquip->defense;
		equipDetailSum->speed -= itemEquip->speed;
		equipDetailSum->steering -= itemEquip->steer_speed;
	}


	itemEquip = getEquipDetailInfo(m_nCurGun1_id);
	if (itemEquip)
	{
		equipDetailSum->attack += itemEquip->attack;
		equipDetailSum->defense += itemEquip->defense;
		equipDetailSum->speed += itemEquip->speed;
		equipDetailSum->steering += itemEquip->steer_speed;
	}
	itemEquip = getEquipDetailInfo(m_nFirst_Gun1_id);
	if (itemEquip)
	{
		equipDetailSum->attack -= itemEquip->attack;
		equipDetailSum->defense -= itemEquip->defense;
		equipDetailSum->speed -= itemEquip->speed;
		equipDetailSum->steering -= itemEquip->steer_speed;
	}

	for (int i = 0; i < m_vCurSails_id.size(); i++)
	{
		itemEquip = getEquipDetailInfo(m_vCurSails_id[i]);
		if (itemEquip)
		{
			equipDetailSum->attack += itemEquip->attack;
			equipDetailSum->defense += itemEquip->defense;
			equipDetailSum->speed += itemEquip->speed;
			equipDetailSum->steering += itemEquip->steer_speed;
		}

		itemEquip = getEquipDetailInfo(m_vFirst_CurSails_id[i]);
		if (itemEquip)
		{
			equipDetailSum->attack -= itemEquip->attack;
			equipDetailSum->defense -= itemEquip->defense;
			equipDetailSum->speed -= itemEquip->speed;
			equipDetailSum->steering -= itemEquip->steer_speed;
		}
	}

	for (int i = 0; i < m_vCurGuns_id.size(); i++)
	{
		itemEquip = getEquipDetailInfo(m_vCurGuns_id[i]);
		if (itemEquip)
		{
			equipDetailSum->attack += itemEquip->attack;
			equipDetailSum->defense += itemEquip->defense;
			equipDetailSum->speed += itemEquip->speed;
			equipDetailSum->steering += itemEquip->steer_speed;
		}

		itemEquip = getEquipDetailInfo(m_vFirst_CurGuns_id[i]);
		if (itemEquip)
		{
			equipDetailSum->attack -= itemEquip->attack;
			equipDetailSum->defense -= itemEquip->defense;
			equipDetailSum->speed -= itemEquip->speed;
			equipDetailSum->steering -= itemEquip->steer_speed;
		}
	}

	for (int i = 0; i < m_vCurArmors_id.size(); i++)
	{
		itemEquip = getEquipDetailInfo(m_vCurArmors_id[i]);
		if (itemEquip)
		{
			equipDetailSum->attack += itemEquip->attack;
			equipDetailSum->defense += itemEquip->defense;
			equipDetailSum->speed += itemEquip->speed;
			equipDetailSum->steering += itemEquip->steer_speed;
		}

		itemEquip = getEquipDetailInfo(m_vFirst_CurArmors_id[i]);
		if (itemEquip)
		{
			equipDetailSum->attack -= itemEquip->attack;
			equipDetailSum->defense -= itemEquip->defense;
			equipDetailSum->speed -= itemEquip->speed;
			equipDetailSum->steering -= itemEquip->steer_speed;
		}
	}
	return equipDetailSum;
}

void UIShipyardEquip::changeVectorContent(bool bIsAddEquip)
{
	dynamic_cast<UIShipyard*>(m_pParent)->setShipEquipFinishData();
	int equipIndex = 0;
	int temp = 0;
	if(bIsAddEquip)
	{
		if (m_pTempButton)
		{
			equipIndex = m_pTempButton->getTag()-START_INDEX;
		}else
		{
			return;
		}
		for (auto iter = m_vAllEquips.begin(); iter != m_vAllEquips.end(); iter++)
		{
			if (iter->first == equipIndex)
			{
				iter->second -= 1;
				if (iter->second < 1)
				{
					m_vAllEquips.erase(iter);
					break;
				}
			}
		}
	}else
	{
		for (auto iter = m_vAllEquips.begin(); iter != m_vAllEquips.end(); iter++)
		{
			if (iter->first == equipIndex)
			{
				iter->second += 1;
				break;
			}
			if (iter == m_vAllEquips.end())
			{
				m_vAllEquips.insert(std::pair<int,int>(equipIndex,1));
			}
		}
	}
	switch (m_nGlobalIndex)
	{
	case FIRSTHEAD:
		{
			m_nCurFirstH_id = equipIndex;
			break;
		}
	case GUN_1:
		{
			m_nCurGun1_id = equipIndex;
			break;
		}
	case GUN_ALL:
		{
			int index = m_pEquipIcon->getTag() - START_INDEX;
			
			bool isfind = false;
			for (int i = 0; i < m_vCurGuns_id.size(); i++)
			{
				if (m_vCurGuns_id[i] == index)
				{
					temp = i;
					isfind = true;
					if (index == 0)
					{
						break;
					}
				}
			}

			if (!isfind)
			{
				for (int i = 0; i < m_vCurGuns_id.size(); i++)
				{
					if (m_vCurGuns_id[i] == 0)
					{
						temp = i;
						break;
					}
				}
			}
			
			m_vCurGuns_id.at(temp) = equipIndex;
			break;
		}
	case SAIL_ALL:
		{
			int index = m_pEquipIcon->getTag() - START_INDEX;
		
			bool isfind = false;
			for (int i = 0; i < m_vCurSails_id.size(); i++)
			{
				if (m_vCurSails_id[i] == index)
				{
					temp = i;
					isfind = true;
					if (index == 0)
					{
						break;
					}
				}
			}

			if (!isfind)
			{
				for (int i = 0; i < m_vCurSails_id.size(); i++)
				{
					if (m_vCurSails_id[i] == 0)
					{
						temp = i;
						break;
					}
				}
			}
			
			m_vCurSails_id.at(temp) = equipIndex;
			break;
		}
	case ARMOR_ALL:
		{	
			int index = m_pEquipIcon->getTag() - START_INDEX;
			
			bool isfind = false;
			for (int i = 0; i < m_vCurArmors_id.size(); i++)
			{
				if (m_vCurArmors_id[i] == index)
				{
					isfind = true;
					temp = i;
					if (index == 0)
					{
						break;
					}
				}
			}

			if (!isfind)
			{
				for (int i = 0; i < m_vCurArmors_id.size(); i++)
				{
					if (m_vCurArmors_id[i] == 0)
					{
						temp = i;
						break;
					}
				}
			}
			
			m_vCurArmors_id.at(temp) = equipIndex;
			break;
		}
	default:
		break;
	}

	flushShipInfo();
}

void UIShipyardEquip::confirmTextColor(Widget * view, int index)
{
	auto t_durable_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_2"));
	auto t_weight_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_capacity_num"));
	auto t_sailor_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_sailors_num"));
	auto t_supply_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_suppy_num"));


	auto t_attack = view->getChildByName<Text*>("label_attack_num");
	auto t_defense = view->getChildByName<Text*>("label_defense_num");
	auto t_speed = view->getChildByName<Text*>("label_speed_num");
	auto t_steer = view->getChildByName<Text*>("label_steering_num");
	switch (index)
	{
	case 1:
		t_defense->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 2:
		t_durable_1->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 3:
		t_speed->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 4:
		t_steer->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 5:
		t_sailor_1->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 6:
		t_weight_1->setTextColor(Color4B(1, 120, 5, 255));
		break;
	default:
		break;
	}
}
void UIShipyardEquip::flushRepairEquipmentView(EquipmentDefine* equipmentInfo)
{
	auto info = getEquipDetailInfo(equipmentInfo->uniqueid);
	info->durable = equipmentInfo->durable;
	if (m_curItem != nullptr)
	{
		
		auto parent = m_curItem->getParent();
		auto child = parent->getChildByName("equip_broken_icon");
		if (child != nullptr)
		{
			child->setVisible(false);
			child->removeFromParentAndCleanup(true);
			setBrokenEquipRed(m_curItem, false);
		}
	}
	if (m_pTempButton != nullptr)
	{
		auto i_p_temp = dynamic_cast<ImageView*>(m_pTempButton->getParent());
		auto child = i_p_temp->getChildByName<ImageView*>("image_goods");
		auto child_icon = i_p_temp->getChildByName("equip_broken_icon");
		if (child_icon != nullptr)
		{
			child_icon->setVisible(false);
			child_icon->removeFromParentAndCleanup(true);
			setBrokenEquipRed(child, false);
		}

	}
	flushShipInfo();
	m_curItem = nullptr;
}

void UIShipyardEquip::showShipModelOrEquipment(int modelId, bool showmodel)
{
	auto view = m_pParent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	auto i_equipBg = Helper::seekWidgetByName(view, "button_image_gear_equip_bg");
	auto i_modelBg = Helper::seekWidgetByName(view, "button_image_gear_equip_bg_2");
	auto p_shipEquip = Helper::seekWidgetByName(view, "panel_shipquip");
	auto i_3Dbg = Helper::seekWidgetByName(view, "image_3d_bg");
	auto b_switch = Helper::seekWidgetByName(view, "button_switch");
	b_switch->addTouchEventListener(CC_CALLBACK_2(UIShipyardEquip::menuCall_func, this));
	i_modelBg->setTouchEnabled(false);
	i_equipBg->setTouchEnabled(false);
	auto instance = ModeLayerManger::getInstance();
	auto layer = instance->getModelLayer();
	if (showmodel)
	{
		i_equipBg->setVisible(false);
		p_shipEquip->setVisible(false);
		i_modelBg->setVisible(true);
		i_3Dbg->setVisible(true);
		showShipModel(modelId);
		dynamic_cast<Button*>(b_switch)->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_SWITCH_SHIP_PARTS"]);
		UISocial::getInstance()->showChat(1);
		if (instance->getModelLayer() != nullptr)
		{
			instance->showModelLayer(true);
			instance->getModelLayer()->setTag(1);
		}
	}
	else
	{
		i_equipBg->setVisible(true);
		p_shipEquip->setVisible(true);
		i_modelBg->setVisible(false);
		i_3Dbg->setVisible(false);
		b_switch->setTag(0);
		dynamic_cast<Button*>(b_switch)->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_SWITCH_SHIP"]);
//		UISocial::getInstance()->showChat(1);
		if (instance->getModelLayer() != nullptr)
		{
			instance->showModelLayer(false);
			instance->getModelLayer()->setTag(0);
		}
	}
}
void UIShipyardEquip::showShipModel(int modelId)
{
// 	auto scene = _director->getRunningScene();
// 	Layer * shiplayer = dynamic_cast<Layer*>(scene->getChildByName("shipLayer"));
// 	if (m_operaModel != nullptr)
// 	{
// 		int tag = m_operaModel->getTag();
// 		if (tag == modelId)
// 		{
// 			if (shiplayer != nullptr)
// 			{
// 				shiplayer->setVisible(true);
// 				return;
// 			}			
// 		}
// 		else
// 		{
// 			m_operaModel->removeFromParentAndCleanup(true);
// 		}
// 	}

	auto view = m_pParent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	auto i_3Dbg = Helper::seekWidgetByName(view, "image_3d_bg");
// 
// 
// 
// 	Node * node = nullptr;
// 	Layer * layer = nullptr;
// 	if (shiplayer != nullptr)
// 	{
// 		shiplayer->setVisible(true);
// 		layer = shiplayer;
// 		node = layer->getChildByName("shipNode");
// 	}
// 	else{
// 		layer = Layer::create();
// 		layer->setName("shipLayer");
// 		layer->setTag(1);
// 		node = Node::create();
// 		node->setName("shipNode");
// 		layer->addChild(node);
// 		scene->addChild(layer, 10000);
// 		//船只camera
// 		Size s = CCDirector::getInstance()->getWinSize();
// 		auto m_camera = Camera::createPerspective(60, (GLfloat)s.width / s.height, 1, 1000);
// 		m_camera->setCameraFlag(CameraFlag::USER1);
// 		Vec3 shipPos = Vec3(0, 0, 0);
// 		m_camera->lookAt(shipPos, Vec3(0, 1, 0));
// 		m_camera->setRotation3D(Vec3(0, 0, 0));
// 		m_camera->setPosition3D(Vec3(0.4, 2.5, 6.5));
// 		m_camera->setDepth(2);
// 		node->addChild(m_camera);
// 		if (this->getCameraMask() != 4)
// 		{
// 			//界面Camera 管理loadingLayer
// 			auto size = Director::getInstance()->getWinSize();
// 			Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
// 			popCam->setCameraFlag(CameraFlag::USER2);
// 			popCam->setDepth(4);
// 			layer->addChild(popCam);
// 		}
// 		auto listener = EventListenerTouchOneByOne::create();
// 		listener->onTouchBegan = CC_CALLBACK_2(UIShipyardEquip::onTouchBegan, this);
// 		listener->onTouchMoved = CC_CALLBACK_2(UIShipyardEquip::onTouchMoved, this);
// 		listener->onTouchEnded = CC_CALLBACK_2(UIShipyardEquip::onTouchEnded, this);
// 		_director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, layer);
// 	}
// 	auto ship = ShipModel::create(modelId);
// 	ship->setCameraMask(2);
// 	ship->setScale(0.4);
// 	ship->setName("Ship");
// 	ship->setForceDepthWrite(true);
// 	ship->setTag(modelId);
// 	m_operaModel = ship;
// 	auto position = i_3Dbg->getWorldPosition();
// 	node->setPosition(i_3Dbg->getWorldPosition());
// 	node->addChild(ship);
// 	auto size = i_3Dbg->getContentSize();
// 	ship->setPosition(Vec2(3.8, 0.5));
// 	ship->setRotation3D(Vec3(0, -65, 0));
	auto instance = ModeLayerManger::getInstance();
	instance->openModelLayer();
	instance->addShip(i_3Dbg,modelId,DOCK_SHIP);

}
bool UIShipyardEquip::onTouchBegan(Touch *touch, Event *unused_event)
{	
	if (touch->getLocation().x >= 730 && touch->getLocation().x <= 1230 && touch->getLocation().y >= 80 && touch->getLocation().y <= 480)
	{
		m_touchBegan = touch->getLocation();
	}
	return true;
}
void UIShipyardEquip::onTouchMoved(Touch *touch, Event *unused_event)
{
	Point p1 = touch->getLocation();
	if (m_touchBegan != Vec2(0, 0))
	{
		if (p1.x - m_touchBegan.x > 0)
		{
			m_operaModel->setRotation3D(m_operaModel->getRotation3D() + Vec3(0, 5, 0));
		}
		else if (p1.x - m_touchBegan.x < 0)
		{
			m_operaModel->setRotation3D(m_operaModel->getRotation3D() + Vec3(0, -5, 0));
		}
		m_touchBegan = p1;
		log("hehe");
		Vec3 pos = m_operaModel->getPosition3D();
		Vec3 roat = m_operaModel->getRotation3D();
		log("x ::%.2f, y :: %.2f , z :: %.2f", pos.x, pos.y, pos.z);
		log("x ::%.2f, y :: %.2f , z :: %.2f", roat.x, roat.y, roat.z);
	}
}
void UIShipyardEquip::onTouchEnded(Touch *touch, Event *unused_event)
{
	m_touchBegan = Vec2(0, 0);
}

void UIShipyardEquip::resetModel()
{
	m_operaModel = nullptr;
}