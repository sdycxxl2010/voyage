/*
 * Utils.cpp
 *
 *  Created on: 2015年1月7日
 *      Author: eong
 */

#include "Utils.h"
#include "SystemVar.h"
#include "cocos2d.h"
#include "ProtocolThread.h"
#include "env.h"
#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#if CC_TARGET_OS_IPHONE
#include "FaceBookSDK.hpp"
#endif
#endif

using namespace std::chrono;

std::string Utils::TDGAOrderId = "";
float Utils::purchasePrice = 0.0f;

Utils::Utils() {
	//TDCAccount = nullptr;
}

Utils::~Utils() {
}

long Utils::gettime(){
	milliseconds ms = duration_cast< milliseconds >(high_resolution_clock::now().time_since_epoch());
	return ms.count();
}

long long  Utils::getnanotime(){
	nanoseconds ns = duration_cast< nanoseconds >(high_resolution_clock::now().time_since_epoch());
		return ns.count();
}


int Utils::getLanguage(){
	if(ENVIRONMENT==PRODUCTION || ENABLE_LANGUAGES){
		int ret = cocos2d::CCUserDefault::getInstance()->getIntegerForKey(LANGUAGE_KEY, -1);
		log("lang: ret:%d key:%s", ret, ProtocolThread::GetInstance()->getFullKeyName(LANGUAGE_KEY).c_str());
		if(ret == -1){
			return (int)cocos2d::Application::getInstance()->getCurrentLanguage();
		}else{
			return ret;
		}
	}else{
		return (int)cocos2d::LanguageType::ENGLISH;
	}



}

/*
 * 给string按照html规则添加<font>标签，不包括<html>标签。
 * s: 原始string， size: 字体大小， colorStr: #A95601 颜色的string
 */
std::string Utils::makeTextWithFontSizeAndColor(std::string s, int size, std::string colorStr){
#if WIN32
	return s;
#else
	std::string ret = "<font size='";
	ret += Utils::to_string(size);
	ret += "' color='";
	ret += colorStr;
	ret +="'>";
	ret += s;
	ret +="</font>";
	return ret;
#endif
}

//std::string Utils::to_string( int x ) {
//  int length = snprintf( NULL, 0, "%d", x );
//  assert( length >= 0 );
//  char* buf = new char[length + 1];
//  snprintf( buf, length + 1, "%d", x );
//  std::string str( buf );
//  delete[] buf;
//  return str;
//}

template <typename T>
std::string Utils::to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

void Utils::ShowFreshDesk(std::string email, std::string name){
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getInstance","()Lcom/piigames/voyage/AppActivity;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","showFreshDesk","(Ljava/lang/String;Ljava/lang/String;)V");
		//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
		if(id)
		{
			jstring email_j = jniInfo.env->NewStringUTF(email.c_str());
			jstring name_j = jniInfo.env->NewStringUTF(name.c_str());
			jniInfo.env->CallVoidMethod(jobj, jniInfo.methodID, email_j, name_j);
			//jniInfo.env->DeleteLocalRef(jniInfo.classID);
		}
	}
#endif
#if CC_TARGET_OS_IPHONE
     Utils::shareToFreshdesk();
    
#endif
}


void Utils::SendStaticsEvent(int value){
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getInstance","()Lcom/piigames/voyage/AppActivity;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","TutorialEvent","(I)V");
		log("TutorialEvent id:%x,%x",jniInfo.methodID,jniInfo.classID);
		if(id)
		{
			jniInfo.env->CallVoidMethod(jobj, jniInfo.methodID, value);
			//jniInfo.env->DeleteLocalRef(jniInfo.classID);
		}
	}
#endif
}

void Utils::TrackEmailMd5(std::string email){
	if(email.length() == 0){
		return;
	}
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getInstance","()Lcom/piigames/voyage/AppActivity;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","trackUserEmail","(Ljava/lang/String;)V");
		log("trackUserEmail id:%x,%x",jniInfo.methodID,jniInfo.classID);
		if(id)
		{
			jstring refCode = jniInfo.env->NewStringUTF(email.c_str());
			jniInfo.env->CallVoidMethod(jobj, jniInfo.methodID, refCode);
			//jniInfo.env->DeleteLocalRef(jniInfo.classID);
		}
	}
#endif
}

std::string Utils::getVersionName(){
	std::string res;
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getVersionName","()Ljava/lang/String;");
	jstring jobj;
	if (isHave) {
		jobj = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		//convert it to string.
		const char* tmp = minfo.env->GetStringUTFChars(jobj, NULL);
		res = std::string(tmp);
		minfo.env->ReleaseStringUTFChars(jobj, tmp);
	}
#endif
	return res;
}

std::string Utils::getVersionCode(){
	std::string res;
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getVersionCode","()Ljava/lang/String;");
	jstring jobj;
	if (isHave) {
		jobj = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		//convert it to string.
		const char* tmp = minfo.env->GetStringUTFChars(jobj, NULL);
		res = std::string(tmp);
		minfo.env->ReleaseStringUTFChars(jobj, tmp);
	}
#endif
	return res;
}

