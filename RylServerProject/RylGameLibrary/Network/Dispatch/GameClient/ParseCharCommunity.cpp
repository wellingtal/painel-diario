
#include "stdafx.h"

#include <Log/ItemLog.h>
#include <Log/GuildLog.h>
#include <Log/LogStruct.h>

#include <Utility/Setup/ServerSetup.h>

#include <Item/Container/ExchangeContainer.h>
#include <Item/Container/StallContainer.h>

#include <Community/Party/PartyMgr.h>
#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/Packet/PacketStruct/FriendPacket.h>

#include <Creature/EnemyCheck.h>
#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/Siege/SiegeObjectMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include "SendCharCommunity.h"
#include "ParseCharCommunity.h"
#include "GameClientDispatch.h"

bool GameClientParsePacket::ParseCharExchangeCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    using namespace GameClientSendPacket;

    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktExC), GameClientDispatch);
    
    PktExC*     lpPktExC    = reinterpret_cast<PktExC*>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());
    
    CSendStream&    SendStream  = GameClientDispatch.GetSendStream();
	unsigned long   dwSenderID	= lpPktExC->m_dwSenderID;
	unsigned long   dwRecverID	= lpPktExC->m_dwRecverID;
	unsigned char   cCmd	    = lpPktExC->m_cCmd;	

	// 캐릭터 얻기
    CCreatureManager& CreatureManager = CCreatureManager::GetInstance();

	CCharacter* lpSenderCharacter = CreatureManager.GetCharacter(dwSenderID); 
	CCharacter* lpRecverCharacter = CreatureManager.GetCharacter(dwRecverID); 

	if (NULL == lpSenderCharacter || NULL == lpRecverCharacter)
	{	
        SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::SERVER_ERROR);
		ERRLOG1(g_Log, "CID:%10u 캐릭터 얻기 실패.", dwRecverID);	
		return true;
	}

	// Dispatcher얻기
	CGameClientDispatch* lpSenderDispatch = lpSenderCharacter->GetDispatcher();
    CGameClientDispatch* lpRecverDispatch = lpRecverCharacter->GetDispatcher();

	if (NULL == lpSenderDispatch || NULL == lpRecverDispatch)
	{	
		SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::SERVER_ERROR);
		ERRLOG1(g_Log, "CID:%10u 유저 세션 얻기 실패.", dwRecverID);	
		return true;	
	}

    CSendStream& SenderSendStream = lpSenderDispatch->GetSendStream();
    CSendStream& RecverSendStream = lpRecverDispatch->GetSendStream();

    Item::CExchangeContainer& SenderExchageContainer = lpSenderCharacter->GetExchange();    
    Item::CExchangeContainer& RecverExchageContainer = lpRecverCharacter->GetExchange();
    
	switch (cCmd)
	{
		case PktExC::EXC_PROPOSE:		// 제안
		{
			if (NULL != SenderExchageContainer.GetExchangeCharacter() || 
				NULL != RecverExchageContainer.GetExchangeCharacter())
			{
                SendCharExchangeCmd(SendStream, 
                    dwSenderID, dwRecverID, cCmd, PktExC::FAIL_EXCHANGING_CHAR);
				break;	
			}

            // Changed by Sparrowhawk ( 2003/04/14 14시47분)
			if (lpRecverCharacter->GetRejectOption().Reject.m_Exchange ||
                lpRecverCharacter->GetBanList().IsBan(lpCharacter->GetCID(), lpCharacter->GetCharacterName()))
			{
                SendCharExchangeCmd(SendStream, 
                    dwSenderID, dwRecverID, cCmd, PktExC::FAIL_REJECT);
				break;	
			}

            SendCharExchangeCmd(RecverSendStream, 
                dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;		

		case PktExC::EXC_ACCEPT:		// 수락
		{	
			if(!lpSenderCharacter->GetExchangeID() && !lpRecverCharacter->GetExchangeID())
			{
				SenderExchageContainer.SetExchangeCharacter(lpRecverCharacter); 
				RecverExchageContainer.SetExchangeCharacter(lpSenderCharacter);

				lpSenderCharacter->SetExchangeID(dwRecverID);
				lpRecverCharacter->SetExchangeID(dwSenderID);

				SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
				SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			}			

		} break;		

		case PktExC::EXC_REFUSE:		// 거절		
		{
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;		

		case PktExC::EXC_LOCK:			// 락하기
		{
			SenderExchageContainer.SetLock(true);

			SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;

		case PktExC::EXC_UNLOCK:		// 락 해제
		{
			if (SenderExchageContainer.GetAccept())
			{
				SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::SERVER_ERROR);
				ERRLOG1(g_Log, "CID:%10u 확인 버튼을 누른 상태에서 락을 해제하려 함.", dwSenderID);	
				break;	
			}

			if (RecverExchageContainer.GetAccept())
			{
				RecverExchageContainer.SetAccept(false);

                SendCharExchangeCmd(SenderSendStream, dwRecverID, dwSenderID, PktExC::EXC_CANCEL, PktExC::NO_SERVER_ERR);
                SendCharExchangeCmd(RecverSendStream, dwRecverID, dwSenderID, PktExC::EXC_CANCEL, PktExC::NO_SERVER_ERR);	


				RecverExchageContainer.SetLock(false);

                SendCharExchangeCmd(SenderSendStream, dwRecverID, dwSenderID, PktExC::EXC_UNLOCK, PktExC::NO_SERVER_ERR);
                SendCharExchangeCmd(RecverSendStream, dwRecverID, dwSenderID, PktExC::EXC_UNLOCK, PktExC::NO_SERVER_ERR);	
			}

			SenderExchageContainer.SetLock(false);

            SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);	

		} break;

		case PktExC::EXC_OK:			// 확인
		{
			if (false == SenderExchageContainer.GetLock() || false == RecverExchageContainer.GetLock())
			{
				SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::SERVER_ERROR);
				ERRLOG1(g_Log, "CID:%10u 락하지 않은 상태에서 확인하러 함.", dwSenderID);	
				break;	
			}

			SenderExchageContainer.SetAccept(true);

			SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;		

		case PktExC::EXC_CANCEL:		// 확인 취소
		{
			SenderExchageContainer.ExchangeCancel();

		} break;	

		case PktExC::EXC_EXCHANGE:		// 거래 성립
		{
			if (!RecverExchageContainer.GetAccept())
			{	
				SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::FAIL_NOT_ALL_OK);

				ERRLOG2(g_Log, "양쪽 모두 확인하지 않은 상태에서 거래 성립 요청. dwSenderID: 0x%08x, dwRecverID: 0x%08x", 
					dwSenderID, dwRecverID);	
				break;	
			}

			// 수락할때에 캐릭터랑 같은지 체크 
			if (lpSenderCharacter->GetExchangeID() != dwRecverID || lpRecverCharacter->GetExchangeID() != dwSenderID)
			{
				// Rodin : 클라이언트단의 에러 메세지 처리가 없습니다. 
				//		그리고 굳이 캐릭터에 ExchangeID를 둬서 비교할 필요가 있을까요?
				//		ExchangeContainer의 ExchangeCharacter를 이용해서 체크해서 괜찮을 듯 한데...

				SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::FAIL_EXCHANGING_ID);
				ERRLOG2(g_Log, "거래 성립 요청을 한 캐릭터가 수락할때에 캐릭터랑 다르다. dwSenderID: 0x%08x, dwRecverID: 0x%08x", 
					dwSenderID, dwRecverID);	
				break;	
			}

			lpSenderCharacter->SetExchangeID(0);
			lpRecverCharacter->SetExchangeID(0);

			SenderExchageContainer.SetAccept(true);
			if (!SenderExchageContainer.ExchangeOK(true)) 
            {
				ERRLOG2(g_Log, "거래 상태가 없는 캐릭터입니다. dwSenderID : 0x%08x, dwRecverID : 0x%08x", 
                    dwSenderID, dwRecverID);	
			}

			SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;		

		case PktExC::EXC_QUIT:			// 거래 취소
		{
			if (!SenderExchageContainer.ExchangeOK(false))
            {
				ERRLOG2(g_Log, "거래 상대가 없는 캐릭터입니다. dwSenderID : 0x%08x, dwRecverID : 0x%08x", 
                    dwSenderID, dwRecverID);	
			}

			SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

			lpSenderCharacter->SetExchangeID(0);
			lpRecverCharacter->SetExchangeID(0);

		} break;		
	}

	return true;
}

