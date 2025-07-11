#ifndef _LOG_STRUCT_H_
#define _LOG_STRUCT_H_

#include <winsock2.h>
#include <windows.h>
#include <numeric>

#include <Item/Item.h>
#include <DB/DBDefine.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>
#include <Creature/CreatureStructure.h>

#pragma pack(1)


// ������ ������ �ٲ� ����, �α׵� �ٸ� ������� ����� ��.
namespace GAMELOG
{
	typedef unsigned char CMDType;
	typedef unsigned char ERRType;

	// �⺻ �α� Structure
	struct sLogBase
	{
		unsigned long	m_dwUID;	// UserID
		unsigned long	m_dwCID;	// Character ID
		
		time_t	        m_time;		// �α� �ð�

		unsigned short  m_usXPos;	// ĳ���� ��ǥX
		unsigned short  m_usYPos;	// ĳ���� ��ǥY
		unsigned short  m_usZPos;	// ĳ���� ��ǥZ

		CMDType	        m_cCmd;		// Ŀ�ǵ�
		ERRType	        m_cErr;		// ���� �ڵ�

		inline void InitLogBase(const unsigned long dwUID, const unsigned long dwCID, const Position& Pos,
			const time_t time, const CMDType cCmd, const ERRType cErr);

		int GetSize();
	};

	inline void sLogBase::InitLogBase(const unsigned long dwUID, const unsigned long dwCID, const Position& Pos,
									const time_t time, const CMDType cCmd, const ERRType cErr)
	{
		m_dwUID = dwUID;
		m_dwCID = dwCID;

		m_usXPos = static_cast<unsigned short>(Pos.m_fPointX);
		m_usYPos = static_cast<unsigned short>(Pos.m_fPointY);
		m_usZPos = static_cast<unsigned short>(Pos.m_fPointZ);

		m_time = time;
		m_cCmd = cCmd;
		m_cErr = cErr;
	}


	// --------------------------------------------------------------------------------------------

	// ĳ���� �α� Structure : �α���, �α׾ƿ�
	// �ٷ� �ڿ� ĳ���� ������ �����.
	struct sCharLoginOut : public sLogBase
	{
		enum LOGINOUT_ERROR
		{		
			CANNOT_UPDATE_LOGOUT = 1
		};

		unsigned long	m_nIP;		// ���� ������
		unsigned short	m_usPort;	// ��Ʈ
		unsigned char	m_cAdmin;	// ����
		unsigned short	m_usDataSize[DBUpdateData::MAX_UPDATE_DB];
        unsigned short  m_usDepositData;
        unsigned long   m_dwDepositMoney;
		
		inline void InitCharLog(const SOCKADDR_IN* lpSockAddr_In, const unsigned char cAdmin, 
			const unsigned short usDataSize[DBUpdateData::MAX_UPDATE_DB], const unsigned short usDepositData, const unsigned long dwDepositMoney);

		inline int GetCharacterInfoSize() { return std::accumulate(&m_usDataSize[0], &m_usDataSize[DBUpdateData::MAX_UPDATE_DB], 0); }
	};

	inline void sCharLoginOut::InitCharLog(const SOCKADDR_IN* lpSockAddr_In, const unsigned char cAdmin,
        const unsigned short usDataSize[DBUpdateData::MAX_UPDATE_DB], const unsigned short usDepositData,
        const unsigned long dwDepositMoney)
	{
		if(NULL == lpSockAddr_In)
		{
			m_nIP = 0;
			m_usPort = 0;
		}
		else
		{
			m_nIP = lpSockAddr_In->sin_addr.s_addr;
			m_usPort = lpSockAddr_In->sin_port;
		}

		m_cAdmin = cAdmin;        
		memcpy(m_usDataSize, usDataSize, sizeof(unsigned short) * DBUpdateData::MAX_UPDATE_DB);
        m_usDepositData = usDepositData;
        m_dwDepositMoney = dwDepositMoney;
	}


	// ĳ���� �α� Structure : ĳ���ͻ���, �Ҹ�
	struct sCharCreateDelete : public sLogBase
	{
		unsigned long	m_nIP;		// ���� ������
		unsigned short	m_usPort;	// ��Ʈ
			
		inline void InitCharLog(const SOCKADDR_IN* lpSockAddr_In);
	};

