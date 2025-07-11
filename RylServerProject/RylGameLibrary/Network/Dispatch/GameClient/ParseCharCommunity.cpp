
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

	// ĳ���� ���
    CCreatureManager& CreatureManager = CCreatureManager::GetInstance();

	CCharacter* lpSenderCharacter = CreatureManager.GetCharacter(dwSenderID); 
	CCharacter* lpRecverCharacter = CreatureManager.GetCharacter(dwRecverID); 

	if (NULL == lpSenderCharacter || NULL == lpRecverCharacter)
	{	
        SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::SERVER_ERROR);
		ERRLOG1(g_Log, "CID:%10u ĳ���� ��� ����.", dwRecverID);	
		return true;
	}

	// Dispatcher���
	CGameClientDispatch* lpSenderDispatch = lpSenderCharacter->GetDispatcher();
    CGameClientDispatch* lpRecverDispatch = lpRecverCharacter->GetDispatcher();

	if (NULL == lpSenderDispatch || NULL == lpRecverDispatch)
	{	
		SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::SERVER_ERROR);
		ERRLOG1(g_Log, "CID:%10u ���� ���� ��� ����.", dwRecverID);	
		return true;	
	}

    CSendStream& SenderSendStream = lpSenderDispatch->GetSendStream();
    CSendStream& RecverSendStream = lpRecverDispatch->GetSendStream();

    Item::CExchangeContainer& SenderExchageContainer = lpSenderCharacter->GetExchange();    
    Item::CExchangeContainer& RecverExchageContainer = lpRecverCharacter->GetExchange();
    
	switch (cCmd)
	{
		case PktExC::EXC_PROPOSE:		// ����
		{
			if (NULL != SenderExchageContainer.GetExchangeCharacter() || 
				NULL != RecverExchageContainer.GetExchangeCharacter())
			{
                SendCharExchangeCmd(SendStream, 
                    dwSenderID, dwRecverID, cCmd, PktExC::FAIL_EXCHANGING_CHAR);
				break;	
			}

            // Changed by Sparrowhawk ( 2003/04/14 14��47��)
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

		case PktExC::EXC_ACCEPT:		// ����
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

		case PktExC::EXC_REFUSE:		// ����		
		{
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;		

		case PktExC::EXC_LOCK:			// ���ϱ�
		{
			SenderExchageContainer.SetLock(true);

			SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;

		case PktExC::EXC_UNLOCK:		// �� ����
		{
			if (SenderExchageContainer.GetAccept())
			{
				SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::SERVER_ERROR);
				ERRLOG1(g_Log, "CID:%10u Ȯ�� ��ư�� ���� ���¿��� ���� �����Ϸ� ��.", dwSenderID);	
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

		case PktExC::EXC_OK:			// Ȯ��
		{
			if (false == SenderExchageContainer.GetLock() || false == RecverExchageContainer.GetLock())
			{
				SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::SERVER_ERROR);
				ERRLOG1(g_Log, "CID:%10u ������ ���� ���¿��� Ȯ���Ϸ� ��.", dwSenderID);	
				break;	
			}

			SenderExchageContainer.SetAccept(true);

			SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;		

		case PktExC::EXC_CANCEL:		// Ȯ�� ���
		{
			SenderExchageContainer.ExchangeCancel();

		} break;	

		case PktExC::EXC_EXCHANGE:		// �ŷ� ����
		{
			if (!RecverExchageContainer.GetAccept())
			{	
				SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::FAIL_NOT_ALL_OK);

				ERRLOG2(g_Log, "���� ��� Ȯ������ ���� ���¿��� �ŷ� ���� ��û. dwSenderID: 0x%08x, dwRecverID: 0x%08x", 
					dwSenderID, dwRecverID);	
				break;	
			}

			// �����Ҷ��� ĳ���Ͷ� ������ üũ 
			if (lpSenderCharacter->GetExchangeID() != dwRecverID || lpRecverCharacter->GetExchangeID() != dwSenderID)
			{
				// Rodin : Ŭ���̾�Ʈ���� ���� �޼��� ó���� �����ϴ�. 
				//		�׸��� ���� ĳ���Ϳ� ExchangeID�� �ּ� ���� �ʿ䰡 �������?
				//		ExchangeContainer�� ExchangeCharacter�� �̿��ؼ� üũ�ؼ� ������ �� �ѵ�...

				SendCharExchangeCmd(SendStream, dwSenderID, dwRecverID, cCmd, PktExC::FAIL_EXCHANGING_ID);
				ERRLOG2(g_Log, "�ŷ� ���� ��û�� �� ĳ���Ͱ� �����Ҷ��� ĳ���Ͷ� �ٸ���. dwSenderID: 0x%08x, dwRecverID: 0x%08x", 
					dwSenderID, dwRecverID);	
				break;	
			}

			lpSenderCharacter->SetExchangeID(0);
			lpRecverCharacter->SetExchangeID(0);

			SenderExchageContainer.SetAccept(true);
			if (!SenderExchageContainer.ExchangeOK(true)) 
            {
				ERRLOG2(g_Log, "�ŷ� ���°� ���� ĳ�����Դϴ�. dwSenderID : 0x%08x, dwRecverID : 0x%08x", 
                    dwSenderID, dwRecverID);	
			}

			SendCharExchangeCmd(SenderSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);
			SendCharExchangeCmd(RecverSendStream, dwSenderID, dwRecverID, cCmd, PktExC::NO_SERVER_ERR);

		} break;		

		case PktExC::EXC_QUIT:			// �ŷ� ���
		{
			if (!SenderExchageContainer.ExchangeOK(false))
            {
				ERRLOG2(g_Log, "�ŷ� ��밡 ���� ĳ�����Դϴ�. dwSenderID : 0x%08x, dwRecverID : 0x%08x", 
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
        ERRLOG0(g_Log, "��Ŷ ���̰� ���� �ʽ��ϴ�.");
        return false;
    }

	PktPC*          lpPktPC         = static_cast<PktPC*>(lpPktBase);
	unsigned long   dwPartyID       = lpPktPC->m_dwPartyID;
	unsigned long   dwSenderID	    = lpPktPC->m_dwSenderID;
	unsigned long   dwReferenceID	= lpPktPC->m_dwReferenceID;

    unsigned short  usCmd           = lpPktPC->m_wCmd;
    unsigned short  usError         = 0;

    // Ư���� szSenderName�� �ʿ���� ��쿡 ���� �����Ⱚ
    char*           szSenderName	= "�ε�";	
    CSendStream&    SendStream      = GameClientDispatch.GetSendStream();
	
    AddressInfo     SenderAddress;

    using namespace GameClientSendPacket;

	switch (usCmd)
	{
		case PktPC::PC_INVITE:		// �ʴ�
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
            CGameClientDispatch* lpRecvGameClientDispatch = NULL;
			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u ��û�� ĳ���� ���̵� Ʋ�� CID:%10u", lpSendCharacter->GetCID(), dwSenderID);					
			}
            else 
            {
			    CCharacter* lpRecvCharacter = CCreatureManager::GetInstance().GetCharacter(dwReferenceID);                 
			    if (NULL == lpRecvCharacter)
			    {	
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u ĳ���� ��� ����.", dwReferenceID);	
			    }
                else if (lpRecvCharacter->GetRejectOption().Reject.m_Party ||
                    lpRecvCharacter->GetBanList().IsBan(lpSendCharacter->GetCID(), lpSendCharacter->GetCharacterName()))
			    {
                    // �ź� �ɼ� üũ
                    usError = PktPC::FAIL_REJECT;
			    }
                else if (NULL != lpRecvCharacter->GetParty())
			    {   
                    usError = PktPC::CHAR_JOINED;
				    ERRLOG1(g_Log, "CID:%10u �̹� ��Ƽ�� ���ԵǾ� �ֽ��ϴ�.", dwReferenceID);				    
			    }
                else
                {
                    lpRecvGameClientDispatch = lpRecvCharacter->GetDispatcher();
			        if (NULL == lpRecvGameClientDispatch)
			        {	
                        usError = PktPC::FAIL_INVITE;
				        ERRLOG1(g_Log, "CID:%10u �̹� �α׾ƿ��� ��뿡�� ��Ƽ ���� ��û�Ͽ����ϴ�.", dwReferenceID);	
			        }
                }
            }

            SendCharPartyCmd((0 == usError) ? lpRecvGameClientDispatch->GetSendStream() : SendStream,
                dwPartyID, SenderAddress, (0 == usError) ? lpSendCharacter->GetCharacterName() : szSenderName,
                dwSenderID, dwReferenceID, usCmd, usError);

		} break;		

		case PktPC::PC_ACCEPT:		// ��Ƽ ����	
		{	
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());	
				
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u ��û�� ĳ���� ���̵� Ʋ�� CID:%10u", lpSendCharacter->GetCID(), dwSenderID);
			}
            else if (NULL != lpSendCharacter->GetParty())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:%10u �̹� ���Ե� ĳ���Ͱ� ������ �ߺ� ����", dwSenderID);					
			}
            else
            {
			    CCharacter* lpRecvCharacter = CCreatureManager::GetInstance().GetCharacter(dwReferenceID); 
			    if (NULL == lpRecvCharacter)
			    {	
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u ĳ���� ��� ����.", dwReferenceID);	
			    }
                else
                {
			        CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpRecvCharacter->GetParty());
			        if (NULL == lpParty)
			        {
				        // ��Ƽ ����

						GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
							CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

						if (0 == lpDBAgentDispatch)
                        {
                            usError = PktPC::SERVER_ERROR;
					        ERRLOG0(g_Log, "������Ʈ ��� ����.");	
                        }
                        else
                        {
							PARTYLOG(DETLOG2(g_Log, "��Ƽ �α�(���� ��û) - PID:0x00000000 ����:0x%08x ������:0x%08x", 
								dwReferenceID, dwSenderID));

							// ĳ���� ����(Ŭ����, ����) //

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
					        ERRLOG0(g_Log, "��Ƽ �ο� �ʰ�.");						    
				        }
                        else
                        {
							PARTYLOG(DETLOG2(g_Log, "��Ƽ �α�(���� ��û) - PID:0x%08x ������:0x%08x", lpParty->GetUID(), dwSenderID));							

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

		case PktPC::PC_REFUSE:		// ����		
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
            CCharacter* lpRecvCharacter = NULL;
            CGameClientDispatch* lpRecvGameClientDispatch = NULL;

			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u ��û�� ĳ���� ���̵� Ʋ�� CID:%10u", lpSendCharacter->GetCID(), dwSenderID);
			}
            else
            {
				lpRecvCharacter = CCreatureManager::GetInstance().GetCharacter(dwReferenceID); 
			    if (NULL == lpRecvCharacter)
			    {	
                    usError = PktPC::SERVER_ERROR;				    
				    ERRLOG1(g_Log, "CID:%10u ĳ���� ��� ����.", dwReferenceID);				    
			    }
                else
                {
                    // ���� ���� ���
			        lpRecvGameClientDispatch = lpRecvCharacter->GetDispatcher();
			        if (0 == lpRecvGameClientDispatch)
			        {	
                        usError = PktPC::SERVER_ERROR;
				        ERRLOG1(g_Log, "CID:%10u ���� ���� ��� ����.", dwReferenceID);	
			        }
                }
            }

            SendCharPartyCmd((0 == usError) ? lpRecvGameClientDispatch->GetSendStream() : SendStream,
                dwPartyID, SenderAddress, (0 == usError) ? lpSendCharacter->GetCharacterName() : NULL, 
                dwSenderID, dwReferenceID, usCmd, usError);
	
		} break;		

		case PktPC::PC_BANISH:		// ����
		{
			CCharacter* lpRecvCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpRecvCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
                ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());					
			}
            else if (dwReferenceID != lpRecvCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u ��û�� ĳ���� ���̵� Ʋ�� CID:%10u", lpRecvCharacter->GetCID(), dwReferenceID);					
			}
            else
            {
                CCharacterParty* lpRecvParty = reinterpret_cast<CCharacterParty*>(lpRecvCharacter->GetParty());
			    if (NULL == lpRecvParty)
			    {
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u ��Ƽ ��� ����.", dwReferenceID);					    
			    }
				else
				{
					if (false == lpRecvParty->IsMember(dwSenderID))
					{
						usError = PktPC::SERVER_ERROR;
						ERRLOG2(g_Log, "CID:%10u ��Ƽ���� �ƴ� ����� Ż�� ��Ű�� �ϰ� ����. Ż����:0x%08x", dwReferenceID, dwSenderID);
					}
					else
					{
						if (false == lpRecvParty->IsLeader(dwReferenceID))
						{
							usError = PktPC::SERVER_ERROR;
							ERRLOG2(g_Log, "CID:%10u ������ �ƴ� ĳ���Ͱ� Ż�� ��û. Ż����:0x%08x", dwReferenceID, dwSenderID);
						}
						else
						{                    
							PARTYLOG(DETLOG3(g_Log, "��Ƽ �α�(ű ��û) - PID:0x%08x Ż����:0x%08x ����:0x%08x", 
								lpRecvParty->GetUID(), dwSenderID, dwReferenceID));
							lpRecvParty->SendPartyMemberDataToDBAgent(dwSenderID, dwReferenceID, 0, 0, 0, "ű", PktDD::SCmdDeletePartyMem);
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

		case PktPC::PC_SECESSION:	// Ż��
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u ��û�� ĳ���� ���̵� Ʋ�� CID:%10u", lpSendCharacter->GetCID(), dwSenderID);
			}
            else 
            {
			    CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpSendCharacter->GetParty());
			    if (NULL == lpParty)
			    {
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u ��Ƽ ��� ����.", dwSenderID);
			    }
                else
                {
					PARTYLOG(DETLOG2(g_Log, "��Ƽ �α�(Ż�� ��û) - PID:0x%08x Ż����:0x%08x", lpParty->GetUID(), dwSenderID));
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

		case PktPC::PC_TRANSFER:	// ���� �絵
		{
			CCharacter* lpRecvCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpRecvCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());
			}
            else if (dwReferenceID != lpRecvCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u ��û�� ĳ���� ���̵� Ʋ�� CID:%10u", lpRecvCharacter->GetCID(), dwReferenceID);
			}
            else
            {
                CCharacterParty* lpRecvParty = reinterpret_cast<CCharacterParty*>(lpRecvCharacter->GetParty());
			    if (NULL == lpRecvParty)
			    {
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u ��Ƽ ��� ����.", dwReferenceID);					            
			    }
				else
				{                      
					if (false == lpRecvParty->IsMember(dwSenderID) || false == lpRecvParty->IsLeader(dwReferenceID))
					{
						usError = PktPC::SERVER_ERROR;
						ERRLOG2(g_Log, "Reference�� ������ �ƴϰų� Sender�� ��Ƽ���� �ƴմϴ�. SenderID:0x%08x, ReferenceID:0x%08x", 
							dwSenderID, dwReferenceID);					            
					}
					else
					{
						PARTYLOG(DETLOG3(g_Log, "��Ƽ �α�(�����絵 ��û) - PID:0x%08x ������:0x%08x ��������:0x%08x", 
							lpRecvParty->GetUID(), dwSenderID, dwReferenceID));
						lpRecvParty->SendPartyMemberDataToDBAgent(dwSenderID, dwReferenceID, 0, 0, 0, "�����絵", PktDD::SCmdTransferLeader);
					}
				}
            }

            if (0 != usError)
            {
                SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, 
                    szSenderName, dwSenderID, dwReferenceID, usCmd, usError);
            }
	
		} break;

		case PktPC::PC_DESTROY:		// �ı�
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (0 == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;
				ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());					
			}
            else if (dwSenderID != lpSendCharacter->GetCID())
			{
                usError = PktPC::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u ��û�� ĳ���� ���̵� Ʋ�� CID:%10u", lpSendCharacter->GetCID(), dwSenderID);
			}
            else
            {
                CParty* lpParty = lpSendCharacter->GetParty();
			    if (NULL == lpParty)
			    {
                    usError = PktPC::SERVER_ERROR;
				    ERRLOG1(g_Log, "CID:%10u ��Ƽ ��� ����.", dwSenderID);					    
			    }
                else if (lpParty->IsLeader(dwSenderID))
			    {          		    
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					if (0 == lpDBAgentDispatch)
                    {
                        usError = PktPC::SERVER_ERROR;
                        ERRLOG0(g_Log, "������Ʈ ��� ����.");                            
                    }
                    else
                    {
						PARTYLOG(DETLOG1(g_Log, "��Ƽ �α�(��ü ��û) - PID:0x%08x", lpParty->GetUID()));
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

		case PktPC::PC_FINDPARTY:	// ��Ƽ ã��
		{
			if (0 != dwPartyID)
			{
				usError = PktPC::ALREADY_PARTY;
				ERRLOG1(g_Log, "CID:%10u ��Ƽ�� ���Ե� �༮�� ��Ƽ ã�⿡ ����Ϸ� �մϴ�.", dwSenderID);					
			}
            else if (0 == dwReferenceID) 
			{
				if (false == CPartyMgr::GetInstance().DeleteFindPartyList(dwSenderID))
				{
                    usError = PktPC::SERVER_ERROR;
					ERRLOG1(g_Log, "CID:%10u ��Ƽ ã�� ����Ʈ���� ���� ����.", dwSenderID);						
				}
			} 
			else 
			{
				if (false == CPartyMgr::GetInstance().AddFindPartyList(dwSenderID))
				{
                    usError = PktPC::SERVER_ERROR;					
					ERRLOG1(g_Log, "CID:%10u ��Ƽ ã�� ����Ʈ�� �߰� ����.", dwSenderID);						
				}
			}

			SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, szSenderName, 
                dwSenderID, dwReferenceID, usCmd, usError);

		} break;

		case PktPC::PC_FINDMEMBER:	// ��Ƽ�� ã��
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpSendCharacter)
			{	
                usError = PktPC::SERVER_ERROR;				
				ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());
			}
            else
            {
                CParty* lpParty = lpSendCharacter->GetParty();
			    if (NULL == lpParty)
			    {
					usError = PktPC::FAIL_NOT_PARTY;
				    ERRLOG1(g_Log, "CID:%10u ��Ƽ ��� ����.", lpSendCharacter->GetCID());
			    }
                else if (false == lpParty->IsLeader(dwSenderID))
			    {
                    usError = PktPC::FAIL_NOT_LEADER;
				    ERRLOG1(g_Log, "CID:%10u ������ �ƴ� ���� ��Ƽ ��� ã�⿡ ����Ϸ� �մϴ�.", dwSenderID);
			    }
                else if (0 == dwReferenceID) 
			    {
				    if (false == CPartyMgr::GetInstance().DeleteFindMemberList(dwPartyID))
				    {
                        usError = PktPC::SERVER_ERROR;
					    ERRLOG1(g_Log, "CID:%10u ��Ƽ�� ã�� ����Ʈ���� ���� ����.", dwSenderID);
				    }
			    } 
			    else 
			    {
				    if (false == CPartyMgr::GetInstance().AddFindMemberList(dwPartyID))
				    {
                        usError = PktPC::SERVER_ERROR;					    
					    ERRLOG1(g_Log, "CID:%10u ��Ƽ�� ã�� ����Ʈ�� �߰� ����.", dwSenderID);
				    }
			    }	
            }

			SendCharPartyCmd(SendStream, dwPartyID, SenderAddress, 
                szSenderName, dwSenderID, dwReferenceID, usCmd, usError);

		} break;

		case PktPC::PC_AUTOROUTING_ON:		// ������� �ѱ�
		case PktPC::PC_AUTOROUTING_OFF:		// ������� ����
		{
			CCharacter* lpSendCharacter = GameClientDispatch.GetCharacter();
			if (NULL == lpSendCharacter)
			{	
				usError = PktPC::SERVER_ERROR;				
				ERRLOG1(g_Log, "UID:0x%08x ĳ���� ��� ����.", GameClientDispatch.GetUID());
			}
			else
			{
				CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpSendCharacter->GetParty());
				if (NULL == lpParty)
				{
					usError = PktPC::SERVER_ERROR;
					ERRLOG1(g_Log, "CID:%10u ��Ƽ ��� ����.", lpSendCharacter->GetCID());
				}
				else if (false == lpParty->IsLeader(lpSendCharacter->GetCID()))
				{
					usError = PktPC::SERVER_ERROR;				    
					ERRLOG1(g_Log, "CID:%10u ������ �ƴ� ���� ��������� �����Ϸ� �մϴ�.", dwSenderID);
				}
				else 
				{
					PARTYLOG(DETLOG3(g_Log, "��Ƽ �α�(������� �ѱ�/����) CID�� 0�̸� ALL- PID:0x%08x CID:%10u On:%d", 
						lpParty->GetUID(), dwSenderID, ((PktPC::PC_AUTOROUTING_ON == usCmd) ? true : false)));

					if (usCmd == PktPC::PC_AUTOROUTING_ON)
					{
						lpParty->SendPartyMemberDataToDBAgent(dwSenderID, dwReferenceID, 0, 0, 0, "�������", PktDD::SCmdAutoRoutingOn);
					}
					else
					{
						lpParty->SendPartyMemberDataToDBAgent(dwSenderID, dwReferenceID, 0, 0, 0, "�������", PktDD::SCmdAutoRoutingOff);
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

	// ��Ƽ ã��
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
			ERRLOG1(g_Log, "CID:%10u �̹� �������� ���� �����Դϴ�.", dwCharID);
		}
	}
	else
	{
		if (false == lpCharacter->GetStall().Open(strStallName))
		{
			ERRLOG1(g_Log, "CID:%10u �̹� �������� ���� �����Դϴ�.", dwCharID);
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
                ERRLOG1(g_Log, "CID:%10u �������� ������ �߸� �÷� ���ҽ��ϴ�."
                    "RegisterPacket�� DstPos�� Stall�� �ƴմϴ�.", lpCharacter->GetCID());
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
				ERRLOG2(g_Log, "CID:%10u ������ ��� ���� : ��� ��� ������ �ƴմϴ�. ShopID:%10u", 
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
				ERRLOG2(g_Log, "CID:%10u ������ ��� ��� ���� : ��� ��� ������ �ƴմϴ�. ShopID:%10u", 
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

	// ������α� : ������ �ø��� �� ������ (TakeItem�� ���� ��������...)
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
		ERRLOG0(g_Log, "ĳ���� ��� ����.");
		usError = PktStE::SERVER_ERROR;
	}
	else
	{
		// �̹� ������ �� �ִ����� Ȯ���Ѵ�.
		unsigned long dwOtherOwner = lpCustomer->GetStall().GetOtherOwner();

		if (0 == dwOwnerID)
		{
			// �������� ������ ���
			Item::CShopContainer* lpContainer = CCreatureManager::GetInstance().GetShopContainer(dwOtherOwner);
			if (NULL == lpContainer)
			{
				ERRLOG2(g_Log, "CID:%10u ������ ���� : �������� ������(%10u)�� ���̵� �̻��մϴ�.", 
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
					ERRLOG1(g_Log, "CID:%10u ������ ���� : �̹� �������� ���� �����Դϴ�.", lpCustomer->GetCID());
					usError = PktStE::FAIL_LEAVE;
				}
			}
		}
		else
		{
			// ������ ���� ���
			Item::CShopContainer* lpContainer = CCreatureManager::GetInstance().GetShopContainer(dwOwnerID);
			if (NULL == lpContainer)
			{
				ERRLOG2(g_Log, "CID:%10u ������ ���� : ������ ������(%10u)�� ���̵� �̻��մϴ�.", 
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
					ERRLOG3(g_Log, "CID:%10u ������ ���� : �̹� ������ �� �����Դϴ�. ������ �ϴ� �� : %10u, ���ִ� �� : %10u", 
						lpCustomer->GetCID(), dwOwnerID, dwOtherOwner);
					usError = PktStE::FAIL_ENTER;
				}
			}
		}
	}

	return GameClientSendPacket::SendCharStallEnter(GameClientDispatch.GetSendStream(), 
        dwCustomerID, dwOwnerID, usError);
}


// ģ��(�ź�)����Ʈ �߰�
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
        // ģ��(Ȥ�� �ź� ����Ʈ �߰�)
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
                    // ��밡 ģ�� �źθ� �߰ų�, ���� ����� Ban List�� ���� ����.
                    usError = PktFriendAck::REJECTED;
                }
				else if (EnemyCheck::EC_ENEMY == lpCharacter->IsEnemy(lpFriendCharacter))
				{
					// ������ ����Ʈ�� �߰��� �� ����.
					usError = PktFriendAck::FAIL_ENEMY;
				}
				else if (false == lpCharacter->GetFriendList().Add(
					lpFriendCharacter->GetCID(), lpFriendCharacter->GetCharacterName(), 
					lpFriendCharacter->GetGID(), lpFriendCharacter->GetClass(), lpFriendCharacter->GetLevel(), 
					CServerSetup::GetInstance().GetServerID()))
                {
                    // ����Ʈ �߰� ����
                    usError = PktFriendAck::LIST_FULL;
                    ERRLOG2(g_Log, "CID:%10u ����Ʈ�� �� ����, �� �̻� ģ���� ����� �� �����ϴ�. ���� ����Ʈ ��:%d", 
                        dwOwnerCID, lpCharacter->GetFriendList().GetFriendNum());
                }
                else
                {
                    // lpFriendCharacter ���� lpCharacter�� �ڽ��� ģ���� ����ߴٴ� �޽����� ���� �ش�.
                    CGameClientDispatch* lpFriendDispatch = lpFriendCharacter->GetDispatcher();
                    if (NULL != lpFriendDispatch)
                    {
                        GameClientSendPacket::SendCharFriendAdded(lpFriendDispatch->GetSendStream(), 
                            lpCharacter->GetCID(), lpCharacter->GetCharacterName());
                    }

                    // ���� ģ�� �߰��� �༮�� �� �ź� ����Ʈ�� ���� ���, �׳༮�� �����Ѵ�.
                    if (lpCharacter->GetBanList().Remove(lpFriendCharacter->GetCID()))
                    {
                        cAckCmd = PktFriendAck::MOVE_BAN_TO_FRIEND;
                    }

                    // DBAgent�� ģ�� �߰� ����
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
                            // �߰��� �༮�� �� �ź� ����Ʈ�� ���� ���, �� �༮�� �ź� ����Ʈ���� ����.
                            GameClientSendPacket::SendFriendListChangeToDB(DBAgentSendStream, lpCharacter->GetUID(),
                                dwOwnerCID, dwFriendUID, dwFriendCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_BAN);

                            GameClientSendPacket::SendFriendListChangeToDB(ChatSendStream, lpCharacter->GetUID(),
                                dwOwnerCID, dwFriendUID, dwFriendCID, 0, 0, 0, 0, 0, PktFriendDB::REMOVE_BAN);
                        }
                    }
                }
                break;

            case PktFriendAddReq::BAN_FRIEND_REQ:                
				// edith 2009.07.30 �źθ���Ʈ�� ������ �ź��� �� �ֵ��� ����. �ٸ� ������ �ٸ���쿡�� �����ϰ� ����
/*				if (EnemyCheck::EC_ENEMY == lpCharacter->IsEnemy(lpFriendCharacter))
				{
					// ������ ����Ʈ�� �߰��� �� ����.
					usError = PktFriendAck::FAIL_ENEMY;
				}
				else */
				if (false == lpCharacter->GetBanList().Add(lpFriendCharacter->GetCID(), lpFriendCharacter->GetCharacterName(), 
					lpFriendCharacter->GetGID(), lpFriendCharacter->GetClass(), lpFriendCharacter->GetLevel(), CServerSetup::GetInstance().GetServerID()))
                {
                    usError = PktFriendAck::LIST_FULL;
                    ERRLOG2(g_Log, "CID:%10u ����Ʈ�� �� ����, �� �̻� �ź� ����� ����� �� �����ϴ�. ���� ����Ʈ ��:%d", 
                        dwOwnerCID, lpCharacter->GetBanList().GetBanNum());
                }
                else
                {
					dwGID	= lpFriendCharacter->GetGID();
					wClass	= lpFriendCharacter->GetClass();
					cLevel	= lpFriendCharacter->GetLevel();

                    // lpFriendCharacter �� �ڽ��� ģ���� ����ϰ� ������, ���� �α׾ƿ��ϰ� �ִ� ��ó�� ���̰� �Ѵ�.
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

                    // ���� �ź� �߰��� �༮�� �� ģ�� ����Ʈ�� ���� ���, �׳༮�� �����Ѵ�.
                    if (lpCharacter->GetFriendList().Remove(lpFriendCharacter->GetCID()))
                    {
                        cAckCmd = PktFriendAck::MOVE_FRIEND_TO_BAN;
                    }

                    szFriendName    = lpFriendCharacter->GetCharacterName();
                    dwFriendUID     = lpFriendCharacter->GetUID();
                    dwFriendCID     = lpFriendCharacter->GetCID();

                    // DBAgent, ä�� ������ Ban �߰� ����                    
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
                ERRLOG2(g_Log, "CID:%10u Ŭ���̾�Ʈ���� �߸��� ģ�� Ŀ�ǵ带 ���� �Խ��ϴ�. Ŀ�ǵ�:%d",
                    dwOwnerCID, lpPktFriendAddRequest->m_cCmd);                
            };
        }
    }

    return GameClientSendPacket::SendCharFriendAck(GameClientDispatch.GetSendStream(), 
        cAckCmd, dwFriendCID, szFriendName, dwGID, wClass, cLevel, CServerSetup::GetInstance().GetServerID(), usError);
}


