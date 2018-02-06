#include "ChatData.h"

ChatData::ChatData()
{
	reset();
}

ChatData::~ChatData()
{

}

void ChatData::reset()
{
	m_public_name_zh.clear();
	m_public_content_zh.clear();
	m_public_nation_zh.clear();
	m_public_name.clear();
	m_public_content.clear();
	m_public_nation.clear();
	m_country_name.clear();
	m_country_content.clear();
	m_country_nation.clear();
	m_zone_name.clear();
	m_zone_content.clear();
	m_zone_nation.clear();
	m_guild_name.clear();
	m_guild_content.clear();
	m_guild_nation.clear();
	m_private_name.clear();
	m_private_content.clear();
	m_private_nation.clear();
	m_public_unread_falg_zh = false;
	m_public_unread_falg = false;
	m_country_unread_falg = false;
	m_zone_unread_falg = false;
	m_guild_unread_falg = false;
	m_private_unread_falg.clear();
}

bool ChatData::init()
{
	bool pRet = false;
	do 
	{
		pRet = true;
	} while (0);

	return pRet;
}

void ChatData::addZone(std::string name, std::string content, int nation)
{
	if (m_zone_name.size() > CHAT_MAX)
	{
		m_zone_name.erase(m_zone_name.begin());
		m_zone_content.erase(m_zone_content.begin());
		m_zone_nation.erase(m_zone_nation.begin());
	}
	m_zone_name.push_back(name);
	m_zone_content.push_back(content);
	m_zone_nation.push_back(nation);
}

void ChatData::addPublic_zh(std::string name, std::string content, int nation)
{
	if (m_public_name_zh.size() > CHAT_MAX)
	{
		m_public_name_zh.erase(m_public_name_zh.begin());
		m_public_content_zh.erase(m_public_content_zh.begin());
		m_public_nation_zh.erase(m_public_nation_zh.begin());
	}
	m_public_name_zh.push_back(name);
	m_public_content_zh.push_back(content);
	m_public_nation_zh.push_back(nation);
}

void ChatData::addPublic(std::string name, std::string content, int nation)
{
	if (m_public_name.size() > CHAT_MAX)
	{
		m_public_name.erase(m_public_name.begin());
		m_public_content.erase(m_public_content.begin());
		m_public_nation.erase(m_public_nation.begin());
	}
	m_public_name.push_back(name);
	m_public_content.push_back(content);
	m_public_nation.push_back(nation);
}

void ChatData::addGuild(std::string name, std::string content, int nation)
{
	if (m_guild_name.size() > CHAT_MAX)
	{
		m_guild_name.erase(m_guild_name.begin());
		m_guild_content.erase(m_guild_content.begin());
		m_guild_nation.erase(m_guild_nation.begin());
	}
	m_guild_name.push_back(name);
	m_guild_content.push_back(content);
	m_guild_nation.push_back(nation);
}

void ChatData::addCountry(std::string name, std::string content, int nation)
{
	if (m_country_name.size() > CHAT_MAX)
	{
		m_country_name.erase(m_country_name.begin());
		m_country_content.erase(m_country_content.begin());
		m_country_nation.erase(m_country_nation.begin());
	}
	m_country_name.push_back(name);
	m_country_content.push_back(content);
	m_country_nation.push_back(nation);
}

void ChatData::addPrivate(std::string nameid, std::string name, std::string content, int nation)
{	
	 auto it = m_private_name.find(nameid);
	 if (it == m_private_name.end())
	 {
		 std::vector<std::string> p_name;
		 std::vector<std::string> p_content;
		 std::vector<int> p_nation;
		 m_private_name.insert(std::pair<std::string,std::vector<std::string>>(nameid,p_name));
		 m_private_content.insert(std::pair<std::string,std::vector<std::string>>(nameid,p_content));
		 m_private_nation.insert(std::pair<std::string, std::vector<int>>(nameid, p_nation));
		 m_private_name[nameid].push_back(name);
		 m_private_content[nameid].push_back(content);
		 m_private_nation[nameid].push_back(nation);
	 }else
	 {
		 if(m_private_name[nameid].size() > CHAT_MAX)
		 {
			m_private_name[nameid].erase(m_private_name[nameid].begin());
			m_private_content[nameid].erase(m_private_content[nameid].begin());
			m_private_nation[nameid].erase(m_private_nation[nameid].begin());
		 }
		 m_private_name[nameid].push_back(name);
		 m_private_content[nameid].push_back(content);
		 m_private_nation[nameid].push_back(nation);
	 }
}