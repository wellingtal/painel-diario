#include "RYLPicture.h"
#include "GMTexture.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMMemory.h"

CRYLPicture::CRYLPicture( CGMUICustomDlg *pParentDlg ) : CGMUILabel( pParentDlg )
{
	m_pImage = NULL ;
}

CRYLPicture::~CRYLPicture()
{
	CRYLImageManager::s_pDefaultImageCabinet->DeleteRYLImage( (CRYLImage*)m_pImage ) ;
	m_pImage = NULL ;
}

VOID CRYLPicture::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CGMUILabel::DoDraw( Sender, r, State ) ;

	if ( m_pImage )
	{
		m_pImage->SetVisible( TRUE ) ;
		m_pImage->Draw( r.left, r.top ) ;
	}
}

VOID CRYLPicture::AttachImageTexture( CGMTexture* pGMTexture )
{
	m_pImage = pGMTexture ;
}

VOID CRYLPicture::DetachImageTexture()
{
	m_pImage = NULL ;
}

VOID CRYLPicture::LoadFromFile( LPSTR pFileName )
{
	CRYLImage* pImage ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromFile (pFileName);
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	SetWidth( pImage->GetWidth() ) ;
	SetHeight( pImage->GetHeight() ) ;
	m_pImage = pImage ;
}

VOID CRYLPicture::SetVisible( BOOL bVisible )
{
	CGMUILabel::SetVisible( bVisible ) ;
	if ( m_pImage )
		m_pImage->SetVisible( bVisible ) ;
}



