#include "stdafx.h"

#include <boost/pool/pool_alloc.hpp>

#include <Network/Session/Session.h>
#include <Network/XORCrypt/XORCrypt.h>

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>

#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Creature/Character/Character.h>
#include <GameGuardLib/ggerror.h>

#include <mmsystem.h>

#include "PacketParse.h"
#include "GameClientDispatch.h"


// �Լ� ����
bool LogFailDispatch(CGameClientDispatch& GameClientDispatch, 
                     const char* szDetailText, const unsigned char cCmd);


CGameClientDispatchTable& CGameClientDispatchTable::GetInstance()
{
    static CGameClientDispatchTable gameClientDispatchTable;
    return gameClientDispatchTable;
}

CGameClientDispatchTable::CGameClientDispatchTable()
:   CPacketDispatchTable<CGameClientDispatch::FnProcess>(UCHAR_MAX)
{
    using namespace GameClientParsePacket;

    // �⺻���� Process���� �� ��. ( character����... )
    
    // ĳ���� ������ ���� - ParseCharItem.cpp
    AddDispatch(CmdCharTakeItem,            ParseCharTakeItem);     			// ������ �̵� �� ����
    AddDispatch(CmdCharSwapItem,         	ParseCharSwapItem);					// ������ ����
    AddDispatch(CmdCharRepairItem,       	ParseCharRepairItem);				// ������ ����
    AddDispatch(CmdCharRepairAllItem,		ParseCharRepairAllItem);			// ������ ���� (����ϰ� �ִ� ������ ���)
    AddDispatch(CmdCharUseItem,          	ParseCharUseItem);					// ������ ���
    AddDispatch(CmdCharTradeItem,        	ParseCharTradeItem);				// ������ �ŷ�
    AddDispatch(CmdCharEquipShopInfo,       ParseCharEquipShopInfo);			// NPC ��� ���� ����
    AddDispatch(CmdCharPickUp,           	ParseCharPickUp);					// ������ ����
    AddDispatch(CmdCharPullDown,         	ParseCharPullDown);					// ������ ������
    AddDispatch(CmdCharSplitItem,        	ParseCharSplitItem);				// ������ ������
    AddDispatch(CmdCharTakeGold,         	ParseCharTakeGold);					// �� �ٷ��
    AddDispatch(CmdDeposit,              	ParseCharDepositCmd);   			// â�� ���� ��Ŷ��
    AddDispatch(CmdCharAutoRouting,			ParseCharAutoRouting);				// ���� ����
    AddDispatch(CmdCharUseCashItem,			ParseCharUseCashItem);				// ĳ��������
	
    // ĳ���� ������ ���׷��̵� ���� - ParseCharUpgradeItem.cpp
    AddDispatch(CmdCharInstallSocket,    	ParseCharInstallSocket);			// ������ �ν��� ����
    AddDispatch(CmdCharItemChemical,		ParseCharItemChemical);				// ������ �ռ�
    AddDispatch(CmdCharUpgradeItem,      	ParseCharUpgradeItem);				// ������ ���׷��̵�
    AddDispatch(CmdCharItemOptionGraft,     ParseCharItemOptionGraft);			// ������ �ɼ� �̽�
	AddDispatch(CmdCharItemCompensation,	ParseCharItemCompensation);			// ������ ���� �Ǹ� 

    // ĳ���� ��ų ���� - ParseCharSkill.cpp
    AddDispatch(CmdCharUseSkill,         	ParseCharUseSkill);					// ��ų ���
//  AddDispatch(CmdCharSkillLock,       	ParseCharSkillLock);				// ��ų ��
//  AddDispatch(CmdCharSkillUnLock,     	ParseCharSkillUnLock);				// ��ų �� ����
    AddDispatch(CmdCharSkillErase,       	ParseCharSkillErase);				// ��ų �����
    
    // ĳ���� ���� ���� ��Ŷ - ParseCharAttack.cpp
    AddDispatch(CmdCharAttack,          	ParseCharAttack);					// ���� ��Ŷ
    AddDispatch(CmdCharSwitchEQ,      		ParseCharSwitchEQ);					// ��� �ٲٱ�
    AddDispatch(CmdCharRespawn,         	ParseCharRespawn);					// ĳ���� ������
    AddDispatch(CmdCharRespawnWaitQueue, 	ParseCharRespawnWaitQueue);			// ������ ť ����� ���� (��Ʋ�׶����)
    AddDispatch(CmdCharRespawnInfo,			ParseCharRespawnInfo);				// ������ ���� (������ ��ġ��)
    AddDispatch(CmdCharRespawnAreaInfo,		ParseCharRespawnAreaInfo);			// ������ ���� ���� ����
//  AddDispatch(CmdCharMoveUpdate,      	ParseCharMoveUpdate);				// ĳ���� ������ ������Ʈ
    AddDispatch(CmdCharDuelCmd,         	ParseCharDuelCmd);					// ��� ��Ŷ ���	
    AddDispatch(CmdCharPeaceMode,			ParseCharPeaceMode);				// ���� ���
    AddDispatch(CmdCharSummonCmd,			ParseCharSummonCmd);				// ��ȯ�� ���

    // ������ UDP p2p��Ŷ������ ������ �ø�
    AddDispatch(CmdCharMove,                ParseCharMoveEx);

    // ĳ���� ������ ���� ��Ŷ - ParseCharLevelUp.cpp
    AddDispatch(CmdCharClassUpgrade,     	ParseCharClassUpgrade);				// Ŭ���� ���׷��̵�
    AddDispatch(CmdCharIncreasePoint,    	ParseCharIncreasePoint);			// IP ���� ��Ŷ
    AddDispatch(CmdCharStateRedistribution, ParseCharStateRedistribution);		// IP ���� ��Ŷ
    AddDispatch(CmdCharStatusRetrain,		ParseCharStatusRetrain);			// ĳ���� �������ͽ� ���Ʒ�

    // ĳ���� Ŀ�´�Ƽ ���� ��Ŷ - ParseCharCommunity.cpp
    AddDispatch(CmdCharExchangeCmd,      	ParseCharExchangeCmd);				// ��ȯ ��Ŷ ���	
    AddDispatch(CmdCharPartyCmd,         	ParseCharPartyCmd);					// ��Ƽ ��Ŷ ���
    AddDispatch(CmdCharPartyFind,        	ParseCharPartyFind);				// ��Ƽ ã��

    AddDispatch(CmdCharStallOpen,			ParseCharStallOpen);				// ĳƽ�� ������ ����
    AddDispatch(CmdCharStallRegisterItem,	ParseCharStallRegisterItem);		// ĳƽ�� ������ ������ ���
    AddDispatch(CmdCharStallEnter,			ParseCharStallEnter);				// ĳƽ�� ������ ����

    AddDispatch(CmdFriendAddRequest,        ParseCharFriendAdd);    			// ģ�� �߰�
    AddDispatch(CmdFriendRemoveRequest,     ParseCharFriendRemove); 			// ģ�� ����
    AddDispatch(CmdFriendEtcRequest,        ParseCharFriendEtc);    			// ��Ÿ

    AddDispatch(CmdCreateGuild,				ParseCharCreateGuild);    			// ��� ����
    AddDispatch(CmdGuildCmd,				ParseCharGuildCmd);    				// ��� ��� ���� ���
    AddDispatch(CmdGuildMark,				ParseCharGuildMark);    			// ��� ��ũ ����
    AddDispatch(CmdGuildLevel,				ParseCharGuildLevel);    			// ��� ���� ����
    AddDispatch(CmdGuildRelation,			ParseCharGuildRelation);    		// ��� ���� ����
    AddDispatch(CmdGuildInclination,		ParseCharGuildInclination);			// ��� ���� ����
    AddDispatch(CmdGuildList,				ParseCharGuildList);    			// ��� ����Ʈ
    AddDispatch(CmdGuildRight,				ParseCharGuildRight);    			// ��� ���� ����
    AddDispatch(CmdGuildMemberList,			ParseCharGuildMemberList);			// ��� ��� ����Ʈ
    AddDispatch(CmdGuildSafe,				ParseCharGuildSafe);				// ��� �ݰ�
	AddDispatch(CmdGuildHostilityList,		ParseCharGuildHostilityList);		// ���� ��� ��Ȳ ����Ʈ
	AddDispatch(CmdGuildRelationInfo,		ParseCharGuildRelationInfo);		// ����� ���� ����Ʈ ��û

    // ���� ���� ��Ŷ - ParseCharAdmin.cpp
    AddDispatch(CmdCharAdminCmd,			ParseCharAdminCmd);					// ���� Ŀ�ǵ�
    AddDispatch(CmdCharNameChange,          ParseCharNameChange);               // ĳ���� �̸� ����

    // ĳ���� ����Ʈ ���� ��Ŷ - ParseCharQuest.cpp
    AddDispatch(CmdCharStartQuest,			ParseCharStartQuest);				// ����Ʈ ����
    AddDispatch(CmdCharOperateTrigger,		ParseCharOperateTrigger);			// Ʈ���� �ߵ�
    AddDispatch(CmdCharCancelQuest,			ParseCharCancelQuest);				// ����Ʈ ���
    
    // ĳ���� ��Ÿ ��Ŷ - ParseCharEtc.cpp
    AddDispatch(CmdCharSuicide,          	ParseCharSuicide);					// ĳ���� �ڻ�	
    AddDispatch(CmdCharBindPosition,     	ParseCharBindPosition);				// ĳ���� ���ε� ������
    AddDispatch(CmdCharQuickSlotMove,    	ParseCharQuickSlotMove);			// ĳ���� �� ���� �̵�
    AddDispatch(CmdCharControlOption,    	ParseCharControlOption);			// ĳ���� �ɼ� ����
    AddDispatch(CmdCharAuthorizePanel,		ParseCharAuthorizePanel);
    AddDispatch(CmdCharFameInfo,         	ParseCharFameInfo);					// ĳ���� ��(���嵵) ���� ��û
	AddDispatch(CmdNameCheck,			  	ParseCharNameCheck);				//hz added for new packet
    AddDispatch(CmdRankingInfo,			 	ParseCharRankingInfo);				// ��ŷ ����

    // ��Ʋ �׶��� ������ ��Ŷ - ParseCharBGServer.cpp
    AddDispatch(CmdBGServerMapList,			ParseCharBGServerMapList);			// ��Ʋ �׶��� ������ ��(��) ���� ����Ʈ
    AddDispatch(CmdBGServerResultList,		ParseCharBGServerResultList);		// ��Ʋ �׶��� ������ ��(��) ��� ����Ʈ
    AddDispatch(CmdBGServerMoveZone,		ParseCharBGServerMoveZone);			// ��Ʋ �׶��� ������ �� �̵� (��(��) �̵�)
    AddDispatch(CmdBGServerMileageChange,	ParseCharBGServerMileageChange);	// ��Ʋ �׶��� ������ ȯ���� ���
    AddDispatch(CmdBGServerCharSlot,		ParseCharBGServerCharSlot);			// ��Ʋ �׶��� ���� ���� ĳ���� ���� ��û

    // ���� ��Ŷ - ParseCharCastle.cpp
    AddDispatch(CmdCastleCmd,				ParseCharCastleCmd);				// �� ���� ������Ʈ ���
    AddDispatch(CmdCampCmd,					ParseCharCampCmd);					// ��� ��� ���
    AddDispatch(CmdSiegeArmsCmd,			ParseCharSiegeArmsCmd);				// ���� ���� ���
    AddDispatch(CmdCastleRight,				ParseCharCastleRight);				// �� ���� ����
    AddDispatch(CmdCampRight,				ParseCharCampRight);				// ��� ��� ���� ����

    // ä�� ��Ŷ - ParseCharChat.cpp 
    AddDispatch(CmdCharWhisper,          	ParseCharWhisper);					// �ӻ���
    AddDispatch(CmdCharChat,             	ParseCharChat);						// ĳƽ�� ä��
    
    AddDispatch(CmdCharCastObjectInfo,      ParseCastObjectInfo);               // ������Ʈ ������ ��Ŷ ����

	AddDispatch(CmdCharInstallRuneSocket,	ParseCharInstallRuneSocket);		// �鼳ġ, ���� ���� ����.

	AddDispatch(CmdTakeMaterial,			ParseCharTakeMaterial);				// ��� ��� ���� �ֱ�/����
	AddDispatch(CmdTakeCastleJewel,			ParseCharTakeCastleJewel);			// �� ��¡�� ���� ������ �ֱ�/����
	AddDispatch(CmdWarOnOff,				ParseWarOnOff);						// �����, ������ �÷��� �ٲٱ�

	AddDispatch(CmdKeyInfo,					ParseKeyInfo);						// ���̽�ƽ Ű ���� ����.
}



