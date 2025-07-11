#include "RYLCastle.h"
#include "RYLCreature.h"
#include "../ItemInstance.h"
#include "RYLCastleManager.h"
#include "RYLStringTable.h"
#include "GMMemory.h"

CRYLCastle::CRYLCastle() 
{
	m_dwCastleID				= 0L ;
	m_cNation					= Creature::STATELESS ;
	m_cZone						= 0 ;
	m_cNameID					= 0 ;
	m_cInvincibleCount			= 0 ;
	m_wTotalSiegeCount			= 0 ;
	m_dwTotalTaxMoney			= 0 ;
	m_cUpgradeStep				= 0 ;
	m_vecPos					= vector3( 0.0f, 0.0f, 0.0f ) ;

	m_castleRight				= CastleRight() ;
	m_szCastleName[ 0 ]			= '\0';

	::memset( &m_CastleTax, 0, sizeof(CastleTaxInfo) * Castle::MAX_TAX_TYPE ) ;
	
	m_bEnableSiege				= false ;
	m_pEmblem					= 0L ;

	for (int i=0; i<Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++i)
	{
		m_lpUpgradeItem[ i ] = NULL ;
	}
}

CRYLCastle::~CRYLCastle() 
{
	for (int i=0; i<Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++i)
	{
		if ( m_lpUpgradeItem[ i ] )
		{
			delete m_lpUpgradeItem[ i ] ;
			m_lpUpgradeItem[ i ] = NULL ;
		}
	}
}

unsigned char	CRYLCastle::GetTax( unsigned char cType ) const
{
	if ( cType < Castle::MAX_TAX_TYPE )
	{
		return m_CastleTax[ cType ].m_cTax ;
	}

	return 0 ;
}

void	CRYLCastle::SetTax( unsigned char cType, unsigned char cTax )
{
	if ( cType < Castle::MAX_TAX_TYPE && cTax <= 50 )
	{
		m_CastleTax[ cType ].m_cTax = cTax ;
		m_CastleTax[ cType ].m_cTaxChangable = Castle::TAX_DISABLE ;
	}
}

unsigned long	CRYLCastle::GetTaxMoney( unsigned char cType ) const
{
	if ( cType < Castle::MAX_TAX_TYPE )
	{
		return m_CastleTax[ cType ].m_dwTaxMoney ;
	}

	return 0 ;
}

void	CRYLCastle::SetTaxMoney( unsigned char cType, unsigned long dwTaxMoney )
{
	if ( cType < Castle::MAX_TAX_TYPE )
	{
		m_CastleTax[ cType ].m_dwTaxMoney = dwTaxMoney ;
	}
}

bool	CRYLCastle::GetEnableTaxChange( unsigned char cType ) const
{
	if ( cType < Castle::MAX_TAX_TYPE )
	{
		return ( Castle::TAX_ENABLE == m_CastleTax[ cType ].m_cTaxChangable ) ;
	}

	return false ;
}

void	CRYLCastle::SetEnableTaxChange( unsigned char cType, bool bEnable )
{
	if ( cType < Castle::MAX_TAX_TYPE )
	{
		unsigned char cFlag = ( true == bEnable ) ? Castle::TAX_ENABLE : Castle::TAX_DISABLE ;
		m_CastleTax[ cType ].m_cTaxChangable = cFlag ;
	}
}

void	CRYLCastle::EnableTaxChange()
{
	for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
	{
		m_CastleTax[ i ].m_cTaxChangable = Castle::TAX_ENABLE ;
	}
}

void	CRYLCastle::SetCastleName( unsigned char cNameID )
{
	m_cNameID = cNameID ;
	strcpy( m_szCastleName, CRYLStringTable::m_strString[ 3810 + cNameID ] ) ;
}

const char*		CRYLCastle::GetTitleName( unsigned char cTitle ) const
{
	return CRYLStringTable::m_strString[ 3802 + cTitle ] ;
}

void	CRYLCastle::SetCastleTaxInfo( CastleTaxInfo* pTaxInfo )
{
	if ( pTaxInfo )
	{
		for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
		{
			m_CastleTax[ i ] = pTaxInfo[ i ] ;
			m_CastleTax[ i ].m_dwTempTaxMoney = 0 ;
		}
	}
}

unsigned short	CRYLCastle::GetCurrentItemID() const
{
	for (int i=0; i<Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++i)
	{
		if ( m_lpUpgradeItem[ i ] )
		{
			return m_lpUpgradeItem[ i ]->GetProtoTypeID() ;
		}
	}

	return 0 ;
}

unsigned char	CRYLCastle::GetCurrentItemNum() const
{
	unsigned char cCurrItemNum = 0 ;
	for (int i=0; i<Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++i)
	{
		if ( m_lpUpgradeItem[ i ] )
		{
			cCurrItemNum += m_lpUpgradeItem[ i ]->GetNowDurability() ;
		}
	}

	return cCurrItemNum ;
}