bool GameClientParsePacket::ParseCharPartyCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    int nPacketLength = lpPktBase->GetLen();

    if (nPacketLength < sizeof(PktPC))
    {
        ERRLOG0(g_Log, "패킷 길이가 맞지 않습니다.");
        return false;
    }

	PktPC*          lpPktPC         = static_cast<PktPC*>(lpPktBase);
	unsigned long   dwPartyID       = lpPktPC->m_dwPartyID;
	unsigned long   dwSenderID	    = lpPktPC->m_dwSenderID;
	unsigned long   dwReferenceID	= lpPktPC->m_dwReferenceID;

    unsigned short  usCmd           = lpPktPC->m_wCmd;
    unsigned short  usError         = 0;

    // 특별히 szSenderName이 필요없는 경우에 들어가는 쓰레기값
    char*           szSenderName	= "로딘";	
    CSendStream&    SendStream      = GameClientDispatch.GetSendStream();
	
    AddressInfo     SenderAddress;

    using namespace GameClientSendPacket;

	switch (usCmd)
	{
		case PktPC::PC_INVITE:		// 초대
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
            CGameClientDispatch* lpRecvGameClientDispatch = NULL;
			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 요청자 캐릭터 아이디 틀림 CID:%10u", lpSendCharacter->GetCID(), dwSenderID);					
			}
            else 
            {
			    CCharacter* lpRecvCharacter = CCreatureManager::GetInstance().GetCharacter(dwReferenceID);                 
			    if (NULL == lpRecvCharacter)
			    {	
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u 캐릭터 얻기 실패.", dwReferenceID);	
			    }
                else if (lpRecvCharacter->GetRejectOption().Reject.m_Party ||
                    lpRecvCharacter->GetBanList().IsBan(lpSendCharacter->GetCID(), lpSendCharacter->GetCharacterName()))
			    {
                    // 거부 옵션 체크
                    usError = PktPC::FAIL_REJECT;
			    }
                else if (NULL != lpRecvCharacter->GetParty())
			    {   
                    usError = PktPC::CHAR_JOINED;
				    ERRLOG1(g_Log, "CID:%10u 이미 파티에 가입되어 있습니다.", dwReferenceID);				    
			    }
                else
                {
                    lpRecvGameClientDispatch = lpRecvCharacter->GetDispatcher();
			        if (NULL == lpRecvGameClientDispatch)
			        {	
                        usError = PktPC::FAIL_INVITE;
				        ERRLOG1(g_Log, "CID:%10u 이미 로그아웃한 상대에게 파티 가입 신청하였습니다.", dwReferenceID);	
			        }
                }
            }

            SendCharPartyCmd((0 == usError) ? lpRecvGameClientDispatch->GetSendStream() : SendStream,
                dwPartyID, SenderAddress, (0 == usError) ? lpSendCharacter->GetCharacterName() : szSenderName,
                dwSenderID, dwReferenceID, usCmd, usError);

		} break;		

		case PktPC::PC_ACCEPT:		// 파티 가입	
		{	
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());	
				
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 요청자 캐릭터 아이디 틀림 CID:%10u", lpSendCharacter->GetCID(), dwSenderID);
			}
            else if (NULL != lpSendCharacter->GetParty())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:%10u 이미 가입된 캐릭터가 가입을 중복 수락", dwSenderID);					
			}
            else
            {
			    CCharacter* lpRecvCharacter = CCreatureManager::GetInstance().GetCharacter(dwReferenceID); 
			    if (NULL == lpRecvCharacter)
			    {	
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u 캐릭터 얻기 실패.", dwReferenceID);	
			    }
                else
                {
			        CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpRecvCharacter->GetParty());
			        if (NULL == lpParty)
			        {
				        // 파티 생성

						GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
							CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

						if (0 == lpDBAgentDispatch)
                        {
                            usError = PktPC::SERVER_ERROR;
					        ERRLOG0(g_Log, "에이전트 얻기 실패.");	
                        }
                        else
                        {
							PARTYLOG(DETLOG2(g_Log, "파티 로그(생성 요청) - PID:0x00000000 리더:0x%08x 가입자:0x%08x", 
								dwReferenceID, dwSenderID));

							// 캐릭터 정보(클래스, 레벨) //

							SendCharPartyCreateToDBAgent(lpDBAgentDispatch->GetSendStream(), dwReferenceID, dwSenderID, CServerSetup::GetInstance().GetServerZone(),
								lpRecvCharacter->GetClass(), lpRecvCharacter->GetLevel(), lpRecvCharacter->GetGID(),
								lpSendCharacter->GetClass(), lpSendCharacter->GetLevel(), lpSendCharacter->GetGID());
                        }
			        }
			        else
			        {
						if (PARTY::MAX_MEM == lpParty->GetMemberNum())
				        {
                            usError = PktPC::OVER_MEMBER;					    
					        ERRLOG0(g_Log, "파티 인원 초과.");						    
				        }
                        else
                        {
							PARTYLOG(DETLOG2(g_Log, "파티 로그(가입 요청) - PID:0x%08x 가입자:0x%08x", lpParty->GetUID(), dwSenderID));							

							lpParty->SendPartyMemberDataToDBAgent(dwSenderID, lpSendCharacter->GetGID(), lpSendCharacter->GetClass(), CServerSetup::GetInstance().GetServerID(), 
										lpSendCharacter->GetLevel(), lpSendCharacter->GetCharacterName(), PktDD::SCmdInsertPartyMem);
                        }
			        }
                }
            }

            if (0 != usError)
            {
				SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, szSenderName, 
                    dwSenderID, dwReferenceID, usCmd, usError);
            }
		
		} break;		

		case PktPC::PC_REFUSE:		// 거절		
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
            CCharacter* lpRecvCharacter = NULL;
            CGameClientDispatch* lpRecvGameClientDispatch = NULL;

			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 요청자 캐릭터 아이디 틀림 CID:%10u", lpSendCharacter->GetCID(), dwSenderID);
			}
            else
            {
				lpRecvCharacter = CCreatureManager::GetInstance().GetCharacter(dwReferenceID); 
			    if (NULL == lpRecvCharacter)
			    {	
                    usError = PktPC::SERVER_ERROR;				    
				    ERRLOG1(g_Log, "CID:%10u 캐릭터 얻기 실패.", dwReferenceID);				    
			    }
                else
                {
                    // 유저 세션 얻기
			        lpRecvGameClientDispatch = lpRecvCharacter->GetDispatcher();
			        if (0 == lpRecvGameClientDispatch)
			        {	
                        usError = PktPC::SERVER_ERROR;
				        ERRLOG1(g_Log, "CID:%10u 유저 세션 얻기 실패.", dwReferenceID);	
			        }
                }
            }

            SendCharPartyCmd((0 == usError) ? lpRecvGameClientDispatch->GetSendStream() : SendStream,
                dwPartyID, SenderAddress, (0 == usError) ? lpSendCharacter->GetCharacterName() : NULL, 
                dwSenderID, dwReferenceID, usCmd, usError);
	
		} break;		

		case PktPC::PC_BANISH:		// 퇴장
		{
			CCharacter* lpRecvCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpRecvCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
                ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());					
			}
            else if (dwReferenceID != lpRecvCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 요청자 캐릭터 아이디 틀림 CID:%10u", lpRecvCharacter->GetCID(), dwReferenceID);					
			}
            else
            {
                CCharacterParty* lpRecvParty = reinterpret_cast<CCharacterParty*>(lpRecvCharacter->GetParty());
			    if (NULL == lpRecvParty)
			    {
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u 파티 얻기 실패.", dwReferenceID);					    
			    }
				else
				{
					if (false == lpRecvParty->IsMember(dwSenderID))
					{
						usError = PktPC::SERVER_ERROR;
						ERRLOG2(g_Log, "CID:%10u 파티원이 아닌 사람을 탈퇴 시키려 하고 있음. 탈퇴자:0x%08x", dwReferenceID, dwSenderID);
					}
					else
					{
						if (false == lpRecvParty->IsLeader(dwReferenceID))
						{
							usError = PktPC::SERVER_ERROR;
							ERRLOG2(g_Log, "CID:%10u 리더가 아닌 캐릭터가 탈퇴 요청. 탈퇴자:0x%08x", dwReferenceID, dwSenderID);
						}
						else
						{                    
							PARTYLOG(DETLOG3(g_Log, "파티 로그(킥 요청) - PID:0x%08x 탈퇴자:0x%08x 리더:0x%08x", 
								lpRecvParty->GetUID(), dwSenderID, dwReferenceID));
							lpRecvParty->SendPartyMemberDataToDBAgent(dwSenderID, dwReferenceID, 0, 0, 0, "킥", PktDD::SCmdDeletePartyMem);
						}
					}
				}
            }

            if (0 != usError)
            {
                SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, szSenderName, 
                    dwSenderID, dwReferenceID, usCmd, usError);
            }
	
		} break;		

		case PktPC::PC_SECESSION:	// 탈퇴
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 요청자 캐릭터 아이디 틀림 CID:%10u", lpSendCharacter->GetCID(), dwSenderID);
			}
            else 
            {
			    CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpSendCharacter->GetParty());
			    if (NULL == lpParty)
			    {
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u 파티 얻기 실패.", dwSenderID);
			    }
                else
                {
					PARTYLOG(DETLOG2(g_Log, "파티 로그(탈퇴 요청) - PID:0x%08x 탈퇴자:0x%08x", lpParty->GetUID(), dwSenderID));
					lpParty->SendPartyMemberDataToDBAgent(dwSenderID, dwSenderID, 0, 0, 0, 
						lpSendCharacter->GetCharacterName(), PktDD::SCmdDeletePartyMem);
                }
            }

            if (0 != usError)
            {
                SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, szSenderName, 
                    dwSenderID, dwReferenceID, usCmd, usError);
            }
	
		} break;	

		case PktPC::PC_TRANSFER:	// 리더 양도
		{
			CCharacter* lpRecvCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpRecvCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());
			}
            else if (dwReferenceID != lpRecvCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 요청자 캐릭터 아이디 틀림 CID:%10u", lpRecvCharacter->GetCID(), dwReferenceID);
			}
            else
            {
                CCharacterParty* lpRecvParty = reinterpret_cast<CCharacterParty*>(lpRecvCharacter->GetParty());
			    if (NULL == lpRecvParty)
			    {
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u 파티 얻기 실패.", dwReferenceID);					            
			    }
				else
				{                      
					if (false == lpRecvParty->IsMember(dwSenderID) || false == lpRecvParty->IsLeader(dwReferenceID))
					{
						usError = PktPC::SERVER_ERROR;
						ERRLOG2(g_Log, "Reference가 리더가 아니거나 Sender가 파티원이 아닙니다. SenderID:0x%08x, ReferenceID:0x%08x", 
							dwSenderID, dwReferenceID);					            
					}
					else
					{
						PARTYLOG(DETLOG3(g_Log, "파티 로그(리더양도 요청) - PID:0x%08x 새리더:0x%08x 기존리더:0x%08x", 
							lpRecvParty->GetUID(), dwSenderID, dwReferenceID));
						lpRecvParty->SendPartyMemberDataToDBAgent(dwSenderID, dwReferenceID, 0, 0, 0, "리더양도", PktDD::SCmdTransferLeader);
					}
				}
            }

            if (0 != usError)
            {
                SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, 
                    szSenderName, dwSenderID, dwReferenceID, usCmd, usError);
            }
	
		} break;

		case PktPC::PC_DESTROY:		// 파괴
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (0 == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());					
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 요청자 캐릭터 아이디 틀림 CID:%10u", lpSendCharacter->GetCID(), dwSenderID);
			}
            else
            {
                CParty* lpParty = lpSendCharacter->GetParty();
			    if (NULL == lpParty)
			    {
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u 파티 얻기 실패.", dwSenderID);					    
			    }
                else if (lpParty->IsLeader(dwSenderID))
			    {          		    
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					if (0 == lpDBAgentDispatch)
                    {
                        usError = PktPC::SERVER_ERROR;
                        ERRLOG0(g_Log, "에이전트 얻기 실패.");                            
                    }
                    else
                    {
						PARTYLOG(DETLOG1(g_Log, "파티 로그(해체 요청) - PID:0x%08x", lpParty->GetUID()));
                        SendCharPartyDestroyToDBAgent(lpDBAgentDispatch->GetSendStream(), lpParty->GetUID());
                    }                    
			    }
            }

            if (0 != usError)
            {
                SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, 
                    szSenderName, dwSenderID, dwReferenceID, usCmd, usError);
            }

		} break;		

		case PktPC::PC_FINDPARTY:	// 파티 찾기
		{
			if (0 != dwPartyID)
			{
				usError = PktPC::ALREADY_PARTY;
				ERRLOG1(g_Log, "CID:%10u 파티에 가입된 녀석이 파티 찾기에 등록하려 합니다.", dwSenderID);					
			}
            else if (0 == dwReferenceID) 
			{
				if (false == CPartyMgr::GetInstance().DeleteFindPartyList(dwSenderID))
				{
                    usError = PktPC::SERVER_ERROR;
					ERRLOG1(g_Log, "CID:%10u 파티 찾기 리스트에서 삭제 실패.", dwSenderID);						
				}
			} 
			else 
			{
				if (false == CPartyMgr::GetInstance().AddFindPartyList(dwSenderID))
				{
                    usError = PktPC::SERVER_ERROR;					
					ERRLOG1(g_Log, "CID:%10u 파티 찾기 리스트에 추가 실패.", dwSenderID);						
				}
			}

			SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, szSenderName, 
                dwSenderID, dwReferenceID, usCmd, usError);

		} break;

		case PktPC::PC_FINDMEMBER:	// 파티원 찾기
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;				
				ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());
			}
            else
            {
                CParty* lpParty = lpSendCharacter->GetParty();
			    if (NULL == lpParty)
			    {
					usError = PktPC::FAIL_NOT_PARTY;
				    ERRLOG1(g_Log, "CID:%10u 파티 얻기 실패.", lpSendCharacter->GetCID());
			    }
                else if (false == lpParty->IsLeader(dwSenderID))
			    {
                    usError = PktPC::FAIL_NOT_LEADER;
				    ERRLOG1(g_Log, "CID:%10u 리더도 아닌 놈이 파티 멤버 찾기에 등록하려 합니다.", dwSenderID);
			    }
                else if (0 == dwReferenceID) 
			    {
				    if (false == CPartyMgr::GetInstance().DeleteFindMemberList(dwPartyID))
				    {
                        usError = PktPC::SERVER_ERROR;
					    ERRLOG1(g_Log, "CID:%10u 파티원 찾기 리스트에서 삭제 실패.", dwSenderID);
				    }
			    } 
			    else 
			    {
				    if (false == CPartyMgr::GetInstance().AddFindMemberList(dwPartyID))
				    {
                        usError = PktPC::SERVER_ERROR;					    
					    ERRLOG1(g_Log, "CID:%10u 파티원 찾기 리스트에 추가 실패.", dwSenderID);
				    }
			    }	
            }

			SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, 
                szSenderName, dwSenderID, dwReferenceID, usCmd, usError);

		} break;

		case PktPC::PC_AUTOROUTING_ON:		// 오토루팅 켜기
		case PktPC::PC_AUTOROUTING_OFF:		// 오토루팅 끄기
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpSendCharacter)
			{	
				usError = PktPC::SERVER_ERROR;				
				ERRLOG1(g_Log, "UID:0x%08x 캐릭터 얻기 실패.", GameClientDispatch.GetUID());
			}
			else
			{
				CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpSendCharacter->GetParty());
				if (NULL == lpParty)
				{
					usError = PktPC::SERVER_ERROR;
					ERRLOG1(g_Log, "CID:%10u 파티 얻기 실패.", lpSendCharacter->GetCID());
				}
				else if (false == lpParty->IsLeader(lpSendCharacter->GetCID()))
				{
					usError = PktPC::SERVER_ERROR;				    
					ERRLOG1(g_Log, "CID:%10u 리더도 아닌 놈이 오토루팅을 조정하려 합니다.", dwSenderID);
				}
				else 
				{
					PARTYLOG(DETLOG3(g_Log, "파티 로그(오토루팅 켜기/끄기) CID가 0이면 ALL- PID:0x%08x CID:%10u On:%d", 
						lpParty->GetUID(), dwSenderID, ((PktPC::PC_AUTOROUTING_ON == usCmd) ? true : false)));

					if (usCmd == PktPC::PC_AUTOROUTING_ON)
					{
						lpParty->SendPartyMemberDataToDBAgent(dwSenderID, dwReferenceID, 0, 0, 0, "오토루팅", PktDD::SCmdAutoRoutingOn);
					}
					else
					{
						lpParty->SendPartyMemberDataToDBAgent(dwSenderID, dwReferenceID, 0, 0, 0, "오토루팅", PktDD::SCmdAutoRoutingOff);
					}
				}	
			}

			if (0 != usError)
			{
				SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, 
					szSenderName, dwSenderID, dwReferenceID, usCmd, usError);
			}

		} break;
	}

    return true;
}

