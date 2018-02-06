/*
*  CopyRight (c) 
*  Created on: 2014年11月15日
*  Author: xie 
*  description: something about bank
*/
#ifndef __BANK_SCENE_H__
#define __BANK_SCENE_H__
#include "UIBasicLayer.h"

class UIVoyageSlider;
class UIBank : public UIBasicLayer
{
public:
	//保险箱物品属性
	struct BankItem
	{
		int type;//类型
		int cid;//主角id
		int iid;//物品id
		int num;//物品数量
		int durability;//耐久
		int max_durability;//最大耐久
		int m_speed;//速度
		int attack;//攻击
		int defense;//防御
		int steer_speed;//转向速度
		int optional_item;//是否强化过，大于0代表强化过
		int optional_num;//用的什么强化道具
		char *optional_value;//用不到
	};
	//我的投保信息
	struct InsuranceInfor
	{
		//剩余保额
		int64_t leftamount;
		//总保额
		int64_t totalamount;
		//剩余时间
		int64_t lefttimeinseconds;
		//投保比率
		int64_t percent;
		//是否选择委托代理
		int32_t protectdelegate;
	};
	enum SAFEPANEL_GOOD_TAG
	{
		SAFE_GOODS= 0,//物品
		SAFE_EQUIP_TAG = 1000,//船装备
		SAFE_ITEM_DRAWING = 100000,//图纸
		SAFE_ITEM_PROP =1000000,//道具
	};
	//投保周期,三挡
	enum INSURANCE_PERIOD
	{
		FIRST_PERIOD =1,
		SECOND_PERIOD = 3,
		THIRD_PERIOD = 7,
	};
	//左侧按钮
	enum LEFT_BUTTON_TAG
	{
		BUTTON_TAG_NONE,
		BUTTON_TAG_PERSONAL_COIN,
		BUTTON_TAG_PERSONAL_INSURANCE,
		BUTTON_TAG_GUILD_COIN,
		BUTTON_TAG_GUILD_SAFE,
		BUTTON_TAG_GUILD_LOG,
	};
public:
	UIBank();
	~UIBank();
	//初始化
	bool init();
	void onEnter();
	void onExit();
	static UIBank* createBank();
	//初始化数据
	void initStatic(float f);

	//默认点击事件
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType) override;
	void onServerEvent(struct ProtobufCMessage* message,int msgType);

	//滑动条确定按钮的返回事件
	void SliderEvent(Ref*);

	//回调事件
	void button_safebackEvent(Ref*);

	//点击事件
	void buttonEventByName(Widget* target,std::string name);
	void subButtonEventByName(Widget* target,std::string name);

	//保险箱
	void updateItemView();

	//更新银币V票
	void updateMoneyView();

	//记录，参数result保存的接口数据
	void updateLogView(const GetBankLogResult* result);

	//初始化货币界面
	void flushMyCoinGold();

	//选择物品
	void depositItem();

	//没有工会时的表达
	void showHaveNoGuild();

	//没有工会权限只能存钱
	void hideTakeButton();

	//商城更新
	void updateOnlyCoinGold();

	//初始化保险箱的物品
	void itemInfoForItemView(Widget* parent,int index);

	//获取记录数据
	void itemInfoForLogView(Widget* parent,std::string context);

	//保存safe界面的物品
	void pushItems_vector();

	//把物品从warehouse存入safe界面  type：物品类型，cid：主角id, iid：物品id, num：物品数量
	void inserItem_vector(std::shared_ptr<BankItem>bankItem);

	//删除safe界面的物品  type：物品类型，cid：主角id, iid：物品id, num：物品数量
	void deleteItem_vector(std::shared_ptr<BankItem>withDrawItem);

	//取物品   index：点击物品的tag值
	void withdrawItem(const int index);

	//更新物品  isDeposit：存还是取
	void updateItemSelect(const bool isDeposit);

	//滑动条数量选择  num：选择物品的数量
	void onSliderNumbers(const int num);

	//滑动条关闭事件
	void bankSliderClose(Ref*);

	//显示数字键盘
	void showBankNumPad();

	//键盘移动结束
	void moveEnd(){ m_bNumpadFlag = true; };

	//数字键盘事件
	void bankNumpadEvent(Ref *pSender, Widget::TouchEventType TouchType);

	//文本框内容透明度变化
	void update(float dt);

	//敌对城市银行的限制
	void opposedCity();

	//敌对银行对safe按钮的处理
	void opposedCitySafeBtn();
	
	//打开保险界面
	void enterInsuranceModule();

	//打开投保界面
	void openInsuranceInsure();

	//我的投保信息
	void showMyInsuranceInfo();

	//投保界面的选择按钮
	void insureBtn_func(Ref *pSender, Widget::TouchEventType TouchType);

	//选择框的点击事件
	void checkBoxCall_func(Ref* target,CheckBoxEventType type);

	//当选择不同的保险比例和天数时，改变花费
	void changeCostByRatioAndTime();

	//投保的倒计时
	void insuranceLeftTime(float f);

	//投保的数字键盘
	void showMyInsuranceNumPad();
	//Unreasonable_code_28;使用公用截取方法
	//log里面物品名字过长，截取，汉字10个，英文20个  itemName：要截取的内容
	std::string logItemName(std::string itemName);

private:
	bool						m_bIsBankItem;//物品
	bool						m_bIsBankLog;//记录
	bool						m_bIsMyBank;//个人银行还是公会银行
	bool						m_bIsChangeBank;//切换标识
	bool						m_bIsDeposit;//存还是取
	bool					    m_bNumpadFlag;//键盘控制
	int64_t						m_nMoneyCount;//存进银行的钱数
	std::string                 m_padString;//数字键盘输入的字符串
	Widget*				m_pButtonDeposit;//物品栏按下的按钮
	UIVoyageSlider*			m_pSlider;//滑动条
	BankItem*			m_pBankItem;//银行物品
	Node *					m_pItemCenter;//存储物品对象
	GetBankInfoResult* m_pBankResult;//存储返回结果
	std::vector<BankItemDefine*> m_vBankItems;//存储银行物品
	//敌对国家不可进入safe界面
	bool m_bIsCanGotoSafe;
	//投保周期
	std::vector<int> m_ratio_times;
	//投保比率
	std::vector<InsuranceItem*>m_insurance_item;
	//选择投保的哪个周期
	int n_chooseRatiotime;
	//选择的投保的哪个比率
	int n_chooseRatio;
	//是否选择委托代理
	int n_protectDelegate;
	//保费
	int64_t n_insurance_coinCost;
	//保费，委托代理
	int64_t n_insurance_VticketCost;
	//投保输入框
	TextField* m_textField_insure;
	//我的投保信息
	InsuranceInfor*m_myInsurance;
	//是否改变投保
	bool m_bChangeInsurePlan;
	//购买保险结果
	BuyInsuranceResult*m_buyInsurenceResult;
	//装备的属性
	EquipmentDefine* m_pEquipDetail;
	//左侧按钮
	LEFT_BUTTON_TAG m_bLeftButtonTag;
};
#endif
