#include "stdafx.h"

#include "ChatLog.h"

#include "ChatGameServerDispatch.h"
#include "ChatToolServerDispatch.h"
#include "ChatClientDispatch.h"

#include <Log/ServerLog.h>

#include <DB/DBComponent.h>
#include <DB/AdminDBComponent.h>

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/FriendPacket.h>
#include <Network/Broadcast/ChatCharData.h>

#include <Creature/Creature.h>
#include <Creature/Character/Character.h>

#include <boost/pool/pool_alloc.hpp>

#include <map>
#include <utility>
#include <functional>
#include <boost/pool/pool_alloc.hpp>

#include <Utility/Math/Math.h>

#include <Community/Guild/GuildMgr.h>

CMultiDispatch& CChatGameServerDispatch::GetDispatchTable()
{
    static CMultiDispatch chatGameDispatchTable;
    return chatGameDispatchTable;
}

// ���� ģ���� ������ ��� ����鿡�� ������ �� ����.
class CFindAllFriended
{
public:    

	// Key : ServerID, Value : CID
	typedef std::multimap<unsigned long, unsigned long, std::less<unsigned long>, 
		boost::fast_pool_allocator< std::pair<const unsigned long, unsigned long> > > FriendedMap;

    CFindAllFriended(FriendedMap& friendedMap, CBanList& banList, const char* szMyName, bool bLogin);

    bool operator() (CXRefFriends::XRefTable::value_type& friendCIDPair);

private:

    FriendedMap&    m_friendedMap;	

    CBanList&       m_banList;
    const char*     m_szMyName;

    bool            m_bLogin;	
};

// ���� �źη� ������ ��� ����鿡�� ������ �� ����.
class CFindAllBaned
{
public:    

	// Key : ServerID, Value : CID
	typedef std::multimap<unsigned long, unsigned long, std::less<unsigned long>, 
		boost::fast_pool_allocator< std::pair<const unsigned long, unsigned long> > > BanedMap;

	CFindAllBaned(BanedMap& banMap, const char* szMyName);

	bool operator() (CXRefBans::XRefTable::value_type& banCIDPair);

private:

	BanedMap&		m_bandMap;	
	
	const char*     m_szMyName;	
};

// ���� ģ���� ������ �ִ� ��� ������� �α��� ������ ��� �ͼ� �����Ѵ�.
class CUpdateFriendLoginStatus
{
public:

    CUpdateFriendLoginStatus(unsigned long dwOwnerCID, const char* szOwnerName);
    bool operator() (CFriendList::Rebind& rebinds);

private:

    ChatData::CCharInfoMap& m_chatDataMap;
    unsigned long           m_dwOwnerCID;
    const char*             m_szOwnerName;
};

class CSendFriendAck
{
public:

    CSendFriendAck(unsigned long dwCID, const char* szName, unsigned long dwGID, unsigned short wClass, char cLevel,
					unsigned long dwServerID, CFindAllFriended::FriendedMap& targets, unsigned char cAckCmd, unsigned short usError);
        
    bool operator () (unsigned long dwServerID, CPacketDispatch& dispatch);

private:

    unsigned long                   m_dwCID;
    const char*                     m_szName;
	unsigned long                   m_dwGID;
	unsigned short					m_wClass;
	char							m_cLevel;
	unsigned long                   m_dwServerID;


    CFindAllFriended::FriendedMap&  m_targets;
    unsigned short                  m_usError;
    unsigned char                   m_cAckCmd;
};

class CSendChatTargetGameServer
{
public:

    CSendChatTargetGameServer(CServerChatPacket::TargetList& targetList, 
        BattleInclination::CharData& senderData, const char* szSenderName,
        const char* szMessage, PktChat::PktChatCmd cChatCmd, unsigned short usLang);
    
    bool operator () (unsigned long dwServerID, CPacketDispatch& dispatch);

private:
    
    CServerChatPacket::TargetList&  m_TargetList;
    const char*                     m_szMessage;
    PktChat::PktChatCmd             m_cChatCmd;
    BattleInclination::CharData&    m_senderData;
    const char*                     m_szSenderName;
	unsigned short					m_usLang;
};



class CFillTargetCIDs
{
public:

    CFillTargetCIDs(CServerChatPacket::TargetList& targetList, unsigned long dwExceptCID)
        :   m_TargetList(targetList), m_dwExceptCID(dwExceptCID) { }

    bool operator() (ChatData::CCharInfo& charInfo)
    {
        if (m_dwExceptCID != charInfo.GetCID())
        {
            m_TargetList.insert(std::make_pair(
                charInfo.GetServerID(), charInfo.GetCID()));
        }
       
        return true;
    }
    
private:

    CServerChatPacket::TargetList&  m_TargetList;
    unsigned long                   m_dwExceptCID;
};



CChatGameServerDispatch::CChatGameServerDispatch(CSession& Session)
:   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
    m_dwServerID(0)
{
    
}

void CChatGameServerDispatch::Connected()
{
	DETLOG2(g_Log, "this:0x%p/GameServer(ID:0x%08x) connected.", this, m_dwServerID);
}

