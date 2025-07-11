#include "stdafx.h"

#include <Item/Item.h>
#include <Skill/SkillMgr.h>

#include "Character.h"

void CCharacter::UpdateQuickSlotSkill(SKILLSLOT Slot)
{
	for (int nQSlot = 0; nQSlot < QUICK::MAX_QUICK_NUM; ++nQSlot)
	{
        QUICKSLOT& QuickSlot = m_DBData.m_Quick.Slots[nQSlot];

		if (QUICKSLOT::SKILL == QuickSlot.nType 
			&& QuickSlot.wID == Slot.SKILLINFO.wSkill)
		{
			QuickSlot.nSkillLevel      = Slot.SKILLINFO.cSkillLevel;
			QuickSlot.nSkillLockCount  = Slot.SKILLINFO.cLockCount;
		}
	}
}


bool CCharacter::MoveQuickSlot(const TakeType takeType, const unsigned short usSkillID, unsigned char cLockCount, unsigned char cSkillLevel)
{
	bool bResult = 0;

	if (QUICK::MAX_QUICK_NUM <= takeType.m_dstPos.m_cIndex)
	{
		ERRLOG3(g_Log, "CID:%10u 퀵슬롯 이동 이상. 퀵슬롯 인덱스 위치가 이상합니다."
			"Dst(%2d,%2d)", m_dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
        return false;
	}

	switch (takeType.m_srcPos.m_cPos) 
	{
		case TakeType::TS_INVEN:
		case TakeType::TS_TEMP:
		{
			Item::CItem* pItem = GetItem(takeType.m_srcPos);
			if (NULL == pItem) 
			{
				ERRLOG3(g_Log, "CID:%10u 퀵슬롯 이동 이상. 아이템을 얻어올 수 없습니다. Src(%2d,%2d)",
					m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
			}
            else if (pItem->IsSet(Item::DetailData::QUICKSLOT_IN))
			{
				m_DBData.m_Quick.Slots[takeType.m_dstPos.m_cIndex] = 
                    QUICKSLOT(QUICKSLOT::ITEM, 0, 0, pItem->GetPrototypeID());
				return true;
			}

		} break;		
		
		case TakeType::TS_SSLOT:
		{
			if (SKILL::MAX_SLOT_NUM <= takeType.m_srcPos.m_cIndex)
			{
				ERRLOG3(g_Log, "CID:%10u 옮기려는 스킬 위치가 이상합니다. Src(%2d,%2d)",
					m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
			}
            else if (TakeType::TS_QSLOT == takeType.m_dstPos.m_cPos)
			{
				const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(usSkillID);
				if (NULL == lpProtoType)
				{
					ERRLOG2(g_Log, "CID:%10u 옮기려는 스킬의 아이디가 존재하지 않습니다. ID:0x%04x", m_dwCID, usSkillID);
					return false;
				}

				unsigned char cSkillLockCount = 0;
				unsigned char cSkillCurLevel = 1;
				// 액션  스크립트이면
				if(lpProtoType->m_eSkillType == Skill::Type::ACTION)
				{
					cSkillLockCount = cLockCount;
					cSkillCurLevel = cSkillLevel;
				}
				else
				{
					SKILLSLOT SkillSlot = m_DBData.m_Skill.SSlot[takeType.m_srcPos.m_cIndex];

					if (false == lpProtoType->m_bIsClassSkill)
					{
						cSkillLockCount = SkillSlot.SKILLINFO.cLockCount;
						cSkillCurLevel = SkillSlot.SKILLINFO.cSkillLevel;
					}
				}

				m_DBData.m_Quick.Slots[takeType.m_dstPos.m_cIndex] = 
					QUICKSLOT(QUICKSLOT::SKILL, cSkillLockCount, cSkillCurLevel, usSkillID);
				return true;
			}

		} break;

		case TakeType::TS_QSLOT:
		{
            if (QUICK::MAX_QUICK_NUM <= takeType.m_srcPos.m_cIndex)
			{
				ERRLOG2(g_Log, "CID:%10u 옮기려는 퀵슬롯 위치가 이상합니다. 스킬 위치 %d",
					m_dwCID, takeType.m_srcPos.m_cIndex);
            }
            else
            {
			    if (TakeType::TS_QSLOT == takeType.m_dstPos.m_cPos)
			    { 
				    std::swap(m_DBData.m_Quick.Slots[takeType.m_srcPos.m_cIndex],
					    m_DBData.m_Quick.Slots[takeType.m_dstPos.m_cIndex]);
			    }
			    else 
			    { 
				    m_DBData.m_Quick.Slots[takeType.m_srcPos.m_cIndex] = 
					    QUICKSLOT(QUICKSLOT::NONE, 0, 0, 0);
			    }

			    return true;
            }

		} break;
	}
	
	return false;
}