	inline void sCharCreateDelete::InitCharLog(const SOCKADDR_IN* lpSockAddr_In)
	{
		if(NULL == lpSockAddr_In)
		{
			m_nIP = 0;
			m_usPort = 0;
		}
		else
		{
			m_nIP = lpSockAddr_In->sin_addr.s_addr;
			m_usPort = lpSockAddr_In->sin_port;
		}
	}


	// --------------------------------------------------------------------------------------------



	// --------------------------------------------------------------------------------------------


	// ������ ����. �ڿ� Equip, Inven, Extra, Exchange�� ������ �ٴ´�.
	struct sItemDump
	{
		enum { EQUIP_DUMP = 0, INVEN_DUMP = 1, EXTRA_DUMP = 2, EXCHANGE_DUMP = 3, MAX_DUMP = 4 };
		unsigned short m_usDataSize[MAX_DUMP];

		inline int sItemDump::GetItemDumpSize() { return std::accumulate(&m_usDataSize[0], &m_usDataSize[MAX_DUMP], 0); }
	};

	const unsigned short MAX_ITEM_DUMP_SIZE = sizeof(sItemDump) + 
		sizeof(EQUIP) + sizeof(INVEN) + sizeof(EXTRA) + sizeof(EXCHANGE);


	struct sMinItemInfo
	{
		DWORD64			m_dwItemUID;
		unsigned short	m_usProtoTypeID;
		unsigned char	m_cNowDurability;
		
        sMinItemInfo() : m_dwItemUID(0), m_usProtoTypeID(0), m_cNowDurability(0) { }
        sMinItemInfo(const Item::CItem* lpItem_In) { InitMinItemInfo(lpItem_In); }

        inline void InitMinItemInfo(const Item::CItem* lpItem_In);
	};

    inline void sMinItemInfo::InitMinItemInfo(const Item::CItem* lpItem_In)
	{
		if(NULL == lpItem_In)
		{
			m_dwItemUID = 0;
			m_usProtoTypeID = 0;
			m_cNowDurability = 0;
		}
		else
		{
			m_dwItemUID = lpItem_In->GetUID();
			m_usProtoTypeID = lpItem_In->GetPrototypeID();
			m_cNowDurability = lpItem_In->GetNumOrDurability();
		}
	}


	// MoveItem�α� - ������ sItemDump�� ����.
	struct sMoveItemLog : public sLogBase
	{
		TakeType		m_takeType;
		sMinItemInfo	m_itemInfo;		
        inline void InitMoveItemLog(const TakeType takeType, const sMinItemInfo minItemInfo);
	};

	inline void sMoveItemLog::InitMoveItemLog(const TakeType takeType, const sMinItemInfo minItemInfo)
	{
		m_takeType = takeType;
		m_itemInfo = minItemInfo;
	}

	// SwapItem�α� - ������ sItemDump�� ����.
	struct sSwapItemLog : public sLogBase
	{
		TakeType		m_srcTake;
		TakeType		m_dstTake;
		sMinItemInfo	m_srcItemInfo;
		sMinItemInfo	m_dstItemInfo;
		inline void InitSwapItemLog(const TakeType srcTake, const TakeType dstTake,
            const Item::CItem* lpSrcItem, const Item::CItem* lpDstItem);
	};

	inline void sSwapItemLog::InitSwapItemLog(const TakeType srcTake, const TakeType dstTake,
        const Item::CItem* lpSrcItem, const Item::CItem* lpDstItem)
	{
		m_srcTake = srcTake;
		m_dstTake = dstTake;
		m_srcItemInfo.InitMinItemInfo(lpSrcItem);
		m_dstItemInfo.InitMinItemInfo(lpDstItem);
	}


	// UseItem�α� - ������ sItemDump�� ����.
	struct sUseItemLog : public sLogBase
	{
        Item::ItemPos   m_usePos;
		sMinItemInfo	m_itemInfo;
        inline void InitUseItemLog(const Item::ItemPos usePos, const Item::CItem* lpUseItem);
	};

	inline void sUseItemLog::InitUseItemLog(const Item::ItemPos usePos, const Item::CItem* lpUseItem)
	{
		m_usePos = usePos;
		m_itemInfo.InitMinItemInfo(lpUseItem);
	}