CGameClientDispatch::CGameClientDispatch(CSession& Session,
										 CGameClientDispatchTable& GameClientDispatchTable)
:   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
    m_lpCharacter(0), 
	m_dwUID(0), 
	m_GameClientDispatchTable(GameClientDispatchTable),
    m_dwMoveServerID(0)
{    
	// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
//	m_CSAuth.Init();
	m_CSAuth2.Init();

	DETLOG2(g_Log, "DP:0x%p/UID:%u/GameClientDispatch Created", this, m_dwUID);
}


CGameClientDispatch::~CGameClientDispatch()
{
    CGameClientDispatch::Disconnected();

    // ��ü �Ҹ� �α�
    DETLOG2(g_Log, "DP:0x%p/UID:%u/GameClientDispatch Destroy", this, m_dwUID);
}

void CGameClientDispatch::Connected()
{
}

void CGameClientDispatch::Disconnected()
{
    if (0 != m_lpCharacter)
    {
        m_lpCharacter->SetDispatcher(0);
    }    
}

void CGameClientDispatch::Disconnect()
{
    CloseSession();
}

bool CGameClientDispatch::DispatchPacket(PktBase* lpPktBase)
{
    unsigned char   cCmd        = lpPktBase->GetCmd();
    bool            bHackCheck  = true;

    // ���������� ���� ��Ŷ �ð� ������Ʈ.
    m_CheckPing.SetLastPingRecvTime(CPulse::GetInstance().GetLastTick());

    switch (cCmd)
    {
		case CmdSysPing:			
            bHackCheck = m_CheckSpeedHack.CheckTimeUpdate(static_cast<PktSyP*>(lpPktBase)->m_dwTickTime);
			break;//return true;
	    
		case CmdCharAttack:     
			bHackCheck = m_CheckSpeedHack.CheckAttackReplay(static_cast<PktAt*>(lpPktBase));
			break;
    }

    if (!bHackCheck)
    {
        ERRLOG3(g_Log, "DP:0x%p/IP:%s/UID:%10d/���ǵ��� üũ�� �ɷȽ��ϴ�. ������ �����ϴ�.", 
            this, GetRemoteAddr().get_addr_string(), m_dwUID);
    }
	else if (CXORCrypt::PAGE_VERSION != CXORCrypt::GetInstance().GetPageVer(lpPktBase->GetCodePage()))
	{
        // (Ŭ���̾�Ʈ -> ���� ����)�� ����� �� ���� �ڵ� ��������(����� 1��)�� ����մϴ�.
        ERRLOG3(g_Log, "DP:0x%p/IP:%s/UID:%d �߸��� �ڵ� �������� ��Ŷ�Դϴ�. ������ �����ϴ�.", 
            this, GetRemoteAddr().get_addr_string(), m_dwUID);
	}
    else 
    {
        CGameClientDispatch::FnProcess fnProcess = 
            m_GameClientDispatchTable.GetDispatch(cCmd);

        const char* szErrorString = 0;

        if (0 == fnProcess)
        {
            szErrorString = "Unknown Gameserver Packet";
        }
	    else if (!fnProcess(*this, lpPktBase))
        {
            szErrorString = "GameServer packet process failed.";
        }

        if (0 != szErrorString)
        {
            LogFailDispatch(*this, szErrorString, cCmd);
        }

        return true;
    }

    return false;
}

