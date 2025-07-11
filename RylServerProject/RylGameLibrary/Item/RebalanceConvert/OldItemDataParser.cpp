
#include "stdafx.h"

#include <Utility/Math/Math.h>

// 클라이언트에서도 쓰이므로 include를 명시한다.
#include <Log/ServerLog.h>

#include "OldItemStructure.h"
#include "OldItemDataParser.h"


using namespace OldItem;

namespace OldItemDataParser
{
    bool ReadString(char* szBuffer_Out, int nBufferLen, const char* szValue_In)
    {
        int nLength = _snprintf(szBuffer_Out, nBufferLen - 1, "%s", szValue_In);
        
        if (0 <= nLength)
        {
            szBuffer_Out[nLength] = '\0';
            return true;
        }

        szBuffer_Out[nBufferLen - 1] = '\0';
        ERRLOG1(g_Log, "버퍼 길이가 모자라서, 데이터를 전부 복사할 수 없습니다. Data:%s", szValue_In);
        return false;    
    }

    inline bool ReadValue(unsigned char& cValue_Out, const char* szValue_In)
    {
        cValue_Out = static_cast<unsigned char>(atoi(szValue_In));
        return true;
    }

    inline bool ReadValue(unsigned short& usValue_Out, const char* szValue_In)
    {
        usValue_Out = static_cast<unsigned short>(atoi(szValue_In));
        return true;
    }

    inline bool ReadValue(unsigned long& dwValue_Out, const char* szValue_In)
    {
        dwValue_Out = static_cast<unsigned long>(atol(szValue_In));
        return true;
    }

    inline bool ReadValue(float& fValue_Out, const char* szValue_In)
    {
	    fValue_Out = static_cast<float>(atof(szValue_In));
	    return true;
    }


    bool ReadStringToTypeValue(unsigned char& cVaule_Out, const CTypeName* lpTypeArray, 
                            const unsigned char nMaxType, const char* szValue_In)
    {   
        for (unsigned char nCount = 0; nCount < nMaxType; ++nCount)
        {
	        if (0 == strcmp(szValue_In, lpTypeArray[nCount].GetTypeName())) 
            {
                cVaule_Out = lpTypeArray[nCount].GetTypeValue();
                return true;
            }
        }

        return false;
    }



    // 기본 데이터들
    bool ReadID(OldItem::ItemInfo& itemInfo, const char* szValue)
    {
        itemInfo.m_usProtoTypeID = static_cast<unsigned short>(atoi(szValue));
        if (0 == itemInfo.m_usProtoTypeID || 0xFFFF == itemInfo.m_usProtoTypeID)
        {
            ERRLOG1(g_Log, "아이템 ID가 이상합니다. 0이나 0xFFFF는 사용하실 수 없습니다. ID:0x%08x",
                itemInfo.m_usProtoTypeID);

            return false;
        }

        return true;
    }

