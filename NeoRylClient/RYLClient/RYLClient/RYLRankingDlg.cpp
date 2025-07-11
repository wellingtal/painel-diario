#include <winsock2.h>
#include "RYLRankingDlg.h"
#include "BaseDataDefine.h"

#include "RYLButton.h"
#include "RYLStringGrid.h"
#include "RYLLabel.h"
#include "RYLComboBox.h"

#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"

#include <Network/Packet/PacketStruct/RankingPacket.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Creature/Character/CharacterClass.h>
#include <RenderOption.h>
#include "GMMemory.h"
CRYLRankingDlg*		g_RankingDlg = NULL ;

CRYLRankingDlg::CRYLRankingDlg() 
{
	m_pServerNameLabel				= NULL ;		// �����̸�
	m_pServerNameValueLabel			= NULL ;		// �����̸� ��
	m_pCurrentRankingLabel			= NULL ;		// ���� ��ŷ
	m_pCurrentRankingValueLabel		= NULL ;		// ���� ��ŷ��
	m_pSortLabel					= NULL ;		// ���� ���

	m_pRankingLabel					= NULL ;		// ����
	m_pClassLabel					= NULL ;		// Ŭ����
	m_pNameLabel					= NULL ;		// �̸�
	m_pPointLabel					= NULL ;		// ����Ʈ
	m_pLebelLabel					= NULL ;		// ����

	m_pPrevButton					= NULL ;		// ���� ������
	m_pNextButton					= NULL ;		// ���� ������
	m_pRankingStrGrid				= NULL ;		// ��ŷ ����Ʈ
	m_pClassCBBox					= NULL ;		// Ŭ���� �޺��ڽ�

	m_cCurrentPage					= 0 ;			// ���� ������
	m_iClass						= 0 ;			// �������� Ŭ����
	
	//----------------------------------------------------------------

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 323, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING01_256x256 ) ) ;
	SetRect( &rt, 0, 0, 67, 15 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING02_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 323, 398, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING01_256x256 ) ) ;

	SetRect( &rt, 0, 15, 67, 256 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING02_256x256 ) ) ;
	
	SetRect( &rt, 0, 0, 256, 157) ;
	pImageDIB->DrawRect( &rt, 0, 241, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING03_256x256 ) ) ;
	
	SetRect( &rt, 67, 0, 134, 157) ;
	pImageDIB->DrawRect( &rt, 256, 241, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING02_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
    AttachWindowTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	
	SetWidth( 322 ) ;
	SetHeight( 413 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;
	g_RankingDlg = this ;
}

CRYLRankingDlg::~CRYLRankingDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLRankingDlg::InitCtrl() 
{
	RECT rt ;
	CGMImageDib*			pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// Ŭ���� �޺��ڽ�
	m_pClassCBBox = new CRYLComboBox( this ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pClassCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 213, 201, 225, 213 ) ;
	m_pClassCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 227, 201, 239, 213 ) ;
	m_pClassCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pClassCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;

	switch( CRYLGameData::Instance()->m_dwClientRace )
	{
		case CClass::RaceType::HUMAN : 
		{
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 2227 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 455 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 456 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 457 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//assassin rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 458 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 459 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 460 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//enchanter rank
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 461 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//priest rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 462 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 2227 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//all
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 469 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//templar
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 470 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//attacker
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 471 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//gunner rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 472 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//rune off
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 473 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//life off
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 474 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//shadow
			break ;
		}

		case CClass::RaceType::AKHAN : 
		{
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 2227 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 455 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 456 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 457 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//assassin rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 458 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 459 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 460 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//enchanter rank
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 461 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//priest rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 462 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;

			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 469 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//templar
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 470 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//attacker
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 471 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//gunner rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 472 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//rune off
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 473 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//life off
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 474 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//shadow
			break ;
		}
	}

	m_pClassCBBox->SetColor( 0xff000000 ) ;
		
	m_pClassCBBox->OnClickEvent = DlgClick ;
	m_pClassCBBox->SetLeft( 61 ) ;
	m_pClassCBBox->SetTop( 46 ) ;//set top(higher go down
	m_pClassCBBox->SetWidth( 72 ) ;//set righter(higher go right
	m_pClassCBBox->SetHeight( 15 ) ;
	m_pClassCBBox->SetButtonPos() ;

	m_pClassCBBox->SetFontColor( 0xffffffff ) ;
	m_pClassCBBox->SelectMenuData( 0 ) ;
	
	m_pClassCBBox->GetButton()->SetInstance( RYLRANKINGDLG::RANKING_COMBO ) ;//wasnt commented out, disable Class rank hz
	m_pClassCBBox->GetButton()->OnClickEvent = DlgClick ;
	m_pClassCBBox->GetMenu()->SetInstance( RYLRANKINGDLG::RANKING_MENU ) ;
	m_pClassCBBox->GetMenu()->OnClickEvent = DlgClick ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	CRYLSpriteList::Instance()->CreateGMImage( 77, 181, &pImageDIB ) ;//was 148
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	if( CClass::RaceType::HUMAN == CRYLGameData::Instance()->m_dwClientRace )
	{
		pImageDIB->MakeFrameRect( 1, 1, 76, 180, 0xff959595, 0xff000000,  1 ) ;//was 147
	}
	else
	{
		pImageDIB->MakeFrameRect( 1, 1, 76, 180, 0xff959595, 0xff000000,  1 ) ;//was 116
	}
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pClassCBBox->GetMenu()->AttachMenuImage( pImage ) ;

	AttachGMUIControl( m_pClassCBBox ) ;

	// ��ŷâ
	m_pRankingDlgLabel = new CRYLLabel( this ) ;		
	m_pRankingDlgLabel->SetLeft( 9 ) ;
	m_pRankingDlgLabel->SetTop( 18 ) ;
	m_pRankingDlgLabel->SetFontColor (0xffffffff);
	m_pRankingDlgLabel->SetAutoSize( TRUE ) ;
	m_pRankingDlgLabel->SetCaption( CRYLStringTable::m_strString[ 1943 ] ) ;
	AttachGMUIControl( m_pRankingDlgLabel ) ;

	// �����̸�
	m_pServerNameLabel = new CRYLLabel( this ) ;		
	m_pServerNameLabel->SetLeft( 147 ) ;
	m_pServerNameLabel->SetTop( 47 ) ;
	m_pServerNameLabel->SetFontColor (0xffffffff);
	m_pServerNameLabel->SetAutoSize( TRUE ) ;
	m_pServerNameLabel->SetCaption( CRYLStringTable::m_strString[ 2237 ] ) ;
	AttachGMUIControl( m_pServerNameLabel ) ;

	// �����̸� ��
	m_pServerNameValueLabel = new CRYLLabel( this ) ;		
	m_pServerNameValueLabel->SetLeft( 211 ) ;
	m_pServerNameValueLabel->SetTop( 47 ) ;
	m_pServerNameValueLabel->SetFontColor (0xffffffff);
	m_pServerNameValueLabel->SetAutoSize( TRUE ) ;
	m_pServerNameValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pServerNameValueLabel ) ;

	// ���� ��ŷ
	m_pCurrentRankingLabel = new CRYLLabel( this ) ;		
	m_pCurrentRankingLabel->SetLeft( 190 ) ;
	m_pCurrentRankingLabel->SetTop( 70 ) ;
	m_pCurrentRankingLabel->SetFontColor (0xffffffff);
	m_pCurrentRankingLabel->SetAutoSize( TRUE ) ;
	m_pCurrentRankingLabel->SetCaption( CRYLStringTable::m_strString[ 2238 ] ) ;
	AttachGMUIControl( m_pCurrentRankingLabel ) ;

	// ���� ��ŷ��
	m_pCurrentRankingValueLabel = new CRYLLabel( this ) ;		
	m_pCurrentRankingValueLabel->SetLeft( 255 ) ;
	m_pCurrentRankingValueLabel->SetTop( 70 ) ;
	m_pCurrentRankingValueLabel->SetFontColor (0xffffffff);
	m_pCurrentRankingValueLabel->SetAutoSize( TRUE ) ;
	m_pCurrentRankingValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pCurrentRankingValueLabel ) ;

	// ����
	m_pRankingLabel = new CRYLLabel( this ) ;		
	m_pRankingLabel->SetLeft( 15 ) ;
	m_pRankingLabel->SetTop( 94 ) ;
	m_pRankingLabel->SetFontColor (0xffffffff);
	m_pRankingLabel->SetAutoSize( TRUE ) ;
	m_pRankingLabel->SetCaption( CRYLStringTable::m_strString[ 2013 ] ) ;
	AttachGMUIControl( m_pRankingLabel ) ;

	// Ŭ����
	m_pClassLabel = new CRYLLabel( this ) ;		
	m_pClassLabel->SetLeft( 60 ) ;
	m_pClassLabel->SetTop( 94 ) ;
	m_pClassLabel->SetFontColor (0xffffffff);
	m_pClassLabel->SetAutoSize( TRUE ) ;
	m_pClassLabel->SetCaption( CRYLStringTable::m_strString[ 1141 ] ) ;
	AttachGMUIControl( m_pClassLabel ) ;

	// �̸�
	m_pNameLabel = new CRYLLabel( this ) ;		
	m_pNameLabel->SetLeft( 153 ) ;
	m_pNameLabel->SetTop( 94 ) ;
	m_pNameLabel->SetFontColor (0xffffffff);
	m_pNameLabel->SetAutoSize( TRUE ) ;
	m_pNameLabel->SetCaption( CRYLStringTable::m_strString[ 1144 ] ) ;
	AttachGMUIControl( m_pNameLabel ) ;

	// ����Ʈ
	m_pPointLabel = new CRYLLabel( this ) ;		
	m_pPointLabel->SetLeft( 234 ) ;
	m_pPointLabel->SetTop( 94 ) ;
	m_pPointLabel->SetFontColor (0xffffffff);
	m_pPointLabel->SetAutoSize( TRUE ) ;
	m_pPointLabel->SetCaption( CRYLStringTable::m_strString[ 2239 ] ) ;
	AttachGMUIControl( m_pPointLabel ) ;

	// ����
	m_pLebelLabel = new CRYLLabel( this ) ;		
	m_pLebelLabel->SetLeft( 287 ) ;
	m_pLebelLabel->SetTop( 94 ) ;
	m_pLebelLabel->SetFontColor (0xffffffff);
	m_pLebelLabel->SetAutoSize( TRUE ) ;
	m_pLebelLabel->SetCaption( CRYLStringTable::m_strString[ 192 ] ) ;
	AttachGMUIControl( m_pLebelLabel ) ;

	// ���� ���
	m_pSortLabel = new CRYLLabel( this ) ;		
	m_pSortLabel->SetLeft( 9 ) ;
	m_pSortLabel->SetTop( 47 ) ;
	m_pSortLabel->SetFontColor (0xffffffff);
	m_pSortLabel->SetAutoSize( TRUE ) ;
	m_pSortLabel->SetCaption( CRYLStringTable::m_strString[ 2011 ] ) ;
	AttachGMUIControl( m_pSortLabel ) ;
	
	// ���� ������
	m_pPrevButton = new CRYLButton( this ) ;
	m_pPrevButton->SetLeft( 287 ) ;
	m_pPrevButton->SetTop( 387 ) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLRANKINGDLG::PREVRANK_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pPrevButton ) ;

	m_pNextButton = new CRYLButton( this ) ;
	m_pNextButton->SetLeft( 302 ) ;
	m_pNextButton->SetTop( 387 ) ;
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLRANKINGDLG::NEXTRANK_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pNextButton ) ;
	m_pRankingStrGrid = new CRYLStringGrid( this, FALSE ) ;
	m_pRankingStrGrid->SetLeft( 13 ) ;
	m_pRankingStrGrid->SetTop( 113 ) ;
	m_pRankingStrGrid->SetWidth( 292 ) ;
	m_pRankingStrGrid->SetHeight( 264 ) ;
	m_pRankingStrGrid->SetColCount( 18 ) ;
	m_pRankingStrGrid->SetFontColor( 0xffffffff ) ;