// ģ��(�ź�)����Ʈ ����
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

	// PktFriendRemoveReq::NOFRIEND_REQ �϶� ���
	memcpy(szTargetName, lpPktFriendRemoveRequest->m_szName, PktFriendAck::MAX_NAME);

    switch (lpPktFriendRemoveRequest->m_cCmd)
    {
		// edith 2009.08.28 NoFriend �߰�
	case PktFriendRemoveReq::NOFRIEND_REQ:
		{
			// szTargetName �� �������� ���ڽ��� ����.
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
					// DBAgent, Chatting���� ģ�� ���� �뺸 ����.
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

					// ������ ����� ����Ʈ���� ������ ���ش�.
					GameClientSendPacket::SendCharFriendAck(lpTargetCharacter->GetDispatcher()->GetSendStream(), 
						cAckCmd, dwOwnerCID, szRemovedName, 0, 0, 0, 0, usError);

					// �ǵ����� �̸� ����.
					memcpy(szRemovedName, szTargetName, PktFriendAck::MAX_NAME);
				}
				else
				{
					usError = PktFriendAck::NOT_IN_LIST;
					ERRLOG1(g_Log, "CID:%10u ģ�� ����Ʈ�� ���� ĳ���͸� �����Ϸ� �Ͽ����ϴ�.", dwOwnerCID);
				}
			}
			else
			{
				usError = PktFriendAck::NOT_LOGINED;
				ERRLOG1(g_Log, "CID:%10u �α׾ƿ� �Ǿ� �ְų� �ٸ����� �ִ� ��뿡�Դ� ����ϽǼ� �����ϴ�.", dwOwnerCID);
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
                // DBAgent, Chatting���� ģ�� ���� �뺸 ����.
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
                ERRLOG1(g_Log, "CID:%10u ģ�� ����Ʈ�� ���� ĳ���͸� �����Ϸ� �Ͽ����ϴ�.", dwOwnerCID);
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
                // DBAgent�� �ź� ���� �뺸 ����.
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
                ERRLOG1(g_Log, "CID:%10u ģ�� ����Ʈ�� ���� ĳ���͸� �����Ϸ� �Ͽ����ϴ�.", dwOwnerCID);
            }
        }
        break;

    default:

        usError = PktFriendAck::CLIENT_ERROR;
        ERRLOG2(g_Log, "0x%08x �߸��� Ŭ���̾�Ʈ ģ�� ����Ʈ ����Դϴ�. %d", 
            dwOwnerCID, lpPktFriendRemoveRequest->m_cCmd);
        break;
    }

    return GameClientSendPacket::SendCharFriendAck(GameClientDispatch.GetSendStream(), 
        cAckCmd, dwOwnerCID, szRemovedName, 0, 0, 0, 0, usError);
}


