
#include "stdafx.h"

#include <Log/LogStruct.h>
#include <Log/GuildLog.h>
#include <Log/ItemLog.h>
#include <Log/CharacterLog.h>


#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/SingleDispatchStorage.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

#include <Community/Guild/GuildStructure.h>

#include <Creature/Character/Character.h>

#include "DBAgentDispatch.h"
#include "GuildPacketParse.h"

#include <Community/Party/Party.h>

bool DBAgentPacketParse::ParseCreateGuild(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCreateGuild), DBAgentDispatch)

	PktCreateGuild* lpPktCG = static_cast<PktCreateGuild *>(lpPktBase);

	unsigned long dwCID = lpPktCG->m_dwCID;
	unsigned long dwGID = lpPktCG->m_dwGID;
	unsigned char cInclination = lpPktCG->m_cInclination;
	char szName[MAX_GUILD_NAME_LEN];
	strncpy(szName, lpPktCG->m_szGuildName, MAX_GUILD_NAME_LEN);
    szName[MAX_GUILD_NAME_LEN - 1] = 0;

	unsigned short wError = lpPktCG->GetError();
/*
	unsigned long dwCreateGold = 
		(GameRYL::CHINA == CServerSetup::GetInstance().GetNationType()) ? 
		ms_arySetLevelGoldForChina[0] : ms_arySetLevelGold[0];
*/
	unsigned long dwCreateGold = ms_arySetLevelGold[0];

	if (PktCreateGuild::FAIL_DISSOLVE_GUILD != wError)
    {
	    GAMELOG::LogGuildCreate(GAMELOG::sGuildLog::RESULT, dwGID, dwCID, dwCreateGold, wError);
    }

	switch (wError)
	{
		// 올바른 길드 생성
		case PktBase::NO_SERVER_ERR:
		{
			CGuildMgr::GetInstance().CreateGuild(dwCID, dwGID, cInclination, szName);

			CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
			if (NULL != lpCharacter)
			{
				CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
				if (NULL != lpGuild)
				{
					if (true == lpGuild->JoinMember(dwCID, MASTER, wError))
					{
						lpCharacter->DeductGold(dwCreateGold, true);
					}
				}

				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharCreateGuild(lpDispatch->GetSendStream(), 
						dwCID, dwGID, cInclination, szName, wError);
				}

				// 길드가 변경 되었을때 파티 정보를 보냄 //
				CParty* pParty = lpCharacter->GetParty();
				
				if(pParty)
				{
					CCharacterParty* lpParty = static_cast<CCharacterParty*>(pParty);

					lpParty->SendPartyMemberDataToDBAgent(dwCID, dwGID, 0, 0, 0, lpCharacter->GetCharacterName(), PktDD::SCmdChangeGuildPartyMem);

					// 친구 리스트에 길드 변경 메세지를 보낸다 //
					lpCharacter->FriendInfoUpdate(lpCharacter->GetUID(), lpCharacter->GetCID(), dwGID, lpCharacter->GetClass(), 
						lpCharacter->GetLevel(), CServerSetup::GetInstance().GetServerID());
				}
			}
			break;
		}

		// 클라이언트 측 오류
		// ex) 이미 존재하는 이름의 길드
		case PktCreateGuild::FAIL_ALREADY_NAME:
		{
			CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
			if (NULL != lpCharacter)
			{
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharCreateGuild(lpDispatch->GetSendStream(), 
						dwCID, dwGID, cInclination, szName, wError);
				}
			}
			break;
		}

		// 길드 해체
		case PktCreateGuild::FAIL_DISSOLVE_GUILD:
		{
			// 파티에 길드가 변경 되었을때 보냄 //
			MemberList sMemberList;

			if(CGuildMgr::GetInstance().GetMemberList(dwGID, &sMemberList))
			{				
				MemberList::iterator pos = sMemberList.begin();
				MemberList::iterator end = sMemberList.end();

				for(; pos!=end;pos++)
				{
					CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(pos->m_dwCID);

					if(lpCharacter)
					{
						CParty* pParty = lpCharacter->GetParty();

						if(pParty)
						{
							CCharacterParty* lpParty = static_cast<CCharacterParty*>(pParty);

							lpParty->SendPartyMemberDataToDBAgent(dwCID, 0, 0, 0, 0, lpCharacter->GetCharacterName(), PktDD::SCmdChangeGuildPartyMem);

							// 친구 리스트에 길드 변경 메세지를 보낸다 //
							lpCharacter->FriendInfoUpdate(lpCharacter->GetUID(), lpCharacter->GetCID(), 0, lpCharacter->GetClass(), 
								lpCharacter->GetLevel(), CServerSetup::GetInstance().GetServerID());
						}
					}										
				}				
			}				

			CGuildMgr::GetInstance().DissolveGuild(dwGID);
			
            const char* szReason = "Unknown reason";

            ServerInfo serverInfo;
            serverInfo.dwServerInfo = lpPktCG->GetServerInfo();

#define CASE_SET_REASON(szCommand, szReason) case PktCreateGuild::##szCommand: szReason = #szCommand; break

            switch(serverInfo.SrvState.wSrvState)
            {
            CASE_SET_REASON(NONE_LOGIN, szReason);
            CASE_SET_REASON(MIN_MEMBER_TIMEOUT, szReason);
            CASE_SET_REASON(NONE_NEXT_GUILDMASTER_BY_NONE_LOGIN, szReason);
            CASE_SET_REASON(NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_OUT, szReason);
            CASE_SET_REASON(NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_DELETED, szReason);
            }

			GAMELOG::LogGuildDispose(GAMELOG::sGuildLog::RESULT, dwGID, szReason, __FILE__, __LINE__, 0);
			break;
		}
	}

	return true;
}

