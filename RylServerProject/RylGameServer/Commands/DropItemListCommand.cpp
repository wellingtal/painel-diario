#include "stdafx.h"
#include "Commands.h"
#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Map/FieldMap/Cell.h>
#include <Item/ItemFactory.h>


CConsoleCommand* CCMDDropItemList::Clone(const char* szCommand, size_t nCommandLength)
{
    const int MAX_BUFFER = 256;
    char szBuffer[MAX_BUFFER + 1];
	char szFileBuffer[MAX_BUFFER + 1];

    const char* szDelimiter = " \t\r\n";

    _snprintf(szBuffer, MAX_BUFFER, "%s", szCommand);
    szBuffer[MAX_BUFFER] = '\0';

    char* token = strtok(szBuffer, szDelimiter);

	char FileName[MAX_NAME + 1] = "";
    CCMDDropItemList* lpCMD = new CCMDDropItemList;
    if (NULL != lpCMD)
    {
        token = strtok(NULL, szDelimiter);        
        _snprintf(lpCMD->m_szName, MAX_NAME, "%s", token);
        lpCMD->m_szName[MAX_NAME] = '\0';

        token = strtok(NULL, szDelimiter);
		_snprintf(FileName, MAX_NAME, "%s", token);
        FileName[MAX_NAME] = '\0';

		lpCMD->m_nItemNum = 0;
		FILE *pFile = fopen(FileName, "rt");
		if (pFile != NULL)
		{
			while (true)
			{
				if (NULL == fgets(szFileBuffer, MAX_BUFFER, pFile) || lpCMD->m_nItemNum >= MAX_DROP_NUM)
				{
					break;
				}

				lpCMD->m_usItemProtoTypeID[lpCMD->m_nItemNum++] = atoi(szFileBuffer);
			}

			fclose(pFile);
		}
	}

    return lpCMD;
}



bool CCMDDropItemList::DoProcess()
{
    CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_szName);
    if (NULL != lpCharacter)
    {
		for (unsigned short nCount = 0; nCount < m_nItemNum; ++nCount)
		{
			Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(m_usItemProtoTypeID[nCount]);
			if (NULL != lpItem)
			{
				if (false == lpCharacter->GiveItem(lpItem))
				{
					DELETE_ITEM(lpItem);
					break;
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
    }

    return true;
}

