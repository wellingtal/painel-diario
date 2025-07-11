#include <winsock2.h>
#include "RYLQuestOpeningDlg.h"
#include "BaseDataDefine.h"
#include "RYLButton.h"
#include "RYLListBox.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLMessageBox.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"

#include "RYLNetworkData.h"
#include "RYLGameData.h"
#include "RYLStringTable.h"

#include <Quest/QuestList.h>

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendQuest.h>
#include <RenderOption.h>
#include "GMMemory.h"
CRYLQuestOpeningDlg*		g_QuestOpeninigDlg = NULL ;

CRYLQuestOpeningDlg::CRYLQuestOpeningDlg() 
{
	m_pQuestNPCTitleLabel			= NULL ;	// 퀘스트
	m_pQuestLevelLabel				= NULL ;	// 퀘스트 난위도
	m_pQuestLevelValueLabel			= NULL ;	// 퀘스트 난위도 값
	m_pQuestCompensationLabel		= NULL ;	// 퀘스트 보상
	m_pAcceptButton					= NULL ;	// 수락버튼
	m_pRefuseButton					= NULL ;	// 거절버튼
	m_pQuestPurposeButton			= NULL ;	// 퀘스트목적 리스트
	m_pQuestCompensationButton		= NULL ;	// 퀘스트보상 리스트
	m_dwChrID						= 0L ;
	m_dwQuestID						= 0L ;
	m_lStartDesc					= 0 ;
	//----------------------------------------------------------------

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 397, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	for ( int i = 0 ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 387, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 0, 251, 22,  0xc0000000, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 2, 20, 251, 1,  0xffB99E6B, 0x80000000,  1 ) ;

	pImageDIB->MakeFrameRect( 7, 68, 242, 197,  0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 7, 289, 242, 67,  0xffB99E6B, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 68, 24, 148, 19,  0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 68, 46, 72, 19,  0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 419 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;
	g_QuestOpeninigDlg = this ;
}

CRYLQuestOpeningDlg::~CRYLQuestOpeningDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLQuestOpeningDlg::InitCtrl() 
{
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 퀘스트 오프닝
	m_pQuestOpeningLabel = new CRYLLabel( this ) ;		
	m_pQuestOpeningLabel->SetLeft( 9 ) ;
	m_pQuestOpeningLabel->SetTop( 18 ) ;
	m_pQuestOpeningLabel->SetFontColor (0xffffffff);
	m_pQuestOpeningLabel->SetAutoSize( TRUE ) ;
	m_pQuestOpeningLabel->SetCaption( CRYLStringTable::m_strString[ 1951 ] ) ;
	AttachGMUIControl( m_pQuestOpeningLabel ) ;

	// 퀘스트
	m_pQuestNPCTitleLabel = new CRYLLabel( this ) ;		
	m_pQuestNPCTitleLabel->SetLeft( 70 ) ;
	m_pQuestNPCTitleLabel->SetTop( 43 ) ;
	m_pQuestNPCTitleLabel->SetFontColor (0xffffffff);
	m_pQuestNPCTitleLabel->SetAutoSize( TRUE ) ;
	m_pQuestNPCTitleLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pQuestNPCTitleLabel ) ;

	// 퀘스트 제목
	m_pQuestTitleLabel = new CRYLLabel( this ) ;		
	m_pQuestTitleLabel->SetLeft( 6 ) ;
	m_pQuestTitleLabel->SetTop( 43 ) ;
	m_pQuestTitleLabel->SetFontColor (0xffffffff);
	m_pQuestTitleLabel->SetAutoSize( TRUE ) ;
	m_pQuestTitleLabel->SetCaption( CRYLStringTable::m_strString[ 1952 ] ) ;
	AttachGMUIControl( m_pQuestTitleLabel ) ;
 
	// 퀘스트 난위도
	m_pQuestLevelLabel = new CRYLLabel( this ) ;		
	m_pQuestLevelLabel->SetLeft( 6 ) ;
	m_pQuestLevelLabel->SetTop( 65 ) ;
	m_pQuestLevelLabel->SetFontColor (0xffffffff);
	m_pQuestLevelLabel->SetAutoSize( TRUE ) ;
	m_pQuestLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1953 ] ) ;
	AttachGMUIControl( m_pQuestLevelLabel ) ;

	// 퀘스트 난위도 값
	m_pQuestLevelValueLabel = new CRYLLabel( this ) ;		
	m_pQuestLevelValueLabel->SetLeft( 70 ) ;
	m_pQuestLevelValueLabel->SetTop( 65 ) ;
	m_pQuestLevelValueLabel->SetFontColor (0xffffffff);
	m_pQuestLevelValueLabel->SetAutoSize( TRUE ) ;
	m_pQuestLevelValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pQuestLevelValueLabel ) ;

	// 퀘스트 보상
	m_pQuestCompensationLabel = new CRYLLabel( this ) ;		
	m_pQuestCompensationLabel->SetLeft( 11 ) ;
	m_pQuestCompensationLabel->SetTop( 289 ) ;
	m_pQuestCompensationLabel->SetFontColor (0xffffffff);
	m_pQuestCompensationLabel->SetAutoSize( TRUE ) ;
	m_pQuestCompensationLabel->SetCaption( CRYLStringTable::m_strString[ 1954 ] ) ;
	AttachGMUIControl( m_pQuestCompensationLabel ) ;

	// 수락버튼	
	m_pAcceptButton = new CRYLButton( this ) ;
	m_pAcceptButton->SetLeft( 89 ) ;
	m_pAcceptButton->SetTop( 378 ) ;
	m_pAcceptButton->SetCaption( CRYLStringTable::m_strString[ 1955 ] ) ;
	m_pAcceptButton->SetFontColor( 0xffffffff) ;
	m_pAcceptButton->SetColor( 0xffffffff) ;
	m_pAcceptButton->OnClickEvent = DlgClick ;
	m_pAcceptButton->SetInstance( RYLQUESTOPENINGDLG::ACCEPT_BUTTON ) ;
	m_pAcceptButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pAcceptButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pAcceptButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pAcceptButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pAcceptButton ) ;

	// 거절버튼
	m_pRefuseButton = new CRYLButton( this ) ;
	m_pRefuseButton->SetLeft( 171 ) ;
	m_pRefuseButton->SetTop( 378 ) ;
	m_pRefuseButton->SetCaption( CRYLStringTable::m_strString[ 1956 ] ) ;
	m_pRefuseButton->SetFontColor( 0xffffffff) ;
	m_pRefuseButton->SetColor( 0xffffffff) ;
	m_pRefuseButton->OnClickEvent = DlgClick ;
	m_pRefuseButton->SetInstance( RYLQUESTOPENINGDLG::REFUSE_BUTTON ) ;
	m_pRefuseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRefuseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRefuseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRefuseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pRefuseButton ) ;

	RECT rtRect ;
	pSprite	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	// 퀘스트목적 리스트
	m_pQuestPurposeButton = new CRYLListBox( this, TRUE ) ;
	m_pQuestPurposeButton->SetLeft( 8 ) ;
	m_pQuestPurposeButton->SetTop( 87 ) ;
	m_pQuestPurposeButton->SetWidth( 237 ) ;
	m_pQuestPurposeButton->SetHeight( 189 ) ;
	
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pQuestPurposeButton->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 186, 240, 200 ) ;
	m_pQuestPurposeButton->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 186, 226, 200 ) ;
	m_pQuestPurposeButton->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pQuestPurposeButton->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 235, 164, 243, 184 ) ;
 	m_pQuestPurposeButton->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestPurposeButton->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestPurposeButton->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestPurposeButton->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 198, 200, 212, 214 ) ;
	m_pQuestPurposeButton->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 200, 240, 214 ) ;					  
	m_pQuestPurposeButton->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 200, 226, 214 ) ;					  
	m_pQuestPurposeButton->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 200, 212, 214 ) ;					  
	m_pQuestPurposeButton->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;
 
	m_pQuestPurposeButton->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pQuestPurposeButton ) ;

	// 퀘스트보상 리스트
	m_pQuestCompensationButton = new CRYLListBox( this, TRUE ) ;
	m_pQuestCompensationButton->SetLeft( 8 ) ;
	m_pQuestCompensationButton->SetTop( 308 ) ;
	m_pQuestCompensationButton->SetWidth( 237 ) ;
	m_pQuestCompensationButton->SetHeight( 60 ) ;
	
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pQuestCompensationButton->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 186, 240, 200 ) ;
	m_pQuestCompensationButton->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 186, 226, 200 ) ;
	m_pQuestCompensationButton->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pQuestCompensationButton->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 235, 164, 243, 184 ) ;
 	m_pQuestCompensationButton->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestCompensationButton->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestCompensationButton->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestCompensationButton->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 198, 200, 212, 214 ) ;
	m_pQuestCompensationButton->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 200, 240, 214 ) ;					  
	m_pQuestCompensationButton->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 200, 226, 214 ) ;					  
	m_pQuestCompensationButton->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 200, 212, 214 ) ;					  
	m_pQuestCompensationButton->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;
 
	m_pQuestCompensationButton->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pQuestCompensationButton ) ;
}

