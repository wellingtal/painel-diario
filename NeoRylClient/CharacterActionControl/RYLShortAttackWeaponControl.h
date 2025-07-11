//================================================================
//
//	Name : RYLShortAttackWeaponControl.h 
//	Desc : 근거리 공성병기 컨트롤
//  Date : 2004. 05. 17
//
//================================================================
#ifndef __RYL_SHORTATTACKWEAPONCONTROL_H__
#define __RYL_SHORTATTACKWEAPONCONTROL_H__

#include "RYLObjectControl.h"
#include "RYLLongAttackWeaponControl.h"

class CCreture ;

class CRYLShortAttackWeaponControl
{
private :

	CCharacterControl*		m_lpObjControl ;
	CZ3DObject*				m_lpLeftTireObject ;			// 왼쪽 바퀴
	CZ3DObject*				m_lpRightTireObject ;			// 오른쪽 바퀴
	CZ3DObject*				m_lpTailTireObject ;			// 뒷 바퀴
	CZ3DObject*				m_lpBodyObject ;			    // 뒷 바퀴

    RYLCreature*            m_lpSiegeModel;				    // 공성병기 크리쳐
	
	unsigned char			m_byVelocity[ 228 ] ;
	unsigned int			m_iVelocityControl ;
	CRYLEaseOutDivideInterpolation	m_EaseOutDivideInterpolation ;


public :

	FLOAT					m_fTireSpeed ;
	LONG					m_lRealTireRotateUpdate ;  // Update Time 
	FLOAT					m_fRealTireAngle ;		   // 실제 회전각도
	
	LONG					m_lTireRotateUpdate ;	   // 모델에 부착된 타이어 Update time
	FLOAT					m_fTireAngle ;		       // 모델에 부착된 타이어 회전각도

	FLOAT					m_fBodyAngle ;				// 몸통 회전
	float					m_fVelocity ;

	CCharacterLightShadowManager::CCharacterDataNode m_pCharacterDataNode ;

	long					m_lTireUpdate ;
	long					m_lVelocityUpdate ;
	long					m_lTireAngleUpdate ;
	long					m_lBodyAngleUpdate ;
	long					m_lRealAngleUpdate ;

public :

	CRYLShortAttackWeaponControl( CCharacterControl* lpObjControl ) ;
	~CRYLShortAttackWeaponControl() ;

	HRESULT		UpdateControl( BOOL bKeyAble = TRUE, BOOL bEdge = FALSE, int MouseX = 0,int MouseY = 0 ) ;
	VOID		SetCamera( int dx = 0, int dy = 0, int dz = 0 ) ;

	VOID		SetModifySkileton() ;
	VOID		SetSiegeModel( unsigned long dwChrID, const char *strName, vector3 vecChrPos, float fDirection ) ;

    RYLCreature*    GetCreature()							{ return m_lpSiegeModel ; }
	void		    SetCreature( RYLCreature* pCreature )   { m_lpSiegeModel = pCreature ; }

	CCharacterLightShadowManager::CCharacterDataNode*	GetCharacterDataNode() { return &m_pCharacterDataNode ; }
	void SetCharacterDataNode( CCharacterLightShadowManager::CCharacterDataNode pCharacterDataNode )
	{
		m_pCharacterDataNode = pCharacterDataNode ;
	}

	bool		MakeAttackInfo( unsigned short wSkill = 0, unsigned char cSkillLock = 0, 
							unsigned char cSkillLevel = 0, unsigned char cAtCount = 1, 
							float fAttackAngle = 0.0f, float fAttackRate = 0.0f, 
							BOOL bAngle = TRUE, BOOL bGod = TRUE ) ;

	bool		MakeSkillAttackInfo( unsigned long dwTargetID, unsigned short wSkill = 0, 
		                             unsigned char cSkillLock = 0, unsigned char cSkillLevel = 0, 
									 unsigned char cAtCount = 1, BOOL bGod = TRUE, BOOL bDead = FALSE ) ;
} ;

#endif //__RYL_SHORTATTACKWEAPONCONTROL_H__