bool GameClientParsePacket::ParseCharPartyFind(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPF), GameClientDispatch);
    
    PktPF*      lpPktPF     = reinterpret_cast<PktPF*>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	// 파티 찾기
	CPartyMgr::GetInstance().SendPartyFind(lpCharacter);
    return true;
}

bool GameClientParsePacket::ParseCharStallOpen(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktStO), GameClientDispatch);
    
    PktStO*		lpPktStO	= reinterpret_cast<PktStO*>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktStO->m_dwCharID;	
	char strStallName[PktStO::MAX_STALL_NAME_LEN];
	strncpy(strStallName, lpPktStO->m_StallName, PktStO::MAX_STALL_NAME_LEN);

	if (0 == strcmp(strStallName, ""))
	{
		if (false == lpCharacter->GetStall().Close())
		{
			ERRLOG1(g_Log, "CID:%10u 이미 노점상을 닫은 상태입니다.", dwCharID);
		}
	}
	else
	{
		if (false == lpCharacter->GetStall().Open(strStallName))
		{
			ERRLOG1(g_Log, "CID:%10u 이미 노점상을 열은 상태입니다.", dwCharID);
		}
	}

	return lpCharacter->GetStall().SendCharStallOpen(strStallName);
}

bool GameClientParsePacket::ParseCharStallRegisterItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktStRI), GameClientDispatch);
    
    PktStRI*	lpPktStRI	= reinterpret_cast<PktStRI*>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

    TakeType takeType = lpPktStRI->m_TakeType;

    unsigned long   dwCharID    = lpPktStRI->m_dwCharID;
	unsigned long	dwShopID	= lpPktStRI->m_dwShopID;
	unsigned long   dwPrice     = lpPktStRI->m_dwPrice;

    Item::CItem*    lpItem      = NULL;

	unsigned short  usError     = PktBase::NO_SERVER_ERR;
    unsigned char   cCmd        = lpPktStRI->m_cCmd;

	switch (cCmd)
	{
		case PktStRI::SC_REGISTER:
		{
			lpItem = lpCharacter->GetItem(takeType.m_srcPos);
			if (NULL == lpItem)
			{
				usError = PktStRI::FAIL_NOT_ITEM;
			}
			else if (false == lpItem->GetItemInfo().m_DetailData.m_bExchangeAndDrop )
			{
				usError = PktStRI::FAIL_NOT_SELL;
			}
            else if (takeType.m_dstPos.m_cPos != TakeType::TS_STALL)
			{
                usError = PktBase::SERVER_ERROR;
                ERRLOG1(g_Log, "CID:%10u 아이템을 노점상에 잘못 올려 놓았습니다."
                    "RegisterPacket에 DstPos가 Stall이 아닙니다.", lpCharacter->GetCID());
            }
            else
            {
				lpItem->SetStallPrice(dwPrice);
				if (false == lpCharacter->MoveItem(takeType))
				{
					usError = PktBase::SERVER_ERROR;
				}
			}
			break;
		} 

		case PktStRI::SC_CANCEL:
		{
			lpItem = lpCharacter->GetItem(takeType.m_dstPos);
			if (false == lpCharacter->RemoveItem(takeType.m_dstPos))
			{
				usError = PktBase::SERVER_ERROR;
			}
			else
			{
                GAMELOG::LogStallRegisterRemoveItem(*lpCharacter, lpItem, takeType, cCmd, usError);
				return true;
			}
			break;
		} 

		case PktStRI::SC_CAMP_REGISTER:
		{
			Item::CShopContainer* lpContainer = CCreatureManager::GetInstance().GetShopContainer(dwShopID);
			if (NULL == lpContainer || Creature::CT_SIEGE_OBJECT != Creature::GetCreatureType(dwShopID))
			{
				usError = PktBase::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 아이템 등록 실패 : 길드 요새 상점이 아닙니다. ShopID:%10u", 
					lpCharacter->GetCID(), dwShopID);
			}
			else
			{
				Item::CCampShopContainer* lpCampShopContainer = reinterpret_cast<Item::CCampShopContainer*>(lpContainer);
				lpItem = lpCampShopContainer->RegisterCancelItem(lpCharacter, takeType, dwPrice, cCmd);
				if (NULL == lpItem)
				{
					usError = PktBase::SERVER_ERROR;
				}
			}
			break;
		}

		case PktStRI::SC_CAMP_CANCEL:
		{
			Item::CShopContainer* lpContainer = CCreatureManager::GetInstance().GetShopContainer(dwShopID);
			if (NULL == lpContainer || Creature::CT_SIEGE_OBJECT != Creature::GetCreatureType(dwShopID))
			{
				usError = PktBase::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 아이템 등록 취소 실패 : 길드 요새 상점이 아닙니다. ShopID:%10u", 
					lpCharacter->GetCID(), dwShopID);
			}
			else
			{
				Item::CCampShopContainer* lpCampShopContainer = reinterpret_cast<Item::CCampShopContainer*>(lpContainer);
				lpItem = lpCampShopContainer->RegisterCancelItem(lpCharacter, takeType, dwPrice, cCmd);
				if (NULL == lpItem)
				{
					usError = PktBase::SERVER_ERROR;
				}
			}
			break;
		}
	}

	// 노점상로그 : 아이템 올리기 및 내리기 (TakeItem과 같은 형식으로...)
    GAMELOG::LogStallRegisterRemoveItem(*lpCharacter, lpItem, takeType, cCmd, usError);

	return GameClientSendPacket::SendCharStallRegisterItem(GameClientDispatch.GetSendStream(), 
        dwCharID, dwShopID, takeType, dwPrice, cCmd, usError);
}

bool GameClientParsePacket::ParseCharStallEnter(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktStE), GameClientDispatch);
    
    PktStE*		lpPktStE	= reinterpret_cast<PktStE*>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCustomerID = lpPktStE->m_dwCustomerID;
	unsigned long dwOwnerID = lpPktStE->m_dwOwnerID;

	unsigned short usError = PktStE::SUCCESS;

	CCharacter* lpCustomer = CCreatureManager::GetInstance().GetCharacter(dwCustomerID);
	if (NULL == lpCustomer)
	{
		ERRLOG0(g_Log, "캐릭터 얻기 실패.");
		usError = PktStE::SERVER_ERROR;
	}
	else
	{
		// 이미 상점에 들어가 있는지를 확인한다.
		unsigned long dwOtherOwner = lpCustomer->GetStall().GetOtherOwner();

		if (0 == dwOwnerID)
		{
			// 노점상을 나가는 경우
			Item::CShopContainer* lpContainer = CCreatureManager::GetInstance().GetShopContainer(dwOtherOwner);
			if (NULL == lpContainer)
			{
				ERRLOG2(g_Log, "CID:%10u 노점상 오류 : 나오려는 노점상(%10u)의 아이디가 이상합니다.", 
					lpCustomer->GetCID(), dwOtherOwner);
				usError = PktStE::FAIL_LEAVE;
			}
			else
			{
				if (0 != dwOtherOwner)
				{
					lpContainer->SendCharStallEnter(lpCustomer->GetCID(), 0);
					lpContainer->Leave(lpCustomer);
					return true;
				}
				else
				{
					ERRLOG1(g_Log, "CID:%10u 노점상 오류 : 이미 노점상을 나온 상태입니다.", lpCustomer->GetCID());
					usError = PktStE::FAIL_LEAVE;
				}
			}
		}
		else
		{
			// 노점상에 들어가는 경우
			Item::CShopContainer* lpContainer = CCreatureManager::GetInstance().GetShopContainer(dwOwnerID);
			if (NULL == lpContainer)
			{
				ERRLOG2(g_Log, "CID:%10u 노점상 오류 : 들어가려는 노점상(%10u)의 아이디가 이상합니다.", 
					lpCustomer->GetCID(), dwOwnerID);
				usError = PktStE::FAIL_ENTER;
			}
			else
			{
				if (0 == dwOtherOwner)
				{
					if (false == lpContainer->Enter(lpCustomer))
					{
						usError = PktStE::FAIL_FULL_STALL;
					}
					else
					{
						return lpContainer->SendCharStallEnter(lpCustomer->GetCID(), dwOwnerID);
					}
				}
				else
				{
					ERRLOG3(g_Log, "CID:%10u 노점상 오류 : 이미 노점상에 들어간 상태입니다. 들어가고자 하는 곳 : %10u, 들어가있는 곳 : %10u", 
						lpCustomer->GetCID(), dwOwnerID, dwOtherOwner);
					usError = PktStE::FAIL_ENTER;
				}
			}
		}
	}

	return GameClientSendPacket::SendCharStallEnter(GameClientDispatch.GetSendStream(), 
        dwCustomerID, dwOwnerID, usError);
}


