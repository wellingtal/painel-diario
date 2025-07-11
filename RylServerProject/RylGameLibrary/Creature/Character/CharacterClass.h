#ifndef _CHARACTER_CLASS_H_
#define _CHARACTER_CLASS_H_

#include <DB/DBDefine.h>


// ���� ����
struct CharacterDBData;
struct ChState;

class CClass 
{
public:

	enum JobType
    {
		NONE_JOB = 0, 

		// �ΰ� �⺻Ŭ����
		Fighter		= 1,	Rogue		= 2,
		Mage		= 3,	Acolyte		= 4,

		// �ΰ� 1stŬ����
		Defender	= 5,	Warrior		= 6,
        Assassin	= 7,	Archer		= 8,
		Sorcerer	= 9,	Enchanter	= 10,
        Priest		= 11,	Cleric		= 12,
	
		// ��ĭ �⺻Ŭ����
		Combatant	= 17,	Officiator	= 18,

		// ��ĭ 1stŬ����
		Templar		= 19, 	Attacker	= 20,
		Gunner		= 21,	RuneOff		= 22,
		LifeOff		= 23,	ShadowOff	= 24,

		MAX_CLASS	= 27 // was 27
	};

	enum StatusType 
	{ 
		NONE_STAT	= 0, 
	
		STR 		= 1, 
		DEX 		= 2, 
		CON 		= 3, 
		INT 		= 4, 
		WIS 		= 5, 

		MAX_TYPE	= 6 
	};

	enum JobLevel	
	{ 
		NONE_CLASS		= 0, 
		DEFAULT_CLASS	= 1, 
		JOB_CHANGE_1ST	= 2 
	};

	enum RaceType
	{ 
		HUMAN		= 0, 
		AKHAN		= 1, 
		MAX_RACE	= 2 
	};

	//hayzohar
	enum NationType
	{ 
		KR		= 1, 
		MK		= 2, 
		MAX_NATION	= 3 
	};

	CClass(const JobType eJobType, const JobType ePrevJobType, const JobLevel eJobLevel, const RaceType eRace,		
		const StatusType eType1, const unsigned char cIncrement1,
		const StatusType eType2, const unsigned char cIncrement2,
		const bool bLevelSwap);

	CClass();
	
	void LevelUp(CHAR_INFOST* InfoSt);
	bool JobChange(CharacterDBData* DBData, unsigned char cClassType);	
	bool UpgradeClass(CharacterDBData* DBData, unsigned char cClassType);	
	bool DowngradeClass(CharacterDBData* DBData, unsigned char cClassType);	
	bool InitializeClass(CharacterDBData* DBData, unsigned char cClassType);	// ���� 1�� �⺻ Ŭ������ �����.

	bool CheckState(ChState& State, unsigned char cLevel);
	bool CheckMinState(ChState& State, unsigned char cLevel);
	unsigned short GetMinState(StatusType eType, unsigned char cLevel);

	static JobLevel GetJobLevel(unsigned char cClass);
	static unsigned char GetPreviousJob(unsigned char cClass);
	static unsigned char GetRequiredIP(unsigned short usClass, unsigned char cType);
	static RaceType GetRace(unsigned char cClass);
	
protected:

	StatusType		m_eIncrementType[2];	// ����� �߰� �ɷ�ġ Ÿ��
	unsigned char	m_cIncrementValue[2];	// ����� �߰� �ɷ�ġ ��

	JobType			m_eJobType;			// ���� ����
	JobType			m_ePrevJobType;		// �ٷ� ���� Ŭ������ ����
	JobLevel		m_eJobLevel;		// ���� ������ ����
	RaceType		m_eRace;			// �Ҽ� ����
		
	bool			m_bLevelSwap;		// ������ ���� ������ �����ư��鼭 ������ (¦�� 2/1 �����̸� Ȧ�� 1/2 ����)

	inline void IncrementByType(CHAR_INFOST* InfoSt, 
		unsigned char cTypeIndex, unsigned char cIncrementIndex);
};

extern CClass ClassTable[CClass::MAX_CLASS];


inline void CClass::IncrementByType(CHAR_INFOST* InfoSt, 
									unsigned char cTypeIndex, unsigned char cIncrementIndex)
{
	switch(m_eIncrementType[cTypeIndex]) {
		case STR:	InfoSt->STR	+= m_cIncrementValue[cIncrementIndex];	break;
		case DEX:	InfoSt->DEX	+= m_cIncrementValue[cIncrementIndex];	break;
		case CON:	InfoSt->CON	+= m_cIncrementValue[cIncrementIndex];	break;
		case INT:	InfoSt->INT	+= m_cIncrementValue[cIncrementIndex];	break;
		case WIS:	InfoSt->WIS	+= m_cIncrementValue[cIncrementIndex];	break;
	}
}

#endif