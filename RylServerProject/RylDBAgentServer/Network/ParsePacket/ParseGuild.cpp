#include "stdafx.h"
#include "ParseGuild.h"

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/GameDispatch.h>
#include <Network/SendPacket/SendGuild.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <DB/DBComponent.h>
#include <DB/GuildDBComponent.h>

#include <Log/ServerLog.h>

#include <DataStorage/SessionDataMgr.h>
#include <DataStorage/CharacterData.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeDBMgr.h>
#include <DB/GameDBComponent.h>

namespace DBAgent
{
namespace ParseGuild
{

bool CreateGuild(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktCreateGuild* lpPktCreateGuild = static_cast<PktCreateGuild*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    CGuildDB* lpGuild = Guild::CGuildDBMgr::GetInstance().CreateGuild(
        lpPktCreateGuild->m_dwCID, lpPktCreateGuild->m_cInclination, 
        lpPktCreateGuild->m_szGuildName, &wError);

    PktCreateGuild pktGCAck;
    memset(&pktGCAck, 0, sizeof(PktCreateGuild));

    pktGCAck.m_dwCID = lpPktCreateGuild->m_dwCID;
    pktGCAck.m_dwGID = (NULL != lpGuild) ? lpGuild->GetGID() : 0;
    pktGCAck.m_cInclination = lpPktCreateGuild->m_cInclination;
    strncpy(pktGCAck.m_szGuildName, lpPktCreateGuild->m_szGuildName, Guild::MAX_GUILD_NAME_LEN);

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGCAck), 
        sizeof(PktCreateGuild), CmdCreateGuild, 0, wError))
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
			using namespace DBAgent::DataStorage;

			CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(lpPktCreateGuild->m_dwCID);
			
			if(lpSessionData)
			{
				CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();

				if(lpCharacterData)
				{
					// edith 2008.11.24 길드생성시 전쟁 자동참가
					lpCharacterData->SetGuildWarFlag(Creature::WAR_ON);

					PktWarOnOff* lpPktWarOnOffAck = reinterpret_cast<PktWarOnOff*>(SendStream.GetBuffer(sizeof(PktWarOnOff)));

					if (lpPktWarOnOffAck)
					{
						lpPktWarOnOffAck->m_dwCID	= lpCharacterData->GetCID();
						lpPktWarOnOffAck->m_cType	= GameTime::GUILD;
						lpPktWarOnOffAck->m_cFlag	= Creature::WAR_ON;

						SendStream.WrapHeader(sizeof(PktWarOnOff), CmdWarOnOff, 0, 0);
					}
				}
			}

            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGCAck), sizeof(PktCreateGuild), CmdCreateGuild));
        }
        else
        {
            SendStream.PutBuffer(reinterpret_cast<char*>(&pktGCAck), 
                sizeof(PktCreateGuild), CmdCreateGuild);
        }

        return true;
    }
    return false;
}



