#ifndef _CCELL_H_
#define _CCELL_H_

#pragma once

#pragma warning(disable:4800)

#include <list>
#include <algorithm>
#include <climits>
#include <boost/pool/pool_alloc.hpp>
#include <Creature/CreatureStructure.h>

#include <Network/Packet/PacketStruct/CharMovepacket.h>
#include <Network/Packet/PacketStruct/AddressPacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>

// 전방 참조
class   CParty;
class   CCreature;
class   CAggresiveCreature;
class   CMonster;
class   CCharacter;
class	CSiegeObject;
class	CBuffer;
struct  PktBase;
struct  PktMVEx;

namespace Item
{
    class CItem;
};


class CCell 
{
public:

	enum ErrorCode
	{
		S_SUCCESS			= 0,		
		S_AUTO_ROUTING		= 1,
		
		E_NOT_ENOUGH_MEMORY = 2,
		E_NOT_OWNER_OF_ITEM = 3,		
		E_NOT_ITEM			= 4,
		E_CREATE_ITEM_FAIL	= 5,
		E_NOT_EXIST_GUILD	= 6,
		E_GOLD_OVERFLOW		= 7
	};

	enum Const
	{ 
		CONNECT_NUM				= 9,

		CHECK_TIME				= 100,			// 아이템이 사라져야 하는지 체크하는 간격 (10초)
		ITEM_RIGHT_TIME			= 1600,			// 플레이어가 드랍한 아이템의 경우 소유권은 10초, 다시 2분 30초 후엔 사라진다.
		ITEM_LIFE_TIME			= 1800,			// 맵에 아이템이 남아있는 시간 (3분)
		NO_OWNER_TIME			= 1500,			// 아이템에 소유권이 지정되어 있지 않은 시간 (2분 30초)

		BROADCASTING_TIME		= 10,			// 기존 브로드 캐스팅 타임 (1초)
        
		MONEY_BIT				= 0x80000000,		
		TYPE_CHECK_BIT			= 0xF0000000,
		ITEM_PROTOTYPE_ID_BIT	= 0x0000FFFF,		
		MAX_MONEY_AMOUNT		= 0x0FFFFFFF
	};

	enum TurnConst
	{
		TURN_OF_CHARACTER		= 0,
		TURN_OF_SIEGE_OBJECT	= 1,
		TURN_OF_MONSTER			= 2,

		TURN_END				= 3
	};

	enum Weather
	{
		WEATHER_FINE			= 0,			// 맑음
		WEATHER_RAIN			= 1,			// 비
		WEATHER_SNOW			= 2,			// 눈
		
		MAX_WEATHER				= 3
	};

	enum ItemAutoRouting
	{
		NONE		= 0,		// 루팅 하지 않는다.
		PARTY		= 1,		// 파티원에게 루팅
		GUILD		= 2,		// 길드원에게 루팅
	};

    enum ConnectCell    
	{ 
		NO			= 0, 
		UP			= 1, 
		DOWN		= 2, 
		LEFT		= 3, 
		RIGHT		= 4, 
		UPPERLEFT	= 5, 
		UPPERRIGHT	= 6, 
		LOWERLEFT	= 7, 
		LOWERRIGHT	= 8
	};

	enum CellSize       
	{ 
		CELL_SIZE_DEFAULT		= 128,
		CELL_SIZE_BATTLE_SERVER	= 70,
		CELL_DISTANCE			= 32, 
		CELL_RESOLUTION			= 5 
	};
	static unsigned char ms_CellSize;

    struct IDField
	{
        enum 
		{ 
			MAX_UID		= UCHAR_MAX 
		};

		unsigned long	dwUID;				// 유니크 ID (생성된 순서에 따라 차례대로...)	
		unsigned short	wMapIndex;			// 가상 맵의 경우 그 인덱스
		unsigned char   cCellX;				// 셀 인덱스 X
		unsigned char   cCellZ;				// 셀 인덱스 Z
	};

    struct ItemInfo
	{
        Position		m_Pos;					// 월드맵 상의 위치

		union
		{
			unsigned __int64	m_nUniqueID;	// 유니크 ID
			IDField				m_Field;

		} UID;

        Item::CItem*    m_lpItem;			    // 아이템
		unsigned long	m_dwGold;				// 돈

		unsigned long	m_dwOwnerID;			// 소유권		
		unsigned short	m_wPulse;				// 소멸까지 남은 펄스	

