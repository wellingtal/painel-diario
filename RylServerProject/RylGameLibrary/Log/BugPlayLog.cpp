#include "stdafx.h"
#include "BugPlayLog.h"

#include <Item/Item.h>
#include <Log/GameLog.h>

#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>

bool GAMELOG::LogDamagePumping(CCharacter& AttackCharacter_In, CMonster& DefendMonster_In, 
                               const short nDamage_In, bool bWriteForce_In)
{
	// 100레벨 부터 이해 못해-_-
	const static short nBugDamage[Creature::PC_MAX_LEVEL] = 
    {
		85,		109,	133,	159,	172,	200,	226,	256,	271,	307,	
		337,	373,	392,	427,	460,	501,	522,	561,	596,	642,	
		666,	709,	747,	798,	825,	871,	912,	967,	997,	1047,	
		1091,	1151,	1183,	1237,	1283,	1348,	1383,	1441,	1490,	1560,	
		1598,	1659,	1711,	1785,	1826,	1891,	1946,	2025,	2068,	2137,	
		2194,	2278,	2324,	2397,	2457,	2546,	2595,	2671,	2734,	2827,	
		2879,	2959,	3025,	3123,	3177,	3261,	3329,	3432,	3489,	3577,	
		3648,	3756,	3816,	3907,	3981,	4093,	4156,	4251,	4328,	4445,	
		4510,	4609,	4688,	4810,	4878,	4981,	5063,	5190,	5261,	5367,	
		5452,	5583,	5657,	5767,	5855,	5991,	6067,	6181,	6271,	6362
	};

	CreatureStatus&		creatureStatus	= AttackCharacter_In.GetStatus();
	CharacterStatus&	characterStatus	= AttackCharacter_In.GetCharStatus();
	FightStatus&		statusInfo		= creatureStatus.m_StatusInfo;

    CreatureStatus&     monsterStatus   = DefendMonster_In.GetStatus();

    const int MAX_BUFFER = 128 * 1024;
    char    szBuffer[MAX_BUFFER];
    int     nLength = 0;

	if (!bWriteForce_In)
	{
		if (monsterStatus.m_nLevel < creatureStatus.m_nLevel)
        {
            return true;        
        }

		if (nDamage_In < nBugDamage[creatureStatus.m_nLevel-1])
        {
            return true;
        }
	}

    nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "데미지 펌핑으로 추측되는 상황이 발생하여 로그를 출력합니다.                                 \r\n");

	nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "============================================================================================\r\n");

	nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "CID : 0x%08x, 이름 : %s, 레벨 : %d, 클래스 : %d                                             \r\n", 
		AttackCharacter_In.GetCID(), AttackCharacter_In.GetCharacterName(), 
        creatureStatus.m_nLevel, AttackCharacter_In.GetClass());
	
    nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "STR : %d, DEX : %d, CON : %d, INT : %d, WIS : %d                                            \r\n", 
        characterStatus.m_nSTR, characterStatus.m_nDEX, characterStatus.m_nCON, characterStatus.m_nINT, characterStatus.m_nWIS);
	
    nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "MinDamage : %d, MaxDamage : %d, HitRate : %d, Critical : %d   \r\n", 
		statusInfo.m_lMinDamage, statusInfo.m_lMaxDamage, 
        statusInfo.m_wHitRate, statusInfo.m_wCritical);
	
    nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "Armor : %d, Evade : %d, MagicResist : %d, Block : %d           \r\n", 
		statusInfo.m_wArmor, statusInfo.m_wEvade, 
        statusInfo.m_wMagicResist, statusInfo.m_wBlock);

	nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "--------------------------------------------------------------------------------------------\r\n");

	for (int nIndex = 0; nIndex < Item::EquipmentPos::MAX_EQUPMENT_POS; ++nIndex)
	{
        Item::ItemPos itemPos(TakeType::TS_EQUIP, nIndex);
        Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(AttackCharacter_In.GetItem(itemPos));

		if (NULL == lpEquipment)
        {
            continue;
        }

		nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
            "- Equip #%d -                                                                               \r\n", nIndex);

		nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
            "ProtoTypeID : %d, Durability : %d / %d                                                      \r\n", 
            lpEquipment->GetPrototypeID(), lpEquipment->GetNumOrDurability(), lpEquipment->GetMaxNumOrDurability());

        short usAttributes[Item::Attribute::MAX_ATTRIBUTE_NUM];
        lpEquipment->GetAttribute(usAttributes, Item::Attribute::MAX_ATTRIBUTE_NUM);

        for (int nAttributeCount = 0; nAttributeCount < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nAttributeCount)
		{
            if(0 == usAttributes[nAttributeCount])
            {
                continue;
            }
       
            #define CaseLogValue(AttributeType) \
                case AttributeType: nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, "%15s %2d\r\n",\
                    #AttributeType, usAttributes[nAttributeCount]);

            using namespace Item::Attribute;

			switch (nAttributeCount) 
			{			
                CaseLogValue(MIN_DAMAGE)	break;
				CaseLogValue(MAX_DAMAGE)	break;			
				CaseLogValue(ARMOR)         break;			
				CaseLogValue(HIT_RATE)		break;					
				CaseLogValue(EVADE)			break;		
				CaseLogValue(MAGIC_POWER)	break;					
				CaseLogValue(MAGIC_RESIST)	break;			
				CaseLogValue(MAX_HP)		break;			
				CaseLogValue(HP_REGEN)		break;				
				CaseLogValue(MAX_MP)        break;			
				CaseLogValue(MP_REGEN)      break;		
				CaseLogValue(BLOCK)			break;			
				CaseLogValue(CRITICAL)		break;			
			}
		}
	}

	nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "--------------------------------------------------------------------------------------------\r\n");

	nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "Damage : %d, MonsterCID : 0x%08x, MonsterLevel : %d                                         \r\n", 
		nDamage_In, DefendMonster_In.GetCID(), monsterStatus.m_nLevel);

	nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, 
        "============================================================================================\r\n");

    ERRLOG0(g_Log, szBuffer);
	return false;
}

