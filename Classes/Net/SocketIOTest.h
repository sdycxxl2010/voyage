//
//  SocketIOTest.h
//  TestCpp
//
//  Created by Chris Hannon on 6/26/13.
//
//
#ifndef __TestCpp__SocketIOTest__
#define __TestCpp__SocketIOTest__

#include "cocos2d.h"
//#include "network/SocketIO.h"
#include "login.pb-c.h"

class SocketIOTestLayer
	: public cocos2d::Layer
{
public:
	static cocos2d::Scene* scene();
	SocketIOTestLayer(void);
	virtual ~SocketIOTestLayer(void);
// 
// 	virtual void onConnect(cocos2d::network::SIOClient* client);
//     virtual void onMessage(cocos2d::network::SIOClient* client, const std::string& data);
//     virtual void onClose(cocos2d::network::SIOClient* client);
//     virtual void onError(cocos2d::network::SIOClient* client, const std::string& data);
// 
	void toExtensionsMainLayer(cocos2d::Ref *sender);
	
	void onMenuSIOClientClicked(cocos2d::Ref *sender);
	void onMenuTestMessageClicked(cocos2d::Ref *sender);
	void onMenuTestEventClicked(cocos2d::Ref *sender);
	void onMenuTestClientDisconnectClicked(cocos2d::Ref *sender);

	void onMenuSIOEndpointClicked(cocos2d::Ref *sender);
	void onMenuTestMessageEndpointClicked(cocos2d::Ref *sender);
	void onMenuTestEventEndpointClicked(cocos2d::Ref *sender);
	void onMenuTestEndpointDisconnectClicked(cocos2d::Ref *sender);


	void testevent(struct ProtobufCMessage* message);
	//void echotest(cocos2d::network::SIOClient *client, const std::string& data);
// 
// 	cocos2d::network::SIOClient *_sioClient, *_sioEndpoint;

	cocos2d::LabelTTF *_sioClientStatus;
};

void runSocketIOTest();

#endif /* defined(__TestCpp__SocketIOTest__) */
