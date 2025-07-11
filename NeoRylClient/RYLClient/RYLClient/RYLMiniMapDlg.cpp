
#include <winsock2.h>

#include <Creature/EnemyCheck.h>
#include <Creature/Monster/MonsterMgr.h>

#include <RenderOption.h>

#include "BaseDataDefine.h"
#include "BaseGraphicsLayer.h"
#include "WinInput.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"

#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLCastleManager.h"
#include "RYLCastle.h"

#include <Network/Packet/PacketStruct/ServerInfo.h>

#include "RYLMiniMapDlg.h"
#include "GMMemory.h"

CRYLMIniMapDlg*	g_pMiniMapDlg = NULL ;

CRYLMIniMapDlg::CRYLMIniMapDlg(INT nLeft, INT nTop, INT iType) : CRYLDialog()
{
	m_pHideButton		= NULL ;		// Hide 버튼
	m_pPositionLabel	= NULL ;		// 마을 이름
	m_pZoomInButton		= NULL ;		// 줌인
	m_pZoomOutButton	= NULL ;		// 줌아웃
	m_pSizeButton		= NULL ;
	m_bZoomClick		= FALSE ;

	miMobGep = 4;
	miViewType = 2;
	miWIDTH = 140;
	miHEIGHT = 164;
	miAreaX = 66;
	miAreaY = 78;
	miSize = 200;
	miGep = 6;

	RECT rt ;
//	SetRect( &rt, 0, 0, 90, 116 ) ;	// W 90, H 116
	SetRect( &rt, 0, 0, miWIDTH, miHEIGHT ) ;
	AttachWindowTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rt, ERYLSPRITENAME::RYL_MINIGUI_256x256 ) ) ;
	
	SetWidth( miWIDTH ) ;
	SetHeight( miHEIGHT ) ;

	SetLeft( nLeft ) ;
	SetTop( nTop ) ;

	SetFormType( RYLDLG_TYPE_NONE ) ;
//	SetDlgMoving( FALSE ) ;
	
	g_pMiniMapDlg = this ;	

	miViewType = iType;
	InitCtrl() ;
}

