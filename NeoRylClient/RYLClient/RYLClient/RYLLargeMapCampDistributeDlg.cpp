#include <winsock2.h>

#include "RYLClientMain.h"

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/ClientSocket/Send/SendCommunity.h>

#include <Creature/EnemyCheck.h>
#include <Creature/CreatureStructure.h>

#include <RenderOption.h>

#include "BaseGraphicsLayer.h"
#include "GMImageDib.h"
#include "WinInput.h"
#include "Texture.h"
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
#include "RYLSpriteList.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include <shlwapi.h>

#include "RYLCamp.h"
#include "RYLCampManager.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h" 
#include "RYLLargeMapCampDistributeDlg.h"

#include "..\\GUITextEdit.h"
#include "GMMemory.h"

CRYLCampDistributePanel*	g_pCampDistributeDlg = NULL;

CRYLCampDistributePanel::CRYLCampDistributePanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg		= pParentDlg;
	g_pCampDistributeDlg	= this;

	m_pCampDistributeSprite[ 0 ] = NULL ;
	m_pCampDistributeSprite[ 1 ] = NULL ;
	m_pCampDistributeSprite[ 2 ] = NULL ;

	m_pCampSimbolSprite[ 0 ] = NULL ;
	m_pCampSimbolSprite[ 1 ] = NULL ;
	m_pCampSimbolSprite[ 2 ] = NULL ;
	m_pCampSimbolSprite[ 3 ] = NULL ;

	m_pCampEnableBuild[ 0 ] = NULL ;
	m_pCampEnableBuild[ 1 ] = NULL ;
	m_pCampEnableBuild[ 2 ] = NULL ;
	m_pCampEnableBuild[ 3 ] = NULL ;
	

	m_pCampAttackedSprite = NULL ;

	InitCtrl();
}


CRYLCampDistributePanel::~CRYLCampDistributePanel()
{
	FinalCtrl() ;
}


