#include "stdafx.h"
#include <Item/Item.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include "EquipmentsContainer.h"
#include <Creature/Character/Character.h>

#include <Utility/Math/Math.h>

#include <Log/ItemLog.h>
#include <Log/LogStruct.h>

Item::CEquipmentsContainer::CEquipmentsContainer()
:   m_cRightHand(Item::EquipmentPos::WEAPON_HAND1), m_cLeftHand(Item::EquipmentPos::SHIELD_HAND1), m_cRideFlag(0)
{


}

Item::CEquipmentsContainer::~CEquipmentsContainer()
{


}

bool Item::CEquipmentsContainer::Initialize(CCharacter* lpCharacter, unsigned short nMaxSize)
{
	m_lpOwner = lpCharacter;
	return Item::CListContainer::Initialize(lpCharacter->GetCID(), nMaxSize);
}


Item::CItem* Item::CEquipmentsContainer::GetItem(Item::ItemPos itemPos) 
{ 
	return Item::CListContainer::GetItem(itemPos); 
}

bool Item::CEquipmentsContainer::SetItem(Item::ItemPos itemPos, Item::CItem* lpItem) 
{ 
	const Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
	if (NULL == lpEquip)
	{ 
		ERRLOG1(g_Log, "��� �������� �ƴѵ� ����Ϸ��� �Ͽ����ϴ�. Item����:%d", lpItem->GetPrototypeID()); 
		return false; 
	}

	if (false == CheckEquipPos(itemPos, lpEquip))
	{
		ERRLOG4(g_Log, "CID:0x%10u ������ �������� �ʽ��ϴ�. �˸��� ��ġ�� �ƴմϴ�. itemPos:%d, itemIndex:%d, Item����:%d", 
			m_lpOwner->GetCID(), lpEquip->GetPos().m_cPos, lpEquip->GetPos().m_cIndex, lpEquip->GetPrototypeID()); 
		return false; 
	}

	if (false == FightStatus::CheckEquipable(lpEquip, 
		m_lpOwner->GetCharStatus(), static_cast<unsigned char>(m_lpOwner->GetClass()),
		static_cast<unsigned char>(m_lpOwner->GetStatus().m_nLevel), m_lpOwner->GetAbilityValue(Skill::Type::AB_EQUIP_LEVELDOWN) ))	//--//
	{ 
		ERRLOG2(g_Log, "CID:0x%10u ������ �������� �ʽ��ϴ�. �˸��� Ŭ������ �ƴϰų� ����, ������ ���ڶ��ϴ�. Item����:%d", //--//
			m_lpOwner->GetCID(), lpEquip->GetPrototypeID()); 
		return false; 
	}

	if (false == Item::CListContainer::SetItem(itemPos, lpItem)) 
	{ 
		ERRLOG0(g_Log, "��� ������ �����߽��ϴ�."); 
		return false; 
	}

	m_lpOwner->CalculateStatusData(false);
	return true;
}

bool Item::CEquipmentsContainer::RemoveItem(Item::ItemPos itemPos) 
{ 
	if (false == Item::CListContainer::RemoveItem(itemPos)) 
	{
		ERRLOG0(g_Log, "��� ������ �����߽��ϴ�.");
		return false;
	}
	
	m_lpOwner->CalculateStatusData(false);
	return true;
}

void Item::CEquipmentsContainer::GetEquipList(Item::CEquipment** ppEquip)
{
	for (int nIndex = 0; nIndex < Item::EquipmentPos::MAX_EQUPMENT_POS; ++nIndex)
	{
		ppEquip[nIndex] = CEquipment::DowncastToEquipment(m_lppItems[nIndex]);
	}
}

size_t LogErrorEquipItem(unsigned long dwCID, const char* szDetail, const char* szBuffer)
{
	const int   MIN_BUFFER = 64;
	char        szItemUID[MIN_BUFFER];

	const Item::ItemData* lpItemData = reinterpret_cast<const Item::ItemData*>(szBuffer);    
	Math::Convert::Hex64ToStr(szItemUID, lpItemData->m_dwUID);
	Item::ItemPos itemPos = lpItemData->m_ItemPos;

	ERRLOG5(g_Log, "CID:%10u %s. UID:%s, ProtoTypeID:%d, Pos:%d", 
		dwCID, szDetail, szItemUID, lpItemData->m_usProtoTypeID, itemPos.m_cIndex);

	return lpItemData->m_cItemSize;
}

