#include "UIModelLayer.h"
#include "TVGoodsType.h"
#include "UIBasicLayer.h"
UIModelLayer::UIModelLayer()
{
	m_curSpriteModel = nullptr;
	m_curType = INITIAL_STATUS;
	m_pSender = nullptr;
	m_touchBegan = Vec2(0, 0);
	m_cameraModel = nullptr;
	m_cameraUI = nullptr;
	m_modelNode = nullptr;
	for (int i = 0; i < 4; i++)
	{
		m_models.push_back(nullptr);
		Rect a;
		m_touchAreas.push_back(a);
	}
}
UIModelLayer::~UIModelLayer()
{
	m_curSpriteModel = nullptr;
	m_curType = INITIAL_STATUS;
	m_pSender = nullptr;
	m_touchBegan = Vec2(0, 0);
	m_cameraModel = nullptr;
	m_cameraUI = nullptr;
	m_modelNode = nullptr;
	m_models.clear();
}
bool UIModelLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(UIModelLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(UIModelLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(UIModelLayer::onTouchEnded, this);
	_director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	m_modelNode = Node::create();
	addChild(m_modelNode);
	setName("modelLayer");
	setLocalZOrder(1001);
	return true;
}
UIModelLayer * UIModelLayer::create()
{
	auto model = new UIModelLayer();
	if (model && model->init())
	{
		model->autorelease();
		return model;
	}
	CC_SAFE_DELETE(model);
	return nullptr;
}

void UIModelLayer::addShipModel(Widget * pSender,int modelId,LAYER_TYPE type)
{
	m_curType = type;
	Sprite3D * ship = m_models.at(type);
	m_pSender = pSender;
	if (ship == nullptr)
	{
		ship = Ship3DModel::create(modelId);
		ship->setName("shipmodel");
		ship->setTag(modelId);
		ship->setCameraMask(8);
		m_curSpriteModel = ship;
		ship->setForceDepthWrite(true);
		m_modelNode->addChild(ship);
	}
	else 
	{
		auto name = m_curSpriteModel->getName();
		int tag = m_curSpriteModel->getTag();
		if (tag != modelId)
		{
			ship->removeFromParentAndCleanup(true);
			ship = Ship3DModel::create(modelId);
			ship->setCameraMask(8);
			ship->setName("shipmodel");
			ship->setTag(modelId);
			m_curSpriteModel = ship;
			ship->setForceDepthWrite(true);
			m_modelNode->addChild(ship);
		}
	}
	auto position = pSender->getWorldPosition();
	m_modelNode->setPosition(pSender->getWorldPosition());
	if (type == COMMON_SHIP)
	{
		m_curSpriteModel->setPosition(Vec2(2.2, 1.2));
		m_curSpriteModel->setRotation3D(Vec3(0, -65, 0));
		m_curSpriteModel->setScale(1.0 / 4);
		m_modelNode->setPosition(pSender->getWorldPosition());
//		ship->retain();
		m_models.at(type) = ship;
	}
	if (type == DOCK_SHIP)
	{
		m_curSpriteModel->setPosition(Vec2(3.8, 0.5));
		m_curSpriteModel->setRotation3D(Vec3(0, -65, 0));
		m_curSpriteModel->setScale(0.4);
//		ship->retain();
		m_models.at(type) = ship;
	}
	auto rect = pSender->boundingBox();
	rect.origin.x = pSender->getWorldPosition().x;
	rect.origin.y = pSender->getWorldPosition().y;
	m_touchAreas.at(type) = rect;
	addCamera(type);
}

void UIModelLayer::addCharacterModel(Widget * pSender, LAYER_TYPE type, CharacterEquiment info)
{
	m_curType = type;
	m_pSender = pSender;
	Sprite3D * character = m_models.at(type);
	std::string path;
	path  = getCharacterModePath(info.gender);
	if (character == nullptr)
	{
		character = CharacterModel::create(path,info);
		character->setName("charactermodel");
		character->setTag(10086);
		character->setCameraMask(8);
		m_curSpriteModel = character;
		m_modelNode->addChild(character);
		m_curSpriteModel->setVisible(true);
		m_curSpriteModel->setForceDepthWrite(true);
		auto animation = Animation3D::create(path);
		auto animate = Animate3D::create(animation);
		m_curSpriteModel->runAction(RepeatForever::create(animate));
	}
	else 
	{
		character->removeFromParentAndCleanup(true);
		character = CharacterModel::create(path,info);
		character->setCameraMask(8);
		character->setName("charactermodel");
		character->setTag(10086);
		m_curSpriteModel = character;
		m_modelNode->addChild(character);
		m_curSpriteModel->setVisible(true);
		m_curSpriteModel->setForceDepthWrite(true);
		auto animation = Animation3D::create(path);
		auto animate = Animate3D::create(animation);
		m_curSpriteModel->runAction(RepeatForever::create(animate));
	}
	auto position = pSender->getWorldPosition();
	m_modelNode->setPosition(pSender->getWorldPosition());
	if (type == COMMON_CHARACTER)
	{
		m_curSpriteModel->setScale(0.9);
		m_curSpriteModel->setPosition(Vec2(-1.5, 0.5));
//		character->retain();
		m_models.at(type) = character;
	}
	if (type == CENTER_CHARACTER)
	{
		m_curSpriteModel->setScale(1.0);
//		m_curSpriteModel->setPosition(Vec2(-1.5, 0.5));
//		character->retain();
		m_models.at(type) = character;
	}
	auto rect = pSender->boundingBox();
	rect.origin.x = pSender->getWorldPosition().x;
	rect.origin.y = pSender->getWorldPosition().y;
	m_touchAreas.at(type) = rect;
	addCamera(type);
}

void UIModelLayer::addCamera(LAYER_TYPE type)
{
	//船只camera
	if (m_cameraModel == nullptr && m_cameraUI == nullptr)
	{
		auto cameras = _director->getRunningScene()->getCameras();
		auto camera = cameras.at(cameras.size() - 1);
		auto cameraMask = camera->getCameraFlag();
		Size s = CCDirector::getInstance()->getWinSize();
		auto m_camera = Camera::createPerspective(60, (GLfloat)s.width / s.height, 1, 1000);
		m_camera->setCameraFlag(CameraFlag::USER3);
		m_camera->setDepth(7);
		m_cameraModel = m_camera;
		m_modelNode->addChild(m_camera);
		if ((unsigned int)cameraMask != 4)
		{
			//界面Camera 管理loadingLayer
			auto size = Director::getInstance()->getWinSize();
			Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
			popCam->setCameraFlag(CameraFlag::USER2);
			popCam->setDepth(8);
			m_cameraUI = popCam;
			this->addChild(popCam);
		}
		else
		{
			auto size = Director::getInstance()->getWinSize();
			Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
			popCam->setCameraFlag(CameraFlag::USER4);
			popCam->setDepth(8);
			m_cameraUI = popCam;
			this->addChild(popCam);
		}

	}
	m_cameraModel->lookAt(m_curSpriteModel->getPosition3D(), Vec3(0, 1, 0));
	m_cameraModel->setRotation3D(Vec3(0, 0, 0));
	switch (type)
	{
	case CENTER_CHARACTER:
		m_cameraModel->setPosition3D(Vec3(0.4, 2.4, 6.5));
		break;
	case COMMON_CHARACTER:
		m_cameraModel->setPosition3D(Vec3(0.4, 2.4, 6.5));
		break;
	case DOCK_SHIP:
		m_cameraModel->setPosition3D(Vec3(0.4, 2.5, 6.5));
		break;
	case COMMON_SHIP:	
		m_cameraModel->setPosition3D(Vec3(0.4, 2.5, 6.5));
		break;
	default:
		break;
	}

}
bool UIModelLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	auto rect = m_touchAreas.at(m_curType);
	auto pos = Vec2(rect.origin.x,rect.origin.y);
	float p_left = pos.x - rect.size.width / 2;
	float p_right = pos.x + rect.size.width / 2;
	float p_top = pos.y + rect.size.height / 2;
	float p_bottom = pos.y - rect.size.height / 2;
	if (touch->getLocation().x >= p_left && touch->getLocation().x <= p_right && touch->getLocation().y >= p_bottom && touch->getLocation().y <= p_top)
	{
		m_touchBegan = touch->getLocation();
	}
	return true;
}
void UIModelLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
	Point p1 = touch->getLocation();
	if (m_touchBegan != Vec2(0, 0))
	{
		if (p1.x - m_touchBegan.x > 0)
		{
			m_curSpriteModel->setRotation3D(m_curSpriteModel->getRotation3D() + Vec3(0, 5, 0));
		}
		else if (p1.x - m_touchBegan.x < 0)
		{
			m_curSpriteModel->setRotation3D(m_curSpriteModel->getRotation3D() + Vec3(0, -5, 0));
		}
		m_touchBegan = p1;
	}

	log("hehe");
	Vec3 pos = m_curSpriteModel->getPosition3D();
	Vec3 roat = m_curSpriteModel->getRotation3D();
	log("x ::%.2f, y :: %.2f , z :: %.2f", pos.x, pos.y, pos.z);
	log("x ::%.2f, y :: %.2f , z :: %.2f", roat.x, roat.y, roat.z);
}
void UIModelLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
	m_touchBegan = Vec2(0, 0);
}