CRYLMIniMapDlg::~CRYLMIniMapDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLMIniMapDlg::InitCtrl ()
{
	RECT rt ;

	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;

	// Hide 버튼
	m_pHideButton = new CRYLButton( this ) ;
	m_pHideButton->SetLeft( 127 ) ;
	m_pHideButton->SetTop( 3 ) ;
	m_pHideButton->OnClickEvent = DlgClick ;
	m_pHideButton->SetInstance( RYLMAINMAPDLG::HIDE_BUTTON ) ;
	SetRect( &rt, 202, 0, 213, 9 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 202, 18, 213, 27 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 202, 9, 213, 18 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 202, 9, 213, 18 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pHideButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1868], 1868 ) ;
	AttachGMUIControl( m_pHideButton ) ;

	// 줌인 버튼
	m_pZoomInButton = new CRYLButton( this ) ;
	m_pZoomInButton->SetLeft( 123 ) ;
	m_pZoomInButton->SetTop( 132 ) ;
	m_pZoomInButton->OnClickEvent = DlgClick ;
	m_pZoomInButton->OnMouseDownEvent = DlgMouseDown ;
	m_pZoomInButton->SetInstance( RYLMAINMAPDLG::ZOOMIN_BUTTON ) ;
	SetRect( &rt, 222, 0, 236, 13 ) ;
	m_pZoomInButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 222, 26, 236, 39 ) ;
	m_pZoomInButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 222, 13, 236, 26 ) ;
	m_pZoomInButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 222, 0, 236, 13 ) ;
	m_pZoomInButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pZoomInButton ) ;
	m_pZoomInButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1781 ], 1781 ) ;

	// 줌아웃 버튼
	m_pZoomOutButton = new CRYLButton( this ) ;
	m_pZoomOutButton->SetLeft( 123 ) ;
	m_pZoomOutButton->SetTop( 146 ) ;
	m_pZoomOutButton->OnClickEvent = DlgClick ;
	m_pZoomOutButton->OnMouseDownEvent = DlgMouseDown ;
	m_pZoomOutButton->SetInstance( RYLMAINMAPDLG::ZOOMOUT_BUTTON ) ;
	SetRect( &rt, 236, 0, 250, 13 ) ;
	m_pZoomOutButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 236, 13, 250, 26 ) ;
	m_pZoomOutButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 236, 26, 250, 39 ) ;
	m_pZoomOutButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 236, 0, 250, 13 ) ;
	m_pZoomOutButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pZoomOutButton ) ;
	m_pZoomOutButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1782 ], 1782 ) ;

	
	m_pPositionLabel = new CRYLLabel( this ) ;
	m_pPositionLabel->SetLeft( 8 ) ;
	m_pPositionLabel->SetTop( 148 ) ;
	m_pPositionLabel->SetFontColor (0xffffffff);
	m_pPositionLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pPositionLabel ) ;


	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	// Size
	m_pSizeButton = new CRYLButton( this ) ;
	m_pSizeButton->SetLeft( 116 ) ;
	m_pSizeButton->SetTop( 3 ) ;
	m_pSizeButton->OnClickEvent = DlgClick ;
	m_pSizeButton->SetInstance( RYLMAINMAPDLG::SIZE_BUTTON ) ;
	SetRect( &rt, 246, 215, 256, 224 ) ;
	m_pSizeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 246, 234, 256, 241 ) ;
	m_pSizeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 246, 224, 256, 234 ) ;
	m_pSizeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 246, 215, 256, 224 ) ;
	m_pSizeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pSizeButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1869], 1869 ) ;
	AttachGMUIControl( m_pSizeButton ) ;


	m_pMapTexture = new CTexture;
	CTexture::SetPath( INTERFACETEXTUREPATH ) ;
	m_pMapTexture->Load("mmap01.dds");

	CRYLSprite *lpSprite;
	lpSprite = new CRYLSprite;
	lpSprite->Create(GetLeft(), GetTop(), 123, 0, 206, 104, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;
	lpSprite->Create(GetLeft() + 83, GetTop(), 105, 0, 121, 104, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;
	lpSprite->Create(GetLeft() + 38, GetTop() + 102, 40, 156, 78, 183, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;
	lpSprite->Create(GetLeft(), GetTop(), 221, 104, 238, 171, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 자신 표시 : 4
	lpSprite->Create(GetLeft() + 43 - 5, GetTop() + 43 - 7, 188, 136, 199, 152, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// PC 표시 : 5
	lpSprite->Create(GetLeft(), GetTop(), 132, 156, 139, 163, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// NPC 표시 : 6
	lpSprite->Create(GetLeft(), GetTop(), 121, 156, 128, 163, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// MONSTER 표시 : 7
	lpSprite->Create(GetLeft(), GetTop(), 143, 156, 150, 163, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 파티원 표시 : 8
	lpSprite->Create(GetLeft(), GetTop(), 154, 156, 161, 163, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 퀘스트 표시 : 9
	lpSprite->Create(GetLeft(), GetTop(), 121, 135, 139, 152, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 퀘스트 표시 : 10
	lpSprite->Create(GetLeft(), GetTop(), 143, 135, 161, 152, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 퀘스트 표시 : 11
	lpSprite->Create(GetLeft(), GetTop(), 165, 135, 183, 152, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 듀얼 상대 표시 : 12
	lpSprite->Create(GetLeft(), GetTop(), 165, 156, 172, 163, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 무기상인 : 13
	lpSprite->Create(GetLeft(), GetTop(), 121, 165, 133, 177, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 방어구상인 : 14
	lpSprite->Create(GetLeft(), GetTop(), 133, 165, 145, 177, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 잡화상인 : 15
	lpSprite->Create(GetLeft(), GetTop(), 145, 165, 157, 177, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 트레이너 : 16
	lpSprite->Create(GetLeft(), GetTop(), 157, 165, 169, 177, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 순간이동술사 : 17
	lpSprite->Create(GetLeft(), GetTop(), 169, 165, 181, 177, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 대장장이 : 18
	lpSprite->Create(GetLeft(), GetTop(), 158, 120, 170, 132, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 창고 : 19
	lpSprite->Create(GetLeft(), GetTop(), 170, 120, 182, 132, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 아카데미 : 20
	lpSprite->Create(GetLeft(), GetTop(), 182, 120, 194, 132, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 무기암시장 : 21
	lpSprite->Create(GetLeft(), GetTop(), 194, 120, 206, 132, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 방어구암시장 : 22
	lpSprite->Create(GetLeft(), GetTop(), 206, 120, 218, 132, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 적 종족 표시 : 23
	lpSprite->Create(GetLeft(), GetTop(), 174, 157, 179, 162, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 반전단체 : 24
	lpSprite->Create(GetLeft(), GetTop(), 206, 132, 218, 144, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 자신 표시 : 25
	lpSprite->Create(GetLeft() + 43 - 5, GetTop() + 43 - 7, 188, 136, 199, 152, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 자신 표시 : 26
	lpSprite->Create(GetLeft(), GetTop(), 206, 90, 218, 102, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 퀘스트 타겟 표시 : 27
	lpSprite->Create(GetLeft(), GetTop(), 119, 135, 140, 154, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 길드관리자 표시 : 28
	lpSprite->Create(GetLeft(), GetTop(), 218, 90, 230, 102, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);


	lpSprite = new CRYLSprite;		// 퀘스트 존재 표시 : 29
	lpSprite->Create(GetLeft(), GetTop(), 97, 165, 109, 177, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;		// 퀘스트 완료 표시 : 30
	lpSprite->Create(GetLeft(), GetTop(), 109, 165, 121, 177, m_pMapTexture);
	m_lstSprite.push_back(lpSprite);

	m_MiniMapSize	= 256 ;
	m_bFullSize		= TRUE ;
	m_lpQuestTarget = NULL ;

	// 기본으로 중간 사이즈로 지정한다
	ProcessDlgSize(miViewType);
}

VOID	CRYLMIniMapDlg::FinalCtrl ()
{
	GM_DELETE( m_pHideButton ) ;
	GM_DELETE( m_pPositionLabel ) ;		
	GM_DELETE( m_pZoomInButton ) ;			// 줌인
	GM_DELETE( m_pZoomOutButton ) ;			// 줌아웃
	GM_DELETE( m_pMapTexture ) ;

	GM_DELETE( m_pSizeButton );

	MiniMapDelete() ;
	ClearSprite() ;
}

VOID	CRYLMIniMapDlg::Show( CGMUIObject* Sender )
{
	
}
	
VOID	CRYLMIniMapDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID CRYLMIniMapDlg::ClearSprite()
{
	std::vector<CRYLSprite *>::iterator itr = m_lstSprite.begin() ;
	while ( itr != m_lstSprite.end() )
	{
		GM_DELETE( (*itr) ) ;
		++ itr ;
	}
	m_lstSprite.clear() ;
}

VOID CRYLMIniMapDlg::MiniMapDelete(void)
{

	int i;
		
	for(  i = 0; i < 9 ; i++ )
	{
	
		if (m_MiniMapTexture[i].GetTexture())
			m_MiniMapTexture[i].Unload();
	}
}

VOID	CRYLMIniMapDlg::GMUIPaint( INT xx, INT yy )
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	
	lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) ;
	lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;
    lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ) ;
	if (!m_bFullSize)
	{
		m_lstSprite[3]->Render(lpD3DDevice);
		return ;
	}

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if( !pSelfCreature )
        return;

	int nChrPosX = (pSelfCreature->GetPosition()->x / SECTORSIZE);
	int nChrPosZ = (pSelfCreature->GetPosition()->z / SECTORSIZE);
	int nMoveX = ((pSelfCreature->GetPosition()->x - (nChrPosX * SECTORSIZE)) / SECTORSIZE) * m_MiniMapSize ;
	int nMoveZ = ((pSelfCreature->GetPosition()->z - (nChrPosZ * SECTORSIZE)) / SECTORSIZE) * m_MiniMapSize ;


	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	// -------------------------------------------------------------------------------------------------
	// 지도 텍스쳐 부르기

	char strMiniMapFileName[MAX_PATH];

	sprintf(strMiniMapFileName, "%d_%d", nChrPosX, nChrPosZ);
	if ( (strstr(m_MiniMapTexture[4].m_strName, strMiniMapFileName) == NULL) ) 		ReloadTexture();		

	// -------------------------------------------------------------------------------------------------

	TLVertex2 Vertex[4];
	Vertex[0].Diffuse.c  = Vertex[1].Diffuse.c  = Vertex[2].Diffuse.c  = Vertex[3].Diffuse.c  = 0xffffffff;
	Vertex[0].Specular.c = Vertex[1].Specular.c = Vertex[2].Specular.c = Vertex[3].Specular.c = 0;
	Vertex[0].w          = Vertex[1].w          = Vertex[2].w          = Vertex[3].w          = 0.1f;
	Vertex[0].v.z        = Vertex[1].v.z        = Vertex[2].v.z        = Vertex[3].v.z        = 0.1f;				

	float fCenterX = Vertex[0].v.x = Vertex[1].v.x = 0 - nMoveX + miAreaX ;
	Vertex[2].v.x  = Vertex[3].v.x = m_MiniMapSize - nMoveX + miAreaX ;
	float fCenterZ = Vertex[0].v.y = Vertex[2].v.y = 0 + nMoveZ - m_MiniMapSize + miAreaY + miGep ;
	Vertex[1].v.y  = Vertex[3].v.y = m_MiniMapSize + nMoveZ - m_MiniMapSize + miAreaY + miGep;

	Vertex[0].tu1 = Vertex[1].tu1 = 0.0f + 0.5f / (float)m_MiniMapSize ;
	Vertex[2].tu1 = Vertex[3].tu1 = 1.0f - 0.5f / (float)m_MiniMapSize ;
	Vertex[0].tv1 = Vertex[2].tv1 = 1.0f - 0.5f / (float)m_MiniMapSize ;
	Vertex[1].tv1 = Vertex[3].tv1 = 0.0f + 0.5f / (float)m_MiniMapSize ;


	for(int i = 0; i < 4; i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize ;
		Vertex[i].tv = Vertex[i].v.y / (float)miSize ;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}

	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	lpD3DDevice->SetTexture(0, pGame->m_lpInterface->m_lstTexture[4]->GetTexture());
	lpD3DDevice->SetTexture(1, m_MiniMapTexture[4].GetTexture());

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEX2FVF);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

	lpD3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	lpD3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	
	lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
	lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertex, sizeof(TLVertex2));			



	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);	

	// 0
	Vertex[0].v.x=Vertex[1].v.x=fCenterX-m_MiniMapSize;
	Vertex[2].v.x=Vertex[3].v.x=fCenterX;
	Vertex[0].v.y=Vertex[2].v.y=fCenterZ-m_MiniMapSize;
	Vertex[1].v.y=Vertex[3].v.y=fCenterZ;

	for(int i=0;i<4;i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize;
		Vertex[i].tv = Vertex[i].v.y / (float)miSize;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}
	lpD3DDevice->SetTexture(1, m_MiniMapTexture[0].GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertex, sizeof(TLVertex2));

	// 1
	Vertex[0].v.x=Vertex[1].v.x=fCenterX;
	Vertex[2].v.x=Vertex[3].v.x=fCenterX+m_MiniMapSize;
	Vertex[0].v.y=Vertex[2].v.y=fCenterZ-m_MiniMapSize;
	Vertex[1].v.y=Vertex[3].v.y=fCenterZ;	

	for(int i=0;i<4;i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize;
		Vertex[i].tv = Vertex[i].v.y / (float)miSize;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}
	lpD3DDevice->SetTexture(1,m_MiniMapTexture[1].GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(TLVertex2));

	// 2
	Vertex[0].v.x=Vertex[1].v.x=fCenterX+m_MiniMapSize;
	Vertex[2].v.x=Vertex[3].v.x=fCenterX+m_MiniMapSize*2;
	Vertex[0].v.y=Vertex[2].v.y=fCenterZ-m_MiniMapSize;
	Vertex[1].v.y=Vertex[3].v.y=fCenterZ;	

	for(int i=0;i<4;i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize;
		Vertex[i].tv = Vertex[i].v.y / (float)miSize;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}
	lpD3DDevice->SetTexture(1,m_MiniMapTexture[2].GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(TLVertex2));

	// 3
	Vertex[0].v.x=Vertex[1].v.x=fCenterX-m_MiniMapSize;
	Vertex[2].v.x=Vertex[3].v.x=fCenterX;
	Vertex[0].v.y=Vertex[2].v.y=fCenterZ;
	Vertex[1].v.y=Vertex[3].v.y=fCenterZ+m_MiniMapSize;	

	for(int i=0;i<4;i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize;
		Vertex[i].tv = Vertex[i].v.y / (float)miSize;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}
	lpD3DDevice->SetTexture(1,m_MiniMapTexture[3].GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(TLVertex2));

	// 5
	Vertex[0].v.x=Vertex[1].v.x=fCenterX+m_MiniMapSize;
	Vertex[2].v.x=Vertex[3].v.x=fCenterX+m_MiniMapSize*2;
	Vertex[0].v.y=Vertex[2].v.y=fCenterZ;
	Vertex[1].v.y=Vertex[3].v.y=fCenterZ+m_MiniMapSize;	

	for(int i=0;i<4;i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize;
		Vertex[i].tv = Vertex[i].v.y / (float)miSize;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}
	lpD3DDevice->SetTexture(1,m_MiniMapTexture[5].GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(TLVertex2));

	// 6
	Vertex[0].v.x=Vertex[1].v.x=fCenterX-m_MiniMapSize;
	Vertex[2].v.x=Vertex[3].v.x=fCenterX;
	Vertex[0].v.y=Vertex[2].v.y=fCenterZ+m_MiniMapSize;
	Vertex[1].v.y=Vertex[3].v.y=fCenterZ+m_MiniMapSize*2;	

	for(int i=0;i<4;i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize;
		Vertex[i].tv = Vertex[i].v.y / (float)miSize;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}
	lpD3DDevice->SetTexture(1,m_MiniMapTexture[6].GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(TLVertex2));

	// 7
	Vertex[0].v.x=Vertex[1].v.x=fCenterX;
	Vertex[2].v.x=Vertex[3].v.x=fCenterX+m_MiniMapSize;
	Vertex[0].v.y=Vertex[2].v.y=fCenterZ+m_MiniMapSize;
	Vertex[1].v.y=Vertex[3].v.y=fCenterZ+m_MiniMapSize*2;	

	for(int i=0;i<4;i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize; 
		Vertex[i].tv = Vertex[i].v.y / (float)miSize;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}
	lpD3DDevice->SetTexture(1,m_MiniMapTexture[7].GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(TLVertex2));


	// 8
	Vertex[0].v.x=Vertex[1].v.x=fCenterX+m_MiniMapSize;
	Vertex[2].v.x=Vertex[3].v.x=fCenterX+m_MiniMapSize*2;
	Vertex[0].v.y=Vertex[2].v.y=fCenterZ+m_MiniMapSize;
	Vertex[1].v.y=Vertex[3].v.y=fCenterZ+m_MiniMapSize*2;

	for(int i=0;i<4;i++)
	{
		Vertex[i].tu = Vertex[i].v.x / (float)miSize;
		Vertex[i].tv = Vertex[i].v.y / (float)miSize;

		Vertex[i].v.x += GetLeft();
		Vertex[i].v.y += (GetTop()-miGep);
	}
	lpD3DDevice->SetTexture(1,m_MiniMapTexture[8].GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(TLVertex2));


	// 스테이스 초기화
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);

	// -------------------------------------------------------------------------------------------------

	int CenterX,CenterY;
	CenterX = GetLeft()+GetWidth()/2;
	CenterY = GetTop()+GetHeight()/2;
	float PixelPerMeter = ((float)m_MiniMapSize/SECTORSIZE);

    vector3 vecPos, vecSelfPos = *( pSelfCreature->GetPosition() );
    
	RECT rcMapSize;

	if(miViewType == 2)
		SetRect( &rcMapSize, GetLeft() + 1, GetTop() + 5, GetLeft() + 1+138, GetTop() + 5+138);
	else if(miViewType == 1)
		SetRect( &rcMapSize, GetLeft() + 1, GetTop() + 5, GetLeft() + 1+112, GetTop() + 5+112);
	else	
		SetRect( &rcMapSize, GetLeft() + 1, GetTop() + 5, GetLeft() + 1+86, GetTop() + 5+86);

	vecSelfPos.y = 0.0f;

	int nPosX, nPosY;
	RECT rc = { 0, 0, 0, 0 } ;

    vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

	struct NPCQUESTICON
	{
		int Type;
		int nPosX;
		int nPosY;
	};

	int iNpcQuestIcon = 0;
	static NPCQUESTICON npcQuestIcon[32];

	
	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for(; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if( NULL == pTempCreature )
            continue;

        if( pTempCreature == pSelfCreature )
            continue;

        if( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) )
            continue;

        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
        if( !pTempData )
            continue;

		vecPos = *( pTempCreature->GetPosition() );
		vecPos.y = 0.0f;
		vecPos = vecPos - vecSelfPos;
		vecPos.x *= PixelPerMeter;
		vecPos.z *= PixelPerMeter;

		nPosX = CenterX + vecPos.x - miMobGep;
		nPosY = CenterY - vecPos.z - miMobGep;

		switch (pGame->GetClientIdentity( pTempCreature->GetCharID() ) )
		{
			case Creature::CT_NPC:
			{
				if( pGame->GetClientEnemyType( pTempCreature->GetCharID() ) == EnemyCheck::EC_ENEMY )
					break;

                NPCNode* lpNode = g_NPCList.GetNPCNode( pTempCreature->GetCharID() );
				if (lpNode)
				{   
					int IsQuestType = 0;	// 0이면 일반아이콘, 1이면 수행할 퀘스트 있음, 2이면 완료할 퀘스트 존재

					// 수행할 퀘스트가 있는지 검사
					if(lpNode->m_lstQuest.size())
					{
						//  렌홀드 성안에 NPC는 수성측에서만 사용할 수 있다.
						bool bTownIDCheck = false;

						if(lpNode->m_dwTownOrNameID==0x70000001)
						{					
							CRYLCastle*	lpCastle = CRYLCastleManager::Instance()->GetCurrentCastle();

							if(lpCastle)
							{
								if(lpCastle->GetNation()!=CRYLGameData::Instance()->m_cAccountNation )
								{													
									bTownIDCheck = true;
								}
							}											
						}

						if(!bTownIDCheck)
						{
							unsigned short aryComplete[1000] = { 0, };
							unsigned short aryQuest[20] = { 0, };
							unsigned long iNPC = 0;

							list<LPQuestInstance>::iterator it;

							for(it = pGame->m_csStatus.m_lstCompleteQuest.begin(); 
								it != pGame->m_csStatus.m_lstCompleteQuest.end(); ++it, ++iNPC)
							{
								aryComplete[ iNPC ] = (*it)->m_dwQuestID;
							}

							iNPC = 0; 
							for(it = pGame->m_csStatus.m_lstQuest.begin(); 
								it != pGame->m_csStatus.m_lstQuest.end(); ++it, ++iNPC)
							{
								aryQuest[ iNPC ] = (*it)->m_dwQuestID;
							}

							const CHAR_INFOST* pInfo = &pGame->m_csStatus.m_Info;
							size_t dwCompleteQuestSize = pGame->m_csStatus.m_lstCompleteQuest.size();

							lpNode->SearchQuestList(pInfo->Level, pInfo->Fame, pInfo->Class, 
								pGame->m_cAccountNation, aryComplete, dwCompleteQuestSize, aryQuest);

							// 하나라도 수행가능한 퀘스트가 있다면..
							if (aryQuest[0] != 0xFFFF)		
							{   
								IsQuestType = 1;
							}
						}
					}
					// 현재 수행중인 퀘스트 중에서 TriggerTalk와 TriggerLevelTalk, TriggerFameTalk의 NPC이면 녹색으로 출력
					bool bPass = false;
					if (!pGame->m_csStatus.m_lstQuest.empty())
					{
						unsigned long dwTriggerCount; 
						list<LPQuestInstance>::iterator it;

						for (it = pGame->m_csStatus.m_lstQuest.begin(); it != pGame->m_csStatus.m_lstQuest.end(); it++)
						{
							LPQuestInstance pInfo = (*it);
							if(pInfo->m_lpPhaseScript == NULL)
								continue;

							for (dwTriggerCount = 0; dwTriggerCount < pInfo->m_lpPhaseScript->m_lstTrigger.size(); dwTriggerCount++)
							{
								if (pInfo->m_lstTriggerCount[dwTriggerCount] > 0)
								{
									if (pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerKind == TRIGGER_TALK ||
										pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerKind == TRIGGER_FAME_TALK ||
										pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerKind == TRIGGER_LEVEL_TALK ||
										pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerKind == TRIGGER_ABILITY_TALK)
									{
										unsigned long dwTempValue = pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerID;
										if (pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerID == pTempCreature->GetCharID())
										{
											bPass = true;

											IsQuestType = 2;
											break;
										}
									}	
								}
							}
							if (bPass)
							{
								break;
							}
						}
					}

					rc.right = m_lstSprite[ lpNode->m_unMiniMapIconID ]->m_rcRender.right - m_lstSprite[ lpNode->m_unMiniMapIconID ]->m_rcRender.left ;
					rc.bottom = m_lstSprite[ lpNode->m_unMiniMapIconID ]->m_rcRender.bottom - m_lstSprite[ lpNode->m_unMiniMapIconID ]->m_rcRender.top ;
				
					if ( ClipRect( rcMapSize, nPosX - 3, nPosY - 3, rc ) )
					{
						if(IsQuestType == 0)
						{
							m_lstSprite[ lpNode->m_unMiniMapIconID ]->SetPosition( nPosX - 3, nPosY - 3 ) ;
							m_lstSprite[ lpNode->m_unMiniMapIconID ]->Render(lpD3DDevice, 0xFF, &rc ) ;
						}
						else if(iNpcQuestIcon < 32)
						{
							npcQuestIcon[iNpcQuestIcon].Type = IsQuestType;
							npcQuestIcon[iNpcQuestIcon].nPosX = nPosX - 6;
							npcQuestIcon[iNpcQuestIcon].nPosY = nPosY - 6;
							iNpcQuestIcon++;
						}

					}
				} 
				else
				{
					rc.right = m_lstSprite[6]->m_rcRender.right - m_lstSprite[6]->m_rcRender.left ;
					rc.bottom = m_lstSprite[6]->m_rcRender.bottom - m_lstSprite[6]->m_rcRender.top ;
					
					if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
					{
						m_lstSprite[6]->SetPosition( nPosX, nPosY ) ;
						m_lstSprite[6]->Render(lpD3DDevice, 0xFF, &rc ) ;
					}
				}
			}
			break;

			case Creature::CT_MONSTER:
            {
				if ( pGame->GetClientEnemyType( pTempCreature->GetCharID() ) != EnemyCheck::EC_ENEMY )
				{
					rc.right = m_lstSprite[6]->m_rcRender.right - m_lstSprite[6]->m_rcRender.left ;
					rc.bottom = m_lstSprite[6]->m_rcRender.bottom - m_lstSprite[6]->m_rcRender.top ;

					if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
					{
						m_lstSprite[6]->SetPosition( nPosX, nPosY ) ;
						m_lstSprite[6]->Render(lpD3DDevice, 0xFF, &rc ) ;
					}
				}
                else if ( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stealth ) )
                {
					// 경비병 적은 적 PC의 색상으로 출력
					const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( pTempCreature->GetCharID() & 0x0000FFFF );
					if (NULL != lpMonster && MonsterInfo::PATTERN_GUARD == lpMonster->m_MonsterInfo.m_cSkillPattern)
					{
						rc.right = m_lstSprite[23]->m_rcRender.right - m_lstSprite[23]->m_rcRender.left ;
						rc.bottom = m_lstSprite[23]->m_rcRender.bottom - m_lstSprite[23]->m_rcRender.top ;

						if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
						{
							m_lstSprite[23]->SetPosition( nPosX, nPosY ) ;
							m_lstSprite[23]->Render(lpD3DDevice, 0xFF, &rc ) ;
						}
					}
					else if (NULL != lpMonster && MonsterInfo::PATTERN_GATHER == lpMonster->m_MonsterInfo.m_cSkillPattern)
					{
						unsigned long MonID = lpMonster->m_MonsterInfo.m_dwKID;
						int	abValue = pGame->m_csStatus.GetAbilityValue(Skill::Type::AB_GATHER_FIND);
						bool bShow = false;
						if(abValue >= 3) // 모두 보여준다.
							bShow = true;
						else if( abValue >= 2 && (3101 <= MonID && MonID <= 3300 ))		// 중급까지 보여준다.
							bShow = true;
						else if( abValue >= 1 && (3101 <= MonID && MonID <= 3250 ))		// 하급만 보여준다.
							bShow = true;


						// 플레이어가 어빌리티가 있을때 표시
						if(bShow)
						{
							rc.right = m_lstSprite[7]->m_rcRender.right - m_lstSprite[7]->m_rcRender.left ;
							rc.bottom = m_lstSprite[7]->m_rcRender.bottom - m_lstSprite[7]->m_rcRender.top ;

							if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
							{
								m_lstSprite[7]->SetPosition( nPosX, nPosY ) ;
								m_lstSprite[7]->Render(lpD3DDevice, 0xFF, &rc ) ;
							}
						}
					}
					else
					{
						rc.right = m_lstSprite[7]->m_rcRender.right - m_lstSprite[7]->m_rcRender.left ;
						rc.bottom = m_lstSprite[7]->m_rcRender.bottom - m_lstSprite[7]->m_rcRender.top ;

						if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
						{
							m_lstSprite[7]->SetPosition( nPosX, nPosY ) ;
							m_lstSprite[7]->Render(lpD3DDevice, 0xFF, &rc ) ;
						}
						
					}
                }
			}
			break;

			case Creature::CT_PC:
            {
				// 파티일때 파티로 뿌리기
				if ( pGame->m_csStatus.CheckPartyMember( pTempCreature->GetCharID() ) )
                {
					rc.right = m_lstSprite[8]->m_rcRender.right - m_lstSprite[8]->m_rcRender.left ;
					rc.bottom = m_lstSprite[8]->m_rcRender.bottom - m_lstSprite[8]->m_rcRender.top ;
					
					if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
					{
						m_lstSprite[8]->SetPosition( nPosX, nPosY ) ;
						m_lstSprite[8]->Render(lpD3DDevice, 0xFF, &rc ) ;
					}
                }
				else if ( pGame->GetClientEnemyType( pTempCreature->GetCharID() ) != EnemyCheck::EC_ENEMY )
				{
					rc.right = m_lstSprite[5]->m_rcRender.right - m_lstSprite[5]->m_rcRender.left ;
					rc.bottom = m_lstSprite[5]->m_rcRender.bottom - m_lstSprite[5]->m_rcRender.top ;
					
					if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
					{
						m_lstSprite[5]->SetPosition( nPosX, nPosY ) ;
						m_lstSprite[5]->Render(lpD3DDevice, 0xFF, &rc ) ;
					}
				}
				else
                {
					if ( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stealth ) )
					{
						if ( g_CharacterData.GetIsDuelTarget( pTempCreature->GetCharID() ) )
						{
							rc.right = m_lstSprite[12]->m_rcRender.right - m_lstSprite[12]->m_rcRender.left ;
							rc.bottom = m_lstSprite[12]->m_rcRender.bottom - m_lstSprite[12]->m_rcRender.top ;
							
							if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
							{
								m_lstSprite[12]->SetPosition( nPosX, nPosY ) ;
								m_lstSprite[12]->Render(lpD3DDevice, 0xFF, &rc ) ;
							}
						}
						else
						{
							rc.right = m_lstSprite[23]->m_rcRender.right - m_lstSprite[23]->m_rcRender.left ;
							rc.bottom = m_lstSprite[23]->m_rcRender.bottom - m_lstSprite[23]->m_rcRender.top ;
							
							if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
							{
								m_lstSprite[23]->SetPosition( nPosX, nPosY ) ;
								m_lstSprite[23]->Render(lpD3DDevice, 0xFF, &rc ) ;
							}
						}
					}
                }
            }
			break;

            case Creature::CT_SUMMON:
            {
                if ( pTempCreature->GetCharID() == pGame->m_csStatus.m_dwSummonID )
                {
					rc.right = m_lstSprite[8]->m_rcRender.right - m_lstSprite[8]->m_rcRender.left ;
					rc.bottom = m_lstSprite[8]->m_rcRender.bottom - m_lstSprite[8]->m_rcRender.top ;
					
					if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
					{
						m_lstSprite[8]->SetPosition( nPosX, nPosY ) ;
						m_lstSprite[8]->Render(lpD3DDevice, 0xFF, &rc ) ;
					}
                }
				else 
				{
                    if (pGame->m_csStatus.CheckPartyMember( pTempData->m_ulSummonMasterCharID ) )
                    {
						rc.right = m_lstSprite[8]->m_rcRender.right - m_lstSprite[8]->m_rcRender.left ;
						rc.bottom = m_lstSprite[8]->m_rcRender.bottom - m_lstSprite[8]->m_rcRender.top ;
						
						if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
						{
							m_lstSprite[8]->SetPosition( nPosX, nPosY ) ;
							m_lstSprite[8]->Render(lpD3DDevice, 0xFF, &rc ) ;
						}
                    }
					else
                    {
						rc.right = m_lstSprite[5]->m_rcRender.right - m_lstSprite[5]->m_rcRender.left ;
						rc.bottom = m_lstSprite[5]->m_rcRender.bottom - m_lstSprite[5]->m_rcRender.top ;
						
						if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
						{
							m_lstSprite[5]->SetPosition( nPosX, nPosY ) ;
							m_lstSprite[5]->Render(lpD3DDevice, 0xFF, &rc ) ;
						}
                    }
				}
            }
			break;

/*
			case Creature::CT_SIEGE_OBJECT:
				// 공성 오브젝트
				m_lstSprite[5]->Render(lpD3DDevice, 0xFF, nPosX, nPosY);
				break;
*/

			case Creature::CT_STRUCT:
            {
				if( pGame->GetClientEnemyType( pTempCreature->GetCharID() ) == EnemyCheck::EC_FRIEND )
                {
					rc.right = m_lstSprite[5]->m_rcRender.right - m_lstSprite[5]->m_rcRender.left ;
					rc.bottom = m_lstSprite[5]->m_rcRender.bottom - m_lstSprite[5]->m_rcRender.top ;
					
					if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
					{
						m_lstSprite[5]->SetPosition( nPosX, nPosY ) ;
						m_lstSprite[5]->Render(lpD3DDevice, 0xFF, &rc ) ;
					}
                }
				else
				{
					if( !pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stealth ) )
                    {
						rc.right = m_lstSprite[7]->m_rcRender.right - m_lstSprite[7]->m_rcRender.left ;
						rc.bottom = m_lstSprite[7]->m_rcRender.bottom - m_lstSprite[7]->m_rcRender.top ;
						
						if ( ClipRect( rcMapSize, nPosX, nPosY, rc ) )
						{
							m_lstSprite[7]->SetPosition( nPosX, nPosY ) ;
							m_lstSprite[7]->Render(lpD3DDevice, 0xFF, &rc ) ;
						}
                    }
				}
            }
			break;
		}
	}   


	rc.left = 0;
	rc.top = 0;
	rc.right = 12;
	rc.bottom = 12;
	for(int i = 0; i < iNpcQuestIcon; ++i)
	{
		NPCQUESTICON& icon = npcQuestIcon[i];

		m_lstSprite[ 28+icon.Type ]->SetPosition( icon.nPosX, icon.nPosY ) ;
		m_lstSprite[ 28+icon.Type ]->Render(lpD3DDevice, 0xFF, &rc ) ;
	}



	m_lstSprite[4]->m_nPutX = CenterX - 2 ;
	m_lstSprite[4]->m_nPutY = CenterY - 2 ;
	m_lstSprite[4]->Render(lpD3DDevice, pSelfCreature->GetDirection() + FLOAT_PHI);

	CHAR szBuff[ 50 ] ;
	sprintf( szBuff, "%d, %d", (long)(vecSelfPos.x / 100.0f), (long)(vecSelfPos.z / 100.0f));
	m_pPositionLabel->SetCaption( szBuff ) ;

	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLMIniMapDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )	
{
	return S_OK ;
}

HRESULT		CRYLMIniMapDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	if (g_DeviceInput.GetIsLeftMousePress())
	{
		// Zoom In
		if(miViewType == 2)
		{
			if ( g_DeviceInput.InRect( GetLeft()+123, GetTop()+132, GetLeft()+123+13, GetTop()+132+13 ) )
			{
				ZoomIn() ;
				m_bZoomClick = TRUE ; 
			}
		}
		else if(miViewType == 1)
		{
			if ( g_DeviceInput.InRect( GetLeft()+97, GetTop()+106, GetLeft()+97+13, GetTop()+106+13 ) )
			{
				ZoomIn() ;
				m_bZoomClick = TRUE ; 
			}
		}
		else if ( g_DeviceInput.InRect( GetLeft()+73, GetTop()+82, GetLeft()+73+13, GetTop()+82+13 ) )
		{
			ZoomIn() ;
			m_bZoomClick = TRUE ; 
		}


		// Zoom Out
		if(miViewType == 2)
		{
			if ( g_DeviceInput.InRect( GetLeft()+123, GetTop()+146, GetLeft()+123+13, GetTop()+146+13 ) )
			{
				ZoomOut() ;
				m_bZoomClick = TRUE ;
			}
		}
		else if(miViewType == 1)
		{
			if ( g_DeviceInput.InRect( GetLeft()+97, GetTop()+120, GetLeft()+97+13, GetTop()+120+13 ) )
			{
				ZoomOut() ;
				m_bZoomClick = TRUE ;
			}
		}
		else if ( g_DeviceInput.InRect( GetLeft()+73, GetTop()+95, GetLeft()+73+13, GetTop()+95+13 ) )
		{
			ZoomOut() ;
			m_bZoomClick = TRUE ;
		}
	}

	if (g_DeviceInput.GetIsLeftMouseUp() )
	{
/*
		// edith 2008.06.12 미니맵클릭했을때 라지맵 나오는거 방지
		if ( g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft()+90, GetTop()+116 ) && !m_bZoomClick )
		{
			( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetLargeMapDlg()->SetVisible( !( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetLargeMapDlg()->GetVisible() ) ;
		}
*/
		m_bZoomClick = FALSE ;
	}

	return S_OK ;
}

VOID	CRYLMIniMapDlg::ZoomIn() 
{
	m_MiniMapSize += 20;
	if (m_MiniMapSize > 512) 
	{
		m_MiniMapSize = 512;
	}
}

VOID	CRYLMIniMapDlg::ZoomOut()
{
	m_MiniMapSize -= 20;
	if (m_MiniMapSize < 128) 
	{
		m_MiniMapSize = 128;
	}
}

VOID	CRYLMIniMapDlg::ProcessDlgSize(int iType)
{
	RECT rtWindow ;

	if(iType == -1)
	{
		miViewType++;
		if(miViewType > 2)
			miViewType = 0;
	}
	else 
		miViewType = iType;

	if(miViewType == 1)
	{
		// 미니맵 중간꺼
		miViewType = 1;

		miWIDTH = 114;
		miHEIGHT = 138;
		miAreaX = 54;
		miAreaY = 66;
		miSize = 164;
		miGep = 4;

		SetRect( &rtWindow, 141, 0, 141+miWIDTH, miHEIGHT ) ;
		AttachWindowTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rtWindow, ERYLSPRITENAME::RYL_MINIGUI_256x256 ) ) ;

		SetWidth( miWIDTH ) ;
		SetHeight( miHEIGHT ) ;

		m_pHideButton->SetLeft( 101 ) ;
		m_pHideButton->SetTop( 3 ) ;
		m_pZoomInButton->SetLeft( 97 ) ;
		m_pZoomInButton->SetTop( 106 ) ;
		m_pZoomOutButton->SetLeft( 97 ) ;
		m_pZoomOutButton->SetTop( 120 ) ;
		m_pPositionLabel->SetLeft( 18 ) ;
		m_pPositionLabel->SetTop( 122 ) ;
		m_pSizeButton->SetLeft( 90 ) ;
		m_pSizeButton->SetTop( 3 ) ;
	}
	else if(miViewType == 2)
	{
		// 미니맵 큰거
		miViewType = 2;

		miWIDTH = 140;
		miHEIGHT = 164;
		miAreaX = 66;
		miAreaY = 78;
		miSize = 200;
		miGep = 6;

		SetRect( &rtWindow, 0, 0, miWIDTH, miHEIGHT ) ;
		AttachWindowTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rtWindow, ERYLSPRITENAME::RYL_MINIGUI_256x256 ) ) ;

		SetWidth( miWIDTH ) ;
		SetHeight( miHEIGHT ) ;

		m_pHideButton->SetLeft( 127 ) ;
		m_pHideButton->SetTop( 3 ) ;
		m_pZoomInButton->SetLeft( 123 ) ;
		m_pZoomInButton->SetTop( 132 ) ;
		m_pZoomOutButton->SetLeft( 123 ) ;
		m_pZoomOutButton->SetTop( 146 ) ;
		m_pPositionLabel->SetLeft( 32 ) ;
		m_pPositionLabel->SetTop( 148 ) ;
		m_pSizeButton->SetLeft( 116 ) ;
		m_pSizeButton->SetTop( 3 ) ;

	}
	else
	{
		// 미니맵 작은거
		miViewType = 0;

		miWIDTH = 90;
		miHEIGHT = 114;
		miAreaX = 42;
		miAreaY = 52;
		miSize = 128;
		miGep = 2;

		SetRect( &rtWindow, 163, 139, 163+miWIDTH, 139+miHEIGHT ) ;
		AttachWindowTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rtWindow, ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;

		SetWidth( miWIDTH ) ;
		SetHeight( miHEIGHT ) ;

		m_pHideButton->SetLeft( 77 ) ;
		m_pHideButton->SetTop( 3 ) ;
		m_pZoomInButton->SetLeft( 73 ) ;
		m_pZoomInButton->SetTop( 82 ) ;
		m_pZoomOutButton->SetLeft( 73 ) ;
		m_pZoomOutButton->SetTop( 96 ) ;
		m_pPositionLabel->SetLeft( 8 ) ;
		m_pPositionLabel->SetTop( 98 ) ;
		m_pSizeButton->SetLeft( 66 ) ;
		m_pSizeButton->SetTop( 3 ) ;
	}
}

VOID	CRYLMIniMapDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pMiniMapDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLMAINMAPDLG::SIZE_BUTTON :
			g_pMiniMapDlg->ProcessDlgSize() ;
			break ;

		case RYLMAINMAPDLG::HIDE_BUTTON :
			{
				g_pMiniMapDlg->SetVisible( FALSE ) ;

				// Visible Flag Setting
				CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::Instance()->GetCurrentScene() ) ;
				if ( pScene ) pScene->SetVisibleInfo( RYLMAINWINDOW::MINIMAP_WINDOW, FALSE ) ;
			}
			break ;
	}
}

VOID	CRYLMIniMapDlg::DlgMouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( !g_pMiniMapDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLMAINMAPDLG::ZOOMIN_BUTTON :
			//g_pMiniMapDlg->ZoomIn() ;
			break ;
		case RYLMAINMAPDLG::ZOOMOUT_BUTTON :
			//g_pMiniMapDlg->ZoomOut() ;
			break ;
	}
}

HRESULT		CRYLMIniMapDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID		CRYLMIniMapDlg::ReloadTexture(void)
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if( !pSelfCreature )
        return;

    int nChrPosX = (pSelfCreature->GetPosition()->x / SECTORSIZE);
	int nChrPosZ = (pSelfCreature->GetPosition()->z / SECTORSIZE);

	CTexture::SetPath(WIDETEXTUREPATH);
	FILE *fp = NULL;

	char strMiniMapFileName[MAX_PATH];
	char strTempDetailFileName[MAX_PATH];		// %d_%d_detail.dds or %d_%d_16_detail.dds
	char strTempFileName[MAX_PATH];				// %d_%d.dds or %d_%d_16.dds

	int aryIndex[ 9 ] = { 4,  0, 1, 2,  3, 5,  6,  7,  8 } ;	// 미니맵 로드 인덱스 순서
	int aryXStep[ 9 ] = { 0, -1, 0, 1, -1, 1, -1,  0,  1 } ;	// 미니맵 파일명을 구성하기 위해 nCharPosX 에 더하는 값
	int aryZStep[ 9 ] = { 0,  1, 1, 1,  0, 0, -1, -1, -1 } ;	// 미니맵 파일명을 구성하기 위해 nCharPosZ 에 더하는 값

	for ( int i = 0; i < 9; ++i )
	{
		if ( m_MiniMapTexture[ aryIndex[ i ] ].GetTexture() )
			m_MiniMapTexture[ aryIndex[ i ] ].Unload() ;

		int nPosX = nChrPosX + aryXStep[ i ] ;
		int nPosZ = nChrPosZ + aryZStep[ i ] ;

		sprintf( strTempDetailFileName, "%d_%d_detail.dds", nPosX, nPosZ ) ;
		sprintf( strTempFileName, "%d_%d.dds", nPosX, nPosZ ) ;
/*
		// edith 2008.02.19 미니맵처리
		// 다크 카나번 미니맵 데이터를 Zone8 의 경로에서 공유하기 위해서 어쩔수 없이 하드 코딩이 들어간 부분임!!
		if ( SERVER_ID::STONE_WAR == CRYLNetworkData::Instance()->m_dwPresentZone )
		{
			if ( (5 == nPosX &&  8 == nPosZ) ||
				 (5 == nPosX &&  9 == nPosZ) ||
				 (5 == nPosX && 10 == nPosZ) ||
				 (6 == nPosX &&  8 == nPosZ) ||
				 (6 == nPosX &&  9 == nPosZ) || 
				 (6 == nPosX && 10 == nPosZ) ||
                 (7 == nPosX &&  9 == nPosZ) )
			{
				sprintf( strTempDetailFileName, "%d_%d_16_detail.dds", nPosX, nPosZ ) ;
				sprintf( strTempFileName, "%d_%d_16.dds", nPosX, nPosZ ) ;
			}
		}
*/
		sprintf( strMiniMapFileName, "%s\\%s", WIDETEXTUREPATH, strTempDetailFileName ) ;
		if ( ( fp = fopen( strMiniMapFileName, "rb" ) ) == NULL )
		{
			sprintf( strMiniMapFileName, "%s\\%s", WIDETEXTUREPATH, strTempFileName ) ;
			fp = fopen( strMiniMapFileName, "rb" ) ;

			if ( fp )
			{
				fclose( fp ) ;
				strcpy( strMiniMapFileName, strTempFileName ) ;
				CTexture::SetPath( WIDETEXTUREPATH ) ;
				m_MiniMapTexture[ aryIndex[ i ] ].Load( strMiniMapFileName ) ;
			}
			else
			{
				CTexture::SetPath( INTERFACETEXTUREPATH ) ;
				m_MiniMapTexture[ aryIndex[ i ] ].Load( "hig1.dds" ) ;
			}
		}
		else
		{
			fclose( fp ) ;
			strcpy( strMiniMapFileName, strTempDetailFileName ) ;
			CTexture::SetPath( WIDETEXTUREPATH ) ;
			m_MiniMapTexture[ aryIndex[ i ] ].Load( strMiniMapFileName ) ;
		}
	}
}

BOOL	CRYLMIniMapDlg::ClipRect( const RECT& rcArea, int iPosX, int iPosY, RECT& rcImage )
{
	if ( iPosX < rcArea.left || iPosY < rcArea.top)
		return FALSE;

	if ( ( iPosX + rcImage.right - rcImage.left ) > rcArea.right )
	{
		rcImage.right -= ( iPosX + rcImage.right - rcImage.left ) - rcArea.right ;
	}

	if ( ( iPosY + rcImage.bottom - rcImage.top ) > rcArea.bottom )
	{
		rcImage.bottom -= ( iPosY + rcImage.bottom - rcImage.top ) - rcArea.bottom ;
	}

	if ( rcImage.right > 0 && rcImage.bottom > 0 )
	{
		return TRUE ;
	}

	return FALSE ;
}