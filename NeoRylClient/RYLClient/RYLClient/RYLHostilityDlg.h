//=================================================================
// Name : RYLHostilityDlg.h
// Data : 2005. 09. 01
// Desc : ��� ���� ��Ȳ ���̾�α�
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
		
	CRYLLabel*			m_pTitleLabel ;							// ���� ��Ȳ

	CRYLLabel*			m_pDecHostilityGuildLabel ;				// ���� ���� ���� ���
	CRYLLabel*			m_pDecHostilityGuildValueLabel ;		// ���� ���� ���� ��� ��
	CRYLLabel*			m_pDecAlertHostilityGuildLabel ;		// ��� ���� ���� ���
	CRYLLabel*			m_pDecAlertHostilityGuildValueLabel ;	// ��� ���� ���� ��� ��

	CRYLLabel*			m_pHostilityGuildLabel ;				// ���� ���� ���� ���
	CRYLLabel*			m_pGuildNameLabel ;						// ����
	CRYLLabel*			m_pDeclareKindLabel ;					// ��������
	CRYLStringGrid*		m_pStringGrid ;							// ���� ���� ���� ��� �׸���

	CRYLButton*			m_pPrevButton ;							// ���� ��ư
	CRYLButton*			m_pNextButton ;							// ���� ��ư
	CRYLLabel*			m_pPageLabel ;							// ������ ��

	DWORD				m_dwGID ;								// �ڽ��� ��� ID

	INT					m_iCurrentPage ;						// ���� ������
	INT					m_iTotalItemNum ;						// ����Ʈ�� ���� �� ������ ����
	INT					m_iPageState ;							// ��û�� �������� ����

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

	// ���� ��Ȳ ����Ʈ ������ ��ư
	VOID	HostilityListPrev() ;
	VOID	HostilityListNext() ;
	
	VOID	RequestHostiliyList() ;
	VOID	SetHostilityList( int iCurrentPage, int iTotalItemNum, char* szHostility, char* szAlert,
							  unsigned char cNodeNum, GuildHostilityInfoNode* lpHostilityInfoNode ) ;
} ;

#endif //__RYL_GUILDHOSTILITYDLG_H__
