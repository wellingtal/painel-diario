
#include "stdafx.h"

#include "TempCharacter.h"


CTempCharacter::CTempCharacter()  
:	m_dwUID(0), m_dwCID(0), m_nDataRequestCount(0), m_cGroup(-1), m_cFlag(0)
{
    memset(&m_szCharacterName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);

    memset(&m_CharInfoEX, 0, sizeof(CHAR_INFOEX));
    memset(&m_Quest, 0, sizeof(QUEST));
    memset(&m_History, 0, sizeof(HISTORY));
    memset(&m_Config, 0, sizeof(CONFIG));
    memset(&m_StoreInfo, 0, sizeof(STORE_INFO));
}

CTempCharacter::~CTempCharacter()
{
		
}

CTempCharacterMgr::~CTempCharacterMgr()
{
    isMapCharList::iterator	pos = m_mapTempChar.begin();
    isMapCharList::iterator	end = m_mapTempChar.end();

    for(;pos != end; ++pos)
    {
        m_tempCharPool.destroy(pos->second);
    }

    m_mapTempChar.clear();
}


// CID/Group을 키로 하고, 일치하는 클래스가 있으면 가져온다. 없으면 생성한 후 리턴한다.
CTempCharacter*	CTempCharacterMgr::GetCharacter(unsigned long dwBattleCID, unsigned char cGroup)
{
    std::pair<isMapCharList::iterator, isMapCharList::iterator>	
        result = m_mapTempChar.equal_range(dwBattleCID);

    CTempCharacter* lpCharacter = 0;

    for(; result.first != result.second; ++result.first)
    {
        lpCharacter = result.first->second;

        if(cGroup == lpCharacter->GetGroup())
        {
            return lpCharacter;
        }
    }

    // 못찾았다. 하나 삽입한다.
    lpCharacter = m_tempCharPool.construct();

    if(0 != lpCharacter)
    {
        lpCharacter->SetGroup(cGroup);
        m_mapTempChar.insert(result.first, std::make_pair(dwBattleCID, lpCharacter));
    }

    return lpCharacter;
}

// 캐릭터 로그아웃시 호출한다.
bool CTempCharacterMgr::EraseChar(unsigned long dwBattleCID)
{
    std::pair<isMapCharList::iterator, isMapCharList::iterator>	
        result = m_mapTempChar.equal_range(dwBattleCID);

    for(; result.first != result.second;)
    {
        m_tempCharPool.destroy(result.first->second);
        m_mapTempChar.erase(result.first++);
    }

    return true;
}
