#include "ProtocolThread.h"
#include "cocos2d.h"
#include "TVSocketHolder.h"
//#include "BaseResponseMsg.h"
#include "Buffer.h"
#include "zlib.h"
#include "login.pb-c.h"
#include "LuaSailScene.h"
#include "cocostudio/TextResUtils.h"
#include "TVSingle.h"
#include "UITips.h"
#include "MainTaskManager.h"
#include "TVSailDefineVar.h"
#include "Utils.h"
#include "ServerManager.h"
#include "UIRegister.h"
#include "TVSceneLoader.h"
#include "UIStart.h"

#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif

USING_NS_CC;

#define ADD_PROTO_SAMPEL 0

#define USER_DEFAULT_USED_EMAIL "have_valid_email_password"

static long exp_array[] = { 0, 150, 340, 590, 920, 1350, 1900, 2610, 3520, 4680, 6150, 8150, 10410, 12970, 15860, 19120, 22790, 26910, 31530, 36710, 42500, 48970, 56190, 64240, 73210, 83190, 94290,
		106620, 120310, 135490, 152320, 170960, 191590, 214420, 239660, 267560, 298370, 332390, 369930, 411340, 456990, 507300, 562720, 623750, 690930, 764850, 846160, 935580, 1033880, 1141900,
		1381900, 1621900, 1861900, 2101900, 2341900, 2581900, 2821900, 3061900, 3301900, 3541900, 3781900, 4021900, 4261900, 4501900, 4741900, 4981900, 5221900, 5461900, 5701900, 5941900, 6181900,
		6421900, 6661900, 6901900, 7141900, 7381900, 7621900, 7861900, 8101900, 8341900, 8581900, 8821900, 9061900, 9301900, 9541900, 9781900, 10021900, 10261900, 10501900, 10741900, 10981900,
		11221900, 11461900, 11701900, 11941900, 12181900, 12421900, 12661900, 12901900};
static int exp_array_len = sizeof(exp_array)/sizeof(long);

static long fame_array[] = { 0, 100, 210, 340, 500, 690, 910, 1160, 1450, 1780, 2150, 2580, 3060, 3610, 4230, 4930, 5720, 6610, 7600, 8710, 9960, 11350, 12910, 14640,
        16570, 18720, 21110, 23770, 26720, 29990, 33620, 37640, 42090, 47010, 52460, 58480, 65130, 72470, 80570, 89510, 99360, 110220, 122180, 135350, 149850, 165810,
        183360, 202660, 223880, 247200, 487200, 727200, 967200, 1207200, 1447200, 1687200, 1927200, 2167200, 2407200, 2647200, 2887200, 3127200, 3367200, 3607200, 3847200,
        4087200, 4327200, 4567200, 4807200, 5047200, 5287200, 5527200, 5767200, 6007200, 6247200, 6487200, 6727200, 6967200, 7207200, 7447200, 7687200, 7927200, 8167200, 8407200,
        8647200, 8887200, 9127200, 9367200, 9607200, 9847200, 10087200, 10327200, 10567200, 10807200, 11047200, 11287200, 11527200, 11767200, 12007200};

static int fame_array_len = sizeof(fame_array)/sizeof(long);

static int get_level_internal(long exp,long array[],int start_idx,int end_idx){
	if(start_idx >= end_idx){
		return end_idx+1;
	}
	int len = end_idx - start_idx;
	int index = len/2 + start_idx;
	if(exp > array[index]){
		if(exp < array[index+1]){
			return index+1;
		}else{
			return get_level_internal(exp,array,index+1,end_idx);
		}
	}else if(exp < array[index]){
		if(exp>= array[index-1]){
			return index;
		}else{
			return get_level_internal(exp,array,start_idx,index-2);
		}
	}else{//equal
		return index+1;
	}
}

static int get_exp_level_internal(long exp,long array[],int start_idx,int end_idx){
	if(start_idx >= end_idx){
		return end_idx+1;
	}
	int len = end_idx - start_idx;
	int index = len/2 + start_idx;
	if(exp > array[index]*3){
		if(exp < array[index+1]*3){
			return index+1;
		}else{
			return get_exp_level_internal(exp,array,index+1,end_idx);
		}
	}else if(exp < array[index]*3){
		if(exp>= array[index-1]*3){
			return index;
		}else{
			return get_exp_level_internal(exp,array,start_idx,index-2);
		}
	}else{//equal
		return index+1;
	}
}

int ProtocolThread::get_exp_level(long exp){
	if(exp <= 0)
		return 1;
	int lv = get_exp_level_internal(exp,exp_array,0,exp_array_len-1);
	if(lv > 50) lv=50;
	return lv;
}

int ProtocolThread::get_fame_level(long fame){
	if(fame <= 0)
		return 1;
	return get_level_internal(fame,fame_array,0,fame_array_len-1);
}

int64_t ProtocolThread::get_level_exp(int level){
	if(level<=1){
		return 0;
	}else if(level>=exp_array_len-1){
		return exp_array[exp_array_len-1]*3;
	}
	return exp_array[level-1]*3;
}

int64_t ProtocolThread::get_level_fame(int level){
	if(level<=1){
		return 0;
	}else if(level>=fame_array_len-1){
		return fame_array[fame_array_len-1];
	}
	return fame_array[level-1];
}

char*copyCString(const char*src){
	if(!src || !src[0]){
		return 0;
	}

	int len = strlen(src);
	char*res = new char[len +1];
	int i=0;
	for(;i<len;i++){
		res[i] = src[i];
	}
	res[i] = 0;
	return res;
}
/************************************************************************/
/* 
call_native("setTaskDone",0,0,2);
call_native("sailToCity",0,0,16);*/
/************************************************************************/
const char* LuaCallNative(const char*funName,int funSubcode,const char*p1,int p2){
	if(funName){
		ProtocolThread*proto = ProtocolThread::GetInstance();
		proto->setAutoPing(1);
		if(strcmp(funName,"userData") == 0){
			if(p1 && funSubcode == 0){				
				if(strcmp(p1,"getNationName") == 0){
					if(proto->getNationIdx() == 1){
						return copyCString("Portugal");	
					}
					else if (proto->getNationIdx() == 2){
						return copyCString("Spain");
					}
					else if (proto->getNationIdx() == 3){
						return copyCString("England");
					}
					else if (proto->getNationIdx() == 5){
						return copyCString("Genoa");
					}
					else if (proto->getNationIdx() == 4){
						return copyCString("Netherlands");
					}
					else{//Portugal,Spain,England,Netherlands,Genoa ,
						//cityid:1,2,3,4,6
						return copyCString("Portugal");
					}
				}else if(strcmp(p1,"getUserName") == 0){
					return copyCString(proto->getHeroName());
				}else if(strcmp(p1,"getCoinNum") == 0){
					char buffer[256];
					snprintf(buffer,256,"%lld",proto->getCoins());
					return copyCString(buffer);
				}else if(strcmp(p1,"getGoldNum") == 0){
					char buffer[256];
					snprintf(buffer,256,"%lld",proto->getGold());
					return copyCString(buffer);
				}
				else if (strcmp(p1, "getLanguage") == 0){
#if 1
					int nType = Utils::getLanguage();
					char buffer[32];
					snprintf(buffer,sizeof(buffer),"%d",(int)nType);
					return copyCString((const char*)buffer);
#else
					LanguageType nType = Application::getInstance()->getCurrentLanguage();
					if (nType == cocos2d::LanguageType::CHINESE){
						return copyCString("1");
					}else if(nType == cocos2d::LanguageType::TRADITIONAL_CHINESE){
						return copyCString("3");
					}else{
						return copyCString("2");
					}
#endif
				}else if(strcmp(p1,"getHeroIcon") == 0){
					char buffer[32];
					sprintf(buffer,"%d",proto->getIcon());
					return copyCString(buffer);
				}else if(strcmp(p1,"logout") == 0){
					ProtocolThread::GetInstance()->logout();
					ProtocolThread::GetInstance()->saveEmail("");
					ProtocolThread::GetInstance()->savePassword("");
					auto scene = UIStart::createScene();
					Director::getInstance()->replaceScene(scene);
				}else if(strcmp(p1,"exit") == 0){
					Director::getInstance()->end();
				}
			}
		}else if(strcmp(funName,"setTaskDone") == 0){
			proto->setMainTaskStatus((char*)p1,p2);
		}else if(strcmp(funName,"sailToCity") == 0){
			//proto->setMainTaskStatus(p2);
			auto scene = LuaSailScene::create(p2);
			Director::getInstance()->replaceScene(scene);
		}else if(strcmp(funName,"getJsonText") == 0){
			cocostudio::TextData t;
			cocostudio::TextResUtils::getInstance()->getText(std::string(p1), &t);//label_advanced_repair_375209
			return copyCString(t.text.c_str());
		}else if(strcmp(funName,"getFullKeyName") == 0){
			auto full = ProtocolThread::GetInstance()->getFullKeyName(p1);
			return copyCString(full.c_str());
		}
	}
	return copyCString("nil");
}

ProtocolThread* ProtocolThread::m_pInstance=0;
ProtocolThread* ProtocolThread::GetInstance(){	
	if(!m_pInstance){
		m_pInstance = new ProtocolThread;
	}
	return m_pInstance;
}

class UserSession{
public:
	UserSession(int userId,int token)
	{
		mUserId = userId;
		mToken = token;
		mCharacterId = 0;
	}
	void setCharacterId(int cid){mCharacterId = cid;}
	int getUserId(){return mUserId;}
	int getSessionId(){return mToken;}
	int getCharacterId(){return mCharacterId;}
private:
	int mUserId;
	int mToken;
	int mCharacterId;
};

ProtocolThread::ProtocolThread(void)
{
	m_serverUrlHash = 0;
	m_serverUrl = "testfronter.voyage.piistudio.com";
	m_googleAccountIsChecking = false;
	m_isWaitingForGoogleToken = false;
	m_googleAccountLogResult = 0;
	m_seaIndex = 0;
	m_serverPort = 0;
	m_chatPort = 0;
	m_loginEmail.clear();
	m_vBuffer = NULL;
	init();
}

void ProtocolThread::init(){
	idleSeconds = 0;
	mUserSession = 0;//new UserSession;
	pendingPassword = 0;
	pendingEmail = 0;
	mLoadingLayer = 0;
	gamePaused = false;
	connectionLost = false;
	mLastLoginResult = 0;
	mGetMailListResult = 0;
	mLastLoginResult = 0;
	gettimeofday(&lastCheckMailTime, nullptr);
	gettimeofday(&lastPingTime,nullptr);
	isWaitingResponse = true;
	selfDisconnect = false;
	chatMsgFunc = nullptr;
	chatServerOK = false;
	nationIdx = -1;
	guildId = -1;
	seaCord.x = -1;
	seaCord.y = -1;
	seaAreaIdx = -1;
	icon = 0;
	waitingReponseSeconds = -1;
	m_isInTutorial = false;
	m_mainTaskStatus = 0;
	heroName = 0;
	m_rc4KeyEnc = 0;
	m_rc4KeyDec = 0;
	m_lastCheckMailBoxResult = nullptr;
	m_lastSendChatTime = 0;
	m_autoPing = 0;
	mBackupLoadingLayer = 0;
}

ProtocolThread::~ProtocolThread(void)
{
	if(mLastLoginResult){
		protobuf_c_message_free_unpacked((ProtobufCMessage *)mLastLoginResult,0);
	}
}

void ProtocolThread::broadcastConnectionDisconnected(){
	connectionLost = true;
	if(selfDisconnect){
		selfDisconnect = false;
	}else{
		broadcastConnectionFailed();
	}
}

void ProtocolThread::notifyGoogleLoginResult(int succ)
{
	//log("notifyGoogleLoginResult %s",succ);
	m_googleAccountLogResult = succ;
	m_googleAccountIsChecking = false;
#if 1
	if(m_googleLoginCallback){
		m_googleLoginCallback(succ);
	}
#else
	if(!succ && m_isWaitingForGoogleToken){
		log("notifyGoogleLoginResult failed,goto register page");
		m_isWaitingForGoogleToken = false;
		ProtocolThread::GetInstance()->unregisterMessageCallback(Director::getInstance()->getRunningScene());
		auto scene = UIRegister::createScene();
		Director::getInstance()->replaceScene(scene);
	}
#endif
}

int ProtocolThread::hasValidEmailAndPassword(){
	std::string acc = cocos2d::CCUserDefault::getInstance()->getStringForKey(USER_DEFAULT_USED_EMAIL);
	if(acc.length() > 0 && acc.compare("1") == 0){
		return 1;
	}
	return 0;
}

bool ProtocolThread::getConnectionStatus(){
	return TVSocketHolder::getInstance()->isConnected() || TVSocketHolder::getInstance()->isConnecting();
}

void ProtocolThread::connectionStatusChanged(bool hasNetwork){
	if(!TVSocketHolder::getInstance()->isConnected())
		return;
	//log("connectionStatusChanged:%d",hasNetwork);
	BaseResponseMsg* basmsg=new BaseResponseMsg();
	basmsg->message = 0;
	basmsg->msgType = -1;	
	if(hasNetwork){
		basmsg->msgType = -2;
	}
	TVSocketHolder::getInstance()->AppendBackendRspMsg(basmsg);

	//auto scheduler = Director::getInstance()->getScheduler();
	//scheduler->performFunctionInCocosThread(CC_CALLBACK_0(ProtocolThread::dispatchResponseCallbacks, this,basmsg));  
}

void ProtocolThread::broadcastConnectionSucceeded()
{
//	if (mLoadingLayer){
//		mLoadingLayer->removeSelf();
//		mLoadingLayer = nullptr;
//	}
	BaseResponseMsg* basmsg=new BaseResponseMsg();
	basmsg->message = 0;
	basmsg->msgType = 0;

	TVSocketHolder::getInstance()->AppendBackendRspMsg(basmsg);

	//auto scheduler = Director::getInstance()->getScheduler();
	//scheduler->performFunctionInCocosThread(CC_CALLBACK_0(ProtocolThread::dispatchResponseCallbacks, this,basmsg));   
}


void ProtocolThread::broadcastConnectionFailed()
{
//	if (mLoadingLayer){
//		mLoadingLayer->removeSelf();
//		mLoadingLayer = nullptr;
//	}
	BaseResponseMsg* basmsg=new BaseResponseMsg();
	basmsg->message = 0;
	basmsg->msgType = -1;

	TVSocketHolder::getInstance()->AppendBackendRspMsg(basmsg);
	//auto scheduler = Director::getInstance()->getScheduler();
	//scheduler->performFunctionInCocosThread(CC_CALLBACK_0(ProtocolThread::dispatchResponseCallbacks, this,basmsg));   
}

void ProtocolThread::processSocketMessage(void * param){    	
	isWaitingResponse = false;
	ProcessSocketFunc();
} 

