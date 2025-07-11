#ifndef _GAMA_RYL_CHATSERVER_CHAT_SEND_H_
#define _GAMA_RYL_CHATSERVER_CHAT_SEND_H_

#include <boost/pool/pool_alloc.hpp>

#include <Community/FriendList.h>
#include <Community/BanList.h>

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>
#include <Network/Packet/PacketStruct/CharConfigPacketStruct.h>
#include <Creature/CreatureStructure.h>

class CMultiDispatch;

namespace ChatData
{
    enum DeltaInfo
    {
        DELTA_NAME          =	(1 << 0),        
        DELTA_PID           =	(1 << 1),
        DELTA_GID           =	(1 << 2),
        DELTA_CASTLEID      =	(1 << 3),
        DELTA_SIEGESTATE    =	(1 << 4),
        DELTA_NATION        =	(1 << 5),
        DELTA_ETCFLAGS      =	(1 << 6),
        DELTA_REJECTOPTION  =	(1 << 7),
        DELTA_ADMINLEVEL    =	(1 << 8),
        DELTA_ACCOUNTNAME   =	(1 << 9),
		DELTA_CREATURETYPE	=	(1 << 10),
		DELTA_OBJECTTYPE	=	(1 << 11),
		DELTA_KID			=	(1 << 12),
		DELTA_WARFLAG		=	(1 << 13),
    };

    class CCharInfo
    {
    public:

        CCharInfo(unsigned long dwUID, unsigned long dwCID, 
            unsigned long dwServerID, CXRefFriends* lpXRefFriends = 0, CXRefBans* lpXRefBans = 0);

		unsigned char	GetRealmWarFlag() const	{ return m_CharData.m_cRealmWarFlag;	}
		unsigned char	GetGuildWarFlag() const	{ return m_CharData.m_cGuildWarFlag;	}

		unsigned char	GetTacticsFlag() const	{ return m_CharData.m_cTacticsFlag;		}
        unsigned long   GetUID() const			{ return m_dwUID;						}
        unsigned long   GetCID() const			{ return m_CharData.m_dwCID;			}
        unsigned long   GetPID() const			{ return m_CharData.m_dwPID;			}
        unsigned long   GetGID() const			{ return m_CharData.m_dwGID;			}        
        unsigned long   GetServerID() const		{ return m_dwServerID;					}
		unsigned char	GetCreatureType() const	{ return m_CharData.m_cCreatureType;	}
		unsigned short	GetObjectType() const	{ return m_CharData.m_wObjectType;		}
		unsigned short	GetKind() const			{ return m_CharData.m_wKind;			}

        unsigned long   GetCastleID() const { return m_CharData.m_dwCastleID; }
        unsigned char   GetSiegeState() const { return m_CharData.m_cSiegeState; }
        unsigned char   GetNation() const { return m_CharData.m_cNation; }
        
        RejectOption    GetRejectOption() const { return m_RejectOption; }

        unsigned char   GetRace() const { return m_CharData.m_cNation; }
        unsigned char   GetAdminLevel() const { return m_cAdminLevel; }         // 0 : 어드민 아님. 1,2,3,... 어드민 레벨.
        unsigned char   GetIsPeaceMode() const { return m_CharData.IsPeaceMode(); }         // 0 : 기본 모드 1 : 반전 모드
        const char*     GetName() const { return m_szName; }
        const char*     GetAccountName() const { return m_szAccountName; }

        unsigned char   GetFlags() const { return m_CharData.m_cFlags; }

        BattleInclination::CharData GetInclinationData() { return m_CharData; }
        CFriendList&                GetFriendList()      { return m_FriendList; }
        CBanList&                   GetBanList()         { return m_BanList; }