bool DBAgentPacketParse::ParseGuildCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktGuildCmd* lpPktGC = static_cast<PktGuildCmd *>(lpPktBase);

	unsigned long dwGID = lpPktGC->m_dwGID;
	unsigned long dwSenderID = lpPktGC->m_dwSenderID;
	unsigned long dwReferenceID = lpPktGC->m_dwReferenceID;
	unsigned short wCmd = lpPktGC->m_wCmd;

	unsigned short wError = lpPktGC->GetError();

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL != lpGuild)
	{
		switch (wCmd)
		{
			// Send : 가입한 캐릭터
			// Reference : 직위
			case PktGuildCmd::GC_JOIN:
			{
				unsigned long dwTitle = JOIN_WAIT;

				CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderID);

				if (PktBase::NO_SERVER_ERR == wError)
				{
					MemberInfo memberInfo = *(reinterpret_cast<MemberInfo *>(lpPktGC + 1));
					lpGuild->JoinMember(memberInfo);

					dwTitle = memberInfo.m_MemberListInfo.m_cTitle;

					char* szPacket = reinterpret_cast<char *>(lpPktGC);
					if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildCmd), CmdGuildCmd, 0, 0))
					{
						lpGuild->SendAllMember(szPacket, sizeof(PktGuildCmd), CmdGuildCmd);
					}

					if (NULL != lpCharacter)
					{
						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
						if (NULL != lpDispatch)
						{
							GameClientSendPacket::SendCharMyGuildInfo(lpDispatch->GetSendStream(), 
								lpGuild->GetGold(), lpGuild->GetRight(), static_cast<unsigned char>(dwReferenceID), wError);
						}
					
						// 길드가 변경 되었을때 파티 정보를 보냄 //
						CParty* pParty = lpCharacter->GetParty();

						if(pParty)
						{
							CCharacterParty* lpParty = static_cast<CCharacterParty*>(pParty);

							lpParty->SendPartyMemberDataToDBAgent(dwSenderID, dwGID, 0, 0, 0, lpCharacter->GetCharacterName(), PktDD::SCmdChangeGuildPartyMem);

							// 친구 리스트에 길드 변경 메세지를 보낸다 //
							lpCharacter->FriendInfoUpdate(lpCharacter->GetUID(), lpCharacter->GetCID(), dwGID, lpCharacter->GetClass(), 
								lpCharacter->GetLevel(), CServerSetup::GetInstance().GetServerID());
						}
					}
				}
				else
				{
					if (NULL != lpCharacter)
					{
						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
						if (NULL != lpDispatch)
						{
							GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
								dwGID, dwSenderID, dwReferenceID, lpCharacter->GetCharacterName(), lpGuild->GetName(), wCmd, wError);
						}
					}
				}

				GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::RESULT, dwGID, dwSenderID, dwTitle, wError);
				break;
			}
			case PktGuildCmd::GC_TACTICS_TITLE:
			{
				if (PktBase::NO_SERVER_ERR == wError)
				{
					lpGuild->SetTacticsTitle(dwSenderID, static_cast<unsigned char>(dwReferenceID));					
				}
				else
				{
					CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderID);
					
					if(lpCharacter)
					{
						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
						
						if(lpDispatch)
						{
							GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
								dwGID, dwSenderID, dwReferenceID, lpCharacter->GetCharacterName(), lpGuild->GetName(), wCmd, wError);
						}
					}
				}

				GAMELOG::LogGuildMemberLevelAdjust(GAMELOG::sGuildLog::RESULT, dwGID, 0, dwSenderID, dwReferenceID, wError);					

				break;
			}
			case PktGuildCmd::GC_TACTICS_REQUEST:
			{
				CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderID);

				if(lpCharacter)
				{
					CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

					if(lpDispatch)
					{
						GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
							dwGID, dwSenderID, dwReferenceID, lpCharacter->GetCharacterName(), lpGuild->GetName(), wCmd, wError);
					}
				}
				break;
			}
			case PktGuildCmd::GC_TACTICS:
			case PktGuildCmd::GC_TACTICS_JOIN:
			{
				unsigned long dwTitle = JOIN_WAIT;

				CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderID);
				
				if(PktBase::NO_SERVER_ERR == wError)
				{
					MemberInfo memberInfo = *(reinterpret_cast<MemberInfo *>(lpPktGC + 1));
					lpGuild->JoinMember(memberInfo);

					dwTitle = memberInfo.m_MemberListInfo.m_cTitle;

					char* szPacket = reinterpret_cast<char *>(lpPktGC);
					if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildCmd), CmdGuildCmd, 0, 0))
					{
						lpGuild->SendAllMember(szPacket, sizeof(PktGuildCmd), CmdGuildCmd);
					}

					if (lpCharacter)
					{
						lpCharacter->SetTacticsFlag(TACTICS_WAIT);

						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

						if (lpDispatch)
						{
							GameClientSendPacket::SendCharMyGuildInfo(lpDispatch->GetSendStream(), 
								lpGuild->GetGold(), lpGuild->GetRight(), static_cast<unsigned char>(dwReferenceID), wError);
						}
					}
				}
				else
				{
					if (lpCharacter)
					{
						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
						
						if (lpDispatch)
						{
							GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
								dwGID, dwSenderID, dwReferenceID, lpCharacter->GetCharacterName(), lpGuild->GetName(), wCmd, wError);
						}
					}
				}
				break;
			}

			case PktGuildCmd::GC_TACTICS_KICK:
			case PktGuildCmd::GC_TACTICS_LEAVE:
			{				
				lpGuild->LeaveTacticsMember(dwSenderID, dwSenderID, wCmd);
				
				GAMELOG::LogGuildLeave(GAMELOG::sGuildLog::RESULT, lpGuild->GetGID(), 0, dwSenderID, wError);
					
				break;
			}

			// Send : 탈퇴한 멤버 (킥이나 탈퇴 대기 후 시간이 지난 경우)
			// Reference : None
			case PktGuildCmd::GC_KICK:
			{
				// 길드가 변경 되었을때 파티 정보를 보냄 //
				CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderID);
				if (NULL != lpCharacter)
				{
					CParty* pParty = lpCharacter->GetParty();
					if (NULL != pParty)
					{
						CCharacterParty* lpParty = static_cast<CCharacterParty*>(pParty);
						lpParty->SendPartyMemberDataToDBAgent(dwSenderID, 0, 0, 0, 0, 
							lpCharacter->GetCharacterName(), PktDD::SCmdChangeGuildPartyMem);

						// 친구 리스트에 길드 변경 메세지를 보낸다 //
						lpCharacter->FriendInfoUpdate(lpCharacter->GetUID(), lpCharacter->GetCID(), 0, lpCharacter->GetClass(), 
							lpCharacter->GetLevel(), CServerSetup::GetInstance().GetServerID());
					}
				}

				lpGuild->LeaveMember(dwSenderID);
				GAMELOG::LogGuildLeave(GAMELOG::sGuildLog::RESULT, 
					lpGuild->GetGID(), 0, dwSenderID, wError);
				break;
			}

			// Send : 직위 변경되는 멤버
			// Reference : 변경 후의 직위
			case PktGuildCmd::GC_SETUP_TITLE:
			{
				if (PktBase::NO_SERVER_ERR == wError)
				{
					lpGuild->SetTitle(dwSenderID, static_cast<unsigned char>(dwReferenceID));
				}
				else
				{
					CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderID);
					if (NULL != lpCharacter)
					{
						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
						if (NULL != lpDispatch)
						{
							GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
								dwGID, dwSenderID, dwReferenceID, lpCharacter->GetCharacterName(), lpGuild->GetName(), wCmd, wError);
						}
					}
				}

				GAMELOG::LogGuildMemberLevelAdjust(GAMELOG::sGuildLog::RESULT, 
					dwGID, 0, dwSenderID, dwReferenceID, wError);
				break;
			}

			// Send : 로그인/아웃한 멤버
			// Reference : 서버 아이디
			case PktGuildCmd::GC_LOGINOUT:
			{
				lpGuild->LogInOutMember(dwSenderID, dwReferenceID);

				char* szPacket = reinterpret_cast<char *>(lpPktGC);
				if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildCmd), CmdGuildCmd, 0, 0))
				{
					lpGuild->SendAllMember(szPacket, sizeof(PktGuildCmd), CmdGuildCmd);
				}
