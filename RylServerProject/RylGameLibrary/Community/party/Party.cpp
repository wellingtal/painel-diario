#include "stdafx.h"

#include <Map/FieldMap/Cell.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/ExpTable.h>
#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/VirtualMonsterMgr.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/GameClient/SendCharLevelUp.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Dispatch/GameClient/SendCharQuest.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketStruct/AddressPacket.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Packet/PacketStruct/CharQuestPacket.h>

#include <Utility/Setup/ServerSetup.h>
#include <Utility/Math/Math.h>

#include <Quest/QuestStruct.h>
using namespace Quest;

#include "Party.h"
#include "PartyMgr.h"
#include <Log/CharacterLog.h>
#include <Log/LogStruct.h>


CParty::CParty(const PARTY& PartyInfo)
: m_Party(PartyInfo)
{
    m_PartySpellMgr.SetOwner(this);
}

CParty::CParty()
{
}


CParty::~CParty()
{
	CPartyMgr::GetInstance().DeleteFindMemberList(m_Party.m_dwPartyID);
}

void CParty::ChangeName(unsigned long dwCID, const char* szChangedName)
{
    for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
    {
        if (m_Party.MemberCID[nMemberIndex] == dwCID)
        {
            strncpy(m_Party.Name[nMemberIndex], szChangedName, CHAR_INFOST::MAX_NAME_LEN);
            m_Party.Name[nMemberIndex][CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
// Function : CParty::IsMember
//
// Description : ��Ƽ�������� ���θ� �Ǻ���
//
// Inputs : dwMemberCID - �Ǻ��Ϸ��� ĳ����
//
// Outputs : None.
//
// Returns : bool - ��Ƽ���̸� true. �ƴϸ� false.
///////////////////////////////////////////////////////////////////////////////////
bool CParty::IsMember(unsigned long dwMemberCID)
{ 
	if (0 == dwMemberCID) 
	{ 
        ERRLOG0(g_Log, "��Ƽ�α� : �߸��� CID�Դϴ�.");
		return false; 
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwMemberCID)
        {
            return true;
        }
	}

	return false;
}

bool CParty::UpdatePartyMemberInfo(unsigned long dwCID, unsigned long dwGID, unsigned short wClass, char cLevel, bool bAutoRouting)
{
	if (0 == dwCID) 
	{ 
		ERRLOG0(g_Log, "��Ƽ�α� : �߸��� CID�Դϴ�.");
		return false; 
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwCID)
		{	
			// ��Ƽ�� ���� ������Ʈ //
			m_Party.m_cLevel[nMemberIndex]			= cLevel;
			m_Party.m_wClass[nMemberIndex]			= wClass;
			m_Party.m_bAutoRouting[nMemberIndex]	= bAutoRouting;
			m_Party.m_dwGID[nMemberIndex]			= dwGID;			

			return true;
		}
	}
	return false;
}

bool CParty::UpdatePartyMemberServer(unsigned long dwCID, unsigned long dwServerID)
{
	if (0 == dwCID) 
	{ 
		ERRLOG0(g_Log, "��Ƽ�α� : �߸��� CID�Դϴ�.");
		return false; 
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwCID)
		{	
			// ��Ƽ�� ���� ������Ʈ //
			m_Party.ServerID[nMemberIndex]	= dwServerID;			

			return true;
		}
	}
	return false;
}

bool CParty::UpdatePartyMemberAutoRouting(unsigned long dwCID, bool bAutoRouting)
{
	if (0 == dwCID) 
	{ 
		ERRLOG0(g_Log, "��Ƽ�α� : �߸��� CID�Դϴ�.");
		return false; 
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwCID)
		{	
			// ��Ƽ�� ���� ������Ʈ //
			m_Party.m_bAutoRouting[nMemberIndex]	= bAutoRouting;			

			return true;
		}
	}
	return false;
}

bool CParty::UpdatePartyMemberLevel(unsigned long dwCID, char cLevel)
{
	if (0 == dwCID) 
	{ 
		ERRLOG0(g_Log, "��Ƽ�α� : �߸��� CID�Դϴ�.");
		return false; 
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwCID)
		{	
			// ��Ƽ�� ���� ������Ʈ //
			m_Party.m_cLevel[nMemberIndex]	= cLevel;			

			return true;
		}
	}
	return false;
}

bool CParty::UpdatePartyMemberClass(unsigned long dwCID, unsigned short wClass)
{
	if (0 == dwCID) 
	{ 
		ERRLOG0(g_Log, "��Ƽ�α� : �߸��� CID�Դϴ�.");
		return false; 
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwCID)
		{	
			// ��Ƽ�� ���� ������Ʈ //			
			m_Party.m_wClass[nMemberIndex]	= wClass;

			return true;
		}
	}
	return false;
}

bool CParty::UpdatePartyMemberGuild(unsigned long dwCID, unsigned long dwGID)
{
	if (0 == dwCID) 
	{ 
		ERRLOG0(g_Log, "��Ƽ�α� : �߸��� CID�Դϴ�.");
		return false; 
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwCID)
		{	
			// ��Ƽ�� ���� ������Ʈ //		
			m_Party.m_dwGID[nMemberIndex]	= dwGID;			

			return true;
		}
	}
	return false;
}


CMonsterParty::CMonsterParty(const PARTY& PartyInfo, unsigned short wMapIndex)
: CParty(PartyInfo), m_fHelpRange(DEFAULT_HELP_RANGE)
{
	std::fill_n(&m_pMemberPointer[0], int(PARTY::MAX_MEM), reinterpret_cast<CMonster *>(NULL));

	CPartyMgr::GetInstance().AddParty(this);

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		CMonster* lpMonster = NULL;

		if (0 != wMapIndex)
		{
			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
			if (lpVirtualArea)
			{
				CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
				if (lpVirtualMonsterMgr) lpMonster = lpVirtualMonsterMgr->GetMonster(m_Party.MemberCID[nMemberIndex]);
			}
		}
		else
		{
			lpMonster = CCreatureManager::GetInstance().GetMonster(m_Party.MemberCID[nMemberIndex]);
		}
		
		if (NULL == lpMonster) 
		{
			ERRLOG2(g_Log, "��Ƽ�α� : �߸��� ���Ͱ� ��Ƽ�� ����ֽ��ϴ�. PartyUID : 0x%08x, MonsterCID : 0x%08x(0x%08x)", 
					m_Party.m_dwPartyID, m_Party.MemberCID[nMemberIndex]);
			continue; 
		}
				
        LogChantBug(lpMonster, this, "��Ƽ�α� : ���� ��Ƽ ��� �߰��� �õ��մϴ�.", LOG_FFLCHANT);

		m_pMemberPointer[nMemberIndex] = lpMonster;
		m_PartySpellMgr.AddMember(lpMonster);        
    }
}


CMonsterParty::CMonsterParty()
: CParty(), m_fHelpRange(DEFAULT_HELP_RANGE)
{
	std::fill_n(&m_pMemberPointer[0], int(PARTY::MAX_MEM), reinterpret_cast<CMonster *>(NULL));
}


CMonsterParty::~CMonsterParty()
{
	// ������ ��Ƽ Ż��� CCreatureManager �Ҹ��ڿ��� ó���Ѵ�.
}

unsigned short CMonsterParty::GetAvgLevel(void)
{
	// ��Ƽ���� ��� ������ �����Ѵ�.
	unsigned short wAvgLevel = 0;
	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		wAvgLevel += m_pMemberPointer[nMemberIndex]->GetStatus().m_nLevel;
	}
	wAvgLevel /= m_Party.m_cMemberNum;

	return wAvgLevel;
}

unsigned short CMonsterParty::GetHighestLevel(void)
{
	unsigned short wHighestLevel = 0;
	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (wHighestLevel < m_pMemberPointer[nMemberIndex]->GetStatus().m_nLevel)
		{
			wHighestLevel = m_pMemberPointer[nMemberIndex]->GetStatus().m_nLevel;
		}
	}

	return wHighestLevel;
}

// ���� ��Ƽ �������� Ÿ�� ���� �������ش�.
int	CMonsterParty::GetMemberTypeNum()
{
	std::set<int> typeSet;

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		typeSet.insert( m_pMemberPointer[nMemberIndex]->GetPattern() );
	}

	return static_cast<int>(typeSet.size());
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonsterParty::Join
//
// Description : ��Ƽ�� ���͸� ���Խ�Ŵ
//
// Inputs : dwSenderCID - ������ ���̵�
//			dwReferenceID - ��� ����
//			strSenderName - ��� ����
//
// Outputs : None.
//
// Returns : bool - ��������
///////////////////////////////////////////////////////////////////////////////////
bool	CMonsterParty::Join(unsigned long dwSenderCID, unsigned long dwReferenceID, const char* strSenderName, unsigned short wMapIndex)
{
	if (m_Party.m_cMemberNum >= PARTY::MAX_MEM) 
	{ 
		ERRLOG0(g_Log, "��Ƽ�α� : ��Ƽ ����� �� á���ϴ�.");
		return false; 
	}

	CMonster*	lpMonster = NULL;

	if (0 != wMapIndex)
	{
		VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
		if (lpVirtualArea)
		{
			CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
			if (lpVirtualMonsterMgr) lpMonster = lpVirtualMonsterMgr->GetMonster(dwSenderCID);
		}
	}
	else
	{
		lpMonster = CCreatureManager::GetInstance().GetMonster(dwSenderCID);
	}
	
	if (NULL == lpMonster)
	{
		ERRLOG2(g_Log, "��Ƽ�α� : ���� ���� �ʴ� ���͸� ��Ƽ�� ���Խ�ų���� �մϴ�. PID:0x%08, CID:0x%08x", m_Party.m_dwPartyID, dwSenderCID);
		return false;
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwSenderCID) 
		{
			ERRLOG0(g_Log, "��Ƽ�α� : �̹� �� ��Ƽ�� ����Դϴ�.");
			return false; 
		}	
	}

	m_Party.MemberCID[m_Party.m_cMemberNum] = dwSenderCID;
	m_Party.ServerID[m_Party.m_cMemberNum] = dwReferenceID;	// 0 �� ������

    LogChantBug(lpMonster, this, "��Ƽ�α� : ���� ��Ƽ ��� �߰��� �õ��մϴ�.", LOG_FFLCHANT);

	m_pMemberPointer[m_Party.m_cMemberNum] = lpMonster;
	m_PartySpellMgr.AddMember(lpMonster);

	++m_Party.m_cMemberNum;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonsterParty::Leave
