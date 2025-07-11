#include <winsock2.h>
#include "RYLExchangeDlg.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLCheckBox.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLPicture.h"

#include "RYLGameData.h"
#include "RYLRaceBase.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "WinInput.h"
#include "RYLChattingDlg.h"

#include "..\\GUITooltipItem.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"
#include "BaseGraphicsLayer.h"

#include "RYLSprite.h"

#include <Network/ClientSocket/Send/SendEtc.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <RenderOption.h>
#include "GMMemory.h"
CRYLExchangeDlg*	g_pExchangeDlg = NULL ;

CRYLExchangeDlg::CRYLExchangeDlg() : CRYLDialog()
{
	m_dwSplitResult			= 0 ;
	m_dwSplitValue			= 0 ;
	m_lpSplitItem			= NULL ;
	m_bSingleWindow			= FALSE ;
	m_dwExchangeGoldResult	= 0 ;

	m_dwExchangeGoldValue = 0;
	RECT rt ;
	
	m_pImageDIB				= NULL ;
	m_pImage				= NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 512, 15, &m_pImageDIB ) ;
	m_pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	m_pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
	m_pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
	
	m_pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pImage->LoadFromImage( m_pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)m_pImage ) ;

	GM_DELETE( m_pImageDIB ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 512, 241, &m_pImageDIB ) ;
	m_pImageDIB->ClearbyColor( 0x00000000 ) ;
	
	SetRect( &rt, 0, 15, 256, 256) ;
	m_pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
	m_pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
	
	m_pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pImage->LoadFromImage( m_pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage ) ;
	AttachWindowTexture( ( CGMTexture*)m_pImage ) ;

	GM_DELETE( m_pImageDIB ) ;
	
	SetWidth( 512 ) ;
	SetHeight( 256 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	g_pExchangeDlg = this ;
	InitCtrl() ;

	m_ptStartPos.x = GetLeft() ;
	m_ptStartPos.y = GetTop() ;
}

