#include "RYLFormatList.h"
#include "RYLTextureList.h"
#include "GMMemory.h"

//------------------------------------------------------------------------------
//					CRYLFormatList
//------------------------------------------------------------------------------
CRYLFormatList::CRYLFormatList( D3DFORMAT FormatType )
{
	m_FormatType		= FormatType ;
	pRYLTexture16List	= NULL ;
	pRYLTexture32List	= NULL ;
	pRYLTexture64List	= NULL ;
	pRYLTexture128List	= NULL ;
	pRYLTexture256List	= NULL ;
}

CRYLFormatList::~CRYLFormatList()
{
	GM_DELETE( pRYLTexture16List ) ;
	GM_DELETE( pRYLTexture32List ) ;
	GM_DELETE( pRYLTexture64List ) ;
	GM_DELETE( pRYLTexture128List ) ;
	GM_DELETE( pRYLTexture256List ) ;
}

HRESULT CRYLFormatList::CreateTexture( ERYLTexture_Size_Type WSizeType, ERYLTexture_Size_Type HSizeType, HRYLTexture *pphRYLTexture ) 
{
	switch( WSizeType )
	{
		case RYLTEXTURESIZETYPE_16 : 
			if ( pRYLTexture16List == NULL )
			{
				pRYLTexture16List = new CRYLTextureList( m_FormatType, RYLTEXTURESIZETYPE_16 ) ;
			}
			pRYLTexture16List->CreateTexture( HSizeType, pphRYLTexture ) ;
			break ;

		case RYLTEXTURESIZETYPE_32 : 
			if ( pRYLTexture32List == NULL )
			{
				pRYLTexture32List = new CRYLTextureList( m_FormatType, RYLTEXTURESIZETYPE_32 ) ;
			}
			pRYLTexture32List->CreateTexture( HSizeType, pphRYLTexture ) ;
			break ;

		case RYLTEXTURESIZETYPE_64 :
			if ( pRYLTexture64List == NULL )
			{
				pRYLTexture64List = new CRYLTextureList( m_FormatType, RYLTEXTURESIZETYPE_64 ) ;
			}
			pRYLTexture64List->CreateTexture( HSizeType, pphRYLTexture ) ;
			break ;

		case RYLTEXTURESIZETYPE_128:
			if ( pRYLTexture128List == NULL )
			{
				pRYLTexture128List = new CRYLTextureList( m_FormatType, RYLTEXTURESIZETYPE_128 ) ;
			}
			pRYLTexture128List->CreateTexture (HSizeType, pphRYLTexture ) ;
			break ;

		case RYLTEXTURESIZETYPE_256 :
			if ( pRYLTexture256List == NULL )
			{
				pRYLTexture256List = new CRYLTextureList( m_FormatType, RYLTEXTURESIZETYPE_256 ) ;
			}
			pRYLTexture256List->CreateTexture( HSizeType, pphRYLTexture ) ;
			break ;
	}

	return S_OK ;
}

HRESULT CRYLFormatList::ReleaseTexture( HRYLTexture *pphRYLTexture )
{
	
	switch ( ( ERYLTexture_Size_Type )( *pphRYLTexture )->GetWidth() )
	{
		case RYLTEXTURESIZETYPE_16:
			if (pRYLTexture16List)
				pRYLTexture16List->ReleaseTexture( pphRYLTexture ) ;
			break ;
		case RYLTEXTURESIZETYPE_32: 
			if ( pRYLTexture32List )
				pRYLTexture32List->ReleaseTexture( pphRYLTexture ) ;
			break ;
		case RYLTEXTURESIZETYPE_64:
			if ( pRYLTexture64List )
				pRYLTexture64List->ReleaseTexture( pphRYLTexture ) ;
			break ;
		case RYLTEXTURESIZETYPE_128:
			if ( pRYLTexture128List )
				pRYLTexture128List->ReleaseTexture( pphRYLTexture ) ;
			break ;
		case RYLTEXTURESIZETYPE_256:
			if ( pRYLTexture256List )
				pRYLTexture256List->ReleaseTexture( pphRYLTexture ) ;
			break ;
	}

	return S_OK ;
}

HRESULT CRYLFormatList::InitDeviceObjects()
{
	if (pRYLTexture16List)
		pRYLTexture16List->InitDeviceObjects();
	if (pRYLTexture32List)
		pRYLTexture32List->InitDeviceObjects();
	if (pRYLTexture64List)
		pRYLTexture64List->InitDeviceObjects();
	if (pRYLTexture128List)
		pRYLTexture128List->InitDeviceObjects();
	if (pRYLTexture256List)
		pRYLTexture256List->InitDeviceObjects();

	return S_OK ;
}

HRESULT CRYLFormatList::DeleteDeviceObjects()
{
	if (pRYLTexture16List)
		pRYLTexture16List->InitDeviceObjects();
	if (pRYLTexture32List)
		pRYLTexture32List->InitDeviceObjects();
	if (pRYLTexture64List)
		pRYLTexture64List->InitDeviceObjects();
	if (pRYLTexture128List)
		pRYLTexture128List->InitDeviceObjects();
	if (pRYLTexture256List)
		pRYLTexture256List->InitDeviceObjects();

	return S_OK ;
}



