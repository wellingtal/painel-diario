#include "GMList.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMMemory.h"


CRYLImageManager* CRYLImageManager::s_pInstance				= NULL ;
CRYLImageCabinet* CRYLImageManager::s_pDefaultImageCabinet	= NULL ;

CRYLImageManager::CRYLImageManager()
: m_RYLImagePool(512)
{
	m_pDataList = new CGMList() ;
}

CRYLImageManager::~CRYLImageManager()
{
	Clear() ;
	GM_DELETE( m_pDataList ) ;
}

VOID CRYLImageManager::Clear ()
{
	CRYLImageCabinet* pImageCabinet ;

	INT count = m_pDataList->Count ();
	for ( INT i = 0 ; i < count ; i ++ )
	{
		m_pDataList->Get( i, ( VOID** )&pImageCabinet ) ;
        m_RYLImageCabinetPool.destroy(pImageCabinet);
	}

	m_pDataList->Clear() ;
}

HRESULT CRYLImageManager::CreateInstance ()
{
	GMASSERT( !s_pInstance, "CRYLImageManager::s_pInstance Failed!" ) ;
	s_pInstance = new CRYLImageManager() ;
	s_pInstance->CreateImageCabinet( &s_pDefaultImageCabinet ) ;
	GMASSERT( s_pInstance, "CRYLImageManager::s_pInstance Failed!" ) ;
	
	return S_OK ;
}

HRESULT CRYLImageManager::DeleteInstance ()
{
	GMASSERT( s_pInstance, "CRYLImageManager::s_pInstance Failed!" ) ;
	GM_DELETE( s_pInstance ) ;

	return S_OK ;
}

BOOL CRYLImageManager::IsInited ()
{
	return s_pInstance != NULL ; 
}


CRYLImage* CRYLImageManager::CreateRYLImage()
{
    return m_RYLImagePool.construct();
}

void CRYLImageManager::DeleteRYLImage(CRYLImage* lpRYLImage)
{
    m_RYLImagePool.destroy(lpRYLImage);
}

CRYLImageManager* CRYLImageManager::Instance ()
{
	GMASSERT( s_pInstance, "CRYLImageManager::s_pInstance Failed!" ) ;
	return s_pInstance ; 
}

VOID CRYLImageManager::CreateImageCabinet (CRYLImageCabinet** ppImageCabinet, LPSTR pFileName)
{
	CRYLImageCabinet* pImageCabinet ;

	pImageCabinet = m_RYLImageCabinetPool.construct();
	*ppImageCabinet = pImageCabinet ;

	m_pDataList->Add (pImageCabinet);
}

CRYLImageCabinet* CRYLImageManager::GetImageCabinet( INT Index )
{
	CRYLImageCabinet* pImageCabinet ;
	
	if ( ( Index < 0 ) && ( Index > m_pDataList->Count() - 1 ) )
		return NULL ;

	m_pDataList->Get( Index, ( VOID** )&pImageCabinet ) ;
	
	return pImageCabinet ;
}

CRYLImageCabinet* CRYLImageManager::GetImageCabinet( LPSTR pFileName )
{
	CRYLImageCabinet* pImageCabinet ;

	INT count = m_pDataList->Count() ;
	for ( INT i = 0 ; i < count ; i ++ )
	{
		m_pDataList->Get( i, ( VOID** )&pImageCabinet ) ;
		if ( strcmp( pImageCabinet->GetFileName(), pFileName ) == 0 )
		{
			return pImageCabinet ;
		}

	}
	return NULL ;
}


