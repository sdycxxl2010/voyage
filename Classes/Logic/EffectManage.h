/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月30日
 *  Author:Sen
 *  effect manage 只用在战斗部分
 *
 */

#ifndef __EFFECT__MANAGE__
#define __EFFECT__MANAGE__

#include "TVBasic.h"
#include "TVBattleData.h"

USING_NS_CC;

enum PARTICLE_TYPE
{
	PT_EXPLOSION = 0,//炮弹爆炸效果
	PT_TOUCH_ANIMA,//海面点击效果
	PT_CLOSE_BATTLE,//近战效果
	PT_BATTLE_BG,//战斗中海水动画
	PT_DROP,//炮弹打到海面的效果动画
	PT_SKILL_ATTACK_STRONG,//技能加强火力
	PT_SHIP_ATTACKED,//船被子弹击中的碎片
	PT_STAR_SHINE,//星星闪亮的效果
	PT_SHIP_FIREING,//船着火
	PT_SHIP_SINKING,//船沉没
	PT_SMOKE_EFFECT,//燃烧弹技能产生的烟雾
	PT_FIRE_EFFECT,//开炮特效
	PT_FIRE_SMOKE_EFFECT,//炮管烟雾
	PT_RECOVER_GREEN,//恢复特效绿色
	PT_RECOVER_RED,//恢复特效红色
	PT_REPAIR_EFFECT,//修理特效
	PT_SPEED_UP_EFFECT,//加速特效
	PT_CLOSE_ATTACK_UP,//近战攻击力提升
	PT_DEFENCE_UP,//船只防御力提升
	PT_SPEED_DOWN,//减速特效
	PT_SPEC_EFFECT,//蓄力特效
	PT_CYCLONE_EFFECT,//旋风特效
};

class EffectManage
{
public:
	EffectManage();
	~EffectManage();
	static EffectManage* createEffect();
	bool init();
	void initAnimation();
	/*
	*近战的特效
	*/
	void createCloseBattleAnim();
	/*
	*战斗海面海水的特效
	*/
	void createBattleWaterAnim();
	/*
	*炮弹打到海水中的特效
	*/
	void createDropAnim();
	void createSkillStrongAttackAndSplash();//攻击技能效果，船碎片效果，星星闪亮效果
	void createShipBefired();//船着火
	void creatShipSink();//船沉没
	void createSmokeEffect();//船被燃烧弹技能击中产生的烟雾
	void createFireEffect();//船开火
	void createFireSmokeEffect();//船只开炮后的烟雾效果
	void createRecoverEffectGreen();//恢复特效(绿色)
	void createRecoverEffectRed();//恢复特效(红色)
	void createRepaireffect();//修理特效
	void createSpeedUpEffect();//加速特效
	void createCloseAttackUp();//近战攻击力提升特效
	void createDefenceUp();//船只防御力提升特效
	void createSpeedDown();//船只减速特效
	void createSpecEffect();//蓄力特效
	void createCyclone();//旋风特效
	
	/*
	*获取动作特效
	*/
	Animation* getAnimation(int tag);
private:
	Vector<Animation*> m_vAnimations;
};

#endif