bool GuildCmd(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktGuildCmd* lpPktGuildCmd = static_cast<PktGuildCmd*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    unsigned long	dwGID			= lpPktGuildCmd->m_dwGID;
    unsigned long	dwSenderID		= lpPktGuildCmd->m_dwSenderID;	    
    unsigned long	dwReferenceID	= lpPktGuildCmd->m_dwReferenceID;	
    unsigned short	wCmd			= lpPktGuildCmd->m_wCmd;             

    using namespace Guild;

    CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));
    if (0 == lpGuild)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        switch (wCmd)
        {
			case PktGuildCmd::GC_JOIN:
            {
                // 가입한 멤버 정보를 뒤에 붙여 다른 크기의 패킷을 만들기 때문에 함수 내에서 독자적으로 패킷을 보낸다.
                lpGuild->SendJoinMember(&SendStream, dwSenderID, static_cast<unsigned char>(dwReferenceID));

                return true;
            }
			case PktGuildCmd::GC_TACTICS:
			{
				// 용병 가입 신청.
				lpGuild->SendJoinTacticsMember(&SendStream, dwSenderID, static_cast<unsigned char>(dwReferenceID), PktGuildCmd::GC_TACTICS);

				return true;
			}
			case PktGuildCmd::GC_TACTICS_JOIN:
			{
				// 용병 가입 신청.
				lpGuild->SendJoinTacticsMember(&SendStream, dwSenderID, static_cast<unsigned char>(dwReferenceID), PktGuildCmd::GC_TACTICS_JOIN);

				return true;
			}
			case PktGuildCmd::GC_TACTICS_TITLE:
			case PktGuildCmd::GC_TACTICS_REQUEST:
			{
				using namespace DBAgent::DataStorage;

				// 용병 허가.
				unsigned char cTitle = static_cast<unsigned char>(dwReferenceID);

				CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwSenderID);

				if(wCmd==PktGuildCmd::GC_TACTICS_REQUEST)
					goto lb_request;

				if(CGameTimeDBMgr::GetInstance().IsGuildWarTime())
				{
					if(lpSessionData)
					{
						CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();

						if(lpCharacterData)
						{
							SendPacket::GuildCmd(&SendStream, dwGID, dwSenderID, COMMON, PktGuildCmd::GC_TACTICS_REQUEST, NULL, NULL, wError);
						}
						else
						{
							goto lb_error;
						}
					}
					else
					{
lb_error:					
						PktGuildCmd pktGuildCmd;
						memset(&pktGuildCmd, 0, sizeof(PktGuildCmd));
						strcpy(pktGuildCmd.m_szSenderName, lpPktGuildCmd->m_szSenderName);

						pktGuildCmd.m_dwGID = dwGID;
						pktGuildCmd.m_dwSenderID = dwSenderID;
						pktGuildCmd.m_dwReferenceID = COMMON;
						pktGuildCmd.m_wCmd = PktGuildCmd::GC_TACTICS_TITLE;											
						SendStream.WrapCompress(reinterpret_cast<char*>(&pktGuildCmd), sizeof(PktGuildCmd), CmdGuildCmd, 0, PktGuildCmd::FAIL_TACTICS_LOGOUT);
					}
				}
				else
				{
lb_request:
					lpGuild->SetTacticsMember(dwSenderID, Guild::TACTICS);				
					lpGuild->SetTitle(dwSenderID, COMMON);

					SendPacket::GuildCmd(&SendStream, dwGID, dwSenderID, COMMON, PktGuildCmd::GC_TACTICS_TITLE, NULL, NULL, wError);

					DBComponent::GuildDB::UpdateGuildMemberTactics(CDBSingleObject::GetInstance(), lpGuild->GetGID(), dwSenderID, Guild::TACTICS);					
					
					if(lpSessionData)
					{
						CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();

						if(lpCharacterData)
						{
							lpCharacterData->SetGuildWarFlag(Creature::WAR_ON);

							PktWarOnOff* lpPktWarOnOffAck = reinterpret_cast<PktWarOnOff*>(SendStream.GetBuffer(sizeof(PktWarOnOff)));

							if (lpPktWarOnOffAck)
							{
								lpPktWarOnOffAck->m_dwCID	= lpCharacterData->GetCID();
								lpPktWarOnOffAck->m_cType	= GameTime::GUILD;
								lpPktWarOnOffAck->m_cFlag	= Creature::WAR_ON;

								SendStream.WrapHeader(sizeof(PktWarOnOff), CmdWarOnOff, 0, 0);
							}
						}
					}
					else
					{
						DBComponent::GameDB::UpdateGuildWarFlag(CDBSingleObject::GetInstance(), dwSenderID, Creature::WAR_ON);
					}
				}
				
				return true;
			}

			case PktGuildCmd::GC_TACTICS_KICK:
			case PktGuildCmd::GC_TACTICS_LEAVE:
			{
				// 용병 퇴출 및 탈퇴.
				if (false == lpGuild->LeaveMember(dwSenderID))
				{
					wError = PktBase::SERVER_ERROR;
				}

				SendPacket::GuildCmd(&SendStream, dwGID, dwSenderID, dwReferenceID, wCmd, NULL, NULL, wError);

				using namespace DBAgent::DataStorage;

				CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwSenderID);

				if(lpSessionData)
				{
					CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();

					if(lpCharacterData)
					{
						lpCharacterData->SetGuildWarFlag(Creature::WAR_OFF);

						PktWarOnOff* lpPktWarOnOffAck = reinterpret_cast<PktWarOnOff*>(SendStream.GetBuffer(sizeof(PktWarOnOff)));

						if (lpPktWarOnOffAck)
						{
							lpPktWarOnOffAck->m_dwCID	= lpCharacterData->GetCID();
							lpPktWarOnOffAck->m_cType	= GameTime::GUILD;
							lpPktWarOnOffAck->m_cFlag	= Creature::WAR_OFF;

							SendStream.WrapHeader(sizeof(PktWarOnOff), CmdWarOnOff, 0, 0);
						}
					}
				}
				else
				{
					DBComponent::GameDB::UpdateGuildWarFlag(CDBSingleObject::GetInstance(), dwSenderID, Creature::WAR_OFF);					
				}

				return true;
			}

	        case PktGuildCmd::GC_SETUP_TITLE:
            {
                unsigned char cTitle = static_cast<unsigned char>(dwReferenceID);

                // 길드 마스터 탈퇴
                if (dwSenderID == lpGuild->GetMaster().m_dwCID)
                {
					unsigned long dwNewMaster = lpGuild->SetNewMaster();
                    if (0 == dwNewMaster)
                    {
                        DETLOG1(g_Log, "GID:%10u 길드가 해체되었습니다. - 길드마스터 탈퇴", lpGuild->GetGID());
                        CGuildDBMgr::GetInstance().DissolveGuild(
                            lpGuild->GetGID(), PktCreateGuild::NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_OUT);
                        break;
                    }

					SendPacket::GuildCmd(&SendStream, dwGID, dwNewMaster, MASTER, PktGuildCmd::GC_SETUP_TITLE, NULL, NULL, wError);

                    DETLOG3(g_Log, "GID:%10u 길드마스터(이전:%10u, 현재:%10u)가 변경되었습니다. - 길드마스터 탈퇴", 
                        lpGuild->GetGID(), dwSenderID, dwNewMaster);
                }

                // 길드 마스터 권한 양도
                if (MASTER == cTitle)
                {
                    MemberInfo PreMasterInfo = lpGuild->GetMaster();
                    lpGuild->SetTitle(PreMasterInfo.m_dwCID, COMMON);

					SendPacket::GuildCmd(&SendStream, dwGID, PreMasterInfo.m_dwCID, COMMON, PktGuildCmd::GC_SETUP_TITLE, NULL, NULL, wError);

					DETLOG3(g_Log, "GID:%10u 길드마스터(이전:%10u, 현재:%10u)가 변경되었습니다. - 길드마스터 권한 양도", 
                        lpGuild->GetGID(), PreMasterInfo.m_dwCID, dwSenderID);
                }

                lpGuild->SetTitle(dwSenderID, cTitle);

				if(cTitle==COMMON)
				{
					using namespace DBAgent::DataStorage;

					// edith 2008.03.15 일반 길드원으로 타이틀을 변경하는데 SenderID가 아닌 dwGID 에서 얻어왔다 잘못됐다.
					CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwSenderID);
//					CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwGID);

					if(lpSessionData)
					{
						CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();

						if(lpCharacterData)
						{
							// edith 2008.11.24 길드마스터가 될시에 전쟁 자동참가
							// edith 2009.05.20 일반 길드원이 될때 전쟁자동참여 OFF 로 설정.
							lpCharacterData->SetGuildWarFlag(Creature::WAR_OFF);

							PktWarOnOff* lpPktWarOnOffAck = reinterpret_cast<PktWarOnOff*>(SendStream.GetBuffer(sizeof(PktWarOnOff)));

							if (lpPktWarOnOffAck)
							{
								lpPktWarOnOffAck->m_dwCID	= lpCharacterData->GetCID();
								lpPktWarOnOffAck->m_cType	= GameTime::GUILD;
								lpPktWarOnOffAck->m_cFlag	= Creature::WAR_OFF;

								SendStream.WrapHeader(sizeof(PktWarOnOff), CmdWarOnOff, 0, 0);
							}
						}
					}
				}
                break;
            }

		    case PktGuildCmd::GC_KICK:
            {
                // 길드 마스터의 경우 삭제시 킥 명령이 온다.
                if (dwSenderID == lpGuild->GetMaster().m_dwCID)
                {
					unsigned long dwNewMaster = lpGuild->SetNewMaster();
					if (0 == dwNewMaster)
                    {
                        DETLOG1(g_Log, "GID:%10u 길드가 해체되었습니다. - 길드마스터 삭제", lpGuild->GetGID());
                        CGuildDBMgr::GetInstance().DissolveGuild(lpGuild->GetGID(), 
                            PktCreateGuild::NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_DELETED);
                        break;
                    }

					SendPacket::GuildCmd(&SendStream, dwGID, dwNewMaster, MASTER, PktGuildCmd::GC_SETUP_TITLE, NULL, NULL, wError);

                    DETLOG3(g_Log, "GID:%10u 길드마스터(이전:%10u, 현재:%10u)가 변경되었습니다. - 길드마스터 삭제", 
                        lpGuild->GetGID(), dwSenderID, dwNewMaster);
                }

                if (false == lpGuild->LeaveMember(dwSenderID))
                {
                    wError = PktBase::SERVER_ERROR;
                }

				using namespace DBAgent::DataStorage;

				CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwSenderID);

				if(lpSessionData)
				{
					CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();

					if(lpCharacterData)
					{
						lpCharacterData->SetGuildWarFlag(Creature::WAR_OFF);

						PktWarOnOff* lpPktWarOnOffAck = reinterpret_cast<PktWarOnOff*>(SendStream.GetBuffer(sizeof(PktWarOnOff)));

						if (lpPktWarOnOffAck)
						{
							lpPktWarOnOffAck->m_dwCID	= lpCharacterData->GetCID();
							lpPktWarOnOffAck->m_cType	= GameTime::GUILD;
							lpPktWarOnOffAck->m_cFlag	= Creature::WAR_OFF;

							SendStream.WrapHeader(sizeof(PktWarOnOff), CmdWarOnOff, 0, 0);
						}
					}
				}
				else
				{
					DBComponent::GameDB::UpdateGuildWarFlag(CDBSingleObject::GetInstance(), dwSenderID, Creature::WAR_OFF);					
				}

                break;
            }
        }
    }

    return SendPacket::GuildCmd(&SendStream, dwGID, dwSenderID, dwReferenceID, wCmd, NULL, NULL, wError);
}

