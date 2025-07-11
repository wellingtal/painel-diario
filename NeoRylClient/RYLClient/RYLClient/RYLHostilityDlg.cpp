#include <winsock2.h>
#include "RYLHostilityDlg.h"
#include "RYLButton.h" 
#include "RYLListBox.h"
#include "RYLStringGrid.h"
#include "RYLLabel.h"
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

#include "RYLMessageBox.h"

#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <RenderOption.h>
#include "GMMemory.h"

CRYLHostilityDlg*	g_pHostilityDlg = NULL ;

CRYLHostilityDlg::CRYLHostilityDlg() : CRYLDialog()
{
	m_pTitleLabel						= NULL ;	// 적대 현황

	m_pDecHostilityGuildLabel			= NULL ;	// 적대 선언 중인 길드
	m_pDecHostilityGuildValueLabel		= NULL ;	// 적대 선언 중인 길드 값
	m_pDecAlertHostilityGuildLabel		= NULL ;	// 경계 적대 선언 길드
	m_pDecAlertHostilityGuildValueLabel	= NULL ;	// 경계 적대 선언 길드 값

	m_pHostilityGuildLabel				= NULL ;	// 적대 선언 받은 길드
	m_pGuildNameLabel					= NULL ;	// 길드명
	m_pDeclareKindLabel					= NULL ;	// 선언종류
	m_pStringGrid						= NULL ;	// 적대 선언 받은 길드 그리드

	m_pPrevButton						= NULL ;	// 이전 버튼
	m_pNextButton						= NULL ;	// 다음 버튼
	m_pPageLabel						= NULL ;	// 페이지 값
	
	m_dwGID								= 0 ;

	m_iCurrentPage						= 1 ;		// 현재 페이지
	m_iTotalItemNum						= 0 ;		// 리스트에 들어가는 총 아이템 갯수
	m_iPageState						= PktGuildHostilityList::PAGE_CURRENT ;	// 현재 페이지 요청

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 200, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 0, 150, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 206, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 150, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 200, 305, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 150, 35 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 206, 15, 256, 35 ) ;
	pImageDIB->DrawRect( &rt, 150, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	int i = 0;
	for (  ; i < 54 ; ++ i )
	{
		SetRect( &rt, 0, 38, 150, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 20 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		SetRect( &rt, 206, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 150, 20 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}

	SetRect( &rt, 0, 38, 150, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 20 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 206, 38, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 150, 20 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->DrawFrameRect( 2, 0, 196, 20, 0xc0000000 ) ;							// 타이틀 영역 색상
	pImageDIB->MakeFrameRect( 2, 19, 196, 1, 0xffB99E6B, 0xc0000000,  1 ) ;			// 한줄

	pImageDIB->MakeFrameRect(  17,  49, 123,  19, 0xff959595, 0xc0000000,  1 ) ;	// 적대 신청 중인 길드명 표시 영역
	pImageDIB->MakeFrameRect(  17, 103, 123,  19, 0xff959595, 0xc0000000,  1 ) ;	// 경계 적대 신청 중인 길드명 표시 영역
	pImageDIB->MakeFrameRect(  17, 157, 166, 106, 0xffB99E6B, 0xc0000000,  1 ) ;	// 적대 선언 받은 길드 금색 사각형
	pImageDIB->MakeFrameRect(  22, 176, 156,   1, 0xff959595, 0xc0000000,  1 ) ;	// 적대 선언 받은 길드 메뉴 분리 한줄 라인
	pImageDIB->MakeFrameRect( 108, 271,  57,  19, 0xff959595, 0xc0000000,  1 ) ;	// 페이지 표시 영역

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 200 ) ;
	SetHeight( 320 ) ;
	SetLeft( 328 ) ;
	SetTop( 0 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;

	g_pHostilityDlg = this ;
}

CRYLHostilityDlg::~CRYLHostilityDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLHostilityDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;

	// 적대 현황
	m_pTitleLabel = new CRYLLabel( this ) ;
	m_pTitleLabel->SetLeft( 10 ) ;
	m_pTitleLabel->SetTop( 17 ) ;
	m_pTitleLabel->SetFontColor( 0xffffffff ) ;
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[3986] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;


	// 적대 선언 중인 길드
	m_pDecHostilityGuildLabel = new CRYLLabel( this ) ;
	m_pDecHostilityGuildLabel->SetLeft( 17 ) ;
	m_pDecHostilityGuildLabel->SetTop( 46 ) ;
	m_pDecHostilityGuildLabel->SetFontColor( 0xffffffff ) ;
	m_pDecHostilityGuildLabel->SetAutoSize( TRUE ) ;
	m_pDecHostilityGuildLabel->SetCaption( CRYLStringTable::m_strString[3987] ) ;
	AttachGMUIControl( m_pDecHostilityGuildLabel ) ;

	// 적대 선언 중인 길드 값
	m_pDecHostilityGuildValueLabel = new CRYLLabel( this ) ;
	m_pDecHostilityGuildValueLabel->SetLeft( 0 ) ;
	m_pDecHostilityGuildValueLabel->SetTop( 0 ) ;
	m_pDecHostilityGuildValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 17, 68, 139, 79 ) ;
	m_pDecHostilityGuildValueLabel->SetAlignRect( rt ) ;
	m_pDecHostilityGuildValueLabel->SetAlign( CENTER ) ;
	m_pDecHostilityGuildValueLabel->SetCaption( "" ) ;
	m_pDecHostilityGuildValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pDecHostilityGuildValueLabel ) ;

	// 경계 적대 선언 길드
	m_pDecAlertHostilityGuildLabel = new CRYLLabel( this ) ;
	m_pDecAlertHostilityGuildLabel->SetLeft( 17 ) ;
	m_pDecAlertHostilityGuildLabel->SetTop( 100 ) ;
	m_pDecAlertHostilityGuildLabel->SetFontColor( 0xffffffff ) ;
	m_pDecAlertHostilityGuildLabel->SetAutoSize( TRUE ) ;
	m_pDecAlertHostilityGuildLabel->SetCaption( CRYLStringTable::m_strString[3988] ) ;
	AttachGMUIControl( m_pDecAlertHostilityGuildLabel ) ;

    // 경계 적대 선언 길드 값
	m_pDecAlertHostilityGuildValueLabel = new CRYLLabel( this ) ;
	m_pDecAlertHostilityGuildValueLabel->SetLeft( 0 ) ;
	m_pDecAlertHostilityGuildValueLabel->SetTop( 0 ) ;
	m_pDecAlertHostilityGuildValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 17, 122, 139, 133 ) ;
	m_pDecAlertHostilityGuildValueLabel->SetAlignRect( rt ) ;
	m_pDecAlertHostilityGuildValueLabel->SetAlign( CENTER ) ;
	m_pDecAlertHostilityGuildValueLabel->SetCaption( "" ) ;
	m_pDecAlertHostilityGuildValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pDecAlertHostilityGuildValueLabel ) ;


	// 적대 선언 받은 길드
	m_pHostilityGuildLabel = new CRYLLabel( this ) ;
	m_pHostilityGuildLabel->SetLeft( 17 ) ;
	m_pHostilityGuildLabel->SetTop( 154 ) ;
	m_pHostilityGuildLabel->SetFontColor( 0xffffffff ) ;
	m_pHostilityGuildLabel->SetAutoSize( TRUE ) ;
	m_pHostilityGuildLabel->SetCaption( CRYLStringTable::m_strString[3989] ) ;
	AttachGMUIControl( m_pHostilityGuildLabel ) ;

	// 길드명
	m_pGuildNameLabel = new CRYLLabel( this ) ;
	m_pGuildNameLabel->SetLeft( 39 ) ;
	m_pGuildNameLabel->SetTop( 177 ) ;
	m_pGuildNameLabel->SetFontColor( 0xffffffff ) ;
	m_pGuildNameLabel->SetAutoSize( TRUE ) ;
	m_pGuildNameLabel->SetCaption( CRYLStringTable::m_strString[1074] ) ;
	AttachGMUIControl( m_pGuildNameLabel ) ;

	// 선언종류
	m_pDeclareKindLabel = new CRYLLabel( this ) ;
	m_pDeclareKindLabel->SetLeft( 114 ) ;
	m_pDeclareKindLabel->SetTop( 177 ) ;
	m_pDeclareKindLabel->SetFontColor( 0xffffffff ) ;
	m_pDeclareKindLabel->SetAutoSize( TRUE ) ;
	m_pDeclareKindLabel->SetCaption( CRYLStringTable::m_strString[3990] ) ;
	AttachGMUIControl( m_pDeclareKindLabel ) ;

	// 적대 선언 받은 길드 그리드
	m_pStringGrid = new CRYLStringGrid( this, FALSE ) ;
	m_pStringGrid->SetLeft( 22 ) ;
	m_pStringGrid->SetTop( 198 ) ;
	m_pStringGrid->SetWidth( 156 ) ;
	m_pStringGrid->SetHeight( 76 ) ;
	m_pStringGrid->SetColCount( 10 ) ;
	m_pStringGrid->SetFontColor( 0xffffffff ) ;
	m_pStringGrid->SetInstance( RYLGUILDHOSTILITYDLG::STRING_GRID ) ;
	m_pStringGrid->OnClickEvent = DlgClick ;
	m_pStringGrid->OnMouseUpEvent = DlgMouseUp ;
	AttachGMUIControl( m_pStringGrid ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	// 이전 버튼
	m_pPrevButton = new CRYLButton( this ) ;
	m_pPrevButton->SetLeft( 90 ) ;
	m_pPrevButton->SetTop( 289 ) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLGUILDHOSTILITYDLG::PREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pPrevButton ) ;

	// 다음 버튼
	m_pNextButton = new CRYLButton( this ) ;
	m_pNextButton->SetLeft( 169 ) ;
	m_pNextButton->SetTop( 289 ) ;
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLGUILDHOSTILITYDLG::NEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pNextButton ) ;

	// 페이지 값
	m_pPageLabel = new CRYLLabel( this ) ;
	m_pPageLabel->SetLeft( 0 ) ;
	m_pPageLabel->SetTop( 0 ) ;
	m_pPageLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 108, 290, 164, 301 ) ;
	m_pPageLabel->SetAlignRect( rt ) ;
	m_pPageLabel->SetAlign( CENTER ) ;
	m_pPageLabel->SetCaption( "" ) ;
	m_pPageLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pPageLabel ) ;
}

