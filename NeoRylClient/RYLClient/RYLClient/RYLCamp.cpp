#include "RYLCamp.h"
#include "Item/ItemConstants.h"
#include "GMMemory.h"

CRYLCamp::CRYLCamp() 
{
	m_dwCID					= 0 ;		// CID 
	m_dwCampID				= 0 ;		// Camp ID
	m_dwGID					= 0 ;		// Camp를 만든 길드 ID
	m_dwHP					= 0 ;		// Camp HP
	m_cState				= 0 ;		// Camp State
	m_cUpgradeStep			= 0 ;		// UpgradeStep
	m_fPointX				= 0 ;		// x
	m_fPointY				= 0 ;		// y
	m_fPointZ				= 0 ;		// z
	m_Nation				= Creature::STATELESS ;

	m_bAttacked				= FALSE ;
	m_dwAttackedTime		= 0 ;
	m_cAlpha				= DEFAULT_ALPHA ;
	m_cAttackedAlpha		= DEFAULT_ATTACKED_ALPHA ;

	m_lpMaterial			= NULL ;

	m_CampRight				= CampRight() ;
}

CRYLCamp::~CRYLCamp() 
{
	if ( m_lpMaterial )
	{
		delete m_lpMaterial ;
		m_lpMaterial = NULL ;
	}
}

BOOL	CRYLCamp::GetAttacked()
{
	if ( m_bAttacked )
	{
		unsigned long dwNowTime = timeGetTime();
		if ( dwNowTime - m_dwAttackedTime < DEFAULT_BLINK_TIME )
		{
			return TRUE ;
		}
		else
		{
			SetAttacked( FALSE ) ;
			m_cAttackedAlpha = DEFAULT_ATTACKED_ALPHA ;
		}
	}

	return FALSE ;
}

void	CRYLCamp::SetAttacked( BOOL bAttacked )
{
	m_bAttacked = bAttacked ;
	if ( bAttacked )
	{
		m_dwAttackedTime = timeGetTime() ;
	}
	else
	{
		m_dwAttackedTime = 0 ;
	}
}

void	CRYLCamp::UpdateAlpha()
{
	if ( 0 == m_cAlpha )
	{
		m_cAlpha = DEFAULT_ALPHA ;
	}
	else if ( m_cAlpha < DECREASE_POINT )
	{
		m_cAlpha = 0x00 ;
	}
	else
	{
		m_cAlpha -= DECREASE_POINT ;
	}
}

void	CRYLCamp::UpdateAttackedAlpha()
{
	if ( 0 == m_cAttackedAlpha )
	{
		m_cAttackedAlpha = DEFAULT_ATTACKED_ALPHA ;
	}
	else if ( m_cAttackedAlpha < ATTACKED_DECREASE_POINT )
	{
		m_cAttackedAlpha = 0x00 ;
	}
	else
	{
		m_cAttackedAlpha -= ATTACKED_DECREASE_POINT ;
	}
}

void	CRYLCamp::UpdateMaterial(unsigned char cMaterial)
{
	if ( m_lpMaterial )
	{
		if ( 0 == cMaterial )
		{
			delete m_lpMaterial ;
			m_lpMaterial = NULL ;
			return;
		}
		else if ( cMaterial == m_lpMaterial->GetNowDurability() )
		{
			return;
		}
	}
	else
	{
		m_lpMaterial = new CItemInstance();
		Item::ItemPos pos( 0, 0 );
		m_lpMaterial->SetItemInfofromID( Item::EtcItemID::SIEGE_MATERIAL_ID, pos );
	}

	if ( cMaterial <= m_lpMaterial->GetMaxDurability() )
	{
		m_lpMaterial->SetNowDurability( cMaterial ) ;
	}
	else
	{
		m_lpMaterial->SetNowDurability( m_lpMaterial->GetMaxDurability() ) ;
	}
}