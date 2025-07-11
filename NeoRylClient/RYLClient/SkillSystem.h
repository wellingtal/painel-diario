// SkillSystem.h: interface for the CSkillSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKILLSYSTEM_H__A1E4214E_97C3_4E39_BB99_605E8BEE36CF__INCLUDED_)
#define AFX_SKILLSYSTEM_H__A1E4214E_97C3_4E39_BB99_605E8BEE36CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SkillNode.h"
#include <vector>

using namespace std;

typedef struct EffectNode
{
	unsigned long m_dwTargetID;
	CEffScript *m_lpEffect;
} EffectNode, *LPEffectNode;

class CSkillSystem  
{
	public:
		CSkillNode * GetNormalSkillNode(void);
		void Update(void);
		void AddSkill(CSkillNode *lpSkillNode);
		vector<CSkillNode *> m_lstSkillNode;
		vector<LPEffectNode> m_lstEffect;

		CSkillSystem();
		~CSkillSystem();
};

extern CSkillSystem g_SkillSystem;

#endif // !defined(AFX_SKILLSYSTEM_H__A1E4214E_97C3_4E39_BB99_605E8BEE36CF__INCLUDED_)