CRYLExchangeDlg::~CRYLExchangeDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLExchangeDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 교환창1
	m_pExchangeLabel1 = new CRYLLabel( this ) ;
	m_pExchangeLabel1->SetLeft( 14 ) ;
	m_pExchangeLabel1->SetTop( 22 ) ;
	m_pExchangeLabel1->SetFontColor (0xffffffff);
	m_pExchangeLabel1->SetAutoSize( TRUE ) ;
	m_pExchangeLabel1->SetCaption( CRYLStringTable::m_strString[ 2163 ] ) ;
	AttachGMUIControl( m_pExchangeLabel1 ) ;

	// 교환창2
	m_pExchangeLabel2 = new CRYLLabel( this ) ;
	m_pExchangeLabel2->SetLeft( 270 ) ;
	m_pExchangeLabel2->SetTop( 22 ) ;
	m_pExchangeLabel2->SetFontColor (0xffffffff);
	m_pExchangeLabel2->SetAutoSize( TRUE ) ;
	m_pExchangeLabel2->SetCaption( CRYLStringTable::m_strString[ 2163 ] ) ;
	AttachGMUIControl( m_pExchangeLabel2 ) ;

	
	// 교환창 값
	m_pExchangeOwnerLabel1 = new CRYLLabel( this ) ;
	m_pExchangeOwnerLabel1->SetLeft( 81 ) ;
	m_pExchangeOwnerLabel1->SetTop( 52 ) ;
	m_pExchangeOwnerLabel1->SetFontColor (0xffffffff);
	m_pExchangeOwnerLabel1->SetAutoSize( TRUE ) ;
	m_pExchangeOwnerLabel1->SetCaption( "" ) ;
	AttachGMUIControl( m_pExchangeOwnerLabel1 ) ;

	// 교환창 값
	m_pExchangeOwnerLabel2 = new CRYLLabel( this ) ;
	m_pExchangeOwnerLabel2->SetLeft( 337 ) ;
	m_pExchangeOwnerLabel2->SetTop( 52 ) ;
	m_pExchangeOwnerLabel2->SetFontColor (0xffffffff);
	m_pExchangeOwnerLabel2->SetAutoSize( TRUE ) ;
	m_pExchangeOwnerLabel2->SetCaption( "" ) ;
	AttachGMUIControl( m_pExchangeOwnerLabel2 ) ;

	
	// 잠금 버튼
	m_pLockCheckBox1 = new CRYLCheckBox( this ) ;
	m_pLockCheckBox1->SetLeft( 223 ) ;
	m_pLockCheckBox1->SetTop( 38 ) ;
	m_pLockCheckBox1->SetInstance( RYLEXCHANGEDDLG::LOCK1_CHECKBOX ) ;
	m_pLockCheckBox1->OnClickEvent = DlgClick ;
	SetRect( &rt, 0, 121, 26, 148 ) ;
	m_pLockCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 121, 52, 148 ) ;
	m_pLockCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 52, 121, 78, 148 ) ;
	m_pLockCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 0, 121, 26, 148 ) ;
	m_pLockCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 26, 121, 52, 148 ) ;
	m_pLockCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 0, 121, 26, 148 ) ;
	m_pLockCheckBox1->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;

	m_pLockCheckBox1->SetChecked( FALSE ) ;
	AttachGMUIControl( m_pLockCheckBox1 ) ;

	// 잠금 버튼
	m_pLockCheckBox2 = new CRYLCheckBox( this ) ;
	m_pLockCheckBox2->SetLeft( 479 ) ;
	m_pLockCheckBox2->SetTop( 38 ) ;
	m_pLockCheckBox2->SetInstance( RYLEXCHANGEDDLG::LOCK2_CHECKBOX ) ;
	m_pLockCheckBox2->OnClickEvent = DlgClick ;
	SetRect( &rt, 0, 121, 26, 148 ) ;
	m_pLockCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 121, 52, 148 ) ;
	m_pLockCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 52, 121, 78, 148 ) ;
	m_pLockCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 0, 121, 26, 148 ) ;
	m_pLockCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 26, 121, 52, 148 ) ;
	m_pLockCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 0, 121, 26, 148 ) ;
	m_pLockCheckBox2->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pLockCheckBox2->SetChecked( FALSE ) ;
	AttachGMUIControl( m_pLockCheckBox2 ) ;
	
	// 돈
	m_pGoldLabel1 = new CRYLLabel( this ) ;
	m_pGoldLabel1->SetLeft( 53 ) ;
	m_pGoldLabel1->SetTop( 196 ) ;
	m_pGoldLabel1->SetFontColor (0xffffffff);
	m_pGoldLabel1->SetAutoSize( TRUE ) ;
	m_pGoldLabel1->SetCaption( "" ) ;
	AttachGMUIControl( m_pGoldLabel1 ) ;

	// 돈
	m_pGoldLabel2 = new CRYLLabel( this ) ;
	m_pGoldLabel2->SetLeft( 309 ) ;
	m_pGoldLabel2->SetTop( 196 ) ;
	m_pGoldLabel2->SetFontColor (0xffffffff);
	m_pGoldLabel2->SetAutoSize( TRUE ) ;
	m_pGoldLabel2->SetCaption( "" ) ;
	AttachGMUIControl( m_pGoldLabel2 ) ;
	
	// 확인
	m_pOkButton1 = new CRYLCheckBox( this ) ;
	m_pOkButton1->SetLeft( 116 ) ;
	m_pOkButton1->SetTop( 220 ) ;
	m_pOkButton1->SetColor( 0xffffffff ) ;
	m_pOkButton1->OnClickEvent = DlgClick ;
	m_pOkButton1->SetInstance( RYLEXCHANGEDDLG::OK1_BUTTON ) ;
	m_pOkButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pOkButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	m_pOkButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pOkButton1->SetCaption( CRYLStringTable::m_strString[ 2164 ], TRUE, TRUE, 0xffffffff ) ;
	AttachGMUIControl( m_pOkButton1 ) ;
	
	// 확인
	m_pOkButton2 = new CRYLCheckBox( this ) ;
	m_pOkButton2->SetLeft( 372 ) ;
	m_pOkButton2->SetTop( 220 ) ;
	m_pOkButton2->SetFontColor( 0xffffffff ) ;
	m_pOkButton2->SetColor( 0xffffffff ) ;
	m_pOkButton2->OnClickEvent = DlgClick ;
	m_pOkButton2->SetInstance( RYLEXCHANGEDDLG::OK2_BUTTON ) ;
	m_pOkButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pOkButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	m_pOkButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pOkButton2->SetCaption( CRYLStringTable::m_strString[ 2164 ], TRUE, TRUE, 0xffffffff ) ;
	AttachGMUIControl( m_pOkButton2 ) ;

	// 취소
	m_pCancelButton1 = new CRYLButton( this ) ;
	m_pCancelButton1->SetLeft( 178 ) ;
	m_pCancelButton1->SetTop( 220  ) ;
	m_pCancelButton1->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton1->SetFontColor( 0xffffffff ) ;
	m_pCancelButton1->SetColor( 0xffffffff ) ;
	m_pCancelButton1->OnClickEvent = DlgClick ;
	m_pCancelButton1->SetInstance( RYLEXCHANGEDDLG::CANCEL1_BUTTON ) ;
	m_pCancelButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCancelButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	m_pCancelButton1->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton1 ) ;

	// 취소
	m_pCancelButton2 = new CRYLButton( this ) ;
	m_pCancelButton2->SetLeft( 434 ) ;
	m_pCancelButton2->SetTop( 220  ) ;
	m_pCancelButton2->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton2->SetFontColor( 0xffffffff ) ;
	m_pCancelButton2->SetColor( 0xffffffff ) ;
	m_pCancelButton2->OnClickEvent = DlgClick ;
	m_pCancelButton2->SetInstance( RYLEXCHANGEDDLG::CANCEL2_BUTTON ) ;
	m_pCancelButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCancelButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	m_pCancelButton2->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton2 ) ;

	// 돈 버튼
	m_pMoneyButton = new CRYLButton( this ) ;
	m_pMoneyButton->SetLeft( 443 ) ;
	m_pMoneyButton->SetTop( 188  ) ;
	m_pMoneyButton->SetCaption( CRYLStringTable::m_strString[ 2165 ] ) ;
	m_pMoneyButton->SetFontColor( 0xffffffff ) ;
	m_pMoneyButton->SetColor( 0xffffffff ) ;
	m_pMoneyButton->OnClickEvent = DlgClick ;
	m_pMoneyButton->SetInstance( RYLEXCHANGEDDLG::MONEY_BUTTON ) ;
	m_pMoneyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pMoneyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pMoneyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pMoneyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pMoneyButton ) ;
}


VOID	CRYLExchangeDlg::FinalCtrl ()
{
	GM_DELETE( m_pExchangeLabel1 ) ;				// 교환창
	GM_DELETE( m_pExchangeLabel2 ) ;				// 교환창
	GM_DELETE( m_pExchangeOwnerLabel1 ) ;			// 상대방 이름 값
	GM_DELETE( m_pExchangeOwnerLabel2 ) ;			// 자신 이름 값
	GM_DELETE( m_pGoldLabel1 ) ;					// 상대방 돈
	GM_DELETE( m_pGoldLabel2 ) ;					// 자신 돈

	GM_DELETE( m_pOkButton1 ) ;						// 상대방 확인
	GM_DELETE( m_pCancelButton1 ) ;					// 상대방 취소
	GM_DELETE( m_pOkButton2 ) ;						// 자신 확인
	GM_DELETE( m_pCancelButton2 ) ;					// 자신 취소
	GM_DELETE( m_pMoneyButton ) ;					// 돈 버튼
	GM_DELETE( m_pLockCheckBox1 ) ;					// 상대방 잠금 버튼
	GM_DELETE( m_pLockCheckBox2 ) ;					// 자신 잠금버튼

	m_pImageDIB = NULL ;
	m_pImage = NULL ;
}

