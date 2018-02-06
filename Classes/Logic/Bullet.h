/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月30日
 *  Author:Sen
 *  bullet
 *
 */

#ifndef __BULLET__
#define __BULLET__

#include "TVBasic.h"
#include "TVBattleDelegate.h"

USING_NS_CC;

class Bullet : public Node
{
public:
	Bullet();
	~Bullet();
	void onEnter() override;
	void onExit() override;
	static Bullet* createBullet(Node* parent,int id,int iid);
	bool init();
	/*设置炮弹的属性
	*info：属性的结构体
	*/
	void setBulletInfo(Ship_info* info);
	/*设置炮弹与目标的距离
	*dircent：距离
	*/
	void setOffsetDirect(float dircent);
	/*设置炮弹速度
	*speed：速度
	*/
	void setTargetSpeed(float speed);
	/*设置炮弹目标
	*speed：速度
	*/
	void setDestination(Vec2 des);
	/*获取炮弹目标
	*Vec2：坐标
	*/
	Vec2 getDestination();
	/*设置代理
	*pDelegate：代理
	*/
	void setDelegate(TVBattleDelegate *pDelegate);
	/*显示炮弹
	*
	*/
	void show();
	/*炮弹icon（占时没有用到）
	*id:根据id设置icon
	*/
	void setIconByTag(int id);
	/*获取炮弹的攻击（占时没有用到）
	*
	*/
	int	 getAttack();
	/*获取炮弹的信息
	*
	*/
	Ship_info* getBulletInfo();
	/*炮弹结束判断
	*主要是实现炮弹是否打到船只即炮弹的特效
	*/
	void sendFinsh();
	/*
	*删除炮弹
	*/
	void removeEffective(Node* target);

	float getTargetDirect(){return m_fOffsetDirect;}
	/*
	 * 设置攻击力
	 */
	void setAttack(int attack);
	/*
	* 获取炮弹的iid
	*/
	int getCannonid();
protected:
	virtual void update(float delta);

private:
	ParticleSystemQuad *m_pPsq;
	Sprite  *m_pIcon;
	Node	*m_pParent;
	Vec2	m_DesPos;

	int		m_nAttack;
	float	m_fOffsetDirect;
	float		m_nTargetSpeed;
	TVBattleDelegate *m_pDelegate;
	Ship_info *m_pOwnInfo;
	float	m_fMoveTime;
	int     m_cannonid;
};

#endif 