bool CGameClientDispatch::Dispatch()
{
    unsigned long dwCurrentTime = timeGetTime();

	// �� üũ
	if (0 != m_lpCharacter &&                   // ĳ���Ͱ� �־�� �ϰ�
        0 == m_lpCharacter->GetAdminLevel() &&  // ��ڰ� �ƴϰ�
        !m_CheckPing.CheckPing(dwCurrentTime))  // Pingüũ�� �����ϸ�
	{
	    const int MAX_BUFFER = 256;
	    char szBuffer[MAX_BUFFER];

	    unsigned long dwPingCount           = 0;
	    unsigned long dwLastPingRecvTime    = 0;
	    unsigned long dwFirstCheckTime      = 0;

        unsigned long   dwCID = m_lpCharacter->GetCID();
	    const char*     szCharacterName = m_lpCharacter->GetCharacterName();
        
        m_CheckPing.GetPingData(dwPingCount, dwLastPingRecvTime, dwFirstCheckTime);

        _snprintf(szBuffer, MAX_BUFFER - 1, 
			"UID:%u/CID:%u/Name:%s/CurrentTime:%u/LastPingTime:%u/PingCount:%u/FirstCheckTime:%u/"
			"PingCheck failed. disconnect now.",
			m_dwUID, dwCID, szCharacterName, dwCurrentTime, 
			dwLastPingRecvTime, dwPingCount, dwFirstCheckTime);

		LogErrorPacket(szBuffer, 0);
		return false;
	}

	return CRylServerDispatch::Dispatch();
}

