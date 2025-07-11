#include <winsock2.h>
#include "RYLPartyFindPanel.h"
#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLCheckBox.h"
#include "RYLListBox.h"
#include "RYLStringGrid.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLChattingDlg.h"
#include "RYLCommunityData.h"
#include "RYLSpriteList.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLChattingDlg.h"
#include "RYLEdit.h"

#include <Network/ClientSocket/Send/SendCommunity.h>
#include "GMMemory.h"


CRYLPartyFindPanel*		g_pPartyFindPanel = NULL ;

CRYLPartyFindPanel::CRYLPartyFindPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pNameLabel			= NULL ;			// 이름
	m_pClassLabel			= NULL ;			// 클레스
	m_pLevelLabel			= NULL ;			// 레벨
	m_pLeaderNameLabel		= NULL ;			// 리더이름
	m_pMemberNumLabel		= NULL ;			// 멤버수
	m_pAverageLevelLabel	= NULL ;			// 평균레벨

	m_pPartyForMemberPartyGrid = NULL ;
	m_MemberForPartyMemberGrid = NULL ;

	m_pGMUIParentDlg = pParentDlg ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	RECT			rt ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 395, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( i - 1 ) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 7,  21, 242, 149, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 7, 194, 242, 149, 0xffB99E6B, 0xc0000000,  1 ) ;
	
	pImageDIB->MakeFrameRect( 18, 37, 219, 1, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 18, 210, 219, 1, 0xff959595, 0xc0000000,  1 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 412 ) ;

	g_pPartyFindPanel = this ;
}

CRYLPartyFindPanel::~CRYLPartyFindPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLPartyFindPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 파티를 찾고 있는 개인
	m_pPartyFindPersonalLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPartyFindPersonalLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 7, 6, 190, 17 ) ;
	m_pPartyFindPersonalLabel->SetAlignRect( rt ) ;
	m_pPartyFindPersonalLabel->SetAlign( LEFT ) ;
	m_pPartyFindPersonalLabel->SetCaption( CRYLStringTable::m_strString[ 1969 ] ) ;
	m_pPartyFindPersonalLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPartyFindPersonalLabel ) ;

	// 파티원을 찾고 있는 파티
	m_pPartyMemberFindPartyLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPartyMemberFindPartyLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 7, 179, 190, 190 ) ;
	m_pPartyMemberFindPartyLabel->SetAlignRect( rt ) ;
	m_pPartyMemberFindPartyLabel->SetAlign( LEFT ) ;
	m_pPartyMemberFindPartyLabel->SetCaption( CRYLStringTable::m_strString[ 1970 ] ) ;
	m_pPartyMemberFindPartyLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPartyMemberFindPartyLabel ) ;

	// 등록
	m_pEntryLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pEntryLabel1->SetFontColor (0xff80ff80);
	SetRect( &rt, 190, 6, 228, 17 ) ;
	m_pEntryLabel1->SetAlignRect( rt ) ;
	m_pEntryLabel1->SetAlign( RIGHT ) ;
	m_pEntryLabel1->SetCaption( CRYLStringTable::m_strString[ 1971 ] ) ;
	m_pEntryLabel1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pEntryLabel1 ) ;

	// 등록
	m_pEntryLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pEntryLabel2->SetFontColor (0xff80ff80);
	SetRect( &rt, 190, 179, 228, 190 ) ;
	m_pEntryLabel2->SetAlignRect( rt ) ;
	m_pEntryLabel2->SetAlign( RIGHT ) ;
	m_pEntryLabel2->SetCaption( CRYLStringTable::m_strString[ 1971 ] ) ;
	m_pEntryLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pEntryLabel2 ) ;

	// 등록 체크