// 친구(거부)리스트 추가
bool GameClientParsePacket::ParseCharFriendAdd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktFriendAddReq), GameClientDispatch);
    
    PktFriendAddReq*	lpPktFriendAddRequest   = reinterpret_cast<PktFriendAddReq*>(lpPktBase);
    CCharacter*         lpCharacter             = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());
    
    unsigned long   dwOwnerCID  = lpCharacter->GetCID();
    unsigned short  usError     = 0;
    unsigned char   cAckCmd     = PktFriendAck::UNKNOWN_ERROR_ACK;

    switch (lpPktFriendAddRequest->m_cCmd)
    {
    case PktFriendAddReq::ADD_FRIEND_REQ:   cAckCmd = PktFriendAck::ADD_FRIEND_ACK; break;
    case PktFriendAddReq::BAN_FRIEND_REQ:   cAckCmd = PktFriendAck::ADD_BAN_ACK;    break;
    default:                                usError = PktFriendAck::CLIENT_ERROR;   break;
    }

    unsigned long       dwFriendUID     = 0;
    unsigned long       dwFriendCID     = 0;
    const char*			szFriendName    = NULL;

	unsigned long		dwGID			= 0;
	unsigned short		wClass			= 0;
	char				cLevel			= 0;

    if (0 == usError)
    {
        // 친구(혹은 거부 리스트 추가)
        CCharacter* lpFriendCharacter = CCreatureManager::GetInstance().GetCharacter(
            lpPktFriendAddRequest->m_szName);

        if (NULL == lpFriendCharacter)
        {
            usError = PktFriendAck::NOT_LOGINED;
        }
        else
        {        
            switch (lpPktFriendAddRequest->m_cCmd)
            {
            case PktFriendAddReq::ADD_FRIEND_REQ:

				dwGID	= lpFriendCharacter->GetGID();
				wClass	= lpFriendCharacter->GetClass();
				cLevel	= lpFriendCharacter->GetLevel();
               
                if (lpFriendCharacter->GetRejectOption().Reject.m_Friend || 
                    lpFriendCharacter->GetBanList().IsBan(dwOwnerCID, lpCharacter->GetCharacterName()))
                {
                    // 상대가 친구 거부를 했거나, 내가 상대의 Ban List에 속해 있음.
                    usError = PktFriendAck::REJECTED;
                }
				else if (EnemyCheck::EC_ENEMY == lpCharacter->IsEnemy(lpFriendCharacter))
				{
					// 적군은 리스트에 추가할 수 없다.
					usError = PktFriendAck::FAIL_ENEMY;
				}
				else if (false == lpCharacter->GetFriendList().Add(
					lpFriendCharacter->GetCID(), lpFriendCharacter->GetCharacterName(), 
					lpFriendCharacter->GetGID(), lpFriendCharacter->GetClass(), lpFriendCharacter->GetLevel(), 
					CServerSetup::GetInstance().GetServerID()))
                {
                    // 리스트 추가 실패
                    usError = PktFriendAck::LIST_FULL;
                    ERRLOG2(g_Log, "CID:%10u 리스트가 꽉 차서, 더 이상 친구를 등록할 수 없습니다. 현재 리스트 수:%d", 
                        dwOwnerCID, lpCharacter->GetFriendList().GetFriendNum());
                }
                else
                {
                    // lpFriendCharacter 에게 lpCharacter가 자신을 친구로 등록했다는 메시지를 보내 준다.
                    CGameClientDispatch* lpFriendDispatch = lpFriendCharacter->GetDispatcher();
                    if (NULL != lpFriendDispatch)
                    {
                        GameClientSendPacket::SendCharFriendAdded(lpFriendDispatch->GetSendStream(), 
                            lpCharacter->GetCID(), lpCharacter->GetCharacterName());
                    }

                    // 만일 친구 추가한 녀석이 내 거부 리스트에 있을 경우, 그녀석을 제거한다.
                    if (lpCharacter->GetBanList().Remove(lpFriendCharacter->GetCID()))
                    {
                        cAckCmd = PktFriendAck::MOVE_BAN_TO_FRIEND;
                    }

                    // DBAgent로 친구 추가 전송
                    szFriendName    = lpFriendCharacter->GetCharacterName();
                    dwFriendUID     = lpFriendCharacter->GetUID();
                    dwFriendCID     = lpFriendCharacter->GetCID();

					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch,
						CChatDispatch::GetDispatchTable());

                    if (NULL != lpDBAgentDispatch && NULL != lpChatDispatch)
                    {
                        CSendStream& DBAgentSendStream  = lpDBAgentDispatch->GetSendStream();
                        CSendStream& ChatSendStream     = lpChatDispatch->GetSendStream();

                        GameClientSendPacket::SendFriendListChangeToDB(DBAgentSendStream, lpCharacter->GetUID(), dwOwnerCID, dwFriendUID, dwFriendCID, 0, 
                            lpFriendCharacter->GetGID(), lpFriendCharacter->GetClass(), lpFriendCharacter->GetLevel(), 
							CServerSetup::GetInstance().GetServerID(), PktFriendDB::ADD_FRIEND);

                        GameClientSendPacket::SendFriendListChangeToDB(ChatSendStream, lpCharacter->GetUID(), dwOwnerCID, dwFriendUID, dwFriendCID, 0, 
                            lpFriendCharacter->GetGID(), lpFriendCharacter->GetClass(), lpFriendCharacter->GetLevel(), 
							CServerSetup::GetInstance().GetServerID(), PktFriendDB::ADD_FRIEND);
							
                        if (PktFriendAck::MOVE_BAN_TO_FRIEND == cAckCmd)
                        {
                            // 추가한 녀석이 내 거부 리스트에 있을 경우, 그 녀석을 거부 리스트에서 제거.
                            GameClientSendPacket::SendFriendListChangeToDB(DBAgentSendStream, lpCharacter->GetUID(),
                                dwOwnerCID, dwFriendUID, dwFriendCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_BAN);

                            GameClientSendPacket::SendFriendListChangeToDB(ChatSendStream, lpCharacter->GetUID(),
                                dwOwnerCID, dwFriendUID, dwFriendCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_BAN);
                        }
                    }
                }
                break;

            case PktFriendAddReq::BAN_FRIEND_REQ:                
				// edith 2009.07.30 거부리스트에 적군도 거부할 수 있도록 수정. 다만 종족이 다를경우에도 가능하게 수정
/*				if (EnemyCheck::EC_ENEMY == lpCharacter->IsEnemy(lpFriendCharacter))
				{
					// 적군은 리스트에 추가할 수 없다.
					usError = PktFriendAck::FAIL_ENEMY;
				}
				else */
				if (false == lpCharacter->GetBanList().Add(lpFriendCharacter->GetCID(), lpFriendCharacter->GetCharacterName(), 
					lpFriendCharacter->GetGID(), lpFriendCharacter->GetClass(), lpFriendCharacter->GetLevel(), CServerSetup::GetInstance().GetServerID()))
                {
                    usError = PktFriendAck::LIST_FULL;
                    ERRLOG2(g_Log, "CID:%10u 리스트가 꽉 차서, 더 이상 거부 대상을 등록할 수 없습니다. 현재 리스트 수:%d", 
                        dwOwnerCID, lpCharacter->GetBanList().GetBanNum());
                }
                else
                {
					dwGID	= lpFriendCharacter->GetGID();
					wClass	= lpFriendCharacter->GetClass();
					cLevel	= lpFriendCharacter->GetLevel();

                    // lpFriendCharacter 가 자신을 친구로 등록하고 있으면, 내가 로그아웃하고 있는 것처럼 보이게 한다.
                    CFriendList::Rebind* lpRebind = lpFriendCharacter->GetFriendList().GetFriend(dwOwnerCID);
                    if (NULL != lpRebind && lpRebind->IsFriend(lpCharacter->GetCharacterName()))
                    {
                        CGameClientDispatch* lpFriendDispatch = lpFriendCharacter->GetDispatcher();
                        if (NULL != lpFriendDispatch)
                        {
                            GameClientSendPacket::SendCharFriendAck(lpFriendDispatch->GetSendStream(),
                                PktFriendAck::FRIEND_LOGOUT, dwOwnerCID, lpCharacter->GetCharacterName(), 0, 0, 0, 0, 0);
                        }
                    }

                    // 만일 거부 추가한 녀석이 내 친구 리스트에 있을 경우, 그녀석을 제거한다.
                    if (lpCharacter->GetFriendList().Remove(lpFriendCharacter->GetCID()))
                    {
                        cAckCmd = PktFriendAck::MOVE_FRIEND_TO_BAN;
                    }

                    szFriendName    = lpFriendCharacter->GetCharacterName();
                    dwFriendUID     = lpFriendCharacter->GetUID();
                    dwFriendCID     = lpFriendCharacter->GetCID();

                    // DBAgent, 채팅 서버로 Ban 추가 전송                    
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch,
						CChatDispatch::GetDispatchTable());

                    if (NULL != lpDBAgentDispatch && NULL != lpChatDispatch)
                    {
                        CSendStream& DBAgentStream  = lpDBAgentDispatch->GetSendStream();
                        CSendStream& ChatStream     = lpChatDispatch->GetSendStream();

                        GameClientSendPacket::SendFriendListChangeToDB(DBAgentStream, lpCharacter->GetUID(), 
                            dwOwnerCID, dwFriendUID, dwFriendCID, 0, lpFriendCharacter->GetGID(), lpFriendCharacter->GetClass(), 
							lpFriendCharacter->GetLevel(), CServerSetup::GetInstance().GetServerID(), PktFriendDB::ADD_BAN);

                        GameClientSendPacket::SendFriendListChangeToDB(ChatStream, lpCharacter->GetUID(), 
							dwOwnerCID, dwFriendUID, dwFriendCID, 0, lpFriendCharacter->GetGID(), lpFriendCharacter->GetClass(), 
                            lpFriendCharacter->GetLevel(), CServerSetup::GetInstance().GetServerID(), PktFriendDB::ADD_BAN);
                        
                        if (PktFriendAck::MOVE_FRIEND_TO_BAN == cAckCmd)
                        {
                            GameClientSendPacket::SendFriendListChangeToDB(DBAgentStream, lpCharacter->GetUID(), 
                                dwOwnerCID, dwFriendUID, dwFriendCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_FRIEND);
                            
                            GameClientSendPacket::SendFriendListChangeToDB(ChatStream, lpCharacter->GetUID(), 
                                dwOwnerCID, dwFriendUID, dwFriendCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_FRIEND);
                        }
                    }
                }
                break;

            default:

                usError = PktFriendAck::CLIENT_ERROR;
                ERRLOG2(g_Log, "CID:%10u 클라이언트에서 잘못된 친구 커맨드를 보내 왔습니다. 커맨드:%d",
                    dwOwnerCID, lpPktFriendAddRequest->m_cCmd);                
            };
        }
    }

    return GameClientSendPacket::SendCharFriendAck(GameClientDispatch.GetSendStream(), 
        cAckCmd, dwFriendCID, szFriendName, dwGID, wClass, cLevel, CServerSetup::GetInstance().GetServerID(), usError);
}


