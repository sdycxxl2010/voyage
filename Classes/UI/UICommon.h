/*
*  CopyRight (c) 
*  Created on: 2014年11月15日
*  Author: 
*  description: public dialog
*/
#ifndef __COMMON_VIEW_H__
#define __COMMON_VIEW_H__

#include "UIBasicLayer.h"
#include "UICenter.h"
#include "UICenterCharactor.h"
#define IMAGE_INDEX1 1000
#define IMAGE_INDEX2 2000

enum IMAGE_ICON_INDEX//图鉴index
{
	IMAGE_ICON_ATTACK = 1,
	IMAGE_ICON_DEFENCE,
	IMAGE_ICON_DURABLE,
	IMAGE_ICON_SPEED,
	IMAGE_ICON_STEERING,
	IMAGE_ICON_SAILOR,
	IMAGE_ICON_WEIGHT,
	IMAGE_ICON_SUPPLY,
	IMAGE_ICON_CARGO,
	IMAGE_ICON_BAGGO,
	IMAGE_ICON_CRAFT,
	IMAGE_ICON_SILVER,
	IMAGE_ICON_CLOCK,
	IMAGE_ICON_PORTUGAL,
	IMAGE_ICON_SPAIN,
	IMAGE_ICON_ENGLAND,
	IMAGE_ICON_NETHERLANDS,
	IMAGE_ICON_GENOVA,
	IMAGE_ICON_VENICE,
	IMAGE_ICON_SWEDEN,
	IMAGE_ICON_SUDAN,
	IMAGE_ICON_FRANCE,
	IMAGE_ICON_PRESTIGE,
	IMAGE_ICON_COINS,
	IMAGE_ICON_VTICKET,
	IMAGE_ICON_POPULATION,
	IMAGE_ICON_TRADE,
	IMAGE_ICON_MANUFACTURE,
	IMAGE_ICON_PACKAGE,
	IMAGE_ICON_HEALTHPOWER,
	IMAGE_ICON_ATTACKPOWER,
	IMAGE_ICON_DEFENSEPOWER,
	IMAGE_ICON_AMOUNTOFSAILORS,
	IMAGE_ICON_MOUNTOFSUPPLIES,
	IMAGE_ICON_SAILINGDAYS,
	IMAGE_ICON_COORDINATE,
	IMAGE_ICON_FRIEND,
	IMAGE_ICON_ENEMY,
	IMAGE_ICON_LICENSE,

};
//改造方案ICON
enum SHIP_ATTR
{
	ICON_ATTACK,
	ICON_DEFENSE,
	ICON_DURABLE,
	ICON_SPEED,
	ICON_STEERING,
	ICON_SAILORS,
	ICON_CAPACITY,
	ICON_SUPPLY,
};
enum CAPTAIN_EXPUI
{
	//普通界面
	CAPTAIN_EXPUI_NORMAL,
	//战斗结算界面
	CAPTAIN_EXPUI_BATTLE,	
};
enum CURRENT_VIEW
{
	VIEW_SHIP = 1,
	VIEW_EQUIPMENT,
};
enum MESH_NAME
{
	//帽子贴图
	MESH_HATS,
	//上衣贴图(衣服)
	MESH_CLOTHES,
	//裤子贴图(衣服)
	MESH_PANTS,
	//饰品
	MESH_NECKLACE,
	//鞋
	MESH_SHOES,
	//手套
	MESH_GLOVES,
};
//特殊处理---船只是从1开始 装备是从0开始
static const std::string PROPERTY_ICON_PTAH[] =
{
	"res/shipAttIcon/att_1.png",
	"res/shipAttIcon/att_2.png",
	"res/shipAttIcon/att_3.png",
	"res/shipAttIcon/att_4.png",
	"res/shipAttIcon/att_5.png",
	"res/shipAttIcon/att_6.png",
	"res/shipAttIcon/att_9.png",
	"res/shipAttIcon/supply.png"
};
/*
*id : 技能的id  lv : 技能的等级
* skill_type : 技能所属的类型 iconId : 技能所属的icon
*/
struct SKILL_DEFINE
{
	int id;
	int lv;
	int skill_type;
	int icon_id;
};