ProtobufCMessage* protobuf_internal_get_message(int msg_type,const uint8_t*buffer,int data_len) {
	//libev_callback_func process_func = 0;
	switch(msg_type){
            PROTO_UNPACK_CASE(EmailLogin,email_login)
            PROTO_UNPACK_CASE(Activity,activity)
            PROTO_UNPACK_CASE(Task,task)
            PROTO_UNPACK_CASE(SysMessage,sys_message)
            PROTO_UNPACK_CASE(City,city)
            PROTO_UNPACK_CASE(LoginFirstData,login_first_data)
            PROTO_UNPACK_CASE(EmailLoginResult,email_login_result)
            PROTO_UNPACK_CASE(RegisterUser,register_user)
#if ADD_PROTO_SAMPEL
            PROTO_UNPACK_CASE(ProtoSample,proto_sample)
            PROTO_UNPACK_CASE(ProtoSampleResult,proto_sample_result)
#endif
	//PROTO_UNPACK_CASE(SailCityFinish,sail_city_finish)
	PROTO_UNPACK_CASE(RegisterUserResult,register_user_result)
	PROTO_UNPACK_CASE(CreateCharacter,create_character)
	PROTO_UNPACK_CASE(CreateCharacterFailed,create_character_failed)
	PROTO_UNPACK_CASE(GetFleetCargo,get_fleet_cargo)
	PROTO_UNPACK_CASE(GetFleetCargoResult,get_fleet_cargo_result)
	PROTO_UNPACK_CASE(GetGoodsForShip,get_goods_for_ship)
	PROTO_UNPACK_CASE(GetGoodsForShipResult,get_goods_for_ship_result)
	PROTO_UNPACK_CASE(ExchangeGoodsForShips,exchange_goods_for_ships)
	PROTO_UNPACK_CASE(ExchangeGoodsForShipsResult,exchange_goods_for_ships_result)
	PROTO_UNPACK_CASE(CheckMailBox,check_mail_box)
	PROTO_UNPACK_CASE(CheckMailBoxResult,check_mail_box_result)
	PROTO_UNPACK_CASE(GetMailList,get_mail_list)
	PROTO_UNPACK_CASE(GetMailListResult,get_mail_list_result)
	PROTO_UNPACK_CASE(SendMail,send_mail)
	PROTO_UNPACK_CASE(SendMailResult,send_mail_result)
	PROTO_UNPACK_CASE(MarkMailStatus,mark_mail_status)
	PROTO_UNPACK_CASE(MarkMailStatusResult,mark_mail_status_result)
	    PROTO_UNPACK_CASE(GetItemsBeingSold,get_items_being_sold)
	    PROTO_UNPACK_CASE(GetItemsBeingSoldResult,get_items_being_sold_result)
	    PROTO_UNPACK_CASE(GetLastPrice,get_last_price)
	    PROTO_UNPACK_CASE(GetLastPriceResult,get_last_price_result)
	    PROTO_UNPACK_CASE(GetOrderList,get_order_list)
	    PROTO_UNPACK_CASE(GetOrderListResult,get_order_list_result)
	    PROTO_UNPACK_CASE(BuyItem,buy_item)
	    PROTO_UNPACK_CASE(BuyItemResult,buy_item_result)
	    PROTO_UNPACK_CASE(SellItem,sell_item)
	    PROTO_UNPACK_CASE(SellItemResult,sell_item_result)
	    PROTO_UNPACK_CASE(CreateOrder,create_order)
	    PROTO_UNPACK_CASE(CreateOrderResult,create_order_result)
	    PROTO_UNPACK_CASE(SearchTradeItems,search_trade_items)
	    PROTO_UNPACK_CASE(SearchTradeItemsResult,search_trade_items_result)
	    PROTO_UNPACK_CASE(GetItemsToSell,get_items_to_sell)
    	    PROTO_UNPACK_CASE(GetItemsToSellResult,get_items_to_sell_result)
	PROTO_UNPACK_CASE(GetPriceData,get_price_data)
    	PROTO_UNPACK_CASE(GetPriceDataResult,get_price_data_result)
		PROTO_UNPACK_CASE(GetPersonalItem,get_personal_item)
		PROTO_UNPACK_CASE(GetPersonalItemResult,get_personal_item_result)
	PROTO_UNPACK_CASE(CancelOrder, cancel_order)
    	PROTO_UNPACK_CASE(CancelOrderResult, cancel_order_result)
    	PROTO_UNPACK_CASE(WarehouseOpration, warehouse_opration)
	PROTO_UNPACK_CASE(WarehouseOprationResult, warehouse_opration_result)
  	PROTO_UNPACK_CASE(GetWarehouseItems, get_warehouse_items)
	PROTO_UNPACK_CASE(GetWarehouseItemsResult, get_warehouse_items_result)
	PROTO_UNPACK_CASE(BarConsume, bar_consume)
	PROTO_UNPACK_CASE(BarConsumeResult, bar_consume_result)
	PROTO_UNPACK_CASE(GetBarInfo, get_bar_info)
	PROTO_UNPACK_CASE(GetBarInfoResult, get_bar_info_result)
	PROTO_UNPACK_CASE(GetCrew, get_crew)
	PROTO_UNPACK_CASE(GetCrewResult, get_crew_result)
	PROTO_UNPACK_CASE(TreatBargirl, treat_bargirl)
	PROTO_UNPACK_CASE(TreatBargirlResult, treat_bargirl_result)
	PROTO_UNPACK_CASE(GiveBargirlGift, give_bargirl_gift)
	PROTO_UNPACK_CASE(GiveBargirlGiftResult, give_bargirl_gift_result)
	PROTO_UNPACK_CASE(GetAvailCrewNum, get_avail_crew_num)
	PROTO_UNPACK_CASE(GetAvailCrewNumResult, get_avail_crew_num_result)
	PROTO_UNPACK_CASE(GetTaskList, get_task_list)
	PROTO_UNPACK_CASE(GetTaskListResult, get_task_list_result)
	PROTO_UNPACK_CASE(GetIntelligence, get_intelligence)
	PROTO_UNPACK_CASE(GetIntelligenceResult, get_intelligence_result)
	PROTO_UNPACK_CASE(GetCaptain, get_captain)
	PROTO_UNPACK_CASE(GetCaptainResult, get_captain_result)
	PROTO_UNPACK_CASE(HandleTask, handle_task)
	PROTO_UNPACK_CASE(HandleTaskResult, handle_task_result)
	PROTO_UNPACK_CASE(GetCurrentCityData, get_current_city_data)
	PROTO_UNPACK_CASE(GetCurrentCityDataResult, get_current_city_data_result)
	PROTO_UNPACK_CASE(GetShipList, get_ship_list)
	PROTO_UNPACK_CASE(GetShipListResult, get_ship_list_result)
	PROTO_UNPACK_CASE(RepairShip, repair_ship)
	PROTO_UNPACK_CASE(RepairShipResult, repair_ship_result)
	PROTO_UNPACK_CASE(RepairAllShips, repair_all_ships)
	PROTO_UNPACK_CASE(RepairAllShipsResult, repair_all_ships_result)
	PROTO_UNPACK_CASE(FleetFormation, fleet_formation)
	PROTO_UNPACK_CASE(FleetFormationResult, fleet_formation_result)
	PROTO_UNPACK_CASE(ParkShip, park_ship)
	PROTO_UNPACK_CASE(ParkShipResult, park_ship_result)
	PROTO_UNPACK_CASE(GetDrawings, get_drawings)
	PROTO_UNPACK_CASE(GetDrawingsResult, get_drawings_result)
	PROTO_UNPACK_CASE(BuildShip, build_ship)
	PROTO_UNPACK_CASE(BuildShipResult, build_ship_result)
	PROTO_UNPACK_CASE(GetBuildingShips, get_building_ships)
	PROTO_UNPACK_CASE(GetBuildingShipsResult, get_building_ships_result)
	PROTO_UNPACK_CASE(FinishBuilding, finish_building)
	PROTO_UNPACK_CASE(FinishBuildingResult, finish_building_result)
	PROTO_UNPACK_CASE(CancelBuild, cancel_build)
	PROTO_UNPACK_CASE(CancelBuildResult, cancel_build_result)
	PROTO_UNPACK_CASE(GetBuildingEquipment, get_building_equipment)
	PROTO_UNPACK_CASE(GetBuildingEquipmentResult, get_building_equipment_result)
	PROTO_UNPACK_CASE(GetFleetAndDockShips, get_fleet_and_dock_ships)
	PROTO_UNPACK_CASE(GetFleetAndDockShipsResult, get_fleet_and_dock_ships_result)
	PROTO_UNPACK_CASE(GetReinforceShipList, get_reinforce_ship_list)
	PROTO_UNPACK_CASE(GetReinforceShipListResult, get_reinforce_ship_list_result)
	PROTO_UNPACK_CASE(ReinforceShip, reinforce_ship)
	PROTO_UNPACK_CASE(ReinforceShipResult, reinforce_ship_result)
	PROTO_UNPACK_CASE(RerollShip, reroll_ship)
	PROTO_UNPACK_CASE(RerollShipResult, reroll_ship_result)
	PROTO_UNPACK_CASE(GetReinforceEquipList, get_reinforce_equip_list)
	PROTO_UNPACK_CASE(GetReinforceEquipListResult, get_reinforce_equip_list_result)
	PROTO_UNPACK_CASE(ReinforceEquip, reinforce_equip)
	PROTO_UNPACK_CASE(ReinforceEquipResult, reinforce_equip_result)
	PROTO_UNPACK_CASE(RerollEquip, reroll_equip)
	PROTO_UNPACK_CASE(RerollEquipResult, reroll_equip_result)
	PROTO_UNPACK_CASE(EquipShip, equip_ship)
	PROTO_UNPACK_CASE(EquipShipResult, equip_ship_result)
	PROTO_UNPACK_CASE(GetEquipShipInfo, get_equip_ship_info)
	PROTO_UNPACK_CASE(GetEquipShipInfoResult, get_equip_ship_info_result)
	PROTO_UNPACK_CASE(GetEquipableItems, get_equipable_items)
	PROTO_UNPACK_CASE(GetEquipableItemsResult, get_equipable_items_result)
	PROTO_UNPACK_CASE(GetVisitedCities, get_visited_cities)
	PROTO_UNPACK_CASE(GetVisitedCitiesResult, get_visited_cities_result)
	PROTO_UNPACK_CASE(AddSupply, add_supply)
	PROTO_UNPACK_CASE(AddSupplyResult, add_supply_result)
	PROTO_UNPACK_CASE(GetNextMapBlock, get_next_map_block)
	PROTO_UNPACK_CASE(GetNextMapBlockResult, get_next_map_block_result)
	PROTO_UNPACK_CASE(LeaveCity, leave_city)
	PROTO_UNPACK_CASE(LeaveCityResult, leave_city_result)
	PROTO_UNPACK_CASE(ReachCity, reach_city)
	PROTO_UNPACK_CASE(ReachCityResult, reach_city_result)
	PROTO_UNPACK_CASE(TriggerSeaEvent, trigger_sea_event)
	PROTO_UNPACK_CASE(TriggerSeaEventResult, trigger_sea_event_result)
	PROTO_UNPACK_CASE(FinishSeaEvent, finish_sea_event)
	PROTO_UNPACK_CASE(FinishSeaEventResult, finish_sea_event_result)
	PROTO_UNPACK_CASE(GetPalaceInfo, get_palace_info)
	PROTO_UNPACK_CASE(GetPalaceInfoResult, get_palace_info_result)
	PROTO_UNPACK_CASE(InvestCity, invest_city)
	PROTO_UNPACK_CASE(InvestCityResult, invest_city_result)
	PROTO_UNPACK_CASE(GetRankList, get_rank_list)
	PROTO_UNPACK_CASE(GetRankListResult, get_rank_list_result)
	PROTO_UNPACK_CASE(VoteMayor, vote_mayor)
	PROTO_UNPACK_CASE(VoteMayorResult, vote_mayor_result)
	PROTO_UNPACK_CASE(ChangeDevDir, change_dev_dir)
	PROTO_UNPACK_CASE(ChangeDevDirResult, change_dev_dir_result)
	PROTO_UNPACK_CASE(GetVoteList, get_vote_list)
	PROTO_UNPACK_CASE(GetVoteListResult, get_vote_list_result)
	PROTO_UNPACK_CASE(GetLabourUnion, get_labour_union)
	PROTO_UNPACK_CASE(GetLabourUnionResult, get_labour_union_result)
	PROTO_UNPACK_CASE(CreateLabourUnion, create_labour_union)
	PROTO_UNPACK_CASE(CreateLabourUnionResult, create_labour_union_result)
	PROTO_UNPACK_CASE(DeleteLabourUnion, delete_labour_union)
	PROTO_UNPACK_CASE(DeleteLabourUnionResult, delete_labour_union_result)
	PROTO_UNPACK_CASE(InviteLabour, invite_labour)
	PROTO_UNPACK_CASE(InviteLabourResult, invite_labour_result)
	PROTO_UNPACK_CASE(ApplyLabourUnion, apply_labour_union)
	PROTO_UNPACK_CASE(ApplyLabourUnionResult, apply_labour_union_result)
	PROTO_UNPACK_CASE(QuitLabourUnion, quit_labour_union)
	PROTO_UNPACK_CASE(QuitLabourUnionResult, quit_labour_union_result)
	PROTO_UNPACK_CASE(KickOutMember, kick_out_member)
	PROTO_UNPACK_CASE(KickOutMemberResult, kick_out_member_result)
	PROTO_UNPACK_CASE(AdjustPrivilege, adjust_privilege)
	PROTO_UNPACK_CASE(AdjustPrivilegeResult, adjust_privilege_result)
	PROTO_UNPACK_CASE(DepositMoney, deposit_money)
	PROTO_UNPACK_CASE(DepositMoneyResult, deposit_money_result)
	PROTO_UNPACK_CASE(WithdrawMoney, withdraw_money)
	PROTO_UNPACK_CASE(WithdrawMoneyResult, withdraw_money_result)
	PROTO_UNPACK_CASE(DepositItem, deposit_item)
	PROTO_UNPACK_CASE(DepositItemResult, deposit_item_result)
	PROTO_UNPACK_CASE(WithdrawItem, withdraw_item)
	PROTO_UNPACK_CASE(WithdrawItemResult, withdraw_item_result)
	PROTO_UNPACK_CASE(ExpandBank, expand_bank)
	PROTO_UNPACK_CASE(ExpandBankResult, expand_bank_result)
	PROTO_UNPACK_CASE(GetBankInfo, get_bank_info)
	PROTO_UNPACK_CASE(GetBankInfoResult, get_bank_info_result)
	PROTO_UNPACK_CASE(GetBankLog, get_bank_log)
	PROTO_UNPACK_CASE(GetBankLogResult, get_bank_log_result)
	PROTO_UNPACK_CASE(SetMainTaskStatus,set_main_task_status)
	PROTO_UNPACK_CASE(SetMainTaskStatusResult,set_main_task_status_result)
	PROTO_UNPACK_CASE(EngageInFight, engage_in_fight)
	PROTO_UNPACK_CASE(EngageInFightResult, engage_in_fight_result)
	PROTO_UNPACK_CASE(EndFight, end_fight)
	PROTO_UNPACK_CASE(EndFightResult, end_fight_result)
	PROTO_UNPACK_CASE(StartAutoFight, start_auto_fight)
	PROTO_UNPACK_CASE(StartAutoFightResult, start_auto_fight_result)
	PROTO_UNPACK_CASE(GetSkillsDetails, get_skills_details)
	PROTO_UNPACK_CASE(GetSkillsDetailsResult, get_skills_details_result)
	PROTO_UNPACK_CASE(AddSkillPoint, add_skill_point)
	PROTO_UNPACK_CASE(AddSkillPointResult, add_skill_point_result)
	PROTO_UNPACK_CASE(ResetSkillPoints, reset_skill_points)
	PROTO_UNPACK_CASE(ResetSkillPointsResult, reset_skill_points_result)
	PROTO_UNPACK_CASE(NotifyBattleLog, notify_battle_log)
	PROTO_UNPACK_CASE(NotifyBattleLogResult, notify_battle_log_result)
	PROTO_UNPACK_CASE(GetPersonalInfo, get_personal_info)
	PROTO_UNPACK_CASE(GetPersonalInfoResult, get_personal_info_result)
	PROTO_UNPACK_CASE(SetBioInfo, set_bio_info)
	PROTO_UNPACK_CASE(SetBioInfoResult, set_bio_info_result)
	PROTO_UNPACK_CASE(CancelFight, cancel_fight)
	PROTO_UNPACK_CASE(CancelFightResult, cancel_fight_result)
	PROTO_UNPACK_CASE(GetGuildList, get_guild_list)
	PROTO_UNPACK_CASE(GetGuildListResult, get_guild_list_result)
	PROTO_UNPACK_CASE(GetMyGuildDetails, get_my_guild_details)
	PROTO_UNPACK_CASE(GetMyGuildDetailsResult, get_my_guild_details_result)
	PROTO_UNPACK_CASE(GetApplyInfo, get_apply_info)
	PROTO_UNPACK_CASE(GetApplyInfoResult, get_apply_info_result)
	PROTO_UNPACK_CASE(CreateNewGuild, create_new_guild)
	PROTO_UNPACK_CASE(CreateNewGuildResult, create_new_guild_result)
	PROTO_UNPACK_CASE(JoinGuildRequest, join_guild_request)
	PROTO_UNPACK_CASE(JoinGuildRequestResult, join_guild_request_result)
	PROTO_UNPACK_CASE(GetGuildDetails, get_guild_details)
	PROTO_UNPACK_CASE(GetGuildDetailsResult, get_guild_details_result)
	PROTO_UNPACK_CASE(ChangeGuildIntro, change_guild_intro)
	PROTO_UNPACK_CASE(ChangeGuildIntroResult, change_guild_intro_result)
	PROTO_UNPACK_CASE(ExpandGuildCapacity, expand_guild_capacity)
	PROTO_UNPACK_CASE(ExpandGuildCapacityResult, expand_guild_capacity_result)
	PROTO_UNPACK_CASE(GetGuildMemberPermission, get_guild_member_permission)
	PROTO_UNPACK_CASE(GetGuildMemberPermissionResult, get_guild_member_permission_result)
	PROTO_UNPACK_CASE(SetGuildMemberPermission, set_guild_member_permission)
	PROTO_UNPACK_CASE(SetGuildMemberPermissionResult, set_guild_member_permission_result)
	PROTO_UNPACK_CASE(GetHiredCaptains, get_hired_captains)
	PROTO_UNPACK_CASE(GetHiredCaptainsResult, get_hired_captains_result)
	PROTO_UNPACK_CASE(DealWithGuildJoin, deal_with_guild_join)
	PROTO_UNPACK_CASE(DealWithGuildJoinResult, deal_with_guild_join_result)
	PROTO_UNPACK_CASE(ChangeGuildAnnouncement, change_guild_announcement)
	PROTO_UNPACK_CASE(ChangeGuildAnnouncementResult, change_guild_announcement_result)
	PROTO_UNPACK_CASE(RemoveGuildMember, remove_guild_member)
	PROTO_UNPACK_CASE(RemoveGuildMemberResult, remove_guild_member_result)
	PROTO_UNPACK_CASE(ExitFromGuild, exit_from_guild)
	PROTO_UNPACK_CASE(ExitFromGuildResult, exit_from_guild_result)
	PROTO_UNPACK_CASE(DismissGuild, dismiss_guild)
	PROTO_UNPACK_CASE(DismissGuildResult, dismiss_guild_result)
	PROTO_UNPACK_CASE(GetSearchGuildList, get_search_guild_list)
	PROTO_UNPACK_CASE(GetSearchGuildListResult, get_search_guild_list_result)
	PROTO_UNPACK_CASE(RefreshCaptainList, refresh_captain_list)
	PROTO_UNPACK_CASE(RefreshCaptainListResult, refresh_captain_list_result)
	PROTO_UNPACK_CASE(GetFriendsList, get_friends_list)
	PROTO_UNPACK_CASE(GetFriendsListResult, get_friends_list_result)
	PROTO_UNPACK_CASE(GetBlackList, get_black_list)
	PROTO_UNPACK_CASE(GetBlackListResult, get_black_list_result)
	PROTO_UNPACK_CASE(FriendsOperation, friends_operation)
	PROTO_UNPACK_CASE(FriendsOperationResult, friends_operation_result)
	PROTO_UNPACK_CASE(GetFriendsReqList, get_friends_req_list)
	PROTO_UNPACK_CASE(GetFriendsReqListResult, get_friends_req_list_result)
	PROTO_UNPACK_CASE(FireCaptain, fire_captain)
	PROTO_UNPACK_CASE(FireCaptainResult, fire_captain_result)
	PROTO_UNPACK_CASE(DestoryItems, destory_items)
	PROTO_UNPACK_CASE(DestoryItemsResult, destory_items_result)
	PROTO_UNPACK_CASE(SearchUserByName, search_user_by_name)
	PROTO_UNPACK_CASE(SearchUserByNameResult, search_user_by_name_result)
	PROTO_UNPACK_CASE(TakeAllAttachment, take_all_attachment)
	PROTO_UNPACK_CASE(TakeAllAttachmentResult, take_all_attachment_result)
	PROTO_UNPACK_CASE(GetItemsDetailInfo, get_items_detail_info)
	PROTO_UNPACK_CASE(GetItemsDetailInfoResult, get_items_detail_info_result)
	PROTO_UNPACK_CASE(GetDevGoodsInfo, get_dev_goods_info)
	PROTO_UNPACK_CASE(GetDevGoodsInfoResult, get_dev_goods_info_result)
	PROTO_UNPACK_CASE(ProvideDevGoods, provide_dev_goods)
	PROTO_UNPACK_CASE(ProvideDevGoodsResult, provide_dev_goods_result)
	PROTO_UNPACK_CASE(GetDevGoodsCount, get_dev_goods_count)
	PROTO_UNPACK_CASE(GetDevGoodsCountResult, get_dev_goods_count_result)
	PROTO_UNPACK_CASE(GetFriendValue, get_friend_value)
	PROTO_UNPACK_CASE(GetFriendValueResult, get_friend_value_result)
	PROTO_UNPACK_CASE(GetUserInfoById, get_user_info_by_id)
	PROTO_UNPACK_CASE(GetUserInfoByIdResult, get_user_info_by_id_result)
	PROTO_UNPACK_CASE(TestCreateNewGuild, test_create_new_guild)
	PROTO_UNPACK_CASE(TestCreateNewGuildResult, test_create_new_guild_result)
	PROTO_UNPACK_CASE(InviteUserToGuild, invite_user_to_guild)
	PROTO_UNPACK_CASE(InviteUserToGuildResult, invite_user_to_guild_result)
	PROTO_UNPACK_CASE(GetInvitationList, get_invitation_list)
	PROTO_UNPACK_CASE(GetInvitationListResult, get_invitation_list_result)
	PROTO_UNPACK_CASE(DealWithInvitation, deal_with_invitation)
	PROTO_UNPACK_CASE(DealWithInvitationResult, deal_with_invitation_result)
	PROTO_UNPACK_CASE(GetVTicketMarketItems, get_vticket_market_items)
	PROTO_UNPACK_CASE(GetVTicketMarketItemsResult, get_vticket_market_items_result)
	PROTO_UNPACK_CASE(BuyVTicketMarketItem, buy_vticket_market_item)
	PROTO_UNPACK_CASE(BuyVTicketMarketItemResult, buy_vticket_market_item_result)
	PROTO_UNPACK_CASE(GetCurrentInvestData, get_current_invest_data)
	PROTO_UNPACK_CASE(GetCurrentInvestDataResult, get_current_invest_data_result)
	PROTO_UNPACK_CASE(GetUserTasks, get_user_tasks)
	PROTO_UNPACK_CASE(GetUserTasksResult, get_user_tasks_result)
	PROTO_UNPACK_CASE(UseSudanMask, use_sudan_mask)
	PROTO_UNPACK_CASE(UseSudanMaskResult, use_sudan_mask_result)
	PROTO_UNPACK_CASE(EquipHero, equip_hero)
	PROTO_UNPACK_CASE(EquipHeroResult, equip_hero_result)
	PROTO_UNPACK_CASE(GetHeroEquip, get_hero_equip)
	PROTO_UNPACK_CASE(GetHeroEquipResult, get_hero_equip_result)
	PROTO_UNPACK_CASE(BuyCityLicense, buy_city_license)
	PROTO_UNPACK_CASE(BuyCityLicenseResult, buy_city_license_result)
	PROTO_UNPACK_CASE(DeleteUser, delete_user)
	PROTO_UNPACK_CASE(DeleteUserResult, delete_user_result)
	PROTO_UNPACK_CASE(ChangePassword, change_password)
	PROTO_UNPACK_CASE(ChangePasswordResult, change_password_result)
	PROTO_UNPACK_CASE(SelectHeroPositiveSkill, select_hero_positive_skill)
	PROTO_UNPACK_CASE(SelectHeroPositiveSkillResult, select_hero_positive_skill_result)
	PROTO_UNPACK_CASE(GetHeroPositiveSkillList, get_hero_positive_skill_list)
	PROTO_UNPACK_CASE(GetHeroPositiveSkillListResult, get_hero_positive_skill_list_result)
	PROTO_UNPACK_CASE(GetSelectedPositiveSkills, get_selected_positive_skills)
	PROTO_UNPACK_CASE(GetSelectedPositiveSkillsResult, get_selected_positive_skills_result)
	PROTO_UNPACK_CASE(GetSailInfo, get_sail_info)
	PROTO_UNPACK_CASE(GetSailInfoResult, get_sail_info_result)
	PROTO_UNPACK_CASE(InsertUserName, insert_user_name)
	PROTO_UNPACK_CASE(InsertUserNameResult, insert_user_name_result)
	PROTO_UNPACK_CASE(GetLeaderboard, get_leaderboard)
	PROTO_UNPACK_CASE(GetLeaderboardResult, get_leaderboard_result)
	PROTO_UNPACK_CASE(GetCityStatus, get_city_status)
	PROTO_UNPACK_CASE(GetCityStatusResult, get_city_status_result)
	PROTO_UNPACK_CASE(GetDiscoveredCities, get_discovered_cities)
	PROTO_UNPACK_CASE(GetDiscoveredCitiesResult, get_discovered_cities_result)
	PROTO_UNPACK_CASE(AddDiscoveredCity, add_discovered_city)
	PROTO_UNPACK_CASE(AddDiscoveredCityResult, add_discovered_city_result)
	PROTO_UNPACK_CASE(ForceFightStatus, force_fight_status)
	PROTO_UNPACK_CASE(ForceFightStatusResult, force_fight_status_result)
	PROTO_UNPACK_CASE(GetCityProduces, get_city_produces)
	PROTO_UNPACK_CASE(GetCityProducesResult, get_city_produces_result)
	PROTO_UNPACK_CASE(GetCityDemands, get_city_demands)
	PROTO_UNPACK_CASE(GetCityDemandsResult, get_city_demands_result)
	PROTO_UNPACK_CASE(GetMainTask, get_main_task)
	PROTO_UNPACK_CASE(GetMainTaskResult, get_main_task_result)
	PROTO_UNPACK_CASE(CompleteMainTask, complete_main_task)
	PROTO_UNPACK_CASE(CompleteMainTaskResult, complete_main_task_result)
	PROTO_UNPACK_CASE(GetMainTaskReward, get_main_task_reward)
	PROTO_UNPACK_CASE(GetMainTaskRewardResult, get_main_task_reward_result)
	PROTO_UNPACK_CASE(GetEventTaskList, get_event_task_list)
	PROTO_UNPACK_CASE(GetEventTaskListResult, get_event_task_list_result)
	PROTO_UNPACK_CASE(HandleEventTask, handle_event_task)
	PROTO_UNPACK_CASE(HandleEventTaskResult, handle_event_task_result)
	PROTO_UNPACK_CASE(GetNpcScore, get_npc_score)
	PROTO_UNPACK_CASE(GetNpcScoreResult, get_npc_score_result)
	PROTO_UNPACK_CASE(GetServerInfo, get_server_info)
	PROTO_UNPACK_CASE(GetServerInfoResult, get_server_info_result)
	PROTO_UNPACK_CASE(PingServer, ping_server)
	PROTO_UNPACK_CASE(PingServerResult, ping_server_result)
	PROTO_UNPACK_CASE(ExpandPackageSize,expand_package_size)
	PROTO_UNPACK_CASE(ExpandPackageSizeResult,expand_package_size_result)
	PROTO_UNPACK_CASE(GetPretaskItems,get_pretask_items)
	PROTO_UNPACK_CASE(GetPretaskItemsResult,get_pretask_items_result)
	PROTO_UNPACK_CASE(GetOnsaleItems,get_onsale_items)
	PROTO_UNPACK_CASE(GetOnsaleItemsResult,get_onsale_items_result)
	PROTO_UNPACK_CASE(BuyOnsaleItems, buy_onsale_items)
	PROTO_UNPACK_CASE(BuyOnsaleItemsResult, buy_onsale_items_result)
	PROTO_UNPACK_CASE(SendCustomServiceInfo, send_custom_service_info)
	PROTO_UNPACK_CASE(SendCustomServiceInfoResult, send_custom_service_info_result)
	PROTO_UNPACK_CASE(GetIABItem, get_iabitem)
	PROTO_UNPACK_CASE(GetIABItemResult, get_iabitem_result)
	PROTO_UNPACK_CASE(ChangeEmail, change_email)
	PROTO_UNPACK_CASE(ChangeEmailResult, change_email_result)
	PROTO_UNPACK_CASE(TokenLogin, token_login)
	PROTO_UNPACK_CASE(EngageInFightForTask, engage_in_fight_for_task)
	PROTO_UNPACK_CASE(EngageInFightForTaskResult, engage_in_fight_for_task_result)
	PROTO_UNPACK_CASE(AuthenticateAccount, authenticate_account)
	PROTO_UNPACK_CASE(AuthenticateAccountResult, authenticate_account_result)
	PROTO_UNPACK_CASE(Salvage, salvage)
	PROTO_UNPACK_CASE(SalvageResult, salvage_result)
	PROTO_UNPACK_CASE(GetEmailVerifiedReward, get_email_verified_reward)
	PROTO_UNPACK_CASE(GetEmailVerifiedRewardResult, get_email_verified_reward_result)
	PROTO_UNPACK_CASE(StartSalvage, start_salvage)
	PROTO_UNPACK_CASE(StartSalvageResult, start_salvage_result)
	PROTO_UNPACK_CASE(GetEmailVerifiedInfo, get_email_verified_info)
	PROTO_UNPACK_CASE(GetEmailVerifiedInfoResult,get_email_verified_info_result)
	PROTO_UNPACK_CASE(GetSalvageCondition, get_salvage_condition)
	PROTO_UNPACK_CASE(GetSalvageConditionResult,get_salvage_condition_result)
	PROTO_UNPACK_CASE(GetActivitiesProps, get_activities_props)
	PROTO_UNPACK_CASE(GetActivitiesPropsResult,get_activities_props_result)
	PROTO_UNPACK_CASE(GetActivitiesGiftInfo, get_activities_gift_info)
	PROTO_UNPACK_CASE(GetActivitiesGiftInfoResult,get_activities_gift_info_result)
	PROTO_UNPACK_CASE(GetActivitiesPrayInfo, get_activities_pray_info)
	PROTO_UNPACK_CASE(GetActivitiesPrayInfoResult,get_activities_pray_info_result)
	PROTO_UNPACK_CASE(GetPray, get_pray)
	PROTO_UNPACK_CASE(GetPrayResult,get_pray_result)
	PROTO_UNPACK_CASE(SendActivitiesGift, send_activities_gift)
	PROTO_UNPACK_CASE(SendActivitiesGiftResult,send_activities_gift_result)
	PROTO_UNPACK_CASE(GetPirateAttackInfo, get_pirate_attack_info)
	PROTO_UNPACK_CASE(GetPirateAttackInfoResult,get_pirate_attack_info_result)
	PROTO_UNPACK_CASE(GetAttackPirateInfo,get_attack_pirate_info)
	PROTO_UNPACK_CASE(GetAttackPirateInfoResult,get_attack_pirate_info_result)
	PROTO_UNPACK_CASE(GetVTicketMarketActivities,get_vticket_market_activities)
	PROTO_UNPACK_CASE(GetVTicketMarketActivitiesResult,get_vticket_market_activities_result)
	PROTO_UNPACK_CASE(BuyVTicketMarketActivities,buy_vticket_market_activities)
	PROTO_UNPACK_CASE(BuyVTicketMarketActivitiesResult,buy_vticket_market_activities_result)
	PROTO_UNPACK_CASE(FinalMyExploitScore,final_my_exploit_score)
	PROTO_UNPACK_CASE(FinalMyExploitScoreResult,final_my_exploit_score_result)
	PROTO_UNPACK_CASE(FindLootPlayer,find_loot_player)
	PROTO_UNPACK_CASE(FindLootPlayerResult,find_loot_player_result)
	PROTO_UNPACK_CASE(LootPlayer,loot_player)
	PROTO_UNPACK_CASE(LootPlayerResult,loot_player_result)
	PROTO_UNPACK_CASE(GetLootPlayerLog,get_loot_player_log)
	PROTO_UNPACK_CASE(GetLootPlayerLogResult,get_loot_player_log_result)
	PROTO_UNPACK_CASE(StartRevengeOnLootPlayer,start_revenge_on_loot_player)
	PROTO_UNPACK_CASE(GetAttackPirateRankInfo,get_attack_pirate_rank_info)
	PROTO_UNPACK_CASE(GetAttackPirateRankInfoResult,get_attack_pirate_rank_info_result)
	PROTO_UNPACK_CASE(GetAttackPirateBossInfo,get_attack_pirate_boss_info)
	PROTO_UNPACK_CASE(GetAttackPirateBossInfoResult,get_attack_pirate_boss_info_result)
	PROTO_UNPACK_CASE(BuyInsurance,buy_insurance)
	PROTO_UNPACK_CASE(BuyInsuranceResult,buy_insurance_result)
	PROTO_UNPACK_CASE(CompleteDialogs, complete_dialogs)
	PROTO_UNPACK_CASE(CompleteDialogsResult, complete_dialogs_result)
	PROTO_UNPACK_CASE(ObtainLottoMessage, obtain_lotto_message)
	PROTO_UNPACK_CASE(ObtainLottoMessageResult, obtain_lotto_message_result)
	PROTO_UNPACK_CASE(GetInviteBonus, get_invite_bonus)
	PROTO_UNPACK_CASE(GetInviteBonusResult, get_invite_bonus_result)
	PROTO_UNPACK_CASE(GetCompanies, get_companies)
	PROTO_UNPACK_CASE(GetCompaniesResult, get_companies_result)
	PROTO_UNPACK_CASE(EquipCompany, equip_company)
	PROTO_UNPACK_CASE(EquipCompanyResult, equip_company_result)
	PROTO_UNPACK_CASE(UnlockCompany, unlock_company)
	PROTO_UNPACK_CASE(UnlockCompanyResult, unlock_company_result)
	PROTO_UNPACK_CASE(GetCompanionsStatus, get_companions_status)
	PROTO_UNPACK_CASE(GetCompanionsStatusResult, get_companions_status_result)
	PROTO_UNPACK_CASE(UnlockShipCabin, unlock_ship_cabin)
	PROTO_UNPACK_CASE(UnlockShipCabinResult, unlock_ship_cabin_result)
	PROTO_UNPACK_CASE(GetShipCompanions, get_ship_companions)
	PROTO_UNPACK_CASE(GetShipCompanionsResult, get_ship_companions_result)
	PROTO_UNPACK_CASE(SetShipCompanions, set_ship_companions)
	PROTO_UNPACK_CASE(SetShipCompanionsResult, set_ship_companions_result)
	PROTO_UNPACK_CASE(SaveFailedFight, save_failed_fight)
	PROTO_UNPACK_CASE(SaveFailedFightResult, save_failed_fight_result)
	PROTO_UNPACK_CASE(EndFailedFightByVTicket, end_failed_fight_by_vticket)
	PROTO_UNPACK_CASE(EndFailedFightByVTicketResult, end_failed_fight_by_vticket_result)
	PROTO_UNPACK_CASE(CompleteCompanionTask, complete_companion_task)
	PROTO_UNPACK_CASE(CompleteCompanionTaskResult, complete_companion_task_result)
	PROTO_UNPACK_CASE(DeductCoins, deduct_coins)
	PROTO_UNPACK_CASE(DeductCoinsResult, deduct_coins_result)
	PROTO_UNPACK_CASE(UpdateDataVersion, update_data_version)
	PROTO_UNPACK_CASE(UpdateDataVersionResult, update_data_version_result)
	PROTO_UNPACK_CASE(CalFastHireCrew, cal_fast_hire_crew)
	PROTO_UNPACK_CASE(CalFastHireCrewResult, cal_fast_hire_crew_result)
	PROTO_UNPACK_CASE(CompanionRebirth, companion_rebirth)
	PROTO_UNPACK_CASE(CompanionRebirthResult, companion_rebirth_result)
	PROTO_UNPACK_CASE(RepairEquipment, repair_equipment)
	PROTO_UNPACK_CASE(RepairEquipmentResult, repair_equipment_result)
	PROTO_UNPACK_CASE(GetRepairEquipmentNeed, get_repair_equipment_need)
	PROTO_UNPACK_CASE(GetRepairEquipmentNeedResult, get_repair_equipment_need_result)
	PROTO_UNPACK_CASE(GetRecentOrder, get_recent_order)
	PROTO_UNPACK_CASE(GetRecentOrderResult, get_recent_order_result)
	PROTO_UNPACK_CASE(GetDropIncreaseRateInfo, get_drop_increase_rate_info)
	PROTO_UNPACK_CASE(GetDropIncreaseRateInfoResult, get_drop_increase_rate_info_result)
	PROTO_UNPACK_CASE(GetDailyActivitiesReward, get_daily_activities_reward)
	PROTO_UNPACK_CASE(GetDailyActivitiesRewardResult, get_daily_activities_reward_result)
	PROTO_UNPACK_CASE(AddProficiency,add_proficiency)
	PROTO_UNPACK_CASE(AddProficiencyResult, add_proficiency_result)
	PROTO_UNPACK_CASE(GetProficiencyValues, get_proficiency_values)
	PROTO_UNPACK_CASE(GetProficiencyValuesResult, get_proficiency_values_result)
//	PROTO_UNPACK_CASE(PrepareCityDefense, prepare_city_defense)
//	PROTO_UNPACK_CASE(PrepareCityDefenseResult, prepare_city_defense_result)
//	PROTO_UNPACK_CASE(PrepareCityAttack, prepare_city_attack)
//	PROTO_UNPACK_CASE(PrepareCityAttackResult, prepare_city_attack_result)
	PROTO_UNPACK_CASE(GetCityPrepareStatus, get_city_prepare_status)
	PROTO_UNPACK_CASE(GetCityPrepareStatusResult, get_city_prepare_status_result)
//	PROTO_UNPACK_CASE(StartStateWar, start_state_war)
//	PROTO_UNPACK_CASE(StartStateWarResult, start_state_war_result)
	PROTO_UNPACK_CASE(GetEnemyNation, get_enemy_nation)
	PROTO_UNPACK_CASE(GetEnemyNationResult, get_enemy_nation_result)
	PROTO_UNPACK_CASE(AddCityRepairPool, add_city_repair_pool)
	PROTO_UNPACK_CASE(AddCityRepairPoolResult, add_city_repair_pool_result)
	PROTO_UNPACK_CASE(AddCityAttackPool, add_city_attack_pool)
	PROTO_UNPACK_CASE(AddCityAttackPoolResult, add_city_attack_pool_result)
	PROTO_UNPACK_CASE(GetPackageInfo, get_package_info)
	PROTO_UNPACK_CASE(GetPackageInfoResult, get_package_info_result)
	PROTO_UNPACK_CASE(GetProficiencyBook, get_proficiency_book)
	PROTO_UNPACK_CASE(GetProficiencyBookResult, get_proficiency_book_result)
	PROTO_UNPACK_CASE(GetNationWarEntrance, get_nation_war_entrance)
	PROTO_UNPACK_CASE(GetNationWarEntranceResult, get_nation_war_entrance_result)
	PROTO_UNPACK_CASE(GetContributionRank, get_contribution_rank)
	PROTO_UNPACK_CASE(GetContributionRankResult, get_contribution_rank_result)
	PROTO_UNPACK_CASE(GetNationWarCityLost, get_nation_war_city_lost)
	PROTO_UNPACK_CASE(GetNationWarCityLostResult, get_nation_war_city_lost_result)
	PROTO_UNPACK_CASE(ApplyStateWar,apply_state_war)
	PROTO_UNPACK_CASE(ApplyStateWarResult, apply_state_war_result)
	PROTO_UNPACK_CASE(StartAttackCity, start_attack_city)
	PROTO_UNPACK_CASE(StartAttackCityResult, start_attack_city_result)
	PROTO_UNPACK_CASE(HireArmy, hire_army)
	PROTO_UNPACK_CASE(HireArmyResult, hire_army_result)
	PROTO_UNPACK_CASE(AddSupplyStation, add_supply_station)
	PROTO_UNPACK_CASE(AddSupplyStationResult, add_supply_station_result)
	PROTO_UNPACK_CASE(EndAttackCity, end_attack_city)
	PROTO_UNPACK_CASE(EndAttackCityResult, end_attack_city_result)
	PROTO_UNPACK_CASE(NationWarBattleTurnResult, nation_war_battle_turn_result)
	PROTO_UNPACK_CASE(RepairCityDefenseInWar, repair_city_defense_in_war)
	PROTO_UNPACK_CASE(RepairCityDefenseInWarResult, repair_city_defense_in_war_result)
	PROTO_UNPACK_CASE(EnhanceAttack, enhance_attack)
	PROTO_UNPACK_CASE(EnhanceAttackResult, enhance_attack_result)
	PROTO_UNPACK_CASE(ReachDepot, reach_depot)
	PROTO_UNPACK_CASE(ReachDepotResult, reach_depot_result)
	PROTO_UNPACK_CASE(ReduceInNationWar, reduce_in_nation_war)
	PROTO_UNPACK_CASE(ReduceInNationWarResult, reduce_in_nation_war_result)
	PROTO_UNPACK_CASE(GetMyNationWar, get_my_nation_war)
	PROTO_UNPACK_CASE(GetMyNationWarResult, get_my_nation_war_result)
	PROTO_UNPACK_CASE(FuncForTest, func_for_test)
	PROTO_UNPACK_CASE(FuncForTestResult, func_for_test_result)
	PROTO_UNPACK_CASE(CheckDeleteCharactersNum, check_delete_characters_num)
	PROTO_UNPACK_CASE(CheckDeleteCharactersNumResult, check_delete_characters_num_result)
	PROTO_UNPACK_CASE(GetPlayerShipList,get_player_ship_list)
	PROTO_UNPACK_CASE(GetPlayerShipListResult, get_player_ship_list_result)
	PROTO_UNPACK_CASE(GetPlayerEquipShipInfo, get_player_equip_ship_info)
	PROTO_UNPACK_CASE(GetPlayerEquipShipInfoResult, get_player_equip_ship_info_result)
	PROTO_UNPACK_CASE(FireCrew, fire_crew)
	PROTO_UNPACK_CASE(FireCrewResult, fire_crew_result)
	PROTO_UNPACK_CASE(GetForceCity, get_force_city)
	PROTO_UNPACK_CASE(GetForceCityResult, get_force_city_result)
	PROTO_UNPACK_CASE(GetChangeLog, get_change_log)
	PROTO_UNPACK_CASE(GetChangeLogResult, get_change_log_result)
        default:
            break;
	}
    return 0;
}

