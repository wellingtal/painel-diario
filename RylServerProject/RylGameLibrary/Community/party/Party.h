#ifndef _RYL_GAME_LIBRARY_PARTY_H_
#define _RYL_GAME_LIBRARY_PARTY_H_

#include <set>
#include <boost/pool/pool_alloc.hpp>

#include <DB/DBdefine.h>
#include <Skill/Spell/PartySpellMgr.h>

#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>

// ���� ����
class CCell;
class CCharacter;
class CMonster;
class CPacketBuffer;
struct Position;

namespace Item
{
	class CItem;
};
    
namespace Quest
{
	struct QuestNode;
};


#define PARTYLOG(x)		(void*)0
/*
#define PARTYLOG(x)		x
*/

class CParty
{
protected:

    CParty();
	CParty(const PARTY& PartyInfo);

	CPartySpellMgr	m_PartySpellMgr;	
	PARTY_EX		m_Party;

public:

	virtual ~CParty();

	// ----------------------------------------------------------------------------------------
	// interface

	CPartySpellMgr& GetPartySpellMgr()		{ return m_PartySpellMgr;			}	

	unsigned long	GetUID(void)			{ return m_Party.m_dwPartyID;		}
	
	void SetUID(unsigned long dwPartyID)	{ m_Party.m_dwPartyID = dwPartyID;	}	

	int	 GetMemberNum(void)					{ return m_Party.m_cMemberNum;		}

	bool IsMember(unsigned long dwMemberCID);

    void ChangeName(unsigned long dwCID, const char* szChangedName);

    virtual bool Join(unsigned long dwSenderCID, unsigned long dwReferenceID,
        const char* strSenderName, unsigned short wMapIndex = 0) { return false; }

	virtual int	Leave(unsigned long dwSenderCID, unsigned long dwReferenceID = 0, 
        unsigned short wMapIndex = 0) { return 0; }

	virtual bool Destory(unsigned long dwSenderCID, unsigned long dwReferenceID)	{ return false; }

	virtual bool Login(unsigned long dwSenderCID, unsigned long dwReferenceID)	{ return false;	}
	virtual bool ReLogin(CCharacter* llpCharacter)								{ return false;	}
	virtual int	Logout(unsigned long dwSenderCID, unsigned long dwReferenceID)	{ return 0;		}
	virtual void PrepareLogout(unsigned long dwMemberID)						{				}
	
	void SetLeader(unsigned long dwLeaderCID)		{ m_Party.m_dwLeaderID = dwLeaderCID; }
	unsigned long GetLeader(void)					{ return m_Party.m_dwLeaderID; }
	bool IsLeader(unsigned long dwLeaderCID)		{ return (m_Party.m_dwLeaderID == dwLeaderCID) ? true : false; }

	// ��Ƽ�� ���� ������Ʈ //
	bool UpdatePartyMemberInfo(unsigned long dwCID, unsigned long dwGID, unsigned short wClass, char cLevel, bool bAutoRouting);
	bool UpdatePartyMemberLevel(unsigned long dwCID, char cLevel);	
	bool UpdatePartyMemberClass(unsigned long dwCID, unsigned short wClass);
	bool UpdatePartyMemberGuild(unsigned long dwCID, unsigned long dwGID);
	bool UpdatePartyMemberServer(unsigned long dwCID, unsigned long dwServerID);
	bool UpdatePartyMemberAutoRouting(unsigned long dwCID, bool bAutoRouting);

	virtual bool TransferLeader(unsigned long dwLeaderCID)	{ return false;	}

	virtual unsigned char GetLoggedMemberAverageLevel(void)	{ return 0;		}

	virtual bool AdjustAutoRouting(unsigned long dwTargetID, bool bSwitch)	{ return false;	}

	// ----------------------------------------------------------------------------------------
	// ��Ƽ�� ��ü�� ������ �ִ� ���

	virtual void MovePos(POS NewPos, char cZone, const bool bSitDown)	{		}
	virtual void MoveZone(POS NewPos, char Zone, char Channel)		    {		}

	virtual bool AutoRouting(CAggresiveCreature* lpPickkingCreature, Item::CItem* lpItem, unsigned long& dwGold)	{ return false;	}

	virtual bool Attack(AtType attackType, CAggresiveCreature** pDefenders, unsigned char* cDefenserJudges, 
        CAggresiveCreature* lpOffencer, float fDistance, unsigned char cTargetType)									{ return false;	}

