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


// 아이템 구조가 바뀜에 따라서, 로그도 다른 방식으로 남기게 됨.
namespace GAMELOG
{
	typedef unsigned char CMDType;
	typedef unsigned char ERRType;

	// 기본 로그 Structure
	struct sLogBase
	{
		unsigned long	m_dwUID;	// UserID
		unsigned long	m_dwCID;	// Character ID
		
		time_t	        m_time;		// 로그 시간

		unsigned short  m_usXPos;	// 캐릭터 좌표X
		unsigned short  m_usYPos;	// 캐릭터 좌표Y
		unsigned short  m_usZPos;	// 캐릭터 좌표Z

		CMDType	        m_cCmd;		// 커맨드
		ERRType	        m_cErr;		// 에러 코드

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

	// 캐릭터 로그 Structure : 로그인, 로그아웃
	// 바로 뒤에 캐릭터 덤프를 기록함.
	struct sCharLoginOut : public sLogBase
	{
		enum LOGINOUT_ERROR
		{		
			CANNOT_UPDATE_LOGOUT = 1
		};

		unsigned long	m_nIP;		// 접속 아이피
		unsigned short	m_usPort;	// 포트
		unsigned char	m_cAdmin;	// 어드민
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


	// 캐릭터 로그 Structure : 캐릭터생성, 소멸
	struct sCharCreateDelete : public sLogBase
	{
		unsigned long	m_nIP;		// 접속 아이피
		unsigned short	m_usPort;	// 포트
			
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


	// 아이템 덤프. 뒤에 Equip, Inven, Extra, Exchange의 덤프가 붙는다.
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


	// MoveItem로그 - 에러시 sItemDump가 붙음.
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

	// SwapItem로그 - 에러시 sItemDump가 붙음.
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


	// UseItem로그 - 에러시 sItemDump가 붙음.
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


    // SplitItem로그 - 에러시 sItemDump가 붙음.
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

	// UseItem로그 - 에러시 sItemDump가 붙음.
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

	// PickupItem 로그 - 뒤에 집은 Item이 하나 붙음, 에러시 sItemDump가 붙음.
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

	// DropItem로그 - 뒤에 버린 Item이 하나 붙음, 에러시 sItemDump가 붙음.
	struct sDropItemLog : public sLogBase
	{	
		unsigned long			m_dwGold;
		Item::ItemPos	m_itemPos;	// cNum은 뒤에 붙는Item의 크기.
		
		inline void InitDropItemLog(const unsigned long dwGold, const Item::ItemPos itemPos);
	};

	inline void sDropItemLog::InitDropItemLog(const unsigned long dwGold, const Item::ItemPos itemPos)
	{	
		m_dwGold = dwGold;
		m_itemPos = itemPos;		
	}


	// TradeItem로그 - 사거나 팔 때 아이템 정보가 뒤에 붙음. 에러시 sItemDump가 붙음.
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

	// InstanllSocketLog : 뒤에 Gem과 업그레이드 된 Item이 하나씩 붙음. (에러시 붙지 않음)
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

    // UpgradeItemLog : 아이템 업그레이드 로그 : 뒤에 광물과 업그레이드 된 Item이 하나씩 붙음. (에러시 붙지 않음)
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
        unsigned char   m_cLastUpgradeLevel;    // 이전 업그레이드 단계. 현재 업그레이드 단계와 비교해서 떨어졌으면 실패.
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

	// ExchangeLog : 아이템 교환 로그. 뒤에 교환하는 아이템들이 따라옴.
	struct sExchangeItemLog : public sLogBase
	{
		unsigned long	m_dwGold;		// 교환하는 돈의 양.
		unsigned long	m_dwDstCID;		// 교환 상대의 CID.
		unsigned short	m_usItemSize;	// 아이템 덤프 크기.

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

    // sRepairItemLog : 아이템 수리 로그.
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

    // sChangeWeaponLog : 손 바꾸기 로그
    struct sChangeWeaponLog : public sLogBase
    {
        unsigned char   m_cCurrentHand;
        inline void InitCurrentHand(const unsigned char cCurrentHand);
    };

    inline void sChangeWeaponLog::InitCurrentHand(const unsigned char cCurrentHand)
    {
        m_cCurrentHand = cCurrentHand;
    }

    // sChangeRideLog : 말 타기 로그
    struct sChangeRideLog : public sLogBase
    {
        unsigned char   m_cCurrentRide;
        inline void InitCurrentRide(const unsigned char cCurrentRide);
    };

