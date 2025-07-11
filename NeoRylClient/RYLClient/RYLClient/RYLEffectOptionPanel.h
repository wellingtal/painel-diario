//================================================================
//
//	Name : RYLEffectOptionPanel.h 
//	Desc : 이펙트 옵션 판넬
//  Date : 2004. 04. 13
//
//================================================================
#ifndef __RYL_EFFECTOPTIONPANEL_H__
#define __RYL_EFFECTOPTIONPANEL_H__

#include "GMUICustomPanel.h"
#include <list>

class CRYLLabel;
class CRYLCheckBox;
class CRYLButton;

class CRYLEffectOptionPanel : public CGMUICustomPanel
{
private :

	enum RYLEFFECTOPTIONPANEL
	{
		// 스킬 이펙트 옵션
		SKILLEFFECT01_BUTTON	= 0, 	// 전체 보기
		SKILLEFFECT02_BUTTON	= 1, 	// 파티것만 보기
		SKILLEFFECT03_BUTTON	= 2, 	// 자기것만 보기
		SKILLEFFECT04_BUTTON	= 3, 	// 자기것만 끄기

		// 챈트 이펙트 옵션
		CHANTEFFECT01_1BUTTON	= 4, 	// 파티 긍정
		CHANTEFFECT02_1BUTTON	= 5, 	// 아군 긍정
		CHANTEFFECT03_1BUTTON	= 6, 	// 중립 긍정
		CHANTEFFECT04_1BUTTON	= 7, 	// 적군 긍정

		CHANTEFFECT01_2BUTTON	= 8, 	// 파티 부정
		CHANTEFFECT02_2BUTTON	= 9, 	// 아군 부정
		CHANTEFFECT03_2BUTTON	= 10,	// 중립 부정
		CHANTEFFECT04_2BUTTON	= 11, 	// 적군 부정

		// 장비품 효과
		EQUIPGRADEEFFECT_BUTTON	= 12,	// 그레이드별 효과 보기

		OK_BUTTON				= 13,
		DEFAULT_BUTTON			= 14,
		ALLOFF_BUTTON			= 15
	};

	CRYLButton*		m_pOkButton;				// 확인
	CRYLButton*		m_pDefaultButton;			// 기본 설정
	CRYLButton*		m_pAlloffButton;			// 다 끄기

	CRYLLabel*		m_pSkillEffectLabel;		// 스킬 이펙트
	CRYLLabel*		m_pChantLabel;				// 챈트, 인챈트 효과
	CRYLLabel*		m_pEquipEffectLabel;		// 장비품 효과

	// 스킬 이펙트 옵션
	CRYLLabel*	m_pSkillEffect01Label;			// 전체 보기
	CRYLLabel*	m_pSkillEffect02Label;			// 파티것만 보기
	CRYLLabel*	m_pSkillEffect03Label;			// 자기것만 보기
	CRYLLabel*	m_pSkillEffect04Label;			// 자기것만 끄기

	// 챈트 이펙트 옵션
	CRYLLabel*	m_pChantEffect01Label;			// 전체 보기
	CRYLLabel*	m_pChantEffect02Label;			// 긍정효과만 보기
	CRYLLabel*	m_pChantEffect03Label;			// 부정효과만 보기
	CRYLLabel*	m_pChantEffect04Label;			// 전체 보기 않음

	CRYLLabel*	m_pChantEffectGoodLabel;		// 긍정효과
	CRYLLabel*	m_pChantEffectBadLabel;			// 부정효과

	// 장비품 효과
	CRYLLabel*	m_pEquipGradeEffectLabel;		// 그레이드별 효과 보기

	// 스킬 이펙트 옵션
	CRYLCheckBox*	m_pSkillEffect01Button;		// 자기 스킬 보기
	CRYLCheckBox*	m_pSkillEffect02Button;		// 아군 스킬 보기
	CRYLCheckBox*	m_pSkillEffect03Button;		// 중립 스킬 보기
	CRYLCheckBox*	m_pSkillEffect04Button;		// 적군 스킬 보기

	// 챈트 이펙트 옵션
	CRYLCheckBox*	m_pChantEffect01_1Button;	// 파티 긍정
	CRYLCheckBox*	m_pChantEffect02_1Button;	// 아군 긍정
	CRYLCheckBox*	m_pChantEffect03_1Button;	// 중립 긍정
	CRYLCheckBox*	m_pChantEffect04_1Button;	// 적군 긍정

	CRYLCheckBox*	m_pChantEffect01_2Button;	// 파티 부정
	CRYLCheckBox*	m_pChantEffect02_2Button;	// 아군 부정
	CRYLCheckBox*	m_pChantEffect03_2Button;	// 중립 부정
	CRYLCheckBox*	m_pChantEffect04_2Button;	// 적군 부정
	
	// 장비품 효과
	CRYLCheckBox*	m_pEquipGradeEffectButton;	// 그레이드별 효과 보기


public :

	CRYLEffectOptionPanel( CGMUICustomDlg *pParentDlg );
	virtual ~CRYLEffectOptionPanel();

	VOID	InitCtrl();
	VOID	FinalCtrl();
	
	HRESULT		Update();

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y );
	static VOID EffectOptionMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );
	VOID	ReLoadOption();
};

#endif //__RYL_EFFECTOPTIONPANEL_H__