    bool ReadItemName(OldItem::ItemInfo& itemInfo, const char* szValue)            { return ReadString(itemInfo.m_SpriteData.m_szName, SpriteData::MAX_NAME, szValue); }
    bool ReadFieldModelName(OldItem::ItemInfo& itemInfo, const char* szValue)      { return ReadString(itemInfo.m_StringData.m_szFieldModelName, StringData::MAX_FIELD_MODEL_NAME, szValue); }
    bool ReadAttachedModelName(OldItem::ItemInfo& itemInfo, const char* szValue)   { return ReadString(itemInfo.m_StringData.m_szEquipModelName, StringData::MAX_EQUIP_MODEL_NAME, szValue); }
    bool ReadSpriteDDS(OldItem::ItemInfo& itemInfo, const char* szValue)           { return ReadString(itemInfo.m_SpriteData.m_szSpriteName, SpriteData::MAX_SPRITE_NAME, szValue); }
    bool ReadMinX(OldItem::ItemInfo& itemInfo, const char* szValue)                { return ReadValue(itemInfo.m_SpriteData.m_nSpriteMinX, szValue); }
    bool ReadMinY(OldItem::ItemInfo& itemInfo, const char* szValue)                { return ReadValue(itemInfo.m_SpriteData.m_nSpriteMinY, szValue); }
    bool ReadMaxX(OldItem::ItemInfo& itemInfo, const char* szValue)                { return ReadValue(itemInfo.m_SpriteData.m_nSpriteMaxX, szValue); }
    bool ReadMaxY(OldItem::ItemInfo& itemInfo, const char* szValue)                { return ReadValue(itemInfo.m_SpriteData.m_nSpriteMaxY, szValue); }
    bool ReadSizeX(OldItem::ItemInfo& itemInfo, const char* szValue)               { return ReadValue(itemInfo.m_DetailData.m_cXSize, szValue); }
    bool ReadSizeY(OldItem::ItemInfo& itemInfo, const char* szValue)               { return ReadValue(itemInfo.m_DetailData.m_cYSize, szValue); }
    bool ReadEffectSound(OldItem::ItemInfo& itemInfo, const char* szValue)         { return ReadString(itemInfo.m_StringData.m_szEffectSoundName, StringData::MAX_EFFECT_SOUND_NAME, szValue); }

    bool ReadItemLevel(OldItem::ItemInfo& itemInfo, const char* szValue)
    {
        unsigned char cGrade = 0;
        if (ReadStringToTypeValue(cGrade, OldItem::Grade::Grades, OldItem::Grade::MAX_ITEM_GRADE, szValue))
        {
            itemInfo.m_DetailData.m_ItemGrade = static_cast<OldItem::Grade::Type>(cGrade);
            return true;
        }

        return false;
    }

    bool ReadItemType(OldItem::ItemInfo& itemInfo, const char* szValue)
    {
        return ReadStringToTypeValue(itemInfo.m_DetailData.m_cItemType, 
            OldItem::CItemType::GetInstance().GetItemTypeName(), OldItem::ItemType::MAX_ITEM_TYPE, szValue);
    }

    bool ReadTypeName(OldItem::ItemInfo& itemInfo, const char* szValue)        { return ReadString(itemInfo.m_StringData.m_szTypeName, StringData::MAX_TYPE_NAME, szValue); }
    bool ReadPrice(OldItem::ItemInfo& itemInfo, const char* szValue)           { return ReadValue(itemInfo.m_DetailData.m_dwPrice, szValue); }
    bool ReadBlackPrice(OldItem::ItemInfo& itemInfo, const char* szValue)      { return ReadValue(itemInfo.m_DetailData.m_dwBlackPrice, szValue); }
    bool ReadMedalPrice(OldItem::ItemInfo& itemInfo, const char* szValue)      { return ReadValue(itemInfo.m_DetailData.m_dwMedalPrice, szValue); }
    bool ReadOptionLimit(OldItem::ItemInfo& itemInfo, const char* szValue)     { return ReadValue(itemInfo.m_DetailData.m_cOptionLimit, szValue); }
    bool ReadDurability(OldItem::ItemInfo& itemInfo, const char* szValue)      { return ReadValue(itemInfo.m_DetailData.m_cDefaultDurabilityOrStack, szValue); }
    bool ReadMaxDurability(OldItem::ItemInfo& itemInfo, const char* szValue)   { return ReadValue(itemInfo.m_DetailData.m_cMaxDurabilityOrStack, szValue); }
    bool ReadClassLimit(OldItem::ItemInfo& itemInfo, const char* szValue)      { return ReadValue(itemInfo.m_UseLimit.m_dwClassLimit, szValue); }
    bool ReadSkillLimitType(OldItem::ItemInfo& itemInfo, const char* szValue)  { return ReadValue(itemInfo.m_UseLimit.m_nSkillType, szValue); }    
    bool ReadSkillLimitLevel(OldItem::ItemInfo& itemInfo, const char* szValue) { return ReadValue(itemInfo.m_UseLimit.m_cSkillLevel, szValue); }
    bool ReadLevelLimit(OldItem::ItemInfo& itemInfo, const char* szValue)      { return ReadValue(itemInfo.m_UseLimit.m_cLevelLimit, szValue); }
    bool ReadSTRLimit(OldItem::ItemInfo& itemInfo, const char* szValue)        { return ReadValue(itemInfo.m_UseLimit.m_nStatusLimit[StatusLimit::STR], szValue); }
    bool ReadDEXLimit(OldItem::ItemInfo& itemInfo, const char* szValue)        { return ReadValue(itemInfo.m_UseLimit.m_nStatusLimit[StatusLimit::DEX], szValue); }
    bool ReadCONLimit(OldItem::ItemInfo& itemInfo, const char* szValue)        { return ReadValue(itemInfo.m_UseLimit.m_nStatusLimit[StatusLimit::CON], szValue); }
    bool ReadINTLimit(OldItem::ItemInfo& itemInfo, const char* szValue)        { return ReadValue(itemInfo.m_UseLimit.m_nStatusLimit[StatusLimit::INT], szValue); }
    bool ReadWISLimit(OldItem::ItemInfo& itemInfo, const char* szValue)        { return ReadValue(itemInfo.m_UseLimit.m_nStatusLimit[StatusLimit::WIS], szValue); }
    bool ReadCraftExp(OldItem::ItemInfo& itemInfo, const char* szValue)		{ return ReadValue(itemInfo.m_DetailData.m_dwCraftExp, szValue); }
}

