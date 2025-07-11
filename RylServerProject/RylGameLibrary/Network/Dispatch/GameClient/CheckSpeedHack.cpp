
#include "stdafx.h"

#include <Utility/Time/Pulse/Pulse.h>
#include <Creature/Character/Character.h>
#include <Skill/SkillMgr.h>

#include <Network/Packet/PacketStruct/CharMovePacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Utility/Setup/ServerSetup.h>

#include <mmsystem.h>

#include "CheckSpeedHack.h"

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

CSpeedHackCheck::CSpeedHackCheck()
:	m_dwServerTotal(0), m_dwServerLast(0), m_dwClientTotal(0),
    m_dwClinetLast(0), 

    m_dwLastServerAttackTime(0),
    m_dwLastClientAttackTime(0),
    m_dwHackFoundCount(0),

    m_lpCharacter(NULL)
{
    
}


bool CSpeedHackCheck::CheckTimeUpdate(DWORD dwClientTime)
{    
	if (CServerSetup::GetInstance().GetHackCheck() &&         
        CheckSpeedHack(timeGetTime(), dwClientTime))
    {
        return true;
    }

    return true;  
}


bool CSpeedHackCheck::CheckAttackReplay(PktAt* lpPktAt)
{
    if (NULL == m_lpCharacter)
    {
        return false;
    }

    // �ϴ��� �����ϴ��� �α׸� ��´�.
    CheckAttackHack(lpPktAt, timeGetTime());

    const unsigned long dwMaxHackFoundCount = 10;

    if (CServerSetup::GetInstance().GetHackCheck() && dwMaxHackFoundCount < m_dwHackFoundCount)
    {
        SOCKADDR_IN sockAddr;        

        CGameClientDispatch* lpDispatch = m_lpCharacter->GetDispatcher();

        if (0 != lpDispatch)
        {
            sockAddr = lpDispatch->GetRemoteAddr().get_addr_in();
        }
        else
        {
            memset(&sockAddr, 0, sizeof(SOCKADDR_IN));
        }
   
        ERRLOG5(g_Log, "/ UID:%10d / CID:%10d / Name:%s / IP:%15s / %dNum Hack found. /",
            m_lpCharacter->GetUID(), m_lpCharacter->GetCID(), m_lpCharacter->GetCharacterName(), 
            inet_ntoa(sockAddr.sin_addr), dwMaxHackFoundCount);

		m_dwHackFoundCount = 0;

		// DB�� ���� ����ƴٰ� �����. (���Ӽ����� DB�� ���� ����̾���.)
//		DBComponent::GameDB::InsertHackLog(CDBSingleObject::GetInstance(), m_lpCharacter->GetUID(), m_lpCharacter->GetCID(), m_lpCharacter->GetCharacterName());

        // ���� ���� �ʴ´�.
        return true;
		// edith 2008.05.14 ���ǵ��� �ɸ��� ����. (������ ���� ������ ���� ���ؼ� ����)
        // return false;
    }

    return true;
}

