#include "EffectManage.h"
#include "cocostudio/DictionaryHelper.h"

using namespace cocostudio;
EffectManage::EffectManage()
{

}

EffectManage::~EffectManage()
{
	m_vAnimations.clear();
}

// not autorelease
EffectManage* EffectManage::createEffect()
{
	auto em = new EffectManage;
	if (em && em->init())
	{
		return em;
	}
	delete em;
	return nullptr;
}

bool EffectManage::init()
{
	bool pRet = false;
	do 
	{
		initAnimation();
		pRet = true;
	} while (0);
	return pRet;
}

void EffectManage::initAnimation()
{
	//炮弹爆炸效果
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 28; i++)
	{
		std::string name = StringUtils::format("explode/eff_explode_%02d.png",i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation_explosion = Animation::createWithSpriteFrames(frames, 0.1f, 1);
	m_vAnimations.pushBack(animation_explosion);
	
	//海面点击效果
	Vector<SpriteFrame *> frames_2;
	for (int i = 0; i <= 12; i++)
	{
		auto name = StringUtils::format("move/eff_move_%02d.png",i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames_2.pushBack(frame);
	}
	
	Animation* animation_move = Animation::createWithSpriteFrames(frames_2,0.05f,2);
	m_vAnimations.pushBack(animation_move);


	createCloseBattleAnim();
	createBattleWaterAnim();
	createDropAnim();
	createSkillStrongAttackAndSplash();
	createShipBefired();
	creatShipSink();
	createSmokeEffect();
	createFireEffect();
	createFireSmokeEffect();
	createRecoverEffectGreen();
	createRecoverEffectRed();
	createRepaireffect();
	createSpeedUpEffect();
	createCloseAttackUp();
	createDefenceUp();
	createSpeedDown();
	createSpecEffect();
	createCyclone();
}

void EffectManage::createCloseBattleAnim()
{
	//近战两把剑的动画
	Vector<SpriteFrame *> frames_2;
	for (int i = 0; i <= 29; i++)
	{
		std::string name = StringUtils::format("melee/eff_melee_%02d.png",i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames_2.pushBack(frame);
	}
	Animation* animation_battleClose = Animation::createWithSpriteFrames(frames_2,0.5f/30,-1);
	m_vAnimations.pushBack(animation_battleClose);
}

void EffectManage::createBattleWaterAnim()
{
	//战斗中海水的动画
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/test/battlewater_200.plist");

	Vector<SpriteFrame *> frames;
	for (int i = 0; i < 120; i++)
	{
		std::string name = StringUtils::format("water%04d.png",i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	
	Animation* animation_water = Animation::createWithSpriteFrames(frames,1.f/30,-1);
	m_vAnimations.pushBack(animation_water);
}

void EffectManage::createDropAnim()
{
	//炮弹打到海面的效果动画
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 30; i++)
	{
		std::string name = StringUtils::format("bulletinwater/eff_bulletinwater_%02d.png",i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}

	Animation* animation_alpha = Animation::createWithSpriteFrames(frames,0.05f,1);
	m_vAnimations.pushBack(animation_alpha);

}
void EffectManage::createSkillStrongAttackAndSplash()
{
	//技能加强火力效果
	Vector<SpriteFrame *> frames_skillAttack;
	for (int i = 0; i <= 14; i++)
	{
		std::string name = StringUtils::format("rangedamgeup/eff_rangedamgeup_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames_skillAttack.pushBack(frame);
	}
	Animation* animation_attack = Animation::createWithSpriteFrames(frames_skillAttack, 0.05f, 1);
	m_vAnimations.pushBack(animation_attack);

	//船被击中时的碎片
	Vector<SpriteFrame *> frames_splash;
	for (int i = 0; i <= 24; i++)
	{
		std::string name = StringUtils::format("fragments/eff_fragments_%02d.png", i);//26
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames_splash.pushBack(frame);
	}
	Animation* animation_splash = Animation::createWithSpriteFrames(frames_splash, 0.05f, 1);
	m_vAnimations.pushBack(animation_splash);


	//星星闪亮
	Vector<SpriteFrame *> frames_starShine;
	for (int i = 0; i <= 53; i++)
	{
		std::string name = StringUtils::format("beamstar/eff_beamstar_%02d.png", i);//18
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames_starShine.pushBack(frame);
	}
	Animation* animation_starShine = Animation::createWithSpriteFrames(frames_starShine, 0.05f, 1);
	m_vAnimations.pushBack(animation_starShine);
}
void EffectManage::createShipBefired()
{
	//船着火
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 42; i++)
	{
		std::string name = StringUtils::format("shipfire/eff_shipfire_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation_shipFire = Animation::createWithSpriteFrames(frames, 0.05f, 1);
	m_vAnimations.pushBack(animation_shipFire);
}
void EffectManage::creatShipSink()
{
	//船沉没
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 31; i++)
	{
		std::string name = StringUtils::format("shipsink/eff_shipsink_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation_shipsink = Animation::createWithSpriteFrames(frames, 0.05f, 1);
	m_vAnimations.pushBack(animation_shipsink);
}
void EffectManage::createSmokeEffect()
{
	//燃烧弹技能产生的烟雾
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 21; i++)
	{
		std::string name = StringUtils::format("smokeeffect/eff_smokeeffect_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation_smoke = Animation::createWithSpriteFrames(frames, 0.05f, 1);
	m_vAnimations.pushBack(animation_smoke);
}

void EffectManage::createFireEffect()
{
    //船只开火
	Vector<SpriteFrame *> frames;
	for (int i = 0; i<= 24; i++)
	{
		std::string name;
		name = StringUtils::format("fire/eff_fire_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 25, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createFireSmokeEffect()
{
	//船只开火烟雾
	Vector<SpriteFrame *> frames;
	for (int i = 0;i <= 15; i++)
	{
		std::string name_1;
		name_1 = StringUtils::format("fireSmoke/eff_fireSmoke_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name_1);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 16, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createRecoverEffectGreen()
{
	//船只恢复特效绿色
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 38; i++)
	{
		std::string name_1;
		name_1 = StringUtils::format("recovergreen/eff_recovergreen_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name_1);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 3.0f / 39, 1);
	m_vAnimations.pushBack(animation);

}


void EffectManage::createRecoverEffectRed()
{
	//船只恢复特效红色
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 38; i++)
	{
		std::string name = StringUtils::format("recoverred/eff_recoverred_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 3.0f / 39, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createRepaireffect()
{
	//船只修理特效
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 14; i++)
	{
		std::string name = StringUtils::format("repaireffect/eff_repaireffect_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 15, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createSpeedUpEffect()
{
	//船只加速
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 15; i++)
	{
		std::string name = StringUtils::format("speedup/eff_speedup_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 16, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createCloseAttackUp()
{
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 15; i++)
	{
		std::string name = StringUtils::format("attackup/eff_attackup_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 16, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createDefenceUp()
{
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 14; i++)
	{
		std::string name = StringUtils::format("defenceup/eff_defenceup_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 15, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createSpeedDown()
{
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 17; i++)
	{
		std::string name = StringUtils::format("speeddown/eff_speeddown_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 18, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createSpecEffect()
{
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 13; i++)
	{
		std::string name = StringUtils::format("spec/eff_spec_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 14, 1);
	m_vAnimations.pushBack(animation);
}

void EffectManage::createCyclone()
{
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 17; i++)
	{
		std::string name = StringUtils::format("cyclone/eff_cyclone_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 18, 1);
	m_vAnimations.pushBack(animation);
}

Animation* EffectManage::getAnimation(int tag)
{
	return m_vAnimations.at(tag);
}