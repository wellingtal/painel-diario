
#include "stdafx.h"

#include <Utility/Setup/ServerSetup.h>

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharConfigPacket.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/EnemyCheck.h>

#include "ChatDispatch.h"

#include <Castle/CastleMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

CSingleDispatch& CChatDispatch::GetDispatchTable()
{
	static CSingleDispatch singleDispatch;
	return singleDispatch;
}


CChatDispatch::CChatDispatch(CSession& Session)
:   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE)
{
    
}


CChatDispatch::~CChatDispatch()
{

}

void CChatDispatch::Connected()
{
    CChatDispatch::SendChatServerLogin(m_SendStream);
}

void CChatDispatch::Disconnected()
{    
    GetDispatchTable().RemoveDispatch(this);
	DETLOG1(g_Log, "this:0x%p/ChatServer Disconnected", this);
}

bool CChatDispatch::DispatchPacket(PktBase* lpPktBase)
{
	bool bResult = false;

    switch (lpPktBase->GetCmd())
    {
		case CmdSysServerLogin:
			if (0 == lpPktBase->GetError())
			{
				CChatDispatch::GetDispatchTable().SetDispatch(this);
				DETLOG1(g_Log, "this:0x%p/ChatServer Connected", this);
				bResult = true;
			}		
			break;

		case CmdFriendDB:       
			bResult = ParseFriendDB(reinterpret_cast<PktFriendDB*>(lpPktBase));
			break;

		case CmdFriendAck:        
			bResult = ParseFriendAck(reinterpret_cast<PktFriendAck*>(lpPktBase));
			break;

		case CmdCharChat:
			bResult = ParseCharChat(lpPktBase);
			break;

		case CmdChatBan:
			bResult = ParseChatBan(reinterpret_cast<PktChatBan*>(lpPktBase));
			break;

		default:
			bResult = true;
			ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Unknown Chatpacket", 
				this, lpPktBase->GetCmd());

			break;
    }      
    
	if (!bResult)
	{
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Chatpacket Process failed", 
			this, lpPktBase->GetCmd());
	}
	
	return true;
}


bool CChatDispatch::SendChatServerLogin(CSendStream& SendStream)
{    
    PktSL* lpPktSL = reinterpret_cast<PktSL*>(SendStream.GetBuffer(sizeof(PktSL)));

    if (NULL != lpPktSL)
    {
        lpPktSL->m_dwServerID           = CServerSetup::GetInstance().GetServerID();
        lpPktSL->m_Address.S_un.S_addr  = 0;

        return SendStream.WrapHeader(sizeof(PktSL), CmdSysServerLogin, 0, 0);
    }

    return false;
}


