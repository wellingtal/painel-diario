//================================================================
//
//	Name : RYLShortDefenceWeaponControl.h 
//	Desc : �ٰŸ� �������� ��Ʈ��
//  Date : 2004. 05. 17
//
//================================================================
#ifndef __RYL_SHORTDEFENCEWEAPONCONTROL_H__
#define __RYL_SHORTDEFENCEWEAPONCONTROL_H__

#include "RYLObjectControl.h"
#include "RYLLongAttackWeaponControl.h"

class CRYLShortDefenceWeaponControl
{
private :
	
	CRYLEaseOutDivideInterpolation	m_EaseOutDivideInterpolation ;
	CZ3DObject*				m_lpWingObject ;
	CZ3DObject*				m_lpLeftTireObject ;			// ���� ����
	CZ3DObject*				m_lpRightTireObject ;			// ������ ����
	FLOAT					m_fTireSpeed ;
	LONG					m_lRealTireRotateUpdate ;  // Update Time 
	FLOAT					m_fRealTireAngle ;		   // ���� ȸ������
	
	LONG					m_lTireRotateUpdate ;	   // �𵨿� ������ Ÿ�̾� Update time
	FLOAT					m_fTireAngle ;		       // �𵨿� ������ Ÿ�̾� ȸ������

	CCharacterControl*		m_lpObjControl ;
    RYLCreature*            m_lpSiegeModel ;				// �������� ũ����

	CCharacterLightShadowManager::CCharacterDataNode m_pCharacterDataNode ;


public :

	CRYLShortDefenceWeaponControl( CCharacterControl* lpObjControl ) ;
	~CRYLShortDefenceWeaponControl() ;

	VOID		SetModifySkileton() ;
	
    RYLCreature*    GetCreature()							{ return m_lpSiegeModel ; }
	void		    SetCreature( RYLCreature* pCreature )   { m_lpSiegeModel = pCreature ; }

	CCharacterLightShadowManager::CCharacterDataNode*	GetCharacterDataNode() { return &m_pCharacterDataNode ; }
	void SetCharacterDataNode( CCharacterLightShadowManager::CCharacterDataNode pCharacterDataNode )
	{
		m_pCharacterDataNode = pCharacterDataNode ;
	}

	HRESULT		UpdateControl( BOOL bKeyAble = TRUE, BOOL bEdge = FALSE, int MouseX = 0,int MouseY = 0 ) ;
	VOID		SetCamera( int dx = 0, int dy = 0, int dz = 0 ) ;

	bool		MakeAttackInfo( unsigned short wSkill = 0, unsigned char cSkillLock = 0, 
							unsigned char cSkillLevel = 0, unsigned char cAtCount = 1, 
							float fAttackAngle = 0.0f, float fAttackRate = 0.0f, 
							BOOL bAngle = TRUE, BOOL bGod = TRUE ) ;

	bool		MakeSkillAttackInfo( unsigned long dwTargetID, unsigned short wSkill = 0, 
		                             unsigned char cSkillLock = 0, unsigned char cSkillLevel = 0, 
									 unsigned char cAtCount = 1, BOOL bGod = TRUE, BOOL bDead = FALSE ) ;

	VOID		SetSiegeModel( unsigned long dwChrID, const char *strName, vector3 vecChrPos, float fDirection ) ;
} ;

#endif //__RYL_SHORTDEFENCEWEAPONCONTROL_H__