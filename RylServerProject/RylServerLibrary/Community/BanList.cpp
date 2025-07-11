#include "stdafx.h"
#include "BanList.h"

#include <algorithm>

inline bool operator < (unsigned long dwCID, const BanInfo& rhs) { return dwCID < rhs.m_dwCID; }
inline bool operator < (const BanInfo& lhs, unsigned long dwCID) { return lhs.m_dwCID < dwCID; }
inline bool operator < (const BanInfo& lhs, const BanInfo& rhs)  { return lhs.m_dwCID < rhs.m_dwCID; }


CXRefBans& CXRefBans::GetInstance()
{
	static CXRefBans xRefBans;
	return xRefBans;
}

CBanList::CBanList(unsigned long dwOwnerCID, CXRefBans* lpXRefTable)
:   m_dwOwnerCID(dwOwnerCID), m_lpXRefTable(lpXRefTable)
{
}

CBanList::~CBanList()
{
    Clear();
}

void CBanList::Clear()
{
	if(NULL != m_lpXRefTable)
	{
		for(BanList::iterator itr = m_banList.begin();
			itr != m_banList.end(); ++itr)
		{
			m_lpXRefTable->Remove(itr->m_XRefItr);
		}
	}
    m_banList.clear();
}

bool CBanList::Add(unsigned long dwBanCID, const char* szCharacterName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID)
{
    if(m_banList.size() < MAX_BAN_NUM)
    {
        BanList::iterator lbound = std::lower_bound(m_banList.begin(), m_banList.end(), dwBanCID);

		Rebind  rebind;

        rebind.m_banInfo.m_dwCID		= dwBanCID;
		rebind.m_banInfo.m_dwGID		= dwGID;
		rebind.m_banInfo.m_wClass		= wClass;
		rebind.m_banInfo.m_cLevel		= cLevel;
		rebind.m_banInfo.m_dwServerID	= dwServerID;		
        strncpy(rebind.m_banInfo.m_szName, szCharacterName, BanInfo::MAX_NAME);

		if(NULL != m_lpXRefTable)
		{
			rebind.m_XRefItr = m_lpXRefTable->Add(m_dwOwnerCID, dwBanCID);
		}

        if(lbound == m_banList.end() || dwBanCID != lbound->m_banInfo.m_dwCID)
        {
            // CID가 다른 경우는 삽입한다.
            m_banList.insert(lbound, rebind);
            
        }
        else
        {
            // CID가 같은 경우는 갱신한다.
            *lbound = rebind;
        }

        return true;
    }

    return false;
}


bool CBanList::Remove(unsigned long dwBanCID)
{
    BanList::iterator finditr = std::lower_bound(m_banList.begin(), m_banList.end(), dwBanCID);

    if(finditr != m_banList.end() && dwBanCID == finditr->m_banInfo.m_dwCID)
    {
		if(NULL != m_lpXRefTable)
		{
			m_lpXRefTable->Remove(finditr->m_XRefItr);
		}

        m_banList.erase(finditr);
        return true;
    }

    return false;
}


void CBanList::GetCIDList(unsigned long* dwCID_In)
{
	BanList::iterator first = m_banList.begin();
	BanList::iterator last  = m_banList.end();

	for(unsigned char cIndex = 0; first != last; ++first)
	{
		dwCID_In[cIndex] = (*first).GetCID();		
	}	
}

CBanList::Rebind* CBanList::GetBan(unsigned long dwBanCID)
{    
	BanList::iterator finditr = std::lower_bound(m_banList.begin(), m_banList.end(), dwBanCID);

	if(finditr != m_banList.end() && dwBanCID == finditr->m_banInfo.m_dwCID)
	{        
		return &(*finditr);
	};

	return NULL;
}


CBanList::Rebind* CBanList::GetBan(const char* szFriendName)
{
	BanList::iterator first = m_banList.begin();
	BanList::iterator last  = m_banList.end();

	for(;first != last; ++first)
	{
		if(0 == strncmp(first->GetCharacterName(), szFriendName, FriendInfo::MAX_NAME))
		{
			return &(*first);
		}
	}

	return NULL;
}

bool CBanList::IsBan(unsigned long dwBanCID, const char* szCharacterName)
{
    BanList::iterator finditr = std::lower_bound(m_banList.begin(), m_banList.end(), dwBanCID);

    if(finditr != m_banList.end() && dwBanCID == finditr->GetCID() &&
        0 == strncmp(szCharacterName, finditr->GetCharacterName(), BanInfo::MAX_NAME))
    {
        return true;
    }

    return false;
}


// 이름으로 거부자의 CID를 리턴한다. 0이면 BanList에 그런 이름이 존재하지 않는다.
unsigned long CBanList::GetBanCID(const char* szCharacterName)
{
    BanList::iterator first = m_banList.begin();
    BanList::iterator last  = m_banList.end();

    for(;first != last; ++first)
    {
        if(0 == strncmp(first->GetCharacterName(), szCharacterName, BanInfo::MAX_NAME))
        {
            return first->GetCID();
        }
    }

    return 0;
}

const char* CBanList::GetBanName(unsigned long dwCID)
{
    BanList::iterator finditr = std::lower_bound(m_banList.begin(), m_banList.end(), dwCID);

    if(finditr != m_banList.end() && dwCID == finditr->GetCID())
    {
        return finditr->GetCharacterName();
    }

    return NULL;
}

