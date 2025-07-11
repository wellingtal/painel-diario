#ifndef _FRIEND_LIST_H_
#define _FRIEND_LIST_H_

#include <Network/Packet/PacketStruct/FriendPacket.h>

#include <map>
#include <list>
#include <vector>
#include <utility>
#include <algorithm>


class CXRefFriends
{
public:

    static CXRefFriends& GetInstance();

    typedef std::multimap<unsigned long, unsigned long>     XRefTable;

    CXRefFriends() { }
    ~CXRefFriends() { }

    // FnProcess는 bool operator() (CXRefFriends::XRefTable::value_type&) 함수를 갖는 구조체(클래스)이다.    
    template<typename FnProcess>
        void Process(unsigned long dwFriendID, FnProcess fnProcess)
    {
        // dwFriendID를 친구로 가진 모든 CID에 대해서 작업한다.
        std::pair<XRefTable::iterator, XRefTable::iterator> result = 
            m_XRefTable.equal_range(dwFriendID);

        std::for_each(result.first, result.second, fnProcess);
    }

private:

    XRefTable::iterator Add(unsigned long dwOwnerID, unsigned long dwFriendID)
    { return m_XRefTable.insert(std::make_pair(dwFriendID, dwOwnerID)); }

    void Remove(XRefTable::iterator erasePos) { m_XRefTable.erase(erasePos); }

    XRefTable           m_XRefTable;
    
    // Add, Remove Method를 감추기 위해서 friend로 지정.
    friend class        CFriendList;
};


class CFriendList
{
public:

    enum { MAX_FRIENDS_NUM  =   100 };

    class Rebind    
    {
    private:

        CXRefFriends::XRefTable::iterator   m_XRefItr;
        FriendInfo                          m_friendInfo;

        Rebind() : m_XRefItr() { }
        Rebind(const CXRefFriends::XRefTable::iterator& XRefItr, const FriendInfo& friendInfo)
            :   m_XRefItr(XRefItr), m_friendInfo(friendInfo) { }

    public:

        inline unsigned long GetCID() { return m_friendInfo.m_dwCID; }        
        inline unsigned long GetGroup() { return m_friendInfo.GetGroup(); }
        inline bool IsLogined() { return m_friendInfo.IsLogined(); }
        inline const char* GetCharacterName() { return m_friendInfo.m_szName; }

        inline bool IsFriend(const char* szName) { return 0 == strncmp(szName, m_friendInfo.m_szName, FriendInfo::MAX_NAME); }
        inline bool SetGroup(unsigned long dwGroup) { return m_friendInfo.SetGroup(dwGroup); }
        inline void SetLoginStatus(bool bLogin) { m_friendInfo.SetLoginStatus(bLogin); }

		inline void UpdateLevel(char cLevel)					{ m_friendInfo.m_cLevel		= cLevel;		}
		inline void UpdateGID(unsigned long dwGID)				{ m_friendInfo.m_dwGID		= dwGID;		}
		inline void UpdateServerID(unsigned long dwServerID)	{ m_friendInfo.m_dwServerID	= dwServerID;	}
		inline void UpdateClass(unsigned short wClass)			{ m_friendInfo.m_wClass		= wClass;		}

		inline char GetLevel()									{ return m_friendInfo.m_cLevel;		}
		inline unsigned long GetGID()							{ return m_friendInfo.m_dwGID;		}
		inline unsigned short GetClass()						{ return m_friendInfo.m_wClass;		}
		inline unsigned long GetServerID()						{ return m_friendInfo.m_dwServerID;	}

		inline void InitializeFriendInfo(unsigned long dwServerID, unsigned long dwGID, unsigned short wClass, char cLevel)
		{
			m_friendInfo.m_dwServerID	= dwServerID;
			m_friendInfo.m_dwGID		= (m_friendInfo.m_dwGID!=dwGID) ? dwGID : m_friendInfo.m_dwGID;
			m_friendInfo.m_cLevel		= (m_friendInfo.m_cLevel!=cLevel) ? cLevel : m_friendInfo.m_cLevel;
			m_friendInfo.m_wClass		= (m_friendInfo.m_wClass!=wClass) ? wClass : m_friendInfo.m_wClass;
		}

        friend class CFriendList;
        friend class CFindFriendCID;

        friend inline static bool operator < (unsigned long dwCID, const Rebind& rhs) { return dwCID < rhs.m_friendInfo.m_dwCID; }
        friend inline static bool operator < (const Rebind& lhs, unsigned long dwCID) { return lhs.m_friendInfo.m_dwCID < dwCID; }
        friend inline static bool operator < (const Rebind& lhs, const Rebind& rhs)   { return lhs.m_friendInfo.m_dwCID < rhs.m_friendInfo.m_dwCID; }
    };

	typedef std::vector<Rebind>   FriendList;

    CFriendList(unsigned long dwOwnerCID, CXRefFriends* lpXRefTable = NULL);
    ~CFriendList();
    
    void Clear();

    bool Add(unsigned long dwFriendCID, const char* szCharacterName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
    bool Remove(unsigned long dwFriendCID);

    // CID가 같다고, 이름까지 같은지는 보장할 수 없다. 반드시 확인할 것.
    Rebind* GetFriend(unsigned long dwFriendCID);
    Rebind* GetFriend(const char* szFriendName);

    bool SerializeIn(const char* szBuffer_In, unsigned long dwBufferSize_In);
    bool SerializeOut(char* szBuffer_Out, unsigned long& dwBufferSize_InOut) const;

	void GetCIDList(unsigned long* dwCID_In);

	// 디비용 //
	bool SerializeOut(char* szBuffer_Out, char* szInfoBuffer_Out, unsigned long& dwBufferSize_InOut, unsigned long& dwInfoBufferSize_InOut) const;
	bool SerializeIn(const char* szBuffer_In, const char* szInfo_In, unsigned long dwBufferSize_In, unsigned long dwInfoBufferSize_In);

    unsigned long GetFriendNum() const { return static_cast<unsigned long>(m_friendList.size()); }

    // FnProcess는 bool operator() (CFriendList::Rebind&) 함수를 갖는 구조체(클래스)이다.
    template<typename FnProcess>
        void Process(FnProcess fnProcess)
    {
        std::for_each(m_friendList.begin(), m_friendList.end(), fnProcess);
    }	

private:

    struct  Flags;    

    unsigned long   m_dwOwnerCID;       // XRef삽입용.
    CXRefFriends*   m_lpXRefTable;      // XRefTable포인터 (NULL이면 삽입 안 함)
    FriendList      m_friendList;
};



#endif