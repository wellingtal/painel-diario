#include "RYLClientMain.h"
#include "GMUIScreen.h"
#include "GMUICustomDlg.h"
#include "GMUICustomEdit.h"

#include "GMUICustomIMEView.h"

#include "GMList.h"
#include "GMUIUtil.h"

#include "IME/ImeConstant.h"
#include "IME/GMIMEPACK.h"

#include "RYLDialog.h"
#include "GMUITooltipManager.h"
#include "GUITooltipManager.h"
#include "GMMemory.h"


CGMUIScreen*	CGMUIScreen::s_pInstance  = NULL ;
BOOL			CGMUIScreen::s_bFirstIme = FALSE ;

CGMUIScreen::CGMUIScreen() 
{
	m_pModalObject      = NULL;

	m_pDlgList          = new CGMList ;
	m_pEditList         = new CGMList ;

	m_iMouseX           = 0;
	m_iMouseY           = 0;

	m_pEnteredObject    = NULL ;
	m_iWidth		    = 640 ;
	m_iHeight		    = 480 ;
	
	m_pCurGMUIEdit		= NULL ;
	m_pGMUICaptureObject= NULL ;
	m_pGMUICustomImeView= NULL;

	OnWinProcEvent = NULL;
}

CGMUIScreen::~CGMUIScreen()
{
	m_pCurGMUIEdit		= NULL ;

   //-- 2004. . . Zergra From.--//
   ClearEditList();
   ClearDlgList();
   //-- Zergra To. --//
}

HRESULT		CGMUIScreen::CreateInstance()
{
	GMASSERT( !s_pInstance, "CGMUIScreen Instance Failed!!" ) ;
	s_pInstance = new CGMUIScreen() ;
	GMASSERT( s_pInstance, "CGMUIScreen Instance Failed!!" ) ;

	return S_OK;
}

HRESULT CGMUIScreen::DeleteInstance()
{
	GMASSERT( s_pInstance, "CGMUIScreen Instance Failed!!" ) ;
	GM_DELETE( s_pInstance ) ;

	return S_OK ;
}

BOOL CGMUIScreen::IsInited() 
{ 
	return s_pInstance != NULL ;  
}

CGMUIScreen *CGMUIScreen::GetInstance()
{
	GMASSERT( s_pInstance, "CGMUIScreen Instance Failed!!" ) ;

	return s_pInstance ; 
}

INT		CGMUIScreen::AddGMUIEdit( CGMUICustomEdit *pGMUIEdit )
{
	m_pEditList->Add( ( VOID * )pGMUIEdit ) ;
	return m_pEditList->Count() ;
}

VOID	CGMUIScreen::DeleteGMUIEdit( CGMUICustomEdit *pGMUIEdit )
{
	CGMUICustomEdit *pTempGMUIEdit ;
	
	for ( INT i = m_pEditList->Count() - 1 ;i >= 0 ; i -- )
	{
		m_pEditList->Get( i, ( VOID ** )&pTempGMUIEdit ) ;
		if ( pTempGMUIEdit == pGMUIEdit )
		{
			m_pEditList->Delete( i ) ;
			return ;
		}
	}
}

VOID	CGMUIScreen::SetCurFocusEdit( CGMUICustomEdit *pGMUIEdit )
{
	if ( m_pCurGMUIEdit != NULL )
	{
		if ( m_pCurGMUIEdit != pGMUIEdit )
		{
			m_pCurGMUIEdit->SetFocusState( FALSE ) ;
		}
	}
	
	if ( m_pCurGMUIEdit != pGMUIEdit )
	{
		m_pCurGMUIEdit = pGMUIEdit ;
		m_pCurGMUIEdit->SetFocusState( TRUE ) ;
	}
}


VOID	CGMUIScreen::GMUIObjectSetCapture( CGMUIObject* GMUIObject, INT X, INT Y )
{

	m_pGMUICaptureObject = GMUIObject ;

	m_iCaptureObjectLeft = X ;
	m_iCaptureObjectTop  = Y ;

	//if (GetCapture() != m_hParentHandle)
	//	::SetCapture( m_hParentHandle );

}