std::string EOFMessage = "\r\n";

void ProtocolThread::connectToChatServer(){
	TVSocketHolder::getInstance()->StartChatThread();
}

bool ProtocolThread::isConnectingChatServer(){
	return TVSocketHolder::getInstance()->m_isConnecting;
}

bool ProtocolThread::isChatServerConnected(){
	return TVSocketHolder::getInstance()->m_chatServerConnected;
}

static unsigned int SDBMHash(const char *str)
{
    unsigned int hash = 0;

    while (*str)
    {
        // equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }

    return (hash & 0x7FFFFFFF);
}

void ProtocolThread::setServerUrl(std::string& url,int port, std::string chatUrl,int chatPort)
{
	m_serverUrl = url;
	m_serverUrlHash = SDBMHash(url.c_str());
	m_serverPort = port;

	m_ChatIpUrl = url;
	m_chatPort = chatPort;

	TVSocketHolder::getInstance()->setCustomIp(url.c_str(),port,chatUrl.c_str(),chatPort);
}

std::string ProtocolThread::getFullKeyName(const char*key)
{
	if(!key)
		return std::string("");
	std::string prefix = getUserDefaultPrefix();
	if (prefix.length() > 0) {
		std::string cKey = key;
		//cKey += "_";
		std::string name = prefix + cKey;
		return name;
	}
	return std::string(key);
}

std::string ProtocolThread::getUserDefaultPrefix(){
	std::string res = "";
	if(m_serverUrlHash != 0 && mUserSession && mUserSession->getUserId() > 0){
		char buffer[128];
		//unsigned int emailHash = SDBMHash(m_loginEmail.c_str());
		snprintf(buffer,sizeof(buffer),"v%u_%d_%d_",m_serverUrlHash,mUserSession->getUserId(),mUserSession->getCharacterId());
		res = buffer;
		return res;
	}else{
		return res;
	}
}

void ProtocolThread::sendChatMessage(const char*p){
	if(p && p[0] && chatServerOK){
		std::string msg = p;
		msg = msg + EOFMessage;
		log("send chat:%s\n",msg.c_str());
		ODSocket cSocket=TVSocketHolder::getInstance()->getChatSocket();
		int count=cSocket.Send(msg.c_str(),msg.size(),0);
	}
}
//
void ProtocolThread::loginChatServer(){
#if 0
	std::string nick = "NICK ";
	std::string name = heroName;
	nick += name;
	sendChatMessage(nick.c_str());
	std::string user = "USER ";
	std::string info = ":Piigame Voyage";
	user = user + nick + info;
	sendChatMessage(user.c_str());
	nick = "JOIN #voyage";
	sendChatMessage(nick.c_str());
#else
	char loginBuffer[512];
	snprintf(loginBuffer,sizeof(loginBuffer),"%s %s %d",m_chatServerEmail.c_str(),m_chatServerToken.c_str(),Utils::getLanguage());
	log("log chat server %s",loginBuffer);
	chatServerOK = true;
	sendChatMessage(loginBuffer);
	chatServerOK = false;
#endif
}

static const char* chatEOF = "\r\n";
void ProtocolThread::processChatMessage(){
	loginChatServer();
	ODSocket& csocket=TVSocketHolder::getInstance()->getChatSocket();
	Buffer* buf = new Buffer;
	while(true){
		int selectResult = csocket.Select();
		if(selectResult == 1){
			buf->ensureWritableBytes(102400);
			int size = csocket.Recv(reinterpret_cast<char*>(buf->beginWrite()),buf->writableBytes(),0);
			//cocos2d::log("recved size:%d",size);
			if(size <= 0){
				//server shut down connection
				csocket.Close(__FILE__,__LINE__);
				//SocketThread::GetInstance()->setConnectionStatus(false);
				break;
			}
			buf->hasWritten(size);
			if(chatServerOK){
				while(buf->readableBytes() >= 1){
					char*chatMsg = new char[buf->readableBytes() + 3];
					memset(chatMsg,0,buf->readableBytes() + 3);
					memcpy(chatMsg,buf->peek(),buf->readableBytes());
					std::string chatStr = chatMsg;
					//int stringLen = strlen(chatMsg);
					auto pos = chatStr.find(chatEOF,0);
					chatMsg[pos] = 0;
					if(pos != std::string::npos){
						auto scheduler = Director::getInstance()->getScheduler();
						scheduler->performFunctionInCocosThread(CC_CALLBACK_0(ProtocolThread::dispatchChatMessageCallbacks, this,chatMsg));

						buf->retrieve(pos+2);
					}
					else{
						delete[] chatMsg;
						break;
					}
				}
			}else{
				if(buf->readableBytes() >= 2){
					char OK[3];
					memcpy(OK,buf->peek(),2);
					OK[2] = 0;
					if(OK[0] == 'O' && OK[1] == 'K'){
						chatServerOK = true;
						buf->retrieve(2);
						if(seaCord.x > 0 && seaCord.y > 0){
							joinSeaChannel(getSeaAreaIndex(seaCord));
						}else{
							if(cityIdx > 0){
								joinCityChannel(cityIdx);
							}
						}
						if(guildId > 0){
							joinGuildChannel(guildId);
						}
					}else{
						csocket.Close(__FILE__,__LINE__);
						break;
					}
				}
			}
		}
	}	
}

void ProtocolThread::joinGuildChannel(int guildid){
	std::string join = "JOIN #";
	char cityStr[32];
	sprintf(cityStr,"GUILD_%d",guildid);
	join += cityStr;
	sendChatMessage(join.c_str());
}

void ProtocolThread::joinCityChannel(int cityId){
	std::string join = "JOIN #";
	char cityStr[32];
	sprintf(cityStr,"CITY_%d",cityId);
	join += cityStr;
	sendChatMessage(join.c_str());
}
int ProtocolThread::getSeaIndex(int seaIndex)
{
	m_seaIndex = seaIndex;
	return m_seaIndex;
}
int ProtocolThread::getSeaAreaIndex(cocos2d::Point cord){
	return m_seaIndex;
}

void ProtocolThread::joinSeaChannel(int seaIdx){
	if(seaAreaIdx == seaIdx)
		return;
	seaAreaIdx = seaIdx;
	std::string join = "JOIN #";
	char seaStr[64];
	sprintf(seaStr,"SEA_%d",seaIdx);
	join += seaStr; //fix me,need sea area data;
	sendChatMessage(join.c_str());

}

void ProtocolThread::listChannelUsers(int channelType)//1,world,2,country,3 city or sea area,4 guild
{
	std::string channel = "LIST ";
	if(channelType == 0){
		sendChatMessage("LIST #voyage_zh");
	}else if (channelType == 1){
		sendChatMessage("LIST #voyage");
	}else if (channelType == 2){
		if(nationIdx >= 1 && nationIdx < 8){
			const char* country[] = {"#Portugal ","#Spain ","#England ","#Netherlands ","#Genoa ","#Venice ","#Sweden "};
			//std::string channel = "PRIVMSG ";
			channel += country[nationIdx-1];
			sendChatMessage(channel.c_str());
		}
	}else if(channelType == 3){ //zone ,sea,city 
		if(seaAreaIdx > 0){
			//std::string channel = "PRIVMSG #";
			char seaStr[64];
			sprintf(seaStr,"#SEA_%d ",seaAreaIdx);
			channel += seaStr;
			sendChatMessage(channel.c_str());
		}else if(cityIdx > 0){ 
			char cityStr[32];
			sprintf(cityStr,"#CITY_%d ",cityIdx);
			channel += cityStr;
			sendChatMessage(channel.c_str());
		} 
	}else if(channelType == 4){
		//guild
		if(guildId > 0){
			char cityStr[32];
			sprintf(cityStr,"#GUILD_%d ",guildId);
			channel += cityStr;
			sendChatMessage(channel.c_str());
		}
	}
}

//1,world,2,country,3 city or sea area,4 guild
bool ProtocolThread::sendChat(int channelType,const char*msg){
	struct timeval now;
	gettimeofday(&now, nullptr);
	if(m_lastSendChatTime +1 > now.tv_sec){
		return false;
	}
	m_lastSendChatTime = now.tv_sec;

	if (channelType == 0){
		std::string channel = "PRIVMSG #voyage_zh ";
		std::string Msg = msg;
		Msg = channel + Msg;
		sendChatMessage(Msg.c_str());
	}else if(channelType == 1){
		std::string channel = "PRIVMSG #voyage ";
		std::string Msg = msg;
		Msg = channel + Msg;
		sendChatMessage(Msg.c_str());
	}else if(channelType == 2){
		if(nationIdx >= 1 && nationIdx < 8){
			const char* country[] = {"#Portugal ","#Spain ","#England ","#Netherlands ","#Genoa ","#Venice ","#Sweden "};
			std::string channel = "PRIVMSG ";
			channel += country[nationIdx-1];
			std::string Msg = msg;
			Msg = channel + Msg;
			sendChatMessage(Msg.c_str());
		}
	}else if(channelType == 3){ //zone ,sea,city 
		std::string Msg = msg;
		if(seaAreaIdx > 0){
			std::string channel = "PRIVMSG #";
			char seaStr[64];
			sprintf(seaStr,"SEA_%d ",seaAreaIdx);
			channel += seaStr;
			Msg = channel + Msg;
			sendChatMessage(Msg.c_str());
		}else if(cityIdx > 0){ 
			std::string channel = "PRIVMSG #";
			char cityStr[32];
			sprintf(cityStr,"CITY_%d ",cityIdx);
			channel += cityStr;
			std::string Msg = msg;
			Msg = channel + Msg;
			sendChatMessage(Msg.c_str());
		} 
	}else if(channelType == 4){
		//guild
		if(guildId > 0){
			std::string Msg = msg;
			std::string channel = "PRIVMSG #";
			char cityStr[32];
			sprintf(cityStr,"GUILD_%d ",guildId);
			channel += cityStr;
			Msg = channel + Msg;
			sendChatMessage(Msg.c_str());
		}
	}
	return true;
}

void ProtocolThread::getPrivateUserInfo(char*name){
	if(name && name[0]){
		std::string channel = "LIST ";
		channel += name;
		sendChatMessage(channel.c_str());
	}
}

bool ProtocolThread::sendPrivateChat(const char* toName,const char*msg){
	struct timeval now;
	gettimeofday(&now, nullptr);
	if(m_lastSendChatTime +1 > now.tv_sec){
		return false;
	}
	m_lastSendChatTime = now.tv_sec;

	std::string channel = "PRIVMSG ";
	channel += toName;
	channel += " ";
	channel += msg;
	sendChatMessage(channel.c_str());
	//PRIVMSG
	return true;
}

void ProtocolThread::ProcessSocketFunc()
{
	ODSocket csocket=TVSocketHolder::getInstance()->getSocket();
	if(TVSocketHolder::getInstance()->isConnected())
	{
		//sendQueuedBuffer();
	
		if (m_vBuffer == NULL)
		{
			m_vBuffer = new Buffer();
		}

		do{			
			//if(true/*csocket.Select()==-2*/){
			int selectResult = csocket.Select();
			if(selectResult == 0){
				if (waitingReponseSeconds >= 0){
					waitingReponseSeconds++;
				}
			}

			if(selfDisconnect){
				
				break;
			}
			if(selectResult == 1){
				idleSeconds = 0;
				m_vBuffer->ensureWritableBytes(204800);
				unsigned char*decrypBuffer = reinterpret_cast<unsigned char*>(m_vBuffer->beginWrite());
				int curReadSize = csocket.Recv(reinterpret_cast<char*>(m_vBuffer->beginWrite()),m_vBuffer->writableBytes(),0);
				cocos2d::log("recved size:%d", curReadSize);
				if (curReadSize == 0){
					//server shut donw connection
					csocket.Close(__FILE__,__LINE__);
					TVSocketHolder::getInstance()->setConnectionStatus(false);
					break;
				}else if(curReadSize < 0){
					//socket error
					csocket.Close(__FILE__,__LINE__);
					TVSocketHolder::getInstance()->setConnectionStatus(false);
					break;
				}
				m_vBuffer->hasWritten(curReadSize);

				if(m_vBuffer->readableBytes() >= kMinMessageLen)
				{
                    const char*data = m_vBuffer->peek();
					int dataLen =  -1;
					unsigned int msg_type = 0;
					unsigned int readableLen = m_vBuffer->readableBytes();
					bool rc = ProtocolDefine::ReadPacket((unsigned char*)data, decrypBuffer, readableLen, curReadSize, &dataLen, &msg_type, m_rc4KeyDec != NULL, m_rc4KeyDec);
					cocos2d::log("read packet rc:%d datalen：%d %d", rc, dataLen, msg_type);

					if (rc && dataLen > 0)
					{
						const uint8_t *dataBuf = (const uint8_t*)(data + PACKET_TAG_AND_SIZE_LEN + PACKET_SID_LEN);
						ProtobufCMessage* protoMsg = protobuf_internal_get_message(msg_type, dataBuf, dataLen);
                        
                        if (protoMsg)
						{
							waitingReponseSeconds = -1;
							BaseResponseMsg* basmsg=new BaseResponseMsg();
							basmsg->message = protoMsg;
							basmsg->msgType = msg_type;
							//scheduler->performFunctionInCocosThread(CC_CALLBACK_0(ProtocolThread::dispatchResponseCallbacks, this,basmsg)); 
							TVSocketHolder::getInstance()->AppendBackendRspMsg(basmsg);

							unsigned int packetLen = dataLen + PACKET_TAG_AND_SIZE_LEN + PACKET_CHECK_SUM_LEN + PACKET_SID_LEN;
							m_vBuffer->retrieve(packetLen);
						}
						else
						{
							assert(0);
							csocket.Close(__FILE__,__LINE__);
							TVSocketHolder::getInstance()->setConnectionStatus(false);
							break;
						}
                    }

				}
			}
			else if (waitingReponseSeconds >= CLIENT_TIME_OUT){
#if !WIN32  && !LINUX
				csocket.Close(__FILE__,__LINE__);
				TVSocketHolder::getInstance()->setConnectionStatus(false);
				break;
#endif
			}else if(selectResult == 0){
				if(m_autoPing){
					struct timeval now;
					gettimeofday(&now, nullptr);
					if(now.tv_sec >= lastPingTime.tv_sec + 20){
						pingServer();
					}
				}
				sendQueuedBuffer();
				//cocos2d::log("idle time :%d s",++idleSeconds);
#if 0
				std::stringstream ss;
				ss << std::this_thread::get_id();
				//uint64_t id = std::stoull(ss.str());
				log("new thread id:%s\n",ss.str().c_str());
#endif
// 				struct timeval now;
// 				gettimeofday(&now, nullptr);
// 				if(now.tv_sec >= lastCheckMailTime.tv_sec + 60){
// 					checkMailBox();
// 				}
			}else if(selectResult == -1){
				//network error;
				csocket.Close(__FILE__,__LINE__);
				TVSocketHolder::getInstance()->setConnectionStatus(false);
				break;
			}
	 }while (0);
	}
	return;
}

void ProtocolThread::clearQueueMsg(){
	while(queueMsgBuffer.size() > 0){
		Buffer* buffer = queueMsgBuffer.back();
		if(buffer){
			//const char*data = buffer->peek();
			//int size = buffer->readableBytes();

			//sendMessage(data,size);
			delete buffer;
		}
		queueMsgBuffer.pop_back();
	}
	bufferToSend.clear();
}

void ProtocolThread::sendQueueMsg(){
	//log("sendQueueMsg:%d:%d",queueMsgBuffer.size() ,isWaitingResponse);
	if(queueMsgBuffer.size() > 0 && !isWaitingResponse){
		Buffer* buffer = queueMsgBuffer.front();
		if(buffer){
			const char*data = buffer->peek();
			int size = buffer->readableBytes();
			//queueMsgBuffer.erase(queueMsgBuffer.begin());
			queueMsgBuffer.pop_front();
			mLoadingLayer = buffer->mLoadingLayer;
			log("queueMsgBuffer size:%d,new loading:%x",queueMsgBuffer.size(),mLoadingLayer);
			if(mLoadingLayer){
				log("mLoadingLayer = buffer->mLoadingLayer;");
			}
			sendMessage(data,size);
			delete buffer;
		}	
	}
}

void ProtocolThread::sendQueuedBuffer(){
	if(bufferToSend.empty() || !TVSocketHolder::getInstance()->isConnected())
		return;
	ODSocket cSocket=TVSocketHolder::getInstance()->getSocket();
	msgMutex.lock();
	auto temp = bufferToSend;
	bufferToSend.clear();
	msgMutex.unlock();

	bool sendFailed = false;
	for( const auto &buffer : temp ) {
		if(sendFailed){
			addBufferToSendingQueue(buffer);
		}else{
			const char*data = buffer->peek();
			int size = buffer->readableBytes();
			int count=cSocket.Send(data,size,0);
			if(count > 0){
				idleSeconds = 0;
			}
			if(count < 0){
				cSocket.Close(__FILE__,__LINE__);
				TVSocketHolder::getInstance()->setConnectionStatus(false);
				break;
			}
			if(count < size){
				sendFailed = true;
				buffer->retrieve(count);
				addBufferToSendingQueue(buffer);
			}else{
				delete buffer;
			}
		}
	}	
}

void ProtocolThread::addBufferToSendingQueue( Buffer*buffer){
	msgMutex.lock();
	bufferToSend.push_back(buffer);
	msgMutex.unlock();
}

void ProtocolThread::sleep(int secstr){
	timeval timeout = { secstr/1000, secstr%1000}; 
	select(0, NULL, NULL, NULL, &timeout);
}

void ProtocolThread::registerChatMessageCallback(const chatMsgCallBack &callback){
	chatMsgFunc = callback;
}

void ProtocolThread::registerGoogleLoginCallback(const googleLoginCallBack &callback){
	if(callback){
		m_googleLoginCallback = callback;
	}else{
		m_googleLoginCallback = nullptr;
	}
}

void ProtocolThread::unregisterChatMessageCallback(){
	chatMsgFunc = nullptr;
}

void ProtocolThread::registerMessageCallback(const ResponseCallBack &callback,void*_this)
{
	//callbackQueueMutex.lock();
	unregisterMessageCallback(_this);
	auto pair =  std::make_pair(_this,callback);
	messageCallback.push_back(pair);
	//callbackQueueMutex.unlock();
}

void ProtocolThread::unregisterMessageCallback(void*_this)
{
// 	if(messageCallback.empty())
// 		return;
	//callbackQueueMutex.lock();
	std::vector<ResponseCallBackPair>::iterator it = messageCallback.end();
	for (auto i = messageCallback.begin(); i != messageCallback.end(); i++)
	{
		void*tmp = i->first;
		if (tmp == _this)
		{
			it = i;
		}
	}
	auto tmp = messageCallback.end();
	if (it != tmp)
	{
		messageCallback.erase(it);
	}
	//callbackQueueMutex.unlock();
}

void ProtocolThread::dispatchChatMessageCallbacks(const char*charMsg){
	cocos2d::log("char server-%s",charMsg);
	if(chatMsgFunc){
		chatMsgFunc((char *)charMsg);
	}
}
/*
 * 这是什么函数？能给点说明吗？注释呢？
 */
void ProtocolThread::dispatchResponseCallbacks(BaseResponseMsg*baseMsg)
{
	if(baseMsg->msgType > 0){
		struct timeval now;
		gettimeofday(&now, nullptr);
		float sec = now.tv_sec - lastSendMessageTime.tv_sec + (now.tv_usec - lastSendMessageTime.tv_usec)/1000000.0;
		log("---------------------UI thread get message type:%d takes:%f s \n",baseMsg->msgType,sec);
	}else if(baseMsg->msgType == -1){
		connectionLost = true;
	}
	MainTaskManager::GetInstance()->onServerEvent(baseMsg->message,baseMsg->msgType);
	if (baseMsg->msgType != PROTO_TYPE_NationWarBattleTurnResult && baseMsg->msgType != PROTO_TYPE_SysMessage)
	{
		isWaitingResponse = false;
	}

	switch(baseMsg->msgType){
	case PROTO_TYPE_LootPlayerResult:{
		LootPlayerResult*result = (LootPlayerResult*)baseMsg->message;
		if(result->failed == 0){

		}
		log("PROTO_TYPE_LootPlayerResult:%d",result->failed);
	}
		break;
	case PROTO_TYPE_ChangeEmailResult:{
		ChangeEmailResult*result = (ChangeEmailResult*)baseMsg->message;
		if(result->failed == 0){
			saveChangedEmail(result->newemail);
		}
	}
		break;
	case PROTO_TYPE_DeleteUserResult:{
		DeleteUserResult*result = (DeleteUserResult*)baseMsg->message;
		if(result->failed == 0 && result->deleteaccount == 1 ){
			//result->
			saveEmail("");
			savePassword("");
		}
	}
		break;
	case PROTO_TYPE_ChangePasswordResult:{
		ChangePasswordResult*result = (ChangePasswordResult*)baseMsg->message;
		if(result->failed == 0){
			savePassword(result->password);
		}
	}
		break;
	case PROTO_TYPE_CheckMailBoxResult:{
		CheckMailBoxResult*result = (CheckMailBoxResult*)baseMsg->message;
		if(m_lastCheckMailBoxResult){
			delete (m_lastCheckMailBoxResult);
		}
		m_lastCheckMailBoxResult = new CheckMailBoxResult;
		memcpy(m_lastCheckMailBoxResult,result,sizeof(CheckMailBoxResult));
	}
		break;
	case PROTO_TYPE_PingServerResult:{
		PingServerResult*result = (PingServerResult*)baseMsg->message;
	}
		break;
//	case PROTO_TYPE_GetServerInfoResult:{
//		GetServerInfoResult*result = (GetServerInfoResult*)baseMsg->message;
//		if(result->clientversion < result->serverversion){
//			GUITip* tip = GUITip::createTip("UPDATE",2);
//
//			auto scene = Director::getInstance()->getRunningScene();
//			scene->addChild(tip,10000);
//				//tip->setNegativeCallback(CC_CALLBACK_0(BasicLayer::exitGame,this));
//		}
//	}
//		break;
	case PROTO_TYPE_GetNextMapBlockResult:
		{
			GetNextMapBlockResult*result = (GetNextMapBlockResult*)baseMsg->message;
			if(result->failed == 0){
				seaCord.x = result->blockinfo->x;
				seaCord.y = result->blockinfo->y;
				int idx = getSeaAreaIndex(seaCord);
				if(idx != seaAreaIdx){
					joinSeaChannel(idx);
				}
			}
		}
		break;
	case PROTO_TYPE_GetSailInfoResult:
	{
		LeaveCityResult*result = (LeaveCityResult*)baseMsg->message;
		if(result->failed == 0){
			seaCord.x = result->shipx;
			seaCord.y = result->shipy;
			joinSeaChannel(getSeaAreaIndex(seaCord));
		}
	}
		break;
	case PROTO_TYPE_LeaveCityResult:
		{
			LeaveCityResult*result = (LeaveCityResult*)baseMsg->message;
			if(result->failed == 0){
				seaCord.x = result->shipx;
				seaCord.y = result->shipy;
				joinSeaChannel(getSeaAreaIndex(seaCord));
				m_autoPing = 0;
			}
		}
		break;
	case PROTO_TYPE_GetCurrentCityDataResult:
		{
			GetCurrentCityDataResult*result = (GetCurrentCityDataResult*)baseMsg->message;
			if(result->failed == 0){
				if(result->data){
					SINGLE_HERO->initHeroInfo(result->data);
					if(heroName)  delete[] 	heroName;
					heroName = copyCString(result->data->heroname);
					cityIdx = result->data->lastcity->cityid;
					coins = result->data->coin;
					gold = result->data->gold;
					nationIdx = result->data->nation;
					guildId = result->data->guildid;
					icon = result->data->iconidx;

				}
				popularGoodsIds.clear();

				for(int i=0;i<result->n_populargoodsids;i++){
					popularGoodsIds.push_back(result->populargoodsids[i]);
				}

//				cityIdx = result->data->lastcity->cityid;
//				coins = result->data->coin;
//				gold = result->data->gold;
#ifdef ANDROID
//				TDCCAccount* account = Utils::getTDCAccount();
//		       	account->setLevel(SINGLE_HERO->m_iLevel);
#endif
				//icon = result->data->iconidx;
				seaCord.x = -1;
				seaCord.y = -1;
				seaAreaIdx = -1;
				if(chatServerOK){
					joinCityChannel(cityIdx);
				}
				m_autoPing = 0;
			}
		}
		break;
	case PROTO_TYPE_SetMainTaskStatusResult:
	{
		SetMainTaskStatusResult*result = (SetMainTaskStatusResult*)baseMsg->message;
		if(result->failed == 0){
			m_mainTaskStatus = result->status;
			if(result->status == 10){
				m_isInTutorial = false;
			}
		}
	}
	break;
	case PROTO_TYPE_EmailLoginResult: //这里是hook吗？注释呢？
		{
			EmailLoginResult*result = (EmailLoginResult*)baseMsg->message;
			if(result && result->userid != -1){
				mLastLoginResult = result;
				SINGLE_HERO->initHeroInfo(result);
				SINGLE_SHOP->loadGuideTaskDialogData();
				SINGLE_SHOP->loadTaskDialogData();
				SINGLE_SHOP->updateRemoteJson(result->refreshjson,result->n_refreshjson);
				if(heroName)  delete[] 	heroName;
				heroName = copyCString(result->heroname);
				cityIdx = result->lastcityid;
				coins = result->coin;
				gold = result->gold;
				nationIdx = result->nation;
				guildId = result->guildid;
				icon = result->iconidx;

				seaCord.x = result->shipx;
				seaCord.y = result->shipy;
				m_mainTaskStatus = result->maintaskstatus;
				if(result->maintaskstatus < 10){
					m_isInTutorial = true;
				}
				if(mUserSession){
					delete mUserSession;
				}
				m_chatServerToken = result->chatservertoken;
				if(result->email){
					m_chatServerEmail = result->email;
				}
				mUserSession = new UserSession(result->userid,result->sessionid);
				mUserSession->setCharacterId(result->cid);
				if(result->fbinvitecode){
					m_inviteCode = result->fbinvitecode;
				}

#ifdef ANDROID
				std::ostringstream stmCid ;
				stmCid << result->cid ;
				//统计追踪
				std::string accountId = ServerManager::CurSelectedServer()->GetServerName() + "_" +stmCid.str();
//				Utils::setTDCAccount(accountId);
//	        	TDCCAccount* account = Utils::getTDCAccount();
	        	//需要区分是否是google
	        	if(result->logtype == 0){
	        		//邮件登陆
//	        		account->setAccountType(TDCCAccount::TDCCAccountType::kAccountType1);
	        	}else{
	        		//谷歌登陆
//	        		account->setAccountType(TDCCAccount::TDCCAccountType::kAccountType2);
	        	}
//	        	account->setAccountName(result->heroname);
//	        	account->setGameServer(ServerConfig::GetInstance()->GetServerName().c_str());
//	        	account->setLevel(EXP_NUM_TO_LEVEL(result->exp));
	        	if(result->gender != 1){
//	        		account->setGender(TDCCAccount::kGenderMale);
	        	}else{
//	        		account->setGender(TDCCAccount::kGenderFemale);
	        	}
#endif
				if(result->logtype == 0){
					if(!m_rc4KeyDec || !m_rc4KeyEnc){
						m_rc4KeyDec = InitRC4((unsigned char*)pendingPassword,strlen(pendingPassword));
						m_rc4KeyEnc = InitRC4((unsigned char*)pendingPassword,strlen(pendingPassword));
					}

					if(pendingEmail){
						m_loginEmail = pendingEmail;
						saveEmail(pendingEmail);
						delete[] pendingEmail;
						pendingEmail = 0;
					}
					if(pendingPassword){
						savePassword(pendingPassword);
						delete[] pendingPassword;
						pendingPassword = 0;
					}
					if(result->email && strlen(result->email) > 1){
						saveEmail(result->email);
					}
					cocos2d::CCUserDefault::getInstance()->setStringForKey(USER_DEFAULT_USED_EMAIL,"1");
				}else{//google account login
					if(!m_rc4KeyDec || !m_rc4KeyEnc){
						m_rc4KeyDec = InitRC4((unsigned char*)pendingPassword,strlen(pendingPassword));
						m_rc4KeyEnc = InitRC4((unsigned char*)pendingPassword,strlen(pendingPassword));
					}
					cocos2d::CCUserDefault::getInstance()->setStringForKey(USER_DEFAULT_USED_EMAIL,"0");
					saveEmail("");
					savePassword("");
				}
//				if(result->password){
//					log("save password");
//					savePassword(result->password);
//				}
				connectToChatServer();
			}else{
				if(result){
					log("login failed code:%d",result->failed);
				}
				if(pendingEmail){
					delete[] pendingEmail;
					pendingEmail = 0;
				}

				if(pendingPassword){
					delete[] pendingPassword;
					pendingPassword = 0;
				}
				if(result->failed == 4){ //wrong password
					//StartGoogleLogin
#if 0
					JniMethodInfo jniInfo;
					bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","StartGoogleLogin","()V");
					log("OpenUrl id:%x,%x,%d",jniInfo.methodID,jniInfo.classID,id);
					if(id)
					{
						jniInfo.env->CallStaticVoidMethod(jniInfo.classID, jniInfo.methodID);
					}
#endif
				}
			}
		}
		break;
	case PROTO_TYPE_RegisterUserResult:
		{
			RegisterUserResult*result = (RegisterUserResult*)baseMsg->message;
			if(result && result->userid != -1){
				if(mUserSession){
					delete mUserSession;
				}

				mUserSession = new UserSession(result->userid,result->sessionid);				
				//savePassword(result->passwd);
				//assert(0);
				m_rc4KeyDec = InitRC4((unsigned char*)pendingPassword,strlen(pendingPassword));
				m_rc4KeyEnc = InitRC4((unsigned char*)pendingPassword,strlen(pendingPassword));
				if(result->logtype == 0){
					if(pendingEmail){
						m_loginEmail = pendingEmail;
						saveEmail(pendingEmail);
						delete[] pendingEmail;
						pendingEmail = 0;
					}
					if(pendingPassword){
						savePassword(pendingPassword);
						delete[] pendingPassword;
						pendingPassword = 0;
					}
					cocos2d::CCUserDefault::getInstance()->setStringForKey(USER_DEFAULT_USED_EMAIL,"1");
				}else{
					cocos2d::CCUserDefault::getInstance()->setStringForKey(USER_DEFAULT_USED_EMAIL,"0");
					saveEmail("");
					savePassword("");
				}
			}else{
				if(pendingEmail){
					delete[] pendingEmail;
					pendingEmail = 0;
				}
			}
		}
		break;
	default:
		break;						   
	}

	if(mLoadingLayer){
		mLoadingLayer->removeSelf();
		mLoadingLayer = nullptr;
	}

	if(baseMsg->message){
		if(baseMsg->msgType == PROTO_TYPE_GetMailListResult){
			mGetMailListResult = (GetMailListResult*)baseMsg->message;
		}else{
			//protobuf_c_message_free_unpacked (baseMsg->message, 0);
		}
	}

	if(!messageCallback.empty())
	{
		auto temp = messageCallback;

		for( const auto &pair : temp )
		{
			pair.second(baseMsg->message,baseMsg->msgType);
		}
		//delete baseMsg->message;
	}else if(baseMsg->message){
		protobuf_c_message_free_unpacked (baseMsg->message, 0);
	}
	log("after users code processed proto");
	sendQueueMsg();
}

uint8_t* protobuf_get_package_buffer(unsigned int* p_size,struct ProtobufCMessage*base_msg,int msg_type){
	//char*res_buff = 0;
	switch(msg_type){
		PROTO_PACK_CASE(EmailLogin,email_login)
		PROTO_PACK_CASE(Activity,activity)
		PROTO_PACK_CASE(Task,task)
		PROTO_PACK_CASE(SysMessage,sys_message)
		PROTO_PACK_CASE(City,city)
		PROTO_PACK_CASE(LoginFirstData,login_first_data)
		PROTO_PACK_CASE(EmailLoginResult,email_login_result)
		PROTO_PACK_CASE(RegisterUser,register_user)
#if ADD_PROTO_SAMPEL
		PROTO_PACK_CASE(ProtoSample,proto_sample)
		PROTO_PACK_CASE(ProtoSampleResult,proto_sample_result)
#endif
		//PROTO_PACK_CASE(SailCityFinish,sail_city_finish)
		PROTO_PACK_CASE(RegisterUserResult,register_user_result)
		PROTO_PACK_CASE(CreateCharacter,create_character)
		PROTO_PACK_CASE(CreateCharacterFailed,create_character_failed)
		PROTO_PACK_CASE(GetFleetCargo,get_fleet_cargo)
		PROTO_PACK_CASE(GetFleetCargoResult,get_fleet_cargo_result)
		PROTO_PACK_CASE(GetGoodsForShip,get_goods_for_ship)
		PROTO_PACK_CASE(GetGoodsForShipResult,get_goods_for_ship_result)
		PROTO_PACK_CASE(ExchangeGoodsForShips,exchange_goods_for_ships)
		PROTO_PACK_CASE(ExchangeGoodsForShipsResult,exchange_goods_for_ships_result)
		PROTO_PACK_CASE(CheckMailBox,check_mail_box)
		PROTO_PACK_CASE(CheckMailBoxResult,check_mail_box_result)
		PROTO_PACK_CASE(GetMailList,get_mail_list)
		PROTO_PACK_CASE(GetMailListResult,get_mail_list_result)
		PROTO_PACK_CASE(SendMail,send_mail)
		PROTO_PACK_CASE(SendMailResult,send_mail_result)
		PROTO_PACK_CASE(MarkMailStatus,mark_mail_status)
	    	PROTO_PACK_CASE(MarkMailStatusResult,mark_mail_status_result)
	    PROTO_PACK_CASE(GetItemsBeingSold,get_items_being_sold)
	    PROTO_PACK_CASE(GetItemsBeingSoldResult,get_items_being_sold_result)
	    PROTO_PACK_CASE(GetLastPrice,get_last_price)
	    PROTO_PACK_CASE(GetLastPriceResult,get_last_price_result)
	    PROTO_PACK_CASE(GetOrderList,get_order_list)
	    PROTO_PACK_CASE(GetOrderListResult,get_order_list_result)
	    PROTO_PACK_CASE(BuyItem,buy_item)
	    PROTO_PACK_CASE(BuyItemResult,buy_item_result)
	    PROTO_PACK_CASE(SellItem,sell_item)
	    PROTO_PACK_CASE(SellItemResult,sell_item_result)
	    PROTO_PACK_CASE(CreateOrder,create_order)
	    PROTO_PACK_CASE(CreateOrderResult,create_order_result)
	    PROTO_PACK_CASE(SearchTradeItems,search_trade_items)
	    PROTO_PACK_CASE(SearchTradeItemsResult,search_trade_items_result)
	    PROTO_PACK_CASE(GetItemsToSell,get_items_to_sell)
    	    PROTO_PACK_CASE(GetItemsToSellResult,get_items_to_sell_result)
	    PROTO_PACK_CASE(GetPriceData,get_price_data)
	    PROTO_PACK_CASE(GetPriceDataResult,get_price_data_result)
	    PROTO_PACK_CASE(GetPersonalItem,get_personal_item)
	    PROTO_PACK_CASE(GetPersonalItemResult,get_personal_item_result)
 	    PROTO_PACK_CASE(CancelOrder, cancel_order)
	    PROTO_PACK_CASE(CancelOrderResult, cancel_order_result)
	    PROTO_PACK_CASE(WarehouseOpration, warehouse_opration)
	    PROTO_PACK_CASE(WarehouseOprationResult, warehouse_opration_result)
  	    PROTO_PACK_CASE(GetWarehouseItems, get_warehouse_items)
	    PROTO_PACK_CASE(GetWarehouseItemsResult, get_warehouse_items_result)
		PROTO_PACK_CASE(BarConsume, bar_consume)
		PROTO_PACK_CASE(BarConsumeResult, bar_consume_result)
		PROTO_PACK_CASE(GetBarInfo, get_bar_info)
		PROTO_PACK_CASE(GetBarInfoResult, get_bar_info_result)
		PROTO_PACK_CASE(GetCrew, get_crew)
		PROTO_PACK_CASE(GetCrewResult, get_crew_result)
		PROTO_PACK_CASE(TreatBargirl, treat_bargirl)
		PROTO_PACK_CASE(TreatBargirlResult, treat_bargirl_result)
		PROTO_PACK_CASE(GiveBargirlGift, give_bargirl_gift)
		PROTO_PACK_CASE(GiveBargirlGiftResult, give_bargirl_gift_result)
		PROTO_PACK_CASE(GetAvailCrewNum, get_avail_crew_num)
		PROTO_PACK_CASE(GetAvailCrewNumResult, get_avail_crew_num_result)
		PROTO_PACK_CASE(GetTaskList, get_task_list)
		PROTO_PACK_CASE(GetTaskListResult, get_task_list_result)
		PROTO_PACK_CASE(GetIntelligence, get_intelligence)
		PROTO_PACK_CASE(GetIntelligenceResult, get_intelligence_result)
		PROTO_PACK_CASE(GetCaptain, get_captain)
		PROTO_PACK_CASE(GetCaptainResult, get_captain_result)
		PROTO_PACK_CASE(HandleTask, handle_task)
		PROTO_PACK_CASE(HandleTaskResult, handle_task_result)
		PROTO_PACK_CASE(GetCurrentCityData, get_current_city_data)
		PROTO_PACK_CASE(GetCurrentCityDataResult, get_current_city_data_result)
		PROTO_PACK_CASE(GetShipList, get_ship_list)
		PROTO_PACK_CASE(GetShipListResult, get_ship_list_result)
		PROTO_PACK_CASE(RepairShip, repair_ship)
		PROTO_PACK_CASE(RepairShipResult, repair_ship_result)
		PROTO_PACK_CASE(RepairAllShips, repair_all_ships)
		PROTO_PACK_CASE(RepairAllShipsResult, repair_all_ships_result)
		PROTO_PACK_CASE(FleetFormation, fleet_formation)
		PROTO_PACK_CASE(FleetFormationResult, fleet_formation_result)
		PROTO_PACK_CASE(ParkShip, park_ship)
		PROTO_PACK_CASE(ParkShipResult, park_ship_result)
		PROTO_PACK_CASE(GetDrawings, get_drawings)
		PROTO_PACK_CASE(GetDrawingsResult, get_drawings_result)
		PROTO_PACK_CASE(BuildShip, build_ship)
		PROTO_PACK_CASE(BuildShipResult, build_ship_result)
		PROTO_PACK_CASE(GetBuildingShips, get_building_ships)
		PROTO_PACK_CASE(GetBuildingShipsResult, get_building_ships_result)
		PROTO_PACK_CASE(FinishBuilding, finish_building)
		PROTO_PACK_CASE(FinishBuildingResult, finish_building_result)
		PROTO_PACK_CASE(CancelBuild, cancel_build)
		PROTO_PACK_CASE(CancelBuildResult, cancel_build_result)
		PROTO_PACK_CASE(GetBuildingEquipment, get_building_equipment)
		PROTO_PACK_CASE(GetBuildingEquipmentResult, get_building_equipment_result)
		PROTO_PACK_CASE(GetFleetAndDockShips, get_fleet_and_dock_ships)
		PROTO_PACK_CASE(GetFleetAndDockShipsResult, get_fleet_and_dock_ships_result)
		PROTO_PACK_CASE(GetReinforceShipList, get_reinforce_ship_list)
		PROTO_PACK_CASE(GetReinforceShipListResult, get_reinforce_ship_list_result)
		PROTO_PACK_CASE(ReinforceShip, reinforce_ship)
		PROTO_PACK_CASE(ReinforceShipResult, reinforce_ship_result)
		PROTO_PACK_CASE(RerollShip, reroll_ship)
		PROTO_PACK_CASE(RerollShipResult, reroll_ship_result)
		PROTO_PACK_CASE(GetReinforceEquipList, get_reinforce_equip_list)
		PROTO_PACK_CASE(GetReinforceEquipListResult, get_reinforce_equip_list_result)
		PROTO_PACK_CASE(ReinforceEquip, reinforce_equip)
		PROTO_PACK_CASE(ReinforceEquipResult, reinforce_equip_result)
		PROTO_PACK_CASE(RerollEquip, reroll_equip)
		PROTO_PACK_CASE(RerollEquipResult, reroll_equip_result)
		PROTO_PACK_CASE(EquipShip, equip_ship)
		PROTO_PACK_CASE(EquipShipResult, equip_ship_result)
		PROTO_PACK_CASE(GetEquipShipInfo, get_equip_ship_info)
		PROTO_PACK_CASE(GetEquipShipInfoResult, get_equip_ship_info_result)
		PROTO_PACK_CASE(GetEquipableItems, get_equipable_items)
		PROTO_PACK_CASE(GetEquipableItemsResult, get_equipable_items_result)
		PROTO_PACK_CASE(GetVisitedCities, get_visited_cities)
		PROTO_PACK_CASE(GetVisitedCitiesResult, get_visited_cities_result)
		PROTO_PACK_CASE(AddSupply, add_supply)
		PROTO_PACK_CASE(AddSupplyResult, add_supply_result)
		PROTO_PACK_CASE(GetNextMapBlock, get_next_map_block)
		PROTO_PACK_CASE(GetNextMapBlockResult, get_next_map_block_result)
		PROTO_PACK_CASE(LeaveCity, leave_city)
		PROTO_PACK_CASE(LeaveCityResult, leave_city_result)
		PROTO_PACK_CASE(ReachCity, reach_city)
		PROTO_PACK_CASE(ReachCityResult, reach_city_result)
		PROTO_PACK_CASE(TriggerSeaEvent, trigger_sea_event)
		PROTO_PACK_CASE(TriggerSeaEventResult, trigger_sea_event_result)
		PROTO_PACK_CASE(FinishSeaEvent, finish_sea_event)
		PROTO_PACK_CASE(FinishSeaEventResult, finish_sea_event_result)
		PROTO_PACK_CASE(GetPalaceInfo, get_palace_info)
		PROTO_PACK_CASE(GetPalaceInfoResult, get_palace_info_result)
		PROTO_PACK_CASE(InvestCity, invest_city)
		PROTO_PACK_CASE(InvestCityResult, invest_city_result)
		PROTO_PACK_CASE(GetRankList, get_rank_list)
		PROTO_PACK_CASE(GetRankListResult, get_rank_list_result)
		PROTO_PACK_CASE(VoteMayor, vote_mayor)
		PROTO_PACK_CASE(VoteMayorResult, vote_mayor_result)
		PROTO_PACK_CASE(ChangeDevDir, change_dev_dir)
		PROTO_PACK_CASE(ChangeDevDirResult, change_dev_dir_result)
		PROTO_PACK_CASE(GetVoteList, get_vote_list)
		PROTO_PACK_CASE(GetVoteListResult, get_vote_list_result)
		PROTO_PACK_CASE(GetLabourUnion, get_labour_union)
		PROTO_PACK_CASE(GetLabourUnionResult, get_labour_union_result)
		PROTO_PACK_CASE(CreateLabourUnion, create_labour_union)
		PROTO_PACK_CASE(CreateLabourUnionResult, create_labour_union_result)
		PROTO_PACK_CASE(DeleteLabourUnion, delete_labour_union)
		PROTO_PACK_CASE(DeleteLabourUnionResult, delete_labour_union_result)
		PROTO_PACK_CASE(InviteLabour, invite_labour)
		PROTO_PACK_CASE(InviteLabourResult, invite_labour_result)
		PROTO_PACK_CASE(ApplyLabourUnion, apply_labour_union)
		PROTO_PACK_CASE(ApplyLabourUnionResult, apply_labour_union_result)
		PROTO_PACK_CASE(QuitLabourUnion, quit_labour_union)
		PROTO_PACK_CASE(QuitLabourUnionResult, quit_labour_union_result)
		PROTO_PACK_CASE(KickOutMember, kick_out_member)
		PROTO_PACK_CASE(KickOutMemberResult, kick_out_member_result)
		PROTO_PACK_CASE(AdjustPrivilege, adjust_privilege)
		PROTO_PACK_CASE(AdjustPrivilegeResult, adjust_privilege_result)
		PROTO_PACK_CASE(DepositMoney, deposit_money)
		PROTO_PACK_CASE(DepositMoneyResult, deposit_money_result)
		PROTO_PACK_CASE(WithdrawMoney, withdraw_money)
		PROTO_PACK_CASE(WithdrawMoneyResult, withdraw_money_result)
		PROTO_PACK_CASE(DepositItem, deposit_item)
		PROTO_PACK_CASE(DepositItemResult, deposit_item_result)
		PROTO_PACK_CASE(WithdrawItem, withdraw_item)
		PROTO_PACK_CASE(WithdrawItemResult, withdraw_item_result)
		PROTO_PACK_CASE(ExpandBank, expand_bank)
		PROTO_PACK_CASE(ExpandBankResult, expand_bank_result)
		PROTO_PACK_CASE(GetBankInfo, get_bank_info)
		PROTO_PACK_CASE(GetBankInfoResult, get_bank_info_result)
		PROTO_PACK_CASE(GetBankLog, get_bank_log)
		PROTO_PACK_CASE(GetBankLogResult, get_bank_log_result)
		PROTO_PACK_CASE(SetMainTaskStatus,set_main_task_status)
		PROTO_PACK_CASE(SetMainTaskStatusResult,set_main_task_status_result)
		PROTO_PACK_CASE(EngageInFight, engage_in_fight)
		PROTO_PACK_CASE(EngageInFightResult, engage_in_fight_result)
		PROTO_PACK_CASE(EndFight, end_fight)
		PROTO_PACK_CASE(EndFightResult, end_fight_result)
		PROTO_PACK_CASE(StartAutoFight, start_auto_fight)
		PROTO_PACK_CASE(StartAutoFightResult, start_auto_fight_result)
		PROTO_PACK_CASE(GetSkillsDetails, get_skills_details)
		PROTO_PACK_CASE(GetSkillsDetailsResult, get_skills_details_result)
		PROTO_PACK_CASE(AddSkillPoint, add_skill_point)
		PROTO_PACK_CASE(AddSkillPointResult, add_skill_point_result)
		PROTO_PACK_CASE(ResetSkillPoints, reset_skill_points)
		PROTO_PACK_CASE(ResetSkillPointsResult, reset_skill_points_result)
		PROTO_PACK_CASE(NotifyBattleLog, notify_battle_log)
		PROTO_PACK_CASE(NotifyBattleLogResult, notify_battle_log_result)
		PROTO_PACK_CASE(GetPersonalInfo, get_personal_info)
		PROTO_PACK_CASE(GetPersonalInfoResult, get_personal_info_result)
		PROTO_PACK_CASE(SetBioInfo, set_bio_info)
		PROTO_PACK_CASE(SetBioInfoResult, set_bio_info_result)
		PROTO_PACK_CASE(CancelFight, cancel_fight)
		PROTO_PACK_CASE(CancelFightResult, cancel_fight_result)
		PROTO_PACK_CASE(GetGuildList, get_guild_list)
		PROTO_PACK_CASE(GetGuildListResult, get_guild_list_result)
		PROTO_PACK_CASE(GetMyGuildDetails, get_my_guild_details)
		PROTO_PACK_CASE(GetMyGuildDetailsResult, get_my_guild_details_result)
		PROTO_PACK_CASE(GetApplyInfo, get_apply_info)
		PROTO_PACK_CASE(GetApplyInfoResult, get_apply_info_result)
		PROTO_PACK_CASE(CreateNewGuild, create_new_guild)
		PROTO_PACK_CASE(CreateNewGuildResult, create_new_guild_result)
		PROTO_PACK_CASE(JoinGuildRequest, join_guild_request)
		PROTO_PACK_CASE(JoinGuildRequestResult, join_guild_request_result)
		PROTO_PACK_CASE(GetGuildDetails, get_guild_details)
		PROTO_PACK_CASE(GetGuildDetailsResult, get_guild_details_result)
		PROTO_PACK_CASE(ChangeGuildIntro, change_guild_intro)
		PROTO_PACK_CASE(ChangeGuildIntroResult, change_guild_intro_result)
		PROTO_PACK_CASE(ExpandGuildCapacity, expand_guild_capacity)
		PROTO_PACK_CASE(ExpandGuildCapacityResult, expand_guild_capacity_result)
		PROTO_PACK_CASE(GetGuildMemberPermission, get_guild_member_permission)
		PROTO_PACK_CASE(GetGuildMemberPermissionResult, get_guild_member_permission_result)
		PROTO_PACK_CASE(SetGuildMemberPermission, set_guild_member_permission)
		PROTO_PACK_CASE(SetGuildMemberPermissionResult, set_guild_member_permission_result)
		PROTO_PACK_CASE(GetHiredCaptains, get_hired_captains)
		PROTO_PACK_CASE(GetHiredCaptainsResult, get_hired_captains_result)
		PROTO_PACK_CASE(DealWithGuildJoin, deal_with_guild_join)
		PROTO_PACK_CASE(DealWithGuildJoinResult, deal_with_guild_join_result)
		PROTO_PACK_CASE(ChangeGuildAnnouncement, change_guild_announcement)
		PROTO_PACK_CASE(ChangeGuildAnnouncementResult, change_guild_announcement_result)
		PROTO_PACK_CASE(RemoveGuildMember, remove_guild_member)
		PROTO_PACK_CASE(RemoveGuildMemberResult, remove_guild_member_result)
		PROTO_PACK_CASE(ExitFromGuild, exit_from_guild)
		PROTO_PACK_CASE(ExitFromGuildResult, exit_from_guild_result)
		PROTO_PACK_CASE(DismissGuild, dismiss_guild)
		PROTO_PACK_CASE(DismissGuildResult, dismiss_guild_result)
		PROTO_PACK_CASE(GetSearchGuildList, get_search_guild_list)
		PROTO_PACK_CASE(GetSearchGuildListResult, get_search_guild_list_result)
		PROTO_PACK_CASE(RefreshCaptainList, refresh_captain_list)
		PROTO_PACK_CASE(RefreshCaptainListResult, refresh_captain_list_result)
		PROTO_PACK_CASE(GetFriendsList, get_friends_list)
		PROTO_PACK_CASE(GetFriendsListResult, get_friends_list_result)
		PROTO_PACK_CASE(GetBlackList, get_black_list)
		PROTO_PACK_CASE(GetBlackListResult, get_black_list_result)
		PROTO_PACK_CASE(FriendsOperation, friends_operation)
		PROTO_PACK_CASE(FriendsOperationResult, friends_operation_result)
		PROTO_PACK_CASE(GetFriendsReqList, get_friends_req_list)
		PROTO_PACK_CASE(GetFriendsReqListResult, get_friends_req_list_result)
		PROTO_PACK_CASE(FireCaptain, fire_captain)
		PROTO_PACK_CASE(FireCaptainResult, fire_captain_result)
		PROTO_PACK_CASE(DestoryItems, destory_items)
		PROTO_PACK_CASE(DestoryItemsResult, destory_items_result)
		PROTO_PACK_CASE(SearchUserByName, search_user_by_name)
		PROTO_PACK_CASE(SearchUserByNameResult, search_user_by_name_result)
		PROTO_PACK_CASE(TakeAllAttachment, take_all_attachment)
		PROTO_PACK_CASE(TakeAllAttachmentResult, take_all_attachment_result)
		PROTO_PACK_CASE(GetItemsDetailInfo, get_items_detail_info)
		PROTO_PACK_CASE(GetItemsDetailInfoResult, get_items_detail_info_result)
		PROTO_PACK_CASE(GetDevGoodsInfo, get_dev_goods_info)
		PROTO_PACK_CASE(GetDevGoodsInfoResult, get_dev_goods_info_result)
		PROTO_PACK_CASE(ProvideDevGoods, provide_dev_goods)
		PROTO_PACK_CASE(ProvideDevGoodsResult, provide_dev_goods_result)
		PROTO_PACK_CASE(GetDevGoodsCount, get_dev_goods_count)
		PROTO_PACK_CASE(GetDevGoodsCountResult, get_dev_goods_count_result)
		PROTO_PACK_CASE(GetFriendValue, get_friend_value)
		PROTO_PACK_CASE(GetFriendValueResult, get_friend_value_result)
		PROTO_PACK_CASE(GetUserInfoById, get_user_info_by_id)
		PROTO_PACK_CASE(GetUserInfoByIdResult, get_user_info_by_id_result)
		PROTO_PACK_CASE(TestCreateNewGuild, test_create_new_guild)
		PROTO_PACK_CASE(TestCreateNewGuildResult, test_create_new_guild_result)
		PROTO_PACK_CASE(InviteUserToGuild, invite_user_to_guild)
		PROTO_PACK_CASE(InviteUserToGuildResult, invite_user_to_guild_result)
		PROTO_PACK_CASE(GetInvitationList, get_invitation_list)
		PROTO_PACK_CASE(GetInvitationListResult, get_invitation_list_result)
		PROTO_PACK_CASE(DealWithInvitation, deal_with_invitation)
		PROTO_PACK_CASE(DealWithInvitationResult, deal_with_invitation_result)
		PROTO_PACK_CASE(GetVTicketMarketItems, get_vticket_market_items)
		PROTO_PACK_CASE(GetVTicketMarketItemsResult, get_vticket_market_items_result)
		PROTO_PACK_CASE(BuyVTicketMarketItem, buy_vticket_market_item)
		PROTO_PACK_CASE(BuyVTicketMarketItemResult, buy_vticket_market_item_result)
		PROTO_PACK_CASE(GetCurrentInvestData, get_current_invest_data)
		PROTO_PACK_CASE(GetCurrentInvestDataResult, get_current_invest_data_result)
		PROTO_PACK_CASE(GetUserTasks, get_user_tasks)
		PROTO_PACK_CASE(GetUserTasksResult, get_user_tasks_result)
		PROTO_PACK_CASE(UseSudanMask, use_sudan_mask)
		PROTO_PACK_CASE(UseSudanMaskResult, use_sudan_mask_result)
		PROTO_PACK_CASE(EquipHero, equip_hero)
		PROTO_PACK_CASE(EquipHeroResult, equip_hero_result)
		PROTO_PACK_CASE(GetHeroEquip, get_hero_equip)
		PROTO_PACK_CASE(GetHeroEquipResult, get_hero_equip_result)
		PROTO_PACK_CASE(BuyCityLicense, buy_city_license)
		PROTO_PACK_CASE(BuyCityLicenseResult, buy_city_license_result)
		PROTO_PACK_CASE(DeleteUser, delete_user)
		PROTO_PACK_CASE(DeleteUserResult, delete_user_result)
		PROTO_PACK_CASE(ChangePassword, change_password)
		PROTO_PACK_CASE(ChangePasswordResult, change_password_result)
		PROTO_PACK_CASE(SelectHeroPositiveSkill, select_hero_positive_skill)
		PROTO_PACK_CASE(SelectHeroPositiveSkillResult, select_hero_positive_skill_result)
		PROTO_PACK_CASE(GetHeroPositiveSkillList, get_hero_positive_skill_list)
		PROTO_PACK_CASE(GetHeroPositiveSkillListResult, get_hero_positive_skill_list_result)
		PROTO_PACK_CASE(GetSelectedPositiveSkills, get_selected_positive_skills)
		PROTO_PACK_CASE(GetSelectedPositiveSkillsResult, get_selected_positive_skills_result)
		PROTO_PACK_CASE(GetSailInfo, get_sail_info)
		PROTO_PACK_CASE(GetSailInfoResult, get_sail_info_result)
		PROTO_PACK_CASE(InsertUserName, insert_user_name)
		PROTO_PACK_CASE(InsertUserNameResult, insert_user_name_result)
		PROTO_PACK_CASE(GetLeaderboard, get_leaderboard)
		PROTO_PACK_CASE(GetLeaderboardResult, get_leaderboard_result)
		PROTO_PACK_CASE(GetCityStatus, get_city_status)
		PROTO_PACK_CASE(GetCityStatusResult, get_city_status_result)
		PROTO_PACK_CASE(GetDiscoveredCities, get_discovered_cities)
		PROTO_PACK_CASE(GetDiscoveredCitiesResult, get_discovered_cities_result)
		PROTO_PACK_CASE(AddDiscoveredCity, add_discovered_city)
		PROTO_PACK_CASE(AddDiscoveredCityResult, add_discovered_city_result)
		PROTO_PACK_CASE(ForceFightStatus, force_fight_status)
		PROTO_PACK_CASE(ForceFightStatusResult, force_fight_status_result)
		PROTO_PACK_CASE(GetCityProduces, get_city_produces)
		PROTO_PACK_CASE(GetCityProducesResult, get_city_produces_result)
		PROTO_PACK_CASE(GetCityDemands, get_city_demands)
		PROTO_PACK_CASE(GetCityDemandsResult, get_city_demands_result)
		PROTO_PACK_CASE(GetMainTask, get_main_task)
		PROTO_PACK_CASE(GetMainTaskResult, get_main_task_result)
		PROTO_PACK_CASE(CompleteMainTask, complete_main_task)
		PROTO_PACK_CASE(CompleteMainTaskResult, complete_main_task_result)
		PROTO_PACK_CASE(GetMainTaskReward, get_main_task_reward)
		PROTO_PACK_CASE(GetMainTaskRewardResult, get_main_task_reward_result)
		PROTO_PACK_CASE(GetEventTaskList, get_event_task_list)
		PROTO_PACK_CASE(GetEventTaskListResult, get_event_task_list_result)
		PROTO_PACK_CASE(HandleEventTask, handle_event_task)
		PROTO_PACK_CASE(HandleEventTaskResult, handle_event_task_result)
		PROTO_PACK_CASE(GetNpcScore, get_npc_score)
		PROTO_PACK_CASE(GetNpcScoreResult, get_npc_score_result)
		PROTO_PACK_CASE(GetServerInfo, get_server_info)
		PROTO_PACK_CASE(GetServerInfoResult, get_server_info_result)
		PROTO_PACK_CASE(PingServer, ping_server)
		PROTO_PACK_CASE(PingServerResult, ping_server_result)
		PROTO_PACK_CASE(ExpandPackageSize,expand_package_size)
		PROTO_PACK_CASE(ExpandPackageSizeResult,expand_package_size_result)
		PROTO_PACK_CASE(GetPretaskItems,get_pretask_items)
		PROTO_PACK_CASE(GetPretaskItemsResult,get_pretask_items_result)
		PROTO_PACK_CASE(GetOnsaleItems,get_onsale_items)
		PROTO_PACK_CASE(GetOnsaleItemsResult,get_onsale_items_result)
		PROTO_PACK_CASE(BuyOnsaleItems, buy_onsale_items)
		PROTO_PACK_CASE(BuyOnsaleItemsResult, buy_onsale_items_result)
		PROTO_PACK_CASE(SendCustomServiceInfo, send_custom_service_info)
		PROTO_PACK_CASE(SendCustomServiceInfoResult, send_custom_service_info_result)
		PROTO_PACK_CASE(GetIABItem, get_iabitem)
		PROTO_PACK_CASE(GetIABItemResult, get_iabitem_result)
		PROTO_PACK_CASE(ChangeEmail, change_email)
		PROTO_PACK_CASE(ChangeEmailResult, change_email_result)
		PROTO_PACK_CASE(TokenLogin, token_login)
		PROTO_PACK_CASE(EngageInFightForTask, engage_in_fight_for_task)
		PROTO_PACK_CASE(EngageInFightForTaskResult, engage_in_fight_for_task_result)
		PROTO_PACK_CASE(AuthenticateAccount, authenticate_account)
		PROTO_PACK_CASE(AuthenticateAccountResult, authenticate_account_result)
		PROTO_PACK_CASE(Salvage, salvage)
		PROTO_PACK_CASE(SalvageResult, salvage_result)
		PROTO_PACK_CASE(GetEmailVerifiedReward, get_email_verified_reward)
		PROTO_PACK_CASE(GetEmailVerifiedRewardResult, get_email_verified_reward_result)
		PROTO_PACK_CASE(StartSalvage, start_salvage)
		PROTO_PACK_CASE(StartSalvageResult, start_salvage_result)
		PROTO_PACK_CASE(GetEmailVerifiedInfo, get_email_verified_info)
		PROTO_PACK_CASE(GetEmailVerifiedInfoResult,get_email_verified_info_result)
		PROTO_PACK_CASE(GetSalvageCondition, get_salvage_condition)
		PROTO_PACK_CASE(GetSalvageConditionResult,get_salvage_condition_result)
		PROTO_PACK_CASE(GetActivitiesProps, get_activities_props)
		PROTO_PACK_CASE(GetActivitiesPropsResult,get_activities_props_result)
		PROTO_PACK_CASE(GetActivitiesGiftInfo, get_activities_gift_info)
		PROTO_PACK_CASE(GetActivitiesGiftInfoResult,get_activities_gift_info_result)
		PROTO_PACK_CASE(GetActivitiesPrayInfo, get_activities_pray_info)
		PROTO_PACK_CASE(GetActivitiesPrayInfoResult,get_activities_pray_info_result)
		PROTO_PACK_CASE(GetPray, get_pray)
		PROTO_PACK_CASE(GetPrayResult,get_pray_result)
		PROTO_PACK_CASE(SendActivitiesGift, send_activities_gift)
		PROTO_PACK_CASE(SendActivitiesGiftResult,send_activities_gift_result)
		PROTO_PACK_CASE(GetPirateAttackInfo, get_pirate_attack_info)
		PROTO_PACK_CASE(GetPirateAttackInfoResult,get_pirate_attack_info_result)
		PROTO_PACK_CASE(GetAttackPirateInfo,get_attack_pirate_info)
		PROTO_PACK_CASE(GetAttackPirateInfoResult,get_attack_pirate_info_result)
		PROTO_PACK_CASE(GetVTicketMarketActivities,get_vticket_market_activities)
		PROTO_PACK_CASE(GetVTicketMarketActivitiesResult,get_vticket_market_activities_result)
		PROTO_PACK_CASE(BuyVTicketMarketActivities,buy_vticket_market_activities)
		PROTO_PACK_CASE(BuyVTicketMarketActivitiesResult,buy_vticket_market_activities_result)
		PROTO_PACK_CASE(FinalMyExploitScore,final_my_exploit_score)
		PROTO_PACK_CASE(FinalMyExploitScoreResult,final_my_exploit_score_result)
		PROTO_PACK_CASE(FindLootPlayer,find_loot_player)
		PROTO_PACK_CASE(FindLootPlayerResult,find_loot_player_result)
		PROTO_PACK_CASE(LootPlayer,loot_player)
		PROTO_PACK_CASE(LootPlayerResult,loot_player_result)
		PROTO_PACK_CASE(GetLootPlayerLog,get_loot_player_log)
		PROTO_PACK_CASE(GetLootPlayerLogResult,get_loot_player_log_result)
		PROTO_PACK_CASE(StartRevengeOnLootPlayer,start_revenge_on_loot_player)
		PROTO_PACK_CASE(GetAttackPirateRankInfo,get_attack_pirate_rank_info)
		PROTO_PACK_CASE(GetAttackPirateRankInfoResult,get_attack_pirate_rank_info_result)
		PROTO_PACK_CASE(GetAttackPirateBossInfo,get_attack_pirate_boss_info)
		PROTO_PACK_CASE(GetAttackPirateBossInfoResult,get_attack_pirate_boss_info_result)
		PROTO_PACK_CASE(BuyInsurance,buy_insurance)
		PROTO_PACK_CASE(BuyInsuranceResult,buy_insurance_result)
		PROTO_PACK_CASE(CompleteDialogs, complete_dialogs)
		PROTO_PACK_CASE(CompleteDialogsResult, complete_dialogs_result)
		PROTO_PACK_CASE(ObtainLottoMessage, obtain_lotto_message)
		PROTO_PACK_CASE(ObtainLottoMessageResult, obtain_lotto_message_result)
		PROTO_PACK_CASE(GetInviteBonus, get_invite_bonus)
		PROTO_PACK_CASE(GetInviteBonusResult, get_invite_bonus_result)
		PROTO_PACK_CASE(GetCompanies, get_companies)
		PROTO_PACK_CASE(GetCompaniesResult, get_companies_result)
		PROTO_PACK_CASE(EquipCompany, equip_company)
		PROTO_PACK_CASE(EquipCompanyResult, equip_company_result)
		PROTO_PACK_CASE(UnlockCompany, unlock_company)
		PROTO_PACK_CASE(UnlockCompanyResult, unlock_company_result)
		PROTO_PACK_CASE(GetCompanionsStatus, get_companions_status)
		PROTO_PACK_CASE(GetCompanionsStatusResult, get_companions_status_result)
		PROTO_PACK_CASE(UnlockShipCabin, unlock_ship_cabin)
		PROTO_PACK_CASE(UnlockShipCabinResult, unlock_ship_cabin_result)
		PROTO_PACK_CASE(GetShipCompanions, get_ship_companions)
		PROTO_PACK_CASE(GetShipCompanionsResult, get_ship_companions_result)
		PROTO_PACK_CASE(SetShipCompanions, set_ship_companions)
		PROTO_PACK_CASE(SetShipCompanionsResult, set_ship_companions_result)
		PROTO_PACK_CASE(SaveFailedFight, save_failed_fight)
		PROTO_PACK_CASE(SaveFailedFightResult, save_failed_fight_result)
		PROTO_PACK_CASE(EndFailedFightByVTicket, end_failed_fight_by_vticket)
		PROTO_PACK_CASE(EndFailedFightByVTicketResult, end_failed_fight_by_vticket_result)
		PROTO_PACK_CASE(CompleteCompanionTask, complete_companion_task)
		PROTO_PACK_CASE(CompleteCompanionTaskResult, complete_companion_task_result)
		PROTO_PACK_CASE(DeductCoins, deduct_coins)
		PROTO_PACK_CASE(DeductCoinsResult, deduct_coins_result)
		PROTO_PACK_CASE(UpdateDataVersion, update_data_version)
		PROTO_PACK_CASE(UpdateDataVersionResult, update_data_version_result)
		PROTO_PACK_CASE(CalFastHireCrew, cal_fast_hire_crew)
		PROTO_PACK_CASE(CalFastHireCrewResult, cal_fast_hire_crew_result)
		PROTO_PACK_CASE(CompanionRebirth, companion_rebirth)
		PROTO_PACK_CASE(CompanionRebirthResult, companion_rebirth_result)
		PROTO_PACK_CASE(RepairEquipment, repair_equipment)
		PROTO_PACK_CASE(RepairEquipmentResult, repair_equipment_result)
		PROTO_PACK_CASE(GetRepairEquipmentNeed, get_repair_equipment_need)
		PROTO_PACK_CASE(GetRepairEquipmentNeedResult, get_repair_equipment_need_result)
		PROTO_PACK_CASE(GetRecentOrder, get_recent_order)
		PROTO_PACK_CASE(GetRecentOrderResult, get_recent_order_result)
		PROTO_PACK_CASE(GetDropIncreaseRateInfo, get_drop_increase_rate_info)
		PROTO_PACK_CASE(GetDropIncreaseRateInfoResult, get_drop_increase_rate_info_result)
		PROTO_PACK_CASE(GetDailyActivitiesReward, get_daily_activities_reward)
		PROTO_PACK_CASE(GetDailyActivitiesRewardResult, get_daily_activities_reward_result)
		PROTO_PACK_CASE(AddProficiency,add_proficiency)
		PROTO_PACK_CASE(AddProficiencyResult, add_proficiency_result)
		PROTO_PACK_CASE(GetProficiencyValues, get_proficiency_values)
		PROTO_PACK_CASE(GetProficiencyValuesResult, get_proficiency_values_result)
//		PROTO_PACK_CASE(PrepareCityDefense, prepare_city_defense)
//		PROTO_PACK_CASE(PrepareCityDefenseResult, prepare_city_defense_result)
//		PROTO_PACK_CASE(PrepareCityAttack, prepare_city_attack)
//		PROTO_PACK_CASE(PrepareCityAttackResult, prepare_city_attack_result)
		PROTO_PACK_CASE(GetCityPrepareStatus, get_city_prepare_status)
		PROTO_PACK_CASE(GetCityPrepareStatusResult, get_city_prepare_status_result)
//		PROTO_PACK_CASE(StartStateWar, start_state_war)
//		PROTO_PACK_CASE(StartStateWarResult, start_state_war_result)
		PROTO_PACK_CASE(GetEnemyNation, get_enemy_nation)
		PROTO_PACK_CASE(GetEnemyNationResult, get_enemy_nation_result)
		PROTO_PACK_CASE(AddCityRepairPool, add_city_repair_pool)
		PROTO_PACK_CASE(AddCityRepairPoolResult, add_city_repair_pool_result)
		PROTO_PACK_CASE(AddCityAttackPool, add_city_attack_pool)
		PROTO_PACK_CASE(AddCityAttackPoolResult, add_city_attack_pool_result)
		PROTO_PACK_CASE(GetPackageInfo, get_package_info)
		PROTO_PACK_CASE(GetPackageInfoResult, get_package_info_result)
		PROTO_PACK_CASE(GetProficiencyBook, get_proficiency_book)
		PROTO_PACK_CASE(GetProficiencyBookResult, get_proficiency_book_result)
		PROTO_PACK_CASE(GetNationWarEntrance, get_nation_war_entrance)
		PROTO_PACK_CASE(GetNationWarEntranceResult, get_nation_war_entrance_result)
		PROTO_PACK_CASE(GetContributionRank, get_contribution_rank)
		PROTO_PACK_CASE(GetContributionRankResult, get_contribution_rank_result)
		PROTO_PACK_CASE(GetNationWarCityLost, get_nation_war_city_lost)
		PROTO_PACK_CASE(GetNationWarCityLostResult, get_nation_war_city_lost_result)
		PROTO_PACK_CASE(ApplyStateWar, apply_state_war)
		PROTO_PACK_CASE(ApplyStateWarResult, apply_state_war_result)
		PROTO_PACK_CASE(StartAttackCity, start_attack_city)
		PROTO_PACK_CASE(StartAttackCityResult, start_attack_city_result)
		PROTO_PACK_CASE(HireArmy, hire_army)
		PROTO_PACK_CASE(HireArmyResult, hire_army_result)
		PROTO_PACK_CASE(AddSupplyStation, add_supply_station)
		PROTO_PACK_CASE(AddSupplyStationResult, add_supply_station_result)
		PROTO_PACK_CASE(EndAttackCity, end_attack_city)
		PROTO_PACK_CASE(EndAttackCityResult, end_attack_city_result)
		PROTO_PACK_CASE(RepairCityDefenseInWar, repair_city_defense_in_war)
		PROTO_PACK_CASE(RepairCityDefenseInWarResult, repair_city_defense_in_war_result)
		PROTO_PACK_CASE(EnhanceAttack, enhance_attack)
		PROTO_PACK_CASE(EnhanceAttackResult, enhance_attack_result)
		PROTO_PACK_CASE(ReachDepot, reach_depot)
		PROTO_PACK_CASE(ReachDepotResult, reach_depot_result)
		PROTO_PACK_CASE(ReduceInNationWar, reduce_in_nation_war)
		PROTO_PACK_CASE(ReduceInNationWarResult, reduce_in_nation_war_result)
		PROTO_PACK_CASE(GetMyNationWar, get_my_nation_war)
		PROTO_PACK_CASE(GetMyNationWarResult, get_my_nation_war_result)
		PROTO_PACK_CASE(FuncForTest, func_for_test)
		PROTO_PACK_CASE(FuncForTestResult, func_for_test_result)	
		PROTO_PACK_CASE(CheckDeleteCharactersNum, check_delete_characters_num)
		PROTO_PACK_CASE(CheckDeleteCharactersNumResult, check_delete_characters_num_result)
		PROTO_PACK_CASE(GetPlayerShipList, get_player_ship_list)
		PROTO_PACK_CASE(GetPlayerShipListResult, get_player_ship_list_result)
		PROTO_PACK_CASE(GetPlayerEquipShipInfo, get_player_equip_ship_info)
		PROTO_PACK_CASE(GetPlayerEquipShipInfoResult, get_player_equip_ship_info_result)
		PROTO_PACK_CASE(FireCrew, fire_crew)
		PROTO_PACK_CASE(FireCrewResult, fire_crew_result)
		PROTO_PACK_CASE(GetForceCity, get_force_city)
		PROTO_PACK_CASE(GetForceCityResult, get_force_city_result)
		PROTO_PACK_CASE(GetChangeLog, get_change_log)
		PROTO_PACK_CASE(GetChangeLogResult, get_change_log_result)
	default:
		break;
	}
	return 0;
}

bool ProtocolThread::sendMessage(const char*buffer,int size)
{
	ODSocket cSocket=TVSocketHolder::getInstance()->getSocket();
	int count=cSocket.Send((const char*)buffer,size,0);
	idleSeconds = 0;
	waitingReponseSeconds = 0;
	log("---------------------send real,%d == %d",size,count);
	if(count == size){
		if(mLoadingLayer){
			mLoadingLayer->showSelf();
		}
		isWaitingResponse = true;
		//free(buffer);
		gettimeofday(&lastSendMessageTime, nullptr);
		return true;
	}else if(count < 0){
		//free(buffer);
		cSocket.Close(__FILE__,__LINE__);
		TVSocketHolder::getInstance()->setConnectionStatus(false);
		return false;
	}else{
		Buffer*buff = new Buffer;
		buff->append((const void*)(buffer+count),size - count);
		ProtocolThread::GetInstance()->addBufferToSendingQueue(buff);	
		if(mLoadingLayer){
			mLoadingLayer->showSelf();
		}
		//free(buffer);
		return true;
	}
}

bool ProtocolThread::sendMessage(struct ProtobufCMessage* message,int type)
{
	unsigned int totalBufSize = 0;
	//create msg buffer
	uint8_t* msgBuffer = protobuf_get_package_buffer(&totalBufSize, message, type);

	//write rule
	ProtocolDefine::WritePacket(type, msgBuffer, totalBufSize, m_rc4KeyEnc != NULL, m_rc4KeyEnc);
	log("send message:%d   --------- %d\n",isWaitingResponse,type);

	if(isWaitingResponse || queueMsgBuffer.size()>0)
	{
		Buffer*buff = new Buffer;
		buff->mLoadingLayer = mBackupLoadingLayer;
		buff->append((const void*)(msgBuffer), totalBufSize);
		mBackupLoadingLayer = 0;
		queueMsgBuffer.push_back(buff);
		//free msg buffer created by writter.
		free(msgBuffer);
		return false;
	}

	bool res = sendMessage((char*)msgBuffer,totalBufSize);
	//free msg buffer created by writter.
	free(msgBuffer);
	return res;
}

void ProtocolThread::insertUserName(const char*name,int gender,int iconIdx,UILoadingIndicator*loading){
	if(mLoadingLayer)
			return;
	setLoadingRes(loading);
	InsertUserName create = INSERT_USER_NAME__INIT;
	create.username = (char*)name;
	create.gender = gender;
	create.iconidx = iconIdx;
	if(mUserSession){
		create.userid = mUserSession->getUserId();
		create.sessionid = mUserSession->getSessionId();
		sendMessage((struct ProtobufCMessage*)&create,PROTO_TYPE_InsertUserName);
	}
}

void ProtocolThread::createCharacter(int companionType,int nation,UILoadingIndicator*loading)
{
	if(mLoadingLayer)
		return;
	setLoadingRes(loading);
	CreateCharacter create = CREATE_CHARACTER__INIT;
	create.nationidx = nation;
	create.companiontype = companionType;
	if(mUserSession){
		create.userid = mUserSession->getUserId();
		create.sessionid = mUserSession->getSessionId();
		auto notificationToken = Utils::GetGCMToken();
		create.ostype = Utils::GetOSType();
		create.notificationid = (char*)notificationToken.c_str();

		sendMessage((struct ProtobufCMessage*)&create,PROTO_TYPE_CreateCharacter);
	}
}


void ProtocolThread::userEmailLogin(const char*email,const char*passwd,UILoadingIndicator*loading)
{
	//log("userEmailLogin load:%d,email:%s,isWaitingResponse:%d",mLoadingLayer!=0,email,isWaitingResponse);
	if(mLoadingLayer)
		return;
	setLoadingRes(loading);
	EmailLogin login = EMAIL_LOGIN__INIT;


	login.email = (char*)email;
	//login.passwd = (char*)passwd;
	int outLen = 0;
	login.passwd1.data = RSAEncrypt((unsigned char*)passwd,strlen(passwd),&outLen);
	login.passwd1.len = outLen;
	login.networktype = (char*)Utils::GetNetWorkString().c_str();
	login.languagecode = Utils::getLanguage();
	login.apiurl = (char*)ServerManager::CurSelectedServer()->GetServerAddr().c_str();
	auto refcode = Utils::GetFaceBookRefcode();
	login.refcode = (char*)refcode.c_str();
	login.jsonmd5 = SINGLE_SHOP->getJsonSigure((int *)&login.n_jsonmd5);
	savePendigEmail(copyCString(email));
	savePendingPassword(copyCString(passwd));
	std::string changedEmail = getChangedEmail();
	if(changedEmail.length() > 0){
		login.changedemail = (char*)changedEmail.c_str();
		if(strcmp(email,login.changedemail) == 0){
			saveChangedEmail("");
		}
	}
	std::string metaData = Utils::GetDeviceInfo();
	login.deviceinfo = (char*)metaData.c_str();
	log("device info:%s",login.deviceinfo);
	auto deviceHash = Utils::GetDeviceHash();
	login.device_hash = (char*)deviceHash.c_str();

	auto notificationToken = Utils::GetGCMToken();
	login.ostype = Utils::GetOSType();
	login.notificationid = (char*)notificationToken.c_str();

	sendMessage((struct ProtobufCMessage*)&login,PROTO_TYPE_EmailLogin);
	free(login.passwd1.data);
}

long g_seed = 100;
void getRandomString(char*buffer, int len) {
	long seed = g_seed ^ time(NULL);
	g_seed++;
	srand(seed);
	int j;
	for (j = 0; j < len; j++) {
		int flag = rand() % 2;
		if (flag)
			buffer[j] = 'A' + rand() % 26;
		else
			buffer[j] = 'a' + rand() % 26;
	}
	buffer[j] = '\0';
}

void ProtocolThread::registerUser(const char*email,const char*password,UILoadingIndicator*loading)
{
	if(mLoadingLayer)
		return;
	setLoadingRes(loading);
	RegisterUser reg = REGISTER_USER__INIT;
	reg.email = (char*)email;
	reg.networktype = (char*)Utils::GetNetWorkString().c_str();
	auto refcode = Utils::GetFaceBookRefcode();
	reg.refcode = (char*)refcode.c_str();

	auto deviceHash = Utils::GetDeviceHash();
	reg.device_hash = (char*)deviceHash.c_str();
	reg.deviceinfo = (char*)Utils::GetDeviceInfo().c_str();
//	char passwd[9];
//	passwd[8] = 0;
//	int pass_len = 8;
//	getRandomString(passwd, pass_len);
	int outLen = 0;
	reg.passwd1.data = RSAEncrypt((unsigned char*)password,strlen(password),&outLen);
	reg.passwd1.len = outLen;

	savePendigEmail(copyCString(email));
	savePendingPassword(copyCString(password));

	reg.languagecode = Utils::getLanguage();
	reg.apiurl = (char*)ServerManager::CurSelectedServer()->GetServerAddr().c_str();
	sendMessage((struct ProtobufCMessage*)&reg,PROTO_TYPE_RegisterUser);
	log("reg.device_hash:%s networktype:%s",reg.device_hash,reg.networktype);
//	m_rc4KeyDec = InitRC4((unsigned char*)passwd,strlen(passwd));
//	m_rc4KeyEnc = InitRC4((unsigned char*)passwd,strlen(passwd));
}

void ProtocolThread::connectServer()
{
	TVSocketHolder* th = TVSocketHolder::getInstance();
	if (th->isConnected() || th->isConnecting()){
		return;
	}
	if (m_rc4KeyEnc){
		free(m_rc4KeyEnc);
		m_rc4KeyEnc = 0;
	}
	if (m_rc4KeyDec){
		free(m_rc4KeyDec);
		m_rc4KeyDec = 0;
	}
	th->start();
}

void ProtocolThread::reset(){
	if(m_rc4KeyEnc){
		free(m_rc4KeyEnc);
		m_rc4KeyEnc = 0;
	}
	if(m_rc4KeyDec){
		free(m_rc4KeyDec);
		m_rc4KeyDec = 0;
	}
	if(TVSocketHolder::getInstance()->isConnected()){
		selfDisconnect = true;
		TVSocketHolder::getInstance()->getSocket().Close(__FILE__,__LINE__);
		TVSocketHolder::getInstance()->setConnectionStatus(false);
	}
	clearQueueMsg();
	auto chatCallback = chatMsgFunc;
	SINGLE_HERO->clean();
	init();
	chatMsgFunc = chatCallback;
}

void ProtocolThread::reconnectServer()
{
	reset();
	connectServer();
}

void ProtocolThread::saveChangedEmail(const char*changedEmail)
{
	char buffer[256];
	char*emailName = (char *)"changed_email";
	snprintf(buffer,sizeof(buffer),"v%u_%s",m_serverUrlHash,emailName);
	log("save changed_email key:%s",buffer);
	cocos2d::CCUserDefault::getInstance()->setStringForKey((const char*)buffer,changedEmail);
}

std::string ProtocolThread::getChangedEmail()
{
	char buffer[256];
	char*email = "changed_email";
	snprintf(buffer,sizeof(buffer),"v%u_%s",m_serverUrlHash,email);
	log("get changed_email key:%s",buffer);
	return cocos2d::CCUserDefault::getInstance()->getStringForKey((const char*)buffer);
}

void ProtocolThread::savePassword(const char*password)
{
	if(password == 0 || !password[0]){
		return;
	}
	char buffer[256];
	char*passwd = "passwd";
	snprintf(buffer,sizeof(buffer),"v%u_%s",m_serverUrlHash,passwd);
	log("save pass key:%s - %s",buffer,password);
	char* hex = getEncryptedPasswordHex((char*)password);
	if(hex){
		cocos2d::CCUserDefault::getInstance()->setStringForKey((const char*)buffer, hex);
		free(hex);
	}
}

void ProtocolThread::saveEmail(const char*email)
{
	if(email == 0){
		return;
	}
	if(email[0] == 0){
		log("reset email");
	}
	char buffer[256];
	char*emailName = "email";
	snprintf(buffer,sizeof(buffer),"v%u_%s",m_serverUrlHash,emailName);
	log("save email key:%s -  %s",buffer,email);
	cocos2d::CCUserDefault::getInstance()->setStringForKey((const char*)buffer,email);
}

std::string ProtocolThread::getSavedPassword()
{
	char buffer[256];
	char*passwd = "passwd";
	snprintf(buffer,sizeof(buffer),"v%u_%s",m_serverUrlHash,passwd);
	log("get pass key:%s",buffer);
	std::string hex = cocos2d::CCUserDefault::getInstance()->getStringForKey((const char*)buffer);
	char*password = 0;
	if(hex.length() > 0){
		password = getStringFromEncryptedHex((char*)hex.c_str());
	}
	std::string str;
	if(password){
		str = password;
		free(password);
	}
	return str;
}

std::string ProtocolThread::getSavedEmail()
{
	char buffer[256];
	char*email = "email";
	snprintf(buffer,sizeof(buffer),"v%u_%s",m_serverUrlHash,email);
	log("get email key:%s",buffer);
	return cocos2d::CCUserDefault::getInstance()->getStringForKey((const char*)buffer);
}

std::string ProtocolThread::getDefaultUserName()
{
	if(mUserSession){
		//std::string();
		char name[32];
		sprintf(name,"Hero_%d",mUserSession->getUserId());
		return std::string(name);
	}else{
		return std::string("");
	}
}

void ProtocolThread::getFleetCargo(UILoadingIndicator*loading){
	GetFleetCargo message = GET_FLEET_CARGO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetFleetCargo);
	}
}