//
// Description : ���Ͱ� �׾ ��Ƽ�� ���Ŵ���
//
// Inputs : dwSenderCID - ���� ����
//			dwReferenceID - ��� ����
//
// Outputs : None.
//
// Returns : 1 - ��� ����
//			 0 - ���� (������ ����ɶ� ��Ƽ �Ŵ������� ���� ������� �Ѵ�.)
//			 -1 - Ż�� ����(�α� Ȯ��)
///////////////////////////////////////////////////////////////////////////////////
int		CMonsterParty::Leave(unsigned long dwSenderCID, unsigned long dwReferenceID, unsigned short wMapIndex)
{
	for (int nLoopIndex = 0; nLoopIndex < m_Party.m_cMemberNum; ++nLoopIndex)
	{
		CMonster* lpMonster = NULL;
		if (m_Party.MemberCID[nLoopIndex] == dwSenderCID)
		{
			// �α�Ǿ� �ִ� �༮�̶�� ��ȣ������ �����ش�.
			if (0 != wMapIndex)
			{
				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
				if (lpVirtualArea)
				{
					CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
					if (lpVirtualMonsterMgr) lpMonster = lpVirtualMonsterMgr->GetMonster(dwSenderCID);
				}
			}
			else
			{
				lpMonster = CCreatureManager::GetInstance().GetMonster(dwSenderCID);
			}
			
			if (NULL != lpMonster)
			{
                LogChantBug(lpMonster, this, "��Ƽ�α� : ���� ��Ƽ ��� ���Ÿ� �õ��մϴ�.", LOG_FFLCHANT);

				m_PartySpellMgr.RemoveMember(lpMonster);

				if (lpMonster->GetPID() != m_Party.m_dwPartyID)
				{
					ERRLOG3(g_Log, "��Ƽ�α� : CID:0x%08x �߸��� ��Ƽ���� ���� ��Ƽ�Դϴ�. ������ PID:0x%08x, ���μ����ϴ� ��Ƽ�� PID:0x%08x", 
						lpMonster->GetCID(), lpMonster->GetPID(), m_Party.m_dwPartyID);
				}
				else
				{
					lpMonster->SetParty(NULL);
				}
			}

			--m_Party.m_cMemberNum;

			for (int nMoveIndex = nLoopIndex; nMoveIndex < m_Party.m_cMemberNum; ++nMoveIndex)
			{
				m_Party.MemberCID[nMoveIndex] = m_Party.MemberCID[nMoveIndex+1];
				m_Party.ServerID[nMoveIndex] = m_Party.ServerID[nMoveIndex+1];	// ��� ���ϴ� ���� ���̵�

				m_pMemberPointer[nMoveIndex] = m_pMemberPointer[nMoveIndex+1];
			}

			m_Party.MemberCID[m_Party.m_cMemberNum] = 0;
			m_Party.ServerID[m_Party.m_cMemberNum] = 0;

			m_pMemberPointer[m_Party.m_cMemberNum] = NULL;

			return 0;
		}
	}

	ERRLOG2(g_Log, "��Ƽ���� �ƴ� ���Ͱ� Ż��(��)�Ϸ� �մϴ�. PartyUID : 0x%08x, MonsterCID : 0x%08x", 
		m_Party.m_dwPartyID, dwSenderCID);

	return -1;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonsterParty::Destory
//
// Description : ��Ƽ �ı� (��Ŷ ������ ��ȣ������ ������. ��ü ���� �������.)
//
// Inputs : dwSenderCID - �ƹ��ų�
//			dwReferenceID - �ƹ��ų�
//
// Outputs : None.
//
// Returns : bool - ��������
///////////////////////////////////////////////////////////////////////////////////
bool CMonsterParty::Destory(unsigned long dwSenderCID, unsigned long dwReferenceID)
{
    LogChantBug(0, this, "��Ƽ�α� : ���� ��Ƽ ��� ���� ���Ÿ� �õ�.", LOG_FFLCHANT);

	m_PartySpellMgr.ClearMember();

	for (int nMemberIndex = 0; nMemberIndex < 2; ++nMemberIndex)
	{
		if (NULL != m_pMemberPointer[nMemberIndex])
        {
			if (m_pMemberPointer[nMemberIndex]->GetPID() != m_Party.m_dwPartyID)
			{
				ERRLOG3(g_Log, "��Ƽ�α� : CID:0x%08x �߸��� ��Ƽ���� ���� ��Ƽ�Դϴ�. ĳ������ PID:0x%08x, ���μ����ϴ� ��Ƽ�� PID:0x%08x", 
					m_pMemberPointer[nMemberIndex]->GetCID(), m_pMemberPointer[nMemberIndex]->GetPID(), m_Party.m_dwPartyID);
				continue;
			}

			m_pMemberPointer[nMemberIndex]->SetParty(NULL);
		    m_pMemberPointer[nMemberIndex]->SetPID(0);
        }
	}

	return true;
}



bool CMonsterParty::Attack(AtType attackType, CAggresiveCreature** pDefenders, unsigned char* cDefenserJudges, 
						   CAggresiveCreature* lpOffencer, float fDistance, unsigned char cTargetType)
{
	unsigned char cDefenderNum = 0;

	for (int nIndex = 0; nIndex < m_Party.m_cMemberNum; ++nIndex)
	{
		if (NULL != m_pMemberPointer[nIndex])
		{
			if (fDistance > pDefenders[0]->GetCurrentPos().GetDistance(m_pMemberPointer[nIndex]->GetCurrentPos()))
			{
				pDefenders[cDefenderNum] = m_pMemberPointer[nIndex];
				++cDefenderNum;

				if (AtNode::MAX_DEFENDER_NUM < cDefenderNum)
				{
					ERRLOG2(g_Log, "��Ƽ�α� : PID:0x%08x ��Ƽ ���ݽ� �ִ� ����ڼ��� �ʰ��Ͽ����ϴ�. Num:%d", 
						m_Party.m_dwPartyID, cDefenderNum);
					cDefenderNum = AtNode::MAX_DEFENDER_NUM;
					break;
				}
			}
		}
	}

	unsigned short wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM] = {0, };
	return lpOffencer->Attack(attackType, cDefenderNum, pDefenders, cDefenserJudges, wDefenserMPHeal);
}



CCharacterParty::CCharacterParty(const PARTY& PartyInfo, bool bCreate)
:	CParty(PartyInfo), m_pHostileParty(NULL)
{	
	std::fill_n(&m_pMemberPointer[0], int(PARTY::MAX_MEM), reinterpret_cast<CCharacter *>(NULL));
	std::fill_n(&m_bAutoRouting[0], int(PARTY::MAX_MEM), true);
	std::fill_n(&m_pFightingMember[0], int(PARTY::MAX_MEM), reinterpret_cast<CCharacter *>(NULL));

    memset(&m_partyMemberData, 0, sizeof(PartyMemberData) * PARTY::MAX_MEM);

	if (!CPartyMgr::GetInstance().AddParty(this))
    {
        ERRLOG3(g_Log, "PID:0x%08x(0x%08x) / LeaderCID:0x%08x / ��Ƽ�α� : ��Ƽ �����ڿ� ��Ƽ �߰� ����",
            PartyInfo.m_dwPartyID, this, PartyInfo.m_dwLeaderID);
    }

	unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.ServerID[nMemberIndex] == dwServerID)
		{
			CCharacter *lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_Party.MemberCID[nMemberIndex]);
			if (NULL == lpCharacter) 
			{
				ERRLOG4(g_Log, "��Ƽ�α� : (1)ĳ������ ���� ID�� �߸��Ǿ����ϴ�. PartyUID:0x%08x, CharacterCID:0x%08x(0x%08x), ServerID:0x%08x", 
					m_Party.m_dwPartyID, m_Party.MemberCID[nMemberIndex], m_Party.ServerID[nMemberIndex], dwServerID);
				continue; 
			}
				
            LogChantBug(lpCharacter, this, "��Ƽ�α� : ĳ���� ��Ƽ ��� �߰��� �õ��մϴ�.", LOG_FFLCHANT);

			m_pMemberPointer[nMemberIndex] = lpCharacter;            
			m_PartySpellMgr.AddMember(lpCharacter);

			SendPartyInfo(lpCharacter);

			if (bCreate == true)
			{
				SendPartyCmdInfo(PktPC::PC_ACCEPT, lpCharacter);
			}
		}
	}
}


CCharacterParty::CCharacterParty()
{	
}


CCharacterParty::~CCharacterParty()	
{
}

unsigned char CCharacterParty::GetRaceRate(void)
{
	unsigned char cHumanNum = 0;
	for (unsigned char cIndex = 0; cIndex < m_Party.m_cMemberNum; ++cIndex)
	{
		if (NULL != m_pMemberPointer[cIndex])
		{
			if (CClass::HUMAN == m_pMemberPointer[cIndex]->GetRace())
			{
				++cHumanNum;
			}
		}
	}

	return static_cast<unsigned char>(cHumanNum * 100.0f / m_Party.m_cMemberNum);
}