VOID	CRYLExchangeDlg::Show( CGMUIObject* Sender )
{
	CRYLDialog::GMUIPaint( 0, 0 ) ;
	GetCloseButton()->SetVisible( FALSE ) ;

	m_bSingleWindow = FALSE;
    SetSingleWindow( FALSE ) ;

	m_pOkButton2->SetEnable( TRUE ) ;			// 자신 확인
	m_pCancelButton2->SetEnable( TRUE ) ;		// 자신 취소
	m_pLockCheckBox2->SetChecked( FALSE )  ;	// 자신 잠금 버튼

	m_pOkButton1->SetDisableEmptyCheck( FALSE ) ;		// 상대방 확인
	m_pCancelButton1->SetDisableEmptyCheck( FALSE ) ;	// 상대방 취소
	m_pLockCheckBox1->SetDisableEmptyCheck( FALSE )  ;	// 상대편  잠금 버튼

	m_ptStartPos.x = GetLeft() ;
	m_ptStartPos.y = GetTop() ;
}
	
VOID	CRYLExchangeDlg::Hide( CGMUIObject* Sender )
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->m_bRYLExchangeDlgClick = FALSE;

	if ( CRYLGameData::Instance()->m_csStatus.m_lstExchange.size() || 
		 CRYLGameData::Instance()->m_csStatus.m_dwExchangeGold || 
		 g_pClientSocket->GetStatusFlag() != NS_NOWAITING || 
		 CRYLGameData::Instance()->m_lpPickItem )
	{
		return;
	}

	m_bSingleWindow = FALSE;
	SetSingleWindow( FALSE ) ;

	m_ptStartPos.x = GetLeft() ;
	m_ptStartPos.y = GetTop() ;
}

VOID	CRYLExchangeDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if (!m_bSingleWindow)
	{
        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pGame->m_lpInterface->m_dwExchangeCmdSenderID );
        if( pTempCreature )
        {
            CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
            if( pTempData )
            {
			    // 상대방 이름
			    m_pExchangeOwnerLabel1->SetCaption( const_cast<char*>( pTempData->m_strName.c_str() ) );
            }
		}

		// 상대방 돈
		m_pGoldLabel1->SetCaption( CRYLStringTable::MakePrintGold( pGame->m_csStatus.m_dwExchange2Gold ) ) ;
	}

	// 자신 이름
	m_pExchangeOwnerLabel2->SetCaption( pGame->m_csStatus.m_Info.Name ) ;

	// 자신 돈
	m_pGoldLabel2->SetCaption( CRYLStringTable::MakePrintGold(pGame->m_csStatus.m_dwExchangeGold) ) ;

	__int64 ddItem;
	list<CItemInstance *>::iterator it;
	const Item::ItemInfo *lpItemInfo;
	for(it = pGame->m_csStatus.m_lstExchange.begin(); it != pGame->m_csStatus.m_lstExchange.end(); it++)
	{
		lpItemInfo = &(*it)->GetItemInfo();
		(*it)->m_wPutX = GetLeft() + 280 + 26 * (*it)->m_lpItemBase->GetPos().GetXIndex() + 
			(((lpItemInfo->m_DetailData.m_cXSize * 26 - 1) - (*it)->m_lpSprite->GetSizeX()) / 2);
		(*it)->m_wPutY = GetTop() + 77 + 26 * (*it)->m_lpItemBase->GetPos().GetYIndex() + 
			(((lpItemInfo->m_DetailData.m_cYSize * 26 - 1) - (*it)->m_lpSprite->GetSizeY()) / 2);
		(*it)->Render(lpD3DDevice);
	}

	if ( !m_bSingleWindow)
	{
		for(it = pGame->m_csStatus.m_lstExchange2.begin(); it != pGame->m_csStatus.m_lstExchange2.end(); it++)
		{
			(*it)->Render(lpD3DDevice);
		}
	}

	if (pGame->m_lpPickItem && g_MessageBoxManager.m_lstMessageBox.empty())
	{
		if (g_DeviceInput.InRect(GetLeft() + 280, GetTop() + 77, GetLeft() + 486, GetTop() + 179))		// 인벤
		{
			Item::ItemPos pItemIndex;
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			pItemIndex.SetPos((ptMousePos->x - (GetLeft() + 280)) / 26, (ptMousePos->y - (GetTop() + 77)) / 26, 0);

			if ((ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_EXCHANGE, pItemIndex.GetXIndex(), pItemIndex.GetYIndex())) != ITEM_FAILED)
			{
				pGame->RenderRect(lpD3DDevice,
					GetLeft() + 280 + pItemIndex.GetXIndex() * 26,
					GetTop() + 77 + pItemIndex.GetYIndex() * 26,
					GetLeft() + 280 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
					GetTop() + 77 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
					0xFF0000FF, 0x80);
			} 
			else
			{
				if (pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize + pItemIndex.GetXIndex() < 8 &&
					pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize + pItemIndex.GetYIndex() < 4)
				{
					pGame->RenderRect(lpD3DDevice,
						GetLeft() + 280 + pItemIndex.GetXIndex() * 26,
						GetTop() + 77 + pItemIndex.GetYIndex() * 26,
						GetLeft() + 280 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
						GetTop() + 77 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
						0xFFFF0000, 0x80);
				}
			}
		}
	}
}

HRESULT		CRYLExchangeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (!GetVisible()) 
		return E_FAIL ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->m_bRYLExchangeDlgClick = FALSE ;
	
	if ( pGame->m_bRYLHumanInventoryFrameDlgClick || g_MessageBoxManager.GetIsExistModal() )
		return S_OK ;

	// 교환창은 기본 창닫기 버튼을 이용하지 못한다. (무조건 취소 버튼을 이용하도록!)
	if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
			pGame->m_bRYLExchangeDlgClick = TRUE ;
		}
	}

	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();

	// 손에 물건을 들고 있을때 취소와 돈 놓기 버튼을 누르지 못한다.
	if ( pGame->m_lpPickItem && !m_bSingleWindow)
	{
		m_pCancelButton2->SetEnable( FALSE ) ;	
		m_pMoneyButton->SetEnable( FALSE ) ;	
		m_pLockCheckBox2->SetEnable( FALSE ) ;
		m_pOkButton2->SetEnable( FALSE ) ;
	}
	else
	{
		m_pCancelButton2->SetEnable( TRUE ) ;	
		m_pMoneyButton->SetEnable( TRUE ) ;		
		m_pLockCheckBox2->SetEnable( TRUE ) ;		

		// 확인 버튼은 양쪽 모두 락된 상태에서만 활성화된다.
		if ( m_pLockCheckBox1->GetDisableEmptyCheck() && m_pLockCheckBox2->GetChecked() )
		{
			m_pOkButton2->SetEnable( TRUE ) ;
		}
		else
		{
			m_pOkButton2->SetEnable( FALSE ) ;
		}
	}

	// 확인이 눌러지면 락이 활동을 못한다.
	if ( m_pOkButton2->GetChecked() )
	{
		m_pLockCheckBox2->SetDisableEmptyCheck( TRUE );
	}
	else
	{
		m_pLockCheckBox2->SetEnable( TRUE );
	}