void CChatGameServerDispatch::Disconnected()
{
    // ���ٸ� ó�� ����.
    if (0 != m_dwServerID)
    {
		DETLOG2(g_Log, "this:0x%p/GameServer(ID:0x%08x) disconnected.", this, m_dwServerID);        
        ChatData::CCharInfoMap::GetInstance().RemoveCharInfo(m_dwServerID);
    }    

    GetDispatchTable().RemoveDispatch(m_dwServerID);
}

bool CChatGameServerDispatch::DispatchPacket(PktBase* lpPktBase)
{
	bool bResult = false;

    switch (lpPktBase->GetCmd())
    {
		case CmdSysServerLogin:     bResult = ProcessServerLogin(static_cast<PktSL*>(lpPktBase));			break;
		case CmdChatInfoChanged:    bResult = ProcessCharInfoChange(static_cast<PktChatData*>(lpPktBase));	break;
		case CmdFriendDB:           bResult = ProcessUpdateFriendDB(static_cast<PktFriendDB*>(lpPktBase));	break;
		case CmdCharChat:           bResult = ProcessChat(static_cast<PktChatRequest*>(lpPktBase));			break;
		case CmdChatBan:			bResult = ProcessChatBan(static_cast<PktChatBan*>(lpPktBase));			break;
		case CmdChatBanInfo:		bResult = ProcessChatBanInfo(static_cast<PktChatBanInfo*>(lpPktBase));	break;

		default:

			bResult = true;
			ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Unknown Command packet", this, lpPktBase->GetCmd());
			break;
	}

	if (!bResult)
	{
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Packet process failed", this, lpPktBase->GetCmd());
	}

	return true;
}

// ���� �α���
bool CChatGameServerDispatch::ProcessServerLogin(PktSL* lpPktSL)
{
    m_dwServerID = lpPktSL->m_dwServerID;

    PktSLAck* lpPktSLAck = reinterpret_cast<PktSLAck*>(m_SendStream.GetBuffer(sizeof(PktSLAck)));
    if (NULL != lpPktSLAck)
    {
        memset(lpPktSLAck, 0, sizeof(PktSLAck));

        if (m_SendStream.WrapHeader(sizeof(PktSLAck), CmdSysServerLogin, 0, 0))
        {
            DETLOG1(g_Log, "���Ӽ���(ID:0x%08x) �� ����Ǿ����ϴ�.", m_dwServerID);

            GetDispatchTable().SetDispatch(m_dwServerID, this);
            return true;
        }
    }

    return false;
}


// ĳ���� ���� �����
bool CChatGameServerDispatch::ProcessCharInfoChange(PktChatData* lpChatData)
{
    int nDataLen = lpChatData->GetLen() - sizeof(PktChatData);
    const char* szDataPos = reinterpret_cast<char*>(lpChatData + 1);

    ChatData::CCharInfo* lpCharInfo = 0;

    switch (lpChatData->m_cType)
    {
		case PktChatData::LOGIN:
		{
			// �ű� ������ ����
			lpCharInfo = new ChatData::CCharInfo(lpChatData->m_dwUID, lpChatData->m_dwCID, 
				m_dwServerID, &CXRefFriends::GetInstance(), &CXRefBans::GetInstance());

			if (NULL == lpCharInfo ||
				!lpCharInfo->UpdateData(szDataPos, nDataLen) ||
				!ChatData::CCharInfoMap::GetInstance().AddCharInfo(lpCharInfo))
			{
				delete lpCharInfo;
			}

            break;
		}

		case PktChatData::LOGOUT:
		{
			// ���� ������ ����
			lpCharInfo = ChatData::CCharInfoMap::GetInstance().RemoveCharInfo(
				lpChatData->m_dwUID, lpChatData->m_dwCID, m_dwServerID);

			if (NULL != lpCharInfo)
			{
				// ĳ���� ���� ���� ����. ���� �α׾ƿ��Ѵٴ� ����� �˸���.
				CFindAllFriended::FriendedMap friendedMap;				

				CXRefFriends::GetInstance().Process(lpCharInfo->GetCID(), 
					CFindAllFriended(friendedMap, lpCharInfo->GetBanList(), lpCharInfo->GetName(), false));

				GetDispatchTable().Process(CSendFriendAck(lpCharInfo->GetCID(), lpCharInfo->GetName(), 0, 0, 0, 0,
					friendedMap, PktFriendAck::FRIEND_LOGOUT_TO_GAME, 0));

				// �ź� ����Ʈ �α׾ƿ�. //
				CFindAllBaned::BanedMap	banedMap;

				CXRefBans::GetInstance().Process(lpCharInfo->GetCID(), CFindAllBaned(banedMap, lpCharInfo->GetName()));

				GetDispatchTable().Process(CSendFriendAck(lpCharInfo->GetCID(), lpCharInfo->GetName(), 0, 0, 0, 0,
					banedMap, PktFriendAck::BAN_LOGOUT_TO_GAME, 0));
	            
				delete lpCharInfo;
			}
			break;
		}

		case PktChatData::DELTA:
		{
			// ���� ������ ����
			ChatData::CCharInfoMap::GetInstance().Update(lpChatData->m_dwUID, 
				lpChatData->m_dwCID, m_dwServerID, szDataPos, nDataLen);
			break;
		}
    }
    
    return true;
}



