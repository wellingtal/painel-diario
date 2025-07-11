//=================================================================
// Name : RYLCamp.h
// Data : 2004. 06. 01
// Desc : Camp Class
//=================================================================
#ifndef __RYL_CAMP_H__
#define __RYL_CAMP_H__

#include <castle/castleConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include "ItemInstance.h"

class CRYLCamp
{
private :
	unsigned long		m_dwCID ;				// CID 
	unsigned long		m_dwCampID ;			// Camp ID
	unsigned long		m_dwGID ;				// Camp를 만든 길드 ID
	unsigned long		m_dwHP ;			 	// Camp HP
	unsigned char		m_cState ;				// Camp State
	unsigned char		m_cUpgradeStep ;		// UpgradeStep
	float				m_fPointX ;				// x (m 단위)
	float				m_fPointY ;				// y (m 단위)
	float				m_fPointZ ;				// z (m 단위)
	CampRight			m_CampRight ;			// 길드 요새 관리 권한
	char				m_Nation ;				// 국가

	BOOL				m_bAttacked ;			// 공격 받고 있는가?
	unsigned long		m_dwAttackedTime ;		// 공격 받기 시작했을때의 시간
	unsigned char		m_cAlpha ;				// 구축주일 경우의 알파 값
	unsigned char		m_cAttackedAlpha ;		// 공격 받고 있을때의 알바 값

	CItemInstance*		m_lpMaterial ;			// 공성 자재

	enum BLINK_CONST
	{
		DEFAULT_ALPHA			= 0x59,
		DEFAULT_ATTACKED_ALPHA	= 0x80,

		DECREASE_POINT			= 0x03,
		ATTACKED_DECREASE_POINT	= 0x08,

		DEFAULT_BLINK_TIME		= 60000,		// 1분
	} ;

public :
	CRYLCamp() ;
	virtual ~CRYLCamp() ;

	//==================================================================================
	// Access Method
	unsigned long	GetCID()										{ return m_dwCID ; }
	void			SetCID( unsigned long dwCID )					{ m_dwCID = dwCID ; }

	unsigned long	GetCampID()										{ return m_dwCampID ; }
	void			SetCampID( unsigned long dwCampID )				{ m_dwCampID = dwCampID ; }
	
	unsigned long	GetGID()										{ return m_dwGID ; }
	void			SetGID( unsigned long dwGID )					{ m_dwGID = dwGID ; }
	
	unsigned long	GetHP()											{ return m_dwHP ; }
	void			SetHP( unsigned long dwHP )						{ m_dwHP = dwHP ; }
	
	unsigned char	GetState()										{ return m_cState ; }
	void			SetState( unsigned char cState )				{ m_cState = cState ; }

	unsigned char	GetUpgradeStep()								{ return m_cUpgradeStep ; }
	void			SetUpgradeStep( unsigned char cUpgradeStep )	{ m_cUpgradeStep = cUpgradeStep ; }

	CampRight*		GetCampRight()									{ return &m_CampRight ; }
	void			SetCampRight( const CampRight& campRight )		{ m_CampRight = campRight ; }
		
	char			GetCampNation()									{ return m_Nation ; }
	void			SetCampNation( char Nation )					{ m_Nation = Nation ; }

	BOOL			GetAttacked() ;
	void			SetAttacked( BOOL bAttacked ) ;

	void			UpdateAlpha() ;
	void			UpdateAttackedAlpha() ;

	unsigned char	GetAlpha()										{ return m_cAlpha ;			}
	unsigned char	GetAttackedAlpha()								{ return m_cAttackedAlpha ;	}
	unsigned char	GetDefaultAlpha()								{ return DEFAULT_ALPHA ;	}

	float			GetPointX()										{ return m_fPointX ;		}
	void			SetPointX( float fPointX )						{ m_fPointX = fPointX ;		}
	float			GetPointY()										{ return m_fPointY ;		}
	void			SetPointY( float fPointY )						{ m_fPointY = fPointY ;		}
	float			GetPointZ()										{ return m_fPointZ ;		}
	void			SetPointZ( float fPointZ )						{ m_fPointZ = fPointZ ;		}	

	CItemInstance*	GetMaterial()									{ return m_lpMaterial ;		}
	void			SetMaterial(CItemInstance* pMaterial)			{ m_lpMaterial = pMaterial;	}
	void			UpdateMaterial(unsigned char cMaterial);
} ;

#endif //__RYL_CAMP_H__