bool CSpeedHackCheck::CheckSpeedHack(unsigned long CurrentServer, unsigned long CurrentClient)
{
	const int HackCheckNum	= 2500;		// ���� ���� 2.5��
	const int HackCheckNum2	= -3000;	// ���� ���� 3��

	if (CPulse::GetInstance().GetTPPOverTwoTime() || 
        0 == m_dwServerLast || 0 == m_dwClinetLast)
	{
		// ó�� �α���, ���� ���Ϸ� ���� �Ұ�
		m_dwServerLast	= CurrentServer;
		m_dwClinetLast	= CurrentClient;
		return false;
	}

	int ServerInterval = CurrentServer - m_dwServerLast;
	int ClientInterval = CurrentClient - m_dwClinetLast;

	if (0 == ServerInterval)
	{
		// ��Ŷ ���Ϸ� üũ �Ұ�
		m_dwServerTotal	= 0;
		m_dwClientTotal	= 0;

		m_dwServerLast	= CurrentServer;
		m_dwClinetLast	= CurrentClient;

		return false;
	}

	// ���� Ÿ�� ���
	m_dwServerTotal += ServerInterval;
	m_dwClientTotal += ClientInterval;
	int SCTotal = m_dwClientTotal - m_dwServerTotal;

	// ���� �缺 üũ
	if (SCTotal > HackCheckNum)
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		// @ ���� �缺 ������ 
		// 1. 100% ���� �����
		//

        unsigned long dwCID = (NULL != m_lpCharacter) ? m_lpCharacter->GetCID() : 0;
    
		ERRLOG6(g_Log, "CID:%10u/Plus/S:%d/C:%d/ST:%d/CT:%d/CK:%d/SpeedHack found", 
            dwCID, ServerInterval, ClientInterval, m_dwServerTotal, m_dwClientTotal, SCTotal);

		m_dwServerTotal	= 0;
		m_dwClientTotal	= 0;

		m_dwServerLast	= CurrentServer;
		m_dwClinetLast	= CurrentClient;

		return true;
	}

	// ���� ���� üũ
	if (SCTotal < HackCheckNum2)
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		// @ ���� ���� ������ 
		// 1. �α� �� ��� �ٽ� �ʱ�ȭ
		//

        unsigned long dwCID = (NULL != m_lpCharacter) ? m_lpCharacter->GetCID() : 0;

		ERRLOG6(g_Log, "CID:%10u/Minus/S:%d/C:%d/ST:%d/CT:%d/CK:%d/SpeedHack found", 
            dwCID, ServerInterval, ClientInterval, m_dwServerTotal, m_dwClientTotal, SCTotal);

		m_dwServerTotal	= 0;
		m_dwClientTotal	= 0;

		m_dwServerLast	= 0;
		m_dwClinetLast	= 0;

		return false;
	}

	// ���� Ÿ�� ũ�� üũ
	if (m_dwServerTotal > 0xFFFFFFF0 || m_dwClientTotal > 0xFFFFFFF0)
	{
		m_dwServerTotal	= 0;
		m_dwClientTotal	= 0;

		m_dwServerLast	= 0;
		m_dwClinetLast	= 0;
	}

	m_dwServerLast = CurrentServer;
	m_dwClinetLast = CurrentClient;
	return false;
}


