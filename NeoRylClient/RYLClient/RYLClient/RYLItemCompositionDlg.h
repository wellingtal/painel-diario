//================================================================
//
//	Name : RYLItemCompositionDlg.h 
//	Desc : ������ �ɼ� �̽� ��Ʈ�� ���̾�α�
//  Date : 2004. 06. 04
//
//================================================================
#ifndef __RYL_ITEMCOMPOSITIONDLG_H__
#define __RYL_ITEMCOMPOSITIONDLG_H__

#include "RYLDialog.h"
#include "GMUICustomPanel.h"
#include "..\\ItemInstance.h"	


namespace RYLITEMCOMPOSITIONDLG
{
	const DWORD		OK_BUTTON						= 0x00000000 ;
	const DWORD		CANCEL_BUTTON					= 0x00000001 ;
	const DWORD		MEMBER_BASE_STRINGGRID			= 0x00000002 ;
	const DWORD		MEMBER_OFFERING_STRINGGRID		= 0x00000003 ;
	const DWORD		MEMBER_COMPOSITION_STRINGGRID	= 0x00000004 ;

	enum	ITEMDATA
	{
		ITEMDEFENSIVEPOWER = 0,				// ������ ����
		ITEMMAGICPOWER,						// ������ ������
		ITEMMINDAMAGE,						// ������ �ּҰ��ݷ�
		ITEMMAXDAMAGE,						// ������ �ִ���ݷ�
		ITEMHITRATIO,						// ������ ����
		ITEMMPMAX,							// ������ MP MAX
		ITEMMPREGEN,						// ������ MP ȸ��
		ITEMCRITCAL							// ������ ũ��Ƽ��
	};

	enum	ITEMTYPE
	{
		ITEMTYPE_ARMOUR,
	};
} ;

namespace RYLITEMCOMPO1PANEL
{
	const DWORD		NEXT_BUTTON						= 0x00000000 ;
	const DWORD		CANCEL_BUTTON					= 0x00000001 ;
} ;

namespace RYLITEMCOMPO2PANEL
{
	const DWORD		NEXT_BUTTON						= 0x00000000 ;
	const DWORD		PREV_BUTTON						= 0x00000001 ;
} ;


namespace RYLITEMCOMPO3PANEL
{
	const DWORD		NEXT_BUTTON						= 0x00000000 ;
	const DWORD		PREV_BUTTON						= 0x00000001 ;
} ;

namespace RYLITEMCOMPO4PANEL
{
	const DWORD		OK_BUTTON						= 0x00000000 ;
	const DWORD		CANCEL_BUTTON					= 0x00000001 ;
} ;


class CRYLButton ;
class CRYLLabel ;
class CRYLImage ;
class CRYLStringGrid ;
class CRYLPicture;

class CRYLItemCompositionDlg : public CRYLDialog
{
private :

	DWORD				m_dwPhaseState ;							// ���� phase 0~3
	BOOL				m_bIsEnd;									// ����� ���Դ���..


	CGMUICustomPanel*	m_pPhase1Panel ;							// Phase 1 �ǳ�
	CGMUICustomPanel*	m_pPhase2Panel ;							// Phase 2 �ǳ�
	CGMUICustomPanel*	m_pPhase4Panel ;							// Phase 4 �ǳ�

	CRYLLabel*			m_pDlgLabel ;								// ���̾�α� ��
	char				m_cAttribute ;								// �̽��� �ɼ�

	short				m_arrOfferingItemValue[ 8 ] ;				// ���� ������ �� ( ����, ����, �ּ� ���ݷ�, .... )
	VOID				SetPhase();

public :

