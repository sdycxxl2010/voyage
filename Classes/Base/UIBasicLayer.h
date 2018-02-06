/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月26日
 *  Author:Sen
 *  all class 
 *
 */

#ifndef __BASIC__LATER__
#define __BASIC__LATER__
#include "TVBasic.h"
#include "login.pb-c.h"
#include "ProtocolThread.h"
#include "TVSingle.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "json/document.h"
#include <unordered_map>
#include "EnumList.h"
#ifdef ANDROID
//#include "TalkingData.h"
#endif
#include "ServerManager.h"

USING_NS_CC;
using namespace ui;
using namespace rapidjson;
using namespace cocostudio;

#ifndef CC_RETURN_IF
#define CC_RETURN_IF(__TYPE__) if(!__TYPE__) {cocos2d::log("scene crete fail");return NULL;}
#endif
#define isButton(_tt) (name.find(#_tt) == 0)
#define COIN_NOT_FAIL  100
#define GOLD_NOT_FAIL  101

const std::string TEXT_MAP_PATH = "fonts/time_total.png";

typedef std::function<Widget* (Widget*,int)> FillListViewCallBack;

class UIBasicLayer : public Layer
{
public:
	enum JobIndex
	{
		NONE,
		MAIN_STORY,
		COMPANION_STORY,
		SMALL_STORY,
		DAILY_SIGN_IN,
		OFFLINE_DELEGATE,
		REDUCE_NOTICE,//扣除玩家的声望和货币
		FIRST_MATE,
		GUARD
	};
	//系统通知走字类型
	enum SystemMessageType
	{
		SHUTDOWN_AFTER_WAIT_TIME = 1,
		COUNTRY_WAR_OVER,
		COUNTRY_WAR_CITY_FALLEN,
		COUNTRY_WAR_START,
		COUNTRY_WAR_ING
	};
	UIBasicLayer();
	virtual ~UIBasicLayer();
	void onEnter() override;
	void onExit() override;
	virtual bool init();

	/* int 1 = Main_port ,2 = bank, 3 = dock ,4 = shipyard, 5 = exchnge,
	6 = bar , cabin = 7 mail = 8,9 = center10 = map,11= palace */
	void setUIType(const UI_VIEW_TYPE type);  //UI type
	void playAudio();		//播放音乐
	void stopAudio();		//暂停播放音乐
	void button_callBack();  //回到主城

