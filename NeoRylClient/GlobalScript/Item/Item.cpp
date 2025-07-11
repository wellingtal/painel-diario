
#include "stdafx.h"

// Ŭ���̾�Ʈ������ ���̹Ƿ� include�� ����Ѵ�.
#include <algorithm>
#include <Log/ServerLog.h>
#include <Utility/Math/Math.h>
#include <Creature/Character/CharacterClass.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>

#ifdef _RYL_GAME_CLIENT_
#include "RYLNetworkData.h"
#endif

#ifndef _RYL_GAME_CLIENT_
#include <Utility/Setup/ServerSetup.h>
#endif

#include "Item.h"
#include "GMMemory.h"

using namespace Item;

static ItemInfo NullProtoType;

CItem::CItem()
:   m_ItemInfo(NullProtoType)
{
	m_ItemData.m_dwUID = 0;
	m_ItemData.m_usProtoTypeID = 0;

	m_ItemData.m_ItemPos.m_cPos = 0;
	m_ItemData.m_ItemPos.m_cIndex = 0;

	m_ItemData.m_cItemSize = sizeof(ItemData);
	m_ItemData.m_cNumOrDurability = 0;
	m_cMaxNumOrDurability = 0;
	m_dwStallPrice  = 0;
	m_dwPrice       = 0;
}

CItem::CItem(const ItemInfo& itemInfo)
:   m_ItemInfo(itemInfo)
{
	m_ItemData.m_dwUID = 0;
	m_ItemData.m_usProtoTypeID = itemInfo.m_usProtoTypeID;

	m_ItemData.m_ItemPos.m_cPos = 0;
	m_ItemData.m_ItemPos.m_cIndex = 0;

	m_ItemData.m_cItemSize = sizeof(ItemData);
	m_ItemData.m_cNumOrDurability = m_ItemInfo.m_DetailData.m_cDefaultDurabilityOrStack;
	m_cMaxNumOrDurability = m_ItemInfo.m_DetailData.m_cMaxDurabilityOrStack;
	m_dwStallPrice  = 0;
	m_dwPrice       = m_ItemInfo.m_DetailData.m_dwPrice;
}

CItem::~CItem()
{

}


bool CItem::SerializeOut(char* lpSerializeItem_Out, size_t& nBufferLength_InOut)
{
	if (nBufferLength_InOut >= sizeof(ItemData))
	{
		m_ItemData.m_cItemSize = sizeof(ItemData);
		nBufferLength_InOut = sizeof(ItemData);

		*reinterpret_cast<ItemData*>(lpSerializeItem_Out) = m_ItemData;
		return true;
	}

	nBufferLength_InOut = 0;
	return false;
}

bool CItem::SerializeIn(const char* lpSerializeItem_In, size_t& nBufferLength_InOut)
{
	const ItemData& itemData = *reinterpret_cast<const ItemData*>(lpSerializeItem_In);

	if (sizeof(ItemData) <= nBufferLength_InOut)
	{        
		nBufferLength_InOut = itemData.m_cItemSize;

		if (itemData.m_usProtoTypeID == m_ItemInfo.m_usProtoTypeID)
		{
			m_ItemData = itemData;
			m_itemPos_Real = m_ItemData.m_ItemPos;
			return true;
		}
	}

	ERRLOG4(g_Log, "SerializeIn�� �����߽��ϴ�. �������� �����մϴ�. ItemDataSize:%d, nBufferLength_InOut:%d, "
		"itemData.m_usProtoTypeID:%d, m_ItemInfo.m_usProtoTypeID:%d", 
		sizeof(ItemData), sizeof(nBufferLength_InOut), itemData.m_usProtoTypeID, m_ItemInfo.m_usProtoTypeID);

	return false;
}

unsigned long CItem::GetBuyPrice(void) const
{ 
	// edith �ǸŰ���
	if (0 != m_dwStallPrice) 
	{ 
		return m_dwStallPrice; 
	}

	if (true == m_ItemInfo.m_DetailData.m_bOptionPrice)
	{
		// ����� ����, �Ǹ�, ����, ���� ������ ��� �ɼǰ� �������� ���� ���� ������ �Ҽ��� ���� �ø��մϴ�.
		return static_cast<unsigned long>((m_dwPrice * 
			(100 - (m_cMaxNumOrDurability - m_ItemData.m_cNumOrDurability) * 0.25f) / 100.0f) + 1);
	}

	return m_dwPrice;
}

unsigned long CItem::GetSellPrice(void) const
{	
	if (0 != m_dwStallPrice) 
	{ 
		return m_dwStallPrice; 
	}

	if (Item::ItemType::GEM_SELL == static_cast<Item::ItemType::Type>(m_ItemInfo.m_DetailData.m_cItemType))
	{
		return static_cast<unsigned long>(m_dwPrice);
	}

	unsigned long Price = static_cast<unsigned long>(m_dwPrice / 3.0f);

	if (true == m_ItemInfo.m_DetailData.m_bOptionPrice)
	{
		Price = static_cast<unsigned long>((m_dwPrice * (100 - (m_cMaxNumOrDurability - m_ItemData.m_cNumOrDurability) * 0.25f) / 100.0f) / 3);
	}

	if(Price == 0)
		Price = 1;

	return Price;
/*
	if (true == m_ItemInfo.m_DetailData.m_bOptionPrice)
	{
		return static_cast<unsigned long>(((m_dwPrice * 
			(100 - (m_cMaxNumOrDurability - m_ItemData.m_cNumOrDurability) * 0.25f) / 100.0f) / 3) + 1);
	}

	return static_cast<unsigned long>(m_dwPrice / 3.0f);
*/
}

unsigned long CItem::GetRepairPrice(void) const
{
	return static_cast<unsigned long>((m_dwPrice * 
		((m_cMaxNumOrDurability - m_ItemData.m_cNumOrDurability) * 0.25f) / 400.0f) + 1);//was 200.0f
}

unsigned long CItem::GetUpgradePrice(void) const
{
	return static_cast<unsigned long>((m_dwPrice * 30.0f) + 5);//was 0.3f
//	return static_cast<unsigned long>(((m_dwPrice * 
//		(100 - (m_cMaxNumOrDurability - m_ItemData.m_cNumOrDurability) * 0.25f) / 100.0f) / 2) + 1); // was 
}

unsigned long CItem::GetGraftPrice(void) const
{
	return static_cast<unsigned long>((m_dwPrice * 30.0f) + 5); //was 0.3f
//	return static_cast<unsigned long>((m_dwPrice * (100 * 2.5f) / 100.0f) + 10); 
}



CEquipment::CEquipment(const ItemInfo& itemInfo)
:   CItem(itemInfo), m_cUpgradeLevel(0), m_cSocketNum(0), m_cSeasonRecord(0), m_cCoreLevel(0)
{
	if (1 == m_ItemInfo.m_DetailData.m_cXSize && 1 == m_ItemInfo.m_DetailData.m_cYSize)
	{
		m_cMaxSocket    = EquipmentInfo::MAX_MINSIZE_SOCKET_NUM;
		m_cMaxAttribute = EquipmentInfo::MAX_MINSIZE_ATTRIBUTE_NUM;
	}
	else
	{
		m_cMaxSocket    = EquipmentInfo::MAX_SOCKET_NUM;
		m_cMaxAttribute = EquipmentInfo::MAX_ATTRIBUTE_NUM;
	}

	m_cMaxSocket = (itemInfo.m_DetailData.m_cMaxSocketNum < m_cMaxSocket) ? 
		itemInfo.m_DetailData.m_cMaxSocketNum : m_cMaxSocket;

	m_ItemData.m_cItemSize += sizeof(EquipmentInfo);

	std::fill_n(m_cSocket, int(EquipmentInfo::MAX_SOCKET_NUM), 0);

	std::fill_n(m_usRuneSocket, int(EquipmentInfo::MAX_RUNE_SOCKET_NUM), 0);

	InitializeAttribute();    
}

CEquipment::~CEquipment()
{
}

bool CEquipment::SerializeOut(char* lpSerializeItem_Out, size_t& nBufferLength_InOut)
{
	// edith 2009.09.16 ���� ���� ����
	const size_t nMaxItemSize = sizeof(ItemData) + sizeof(EquipmentInfo) + 
		EquipmentInfo::MAX_SOCKET_NUM * sizeof(unsigned char) + 
		Item::EquipmentInfo::MAX_ATTRIBUTE_NUM * sizeof(ItemAttribute);

	float fGradeInfo = 0;
	const CItemType::ArrayType eEquipType = CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags);

	if (nMaxItemSize <= nBufferLength_InOut)
	{
		unsigned int nIndex = 0, nSocketIndex = 0, nAttributeNum = 0;

		// �⺻ ���� ����
		ItemData& itemData = *reinterpret_cast<ItemData*>(lpSerializeItem_Out);
		itemData = m_ItemData;

		EquipmentInfo&  equipmentInfo   = *reinterpret_cast<EquipmentInfo*>(lpSerializeItem_Out + sizeof(ItemData));
		char*           lpSocketInfo    = lpSerializeItem_Out + sizeof(ItemData) + sizeof(EquipmentInfo);

		// ������ ���� ���� ����
		for(nIndex = 0, nSocketIndex = 0; 
			nIndex < EquipmentInfo::MAX_SOCKET_NUM && nSocketIndex < m_cMaxSocket; ++nIndex)
		{
			if (0 != m_cSocket[nIndex])
			{
				lpSocketInfo[nSocketIndex] = m_cSocket[nIndex];
				++nSocketIndex;
			}
		}

		// �Ӽ������� ����, ���׷��̵忡 ���� ���� ����
		ApplyGemAttribute(REMOVE);
		ApplyUpgradeAttribute(REMOVE);		
		ApplyRuneAttribute(REMOVE);

		// ������ �Ӽ� ���� ����
		ItemAttribute* lpAttribute = reinterpret_cast<ItemAttribute*>(
			lpSocketInfo + (nSocketIndex * sizeof(unsigned char)));

		for(nIndex = 0, nAttributeNum = 0;
			nIndex < Item::Attribute::MAX_DB_ATTRIBUTE_NUM && nAttributeNum < m_cMaxAttribute; ++nIndex)
		{
			// ���� �Ӽ��� - ������(��ũ��Ʈ)
			short usDiffAttribute = m_wAttribute[nIndex] - m_ItemInfo.m_EquipAttribute.m_usAttributeValue[nIndex];

			// edith 2008.07.16 ������� �ʴ� �ɷ�ġ�� �����ϱ�
			fGradeInfo = Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::S_GRADE, static_cast<Attribute::Type>(nIndex));
			if(fGradeInfo == 0.0f)
				usDiffAttribute = 0;

			if (0 != usDiffAttribute)
			{
				lpAttribute->m_cType    = nIndex;
				lpAttribute->m_usValue  = usDiffAttribute;
				++nAttributeNum;
				++lpAttribute;
			}
		}
