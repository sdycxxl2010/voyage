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
	//��ʼ��
	INITIAL_STATUS = -1,
	//������������ģ��
	CENTER_CHARACTER,
	//ͨ�ý�������ģ��
	COMMON_CHARACTER,
	//���봬ֻģ��
	DOCK_SHIP,
	//ͨ�ý��洬ֻģ��
	COMMON_SHIP,
};

enum MESH_TYPE
{
	//����
	SHIP_BODY_MESH,
	//����
	SHIP_SAIL_MESH,
	//����
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
	//pSender ģ����ʾλ�ö�Ӧ�Ŀؼ�
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
//ģ�͵ı���
	Widget * m_pSender;

	Vec2 m_touchBegan;
//ģ�����
	Camera * m_cameraModel;
//UI���
	Camera * m_cameraUI;
//ģ�����ڵ�node����ȷ��model��λ��
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
	*��Ӵ�����
	*@param p_sender ��ǰ���صĴ�ֻ
	*/
	void addSail(Sprite3D * p_Sender);
	/**
	*������ȷ��
	*@parm bone ��Ҫ��Ӳ����Ĺ���
	*/
	Sprite3D * confirmSail(Bone3D * bone);

	/*
	* ��ȡ�ǵ�������Ϣ
	* @param name ��������
	*/
	std::vector<std::string> analysisName(std::string name);
//todo
	void changeMeshAndColor(int meshindex, int colorIndex,MESH_TYPE type);

protected:
	/**
	*�������
	*/
	int m_oars_index;
	/**
	*����Mesh���
	*/
	int m_flag_index;
};
class CharacterModel :public Sprite3D
{
public:

	enum CHARACTER_MESH_INDEX
	{
		//ñ����ͼ
		MESH_HATS_INDEX,
		//������ͼ(�·�)
		MESH_CLOTHES_INDEX,
		//������ͼ(�·�)
		MESH_PANTS_INDEX,
		//��Ʒ
		MESH_NECKLACE_INDEX,
		//Ь
		MESH_SHOES_INDEX,
		//����
		MESH_GLOVES_INDEX,
	};
	enum MESH_TYPE
	{
		//������ͼ(�·�)
		MESH_TYPE_CLOTHES,
		//ñ����ͼ
		MESH_TYPE_HATS,
		//��Ʒ
		MESH_TYPE_NECKLACE,
		//Ь
		MESH_TYPE_SHOES,
		//����
		MESH_TYPE_WEAPON,
	};
	CharacterModel();
	~CharacterModel();
	static CharacterModel* create(const std::string &modelPath, CharacterEquiment info);
	void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
	bool initWithFile(const std::string &path, CharacterEquiment info);
	void addWeapon(int iid);

	void showArmors(int iid,MESH_TYPE meshType);
	//��ӹ��� ��ͬ��װ��
	void addOrnament();

	void confirmCharacterFaceAndHair();

private:
	CharacterEquiment m_euqipmentInfo;
};
#endif