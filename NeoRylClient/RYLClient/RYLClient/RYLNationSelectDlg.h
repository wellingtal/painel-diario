//================================================================
//
//	Name : RYLNationSelectDlg.h 
//	Desc : ���� ���� ������ �����ϱ� ���� �ʿ��� ���̾�α�
//  Date : 2004. 11. 16
//
//================================================================

// WORK_LIST 2.3 ���� ���� ���� UI �߰�

#ifndef __RYL_NATION_SELECT_DLG_H__
#define __RYL_NATION_SELECT_DLG_H__

#include "RYLDialog.h"

namespace RYLNATIONSELECTDLG
{
	const DWORD		KARTERANT_BUTTON	= 0x00000000 ;
	const DWORD		MERKADIA_BUTTON		= 0x00000001 ;
	const DWORD		PIRATE_BUTTON		= 0x00000002 ;

	const DWORD		CHANGE_BUTTON		= 0x00000010 ;
	const DWORD		CANCEL_BUTTON		= 0x00000011 ;
} ;

class CRYLButton ;
class CRYLLabel ;

//===========================================================
// Nation Select Dlg
//===========================================================
class CRYLNationSelectDlg : public CRYLDialog
{
private:
	enum Const
	{
		DESCRIPTION_LINE	= 10
	};

	CRYLButton*		m_pKarterantBtn ;		// ���� ���� ���� ��ư (ī���׶�Ʈ ���� ��� ǥ��)
	CRYLButton*		m_pMerkadiaBtn ;		// ���� ���� ���� ��ư (�޸�ī��� ���� ��� ǥ��)
	CRYLButton*		m_pPirateBtn ;			// ���� ���� ���� ��ư (���� ������ ���� ��� ǥ��)

	CRYLButton*		m_pChangeButton ;		// ���� ���� ���� ��ư
	CRYLButton*		m_pCancelButton ;		// ���� ���� ���� ��� ��ư

	CRYLLabel*		m_pDlgName ;			// ���� ���� �ؽ�Ʈ
	CRYLLabel*		m_pDescLabel[ DESCRIPTION_LINE ] ;		// ���̾�α� ���� �ؽ�Ʈ
	CRYLLabel*		m_pKarterantLabel ;
	CRYLLabel*		m_pMerkadiaLabel ;
	CRYLLabel*		m_pPirateLabel ;

	CHAR			m_szNationDesc[ Creature::MAX_NATION ][ DESCRIPTION_LINE ][ MAX_PATH ] ;

	DWORD			m_dwRace ;				// ���� ����
	DWORD			m_dwSelectNation ;		// ���� ������ ����
	bool			m_dUnifiedEnable ;		// ���ռ���ȭ���ΰ�? ĳ����ȭ�鼱���ΰ�?

public :
	CRYLNationSelectDlg() ;
	virtual ~CRYLNationSelectDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )	;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ;

	DWORD GetSelectNation() const	{ return m_dwSelectNation; }

	VOID ChangeNationDesc( DWORD dwNation ) ;
	VOID InitValue( DWORD dwRace, DWORD dwNation ) ;

	void UnifiedNextPage() ;
	void SetUnifiedEnable( bool bEnable ) ;
	bool GetUnifiedEnable()							{ return m_dUnifiedEnable ; }
} ;


#endif //__RYL_NATION_SELECT_DLG_H__