Ship3DModel::Ship3DModel()
{
	m_flag_index = 0;
	m_oars_index = 0;
}
Ship3DModel::~Ship3DModel()
{

}
bool Ship3DModel::init(int modelId)
{
	if (!Sprite3D::initWithFile(getShipTestModePath_3D(modelId)))
	{
		return false;
	}
	addSail(this);
	return true;
}
void Ship3DModel::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (isVisitableByVisitingCamera()){
		Mat4 transform = parentTransform;
#if  0
		std::vector<Node*> myParents;
		Node* start = this->getParent();
		while (start){
			myParents.push_back(start);
			start = start->getParent();
		}
		std::vector<Node*>::iterator iter;
		for (iter = myParents.end() - 1; iter != myParents.begin() - 1; iter--)
		{
			Node*node = *iter;
			// 			parentFlags = node->processParentFlags(transform, parentFlags);
			// 			transform = node->getTransform;
		}
#endif
		Sprite3D::visit(renderer, transform, parentFlags);
	}
	else{

	}
}
Ship3DModel* Ship3DModel::create(int modelId)
{
	auto model = new Ship3DModel;
	CC_RETURN_IF(model);
	if (model && model->init(modelId))
	{
		model->autorelease();
		return model;
	}
	CC_SAFE_DELETE(model);
	return nullptr;
}
void Ship3DModel::addSail(Sprite3D * p_Sender)
{
	auto skele = p_Sender->getSkeleton();
	if (skele)
	{
		int boneNum = skele->getBoneCount();
		for (int i = 0; i < boneNum; i++)
		{
			auto bone = skele->getBoneByIndex(i);
			auto attach = p_Sender->getAttachNode(bone->getName());
			attach->addChild(confirmSail(bone));
		}
	}
}

