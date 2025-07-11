//================================================================
//
//	Name : RYLLargeMapCampDistributeDlg.h 
//	Desc : 라지 맵 다이얼로그
//  Date : 2004. 07. 16
//
//================================================================
#ifndef __RYL_LARGEMAPCAMPDISTRIBUTEDLG_H__
#define __RYL_LARGEMAPCAMPDISTRIBUTEDLG_H__

#include "GMUICustomPanel.h"
#include <d3dx8.h>

class CRYLImage ;

class CRYLCampDistributePanel : public CGMUICustomPanel
{
private :
	CRYLSpriteEx*			m_pCampDistributeSprite[ 3 ] ;
	CRYLSpriteEx*			m_pCampSimbolSprite[ 4 ] ;
	CRYLSpriteEx*			m_pCampEnableBuild[ 4 ] ;
	CRYLSpriteEx*			m_pCampAttackedSprite ;

public :
	CRYLCampDistributePanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLCampDistributePanel() ;

	VOID					InitCtrl() ;
	VOID					FinalCtrl() ;
	VOID					LoadCampEnableBuildMapTexture() ;

	virtual VOID			Show( CGMUIObject* Sender ) ;
	virtual VOID			Hide( CGMUIObject* Sender ) ;

	virtual VOID			GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT			Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT			FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; } ;
	virtual HRESULT			Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID				DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	
protected:
	void	DrawCampArea( INT xx, INT yy ) ;
    void	DrawGuildInfo( INT xx, INT yy ) ;
} ;

#endif