    // SplitItem�α� - ������ sItemDump�� ����.
	struct sSplitItemLog : public sLogBase
	{
		TakeType		m_splitTake;
		sMinItemInfo	m_prevItem;
		sMinItemInfo	m_splitItem;
		inline void InitSplitItemLog(const TakeType splitTake, 
            const Item::CItem* lpPrevItem, const Item::CItem* lpSplitItem);
	};

	inline void sSplitItemLog::InitSplitItemLog(const TakeType splitTake, 
        const Item::CItem* lpPrevItem, const Item::CItem* lpSplitItem)
	{
		m_splitTake = splitTake;
		m_prevItem.InitMinItemInfo(lpPrevItem);
		m_splitItem.InitMinItemInfo(lpSplitItem);
	}


	// --------------------------------------------------------------------------------------------

	// UseItem�α� - ������ sItemDump�� ����.
	struct sUseLotteryLog : public sLogBase
	{
		sMinItemInfo	m_itemInfo;
        inline void InitUseLotteryLog(const Item::CItem* lpGetItem);
	};

	inline void sUseLotteryLog::InitUseLotteryLog(const Item::CItem* lpGetItem)
	{
		m_itemInfo.InitMinItemInfo(lpGetItem);
	}


	// --------------------------------------------------------------------------------------------

	// PickupItem �α� - �ڿ� ���� Item�� �ϳ� ����, ������ sItemDump�� ����.
	struct sPickupItemLog : public sLogBase
	{
		unsigned long			m_dwGold;
        Item::ItemPos   m_itemPos;		

        inline void InitPickupItemLog(const unsigned long dwGold, const Item::ItemPos itemPos);
	};

	inline void sPickupItemLog::InitPickupItemLog(const unsigned long dwGold, const Item::ItemPos itemPos)
	{
		m_dwGold = dwGold;
		m_itemPos = itemPos;		
	}

	// DropItem�α� - �ڿ� ���� Item�� �ϳ� ����, ������ sItemDump�� ����.
	struct sDropItemLog : public sLogBase
	{	
		unsigned long			m_dwGold;
		Item::ItemPos	m_itemPos;	// cNum�� �ڿ� �ٴ�Item�� ũ��.
		
		inline void InitDropItemLog(const unsigned long dwGold, const Item::ItemPos itemPos);
	};

	inline void sDropItemLog::InitDropItemLog(const unsigned long dwGold, const Item::ItemPos itemPos)
	{	
		m_dwGold = dwGold;
		m_itemPos = itemPos;		
	}


	// TradeItem�α� - ��ų� �� �� ������ ������ �ڿ� ����. ������ sItemDump�� ����.
	struct sTradeItemLog : public sLogBase
	{
        unsigned long           m_dwTraderCID;
		unsigned long			m_dwGold;
		Item::ItemPos	m_itemPos;

		inline void InitTradeItemLog(const unsigned long dwTraderCID, const unsigned long dwGold, const Item::ItemPos itemPos);
	};

	inline void sTradeItemLog::InitTradeItemLog(const unsigned long dwTraderCID, const unsigned long dwGold, const Item::ItemPos itemPos)
	{
        m_dwTraderCID = dwTraderCID;
		m_dwGold = dwGold;
		m_itemPos = itemPos;		
	}

	// InstanllSocketLog : �ڿ� Gem�� ���׷��̵� �� Item�� �ϳ��� ����. (������ ���� ����)
	struct sInstallSocketLog : public sLogBase
	{
		TakeType		m_GemAndEquip;		// Src - Gem, Dst - Equip
        unsigned char   m_cGemSize;
        unsigned char   m_cEquipSize;
		inline void InitInstallSocketLog(const TakeType GemAndEquip,
            const unsigned char cGemSize, const unsigned char cEquipSize);
	};

	inline void sInstallSocketLog::InitInstallSocketLog(const TakeType GemAndEquip,
        const unsigned char cGemSize, const unsigned char cEquipSize)
	{
		m_GemAndEquip   = GemAndEquip;
        m_cGemSize      = cGemSize;
        m_cEquipSize    = cEquipSize;
	}