class UICommon : public UIBasicLayer
{
public:
	static UICommon*  getInstance()
	{
		if (m_commonView == nullptr)
		{
			m_commonView = new UICommon;
		}
		return m_commonView;
	}
	UICommon();
	~UICommon();
	void openCommonView(UIBasicLayer *parent);//获取当前场景

	void flushShipDetail(ShipDefine* shpInfo,int id,bool isHideDrop = false);//船详细信息
	void flushGoodsDetail(HatchItemsDefine* itemDefine,int id,bool isHideDrop = false);//物品详细信息goods
	void flushItemsDetail(EquipmentDefine* itemDefine,int id,bool isHideDrop = false);//装备，图纸等items
	void flushEquipsDetail(EquipmentDefine* equipDetail,int id,bool isHideDrop = false,bool canNotRepair = false);//装备信息
	void flushDrawingDetail(DrawingItemsDefine* drawDefine,int id,bool isHideDrop = false);//图纸信息
	void flushSpecialDetail(int id, bool isHideDrop = false);//其他物品
	void flushSilverConfirmView(std::string titleName,std::string contentName,int coin); //银币
	void flushVConfirmView(std::string titleName,std::string contentName,int v); //V票
	/*
	*问号解释
	*参数  titleName:tip标题名 contentName:tip内容名  roomId:读本地小伙伴工作室数据json文件中的文案
	*/
	void flushInfoBtnView(std::string titleName, std::string contentName, int roomId = -1);
	/*
	*技能详情界面
	*参数  skillDefine 技能数据
	*/
	void flushSkillView(const SKILL_DEFINE skillDefine,bool needShow = true);
	void flushFriendDetail(GetUserInfoByIdResult *result,bool isHideButton = false);//好友信息
	void getNameAndPath(int type,int id,std::string &name,std::string &path);//获取物品名字
	void buttonEventByName(Widget* target,std::string name);//点击事件
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType) override;
	void buttonYesEvent(Ref *pSender,Widget::TouchEventType TouchType);
	void imageCallEvent(Ref *pSender,Widget::TouchEventType TouchType);
	void flushImageDetail(Widget *target);//显示图标信息
	void imageDetailAutoColse(float time);//图标信息显示后，自动关闭
	void flushPlayerLevelUp();//人物升级
	void flushCaptainLevelUp(int n_captains, CaptainInfo **captains);//船长加经验
	void flushWarning(WARNING_LV wargingLv);//显示警告信息
	void flushPrestigeLevelUp();//声望升级
	void flushRank(GetLeaderboardResult *result);//投资排行
	void showNumpad(Text* inputNum);//数字键盘
	void closeNumpad();
	void buttonNumpadEvent(Ref *pSender,Widget::TouchEventType TouchType);//点击数字键盘
	void getNotZeroFromString(std::string src,int& value_1,int& index_1,int& value_2,int& index_2);
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	void flushPlayerAddExpOrFrame(int64_t resultExp,int64_t resultFrame,int64_t addExp,int64_t addFrame);//显示人物增加的经验和声望
	void flushCostOnlyCoin(int64_t coin,std::string titleTest,std::string contentText,std::string costFor);//雇用成功，投资成功结算框
	void openCestatusView(GetForceCityResult* result);
	void flushContryListview();
	void openManufactureView(GetForceCityResult* result);
	/*
	*获取港口类型
	*/
	std::string getPortTypeOrInfo(int port, int citiesId);
	/**
	*获取船长经验增加界面
	*flag : 界面标识 普通/战斗结束
	*/
	void getCaptainAddexp(CAPTAIN_EXPUI flag = CAPTAIN_EXPUI_NORMAL);
	void flushPrestigeLevelDown();//声望降级
	void getNotZeroFromString(std::string src, std::string& value1, std::string& value2, int& index1, int& index2);//截取强化波动字符串
	void flushEquipBrokenView(OutOfDurableItemDefine **brokenequips = nullptr, size_t n_equipnum = 0 );//装备耐久为0时弹出对话框
	void equipBrokenTouchEvent(Ref * pSender, Widget::TouchEventType TouchType);//装备损坏对话框点击事件
	//boss基地排行榜
	void flushBossDamageRank(GetAttackPirateRankInfoResult *result, bool isShowClose = false);
	//船长经验增加界面Item显示
	void addItem(float dt);
	//带滚动条的长文本显示
	void flushWarningLongText(std::string title, std::string content);
	//打开船只详细信息的船舱界面
	void flushShipCabinsView();
	//打开船只详细信息的装备界面
	void flushShipPartsView();
	/**
	*获取不同类型船只舱室信息
	*/
	void getCabinsInfo(std::map<int,int> &carbins);
	/**
	*判定当前船只的熟练度等级是否满足
	*curView : 1为船只详情 2为装备详情
	*/
	void judgeProficiencyLevel(int curView);
	/**
	*获取当前熟练度对应的熟练度等级
	*/
	int getProficiencyLevel(int proficiencyNum);

	/**
	*显示船只模型界面
	*/
	void flushShipListView(GetPlayerShipListResult* result);
	/**
	*显示船只装备详情
	*showMode true 显示模型 false 显示装备信息
	*/
	void showShipModelView(int modeId);

	//显示船只装备 或者 模型界面
	// true 模型 fasle 装备
	void showEquipmentOrShipModel(bool show);

	/**
	*刷新装备界面
	*/
	void flushShipEquimentView(GetPlayerEquipShipInfoResult * result);


	GetPlayerEquipShipInfoResult* getEquipDetailSumInfo(GetPlayerEquipShipInfoResult * shipInfo);

	//舰队列表更改船只的选中状态
	void changeSlectShipButton(Widget * psender);

	/**
	*显示角色模型界面
	*/
	void flushCharacterModelView(GetHeroEquipResult *  info);

	void confirmCharacterFaceAndHair(Sprite3D * character);

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);

	void convertToVector(std::string &_src, std::vector<int> &_des);

	int getEquipIID(const int nId, GetPlayerEquipShipInfoResult * m_pShipInfoResult);
	EquipDetailInfo * getEquipDetailInfo(const int iid);
