#ifndef __BASIC_H__
#define __BASIC_H__
#include <string>
#include <map>
#include "env.h"
#include "TVTemplates.h"

#include "cocos2d.h"
#include "cocos-ext.h"
#include "SystemVar.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "cocostudio/DictionaryHelper.h"
#include "network/HttpClient.h"

#include "TVAudioData.h"
#include "TVBattleData.h"
#include "TVGoodsType.h"




using namespace std;
USING_NS_CC;
//USING_NS_CC_EXT;
USING_NS_CC_MATH;
using namespace cocostudio;
using namespace cocos2d::ui;
using namespace cocos2d::network;

enum TV_E_UPDATE_PRIORITY
{
	UPDATE_PRIORITY_HIGH = -128,
	UPDATE_PRIORITY_NORMAL = 0,
	UPDATE_PRIORITY_LOW = 128,
};

class TVBasic : public cocos2d::Node
{
public:

	TVBasic():m_vImage(nullptr){};
	virtual ~TVBasic(){};

	cocos2d::Sprite* getImage(){ return m_vImage; };

	void setImage(cocos2d::Sprite* imageName) { m_vImage = imageName;};

	void setName(std::string strName){m_sName = strName;};

	std::string getName(){ return m_sName;};

protected:

	
	std::string m_sName;
	cocos2d::Sprite* m_vImage;
private:
	
	
};


#endif

