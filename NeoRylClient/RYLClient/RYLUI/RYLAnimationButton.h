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
		Max_Animation_Frame	= 30			// �Ѱ��� �ν��Ͻ��� �ִ� ������ ��.
	};
	
	CRYLAnimationButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLAnimationButton() ;
	
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	
	BOOL			AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;

	VOID			SetFrame(unsigned char cStartFrm, unsigned char cEndFrm, INT iInstance);

	VOID			SetFrameTick(unsigned long dwTick);

private:

	unsigned long	m_dwFrameTick;														// �ִϸ��̼� �ӵ�.
	unsigned long	m_dwPrevTick;														// ���� Tick.

	INT				m_iPrevInstance;													// ���� �ν��Ͻ�.

	unsigned char	m_cFrame[GMUIButton_Image_Count];									// ���� ������.

	unsigned char	m_cFrameIndex[GMUIButton_Image_Count][2];							// �ִϸ��̼� ����, �� ������.

	unsigned char	m_cFrameCnt[GMUIButton_Image_Count];								// Ÿ�Կ� ���� ������ ����.
	
	CGMTexture*		m_pAniButtonTexture[GMUIButton_Image_Count*Max_Animation_Frame];	// �̹���.
} ;

#endif //__RYLANIMATIONBUTTON_H__