void ProtocolThread::getGoodsForShip(int shipId,UILoadingIndicator*loading){
	GetGoodsForShip message = GET_GOODS_FOR_SHIP__INIT;
	message.shipid = shipId;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetGoodsForShip);
	}
}

void ProtocolThread::exchangeGoodsForShips(int firstShipId,int secendShipId,int*firstShipItemIds,int*firstShipItemAmounts,
										   int*secondShipItemIds,int*secondShipItemAmounts,int firstCount,int sencondCount,UILoadingIndicator*loading){
	ExchangeGoodsForShips message = EXCHANGE_GOODS_FOR_SHIPS__INIT;
	message.firestshipid = firstShipId;
	message.secondshipid = secendShipId;
	message.firstshipgoodsids = firstShipItemIds;
	message.secondshipgoodsids = secondShipItemIds;
	message.firstshipgoodsamounts = firstShipItemAmounts;
	message.secondshipgoodsamounts = secondShipItemAmounts;
	message.n_firstshipgoodsamounts = message.n_firstshipgoodsids = firstCount;
	message.n_secondshipgoodsamounts = message.n_secondshipgoodsids = sencondCount;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ExchangeGoodsForShips);
	}
}

void ProtocolThread::getMailList(UILoadingIndicator*loading){
	GetMailList message = GET_MAIL_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetMailList);
	}
}

