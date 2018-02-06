#include "UINationWarMap.h"
#include "WarManage.h"
#include "UINationWarShip.h"
#include "EffectManage.h"
#include "UINationWarHUD.h"

UINationWarMap::UINationWarMap()
{
	m_eUIType = UI_COUNTRY_WAR;
	m_pCityButton = nullptr;
	m_pParent = nullptr;
}

UINationWarMap::~UINationWarMap()
{

}

UINationWarMap* UINationWarMap::createWarMapLayer(UIBasicLayer* parent)
{
	UINationWarMap* wml = new UINationWarMap;
	if (wml && wml->init())
	{
		wml->m_pParent = parent;
		wml->autorelease();
		return wml;
	}
	CC_SAFE_DELETE(wml);
	return nullptr;
}

bool UINationWarMap::init()
{
	bool pRet = false;
	do
	{
		this->setContentSize(Size(MAP_HEIGH * MAP_CELL_HEIGH, MAP_WIDTH * MAP_CELL_WIDTH));
		auto cache = AnimationCache::getInstance();
		Animation *an = cache->getAnimation("waterAnimation");
		if (an == NULL) {
			SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
			Director::getInstance()->getTextureCache()->removeUnusedTextures();
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/test/battlewater.plist");
			Vector<SpriteFrame*> sfs(120);
			for (int i = 0; i < 120; i++) {

				SpriteFrame *sf = SpriteFrameCache::getInstance()->getSpriteFrameByName(String::createWithFormat("water%04d.png", i)->getCString());
				sfs.pushBack(sf);
			}
			an = Animation::createWithSpriteFrames(sfs, 1.f / 30, -1);
			cache->addAnimation(an, "waterAnimation");
		}

		auto centerPoint = Director::getInstance()->getWinSize() / 2;
		/*
		//玩家船只居中
		auto ship_position = SINGLE_SHOP->getCityWarPositionInfo().find(SINGLE_HERO->m_nAttackCityId)->second.position;
		int city_x = -ship_position[0].x + centerPoint.width;
		int city_y = -ship_position[0].y + centerPoint.height;
		*/
		//城市居中
		int city_x = -SINGLE_SHOP->getCitiesInfo().find(SINGLE_HERO->m_nAttackCityId)->second.x + centerPoint.width;
		int city_y = -SINGLE_SHOP->getCitiesInfo().find(SINGLE_HERO->m_nAttackCityId)->second.y + centerPoint.height;
	
		Point m_ScreenFirstPos = Vec2(-city_x, -city_y) / MAP_CELL_WIDTH;
		m_ScreenFirstPos = Vec2(int(m_ScreenFirstPos.x), int(m_ScreenFirstPos.y));
		int firstX = m_ScreenFirstPos.x - SCREEN_WIDTH_NUM / 2;
		int firstY = m_ScreenFirstPos.y - SCREEN_HEIGH_NUM / 2;
		this->setPosition(city_x, city_y);

		for (int i = 0; i < SCREEN_HEIGH_NUM * 2; i++)
		{
			for (int j = 0; j < 2 * SCREEN_WIDTH_NUM; j++)
			{
				std::string fileName = getCellName(firstX + j, firstY + i);
				auto mapCell = Sprite::create();
				mapCell->setTexture(fileName);
				auto bg = Sprite::create();
				bg->setAnchorPoint(Vec2(0, 0));
				int randomCount = cocos2d::random(0, 60);
				Animation* temp_an = an->clone();
				getRandomAnimations(randomCount, temp_an);
				bg->runAction(Animate::create(temp_an));

				mapCell->setAnchorPoint(Vec2(0, 0));
				mapCell->addChild(bg, -2, 2);
				this->addChild(mapCell, 1);
				mapCell->setPosition(MAP_CELL_WIDTH * (firstX + j), MAP_CELL_HEIGH*(firstY + i));
			}
		}

		initCityData();
		pRet = true;
	} while (0);

	return pRet;
}

void UINationWarMap::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* widget = dynamic_cast<Widget*>(pSender);
		std::string name = widget->getName();

		if (isButton(button_close))
		{
			return;
		}
	}
}

