#include "RYLStatusAndQuestFrameDlg.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLStatusPanel.h"

CRYLSocietyDlg::CRYLSocietyDlg() 
{
	m_pTabButton = NULL ;
	
	CHAR		szBuff[ MAX_PATH ] ;
	strcpy( szBuff, INTERFACETEXTUREPATH ) ;
	strcat( szBuff, "\\Data\\UI\\RYL\\RYL_TITLEBAR.tga" ) ;
	AttachCaptionTexture( CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( szBuff ) ) ;
	strcpy( szBuff, INTERFACETEXTUREPATH ) ;
	strcat( szBuff, "\\Data\\UI\\RYL\\RYL_WINDOW.tga" ) ;
	AttachWindowTexture( CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( szBuff ) ) ;

	SetWidth( 256 ) ;
	SetHeight( 383 ) ;
	SetCaption( "커뮤니티" ) ;
	SetFontColor( 0xffffffff ) ;

	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;
}

CRYLSocietyDlg::~CRYLSocietyDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLSocietyDlg::InitCtrl() 
{
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 27 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 31 ) ;
	m_pTabButton->SetInstance( RYLCHARACTERINFODLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( "파티" ) ;
	m_pTabButton->AttachTabButton( "친구" ) ;
	m_pTabButton->AttachTabButton( "거부" ) ;
	m_pTabButton->AttachTabButton( "파티찾기" ) ;
	AttachGMUIControl( m_pTabButton ) ;
	/*
	m_pStatusPanel = new CRYLStatusPanel( this ) ;
	m_pStatusPanel->SetLeft( 1 ) ;
	m_pStatusPanel->SetTop( 48 ) ;
	m_pStatusPanel->SetWidth( 255 ) ;
	m_pStatusPanel->SetHeight( 336 ) ;
	( ( CRYLStatusPanel* )m_pStatusPanel )->InitCtrl() ;
	AttachGMUIControl( m_pStatusPanel ) ;
	*/
}

HRESULT		CRYLSocietyDlg::Update() 
{
	//( ( CRYLStatusPanel* )m_pStatusPanel )->Update() ;

	return S_OK ;
}

VOID	CRYLSocietyDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
}

VOID	CRYLSocietyDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLStatusAndQuestFrameDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLSocietyDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLSocietyDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	
}

