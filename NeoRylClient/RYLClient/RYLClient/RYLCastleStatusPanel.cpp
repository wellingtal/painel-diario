#include <winsock2.h>
#include "RYLCastleStatusPanel.h"
#include "RYLLabel.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLImageManager.h"

#include "RYLStringTable.h"
#include "RYLCastle.h"
#include "RYLCastleManager.h"

#include <Castle/CastleBlessMgr.h>
#include "GMMemory.h"


CRYLCastleStatusPanel*	g_pCastlePanel = NULL ;

CRYLCastleStatusPanel::CRYLCastleStatusPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	m_pCastleTitleLabel		= NULL ;
	m_pCastleTitleValue		= NULL ;
	m_pCastleAreaLabel		= NULL ;
	m_pMeterLabel			= NULL ;	
	m_pPercentLabel			= NULL ;
	m_pCastleAreaValue		= NULL ;
	m_pCastleHarvestValue	= NULL ;
	m_pCastleDefenceLabel	= NULL ;
	m_pCastleDefenceValue	= NULL ;
	m_pCastleIncomeLabel	= NULL ;
	m_pCastleIncomeValue	= NULL ;
	m_pGoldLabel			= NULL ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	RECT			rt ;
	CRYLSpriteList::Instance()->CreateGMImage( 256, 265, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 48 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( i - 1 ) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect(  10,   33, 236,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  10,   94,  94,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 138,   94,  94,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  10,  155, 236,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  10,  216, 179,  19, 0xff959595, 0xc0000000,  1 ) ;
		
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 265 ) ;

	g_pCastlePanel = this ;
}

CRYLCastleStatusPanel::~CRYLCastleStatusPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLCastleStatusPanel::InitCtrl() 
{
	RECT rt;

	m_pCastleTitleLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleTitleLabel->SetLeft( 10 ) ;
	m_pCastleTitleLabel->SetTop( 9 ) ;
	m_pCastleTitleLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pCastleTitleLabel->SetCaption( CRYLStringTable::m_strString[4007] ) ;
	m_pCastleTitleLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleTitleLabel ) ;

	m_pCastleTitleValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleTitleValue->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 11, 37, 244, 47 ) ;
	m_pCastleTitleValue->SetAlignRect( rt ) ;
	m_pCastleTitleValue->SetAlign( CENTER ) ;
	m_pCastleTitleValue->SetCaption( "" ) ;
	m_pCastleTitleValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleTitleValue ) ;

	m_pCastleAreaLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleAreaLabel->SetLeft( 10 ) ;
	m_pCastleAreaLabel->SetTop( 70 ) ;
	m_pCastleAreaLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pCastleAreaLabel->SetCaption( CRYLStringTable::m_strString[4008] ) ;
	m_pCastleAreaLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleAreaLabel ) ;

	m_pMeterLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMeterLabel->SetLeft( 109 ) ;
	m_pMeterLabel->SetTop( 98 ) ;
	m_pMeterLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pMeterLabel->SetCaption( "m" ) ;
	m_pMeterLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pMeterLabel ) ;

	m_pPercentLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPercentLabel->SetLeft( 237 ) ;
	m_pPercentLabel->SetTop( 98 ) ;
	m_pPercentLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pPercentLabel->SetCaption( "%" ) ;
	m_pPercentLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPercentLabel ) ;

	m_pCastleAreaValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleAreaValue->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 11, 98, 102, 108 ) ;
	m_pCastleAreaValue->SetAlignRect( rt ) ;
	m_pCastleAreaValue->SetAlign( CENTER ) ;
	m_pCastleAreaValue->SetCaption( "" ) ;
	m_pCastleAreaValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleAreaValue ) ;

	m_pCastleHarvestValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleHarvestValue->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 139, 98, 230, 108 ) ;
	m_pCastleHarvestValue->SetAlignRect( rt ) ;
	m_pCastleHarvestValue->SetAlign( CENTER ) ;
	m_pCastleHarvestValue->SetCaption( "" ) ;
	m_pCastleHarvestValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleHarvestValue ) ;

	m_pCastleDefenceLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleDefenceLabel->SetLeft( 10 ) ;
	m_pCastleDefenceLabel->SetTop( 131 ) ;
	m_pCastleDefenceLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pCastleDefenceLabel->SetCaption( CRYLStringTable::m_strString[4009] ) ;
	m_pCastleDefenceLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleDefenceLabel ) ;

	m_pCastleDefenceValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleDefenceValue->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 11, 159, 244, 169 ) ;
	m_pCastleDefenceValue->SetAlignRect( rt ) ;
	m_pCastleDefenceValue->SetAlign( CENTER ) ;
	m_pCastleDefenceValue->SetCaption( "" ) ;
	m_pCastleDefenceValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleDefenceValue ) ;

	m_pCastleIncomeLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleIncomeLabel->SetLeft( 10 ) ;
	m_pCastleIncomeLabel->SetTop( 192 ) ;
	m_pCastleIncomeLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pCastleIncomeLabel->SetCaption( CRYLStringTable::m_strString[4010] ) ;
	m_pCastleIncomeLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleIncomeLabel ) ;

	m_pCastleIncomeValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCastleIncomeValue->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 11, 220, 187, 230 ) ;
	m_pCastleIncomeValue->SetAlignRect( rt ) ;
	m_pCastleIncomeValue->SetAlign( CENTER ) ;
	m_pCastleIncomeValue->SetCaption( "" ) ;
	m_pCastleIncomeValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCastleIncomeValue ) ;

	m_pGoldLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGoldLabel->SetLeft( 194 ) ;
	m_pGoldLabel->SetTop( 220 ) ;
	m_pGoldLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pGoldLabel->SetCaption( "Gold" ) ;
	m_pGoldLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGoldLabel ) ;
}