Sprite3D * Ship3DModel::confirmSail(Bone3D * bone)
{
	auto name = bone->getName();
	std::vector<std::string> nameInfos;
	nameInfos = analysisName(name);
	Sprite3D * sp = Sprite3D::create();
	int a = atoi(nameInfos.at(3).c_str());
	float scaleValue = float(a) / 100;
	if (nameInfos.at(1).find("flag") != -1)
	{

		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		sp->setName(nameInfos.at(0) + "_" + nameInfos.at(1));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));
		}
		for (int a = 0; a < sp->getMeshCount(); a++)
		{
			auto mesh = sp->getMeshByIndex(a);
			if (a == m_flag_index)
			{
				mesh->setVisible(true);
				continue;
			}
			mesh->setVisible(false);
		}
	}
	else if (nameInfos.at(1).find("sail") != -1)
	{
		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));
		}
	}
	else if (nameInfos.at(1).find("oar") != -1)
	{
		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));

		}
		sp->setName(nameInfos.at(0) + "_" + nameInfos.at(1));
		/*		m_oars.pushBack(sp);*/
		for (int a = 0; a < sp->getMeshCount(); a++)
		{
			auto mesh = sp->getMeshByIndex(a);
			if (a == m_oars_index)
			{
				mesh->setVisible(true);
				continue;
			}
			mesh->setVisible(false);
		}
	}
	//	sp->setTag(2);
	sp->setCameraMask(8);
	return sp;
}
std::vector<std::string> Ship3DModel::analysisName(std::string name)
{
	std::string str = name;
	std::vector<std::string> nameInfos;
	while (str.find_first_of("_") != -1)
	{
		int a = str.find_first_of("_");
		std::string str1 = str.substr(0, a);
		nameInfos.push_back(str1);
		str = str.substr(a + 1, str.npos);
		if (str.find_first_of("_") == -1)
		{
			nameInfos.push_back(str);
		}
	}
	return nameInfos;
}

