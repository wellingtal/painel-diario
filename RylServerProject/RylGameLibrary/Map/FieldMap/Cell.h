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

// ���� ����
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

		CHECK_TIME				= 100,			// �������� ������� �ϴ��� üũ�ϴ� ���� (10��)
		ITEM_RIGHT_TIME			= 1600,			// �÷��̾ ����� �������� ��� �������� 10��, �ٽ� 2�� 30�� �Ŀ� �������.
		ITEM_LIFE_TIME			= 1800,			// �ʿ� �������� �����ִ� �ð� (3��)
		NO_OWNER_TIME			= 1500,			// �����ۿ� �������� �����Ǿ� ���� ���� �ð� (2�� 30��)

		BROADCASTING_TIME		= 10,			// ���� ��ε� ĳ���� Ÿ�� (1��)
        
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
		WEATHER_FINE			= 0,			// ����
		WEATHER_RAIN			= 1,			// ��
		WEATHER_SNOW			= 2,			// ��
		
		MAX_WEATHER				= 3
	};

	enum ItemAutoRouting
	{
		NONE		= 0,		// ���� ���� �ʴ´�.
		PARTY		= 1,		// ��Ƽ������ ����
		GUILD		= 2,		// �������� ����
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

		unsigned long	dwUID;				// ����ũ ID (������ ������ ���� ���ʴ��...)	
		unsigned short	wMapIndex;			// ���� ���� ��� �� �ε���
		unsigned char   cCellX;				// �� �ε��� X
		unsigned char   cCellZ;				// �� �ε��� Z
	};

    struct ItemInfo
	{
        Position		m_Pos;					// ����� ���� ��ġ

		union
		{
			unsigned __int64	m_nUniqueID;	// ����ũ ID
			IDField				m_Field;

		} UID;

        Item::CItem*    m_lpItem;			    // ������
		unsigned long	m_dwGold;				// ��

		unsigned long	m_dwOwnerID;			// ������		
		unsigned short	m_wPulse;				// �Ҹ���� ���� �޽�	

		unsigned char	m_cAutoRouting;			// ������õ� �������ΰ�? (ItemAutoRouting enum ����)

		ItemInfo();
        void MakeFieldObject(FieldObject& fieldObject);
	};

    typedef std::list<CMonster*,        boost::fast_pool_allocator<CMonster*> >			MonsterList;
    typedef std::list<CCharacter*,      boost::fast_pool_allocator<CCharacter*> >		CharacterList;
    typedef std::list<ItemInfo,         boost::fast_pool_allocator<ItemInfo> >			ItemList;
    typedef std::list<CSiegeObject*,    boost::fast_pool_allocator<CSiegeObject*> >	    SiegeObjectList;


    //--------------------------------------------------------------------------------------------
    // �ʱ�ȭ

	CCell();
	~CCell();

    
    void Initialize(unsigned char cIndexX, unsigned char cIndexZ); // �� ��ǥ�� �ʱ�ȭ�ϰ�, ��Ʈ�� ��ũ�� �����Ѵ�.
	void SetMapIndex(unsigned short wMapIndex)					  { m_wMapIndex = wMapIndex; }
	unsigned short GetMapIndex()								  { return m_wMapIndex;		 }
	

    //--------------------------------------------------------------------------------------------
    // interface		

	CCell* GetConnectCell(unsigned int Dir);
	void SetConnectCell(unsigned int Dir, CCell* lpConnectedCell);
	bool IsNearCell(CCell* lpNearCell);


    //--------------------------------------------------------------------------------------------
	// �� ����

	unsigned char	GetIndexX() const	{ return m_cIndexX;					}
	unsigned char	GetIndexZ() const	{ return m_cIndexZ;					}
	size_t  GetMonsterNum()     const	{ return m_lstMonster.size();		}
	size_t  GetCharacterNum()   const	{ return m_lstCharacter.size();		}
	size_t  GetItemNum()        const	{ return m_lstItem.size();			}
	size_t	GetSiegeObjectNum() const	{ return m_lstSiegeObject.size();	}
    size_t  GetNearCellCharacterNum() const;


    //--------------------------------------------------------------------------------------------
	// ���� ũ���� �߰� �� ����, ��� ����.

	void SetCreature(unsigned long dwCID, CCreature* lpCreature, CCell* lpLastLogoutCell = 0);
	void DeleteCreature(unsigned long dwCID, CCell* lpPrepareLoginCell = 0); 
	CCreature* GetCreature(unsigned long dwCID);
	
	CAggresiveCreature* GetFirstAggresiveCreature();
	CAggresiveCreature* GetNextAggresiveCreature();

	void KillAll(CCharacter* lpAttacker);


	//--------------------------------------------------------------------------------------------
	// ���� ���� ó��

	bool IsMonster() const { return !(m_lstMonster.empty()); }
	CMonster* GetFirstMonster();
    CMonster* GetNextMonster() { return (++m_MonsterIt != m_lstMonster.end()) ? *m_MonsterIt : NULL; }

	void LogoutAllMonster() { m_lstMonster.clear(); }


	//--------------------------------------------------------------------------------------------
	// ���� ������Ʈ ���� ó��

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
	// ĳ���� ���� ó��

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
	// ������ ���� ó��

	bool IsItem(void) { return !(m_lstItem.empty()); }

	void SetItem(const Position& Pos, Item::CItem* lpItem, unsigned long dwGold, 
		unsigned long dwOwnerID, unsigned char cAutoRouting, CCell::ItemInfo& cellItemInfo);

	CCell::ErrorCode GetItem(unsigned long dwCreatureID, unsigned __int64 nItemInfoID, 
        Item::CItem** lppItem, unsigned long& dwMoney_Out);

	void CheckDeleteItem(void);
	void DeleteAllItem(void);


    //--------------------------------------------------------------------------------------------
	// �� ���� ��ε� ĳ����

    void PrepareBroadCast();
	void BroadCast(unsigned long dwCurrentPulse);

    // ���� ��Ŷ��, Wrap���� ���� ������(���ο��� Wrap��. srvState�� usError�� ������ ����.)
    // ��Ŷ ������ �ٲ� �� �����Ƿ� �����Ѵ�.

	void SendAllNearCellCharacter(const PktBase* lpPktBase, unsigned short usLength, unsigned char cCMD);
	void SendAllCharacter(const PktBase* lpPktBase, unsigned short usLength, unsigned char cCMD);

    void SendNowAllNearCellCharacter(const char* szData, unsigned short usLength, unsigned char cCMD);
    void SendNowAllCharacter(const char* szData, unsigned short usLength, unsigned char cCMD);

	void SendAttackInfo(unsigned long AttackerID_In, const AtType &AtType_In, 
        unsigned char DefenserNum_In, DefenserNode* lpNode_In);

	void SendCastObjectInfo(unsigned long SenderID, unsigned long ReceiverID, CastObject& CastObject_In);