/*
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
						dwGID, dwSenderID, dwReferenceID, lpCharacter->GetCharacterName(), lpGuild->GetName(), wCmd, wError);
				}
*/
				break;
			}
		}
	}

	return true;
}

bool DBAgentPacketParse::ParseGuildMark(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildMark), DBAgentDispatch)

	PktGuildMark* lpPktGM = static_cast<PktGuildMark *>(lpPktBase);

	unsigned long dwCID = lpPktGM->m_dwCID;
	unsigned long dwGID = lpPktGM->m_dwGID;

	unsigned long dwGold = lpPktGM->m_dwGold;

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL != lpGuild)
	{
		unsigned long dwPreChangeGold = lpGuild->GetGold();
		lpGuild->SetMark(dwCID, lpPktGM->m_szMark, dwGold);

		GAMELOG::LogGuildMarkChange(GAMELOG::sGuildLog::RESULT, dwGID, 
            lpGuild->GetMaster().m_dwCID, dwPreChangeGold, lpGuild->GetGID(), 
			lpPktGM->m_szMark, MAX_MARK_SIZE, 0);
	}

	return true;
}

bool DBAgentPacketParse::ParseGuildLevel(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildLevel), DBAgentDispatch)

	PktGuildLevel* lpPktGL = static_cast<PktGuildLevel *>(lpPktBase);

	unsigned long dwGID = lpPktGL->m_dwUID;
	unsigned char cLevel = lpPktGL->m_cLevel;
	unsigned long dwGold = lpPktGL->m_dwGold;

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL != lpGuild)
	{
		unsigned long dwPreChangeGold = lpGuild->GetGold();
		lpGuild->SetLevel(cLevel, dwGold);

		GAMELOG::LogGuildLevel(GAMELOG::sGuildLog::RESULT, dwGID, 
            lpGuild->GetMaster().m_dwCID, cLevel, dwPreChangeGold, lpGuild->GetGold(), 0);
	}

	return true;
}