void CGameClientDispatch::ProcessTooManyPacket(CBufferQueue& bufferQueue)
{
    if (0 != m_lpCharacter)
    {
        typedef std::map<unsigned char, unsigned long, std::greater<unsigned long>, 
            boost::fast_pool_allocator< std::pair<unsigned char, unsigned long> > > PacketStatistics;

        const int           MAX_BUFFER = 512;
        char                szBuffer[MAX_BUFFER];

        PacketStatistics    packetStatistics;

        const SOCKADDR_IN&  sockAddr = GetRemoteAddr().get_addr_in();

        int nLength = _snprintf(szBuffer, MAX_BUFFER, 
            "/UID:%d/CID:0x%08x/Name:%s/IP:%15s/ProcessPkt:%4d/RemainPkt:%4d/",
            m_lpCharacter->GetUID(), m_lpCharacter->GetCID(), m_lpCharacter->GetCharacterName(),        
            inet_ntoa(sockAddr.sin_addr), m_dwMaxProcessPacketPerPulse, bufferQueue.getBufferNum());
        
        CBufferQueue tempQueue;
        tempQueue.splice(bufferQueue);
    
        const int DROP_PACKET_LIMIT     = 20;
        const int DROP_PACKET_MAX_LIMIT = 40;

        bool bDropPacket = DROP_PACKET_MAX_LIMIT < tempQueue.getBufferNum();

        // ��Ŷ ��踦 ���Ѵ�.
        CBuffer* lpBuffer = tempQueue.getHead();
        for(; 0 != lpBuffer; lpBuffer = lpBuffer->next())
        {
            unsigned char cCmd = reinterpret_cast<PktBase*>(lpBuffer->rd_ptr())->GetCmd();               
            ++packetStatistics[cCmd];
        }

        const unsigned long dwMaxPrint  = 5;
        unsigned long       dwPrinted   = 0;

        PktBase::CMDType aryDropCmd[dwMaxPrint];
        PktBase::CMDType* lpDropCmdEnd = aryDropCmd;

        std::pair<unsigned char, unsigned long> resultPair;

        for(PacketStatistics::iterator itr = packetStatistics.begin();
            itr != packetStatistics.end() && dwPrinted < dwMaxPrint; ++itr, ++dwPrinted)
        {
            resultPair = *itr;

            nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, "0x%02x:%5d/", resultPair.first, resultPair.second);

            // ��Ŷ�� �ѹ��� �� �̻� �� ���� Drop�� ����Ʈ�� ���Խ�Ų��.
            // ���⼭�� Top 5�� ���ϴ� ��Ŷ �� DROP_PACKET_LIMIT �̻� �Ǵ� ��Ŷ�� ������.
            if(DROP_PACKET_LIMIT < resultPair.second)
            {
                *lpDropCmdEnd = resultPair.first;
                ++lpDropCmdEnd;
            }
        }

        std::sort(aryDropCmd, lpDropCmdEnd);

        ERRLOG0(g_Log, szBuffer);

        for(;;)
        {
            lpBuffer = tempQueue.dequeue();
            if(0 == lpBuffer)
            {
                break;
            }
            else 
            {
                unsigned char cCmd = reinterpret_cast<PktBase*>(lpBuffer->rd_ptr())->GetCmd();               

                // ������ �� ��Ŷ�̸� ������.
                if(bDropPacket && std::binary_search(aryDropCmd, lpDropCmdEnd, cCmd))
                {                    
                    SAFE_RELEASE_BUFFER(lpBuffer);
                }
                else
                {
                    bufferQueue.enqueue(lpBuffer);
                }
            }
        }
    }
}