///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::Join
//
// Description : ��Ƽ�� ĳ���͸� ���Խ�Ŵ
//
// Inputs : dwSenderCID - ������ ���̵�
//			dwReferenceID - ���� ���̵�
//			strSenderName - ������ �̸�
//
// Outputs : None.
//
// Returns : bool - ��������
///////////////////////////////////////////////////////////////////////////////////
bool CCharacterParty::Join(unsigned long dwSenderCID, unsigned long dwReferenceID, const char* strSenderName, unsigned short wMapIndex)
{
	if (m_Party.m_cMemberNum >= PARTY::MAX_MEM) 
	{ 
		ERRLOG0(g_Log, "��Ƽ�α� : ��Ƽ ����� �� á���ϴ�.");
		return false; 
	}
	
	CCharacter *lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderCID);
	if (NULL != lpCharacter) 
	{ 
		if (0 != strcmp(strSenderName, lpCharacter->GetCharacterName()))
		{
			ERRLOG4(g_Log, "��Ƽ�α� : PID:0x%08x ��Ƽ�� �����ϴ� ĳ������ �̸��� �̻��մϴ�. CID:0x%08x, ��¥�̸�:%s, �����̸�:%s", 
				m_Party.m_dwPartyID, dwSenderCID, lpCharacter->GetCharacterName(), strSenderName);
			return false; 
		}

		if (dwReferenceID != CServerSetup::GetInstance().GetServerID())
		{
			ERRLOG4(g_Log, "��Ƽ�α� : PID:0x%08x ��Ƽ�� �����ϴ� ĳ������ ���� ���̵� �̻��մϴ�. CID:0x%08x, �̰���SID:0x%08x, ����SID:0x%08x", 
				m_Party.m_dwPartyID, dwSenderCID, CServerSetup::GetInstance().GetServerID(), dwReferenceID);
			return false; 
		}
	}

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwSenderCID) 
		{
			ERRLOG0(g_Log, "��Ƽ�α� : �̹� �� ��Ƽ�� ����Դϴ�.");
			return false; 
		}	
	}

	SendPartyCommand(PktPC::PC_ACCEPT, strSenderName, dwSenderCID, dwReferenceID);

	m_Party.MemberCID[m_Party.m_cMemberNum] = dwSenderCID;
	::memcpy(m_Party.Name[m_Party.m_cMemberNum], strSenderName, CHAR_INFOST::MAX_NAME_LEN);
	m_Party.ServerID[m_Party.m_cMemberNum] = dwReferenceID;

	m_pMemberPointer[m_Party.m_cMemberNum] = lpCharacter;    
	++m_Party.m_cMemberNum;

	if (lpCharacter)
	{
        LogChantBug(lpCharacter, this, "��Ƽ�α� : ĳ���� ��Ƽ ��� �߰��� �õ��մϴ�.", LOG_FFLCHANT);

		m_PartySpellMgr.AddMember(lpCharacter);
		SendPartyInfo(lpCharacter);
		SendPartyCmdInfo(PktPC::PC_ACCEPT, lpCharacter);
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::Leave
//
// Description : ��Ƽ���� Ż��(��)��
//
// Inputs : dwSenderCID - ������ or Ż����
//			dwReferenceID - 0�̸� ű. �׿ܿ��� ������ Ż��(������ Ż����. ���� Ż��ÿ� �� ����)
//
// Outputs : None.
//
// Returns : 1 - �� ������ �α�Ǿ��ִ� ��Ƽ���� ����(��Ƽ�Ŵ������� ��������)
//			 0 - ����
//			 -1 - Ż�� ����(�α� Ȯ��)
///////////////////////////////////////////////////////////////////////////////////
int CCharacterParty::Leave(unsigned long dwSenderCID, unsigned long dwReferenceID, unsigned short wMapIndex)
{
	bool bSendPacket = false;
	CCharacter* lpCharacter = NULL;

    PktPC::PartyCmd partyCmd;

	for (int nLoopIndex = 0; nLoopIndex < m_Party.m_cMemberNum; ++nLoopIndex)
	{
		if (m_Party.MemberCID[nLoopIndex] == dwSenderCID)
		{
			unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

            partyCmd = (0 == dwReferenceID) ? PktPC::PC_BANISH : PktPC::PC_SECESSION;
            SendPartyCommand(partyCmd, m_Party.Name[nLoopIndex], dwSenderCID, dwReferenceID);

			// �α�Ǿ� �ִ� �༮�̶�� ��ȣ������ �����ش�.
			lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderCID);
			if (NULL != lpCharacter) 
			{
				// ����Ʋ ���̶�� ���
				if (NULL != m_pHostileParty)
				{
					if (0 == DropMember(lpCharacter, PktDuC::DUC_DEAD))
					{
						m_pHostileParty->EndTeamBattle();
						EndTeamBattle();
					}
				}		

				SendPartyCmdInfo(partyCmd, lpCharacter);

                LogChantBug(lpCharacter, this, "ĳ���� ��Ƽ ��� ���Ÿ� �õ��մϴ�.", LOG_FFLCHANT);
				m_PartySpellMgr.RemoveMember(lpCharacter);

				if (lpCharacter->GetPID() != m_Party.m_dwPartyID)
				{
					ERRLOG3(g_Log, "��Ƽ�α� : CID:0x%08x �߸��� ��Ƽ���� ���� ��Ƽ�Դϴ�. ĳ������ PID:0x%08x, ���μ����ϴ� ��Ƽ�� PID:0x%08x", 
						lpCharacter->GetCID(), lpCharacter->GetPID(), m_Party.m_dwPartyID);
				}
				else
				{
					lpCharacter->SetParty(NULL);
					lpCharacter->SetPID(0);
				}
			}

			--m_Party.m_cMemberNum;

			for (int nMoveIndex = nLoopIndex; nMoveIndex < m_Party.m_cMemberNum; ++nMoveIndex)
			{
				m_Party.MemberCID[nMoveIndex] = m_Party.MemberCID[nMoveIndex+1];
				memcpy(m_Party.Name[nMoveIndex], m_Party.Name[nMoveIndex+1], CHAR_INFOST::MAX_NAME_LEN);
				m_Party.ServerID[nMoveIndex] = m_Party.ServerID[nMoveIndex+1];

	            m_Party.m_wClass[nMoveIndex] = m_Party.m_wClass[nMoveIndex+1];
	            m_Party.m_cLevel[nMoveIndex] = m_Party.m_cLevel[nMoveIndex+1];
	            m_Party.m_dwGID[nMoveIndex]  = m_Party.m_dwGID[nMoveIndex+1];

                m_Party.m_bAutoRouting[nMoveIndex] = m_Party.m_bAutoRouting[nMoveIndex+1];
	            m_Party.m_Position[nMoveIndex] = m_Party.m_Position[nMoveIndex+1];

				m_pMemberPointer[nMoveIndex] = m_pMemberPointer[nMoveIndex+1];
				m_bAutoRouting[nMoveIndex] = m_bAutoRouting[nMoveIndex+1];
                m_partyMemberData[nMoveIndex] = m_partyMemberData[nMoveIndex+1];
			}

			m_Party.MemberCID[m_Party.m_cMemberNum] = 0;
			memset(m_Party.Name[m_Party.m_cMemberNum], NULL, CHAR_INFOST::MAX_NAME_LEN);
			m_Party.ServerID[m_Party.m_cMemberNum] = 0;

	        m_Party.m_wClass[m_Party.m_cMemberNum] = 0;
	        m_Party.m_cLevel[m_Party.m_cMemberNum] = 0;
	        m_Party.m_dwGID[m_Party.m_cMemberNum]  = 0;

            m_Party.m_bAutoRouting[m_Party.m_cMemberNum] = true;
	        m_Party.m_Position[m_Party.m_cMemberNum].fPointX = 0.0f;
            m_Party.m_Position[m_Party.m_cMemberNum].fPointY = 0.0f;
            m_Party.m_Position[m_Party.m_cMemberNum].fPointZ = 0.0f;

			m_pMemberPointer[m_Party.m_cMemberNum] = NULL;
			m_bAutoRouting[m_Party.m_cMemberNum] = true;
            memset(m_partyMemberData + m_Party.m_cMemberNum, 0, sizeof(PartyMemberData));

			// �α�� ��Ƽ���� ������ ��ü ����
			for (int nCheckIndex = 0; nCheckIndex < m_Party.m_cMemberNum; ++nCheckIndex)
			{
				if (m_Party.ServerID[nCheckIndex] == dwServerID)
                {
					return 0;
                }
			}

			return 1;
		}
	}

	ERRLOG2(g_Log, "��Ƽ�α� : ��Ƽ���� �ƴ� ĳ���Ͱ� Ż��(��)�Ϸ� �մϴ�. PartyUID:0x%08x, CharacterCID:0x%08x", 
		m_Party.m_dwPartyID, dwSenderCID);
	return -1;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::Login
//
// Description : ��Ƽ���� �α���
//
// Inputs : dwSenderCID - �α�����
//			dwReferenceID - ���� ���̵�
//
// Outputs : None.
//
// Returns : bool - ��������
///////////////////////////////////////////////////////////////////////////////////
bool CCharacterParty::Login(unsigned long dwSenderCID, unsigned long dwReferenceID)
{
	for (int nMemberIndex=0; nMemberIndex<m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwSenderCID)
		{
			m_Party.ServerID[nMemberIndex] = dwReferenceID;

			if(!GetLeader())
			{
				SetLeader(dwSenderCID);
			}

			CCharacter *lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderCID);
			if (NULL != lpCharacter)
			{
				SendPartyCommand(PktPC::PC_LOGIN, m_Party.Name[nMemberIndex], dwSenderCID, dwReferenceID);

                LogChantBug(lpCharacter, this, "��Ƽ�α� : ĳ���� ��Ƽ ��� �߰��� �õ��մϴ�.", LOG_FFLCHANT);

			    m_pMemberPointer[nMemberIndex] = lpCharacter;
			    m_PartySpellMgr.AddMember(lpCharacter);

			    SendPartyInfo(lpCharacter);
            }
			else
			{
				SendPartyCommand(PktPC::PC_LOGIN, m_Party.Name[nMemberIndex], dwSenderCID, dwReferenceID);
			}

			return true;
		}
	}

	ERRLOG2(g_Log, "��Ƽ�α� : ��Ƽ���� �ƴ� ĳ���͸� ��Ƽ�� �α��Ű�� �մϴ�. PartyUID:0x%08x, CharacterCID:0x%08x", 
		m_Party.m_dwPartyID, dwSenderCID);

	return false;
}