VOID	CGMUIScreen::GMUIObjectReleaseCapture ()
{
	m_pGMUICaptureObject = NULL ;
	m_iCaptureObjectLeft = 0 ;
	m_iCaptureObjectTop  = 0 ;
/*
	if (GetCapture() == m_hParentHandle)
		::ReleaseCapture();
*/
}

VOID	CGMUIScreen::GetScreenPosition( CGMUIObject *GMUIObject, INT* pX, INT* pY )
{
	*pX = GMUIObject->GetLeft() ;
	*pY = GMUIObject->GetTop() ;
}

INT		CGMUIScreen::GetDlgCount()
{
	return m_pDlgList->Count() ;
}

VOID	CGMUIScreen::AddDlg( CGMUICustomDlg *pGMUIDlg )
{
	CGMUICustomDlg *ptmpDlg ;
	for ( INT i = m_pDlgList->Count() - 1 ; i >= 0 ; i -- )
	{
		m_pDlgList->Get( i, ( VOID ** )&ptmpDlg ) ;
		if ( ptmpDlg->GetZOrder() >= pGMUIDlg->GetZOrder() )
		{
			m_pDlgList->Insert( i + 1, ( VOID * )pGMUIDlg ) ;
			return ;
		}
	}
	m_pDlgList->Insert( 0, ( VOID * )pGMUIDlg ) ;
}

VOID	CGMUIScreen::DeleteDlg( CGMUICustomDlg *pGMUIDlg )
{
	CGMUICustomDlg *ptmpDlg ;
	
	for ( INT i = m_pDlgList->Count() - 1 ;i >= 0 ; i -- )
	{
		m_pDlgList->Get( i, ( VOID ** )&ptmpDlg ) ;
		if ( ptmpDlg == pGMUIDlg )
		{
         m_pGMUICaptureObject = NULL ;
			m_pDlgList->Delete( i ) ;
			return ;
		}
	}
}

VOID	CGMUIScreen::SetViewDlg( CGMUICustomDlg *pGMUIDlg )
{
	INT n = m_pDlgList->Remove( ( VOID * )pGMUIDlg ) ;
	if ( n == -1 )
	{
		return ;
	}

	CGMUICustomDlg *ptmpDlg ;
	for ( INT i = 0 ; i < m_pDlgList->Count() ; i ++ )
	{
		m_pDlgList->Get( i, (	VOID** )&ptmpDlg ) ;
		if ( ptmpDlg->GetZOrder() >= pGMUIDlg->GetZOrder() )
		{
			m_pDlgList->Insert( i, ( VOID * )pGMUIDlg ) ;
			return ;
		}
	}

	m_pDlgList->Add( ( VOID * )pGMUIDlg ) ;
}

VOID	CGMUIScreen::GetDlg( INT aIndex, CGMUICustomDlg **ppGMUIDlg )
{
	if ( ( aIndex < 0 ) || ( aIndex > m_pDlgList->Count() - 1 ) )
	{
		*ppGMUIDlg = NULL ;
		return ;
	}

	VOID* tmp ;
	m_pDlgList->Get( aIndex, &tmp ) ;
	*ppGMUIDlg = ( CGMUICustomDlg * )tmp ;
}

BOOL	CGMUIScreen::Click( CGMUIObject* Sender, INT X, INT Y )
{
	CGMUICustomDlg *pGMUIDlg ;

	if ( m_pModalObject )
	{
		pGMUIDlg = ( CGMUICustomDlg * )m_pModalObject ;
		if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
		{
			pGMUIDlg->Click( pGMUIDlg, X - pGMUIDlg->GetLeft(), Y- pGMUIDlg->GetTop() ) ;
			return TRUE ;
		}
		return TRUE ;
	}

	pGMUIDlg = NULL ;
	for ( INT i = 0 ; i < m_pDlgList->Count() ; i ++ )
	{
		m_pDlgList->Get( i, ( VOID ** )&pGMUIDlg ) ;
		if ( pGMUIDlg->GetVisible())
		{
			if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
			{
				pGMUIDlg->Click( pGMUIDlg, X - pGMUIDlg->GetLeft(), Y- pGMUIDlg->GetTop() ) ;
				return TRUE ;
			}
		}
	}
	return FALSE ;
}

