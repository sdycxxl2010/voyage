/*
 * ServerConfig.h
 *
 *  Created on: 2015年9月30日
 *      Author: eong
 */

#ifndef SERVERCONFIG_H_
#define SERVERCONFIG_H_
#include <string>
#include "TVBasic.h"

class ServerInfo{
public:
	std::string name;
	std::string host;
	int port;
	std::string chatServer;
	int chatPort;
	std::string apiServer;
	int status; //0 服务器关闭状态，1 正常状态，2 维护状态
	std::string maintainEnd; //维护截止时间 gmt时间
public:
	string GetServerName()
	{
		return name;
	}

	string GetServerAddr()
	{
		return apiServer;
	}
};


class ServerManager :public SingletonPtr<ServerManager> 
{
	friend class SingletonPtr<ServerManager>;
public:
	ServerManager();

	virtual ~ServerManager();

public:
	static ServerInfo *CurSelectedServer();

public:
	void RequestServerList();

public:
	vector<ServerInfo*>& GetAllServerInfo()
	{
		return m_vServerArr;
	}

	ServerInfo* GetServerByName(string serverName);

	ServerInfo* CurSelectServer();

	void SelectServerByName(string serverName);

	void AddServer(ServerInfo* server);

	void RemoveServerByName(string serverName);

private:
	void OnServerListResponse(network::HttpClient* client, network::HttpResponse* response);

private:
	vector<ServerInfo*>    m_vServerArr;

	ServerInfo*  m_pCurSever;

};




#endif /* SERVERCONFIG_H_ */
