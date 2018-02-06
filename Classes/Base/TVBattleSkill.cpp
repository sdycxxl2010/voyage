#include "TVBattleSkill.h"
#include "TVBasicShip.h"
#include "TVSingle.h"
#include "ShopData.h"

TVBasicSkill::TVBasicSkill()
{
	m_SkillData.skill_hitRate = 0;
	m_SkillData.skill_level = 0;
	m_SkillData.skill_own_id = 0;
	m_SkillData.skill_sid = 0;
	m_SkillData.skill_effect = 0;
	m_SkillData.skill_time = 0;
	m_SkillData.skill_hitRate = 0;
	m_SkillData.skill_type = 0;
}

SkillData* SkillManage::getUsedBufferInfo(std::string ky)
{
	auto iter = m_UseSkills.find(ky);
	if (iter != m_UseSkills.end())
	{
		 return &(m_UseSkills[ky]->getSkillData());
	}
	return nullptr;
	
}
void SkillManage::checkBufferState()
{
	for (auto iter : m_UseSkills)
	{
		auto &skilldata = iter.second->getSkillData();
		
		if (skilldata.skill_time < 1)
		{
			int tag = skilldata.skill_target;	
			auto delBuffer = [&skilldata](std::vector<TVBasicShip*> &ships,int tag){
				
				for (auto iter : ships)
				{
					if (iter->getTag() == tag)
					{
						iter->deleteBuffer(skilldata);
					}
				}
			};

			if (tag > 5)
			{
				auto &enemys = m_pShareObj->m_vEnemys;
				delBuffer(enemys,tag);

			}else if(m_pShareObj->m_vMyFriends.size() > tag - 1)
			{
				auto &friends = m_pShareObj->m_vMyFriends;
				delBuffer(friends,tag);
			}
			m_deleteQueue.push_back(iter.first);
		}else
		{
			skilldata.skill_time--;
		}
		
	}
	
	for (auto iter : m_deleteQueue)
	{
		delete m_UseSkills[iter];
		m_UseSkills.erase(iter);
	}
	m_deleteQueue.clear();
}

SkillManage::~SkillManage()
{
	for (auto iter : m_UseSkills)
	{
		delete iter.second;
	}
	m_UseSkills.clear();
	
}

void SkillManage::pushAllSkillData(EngageInFightResult *pResult)
{
	m_vAllSkillInfo.clear();
	SkillData1 skillInfo;
	auto& skill = SINGLE_SHOP->getSkillTrees();
	for (size_t i = 0; i < pResult->n_positiveskills; i++)
	{
		auto positiveskillInfo = pResult->positiveskills[i];
		skillInfo.id = positiveskillInfo->id;
		skillInfo.lv = positiveskillInfo->level;
		skillInfo.type = skill[skillInfo.id].ap_type;
		skillInfo.state = false;
		skillInfo.used[0] = false;
		skillInfo.used[1] = false;
		skillInfo.used[2] = false;
		skillInfo.used[3] = false;
		skillInfo.used[4] = false;
		m_vAllSkillInfo.push_back(skillInfo);
	}
}

void SkillManage::setSkillUsed(int index, bool isUsed)
{
	auto& skillInfo = m_vAllSkillInfo[index];
	skillInfo.used[0] = isUsed;

	if (skillInfo.id == SKILL_FLEET_REPAIR) //作用全体船只的技能
	{
		for (size_t i = 0; i < 5; i++)
		{
			skillInfo.used[i] = isUsed;
		}
	}
}