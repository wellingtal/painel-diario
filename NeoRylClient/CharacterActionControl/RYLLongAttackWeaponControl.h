//================================================================
//
//	Name : RYLLongAttackWeaponControl.h 
//	Desc : 원거리 공성병기 컨트롤
//  Date : 2004. 05. 17
//
//================================================================
#ifndef __RYL_LONGATTACKWEAPONCONTROLL_H__
#define __RYL_LONGATTACKWEAPONCONTROLL_H__

#include "RYLObjectControl.h"

typedef struct __SDiffuseVertex 
{
	D3DXVECTOR3 Position ;
	DWORD Color;
} SDiffuseVertex;

#define FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

class CRYLEaseOutShiftInterpolation
{
private :
	int _value ;
	int _target ;
	int _shift ;

public :
	bool	Setup( int from, int to, int shift )
	{
		if ( shift <= 0 )
		{
			return false ;
		}

		_value = from ;
		_target = to ;
		_shift = shift ;

		return true ;
	}

	// 시간에 의존적이지 않음
	bool Interpolate()
	{
		int oldValue = _value ;
		if ( _shift > 0 )
		{
			_value = ( _value * ( ( 1 << _shift ) - 1 ) + _target ) >> _shift ;
		}

		return ( _value == oldValue ) ;
	}

	int GetValue()
	{
		return _value ;
	}
} ;


//=================================================================================
class CRYLEaseOutDivideInterpolation : public CRYLEaseOutShiftInterpolation
{
public :
	float _value ;
	float _target ;
	float _divisor ;

public :
	bool Setup( float from, float to, float divisor )
	{
		if ( divisor <= 0 )
		{
			return false ;
		}

		_value = from ;
		_target = to ;
		_divisor = divisor ;
		
		return true ;
	}

	// 시간에 의존적이지 않음
	bool Interpolate()
	{
		float oldValue = _value ;

		if ( _divisor > 0 ) 
		{
			_value = ( _value * ( _divisor - 1.0f ) + _target ) / _divisor ;
		}

		return ( _value == oldValue ) ;
	}

	float GetValue()
	{
		return _value ;
	}
} ;

class CRYLLongAttackWeaponControl
{
private :

	CZ3DObject*				m_lpLeftTireObject ;			// 왼쪽 바퀴
	CZ3DObject*				m_lpRightTireObject ;			// 오른쪽 바퀴
	CZ3DObject*				m_lpBodyObject ;
	FLOAT					m_fBodyAngle ;				// 몸통 회전
	float					m_fVelocity ;

    RYLCreature*            m_lpSiegeModel;
	
	CCharacterControl*		m_lpObjControl ;
	CRYLEaseOutDivideInterpolation	m_EaseOutDivideInterpolation ;

	FLOAT					m_fTireSpeed ;
	LONG					m_lRealTireRotateUpdate ;  // Update Time 
	FLOAT					m_fRealTireAngle ;		   // 실제 회전각도
	
	LONG					m_lTireRotateUpdate ;	   // 모델에 부착된 타이어 Update time
	FLOAT					m_fTireAngle ;		       // 모델에 부착된 타이어 회전각도

	LONG					m_lFlowLine ;
	D3DXVECTOR3				m_v3BombPos[ 100 ] ;
	SDiffuseVertex			m_DiffuseVertex[ 100 ] ;
	LPDIRECT3DVERTEXBUFFER8	m_pVertexBuffer ;
	CEffScript *test_script ;
	
	CCharacterLightShadowManager::CCharacterDataNode m_pCharacterDataNode ;
 
	long					m_lTireUpdate ;
	long					m_lVelocityUpdate ;
	long					m_lTireAngleUpdate ;
	long					m_lBodyAngleUpdate ;
	long					m_lRealAngleUpdate ;

public :
	
	CRYLLongAttackWeaponControl( CCharacterControl* lpObjControl ) ;
	~CRYLLongAttackWeaponControl() ;

	HRESULT		UpdateControl( BOOL bKeyAble = TRUE, BOOL bEdge = FALSE, int MouseX = 0,int MouseY = 0 ) ;
	VOID		SetCamera( int dx = 0, int dy = 0, int dz = 0 ) ;
	void		Render( LPDIRECT3DDEVICE8 pd3dDevice ) ;

	VOID		SetModifySkileton() ;
	VOID		SetSiegeModel( unsigned long dwChrID, const char *strName, vector3 vecChrPos, float fDirection ) ;

	HRESULT		CreateBombLine( vector3& v3Start, vector3& v3Dest) ;
	double		CalcBi_n( INT n, INT i, DOUBLE u )  ;

    RYLCreature*	GetCreature()							{ return m_lpSiegeModel ; }
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

#endif //__RYL_LONGATTACKWEAPONCONTROLL_H__