		unsigned char	m_cAutoRouting;			// 오토루팅될 아이템인가? (ItemAutoRouting enum 참고)

		ItemInfo();
        void MakeFieldObject(FieldObject& fieldObject);
	};

    typedef std::list<CMonster*,        boost::fast_pool_allocator<CMonster*> >			MonsterList;
    typedef std::list<CCharacter*,      boost::fast_pool_allocator<CCharacter*> >		CharacterList;
    typedef std::list<ItemInfo,         boost::fast_pool_allocator<ItemInfo> >			ItemList;
    typedef std::list<CSiegeObject*,    boost::fast_pool_allocator<CSiegeObject*> >	    SiegeObjectList;


    //--------------------------------------------------------------------------------------------
    // 초기화

	CCell();
	~CCell();

    
    void Initialize(unsigned char cIndexX, unsigned char cIndexZ); // 셀 좌표를 초기화하고, 소트된 링크를 생성한다.
	void SetMapIndex(unsigned short wMapIndex)					  { m_wMapIndex = wMapIndex; }
	unsigned short GetMapIndex()								  { return m_wMapIndex;		 }
	

    //--------------------------------------------------------------------------------------------
    // interface		

	CCell* GetConnectCell(unsigned int Dir);
	void SetConnectCell(unsigned int Dir, CCell* lpConnectedCell);
	bool IsNearCell(CCell* lpNearCell);


    //--------------------------------------------------------------------------------------------
	// 셀 정보

	unsigned char	GetIndexX() const	{ return m_cIndexX;					}
	unsigned char	GetIndexZ() const	{ return m_cIndexZ;					}
	size_t  GetMonsterNum()     const	{ return m_lstMonster.size();		}
	size_t  GetCharacterNum()   const	{ return m_lstCharacter.size();		}
	size_t  GetItemNum()        const	{ return m_lstItem.size();			}
	size_t	GetSiegeObjectNum() const	{ return m_lstSiegeObject.size();	}
    size_t  GetNearCellCharacterNum() const;


    //--------------------------------------------------------------------------------------------
	// 셀에 크리쳐 추가 및 삭제, 얻어 오기.

	void SetCreature(unsigned long dwCID, CCreature* lpCreature, CCell* lpLastLogoutCell = 0);
	void DeleteCreature(unsigned long dwCID, CCell* lpPrepareLoginCell = 0); 
	CCreature* GetCreature(unsigned long dwCID);
	
	CAggresiveCreature* GetFirstAggresiveCreature();
	CAggresiveCreature* GetNextAggresiveCreature();

	void KillAll(CCharacter* lpAttacker);


	//--------------------------------------------------------------------------------------------
	// 몬스터 관련 처리

	bool IsMonster() const { return !(m_lstMonster.empty()); }
	CMonster* GetFirstMonster();
    CMonster* GetNextMonster() { return (++m_MonsterIt != m_lstMonster.end()) ? *m_MonsterIt : NULL; }

	void LogoutAllMonster() { m_lstMonster.clear(); }


	//--------------------------------------------------------------------------------------------
	// 공성 오브젝트 관련 처리

	bool IsSiegeObject()	{ return !(m_lstSiegeObject.empty()); }
	CSiegeObject* GetFirstAirShip();
	CSiegeObject* GetNextAirShip();
	CSiegeObject* GetFirstSiegeObject();
	CSiegeObject* GetNextSiegeObject();

	void	UpgradeByEmblem(unsigned long dwCID);
	void	DegradeByEmblem();

	bool	IsDetectionCell()					{ return ( 0 != m_dwCastleEmblemCID ); }
	void	DetectionAttack(CSiegeObject* lpEmblem, CAggresiveCreature* lpTargetCreature);
	bool	SendStealthInfo(CCharacter& character, bool bUseStealth);
		

    //--------------------------------------------------------------------------------------------
	// 캐릭터 관련 처리

	bool IsCharacter() { return !(m_lstCharacter.empty()); }
	CCharacter* GetFirstCharacter();
    CCharacter* GetNextCharacter() { return (++m_CharacterIt != m_lstCharacter.end()) ? *m_CharacterIt : NULL; }

	template<typename FnProcess>
		inline void ProcessAllMonster(FnProcess fnProcess) 
    {
		std::for_each(m_lstMonster.begin(), m_lstMonster.end(), fnProcess);
	}

	template<typename FnProcess>
		inline void ProcessAllCharacter(FnProcess fnProcess) 
    {
		std::for_each(m_lstCharacter.begin(), m_lstCharacter.end(), fnProcess);
	}

