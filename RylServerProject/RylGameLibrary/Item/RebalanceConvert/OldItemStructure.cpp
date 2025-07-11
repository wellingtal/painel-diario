#include "stdafx.h"

#include <Utility/Math/Math.h>
#include <Log/ServerLog.h>

#include <algorithm>

#include "OldItemStructure.h"

using namespace OldItem;

void OldItem::ItemData::DumpInfo(unsigned long dwCID, const char* szExtraString) const
{
    const int MIN_BUFFER = 32;
    char szItemUID[MIN_BUFFER];    

    Math::Convert::Hex64ToStr(szItemUID, m_dwUID);
    ERRLOG8(g_Log, "CID:0x%08x / ItemUID:%s / ProtoTypeID:%d / ItemPos(%d, 0x%04x) / ItemSize:%d / NumOrDurability:%d %s",
        dwCID, szItemUID, m_usProtoTypeID, m_ItemPos.m_cPos, m_ItemPos.m_cIndex, 
        m_cItemSize, m_cNumOrDurability, szExtraString);
}


OldItem::SpriteData::SpriteData() { Initialize(); }

void OldItem::SpriteData::Initialize()
{
    memset(m_szName, 0, MAX_NAME);
    memset(m_szSpriteName, 0, MAX_SPRITE_NAME);

	m_nSpriteMinX = m_nSpriteMinY = m_nSpriteMaxX = m_nSpriteMaxY = 0;
}

OldItem::StringData::StringData() { Initialize(); }
void OldItem::StringData::Initialize()
{
    memset(m_szFieldModelName, 0, MAX_FIELD_MODEL_NAME);
    memset(m_szEquipModelName, 0, MAX_EQUIP_MODEL_NAME);
    memset(m_szEffectSoundName, 0, MAX_EFFECT_SOUND_NAME);
	memset(m_szTypeName, 0, MAX_TYPE_NAME);
    memset(m_szItemDescribe, 0, MAX_ITEM_DESCRIBE);
}

OldItem::DetailData::DetailData() { Initialize(); }
void OldItem::DetailData::Initialize()
{
    m_ItemGrade = OldItem::Grade::ETC;
	m_dwFlags = m_dwPrice = m_dwBlackPrice = m_dwMedalPrice = m_dwCraftExp = 0;
	m_cOptionLimit = m_cItemType = m_cXSize = m_cYSize = 
		m_cDefaultDurabilityOrStack = m_cMaxDurabilityOrStack = m_cMaterialType = m_cMaxSocketNum = 0;        
}

OldItem::EquipAttribute::EquipAttribute() { Initialize(); }
void OldItem::EquipAttribute::Initialize()
{
    m_nAttibuteNum = 0;
    std::fill_n(m_usAttributeValue, unsigned short(OldItem::Attribute::MAX_ATTRIBUTE_NUM), 0);
}

OldItem::UseItemInfo::UseItemInfo() { Initialize(); }
void OldItem::UseItemInfo::Initialize() 
{ 
	m_usSkill_ID = 0;
	m_usSkill_LockCount = 0;
	m_cZone = 0;
	m_dwAmount = 0; 
}

               
OldItem::UseLimit::UseLimit() { Initialize(); }
void OldItem::UseLimit::Initialize()
{
    std::fill_n(m_nStatusLimit, unsigned short(StatusLimit::MAX_STATUS_LIMIT), 0);
	m_dwClassLimit = m_nSkillType = m_cSkillLevel = m_cLevelLimit = 0;
}


OldItem::ItemInfo::ItemInfo() { Initialize(); }
OldItem::ItemInfo::ItemInfo(unsigned short usProtoTypeID) { Initialize(); m_usProtoTypeID = usProtoTypeID; }

void OldItem::ItemInfo::Initialize()
{
    m_usProtoTypeID = 0;

    m_DetailData.Initialize();
    m_UseLimit.Initialize();
    m_SpriteData.Initialize();
    m_StringData.Initialize();

    m_EquipAttribute.Initialize();
    m_UseItemInfo.Initialize();   
}


/*
const char* OldItem::ItemInfo::GetItemDescribe(void) const
{
	if (OldItem::ItemType::SKILLBOOK == m_DetailData.m_cItemType)
	{
		const Skill::ProtoType* pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(m_UseItemInfo.m_usSkill_ID);
		if (NULL != pSkillProtoType)
		{
			return pSkillProtoType[m_UseItemInfo.m_usSkill_LockCount].m_szSkillDescribe;
		}
	}

	return m_StringData.m_szItemDescribe;
}
*/

OldItem::ItemGarbage::ItemGarbage(CItem* lpItem, unsigned long dwRemainNum)
:	m_lpItem(lpItem), m_dwRemainNum(dwRemainNum)
{

}

OldItem::ChemicalInfo::ChemicalInfo()
:	m_wPickkingItemID(0), m_wTargetItemID(0), m_wResultItemID(0), 
	m_cPickkingItemNum(0), m_cTargetItemNum(0), m_cResultItemNum(0)
{

}

OldItem::ChemicalInfo::ChemicalInfo(unsigned short wPickkingItemID, unsigned char cPickkingItemNum, 
						 unsigned short wTargetItemID, unsigned char cTargetItemNum)
:	m_wPickkingItemID(wPickkingItemID), m_wTargetItemID(wTargetItemID), m_wResultItemID(0), 
	m_cPickkingItemNum(cPickkingItemNum),m_cTargetItemNum(cTargetItemNum), m_cResultItemNum(0)
{

}