	bool isNumber(const std::string str);   //是否是数字
	void exitGame();				  //退出游戏
	void reConnect();				  //重新连接服务器
	void gotoLoginPage(int succ );				//google play 登录回调
	void openReconnectDialog(const std::string name);  //提示连接服务器
	void openSuccessOrFailDialog(const std::string name, Node* parent = nullptr, const int zorder = 10001);  //提示弹框
    void openSuccessOrFailDialogWithString(const std::string content, Node* parent = nullptr, const int zorder = 10001);  //提示弹框
	void registerCallBack();			//注册
	void unregisterCallBack();			//注销
	void addListViewBar(ListView* lv,ImageView* iv);  //右侧辅助条
	void addEventForChildren(Widget* parent,std::string type = "button_"); //为所有的子控件增加点击事件
	void setOnlyoneDialogVisibleFromParent(Widget* parent,Widget* child);  //设置所有的子控件中的除"child"显示其它都隐藏
	void setTouchabedForChildren(Widget* parent,bool touchabled);  //设置所有的子控件是否可以点击
	
	
	virtual int64_t	getCurrentTimeUsev();  //获取时间
	virtual void onServerEvent(struct ProtobufCMessage* message,int msgType);    //服务器数据
	virtual void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType) = 0;//默认button相应
	virtual void checkboxCall_func(Ref* target,CheckBoxEventType type);					
	virtual void scrollEvent(Ref *target,ScrollviewEventType type);

	void getItemNameAndPath(const int itemtype,const int itemid,std::string& name,std::string& path);  //获取物品的名称和图标路径
	void openGoodInfo(const GetItemsDetailInfoResult *result,const int type,const int id); //物品详情
	float getLabelHight(const std::string content, const float width, const std::string foneName, const int fontSize = 24);		//根据宽度获取文本的高度
	void setGLProgramState(Widget* target,const bool isGrey);			//设置图片变灰
	void setTextProperty(TextAtlas *target, const int num, const std::string& charMapFile = TEXT_MAP_PATH,
		int itemWidth = 20, int itemHeight = 26, const std::string& startCharMap = "/");//图片数字
	
	void setTextColorFormIdAndType(Text* target,const int itemId,const int itemType,bool iswhite = false); //根据稀有等级设置文本的颜色
	void setBgButtonFormIdAndType(Widget* target,const int itemId,const int itemType); //根据稀有等级设置背景按钮框的图片
	void setBgImageColorFormIdAndType(ImageView* target,const int itemId,const int itemType);//根据稀有等级设置背景图片
	void addStrengtheningIcon(Widget* target);
	void addequipBrokenIcon(Widget*target);//装备破损的图标

	void openView(const std::string filePath,const int zOrder = 0); //打开（加载）交互窗口
	void closeView();					//关闭（删除）当前窗口
	void closeView(const std::string filePath,const float fTime = 0);  //关闭（删除）已达开的窗口（第二个参数是有延迟删除Node时用到）   
	void removeView(Node *view);
	Widget* getViewRoot(const std::string filePath);  //获取已达开窗口的csb文件的根节点   
	void setViewZOrder(const std::string filePath,const int zOrder);  //设置已达开窗口的zorder
	Node* getView(const std::string filePath);                       //获取已达开窗口的Node
	void registerListViewCallback(const FillListViewCallBack & callback){m_listviewCallback = new FillListViewCallBack(callback);}
	int chatGetUtf8CharLen(const char*utf8);//得到字节数
	void clearListViewCallback(){
		if(m_listviewCallback){
			delete m_listviewCallback;
			m_listviewCallback = nullptr;
		}
	}

	std::string numSegment( std::string numstr);//数字字符串 逗号分割
	void setBrokenEquipRed(Widget* target,bool isRed = true);//损坏的装备变红

	void addNeedControlWidgetForHideOrShow(Widget* target, bool isClear = true);//将剧情任务中要控制的的子控件的父节点为统一管理子控件的显示与隐藏做铺垫
	/*
	*剧情任务中控制其他的子控件显示和隐藏（在显示时提交服务器）
	*隐藏时配合使用addNeedControlWidgetForHideOrShow
	*/
	void allTaskInfo(bool isShow,  JobIndex index, float time = 0.0f,bool isCommit = false); 
	//新手引导的剧情中控制其他子空间的显示和隐藏
	void allGuideTaskInfo(bool isShow,float time = 0.0f);
	//新手引导中，将剧情任务中要控制的的子控件的父节点为统一管理子控件的显示与隐藏做铺垫
	void guideAddNeedControlWidgetForHideOrShow(Widget* target, bool isClear = true);

	void notifyUIPartDelayTime(float time);
	void notifyUIPartCompleted(float t);
	//键盘响应
	void keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
	//回调函数
	virtual void notifyCompleted(int actionIndex);
	void closeRawardEvent(Ref *pSender, Widget::TouchEventType TouchType);

	//用于特殊处理协议与暂停界面的层级关系 0：正常情况下 1:游戏开始时 2：设置中
	void setEula(int nEula) { m_nEula = nEula; };
	//设置下标字体大小和描边
	void setTextSizeAndOutline(Text *target, int vlaue);
	//跳转到app store详情页面
	void gotoStorePage();
	//获取utf8字节数
	int my_strlen_utf8_c(char *s);
	/*
	*限制物品的名字长度
	*参数  content:内容  len:长度
	*/
	std::string apostrophe(std::string content, int len);

	/*
	*判断字符串语言
	*/
	bool isChineseCharacterIn(std::string str);
	/*
	*文字显示不完全时，修改字体大小
	*/
	void setTextFontSize(Text* target);

	std::vector<Node*> getRoots()
	{
		return m_vRoots;
	}
protected:
	
	Sprite		*m_vImage;
	Widget		*m_vRoot;
	Label		*m_HideLabel;
	UI_VIEW_TYPE m_eUIType;
	std::vector<Node*> m_vRoots;	//Node
	std::vector<std::string> m_vFilePath;	//csb文件路径
	Widget		*m_pRoot; //当前打开的csb
	std::vector<Widget*> m_pAllNeedControlWidget; //因剧情需要 隐藏或显示的widget
	std::vector<Widget*> m_pAllNeedControlGuideWidget; //因剧情需要 隐藏或显示的widget
	bool 		m_isFirstScrollEvent;
	void reconnectGameOrExit(Ref *pSender, Widget::TouchEventType TouchType);//掉线提示框btn事件
	//水手图标标志
	WARNING_LV n_sailorNotEnoughTag;
private:
	FillListViewCallBack* m_listviewCallback;
	EventListenerKeyboard*m_keyboardListener;//返回键监听器
	
	//用于特殊处理协议与暂停界面的层级关系 0：正常情况下 1:游戏开始时 2：设置中
	int m_nEula;
};

#endif
