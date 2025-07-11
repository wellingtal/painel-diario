#include "stdafx.h"
#include "ClientCellMgr.h"
#include "BroadcastCharacterData.h"

#include <functional>
#include <utility>
#include <algorithm>

#include <Creature/CreatureStructure.h>
#include "GMMemory.h"

namespace Broadcast2nd
{

CClientCellMgr::CClientCellMgr()
:   m_dwCID(0)
{

}

CClientCellMgr::~CClientCellMgr()
{

}

void CClientCellMgr::Initialize(unsigned long dwCID)
{
    Clear();
    m_dwCID = dwCID;
}


void CClientCellMgr::Clear()
{
    m_CharacterDataMap.clear();
}




void CClientCellMgr::CellLogin(const char* szData, unsigned short usDataSize)
{
    int nDataSize = usDataSize;
    const char* szDataPos = szData; 
    unsigned long dwCID = 0;

    CCharacterData characterData;
    CMonsterData monsterData;
    CharacterDataMap::iterator char_pos;
    MonsterDataMap::iterator   mon_pos;

    for(; sizeof(unsigned long) < nDataSize; )
    {
        memcpy(&dwCID, szDataPos, sizeof(unsigned long));
        szDataPos += sizeof(unsigned long);
		nDataSize -= sizeof(unsigned long);

		int nBufferSize = nDataSize;

        switch(Creature::GetCreatureType(dwCID))
        {
        case Creature::CT_PC:

            characterData.ClearData();
            if(characterData.UpdateData(dwCID, 0, szDataPos, nBufferSize))
            {
                char_pos = m_CharacterDataMap.lower_bound(dwCID);
                if(char_pos != m_CharacterDataMap.end() && char_pos->first == dwCID)
                {
                    // 이미 캐릭터가 있을 경우, 데이터를 갱신한다.
                    char_pos->second = characterData; 
                }
                else
                {
                    // 캐릭터가 없을 경우, 신규로 삽입한다.
                    m_CharacterDataMap.insert(char_pos, std::make_pair(dwCID, characterData));
                }
            }
            break;

        case Creature::CT_MONSTER:
        case Creature::CT_SUMMON:
        case Creature::CT_STRUCT:

            monsterData.ClearData();
            if (monsterData.UpdateData(dwCID, 0, szDataPos, nBufferSize))
            {
                mon_pos = m_MonsterDataMap.lower_bound(dwCID);
                if (mon_pos != m_MonsterDataMap.end() && mon_pos->first == dwCID)
                {
                    mon_pos->second = monsterData;
                }
                else
                {
                    m_MonsterDataMap.insert(mon_pos, std::make_pair(dwCID, monsterData));
                }
            }
            break;
        }

        // 한마리라도 업데이트 실패하면, 버퍼가 왕창 깨졌을수도 있으므로
        // 그냥 Pass한다.
        szDataPos += nBufferSize;
        nDataSize -= nBufferSize;
    }
}


void CClientCellMgr::CellUpdateData(const char* szData, unsigned short usDataSize,
                                                  unsigned long dwCurrentPulse)
{
    int nDataSize = usDataSize;
    const char* szDataPos = szData; 
    unsigned long dwCID = 0;
    unsigned long dwUpdateData = 0;
    
    CharacterDataMap::iterator char_pos;    
    MonsterDataMap::iterator   mon_pos;
    
    for(; sizeof(unsigned long) < nDataSize; )
    {
        // CID를 복사하고, CID길이를 일단 제거한다.
        memcpy(&dwCID, szDataPos, sizeof(unsigned long));
        szDataPos += sizeof(unsigned long);
		nDataSize -= sizeof(unsigned long);

		int nBufferSize = nDataSize;

        switch(Creature::GetCreatureType(dwCID))
        {
        case Creature::CT_PC:

            char_pos = m_CharacterDataMap.find(dwCID);
            if(char_pos != m_CharacterDataMap.end())
            {
                CCharacterData& charData = char_pos->second;

                if(!charData.UpdateData(dwCID, 
                    dwCurrentPulse, szDataPos, nBufferSize))
                {                
                    break;
                }            
            }
            else
            {
                // 캐릭터가 없다!. 없는 캐릭터의 update가 오면 데이터를 버린다.
                memcpy(&dwUpdateData, szDataPos, sizeof(unsigned long));
                szDataPos += sizeof(unsigned long);
			    nDataSize -= sizeof(unsigned long);

                nBufferSize = CCharacterData::EstimateBufferSize(dwUpdateData);
            }
            break;

        case Creature::CT_MONSTER:
        case Creature::CT_SUMMON:
        case Creature::CT_STRUCT:

            mon_pos = m_MonsterDataMap.find(dwCID);
            if(mon_pos != m_MonsterDataMap.end())
            {
                CMonsterData& monData = mon_pos->second;

                if(!monData.UpdateData(dwCID, 
                    dwCurrentPulse, szDataPos, nBufferSize))
                {                
                    break;
                }            
            }
            else
            {
                // 몬스터가 없다!. 없는 몬스터의 update가 오면 데이터를 버린다.
                unsigned char cUpdateData = 
                    *reinterpret_cast<const unsigned char*>(szDataPos);
                
                szDataPos += sizeof(unsigned char);
			    nDataSize -= sizeof(unsigned char);

                nBufferSize = CMonsterData::EstimateBufferSize(cUpdateData);
            }
            break;
        }

        szDataPos += nBufferSize;
        nDataSize -= nBufferSize;
    }
}


void CClientCellMgr::CellLogout(unsigned long dwCID)
{
    switch(Creature::GetCreatureType(dwCID))
    {
    case Creature::CT_PC:
        m_CharacterDataMap.erase(dwCID);
        break;

    case Creature::CT_MONSTER:
    case Creature::CT_SUMMON:
    case Creature::CT_STRUCT:
        m_MonsterDataMap.erase(dwCID);
        break;
    }
}

void CClientCellMgr::RemoveCharDataByPulse(unsigned long dwCurrentPulse,
                                           unsigned long dwExceptCID)
{
    CharacterDataMap::iterator char_pos = m_CharacterDataMap.begin();
    CharacterDataMap::iterator char_end = m_CharacterDataMap.end();

    for (; char_pos != char_end ; )
    {
        CCharacterData& charData = char_pos->second;

        if (charData.GetLastUpdatedPulse() == dwCurrentPulse ||
            charData.GetCID() == dwExceptCID)
        {
            ++char_pos;
        }
        else
        {
            char_pos = m_CharacterDataMap.erase(char_pos);
        }
    }

    MonsterDataMap::iterator mon_pos = m_MonsterDataMap.begin();
    MonsterDataMap::iterator mon_end = m_MonsterDataMap.end();

    for (; mon_pos != mon_end; )
    {
        CMonsterData& monData = mon_pos->second;

        if (monData.GetLastUpdatedPulse() == dwCurrentPulse ||
            monData.GetCID() == dwExceptCID)
        {
            ++mon_pos;
        }
        else
        {
            mon_pos = m_MonsterDataMap.erase(mon_pos);
        }
    }
}

CCharacterData* CClientCellMgr::GetCharacterData(unsigned long dwCID)
{
    CharacterDataMap::iterator pos = m_CharacterDataMap.find(dwCID);    
    return (pos != m_CharacterDataMap.end()) ? &pos->second : 0;
}


void CClientCellMgr::UpdatePos(unsigned long dwCID, const CNetworkPos& netPos)
{
    switch(Creature::GetCreatureType(dwCID))
    {
    case Creature::CT_PC:

        {
            CharacterDataMap::iterator pos = m_CharacterDataMap.find(dwCID);
            if(pos != m_CharacterDataMap.end())
            {
                pos->second.SetNetworkPos(netPos);
            }
        }
        break;

    case Creature::CT_MONSTER:
    case Creature::CT_SUMMON:
    case Creature::CT_STRUCT:
        {
            MonsterDataMap::iterator pos = m_MonsterDataMap.find(dwCID);
            if(pos != m_MonsterDataMap.end())
            {
                pos->second.SetNetworkPos(netPos);
            }
        }
        break;
    }
}

void CClientCellMgr::UpdateStallName(unsigned long dwCID, const char* szStallName)
{
    if(0 == szStallName) 
    {
        szStallName = "";
    }

    CharacterDataMap::iterator pos = m_CharacterDataMap.find(dwCID);
    if(pos != m_CharacterDataMap.end())
    {
        pos->second.SetStallName(szStallName, strlen(szStallName) + 1);
    }
}

}