void ProtocolThread::checkMailBox(){
	if(isWaitingResponse || gamePaused)
		return;
	CheckMailBox message = CHECK_MAIL_BOX__INIT;
	if(mUserSession){
		gettimeofday(&lastCheckMailTime, nullptr);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_CheckMailBox);
	}
}

/*
int32_t userid;
int32_t sessionid;
char *receivername;
int32_t coceral;
char *title;
char *message;
int32_t attachid;
int32_t attachtype;
int32_t attachamount;
int32_t goodnum;
int32_t coinnum;
*/

void ProtocolThread::sendMail(char *receiverName,char *title,char *body,int32_t attachid,int32_t attachamount,int32_t attachtype,int32_t goldnum,int64_t coinnum,UILoadingIndicator*loading){
	SendMail message = SEND_MAIL__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.receivername = receiverName;
		message.title = title;
		message.message = body;
		message.attachid = attachid;
		message.attachamount = attachamount;
		message.attachtype = attachtype;
		message.goldnum = goldnum;
		message.coinnum = coinnum;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_SendMail);
	}
}

//1 mark as read, 2 move to dustbin , 3 delete permanently ,4 restore,5 fetch attachment
void ProtocolThread::markMailStatus(int* mailIds,int n_mails,int action,UILoadingIndicator*loading){
	MarkMailStatus message = MARK_MAIL_STATUS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.n_mailids = n_mails;
		message.mailids = mailIds;
		message.action = action;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_MarkMailStatus);
	}
}