private:

    //--------------------------------------------------------------------------------------------
    // ��Ŷ ���� ���� (���ο����� ȣ���ϴ� �Լ���)

    void SendPullDownInfo(unsigned long dwOwnerID, ItemInfo& itemInfo);
    void SendPickUpInfo(unsigned long dwCreatureID, unsigned __int64 nItemInfoID);
        
    void SendCellLogin(CAggresiveCreature* lpAggresiveCreature, CCell* lpLastLogoutCell);
    void SendCellLogout(CAggresiveCreature* lpAggresiveCreature, CCell* lpPrepareLoginCell);

    // ���۸� ���� Release�Ѵ�.
    static void ReleaseAllBuffer(CBuffer*& lpBuffer);

    //--------------------------------------------------------------------------------------------
    // ������

    CCell*			m_lpConnectCell[CONNECT_NUM];           // �ֺ� ���� ����Ʈ. �ڱ� �ڽŵ� �����Ѵ�.
    CCell*          m_lpSortedConnectedCell[CONNECT_NUM];   // �ֺ� ���� ����Ʈ�� �����ͼ����� ������ �迭.

    CharacterList	m_lstCharacter;
    MonsterList		m_lstMonster;
    ItemList		m_lstItem;
    SiegeObjectList	m_lstSiegeObject;               // ���� ����Ʈ�� ������ ���� ��, ��� ���� ������Ʈ�� CSiegeObjectMgr �� �����Ѵ�.
    
    MonsterList::iterator		m_MonsterIt;        // ������... �� �༮�� �� ���� ���� �ٸ� �༮�� �޶�� �ϸ� ����մϴ�.
    CharacterList::iterator		m_CharacterIt;      // ������... �� �༮�� �� ���� ���� �ٸ� �༮�� �޶�� �ϸ� ����մϴ�.
    SiegeObjectList::iterator	m_SiegeObjectIt;    // ������... �� �༮�� �� ���� ���� �ٸ� �༮�� �޶�� �ϸ� ����մϴ�.

    CBuffer*        m_lpBroadcast2ndBuffer;         // ��ε�ĳ��Ʈ 2nd����(�� ��ε�ĳ��Ʈ ����, ĳ���� �� �����ۿ�)

    unsigned long	m_dwCastleEmblemCID;	        // �� ��¡���� ���ؼ� ������ �ִ� ���̸� ��¡���� CID�� ������ �ִ�.

    unsigned short	m_wMapIndex;                    // �� �ε���
    unsigned char	m_cIndexX;                      // �� X��ǥ
    unsigned char	m_cIndexZ;                      // �� Z��ǥ
    
    unsigned char	m_cTurnOfGetAggresiveCreature;  

	CCell::Weather	m_eWeather;						// ���� ����
};

#endif