// ���Ӽ��� -> ä�ü����� �ִ� Ŀ�ǵ�.
bool CChatGameServerDispatch::ProcessUpdateFriendDB(PktFriendDB* lpFriendDB)
{
    ChatData::CCharInfo* lpCharInfo = 
        ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(lpFriendDB->m_dwOwnerCID);

    ChatData::CCharInfo* lpReferenceCharInfo = 0;

    if (0 != lpCharInfo)
    {
        if (lpCharInfo->GetUID() == lpFriendDB->m_dwOwnerUID &&
            lpCharInfo->GetServerID() == m_dwServerID)
        {            
            CFriendList&    friendList  = lpCharInfo->GetFriendList();
            CBanList&       banList     = lpCharInfo->GetBanList();

            switch (lpFriendDB->m_cCmd)
            {
            case PktFriendDB::ADD_FRIEND:       // ģ�� ���     (GameServer->DBAgent) Owner�� Reference�� ���.

                lpReferenceCharInfo = ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(lpFriendDB->m_dwReferenceCID);

                if (0 != lpReferenceCharInfo && 
                    lpReferenceCharInfo->GetUID() == lpFriendDB->m_dwReferenceUID)
                {
                    friendList.Add(lpFriendDB->m_dwReferenceCID, lpReferenceCharInfo->GetName(), lpFriendDB->m_dwGID, lpFriendDB->m_wClass, lpFriendDB->m_cLevel, lpFriendDB->m_dwServerID);
                }
                break;

            case PktFriendDB::REMOVE_FRIEND:    // ģ�� ����     (GameServer->DBAgent) Owner�� Reference�� ����.

                friendList.Remove(lpFriendDB->m_dwReferenceCID);
                break;

            case PktFriendDB::ADD_BAN:          // �ź� ���     (GameServer->DBAgent) Owner�� Reference�� ���.

                lpReferenceCharInfo = ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(lpFriendDB->m_dwReferenceCID);

                if (0 != lpReferenceCharInfo && 
                    lpReferenceCharInfo->GetUID() == lpFriendDB->m_dwReferenceUID)
                {
                    banList.Add(lpFriendDB->m_dwReferenceCID, lpReferenceCharInfo->GetName(), lpFriendDB->m_dwGID, 
						lpFriendDB->m_wClass, lpFriendDB->m_cLevel, lpFriendDB->m_dwServerID);
                }
                break;

            case PktFriendDB::REMOVE_BAN:       // �ź� ����     (GameServer->DBAgent) Owner�� Reference�� ����.
                banList.Remove(lpFriendDB->m_dwReferenceCID);
                break;

            case PktFriendDB::SETGROUP:         // �׷� ����     (GameServer->DBAgent) Owner�� Reference�� m_dwData�� ��� �ִ� �׷����� ����.
                {
                    CFriendList::Rebind* lpFriendRebind = friendList.GetFriend(lpFriendDB->m_dwReferenceCID);
                    if (NULL != lpFriendRebind)
                    {
                        lpFriendRebind->SetGroup(lpFriendDB->m_dwData);
                    }
                }

            case PktFriendDB::BAN_LIST:
				{
					// FriendList���� ���� �ͼ� ���õȴ�.
					banList.SerializeIn(reinterpret_cast<char*>(lpFriendDB + 1), lpFriendDB->m_dwData);

					// �ź� ����Ʈ �α���. //
					CFindAllBaned::BanedMap	banedMap;
									
					CXRefBans::GetInstance().Process(lpCharInfo->GetCID(), CFindAllBaned(banedMap, lpCharInfo->GetName()));

					GetDispatchTable().Process(CSendFriendAck(lpCharInfo->GetCID(), lpCharInfo->GetName(), lpFriendDB->m_dwGID, lpFriendDB->m_wClass,
						lpFriendDB->m_cLevel, lpFriendDB->m_dwServerID, banedMap, PktFriendAck::BAN_LOGIN_TO_GAME, 0));
				}
                break;

            case PktFriendDB::FRIEND_LIST:
                friendList.SerializeIn(reinterpret_cast<char*>(lpFriendDB + 1), lpFriendDB->m_dwData);

                // ���� ģ���� ������ �ִ� ��� ����鿡��, ���� �α����ߴٴ� ����� ����. 
                // ��, �� Ban����Ʈ�� ���� ���� ���� �����..

                {
                    CFindAllFriended::FriendedMap friendedMap;					

                    CXRefFriends::GetInstance().Process(lpCharInfo->GetCID(), 
                        CFindAllFriended(friendedMap, banList, lpCharInfo->GetName(), true));

                    GetDispatchTable().Process(CSendFriendAck(lpCharInfo->GetCID(), lpCharInfo->GetName(), lpFriendDB->m_dwGID, lpFriendDB->m_wClass,
                        lpFriendDB->m_cLevel, lpFriendDB->m_dwServerID, friendedMap, PktFriendAck::FRIEND_LOGIN_TO_GAME, 0));
                }

                // �� ģ�� ����Ʈ��, �ٸ� ������� �α����ߴ��� ���θ� ����Ѵ�.
                friendList.Process(CUpdateFriendLoginStatus(lpCharInfo->GetCID(), lpCharInfo->GetName()));

                // �� ģ�� ����Ʈ��, �ٽ� ���� ������ ������.
                {
                    CCommunityListSend sendFriendList(friendList);
                    
					if (!sendFriendList.Send(lpCharInfo->GetUID(), lpCharInfo->GetCID(), lpFriendDB->m_dwGID, lpFriendDB->m_wClass, 
						lpFriendDB->m_cLevel, lpFriendDB->m_dwServerID, GetSendStream()))
                    {
                        ERRLOG2(g_Log, "UID:%d/CID:%d/ ģ�� ����Ʈ�� ���Ӽ����� ���� ������ �� �����߽��ϴ�.",
                            lpCharInfo->GetUID(), lpCharInfo->GetCID());
                    }
                }

                break;
			case PktFriendDB::FRIEND_INFO_UPDATE:
				{
					// ĳ���� �� �̵�, ������, Ŭ���� ����, ��� ����� ���Ӽ����� �����ش� //
					CFindAllFriended::FriendedMap friendedMap;					
					CFindAllBaned::BanedMap	banedMap;

					CXRefFriends::GetInstance().Process(lpCharInfo->GetCID(), 
						CFindAllFriended(friendedMap, banList, lpCharInfo->GetName(), true));

					GetDispatchTable().Process(CSendFriendAck(lpCharInfo->GetCID(), lpCharInfo->GetName(), lpFriendDB->m_dwGID, lpFriendDB->m_wClass,
						lpFriendDB->m_cLevel, lpFriendDB->m_dwServerID, friendedMap, PktFriendAck::FRIEND_INFO_UPDATE, 0));

					// �ź� ����Ʈ�� �޴´�. //					
					CXRefBans::GetInstance().Process(lpCharInfo->GetCID(), CFindAllBaned(banedMap, lpCharInfo->GetName()));

					GetDispatchTable().Process(CSendFriendAck(lpCharInfo->GetCID(), lpCharInfo->GetName(), lpFriendDB->m_dwGID, lpFriendDB->m_wClass,
						lpFriendDB->m_cLevel, lpFriendDB->m_dwServerID, banedMap, PktFriendAck::BAN_INFO_UPDATE, 0));
				}
				break;

            }
        }
        else
        {
            // SPHAWK TODO : ������ UID�ٸ�.

        }
    }
    else
    {
        // SPHAWK TODO : �α� ���� ��
    }

    return true;
}


