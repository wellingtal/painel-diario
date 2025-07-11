#ifndef _RYL_DBAGENT_CHARACTER_DATA_H_
#define _RYL_DBAGENT_CHARACTER_DATA_H_

#include <DB/DBdefine.h>
#include <Community/FriendList.h>
#include <Community/BanList.h>
#include <Skill/Spell/Affected.h>

#include <map>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>

// forward decl.
class CDBComponent;

namespace DBAgent
{
namespace DataStorage
{
    // forward decl.
    class CSessionData;

    template<typename Key, typename Data, typename Pred>
    class CDBDataMgr;

    class CCharacterData
    {
    public:

        enum
        {
            MAX_EQUIP_VIEW = 15
        };

		enum CHANGED_CHAR_DATA
		{
			CHANGED_CHAR_INFOST	    = (1 <<  0),
			CHANGED_CHAR_POS	    = (1 <<  1),
			CHANGED_SKILL		    = (1 <<  2),
			CHANGED_QUICK		    = (1 <<  3),
			CHANGED_EQUIP		    = (1 <<  4),
			CHANGED_INVEN		    = (1 <<  5),
			CHANGED_EXTRA		    = (1 <<  6),
			CHANGED_EXCHANGE		= (1 <<  7),
			CHANGED_TEMPINVEN	    = (1 <<  8),
			CHANGED_CHAR_INFOEX	    = (1 <<  9),
			CHANGED_QUEST		    = (1 << 10),
			CHANGED_HISTORY		    = (1 << 11),
			CHANGED_CONFIG		    = (1 << 12),
			CHANGED_FRIENDLIST      = (1 << 13),
			CHANGED_BANLIST         = (1 << 14),
			CHANGED_SPELL		    = (1 << 15),
			CHANGED_ENCHANT		    = (1 << 16)
		};


        CCharacterData();
        ~CCharacterData();

        // 캐릭터 데이터 얻기 관련 메소드

        //! 키 값을 얻어 온다. 여기서는 CID이다.
        unsigned long GetKey() const        { return GetCID(); }

        unsigned long GetCID() const        { return m_CharInfo.CID;			}
        unsigned long GetPID() const	    { return m_CharInfo.PID;			}
        unsigned long GetGID() const	    { return m_CharInfo.GID;			}
        unsigned long GetServerID() const   { return m_CharInfoEx.ServerID;		}
		unsigned char GetAdminFlag() const	{ return m_CharInfoEx.cAdminFlag;	}
		
        const char* GetName() const	        { return m_CharInfo.Name;           }
        char        GetRace() const	        { return m_CharInfo.Race;			}

		unsigned short GetClass()			{ return m_CharInfo.Class;		}
		char GetLevel()	const				{ return m_CharInfo.Level;		}

        const CHAR_INFOST&	GetInfo() const		    { return m_CharInfo;	    }
        const CHAR_POS&		GetPos() const 		    { return m_CharPos;		    }
        const SKILL&		GetSkill() const		{ return m_Skill;			}
        const QUICK&		GetQuick() const 	    { return m_Quick;			}
		const SPELL&		GetSpell() const		{ return m_Spell;			}

        const EQUIP&		GetEquip() const		{ return m_Equip;			}
        const INVEN&		GetInven() const		{ return m_Inven;			}
        const EXTRA&		GetExtra() const		{ return m_Extra;			}
        const EXCHANGE&		GetExchange() const     { return m_Exchange;		}
        const TEMPINVEN&	GetTempInven() const    { return m_TempInven;		}

		const CHAR_INFOEX&	GetInfoEx() const	    { return m_CharInfoEx;	    }
        const QUEST&		GetQuest() const		{ return m_Quest;			}
        const HISTORY&		GetHistory() const	    { return m_History;			}
        const CONFIG&		GetConfig() const	    { return m_Config;			}

        const CFriendList&	GetFriendList()	const	{ return m_FriendList;  }
        const CBanList&		GetBanList() const		{ return m_BanList;		}

		TIME*				GetLogoutTime()			{ return &m_CharInfoEx.LoggoutTime;	}
		TIME*				GetLoginTime()			{ return &m_LogginTime;	}

		


