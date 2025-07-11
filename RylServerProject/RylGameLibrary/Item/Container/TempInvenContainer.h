#ifndef _TEMP_INVEN_CONTAINER_H_
#define _TEMP_INVEN_CONTAINER_H_

// ���� ����
class CCharacter;

namespace Item
{
	class CTempInvenContainer : public CListContainer
	{
	protected:

		CCharacter*		m_lpOwner;			// �ӽ� �κ��丮�� ����
		unsigned char	m_cItemNum;			// ���� �ӽ� �κ��丮�� ������ ����


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