BOOL	CGMUIScreen::DblClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
 	CGMUICustomDlg *pGMUIDlg ;

	if ( m_pModalObject )
	{
		pGMUIDlg = (CGMUICustomDlg *)m_pModalObject ;
		if (InArea (pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
		{
			pGMUIDlg->DblClick( pGMUIDlg, Button, Shift, X - pGMUIDlg->GetLeft(), Y - pGMUIDlg->GetTop() ) ;
			return TRUE ;
		}
		return TRUE ;
	}

	pGMUIDlg = NULL ;
	for ( INT i = 0 ; i < m_pDlgList->Count() ; i ++ )
	{
		m_pDlgList->Get( i, ( VOID ** )&pGMUIDlg ) ;
		if ( pGMUIDlg->GetVisible() )
		{
			if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
			{
				pGMUIDlg->DblClick( pGMUIDlg, Button, Shift, X - pGMUIDlg->GetLeft(), Y - pGMUIDlg->GetTop() ) ;
				return TRUE ;
			}
		}
	}	

	return FALSE ;
}

BOOL	CGMUIScreen::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
 	m_iMouseX = X ;
	m_iMouseY = Y ;

	// Captured Object;
	if ( m_pGMUICaptureObject != NULL )
	{
      if ( !m_pGMUICaptureObject->GetVisible() )
			return TRUE ;

		m_pGMUICaptureObject->MouseDown( m_pGMUICaptureObject, Button, Shift, X - m_iCaptureObjectLeft - m_pGMUICaptureObject->GetParentDlg()->GetLeft(), Y - m_pGMUICaptureObject->GetParentDlg()->GetTop() ) ;
		return TRUE ;
	}

	CGMUICustomDlg* pGMUIDlg ;
	if ( m_pModalObject )
	{
		pGMUIDlg = ( CGMUICustomDlg * )m_pModalObject ;
		if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
		{
			pGMUIDlg->MouseDown( pGMUIDlg, Button, Shift, X - pGMUIDlg->GetLeft(), Y - pGMUIDlg->GetTop());
			return TRUE;
		}
		return TRUE;
	}

	pGMUIDlg = NULL ;
	for ( INT i = 0 ; i < m_pDlgList->Count() ; i++ )
	{
		m_pDlgList->Get( i, ( VOID ** )&pGMUIDlg ) ;
		if ( pGMUIDlg->GetVisible() )
		{
			if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
			{
				if ( pGMUIDlg->GetGMUICtrlType() != GMUISPRITE )
				{
					pGMUIDlg->MouseDown( pGMUIDlg, Button, Shift, X - pGMUIDlg->GetLeft(), Y - pGMUIDlg->GetTop() ) ;
					return TRUE ;
				}
			}
		}
	}
	return FALSE ;
}

BOOL	CGMUIScreen::MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y )
{
	m_iMouseX = X ;
	m_iMouseY = Y ;

	// Captured Object;
	if ( m_pGMUICaptureObject != NULL )
	{
      if ( !m_pGMUICaptureObject->GetVisible() )
			return TRUE ;

		m_pGMUICaptureObject->MouseMove( m_pGMUICaptureObject, Shift, X - m_pGMUICaptureObject->GetParentDlg()->GetLeft(), Y - m_pGMUICaptureObject->GetParentDlg()->GetTop() ) ;
		return TRUE;
	}

	CGMUICustomDlg *pGMUIDlg ;
	if ( m_pModalObject != NULL )
	{
		pGMUIDlg = ( CGMUICustomDlg * )m_pModalObject ;
		if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
		{
			pGMUIDlg->MouseMove( pGMUIDlg, Shift, X - pGMUIDlg->GetLeft(), Y - pGMUIDlg->GetTop() ) ;
			return TRUE ;
		}
		return TRUE ;
	}

	pGMUIDlg = NULL ;
	for ( INT i = 0 ; i < m_pDlgList->Count() ; i ++ )
	{
		m_pDlgList->Get( i, ( VOID** )&pGMUIDlg ) ;
		if ( pGMUIDlg->GetVisible() )
		{
			if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
			{
				m_ptDlg.x = X - pGMUIDlg->GetLeft() ;
				m_ptDlg.y = Y - pGMUIDlg->GetTop() ;

				if ( !g_TooltipManager.m_ttRenderTarget )
				{
					GUITooltipMgr.m_ttRenderTarget = pGMUIDlg->GetTooltip();	
				}
				else
				{
					GUITooltipMgr.m_ttRenderTarget = NULL ;
				}
				
				pGMUIDlg->MouseMove( pGMUIDlg, Shift, X - pGMUIDlg->GetLeft(), Y - pGMUIDlg->GetTop() ) ;
				return TRUE ;
			}
		}
	}
	return FALSE ;
}