// 친구(거부)리스트 삭제
bool GameClientParsePacket::ParseCharFriendRemove(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktFriendRemoveReq), GameClientDispatch);
    
    PktFriendRemoveReq* lpPktFriendRemoveRequest    = reinterpret_cast<PktFriendRemoveReq*>(lpPktBase);
    CCharacter*         lpCharacter                 = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

    unsigned long       dwOwnerCID  = lpCharacter->GetCID();
    unsigned short      usError     = 0;
    unsigned char       cAckCmd     = PktFriendAck::UNKNOWN_ERROR_ACK;
	
	char szTargetName[PktFriendAck::MAX_NAME];
    char szRemovedName[PktFriendAck::MAX_NAME];
	memset(szTargetName, 0, sizeof(PktFriendAck::MAX_NAME));
	memset(szRemovedName, 0, sizeof(PktFriendAck::MAX_NAME));

	// PktFriendRemoveReq::NOFRIEND_REQ 일때 사용
	memcpy(szTargetName, lpPktFriendRemoveRequest->m_szName, PktFriendAck::MAX_NAME);

    switch (lpPktFriendRemoveRequest->m_cCmd)
    {
		// edith 2009.08.28 NoFriend 추가
	case PktFriendRemoveReq::NOFRIEND_REQ:
		{
			// szTargetName 의 유저에서 나자신을 뺀다.
			cAckCmd = PktFriendAck::NOFRIEND_ACK;

			CCharacter* lpTargetCharacter = CCreatureManager::GetInstance().GetCharacter(szTargetName);
			if(lpTargetCharacter)
			{
				CFriendList& friendList = lpTargetCharacter->GetFriendList();

				CFriendList::Rebind* lpRebind = friendList.GetFriend(dwOwnerCID);
				if (NULL != lpRebind)
				{
					memcpy(szRemovedName, lpRebind->GetCharacterName(), PktFriendAck::MAX_NAME);
				}    

				if (lpTargetCharacter->GetFriendList().Remove(dwOwnerCID))
				{
					// DBAgent, Chatting으로 친구 삭제 통보 보냄.
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch,
						CChatDispatch::GetDispatchTable());

					if (NULL != lpDBAgentDispatch && NULL != lpChatDispatch)
					{
						GameClientSendPacket::SendFriendListChangeToDB(lpDBAgentDispatch->GetSendStream(),
							lpTargetCharacter->GetUID(), lpTargetCharacter->GetCID(), 0, dwOwnerCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_FRIEND);

						GameClientSendPacket::SendFriendListChangeToDB(lpChatDispatch->GetSendStream(),
							lpTargetCharacter->GetUID(), lpTargetCharacter->GetCID(), 0, dwOwnerCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_FRIEND);
					}

					// 빼야할 사람의 리스트에서 강제로 빼준다.
					GameClientSendPacket::SendCharFriendAck(lpTargetCharacter->GetDispatcher()->GetSendStream(), 
						cAckCmd, dwOwnerCID, szRemovedName, 0, 0, 0, 0, usError);

					// 되돌려줄 이름 수정.
					memcpy(szRemovedName, szTargetName, PktFriendAck::MAX_NAME);
				}
				else
				{
					usError = PktFriendAck::NOT_IN_LIST;
					ERRLOG1(g_Log, "CID:%10u 친구 리스트에 없는 캐릭터를 제거하려 하였습니다.", dwOwnerCID);
				}
			}
			else
			{
				usError = PktFriendAck::NOT_LOGINED;
				ERRLOG1(g_Log, "CID:%10u 로그아웃 되어 있거나 다른존에 있는 상대에게는 사용하실수 없습니다.", dwOwnerCID);
			}
			break;
		}

    case PktFriendRemoveReq::REMOVE_FRIEND_REQ:
        {
            cAckCmd = PktFriendAck::REMOVE_FRIEND_ACK;
            CFriendList& friendList = lpCharacter->GetFriendList();

            CFriendList::Rebind* lpRebind = friendList.GetFriend(lpPktFriendRemoveRequest->m_dwCID);
            if (NULL != lpRebind)
            {
                memcpy(szRemovedName, lpRebind->GetCharacterName(), PktFriendAck::MAX_NAME);
            }            

            if (lpCharacter->GetFriendList().Remove(lpPktFriendRemoveRequest->m_dwCID))
            {
                // DBAgent, Chatting으로 친구 삭제 통보 보냄.
				GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
					CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
				
				GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch,
					CChatDispatch::GetDispatchTable());

                if (NULL != lpDBAgentDispatch && NULL != lpChatDispatch)
                {
                    GameClientSendPacket::SendFriendListChangeToDB(lpDBAgentDispatch->GetSendStream(),
                        lpCharacter->GetUID(), dwOwnerCID, 0, lpPktFriendRemoveRequest->m_dwCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_FRIEND);

                    GameClientSendPacket::SendFriendListChangeToDB(lpChatDispatch->GetSendStream(),
                        lpCharacter->GetUID(), dwOwnerCID, 0, lpPktFriendRemoveRequest->m_dwCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_FRIEND);
                }
            }
            else
            {
                usError = PktFriendAck::NOT_IN_LIST;
                ERRLOG1(g_Log, "CID:%10u 친구 리스트에 없는 캐릭터를 제거하려 하였습니다.", dwOwnerCID);
            }
        }
        break;

    case PktFriendRemoveReq::REMOVE_BAN_REQ:
        {
            cAckCmd = PktFriendAck::REMOVE_BAN_ACK;

            CBanList& banList = lpCharacter->GetBanList();
            const char* szBanName = banList.GetBanName(lpPktFriendRemoveRequest->m_dwCID);
            if (NULL != szBanName)
            {
                memcpy(szRemovedName, szBanName, PktFriendAck::MAX_NAME);
            }

            if (lpCharacter->GetBanList().Remove(lpPktFriendRemoveRequest->m_dwCID))
            {
                // DBAgent로 거부 삭제 통보 보냄.
				GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
					CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
				
				GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch,
					CChatDispatch::GetDispatchTable());

                if (NULL != lpDBAgentDispatch && NULL != lpChatDispatch)
                {
                    GameClientSendPacket::SendFriendListChangeToDB(lpDBAgentDispatch->GetSendStream(),
                        lpCharacter->GetUID(), dwOwnerCID, 0, lpPktFriendRemoveRequest->m_dwCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_BAN);

                    GameClientSendPacket::SendFriendListChangeToDB(lpChatDispatch->GetSendStream(),
                        lpCharacter->GetUID(), dwOwnerCID, 0, lpPktFriendRemoveRequest->m_dwCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_BAN);
                }
            }
            else
            {
                usError = PktFriendAck::NOT_IN_LIST;
                ERRLOG1(g_Log, "CID:%10u 친구 리스트에 없는 캐릭터를 제거하려 하였습니다.", dwOwnerCID);
            }
        }
        break;

    default:

        usError = PktFriendAck::CLIENT_ERROR;
        ERRLOG2(g_Log, "0x%08x 잘못된 클라이언트 친구 리스트 명령입니다. %d", 
            dwOwnerCID, lpPktFriendRemoveRequest->m_cCmd);
        break;
    }

    return GameClientSendPacket::SendCharFriendAck(GameClientDispatch.GetSendStream(), 
        cAckCmd, dwOwnerCID, szRemovedName, 0, 0, 0, 0, usError);
}


// 기타 처리(그룹 설정 등)
bool GameClientParsePacket::ParseCharFriendEtc(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktFriendEtc), GameClientDispatch);
    
    PktFriendEtc*   lpPktFriendEtc  = reinterpret_cast<PktFriendEtc*>(lpPktBase);
    CCharacter*     lpCharacter     = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

    switch (lpPktFriendEtc->m_cCmd)
    {
		case PktFriendEtc::SETGROUP:
        {
            // 그룹 설정은 Ack없음.
            CFriendList::Rebind* lpRebind = lpCharacter->GetFriendList().GetFriend(lpPktFriendEtc->m_dwCID);
            if (NULL != lpRebind)
            {
                if (lpRebind->SetGroup(lpPktFriendEtc->m_dwData))
                {
                    // DBAgent로 SetGroup전송
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
                    
					GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch,
						CChatDispatch::GetDispatchTable());

                    if (NULL != lpDBAgentDispatch && NULL != lpChatDispatch)
                    {
                        GameClientSendPacket::SendFriendListChangeToDB(lpDBAgentDispatch->GetSendStream(),
                            lpCharacter->GetUID(), lpCharacter->GetCID(), 0, lpPktFriendEtc->m_dwCID, lpPktFriendEtc->m_dwData, 0, 0, 0, 0, 
                            PktFriendDB::SETGROUP);

                        GameClientSendPacket::SendFriendListChangeToDB(lpChatDispatch->GetSendStream(),
                            lpCharacter->GetUID(), lpCharacter->GetCID(), 0, lpPktFriendEtc->m_dwCID, lpPktFriendEtc->m_dwData, 0, 0, 0, 0, 
                            PktFriendDB::SETGROUP);
                    }                    
                }
            }
			break;
        }
    }

    return true;
}


using namespace Guild;

// 길드 생성
bool GameClientParsePacket::ParseCharCreateGuild(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCreateGuild), GameClientDispatch);

	PktCreateGuild*	lpPktCreateGuild	= reinterpret_cast<PktCreateGuild *>(lpPktBase);
	CCharacter*		lpCharacter			= GameClientDispatch.GetCharacter();
    CCharacter*     lpMaster            = 0;

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned short wError = PktBase::NO_SERVER_ERR;

    lpPktCreateGuild->m_szGuildName[Guild::MAX_GUILD_NAME_LEN - 1] = 0;
    if (!Guild::CheckGuildName(lpPktCreateGuild->m_szGuildName))
    {
        wError = PktCreateGuild::FAIL_INVALID_NAME;
    }
    else if (NULL == (lpMaster = CCreatureManager::GetInstance().GetCharacter(lpPktCreateGuild->m_dwCID)))
	{
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpPktCreateGuild->m_szGuildName);
		if (NULL != lpGuild)
		{
			wError = PktCreateGuild::FAIL_ALREADY_NAME;
		}
		else
		{
			// edith 2008.02.27 길드생성 레벨 제외
//			unsigned char cCreateLevel = CREATE_LEVEL;
			unsigned long dwCreateGold = Guild::ms_arySetLevelGold[0];
/*
			if (GameRYL::CHINA == CServerSetup::GetInstance().GetNationType())
			{
				cCreateLevel = CREATE_LEVEL_FOR_CHINA;
				dwCreateGold = Guild::ms_arySetLevelGoldForChina[0];
			}
*/
/*
			if (cCreateLevel > lpCharacter->GetStatus().m_nLevel)
			{
				wError = PktBase::SERVER_ERROR;
			}
			else 
*/
			if (dwCreateGold > lpCharacter->GetGold())
			{
				wError = PktBase::SERVER_ERROR;
			}
			else
			{
				GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
					CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

				if (0 == lpDBAgentDispatch)
				{
					wError = PktBase::SERVER_ERROR;
				}
				else
				{ 
                    GAMELOG::LogGuildCreate(GAMELOG::sGuildLog::REQUEST, 0, lpPktCreateGuild->m_dwCID, 0, 0);

					return GameClientSendPacket::SendCharCreateGuild(lpDBAgentDispatch->GetSendStream(), 
						lpPktCreateGuild->m_dwCID, 0, lpCharacter->GetNation(), lpPktCreateGuild->m_szGuildName, wError);
				}
			}
		}
	}

    GAMELOG::LogGuildCreate(GAMELOG::sGuildLog::REQUEST, 0, lpPktCreateGuild->m_dwCID, 0, wError);

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		return GameClientSendPacket::SendCharCreateGuild(lpDispatch->GetSendStream(), 
			lpPktCreateGuild->m_dwCID, 0, lpPktCreateGuild->m_cInclination, lpPktCreateGuild->m_szGuildName, wError);
	}

	return true;
}

