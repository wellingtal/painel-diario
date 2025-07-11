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
        unsigned char   GetAdminLevel() const { return m_cAdminLevel; }         // 0 : ���� �ƴ�. 1,2,3,... ���� ����.
        unsigned char   GetIsPeaceMode() const { return m_CharData.IsPeaceMode(); }         // 0 : �⺻ ��� 1 : ���� ���
        const char*     GetName() const { return m_szName; }
        const char*     GetAccountName() const { return m_szAccountName; }

        unsigned char   GetFlags() const { return m_CharData.m_cFlags; }

        BattleInclination::CharData GetInclinationData() { return m_CharData; }
        CFriendList&                GetFriendList()      { return m_FriendList; }
        CBanList&                   GetBanList()         { return m_BanList; }

        // Ű�� �� �� �ִ� ����, ���Ƿ� ���� �����ϸ� 
        // ���߿� �˻��� ��Ǵ� ��찡 �����Ƿ� �Ժη� ���� �� ��.
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

        // ��Ŷ���κ��� �����͸� ������Ʈ�Ѵ�.
        bool UpdateData(const char* szData, int& nDataLen);
        static int EstimateBufferSize(unsigned short usUpdateFlag);

        bool IsSendLoginData() const { return m_bSendLoginData; }
        void SetSendLoginData(bool bSendLoginData) { m_bSendLoginData = bSendLoginData; }

    private:

        char                            m_szName[CHAR_INFOST::MAX_NAME_LEN];            // ���� ����(�̺�Ʈ)
        char                            m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN];  // ���� �Ұ�

        unsigned long                   m_dwUID;            // ���� �Ұ�        
		
        unsigned long                   m_dwServerID;       // ���� ����
        BattleInclination::CharData     m_CharData;         // ���� ����

        RejectOption                    m_RejectOption;     // �ź� �ɼ�
        unsigned char                   m_cAdminLevel;      // ���� �Ұ�
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

        CharInfoMap         m_cidMap;       // ��ü ������ CID������.
                
        CharInfoMultiMap    m_pidMap;       // PID�� 0�� ����, ���� �ʴ´�.
        CharInfoMultiMap    m_gidMap;       // GID�� 0�� ����, ���� �ʴ´�.

		CharInfoMultiMap    m_raceMap;       // Race�� 

        CharInfoMultiMap    m_nameMap;      // ĳ���� �̸����� ���� �����. ���� �����ϴ�.
        CharInfoMultiMap    m_accountMap;   // ĳ���� �������� ���� �����. ���� �Ұ����ϴ�.

		CharBanMap			m_banMap;	    // ä�� ����Ʈ
    };
}

// �� ������ ä�� ���� �˸���
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