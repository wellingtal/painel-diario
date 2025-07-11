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
		ERRLOG1(g_Log, "장비 아이템이 아닌데 장비하려고 하였습니다. Item종류:%d", lpItem->GetPrototypeID()); 
		return false; 
	}

	if (false == CheckEquipPos(itemPos, lpEquip))
	{
		ERRLOG4(g_Log, "CID:0x%10u 장착이 가능하지 않습니다. 알맞은 위치가 아닙니다. itemPos:%d, itemIndex:%d, Item종류:%d", 
			m_lpOwner->GetCID(), lpEquip->GetPos().m_cPos, lpEquip->GetPos().m_cIndex, lpEquip->GetPrototypeID()); 
		return false; 
	}

	if (false == FightStatus::CheckEquipable(lpEquip, 
		m_lpOwner->GetCharStatus(), static_cast<unsigned char>(m_lpOwner->GetClass()),
		static_cast<unsigned char>(m_lpOwner->GetStatus().m_nLevel), m_lpOwner->GetAbilityValue(Skill::Type::AB_EQUIP_LEVELDOWN) ))	//--//
	{ 
		ERRLOG2(g_Log, "CID:0x%10u 장착이 가능하지 않습니다. 알맞은 클래스가 아니거나 스탯, 레벨이 모자랍니다. Item종류:%d", //--//
			m_lpOwner->GetCID(), lpEquip->GetPrototypeID()); 
		return false; 
	}

	if (false == Item::CListContainer::SetItem(itemPos, lpItem)) 
	{ 
		ERRLOG0(g_Log, "장비 장착에 실패했습니다."); 
		return false; 
	}

	m_lpOwner->CalculateStatusData(false);
	return true;
}