	VOID		NextPhase();					// �Ѵܰ� ���� �ܰ��. 4 �ܰ迡�� ���� OK()��. 
	VOID		PrevPhase();					// �Ѵܰ� �� �������. 1 �ܰ迡�� ���� Cancel()��
	VOID		Cancel();						// ���̾�α׸� �ݰ� ����� 1 �ܰ��!
	VOID		OK();							// �̽� �õ� �ϰ� ���̾�α״� �״��. ������ ���� ����
	char		GetOption(){ return m_cAttribute; };
	VOID		SetOption( int nSelect ){ m_cAttribute = nSelect ; };
	VOID		SetPhaseState2First();
	CRYLItemCompositionDlg() ;
	virtual ~CRYLItemCompositionDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT			FinalRender(  LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK; } ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
    virtual HRESULT		Update() ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	virtual VOID		SetVisible( BOOL bVisible ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;


class CRYLItemCompo1Panel : public CGMUICustomPanel
{
private :

	CRYLButton*			m_pCancelButton ;							// ��� ��ư
	CRYLButton*			m_pNextButton ;								// ���� ��ư

	CRYLLabel*			m_pPhaseLabel ;								// ������ ��
	CRYLLabel*			m_pDescription1Label ;						// ���� ù°��
	CRYLLabel*			m_pDescription2Label ;						// ���� ��°��

	CRYLPicture*		m_pItemSlotPicture ;						// ������ ����

public :

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	CRYLItemCompo1Panel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLItemCompo1Panel() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
};

class CRYLItemCompo2Panel : public CGMUICustomPanel
{
private :
	
	CRYLButton*			m_pNextButton ;								// ���� ��ư

	CRYLLabel*			m_pPhaseLabel ;								// ������ ��

	CRYLLabel*			m_pCostLabel ;								// �ɼ� ��� ��
	CRYLLabel*			m_pOptionLabel ;							// �ɼ� ����
	CRYLLabel*			m_pOptionOptLabel;							// �ɼ�
	CRYLLabel*			m_pOptionOriginLabel ;						// ����
	CRYLLabel*			m_pOptionBonusLabel;						// ���ʽ�

	CRYLPicture*		m_pItemSlotPicture ;						// ������ ����

	CRYLStringGrid*		m_pOfferingStringGrid ;						// ���� ������ ����Ʈ	
	int					m_nOfferingCount;
	int					m_nOfferingSelected;
	int					m_aryOffering[8];
	bool				m_aryCanComposition[8];						// �̽� ���� ����

	unsigned long		m_dwCheckLimitResult;						// ���� �ʰ� �ɼǿ� ���� �޽��� �ڽ� ���
	
public :

	CRYLButton*			m_pPrevButton ;								// ���� ��ư
	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	VOID	InitStringGrid( CItemInstance * pItem );
	int		SelectStringGrid();

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	CRYLItemCompo2Panel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLItemCompo2Panel() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
};

class CRYLItemCompo3Panel : public CGMUICustomPanel
{
private :

	CRYLButton*			m_pPrevButton ;								// ���� ��ư
	CRYLButton*			m_pNextButton ;								// ���� ��ư
	

	CRYLLabel*			m_pPhaseLabel ;								// ������ ��
	CRYLLabel*			m_pDescription1Label ;						// ���� ù°��
	CRYLLabel*			m_pDescription2Label ;						// ���� ��°��
	CRYLLabel*			m_pDescription3Label ;						// ���� ��°��
	CRYLLabel*			m_pDescription4Label ;						// ���� ��°��

	CRYLStringGrid*		m_pOfferingStringGrid ;						// ���� ������ ����Ʈ	
	int					m_nOfferingCount;
	int					m_nOfferingSelected;
	int					m_aryOffering[8];
	bool				m_aryCanComposition[8];						// �̽� ���� ����

public :

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	CRYLItemCompo3Panel( CGMUICustomDlg *pParentDlg ) ;
	VOID	InitStringGrid();
	int		SelectStringGrid();
	virtual ~CRYLItemCompo3Panel() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
};

class CRYLItemCompo4Panel : public CGMUICustomPanel
{
private :

	CRYLButton*			m_pOKButton ;						// Ȯ�� ��ư
	CRYLButton*			m_pCancelButton ;					// ��� ��ư

	CRYLLabel*			m_pPhaseLabel ;						// ������ ��
	CRYLLabel*			m_pDescription1Label ;				// ���� ù°��
	CRYLLabel*			m_pDescription2Label ;				// ���� ��°��
	CRYLLabel*			m_pDescription3Label ;				// ���� ��°��
	CRYLLabel*			m_pDescription4Label ;				// ���� ��°��
	CRYLLabel*			m_pDescription5Label ;				// ���� �ټ�°��

	
	CRYLPicture*		m_pItemSlotPicture ;				// ������ ����
	friend				CRYLItemCompositionDlg;

public :

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	VOID ResetText();

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	CRYLItemCompo4Panel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLItemCompo4Panel() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
};

#endif // __RYL_ITEMCOMPOSITIONDLG_H__