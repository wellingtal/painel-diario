#include <winsock2.h>
//hz added this
#include "wtypes.h"
#include <iostream>
//hz end for change screen picture
#include "GUIDailyLoginPanel.h"
#include "RYLDialog.h"
#include "RYLCheckBox.h"
#include "RYLEdit.h"
#include "RYLClientMain.h"
#include "RYLLoginScene.h"
#include "RYLSprite.h"
#include "Texture.h"
#include "RYLTimer.h"
#include "FrameTimer.h"
#include "BaseDataDefine.h"
#include "RYLLoginDlg.h"
#include "RYLAgreeDlg.h"
#include "RYLIntegServerSelCharDlg.h"
#include "RYLIntegServerSelVaultDlg.h"
#include "RYLSelectedConfirmDlg.h"
#include "RYLPasswordReAuthDlg.h"
#include "RYLPartChoiceDlg.h"
#include "RYLChinaUnifiedServerSelCharDlg.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLSceneManager.h"
#include "RYLStringTable.h"
#include "RYLPCCheckAgreementDlg.h"
#include "GMMemory.h"

CRYLLoginScene::CRYLLoginScene( INT iID ) : CRYLSceneObject( iID )  
{
	m_pLoginImage		= NULL ;

	m_pRYLLogDlg		= new CRYLLoginDlg ;
	m_pRYLAgreeDlg		= new CRYLAgreeDlg ;

	m_pIntegServerSelCharDlg	= NULL ;
	m_pIntegServerSelVaultDlg	= NULL ;
	m_pSelectedConfirmDlg		= NULL ;
	m_pPasswordReAuthDlg		= NULL ;
	m_pPartChoiceDlg			= NULL ;
	m_pPCCheckAgreementDlg			= NULL;
	m_ChinaUnifiedServerSelCharDlg = NULL ;	

	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{ 
		m_pIntegServerSelCharDlg = new CRYLIntegServerSelCharDlg ;
		m_pIntegServerSelVaultDlg = new CRYLIntegServerSelVaultDlg ;
		m_pSelectedConfirmDlg = new CRYLSelectedConfirmDlg ;
		m_pPasswordReAuthDlg = new CRYLPasswordReAuthDlg ;
		m_pPartChoiceDlg = new CRYLPartChoiceDlg ;
		m_pPCCheckAgreementDlg		= new CRYLPCCheckAgreementDlg;		
	}
	else
	{
		m_ChinaUnifiedServerSelCharDlg = new CRYLChinaUnifiedServerSelCharDlg ;
	}
		
	m_bShowAgree		= FALSE ;

	CRYLGameData::Instance()->m_dwClientMode = CLIENT_MAIN;
    ClientNet::CClientEventHandler *lpAuthHandler = new CAuthEventHandler;

	g_pClientSocket->Open(CRYLNetworkData::m_strIP, ClientSocket::DEFAULT_AUTH_TCP_PORT, lpAuthHandler);

	m_bAuthSuccess = FALSE;
	m_bShowAgree = FALSE;

	m_uiTitleImageSX = g_ClientMain.m_iScreenWidth/2-400;//hz login screen resolution side was -400
	m_uiTitleImageSY = g_ClientMain.m_iScreenHeight/2-300;//hz login screen resolution top was -300

	m_dwConfirmResult		= 0 ;
	m_dwFirstLoginResult	= 0 ;
	m_dwAddUnifiedCharMsg	= 0 ;
	m_dwSelectServerResult  = 0 ;
}

CRYLLoginScene::~CRYLLoginScene() 
{
	DeleteResourceObject() ;	
}

HRESULT			CRYLLoginScene::SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK ;
}

HRESULT			CRYLLoginScene::Render( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID() ) ;
	
	SetRYLRenderState( lpD3DDevice ) ;
	m_pLoginImage->Draw( m_uiTitleImageSX, m_uiTitleImageSY ) ;

	if (m_dwFadeMode == FADE_IN)
	{
		m_lScreenFade += fUpdate ;
		
		if (m_lScreenFade <= FADE_END) 
			return S_OK ;
		
		if ( m_bShowAgree && CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
		{
			m_pRYLAgreeDlg->SetVisible( TRUE ) ;
		}
	} 
	else if ( m_dwFadeMode == FADE_OUT )
	{
		m_lScreenFade -= fUpdate ;
	}

	return S_OK ;
}