void ProtocolThread::moveMailToDustbin(int* mailIds,int n_mails,UILoadingIndicator*loading){
	markMailStatus(mailIds,n_mails,2,loading);
}
void ProtocolThread::deleteMail(int* mailIds,int n_mails,UILoadingIndicator*loading){
	markMailStatus(mailIds,n_mails,3,loading);
}
void ProtocolThread::restoreMailFromDustbin(int* mailIds,int n_mails,UILoadingIndicator*loading){
	markMailStatus(mailIds,n_mails,4,loading);
}
void ProtocolThread::markMailAsRead(int* mailIds,int n_mails,UILoadingIndicator*loading){
	markMailStatus(mailIds,n_mails,1,loading);
}
void ProtocolThread::fetchAttachmentFromMail(int* mailIds,int n_mails,UILoadingIndicator*loading){
	markMailStatus(mailIds,n_mails,5,loading);
}

void ProtocolThread::getPersonalItems(int itemType,int isTrade,UILoadingIndicator*loading){
	GetPersonalItem message = GET_PERSONAL_ITEM__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		message.fortrade = isTrade;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetPersonalItem);
	}
}

void ProtocolThread::getItemsToSell(int itemType,int forTrade,UILoadingIndicator*loading){
	GetItemsToSell message = GET_ITEMS_TO_SELL__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		message.fortrade = forTrade;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetItemsToSell);
	}
}

void ProtocolThread::getItemsBeingSold(int itemType,UILoadingIndicator*loading){
	GetItemsBeingSold message = GET_ITEMS_BEING_SOLD__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetItemsBeingSold);
	}
}

void ProtocolThread::getLastPrice(int itemType,int*ids,int n_ids,UILoadingIndicator*loading){
	GetLastPrice message = GET_LAST_PRICE__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetLastPrice);
	}
}

void ProtocolThread::getOrderList(int isSell,UILoadingIndicator*loading){
	GetOrderList message = GET_ORDER_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.issell = isSell;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetOrderList);
	}
}

void ProtocolThread::buyItem(int itemType,int itemId,float averagePrice,int* shipIds,int* counts,int shipNum,int optionalNum,int characterItemId,UILoadingIndicator*loading){
	BuyItem message = BUY_ITEM__INIT;
	if(mUserSession && shipNum > 0 ){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		message.count = 0;
		message.itemid = itemId;
		message.averageprice = averagePrice;
		message.characteritemid = characterItemId;
		message.optionalnum = optionalNum;

		if(itemType == 1){
			message.n_shipcount = 0;
			message.shipcount = nullptr;
			message.count = shipNum;
			sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_BuyItem);
		}else if(itemType == 0){
			message.n_shipcount = shipNum;
			message.shipcount = new BuyItemShipDetail *[shipNum];
			for(int i=0;i<shipNum;i++){
				message.count += counts[i];
				message.shipcount[i] = new BuyItemShipDetail;
				buy_item_ship_detail__init(message.shipcount[i]);
				message.shipcount[i]->shipid = shipIds[i];
				message.shipcount[i]->count = counts[i];
			}

			sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_BuyItem);

			for(int i=0;i<shipNum;i++){
				buy_item_ship_detail__free_unpacked(message.shipcount[i],0);
			}
			delete[] message.shipcount;
		}else{
			message.count = shipNum;
			sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_BuyItem);
		}
		
	}
}

void ProtocolThread::sellItem(int itemType,int itemId,int count,int optionalNum,int characterItemId,float averagePrice,int npcOnly,UILoadingIndicator*loading){
	SellItem message = SELL_ITEM__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		message.itemid = itemId;
		message.count = count;
		message.characteritemid = characterItemId;
		message.averageprice = averagePrice;
		message.optionalnum = optionalNum;
		message.npconly = npcOnly;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_SellItem);
	}
}
// isbid : 0 = buy, 1 = sell;
void ProtocolThread::createOrder(int itemType,int itemId,int count,int maxPrice,int expDays,int optionalNum,int characterItemId,int currency,int isbid,UILoadingIndicator*loading){
	CreateOrder message = CREATE_ORDER__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		message.itemid = itemId;
		message.count = count;
		message.price = maxPrice;
		message.currencytype = currency;
		message.expdays = expDays;
		message.isbid = isbid;
		message.characteritemid = characterItemId;
		message.optionalnum = optionalNum;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_CreateOrder);
	}
}

void ProtocolThread::searchTradeItems(int*types,int*ids,int n,UILoadingIndicator*loading){
	SearchTradeItems message = SEARCH_TRADE_ITEMS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtypes = types;
		message.itemids = ids;
		message.n_itemids = message.n_itemtypes = n;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_SearchTradeItems);
	}
}

void ProtocolThread::getPriceData(int itemType,int itemId,int characterItemId,int count,int isSell,UILoadingIndicator*loading){
	GetPriceData message = GET_PRICE_DATA__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		message.itemid = itemId;
		message.askedcount = count;
		message.issell = isSell;
		message.characteritemid = characterItemId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetPriceData);
	}
}

void ProtocolThread::cancelOrder(int itemType,int itemId,char*orderId,UILoadingIndicator*loading){
	CancelOrder message = CANCEL_ORDER__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		message.itemid = itemId;
		message.orderid = orderId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_CancelOrder);
	}
}

void ProtocolThread::fetchAllFromWarehouse(UILoadingIndicator*loading){
	WarehouseOpration message = WAREHOUSE_OPRATION__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.actioncode = 2;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_WarehouseOpration);
	}
}

void ProtocolThread::fetchItemFromWarehouse(int id,UILoadingIndicator*loading){
	WarehouseOpration message = WAREHOUSE_OPRATION__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.id =id;
		message.actioncode = 0;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_WarehouseOpration);
	}
}

void ProtocolThread::deleteItemFromWarehouse(int id,long count,UILoadingIndicator*loading){
	WarehouseOpration message = WAREHOUSE_OPRATION__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.id= id;
		message.actioncode = 1;
		message.count = count;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_WarehouseOpration);
	}
}

void ProtocolThread::getWarehouseItems(UILoadingIndicator*loading){
	GetWarehouseItems message = GET_WAREHOUSE_ITEMS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetWarehouseItems);
	}
}

void ProtocolThread::getBarInfo(int refresh,UILoadingIndicator*loading){
	GetBarInfo message = GET_BAR_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.refreshcaptain = refresh;
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetBarInfo);
	}
}


void ProtocolThread::barConsume(int id,int actioncode,UILoadingIndicator*loading){
	BarConsume message = BAR_CONSUME__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.actioncode = actioncode;
		message.itemid = id;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_BarConsume);
	}
}

void ProtocolThread::getAvailCrewNum(UILoadingIndicator*loading){
	GetAvailCrewNum message = GET_AVAIL_CREW_NUM__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetAvailCrewNum);
	}
}

void ProtocolThread::getCrew(int count, UILoadingIndicator*loading, int fasthire){
	GetCrew message = GET_CREW__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.count = count;
		message.fasthire = fasthire;
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetCrew);
	}
}

void ProtocolThread::treatBargirl(UILoadingIndicator*loading){
	TreatBargirl message = TREAT_BARGIRL__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_TreatBargirl);
	}
}

void ProtocolThread::giveBargirlGift(int itemId,int itemType,UILoadingIndicator*loading){
	GiveBargirlGift message = GIVE_BARGIRL_GIFT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.itemtype = itemType;
		message.itemid = itemId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GiveBargirlGift);
	}
}

void ProtocolThread::getTaskList(int refresh,UILoadingIndicator*loading){
	GetTaskList message = GET_TASK_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.refresh = refresh;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetTaskList);
	}
}

void ProtocolThread::getIntelligence(UILoadingIndicator*loading){
	GetIntelligence message = GET_INTELLIGENCE__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetIntelligence);
	}
}

void ProtocolThread::getCaptain(int index,UILoadingIndicator*loading){
	GetCaptain message = GET_CAPTAIN__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.index = index;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetCaptain);
	}	
}


void ProtocolThread::handleTask(int id,int actionCode,UILoadingIndicator*loading){
	HandleTask message = HANDLE_TASK__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.taskid = id;
		message.actioncode = actionCode;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_HandleTask);
	}	
}


void ProtocolThread::getCurrentCityData(UILoadingIndicator*loading){
	GetCurrentCityData message = GET_CURRENT_CITY_DATA__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetCurrentCityData);
	}	
}

void ProtocolThread::getShipList(UILoadingIndicator*loading){
	GetShipList message = GET_SHIP_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetShipList);
	}	
}


void ProtocolThread::repairShipMaxHP(int shipId,UILoadingIndicator*loading){
	RepairShip message = REPAIR_SHIP__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipid = shipId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_RepairShip);
	}	
}


void ProtocolThread::repairAllShips(int type,UILoadingIndicator*loading){
	RepairAllShips message = REPAIR_ALL_SHIPS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.type = type;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_RepairAllShips);
	}	
}


void ProtocolThread::fleetFormation(int*shipIds, int n, FleetCaptain** captain, int nCaptain, int*deletedShipIds, int nDelShips, int *renameShipIds, char**renameShipNames, int nRenameShips, int n_catchshipid, int *catchshipid, UILoadingIndicator*loading){
	FleetFormation message = FLEET_FORMATION__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipids = shipIds;
		message.n_shipids = n;
		if (nCaptain > 0)
		{
			message.captainids = new FleetCaptain *[nCaptain];
			for (int i = 0; i< nCaptain; i++)
			{
				message.captainids[i] = new FleetCaptain;
				fleet_captain__init(message.captainids[i]);
				message.captainids[i]->captainid = captain[i]->captainid;
				message.captainids[i]->iscaptain = captain[i]->iscaptain;
			}
		}

		message.n_captainids = nCaptain;
		message.deletedshipids = deletedShipIds;
		message.n_deletedshipids = nDelShips;
		message.n_catchshipid = n_catchshipid;
		if(nRenameShips > 0 && renameShipIds && renameShipNames){
			ShipName **shipNames = (ShipName **)malloc(sizeof(ShipName *)*nRenameShips);
			for(int i =0;i<nRenameShips;i++){
				ShipName *shipName = (ShipName *)malloc(sizeof(ShipName ));
				ship_name__init(shipName);
				shipName->shipid = renameShipIds[i];
				shipName->utf8name = renameShipNames[i];
				shipNames[i] = shipName;
			}
			message.shipnames = shipNames;
			message.n_shipnames = nRenameShips;
		}
		if (n_catchshipid>0){
			message.catchshipid = new int[n_catchshipid];
			for (int i = 0; i<n_catchshipid; i++){
				message.catchshipid[i] = catchshipid[i];
			}
		}

		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_FleetFormation);
		if(message.n_shipnames > 0){
			ShipName **shipNames = message.shipnames;
			for(int i =0;i<message.n_shipnames;i++){
				free(shipNames[i]);
			}
			free(shipNames);
		}
		if (n_catchshipid>0){
			delete[] message.catchshipid;
		}
	}	

	if (nCaptain > 0)
	{
		for (int i = 0; i< nCaptain; i++){
			delete message.captainids[i];
		}
		delete[] message.captainids;
	}
}


void ProtocolThread::parkShip(int shipId,int isUnpark,UILoadingIndicator*loading){
	ParkShip message = PARK_SHIP__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipid = shipId;
		message.isunpark = isUnpark;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ParkShip);
	}	
}
//type 0:ship  1:Equip          subType : 0
void ProtocolThread::getDrawings(int type,int subType,UILoadingIndicator*loading){
	GetDrawings message = GET_DRAWINGS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.subtype = subType;
		message.drawingtype = type;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetDrawings);
	}	
}


void ProtocolThread::buildShip(int drawingId,UILoadingIndicator*loading){
	BuildShip message = BUILD_SHIP__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.drawingid = drawingId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_BuildShip);
	}
}

void ProtocolThread::getBuildingShips(UILoadingIndicator*loading){
	GetBuildingShips message = GET_BUILDING_SHIPS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetBuildingShips);
	}
}
void ProtocolThread::finishBuilding(int shipManufactureId,UILoadingIndicator*loading){
	FinishBuilding message = FINISH_BUILDING__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipmanufactureid = shipManufactureId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_FinishBuilding);
	}
}

void ProtocolThread::getBuildingEquipment(int type,int subType,UILoadingIndicator*loading){
	GetBuildingEquipment message = GET_BUILDING_EQUIPMENT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.drawingtype = type;
		message.subtype = subType;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetBuildingEquipment);
	}
}

void ProtocolThread::cancelBuild(int shipManufactureId,UILoadingIndicator*loading){
	CancelBuild message = CANCEL_BUILD__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipmanufactureid = shipManufactureId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_CancelBuild);
	}
}


void ProtocolThread::getFleetAndDockShips(UILoadingIndicator*loading){
	GetFleetAndDockShips message = GET_FLEET_AND_DOCK_SHIPS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetFleetAndDockShips);
	}
}

void ProtocolThread::getReinforceShipList(UILoadingIndicator*loading){
	GetReinforceShipList message = GET_REINFORCE_SHIP_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetReinforceShipList);
	}
}


void ProtocolThread::reinforceShip(int shipid,int optional1,int type,int optionNum,UILoadingIndicator*loading){
	ReinforceShip message = REINFORCE_SHIP__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipid = shipid;
		message.itemid = optional1;
		message.itemtype = type;
		message.optionalnum = optionNum;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ReinforceShip);
	}
}
void ProtocolThread::rerollShip(int shipid,int optional1,int optional2,int optionNum,UILoadingIndicator*loading){
	RerollShip message = REROLL_SHIP__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipid = shipid;
		message.optionalitem1 = optional1;
		message.optionalitem2 = optional2;
		message.optinoalnum = optionNum;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_RerollShip);
	}
}
void ProtocolThread::getReinforceEquipList(UILoadingIndicator*loading){
	GetReinforceEquipList message = GET_REINFORCE_EQUIP_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetReinforceEquipList);
	}
}