bool OldItemDataParser::SetDefaultData(ParseDataArray& parseDataArray)
{
    parseDataArray.reserve(40);

    parseDataArray.push_back(ParseData("ID",                    ReadID));
    parseDataArray.push_back(ParseData("ItemName",              ReadItemName));
    parseDataArray.push_back(ParseData("FieldModelName",        ReadFieldModelName));
    parseDataArray.push_back(ParseData("AttachedModelName", 	ReadAttachedModelName));
    parseDataArray.push_back(ParseData("SpriteDDS",             ReadSpriteDDS));
    parseDataArray.push_back(ParseData("MinX",                  ReadMinX));
    parseDataArray.push_back(ParseData("MinY",                  ReadMinY));
    parseDataArray.push_back(ParseData("MaxX",                  ReadMaxX));
    parseDataArray.push_back(ParseData("MaxY",                  ReadMaxY));
    parseDataArray.push_back(ParseData("SizeX",                 ReadSizeX));
    parseDataArray.push_back(ParseData("SizeY",                 ReadSizeY));
    parseDataArray.push_back(ParseData("EffectSound",           ReadEffectSound));
    parseDataArray.push_back(ParseData("ItemLevel",             ReadItemLevel));
    parseDataArray.push_back(ParseData("ItemType",              ReadItemType));
	parseDataArray.push_back(ParseData("TypeName",              ReadTypeName));
    parseDataArray.push_back(ParseData("Price",                 ReadPrice));
    parseDataArray.push_back(ParseData("BlackPrice",            ReadBlackPrice));
	parseDataArray.push_back(ParseData("MedalPrice",            ReadMedalPrice));
    parseDataArray.push_back(ParseData("OptionLimit",           ReadOptionLimit));
    parseDataArray.push_back(ParseData("Durability",            ReadDurability));
    parseDataArray.push_back(ParseData("MaxDurability",         ReadMaxDurability));
    parseDataArray.push_back(ParseData("ClassLimit",            ReadClassLimit));
    parseDataArray.push_back(ParseData("SkillLimitType",        ReadSkillLimitType));
    parseDataArray.push_back(ParseData("SkillLimitLevel",       ReadSkillLimitLevel));
    parseDataArray.push_back(ParseData("LevelLimit",            ReadLevelLimit));
    parseDataArray.push_back(ParseData("STRLimit",              ReadSTRLimit));
    parseDataArray.push_back(ParseData("DEXLimit",              ReadDEXLimit));
    parseDataArray.push_back(ParseData("CONLimit",              ReadCONLimit));
    parseDataArray.push_back(ParseData("INTLimit",              ReadINTLimit));
    parseDataArray.push_back(ParseData("WISLimit",              ReadWISLimit));
	parseDataArray.push_back(ParseData("CraftExp",				ReadCraftExp));

	return true;
}