// 길드 멤버 관련 명령
bool GameClientParsePacket::ParseCharGuildCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildCmd), GameClientDispatch);

	PktGuildCmd*	lpPktGuildCmd	= reinterpret_cast<PktGuildCmd *>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwGID			= lpPktGuildCmd->m_dwGID;
	unsigned long   dwSenderID		= lpPktGuildCmd->m_dwSenderID;
	unsigned long   dwReferenceID	= lpPktGuildCmd->m_dwReferenceID;
	unsigned short  wCmd			= lpPktGuildCmd->m_wCmd;

	unsigned short	wError			= PktGuildCmd::NO_SERVER_ERR;


	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL == lpGuild)
	{
		ERRLOG4(g_Log, "GID:0x%08x 존재하지 않는 길드에게 멤버 명령을 요청하였습니다. SenderID:0x%08x, ReferenceID:0x%08x, Cmd:%d", 
			dwGID, dwSenderID, dwReferenceID, wCmd);
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		switch (wCmd)
		{
			// Send : 초대 받은 캐릭터
			// Reference : 길드 멤버
			case PktGuildCmd::GC_INVITE:
			{
				lpGuild->InviteMember(dwReferenceID, dwSenderID, wError);
				break;
			}

			// Send : (초대를) 수락하는 캐릭터
			// Reference : 길드 멤버
			case PktGuildCmd::GC_JOIN:
			{
				lpGuild->JoinMember(dwSenderID, Guild::COMMON, wError);
                GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::REQUEST, 
                    lpGuild->GetGID(), dwSenderID, Guild::COMMON, wError);
				break;
			}

			// Send : (초대를) 거절하는 캐릭터
			// Reference : 길드 멤버
			case PktGuildCmd::GC_REFUSE:
			{
				CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(dwReferenceID);
				if (NULL == lpMember) { return true; }

				CGameClientDispatch* lpDispatch = lpMember->GetDispatcher();
				if (NULL != lpDispatch)
				{
					return GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
						dwGID, dwSenderID, dwReferenceID, lpCharacter->GetCharacterName(), lpGuild->GetName(), wCmd, wError);
				}

				break;
			}

			// Send : 가입 요청하는 캐릭터
			// Reference : None
			case PktGuildCmd::GC_ASK_ENTER:
			{
				lpGuild->JoinMember(dwSenderID, Guild::JOIN_WAIT, wError);
                GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::REQUEST, 
                    lpGuild->GetGID(), dwSenderID, Guild::JOIN_WAIT, wError);
				break;
			}

			// Send : 탈퇴 요청하는 멤버
			// Reference : None
			case PktGuildCmd::GC_ASK_LEAVE:
			{
				lpGuild->SetTitle(dwSenderID, dwSenderID, LEAVE_WAIT, wError);

                GAMELOG::LogGuildMemberLevelAdjust(GAMELOG::sGuildLog::REQUEST, 
                    lpGuild->GetGID(), dwSenderID, dwSenderID, LEAVE_WAIT, wError);

                GAMELOG::LogGuildLeave(GAMELOG::sGuildLog::REQUEST, 
                    lpGuild->GetGID(), dwSenderID, dwSenderID, wError);

                break;
			}

			// Send : 탈퇴 당하는 멤버
			// Reference : 탈퇴 시키는 멤버
			case PktGuildCmd::GC_KICK:	
			{
				// 길드전 시간이 아닐때만 탈퇴 가능
				if (!CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					lpGuild->KickMember(dwReferenceID, dwSenderID, wError);

					GAMELOG::LogGuildLeave(GAMELOG::sGuildLog::REQUEST, 
						lpGuild->GetGID(), dwReferenceID, dwSenderID, wError);
				}
				else
				{
					wError = PktGuildCmd::FAIL_GUILDWAR_TIME_KICK;
				}

				break;
			}

			// Send : 직위 변경되는 멤버
			// Reference : 변경 후의 직위
			case PktGuildCmd::GC_SETUP_TITLE:
			{
				// 길드전 시간이 아닐때만 탈퇴 가능
				if (CGameTimeMgr::GetInstance().IsGuildWarTime() && Guild::LEAVE_WAIT == dwReferenceID)
				{
					wError = PktGuildCmd::FAIL_GUILDWAR_TIME_KICK;
				}
				else
				{
					lpGuild->SetTitle(lpCharacter->GetCID(), dwSenderID, 
						static_cast<unsigned char>(dwReferenceID), wError);

					GAMELOG::LogGuildMemberLevelAdjust(GAMELOG::sGuildLog::REQUEST, 
						lpGuild->GetGID(), 0, dwSenderID, dwReferenceID, wError);
				}

                break;
			}
			// 용병 신청
			case PktGuildCmd::GC_TACTICS:
			{
				lpGuild->TacticsMember(dwSenderID, Guild::JOIN_WAIT, PktGuildCmd::GC_TACTICS, wError);

				GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), dwSenderID, Guild::TACTICS_WAIT, wError);

				break;
			}
			// 용병 신청 권유.
			case PktGuildCmd::GC_TACTICS_JOIN:				
			{
				// edith 2009.08.05 용병신청 권유는 막아버린다.
//				lpGuild->TacticsMember(dwSenderID, Guild::JOIN_WAIT, PktGuildCmd::GC_TACTICS_JOIN, wError);
//				GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), dwSenderID, Guild::TACTICS_WAIT, wError);
				break;
			}
			// 용병 허가.
			case PktGuildCmd::GC_TACTICS_TITLE:
			{								
				lpGuild->SetTacticsTitle(lpCharacter->GetCID(), dwSenderID, wCmd, static_cast<unsigned char>(dwReferenceID), 0, wError);						

				GAMELOG::LogGuildMemberLevelAdjust(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), 0, dwSenderID, dwReferenceID, wError);						
				
				break;
			}
			// 용병 지원.
			case PktGuildCmd::GC_TACTICS_REQUEST:
			{
				lpGuild->SetTacticsTitle(lpCharacter->GetCID(), dwSenderID, wCmd, static_cast<unsigned char>(dwReferenceID), 1, wError);						

				GAMELOG::LogGuildMemberLevelAdjust(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), 0, dwSenderID, dwReferenceID, wError);						
								
				break;
			}
			case PktGuildCmd::GC_TACTICS_KICK:
			case PktGuildCmd::GC_TACTICS_LEAVE:
			{
				// 길드전 시간이 아닐때만 탈퇴 가능
				if (!CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					lpGuild->KickTacticsMember(dwSenderID, dwReferenceID, wCmd, wError);
					GAMELOG::LogGuildLeave(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), dwReferenceID, dwSenderID, wError);
				}
				else
				{
					// 길드전 시간에 용병 탈퇴는 가능하다..
					if ( lpGuild->IsTacticsWaitMember(dwSenderID) )
					{
						lpGuild->KickTacticsMember(dwSenderID, dwReferenceID, wCmd, wError);
						GAMELOG::LogGuildLeave(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), dwReferenceID, dwSenderID, wError);
					}
					else
					{
						wError = PktGuildCmd::FAIL_GUILDWAR_TIME_KICK;
					}
				}
				break;
			}
			default:
			{
				ERRLOG4(g_Log, "GID:0x%08x 잘못된 길드 멤버 명령입니다. SenderID:0x%08x, ReferenceID:0x%08x, Cmd:%d", 
					dwGID, dwSenderID, dwReferenceID, wCmd);
				wError = PktBase::SERVER_ERROR;
				break;
			}
		}
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
				dwGID, dwSenderID, dwReferenceID, "에러", "에러", wCmd, wError);
		}
	}

	return true;
}

// 길드 마크 변경
bool GameClientParsePacket::ParseCharGuildMark(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildMark), GameClientDispatch);

	PktGuildMark*	lpPktGuildMark	= reinterpret_cast<PktGuildMark *>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCID	= lpPktGuildMark->m_dwCID;
	unsigned long	dwGID	= lpPktGuildMark->m_dwGID;

	unsigned short	wError	= PktBase::NO_SERVER_ERR;

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(dwGID);
	if (NULL == lpGuild)
	{
		ERRLOG1(g_Log, "CID:%10u 존재하지않는 길드의 마크를 변경하려 합니다.", dwGID);
		wError = PktGuildMark::FAIL_INVALID_GUILD;
	}
	else
	{            
		wError = lpGuild->SetMark(dwCID, lpPktGuildMark->m_szMark);

        GAMELOG::LogGuildMarkChange(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), 
            dwCID, lpGuild->GetGold(), lpGuild->GetGold(), 
            lpPktGuildMark->m_szMark, MAX_MARK_SIZE, wError);
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharGuildMark(lpDispatch->GetSendStream(), 
				dwCID, dwGID, lpPktGuildMark->m_szMark, 0, wError);
		}
	}

	return true;
}

// 길드 레벨 변경
bool GameClientParsePacket::ParseCharGuildLevel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildLevel), GameClientDispatch);

	PktGuildLevel*	lpPktGuildLevel	= reinterpret_cast<PktGuildLevel *>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCID	= lpPktGuildLevel->m_dwUID;
	unsigned char	cLevel	= lpPktGuildLevel->m_cLevel;

	unsigned short	wError	= PktBase::NO_SERVER_ERR;

	CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpMaster)
	{
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMaster->GetGID());
		if (NULL == lpGuild)
		{
			ERRLOG1(g_Log, "CID:%10u 길드에 속하지 않은 캐릭터가 길드 레벨을 변경하려 합니다.", dwCID);
			wError = PktBase::SERVER_ERROR;
		}
		else
		{
			if (false == lpGuild->SetLevel(dwCID, cLevel))
			{
				wError = PktBase::SERVER_ERROR;
            }

            GAMELOG::LogGuildLevel(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), dwCID, 
                cLevel, lpGuild->GetGold(), lpGuild->GetGold(), 0);
		}
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharGuildLevel(lpDispatch->GetSendStream(), 
				dwCID, cLevel, 0, wError);
		}
	}

	return true;
}