bool LogFailDispatch(CGameClientDispatch& GameClientDispatch, 
                    const char* szDetailText, const unsigned char cCmd)
{
    unsigned long dwUID = GameClientDispatch.GetUID();

    ERRLOG5(g_Log, "DP:0x%p/UID:%d/PacketCMD:0x%02x/IP:%15s/%s",
        &GameClientDispatch, dwUID, cCmd, 
        GameClientDispatch.GetRemoteAddr().get_addr_string(), szDetailText);

    return false;
}

unsigned long CGameClientDispatch::PopRequestKey() 
{
    unsigned long dwRequestKey = 0;
    if (!m_DBRequestQueue.empty())
    {
        dwRequestKey = m_DBRequestQueue.front();
        m_DBRequestQueue.pop_front();
    }

    unsigned long dwUID = 0;
    unsigned long dwCID = 0;
    
    if (0 != m_lpCharacter)
    {
        dwUID = m_lpCharacter->GetUID();
        dwCID = m_lpCharacter->GetCID();
    }

    DETLOG7(g_Log, "UID:%u/CID:0x%p(0x%p)/IP:%15s/DP:0x%p/DUID:%u/RequestKey:%d/ "
        "Pop RequestKey", dwUID, dwCID, m_lpCharacter, 
		GetRemoteAddr().get_addr_string(),        
        this, m_dwUID, dwRequestKey);

    return dwRequestKey;
}
    