    // UpgradeItemLog : ������ ���׷��̵� �α� : �ڿ� ������ ���׷��̵� �� Item�� �ϳ��� ����. (������ ���� ����)
    struct sUpgradeItemLog : public sLogBase
    {
        unsigned long   m_dwCurrentGold;
        unsigned long   m_dwUsedGold;        
        unsigned char   m_cMineralSize;
        unsigned char   m_cEquipSize;

        inline void InitUpgradeItemLog(const unsigned long dwCurrentGold, const unsigned long dwUsedGold, 
            const unsigned char cMineralSize, const unsigned char cEquipSize);
    };

    inline void sUpgradeItemLog::InitUpgradeItemLog(const unsigned long dwCurrentGold,
        const unsigned long dwUsedGold, const unsigned char cMineralSize, const unsigned char cEquipSize)
    {
        m_dwCurrentGold = dwCurrentGold;
        m_dwUsedGold    = dwUsedGold;
        m_cMineralSize  = cMineralSize;
        m_cEquipSize    = cEquipSize;
    }

    struct sUpgradeItemLogV2 : public sLogBase
    {
        unsigned long   m_dwCurrentGold;
        unsigned long   m_dwUsedGold;
        unsigned char   m_cLastUpgradeLevel;    // ���� ���׷��̵� �ܰ�. ���� ���׷��̵� �ܰ�� ���ؼ� ���������� ����.
        unsigned char   m_cMineralSize;
        unsigned char   m_cEquipSize;

        inline void InitUpgradeItemLog(const unsigned long dwCurrentGold, const unsigned long dwUsedGold, 
            const unsigned char cLastUpgradeLevel, const unsigned char cMineralSize, const unsigned char cEquipSize);
    };

    inline void sUpgradeItemLogV2::InitUpgradeItemLog(const unsigned long dwCurrentGold,
        const unsigned long dwUsedGold, const unsigned char cLastUpgradeLevel,
        const unsigned char cMineralSize, const unsigned char cEquipSize)
    {
        m_dwCurrentGold     = dwCurrentGold;
        m_dwUsedGold        = dwUsedGold;
        m_cLastUpgradeLevel = cLastUpgradeLevel;
        m_cMineralSize      = cMineralSize;
        m_cEquipSize        = cEquipSize;
    }

	// ExchangeLog : ������ ��ȯ �α�. �ڿ� ��ȯ�ϴ� �����۵��� �����.
	struct sExchangeItemLog : public sLogBase
	{
		unsigned long	m_dwGold;		// ��ȯ�ϴ� ���� ��.
		unsigned long	m_dwDstCID;		// ��ȯ ����� CID.
		unsigned short	m_usItemSize;	// ������ ���� ũ��.

		inline void InitExchangeItemLog(const unsigned long dwGold, const unsigned long dwDstCID, 
			const unsigned short usItemSize);	
	};

	inline void sExchangeItemLog::InitExchangeItemLog(const unsigned long dwGold, const unsigned long dwDstCID, 
		const unsigned short usItemSize)
	{
		m_dwGold = dwGold;
		m_dwDstCID = dwDstCID;
		m_usItemSize = usItemSize;
	}

    // sRepairItemLog : ������ ���� �α�.
    struct sRepairItemLog : public sLogBase
    {
        unsigned long           m_dwUsed;
        sMinItemInfo    m_RepairedItem;
        unsigned char   m_cPreRepairDurability;

        inline void InitRepairItemLog(const unsigned long dwUsed, const Item::CItem* lpItem, 
            const unsigned char cPreRepairDurability);
    };

    inline void sRepairItemLog::InitRepairItemLog(const unsigned long dwUsed, 
        const Item::CItem* lpItem, const unsigned char cPreRepairDurability)
    {
        m_dwUsed = dwUsed;        
        m_RepairedItem.InitMinItemInfo(lpItem);
        m_cPreRepairDurability = cPreRepairDurability;
    }

    // sChangeWeaponLog : �� �ٲٱ� �α�
    struct sChangeWeaponLog : public sLogBase
    {
        unsigned char   m_cCurrentHand;
        inline void InitCurrentHand(const unsigned char cCurrentHand);
    };

    inline void sChangeWeaponLog::InitCurrentHand(const unsigned char cCurrentHand)
    {
        m_cCurrentHand = cCurrentHand;
    }

