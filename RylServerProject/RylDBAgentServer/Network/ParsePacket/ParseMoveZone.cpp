#include "stdafx.h"
#include "ParseMoveZone.h"

#include <Creature/CreatureManager.h>
#include <Network/Dispatch/GameDispatch.h>
#include <Network/SendPacket/SendMoveZone.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

#include <DataStorage/CharacterData.h>
#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>

#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>

#include <GameTime/GameTimeDBMgr.h>

namespace DBAgent
{
namespace ParseMoveZone
{

bool CharMoveZone(CSendStream& SendStream, PktSZMvD* lpPktSZMvD);
bool ServerZone(CSendStream& SendStream, PktSA* lpPktSA);
bool ServerZoneEnd(CSendStream& SendStream, PktDD* lpPktDD);


bool Parse(CSendStream& SendStream, PktBase* lpPktBase)
{    
    bool bResult = false;
    
    PktDD* lpPktDD = static_cast<PktDD*>(lpPktBase);

    switch (lpPktDD->m_wCmd) 
    {
    case PktDD::SCmdCharMoveZone:	
        bResult = CharMoveZone(SendStream, static_cast<PktSZMvD*>(lpPktDD));
        break;

    case PktDD::SCmdServerZone:		
        bResult = ServerZone(SendStream, static_cast<PktSA*>(lpPktDD));   
        break;

    case PktDD::SCmdServerZoneEnd:  
        bResult = ServerZoneEnd(SendStream, lpPktDD);
        break;

    default:
        ERRLOG1(g_Log, "ZoneMove ��Ŷ ó�� ���� : �� �� ���� ��Ŷ CMD:0x%02X�� �޾ҽ��ϴ�.", 
            lpPktDD->m_wCmd);

        bResult = true;
        break;
    }

    return bResult;
}


class CFindGameServerDispatch
{
public:

    CFindGameServerDispatch(CGameDispatch*& lpGameDispatch, SERVER_ID serverID)
        :   m_lpGameDispatch(lpGameDispatch), m_serverID(serverID)
    {
        m_lpGameDispatch = 0;
    }

    bool operator() (unsigned long dwServerID, CPacketDispatch& packetDispatch)
    {
        if(0 == m_lpGameDispatch)
        {
            SERVER_ID serverID;
            serverID.dwID = dwServerID;

            // ���� ID�� ������ ����, �ٸ�����, ä�θ� -1�̰� �ٸ��� ������ ����.
            if ((serverID.dwID == m_serverID.dwID) ||
                
                (-1 == m_serverID.GetChannel() && 
                serverID.GetZone() == m_serverID.GetZone() &&
                serverID.GetGroup() == m_serverID.GetGroup() &&
                serverID.GetType() == m_serverID.GetType()))
            {
                m_lpGameDispatch = &static_cast<CGameDispatch&>(packetDispatch);
            }
        }

        return true;
    }

private:

