#ifndef _SERIALIZE_CHARACTER_DATA_H_
#define _SERIALIZE_CHARACTER_DATA_H_

#include "BroadcastCharacterData.h"
#include "ChatCharData.h"


//! �� ������ ������ ����ϴ� �����̴�. Ŭ���̾�Ʈ���� �����͸� ������ ���ؼ�
//! ���ۿ� ����ȭ(Serialze) �� �����ϰ�, �� �����͸� ������ �ִ� Ŭ�����̴�.
//! ����� ���� ���� �����͵� ������ �����Ƿ�, �� �ʸ��� ���� �����͸� �Ѹ���.

//! forward decl
class CCharacter;
class CMonster;

namespace Broadcast2nd
{
    class CSerializeCharacterData
    {
    public:

        enum
        {
            MAX_CHARACTER_DATA = 160
        };

        CSerializeCharacterData();
        ~CSerializeCharacterData();

        //! �ʱ� �����͸� �����Ѵ�.
        void InitializeData(CCharacter& character);

        //! �����͸� �����ϰ�, �غ��ϰ�, Delta�� ���Ѵ�.
        void PrepareData(CCharacter& character);
        
        void PrepareBroadcastData(CCharacter& character);
        void PrepareDeltaData(CCharacter& character);
        void ClearDeltaData() { m_usDeltaBroadcastDataLen = 0; }

        CCharacterData& GetCharacterData()  { return m_LastCharacterData; }

        const char*     GetBroadcastData()      const { return m_aryBroadcastData;      }
        unsigned short  GetBroadcastDataLen()   const { return m_usBroadcastDataLen;    }

        const char*     GetDeltaBroadcastData()      const { return m_aryDeltaBroadcastData;     }
        unsigned short  GetDeltaBroadcastDataLen()   const { return m_usDeltaBroadcastDataLen;   }
        
        bool SendChatLogin(CCharacter& character);
        bool SendChatLogout(CCharacter& character);

    private:

        // ä�ü����� ���� Diff�����͸� �غ��ϰ� �����Ѵ�.
        void SendDeltaChatData(CCharacter& character);

        CCharacterData      m_LastCharacterData;
        ChatData::CCharInfo m_LastChatData;

        unsigned long       m_dwPreparedCount;
        unsigned short      m_usBroadcastDataLen;
        unsigned short      m_usDeltaBroadcastDataLen;

        char                m_aryBroadcastData[MAX_CHARACTER_DATA];
        char                m_aryDeltaBroadcastData[MAX_CHARACTER_DATA];
    };

    class CSerializeMonsterData
    {
    public:

        enum
        {
            MAX_MONSTER_DATA = 64
        };

        CSerializeMonsterData();
        ~CSerializeMonsterData();

        //! �ʱ� �����͸� �����Ѵ�.
        void InitializeData(CMonster& monster);

        //! �����͸� �����ϰ�, �غ��ϰ�, Delta�� ���Ѵ�.
        void PrepareData(CMonster& monster);
        
        void PrepareBroadcastData(CMonster& monster);
        void PrepareDeltaData(CMonster& monster);
        void ClearDeltaData() { m_usDeltaBroadcastDataLen = 0; }

        CMonsterData& GetMonsterData()  { return m_LastMonsterData; }

        const char*     GetBroadcastData()      const { return m_aryBroadcastData;      }
        unsigned short  GetBroadcastDataLen()   const { return m_usBroadcastDataLen;    }

        const char*     GetDeltaBroadcastData()      const { return m_aryDeltaBroadcastData;     }
        unsigned short  GetDeltaBroadcastDataLen()   const { return m_usDeltaBroadcastDataLen;   }
        
    private:

        CMonsterData        m_LastMonsterData;

        unsigned long       m_dwPreparedCount;
        unsigned short      m_usBroadcastDataLen;
        unsigned short      m_usDeltaBroadcastDataLen;

        char                m_aryBroadcastData[MAX_MONSTER_DATA];
        char                m_aryDeltaBroadcastData[MAX_MONSTER_DATA];
    };
};


#endif