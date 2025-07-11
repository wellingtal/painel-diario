//=================================================================
// Name : RYLDialogControlDlg.h
// Data : 2003. 12. 24
// Desc : 메인 윈도우 컨트롤 페널 다이얼로그
//=================================================================
#ifndef __RYL_DIALOGCONTROLDLG_H__
#define __RYL_DIALOGCONTROLDLG_H__

#include "RYLDialog.h"

namespace RYLDIALOGCONTROLDLG
{
	const DWORD		MAP_BUTTON					= 0x00000000 ;
	const DWORD		STATUS_BUTTON				= 0x00000001 ;
	const DWORD		SOCIETY_BUTTON				= 0x00000002 ;
	const DWORD		INVEN_BUTTON				= 0x00000003 ;
	const DWORD		GUILD_BUTTON				= 0x00000004 ;
	const DWORD		BATTLE_BUTTON				= 0x00000005 ;
//	const DWORD		GMREPORT_BUTTON				= 0x00000006 ;
	const DWORD		HELP_BUTTON					= 0x00000007 ;
	const DWORD		OPTION_BUTTON				= 0x00000008 ;

	const DWORD		ROTATION_BUTTON				= 0x00000009 ;

	const DWORD		SIEGETIME_BUTTON			= 0x0000005 ;
} ;

class CRYLButton ;
class CRYLImage ;
class CRYLAnimationButton;

class CRYLDialogControlDlg : public CRYLDialog
{
private:
	CRYLButton*				m_pMapButton ;				// 멥 버튼
	CRYLButton*				m_pStatusButton ;			// 스탯 버튼
	CRYLButton*				m_pSocietyButton ;			// 사회 버튼
	CRYLButton*				m_pInvenButton ;			// 인벤토리 버튼
	CRYLButton*				m_pGuildButton ;			// 길드 버튼
	CRYLButton*				m_pBattleButton ;			// 전장 버튼
//	CRYLButton*				m_pGMReportButton ;			// GM신고 버튼
	CRYLButton*				m_pHelpButton ;				// 헬프 버튼
	CRYLButton*				m_pOptionButton ;			// 옵션 버튼
	CRYLButton*				m_pRotationButton ;			// 회전 버튼

	CRYLAnimationButton*	m_pSiegeTimeButton;			// 전장 상태 확인 버튼.

	CRYLImage*				m_pVertImage ;
	BOOL					m_bVertEnable ;
			
public :
	CRYLDialogControlDlg( INT nLeft, INT nTop, BOOL bVert = FALSE ) ;
	virtual ~CRYLDialogControlDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; }
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	RotationDlg() ;

	BOOL	GetVerticalEnable()			{  return m_bVertEnable ; }

	VOID	SetWarTypeButton(unsigned char cType);

	enum Const
	{
		NONE_TYPE		= 0,
		GUILD_WAR_TYPE	= 1,
		REALM_WAR_TYPE	= 2
	};
} ;

#endif //__RYL_DIALOGCONTROLDLG_H__