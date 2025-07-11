
#include "stdafx.h"

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

#include <Network/Session/Session.h>
#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacket.h>

#include <Network/Packet/PacketStruct/GuildPacket.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include "ParseCharLoginOut.h"
#include "GameClientDispatch.h"
#include "SendCharLoginOut.h"

#include <Network/Packet/PacketCommand.h>

bool GameClientParsePacket::ParseCharLogin(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCLi), GameClientDispatch);

	PktCLi*         lpPktCLi    = static_cast<PktCLi *>(lpPktBase);
    unsigned long   dwSID       = lpPktCLi->m_dwSessionID;
    unsigned long   dwUID       = lpPktCLi->m_dwUserID;
    unsigned long   dwCID       = lpPktCLi->m_dwCharID;

    GameClientDispatch.SetUID(dwUID);

	// 캐릭터가 매니저에 있는지 확인
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL != lpCharacter) 
	{
        // 캐릭터가 이미 서버에 있습니다.접속을 끊도록, 에러 코드를 넘깁니다.
        lpCharacter = NULL;

        LOG_INOUT(DETLOG3(g_Log, "UID:%d / CID:0x%08x / SID:0x%08x 캐릭터가 이미 서버에 있습니다. 접속을 끊습니다.", 
            dwUID, dwCID, dwSID));
    }
	else
	{		
        LOG_INOUT(DETLOG3(g_Log, "UID:%d / CID:0x%08x / SID:0x%08x 로그인을 처리합니다. DBAgent로 로그인을 전달합니다.", 
            dwUID, dwCID, dwSID));

        CDBRequest DBRequest(GameClientDispatch, 600);

        if (DBRequest.IsValid())
        {
            if (GameClientSendPacket::SendLoginToDBAgent(DBRequest.GetSendStream(),
                DBRequest.GetRequestKey(), dwSID, dwUID, dwCID,
				GameClientDispatch.GetRemoteAddr().get_addr_in().sin_addr))
            {
                GameClientDispatch.PushRequestKey(DBRequest.GetRequestKey());

                LOG_INOUT(DETLOG5(g_Log, "UID:%d / CID:0x%08x / SID:0x%08x / RequestKey:%d / DispatchPointer:0x%p 로그인을 처리합니다."
                    "DBAgent로 로그인을 전달하는 데 성공하였습니다.", 
                    dwUID, dwCID, dwSID, DBRequest.GetRequestKey(), &GameClientDispatch));

                return true;
            }

			DBRequest.CancelRequest();
        }
	}

    return GameClientSendPacket::SendCharLogin(GameClientDispatch.GetSendStream(), lpCharacter, 
		&GameClientDispatch.GetRemoteAddr().get_addr_in(), PktBase::NO_SERVER_ERR);
}


bool GameClientParsePacket::ParseCharMoveZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSZMv), GameClientDispatch);

	PktSZMv*        lpPktSZMv   = static_cast<PktSZMv *>(lpPktBase);
    CCharacter*     lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	// Admin 캐릭터에는 예외처리 (QA 쪽 요청).
	if(lpCharacter->GetAdminLevel())
	{
		goto lb_move;
	}

	// 길드전 귀속, 국가전 귀속 체크
	if (CGameTimeMgr::GetInstance().IsGuildWarTime() &&
		(lpCharacter->GetGuildWarFlag() == Creature::WAR_ON || lpCharacter->GetGuildWarFlag() == Creature::WAR_INSTANCE))
	{
		return GameClientSendPacket::SendCharMoveZone(
			GameClientDispatch.GetSendStream(), NULL, 0, 0, 1);
	}
	
	// 컨텐츠 : 다크 카나번 국가 전쟁
	if (16 <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= 18)//hayzohar statue war fix was (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))
	{
		if ((CGameTimeMgr::GetInstance().IsRealmWarReadyTime() || CGameTimeMgr::GetInstance().IsRealmWarTime()) &&
			(lpCharacter->GetRealmWarFlag() == Creature::WAR_ON || lpCharacter->GetRealmWarFlag() == Creature::WAR_INSTANCE))
		{
			return GameClientSendPacket::SendCharMoveZone(
				GameClientDispatch.GetSendStream(), NULL, 0, 0, 1);
		}
	}

lb_move:
	
	if (lpCharacter->MoveZone(lpPktSZMv->m_NewPos, lpPktSZMv->m_cZone, lpPktSZMv->m_cChannel))	// 채널은 없으므로 -1
	{
		return true;
	}

    ERRLOG0(g_Log, "MoveZone패킷을 DBAgent로 보내는 데 실패했습니다.");

    return GameClientSendPacket::SendCharMoveZone(
        GameClientDispatch.GetSendStream(), NULL, 0, 0, 1);
}


