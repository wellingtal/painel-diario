#ifndef _RYL_REBALANCE_LIB_REBALANCE_ITEM_H_
#define _RYL_REBALANCE_LIB_REBALANCE_ITEM_H_

#include <algorithm>

#include <Skill/SkillMgr.h>
#include <Item/ItemConstants.h>

namespace Item
{
    // forward decl.
    struct ItemData;
    struct ItemInfo;

    namespace Grade
    {
        struct GradeInfo;
    }

    class CItemSerialMgr;
}

namespace OldItem
{
    // forward decl.
    struct ItemInfo;
}

namespace RebalanceLib
{
    class CItemChecker;

    class CItemGrade
    {
    public:

        static CItemGrade& GetInstance();

        CItemGrade() { std::fill_n(m_dwItemGradeNum, size_t(Item::EquipType::MAX_GRADE), 0LL); }

        void AddGradeItemNum(Item::EquipType::Grade eGrade) { ++m_dwItemGradeNum[eGrade]; }        
        unsigned __int64 GetGradeItemNum(Item::EquipType::Grade eGrade) const { return m_dwItemGradeNum[eGrade]; }

        void WriteGradeNum();

    private:

        unsigned __int64 m_dwItemGradeNum[Item::EquipType::MAX_GRADE];
    };

    struct SkillBookInfo
    {
        unsigned long m_dwBookNum[CSkillMgr::MAX_SKILL_LOCKCOUNT];

        bool IsEmpty()
        {
            for(int nCount = 0; nCount < CSkillMgr::MAX_SKILL_LOCKCOUNT; ++nCount)
            {
                if(0 != m_dwBookNum[nCount])
                {
                    return false;
                }
            }

            return true;
        }
    };

    const char* GetGradeString(Item::EquipType::Grade grade);

    bool ConvertRebalanceItem(unsigned long dwUID, unsigned long dwCID, 
        const Item::ItemData* lpItemData, 
        const Item::ItemInfo& newItemInfo, const OldItem::ItemInfo& oldItemInfo, 
        char* szData_Out, unsigned long& dwDataSize_InOut, Item::Grade::GradeInfo& convertedGrade);

    bool ConvertContainer(unsigned long dwUID, unsigned long dwCID, 
        CItemChecker* lpItemChecker, SkillBookInfo* lpSkillBookInfo,
        const char* szOriginalData, unsigned long dwOriginalSize,
        char* szChangedData, unsigned long& dwChangedSize_InOut);

    bool AddSkillBook(unsigned long dwUID, unsigned long dwCID, const char* szPosition, 
        Item::CItemSerialMgr& itemSerialMgr, unsigned long& dwGold_InOut, 
        char* szData, unsigned long& dwDataSize_InOut, unsigned long dwMaxDataSize,
        CItemChecker& itemChecker, SkillBookInfo& skillBookInfo);
};


#endif
