// NPCList.h: interface for the CNPCList class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __QUEST_LIST_H__
#define __QUEST_LIST_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector>
#include <map>

#define TRIGGER_START 0
#define TRIGGER_PUTON 1
#define TRIGGER_GETON 2
#define TRIGGER_TALK 3
#define TRIGGER_KILL 4
#define TRIGGER_PICK 5
#define TRIGGER_FAME 6
#define TRIGGER_LEVEL_TALK 7
#define TRIGGER_FAME_TALK 8
#define TRIGGER_ABILITY_TALK 9


#define EVENT_DISAPPEAR 0
#define EVENT_GET 1
#define EVENT_SPAWN 2
#define EVENT_MONSTERDROP 3
#define EVENT_END 4
#define EVENT_AWARD 5
#define EVENT_MSGBOX 6
#define EVENT_PHASE 7
#define EVENT_ADDQUEST 8
#define EVENT_AWARDITEM 9
#define EVENT_AWARDITEMID 10
#define EVENT_AWARDITEMCLASS 11
#define EVENT_MOVE 12
#define EVENT_THEATERMODE 13
#define EVENT_ALARMBOX 14
#define EVENT_AWARDABILITY 15

#define NPC_QUEST	0					// NPC �� �ִ� ����Ʈ
#define ITEM_QUEST	1					// ������ ����� ���� ����Ʈ
#define AREA_QUEST	2					// ��ġ Ʈ���� ����Ʈ

#define PARTY_QUEST_BIT	0xF000			// ��Ƽ ����Ʈ ��Ʈ

using namespace std;

typedef struct EventNode
{
	unsigned long m_dwEventKind;
	unsigned long m_dwEventNumber;
	unsigned long m_dwEventAmount;
	unsigned long m_dwEventAmount2;
	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;
	char *m_strWord;
} EventNode, *LPEventNode;

typedef struct TriggerNode
{
	unsigned long m_dwTriggerKind;
	unsigned long m_dwTriggerID;
	unsigned long m_dwZoneID;
	unsigned long m_dwMaxCount;
	unsigned long m_dwLevel;
	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;
	float m_fDistance;
	std::string	m_strElseMsg;	// LevelTalk ���� Level�� ���� ������ ��µ� �޼���

	vector<LPEventNode> m_lstEvent;
	vector<LPEventNode> m_lstFalseEvent;

	vector<char *> m_lstMsgBox;
	vector<char *> m_lstFalseMsgBox;
} TriggerNode, *LPTriggerNode;

typedef struct TargetPos
{
	int			m_iType;
	unsigned long m_dwPosX;
	unsigned long m_dwPosY;
} TargetPos;

typedef struct PhaseNode
{
	unsigned long m_dwZoneID;
	unsigned long m_dwPhaseNumber;
	char *m_strPhaseDesc;
	vector<LPTriggerNode> m_lstTrigger;
	vector<TargetPos> m_lstTargetPos;

	LPTriggerNode GetTrigger(unsigned long dwTriggerKind, unsigned long dwStartPos, unsigned long *dwTriggerPos)
	{
		vector<LPTriggerNode>::iterator it;

		*dwTriggerPos = 0;
		dwStartPos++;
		for(it = m_lstTrigger.begin(); it != m_lstTrigger.end(); it++, *dwTriggerPos++)
		{
			if(dwStartPos <= *dwTriggerPos)
			{
				if((*it)->m_dwTriggerKind == dwTriggerKind) return (*it);
			}
		}

		return NULL;
	}
} PhaseNode, *LPPhaseNode;