/*
		for(unsigned char cIndex = 0; cIndex < 2; cIndex++)
		{
			unsigned short usRune = GetRuneSocket(cIndex);

			if(usRune)
			{
				lpAttribute->m_cType    = Attribute::RUNE;
				lpAttribute->m_usValue  = usRune-EtcItemID::RUNE_START_ID;
				++nAttributeNum;
				++lpAttribute;
			}
		}
*/
		// �Ӽ������� ����, ���׷��̵忡 ���� ���� �߰�
		ApplyGemAttribute(APPLY);
		ApplyUpgradeAttribute(APPLY);
		ApplyRuneAttribute(APPLY);

		// ���� �� ũ�� ���� ����
		m_ItemData.m_cItemSize = itemData.m_cItemSize = sizeof(ItemData) + sizeof(EquipmentInfo) + 
			nSocketIndex * sizeof(unsigned char) + nAttributeNum * sizeof(ItemAttribute);

		equipmentInfo.m_cReserved1			= 0;
		equipmentInfo.m_cUpgradeLevel		= m_cUpgradeLevel;
		equipmentInfo.m_cDiffMaxSocket      = m_cMaxSocket - m_ItemInfo.m_DetailData.m_cMaxSocketNum;
		equipmentInfo.m_cSocketNum          = nSocketIndex;      
		equipmentInfo.m_cAttributeNum       = nAttributeNum;
		equipmentInfo.m_cDiffMaxDurability  = m_cMaxNumOrDurability - m_ItemInfo.m_DetailData.m_cMaxDurabilityOrStack;
		equipmentInfo.m_cSeasonRecord		= m_cSeasonRecord;
		equipmentInfo.m_cReserved2			= 0;
		equipmentInfo.m_cUpgradeCnt			= m_cUpgradeCnt;
		equipmentInfo.m_cCoreLevel			= m_cCoreLevel;

		nBufferLength_InOut = m_ItemData.m_cItemSize;
		return true;
	}

	nBufferLength_InOut = 0;
	return false;
}

bool CEquipment::SerializeIn(const char* lpSerializeItem_In, size_t& nBufferLength_InOut)
{    
	if (true == CItem::SerializeIn(lpSerializeItem_In, nBufferLength_InOut))
	{
		const EquipmentInfo& equipmentInfo = 
			*reinterpret_cast<const EquipmentInfo*>(lpSerializeItem_In + sizeof(ItemData));

		m_cUpgradeLevel         = equipmentInfo.m_cUpgradeLevel;
		m_cMaxSocket            = m_ItemInfo.m_DetailData.m_cMaxSocketNum + equipmentInfo.m_cDiffMaxSocket;         
		m_cSocketNum            = equipmentInfo.m_cSocketNum;
		m_cMaxNumOrDurability   = m_ItemInfo.m_DetailData.m_cMaxDurabilityOrStack + equipmentInfo.m_cDiffMaxDurability;
		m_cSeasonRecord			= equipmentInfo.m_cSeasonRecord;
		m_cUpgradeCnt			= equipmentInfo.m_cUpgradeCnt;
		m_cCoreLevel			= equipmentInfo.m_cCoreLevel;

		const unsigned char* lpSocketIndex   = reinterpret_cast<const unsigned char*>(&equipmentInfo + 1);
		const unsigned char* lpSocketPastEnd = lpSocketIndex + m_cSocketNum;
		std::copy(lpSocketIndex, lpSocketPastEnd, m_cSocket);

		InitializeAttribute();

		float fGradeInfo = 0;
		const CItemType::ArrayType eEquipType = CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags);

		const ItemAttribute* first = reinterpret_cast<const ItemAttribute*>(lpSocketPastEnd);
		const ItemAttribute* last  = first + equipmentInfo.m_cAttributeNum;

		for (; first != last; ++first) 
		{ 
			// edith 2008.07.16 ������� �ʴ� �ɷ�ġ�� �����ϱ�
			fGradeInfo = Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::S_GRADE, static_cast<Attribute::Type>(first->m_cType));

			if(fGradeInfo > 0.0f)
				AddAttribute(static_cast<Attribute::Type>(first->m_cType), first->m_usValue);
		}

		InitializeGemAttribute();
		InitializeUpgradeAttribute();

		ApplyGemAttribute(APPLY);
		ApplyUpgradeAttribute(APPLY);

		CalculateItemGrade();
		CalculateItemPrice();

		// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
		RevisionLimit();

		return true;
	}

	return false;
}

