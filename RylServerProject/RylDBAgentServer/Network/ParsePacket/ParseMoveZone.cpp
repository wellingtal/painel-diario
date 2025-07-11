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
        ERRLOG1(g_Log, "ZoneMove 패킷 처리 에러 : 알 수 없는 패킷 CMD:0x%02X를 받았습니다.", 
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

            // 서버 ID가 같으면 선택, 다르더라도, 채널만 -1이고 다른게 같으면 선택.
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
        ERRLOG1(g_Log, "UID:%10u / 캐릭터 이동 실패 : 세션이 열려 있지 않습니다.", dwUID);
        usError = PktSZMvDAck::FAIL_INVALID_CHAR;
    }
    else if (0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        ERRLOG1(g_Log, "UID:%10u / 캐릭터 이동 실패 : 캐릭터 데이터가 로드되지 않았습니다.", dwUID);
        usError = PktSZMvDAck::FAIL_INVALID_CHAR;
    }
    else if (0 == lpGameDispatch)
    {
        ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / 캐릭터 이동 실패 : 서버가 없습니다.",
            dwUID, serverID.dwID);

		usError = PktSZMvDAck::FAIL_INVALID_ZONE;
    }
    else if (0 == lpSessionData->GetAdminLevel())
    {
		// edith 2009.06.13 석상전 유저 수 체크
		if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= cZone && cZone <= SERVER_ID::STONE_WAR3)
		{
			if (CServerSetup::GetInstance().GetBattleLimit() < lpGameDispatch->GetCharNum())
			{
				// 사람이 너무 많다.
                ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / 캐릭터 이동 실패 : 사람이 너무 많습니다.",
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

                // 내가 인간이고, 인간이 아칸보다 1.5배 많은 상황이면 입장 불가.
                if (CClass::HUMAN == lpCharacterData->GetRace())
				{
					int overCount = (int)((float)akhanUser * limitPer) - akhanUser;
					if(overCount < maxDefaultUser)
						overCount = maxDefaultUser;

					int overUser = akhanUser + overCount;

                    if(overUser < lpGameDispatch->GetCharNum(CClass::HUMAN))
					{
						ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / 캐릭터 이동 실패 : 인간종족 불균형.",
							dwUID, serverID.dwID);

						usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;                    
					}
				}
                // 내가 아칸이고, 아칸이 인간보다 1.5배 많은 상황이면 입장 불가.
                else if (CClass::AKHAN == lpCharacterData->GetRace())
				{
					int overCount = (int)((float)humanUser * limitPer) - humanUser;
					if(overCount < maxDefaultUser)
						overCount = maxDefaultUser;

					int overUser = humanUser + overCount;

                    if(overUser < lpGameDispatch->GetCharNum(CClass::AKHAN))
					{
						ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / 캐릭터 이동 실패 : 아칸종족 불균형.",
							dwUID, serverID.dwID);

						usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;
					}
				}
            }	
		}
        else if (SERVER_ID::ZONE3 == cZone)
        {
	        // 배틀그라운드의 경우 인원 제한 체크
			// 2008.10.12 ZONE3번 유저 설정 제거.
			ERRLOG3(g_Log, "UID:%10u / 3번 존 접속 시도 : Zone(%d,%d)", dwUID, cZone, cChannel);
			usError = PktSZMvDAck::FAIL_INVALID_CHAR;

/*
            SYSTEMTIME systemTime;
            GetSystemTime(&systemTime);

            // 배틀 그라운드 휴식 시간동안에는 각 종족의 초기 존으로 이동 시켜준다.
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
                ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / 캐릭터 이동 실패 : 서버가 없습니다.",
                    dwUID, serverID.dwID);

                usError = PktSZMvDAck::FAIL_INVALID_ZONE;
            }
            else if (CServerSetup::GetInstance().GetBattleLimit() > lpGameDispatch->GetCharNum())
            {
                // 내가 인간이고, 인간이 아칸보다 1.5배 많은 상황이면 입장 불가.
                if (CClass::HUMAN == lpCharacterData->GetRace() &&
                    lpGameDispatch->GetCharNum(CClass::AKHAN) * 1.5 < lpGameDispatch->GetCharNum(CClass::HUMAN))
                {
                    usError = PktSZMvDAck::FAIL_PERSONNEL_OVER;                    
                }
                // 내가 아칸이고, 아칸이 인간보다 1.5배 많은 상황이면 입장 불가.
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
        // 미리 갈 존 설정
        if (!lpSessionData->SetMoveZone(cZone, Pos))
        {
/*
			if(SERVER_ID::ZONE5 == cZone && !CGameTimeDBMgr::GetInstance().IsEnterTime())
			{
				// 테섭용 신규존 진입제한에 걸렸음.
				DETLOG5(g_Log, "UID:%10u / Zone:%2u / X:%f / Y:%f / Z:%f / 캐릭터 이동 실패 : 신규존 진입제한.", 
					dwUID, cZone, Pos.fPointX, Pos.fPointY, Pos.fPointZ);
			}
			else
			{
				ERRLOG5(g_Log, "UID:%10u / Zone:%2u / X:%f / Y:%f / Z:%f / 캐릭터 이동 실패 : 잘못된 존으로 이동 시도.", 
					dwUID, cZone, Pos.fPointX, Pos.fPointY, Pos.fPointZ);
			}
*/
			ERRLOG5(g_Log, "UID:%10u / Zone:%2u / X:%f / Y:%f / Z:%f / 캐릭터 이동 실패 : 잘못된 존으로 이동 시도.", 
				dwUID, cZone, Pos.fPointX, Pos.fPointY, Pos.fPointZ);

            usError = PktSZMvDAck::FAIL_INVALID_ZONE;            
        }
    }

	// edith 2009.06.28 존이동실패시 전쟁 플래그를 꺼준다.
	// gameserver에서 dbagnet로 패킷 전달하게 소스 수정함.
	if(0 != usError)
	{
		if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= cZone && cZone <= SERVER_ID::STONE_WAR3)
			lpCharacterData->SetRealmWarFlag(Creature::WAR_OFF);
		else if(SERVER_ID::CAPITAL == cZone)
			lpCharacterData->SetGuildWarFlag(Creature::WAR_OFF);
	}

    return (-1 == cChannel || 0 != usError) ? 
        // 채널이 -1이거나, 에러가 있는 경우 이 패킷을 보냄.
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
        ERRLOG1(g_Log, "UID:%10u / ServerZone 실패 : 세션이 열려 있지 않습니다.", dwUID);
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
