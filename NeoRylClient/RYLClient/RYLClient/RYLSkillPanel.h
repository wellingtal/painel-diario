//================================================================
//
//	Name : RYLSkillPanel.h 
//	Desc : ��ų ���
//  Date : 2003. 12. 31
//
//================================================================
#ifndef __RYL_SKILLPANEL_H__
#define __RYL_SKILLPANEL_H__

#include "GMUICustomPanel.h"
#include "RYLButton.h"
#include <d3d8.h>
#include <vector>
#include <skill/skillstructure.h>

class CRYLLabel ;
class CRYLSpriteEx ;
class CRYLProgressBar ;
class CItemInstance ;

enum SKILL_KIND 
{ 
	CAST_SKILL		= 0, 
	CHANT_SKILL		= 1, 
	PASSIVE_SKILL	= 2, 
	INSTANT_SKILL	= 3 
} ;

struct VIRTUAL_SKILL_SLOT
{
	CItemInstance* lpItem ;
	bool		   bChild ;

	VIRTUAL_SKILL_SLOT()
	{
		lpItem = NULL ;
		bChild = false ;
	}
} ;

class CRYLSkillButton : public CRYLButton
{
private :
	INT			m_iSkillIndex ;

public :
	CRYLSkillButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLSkillButton() ;

	INT		GetSkillIndex()					{ return m_iSkillIndex ; }
	VOID	SetSkillIndex( INT iIndex )		{ m_iSkillIndex = iIndex ; }
} ;

class CRYLSkillSlot : public CGMUICustomPanel
{
public :
	CRYLSkillButton*	m_pEraseButton ;				// ���� ��ư
	CRYLSpriteEx*		m_pSkillSprite ;				// ��ų ��������Ʈ
	CRYLLabel*			m_pSkillNameLabel ;				// ��ų �̸�
	CRYLLabel*			m_pSkillLevelLabel ;			// ��ų ����
	CRYLProgressBar*	m_pSkillProgrss ;				// ��ų ���α׷�����
	CRYLProgressBar*	m_pMasterSkillProgrss ;			// ��ų ������ ���α׷�����

	INT					m_iSkillSlotIndex ;				// ��ų �ε���
	DWORD				m_dwSkillClass ;				// class skill, normal skill, alternative skill
	
public :
	CRYLSkillSlot( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLSkillSlot() ;

	VOID	MakeSkillSlot( INT iSkillKind, UINT uiKind ) ;
	VOID	MakeAlterSkillSlot( INT iSkillKind, UINT uiKind ) ;
	VOID	CreateSkillName( LPSTR lpSkillName, bool bAlterSkill = false ) ;
	VOID	CreateSkillLevel( LPSTR lpSkillName ) ;
	VOID	CreateSkillProgrssBar( INT iLevel ) ;
	VOID	CreateMasterSkillProgrssBar() ;

	VOID	SetSkillSlotIndex( INT iIndex )				{ m_iSkillSlotIndex = iIndex ; }
	INT		GetSkillSlotIndex()							{ return m_iSkillSlotIndex ; }
	VOID	SetSkillLevel( DWORD dwLevel ) ;
	VOID	SetSkillName( LPSTR lpSkillName ) ;
	VOID	SetSkillLockCount( INT iCount ) ;
} ;


//========================================================
namespace RYLSKILLPANEL
{
	const DWORD			PREVSKILL_BUTTON		= 0x00000000 ;
	const DWORD			NEXTSKILL_BUTTON		= 0x00000001 ;
	const DWORD			QUICKSLOT_BUTTON		= 0x00000002 ;
	const DWORD			ERASE_BUTTON			= 0x00000003 ;
	const DWORD			SKILLSLOT_PANEL			= 0x00000004 ;
} ;

enum EDITSLOT { EDIT_SKILL = 0, DEL_SKILLSLOT } ;
enum SKILLKIND { CLASSSKILL = 0, GENERALSKILL, ALTERNATIVESKILL } ;

class CRYLSkillPanel : public CGMUICustomPanel
{
private :