// 길드 관계 변경
bool GameClientParsePacket::ParseCharGuildRelation(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktGuildRelation), GameClientDispatch);

	PktGuildRelation*	lpPktGuildRelation	= reinterpret_cast<PktGuildRelation *>(lpPktBase);
	CCharacter*			lpCharacter			= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCID		= lpPktGuildRelation->m_dwCID;
	unsigned long	dwGID		= lpPktGuildRelation->m_dwGID;
	unsigned long	dwTargetGID	= lpPktGuildRelation->m_dwTargetGID;
	unsigned long	dwValue		= lpPktGuildRelation->m_dwValue;
	unsigned char	cSubCmd		= lpPktGuildRelation->m_cSubCmd;
	char			szTargetGuildName[Guild::MAX_GUILD_NAME_LEN] = "";
	char			szTempGuildName[Guild::MAX_GUILD_NAME_LEN] = "";

	unsigned short	wError		= PktBase::NO_SERVER_ERR;

	CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpMember)
	{
		wError = PktGuildRelation::FAIL_NOT_MEMBER;
	}
	else
	{
		if (PktGuildRelation::GR_HOSTILITY == cSubCmd &&
			GameTime::GT_NORMAL != CGameTimeMgr::GetInstance().GetCurrentGuildTime())
		{
			ERRLOG1(g_Log, "CID:%10u 적대 선언을 할 수 없는 시간에 관계 변경을 요청합니다.", dwCID);
			wError = PktGuildRelation::FAIL_NOT_HOSTILITY_TIME;
		}

		if (PktGuildRelation::GR_ALERT == cSubCmd &&
			GameTime::GT_GUILD_WAR != CGameTimeMgr::GetInstance().GetCurrentGuildTime() &&
			GameTime::GT_PREPARE_END_30 != CGameTimeMgr::GetInstance().GetCurrentGuildTime())
		{
			ERRLOG1(g_Log, "CID:%10u 경계 적대 선언을 할 수 없는 시간에 관계 변경을 요청합니다.", dwCID);
			wError = PktGuildRelation::FAIL_NOT_ALERT_TIME;
		}

		if (PktGuildRelation::GR_COUNTER == cSubCmd &&
			GameTime::GT_GUILD_WAR != CGameTimeMgr::GetInstance().GetCurrentGuildTime() &&
			GameTime::GT_PREPARE_END_30 != CGameTimeMgr::GetInstance().GetCurrentGuildTime())
		{
			ERRLOG1(g_Log, "CID:%10u 카운터 적대 선언을 할수 없는 시간에 관계 변경을 요청합니다.", dwCID);
			wError = PktGuildRelation::FAIL_NOT_COUNTER_TIME;
		}

		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMember->GetGID());
		if (NULL == lpGuild)
		{
			ERRLOG1(g_Log, "CID:%10u 길드에 속하지 않은 캐릭터가 길드 관계을 변경하려 합니다.", dwCID);
			wError = PktGuildRelation::FAIL_NOT_EXIST_GUILD;
		}
		else
		{
			if (lpGuild->GetMaster().m_dwCID != dwCID)
			{
				ERRLOG1(g_Log, "CID:%10u 길드 마스터가 아닌 캐릭터가 길드 관계를 변경하려 합니다.", dwCID);
				wError = PktGuildRelation::FAIL_NOT_MASTER;
			}
		}

		CGuild* lpTargetGuild = CGuildMgr::GetInstance().GetGuild(dwTargetGID);
		if (NULL == lpTargetGuild)
		{
			ERRLOG2(g_Log, "CID:%10u 길드 관계를 변경할려는 상대 길드가 존재하지 않습니다. GID:0x%08x", dwCID, dwTargetGID);
			wError = PktGuildRelation::FAIL_NOT_EXIST_GUILD;
		}
		else
		{
			strncpy(szTargetGuildName, lpTargetGuild->GetName(), Guild::MAX_GUILD_NAME_LEN);
		}

		if (wError == PktBase::NO_SERVER_ERR)
		{
			switch (cSubCmd)
			{
				case PktGuildRelation::GR_HOSTILITY:
				{
					// 자신의 길드에게 적대 선언 체크
					if (lpGuild->GetGID() == dwTargetGID)
					{
						ERRLOG1(g_Log, "GID:0x%08x 자신의 길드에게 적대 선언을 하려합니다.", lpGuild->GetGID());
						wError = PktGuildRelation::FAIL_SELF_GUILD;
					}

					// 이미 적대 관계인지 체크
					if (Guild::HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드와 이미 적대 관계입니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_HOSTILITY;
					}
					// 내가 적대 상태를 받은 상태인지 : 크로스 적대를 방지하기 위해서
					// 상대방이 이미 나를 적대 신청을 했으면..
					if (Guild::HOSTILITY == lpGuild->GetRelation(dwTargetGID, BY_TARGET) ||
						Guild::TARGET_NEUTRALITY == lpGuild->GetRelation(dwTargetGID, BY_TARGET) )
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드와 이미 적대 관계입니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_HOSTILITY;
					}
					
					// 적대 선언권 초과 체크
					if (lpGuild->GetNumOfRelation(Guild::HOSTILITY) >= Guild::MAX_HOSTILITY_NUM)
					{
						dwValue = lpGuild->GetRelationGID(Guild::HOSTILITY);
						CGuild* lpTempGuild = CGuildMgr::GetInstance().GetGuild(dwValue);
						if (NULL != lpTempGuild)
						{
							strncpy(szTempGuildName, lpTempGuild->GetName(), Guild::MAX_GUILD_NAME_LEN);
						}

						wError = PktGuildRelation::FAIL_REQUEST_WINDOW;
					}

					// 상대방이 받을수 있는 적대 관계 최대치 초과 체크
					if (lpTargetGuild->GetNumOfRelation(Guild::ALL_HOSTILITY, Guild::BY_TARGET) >= Guild::MAX_HOSTILITY_APPLY_NUM)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드가 받을 수 있는 적대 관계 한계치입니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_TARGET_HOSTILITY_FULL;
					}

					// 적대 선언
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB 중계 서버로 전송
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
							CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

						if (0 != lpDBAgentDispatch)
						{
							return GameClientSendPacket::SendCharGuildRelation(lpDBAgentDispatch->GetSendStream(), 
								dwCID, lpGuild->GetGID(), dwTargetGID, 0, PktGuildRelation::GR_HOSTILITY, 0);
						}
					}

					break;
				}

				case PktGuildRelation::GR_PEACE:
				{
					// 자신의 길드에게 평화 선언 체크
					if (lpGuild->GetGID() == dwTargetGID)
					{
						ERRLOG1(g_Log, "GID:0x%08x 자신의 길드에게 평화 선언을 하려합니다.", lpGuild->GetGID());
						wError = PktGuildRelation::FAIL_SELF_GUILD;
					}

					// 중립 관계의 길드에게 평화 선언 체크
					if (Guild::NEUTRALITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 중립 관계 길드에게 평화 선언을 하려 합니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_NEUTRALITY;
					}

					// 평화 선언
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB 중계 서버로 전송
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
							CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

						if (0 != lpDBAgentDispatch)
						{
							return GameClientSendPacket::SendCharGuildRelation(lpDBAgentDispatch->GetSendStream(), 
								dwCID, lpGuild->GetGID(), dwTargetGID, 0, PktGuildRelation::GR_PEACE, 0);
						}
					}

					break;
				}

				case PktGuildRelation::GR_HOSTILITYDEL:
				{
					// 자신의 길드에게 평화 선언 체크
					if (lpGuild->GetGID() == dwTargetGID)
					{
						ERRLOG1(g_Log, "GID:0x%08x 자신의 길드에게 평화 선언을 하려합니다.", lpGuild->GetGID());
						wError = PktGuildRelation::FAIL_SELF_GUILD;
					}

					// 해당 타겟길드가 들어가있지 않다면
					if (Guild::TARGET_NEUTRALITY != lpGuild->GetRelation(dwTargetGID, Guild::BY_TARGET))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 허락대기 관계 길드가 아닌데 평화 선언을 하려 합니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_NEUTRALITY;
					}

					// 평화 선언
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB 중계 서버로 전송
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
							CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

						if (0 != lpDBAgentDispatch)
						{
							return GameClientSendPacket::SendCharGuildRelation(lpDBAgentDispatch->GetSendStream(), 
								dwCID, lpGuild->GetGID(), dwTargetGID, 0, PktGuildRelation::GR_HOSTILITYDEL, 0);
						}
					}

					break;
				}
				case PktGuildRelation::GR_ALERT:
				{
					// 자신의 길드에게 경계 적대 선언 체크
					if (lpGuild->GetGID() == dwTargetGID)
					{
						ERRLOG1(g_Log, "GID:0x%08x 자신의 길드에게 경계 적대 선언을 하려합니다.", lpGuild->GetGID());
						wError = PktGuildRelation::FAIL_SELF_GUILD;
					}

					// 이미 적대 관계인지 체크
					if (Guild::HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드와 이미 적대 관계입니다.",
								lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_HOSTILITY;
					}

					// 이미 카운터 적대 관계인지 체크
					if (Guild::COUNTER_HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드와 이미 카운터 적대 관계입니다.",
								lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_COUNTER_HOSTILITY;
					}

					// 이미 경계 적대 관계인지 체크
					if (Guild::ALERT_HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드와 이미 경계 적대 관계입니다.",
								lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_ALERT_HOSTILITY;
					}

					// 경계 적대 선언권 초과 체크
					if (lpGuild->GetNumOfRelation(Guild::ALERT_HOSTILITY) >= Guild::MAX_ALERT_NUM)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 경계 적대 선언권 초과로 경계 적대 선언 실패",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_MAX_ALERT_NUM;
					}

					// 상대방이 받을수 있는 적대 관계 최대치 초과 체크
					if (lpTargetGuild->GetNumOfRelation(Guild::ALL_HOSTILITY, Guild::BY_TARGET) >= Guild::MAX_HOSTILITY_APPLY_NUM)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드가 받을 수 있는 적대 관계 한계치입니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_TARGET_HOSTILITY_FULL;
					}

					// 구축중인 요새 유무 체크
					bool bHasDevelopingCamp = false, bHasCompleteCamp = false;
					CSiegeObjectMgr::GetInstance().HasCampByGID(dwTargetGID, bHasDevelopingCamp, bHasCompleteCamp);
					if (!bHasDevelopingCamp)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드가 구축중인 길드 요새를 가지고 있지 않습니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_NOT_HAVE_DEVELOPING_CAMP;
					}

					// 경계 적대 선언
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB 중계 서버로 전송
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
							CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

						if (0 != lpDBAgentDispatch)
						{
							return GameClientSendPacket::SendCharGuildRelation(lpDBAgentDispatch->GetSendStream(), 
								dwCID, lpGuild->GetGID(), dwTargetGID, 0, PktGuildRelation::GR_ALERT, 0);
						}
					}

					break;
				}

				case PktGuildRelation::GR_COUNTER:
				{
					// 이미 적대 관계인지 체크
					if (Guild::HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드와 이미 적대 관계입니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_HOSTILITY;
					}

					// 이미 카운터 적대 관계인지 체크
					if (Guild::COUNTER_HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드와 이미 카운터 적대 관계입니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_COUNTER_HOSTILITY;
					}

					// 대상 길드가 나에게 경계 적대 선언을 하였는지 체크
					if (Guild::ALERT_HOSTILITY != lpTargetGuild->GetRelation(lpGuild->GetGID()))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드가 경계 적대를 신청하지 않았는데 카운터 적대 신청을 하려합니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_NOT_HAVE_ALERT;
					}

					// 상대방이 받을수 있는 적대 관계 최대치 초과 체크
					if (lpTargetGuild->GetNumOfRelation(Guild::ALL_HOSTILITY, Guild::BY_TARGET) >= Guild::MAX_HOSTILITY_APPLY_NUM)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x 대상 길드가 받을 수 있는 적대 관계 한계치입니다.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_TARGET_HOSTILITY_FULL;
					}

					// 카운터 적대 선언
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB 중계 서버로 전송
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
							CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

						if (0 != lpDBAgentDispatch)
						{
							return GameClientSendPacket::SendCharGuildRelation(lpDBAgentDispatch->GetSendStream(), 
								dwCID, lpGuild->GetGID(), dwTargetGID, 0, PktGuildRelation::GR_COUNTER, 0);
						}
					}

					break;
				}

				case PktGuildRelation::GR_REQUEST:
				{
					// 지금 적대 선언중인 길드와 평화 선언후 대상 길드와 적대 선언
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB 중계 서버로 전송
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
							CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

						if (0 != lpDBAgentDispatch)
						{
							return GameClientSendPacket::SendCharGuildRelation(lpDBAgentDispatch->GetSendStream(), 
								dwCID, dwGID, dwTargetGID, dwValue, PktGuildRelation::GR_REQUEST, 0);
						}
					}
					break;
				}
			}
		}
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			if (PktGuildRelation::FAIL_REQUEST_WINDOW == wError)
			{
				return GameClientSendPacket::SendCharGuildRelation(lpDispatch->GetSendStream(), 
					dwCID, dwGID, dwTargetGID, dwValue, cSubCmd, wError, szTargetGuildName, szTempGuildName);
			}
			else
			{
				return GameClientSendPacket::SendCharGuildRelation(lpDispatch->GetSendStream(), 
					dwCID, dwGID, dwTargetGID, dwValue, cSubCmd, wError);
			}
		}
	}

	return true;
}


