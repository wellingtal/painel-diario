
#ifndef _QUEST_STRUCT_
#define _QUEST_STRUCT_

#include <vector>
#include <Log/ServerLog.h>
#include <Creature/CreatureStructure.h>

using namespace std;


namespace Quest
{
	enum eQeustType
	{
		NPC_QUEST		= 0,					// NPC �� �ִ� ����Ʈ
		ITEM_QUEST		= 1,					// ������ ����� ���� ����Ʈ
		AREA_QUEST		= 2,					// ��ġ Ʈ���� ����Ʈ

		PARTY_QUEST_BIT	= 0xF000				// ��Ƽ ����Ʈ ��Ʈ
	};

	enum Trigger
	{
		TRIGGER_START		= 0,				// Ʈ���� : ����
		TRIGGER_PUTON		= 1,				// Ʈ���� : Ư�� ��ҿ� ��ǰ ������
		TRIGGER_GETON		= 2,				// Ʈ���� : Ư�� ��� ����
		TRIGGER_TALK		= 3,				// Ʈ���� : ��ȭ�ϱ�
		TRIGGER_KILL		= 4,				// Ʈ���� : ���̱�
		TRIGGER_PICK		= 5,				// Ʈ���� : ��ǰ ���
		TRIGGER_FAME		= 6,				// Ʈ���� : ���� �� ���
		TRIGGER_LEVEL_TALK	= 7,				// Ʈ���� : ���� �������� ��ȭ�ϱ�
		TRIGGER_FAME_TALK	= 8,				// Ʈ���� : ���� ������ ��ȭ�ϱ�
		TRIGGER_ABILITY_TALK = 9				// Ʈ���� : ���� �����Ƽ���� ��ȭ�ϱ�
	};

	enum Event
	{
		EVENT_DISAPPEAR		= 0,				// �̺�Ʈ : ��ǰ ����
		EVENT_GET			= 1,				// �̺�Ʈ : ��ǰ ���
		EVENT_SPAWN			= 2,				// �̺�Ʈ : ���� ����
		EVENT_MONSTERDROP	= 3,				// �̺�Ʈ : ���� ��ǰ ������
		EVENT_END			= 4,				// �̺�Ʈ : ����Ʈ ����
		EVENT_AWARD			= 5,				// �̺�Ʈ : ��ǰ ����
		EVENT_MSGBOX		= 6,				// �̺�Ʈ : �޼��� �ڽ� ���
		EVENT_PHASE			= 7,				// �̺�Ʈ : ���� ������ ����
		EVENT_ADDQUEST		= 8,				// �̺�Ʈ : ���� ����Ʈ
		EVENT_AWARDITEM		= 9,				// �̺�Ʈ : ��� ����
		EVENT_AWARDITEMID	= 10,				// �̺�Ʈ : ��� ����
		EVENT_AWARDITEMCLASS= 11,				// �̺�Ʈ : ��� ����
		EVENT_MOVE			= 12,				// �̺�Ʈ : �� �̵�
		EVENT_THEATERMODE	= 13,				// �̺�Ʈ : ������ ���
		EVENT_ALARMBOX		= 14,				// �̺�Ʈ : �˶� �޼��� �ڽ� ���
		EVENT_AWARDABILITY	= 15,				// �̺�Ʈ : �����Ƽ ����
	};

	enum Const
	{ 
		ERROR_OF_DIST		= 7,				// ��ġ ����Ʈ üũ ���� �Ÿ� ������
		PARTY_QUEST_DIST	= 32,				// 32m �ȿ� �ִ� ��Ƽ���� ��Ƽ ����Ʈ ���� ����

		MAX_HISTORY_QUEST	= 200,				// �����丮 �ִ� ���
		MAX_EXECUTING_QUEST	= 20,				// ���� ���� ������ �ִ� ����Ʈ ��
		MAX_TRIGGER			= 10,				// �� ������� �ִ� Ʈ���� ��

		DISABLE_CANCEL_QUEST_BIT	= 0xF000	// ���� �Ұ����� ���� ����Ʈ
	};

	enum ErrorCode
	{
		S_SUCCESS			= 0,
		S_END_QUEST			= 1,
		E_EVENT_DISAPPEAR	= 2,
		E_EVENT_GET			= 3,
		E_EVENT_MONSTERDROP	= 4, 
		E_EVENT_AWARDITEM	= 5,
		E_EVENT_AWARDGOLD	= 6
	};


	struct EventNode
	{
		unsigned long 	m_dwEventKind;
		unsigned long 	m_dwEventNumber;
		unsigned long 	m_dwEventAmount1;
		unsigned long 	m_dwEventAmount2;
		unsigned long	m_dwEventAmount3;

