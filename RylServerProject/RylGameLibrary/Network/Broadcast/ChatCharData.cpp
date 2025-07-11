
#include "stdafx.h"

#include "ChatCharData.h"
#include "BroadcastCharacterData.h"

#include <DB/DBComponent.h>
#include <DB/AdminDBComponent.h>

#include <Community/FriendList.h>
#include <Community/BanList.h>

#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/MultiDispatchStorage.h>

using Broadcast2nd::COPY_AND_ADVANCE_SRC;
using Broadcast2nd::COPY_AND_ADVANCE_DST;

ChatData::CCharInfo::CCharInfo(unsigned long dwUID, unsigned long dwCID, 
                               unsigned long dwServerID,  CXRefFriends* lpXRefFriends, CXRefBans* lpXRefBans)
:   m_dwUID(dwUID), m_dwServerID(dwServerID), m_bSendLoginData(false),
    m_cAdminLevel(0), m_FriendList(dwCID, lpXRefFriends), m_BanList(dwCID, lpXRefBans)
{
    memset(m_szName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    memset(m_szAccountName, 0, sizeof(char) * CHAR_INFOST::MAX_ACCOUNT_LEN);
    memset(&m_CharData, 0, sizeof(BattleInclination::CharData));
    memset(&m_RejectOption, 0, sizeof(RejectOption));

    m_CharData.m_dwCID = dwCID;
}

bool ChatData::CCharInfo::UpdateData(const char* szData, int& nDataLen)
{
    // 업데이트되어야 할 데이터 종류
    const char* szDataPos = szData;

    if (nDataLen < sizeof(unsigned short))
    {
        return false;
    }

    unsigned short usUpdateFlag = 0;
    COPY_AND_ADVANCE_SRC(&usUpdateFlag, szDataPos, sizeof(unsigned short));

    int nEstimateSize = EstimateBufferSize(usUpdateFlag) + sizeof(unsigned short);

    if (nDataLen < nEstimateSize)
    {
        return false;
    }

    if (usUpdateFlag & DELTA_NAME)
    {
        COPY_AND_ADVANCE_SRC(m_szName, 
            szDataPos, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    }

    if (usUpdateFlag & DELTA_PID)
    { 
        COPY_AND_ADVANCE_SRC(&m_CharData.m_dwPID, 
            szDataPos, sizeof(unsigned long));		
    }

    if (usUpdateFlag & DELTA_GID)            
    { 
        COPY_AND_ADVANCE_SRC(&m_CharData.m_dwGID, 
            szDataPos, sizeof(unsigned long));

		COPY_AND_ADVANCE_SRC(&m_CharData.m_cTacticsFlag, 
			szDataPos, sizeof(unsigned char));
    }
    
	if (usUpdateFlag & DELTA_WARFLAG)            
	{
		COPY_AND_ADVANCE_SRC(&m_CharData.m_cRealmWarFlag, 
			szDataPos, sizeof(unsigned char));

		COPY_AND_ADVANCE_SRC(&m_CharData.m_cGuildWarFlag, 
			szDataPos, sizeof(unsigned char));
	}

    if (usUpdateFlag & DELTA_CASTLEID)       
    { 
        COPY_AND_ADVANCE_SRC(&m_CharData.m_dwCastleID, 
            szDataPos, sizeof(unsigned long));
    }
    
    if (usUpdateFlag & DELTA_SIEGESTATE)     
    { 
        m_CharData.m_cSiegeState = *szDataPos;
        ++szDataPos;
    }
    
    if (usUpdateFlag & DELTA_NATION)         
    {
        m_CharData.m_cNation = *szDataPos;
        ++szDataPos;
    }
    
    if (usUpdateFlag & DELTA_ETCFLAGS)
    {
        m_CharData.m_cFlags = *szDataPos;
        ++szDataPos;
    }
    
    if (usUpdateFlag & DELTA_REJECTOPTION)
    { 
        COPY_AND_ADVANCE_SRC(&m_RejectOption, 
            szDataPos, sizeof(RejectOption));
    }

    if (usUpdateFlag & DELTA_ADMINLEVEL)
    {
        m_cAdminLevel = *szDataPos;
        ++szDataPos;
    }

    if (usUpdateFlag & DELTA_ACCOUNTNAME)
    {
        COPY_AND_ADVANCE_SRC(m_szAccountName, 
            szDataPos, sizeof(char) * CHAR_INFOST::MAX_ACCOUNT_LEN);
    }

	if (usUpdateFlag & DELTA_CREATURETYPE)
	{
		m_CharData.m_cCreatureType = *szDataPos;
		++szDataPos;
	}

	if (usUpdateFlag & DELTA_OBJECTTYPE)
	{
		COPY_AND_ADVANCE_SRC(&m_CharData.m_wObjectType, 
			szDataPos, sizeof(unsigned short));
	}

	if (usUpdateFlag & DELTA_KID)
	{
		COPY_AND_ADVANCE_SRC(&m_CharData.m_wObjectType, 
			szDataPos, sizeof(unsigned short));
	}

    nDataLen = static_cast<int>(szDataPos - szData);
    return true;
}


int ChatData::CCharInfo::EstimateBufferSize(unsigned short usUpdateFlag)
{
    int nBufferSize = 0;

    if (usUpdateFlag & DELTA_NAME)          { nBufferSize += sizeof(char) * CHAR_INFOST::MAX_NAME_LEN;		}    
    if (usUpdateFlag & DELTA_PID)           { nBufferSize += sizeof(unsigned long);							}
    if (usUpdateFlag & DELTA_GID)           { nBufferSize += sizeof(unsigned long);							}
	if (usUpdateFlag & DELTA_GID)           { nBufferSize += sizeof(unsigned char);							}
	if (usUpdateFlag & DELTA_WARFLAG)       { nBufferSize += sizeof(unsigned char)*2;						}    
	if (usUpdateFlag & DELTA_CASTLEID)      { nBufferSize += sizeof(unsigned long);							}
    if (usUpdateFlag & DELTA_SIEGESTATE)    { nBufferSize += sizeof(unsigned char);							}
    if (usUpdateFlag & DELTA_NATION)        { nBufferSize += sizeof(unsigned char);							}
    if (usUpdateFlag & DELTA_ETCFLAGS)      { nBufferSize += sizeof(unsigned char);							}
    if (usUpdateFlag & DELTA_REJECTOPTION)  { nBufferSize += sizeof(RejectOption);							}
    if (usUpdateFlag & DELTA_ADMINLEVEL)    { nBufferSize += sizeof(unsigned char);							}
    if (usUpdateFlag & DELTA_ACCOUNTNAME)   { nBufferSize += sizeof(char) * CHAR_INFOST::MAX_ACCOUNT_LEN;	}
	if (usUpdateFlag & DELTA_CREATURETYPE)  { nBufferSize += sizeof(unsigned char);							}
	if (usUpdateFlag & DELTA_OBJECTTYPE)    { nBufferSize += sizeof(unsigned short);						}
	if (usUpdateFlag & DELTA_KID)			{ nBufferSize += sizeof(unsigned short);						}

    return nBufferSize;
}


ChatData::CCharInfoMap& ChatData::CCharInfoMap::GetInstance()
{
    static CCharInfoMap charInfoMap;
    return charInfoMap;
}


ChatData::CCharInfoMap::~CCharInfoMap()
{
    Destroy();
}

void ChatData::CCharInfoMap::Destroy()
{
    CharInfoMap::iterator pos = m_cidMap.begin();
    CharInfoMap::iterator end = m_cidMap.end();

    for(;pos != end; ++pos)
    {
        delete pos->second;
    }

    m_cidMap.clear();    
    m_pidMap.clear();
    m_gidMap.clear();
	m_raceMap.clear();
    m_nameMap.clear();
}

inline unsigned long nameHash(const unsigned char *str, int length)
{
    unsigned long hash = 0;
    int c;

    while (c = *str && 0 < length)
    {
        ++str;
        --length;

        hash = c + (hash << 6) + (hash << 16) - hash;         
    }
    return hash;
}


bool ChatData::CCharInfoMap::AddCharInfo(CCharInfo* lpCharInfo)
{
    if (0 != lpCharInfo)
    {
        // 이미 있는 캐릭터를 제거하고, 신규 캐릭터를 넣는다.

        CharInfoMap::iterator insertPos = m_cidMap.lower_bound(lpCharInfo->GetCID());

        if (insertPos != m_cidMap.end() && insertPos->first == lpCharInfo->GetCID())
        {
            // 기존 데이터를 찾았다. 기존 데이터는 삭제한다.

            CCharInfo* lpDelCharInfo = insertPos->second;

            CCharInfo* lpRemovedCharInfo = RemoveCharInfo(lpDelCharInfo->GetUID(), 
                lpDelCharInfo->GetCID(), lpDelCharInfo->GetServerID());

            if (0 != lpRemovedCharInfo)
            {
                delete lpRemovedCharInfo;
            }            
        }

        // 신규 데이터이다.
        CharInfoMap::iterator result = m_cidMap.insert(insertPos, 
            CharInfoMap::value_type(lpCharInfo->GetCID(), lpCharInfo));
        
        if (result != m_cidMap.end())
        {
            // GID
            if (0 != lpCharInfo->GetGID())
            {
                m_gidMap.insert(CharInfoMap::value_type(
                    lpCharInfo->GetGID(), lpCharInfo));
            }
            
            // PID
            if (0 != lpCharInfo->GetPID())
            {
                m_pidMap.insert(CharInfoMap::value_type(
                    lpCharInfo->GetPID(), lpCharInfo));
            }

			// Race
            m_raceMap.insert(CharInfoMap::value_type(
                lpCharInfo->GetRace(), lpCharInfo));

            // 캐릭터 이름
            unsigned long dwNameHash = nameHash(
                reinterpret_cast<const unsigned char*>(lpCharInfo->GetName()), 
                CHAR_INFOST::MAX_NAME_LEN);

            m_nameMap.insert(CharInfoMultiMap::value_type(dwNameHash, lpCharInfo));

            // 계정명
            unsigned long dwAccountHash = nameHash(
                reinterpret_cast<const unsigned char*>(lpCharInfo->GetAccountName()),
                CHAR_INFOST::MAX_ACCOUNT_LEN);

            m_accountMap.insert(CharInfoMultiMap::value_type(dwAccountHash, lpCharInfo));

			// 채팅 금지
			unsigned long dwMinutes = 0;
			if (DBComponent::AdminDB::GetCharChatBan(
                CDBSingleObject::GetInstance(), lpCharInfo->GetCID(), dwMinutes) && dwMinutes)
			{
				AddCharChatBan(lpCharInfo->GetCID(), dwMinutes);
			}

            DETLOG3(g_Log, "UID:%10u/CID:0x%08x/ServerID:0x%08x/캐릭터 정보 추가 성공", 
                lpCharInfo->GetUID(), lpCharInfo->GetCID(), lpCharInfo->GetServerID());

            return true;
        }
        else
        {
            ERRLOG1(g_Log, "CID:0x%08x/데이터를 삽입할 수 없습니다.", lpCharInfo->GetCID());
        }
    }

    return false;
}


ChatData::CCharInfo* ChatData::CCharInfoMap::GetCharInfoByUID(unsigned long dwUID)
{
    CharInfoMap::iterator pos = m_cidMap.begin();
    CharInfoMap::iterator end = m_cidMap.end();

    for(; pos != end; ++pos)
    {
        CCharInfo* lpCharInfo = pos->second;

        if (lpCharInfo->GetUID() == dwUID)
        {
            return lpCharInfo;
        }
    }

    return 0;
}


ChatData::CCharInfo* ChatData::CCharInfoMap::GetCharInfoByCID(unsigned long dwCID)
{
    CharInfoMap::iterator pos = m_cidMap.find(dwCID);
    CharInfoMap::iterator end = m_cidMap.end();

    return pos != end ? pos->second : 0;
}


ChatData::CCharInfo* ChatData::CCharInfoMap::GetCharInfoByName(const char* szName)
{
    std::pair<CharInfoMultiMap::iterator, CharInfoMultiMap::iterator> 
        result = m_nameMap.equal_range(nameHash(
        reinterpret_cast<const unsigned char*>(szName), CHAR_INFOST::MAX_NAME_LEN));

    for(; result.first != result.second; ++result.first)
    {
        CCharInfo* lpCharInfo = result.first->second;

        if (0 == strncmp(lpCharInfo->GetName(), szName, CHAR_INFOST::MAX_NAME_LEN))
        {            
            return lpCharInfo;
        }
    }
    
    return 0;
}

ChatData::CCharInfo* ChatData::CCharInfoMap::GetCharInfoByAccountName(const char* szAccountName)
{
    std::pair<CharInfoMultiMap::iterator, CharInfoMultiMap::iterator> 
        result = m_accountMap.equal_range(nameHash(
        reinterpret_cast<const unsigned char*>(szAccountName), CHAR_INFOST::MAX_ACCOUNT_LEN));

    for(; result.first != result.second; ++result.first)
    {
        CCharInfo* lpCharInfo = result.first->second;

        if (0 == strncmp(lpCharInfo->GetAccountName(), szAccountName, CHAR_INFOST::MAX_ACCOUNT_LEN))
        {            
            return lpCharInfo;
        }
    }

    return 0;
}

namespace ChatData
{
    inline bool EraseByKey(CCharInfoMap::CharInfoMultiMap& map, 
        unsigned long dwUID, unsigned long dwCID, unsigned long dwServerID, unsigned long dwKey)
    {
        std::pair<CCharInfoMap::CharInfoMultiMap::iterator, 
            CCharInfoMap::CharInfoMultiMap::iterator> result = map.equal_range(dwKey);

        for(; result.first != result.second; ++result.first)
        {
            CCharInfo* lpDelCharInfo = result.first->second;

			if(!lpDelCharInfo)
			{
				return false;
			}

            if (lpDelCharInfo->GetUID() == dwUID &&
                lpDelCharInfo->GetCID() == dwCID &&
                lpDelCharInfo->GetServerID() == dwServerID)
            {
                map.erase(result.first);
                return true;
            }
        }

        return false;
    }
};

ChatData::CCharInfo* ChatData::CCharInfoMap::RemoveCharInfo(unsigned long dwUID, 
                                                            unsigned long dwCID, unsigned long dwServerID)
{
    CharInfoMap::iterator pos = m_cidMap.find(dwCID);

    if (pos != m_cidMap.end())
    {
        CCharInfo* lpCharInfo = pos->second;
        CCharInfo* lpDelCharInfo = 0;

        if (dwUID == lpCharInfo->GetUID() && 
            dwCID == lpCharInfo->GetCID() && 
            dwServerID == lpCharInfo->GetServerID())
        {
            // UID맵에서 삭제
            if (0 != lpCharInfo->GetGID())
            {
                EraseByKey(m_gidMap, dwUID, dwCID, 
                    dwServerID, lpCharInfo->GetGID());
            }

            // PID맵에서 삭제
            if (0 != lpCharInfo->GetPID())
            {
                EraseByKey(m_pidMap, dwUID, dwCID,
                    dwServerID, lpCharInfo->GetPID());                
            }

			// Race맵에서 삭제
            EraseByKey(m_raceMap, dwUID, dwCID,
                dwServerID, lpCharInfo->GetRace());                


            // 캐릭터 이름 맵에서 삭제
            EraseByKey(m_nameMap, dwUID, dwCID, dwServerID, 
                nameHash(reinterpret_cast<const unsigned char*>(lpCharInfo->GetName()), 
                CHAR_INFOST::MAX_NAME_LEN));
            
            // 계정 맵에서 삭제
            EraseByKey(m_accountMap, dwUID, dwCID, dwServerID, 
                nameHash(reinterpret_cast<const unsigned char*>(lpCharInfo->GetAccountName()), 
                CHAR_INFOST::MAX_ACCOUNT_LEN));
            
            // 채팅 금지 제거
            const unsigned long dwRemainTime = GetCharChatBan(dwCID);
            if (0 != dwRemainTime)
            {
                DBComponent::AdminDB::UpdateCharChatBanTime(
                    CDBSingleObject::GetInstance(), dwCID, dwRemainTime);
                
                DeleteCharChatBan(dwCID);
            }

            m_cidMap.erase(pos);

            DETLOG3(g_Log, "UID:%10u/CID:0x%08x/ServerID:0x%08x/캐릭터 정보 제거 성공", 
                dwUID, dwCID, dwServerID);

            return lpCharInfo;
        }
        else
        {
            ERRLOG6(g_Log, "UID:%10u/CID:0x%08x/ServerID:0x%08x/"
                "CurrentUID:%10u/CurrentCID:0x%08x/CurrentServerID:0x%08x/"
                "제거 실패 : 현재 데이터와 새로 들어온 데이터의 값이 다릅니다.", dwUID, dwCID, dwServerID, 
                lpCharInfo->GetUID(), lpCharInfo->GetCID(), lpCharInfo->GetServerID());
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u/CID:0x%08x/ServerID:0x%08x/"            
            "업데이트 실패 : 해당하는 데이터를 찾을 수 없습니다", dwUID, dwCID, dwServerID);
    }

    return 0;
}

void ChatData::CCharInfoMap::RemoveCharInfo(unsigned long dwServerID)
{
    CharInfoMap::iterator pos = m_cidMap.begin();
    CharInfoMap::iterator end = m_cidMap.end();

    for(; pos != end;)
    {
        CCharInfo* lpCharInfo = pos->second;

        if(lpCharInfo->GetServerID() == dwServerID)
        {
            unsigned long dwUID = lpCharInfo->GetUID();
            unsigned long dwCID = lpCharInfo->GetCID();

            if (0 != lpCharInfo->GetGID())
            {
                EraseByKey(m_gidMap, dwUID, dwCID, 
                    dwServerID, lpCharInfo->GetGID());
            }

            if (0 != lpCharInfo->GetPID())
            {
                EraseByKey(m_pidMap, dwUID, dwCID,
                    dwServerID, lpCharInfo->GetPID());                
            }

			// Race맵에서 삭제
            EraseByKey(m_raceMap, dwUID, dwCID,
                dwServerID, lpCharInfo->GetRace());                

            unsigned long dwNameHash = nameHash(
                reinterpret_cast<const unsigned char*>(lpCharInfo->GetName()), CHAR_INFOST::MAX_NAME_LEN);

            EraseByKey(m_nameMap, dwUID, dwCID, dwServerID, dwNameHash);

            // 채팅 금지 제거
            const unsigned long dwRemainTime = GetCharChatBan(dwCID);
            if (0 != dwRemainTime)
            {
                DBComponent::AdminDB::UpdateCharChatBanTime(
                    CDBSingleObject::GetInstance(), dwCID, dwRemainTime);
                
                DeleteCharChatBan(dwCID);
            }

            pos = m_cidMap.erase(pos);
            delete lpCharInfo;            
        }
        else
        {
            ++pos;
        }
    }
}


bool ChatData::CCharInfoMap::AddCharChatBan(unsigned long dwTargetCID, unsigned long dwMinutes)
{
	CharBanMap::iterator pos = m_banMap.find(dwTargetCID);

	if (pos != m_banMap.end())
	{
        // 이미 존재하면 종료시킨 후 다시 추가시킨다. (로긴시에는 관계없다.)
		m_banMap.erase(pos);			
        //DBComponent::AdminDB::EndCharChatBan(CDBSingleObject::GetInstance(), dwTargetCID);		
	}

	m_banMap.insert(std::make_pair(dwTargetCID, dwMinutes));	
	return true;
}

bool ChatData::CCharInfoMap::GetCharChatBanInfo(unsigned long dwTargetCID, unsigned long& dwMinutes)
{
    DBComponent::AdminDB::GetCharChatBan(CDBSingleObject::GetInstance(), dwTargetCID, dwMinutes);			
	
	return true;
}

bool ChatData::CCharInfoMap::DeleteCharChatBan(unsigned long dwCID)
{
	CharBanMap::iterator pos = m_banMap.find(dwCID);

	if (pos != m_banMap.end())
	{
		if(pos->second)
		{
			DBComponent::AdminDB::UpdateCharChatBanTime(CDBSingleObject::GetInstance(), pos->first, pos->second);
		}

		m_banMap.erase(pos);
		return true;
	}

	return false;
}

unsigned long ChatData::CCharInfoMap::GetCharChatBan(unsigned long dwCID)
{
	CharBanMap::iterator pos = m_banMap.find(dwCID);

	if (pos != m_banMap.end())
	{
		return pos->second;
	}

	return 0;
}

void ChatData::CCharInfoMap::ProcessCharChatBan(CMultiDispatch& dispatch)
{
	for (CharBanMap::iterator pos = m_banMap.begin(); pos != m_banMap.end(); )
	{
        if (0 < pos->second)
        {
            --(pos->second);
        }

		if (0 == pos->second)
		{
            DWORD dwCID = pos->first;
            DWORD dwMinute = pos->second;

			dispatch.Process(CSendCharChatBanAllGameServer(dwCID, dwMinute));

            pos = m_banMap.erase(pos);
            DBComponent::AdminDB::EndCharChatBan(CDBSingleObject::GetInstance(), dwCID);
		}
		else
		{
			// 10분 간격으로 DB에 남은 시간을 업데이트 해준다.
			if (0 == pos->second % 10)
			{
				DBComponent::AdminDB::UpdateCharChatBanTime(
                    CDBSingleObject::GetInstance(), pos->first, pos->second);
			}

            ++pos;
		}
	}
}


bool ChatData::CCharInfoMap::Update(unsigned long dwUID, unsigned long dwCID, 
                                    unsigned long dwServerID, const char* lpData, int& nDataLen)
{
    CharInfoMap::iterator pos = m_cidMap.find(dwCID);

    if (pos != m_cidMap.end())
    {
        CCharInfo* lpCharInfo = pos->second;

        if (dwUID == lpCharInfo->GetUID() && 
            dwCID == lpCharInfo->GetCID() && 
            dwServerID == lpCharInfo->GetServerID())
        {
            unsigned long dwGID = lpCharInfo->GetGID();
            unsigned long dwPID = lpCharInfo->GetPID();
            
            char szName[CHAR_INFOST::MAX_NAME_LEN];
            strncpy(szName, lpCharInfo->GetName(), CHAR_INFOST::MAX_NAME_LEN);
            szName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

            if (lpCharInfo->UpdateData(lpData, nDataLen))
            {
                // 업데이트 성공.

                // GID변경시 맵 다시 세팅
                if (dwGID != lpCharInfo->GetGID())
                {
                    if (0 != dwGID) { EraseByKey(m_gidMap, dwUID, dwCID, dwServerID, dwGID); }
                    if (0 != lpCharInfo->GetGID())
                    {
                        m_gidMap.insert(CharInfoMultiMap::value_type(
                            lpCharInfo->GetGID(), lpCharInfo));
                    }
                }

                // PID변경시 맵 다시 세팅
                if (dwPID != lpCharInfo->GetPID())
                {
                    if (0 != dwPID) { EraseByKey(m_pidMap, dwUID, dwCID, dwServerID, dwPID); }
                    if (0 != lpCharInfo->GetPID())
                    {
                        m_pidMap.insert(CharInfoMultiMap::value_type(
                            lpCharInfo->GetPID(), lpCharInfo));
                    }
                }

                // 이름 변경시 이름 맵 다시 세팅
                if (0 != strncmp(szName, lpCharInfo->GetName(), CHAR_INFOST::MAX_NAME_LEN))
                {
                    EraseByKey(m_nameMap, dwUID, dwCID, dwServerID, nameHash(
                        reinterpret_cast<const unsigned char*>(szName), CHAR_INFOST::MAX_NAME_LEN));

                    m_nameMap.insert(CharInfoMultiMap::value_type(
                        nameHash(reinterpret_cast<const unsigned char*>(
                        lpCharInfo->GetName()), CHAR_INFOST::MAX_NAME_LEN), lpCharInfo));
                }

                return true;
            }
            else
            {
                ERRLOG3(g_Log, "UID:%10u/CID:0x%08x/ServerID:0x%08x/채팅 데이터 업데이트에 실패했습니다",
                    dwUID, dwCID, dwServerID);
            }
        }
        else
        {
            ERRLOG6(g_Log, "UID:%10u/CID:0x%08x/ServerID:0x%08x/"
                "CurrentUID:%10u/CurrentCID:0x%08x/CurrentServerID:0x%08x/"
                "업데이트 실패 : 현재 데이터와 새로 들어온 데이터의 값이 다릅니다.", dwUID, dwCID, dwServerID, 
                lpCharInfo->GetUID(), lpCharInfo->GetCID(), lpCharInfo->GetServerID());
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u/CID:0x%08x/ServerID:0x%08x/"            
            "업데이트 실패 : 해당하는 데이터를 찾을 수 없습니다", dwUID, dwCID, dwServerID);
    }

    return false;
}

bool CSendCharChatBanAllGameServer::operator () (unsigned long dwServerID, CPacketDispatch& dispatch)
{
	CRylServerDispatch& rylServerDispatch = static_cast<CRylServerDispatch&>(dispatch);
	
	// 게임서버에는 채금을 건 운영자 정보가 필요없다.
	return GameClientSendPacket::SendCharChatBan(rylServerDispatch.GetSendStream(), 0, m_dwCID, m_dwMinutes);
}