bool DBAgentPacketParse::ParseGuildRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildRight), DBAgentDispatch)

	PktGuildRight* lpPktGR = static_cast<PktGuildRight *>(lpPktBase);

	unsigned long dwGID = lpPktGR->m_dwUID;
	GuildRight guildRight = lpPktGR->m_GuildRight;

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL != lpGuild)
	{
		lpGuild->SetRight(guildRight);

        GAMELOG::LogGuildRightsChange(GAMELOG::sGuildLog::RESULT, lpGuild->GetGID(),
			0, reinterpret_cast<const char*>(guildRight.m_aryRight), MAX_RIGHT_SIZE, 0);
	}

	return true;
}

bool DBAgentPacketParse::ParseGuildRelation(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildRelation), DBAgentDispatch)

	PktGuildRelation* lpPktGR = static_cast<PktGuildRelation *>(lpPktBase);

	unsigned long dwCID = lpPktGR->m_dwCID;
	unsigned long dwGID = lpPktGR->m_dwGID;
	unsigned long dwTargetGID = lpPktGR->m_dwTargetGID;
	unsigned long dwValue = lpPktGR->m_dwValue;
	unsigned char cSubCmd = lpPktGR->m_cSubCmd;
	unsigned short wError = lpPktGR->GetError();

	if (wError == PktBase::NO_SERVER_ERR)
	{
		switch (cSubCmd)
		{
			case PktGuildRelation::GR_HOSTILITY:
			{
				// A->B를 적대
				CGuildMgr::GetInstance().SetRelation(dwGID, dwTargetGID, HOSTILITY);
				break;
			}

			case PktGuildRelation::GR_PEACE:
			{
				// A->B를 적대품
				CGuildMgr::GetInstance().SetRelation(dwGID, dwTargetGID, NEUTRALITY);
				break;
			}

			case PktGuildRelation::GR_HOSTILITYDEL:
			{
				// B가 A의 평화를 받아줌.
				CGuildMgr::GetInstance().SetRelation(dwGID, dwTargetGID, TARGET_NEUTRALITY);
				break;
			}

			case PktGuildRelation::GR_ALERT:
			{
				CGuildMgr::GetInstance().SetRelation(dwGID, dwTargetGID, ALERT_HOSTILITY);

				// dwTargetGID 의 길드 마스터에게 카운터 적대 선언을 할것인지 인증
				CGuild* lpTargetGuild = CGuildMgr::GetInstance().GetGuild(dwTargetGID);
				if (0 != lpTargetGuild)
				{
					unsigned long dwMasterCID = lpTargetGuild->GetMaster().m_dwCID;
					CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter(dwMasterCID);
					if (0 != lpMaster)
					{
						CGameClientDispatch* lpDispatch = lpMaster->GetDispatcher();
						if (NULL != lpDispatch)
						{
							return GameClientSendPacket::SendCharGuildRelation(lpDispatch->GetSendStream(), 
								dwCID, dwGID, dwTargetGID, 0, PktGuildRelation::GR_REQUEST, PktGuildRelation::FAIL_REQUEST_COUNTER);
						}
					}
				}

				break;
			}

			case PktGuildRelation::GR_COUNTER:
			{
				CGuildMgr::GetInstance().SetRelation(dwGID, dwTargetGID, COUNTER_HOSTILITY);
				break;
			}

			case PktGuildRelation::GR_REQUEST:
			{
				CGuildMgr::GetInstance().SetRelation(dwGID, dwValue, NEUTRALITY);
				CGuildMgr::GetInstance().SetRelation(dwGID, dwTargetGID, HOSTILITY);
				break;
			}
		}
	}
	else
	{
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
		if (NULL != lpCharacter)
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				return GameClientSendPacket::SendCharGuildRelation(lpDispatch->GetSendStream(), 
					dwCID, dwGID, dwTargetGID, dwValue, cSubCmd, PktGuildRelation::SERVER_ERROR);
			}
		}
	}

	return true;
}