HRESULT			CRYLLoginScene::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT			CRYLLoginScene::Update() 
{
	BOOL bClick = FALSE, bEdge = FALSE;
	if ( m_pRYLLogDlg->GetVisible() )					m_pRYLLogDlg->Update( bClick, bEdge ) ;
	if ( m_pRYLAgreeDlg->GetVisible() )					m_pRYLAgreeDlg->Update( bClick, bEdge ) ;
	
	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{ 
		if ( m_pIntegServerSelCharDlg->GetVisible() )		m_pIntegServerSelCharDlg->Update( bClick, bEdge ) ;
		if ( m_pIntegServerSelVaultDlg->GetVisible() )		m_pIntegServerSelVaultDlg->Update( bClick, bEdge ) ;
		if ( m_pSelectedConfirmDlg->GetVisible() )			m_pSelectedConfirmDlg->Update( bClick, bEdge ) ;
		if ( m_pPasswordReAuthDlg->GetVisible() )			m_pPasswordReAuthDlg->Update( bClick, bEdge ) ;
		if ( m_pPartChoiceDlg->GetVisible() )				m_pPartChoiceDlg->Update( bClick, bEdge ) ;
		if ( m_pPCCheckAgreementDlg->GetVisible() )			m_pPCCheckAgreementDlg->Update( bClick, bEdge ) ;
	}
	else
	{
		if ( m_ChinaUnifiedServerSelCharDlg->GetVisible() ) m_ChinaUnifiedServerSelCharDlg->Update( bClick, bEdge ) ;
	}

	g_MessageBoxManager.Update() ;	

	if (m_dwFadeMode == FADE_IN && m_lScreenFade <= FADE_END) 
	{
		return E_FAIL ;
	}

	if (m_dwFadeMode == FADE_OUT)
	{
		CRYLGameData::Instance()->EnterClient();
		return E_FAIL ;
	}

	if ( ClientSocket::IsNSFlagChecked( CmdAuthAccount ) || ClientSocket::IsNSFlagChecked( CmdUserLogin ) )
	{
		return E_FAIL ;
	}

	if ( m_dwConfirmResult & MB_YES ) 
	{
		// �ʱ�ȭ��
		GetIntegServerSelCharDlg()->SetVisible( TRUE ) ;
		
		m_dwConfirmResult = 0 ;
	}
	else if ( m_dwConfirmResult & MB_NO || m_dwConfirmResult & MB_EXIT ) 
	{
		// ���ÿϷ�
		CRYLPasswordReAuthDlg* pDlg = static_cast< CRYLPasswordReAuthDlg* >( GetPasswordReAuthDlg() ) ;
		if ( pDlg->m_bAddCharEnable )
		{
			GetPasswordReAuthDlg()->SetVisible( TRUE ) ;
		}

		m_dwConfirmResult = 0 ;
	}	

	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{
		if ( m_dwFirstLoginResult & MB_YES )
		{
			// Ȯ��
			// ���ռ����� ����� ĳ���ͼ���â�� ����.
			GetIntegServerSelCharDlg()->SetVisible( TRUE ) ;
			m_dwFirstLoginResult = 0 ;
		}
		else if ( m_dwFirstLoginResult & MB_NO || m_dwFirstLoginResult & MB_EXIT )
		{
			CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
			PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			// ����
			m_dwFirstLoginResult = 0 ;
		}

		if ( m_dwAddUnifiedCharMsg & MB_YES )
		{
			// �߰�����
			CRYLPasswordReAuthDlg* pDlg = static_cast< CRYLPasswordReAuthDlg* >( GetPasswordReAuthDlg() ) ;
			pDlg->m_bAddCharEnable = true ;

			GetIntegServerSelCharDlg()->SetVisible( TRUE ) ;
			m_dwAddUnifiedCharMsg = 0 ;
		}
		else if ( m_dwAddUnifiedCharMsg & MB_NO || m_dwAddUnifiedCharMsg & MB_EXIT )
		{
			// ���þ���

			CRYLPasswordReAuthDlg* pDlg = static_cast< CRYLPasswordReAuthDlg* >( GetPasswordReAuthDlg() ) ;
			pDlg->m_bAddCharEnable = false ;

			pDlg->CheckPassword() ;
			//GetPartChoiceDlg()->SetVisible( TRUE ) ;
			m_dwAddUnifiedCharMsg = 0 ;
		}
	}
	else
	{
		if ( m_dwFirstLoginResult & MB_CONFIRM )
		{
			AddUnifiedCharMsg() ;
			m_dwFirstLoginResult = 0 ;
		}
		else if ( m_dwFirstLoginResult & MB_EXIT ) 
		{
			m_dwFirstLoginResult = 0 ; 
		}	

		CRYLChinaUnifiedServerSelCharDlg* pDlg = static_cast< CRYLChinaUnifiedServerSelCharDlg* >( GetChinaUnifiedServerSelCharDlg() ) ;
			
		if ( m_dwAddUnifiedCharMsg & MB_YES )
		{
			// �����α���
			if ( pDlg->LoadFastLoginSlot() )
				pDlg->SendUnifiedCharSelect() ;
			else
			{
				pDlg->SetVisible( TRUE ) ;

				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ; 
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3341 ] ) ;
			}
			
			m_dwAddUnifiedCharMsg = 0 ;
		}
		else if ( m_dwAddUnifiedCharMsg & MB_NO || m_dwAddUnifiedCharMsg & MB_EXIT )
		{
			// ��������
			pDlg->SetVisible( TRUE ) ;
			
			m_dwAddUnifiedCharMsg = 0 ;
		}

		if ( m_dwSelectServerResult & MB_YES )
		{
			GetChinaUnifiedServerSelCharDlg()->SetVisible( TRUE ) ;
			m_dwSelectServerResult = 0 ;
		}
		else if ( m_dwSelectServerResult & MB_NO || m_dwSelectServerResult & MB_EXIT )
		{
			pDlg->SendUnifiedCharSelect() ;

			m_dwSelectServerResult = 0 ;
		}
	}	

	return S_OK ;
}