    // --------------------------------------------------------------------------------------
    // �����ֱ�� ȣ��Ǵ� �Լ�

    virtual void UpdateData() { };
};


class CMonsterParty : public CParty
{
public:

    typedef std::set<unsigned long, std::less<unsigned long>, 
        boost::fast_pool_allocator<unsigned long> > PartyTargetSet;
	
protected:

	CMonster*				m_pMemberPointer[PARTY::MAX_MEM];

	float					m_fHelpRange;		// �ο�� �ִ� ��Ƽ���� �����ַ� ���� �Ÿ�

	PartyTargetSet			m_partyTargets;		// ��Ƽ���� �ο�� �ִ� ����� CID


public:

	CMonsterParty(const PARTY& PartyInfo, unsigned short wMapIndex = 0);
	CMonsterParty();
	virtual ~CMonsterParty();

	enum Const
	{
		DEFAULT_HELP_RANGE = 36,
		ERROR_OF_LEVEL_GAP = 10
	};

	PartyTargetSet&				GetPartyTargetSet(void)	{ return m_partyTargets;	}
	float						GetHelpRange(void)		{ return m_fHelpRange;		}
	unsigned short				GetAvgLevel(void);
	unsigned short				GetHighestLevel(void);
	int							GetMemberTypeNum(void);

	bool	Join(unsigned long dwSenderCID, unsigned long dwReferenceID = 0, const char* strSenderName = NULL, unsigned short wMapIndex = 0);
	int		Leave(unsigned long dwSenderCID, unsigned long dwReferenceID = 0, unsigned short wMapIndex = 0);
	bool	Destory(unsigned long dwSenderCID, unsigned long dwReferenceID);

	CMonster*	GetMember(unsigned int nIndex)
	{
		if (nIndex < 0 || nIndex >= PARTY::MAX_MEM) return NULL;
		return m_pMemberPointer[nIndex];
	}

	// ----------------------------------------------------------------------------------------
	// ��Ƽ�� ��ü�� ������ �ִ� ���

	bool Attack(AtType attackType, CAggresiveCreature** pDefenders, unsigned char* cDefenserJudges, 
		CAggresiveCreature* lpOffencer, float fDistance, unsigned char cTargetType);
};


class CCharacterParty : public CParty
{
protected:

	CCharacter*			m_pMemberPointer[PARTY::MAX_MEM];
	bool				m_bAutoRouting[PARTY::MAX_MEM];

	CCharacterParty*	m_pHostileParty;
	CCharacter*			m_pFightingMember[PARTY::MAX_MEM];

    PartyMemberData     m_partyMemberData[PARTY::MAX_MEM];

	void SendPartyCmdInfo(PktPC::PartyCmd Command, CCharacter *lpCharacter);
	void SendPartyCommand(PktPC::PartyCmd Command, const char *SenderName_In, unsigned long dwSenderCID, unsigned long ReceiverCID);

    static void SetUpdateFlag(PartyMemberData& partyMemberData, CCharacter& character, char*& szPacket, bool bUpdateForce);

public:

	// �ؿ� �������� ����ϴ� ���
	enum ForeignConst
	{
		ADDITIONAL_EXP_CAP_MIN_PARTY_MENMBER	= 2,	// �ּ� 2�� �̻��� ��Ƽ���� ��ó�� ���� ��		
		ADDITIONAL_EXP_CAP_PERCENTAGE			= 40,	// 40 % �߰� Exp Cap ���ʽ�
	} ;

	CCharacterParty(const PARTY& PartyInfo, bool bCreate);
	CCharacterParty();
	~CCharacterParty();

	// ��Ƽ���� �޸��� ������ ��ȯ�Ѵ�.
	unsigned char GetRaceRate(void);

	bool Join(unsigned long dwSenderCID, unsigned long dwReferenceID, const char* strSenderName, unsigned short wMapIndex = 0);
	int Leave(unsigned long dwSenderCID, unsigned long dwReferenceID = 0, unsigned short wMapIndex = 0);
	bool Destory(unsigned long dwSenderCID, unsigned long dwReferenceID);

	bool Login(unsigned long dwSenderCID, unsigned long dwReferenceID);
	bool ReLogin(CCharacter* llpCharacter);
	int	Logout(unsigned long dwSenderCID, unsigned long dwReferenceID);
	void PrepareLogout(unsigned long dwMemberID);
	