    inline void sChangeRideLog::InitCurrentRide(const unsigned char cCurrentRide)
    {
        m_cCurrentRide = cCurrentRide;
    }

	// ILLEGAL_ITEM로그 - 에러시 sItemDump가 붙음.
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


	// ILLEGAL_WARPPOS로그
	struct sIllegalWarpPosLog : public sLogBase
	{
		unsigned short  m_usWarpXPos;	// 캐릭터 Warp좌표X
		unsigned short  m_usWarpYPos;	// 캐릭터 Warp좌표Y
		unsigned short  m_usWarpZPos;	// 캐릭터 Warp좌표Z
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

	// 핵의심메시지	
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

	
	// 돈 이동 로그
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

    // 돈 이동 로그 v2 : made by sparrowhawk (2004-03-24)
    // 이전 돈 이동 로그에, 얼마가 이동되었는지 내용이 없어서 추가함.
    struct sTakeGoldLogV2 : public sLogBase
    {
        enum Purpose
        {
            MOVE_GOLD           = 0,    // 돈 이동 (창고, 인벤, 등등사이의 이동)
            STORE_USE           = 1,    // 창고 이용료
            BUY_STORE_TAB       = 2,    // 창고 탭 구매
            USE_CASHBAG         = 3,    // 캐쉬백 아이템 사용
            QUEST_AWARD         = 4,    // 퀘스트 보상
            QUEST_DEDUCT        = 5,    // 퀘스트에서 돈 제거.
            ADMIN_GIVEGOLD      = 6,    // 운영자가 임의로 지원.
            BATTLE_DEPOSIT      = 7,    // 배틀그라운드에서 창고로 입금
            STATE_REDISTRIBUTE  = 8,    // 능력치 재분배.
			STARTERKIT_AWARD	= 9		// 길드 요새 스타터킷대신 돈으로 보상	
        };

        unsigned long m_SrcGold;
        unsigned long m_DstGold;
        unsigned long m_MoveGold;

        unsigned char m_cSrcPos;
        unsigned char m_cDstPos;
        unsigned char m_cPurpose;
    };

    // 노점상 열고 닫기 로그
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


    // 노점상 입장 퇴장 로그
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


    // 아이템 올리고 내리기 로그
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
        // 구조체 뒤에 이전 아이템 / 제물 아이템 / 새 아이템이 붙어 온다

        unsigned long   m_dwOldInvenGold;
        unsigned long   m_dwNewInvenGold;

        unsigned char   m_cSelectedOptionIndex;
        unsigned char   m_cOldItemLen;
        unsigned char   m_cUseItemLen;
        unsigned char   m_cNewItemLen;
    };

    struct sItemCompensation : public sLogBase
    {
        // 구조체 뒤에 이전 아이템 / 보상 받은 아이템이 붙어 온다.

        unsigned long   m_dwOldInvenGold;
        unsigned long   m_dwNewInvenGold;

        unsigned char   m_cOldItemLen;
        unsigned char   m_cNewItemLen;
    };

	// --------------------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------------------

	// 캐릭터 레벨업 로그
	struct sCharLevelUpLog : public sLogBase
	{
		unsigned char	m_cLevel;	// 레벨
		unsigned short	m_usIP;		// 현재 IP
		inline void InitCharLevelUpLog(const unsigned short usIP, const unsigned char cLevel);
	};

	inline void sCharLevelUpLog::InitCharLevelUpLog(const unsigned short usIP, const unsigned char cLevel)
	{
		m_usIP = usIP;
		m_cLevel = cLevel;	
	}

	// 캐릭터 위치 바인드 로그 - 현재 위치가 그대로 바인드 됨.
	struct sCharBindPosLog : public sLogBase
	{
		unsigned long	m_dwNPCID;
		inline void InitCharBindPosLog(const unsigned long dwNPCID) { m_dwNPCID = dwNPCID; }
	};


	// 캐릭터 사망 로그
	struct sCharDeadLog : public sLogBase
	{
		DWORD64			m_nPrevExp;			// 사망 전 Exp
		DWORD64			m_nNextExp;			// 사망 후 Exp
		unsigned long	m_dwAttackerCID;	// 때린 놈 CID
		unsigned char	m_cLevel;			// 현재 레벨

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

	// 캐릭터 리스폰 로그
	struct sCharRespawnLog : public sLogBase
	{
		DWORD64		m_nPrevExp;		// 리스폰 전 Exp
		DWORD64		m_nNextExp;		// 리스폰 후 Exp

