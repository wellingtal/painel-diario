//=================================================================
//
// Name : RYLMiningAmountDlg.h
// Data : 2004. 12. 14
// Desc : Mining Amount Dialog Class, ���� �Ÿ��� Ȯ�� ���̾�α�
//
//=================================================================
#ifndef __RYL_MININGAMOUNTDLG_H__
#define __RYL_MININGAMOUNTDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"
#include <vector>

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

namespace RYLMININGAMOUNTDLG
{
	const DWORD		PREV_BUTTON				= 0x00000000 ;
	const DWORD		NEXT_BUTTON				= 0x00000001 ;
} ;


struct MineralVeinInfo;

 

class CRYLMiningAmountDlg : public CRYLDialog
{
private : 

	CRYLButton*			m_pPrevButton ;							// ������ �̵� ��ư
	CRYLButton*			m_pNextButton ;							// ������ �̵� ��ư
	CRYLLabel*			m_pPhaseLabel ;							// ������ ��
	CRYLLabel*			m_pMineNameLabel[8] ;					// ���� �̸�
	CRYLLabel*			m_pMineCountLabel[8] ;					// ���� ����
	CRYLLabel*			m_pTitleLabel ;							// ������ ���Է� Ȯ�� ( Ÿ��Ʋ )
	CRYLLabel*			m_pVeinNameLabel ;						// ���� �̸�
	CRYLLabel*			m_pDescription1 ;						// ���� 1
	CRYLLabel*			m_pDescription2 ;						// ���� 2
	CRYLLabel*			m_pDescription3 ;						// ���� 3
	CRYLLabel*			m_pDescription4 ;						// ���� 4
	std::vector< CItemInstance* >	m_vecMineIcon ;				// ���� ������
	std::vector< int >	m_vecMineCnt;							// ���� ����


	int					m_nCurPhase ;							// phase 
	int					m_nMaxPhase ;							// ������ �ƽ� ��.

protected:
	VOID				NextPhase();
	VOID				PrevPhase();
	VOID				SetPhaseLabel();
	VOID				ClearItem();

public :
	CRYLMiningAmountDlg() ;
	virtual ~CRYLMiningAmountDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	SetItemIcon( );				//�ӽ�

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	VOID	UpdateMineral();

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };


	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

} ;

#endif //__RYL_MININGAMOUNTDLG_H__