	enum Const
	{
		MAX_SLOT_PER_PAGE		= 9			// ������ �� �ִ� ���� ��
	};

	std::vector< CRYLSkillSlot* >	m_vecSkillSlot ;
	CRYLButton*						m_pPrevSkillButton ;
	CRYLButton*						m_pNextSkillButton ;
	CRYLButton*						m_pQuickSlotButton ;
	CRYLLabel*						m_pSkillPointLabel ;// ��ų ����
	CRYLLabel*						m_pSkillPointValueLabel ;// ��ų ���� ��
	

	INT					m_iSkillYMargin ;
	DWORD				m_dwStartSlot ;
	DWORD				m_dwProcessSkillIndex ;
	unsigned char		m_cSkillButtonIndex ;			// ��ų ��ư �ε���
	unsigned char		m_cSkillSlotIndex ;
	
	// ���� ���õ� ��ų ���̿� ���� �ִ� ���ͳ�Ƽ�� ��ų ����
	unsigned char		m_cTmpAddAlterSkillNum ;		


	// �����ǵ� ������
	CItemInstance*		m_lpStatusRetrainItem;

public :

	DWORD				m_dwEraseResult ;
	

public :

	CRYLSkillPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLSkillPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;


	VOID		InsertSkillSlot( CItemInstance* lpSkill, 
								 unsigned short wSkillID, 
								 unsigned char cGrade, 
								 unsigned char cLevel, 
								 unsigned char cPos, 
								 unsigned char cIndex, 
								 UINT uiKind )  ;

	VOID		InsertAlterSkillSlot( CItemInstance* pItem, 
									  unsigned short wSkillID, 
									  unsigned char cPos, 
									  UINT uiKind )  ;

	VOID		RenderClassSlot( LPDIRECT3DDEVICE8 lpD3DDevice, CItemInstance *lpSkill, LONG lSlot, 
							     LONG lMoveX, LONG lMoveY ) ;
	VOID		RenderSkillSlot( LPDIRECT3DDEVICE8 lpD3DDevice, 
								 VIRTUAL_SKILL_SLOT* lpSlot, 
								 LONG lMoveX, LONG lMoveY,
								 bool bAlterSkill = false ) ;

	VOID		SetProcessSkillIndex( INT iIndex )			{ m_dwProcessSkillIndex = iIndex ; }
	
	VOID		SetSkillSlot( CItemInstance* lpSkill, INT iKInd, INT iIndex = 0, bool bAlter = false )  ;
	VOID		DeleteCurrentSkillSlot() ;
	VOID		UpdateSkillSlotPosition() ;

	VOID		SetStartSlot( INT iStart )			{ m_dwStartSlot = iStart ; }
	INT			GetStartSlot()						{ return m_dwStartSlot ; }

	INT			GetSlotCount()						{ return m_cSkillSlotIndex ; }
	VOID		SkillSlotVisible() ;
	VOID		PrevSkillSlotPage() ;
	VOID		NextSkillSlotPage() ;

	VOID		ClearSkillSlot() ;
	void		DeleteSkillSlot( CRYLSkillSlot* lpSkillSlot ) ;


public :

	//==========================================================================
	// virtual skill slot
	//==========================================================================
	std::vector< VIRTUAL_SKILL_SLOT * >		m_lstDisplaySkillSlot ;	

	VOID	InsertVirtualSkill( CItemInstance* lpItem, bool bChild ) ;
	
	CItemInstance*	FindVirtualSkill( unsigned short wSkillID ) ;
	unsigned short	FindVirtualIndex( unsigned short wSkillID ) ;
	VOID	DelVirtualSkill( unsigned short wSkillID ) ;
	VOID	ClearVirtualSkill() ;
	void	SetBindingItem( CItemInstance* lpItem, int iIndex ) ;

	unsigned char GetAlterSkillNum( int iIdx ) ;
} ;

#endif //__RYL_SKILLPANEL_H__