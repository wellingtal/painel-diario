#include <winsock2.h>
#include "RYLGuildInclinationDlg.h"
#include "RYLButton.h" 
#include "RYLListBox.h"
#include "RYLSpriteEx.h"
#include "RYLLabel.h"
#include "RYLSprite.h"
#include "..\\RYLBitmap.h"
#include "Texture.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "WinInput.h"

#include "RYLStringGrid.h"
#include "RYLMessageBox.h"
#include "RYLGuildInfoPanel.h"

#include <Network/ClientSocket/Send/SendCommunity.h>
#include <RenderOption.h>
CRYLGuildInclinationDlg*	g_pGuildInclinationDlg = NULL ;


CRYLGuildInclinationDlg::CRYLGuildInclinationDlg( CRYLGuildInfoPanel* pInfoPanel ) : CRYLDialog()
{
	RECT rt ;
	CGMImageDib*	pImageDIB		= NULL ;
	CRYLImage*		pImage			= NULL ;

	m_pInfoPanel					= pInfoPanel;
	m_pInclinationChangeLabel		= NULL;			// 길드 성향 변경 텍스트
	m_pInclinationListLabel			= NULL;			// 길드 성향 리스트 텍스트

	m_pInclinationStringGrid		= NULL;			// 성향 리스트


	m_pInclinationDescLabel			= NULL;			// 성향 설명 텍스트
	m_pInclinationDescValueLabel1	= NULL;			// 성향 설명 값 텍스트
	m_pInclinationDescValueLabel2	= NULL;			// 성향 설명 값 텍스트
	m_pInclinationDescValueLabel3	= NULL;			// 성향 설명 값 텍스트
	m_pInclinationDescValueLabel4	= NULL;			// 성향 설명 값 텍스트

	m_pInclinationAbleLabel			= NULL;			// 길드 가입 가능 텍스트
	m_pInclinationAbleValueLabel	= NULL;			// 길드 가입 가능 값 텍스트

	m_pInclinationNationLabel		= NULL;			// 길드 국적 텍스트
	m_pInclinationNationValueLabel	= NULL;			// 길드 국적 값 텍스트

	m_pInclinationPeaceLabel		= NULL;			// 길드 전쟁/평화 성향 텍스트
	m_pInclinationPeaceValueLabel	= NULL;			// 길드 전쟁/평화 성향 값 텍스트

	m_pInclinationCautionLabel1		= NULL;			// 주의 텍스트
	m_pInclinationCautionLabel2		= NULL;			// 주의 텍스트
	m_pInclinationOKButton			= NULL;
	m_pInclinationCancelButton		= NULL;

	
	CRYLSpriteList::Instance()->CreateGMImage( 256, 10, &pImageDIB ) ;						//맨 윗 틀
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 10 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	
	pImage = new CRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;


	/*
	CRYLSpriteList::Instance()->CreateGMImage( 256, 450, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	
	pImageDIB->DrawFrameRect( 182, 0, 143, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 182, 20, 143, 1, 0xffa88a60 ) ;

	pImage = new CRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	*/
	
	

	CRYLSpriteList::Instance()->CreateGMImage( 256, 450, &pImageDIB ) ;			//테두리 밑 다이얼로그 바탕
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	//pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	//pImageDIB->DrawFrameRect( 182, 0, 143, 21, 0xc0000000 ) ;
	
	for ( int i = 0 ; i < 85 ; i ++ )
	{
		SetRect( &rt, 0, 38, 184, 43 ) ;
		//SetRect( &rt, 0, 29, 143, 33 );
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
		SetRect( &rt, 72, 53, 144, 58 ) ;
		pImageDIB->DrawRect( &rt, 184, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	}
	
	SetRect( &rt, 0, 43, 184, 53 ) ;
	pImageDIB->DrawRect( &rt, 0, (i-1) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	SetRect( &rt, 144, 53, 216, 63 ) ;
	pImageDIB->DrawRect( &rt, 184, (i-1) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	
	

	pImageDIB->MakeFrameRect( 1, 0,  254,  25, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 6,  50, 243, 139, 0xffB99E6B, 0xc0000000,  1 ) ;		// 길드 성향 리스트 박스
	pImageDIB->MakeFrameRect( 6, 210, 243, 75, 0xffB99E68, 0xc0000000, 1 );			// 길드 성향 설명 박스
	pImageDIB->MakeFrameRect( 169, 289, 80, 17, 0xff959595, 0xc0000000, 1 );			// 종족 박스
	pImageDIB->MakeFrameRect( 169, 312, 80, 17, 0xff959595, 0xc0000000, 1 );			// 국적 박스
	pImageDIB->MakeFrameRect( 169, 335, 80, 17, 0xff959595, 0xc0000000, 1 );			// 전쟁/평화 성향 박스


//	pImageDIB->MakeFrameRect( 7, 238, 313, 121, 0xffB99E6B, 0xc0000000,  1 ) ;

//	pImageDIB->MakeFrameRect( 13, 50,  304,  1, 0xff959595, 0xc0000000,  1 ) ;
//	pImageDIB->MakeFrameRect( 23, 4,  106,  19, 0xff959595, 0xc0000000,  1 ) ;
//	pImageDIB->MakeFrameRect( 74, 250,  115,  19, 0xff959595, 0xc0000000,  1 ) ;
//	pImageDIB->MakeFrameRect( 74, 276,  115,  19, 0xff959595, 0xc0000000,  1 ) ;
//	pImageDIB->MakeFrameRect( 74, 302,  115,  19, 0xff959595, 0xc0000000,  1 ) ;
	
	
	pImage = new CRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 450 ) ;

//	SetLeft( 100 ) ;
//	SetTop( 100 ) ;
		

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
		
	m_cNation = NATION_AKHAN;
	g_pGuildInclinationDlg = this;
	InitCtrl();
}


CRYLGuildInclinationDlg::~CRYLGuildInclinationDlg()
{
	
	FinalCtrl();
	g_pGuildInclinationDlg = NULL;
	m_pInfoPanel = NULL;
}

VOID
CRYLGuildInclinationDlg::FinalCtrl() 
{
	GM_DELETE( m_pInclinationChangeLabel	);			// 길드 성향 변경 텍스트
	GM_DELETE( m_pInclinationListLabel		);			// 길드 성향 리스트 텍스트

	GM_DELETE( m_pInclinationStringGrid		);			// 성향 리스트


	GM_DELETE( m_pInclinationDescLabel		);			// 성향 설명 텍스트
	GM_DELETE( m_pInclinationDescValueLabel1);			// 성향 설명 값 텍스트
	GM_DELETE( m_pInclinationDescValueLabel2);			// 성향 설명 값 텍스트
	GM_DELETE( m_pInclinationDescValueLabel3);			// 성향 설명 값 텍스트
	GM_DELETE( m_pInclinationDescValueLabel4);			// 성향 설명 값 텍스트


	GM_DELETE( m_pInclinationAbleLabel		);			// 길드 가입 가능 텍스트
	GM_DELETE( m_pInclinationAbleValueLabel	);			// 길드 가입 가능 값 텍스트

	GM_DELETE( m_pInclinationNationLabel	);			// 길드 국적 텍스트
	GM_DELETE( m_pInclinationNationValueLabel);			// 길드 국적 값 텍스트

	GM_DELETE( m_pInclinationPeaceLabel		);			// 길드 전쟁/평화 성향 텍스트
	GM_DELETE( m_pInclinationPeaceValueLabel);			// 길드 전쟁/평화 성향 값 텍스트

	GM_DELETE( m_pInclinationCautionLabel1	);			// 주의 텍스트
	GM_DELETE( m_pInclinationCautionLabel2	);			// 주의 텍스트
	GM_DELETE( m_pInclinationOKButton		);
	GM_DELETE( m_pInclinationCancelButton	);
}

VOID
CRYLGuildInclinationDlg::InitCtrl ()
{
	const int offsetY = 10;
	CGMImageDib* pSprite = NULL;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;


	// 길드 성향 변경 텍스트
	m_pInclinationChangeLabel = new CRYLLabel( this ) ;
	m_pInclinationChangeLabel->SetLeft( 9 ) ;
	m_pInclinationChangeLabel->SetTop( 7 + offsetY ) ;
	m_pInclinationChangeLabel->SetFontColor (0xffffffff);
	m_pInclinationChangeLabel->SetAutoSize( TRUE ) ;
	m_pInclinationChangeLabel->SetCaption( "길드성향변경" ) ;
	AttachGMUIControl( m_pInclinationChangeLabel ) ;


	// 길드 성향 리스트 텍스트
	m_pInclinationListLabel = new CRYLLabel( this ) ;
	m_pInclinationListLabel->SetLeft( 9 ) ;
	m_pInclinationListLabel->SetTop( 34 + offsetY ) ;
	m_pInclinationListLabel->SetFontColor (0xffffffff);
	m_pInclinationListLabel->SetAutoSize( TRUE ) ;
	m_pInclinationListLabel->SetCaption( "길드성향리스트" ) ;
	AttachGMUIControl( m_pInclinationListLabel ) ;

	// 성향 리스트
	m_pInclinationStringGrid = new CRYLStringGrid( this, FALSE ) ;
	m_pInclinationStringGrid->SetLeft( 6 ) ;
	m_pInclinationStringGrid->SetTop( 54 + offsetY ) ;
	m_pInclinationStringGrid->SetWidth( 200 ) ;
	m_pInclinationStringGrid->SetHeight( 148 ) ;
	m_pInclinationStringGrid->SetColCount( 8 ) ;
	m_pInclinationStringGrid->SetItemHeight( 16 ) ;
	m_pInclinationStringGrid->SetFontColor( 0xffffffff ) ;
	m_pInclinationStringGrid->SetInstance( RYLGUILDINCLINATIONDLG::MEMBER_STRINGGRID ) ;
	m_pInclinationStringGrid->OnClickEvent = DlgClick ;
	AttachGMUIControl( m_pInclinationStringGrid ) ;
	UpdateStringGrid();


	// 성향 설명 텍스트
	m_pInclinationDescLabel = new CRYLLabel( this ) ;
	m_pInclinationDescLabel->SetLeft( 9 ) ;
	m_pInclinationDescLabel->SetTop( 195 + offsetY ) ;
	m_pInclinationDescLabel->SetFontColor (0xffffffff);
	m_pInclinationDescLabel->SetAutoSize( TRUE ) ;
	m_pInclinationDescLabel->SetCaption( "길드성향 설명" ) ;
	AttachGMUIControl( m_pInclinationDescLabel ) ;
	

	// 성향 설명 값 텍스트
	m_pInclinationDescValueLabel1 = new CRYLLabel( this ) ;
	m_pInclinationDescValueLabel1->SetLeft( 16 ) ;
	m_pInclinationDescValueLabel1->SetTop( 218 + offsetY ) ;
	m_pInclinationDescValueLabel1->SetFontColor (0xffffffff);
	m_pInclinationDescValueLabel1->SetAutoSize( TRUE ) ;
	m_pInclinationDescValueLabel1->SetCaption( "길드" ) ;
	AttachGMUIControl( m_pInclinationDescValueLabel1 ) ;

	m_pInclinationDescValueLabel2 = new CRYLLabel( this ) ;
	m_pInclinationDescValueLabel2->SetLeft( 16 ) ;
	m_pInclinationDescValueLabel2->SetTop( 218 + offsetY +13) ;
	m_pInclinationDescValueLabel2->SetFontColor (0xffffffff);
	m_pInclinationDescValueLabel2->SetAutoSize( TRUE ) ;
	m_pInclinationDescValueLabel2->SetCaption( "성향" ) ;
	AttachGMUIControl( m_pInclinationDescValueLabel2 ) ;

	m_pInclinationDescValueLabel3 = new CRYLLabel( this ) ;
	m_pInclinationDescValueLabel3->SetLeft( 16 ) ;
	m_pInclinationDescValueLabel3->SetTop( 218 + offsetY +26) ;
	m_pInclinationDescValueLabel3->SetFontColor (0xffffffff);
	m_pInclinationDescValueLabel3->SetAutoSize( TRUE ) ;
	m_pInclinationDescValueLabel3->SetCaption( "설명" ) ;
	AttachGMUIControl( m_pInclinationDescValueLabel3 ) ;

	m_pInclinationDescValueLabel4 = new CRYLLabel( this ) ;
	m_pInclinationDescValueLabel4->SetLeft( 16 ) ;
	m_pInclinationDescValueLabel4->SetTop( 218 + offsetY +39) ;
	m_pInclinationDescValueLabel4->SetFontColor (0xffffffff);
	m_pInclinationDescValueLabel4->SetAutoSize( TRUE ) ;
	m_pInclinationDescValueLabel4->SetCaption( "!!!" ) ;
	AttachGMUIControl( m_pInclinationDescValueLabel4 ) ;


	// 길드 가입 가능 텍스트
	m_pInclinationAbleLabel = new CRYLLabel( this ) ;
	m_pInclinationAbleLabel->SetLeft( 69 ) ;
	m_pInclinationAbleLabel->SetTop( 293 + offsetY ) ;
	m_pInclinationAbleLabel->SetFontColor (0xffffffff);
	m_pInclinationAbleLabel->SetAutoSize( TRUE ) ;
	m_pInclinationAbleLabel->SetCaption( "가입 가능한 종족" ) ;
	AttachGMUIControl( m_pInclinationAbleLabel ) ;

	// 길드 가입 가능 값 텍스트
	m_pInclinationAbleValueLabel = new CRYLLabel( this ) ;
	m_pInclinationAbleValueLabel->SetLeft( 186 ) ;
	m_pInclinationAbleValueLabel->SetTop( 293 + offsetY ) ;
	m_pInclinationAbleValueLabel->SetFontColor (0xffffffff);
	m_pInclinationAbleValueLabel->SetAutoSize( TRUE ) ;
	m_pInclinationAbleValueLabel->SetCaption( "양쪽모두" ) ;
	AttachGMUIControl( m_pInclinationAbleValueLabel ) ;

	// 길드 국적 텍스트
	m_pInclinationNationLabel = new CRYLLabel( this ) ;
	m_pInclinationNationLabel->SetLeft( 98 ) ;
	m_pInclinationNationLabel->SetTop( 316 + offsetY ) ;
	m_pInclinationNationLabel->SetFontColor (0xffffffff);
	m_pInclinationNationLabel->SetAutoSize( TRUE ) ;
	m_pInclinationNationLabel->SetCaption( "길드의 국적" ) ;
	AttachGMUIControl( m_pInclinationNationLabel ) ;

	// 길드 국적 값 텍스트
	m_pInclinationNationValueLabel = new CRYLLabel( this ) ;
	m_pInclinationNationValueLabel->SetLeft( 180 ) ;
	m_pInclinationNationValueLabel->SetTop( 316 + offsetY ) ;
	m_pInclinationNationValueLabel->SetFontColor (0xffffffff);
	m_pInclinationNationValueLabel->SetAutoSize( TRUE ) ;
	m_pInclinationNationValueLabel->SetCaption( "카르테란트" ) ;
	AttachGMUIControl( m_pInclinationNationValueLabel ) ;

	// 길드 전쟁/평화 성향 텍스트
	m_pInclinationPeaceLabel = new CRYLLabel( this ) ;
	m_pInclinationPeaceLabel->SetLeft( 82 ) ;
	m_pInclinationPeaceLabel->SetTop( 339 + offsetY ) ;
	m_pInclinationPeaceLabel->SetFontColor (0xffffffff);
	m_pInclinationPeaceLabel->SetAutoSize( TRUE ) ;
	m_pInclinationPeaceLabel->SetCaption( "전쟁/평화성향" ) ;
	AttachGMUIControl( m_pInclinationPeaceLabel ) ;

	// 길드 전쟁/평화 성향 값 텍스트
	m_pInclinationPeaceValueLabel = new CRYLLabel( this ) ;
	m_pInclinationPeaceValueLabel->SetLeft( 186 ) ;
	m_pInclinationPeaceValueLabel->SetTop( 339 + offsetY ) ;
	m_pInclinationPeaceValueLabel->SetFontColor (0xffffffff);
	m_pInclinationPeaceValueLabel->SetAutoSize( TRUE ) ;
	m_pInclinationPeaceValueLabel->SetCaption( "전쟁모드" ) ;
	AttachGMUIControl( m_pInclinationPeaceValueLabel ) ;

	// 주의 텍스트
	m_pInclinationCautionLabel1 = new CRYLLabel( this ) ;
	m_pInclinationCautionLabel1->SetLeft( 10 ) ;
	m_pInclinationCautionLabel1->SetTop( 363 + offsetY ) ;
	m_pInclinationCautionLabel1->SetFontColor (0xffffffff);
	m_pInclinationCautionLabel1->SetAutoSize( TRUE ) ;
	m_pInclinationCautionLabel1->SetCaption("*주의 : 길드성향을 변경하시면 24시간이내" ) ;
	AttachGMUIControl( m_pInclinationCautionLabel1 ) ;
	
	m_pInclinationCautionLabel2 = new CRYLLabel( this ) ;
	m_pInclinationCautionLabel2->SetLeft( 10 ) ;
	m_pInclinationCautionLabel2->SetTop( 363 + offsetY + 13) ;
	m_pInclinationCautionLabel2->SetFontColor (0xffffffff);
	m_pInclinationCautionLabel2->SetAutoSize( TRUE ) ;
	m_pInclinationCautionLabel2->SetCaption("        재변경이 불가능 합니다." ) ;
	AttachGMUIControl( m_pInclinationCautionLabel2 ) ;


	// 성향 변경 확인 버튼
	m_pInclinationOKButton = new CRYLButton( this ) ;
	m_pInclinationOKButton->SetLeft( 113 ) ;
	m_pInclinationOKButton->SetTop( 398 + offsetY ) ;
	m_pInclinationOKButton->SetCaption( CRYLStringTable::m_strString[ 125 ] ) ;
	m_pInclinationOKButton->SetFontColor( 0xffffffff) ;
	m_pInclinationOKButton->SetColor( 0xffffffff) ;
	m_pInclinationOKButton->OnClickEvent = DlgClick ;
	m_pInclinationOKButton->SetInstance( RYLGUILDINCLINATIONDLG::INCLINATIONOK_BUTTON ) ;
	m_pInclinationOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pInclinationOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pInclinationOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pInclinationOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pInclinationOKButton ) ;


	// 성향 변경 취소 버튼
	m_pInclinationCancelButton = new CRYLButton( this ) ;
	m_pInclinationCancelButton->SetLeft(183 ) ;
	m_pInclinationCancelButton->SetTop( 398 + offsetY ) ;
	m_pInclinationCancelButton->SetCaption( CRYLStringTable::m_strString[ 126 ] ) ;
	m_pInclinationCancelButton->SetFontColor( 0xffffffff) ;
	m_pInclinationCancelButton->SetColor( 0xffffffff) ;
	m_pInclinationCancelButton->OnClickEvent = DlgClick ;
	m_pInclinationCancelButton->SetInstance( RYLGUILDINCLINATIONDLG::INCLINATIONCANCEL_BUTTON ) ;
	m_pInclinationCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pInclinationCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pInclinationCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pInclinationCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pInclinationCancelButton ) ;


}


