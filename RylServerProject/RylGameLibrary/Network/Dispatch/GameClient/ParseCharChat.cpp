
#include "stdafx.h"

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

#include <Map/FieldMap/Cell.h>

#include <Utility/Math/Math.h>

#include "SendCharCommunity.h"
#include "ParseCharChat.h"
#include "GameClientDispatch.h"


bool GameClientParsePacket::ParseCharWhisper(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    int nPacketLength = lpPktBase->GetLen();
    if (nPacketLength <= sizeof(PktWh))
    {
        ERRLOG3(g_Log, "UID:%10u/IP:%15s/�ӼӸ� ��Ŷ ���� �̻�. ����:%d", 
            GameClientDispatch.GetUID(), GameClientDispatch.GetRemoteAddr().get_addr_string(), nPacketLength);

        return false;
    }    

    // �ӼӸ��� ������, �ٷ� ä�ü����� Relay�Ѵ�.
    PktWh* lpPktWh = reinterpret_cast<PktWh*>(lpPktBase);
    char* szMessagePos = reinterpret_cast<char*>(lpPktWh + 1);
    
    // ���� �����÷ο츦 ���� ���ؼ�, �������� �� ���ڸ� ���δ�.
    szMessagePos[nPacketLength - sizeof(PktWh) - 1] = 0;

    CCharacter* lpSender = GameClientDispatch.GetCharacter();
    if (NULL != lpSender)
    {
		// ä�� ����
		if (true == lpSender->IsChatBan())
		{
			CGameClientDispatch* lpDispatch = lpSender->GetDispatcher();
	        if (NULL != lpDispatch)
			{
				char* lpBuffer = lpDispatch->GetSendStream().GetBuffer(sizeof(PktWh));
				if (NULL != lpBuffer)
				{
					PktWh* lpPktWhAck = reinterpret_cast<PktWh*>(lpBuffer);

					strncpy(lpPktWhAck->m_SenderName, lpPktWh->m_SenderName, CHAR_INFOST::MAX_NAME_LEN);
					strncpy(lpPktWhAck->m_RecverName, lpPktWh->m_RecverName, CHAR_INFOST::MAX_NAME_LEN);

					return lpDispatch->GetSendStream().WrapCrypt(
						sizeof(PktWh), CmdCharWhisper, 0, PktWh::FAIL_CHAT_BAN);
				}
			}

			return true;
		}

	    unsigned long   dwOwnerCID  = lpSender->GetCID();
        CCharacter* lpRecver = CCreatureManager::GetInstance().GetCharacter(lpPktWh->m_RecverName);
		if(lpRecver)
		{
			// edith 2009.04.04 ���� Ban����Ʈ�� ��ϵǾ������� ��� ä�� �ź��ϰ� ó��
	//		lpRecver->GetRejectOption().Reject.m_Friend || 
			if ( lpRecver->GetBanList().IsBan(dwOwnerCID, lpPktWh->m_SenderName ) )
			{
				// �긮��Ʈ�� ��ϵǾ�������� �Ӹ��ź�
				CGameClientDispatch* lpDispatch = lpSender->GetDispatcher();
				if (NULL != lpDispatch)
				{
					char* lpBuffer = lpDispatch->GetSendStream().GetBuffer(sizeof(PktWh));
					if (NULL != lpBuffer)
					{
						PktWh* lpPktWhAck = reinterpret_cast<PktWh*>(lpBuffer);

						strncpy(lpPktWhAck->m_SenderName, lpPktWh->m_SenderName, CHAR_INFOST::MAX_NAME_LEN);
						strncpy(lpPktWhAck->m_RecverName, lpPktWh->m_RecverName, CHAR_INFOST::MAX_NAME_LEN);

						return lpDispatch->GetSendStream().WrapCrypt(
							sizeof(PktWh), CmdCharWhisper, 0, PktWh::FAIL_REJECT);
					}
				}

				return true;
			}
		}

		// ä���� ������ ���� ó���� ä�� �������� ó���մϴ�. 
		// (ä�� ������ ä�� ������ ���޵Ǹ� �Ϸ������� ó���ϴ°� ���� �� �����ϴ�.)

        GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, 
            CChatDispatch::GetDispatchTable());

        if (0 != lpChatDispatch)
        {
            const Position& pos = lpSender->GetCurrentPos();
			
            CChatRequestPacket chatReqPacket(szMessagePos, 
                PktChat::WHISPER, 0, lpSender->GetUID(), lpSender->GetCID(), 
                static_cast<unsigned short>(pos.m_fPointX), 
                static_cast<unsigned short>(pos.m_fPointY),
                static_cast<unsigned short>(pos.m_fPointZ),
                lpPktWh->m_RecverName, 1);

            if (chatReqPacket.IsValid())
            {
                return lpChatDispatch->GetSendStream().PutBuffer(
                    chatReqPacket.GetCompressedPacket(),
                    chatReqPacket.GetCompressedSize(), CmdCharChat);
            }
        }
    }

    return false;
}


