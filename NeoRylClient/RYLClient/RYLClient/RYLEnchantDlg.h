//=================================================================
// Name : RYLEnchantDlg.h
// Data : 2004. 02. 06
// Desc : 인챈트 다이얼로그
//=================================================================
#ifndef __RYL_ENCHANTDLG_H__
#define __RYL_ENCHANTDLG_H__

#include "RYLDialog.h"
#include "GMUITooltipBox.h"

namespace RYLENCHANTDLG
{
	const DWORD		HIDE_BUTTON					= 0x00000000 ;
} ;

class CRYLSpriteEx ;
class CRYLButton ;

class CRYLEnchantDlg : public CRYLDialog
{
private :
	BOOL				m_bShowTooltip;
	CGMUITooltipBox*	m_lpTooltip;


	CRYLButton*	 m_pHideButton ;

	CRYLSprite *m_lpChant[61];					// from 59 to 60 for clairvoyance ( RYLEnchantDlg.cpp )	
	CRYLSprite *m_lpChantBig[72];					// 화면 좌하단에 나오는 챈트 아이콘	// was 68	

	CTexture *m_lpChantIcon;					// 챈트 아이콘 텍스쳐
	CTexture *m_lpChantIconBig;					// 챈트 아이콘 텍스쳐

public :
	CRYLEnchantDlg( INT nLeft, INT nTop ) ;
	virtual ~CRYLEnchantDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	CRYLSprite *GetChantIcon(unsigned long dwChantNum);
} ;

#endif //__RYL_ENCHANTDLG_H__