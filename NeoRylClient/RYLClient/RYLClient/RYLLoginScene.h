//=================================================================
// Name : RYL Login Scene.h
// Data : 2003. 11. 13
// Desc : Login Scene 
//			- public : CRYLSceneObject
//=================================================================

#ifndef __RYL_LOGINSCENE_H__
#define __RYL_LOGINSCENE_H__

#include "RYLSceneObject.h"

class CRYLDialog ;
class CRYLLoginDlg ;
class CRYLAgreeDlg ;
class CRYLImage ;
class CRYLIntegServerSelCharDlg ;

class CRYLLoginScene : public CRYLSceneObject
{
public :
	CRYLDialog*				m_pRYLLogDlg ;
	CRYLAgreeDlg*			m_pRYLAgreeDlg ;
	CRYLImage*				m_pLoginImage ;
	CRYLDialog*				m_pIntegServerSelCharDlg ;
	CRYLDialog*				m_pIntegServerSelVaultDlg ;
	CRYLDialog*				m_pSelectedConfirmDlg ;
	CRYLDialog*				m_pPasswordReAuthDlg ;
	CRYLDialog*				m_pPartChoiceDlg ;
	CRYLDialog*				m_ChinaUnifiedServerSelCharDlg ;
	CRYLDialog*				m_pPCCheckAgreementDlg;

	BOOL					m_bShowAgree ;
	BOOL					m_bAuthSuccess ;

	UINT					m_uiTitleImageSX ;
	UINT					m_uiTitleImageSY ;

	DWORD					m_dwConfirmResult ;
	DWORD					m_dwFirstLoginResult ;
	DWORD					m_dwAddUnifiedCharMsg ;
	DWORD					m_dwSelectServerResult ;

public :
	CRYLLoginScene( INT iID ) ;
	virtual ~CRYLLoginScene() ;	

	virtual HRESULT			SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual	HRESULT			Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT			FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT			Update() ;
	
	virtual HRESULT			InitResourceObject() ;
	virtual HRESULT			DeleteResourceObject() ; 

	virtual HRESULT			FinalCleanup() ;
	virtual HRESULT			MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) ;

	virtual VOID			SceneShow() ;
	virtual VOID			SceneHide() ;
    
public :
	VOID	SetPCCheckAgreeMode( BOOL bAgree );

	VOID	SetAgreeMode( BOOL bAgree ) ;
	BOOL	GetAgreeMode()					{ return m_bShowAgree ; } 
	BOOL	GetAuthSuccess()				{ return m_bAuthSuccess ; }
	VOID	SetAuthSuccess( BOOL bAuth )	{ m_bAuthSuccess = bAuth ; }
	void	FirstLoginMsgBox() ;
	void	AddUnifiedCharMsg() ;
	void	ChinaSelectSlot() ;

	CRYLDialog*		GetLoginDlg()				{ return m_pRYLLogDlg ; }
	CRYLDialog*		GetIntegServerSelCharDlg()  { return m_pIntegServerSelCharDlg ; }
	CRYLDialog*		GetIntegServerSelVaultDlg() { return m_pIntegServerSelVaultDlg ; }
	CRYLDialog*		GetSelectedConfirmDlg()		{ return m_pSelectedConfirmDlg ; }
	CRYLDialog*		GetPasswordReAuthDlg()		{ return m_pPasswordReAuthDlg ; }
	CRYLDialog*		GetPartChoiceDlg()			{ return m_pPartChoiceDlg ; }
	CRYLDialog*		GetChinaUnifiedServerSelCharDlg() { return m_ChinaUnifiedServerSelCharDlg ; }
	CRYLDialog*		GetPCCheckAgreementDlg()			{ return m_pPCCheckAgreementDlg;			}
} ;

#endif //__RYL_LOGINSCENE_H__