bool DBAgentPacketParse::ParseGuildInclination(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
// 더이상 길드 국적은 변경할 수 없습니다. (2004-11-22 by 로딘)
/*
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildInclination), DBAgentDispatch)

	PktGuildInclination* lpPktGI = static_cast<PktGuildInclination *>(lpPktBase);

	unsigned long dwGID = lpPktGI->m_dwUID;
	unsigned char cInclination = lpPktGI->m_cInclination;

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL != lpGuild)
	{
		lpGuild->SetInclination(cInclination);

		// TODO : 게임 로그를 넣어주세요.
	}
*/

	return true;
}

bool DBAgentPacketParse::ParseGuildDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktGuildDB* lpPktGuildDB = static_cast<PktGuildDB *>(lpPktBase);

	bool bStartFlag = lpPktGuildDB->m_bStartFlag;

	unsigned long dwGID = lpPktGuildDB->m_dwGID;

	unsigned char cTotalMemberNum = lpPktGuildDB->m_cTotalMemberNum;
	unsigned char cRelationByMineNum = lpPktGuildDB->m_cRelationByMineNum;
	unsigned char cRelationByTargetNum = lpPktGuildDB->m_cRelationByTargetNum;
	unsigned short wSize = lpPktGuildDB->m_wSize;

	if (true == bStartFlag)
	{
		CGuildMgr::GetInstance().Destroy();
	}

	CGuildMgr::GetInstance().SerializeIn(reinterpret_cast<char *>(lpPktGuildDB + 1), wSize, 
		cTotalMemberNum, cRelationByMineNum, cRelationByTargetNum);

	return true;
}

