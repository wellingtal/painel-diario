#include "stdafx.h"
#include "Commands.h"
#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Map/FieldMap/Cell.h>
#include <Item/ItemFactory.h>


CConsoleCommand* CCMDDropItem::Clone(const char* szCommand, size_t nCommandLength)
{
    const int MAX_BUFFER = 256;
    char szBuffer[MAX_BUFFER + 1];

    const char* szDelimiter = " \t\r\n";

    _snprintf(szBuffer, MAX_BUFFER, "%s", szCommand);
    szBuffer[MAX_BUFFER] = '\0';

    char* token = strtok(szBuffer, szDelimiter);

    CCMDDropItem* lpCMD = new CCMDDropItem;
    if (NULL != lpCMD)
    {
        token = strtok(NULL, szDelimiter);        
        _snprintf(lpCMD->m_szName, MAX_NAME, "%s", token);
        lpCMD->m_szName[MAX_NAME] = '\0';

        token = strtok(NULL, szDelimiter);
        lpCMD->m_usItemProtoTypeID = (NULL == token) ? 0 : atoi(token);
    }

    return lpCMD;
}



bool CCMDDropItem::DoProcess()
{
    CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_szName);
    if (NULL != lpCharacter)
    {
		Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(m_usItemProtoTypeID);
		if (NULL != lpItem)
		{
			if (false == lpCharacter->GiveItem(lpItem))
			{
				DELETE_ITEM(lpItem);
			}

			// GievItem 으로 스택된 경우
			if (NULL != lpItem)
			{
				if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
				{
					DELETE_ITEM(lpItem);
				}
			}
		}
    }

    return true;
}