VOID				
CRYLGuildInclinationDlg::UpdateStringGrid()
{
	// test
	CHAR Buff[ 64 ] ;
	m_pInclinationStringGrid->ClearItem();

	switch( m_cNation )
	{
	case NATION_AKHAN:
	sprintf( Buff, "아칸 카르테란트 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "아칸 메르카디아 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "아칸 제삼세력 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "아칸 평화 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
		break;
	case NATION_HUMAN:
	sprintf( Buff, "인간 카르테란트 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "인간 메르카디아 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "인간 제삼세력 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "인간 평화 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
		break;
	case NATION_UNKNONE:
	default:
		break;
	}

	sprintf( Buff, "혼종 카르테란트 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "혼종 메르카디아 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "혼종 제삼세력 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	sprintf( Buff, "혼종 평화 길드");
	m_pInclinationStringGrid->AddItem( 0, Buff, 128, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
	return;


}
	

VOID		
CRYLGuildInclinationDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		
CRYLGuildInclinationDlg::Update( BOOL &bClick, BOOL &bEdge )
{
	if(g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}
	return S_OK ;
}

VOID 
CRYLGuildInclinationDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pGuildInclinationDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLGUILDINCLINATIONDLG::MEMBER_STRINGGRID:
		{
			//i = g_pGuildInclinationDlg->m_pInclinationStringGrid->GetItemIndex();
			g_pGuildInclinationDlg->SetInclination();
			//g_pGuildInclinationDlg->SetInclinationType( );
			break;
		}

		case RYLGUILDINCLINATIONDLG::INCLINATIONOK_BUTTON:
		{
			unsigned char		type;
			type = g_pGuildInclinationDlg->m_pInclinationStringGrid->GetItemIndex();
			type = List2Inclination( type ) ;
			SendPacket::CharGuildInclination(  g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, type ) ;
			g_pGuildInclinationDlg->SetVisible( FALSE );

			if( g_pGuildInclinationDlg->m_pInfoPanel )
			{
				//g_pGuildInclinationDlg->UpdateMyGuildInfo( -1 );
			}
			break;
		}

		case RYLGUILDINCLINATIONDLG::INCLINATIONCANCEL_BUTTON:
		{
			g_pGuildInclinationDlg->SetInclinationType( g_pGuildInclinationDlg->m_cInclinationType );
			g_pGuildInclinationDlg->SetVisible( FALSE );
			break;
		}

		default:	break;
	}
}

VOID	
CRYLGuildInclinationDlg::DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( !g_pGuildInclinationDlg )
		return ;

}


VOID	
CRYLGuildInclinationDlg::SetNation( unsigned char cNation )
{
	if( g_pGuildInclinationDlg )
	{
		if( cNation != g_pGuildInclinationDlg->m_cNation )
		{
			g_pGuildInclinationDlg->m_cNation = cNation;
			g_pGuildInclinationDlg->UpdateStringGrid();
		}
	}
}


VOID				
CRYLGuildInclinationDlg::SetInclinationType( unsigned char type )
{
	unsigned char		cType;
	UpdateLabel( type );
	cType = Inclination2List( type );
	m_pInclinationStringGrid->SelectItemIndex( cType );
    
}


VOID				
CRYLGuildInclinationDlg::SetInclination()
{
	unsigned char		type;
	type = m_pInclinationStringGrid->GetItemIndex();
	SetInclinationType( List2Inclination( type ) );
	
}

VOID				
// 현재 선택된 성향으로 설명 문구 체인지
CRYLGuildInclinationDlg::UpdateLabel( unsigned char type )	
{
	switch( type )
	{
		case RYLGUILDINCLINATIONDLG::HUMAN_KARTERANT_WAR:
			m_pInclinationDescValueLabel1->SetCaption("인간 길드의 기본 형태입니다.   ");
			m_pInclinationDescValueLabel2->SetCaption("카르테란트는 기본적으로 인간의 ");
			m_pInclinationDescValueLabel3->SetCaption("국가입니다.                    ");
			m_pInclinationDescValueLabel4->SetCaption("");
			m_pInclinationAbleValueLabel->SetCaption(" 인간 ");
			m_pInclinationNationValueLabel->SetCaption("카르테란트");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
		break;
		case RYLGUILDINCLINATIONDLG::HUMAN_MERKADIA_WAR:
			m_pInclinationDescValueLabel1->SetCaption("인간에 대해 반기를 든 인간들의");
			m_pInclinationDescValueLabel2->SetCaption("길드.");
			m_pInclinationDescValueLabel3->SetCaption("메르카디아는 기본적으로 아칸의");
			m_pInclinationDescValueLabel4->SetCaption("국가입니다.");
			m_pInclinationAbleValueLabel->SetCaption(" 인간 ");
			m_pInclinationNationValueLabel->SetCaption("메르카디아");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			break;
		case RYLGUILDINCLINATIONDLG::HUMAN_NATION_THIRD_WAR:
			m_pInclinationDescValueLabel1->SetCaption("새로운 세력을 꿈꾸는 인간들의");
			m_pInclinationDescValueLabel2->SetCaption("길드.");
			m_pInclinationDescValueLabel3->SetCaption("같은 제삼세력들만이 이들의 아군");
			m_pInclinationDescValueLabel4->SetCaption("이 됩니다.");
			m_pInclinationAbleValueLabel->SetCaption(" 인간 ");
			m_pInclinationNationValueLabel->SetCaption("써드네이션");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			break;
		case RYLGUILDINCLINATIONDLG::HUMAN_NATION_THIRD_PEACE:
			m_pInclinationDescValueLabel1->SetCaption("인간들만 받아들이지만 전쟁을");
			m_pInclinationDescValueLabel2->SetCaption("반대하는 입장을 가진이들의 길드");
			m_pInclinationDescValueLabel3->SetCaption("입니다.");
			m_pInclinationDescValueLabel4->SetCaption("");
			m_pInclinationAbleValueLabel->SetCaption(" 인간 ");
			m_pInclinationNationValueLabel->SetCaption("써드네이션");
			m_pInclinationPeaceValueLabel->SetCaption("평화모드");
			break;
		case RYLGUILDINCLINATIONDLG::AKHAN_KARTERANT_WAR:
			m_pInclinationDescValueLabel1->SetCaption("아칸에 대해 반기를 든 아칸들의");
			m_pInclinationDescValueLabel2->SetCaption("길드");
			m_pInclinationDescValueLabel3->SetCaption("카르테란트는 기본적으로 인간의");
			m_pInclinationDescValueLabel4->SetCaption("국가입니다.");
			m_pInclinationAbleValueLabel->SetCaption(" 아칸 ");
			m_pInclinationNationValueLabel->SetCaption("카르테란트");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			break;
		case RYLGUILDINCLINATIONDLG::AKHAN_MERKADIA_WAR:
			m_pInclinationDescValueLabel1->SetCaption("아칸 길드의 기본 형태 입니다.");
			m_pInclinationDescValueLabel2->SetCaption("메르카디아는 기본적으로 아칸의");
			m_pInclinationDescValueLabel3->SetCaption("국가입니다.");
			m_pInclinationDescValueLabel4->SetCaption("");
			m_pInclinationAbleValueLabel->SetCaption(" 아칸 ");
			m_pInclinationNationValueLabel->SetCaption("메르카디아");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			break;
		case RYLGUILDINCLINATIONDLG::AKHAN_NATION_THIRD_WAR:
			m_pInclinationDescValueLabel1->SetCaption("새로운 세력을 꿈꾸는 아칸들의");
			m_pInclinationDescValueLabel2->SetCaption("길드.");
			m_pInclinationDescValueLabel3->SetCaption("같은 제삼세력들만이 이들의 아군");
			m_pInclinationDescValueLabel4->SetCaption("이 됩니다.");
			m_pInclinationAbleValueLabel->SetCaption(" 아칸 ");
			m_pInclinationNationValueLabel->SetCaption("써드네이션");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			break;
		case RYLGUILDINCLINATIONDLG::AKHAN_NATION_THIRD_PEACE:
			m_pInclinationDescValueLabel1->SetCaption("아칸들만 받아들이지만 전쟁을");
			m_pInclinationDescValueLabel2->SetCaption("반대하는 입장을 가진이들의 길드");
			m_pInclinationDescValueLabel3->SetCaption("입니다.");
			m_pInclinationDescValueLabel4->SetCaption("");
			m_pInclinationAbleValueLabel->SetCaption(" 아칸 ");
			m_pInclinationNationValueLabel->SetCaption("써드네이션");
			m_pInclinationPeaceValueLabel->SetCaption("평화모드");
			break;
		case RYLGUILDINCLINATIONDLG::BOTH_KARTERANT_WAR:
			m_pInclinationDescValueLabel1->SetCaption("혼종으로 이루어진 카르테란트 국");
			m_pInclinationDescValueLabel2->SetCaption("적 길드입니다.");
			m_pInclinationDescValueLabel3->SetCaption("카르테란트는 기본적으로 인간의");
			m_pInclinationDescValueLabel4->SetCaption("국가입니다.");
			m_pInclinationAbleValueLabel->SetCaption("양쪽모두");
			m_pInclinationNationValueLabel->SetCaption("카르테란트");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			break;
		case RYLGUILDINCLINATIONDLG::BOTH_MERKADIA_WAR:
			m_pInclinationDescValueLabel1->SetCaption("혼종으로 이루어진 메르카디아 국");
			m_pInclinationDescValueLabel2->SetCaption("적 길드입니다.");
			m_pInclinationDescValueLabel3->SetCaption("메르카디아는 기본적으로 아칸의");
			m_pInclinationDescValueLabel4->SetCaption("국가입니다.");
			m_pInclinationAbleValueLabel->SetCaption("양쪽모두");
			m_pInclinationNationValueLabel->SetCaption("메르카디아");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			break;
		case RYLGUILDINCLINATIONDLG::BOTH_NATION_THIRD_WAR:
			m_pInclinationDescValueLabel1->SetCaption("양 종족이 혼용된 통합 제삼세력");
			m_pInclinationDescValueLabel2->SetCaption("생성이 목적인 길드입니다.");
			m_pInclinationDescValueLabel3->SetCaption("같은 제삼세력들만이 이들의 아군");
			m_pInclinationDescValueLabel4->SetCaption("이 됩니다.");
			m_pInclinationAbleValueLabel->SetCaption("양쪽모두");
			m_pInclinationNationValueLabel->SetCaption("써드네이션");
			m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			break;
		case RYLGUILDINCLINATIONDLG::BOTH_NATION_THIRD_PEACE:
			m_pInclinationDescValueLabel1->SetCaption("종족을 뛰어넘는 안전 평화 목적");
			m_pInclinationDescValueLabel2->SetCaption("의 길드. 모두가 행복환 세계를");
			m_pInclinationDescValueLabel3->SetCaption("지향합니다.");
			m_pInclinationDescValueLabel4->SetCaption("");
			m_pInclinationAbleValueLabel->SetCaption("양쪽모두");
			m_pInclinationNationValueLabel->SetCaption("써드네이션");
			m_pInclinationPeaceValueLabel->SetCaption("평화모드");
			break;
		default:
			if( m_cNation == NATION_AKHAN )
			{
				m_pInclinationDescValueLabel1->SetCaption("아칸 길드의 기본 형태 입니다.");
				m_pInclinationDescValueLabel2->SetCaption("메르카디아는 기본적으로 아칸의");
				m_pInclinationDescValueLabel3->SetCaption("국가입니다.");
				m_pInclinationDescValueLabel4->SetCaption("");
				m_pInclinationAbleValueLabel->SetCaption(" 아칸 ");
				m_pInclinationNationValueLabel->SetCaption("메르카디아");
				m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");
			}
			else
			{
				m_pInclinationDescValueLabel1->SetCaption("인간 길드의 기본 형태입니다.   ");
				m_pInclinationDescValueLabel2->SetCaption("카르테란트는 기본적으로 인간의 ");
				m_pInclinationDescValueLabel3->SetCaption("국가입니다.                    ");
				m_pInclinationDescValueLabel4->SetCaption("");
				m_pInclinationAbleValueLabel->SetCaption(" 인간 ");
				m_pInclinationNationValueLabel->SetCaption("카르테란트");
				m_pInclinationPeaceValueLabel->SetCaption("전쟁모드");

			}
			break;

	}


}


// 성향 타입(비트값) -> 리스트 타입(순차적)
unsigned char		
CRYLGuildInclinationDlg::Inclination2List( unsigned char type )
{
	unsigned char res = 0;
	if( g_pGuildInclinationDlg )
	{
		switch( type )
		{
		case RYLGUILDINCLINATIONDLG::HUMAN_KARTERANT_WAR:
			if( NATION_HUMAN == g_pGuildInclinationDlg->m_cNation )
				res = 0;
			break;
		case RYLGUILDINCLINATIONDLG::HUMAN_MERKADIA_WAR:
			if( NATION_HUMAN == g_pGuildInclinationDlg->m_cNation )
				res = 1;
			break;
		case RYLGUILDINCLINATIONDLG::HUMAN_NATION_THIRD_WAR:
			if( NATION_HUMAN == g_pGuildInclinationDlg->m_cNation )
				res = 2;
			break;
		case RYLGUILDINCLINATIONDLG::HUMAN_NATION_THIRD_PEACE:
			if( NATION_HUMAN == g_pGuildInclinationDlg->m_cNation )
				res = 3;
			break;
		case RYLGUILDINCLINATIONDLG::AKHAN_KARTERANT_WAR:
			if( NATION_AKHAN == g_pGuildInclinationDlg->m_cNation )
				res = 0;
			break;
		case RYLGUILDINCLINATIONDLG::AKHAN_MERKADIA_WAR:
			if( NATION_AKHAN == g_pGuildInclinationDlg->m_cNation )
				res = 1;
			break;
		case RYLGUILDINCLINATIONDLG::AKHAN_NATION_THIRD_WAR:
			if( NATION_AKHAN == g_pGuildInclinationDlg->m_cNation )
				res = 2;
			break;
		case RYLGUILDINCLINATIONDLG::AKHAN_NATION_THIRD_PEACE:
			if( NATION_AKHAN == g_pGuildInclinationDlg->m_cNation )
				res = 3;
			break;
		case RYLGUILDINCLINATIONDLG::BOTH_KARTERANT_WAR:
			res = 4;
			break;
		case RYLGUILDINCLINATIONDLG::BOTH_MERKADIA_WAR:
			res = 5;
			break;
		case RYLGUILDINCLINATIONDLG::BOTH_NATION_THIRD_WAR:
			res = 6;
			break;
		case RYLGUILDINCLINATIONDLG::BOTH_NATION_THIRD_PEACE:
			res = 7;
			break;
		}

	}
	return res;
}


// 리스트 타입(순차적) -> 성향 타입(비트값)
unsigned char		
CRYLGuildInclinationDlg::List2Inclination( unsigned char type )			
{

	unsigned char res = 0;
	if( g_pGuildInclinationDlg )
	{
		switch( type )
		{
		case 0 :
			if( NATION_AKHAN == g_pGuildInclinationDlg->m_cNation )
				res = RYLGUILDINCLINATIONDLG::AKHAN_KARTERANT_WAR;
			else
				res = RYLGUILDINCLINATIONDLG::HUMAN_KARTERANT_WAR;
			break;
		case 1:
			if( NATION_AKHAN == g_pGuildInclinationDlg->m_cNation )
				res = RYLGUILDINCLINATIONDLG::AKHAN_MERKADIA_WAR;
			else
				res = RYLGUILDINCLINATIONDLG::HUMAN_MERKADIA_WAR;
			break;
		case 2:
			if( NATION_AKHAN == g_pGuildInclinationDlg->m_cNation )
				res = RYLGUILDINCLINATIONDLG::AKHAN_NATION_THIRD_WAR;
			else
				res = RYLGUILDINCLINATIONDLG::HUMAN_NATION_THIRD_WAR;
			break;
		case 3:
			if( NATION_AKHAN == g_pGuildInclinationDlg->m_cNation )
				res = RYLGUILDINCLINATIONDLG::AKHAN_NATION_THIRD_PEACE;
			else
				res = RYLGUILDINCLINATIONDLG::HUMAN_NATION_THIRD_PEACE;
			break;
		case 4:
			res = RYLGUILDINCLINATIONDLG::BOTH_KARTERANT_WAR;
			break;
		case 5:
			res = RYLGUILDINCLINATIONDLG::BOTH_MERKADIA_WAR;
			break;
		case 6:
			res = RYLGUILDINCLINATIONDLG::BOTH_NATION_THIRD_WAR;
			break;
		case 7:
			res = RYLGUILDINCLINATIONDLG::BOTH_NATION_THIRD_PEACE;
			break;
		}
	}
	return res;
}