bool DBAgentPacketParse::ParseGuildSafe(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildSafeAck), DBAgentDispatch)

	PktGuildSafeAck* lpPktGSAck = static_cast<PktGuildSafeAck *>(lpPktBase);

	unsigned long dwCID = lpPktGSAck->m_dwCID;
	unsigned long dwGID = lpPktGSAck->m_dwGID;
	unsigned long dwSafeGold = lpPktGSAck->m_dwSafeGold;
	unsigned long dwCharGold = lpPktGSAck->m_dwCharGold;
	unsigned char cCmd = lpPktGSAck->m_cCmd;

	unsigned short wError = lpPktGSAck->GetError();

	unsigned long dwPreChangeGold = 0;
	unsigned long dwPostChangeGold = 0;

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);

	bool bResult = true;

	if (PktBase::NO_SERVER_ERR == wError)
	{
		if (NULL != lpGuild)
		{
			dwPreChangeGold = lpGuild->GetGold();

			switch (cCmd)
			{
				case PktGuildSafe::SC_WITHDRAWAL:
				{
					lpGuild->DeductGold(dwSafeGold);
					if (NULL != lpCharacter)
					{
						lpCharacter->AddGold(dwCharGold, false);
					}
					break;
				}

				case PktGuildSafe::SC_DEPOSIT:
				{
					lpGuild->AddGold(dwSafeGold);
					if (NULL != lpCharacter)
					{
						lpCharacter->DeductGold(dwCharGold, false);
					}
					break;
				}

				case PktGuildSafe::SC_BONUS:
				{
					lpGuild->AddGold(dwSafeGold);
					break;
				}

				case PktGuildSafe::SC_RELEASE:
				{
					lpGuild->DeductGold(dwSafeGold);
					lpGuild->ReleaseGold(dwCharGold);
					break;
				}
			}

			// edith 2008.03.15 길드 메소드 초기화
			if (NULL != lpCharacter)
				lpCharacter->SetGuildSafe(-1);

			if (NULL != lpGuild)
				lpGuild->SetGuildSafe(-1);

			dwPostChangeGold = lpGuild->GetGold();
			bResult = lpGuild->SendGuildSafe(dwCID, lpPktGSAck->m_szCharName, cCmd);
		}

		// edith 2008.03.15 길드 메소드 초기화
		if (NULL != lpCharacter)
			lpCharacter->SetGuildSafe(-1);

		if (NULL != lpGuild)
			lpGuild->SetGuildSafe(-1);

	}
	else
	{
		// edith 2008.03.15 길드 메소드 초기화
		if (NULL != lpCharacter)
			lpCharacter->SetGuildSafe(-1);

		if (NULL != lpGuild)
			lpGuild->SetGuildSafe(-1);

		if (NULL != lpCharacter)
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				bResult = GameClientSendPacket::SendCharGuildSafe(lpDispatch->GetSendStream(), 
					dwCID, dwGID, dwSafeGold, dwCharGold, cCmd, lpPktGSAck->m_szCharName, wError);
			}
		}
	}

	GAMELOG::LogGuildStoreGoldChange(GAMELOG::sGuildLog::RESULT, 
		dwGID, dwCID, cCmd, dwPreChangeGold, dwPostChangeGold, wError);

	return bResult;
}

