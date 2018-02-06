#ifndef __BATTLE_SKILL_H__
#define __BATTLE_SKILL_H__

#include "TVBasic.h"
#include "TVBattleShareObj.h"
#include "UIBasicLayer.h"

struct SkillData1
{
	int id;   //技能的id
	int lv;   //等级
	int type; //技能的类型， 1:舰队主动 2：舰队别动 3：NPC被动
	bool state;//是否在cd状态
	bool used[5];//是否使用(一个技能作用多艘船只)
};

struct SkillData
{
	int skill_own_id;
	int skill_target;
	int skill_effect;
	int skill_sid;
	int skill_type;
	int skill_time;
	int skill_hitRate;
	int skill_level;
};

class TVBasicSkill
{
public:
	TVBasicSkill();
	virtual ~TVBasicSkill(){};

	SkillData& getSkillData(){ return m_SkillData; };
	virtual	void initMem(int ownId,int level,int target){};
	void setOwnSelf(int id){ m_SkillData.skill_own_id = id;};
	void setTarget(int id) { m_SkillData.skill_target = id;};
	void setHitRate(int rate) {m_SkillData.skill_hitRate = rate;};
template<class T>
T* clone()
	{
		T* skill = new T;
		auto &skill_data = skill->getSkillData();
		skill_data.skill_own_id = m_SkillData.skill_own_id;
		skill_data.skill_sid = m_SkillData.skill_sid;
		skill_data.skill_type = m_SkillData.skill_type;
		skill_data.skill_time = m_SkillData.skill_time;
		skill_data.skill_hitRate = m_SkillData.skill_hitRate;
		
		skill_data.skill_effect = m_SkillData.skill_effect;
		skill_data.skill_level = m_SkillData.skill_level;
		skill_data.skill_target = m_SkillData.skill_target;
		return skill;
	};
protected:
	SkillData m_SkillData;
};


class SkillManage
{
public:
	SkillManage():m_pShareObj(nullptr){};
	~SkillManage();
	void checkBufferState();
	void setShareObj(TVBattleShareObj* shareObj){ m_pShareObj = shareObj;};
	SkillData* getUsedBufferInfo(std::string ky);

	void setSkillUsed(int index, bool isUsed);
	void pushAllSkillData(EngageInFightResult *pResult);
	std::vector<SkillData1> m_vAllSkillInfo;
private:
	std::map<std::string,TVBasicSkill*> m_UseSkills;
	std::vector<std::string> m_deleteQueue;
	TVBattleShareObj* m_pShareObj;
};

#endif