bool GuildMark(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktGuildMark* lpPktGuildMark = static_cast<PktGuildMark*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned long dwCID = lpPktGuildMark->m_dwCID;
	unsigned long dwGID	= lpPktGuildMark->m_dwGID;

    unsigned long dwGold = 0;

    CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));
    if (0 == lpGuild)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        lpGuild->SetMark(lpPktGuildMark->m_szMark);
        dwGold = lpGuild->GetGold();
    }

    PktGuildMark pktGuildMark;
    memset(&pktGuildMark, 0, sizeof(PktGuildMark));

	pktGuildMark.m_dwCID = dwCID;    
    pktGuildMark.m_dwGID = dwGID;    
    pktGuildMark.m_dwGold = dwGold;
    memcpy(pktGuildMark.m_szMark, lpPktGuildMark->m_szMark, MAX_MARK_SIZE);

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildMark), 
        sizeof(PktGuildMark), CmdGuildMark, 0, wError))
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGuildMark), sizeof(PktGuildMark), CmdGuildMark));
        }
        else
        {
            SendStream.PutBuffer(reinterpret_cast<char*>(&pktGuildMark), 
                sizeof(PktGuildMark), CmdGuildMark);
        }

        return true;
    }

    return false;    
}


bool GuildLevel(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktGuildLevel* lpPktGuildLevel = static_cast<PktGuildLevel*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    unsigned long dwGID = lpPktGuildLevel->m_dwUID;
    unsigned char cLevel = lpPktGuildLevel->m_cLevel;	
    unsigned long dwGold = 0;

    using namespace Guild;

    CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));
    if (NULL == lpGuild)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        lpGuild->SetLevel(cLevel);
        dwGold = lpGuild->GetGold();
    }

    PktGuildLevel pktGuildLevel;
    memset(&pktGuildLevel, 0, sizeof(PktGuildLevel));

    pktGuildLevel.m_dwUID = dwGID;
    pktGuildLevel.m_cLevel = cLevel;
    pktGuildLevel.m_dwGold = dwGold;

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildLevel),
        sizeof(PktGuildLevel), CmdGuildLevel, 0, wError))
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGuildLevel), sizeof(PktGuildLevel), CmdGuildLevel));
        }
        else
        {
            SendStream.PutBuffer(reinterpret_cast<char*>(&pktGuildLevel),
                sizeof(PktGuildLevel), CmdGuildLevel);            
        }

        return true;
    }

    return false;
}