VOID	CRYLCastleStatusPanel::FinalCtrl() 
{
	GM_DELETE( m_pCastleTitleLabel ) ;
	GM_DELETE( m_pCastleTitleValue ) ;
	GM_DELETE( m_pCastleAreaLabel ) ;
	GM_DELETE( m_pMeterLabel ) ;
	GM_DELETE( m_pPercentLabel ) ;
	GM_DELETE( m_pCastleAreaValue ) ;
	GM_DELETE( m_pCastleHarvestValue ) ;
	GM_DELETE( m_pCastleDefenceLabel ) ;
	GM_DELETE( m_pCastleDefenceValue ) ;
	GM_DELETE( m_pCastleIncomeLabel ) ;
	GM_DELETE( m_pCastleIncomeValue ) ;
	GM_DELETE( m_pGoldLabel ) ;
}


VOID CRYLCastleStatusPanel::Show( CGMUIObject* Sender )
{
}


VOID CRYLCastleStatusPanel::Hide( CGMUIObject* Sender )
{
}


HRESULT		CRYLCastleStatusPanel::Update()
{
	CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCurrCastleID ) ;
	if ( pCastle )
	{
		char szBuffer[ MAX_PATH ] ;
		
		unsigned char cTitle = g_CastleBlessMgr.GetTitle( pCastle->GetTotalGainTaxCount(), pCastle->GetUpgradeStep() ) ;
		sprintf( szBuffer, "%s%s¼º (+%d)", pCastle->GetTitleName( cTitle ), pCastle->GetCastleName(), pCastle->GetUpgradeStep() ) ;
		m_pCastleTitleValue->SetCaption( szBuffer ) ;

		sprintf( szBuffer, "%4.1f", g_CastleBlessMgr.GetBlessArea( pCastle->GetTotalGainTaxCount(), pCastle->GetUpgradeStep() ) ) ;
		m_pCastleAreaValue->SetCaption( szBuffer ) ;

		sprintf( szBuffer, "%d", g_CastleBlessMgr.GetBonusPercent( pCastle->GetTotalGainTaxCount(), pCastle->GetUpgradeStep() ) ) ;
		m_pCastleHarvestValue->SetCaption( szBuffer ) ;

		sprintf( szBuffer, CRYLStringTable::m_strString[4011], pCastle->GetTotalSiegeCount() ) ;
		m_pCastleDefenceValue->SetCaption( szBuffer ) ;

		m_pCastleIncomeValue->SetCaption( CRYLStringTable::MakePrintGold( pCastle->GetTotalTaxMoney() ) ) ;
	}

	return S_OK ;
}

VOID		CRYLCastleStatusPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
}


