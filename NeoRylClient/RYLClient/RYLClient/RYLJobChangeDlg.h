//================================================================
//
//	Name : RYLJobChangeDlg.h 
//	Desc : Job Change ���̾�α�
//  Date : 2004. 1. 18
//
//================================================================
#ifndef __RYL_JOBCHANGEDLG_H__
#define __RYL_JOBCHANGEDLG_H__

#include "RYLDialog.h"
#include <vector>

class CRYLLabel ;
class CRYLButton ;
class CRYLListBox ;

namespace RYLJOBCHANGEDLG
{
	const DWORD		WARRIOR_BUTTON					= 0x00000000 ;
	const DWORD		DEFENDER_BUTTON					= 0x00000001 ;
	const DWORD		CLERIC_BUTTON					= 0x00000002 ;
	const DWORD		PRIEST_BUTTON					= 0x00000003 ;
	const DWORD		SORCERER_BUTTON					= 0x00000004 ;
	const DWORD		ENCHANTER_BUTTON				= 0x00000005 ;
	const DWORD		ARCHER_BUTTON					= 0x00000006 ;
	const DWORD		ASSASSIN_BUTTON					= 0x00000007 ;

	const DWORD		TEMPLAR_BUTTON					= 0x00000008 ;
	const DWORD		ATTACKER_BUTTON					= 0x00000009 ;
	const DWORD		GUNNER_BUTTON					= 0x00000010 ;
	const DWORD		RUNEOFF_BUTTON					= 0x00000011 ;
	const DWORD		LIFEOFF_BUTTON					= 0x00000012 ;
	const DWORD		SHADOWOFF_BUTTON				= 0x00000013 ;
} ;

class CRYLJobChangeDlg : public CRYLDialog
{
private :
	typedef struct Message
	{
		CHAR	strMessage[ MAX_PATH ] ;
	} Message, *LPMessage ;
	std::vector<LPMessage> m_lstMessage ;

	CRYLLabel*		m_pChangeJobLabel ;

	CRYLButton*		m_pWarriorButton ;		// ������ 
	CRYLButton*		m_pDefenderButton ;		// ����� 
	CRYLButton*		m_pClericButton ;		// Ŭ���� 
	CRYLButton*		m_pPriestButton ;		// ������Ʈ
	CRYLButton*		m_pSorcererButton ;		// �Ҽ���
	CRYLButton*		m_pEnchanterButton ;	// ��þ��
	CRYLButton*		m_pArcherButton ;		// ��ó
	CRYLButton*		m_pAssassinButton ;		// ����

	CRYLButton*		m_pTemplarButton ;		// ���÷�
	CRYLButton*		m_pAttackerButton ;		// ����Ŀ
	CRYLButton*		m_pGunnerButton ;		// �ų�
	CRYLButton*		m_pRuneOffButton ;		// �����
	CRYLButton*		m_pLifeOffButton ;		// ����������
	CRYLButton*		m_pShadowOffButton ;	// ���������

	CRYLListBox*	m_pJobDataListBox ;		// ���� ����

	ULONG			m_dwChangeResult ;
	USHORT			m_wClassChange ;

	
public :
	CRYLJobChangeDlg() ;
	virtual ~CRYLJobChangeDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	VOID	SetText( LPSTR strMessage ) ;
	VOID	ClearText() ;
	VOID	JobChangeMsgBox( INT iJob ) ;
} ;

#endif // __RYL_JOBCHANGEDLG_H__
