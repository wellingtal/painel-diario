#ifndef _CLIENT_BROADCAST_2ND_CELL_MGR_H_
#define _CLIENT_BROADCAST_2ND_CELL_MGR_H_

#include <map>
#include "BroadcastCharacterData.h"

//! 클라이언트에서만 사용할 브로드캐스트 관리자입니다.
//! 네트워크로 전송된 바이너리 데이터로부터 CCharacterData를 생성하고, 
//! 관리 및, 접근할 수 있는 인터페이스를 제공합니다.

//! 셀 로그인, 셀 로그아웃시 패킷을 받아서, 가시범위 내의 캐릭터 관리를 하고
//! 브로드캐스트를 받아서 변경된 캐릭터 데이터를 갱신해 줍니다.

/*!
    셀 브로드캐스트 패킷 구조 및 처리 (클라이언트쪽)
    
    1byte(브로드캐스트 타입)
        1. 셀 로그인
        2. 셀 로그인 데이터
        3. 셀 로그아웃
        4. 셀 브로드캐스트

    여기까지 파싱한 후에..

*/

class CNetworkPos;

namespace Broadcast2nd
{
    class CClientCellMgr
    {
    public:

        CClientCellMgr();
        ~CClientCellMgr();

        //! 셀에 있는 캐릭터 데이터를 전부 지우고, 새 CID로 초기화한다. (처음 접속 시 수행)
        void Initialize(unsigned long dwCID);

        //! 셀에 있는 캐릭터 데이터를 전부 지운다.
        void Clear();

        //! 셀에 캐릭터를 추가한다.
        void CellLogin(const char* szData, unsigned short usDataSize);

        //! 셀 내부의 데이터를 업데이트한다.
        void CellUpdateData(const char* szData, 
            unsigned short usDataSize, unsigned long dwCurrentPulse);

        //! 캐릭터 리스트에서 캐릭터를 제거한다.
        void CellLogout(unsigned long dwCID);

        //! 받은 Pulse와 다른 Pulse를 가지고 있는 캐릭터를 전부 제거한다.
        void RemoveCharDataByPulse(unsigned long dwCurrentPulse, unsigned long dwExceptCID);

        //! 셀에서 캐릭터 데이터를 얻어 온다.
        CCharacterData* GetCharacterData(unsigned long dwCID);
        CMonsterData*   GetMonsterData(unsigned long dwCID);
        
        void UpdatePos(unsigned long dwCID, const CNetworkPos& netPos);         //! 위치를 업데이트한다.
        void UpdateStallName(unsigned long dwCID, const char* szStallName = 0); //! 노점 이름을 업데이트한다.

        size_t GetCharacterNum() const { return m_CharacterDataMap.size(); }
        size_t GetMonsterNum() const { return m_MonsterDataMap.size(); }

        //! 순회 메서드. 캐릭터 데이터들 전체를 순회한다.
        template<typename FnProcess>
            void ProcessCharacter(FnProcess fnProcess)
        {
            CharacterDataMap::iterator pos = m_CharacterDataMap.begin();
            CharacterDataMap::iterator end = m_CharacterDataMap.end();
            for(; pos != end; ++pos) { fnProcess(pos->second); }
        }

        template<typename FnProcess>
            void ProcessMonster(FnProcess fnProcess)
        {
            MonsterDataMap::iterator pos = m_MonsterDataMap.begin();
            MonsterDataMap::iterator end = m_MonsterDataMap.end();
            for(; pos != end; ++pos) { fnProcess(pos->second); }
        }

    private:

        typedef std::map<unsigned long, CCharacterData> CharacterDataMap;
        typedef std::map<unsigned long, CMonsterData>   MonsterDataMap;
        
        unsigned long       m_dwCID;            // 나의 CID;               
        CharacterDataMap    m_CharacterDataMap;
        MonsterDataMap      m_MonsterDataMap;
    };
};

#endif
