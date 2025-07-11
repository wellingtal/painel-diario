#include "RYLGridLabel.h"
#include "GMFontTexture.h"
#include "GMFont.h"
#include "RYLStringTable.h"
#include "GMMemory.h"

CRYLGridLabel::CRYLGridLabel( INT DataCount )
{
	m_iWidth = 0 ;
	m_iDataCount = DataCount ;
	m_ArrGridLabelData = new SGridLabelData[ m_iDataCount ] ;
	for ( INT i = 0 ; i < m_iDataCount ; i ++ )
	{
		m_ArrGridLabelData[ i ].FontTexture = NULL ;
	}
}

CRYLGridLabel::~CRYLGridLabel()
{
	Clear() ;
}

VOID CRYLGridLabel::Clear()
{
	for ( INT i = 0 ; i < m_iDataCount ; i ++ )
	{
		GM_DELETE( m_ArrGridLabelData[ i ].FontTexture ) ;
	}
	GM_DELETE_ARRAY( m_ArrGridLabelData ) ;
	m_iDataCount = 0 ;
}

VOID CRYLGridLabel::SetDataCount( INT DataCount)
{
	Clear() ;
	m_iDataCount = DataCount ;
	m_ArrGridLabelData = new SGridLabelData[ m_iDataCount ] ;
	
	for ( INT i = 0 ; i < m_iDataCount ; i ++ )
	{
		m_ArrGridLabelData[ i ].FontTexture = NULL ;
	}
}

INT CRYLGridLabel::GetDataCount()
{
	return m_iDataCount ;
}

LPSTR CRYLGridLabel::GetData( INT Index )
{
    if ( Index > m_iDataCount )
    {
		GMASSERT( FALSE, "CRYLGridLabel::GetData Failed!" ) ;
    }

	return m_ArrGridLabelData[ Index ].Str ;
}

VOID CRYLGridLabel::SetData( INT Index, LPSTR pStr )
{
	if ( Index > m_iDataCount )
    {
		GMASSERT( FALSE, "CRYLGridLabel::SetData Failed!" ) ;
    }
		
	if ( strcmp( m_ArrGridLabelData[ Index ].Str, pStr ) != 0 )
	{
		m_ArrGridLabelData[ Index ].FontTexture->SetText( pStr ) ;
		m_ArrGridLabelData[ Index ].Str = pStr ;
	}
}

VOID CRYLGridLabel::SetFontColor( INT Index, DWORD dwFontColor )
{
	if ( Index > m_iDataCount )
    {
		GMASSERT( FALSE, "CRYLGridLabel::SetData Failed!" ) ;
    }

	m_ArrGridLabelData[ Index ].m_dwFontColor = dwFontColor ;
	m_ArrGridLabelData[ Index ].FontTexture->SetFontColor( dwFontColor ) ;
}

VOID CRYLGridLabel::SetFontColor( DWORD dwFontColor )
{
	for ( INT i = 0 ; i < m_iDataCount ; i ++ )
	{
		if ( m_ArrGridLabelData[ i ].FontTexture )
			m_ArrGridLabelData[ i ].m_dwFontColor = dwFontColor ;
	}
}

DWORD CRYLGridLabel::GetFontColor( INT Index )
{
	if ( Index > m_iDataCount )
    {
        GMASSERT( FALSE, "CRYLGridLabel::GetData Failed!" ) ;
    }

	return m_ArrGridLabelData[ Index ].m_dwFontColor ;
}

VOID CRYLGridLabel::SetViewFontColor( DWORD dwFontColor )
{
	for ( INT i = 0 ; i < m_iDataCount ; i ++ )
	{
		if ( m_ArrGridLabelData[ i ].FontTexture )
			m_ArrGridLabelData[ i ].FontTexture->SetFontColor( dwFontColor ) ;
	}
}

VOID CRYLGridLabel::SetViewFontColor()
{
	for ( INT i = 0 ; i < m_iDataCount ; i ++ )
	{
		if ( m_ArrGridLabelData[ i ].FontTexture )
			m_ArrGridLabelData[ i ].FontTexture->SetFontColor( m_ArrGridLabelData[ i ].m_dwFontColor ) ;
	}
}

VOID CRYLGridLabel::SetSize( INT Index, INT Width, int iPosX )
{
	if ( Index > m_iDataCount )
    {
		GMASSERT( FALSE, "CRYLGridLabel::SetSize Failed!" ) ;
    }

	if ( !m_ArrGridLabelData[ Index ].FontTexture )
	{
		CGMFont::Instance()->CreateFontTexture( &m_ArrGridLabelData[ Index ].FontTexture ) ;
	}
	m_ArrGridLabelData[Index].FontTexture->SetSize( Width, 16 ) ;// 16∞Ì¡§
	m_ArrGridLabelData[Index].iPosX = iPosX ;

	m_iWidth = Width ;
}


VOID CRYLGridLabel::Paint( INT xx, INT yy )
{
	INT n = xx;
	for ( INT i = 0 ; i < m_iDataCount ; i ++ )
	{
		if ( m_ArrGridLabelData[ i ].iPosX == -1 )
		{
			if ( m_ArrGridLabelData[ i ].FontTexture )
			{
				INT iWidth = CRYLStringTable::GetStringWidth( m_ArrGridLabelData[ i ].Str ) ;
				INT iTempX = n + ( ( m_ArrGridLabelData[ i ].FontTexture->GetWidth() - iWidth ) / 2 ) ;
				m_ArrGridLabelData[ i ].FontTexture->Draw( iTempX, yy ) ;
				n = n + m_ArrGridLabelData[ i ].FontTexture->GetWidth() ;
			}
		}
		else
		{
			if ( m_ArrGridLabelData[ i ].FontTexture )
			{
				m_ArrGridLabelData[i].FontTexture->Draw( m_ArrGridLabelData[i].iPosX, yy);
				//n = n + m_ArrGridLabelData[i].FontTexture->GetWidth ();				
			}			
		}
	}
}


