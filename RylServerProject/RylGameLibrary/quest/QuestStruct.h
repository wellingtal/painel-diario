
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
		NPC_QUEST		= 0,					// NPC 가 주는 퀘스트
		ITEM_QUEST		= 1,					// 아이템 사용을 통한 퀘스트
		AREA_QUEST		= 2,					// 위치 트리거 퀘스트

		PARTY_QUEST_BIT	= 0xF000				// 파티 퀘스트 비트
	};

	enum Trigger
	{
		TRIGGER_START		= 0,				// 트리거 : 시작
		TRIGGER_PUTON		= 1,				// 트리거 : 특정 장소에 물품 떨구기
		TRIGGER_GETON		= 2,				// 트리거 : 특정 장소 도착
		TRIGGER_TALK		= 3,				// 트리거 : 대화하기
		TRIGGER_KILL		= 4,				// 트리거 : 죽이기
		TRIGGER_PICK		= 5,				// 트리거 : 물품 얻기
		TRIGGER_FAME		= 6,				// 트리거 : 일정 명성 얻기
		TRIGGER_LEVEL_TALK	= 7,				// 트리거 : 일정 레벨에서 대화하기
		TRIGGER_FAME_TALK	= 8,				// 트리거 : 일정 명성에서 대화하기
		TRIGGER_ABILITY_TALK = 9				// 트리거 : 일정 어빌리티에서 대화하기
	};

	enum Event
	{
		EVENT_DISAPPEAR		= 0,				// 이벤트 : 물품 삭제
		EVENT_GET			= 1,				// 이벤트 : 물품 얻기
		EVENT_SPAWN			= 2,				// 이벤트 : 몬스터 스폰
		EVENT_MONSTERDROP	= 3,				// 이벤트 : 몬스터 물품 떨구기
		EVENT_END			= 4,				// 이벤트 : 퀘스트 종료
		EVENT_AWARD			= 5,				// 이벤트 : 물품 보상
		EVENT_MSGBOX		= 6,				// 이벤트 : 메세지 박스 출력
		EVENT_PHASE			= 7,				// 이벤트 : 다음 페이즈 진행
		EVENT_ADDQUEST		= 8,				// 이벤트 : 강제 퀘스트
		EVENT_AWARDITEM		= 9,				// 이벤트 : 장비 보상
		EVENT_AWARDITEMID	= 10,				// 이벤트 : 장비 보상
		EVENT_AWARDITEMCLASS= 11,				// 이벤트 : 장비 보상
		EVENT_MOVE			= 12,				// 이벤트 : 존 이동
		EVENT_THEATERMODE	= 13,				// 이벤트 : 씨어터 모드
		EVENT_ALARMBOX		= 14,				// 이벤트 : 알람 메세지 박스 출력
		EVENT_AWARDABILITY	= 15,				// 이벤트 : 어빌리티 보상
	};

	enum Const
	{ 
		ERROR_OF_DIST		= 7,				// 위치 퀘스트 체크 영역 거리 오차값
		PARTY_QUEST_DIST	= 32,				// 32m 안에 있는 파티원만 파티 퀘스트 영향 받음

		MAX_HISTORY_QUEST	= 200,				// 히스토리 최대 목록
		MAX_EXECUTING_QUEST	= 20,				// 동시 수행 가능한 최대 퀘스트 수
		MAX_TRIGGER			= 10,				// 한 페이즈당 최대 트리거 수

		DISABLE_CANCEL_QUEST_BIT	= 0xF000	// 삭제 불가능한 강제 퀘스트
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
		unsigned long 	m_dwPhaseNumber;				// 페이즈 번호

		vector<DeletedItem>		m_lstDeletedItem;		// 퀘스트 삭제시 함께 지워질 아이템
		vector<TriggerNode *>	m_lstTrigger;			// 트리거 리스트

		PhaseNode() : m_dwZondID(0), m_dwPhaseNumber(0) { }
		PhaseNode(unsigned long dwZoneID, unsigned long dwPhaseNumber) : m_dwZondID(dwZoneID), m_dwPhaseNumber(dwPhaseNumber) { }

	};


	struct QuestNode
	{
		unsigned short 	m_wQuestID;				// 퀘스트 아이디
		unsigned short	m_wQuestType;			// 퀘스트 타입
		unsigned short 	m_wMaxPhase;			// 최대 페이즈 수
		unsigned short 	m_wMinLevel;			// 수행 가능 최소 레벨
		unsigned short 	m_wMaxLevel;			// 수행 가능 최대 레벨
		unsigned long 	m_dwMinFame;			// 수행 가능 최소 명성
		unsigned long 	m_dwMaxFame;			// 수행 가능 최대 명성
		unsigned long	m_dwClass;				// 수행 가능 클래스
		unsigned short	m_wCompletedQuest;		// 선행 퀘스트
		bool			m_bSave;				// 히스토리 기록 여부

		unsigned char	m_cNation;				// 수행 가능 국적
		bool			m_bNationDependent;		// 국가 종속 여부 (종속 퀘스트는 국적 변경시 삭제된다.)
		unsigned char	m_cSkillPointBonus;		// 스킬포인트 어드벤티지

		unsigned char	m_cAbilityPointBonus;	// 어빌리티포인트 어드벤티지


		float			m_fPosX;				// 위치 퀘스트의 영역 중심 위치
		float			m_fPosY;
		float			m_fPosZ;
		float			m_fDistance;			// 중심 위치에서의 거리
		unsigned long	m_dwZoneID;				// 위치 퀘스트의 존
		bool			m_bDelLinkItem;			// 퀘스트 취소 시 관련 아이템 삭제 여부
		
		vector<PhaseNode *>		m_lstPhase;		// 페이즈 리스트

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
			// 국가 종속적인 퀘스트중에서 인자로 넘어온 국적이 수행할 수 없는 퀘스트이면 삭제해야할 퀘스트이다.
			if ( m_bNationDependent )
			{
				// 수행할 수 있는 국적인지 체크
				bool bEnableQuest = false ;
				if ( 0 == m_cNation )
				{
					ERRLOG1(g_Log, "QuestID : 0x%04x 모든 국가가 수행할수 있는 퀘스트가 국가 종속성을 가지고 있습니다.", m_wQuestID) ;
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
					// 지워야하는 퀘스트이다.
					return true ;
				}
			}

			return false;
		}
	};

	struct TriggerMsg
	{
		unsigned long	m_dwCID;			// 캐릭터 CID.
		unsigned short	m_wQuestID;			// 퀘스트 ID.
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
		unsigned char 	m_cPhase;						// 현재 페이즈
		unsigned char 	m_cTriggerCount[MAX_TRIGGER];	// 각 트리거의 발동 횟수

		QuestNode*		m_QuestNode;					// 퀘스트 노드 (원본)

		ExecutingQuest();
		ExecutingQuest(unsigned short wQuestID, unsigned char cPhase, unsigned char* aryTriggerCount);
	};
}


#endif