std::string Utils::GetFaceBookRefcode(){
#ifdef ANDROID
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","getFacebookRefCode","()Ljava/lang/String;");
	//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
	if(id)
	{
		jstring res = (jstring)jniInfo.env->CallStaticObjectMethod(jniInfo.classID, jniInfo.methodID);
		if(res){
			const char* smsList = jniInfo.env->GetStringUTFChars(res, NULL);
			std::string defaultName(smsList);
			jniInfo.env->ReleaseStringUTFChars(res, smsList);
			return defaultName;
		}
	}
#endif
	return "";
}

std::string& Utils::GetDeviceHash(){
	static std::string deviceHash;// = "test device hash";//getUniqueId
#if ANDROID
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","getUniqueId","()Ljava/lang/String;");
	//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
	if(id)
	{
		jstring res = (jstring)jniInfo.env->CallStaticObjectMethod(jniInfo.classID, jniInfo.methodID);
		if(res){
			const char* smsList = jniInfo.env->GetStringUTFChars(res, NULL);
			deviceHash = smsList;
			jniInfo.env->ReleaseStringUTFChars(res, smsList);
		}
	}
#else
	char buffer[256];
	snprintf(buffer,sizeof(buffer),"pc_%d",rand());
	deviceHash = buffer;
#endif
	return deviceHash;
}

void Utils::ShowFacebookInviteDialog(std::string fbInviteCode){
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getInstance","()Lcom/piigames/voyage/AppActivity;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","showFBInviteDialog","(Ljava/lang/String;)V");
		//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
		if(id)
		{
			jstring refCode = jniInfo.env->NewStringUTF(fbInviteCode.c_str());
			jniInfo.env->CallVoidMethod(jobj, jniInfo.methodID, refCode);
			//jniInfo.env->DeleteLocalRef(jniInfo.classID);
		}
	}
#endif
#if CC_TARGET_OS_IPHONE
    Utils::shareToFacebook(fbInviteCode);
#endif
}

std::string& Utils::GetDeviceInfo(){
	static std::string metaData = "pc device info";
#if ANDROID
	//
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","getCustomServiceString","()Ljava/lang/String;");
	//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
	if(id)
	{
		jstring res = (jstring)jniInfo.env->CallStaticObjectMethod(jniInfo.classID, jniInfo.methodID);
		if(res){
			const char* smsList = jniInfo.env->GetStringUTFChars(res, NULL);
			metaData = smsList;
			jniInfo.env->ReleaseStringUTFChars(res, smsList);
		}
	}
#endif
	return metaData;
}

std::string& Utils::GetNetWorkString(){
	static std::string network = "PC";
#if ANDROID
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","getNetworkString","()Ljava/lang/String;");
	//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
	if(id)
	{
		jstring res = (jstring)jniInfo.env->CallStaticObjectMethod(jniInfo.classID, jniInfo.methodID);
		if(res){
			const char* smsList = jniInfo.env->GetStringUTFChars(res, NULL);
			network = smsList;
			jniInfo.env->ReleaseStringUTFChars(res, smsList);
		}
	}
#endif
	return network;
}

void Utils::sendNewPlayerTrackingStep(std::string stepname, int nation){
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getInstance","()Lcom/piigames/voyage/AppActivity;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","sendNewPlayerTrackingStep","(Ljava/lang/String;I)V");
		//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
		if(id)
		{
			jstring refCode = jniInfo.env->NewStringUTF(stepname.c_str());
			jint nation_id = nation;
			jniInfo.env->CallVoidMethod(jobj, jniInfo.methodID, refCode, nation_id);
			//jniInfo.env->DeleteLocalRef(jniInfo.classID);
		}
	}
#endif
}
//task start
void Utils::onTaskBegin(std::string taskName){
#if ANDROID
//	TDCCMission::onBegin(taskName.c_str());
#endif
}

void Utils::onTaskCompleted(std::string taskName){
#if ANDROID
//	TDCCMission::onCompleted(taskName.c_str());
#endif
}

void Utils::onTaskFailed(std::string taskName,std::string reason){
#if ANDROID
//	TDCCMission::onFailed(taskName.c_str(),reason.c_str());
#endif
}

void Utils::consumeVTicket(std::string itemName,int count,float price){
#if ANDROID
//	TDCCItem::onPurchase(itemName.c_str(), count,price);
#endif
}
	//获取的奖励v票
void Utils::rewardVTicket(std::string rewardName,int count){
#if ANDROID
//	TDCCVirtualCurrency::onReward(count,rewardName.c_str());
#endif
}
	//充值,参数分别是：订单号，充值购买的商品，花了多少usd，得到多少v票
void Utils::onChargeRequest(std::string orderName, std::string chargeItemName,  float usdCount,int vTicketCount){
	purchasePrice = usdCount;
#if ANDROID
//	TDGAOrderId = orderName;
//	TDCCVirtualCurrency::onChargeRequest(orderName.c_str(),chargeItemName.c_str(), usdCount,"USD", vTicketCount, "GOOGLE PLAY");
#endif
}
	//充值成功的回调