//	m_TooltipManager.Update();

	// 나누기
	if ( m_dwSplitResult & MB_YES )
	{
		if ( !m_pOkButton1->GetDisableEmptyCheck() ||		// 상대방 확인
			 !m_pOkButton2->GetEnable() ||					// 자신 확인
			  m_pLockCheckBox1->GetDisableEmptyCheck() ||	// 상대방 잠금 버튼
			  m_pLockCheckBox2->GetChecked() )				// 자신 버튼
		{
			CRYLNetworkData::Instance()->CRYLNetworkData::Instance()->SendExchangeCmd( 
				pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Cancel );
		}

		if ( m_dwSplitValue )
		{
			if (m_dwSplitValue >= m_lpSplitItem->GetNowDurability())
			{
				pGame->SetPickItem(m_lpSplitItem, FALSE);
				pGame->m_csStatus.DeleteExchangeItem(m_lpSplitItem);
			} 
			else
			{
				pGame->m_lpPickItem = new CItemInstance;
				pGame->m_lpPickItem->m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem(m_lpSplitItem->GetProtoTypeID());
				pGame->m_lpPickItem->SetPos(m_lpSplitItem->m_lpItemBase->GetPos().m_cIndex);
				pGame->m_lpPickItem->SetNowDurability(m_dwSplitValue);

				pGame->m_lpPickItem->SetItemPos(TakeType::TS_EXCHANGE);
				pGame->m_lpPickItem->m_dwItemUID = m_lpSplitItem->m_dwItemUID;
				pGame->m_lpPickItem->m_lpSprite = new CRYLSprite;
				pGame->m_lpPickItem->m_lpSprite->Create(0, 0, pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinX,
					pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinY,
					pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxX,
					pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);
				pGame->m_lpPickItem->m_lpTooltip = new CGUITooltipItem;
				((CGUITooltipItem *)pGame->m_lpPickItem->m_lpTooltip)->SetItem(pGame->m_lpPickItem);

				unsigned long dwDurability = m_lpSplitItem->GetNowDurability() - m_dwSplitValue;
				m_lpSplitItem->SetNowDurability(dwDurability);
			}
		}

		m_dwSplitResult = 0;
		m_dwSplitValue = 0;
		m_lpSplitItem = NULL;

		return TRUE;
	} 
	else if (m_dwSplitResult & MB_NO || m_dwSplitResult & MB_EXIT)
	{
		m_dwSplitResult = 0;
		m_dwSplitValue = 0;
		m_lpSplitItem = NULL;

		return TRUE;
	}

	// 돈 올리기
	if (m_dwExchangeGoldResult & MB_YES)
	{
		if ( !m_pOkButton1->GetDisableEmptyCheck() ||		// 상대방 확인
			 !m_pOkButton2->GetEnable() ||					// 자신 확인
			  m_pLockCheckBox1->GetDisableEmptyCheck() ||	// 상대방 잠금 버튼
			  m_pLockCheckBox2->GetChecked() )				// 자신 버튼
		{
			CRYLNetworkData::Instance()->CRYLNetworkData::Instance()->SendExchangeCmd(
				pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_UnLock );
			CRYLNetworkData::Instance()->CRYLNetworkData::Instance()->SendExchangeCmd(
				pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Cancel );
		}

		if (pGame->m_csStatus.m_Info.Gold <= m_dwExchangeGoldValue)
		{
			m_dwExchangeGoldValue = pGame->m_csStatus.m_Info.Gold;
		}

		__int64 ddGold = (__int64)m_dwExchangeGoldValue - (__int64)pGame->m_csStatus.m_dwExchangeGold;
		if (ddGold > 0)
		{
			g_pClientSocket->SetStatusFlag(NS_TAKEGOLD);
			SendPacket::CharTakeGold(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				TakeType::TS_INVEN, TakeType::TS_EXCHANGE, (unsigned long)ddGold);
		} 
		else if (ddGold < 0)
		{
			g_pClientSocket->SetStatusFlag(NS_TAKEGOLD);
			SendPacket::CharTakeGold(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				TakeType::TS_EXCHANGE, TakeType::TS_INVEN, (unsigned long)-ddGold);
		}

		m_dwExchangeGoldResult = 0;
		m_dwExchangeGoldValue = 0;

		return TRUE;
	} 
	else if (m_dwExchangeGoldResult & MB_NO || m_dwExchangeGoldResult & MB_EXIT)
	{
		m_dwExchangeGoldResult = 0;
		m_dwExchangeGoldValue = 0;

		return TRUE;
	}

	// 교환창 아이템 선택
	if ( !m_bSingleWindow /*&& !m_pLockCheckBox2->GetChecked() */)
	{
		if (SelectItem())
		{
			if ( !m_pOkButton1->GetDisableEmptyCheck() ||		// 상대방 확인
				 !m_pOkButton2->GetEnable() ||					// 자신 확인
				  m_pLockCheckBox1->GetDisableEmptyCheck() ||	// 상대방 잠금 버튼
				  m_pLockCheckBox2->GetChecked() )				// 자신 버튼
			{
				CRYLNetworkData::Instance()->SendExchangeCmd(
					pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_UnLock );
				CRYLNetworkData::Instance()->SendExchangeCmd(
					pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Cancel );
			}

			return TRUE;
		}
	}

	if ( m_bSingleWindow )
	{
		SelectItem() ;
	}

	// 돈 버튼 누르기
	
	unsigned long dwInvenX = (ptMousePos->x - (GetLeft() + 280)) / 26;
	unsigned long dwInvenY = (ptMousePos->y - (GetTop() + 77)) / 26;

	__int64 ddItem;

	if ( !m_pLockCheckBox2->GetChecked() && !m_bSingleWindow )
	{
		if (g_DeviceInput.GetIsLeftMouseDown() && pGame->m_lpPickItem && pGame->m_lpPickItem->GetItemPos() != TakeType::TS_QSLOT)
		{
			if (g_DeviceInput.InRect(GetLeft() + 280, GetTop() + 77, GetLeft() + 468, GetTop() + 179))		// 교환창
			{
				if ((ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_EXCHANGE, dwInvenX, dwInvenY)) != ITEM_FAILED)
				{
					if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bExchangeAndDrop)
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(CRYLStringTable::m_strString[ 136 ]);
						return TRUE;
					}

					Item::ItemPos pItemIndex;
					pItemIndex.SetPos(dwInvenX, dwInvenY, 0);
					pItemIndex.m_cPos = TakeType::TS_EXCHANGE;

					if (pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
					{
						CItemInstance *lpExistItem = pGame->m_csStatus.GetExchangeItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());

						if (lpExistItem)
						{
							if (pGame->m_lpPickItem->GetIsEnableStack())
							{
								unsigned dwDurability = lpExistItem->GetNowDurability() + pGame->m_lpPickItem->GetNowDurability();
								lpExistItem->SetNowDurability(dwDurability);
								delete pGame->m_lpPickItem;
								pGame->SetPickItem(NULL);
							}
						} 
						else
						{
							pGame->m_csStatus.AddExchangeItem(pGame->m_lpPickItem);
							pGame->SetPickItem(NULL);
						}

						return TRUE;
					}

					if (ddItem == ITEM_EMPTY)
					{
						if (pGame->m_lpPickItem->GetIsEnableStack())
						{
							CItemInstance *lpExistItem = NULL;
							switch(pGame->m_lpPickItem->GetItemPos())
							{
								case TakeType::TS_INVEN:
									lpExistItem = pGame->m_csStatus.GetInventoryItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
									break;

								case TakeType::TS_EXCHANGE:
									lpExistItem = pGame->m_csStatus.GetExchangeItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
									break;
							}

							if (lpExistItem && lpExistItem->GetProtoTypeID() == pGame->m_lpPickItem->GetProtoTypeID())
							{
								// 기존의 자리에 아이템이 남아있으면 아이템이 나눠진걸로 생각, 아이템을 스택한다.
								if ( !m_pOkButton1->GetDisableEmptyCheck() ||		// 상대방 확인
									 !m_pOkButton2->GetEnable() ||					// 자신 확인
								  	  m_pLockCheckBox1->GetDisableEmptyCheck() ||	// 상대방 잠금 버튼
									  m_pLockCheckBox2->GetChecked() )				// 자신 버튼
								{
									CRYLNetworkData::Instance()->SendExchangeCmd(
										pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_UnLock );
									CRYLNetworkData::Instance()->SendExchangeCmd(
										pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Cancel );
								}
								CRYLNetworkData::Instance()->SendSplitItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
							}
							else
							{
								if ( !m_pOkButton1->GetDisableEmptyCheck() ||		// 상대방 확인
									 !m_pOkButton2->GetEnable() ||					// 자신 확인
								  	  m_pLockCheckBox1->GetDisableEmptyCheck() ||	// 상대방 잠금 버튼
									  m_pLockCheckBox2->GetChecked() )				// 자신 버튼
								{
									CRYLNetworkData::Instance()->SendExchangeCmd(
										pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_UnLock );
									CRYLNetworkData::Instance()->SendExchangeCmd(
										pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Cancel );
								}
								CRYLNetworkData::Instance()->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
							}
						} 
						else
						{
							// 아이템 이동
							if ( !m_pOkButton1->GetDisableEmptyCheck() ||		// 상대방 확인
								 !m_pOkButton2->GetEnable() ||					// 자신 확인
								  m_pLockCheckBox1->GetDisableEmptyCheck() ||	// 상대방 잠금 버튼
								  m_pLockCheckBox2->GetChecked() )				// 자신 버튼
							{
								CRYLNetworkData::Instance()->SendExchangeCmd(
									pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_UnLock );
								CRYLNetworkData::Instance()->SendExchangeCmd(
									pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Cancel );
							}
							CRYLNetworkData::Instance()->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, 1);
						}
					} 
					else
					{
						CItemInstance *lpExistItem = pGame->m_csStatus.GetExchangeItem(ddItem);

						if (pGame->m_lpPickItem->GetIsEnableStack() && lpExistItem &&
							(dwInvenX == 
							lpExistItem->m_lpItemBase->GetPos().GetXIndex() &&
							dwInvenY == lpExistItem->m_lpItemBase->GetPos().GetYIndex()) &&
							lpExistItem->GetProtoTypeID() == pGame->m_lpPickItem->GetProtoTypeID() &&
							lpExistItem->GetNowDurability() < lpExistItem->GetMaxDurability())
						{
							// 아이템을 스택한다.
							unsigned char cNum;
							if (lpExistItem->GetMaxDurability() - lpExistItem->GetNowDurability() < pGame->m_lpPickItem->GetNowDurability())
							{
								cNum = lpExistItem->GetMaxDurability() - lpExistItem->GetNowDurability();
							} 
							else
							{
								cNum = pGame->m_lpPickItem->GetNowDurability();
							}

							if ( !m_pOkButton1->GetDisableEmptyCheck() ||		// 상대방 확인
								 !m_pOkButton2->GetEnable() ||					// 자신 확인
								  m_pLockCheckBox1->GetDisableEmptyCheck() ||	// 상대방 잠금 버튼
								  m_pLockCheckBox2->GetChecked() )				// 자신 버튼
							{
								CRYLNetworkData::Instance()->SendExchangeCmd(
									pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_UnLock );
								CRYLNetworkData::Instance()->SendExchangeCmd(
									pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Cancel );
							}

							CRYLNetworkData::Instance()->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, cNum);
							return TRUE;
						}

						// 인벤토리 리스트에서 아이템을 찾는다.(완료)
						// 스왑하기
						CItemInstance *lpInvenItem = NULL;

						if (pGame->m_lpPickItem->GetIsEnableStack() && pGame->m_lpPickItem->GetItemPos() != TakeType::TS_TEMP)
							lpInvenItem = pGame->m_csStatus.GetExchangeItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());

						if (!lpInvenItem)
						{
							if ( !m_pOkButton1->GetDisableEmptyCheck() ||		// 상대방 확인
 								 !m_pOkButton2->GetEnable() ||					// 자신 확인
								  m_pLockCheckBox1->GetDisableEmptyCheck() ||	// 상대방 잠금 버튼
								  m_pLockCheckBox2->GetChecked() )				// 자신 버튼
							{
								CRYLNetworkData::Instance()->SendExchangeCmd(
									pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_UnLock );
								CRYLNetworkData::Instance()->SendExchangeCmd(
									pGame->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Cancel );
							}

							CRYLNetworkData::Instance()->SendSwapItem(pGame->m_lpPickItem, &pItemIndex, lpExistItem);
						}
					}

					return TRUE;
				}
			}
		}
	}

	return S_OK ;	
}