    CGameDispatch*& m_lpGameDispatch;    
    SERVER_ID       m_serverID;
};


bool CharMoveZone(CSendStream& SendStream, PktSZMvD* lpPktSZMvD)
{
    unsigned long dwRequestKey  = lpPktSZMvD->m_dwRequestKey;
    unsigned long dwUID         = lpPktSZMvD->m_dwUserID;

    const POS& Pos  = lpPktSZMvD->m_NewPos;
    char cZone      = lpPktSZMvD->m_cZone;
    char cChannel	= lpPktSZMvD->m_cChannel;
    
#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseMoveZone::CharMoveZone : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, 0, 0, 0);
#endif

    unsigned short usError = 0;

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0;

    SERVER_ID serverID;
    serverID.sID.Type       = CServerSetup::GameServer;
    serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
    serverID.sID.Channel    = cChannel;
    serverID.sID.ID         = cZone;

    CGameDispatch* lpGameDispatch = 0;

    CGameDispatch::GetDispatchTable().Process(
        CFindGameServerDispatch(lpGameDispatch, serverID));

    if (0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ĳ���� �̵� ���� : ������ ���� ���� �ʽ��ϴ�.", dwUID);
        usError = PktSZMvDAck::FAIL_INVALID_CHAR;
    }
    else if (0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        ERRLOG1(g_Log, "UID:%10u / ĳ���� �̵� ���� : ĳ���� �����Ͱ� �ε���� �ʾҽ��ϴ�.", dwUID);
        usError = PktSZMvDAck::FAIL_INVALID_CHAR;
    }
    else if (0 == lpGameDispatch)
    {
        ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / ĳ���� �̵� ���� : ������ �����ϴ�.",
            dwUID, serverID.dwID);

		usError = PktSZMvDAck::FAIL_INVALID_ZONE;
    }
    else if (0 == lpSessionData->GetAdminLevel())
    {
		// edith 2009.06.13 ������ ���� �� üũ
		if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= cZone && cZone <= SERVER_ID::STONE_WAR3)
		{
			if (CServerSetup::GetInstance().GetBattleLimit() < lpGameDispatch->GetCharNum())
			{
				// ����� �ʹ� ����.
                ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / ĳ���� �̵� ���� : ����� �ʹ� �����ϴ�.",
                    dwUID, serverID.dwID);

                usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;
			}
            else
            {
				int maxDefaultUser = 200;
				float limitPer = (float)CServerSetup::GetInstance().GetBattleLimitPer()/100.f;
				if(limitPer < 1.0f)
					limitPer = 1.0f;

				int akhanUser = lpGameDispatch->GetCharNum(CClass::AKHAN);
				int humanUser = lpGameDispatch->GetCharNum(CClass::HUMAN);

                // ���� �ΰ��̰�, �ΰ��� ��ĭ���� 1.5�� ���� ��Ȳ�̸� ���� �Ұ�.
                if (CClass::HUMAN == lpCharacterData->GetRace())
				{
					int overCount = (int)((float)akhanUser * limitPer) - akhanUser;
					if(overCount < maxDefaultUser)
						overCount = maxDefaultUser;

					int overUser = akhanUser + overCount;

                    if(overUser < lpGameDispatch->GetCharNum(CClass::HUMAN))
					{
						ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / ĳ���� �̵� ���� : �ΰ����� �ұ���.",
							dwUID, serverID.dwID);

						usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;                    
					}
				}
                // ���� ��ĭ�̰�, ��ĭ�� �ΰ����� 1.5�� ���� ��Ȳ�̸� ���� �Ұ�.
                else if (CClass::AKHAN == lpCharacterData->GetRace())
				{
					int overCount = (int)((float)humanUser * limitPer) - humanUser;
					if(overCount < maxDefaultUser)
						overCount = maxDefaultUser;

					int overUser = humanUser + overCount;

                    if(overUser < lpGameDispatch->GetCharNum(CClass::AKHAN))
					{
						ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / ĳ���� �̵� ���� : ��ĭ���� �ұ���.",
							dwUID, serverID.dwID);

						usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;
					}
				}
            }	
		}
        else if (SERVER_ID::ZONE3 == cZone)
        {
	        // ��Ʋ�׶����� ��� �ο� ���� üũ
			// 2008.10.12 ZONE3�� ���� ���� ����.
			ERRLOG3(g_Log, "UID:%10u / 3�� �� ���� �õ� : Zone(%d,%d)", dwUID, cZone, cChannel);
			usError = PktSZMvDAck::FAIL_INVALID_CHAR;

/*
            SYSTEMTIME systemTime;
            GetSystemTime(&systemTime);

            // ��Ʋ �׶��� �޽� �ð����ȿ��� �� ������ �ʱ� ������ �̵� �����ش�.
            if ((systemTime.wMinute >= STATUE_REST_TIME_1ST_START && systemTime.wMinute <= STATUE_REST_TIME_1ST_END) ||
            (systemTime.wMinute >= STATUE_REST_TIME_2ND_START && systemTime.wMinute <= STATUE_REST_TIME_2ND_END))
            {
                cZone = lpDataSession->GetCharDataSession().GetCharStartZone(lpCharacterData->GetRace());
            }
*/          
/*            SERVER_ID serverID;
            serverID.sID.Type       = CServerSetup::GameServer;
            serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
            serverID.sID.Channel    = cChannel;
            serverID.sID.ID         = cZone;

            CGameDispatch* lpGameDispatch = 0;

            CGameDispatch::GetDispatchTable().Process(
                CFindGameServerDispatch(lpGameDispatch, serverID));

            if (0 == lpGameDispatch)
            {
                ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / ĳ���� �̵� ���� : ������ �����ϴ�.",
                    dwUID, serverID.dwID);

                usError = PktSZMvDAck::FAIL_INVALID_ZONE;
            }
            else if (CServerSetup::GetInstance().GetBattleLimit() > lpGameDispatch->GetCharNum())
            {
                // ���� �ΰ��̰�, �ΰ��� ��ĭ���� 1.5�� ���� ��Ȳ�̸� ���� �Ұ�.
                if (CClass::HUMAN == lpCharacterData->GetRace() &&
                    lpGameDispatch->GetCharNum(CClass::AKHAN) * 1.5 < lpGameDispatch->GetCharNum(CClass::HUMAN))
                {
                    usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;                    
                }
                // ���� ��ĭ�̰�, ��ĭ�� �ΰ����� 1.5�� ���� ��Ȳ�̸� ���� �Ұ�.
                else if (CClass::AKHAN == lpCharacterData->GetRace() &&
                    lpGameDispatch->GetCharNum(CClass::HUMAN    ) * 1.5 < lpGameDispatch->GetCharNum(CClass::AKHAN))
                {
                    usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;
                }
            }	
*/
        }
    }