	void RespawnAllCharacter(unsigned char cExceptNation);
	
    //--------------------------------------------------------------------------------------------
	// 아이템 관련 처리

	bool IsItem(void) { return !(m_lstItem.empty()); }

	void SetItem(const Position& Pos, Item::CItem* lpItem, unsigned long dwGold, 
		unsigned long dwOwnerID, unsigned char cAutoRouting, CCell::ItemInfo& cellItemInfo);

	CCell::ErrorCode GetItem(unsigned long dwCreatureID, unsigned __int64 nItemInfoID, 
        Item::CItem** lppItem, unsigned long& dwMoney_Out);

	void CheckDeleteItem(void);
	void DeleteAllItem(void);


    //--------------------------------------------------------------------------------------------
	// 셀 관련 브로드 캐스팅

    void PrepareBroadCast();
	void BroadCast(unsigned long dwCurrentPulse);

    // 만든 패킷을, Wrap하지 말고 보낸다(내부에서 Wrap함. srvState나 usError는 넣을수 없다.)
    // 패킷 순서가 바뀔 수 있으므로 주의한다.

	void SendAllNearCellCharacter(const PktBase* lpPktBase, unsigned short usLength, unsigned char cCMD);
	void SendAllCharacter(const PktBase* lpPktBase, unsigned short usLength, unsigned char cCMD);

    void SendNowAllNearCellCharacter(const char* szData, unsigned short usLength, unsigned char cCMD);
    void SendNowAllCharacter(const char* szData, unsigned short usLength, unsigned char cCMD);

	void SendAttackInfo(unsigned long AttackerID_In, const AtType &AtType_In, 
        unsigned char DefenserNum_In, DefenserNode* lpNode_In);

	void SendCastObjectInfo(unsigned long SenderID, unsigned long ReceiverID, CastObject& CastObject_In);

private:

    //--------------------------------------------------------------------------------------------
    // 패킷 전송 관련 (내부에서만 호출하는 함수들)

    void SendPullDownInfo(unsigned long dwOwnerID, ItemInfo& itemInfo);
    void SendPickUpInfo(unsigned long dwCreatureID, unsigned __int64 nItemInfoID);
        
    void SendCellLogin(CAggresiveCreature* lpAggresiveCreature, CCell* lpLastLogoutCell);
    void SendCellLogout(CAggresiveCreature* lpAggresiveCreature, CCell* lpPrepareLoginCell);

    // 버퍼를 전부 Release한다.
    static void ReleaseAllBuffer(CBuffer*& lpBuffer);

    //--------------------------------------------------------------------------------------------
    // 데이터

    CCell*			m_lpConnectCell[CONNECT_NUM];           // 주변 연결 리스트. 자기 자신도 포함한다.
    CCell*          m_lpSortedConnectedCell[CONNECT_NUM];   // 주변 연결 리스트를 포인터순으로 소팅한 배열.

    CharacterList	m_lstCharacter;
    MonsterList		m_lstMonster;
    ItemList		m_lstItem;
    SiegeObjectList	m_lstSiegeObject;               // 단지 리스트를 가지고 있을 뿐, 모든 공성 오브젝트는 CSiegeObjectMgr 이 관리한다.
    
    MonsterList::iterator		m_MonsterIt;        // 범용적... 한 녀석이 다 쓰기 전에 다른 녀석이 달라고 하면 곤란합니다.
    CharacterList::iterator		m_CharacterIt;      // 범용적... 한 녀석이 다 쓰기 전에 다른 녀석이 달라고 하면 곤란합니다.
    SiegeObjectList::iterator	m_SiegeObjectIt;    // 범용적... 한 녀석이 다 쓰기 전에 다른 녀석이 달라고 하면 곤란합니다.

    CBuffer*        m_lpBroadcast2ndBuffer;         // 브로드캐스트 2nd버퍼(새 브로드캐스트 버퍼, 캐릭터 및 아이템용)

    unsigned long	m_dwCastleEmblemCID;	        // 성 상징물의 디텍션 범위에 있는 셀이면 상징물의 CID를 가지고 있다.

    unsigned short	m_wMapIndex;                    // 맵 인덱스
    unsigned char	m_cIndexX;                      // 맵 X좌표
    unsigned char	m_cIndexZ;                      // 맵 Z좌표
    
    unsigned char	m_cTurnOfGetAggresiveCreature;  

	CCell::Weather	m_eWeather;						// 현재 날씨
};

#endif