		float 			m_fPosX;
		float 			m_fPosY;
		float 			m_fPosZ;

		EventNode()	
		:	m_dwEventKind(0), m_dwEventNumber(0), m_dwEventAmount1(0), m_dwEventAmount2(0), m_dwEventAmount3(0), 
			m_fPosX(0), m_fPosY(0), m_fPosZ(0) 
		{ }

		EventNode(unsigned long dwEventKind, unsigned long dwEventNumber, 
			unsigned long dwEventAmount1, unsigned long dwEventAmount2, unsigned long dwEventAmount3, 
			float fPosX, float fPosY, float fPosZ)
		:	m_dwEventKind(dwEventKind), m_dwEventNumber(dwEventNumber), 
			m_dwEventAmount1(dwEventAmount1), m_dwEventAmount2(dwEventAmount2), m_dwEventAmount3(dwEventAmount3), 
			m_fPosX(fPosX), m_fPosY(fPosY), m_fPosZ(fPosZ) 
		{ }

	};


	struct TriggerNode
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

		vector<EventNode *> m_lstEvent;
		vector<EventNode *> m_lstFalseEvent;

		TriggerNode()
			: m_dwTriggerKind(0), m_dwTriggerID(0), m_dwZoneID(0), m_dwMaxCount(0), m_dwLevel(0),
			 m_fPosX(0), m_fPosY(0), m_fPosZ(0), m_fDistance(0)
		{ }