bool GameClientParsePacket::ParseCharLogout(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCLo), GameClientDispatch);

	PktCLo*         lpPktCLo    = static_cast<PktCLo *>(lpPktBase);    
    CCharacter*     lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());
    
    unsigned long dwCID = lpPktCLo->m_dwCharID;

    if (dwCID == lpCharacter->GetCID())
    {
        lpCharacter->SetDispatcher(NULL);
    }

    return false;
}


bool GameClientParsePacket::ParseServerZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSZ), GameClientDispatch);

    PktSZ*		lpPktSZ = reinterpret_cast<PktSZ *>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

    if (lpCharacter->IsOperationFlagSet(CCharacter::SERVERZONE_PROCESSED))
    {
        return true;
    }

    CDBRequest  DBRequest(GameClientDispatch, 600);
    
    if (DBRequest.IsValid())
    {
        if (GameClientSendPacket::SendServerZoneToDBAgent(DBRequest.GetSendStream(),
            DBRequest.GetRequestKey(), GameClientDispatch.GetUID(), 
            lpPktSZ->m_cZone, static_cast<unsigned char>(lpPktSZ->m_cChannel)))
        {
            DETLOG5(g_Log, "UID:%d/CID:0x%08x(0x%p)/RequestKey:%d/DispatchPointer:0x%p ServerZone패킷을 보냈습니다.",
                lpCharacter->GetUID(), lpCharacter->GetCID(), lpCharacter,
                DBRequest.GetRequestKey(), &GameClientDispatch);

            lpCharacter->SetOperationFlag(CCharacter::SERVERZONE_PROCESSED);
            GameClientDispatch.PushRequestKey(DBRequest.GetRequestKey());
            return true;
        }

		DBRequest.CancelRequest();
    }

    ERRLOG0(g_Log, "ServerZone 패킷을 보내는 데 실패했습니다.");

    return GameClientSendPacket::SendCharMoveZone(
        GameClientDispatch.GetSendStream(), NULL, 0, 0, 1);
}


bool GameClientParsePacket::ParseCSAuth(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCSAuth), GameClientDispatch);

	PktCSAuth*	lpPktCSAuth = reinterpret_cast<PktCSAuth *>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	switch(lpPktCSAuth->m_AuthCodeType)
	{
	case 1:
/*		// edith 2009.08.11 게임가드 2.5 업그레이드
		if (false == GameClientDispatch.CheckAuthCode(lpPktCSAuth->m_dwAuthCode))
		{
			ERRLOG1(g_Log, "CID:0x%08x 게임 가드 인증 코드가 잘못되어 연결을 끊습니다.", lpCharacter->GetCID());
			GameClientDispatch.Disconnect();
			return false;
		}
*/
		break;

	case 2:
		GameClientDispatch.SetAuthAnswer(lpPktCSAuth->m_AuthCode2);

		if (false == GameClientDispatch.CheckAuthAnswer())
		{
			ERRLOG1(g_Log, "CID:0x%08x 게임 가드 인증 코드(2)가 잘못되어 연결을 끊습니다.", lpCharacter->GetCID());
			GameClientDispatch.Disconnect();
			return false;
		}
		break;

	default:

		ERRLOG2(g_Log, "CID:0x%08x 클라이언트가 알 수 없는 게임 가드 인증 타입(%d) 를 보냈습니다",
			lpCharacter->GetCID(), lpPktCSAuth->m_AuthCodeType);
		return false;
	}

	return true;
}

bool GameClientParsePacket::ParseKeyInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktKeyInfo), GameClientDispatch);

	PktKeyInfo*	lpPktKeyInfo = reinterpret_cast<PktKeyInfo*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if(0 != lpDBAgentDispatch)
	{
		CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();

		char* szBuffer = SendStream.GetBuffer(sizeof(PktKeyInfo));

		if (0 != szBuffer)
		{	   
			PktKeyInfo*	pPktKeyInfo = reinterpret_cast<PktKeyInfo*>(szBuffer);

			pPktKeyInfo->m_dwCID = lpPktKeyInfo->m_dwCID;
			memcpy(pPktKeyInfo->m_dwKeyInfo, lpPktKeyInfo->m_dwKeyInfo, sizeof(unsigned long)*PktKeyInfo::MAX_KEY_INFO);

			return SendStream.WrapHeader(sizeof(PktKeyInfo), CmdKeyInfo, 0, 0);
		}
	}

	return false;
}