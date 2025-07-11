#ifndef _TEMP_INVEN_CONTAINER_H_
#define _TEMP_INVEN_CONTAINER_H_

// 전방 참조
class CCharacter;

namespace Item
{
	class CTempInvenContainer : public CListContainer
	{
	protected:

		CCharacter*		m_lpOwner;			// 임시 인벤토리의 주인
		unsigned char	m_cItemNum;			// 현재 임시 인벤토리의 아이템 갯수


	public:

		CTempInvenContainer();
		virtual ~CTempInvenContainer();

		bool Initialize(CCharacter* lpCharacter, unsigned short nMaxSize);

		virtual CItem* GetItem(ItemPos itemPos);
		virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
		virtual bool RemoveItem(ItemPos itemPos);

		virtual bool SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In);

		bool IsFull(void)	{ return (m_cItemNum == m_nMaxSize);	}
	};
};

#endif