		TriggerNode(unsigned long dwTriggerKind, unsigned long dwTriggerID, 
			unsigned long dwZoneID, unsigned long dwMaxCount, unsigned long dwLevel,
			float fPosX, float fPosY, float fPosZ, float fDistance)
			: m_dwTriggerKind(dwTriggerKind), m_dwTriggerID(dwTriggerID), 
			  m_dwZoneID(dwZoneID), m_dwMaxCount(dwMaxCount), m_dwLevel(dwLevel),
			  m_fPosX(fPosX), m_fPosY(fPosY), m_fPosZ(fPosZ), m_fDistance(fDistance) 
		{ }

	};


	struct PhaseNode
	{
		struct DeletedItem
		{
			unsigned short		m_wItemID;
			unsigned char		m_cItemNum;

			DeletedItem() : m_wItemID(0), m_cItemNum(0) { }
			DeletedItem(unsigned short wItemID, unsigned char cItemNum) 
			:	m_wItemID(wItemID), m_cItemNum(cItemNum)
			{ }
		};

		unsigned long 	m_dwZondID;
		unsigned long 	m_dwPhaseNumber;				// ������ ��ȣ

		vector<DeletedItem>		m_lstDeletedItem;		// ����Ʈ ������ �Բ� ������ ������
		vector<TriggerNode *>	m_lstTrigger;			// Ʈ���� ����Ʈ

		PhaseNode() : m_dwZondID(0), m_dwPhaseNumber(0) { }
		PhaseNode(unsigned long dwZoneID, unsigned long dwPhaseNumber) : m_dwZondID(dwZoneID), m_dwPhaseNumber(dwPhaseNumber) { }

	};


	struct QuestNode
	{
		unsigned short 	m_wQuestID;				// ����Ʈ ���̵�
		unsigned short	m_wQuestType;			// ����Ʈ Ÿ��
		unsigned short 	m_wMaxPhase;			// �ִ� ������ ��
		unsigned short 	m_wMinLevel;			// ���� ���� �ּ� ����
		unsigned short 	m_wMaxLevel;			// ���� ���� �ִ� ����
		unsigned long 	m_dwMinFame;			// ���� ���� �ּ� ��
		unsigned long 	m_dwMaxFame;			// ���� ���� �ִ� ��
		unsigned long	m_dwClass;				// ���� ���� Ŭ����
		unsigned short	m_wCompletedQuest;		// ���� ����Ʈ
		bool			m_bSave;				// �����丮 ��� ����

		unsigned char	m_cNation;				// ���� ���� ����
		bool			m_bNationDependent;		// ���� ���� ���� (���� ����Ʈ�� ���� ����� �����ȴ�.)
		unsigned char	m_cSkillPointBonus;		// ��ų����Ʈ ��庥Ƽ��

		unsigned char	m_cAbilityPointBonus;	// �����Ƽ����Ʈ ��庥Ƽ��


		float			m_fPosX;				// ��ġ ����Ʈ�� ���� �߽� ��ġ
		float			m_fPosY;
		float			m_fPosZ;
		float			m_fDistance;			// �߽� ��ġ������ �Ÿ�
		unsigned long	m_dwZoneID;				// ��ġ ����Ʈ�� ��
		bool			m_bDelLinkItem;			// ����Ʈ ��� �� ���� ������ ���� ����
		
		vector<PhaseNode *>		m_lstPhase;		// ������ ����Ʈ

		QuestNode()
			: m_wQuestID(0), m_wQuestType(0), m_wMaxPhase(0), m_wMinLevel(0), m_wMaxLevel(0), m_dwMinFame(0), m_dwMaxFame(0), 
			  m_dwClass(0), m_wCompletedQuest(0), m_bSave(false), m_cSkillPointBonus(0), m_cAbilityPointBonus(0), 
			  m_cNation(0), m_bNationDependent(false),
			  m_fPosX(0), m_fPosY(0), m_fPosZ(0), m_fDistance(0), m_dwZoneID(0), m_bDelLinkItem(true)
		{ }

		QuestNode(unsigned short wQuestID, unsigned short wQuestType, unsigned short wMaxPhase, unsigned short wMinLevel, unsigned short wMaxLevel, 
			unsigned long dwMinFame, unsigned long dwMaxFame,
			unsigned long dwClass, unsigned short wCompleteQuest, bool bSave, unsigned char cNation, bool bNationDependent)
			: m_wQuestID(wQuestID), m_wQuestType(wQuestType), m_wMaxPhase(wMaxPhase), m_wMinLevel(wMinLevel),
			  m_wMaxLevel(wMaxLevel), m_dwMinFame(dwMinFame), m_dwMaxFame(dwMaxFame),m_dwClass(dwClass), m_wCompletedQuest(wCompleteQuest), m_bSave(bSave),
			  m_cNation(cNation), m_bNationDependent(bNationDependent), m_cSkillPointBonus(0), m_cAbilityPointBonus(0),
			  m_fPosX(0), m_fPosY(0), m_fPosZ(0), m_fDistance(0), m_dwZoneID(0), m_bDelLinkItem(true)
		{ }

		unsigned short	GetQuestType()	{ return (m_wQuestType & ~Quest::PARTY_QUEST_BIT);			}
		bool			IsPartyQuest()	{ return (0 != (m_wQuestType & Quest::PARTY_QUEST_BIT));	}

		bool			CheckNationDependent( unsigned char cNation )
		{
			// ���� �������� ����Ʈ�߿��� ���ڷ� �Ѿ�� ������ ������ �� ���� ����Ʈ�̸� �����ؾ��� ����Ʈ�̴�.
			if ( m_bNationDependent )
			{
				// ������ �� �ִ� �������� üũ
				bool bEnableQuest = false ;
				if ( 0 == m_cNation )
				{
					ERRLOG1(g_Log, "QuestID : 0x%04x ��� ������ �����Ҽ� �ִ� ����Ʈ�� ���� ���Ӽ��� ������ �ֽ��ϴ�.", m_wQuestID) ;
					return false;
				}
				else
				{
					unsigned char cQuestNation = m_cNation ;
					unsigned char cNationRes = 0 ;

					while ( cQuestNation != 0 )
					{
						cNationRes = cQuestNation % 10 ;
						cQuestNation /= 10 ;

						if ( cNationRes == cNation )
						{
							bEnableQuest = true ;
							break;
						}
					}
				}

				if ( !bEnableQuest ) //
				{
					// �������ϴ� ����Ʈ�̴�.
					return true ;
				}
			}

			return false;
		}
	};

	struct TriggerMsg
	{
		unsigned long	m_dwCID;			// ĳ���� CID.
		unsigned short	m_wQuestID;			// ����Ʈ ID.
		unsigned char	m_cPhase;
		unsigned char	m_cTrigger;
		unsigned char	m_cCount;
		Position		m_Pos;

		TriggerMsg()
		{
			m_dwCID		= 0;
			m_wQuestID	= 0;
			m_cPhase	= 0;
			m_cTrigger	= 0;
			m_cCount	= 0;
		}
	};


	struct ExecutingQuest
	{
		unsigned char 	m_cPhase;						// ���� ������
		unsigned char 	m_cTriggerCount[MAX_TRIGGER];	// �� Ʈ������ �ߵ� Ƚ��

		QuestNode*		m_QuestNode;					// ����Ʈ ��� (����)

		ExecutingQuest();
		ExecutingQuest(unsigned short wQuestID, unsigned char cPhase, unsigned char* aryTriggerCount);
	};
}


#endif