bool Item::CEquipmentsContainer::SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In)
{
	size_t nBufferSize = static_cast<size_t>(dwBufferSize_In);
	size_t nUsed = 0;

	Item::CItemFactory& ItemFactory = CItemFactory::GetInstance();

	while(nBufferSize > 0)
	{
		size_t nItemSize = nBufferSize;
		Item::CItem* lpItem = ItemFactory.CreateItem(szItemBuffer_In + nUsed, nItemSize);

		if(NULL == lpItem)
		{
			nItemSize = LogErrorEquipItem(m_dwCID, "������ ������ �����߽��ϴ�", szItemBuffer_In + nUsed);
		}

		// edith 2009.07.16 ���â�� ��� ����� ���� ������ ������� Ȯ���ؼ�
		// ������ �Ұ����� ����̸� �����Ѵ�.
		CEquipment* lpEquip = CEquipment::DowncastToEquipment(lpItem);
		if(!CheckEquipInitPos(lpEquip->GetPos(), lpEquip))
		{
			nItemSize = LogErrorEquipItem(m_dwCID, "������ ������ �����߽��ϴ�", szItemBuffer_In + nUsed);
			GAMELOG::LogIllegalItem(m_dwCID, lpItem->GetPos(), lpItem);
			DELETE_ITEM(lpItem);
		}
		else if(!CListContainer::SetItem(lpItem->GetPos(), lpItem))
		{
			nItemSize = LogErrorEquipItem(m_dwCID, "������ ���⸦ �����߽��ϴ�", szItemBuffer_In + nUsed);
			DELETE_ITEM(lpItem);
		}

		nUsed += nItemSize;
		nBufferSize -= nItemSize;        
	}

	return true;
}

