
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
        ERRLOG3(g_Log, "UID:%10u/IP:%15s/귓속말 패킷 길이 이상. 길이:%d", 
            GameClientDispatch.GetUID(), GameClientDispatch.GetRemoteAddr().get_addr_string(), nPacketLength);

        return false;
    }    

    // 귓속말을 받으면, 바로 채팅서버로 Relay한다.
    PktWh* lpPktWh = reinterpret_cast<PktWh*>(lpPktBase);
    char* szMessagePos = reinterpret_cast<char*>(lpPktWh + 1);
    
    // 버퍼 오버플로우를 막기 위해서, 마지막에 널 문자를 붙인다.
    szMessagePos[nPacketLength - sizeof(PktWh) - 1] = 0;

    CCharacter* lpSender = GameClientDispatch.GetCharacter();
    if (NULL != lpSender)
    {
		// 채팅 금지
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
			// edith 2009.04.04 나의 Ban리스트에 등록되어있으면 모든 채팅 거부하게 처리
	//		lpRecver->GetRejectOption().Reject.m_Friend || 
			if ( lpRecver->GetBanList().IsBan(dwOwnerCID, lpPktWh->m_SenderName ) )
			{
				// 밴리스트에 등록되어있을경우 귓말거부
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

		// 채금을 제외한 에러 처리는 채팅 서버에서 처리합니다. 
		// (채금 정보도 채팅 서버에 전달되면 일률적으로 처리하는게 좋을 것 같습니다.)

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
    //  가능한 채팅 목록

    //  서버간 통신이 필요 없는 채팅 (로그만 남김)

    //  NORMAL		        = 0,    // 현재 캐릭터 좌표를 가지고 판단.
    //  ADMIN_NORMAL_CHAT   = 8,    // 운영자 통상 채팅     (Ack로만 사용)
    //  STALL		        = 6,    // 노점상 말하기    (이름 char[16] * m_cNum - MAX 10개)	
    //  CLIENT_LOG	        = 5,
	//	FIND_PARTY			= 14	// 파티 찾기
	//	DICE				= 16	// 주사위

    //  서버간 통신이 필요한 채팅

    //  PARTY		        = 1,    // 파티 말하기
    //  FRIEND		        = 2,    // 친구 말하기      (이름 char[16] * m_cNum - MAX 100개)
    //  GUILD	            = 3,    // 길드 말하기
    //  SHOUT               = 7,    // 전체 말하기

    //  ADMIN_SHOUT         = 9,    // 운영자 외치기        (Ack로만 사용)
    //  WHISPER             = 10,   // 이 번호는 쓰이지 않는다.Whisper는 별도의 패킷을 사용하기 때문이다. 단, 로그시에 쓰인다.

    int nPacketLength = lpPktBase->GetLen();
    if (nPacketLength <= sizeof(PktChat))
    {
        ERRLOG3(g_Log, "UID:%10u/IP:%15s 패킷 길이가 이상합니다. 길이:%d", 
            GameClientDispatch.GetUID(), GameClientDispatch.GetRemoteAddr().get_addr_string(), nPacketLength);

        return false;
    }

    PktChat* lpPktChat = reinterpret_cast<PktChat*>(lpPktBase);

    int nMinPacketSize = sizeof(PktChat) + sizeof(char) * CHAR_INFOST::MAX_NAME_LEN * lpPktChat->m_cNum;
    int nMaxPacketSize = nMinPacketSize + PktChat::PktChatMaxSize;

    if (nPacketLength < nMinPacketSize || nMaxPacketSize < nPacketLength)
    {
        ERRLOG3(g_Log, "UID:%10u/IP:%15s 패킷 길이가 이상합니다. 길이:%d", 
            GameClientDispatch.GetUID(), GameClientDispatch.GetRemoteAddr().get_addr_string(), nPacketLength);

        return false;
    }

    char* szNames   = reinterpret_cast<char*>(lpPktChat + 1);
    char* szMessage = szNames + sizeof(char) * CHAR_INFOST::MAX_NAME_LEN * lpPktChat->m_cNum;

    // 버퍼 오버플로우를 막기 위해서, 마지막에 널 문자를 붙인다.
    szMessage[nPacketLength - nMinPacketSize - 1] = 0;

    CCharacter* lpSender = GameClientDispatch.GetCharacter();

    if (NULL == lpSender)
		return true;

	// 채팅 금지
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
		case PktChat::DICE:	// 주사위도 동일
		{
			// 주사위면 랜덤으로 얼마나 돌렸는지확인한다.
			CHAR		seps[] = " ,\t\n" ;
			CHAR		*token, strCommand[ MAX_PATH ] ;

			strcpy( strCommand, szMessage ) ;

			token = strtok( strCommand, seps ) ;
			ULONG len = strlen( token ) + 1 ;

			int iTotal = Math::Convert::Atoi( &strCommand[ len ] );
			int iDice  = Math::Random::ComplexRandom(iTotal)+1;

			sprintf(szMessage, "%s %d %d", token, iTotal, iDice);

			// 파티면 상단으로 보낸다.
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
								// 나자신도 보내게 하는게 틀리다.
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

            // 이름 등은 채팅서버로 보내지 않는다.
            lpPktChat->m_cNum = 0;
		}
		break;

        // 이 경우에는, 서버에서 자체적으로 처리한다.
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
									// 보내는 사람이나, 받는 사람이나 어드민 레벨이 0이 아니거나 종족이
									// 같을때만 대화 가능
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

            // 이름 등은 채팅서버로 보내지 않는다.
            lpPktChat->m_cNum = 0;
        }
        break;

        // 이 경우에는, 서버에서 자체적으로 처리한다.
		case PktChat::STALL:
		case PktChat::CAMP_SHOP:
        {            
            CChatPacket chatPacket(szMessage, lpSender->GetCID(), 
                static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), lpPktChat->m_usLang);

            if (chatPacket.IsValid())
            {
                for(; szNames < szMessage; szNames += sizeof(char) * CHAR_INFOST::MAX_NAME_LEN)
                {
                    // 버퍼 오버플로우 막기
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

            // 이름 등은 채팅서버로 보내지 않는다.
            lpPktChat->m_cNum = 0;
        }
        break;

		// 이 경우에는, 서버에서 자체적으로 처리한다.
		case PktChat::FIND_PARTY:
		{
			CChatPacket chatPacket(szMessage, lpSender->GetCID(), 
				static_cast<PktChat::PktChatCmd>(lpPktChat->m_cCmd), lpPktChat->m_usLang);

			if (chatPacket.IsValid())
			{
				CCreatureManager::GetInstance().SendFindPartyMsg(
                    lpSender->GetCID(), chatPacket.GetCompressedPacket(), chatPacket.GetCompressedSize());
			}

			// 이름 등은 채팅서버로 보내지 않는다.
			lpPktChat->m_cNum = 0;
		}
		break;

        // 로그를 찍는다.
		case PktChat::CLIENT_LOG:
		{
			RULLOG3(g_Log, "UID:%10u/CID:%10u/클라이언트 로그 : %s", 
				lpSender->GetUID(), lpSender->GetCID(), szMessage);

			// 이름 등은 채팅서버로 보내지 않는다.
			lpPktChat->m_cNum = 0;
		}
		break;

        // 다음 경우엔 채팅서버로 고스란히 보내 주면 된다.
		case PktChat::PARTY:
		case PktChat::FRIEND:
		case PktChat::GUILD:        
		case PktChat::ADMIN_SHOUT:
			break;

		case PktChat::GMREPORT:
			break;

		case PktChat::TRADE:
		case PktChat::SHOUT:
			// edith 2008.05.27 외치기, Trade 외치기 존에 같은 종족으로 제한
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
			// edith 2008.05.27 외치기, Trade 외치기 존에 같은 종족으로 제한
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

				// 이름 등은 채팅서버로 보내지 않는다.
				lpPktChat->m_cNum = 0;                    

				lpSender->Shouted();
			}
		}
		break;
*/
	}

    // 일단 채팅서버로는 무조건 보낸다.
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

        // 채팅서버로 패킷을 보낸다.
        if (chatReqPacket.IsValid())
        {
            lpChatDispatch->GetSendStream().PutBuffer(
                chatReqPacket.GetCompressedPacket(),
                chatReqPacket.GetCompressedSize(), CmdCharChat);                    
        }
    }

    return true;
}