///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::ReLogin
//
// Description : ��Ƽ���� ��α���
//
// Inputs :	llpCharacter - ĳ����
//
// Outputs : None.
//
// Returns : bool - ��������
///////////////////////////////////////////////////////////////////////////////////
bool CCharacterParty::ReLogin(CCharacter* lpCharacter)
{
	if (NULL != lpCharacter) 
    {
        SendPartyCommand(PktPC::PC_RELOGIN, lpCharacter->GetCharacterName(), lpCharacter->GetCID(), 0);

        SendPartyInfo(lpCharacter);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::Logout
//
// Description : ��Ƽ���� �α׾ƿ���
//
// Inputs : dwSenderCID - �α׾ƿ���
//			dwReferenceID - ���� ���̵�	// ���� ���̵� �ƴϰ� ���� ��Ƽ�� ���� ���̵� �ε� (03/11/07) by ����
//
// Outputs : None.
//
// Returns : 1 - �� ������ �α�Ǿ��ִ� ��Ƽ���� ����(��Ƽ�Ŵ������� ��������)
//			 0 - ����
//			 -1 - �α׾ƿ� ����(�α� Ȯ��)
///////////////////////////////////////////////////////////////////////////////////
int CCharacterParty::Logout(unsigned long dwSenderCID, unsigned long dwReferenceID)
{
	unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

	AddressInfo Address; 

	for (int nLoopIndex = 0; nLoopIndex < m_Party.m_cMemberNum; ++nLoopIndex)
	{
		if (m_Party.MemberCID[nLoopIndex] == dwSenderCID)
		{
            if (0 != m_pMemberPointer[nLoopIndex])
            {
                LogChantBug(m_pMemberPointer[nLoopIndex], this, "��Ƽ�α� : ĳ���� ��Ƽ ��� ���Ÿ� �õ��մϴ�.", LOG_FFLCHANT);
                m_PartySpellMgr.RemoveMember(m_pMemberPointer[nLoopIndex]);
            }

			m_Party.ServerID[nLoopIndex] = 0;
			m_pMemberPointer[nLoopIndex] = 0;
			m_bAutoRouting[nLoopIndex] = true;
            
			// �� ������ �α�� ��Ƽ���� 1�� �̻� �־�� ��Ŷ ������ ��Ƽ ��ü�� ����.
			for (int nCheckIndex = 0; nCheckIndex < m_Party.m_cMemberNum; ++nCheckIndex)
			{
				if (m_Party.ServerID[nCheckIndex] == dwServerID)
				{
					SendPartyCommand(PktPC::PC_LOGOUT, m_Party.Name[nLoopIndex], 
                        dwSenderCID, dwReferenceID);
					return 0;
				}
			}

			return 1;
		}
	}

	ERRLOG2(g_Log, "��Ƽ�α� : ��Ƽ���� �ƴ� ĳ���͸� ��Ƽ���� �α׾ƿ���Ű�� �մϴ�. PartyUID:0x%08x, CharacterCID:0x%08x", 
		m_Party.m_dwPartyID, dwSenderCID);
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::PrepareLogout
//
// Description : �߰� ������ ���� �� ���� �α׾ƿ��� �˸��� ���� ���� �����͸� �ʱ�ȭ�Ѵ�.
//				(�߰� ������ Ȯ���� �������� ���� �α׾ƿ��ؼ� ������ ĳ���͸� �����ϴ� ��츦 ����)
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CCharacterParty::PrepareLogout(unsigned long dwMemberID)
{
	for (int nLoopIndex = 0; nLoopIndex < m_Party.m_cMemberNum; ++nLoopIndex)
	{
		if (m_Party.MemberCID[nLoopIndex] == dwMemberID)
		{
            if (0 != m_pMemberPointer[nLoopIndex])
            {
                LogChantBug(m_pMemberPointer[nLoopIndex], this, "��Ƽ�α� : ĳ���� ��Ƽ ��� ���Ÿ� �õ��մϴ�.", LOG_FFLCHANT);
                m_PartySpellMgr.RemoveMember(m_pMemberPointer[nLoopIndex]);
            }

			m_pMemberPointer[nLoopIndex] = 0;
            memset(m_partyMemberData + nLoopIndex, 0, sizeof(PartyMemberData));
			return;
		}	
	}
}






///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::TransferLeader
//
// Description : ���� �絵
//
// Inputs : dwLeaderCID - �� ����
//
// Outputs : None.
//
// Returns : bool - ��������
///////////////////////////////////////////////////////////////////////////////////
bool CCharacterParty::TransferLeader(unsigned long dwLeaderCID)
{
	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (m_Party.MemberCID[nMemberIndex] == dwLeaderCID)
		{
			SendPartyCommand(PktPC::PC_TRANSFER, m_Party.Name[nMemberIndex], 
                dwLeaderCID, m_Party.m_dwLeaderID);

            m_Party.m_dwLeaderID = dwLeaderCID;
			CPartyMgr::GetInstance().DeleteFindMemberList(m_Party.m_dwPartyID);		
			return true;
		}
	}

	ERRLOG2(g_Log, "��Ƽ�α� : ��Ƽ���� �ƴ� ĳ���Ϳ��� ������ �絵�Ϸ� �մϴ�. PartyUID:0x%08x, CharacterCID:0x%08x", 
		m_Party.m_dwPartyID, dwLeaderCID);
	return false;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::GetLoggedMemberAverageLevel
//
// Description : �α����� ��Ƽ������ ��� ������ ����
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : unsigned char - ��� ����
///////////////////////////////////////////////////////////////////////////////////
unsigned char CCharacterParty::GetLoggedMemberAverageLevel(void)
{
	unsigned char   aryMemberLevel[PARTY::MAX_MEM] = {0, };
	unsigned short  nSumLevel = 0;
	unsigned char   nLoggedMemberNum = 0;

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (NULL != m_pMemberPointer[nMemberIndex]) 
        {
		    nSumLevel += m_pMemberPointer[nMemberIndex]->GetStatus().m_nLevel;
		    ++nLoggedMemberNum;
		}
	}

	if (0 == nLoggedMemberNum)
	{
		ERRLOG0(g_Log, "��Ƽ�α� : �߸��� ��Ƽ ��ü�Դϴ�. �α�� ��Ƽ������ 0�Դϴ�.");
		return 0;
	}

	return static_cast<unsigned char>(nSumLevel / nLoggedMemberNum);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::Destory
//
// Description : ��Ƽ �ı� (��Ŷ ������ ��ȣ������ ������. ��ü ���� �������.)
//
// Inputs : dwSenderCID - �ƹ��ų�
//			dwReferenceID - �ƹ��ų�
//
// Outputs : None.
//
// Returns : bool - ��������
///////////////////////////////////////////////////////////////////////////////////
bool CCharacterParty::Destory(unsigned long dwSenderCID, unsigned long dwReferenceID)
{
	if (2 != m_Party.m_cMemberNum)
    {
		ERRLOG2(g_Log, "��Ƽ�α� : ��Ƽ ����� 2���� �ƴѵ� ��ü�Ϸ� �մϴ�. PartyUID:0x%08x, ���� ��� ���� %d��.", 
			m_Party.m_dwPartyID, m_Party.m_cMemberNum);
		return false;
	}

	SendPartyCommand(PktPC::PC_DESTROY, "��Ƽ�ı�", dwSenderCID, dwReferenceID);

    LogChantBug(0, this, "��Ƽ�α� : ĳ���� ��Ƽ ��� ���� ���Ÿ� �õ��մϴ�.", LOG_FFLCHANT);
	m_PartySpellMgr.ClearMember();

	for (int nMemberIndex = 0; nMemberIndex < 2; ++nMemberIndex)
	{
		if (NULL != m_pMemberPointer[nMemberIndex])
        {
			// ����Ʋ ���̶�� ���
			if (NULL != m_pHostileParty)
			{
				if (0 == DropMember(m_pMemberPointer[nMemberIndex], PktDuC::DUC_DEAD))
				{
					m_pHostileParty->EndTeamBattle();
					EndTeamBattle();
				}
			}		

    		SendPartyCmdInfo(PktPC::PC_DESTROY, m_pMemberPointer[nMemberIndex]);

		    if (m_pMemberPointer[nMemberIndex]->GetPID() != m_Party.m_dwPartyID)
			{
				ERRLOG3(g_Log, "��Ƽ�α� : CID:0x%08x �߸��� ��Ƽ���� ���� ��Ƽ�Դϴ�. ĳ������ PID:0x%08x, ���μ����ϴ� ��Ƽ�� PID:0x%08x", 
					m_pMemberPointer[nMemberIndex]->GetCID(), m_pMemberPointer[nMemberIndex]->GetPID(), m_Party.m_dwPartyID);
				continue;
			}

			m_pMemberPointer[nMemberIndex]->SetParty(NULL);
		    m_pMemberPointer[nMemberIndex]->SetPID(0);
        }
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::GetNearMemberList
//
// Description : ��ó�� ��Ƽ�� ����Ʈ�� ����
//
// Inputs : pCell - ��ó�� ��Ƽ���� ����� �ϴ� �߽� �� ������
//			bAutoRouting - ���� ���� ���� üũ
//
// Outputs : aryNearCharacterList - ��ó�� ��Ƽ�� ����Ʈ
//			 cHighestLevel - ��ó�� ��Ƽ�� �� �ְ� ����
//
// Returns : int - ��ó�� ��Ƽ�� ��
///////////////////////////////////////////////////////////////////////////////////
int CCharacterParty::GetNearMemberList(CCell *pCell, bool bAutoRouting, CCharacter **aryNearCharacterList, unsigned char &cHighestLevel)
{
	if (NULL == pCell) { return 0; }

	int nNearPartyMemberNum = 0;
	for (int nIndex = 0; nIndex < m_Party.m_cMemberNum; ++nIndex)
	{
		if (NULL == m_pMemberPointer[nIndex]) 
        { 
			continue; 
		}

		if (0 == m_pMemberPointer[nIndex]->GetStatus().m_nNowHP)
		{
			continue;
		}

		if (true == bAutoRouting)
		{
			if (false == m_bAutoRouting[nIndex])
			{
				continue;
			}
		}

		if (pCell->IsNearCell(m_pMemberPointer[nIndex]->GetCellPos().m_lpCell))
		{
			aryNearCharacterList[nNearPartyMemberNum] = m_pMemberPointer[nIndex];
			const int nThisLevel = aryNearCharacterList[nNearPartyMemberNum]->GetStatus().m_nLevel;
			cHighestLevel = max(cHighestLevel, unsigned char(nThisLevel));
			++nNearPartyMemberNum;
		}
	}

	return nNearPartyMemberNum;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::AdjustAutoRouting
//
// Description : ���� ���� ���� ����
//
// Params : dwTargetID[In] - ���� ���
//			bSwitch[In] - On/Off
//
// Returns : bool - ���� ����
///////////////////////////////////////////////////////////////////////////////////
bool CCharacterParty::AdjustAutoRouting(unsigned long dwTargetID, bool bSwitch)
{
	bool bResult = false;

	for (int nIndex = 0; nIndex < PARTY::MAX_MEM; ++nIndex)
	{
		if (0 == dwTargetID)
		{
			m_bAutoRouting[nIndex] = bSwitch;
			bResult = true;
			continue;
		}

		if (m_Party.MemberCID[nIndex] == dwTargetID)
		{
			if (m_bAutoRouting[nIndex] == bSwitch)
			{
				ERRLOG2(g_Log, "��Ƽ�α� : CID:0x%08x ���� ���� ������ ��ũ�� ���� �ʽ��ϴ�. PID:0x%08x", 
					dwTargetID, m_Party.m_dwPartyID);
			}

			m_bAutoRouting[nIndex] = bSwitch;
			bResult = true;
			break;
		}
	}

	if (false == bResult)
	{
		ERRLOG2(g_Log, "��Ƽ�α� : CID:0x%08x ��Ƽ���� �ƴ� ĳ������ ���� ���� ������ �����Ϸ� �մϴ�. PID:0x%08x", 
			dwTargetID, m_Party.m_dwPartyID);

	}
	else
	{
		PktPC::PartyCmd partyCmd = (true == bSwitch) ? PktPC::PC_AUTOROUTING_ON : PktPC::PC_AUTOROUTING_OFF;
		SendPartyCommand(partyCmd, "�������", dwTargetID, m_Party.m_dwLeaderID);
	}

	return bResult;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCharacterParty::AutoRouting
//
// Description : ���� ����
//
// Params : lpPickkingCreature[In] - ���� �༮
//			lpItem[In] - ���� ������
//			dwGold[InOut] - ���� �ݾ�
//
// Returns : bool - ������õǾ� ��� ���� �ٲ���°�?
///////////////////////////////////////////////////////////////////////////////////
bool CCharacterParty::AutoRouting(CAggresiveCreature* lpPickkingCreature, Item::CItem* lpItem, unsigned long& dwGold)
{
	CCharacter *aryNearCharacterList[PARTY::MAX_MEM];
	unsigned char cHighestLevel = 0;

	// ���� ���ѿ� ������� ��ó ��Ƽ�� ��ο��� �й�
	const int nNearPartyMemberNum = GetNearMemberList(lpPickkingCreature->GetCellPos().m_lpCell, 
		(0 == dwGold) ? true : false, aryNearCharacterList, cHighestLevel);
	if (0 == nNearPartyMemberNum) { return false; }

	if (0 != dwGold)
	{
		unsigned long dwEachGold = dwGold / nNearPartyMemberNum;
		dwGold = dwEachGold + (dwGold - dwEachGold * nNearPartyMemberNum);

		for (int nIndex = 0; nIndex < nNearPartyMemberNum; ++nIndex)
		{
			CCharacter* lpNearCharacter = aryNearCharacterList[nIndex];
			//if (lpNearCharacter == lpPickkingCreature) { continue; }//hz wasnt commented out

			lpNearCharacter->AddGold(dwEachGold, true);
		}

		return false;
	}
	else if (NULL != lpItem)
	{
		// ���� ���� ���
		unsigned char cRandomNum = 
			static_cast<unsigned char>(Math::Random::ComplexRandom(nNearPartyMemberNum));
		CCharacter* lpOrderCharacter = aryNearCharacterList[cRandomNum];
		if(lpItem->GetPrototypeID() == 2017)//if it is a gemcube box
		{
			if (true == lpOrderCharacter->GiveItem(lpItem))
			{
				SendAutoRouting(lpOrderCharacter->GetCID(), lpItem->GetPrototypeID(), PktAutoRouting::ARC_POSSIBLE);
				// GiveItem ���� ���õ� ���, AutoRouting() ȣ���� ������ Item �� �����Ѵ�.
				return true;
			}
		}

		/*if (lpPickkingCreature == lpOrderCharacter)
		{
			return false; hz wasnt commented out
		}
		else
		{*/
			if (true == lpOrderCharacter->GiveItem(lpItem))
			{
				SendAutoRouting(lpOrderCharacter->GetCID(), lpItem->GetPrototypeID(), PktAutoRouting::ARC_POSSIBLE);
				// GiveItem ���� ���õ� ���, AutoRouting() ȣ���� ������ Item �� �����Ѵ�.
				return true;
			}
		//}
	}
	
	return false;
}


bool CCharacterParty::Attack(AtType attackType, CAggresiveCreature** pDefenders, unsigned char* cDefenserJudges, 
							 CAggresiveCreature* lpOffencer, float fDistance, unsigned char cTargetType)
{
	unsigned char cDefenderNum = 0;

	for (int nIndex = 0; nIndex < m_Party.m_cMemberNum; ++nIndex)
	{
		if (NULL != m_pMemberPointer[nIndex])
		{
			if (fDistance > pDefenders[0]->GetCurrentPos().GetDistance(m_pMemberPointer[nIndex]->GetCurrentPos()))
			{
				if (true == m_pMemberPointer[nIndex]->GetEnchantInfo().GetFlag(Skill::SpellID::Hide)) 
				{
					continue;
				}

				if (Skill::Target::FRIEND == cTargetType)
				{
					if (EnemyCheck::EC_FRIEND != lpOffencer->IsEnemy(m_pMemberPointer[nIndex]))
					{
						continue;
					}

					if (lpOffencer->GetMapIndex() != m_pMemberPointer[nIndex]->GetMapIndex())
					{
						continue;
					}
				}

				pDefenders[cDefenderNum] = m_pMemberPointer[nIndex];
				++cDefenderNum;

				if (AtNode::MAX_DEFENDER_NUM < cDefenderNum)
				{
					ERRLOG2(g_Log, "��Ƽ�α� : PID:0x%08x ��Ƽ ���ݽ� �ִ� ����ڼ��� �ʰ��Ͽ����ϴ�. Num:%d", 
						m_Party.m_dwPartyID, cDefenderNum);
					cDefenderNum = AtNode::MAX_DEFENDER_NUM;
					break;
				}
			}
		}
	}

	unsigned short wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM] = {0, };
	return lpOffencer->Attack(attackType, cDefenderNum, pDefenders, cDefenserJudges, wDefenserMPHeal);
}


bool CCharacterParty::StartTeamBattle(CCharacterParty* pHostileParty)
{
	if (NULL != m_pHostileParty) { return false; }

	m_pHostileParty = pHostileParty;

	int nFightingMemberNum = 0;
	for (int nIndex = 0; nIndex < PARTY::MAX_MEM; ++nIndex)
	{
		if (NULL != m_pMemberPointer[nIndex])
		{
			m_pFightingMember[nFightingMemberNum] = m_pMemberPointer[nIndex];
			++nFightingMemberNum;
		}
	}

	const int MAX_BUFFER = sizeof(PktTBInfo) + PARTY::MAX_MEM * (sizeof(unsigned long) + sizeof(unsigned char));
	char Buffer[MAX_BUFFER];
	unsigned short dwBufferSize = 0;

	if (true == pHostileParty->MakeTeamBattleInfo(Buffer, dwBufferSize, NULL, PktTBInfo::DUC_PARTY_READY))
	{
		SendAllLoggedMember(Buffer, dwBufferSize, 0, CmdCharTeamBattleInfo);
	}

	return true;
}

int CCharacterParty::DropMember(CCharacter* pDropMember, PktDuC::DuelCmd eCmd)
{
	if (NULL == pDropMember) { return -1; }

	for (int nIndex = 0; nIndex < PARTY::MAX_MEM; ++nIndex)
	{
		if (pDropMember == m_pFightingMember[nIndex])
		{
			CGameClientDispatch* lpDispatch = pDropMember->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharDuelCmd(lpDispatch->GetSendStream(), 
					pDropMember->GetCID(), m_pHostileParty->GetLeader(), eCmd, PktDuC::NO_SERVER_ERR);
			}
			m_pHostileParty->SendDropMember(pDropMember, eCmd);

			int nRemainMemberNum = 0;
			for (nRemainMemberNum = nIndex; nRemainMemberNum < PARTY::MAX_MEM-1; ++nRemainMemberNum)
			{
				m_pFightingMember[nRemainMemberNum] = m_pFightingMember[nRemainMemberNum+1];
				if (NULL == m_pFightingMember[nRemainMemberNum+1]) 
				{ 
					return nRemainMemberNum;
				}
			}

			m_pFightingMember[PARTY::MAX_MEM-1] = NULL;
			return PARTY::MAX_MEM-1;
		}
	}

	return -1;
}

void CCharacterParty::EndTeamBattle(void)
{
	if (NULL == m_pHostileParty)
	{
		ERRLOG0(g_Log, "��Ƽ�α� : ����Ʋ�� �ϰ� ���� �ʴ� ��Ƽ�Դϴ�.");
	}

	for (int nIndex = 0; nIndex < PARTY::MAX_MEM; ++nIndex)
	{
		if (NULL == m_pFightingMember[nIndex]) { break; }

		CGameClientDispatch* lpDispatch = m_pFightingMember[nIndex]->GetDispatcher();
		if (NULL != lpDispatch)
		{
			GameClientSendPacket::SendCharDuelCmd(lpDispatch->GetSendStream(), 
				m_pFightingMember[nIndex]->GetCID(), m_pFightingMember[nIndex]->GetCID(), PktDuC::DUC_CANCEL, PktDuC::NO_SERVER_ERR);
		}		
	}

	m_pHostileParty = NULL;
	std::fill_n(&m_pFightingMember[0], int(PARTY::MAX_MEM), reinterpret_cast<CCharacter *>(NULL));
}

bool CCharacterParty::MakeTeamBattleInfo(char* szPacket, unsigned short& dwPacketSize, CCharacter* pChallenger, unsigned char cCmd)
{
	PktTBInfo* lpPktTBInfo = reinterpret_cast<PktTBInfo*>(szPacket);
	dwPacketSize = sizeof(PktTBInfo);

	if (NULL != pChallenger)
	{
		lpPktTBInfo->m_dwCharID = pChallenger->GetCID();
		::strncpy(lpPktTBInfo->m_strChallengerName, pChallenger->GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
	}

	lpPktTBInfo->m_cCmd = cCmd;
	lpPktTBInfo->m_cMemberNum = 0;

	unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();
	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if (NULL != lpCharacter) 
		{
			if (m_Party.ServerID[nMemberIndex] == dwServerID)
			{
				unsigned long* dwCID = reinterpret_cast<unsigned long*>(szPacket + sizeof(PktTBInfo) + 
					lpPktTBInfo->m_cMemberNum * (sizeof(unsigned long) + sizeof(unsigned char)));
				unsigned char* cLevel = reinterpret_cast<unsigned char*>(szPacket + sizeof(PktTBInfo) + 
					lpPktTBInfo->m_cMemberNum * (sizeof(unsigned long) + sizeof(unsigned char)) + sizeof(unsigned long));

				*dwCID = lpCharacter->GetCID();
				*cLevel = lpCharacter->GetStatus().m_nLevel;

				++lpPktTBInfo->m_cMemberNum;
				dwPacketSize += sizeof(unsigned long) + sizeof(unsigned char);
			}
		}
	}

	return PacketWrap::WrapCrypt(szPacket, dwPacketSize, CmdCharTeamBattleInfo, 0, 0);
}

void CCharacterParty::MovePos(POS NewPos, char cZone, const bool bSitDown)
{
	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
        CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if (NULL != lpCharacter) 
		{
			lpCharacter->MovePos(NewPos, cZone, bSitDown);
		}
	}
}

void CCharacterParty::MoveZone(POS NewPos, char Zone, char Channel)
{
	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
        CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if (NULL != lpCharacter) 
		{
			lpCharacter->MoveZone(NewPos, Zone, Channel);
		}
	}
}


// ----------------------------------------------------------------------------------------
// Send ���� �޼ҵ�

void CCharacterParty::SendPartyCommand(PktPC::PartyCmd Command, const char *SenderName_In, 
                                       unsigned long dwSenderCID, unsigned long dwReferenceID)
{
	PktPCAck    pktPCAck;

	pktPCAck.m_cCmd                 = Command;
	pktPCAck.m_dwPartyID            = m_Party.m_dwPartyID;	

	strncpy(pktPCAck.m_strSenderName, SenderName_In, CHAR_INFOST::MAX_NAME_LEN);

	pktPCAck.m_dwSenderID           = dwSenderCID;
	pktPCAck.m_dwReferenceID        = dwReferenceID;
	memset(&pktPCAck.m_SenderAddressInfo, 0, sizeof(AddressInfo));

	char* szPacket = reinterpret_cast<char*>(&pktPCAck);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktPCAck), CmdCharPartyCmd, 0, 0))
	{
		const unsigned long dwExclusion = (Command == PktPC::PC_LOGIN) ? dwSenderCID : 0;
		SendAllLoggedMember(szPacket, sizeof(PktPCAck), dwExclusion, CmdCharPartyCmd);
	}
}


