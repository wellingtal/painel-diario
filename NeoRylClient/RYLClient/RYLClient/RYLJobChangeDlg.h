//================================================================
//
//	Name : RYLJobChangeDlg.h 
//	Desc : Job Change 다이얼로그
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

	CRYLButton*		m_pWarriorButton ;		// 워리어 
	CRYLButton*		m_pDefenderButton ;		// 디펜더 
	CRYLButton*		m_pClericButton ;		// 클래릭 
	CRYLButton*		m_pPriestButton ;		// 프리스트
	CRYLButton*		m_pSorcererButton ;		// 소서리
	CRYLButton*		m_pEnchanterButton ;	// 인첸터
	CRYLButton*		m_pArcherButton ;		// 아처
	CRYLButton*		m_pAssassinButton ;		// 어쎄신

	CRYLButton*		m_pTemplarButton ;		// 템플러
	CRYLButton*		m_pAttackerButton ;		// 어태커
	CRYLButton*		m_pGunnerButton ;		// 거너
	CRYLButton*		m_pRuneOffButton ;		// 룬오프
	CRYLButton*		m_pLifeOffButton ;		// 라이프오프
	CRYLButton*		m_pShadowOffButton ;	// 쉐도우오프

	CRYLListBox*	m_pJobDataListBox ;		// 직업 설명

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
