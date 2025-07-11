//================================================================
//
//	Name : RYLCharacterControl.h 
//	Desc : 캐릭터 컨트롤
//  Date : 2004. 05. 17
//
//================================================================
#ifndef __RYL_CHARACTERCONTROL_H__
#define __RYL_CHARACTERCONTROL_H__

#include "RYLObjectControl.h"

class CRYLCharacterControl
{
private :

	CCharacterControl*		m_lpObjControl ;
    RYLCreature*            m_lpCreature;

	CCharacterLightShadowManager::CCharacterDataNode m_pCharacterDataNode ;

	BOOL            m_bLeftMousePress;
	BOOL            m_bRightMousePress;
	BOOL            m_bClickPlayerAndNpc;
	BOOL            m_bClickSkill;

	unsigned long   m_dwNormalTargetID;
	unsigned long   m_dwSpecialTargetID;

	BOOL			m_bLeftClickDown;
	bool			m_bKeyPress;

public :
	
	CRYLCharacterControl( CCharacterControl* lpObjControl ) ;
	~CRYLCharacterControl() ;

	HRESULT			UpdateControl( BOOL bKeyAble = TRUE, BOOL bEdge = FALSE, int nMouseX = 0,int nMouseY = 0 ) ;

	VOID			MouseClickModeSelfCharacterUpdate( BOOL bKeyAble, BOOL bEdge, int nMouseX, int nMouseY ) ;
	RYLCreature*	MouseClickModeSelectSpecialTarget( RYLCreature* pCreature, int nMouseX, int nMouseY );

	VOID			SetCamera( int dx = 0, int dy = 0, int dz = 0 ) ;
	VOID			RestoreCamera() ;

    RYLCreature*    GetCreature()                           { return m_lpCreature; }
	CCharacterControl  *GetCreatureControl()                { return m_lpObjControl; }
    VOID            SetCreature( RYLCreature* pCreature )   { m_lpCreature = pCreature; }

	bool			MakeAttackInfo( unsigned short wSkill = 0, unsigned char cSkillLock = 0, 
									unsigned char cSkillLevel = 0, unsigned char cAtCount = 1, 
									float fAttackAngle = 0.0f, float fAttackRate = 0.0f, 
									BOOL bAngle = TRUE, BOOL bGod = TRUE ) ;

	bool			MakeSkillAttackInfo( unsigned long dwTargetID, unsigned short wSkill = 0, 
		        	                     unsigned char cSkillLock = 0, unsigned char cSkillLevel = 0, 
										 unsigned char cAtCount = 1, BOOL bGod = TRUE, BOOL bDead = FALSE ) ;

	CCharacterLightShadowManager::CCharacterDataNode*	GetCharacterDataNode() { return &m_pCharacterDataNode ; }
	void SetCharacterDataNode( CCharacterLightShadowManager::CCharacterDataNode pCharacterDataNode )
	{
		m_pCharacterDataNode = pCharacterDataNode ;
	}
} ;

#endif //__RYL_CHARACTERCONTROL_H__