// ��Ÿ ó��(�׷� ���� ��)
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
            // �׷� ������ Ack����.
            CFriendList::Rebind* lpRebind = lpCharacter->GetFriendList().GetFriend(lpPktFriendEtc->m_dwCID);
            if (NULL != lpRebind)
            {
                if (lpRebind->SetGroup(lpPktFriendEtc->m_dwData))
                {
                    // DBAgent�� SetGroup����
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

// ��� ����
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
			// edith 2008.02.27 ������ ���� ����
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

// ��� ��� ���� ���
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
		ERRLOG4(g_Log, "GID:0x%08x �������� �ʴ� ��忡�� ��� ����� ��û�Ͽ����ϴ�. SenderID:0x%08x, ReferenceID:0x%08x, Cmd:%d", 
			dwGID, dwSenderID, dwReferenceID, wCmd);
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		switch (wCmd)
		{
			// Send : �ʴ� ���� ĳ����
			// Reference : ��� ���
			case PktGuildCmd::GC_INVITE:
			{
				lpGuild->InviteMember(dwReferenceID, dwSenderID, wError);
				break;
			}

			// Send : (�ʴ븦) �����ϴ� ĳ����
			// Reference : ��� ���
			case PktGuildCmd::GC_JOIN:
			{
				lpGuild->JoinMember(dwSenderID, Guild::COMMON, wError);
                GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::REQUEST, 
                    lpGuild->GetGID(), dwSenderID, Guild::COMMON, wError);
				break;
			}

			// Send : (�ʴ븦) �����ϴ� ĳ����
			// Reference : ��� ���
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

			// Send : ���� ��û�ϴ� ĳ����
			// Reference : None
			case PktGuildCmd::GC_ASK_ENTER:
			{
				lpGuild->JoinMember(dwSenderID, Guild::JOIN_WAIT, wError);
                GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::REQUEST, 
                    lpGuild->GetGID(), dwSenderID, Guild::JOIN_WAIT, wError);
				break;
			}

			// Send : Ż�� ��û�ϴ� ���
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

			// Send : Ż�� ���ϴ� ���
			// Reference : Ż�� ��Ű�� ���
			case PktGuildCmd::GC_KICK:	
			{
				// ����� �ð��� �ƴҶ��� Ż�� ����
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

			// Send : ���� ����Ǵ� ���
			// Reference : ���� ���� ����
			case PktGuildCmd::GC_SETUP_TITLE:
			{
				// ����� �ð��� �ƴҶ��� Ż�� ����
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
			// �뺴 ��û
			case PktGuildCmd::GC_TACTICS:
			{
				lpGuild->TacticsMember(dwSenderID, Guild::JOIN_WAIT, PktGuildCmd::GC_TACTICS, wError);

				GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), dwSenderID, Guild::TACTICS_WAIT, wError);

				break;
			}
			// �뺴 ��û ����.
			case PktGuildCmd::GC_TACTICS_JOIN:				
			{
				// edith 2009.08.05 �뺴��û ������ ���ƹ�����.
//				lpGuild->TacticsMember(dwSenderID, Guild::JOIN_WAIT, PktGuildCmd::GC_TACTICS_JOIN, wError);
//				GAMELOG::LogGuildJoin(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), dwSenderID, Guild::TACTICS_WAIT, wError);
				break;
			}
			// �뺴 �㰡.
			case PktGuildCmd::GC_TACTICS_TITLE:
			{								
				lpGuild->SetTacticsTitle(lpCharacter->GetCID(), dwSenderID, wCmd, static_cast<unsigned char>(dwReferenceID), 0, wError);						

				GAMELOG::LogGuildMemberLevelAdjust(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), 0, dwSenderID, dwReferenceID, wError);						
				
				break;
			}
			// �뺴 ����.
			case PktGuildCmd::GC_TACTICS_REQUEST:
			{
				lpGuild->SetTacticsTitle(lpCharacter->GetCID(), dwSenderID, wCmd, static_cast<unsigned char>(dwReferenceID), 1, wError);						

				GAMELOG::LogGuildMemberLevelAdjust(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), 0, dwSenderID, dwReferenceID, wError);						
								
				break;
			}
			case PktGuildCmd::GC_TACTICS_KICK:
			case PktGuildCmd::GC_TACTICS_LEAVE:
			{
				// ����� �ð��� �ƴҶ��� Ż�� ����
				if (!CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					lpGuild->KickTacticsMember(dwSenderID, dwReferenceID, wCmd, wError);
					GAMELOG::LogGuildLeave(GAMELOG::sGuildLog::REQUEST, lpGuild->GetGID(), dwReferenceID, dwSenderID, wError);
				}
				else
				{
					// ����� �ð��� �뺴 Ż��� �����ϴ�..
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
				ERRLOG4(g_Log, "GID:0x%08x �߸��� ��� ��� ����Դϴ�. SenderID:0x%08x, ReferenceID:0x%08x, Cmd:%d", 
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
				dwGID, dwSenderID, dwReferenceID, "����", "����", wCmd, wError);
		}
	}

	return true;
}