void Ship3DModel::changeMeshAndColor(int meshindex, int colorIndex, MESH_TYPE type)
{

}


std::string manmesh_name[] =
{
	//帽子
	"man_hats_M_100_1",
	//衣服
	"man_frock_M_100_1",
	"man_pants_M_100_1",
	//项链
	"man_necklace_M_100_1",
	//鞋子
	"man_shoe_M_100_1",
};
std::string womanmesh_name[]
{
	//帽子
	"woman_hats_M_100_1",
	//衣服
	"woman_frock_M_100_1",
	"woman_pants_M_100_1",
	//项链
	"woman_necklace_M_100_1",
	//鞋子
	"woman_shoe_M_100_1",
	//手套
	"woman_gloves_M_100_1",
};
CharacterModel::CharacterModel()
{

}
CharacterModel::~CharacterModel()
{

}
CharacterModel *CharacterModel::create(const std::string &modelPath, CharacterEquiment info)
{
	if (modelPath.length() < 4)
		CCASSERT(false, "invalid filename for Sprite3D");

	auto sprite = new (std::nothrow) CharacterModel();
	if (sprite && sprite->initWithFile(modelPath,info))
	{
		sprite->_contentSize = sprite->getBoundingBox().size;
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}
bool CharacterModel::initWithFile(const std::string &path, CharacterEquiment info)
{
	if (!Sprite3D::initWithFile(path))
	{
		return false;
	}
	m_euqipmentInfo = info;
	showArmors(info.clothId, MESH_TYPE_CLOTHES);
	showArmors(info.hatId, MESH_TYPE_HATS);
	showArmors(info.necklaceId, MESH_TYPE_NECKLACE);
	showArmors(info.shoesId, MESH_TYPE_SHOES);
	showArmors(info.weaponId, MESH_TYPE_WEAPON);
	confirmCharacterFaceAndHair();
	return true;
}

void CharacterModel::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (isVisitableByVisitingCamera()){
		Mat4 transform = parentTransform;
#if 0
		std::vector<Node*> myParents;
		Node* start = this->getParent();
		while (start){
			myParents.push_back(start);
			start = start->getParent();
		}
		std::vector<Node*>::iterator iter;
		for (iter = myParents.end() - 1; iter != myParents.begin() - 1; iter--)
		{
			Node*node = *iter;
			parentFlags = node->processParentFlags(transform, parentFlags);
			transform = node->getModelViewTransform();
		}
#endif
		Sprite3D::visit(renderer, transform, parentFlags);
	}
	else{

	}
}
void CharacterModel::addWeapon(int iid)
{
//iid 
 	auto sp_weapon = Sprite3D::create("Character3D/weaponA.c3b");
	auto skeleton = this->getSkeleton();
 	auto bone = skeleton->getBoneByName("joint_weapon_R_100_1");
	auto attach = this->getAttachNode(bone->getName());
 	//添加武器
 	if (attach->getChildren().size() == 0 && iid != 0)
 	{
 		sp_weapon->setCameraMask(8, true);
 		attach->addChild(sp_weapon);
	}
	else if( iid == 0)
	{
		if (attach->getChildren().size() > 0)
		{
			auto child = attach->getChildren().at(0);
			attach->removeChild(child, true);
		}			 
	}
}
void CharacterModel::showArmors(int iid, MESH_TYPE meshType)
{
//衣服
	int gender = m_euqipmentInfo.gender;
	if (gender == 1 && meshType == MESH_TYPE_CLOTHES)
	{
		auto meshcloth = this->getMeshByName(manmesh_name[MESH_CLOTHES_INDEX]);
		auto meshpants = this->getMeshByName(manmesh_name[MESH_PANTS_INDEX]);
		meshcloth->setVisible(true);
		meshpants->setVisible(true);
		if (iid <= 0)
		{
			meshcloth->setVisible(false);
			meshpants->setVisible(false);
		}
	}
	else if (gender == 2 && meshType == MESH_TYPE_CLOTHES)
	{
		auto meshcloth = this->getMeshByName(womanmesh_name[MESH_CLOTHES_INDEX]);
		auto meshgloves = this->getMeshByName(womanmesh_name[MESH_GLOVES_INDEX]);
		auto meshpants = this->getMeshByName(womanmesh_name[MESH_PANTS_INDEX]);
		meshcloth->setVisible(true);
		meshgloves->setVisible(true);
		meshpants->setVisible(true);
		if (iid <= 0)
		{
			meshcloth->setVisible(false);
			meshgloves->setVisible(false);
			meshpants->setVisible(false);
		}
	}

// 	帽子
	if (meshType == MESH_TYPE_HATS)
	{
		if (gender == 1)
		{
			auto mesh = this->getMeshByName(manmesh_name[MESH_HATS_INDEX]);
			mesh->setVisible(true);
			if (iid <= 0 )
			{
				mesh->setVisible(false);
			}
		}
		else
		{
			auto mesh = this->getMeshByName(womanmesh_name[MESH_HATS_INDEX]);
			mesh->setVisible(true);
			if (iid <= 0)
			{
				mesh->setVisible(false);
			}
		}
	}
	if (meshType == MESH_TYPE_SHOES)
	{
		if (gender == 1)
		{
			auto mesh = this->getMeshByName(manmesh_name[MESH_SHOES_INDEX]);
			mesh->setVisible(true);
			if (iid <= 0)
			{
				mesh->setVisible(false);
			}
		}
		else
		{
			auto mesh = this->getMeshByName(womanmesh_name[MESH_SHOES_INDEX]);
			mesh->setVisible(true);
			if (iid <= 0)
			{
				mesh->setVisible(false);
			}
		}
		
	}
	if (meshType == MESH_TYPE_NECKLACE)
	{
		if (gender == 1)
		{
			auto mesh = this->getMeshByName(manmesh_name[MESH_NECKLACE_INDEX]);
			mesh->setVisible(false);
			if (iid <= 0)
			{
				mesh->setVisible(false);
			}
		}
		else
		{
			auto mesh = this->getMeshByName(womanmesh_name[MESH_NECKLACE_INDEX]);
			mesh->setVisible(false);
			if (iid <= 0)
			{
				mesh->setVisible(false);
			}
		}
	}
	if (meshType == MESH_TYPE_WEAPON)
	{
		addWeapon(iid);
	}
}
void CharacterModel::addOrnament()
{
	auto sextant = Sprite3D::create("Character3D/sextant.c3b");
	sextant->setCameraMask(8, true);
	sextant->setVisible(false);
	auto attach = this->getAttachNode("joint_hem_LM_100_1");
	attach->addChild(sextant);
}
void CharacterModel::confirmCharacterFaceAndHair()
{
	std::string face;
	std::string hair;
	int icon = m_euqipmentInfo.iconindex;
	if (icon <= 4)
	{
		face = "man_face_M_100";
		hair = "man_hair_M_100";
	}
	else
	{
		face = "woman_face_M_100";
		hair = "woman_hair_M_100";
	}

	for (int i = 0; i < this->getMeshCount(); i++)
	{
		auto mesh = this->getMeshByIndex(i);
		if (mesh->getName().find(face) == 0)
		{
			std::string str_face = StringUtils::format("%s_%d", face.c_str(), icon);
			log("Mesh name:%s", str_face.c_str());
			if (mesh->getName().find(str_face) == 0)
			{
				mesh->setVisible(true);
				log("LOGTEST:head is show");
				log("Mesh name:%s", str_face.c_str());
			}
			else
			{
				mesh->setVisible(false);
			}
		}
		if (mesh->getName().find(hair) == 0)
		{
			std::string str_hair = StringUtils::format("%s_%d", hair.c_str(), icon);
			log("Mesh name:%s", str_hair.c_str());
			if (mesh->getName().find(str_hair) == 0)
			{
				mesh->setVisible(true);
				log("LOGTEST:head is show");
				log("Mesh name:%s", str_hair.c_str());
			}
			else
			{
				mesh->setVisible(false);
			}
		}
	}
}