BOOL CEquipment::AddRandomSocket()
{
	int iRandom = Math::Random::ComplexRandom(1000);

	int iRnd[3] = { 395, 600, 5 };

	for(int i = 0; i < 3; ++i)
	{
		if(iRandom <= iRnd[i])
		{
			m_cMaxSocket = i+1;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CEquipment::AddRandomOption(EquipType::Grade eGrade, unsigned char cBaseNum, int iMagicChance)
{
	if (0 == m_ItemInfo.m_DetailData.m_wDropOption)	{ return FALSE; }

	// ��������
	if(iMagicChance > 0)
	{
		if(eGrade == EquipType::F_GRADE)
		{
			if((int)Math::Random::ComplexRandom(10000) < (int)iMagicChance*100)
			{
				eGrade = EquipType::D_GRADE;
				iMagicChance = 1;
			}
		}
		else
		{            
			int iPerNum = cBaseNum;

			int iNum = iMagicChance/100;
			
			if(iNum > 0)
				cBaseNum += iNum;

			iNum = iMagicChance-(iNum*100);
			
			if((int)Math::Random::ComplexRandom(10000) < (int)iNum*100)
				++cBaseNum;

			if(iPerNum == cBaseNum)
				iMagicChance = 0;
			else 
				iMagicChance = 1;

			// ���̽��� +5���� �ֱ� ������ +5 �̻� �ö��� �ʰ� 5���� �����Ѵ�.
			if(cBaseNum >= 5)
				cBaseNum = 5;
		}
	}
	else
		iMagicChance = 0;

	unsigned char cRequiredNum = cBaseNum;

	unsigned char cIndex = 0;

	// ���̽��� ����� �Ӽ��� �������� ���ϱ� ���� �Ӽ� �ε����� ����д�.
	unsigned char aryAttributeIndex[Attribute::MAX_ATTRIBUTE_NUM] = { 0, };
	for (; cIndex < Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
	{
		aryAttributeIndex[cIndex] = cIndex;
	}
	std::random_shuffle(aryAttributeIndex, aryAttributeIndex + Attribute::MAX_ATTRIBUTE_NUM);

	const Item::CItemType::ArrayType eEquipType = CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags);

	// ���� �Ӽ��� ���� �ɼ��� ���
	for (cIndex = 0; cIndex < Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
	{
		EquipType::Grade eUseGrade = eGrade;
		if (eEquipType == CItemType::DAGGER_TYPE || 
			eEquipType == CItemType::ONEHANDED_TYPE || 	
			eEquipType == CItemType::LONGRANGE_TYPE || 		
			eEquipType == CItemType::TWOHANDED_TYPE || 	
			eEquipType == CItemType::STAFF_TYPE)
		{
			if (aryAttributeIndex[cIndex] == Attribute::BLOCK)
			{
				// ���⿡ �ٴ� ��� �ɼ��� C �׷��̵带 �ɰ��� �� ����.
				eUseGrade = max(eGrade, EquipType::C_GRADE);
			}
		}

		const short wMaxValue = static_cast<short>(m_ItemInfo.m_DetailData.m_wDropOption * 
			Grade::GetGradeValue(eEquipType, Grade::DROP_MAX_VALUE, eUseGrade, static_cast<Attribute::Type>(aryAttributeIndex[cIndex])));

		if (0 != wMaxValue)
		{
			short wMinValue = 0;

			if (cBaseNum > 0)
			{
				// ���̽��� ������ �Ӽ��� ���Ѱ��� ��ġ�׷��̵忡�� ���������μ� ������ ���ϴ� �׷��̵尡 �ǰԲ� �Ѵ�.
				wMinValue = static_cast<short>(ceil(m_ItemInfo.m_DetailData.m_wDropOption * 
					Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, eUseGrade, static_cast<Attribute::Type>(aryAttributeIndex[cIndex]))));
				--cBaseNum;
			}
			else
			{
				wMinValue = static_cast<short>(ceil(m_ItemInfo.m_DetailData.m_wDropOption * 
					Grade::GetGradeValue(eEquipType, Grade::DROP_MIN_VALUE, eUseGrade, static_cast<Attribute::Type>(aryAttributeIndex[cIndex]))));
			}

			const unsigned short wRange = wMaxValue - wMinValue + 1;

			short wPlusOption = static_cast<short>(Math::Random::ComplexRandom(wRange)) + wMinValue;
			wPlusOption = max(short(0), wPlusOption);

			AddAttribute(static_cast<Attribute::Type>(aryAttributeIndex[cIndex]), wPlusOption);
		}
	}

	// (�ִ� �������� �⺻ �ɼ� + �߰� �ɼ�)�� (�ּ� �������� �⺻ �ɼ� + �߰� �ɼ�)���� ���� �� ����.
	m_wAttribute[Attribute::MAX_DAMAGE] = max(m_wAttribute[Attribute::MIN_DAMAGE], m_wAttribute[Attribute::MAX_DAMAGE]);

	// �ִ� ������ ���
	const short wAddMaxDurability = static_cast<short>(Math::Random::ComplexRandom(101) - 50);
	if (wAddMaxDurability < 0 && m_cMaxNumOrDurability < abs(wAddMaxDurability))
	{
		m_cMaxNumOrDurability = 1;
	}
	else
	{
		m_cMaxNumOrDurability = m_ItemInfo.m_DetailData.m_cMaxDurabilityOrStack + wAddMaxDurability;
	}

	// ���� ������ ��� (1 ~ �ִ� ������)
	m_ItemData.m_cNumOrDurability = static_cast<unsigned char>(Math::Random::ComplexRandom(m_cMaxNumOrDurability) + 1);

	// �׷��̵�/���� ����
	CalculateItemGrade();
	CalculateItemPrice();

	// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
	RevisionLimit();

	if (m_GradeInfo.m_eItemGrade != eGrade)
	{
		ERRLOG4(g_Log, "�����ۿ� �ɼ��� ���̴µ� ������ �ֽ��ϴ�. ��û�׷��̵�:%d(%d), �����׷��̵�:%d(%d)", 
			eGrade, cRequiredNum, m_GradeInfo.m_eItemGrade, m_GradeInfo.m_cPlus);
	}


	if(iMagicChance == 0)
		return TRUE;

	return FALSE;
}

// ryl2 avalansa
CEquipment::EQUIPMENT_ERROR CEquipment::OptionGraft(CEquipment* lpSacrifice, bool bUpgradeLevelLimit, unsigned char cAttributeType, 
													unsigned long dwCurrentGold, unsigned long& dwUsedGold)
{
	// Additional options before transplantation option to delete the runes.
	Item::CEquipment::RuneApplyType	enRuneApplyType	= static_cast<Item::CEquipment::RuneApplyType>(Item::CEquipment::RUNE_ALL | Item::CEquipment::RUNE_REMOVE);

	SetRuneAttribute(enRuneApplyType, 0); // K�mnes pos + 
	lpSacrifice->SetRuneAttribute(enRuneApplyType, 0); // 11 pos + 

	if (false == CanOptionGraft(
		static_cast<CItemType::ArrayType>(CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags)), 
		static_cast<Attribute::Type>(cAttributeType))) // 12 pos + 
//	{
//		ERRLOG0(g_Log, "Options transplant error: option option is not possible transplant.");
//		return EQUIPMENT_ERROR::E_WRONG_ATTRIBUTE;
//	}

	if (m_ItemInfo.m_usProtoTypeID != lpSacrifice->GetPrototypeID())
	{
		ERRLOG0(g_Log, "Options transplant error: Item is not the same kind of.");
		return EQUIPMENT_ERROR::E_NOT_EQUAL_KIND;
	}





	if (m_GradeInfo.m_eItemGrade < lpSacrifice->GetItemGrade().m_eItemGrade) // 13 pos +
	{
		ERRLOG0(g_Log, "Options transplant error: the higher the grade of the original item.");
		return EQUIPMENT_ERROR::E_HIGH_GRADE_ORIGINAL;
	}

	// edith By option prices to obtain transplants
	dwUsedGold = GetGraftPrice(); // 14 pos +
	if (dwCurrentGold < dwUsedGold)
	{
		ERRLOG0(g_Log, "Options transplant Error: There is not enough money.");
		return EQUIPMENT_ERROR::E_NOT_ENOUGH_MONEY;
	} 
/*
	// edith Additional options graft failure logic. Should be checked only on the server..
	if( 30 < Math::Random::ComplexRandom(100)) 
	{
	// Options graft failure 
	// apparent error message is not output.
		return EQUIPMENT_ERROR::E_GRAFE_FAILD;
	}
*/
	if (true == bUpgradeLevelLimit)
	{
		if (m_cUpgradeLevel != 0 && m_cUpgradeLevel != 10)
		{
			ERRLOG0(g_Log, "Options transplant error: smelting upgrade is only possible level of 0 or 10 (the original)");
			return EQUIPMENT_ERROR::E_NOT_UPGRADE_LEVEL;
		}

		if (lpSacrifice->m_cUpgradeLevel != 0 && lpSacrifice->m_cUpgradeLevel != 10)
		{
			ERRLOG0(g_Log, "Options transplant error: smelting upgrade is only possible level of 0 or 10 (jemyul)");
			return EQUIPMENT_ERROR::E_NOT_UPGRADE_LEVEL;
		}
	}


	const Item::Attribute::Type eAttributeType = static_cast<Item::Attribute::Type>(cAttributeType);
	const short nAttributeValue = 
		((m_wAttribute[eAttributeType] + lpSacrifice->GetAttribute(eAttributeType) + 1) / 2) - m_wAttribute[eAttributeType];

	// Edith 2009.03.15 min max damage is higher than the success of transplant options 
	// Minimum Damage Max Damage when the value is larger than when 
	// Fail option must be transplanted.
	if(eAttributeType == Attribute::MIN_DAMAGE)
	{
	// edith 2008.11.14 obsyeon-isig MIN_DEMAGE beogeusujeong.
	// choeso demijiga maegseu demijiboda keumyeon 
	// isig bulganeunghan obsyeon-ilago mesijileul ttuiunda. hue mesijileul sebunhwahaja.
		if(m_wAttribute[Attribute::MIN_DAMAGE]+nAttributeValue > m_wAttribute[Item::Attribute::MAX_DAMAGE])
		{
			ERRLOG0(g_Log, "Options graft failure: MinDemage is higher than MaxDemage. Options transplant failed..");
			return EQUIPMENT_ERROR::E_GRAFE_FAILD;
		}
	}
	AddAttribute(eAttributeType, nAttributeValue);
	using namespace EquipType;
	if(m_GradeInfo.m_eItemGrade == F_GRADE || 
		m_GradeInfo.m_eItemGrade == D_GRADE ||
		m_GradeInfo.m_eItemGrade == C_GRADE ||
		m_GradeInfo.m_eItemGrade == B_GRADE ||
		m_GradeInfo.m_eItemGrade == A_GRADE ||
		m_GradeInfo.m_eItemGrade == AAA_GRADE )
//		m_GradeInfo.m_eItemGrade == AAA_GRADE) // add by shadow
		{
			m_cUpgradeLevel = 0;
			InitializeUpgradeAttribute();  
			m_cSocketNum = 0;
			std::fill_n(m_cSocket, unsigned char(Item::EquipmentInfo::MAX_SOCKET_NUM), 0);
			m_cMaxSocket = m_ItemInfo.m_DetailData.m_cMaxSocketNum;
			InitializeGemAttribute();

			// Add the property back to Rune.
			enRuneApplyType	= static_cast<Item::CEquipment::RuneApplyType>(Item::CEquipment::RUNE_ALL | Item::CEquipment::RUNE_APPLY);

			SetRuneAttribute(enRuneApplyType, 0);
			lpSacrifice->SetRuneAttribute(enRuneApplyType, 0);

		// Grade / price recalculation
			CalculateItemGrade();
			CalculateItemPrice();

		// Third balance patch (S grade item limit line correction)
		//	RevisionLimit();

		// edith Set of new equipment.
			SetNewEquip();

			return EQUIPMENT_ERROR::S_SUCCESS; // 15 pos +
		}
		else 
		{
			InitializeGemAttribute();

			// Add the property back to Rune.
			enRuneApplyType	= static_cast<Item::CEquipment::RuneApplyType>(Item::CEquipment::RUNE_ALL | Item::CEquipment::RUNE_APPLY);

			SetRuneAttribute(enRuneApplyType, 0);
			lpSacrifice->SetRuneAttribute(enRuneApplyType, 0);

		// Grade / price recalculation
			CalculateItemGrade();
			CalculateItemPrice();

		// Third balance patch (S grade item limit line correction)
			RevisionLimit();

		// edith Set of new equipment.
			SetNewEquip();

			return EQUIPMENT_ERROR::S_SUCCESS; // 15 pos +
		}
}



/* ----------------
CEquipment::EQUIPMENT_ERROR CEquipment::OptionGraft(CEquipment* lpSacrifice, bool bUpgradeLevelLimit, unsigned char cAttributeType, 
													unsigned long dwCurrentGold, unsigned long& dwUsedGold)
{
	// �ɼ� �̽��� �ϱ����� �� �߰� �ɼ��� �����Ѵ�.
	Item::CEquipment::RuneApplyType	enRuneApplyType	= static_cast<Item::CEquipment::RuneApplyType>(Item::CEquipment::RUNE_ALL | Item::CEquipment::RUNE_REMOVE);

	SetRuneAttribute(enRuneApplyType, 0);
	lpSacrifice->SetRuneAttribute(enRuneApplyType, 0);

	if (false == CanOptionGraft(
		static_cast<CItemType::ArrayType>(CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags)), 
		static_cast<Attribute::Type>(cAttributeType)))
	{
		ERRLOG0(g_Log, "�ɼ� �̽� ���� : �ɼ� �̽��� �Ұ����� �ɼ��Դϴ�.");
		return EQUIPMENT_ERROR::E_WRONG_ATTRIBUTE;
	}

	if (m_ItemInfo.m_usProtoTypeID != lpSacrifice->GetPrototypeID())
	{
		ERRLOG0(g_Log, "�ɼ� �̽� ���� : ���� ������ �������� �ƴմϴ�.");
		//if the fuser is not iron
		if (lpSacrifice->GetPrototypeID() >98 && lpSacrifice->GetPrototypeID() <91 ){
			return EQUIPMENT_ERROR::E_NOT_EQUAL_KIND;
		}
		//if the original is not the same equip type as the fuser(iron)
		if (Item::CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags) != Item::CItemType::GetEquipType(lpSacrifice->GetItemInfo().m_DetailData.m_dwFlags))
		{return EQUIPMENT_ERROR::E_NOT_EQUAL_KIND;}				
		
	}

	/*if (m_GradeInfo.m_eItemGrade < lpSacrifice->GetItemGrade().m_eItemGrade)
	{
		ERRLOG0(g_Log, "�ɼ� �̽� ���� : ���� �������� �׷��̵尡 �� �����ϴ�.");
		return EQUIPMENT_ERROR::E_HIGH_GRADE_ORIGINAL;
	}*/
/* --------------------------
	// edith �ɼ��̽Ŀ� ���� ���� ���ϱ�
	dwUsedGold = GetGraftPrice();
	if (dwCurrentGold < dwUsedGold)
	{
		ERRLOG0(g_Log, "�ɼ� �̽� ���� : ���� �����մϴ�.");
		return EQUIPMENT_ERROR::E_NOT_ENOUGH_MONEY;
	}
/*
	// edith �ɼ��̽� ���� ���� �߰�. ���������� üũ�ؾ��Ѵ�.
	if( 30 < Math::Random::ComplexRandom(100)) 
	{
		// �ɼ��̽� ���� 
		// ���� �����޽����� ������� �ʴ´�.
		return EQUIPMENT_ERROR::E_GRAFE_FAILD;
	}
*/
/* --------------------------
	if (true == bUpgradeLevelLimit)
	{
		if (m_cUpgradeLevel != 0 && m_cUpgradeLevel != 10)
		{
			ERRLOG0(g_Log, "�ɼ� �̽� ���� : ���� ���׷��̵� ������ 0 �̰ų� 10 �ΰ͸� ����(����)");
			return EQUIPMENT_ERROR::E_NOT_UPGRADE_LEVEL;
		}

		if (lpSacrifice->m_cUpgradeLevel != 0 && lpSacrifice->m_cUpgradeLevel != 10)
		{
			ERRLOG0(g_Log, "�ɼ� �̽� ���� : ���� ���׷��̵� ������ 0 �̰ų� 10 �ΰ͸� ����(����)");
			return EQUIPMENT_ERROR::E_NOT_UPGRADE_LEVEL;
		}
	}
	//hz added this for fuser level 10 fuse
	if (GetPrototypeID() <= 98 && GetPrototypeID() >= 91 )
	{
		if (lpSacrifice->m_cUpgradeLevel != 10)
			{
				ERRLOG0(g_Log, "�ɼ� �̽� ���� : ���� ���׷��̵� ������ 0 �̰ų� 10 �ΰ͸� ����(����)");
				return EQUIPMENT_ERROR::E_NOT_UPGRADE_LEVEL;
			}
	}
	//hz end
	// ������ �ɼ� �̽�
	const Item::Attribute::Type eAttributeType = static_cast<Item::Attribute::Type>(cAttributeType);
	const short nAttributeValue = 
		((m_wAttribute[eAttributeType] + lpSacrifice->GetAttribute(eAttributeType) + 1) / 2) - m_wAttribute[eAttributeType];

	// edith 2009.03.15 min �������� max���� ���Ƶ� �ɼ��̽� ����
	// �ּ� �������϶� �ƽ� ���������� ���� Ŀ���� �Ǹ� 
	// �ɼ��̽Ŀ� �����ؾ��Ѵ�.
	if(eAttributeType == Attribute::MIN_DAMAGE)
	{
		// edith 2008.11.14 �ɼ��̽� MIN_DEMAGE ���׼���.
		// �ּ� �������� �ƽ� ���������� ũ�� 
		// �̽� �Ұ����� �ɼ��̶�� �޽����� ����. �Ŀ� �޽����� ����ȭ����.
		if(m_wAttribute[Attribute::MIN_DAMAGE]+nAttributeValue > m_wAttribute[Item::Attribute::MAX_DAMAGE])
		{
			ERRLOG0(g_Log, "�ɼ� �̽� ���� : MinDemage �� MaxDemage ���� �����ϴ�. �ɼ��̽Ŀ� �����Ͽ����ϴ�.");
			return EQUIPMENT_ERROR::E_GRAFE_FAILD;
		}
	}

	AddAttribute(eAttributeType, nAttributeValue);

	// edith �ɼ��̽Ľ� ���ôܰ� �ʱ�ȭ�� ���� �ʴ´�.
	// ���� �ܰ�� �ʱ�ȭ�ȴ�. (�ɼ��� ���´�.)
//	m_cUpgradeLevel = 0;
//	InitializeUpgradeAttribute();   

/*
	// edith �ɼ��̽Ľ� ������ �ʱ�ȭ �ϸ� �ȵȴ�. 
	// ���� ������ ���������� ������ �ʱ�ȭ�ϸ� �ȵ�.
	// ���ϵ� �ʱ�ȭ�ȴ�. (������ ��� ���ŵ����� �ɼ��� ���´�.)
	m_cSocketNum = 0;
	std::fill_n(m_cSocket, unsigned char(Item::EquipmentInfo::MAX_SOCKET_NUM), 0);
	m_cMaxSocket = m_ItemInfo.m_DetailData.m_cMaxSocketNum;
*/
/* ----------------------------------
	InitializeGemAttribute();

	// �� �Ӽ��� �ٽ� �߰��Ѵ�.
	enRuneApplyType	= static_cast<Item::CEquipment::RuneApplyType>(Item::CEquipment::RUNE_ALL | Item::CEquipment::RUNE_APPLY);

	SetRuneAttribute(enRuneApplyType, 0);
	lpSacrifice->SetRuneAttribute(enRuneApplyType, 0);

	// �׷��̵�/���� ����
	CalculateItemGrade();
	CalculateItemPrice();

	// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
	RevisionLimit();

	// edith ���ο� ���� ����.
	SetNewEquip();

	return EQUIPMENT_ERROR::S_SUCCESS;
}
*/

//bool Item::CEquipment::CheckAttributeLimit(Item::Attribute::Type eAttributeType, short nAttributeValue, unsigned char cType)
//{
//	const CItemType::ArrayType eEquipType = CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags);
//
//	const float fGradeGap = 
//		Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::AAA_GRADE, eAttributeType) - 
//		Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::AA_GRADE, eAttributeType);
//
//	unsigned short wLimitMagnification = 4;	
//
//	if(!cType)
//	{
//		switch (eAttributeType)
//		{
//		case Attribute::MIN_DAMAGE:		wLimitMagnification = 35;	break;
//		case Attribute::MAX_DAMAGE:		wLimitMagnification = 35;	break;
//		case Attribute::ARMOR:			wLimitMagnification = 10;	break;
//		case Attribute::HIT_RATE:		wLimitMagnification = 10;	break;
//		case Attribute::EVADE:			wLimitMagnification = 10;	break;
//		case Attribute::MAX_HP:			wLimitMagnification = 20;	break;
//		case Attribute::HP_REGEN:		wLimitMagnification = 10;	break;
//		case Attribute::MAX_MP:			wLimitMagnification = 10;	break;
//		case Attribute::MP_REGEN:		wLimitMagnification = 10;	break;
//		case Attribute::CRITICAL:		wLimitMagnification = 15;	break;
//		case Attribute::BLOCK:			wLimitMagnification = 10;	break;
//		case Attribute::MAGIC_POWER:	wLimitMagnification = 10;	break;
//		case Attribute::MAGIC_RESIST:	wLimitMagnification = 10;	break;
//
//		default:						wLimitMagnification = 10;	break;
//		}
//
//		// ��Ŭ���� ��� ���� ó�� �ʿ�
//		if (Item::EtcItemID::BUCKLER == GetPrototypeID())
//
//
//		{
//			switch (eAttributeType)
//			{
//			case Attribute::MAX_HP:			wLimitMagnification = 330;	break;
//			case Attribute::HP_REGEN:		wLimitMagnification = 33;	break;
//			case Attribute::MAX_MP:			wLimitMagnification = 33;	break;
//			case Attribute::MP_REGEN:		wLimitMagnification = 33;	break;
//			case Attribute::BLOCK:			wLimitMagnification = 50;	break;
//			}
//		}
//	}
//
//	// �ɼǺ� ���� ��ġ �׷��̵� ��
//	const float fLimitGradeValue = 
//		Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::AAA_GRADE, eAttributeType) + (fGradeGap * wLimitMagnification);
//
//	// �ɼǺ� ���� ��
//	const float fLimitAttributeValue = m_ItemInfo.m_DetailData.m_wDropOption * fLimitGradeValue;
//
//	// ���� �� �ɼ� ��
//	const unsigned short wCurrnetAttributeValue = 
//		m_wAttribute[eAttributeType] + nAttributeValue - m_ItemInfo.m_EquipAttribute.m_usAttributeValue[eAttributeType];
//
//	if (0 != wCurrnetAttributeValue && wCurrnetAttributeValue >= fLimitAttributeValue)
//	{
//		// ���ѿ� �ɸ��� ���� �������� ������ �پ���.
//		return true;
//	}
//
//	return false;
//}

bool Item::CEquipment::CheckAttributeLimit(Item::Attribute::Type eAttributeType, unsigned char cType)
{
	short sattrLimit = 0, sattrFactor = 0;
	GetLimitValue(eAttributeType, sattrLimit);
	GetSubRuneAttribute(eAttributeType, sattrFactor);

	return (0 != sattrFactor && sattrFactor >= sattrLimit);		
}
// ryl2 avalansa
bool CEquipment::CanOptionGraft(CItemType::ArrayType eItemType, Attribute::Type eAttributeType)
{
	switch (eItemType)
	{
	case CItemType::ARMOUR_TYPE:		

		// edith 2008.01.14 �ɼ��̽� ���� ����.
	case CItemType::HELM_TYPE:		
		if (Attribute::ARMOR == eAttributeType || 
			Attribute::EVADE == eAttributeType || 
			Attribute::MAX_HP == eAttributeType || 
			Attribute::HP_REGEN == eAttributeType || 
//			Attribute::MAX_MP == eAttributeType ||   // disable by shadow
			Attribute::MAGIC_RESIST == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::DAGGER_TYPE:		
		if (Attribute::MIN_DAMAGE == eAttributeType || 
			Attribute::MAX_DAMAGE == eAttributeType || 
			Attribute::HIT_RATE == eAttributeType ||
			Attribute::MAX_MP == eAttributeType ||   // add by shadow
			Attribute::MP_REGEN == eAttributeType ||   // add by shadow
			Attribute::CRITICAL == eAttributeType ||
			Attribute::BLOCK == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::ONEHANDED_TYPE:	
		if (Attribute::MIN_DAMAGE == eAttributeType || 
			Attribute::MAX_DAMAGE == eAttributeType || 
			Attribute::HIT_RATE == eAttributeType ||
			Attribute::MAX_MP == eAttributeType ||   // add by shadow
			Attribute::MP_REGEN == eAttributeType ||   // add by shadow
			Attribute::CRITICAL == eAttributeType ||
			Attribute::BLOCK == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::LONGRANGE_TYPE:
	case CItemType::TWOHANDED_TYPE:	
		if (Attribute::MIN_DAMAGE == eAttributeType || 
			Attribute::MAX_DAMAGE == eAttributeType || 
			Attribute::HIT_RATE == eAttributeType ||
			Attribute::MAX_MP == eAttributeType ||   // add by shadow
			Attribute::MP_REGEN == eAttributeType ||   // add by shadow
			Attribute::CRITICAL == eAttributeType ||
			Attribute::BLOCK == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::STAFF_TYPE:		
		if (Attribute::MAGIC_POWER == eAttributeType || 
			Attribute::MAX_DAMAGE == eAttributeType || 
			Attribute::HIT_RATE == eAttributeType || 
			Attribute::MAX_MP == eAttributeType || 
			Attribute::MP_REGEN == eAttributeType || 
			Attribute::CRITICAL == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::SHIELD_TYPE:		
		if (Attribute::MAX_HP == eAttributeType || 
			Attribute::HP_REGEN == eAttributeType || 
			Attribute::MAX_MP == eAttributeType || 
			Attribute::MP_REGEN == eAttributeType || 
			Attribute::BLOCK == eAttributeType)
		{
			return true;
		}
		break;
	}

	return false;
}

/*
bool CEquipment::CanOptionGraft(CItemType::ArrayType eItemType, Attribute::Type eAttributeType)
{
	switch (eItemType)
	{
	case CItemType::ARMOUR_TYPE:		

		// edith 2008.01.14 �ɼ��̽� ���� ����.
	case CItemType::HELM_TYPE:		
		if (Attribute::ARMOR == eAttributeType || 
			Attribute::EVADE == eAttributeType || 
			Attribute::MAX_HP == eAttributeType || 
			Attribute::HP_REGEN == eAttributeType || 
			Attribute::MAX_MP == eAttributeType || 
			Attribute::MAGIC_RESIST == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::DAGGER_TYPE:		
		if (Attribute::MIN_DAMAGE == eAttributeType || 
			Attribute::MAX_DAMAGE == eAttributeType || 
			Attribute::HIT_RATE == eAttributeType || 
			Attribute::BLOCK == eAttributeType || 
			Attribute::CRITICAL == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::ONEHANDED_TYPE:	
		if (Attribute::MIN_DAMAGE == eAttributeType || 
			Attribute::MAX_DAMAGE == eAttributeType || 
			Attribute::HIT_RATE == eAttributeType || 
			Attribute::BLOCK == eAttributeType || 
			Attribute::CRITICAL == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::LONGRANGE_TYPE:
	case CItemType::TWOHANDED_TYPE:	
		if (Attribute::MIN_DAMAGE == eAttributeType || 
			Attribute::MAX_DAMAGE == eAttributeType || 
			Attribute::HIT_RATE == eAttributeType || 
			Attribute::BLOCK == eAttributeType || 
			Attribute::CRITICAL == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::STAFF_TYPE:		
		if (Attribute::MAGIC_POWER == eAttributeType || 
			Attribute::MAX_DAMAGE == eAttributeType || 
			Attribute::HIT_RATE == eAttributeType || 
			Attribute::MAX_MP == eAttributeType || 
			Attribute::MP_REGEN == eAttributeType || 
			Attribute::CRITICAL == eAttributeType)
		{
			return true;
		}
		break;

	case CItemType::SHIELD_TYPE:		
		if (Attribute::MAX_HP == eAttributeType || 
			Attribute::HP_REGEN == eAttributeType || 
			Attribute::MAX_MP == eAttributeType || 
			Attribute::MP_REGEN == eAttributeType || 
			Attribute::BLOCK == eAttributeType)
		{
			return true;
		}
		break;
	}

	return false;
}
*/
CEquipment::EQUIPMENT_ERROR CEquipment::Compensation(unsigned char cClass, unsigned char cLevel, CharacterStatus status, 
													 CEquipment** lppResultEquip, long& lCompensationGold)
{
	if (0 != m_cSeasonRecord)
	{
		// ���Ӱ� ����� �������̰ų� �̹� ���� �ǸŸ� ���� �������Դϴ�. 
		return CEquipment::E_NEW_SEASON_EQUIP;
	}

	if (0 == m_ItemInfo.m_DetailData.m_wDropOption)
	{
		// ���� �Ǹ� ��� ������ �������� �ƴմϴ�.
		return CEquipment::E_WRONG_TYPE;
	}

	unsigned char cCompensationType = 0;
	short wNeedStatusValue = 0;
	StatusLimit::Type eNeedStatusType = StatusLimit::NONE;

	// Ư�� ���̽�
	switch (m_ItemInfo.m_DetailData.m_cItemType)
	{
	case ItemType::SHIELD:
		{
			if (CClass::Defender != cClass && CClass::Cleric != cClass)
			{
				cCompensationType = Compensation::CASE1;
				eNeedStatusType = StatusLimit::CON;
			}
			break;
		}

	case ItemType::CON_ARMOUR:
		{
			if (CClass::Defender == cClass || CClass::Warrior == cClass || CClass::Fighter == cClass)
			{
				cCompensationType = Compensation::CASE2;
				eNeedStatusType = StatusLimit::CON;
				break;
			}

			if (CClass::Priest == cClass || CClass::Cleric == cClass || CClass::Acolyte == cClass)
			{
				cCompensationType = Compensation::CASE3;
				eNeedStatusType = StatusLimit::CON;
				break;
			}
			break;
		}

	case ItemType::DEX_ARMOUR:
		{
			if (CClass::Sorcerer == cClass || CClass::Enchanter == cClass || CClass::Mage == cClass)
			{
				cCompensationType = Compensation::CASE4;
				eNeedStatusType = StatusLimit::DEX;
				break;
			}

			if (CClass::Assassin == cClass || CClass::Archer == cClass || CClass::Rogue == cClass)
			{
				cCompensationType = Compensation::CASE6;
				eNeedStatusType = StatusLimit::DEX;
				break;
			}
			break;
		}

	case ItemType::DEX_HELM:
		{
			if (CClass::Sorcerer == cClass || CClass::Enchanter == cClass || CClass::Mage == cClass)
			{
				cCompensationType = Compensation::CASE5;
				eNeedStatusType = StatusLimit::DEX;
				break;
			}

			if (CClass::Assassin == cClass || CClass::Archer == cClass || CClass::Rogue == cClass)
			{
				cCompensationType = Compensation::CASE7;
				eNeedStatusType = StatusLimit::DEX;
				break;
			}
			break;
		}

		// 

	case ItemType::DEX_HEAD:
		{
			if (CClass::RuneOff == cClass || CClass::LifeOff == cClass)
			{
				cCompensationType = Compensation::CASE8;
				eNeedStatusType = StatusLimit::DEX;
				break;
			}

			if (CClass::Officiator == cClass || CClass::ShadowOff == cClass)
			{
				cCompensationType = Compensation::CASE9;
				eNeedStatusType = StatusLimit::DEX;
				break;
			}
			break;
		}

	case ItemType::ONEHANDED_SWORD:
	case ItemType::ONEHANDED_AXE:
		{
			if (status.m_nSTR >= status.m_nCON)
			{
				cCompensationType = ItemType::ONEHANDED_SWORD;
				eNeedStatusType = StatusLimit::STR;
			}
			else
			{
				cCompensationType = ItemType::ONEHANDED_AXE;
				eNeedStatusType = StatusLimit::CON;
			}
			break;
		}

	case ItemType::TWOHANDED_SWORD:
	case ItemType::TWOHANDED_AXE:
		{
			if (status.m_nSTR >= status.m_nCON)
			{
				cCompensationType = ItemType::TWOHANDED_SWORD;
				eNeedStatusType = StatusLimit::STR;
			}
			else
			{
				cCompensationType = ItemType::TWOHANDED_AXE;
				eNeedStatusType = StatusLimit::CON;
			}
			break;
		}

	case ItemType::ONEHANDED_BLUNT:
		{
			if (status.m_nCON >= status.m_nWIS)
			{
				cCompensationType = ItemType::ONEHANDED_AXE;
				eNeedStatusType = StatusLimit::CON;
			}
			else
			{
				cCompensationType = ItemType::ONEHANDED_BLUNT;
				eNeedStatusType = StatusLimit::WIS;
			}
			break;
		}

	case ItemType::TWOHANDED_BLUNT:
		{
			if (status.m_nCON >= status.m_nWIS)
			{
				cCompensationType = ItemType::TWOHANDED_AXE;
				eNeedStatusType = StatusLimit::CON;
			}
			else
			{
				cCompensationType = ItemType::TWOHANDED_BLUNT;
				eNeedStatusType = StatusLimit::WIS;
			}
			break;
		}

	case ItemType::COM_SWORD:
	case ItemType::COM_BLUNT:
		{
			if (status.m_nSTR >= status.m_nCON)
			{
				cCompensationType = ItemType::COM_SWORD;
				eNeedStatusType = StatusLimit::STR;
			}
			else
			{
				cCompensationType = ItemType::COM_BLUNT;
				eNeedStatusType = StatusLimit::CON;
			}
			break;
		}

	case ItemType::OPP_SLUSHER:
	case ItemType::OPP_AXE:
		{
			if (status.m_nSTR >= status.m_nCON)
			{
				cCompensationType = ItemType::OPP_SLUSHER;
				eNeedStatusType = StatusLimit::STR;
			}
			else
			{
				cCompensationType = ItemType::OPP_AXE;
				eNeedStatusType = StatusLimit::CON;
			}
			break;
		}
	}

	if (0 == cCompensationType)
	{
		cCompensationType = m_ItemInfo.m_DetailData.m_cItemType;
		eNeedStatusType = static_cast<StatusLimit::Type>(m_ItemInfo.m_UseLimit.m_cLimitStatus);
	}

	if (m_ItemInfo.m_UseLimit.m_wLimitValue <= 
		ClassTable[cClass].GetMinState(static_cast<CClass::StatusType>(eNeedStatusType), cLevel) / 2)
	{
		// ĳ������ �������ͽ��� ���� ���� ������ ����Դϴ�.
		return CEquipment::E_LOW_STATUS_EQUIP;
	}

	switch (eNeedStatusType)
	{
	case StatusLimit::STR:		wNeedStatusValue = status.m_nSTR;	break;
	case StatusLimit::DEX:		wNeedStatusValue = status.m_nDEX;	break;
	case StatusLimit::CON:		wNeedStatusValue = status.m_nCON;	break;
	case StatusLimit::INT:		wNeedStatusValue = status.m_nINT;	break;
	case StatusLimit::WIS:		wNeedStatusValue = status.m_nWIS;	break;
	case StatusLimit::LEVEL:	wNeedStatusValue = cLevel;			break;	//--//
	}

	unsigned short wCompensationItem = 
		CItemMgr::GetInstance().GetCompensationItem(cCompensationType, wNeedStatusValue);

	if (0 == wCompensationItem)
	{
		// ���� �Ǹ� ��� ������ �������� �ƴմϴ�.
		return CEquipment::E_WRONG_TYPE;
	}

	Item::CEquipment* lpEquip = reinterpret_cast<CEquipment *>(CItemFactory::GetInstance().CreateItem(wCompensationItem));
	if (NULL == lpEquip)
	{
		return CEquipment::E_SERVER_ERROR;
	}

	lpEquip->m_ItemData.m_cNumOrDurability = m_ItemData.m_cNumOrDurability;
	lpEquip->m_cMaxNumOrDurability = m_cMaxNumOrDurability;

	for (unsigned char cAttributeIndex = 0; cAttributeIndex < Attribute::MAX_ATTRIBUTE_NUM; ++cAttributeIndex)
	{
		unsigned short wAttributeValue = 
			(GetAttribute(static_cast<Attribute::Type>(cAttributeIndex)) - m_ItemInfo.m_EquipAttribute.m_usAttributeValue[cAttributeIndex]) * 
			lpEquip->GetItemInfo().m_DetailData.m_wDropOption / m_ItemInfo.m_DetailData.m_wDropOption;

		lpEquip->AddAttribute(static_cast<Attribute::Type>(cAttributeIndex), wAttributeValue);
	}

	lpEquip->CalculateItemGrade();

	const Grade::GradeInfo originalGrade = GetItemGrade();
	const Grade::GradeInfo resultGrade = lpEquip->GetItemGrade();

	for (unsigned char cAttributeIndex = 0; cAttributeIndex < Attribute::MAX_ATTRIBUTE_NUM; ++cAttributeIndex)
	{
		if (resultGrade.m_aryAttributeGrade[cAttributeIndex] < originalGrade.m_eItemGrade)
		{
			const Item::CItemType::ArrayType eEquipType = CItemType::GetEquipType(lpEquip->GetItemInfo().m_DetailData.m_dwFlags);

			const short wMaxValue = static_cast<short>(m_ItemInfo.m_DetailData.m_wDropOption * 
				Grade::GetGradeValue(eEquipType, Grade::DROP_MAX_VALUE, originalGrade.m_eItemGrade, static_cast<Attribute::Type>(cAttributeIndex)));

			lpEquip->AddAttribute(static_cast<Attribute::Type>(cAttributeIndex), 
				wMaxValue - lpEquip->GetAttribute(static_cast<Attribute::Type>(cAttributeIndex)));
		}
	}

	lpEquip->CalculateItemGrade();
	lpEquip->CalculateItemPrice();

	// Rodin : ��а��� ���� �ǸŸ� ���������� �̿��� �� �ִ�.
	//	SetNewEquip();

	const unsigned long dwOriginalGold = GetBuyPrice();
	const unsigned long dwResultGold = lpEquip->GetBuyPrice();

	lCompensationGold = static_cast<long>(dwOriginalGold - dwResultGold);

	*lppResultEquip = lpEquip;
	return CEquipment::S_SUCCESS;
}

unsigned long CEquipment::GetBuyPrice(void) const
{
	if (0 != m_dwStallPrice) 
	{ 
		return m_dwStallPrice; 
	}

	unsigned long dwPrice = CItem::GetBuyPrice();
	if (0 == m_cSeasonRecord)
	{
		if (m_GradeInfo.m_eItemGrade != EquipType::D_GRADE && 
			m_GradeInfo.m_eItemGrade != EquipType::F_GRADE &&
			m_GradeInfo.m_eItemGrade != EquipType::X_GRADE)	//--//
		{
			dwPrice *= PRICE_AGGRAVATION;
		}
	}

	return dwPrice;
}

unsigned long CEquipment::GetSellPrice(void) const
{
	if (0 != m_dwStallPrice) 
	{ 
		return m_dwStallPrice; 
	}

	unsigned long dwPrice = CItem::GetSellPrice();

	if (0 == m_cSeasonRecord)
	{
		if (m_GradeInfo.m_eItemGrade != EquipType::D_GRADE && 
			m_GradeInfo.m_eItemGrade != EquipType::F_GRADE &&
			m_GradeInfo.m_eItemGrade != EquipType::X_GRADE)	//--//
		{
			dwPrice *= PRICE_AGGRAVATION;
		}
	}

	return dwPrice;
}

unsigned long CEquipment::GetRepairPrice(void) const
{
	unsigned long dwPrice = CItem::GetRepairPrice();
	if (0 == m_cSeasonRecord)
	{
		switch(m_GradeInfo.m_eItemGrade)
		{
		case EquipType::D_GRADE:
			dwPrice*=3;
			break;

		case EquipType::C_GRADE:
			dwPrice*=5;
			break;

		case EquipType::B_GRADE:
			dwPrice*=7;
			break;

		case EquipType::A_GRADE:
			dwPrice*=9;
			break;

		case EquipType::AA_GRADE:
			dwPrice*=11;
			break;

		case EquipType::AAA_GRADE:
			dwPrice*=13;
			break;

		case EquipType::S_GRADE:
			dwPrice*=15;
			break;

		default:
			dwPrice*=2;
			break;
		}
		/*if (m_GradeInfo.m_eItemGrade != EquipType::D_GRADE && 
			m_GradeInfo.m_eItemGrade != EquipType::F_GRADE &&
			m_GradeInfo.m_eItemGrade != EquipType::X_GRADE)	//--//
		{
			dwPrice *= PRICE_AGGRAVATION;
		}*/
	}

	return dwPrice;
}

// ring of war code upgrade price
/*
unsigned long CEquipment::GetUpgradePrice(void) const
{
	unsigned long dwPrice = CItem::GetUpgradePrice();
	if (0 == m_cSeasonRecord)
	{
		if (m_GradeInfo.m_eItemGrade != EquipType::D_GRADE && 
			m_GradeInfo.m_eItemGrade != EquipType::F_GRADE &&
			m_GradeInfo.m_eItemGrade != EquipType::X_GRADE)	//--//
		{
			dwPrice *= PRICE_AGGRAVATION;
		}
	}
	unsigned long upgradePrice=static_cast<float>(dwPrice*0.25f);
	return dwPrice;
	//return upgradePrice;
}
*/
/*
// original code upgrade price
unsigned long CEquipment::GetUpgradePrice(void) const
{
	unsigned long dwPrice = CItem::GetUpgradePrice();
	if (0 == m_cSeasonRecord)
	{
		if (m_GradeInfo.m_eItemGrade != EquipType::D_GRADE && 
			m_GradeInfo.m_eItemGrade != EquipType::F_GRADE &&
			m_GradeInfo.m_eItemGrade != EquipType::X_GRADE)	//--//
		{
			dwPrice *= PRICE_AGGRAVATION;
		}
	}

	return dwPrice;
}
*/

unsigned long CEquipment::GetUpgradePrice(void) const
{
	unsigned long dwPrice = CItem::GetUpgradePrice();
	if (0 == m_cSeasonRecord)
	{
		switch(m_GradeInfo.m_eItemGrade)
		{
		case EquipType::D_GRADE:
			dwPrice*=30;
			break;

		case EquipType::C_GRADE:
			dwPrice*=50;
			break;

		case EquipType::B_GRADE:
			dwPrice*=70;
			break;

		case EquipType::A_GRADE:
			dwPrice*=90;
			break;

		case EquipType::AA_GRADE:
			dwPrice*=110;
			break;

		case EquipType::AAA_GRADE:
			dwPrice*=130;
			break;

		case EquipType::S_GRADE:
			dwPrice*=150;
			break;

		default:
			dwPrice*=200;
			break;
		}
	}

	return dwPrice;
}

unsigned long CEquipment::GetGraftPrice(void) const
{
	unsigned long dwPrice = CItem::GetGraftPrice();
	if (0 == m_cSeasonRecord)
	{
		if (m_GradeInfo.m_eItemGrade != EquipType::D_GRADE && 
			m_GradeInfo.m_eItemGrade != EquipType::F_GRADE &&
			m_GradeInfo.m_eItemGrade != EquipType::X_GRADE)	//--//
		{
			dwPrice *= PRICE_AGGRAVATION;
		}
	}

	return dwPrice;
}

void CEquipment::CalculateItemGrade(void)
{
	if (0 == m_ItemInfo.m_DetailData.m_wDropOption)	{ return; }

	// �������� �׷��̵� ������ �ʱ�ȭ
	m_GradeInfo = Grade::GradeInfo();	

	ApplyRuneAttribute(REMOVE);

	for (unsigned char cAttributeIndex = 0; cAttributeIndex < Attribute::MAX_ATTRIBUTE_NUM; ++cAttributeIndex)
	{
		const short sAttrFactor = GetAttribute(static_cast<Attribute::Type>(cAttributeIndex));

		const float fGradeFactor = 
			(sAttrFactor - m_ItemInfo.m_EquipAttribute.m_usAttributeValue[cAttributeIndex]) / 
			static_cast<float>(m_ItemInfo.m_DetailData.m_wDropOption);

		// D �׷��̵带 �ʰ��ϸ� F �׷��̵尡 �ȴ�.
		for (unsigned char cGradeIndex = 0; cGradeIndex <= EquipType::D_GRADE; ++cGradeIndex)
		{
			const Item::CItemType::ArrayType eEquipType = CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags);
			short sAttrValue = 0;

			// 3�� �뷱�� ��ġ
			// S �׷��̵� ���Ѱ� �������� ���������� ����Ǿ� �ش� �κп����� S �׷��̵� ���Ѱ� ������ ����ó���Ѵ�.
			if(EquipType::S_GRADE == static_cast<EquipType::Grade>(cGradeIndex)) // was S_GRADE 
			{
				GetLimitValue(static_cast<Attribute::Type>(cAttributeIndex), sAttrValue);
				if(0 < sAttrValue)
				{
					--sAttrValue;
				}
			}

			const float fItemFactor = (EquipType::S_GRADE == static_cast<EquipType::Grade>(cGradeIndex)) ? // was S_GRADE 
				static_cast<const float>(sAttrFactor) : fGradeFactor;				

			const float fItemValue = (EquipType::S_GRADE == static_cast<EquipType::Grade>(cGradeIndex)) ? // was S_GRADE 
				static_cast<const float>(sAttrValue) :
			Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, 
				static_cast<EquipType::Grade>(cGradeIndex), static_cast<Attribute::Type>(cAttributeIndex));

			if (0 != fItemValue && fItemFactor > fItemValue)
			{
				m_GradeInfo.m_aryAttributeGrade[cAttributeIndex] = static_cast<EquipType::Grade>(cGradeIndex);

				if (m_GradeInfo.m_eItemGrade > m_GradeInfo.m_aryAttributeGrade[cAttributeIndex])
				{
					m_GradeInfo.m_eItemGrade = m_GradeInfo.m_aryAttributeGrade[cAttributeIndex];
					m_GradeInfo.m_cPlus = 0;
				}
				else
				{
					if (m_GradeInfo.m_eItemGrade == m_GradeInfo.m_aryAttributeGrade[cAttributeIndex])
					{
						++m_GradeInfo.m_cPlus;
					}
				}
				break;
			}
		}
	}

	ApplyRuneAttribute(APPLY);
}

void CEquipment::CalculateItemPrice(void)
{
	// edith ������ ���� ����

	// -------------------------------------------------------------------------------------------------
	// ���� å�� ���̺�
	//--//	start..
	static const float aryPriceRateTable[Attribute::MAX_ATTRIBUTE_NUM] = {
//				MinDam.	MaxDam.	Armor	HitRate	Evade	MaxHP	HPRegen	MaxMP	MPRegen	Cri.	Block	Speed	M.Power	M.Res.	Rune	CoolDw.	SkillPt	Frost	Fire	Electro	Dark
		0,		2,		2,		2,		2,		2,		1,		1,		1,		1,		1,		2,		0,		1,		1,		0,		0,		0,		0,		0,		0,		0,
	};
	//--//	end..

//	static const float aryGradeRateTable[EquipType::MAX_GRADE] = {
	//	S		AAA		AA		A		B		C		D		F		X
//		3.0f,	2.4f,	1.8f,	1.4f,	1.2f,	1.15f,	1.1f,	1,		1
//	};

		static const float aryGradeRateTable[EquipType::MAX_GRADE] = {
	//	S		AAA		AA		A		B		C		D		F		X
		50.0f,	38.0f,	32.0f,	26.0f,	20.0f,	15.5f,	11.0f,	1,		1
	};

	// -------------------------------------------------------------------------------------------------

	if (true == m_ItemInfo.m_DetailData.m_bOptionPrice)
	{
/*
		// �������� ��� ��հ��� �̿��Ѵ�.
		float fPriceFactor = 1;//((GetAttribute(Attribute::MIN_DAMAGE) + GetAttribute(Attribute::MAX_DAMAGE)) / 2.0f - 10) * aryPriceRateTable[Attribute::MIN_DAMAGE];
		// fPriceFactor = max(fPriceFactor, 0.0f);

		for (unsigned char cAttributeIndex = 0; cAttributeIndex < Attribute::MAX_ATTRIBUTE_NUM; ++cAttributeIndex)
		{
			if (Attribute::MIN_DAMAGE != cAttributeIndex && Attribute::MAX_DAMAGE != cAttributeIndex)
			{
				fPriceFactor += GetAttribute(static_cast<Attribute::Type>(cAttributeIndex)) * aryPriceRateTable[cAttributeIndex];
			}
		}

		m_dwPrice = static_cast<unsigned long>(((fPriceFactor * fPriceFactor) + 10) * aryGradeRateTable[m_GradeInfo.m_eItemGrade])/2 + m_ItemInfo.m_DetailData.m_dwPrice;
*/
		float fPriceFactor = 
			((GetAttribute(Attribute::MIN_DAMAGE) + GetAttribute(Attribute::MAX_DAMAGE)) / 2.0f - 10) * 
			aryPriceRateTable[Attribute::MIN_DAMAGE];

		fPriceFactor = max(fPriceFactor, 0.0f);

		for (unsigned char cAttributeIndex = 0; cAttributeIndex < Attribute::MAX_ATTRIBUTE_NUM; ++cAttributeIndex)
		{
			if (Attribute::MIN_DAMAGE != cAttributeIndex && Attribute::MAX_DAMAGE != cAttributeIndex)
			{
				fPriceFactor += GetAttribute(static_cast<Attribute::Type>(cAttributeIndex)) * aryPriceRateTable[cAttributeIndex];
			}
		}

		m_dwPrice = static_cast<unsigned long>(((fPriceFactor /** fPriceFactor*/) + 10) * aryGradeRateTable[m_GradeInfo.m_eItemGrade]);
		// ����� �⺻������ �����ش�.
		m_dwPrice += m_ItemInfo.m_DetailData.m_dwPrice;
	}
}

// ���׷��̵� ���� ���� ����
bool CEquipment::IsUpgradable(void) const
{
	switch(m_ItemInfo.m_DetailData.m_cItemType)
	{
		// �ΰ� ��
	case Item::ItemType::CON_ARMOUR:
	case Item::ItemType::DEX_ARMOUR:

		// �ΰ� ����
	case Item::ItemType::ONEHANDED_SWORD:
	case Item::ItemType::TWOHANDED_SWORD:
	case Item::ItemType::ONEHANDED_AXE:
	case Item::ItemType::TWOHANDED_AXE:
	case Item::ItemType::ONEHANDED_BLUNT:
	case Item::ItemType::TWOHANDED_BLUNT:
	case Item::ItemType::BOW:
	case Item::ItemType::CROSSBOW:
	case Item::ItemType::STAFF:
	case Item::ItemType::DAGGER:
	case Item::ItemType::SHIELD:

		// ��ĭ �� 
	case Item::ItemType::CON_BODY:
	case Item::ItemType::DEX_BODY:

		// ��ĭ ����
	case Item::ItemType::COM_BLUNT:
	case Item::ItemType::COM_SWORD:
	case Item::ItemType::OPP_HAMMER:
	case Item::ItemType::OPP_AXE:
	case Item::ItemType::OPP_SLUSHER:
	case Item::ItemType::OPP_TALON:
	case Item::ItemType::OPP_SYTHE:

		// ��ų��
	case Item::ItemType::SKILL_A_GUARD:
	case Item::ItemType::SKILL_A_ATTACK:
	case Item::ItemType::SKILL_A_GUN:
	case Item::ItemType::SKILL_A_KNIFE:
// disable by avalansa to make ryl2
/*
	// edith 2008.01.14 ���׷��̵� �������� �߰�
	case Item::ItemType::CON_HELM:
	case Item::ItemType::DEX_HELM:
	case Item::ItemType::CON_HEAD:
	case Item::ItemType::DEX_HEAD:

	case Item::ItemType::CON_GLOVE:
	case Item::ItemType::DEX_GLOVE:
	case Item::ItemType::CON_BOOTS:
	case Item::ItemType::DEX_BOOTS:
	case Item::ItemType::CON_PELVIS:
	case Item::ItemType::DEX_PELVIS:
	case Item::ItemType::CON_PROTECT_A:
	case Item::ItemType::DEX_PROTECT_A:
*/
		return true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------------------------
// 3�� �뷱�� ��ġ ���� �Լ�

// S �׷��̵� ���Ѱ� ���ϱ�
bool CEquipment::GetLimitValue(short* lpattrLimit)
{
	short attrLimit[Item::Attribute::MAX_ATTRIBUTE_NUM] = {0};

	for(unsigned char cAttributeIndex = 0; 
		cAttributeIndex < Item::Attribute::MAX_ATTRIBUTE_NUM;
		++cAttributeIndex)
	{
		GetLimitValue(static_cast<Item::Attribute::Type>(cAttributeIndex), attrLimit[cAttributeIndex]);
	}

	memcpy(lpattrLimit, attrLimit, sizeof(short) * Item::Attribute::MAX_ATTRIBUTE_NUM);

	return true;
}
// ryl2 avalansa
bool CEquipment::GetLimitValue(Item::Attribute::Type eAttributeType, short& attrLimit)
{
	const CItemType::ArrayType eEquipType = CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags);
	const Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>(GetItemInfo().m_DetailData.m_cItemType);

	const float fMaxGrade = Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::AAA_GRADE, eAttributeType);
	const float fGradeGap = fMaxGrade - Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::AA_GRADE, eAttributeType);

	// ���� ���� ����
	unsigned short wLimitMagnification = 1;

	switch (eItemType)
	{
// original shadow
		/*
	case Item::ItemType::NECKLACE:			// �Ǽ��縮(�����)
	case Item::ItemType::RING:				// �Ǽ��縮(����)
	case Item::ItemType::RUNE:				// �Ǽ��縮(��)
	// �Ǽ��縮 �������� ��ũ��Ʈ�� ������ ������ ���ѵȴ�.
	*/
		// new by shadow
		//--
	case Item::ItemType::NECKLACE:
	case Item::ItemType::RING:				// �Ǽ��縮(����)
	case Item::ItemType::RUNE:				// �Ǽ��縮(��)
		//--
		attrLimit = static_cast<short>(m_ItemInfo.m_EquipAttribute.m_usAttributeValue[eAttributeType]);
		return true;
	case Item::ItemType::CON_ARMOUR:		// �ΰ� ��(CON ����)
	case Item::ItemType::CON_BODY:			// ��ĭ ��(CON ����)
	case Item::ItemType::DEX_BODY:			// ��ĭ ��(DEX ����)
		switch (eAttributeType)
		{
		case Attribute::ARMOR:			wLimitMagnification =  5;	break;
		case Attribute::EVADE:			wLimitMagnification =  5;	break;
		case Attribute::MAX_HP:			wLimitMagnification = 10;	break;
		case Attribute::HP_REGEN:		wLimitMagnification =  5;	break;
//		case Attribute::MAX_MP:			wLimitMagnification = 10;	break;  // disable by shadow
		case Attribute::MAGIC_RESIST:	wLimitMagnification =  5;	break;
		}
		break;
	case Item::ItemType::ONEHANDED_SWORD:	// �ΰ� ����(�Ѽհ�)
	case Item::ItemType::TWOHANDED_SWORD:	// �ΰ� ����(��հ�)
	case Item::ItemType::ONEHANDED_BLUNT:	// �ΰ� ����(�Ѽյб�)
	case Item::ItemType::TWOHANDED_BLUNT:	// �ΰ� ����(��յб�)
	case Item::ItemType::ONEHANDED_AXE:		// �ΰ� ����(�Ѽյ���)
	case Item::ItemType::TWOHANDED_AXE:		// �ΰ� ����(��յ���)
	case Item::ItemType::DAGGER:			// �ΰ� ����(�ܰ�)
	case Item::ItemType::COM_BLUNT:			// ��ĭ ����(�Ĺ���Ʈ ũ��������)
	case Item::ItemType::COM_SWORD:			// ��ĭ ����(�Ĺ���Ʈ ���̵�)
	case Item::ItemType::OPP_HAMMER:		// ��ĭ ����(���Ǽ����� �ظ�)
	case Item::ItemType::OPP_AXE:			// ��ĭ ����(���Ǽ����� ���̵�)
	case Item::ItemType::OPP_SLUSHER:		// ��ĭ ����(���Ǽ���Ʈ ũ��������)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 15;	break;
		case Attribute::MAX_DAMAGE:		wLimitMagnification = 15;	break;
		case Attribute::HIT_RATE:		wLimitMagnification =  5;	break;
		case Attribute::MAX_MP:			wLimitMagnification =  5;	break;  // add by shadow
		case Attribute::MP_REGEN:		wLimitMagnification =  5;	break;  // add by shadow
		case Attribute::CRITICAL:		wLimitMagnification =  5;	break;
		case Attribute::BLOCK:			wLimitMagnification =  5;	break;
		}
		break;	
	case Item::ItemType::SHIELD:			// �ΰ� ��(����)
	case Item::ItemType::SKILL_A_GUARD:		// ��ų��(�����)
		switch (eAttributeType)
		{
		case Attribute::MAX_HP:			wLimitMagnification = 10;	break;
		case Attribute::HP_REGEN:		wLimitMagnification = 10;	break;
		case Attribute::MAX_MP:			wLimitMagnification = 10;	break;
		case Attribute::MP_REGEN:		wLimitMagnification = 10;	break;
		case Attribute::BLOCK:			wLimitMagnification	= 10;	break;
		}
		break;	
	case Item::ItemType::SKILL_A_ATTACK:	// ��ų��(���þ�)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 16;	break;
		case Attribute::MAX_DAMAGE:		wLimitMagnification = 16;	break;
		case Attribute::HIT_RATE:		wLimitMagnification =  5;	break;
		case Attribute::MAX_MP:			wLimitMagnification =  5;	break;  // add by shadow
		case Attribute::MP_REGEN:		wLimitMagnification =  5;	break;  // add by shadow
		case Attribute::CRITICAL:		wLimitMagnification =  5;	break;
		case Attribute::BLOCK:			wLimitMagnification =  5;	break;
		}
		break;	
	case Item::ItemType::BOW:				// �ΰ� ����(Ȱ)
	case Item::ItemType::CROSSBOW:			// �ΰ� ����(����)
	case Item::ItemType::SKILL_A_GUN:		// ��ų��(�Ǿ�)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 25;	break;
		case Attribute::MAX_DAMAGE:		wLimitMagnification = 25;	break;
		case Attribute::HIT_RATE:		wLimitMagnification = 10;	break;
		case Attribute::MAX_MP:			wLimitMagnification = 10;	break;  // add by shadow
		case Attribute::MP_REGEN:		wLimitMagnification = 10;	break;  // add by shadow
		case Attribute::CRITICAL:		wLimitMagnification = 10;	break;
		case Attribute::BLOCK:			wLimitMagnification =  5;	break;
		}
		break;
	case Item::ItemType::OPP_TALON:			// ��ĭ ����(Ŭ�ο�)
	case Item::ItemType::SKILL_A_KNIFE:		// ��ų��(��������)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 21;	break;
		case Attribute::MAX_DAMAGE:		wLimitMagnification = 21;	break;
		case Attribute::HIT_RATE:		wLimitMagnification =  5;	break;
		case Attribute::MAX_MP:			wLimitMagnification =  5;	break;  // add by shadow
		case Attribute::MP_REGEN:		wLimitMagnification =  5;	break;  // add by shadow
		case Attribute::CRITICAL:		wLimitMagnification =  5;	break;
		case Attribute::BLOCK:			wLimitMagnification =  5;	break;
		}
		break;

		// ����ó�� ����
	case Item::ItemType::DEX_ARMOUR:		// �ΰ� ��(DEX ����)
		switch (eAttributeType)
		{
		case Attribute::ARMOR:			wLimitMagnification =  3;	break;
		case Attribute::EVADE:			wLimitMagnification =  3;	break;
		case Attribute::MAX_HP:			wLimitMagnification =  7;	break;
		case Attribute::HP_REGEN:		wLimitMagnification =  3;	break;
//		case Attribute::MAX_MP:			wLimitMagnification =  7;	break;  // disable by shadow
		case Attribute::MAGIC_RESIST:	wLimitMagnification =  3;	break;
		}
		break;
	case Item::ItemType::STAFF:				// �ΰ� ����(������)
	case Item::ItemType::OPP_SYTHE:			// ��ĭ ����(���̵�)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 10;	break;		// Dummy Factor. ������, ���̵�� �ּҵ����� ����
		case Attribute::MAX_DAMAGE:		wLimitMagnification =  7;	break;
		case Attribute::HIT_RATE:		wLimitMagnification =  2;	break;
		case Attribute::MAX_MP:			wLimitMagnification =  2;	break;
		case Attribute::MP_REGEN:		wLimitMagnification =  2;	break;
		case Attribute::CRITICAL:		wLimitMagnification =  2;	break;
		case Attribute::MAGIC_POWER:	wLimitMagnification =  7;	break;
		}
		break;
	}