HRESULT		CRYLQuestOpeningDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

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

VOID	CRYLQuestOpeningDlg::FinalCtrl() 
{
	GM_DELETE( m_pQuestOpeningLabel ) ;		 // 퀘스트 오프닝
	GM_DELETE( m_pQuestNPCTitleLabel ) ;	 // 퀘스트
	GM_DELETE( m_pQuestLevelLabel ) ;		 // 퀘스트 난위도
	GM_DELETE( m_pQuestLevelValueLabel ) ;	 // 퀘스트 난위도 값
	GM_DELETE( m_pQuestCompensationLabel ) ; // 퀘스트 보상
	GM_DELETE( m_pQuestTitleLabel ) ;		 // 퀘스트 제목
	GM_DELETE( m_pAcceptButton  ) ;			 // 수락버튼
	GM_DELETE( m_pRefuseButton  ) ;			 // 거절버튼
	GM_DELETE( m_pQuestPurposeButton ) ;	 // 퀘스트목적 리스트
	GM_DELETE( m_pQuestCompensationButton ) ;// 퀘스트보상 리스트
}

VOID	CRYLQuestOpeningDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLQuestOpeningDlg::Hide( CGMUIObject* Sender ) 
{
	vector< LPMessage >::iterator	it ;
	LPMessage						lpMessage ;
	
	for ( it = m_lstMessage.begin() ; it != m_lstMessage.end() ; )
	{
		lpMessage = ( *it ) ;
		it = m_lstMessage.erase( it ) ;
		GM_DELETE( lpMessage ) ;
	}
	m_lstMessage.clear() ;
	m_pQuestPurposeButton->ClearItem() ;

	for ( it = m_lstQuestAward.begin() ; it != m_lstQuestAward.end() ; )
	{
		lpMessage = ( *it ) ;
		it = m_lstQuestAward.erase( it ) ;
		GM_DELETE( lpMessage ) ;
	}
	m_lstQuestAward.clear() ;
	m_pQuestCompensationButton->ClearItem() ;
}

