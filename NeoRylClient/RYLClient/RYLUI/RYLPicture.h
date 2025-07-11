//================================================================
//
//	Name : RYLPicture.h 
//	Desc : RYL Picture
//  Date : 2003. 09. 25
//
//================================================================
#ifndef __RYLPICTURE_H__
#define __RYLPICTURE_H__

#include "GMUILabel.h"

class CGMTexture ;
class CGMUICustomDlg ;

class CRYLPicture : public CGMUILabel
{
protected :
	CGMTexture*		m_pImage ;

public:
	CRYLPicture( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLPicture() ;

	virtual VOID	SetVisible( BOOL boVisible ) ;
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	AttachImageTexture( CGMTexture* pGMTexture ) ;
	virtual VOID	DetachImageTexture() ;

	VOID		LoadFromFile( LPSTR pFileName ) ;
} ;

#endif // __RYLPICTURE_H__