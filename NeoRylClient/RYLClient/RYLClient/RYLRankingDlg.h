//================================================================
//
//	Name : RYLQuestRankingDlg.h 
//	Desc : ����Ʈ ������ ���̾�α�
//  Date : 2004. 01. 05
//
//================================================================
#ifndef __RYL_RANKINGDLG_H__
#define __RYL_RANKINGDLG_H__

#include "RYLDialog.h"
#include <vector>

class CRYLButton ;
class CRYLStringGrid ;
class CRYLLabel ;
class CRYLComboBox ;
struct RankingNode ;


namespace RYLRANKINGDLG
{
	const DWORD		PREVRANK_BUTTON		= 0x00000000 ;
	const DWORD		NEXTRANK_BUTTON		= 0x00000001 ;
	const DWORD		RANKING_COMBO		= 0x00000002 ;
	const DWORD		RANKING_MENU		= 0x00000003 ;
	const DWORD		RANKING_LISTBOX		= 0x00000004 ;
} ;

class CRYLRankingDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pRankingDlgLabel ;				// ��ŷâ
	CRYLLabel*			m_pServerNameLabel ;				// �����̸�
	CRYLLabel*			m_pServerNameValueLabel ;			// �����̸� ��
	CRYLLabel*			m_pCurrentRankingLabel ;			// ���� ��ŷ
	CRYLLabel*			m_pCurrentRankingValueLabel ;		// ���� ��ŷ��
	CRYLLabel*			m_pSortLabel ;						// ���� ���
	
	CRYLLabel*			m_pRankingLabel ;					// ����
	CRYLLabel*			m_pClassLabel ;						// Ŭ����
	CRYLLabel*			m_pNameLabel ;						// �̸�
	CRYLLabel*			m_pPointLabel ;						// ����Ʈ
	CRYLLabel*			m_pLebelLabel ;						// ����

	CRYLButton*			m_pPrevButton ;						// ���� ������
	CRYLButton*			m_pNextButton ;						// ���� ������
	CRYLStringGrid*		m_pRankingStrGrid ;					// ��ŷ ����Ʈ
	CRYLComboBox*		m_pClassCBBox ;						// Ŭ���� �޺��ڽ�

	unsigned char		m_cCurrentPage ;					// ���� ������
	INT					m_iClass ;							// �������� Ŭ����

public :
	// TODO : �ϳ��� ���ͷ� �����ص� �� �� �մϴ�. 
	//	���� �Ѹ��� �������� ������ �ν��Ͻ���� �������� ������. (2004-02-09 �ε�)

	std::vector<RankingNode>			m_vecAll ;			// ��ü ����Ʈ
	std::vector<RankingNode>			m_vecDefender ;		// ����� ����Ʈ
	std::vector<RankingNode>			m_vecWarrior ;		// ������ ����Ʈ
	std::vector<RankingNode>			m_vecAssassin ;		// ���� ����Ʈ
	std::vector<RankingNode>			m_vecArcher ;		// ��ó ����Ʈ
	std::vector<RankingNode>			m_vecSorcerer ;		// �Ҽ��� ����Ʈ
	std::vector<RankingNode>			m_vecEnchanter ;	// ��þ�� ����Ʈ
	std::vector<RankingNode>			m_vecPriest ;		// ������Ʈ ����Ʈ
	std::vector<RankingNode>			m_vecCleric ;		// Ŭ���� ����Ʈ

	std::vector<RankingNode>			m_vecTemplar ;		// ���÷� ����Ʈ
	std::vector<RankingNode>			m_vecAttacker ;		// ����Ŀ ����Ʈ
	std::vector<RankingNode>			m_vecGunner ;		// �ų�	����Ʈ
	std::vector<RankingNode>			m_vecRuneOff ;		// ����� ����Ʈ
	std::vector<RankingNode>			m_vecLifeOff ;		// ���������� ����Ʈ
	std::vector<RankingNode>			m_vecShadowOff ;	// ��������� ����Ʈ
	
public :
	CRYLRankingDlg() ;
	virtual ~CRYLRankingDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } 
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
    virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	InsertRanking( INT iClass, INT iPage, INT iNum, RankingNode* lpRankingNode ) ;
	VOID	InsertDataListbox( INT iClass ) ;

	unsigned char	GetCurrentPage( VOID ) { return m_cCurrentPage ; }
	VOID			SetCurrentPage( unsigned char cPage ) { m_cCurrentPage = cPage ; }
} ;

#endif // __RYL_RANKINGDLG_H__