VOID	CRYLQuestOpeningDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLQuestOpeningDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_QuestOpeninigDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLQUESTOPENINGDLG::ACCEPT_BUTTON :
		{
			if (QuestInstance::MAX_REQUEST_QUEST <= CRYLGameData::Instance()->m_csStatus.m_lstQuest.size())
			{
				g_QuestOpeninigDlg->SetVisible( FALSE ) ;

				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2712 ]);

				break;
			}

			LPQuestNode lpQuest = g_QuestList.GetQuest( g_QuestOpeninigDlg->GetQuestID() ) ;
			if ( lpQuest )
			{
				if ( CRYLGameData::Instance()->m_csStatus.CheckQuestInvenSpace(lpQuest) )
				{
					unsigned short wType = lpQuest->GetQuestType();

					if ( ITEM_QUEST == wType )
					{
						SendPacket::CharUseItem(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, CRYLNetworkData::Instance()->m_dwMyChrID, &g_QuestOpeninigDlg->GetQuestItemPos() );
						g_QuestOpeninigDlg->SetVisible( FALSE ) ;
					}
					else if ( NPC_QUEST == wType )
					{
						SendPacket::CharStartQuest(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, g_QuestOpeninigDlg->GetCharID(), g_QuestOpeninigDlg->GetQuestID() ) ; 
						g_QuestOpeninigDlg->SetVisible( FALSE ) ;
					}
				}
				else
				{
					g_QuestOpeninigDlg->SetVisible( FALSE ) ;

					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[ 3569 ]);
				}
			}
		}
		break ;
	case RYLQUESTOPENINGDLG::REFUSE_BUTTON :
		g_QuestOpeninigDlg->SetVisible( FALSE ) ;
		break ;
	}
}