void CCharacterParty::SendPartyInfo(CCharacter *lpCharacter)
{
	if (NULL != lpCharacter)
	{
		CPartyMgr::GetInstance().DeleteFindPartyList(lpCharacter->GetCID());

		lpCharacter->SetParty(this);
		lpCharacter->SetPID(m_Party.m_dwPartyID);

		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

		if (NULL != lpDispatch)
		{
			CSendStream& SendStream = lpDispatch->GetSendStream();

			char* lpBuffer = SendStream.GetBuffer(sizeof(PktPI));
			if (NULL != lpBuffer)
			{
				PktPI* lpPktPI = reinterpret_cast<PktPI*>(lpBuffer);

				PARTY_EX PartyEx(m_Party);
				for (int nIndex = 0; nIndex < m_Party.m_cMemberNum; ++nIndex)
				{
                    CCharacter* lpMember = 0;
                    if (0 != m_Party.MemberCID[nIndex] && 
                       (0 != (lpMember = CCreatureManager::GetInstance().GetCharacter(m_Party.MemberCID[nIndex]))))
					{
                        m_pMemberPointer[nIndex] = lpMember;						
						PartyEx.m_cLevel[nIndex] = lpMember->GetStatus().m_nLevel;
						PartyEx.m_wClass[nIndex] = lpMember->GetClass();
                        PartyEx.m_bAutoRouting[nIndex] = m_bAutoRouting[nIndex];
					}
                    else
                    {
                        m_pMemberPointer[nIndex] = 0;
                    }
                }

                lpPktPI->m_Party = PartyEx;
				SendStream.WrapCrypt(sizeof(PktPI), CmdCharPartyInfo, 0, 0);
			}

            // ����� �ٸ� �����͸� ������ ���� �ѱ��.
            const unsigned short MAX_PACKET_SIZE = (sizeof(PartyMemberData) + 
                sizeof(unsigned long)) * PARTY::MAX_MEM * 2 + sizeof(PktBase);

            char szPacket[MAX_PACKET_SIZE];
            char* szPacketPos = szPacket + sizeof(PktBase);

            unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();
            for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
            {
                if (m_Party.ServerID[nMemberIndex] == dwServerID && 
                    0 != m_Party.MemberCID[nMemberIndex] &&
                    0 != m_pMemberPointer[nMemberIndex])
                {                    
                    // �����͸� �����ϰ�, ������ ���� Flag�� Set�Ѵ�.
                    SetUpdateFlag(m_partyMemberData[nMemberIndex], *m_pMemberPointer[nMemberIndex], szPacketPos, true);
                }
            }

            unsigned short usPacketSize = static_cast<unsigned short>(szPacketPos - szPacket);

            if (sizeof(PktBase) < usPacketSize && 
                PacketWrap::WrapCrypt(szPacket, usPacketSize, CmdCharPartyMemInfo, 0, 0))
            {        
                SendStream.PutBuffer(szPacket, usPacketSize, CmdCharPartyMemInfo);
            }

            // �� �����͸� �ٸ� ����鿡�� ���� ������.            
            for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
            {
                if (m_Party.MemberCID[nMemberIndex] == lpCharacter->GetCID())
                {
                    szPacketPos = szPacket + sizeof(PktBase);
                    SetUpdateFlag(m_partyMemberData[nMemberIndex], 
                        *m_pMemberPointer[nMemberIndex], szPacketPos, true);

                    break;
                }
            }

            usPacketSize = static_cast<unsigned short>(szPacketPos - szPacket);
            if (sizeof(PktBase) < usPacketSize && 
                PacketWrap::WrapCrypt(szPacket, usPacketSize, CmdCharPartyMemInfo, 0, 0))
            {
                for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
                {
                    if (m_Party.ServerID[nMemberIndex] == dwServerID && 
                        0 != m_Party.MemberCID[nMemberIndex] &&
                        0 != m_pMemberPointer[nMemberIndex] &&
                        lpCharacter != m_pMemberPointer[nMemberIndex])
                    {                    
                        // �����͸� �����Ѵ�.                        
                        CGameClientDispatch* lpDispatch = 
                            m_pMemberPointer[nMemberIndex]->GetDispatcher();

                        if (0 != lpDispatch)
                        {
                            lpDispatch->GetSendStream().PutBuffer(
                                szPacket, usPacketSize, CmdCharPartyMemInfo);
                        }
                    }
                }
            }
        }
	}
}


void CCharacterParty::SendPartyCmdInfo(PktPC::PartyCmd Command, CCharacter* lpCharacter)
{
    CCell* lpCell = 0;
	if (0 != lpCharacter && 
        0 != (lpCell = lpCharacter->GetCellPos().m_lpCell))
	{
		PktPCInfo   pktPCInfo;
        memset(&pktPCInfo, 0, sizeof(PktPCInfo));

		pktPCInfo.m_cCmd        = Command;
		pktPCInfo.m_dwPartyID   = m_Party.m_dwPartyID;
		pktPCInfo.m_dwMemberID  = lpCharacter->GetCID();

		lpCell->SendAllNearCellCharacter(&pktPCInfo, sizeof(PktPCInfo), CmdCharPartyCmdInfo);    
	}
}


void CCharacterParty::SendAllLoggedMember(const char* szPacket, unsigned long dwPacketSize, 
                                          unsigned long dwExclusion, unsigned char cCMD_In)
{
	unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		if (0 != dwExclusion && dwExclusion == m_Party.MemberCID[nMemberIndex]) 
        {
            continue; 
        }

		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if (NULL != lpCharacter) 
		{
			if (m_Party.ServerID[nMemberIndex] != dwServerID)
			{
				ERRLOG4(g_Log, "��Ƽ�α� : (2)ĳ������ ���� ID�� �߸��Ǿ����ϴ�. PartyUID:0x%08x, CharacterCID:0x%08x(0x%08x), ServerID:0x%08x", 
					m_Party.m_dwPartyID, m_Party.MemberCID[nMemberIndex], m_Party.ServerID[nMemberIndex], dwServerID);
			}
			else
			{
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->GetSendStream().PutBuffer(szPacket, dwPacketSize, cCMD_In);
				}
			}
		}
	}
}

bool CCharacterParty::SendMemberInfoOutNZone(unsigned long dwCID, unsigned short wCmd)
{
	unsigned long dwGameServerID = CServerSetup::GetInstance().GetServerID();

	for(int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if(lpCharacter)
		{
			// ĳ���Ϳ��� ���� ���� ���� ĳ���� ������ �����ش� //			
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

			if(lpDispatch)
			{																				
				// ������ �α׾ƿ� ������ ������ //
				char* lpBuffer = lpDispatch->GetSendStream().GetBuffer(sizeof(PktPMD));

				PktPMD* pktPMD = reinterpret_cast<PktPMD*>(lpBuffer);

				pktPMD->m_dwPartyID		= m_Party.m_dwPartyID;								
				pktPMD->m_dwSenderID	= dwCID;
				pktPMD->m_wCmd			= wCmd;

				if (!lpDispatch->GetSendStream().WrapCrypt(sizeof(PktPMD), 
                    CmdCharPartyMemData, 0, PktBase::NO_SERVER_ERR))
                {
					return false;
                }
			}
		}
	}
	return false;
}

