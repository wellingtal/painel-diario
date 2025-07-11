//================================================================
//
//	Name : RYLAnimationButton.h 
//	Desc : RYL Animation Button
//  Date : 2005. 07. 12
//
//================================================================
#ifndef __RYLANIMATIONBUTTON_H__
#define __RYLANIMATIONBUTTON_H__

#include "GMUICaptionButton.h"

class CGMUICustomDlg ;
class CGMImageDib ;

class CRYLAnimationButton : public CGMUICaptionButton
{
public:

	enum Const
	{
		Max_Animation_Frame	= 30			// 한개의 인스턴스에 최대 프레임 수.
	};
	
	CRYLAnimationButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLAnimationButton() ;
	
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	
	BOOL			AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;

	VOID			SetFrame(unsigned char cStartFrm, unsigned char cEndFrm, INT iInstance);

	VOID			SetFrameTick(unsigned long dwTick);

private:

	unsigned long	m_dwFrameTick;														// 애니메이션 속도.
	unsigned long	m_dwPrevTick;														// 지난 Tick.

	INT				m_iPrevInstance;													// 이전 인스턴스.

	unsigned char	m_cFrame[GMUIButton_Image_Count];									// 현재 프레임.

	unsigned char	m_cFrameIndex[GMUIButton_Image_Count][2];							// 애니메이션 시작, 끝 프레임.

	unsigned char	m_cFrameCnt[GMUIButton_Image_Count];								// 타입에 따른 프레임 갯수.
	
	CGMTexture*		m_pAniButtonTexture[GMUIButton_Image_Count*Max_Animation_Frame];	// 이미지.
} ;

#endif //__RYLANIMATIONBUTTON_H__