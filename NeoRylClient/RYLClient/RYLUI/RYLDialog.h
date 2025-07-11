//================================================================
//
//	Name : RYLDialog.h 
//	Desc : RYL UI Dialog Class
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __RYLDIALOG_H__
#define __RYLDIALOG_H__

#include <d3d8.h>
#include "GMUICustomDlg.h"
#include "RYLButton.h"
 
class CGMUICustomDlg ;
class CGMTexture ;
class CGMFontTexture ;
class CRYLDialog ;

class CRYLDlgCloseButton : public CRYLButton
{
public:
	CRYLDlgCloseButton( CRYLDialog *pRYLDialog ) ;
	virtual ~CRYLDlgCloseButton() ;

	virtual VOID	Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;

private:
	VOID	SetDefaultTexture( INT State ) ;
};

class CRYLDlgMinimizeButton : public CRYLButton
{
public:
	CRYLDlgMinimizeButton( CRYLDialog *pRYLDialog ) ;
	virtual ~CRYLDlgMinimizeButton() ;

	virtual VOID	Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;

private:
	VOID	SetDefaultTexture( INT State ) ;
};

class CRYLDialog : public CGMUICustomDlg
{
private:
	INT		m_iOrWidth, m_iOrHeight ;
	INT		m_CaptionHeight ;
	BOOL	m_bDlgTypeFlag ;

	CGMFontTexture*		m_pCaptionTexture ;
	CGMTexture*			m_pCaptionImage ;
	CGMTexture*			m_pWindowImage ;

	CGMTexture*			m_pSmallIconImage ;
	CGMTexture*			m_pLargeIconImage ;

	CRYLDlgCloseButton*    m_pCloseButton ;
	CRYLDlgMinimizeButton* m_pMinimizeButton ;

protected :
	VOID	SetNormalState() ;
	VOID	SetMiniMizeState() ;
	
	VOID	SetIconState() ;
	VOID	SetCloseState() ;
	
	VOID	CreateCloseButton() ;
	VOID	CreateMinimizeButton() ;

	VOID	DefaultCaptionDraw( INT X, INT Y, INT Width, INT Height ) ;
	VOID	DefaultWindowDraw( INT X, INT Y, INT Width, INT Height ) ;

public:
	enum ERYLDLG_STATE
	{
		RYLDLG_STATE_NORMAL,
		RYLDLG_STATE_MINIMIZE,
		RYLDLG_STATE_ICON
	};
	ERYLDLG_STATE m_DlgState ;

	enum ERYLDLG_TYPE
	{
		RYLDLG_TYPE_NONE,					// 일반 윈도우
		RYLDLG_TYPE_CLOSEBUTTON,			// Close Button
		RYLDLG_TYPE_MINIMIZEBUTTON,			// Minimize Button
		RYLDLG_TYPE_BOTHBUTTON,				// Close Button || Minimize Button
		RYLDLG_FRAME_TYPE_NONE,				// 프레임 윈도우
		RYLDLG_FRAME_TYPE_CLOSEBUTTON,		// Close Button (프레임 윈도우)
		RYLDLG_FRAME_TYPE_MINIMIZEBUTTON,	// Minimize Button (프레임 윈도우)
		RYLDLG_FRAME_TYPE_BOTHBUTTON		// Close Button || Minimize Button (프레임 윈도우)
	};
	ERYLDLG_TYPE m_DlgType ;

public:
	CRYLDialog() ;
	virtual ~CRYLDialog() ;

	VOID	SetCaptionHeight( INT Height ) ;
	INT		GetCaptionHeight() ; 

	VOID	SetChangeSizeType() ;

	VOID	SetFormState( ERYLDLG_STATE DlgState ) ;
	VOID	SetFormType( ERYLDLG_TYPE DlgType ) ;
	ERYLDLG_STATE GetFormState();
	ERYLDLG_TYPE  GetFormType() ;

	VOID	AttachCaptionTexture( CGMTexture* pGMTexture ) ;
	VOID	DetachCaptionTexture();

	VOID	AttachWindowTexture( CGMTexture* pGMTexture ) ;
	CGMTexture*		DetachWindowTexture() ;

	VOID	AttachSmallIconTexture( CGMTexture* pGMTexture ) ;
	VOID	DetachSmallIconTexture();

	VOID	AttachLargeIconTexture( CGMTexture* pGMTexture ) ;
	VOID	DetachLargeIconTexture() ;

	VOID	AttachCloseButtonTexture( CGMUIButton::EButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	VOID	DetachCloseButtonTexture( CGMUIButton::EButtonImageStateType StateType ) ;

	VOID	AttachMinimizeButtonTexture( CGMUIButton::EButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	VOID	DetachMinimizeButtonTexture( CGMUIButton::EButtonImageStateType StateType ) ;

	CRYLDlgCloseButton*    GetCloseButton()		{ return m_pCloseButton ; }

public:
	virtual VOID	SetCaption( LPSTR pCaption ) ; 

	virtual VOID	DoChangeDlgState( CGMUIObject* Sender, ERYLDLG_STATE DlgState ) ;
	virtual VOID	DoChangeDlgType( CGMUIObject* Sender, ERYLDLG_TYPE DlgType ) ;

	virtual VOID	SetSize( CGMUIObject* Sender ) ;

	virtual VOID	Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID	DblClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID	MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID	MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y);

	virtual VOID	DoStartDraw( CGMUIObject* Sender, RECT r ) ;
	virtual VOID	DoEndDraw( CGMUIObject* Sender, RECT r ) ;

	virtual VOID	SetVisible( BOOL bVisible ) ;

	virtual	HRESULT			Render(  LPDIRECT3DDEVICE8 lpD3DDevice ) PURE ;
	virtual	HRESULT			FinalRender(  LPDIRECT3DDEVICE8 lpD3DDevice ) PURE ;
	virtual HRESULT			Update( BOOL &bClick, BOOL &bEdge ) PURE ;
};

#endif