void Utils::onChargeSuccess(std::string sku, std::string Data){
#if ANDROID
//	TDCCVirtualCurrency::onChargeSuccess(TDGAOrderId.c_str());
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getInstance","()Lcom/piigames/voyage/AppActivity;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","trackIAP","(FLjava/lang/String;)V");
		//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
		if(id)
		{
			jstring data = jniInfo.env->NewStringUTF(Data.c_str());
			jfloat price = purchasePrice;
			jniInfo.env->CallVoidMethod(jobj, jniInfo.methodID, price, data);
			//jniInfo.env->DeleteLocalRef(jniInfo.classID);
		}
	}
#endif
}

std::string Utils::GetImagePath(std::string ImagePath, int type)
{
	Image* image;
	std::string fullPath;
	std::string fullPath1;
	std::string fullPath2;
	switch (type)
	{
	case 1:
		fullPath = ImagePath + ".pkm";
		fullPath1 = ImagePath + ".png";
		fullPath2 = ImagePath + ".jpg";
		if (image->initWithImageFile(fullPath))
		{
			return fullPath;
		}
		else if (image->initWithImageFile(fullPath1))
		{
			return fullPath1;
		}
		else if (image->initWithImageFile(fullPath2))
		{
			return fullPath2;
		}
		break;
	case 2:
		fullPath = ImagePath + ".pkm";
		fullPath1 = ImagePath + ".png";
		fullPath2 = ImagePath + ".jpg";
		if (image->initWithImageFile(fullPath))
		{
			return fullPath;
		}
		else if (image->initWithImageFile(fullPath1))
		{
			return fullPath1;
		}
		else if (image->initWithImageFile(fullPath2))
		{
			return fullPath2;
		}
		break;
	default:
		break;
	}
}
//void Utils::setTDCAccount(std::string Name){
//#if ANDROID
//	TDCAccount = TDCCAccount::setAccount(Name.c_str());
	//TDGAVirtualCurrency.onReward(5,"oooxxx");
	//TDGAItem.onPurchase(“helmet1”, 2, 25);
//#endif
//}
//TDCCAccount* Utils::TDCAccount = nullptr;
//TDCCAccount*Utils::getTDCAccount(){
//	return TDCAccount;
//}

std::string Utils::GetGCMToken(){
#if ANDROID
	std::string token("");
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","GetGCMToken","()Ljava/lang/String;");
	//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
	if(id)
	{
		jstring res = (jstring)jniInfo.env->CallStaticObjectMethod(jniInfo.classID, jniInfo.methodID);
		if(res){
			const char* smsList = jniInfo.env->GetStringUTFChars(res, NULL);
			if(smsList){
				token = smsList;
			}
			jniInfo.env->ReleaseStringUTFChars(res, smsList);
		}
	}
	return token;
#else
	std::string token("");
	return token;
#endif
}

int Utils::GetOSType(){
#if ANDROID
	return 0;
#else
	return 2;
#endif
}

/*
 * Return if the number of unread customer service messages.
 */
int Utils::getUnreadServiceMsg(){
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getInstance","()Lcom/piigames/voyage/AppActivity;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","getUnreadServiceMsg","()I");
		//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
		if(id)
		{
			log("find jni method");
			jint ret = (jint)jniInfo.env->CallIntMethod(jobj, jniInfo.methodID);
			//jniInfo.env->DeleteLocalRef(jniInfo.classID);
			return ret;
		}else{
			log("no such jni method");
		}
	}
#endif
	return 0;
}

void Utils::clearCache(){
#if ANDROID
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"com/piigames/voyage/AppActivity","getInstance","()Lcom/piigames/voyage/AppActivity;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}
	if(jobj){
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","showApplicationInfo","(Ljava/lang/String;)V");
		//cocos2d::cocos2d::log("LaunchPurchase id:%x,%x",jniInfo.methodID,jniInfo.classID);
		if(id)
		{
			log("clearCache find jni method");
			jstring jstr = minfo.env->NewStringUTF("com.android.vending");
			jniInfo.env->CallVoidMethod(jobj, jniInfo.methodID,jstr);
			//jniInfo.env->DeleteLocalRef(jniInfo.classID);
			//return ret;
		}else{
			log("no such jni method");
		}
	}
#endif
}

bool Utils::shouldUseBackServer(){
#if ANDROID
	std::string triggerPath = "/sdcard/useback";
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(triggerPath);
	if (contentStr.length() > 0){
		return true;
	}
#endif
	return false;
}
void Utils::shareToFacebook(std::string fbInviteCode)
{
#if CC_TARGET_OS_IPHONE
    Utils::shareToFacebookforIOS(fbInviteCode);
#endif
}
void Utils::shareToFreshdesk()
{
#if CC_TARGET_OS_IPHONE
    Utils::shareToFreshdeskforIOS();
#endif
}
