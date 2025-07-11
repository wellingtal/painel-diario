#ifndef _SKILL_MGR_H_
#define _SKILL_MGR_H_

#define g_SkillMgr CSkillMgr::GetInstance()

#include <limits>

#include <Pattern/Singleton.h>
#include "SkillStructure.h"

// ���� ����
class CDelimitedFile;


class CSkillMgr : public CSingleton<CSkillMgr>
{
public:

	enum _Const
	{ 
		MAX_SKILL_LOCKCOUNT     = 5,	    // ��ų�� �ִ� ���� (�� ������ŭ ProtoType�� �����Ѵ�)
		MAX_SKILL_LEVEL         = 6,		// ��ų ������ �Ѱ�

		MAX_SKILL_PER_STATUS    = 16,	    // æƮ�� ��æƮ�� ������
	};

	~CSkillMgr();

	void ClearProtoType(void);

	bool LoadSkillsFromFile(const char* szFileName = 0);
	bool LoadSkillsFromBinary(const char* szFileNameBinary = 0);
	bool SaveSkillsToBinary(const char* szFileNameBinary = 0, const char* szTrashFile = 0);

	const Skill::ProtoType* GetSkillProtoType(unsigned short usSkill_ID);
	void GetClassSkill(unsigned char cClass, unsigned short *arySkillID, unsigned char *cSkillNum);

    // �ε� ����� üũ����, ���� �����ͷ� ���� üũ���� ���Ѵ�.
    // �����Ͱ� �ٲ�� false�� �����Ѵ�.
    bool CheckData();

private:

	CSkillMgr();
	
    struct ProtoTypeArray 
    {
        Skill::ProtoType m_ProtoTypes[MAX_SKILL_LOCKCOUNT];
        inline bool operator < (ProtoTypeArray& rhs)
        { return m_ProtoTypes[0].m_usSkill_ID < rhs.m_ProtoTypes[0].m_usSkill_ID; }
    };

	static const char*	ms_szSkillScriptFileName;
    static CSkillMgr    ms_this;

	ProtoTypeArray*     m_ProtoTypeArray;
    size_t              m_nSkillNum;

    unsigned long       m_dwCRC32;

	// ���ڿ��� ���ؼ�, �˸��� Ÿ�� ���� �����Ѵ�. ���н� nMaxType�� �����Ѵ�.
	unsigned char ReadStringToTypeValue(CDelimitedFile& DelimitedFile, const char* szColumn, 
        const CTypeName* TypeArray, const unsigned char nMaxType);

	bool CheckParentChildRule(void);
};

#endif