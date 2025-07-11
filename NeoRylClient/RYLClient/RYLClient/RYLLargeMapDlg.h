//=================================================================
// Name : RYLLargeMapDlg.h
// Data : 2004. 01. 09
// Desc : 라지 멥 다이얼로그
//=================================================================
#ifndef __RYL_LARGEMAP_H__
#define __RYL_LARGEMAP_H__

#include "RYLDialog.h"
#include "GMUICustomPanel.h"
#include <d3dx8.h>
#include <map>

namespace RYLLARGEMAPDLG
{
	const DWORD		HUMANRESPAWNPOS1_BUTTON					= 0x00000000 ;
	const DWORD		HUMANRESPAWNPOS2_BUTTON					= 0x00000001 ;
	const DWORD		HUMANRESPAWNPOS3_BUTTON					= 0x00000002 ;

	const DWORD		AKHANRESPAWNPOS1_BUTTON					= 0x00000003 ;
	const DWORD		AKHANRESPAWNPOS2_BUTTON					= 0x00000004 ;
	const DWORD		AKHANRESPAWNPOS3_BUTTON					= 0x00000005 ;

	const DWORD		BATTLEEXIT_BUTTON						= 0x00000006 ;
} ;

class CRYLButton ;
class CRYLSpriteEx ;
class CRYLLabel ;
class CRYLPicture ;
class CGMFontTexture ;
class CRYLSprite ;
class CRYLImage ;

typedef struct __HUMANRESPAWNEX
{
	int			iPutX ;
	int			iPutY ;
	D3DXVECTOR3 v3Point ;
}HUMANRESPAWNEX, *LPHUMANRESPAWNEX ;

typedef struct __AKHANRESPAWNEX
{
	int			iPutX ;
	int			iPutY ;
	D3DXVECTOR3 v3Point ;
}AKHANRESPAWNEX, *LPAKHANRESPAWNEX ;

//-- 2004. 3. 12. Zergra From. --//
typedef struct StoneState
{
    unsigned short  wKID;
    unsigned char   cState;
} StoneState, **lpStoneState;
//-- Zergra To. --//

class CRYLLargeMapDlg : public CGMUICustomPanel
//class CRYLLargeMapDlg : public CRYLDialog ///////////////////////
{
private:
	CRYLSpriteEx*		m_pLargeMapSprite[ 4 ] ;
	
	CGMFontTexture*		m_pFontTexture ;

    CRYLImage*          m_pHumanStoneIcon;
    CRYLImage*          m_pAkhanStoneIcon;
    CRYLImage*          m_pNuetralStoneIcon;
    CRYLImage*          m_pDangerIcon;

    //-- 2004. 3. 12. Zergra From. --//
    StoneState          m_StoneState[ 5 ];
    //-- Zergra To. --//	\

	CRYLSpriteEx*			m_pCampDistributeSprite[ 3 ] ;
	CRYLSpriteEx*			m_pCampAttackedSprite ;
	
public :
	
public :
	CRYLLargeMapDlg( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLLargeMapDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;


	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	LoadLargeMapTexture() ;
	
	//-------------------------------------------------------------------
	// 명예의 석상 관련 데이타
	BOOL				m_bShowLargeMap ;
	BOOL				m_bBattleRespawn ;
	INT					m_SpawnNumber ;
	INT					m_iSelectRespawnPoint ;
	
	CRYLButton*			m_pBattleExitButton ;			// 베틀그라운드 종료버튼
	CRYLButton*			m_pHumanSpawnButton[ 3 ] ;		// 휴먼 리스폰 버튼
	CRYLPicture*		m_pHumanSpawnSprite[ 3 ] ;		// 휴먼 리스폰 스프라이트
	
	CRYLButton*			m_pAkhanSpawnButton[ 3 ] ;		// 아칸 리스폰 버튼
	CRYLPicture*		m_pAkhanSpawnSprite[ 3 ] ;		// 아칸 리스폰 스프라이트

	USHORT				m_WaitNum_Out[ 3 ] ;
	HUMANRESPAWNEX		m_HumanRespawn[ 3 ] ;
	AKHANRESPAWNEX		m_AkhanRespawn[ 3 ] ;

	VOID	SetShowLargeMap( BOOL bShowLargeMap ) ;
	BOOL	GetShowLargeMap()		{ return m_bShowLargeMap ; }

	VOID	ShowLargeMap( USHORT& pWaitNum_Out ) ;
	BOOL	GetRespawnEnable()		{ return m_bBattleRespawn ; }
	VOID	HideControl() ;
	
	//-------------------------------------------------------------------

    //-- 2004. 3. 12. Zergra From. --//
    VOID                SetStoneState( unsigned char cIndex, unsigned wKID, unsigned char cState );
    //-- Zergra To. --//	

	//Minotaurs

	void	DrawCampArea( INT xx, INT yy ) ;
    void	DrawGuildInfo( INT xx, INT yy ) ;
} ;

#endif //__RYL_LARGEMAP_H__