namespace OldItemDataParser
{
    bool ReadCriticalType(OldItem::ItemInfo& itemInfo, const char* szValue)        { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::CRITICAL_TYPE], szValue);          }
    bool ReadCriticalPercentage(OldItem::ItemInfo& itemInfo, const char* szValue)  { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::CRITICAL_PERCENTAGE], szValue);    }
    bool ReadMinDamage(OldItem::ItemInfo& itemInfo, const char* szValue)           { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::MIN_DAMAGE], szValue);             }
    bool ReadMaxDamage(OldItem::ItemInfo& itemInfo, const char* szValue)           { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::MAX_DAMAGE], szValue);             }
    bool ReadDRC(OldItem::ItemInfo& itemInfo, const char* szValue)                 { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::DRC], szValue);                    }
    bool ReadOffenceRevision(OldItem::ItemInfo& itemInfo, const char* szValue)     { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::OFFENCE_REVISION], szValue);       }
    bool ReadDefence(OldItem::ItemInfo& itemInfo, const char* szValue)             { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::DEFENCE], szValue);                }
    bool ReadDefenceRevision(OldItem::ItemInfo& itemInfo, const char* szValue)     { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::DEFENCE_REVISION], szValue);       }
    bool ReadResistance(OldItem::ItemInfo& itemInfo, const char* szValue)          { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::MAGIC_RESISTANCE], szValue);       }
    bool ReadBlockingPercentage(OldItem::ItemInfo& itemInfo, const char* szValue)  { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::BLOCKING_PERCENTAGE], szValue);    }
    bool ReadAttackSpeed(OldItem::ItemInfo& itemInfo, const char* szValue)         { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::ATTACK_SPEED], szValue);           }
    bool ReadMoveSpeed(OldItem::ItemInfo& itemInfo, const char* szValue)           { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::MOVE_SPEED], szValue);             }
    bool ReadAttackRange(OldItem::ItemInfo& itemInfo, const char* szValue)         { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::ATTACK_RANGE], szValue);           }
    bool ReadRangedAttack(OldItem::ItemInfo& itemInfo, const char* szValue)        { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::RANGE_ATTACK_DISTANCE], szValue);  }
    bool ReadHpMaxPlus(OldItem::ItemInfo& itemInfo, const char* szValue)           { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::MAX_HP], szValue);                 }
    bool ReadMpMaxPlus(OldItem::ItemInfo& itemInfo, const char* szValue)           { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::MAX_MP], szValue);                 }
    bool ReadHPRegen(OldItem::ItemInfo& itemInfo, const char* szValue)             { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::HP_REGEN_AMOUNT], szValue);        }
    bool ReadMPRegen(OldItem::ItemInfo& itemInfo, const char* szValue)             { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::MP_REGEN_AMOUNT], szValue);        }
    bool ReadFireAttribute(OldItem::ItemInfo& itemInfo, const char* szValue)       { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::FIRE_ATTACK], szValue);            }
    bool ReadLightingAttribute(OldItem::ItemInfo& itemInfo, const char* szValue)   { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::LIGHTNING_ATTACK], szValue);       }
    bool ReadColdAttribute(OldItem::ItemInfo& itemInfo, const char* szValue)       { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::COLD_ATTACK], szValue);            }
    bool ReadDrainAttribute(OldItem::ItemInfo& itemInfo, const char* szValue)      { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::DRAIN_ATTACK], szValue);           }
    bool ReadPoisonAttribute(OldItem::ItemInfo& itemInfo, const char* szValue)     { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::POISON_ATTACK], szValue);          }
    bool ReadFireResistance(OldItem::ItemInfo& itemInfo, const char* szValue)      { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::FIRE_RESISTANCE], szValue);        }
    bool ReadLightningResistance(OldItem::ItemInfo& itemInfo, const char* szValue) { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::LIGHTNING_RESISTANCE], szValue);   }
    bool ReadColdResistance(OldItem::ItemInfo& itemInfo, const char* szValue)      { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::COLD_RESISTANCE], szValue);        }
    bool ReadDrainResistance(OldItem::ItemInfo& itemInfo, const char* szValue)     { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::DRAIN_RESISTANCE], szValue);       }
    bool ReadPoisonResistance(OldItem::ItemInfo& itemInfo, const char* szValue)    { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::POISON_RESISTANCE], szValue);      }
    bool ReadSTRAdd(OldItem::ItemInfo& itemInfo, const char* szValue)              { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::ADD_STR], szValue);                }
    bool ReadDEXAdd(OldItem::ItemInfo& itemInfo, const char* szValue)              { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::ADD_DEX], szValue);                }
    bool ReadCONAdd(OldItem::ItemInfo& itemInfo, const char* szValue)              { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::ADD_CON], szValue);                }
    bool ReadINTAdd(OldItem::ItemInfo& itemInfo, const char* szValue)              { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::ADD_INT], szValue);                }
    bool ReadWISAdd(OldItem::ItemInfo& itemInfo, const char* szValue)              { ++itemInfo.m_EquipAttribute.m_nAttibuteNum; return ReadValue(itemInfo.m_EquipAttribute.m_usAttributeValue[OldItem::Attribute::ADD_WIS], szValue);                }
    bool ReadMaterialValue(OldItem::ItemInfo& itemInfo, const char* szValue)       { return ReadValue(itemInfo.m_DetailData.m_cMaterialType, szValue); }
    bool ReadMaxSocketNum(OldItem::ItemInfo& itemInfo, const char* szValue)        { return ReadValue(itemInfo.m_DetailData.m_cMaxSocketNum, szValue); }
}