VOID	CRYLExchangeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pExchangeDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLEXCHANGEDDLG::LOCK1_CHECKBOX :
			break ;

		case RYLEXCHANGEDDLG::LOCK2_CHECKBOX :
			g_pExchangeDlg->ExchangeLockEvent() ;
			break ;

		case RYLEXCHANGEDDLG::OK1_BUTTON :
			break ;

		case RYLEXCHANGEDDLG::OK2_BUTTON :
			g_pExchangeDlg->ExchangeOkEvent() ;
			break ;

		case RYLEXCHANGEDDLG::CANCEL1_BUTTON :
			break ;

		case RYLEXCHANGEDDLG::CANCEL2_BUTTON :
			g_pExchangeDlg->ExchangeCancelEvent() ;
			break ;

		case RYLEXCHANGEDDLG::MONEY_BUTTON :
			g_pExchangeDlg->ExchangeMoneyEvent() ;
			break ;
	}
}

VOID	CRYLExchangeDlg::ExchangeOkEvent() 
{
	if ( !m_bSingleWindow && !CRYLGameData::Instance()->m_lpPickItem &&
		  m_pLockCheckBox1->GetDisableEmptyCheck() &&
 		  m_pLockCheckBox2->GetChecked() )
	{
		unsigned short wCmd;

		if ( m_pOkButton2->GetChecked() )
		{
			if ( !m_pOkButton1->GetDisableEmptyCheck() )
			{
				if ( FALSE == CheckInvenMoney() )
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[476]);
					return;
				}

				if ( CRYLGameData::Instance()->m_csStatus.m_lstExchange2.size() > 0 )
				{
					if ( FALSE == CheckInvenSpace() )
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(CRYLStringTable::m_strString[315]);
						return;
					}
				}

				wCmd = ClientConstants::ExchangeCmd_OK;
			}
			else
			{
				if ( FALSE == CheckInvenMoney() )
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[476]);
					return;
				}

				if ( CRYLGameData::Instance()->m_csStatus.m_lstExchange2.size() > 0 )
				{
					if ( FALSE == CheckInvenSpace() )
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(CRYLStringTable::m_strString[315]);
						return;
					}
				}

				wCmd = ClientConstants::ExchangeCmd_Exchange;
			}
		}
		else
		{
			wCmd = ClientConstants::ExchangeCmd_Cancel;
		}

		CRYLNetworkData::Instance()->SendExchangeCmd( CRYLGameData::Instance()->m_lpInterface->m_dwExchangeCmdSenderID, wCmd);
	}
}

