
#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLProgressBar.h"

#include "WinInput.h"
#include "BaseGraphicsLayer.h"
#include "GMImageDib.h"
#include "RYLGameData.h"
#include "RYLGuildWarPenaltyDlg.h"

#include "RYLButton.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"
#include "RYLLabel.h"
#include "GMMemory.h"

CRYLGuildWarPenaltyDlg* g_GuilWarPenaltyDlg = NULL;

CRYLGuildWarPenaltyDlg::CRYLGuildWarPenaltyDlg() : CRYLDialog()
{
	memset(m_szGuildName, 0, Guild::MAX_GUILD_NAME_LEN);

	m_pCaption	= NULL;		
	m_pProgress	= NULL;		
	m_dwTick	= 0;	
	m_cZone		= 0;

	for(unsigned char cIndex = 0; cIndex < 4; cIndex++)
	{
		m_pText[cIndex]	= NULL;	
	}

	SetWidth(256);
	SetHeight(180);
	
	SetLeft(100);
	SetTop(100);
	
	InitCtrl();
	
	g_GuilWarPenaltyDlg = this;
}

VOID CRYLGuildWarPenaltyDlg::InitCtrl()
{	
	RECT rt;

	// 라벨.

	m_pCaption = new CRYLLabel( this ) ;
	m_pCaption->SetLeft(12);
	m_pCaption->SetTop(18);
	m_pCaption->SetFontColor (0xffffffff);
	m_pCaption->SetAutoSize( TRUE ) ;
	m_pCaption->SetCaption(CRYLStringTable::m_strString[4042]);	
	AttachGMUIControl(m_pCaption);	

	for(unsigned char cIndex = 0; cIndex < 4; cIndex++)
	{
		m_pText[cIndex] = new CRYLLabel( this ) ;
		m_pText[cIndex]->SetLeft(12);
		m_pText[cIndex]->SetTop(52+cIndex*14);
		m_pText[cIndex]->SetFontColor (0xffffffff);
		m_pText[cIndex]->SetAutoSize( TRUE ) ;
		m_pText[cIndex]->SetCaption(CRYLStringTable::m_strString[4038+cIndex]);	
		AttachGMUIControl(m_pText[cIndex]);
	}	
	
	// 진행상황 프로그래스바
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pProgress = new CRYLProgressBar( this ) ;
	m_pProgress->SetLeft( 15 ) ;
	m_pProgress->SetTop( 130 ) ;
	m_pProgress->SetWidth( 220 ) ;
	m_pProgress->SetHeight( 11 ) ;
	m_pProgress->SetMax( 20 ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 220, 11, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt,140, 53, 250, 64 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 110, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pProgress->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pProgress->SetPosition( 0 ) ;
	AttachGMUIControl( m_pProgress ) ;
	GM_DELETE( pImageDIB ) ;

	// 다이얼 로그 이미지.	

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, ( 30 * 5 ) + 15 , &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;		
	int i = 0;
	for (  ; i < 30 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}	

	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;			

	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xffB99E6B, 0x80000000,  1 ) ;
	
	// 라인그리기.	

	pImageDIB->MakeFrameRect( 12, 111, 225, 20, 0xffB99E6B, 0x80000000,  1 ) ;
	pImageDIB->DrawFrameRect( 13, 112, 223, 18, 0xc0000000 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture((CGMTexture*)pImage);		

	GM_DELETE( pImageDIB ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
}

CRYLGuildWarPenaltyDlg::~CRYLGuildWarPenaltyDlg()
{
	FinalCtrl() ;
}

VOID CRYLGuildWarPenaltyDlg::FinalCtrl ()
{
	GM_DELETE(m_pCaption);	
	
	for(unsigned char cIndex = 0; cIndex < 4; cIndex++)
	{
		GM_DELETE(m_pText[cIndex]);	
	}

	GM_DELETE(m_pProgress);	
}

VOID CRYLGuildWarPenaltyDlg::Show( CGMUIObject* Sender )
{	
}
	
VOID CRYLGuildWarPenaltyDlg::Hide( CGMUIObject* Sender )
{	
}

VOID CRYLGuildWarPenaltyDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;	
}

HRESULT CRYLGuildWarPenaltyDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible() )
	{
		return S_OK;
	}

	if(GetCloseButton())
	{
		GetCloseButton()->SetVisible( FALSE ) ;
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
	
	unsigned long dwTick = timeGetTime();

	char szInfo[1024] = {0,};	
	sprintf(szInfo, CRYLStringTable::m_strString[4040], 10-(dwTick-m_dwTick)/1000);		
	m_pText[2]->SetCaption(szInfo);
	
	if( (dwTick-m_dwTick)/500 < 20 )
	{   
		m_pProgress->SetPosition( (dwTick-m_dwTick)/500 );
	}
	else
	{ 
		m_pProgress->SetPosition( 20 );
	}

	if((dwTick-m_dwTick)/1000>10)
	{		
		SetVisible( FALSE ) ;		
	}

	return S_OK ;
}

VOID CRYLGuildWarPenaltyDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{	
}

HRESULT	CRYLGuildWarPenaltyDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )
{ 
	return S_OK;
}

HRESULT	CRYLGuildWarPenaltyDlg::FinalRender(LPDIRECT3DDEVICE8 lpD3DDevice) 
{
	return S_OK ;
}

void CRYLGuildWarPenaltyDlg::SetPenalty(char* szGuildName)
{
	m_dwTick = timeGetTime();
	memcpy(m_szGuildName, szGuildName, Guild::MAX_GUILD_NAME_LEN);

	char szInfo[1024] = {0,};	
	sprintf(szInfo, CRYLStringTable::m_strString[4038], m_szGuildName);		
	m_pText[0]->SetCaption(szInfo);

	// 무방비 페널티 이펙트 //
	/*RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	
	if(!pSelfCreature )
		return;

	const vector3 vPos	= *(pSelfCreature->GetPosition());

	CEffScript* pPenaltyEff = new CEffScript;
	CSceneManager::AddEffectScript( pPenaltyEff );

	pPenaltyEff->GetScriptBinData( "Grease1.esf", CRYLGameData::Instance()->m_bSelfEffect );
	pPenaltyEff->SetStartPos( vPos.x, vPos.y, vPos.z );
	pPenaltyEff->SetEndPos( (D3DXVECTOR3*)pSelfCreature->GetPosition(), 1 );


	CX3DEffect *pEffect = new CX3DEffect;
	CSceneManager::AddEffect( pEffect );	
	pEffect->Load( EFFECTPATH, "Grease1.eff" );	
	pEffect->SetLoop( FALSE );				
	pEffect->SetAxisSetAxis( 0.0f,0.0f,0.0f );
	pEffect->SetCenter(vPos.x,vPos.y,vPos.z );*/
}