VOID	CRYLQuestOpeningDlg::SetText( DWORD dwChrID, DWORD dwQuestID, LPCTSTR strQuestTitle, LPCSTR strQuestLevel,
									  LPCTSTR strMessage, LPCSTR strQuestAward ) 
{
	m_dwChrID	= dwChrID ;
	m_dwQuestID = dwQuestID ;

	vector<LPMessage>::iterator it ;
	LPMessage					lpMessage ;

	for ( it = m_lstMessage.begin(); it != m_lstMessage.end() ; )
	{
		lpMessage = ( *it ) ;
		it = m_lstMessage.erase( it ) ;
		GM_DELETE( lpMessage ) ;
	}
	m_lstMessage.clear() ;
	m_pQuestPurposeButton->ClearItem() ;

	for ( it = m_lstQuestAward.begin(); it != m_lstQuestAward.end() ; )
	{
		lpMessage = ( *it ) ;
		it = m_lstQuestAward.erase( it ) ;
		GM_DELETE( lpMessage ) ;
	}
	m_lstQuestAward.clear() ;
	m_pQuestCompensationButton->ClearItem() ;

	strcpy( m_strQuestTitle, strQuestTitle ) ;
	m_pQuestNPCTitleLabel->SetCaption( m_strQuestTitle ) ;
	strcpy( m_strQuestLevel, strQuestLevel ) ;
	m_pQuestLevelValueLabel->SetCaption( m_strQuestLevel ) ;

	char *strMessageTemp, *token, seps[] = "\\" ;
	strMessageTemp = CRYLStringTable::MakeWordBreak( strMessage, 220 ) ;
	token = strtok( strMessageTemp, seps ) ;

	while( token != NULL )
	{
		lpMessage = new Message ;
		strcpy( lpMessage->strMessage, token ) ;
		m_pQuestPurposeButton->AddItem( lpMessage->strMessage, 0xffffffff, 0xffffffff, 0xffffffff ) ;
		m_lstMessage.push_back( lpMessage ) ;
		token = strtok(NULL, seps ) ;
	}

	strMessageTemp = CRYLStringTable::MakeWordBreak( strQuestAward, 220 ) ;
	token = strtok( strMessageTemp, seps ) ;

	while( token != NULL )
	{
		lpMessage = new Message ;
		strcpy( lpMessage->strMessage, token ) ;
		m_pQuestCompensationButton->AddItem( lpMessage->strMessage, 0xffffffff, 0xffffffff, 0xffffffff ) ;
		m_lstQuestAward.push_back( lpMessage ) ;
		token = strtok(NULL, seps ) ;
	}
}

