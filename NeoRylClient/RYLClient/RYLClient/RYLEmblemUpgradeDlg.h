//================================================================
//
//	Name : RYLEmblemUpgrade.h 
//	Desc : ��¡���� ���׷��̵� ���̾�α�
//  Date : 2004. 06. 01
//
//================================================================
#ifndef __RYL_EMBLEMUPGRADEDLG_H__
#define __RYL_EMBLEMUPGRADEDLG_H__

#include "RYLDialog.h"

namespace RYLEMBLEMUPGRADEDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// Ȯ�� ��ư
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// ��� ��ư
};

class CRYLButton ;
class CRYLLabel ;
class CRYLCastle ;

class CRYLEmblemUpgradeDlg : public CRYLDialog
{
public:
	enum eUpgradeError
	{
		S_ALL_READY		= 0,
		E_NOT_RIGHT,
		E_MAX_LEVEL,
		E_NOT_ENOUGH_NUM,
		E_DIFFERENT_JEWEL
	};

private :
	CRYLLabel*		m_pEmblemUpgradeLabel ;				// ��¡���� ���׷��̵�
	CRYLLabel*		m_pJewelLabel ;						// [��������]
	CRYLLabel*		m_pEmblemUpgradeExpla1Label ;		// ���׷��̵� ���� 1
	CRYLLabel*		m_pEmblemUpgradeExpla2Label ;		// ���׷��̵� ���� 2
	CRYLLabel*		m_pEmblemUpgradeExpla3Label ;		// ���׷��̵� ���� 3
	CRYLLabel*		m_pEmblemUpgradeExpla4Label ;		// ���׷��̵� ���� 4
	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;

	unsigned long	m_dwCastleID ;						// ��¡���� ���� �ִ� �� ID
	unsigned long	m_dwEmblemCID ;						// ��¡���� ũ���� ID

	eUpgradeError	CheckUpgradable() ;					// ���׷��̵尡 �������� üũ

public :
	CRYLEmblemUpgradeDlg() ;
	virtual ~CRYLEmblemUpgradeDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	
	VOID	SetCastleAndEmblemID( unsigned long dwCastleID, unsigned long dwEmblemID )
	{
		m_dwCastleID = dwCastleID ;
		m_dwEmblemCID = dwEmblemID ;
	}

	unsigned long	GetCastleID()		{ return m_dwCastleID ; }
	unsigned long	GetEmblemCID()		{ return m_dwEmblemCID;	}

	void	SetEmblemInfo( unsigned long dwEmblemID ) ;
} ;

#endif // __RYL_EMBLEMUPGRADEDLG_H__