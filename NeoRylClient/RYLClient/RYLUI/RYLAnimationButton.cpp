#include "RYLAnimationButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "GMTexture.h"
#include "GMImageDib.h"
#include "GMMemory.h"

CRYLAnimationButton::CRYLAnimationButton( CGMUICustomDlg *pParentDlg ) : CGMUICaptionButton( pParentDlg )
{
	m_dwFrameTick	= 0;
	m_dwPrevTick	= 0;
	m_iPrevInstance	= CRYLImageCabinet::EDCT_BUTTON_NORMAL;

	for(unsigned char cIndex = 0; cIndex < GMUIButton_Image_Count*Max_Animation_Frame; cIndex++)
	{
		m_pAniButtonTexture[cIndex] = NULL;
	}

	for(unsigned char cIndex = 0; cIndex < GMUIButton_Image_Count; cIndex++)
	{
		m_cFrame[cIndex]			= 0;
		m_cFrameCnt[cIndex]			= 0;
		m_cFrameIndex[cIndex][0]	= 0;
		m_cFrameIndex[cIndex][1]	= 0;
	}
}

CRYLAnimationButton::~CRYLAnimationButton()
{
}

VOID CRYLAnimationButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	unsigned long dwTick = timeGetTime();

	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType;

	switch( State )
	{
	case BTN_STATE_UP:
		DefaultCtrlType = CRYLImageCabinet::EDCT_BUTTON_NORMAL;
		break;
	case BTN_STATE_DOWN:
		DefaultCtrlType = CRYLImageCabinet::EDCT_BUTTON_DOWN;
		break;
	case BTN_STATE_ENTER:
		DefaultCtrlType = CRYLImageCabinet::EDCT_BUTTON_UP;
		break;
	case BTN_STATE_DISIBLE:
		DefaultCtrlType = CRYLImageCabinet::EDCT_BUTTON_DISIBLE;
		break;
	}

	// 이미지 교체.
	if(!m_paButtonTexture[State] || State!=m_iPrevInstance || dwTick-m_dwPrevTick>m_dwFrameTick)
	{
		unsigned char cIndex = 0;

		if(State!=m_iPrevInstance)
		{
			m_cFrame[m_iPrevInstance] = m_cFrameIndex[State][0];

			cIndex = State*Max_Animation_Frame;
		}
		else if(dwTick-m_dwPrevTick>m_dwFrameTick)
		{
			m_cFrame[State]++;

			if(m_cFrame[State]>m_cFrameIndex[State][1])
			{
				m_cFrame[State] = m_cFrameIndex[State][0];
			}

			cIndex = State*Max_Animation_Frame+m_cFrame[State];
		}

		if(m_pAniButtonTexture[cIndex])
		{
			m_paButtonTexture[State] = m_pAniButtonTexture[cIndex];
		}
		else
		{
			m_paButtonTexture[State] = (CGMTexture*)CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage(m_iWidth, m_iHeight, DefaultCtrlType);
			m_iWidth	= m_paButtonTexture[State]->GetWidth();
			m_iHeight	= m_paButtonTexture[State]->GetHeight();
		}

		m_dwPrevTick	= dwTick;
		m_iPrevInstance = DefaultCtrlType;
	}	

	CGMUICaptionButton::DoDraw (Sender, r, State);
}

BOOL CRYLAnimationButton::AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
{
	if(m_cFrameCnt[iInstance]>=Max_Animation_Frame)
		return FALSE;

	CGMImageDib* pdestDib;
	CRYLImage* pImage;

	m_iWidth  = lpRect->right - lpRect->left ;
	m_iHeight = lpRect->bottom - lpRect->top ; 

	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( m_iWidth, m_iHeight, FALSE, EAIF_32Bit ) ;

	pdestDib->ClearbyColor( 0x00000000 ) ;
	pdestDib->Draw( -lpRect->left, -lpRect->top, pDib ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pdestDib ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	m_pAniButtonTexture[iInstance*Max_Animation_Frame+m_cFrameCnt[iInstance]] = pImage;
	m_cFrameCnt[iInstance]++;

	GM_DELETE( pdestDib )

	return TRUE;
}

VOID CRYLAnimationButton::SetFrame(unsigned char cStartFrm, unsigned char cEndFrm, INT iInstance)
{
	m_cFrameIndex[iInstance][0]	= cStartFrm;
	m_cFrameIndex[iInstance][1]	= cEndFrm;
	m_cFrame[iInstance] = cStartFrm;
}

VOID CRYLAnimationButton::SetFrameTick(unsigned long dwTick)
{
	m_dwFrameTick = dwTick;
}