bool CSpeedHackCheck::CheckAttackHack(PktAt* lpPktAt, unsigned long dwCurrentServerTime)
{
    static const unsigned long  dwAverageAttackTime              = 500;
    static const unsigned long  dwMaxAttackInterval              = 1500;
    static const unsigned long  dwMaxAttackPerInterval           = 5;    // dwMaxAttackInterval���� ������ ���� ��
    static const unsigned long  dwMaxSkillHistorySize            = 5;
    static const long           nAdditionalCoolDownTimeInterval  = 1000; // ��ٿ� Ÿ�� ����. (1�ʱ��� ����)

    unsigned short usSkill_ID           = lpPktAt->m_AtType.m_wType;
    unsigned short usSkill_LockCount    = lpPktAt->m_AtType.m_cSkillLockCount;

    if (0 == (usSkill_ID & AtType::SKILL_BIT))
    {
        const long nDiffServerTime = (0 != m_dwLastServerAttackTime) ? 
            dwCurrentServerTime - m_dwLastServerAttackTime : dwAverageAttackTime;

        const long nDiffClientTime = (0 != m_dwLastClientAttackTime) ? 
            lpPktAt->GetServerInfo() - m_dwLastClientAttackTime : dwAverageAttackTime;

        m_dwLastServerAttackTime = dwCurrentServerTime;
        m_dwLastClientAttackTime = lpPktAt->GetServerInfo();
        
        m_AttackTimeList.push_back(AttackTime(nDiffServerTime, nDiffClientTime));
        m_SumAttackTime.add(nDiffServerTime, nDiffClientTime);

        if (dwMaxAttackPerInterval < m_AttackTimeList.size()) 
        {
            // ������ �׻� dwMaxAttackPerInterval���� �����Ѵ�.
            m_SumAttackTime.minus(m_AttackTimeList.front());
            m_AttackTimeList.pop_front();
        }

        // �Ϲ� ���� üũ
        if (nDiffClientTime <= 120)
        {
            ++m_dwHackFoundCount;
            LogAttackHack("Adjusted Client or Attack Hack found");

            m_SumAttackTime.clear();
            m_AttackTimeList.clear();
            return false;
        }
        else if (dwMaxAttackPerInterval == m_AttackTimeList.size())
        {
            if (m_SumAttackTime.m_nSumServerAttackTime < dwMaxAttackInterval || 
                m_SumAttackTime.m_nSumClientAttackTime < dwMaxAttackInterval)
            {
                ++m_dwHackFoundCount;
                LogAttackHack("Attack Hack found");

                m_SumAttackTime.clear();
                m_AttackTimeList.clear();                
                return false;
            }
        }
    }
    else if (0 == lpPktAt->m_AtType.m_cCasting)
    {
        // ��ų �����.
        CoolDownInfoList::iterator itr = 
            std::lower_bound(m_CoolDownInfo.begin(), m_CoolDownInfo.end(), usSkill_ID);

        if (itr != m_CoolDownInfo.end() && 
            usSkill_ID == itr->m_attackType.m_wType && 
            usSkill_LockCount < CSkillMgr::MAX_SKILL_LOCKCOUNT)
        {
            // �ѹ� ����ߴ� �ֹ���
            CoolDownInfo& coolDownInfo = *itr;

            ++coolDownInfo.m_dwSkillUsedCount;

            const long nServerDiffCoolDownTime = dwCurrentServerTime - 
                coolDownInfo.m_dwServerLastCastingTime;

            const long nClientDiffCoolDownTime = lpPktAt->GetServerInfo() -
                coolDownInfo.m_dwClientLastCastingTime;

            const long nMinCoolDownTime = 
                static_cast<unsigned long>(coolDownInfo.m_lpSkillProtoType[coolDownInfo.m_attackType.m_cSkillLockCount].m_dwCoolDownTime * 
				m_lpCharacter->GetStatus().m_StatusInfo.m_fCoolDownRevisionRate / 100.0f);
    
            m_SkillHistoryList.push_back(SkillHistory(lpPktAt->m_AtType, nMinCoolDownTime,
                nServerDiffCoolDownTime, nClientDiffCoolDownTime));

            if (dwMaxSkillHistorySize < m_SkillHistoryList.size())
            {                
                m_SkillHistoryList.pop_front();
            }

            if (nServerDiffCoolDownTime + nAdditionalCoolDownTimeInterval < nMinCoolDownTime
                || nClientDiffCoolDownTime + nAdditionalCoolDownTimeInterval < nMinCoolDownTime)
            {
                ++m_dwHackFoundCount;

                // ���������� ĳ���� �� �ֹ��� �ð����� ���� �ð��� �� ���� ��ٿ� �ð����� �۴�.
                LogSkillCoolDownTime(*itr, nMinCoolDownTime, 
                    nServerDiffCoolDownTime, nClientDiffCoolDownTime);

                coolDownInfo.m_attackType               = lpPktAt->m_AtType;
                coolDownInfo.m_dwServerLastCastingTime  = dwCurrentServerTime;
                coolDownInfo.m_dwClientLastCastingTime  = lpPktAt->GetServerInfo();
                return false;
            }

            coolDownInfo.m_attackType               = lpPktAt->m_AtType;
            coolDownInfo.m_dwServerLastCastingTime  = dwCurrentServerTime;
            coolDownInfo.m_dwClientLastCastingTime  = lpPktAt->GetServerInfo();
        }
        else
        {
            // �� �ֹ��� �������.
            const Skill::ProtoType* lpSkillProtoType = 
                CSkillMgr::GetInstance().GetSkillProtoType(usSkill_ID);

            if (NULL != lpSkillProtoType && usSkill_LockCount < CSkillMgr::MAX_SKILL_LOCKCOUNT)
            {
                // 0x9305�� FastHit�̴�. (���Ӱ��ݽ�ų)
                if (0 != lpSkillProtoType->m_dwCoolDownTime && 
                    Skill::Type::CHANT != lpSkillProtoType->m_eSkillType &&
                    0x9305 != usSkill_ID)
                {
                    // ��ٿ��� �ִ� �ֹ��̰�, �ѹ��� ������� �ʾҴ� �ֹ���
                    // FastHit��ų�� ��� ������.
                    m_CoolDownInfo.insert(itr, CoolDownInfo(lpSkillProtoType, 
                        lpPktAt->m_AtType, dwCurrentServerTime, lpPktAt->GetServerInfo()));
                }
            }
        }
    }

    return true;
}