	bool TransferLeader(unsigned long dwLeaderCID);

	CCharacter** GetMemberPointerList(void)				{ return m_pMemberPointer; }

	unsigned char GetLoggedMemberAverageLevel(void);
	int	GetNearMemberList(CCell *pCell, bool bAutoRouting, CCharacter **aryNearCharacterList, unsigned char &cHighestLevel);

	bool AdjustAutoRouting(unsigned long dwTargetID, bool bSwitch);

	// ----------------------------------------------------------------------------------------
	// ����Ʋ ���� �޼ҵ�

	CCharacterParty* GetHostileParty(void) { return m_pHostileParty; }

	bool StartTeamBattle(CCharacterParty* pHostileParty);
	int	DropMember(CCharacter* pDropMember, PktDuC::DuelCmd eCmd);
	void EndTeamBattle(void);
	bool MakeTeamBattleInfo(char* szPacket, unsigned short& dwPacketSize, CCharacter* pChallenger, unsigned char cCmd);

	// ----------------------------------------------------------------------------------------
	// ��Ƽ�� ��ü�� ������ �ִ� ���

	void MovePos(POS NewPos, char cZone, const bool bSitDown);
	void MoveZone(POS NewPos, char Zone, char Channel);

	bool AutoRouting(CAggresiveCreature* lpPickkingCreature, Item::CItem* lpItem, unsigned long& dwGold);

	bool Attack(AtType attackType, CAggresiveCreature** pDefenders, unsigned char* cDefenserJudges, 
				CAggresiveCreature* lpOffencer, float fDistance, unsigned char cTargetType);

	// ----------------------------------------------------------------------------------------
	// ��Ƽ ����Ʈ ���� �Լ�

	bool StartQuest(Quest::QuestNode* lpQuestNode, const Position& CenterPos, unsigned long dwNPCID);
	bool CheckTrigger(unsigned long dwExceptCID, unsigned char cTriggerKind, unsigned long dwReferenceID, Position Pos, short wCount);
	void OperateTrigger(unsigned short wQuestID, unsigned char cPhase, unsigned char cTrigger, unsigned char cCount, Position Pos);

	// ----------------------------------------------------------------------------------------
	// Send ����

	void SendPartyInfo(CCharacter *lpCharacter);
	void SendAllLoggedMember(const char* szPacket, unsigned long dwPacketSize, unsigned long dwExclusion, unsigned char cCMD_In);
    void SendNotNearCellMember(const char* szPacket, unsigned long dwPacketSize, CCell* lpCell, unsigned char cCMD_In);
	void SendDivisionExp(CCharacter* lpCharacter, CAggresiveCreature* lpDeadCreature, unsigned long dwExp, int nStandardLevel);
	void SendDropMember(CCharacter* pDropMember, PktDuC::DuelCmd eCmd);
	void SendRecall(CCharacter* lpCaster);
	void SendAutoRouting(unsigned long dwCharID, unsigned short wItemID, unsigned char cCmd);
	void SendPartyAddress(unsigned long dwCharID, const SOCKADDR_IN& PublicAddress, const SOCKADDR_IN& PrivateAddress);
	
	void SendPartyMemberDataToDBAgent(unsigned long dwSenderID, unsigned long dwGID, unsigned short wClass, unsigned long dwServerID, 
			char cLevel, const char* strSenderName, unsigned short usCmd);

	void SendDivisionFame(CCharacter* lpWinnerCharacter, CCharacter* lpLoserCharacter, unsigned char cCmd, unsigned char cAcquireAmount);
	// ĳ���� ������ ������Ʈ �Ǿ��� �� �ٸ� ���� �ִ� ĳ���� ������ ������ �Լ�
	void SendMemberInfoNZone(const char* szPacket, unsigned long dwPacketSize, unsigned long dwSenderID, unsigned char cCMD_In);

	// ĳ���� ���ӽ� �ٸ� ���� �ִ� ĳ���� ������ ������ �Լ�
	bool SendMemberInfoAllNZone(unsigned long dwCID);

	// �α׾ƿ��̳� ���� �޼����� ������.
	bool SendMemberInfoOutNZone(unsigned long dwCID, unsigned short wCmd);

    // ���� �ֱ�� ȣ��Ǵ� �Լ� (���⼭�� ĳ���� ��ǥ �� ��Ÿ ���� ������ ������Ʈ ���ش�.)
    void UpdateData();
};

#endif