/*void GetDesktopResolution(unsigned long& horizontal, unsigned long& vertical)//hz was without the CRYLLoginScene::
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}*/

HRESULT			CRYLLoginScene::InitResourceObject() 
{ 
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	//hz added this
	CRYLSpriteList::Instance()->CreateGMImage( 800, 600, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 800, 600 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN ) ) ;

	m_pLoginImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pLoginImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pLoginImage ) ;
	//hz end
	/*CRYLSpriteList::Instance()->CreateGMImage( 800, 600, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN01 ) ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN02 ) ) ;
	pImageDIB->DrawRect( &rt, 512, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN03 ) ) ;
	SetRect( &rt, 0, 0, 32,256 ) ;
	pImageDIB->DrawRect( &rt, 768, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN04_08_12 ) ) ;

	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN05 ) ) ;
	pImageDIB->DrawRect( &rt, 256, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN06 ) ) ;
	pImageDIB->DrawRect( &rt, 512, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN07 ) ) ;
	SetRect( &rt, 32, 0, 64, 256 ) ;
	pImageDIB->DrawRect( &rt, 768, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN04_08_12 ) ) ;
	
	SetRect( &rt, 0, 0, 256, 88 ) ;
	pImageDIB->DrawRect( &rt, 0, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN09_10) ) ;
	SetRect( &rt, 0, 88, 256, 176 ) ;
	pImageDIB->DrawRect( &rt, 256, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN09_10) ) ;
	SetRect( &rt, 0, 0, 256, 88 ) ;
	pImageDIB->DrawRect( &rt, 512, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN11) ) ;
	SetRect( &rt, 64, 0, 96, 88 ) ;
	pImageDIB->DrawRect( &rt, 768, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN04_08_12) ) ;

	m_pLoginImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pLoginImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pLoginImage ) ;*/
	
	GM_DELETE( pImageDIB ) ;

	FadeIn() ;

	return S_OK ;
}

HRESULT			CRYLLoginScene::DeleteResourceObject() 
{
	GM_DELETE( m_pRYLLogDlg ) ;
	GM_DELETE( m_pRYLAgreeDlg ) ;
	GM_DELETE( m_pIntegServerSelCharDlg ) ;
	GM_DELETE( m_pIntegServerSelVaultDlg ) ;
	GM_DELETE( m_pSelectedConfirmDlg ) ;
	GM_DELETE( m_pPasswordReAuthDlg ) ;
	GM_DELETE( m_ChinaUnifiedServerSelCharDlg ) ;
	GM_DELETE( m_pPCCheckAgreementDlg );
	GM_DELETE( m_pPartChoiceDlg ) ;

	return S_OK ;
}

HRESULT			CRYLLoginScene::FinalCleanup() 
{
	return S_OK ;
}

HRESULT			CRYLLoginScene::MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) 
{
	return S_OK ;
}

VOID CRYLLoginScene::SetPCCheckAgreeMode( BOOL bAgree )
{
	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{
		if(bAgree)
		{
			m_pRYLLogDlg->SetVisible( FALSE ) ;
			m_pPCCheckAgreementDlg->SetVisible( TRUE ) ;
		}
		else
		{			
			m_pPCCheckAgreementDlg->SetVisible( FALSE ) ;

			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[32]);
		}
	}
}

VOID			CRYLLoginScene::SetAgreeMode( BOOL bAgree )
{ 
	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{
		m_bShowAgree = bAgree ; 
		if ( m_bShowAgree )
		{
			m_pRYLLogDlg->SetVisible( FALSE ) ;
			m_pRYLAgreeDlg->SetVisible( TRUE ) ;
		}
		else
			m_pRYLLogDlg->SetVisible( TRUE ) ;
	}
	else
	{
		m_pRYLLogDlg->SetVisible( TRUE ) ;
	}
}


