#ifndef _CLIENT_BROADCAST_2ND_CELL_MGR_H_
#define _CLIENT_BROADCAST_2ND_CELL_MGR_H_

#include <map>
#include "BroadcastCharacterData.h"

//! Ŭ���̾�Ʈ������ ����� ��ε�ĳ��Ʈ �������Դϴ�.
//! ��Ʈ��ũ�� ���۵� ���̳ʸ� �����ͷκ��� CCharacterData�� �����ϰ�, 
//! ���� ��, ������ �� �ִ� �������̽��� �����մϴ�.

//! �� �α���, �� �α׾ƿ��� ��Ŷ�� �޾Ƽ�, ���ù��� ���� ĳ���� ������ �ϰ�
//! ��ε�ĳ��Ʈ�� �޾Ƽ� ����� ĳ���� �����͸� ������ �ݴϴ�.

/*!
    �� ��ε�ĳ��Ʈ ��Ŷ ���� �� ó�� (Ŭ���̾�Ʈ��)
    
    1byte(��ε�ĳ��Ʈ Ÿ��)
        1. �� �α���
        2. �� �α��� ������
        3. �� �α׾ƿ�
        4. �� ��ε�ĳ��Ʈ

    ������� �Ľ��� �Ŀ�..

*/

class CNetworkPos;

namespace Broadcast2nd
{
    class CClientCellMgr
    {
    public:

        CClientCellMgr();
        ~CClientCellMgr();

        //! ���� �ִ� ĳ���� �����͸� ���� �����, �� CID�� �ʱ�ȭ�Ѵ�. (ó�� ���� �� ����)
        void Initialize(unsigned long dwCID);

        //! ���� �ִ� ĳ���� �����͸� ���� �����.
        void Clear();

        //! ���� ĳ���͸� �߰��Ѵ�.
        void CellLogin(const char* szData, unsigned short usDataSize);

        //! �� ������ �����͸� ������Ʈ�Ѵ�.
        void CellUpdateData(const char* szData, 
            unsigned short usDataSize, unsigned long dwCurrentPulse);

        //! ĳ���� ����Ʈ���� ĳ���͸� �����Ѵ�.
        void CellLogout(unsigned long dwCID);

        //! ���� Pulse�� �ٸ� Pulse�� ������ �ִ� ĳ���͸� ���� �����Ѵ�.
        void RemoveCharDataByPulse(unsigned long dwCurrentPulse, unsigned long dwExceptCID);

        //! ������ ĳ���� �����͸� ��� �´�.
        CCharacterData* GetCharacterData(unsigned long dwCID);
        CMonsterData*   GetMonsterData(unsigned long dwCID);
        
        void UpdatePos(unsigned long dwCID, const CNetworkPos& netPos);         //! ��ġ�� ������Ʈ�Ѵ�.
        void UpdateStallName(unsigned long dwCID, const char* szStallName = 0); //! ���� �̸��� ������Ʈ�Ѵ�.

        size_t GetCharacterNum() const { return m_CharacterDataMap.size(); }
        size_t GetMonsterNum() const { return m_MonsterDataMap.size(); }

        //! ��ȸ �޼���. ĳ���� �����͵� ��ü�� ��ȸ�Ѵ�.
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
        
        unsigned long       m_dwCID;            // ���� CID;               
        CharacterDataMap    m_CharacterDataMap;
        MonsterDataMap      m_MonsterDataMap;
    };
};

#endif