void CGameClientDispatch::PushRequestKey(unsigned long dwRequestKey) 
{
    unsigned long dwUID = 0;
    unsigned long dwCID = 0;
    
    if (0 != m_lpCharacter)
    {
        dwUID = m_lpCharacter->GetUID();
        dwCID = m_lpCharacter->GetCID();
    }

    DETLOG7(g_Log, "UID:%u/CID:0x%p(0x%p)/IP:%15s/DP:0x%p/DUID:%u/RequestKey:%d/ "
        "Push RequestKey", dwUID, dwCID, m_lpCharacter, 
        GetRemoteAddr().get_addr_string(),
        this, m_dwUID, dwRequestKey);

    m_DBRequestQueue.push_back(dwRequestKey);
}

bool CGameClientDispatch::GetAuthQuery(GG_AUTH_DATA** lppAuthData_Out)
{
	unsigned long dwGGErrCode = m_CSAuth2.GetAuthQuery();
	if (dwGGErrCode != ERROR_SUCCESS)
	{
		PrintGameGuardError(dwGGErrCode);
		ERRLOG1(g_Log, "CID:0x%08x ���� �ڵ�(2) GetAuthQuery ����", m_lpCharacter->GetCID());
		return false;
	}

	*lppAuthData_Out = &m_CSAuth2.m_AuthQuery;
	return true;
}

bool CGameClientDispatch::CheckAuthAnswer(void)
{
	unsigned long dwGGErrCode = m_CSAuth2.CheckAuthAnswer();
	if (dwGGErrCode != ERROR_SUCCESS)
	{
		PrintGameGuardError(dwGGErrCode);
		ERRLOG1(g_Log, "CID:0x%08x ���� �ڵ�(2) CheckAuthAnswer ����", m_lpCharacter->GetCID());
		return false;
	}

	return true;
}

