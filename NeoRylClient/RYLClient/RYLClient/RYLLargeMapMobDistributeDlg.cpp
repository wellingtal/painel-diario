
#include <winsock2.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/ClientSocket/Send/SendCommunity.h>

#include "RYLClientMain.h"
#include "RYLLargeMapMobDistributeDlg.h"
#include "GMUIObject.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLPicture.h"
#include "RYLSpriteEx.h"
#include "RYLSprite.h"
#include "Texture.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"

#include "BaseGraphicsLayer.h"
#include <RenderOption.h>
#include <shlwapi.h>

#include "RYLLargeMapFrameDlg.h"
#include "GMMemory.h"

CRYLMobDistributeDlg*	g_pMobDistributeDlg = NULL;

CRYLMobDistributeDlg::CRYLMobDistributeDlg( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	INT i = 0 ;
	m_pGMUIParentDlg		= pParentDlg;
	g_pMobDistributeDlg		= this;
	m_cLevel				= 0;
	
	for ( i = 0 ; i < 4 ; i ++ )
	{
		m_pMobDistributeSprite[ i ]	= NULL;
		m_pOriginSprite[ i ]		= NULL;
		
	}
	for ( i = 0 ; i < 6 ; i ++ )
	{
		m_pIndexLabel[ i ]			= NULL;
	}
	InitCtrl();
}


CRYLMobDistributeDlg::~CRYLMobDistributeDlg()
{
	FinalCtrl() ;
}