    // sChangeRideLog : �� Ÿ�� �α�
    struct sChangeRideLog : public sLogBase
    {
        unsigned char   m_cCurrentRide;
        inline void InitCurrentRide(const unsigned char cCurrentRide);
    };

    inline void sChangeRideLog::InitCurrentRide(const unsigned char cCurrentRide)
    {
        m_cCurrentRide = cCurrentRide;
    }

	// ILLEGAL_ITEM�α� - ������ sItemDump�� ����.
	struct sIllegalItemLog : public sLogBase
	{
		Item::ItemPos   m_usePos;
		sMinItemInfo	m_itemInfo;
		inline void InitIllegalItemLog(const Item::ItemPos usePos, const Item::CItem* lpItem);
	};

	inline void sIllegalItemLog::InitIllegalItemLog(const Item::ItemPos usePos, const Item::CItem* lpItem)
	{
		m_usePos = usePos;
		m_itemInfo.InitMinItemInfo(lpItem);
	}


	// ILLEGAL_WARPPOS�α�
	struct sIllegalWarpPosLog : public sLogBase
	{
		unsigned short  m_usWarpXPos;	// ĳ���� Warp��ǥX
		unsigned short  m_usWarpYPos;	// ĳ���� Warp��ǥY
		unsigned short  m_usWarpZPos;	// ĳ���� Warp��ǥZ
		unsigned long	m_lDis;

		inline void InitCharWarpPosLog(const Position& Pos, unsigned long dis);
	};
	inline void sIllegalWarpPosLog::InitCharWarpPosLog(const Position& Pos, unsigned long dis)
	{
		m_usWarpXPos = static_cast<unsigned short>(Pos.m_fPointX);
		m_usWarpYPos = static_cast<unsigned short>(Pos.m_fPointY);
		m_usWarpZPos = static_cast<unsigned short>(Pos.m_fPointZ);
		m_lDis = dis;
	}

	// ���ǽɸ޽���	
	struct sHockDoubtLog : public sLogBase
	{
		unsigned short m_wHackID;
		unsigned short m_wKindItem;
		unsigned short m_wItemSize;

		inline void InitHackDoubt(unsigned short wHackID, unsigned short wKindItem, unsigned short wItemSize);
	};
	inline void sHockDoubtLog::InitHackDoubt(unsigned short wHackID, unsigned short wKindItem, unsigned short wItemSize)
	{
		m_wKindItem = wKindItem;
		m_wHackID = wHackID;
		m_wItemSize = wItemSize;
	}

	
	// �� �̵� �α�
    struct sTakeGoldLog : public sLogBase
    {
        unsigned long m_SrcGold;
        unsigned long m_DstGold;

        unsigned char m_cSrcPos;
        unsigned char m_cDstPos;

        inline void InitTakeGold(unsigned long SrcGold, unsigned long DstGold, unsigned char cSrcPos, unsigned char cDstPos);
    };

    inline void sTakeGoldLog::InitTakeGold(unsigned long SrcGold, unsigned long DstGold, 
        unsigned char cSrcPos, unsigned char cDstPos)
    {
        m_SrcGold = SrcGold;
        m_DstGold = DstGold;

        m_cSrcPos = cSrcPos;
        m_cDstPos = cDstPos;
    }

    // �� �̵� �α� v2 : made by sparrowhawk (2004-03-24)
    // ���� �� �̵� �α׿�, �󸶰� �̵��Ǿ����� ������ ��� �߰���.
    struct sTakeGoldLogV2 : public sLogBase
    {
        enum Purpose
        {
            MOVE_GOLD           = 0,    // �� �̵� (â��, �κ�, �������� �̵�)
            STORE_USE           = 1,    // â�� �̿��
            BUY_STORE_TAB       = 2,    // â�� �� ����
            USE_CASHBAG         = 3,    // ĳ���� ������ ���
            QUEST_AWARD         = 4,    // ����Ʈ ����
            QUEST_DEDUCT        = 5,    // ����Ʈ���� �� ����.
            ADMIN_GIVEGOLD      = 6,    // ��ڰ� ���Ƿ� ����.
            BATTLE_DEPOSIT      = 7,    // ��Ʋ�׶��忡�� â��� �Ա�
            STATE_REDISTRIBUTE  = 8,    // �ɷ�ġ ��й�.
			STARTERKIT_AWARD	= 9		// ��� ��� ��Ÿ��Ŷ��� ������ ����	
        };