BOOL CGMUIScreen::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
	m_iMouseX = X ;
	m_iMouseY = Y ;

	// Captured Object;
	if ( m_pGMUICaptureObject != NULL )
	{
		//by Hades Kang
		/*
		if ( !m_pGMUICaptureObject->GetVisible() )
			return TRUE ;
			*/

		// Minotaurs UI 에러 수정

		INT iLeft = m_pGMUICaptureObject->GetParentDlg()->GetLeft() ;
		INT iTop = m_pGMUICaptureObject->GetParentDlg()->GetTop() ;

		if ( ((CRYLDialog*)m_pGMUICaptureObject->GetParentDlg())->GetFormType() == CRYLDialog::RYLDLG_FRAME_TYPE_NONE ||
			 ((CRYLDialog*)m_pGMUICaptureObject->GetParentDlg())->GetFormType() == CRYLDialog::RYLDLG_FRAME_TYPE_CLOSEBUTTON )
		{
			iTop += 15 ;
		}
			
		m_pGMUICaptureObject->MouseUp( m_pGMUICaptureObject, Button, Shift, X - iLeft, Y - iTop ) ;
		return TRUE ;
	}

	CGMUICustomDlg *pGMUIDlg ;

	if ( m_pModalObject != NULL )
	{
		pGMUIDlg = ( CGMUICustomDlg * )m_pModalObject ;
		if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
		{
			if ( pGMUIDlg->GetGMUICtrlType() != GMUISPRITE )
			{
				pGMUIDlg->MouseUp( pGMUIDlg, Button, Shift, X - pGMUIDlg->GetLeft(), Y - pGMUIDlg->GetTop() ) ;
				return TRUE ;
			}
		}
		return TRUE ;
	}

	pGMUIDlg = NULL ;
	for ( INT i = 0 ; i < m_pDlgList->Count() ; i ++ )
	{
		m_pDlgList->Get( i, ( VOID** )&pGMUIDlg ) ;
		if ( pGMUIDlg->GetVisible() )
		{
			if ( InArea( pGMUIDlg->GetLeft(), pGMUIDlg->GetTop(), pGMUIDlg->GetWidth(), pGMUIDlg->GetHeight(), X, Y ) == 0 )
			{
				pGMUIDlg->MouseUp( pGMUIDlg, Button, Shift, X - pGMUIDlg->GetLeft(), Y - pGMUIDlg->GetTop() ) ;
				return TRUE ;
			}
		}
	}
	return FALSE ;
}

VOID	CGMUIScreen::SetCurrentHint( CGMUIObject *pGMUIObject, RECT HintR, LPSTR pHint )
{
	m_strHint   = pHint ;
	m_rHintRect = HintR ;
	
	if ( m_pEnteredObject != pGMUIObject )
	{
		if ( m_pEnteredObject != NULL )
		{
			m_pEnteredObject->MouseLeave( m_pEnteredObject ) ;
		}
		m_pEnteredObject = pGMUIObject ;	
		m_pEnteredObject->MouseEnter( m_pEnteredObject ) ;
	}
}

#define AltMask 0x20000000
INT		KeyDataToShiftState( LPARAM keyData )
{
	INT n = KEY_NONE ;
	if ( GetKeyState( VK_SHIFT ) < 0 )   { n = KEY_SHIFT ; }
	if ( GetKeyState( VK_CONTROL ) < 0 ) { n = KEY_CTRL ; }
	if ( ( keyData & AltMask ) != 0 )    { n = KEY_ALT ; }
	return n ;
}

BOOL	CGMUIScreen::GetisCurGMUIEdit()
{
	if ( m_pCurGMUIEdit )  
	{
		if ( m_pCurGMUIEdit->GetParentDlg()->GetVisible() )
		{
			return TRUE ;
		}
	}
	m_pCurGMUIEdit = NULL ;
	return FALSE ;
}