//	m_pRankingStrGrid->SetInstance( RYLGUILDPANEL::GUILD_STRINGGRID ) ;
	m_pRankingStrGrid->OnClickEvent = DlgClick ;
	AttachGMUIControl( m_pRankingStrGrid ) ;
}

HRESULT		CRYLRankingDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	static CHAR strText[ MAX_PATH ] ;

	switch( CRYLNetworkData::m_dwServerID )
	{
		case 0:		strcpy( strText, CRYLStringTable::m_strString[ 2228 ] ) ;	break ;
		case 1:		strcpy( strText, CRYLStringTable::m_strString[ 2229 ] ) ;	break ;
		case 2:		strcpy( strText, CRYLStringTable::m_strString[ 2230 ] ) ;	break ;
		case 3:		strcpy( strText, CRYLStringTable::m_strString[ 2231 ] ) ;	break ;
		default:	strcpy( strText, CRYLStringTable::m_strString[ 2232 ] ) ;	break ;
	}
	m_pServerNameValueLabel->SetCaption( strText ) ;
	
	if ( CRYLGameData::Instance()->m_csStatus.m_dwMyTotalRanking )
	{
		sprintf( strText, "%d", CRYLGameData::Instance()->m_csStatus.m_dwMyTotalRanking ) ;
	} 
	else
	{
		strcpy( strText, CRYLStringTable::m_strString[ 2236 ] ) ;
	}
	
	m_pCurrentRankingValueLabel->SetCaption( strText ) ;


	return S_OK ;
}