bool OldItemDataParser::SetEquipData(ParseDataArray& parseDataArray)
{
    parseDataArray.push_back(ParseData("CriticalType",          ReadCriticalType));
    parseDataArray.push_back(ParseData("CriticalPercentage",    ReadCriticalPercentage));
    parseDataArray.push_back(ParseData("MinDamage",             ReadMinDamage));
    parseDataArray.push_back(ParseData("MaxDamage",             ReadMaxDamage));
    parseDataArray.push_back(ParseData("DRC",                   ReadDRC));
    parseDataArray.push_back(ParseData("OffenceRevision",       ReadOffenceRevision));
    parseDataArray.push_back(ParseData("Defence",               ReadDefence));
    parseDataArray.push_back(ParseData("DefenceRevision",       ReadDefenceRevision));
    parseDataArray.push_back(ParseData("Resistance",            ReadResistance));
    parseDataArray.push_back(ParseData("BlockingPercentage",    ReadBlockingPercentage));
    parseDataArray.push_back(ParseData("AttackSpeed",           ReadAttackSpeed));
    parseDataArray.push_back(ParseData("MoveSpeed",             ReadMoveSpeed));
    parseDataArray.push_back(ParseData("AttackRange",           ReadAttackRange));
    parseDataArray.push_back(ParseData("LongRangeAttack",       ReadRangedAttack));
    parseDataArray.push_back(ParseData("HpMaxPlus",             ReadHpMaxPlus));
    parseDataArray.push_back(ParseData("MpMaxPlus",             ReadMpMaxPlus));
    parseDataArray.push_back(ParseData("HPRegen",               ReadHPRegen));
    parseDataArray.push_back(ParseData("MPRegen",               ReadMPRegen));
    parseDataArray.push_back(ParseData("FireAttribute",         ReadFireAttribute));
    parseDataArray.push_back(ParseData("LightingAttribute",     ReadLightingAttribute));
    parseDataArray.push_back(ParseData("ColdAttribute",         ReadColdAttribute));
    parseDataArray.push_back(ParseData("DrainAttribute",        ReadDrainAttribute));
    parseDataArray.push_back(ParseData("PoisonAttribute",       ReadPoisonAttribute));
    parseDataArray.push_back(ParseData("FireResistance",        ReadFireResistance));
    parseDataArray.push_back(ParseData("LightningResistance",   ReadLightningResistance));
    parseDataArray.push_back(ParseData("ColdResistance",        ReadColdResistance));
    parseDataArray.push_back(ParseData("DrainResistance",       ReadDrainResistance));
    parseDataArray.push_back(ParseData("PoisonResistance",      ReadPoisonResistance));
    parseDataArray.push_back(ParseData("STRAdd",                ReadSTRAdd));
    parseDataArray.push_back(ParseData("DEXAdd",                ReadDEXAdd));
    parseDataArray.push_back(ParseData("CONAdd",                ReadCONAdd));
    parseDataArray.push_back(ParseData("INTAdd",                ReadINTAdd));
    parseDataArray.push_back(ParseData("WISAdd",                ReadWISAdd));

    parseDataArray.push_back(ParseData("MaterialValue",         ReadMaterialValue));
    parseDataArray.push_back(ParseData("MaxSocketNum",          ReadMaxSocketNum));

    return true;
}


