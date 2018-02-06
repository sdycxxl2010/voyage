#ifndef __UIMODELLAYER_H
#define __UIMODELLAYER_H
#include "TVBasic.h"
#include "login.pb-c.h"
#include "ProtocolThread.h"
#include "TVSingle.h"
#include <unordered_map>
#include "EnumList.h"

USING_NS_CC;
enum LAYER_TYPE
{
	//初始化
	INITIAL_STATUS = -1,
	//个人中心人物模型
	CENTER_CHARACTER,
	//通用界面任务模型
	COMMON_CHARACTER,
	//船坞船只模型
	DOCK_SHIP,
	//通用界面船只模型
	COMMON_SHIP,
};

enum MESH_TYPE
{
	//船体
	SHIP_BODY_MESH,
	//船帆
	SHIP_SAIL_MESH,
	//旗帜
	SHIP_FLAG_MESH,
};

struct CharacterEquiment
{
	int clothId;
	int hatId;
	int necklaceId;
	int shoesId;
	int weaponId;
	int gender;
	int iconindex;
};
class UIModelLayer : public Layer
{
public:

	UIModelLayer();
	~UIModelLayer();
	bool init();
	//pSender 模型显示位置对应的控件
	static UIModelLayer * create();

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);

	void addShipModel(Widget * pSender, int modelId, LAYER_TYPE type);
	
	void addCharacterModel(Widget * pSender, LAYER_TYPE type, CharacterEquiment info);

	void addCamera(LAYER_TYPE type);

	std::vector<Sprite3D *> getModels(){ return m_models;};
	LAYER_TYPE getCurrentType(){ return m_curType;};
	Sprite3D * getCurModel(){ return m_curSpriteModel;};
	void setLayerType(LAYER_TYPE type){ m_curType = type; };
private:
	std::vector<Sprite3D *> m_models;
	std::vector<Rect> m_touchAreas;
	LAYER_TYPE m_curType;
	Sprite3D * m_curSpriteModel;
//模型的背景
	Widget * m_pSender;

	Vec2 m_touchBegan;
//模型相机
	Camera * m_cameraModel;
//UI相机
	Camera * m_cameraUI;
//模型所在的node用来确定model的位置
	Node * m_modelNode;

};
class Ship3DModel :public Sprite3D
{
public:
	Ship3DModel();
	~Ship3DModel();
	bool init(int modelId);
	static Ship3DModel* create(int modelId);
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
//todo
	void changeMeshAndColor(int meshindex, int colorIndex,MESH_TYPE type);

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
class CharacterModel :public Sprite3D
{
public:

	enum CHARACTER_MESH_INDEX
	{
		//帽子贴图
		MESH_HATS_INDEX,
		//上衣贴图(衣服)
		MESH_CLOTHES_INDEX,
		//裤子贴图(衣服)
		MESH_PANTS_INDEX,
		//饰品
		MESH_NECKLACE_INDEX,
		//鞋
		MESH_SHOES_INDEX,
		//手套
		MESH_GLOVES_INDEX,
	};
	enum MESH_TYPE
	{
		//上衣贴图(衣服)
		MESH_TYPE_CLOTHES,
		//帽子贴图
		MESH_TYPE_HATS,
		//饰品
		MESH_TYPE_NECKLACE,
		//鞋
		MESH_TYPE_SHOES,
		//武器
		MESH_TYPE_WEAPON,
	};
	CharacterModel();
	~CharacterModel();
	static CharacterModel* create(const std::string &modelPath, CharacterEquiment info);
	void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
	bool initWithFile(const std::string &path, CharacterEquiment info);
	void addWeapon(int iid);

	void showArmors(int iid,MESH_TYPE meshType);
	//添加挂饰 不同于装备
	void addOrnament();

	void confirmCharacterFaceAndHair();

private:
	CharacterEquiment m_euqipmentInfo;
};
#endif