VOID	CRYLRankingDlg::FinalCtrl() 
{
	GM_DELETE( m_pRankingDlgLabel ) ;
	GM_DELETE( m_pServerNameLabel ) ; 			// �����̸�
	GM_DELETE( m_pServerNameValueLabel ) ;		// �����̸� ��
	GM_DELETE( m_pCurrentRankingLabel ) ;		// ���� ��ŷ
	GM_DELETE( m_pCurrentRankingValueLabel ) ;	// ���� ��ŷ��

	GM_DELETE( m_pRankingLabel ) ;				// ����
	GM_DELETE( m_pClassLabel ) ;				// Ŭ����
	GM_DELETE( m_pNameLabel	) ;					// �̸�
	GM_DELETE( m_pPointLabel ) ;				// ����Ʈ
	GM_DELETE( m_pLebelLabel ) ;				// ����

	GM_DELETE( m_pPrevButton ) ;				// ���� ������
	GM_DELETE( m_pNextButton ) ;				// ���� ������
	GM_DELETE( m_pRankingStrGrid ) ;			// ��ŷ ����Ʈ
	GM_DELETE( m_pClassCBBox ) ;				// Ŭ���� �޺��ڽ�

	GM_DELETE( m_pSortLabel ) ;					// ��Ʈ ���
}