VOID	CRYLExchangeDlg::ExchangeCancelEvent() 
{
	if ( !m_bSingleWindow )
	{
		CRYLNetworkData::Instance()->SendExchangeCmd( 
			CRYLGameData::Instance()->m_lpInterface->m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Quit );
	}
	else
	{
		ExchangeQuit();
	}
}

VOID	CRYLExchangeDlg::ExchangeMoneyEvent()
{
	if(CRYLGameData::Instance()->m_bGuildSafeResult)
	{
		m_dwExchangeGoldResult = 0;
		m_dwExchangeGoldValue = 0;
		return;
	}

	if ( !m_pLockCheckBox2->GetChecked() &&
		CRYLGameData::Instance()->m_dwKindCursor == CURSOR_NORMAL && !CRYLGameData::Instance()->m_lpPickItem )
	{
		CRYLMessageBox *lpMessageBox;
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[128], MB_NUMBER | MB_YES | MB_NO | MB_EXIT);
		lpMessageBox->SetResult(&m_dwExchangeGoldResult);
		lpMessageBox->SetValue(&m_dwExchangeGoldValue);
		
		m_dwExchangeGoldResult = 0;
		m_dwExchangeGoldValue = 0;
	}
}

VOID	CRYLExchangeDlg::ExchangeLockEvent() 
{
	if ( !m_bSingleWindow && !CRYLGameData::Instance()->m_lpPickItem )
	{
		unsigned short wCmd;
		if ( m_pLockCheckBox2->GetChecked() )
		{
			wCmd = ClientConstants::ExchangeCmd_Lock;
			CRYLNetworkData::Instance()->CRYLNetworkData::Instance()->SendExchangeCmd(
				CRYLGameData::Instance()->m_lpInterface->m_dwExchangeCmdSenderID, wCmd );
		} else
		{
			wCmd = ClientConstants::ExchangeCmd_UnLock;
			CRYLNetworkData::Instance()->CRYLNetworkData::Instance()->SendExchangeCmd(
				CRYLGameData::Instance()->m_lpInterface->m_dwExchangeCmdSenderID, wCmd );
		}
	}
}

