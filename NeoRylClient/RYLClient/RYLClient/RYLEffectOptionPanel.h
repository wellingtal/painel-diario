//================================================================
//
//	Name : RYLEffectOptionPanel.h 
//	Desc : ����Ʈ �ɼ� �ǳ�
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
		// ��ų ����Ʈ �ɼ�
		SKILLEFFECT01_BUTTON	= 0, 	// ��ü ����
		SKILLEFFECT02_BUTTON	= 1, 	// ��Ƽ�͸� ����
		SKILLEFFECT03_BUTTON	= 2, 	// �ڱ�͸� ����
		SKILLEFFECT04_BUTTON	= 3, 	// �ڱ�͸� ����

		// æƮ ����Ʈ �ɼ�
		CHANTEFFECT01_1BUTTON	= 4, 	// ��Ƽ ����
		CHANTEFFECT02_1BUTTON	= 5, 	// �Ʊ� ����
		CHANTEFFECT03_1BUTTON	= 6, 	// �߸� ����
		CHANTEFFECT04_1BUTTON	= 7, 	// ���� ����

		CHANTEFFECT01_2BUTTON	= 8, 	// ��Ƽ ����
		CHANTEFFECT02_2BUTTON	= 9, 	// �Ʊ� ����
		CHANTEFFECT03_2BUTTON	= 10,	// �߸� ����
		CHANTEFFECT04_2BUTTON	= 11, 	// ���� ����

		// ���ǰ ȿ��
		EQUIPGRADEEFFECT_BUTTON	= 12,	// �׷��̵庰 ȿ�� ����

		OK_BUTTON				= 13,
		DEFAULT_BUTTON			= 14,
		ALLOFF_BUTTON			= 15
	};

	CRYLButton*		m_pOkButton;				// Ȯ��
	CRYLButton*		m_pDefaultButton;			// �⺻ ����
	CRYLButton*		m_pAlloffButton;			// �� ����

	CRYLLabel*		m_pSkillEffectLabel;		// ��ų ����Ʈ
	CRYLLabel*		m_pChantLabel;				// æƮ, ��æƮ ȿ��
	CRYLLabel*		m_pEquipEffectLabel;		// ���ǰ ȿ��

	// ��ų ����Ʈ �ɼ�
	CRYLLabel*	m_pSkillEffect01Label;			// ��ü ����
	CRYLLabel*	m_pSkillEffect02Label;			// ��Ƽ�͸� ����
	CRYLLabel*	m_pSkillEffect03Label;			// �ڱ�͸� ����
	CRYLLabel*	m_pSkillEffect04Label;			// �ڱ�͸� ����

	// æƮ ����Ʈ �ɼ�
	CRYLLabel*	m_pChantEffect01Label;			// ��ü ����
	CRYLLabel*	m_pChantEffect02Label;			// ����ȿ���� ����
	CRYLLabel*	m_pChantEffect03Label;			// ����ȿ���� ����
	CRYLLabel*	m_pChantEffect04Label;			// ��ü ���� ����

	CRYLLabel*	m_pChantEffectGoodLabel;		// ����ȿ��
	CRYLLabel*	m_pChantEffectBadLabel;			// ����ȿ��

	// ���ǰ ȿ��
	CRYLLabel*	m_pEquipGradeEffectLabel;		// �׷��̵庰 ȿ�� ����

	// ��ų ����Ʈ �ɼ�
	CRYLCheckBox*	m_pSkillEffect01Button;		// �ڱ� ��ų ����
	CRYLCheckBox*	m_pSkillEffect02Button;		// �Ʊ� ��ų ����
	CRYLCheckBox*	m_pSkillEffect03Button;		// �߸� ��ų ����
	CRYLCheckBox*	m_pSkillEffect04Button;		// ���� ��ų ����

	// æƮ ����Ʈ �ɼ�
	CRYLCheckBox*	m_pChantEffect01_1Button;	// ��Ƽ ����
	CRYLCheckBox*	m_pChantEffect02_1Button;	// �Ʊ� ����
	CRYLCheckBox*	m_pChantEffect03_1Button;	// �߸� ����
	CRYLCheckBox*	m_pChantEffect04_1Button;	// ���� ����

	CRYLCheckBox*	m_pChantEffect01_2Button;	// ��Ƽ ����
	CRYLCheckBox*	m_pChantEffect02_2Button;	// �Ʊ� ����
	CRYLCheckBox*	m_pChantEffect03_2Button;	// �߸� ����
	CRYLCheckBox*	m_pChantEffect04_2Button;	// ���� ����
	
	// ���ǰ ȿ��
	CRYLCheckBox*	m_pEquipGradeEffectButton;	// �׷��̵庰 ȿ�� ����


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