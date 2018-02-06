/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game center equip implementation class
 *
 */

#ifndef __CENTER__EQUIP__
#define __CENTER__EQUIP__

#include "UIBasicLayer.h"

class UICenterCharactorEquip : public UIBasicLayer
{
	enum EQUIP_INDEX
	{
		//帽子
		EQUIP_HAT = 0,
		//衣服
		EQUIP_CLOTH,
		//武器
		EQUIP_WEAPON,
		//饰品
		EQUIP_ACCESS,
		//鞋子
		EQUIP_SHOES,	
	};

	enum MESH_INDEX
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
public:
	UICenterCharactorEquip();
	~UICenterCharactorEquip();
	static UICenterCharactorEquip* createEquip(UIBasicLayer *parent);
	/*
	*打开个人装备界面
	*/
	void openCenterEquip();
	/*
	*按钮点击事件
	*/
	void buttonEvent(Widget *pSender, std::string name);
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage *message, int msgType);
	/*
	*获取装备数据是否保存
	*/
	bool getFinishData();
	/*
	*设置装备数据是否保存
	*参数 bFinish：是否保存了数据
	*/
	void setFinishData(const bool bFinish);
	/*
	*保存人物装备
	*/
	void saveEquipHero();
	/**
	*打开equip层时是否添加3d角色
	*/
	void setAddCharacterEnable(){ m_alreadyAdd = false; };

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);

	bool init();

	void onEnter();

	//获取装备属性值
	std::vector<int> getEquipAttribute(const std::string attributeStr);
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*人物的装备主界面
	*/
	void flushEquipButton();
	/*
	*人物的装备界面--刷新装备icon
	*参数 pResult:人物装备数据
	*/
	void flushHeroEquipInfomation(const GetHeroEquipResult *pResult);
	/*
	*选择装备的界面
	*/
	void flushEquipinfo();
	/*
	*选择装备后更新界面显示
	*参数 nId:装备的id
	*/
	void updataSelectEquip(const int nId);
	/*
	*更新装备数据
	*参数 pResult:装备数据 
	*/
	void updateEquipData(const GetEquipableItemsResult *pResult);

	/**
	*添加任务模型
	*/
	void addCharacter3DModel(Node * parent, const std::string path);

	/**
	*添加装备
	*/
	void addEquipment(Sprite3D * character);

	/**
	*更新角色数据
	*/
	void updteCharacterInfo(int hp, int defence, int attack);

	/**
	*确定3D角色的脸和发型
	*character : 3d模型
	*/
	void confirmCharacterFaceAndHair(Sprite3D * character);
	//calculate  rotation quaternion parameters
	void calculateArcBall(cocos2d::Vec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y);

	void updateCameraTransform();

	float projectToSphere(float r, float x, float y);

private:
	/*
	*确认索引
	*/
	enum CONFIRM_INDEX
	{
		CONFIRM_INDEX_SAVE_DATA,
		CONFIRM_INDEX_EQUIP,
	};
	/*
	*根节点(CenterLayer)
	*/
	UIBasicLayer	   *m_pParent;
	/*
	*记录选择的装备
	*/
	Widget*         m_pTempButton;
	/*
	*人物装备的类型
	*/
	int             m_nType;
	/*
	*点击按钮时，区分不同的操作
	*/
	CONFIRM_INDEX     m_eConfirmIndex;
	/*
	*人物装备数据是否保存
	*/
	bool            m_bFinishData;
	/*
	*人物装备结构
	*/
	struct ROLE_EQUIP
	{
		int iid; //装备的item id
		int amount; //装备的个数
	};
	/*
	*根据唯一id 寻找
	*当前没有装备的所有装备
	*/
	std::map<int,ROLE_EQUIP> m_vAllEquip[5];
	/*
	*当前装备的唯一id
	*/
	int                      m_gCurEquipId[5];
	/*
	*当前装备的item id
	*/
	int                      m_gCurEquipIid[5];
	/*
	*是否还需要刷新数据
	*/
	bool					 m_gIsFulshEquip[5];

	/**
	*3d人物模型
	*/
	Sprite3D * m_character;

	/**
	*装备界面摄像机
	*/
	Camera * m_viewCamera;

	/**
	*玩家属性信息
	*/
	int m_characterInfo[3];

	/**
	*男玩家需要操作的mesh
	*/
	std::vector<std::string> m_manmesh;

	/**
	*女玩家需要操作的mesh
	*/
	std::vector<std::string> m_womanmesh;

	/**
	*已经添加3d角色
	*/
	bool m_alreadyAdd;

	/**
	*角色层
	*/
	Layer * m_characterLayer;
	
	/**
	*点击起始位置
	*/
	Point m_touchBegan;

	//arc ball radius
	float                            _radius;            
	float                            _distanceZ;
	cocos2d::Vec3                    _center;
};
class Character3DModel :public Sprite3D
{
public:
	Character3DModel();
	~Character3DModel();
	static Sprite3D* create(const std::string &modelPath);
	void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
	void onEnter();
	//获取摄像机
	Camera * getCamera(){ 
		CC_SAFE_RETAIN(m_camera);
		return m_camera;
	};
private:
	Camera* m_camera;
};
#endif