VOID	CRYLRankingDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLRankingDlg::Hide( CGMUIObject* Sender ) 
{
	
}

VOID	CRYLRankingDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLRankingDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_RankingDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLRANKINGDLG::PREVRANK_BUTTON :
			SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				static_cast<unsigned char>(g_RankingDlg->m_iClass), g_RankingDlg->GetCurrentPage() - 1 ) ;
			break ;

		case RYLRANKINGDLG::NEXTRANK_BUTTON : 		
			SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				static_cast<unsigned char>(g_RankingDlg->m_iClass), g_RankingDlg->GetCurrentPage() + 1 ) ;
			break ;

		case RYLRANKINGDLG::RANKING_COMBO :
//			g_RankingDlg->InsertDataListbox( g_RankingDlg->m_pClassCBBox->GetMenu()->GetItemIndex() ) ;//was commented out hz show class rank
			break ;

		case RYLRANKINGDLG::RANKING_MENU :
		{
			switch( CRYLGameData::Instance()->m_dwClientRace )
			{
				case CClass::RaceType::HUMAN : 
				{
					switch ( g_RankingDlg->m_pClassCBBox->GetMenu()->GetItemIndex() )
					{
						case 0 :/*��ü����*/	g_RankingDlg->m_iClass = 0 ; 	break ;
						case 1 :/*�����*/		g_RankingDlg->m_iClass = 5 ; 	break ;
						case 2 :/*������*/		g_RankingDlg->m_iClass = 6 ; 	break ;
						//case 3 :/*����*/		g_RankingDlg->m_iClass = 7 ; 	break ;
						case 3 :/*��ó*/		g_RankingDlg->m_iClass = 8 ; 	break ;
						case 4 :/*�Ҽ���*/		g_RankingDlg->m_iClass = 9 ; 	break ;
						//case 6 :/*��æ��*/		g_RankingDlg->m_iClass = 10 ; 	break ;
						//case 7 :/*������Ʈ*/	g_RankingDlg->m_iClass = 11 ; 	break ;
						case 5 :/*Ŭ����*/		g_RankingDlg->m_iClass = 12 ; 	break ;
						case 6 :/*���÷�*/		g_RankingDlg->m_iClass = 19 ; 	break ;
						case 7 :/*����Ŀ*/		g_RankingDlg->m_iClass = 20 ; 	break ;
						//case 3 :/*�ų�*/		g_RankingDlg->m_iClass = 21 ; 	break ;
						case 8 :/*�����*/		g_RankingDlg->m_iClass = 22 ; 	break ;
						case 9 :/*����������*/	g_RankingDlg->m_iClass = 23 ; 	break ;
						case 110 :/*���������*/	g_RankingDlg->m_iClass = 24 ; 	break ;
					}
					break ;
				}

				case CClass::RaceType::AKHAN : 
				{
					switch ( g_RankingDlg->m_pClassCBBox->GetMenu()->GetItemIndex() )
					{
						case 0 :/*��ü����*/	g_RankingDlg->m_iClass = 0 ; 	break ;
						case 1 :/*�����*/		g_RankingDlg->m_iClass = 5 ; 	break ;
						case 2 :/*������*/		g_RankingDlg->m_iClass = 6 ; 	break ;
						//case 3 :/*����*/		g_RankingDlg->m_iClass = 7 ; 	break ;
						case 3 :/*��ó*/		g_RankingDlg->m_iClass = 8 ; 	break ;
						case 4 :/*�Ҽ���*/		g_RankingDlg->m_iClass = 9 ; 	break ;
						//case 6 :/*��æ��*/		g_RankingDlg->m_iClass = 10 ; 	break ;
						//case 7 :/*������Ʈ*/	g_RankingDlg->m_iClass = 11 ; 	break ;
						case 5 :/*Ŭ����*/		g_RankingDlg->m_iClass = 12 ; 	break ;
						case 6 :/*���÷�*/		g_RankingDlg->m_iClass = 19 ; 	break ;
						case 7 :/*����Ŀ*/		g_RankingDlg->m_iClass = 20 ; 	break ;
						//case 3 :/*�ų�*/		g_RankingDlg->m_iClass = 21 ; 	break ;
						case 8 :/*�����*/		g_RankingDlg->m_iClass = 22 ; 	break ;
						case 9 :/*����������*/	g_RankingDlg->m_iClass = 23 ; 	break ;
						case 10 :/*���������*/	g_RankingDlg->m_iClass = 24 ; 	break ;
					}
					break ;
				}
			}

			SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, g_RankingDlg->m_iClass, 0 ) ;
			break ;
		}

		case RYLRANKINGDLG::RANKING_LISTBOX : 		
			break ;
	}
}

