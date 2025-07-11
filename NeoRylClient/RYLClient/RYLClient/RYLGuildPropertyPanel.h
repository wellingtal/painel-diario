//================================================================
//
//	Name : RYLPropertyPanel.h 
//	Desc : ���ݰ� �ǳ�
//  Date : 2003. 12. 23
//
//================================================================
#ifndef __RYL_GUILDPROPERTYPANEL_H__
#define __RYL_GUILDPROPERTYPANEL_H__

#include "GMUICustomPanel.h"
#include <Community/Guild/GuildConstants.h>
#include <d3d8.h>

class CRYLLabel ;
class CRYLButton ;
class CRYLEdit ;

class CRYLListBox ;
class CRYLSpriteEx ;
class CRYLLabel ;
class CRYLBitmap ;
class CRYLStringGrid ;
class CRYLGuildInfoPanel;

namespace RYLGUILDPROPERTYPANEL
{
	const DWORD		PAYMENTGOLD_BUTTON				= 0x00000000 ;
	const DWORD		DEPOSITGOLD_BUTTON				= 0x00000001 ;
	const DWORD		DIVISIONGOLD_BUTTON				= 0x00000002 ;
	const DWORD		USEINQUIRY_BUTTON				= 0x00000003 ;

	const DWORD		GUILDMARK_BUTTON				= 0x00000004 ;
	const DWORD		GUILDLEVEL_BUTTON				= 0x00000005 ;

	/*
	enum	INCLINATION
	{
		HUMAN_KARTERANT_WAR			= Guild::RACE_HUMAN | Guild::NATION_KARTERANT 	| Guild::MODE_WAR ,
		HUMAN_MERKADIA_WAR			= Guild::RACE_HUMAN | Guild::NATION_MERKADIA  	| Guild::MODE_WAR ,
		HUMAN_NATION_THIRD_WAR		= Guild::RACE_HUMAN | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		HUMAN_NATION_THIRD_PEACE	= Guild::RACE_HUMAN | Guild::NATION_THIRD		| Guild::MODE_PEACE ,

		AKHAN_KARTERANT_WAR			= Guild::RACE_AKHAN | Guild::NATION_KARTERANT 	| Guild::MODE_WAR ,
		AKHAN_MERKADIA_WAR			= Guild::RACE_AKHAN | Guild::NATION_MERKADIA  	| Guild::MODE_WAR ,
		AKHAN_NATION_THIRD_WAR		= Guild::RACE_AKHAN | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		AKHAN_NATION_THIRD_PEACE	= Guild::RACE_AKHAN | Guild::NATION_THIRD		| Guild::MODE_PEACE ,

		BOTH_KARTERANT_WAR			= Guild::RACE_BOTH  | Guild::NATION_KARTERANT	| Guild::MODE_WAR ,
		BOTH_MERKADIA_WAR			= Guild::RACE_BOTH  | Guild::NATION_MERKADIA	| Guild::MODE_WAR ,
		BOTH_NATION_THIRD_WAR		= Guild::RACE_BOTH  | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		BOTH_NATION_THIRD_PEACE		= Guild::RACE_BOTH  | Guild::NATION_THIRD		| Guild::MODE_PEACE 
	};
	*/
} ;


class CRYLGuildPropertyPanel : public CGMUICustomPanel
{
public :

	// -----------------------------------------------------------------------------------
	// ���ݰ� / â�����

	CRYLLabel*			m_pStorehouseLabel ;					// ���ݰ� / â�����
	CRYLLabel*			m_pGoldManagementLabel ;				// ���ݰ����
	CRYLLabel*			m_pCurrentGoldLabel	;					// ���� �ܰ�
	CRYLLabel*			m_pCurrentGoldValueLabel ;				// ���� �ܰ� ��
	CRYLButton*			m_pPaymentGoldButton ;					// ���
	CRYLButton*			m_pDepositGoldButton ;					// �Ա�
	CRYLButton*			m_pDivisionGoldButton ;					// �й�
	CRYLButton*			m_pUseInquiryButton ;					// ��볻�� ��ȸ

	// -----------------------------------------------------------------------------------
	// ��ũ���� / �ο�������

	CRYLLabel*			m_pMarkNMemberNumLabel ;				// ��ũ���� / �ο�������
	CRYLLabel*			m_pMarkLabel ;							// ��� ��ũ ����
	CRYLLabel*			m_pCurrentMarkLabel ;					// ��ũ ��Ȳ
	CRYLSpriteEx*		m_pCurrentMarkSprite ;					// ��� ��ũ ��������Ʈ
	CRYLButton*			m_pChangeMarkButton ;					// ��ũ ���� ��ư

	CRYLLabel*			m_pMemberNumLabel ;						// ��� �ִ� �ο��� ����
	CRYLLabel*			m_pCurrentMemberNumLabel ;				// ���� �ִ� ���� �ο�
	CRYLLabel*			m_pCurrentMemberNumValueLabel ;			// ���� �ִ� ���� �ο� ��
	CRYLButton*			m_pChangeMemberNumButton ;				// �ο� ���� ��ư

	DWORD				m_dwCurrentMemberNum ;					// ���� ��� ��� ��
	DWORD				m_dwGuildLevel ;						// ���� ��� ����
	DWORD				m_dwGuildFame ;							// ���� ��� ��

	DWORD				m_dwGuildMarkResult ;					// ��� ��ũ ���� ���
	DWORD				m_dwGuildLevelResult ;					// ��� ����(�ο� ����) ���� ���

	// -----------------------------------------------------------------------------------
	// ����

	unsigned char		m_ucInclination;						// �ڽ��� ��� ����
	unsigned char		m_ucSelectInclination;					// ���õ� ��� ����


	DWORD		m_dwRstInputGold ;
	DWORD		m_dwValInputGold ;

	DWORD		m_dwRstOutputGold ;
	DWORD		m_dwValOutputGold ;

	DWORD		m_dwRstEmissionGold ;
	DWORD		m_dwValEmissionGold ;

	ULONG		m_iCurrentGold ;

	bool		m_bMessageBox ;


public :

	CRYLGuildPropertyPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLGuildPropertyPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	Hide( CGMUIObject* Sender ) ;

	VOID	CheckRightButton() ;
	VOID	SetCurrentGold( INT iGold )		{ m_iCurrentGold = iGold ; }
	ULONG	GetCurrentGold()				{ return m_iCurrentGold ; }
	ULONG	GetGuildGold()					{ return m_iCurrentGold ; }

	VOID	PaymentGold() ;						// ���
	VOID	DepositGold() ;						// �Ա�
	VOID	DivisionGold() ;					// �й�
	VOID	SetSafeAndMyGold( DWORD dwCID, char* szName, DWORD dwSafeGold, DWORD dwCharGold, DWORD dwCmd )  ;

	VOID	UpdateCheckPropertyMsgBox() ;
	
	VOID			SetInclination( unsigned char type ) ;			// �ڽ��� ��� ���� ����
	unsigned char	GetInclination();
	unsigned char	GetSelectInclination();

	
	VOID	Clear() ;

	VOID	SetGuildMark( CRYLSprite* pMarkSprite ) ;
	VOID	SetGuildMemberLevelFame( DWORD dwCurrentMemberNum, DWORD dwLevel, DWORD dwFame ) ;

	VOID	ChangeGuildMarkButtonEvent() ;			// ��� ��ũ ����
	VOID	ChangeGuildLevelButtonEvent() ;			// ��� �ο� ����

	static VOID	SetNation( unsigned char cNation );
} ;

#endif //__RYL_GUILDPROPERTYPANEL_H__