        unsigned long m_SrcGold;
        unsigned long m_DstGold;
        unsigned long m_MoveGold;

        unsigned char m_cSrcPos;
        unsigned char m_cDstPos;
        unsigned char m_cPurpose;
    };

    // ������ ���� �ݱ� �α�
    struct sStallOpenCloseLog : public sLogBase
    {
        enum { MAX_STALL_NAME = 32 };
        enum StallMode
        {
            STALL_OPEN  = 0,
            STALL_CLOSE = 1        
        };

        unsigned char   m_cMode;
        char            m_szStallName[MAX_STALL_NAME];

        inline void InitStallOpenCloseLog(StallMode eStallMode, const char* szStallName);
    };

    inline void sStallOpenCloseLog::InitStallOpenCloseLog(sStallOpenCloseLog::StallMode eStallMode, const char* szStallName)
    {
        m_cMode = eStallMode;
        if(NULL == szStallName) { memset(m_szStallName, 0, MAX_STALL_NAME); }
        else { strncpy(m_szStallName, szStallName, MAX_STALL_NAME); }
    }


    // ������ ���� ���� �α�
    struct sStallEnterLeaveLog : public sLogBase
    {
        enum StallMode
        {
            STALL_ENTER = 0,
            STALL_LEAVE = 1
        };

        unsigned long m_dwCustomerCID;
        unsigned char m_cMode;
        inline void InitStallEnterLeaveLog(unsigned long dwCustomerCID, StallMode eStallMode)
        { m_dwCustomerCID = dwCustomerCID;  m_cMode = eStallMode; }
    };


    // ������ �ø��� ������ �α�
    struct sStallRegisterRemoveItemLog : public sLogBase
    {   
        enum StallMode
        {            
            STALL_UNKNOWN       = 0,
            STALL_REGISTERITEM  = 1,
            STALL_REMOVEITEM    = 2,
            STALL_CHANGEPRICE   = 3,
			STALL_CAMP_REGISTER	= 4,
			STALL_CAMP_REMOVE	= 5
        };

        TakeType        m_takeType;
        sMinItemInfo    m_itemInfo;
        unsigned long   m_dwStallPrice;
        unsigned char   m_cMode;

        inline void InitStallRegisterRemoveItemLog(StallMode eStallMode, const Item::CItem* lpItem,
            const TakeType takeType);
    };

    inline void sStallRegisterRemoveItemLog::InitStallRegisterRemoveItemLog(
        sStallRegisterRemoveItemLog::StallMode eStallMode, const Item::CItem* lpItem, const TakeType takeType)
    {        
        m_itemInfo.InitMinItemInfo(lpItem);

        m_takeType      = takeType;
        m_dwStallPrice  = NULL != lpItem ? lpItem->GetStallPrice() : 0;
        m_cMode         = eStallMode;
    }

    struct sItemAttachOption : public sLogBase
    {
        // ����ü �ڿ� ���� ������ / ���� ������ / �� �������� �پ� �´�

        unsigned long   m_dwOldInvenGold;
        unsigned long   m_dwNewInvenGold;

        unsigned char   m_cSelectedOptionIndex;
        unsigned char   m_cOldItemLen;
        unsigned char   m_cUseItemLen;
        unsigned char   m_cNewItemLen;
    };

    struct sItemCompensation : public sLogBase
    {
        // ����ü �ڿ� ���� ������ / ���� ���� �������� �پ� �´�.

        unsigned long   m_dwOldInvenGold;
        unsigned long   m_dwNewInvenGold;

        unsigned char   m_cOldItemLen;
        unsigned char   m_cNewItemLen;
    };

	// --------------------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------------------

	// ĳ���� ������ �α�
	struct sCharLevelUpLog : public sLogBase
	{
		unsigned char	m_cLevel;	// ����
		unsigned short	m_usIP;		// ���� IP
		inline void InitCharLevelUpLog(const unsigned short usIP, const unsigned char cLevel);
	};

	inline void sCharLevelUpLog::InitCharLevelUpLog(const unsigned short usIP, const unsigned char cLevel)
	{
		m_usIP = usIP;
		m_cLevel = cLevel;	
	}