bool GuildRelation(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktGuildRelation* lpPktGuildRelation = static_cast<PktGuildRelation*>(lpPktBase);

    unsigned short  wError = PktBase::NO_SERVER_ERR;

    unsigned long	dwCID			= lpPktGuildRelation->m_dwCID;
	unsigned long	dwGID			= lpPktGuildRelation->m_dwGID;
	unsigned long	dwTargetGID		= lpPktGuildRelation->m_dwTargetGID;
	unsigned long	dwValue			= lpPktGuildRelation->m_dwValue;
    char			cSubCmd			= lpPktGuildRelation->m_cSubCmd;

    CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));
    CGuildDB* lpTargetGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwTargetGID));

    if (NULL == lpGuild || NULL == lpTargetGuild)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
		switch (cSubCmd)
		{
			case PktGuildRelation::GR_HOSTILITY:
			{
				if (!CGuildDBMgr::GetInstance().SetRelation(dwGID, dwTargetGID, Guild::HOSTILITY))
				{
					wError = PktGuildRelation::SERVER_ERROR;
				}
				break;
			}

			case PktGuildRelation::GR_PEACE:
			{
				if (!CGuildDBMgr::GetInstance().SetRelation(dwGID, dwTargetGID, Guild::NEUTRALITY))
				{
					wError = PktGuildRelation::SERVER_ERROR;
				}
				break;
			}

			case PktGuildRelation::GR_HOSTILITYDEL:
			{
				// 내가 TargetGuild에게 평화허락을 한다.
				if (!CGuildDBMgr::GetInstance().SetRelation(dwGID, dwTargetGID, Guild::TARGET_NEUTRALITY))
				{
					wError = PktGuildRelation::SERVER_ERROR;
				}
				break;
			}

			case PktGuildRelation::GR_ALERT:
			{
				if (!CGuildDBMgr::GetInstance().SetRelation(dwGID, dwTargetGID, Guild::ALERT_HOSTILITY))
				{
					wError = PktGuildRelation::SERVER_ERROR;
				}
				break;
			}

			case PktGuildRelation::GR_COUNTER:
			{
				if (!CGuildDBMgr::GetInstance().SetRelation(dwGID, dwTargetGID, Guild::COUNTER_HOSTILITY))
				{
					wError = PktGuildRelation::SERVER_ERROR;
				}
				break;
			}

			case PktGuildRelation::GR_REQUEST:
			{
				CGuildDB* lpHostilityGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwValue));
				if (NULL == lpHostilityGuild)
				{
					wError = PktGuildRelation::SERVER_ERROR;
				}
				else
				{
					if (!CGuildDBMgr::GetInstance().SetRelation(dwGID, dwValue, Guild::NEUTRALITY))
					{
						wError = PktGuildRelation::SERVER_ERROR;
					}
					else
					{
						if (!CGuildDBMgr::GetInstance().SetRelation(dwGID, dwTargetGID, Guild::HOSTILITY))
						{
							wError = PktGuildRelation::SERVER_ERROR;
						}
					}
				}

				break;
			}
		}
    }

    PktGuildRelation pktGuildRelation;
    memset(&pktGuildRelation, 0, sizeof(PktGuildRelation));

    pktGuildRelation.m_dwCID = dwCID;
    pktGuildRelation.m_dwGID = dwGID;
	pktGuildRelation.m_dwTargetGID = dwTargetGID;
	pktGuildRelation.m_dwValue = dwValue;
    pktGuildRelation.m_cSubCmd = cSubCmd;

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildRelation), 
        sizeof(PktGuildRelation), CmdGuildRelation, 0, wError))
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGuildRelation), 
                sizeof(PktGuildRelation), CmdGuildRelation));
        }
        else
        {
            SendStream.PutBuffer(reinterpret_cast<char*>(&pktGuildRelation), 
                sizeof(PktGuildRelation), CmdGuildRelation);
        }

        return true;
    }

    return false;
}

