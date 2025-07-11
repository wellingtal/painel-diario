//================================================================
//
//	Name : RYLProgressBar.h 
//	Desc : RYL ProgressBar
//  Date : 2003. 09. 25
//
//================================================================
#ifndef __RYLPROGRESSBAR_H__
#define __RYLPROGRESSBAR_H__

#include "GMUICustomLabel.h"

class CRYLImage ;
class CGMTexture ;
class CGMUICustomDlg ;
class CRYLFontTexture ;

//------------------------------------------------------------------------------
//		*** 현재는 Min의 값이 음수일수없음 ***
//------------------------------------------------------------------------------
class CRYLProgressBar : public CGMUICustomLabel
{
private :
	DWORD		m_iPosition ;
	DWORD		m_iValueMax, m_iValueMin ;

	DWORD		m_iOffsetX, m_iOffsetY ;

	CRYLFontTexture*	m_pFontTexture ;
	CRYLImage*			m_pBackImage ;
	CRYLImage*			m_pBarImage ;

public :
	CRYLProgressBar( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLProgressBar () ;

	VOID	SetMax( DWORD iMax ) ;
	INT		GetMax() ;
	VOID	SetMin( DWORD iMin ) ;
	INT		GetMin() ;

	VOID	SetPosition( DWORD dwPosition ) ;
	INT		GetPosition() ;

	VOID	DefaultBackDraw() ;
	VOID	DefaultBarDraw() ;

	VOID	AttachBackImageTexture( CGMTexture* pGMTexture ) ;
	VOID	DetachBackImageTexture() ;
	VOID	AttachBarImageTexture( CGMTexture* pGMTexture ) ;
	VOID	DetachBarImageTexture() ;

	virtual VOID	SetFontColor( DWORD dwColor ) ;
	virtual VOID	SetColor( DWORD dwColor ) ;
	virtual VOID	Change( CGMUIObject* Sender ) ;
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
} ;

#endif //__RYLPICTURE_H__