VOID	CRYLHostilityDlg::FinalCtrl ()
{
	GM_DELETE( m_pTitleLabel ) ;							// 적대 현황

	GM_DELETE( m_pDecHostilityGuildLabel ) ;				// 적대 선언 중인 길드
	GM_DELETE( m_pDecHostilityGuildValueLabel ) ;			// 적대 선언 중인 길드 값
	GM_DELETE( m_pDecAlertHostilityGuildLabel ) ;			// 경계 적대 선언 길드
	GM_DELETE( m_pDecAlertHostilityGuildValueLabel ) ;		// 경계 적대 선언 길드 값

	GM_DELETE( m_pHostilityGuildLabel ) ;					// 적대 선언 받은 길드
	GM_DELETE( m_pGuildNameLabel ) ;						// 길드명
	GM_DELETE( m_pDeclareKindLabel ) ;						// 선언종류
	GM_DELETE( m_pStringGrid ) ;							// 적대 선언 받은 길드 그리드

	GM_DELETE( m_pPrevButton ) ;							// 이전 버튼
	GM_DELETE( m_pNextButton ) ;							// 다음 버튼
	GM_DELETE( m_pPageLabel	) ;								// 페이지 값
}

VOID	CRYLHostilityDlg::Show( CGMUIObject* Sender )
{
}

