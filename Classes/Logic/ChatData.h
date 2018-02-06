#ifndef __CHATDATA_H__
#define __CHATDATA_H__
#include "cocos2d.h"

#include "login.pb-c.h"
USING_NS_CC;
#define  CHAT_MAX  50
class ChatData
{
public:
	ChatData();
	~ChatData();
	void reset();
	bool init();

	void addPublic_zh(std::string name, std::string content, int nation);
	void addPublic(std::string name, std::string content, int nation);
	void addCountry(std::string name, std::string content, int nation);
	void addZone(std::string name, std::string content, int nation);
	void addGuild(std::string name, std::string content, int nation);
	void addPrivate(std::string nameid, std::string name, std::string content, int nation);

	std::vector<std::string> m_public_name_zh;
	std::vector<std::string> m_public_content_zh;
	std::vector<int> m_public_nation_zh;
	bool m_public_unread_falg_zh;
	std::vector<std::string> m_public_name;
	std::vector<std::string> m_public_content;
	std::vector<int> m_public_nation;
	bool m_public_unread_falg;
	std::vector<std::string> m_country_name;
	std::vector<std::string> m_country_content;
	std::vector<int> m_country_nation;
	bool m_country_unread_falg;
	std::vector<std::string> m_zone_name;
	std::vector<std::string> m_zone_content;
	std::vector<int> m_zone_nation;
	bool m_zone_unread_falg;
	std::vector<std::string> m_guild_name;
	std::vector<std::string> m_guild_content;
	std::vector<int> m_guild_nation;
	bool m_guild_unread_falg;
	std::map<std::string, std::vector<std::string>> m_private_name;
	std::map<std::string, std::vector<std::string>> m_private_content;
	std::map<std::string, std::vector<int>> m_private_nation;
	std::map<std::string, bool>m_private_unread_falg;
};

#endif