        // 친구/거부 관련 메서드.
        bool AddFriend(unsigned long dwFriendCID, const char* szCharacterName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
        bool RemoveFriend(unsigned long dwFriendCID);
        
		bool AddBan(unsigned long dwBanCID, const char* szBanName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
        bool RemoveBan(unsigned long dwBanCID);

        bool SetFriendGroup(unsigned long dwCID, unsigned long dwGroup);

        bool FriendSerializeIn(const char* lpData, unsigned long dwDataLen);
        bool BanSerializeIn(const char* lpData, unsigned long dwDataLen);

        // 캐릭터 데이터 기록 관련 메소드
		void SetUID(unsigned long dwUID);
        void SetPID(unsigned long dwPID);
        void SetGID(unsigned long dwGID);
        void SetServerID(unsigned long dwServerID);

        // 이름의 경우에는 업데이트하지 않는다!
        bool SetInfo(const CHAR_INFOST& charInfoST, bool bChangeName = false);
        void SetPos(const CHAR_POS& charPos);
        void SetSkill(const SKILL& skill);
        void SetQuick(const QUICK& quick);
		// 스펠
		void SetSpell(const SPELL& spell);

		void SetInfoEx(const CHAR_INFOEX& charInfoEx);
        void SetQuest(const QUEST& quest);
        void SetHistory(const HISTORY& history);
        void SetConfig(const CONFIG& config);

        bool SetEquip(const char* szData, unsigned long dwDataLen);
        bool SetInven(const char* szData, unsigned long dwDataLen);
        bool SetExtra(const char* szData, unsigned long dwDataLen);
        bool SetExchange(const char* szData, unsigned long dwDataLen);
        bool SetTempInven(const char* szData, unsigned long dwDataLen);

		
        //! 데이터 변경 관련 메소드
        bool IsDataChanged() const { return 0 != m_dwUpdateData; }
        bool IsValidData() const { return !(0 == GetCID() || 0 == GetInfo().Level); }
        
        void ClearData();       //! 데이터를 전부 클리어한다.
        void ReloadEquipView(); //! 장비 뷰를 다시 계산한다.
        
        //! 데이터를 네트워크로 전송하기 좋게 가공한다.
        bool SerializeOut(char* lpBuffer_Out, unsigned short* lpSizeArray_Out, 
            unsigned long &dwBufferSize_InOut, unsigned long dwSizeArrayNum);

        //! 네트워크로부터 전송되어 온 데이터를 적용한다.
        bool SerializeIn(const char* lpBuffer_In, const unsigned short* lpSizeArray_In, 
            unsigned long dwBufferSize, unsigned long dwSizeArrayNum);

        //! DB에 변경된 데이터만 기록한다.
        bool UpdateDBData(CDBComponent& DBComponent);

        //! 강제로 모든 데이터를 기록한다.
        bool ForceUpdateDBAllData(CDBComponent& DBComponent)
        {
            m_dwUpdateData = 0xFFFFFFFF;
            return UpdateDBData(DBComponent);
        }     

		void SetFriendUpdateData(void)	{ m_dwUpdateData |= CCharacterData::CHANGED_FRIENDLIST;		}
		void SetBadUpdateData(void)		{ m_dwUpdateData |= CCharacterData::CHANGED_BANLIST;		}

		void SetAdminFlag(unsigned char cAdminFlag) { m_CharInfoEx.cAdminFlag = cAdminFlag;			}

		unsigned char GetRealmCheckPoint(void) const			{ return m_CharInfoEx.RealmCheckPoint;				}
		void SetRealmCheckPoint(unsigned char cRealmCheckPoint)	{ m_CharInfoEx.RealmCheckPoint = cRealmCheckPoint;	}

		unsigned char GetRealmMinute(void) const				{ return m_CharInfoEx.RealmMinute;					}
		void SetRealmMinute(unsigned char cRealmMinute)			{ m_CharInfoEx.RealmMinute = cRealmMinute;			}

		unsigned char GetGuildWarFlag(void) const		{ return m_CharInfoEx.GuildWarFlag;	}
		unsigned char GetRealmWarFlag(void) const		{ return m_CharInfoEx.RealmWarFlag;	}
		bool	SetGuildWarFlag(unsigned char cFlag);
		bool	SetRealmWarFlag(unsigned char cFlag);

		unsigned char GetRealmPoint() const				{ return m_CharInfoEx.RealmPoint;			}
		void SetRealmPoint(unsigned char cRealmPoint)	{ m_CharInfoEx.RealmPoint = cRealmPoint;	}

        unsigned long GetLastDBUpdateTime() const { return m_dwLastDBUpdateTime; }

        //! DB로부터 데이터를 얻어와서 세팅한다.
        bool GetFromDB(CDBComponent& DBComponent, unsigned long dwKey); 

        //! 가지고 있는 데이터를 파일로 덤프를 남기거나, 파일에서 데이터를 불러 온다.
        bool WriteDataToDumpFile(const char* szFileName);
        bool ReadDataFromDumpFile(const char* szFileName);

        //! 서버통합 관련 메서드
        bool ConvertToPart2Item(bool bConvertSkillToTicket);

		// 길드
		bool DeductGold(unsigned long dwGold);
		bool AddGold(unsigned long dwGold);

    private:

        CHAR_INFOST			m_CharInfo;		// 기본 정보
        CHAR_POS			m_CharPos;		// 위치 정보
        SKILL				m_Skill;		// 스킬 정보
        QUICK				m_Quick;		// 퀵슬롯

        CHAR_INFOEX			m_CharInfoEx;	// 추가 정보
        QUEST				m_Quest;		// 퀘스트
        HISTORY				m_History;		// 히스토리	
        CONFIG				m_Config;		// 설정

        EQUIP				m_Equip;		// 장비
        INVEN				m_Inven;		// 인벤
        EXTRA				m_Extra;		// 여분
        EXCHANGE			m_Exchange;		// 교환
        TEMPINVEN			m_TempInven;	// 임시 인벤토리
		SPELL				m_Spell;		// 스펠

        CBanList			m_BanList;		// 거부
        CFriendList			m_FriendList;	// 친구


		TIME				m_LogginTime;	// 현재서버에 로그인된 시간.

		unsigned short		m_EquipView[MAX_EQUIP_VIEW];    // 장비 뷰
        unsigned long       m_dwUpdateData;                 // 데이터가 업데이트되었는지를 나타냄.
        unsigned long       m_dwLastDBUpdateTime;

        unsigned long       m_dwUID;
    };

}
}

#endif