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

        // ĳ���� ������ ��� ���� �޼ҵ�

        //! Ű ���� ��� �´�. ���⼭�� CID�̴�.
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

		


        // ģ��/�ź� ���� �޼���.
        bool AddFriend(unsigned long dwFriendCID, const char* szCharacterName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
        bool RemoveFriend(unsigned long dwFriendCID);
        
		bool AddBan(unsigned long dwBanCID, const char* szBanName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
        bool RemoveBan(unsigned long dwBanCID);

        bool SetFriendGroup(unsigned long dwCID, unsigned long dwGroup);

        bool FriendSerializeIn(const char* lpData, unsigned long dwDataLen);
        bool BanSerializeIn(const char* lpData, unsigned long dwDataLen);

        // ĳ���� ������ ��� ���� �޼ҵ�
		void SetUID(unsigned long dwUID);
        void SetPID(unsigned long dwPID);
        void SetGID(unsigned long dwGID);
        void SetServerID(unsigned long dwServerID);

        // �̸��� ��쿡�� ������Ʈ���� �ʴ´�!
        bool SetInfo(const CHAR_INFOST& charInfoST, bool bChangeName = false);
        void SetPos(const CHAR_POS& charPos);
        void SetSkill(const SKILL& skill);
        void SetQuick(const QUICK& quick);
		// ����
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

		
        //! ������ ���� ���� �޼ҵ�
        bool IsDataChanged() const { return 0 != m_dwUpdateData; }
        bool IsValidData() const { return !(0 == GetCID() || 0 == GetInfo().Level); }
        
        void ClearData();       //! �����͸� ���� Ŭ�����Ѵ�.
        void ReloadEquipView(); //! ��� �並 �ٽ� ����Ѵ�.
        
        //! �����͸� ��Ʈ��ũ�� �����ϱ� ���� �����Ѵ�.
        bool SerializeOut(char* lpBuffer_Out, unsigned short* lpSizeArray_Out, 
            unsigned long &dwBufferSize_InOut, unsigned long dwSizeArrayNum);

        //! ��Ʈ��ũ�κ��� ���۵Ǿ� �� �����͸� �����Ѵ�.
        bool SerializeIn(const char* lpBuffer_In, const unsigned short* lpSizeArray_In, 
            unsigned long dwBufferSize, unsigned long dwSizeArrayNum);

        //! DB�� ����� �����͸� ����Ѵ�.
        bool UpdateDBData(CDBComponent& DBComponent);

        //! ������ ��� �����͸� ����Ѵ�.
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

        //! DB�κ��� �����͸� ���ͼ� �����Ѵ�.
        bool GetFromDB(CDBComponent& DBComponent, unsigned long dwKey); 

        //! ������ �ִ� �����͸� ���Ϸ� ������ ����ų�, ���Ͽ��� �����͸� �ҷ� �´�.
        bool WriteDataToDumpFile(const char* szFileName);
        bool ReadDataFromDumpFile(const char* szFileName);

        //! �������� ���� �޼���
        bool ConvertToPart2Item(bool bConvertSkillToTicket);

		// ���
		bool DeductGold(unsigned long dwGold);
		bool AddGold(unsigned long dwGold);

    private:

        CHAR_INFOST			m_CharInfo;		// �⺻ ����
        CHAR_POS			m_CharPos;		// ��ġ ����
        SKILL				m_Skill;		// ��ų ����
        QUICK				m_Quick;		// ������

        CHAR_INFOEX			m_CharInfoEx;	// �߰� ����
        QUEST				m_Quest;		// ����Ʈ
        HISTORY				m_History;		// �����丮	
        CONFIG				m_Config;		// ����

        EQUIP				m_Equip;		// ���
        INVEN				m_Inven;		// �κ�
        EXTRA				m_Extra;		// ����
        EXCHANGE			m_Exchange;		// ��ȯ
        TEMPINVEN			m_TempInven;	// �ӽ� �κ��丮
		SPELL				m_Spell;		// ����

        CBanList			m_BanList;		// �ź�
        CFriendList			m_FriendList;	// ģ��


		TIME				m_LogginTime;	// ���缭���� �α��ε� �ð�.

		unsigned short		m_EquipView[MAX_EQUIP_VIEW];    // ��� ��
        unsigned long       m_dwUpdateData;                 // �����Ͱ� ������Ʈ�Ǿ������� ��Ÿ��.
        unsigned long       m_dwLastDBUpdateTime;

        unsigned long       m_dwUID;
    };

}
}

#endif