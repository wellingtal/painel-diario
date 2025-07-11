#ifndef _EQUIPMENTS_CONTAINER_H_
#define _EQUIPMENTS_CONTAINER_H_

#include <Item/Item.h>
#include "ItemContainer.h"

// 전방 참조
class CCharacter;

namespace Item
{
	class CEquipmentsContainer : public CListContainer
    {
	protected:

        CCharacter*		m_lpOwner;

		unsigned char	m_cRightHand;
		unsigned char	m_cLeftHand;
		unsigned char	m_cRideFlag;
	
	public:

        CEquipmentsContainer();
        virtual ~CEquipmentsContainer();

        bool Initialize(CCharacter* lpCharacter, unsigned short nMaxSize);

		virtual CItem* GetItem(ItemPos itemPos);
        virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
        virtual bool RemoveItem(ItemPos itemPos);

        void GetEquipmentView(unsigned short* usProtoTypeArray, unsigned int nStartPos = 0,
            int nCopyNum = Item::EquipmentPos::MAX_EQUPMENT_POS);

		void ChangeWeaponAndShield() { m_cRightHand = GetExtraRightHandIndex(); m_cLeftHand = GetExtraLeftHandIndex(); }
		void ChangeRide(char cRide) 
		{ 
			 m_cRideFlag = cRide;
		}
		

		unsigned char GetRightHandIndex(void) const		{ return m_cRightHand;	}
		unsigned char GetLeftHandIndex(void) const		{ return m_cLeftHand;	}

		unsigned char GetExtraRightHandIndex() const 
		{ 
			return (m_cRightHand == Item::EquipmentPos::WEAPON_HAND1) ? 
				Item::EquipmentPos::WEAPON_HAND2 : Item::EquipmentPos::WEAPON_HAND1; 
		}
		unsigned char GetExtraLeftHandIndex() const 
		{ 
			return (m_cLeftHand == Item::EquipmentPos::SHIELD_HAND1) ? 
				Item::EquipmentPos::SHIELD_HAND2 : Item::EquipmentPos::SHIELD_HAND1; 
		}

		Item::CEquipment* GetRightHand(void) const	{ return static_cast<CEquipment*>(m_lppItems[m_cRightHand]);	}
		Item::CEquipment* GetLeftHand(void) const	{ return static_cast<CEquipment*>(m_lppItems[m_cLeftHand]);		}

		void GetEquipList(Item::CEquipment** ppEquip);

		bool CheckEquipPos(ItemPos itemPos, const CEquipment* lpEquip);
		bool CheckEquipInitPos(ItemPos itemPos, const CEquipment* lpEquip);

		virtual bool SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In);
	};
};


inline void Item::CEquipmentsContainer::GetEquipmentView(unsigned short* usProtoTypeArray, unsigned int nStartPos, int nCopyNum)
{
    int nPastEnd = nStartPos + nCopyNum;
    nPastEnd = Item::EquipmentPos::MAX_EQUPMENT_POS < nPastEnd ? Item::EquipmentPos::MAX_EQUPMENT_POS : nPastEnd;

	for (int nDstIndex = 0, nSrcIndex = nStartPos; nSrcIndex < nPastEnd; ++nSrcIndex, ++nDstIndex) 
	{
		if (NULL == m_lppItems[nSrcIndex])
		{ 
			usProtoTypeArray[nDstIndex] = 0; 
			continue; 
		}		

		usProtoTypeArray[nDstIndex] = m_lppItems[nSrcIndex]->GetPrototypeID();
	}
}


#endif