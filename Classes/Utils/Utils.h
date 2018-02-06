/*
 * Utils.h
 *
 *  Created on: 2015年1月7日
 *      Author: eong
 */

#ifndef UTILS_H_
#define UTILS_H_
//#ifdef ANDROID
//#include "TalkingData.h"
//#include "TDCCVirtualCurrency.h"
//#include "TDCCItem.h"
//#include "TDCCMission.h"
//#else
#include <string>
//#endif
class TDCCAccount;

class Utils {
public:
	Utils();
	virtual ~Utils();
	static long gettime();
	static long long getnanotime();
	static int getLanguage();
	static std::string makeTextWithFontSizeAndColor(std::string s, int size, std::string colorStr);
	template <typename T>
	static std::string to_string(T value);
	static void ShowFreshDesk(std::string email, std::string name);
	static void SendStaticsEvent(int value);
	static void TrackEmailMd5(std::string email);
	static std::string getVersionName();
	static std::string getVersionCode();
	static void ShowFacebookInviteDialog(std::string fbInviteCode);
	static std::string GetFaceBookRefcode();
	static std::string& GetDeviceHash();
	static std::string& GetNetWorkString();
	static std::string& GetDeviceInfo();
	static void sendNewPlayerTrackingStep(std::string stepname, int nation);
	//给tdc统计设置玩家名字
//	static void setTDCAccount(std::string Name);
//	static TDCCAccount*getTDCAccount();
	static std::string GetGCMToken();
	static int GetOSType();
	static int getUnreadServiceMsg();
	//花费v票，itemame 购买的东西，count 数量，price单价
	static void consumeVTicket(std::string itemName,int count,float price);
	//获取的奖励v票
	static void rewardVTicket(std::string rewardName,int count);
	//充值,参数分别是：订单号，充值购买的商品，花了多少usd，得到多少v票
	static void onChargeRequest(std::string orderName, std::string chargeItemName,  float usdCount,int vTicketCount);
	//充值成功的回调
	static void onChargeSuccess(std::string sku, std::string data);
	//task start
	static void onTaskBegin(std::string taskName);
	static void onTaskCompleted(std::string taskName);
	static void onTaskFailed(std::string taskName,std::string reason);
	//清理缓存
	static void clearCache();
	static bool shouldUseBackServer();
	//使用etc1
	static std::string GetImagePath(std::string ImageName, int type);
private:
//	static TDCCAccount* TDCAccount;
	static std::string TDGAOrderId;
	static float purchasePrice;
public:
    static void shareToFacebook(std::string fbInviteCode);// 分享内容到Facebook
    static void shareToFreshdesk();
public:
#if CC_TARGET_OS_IPHONE// 判断如果是IOS系统
    
    static void shareToFacebookforIOS(std::string fbInviteCode);
    static void shareToFreshdeskforIOS();
    
public:
    static void setUIViewController(void* viewController);
#endif
};

#endif /* UTILS_H_ */