bool GameClientParsePacket::ParseCharChat(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{    
    //  ������ ä�� ���

    //  ������ ����� �ʿ� ���� ä�� (�α׸� ����)

    //  NORMAL		        = 0,    // ���� ĳ���� ��ǥ�� ������ �Ǵ�.
    //  ADMIN_NORMAL_CHAT   = 8,    // ��� ��� ä��     (Ack�θ� ���)
    //  STALL		        = 6,    // ������ ���ϱ�    (�̸� char[16] * m_cNum - MAX 10��)	
    //  CLIENT_LOG	        = 5,
	//	FIND_PARTY			= 14	// ��Ƽ ã��
	//	DICE				= 16	// �ֻ���

    //  ������ ����� �ʿ��� ä��

    //  PARTY		        = 1,    // ��Ƽ ���ϱ�
    //  FRIEND		        = 2,    // ģ�� ���ϱ�      (�̸� char[16] * m_cNum - MAX 100��)
    //  GUILD	            = 3,    // ��� ���ϱ�
    //  SHOUT               = 7,    // ��ü ���ϱ�

    //  ADMIN_SHOUT         = 9,    // ��� ��ġ��        (Ack�θ� ���)
    //  WHISPER             = 10,   // �� ��ȣ�� ������ �ʴ´�.Whisper�� ������ ��Ŷ�� ����ϱ� �����̴�. ��, �α׽ÿ� ���δ�.

    int nPacketLength = lpPktBase->GetLen();
    if (nPacketLength <= sizeof(PktChat))
    {
        ERRLOG3(g_Log, "UID:%10u/IP:%15s ��Ŷ ���̰� �̻��մϴ�. ����:%d", 
            GameClientDispatch.GetUID(), GameClientDispatch.GetRemoteAddr().get_addr_string(), nPacketLength);

        return false;
    }

    PktChat* lpPktChat = reinterpret_cast<PktChat*>(lpPktBase);

    int nMinPacketSize = sizeof(PktChat) + sizeof(char) * CHAR_INFOST::MAX_NAME_LEN * lpPktChat->m_cNum;
    int nMaxPacketSize = nMinPacketSize + PktChat::PktChatMaxSize;

    if (nPacketLength < nMinPacketSize || nMaxPacketSize < nPacketLength)
    {
        ERRLOG3(g_Log, "UID:%10u/IP:%15s ��Ŷ ���̰� �̻��մϴ�. ����:%d", 
            GameClientDispatch.GetUID(), GameClientDispatch.GetRemoteAddr().get_addr_string(), nPacketLength);

        return false;
    }

    char* szNames   = reinterpret_cast<char*>(lpPktChat + 1);
    char* szMessage = szNames + sizeof(char) * CHAR_INFOST::MAX_NAME_LEN * lpPktChat->m_cNum;

    // ���� �����÷ο츦 ���� ���ؼ�, �������� �� ���ڸ� ���δ�.
    szMessage[nPacketLength - nMinPacketSize - 1] = 0;

    CCharacter* lpSender = GameClientDispatch.GetCharacter();

    if (NULL == lpSender)
		return true;

	// ä�� ����
	if (lpSender->IsChatBan())
	{
		if (lpPktChat->m_cCmd != PktChat::CLIENT_LOG)
		{
			CGameClientDispatch* lpDispatch = lpSender->GetDispatcher();
	        if (NULL != lpDispatch)
			{
				CChatPacket ErrorResult(szMessage, lpSender->GetCID(), 
					static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), 0, 0, PktChat::CHAT_BAN);

				if (ErrorResult.IsValid())
				{
					lpDispatch->GetSendStream().PutBuffer(
						ErrorResult.GetCompressedPacket(), 
						ErrorResult.GetCompressedSize(), CmdCharChat);
				}
			}

			return true;
		}
	}
    switch (lpPktChat->m_cCmd)
    {
		case PktChat::DICE:	// �ֻ����� ����
		{
			// �ֻ����� �������� �󸶳� ���ȴ���Ȯ���Ѵ�.
			CHAR		seps[] = " ,\t\n" ;
			CHAR		*token, strCommand[ MAX_PATH ] ;

			strcpy( strCommand, szMessage ) ;

			token = strtok( strCommand, seps ) ;
			ULONG len = strlen( token ) + 1 ;

			int iTotal = Math::Convert::Atoi( &strCommand[ len ] );
			int iDice  = Math::Random::ComplexRandom(iTotal)+1;

			sprintf(szMessage, "%s %d %d", token, iTotal, iDice);

			// ��Ƽ�� ������� ������.
			if( lpSender->GetPID() != 0)
			{
				break;
			}

            const unsigned long MAX_SQUARED_DISTANCE = 32 * 32; 
            CCell* lpSenderCell = lpSender->GetCellPos().m_lpCell;

            if (0 != lpSenderCell)
            {
                CChatPacket chatPacket(szMessage, lpSender->GetCID(),
                    static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), lpPktChat->m_usLang);

                if (chatPacket.IsValid())
                {
                    for(int nCount = 0; nCount < CCell::CONNECT_NUM; ++nCount)
                    {
                        CCell* lpSendCell = lpSenderCell->GetConnectCell(nCount);

                        if (0 != lpSendCell)
                        {
                            CCharacter* lpReceiver = lpSendCell->GetFirstCharacter();

                            while (0 != lpReceiver)
                            {
								// ���ڽŵ� ������ �ϴ°� Ʋ����.
                                if (lpSender->GetCurrentPos().GetSquaredDistance(
                                    lpReceiver->GetCurrentPos()) < MAX_SQUARED_DISTANCE)
                                {
                                    if (0 != lpSender->GetAdminLevel() || 
										lpSender->GetNation() == lpReceiver->GetNation())
                                    {
                                        CGameClientDispatch* lpDispatch = lpReceiver->GetDispatcher();
                                        if (NULL != lpDispatch)
                                        {

                                            lpDispatch->GetSendStream().PutBuffer(
                                                chatPacket.GetCompressedPacket(), 
                                                chatPacket.GetCompressedSize(), CmdCharChat);
                                        }
                                    }
                                }

                                lpReceiver = lpSendCell->GetNextCharacter();
                            }
                        }
                    }
                }                        
            }

            // �̸� ���� ä�ü����� ������ �ʴ´�.
            lpPktChat->m_cNum = 0;
		}
		break;

        // �� ��쿡��, �������� ��ü������ ó���Ѵ�.
		case PktChat::NORMAL:
		case PktChat::ADMIN_NORMAL_CHAT:
        {            
            const unsigned long MAX_SQUARED_DISTANCE = 32 * 32; 

            CCell* lpSenderCell = lpSender->GetCellPos().m_lpCell;

            if (0 != lpSenderCell)
            {
                CChatPacket chatPacket(szMessage, lpSender->GetCID(), 
                    static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), lpPktChat->m_usLang);

                if (chatPacket.IsValid())
                {
                    for(int nCount = 0; nCount < CCell::CONNECT_NUM; ++nCount)
                    {
                        CCell* lpSendCell = lpSenderCell->GetConnectCell(nCount);

                        if (0 != lpSendCell)
                        {
                            CCharacter* lpReceiver = lpSendCell->GetFirstCharacter();

                            while (0 != lpReceiver)
                            {
                                if (lpSender != lpReceiver && 
                                    lpSender->GetCurrentPos().GetSquaredDistance(
                                    lpReceiver->GetCurrentPos()) < MAX_SQUARED_DISTANCE)
                                {
									// ������ ����̳�, �޴� ����̳� ���� ������ 0�� �ƴϰų� ������
									// �������� ��ȭ ����
                                    if (0 != lpSender->GetAdminLevel() || 0 != lpReceiver->GetAdminLevel() ||
										lpSender->GetNation() == lpReceiver->GetNation())
                                    {
                                        CGameClientDispatch* lpDispatch = lpReceiver->GetDispatcher();
                                        if (NULL != lpDispatch)
                                        {

                                            lpDispatch->GetSendStream().PutBuffer(
                                                chatPacket.GetCompressedPacket(), 
                                                chatPacket.GetCompressedSize(), CmdCharChat);
                                        }
                                    }
                                }

                                lpReceiver = lpSendCell->GetNextCharacter();
                            }
                        }
                    }
                }                        
            }

            // �̸� ���� ä�ü����� ������ �ʴ´�.
            lpPktChat->m_cNum = 0;
        }
        break;

        // �� ��쿡��, �������� ��ü������ ó���Ѵ�.
		case PktChat::STALL:
		case PktChat::CAMP_SHOP:
        {            
            CChatPacket chatPacket(szMessage, lpSender->GetCID(), 
                static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), lpPktChat->m_usLang);

            if (chatPacket.IsValid())
            {
                for(; szNames < szMessage; szNames += sizeof(char) * CHAR_INFOST::MAX_NAME_LEN)
                {
                    // ���� �����÷ο� ����
                    szNames[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

                    CCharacter* lpReceiver = 
                        CCreatureManager::GetInstance().GetCharacter(szNames);

                    if (0 != lpReceiver && lpSender->GetNation() == lpReceiver->GetNation())
                    {
                        CGameClientDispatch* lpDispatch = lpReceiver->GetDispatcher();
                        if (NULL != lpDispatch)
                        {
                            lpDispatch->GetSendStream().PutBuffer(
                                chatPacket.GetCompressedPacket(),
                                chatPacket.GetCompressedSize(), CmdCharChat);
                        }                            
                    }
                }
            }

            // �̸� ���� ä�ü����� ������ �ʴ´�.
            lpPktChat->m_cNum = 0;
        }
        break;

		// �� ��쿡��, �������� ��ü������ ó���Ѵ�.
		case PktChat::FIND_PARTY:
		{
			CChatPacket chatPacket(szMessage, lpSender->GetCID(), 
				static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), lpPktChat->m_usLang);

			if (chatPacket.IsValid())
			{
				CCreatureManager::GetInstance().SendFindPartyMsg(
                    lpSender->GetCID(), chatPacket.GetCompressedPacket(), chatPacket.GetCompressedSize());
			}

			// �̸� ���� ä�ü����� ������ �ʴ´�.
			lpPktChat->m_cNum = 0;
		}
		break;

        // �α׸� ��´�.
		case PktChat::CLIENT_LOG:
		{
			RULLOG3(g_Log, "UID:%10u/CID:%10u/Ŭ���̾�Ʈ �α� : %s", 
				lpSender->GetUID(), lpSender->GetCID(), szMessage);

			// �̸� ���� ä�ü����� ������ �ʴ´�.
			lpPktChat->m_cNum = 0;
		}
		break;

        // ���� ��쿣 ä�ü����� ������ ���� �ָ� �ȴ�.
		case PktChat::PARTY:
		case PktChat::FRIEND:
		case PktChat::GUILD:        
		case PktChat::ADMIN_SHOUT:
			break;

		case PktChat::GMREPORT:
			break;

		case PktChat::TRADE:
		case PktChat::SHOUT:
			// edith 2008.05.27 ��ġ��, Trade ��ġ�� ���� ���� �������� ����
			if (0 == lpSender->GetAdminLevel() && !lpSender->CanShout())
			{
				CGameClientDispatch* lpDispatch = lpSender->GetDispatcher();
	            if (NULL != lpDispatch)
				{
					CChatPacket ErrorResult(szMessage, lpSender->GetCID(), 
						static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), 0, 0, PktChat::SHOUT_TIMEOUT);

					if (ErrorResult.IsValid())
					{
						lpDispatch->GetSendStream().PutBuffer(
							ErrorResult.GetCompressedPacket(), 
							ErrorResult.GetCompressedSize(), CmdCharChat);
					}
				}

				return true;
			}
			break;