bool CChatGameServerDispatch::ProcessChat(PktChatRequest* lpPktChatReq)
{
	SERVER_ID serverID;
	serverID.dwID = m_dwServerID;

	if (SERVER_ID::PRISON == serverID.sID.ID)
	{
		if (lpPktChatReq->m_cCmd != PktChat::NORMAL && 
			lpPktChatReq->m_cCmd != PktChat::CLIENT_LOG && 
			lpPktChatReq->m_cCmd != PktChat::ADMIN_NORMAL_CHAT && 
			lpPktChatReq->m_cCmd != PktChat::ADMIN_SHOUT &&
			lpPktChatReq->m_cCmd != PktChat::NOTIFY_CHAR_INFO &&
			lpPktChatReq->m_cCmd != PktChat::ENEMY_CHECK)
		{
			// ���� ���� ���ݵ� ĳ���ʹ� ���� ǥ��� ������ ä�ÿܿ��� �� �� �����ϴ�.
			return true;
		}
	}

	// ä�� ����
	if (lpPktChatReq->m_cCmd != PktChat::NOTIFY_CHAR_INFO && lpPktChatReq->m_cCmd != PktChat::ENEMY_CHECK &&
		0 != ChatData::CCharInfoMap::GetInstance().GetCharChatBan(lpPktChatReq->m_dwCID))
	{
		if (lpPktChatReq->m_cCmd != PktChat::CLIENT_LOG)
		{
			ERRLOG1(g_Log, "CID:0x%08x ä�� ���� ������ ĳ���Ͱ� ä�ü����� ä���� ���½��ϴ�.", lpPktChatReq->m_dwCID);
			return true;
		}
	}

    // �⺻ �Ľ�
    int nPacketLength = lpPktChatReq->GetLen();
    if (nPacketLength <= sizeof(PktChat))
    {
        ERRLOG2(g_Log, "ServerID:0x%08x/��Ŷ ���̰� �̻��մϴ�. ����:%d", 
            m_dwServerID, nPacketLength);
        return false;
    }

    int nMinPacketSize = sizeof(PktChat) + sizeof(char) * CHAR_INFOST::MAX_NAME_LEN * lpPktChatReq->m_cNum;
    int nMaxPacketSize = nMinPacketSize + PktChat::PktChatMaxSize;

    if (nPacketLength < nMinPacketSize || nMaxPacketSize < nPacketLength)
    {
        ERRLOG4(g_Log, "UID:%10u/CID:%10u/ServerID:0x%08x/��Ŷ ���̰� �̻��մϴ�. ����:%d", 
            lpPktChatReq->m_dwUID, lpPktChatReq->m_dwCID, m_dwServerID, nPacketLength);

        return false;
    }

    char* szNames   = reinterpret_cast<char*>(lpPktChatReq + 1);
    char* szMessage = szNames + sizeof(char) * CHAR_INFOST::MAX_NAME_LEN * lpPktChatReq->m_cNum;

    bool bTargetIsToolAdmin = false;

    // ���� �����÷ο츦 ���� ���ؼ�, �������� �� ���ڸ� ���δ�.
    szMessage[nPacketLength - nMinPacketSize - 1] = 0;
    
    ChatData::CCharInfo* lpCharInfo = 
        ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(lpPktChatReq->m_dwCID);

    if (0 != lpCharInfo && lpCharInfo->GetUID() == lpPktChatReq->m_dwUID)
    {
        CServerChatPacket::TargetList targetList;
        
        switch (lpPktChatReq->m_cCmd)
        {
			case PktChat::ADMIN_NORMAL_CHAT:
			
				{
					return true;
				}
/*
				// ��� ���� ������ �� ������. (�ܼ� Relay�� �Ѵ�.)
				{
					CServerChatPacket serverChatPacket(szMessage, 
						static_cast<PktChat::PktChatCmd>(lpPktChatReq->m_cCmd),
						lpCharInfo->GetInclinationData(), lpCharInfo->GetName());

					if (serverChatPacket.IsValid())
					{
						// �� ���Ӽ����� ��� �����Ѵ�.
						GetDispatchTable().Process(
							CSendChatAllGameServer(serverChatPacket));
					}
				}
				break;
*/

	        case PktChat::DICE:
				// PID�� ��ġ�ϴ� ĳ���͵� SERVERID/CID�� Targets�� �ִ´�.
				if (0 != lpCharInfo->GetPID())
				{
					// ��Ƽ�� ������� ��Ƽ���Ը� ������.
					ChatData::CCharInfoMap::GetInstance().EnumerateByPID(
						lpCharInfo->GetPID(), CFillTargetCIDs(targetList, 0));
					break;
				}

            // �� ��쿡��, ���Ӽ������� ��ü������ ó���ϹǷ� �� �ʿ� ����.
			case PktChat::NORMAL:
			case PktChat::STALL:
			case PktChat::CAMP_SHOP:
			case PktChat::FRIEND:


            // �α׸� ��´�.
			case PktChat::CLIENT_LOG:

				RULLOG3(g_Log, "UID:%10u/CID:%10u/Ŭ���̾�Ʈ �α� : %s", 
					lpCharInfo->GetUID(), lpCharInfo->GetCID(), szMessage);
				break;

			case PktChat::WHISPER:

            // szNames�� ĳ���͸� ã�Ƽ� SERVERID/CID�� Targets�� �ִ´�.
            // �ӼӸ��� ���, ���� ����ó���� �� �� �־�� �Ѵ�.. ������;;
            {
                char* szNamesPos = szNames;
                ChatData::CCharInfoMap& charInfoMap = ChatData::CCharInfoMap::GetInstance();

                for(int nNamesNum = lpPktChatReq->m_cNum; 0 < nNamesNum;
                    --nNamesNum, szNamesPos += CHAR_INFOST::MAX_NAME_LEN)
                {
                    szNamesPos[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

                    ChatData::CCharInfo* lpTargetCharInfo = 
                        charInfoMap.GetCharInfoByName(szNamesPos);

                    unsigned short usError = 0;

                    if (0 != lpTargetCharInfo)
                    {
						if (0 < lpCharInfo->GetAdminLevel() || 
							0 < lpTargetCharInfo->GetAdminLevel())
						{
							// ��ڴ� ������ �Ӹ� ����
							targetList.insert(std::make_pair(
								lpTargetCharInfo->GetServerID(), lpTargetCharInfo->GetCID()));
						}
						else if (lpCharInfo->GetRace() != lpTargetCharInfo->GetRace())
						{
							// ���� �������� ��ȭ �Ұ�
							usError = PktWh::FAIL_ENEMY;
						}
						else if (0 != lpTargetCharInfo->GetRejectOption().Reject.m_Whisper &&
							lpTargetCharInfo->GetBanList().IsBan(lpCharInfo->GetCID(), lpCharInfo->GetName()))
						{
							// ��밡 �ź�����.
							usError = PktWh::FAIL_REJECT;
						}
						else
						{
							targetList.insert(std::make_pair(
								lpTargetCharInfo->GetServerID(), lpTargetCharInfo->GetCID()));
						}
                    }
                    else
                    {
                        // �̹� �α׾ƿ�����.
                        usError = PktWh::FAIL_CHAR_LOGOUT;
                    }

                    // ����� ��� �̸� ����Ʈ�� �ִ��� ���ɴ�. 
                    // ��� �̸� ����Ʈ�� ������ ���� �ӼӸ� ������ ���� �ʿ䰡 ����.
                    bTargetIsToolAdmin = CChatAdminNames::GetInstance().HasName(szNamesPos);
                    if (0 != usError && !bTargetIsToolAdmin)
                    {
                        // �ӼӸ� ������ ��쿡�� SenderName�� TargetCID�� �ݴ�� ���´�.
                        // (SenderName <- ReceiverName, TargetCID <- SenderCID)
                        CServerChatPacket serverChatError("ERROR", PktChat::WHISPER, 0,
                            lpCharInfo->GetInclinationData(), szNamesPos,
                            &lpPktChatReq->m_dwCID, 1, 0, usError);

                        if (serverChatError.IsValid())
                        {
                            // �ӼӸ� ������ ������.
                            GetSendStream().PutBuffer(serverChatError.GetCompressedPacket(),
                                serverChatError.GetCompressedSize(), CmdCharChat);
                        }
                    }
                }
				break;
            }

			case PktChat::PARTY:
	            
				// PID�� ��ġ�ϴ� ĳ���͵� SERVERID/CID�� Targets�� �ִ´�.
				if (0 != lpCharInfo->GetPID())
				{
					ChatData::CCharInfoMap::GetInstance().EnumerateByPID(
						lpCharInfo->GetPID(), CFillTargetCIDs(targetList, lpCharInfo->GetCID()));
				}
				break;

			case PktChat::GUILD:

				// GID�� ��ġ�ϴ� ĳ���͵� SERVERID/CID�� Targets�� �ִ´�.
				if (0 != lpCharInfo->GetGID())
				{
					ChatData::CCharInfoMap::GetInstance().EnumerateByGID(
						lpCharInfo->GetGID(), CFillTargetCIDs(targetList, lpCharInfo->GetCID()));
				}
				break;

				// edith 2008.05.27 ��ġ��, Trade ��ġ�� ���� ���� �������� ����
			case PktChat::ADMIN_SHOUT:
				if (lpCharInfo->GetAdminLevel() < 1 ){return true;};
				ChatData::CCharInfoMap::GetInstance().EnumerateAll( CFillTargetCIDs(targetList, 0));
			break;

			case PktChat::TRADE:
				//if (lpCharInfo->GetAdminLevel() < 1 ){return true;};
				ChatData::CCharInfoMap::GetInstance().EnumerateAll( CFillTargetCIDs(targetList, 0));
			break;

			case PktChat::SHOUT:
			
				// PID�� ��ġ�ϴ� ĳ���͵� SERVERID/CID�� Targets�� �ִ´�.
				// ���� �������Ը� ������.
				ChatData::CCharInfoMap::GetInstance().EnumerateByRace(
					lpCharInfo->GetRace(), CFillTargetCIDs(targetList, 0));
				break;

			case PktChat::NOTIFY_CHAR_INFO:
				{
					char* strAdminCID = szNames;
					unsigned long dwAdminCID = Math::Convert::StrToHex32(strAdminCID);

					ChatData::CCharInfo* lpTargetCharInfo = 
						ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(dwAdminCID);

					if (0 != lpTargetCharInfo)
					{
						strncpy(szNames, lpTargetCharInfo->GetName(), CHAR_INFOST::MAX_NAME_LEN);
						szNames[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

						targetList.insert(std::make_pair(
							lpTargetCharInfo->GetServerID(), lpTargetCharInfo->GetCID()));
					}
				}
				break;
			case PktChat::ENEMY_CHECK:
				{
					char* strAdminCID = szNames;
					unsigned long dwAdminCID = Math::Convert::StrToHex32(strAdminCID);

					ChatData::CCharInfo* lpTargetCharInfo = 
						ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(dwAdminCID);

					if (0 != lpTargetCharInfo)
					{
						strncpy(szNames, lpTargetCharInfo->GetName(), CHAR_INFOST::MAX_NAME_LEN);
						szNames[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

						targetList.insert(std::make_pair(
							lpTargetCharInfo->GetServerID(), lpTargetCharInfo->GetCID()));
					}
				}
				break;
        }

        if (!targetList.empty())
        {
            // Ÿ���� ������, �� ���Ӽ����� �����Ѵ�.
            GetDispatchTable().Process(
                CSendChatTargetGameServer(targetList, 
                lpCharInfo->GetInclinationData(), 
                lpCharInfo->GetName(), szMessage,                
                static_cast<PktChat::PktChatCmd>(lpPktChatReq->m_cCmd), lpPktChatReq->m_usLang));
        }

        // ä�� �α׸� �����.
        if (lpPktChatReq->m_cCmd != PktChat::CLIENT_LOG)
        {            
            if (lpPktChatReq->m_cCmd == PktChat::WHISPER && 1 == lpPktChatReq->m_cNum)
            {
				CChatLog::GetInstance().WhisperLog(*lpCharInfo, lpPktChatReq->m_cZone, serverID.GetChannel(), szMessage, 
                    szNames, lpPktChatReq->m_usXPos, lpPktChatReq->m_usYPos, 
                    lpPktChatReq->m_usZPos, lpPktChatReq->m_usLang, bTargetIsToolAdmin);
            }
            else
            {
                CChatLog::GetInstance().Log(*lpCharInfo, lpPktChatReq->m_cZone, serverID.GetChannel(), szMessage,
                    static_cast<PktChat::PktChatCmd>(lpPktChatReq->m_cCmd),
                    lpPktChatReq->m_usXPos, lpPktChatReq->m_usYPos, lpPktChatReq->m_usZPos, lpPktChatReq->m_usLang);
            }
        }
    }

    return true;
}

bool CChatGameServerDispatch::ProcessChatBan(PktChatBan* lpPktChatBan)
{
	unsigned long dwAdminCID	= lpPktChatBan->m_dwAdminCID;
	unsigned long dwTargetCID	= lpPktChatBan->m_dwTargetCID;
	unsigned long dwMinutes		= lpPktChatBan->m_dwMinutes;

	if (0 != dwMinutes)
	{
		if (false == ChatData::CCharInfoMap::GetInstance().AddCharChatBan(dwTargetCID, dwMinutes))
		{
			return false;
		}

		DBComponent::AdminDB::StartCharChatBan(CDBSingleObject::GetInstance(), dwAdminCID, dwTargetCID, dwMinutes);
	}
	else
	{
		if (false == ChatData::CCharInfoMap::GetInstance().DeleteCharChatBan(dwTargetCID))
		{
			return false;
		}

		DBComponent::AdminDB::EndCharChatBan(CDBSingleObject::GetInstance(), dwTargetCID);
	}

	GetDispatchTable().Process(CSendCharChatBanAllGameServer(dwTargetCID, dwMinutes));
	return true;
}

bool CChatGameServerDispatch::ProcessChatBanInfo(PktChatBanInfo* lpPktChatBanInfo)
{	
	unsigned long dwCID			= lpPktChatBanInfo->m_dwCID;
	unsigned long dwMinutes		= ChatData::CCharInfoMap::GetInstance().GetCharChatBan(dwCID);

	if(dwMinutes)
	{
		GetDispatchTable().Process(CSendCharChatBanAllGameServer(dwCID, dwMinutes));
	}
	else
	{		
		ChatData::CCharInfoMap::GetInstance().GetCharChatBanInfo(dwCID, dwMinutes);
		
		// ��񿡼� ���� �������� //
		if(dwMinutes)
		{
			if (false == ChatData::CCharInfoMap::GetInstance().AddCharChatBan(dwCID, dwMinutes))
			{
				return false;
			}			

			GetDispatchTable().Process(CSendCharChatBanAllGameServer(dwCID, dwMinutes));
		}
	}

	return true;
}

CFindAllFriended::CFindAllFriended(FriendedMap& friendedMap, CBanList& banList, const char* szMyName, bool bLogin)
:   m_friendedMap(friendedMap), m_banList(banList), m_szMyName(szMyName), m_bLogin(bLogin)
{

}

bool CFindAllFriended::operator() (CXRefFriends::XRefTable::value_type& friendCIDPair)
{    
    // first�� ��, second�� ���� ģ���� ���� ��.
    ChatData::CCharInfo* lpCharInfo = 
        ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(friendCIDPair.second);

    if (0 != lpCharInfo)
    {
        if (!m_banList.IsBan(friendCIDPair.second, lpCharInfo->GetName()))
        {
            // �ź� ��밡 �ƴ�.
            CFriendList::Rebind* lpRebind = lpCharInfo->GetFriendList().GetFriend(friendCIDPair.first);
            if (0 != lpRebind && lpRebind->IsFriend(m_szMyName))
            {
                // �̸� �˻� ����. Ȯ���� ���� ģ���� ������ ����.
                lpRebind->SetLoginStatus(m_bLogin);

                // �ʿ� �̳� ������ ���ųִ´�. ���߿� �ѹ��� �����ؼ� ������.
                // Key : ServerID / Value : CID 

                m_friendedMap.insert(FriendedMap::value_type(
                    lpCharInfo->GetServerID(), lpCharInfo->GetCID()));
            }
        }		
    }

    return true;
}

CFindAllBaned::CFindAllBaned(BanedMap& banedMap, const char* szMyName)
:   m_bandMap(banedMap), m_szMyName(szMyName)
{

}

bool CFindAllBaned::operator() (CXRefBans::XRefTable::value_type& banCIDPair)
{    
	// first�� ��, second�� ���� �źη� ���� ��.
	ChatData::CCharInfo* lpCharInfo = ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(banCIDPair.second);

	if (0 != lpCharInfo)
	{				
		CBanList::Rebind* lpRebind = lpCharInfo->GetBanList().GetBan(banCIDPair.first);
		
		if (0 != lpRebind)
		{		
			// �ʿ� �̳� ������ ���ųִ´�. ���߿� �ѹ��� �����ؼ� ������.
			// Key : ServerID / Value : CID 

			m_bandMap.insert(BanedMap::value_type(
				lpCharInfo->GetServerID(), lpCharInfo->GetCID()));
		}		
	}

	return true;
}

CUpdateFriendLoginStatus::CUpdateFriendLoginStatus(unsigned long dwOwnerCID, const char* szOwnerName)
:   m_dwOwnerCID(dwOwnerCID), m_szOwnerName(szOwnerName), m_chatDataMap(ChatData::CCharInfoMap::GetInstance())
{

}


bool CUpdateFriendLoginStatus::operator() (CFriendList::Rebind& rebinds)
{
    bool bLoginStatus = false;

    ChatData::CCharInfo* lpCharInfo = 
        m_chatDataMap.GetCharInfoByCID(rebinds.GetCID());

    if (0 != lpCharInfo)
    {
        if (!lpCharInfo->GetBanList().IsBan(m_dwOwnerCID, m_szOwnerName))
        {
            bLoginStatus = true;
        }
    }

    rebinds.SetLoginStatus(bLoginStatus);
    return true;
}


CSendFriendAck::CSendFriendAck(unsigned long dwCID, const char* szName, unsigned long dwGID, unsigned short wClass,
							   char cLevel, unsigned long dwServerID, CFindAllFriended::FriendedMap& targets, unsigned char cAckCmd, unsigned short usError)
                               
							   :    m_dwCID(dwCID), m_szName(szName), m_dwGID(dwGID), m_wClass(wClass), m_cLevel(cLevel), m_dwServerID(dwServerID),
                                    m_targets(targets), m_cAckCmd(cAckCmd), m_usError(usError) 
{

}

bool CSendFriendAck::operator () (unsigned long dwServerID, CPacketDispatch& dispatch)
{
    CRylServerDispatch& rylServerDispatch = 
        static_cast<CRylServerDispatch&>(dispatch);

    CSendStream& SendStream = rylServerDispatch.GetSendStream();

    std::pair<CFindAllFriended::FriendedMap::iterator, 
        CFindAllFriended::FriendedMap::iterator> result = m_targets.equal_range(dwServerID);

    // ���� ģ���� ���� ���� �ƹ��� ���Ƶ�, ������ ���������� ������ ���� ���� ����.
    // ���� ������ �ִ� 3000�� �����̶�, 16kb�ȿ� �� �� �ִ�.
    // �׷��Ƿ� ��Ŷ�� �ɰ� ���� �ʿ�� ����.

    unsigned long  dwCIDNum = static_cast<unsigned long>(std::distance(result.first, result.second));
    unsigned short usSourceLength = static_cast<unsigned short>(sizeof(PktFriendAck) + sizeof(unsigned long) + 
        sizeof(unsigned long) * dwCIDNum);

    char szPacketData[PktMaxLen];

    if (usSourceLength < PktMaxLen)
    {
        PktFriendAck* lpPktFriendAck = reinterpret_cast<PktFriendAck*>(szPacketData);

        lpPktFriendAck->m_dwCID			= m_dwCID;
        lpPktFriendAck->m_cCmd			= m_cAckCmd;
		lpPktFriendAck->m_dwGID			= m_dwGID;
		lpPktFriendAck->m_wClass		= m_wClass;
		lpPktFriendAck->m_cLevel		= m_cLevel;
		lpPktFriendAck->m_dwServerID	= m_dwServerID;

        if (0 == m_szName)
        {
            memset(lpPktFriendAck->m_szName, 0, PktFriendAck::MAX_NAME);
        }
        else
        {
            strncpy(lpPktFriendAck->m_szName, m_szName, PktFriendAck::MAX_NAME);
            lpPktFriendAck->m_szName[PktFriendAck::MAX_NAME - 1] = 0;
        }

        unsigned long* lpdwDataPos = reinterpret_cast<unsigned long*>(lpPktFriendAck + 1);
        *lpdwDataPos = dwCIDNum;
        ++lpdwDataPos;

        for(; result.first != result.second; ++result.first, ++lpdwDataPos)
        {
            *lpdwDataPos = result.first->second;
        }

        return SendStream.WrapCompress(szPacketData, usSourceLength, CmdFriendAck, 0, m_usError);
    }

    return true;
}


CSendChatTargetGameServer::CSendChatTargetGameServer(CServerChatPacket::TargetList& targetList,
                                                     BattleInclination::CharData& senderData,
                                                     const char* szSenderName,
                                                     const char* szMessage, PktChat::PktChatCmd cChatCmd, unsigned short usLang)
                                                     :  m_TargetList(targetList), m_szMessage(szMessage), 
                                                        m_cChatCmd(cChatCmd), m_senderData(senderData),
                                                        m_szSenderName(szSenderName), m_usLang(usLang)
{

}

bool CSendChatTargetGameServer::operator () (unsigned long dwServerID, CPacketDispatch& dispatch)
{
    CRylServerDispatch& rylServerDispatch = static_cast<CRylServerDispatch&>(dispatch);

    std::pair<CServerChatPacket::TargetList::iterator, CServerChatPacket::TargetList::iterator>             
        result = m_TargetList.equal_range(dwServerID);

    if (result.first != result.second)
    {
        CServerChatPacket serverChatPacket(m_szMessage, 
            m_cChatCmd, m_usLang, m_senderData, m_szSenderName,
            result.first, result.second);

        if (serverChatPacket.IsValid())
        {
            rylServerDispatch.GetSendStream().PutBuffer(
                serverChatPacket.GetCompressedPacket(), 
                serverChatPacket.GetCompressedSize(), CmdCharChat);
        }
    }

    return true;        
}

CSendChatAllGameServer::CSendChatAllGameServer(CServerChatPacket& serverChatPacket)
:   m_ServerChatPacket(serverChatPacket) 
{

}

bool CSendChatAllGameServer::operator () (unsigned long dwServerID, CPacketDispatch& dispatch)
{
    CRylServerDispatch& rylServerDispatch = static_cast<CRylServerDispatch&>(dispatch);

    return m_ServerChatPacket.IsValid() ? rylServerDispatch.GetSendStream().PutBuffer(
        m_ServerChatPacket.GetCompressedPacket(), m_ServerChatPacket.GetCompressedSize(), CmdCharChat) : false;
}