//	if(SERVER_ID::STONE_WAR1 <= cZone && cZone <= SERVER_ID::STONE_WAR3)
//		usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;                    

    if(0 == usError && 0 != lpSessionData)
    {
        // �̸� �� �� ����
        if (!lpSessionData->SetMoveZone(cZone, Pos))
        {
/*
			if(SERVER_ID::ZONE5 == cZone && !CGameTimeDBMgr::GetInstance().IsEnterTime())
			{
				// �׼��� �ű��� �������ѿ� �ɷ���.
				DETLOG5(g_Log, "UID:%10u / Zone:%2u / X:%f / Y:%f / Z:%f / ĳ���� �̵� ���� : �ű��� ��������.", 
					dwUID, cZone, Pos.fPointX, Pos.fPointY, Pos.fPointZ);
			}
			else
			{
				ERRLOG5(g_Log, "UID:%10u / Zone:%2u / X:%f / Y:%f / Z:%f / ĳ���� �̵� ���� : �߸��� ������ �̵� �õ�.", 
					dwUID, cZone, Pos.fPointX, Pos.fPointY, Pos.fPointZ);
			}
*/
			ERRLOG5(g_Log, "UID:%10u / Zone:%2u / X:%f / Y:%f / Z:%f / ĳ���� �̵� ���� : �߸��� ������ �̵� �õ�.", 
				dwUID, cZone, Pos.fPointX, Pos.fPointY, Pos.fPointZ);

            usError = PktSZMvDAck::FAIL_INVALID_ZONE;            
        }
    }

	// edith 2009.06.28 ���̵����н� ���� �÷��׸� ���ش�.
	// gameserver���� dbagnet�� ��Ŷ �����ϰ� �ҽ� ������.
	if(0 != usError)
	{
		if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= cZone && cZone <= SERVER_ID::STONE_WAR3)
			lpCharacterData->SetRealmWarFlag(Creature::WAR_OFF);
		else if(SERVER_ID::CAPITAL == cZone)
			lpCharacterData->SetGuildWarFlag(Creature::WAR_OFF);
	}

    return (-1 == cChannel || 0 != usError) ? 
        // ä���� -1�̰ų�, ������ �ִ� ��� �� ��Ŷ�� ����.
        SendPacket::CharMoveZone(SendStream, dwRequestKey, cZone, usError)
    :   SendPacket::ServerZone(SendStream, dwRequestKey, cZone, cChannel);
}



bool ServerZone(CSendStream& SendStream, PktSA* lpPktSA)
{
    unsigned long dwUID = lpPktSA->m_dwUserID;
    unsigned short usError = 0;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseMoveZone::ServerZone : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, 0, 0, 0);
#endif

    using namespace DataStorage;   
    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    if (0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ServerZone ���� : ������ ���� ���� �ʽ��ϴ�.", dwUID);
        usError = PktSAAck::SERVER_ERROR;
    }

    return SendPacket::ServerZone(SendStream, 
        lpPktSA->m_dwRequestKey, lpPktSA->m_cZone, lpPktSA->m_cChannel, usError);
}



bool ServerZoneEnd(CSendStream& SendStream, PktDD* lpPktDD)
{
#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseMoveZone::ServerZoneEnd : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		0, 0, 0, 0);
#endif

    return SendPacket::ServerZoneEnd(SendStream,  lpPktDD->m_dwRequestKey);
}

}
}