// ��� ��ũ ����
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
		ERRLOG1(g_Log, "CID:%10u ���������ʴ� ����� ��ũ�� �����Ϸ� �մϴ�.", dwGID);
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

// ��� ���� ����
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
			ERRLOG1(g_Log, "CID:%10u ��忡 ������ ���� ĳ���Ͱ� ��� ������ �����Ϸ� �մϴ�.", dwCID);
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

// ��� ���� ����
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
			ERRLOG1(g_Log, "CID:%10u ���� ������ �� �� ���� �ð��� ���� ������ ��û�մϴ�.", dwCID);
			wError = PktGuildRelation::FAIL_NOT_HOSTILITY_TIME;
		}

		if (PktGuildRelation::GR_ALERT == cSubCmd &&
			GameTime::GT_GUILD_WAR != CGameTimeMgr::GetInstance().GetCurrentGuildTime() &&
			GameTime::GT_PREPARE_END_30 != CGameTimeMgr::GetInstance().GetCurrentGuildTime())
		{
			ERRLOG1(g_Log, "CID:%10u ��� ���� ������ �� �� ���� �ð��� ���� ������ ��û�մϴ�.", dwCID);
			wError = PktGuildRelation::FAIL_NOT_ALERT_TIME;
		}

		if (PktGuildRelation::GR_COUNTER == cSubCmd &&
			GameTime::GT_GUILD_WAR != CGameTimeMgr::GetInstance().GetCurrentGuildTime() &&
			GameTime::GT_PREPARE_END_30 != CGameTimeMgr::GetInstance().GetCurrentGuildTime())
		{
			ERRLOG1(g_Log, "CID:%10u ī���� ���� ������ �Ҽ� ���� �ð��� ���� ������ ��û�մϴ�.", dwCID);
			wError = PktGuildRelation::FAIL_NOT_COUNTER_TIME;
		}

		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(lpMember->GetGID());
		if (NULL == lpGuild)
		{
			ERRLOG1(g_Log, "CID:%10u ��忡 ������ ���� ĳ���Ͱ� ��� ������ �����Ϸ� �մϴ�.", dwCID);
			wError = PktGuildRelation::FAIL_NOT_EXIST_GUILD;
		}
		else
		{
			if (lpGuild->GetMaster().m_dwCID != dwCID)
			{
				ERRLOG1(g_Log, "CID:%10u ��� �����Ͱ� �ƴ� ĳ���Ͱ� ��� ���踦 �����Ϸ� �մϴ�.", dwCID);
				wError = PktGuildRelation::FAIL_NOT_MASTER;
			}
		}

		CGuild* lpTargetGuild = CGuildMgr::GetInstance().GetGuild(dwTargetGID);
		if (NULL == lpTargetGuild)
		{
			ERRLOG2(g_Log, "CID:%10u ��� ���踦 �����ҷ��� ��� ��尡 �������� �ʽ��ϴ�. GID:0x%08x", dwCID, dwTargetGID);
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
					// �ڽ��� ��忡�� ���� ���� üũ
					if (lpGuild->GetGID() == dwTargetGID)
					{
						ERRLOG1(g_Log, "GID:0x%08x �ڽ��� ��忡�� ���� ������ �Ϸ��մϴ�.", lpGuild->GetGID());
						wError = PktGuildRelation::FAIL_SELF_GUILD;
					}

					// �̹� ���� �������� üũ
					if (Guild::HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ���� �̹� ���� �����Դϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_HOSTILITY;
					}
					// ���� ���� ���¸� ���� �������� : ũ�ν� ���븦 �����ϱ� ���ؼ�
					// ������ �̹� ���� ���� ��û�� ������..
					if (Guild::HOSTILITY == lpGuild->GetRelation(dwTargetGID, BY_TARGET) ||
						Guild::TARGET_NEUTRALITY == lpGuild->GetRelation(dwTargetGID, BY_TARGET) )
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ���� �̹� ���� �����Դϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_HOSTILITY;
					}
					
					// ���� ����� �ʰ� üũ
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

					// ������ ������ �ִ� ���� ���� �ִ�ġ �ʰ� üũ
					if (lpTargetGuild->GetNumOfRelation(Guild::ALL_HOSTILITY, Guild::BY_TARGET) >= Guild::MAX_HOSTILITY_APPLY_NUM)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ��尡 ���� �� �ִ� ���� ���� �Ѱ�ġ�Դϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_TARGET_HOSTILITY_FULL;
					}

					// ���� ����
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB �߰� ������ ����
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
					// �ڽ��� ��忡�� ��ȭ ���� üũ
					if (lpGuild->GetGID() == dwTargetGID)
					{
						ERRLOG1(g_Log, "GID:0x%08x �ڽ��� ��忡�� ��ȭ ������ �Ϸ��մϴ�.", lpGuild->GetGID());
						wError = PktGuildRelation::FAIL_SELF_GUILD;
					}

					// �߸� ������ ��忡�� ��ȭ ���� üũ
					if (Guild::NEUTRALITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x �߸� ���� ��忡�� ��ȭ ������ �Ϸ� �մϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_NEUTRALITY;
					}

					// ��ȭ ����
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB �߰� ������ ����
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
					// �ڽ��� ��忡�� ��ȭ ���� üũ
					if (lpGuild->GetGID() == dwTargetGID)
					{
						ERRLOG1(g_Log, "GID:0x%08x �ڽ��� ��忡�� ��ȭ ������ �Ϸ��մϴ�.", lpGuild->GetGID());
						wError = PktGuildRelation::FAIL_SELF_GUILD;
					}

					// �ش� Ÿ�ٱ�尡 ������ �ʴٸ�
					if (Guild::TARGET_NEUTRALITY != lpGuild->GetRelation(dwTargetGID, Guild::BY_TARGET))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ������ ���� ��尡 �ƴѵ� ��ȭ ������ �Ϸ� �մϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_NEUTRALITY;
					}

					// ��ȭ ����
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB �߰� ������ ����
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
					// �ڽ��� ��忡�� ��� ���� ���� üũ
					if (lpGuild->GetGID() == dwTargetGID)
					{
						ERRLOG1(g_Log, "GID:0x%08x �ڽ��� ��忡�� ��� ���� ������ �Ϸ��մϴ�.", lpGuild->GetGID());
						wError = PktGuildRelation::FAIL_SELF_GUILD;
					}

					// �̹� ���� �������� üũ
					if (Guild::HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ���� �̹� ���� �����Դϴ�.",
								lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_HOSTILITY;
					}

					// �̹� ī���� ���� �������� üũ
					if (Guild::COUNTER_HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ���� �̹� ī���� ���� �����Դϴ�.",
								lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_COUNTER_HOSTILITY;
					}

					// �̹� ��� ���� �������� üũ
					if (Guild::ALERT_HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ���� �̹� ��� ���� �����Դϴ�.",
								lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_ALERT_HOSTILITY;
					}

					// ��� ���� ����� �ʰ� üũ
					if (lpGuild->GetNumOfRelation(Guild::ALERT_HOSTILITY) >= Guild::MAX_ALERT_NUM)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ���� ����� �ʰ��� ��� ���� ���� ����",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_MAX_ALERT_NUM;
					}

					// ������ ������ �ִ� ���� ���� �ִ�ġ �ʰ� üũ
					if (lpTargetGuild->GetNumOfRelation(Guild::ALL_HOSTILITY, Guild::BY_TARGET) >= Guild::MAX_HOSTILITY_APPLY_NUM)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ��尡 ���� �� �ִ� ���� ���� �Ѱ�ġ�Դϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_TARGET_HOSTILITY_FULL;
					}

					// �������� ��� ���� üũ
					bool bHasDevelopingCamp = false, bHasCompleteCamp = false;
					CSiegeObjectMgr::GetInstance().HasCampByGID(dwTargetGID, bHasDevelopingCamp, bHasCompleteCamp);
					if (!bHasDevelopingCamp)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ��尡 �������� ��� ����� ������ ���� �ʽ��ϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_NOT_HAVE_DEVELOPING_CAMP;
					}

					// ��� ���� ����
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB �߰� ������ ����
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
					// �̹� ���� �������� üũ
					if (Guild::HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ���� �̹� ���� �����Դϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_HOSTILITY;
					}

					// �̹� ī���� ���� �������� üũ
					if (Guild::COUNTER_HOSTILITY == lpGuild->GetRelation(dwTargetGID))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ���� �̹� ī���� ���� �����Դϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_ALREADY_COUNTER_HOSTILITY;
					}

					// ��� ��尡 ������ ��� ���� ������ �Ͽ����� üũ
					if (Guild::ALERT_HOSTILITY != lpTargetGuild->GetRelation(lpGuild->GetGID()))
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ��尡 ��� ���븦 ��û���� �ʾҴµ� ī���� ���� ��û�� �Ϸ��մϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_NOT_HAVE_ALERT;
					}

					// ������ ������ �ִ� ���� ���� �ִ�ġ �ʰ� üũ
					if (lpTargetGuild->GetNumOfRelation(Guild::ALL_HOSTILITY, Guild::BY_TARGET) >= Guild::MAX_HOSTILITY_APPLY_NUM)
					{
						ERRLOG2(g_Log, "GID:0x%08x  TargetGID:0x%08x ��� ��尡 ���� �� �ִ� ���� ���� �Ѱ�ġ�Դϴ�.",
							lpGuild->GetGID(), dwTargetGID);
						wError = PktGuildRelation::FAIL_TARGET_HOSTILITY_FULL;
					}

					// ī���� ���� ����
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB �߰� ������ ����
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
					// ���� ���� �������� ���� ��ȭ ������ ��� ���� ���� ����
					if (wError == PktBase::NO_SERVER_ERR)
					{
						// DB �߰� ������ ����
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


// ��� ���� ����
bool GameClientParsePacket::ParseCharGuildInclination(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
// ���̻� ��� ������ ������ �� �����ϴ�. (2004-11-22 by �ε�)
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
			ERRLOG1(g_Log, "CID:%10u ��忡 ������ ���� ĳ���Ͱ� ��� ������ �����Ϸ� �մϴ�.", dwCID);
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

// ��� ����Ʈ
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

// ��� ���� ����
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
			ERRLOG1(g_Log, "CID:%10u ��忡 ������ ���� ĳ���Ͱ� ��� ������ �����Ϸ� �մϴ�.", dwCID);
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

// ��� ��� ����Ʈ
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

// ��� �ݰ�
bool GameClientParsePacket::ParseCharGuildSafe(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	// edith 2008.07.07 ���ݰ� ���׷� ���� �Ͻ������� ���ݰ� ������.
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

		// edith 2008.03.15 ĳ���� DB�� ����� ���� ��Ŷ�� ���´��� Ȯ��
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

			// dwCharGold : ��Ŷ�� ���� ����� ��
			// dwGold : Ŭ���̾�Ʈ�� ������ �ݾ�
			lpMember->SetGuildSafe(cCmd);
			lpGuild->SetGuildSafe(cCmd);

			switch (cCmd)
			{
				// ���
				case PktGuildSafe::SC_WITHDRAWAL:
				{
					if (lpGuild->GetTitle(lpMember->GetCID()) > lpGuild->GetRight().m_aryRight[GuildRight::USE_SAFE])
					{
						ERRLOG2(g_Log, "CID:%10u ��� �ݰ� ��� ������ ���� ������ �ݰ��� ����Ϸ� �մϴ�. GID:0x%08x", 
							lpMember->GetCID(), lpGuild->GetGID());
						wError = PktBase::SERVER_ERROR;
					}
					else
					{
						// ��忡�� ���� ����ߴ�.
						if (dwCharGold > ULONG_MAX - dwGold)
						{
							//ĳ���Ͱ� �ִ� ������ �ִµ� 4,294,967,295�� �Ѵ��� Ȯ��.
							wError = PktBase::SERVER_ERROR;
						}
                        else
                        {
                            // �α׿�. (â���� �� ���� ���� �ݾ� ���)
							// ��忡 �ִ� �ݾ׺��� �����ݾ��� ũ��
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
					// �Ա��� �ݰ� ��� ���� ���� ����
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
					// TODO : �ݰ� ��� ���� ��ȸ
					break;

				case PktGuildSafe::SC_RELEASE:
				{
					if (lpGuild->GetTitle(lpMember->GetCID()) > lpGuild->GetRight().m_aryRight[GuildRight::USE_SAFE])
					{
						ERRLOG2(g_Log, "CID:%10u ��� �ݰ� ��� ������ ���� ������ �ݰ� ������ �Ϸ� �մϴ�. GID:0x%08x", 
							lpMember->GetCID(), lpGuild->GetGID());
						wError = PktBase::SERVER_ERROR;
					}
					// �߰� �������� ó���Ѵ�.
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
				// ���� ó��
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
				// dwCharGold : ��Ŷ�� ���� ����� ��
				// dwGold : Ŭ���̾�Ʈ�� ������ �ݾ�
				lpMember->SetGuildSafe(-1);
				lpGuild->SetGuildSafe(-1);
			}
		}

		// ������ �� ���
		CGameClientDispatch* lpDispatch = lpMember->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharGuildSafe(lpDispatch->GetSendStream(), 
				dwCID, 0, 0, dwGold, cCmd, lpMember->GetCharacterName(), wError);
		}
	}

	return true;
}

// ���� ��� ��Ȳ ����Ʈ
bool GameClientParsePacket::ParseCharGuildHostilityList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktGuildHostilityList*	lpPktGuildHostilityList = reinterpret_cast<PktGuildHostilityList *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCID			= lpPktGuildHostilityList->m_dwCID;			// ĳ���� ���̵�
	unsigned char	cCurrentPage	= lpPktGuildHostilityList->m_cCurrentPage;	// ���� ������
	unsigned char	cPageState		= lpPktGuildHostilityList->m_cPageState;	// ��û�� �������� ����

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

// ����� ���� ����Ʈ ��û
bool GameClientParsePacket::ParseCharGuildRelationInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktGuildRelationInfo*	lpPktGuildRelationInfo	= reinterpret_cast<PktGuildRelationInfo *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCID	= lpPktGuildRelationInfo->m_dwCID;		// ��û�� ĳ���� ���̵�
	unsigned long	dwGID	= lpPktGuildRelationInfo->m_dwGID;		// ��û�� ĳ������ ��� ���̵�

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