BOOL	CGMUIScreen::WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	INT			iShiftState = 0 ;
	INT			xx, yy ;

	if ( OnWinProcEvent )
		OnWinProcEvent( hWnd, uMsg, wParam, lParam ) ;

	if( m_pCurGMUIEdit && m_pCurGMUIEdit->GetFocusState() )
	{
		if( bool bResult = m_pCurGMUIEdit->GetIMEMessage( hWnd, uMsg, wParam, lParam ) )
		{
			switch( uMsg )
			{
				case WM_CHAR:
				{
                    if( wParam == VK_RETURN )
						return FALSE;

					m_pCurGMUIEdit->KeyDown( m_pCurGMUIEdit, wParam, iShiftState ) ;
				}
				break;

				case WM_KEYDOWN:
				{
					switch( wParam )
					{
						case VK_TAB:
						{
							int curidx = -1 ;
                     		int i ;
                     		CGMUICustomEdit *Edit = NULL ;

                     		if ( m_pEditList->Count() > 0 )
                     		{
                        		for ( i = 0 ; i < m_pEditList->Count() ; i ++ )
                        		{
                           			m_pEditList->Get( i, ( void** )&Edit ) ;

                           			if (m_pCurGMUIEdit == Edit)
                           			{
                           				curidx = i ;	
                           			}
								}

								Edit = NULL ;

                        		for ( i = 0 ; i < m_pEditList->Count() ; i ++ )
                        		{
                        		   	curidx ++ ;

                        		   	if ( curidx >= m_pEditList->Count() )
                        		   	{
                        		   		curidx = 0 ;
                        		   	}
                        		   	m_pEditList->Get( curidx, ( void** )&Edit ) ;
                        		   	if ( !Edit->GetVisible() ) 
                        		   	{
                        		   		continue ;
                        		   	}

                        		   	if ( Edit->GetParentDlg() == NULL ) 
                        		   	{
                        		   		continue ;
                        		   	}

                        		   	if ( !Edit->GetParentDlg()->GetVisible() ) 
                        		   	{
                        		   		continue ;
                        		   	}

                        		   	Edit->SetFocus();
                        		   	break ;
								}
							}
						}
						break;
					}

					m_pCurGMUIEdit->KeyDown( m_pCurGMUIEdit, wParam, iShiftState ) ;

					if( wParam == VK_RETURN )
						return FALSE;
				}
				break;
			}
			return static_cast<BOOL>( bResult );
		}
	}
	else if (uMsg == WM_IME_STARTCOMPOSITION)
	{
		HIMC hImc;
		hImc = ImmGetContext(g_ClientMain.m_hWnd);
		ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
		ImmReleaseContext(g_ClientMain.m_hWnd, hImc);
	}


	switch( uMsg )
	{
		// MOUSE DOWN		 	
		case WM_LBUTTONDOWN :
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
		
			if ( MouseDown( NULL, MB_LEFTCLICK, 0, xx, yy ) )
				return TRUE ;
		}
		break;

		case WM_RBUTTONDOWN :
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
		
			if ( MouseDown( NULL, MB_RIGHTCLICK, iShiftState, xx, yy ) )
				return TRUE ;
		}
		break ;

		case WM_MBUTTONDOWN :
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
		
			if ( MouseDown( NULL, MB_MIDDLECLICK, iShiftState, xx, yy ) )
				return TRUE ;
		}
		break ;

		// MOUSE UP
		case WM_LBUTTONUP:
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
			MouseUp( NULL, MB_LEFTCLICK, iShiftState, xx, yy ) ;
			
			if ( Click( NULL, xx, yy ) )
				return TRUE ;
		}
		break ;
		
		case WM_RBUTTONUP:
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
		
			if ( MouseUp( NULL, MB_RIGHTCLICK, iShiftState, xx, yy ) )
				return TRUE ;
		}
		break;

		case WM_MBUTTONUP:
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
		
			if ( MouseUp( NULL, MB_MIDDLECLICK, iShiftState, xx, yy ) )
				return TRUE ;
		}
		break;

		// MOUSE Dbl CLICK
		case WM_LBUTTONDBLCLK:
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
			
			DblClick( NULL, MB_LEFTCLICK, iShiftState, xx, yy ) ;
			
			if ( MouseDown( NULL, MB_LEFTCLICK, 0, xx, yy ) )
				return TRUE ;
		}
		break ;

		case WM_RBUTTONDBLCLK:
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
			DblClick( NULL, MB_RIGHTCLICK, iShiftState, xx, yy ) ; 

			if ( MouseDown( NULL, MB_RIGHTCLICK, iShiftState, xx, yy ) )
			return TRUE ;
		}
		break ;

		case WM_MBUTTONDBLCLK :
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
			DblClick( NULL, MB_MIDDLECLICK, iShiftState, xx, yy ) ; 
			if ( MouseDown( NULL, MB_MIDDLECLICK, iShiftState, xx, yy ) )
				return TRUE ;
		}
		break ;

		// MOUSE MOVE
		case WM_MOUSEMOVE :
		{
			xx = LOWORD( lParam ) ;
			yy = HIWORD( lParam ) ;
			iShiftState = KeysToShiftState( wParam ) ;
			if ( MouseMove( NULL, iShiftState, xx, yy ) )
				return TRUE ;
		}
		break ;

		case WM_SYSKEYDOWN :
		{
			iShiftState = KeyDataToShiftState( lParam ) ;

			// F10키를 눌렀을대 처리하기
			if(wParam == VK_F10)
				return TRUE;
			
			if ( m_pCurGMUIEdit != NULL )
			{
				m_pCurGMUIEdit->KeyDown( m_pCurGMUIEdit, wParam, iShiftState ) ;
			}
		}
		break ;

		case WM_SYSKEYUP :
		{
			iShiftState = KeyDataToShiftState( lParam ) ;
			if ( m_pCurGMUIEdit != NULL )
			{
				m_pCurGMUIEdit->KeyUp( m_pCurGMUIEdit, wParam, iShiftState ) ;
			}
		}
		break ;
	}

	return FALSE ;
}