/*
	m_pEntryCheckBox1 = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	//m_pEntryCheckBox1->SetLeft( 232 ) ;
	//m_pEntryCheckBox1->SetTop( 3 ) ;
	//m_pEntryCheckBox1->SetParent( this ) ;
	m_pEntryCheckBox1->SetLeft( 10 ) ;
	m_pEntryCheckBox1->SetTop( 345 ) ;
	m_pEntryCheckBox1->SetInstance( RYLPARTYFINDPANEL::FINDPERSONAL_CHECKBOX ) ;
	m_pEntryCheckBox1->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEntryCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEntryCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEntryCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEntryCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pEntryCheckBox1->SetChecked( FALSE ) ;
	CGMUICustomPanel::AddChild( m_pEntryCheckBox1 ) ;
*/
	m_pEntryCheckBox1 = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pEntryCheckBox1->SetLeft( 232 ) ;
	m_pEntryCheckBox1->SetTop( 3 ) ;
	m_pEntryCheckBox1->SetSecondParent( this ) ;
	m_pEntryCheckBox1->SetInstance( RYLPARTYFINDPANEL::FINDPERSONAL_CHECKBOX ) ;
	m_pEntryCheckBox1->OnMouseUpEvent = PartyFindMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEntryCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEntryCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEntryCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEntryCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pEntryCheckBox1 ) ;

	// 등록 체크
	m_pEntryCheckBox2 = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pEntryCheckBox2->SetLeft( 232 ) ;
	m_pEntryCheckBox2->SetTop( 176 ) ;
	m_pEntryCheckBox2->SetSecondParent( this ) ;
	m_pEntryCheckBox2->SetInstance( RYLPARTYFINDPANEL::MEMBERFINDPARTY_CHECKBOX ) ;
	m_pEntryCheckBox2->OnMouseUpEvent = PartyFindMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEntryCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEntryCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEntryCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEntryCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pEntryCheckBox2->SetChecked( FALSE ) ;
	CGMUICustomPanel::AddChild( m_pEntryCheckBox2 ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	// 새로고침	
	m_pRefreshButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pRefreshButton->SetLeft( 182 ) ;
	m_pRefreshButton->SetTop( 345 ) ;
	m_pRefreshButton->SetCaption( CRYLStringTable::m_strString[ 1972 ] ) ;
	m_pRefreshButton->SetFontColor( 0xffffffff) ;
	m_pRefreshButton->SetColor( 0xffffffff) ;
	m_pRefreshButton->OnClickEvent = DlgClick ;
	m_pRefreshButton->SetInstance( RYLPARTYFINDPANEL::REFRESH_BUTTON ) ;
	m_pRefreshButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRefreshButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRefreshButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRefreshButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pRefreshButton ) ;

	m_pNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 24, 24, 116, 35 ) ;
	m_pNameLabel->SetAlignRect( rt ) ;
	m_pNameLabel->SetAlign( CENTER ) ;
	m_pNameLabel->SetCaption( CRYLStringTable::m_strString[ 1973 ] ) ;
	m_pNameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNameLabel ) ;

	m_pClassLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pClassLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 116, 24, 198, 35 ) ;
	m_pClassLabel->SetAlignRect( rt ) ;
	m_pClassLabel->SetAlign( CENTER ) ;
	m_pClassLabel->SetCaption( CRYLStringTable::m_strString[ 1928 ] ) ;
	m_pClassLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pClassLabel ) ;

	m_pLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLevelLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 198, 24, 235, 35 ) ;
	m_pLevelLabel->SetAlignRect( rt ) ;
	m_pLevelLabel->SetAlign( CENTER ) ;
	m_pLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1930 ] ) ;
	m_pLevelLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabel ) ;

	m_pLeaderNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLeaderNameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 24, 197, 116, 208 ) ;
	m_pLeaderNameLabel->SetAlignRect( rt ) ;
	m_pLeaderNameLabel->SetAlign( CENTER ) ;
	m_pLeaderNameLabel->SetCaption( CRYLStringTable::m_strString[ 1974 ] ) ;
	m_pLeaderNameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLeaderNameLabel ) ;

	m_pMemberNumLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMemberNumLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 116, 197, 198, 208 ) ;
	m_pMemberNumLabel->SetAlignRect( rt ) ;
	m_pMemberNumLabel->SetAlign( CENTER ) ;
	m_pMemberNumLabel->SetCaption( CRYLStringTable::m_strString[ 1975 ] ) ;
	m_pMemberNumLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pMemberNumLabel ) ;

	m_pAverageLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pAverageLevelLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 198, 197, 235, 208 ) ;
	m_pAverageLevelLabel->SetAlignRect( rt ) ;
	m_pAverageLevelLabel->SetAlign( CENTER ) ;
	m_pAverageLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1976 ] ) ;
	m_pAverageLevelLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pAverageLevelLabel ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	// 파티원을 찾고 있는 파티
	m_pPartyForMemberPartyGrid = new CRYLStringGrid( m_pGMUIParentDlg, FALSE ) ;
	m_pPartyForMemberPartyGrid->SetLeft( 24 ) ;
	m_pPartyForMemberPartyGrid->SetTop( 42 ) ;
	m_pPartyForMemberPartyGrid->SetWidth( 221 ) ;
	m_pPartyForMemberPartyGrid->SetHeight( 123 ) ;
	m_pPartyForMemberPartyGrid->SetColCount( 10 ) ;
	m_pPartyForMemberPartyGrid->SetFontColor( 0xffffffff ) ;
	m_pPartyForMemberPartyGrid->SetInstance( RYLPARTYFINDPANEL::PFMP_STRINGGRID ) ;
	m_pPartyForMemberPartyGrid->OnClickEvent = DlgClick ;
	
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pPartyForMemberPartyGrid->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 186, 240, 200 ) ;
	m_pPartyForMemberPartyGrid->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 186, 226, 200 ) ;
	m_pPartyForMemberPartyGrid->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pPartyForMemberPartyGrid->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 235, 164, 243, 184 ) ;
 	m_pPartyForMemberPartyGrid->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pPartyForMemberPartyGrid->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pPartyForMemberPartyGrid->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pPartyForMemberPartyGrid->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 198, 200, 212, 214 ) ;
	m_pPartyForMemberPartyGrid->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 200, 240, 214 ) ;					  
	m_pPartyForMemberPartyGrid->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 200, 226, 214 ) ;					  
	m_pPartyForMemberPartyGrid->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 200, 212, 214 ) ;					  
	m_pPartyForMemberPartyGrid->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;
 	
	CGMUICustomPanel::AddChild( m_pPartyForMemberPartyGrid) ;

	// 파티를 찾고 있는 개인 리스트 박스
	m_MemberForPartyMemberGrid = new CRYLStringGrid( m_pGMUIParentDlg, FALSE ) ;
	m_MemberForPartyMemberGrid->SetLeft( 24 ) ;
	m_MemberForPartyMemberGrid->SetTop( 213 ) ;
	m_MemberForPartyMemberGrid->SetWidth( 221 ) ;
	m_MemberForPartyMemberGrid->SetHeight( 123 ) ;
	m_MemberForPartyMemberGrid->SetColCount( 10 ) ;
	m_MemberForPartyMemberGrid->SetFontColor( 0xffffffff ) ;
	m_MemberForPartyMemberGrid->SetInstance( RYLPARTYFINDPANEL::PFMP_STRINGGRID ) ;
	m_MemberForPartyMemberGrid->OnClickEvent = DlgClick ;
	
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_MemberForPartyMemberGrid->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 186, 240, 200 ) ;
	m_MemberForPartyMemberGrid->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 186, 226, 200 ) ;
	m_MemberForPartyMemberGrid->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_MemberForPartyMemberGrid->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 235, 164, 243, 184 ) ;
 	m_MemberForPartyMemberGrid->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_MemberForPartyMemberGrid->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_MemberForPartyMemberGrid->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_MemberForPartyMemberGrid->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 198, 200, 212, 214 ) ;
	m_MemberForPartyMemberGrid->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 200, 240, 214 ) ;					  
	m_MemberForPartyMemberGrid->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 200, 226, 214 ) ;					  
	m_MemberForPartyMemberGrid->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 200, 212, 214 ) ;					  
	m_MemberForPartyMemberGrid->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;
 	
	CGMUICustomPanel::AddChild( m_MemberForPartyMemberGrid ) ;
}

