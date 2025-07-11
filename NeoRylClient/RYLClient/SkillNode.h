// SkillNode.h: interface for the CSkillNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKILLNODE_H__29494825_4625_40EA_BE4B_436BBB1A085E__INCLUDED_)
#define AFX_SKILLNODE_H__29494825_4625_40EA_BE4B_436BBB1A085E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSkillNode  
{
public:

	CEffScript* 	m_lpEffectIndex;
	CEffScript* 	m_lpButtonDown;
	CEffScript* 	m_lpButtonSuccess;

	CItemInstance*	m_lpUseItem;

	unsigned long	m_dwTargetChrID;

	unsigned char 	m_cLevel;

	unsigned short 	m_wSkill;
	unsigned short 	m_wSkillGrade;
	unsigned short 	m_wNumAttack;
	unsigned short 	m_wNumAttackCount;

	BOOL 			m_bMelee;
	BOOL 			m_bAngle;
	BOOL 			m_bFirstAttack;

	CSkillNode();
	~CSkillNode();

	void Create(unsigned long dwChrID, unsigned short wSkill, unsigned short wSkillGrade, 
		unsigned long dwCastingTick, CItemInstance* lpUseItem);
};

#endif // !defined(AFX_SKILLNODE_H__29494825_4625_40EA_BE4B_436BBB1A085E__INCLUDED_)
