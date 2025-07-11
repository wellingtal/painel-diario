//=================================================================
//
// Name : RYLCastleMineralDlg.h
// Data : 2005. 4. 25
// Desc : ���� �������� ���� ���� Ȯ�� ���̾�α�
//
//=================================================================


#ifndef __RYL_CASTLEMINERALDLG_H__
#define __RYL_CASTLEMINERALDLG_H__

#include <Network/Packet/PacketStruct/CastlePacket.h>

class CRYLButton ;
class CRYLLabel ;

namespace RYLCASTLEMINERALDLG
{
	const DWORD		PREV_BUTTON				= 0x00000000 ;
	const DWORD		NEXT_BUTTON				= 0x00000001 ;
	const DWORD		GET_BUTTON_0			= 0x00000002 ;
	const DWORD		GET_BUTTON_1			= 0x00000003 ;
	const DWORD		GET_BUTTON_2			= 0x00000004 ;
	const DWORD		GET_BUTTON_3			= 0x00000005 ;
	const DWORD		GET_BUTTON_4			= 0x00000006 ;
	const DWORD		GET_BUTTON_5			= 0x00000007 ;
	const DWORD		GET_BUTTON_6			= 0x00000008 ;
	const DWORD		GET_BUTTON_7			= 0x00000009 ;
} ;

class CRYLCastleMineralDlg : public CRYLDialog
{
private : 

	CRYLButton*			m_pPrevButton ;							// ������ �̵� ��ư
	CRYLButton*			m_pNextButton ;							// ������ �̵� ��ư
	CRYLButton*			m_pGetMineButton[8] ;					// ���� ȹ�� ��ư

	CRYLLabel*			m_pPhaseLabel ;							// ������ ��
	CRYLLabel*			m_pMineNameLabel[8] ;					// ���� �̸�
	CRYLLabel*			m_pMineCountLabel[8] ;					// ���� ����
	CRYLLabel*			m_pTitleLabel ;							// ������ ���Է� Ȯ�� ( Ÿ��Ʋ )
	std::vector< CItemInstance* >	m_vecMineIcon ;				// ���� ������
	std::vector< int >	m_vecMineCnt;							// ���� ����

	int					m_nCurPhase ;							// phase 
	int					m_nMaxPhase ;							// ������ �ƽ� ��.

	DWORD				m_dwSplitResult ;
	DWORD				m_dwSplitValue ;
	DWORD				m_dwSplitIndex ;
	DWORD				m_dwCastleID ;							// �� ���̵�

protected:
	VOID				NextPhase();
	VOID				PrevPhase();
	VOID				SetPhaseLabel();
	VOID				GetMine( int index );					//���� ȹ�� ��ư ( 1~8 )
	VOID				AddItem( unsigned short id, unsigned short amount );
	VOID				ClearItem();

public :
	CRYLCastleMineralDlg() ;
	virtual ~CRYLCastleMineralDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	SetCastle( DWORD dwCastleID )		{ m_dwCastleID = dwCastleID; };
	VOID	InitMineral();
	VOID	SetMineral( unsigned long dwCastleID, unsigned char cNum, CastleMineral* lpMineralList );
	BOOL	RemoveItem( unsigned short id, unsigned short amountleft );

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif //__RYL_CASTLEMINERALDLG_H__