VOID	CRYLRankingDlg::InsertDataListbox( INT iClass ) 
{
	m_pRankingStrGrid->ClearItem() ;

	INT iRank = m_cCurrentPage * PktRankingInfo::MAX_NUM_PER_PAGE + 1 ;

	// ����
	// Ŭ����
	// �̸�
	// ����Ʈ
	// ����
	switch( iClass )
	{
		case 0:		// ��ü
			{
				//std::vector<RankingNode>			m_vecAll ;			// ��ü ����Ʈ		
				for ( std::vector<RankingNode>::iterator it = m_vecAll.begin() ; it != m_vecAll.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;			
		case 1:		// ������
			break ;
		case 2:		// �α�
			break ;
		case 3:		// ������
			break ;
		case 4:		// ������
			break ;
		case 5:		// �����
			{
				//std::vector<RankingNode>			m_vecDefender ;		// ����� ����Ʈ		
				for ( std::vector<RankingNode>::iterator it = m_vecDefender.begin() ; it != m_vecDefender.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 6:		// ������
			{
				//std::vector<RankingNode>			m_vecWarrior ;		// ������ ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecWarrior.begin() ; it != m_vecWarrior.end(); ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 7:		// ��ؽ�
			{
				//std::vector<RankingNode>			m_vecAssassin ;		// ���� ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecAssassin.begin() ; it != m_vecAssassin.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 8:		// ��ó
			{
				//std::vector<RankingNode>			m_vecArcher ;		// ��ó ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecArcher.begin() ; it != m_vecArcher.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			
			break ;
		case 9:		// �Ҽ���
			{
				//std::vector<RankingNode>			m_vecSorcerer ;		// �Ҽ��� ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecSorcerer.begin() ; it != m_vecSorcerer.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 10:		// ��æ��
			{
				//std::vector<RankingNode>			m_vecEnchanter ;	// ��þ�� ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecEnchanter.begin() ; it != m_vecEnchanter.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 11:		// ������Ʈ
			{
				//std::vector<RankingNode>			m_vecPriest ;		// ������Ʈ ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecPriest.begin() ; it != m_vecPriest.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 12:		// Ŭ����
			{
				//std::vector<RankingNode>			m_vecCleric ;		// Ŭ���� ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecCleric.begin() ; it != m_vecCleric.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 17:		// �Ĺ���Ʈ
			break ;
		case 18:		// ���Ǽ�����
			break ;
		case 19:		// ���÷�
			{
				//std::vector<RankingNode>			m_vecTemplar ;		// ���÷� ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecTemplar.begin() ; it != m_vecTemplar.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 20:		// ����Ŀ
			{
				//std::vector<RankingNode>			m_vecAttacker ;		// ����Ŀ ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecAttacker.begin() ; it != m_vecAttacker.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 21:		// �ų�
			{
				//std::vector<RankingNode>			m_vecGunner ;		// �ų�	����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecGunner.begin() ; it != m_vecGunner.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 22:		// �����
			{
				//std::vector<RankingNode>			m_vecRuneOff ;		// ����� ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecRuneOff.begin() ; it != m_vecRuneOff.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 23:		// ����������
			{
				//std::vector<RankingNode>			m_vecLifeOff ;		// ���������� ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecLifeOff.begin() ; it != m_vecLifeOff.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}
			}
			break ;
		case 24:		// ���������
			{
				//std::vector<RankingNode>			m_vecShadowOff ;	// ��������� ����Ʈ
				for ( std::vector<RankingNode>::iterator it = m_vecShadowOff.begin() ; it != m_vecShadowOff.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// ����
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// Ŭ����
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// �̸�
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// ����Ʈ
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// ����
					++ iRank ;
				}

			}
			break ;
	}
}

VOID	CRYLRankingDlg::InsertRanking( INT iClass, INT iPage, INT iNum, RankingNode* lpRankingNode ) 
{
	RankingNode AddNode;
	m_cCurrentPage = iPage ;

	switch( iClass )
	{
		case 0:		// ��ü
			{
				//std::vector<RankingNode>			m_vecAll ;			// ��ü ����Ʈ
				m_vecAll.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecAll.push_back( AddNode ) ;
				}
			}
			break ;
		case 1:		// ������
			break ;
		case 2:		// �α�
			break ;
		case 3:		// ������
			break ;
		case 4:		// ������
			break ;
		case 5:		// �����
			{
				//std::vector<RankingNode>			m_vecDefender ;		// ����� ����Ʈ
				m_vecDefender.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecDefender.push_back( AddNode ) ;
				}
			}
			break ;
		case 6:		// ������
			{
				//std::vector<RankingNode>			m_vecWarrior ;		// ������ ����Ʈ
				m_vecWarrior.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecWarrior.push_back( AddNode ) ;
				}
			}
			break ;
		case 7:		// ��ؽ�
			{
				//std::vector<RankingNode>			m_vecAssassin ;		// ���� ����Ʈ
				m_vecAssassin.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecAssassin.push_back( AddNode ) ;
				}
			}
			break ;
		case 8:		// ��ó
			{
				//std::vector<RankingNode>			m_vecArcher ;		// ��ó ����Ʈ
				m_vecArcher.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecArcher.push_back( AddNode ) ;
				}
			}
			
			break ;
		case 9:		// �Ҽ���
			{
				//std::vector<RankingNode>			m_vecSorcerer ;		// �Ҽ��� ����Ʈ
				m_vecSorcerer.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecSorcerer.push_back( AddNode ) ;
				}
			}
			break ;
		case 10:		// ��æ��
			{
				//std::vector<RankingNode>			m_vecEnchanter ;	// ��þ�� ����Ʈ
				m_vecEnchanter.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecEnchanter.push_back( AddNode ) ;
				}
			}
			break ;
		case 11:		// ������Ʈ
			{
				//std::vector<RankingNode>			m_vecPriest ;		// ������Ʈ ����Ʈ
				m_vecPriest.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecPriest.push_back( AddNode ) ;
				}
			}
			break ;
		case 12:		// Ŭ����
			{
				//std::vector<RankingNode>			m_vecCleric ;		// Ŭ���� ����Ʈ
				m_vecCleric.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecCleric.push_back( AddNode ) ;
				}
			}
			break ;
		case 17:		// �Ĺ���Ʈ
			break ;
		case 18:		// ���Ǽ�����
			break ;
		case 19:		// ���÷�
			{
				//std::vector<RankingNode>			m_vecTemplar ;		// ���÷� ����Ʈ
				m_vecTemplar.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecTemplar.push_back( AddNode ) ;
				}
			}
			break ;
		case 20:		// ����Ŀ
			{
				//std::vector<RankingNode>			m_vecAttacker ;		// ����Ŀ ����Ʈ
				m_vecAttacker.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecAttacker.push_back( AddNode ) ;
				}
			}
			break ;
		case 21:		// �ų�
			{
				//std::vector<RankingNode>			m_vecGunner ;		// �ų�	����Ʈ
				m_vecGunner.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecGunner.push_back( AddNode ) ;
				}
			}
			break ;
		case 22:		// �����
			{
				//std::vector<RankingNode>			m_vecRuneOff ;		// ����� ����Ʈ
				m_vecRuneOff.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecRuneOff.push_back( AddNode ) ;
				}
			}
			break ;
		case 23:		// ����������
			{
				//std::vector<RankingNode>			m_vecLifeOff ;		// ���������� ����Ʈ
				m_vecLifeOff.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecLifeOff.push_back( AddNode ) ;
				}
			}
			break ;
		case 24:		// ���������
			{
				//std::vector<RankingNode>			m_vecShadowOff ;	// ��������� ����Ʈ
				m_vecShadowOff.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecShadowOff.push_back( AddNode ) ;
				}
			}
			break ;
	}
}