VOID CGMUIScreen::SetModal (CGMUIObject *pGMUIObject)
{
	m_pModalObject = pGMUIObject;
}

VOID CGMUIScreen::GMUIPaint()
{
	int i;
	CGMUICustomDlg *pGMUIDlg;

	for (i=m_pDlgList->Count()-1; i>=0; i--)
	{
		m_pDlgList->Get (i, (void **)&pGMUIDlg);
		if (pGMUIDlg->GetVisible())
		{
			pGMUIDlg->GMUIPaint(pGMUIDlg->GetLeft(), pGMUIDlg->GetTop());
		}
	}

	if (m_strHint != "")
	{
		int w, h, xx, yy;

		w = GetGMUITextWidth (m_strHint);
		h = GetGMUITextHeight (m_strHint);
		xx = (m_rHintRect.left + m_rHintRect.right) / 2 - w / 2;
		if (xx < 0) { xx = 0; }
		if ((xx + w) > m_iWidth) { xx = m_iWidth - w; }
		yy = m_rHintRect.top - h - 2;
		if (yy < 0)
		{
			yy = m_rHintRect.bottom + 2;
		}
	}
}

VOID CGMUIScreen::Update( BOOL &bClick, BOOL &bEdge )
{
	CGMUICustomDlg* pGMUIDlg = NULL;

	for (int i = 0; i < m_pDlgList->Count(); ++i)
	{
		m_pDlgList->Get(i, (void **)&pGMUIDlg);
		if (NULL != pGMUIDlg)
		{
			pGMUIDlg->Update(bClick, bEdge);
			if (TRUE == bClick)
			{
				break;
			}
		}
	}
}

VOID CGMUIScreen::InterfaceBegin ()
{
}

VOID CGMUIScreen::InterfaceEnd ()
{
}


//-----------------------------------------------------------------------------------------------------
// CGMUIScreen::ClearEditList()
VOID CGMUIScreen::ClearEditList()
{
   m_pEditList->Clear() ;
   GM_DELETE( m_pEditList ) ;
}

//-----------------------------------------------------------------------------------------------------
// CGMUIScreen::ClearDlgList()
VOID CGMUIScreen::ClearDlgList()
{
   m_pDlgList->Clear() ;
   GM_DELETE( m_pDlgList ) ;
}