bool CChatDispatch::ParseFriendDB(PktFriendDB* lpPktFriendDB)
{
    if (lpPktFriendDB->m_cCmd == PktFriendDB::FRIEND_LIST)
    {
        // ���� �α��� ���°� ��� ģ�� ��Ŷ�� ���ɴϴ�.

        CCharacter* lpCharacter = 
            CCreatureManager::GetInstance().GetCharacter(lpPktFriendDB->m_dwOwnerCID);

        if (0 != lpCharacter && lpCharacter->GetUID() == lpPktFriendDB->m_dwOwnerUID)
        {
            CFriendList&    friendList  = lpCharacter->GetFriendList();
            CBanList&       banList     = lpCharacter->GetBanList();

            CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

            if (friendList.SerializeIn(
                reinterpret_cast<char*>(lpPktFriendDB + 1), lpPktFriendDB->m_dwData))
            {
                if (0 != lpDispatch)
                {
                    CSendStream& sendStream = lpDispatch->GetSendStream();

                    // ģ�� ���� �� �ź� ������ ������.
                    CCommunityListSend sendFriendList(friendList);
                    CCommunityListSend sendBanList(banList);
					
                    if (!sendFriendList.Send(lpPktFriendDB->m_dwOwnerUID, lpPktFriendDB->m_dwOwnerCID, 
                        lpPktFriendDB->m_dwGID, lpPktFriendDB->m_wClass, lpPktFriendDB->m_cLevel, lpPktFriendDB->m_dwServerID, sendStream))
                    {
                        ERRLOG2(g_Log, "UID:%d/CID:%d/ ģ�� ����Ʈ�� Ŭ���̾�Ʈ�� ������ �� �����߽��ϴ�.", 
                            lpPktFriendDB->m_dwOwnerUID, lpPktFriendDB->m_dwOwnerCID);
                    }

                    if (!sendBanList.Send(lpPktFriendDB->m_dwOwnerUID, lpPktFriendDB->m_dwOwnerCID, 
                        lpPktFriendDB->m_dwGID, lpPktFriendDB->m_wClass, lpPktFriendDB->m_cLevel, lpPktFriendDB->m_dwServerID, sendStream))
                    {
                        ERRLOG2(g_Log, "UID:%d/CID:%d/ �ź� ����Ʈ�� Ŭ���̾�Ʈ�� ������ �� �����߽��ϴ�.",
                            lpPktFriendDB->m_dwOwnerUID, lpPktFriendDB->m_dwOwnerCID);
                    }

                    // ģ�� �� �ź� ������ ���� ���´�. ���� �α����� �Ϸ�ȴ�.
                    lpCharacter->Login();
                }
            }
        }
    }

    return true;
}