bool CBanList::SerializeOut(char* szBuffer_Out, unsigned long& dwBufferSize_InOut) const
{
    char* szBufferPos       = szBuffer_Out;
    char* szBufferPastEnd   = szBuffer_Out + dwBufferSize_InOut;

    BanList::const_iterator first  = m_banList.begin();
    BanList::const_iterator last   = m_banList.end();

    for(;szBufferPos + sizeof(BanInfo) <= szBufferPastEnd && first != last;
         szBufferPos += sizeof(BanInfo), ++first)
    {
        memcpy(szBufferPos, &first->m_banInfo, sizeof(BanInfo));
    }

    dwBufferSize_InOut = static_cast<unsigned long>(szBufferPos - szBuffer_Out);
    return true;
}


bool CBanList::SerializeIn(const char* szBuffer_In, unsigned long dwBufferSize_In)
{
    const char* szBufferPos       = szBuffer_In;
    const char* szBufferPastEnd   = szBuffer_In + dwBufferSize_In;

    Rebind rebind;

    for(;szBufferPos + sizeof(BanInfo) <= szBufferPastEnd;
         szBufferPos += sizeof(BanInfo))
    {
        unsigned long dwBanCID = reinterpret_cast<const BanInfo*>(szBufferPos)->m_dwCID;

        BanList::iterator lbound = std::lower_bound(m_banList.begin(), m_banList.end(), dwBanCID);
        if(lbound == m_banList.end() || dwBanCID != lbound->m_banInfo.m_dwCID)
        {
			if(NULL != m_lpXRefTable)
			{
				rebind.m_XRefItr = m_lpXRefTable->Add(m_dwOwnerCID, dwBanCID);
			}

            memcpy(&rebind.m_banInfo, szBufferPos, sizeof(BanInfo));
            m_banList.insert(lbound, rebind);
        }
    }

    return true;
}

bool CBanList::SerializeOut(char* szBuffer_Out, char* szInfoBuffer_Out, unsigned long& dwBufferSize_InOut, unsigned long& dwInfoBufferSize_InOut) const
{
	char* szBufferPos			= szBuffer_Out;
	char* szBufferPastEnd		= szBuffer_Out + dwBufferSize_InOut;
	char* szInfoBufferPos       = szInfoBuffer_Out;
	char* szInfoBufferPastEnd   = szInfoBuffer_Out + dwInfoBufferSize_InOut;

	BanList::const_iterator first  = m_banList.begin();
	BanList::const_iterator last   = m_banList.end();

	for(;szBufferPos + sizeof(DBBan) <= szBufferPastEnd && first != last;
		szBufferPos += sizeof(DBBan), ++first)
	{
		DBBan sDBBan;
		memset(&sDBBan, 0, sizeof(DBBan));
		sDBBan.m_dwCID	= first->m_banInfo.m_dwCID;
		memcpy(sDBBan.m_szName, first->m_banInfo.m_szName, DBBan::MAX_NAME);
		memcpy(szBufferPos, &sDBBan, sizeof(DBFriend));
	}

	dwBufferSize_InOut = static_cast<unsigned long>(szBufferPos - szBuffer_Out);

	first  = m_banList.begin();
	last   = m_banList.end();

	for(;szInfoBufferPos + sizeof(DBBanInfo) <= szInfoBufferPastEnd && first != last;
		szInfoBufferPos += sizeof(DBBanInfo), ++first)
	{
		DBBanInfo sDBBanInfo;
		memset(&sDBBanInfo, 0, sizeof(DBBanInfo));
		sDBBanInfo.m_cLevel	= first->m_banInfo.m_cLevel;
		sDBBanInfo.m_dwGID	= first->m_banInfo.m_dwGID;
		sDBBanInfo.m_wClass	= first->m_banInfo.m_wClass;
		memcpy(szInfoBufferPos, &sDBBanInfo, sizeof(DBBanInfo));
	}

	dwInfoBufferSize_InOut = static_cast<unsigned long>(szInfoBufferPos - szInfoBuffer_Out);

	return true;
}

bool CBanList::SerializeIn(const char* szBuffer_In, const char* szInfo_In, unsigned long dwBufferSize_In, unsigned long dwInfoBufferSize_In)
{
	const char* szBufferPos			= szBuffer_In;
	const char* szBufferPastEnd		= szBuffer_In + dwBufferSize_In;
	const char* szBufferInfoPos		= szInfo_In;
	const char* szBufferInfoPastEnd	= szInfo_In + dwInfoBufferSize_In;

	Rebind rebind;

	for(;szBufferPos + sizeof(DBBan) <= szBufferPastEnd; szBufferPos += sizeof(DBBan), szBufferInfoPos += sizeof(DBBanInfo))	
	{
		BanInfo banInfo;

		memcpy(&banInfo, szBufferPos, 4);

		memset(banInfo.m_szName, 0, DBFriend::MAX_NAME);
		memcpy(banInfo.m_szName, szBufferPos+4, DBFriend::MAX_NAME);

		memcpy(&banInfo.m_dwGID, szBufferInfoPos, 7);

		unsigned long dwBanCID = reinterpret_cast<const BanInfo*>(szBufferPos)->m_dwCID;

		BanList::iterator lbound = std::lower_bound(m_banList.begin(), m_banList.end(), dwBanCID);
		
		if(lbound == m_banList.end() || dwBanCID != lbound->m_banInfo.m_dwCID)
		{
			memcpy(&rebind.m_banInfo, &banInfo, sizeof(BanInfo));
			m_banList.insert(lbound, rebind);

			if(NULL != m_lpXRefTable)
			{
				rebind.m_XRefItr = m_lpXRefTable->Add(m_dwOwnerCID, dwBanCID);
			}
		}
	}

	return true;
}