void ProtocolThread::reinforceEquip(int equipmentId,int optionId,int optionNum,int shipId,int position,UILoadingIndicator*loading){
	ReinforceEquip message = REINFORCE_EQUIP__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.equipmentid = equipmentId;
		message.optionalitemid = optionId;
		message.optionalnum = optionNum;
		message.shipid = shipId;
		message.position = position;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ReinforceEquip);
	}
}
void ProtocolThread::rerollEquip(int equipmentId,int optionId,int optionNum,int shipId,int position,UILoadingIndicator*loading){
	RerollEquip message = REROLL_EQUIP__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.equipmentid = equipmentId;
		message.optionalitem = optionId;
		message.optionalnum = optionNum;
		message.shipid = shipId;
		message.position = position;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_RerollEquip);
	}
}


void ProtocolThread::equipShip(int shipId,int bowIconId,int bowGunId,int aftGunId,char*spinnakerIds,char*armorIds,char*cannonIds,UILoadingIndicator*loading){
	EquipShip message = EQUIP_SHIP__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipid = shipId;
		message.bowiconid = bowIconId;
		message.bowgunid = bowGunId;
		message.aftgunid = aftGunId;
		message.spinnakerids = spinnakerIds;
		message.armorids = armorIds;
		message.cannonids = cannonIds;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_EquipShip);
	}
}

void ProtocolThread::getEquipShipInfo(int shipid,UILoadingIndicator*loading){
	GetEquipShipInfo message = GET_EQUIP_SHIP_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipid = shipid;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetEquipShipInfo);
	}
}

void ProtocolThread::getEquipableItems(int type,int isForHero,UILoadingIndicator*loading){
	GetEquipableItems message = GET_EQUIPABLE_ITEMS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.type = type;
		message.isheroequip = isForHero;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetEquipableItems);
	}
}

void ProtocolThread::getVisitedCities(UILoadingIndicator*loading){
	GetVisitedCities message = GET_VISITED_CITIES__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetVisitedCities);
	}
}

void ProtocolThread::addSupply(int amountToAdd,UILoadingIndicator*loading){
	AddSupply message = ADD_SUPPLY__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.addamount = amountToAdd;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_AddSupply);
	}
}

bool ProtocolThread::getNextMapBlock(ShipRoutine*shipRoutine,int sailDays,UILoadingIndicator*loading){
	GetNextMapBlock message = GET_NEXT_MAP_BLOCK__INIT;
	if(isWaitingResponse){
		log("isWaitingResponse,ingnore");
		return false;
	}
	if(mUserSession){
		setLoadingRes(loading);
		message.roadmap = shipRoutine;
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.days = sailDays;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetNextMapBlock);
		//MainTaskManager::GetInstance()->reportSeaXY(currentX,MAP_WIDTH * MAP_CELL_WIDTH - currentY);
		if(shipRoutine){
			//ship_routine__free_unpacked(shipRoutine,0);
			for(auto i=0;i<shipRoutine->n_roads;i++){
				//log("-------------------------------------------------------------------------------upload (%d,%d)",shipRoutine->roads[i]->x,shipRoutine->roads[i]->y);
				free(shipRoutine->roads[i]);
			}
			if(shipRoutine->roads)
				free(shipRoutine->roads);
			free(shipRoutine);
		}
	}
	return true;
}

void ProtocolThread::leaveCity(UILoadingIndicator*loading){
	LeaveCity message = LEAVE_CITY__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_LeaveCity);
	}
}
/*
 * Call this api when you reach a point near city or return from a failed battle.
 * Params:
 * currentX: the x position of player, use -1 if return from failed battle
 * currentY: the y position of player, use -1 if return from failed battle
 * loading: the loading layer ref
 */
void ProtocolThread::reachCity(int currentX,int currentY,UILoadingIndicator*loading){
	ReachCity message = REACH_CITY__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.currentx = currentX;
		message.currenty = currentY;
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ReachCity);
	}
}

void ProtocolThread::triggerSeaEvent(int eventId,int propId,UILoadingIndicator*loading){
	TriggerSeaEvent message = TRIGGER_SEA_EVENT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.eventid = eventId;
		message.propid = propId;
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_TriggerSeaEvent);
	}
}

void ProtocolThread::finishSeaEvent(UILoadingIndicator*loading){
	FinishSeaEvent message = FINISH_SEA_EVENT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_FinishSeaEvent);
	}
}


void ProtocolThread::getPalaceInfo(UILoadingIndicator*loading){
	GetPalaceInfo message = GET_PALACE_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetPalaceInfo);
	}
}


void ProtocolThread::investCity(int64_t investNum,UILoadingIndicator*loading){
	InvestCity message = INVEST_CITY__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.investcoin = investNum;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_InvestCity);
	}
}


void ProtocolThread::getRankList(UILoadingIndicator*loading){
	GetRankList message = GET_RANK_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetRankList);
	}
}

void ProtocolThread::getVoteList(UILoadingIndicator*loading){
	GetVoteList message = GET_VOTE_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetVoteList);
	}
}


void ProtocolThread::voteMayor(int id,UILoadingIndicator*loading){
	VoteMayor message = VOTE_MAYOR__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.candidateid = id;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_VoteMayor);
	}
}


void ProtocolThread::changeDevDir(int newDevDirection,UILoadingIndicator*loading){
	ChangeDevDir message = CHANGE_DEV_DIR__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.newdevdirection = newDevDirection;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ChangeDevDir);
	}
}

void ProtocolThread::depositMoney(int64_t money,int isPersonal,UILoadingIndicator*loading){
	DepositMoney message = DEPOSIT_MONEY__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.money = money;
		message.ispersonal = isPersonal;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_DepositMoney);
	}
}

void ProtocolThread::withdrawMoney(int64_t money,int isPersonal,UILoadingIndicator*loading){
	WithdrawMoney message = WITHDRAW_MONEY__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.money = money;
		message.ispersonal = isPersonal;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_WithdrawMoney);
	}
}

void ProtocolThread::depositItem(int id,int iid,int type,int num,int isPersonal,UILoadingIndicator*loading){
	DepositItem message = DEPOSIT_ITEM__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.id = id;
		message.type = type;
		message.iid = iid;
		message.ispersonal = isPersonal;
		message.num = num;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_DepositItem);
	}
}

void ProtocolThread::withdrawItem(int id,int type,int num,int isPersonal,UILoadingIndicator*loading){
	WithdrawItem message = WITHDRAW_ITEM__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.id = id;
		message.type = type;
		message.ispersonal = isPersonal;
		message.num = num;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_WithdrawItem);
	}
}

void ProtocolThread::expandBank(int isPersonal,UILoadingIndicator*loading){
	ExpandBank message = EXPAND_BANK__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();

		message.ispersonal = isPersonal;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ExpandBank);
	}
}


void ProtocolThread::getBankInfo(int isPersonal,UILoadingIndicator*loading){
	GetBankInfo message = GET_BANK_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();

		message.ispersonal = isPersonal;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetBankInfo);
	}
}

void ProtocolThread::getBankLog(int isPersonal,UILoadingIndicator*loading){
	GetBankLog message = GET_BANK_LOG__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();

		message.ispersonal = isPersonal;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetBankLog);
	}
}

void ProtocolThread::setMainTaskStatus(char*shipName,int stage,UILoadingIndicator*loading){
	SetMainTaskStatus message = SET_MAIN_TASK_STATUS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipname = shipName;
		message.status = stage;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_SetMainTaskStatus);
	}
}

void ProtocolThread::setLoadingRes(UILoadingIndicator*loading){
	if(!isWaitingResponse){
		mLoadingLayer = loading;
	}else{
		mBackupLoadingLayer = loading;
		//__asm nop;
		//fix me/
	}
}

void ProtocolThread::engageInFight(int eventId,int forceFight,int64_t eventVersion,int fightType,int fightLevel,UILoadingIndicator*loading){
	EngageInFight message = ENGAGE_IN_FIGHT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.eventid = eventId;
		message.forcefight = forceFight;
		message.version = eventVersion;
		message.fighttype = fightType;
		message.fightlevel = fightLevel;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_EngageInFight);
	}
}

void ProtocolThread::notifyBattleLog(int fightType,UILoadingIndicator*loading){
	NotifyBattleLog message = NOTIFY_BATTLE_LOG__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.fighttype = fightType;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_NotifyBattleLog);
	}
}
void ProtocolThread::cancelFight(int type,EndFight*lastFightInfo,UILoadingIndicator*loading){
	CancelFight message = CANCEL_FIGHT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
// 		message.eventid = eventId;
 		message.type = type;
 		message.lastfightinfo = lastFightInfo;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_CancelFight);
	}
}
void ProtocolThread::forceFightStatus(UILoadingIndicator*loading){
	ForceFightStatus message = FORCE_FIGHT_STATUS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
// 		message.eventid = eventId;
// 		message.type = type;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ForceFightStatus);
	}
}

void ProtocolThread::endFight(int reason, int nShip, int*hpArray, int*sailNumArray, int* shipIdArry, PropsInfo**props, int nProp, int morale, int fightType, int remainSec, int attack_hurt, int attack_sailors, int n_catchshipids, int* catchshipids, int n_catchgoodsshipids, int *catchgoodsshipids, int usesec,BattleLog*log,UILoadingIndicator*loading){
	EndFight message = END_FIGHT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.morale = morale;
		message.reason = reason;
		message.fighttype = fightType;
		message.remainsec = remainSec;
		message.attack_hurt = attack_hurt;
		message.attack_sailors = attack_sailors;
		message.n_catchshipids = n_catchshipids;
		message.n_catchgoodsshipids = n_catchgoodsshipids;
		message.usesec = usesec;
		//message.
		if(nShip > 5)
			return;
	
		message.n_myships = nShip;
		message.n_usedprops = nProp;
		if(nProp > 0){
			message.usedprops = new PropsInfo *[nProp];
			for(int i =0;i<nProp;i++){
				message.usedprops[i] = new PropsInfo;
				props_info__init(message.usedprops[i]);
				message.usedprops[i]->id = props[i]->id;
				message.usedprops[i]->num = props[i]->num;
				message.usedprops[i]->propid = props[i]->propid;
			}
		}
		if(nShip>0){
			message.myships = new EndFightShipInfo *[nShip];
			for(int i=0;i<nShip;i++){
				message.myships[i] = new EndFightShipInfo;
				end_fight_ship_info__init(message.myships[i]);
				message.myships[i]->hp = hpArray[i];
				message.myships[i]->sailornum = sailNumArray[i];
				message.myships[i]->shipid = shipIdArry[i];
			}
		}
		if (n_catchshipids>0){
			message.catchshipids = new int[n_catchshipids];
			for (int i = 0; i<n_catchshipids; i++){
				message.catchshipids[i] = catchshipids[i];
			}
		}

		if (n_catchgoodsshipids>0){
			message.catchgoodsshipids = new int[n_catchgoodsshipids];
			for (int i = 0; i<n_catchgoodsshipids; i++){
				message.catchgoodsshipids[i] = catchgoodsshipids[i];
			}
		}

		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_EndFight);
		if(nProp > 0){
			for(int i =0;i<nProp;i++){
				delete message.usedprops[i];
			}
			delete [] message.usedprops;
		}

		if(nShip>0){
			for(int i =0;i<nShip;i++){
				delete message.myships[i];
			}
			delete [] message.myships;
		}

		if (n_catchshipids>0){
			delete[] message.catchshipids;
		}
		if (n_catchgoodsshipids>0){
			delete[] message.catchgoodsshipids;
		}
	}
}

void ProtocolThread::startAutoFight(UILoadingIndicator*loading){
	StartAutoFight message = START_AUTO_FIGHT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_StartAutoFight);
	}
}

void ProtocolThread::getSkillsDetails(UILoadingIndicator*loading){
	GetSkillsDetails message = GET_SKILLS_DETAILS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetSkillsDetails);
	}
}


void ProtocolThread::addSkillPoint(int skillId,int points,UILoadingIndicator*loading){
	AddSkillPoint message = ADD_SKILL_POINT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.points = points;
		message.skillid = skillId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_AddSkillPoint);
	}
}


void ProtocolThread::resetSkillPoints(UILoadingIndicator*loading){
	ResetSkillPoints message = RESET_SKILL_POINTS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ResetSkillPoints);
	}
}

void ProtocolThread::getPersonalInfo(UILoadingIndicator*loading){
	GetPersonalInfo message = GET_PERSONAL_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetPersonalInfo);
	}
}

void ProtocolThread::setBioInfo(char*info,UILoadingIndicator*loading){
	SetBioInfo message = SET_BIO_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.bio = info;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_SetBioInfo);
	}
}

void ProtocolThread::getGuildList(UILoadingIndicator*loading){
	GetGuildList message = GET_GUILD_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetGuildList);
	}
}

void ProtocolThread::getMyGuildDetails(UILoadingIndicator*loading){
	GetMyGuildDetails message = GET_MY_GUILD_DETAILS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetMyGuildDetails);
	}
}

void ProtocolThread::getApplyInfo(UILoadingIndicator*loading){
	GetApplyInfo message = GET_APPLY_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetApplyInfo);
	}
}

void ProtocolThread::createNewGuild(char *guildName,int iconIdx,char*intro, UILoadingIndicator*loading){
	CreateNewGuild message = CREATE_NEW_GUILD__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.guildname = guildName;
		message.guildiconidx = iconIdx;
		message.guildintro = intro;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_CreateNewGuild);
	}
}

void ProtocolThread::joinGuildRequest(int guildId, UILoadingIndicator*loading){
	JoinGuildRequest message = JOIN_GUILD_REQUEST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.guildid = guildId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_JoinGuildRequest);
	}
}

void ProtocolThread::getGuildDetails(int guildId, UILoadingIndicator*loading){
	GetGuildDetails message = GET_GUILD_DETAILS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.guildid = guildId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetGuildDetails);
	}
}

void ProtocolThread::changeGuildIntro(char *intro, UILoadingIndicator*loading){
	ChangeGuildIntro message = CHANGE_GUILD_INTRO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.newintro = intro;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ChangeGuildIntro);
	}
}


void ProtocolThread::expandGuildCapacity(UILoadingIndicator*loading){
	ExpandGuildCapacity message = EXPAND_GUILD_CAPACITY__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ExpandGuildCapacity);
	}
}

void ProtocolThread::getGuildMemberPermission(UILoadingIndicator*loading){
	GetGuildMemberPermission message = GET_GUILD_MEMBER_PERMISSION__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetGuildMemberPermission);
	}
}

void ProtocolThread::setGuildMemberPermission(SetGuildPermissionDefine**defines,int n_define,UILoadingIndicator*loading){
	SetGuildMemberPermission message = SET_GUILD_MEMBER_PERMISSION__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.members = defines;
		message.n_members = n_define;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_SetGuildMemberPermission);
	}
}

void ProtocolThread::getHiredCaptains(UILoadingIndicator*loading){
	GetHiredCaptains message = GET_HIRED_CAPTAINS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetHiredCaptains);
	}
}

void ProtocolThread::dealWithGuildJoin(int accept,int application_id, int cid, int guild_id, UILoadingIndicator*loading){
	DealWithGuildJoin message = DEAL_WITH_GUILD_JOIN__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.join_id =application_id;
		message.accept =accept;
		message.guild_id = guild_id;
		message.character_id = cid;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_DealWithGuildJoin);
	}
}
void ProtocolThread::removeGuildMember(int guild_id, int member_character_id,
		UILoadingIndicator*loading) {
	RemoveGuildMember message = REMOVE_GUILD_MEMBER__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.guildid=guild_id;
		message.membercharacterid = member_character_id;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_RemoveGuildMember);
	}
}
void ProtocolThread::exitFromGuild(UILoadingIndicator*loading) {
	ExitFromGuild message = EXIT_FROM_GUILD__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ExitFromGuild);
	}
}
void ProtocolThread::dismissGuild(UILoadingIndicator*loading) {
	DismissGuild message = DISMISS_GUILD__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_DismissGuild);
	}
}
void ProtocolThread::getSearchGuildList(char *keyword, UILoadingIndicator*loading) {
	GetSearchGuildList message = GET_SEARCH_GUILD_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.keyword = keyword;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetSearchGuildList);
	}
}


void ProtocolThread::refreshCaptainList(int useGold,UILoadingIndicator*loading){
	RefreshCaptainList message = REFRESH_CAPTAIN_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.usegold = useGold;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_RefreshCaptainList);
	}
}
void ProtocolThread::changeGuildAnnouncement(int guildId, char *newAnnouncement, UILoadingIndicator*loading){
	ChangeGuildAnnouncement message = CHANGE_GUILD_ANNOUNCEMENT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.newannouncement = newAnnouncement;
		message.guildid = guildId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ChangeGuildAnnouncement);
	}
}

void ProtocolThread::getFriendsList(UILoadingIndicator*loading){
	GetFriendsList message = GET_FRIENDS_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetFriendsList);
	}
}

void ProtocolThread::getBlackList(UILoadingIndicator*loading){
	GetBlackList message = GET_BLACK_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetBlackList);
	}
}

void ProtocolThread::friendsOperation(int characterId,int actionCode,UILoadingIndicator*loading){ ////0 add,1,delete,2,blacklist,3 accept friends req,4 refuse friends req
	FriendsOperation message = FRIENDS_OPERATION__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.actioncode = actionCode;
		message.characterid = characterId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_FriendsOperation);
	}
}

void ProtocolThread::getFriendsReqList(UILoadingIndicator*loading){
	GetFriendsReqList message = GET_FRIENDS_REQ_LIST__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetFriendsReqList);
	}

}

void ProtocolThread::fireCaptain(long captainId,UILoadingIndicator*loading){
	FireCaptain message = FIRE_CAPTAIN__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.captainid = captainId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_FireCaptain);
	}
}

void ProtocolThread::destoryItems(int uniqueId,long count,int type,UILoadingIndicator*loading){
	DestoryItems message = DESTORY_ITEMS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.uniqueid = uniqueId;
		message.count = count;
		message.itemtype = type;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_DestoryItems);
	}
}

void ProtocolThread::searchUserByName(char*name,UILoadingIndicator*loading){
	SearchUserByName message = SEARCH_USER_BY_NAME__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.heroname = name;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_SearchUserByName);
	}
}

void ProtocolThread::takeAllAttachment(int mailId,UILoadingIndicator*loading){
	TakeAllAttachment message = TAKE_ALL_ATTACHMENT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.mailid = mailId;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_TakeAllAttachment);
	}
}

void ProtocolThread::getItemsDetailInfo(int itemId,int itemType,int uniqueId,UILoadingIndicator*loading){
	GetItemsDetailInfo message = GET_ITEMS_DETAIL_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.uniqueid = uniqueId;
		message.itemid = itemId;
		message.itemtype = itemType;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetItemsDetailInfo);
	}
}

void ProtocolThread::getDevGoodsInfo(UILoadingIndicator*loading){
	GetDevGoodsInfo message = GET_DEV_GOODS_INFO__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetDevGoodsInfo);
	}
}

void ProtocolThread::provideDevGoods(int goodsIndex,int count,UILoadingIndicator*loading){
	ProvideDevGoods message = PROVIDE_DEV_GOODS__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.goodsindex = goodsIndex;
		message.count = count;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_ProvideDevGoods);
	}
}

void ProtocolThread::getDevGoodsCount(int goodsIndex,UILoadingIndicator*loading){
	GetDevGoodsCount message = GET_DEV_GOODS_COUNT__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.goodsindex = goodsIndex;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetDevGoodsCount);
	}
}

void ProtocolThread::getFriendValue(int forCity,UILoadingIndicator*loading){
	GetFriendValue message = GET_FRIEND_VALUE__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.forcity = forCity;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetFriendValue);
	}
}

void ProtocolThread::getUserInfoById(int cid,UILoadingIndicator*loading){
	GetUserInfoById message = GET_USER_INFO_BY_ID__INIT;
	if(mUserSession){
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.cid = cid;
		sendMessage((struct ProtobufCMessage*)&message,PROTO_TYPE_GetUserInfoById);
	}
}

void ProtocolThread::testCreateNewGuild(UILoadingIndicator*loading) {
	TestCreateNewGuild message = TEST_CREATE_NEW_GUILD__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_TestCreateNewGuild);
	}
}


void ProtocolThread::inviteUserToGuild(int cid, UILoadingIndicator*loading) {
	InviteUserToGuild message = INVITE_USER_TO_GUILD__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_InviteUserToGuild);
	}
}


void ProtocolThread::getInvitationList(UILoadingIndicator*loading) {
	GetInvitationList message = GET_INVITATION_LIST__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_GetInvitationList);
	}
}

void ProtocolThread::dealWithInvitation(int invitationId, int accept,UILoadingIndicator*loading) {
	DealWithInvitation message = DEAL_WITH_INVITATION__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.accept = accept;
		message.invitationid = invitationId;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_DealWithInvitation);
	}
}

void ProtocolThread::getVTicketMarketItems(int type,UILoadingIndicator*loading){
	GetVTicketMarketItems message = GET_VTICKET_MARKET_ITEMS__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.type = type;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_GetVTicketMarketItems);
	}
}
void ProtocolThread::buyVTicketMarketItem(int type,int uniqueId,int count,UILoadingIndicator*loading){
	BuyVTicketMarketItem message = BUY_VTICKET_MARKET_ITEM__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.type = type;
		message.uniqueid = uniqueId;
		message.count = count;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_BuyVTicketMarketItem);
	}
}

void ProtocolThread::getCurrentInvestData(UILoadingIndicator*loading){
	GetCurrentInvestData message = GET_CURRENT_INVEST_DATA__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_GetCurrentInvestData);
	}
}

void ProtocolThread::getUserTasks(UILoadingIndicator*loading){
	GetUserTasks message = GET_USER_TASKS__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_GetUserTasks);
	}
}

void ProtocolThread::useSudanMask(int64_t id,UILoadingIndicator*loading){
	UseSudanMask message = USE_SUDAN_MASK__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.sudanmaskid = id;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_UseSudanMask);
	}
}

void ProtocolThread::equipHero(int headId,int clothId,int weaponId,int accessId,int shoeId,UILoadingIndicator*loading){
	EquipHero message = EQUIP_HERO__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.headid = headId;
		message.clothesid = clothId;
		message.weaponid = weaponId;
		message.accessid = accessId;
		message.shoeid = shoeId;
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_EquipHero);
	}
}

void ProtocolThread::getHeroEquip(int cid ,UILoadingIndicator*loading){
	GetHeroEquip message = GET_HERO_EQUIP__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.has_character_id = false;
		if (cid != 0)
		{
			message.has_character_id = true;
			message.character_id = cid;
		}
	
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_GetHeroEquip);
	}
}

void ProtocolThread::buyCityLicense(UILoadingIndicator*loading){
	BuyCityLicense message = BUY_CITY_LICENSE__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_BuyCityLicense);
	}
}

void ProtocolThread::deleteUser(int deleteType,UILoadingIndicator*loading){
	DeleteUser message = DELETE_USER__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.deleteaccount = deleteType;
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_DeleteUser);
	}
}
void ProtocolThread::changePassword(char*password,UILoadingIndicator*loading){
	ChangePassword message = CHANGE_PASSWORD__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.languagecode = Utils::getLanguage();
		message.apiurl = (char*)ServerManager::CurSelectedServer()->GetServerAddr().c_str();
		message.password = (char*)password;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_ChangePassword);
	}
}

void ProtocolThread::selectHeroPositiveSkill(int slotIndex, int skillId,UILoadingIndicator*loading) {
	SelectHeroPositiveSkill message = SELECT_HERO_POSITIVE_SKILL__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.slotindex = slotIndex;
		message.skillid = skillId;
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_SelectHeroPositiveSkill);
	}
}

void ProtocolThread::getHeroPositiveSkillList(UILoadingIndicator*loading) {
	GetHeroPositiveSkillList message = GET_HERO_POSITIVE_SKILL_LIST__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_GetHeroPositiveSkillList);
	}
}

void ProtocolThread::getSelectedPositiveSkills(UILoadingIndicator*loading) {
	GetSelectedPositiveSkills message = GET_SELECTED_POSITIVE_SKILLS__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_GetSelectedPositiveSkills);
	}
}

void ProtocolThread::getSailInfo(UILoadingIndicator*loading){
	GetSailInfo message = GET_SAIL_INFO__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetSailInfo);
	}
}
void ProtocolThread::getLeaderboard(int byCoins,UILoadingIndicator*loading){
	GetLeaderboard message = GET_LEADERBOARD__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.bycoins = byCoins;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetLeaderboard);
	}
}

void ProtocolThread::getCityStatus(int cityId,UILoadingIndicator*loading){
	GetCityStatus message = GET_CITY_STATUS__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.cityid = cityId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetCityStatus);
	}
}

void ProtocolThread::getDiscoveredCities(UILoadingIndicator*loading){
	GetDiscoveredCities message = GET_DISCOVERED_CITIES__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetDiscoveredCities);
	}
}

void ProtocolThread::addDiscoveredCity(int cityId,UILoadingIndicator*loading){
	AddDiscoveredCity message = ADD_DISCOVERED_CITY__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.cityid = cityId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_AddDiscoveredCity);
	}
}

void ProtocolThread::getCityProduces(UILoadingIndicator*loading){
	GetCityProduces message = GET_CITY_PRODUCES__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetCityProduces);
	}
}

void ProtocolThread::getCityDemands(UILoadingIndicator*loading){
	GetCityDemands message = GET_CITY_DEMANDS__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetCityDemands);
	}
}

void ProtocolThread::getMainTask(UILoadingIndicator*loading) {
	GetMainTask message = GET_MAIN_TASK__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetMainTask);
	}
}

void ProtocolThread::completeMainTask(int partId,UILoadingIndicator*loading) {
	CompleteMainTask message = COMPLETE_MAIN_TASK__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.partid = partId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_CompleteMainTask);
	}
}

void ProtocolThread::getMainTaskReward(UILoadingIndicator*loading) {
	GetMainTaskReward message = GET_MAIN_TASK_REWARD__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetMainTaskReward);
	}
}

void ProtocolThread::getEventTaskList(UILoadingIndicator*loading){
	GetEventTaskList message = GET_EVENT_TASK_LIST__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetEventTaskList);
	}
}

