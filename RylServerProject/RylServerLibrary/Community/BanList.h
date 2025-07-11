#ifndef _BAN_LIST_H_
#define _BAN_LIST_H_

#include <Network/Packet/PacketStruct/FriendPacket.h>

#include <list>
#include <vector>
#include <utility>
#include <algorithm>
#include <map>

class CXRefBans
{
public:

	static CXRefBans& GetInstance();

	typedef std::multimap<unsigned long, unsigned long>     XRefTable;

	CXRefBans() { }
	~CXRefBans() { }

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
	friend class        CBanList;
};

class CBanList
{
public:

	class Rebind
	{
	private:

		CXRefBans::XRefTable::iterator   m_XRefItr;
		BanInfo                          m_banInfo;

		Rebind() : m_XRefItr() {}
		Rebind(const CXRefBans::XRefTable::iterator& XRefItr, const BanInfo& banInfo)
			:   m_XRefItr(XRefItr), m_banInfo(banInfo) { }

	public:

		inline unsigned long GetCID()			{ return m_banInfo.m_dwCID; }		
		inline const char* GetCharacterName()	{ return m_banInfo.m_szName; }

		inline bool IsBan(const char* szName)	{ return 0 == strncmp(szName, m_banInfo.m_szName, FriendInfo::MAX_NAME); }

		inline void UpdateLevel(char cLevel)					{ m_banInfo.m_cLevel		= cLevel;		}
		inline void UpdateGID(unsigned long dwGID)				{ m_banInfo.m_dwGID			= dwGID;		}
		inline void UpdateServerID(unsigned long dwServerID)	{ m_banInfo.m_dwServerID	= dwServerID;	}
		inline void UpdateClass(unsigned short wClass)			{ m_banInfo.m_wClass		= wClass;		}

		inline char GetLevel()									{ return m_banInfo.m_cLevel;		}
		inline unsigned long GetGID()							{ return m_banInfo.m_dwGID;			}
		inline unsigned short GetClass()						{ return m_banInfo.m_wClass;		}
		inline unsigned long GetServerID()						{ return m_banInfo.m_dwServerID;	}

		inline void InitializeBanInfo(unsigned long dwServerID, unsigned long dwGID, unsigned short wClass, char cLevel)
		{
			m_banInfo.m_dwServerID	= dwServerID;
			m_banInfo.m_dwGID		= (m_banInfo.m_dwGID!=dwGID) ? dwGID : m_banInfo.m_dwGID;
			m_banInfo.m_cLevel		= (m_banInfo.m_cLevel!=cLevel) ? cLevel : m_banInfo.m_cLevel;
			m_banInfo.m_wClass		= (m_banInfo.m_wClass!=wClass) ? wClass : m_banInfo.m_wClass;
		}

		friend class CBanList;

		friend inline static bool operator < (unsigned long dwCID, const Rebind& rhs) { return dwCID < rhs.m_banInfo.m_dwCID; }
		friend inline static bool operator < (const Rebind& lhs, unsigned long dwCID) { return lhs.m_banInfo.m_dwCID < dwCID; }
		friend inline static bool operator < (const Rebind& lhs, const Rebind& rhs)   { return lhs.m_banInfo.m_dwCID < rhs.m_banInfo.m_dwCID; }
	};

    enum { MAX_BAN_NUM = 100 };

    CBanList(unsigned long dwOwnerCID, CXRefBans* lpXRefTable = NULL);
    ~CBanList();

    void Clear();

    bool Add(unsigned long dwBanCID, const char* szCharacterName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
    bool Remove(unsigned long dwBanCID);
    bool IsBan(unsigned long dwBanCID, const char* szCharacterName);

    // 이름으로 거부자의 CID를 리턴한다. 0이면 BanList에 그런 이름이 존재하지 않는다.
    unsigned long   GetBanCID(const char* szCharacterName);
    const char*     GetBanName(unsigned long dwCID);

	CBanList::Rebind* GetBan(const char* szFriendName);
	CBanList::Rebind* GetBan(unsigned long dwBanCID);

	void GetCIDList(unsigned long* dwCID_In);

    bool SerializeIn(const char* szBuffer_In, unsigned long dwBufferSize_In);
    bool SerializeOut(char* szBuffer_Out, unsigned long& dwBufferSize_InOut) const;

	bool SerializeOut(char* szBuffer_Out, char* szInfoBuffer_Out, unsigned long& dwBufferSize_InOut, unsigned long& dwInfoBufferSize_InOut) const;
	bool SerializeIn(const char* szBuffer_In, const char* szInfo_In, unsigned long dwBufferSize_In, unsigned long dwInfoBufferSize_In);

    unsigned long GetBanNum() const { return static_cast<unsigned long>(m_banList.size()); }

    // FnProcess는 bool operator() (const BanInfo&) 함수를 갖는 구조체(클래스)이다.
    template<typename FnProcess>
        void Process(FnProcess fnProcess)
    {
        std::for_each(m_banList.begin(), m_banList.end(), fnProcess);
    }

private:

    struct  Flags;
    typedef std::vector<Rebind>  BanList;

    BanList			m_banList;
	CXRefBans*		m_lpXRefTable;
	unsigned long	m_dwOwnerCID;
};

#endif