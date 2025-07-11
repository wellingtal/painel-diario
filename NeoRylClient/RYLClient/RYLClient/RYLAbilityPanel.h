//================================================================
//
//	Name : RYLAbilityPanel.h 
//	Desc : 스킬 페널
//  Date : 2003. 12. 31
//
//================================================================
#ifndef __RYL_ABILITYPANEL_H__
#define __RYL_ABILITYPANEL_H__

#include "GMUICustomPanel.h"
#include "RYLButton.h"
#include <d3d8.h>
#include <vector>
#include <skill/skillstructure.h>

class CRYLLabel ;
class CRYLSpriteEx ;
class CRYLProgressBar ;
class CItemInstance ;

struct VIRTUAL_ABILITY_SLOT
{
	CItemInstance* lpItem ;

	VIRTUAL_ABILITY_SLOT()
	{
		lpItem = NULL ;
	}
} ;

class CRYLAbilityButton : public CRYLButton
{
private :
	INT			m_iAbilityPosIndex ;
	INT			m_iAbilityIndex ;

public :
	CRYLAbilityButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLAbilityButton() ;

	INT		GetAbilityPosIndex()					{ return m_iAbilityPosIndex ; }
	VOID	SetAbilityPosIndex( INT iIndex )		{ m_iAbilityPosIndex = iIndex ; }

	INT		GetAbilityIndex()					{ return m_iAbilityIndex ; }
	VOID	SetAbilityIndex( INT iIndex )		{ m_iAbilityIndex = iIndex ; }
} ;

class CRYLAbilitySlot : public CGMUICustomPanel
{
public :
	CRYLAbilityButton*	m_pEraseButton ;				// 지움 버튼
	CRYLSpriteEx*		m_pAbilitySprite ;				// 스킬 스프라이트
	CRYLLabel*			m_pAbilityNameLabel ;			// 스킬 이름
	CRYLLabel*			m_pAbilityLevelLabel ;			// 스킬 레벨
	CRYLProgressBar*	m_pAbilityProgrss ;				// 스킬 프로그래스바
	CRYLProgressBar*	m_pMasterAbilityProgrss ;		// 스킬 마스터 프로그래스바

	INT					m_iAbilitySlotIndex ;			// 스킬 인덱스
	
	unsigned short		m_wAbilityID;
	unsigned short		m_wAbilityLV;
public :
	CRYLAbilitySlot( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLAbilitySlot() ;

	VOID	MakeAbilitySlot( INT iAbilityKind ) ;
	VOID	CreateAbilityName( LPSTR lpAbilityName ) ;
	VOID	CreateAbilityLevel( LPSTR lpAbilityName ) ;
	VOID	CreateAbilityProgrssBar( INT iLevel ) ;
	VOID	CreateMasterAbilityProgrssBar() ;

	VOID	SetAbilitySlotIndex( INT iIndex )				{ m_iAbilitySlotIndex = iIndex ; }
	INT		GetAbilitySlotIndex()							{ return m_iAbilitySlotIndex ; }
	VOID	SetAbilityLevel( DWORD dwLevel ) ;
	VOID	SetAbilityName( LPSTR lpAbilityName ) ;
	VOID	SetAbilityLockCount( INT iCount ) ;
} ;


//========================================================
namespace RYLABILITYPANEL
{
	const DWORD			PREVABILITY_BUTTON		= 0x00000000 ;
	const DWORD			NEXTABILITY_BUTTON		= 0x00000001 ;
	const DWORD			ERASE_BUTTON			= 0x00000002 ;
	const DWORD			ABILITYSLOT_PANEL		= 0x00000003 ;
} ;

enum ABILITYEDITSLOT { EDIT_ABILITY = 0, DEL_ABILITYSLOT } ;
enum ABILITYKIND { GENERALABILITY} ;

class CRYLAbilityPanel : public CGMUICustomPanel
{
private :

	enum Const
	{
		MAX_SLOT_PER_PAGE		= 9			// 페이지 당 최대 슬롯 수
	};

	std::vector< CRYLAbilitySlot* >	m_vecAbilitySlot ;
	CRYLButton*						m_pPrevAbilityButton ;
	CRYLButton*						m_pNextAbilityButton ;
	CRYLLabel*						m_pAbilityPointLabel ;// 스킬 레벨
	CRYLLabel*						m_pAbilityPointValueLabel ;// 스킬 레벨 값
	
	INT					m_iAbilityYMargin ;
	DWORD				m_dwStartSlot ;

	DWORD				m_dwProcessAbilityIndex ;		// 화면에서의 위치
	DWORD				m_dwProcessAbilityPosIndex;		// SkillSlot에서의 위치

	unsigned char		m_cAbilitySlotIndex ;			// 어빌리티가 위치한 위치슬롯
	
	// 망각의돌 아이템
	CItemInstance*		m_lpStatusRetrainItem;

public :

	DWORD				m_dwEraseResult ;
	

public :

	CRYLAbilityPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLAbilityPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;


	VOID		InsertAbilitySlot( CItemInstance* lpAbility, 
								 unsigned short wAbilityID, 
								 unsigned char cGrade, 
								 unsigned char cLevel, 
								 unsigned char cPos, 
								 unsigned char cIndex)  ;

	VOID		RenderAbilitySlot( LPDIRECT3DDEVICE8 lpD3DDevice, 
								 VIRTUAL_ABILITY_SLOT* lpSlot, 
								 LONG lMoveX, LONG lMoveY,
								 bool bAlterAbility = false ) ;

	VOID		SetProcessAbilityIndex( INT iIndex )			{ m_dwProcessAbilityIndex = iIndex ; }
	VOID		SetProcessAbilityPosIndex( INT iIndex )			{ m_dwProcessAbilityPosIndex = iIndex ; }
	
	VOID		SetAbilitySlot( CItemInstance* lpAbility, INT iKInd, int iIndex = 0 )  ;
	VOID		DeleteCurrentAbilitySlot() ;
	VOID		UpdateAbilitySlotPosition() ;

	VOID		SetStartSlot( INT iStart )			{ m_dwStartSlot = iStart ; }
	INT			GetStartSlot()						{ return m_dwStartSlot ; }

	INT			GetSlotCount()						{ return m_cAbilitySlotIndex ; }
	VOID		AbilitySlotVisible() ;
	VOID		PrevAbilitySlotPage() ;
	VOID		NextAbilitySlotPage() ;

	VOID		ClearAbilitySlot() ;
	void		DeleteAbilitySlot( CRYLAbilitySlot* lpAbilitySlot ) ;

	BOOL		CheckAbility(unsigned short wAbilityID, unsigned short wAbilityLV);


public :

	//==========================================================================
	// virtual skill slot
	//==========================================================================
	std::vector< VIRTUAL_ABILITY_SLOT * >		m_lstDisplayAbilitySlot ;	

	VOID	InsertVirtualAbility( CItemInstance* lpItem) ;
	
	CItemInstance*	FindVirtualAbility( unsigned short wAbilityID ) ;
	unsigned short	FindVirtualIndex( unsigned short wAbilityID ) ;
	VOID	DelVirtualAbility( unsigned short wAbilityID ) ;
	VOID	ClearVirtualAbility() ;
	void	SetBindingItem( CItemInstance* lpItem, int iIndex ) ;
} ;

#endif //__RYL_ABILITYPANEL_H__