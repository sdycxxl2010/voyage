#ifndef __NPCSHIP_H__
#define __NPCSHIP_H__
#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;
#define TopWidgetOffset 10

class TVSeaEvent;
class  UINPCShip : public Widget
{
public:

	struct NPCShipInfo
	{
		int i_eventId;
		int i_id;
		int i_type;
		int i_direction;
		int i_country;
		int i_shipLevel;
		int i_level;
		bool b_isFridend;
		std::string shipTexture;
		std::string s_name;
		bool b_isActiveAttack;
		bool b_isTaskNpc;
		bool IsNeutralChange;//是否由中立变敌对
	};

	UINPCShip();
	virtual ~UINPCShip();

	static UINPCShip* create();
	static UINPCShip* create(NPCShipInfo npcShipInfo);
	bool flushInit(NPCShipInfo npcShipInfo);
	bool getIsFriend(){ return m_isFridend; }
	bool getIsActiveAttack(){return m_isActiveAttack;}
	void setAciveAttack(bool activeAttack) {m_isActiveAttack = activeAttack;}
	int getAverageLevel() { return m_level; }
	int getNpcId() {return i_id;}
	int getNpcCountryId(){return i_country;}
	void isShowGuardCircle(bool isTure, float f);
	ImageView* getAttackCircle(){ return m_attackCircle; }
	bool AttackCirclehitTest(const Vec2 &pt);
	int getEventID();
	int getNpcType(){return i_npcType;};
	void setNpcType(int npctype){i_npcType=npctype;};
	void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
	virtual void setCameraMask(unsigned short mask, bool applyChildren);
	int n;
	TVSeaEvent*m_eventData;
protected:

private:

	virtual bool init();
	virtual bool init(NPCShipInfo npcShipInfo);

public:

	enum NPCSHIP_DIRECT
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		L_UP,
		L_DOWN,
		R_UP,
		R_DOWN,
	};
	ImageView* m_shipTexture;

protected:

private:

	int i_id;
	int i_type;
	int i_direction;
	int i_country;
	//int m_minLevel;
	//int m_maxLevel;
	int m_level;
	bool m_isFridend;
	bool m_isActiveAttack;
	bool m_isScale;
	int i_eventId;
	int i_npcType;
	Widget* w_topWidget;
	Text* t_name;
	Text* t_level;
	std::string s_shipTexture;

	ImageView* m_country;
	ImageView* m_guardCircle;
	ImageView* m_attackCircle;
	ImageView* m_isFriendImage;
	ImageView* supply_2day;

};


#endif