bool GuildInclination(CSendStream& SendStream, PktBase* lpPktBase)
{
// 더이상 길드 국적은 변경할 수 없습니다. (2004-11-22 by 로딘)
/*
    PktGuildInclination* lpPktInclination = static_cast<PktGuildInclination*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    unsigned long dwGID = lpPktInclination->m_dwUID;
    unsigned char cInclination = lpPktInclination->m_cInclination;	

    CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));
    if (NULL == lpGuild)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        lpGuild->SetInclination(cInclination);
    }

    PktGuildInclination pktGuildInclination;
    memset(&pktGuildInclination, 0, sizeof(PktGuildInclination));

    pktGuildInclination.m_dwUID = dwGID;
    pktGuildInclination.m_cInclination = cInclination;

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildInclination), 
        sizeof(PktGuildInclination), CmdGuildInclination, 0, wError))
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGuildInclination), 
                sizeof(PktGuildInclination), CmdGuildInclination));
        }
        else
        {
            SendStream.PutBuffer(reinterpret_cast<char*>(&pktGuildInclination), 
                sizeof(PktGuildInclination), CmdGuildInclination);
        }

        return true;
    }
*/

    return false;
}

bool SetGuildRight(CSendStream& SendStream, PktBase* lpPktBase)
{
    using namespace Guild;

    PktGuildRight* lpPktGuildRight = static_cast<PktGuildRight *>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    unsigned long   dwGID	= lpPktGuildRight->m_dwUID;
    GuildRight      guildRight = lpPktGuildRight->m_GuildRight;

    CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));
    if (NULL == lpGuild)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        lpGuild->SetRight(guildRight);
    }

    PktGuildRight pktGuildRight;
    memset(&pktGuildRight, 0, sizeof(PktGuildRight));

    pktGuildRight.m_dwUID = dwGID;
    pktGuildRight.m_GuildRight = guildRight;

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildRight), 
        sizeof(PktGuildRight), CmdGuildRight, 0, wError))
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGuildRight), sizeof(PktGuildRight), CmdGuildRight));
        }
        else
        {
            SendStream.PutBuffer(reinterpret_cast<char*>(&pktGuildRight), 
                sizeof(PktGuildRight), CmdGuildRight);
        }

        return true;
    }

    return false;    
}


