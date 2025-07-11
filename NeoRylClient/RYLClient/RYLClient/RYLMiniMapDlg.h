//=================================================================
// Name : RYLMiniMapDlg.h
// Data : 2004. 01. 05
// Desc : ¸ÞÀÎ À©µµ¿ì ¹Ì´Ï¸ã
//=================================================================
#ifndef __RYL_MINIMAPDLG_H__
#define __RYL_MINIMAPDLG_H__

#include "RYLDialog.h"
#include "Texture.h"
#include "RYLSprite.h"
#include <vector>

namespace RYLMAINMAPDLG
{
	const DWORD		HIDE_BUTTON					= 0x00000000 ;
	const DWORD		SIZE_BUTTON				    = 0x00000001 ;
	const DWORD		ZOOMIN_BUTTON				= 0x00000002 ;
	const DWORD		ZOOMOUT_BUTTON				= 0x00000003 ;
} ;

class CRYLButton ;
class CRYLLabel ;

struct QuestInstance ;

enum 
{ 
    DEFAULT_ICON                = 6, 

    WEAPON_MERCHANT_ICON        = 13,
    ARMOR_MERCHANT_ICON         = 14,
    ETC_MERCHANT_ICON           = 15,
    TRAINER_ICON                = 16, 
    TELEPORTER_ICON             = 17,
    BLACK_SMITH_ICON            = 18,
    CONTAINER_ICON              = 19,
    DARK_WEAPON_MERCHANT_ICON   = 21,
    DARK_ARMOR_MERCHANT_ICON    = 22,
    PEACE_GHUILD_ICON           = 24,
    CLEANER_ICON                = 26,
    GUILD_MAKER_ICON            = 28,
};

class CRYLMIniMapDlg : public CRYLDialog
{
private:
	CRYLButton*			m_pHideButton ;				// Hide ¹öÆ°
	CRYLLabel*			m_pPositionLabel ;			// À§Ä¡ ÀÌ¸§
	CRYLButton*			m_pZoomInButton ;			// ÁÜÀÎ
	CRYLButton*			m_pZoomOutButton ;			// ÁÜ¾Æ¿ô

	CRYLButton*			m_pSizeButton ;			// ÁÜ¾Æ¿ô

	std::vector<CRYLSprite *> m_lstSprite ;
	CTexture*			m_pMapTexture ;

	CTexture			m_lpLargeMapTexture[ 4 ] ;
	CTexture			m_MiniMapTexture[ 9 ] ;
	CRYLSprite			m_lpLargeMapSprite[ 4 ] ;

	INT					m_MiniMapSize ;
	BOOL				m_bFullSize ;
	BOOL				m_bBattleRespawn ;
	BOOL				m_bZoomClick ;
	QuestInstance*		m_lpQuestTarget ;

	int miViewType;
	int miWIDTH;
	int miHEIGHT;
	int miAreaX;
	int miAreaY;
	int miSize;
	int miGep;
	int miMobGep;

public:

public :

	CRYLMIniMapDlg(INT nLeft, INT nTop, INT iType) ;
	virtual ~CRYLMIniMapDlg() ;

	int GetViewType() { return miViewType; }

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )	;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgMouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	VOID		ZoomIn() ;
	VOID		ZoomOut() ;
	CTexture*	GetMapTexture()			{ return m_pMapTexture ; }

	BOOL		ClipRect( const RECT& rcArea, int iPosX, int iPosY, RECT& rcImage ) ;

	VOID ClearSprite() ;

	VOID ProcessDlgSize(int iType = -1);

	VOID ReloadTexture(void);		// By Belldandy
	VOID MiniMapDelete(void);       // 05.06.03   ¹Ì´Ï¸Ê ÅØ½ºÃÄ Áö¿ì±â
} ;

#endif //__RYL_MINIMAPDLG_H__