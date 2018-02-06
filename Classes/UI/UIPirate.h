/*
 * AttackPirate.h
 *
 *  Created on: Dec 12, 2015
 *      Author: xijing
 * ������Ϯ����������
 */

#ifndef __ATTACK__PIRATE__
#define __ATTACK__PIRATE__

#include "cocos2d.h"
#include "UIBasicLayer.h"
using namespace cocos2d;

class UIPriate : public UIBasicLayer
{
public:
	UIPriate();
	virtual ~UIPriate();
	/**
	* ��ʼ�����
	*����:pAttactPirateResult ����������� pPirateAttactResult��������Ϯ
	*/
	bool init(struct _GetAttackPirateInfoResult *pAttactPirateResult, struct _GetPirateAttackInfoResult *pPirateAttactResult);
	/**
	*m_pAttactPirateResult ��������������
	*/
	struct _GetAttackPirateInfoResult *m_pAttactPirateResult;
	/**
	*m_pPirateAttactResult ������Ϯ���
	*/
	struct _GetPirateAttackInfoResult *m_pPirateAttactResult;
	/**
	*�������������
	*/
	void onServerEvent(struct ProtobufCMessage *message,int msgType);
	/**
	*��ť����¼�
	*/
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);

private:
	/**
	* ������붯��
	*����:fTime �ӳ�ʱ��
	*/
	void attackPirateGuardExitScene(const float fTime);
	/**
	* �����˳�����
	*����:fTime �ӳ�ʱ��
	*/
	void attackPirateGuardEnterScene(const float fTime);
	/**
	* ����ʱ(ÿ�����һ��)
	*����:fTime �ӳ�ʱ��
	*/
	void updateBySecond(const float fTime);
	/**
	* �ı�ѡ��ť��״̬
	*����:fTime �ӳ�ʱ��
	*/
	void changeMainButtonState(const float fTime);
	/**
	* �˶��������״̬���ر���Ļ��Ľ���
	*/
	void attackMoveEnd(){ m_bAttackMoveActionFlag = true; closeOtherView(0); };
	/**
	* �ر���Ļ��Ľ���
	*����:fTime �ӳ�ʱ��
	*/
	void closeOtherView(const float fTime);
	/**
	* �����������--boss����
	*����:fTime �ӳ�ʱ��
	*/
	void openBoss(const float fTime);
	/**
	* �����������--���н���
	*����:fTime �ӳ�ʱ��
	*/
	void openRanking(const float fTime);
	/**
	* �����������--�������
	*����:fTime �ӳ�ʱ��
	*/
	void openRule(const float fTime);

	/**
	* ��������--ѡ���ѶȽ���
	*����:fTime �ӳ�ʱ��
	*/
	void openChooseLevel(const float fTime);
	/**
	* ��������--���˵������
	*����:fTime �ӳ�ʱ��
	*/
	void openFeatsOfIntegral(const float fTime);
	/**
	* ��������--�������
	*����:fTime �ӳ�ʱ��
	*/
	void openPirateAttackRule(const float fTime);
private:
	/**
	*���ڼ�¼ѡ��ť(������İ�ť��״̬)
	*/
	Widget *m_pMainButton;
	/**
	*���ڼ�¼�򿪵Ľ���(��������)
	*/
	Widget *m_pView;
	/**
	*�������˶�ʱȥ�����¼�
	*true:���Ե�� false:�����Ե��
	*/
	bool    m_bAttackMoveActionFlag;
	/**
	*�������--boss�����
	*/
	GetAttackPirateBossInfoResult * m_pBossResult;
	/**
	*�������--���е����
	*/
	GetAttackPirateRankInfoResult * m_pRankResult;
	/**
	*�Ƿ��ǹ���������� 
	*false:�������� true:�����������
	*/
	bool m_bIsAttackPirate;
};


#endif /* ATTACKPIRATE_H_ */