bool GuildSafe(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktGuildSafeAck* lpPktGuildSafe = reinterpret_cast<PktGuildSafeAck *>(lpPktBase);

    unsigned wError = PktBase::NO_SERVER_ERR;

    unsigned long dwCID = lpPktGuildSafe->m_dwCID;
    unsigned long dwGID = lpPktGuildSafe->m_dwGID;

    unsigned long dwSafeGold = lpPktGuildSafe->m_dwSafeGold;
    unsigned long dwCharGold = lpPktGuildSafe->m_dwCharGold;

    unsigned char cCmd = lpPktGuildSafe->m_cCmd;

	using namespace DBAgent::DataStorage;
	CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);
	CCharacterData* lpCharacterData = NULL;

	if(lpSessionData)
		lpCharacterData = lpSessionData->GetCharacterData();

    CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));
    if (NULL == lpGuild)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        switch (cCmd)
        {
			// 출금
        case PktGuildSafe::SC_WITHDRAWAL:
            {			
                if (false == lpGuild->DeductGold(dwCharGold))
                {
                    wError = PktGuildSafe::FAIL_NOT_ENOUGH_MONEY;
                }
                dwSafeGold = dwCharGold;
                break;
            }

			// 입금
        case PktGuildSafe::SC_DEPOSIT:
            {
				// edith 2008.03.15 캐릭터의 골드를 뺀다.
				if(false == lpCharacterData->DeductGold(dwCharGold))
				{
                    wError = PktGuildSafe::FAIL_NOT_CHAR_MONEY;
				}
                else if (false == lpGuild->AddGold(dwCharGold))
                {
					// 되돌린다 금액을
					lpCharacterData->AddGold(dwCharGold);

                    wError = PktGuildSafe::FAIL_NOT_GUILD_DEPOSIT;
                }
                dwSafeGold = dwCharGold;
                break;
            }

			// 길드 보너스
		case PktGuildSafe::SC_BONUS:
			{
				if (false == lpGuild->AddGold(dwCharGold))
                {
                    wError = PktGuildSafe::FAIL_NOT_GUILD_DEPOSIT;
                }
                dwSafeGold = dwCharGold;
			}
			break;

			// 분배
        case PktGuildSafe::SC_RELEASE:
            {
                unsigned long dwPreSafeGold = lpGuild->GetGold();
                dwCharGold = lpGuild->ReleaseGold(dwCharGold);
                if (0 == dwCharGold)
                {
                    wError = PktGuildSafe::FAIL_NOT_ENOUGH_MONEY;
                    break;
                }
                dwSafeGold = dwPreSafeGold - lpGuild->GetGold();
                break;
            }
		default:
			wError = PktBase::SERVER_ERROR;
			break;
        }
    }

    PktGuildSafeAck pktGuildSafeAck;
    memset(&pktGuildSafeAck, 0, sizeof(PktGuildSafeAck));

    pktGuildSafeAck.m_dwCID = dwCID;
    pktGuildSafeAck.m_dwGID = dwGID;
    pktGuildSafeAck.m_dwSafeGold = dwSafeGold;
    pktGuildSafeAck.m_dwCharGold = dwCharGold;
    pktGuildSafeAck.m_cCmd = cCmd;

    strncpy(pktGuildSafeAck.m_szCharName, 
        lpPktGuildSafe->m_szCharName, Guild::MAX_MEMBER_NAME_LEN);

    if (!DBComponent::GuildDB::UpdateGuildGold(
        CDBSingleObject::GetInstance(), lpGuild->GetGID(), lpGuild->GetGold()))
    {
        wError = PktGuildSafe::FAIL_NOT_GUILD_UPDATE;
    }

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildSafeAck),
        sizeof(PktGuildSafeAck), CmdGuildSafe, 0, wError))
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGuildSafeAck), sizeof(PktGuildSafeAck), CmdGuildSafe));
        } 
        else
        {
            SendStream.PutBuffer(reinterpret_cast<char*>(&pktGuildSafeAck),
                sizeof(PktGuildSafeAck), CmdGuildSafe);
        }

        return true;        
    }

    return false;
}



