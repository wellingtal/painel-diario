#include "stdafx.h"
#include "FriendList.h"

#include <algorithm>

CXRefFriends& CXRefFriends::GetInstance()
{
    static CXRefFriends xRefFriends;
    return xRefFriends;
}

class CFindFriendCID
{
public:
    explicit CFindFriendCID(unsigned long dwFriendCID) : m_dwFriendCID(dwFriendCID) { }
    bool operator() (const CFriendList::Rebind& rebind) const { return (m_dwFriendCID == rebind.m_friendInfo.m_dwCID); }
private:
    unsigned long m_dwFriendCID;
};


CFriendList::CFriendList(unsigned long dwOwnerCID, CXRefFriends* lpXRefTable) 
:   m_dwOwnerCID(dwOwnerCID), m_lpXRefTable(lpXRefTable)
{

}


CFriendList::~CFriendList()
{
    Clear();
}



void CFriendList::Clear()
{
    if(NULL != m_lpXRefTable)
    {
        for(FriendList::iterator itr = m_friendList.begin();
            itr != m_friendList.end(); ++itr)
        {
            m_lpXRefTable->Remove(itr->m_XRefItr);
        }
    }

    m_friendList.clear();
}


bool CFriendList::Add(unsigned long dwFriendCID, const char* szCharacterName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID)
{
    if(m_friendList.size() < MAX_FRIENDS_NUM)
    {
        FriendList::iterator lbound = std::lower_bound(m_friendList.begin(), m_friendList.end(), dwFriendCID);

        Rebind  rebind;

        if(NULL != m_lpXRefTable)
        {
            rebind.m_XRefItr = m_lpXRefTable->Add(m_dwOwnerCID, dwFriendCID);
        }

        rebind.m_friendInfo.m_dwCID			= dwFriendCID;
		rebind.m_friendInfo.m_dwGID			= dwGID;
		rebind.m_friendInfo.m_wClass		= wClass;
		rebind.m_friendInfo.m_cLevel		= cLevel;
		rebind.m_friendInfo.m_dwServerID	= dwServerID;

        strncpy(rebind.m_friendInfo.m_szName, szCharacterName, FriendInfo::MAX_NAME);
        rebind.m_friendInfo.SetLoginStatus(true);

        if(lbound == m_friendList.end() || dwFriendCID != lbound->m_friendInfo.m_dwCID)
        {
            // 없는 경우에만 삽입.
            m_friendList.insert(lbound, rebind);            
        }
        else
        {
            // CID가 같은 경우는 덮어 씌운다.(이름은 다를 수도 있다..)
            *lbound = rebind;
        }

        return true;
    }

    return false;
}


bool CFriendList::Remove(unsigned long dwFriendCID)
{
    FriendList::iterator finditr = std::lower_bound(m_friendList.begin(), m_friendList.end(), dwFriendCID);

    if(finditr != m_friendList.end() && dwFriendCID == finditr->m_friendInfo.m_dwCID)
    {
        if(NULL != m_lpXRefTable)
        {
            m_lpXRefTable->Remove(finditr->m_XRefItr);
        }

        m_friendList.erase(finditr);
        return true;
    }

    return false;
}

CFriendList::Rebind* CFriendList::GetFriend(unsigned long dwFriendCID)
{    
    FriendList::iterator finditr = std::lower_bound(m_friendList.begin(), m_friendList.end(), dwFriendCID);

    if(finditr != m_friendList.end() && dwFriendCID == finditr->m_friendInfo.m_dwCID)
    {        
        return &(*finditr);
    };

    return NULL;
}

void CFriendList::GetCIDList(unsigned long* dwCID_In)
{
	FriendList::iterator first = m_friendList.begin();
	FriendList::iterator last  = m_friendList.end();

	for(unsigned char cIndex = 0; first != last; ++first)
	{
		dwCID_In[cIndex] = (*first).GetCID();		
	}	
}

CFriendList::Rebind* CFriendList::GetFriend(const char* szFriendName)
{
    FriendList::iterator first = m_friendList.begin();
    FriendList::iterator last  = m_friendList.end();

    for(;first != last; ++first)
    {
        if(0 == strncmp(first->GetCharacterName(), szFriendName, FriendInfo::MAX_NAME))
        {
            return &(*first);
        }
    }

    return NULL;
}


bool CFriendList::SerializeOut(char* szBuffer_Out, unsigned long& dwBufferSize_InOut) const
{        
	char* szBufferPos       = szBuffer_Out;
	char* szBufferPastEnd   = szBuffer_Out + dwBufferSize_InOut;

	FriendList::const_iterator first  = m_friendList.begin();
	FriendList::const_iterator last   = m_friendList.end();

	for(;szBufferPos + sizeof(FriendInfo) <= szBufferPastEnd && first != last;
		szBufferPos += sizeof(FriendInfo), ++first)
	{
		memcpy(szBufferPos, &first->m_friendInfo, sizeof(FriendInfo));
	}

	dwBufferSize_InOut = static_cast<unsigned long>(szBufferPos - szBuffer_Out);

	return true;
}