std::string UINationWarMap::getCellName(int x, int y)
{
	y = MAP_WIDTH - y - 1;
	if (x > MAP_HEIGH - 1 || x < 0)
	{
		return StringUtils::format("res/map_cell/land/1.png");
	}
	if (y > MAP_WIDTH - 1 || y < 0)
	{
		return StringUtils::format("res/map_cell/land/1.png");
	}
	int index = MAPINDEX[y][x];
	return StringUtils::format("res/map_cell/land/%d.png", MAPINDEX[y][x]);
}

void UINationWarMap::getRandomAnimations(int randomCount, Animation* ani)
{
	Vector<AnimationFrame*> &sprites = (Vector<AnimationFrame*>&)(ani->getFrames());
	for (int i = 0; i < randomCount; i++)
	{
		AnimationFrame* tempFrames = sprites.at(0);
		tempFrames->retain();
		sprites.erase(0);
		sprites.pushBack(tempFrames);
		tempFrames->release();
	}
}


void UINationWarMap::initCityData()
{
	auto cityId = SINGLE_HERO->m_nAttackCityId;
	std::string path = StringUtils::format("res/city_icon/nation_war_%d.png", 1);
	auto citysInfo = SINGLE_SHOP->getCitiesInfo();
	m_pCityButton = Button::create(path);
	m_pCityButton->setPosition(Vec2(citysInfo[cityId].x, citysInfo[cityId].y));
	m_pCityButton->setAnchorPoint(Vec2(0.5, 0.5));
	m_pCityButton->setName("button_city");
	m_pCityButton->setTouchEnabled(false);
	auto i_bg = ImageView::create("res/city_icon/cityname_bg.png");
	i_bg->setPosition(Vec2(citysInfo[cityId].x, citysInfo[cityId].y - m_pCityButton->getContentSize().height*0.4));
	auto t_city = Text::create();
	t_city->setString(citysInfo[cityId].name);
	t_city->setPosition(Vec2(i_bg->getContentSize().width / 2, i_bg->getContentSize().height*0.4));
	t_city->setColor(Color3B(255, 191, 38));
	t_city->setFontSize(20);
	this->addChild(m_pCityButton, 1, cityId);
	this->addChild(i_bg, 2);
	i_bg->addChild(t_city);
}

Button* UINationWarMap::getCityImage()
{
	return m_pCityButton;
}

void UINationWarMap::updateBySecond(const float fTime)
{
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(fTime), CCCallFunc::create(CC_CALLBACK_0(UINationWarMap::fired, this))));
}

void UINationWarMap::fired()
{
	auto shipFleet = dynamic_cast<WarManage*>(m_pParent)->m_vWarShip.size();
	if (shipFleet == 1)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FIRE_CANNON_07);
	}
	else
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FIRE_CANNON_08);
	}
	for (size_t i = 0; i < shipFleet; i++)
	{
		this->runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create(CC_CALLBACK_0(UINationWarMap::hurt, this, i)), nullptr));
	
		auto warShip = dynamic_cast<WarManage*>(m_pParent)->getWarShip(i);
		auto animation = dynamic_cast<WarManage*>(m_pParent)->getEffectManage()->getAnimation(PT_FIRE_EFFECT);
		auto eff = Sprite::create();
		eff->runAction(Sequence::createWithTwoActions(Animate::create(animation->clone()), RemoveSelf::create(true)));
		int rx = cocos2d::random(int(m_pCityButton->getContentSize().width / 2 - 20), int(m_pCityButton->getContentSize().width / 2 + 20));
		int ry = cocos2d::random(int(m_pCityButton->getContentSize().height / 2 - 20), int(m_pCityButton->getContentSize().height / 2 + 20));
		eff->setPosition(Vec2(rx, ry));
		eff->setAnchorPoint(Vec2(0.25, 0.5));
		eff->setScale(0.5f);
		eff->setRotation(warShip->getShipRotation() - 180);
		m_pCityButton->addChild(eff);
	}
}