VOID
CRYLMobDistributeDlg::InitCtrl()
{
	
	m_pIndexLabel[0] = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pIndexLabel[0]->SetLeft( 9 );
	m_pIndexLabel[0]->SetTop( 473 );
	m_pIndexLabel[0]->SetFontColor (0xffffffff);
	m_pIndexLabel[0]->SetAutoSize( FALSE ) ;
	m_pIndexLabel[0]->SetCaption("");
	m_pIndexLabel[0]->SetWidth( 34 );
	m_pIndexLabel[0]->SetHeight( 72 );
	m_pIndexLabel[0]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2728 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pIndexLabel[0] ) ;

	m_pIndexLabel[1] = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pIndexLabel[1]->SetLeft( 47 );
	m_pIndexLabel[1]->SetTop( 473 );
	m_pIndexLabel[1]->SetFontColor (0xffffffff);
	m_pIndexLabel[1]->SetAutoSize( FALSE ) ;
	m_pIndexLabel[1]->SetWidth( 34 );
	m_pIndexLabel[1]->SetHeight( 72 );
	m_pIndexLabel[1]->SetCaption("");
	m_pIndexLabel[1]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2729 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pIndexLabel[1] ) ;

	m_pIndexLabel[2] = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pIndexLabel[2]->SetLeft( 85 );
	m_pIndexLabel[2]->SetTop( 473 );
	m_pIndexLabel[2]->SetFontColor (0xffffffff);
	m_pIndexLabel[2]->SetAutoSize( FALSE ) ;
	m_pIndexLabel[2]->SetWidth( 34 );
	m_pIndexLabel[2]->SetHeight( 72 );
	m_pIndexLabel[2]->SetCaption("");
	m_pIndexLabel[2]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2730 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pIndexLabel[2] ) ;

	m_pIndexLabel[3] = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pIndexLabel[3]->SetLeft( 123 );
	m_pIndexLabel[3]->SetTop( 473 );
	m_pIndexLabel[3]->SetFontColor (0xffffffff);
	m_pIndexLabel[3]->SetAutoSize( FALSE ) ;
	m_pIndexLabel[3]->SetWidth( 34 );
	m_pIndexLabel[3]->SetHeight( 72 );
	m_pIndexLabel[3]->SetCaption("");
	m_pIndexLabel[3]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2731 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pIndexLabel[3] ) ;

	m_pIndexLabel[4] = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pIndexLabel[4]->SetLeft( 161 );
	m_pIndexLabel[4]->SetTop( 473 );
	m_pIndexLabel[4]->SetFontColor (0xffffffff);
	m_pIndexLabel[4]->SetAutoSize( FALSE ) ;
	m_pIndexLabel[4]->SetWidth( 34 );
	m_pIndexLabel[4]->SetHeight( 72 );
	m_pIndexLabel[4]->SetCaption("");
	m_pIndexLabel[4]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2732 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pIndexLabel[4] ) ;
	


	m_pIndexLabel[5] = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pIndexLabel[5]->SetLeft( 199 );
	m_pIndexLabel[5]->SetTop( 473 );
	m_pIndexLabel[5]->SetFontColor (0xffffffff);
	m_pIndexLabel[5]->SetAutoSize( FALSE ) ;
	m_pIndexLabel[5]->SetWidth( 34 );
	m_pIndexLabel[5]->SetHeight( 72 );
	m_pIndexLabel[5]->SetCaption("");
	m_pIndexLabel[5]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2733 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pIndexLabel[5] ) ;


	m_pMobDistributeSprite[ 0 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pMobDistributeSprite[ 0 ]->m_pSprite = new CRYLSprite ;
	m_pMobDistributeSprite[ 0 ]->m_pTexture = new CTexture ;
	m_pMobDistributeSprite[ 0 ]->SetLeft( 0 ) ;
	m_pMobDistributeSprite[ 0 ]->SetTop( 0 ) ;
	m_pMobDistributeSprite[ 0 ]->SetWidth( 256 ) ;
	m_pMobDistributeSprite[ 0 ]->SetHeight( 256 ) ;
	m_pMobDistributeSprite[ 0 ]->SetTextureAlpha( TRUE );
	CGMUICustomPanel::AddChild( m_pMobDistributeSprite[ 0 ] ) ;

	m_pMobDistributeSprite[ 1 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pMobDistributeSprite[ 1 ]->m_pSprite = new CRYLSprite ;
	m_pMobDistributeSprite[ 1 ]->m_pTexture = new CTexture ;
	m_pMobDistributeSprite[ 1 ]->SetLeft( 256 ) ;
	m_pMobDistributeSprite[ 1 ]->SetTop( 0 ) ;
	m_pMobDistributeSprite[ 1 ]->SetWidth( 256 ) ;
	m_pMobDistributeSprite[ 1 ]->SetHeight( 256 ) ;
	m_pMobDistributeSprite[ 1 ]->SetTextureAlpha( TRUE );
	CGMUICustomPanel::AddChild( m_pMobDistributeSprite[ 1 ] ) ;

	m_pMobDistributeSprite[ 2 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pMobDistributeSprite[ 2 ]->m_pSprite = new CRYLSprite ;
	m_pMobDistributeSprite[ 2 ]->m_pTexture = new CTexture ;
	m_pMobDistributeSprite[ 2 ]->SetLeft( 0 ) ;
	m_pMobDistributeSprite[ 2 ]->SetTop( 256 ) ;
	m_pMobDistributeSprite[ 2 ]->SetWidth( 256 ) ;
	m_pMobDistributeSprite[ 2 ]->SetHeight( 256 ) ;
	m_pMobDistributeSprite[ 2 ]->SetTextureAlpha( TRUE );
	CGMUICustomPanel::AddChild( m_pMobDistributeSprite[ 2 ] ) ;

	m_pMobDistributeSprite[ 3 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pMobDistributeSprite[ 3 ]->m_pSprite = new CRYLSprite ;
	m_pMobDistributeSprite[ 3 ]->m_pTexture = new CTexture ;
	m_pMobDistributeSprite[ 3 ]->SetLeft( 256 ) ;
	m_pMobDistributeSprite[ 3 ]->SetTop( 256 ) ;
	m_pMobDistributeSprite[ 3 ]->SetWidth( 256 ) ;
	m_pMobDistributeSprite[ 3 ]->SetHeight( 256 ) ;
	m_pMobDistributeSprite[ 3 ]->SetTextureAlpha( TRUE );
	CGMUICustomPanel::AddChild( m_pMobDistributeSprite[ 3 ] ) ;

	

}

VOID
CRYLMobDistributeDlg::FinalCtrl()
{
	INT i = 0 ;
	for ( i = 0 ; i < 4 ; i ++ )
		GM_DELETE( m_pMobDistributeSprite[ i ] ) ;
	for ( i = 0 ; i < 4 ; i ++ )
		GM_DELETE_ARRAY( m_pOriginSprite[ i ] ) ;
	for ( i = 0 ; i < 6 ; i ++ )
		GM_DELETE( m_pIndexLabel[i] ) ;
}


VOID	
CRYLMobDistributeDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;


	CGMUICustomPanel::GMUIPaint( xx, yy ) ;
///////////////////
	CRYLLargeMapFrameDlg *pDlg = (CRYLLargeMapFrameDlg*)( ((CRYLLargeMapFrameDlg*)m_pGMUIParentDlg)->GetCampDistributePanel() ) ;
	pDlg->GMUIPaint(xx,yy) ;

	
	INT			i = 0;
	for( i = 0 ; i < 4; i++ )
	{
        m_pMobDistributeSprite[i];
	}
/////////////////

}



HRESULT		
CRYLMobDistributeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	
CRYLMobDistributeDlg::Show( CGMUIObject* Sender ) 
{

	
}

VOID	
CRYLMobDistributeDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	
CRYLMobDistributeDlg::LoadMobDistributeTexture()
{
	INT i;
	long	width, height;
	CHAR	strFile[ MAX_PATH ] ;
	CHAR	strFullPath[ MAX_PATH ] ;
	CTexture::SetPath(INTERFACETEXTUREPATH);

	for ( i = 0 ; i < 4 ; i ++ )
	{
		GM_DELETE_ARRAY( m_pOriginSprite[ i ] ) ;
	}
	
	unsigned char cLargeMapZone = CRYLNetworkData::Instance()->m_dwPresentZone;
	switch (cLargeMapZone)
	{
		case SERVER_ID::PRISON:				cLargeMapZone = 100;					break;
	}

	for ( unsigned long i = 0 ; i < 4 ; i ++ )
	{
        sprintf( strFile, "Zone%d\\dmap%02d.dds", cLargeMapZone, i + 1 ) ;

		if ( m_pMobDistributeSprite[ i ]->m_pTexture->GetTexture()) 
		{
			m_pMobDistributeSprite[ i ]->m_pTexture->Unload() ;
		}

		memset( strFullPath, 0, sizeof(CHAR) * MAX_PATH );
		strcpy( strFullPath , CTexture::GetPath() );
		strcat( strFullPath, "\\" );
		strcat( strFullPath, strFile );

		if( !PathFileExists( strFullPath ) )
		{
			continue;
		}

		m_pMobDistributeSprite[ i ]->m_pTexture->Load(strFile);

		height = m_pMobDistributeSprite[ i ]->m_pTexture->m_dwHeight;
		width  = m_pMobDistributeSprite[ i ]->m_pTexture->m_dwWidth;
		m_pOriginSprite[ i ] = new COLOR_ARGB[ height * width ];

		DWORD* pTexture = (DWORD*) m_pMobDistributeSprite[ i ]->m_pTexture->Lock();
		
		if( pTexture )
		{
			memcpy( m_pOriginSprite[i], pTexture, sizeof(DWORD) * height * width );
		}
		
		m_pMobDistributeSprite[ i ]->m_pTexture->Unlock();
	}

	PlayerLevelChanged();
}

VOID	
CRYLMobDistributeDlg::PlayerLevelChanged()
{
	m_cLevel = CRYLGameData::Instance()->m_csStatus.m_Info.Level;
	UpdateDistributeMap();
}


VOID	
CRYLMobDistributeDlg::UpdateDistributeMap()
{
	INT		i = 0;	
	long	width, height;
	long	dx, dy;
	long	pos;
	short	mobLevel;


	COLOR_ARGB*		pClr;
	COLOR_ARGB		clr;
	for ( i = 0 ; i < 4 ; i ++ )
	{
		if( !m_pOriginSprite[ i ] )
		{
			m_pMobDistributeSprite[i]->SetVisible( FALSE );
			continue;
		}


		height = m_pMobDistributeSprite[ i ]->m_pTexture->m_dwHeight;
		width  = m_pMobDistributeSprite[ i ]->m_pTexture->m_dwWidth;
		DWORD *pTexture = (DWORD*) m_pMobDistributeSprite[ i ]->m_pTexture->Lock();
		if( pTexture )
		{
			for( dy = 0; dy <  height; dy++ )
			{
				for( dx = 0; dx < width ; dx++ )
				{
					pos = dx + dy*width;
					pClr = &(m_pOriginSprite[i][ pos ]);
					if( pClr->a == 0xFF )
					{
						if( pClr->r == 255 )			//문자열 이라면
						{
							
							switch( pClr->g )
							{
							case 6:
								clr.color = RYLMOBDISTRIBUTEDLG::DARKRED ;
								clr.a = 0xC8;
								break;
							case 5:
								clr.color = RYLMOBDISTRIBUTEDLG::RED ;
								clr.a = 0xC8;
								break;
							case 4:
								clr.color = RYLMOBDISTRIBUTEDLG::BRIGHTRED ;
								clr.a = 0xC8;
								break;
							case 3:
								clr.color = RYLMOBDISTRIBUTEDLG::ORANGE ;
								clr.a = 0xC8;
								break;
							case 2:
								clr.color = RYLMOBDISTRIBUTEDLG::YELLOW ;
								clr.a = 0xC8;
								break;
							case 1:
								clr.color = RYLMOBDISTRIBUTEDLG::GREEN ;
								clr.a = 0xC8;
                                break;
							case 7:
								clr.color = RYLMOBDISTRIBUTEDLG::BLACK ;
								clr.a = 0x80;
								break;
							default:
								clr.color = 0x00000000 ;
								break;
							}


						}
						else
						{
							mobLevel = pClr->r * 2 ;
							mobLevel = mobLevel - m_cLevel ;

							if ( mobLevel > 20 || mobLevel < -4 )			// 21 이상 혹은 -5 이하
								clr.color = 0x00000000 ;
							else if ( mobLevel > 16 && mobLevel < 21 )		// 17 ~ 20 사이
								clr.color = RYLMOBDISTRIBUTEDLG::DARKRED ;
							else if ( mobLevel > 12 && mobLevel < 17 )		// 13 ~ 16 사이
								clr.color = RYLMOBDISTRIBUTEDLG::RED ;
							else if ( mobLevel > 8 && mobLevel < 13 )		// 9 ~ 12 사이
								clr.color = RYLMOBDISTRIBUTEDLG::BRIGHTRED ;
							else if ( mobLevel > 4 && mobLevel < 9 )		// 5 ~ 8 사이
								clr.color = RYLMOBDISTRIBUTEDLG::ORANGE ;
							else if ( mobLevel > 0 && mobLevel < 5 )		// 1 ~ 4 사이
								clr.color = RYLMOBDISTRIBUTEDLG::YELLOW ;
							else if ( mobLevel > -5 && mobLevel < 1 )		// -4 ~ 0 사이
								clr.color = RYLMOBDISTRIBUTEDLG::GREEN ;
						}
						pTexture[ pos ] = clr.color ;
					}
					else
					{
						pTexture[ pos ] = 0x00000000;
					}
				}
			}
			
		}
		m_pMobDistributeSprite[ i ]->m_pTexture->Unlock();
	}



	for ( i = 0 ; i < 4 ; i ++ )
	{
		m_pMobDistributeSprite[ i ]->m_pSprite->Create( 0, 0, 0, 0, 256, 256, m_pMobDistributeSprite[ i ]->m_pTexture ) ;
	}

}


VOID	
CRYLMobDistributeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pMobDistributeDlg )
		return ;

 ///////////////
	switch( Sender->GetInstance() )
	{
	default:
		break;
	}
//////////////
}