bool CChatDispatch::ParseFriendAck(PktFriendAck* lpPktFriendAck)
{
    switch(lpPktFriendAck->m_cCmd)
    {
    case PktFriendAck::FRIEND_LOGIN_TO_GAME:
    case PktFriendAck::FRIEND_LOGOUT_TO_GAME:

        // ��Ŷ ���� üũ`
        if (sizeof(PktFriendAck) + sizeof(unsigned long) <= lpPktFriendAck->GetLen())
        {
            unsigned long* lpdwPos = reinterpret_cast<unsigned long*>(lpPktFriendAck + 1);
            unsigned long dwCIDNum = *lpdwPos;
            
            if (*lpdwPos == ((lpPktFriendAck->GetLen() - sizeof(PktFriendAck) - sizeof(unsigned long)) / sizeof(unsigned long)))
            {
                ++lpdwPos;

                unsigned char cAckCmd  = (lpPktFriendAck->m_cCmd == PktFriendAck::FRIEND_LOGIN_TO_GAME) ?
                    PktFriendAck::FRIEND_LOGIN : PktFriendAck::FRIEND_LOGOUT;

                unsigned long* lpdwEnd = lpdwPos + dwCIDNum;

                for(; lpdwPos != lpdwEnd; ++lpdwPos)
                {
                    CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(*lpdwPos);

                    if (0 != lpCharacter)
                    {
                        CFriendList::Rebind* lpRebind = 
                            lpCharacter->GetFriendList().GetFriend(lpPktFriendAck->m_dwCID);

                        if (0 != lpRebind && lpRebind->IsFriend(lpPktFriendAck->m_szName))
                        {
							bool bUpdate = false;

							if(lpRebind->GetLevel()!=lpPktFriendAck->m_cLevel || lpRebind->GetGID()!=lpPktFriendAck->m_dwGID
								|| lpRebind->GetClass()!=lpPktFriendAck->m_wClass)
							{
								bUpdate = true;
							}

							lpRebind->UpdateServerID(lpPktFriendAck->m_dwServerID);
                            lpRebind->SetLoginStatus(lpPktFriendAck->m_cCmd == PktFriendAck::FRIEND_LOGIN_TO_GAME);

							if(cAckCmd==PktFriendAck::FRIEND_LOGIN)
							{
								// ���� ���� //
								lpRebind->UpdateLevel(lpPktFriendAck->m_cLevel);
								lpRebind->UpdateGID(lpPktFriendAck->m_dwGID);
								lpRebind->UpdateClass(lpPktFriendAck->m_wClass);

								// ������ �����Ǿ����� DB Agent �� ������Ʈ �Ѵ� //
								if(bUpdate)
								{
									GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

									if(lpDBAgentDispatch)
									{
										char* lpBuffer = lpDBAgentDispatch->GetSendStream().GetBuffer(sizeof(PktFriendDB));

										if(lpBuffer)
										{
											PktFriendDB* lpPktFriendDB = reinterpret_cast<PktFriendDB*>(lpBuffer);

											lpPktFriendDB->m_dwOwnerUID		= lpCharacter->GetUID();
											lpPktFriendDB->m_dwOwnerCID		= lpCharacter->GetCID();
											lpPktFriendDB->m_dwReferenceUID	= 0;
											lpPktFriendDB->m_dwReferenceCID	= lpRebind->GetCID();
											lpPktFriendDB->m_dwData			= 0;
											lpPktFriendDB->m_cCmd			= PktFriendDB::FRIEND_INFO;
											lpPktFriendDB->m_dwGID			= lpRebind->GetGID();
											lpPktFriendDB->m_wClass			= lpRebind->GetClass();
											lpPktFriendDB->m_cLevel			= lpRebind->GetLevel();
											lpPktFriendDB->m_dwServerID		= lpRebind->GetServerID();

											lpDBAgentDispatch->GetSendStream().WrapCrypt(sizeof(PktFriendDB), CmdFriendDB, 0, 0);
										}	
									}
								}
							}																					
                        }

                        CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

                        if (0 != lpDispatch)
                        {
                            CSendStream& sendStream = lpDispatch->GetSendStream();

                            GameClientSendPacket::SendCharFriendAck(sendStream, 
                                cAckCmd, lpPktFriendAck->m_dwCID, lpPktFriendAck->m_szName, lpPktFriendAck->m_dwGID, 
								lpPktFriendAck->m_wClass, lpPktFriendAck->m_cLevel, lpPktFriendAck->m_dwServerID, lpPktFriendAck->GetError());							
                        }
                    }
                }                
            }
        }
        break;
	case PktFriendAck::BAN_LOGIN_TO_GAME:
	case PktFriendAck::BAN_LOGOUT_TO_GAME:
		{
			// ��Ŷ ���� üũ`
			if (sizeof(PktFriendAck) + sizeof(unsigned long) <= lpPktFriendAck->GetLen())
			{
				unsigned long* lpdwPos = reinterpret_cast<unsigned long*>(lpPktFriendAck + 1);
				unsigned long dwCIDNum = *lpdwPos;

				if (*lpdwPos == ((lpPktFriendAck->GetLen() - sizeof(PktFriendAck) - sizeof(unsigned long)) / sizeof(unsigned long)))
				{
					++lpdwPos;

					unsigned char cAckCmd  = (lpPktFriendAck->m_cCmd == PktFriendAck::BAN_LOGIN_TO_GAME) ?
						PktFriendAck::BAN_LOGIN : PktFriendAck::BAN_LOGOUT;

					unsigned long* lpdwEnd = lpdwPos + dwCIDNum;

					for(; lpdwPos != lpdwEnd; ++lpdwPos)
					{
						CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(*lpdwPos);

						if (0 != lpCharacter)
						{
							CBanList::Rebind* lpRebind = 
								lpCharacter->GetBanList().GetBan(lpPktFriendAck->m_dwCID);

							if (0 != lpRebind && lpRebind->IsBan(lpPktFriendAck->m_szName))
							{
								bool bUpdate = false;

								if(lpRebind->GetLevel()!=lpPktFriendAck->m_cLevel || lpRebind->GetGID()!=lpPktFriendAck->m_dwGID
									|| lpRebind->GetClass()!=lpPktFriendAck->m_wClass)
								{
									bUpdate = true;
								}

								lpRebind->UpdateServerID(lpPktFriendAck->m_dwServerID);								

								if(cAckCmd==PktFriendAck::BAN_LOGIN)
								{
									// ���� ���� //
									lpRebind->UpdateLevel(lpPktFriendAck->m_cLevel);
									lpRebind->UpdateGID(lpPktFriendAck->m_dwGID);
									lpRebind->UpdateClass(lpPktFriendAck->m_wClass);

									// ������ �����Ǿ����� DB Agent �� ������Ʈ �Ѵ� //									
									if(bUpdate)
									{
										GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

										if(lpDBAgentDispatch)
										{
											char* lpBuffer = lpDBAgentDispatch->GetSendStream().GetBuffer(sizeof(PktFriendDB));

											if(lpBuffer)
											{
												PktFriendDB* lpPktFriendDB = reinterpret_cast<PktFriendDB*>(lpBuffer);

												lpPktFriendDB->m_dwOwnerUID		= lpCharacter->GetUID();
												lpPktFriendDB->m_dwOwnerCID		= lpCharacter->GetCID();
												lpPktFriendDB->m_dwReferenceUID	= 0;
												lpPktFriendDB->m_dwReferenceCID	= lpRebind->GetCID();
												lpPktFriendDB->m_dwData			= 0;
												lpPktFriendDB->m_cCmd			= PktFriendDB::BAN_INFO;
												lpPktFriendDB->m_dwGID			= lpRebind->GetGID();
												lpPktFriendDB->m_wClass			= lpRebind->GetClass();
												lpPktFriendDB->m_cLevel			= lpRebind->GetLevel();
												lpPktFriendDB->m_dwServerID		= lpRebind->GetServerID();

												lpDBAgentDispatch->GetSendStream().WrapCrypt(sizeof(PktFriendDB), CmdFriendDB, 0, 0);
											}	
										}
									}
								}
							}

							CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

							if (0 != lpDispatch)
							{
								CSendStream& sendStream = lpDispatch->GetSendStream();

								GameClientSendPacket::SendCharFriendAck(sendStream, 
									cAckCmd, lpPktFriendAck->m_dwCID, lpPktFriendAck->m_szName, lpPktFriendAck->m_dwGID, lpPktFriendAck->m_wClass,
									lpPktFriendAck->m_cLevel, lpPktFriendAck->m_dwServerID, lpPktFriendAck->GetError());							
							}
						}
					}                
				}
			}
		}
		break;
	case PktFriendAck::FRIEND_INFO_UPDATE:
		{
			if (sizeof(PktFriendAck) + sizeof(unsigned long) <= lpPktFriendAck->GetLen())
			{
				unsigned long* lpdwPos = reinterpret_cast<unsigned long*>(lpPktFriendAck + 1);
				unsigned long dwCIDNum = *lpdwPos;

				if (*lpdwPos == ((lpPktFriendAck->GetLen() - sizeof(PktFriendAck) - sizeof(unsigned long)) / sizeof(unsigned long)))
				{
					++lpdwPos;

					unsigned char cAckCmd  = PktFriendAck::FRIEND_INFO_UPDATE;
						
					unsigned long* lpdwEnd = lpdwPos + dwCIDNum;

					for(; lpdwPos != lpdwEnd; ++lpdwPos)
					{
						CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(*lpdwPos);

						if (lpCharacter)
						{
							CFriendList::Rebind* lpRebind = lpCharacter->GetFriendList().GetFriend(lpPktFriendAck->m_dwCID);

							if (lpRebind && lpRebind->IsFriend(lpPktFriendAck->m_szName))
							{
								bool bUpdate = false;

								if(lpRebind->GetLevel()!=lpPktFriendAck->m_cLevel || lpRebind->GetGID()!=lpPktFriendAck->m_dwGID
									|| lpRebind->GetClass()!=lpPktFriendAck->m_wClass)
								{
									bUpdate = true;
								}

								// ���� ���� //
								lpRebind->UpdateLevel(lpPktFriendAck->m_cLevel);
								lpRebind->UpdateGID(lpPktFriendAck->m_dwGID);
								lpRebind->UpdateServerID(lpPktFriendAck->m_dwServerID);
								lpRebind->UpdateClass(lpPktFriendAck->m_wClass);	

								// ������ �����Ǿ����� DB Agent �� ������Ʈ �Ѵ� //
								if(bUpdate)
								{
									GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

									if(lpDBAgentDispatch)
									{
										char* lpBuffer = lpDBAgentDispatch->GetSendStream().GetBuffer(sizeof(PktFriendDB));

										if(lpBuffer)
										{
											PktFriendDB* lpPktFriendDB = reinterpret_cast<PktFriendDB*>(lpBuffer);

											lpPktFriendDB->m_dwOwnerUID		= lpCharacter->GetUID();
											lpPktFriendDB->m_dwOwnerCID		= lpCharacter->GetCID();
											lpPktFriendDB->m_dwReferenceUID	= 0;
											lpPktFriendDB->m_dwReferenceCID	= lpRebind->GetCID();
											lpPktFriendDB->m_dwData			= 0;
											lpPktFriendDB->m_cCmd			= PktFriendDB::FRIEND_INFO;
											lpPktFriendDB->m_dwGID			= lpRebind->GetGID();
											lpPktFriendDB->m_wClass			= lpRebind->GetClass();
											lpPktFriendDB->m_cLevel			= lpRebind->GetLevel();
											lpPktFriendDB->m_dwServerID		= lpRebind->GetServerID();

											lpDBAgentDispatch->GetSendStream().WrapCrypt(sizeof(PktFriendDB), CmdFriendDB, 0, 0);
										}	
									}
								}															
							}

							CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

							if(lpDispatch)
							{
								CSendStream& sendStream = lpDispatch->GetSendStream();

								GameClientSendPacket::SendCharFriendAck(sendStream, 
									cAckCmd, lpPktFriendAck->m_dwCID, lpPktFriendAck->m_szName, lpPktFriendAck->m_dwGID,
									lpPktFriendAck->m_wClass, lpPktFriendAck->m_cLevel, lpPktFriendAck->m_dwServerID, lpPktFriendAck->GetError());							
							}
						}
					}                
				}
			}
		}
		break;
	case PktFriendAck::BAN_INFO_UPDATE:
		{
			if (sizeof(PktFriendAck) + sizeof(unsigned long) <= lpPktFriendAck->GetLen())
			{
				unsigned long* lpdwPos = reinterpret_cast<unsigned long*>(lpPktFriendAck + 1);
				unsigned long dwCIDNum = *lpdwPos;

				if (*lpdwPos == ((lpPktFriendAck->GetLen() - sizeof(PktFriendAck) - sizeof(unsigned long)) / sizeof(unsigned long)))
				{
					++lpdwPos;

					unsigned char cAckCmd  = PktFriendAck::BAN_INFO_UPDATE;

					unsigned long* lpdwEnd = lpdwPos + dwCIDNum;

					for(; lpdwPos != lpdwEnd; ++lpdwPos)
					{
						CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(*lpdwPos);

						if (lpCharacter)
						{
							CBanList::Rebind* lpRebind = lpCharacter->GetBanList().GetBan(lpPktFriendAck->m_dwCID);

							if (lpRebind)
							{
								bool bUpdate = false;

								if(lpRebind->GetLevel()!=lpPktFriendAck->m_cLevel || lpRebind->GetGID()!=lpPktFriendAck->m_dwGID
									|| lpRebind->GetClass()!=lpPktFriendAck->m_wClass)
								{
									bUpdate = true;
								}

								// ���� ���� //
								lpRebind->UpdateLevel(lpPktFriendAck->m_cLevel);
								lpRebind->UpdateGID(lpPktFriendAck->m_dwGID);
								lpRebind->UpdateServerID(lpPktFriendAck->m_dwServerID);
								lpRebind->UpdateClass(lpPktFriendAck->m_wClass);

								// ������ �����Ǿ����� DB Agent �� ������Ʈ �Ѵ� //
								if(bUpdate)
								{
									GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

									if(lpDBAgentDispatch)
									{
										char* lpBuffer = lpDBAgentDispatch->GetSendStream().GetBuffer(sizeof(PktFriendDB));

										if(lpBuffer)
										{
											PktFriendDB* lpPktFriendDB = reinterpret_cast<PktFriendDB*>(lpBuffer);

											lpPktFriendDB->m_dwOwnerUID		= lpCharacter->GetUID();
											lpPktFriendDB->m_dwOwnerCID		= lpCharacter->GetCID();
											lpPktFriendDB->m_dwReferenceUID	= 0;
											lpPktFriendDB->m_dwReferenceCID	= lpRebind->GetCID();
											lpPktFriendDB->m_dwData			= 0;
											lpPktFriendDB->m_cCmd			= PktFriendDB::BAN_INFO;
											lpPktFriendDB->m_dwGID			= lpRebind->GetGID();
											lpPktFriendDB->m_wClass			= lpRebind->GetClass();
											lpPktFriendDB->m_cLevel			= lpRebind->GetLevel();
											lpPktFriendDB->m_dwServerID		= lpRebind->GetServerID();

											lpDBAgentDispatch->GetSendStream().WrapCrypt(sizeof(PktFriendDB), CmdFriendDB, 0, 0);
										}	
									}
								}
							}

							CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

							if(lpDispatch)
							{
								CSendStream& sendStream = lpDispatch->GetSendStream();

								GameClientSendPacket::SendCharFriendAck(sendStream, 
									cAckCmd, lpPktFriendAck->m_dwCID, lpPktFriendAck->m_szName, lpPktFriendAck->m_dwGID,
									lpPktFriendAck->m_wClass, lpPktFriendAck->m_cLevel, lpPktFriendAck->m_dwServerID, lpPktFriendAck->GetError());							
							}
						}
					}                
				}
			}
		}
		break;
    }

    return true;
}