/*

// S �׷��̵� ���Ѱ� ���ϱ�
bool CEquipment::GetLimitValue(Item::Attribute::Type eAttributeType, short& attrLimit)
{
	const CItemType::ArrayType eEquipType = CItemType::GetEquipType(m_ItemInfo.m_DetailData.m_dwFlags);
	const Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>(GetItemInfo().m_DetailData.m_cItemType);

	const float fMaxGrade = Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::AAA_GRADE, eAttributeType);
	const float fGradeGap = fMaxGrade - Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, EquipType::AA_GRADE, eAttributeType);

	// ���� ���� ����
	unsigned short wLimitMagnification = 1;

	switch (eItemType)
	{
	case Item::ItemType::NECKLACE:			// �Ǽ��縮(�����)
	case Item::ItemType::RING:				// �Ǽ��縮(����)
	case Item::ItemType::RUNE:				// �Ǽ��縮(��)
	// �Ǽ��縮 �������� ��ũ��Ʈ�� ������ ������ ���ѵȴ�.
		attrLimit = static_cast<short>(m_ItemInfo.m_EquipAttribute.m_usAttributeValue[eAttributeType]);
		return true;
	case Item::ItemType::CON_ARMOUR:		// �ΰ� ��(CON ����)
	case Item::ItemType::CON_BODY:			// ��ĭ ��(CON ����)
	case Item::ItemType::DEX_BODY:			// ��ĭ ��(DEX ����)
		switch (eAttributeType)
		{
		case Attribute::ARMOR:			wLimitMagnification =  5;	break;
		case Attribute::EVADE:			wLimitMagnification =  5;	break;
		case Attribute::MAX_HP:			wLimitMagnification = 10;	break;
		case Attribute::HP_REGEN:		wLimitMagnification =  5;	break;
		case Attribute::MAX_MP:			wLimitMagnification = 10;	break;
		case Attribute::MAGIC_RESIST:	wLimitMagnification =  5;	break;
		}
		break;
	case Item::ItemType::ONEHANDED_SWORD:	// �ΰ� ����(�Ѽհ�)
	case Item::ItemType::TWOHANDED_SWORD:	// �ΰ� ����(��հ�)
	case Item::ItemType::ONEHANDED_BLUNT:	// �ΰ� ����(�Ѽյб�)
	case Item::ItemType::TWOHANDED_BLUNT:	// �ΰ� ����(��յб�)
	case Item::ItemType::ONEHANDED_AXE:		// �ΰ� ����(�Ѽյ���)
	case Item::ItemType::TWOHANDED_AXE:		// �ΰ� ����(��յ���)
	case Item::ItemType::DAGGER:			// �ΰ� ����(�ܰ�)
	case Item::ItemType::COM_BLUNT:			// ��ĭ ����(�Ĺ���Ʈ ũ��������)
	case Item::ItemType::COM_SWORD:			// ��ĭ ����(�Ĺ���Ʈ ���̵�)
	case Item::ItemType::OPP_HAMMER:		// ��ĭ ����(���Ǽ����� �ظ�)
	case Item::ItemType::OPP_AXE:			// ��ĭ ����(���Ǽ����� ���̵�)
	case Item::ItemType::OPP_SLUSHER:		// ��ĭ ����(���Ǽ���Ʈ ũ��������)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 15;	break;
		case Attribute::MAX_DAMAGE:		wLimitMagnification = 15;	break;
		case Attribute::HIT_RATE:		wLimitMagnification =  5;	break;
		case Attribute::CRITICAL:		wLimitMagnification =  5;	break;
		case Attribute::BLOCK:			wLimitMagnification =  5;	break;
		}
		break;	
	case Item::ItemType::SHIELD:			// �ΰ� ��(����)
	case Item::ItemType::SKILL_A_GUARD:		// ��ų��(�����)
		switch (eAttributeType)
		{
		case Attribute::MAX_HP:			wLimitMagnification = 10;	break;
		case Attribute::HP_REGEN:		wLimitMagnification = 10;	break;
		case Attribute::MAX_MP:			wLimitMagnification = 10;	break;
		case Attribute::MP_REGEN:		wLimitMagnification = 10;	break;
		case Attribute::BLOCK:			wLimitMagnification	= 10;	break;
		}
		break;	
	case Item::ItemType::SKILL_A_ATTACK:	// ��ų��(���þ�)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 16;	break;
		case Attribute::MAX_DAMAGE:		wLimitMagnification = 16;	break;
		case Attribute::HIT_RATE:		wLimitMagnification =  5;	break;
		case Attribute::CRITICAL:		wLimitMagnification =  5;	break;
		case Attribute::BLOCK:			wLimitMagnification =  5;	break;
		}
		break;	
	case Item::ItemType::BOW:				// �ΰ� ����(Ȱ)
	case Item::ItemType::CROSSBOW:			// �ΰ� ����(����)
	case Item::ItemType::SKILL_A_GUN:		// ��ų��(�Ǿ�)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 25;	break;
		case Attribute::MAX_DAMAGE:		wLimitMagnification = 25;	break;
		case Attribute::HIT_RATE:		wLimitMagnification = 10;	break;
		case Attribute::CRITICAL:		wLimitMagnification = 10;	break;
		case Attribute::BLOCK:			wLimitMagnification =  5;	break;
		}
		break;
	case Item::ItemType::OPP_TALON:			// ��ĭ ����(Ŭ�ο�)
	case Item::ItemType::SKILL_A_KNIFE:		// ��ų��(��������)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 21;	break;
		case Attribute::MAX_DAMAGE:		wLimitMagnification = 21;	break;
		case Attribute::HIT_RATE:		wLimitMagnification =  5;	break;
		case Attribute::CRITICAL:		wLimitMagnification =  5;	break;
		case Attribute::BLOCK:			wLimitMagnification =  5;	break;
		}
		break;

		// ����ó�� ����
	case Item::ItemType::DEX_ARMOUR:		// �ΰ� ��(DEX ����)
		switch (eAttributeType)
		{
		case Attribute::ARMOR:			wLimitMagnification =  3;	break;
		case Attribute::EVADE:			wLimitMagnification =  3;	break;
		case Attribute::MAX_HP:			wLimitMagnification =  7;	break;
		case Attribute::HP_REGEN:		wLimitMagnification =  3;	break;
		case Attribute::MAX_MP:			wLimitMagnification =  7;	break;
		case Attribute::MAGIC_RESIST:	wLimitMagnification =  3;	break;
		}
		break;
	case Item::ItemType::STAFF:				// �ΰ� ����(������)
	case Item::ItemType::OPP_SYTHE:			// ��ĭ ����(���̵�)
		switch (eAttributeType)
		{
		case Attribute::MIN_DAMAGE:		wLimitMagnification = 10;	break;		// Dummy Factor. ������, ���̵�� �ּҵ����� ����
		case Attribute::MAX_DAMAGE:		wLimitMagnification =  7;	break;
		case Attribute::HIT_RATE:		wLimitMagnification =  2;	break;
		case Attribute::MAX_MP:			wLimitMagnification =  2;	break;
		case Attribute::MP_REGEN:		wLimitMagnification =  2;	break;
		case Attribute::CRITICAL:		wLimitMagnification =  2;	break;
		case Attribute::MAGIC_POWER:	wLimitMagnification =  7;	break;
		}
		break;
	}
*/

	// ���Ѱ� ���
	float fLimitFactor = fMaxGrade + (fGradeGap * wLimitMagnification);
	unsigned short attrDefault = m_ItemInfo.m_EquipAttribute.m_usAttributeValue[eAttributeType];

	#ifdef _RYL_GAME_CLIENT_
	if(CRYLNetworkData::Instance()->UseContents(GameRYL::REBALANCE_OVERITEM))
	#endif

	#ifndef _RYL_GAME_CLIENT_
	if(CServerSetup::GetInstance().UseContents(GameRYL::REBALANCE_OVERITEM))
	#endif
	{
		switch(eAttributeType)
		{
		case Attribute::MIN_DAMAGE:
			// ������, ���̵�� �ּҵ����� ����. Dummy ��������� ��ü
			if(Item::ItemType::STAFF == eItemType ||
				Item::ItemType::OPP_SYTHE == eItemType)
			{
				attrLimit = static_cast<short>(ceil(static_cast<float>(attrDefault) + (wLimitMagnification * m_ItemInfo.m_DetailData.m_wDropOption)));
				break;
			}
		case Attribute::MAX_DAMAGE:
		case Attribute::ARMOR:
		case Attribute::BLOCK:
			attrLimit = static_cast<short>(ceil(static_cast<float>(attrDefault) + (fLimitFactor * m_ItemInfo.m_DetailData.m_wDropOption)));
			break;
		default:
			attrLimit = static_cast<short>(ceil(fLimitFactor * m_ItemInfo.m_DetailData.m_wDropOption));
			break;
		}
	}
	else
	{
		attrLimit = SHRT_MAX;
		return false;
	}

	return true;
}