// 길드 성향 변경
bool GameClientParsePacket::ParseCharGuildInclination(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
// 더이상 길드 국적은 변경할 수 없습니다. (2004-11-22 by 로딘)
/*
	PktGuildInclination*	lpPktGuildInclination	= reinterpret_cast<PktGuildInclination *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCID			= lpPktGuildInclination->m_dwUID;
	unsigned char	cInclination	= lpPktGuildInclination->m_cInclination;

	unsigned short	wError			= PktBase::NO_SERVER_ERR;

	CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpMember)
	{
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMember->GetGID());
		if (NULL == lpGuild)
		{
			ERRLOG1(g_Log, "CID:%10u 길드에 속하지 않은 캐릭터가 길드 성향을 변경하려 합니다.", dwCID);
			wError = PktBase::SERVER_ERROR;
		}
		else
		{
			lpGuild->SetInclination(dwCID, cInclination, wError);
		}
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharGuildInclination(lpDispatch->GetSendStream(), 
				dwCID, cInclination, wError);
		}
	}
*/

	return true;
}

// 길드 리스트
bool GameClientParsePacket::ParseCharGuildList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktGuildList*	lpPktGuildList	= reinterpret_cast<PktGuildList *>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long 	dwCID		= lpPktGuildList->m_dwCID;	
	unsigned char 	cSortCmd	= lpPktGuildList->m_cSortCmd;
	unsigned char 	cPage		= lpPktGuildList->m_cPage;
	unsigned char 	cNodeNum	= lpPktGuildList->m_cSmallNodeNum;

	GuildCheckSumNode* lpNode = reinterpret_cast<GuildCheckSumNode *>(lpPktGuildList + 1);
	return CGuildMgr::GetInstance().SendGuildList(dwCID, cSortCmd, cPage, cNodeNum, lpNode);
}

// 길드 권한 설정
bool GameClientParsePacket::ParseCharGuildRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktGuildRight*	lpPktGuildRight	= reinterpret_cast<PktGuildRight *>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCID		= lpPktGuildRight->m_dwUID;	
	GuildRight guildRight	= lpPktGuildRight->m_GuildRight;

	unsigned short	wError	= PktBase::NO_SERVER_ERR;

	CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpMaster)
	{
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMaster->GetGID());
		if (NULL == lpGuild)
		{
			ERRLOG1(g_Log, "CID:%10u 길드에 속하지 않은 캐릭터가 길드 레벨을 변경하려 합니다.", dwCID);
			wError = PktBase::SERVER_ERROR;
		}
		else
		{
			if (false == lpGuild->SetRight(dwCID, guildRight))
			{
				wError = PktBase::SERVER_ERROR;
			}

            GAMELOG::LogGuildRightsChange(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(),
                dwCID, reinterpret_cast<const char*>(guildRight.m_aryRight), MAX_RIGHT_SIZE, wError);
		}
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharGuildRight(lpDispatch->GetSendStream(), 
				dwCID, guildRight, wError);
		}
	}

	return true;
}

// 길드 멤버 리스트
bool GameClientParsePacket::ParseCharGuildMemberList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktGuildMemberList*	lpPktGuildMemberList	= reinterpret_cast<PktGuildMemberList *>(lpPktBase);
	CCharacter*			lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCID			= lpPktGuildMemberList->m_dwCID;
	unsigned char cMemberType	= lpPktGuildMemberList->m_cMemberType;
	unsigned char cSortCmd		= lpPktGuildMemberList->m_cSortCmd;
	unsigned char cPage			= lpPktGuildMemberList->m_cPage;

	CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL != lpMember)
	{
		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMember->GetGID());
		if (NULL != lpGuild)
		{
			lpGuild->SendMemberList(lpMember, cMemberType, cSortCmd, cPage);
		}
	}

	return true;
}

// 길드 금고
bool GameClientParsePacket::ParseCharGuildSafe(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	// edith 2008.07.07 길드금고 버그로 인해 일시적으로 길드금고를 제한함.
//	return true;

	PktGuildSafe*	lpPktGuildSafe	= reinterpret_cast<PktGuildSafe *>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCID		= lpPktGuildSafe->m_dwCID;
	unsigned long dwGold	= lpPktGuildSafe->m_dwGold;
	unsigned char cCmd		= lpPktGuildSafe->m_cCmd;

	unsigned short wError	= PktBase::NO_SERVER_ERR;

	CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL != lpMember)
	{
		unsigned long dwCharGold = lpMember->GetGold();

		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMember->GetGID());

		// edith 2008.03.15 캐릭이 DB에 입출금 관련 패킷을 보냈는지 확인
		if (NULL == lpGuild)
		{
			wError = PktBase::SERVER_ERROR;
		}
		else if(lpMember->GetGuildSafe() != -1 || lpGuild->GetGuildSafe() != -1)
		{
			wError = PktGuildSafe::FAIL_REQUEST_DATA;
		}
		else
		{
            unsigned long dwPostChangeGold = 0;

			// dwCharGold : 패킷을 보낸 대상의 돈
			// dwGold : 클라이언트가 보내온 금액
			lpMember->SetGuildSafe(cCmd);
			lpGuild->SetGuildSafe(cCmd);

			switch (cCmd)
			{
				// 출금
				case PktGuildSafe::SC_WITHDRAWAL:
				{
					if (lpGuild->GetTitle(lpMember->GetCID()) > lpGuild->GetRight().m_aryRight[GuildRight::USE_SAFE])
					{
						ERRLOG2(g_Log, "CID:%10u 길드 금고 사용 권한이 없는 길드원이 금고에서 출금하려 합니다. GID:0x%08x", 
							lpMember->GetCID(), lpGuild->GetGID());
						wError = PktBase::SERVER_ERROR;
					}
					else
					{
						// 길드에서 돈을 출금했다.
						if (dwCharGold > ULONG_MAX - dwGold)
						{
							//캐릭터가 최대 갖을수 있는돈 4,294,967,295이 넘는지 확인.
							wError = PktBase::SERVER_ERROR;
						}
                        else
                        {
                            // 로그용. (창고에서 돈 빼고 남은 금액 계산)
							// 길드에 있는 금액보다 꺼낸금액이 크면
                            if (dwGold <= lpGuild->GetGold())
                            {
                                dwPostChangeGold = lpGuild->GetGold() - dwGold;
                            }
							else
							{
								dwPostChangeGold = 0;
								wError = PktBase::SERVER_ERROR;
							}
                        }
					}
					break;
				}

				case PktGuildSafe::SC_DEPOSIT:
				{
					// 입금은 금고 사용 권한 없이 가능
					if (dwGold > dwCharGold)
					{
						wError = PktBase::SERVER_ERROR;
					}
                    else
                    {
						dwPostChangeGold = lpGuild->GetGold() + dwGold;
                    }
					break;
				}

				case PktGuildSafe::SC_REFER:
					// TODO : 금고 사용 내역 조회
					break;

				case PktGuildSafe::SC_RELEASE:
				{
					if (lpGuild->GetTitle(lpMember->GetCID()) > lpGuild->GetRight().m_aryRight[GuildRight::USE_SAFE])
					{
						ERRLOG2(g_Log, "CID:%10u 길드 금고 사용 권한이 없는 길드원이 금고 방출을 하려 합니다. GID:0x%08x", 
							lpMember->GetCID(), lpGuild->GetGID());
						wError = PktBase::SERVER_ERROR;
					}
					// 중계 서버에서 처리한다.
					break;
				}

				default:
					wError = PktBase::SERVER_ERROR;
					break;
			}

            GAMELOG::LogGuildStoreGoldChange(GAMELOG::sGuildLog::REQUEST, 
                lpGuild->GetGID(), dwCID, cCmd, lpGuild->GetGold(), dwPostChangeGold, wError);

			if (PktBase::NO_SERVER_ERR == wError)
			{
				// 정상 처리
				GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
					CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

				if (0 != lpDBAgentDispatch)				
				{
					return GameClientSendPacket::SendCharGuildSafe(lpDBAgentDispatch->GetSendStream(), 
						dwCID, lpGuild->GetGID(), 0, dwGold, cCmd, lpMember->GetCharacterName(), wError);
				}
			}
			else
			{
				// dwCharGold : 패킷을 보낸 대상의 돈
				// dwGold : 클라이언트가 보내온 금액
				lpMember->SetGuildSafe(-1);
				lpGuild->SetGuildSafe(-1);
			}
		}

		// 에러가 난 경우
		CGameClientDispatch* lpDispatch = lpMember->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharGuildSafe(lpDispatch->GetSendStream(), 
				dwCID, 0, 0, dwGold, cCmd, lpMember->GetCharacterName(), wError);
		}
	}

	return true;
}

// 적대 길드 형황 리스트
bool GameClientParsePacket::ParseCharGuildHostilityList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktGuildHostilityList*	lpPktGuildHostilityList = reinterpret_cast<PktGuildHostilityList *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCID			= lpPktGuildHostilityList->m_dwCID;			// 캐릭터 아이디
	unsigned char	cCurrentPage	= lpPktGuildHostilityList->m_cCurrentPage;	// 현재 페이지
	unsigned char	cPageState		= lpPktGuildHostilityList->m_cPageState;	// 요청한 페이지의 상태

	CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL != lpMember)
	{
		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMember->GetGID());
		if (NULL != lpGuild)
		{
			lpGuild->SendHostilityList(lpMember, cCurrentPage, cPageState);
		}
	}

	return true;
}

// 길드의 관계 리스트 요청
bool GameClientParsePacket::ParseCharGuildRelationInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktGuildRelationInfo*	lpPktGuildRelationInfo	= reinterpret_cast<PktGuildRelationInfo *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCID	= lpPktGuildRelationInfo->m_dwCID;		// 요청한 캐릭터 아이디
	unsigned long	dwGID	= lpPktGuildRelationInfo->m_dwGID;		// 요청한 캐릭터의 길드 아이디

	CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL != lpMember)
	{
		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMember->GetGID());
		if (NULL != lpGuild)
		{
			CGuildMgr::GetInstance().SendGuildRelationInfo(dwCID, dwGID);
		}
	}

	return true;
}