bool CCharacterParty::SendMemberInfoAllNZone(unsigned long dwCID)
{
	unsigned long dwGameServerID = CServerSetup::GetInstance().GetServerID();

	for(int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if(lpCharacter)
		{
			// ĳ���Ϳ��� ���� ���� ���� ĳ���� ������ �����ش� //
			if(lpCharacter->GetCID()==dwCID)
			{
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

				if(lpDispatch)
				{
					for(int nIndex = 0; nIndex < m_Party.m_cMemberNum; ++nIndex)
					{
						if(m_Party.MemberCID[nIndex])
						{														
							if(dwGameServerID!=m_Party.ServerID[nIndex])
							{
								// ������ ������.
								char* lpBuffer = lpDispatch->GetSendStream().GetBuffer(sizeof(PktPMD));

								PktPMD* pktPMD = reinterpret_cast<PktPMD*>(lpBuffer);

								pktPMD->m_dwPartyID		= m_Party.m_dwPartyID;
								pktPMD->m_dwSenderID	= m_Party.MemberCID[nIndex];
								pktPMD->m_dwGID			= m_Party.m_dwGID[nIndex];
								pktPMD->m_dwServerID	= m_Party.ServerID[nIndex];
								pktPMD->m_wClass		= m_Party.m_wClass[nIndex];
								pktPMD->m_cLevel		= m_Party.m_cLevel[nIndex];
								pktPMD->m_wCmd			= PktDD::SCmdLoginPartyMem;

								if (!lpDispatch->GetSendStream().WrapCrypt(sizeof(PktPMD), 
                                    CmdCharPartyMemData, 0, PktBase::NO_SERVER_ERR))
                                {
									return false;
                                }
							}
						}
					}
				}

				return true;
			}
		}
	}

	return false;
}

void CCharacterParty::SendMemberInfoNZone(const char* szPacket, unsigned long dwPacketSize, unsigned long dwSenderID, unsigned char cCMD_In)
{
	unsigned long	dwGameServerID	= CServerSetup::GetInstance().GetServerID();
	unsigned long	dwServerID		= 0;

	for(int nIndex = 0; nIndex < m_Party.m_cMemberNum; ++nIndex)
	{
		if(dwSenderID==m_Party.MemberCID[nIndex])
		{
			dwServerID = m_Party.ServerID[nIndex];
			break;
		}		
	}

	for(int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if(lpCharacter)
		{
			// �ٸ� ���� �ִ� ĳ���� ������ ������Ʈ �Ǿ����� //
			if(dwGameServerID!=dwServerID)
			{
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

				if(lpDispatch)
				{
					lpDispatch->GetSendStream().PutBuffer(szPacket, dwPacketSize, cCMD_In);
				}
			}			
		}
	}
}

void CCharacterParty::SendNotNearCellMember(const char* szPacket, unsigned long dwPacketSize, 
                                            CCell* lpCell, unsigned char cCMD_In)
{
    unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

    for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
    {
        CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];
        if (0 != m_Party.MemberCID[nMemberIndex] &&             
            dwServerID == m_Party.ServerID[nMemberIndex] && 
            0 != lpCharacter) 
        {
            CCell* lpMemberCell = lpCharacter->GetCellPos().m_lpCell;
            CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
            if (0 != lpDispatch && 0 != lpMemberCell && !lpMemberCell->IsNearCell(lpCell))
            {
                lpDispatch->GetSendStream().PutBuffer(
                    szPacket, dwPacketSize, cCMD_In);
            }
        }
    }
}

/*void CCharacterParty::SendDivisionExp(CCharacter* lpCharacter, CAggresiveCreature* lpDeadCreature, 
									  unsigned long dwExp, int nStandardLevel)
{
	//Minotaurs ��Ƽ����ġ (?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)
	CCharacter *aryNearCharacterList[PARTY::MAX_MEM];
	unsigned char cHighestLevel = 0;

	const int nNearPartyMemberNum = 
		GetNearMemberList(lpDeadCreature->GetCellPos().m_lpCell, false, aryNearCharacterList, cHighestLevel);
	if (0 == nNearPartyMemberNum) 
	{ 
		ERRLOG1(g_Log, "��Ƽ�α� : ����ġ�� ������ ��Ƽ���� �����. Party ID:0x%08x", m_Party.m_dwPartyID);
		return; 
	}

	for (int nIndex = 0; nIndex < nNearPartyMemberNum; ++nIndex)
	{
		CCharacter* lpNearCharacter = aryNearCharacterList[nIndex];

		// ���� �й� : ��Ƽ �� ����ġ * (EXP_BALANCE_HANDLE - (��Ƽ�� �� �ְ� ���� - �ڽ��� ����)) / (�ֺ��� �ִ� ��Ƽ�� �� * EXP_BALANCE_HANDLE) * ������
		int nFriendlyLevelGap = CThreat::EXP_BALANCE_HANDLE - (cHighestLevel - lpNearCharacter->GetStatus().m_nLevel);

		nFriendlyLevelGap = (nFriendlyLevelGap < 0) ? 0 : nFriendlyLevelGap;

		unsigned long dwIndividualExp = static_cast<unsigned long>( (dwExp * nFriendlyLevelGap) / 
			(nNearPartyMemberNum * CThreat::EXP_BALANCE_HANDLE) * lpDeadCreature->GetThreat().GetAggravation(lpNearCharacter) );

/*
		// �յ� �й� : ��Ƽ �� ����ġ / �ֺ��� �ִ� ��Ƽ�� ��
		unsigned long dwIndividualExp = dwExp / nNearPartyMemberNum;
*/

	/*	if (::abs(nStandardLevel - lpNearCharacter->GetStatus().m_nLevel) > CThreat::EXP_CONSENT_GAP || dwIndividualExp < 1) 
		{ 
			dwIndividualExp = 1; 
		}
//		if(lpCharacter->GetLevel() >(unsigned char)(lpDeadCreature->GetStatus().m_nLevel + 4))//hayzohar
//				return;

		//if(lpNearCharacter->GetLevel() < 50)//hz exp up to lvl 50
			lpNearCharacter->GetHuntingExp(lpDeadCreature, dwIndividualExp, nNearPartyMemberNum);
	}
}*/

void CCharacterParty::SendDivisionExp(CCharacter* lpCharacter, CAggresiveCreature* lpDeadCreature, 
									  unsigned long dwExp, int nStandardLevel)
{
	//Minotaurs ��Ƽ����ġ (?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?)(?) peter you stupid? minotaur has nothing to do with lvling
	CCharacter *aryNearCharacterList[PARTY::MAX_MEM];
	unsigned char cHighestLevel = 0;

	const int nNearPartyMemberNum = 
		GetNearMemberList(lpDeadCreature->GetCellPos().m_lpCell, false, aryNearCharacterList, cHighestLevel);
	if (0 == nNearPartyMemberNum) 
	{ 
		ERRLOG1(g_Log, "��Ƽ�α� : ����ġ�� ������ ��Ƽ���� �����. Party ID:0x%08x", m_Party.m_dwPartyID);
		return; 
	}

	for (int nIndex = 0; nIndex < nNearPartyMemberNum; ++nIndex)
	{
		CCharacter* lpNearCharacter = aryNearCharacterList[nIndex];

		// ���� �й� : ��Ƽ �� ����ġ * (EXP_BALANCE_HANDLE - (��Ƽ�� �� �ְ� ���� - �ڽ��� ����)) / (�ֺ��� �ִ� ��Ƽ�� �� * EXP_BALANCE_HANDLE) * ������
		int nFriendlyLevelGap = CThreat::EXP_BALANCE_HANDLE - (cHighestLevel - lpNearCharacter->GetStatus().m_nLevel);

		nFriendlyLevelGap = (nFriendlyLevelGap < 0) ? 0 : nFriendlyLevelGap;

		unsigned long dwIndividualExp = static_cast<unsigned long>( (dwExp * nFriendlyLevelGap) / 
			(nNearPartyMemberNum * CThreat::EXP_BALANCE_HANDLE) * lpDeadCreature->GetThreat().GetAggravation(lpNearCharacter) );

/*
		// �յ� �й� : ��Ƽ �� ����ġ / �ֺ��� �ִ� ��Ƽ�� ��
		unsigned long dwIndividualExp = dwExp / nNearPartyMemberNum;
*/

		if (::abs(nStandardLevel - lpNearCharacter->GetStatus().m_nLevel) > CThreat::EXP_CONSENT_GAP || dwIndividualExp < 1) 
		{ 
			dwIndividualExp = 1; 
		}

		lpNearCharacter->GetHuntingExp(lpDeadCreature, dwIndividualExp, nNearPartyMemberNum);
	}
}

void CCharacterParty::SendDropMember(CCharacter* pDropMember, PktDuC::DuelCmd eCmd)
{
	for (int nIndex = 0; nIndex < PARTY::MAX_MEM; ++nIndex)
	{
		if (NULL == m_pFightingMember[nIndex]) { break; }

		CGameClientDispatch* lpDispatch = m_pFightingMember[nIndex]->GetDispatcher();
		if (NULL == lpDispatch) { continue; }

		GameClientSendPacket::SendCharDuelCmd(lpDispatch->GetSendStream(), 
			pDropMember->GetCID(), m_pFightingMember[nIndex]->GetCID(), eCmd, PktDuC::NO_SERVER_ERR);		
	}
}

void CCharacterParty::SendRecall(CCharacter* lpCaster)
{
	PktAPAck pktAPAck;

	pktAPAck.m_dwCasterID = lpCaster->GetCID();
	strncpy(pktAPAck.m_szCasterName, lpCaster->GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
	pktAPAck.m_cCmd = PktAP::AP_RECALL;

	char* szPacket = reinterpret_cast<char*>(&pktAPAck);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktAPAck), CmdCharAuthorizePanel, 0, PktBase::NO_SERVER_ERR))
	{
		unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

		for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
		{
			CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];
			if (NULL != lpCharacter) 
			{
				if (lpCaster == lpCharacter)
				{
					// �ڱ� �ڽ��� �������� �ʴ´�.
					continue;
				}

				// edith 2009.04.26 ���Ͽ����, ĳ���Ͱ� �ǽ����� Ÿ���� �ǽ���尡 �ƴϸ� ���ݺҰ�
				if (true == lpCaster->IsPeaceMode() && false == lpCharacter->IsPeaceMode())
				{
					// ĳ���Ͱ� �ǽ����� Ÿ���� �ǽ���尡 �ƴϸ� ���ݺҰ�
					continue;
				}

				if (lpCharacter->IsRideArms())
				{
					// ���⿡ ž���� ������ �������� �ʴ´�.
					continue;
				}

				if (true == lpCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
				{
					// Hide ���� ĳ���ʹ� ���� �ȵȴ�.
					continue;
				}

				if (lpCharacter->GetRejectOption().Reject.m_Recall == 1)
				{
					// ���ݰźλ���
					continue;
				}

				if (EnemyCheck::EC_FRIEND != lpCaster->IsEnemy(lpCharacter))
				{
					// �Ʊ��� �����ϵ��� �Ѵ�.
					continue;
				}

                if (lpCaster->GetMapIndex() != lpCharacter->GetMapIndex())
				{
					// �ٸ� ���� ���� ������ ������ �Ұ����ϴ�.
					continue;
				}

				if (m_Party.ServerID[nMemberIndex] != dwServerID)
				{
					ERRLOG4(g_Log, "��Ƽ�α� : (3)ĳ������ ���� ID�� �߸��Ǿ����ϴ�. PartyUID:0x%08x, CharacterCID:0x%08x(0x%08x), ServerID:0x%08x", 
						m_Party.m_dwPartyID, m_Party.MemberCID[nMemberIndex], m_Party.ServerID[nMemberIndex], dwServerID);
				}
				else
				{
					CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
					if (NULL != lpDispatch)
					{
						lpDispatch->GetSendStream().PutBuffer(szPacket, sizeof(PktAPAck), CmdCharAuthorizePanel);
					}
				}
			}
		}
	}
}