namespace OldItemDataParser
{
    bool ReadSkill_ID(OldItem::ItemInfo& itemInfo, const char* szValue)        { itemInfo.m_UseItemInfo.m_usSkill_ID = Math::Convert::Atos(szValue); return true; }
    bool ReadSkill_Level(OldItem::ItemInfo& itemInfo, const char* szValue)     { return ReadValue(itemInfo.m_UseItemInfo.m_usSkill_LockCount, szValue); }
    bool ReadZone(OldItem::ItemInfo& itemInfo, const char* szValue)			{ return ReadValue(itemInfo.m_UseItemInfo.m_cZone, szValue); }
    bool ReadPositionX(OldItem::ItemInfo& itemInfo, const char* szValue)		{ return ReadValue(itemInfo.m_UseItemInfo.m_Pos.m_fPointX, szValue); }
    bool ReadPositionY(OldItem::ItemInfo& itemInfo, const char* szValue)		{ return ReadValue(itemInfo.m_UseItemInfo.m_Pos.m_fPointY, szValue); }
    bool ReadPositionZ(OldItem::ItemInfo& itemInfo, const char* szValue)		{ return ReadValue(itemInfo.m_UseItemInfo.m_Pos.m_fPointZ, szValue); }
    bool ReadAmount(OldItem::ItemInfo& itemInfo, const char* szValue)			{ return ReadValue(itemInfo.m_UseItemInfo.m_dwAmount, szValue); }
    bool ReadItemDescribe(OldItem::ItemInfo& itemInfo, const char* szValue)    { return ReadString(itemInfo.m_StringData.m_szItemDescribe, OldItem::StringData::MAX_ITEM_DESCRIBE, szValue); }
}

bool OldItemDataParser::SetUseItemData(ParseDataArray& parseDataArray)
{
    parseDataArray.push_back(ParseData("CriticalType",			ReadSkill_ID));
    parseDataArray.push_back(ParseData("CriticalPercentage",	ReadSkill_Level));
	parseDataArray.push_back(ParseData("MinDamage",				ReadZone));
	parseDataArray.push_back(ParseData("MaxDamage",				ReadPositionX));
	parseDataArray.push_back(ParseData("DRC",					ReadPositionY));
	parseDataArray.push_back(ParseData("OffenceRevision",		ReadPositionZ));
	parseDataArray.push_back(ParseData("Defence",				ReadAmount));
    parseDataArray.push_back(ParseData("DefenceRevision",		ReadItemDescribe));
	return true;
}


bool OldItemDataParser::SetEtcItemData(ParseDataArray& parseDataArray)
{
    parseDataArray.push_back(ParseData("CriticalType",          ReadItemDescribe));
    return true;
}