bool CFriendList::SerializeIn(const char* szBuffer_In, unsigned long dwBufferSize_In)
{    
	const char* szBufferPos       = szBuffer_In;
	const char* szBufferPastEnd   = szBuffer_In + dwBufferSize_In;

	Rebind rebind;

	for(;szBufferPos + sizeof(FriendInfo) <= szBufferPastEnd;
		szBufferPos += sizeof(FriendInfo))
	{
		const FriendInfo* lpFriendInfo = reinterpret_cast<const FriendInfo*>(szBufferPos);

		unsigned long dwFriendCID = lpFriendInfo->m_dwCID;

		FriendList::iterator lbound = std::lower_bound(m_friendList.begin(), m_friendList.end(), dwFriendCID);
		if(lbound == m_friendList.end() || dwFriendCID != lbound->m_friendInfo.m_dwCID)
		{
			// 데이터가 없다. 신규로 추가한다.
			memcpy(&rebind.m_friendInfo, szBufferPos, sizeof(FriendInfo));

			if(NULL != m_lpXRefTable)
			{
				rebind.m_XRefItr = m_lpXRefTable->Add(m_dwOwnerCID, dwFriendCID);
			}

			m_friendList.insert(lbound, rebind);
		}
		else if(0 == strncmp(lbound->m_friendInfo.m_szName, lpFriendInfo->m_szName, FriendInfo::MAX_NAME))
		{
			// 이미 CID/Name이 같은 데이터가 있으면, 상태 플래그만 업데이트한다.
			lbound->m_friendInfo.m_dwStatusFlag = lpFriendInfo->m_dwStatusFlag;
		}
	}

	return true;
}

bool CFriendList::SerializeOut(char* szBuffer_Out, char* szInfoBuffer_Out, unsigned long& dwBufferSize_InOut, unsigned long& dwInfoBufferSize_InOut) const
{        
	char* szBufferPos			= szBuffer_Out;
	char* szBufferPastEnd		= szBuffer_Out + dwBufferSize_InOut;
	char* szInfoBufferPos       = szInfoBuffer_Out;
	char* szInfoBufferPastEnd   = szInfoBuffer_Out + dwInfoBufferSize_InOut;

	FriendList::const_iterator first  = m_friendList.begin();
	FriendList::const_iterator last   = m_friendList.end();

	for(;szBufferPos + sizeof(DBFriend) <= szBufferPastEnd && first != last;
		szBufferPos += sizeof(DBFriend), ++first)
	{
		DBFriend sDBFriend;
		memset(&sDBFriend, 0, sizeof(DBFriend));
		sDBFriend.m_dwCID			= first->m_friendInfo.m_dwCID;
		sDBFriend.m_dwStatusFlag	= first->m_friendInfo.m_dwServerID;
		memcpy(sDBFriend.m_szName, first->m_friendInfo.m_szName, DBFriend::MAX_NAME);

		memcpy(szBufferPos, &sDBFriend, sizeof(DBFriend));
	}

	dwBufferSize_InOut = static_cast<unsigned long>(szBufferPos - szBuffer_Out);

	first  = m_friendList.begin();
	last   = m_friendList.end();

	for(;szInfoBufferPos + sizeof(DBFriendInfo) <= szInfoBufferPastEnd && first != last;
		szInfoBufferPos += sizeof(DBFriendInfo), ++first)
	{
		DBFriendInfo sDBFriendInfo;
		memset(&sDBFriendInfo, 0, sizeof(DBFriendInfo));
		sDBFriendInfo.m_cLevel	= first->m_friendInfo.m_cLevel;
		sDBFriendInfo.m_dwGID	= first->m_friendInfo.m_dwGID;
		sDBFriendInfo.m_wClass	= first->m_friendInfo.m_wClass;
		memcpy(szInfoBufferPos, &sDBFriendInfo, sizeof(DBFriendInfo));
	}

	dwInfoBufferSize_InOut = static_cast<unsigned long>(szInfoBufferPos - szInfoBuffer_Out);

	return true;
}


bool CFriendList::SerializeIn(const char* szBuffer_In, const char* szInfo_In, unsigned long dwBufferSize_In, unsigned long dwInfoBufferSize_In)
{    
	const char* szBufferPos			= szBuffer_In;
	const char* szBufferPastEnd		= szBuffer_In + dwBufferSize_In;
	const char* szBufferInfoPos		= szInfo_In;
	const char* szBufferInfoPastEnd	= szInfo_In + dwInfoBufferSize_In;
	
	Rebind rebind;

	for(;szBufferPos + sizeof(DBFriend) <= szBufferPastEnd; szBufferPos += sizeof(DBFriend), szBufferInfoPos += sizeof(DBFriendInfo))
	{
		FriendInfo sFriendInfo;

		memcpy(&sFriendInfo, szBufferPos, 8);
		
		memset(sFriendInfo.m_szName, 0, DBFriend::MAX_NAME);
		memcpy(sFriendInfo.m_szName, szBufferPos+8, DBFriend::MAX_NAME);

		memcpy(&sFriendInfo.m_dwGID, szBufferInfoPos, 7);
		
		unsigned long dwFriendCID = sFriendInfo.m_dwCID;		
		
		FriendList::iterator lbound = std::lower_bound(m_friendList.begin(), m_friendList.end(), dwFriendCID);
		
		if(lbound == m_friendList.end() || dwFriendCID != lbound->m_friendInfo.m_dwCID)
		{
			// 데이터가 없다. 신규로 추가한다.
			memcpy(&rebind.m_friendInfo, &sFriendInfo, sizeof(FriendInfo));

			if(NULL != m_lpXRefTable)
			{
				rebind.m_XRefItr = m_lpXRefTable->Add(m_dwOwnerCID, dwFriendCID);
			}

			m_friendList.insert(lbound, rebind);

		}
		else if(0 == strncmp(lbound->m_friendInfo.m_szName, sFriendInfo.m_szName, FriendInfo::MAX_NAME))
		{
			// 이미 CID/Name이 같은 데이터가 있으면, 상태 플래그만 업데이트한다.
			lbound->m_friendInfo.m_dwStatusFlag = sFriendInfo.m_dwStatusFlag;
		}
	}

	return true;
}