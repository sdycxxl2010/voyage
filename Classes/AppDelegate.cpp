#include "AppDelegate.h"
#include "AppMacros.h"
#include <vector>
#include <string>
#include "CCLuaEngine.h"
#include "UIRegister.h"
#include "UILogin.h"
#include "ProtocolThread.h"
#include "TVLoadingScene.h"
#include "UICreateRole.h"
#include "TVSingle.h"
#include "cocostudio/TextResUtils.h"
#include "UIStart.h"
#include "Utils.h"
#include "env.h"
#include "tinyxml2/tinyxml2.h"
#include "base/base64.h"
#include "base/ccUtils.h"

#include "AppHelper.h"
#include "TVGlobalConfig.h"

#if ANDROID
//#include "MobClickCpp.h"
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif

using namespace std;
USING_NS_CC;

AppDelegate::AppDelegate() 
{

}

AppDelegate::~AppDelegate() 
{
	delete TVSingle::getInstance();
}


//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
	//set OpenGL context attributions,now can only set six attributions:
	//red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

	GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
	//mod by eong, load global texts before everything.

#ifdef ANDROID
	//MOBCLICKCPP_START_WITH_APPKEY_AND_CHANNEL("561e388867e58e13410007f3", "global");
#endif
	FileUtils::getInstance()->setPopupNotify(false);
    FileUtils::getInstance()->addSearchPath("app_res");
    FileUtils::getInstance()->addSearchPath("app_res/cocosstudio");
	FileUtils::getInstance()->addSearchPath("cocosstudio");
	FileUtils::getInstance()->addSearchPath("res_lua");
	FileUtils::getInstance()->addSearchPath("MainCity_bg");
	FileUtils::getInstance()->addSearchPath("audio");

	/* set lang for cocostudio */
	TEXTRESOOL->setLang((LanguageType)Utils::getLanguage());
	/*end set lang */

//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(TEXT_PATH_JSON);
//	cocostudio::TextResUtils::getInstance()->loadData(jsonpath);
	SINGLE_SHOP->loadTextData();

    // initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) 
	{
		glview = GLViewImpl::create("My Game");
	}
	director->setOpenGLView(glview);

	// Set the design resolution
	glview->setDesignResolutionSize(1280, 720, ResolutionPolicy::SHOW_ALL);
    
	//LuaEngine* pEngine = LuaEngine::getInstance();

	// ScriptEngineManager::getInstance()->setScriptEngine(pEngine);
    

	// turn on display FPS
if(ENVIRONMENT != PRODUCTION){
	director->setDisplayStats(true);
}
	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);
	
	TVSingle::getInstance();

	if (TVGlobalConfig::getInstance()->Enalbe_NewUIArch != 1)
	{
		director->runWithScene(UIStart::createScene());
	}

#if (defined ANDROID)
        if(!GPGSManager::IsSignedIn()){
                GPGSManager::BeginUserInitiatedSignIn();
                log("test login google play services");
        }
        
#endif
	AppHelper::InitHelper();


    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() 
{
#ifdef ANDROID
	//umeng::MobClickCpp::applicationDidEnterBackground();
#endif
    Director::getInstance()->stopAnimation();
	ProtocolThread::GetInstance()->notifyGamePaused(true);
    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() 
{
#ifdef ANDROID
	//umeng::MobClickCpp::applicationWillEnterForeground();
#endif
    Director::getInstance()->startAnimation();

	ProtocolThread::GetInstance()->notifyGamePaused(false);
    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
