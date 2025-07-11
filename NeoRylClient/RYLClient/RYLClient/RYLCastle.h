//=================================================================
// Name : RYLCastle.h
// Data : 2004. 05. 18
// Desc : Castle Class
//=================================================================
#ifndef __RYL_CASTLE_H__
#define __RYL_CASTLE_H__

#include <Castle/CastleConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <map>
#include "VECTOR.h"

class CItemInstance ;
class RYLCreature ;
struct vector3 ;

class CRYLCastle
{
private :

	unsigned long					m_dwCastleID ;								// 성 ID
	unsigned char					m_cNation ;									// 성을 가진 국가
	unsigned char					m_cZone ;
	unsigned char					m_cNameID ;									// 성 이름 ID ( 1부터~~ )
	unsigned char					m_cInvincibleCount ;						// 무적 횟수
	unsigned short					m_wTotalSiegeCount ;						// 누적 공성 횟수
	unsigned long					m_dwTotalTaxMoney ;							// 누적 세금 회수량
	unsigned char					m_cUpgradeStep ;							// 성 상징물 업그레이드 단계
	vector3							m_vecPos ;									// 성의 위치

	CastleRight						m_castleRight ;								// 성 권한
	char							m_szCastleName[ Castle::MAX_CASTLE_NAME_LEN ] ;	// 성 이름

	CastleTaxInfo					m_CastleTax[ Castle::MAX_TAX_TYPE ] ;		// 세율 및 세금 정보

	bool							m_bEnableSiege ;							// 현재 공성이 가능한가?

	CItemInstance*					m_lpUpgradeItem[ Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM ] ;	// 업그레이드 보석 아이템

	RYLCreature*					m_pEmblem ;									// 성 상징물

public :

	CRYLCastle() ;
	virtual ~CRYLCastle() ;

	// ---------------------------------------------------------------------------------------------
	// Access Method

	unsigned long	GetCastleID() const										{ return m_dwCastleID ;					}
	void			SetCastleID( unsigned long dwCastleID )					{ m_dwCastleID = dwCastleID ;			}
	
	unsigned char	GetNation() const										{ return m_cNation ;					}
	void			SetNation( unsigned char cNation )						{ m_cNation = cNation ;					}

	unsigned char	GetZone() const											{ return m_cZone ;						}
	void			SetZone( unsigned char cZone )							{ m_cZone = cZone ;						}

	unsigned char	GetTax( unsigned char cType ) const	;
	void			SetTax( unsigned char cType, unsigned char cTax ) ;
	
	unsigned long	GetTaxMoney( unsigned char cType ) const ;
	void			SetTaxMoney( unsigned char cType, unsigned long dwTaxMoney ) ;

	bool			GetEnableTaxChange( unsigned char cType ) const ;
	void			SetEnableTaxChange( unsigned char cType, bool bEnable ) ;
	void			EnableTaxChange() ;

	unsigned char	GetInvincibleCount() const								{ return m_cInvincibleCount ;				}
	void			SetInvincibleCount( unsigned char cInvincibleCount )	{ m_cInvincibleCount = cInvincibleCount ;	}

	unsigned short	GetTotalSiegeCount() const								{ return m_wTotalSiegeCount ;				}
	void			SetTotalSiegeCount( unsigned short wTotalSiegeCount )	{ m_wTotalSiegeCount = wTotalSiegeCount ;	}

	unsigned short	GetTotalGainTaxCount() const							{ return (m_wTotalSiegeCount / Castle::TEMP_TAX_GAIN_COUNT);	}

	unsigned long	GetTotalTaxMoney() const								{ return m_dwTotalTaxMoney ;			}
	void			SetTotalTaxMoney( unsigned long dwTotalTaxMoney )		{ m_dwTotalTaxMoney = dwTotalTaxMoney ;	}

	CastleRight*	GetCastleRight()										{ return &m_castleRight ;				}
	void			SetCastleRight( const CastleRight& castleRight )		{ m_castleRight = castleRight ;			}

	unsigned char	GetNameID() const										{ return m_cNameID ;					}		
	const char*		GetCastleName() const									{ return m_szCastleName ;				}
	void			SetCastleName( unsigned char cNameID ) ;
	const char*		GetTitleName( unsigned char cTitle ) const ;

	bool			GetEnableSiege() const									{ return m_bEnableSiege ;				}
	void			SetEnableSiege( bool bEnableSiege )						{ m_bEnableSiege = bEnableSiege ;		}

	RYLCreature*    GetEmblem()												{ return m_pEmblem ;					}
	void			SetEmblem( RYLCreature* pEmblem ) ;

	unsigned char	GetUpgradeStep() ;
	void			SetUpgradeStep( unsigned char cUpgradeStep )			{ m_cUpgradeStep = cUpgradeStep ;		}

	const vector3&	GetPos() const											{ return m_vecPos ;						}
	void			SetPos( float fX, float fY, float fZ )					{ m_vecPos = vector3( fX, fY, fZ ) ;	}

	unsigned short	GetCurrentItemID() const ;
	unsigned char	GetCurrentItemNum() const ;
	CItemInstance*	GetUpgradeItem( int iIndex ) const ;

	void			SetCastleTaxInfo( CastleTaxInfo* pTaxInfo ) ;
	void			SetUpgradeItem( unsigned short wItemID, unsigned char* cItemNum ) ;

	void			AddUpgradeItem( unsigned short wItemID, unsigned char cIndex, unsigned char cNum ) ;
	void			DeleteUpgradeItem( unsigned short wItemID, unsigned char cIndex, unsigned char cNum ) ;

	void			ChangeMaster( unsigned char cNation )	;
} ;

#endif //__RYL_CASTLE_H__