void CCharacterParty::SendAutoRouting(unsigned long dwCharID, unsigned short wItemID, unsigned char cCmd)
{
	PktAutoRouting pktAR;

	pktAR.m_dwCharID = dwCharID;
	pktAR.m_nObjectID = 0;

	pktAR.m_wItemID = wItemID;
	pktAR.m_cNum = 0;

	pktAR.m_cCmd = cCmd;

	char* szPacket = reinterpret_cast<char*>(&pktAR);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktAutoRouting), CmdCharAutoRouting, 0, 0))
	{
		SendAllLoggedMember(szPacket, sizeof(PktAutoRouting), dwCharID, CmdCharAutoRouting);
	}
}

void CCharacterParty::SendPartyMemberDataToDBAgent(unsigned long dwSenderID, unsigned long dwGID, unsigned short wClass, unsigned long dwServerID, 
												   char cLevel, const char* strSenderName, unsigned short usCmd)
{
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 == lpDBAgentDispatch)
	{
		ERRLOG0(g_Log, "��Ƽ�α� : ������Ʈ ��� ����.");                            
	}
	else
	{
		GameClientSendPacket::SendPartyMemberData(lpDBAgentDispatch->GetSendStream(), GetUID(),
									dwSenderID, dwGID, wClass, dwServerID, cLevel, strSenderName, usCmd);
	}
}

void CCharacterParty::SendDivisionFame(CCharacter* lpWinnerCharacter, CCharacter* lpLoserCharacter, 
									   unsigned char cCmd, unsigned char cAcquireAmount)
{
	
	CCharacter* aryNearCharacterList[PARTY::MAX_MEM] = { 0, };
	unsigned char cHighestLevel = 0;

	const int nNearPartyMemberNum = 
		GetNearMemberList(lpWinnerCharacter->GetCellPos().m_lpCell, false, aryNearCharacterList, cHighestLevel);
	if (0 == nNearPartyMemberNum) 
	{ 
		ERRLOG1(g_Log, "��Ƽ�α� : ���� ������ ��Ƽ���� �����. Party ID : 0x%08x", m_Party.m_dwPartyID);
		return; 
	}

	CCharacterParty* lpLoserParty = reinterpret_cast<CCharacterParty*>(lpLoserCharacter->GetParty());
	CCharacterParty* lpWinnerParty = reinterpret_cast<CCharacterParty*>(lpWinnerCharacter->GetParty());
	GAMELOG::ERRType eReturnCode = 0;

	for (unsigned char cMemberIndex = 0; cMemberIndex < nNearPartyMemberNum; ++cMemberIndex)
	{
		unsigned char cCmdPerMember = cCmd;
		CCharacter* lpCharacter = aryNearCharacterList[cMemberIndex];
		if (NULL != lpCharacter)
		{
			// edith 2009.12.26 ���� 20�̻��̸� �׾����� ��ó���� ���� �ʴ´�.
			// ���� �޴� ������ ������ ���� �������� 20���̻� ������ ���� �����ʴ´�.
			unsigned char cWinCmd = cCmd;

			int iLevelGap = lpCharacter->GetLevel() - lpLoserCharacter->GetLevel();
			if (iLevelGap >= 20)
				cWinCmd = PktFIAck::FAME_WIN_LEVELMIN;

			unsigned long dwPrevFame = 0;
			unsigned long dwNextFame = 0;
			unsigned long dwPrevMileage = 0;
			unsigned long dwNextMileage = 0;

			// �Ҽ��� ���� �ø�
			unsigned long dwGetAward = static_cast<unsigned long>(ceil(cAcquireAmount + static_cast<float>(nNearPartyMemberNum))); // was /         ---

			if(PktFIAck::FAME_WIN == cWinCmd)
			{
				unsigned long dwPrevFame = lpCharacter->GetFame();
				unsigned long dwGetFame = std::min(dwGetAward * static_cast<unsigned long>(CServerSetup::GetInstance().GetFameDefault() / 500.0f), 
					UINT_MAX - lpCharacter->GetFame());
				unsigned long dwNextFame = dwPrevFame + dwGetFame;

				lpCharacter->SetFame(dwNextFame);
			}

			if (PktFIAck::FAME_WIN == cWinCmd || PktFIAck::FAME_WIN_LEVELMIN == cWinCmd )
			{
				unsigned long dwPrevMileage = lpCharacter->GetMileage();
				unsigned long dwNextMileage = dwPrevMileage + 10;

				// edith 2009.06.27 �޴��� ������, ��������� ���� ���϶��� ��´�.
				if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
				{
					// ��� ���� �ð��̸�
					if (CGameTimeMgr::GetInstance().IsGuildWarTime())
					{
						unsigned long dwGetMileage = std::min(dwGetAward * static_cast<unsigned long>(CServerSetup::GetInstance().GetMileageDefault() / 500.0f), 
							UINT_MAX - lpCharacter->GetMileage());
						dwNextMileage = dwPrevMileage + 10;
					}
				}
				else if (SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
				{
					// ���� ���� �ð��� �ƴ϶��, ���� ��æƮ ȿ���� �ɾ��ش�.
					if (CGameTimeMgr::GetInstance().IsRealmWarTime())//mileage reward hayzohar
					{
						unsigned long dwGetMileage = std::min(dwGetAward * static_cast<unsigned long>(CServerSetup::GetInstance().GetMileageDefault() / 500.0f), 
							UINT_MAX - lpCharacter->GetMileage());

						dwNextMileage = dwPrevMileage + dwGetMileage;
					}
				}

				lpCharacter->SetMileage(dwNextMileage);

				GAMELOG::LogFameGetBattle(*lpCharacter, lpLoserCharacter, lpWinnerParty, 
					dwPrevFame, dwNextFame, dwPrevMileage, dwNextMileage, eReturnCode); 
			}

			// ���� ����ڰ� �ƴ� ��Ƽ������ ������ �޼����� ����.
			if (lpWinnerCharacter != lpCharacter)
			{
				switch (cWinCmd)
				{
					case PktFIAck::FAME_LOSE:			cCmdPerMember = PktFIAck::FAME_LOSE_MEMBER;				break;
					case PktFIAck::FAME_WIN_DUEL:		cCmdPerMember = PktFIAck::FAME_WIN_MEMBER_DUEL;			break;
					case PktFIAck::FAME_WIN_LARGE_GAP:	cCmdPerMember = PktFIAck::FAME_WIN_MEMBER_LARGE_GAP;	break;
					case PktFIAck::FAME_WIN_CONTINUE:	cCmdPerMember = PktFIAck::FAME_WIN_MEMBER_CONTINUE;		break;
					case PktFIAck::FAME_WIN:			cCmdPerMember = PktFIAck::FAME_WIN_MEMBER;				break;
					case PktFIAck::FAME_WIN_LEVELMIN:	cCmdPerMember = PktFIAck::FAME_WIN_MEMBER_LEVELMIN;		break;
				}
			}

			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				if (PktFIAck::FAME_LOSE_MEMBER == cCmdPerMember)
				{
					GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpCharacter, 
						lpWinnerCharacter->GetCharacterName(), lpLoserCharacter->GetCharacterName(),
						lpWinnerCharacter->GetNation(), (unsigned char)lpWinnerCharacter->GetClass(),
						cCmdPerMember, PktBase::NO_SERVER_ERR);
				}
				else
				{
					GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpCharacter, 
						lpWinnerCharacter->GetCharacterName(), lpLoserCharacter->GetCharacterName(),
						lpLoserCharacter->GetNation(), (unsigned char)lpLoserCharacter->GetClass(),
						cCmdPerMember, PktBase::NO_SERVER_ERR);
				}
			}
		}
	}
}



bool CCharacterParty::StartQuest(Quest::QuestNode* lpQuestNode, const Position& CenterPos, unsigned long dwNPCID)
{
	if (!lpQuestNode) return false;

	unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();
	unsigned short wError;

	for (int nMemberIndex=0; nMemberIndex<m_Party.m_cMemberNum; ++nMemberIndex)
	{
		wError = PktStartQuest::NO_SERVER_ERR;
		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if (NULL != lpCharacter)
		{
			if (m_Party.ServerID[nMemberIndex] == dwServerID)
			{
				if (lpCharacter->GetCurrentPos().GetDistance(CenterPos) > Quest::PARTY_QUEST_DIST)
				{
					wError = PktStartQuest::FAIL_TOO_FAR_PARTY;
				}
				else
				{
					if (false == lpCharacter->CheckQuest(lpQuestNode, wError))
					{
						continue;
					}
					else
					{
						if (false == lpCharacter->GiveQuest(lpQuestNode))
						{
							wError = PktStartQuest::FAIL_FULL_QUEST;
						}
					}
				}

				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharStartQuest(lpDispatch->GetSendStream(),
															 lpCharacter->GetCID(), dwNPCID, 
                                                             lpQuestNode->m_wQuestID, wError);

					if (PktStartQuest::NO_SERVER_ERR == wError)
					{
						lpCharacter->StartPhase(lpQuestNode->m_wQuestID, 1);
					}
				}
			}
		}
	}

	return true;
}