CItemInstance*	CRYLCastle::GetUpgradeItem( int iIndex ) const
{
	if ( iIndex < 0 || iIndex >= Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM )
	{
		return NULL ;
	}

	if ( m_lpUpgradeItem[ iIndex ] )
	{
		return m_lpUpgradeItem[ iIndex ] ;
	}

	return NULL ;
}

void	CRYLCastle::SetUpgradeItem( unsigned short wItemID, unsigned char* cItemNum )
{
	// 모두 지우고 새로 생성
	for (int i=0; i<Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++i)
	{
		if ( m_lpUpgradeItem[ i ] )
		{
			delete m_lpUpgradeItem[ i ] ;
			m_lpUpgradeItem[ i ] = NULL ;
		}

		if ( 0 != cItemNum && cItemNum[ i ] > 0 )
		{
			m_lpUpgradeItem[ i ] = new CItemInstance() ;
			Item::ItemPos pos( 0, 0 ) ;
			m_lpUpgradeItem[ i ]->SetItemInfofromID( wItemID, pos ) ;
			m_lpUpgradeItem[ i ]->SetNowDurability( cItemNum[ i ] ) ;
		}
	}
}

void	CRYLCastle::AddUpgradeItem( unsigned short wItemID, unsigned char cIndex, unsigned char cNum )
{
	if ( cIndex >= Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM )
	{
		// 에러닷!!
		return;
	}

	if ( NULL == m_lpUpgradeItem[ cIndex ] )
	{
		m_lpUpgradeItem[ cIndex ] = new CItemInstance() ;
		Item::ItemPos pos( 0, 0 ) ;
		m_lpUpgradeItem[ cIndex ]->SetItemInfofromID( wItemID, pos ) ;
		m_lpUpgradeItem[ cIndex ]->SetNowDurability( 0 ) ;
	}

    if ( m_lpUpgradeItem[ cIndex ]->GetProtoTypeID() == wItemID )
	{
		if ( m_lpUpgradeItem[ cIndex ]->GetMaxDurability() - m_lpUpgradeItem[ cIndex ]->GetNowDurability() < cNum )
		{
			// 에러일 가능성이 있다.
			m_lpUpgradeItem[ cIndex ]->SetNowDurability( m_lpUpgradeItem[ cIndex ]->GetMaxDurability() ) ;
		}
		else
		{
			m_lpUpgradeItem[ cIndex ]->SetNowDurability( m_lpUpgradeItem[ cIndex ]->GetNowDurability() + cNum ) ;
		}
	}
	else
	{
		// 에러닷!!
	}
}

void	CRYLCastle::DeleteUpgradeItem( unsigned short wItemID, unsigned char cIndex, unsigned char cNum )
{
	if ( cIndex >= Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM || NULL == m_lpUpgradeItem[ cIndex ] )
	{
		// 에러닷!!
		return;
	}
	
	if ( m_lpUpgradeItem[ cIndex ]->GetProtoTypeID() == wItemID )
	{
		if ( m_lpUpgradeItem[ cIndex ]->GetNowDurability() >= cNum )
		{
			m_lpUpgradeItem[ cIndex ]->SetNowDurability( m_lpUpgradeItem[ cIndex ]->GetNowDurability() - cNum ) ;

			if ( 0 == m_lpUpgradeItem[ cIndex ]->GetNowDurability() )
			{
				delete m_lpUpgradeItem[ cIndex ] ;
				m_lpUpgradeItem[ cIndex ] = NULL ;
			}
		}
		else
		{
			// 에러일 가능성이 있다.
			delete m_lpUpgradeItem[ cIndex ] ;
			m_lpUpgradeItem[ cIndex ] = NULL ;
		}
	}
	else
	{
		// 에러닷!!
	}
}

void	CRYLCastle::SetEmblem( RYLCreature* pEmblem )
{
	m_pEmblem = pEmblem ;

	if ( m_pEmblem )
	{
		m_cUpgradeStep = m_pEmblem->GetUpgradeStep() ;
	}
}

unsigned char	CRYLCastle::GetUpgradeStep()
{
	if ( m_pEmblem )
	{
		m_cUpgradeStep = m_pEmblem->GetUpgradeStep() ;
	}

	return m_cUpgradeStep ;
}

void	CRYLCastle::ChangeMaster( unsigned char cNation )	
{
	m_cNation = cNation ;
	m_cInvincibleCount = ( Creature::STATELESS == cNation ) ? 0 : Castle::INVINCIBLE_COUNT + 1 ;
	m_wTotalSiegeCount = 0 ;
	m_dwTotalTaxMoney = 0 ;
	m_cUpgradeStep = 0 ;

	m_castleRight = CastleRight() ;
	::memset(&m_CastleTax, 0, sizeof(CastleTaxInfo) * Castle::MAX_TAX_TYPE);

	m_bEnableSiege = FALSE ;

	for (int i=0; i<Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++i)
	{
		if ( m_lpUpgradeItem[ i ] )
		{
			delete m_lpUpgradeItem[ i ] ;
			m_lpUpgradeItem[ i ] = NULL ;
		}
	}
}
