/*
 * ServerConfig.cpp
 *
 *  Created on: 2015年9月30日
 *      Author: eong
 */

#include "ServerManager.h"
#include "env.h"
#include "Utils.h"
#include "TVEventManager.h"

using namespace cocostudio;

ServerManager::ServerManager()
{
}

ServerManager::~ServerManager() {

}

ServerInfo* ServerManager::CurSelectedServer()
{
	return ServerManager::getInstance()->CurSelectServer();
}

ServerInfo* ServerManager::GetServerByName(string serverName)
{
	for (auto it = m_vServerArr.begin(); it != m_vServerArr.end(); it ++)
	{
		if ((*it)->name == serverName)
		{
			return *it;
		}
	}
	return NULL;
}

ServerInfo* ServerManager::CurSelectServer()
{
	return m_pCurSever;
}

void ServerManager::SelectServerByName(string serverName)
{
	m_pCurSever = NULL;
	for (auto it = m_vServerArr.begin(); it != m_vServerArr.end(); it++)
	{
		if ((*it)->name == serverName)
		{

			m_pCurSever = *it;
		}
	}
}

void ServerManager::AddServer(ServerInfo* server)
{
	m_vServerArr.push_back(server);
}

void ServerManager::RemoveServerByName(string serverName)
{
	for (auto it = m_vServerArr.begin(); it != m_vServerArr.end();)
	{
		if ((*it)->name == serverName)
		{
			m_vServerArr.erase(it);
			break;
		}
	}
}


void ServerManager::RequestServerList()
{
	cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
	std::string urlpath;

	bool useBack = Utils::shouldUseBackServer();
	std::string triggerPath = "/sdcard/useback";
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(triggerPath);
	if (contentStr.length() > 0){
		useBack = true;
	}


	if (ENVIRONMENT == PRODUCTION){
		urlpath = "http://pubapi.voyage.piistudio.com/status.php";
		if (USE_BAKCUP_SERVER || useBack){
			urlpath = "http://pubapi.voyage.piistudio.com/status.php?useback=1";
		}
	}
	else{
		urlpath = "http://pubapi-dev.voyage.piistudio.com/status.php";
	}
	request->setUrl(urlpath.c_str());
	request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(ServerManager::OnServerListResponse, this));


	request->setTag("log_test");

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void ServerManager::OnServerListResponse(network::HttpClient* client, network::HttpResponse* response)
{
	long code = response->getResponseCode();
	int valid = 0;
	if(code == 200)
	{
		auto data = response->getResponseData();
		std::string s_data(data->begin(),data->end());
		rapidjson::Document root;
		log("server config:%s",s_data.c_str());
		if(root.Parse<0>(s_data.c_str()).HasParseError()){
			return;
		}
		int configClientVersion = DictionaryHelper::getInstance()->getIntValue_json(root, "version");

		const char* default_zone_name = DictionaryHelper::getInstance()->getStringValue_json(root,"default_zone_name");
		if(1)
		{
			int version = DictionaryHelper::getInstance()->getIntValue_json(root,"version");
			auto&zones = DictionaryHelper::getInstance()->getSubDictionary_json(root,"zones");
			int n = (int)zones.Size();
			for(int i=0;i<n;i++){
				auto server = new ServerInfo;
				auto& item = DictionaryHelper::getInstance()->getSubDictionary_json(zones,i);
				auto status = DictionaryHelper::getInstance()->getStringValue_json(item,"status");
				if(status){
					if(strcmp(status,"ok") == 0)
					{
						server->status = 1;
					}else if(strcmp(status,"maintain") == 0){
						if(FORCE_CONNECT){
							server->status = 1;
						}else{
							server->status = 2;
						}
					}
					else
					{
						server->status = 0;
					}
				}
				auto host = DictionaryHelper::getInstance()->getStringValue_json(item,"host");
				server->host = host;
				auto name = DictionaryHelper::getInstance()->getStringValue_json(item,"name");
				server->name = name;
				server->port = DictionaryHelper::getInstance()->getIntValue_json(item,"port");
				server->chatServer = DictionaryHelper::getInstance()->getStringValue_json(item,"chat");
				server->chatPort = DictionaryHelper::getInstance()->getIntValue_json(item,"chat_port");
				server->apiServer = DictionaryHelper::getInstance()->getStringValue_json(item,"api");
				auto maintain = DictionaryHelper::getInstance()->getStringValue_json(item,"maintain_end");
				if(maintain){
					server->maintainEnd = maintain;
				}

				m_vServerArr.push_back(server);

				if(name && default_zone_name && strcmp(name,default_zone_name) == 0)
				{
					m_pCurSever = server;
				}
			}
		}
	}


	//TODO: truely temporay solution
	TVEventManager::PushEvent(TVEventType::_EVT_FINISH_REQUEST_SERVER_LIST);
}