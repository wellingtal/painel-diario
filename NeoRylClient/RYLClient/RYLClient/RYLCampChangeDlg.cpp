
#include <winsock2.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "GMImageDib.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLCamp.h"
#include "RYLCampManager.h"

#include "RYLCampChangeDlg.h"
#include "GMMemory.h"

CRYLCampChangeDlg*	g_pRYLCampChangeDlg = NULL ;

CRYLCampChangeDlg::CRYLCampChangeDlg() 
{
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 180, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 35 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 13, 73, 55, 55, 0xff946b42, 0xc0000000,  1 ) ;	

	pImageDIB->DrawFrameRect( 14, 100, 53, 1, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 39, 74, 1, 53, 0xff675e46 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	
	SetWidth( 256 ) ;
	SetHeight( 192 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;
	
	g_pRYLCampChangeDlg = this ;

	m_dwCampID = 0L ;
	m_dwChangeCampType = 0L ;
	m_nMaterialNeed = 0L;
}

CRYLCampChangeDlg::~CRYLCampChangeDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLCampChangeDlg::InitCtrl() 
{
	CHAR strString[MAX_PATH];
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 병기 생성 메뉴
	m_pArmsCreatMenuLabel = new CRYLLabel( this ) ;
	m_pArmsCreatMenuLabel->SetLeft( 12 ) ;
	m_pArmsCreatMenuLabel->SetTop( 18 ) ;
	m_pArmsCreatMenuLabel->SetFontColor (0xffffffff);
	m_pArmsCreatMenuLabel->SetAutoSize( TRUE ) ;
	m_pArmsCreatMenuLabel->SetCaption( CRYLStringTable::m_strString[ 3180 ] ) ;
	AttachGMUIControl( m_pArmsCreatMenuLabel ) ;

	// 타입 변형 시간 설명
	sprintf( strString, CRYLStringTable::m_strString[ 3049 ], Siege::CAMP_CHANGING_TIME ) ;
	m_pArmsRemainTimeLabel = new CRYLLabel( this ) ;
	m_pArmsRemainTimeLabel->SetLeft( 12 ) ;
	m_pArmsRemainTimeLabel->SetTop( 64 ) ;
	m_pArmsRemainTimeLabel->SetFontColor (0xffffffff);
	m_pArmsRemainTimeLabel->SetAutoSize( TRUE ) ;
	m_pArmsRemainTimeLabel->SetCaption( strString ) ;
	AttachGMUIControl( m_pArmsRemainTimeLabel ) ;

	// 병기 생성 설명 1
	m_pArmsCreatExpla1Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla1Label->SetLeft( 78 ) ;
	m_pArmsCreatExpla1Label->SetTop( 88 ) ;
	m_pArmsCreatExpla1Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla1Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla1Label->SetCaption( CRYLStringTable::m_strString[ 3181 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla1Label ) ;

	int  nCnt = 30;
	sprintf( strString, CRYLStringTable::m_strString[ 3182 ], nCnt ) ;
	
	// 병기 생성 설명 2
	m_pArmsCreatExpla2Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla2Label->SetLeft( 78 ) ;
	m_pArmsCreatExpla2Label->SetTop( 104 ) ;
	m_pArmsCreatExpla2Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla2Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla2Label->SetCaption( strString ) ;
	AttachGMUIControl( m_pArmsCreatExpla2Label ) ;

/*
	// 병기 생성 설명 3
	m_pArmsCreatExpla3Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla3Label->SetLeft( 78 ) ;
	m_pArmsCreatExpla3Label->SetTop( 122 ) ;
	m_pArmsCreatExpla3Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla3Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla3Label->SetCaption( CRYLStringTable::m_strString[ 3050 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla3Label ) ;

	// 병기 생성 설명 4
	m_pArmsCreatExpla4Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla4Label->SetLeft( 78 ) ;
	m_pArmsCreatExpla4Label->SetTop( 140 ) ;
	m_pArmsCreatExpla4Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla4Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla4Label->SetCaption( CRYLStringTable::m_strString[ 3051 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla4Label ) ;
*/

	// 병기 생성 설명 5
	m_pArmsCreatExpla5Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla5Label->SetLeft( 12 ) ;
	m_pArmsCreatExpla5Label->SetTop( 48 ) ;
	m_pArmsCreatExpla5Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla5Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla5Label->SetCaption( CRYLStringTable::m_strString[ 2696 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla5Label ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	// 확인 버튼
	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 99 ) ;
	m_pOKButton->SetTop( 159 ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff) ;
	m_pOKButton->SetColor( 0xffffffff) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLCAMPCHANGEDLG::OK_BUTTON ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOKButton ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 173 ) ;
	m_pCancelButton->SetTop( 159 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLCAMPCHANGEDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLCampChangeDlg::FinalCtrl() 
{
	GM_DELETE( m_pArmsCreatMenuLabel ) ;			// 병기 생성 메뉴
	GM_DELETE( m_pArmsRemainTimeLabel ) ;			// 타입 변형 시간 설명
	GM_DELETE( m_pArmsCreatExpla1Label ) ;			// 병기 생성 설명 1
	GM_DELETE( m_pArmsCreatExpla2Label ) ;			// 병기 생성 설명 2
/*
	GM_DELETE( m_pArmsCreatExpla3Label ) ;			// 병기 생성 설명 3
	GM_DELETE( m_pArmsCreatExpla4Label ) ;			// 병기 생성 설명 4
*/
	GM_DELETE( m_pArmsCreatExpla5Label ) ;			// 병기 생성 설명 5
	GM_DELETE( m_pOKButton ) ;
	GM_DELETE( m_pCancelButton ) ;
}

VOID	CRYLCampChangeDlg::InitValue() 
{
}

VOID	CRYLCampChangeDlg::Show( CGMUIObject* Sender ) 
{
	CHAR strString[MAX_PATH];
	sprintf( strString, CRYLStringTable::m_strString[ 3182 ], m_nMaterialNeed ) ;
	m_pArmsCreatExpla2Label->SetCaption( strString ) ;
}

VOID	CRYLCampChangeDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLCampChangeDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;

	CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID );
	if ( pCamp && pCamp->GetMaterial() )
	{
		if ( pCamp->GetMaterial()->GetNowDurability() > 0 )
		{
			pCamp->GetMaterial()->m_wPutX = GetLeft() - 6 + ( pCamp->GetMaterial()->m_lpSprite->GetSizeX() / 2 ) ;
			pCamp->GetMaterial()->m_wPutY = GetTop()  + 73 + ( pCamp->GetMaterial()->m_lpSprite->GetSizeY() / 2 ) ;
			pCamp->GetMaterial()->Render( lpD3DDevice ) ;
		}
	}
	
	if ( pGame->m_lpPickItem )
	{
		if ( g_DeviceInput.InRect( GetLeft() + 13, GetTop() + 88, GetLeft() + 66, GetTop() + 142 ) )
		{
			pGame->RenderRect( lpD3DDevice, GetLeft() + 14, GetTop() + 89, GetLeft() + 67, GetTop() + 143, 0xFF0000FF, 0x80 ) ;
		} 
	}
}

HRESULT		CRYLCampChangeDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLCampChangeDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLCampChangeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal() ;
	unsigned long dwMousePosX = ptMousePos->x - ( GetLeft() + 13 ) ;
	unsigned long dwMousePosY = ptMousePos->y - ( GetTop() + 73 ) ;
	unsigned long dwWindowX = dwMousePosX / 26 ;
	unsigned long dwWindowY = dwMousePosY / 26 ;

	if (g_DeviceInput.InRect( GetLeft() + 13, GetTop() + 88, GetLeft() + 66, GetTop() + 142 ) )
	{
		if (g_DeviceInput.GetIsLeftMouseDown())
		{
			CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID );
			if ( pCamp )
			{
				if ( pGame->m_lpPickItem )	// 손에 있을시
				{
					if ( pGame->m_lpPickItem->GetItemType() == Item::ItemType::ETC )
					{
						if ( pCamp->GetMaterial() )
						{
							if (pGame->m_lpPickItem->GetProtoTypeID() == pCamp->GetMaterial()->GetProtoTypeID())
							{
								if (pCamp->GetMaterial()->GetNowDurability() < pCamp->GetMaterial()->GetMaxDurability())
								{
									unsigned char cNum = 0;
									if (pCamp->GetMaterial()->GetMaxDurability() - pCamp->GetMaterial()->GetNowDurability() < pGame->m_lpPickItem->GetNowDurability())
									{
										cNum = pCamp->GetMaterial()->GetMaxDurability() - pCamp->GetMaterial()->GetNowDurability();
									} 
									else
									{
										cNum = pGame->m_lpPickItem->GetNowDurability();
									}

									pNetwork->SendTakeMaterial(m_dwCampID, PktTakeMaterial::SC_TAKE_IN, cNum, pGame->m_lpPickItem->m_lpItemBase->GetPos());
								}
								else
								{
									CRYLMessageBox* pMessageBox = new CRYLMessageBox ;
									pMessageBox->Create( CRYLStringTable::m_strString[ 3566 ] ) ;
								}

								return S_OK;
							} 
							else	// 다른 광물
							{
								// 에러 메세지 출력해야함!!!

								return S_OK;
							}
						} 
						else
						{
							pNetwork->SendTakeMaterial(m_dwCampID, PktTakeMaterial::SC_TAKE_IN,
								pGame->m_lpPickItem->GetNowDurability(), pGame->m_lpPickItem->m_lpItemBase->GetPos());
							
							return S_OK;
						}
					}
				} 
				else
				{
					// 광물을 줍는다.
					if (pGame->m_dwKindCursor == CURSOR_NORMAL && pCamp->GetMaterial())
					{
						if ( pCamp->GetMaterial()->GetNowDurability() > 0 )
						{
							Item::ItemPos ItemIndex;
							ItemIndex.m_cIndex = Item::ExtraSpacePos::HOLDITEM_POS;
							ItemIndex.m_cPos = TakeType::TS_TEMP;

							pNetwork->SendTakeMaterial(m_dwCampID, PktTakeMaterial::SC_TAKE_OUT,
								pCamp->GetMaterial()->GetNowDurability(), ItemIndex);
						}
					}
				}
			}
		}
	}

	return S_OK ;
}