typedef struct QuestNode
{
	enum eCheckResult
	{
		RES_SUCCESS						= 0,	// ����
		RES_MISMATCH_CLASS				= 1,	// Ŭ������ ���� ����
		RES_ALREADY_TAKEN				= 2,	// �̹� ���� ����Ʈ
		RES_NOT_COMPLETE_PRIOR_QUEST	= 3,	// ���� ����Ʈ�� �Ϸ����� �ʾ���
		RES_HIGH_THAN_MAX_LEVEL			= 4,	// ������ �� �ִ� �ְ� �������� ����.
		RES_10_LOW_THAN_MIN_LEVEL		= 5,	// ������ �� �ִ� ���� �������� 10 ���� ����.
		RES_LOW_LEVEL					= 6,	// ������ �� �ִ� �������� ���� ����.
		RES_MISMATCH_NATION				= 7,	// ������ �� ���� �����Դϴ�.
		RES_LOW_FAME					= 8,	// ������ �� �ִ� ������ ���� ����.
		RES_HIGH_THAN_MAX_FAME			= 9,	// ������ �� �ִ� �ְ� �� ���� ����.
	};

    enum eCheckNation
    {
        NATION_ALL                      = 0,    // ��� ����( ī���׶�Ʈ + �޸�ī��� + ���������� )
        NATION_KARTERANT                = 1,    // ī���׶�Ʈ
        NATION_MERKADIA                 = 2,    // �޸�ī���
        NATION_ALMIGHTY_PIRATE          = 3,    // ����������

		// ��� ��
		// 12 : ī���׶�Ʈ + �޸�ī���
		// 13 : ī���׶�Ʈ + ����������
		// 23 : �޸�ī��� + ����������
		// 123 : ī���׶�Ʈ + �޸�ī��� + ���������� = 0 (��� ����)
    };

	unsigned short	m_wQuestID;
	unsigned short	m_wQuestType;
	unsigned short	m_wMaxPhase;
	unsigned short	m_wMinLevel;
	unsigned short	m_wMaxLevel;
	unsigned long	m_dwMinFame;
	unsigned long	m_dwMaxFame;
	unsigned long	m_dwClass;
    unsigned short	m_usNation;                 // ����Ʈ ���� ���� ����
	BOOL			m_bNationDependent;			// ����Ʈ�� ���� ���Ӽ��̶� ������ �ٲ�� �������°�?
    unsigned short	m_usBonusSkillPoint;        // ����Ʈ ���� �� �޴� ���ʽ� ��ų ����Ʈ
    unsigned short	m_usBonusAbilityPoint;        // ����Ʈ ���� �� �޴� ���ʽ� ��ų ����Ʈ

	unsigned long m_dwCompletedQuest;
	char *m_strQuestTitle;
	char *m_strQuestDesc;
	char *m_strQuestShortDesc;
	char *m_strQuestLevel;
	char *m_strQuestAward;
	char m_strIconTexture[32];
	unsigned short m_wPosX1, m_wPosY1;
	unsigned short m_wPosX2, m_wPosY2;
	BOOL m_bSave;
	BOOL m_bInvisible;			// pop up �޴����� �Ⱥ��̴��� ����
	float m_fPosX;				// ��ġ ����Ʈ�� ���� �߽� ��ġ
	float m_fPosY;
	float m_fPosZ;
	float m_fDistance;			// �߽� ��ġ������ �Ÿ�
	unsigned long	m_dwZoneID;	// ��ġ ����Ʈ�� �޴� ��
	bool			m_bDelLinkItem;			// ����Ʈ ��� �� ���� ������ ���� ����

	vector<LPPhaseNode> m_lstPhase;

	map<unsigned short, unsigned char>	m_mapEventGetItem;	// ����Ʈ ���۽� TRIGGER_START �κп� �ִ� EventGet item map

	LPPhaseNode GetPhase(unsigned long dwPhase)
	{
		vector<LPPhaseNode>::iterator it;
		for(it = m_lstPhase.begin(); it != m_lstPhase.end(); it++)
		{
			if((*it)->m_dwPhaseNumber == dwPhase) return (*it);
		}

		return NULL;
	}

	unsigned short	GetQuestType()	{ return (m_wQuestType & ~PARTY_QUEST_BIT); }
	bool			IsPartyQuest()	{ return (0 != (m_wQuestType & PARTY_QUEST_BIT));	}

	unsigned long	CheckQuest(unsigned long dwLevel, unsigned long dwFame, unsigned long dwClass, unsigned long dwNation, unsigned short *lstCompleted, unsigned short wNumCompleted, unsigned short *lstQuestList);
} QuestNode, *LPQuestNode;

typedef struct QuestInstanceNode
{
	unsigned short m_wQuestID;
	LPQuestNode m_lpQuest;
} QuestInstanceNode, *LPQuestInstanceNode;


class CQuestList
{
public:
	LPQuestNode m_lpQuestNode;
	LPPhaseNode m_lpPhaseNode;
	LPTriggerNode m_lpTriggerNode;
	vector<LPQuestNode> m_lstQuestNode;

	CQuestList();
	~CQuestList();

	BOOL		Load(const char *strQuestScriptFile);
	LPQuestNode	GetQuest(unsigned short wQuestID);

protected:
	VOID		DestroyQuestList();
};

extern CQuestList g_QuestList;

#endif // __QUEST_LIST_H__
