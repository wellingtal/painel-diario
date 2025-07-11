#ifndef _CITEM_MGR_H_
#define _CITEM_MGR_H_

#define g_ItemMgr   Item::CItemMgr::GetInstance()

#include <vector>
#include <map>

#include "ItemStructure.h"


// ----------------------------------------------------------------------------
// class : CItemMgr ( must use in single thread )
// ������ ��ũ��Ʈ�� �ε��ϰ�, �����ϰ�, �� �������� �����ϴ� ���� ������ �Ѵ�.

namespace Item
{
	// ���� ����
	class CItem;

	class CItemMgr
	{
	public:

		static CItemMgr& GetInstance();

		typedef std::map<ItemType::Type, unsigned char> RuneItemCnt;

		// ----------------------------------------------------------------------------
		// ������ ��ũ��Ʈ (ItemScript.txt)

		bool LoadItemProtoType(const char* szFileName = NULL);
		bool LoadItemProtoTypeBinary(const char* szFileNameBinary = NULL);
		bool SaveItemProtoTypeBinary(const char* szFileNameBinary = NULL, const char* szTrashFile = NULL);

		const ItemInfo* GetItemInfo(unsigned short usProtoTypeID);
		const ItemInfo* GetItemInfoFromItemName(const char* szItemName); 
		const ItemInfo* GetItemInfoFromEquipName(const char* szEquipName);

		const char*	GetEquipName(const unsigned short usItemProtoTypeID);
		const unsigned short GetItemIDFromSkillID(const unsigned short usSkill_ID, const unsigned short unSkill_LockCount);        	

		const unsigned short GetDropItem(const Item::ItemType::Type eItemType, const unsigned char cLevel);
		const unsigned short GetRingDropItem(const unsigned char cLevel, const unsigned char cGrade);	//--//
		const unsigned short GetNecklaceDropItem(const unsigned char cLevel, const unsigned char cGrade);	//--//
		const unsigned short GetRuneDropItem(const unsigned char cLevel, const unsigned char cGrade);	//--//
		void PrintDropListByLevel(void);
		void PrintRingDropListByLevel(void);	//--//
		void PrintNecklaceDropListByLevel(void);	//--//
		void PrintRuneDropListByLevel(void);	//--//


		// ----------------------------------------------------------------------------
		// ������ �ռ� ��ũ��Ʈ (ChemicalInfo.txt)

		bool LoadItemChemical(const char* szFileName = NULL);
		bool LoadItemChemicalBinary(const char* szFileNameBinary = NULL);
		bool SaveItemChemicalBinary(const char* szFileNameBinary = NULL, const char* szTrashFile = NULL);

		unsigned char GetChemicalResult(ChemicalInfo& chemical);

		// ----------------------------------------------------------------------------
		// ������ ���� ��ũ��Ʈ (SpeacialCompensation.txt)

		bool LoadItemSpeacialCompensation(const char* szFileName = NULL);
		bool LoadItemSpeacialCompensationBinary(const char* szFileNameBinary = NULL);
		bool SaveItemSpeacialCompensationBinary(const char* szFileNameBinary = NULL, const char* szTrashFile = NULL);

		unsigned short GetCompensationItem(unsigned char cItemType, short wLimitValue);

		// ������ Ÿ�Կ� ���� �� ���� ���밪.
		unsigned char GetMaxRuneSocket(ItemType::Type type);

	private:

		CItemMgr();
		~CItemMgr();

		void DestoryItemInfo(void);
		void DestoryChemical(void);
		void DestoryCompensation(void);

		static const char*	ms_szChemicalScriptFileName;
		static const char*	ms_szSpeacialCompensationFileName;

		static CItemMgr     ms_this;

		size_t              m_nItemNum;
		ItemInfo*			m_ItemInfoArray;

		typedef std::vector<unsigned short> ItemIDVector;

		ItemIDVector	m_vecDropListByLevel[Creature::MONSTER_MAX_LEVEL];
		ItemIDVector	m_vecRingDropListByLevel[Creature::MONSTER_MAX_LEVEL][EquipType::MAX_GRADE];	//--//
		ItemIDVector	m_vecNecklaceDropListByLevel[Creature::MONSTER_MAX_LEVEL][EquipType::MAX_GRADE];	//--//
		ItemIDVector	m_vecRuneDropListByLevel[Creature::MONSTER_MAX_LEVEL][EquipType::MAX_GRADE];	//--//
		ItemIDVector	m_vecCompensation[Compensation::MAX_CASE];

		size_t				m_nChemicalNum;
		ChemicalInfo*		m_ChemicalArray;

		RuneItemCnt			m_mapRuneItemCnt;				// �� ������ �ִ� ����.
	};
}


#endif