VOID	CRYLPartyFindPanel::FinalCtrl() 
{
	GM_DELETE( m_pPartyFindPersonalLabel ) ;	
	GM_DELETE( m_pPartyMemberFindPartyLabel ) ;
	GM_DELETE( m_pEntryLabel1 ) ;			
	GM_DELETE( m_pEntryLabel2 ) ;			
	GM_DELETE( m_pEntryCheckBox1 ) ;			
	GM_DELETE( m_pEntryCheckBox2 ) ;			
	GM_DELETE( m_pRefreshButton ) ;			

	GM_DELETE( m_pNameLabel ) ;				
	GM_DELETE( m_pClassLabel ) ;				
	GM_DELETE( m_pLevelLabel ) ;				
	GM_DELETE( m_pLeaderNameLabel ) ;		
	GM_DELETE( m_pMemberNumLabel ) ;			
	GM_DELETE( m_pAverageLevelLabel ) ;		
	GM_DELETE( m_pPartyForMemberPartyGrid ) ;
	GM_DELETE( m_MemberForPartyMemberGrid ) ;

}

HRESULT		CRYLPartyFindPanel::UpdateMemberForPartyMember() 
{
	CRYLCommunityData* pCommunity = CRYLCommunityData::Instance() ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	m_pPartyForMemberPartyGrid->ClearItem() ;				// 파티를 찾고 있는 개인 리스트 박스
	if ( pCommunity->m_cNumMember )
	{
		char strLevel[5], strClass[30];
		for ( INT i = 0 ; i < 30 ; i ++ )
		{
			if ( !pCommunity->m_mfpMember[ i ].m_cLevel ) 
				break ;

			sprintf( strLevel, "%d", pCommunity->m_mfpMember[ i ].m_cLevel ) ;
			strcpy( strClass, pGame->GetClassName( pCommunity->m_mfpMember[ i ].m_cClass ) ) ;

			m_pPartyForMemberPartyGrid->AddItem( 0, pCommunity->m_mfpMember[i].m_strName, 93, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
			m_pPartyForMemberPartyGrid->AddItem( 1, strClass, 81, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
			m_pPartyForMemberPartyGrid->AddItem( 2, strLevel, 21, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
		}
	}

	return S_OK ;
}

HRESULT		CRYLPartyFindPanel::UpdatePartyForMemberParty() 
{
	CRYLCommunityData* pCommunity = CRYLCommunityData::Instance() ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	m_MemberForPartyMemberGrid->ClearItem() ;				// 파티원을 찾고 있는 파티
	if ( pCommunity->m_cNumParty )
	{
		char strLevel[5], strNum[5];
		for ( INT i = 0 ; i < 9 ; i ++ )
		{
			if ( !pCommunity->m_pfmParty[ i ].m_cAverageLevel ) 
				break ;

			sprintf( strLevel, "%d", pCommunity->m_pfmParty[ i ].m_cAverageLevel ) ;
			sprintf( strNum, "%d", pCommunity->m_pfmParty[ i ].m_cMemberNum ) ;

			m_MemberForPartyMemberGrid->AddItem( 0, pCommunity->m_pfmParty[ i ].m_strLeaderName, 93, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
			m_MemberForPartyMemberGrid->AddItem( 1, strNum, 81, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
			m_MemberForPartyMemberGrid->AddItem( 2, strLevel, 21, D3DCOLOR_RGBA( 255, 255, 255, 255) ) ;
		}
	}
	return S_OK ;
}

HRESULT		CRYLPartyFindPanel::Update()
{
	/*	
	if(m_ButtonManager.m_lstButton[81]->m_bShow && g_DeviceInput.InRect(m_rcWindowSize.left + 15, m_rcWindowSize.top + 396, m_rcWindowSize.left + 33, m_rcWindowSize.top + 413) && g_DeviceInput.GetIsLeftMouseDown())
	{
		long lCounter;
		if((lCounter = g_ClientMain.SendPartyCmd(!g_ClientMain.m_bPartyFind, ClientConstants::PartyCmd_FindParty)) != 0)
		{
			char strText[100];
			sprintf(strText, g_StringTable.m_strString[223], lCounter / 1000.0f);
			g_TextEdit.AddMessage(strText, g_ClientMain.m_dwMyChrID, 129);
		}
	}

	if(m_ButtonManager.m_lstButton[82]->m_bShow && g_DeviceInput.InRect(m_rcWindowSize.left + 15, m_rcWindowSize.top + 417, m_rcWindowSize.left + 33, m_rcWindowSize.top + 434) && g_DeviceInput.GetIsLeftMouseDown())
	{
		long lCounter;
		if((lCounter = g_ClientMain.SendPartyCmd(!g_ClientMain.m_bPartyPlayerFind, ClientConstants::PartyCmd_FindMember)) != 0)
		{
			char strText[100];
			sprintf(strText, g_StringTable.m_strString[223], lCounter / 1000.0f);
			g_TextEdit.AddMessage(strText, g_ClientMain.m_dwMyChrID, 129);
		}
	}

	if(m_ButtonManager.m_lstButton[83]->m_nState == BUTTON_CLICK)
	{
		long lCounter;
		if((lCounter = g_ClientMain.SendPartyFind()) != 0)
		{
			char strText[100];
			sprintf(strText, g_StringTable.m_strString[223], lCounter / 1000.0f);
			g_TextEdit.AddMessage(strText, g_ClientMain.m_dwMyChrID, 129);
		} else
		{
			g_ClientMain.m_dwSelectMemberSlot = 0xFFFFFFFF;
			g_ClientMain.m_dwSelectPartySlot = 0xFFFFFFFF;
		}
	}

	if(m_ButtonManager.m_lstButton[84]->m_nState == BUTTON_CLICK)
	{
		unsigned long dwSelect = 0xFFFFFFFF;
		BOOL bParty = FALSE;
		if(g_ClientMain.m_dwSelectMemberSlot != 0xFFFFFFFF) { dwSelect = g_ClientMain.m_dwSelectMemberSlot; bParty = FALSE; }
		if(g_ClientMain.m_dwSelectPartySlot != 0xFFFFFFFF) { dwSelect = g_ClientMain.m_dwSelectPartySlot; bParty = TRUE; }

		if(dwSelect != 0xFFFFFFFF)
		{
			if(bParty)
				strcpy(g_ClientMain.m_strWhisperChrID, g_ClientMain.m_pfmParty[dwSelect].m_strLeaderName);
			else
				strcpy(g_ClientMain.m_strWhisperChrID, g_ClientMain.m_mfpMember[dwSelect].m_strName);
			g_ClientMain.m_bInstanceWhisper = TRUE;
			g_TextEdit.SetFocus();
		}
	}*/
	return S_OK ;
}

VOID		CRYLPartyFindPanel::PartyFindEvent() 
{
	if ( CRYLNetworkData::m_lPartyCmdCounter <= 0 )
	{
		CRYLNetworkData::Instance()->SendPartyCmd( g_pPartyFindPanel->m_pEntryCheckBox1->GetChecked(), ClientConstants::PartyCmd_FindParty ) ;
		CRYLNetworkData::m_lPartyCmdCounter = 3000 ;

		if ( g_pPartyFindPanel->m_pEntryCheckBox1->GetChecked() )
		{
			// 파티 찾기 버튼 클릭시 채팅창에 내용 표시 루틴 구현중!! 임시로 막아두기!!
/*			CRYLGameData* pGame = CRYLGameData::Instance() ;
			RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
			if ( pSelfCreature )
			{
				CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
				if ( pSelfData )
				{
					char szMessage[ MAX_PATH ] ;
					sprintf( szMessage, CRYLStringTable::m_strString[ 2908 ],
						pGame->m_csStatus.m_Info.Name,
						pGame->GetClassName(pGame->m_csStatus.m_Info.Class),
						pGame->m_csStatus.m_Info.Level );

					CRYLNetworkData::SendChatMessage( NULL, szMessage, PktChat::FIND_PARTY );
				}
			}
*/
		}
	}
	else
	{
		g_pPartyFindPanel->m_pEntryCheckBox1->SetChecked( !g_pPartyFindPanel->m_pEntryCheckBox1->GetChecked() ) ;
		
		char strText[ 100 ] ;
		sprintf(strText, CRYLStringTable::m_strString[ 223 ], CRYLNetworkData::m_lPartyCmdCounter / 1000.0f ) ;
		CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
	}
}

VOID		CRYLPartyFindPanel::PartyPlayerFind() 
{
	if ( CRYLNetworkData::m_lPartyCmdCounter <= 0 )
	{
		CRYLNetworkData::Instance()->SendPartyCmd( g_pPartyFindPanel->m_pEntryCheckBox2->GetChecked(), ClientConstants::PartyCmd_FindMember ) ;
		CRYLNetworkData::m_lPartyCmdCounter = 3000 ;
	}
	else
	{
		g_pPartyFindPanel->m_pEntryCheckBox2->SetChecked( !g_pPartyFindPanel->m_pEntryCheckBox2->GetChecked() ) ;

		char strText[100];
		sprintf(strText, CRYLStringTable::m_strString[223], CRYLNetworkData::m_lPartyCmdCounter / 1000.0f);
		CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
	}
}

VOID		CRYLPartyFindPanel::UpdateCheckBox(BOOL bFindChar, BOOL bFindParty)
{
	g_pPartyFindPanel->m_pEntryCheckBox1->SetChecked( bFindChar ) ;
	g_pPartyFindPanel->m_pEntryCheckBox2->SetChecked( bFindParty ) ;
}

VOID		CRYLPartyFindPanel::RefreshEvent()
{
	LONG	lCounter;
	if ( ( lCounter = CRYLNetworkData::Instance()->SendPartyFind()) != 0 )
	{
		char strText[100];
		sprintf(strText, CRYLStringTable::m_strString[223], lCounter / 1000.0f);
		CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
	} else
	{
		CRYLCommunityData::Instance()->m_dwSelectMemberSlot = 0xFFFFFFFF;
		CRYLCommunityData::Instance()->m_dwSelectPartySlot = 0xFFFFFFFF;
	}
}

VOID		CRYLPartyFindPanel::PartyFindWhisperEvent() 
{
	INT iIndex = m_pPartyForMemberPartyGrid->GetItemIndex() ;
	if ( iIndex == -1 )		return ;

	char strText[MAX_PATH] ;
	strcpy( strText, "/s ") ;
	strcat( strText, CRYLCommunityData::Instance()->m_mfpMember[iIndex].m_strName ) ;
	strcat( strText, " ");

	strcpy( CRYLCommunityData::Instance()->m_strWhisperChrID, CRYLCommunityData::Instance()->m_mfpMember[iIndex].m_strName ) ;
    CRYLCommunityData::Instance()->m_bInstanceWhisper = TRUE ;
	CRYLChattingDlg::Instance()->m_pChatEdit->SetText( strText ) ;
	//CRYLChattingDlg::Instance()->SetWhisperMode() ;
	CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus() ;
	CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( TRUE ) ;
}

VOID		CRYLPartyFindPanel::PartyPlayerWhisperEvent() 
{
	INT iIndex = m_MemberForPartyMemberGrid->GetItemIndex() ;
	if ( iIndex == -1 )		return ;

	char strText[MAX_PATH] ;
	strcpy( strText, "/s ") ;
	strcat( strText, CRYLCommunityData::Instance()->m_pfmParty[iIndex].m_strLeaderName ) ;
	strcat( strText, " ");

	strcpy( CRYLCommunityData::Instance()->m_strWhisperChrID, CRYLCommunityData::Instance()->m_pfmParty[iIndex].m_strLeaderName ) ;
    CRYLCommunityData::Instance()->m_bInstanceWhisper = TRUE ;
	CRYLChattingDlg::Instance()->m_pChatEdit->SetText( strText ) ;
	//CRYLChattingDlg::Instance()->SetWhisperMode() ;
	CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus() ;
	CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( TRUE ) ;
}

VOID		CRYLPartyFindPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pPartyFindPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLPARTYFINDPANEL::REFRESH_BUTTON :
		{
			g_pPartyFindPanel->RefreshEvent() ;
		}
		break ;
	case RYLPARTYFINDPANEL::PFMP_STRINGGRID :
		{
			g_pPartyFindPanel->PartyFindWhisperEvent() ;
		}
		break ;
	case RYLPARTYFINDPANEL::MFPM_STRINGGRID :
		{
			g_pPartyFindPanel->PartyPlayerWhisperEvent() ;
		}
		break ;
	}
}

VOID		CRYLPartyFindPanel::PartyFindMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( !g_pPartyFindPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLPARTYFINDPANEL::FINDPERSONAL_CHECKBOX :
		g_pPartyFindPanel->PartyFindEvent() ;
		break ;
	case RYLPARTYFINDPANEL::MEMBERFINDPARTY_CHECKBOX :
		g_pPartyFindPanel->PartyPlayerFind() ;
		break ;
	}
}