/*
		{
			// edith 2008.05.27 ��ġ��, Trade ��ġ�� ���� ���� �������� ����
			if (0 == lpSender->GetAdminLevel() && !lpSender->CanShout())
			{
				CGameClientDispatch* lpDispatch = lpSender->GetDispatcher();
	            if (NULL != lpDispatch)
				{
					CChatPacket ErrorResult(szMessage, lpSender->GetCID(), 
						static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), 0, PktChat::SHOUT_TIMEOUT);

					if (ErrorResult.IsValid())
					{
						lpDispatch->GetSendStream().PutBuffer(
							ErrorResult.GetCompressedPacket(), 
							ErrorResult.GetCompressedSize(), CmdCharChat);
					}
				}

				return true;
			}
			else
			{
				CCell* lpSenderCell = lpSender->GetCellPos().m_lpCell;

				if (0 != lpSenderCell)
				{
					CChatPacket chatPacket(szMessage, lpSender->GetCID(),
						static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd));

					if (chatPacket.IsValid())
					{
						for(int nCount = 0; nCount < CCell::CONNECT_NUM; ++nCount)
						{
							CCell* lpSendCell = lpSenderCell->GetConnectCell(nCount);

							if (0 != lpSendCell)
							{
								CCharacter* lpReceiver = lpSendCell->GetFirstCharacter();

								while (0 != lpReceiver)
								{
									if (0 != lpSender->GetAdminLevel() || 
										lpSender->GetNation() == lpReceiver->GetNation())
									{
										CGameClientDispatch* lpDispatch = lpReceiver->GetDispatcher();
										if (NULL != lpDispatch)
										{
											lpDispatch->GetSendStream().PutBuffer(
												chatPacket.GetCompressedPacket(), 
												chatPacket.GetCompressedSize(), CmdCharChat);
										}
									}

									lpReceiver = lpSendCell->GetNextCharacter();
								}
							}
						}
					}                        
				}

				// �̸� ���� ä�ü����� ������ �ʴ´�.
				lpPktChat->m_cNum = 0;                    

				lpSender->Shouted();
			}
		}
		break;
*/
	}

    // �ϴ� ä�ü����δ� ������ ������.
    GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, CChatDispatch::GetDispatchTable());

    if (0 != lpChatDispatch)
    {
        const Position& pos = lpSender->GetCurrentPos();

        CChatRequestPacket chatReqPacket(szMessage, static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd),
            lpPktChat->m_usLang, lpSender->GetUID(), lpSender->GetCID(), 
            static_cast<unsigned short>(pos.m_fPointX),
            static_cast<unsigned short>(pos.m_fPointY),
            static_cast<unsigned short>(pos.m_fPointZ),
            szNames, lpPktChat->m_cNum);

        // ä�ü����� ��Ŷ�� ������.
        if (chatReqPacket.IsValid())
        {
            lpChatDispatch->GetSendStream().PutBuffer(
                chatReqPacket.GetCompressedPacket(),
                chatReqPacket.GetCompressedSize(), CmdCharChat);                    
        }
    }

    return true;
}

