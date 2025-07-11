//================================================================
//
//	Name : GMUIScreen.h 
//	Desc : GMDlg Manager
//  Date : 2003. 09. 17
//
//================================================================
#ifndef __GMUISCREEN_H__
#define __GMUISCREEN_H__

#include "GMUIObject.h"

class CGMList;

class CGMUICustomDlg;
class CGMUICustomEdit;
class CGMUICustomIMEView;

//-- zergra from. --//
class GMIMEPACK;
//-- to. --//


class CGMUIScreen
{
private :

	INT			        m_iMouseX;
	INT			        m_iMouseY;
	INT			        m_iWidth;
	INT			        m_iHeight;

	HWND		        m_hParentHandle;
	CGMList*	        m_pDlgList;
	CGMList*	        m_pEditList;

	CGMUICustomEdit*    m_pCurGMUIEdit;

	CGMUIObject*	    m_pModalObject;
	CGMUIObject*	    m_pGMUICaptureObject;

	INT			        m_iCaptureObjectLeft;
	INT			        m_iCaptureObjectTop;

	CGMString	        m_strHint;
	RECT				m_rHintRect;
	CGMUIObject*		m_pEnteredObject;
	CGMUICustomIMEView*	m_pGMUICustomImeView;

	POINT		        m_ptDlg;
	

public:

	static CGMUIScreen*	s_pInstance;
	static BOOL			s_bFirstIme;
	static CGMUIScreen* GetInstance();

	static HRESULT		CreateInstance();
	static HRESULT		DeleteInstance();
	static BOOL			IsInited();

	static VOID			InterfaceBegin();
	static VOID			InterfaceEnd();
	
	OnWinProc			OnWinProcEvent;


public:

	CGMUIScreen();
	virtual ~CGMUIScreen();

	BOOL			GetisCurGMUIEdit();

	INT		        GetMouseX()							                { return m_iMouseX; }
	INT		        GetMouseY()							                { return m_iMouseY; }
	VOID	        SetWidth( INT Width )				                { m_iWidth = Width;}
	INT		        GetWidth()							                { return m_iWidth; }
	VOID	        SetHeight( INT Height )				                { m_iHeight = Height; }
	INT		        GetHeight()							                { return m_iHeight; }
	VOID	        SetParentHandle( HWND hWnd )		                { m_hParentHandle = hWnd; }
	VOID	        SetSize( INT  Width, INT Height ) 	                {  m_iWidth  = Width;	 m_iHeight = Height; }
	VOID	        GetSize( INT* pWidth, INT* pHeight )                { *pWidth = m_iWidth;	*pHeight = m_iHeight; }

	VOID	        GetCurFocusEdit( CGMUICustomEdit **ppGMUIEdit )		{ *ppGMUIEdit = m_pCurGMUIEdit; }	
	VOID			SetCurrentEditFocus( CGMUICustomEdit *pGMUIEdit )	{ m_pCurGMUIEdit = pGMUIEdit ; }
	CGMUIObject*	GetGMUICaptureObject()								{ return m_pGMUICaptureObject; }

	INT		        GetDlgCount();
	VOID	        AddDlg( CGMUICustomDlg* pGMUIDlg );
	VOID	        DeleteDlg( CGMUICustomDlg* pGMUIDlg );
	VOID	        GetDlg( INT aIndex, CGMUICustomDlg **ppGMUIDlg );
	VOID	        SetViewDlg( CGMUICustomDlg* pGMUIDlg );

	INT		        AddGMUIEdit( CGMUICustomEdit* pGMUIEdit );
	VOID			DeleteGMUIEdit( CGMUICustomEdit *pGMUIEdit ) ;

	VOID	        SetCurFocusEdit( CGMUICustomEdit* pGMUIEdit );
	VOID	        GetScreenPosition( CGMUIObject* pGMUIObject, INT* pX, INT* pY );

	VOID	        GMUIObjectSetCapture( CGMUIObject* GMUIObject, INT X, INT Y );
	VOID	        GMUIObjectReleaseCapture();

	VOID	        SetModal( CGMUIObject* pGMUIObject );
	
	BOOL	        Click( CGMUIObject* Sender, INT X, INT Y );
	BOOL	        DblClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );
	BOOL	        MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );
	BOOL	        MouseMove(CGMUIObject* Sender, INT Shift, INT X, INT Y );
	BOOL	        MouseUp(CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );

	virtual BOOL	WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual VOID	SetCurrentHint( CGMUIObject* pAUXUIObject, RECT HINTR, LPSTR pHINT );
	virtual VOID	GMUIPaint();

	VOID			Update( BOOL &bClick, BOOL &bEdge );


public:

	VOID			ClearEditList();
	VOID			ClearDlgList();
};

#endif // __GMUISCREEN_H__

 