	// ĳ���� ��ġ ���ε� �α� - ���� ��ġ�� �״�� ���ε� ��.
	struct sCharBindPosLog : public sLogBase
	{
		unsigned long	m_dwNPCID;
		inline void InitCharBindPosLog(const unsigned long dwNPCID) { m_dwNPCID = dwNPCID; }
	};


	// ĳ���� ��� �α�
	struct sCharDeadLog : public sLogBase
	{
		DWORD64			m_nPrevExp;			// ��� �� Exp
		DWORD64			m_nNextExp;			// ��� �� Exp
		unsigned long	m_dwAttackerCID;	// ���� �� CID
		unsigned char	m_cLevel;			// ���� ����

		inline void InitCharDeadLog(const DWORD64 nPrevExp, const DWORD64 nNextExp,
			const unsigned long dwAttackerCID, const unsigned char cLevel);
	};

    inline void sCharDeadLog::InitCharDeadLog(const DWORD64 nPrevExp, const DWORD64 nNextExp, 
											const unsigned long dwAttackerCID, const unsigned char cLevel)
	{
		m_nPrevExp = nPrevExp;
		m_nNextExp = nNextExp;
		m_dwAttackerCID = dwAttackerCID;
		m_cLevel = cLevel;
	}

	// ĳ���� ������ �α�
	struct sCharRespawnLog : public sLogBase
	{
		DWORD64		m_nPrevExp;		// ������ �� Exp
		DWORD64		m_nNextExp;		// ������ �� Exp

		inline void InitCharRespawnLog(const DWORD64 nPrevExp, const DWORD64 nNextExp);
	};

	
    inline void sCharRespawnLog::InitCharRespawnLog(const DWORD64 nPrevExp, const DWORD64 nNextExp)
	{
		m_nPrevExp = nPrevExp;	
		m_nNextExp = nNextExp;
	}

	// ������ġ ȹ�� �α�
	struct sFameGetBattleLog : public sLogBase
	{
		unsigned long	m_dwPrevFame;		// ȹ�� �� ��
		unsigned long	m_dwNextFame;		// ȹ�� �� ��
		unsigned long   m_dwPrevMileage;	// ȹ�� �� ����Ŵ�
		unsigned long   m_dwNextMileage;	// ȹ�� �� ����Ŵ�
		unsigned long	m_dwEnemyCID;		// ���� ĳ���� CID
		unsigned long   m_dwOurPartyUID;	// �Ʊ� ��Ƽ UID