VOID	CRYLCampDistributePanel::InitCtrl()
{
	INT iSize;
	DWORD* pBits = NULL ;
	CGMImageDib* pImageDIB = NULL ;

	m_pCampEnableBuild[ 0 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampEnableBuild[ 0 ]->m_pSprite = new CRYLSprite ;
	m_pCampEnableBuild[ 0 ]->m_pTexture = new CTexture ;
	m_pCampEnableBuild[ 0 ]->SetLeft( 0 ) ;
	m_pCampEnableBuild[ 0 ]->SetTop( 0 ) ;
	m_pCampEnableBuild[ 0 ]->SetWidth( 256 ) ;
	m_pCampEnableBuild[ 0 ]->SetHeight( 256 ) ;
	CGMUICustomPanel::AddChild( m_pCampEnableBuild[ 0 ] ) ;

	m_pCampEnableBuild[ 1 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampEnableBuild[ 1 ]->m_pSprite = new CRYLSprite ;
	m_pCampEnableBuild[ 1 ]->m_pTexture = new CTexture ;
	m_pCampEnableBuild[ 1 ]->SetLeft( 256 ) ;
	m_pCampEnableBuild[ 1 ]->SetTop( 0 ) ;
	m_pCampEnableBuild[ 1 ]->SetWidth( 256 ) ;
	m_pCampEnableBuild[ 1 ]->SetHeight( 256 ) ;
	CGMUICustomPanel::AddChild( m_pCampEnableBuild[ 1 ] ) ;

	m_pCampEnableBuild[ 2 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampEnableBuild[ 2 ]->m_pSprite = new CRYLSprite ;
	m_pCampEnableBuild[ 2 ]->m_pTexture = new CTexture ;
	m_pCampEnableBuild[ 2 ]->SetLeft( 0 ) ;
	m_pCampEnableBuild[ 2 ]->SetTop( 256 ) ;
	m_pCampEnableBuild[ 2 ]->SetWidth( 256 ) ;
	m_pCampEnableBuild[ 2 ]->SetHeight( 256 ) ;
	CGMUICustomPanel::AddChild( m_pCampEnableBuild[ 2 ] ) ;

	m_pCampEnableBuild[ 3 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampEnableBuild[ 3 ]->m_pSprite = new CRYLSprite ;
	m_pCampEnableBuild[ 3 ]->m_pTexture = new CTexture ;
	m_pCampEnableBuild[ 3 ]->SetLeft( 256 ) ;
	m_pCampEnableBuild[ 3 ]->SetTop( 256 ) ;
	m_pCampEnableBuild[ 3 ]->SetWidth( 256 ) ;
	m_pCampEnableBuild[ 3 ]->SetHeight( 256 ) ;
	CGMUICustomPanel::AddChild( m_pCampEnableBuild[ 3 ] ) ;

	// 카르테란트 길드 요새 영역 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_KARTERANT ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampDistributeSprite[0] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampDistributeSprite[0]->m_pSprite = new CRYLSprite ;
	m_pCampDistributeSprite[0]->m_pTexture = new CTexture ;
	m_pCampDistributeSprite[0]->m_pTexture->CreateEmptyTexture( 128, 128, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampDistributeSprite[0]->m_pTexture->SetBitTexture32( 94, 94, ( DWORD* )pBits ) ;
	m_pCampDistributeSprite[0]->m_pSprite->Create( 0, 0, 0, 0, 94, 94, m_pCampDistributeSprite[0]->m_pTexture ) ;

	// 메르카디아 길드 요새 영역 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_MERKADIA ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampDistributeSprite[1] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampDistributeSprite[1]->m_pSprite = new CRYLSprite ;
	m_pCampDistributeSprite[1]->m_pTexture = new CTexture ;
	m_pCampDistributeSprite[1]->m_pTexture->CreateEmptyTexture( 128, 128, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampDistributeSprite[1]->m_pTexture->SetBitTexture32( 94, 94, ( DWORD* )pBits ) ;
	m_pCampDistributeSprite[1]->m_pSprite->Create( 0, 0, 0, 0, 94, 94, m_pCampDistributeSprite[1]->m_pTexture ) ;

	// 제삼세력 길드 요새 영역 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_THIRD ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampDistributeSprite[2] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampDistributeSprite[2]->m_pSprite = new CRYLSprite ;
	m_pCampDistributeSprite[2]->m_pTexture = new CTexture ;
	m_pCampDistributeSprite[2]->m_pTexture->CreateEmptyTexture( 128, 128, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampDistributeSprite[2]->m_pTexture->SetBitTexture32( 94, 94, ( DWORD* )pBits ) ;
	m_pCampDistributeSprite[2]->m_pSprite->Create( 0, 0, 0, 0, 94, 94, m_pCampDistributeSprite[2]->m_pTexture ) ;

	// 길드 요새 공격 받을 시 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_ATTACKED ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampAttackedSprite = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampAttackedSprite->m_pSprite = new CRYLSprite ;
	m_pCampAttackedSprite->m_pTexture = new CTexture ;
	m_pCampAttackedSprite->m_pTexture->CreateEmptyTexture( 128, 128, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampAttackedSprite->m_pTexture->SetBitTexture32( 94, 94, ( DWORD* )pBits ) ;
	m_pCampAttackedSprite->m_pSprite->Create( 0, 0, 0, 0, 94, 94, m_pCampAttackedSprite->m_pTexture ) ;

	// 같은 국가 협력 심볼 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_SIMBOL1 ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampSimbolSprite[0] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampSimbolSprite[0]->m_pSprite = new CRYLSprite ;
	m_pCampSimbolSprite[0]->m_pTexture = new CTexture ;
	m_pCampSimbolSprite[0]->m_pTexture->CreateEmptyTexture( 64, 32, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampSimbolSprite[0]->m_pTexture->SetBitTexture32( 53, 17, ( DWORD* )pBits ) ;
	m_pCampSimbolSprite[0]->m_pSprite->Create( 0, 0, 0, 0, 53, 17, m_pCampSimbolSprite[0]->m_pTexture ) ;

	// 다른 국가 협력 심볼 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_SIMBOL2 ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampSimbolSprite[1] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampSimbolSprite[1]->m_pSprite = new CRYLSprite ;
	m_pCampSimbolSprite[1]->m_pTexture = new CTexture ;
	m_pCampSimbolSprite[1]->m_pTexture->CreateEmptyTexture( 64, 32, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampSimbolSprite[1]->m_pTexture->SetBitTexture32( 53, 17, ( DWORD* )pBits ) ;
	m_pCampSimbolSprite[1]->m_pSprite->Create( 0, 0, 0, 0, 53, 17, m_pCampSimbolSprite[1]->m_pTexture ) ;

	// 같은 국가 적대 심볼 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_SIMBOL3 ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampSimbolSprite[2] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampSimbolSprite[2]->m_pSprite = new CRYLSprite ;
	m_pCampSimbolSprite[2]->m_pTexture = new CTexture ;
	m_pCampSimbolSprite[2]->m_pTexture->CreateEmptyTexture( 64, 32, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampSimbolSprite[2]->m_pTexture->SetBitTexture32( 53, 17, ( DWORD* )pBits ) ;
	m_pCampSimbolSprite[2]->m_pSprite->Create( 0, 0, 0, 0, 53, 17, m_pCampSimbolSprite[2]->m_pTexture ) ;

	// 다른 국가 적대 심볼 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_SIMBOL4 ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampSimbolSprite[3] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampSimbolSprite[3]->m_pSprite = new CRYLSprite ;
	m_pCampSimbolSprite[3]->m_pTexture = new CTexture ;
	m_pCampSimbolSprite[3]->m_pTexture->CreateEmptyTexture( 64, 32, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampSimbolSprite[3]->m_pTexture->SetBitTexture32( 53, 17, ( DWORD* )pBits ) ;
	m_pCampSimbolSprite[3]->m_pSprite->Create( 0, 0, 0, 0, 53, 17, m_pCampSimbolSprite[3]->m_pTexture ) ;
}

VOID	CRYLCampDistributePanel::FinalCtrl()
{
	GM_DELETE( m_pCampEnableBuild[0] ) ;
	GM_DELETE( m_pCampEnableBuild[1] ) ;
	GM_DELETE( m_pCampEnableBuild[2] ) ;
	GM_DELETE( m_pCampEnableBuild[3] ) ;

	GM_DELETE( m_pCampDistributeSprite[0] ) ;
	GM_DELETE( m_pCampDistributeSprite[1] ) ;
	GM_DELETE( m_pCampDistributeSprite[2] ) ;

	GM_DELETE( m_pCampSimbolSprite[0] ) ;
	GM_DELETE( m_pCampSimbolSprite[1] ) ;
	GM_DELETE( m_pCampSimbolSprite[2] ) ;
	GM_DELETE( m_pCampSimbolSprite[3] ) ;

	GM_DELETE( m_pCampAttackedSprite ) ;
}

VOID	CRYLCampDistributePanel::LoadCampEnableBuildMapTexture()
{
	CTexture::SetPath( INTERFACETEXTUREPATH ) ;
	CHAR	strFile[ MAX_PATH ] ;

	unsigned char cLargeMapZone = CRYLNetworkData::Instance()->m_dwPresentZone;
	switch (cLargeMapZone)
	{
		case SERVER_ID::PRISON:				cLargeMapZone = 100;					break;
	}

	for ( unsigned long i = 0 ; i < 4 ; i ++ )
	{
        sprintf( strFile, "Zone%d\\tmap%02d.dds", cLargeMapZone, i + 1 ) ;

		if ( m_pCampEnableBuild[ i ]->m_pTexture->GetTexture()) 
		{
			m_pCampEnableBuild[ i ]->m_pTexture->Unload() ;
		}

		m_pCampEnableBuild[ i ]->m_pTexture->Load(strFile);
		m_pCampEnableBuild[ i ]->m_pSprite->Create( 0, 0, 0, 0, 256, 256, m_pCampEnableBuild[ i ]->m_pTexture ) ;
	}

}

VOID	CRYLCampDistributePanel::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CGMUICustomPanel::GMUIPaint( xx, yy ) ;

	lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) ;
	lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;
    lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ) ;

	DrawCampArea( xx, yy ) ;
	DrawGuildInfo( xx, yy ) ;
}



HRESULT	CRYLCampDistributePanel::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLCampDistributePanel::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLCampDistributePanel::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLCampDistributePanel::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pCampDistributeDlg )
		return ;
}

void	CRYLCampDistributePanel::DrawCampArea( INT xx, INT yy )
{
	std::map< unsigned long, CRYLCamp* > Node = CRYLCampManager::Instance()->GetNode() ;
	std::map< unsigned long, CRYLCamp* >::iterator iter = Node.begin() ;
	long lX, lY ;

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLGuildFrameDlg* pDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
	CRYLGuildInfoPanel* pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel();
	INT iIndex ;
	RECT rt ;

	while( iter != Node.end() )
	{
		CRYLCamp* pCamp = iter->second ;
		if ( pCamp && 0 != pCamp->GetCampNation() )
		{
			switch(	CRYLNetworkData::Instance()->m_dwPresentZone)
			{
				case SERVER_ID::ZONE1:
				//	lX = (long)(pCamp->GetPointX() / 3465.0f * 512) - 48;
				//	lY = (long)( 512 - pCamp->GetPointZ() / 3465.0f * 512) - 46 ;
					lX = (long)( (long)(pCamp->GetPointX() - 315) * 512 / 3465.0f ) - 47 ;
					lY = (long)( 512 - (long)(pCamp->GetPointZ() - 315) * 512 / 3465.0f ) - 47 ;
					break;
					break;

				case SERVER_ID::ZONE2:
					lX = (long)(pCamp->GetPointX() / 3422.0f * 512) - 52;
					lY = (long)( 512 - pCamp->GetPointZ() / 3438.0f * 512) + 52;
					break;

				case SERVER_ID::ZONE4:
					lX = (long)(pCamp->GetPointX() / 2463.0f * 512) - 68;
					lY = (long)( 512 - pCamp->GetPointZ() / 2383.0f * 512) + 83;
					break;

				case SERVER_ID::ZONE5:
					lX = (long)(pCamp->GetPointX() / 3120.0f * 512) - 53;
					lY = (long)( 512 - pCamp->GetPointZ() / 3104 * 512) + 55;
					break;

				case SERVER_ID::CAPITAL :
				case SERVER_ID::ZONE12 :
				{
					lX = (long)( (long)(pCamp->GetPointX() - 315) * 512 / 3465.0f ) - 47 ;
					lY = (long)( 512 - (long)(pCamp->GetPointZ() - 315) * 512 / 3465.0f ) - 47 ;
					break;
				}
				case SERVER_ID::ZONE9 :
				{
					lX = (long)( (long)(pCamp->GetPointX() - 630) * 512 / 3150.0f ) - 47 ;
					lY = (long)( 512 - (long)(pCamp->GetPointZ() - 630) * 512 / 3150.0f ) - 47 ;
					break;
				}
			}

			SetRect( &rt, 0, 0, 94, 94 ) ;

			if ( (lX + 94) >= 512 ) rt.right -= (lX + 94) - 512 ;
			if ( (lY + 94) >= 512 ) rt.bottom -= (lY + 94) - 512 ;

			if ( lX <= 0 )
			{
				rt.left -= lX ;
				lX = 0 ;
			}
			if ( lY <= 0 ) 				
			{
				rt.top -= lY ;
				lY = 0 ;
			}

			if ( lX >= 512 || lY >= 512 ||
				 lX + rt.right <= 0 ||
				 lY + rt.bottom <= 0 )
			{
				++ iter ;
				continue;
			}

			switch ( pCamp->GetCampNation() )
			{
				case Creature::KARTERANT:			iIndex = 0 ;	break;
				case Creature::MERKADIA:			iIndex = 1 ;	break;
				case Creature::ALMIGHTY_PIRATE:		iIndex = 2 ;	break;
			}

			if ( Siege::DEVELOPING == pCamp->GetState() )
			{
				pCamp->UpdateAlpha() ;
				if ( pCamp->GetAlpha() > 0 )
				{
					m_pCampDistributeSprite[ iIndex ]->m_pSprite->m_nPutX = xx + lX ;
					m_pCampDistributeSprite[ iIndex ]->m_pSprite->m_nPutY = yy + lY ;
					m_pCampDistributeSprite[ iIndex ]->m_pSprite->Render( lpD3DDevice, pCamp->GetAlpha(), &rt ) ;
				}
			}
			else
			{
				m_pCampDistributeSprite[ iIndex ]->m_pSprite->m_nPutX = xx + lX ;
				m_pCampDistributeSprite[ iIndex ]->m_pSprite->m_nPutY = yy + lY ;
				m_pCampDistributeSprite[ iIndex ]->m_pSprite->Render( lpD3DDevice, pCamp->GetDefaultAlpha(), &rt ) ;
			}

			if ( pCamp->GetAttacked() )
			{
				pCamp->UpdateAttackedAlpha() ;
				if ( pCamp->GetAttackedAlpha() > 0 )
				{
					m_pCampAttackedSprite->m_pSprite->m_nPutX = xx + lX ;
					m_pCampAttackedSprite->m_pSprite->m_nPutY = yy + lY ;
					m_pCampAttackedSprite->m_pSprite->Render( lpD3DDevice, pCamp->GetAttackedAlpha(), &rt ) ;
				}
			}
		}

		++ iter ;
	}
}

void	CRYLCampDistributePanel::DrawGuildInfo( INT xx, INT yy )
{
	std::map< unsigned long, CRYLCamp* > Node = CRYLCampManager::Instance()->GetNode() ;
	std::map< unsigned long, CRYLCamp* >::iterator iter = Node.begin() ;
	RECT rcWeapon, rcName, rcMark;
	long lX, lY ;
	unsigned long dwLength ;
	int iHalfSize ;

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLGuildFrameDlg* pDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
	CRYLGuildInfoPanel* pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel();
	CRYLSprite* pMarkSprite = NULL ;

	while ( iter != Node.end() )
	{
		CRYLCamp* pCamp = iter->second ;
		if ( pCamp )
		{
			switch(	CRYLNetworkData::Instance()->m_dwPresentZone)
			{
				case SERVER_ID::ZONE1:
				//	lX = (long)(pCamp->GetPointX() / 3465.0f * 512) - 48;
				//	lY = (long)( 512 - pCamp->GetPointZ() / 3465.0f * 512) - 46 ;
					lX = (long)( ( (long)(pCamp->GetPointX()) - 315 ) * 512 / 3465.0f ) ;
					lY = (long)( 512 - ( (long)(pCamp->GetPointZ()) - 315 ) * 512 / 3465.0f ) ;
					break;

				case SERVER_ID::ZONE2:
					lX = (long)(pCamp->GetPointX() / 3422.0f * 512) - 52;
					lY = (long)( 512 - pCamp->GetPointZ() / 3438.0f * 512) + 52;
					break;

				case SERVER_ID::ZONE4:
					lX = (long)(pCamp->GetPointX() / 2463.0f * 512) - 68;
					lY = (long)( 512 - pCamp->GetPointZ() / 2383.0f * 512) + 83;
					break;

				case SERVER_ID::ZONE5:
					lX = (long)(pCamp->GetPointX() / 3120.0f * 512) - 53;
					lY = (long)( 512 - pCamp->GetPointZ() / 3104 * 512) + 55;
					break;

				case SERVER_ID::CAPITAL :
				case SERVER_ID::ZONE12 :
				{
					lX = (long)( ( (long)(pCamp->GetPointX()) - 315 ) * 512 / 3465.0f ) ;
					lY = (long)( 512 - ( (long)(pCamp->GetPointZ()) - 315 ) * 512 / 3465.0f ) ;
					break ;
				}
				case SERVER_ID::ZONE9 :
				{
					lX = (long)( ( (long)(pCamp->GetPointX()) - 630 ) * 512 / 3150.0f ) ;
					lY = (long)( 512 - ( (long)(pCamp->GetPointZ()) - 630 ) * 512 / 3150.0f ) ;
					break ;
				}
			}

			// 월드 웨폰은 월드 웨폰이라고 출력해준다.
			bool bWorldWeaponRender = false;
			RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() ) ;
			if ( pTempCreature )
			{
				if ( Siege::KARTERANT_WEAPON == pTempCreature->GetObjectType() ||
					 Siege::MERKADIA_WEAPON == pTempCreature->GetObjectType() )
				{
					dwLength = CRYLStringTable::GetStringWidth( CRYLStringTable::m_strString[ 3480 ] ) ;
					iHalfSize = dwLength / 2 ;

					// 월드 웨폰 출력
					SetRect( &rcWeapon, lX - iHalfSize, lY - 54, lX + iHalfSize, lY - 24 ) ;
					if ( rcWeapon.right > 512 )
					{
						rcWeapon.left -= rcWeapon.right - 512;
						rcWeapon.right = 512;
					}

					if ( rcWeapon.bottom > 512 )
					{
						rcWeapon.top -= rcWeapon.bottom - 512 - 15;
						rcWeapon.bottom = 512 - 15;
					}

					if ( rcWeapon.left < 0 )
					{
						rcWeapon.right += -rcWeapon.left;
						rcWeapon.left = 0;
					}

					if ( rcWeapon.top < 0 )
					{
						rcWeapon.bottom += -rcWeapon.top;
						rcWeapon.top = 0;
					}

					rcWeapon.left += xx ;
					rcWeapon.right += xx ;
					rcWeapon.top += yy ;
					rcWeapon.bottom += yy ;

					bWorldWeaponRender = true;
						
					++rcWeapon.left ;	++rcWeapon.right ;	++rcWeapon.top ;	++rcWeapon.bottom ;
					g_TextEdit.DrawText( CRYLStringTable::m_strString[ 3480 ], &rcWeapon, DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255) ) ;
					--rcWeapon.left ;	--rcWeapon.right ;	--rcWeapon.top ;	--rcWeapon.bottom ;
					g_TextEdit.DrawText( CRYLStringTable::m_strString[ 3480 ], &rcWeapon, DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255) ) ;
				}
			}

			// 길드명 && 길드 마크 출력
			const char* szGuildName = pPanel->GetGuildName( pCamp->GetGID() ) ;
			pMarkSprite = pPanel->GetGuildSprite( pCamp->GetGID() ) ;

			if ( szGuildName )
			{
				dwLength = CRYLStringTable::GetStringWidth( szGuildName ) ;
				
				if ( pMarkSprite )
				{
					iHalfSize = (dwLength + 3 + 18) / 2 ;
				}
				else
				{
					iHalfSize = dwLength / 2 ;
				}

				if ( bWorldWeaponRender )
				{
					SetRect( &rcName, lX - iHalfSize, lY - 39, lX + iHalfSize, lY - 9 ) ;

					if ( rcName.right > 512 )
					{
						rcName.left -= rcName.right - 512;
						rcName.right = 512;
					}

					if ( rcName.bottom > 512 )
					{
						rcName.top -= rcName.bottom - 512;
						rcName.bottom = 512;
					}

					if ( rcName.left < 0 )
					{
						rcName.right += -rcName.left;
						rcName.left = 0;
					}

					if ( rcName.top < 0 )
					{
						rcName.bottom += -rcName.top + 15;
						rcName.top = 15;
					}
				}
				else
				{
					SetRect( &rcName, lX - iHalfSize, lY - 39, lX + iHalfSize, lY - 9 ) ;

					if ( rcName.right > 512 )
					{
						rcName.left -= rcName.right - 512;
						rcName.right = 512;
					}

					if ( rcName.bottom > 512 )
					{
						rcName.top -= rcName.bottom - 512;
						rcName.bottom = 512;
					}

					if ( rcName.left < 0 )
					{
						rcName.right += -rcName.left;
						rcName.left = 0;
					}

					if ( rcName.top < 0 )
					{
						rcName.bottom += -rcName.top;
						rcName.top = 0;
					}
				}
				
				rcName.left += xx ;
				rcName.right += xx ;
				rcName.top += yy ;
				rcName.bottom += yy ;

				// 길드명 출력
				++rcName.left ;	++rcName.right ;	++rcName.top ;	++rcName.bottom ;
				g_TextEdit.DrawText( szGuildName, &rcName, DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255) ) ;
				--rcName.left ;	--rcName.right ;	--rcName.top ;	--rcName.bottom ;
				g_TextEdit.DrawText( szGuildName, &rcName, DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255) ) ;
				
				if ( pMarkSprite )
				{
					SetRect( &rcMark, 0, 0, 18, 12 ) ;
					pMarkSprite->SetPosition( xx + lX + iHalfSize - 18, yy + lY - 29 ) ;

					// Mark Rect Clipping
					if ( lX + iHalfSize >= 512 )
					{
						rcMark.right -= (lX + iHalfSize) - 512 ;
					}
					if ( lY - 28 + 12 >= 512 )
					{
						rcMark.bottom -= (lY - 28 + 12) - 512 ;
					}

					if ( lX + iHalfSize - 18 < 0 )
					{
						rcMark.left -= (lX + iHalfSize - 18) ;
						pMarkSprite->m_nPutX = xx ;
					}
					if ( lY - 28 < 0 )
					{
						rcMark.top -= (lY - 28) ;
						pMarkSprite->m_nPutY = yy ;
					}

					if ( rcMark.left >= 0 && rcMark.top >= 0 &&
						rcMark.right <= 512 && rcMark.bottom <= 512 )
					{
						// 마크 출력
						pMarkSprite->Render( lpD3DDevice, 0xFF, &rcMark ) ;
					}
				}
			}
		}

		++ iter ;
	}
}