BOOL	CRYLExchangeDlg::SelectItem()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	BOOL bPickItem = FALSE;
	const Item::ItemInfo *lpItemInfo;

	list<CItemInstance *>::iterator it;
	for(it = pGame->m_csStatus.m_lstExchange2.begin(); it != pGame->m_csStatus.m_lstExchange2.end(); it++)
	{
		lpItemInfo = &(*it)->GetItemInfo();

		(*it)->m_wPutX = GetLeft() + 24 + 26 * (*it)->m_lpItemBase->GetPos().GetXIndex() + 
			(((lpItemInfo->m_DetailData.m_cXSize * 26 - 1) - (*it)->m_lpSprite->GetSizeX()) / 2);
		(*it)->m_wPutY = GetTop() + 77 + 26 * (*it)->m_lpItemBase->GetPos().GetYIndex() + 
			(((lpItemInfo->m_DetailData.m_cYSize * 26 - 1) - (*it)->m_lpSprite->GetSizeY()) / 2);
	}

///// 인벤 아이템 업데이트 및 선택
	for(it = pGame->m_csStatus.m_lstExchange.begin(); it != pGame->m_csStatus.m_lstExchange.end();)
	{
		lpItemInfo = &(*it)->GetItemInfo();

		(*it)->m_wPutX = GetLeft() + 280 + 26 * (*it)->m_lpItemBase->GetPos().GetXIndex() + 
			(((lpItemInfo->m_DetailData.m_cXSize * 26 - 1) - (*it)->m_lpSprite->GetSizeX()) / 2);
		(*it)->m_wPutY = GetTop() + 77 + 26 * (*it)->m_lpItemBase->GetPos().GetYIndex() + 
			(((lpItemInfo->m_DetailData.m_cYSize * 26 - 1) - (*it)->m_lpSprite->GetSizeY()) / 2);

		if ( !m_pLockCheckBox2->GetChecked() && 
			 pGame->m_dwKindCursor == CURSOR_NORMAL && 
			 !pGame->m_lpPickItem && 
			 g_DeviceInput.GetIsLeftMouseDown() && 
			 (*it)->IsOverMouse() &&
             !ClientSocket::IsNSFlagChainChecked(CmdCharUseItem))   // 아이템을 사용중이면 집을 수 없다.
		{
			pGame->SetPickItem((*it));
			pGame->m_csStatus.CheckExchangeField(CCharStatus::REMOVE_FIELD, pGame->m_lpPickItem);
			it = pGame->m_csStatus.m_lstExchange.erase(it);

			bPickItem = TRUE;
		} 
		else
		{
			it++;
		}
	}

	if (bPickItem)
	{
		return TRUE;
	}

	return FALSE;
}

VOID	CRYLExchangeDlg::ExchangeAccept( ULONG dwChrID)
{
	if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
	{
		m_pOkButton2->SetChecked( TRUE ) ;
	}
	else
	{
		m_pOkButton1->SetDisableEmptyCheck( TRUE ) ;
	}
}

VOID	CRYLExchangeDlg::ExchangeCancel( ULONG dwChrID)
{
	if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
	{
		m_pOkButton2->SetChecked( FALSE ) ;
	}
	else
	{
		m_pOkButton1->SetDisableEmptyCheck( FALSE ) ;
	}
}

VOID	CRYLExchangeDlg::ExchangeQuit()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	m_bSingleWindow = TRUE;

	list<CItemInstance *>::iterator it;
	CItemInstance *lpItem;
	for(it = pGame->m_csStatus.m_lstExchange2.begin(); it != pGame->m_csStatus.m_lstExchange2.end();)
	{
		lpItem = (*it);
		pGame->m_csStatus.CheckExchangeField(CCharStatus::REMOVE_FIELD, lpItem, FALSE);
		it = pGame->m_csStatus.m_lstExchange2.erase(it);
		delete lpItem;
	}
	pGame->m_csStatus.m_lstExchange2.clear();
	
	pGame->m_csStatus.m_Info.Gold += pGame->m_csStatus.m_dwExchangeGold;
	pGame->m_csStatus.m_dwExchangeGold = 0;
	pGame->m_csStatus.m_dwExchange2Gold = 0;

	if (pGame->m_csStatus.m_Info.Gold >= 4000000000)
	{
		char strString[200];
		sprintf( strString, CRYLStringTable::m_strString[ 216 ], CRYLStringTable::MakePrintGold(ULONG_MAX) ) ;
		CRYLChattingDlg::Instance()->AddMessage( strString, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
	}

	SetSingleWindow( TRUE ) ;
}