		inline void InitCharRespawnLog(const DWORD64 nPrevExp, const DWORD64 nNextExp);
	};

	
    inline void sCharRespawnLog::InitCharRespawnLog(const DWORD64 nPrevExp, const DWORD64 nNextExp)
	{
		m_nPrevExp = nPrevExp;	
		m_nNextExp = nNextExp;
	}

	// 전투명성치 획득 로그
	struct sFameGetBattleLog : public sLogBase
	{
		unsigned long	m_dwPrevFame;		// 획득 전 명성
		unsigned long	m_dwNextFame;		// 획득 후 명성
		unsigned long   m_dwPrevMileage;	// 획득 전 공헌매달
		unsigned long   m_dwNextMileage;	// 획득 후 공헌매달
		unsigned long	m_dwEnemyCID;		// 적대 캐릭터 CID
		unsigned long   m_dwOurPartyUID;	// 아군 파티 UID

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

	// 전투명성치 잃은 로그
	struct sFameLoseBattleLog : public sLogBase
	{
		unsigned long	m_dwPrevFame;		// 잃기 전 명성
		unsigned long	m_dwNextFame;		// 잃은 후 명성
		unsigned long	m_dwEnemyCID;		// 적대 캐릭터 CID
		unsigned long   m_dwOurPartyUID;	// 아군 파티 UID
		unsigned char	m_cLevel;			// 현재 레벨

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

	// 요새명성치 획득 로그
	struct sFameGetCampLog : public sLogBase
	{
		unsigned long	m_dwPrevFame;		// 획득 전 명성
		unsigned long	m_dwNextFame;		// 획득 후 명성
		unsigned long	m_dwEnemyGID;		// 적대 길드 GID

		inline void InitFameGetCampLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwEnemyGID);
	};

	inline void sFameGetCampLog::InitFameGetCampLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwEnemyGID)
	{
		m_dwPrevFame	= dwPrevFame;
		m_dwNextFame	= dwNextFame;
		m_dwEnemyGID	= dwEnemyGID;
	}

	// 요새명성치 잃은 로그
	struct sFameLoseCampLog : public sLogBase
	{
		unsigned long	m_dwPrevFame;		// 잃기 전 명성
		unsigned long	m_dwNextFame;		// 잃은 후 명성
		unsigned long	m_dwEnemyGID;		// 적대 길드 GID

		inline void InitFameLoseCampLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwEnemyGID);
	};

	inline void sFameLoseCampLog::InitFameLoseCampLog(const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwEnemyGID)
	{
		m_dwPrevFame	= dwPrevFame;
		m_dwNextFame	= dwNextFame;
		m_dwEnemyGID	= dwEnemyGID;
	}

	// 퀘스트 수행후 보상획득 로그
	struct sQuestGetRewardLog : public sLogBase
	{
		DWORD64			 m_dwItemUID;		// 보상받은 아이템UID
		unsigned long	 m_dwExp;			// 보상받은 경험치
		unsigned long	 m_dwGold;			// 보상받은 골드
		unsigned long	 m_dwFame;			// 보상받은 명성
		unsigned long	 m_dwMileage;		// 보상받은 공헌메달
		unsigned short	 m_wQuestID;		// 보상받은 퀘스트ID

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

	// 아직 넣지 않은 것들. 파티 가입, 탈퇴, 몬스터 죽음, 경험치 획득, 공격.

	// 몬스터 죽음 로그 / 뒤에 item정보가 DWORD로 m_cItemNum개 만큼 따라온다.
	struct sMonsterDeadLog : public sLogBase
	{
		unsigned long	m_dwMonsterCID;		// 죽는 몬스터 CID
		unsigned char	m_cMonsterLevel;    // 몬스터 레벨
        unsigned char   m_cDropItemNum;     // 아이템 개수
	};

	// --------------------------------------------------------------------------------------------
    

    // 길드 관련 로그들.
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

        unsigned long   m_dwGID;            // 길드 ID
        unsigned long   m_dwSrcCID;         // 행위자 ID
        unsigned long   m_dwDstCID;         // 대상자 ID
       
        unsigned char   m_cType;            // 로그 타입 : 요청/결과(성공/실패)        
		unsigned char	m_cCmd;				// 길드 로그 커맨드(길드 입출금시 사용.)
        unsigned char   m_cData;            // 추가 데이터
        unsigned short  m_usExtraDataSize;  // 추가 데이터 길이
    };


    // 존 이동 관련 로그
    struct sMoveZoneLog : public sLogBase
    {
        unsigned char   m_cDstZone;
        unsigned char   m_cDstChannel;
    };
};


#pragma pack()

#endif