void UINationWarMap::hurt(const int tag)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CANNON_HIT_29);
	auto warShip = dynamic_cast<WarManage*>(m_pParent)->getWarShip(tag);
	auto ship_icon = warShip->getShipIcon();

	int rx = cocos2d::random(int(ship_icon->getContentSize().width / 2 - 10), int(ship_icon->getContentSize().width / 2 + 10));
	int ry = cocos2d::random(int(ship_icon->getContentSize().width / 2 + 10), int(ship_icon->getContentSize().height / 2 + 30));
	auto animation_explosion = dynamic_cast<WarManage*>(m_pParent)->getEffectManage()->getAnimation(PT_EXPLOSION);
	auto animateEff = Animate::create(animation_explosion->clone());
	auto eff = Sprite::create();
	ship_icon->addChild(eff);
	eff->setAnchorPoint(Vec2(0.5, 0.5));
	eff->setPosition(Vec2(rx, ry));
	eff->setScale(0.5f);
	eff->runAction(Sequence::createWithTwoActions(animateEff, RemoveSelf::create(true)));

	int height = 60;
	int randomCount = cocos2d::random(-15, 15);
	int attack = warShip->getShipFleetInfo()->get_hurt;
	const char* formatString = attack > 0 ? "/%d" : "%d";
	std::string s_capNums = StringUtils::format(formatString, attack);
	TextAtlas *capNums = TextAtlas::create(s_capNums, IMAGE_FONT[1], 20, 26, "/");

	capNums->setPosition(Vec2(rx, ry));
	ship_icon->addChild(capNums, 50);
	auto seq_1 = Sequence::create(DelayTime::create(1.8f), FadeOut::create(0.3f), nullptr);
	auto easeAct = EaseQuinticActionOut::create(Spawn::createWithTwoActions(MoveBy::create(1.5f, Vec2(0, height)), seq_1));
	auto seq = Sequence::createWithTwoActions(easeAct, RemoveSelf::create(true));
	capNums->runAction(seq);

	if (warShip->getShipFleetType() == MY_SHIP_FLEET)
	{
		auto warUI = dynamic_cast<WarManage*>(m_pParent)->getWarUI();
		for (size_t i = 0; i < warShip->getShipFleetInfo()->n_ships; i++)
		{
			warUI->cityAttackShip(warShip->getShipFleetInfo()->ships[i]->ship_hurt, i);
		}
	}
	else
	{
		bool is_die = true;
		auto shipInfo = warShip->getShipFleetInfo();
		for (size_t i = 0; i < shipInfo->n_ships; i++)
		{
			//当前血量就是本回合后的血量
			//if (shipInfo->ships[i]->hp - shipInfo->ships[i]->ship_hurt > 0)
			if (shipInfo->ships[i]->hp > 0)
			{
				is_die = false;
			}
		}

		//舰队死亡
		if (is_die)
		{
			warShip->shipDied();
		}
	}

	//回合结束后判断城市是否被其他玩家攻破
	if (dynamic_cast<WarManage*>(m_pParent)->m_vWarShip.size() == tag + 1)
	{
		auto warUI = dynamic_cast<WarManage*>(m_pParent)->getWarUI();
		auto cityInfo = warUI->getCityStatus();
		auto battleTurn = dynamic_cast<WarManage*>(m_pParent)->getBattleTurnResult();
		if (battleTurn->failed == 3)
		{
			cityInfo->start_hp = 0;
			warUI->updateCityDuable(true);
		}
	}
}

void UINationWarMap::countryWarStop()
{
	this->stopAllActions();
}

void UINationWarMap::updateCityButton(int duable, int max_duable)
{
	int index = 0;
	int percent = 100.0 * duable / max_duable;
	if (percent >= 80)
	{
		index = 1;
	}
	else if (percent >= 60)
	{
		index = 2;
	}
	else if (percent >= 40)
	{
		index = 3;
	}
	else if (percent >= 20)
	{
		index = 4;
	}
	else 
	{
		index = 5;
	}
	std::string path = StringUtils::format("res/city_icon/nation_war_%d.png", index);
	m_pCityButton->loadTextureNormal(path);
}