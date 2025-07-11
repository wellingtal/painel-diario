//================================================================
//
//	Name : RYLWorldWeaponLaunchDlg.h 
//	Desc : 월드 웨폰 발사 메뉴
//  Date : 2004. 12. 30
//
//================================================================
#ifndef __RYL_WORLDWEAPONLAUNCHDLG_H__
#define __RYL_WORLDWEAPONLAUNCHDLG_H__

#include "RYLDialog.h"

namespace RYLWORLDWEAPONLAUNCHDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// 확인 버튼
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// 취소 버튼
	const DWORD		ID_POSX_EDIT			= 0x00000002 ;// 좌표 X
	const DWORD		ID_POSY_EDIT			= 0x00000003 ;// 좌표 Y
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLWorldWeaponLaunchDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pWeaponLaunchLabel ;				// 월드 웨폰 발사
	CRYLLabel*		m_pWeaponLaunchPosionLabel ;		// 발사 좌표
	CRYLLabel*		m_pWeaponLaunchExpla1Label ;		// 웨폰 발사 설명 1
	CRYLLabel*		m_pWeaponLaunchExpla2Label ;		// 웨폰 발사 설명 2
	CRYLLabel*		m_pWeaponLaunchPosXLabel ;			// X 좌표
	CRYLLabel*		m_pWeaponLaunchPosYLabel ;			// Y 좌표
	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;
	CRYLEdit*		m_pPosXEdit ;						//좌표X
	CRYLEdit*		m_pPosYEdit ;						//좌표Y
	int				m_nPosX ;							//좌표X
	int				m_nPosY ;							//좌표Y
	DWORD			m_dwCampID ;							// 캠프 아이디

protected :
	BOOL			CheckLaunch();
	VOID			WorldWeaponLaunch();


public :
	CRYLWorldWeaponLaunchDlg() ;
	virtual ~CRYLWorldWeaponLaunchDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;
	
	VOID	SetCamp( DWORD dwCampID ){ m_dwCampID = dwCampID; };
	VOID	SetLoad( BOOL bLoad );

	int			GetPosX()			{ return m_nPosX;		}
	int			GetPosY()			{ return m_nPosY;		}
	CRYLEdit*	GetPosXEdit()		{ return m_pPosXEdit;	}
	CRYLEdit*	GetPosYEdit()		{ return m_pPosYEdit;	}

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

	VOID	Effect( DWORD nPosX, DWORD nPosZ );
} ;

#endif // __RYL_WORLDWEAPONLAUNCHDLG_H__