bool Item::CEquipmentsContainer::RemoveItem(Item::ItemPos itemPos) 
{ 
	if (false == Item::CListContainer::RemoveItem(itemPos)) 
	{
		ERRLOG0(g_Log, "장비 해제에 실패했습니다.");
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
			nItemSize = LogErrorEquipItem(m_dwCID, "아이템 생성에 실패했습니다", szItemBuffer_In + nUsed);
		}

		// edith 2009.07.16 장비창에 장비 착용시 착용 가능한 장비인지 확인해서
		// 착용이 불가능한 장비이면 제거한다.
		CEquipment* lpEquip = CEquipment::DowncastToEquipment(lpItem);
		if(!CheckEquipInitPos(lpEquip->GetPos(), lpEquip))
		{
			nItemSize = LogErrorEquipItem(m_dwCID, "아이템 착용을 실패했습니다", szItemBuffer_In + nUsed);
			GAMELOG::LogIllegalItem(m_dwCID, lpItem->GetPos(), lpItem);
			DELETE_ITEM(lpItem);
		}
		else if(!CListContainer::SetItem(lpItem->GetPos(), lpItem))
		{
			nItemSize = LogErrorEquipItem(m_dwCID, "아이템 놓기를 실패했습니다", szItemBuffer_In + nUsed);
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

		// 비기능성 장비
		case ItemType::SHIRT:		if (itemPos.m_cIndex != EquipmentPos::SHIRT)		{ return false; }	break;
		case ItemType::TUNIC:		if (itemPos.m_cIndex != EquipmentPos::TUNIC)		{ return false; }	break;

		// 인간 방어구
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

		// 아칸 방어구
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

		// 인간용 한손 무기 (워리어 이도류 가능)
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
						ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
						return false;
					}

				} break;

				case EquipmentPos::SHIELD_HAND2:
				{
					if (NULL == m_lppItems[EquipmentPos::WEAPON_HAND2]) { return false; }

					unsigned char cItemType = m_lppItems[EquipmentPos::WEAPON_HAND2]->GetItemInfo().m_DetailData.m_cItemType;
					if (ItemType::ONEHANDED_SWORD != cItemType && ItemType::ONEHANDED_BLUNT != cItemType && ItemType::ONEHANDED_AXE != cItemType)
					{
						ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
						return false;
					}

				} break;

				default:	return false;
			}

		} break;

		// 인간용 한손 무기 (어쌔신 이도류 가능)
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
							return false;
						}
					}

				} break;

				case EquipmentPos::SHIELD_HAND1:
				{
					if (NULL == m_lppItems[EquipmentPos::WEAPON_HAND1]) { return false; }
					if (ItemType::DAGGER != m_lppItems[EquipmentPos::WEAPON_HAND1]->GetItemInfo().m_DetailData.m_cItemType)
					{
						ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
						return false;
					}

				} break;

				case EquipmentPos::SHIELD_HAND2:
				{
					if (NULL == m_lppItems[EquipmentPos::WEAPON_HAND2]) { return false; }
					if (ItemType::DAGGER != m_lppItems[EquipmentPos::WEAPON_HAND2]->GetItemInfo().m_DetailData.m_cItemType)
					{
						ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
						return false;
					}

				} break;

				default:	return false;
			}

		} break;

		// 아칸용 한손 무기
		case ItemType::COM_BLUNT:		case ItemType::COM_SWORD:
		case ItemType::OPP_HAMMER:		case ItemType::OPP_AXE:			
		case ItemType::OPP_SLUSHER:		case ItemType::OPP_TALON:
		{
			if (itemPos.m_cIndex != EquipmentPos::WEAPON_HAND1) {
				return false;
			}

		} break;

		// 인간용 양손 무기
		case ItemType::TWOHANDED_SWORD:	case ItemType::TWOHANDED_AXE:
		case ItemType::TWOHANDED_BLUNT:	case ItemType::STAFF:	
		{
			// 양손 무기는 다른편 손에 방패가 있나도 확인해야 함.
			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::WEAPON_HAND1: 
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						ERRLOG0(g_Log, "첫번째 무기 장착 : 반대편 손에 방패를 들고 있습니다. 양손 무기를 장착할 수 없습니다.");
						return false;
					}
					break;

				case EquipmentPos::WEAPON_HAND2:
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
					{
						ERRLOG0(g_Log, "두번째 무기 장착 : 반대편 손에 방패를 들고 있습니다. 양손 무기를 장착할 수 없습니다.");
						return false;
					}
					break;

				default:	return false;
			}

		} break;

		// 활
		case ItemType::BOW:
		{
			// 다른쪽 손이 비었거나 화살을 가지고 있어야 한다.
			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::WEAPON_HAND1: 
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType == ItemType::ARROW) {
							break;
						}

						ERRLOG0(g_Log, "첫번째 무기(활) 장착 : 반대편 손에 화살이 아닌 무언가를 들고 있습니다. 활을 장착할 수 없습니다.");
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

						ERRLOG0(g_Log, "두번째 무기(활) 장착 : 반대편 손에 화살이 아닌 무언가를 들고 있습니다. 활을 장착할 수 없습니다.");
						return false;
					}

				} break;

				default:	return false;
			}

		} break;

		// 석궁
		case ItemType::CROSSBOW:			
		{
			// 다른쪽 손이 비었거나 볼트를 가지고 있어야 한다.
			switch (itemPos.m_cIndex)
			{
				case EquipmentPos::WEAPON_HAND1: 
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType == ItemType::BOLT) {
							break;
						}

						ERRLOG0(g_Log, "첫번째 무기(석궁) 장착 : 반대편 손에 볼트가 아닌 무언가를 들고 있습니다. 석궁을 장착할 수 없습니다.");
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

						ERRLOG0(g_Log, "두번째 무기(석궁) 장착 : 반대편 손에 볼트가 아닌 무언가를 들고 있습니다. 석궁을 장착할 수 없습니다.");
						return false;
					}

				} break;

				default:	return false;
			}

		} break;

		// 아칸용 양손 무기
		case ItemType::OPP_SYTHE:
		{
			if (itemPos.m_cIndex != EquipmentPos::WEAPON_HAND1) {
				return false;
			}

			// 양손 무기는 다른편 손에 뭔가가 있나도 확인해야 함.
			if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
			{
				ERRLOG0(g_Log, "첫번째 무기 장착 : 반대편 손에 뭔가를 들고 있습니다. 양손 무기를 장착할 수 없습니다.");
				return false;
			}

		} break;

		// 방패
		case ItemType::SHIELD:
		{
			unsigned char nWeaponType = 0;

			switch (itemPos.m_cIndex)
			{	// Shield 타입이 맞나 확인.
				case EquipmentPos::SHIELD_HAND1: nWeaponType = EquipmentPos::WEAPON_HAND1; break;
				case EquipmentPos::SHIELD_HAND2: nWeaponType = EquipmentPos::WEAPON_HAND2; break;
				default: return false;
			}

			// 무기가 장비되어 있지 않으면 방패를 장비할 수 있음.			
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// 양손무기가 장비되어 있으면 방패를 장비할 수 없음.
			switch (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType)
			{ 
				case ItemType::TWOHANDED_BLUNT:		case ItemType::TWOHANDED_AXE:
				case ItemType::TWOHANDED_SWORD:		case ItemType::BOW:
				case ItemType::CROSSBOW:			case ItemType::STAFF:					
				{
					ERRLOG1(g_Log, "%d위치에 양손 무기가 장착되어 있습니다. 따라서 방패를 장착할 수 없습니다.", nWeaponType);
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

			// 무기를 안 들었을 때도 장비 가능
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// 활(석궁)을 장착한 경우에만 화살(볼트)를 장비할 수 있음.
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

			// 무기를 안 들었을 때도 장비 가능
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// 활(석궁)을 장착한 경우에만 화살(볼트)를 장비할 수 있음.
			if (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType != ItemType::CROSSBOW) { 
				return false;
			}

		} break;

		// 스킬암
		case ItemType::SKILL_A_GUARD:	case ItemType::SKILL_A_ATTACK:	
		case ItemType::SKILL_A_GUN:		case ItemType::SKILL_A_KNIFE:
		{
			if (itemPos.m_cIndex != EquipmentPos::SKILL_ARM) {
				return false;
			}

			// 무기가 장비되어 있지 않으면 스킬암을 장비할 수 있음.			
			if (NULL == m_lppItems[EquipmentPos::WEAPON]) { break; }

			// 양손무기가 장비되어 있으면 스킬암을 장비할 수 없음.
			switch (m_lppItems[EquipmentPos::WEAPON]->GetItemInfo().m_DetailData.m_cItemType)
			{ 
				case ItemType::OPP_SYTHE:
				{
					ERRLOG0(g_Log, "양손 무기가 장착되어 있습니다. 따라서 방패를 장착할 수 없습니다.");
					return false;

				} break;
			}
			
		} break;

		// 반지
		case ItemType::RING:	
		{
			if (itemPos.m_cIndex != EquipmentPos::RINGR && itemPos.m_cIndex != EquipmentPos::RINGL) { return false; }

		} break;

		// Rodin : 각 아이템이 추가되면 사용하도록 합시다.
		// 망토
		case ItemType::RIDE:		
		case ItemType::TRANSFORM:		
		{
			if (itemPos.m_cIndex != EquipmentPos::AVATA)	{ return false; } 
			break;
		}

		// 목걸이
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

		// 비기능성 장비
	case ItemType::SHIRT:		if (itemPos.m_cIndex != EquipmentPos::SHIRT)		{ return false; }	break;
	case ItemType::TUNIC:		if (itemPos.m_cIndex != EquipmentPos::TUNIC)		{ return false; }	break;

		// 인간 방어구
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

		// 아칸 방어구
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

		// 인간용 한손 무기 (워리어 이도류 가능)
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
							return false;
						}
					}

				} break;

			default:	return false;
			}

		} break;

		// 인간용 한손 무기 (어쌔신 이도류 가능)
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
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
							ERRLOG0(g_Log, "인간용 한손 무기 장착 실패.");
							return false;
						}
					}

				} break;

			default:	return false;
			}

		} break;

		// 아칸용 한손 무기
	case ItemType::COM_BLUNT:		case ItemType::COM_SWORD:
	case ItemType::OPP_HAMMER:		case ItemType::OPP_AXE:			
	case ItemType::OPP_SLUSHER:		case ItemType::OPP_TALON:
		{
			if (itemPos.m_cIndex != EquipmentPos::WEAPON_HAND1) {
				return false;
			}

		} break;

		// 인간용 양손 무기
	case ItemType::TWOHANDED_SWORD:	case ItemType::TWOHANDED_AXE:
	case ItemType::TWOHANDED_BLUNT:	case ItemType::STAFF:	
		{
			// 양손 무기는 다른편 손에 방패가 있나도 확인해야 함.
			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::WEAPON_HAND1: 
				if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
				{
					ERRLOG0(g_Log, "첫번째 무기 장착 : 반대편 손에 방패를 들고 있습니다. 양손 무기를 장착할 수 없습니다.");
					return false;
				}
				break;

			case EquipmentPos::WEAPON_HAND2:
				if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND2])
				{
					ERRLOG0(g_Log, "두번째 무기 장착 : 반대편 손에 방패를 들고 있습니다. 양손 무기를 장착할 수 없습니다.");
					return false;
				}
				break;

			default:	return false;
			}

		} break;

		// 활
	case ItemType::BOW:
		{
			// 다른쪽 손이 비었거나 화살을 가지고 있어야 한다.
			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::WEAPON_HAND1: 
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType == ItemType::ARROW) {
							break;
						}

						ERRLOG0(g_Log, "첫번째 무기(활) 장착 : 반대편 손에 화살이 아닌 무언가를 들고 있습니다. 활을 장착할 수 없습니다.");
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

						ERRLOG0(g_Log, "두번째 무기(활) 장착 : 반대편 손에 화살이 아닌 무언가를 들고 있습니다. 활을 장착할 수 없습니다.");
						return false;
					}

				} break;

			default:	return false;
			}

		} break;

		// 석궁
	case ItemType::CROSSBOW:			
		{
			// 다른쪽 손이 비었거나 볼트를 가지고 있어야 한다.
			switch (itemPos.m_cIndex)
			{
			case EquipmentPos::WEAPON_HAND1: 
				{
					if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
					{
						if (m_lppItems[EquipmentPos::SHIELD_HAND1]->GetItemInfo().m_DetailData.m_cItemType == ItemType::BOLT) {
							break;
						}

						ERRLOG0(g_Log, "첫번째 무기(석궁) 장착 : 반대편 손에 볼트가 아닌 무언가를 들고 있습니다. 석궁을 장착할 수 없습니다.");
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

						ERRLOG0(g_Log, "두번째 무기(석궁) 장착 : 반대편 손에 볼트가 아닌 무언가를 들고 있습니다. 석궁을 장착할 수 없습니다.");
						return false;
					}

				} break;

			default:	return false;
			}

		} break;

		// 아칸용 양손 무기
	case ItemType::OPP_SYTHE:
		{
			if (itemPos.m_cIndex != EquipmentPos::WEAPON_HAND1) {
				return false;
			}

			// 양손 무기는 다른편 손에 뭔가가 있나도 확인해야 함.
			if (NULL != m_lppItems[EquipmentPos::SHIELD_HAND1])
			{
				ERRLOG0(g_Log, "첫번째 무기 장착 : 반대편 손에 뭔가를 들고 있습니다. 양손 무기를 장착할 수 없습니다.");
				return false;
			}

		} break;

		// 방패
	case ItemType::SHIELD:
		{
			unsigned char nWeaponType = 0;

			switch (itemPos.m_cIndex)
			{	// Shield 타입이 맞나 확인.
			case EquipmentPos::SHIELD_HAND1: nWeaponType = EquipmentPos::WEAPON_HAND1; break;
			case EquipmentPos::SHIELD_HAND2: nWeaponType = EquipmentPos::WEAPON_HAND2; break;
			default: return false;
			}

			// 무기가 장비되어 있지 않으면 방패를 장비할 수 있음.			
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// 양손무기가 장비되어 있으면 방패를 장비할 수 없음.
			switch (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType)
			{ 
			case ItemType::TWOHANDED_BLUNT:		case ItemType::TWOHANDED_AXE:
			case ItemType::TWOHANDED_SWORD:		case ItemType::BOW:
			case ItemType::CROSSBOW:			case ItemType::STAFF:					
				{
					ERRLOG1(g_Log, "%d위치에 양손 무기가 장착되어 있습니다. 따라서 방패를 장착할 수 없습니다.", nWeaponType);
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

			// 무기를 안 들었을 때도 장비 가능
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// 활(석궁)을 장착한 경우에만 화살(볼트)를 장비할 수 있음.
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

			// 무기를 안 들었을 때도 장비 가능
			if (NULL == m_lppItems[nWeaponType]) { break; }

			// 활(석궁)을 장착한 경우에만 화살(볼트)를 장비할 수 있음.
			if (m_lppItems[nWeaponType]->GetItemInfo().m_DetailData.m_cItemType != ItemType::CROSSBOW) { 
				return false;
			}

		} break;

		// 스킬암
	case ItemType::SKILL_A_GUARD:	case ItemType::SKILL_A_ATTACK:	
	case ItemType::SKILL_A_GUN:		case ItemType::SKILL_A_KNIFE:
		{
			if (itemPos.m_cIndex != EquipmentPos::SKILL_ARM) {
				return false;
			}

			// 무기가 장비되어 있지 않으면 스킬암을 장비할 수 있음.			
			if (NULL == m_lppItems[EquipmentPos::WEAPON]) { break; }

			// 양손무기가 장비되어 있으면 스킬암을 장비할 수 없음.
			switch (m_lppItems[EquipmentPos::WEAPON]->GetItemInfo().m_DetailData.m_cItemType)
			{ 
			case ItemType::OPP_SYTHE:
				{
					ERRLOG0(g_Log, "양손 무기가 장착되어 있습니다. 따라서 방패를 장착할 수 없습니다.");
					return false;

				} break;
			}

		} break;

		// 반지
	case ItemType::RING:	
		{
			if (itemPos.m_cIndex != EquipmentPos::RINGR && itemPos.m_cIndex != EquipmentPos::RINGL) { return false; }

		} break;

		// Rodin : 각 아이템이 추가되면 사용하도록 합시다.
		// 망토
	case ItemType::RIDE:		
	case ItemType::TRANSFORM:		
		{
			if (itemPos.m_cIndex != EquipmentPos::AVATA)	{ return false; } 
			break;
		}

		// 목걸이
	case ItemType::NECKLACE:	if (itemPos.m_cIndex != EquipmentPos::NECKLACE)	{ return false; } break;

	default: return false;			
	}

	return true;
}