bool Item::CEquipmentsContainer::CheckEquipPos(Item::ItemPos itemPos, const CEquipment* lpEquip)
{
	switch (lpEquip->GetItemInfo().m_DetailData.m_cItemType)
	{
		using namespace Item;

		// ���ɼ� ���
		case ItemType::SHIRT:		if (itemPos.m_cIndex != EquipmentPos::SHIRT)		{ return false; }	break;
		case ItemType::TUNIC:		if (itemPos.m_cIndex != EquipmentPos::TUNIC)		{ return false; }	break;

		// �ΰ� ��
		case ItemType::CON_ARMOUR:		
		case ItemType::DEX_ARMOUR:
		{
			if (itemPos.m_cIndex != EquipmentPos::ARMOUR)		
			{ 
				return false; 
			}	
			break;
		}

		case ItemType::CON_HELM:		
		case ItemType::DEX_HELM:		
		{
			if (itemPos.m_cIndex != EquipmentPos::HELM)		
			{ 
				return false; 
			}	
			break;
		}

		case ItemType::CON_GLOVE:		
		case ItemType::DEX_GLOVE:		
		{
			if (itemPos.m_cIndex != EquipmentPos::GLOVE)		
			{ 
				return false; 
			}	
			break;
		}

		case ItemType::CON_BOOTS:		
		case ItemType::DEX_BOOTS:		
		{
			if (itemPos.m_cIndex != EquipmentPos::BOOTS)		
			{ 
				return false; 
			}	
			break;
		}

		// ��ĭ ��
		case ItemType::CON_BODY:		
		case ItemType::DEX_BODY:
		{
			if (itemPos.m_cIndex != EquipmentPos::BODY)		
			{ 
				return false; 
			}	
			break;
		}

		case ItemType::CON_HEAD:		
		case ItemType::DEX_HEAD:		
		{
			if (itemPos.m_cIndex != EquipmentPos::HEAD)		
			{ 
				return false; 
			}	
			break;
		}

		case ItemType::CON_PELVIS:		
		case ItemType::DEX_PELVIS:		
		{
			if (itemPos.m_cIndex != EquipmentPos::PELVIS)		
			{ 
				return false; 
			}	
			break;
		}

		case ItemType::CON_PROTECT_A:		
		case ItemType::DEX_PROTECT_A:		
		{
			if (itemPos.m_cIndex != EquipmentPos::PROTECT_ARM)		
			{ 
				return false; 
			}	
			break;
		}

		// �ΰ��� �Ѽ� ���� (������ �̵��� ����)
		case ItemType::ONEHANDED_SWORD:				
		case ItemType::ONEHANDED_AXE:
		case ItemType::ONEHANDED_BLUNT:				
		{
			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::WEAPON_HAND1:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::DAGGER == cItemType || ItemType::ARROW == cItemType || ItemType::BOLT == cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

				case EquipmentPos::WEAPON_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::SHIELD_HAND2]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::DAGGER == cItemType || ItemType::ARROW == cItemType || ItemType::BOLT == cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}
				} break;

				case EquipmentPos::SHIELD_HAND1:
				{
					if (NULL == m_lppItems[EquipmentPos::WEAPON_HAND1]) { return false; }

					unsigned char cItemType = m_lppItems[EquipmentPos::WEAPON_HAND1]->GetItemInfo().m_DetailData.m_cItemType;
					if (ItemType::ONEHANDED_SWORD != cItemType && ItemType::ONEHANDED_BLUNT != cItemType && ItemType::ONEHANDED_AXE != cItemType)
					{
						ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
						return false;
					}

				} break;

				case EquipmentPos::SHIELD_HAND2:
				{
					if (NULL == m_lppItems[EquipmentPos::WEAPON_HAND2]) { return false; }

					unsigned char cItemType = m_lppItems[EquipmentPos::WEAPON_HAND2]->GetItemInfo().m_DetailData.m_cItemType;
					if (ItemType::ONEHANDED_SWORD != cItemType && ItemType::ONEHANDED_BLUNT != cItemType && ItemType::ONEHANDED_AXE != cItemType)
					{
						ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
						return false;
					}

				} break;

				default:	return false;
			}

		} break;

		// �ΰ��� �Ѽ� ���� (��ؽ� �̵��� ����)
		case ItemType::DAGGER:
		{
			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::WEAPON_HAND1:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::ONEHANDED_SWORD == cItemType || ItemType::ONEHANDED_BLUNT == cItemType || ItemType::ONEHANDED_AXE == cItemType || 
							ItemType::ARROW == cItemType || ItemType::BOLT == cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

				case EquipmentPos::WEAPON_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::SHIELD_HAND2]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::ONEHANDED_SWORD == cItemType || ItemType::ONEHANDED_BLUNT == cItemType || ItemType::ONEHANDED_AXE == cItemType || 
							ItemType::ARROW == cItemType || ItemType::BOLT == cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

				case EquipmentPos::SHIELD_HAND1:
				{
					if (NULL == m_lppItems[EquipmentPos::WEAPON_HAND1]) { return false; }
					if (ItemType::DAGGER != m_lppItems[EquipmentPos::WEAPON_HAND1]->GetItemInfo().m_DetailData.m_cItemType)
					{
						ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
						return false;
					}

				} break;

				case EquipmentPos::SHIELD_HAND2:
				{
					if (NULL == m_lppItems[EquipmentPos::WEAPON_HAND2]) { return false; }
					if (ItemType::DAGGER != m_lppItems[EquipmentPos::WEAPON_HAND2]->GetItemInfo().m_DetailData.m_cItemType)
					{
						ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
						return false;
					}

				} break;

				default:	return false;
			}

		} break;

		// ��ĭ�� �Ѽ� ����
		case ItemType::COM_BLUNT:		case ItemType::COM_SWORD:
		case ItemType::OPP_HAMMER:		case ItemType::OPP_AXE:			
		case ItemType::OPP_SLUSHER:		case ItemType::OPP_TALON:
		{
			if (itemPos.m_cIndex != EquipmentPos::WEAPON_HAND1) {
				return false;
			}

		} break;

		// �ΰ��� ��� ����
		case ItemType::TWOHANDED_SWORD:	case ItemType::TWOHANDED_AXE:
		case ItemType::TWOHANDED_BLUNT:	case ItemType::STAFF:	
		{
			// ��� ����� �ٸ��� �տ� ���а� �ֳ��� Ȯ���ؾ� ��.
			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::WEAPON_HAND1: 
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						ERRLOG0(g_Log, "ù��° ���� ���� : �ݴ��� �տ� ���и� ��� �ֽ��ϴ�. ��� ���⸦ ������ �� �����ϴ�.");
						return false;
					}
					break;

				case EquipmentPos::WEAPON_HAND2:
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						ERRLOG0(g_Log, "�ι�° ���� ���� : �ݴ��� �տ� ���и� ��� �ֽ��ϴ�. ��� ���⸦ ������ �� �����ϴ�.");
						return false;
					}
					break;

				default:	return false;
			}

		} break;

		// Ȱ
		case ItemType::BOW:
		{
			// �ٸ��� ���� ����ų� ȭ���� ������ �־�� �Ѵ�.
			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::WEAPON_HAND1: 
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType == ItemType::ARROW) {
							break;
						}

						ERRLOG0(g_Log, "ù��° ����(Ȱ) ���� : �ݴ��� �տ� ȭ���� �ƴ� ���𰡸� ��� �ֽ��ϴ�. Ȱ�� ������ �� �����ϴ�.");
						return false;
					}

				} break;

				case EquipmentPos::WEAPON_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND2]->GetItemInfo().m_DetailData.m_cItemType == ItemType::ARROW) {
							break;
						}

						ERRLOG0(g_Log, "�ι�° ����(Ȱ) ���� : �ݴ��� �տ� ȭ���� �ƴ� ���𰡸� ��� �ֽ��ϴ�. Ȱ�� ������ �� �����ϴ�.");
						return false;
					}

				} break;

				default:	return false;
			}

		} break;

		// ����
		case ItemType::CROSSBOW:			
		{
			// �ٸ��� ���� ����ų� ��Ʈ�� ������ �־�� �Ѵ�.
			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::WEAPON_HAND1: 
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType == ItemType::BOLT) {
							break;
						}

						ERRLOG0(g_Log, "ù��° ����(����) ���� : �ݴ��� �տ� ��Ʈ�� �ƴ� ���𰡸� ��� �ֽ��ϴ�. ������ ������ �� �����ϴ�.");
						return false;
					}

				} break;

				case EquipmentPos::WEAPON_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND2]->GetItemInfo().m_DetailData.m_cItemType == ItemType::BOLT) {
							break;
						}

						ERRLOG0(g_Log, "�ι�° ����(����) ���� : �ݴ��� �տ� ��Ʈ�� �ƴ� ���𰡸� ��� �ֽ��ϴ�. ������ ������ �� �����ϴ�.");
						return false;
					}

				} break;

				default:	return false;
			}

		} break;

		// ��ĭ�� ��� ����
		case ItemType::OPP_SYTHE:
		{
			if (itemPos.m_cIndex != EquipmentPos::WEAPON_HAND1) {
				return false;
			}

			// ��� ����� �ٸ��� �տ� ������ �ֳ��� Ȯ���ؾ� ��.
			if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
			{
				ERRLOG0(g_Log, "ù��° ���� ���� : �ݴ��� �տ� ������ ��� �ֽ��ϴ�. ��� ���⸦ ������ �� �����ϴ�.");
				return false;
			}

		} break;

		// ����
		case ItemType::SHIELD:
		{
			unsigned char nWeaponType = 0;

			switch (itemPos.m_cIndex)
			{	// Shield Ÿ���� �³� Ȯ��.
				case EquipmentPos::SHIELD_HAND1: nWeaponType = EquipmentPos::WEAPON_HAND1; break;
				case EquipmentPos::SHIELD_HAND2: nWeaponType = EquipmentPos::WEAPON_HAND2; break;
				default: return false;
			}

			// ���Ⱑ ���Ǿ� ���� ������ ���и� ����� �� ����.			
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// ��չ��Ⱑ ���Ǿ� ������ ���и� ����� �� ����.
			switch (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType)
			{ 
				case ItemType::TWOHANDED_BLUNT:		case ItemType::TWOHANDED_AXE:
				case ItemType::TWOHANDED_SWORD:		case ItemType::BOW:
				case ItemType::CROSSBOW:			case ItemType::STAFF:					
				{
					ERRLOG1(g_Log, "%d��ġ�� ��� ���Ⱑ �����Ǿ� �ֽ��ϴ�. ���� ���и� ������ �� �����ϴ�.", nWeaponType);
					return false;

				} break;
			}
			
		} break;

		case ItemType::ARROW:
		{
			unsigned char nWeaponType = 0;

			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::SHIELD_HAND1: nWeaponType = EquipmentPos::WEAPON_HAND1; break;
				case EquipmentPos::SHIELD_HAND2: nWeaponType = EquipmentPos::WEAPON_HAND2; break;
				default: return false;
			}

			// ���⸦ �� ����� ���� ��� ����
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// Ȱ(����)�� ������ ��쿡�� ȭ��(��Ʈ)�� ����� �� ����.
			if (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType != ItemType::BOW) { 
				return false;
			}

		} break;

		case ItemType::BOLT:
		{
			unsigned char nWeaponType = 0;

			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::SHIELD_HAND1: nWeaponType = EquipmentPos::WEAPON_HAND1; break;
				case EquipmentPos::SHIELD_HAND2: nWeaponType = EquipmentPos::WEAPON_HAND2; break;
				default: return false;
			}

			// ���⸦ �� ����� ���� ��� ����
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// Ȱ(����)�� ������ ��쿡�� ȭ��(��Ʈ)�� ����� �� ����.
			if (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType != ItemType::CROSSBOW) { 
				return false;
			}

		} break;

		// ��ų��
		case ItemType::SKILL_A_GUARD:	case ItemType::SKILL_A_ATTACK:	
		case ItemType::SKILL_A_GUN:		case ItemType::SKILL_A_KNIFE:
		{
			if (itemPos.m_cIndex != EquipmentPos::SKILL_ARM) {
				return false;
			}

			// ���Ⱑ ���Ǿ� ���� ������ ��ų���� ����� �� ����.			
			if (NULL == m_lppItems[EquipmentPos::WEAPON]) { break; }

			// ��չ��Ⱑ ���Ǿ� ������ ��ų���� ����� �� ����.
			switch (m_lppItems[EquipmentPos::WEAPON]->GetItemInfo().m_DetailData.m_cItemType)
			{ 
				case ItemType::OPP_SYTHE:
				{
					ERRLOG0(g_Log, "��� ���Ⱑ �����Ǿ� �ֽ��ϴ�. ���� ���и� ������ �� �����ϴ�.");
					return false;

				} break;
			}
			
		} break;

		// ����
		case ItemType::RING:	
		{
			if (itemPos.m_cIndex != EquipmentPos::RINGR && itemPos.m_cIndex != EquipmentPos::RINGL) { return false; }

		} break;

		// Rodin : �� �������� �߰��Ǹ� ����ϵ��� �սô�.
		// ����
		case ItemType::RIDE:		
		case ItemType::TRANSFORM:		
		{
			if (itemPos.m_cIndex != EquipmentPos::AVATA)	{ return false; } 
			break;
		}

		// �����
		case ItemType::NECKLACE:	if (itemPos.m_cIndex != EquipmentPos::NECKLACE)	{ return false; } break;

		default: return false;			
	}

	return true;
}