bool GuildMemberInfoUpdate(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktGuildMemberInfoUpdate* lpPktGMIU = static_cast<PktGuildMemberInfoUpdate*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    unsigned long dwGID	= lpPktGMIU->m_dwGID;
    unsigned long dwCID	= lpPktGMIU->m_dwCID;

    CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));
    if (NULL == lpGuild)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        lpGuild->UpdateMemberInfo(dwCID, 
            lpPktGMIU->m_MemberListInfo, lpPktGMIU->m_MemberDetailInfo);
    }

    PktGuildMemberInfoUpdate pktGMIUAck;
    memset(&pktGMIUAck, 0, sizeof(PktGuildMemberInfoUpdate));

    pktGMIUAck.m_dwGID = dwGID;
    pktGMIUAck.m_dwCID = dwCID;

    pktGMIUAck.m_MemberListInfo = lpPktGMIU->m_MemberListInfo;
    pktGMIUAck.m_MemberDetailInfo = lpPktGMIU->m_MemberDetailInfo;

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGMIUAck),
        sizeof(PktGuildMemberInfoUpdate), CmdGuildMemberInfoUpdate, 0, wError))
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGMIUAck),
                sizeof(PktGuildMemberInfoUpdate), CmdGuildMemberInfoUpdate));
        }
        else
        {
            SendStream.PutBuffer(reinterpret_cast<char*>(&pktGMIUAck),
                sizeof(PktGuildMemberInfoUpdate), CmdGuildMemberInfoUpdate);
        }

        return true;
    }

    return false;
}


}
}

