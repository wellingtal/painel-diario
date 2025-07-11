#ifndef __RYLABILITYDESCDLG_H__
#define __RYLABILITYDESCDLG_H__

#include "RYLDialog.h"
#include "RYLImage.h"
#include "RYLLabel.h"

class CRYLListBox ;

namespace RYLABILITYDESCDLG
{
	const DWORD		ABILITYDESCENT_LISTBOX					= 0x00000000 ;
	const DWORD		ABILITYDESCENTDESC_LISTBOX				= 0x00000001 ;
	const DWORD		ABILITYDETAIL_LISTBOX						= 0x00000002 ;
	const DWORD		ABILITYSUBDESC_LISTBOX					= 0x00000003 ;
} ;

class CRYLAbilityDescDlg : public CRYLDialog
{
public :

	static bool				ms_bSameSkillTooltip ;
	static unsigned short	ms_wSkillID ;
	static unsigned char	ms_cSkillStep ;
	static unsigned char	ms_cSkillLevel ;
	static unsigned char    ms_cWeaponType ;

	CRYLImage*				m_pSkillFrameImage ;

	CRYLLabel*				m_pSkillKindLabel ;		// 스킬 종류
	CRYLLabel*				m_pSkillDescentLabel1 ;	// 스킬 계열
	CRYLLabel*				m_pSkillDescentLabel2 ;	
	CRYLLabel*				m_pSkillDetailLabel ;	

	CRYLLabel*				m_pAltSkillDescLabel ;
	
	CRYLListBox*			m_pSkillDescentListBox ;
	CRYLListBox*			m_pSkillDescentDescListBox ;
	CRYLListBox*			m_pSkillDetailListBox ; 
	CRYLListBox*			m_pSkillSubDescListBox ;

	unsigned int			m_uiDlgHeight ;


public :

	CRYLAbilityDescDlg() ;
	virtual ~CRYLAbilityDescDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )	;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	void				RestoreAbilityUI( unsigned short wSkillID, unsigned char cSkillStep, unsigned char cSkillLevel, unsigned char cWeaponType ) ;
	void				RestorePaint() ;
	bool				RestoreTooltip( unsigned short wSkillID ) ;

	void				SetDetailLabelCaption( LPSTR lpStr ) { m_pSkillDetailLabel->SetCaption( lpStr ) ; }

	static bool			GetSameAbilityTooltip( unsigned char cSkillStep, unsigned char cSkillLevel, unsigned char cWeaponType )			
	{
		// 같은 스킬이지만 스킬 단계가 다를땐 false를 반환한다.
		if ( cSkillStep == ms_cSkillStep && cSkillLevel == ms_cSkillLevel && cWeaponType == ms_cWeaponType )
			return ms_bSameSkillTooltip ; 
		else
			return false ;
	}
	static void			SetSameAbilityTooltip( unsigned short wSkillID )
	{
		if ( ms_wSkillID == wSkillID )
			ms_bSameSkillTooltip = true ;
		else
			ms_bSameSkillTooltip = false ;

		ms_wSkillID = wSkillID ;
	}
} ;

extern CRYLAbilityDescDlg*	g_pAbilityDescDlg ;

#endif // __RYLABILITYDESCDLG_H__