bool DBAgentPacketParse::ParseGuildMemberInfoUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktGuildMemberInfoUpdate* lpPktGMIU = static_cast<PktGuildMemberInfoUpdate *>(lpPktBase);

	unsigned long dwGID = lpPktGMIU->m_dwGID;
	unsigned long dwCID = lpPktGMIU->m_dwCID;

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL != lpGuild)
	{
		lpGuild->UpdateMemberInfo(dwCID, lpPktGMIU->m_MemberListInfo, lpPktGMIU->m_MemberDetailInfo);
	}

	return true;
}

bool DBAgentPacketParse::ParseGuildMemberFameUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktGuildMemberFameUpdate* lpPktGMFU = static_cast<PktGuildMemberFameUpdate *>(lpPktBase);

	unsigned long	dwOurGID = lpPktGMFU->m_dwOurGID;
	unsigned long	dwEnemyGID = lpPktGMFU->m_dwEnemyGID;
	unsigned long	dwCID = lpPktGMFU->m_dwCID;
	unsigned char	cType = lpPktGMFU->m_cType;
	int				nAddFame = lpPktGMFU->m_nAddFame * static_cast<int>(CServerSetup::GetInstance().GetFameDefault() / 100.0f);

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwOurGID);
	if (NULL != lpGuild)
	{
		GAMELOG::ERRType eReturnCode = 0;
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);

		if (lpCharacter)
		{
			const unsigned long dwPrevFame = lpCharacter->GetFame();

			if (TYPE_VALUE == cType)
			{
				if (nAddFame < 0 && static_cast<unsigned long>(abs(nAddFame)) > lpCharacter->GetFame()) 
				{
					lpCharacter->SetFame(0);
				}
				else 
				{
					lpCharacter->SetFame(lpCharacter->GetFame() + nAddFame);
				}
			}
			else if (TYPE_HALF == cType)
			{
				lpCharacter->SetFame(lpCharacter->GetFame() / 2);
			}

			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpCharacter, 
					"", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);
			}

			if(nAddFame < 0)
				GAMELOG::LogFameLoseCamp(*lpCharacter, dwEnemyGID, dwPrevFame, lpCharacter->GetFame(), eReturnCode);
			else
				GAMELOG::LogFameGetCamp(*lpCharacter, dwEnemyGID, dwPrevFame, lpCharacter->GetFame(), eReturnCode);
		}
	}

	return true;
}

bool DBAgentPacketParse::ParseGuildMemberGoldUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktGuildMemberGoldUpdate* lpPktGMGU = static_cast<PktGuildMemberGoldUpdate *>(lpPktBase);

	unsigned long	dwGID	= lpPktGMGU->m_dwGID;
	unsigned long	dwCID	= lpPktGMGU->m_dwCID;
	unsigned long	dwGold	= lpPktGMGU->m_dwGold;

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL != lpGuild)
	{
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
		if (lpCharacter)
		{
			unsigned long dwSrcGold = lpCharacter->GetGold();
			lpCharacter->AddGold(dwGold, true);
			GAMELOG::LogTakeGold(*lpCharacter, dwSrcGold, lpCharacter->GetGold(), dwGold, 
				TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::STARTERKIT_AWARD, 0);
		}
	}

	return true;
}