        // 키가 될 수 있는 값은, 임의로 값을 수정하면 
        // 나중에 검색시 즐되는 경우가 있으므로 함부로 쓰지 말 것.
		void SetTacticsFlag(unsigned char cFlag)	{ m_CharData.m_cTacticsFlag		= cFlag;		}
        void SetUID(unsigned long dwUID)			{ m_dwUID						= dwUID;		}
        void SetCID(unsigned long dwCID)			{ m_CharData.m_dwCID			= dwCID;		}
        void SetPID(unsigned long dwPID)			{ m_CharData.m_dwPID			= dwPID;		}
        void SetGID(unsigned long dwGID)			{ m_CharData.m_dwGID			= dwGID;		}
        void SetServerID(unsigned long dwServerID)	{ m_dwServerID					= dwServerID;	}

		void SetRealmWarFlag(unsigned char cFlag)	{ m_CharData.m_cRealmWarFlag	= cFlag;		}
		void SetGuildWarFlag(unsigned char cFlag)	{ m_CharData.m_cGuildWarFlag	= cFlag;		}

        void SetName(const char* szName) 
        { 
            strncpy(m_szName, szName, CHAR_INFOST::MAX_NAME_LEN);
            m_szName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
        }

        void SetAccountName(const char* szAccountName)
        {
            strncpy(m_szAccountName, szAccountName, CHAR_INFOST::MAX_ACCOUNT_LEN);
            m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN - 1] = 0;
        }
        
        void SetCastleID(unsigned long dwCastleID)			{ m_CharData.m_dwCastleID = dwCastleID;			}
        void SetSiegeState(unsigned char cSiegeState)		{ m_CharData.m_cSiegeState = cSiegeState;		}
        void SetNation(unsigned char cNation)				{ m_CharData.m_cNation = cNation;				}
        void SetFlags(unsigned char cFlags)					{ m_CharData.m_cFlags = cFlags;					}
		void SetCreatureType(unsigned char cCreatureType)	{ m_CharData.m_cCreatureType = cCreatureType;	}
		void SetObjectType(unsigned short wObjectType)		{ m_CharData.m_wObjectType = wObjectType;		}
		void SetKind(unsigned short wKind)					{ m_CharData.m_wKind = wKind;					}
        
        void SetRejectOption(RejectOption rejectOption) { m_RejectOption = rejectOption; }
        void SetAdminLevel(unsigned char cAdminLevel) { m_cAdminLevel = cAdminLevel; }

        // 패킷으로부터 데이터를 업데이트한다.
        bool UpdateData(const char* szData, int& nDataLen);
        static int EstimateBufferSize(unsigned short usUpdateFlag);

        bool IsSendLoginData() const { return m_bSendLoginData; }
        void SetSendLoginData(bool bSendLoginData) { m_bSendLoginData = bSendLoginData; }

    private:

        char                            m_szName[CHAR_INFOST::MAX_NAME_LEN];            // 수정 가능(이벤트)
        char                            m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN];  // 수정 불가

        unsigned long                   m_dwUID;            // 수정 불가        
		
        unsigned long                   m_dwServerID;       // 수정 가능
        BattleInclination::CharData     m_CharData;         // 수정 가능

        RejectOption                    m_RejectOption;     // 거부 옵션
        unsigned char                   m_cAdminLevel;      // 수정 불가
        bool                            m_bSendLoginData;   

        CBanList                        m_BanList;
        CFriendList                     m_FriendList;
    };


    class CCharInfoMap
    {
    public:

        typedef std::map<unsigned long, CCharInfo*, std::less<unsigned long>,  
            boost::fast_pool_allocator<std::pair<const unsigned long, CCharInfo*> > >		CharInfoMap;

        typedef std::multimap<unsigned long, CCharInfo*, std::less<unsigned long>, 
            boost::fast_pool_allocator< std::pair <const unsigned long, CCharInfo*> > >		CharInfoMultiMap;        

        typedef std::map<unsigned long, unsigned long, std::less<unsigned long>, 
            boost::fast_pool_allocator< std::pair <const unsigned long, unsigned long> > >	CharBanMap;        

        static CCharInfoMap& GetInstance();

        bool AddCharInfo(CCharInfo* lpCharInfo);
        void RemoveCharInfo(unsigned long dwServerID);
        CCharInfo* RemoveCharInfo(unsigned long dwUID, unsigned long dwCID, unsigned long dwServerID);

		bool AddCharChatBan(unsigned long dwTargetCID, unsigned long dwMinutes);
		bool DeleteCharChatBan(unsigned long dwCID);
		unsigned long GetCharChatBan(unsigned long dwCID);
		void ProcessCharChatBan(CMultiDispatch& dispatch);

		bool GetCharChatBanInfo(unsigned long dwTargetCID, unsigned long& dwMinutes);

        CCharInfo* GetCharInfoByUID(unsigned long dwUID);
        CCharInfo* GetCharInfoByCID(unsigned long dwCID);
        CCharInfo* GetCharInfoByName(const char* szName);
        CCharInfo* GetCharInfoByAccountName(const char* szAccountName);
                
        bool Update(unsigned long dwUID, unsigned long dwCID, 
            unsigned long dwServerID, const char* lpData, int& nDataLen);

        template<typename FnProcess>
            void EnumerateByPID(unsigned long dwPID, FnProcess fnProcess)
        {
            std::pair<CharInfoMultiMap::iterator, CharInfoMultiMap::iterator> result
                = m_pidMap.equal_range(dwPID);

            for(; result.first != result.second; ++result.first)
            {
                fnProcess(*result.first->second);
            }
        }

        template<typename FnProcess>
            void EnumerateByGID(unsigned long dwGID, FnProcess fnProcess)
        {
            std::pair<CharInfoMultiMap::iterator, CharInfoMultiMap::iterator> result
                = m_gidMap.equal_range(dwGID);

            for(; result.first != result.second; ++result.first)
            {
                fnProcess(*result.first->second);
            }
        }

        template<typename FnProcess>
            void EnumerateByRace(unsigned long dwRace, FnProcess fnProcess)
        {
            std::pair<CharInfoMultiMap::iterator, CharInfoMultiMap::iterator> result
                = m_raceMap.equal_range(dwRace);

            for(; result.first != result.second; ++result.first)
            {
                fnProcess(*result.first->second);
            }
        }

        template<typename FnProcess>
            void EnumerateAll(FnProcess fnProcess)
        {
            CharInfoMap::iterator pos = m_cidMap.begin();
            CharInfoMap::iterator end = m_cidMap.end();

            for(; pos != end; ++pos)
            {
                fnProcess(*pos->second);
            }
        }

        void Destroy();

    private:

        ~CCharInfoMap();

        CharInfoMap         m_cidMap;       // 객체 관리는 CID맵으로.
                
        CharInfoMultiMap    m_pidMap;       // PID가 0인 경우는, 넣지 않는다.
        CharInfoMultiMap    m_gidMap;       // GID가 0인 경우는, 넣지 않는다.

		CharInfoMultiMap    m_raceMap;       // Race가 

        CharInfoMultiMap    m_nameMap;      // 캐릭터 이름으로 맵을 만든다. 변경 가능하다.
        CharInfoMultiMap    m_accountMap;   // 캐릭터 계정으로 맵을 만든다. 변경 불가능하다.

		CharBanMap			m_banMap;	    // 채금 리스트
    };
}

// 전 서버에 채금 상태 알리기
class CSendCharChatBanAllGameServer
{
public:

    CSendCharChatBanAllGameServer(unsigned long dwCID, unsigned long dwMinutes) 
	: m_dwCID(dwCID), m_dwMinutes(dwMinutes) 
	{ }
    
	bool operator () (unsigned long dwServerID, CPacketDispatch& dispatch);

private:

    unsigned long   m_dwCID;
	bool			m_dwMinutes;
};

#endif