//=================================================================
// Name : RYLHostilityDlg.h
// Data : 2005. 09. 01
// Desc : 길드 적대 현황 다이얼로그
//=================================================================

#ifndef __RYL_GUILDHOSTILITYDLG_H__
#define __RYL_GUILDHOSTILITYDLG_H__

#include "RYLDialog.h"
#include <vector>

struct GuildHostilityInfoNode;

namespace RYLGUILDHOSTILITYDLG
{
	const DWORD		STRING_GRID		= 0x00000000 ;
	const DWORD		PREV_BUTTON		= 0x00000001 ;
	const DWORD		NEXT_BUTTON		= 0x00000002 ;
} ;


class CRYLButton ;
class CRYLListBox ;
class CRYLLabel ;
class CRYLStringGrid ;

class CRYLHostilityDlg : public CRYLDialog
{
public :
		
	CRYLLabel*			m_pTitleLabel ;							// 적대 현황

	CRYLLabel*			m_pDecHostilityGuildLabel ;				// 적대 선언 중인 길드
	CRYLLabel*			m_pDecHostilityGuildValueLabel ;		// 적대 선언 중인 길드 값
	CRYLLabel*			m_pDecAlertHostilityGuildLabel ;		// 경계 적대 선언 길드
	CRYLLabel*			m_pDecAlertHostilityGuildValueLabel ;	// 경계 적대 선언 길드 값

	CRYLLabel*			m_pHostilityGuildLabel ;				// 적대 선언 받은 길드
	CRYLLabel*			m_pGuildNameLabel ;						// 길드명
	CRYLLabel*			m_pDeclareKindLabel ;					// 선언종류
	CRYLStringGrid*		m_pStringGrid ;							// 적대 선언 받은 길드 그리드

	CRYLButton*			m_pPrevButton ;							// 이전 버튼
	CRYLButton*			m_pNextButton ;							// 다음 버튼
	CRYLLabel*			m_pPageLabel ;							// 페이지 값

	DWORD				m_dwGID ;								// 자신의 길드 ID

	INT					m_iCurrentPage ;						// 현재 페이지
	INT					m_iTotalItemNum ;						// 리스트에 들어가는 총 아이템 갯수
	INT					m_iPageState ;							// 요청할 페이지의 상태

public :

	CRYLHostilityDlg() ;
	virtual ~CRYLHostilityDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	VOID	SetGID( unsigned long dwGID )	{ m_dwGID = dwGID;	}

	// 적대 현황 리스트 페이지 버튼
	VOID	HostilityListPrev() ;
	VOID	HostilityListNext() ;
	
	VOID	RequestHostiliyList() ;
	VOID	SetHostilityList( int iCurrentPage, int iTotalItemNum, char* szHostility, char* szAlert,
							  unsigned char cNodeNum, GuildHostilityInfoNode* lpHostilityInfoNode ) ;
} ;

#endif //__RYL_GUILDHOSTILITYDLG_H__