bool CCharacterParty::CheckTrigger(unsigned long dwExceptCID, unsigned char cTriggerKind, unsigned long dwReferenceID, Position Pos, short wCount)
{
	unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		// �̹� �ڽ��� Ʈ���Ŵ� �ߵ��� �����̹Ƿ�, �Ѿ��.
		if (dwExceptCID == m_Party.MemberCID[nMemberIndex])
		{
			continue;
		}

		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if (NULL == lpCharacter || m_Party.ServerID[nMemberIndex] != dwServerID)
		{
			continue;
		}

		float fDX = fabs( lpCharacter->GetCurrentPos().m_fPointX - Pos.m_fPointX );
		float fDZ = fabs( lpCharacter->GetCurrentPos().m_fPointZ - Pos.m_fPointZ );
		float fDist = sqrtf( fDX * fDX + fDZ * fDZ );

		if (fDist > Quest::PARTY_QUEST_DIST)
		{
			continue;
		}

		for (int nQuestIndex = 0; nQuestIndex < Quest::MAX_EXECUTING_QUEST; ++nQuestIndex)
		{
			Quest::ExecutingQuest* lpExecutingQuest = lpCharacter->GetExecutingQuest();

			if (NULL != lpExecutingQuest[nQuestIndex].m_QuestNode && lpExecutingQuest[nQuestIndex].m_QuestNode->IsPartyQuest())
			{
				PhaseNode* phaseNode = lpExecutingQuest[nQuestIndex].m_QuestNode->m_lstPhase[lpExecutingQuest[nQuestIndex].m_cPhase - 1];
				for (size_t nTriggerIndex = 0; nTriggerIndex < phaseNode->m_lstTrigger.size(); nTriggerIndex++)
				{
					if (cTriggerKind == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerKind)
					{
						bool bOperateTrigger = false;

						switch (cTriggerKind)
						{
						case TRIGGER_START:
							break;

						case TRIGGER_PUTON:
							if (dwReferenceID == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
							{ 
								Position TriggerPos(phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosX, 
									phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosY,
									phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosZ);

								if (Pos.GetDistance(TriggerPos) <= phaseNode->m_lstTrigger[nTriggerIndex]->m_fDistance)
								{
									bOperateTrigger = true; 
								}
							}
							break;

						case TRIGGER_GETON:
						case TRIGGER_TALK:
							break;

						case TRIGGER_KILL:
						case TRIGGER_PICK:	
							if (dwReferenceID == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
							{ 
								bOperateTrigger = true; 
							}
							break;

						case TRIGGER_FAME:
							if (lpCharacter->GetFame() >= phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
							{ 
								bOperateTrigger = true; 
							}
							break;

						case TRIGGER_LEVEL_TALK:
							if (lpCharacter->GetLevel() >= phaseNode->m_lstTrigger[nTriggerIndex]->m_dwLevel)
							{
								bOperateTrigger = true;
							}
							break;

						case TRIGGER_FAME_TALK:
							if (lpCharacter->GetFame() >= phaseNode->m_lstTrigger[nTriggerIndex]->m_dwLevel)
							{
								bOperateTrigger = true;
							}
							break;

						case TRIGGER_ABILITY_TALK:
/*
							if (lpCharacter->GetFame() >= phaseNode->m_lstTrigger[nTriggerIndex]->m_dwLevel)
							{
								bOperateTrigger = true;
							}
*/
							break;

						}

						if (true == bOperateTrigger)
						{
							lpExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex] -= wCount;
							lpCharacter->OperateTrigger(lpExecutingQuest[nQuestIndex].m_QuestNode->m_wQuestID, 
								lpExecutingQuest[nQuestIndex].m_cPhase, static_cast<unsigned char>(nTriggerIndex), 
								lpExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex], Pos);
							bOperateTrigger = false;
						}
					}
				}
			}
		}
	}

	return true;
/*
	unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

	for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
	{
		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if (NULL == lpCharacter || m_Party.ServerID[nMemberIndex] != dwServerID)
		{
			continue;
		}

		float fDX = fabs( lpCharacter->GetCurrentPos().m_fPointX - Pos.m_fPointX );
		float fDZ = fabs( lpCharacter->GetCurrentPos().m_fPointZ - Pos.m_fPointZ );
		float fDist = sqrtf( fDX * fDX + fDZ * fDZ );

		if (fDist > Quest::PARTY_QUEST_DIST)
		{
			continue;
		}

		for (int nQuestIndex = 0; nQuestIndex < Quest::MAX_EXECUTING_QUEST; ++nQuestIndex)
		{
			Quest::ExecutingQuest* lpExecutingQuest = lpCharacter->GetExecutingQuest();

			if (NULL != lpExecutingQuest[nQuestIndex].m_QuestNode && 
				wQuestID == lpExecutingQuest[nQuestIndex].m_QuestNode->m_wQuestID)
			{
				PhaseNode* phaseNode = lpExecutingQuest[nQuestIndex].m_QuestNode->m_lstPhase[lpExecutingQuest[nQuestIndex].m_cPhase - 1];
				for (size_t nTriggerIndex = 0; nTriggerIndex < phaseNode->m_lstTrigger.size(); nTriggerIndex++)
				{
					if (cTriggerKind == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerKind)
					{
						bool bOperateTrigger = false;

						switch (cTriggerKind)
						{
							case TRIGGER_START:
								break;

							case TRIGGER_PUTON:
								if (dwReferenceID == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
								{ 
									Position TriggerPos(phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosX, 
														phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosY,
														phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosZ);

									if (Pos.GetDistance(TriggerPos) <= phaseNode->m_lstTrigger[nTriggerIndex]->m_fDistance)
									{
										bOperateTrigger = true; 
									}
								}
								break;

							case TRIGGER_GETON:
							case TRIGGER_TALK:
								break;

							case TRIGGER_KILL:
							case TRIGGER_PICK:	
								if (dwReferenceID == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
								{ 
									bOperateTrigger = true; 
								}
								break;

							case TRIGGER_FAME:
								if (dwReferenceID >= phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
								{ 
									bOperateTrigger = true; 
								}
								break;

							case TRIGGER_LEVEL_TALK:
								break;

							case TRIGGER_FAME_TALK:
								break;

							case TRIGGER_ABILITY_TALK:
								break;
						}

						if (true == bOperateTrigger)
						{
							lpExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex] -= wCount;
							lpCharacter->OperateTrigger(lpExecutingQuest[nQuestIndex].m_QuestNode->m_wQuestID, 
								lpExecutingQuest[nQuestIndex].m_cPhase, static_cast<unsigned char>(nTriggerIndex), 
								lpExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex], Pos);
							bOperateTrigger = false;
						}
					}
				}
			}
		}
	}

	return true;
*/
}

void CCharacterParty::OperateTrigger(unsigned short wQuestID, unsigned char cPhase, unsigned char cTrigger, unsigned char cCount, Position Pos)
{
	unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

	for (int nMemberIndex=0; nMemberIndex<m_Party.m_cMemberNum; ++nMemberIndex)
	{
		CCharacter* lpCharacter = m_pMemberPointer[nMemberIndex];

		if (NULL != lpCharacter)
		{
			if (m_Party.ServerID[nMemberIndex] == dwServerID)
			{
				if (lpCharacter->GetCurrentPos().GetDistance(Pos) > Quest::PARTY_QUEST_DIST)
				{
					continue;
				}
				else
				{
					unsigned short wError = lpCharacter->OperateTrigger(wQuestID, cPhase, cTrigger, cCount, lpCharacter->GetCurrentPos());
					if (PktOperateTrigger::NO_SERVER_ERR != wError)
					{
						ERRLOG3(g_Log, "��Ƽ�α� : CID:0x%08x Ʈ���� �ߵ��� ������ �߻��Ͽ����ϴ�. ����Ʈ:%d, �����ڵ�:%d", lpCharacter->GetCID(), wQuestID, wError);
					}
				}
			}
		}
	}
}

void CCharacterParty::UpdateData()
{
    unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

    const unsigned short MAX_PACKET_SIZE = (sizeof(PartyMemberData) + 
        sizeof(unsigned long)) * PARTY::MAX_MEM + sizeof(PktBase);

    char szPacket[MAX_PACKET_SIZE];
    char* szPacketPos = szPacket + sizeof(PktBase);
    
    for (int nMemberIndex = 0; nMemberIndex < m_Party.m_cMemberNum; ++nMemberIndex)
    {
        if (m_Party.ServerID[nMemberIndex] == dwServerID && 0 != m_Party.MemberCID[nMemberIndex])
        {
            CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_Party.MemberCID[nMemberIndex]);
            if (NULL != lpCharacter)
            {
                // �����͸� �����ϰ�, ������ ���� Flag�� Set�Ѵ�.
                SetUpdateFlag(m_partyMemberData[nMemberIndex], *lpCharacter, szPacketPos, false);
            }
            else
            {
                // �����δ� ĳ���Ͱ� �������� �ʴ´�.
                m_pMemberPointer[nMemberIndex] = NULL;
            }
        }
    }

    unsigned short usPacketSize = static_cast<unsigned short>(szPacketPos - szPacket);

    if (sizeof(PktBase) < usPacketSize && 
		PacketWrap::WrapCrypt(szPacket, usPacketSize, CmdCharPartyMemInfo, 0, PktBase::NO_SERVER_ERR))
    {        
        SendAllLoggedMember(szPacket, usPacketSize, 0, CmdCharPartyMemInfo);
    }    
}

inline void COPY_AND_ADVANCE(char*& szDst, const void* szSrc, size_t nSize)
{
    memcpy(szDst, szSrc, nSize);
    szDst += nSize;
}

void CCharacterParty::SetUpdateFlag(PartyMemberData& partyMemberData, CCharacter& character, char*& szPacket, bool bUpdateForce)
{
    CreatureStatus& 	creatureStatus  = character.GetStatus();
    const Position& 	pos             = character.GetCurrentPos();
    unsigned char   	cClass          = static_cast<unsigned char>(character.GetClass());
	const EnchantInfo&	enchantInfo		= character.GetEnchantInfo();
    
    partyMemberData.m_usDataChanged = 0;

    // �ϴ� CID�� StatusFlag�ڸ��� Ȯ���س��´�.
    char* szCIDPos = szPacket;
    char* szDataChanged = szCIDPos + sizeof(unsigned long);
    char* szPacketPos = szDataChanged + sizeof(unsigned short);

    partyMemberData.m_dwCID = character.GetCID();

	// ------------------------------------------------------------------------------------------------------------------
	// ���� ������ �ٲ�� ġ�����Դϴ�.

    // X��ġ
    if (true == bUpdateForce || partyMemberData.m_fXPos != pos.m_fPointX) 
    {
        partyMemberData.m_fXPos = pos.m_fPointX;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_fXPos, sizeof(float));        
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_XPOS;
    }

    // Y��ġ
    if (true == bUpdateForce || partyMemberData.m_fYPos != pos.m_fPointY)
    {
        partyMemberData.m_fYPos = pos.m_fPointY;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_fYPos, sizeof(float));
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_YPOS;
    }

    // Z��ġ
    if (true == bUpdateForce || partyMemberData.m_fZPos != pos.m_fPointZ)
    {
        partyMemberData.m_fZPos = pos.m_fPointZ;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_fZPos, sizeof(float));
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_ZPOS;
    }

    // �ִ� HP
    if (true == bUpdateForce || partyMemberData.m_usMaxHP != creatureStatus.m_StatusInfo.m_nMaxHP)
    {
        partyMemberData.m_usMaxHP = creatureStatus.m_StatusInfo.m_nMaxHP;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_usMaxHP, sizeof(unsigned short));
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_MAX_HP;
    }

    // �ִ� MP
    if (true == bUpdateForce || partyMemberData.m_usMaxMP != creatureStatus.m_StatusInfo.m_nMaxMP)
    {
        partyMemberData.m_usMaxMP = creatureStatus.m_StatusInfo.m_nMaxMP;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_usMaxMP, sizeof(unsigned short));
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_MAX_MP;
    }

    // ���� HP
    if (true == bUpdateForce || partyMemberData.m_usCurHP != creatureStatus.m_nNowHP)
    {
        partyMemberData.m_usCurHP = creatureStatus.m_nNowHP;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_usCurHP, sizeof(unsigned short));
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_CUR_HP;
    }

    // ���� MP
    if (true == bUpdateForce || partyMemberData.m_usCurMP != creatureStatus.m_nNowMP)
    {
        partyMemberData.m_usCurMP = creatureStatus.m_nNowMP;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_usCurMP, sizeof(unsigned short));
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_CUR_MP;
    }

    // ĳ���� ����
    if (true == bUpdateForce || partyMemberData.m_cLevel != creatureStatus.m_nLevel)
    {
        partyMemberData.m_cLevel = creatureStatus.m_nLevel;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_cLevel, sizeof(unsigned char));
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_LEVEL;
    }

    // ĳ���� Ŭ����
    if (true == bUpdateForce || partyMemberData.m_cClass != cClass)
    {
        partyMemberData.m_cClass = cClass;
        COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_cClass, sizeof(unsigned char));
        partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_CLASS;
    }

	// ��æƮ ����
	for (int nIndex = 0; nIndex < EnchantInfo::MAX_ARRAY && nIndex < PartyMemberData::MAX_ENCHANT_INFO_NUM; ++nIndex)
	{
		if (true == bUpdateForce || partyMemberData.m_dwEnchantInfo[nIndex] != enchantInfo.m_dwStatusFlag[nIndex])
		{
			partyMemberData.m_dwEnchantInfo[nIndex] = enchantInfo.m_dwStatusFlag[nIndex];
			COPY_AND_ADVANCE(szPacketPos, &partyMemberData.m_dwEnchantInfo[nIndex], sizeof(unsigned long));

			switch (nIndex)
			{
				case 0:		partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_ENCHANT_INFO_0;		break;
				case 1:		partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_ENCHANT_INFO_1;		break;
				case 2:		partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_ENCHANT_INFO_2;		break;
				case 3:		partyMemberData.m_usDataChanged |= PartyMemberData::CHANGED_ENCHANT_INFO_3;		break;
			}		
		}
	}
	// ------------------------------------------------------------------------------------------------------------------

	// �ٲ� �����Ͱ� �ִٸ�, Ȯ���س��� �ڸ��� �����͸� ����Ѵ�. �ƴϸ� Ȯ���س��� �ڸ��� ���� ���´�.
    if (0 != partyMemberData.m_usDataChanged)
    {        
        memcpy(szCIDPos, &partyMemberData.m_dwCID, sizeof(unsigned long));
        memcpy(szDataChanged, &partyMemberData.m_usDataChanged, sizeof(unsigned short));

        szPacket = szPacketPos;
    }
}