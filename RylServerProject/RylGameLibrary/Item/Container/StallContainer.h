#ifndef _STALL_CONTAINER_H_
#define _STALL_CONTAINER_H_

#include "ItemContainer.h"


// 전방 참조
class CChatPacket;
class CSendStream;
class CAggresiveCreature;
class CCharacter;
class CCell;

namespace Item
{
	class CShopContainer : public CArrayContainer
	{
	public:

        enum 
        { 
            MAX_CUSTOMER_NUM	= 10
        };

        CShopContainer();
        virtual ~CShopContainer();

		bool Initialize(unsigned long dwCID, unsigned char cWidth, unsigned char cHeight);

		virtual void Clear(void);

        virtual CItem* GetItem(ItemPos itemPos);
        virtual bool SetItem(ItemPos itemPos, CItem* lpItem) = 0;
        virtual bool RemoveItem(ItemPos itemPos);

		void SwapPosAllItem(void);

		bool StallPriceOut(unsigned long* szStallPriceBuffer_Out, unsigned char& cItemNum_Out) const;

		bool Enter(CCharacter *lpCustomer);
		bool Leave(CCharacter *lpCustomer);

		unsigned char GetCurrentCustomerNum(void);

		virtual bool Close(void);

		virtual bool SendCharStallItemInfo(CCharacter *lpCustomer) = 0;
		virtual bool SendAllCustomer(const char* szPacket, const unsigned long dwPacketSize,
            bool bIncludeOwner, unsigned char cCMD_In);

		bool SendRemoveItem(TakeType takeType, unsigned char cCmd, const char* strBuyUser);
		bool SendCharStallEnter(unsigned long dwCustomerID, unsigned long dwOwnerID);

	protected:

		void Destroy();

		unsigned long	m_aryCustomerID[MAX_CUSTOMER_NUM];	// 손님들의 아이디 배열
	};


	class CCharacterShopContainer : public CShopContainer
    {
    public:

        enum 
        { 
            MAX_STALL_NAME_LEN	= 32 
        };

        CCharacterShopContainer();
        virtual ~CCharacterShopContainer();

		virtual void Clear(void);

		void RollBackAllItem(void);

        virtual bool SetItem(ItemPos itemPos, CItem* lpItem);

		void SetOtherOwner(unsigned long dwCID) { m_dwOtherOwnerID = dwCID;	}
		unsigned long GetOtherOwner(void)		{ return m_dwOtherOwnerID;	}

		char* GetStallName(void)				{ return m_strStallName;	}

		bool Open(char *strStallName);
		virtual bool Close(void);

		virtual bool SendCharStallItemInfo(CCharacter *lpCustomer);
		virtual bool SendAllCustomer(const char* szPacket, const unsigned long dwPacketSize,
            bool bIncludeOwner, unsigned char cCMD_In);

		bool SendCharStallOpen(char *strStallName);

	protected:

		unsigned long	m_dwOtherOwnerID;						// 주인의 아이디 (남이 개설한 노점상에 입장한 경우)
        char			m_strStallName[MAX_STALL_NAME_LEN];		// 노점상 이름
    };


	class CCampShopContainer : public CShopContainer
	{
	public:

		CCampShopContainer(void);
		virtual ~CCampShopContainer(void);

        virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
		virtual bool RemoveItem(ItemPos itemPos);

		void DropItem(CCell* lpCell, const Position& currentPos, unsigned long dwOffencerGID);

		bool StallPriceIn(const unsigned long* szStallPriceBuffer_In, unsigned char cItemNum_In);

		virtual bool SendCharStallItemInfo(CCharacter *lpCustomer);

		CItem* RegisterCancelItem(CCharacter* lpCharacter, TakeType takeType, unsigned long dwPrice, unsigned char cCmd);
		bool CheckUpdateCount(void);
		void IncreaseUpdateCount(void)	{ ++m_cUpdateCount;	}

		bool SendCharStallItemInfo(CSendStream& SendStream);

		bool IsEmpty() const;
		
	protected:

		enum Const
		{
			UPDATE_COUNT	= 1								
		};

		unsigned char	m_cUpdateCount;							// 업데이트 카운트 (UPDATE_COUNT를 넘어서면 DB에 업데이트한다.)
	};
};


#endif