class CSendChatPacket
{
public:

    CSendChatPacket(CChatPacket& chatPacket, const char* szMessage,
        PktChat::PktChatCmd chatCmd, BattleInclination::CharData& senderData)
        :   m_chatPacket(chatPacket), m_szMessage(szMessage),
            m_cChatCmd(chatCmd), m_senderData(senderData)
    {
    
    }

    bool operator () (CCharacter* lpCharacter)
    {
        if (0 != lpCharacter)
        {
            bool bSend = true;

			// ��ġ���̸鼭 ��� ����� ���� ����� �ƴ� ���
            if (PktChat::SHOUT == m_cChatCmd || PktChat::TRADE == m_cChatCmd)
            {
                if (lpCharacter->GetRejectOption().Reject.m_Shout)
                {
                    // �ź� ���õ� ��쿡 ������ ����
                    bSend = false;
                }
                else if (m_senderData.m_dwCID != lpCharacter->GetCID())
                {
					unsigned char cResult = 0;

					// ���� ����� �޴� ����� �ٸ� ��쿡�� üũ
					BattleInclination::CharData recevierData;
					BattleInclination::SetCharData(*lpCharacter, recevierData);

					// RelationData�� �ٸ� ���� �ִ� ĳ���Ϳʹ� ���� ���� �����ͷ� �����ȴ�.
					// (�ʿ������� ä�� ���� <-> ���� ���� ���� ��Ŷ �� ��Ը� ���� �ʿ�)
					BattleInclination::RelationData	relationInfo;
					relationInfo.m_cVSFlag = 0;
					relationInfo.m_bHostileGuild = false;

					// ��밡 ���� �����̰ų� ��ڸ� ����
					bSend = (0 < m_senderData.m_cAdminLevel || 0 < recevierData.m_cAdminLevel || 
							 m_senderData.m_cNation == recevierData.m_cNation);
                }
            }

            if (bSend)
            {
                CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
                if (0 != lpDispatch)
                {
                    lpDispatch->GetSendStream().PutBuffer(
                        m_chatPacket.GetCompressedPacket(), 
                        m_chatPacket.GetCompressedSize(), CmdCharChat);
                }
            }
        }

        return true;
    }

private:

    CChatPacket&                    m_chatPacket;
    const char*                     m_szMessage;
    PktChat::PktChatCmd             m_cChatCmd;
    BattleInclination::CharData&    m_senderData;
};



bool CChatDispatch::ParseCharChat(PktBase* lpPktBase)
{
    char*               szMessage       = 0;
    char*               szSenderName    = 0;
    PktChat::PktChatCmd cChatCmd        = PktChat::NORMAL;
    unsigned long*      lpdwCIDs        = 0;
	unsigned char       cCIDNum         = 0;
    unsigned short      usLang          = 0;

    BattleInclination::CharData senderData;
    memset(&senderData, 0, sizeof(BattleInclination::CharData));

    if (CServerChatPacket::HandleServerChatPacket(lpPktBase, szMessage, 
        cChatCmd, usLang, senderData, szSenderName, lpdwCIDs, cCIDNum))
    {
        // INFLOG4(g_Log, "CID:%10u / TargetCIDNum:%3d / ä�� �����κ��� ä�� ��Ŷ ���� : [%2d]%s", 
        //    senderData.m_dwCID, cCIDNum, cChatCmd, szMessage);
        

        switch(cChatCmd)
        {
        case PktChat::NORMAL:
        case PktChat::CLIENT_LOG:
        case PktChat::STALL:
		case PktChat::CAMP_SHOP:
        case PktChat::ADMIN_NORMAL_CHAT:
            // ����� �� �� ����.
            break;

        case PktChat::WHISPER:

            // �ӼӸ��� ���, �ӼӸ� ��Ŷ���� ������.
            if (0 != lpdwCIDs && 1 == cCIDNum)
            {
                // �ӼӸ��� Ÿ���� �Ѹ��̴�.
                CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(*lpdwCIDs);

                if (0 != lpCharacter)
                {
                    CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
                    if (0 != lpDispatch)
                    {
                        char szWhisperPacket[sizeof(PktWh) + PktChat::PktChatMaxSize * 2];
                        
                        PktWh* lpPktWh = reinterpret_cast<PktWh*>(szWhisperPacket);

                        const char* szWhSenderName = 0;
                        const char* szWhReceiverName = 0;

                        // �ӼӸ� ������ ��쿡�� SenderName�� TargetCID�� �ݴ�� ���´�.
                        // (SenderName <- ReceiverName, TargetCID <- SenderCID)

                        if (0 == lpPktBase->GetError())
                        {
                            szWhSenderName      = szSenderName;
                            szWhReceiverName    = lpCharacter->GetCharacterName();
                        }
                        else
                        {
                            szWhSenderName      = lpCharacter->GetCharacterName();
                            szWhReceiverName    = szSenderName;
                        }

                        strncpy(lpPktWh->m_SenderName, szWhSenderName, CHAR_INFOST::MAX_NAME_LEN);
                        strncpy(lpPktWh->m_RecverName, szWhReceiverName, CHAR_INFOST::MAX_NAME_LEN);

                        lpPktWh->m_SenderName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
                        lpPktWh->m_RecverName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

                        int nLength = _snprintf(szWhisperPacket + sizeof(PktWh), 
                            PktChat::PktChatMaxSize - 1, "%s", szMessage);

                        if (0 < nLength)
                        {
                            *(szWhisperPacket + sizeof(PktWh) + nLength) = 0;

                            lpDispatch->GetSendStream().WrapCompress(szWhisperPacket, 
                                static_cast<unsigned short>(sizeof(PktWh) + nLength + 1),
                                CmdCharWhisper, 0, lpPktBase->GetError());
                        }
                    }
                }
            }
            break;

        case PktChat::DICE:
        case PktChat::PARTY:
        case PktChat::FRIEND:
        case PktChat::GUILD:
        
            // SHOUT / ADMIN_SHOT / NOTICE�����, Target�� �����Ǿ� ������ TargetChat���� ����
        case PktChat::SHOUT:
        case PktChat::TRADE:
        case PktChat::ADMIN_SHOUT:
        case PktChat::NOTICE:
		case PktChat::NOTIFY_CHAR_INFO:
		case PktChat::ENEMY_CHECK:

            // Target chat���� �´�.
            if (0 != lpdwCIDs && 0 != cCIDNum)
            {
                unsigned long* lpdwCIDEnd = lpdwCIDs + cCIDNum;

                CChatPacket chatPacket(szMessage, 
                    senderData.m_dwCID, cChatCmd, usLang, 0, lpPktBase->GetError());

                if (chatPacket.IsValid())
                {
                    CGuild* lpGuild = (cChatCmd == PktChat::GUILD && (0 != senderData.m_dwGID)) ? 
                        lpGuild = CGuildMgr::GetInstance().GetGuild(senderData.m_dwGID) : 0;

                    for(; lpdwCIDs != lpdwCIDEnd; ++lpdwCIDs)
                    {
                        CCharacter*             lpCharacter = CCreatureManager::GetInstance().GetCharacter(*lpdwCIDs);
                        CGameClientDispatch*    lpDispatch  = 0;
                        
                        if (0 != lpCharacter && 0 != (lpDispatch = lpCharacter->GetDispatcher()))
                        {
                            bool bSendChatPacket = false;

                            switch(cChatCmd)
                            {
                            case PktChat::GUILD:
                                bSendChatPacket = (0 != lpGuild && Guild::JOIN_WAIT != lpGuild->GetTitle(*lpdwCIDs));
                                break;

                            default:
                                bSendChatPacket = true;
                                break;
                            }

                            if (bSendChatPacket)
                            {
                                lpDispatch->GetSendStream().PutBuffer(
                                    chatPacket.GetCompressedPacket(),
                                    chatPacket.GetCompressedSize(), CmdCharChat);
                            }
                        }
                    }
                }
            }
            else
            {
                // SHOUT / ADMIN_SHOT / NOTICE �� Target�� �����Ǿ� ���� ������, ��ο��� ������.
                switch(cChatCmd)
                {
                case PktChat::SHOUT:
                case PktChat::TRADE:
                case PktChat::ADMIN_SHOUT:
                case PktChat::NOTICE:
        
                    // Target ������ ������� ������ ��� ĳ�������� �����Ѵ�.
                    {
                        CChatPacket chatPacket(szMessage, 
                            senderData.m_dwCID, cChatCmd, usLang, 0, lpPktBase->GetError());

                        if (chatPacket.IsValid())
                        {
                            CCreatureManager::GetInstance().ProcessAllCharacter(
                                CSendChatPacket(chatPacket, szMessage, cChatCmd, senderData));                         
                        }
                    }
                    break;
                }
            }
            break;
        }
    }

    return true;
}

bool CChatDispatch::ParseChatBan(PktChatBan* lpPktChatBan)
{
	unsigned long dwTargetCID	= lpPktChatBan->m_dwTargetCID;
	unsigned long dwMinutes		= lpPktChatBan->m_dwMinutes;

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwTargetCID);
	if (NULL != lpCharacter)
	{
		lpCharacter->SetChatBan((0 == dwMinutes) ? false : true);

		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharChatBan(lpDispatch->GetSendStream(), 0, dwTargetCID, dwMinutes);
		}
	}

	return true;
}