void CGameClientDispatch::PrintGameGuardError(void)
{
/*	switch (m_CSAuth.PPGetLastError())
	{
		case 1:
            ERRLOG1(g_Log, "CID:0x%08x Ű�� ��ġ���� �ʽ��ϴ�. "
				"��ȣȭ���� ���� ��Ŷ�̰ų�, UserKey�� �ٸ� Ŭ���̾�Ʈ�κ��� �� ��Ŷ�� �� �ֽ��ϴ�.", m_lpCharacter->GetCID());
			break;

		case 2:
			ERRLOG1(g_Log, "CID:0x%08x �������ѹ��� �ֱ��� ��Ŷ�� �����ϴ�. "
				"��Ŷ ���ö��� ������ ������ �� �ֽ��ϴ�.", m_lpCharacter->GetCID());
			break;

		case 3:
			ERRLOG1(g_Log, "CID:0x%08x �������ѹ��� �������Դϴ�. ������ Ŭ���̾�Ʈ�� ��ӵ� �������ѹ��� ��ġ���� �ʽ��ϴ�. "
				"��Ʈ�� ������ ��Ŷ �� ���� ���� �Ǿ��� �� �ֽ��ϴ�. TCP �������ݿ����� ���� �߻����� �ʴ� �����Դϴ�.", m_lpCharacter->GetCID());
			break;

		case 4:
			ERRLOG1(g_Log, "CID:0x%08x ��Ŷ CRC üũ�� �����߽��ϴ�. "
				"��Ŷ ������ ������ ���Ƿ� ��Ŷ�� �������� �� �ֽ��ϴ�.", m_lpCharacter->GetCID());
			break;
	}
*/
	// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
    ERRLOG1(g_Log, "CID:0x%08x ���ŵ� ���Ӱ��� ������ ����Ͽ����ϴ�.", m_lpCharacter->GetCID());
}

void CGameClientDispatch::PrintGameGuardError(unsigned long dwGGErrCode)
{
    const char* szErrorString = "�� �� ���� �����Դϴ�";

	switch (dwGGErrCode)
	{
		case ERROR_GGAUTH_FAIL_MEM_ALLOC:	
			szErrorString = "�޸� �Ҵ� ����";
			break;

		case ERROR_GGAUTH_FAIL_LOAD_DLL:	
			szErrorString = "ggauth.dll �ε� ����";
			break;

		case ERROR_GGAUTH_FAIL_GET_PROC:	
			szErrorString = "ggauth.dll�� Export �Լ� �������� ����";
			break;

		case ERROR_GGAUTH_FAIL_BEFORE_INIT:	
			szErrorString = "ggauth.dll�� �ʱ�ȭ�Ǳ� ���� Export �Լ� ȣ���Ͽ���";
			break;

		case ERROR_GGAUTH_INVALID_PARAM:
			szErrorString = "�Լ� ȣ�� �� invalid parameter ����";
			break;

		case ERROR_GGAUTH_NO_REPLY:
			szErrorString = "���� Query�� ���� Ŭ���̾�Ʈ�� ���� ����";
			break;

		case ERROR_GGAUTH_INVALID_PROTOCOL_VERSION:
			szErrorString = "Ŭ���̾�Ʈ�� ���� �������� ���� Ʋ��";
			break;

		case ERROR_GGAUTH_INVALID_REPLY:
			szErrorString = "���� Query�� ���� Ŭ���̾�Ʈ�� ���䰪�� Ʋ��";
			break;
	}

    DWORD dwCID = (0 != m_lpCharacter) ? m_lpCharacter->GetCID() : 0;

    const int MAX_LOG = 256;
    char szLog[MAX_LOG];

    if (0 < _snprintf(szLog, MAX_LOG - 1, "CID:0x%08x %s : %d / "
        "AQ.Index:0x%08x, AQ.V1:0x%08x, AQ.V2:0x%08x, AQ.V3:0x%08x, "
        "AA.Index:0x%08x, AA.V1:0x%08x, AA.V2:0x%08x, AA.V3:0x%08x",

        dwCID, szErrorString, dwGGErrCode,

        m_CSAuth2.m_AuthQuery.dwIndex, 
        m_CSAuth2.m_AuthQuery.dwValue1, 
        m_CSAuth2.m_AuthQuery.dwValue2, 
        m_CSAuth2.m_AuthQuery.dwValue3, 

        m_CSAuth2.m_AuthAnswer.dwIndex, 
        m_CSAuth2.m_AuthAnswer.dwValue1, 
        m_CSAuth2.m_AuthAnswer.dwValue2, 
        m_CSAuth2.m_AuthAnswer.dwValue3))
    {
        ERRLOG0(g_Log, szLog);
    }
}

void CGameClientDispatch::SetCharacter(CCharacter* lpCharacter)
{
    m_lpCharacter = lpCharacter;
    m_CheckSpeedHack.SetCharacter(lpCharacter);
}