VOID	CRYLExchangeDlg::SetSingleWindow( BOOL bSingle )
{
	m_pImageDIB		= NULL ;
	m_pImage		= NULL ;
	RECT			rt ;

	// 교환 후(취소의 경우라도) 자신의 창만 떠 있는 상태
	if ( bSingle )
	{	
		CRYLSpriteList::Instance()->CreateGMImage( 512, 15, &m_pImageDIB ) ;
		m_pImageDIB->ClearbyColor( 0x00000000 ) ;
		SetRect( &rt, 0, 0, 256, 15 ) ;
		m_pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
		m_pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		m_pImage->LoadFromImage( m_pImageDIB ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage ) ;
		AttachCaptionTexture( ( CGMTexture*)m_pImage ) ;
		GM_DELETE( m_pImageDIB ) ;

		CRYLSpriteList::Instance()->CreateGMImage( 512, 241, &m_pImageDIB ) ;
		m_pImageDIB->ClearbyColor( 0x00000000 ) ;
		SetRect( &rt, 0, 15, 256, 256) ;
		m_pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
		m_pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		m_pImage->LoadFromImage( m_pImageDIB ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage ) ;
		AttachWindowTexture( ( CGMTexture*)m_pImage ) ;
		GM_DELETE( m_pImageDIB ) ;

		m_pExchangeLabel1->SetVisible( FALSE ) ;			// 교환창
		m_pExchangeOwnerLabel1->SetVisible( FALSE ) ;		// 상대방 이름 값
		m_pGoldLabel1->SetVisible( FALSE ) ;				// 상대방 돈
		m_pOkButton1->SetVisible( FALSE ) ;					// 상대방 확인
		m_pCancelButton1->SetVisible( FALSE ) ;				// 상대방 취소
		m_pMoneyButton->SetVisible( FALSE ) ;				// 돈 버튼
		m_pLockCheckBox1->SetVisible( FALSE ) ;				// 상대방 잠금 버튼

		// 자신의 창의 버튼들은 더 이상 동작하지 않는다.
		m_pOkButton2->SetChecked( FALSE ) ;					
		m_pLockCheckBox2->SetChecked( FALSE ) ;				
		m_pOkButton2->SetEnable( FALSE ) ;					
		m_pLockCheckBox2->SetEnable( FALSE ) ;				

		// 자신의 창이 비어있다면 자동으로 닫아준다.
		if ( CRYLGameData::Instance()->m_csStatus.m_lstExchange.empty() && 
			 CRYLGameData::Instance()->m_csStatus.m_lstExchange2.empty() )
		{
			SetVisible( FALSE ) ;
		}
	}
	else
	{
		CRYLSpriteList::Instance()->CreateGMImage( 512, 15, &m_pImageDIB ) ;
		m_pImageDIB->ClearbyColor( 0x00000000 ) ;
		SetRect( &rt, 0, 0, 256, 15 ) ;
		m_pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
		m_pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
		m_pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		m_pImage->LoadFromImage( m_pImageDIB ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage ) ;
		AttachCaptionTexture( ( CGMTexture*)m_pImage ) ;
		GM_DELETE( m_pImageDIB ) ;
		
		CRYLSpriteList::Instance()->CreateGMImage( 512, 241, &m_pImageDIB ) ;
		m_pImageDIB->ClearbyColor( 0x00000000 ) ;
		SetRect( &rt, 0, 15, 256, 256) ;
		m_pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
		m_pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) ;
		m_pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		m_pImage->LoadFromImage( m_pImageDIB ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage ) ;
		AttachWindowTexture( ( CGMTexture*)m_pImage ) ;
		GM_DELETE( m_pImageDIB ) ;

		m_pExchangeLabel1->SetVisible( TRUE ) ;				// 교환창
		m_pExchangeOwnerLabel1->SetVisible( TRUE ) ;		// 상대방 이름 값
		m_pGoldLabel1->SetVisible( TRUE ) ;					// 상대방 돈
		m_pOkButton1->SetVisible( TRUE ) ;					// 상대방 확인
		m_pCancelButton1->SetVisible( TRUE ) ;				// 상대방 취소
		m_pMoneyButton->SetVisible( TRUE ) ;				// 돈 버튼
		m_pLockCheckBox1->SetVisible( TRUE ) ;				// 상대방 잠금 버튼
	}
}

VOID	CRYLExchangeDlg::ProcessExchange()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	m_bSingleWindow = TRUE;
	
	pGame->m_csStatus.m_Info.Gold += pGame->m_csStatus.m_dwExchange2Gold;
	if (pGame->m_csStatus.m_Info.Gold >= 4000000000)
	{
		char strString[200];
		sprintf( strString, CRYLStringTable::m_strString[ 216 ], CRYLStringTable::MakePrintGold(ULONG_MAX) ) ;
		CRYLChattingDlg::Instance()->AddMessage( strString, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
	}

	list<CItemInstance *>::iterator it;
	CItemInstance *lpItem;
	for(it = pGame->m_csStatus.m_lstExchange.begin(); it != pGame->m_csStatus.m_lstExchange.end();)
	{
		lpItem = (*it);
		pGame->m_csStatus.CheckExchangeField(CCharStatus::REMOVE_FIELD, lpItem);
		it = pGame->m_csStatus.m_lstExchange.erase(it);
		delete lpItem;
	}

	pGame->m_csStatus.m_lstExchange.clear();
	pGame->m_csStatus.m_dwExchangeGold = 0;

	for(it = pGame->m_csStatus.m_lstExchange2.begin(); it != pGame->m_csStatus.m_lstExchange2.end();)
	{
		lpItem = (*it);
		pGame->m_csStatus.CheckExchangeField(CCharStatus::REMOVE_FIELD, lpItem, FALSE);
		pGame->m_csStatus.AddExchangeItem(lpItem);
		it = pGame->m_csStatus.m_lstExchange2.erase(it);
	}

	pGame->m_csStatus.m_lstExchange2.clear();
	pGame->m_csStatus.m_dwExchange2Gold = 0;

	SetSingleWindow( TRUE ) ;
}

VOID	CRYLExchangeDlg::InitPos( CItemInstance *lpItem )
{
	const Item::ItemInfo *lpItemInfo = &lpItem->GetItemInfo();

	lpItem->m_wPutX = GetLeft() + 24 + 26 * lpItem->m_lpItemBase->GetPos().GetXIndex() + 
		(((lpItemInfo->m_DetailData.m_cXSize * 26 - 1) - lpItem->m_lpSprite->GetSizeX()) / 2);
	lpItem->m_wPutY = GetTop() + 77 + 26 * lpItem->m_lpItemBase->GetPos().GetYIndex() + 
		(((lpItemInfo->m_DetailData.m_cYSize * 26 - 1) - lpItem->m_lpSprite->GetSizeY()) / 2);
}

VOID	CRYLExchangeDlg::ExchangeLock( ULONG dwChrID)
{
	if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
	{
		m_pLockCheckBox2->SetChecked( TRUE ) ;
	}
	else
	{
		m_pLockCheckBox1->SetDisableEmptyCheck( TRUE ) ;
	}
}

VOID	CRYLExchangeDlg::ExchangeUnLock( ULONG dwChrID)
{
	if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
	{
		m_pLockCheckBox2->SetChecked( FALSE ) ;
	}
	else
	{
		m_pLockCheckBox1->SetDisableEmptyCheck( FALSE ) ;
	}
}

BOOL	CRYLExchangeDlg::CheckInvenSpace()
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	pGame->m_csStatus.InitTestItemInfo() ;

	list<CItemInstance *>::iterator it;
	for (it = pGame->m_csStatus.m_lstExchange2.begin(); it != pGame->m_csStatus.m_lstExchange2.end(); it++)
	{
		if ( FALSE == pGame->m_csStatus.CheckAbleInven( (*it)->GetProtoTypeID(), (*it)->GetNowDurability() ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}


BOOL	CRYLExchangeDlg::CheckInvenMoney()
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	return pGame->m_csStatus.CheckInvenMoney(pGame->m_csStatus.m_dwExchange2Gold, pGame->m_csStatus.m_dwExchangeGold);
}