VOID	CRYLHostilityDlg::Hide( CGMUIObject* Sender )
{
	m_iCurrentPage = 1 ;
	m_iPageState = PktGuildHostilityList::PAGE_CURRENT ;
	m_iTotalItemNum = 0 ;
}

VOID	CRYLHostilityDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLHostilityDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLHostilityDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pHostilityDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLGUILDHOSTILITYDLG::PREV_BUTTON :
			g_pHostilityDlg->HostilityListPrev() ;
			break ;

		case RYLGUILDHOSTILITYDLG::NEXT_BUTTON :
			g_pHostilityDlg->HostilityListNext() ;
			break ;
	}
}

VOID	CRYLHostilityDlg::DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( !g_pHostilityDlg )
		return ;
}

VOID	CRYLHostilityDlg::HostilityListPrev()
{
	m_iPageState = PktGuildHostilityList::PAGE_PREV ;
	RequestHostiliyList() ;
}

VOID	CRYLHostilityDlg::HostilityListNext()
{
	m_iPageState = PktGuildHostilityList::PAGE_NEXT ;
	RequestHostiliyList() ;
}

VOID	CRYLHostilityDlg::RequestHostiliyList()
{
	SendPacket::CharGuildHostilityList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_iCurrentPage, m_iPageState ) ;
}

