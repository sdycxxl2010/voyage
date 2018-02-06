#ifndef __SCENE__LOADING__LAYER__
#define __SCENE__LOADING__LAYER__

#include "UIBasicLayer.h"

class UISceneLoading : public cocos2d::Layer
{
public:
	UISceneLoading();
	~UISceneLoading();
	bool init();
	void onEnter();
	void onExit();
	void showLoading_1(float loadDiffTag);
	static UISceneLoading* create(std::vector<std::string> &, int);
	void loadingPic();
	void loadPicCallBack(cocos2d::Texture2D* texture, int index);
	void LoadingParticle();
	void goToNextScene();
	void cleanCache();
	void runNextScene(float dt);
	/*
	*º”‘ÿÃÿ–ß
	*/
	void loadEffectPlist();

	void update(float f);
private:
	cocos2d::Layer*mParent;
	std::vector<std::string> *m_Paths;
	int m_Max;
	int m_Tag;
	static int particleloaded;

	bool  m_bStartLoadPrc;
	int   m_nLoadCnt;
	int   m_nTickCnt;
	int   m_nWaitCnt;
};

#endif