// S �׷��̵� ���Ѱ� ����(S �׷��̵� �̿ܿ��� ���� ������. ������ ��Ȯ���� �ʽ��ϴ�. - by hackermz)
void CEquipment::RevisionLimit()
{
	short attrLimit[Item::Attribute::MAX_ATTRIBUTE_NUM] = {0};

	GetLimitValue(attrLimit);

	RevisionLimit(attrLimit);
}

void CEquipment::RevisionLimit(short* lpattrLimit)
{
	for(unsigned char cAttributeIndex = 0; 
		cAttributeIndex < Item::Attribute::MAX_ATTRIBUTE_NUM;
		++cAttributeIndex)
	{
		RevisionLimit(static_cast<Item::Attribute::Type>(cAttributeIndex), lpattrLimit[cAttributeIndex]);
	}
}

void CEquipment::RevisionLimit(Item::Attribute::Type eAttributeType, short& attrLimit)
{
	short attrFactor = 0;
	GetSubRuneAttribute(eAttributeType, attrFactor);

	if (attrFactor >= attrLimit)
	{
		// std::min�� Ŭ���̾�Ʈ���� ���� �ʴ� �����..-_-;; (by hackermz)
		//		short attrRevision = (attrFactor >= attrLimit) ? attrLimit : attrFactor;

		RemoveUpgradeAttribute(eAttributeType);
		RemoveGemAttribute(eAttributeType);

		SetAttribute(eAttributeType, attrLimit);
	}
}

CUseItem::CUseItem(const ItemInfo& itemInfo)
:   CItem(itemInfo)
{
}

CUseItem::~CUseItem()
{
}