VOID	CRYLCampChangeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLCampChangeDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLCAMPCHANGEDLG::OK_BUTTON :
			{
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( g_pRYLCampChangeDlg->m_dwCampID );
				if ( pCamp )
				{
					if ( g_pRYLCampChangeDlg->m_nMaterialNeed == 0 )
					{
						CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
						SendPacket::CharCampCmd( g_GameSession, 
												pNetwork->m_dwMyChrID, 
												g_pRYLCampChangeDlg->m_dwCampID, 
												g_pRYLCampChangeDlg->m_dwChangeCampType, 
												0,
												PktCampCmd::CAMP_CHANGE_TYPE ) ;
							

						g_pRYLCampChangeDlg->SetVisible( FALSE ) ;
					}
					else if ( pCamp->GetMaterial() )
					{
						if ( g_pRYLCampChangeDlg->m_nMaterialNeed <= pCamp->GetMaterial()->GetNowDurability() )
						{
							CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
							SendPacket::CharCampCmd( g_GameSession, 
													pNetwork->m_dwMyChrID, 
													g_pRYLCampChangeDlg->m_dwCampID, 
													g_pRYLCampChangeDlg->m_dwChangeCampType, 
													0,
													PktCampCmd::CAMP_CHANGE_TYPE ) ;
							

							g_pRYLCampChangeDlg->SetVisible( FALSE ) ;
						}
						else
						{
							CRYLMessageBox* pMessageBox = new CRYLMessageBox ;
							pMessageBox->Create( CRYLStringTable::m_strString[ 3029 ] ) ;
						}
					}
					else
					{
						CRYLMessageBox* pMessageBox = new CRYLMessageBox ;
						pMessageBox->Create( CRYLStringTable::m_strString[ 3030 ] ) ;
					}
				}
			}
			break ;
		case RYLCAMPCHANGEDLG::CANCEL_BUTTON :
			{
				g_pRYLCampChangeDlg->SetVisible( FALSE ) ;
			}
			break ;
	}
}