private:
	static UICommon    *m_commonView;
	std::vector<Widget*> m_vViews;
	std::vector<int> m_vOpenViews;

	UIBasicLayer* m_pParent;
	
	Text * m_inputNum;
	Widget*m_pCaptainAddExp;//船长增加经验
	//船长增加经验战斗界面
	Widget*m_pCaptainAddExpBattle;
	size_t  m_brokennum;//损坏的装备数
	OutOfDurableItemDefine ** m_brokenItems;
	//海上结算船长经验界面的listView
	ListView * m_captainlist;
	//海上结算船长经验界面的items
	std::vector<Widget*> m_listitems;
	//船长升级列表是否完全显示
	bool m_bitemsaddCompleted;
	EquipmentDefine*  m_pEquipDetail;
	int  m_curShipId;
	int  m_curEquipId;
	/**
	*熟练度信息
	*/
	GetProficiencyValuesResult * m_proficiencyResult;
	/**
	*当前详情界面 船只/装备
	*/
	int m_curView;
	/**
	*舰队船只信息
	*/
	GetShipListResult* m_shipInfoResult;
	/*
	* 所选势力港口信息
	*/
	GetForceCityResult  *m_forceResult;
	/**
	*当前操作的模型
	*/
	Sprite3D * m_operaModel;
	Vec2 m_touchBegan;
	/**
	*当前玩家信息
	*/
	GetUserInfoByIdResult * m_curuserInfo;

	/**
	*模型layer是否需要关闭
	*/
	bool m_needClose;
	/*
	*下拉列表是否收起
	*/
	bool m_ListClose;
	/**
	*当前是否正显示船装备界面
	*/
	bool m_shipEquimentShow;
	/*
	*当前所点击的势力id
	*/
	int  n_ForceId;
	/**
	*装备信息
	*/
	GetPlayerEquipShipInfoResult * m_ShipInfoResult;
};

class ShipModel :public Sprite3D
{
public:
	ShipModel();
	~ShipModel();
	bool init(int modelId);
    static ShipModel* create(int modelId);
	void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
protected:
	/**
	*添加船部件
	*@param p_sender 当前加载的船只
	*/
	void addSail(Sprite3D * p_Sender);
	/**
	*船部件确认
	*@parm bone 需要添加部件的骨骼
	*/
	Sprite3D * confirmSail(Bone3D * bone);

	/*
	* 读取骨点名中信息
	* @param name 骨骼名字
	*/
	std::vector<std::string> analysisName(std::string name);
protected:
	/**
	*船桨编号
	*/
	int m_oars_index;
	/**
	*旗帜Mesh编号
	*/
	int m_flag_index;
};
#endif
