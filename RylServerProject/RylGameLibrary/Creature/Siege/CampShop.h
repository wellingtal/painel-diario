
#pragma once

#include <Creature/Siege/Camp.h>
#include <Item/Container/StallContainer.h>

namespace Item
{
	class CEquipment;
}

class CCampShop : public CCamp
{
public:

	virtual ~CCampShop(void);

	Item::CCampShopContainer& GetContainer(void)	{ return m_Container;	}
	unsigned long GetTempSafe(void)					{ return m_dwTempSafe;	}
	unsigned char GetTax(void)						{ return m_cTax;		}

	void SetTax(unsigned char cTax);
	bool AddGold(unsigned long dwGold);
	bool DeductGold(unsigned long dwGold);

	void DBUpdate(bool bForce);
	bool SerializeIn(unsigned long dwTempSafe, unsigned char cTax, 
		char* lpItemBuffer, unsigned long dwBufferSize, unsigned char cItemNum);

	Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError);

	bool Destroy(unsigned long dwOffencerGID = 0);

	virtual unsigned long RepairItem(Item::CEquipment* lpEquipment, unsigned long& dwCurrentGold);
	virtual bool ItemDump(char* pBuffer, int* nBufferSize_InOut) const;


private:

	CCampShop(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cSubState,
		unsigned char cUpgradeStep,	unsigned char cMaterial, unsigned char cSiegeCount,
		const CampRight& campRight, bool bFullHP);

	Item::CCampShopContainer	m_Container;	// 판매 물품용 컨테이너
	unsigned long				m_dwTempSafe;	// 임시 금고

	unsigned char				m_cTax;			// 세율

	friend	class	CSiegeObjectMgr;
};