		inline void InitFameGetBattleLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwPrevMileage, const unsigned long dwNextMileage, 
			const unsigned long dwEnemyCID, const unsigned long dwOurPartyUID);
	};

	inline void sFameGetBattleLog::InitFameGetBattleLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwPrevMileage, const unsigned long dwNextMileage, 
		const unsigned long dwEnemyCID, const unsigned long dwOurPartyUID)
	{
		m_dwPrevFame	= dwPrevFame;
		m_dwNextFame	= dwNextFame;
		m_dwPrevMileage	= dwPrevMileage;
		m_dwNextMileage	= dwNextMileage;
		m_dwEnemyCID	= dwEnemyCID;
		m_dwOurPartyUID	= dwOurPartyUID;
	}

	// ������ġ ���� �α�
	struct sFameLoseBattleLog : public sLogBase
	{
		unsigned long	m_dwPrevFame;		// �ұ� �� ��
		unsigned long	m_dwNextFame;		// ���� �� ��
		unsigned long	m_dwEnemyCID;		// ���� ĳ���� CID
		unsigned long   m_dwOurPartyUID;	// �Ʊ� ��Ƽ UID
		unsigned char	m_cLevel;			// ���� ����

		inline void InitFameLoseBattleLog(const unsigned long dwPrevFame, const unsigned long dwNextFame,
			const unsigned long dwEnemyCID, const unsigned long dwOurPartyUID);
	};

	inline void sFameLoseBattleLog::InitFameLoseBattleLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, 
		const unsigned long dwEnemyCID, const unsigned long dwOurPartyUID)
	{
		m_dwPrevFame	= dwPrevFame;
		m_dwNextFame	= dwNextFame;
		m_dwEnemyCID	= dwEnemyCID;
		m_dwOurPartyUID	= dwOurPartyUID;
	}

	// �����ġ ȹ�� �α�
	struct sFameGetCampLog : public sLogBase
	{
		unsigned long	m_dwPrevFame;		// ȹ�� �� ��
		unsigned long	m_dwNextFame;		// ȹ�� �� ��
		unsigned long	m_dwEnemyGID;		// ���� ��� GID

		inline void InitFameGetCampLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwEnemyGID);
	};

	inline void sFameGetCampLog::InitFameGetCampLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwEnemyGID)
	{
		m_dwPrevFame	= dwPrevFame;
		m_dwNextFame	= dwNextFame;
		m_dwEnemyGID	= dwEnemyGID;
	}

	// �����ġ ���� �α�
	struct sFameLoseCampLog : public sLogBase
	{
		unsigned long	m_dwPrevFame;		// �ұ� �� ��
		unsigned long	m_dwNextFame;		// ���� �� ��
		unsigned long	m_dwEnemyGID;		// ���� ��� GID

		inline void InitFameLoseCampLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwEnemyGID);
	};

	inline void sFameLoseCampLog::InitFameLoseCampLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwEnemyGID)
	{
		m_dwPrevFame	= dwPrevFame;
		m_dwNextFame	= dwNextFame;
		m_dwEnemyGID	= dwEnemyGID;
	}

	// ����Ʈ ������ ����ȹ�� �α�
	struct sQuestGetRewardLog : public sLogBase
	{
		DWORD64			 m_dwItemUID;		// ������� ������UID
		unsigned long	 m_dwExp;			// ������� ����ġ
		unsigned long	 m_dwGold;			// ������� ���
		unsigned long	 m_dwFame;			// ������� ��
		unsigned long	 m_dwMileage;		// ������� ����޴�
		unsigned short	 m_wQuestID;		// ������� ����ƮID

		inline void InitQuestGetRewardLog(const unsigned short wQuestID, const DWORD64 dwItemUID,
			const unsigned long dwExp, const unsigned long dwGold, const unsigned long dwFame, const unsigned long dwMileage);
	};

	inline void sQuestGetRewardLog::InitQuestGetRewardLog(const unsigned short wQuestID, const DWORD64 dwItemUID,
		const unsigned long dwExp, const unsigned long dwGold, const unsigned long dwFame, const unsigned long dwMileage)
	{
		m_dwItemUID	= dwItemUID;
		m_dwExp		= dwExp;
		m_dwGold	= dwGold;
		m_dwFame	= dwFame;
		m_dwMileage	= dwMileage;
		m_wQuestID	= wQuestID;
	}



	// --------------------------------------------------------------------------------------------



	// --------------------------------------------------------------------------------------------

	// ���� ���� ���� �͵�. ��Ƽ ����, Ż��, ���� ����, ����ġ ȹ��, ����.

	// ���� ���� �α� / �ڿ� item������ DWORD�� m_cItemNum�� ��ŭ ����´�.
	struct sMonsterDeadLog : public sLogBase
	{
		unsigned long	m_dwMonsterCID;		// �״� ���� CID
		unsigned char	m_cMonsterLevel;    // ���� ����
        unsigned char   m_cDropItemNum;     // ������ ����
	};

	// --------------------------------------------------------------------------------------------
    

    // ��� ���� �α׵�.
    struct sGuildLog : public sLogBase
    {
        enum
        {
            MAX_GUILD_NAME_LEN  = 11,
            MAX_NAME_LEN        = 16
        };

        enum
        {
            REQUEST     = 0,
            RESULT      = 1
        };

        unsigned long   m_dwGID;            // ��� ID
        unsigned long   m_dwSrcCID;         // ������ ID
        unsigned long   m_dwDstCID;         // ����� ID
       
        unsigned char   m_cType;            // �α� Ÿ�� : ��û/���(����/����)        
		unsigned char	m_cCmd;				// ��� �α� Ŀ�ǵ�(��� ����ݽ� ���.)
        unsigned char   m_cData;            // �߰� ������
        unsigned short  m_usExtraDataSize;  // �߰� ������ ����
    };


    // �� �̵� ���� �α�
    struct sMoveZoneLog : public sLogBase
    {
        unsigned char   m_cDstZone;
        unsigned char   m_cDstChannel;
    };
};


#pragma pack()

#endif