bool Item::CEquipmentsContainer::CheckEquipInitPos(Item::ItemPos itemPos, const CEquipment* lpEquip)
{
	switch (lpEquip->GetItemInfo().m_DetailData.m_cItemType)
	{
		using namespace Item;

		// ���ɼ� ���
	case ItemType::SHIRT:		if (itemPos.m_cIndex != EquipmentPos::SHIRT)		{ return false; }	break;
	case ItemType::TUNIC:		if (itemPos.m_cIndex != EquipmentPos::TUNIC)		{ return false; }	break;

		// �ΰ� ��
	case ItemType::CON_ARMOUR:		
	case ItemType::DEX_ARMOUR:
		{
			if (itemPos.m_cIndex != EquipmentPos::ARMOUR)		
			{ 
				return false; 
			}	
			break;
		}

	case ItemType::CON_HELM:		
	case ItemType::DEX_HELM:		
		{
			if (itemPos.m_cIndex != EquipmentPos::HELM)		
			{ 
				return false; 
			}	
			break;
		}

	case ItemType::CON_GLOVE:		
	case ItemType::DEX_GLOVE:		
		{
			if (itemPos.m_cIndex != EquipmentPos::GLOVE)		
			{ 
				return false; 
			}	
			break;
		}

	case ItemType::CON_BOOTS:		
	case ItemType::DEX_BOOTS:		
		{
			if (itemPos.m_cIndex != EquipmentPos::BOOTS)		
			{ 
				return false; 
			}	
			break;
		}

		// ��ĭ ��
	case ItemType::CON_BODY:		
	case ItemType::DEX_BODY:
		{
			if (itemPos.m_cIndex != EquipmentPos::BODY)		
			{ 
				return false; 
			}	
			break;
		}

	case ItemType::CON_HEAD:		
	case ItemType::DEX_HEAD:		
		{
			if (itemPos.m_cIndex != EquipmentPos::HEAD)		
			{ 
				return false; 
			}	
			break;
		}

	case ItemType::CON_PELVIS:		
	case ItemType::DEX_PELVIS:		
		{
			if (itemPos.m_cIndex != EquipmentPos::PELVIS)		
			{ 
				return false; 
			}	
			break;
		}

	case ItemType::CON_PROTECT_A:		
	case ItemType::DEX_PROTECT_A:		
		{
			if (itemPos.m_cIndex != EquipmentPos::PROTECT_ARM)		
			{ 
				return false; 
			}	
			break;
		}

		// �ΰ��� �Ѽ� ���� (������ �̵��� ����)
	case ItemType::ONEHANDED_SWORD:				
	case ItemType::ONEHANDED_AXE:
	case ItemType::ONEHANDED_BLUNT:				
		{
			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::WEAPON_HAND1:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::DAGGER == cItemType || ItemType::ARROW == cItemType || ItemType::BOLT == cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

			case EquipmentPos::WEAPON_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::SHIELD_HAND2]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::DAGGER == cItemType || ItemType::ARROW == cItemType || ItemType::BOLT == cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}
				} break;

			case EquipmentPos::SHIELD_HAND1:
				{
					if (NULL != m_lppItems[EquipmentPos::WEAPON_HAND1])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::WEAPON_HAND1]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::ONEHANDED_SWORD != cItemType && ItemType::ONEHANDED_BLUNT != cItemType && ItemType::ONEHANDED_AXE != cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

			case EquipmentPos::SHIELD_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::WEAPON_HAND2])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::WEAPON_HAND2]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::ONEHANDED_SWORD != cItemType && ItemType::ONEHANDED_BLUNT != cItemType && ItemType::ONEHANDED_AXE != cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

			default:	return false;
			}

		} break;

		// �ΰ��� �Ѽ� ���� (��ؽ� �̵��� ����)
	case ItemType::DAGGER:
		{
			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::WEAPON_HAND1:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::ONEHANDED_SWORD == cItemType || ItemType::ONEHANDED_BLUNT == cItemType || ItemType::ONEHANDED_AXE == cItemType || 
							ItemType::ARROW == cItemType || ItemType::BOLT == cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

			case EquipmentPos::WEAPON_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						unsigned char cItemType = m_lppItems[EquipmentPos::SHIELD_HAND2]->GetItemInfo().m_DetailData.m_cItemType;
						if (ItemType::ONEHANDED_SWORD == cItemType || ItemType::ONEHANDED_BLUNT == cItemType || ItemType::ONEHANDED_AXE == cItemType || 
							ItemType::ARROW == cItemType || ItemType::BOLT == cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

			case EquipmentPos::SHIELD_HAND1:
				{
					if (NULL != m_lppItems[EquipmentPos::WEAPON_HAND1])
					{
						if (ItemType::DAGGER != m_lppItems[EquipmentPos::WEAPON_HAND1]->GetItemInfo().m_DetailData.m_cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

			case EquipmentPos::SHIELD_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::WEAPON_HAND2])
					{
						if (ItemType::DAGGER != m_lppItems[EquipmentPos::WEAPON_HAND2]->GetItemInfo().m_DetailData.m_cItemType)
						{
							ERRLOG0(g_Log, "�ΰ��� �Ѽ� ���� ���� ����.");
							return false;
						}
					}

				} break;

			default:	return false;
			}

		} break;

		// ��ĭ�� �Ѽ� ����
	case ItemType::COM_BLUNT:		case ItemType::COM_SWORD:
	case ItemType::OPP_HAMMER:		case ItemType::OPP_AXE:			
	case ItemType::OPP_SLUSHER:		case ItemType::OPP_TALON:
		{
			if (itemPos.m_cIndex != EquipmentPos::WEAPON_HAND1) {
				return false;
			}

		} break;

		// �ΰ��� ��� ����
	case ItemType::TWOHANDED_SWORD:	case ItemType::TWOHANDED_AXE:
	case ItemType::TWOHANDED_BLUNT:	case ItemType::STAFF:	
		{
			// ��� ����� �ٸ��� �տ� ���а� �ֳ��� Ȯ���ؾ� ��.
			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::WEAPON_HAND1: 
				if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
				{
					ERRLOG0(g_Log, "ù��° ���� ���� : �ݴ��� �տ� ���и� ��� �ֽ��ϴ�. ��� ���⸦ ������ �� �����ϴ�.");
					return false;
				}
				break;

			case EquipmentPos::WEAPON_HAND2:
				if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
				{
					ERRLOG0(g_Log, "�ι�° ���� ���� : �ݴ��� �տ� ���и� ��� �ֽ��ϴ�. ��� ���⸦ ������ �� �����ϴ�.");
					return false;
				}
				break;

			default:	return false;
			}

		} break;

		// Ȱ
	case ItemType::BOW:
		{
			// �ٸ��� ���� ����ų� ȭ���� ������ �־�� �Ѵ�.
			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::WEAPON_HAND1: 
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType == ItemType::ARROW) {
							break;
						}

						ERRLOG0(g_Log, "ù��° ����(Ȱ) ���� : �ݴ��� �տ� ȭ���� �ƴ� ���𰡸� ��� �ֽ��ϴ�. Ȱ�� ������ �� �����ϴ�.");
						return false;
					}

				} break;

			case EquipmentPos::WEAPON_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND2]->GetItemInfo().m_DetailData.m_cItemType == ItemType::ARROW) {
							break;
						}

						ERRLOG0(g_Log, "�ι�° ����(Ȱ) ���� : �ݴ��� �տ� ȭ���� �ƴ� ���𰡸� ��� �ֽ��ϴ�. Ȱ�� ������ �� �����ϴ�.");
						return false;
					}

				} break;

			default:	return false;
			}

		} break;

		// ����
	case ItemType::CROSSBOW:			
		{
			// �ٸ��� ���� ����ų� ��Ʈ�� ������ �־�� �Ѵ�.
			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::WEAPON_HAND1: 
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType == ItemType::BOLT) {
							break;
						}

						ERRLOG0(g_Log, "ù��° ����(����) ���� : �ݴ��� �տ� ��Ʈ�� �ƴ� ���𰡸� ��� �ֽ��ϴ�. ������ ������ �� �����ϴ�.");
						return false;
					}

				} break;

			case EquipmentPos::WEAPON_HAND2:
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND2]->GetItemInfo().m_DetailData.m_cItemType == ItemType::BOLT) {
							break;
						}

						ERRLOG0(g_Log, "�ι�° ����(����) ���� : �ݴ��� �տ� ��Ʈ�� �ƴ� ���𰡸� ��� �ֽ��ϴ�. ������ ������ �� �����ϴ�.");
						return false;
					}

				} break;

			default:	return false;
			}

		} break;

		// ��ĭ�� ��� ����
	case ItemType::OPP_SYTHE:
		{
			if (itemPos.m_cIndex != EquipmentPos::WEAPON_HAND1) {
				return false;
			}

			// ��� ����� �ٸ��� �տ� ������ �ֳ��� Ȯ���ؾ� ��.
			if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
			{
				ERRLOG0(g_Log, "ù��° ���� ���� : �ݴ��� �տ� ������ ��� �ֽ��ϴ�. ��� ���⸦ ������ �� �����ϴ�.");
				return false;
			}

		} break;

		// ����
	case ItemType::SHIELD:
		{
			unsigned char nWeaponType = 0;

			switch (itemPos.m_cIndex)
			{	// Shield Ÿ���� �³� Ȯ��.
			case EquipmentPos::SHIELD_HAND1: nWeaponType = EquipmentPos::WEAPON_HAND1; break;
			case EquipmentPos::SHIELD_HAND2: nWeaponType = EquipmentPos::WEAPON_HAND2; break;
			default: return false;
			}

			// ���Ⱑ ���Ǿ� ���� ������ ���и� ����� �� ����.			
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// ��չ��Ⱑ ���Ǿ� ������ ���и� ����� �� ����.
			switch (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType)
			{ 
			case ItemType::TWOHANDED_BLUNT:		case ItemType::TWOHANDED_AXE:
			case ItemType::TWOHANDED_SWORD:		case ItemType::BOW:
			case ItemType::CROSSBOW:			case ItemType::STAFF:					
				{
					ERRLOG1(g_Log, "%d��ġ�� ��� ���Ⱑ �����Ǿ� �ֽ��ϴ�. ���� ���и� ������ �� �����ϴ�.", nWeaponType);
					return false;

				} break;
			}

		} break;

	case ItemType::ARROW:
		{
			unsigned char nWeaponType = 0;

			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::SHIELD_HAND1: nWeaponType = EquipmentPos::WEAPON_HAND1; break;
			case EquipmentPos::SHIELD_HAND2: nWeaponType = EquipmentPos::WEAPON_HAND2; break;
			default: return false;
			}

			// ���⸦ �� ����� ���� ��� ����
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// Ȱ(����)�� ������ ��쿡�� ȭ��(��Ʈ)�� ����� �� ����.
			if (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType != ItemType::BOW) { 
				return false;
			}

		} break;

	case ItemType::BOLT:
		{
			unsigned char nWeaponType = 0;

			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::SHIELD_HAND1: nWeaponType = EquipmentPos::WEAPON_HAND1; break;
			case EquipmentPos::SHIELD_HAND2: nWeaponType = EquipmentPos::WEAPON_HAND2; break;
			default: return false;
			}

			// ���⸦ �� ����� ���� ��� ����
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// Ȱ(����)�� ������ ��쿡�� ȭ��(��Ʈ)�� ����� �� ����.
			if (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType != ItemType::CROSSBOW) { 
				return false;
			}

		} break;

		// ��ų��
	case ItemType::SKILL_A_GUARD:	case ItemType::SKILL_A_ATTACK:	
	case ItemType::SKILL_A_GUN:		case ItemType::SKILL_A_KNIFE:
		{
			if (itemPos.m_cIndex != EquipmentPos::SKILL_ARM) {
				return false;
			}

			// ���Ⱑ ���Ǿ� ���� ������ ��ų���� ����� �� ����.			
			if (NULL == m_lppItems[EquipmentPos::WEAPON]) { break; }

			// ��չ��Ⱑ ���Ǿ� ������ ��ų���� ����� �� ����.
			switch (m_lppItems[EquipmentPos::WEAPON]->GetItemInfo().m_DetailData.m_cItemType)
			{ 
			case ItemType::OPP_SYTHE:
				{
					ERRLOG0(g_Log, "��� ���Ⱑ �����Ǿ� �ֽ��ϴ�. ���� ���и� ������ �� �����ϴ�.");
					return false;

				} break;
			}

		} break;

		// ����
	case ItemType::RING:	
		{
			if (itemPos.m_cIndex != EquipmentPos::RINGR && itemPos.m_cIndex != EquipmentPos::RINGL) { return false; }

		} break;

		// Rodin : �� �������� �߰��Ǹ� ����ϵ��� �սô�.
		// ����
	case ItemType::RIDE:		
	case ItemType::TRANSFORM:		
		{
			if (itemPos.m_cIndex != EquipmentPos::AVATA)	{ return false; } 
			break;
		}

		// �����
	case ItemType::NECKLACE:	if (itemPos.m_cIndex != EquipmentPos::NECKLACE)	{ return false; } break;

	default: return false;			
	}

	return true;
}
