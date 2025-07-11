#ifndef _SERIALIZE_CHARACTER_DATA_H_
#define _SERIALIZE_CHARACTER_DATA_H_

#include "BroadcastCharacterData.h"
#include "ChatCharData.h"


//! 본 파일은 서버만 사용하는 파일이다. 클라이언트에게 데이터를 보내기 위해서
//! 버퍼에 직렬화(Serialze) 를 수행하고, 그 데이터를 가지고 있는 클래스이다.
//! 변경된 점에 대한 데이터도 가지고 있으므로, 매 초마다 변경 데이터를 뿌린다.

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

        //! 초기 데이터를 세팅한다.
        void InitializeData(CCharacter& character);

        //! 데이터를 세팅하고, 준비하고, Delta를 구한다.
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

        // 채팅서버로 보낼 Diff데이터를 준비하고 전송한다.
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

        //! 초기 데이터를 세팅한다.
        void InitializeData(CMonster& monster);

        //! 데이터를 세팅하고, 준비하고, Delta를 구한다.
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