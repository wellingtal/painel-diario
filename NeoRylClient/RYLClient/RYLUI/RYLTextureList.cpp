#include "RYLTextureList.h"
#include "RYLTextureManager.h"
#include "GMList.h"
#include "GMMemory.h"

CRYLTextureList::CRYLTextureList( D3DFORMAT FormatType, ERYLTexture_Size_Type WSizeType )
{
	m_WSizeType		= WSizeType ;
	m_FormatType	= FormatType ;

	m_pUsedList		= new CGMList() ;
	m_pUnUsedList	= new CGMList() ;
}

CRYLTextureList::~CRYLTextureList()
{
	Clear() ;
	GM_DELETE( m_pUsedList ) ;
	GM_DELETE( m_pUnUsedList ) ;
}

HRESULT CRYLTextureList::Clear ()
{
    CRYLTextureManager* lpRYLTextureManager = CRYLTextureManager::Instance();

	HRYLTexture phRYLTexture ;

	INT count = m_pUsedList->Count() ;
	for ( INT i = 0 ; i < count ; ++ i )
	{
		phRYLTexture = NULL ;
		m_pUsedList->Get( i, ( VOID ** )&phRYLTexture ) ;
        lpRYLTextureManager->DeleteRYLTexture( phRYLTexture ) ; 
	}
	m_pUsedList->Clear() ;

	count = m_pUnUsedList->Count();
	for ( INT i = 0 ; i < count ; ++ i )
	{
		phRYLTexture = NULL ;
		m_pUnUsedList->Get( i, ( VOID **)&phRYLTexture ) ;
		lpRYLTextureManager->DeleteRYLTexture( phRYLTexture ) ;
	}
	m_pUnUsedList->Clear() ;

	return S_OK;
}

HRYLTexture CRYLTextureList::IsUnUsedTexture( ERYLTexture_Size_Type HSizeType )
{
	HRYLTexture		phRYLTexture ;
	
	for ( INT i = 0 ; i < m_pUnUsedList->Count() ; i ++ )
	{
		m_pUnUsedList->Get( i, ( VOID ** )&phRYLTexture ) ;
		if ( phRYLTexture->GetHeight () == HSizeType )
		{
			m_pUsedList->Add( phRYLTexture ) ;
			return phRYLTexture ;
		}
	}

	return NULL ;
}

INT CRYLTextureList::IsUsedTexture( HRYLTexture phRYLTexture)
{
	HRYLTexture phRYLTex ;

	for ( INT i = 0 ; i < m_pUsedList->Count() ; i ++ )
	{
		m_pUsedList->Get( i, ( VOID ** )&phRYLTex ) ;
		if ( phRYLTex == phRYLTexture )
		{
			return i ;
		}
	}

	return -1;
}

HRESULT CRYLTextureList::CreateTexture( ERYLTexture_Size_Type HSizeType, HRYLTexture *pphRYLTexture )
{
	HRYLTexture		phRYLtex ;
	phRYLtex = IsUnUsedTexture( HSizeType ) ;
	
	if ( phRYLtex == NULL )
	{
		phRYLtex = CRYLTextureManager::Instance()->CreateRYLTexture( m_FormatType, m_WSizeType, HSizeType ) ;
		m_pUsedList->Add( phRYLtex ) ;
	}

	*pphRYLTexture = phRYLtex ;	
	return S_OK ;
}

HRESULT CRYLTextureList::ReleaseTexture( HRYLTexture *pphRYLTexture )
{
	INT n = IsUsedTexture( *pphRYLTexture ) ;
	if ( n > 0 )
	{
		m_pUnUsedList->Add( *pphRYLTexture ) ;
		m_pUsedList->Delete( n ) ;
	}

	*pphRYLTexture = NULL ;
	return S_OK ;
}

HRESULT CRYLTextureList::InitDeviceObjects()
{
	HRYLTexture phRYLTexture ;
	INT			i ;

	for (i = 0 ; i < m_pUsedList->Count() ; i ++ )
	{
		m_pUsedList->Get( i, ( VOID **)&phRYLTexture ) ;
		phRYLTexture->InitDeviceObjects() ;
	}

	for (i = 0 ; i < m_pUnUsedList->Count() ;i ++ )
	{
		m_pUnUsedList->Get( i, ( VOID **)&phRYLTexture ) ;
		phRYLTexture->InitDeviceObjects() ;
	}

	return S_OK ;
}

HRESULT CRYLTextureList::DeleteDeviceObjects()
{
	HRYLTexture phRYLTexture ;
	INT			i ;

	for ( i = 0 ; i < m_pUsedList->Count() ; i ++ )
	{
		m_pUsedList->Get( i, ( VOID ** )&phRYLTexture ) ;
		phRYLTexture->DeleteDeviceObjects() ;
	}

	for ( i = 0 ; i < m_pUnUsedList->Count() ;i ++ )
	{
		m_pUnUsedList->Get( i, ( VOID ** )&phRYLTexture ) ;
		phRYLTexture->DeleteDeviceObjects() ;
	}

	return S_OK ;
}
