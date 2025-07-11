//================================================================
//
//	Name : RYLWorldWeaponLaunchDlg.h 
//	Desc : ���� ���� �߻� �޴�
//  Date : 2004. 12. 30
//
//================================================================
#ifndef __RYL_WORLDWEAPONLAUNCHDLG_H__
#define __RYL_WORLDWEAPONLAUNCHDLG_H__

#include "RYLDialog.h"

namespace RYLWORLDWEAPONLAUNCHDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// Ȯ�� ��ư
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// ��� ��ư
	const DWORD		ID_POSX_EDIT			= 0x00000002 ;// ��ǥ X
	const DWORD		ID_POSY_EDIT			= 0x00000003 ;// ��ǥ Y
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLWorldWeaponLaunchDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pWeaponLaunchLabel ;				// ���� ���� �߻�
	CRYLLabel*		m_pWeaponLaunchPosionLabel ;		// �߻� ��ǥ
	CRYLLabel*		m_pWeaponLaunchExpla1Label ;		// ���� �߻� ���� 1
	CRYLLabel*		m_pWeaponLaunchExpla2Label ;		// ���� �߻� ���� 2
	CRYLLabel*		m_pWeaponLaunchPosXLabel ;			// X ��ǥ
	CRYLLabel*		m_pWeaponLaunchPosYLabel ;			// Y ��ǥ
	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;
	CRYLEdit*		m_pPosXEdit ;						//��ǥX
	CRYLEdit*		m_pPosYEdit ;						//��ǥY
	int				m_nPosX ;							//��ǥX
	int				m_nPosY ;							//��ǥY
	DWORD			m_dwCampID ;							// ķ�� ���̵�

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
