//================================================================
//
//	Name : RYLNationSelectDlg.h 
//	Desc : 현재 계정 국적을 변경하기 위해 필요한 다이얼로그
//  Date : 2004. 11. 16
//
//================================================================

// WORK_LIST 2.3 계정 국적 변경 UI 추가

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

	CRYLButton*		m_pKarterantBtn ;		// 계정 국적 변경 버튼 (카르테란트 국적 깃발 표시)
	CRYLButton*		m_pMerkadiaBtn ;		// 계정 국적 변경 버튼 (메르카디아 국적 깃발 표시)
	CRYLButton*		m_pPirateBtn ;			// 계정 국적 변경 버튼 (신의 해적단 국적 깃발 표시)

	CRYLButton*		m_pChangeButton ;		// 계정 국가 변경 버튼
	CRYLButton*		m_pCancelButton ;		// 계정 국가 변경 취소 버튼

	CRYLLabel*		m_pDlgName ;			// 국적 변경 텍스트
	CRYLLabel*		m_pDescLabel[ DESCRIPTION_LINE ] ;		// 다이얼로그 설명 텍스트
	CRYLLabel*		m_pKarterantLabel ;
	CRYLLabel*		m_pMerkadiaLabel ;
	CRYLLabel*		m_pPirateLabel ;

	CHAR			m_szNationDesc[ Creature::MAX_NATION ][ DESCRIPTION_LINE ][ MAX_PATH ] ;

	DWORD			m_dwRace ;				// 현재 종족
	DWORD			m_dwSelectNation ;		// 현재 선택한 국가
	bool			m_dUnifiedEnable ;		// 통합서버화면인가? 캐릭터화면선택인가?

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
