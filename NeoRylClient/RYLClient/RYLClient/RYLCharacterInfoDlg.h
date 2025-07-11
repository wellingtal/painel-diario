//================================================================
//
//	Name : RYLCharacterInfoDlg.h 
//	Desc : 캐릭터 정보
//  Date : 2003. 12. 1
//
//================================================================
#ifndef __RYL_CHARACTERINFODLG_H__
#define __RYL_CHARACTERINFODLG_H__

#include "RYLDialog.h"

namespace RYLCHARACTERINFODLG
{
	const DWORD		EXIT_BUTTON			= 0x00000000 ;
	const DWORD		START_BUTTON		= 0x00000001 ;
	const DWORD		DELETE_BUTTON		= 0x00000002 ;
	const DWORD		CANCEL_BUTTON		= 0x00000003 ;
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLSpriteEx ;

class CRYLCharacterInfoDlg : public CRYLDialog
{
private:
	CRYLButton*			m_pExitBtn ;
	CRYLLabel*			m_pCharNameLabel ;
	CRYLLabel*			m_pCharInfoLabel ;
	CRYLLabel*			m_pCharNameValueLabel ;
	CRYLLabel*			m_pCharLevelLabel ;
	CRYLLabel*			m_pCharLevelValueLabel ;
	CRYLLabel*			m_pCharFameLabel ;
	CRYLLabel*			m_pCharFameValueLabel ;
	CRYLLabel*			m_pCharClassLabel ;
	CRYLLabel*			m_pCharClassValueLabel ;
	CRYLLabel*			m_pCharNationLabel ;
	CRYLLabel*			m_pCharNationValueLabel ;
	CRYLLabel*			m_pCharGuildNameLabel ;
	CRYLLabel*			m_pCharGuildNameValueLabel ;
	
	DWORD				m_dwOverChrID ;
	DWORD				m_dwOverChr ;
	DWORD				m_dwEndTimer ;

	CRYLButton*			m_pStartBtn ;
	CRYLButton*			m_pDeleteBtn ;
	CRYLButton*			m_pCancelBtn ;
	CRYLSpriteEx*		m_pMarkSprite ;
		
	CHAR				m_szNationName[ 4 ][ MAX_PATH ] ;

public :
	BOOL				m_dMsgBox ;

public :
	CRYLCharacterInfoDlg() ;
	virtual ~CRYLCharacterInfoDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	InitValue() ;
	VOID	InitDeleteValue() ;
	VOID	CharacterCancelEvent() ;

	HRESULT		CharacterSelctUpdate() ;
	VOID		EnableButton( BOOL bEnable = TRUE ) ;

	VOID		PrintInfoLabel( DWORD dwSelectChr ) ;
	VOID		CharacterDelete() ;
} ;

#endif //__RYL_CHARACTERINFODLG_H__