void ProtocolThread::handleEventTask(int eventTaskId,int actionCode,UILoadingIndicator*loading){
	HandleEventTask message = HANDLE_EVENT_TASK__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.taskid = eventTaskId;
		message.actioncode = actionCode;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_HandleEventTask);
	}
}

void ProtocolThread::getNpcScore(int*ids,int num,UILoadingIndicator*loading)
{
	GetNpcScore message = GET_NPC_SCORE__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.n_npcids = num;
		if(num > 0){
			message.npcids = new int[num];
			memcpy(message.npcids,ids,sizeof(int)*num);
		}
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetNpcScore);
		if(num > 0){
			delete [] message.npcids;
		}
	}
}

void ProtocolThread::getServerInfo(){
	//GetServerInfo message = GET_SERVER_INFO__INIT;
	//sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetServerInfo);
}

void ProtocolThread::pingServer(){
	PingServer message = PING_SERVER__INIT;
	if (mUserSession) {
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_PingServer);
		log("auto ping for lua");
		gettimeofday(&lastPingTime, nullptr);
	}
}

void ProtocolThread::expandPackageSize(UILoadingIndicator*loading)
{
	ExpandPackageSize message = EXPAND_PACKAGE_SIZE__INIT;
	if (mUserSession) {
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_ExpandPackageSize);
	}
}

void ProtocolThread::getPretaskItems(UILoadingIndicator*loading) {
	GetPretaskItems message = GET_PRETASK_ITEMS__INIT;
	if (mUserSession) {
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetPretaskItems);
	}
}

void ProtocolThread::getOnsaleItems(UILoadingIndicator*loading){
	GetOnsaleItems message = GET_ONSALE_ITEMS__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetOnsaleItems);
	}
}

void ProtocolThread::buyOnsaleItems(int uniqueid,int type,UILoadingIndicator*loading){
	BuyOnsaleItems message = BUY_ONSALE_ITEMS__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.uniqueid = uniqueid;
		message.type = type;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_BuyOnsaleItems);
	}
}

void ProtocolThread::sendCustomServiceInfo(int type,char*info,char*email,UILoadingIndicator*loading){
	SendCustomServiceInfo message = SEND_CUSTOM_SERVICE_INFO__INIT;
	if (1) {
		setLoadingRes(loading);
		if (mUserSession) {
			message.sessionid = mUserSession->getSessionId();
			message.userid = mUserSession->getUserId();
		}
		message.info = info;
		message.type = type;
		message.email = email;
		//： app_version：%s,app_locale:%s,os:%s,os version:%s,os locale:%s,devcie_maker:%s,device_model:%s"
		std::string metaData = Utils::GetDeviceInfo();
		message.metadata = (char*)metaData.c_str();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_SendCustomServiceInfo);
	}
}

void ProtocolThread::getIABItem(char*iabItemName,UILoadingIndicator*loading){
	GetIABItem message = GET_IABITEM__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.name = iabItemName;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetIABItem);
	}
}

void ProtocolThread::setGoogleAccountAndToken(const char*email,const char*token){
	m_googleAccount = email;
	m_googleToken = token;
	if(m_isWaitingForGoogleToken){
		log("ProtocolThread::setGoogleAccountAndToken tokenLogin email:%s",email);
		m_isWaitingForGoogleToken = false;
		tokenLogin();
	}
}

void ProtocolThread::changeEmail(char*newEmail,UILoadingIndicator*loading)
{
	ChangeEmail message = CHANGE_EMAIL__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.newemail = newEmail;
		message.languagecode = Utils::getLanguage();
		message.apiurl = (char*)ServerManager::CurSelectedServer()->GetServerAddr().c_str();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_ChangeEmail);
	}
}

void ProtocolThread::tokenLogin(UILoadingIndicator*loading)
{
	TokenLogin message = TOKEN_LOGIN__INIT;
	if (1) {
		if(loading)setLoadingRes(loading);
		message.email = (char*)m_googleAccount.c_str();
		message.token = (char*)m_googleToken.c_str();

		message.networktype = (char*)Utils::GetNetWorkString().c_str();
		char passwd[13];
		passwd[12] = 0;
		int pass_len = 12;
		getRandomString(passwd, pass_len);
		int outLen = 0;
		message.passwd1.data = RSAEncrypt((unsigned char*)passwd,strlen(passwd),&outLen);
		message.passwd1.len = outLen;
		//savePendigEmail(copyCString(message.email));
		savePendingPassword(copyCString(passwd));//used as rc4 key
		message.languagecode = Utils::getLanguage();
		message.apiurl = (char*)ServerManager::CurSelectedServer()->GetServerAddr().c_str();
		message.refcode = (char*)Utils::GetFaceBookRefcode().c_str();
		std::string metaData = Utils::GetDeviceInfo();
		message.deviceinfo = (char*)metaData.c_str();
		log("device info:%s",message.deviceinfo);
		auto deviceHash = Utils::GetDeviceHash();
		message.device_hash = (char*)deviceHash.c_str();
		message.jsonmd5 = SINGLE_SHOP->getJsonSigure((int *)&message.n_jsonmd5);

		auto notificationToken = Utils::GetGCMToken();
		message.ostype = Utils::GetOSType();
		message.notificationid = (char*)notificationToken.c_str();

		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_TokenLogin);
	}
}

void ProtocolThread::engageInFightForTask(int npcId,UILoadingIndicator*loading)
{
	EngageInFightForTask message = ENGAGE_IN_FIGHT_FOR_TASK__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.npcid = npcId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_EngageInFightForTask);
	}
}

void ProtocolThread::authenticateAccount(char*newEmail,int forSecurity,UILoadingIndicator*loading)
{
	AuthenticateAccount message = AUTHENTICATE_ACCOUNT__INIT;
	if(forSecurity){
		message.languagecode = Utils::getLanguage();
		message.forsecurity = forSecurity;
		message.newemail = newEmail;
		message.apiurl = (char*)ServerManager::CurSelectedServer()->GetServerAddr().c_str();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_AuthenticateAccount);
	}else if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.languagecode = Utils::getLanguage();
		message.forsecurity = forSecurity;
		message.newemail = newEmail;
		message.apiurl = (char*)ServerManager::CurSelectedServer()->GetServerAddr().c_str();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_AuthenticateAccount);
	}
}


void ProtocolThread::logout(){
	if(mLastLoginResult){
		protobuf_c_message_free_unpacked ((ProtobufCMessage *)mLastLoginResult, 0);
		mLastLoginResult = 0;
	}
	TVSocketHolder::getInstance()->getSocket().Close(__FILE__,__LINE__);
	TVSocketHolder::getInstance()->getChatSocket().Close(__FILE__,__LINE__);
	auto chatCallback = chatMsgFunc;
	cocos2d::CCUserDefault::getInstance()->setStringForKey(USER_DEFAULT_USED_EMAIL,"0");
	m_googleAccount.clear();
	m_googleToken.clear();
	SINGLE_HERO->clean();
	if(mUserSession) delete mUserSession;
	init();
	chatMsgFunc = chatCallback;
}

void ProtocolThread::salvage(int x,int y,UILoadingIndicator*loading)
{
	Salvage message = SALVAGE__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.position_x = x;
		message.position_y = y;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_Salvage);
	}
}

void ProtocolThread::getEmailVerifiedReward(UILoadingIndicator*loading)
{
	GetEmailVerifiedReward message = GET_EMAIL_VERIFIED_REWARD__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetEmailVerifiedReward);
	}
}

void ProtocolThread::startSalvage(int x,int y,UILoadingIndicator*loading)
{
	StartSalvage message = START_SALVAGE__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.position_x = x;
		message.position_y = y;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_StartSalvage);
	}
}

void ProtocolThread::getEmailVerifiedInfo(UILoadingIndicator*loading)
{
	GetEmailVerifiedInfo message = GET_EMAIL_VERIFIED_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetEmailVerifiedInfo);
	}
}

void ProtocolThread::getSalvageCondition(int x,int y,UILoadingIndicator*loading)
{
	GetSalvageCondition message = GET_SALVAGE_CONDITION__INIT;
		if (mUserSession) {
			if(loading)setLoadingRes(loading);
			message.sessionid = mUserSession->getSessionId();
			message.userid = mUserSession->getUserId();
			message.position_x = x;
			message.position_y = y;
			sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetSalvageCondition);
		}
}

void ProtocolThread::getActivitiesProps(UILoadingIndicator*loading)
{
	GetActivitiesProps message = GET_ACTIVITIES_PROPS__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetActivitiesProps);
	}
}

void ProtocolThread::getActivitiesGiftInfo(UILoadingIndicator*loading)
{
	GetActivitiesGiftInfo message = GET_ACTIVITIES_GIFT_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetActivitiesGiftInfo);
	}
}

void ProtocolThread::getActivitiesPrayInfo(UILoadingIndicator*loading)
{
	GetActivitiesPrayInfo message = GET_ACTIVITIES_PRAY_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetActivitiesPrayInfo);
	}
}

void ProtocolThread::getPray(int idx,UILoadingIndicator*loading)
{
	GetPray message = GET_PRAY__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.idx = idx;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetPray);
	}
}

void ProtocolThread::sendActivitiesGift(int recipientsCid,UILoadingIndicator*loading)
{
	SendActivitiesGift message = SEND_ACTIVITIES_GIFT__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.recipientscid = recipientsCid;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_SendActivitiesGift);
	}
}

void ProtocolThread::getPirateAttackInfo(UILoadingIndicator*loading){
	GetPirateAttackInfo message = GET_PIRATE_ATTACK_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetPirateAttackInfo);
	}
}

void ProtocolThread::getAttackPirateInfo(int eventid, UILoadingIndicator*loading){
	GetAttackPirateInfo message = GET_ATTACK_PIRATE_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.eventid = eventid;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetAttackPirateInfo);
	}
}

void ProtocolThread::getVTicketMarketActivities(int type,UILoadingIndicator*loading){
	GetVTicketMarketActivities message = GET_VTICKET_MARKET_ACTIVITIES__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.type = type;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetVTicketMarketActivities);
	}
}

void ProtocolThread::buyVTicketMarketActivities(int type,int uniqueId,int count,UILoadingIndicator*loading){
	BuyVTicketMarketActivities message = BUY_VTICKET_MARKET_ACTIVITIES__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.type = type;
		message.uniqueid = uniqueId;
		message.count = count;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_BuyVTicketMarketActivities);
	}
}

void ProtocolThread::finalMyExploitScore(UILoadingIndicator*loading){
	FinalMyExploitScore message = FINAL_MY_EXPLOIT_SCORE__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_FinalMyExploitScore);
	}
}

void ProtocolThread::findLootPlayer(UILoadingIndicator*loading){
	FindLootPlayer message = FIND_LOOT_PLAYER__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_FindLootPlayer);
	}
}

void ProtocolThread::lootPlayer(UILoadingIndicator*loading){
	LootPlayer message = LOOT_PLAYER__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_LootPlayer);
	}
}

void ProtocolThread::getLootPlayerLog(int attackFlag, int displayalllog, UILoadingIndicator*loading){
	GetLootPlayerLog message = GET_LOOT_PLAYER_LOG__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.attackflag = attackFlag;
		message.displayalllog = displayalllog;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetLootPlayerLog);
	}
}

void ProtocolThread::startRevengeOnLootPlayer(int logId,UILoadingIndicator*loading){
	StartRevengeOnLootPlayer message = START_REVENGE_ON_LOOT_PLAYER__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.logid = logId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_StartRevengeOnLootPlayer);
	}
}

void ProtocolThread::getAttackPirateRankInfo(UILoadingIndicator*loading){
	GetAttackPirateRankInfo message = GET_ATTACK_PIRATE_RANK_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetAttackPirateRankInfo);
	}
}

void ProtocolThread::getAttackPirateBossInfo(int enent, UILoadingIndicator*loading){
	GetAttackPirateBossInfo message = GET_ATTACK_PIRATE_BOSS_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.eventid = enent;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetAttackPirateBossInfo);
	}
}

void ProtocolThread::buyInsurance(int64_t count,int days,int percent,int protectDelegate,UILoadingIndicator*loading){
	BuyInsurance message = BUY_INSURANCE__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.days = days;
		message.count = count;
		message.percent = percent;
		message.protectdelegate = protectDelegate;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_BuyInsurance);
	}
}

void ProtocolThread::completeDialogs(int dialogId,UILoadingIndicator*loading){
	CompleteDialogs message = COMPLETE_DIALOGS__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.dialogid = dialogId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_CompleteDialogs);
	}
}

void ProtocolThread::obtainLottoMessage(UILoadingIndicator*loading){
	ObtainLottoMessage message = OBTAIN_LOTTO_MESSAGE__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.lang = Utils::getLanguage();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_ObtainLottoMessage);
	}
}

void ProtocolThread::getInviteBonus(UILoadingIndicator*loading){
	GetInviteBonus message = GET_INVITE_BONUS__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetInviteBonus);
	}
}

std::string& ProtocolThread::getInviteCode(){
	return m_inviteCode;
}

void ProtocolThread::getCompanies(UILoadingIndicator*loading){
	GetCompanies message = GET_COMPANIES__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetCompanies);
	}
}

void ProtocolThread::equipCompany(int companyId,int isCaptain,int headId, int clothId, int weaponId, int accessId, int shoeId,UILoadingIndicator*loading){
	EquipCompany message = EQUIP_COMPANY__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.headid = headId;
		message.clothesid = clothId;
		message.weaponid = weaponId;
		message.accessid = accessId;
		message.shoeid = shoeId;
		message.id = companyId;
		message.iscaptain = isCaptain;
		//message.cid = cid;
		sendMessage((struct ProtobufCMessage*) &message,PROTO_TYPE_EquipCompany);
	}
}

void ProtocolThread::unlockCompany(int protoId,int method,UILoadingIndicator*loading){
	UnlockCompany message = UNLOCK_COMPANY__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.companyprotoid = protoId;
		message.method = method;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_UnlockCompany);
	}
}

void ProtocolThread::getCompanionsStatus(UILoadingIndicator*loading){
	GetCompanionsStatus message = GET_COMPANIONS_STATUS__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetCompanionsStatus);
	}
}

void ProtocolThread::getShipCompanions(UILoadingIndicator*loading){
	GetShipCompanions message = GET_SHIP_COMPANIONS__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetShipCompanions);
	}
}

void ProtocolThread::setShipCompanions(int companionsId, int cabinno, int  isCaptain, int shipId, UILoadingIndicator*loading){
	SetShipCompanions message = SET_SHIP_COMPANIONS__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.companionsid = companionsId;
		message.cabinno = cabinno;
		message.iscaptain = isCaptain;
		message.shipid = shipId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_SetShipCompanions);
	}
}

void ProtocolThread::unlockShipCabin(int shipId, int sid,int cabinNo,UILoadingIndicator*loading){
	UnlockShipCabin message = UNLOCK_SHIP_CABIN__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.cabinno = cabinNo;
		message.sid = sid;
		message.shipid = shipId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_UnlockShipCabin);
	}
}

void ProtocolThread::saveFailedFight(UILoadingIndicator*loading){
	SaveFailedFight message = SAVE_FAILED_FIGHT__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_SaveFailedFight);
	}
}

void ProtocolThread::endFailedFightByVTicket(int useVTicket, int fighttype, UILoadingIndicator*loading){
	EndFailedFightByVTicket message = END_FAILED_FIGHT_BY_VTICKET__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.usevticket = useVTicket;
		message.fighttype = fighttype;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_EndFailedFightByVTicket);
	}
}

void ProtocolThread::completeCompanionTask(int taskId,int partId,int missionFailed,UILoadingIndicator*loading){
	CompleteCompanionTask message = COMPLETE_COMPANION_TASK__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.partid = partId;
		message.taskid = taskId;
		message.missionfailed = missionFailed;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_CompleteCompanionTask);
	}
}

void ProtocolThread::deductCoins(int coins,UILoadingIndicator*loading){
	DeductCoins message = DEDUCT_COINS__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.decuctcoins = coins;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_DeductCoins);
	}
}

void ProtocolThread::updateDataVersion(UpdateUserSelection**select,int num,UILoadingIndicator*loading){
	UpdateDataVersion message = UPDATE_DATA_VERSION__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.update = select;
		message.n_update = num;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_UpdateDataVersion);
	}
}

void ProtocolThread::calFastHireCrew(UILoadingIndicator*loading){
	CalFastHireCrew message = CAL_FAST_HIRE_CREW__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_CalFastHireCrew);
	}
}

void ProtocolThread::companionRebirth(int companionId,int isCaptain,UILoadingIndicator*loading){
	CompanionRebirth message = COMPANION_REBIRTH__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.companionid = companionId;
		message.iscaptain = isCaptain;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_CompanionRebirth);
	}
}

void ProtocolThread::repairEquipment(int equipId,int useType,UILoadingIndicator*loading){
	RepairEquipment message = REPAIR_EQUIPMENT__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.equipid = equipId;
		message.usetype = useType;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_RepairEquipment);
	}
}

void ProtocolThread::getRepairEquipmentNeed(int equipId,UILoadingIndicator*loading){
	GetRepairEquipmentNeed message = GET_REPAIR_EQUIPMENT_NEED__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.equipid = equipId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetRepairEquipmentNeed);
	}
}

void ProtocolThread::getRecentOrder(int type,UILoadingIndicator*loading){
	GetRecentOrder message = GET_RECENT_ORDER__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.type = type;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetRecentOrder);
	}
}

void ProtocolThread::getDropIncreaseRateInfo(UILoadingIndicator*loading){
	GetDropIncreaseRateInfo message = GET_DROP_INCREASE_RATE_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetDropIncreaseRateInfo);
	}
}

void ProtocolThread::getDailyActivitiesReward(int rewardType,UILoadingIndicator*loading){
	GetDailyActivitiesReward message = GET_DAILY_ACTIVITIES_REWARD__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.reward_type = rewardType;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetDailyActivitiesReward);
	}
}

void ProtocolThread::addProficiency(int proficiencyId,int count,UILoadingIndicator*loading){
	AddProficiency message = ADD_PROFICIENCY__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.proficiencyid = proficiencyId;
		message.count = count;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_AddProficiency);
	}
}

void ProtocolThread::getProficiencyBook(int proficiencyId,UILoadingIndicator*loading){
	GetProficiencyBook message = GET_PROFICIENCY_BOOK__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.proficiencyid = proficiencyId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetProficiencyBook);
	}
}

void ProtocolThread::getProficiencyValues(UILoadingIndicator*loading){
	GetProficiencyValues message = GET_PROFICIENCY_VALUES__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetProficiencyValues);
	}
}

//void ProtocolThread::prepareCityDefense(int type,int count,LoadingLayer*loading){
//	PrepareCityDefense message = PREPARE_CITY_DEFENSE__INIT;
//	if (mUserSession) {
//		if(loading)setLoadingRes(loading);
//		message.sessionid = mUserSession->getSessionId();
//		message.userid = mUserSession->getUserId();
//		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_PrepareCityDefense);
//	}
//}
//void ProtocolThread::prepareCityAttack(int type,int count,LoadingLayer*loading){
//	PrepareCityAttack message = PREPARE_CITY_ATTACK__INIT;
//	if (mUserSession) {
//		if(loading)setLoadingRes(loading);
//		message.sessionid = mUserSession->getSessionId();
//		message.userid = mUserSession->getUserId();
//		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_PrepareCityAttack);
//	}
//}
void ProtocolThread::getCityPrepareStatus(UILoadingIndicator*loading){
	GetCityPrepareStatus message = GET_CITY_PREPARE_STATUS__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetCityPrepareStatus);
	}
}
//void ProtocolThread::startStateWar(int nation,LoadingLayer*loading){
//	StartStateWar message = START_STATE_WAR__INIT;
//	if (mUserSession) {
//		if(loading)setLoadingRes(loading);
//		message.sessionid = mUserSession->getSessionId();
//		message.userid = mUserSession->getUserId();
//		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_StartStateWar);
//	}
//}
void ProtocolThread::getEnemyNation(UILoadingIndicator*loading){
	GetEnemyNation message = GET_ENEMY_NATION__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetEnemyNation);
	}
}
void ProtocolThread::addCityRepairPool(int64_t count,UILoadingIndicator*loading){
	AddCityRepairPool message = ADD_CITY_REPAIR_POOL__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.consume_coins = count;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_AddCityRepairPool);
	}
}
void ProtocolThread::addCityAttackPool(int64_t count,UILoadingIndicator*loading){
	AddCityAttackPool message = ADD_CITY_ATTACK_POOL__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.consume_coins = count;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_AddCityAttackPool);
	}
}

void ProtocolThread::getPackageInfo(UILoadingIndicator*loading){
	GetPackageInfo message = GET_PACKAGE_INFO__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetPackageInfo);
	}
}

void ProtocolThread::getNationWarEntrance(int city, UILoadingIndicator*loading){
	GetNationWarEntrance message = GET_NATION_WAR_ENTRANCE__INIT;
	if (mUserSession) {
		if (loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.city = city;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetNationWarEntrance);
	}
}
void ProtocolThread::getContributionRank(UILoadingIndicator*loading){
	GetContributionRank message = GET_CONTRIBUTION_RANK__INIT;
	if (mUserSession) {
		if (loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetContributionRank);
	}
}

void ProtocolThread::getNationWarCityLost(UILoadingIndicator*loading){
	GetNationWarCityLost message = GET_NATION_WAR_CITY_LOST__INIT;
	if (mUserSession) {
		if (loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetNationWarCityLost);
	}
}

void ProtocolThread::reachDepot(UILoadingIndicator*loading){
	ReachDepot message = REACH_DEPOT__INIT;
	if (mUserSession) {
		if (loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_ReachDepot);
	}
}

void ProtocolThread::reduceInNationWar(UILoadingIndicator*loading){
	ReduceInNationWar message = REDUCE_IN_NATION_WAR__INIT;
	if (mUserSession) {
		if (loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_ReduceInNationWar);
	}
}

void ProtocolThread::applyStateWar(int nation_id, UILoadingIndicator*loading)
{
	ApplyStateWar message = APPLY_STATE_WAR__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.nation = nation_id;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_ApplyStateWar);
	}
}

void ProtocolThread::startAttackCity(int city_id, UILoadingIndicator*loading)
{
	StartAttackCity message = START_ATTACK_CITY__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.city_id = city_id;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_StartAttackCity);
	}
}

void ProtocolThread::endAttackCity(int city_id, UILoadingIndicator*loading)
{
	EndAttackCity message = END_ATTACK_CITY__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.city_id = city_id;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_EndAttackCity);
	}
}


void ProtocolThread::warHiredArmy(int nationId, int64_t comsumeCoins, UILoadingIndicator*loading /* = 0 */)
{
	HireArmy message = HIRE_ARMY__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.nation = nationId;
		message.consume_coins = comsumeCoins;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_HireArmy);
	}

}
void ProtocolThread::buildcountryWarDepot(int nationId, int32_t consumeV, UILoadingIndicator*loading)
{
	AddSupplyStation message = ADD_SUPPLY_STATION__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.nation = nationId;
		message.consume_golds = consumeV;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_AddSupplyStation);
	}
}
void ProtocolThread::onWaringRepairDefense(int type, int64_t count, UILoadingIndicator*loading)
{
	RepairCityDefenseInWar message = REPAIR_CITY_DEFENSE_IN_WAR__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.type = type;
		message.consume = count;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_RepairCityDefenseInWar);
	}
}
void ProtocolThread::onWaringEnhanceAttack(int city, int64_t consume_coins, UILoadingIndicator*loading)
{
	EnhanceAttack message = ENHANCE_ATTACK__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.city = city;
		message.consume_coins = consume_coins;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_EnhanceAttack);
	}
}

void ProtocolThread::getMyNationWar(UILoadingIndicator*loading)
{
	GetMyNationWar message = GET_MY_NATION_WAR__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetMyNationWar);
	}
}

void ProtocolThread::funcForTest(UILoadingIndicator*loading)
{
	FuncForTest message = FUNC_FOR_TEST__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_FuncForTest);
	}
}

void ProtocolThread::checkDeleteCharactersNum(UILoadingIndicator*loading)
{
	CheckDeleteCharactersNum message = CHECK_DELETE_CHARACTERS_NUM__INIT;
	if (mUserSession) {
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_CheckDeleteCharactersNum);
	}
}


void ProtocolThread::getPlayerShipList(int cid, UILoadingIndicator * loading)
{
	GetPlayerShipList message = GET_PLAYER_SHIP_LIST__INIT;
	if (mUserSession)
	{
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.character_id = cid;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetPlayerShipList);
	}
}


void ProtocolThread::getPlayerEquipShipInfo(int shipId, UILoadingIndicator * loading)
{
	GetPlayerEquipShipInfo message = GET_PLAYER_EQUIP_SHIP_INFO__INIT;
	if (mUserSession)
	{
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.shipid = shipId;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetPlayerEquipShipInfo);
	}
}
void ProtocolThread::tarvenFireSailors(int fireNums, UILoadingIndicator*loading /* = 0 */)
{
	FireCrew message = FIRE_CREW__INIT;
	if (mUserSession)
	{
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.fire_crew_num = fireNums;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_FireCrew);
	}
}
void ProtocolThread::getVersionChangeLog(int32_t os_type, int32_t lang, UILoadingIndicator*loading /* = 0 */, int32_t version /* = 0 */, int has_lang /* = 0 */, int has_version /* = 0 */)
{
	GetChangeLog message = GET_CHANGE_LOG__INIT;
	if (mUserSession)
	{
		setLoadingRes(loading);
		//message.sessionid = mUserSession->getSessionId();
		//message.userid = mUserSession->getUserId();
		message.os_type = os_type;
		message.lang = lang;
		message.version = version;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetChangeLog);
	}
}

void ProtocolThread::getForceCity(int forceid, UILoadingIndicator*loading /* = 0 */)
{
	GetForceCity message = GET_FORCE_CITY__INIT;
	if (mUserSession)
	{
		setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		message.force_id = forceid;
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_GetForceCity);
	}
}

#if ADD_PROTO_SAMPEL
void ProtocolThread::protoSample(int argu1,UILoadingIndicator*loading){
	ProtoSample message = PROTO_SAMPLE__INIT;
	if (mUserSession) {
		if(loading)setLoadingRes(loading);
		message.sessionid = mUserSession->getSessionId();
		message.userid = mUserSession->getUserId();
		sendMessage((struct ProtobufCMessage*) &message, PROTO_TYPE_ProtoSample);
	}
}
#endif
