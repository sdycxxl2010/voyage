/*
*  CopyRight (c) 
*  Created on: 2014年10月15日
*  Author:  
*  description: Save the character data
*/
#ifndef __HERO_H__
#define __HERO_H__
#include "TVBasic.h"
#include "login.pb-c.h"
#include "TVSailDefineVar.h"

USING_NS_CC;

//系统通知结构
struct SYSTEM_IFNO
{
	int sys_type;
	int nationId1;
	int nationId2;
	int cityId;
	bool isWait;
};

class TVHero
{
public:
	TVHero();
	~TVHero();
	//注销的时候，需要重新初始化
	bool init();
	//注销的时候，需要清理内存
	bool clean();
	void moveTo(int direct);
	//剩余删除角色次数
	int m_iDeleteNum;
	bool m_bShipStateIsAuto;			//在海上点击地图时，船是不是在自动航行
	int64_t m_iIconidx;						//主角头像的id
	int64_t m_iCoin;							//银币
	int64_t m_iGold;							//v票
	int m_iCityID;								//登录城市id
	//当前城市所属国家id
	int m_iCurCityNation;
	int m_iGender;								//性别
	int m_iID;										//主角id
	int m_iNation;								//所属势力id
	int m_iLevel;								//经验等级
	int m_iPrestigeLv;						//声望等级
	int m_iGuildId;								//公会id
	int m_iHaslisence;						//是否有商业许可证
	int m_iEffectivelisence;				//商业许可证是否有效
	int m_iTotalcontinuousdays;				//连续签到的天数(没有周期)
	int m_iNdailyrewarddata;					//连续签到的天数(30天一周期)
	DailyRewardData **m_iDailyrewarddata;//保存签到的信息
	int m_iNwantedforceids;					//由中立变敌对的势力个数
	int *m_iWantedforceids;					//保存由中立变敌对的势力信息

	int m_iBornCity;								//出生城市的id
	std::vector<bool>findCityStatues;//城市是否被发现的状态保存
	bool m_escortCitySucces;				//护送服务是否成功回到主城
	bool getFindCityStatus(int cityIdIndex);
	void  setFindCityStatus(int cityIdIndex, bool haveFind = false);
	bool initHeroInfo(LoginFirstData* result);//初始化信息
	bool initHeroInfo(EmailLoginResult* result);//初始化信息
	void getHeroBornCity(int nationId);//根据势力id计算城市id
	bool shipGuideEnemy;
	//保存战斗数据
	EngageInFightResult *m_pBattelData;
	Vec2 shippos;									//保存船的坐标
	std::string m_sName;						//保存名字

	int64_t m_iExp;                             //人物经验
	int64_t m_iRexp;                            //声望值
	int64_t m_iSilver;							//新手银币
	int64_t m_iVp;							    //新手银票
	int m_iStage;                               //新手任务进度
	bool m_bSeaToCenter;              //从海上带个人中心flag
	bool m_heroIsOnsea;//是否在海上(用于back键的显示)
	//出海时，最近离开的，有许可证的城市
	int m_iHave_licence_cityId;
	//账户是否验证通过
	bool m_iMyEmailAccountVerified;
	//账户异地登陆是否通过安全检测
	bool m_iAccountSafe;
	//进入海上时不可避免的战斗npc的id
	int m_forceNPCId;
	//是否需要显示离线委托的信息
	bool m_isShowOutLineResult;
	//登录时时候有未完成的战斗
	bool m_lastBattleNotComplete;
	//伦敦的天气
	int m_londonWeather;
	//保存战斗时需要的Boss event id
	int m_nBossEventId;

	int m_nTriggerId;
	//欲让对方加入公会，保存对方的国家id（双方国家id不同，对方不可加入公会）
	int m_InvitedToGuildNation;

	//控制按钮点击的问题
	bool m_bClickTouched;
	//是否在安全海域
	bool m_bInSafeArea;
	//点击主城船损坏图标，直接去船坞修理界面
	bool m_bGoshipFixDirect;
	//是不是GM
	bool m_bGameMaster;
	//新手引导开头对话跳转到酒馆
	bool m_bDialogToTarven;
	//提示消息显示时间
	float m_Infoshowtime;
	//服务器关闭时间
	int   m_waitTime;
	/*
	*国战攻击城市的id
	*/
	int   m_nAttackCityId;

	
	//系统通知走字类型
	int   m_nSystemMessageType;
	//个人中心回到海上
	bool m_centerTosea;

	std::vector<SYSTEM_IFNO> m_vSystemInfo;
};

#endif