VOID			CRYLLoginScene::SceneShow() 
{
	CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
	CRYLLoginDlg* pDlg = static_cast< CRYLLoginDlg* >( pScene->GetLoginDlg() ) ;

	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{
		if ( m_bShowAgree )
		{
			pDlg->SetVisible( FALSE ) ;
			m_pRYLAgreeDlg->SetVisible( TRUE ) ;
			
		}
		else
		{
			pDlg->SetVisible( TRUE ) ;
			if ( CRYLGameData::Instance()->m_bSaveID )
			{
				pDlg->m_pIDSaveCheckBox->SetChecked( TRUE ) ;
				pDlg->m_pIDEdit->SetText( CRYLNetworkData::Instance()->m_strLoginID ) ;
				pDlg->m_pPassWordEdit->SetFocus() ;
			}
		}
	}
	else
	{
		pDlg->SetVisible( TRUE ) ;
		if ( CRYLGameData::Instance()->m_bSaveID )
		{
			pDlg->m_pIDSaveCheckBox->SetChecked( TRUE ) ;
			pDlg->m_pIDEdit->SetText( CRYLNetworkData::Instance()->m_strLoginID ) ;
			pDlg->m_pPassWordEdit->SetFocus() ;
		}
	}
}
VOID			CRYLLoginScene::SceneHide() 
{
	m_pRYLLogDlg->SetVisible( FALSE ) ;
	m_pRYLAgreeDlg->SetVisible( FALSE ) ;
}

void	CRYLLoginScene::FirstLoginMsgBox() 
{
	char buff[ 2048 ] ;

	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{
		// part I
		sprintf( buff, "%s%s%s%s%s%s%s%s",			   
			CRYLStringTable::m_strString[3342],
			CRYLStringTable::m_strString[3343],
			CRYLStringTable::m_strString[3344],
			CRYLStringTable::m_strString[3345],
			CRYLStringTable::m_strString[3346],
			CRYLStringTable::m_strString[3347],
			CRYLStringTable::m_strString[3348],
			CRYLStringTable::m_strString[3349]);

		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ; 
		lpMessageBox->Create( CRYLStringTable::m_strString[2164],  CRYLStringTable::m_strString[2200], buff, MB_YES | MB_NO | MB_EXIT ) ;
		lpMessageBox->SetResult( &m_dwFirstLoginResult ) ;
	}
	else
	{
		// part I
		sprintf( buff, "%s%s",			   
			CRYLStringTable::m_strString[3350],
			CRYLStringTable::m_strString[3351]);

		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ; 
		lpMessageBox->Create( buff, MB_CONFIRM | MB_EXIT ) ;
		lpMessageBox->SetResult( &m_dwFirstLoginResult ) ;
	}
}

void	CRYLLoginScene::AddUnifiedCharMsg()
{
	char buff[ 512 ] ;
	
	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{
		// part I
		sprintf( buff, CRYLStringTable::m_strString[3352] ) ;
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ; 
		lpMessageBox->Create( CRYLStringTable::m_strString[3353], CRYLStringTable::m_strString[3354], buff, MB_YES | MB_NO | MB_EXIT ) ;
		lpMessageBox->SetResult( &m_dwAddUnifiedCharMsg ) ;
	}
	else
	{
		// part I// ????
		sprintf( buff, CRYLStringTable::m_strString[3355] ) ;

		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ; 
		lpMessageBox->Create( CRYLStringTable::m_strString[3356], CRYLStringTable::m_strString[3357], buff, MB_YES | MB_NO | MB_EXIT ) ;
		lpMessageBox->SetResult( &m_dwAddUnifiedCharMsg ) ;
	}
}

void	CRYLLoginScene::ChinaSelectSlot()
{
	CRYLChinaUnifiedServerSelCharDlg* pDlg = static_cast< CRYLChinaUnifiedServerSelCharDlg* >( GetChinaUnifiedServerSelCharDlg() ) ;

	char buff[ 128 ] ;
	sprintf( buff, CRYLStringTable::m_strString[3358], pDlg->GetSlotName() ) ;

	CRYLMessageBox *lpMessageBox ;
	lpMessageBox = new CRYLMessageBox ; 
	lpMessageBox->Create( CRYLStringTable::m_strString[3359], CRYLStringTable::m_strString[3360], buff, MB_YES | MB_NO | MB_EXIT ) ;
	lpMessageBox->SetResult( &m_dwSelectServerResult ) ;
}