void CSpeedHackCheck::LogAttackHack(const char* szDetailText)
{
    SOCKADDR_IN sockAddr;        

    CGameClientDispatch* lpDispatch = m_lpCharacter->GetDispatcher();

    if (0 != lpDispatch)
    {
        sockAddr = lpDispatch->GetRemoteAddr().get_addr_in();
    }
    else
    {
        memset(&sockAddr, 0, sizeof(SOCKADDR_IN));
    }

    const int MAX_BUFFER = 1023;
    char szBuffer[MAX_BUFFER + 1];
    szBuffer[MAX_BUFFER] = 0;

    int nLength = _snprintf(szBuffer, MAX_BUFFER, 
        "/UID:%d/CID:0x%08x/Name:%s/IP:%15s/%s/ServerDiff(ClientDiff):",
        m_lpCharacter->GetUID(), m_lpCharacter->GetCID(), m_lpCharacter->GetCharacterName(),
        inet_ntoa(sockAddr.sin_addr), szDetailText);

    AttackTimeList::iterator pos = m_AttackTimeList.begin();
    AttackTimeList::iterator end = m_AttackTimeList.end();

    for (;pos != end; ++pos) 
    {
        AttackTime& atTime = *pos;
        nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, "%dms(%dms),",
            atTime.m_nDiffServerInterval, atTime.m_nDiffClientInterval);
    }
   
    nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "/ServerSum: %dms/ClientSum: %dms/AttackCount: %d/", 
        m_SumAttackTime.m_nSumServerAttackTime, 
        m_SumAttackTime.m_nSumClientAttackTime,
        m_AttackTimeList.size());

    ERRLOG0(g_Log, szBuffer);

	// DB�� �� �α׸� �����.
}


void CSpeedHackCheck::LogSkillCoolDownTime(const CoolDownInfo& coolDownInfo, 
                                           long nMinCoolDownTime,
                                           long nServerCoolDownTimeInterval, 
                                           long nClientCoolDownTimeInterval)
{
    SOCKADDR_IN sockAddr;        

    CGameClientDispatch* lpDispatch = m_lpCharacter->GetDispatcher();

    if (0 != lpDispatch)
    {
        sockAddr = lpDispatch->GetRemoteAddr().get_addr_in();
    }
    else
    {
        memset(&sockAddr, 0, sizeof(SOCKADDR_IN));
    }

    const int MAX_BUFFER = 4096;
    char szBuffer[MAX_BUFFER + 1];
    szBuffer[MAX_BUFFER] = 0;

    int nLength = _snprintf(szBuffer, MAX_BUFFER, 
        "/UID:%d/CID:0x%08x/Name:%s/IP:%15s/Cooldown Hack found. /"
        "Skill_ID:0x%04x/Lc:%d/Lv:%d/CharLevel:%d/Dex:%d/"
        "ServerInterval:%dms/ClientInterval:%dms/MinCoolDownTime:%dms/SkillUsedCount:%d/SkillHistory-",

        m_lpCharacter->GetUID(), m_lpCharacter->GetCID(), m_lpCharacter->GetCharacterName(),
        inet_ntoa(sockAddr.sin_addr),
        coolDownInfo.m_attackType.m_wType, coolDownInfo.m_attackType.m_cSkillLockCount, 
        coolDownInfo.m_attackType.m_cSkillLevel, 
        m_lpCharacter->GetStatus().m_nLevel, m_lpCharacter->GetCharStatus().m_nDEX, 
        nServerCoolDownTimeInterval, nClientCoolDownTimeInterval, 
        nMinCoolDownTime, coolDownInfo.m_dwSkillUsedCount);

    SkillHistoryList::iterator pos = m_SkillHistoryList.begin();
    SkillHistoryList::iterator end = m_SkillHistoryList.end();
    
    for (;pos != end; ++pos)
    {
        SkillHistory& skillHistory = *pos;

        nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
            "ID:0x%04x Lc:%2d Lv:%2d SInterval:%d CInterval:%d MinCoolDownTime:%d/",
            skillHistory.m_attackType.m_wType, skillHistory.m_attackType.m_cSkillLockCount, 
            skillHistory.m_attackType.m_cSkillLevel, skillHistory.m_nServerInterval,
            skillHistory.m_nClientInterval, skillHistory.m_nMinCoolDownTime);
    }

    ERRLOG0(g_Log, szBuffer);
}