VOID	CRYLHostilityDlg::SetHostilityList( int iCurrentPage, int iTotalItemNum, char* szHostility, char* szAlert,
											unsigned char cNodeNum, GuildHostilityInfoNode* lpHostilityInfoNode )
{
	m_iCurrentPage = iCurrentPage ;
	m_iTotalItemNum = iTotalItemNum ;
	int iTotalPage = m_iTotalItemNum / PktGuildHostilityList::MAX_NUM_PER_PAGE ;
	if ( 0 != ( m_iTotalItemNum % PktGuildHostilityList::MAX_NUM_PER_PAGE ) )
	{
		++iTotalPage ;
	}
	if(iTotalPage <= 0)
		iTotalPage = 1;

	CHAR szBuffer[ 32 ] = "" ;
	sprintf( szBuffer, "%2d/%2d", m_iCurrentPage, iTotalPage ) ;
	m_pPageLabel->SetCaption( szBuffer ) ;

	m_pDecHostilityGuildValueLabel->SetCaption( szHostility ) ;
	m_pDecAlertHostilityGuildValueLabel->SetCaption( szAlert ) ;

	m_pStringGrid->ClearItem() ;
	for ( int i=0; i<cNodeNum; ++i, ++lpHostilityInfoNode )
	{
		switch ( lpHostilityInfoNode->m_cRelationType )
		{
			case Guild::HOSTILITY :				strcpy( szBuffer, CRYLStringTable::m_strString[3991] ) ;		break ;
			case Guild::ALERT_HOSTILITY :		strcpy( szBuffer, CRYLStringTable::m_strString[3992] ) ;		break ;
			case Guild::COUNTER_HOSTILITY :		strcpy( szBuffer, CRYLStringTable::m_strString[3993] ) ;		break ;
			case Guild::TARGET_NEUTRALITY :		strcpy( szBuffer, CRYLStringTable::m_strString[3991] ) ;		break ;
			default :							strcpy( szBuffer, "" ) ;										break ;
		}

		if(lpHostilityInfoNode->m_cRelationType == Guild::TARGET_NEUTRALITY)
		{
			m_pStringGrid->AddItem( 0, lpHostilityInfoNode->m_szName, 67, D3DCOLOR_RGBA( 138, 255, 0, 255 ) ) ;
			m_pStringGrid->AddItem( 1, "*", 18, D3DCOLOR_RGBA( 138, 255, 0, 255 ) ) ;
			m_pStringGrid->AddItem( 2, szBuffer, 59, D3DCOLOR_RGBA( 138, 255, 0, 255 ) ) ;
			m_pStringGrid->AddItem( 3, "", 10, D3DCOLOR_RGBA( 138, 255, 0, 255 ) ) ;
		}
		else 
		{
			m_pStringGrid->AddItem( 0, lpHostilityInfoNode->m_szName, 67, D3DCOLOR_RGBA( 255, 255, 255, 255 ) ) ;
			m_pStringGrid->AddItem( 1, "", 18, D3DCOLOR_RGBA( 255, 255, 255, 255 ) ) ;
			m_pStringGrid->AddItem( 2, szBuffer, 59, D3DCOLOR_RGBA( 255, 255, 255, 255 ) ) ;
			m_pStringGrid->AddItem( 3, "", 10, D3DCOLOR_RGBA( 255, 255, 255, 255 ) ) ;
		}
	}
}
