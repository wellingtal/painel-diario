#ifndef _OLD_CITEM_MGR_H_
#define _OLD_CITEM_MGR_H_

#include "OldItemStructure.h"


// ----------------------------------------------------------------------------
// class : CItemMgr ( must use in single thread )
// ������ ��ũ��Ʈ�� �ε��ϰ�, �����ϰ�, �� �������� �����ϴ� ���� ������ �Ѵ�.

namespace OldItem
{
    class CItemMgr
    {
    public:

        static CItemMgr& GetInstance();
    	
		~CItemMgr();

		// ----------------------------------------------------------------------------
		// ������ ��ũ��Ʈ (ItemScript.txt)

	    bool LoadItemProtoType(const char* szFileName = NULL);
	    bool LoadItemProtoTypeBinary(const char* szFileNameBinary = NULL);
	    bool SaveItemProtoTypeBinary(const char* szFileNameBinary = NULL, const char* szTrashFile = NULL);

        const OldItem::ItemInfo* GetItemInfo(unsigned short usProtoTypeID);
        const OldItem::ItemInfo* GetItemInfoFromItemName(const char* szItemName); 
        const OldItem::ItemInfo* GetItemInfoFromEquipName(const char* szEquipName);

        const char*	GetEquipName(const unsigned short usItemProtoTypeID);
		const unsigned short GetItemIDFromSkillID(const unsigned short usSkill_ID, const unsigned short unSkill_LockCount);        		


		// ----------------------------------------------------------------------------
		// ������ �ռ� ��ũ��Ʈ (ChemicalInfo.txt)

		bool LoadItemChemical(const char* szFileName = NULL);
		bool LoadItemChemicalBinary(const char* szFileNameBinary = NULL);
		bool SaveItemChemicalBinary(const char* szFileNameBinary = NULL, const char* szTrashFile = NULL);

		unsigned char GetChemicalResult(ChemicalInfo& chemical);


    private:

	    CItemMgr();
	    
        void DestoryItemInfo(void);
		void DestoryChemical(void);

        static const char*  ms_szItemScriptFileName;
		static const char*	ms_szBGItemScriptFileName;
		static const char*	ms_szChemicalScriptFileName;

        size_t              m_nItemNum;
        ItemInfo*			m_ItemInfoArray;

		size_t				m_nChemicalNum;
		ChemicalInfo*		m_ChemicalArray;
    };
}


#endif