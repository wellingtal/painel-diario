///////////////////////////////////////////////////////////////////////////////////
//
// Purpose : Cell을 관리하는 클래스
//
///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cell.h"

#include <Item/Item.h>
#include <Item/ItemFactory.h>
#include <Item/ItemMgr.h>

#include <Creature/Creature.h>
#include <Creature/AggresiveCreature.h>
#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/VirtualMonsterMgr.h>
#include <Creature/CreatureManager.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeObjectMgr.h>

#include <Network/Packet/ChatPacket.h>
#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/ClientSocket/ClientConstants.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Community/Party/Party.h>
#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>


unsigned char CCell::ms_CellSize = CCell::CELL_SIZE_DEFAULT;


class CCellObjectUIDMgr
{
public:

    static CCellObjectUIDMgr& GetInstance();
    unsigned long GetNewObjectUID() { return m_dwCellObjectUID++; }

private:

    CCellObjectUIDMgr() : m_dwCellObjectUID(1) { }

    unsigned long m_dwCellObjectUID;
};

CCellObjectUIDMgr& CCellObjectUIDMgr::GetInstance()
{
    static CCellObjectUIDMgr cellObjectUIDMgr;
    return cellObjectUIDMgr;
}

class CFindCreatureFromCID : public std::unary_function<CCreature, bool>
{
public:

	explicit CFindCreatureFromCID(unsigned long dwCID) 
	:	m_dwCID(dwCID) 
	{ }		

    bool operator() (CCreature* pCreature) 
	{ 
		return (m_dwCID == pCreature->GetCID()) ? true : false; 
	}


private:

	const unsigned long m_dwCID;
};

CCell::ItemInfo::ItemInfo() 
:	m_lpItem(NULL), m_dwGold(0), m_dwOwnerID(0), m_wPulse(0), m_cAutoRouting(0)
{ 
	UID.m_nUniqueID = 0; 
}

void CCell::ItemInfo::MakeFieldObject(FieldObject& fieldObject)
{
    fieldObject.m_nOID	= UID.m_nUniqueID;

	// 소수점 한자리수까지 정보를 남기기위해서 10을 곱해준다.
	fieldObject.m_usXPos = static_cast<unsigned short>(m_Pos.m_fPointX * 10.0f);
    fieldObject.m_usYPos = static_cast<unsigned short>(m_Pos.m_fPointY * 10.0f);
    fieldObject.m_usZPos = static_cast<unsigned short>(m_Pos.m_fPointZ * 10.0f);

	if (CServerSetup::GetInstance().GetServerZone() == SERVER_ID::BATTLE_SERVER && 0 != fieldObject.m_usYPos)
	{
		// Y 좌표가 0이 아닌경우(몬스터가 떨어뜨린게 아닌경우), 배틀로한은 1m 를 높여준다.
		fieldObject.m_usYPos += 10;
	}

    if (0 != m_lpItem)
    {
        fieldObject.m_cNum       = m_lpItem->GetNumOrDurability();
        fieldObject.m_dwTypeID   = m_lpItem->GetPrototypeID();
    }
    else
    {
        fieldObject.m_cNum       = 1;
        fieldObject.m_dwTypeID   = (CCell::MONEY_BIT + m_dwGold);
    }
}


CCell::CCell()
:   m_lpBroadcast2ndBuffer(0),    
	m_wMapIndex(0),
	m_cIndexX(0), 
	m_cIndexZ(0),    
	m_cTurnOfGetAggresiveCreature(TURN_OF_CHARACTER),
	m_dwCastleEmblemCID(0),
	m_eWeather(CCell::WEATHER_FINE)
{
    m_MonsterIt     = m_lstMonster.end();
    m_CharacterIt   = m_lstCharacter.end();
    m_SiegeObjectIt = m_lstSiegeObject.end();

	std::fill_n(m_lpConnectCell,            int(CONNECT_NUM), reinterpret_cast<CCell*>(0));
    std::fill_n(m_lpSortedConnectedCell,    int(CONNECT_NUM), reinterpret_cast<CCell*>(0));
    m_lpConnectCell[NO] = this;
}


CCell::~CCell(void)
{
	for (ItemList::iterator itr = m_lstItem.begin(); itr != m_lstItem.end(); ++itr)
	{
        ItemInfo& itemInfo = *itr;

		if (NULL != itemInfo.m_lpItem)
		{
            DELETE_ITEM(itemInfo.m_lpItem);
		}
	}

    ReleaseAllBuffer(m_lpBroadcast2ndBuffer);
}



void CCell::Initialize(unsigned char cIndexX, unsigned char cIndexZ)
{
    m_cIndexX = cIndexX;
    m_cIndexZ = cIndexZ;

    std::partial_sort_copy(m_lpConnectCell, m_lpConnectCell + CCell::CONNECT_NUM,
        m_lpSortedConnectedCell, m_lpSortedConnectedCell + CCell::CONNECT_NUM);
}



CMonster* CCell::GetFirstMonster(void)
{
	if (false == m_lstMonster.empty()) 
	{
       	m_MonsterIt = m_lstMonster.begin();
    	return *m_MonsterIt;
	}

    return NULL;
}

CSiegeObject* CCell::GetFirstAirShip(void)
{
	m_SiegeObjectIt = m_lstSiegeObject.begin();

	while (m_SiegeObjectIt != m_lstSiegeObject.end())
	{
		CSiegeObject* lpSiegeObject = *m_SiegeObjectIt;
		if (lpSiegeObject && Siege::AIRSHIP == lpSiegeObject->GetObjectType())
		{
			return lpSiegeObject;
		}

		++m_SiegeObjectIt;
	}

	return NULL;
}

CSiegeObject* CCell::GetNextAirShip(void)
{
	while (++m_SiegeObjectIt != m_lstSiegeObject.end())
	{
		CSiegeObject* lpSiegeObject = *m_SiegeObjectIt;
		if (lpSiegeObject && Siege::AIRSHIP == lpSiegeObject->GetObjectType())
		{
			return lpSiegeObject;
		}
	}

	return NULL;
}

CSiegeObject* CCell::GetFirstSiegeObject()
{
	m_SiegeObjectIt = m_lstSiegeObject.begin();

	while (m_SiegeObjectIt != m_lstSiegeObject.end())
	{
		CSiegeObject* lpSiegeObject = *m_SiegeObjectIt;
		if (lpSiegeObject)
		{
			return lpSiegeObject;
		}

		++m_SiegeObjectIt;
	}

	return NULL;
}

CSiegeObject* CCell::GetNextSiegeObject()
{
	while (++m_SiegeObjectIt != m_lstSiegeObject.end())
	{
		CSiegeObject* lpSiegeObject = *m_SiegeObjectIt;
		if (lpSiegeObject)
		{
			return lpSiegeObject;
		}
	}

	return NULL;
}


CCharacter* CCell::GetFirstCharacter(void)
{
	if (false == m_lstCharacter.empty())
	{
	    m_CharacterIt = m_lstCharacter.begin();
	    return *m_CharacterIt;
	}

    return 0;
}


CAggresiveCreature* CCell::GetFirstAggresiveCreature(void)
{
	m_cTurnOfGetAggresiveCreature = TURN_OF_CHARACTER;

	CAggresiveCreature* pCreature = reinterpret_cast<CAggresiveCreature *>(GetFirstCharacter());
    if (NULL == pCreature)
	{
		m_cTurnOfGetAggresiveCreature = TURN_OF_SIEGE_OBJECT;
		
		pCreature = reinterpret_cast<CAggresiveCreature*>(GetFirstSiegeObject());
		if (NULL == pCreature)
		{
			m_cTurnOfGetAggresiveCreature = TURN_OF_MONSTER;
			return reinterpret_cast<CAggresiveCreature *>(GetFirstMonster());
		}
	}

	return pCreature;
}


CAggresiveCreature* CCell::GetNextAggresiveCreature(void)
{
	CAggresiveCreature* pCreature = NULL;

	switch (m_cTurnOfGetAggresiveCreature)
	{
	case TURN_OF_CHARACTER:
		{
			pCreature = reinterpret_cast<CAggresiveCreature *>(GetNextCharacter());
			if (NULL == pCreature)
			{
				m_cTurnOfGetAggresiveCreature = TURN_OF_SIEGE_OBJECT;

				pCreature = reinterpret_cast<CAggresiveCreature *>(GetFirstSiegeObject());
				if (NULL == pCreature)
				{
					m_cTurnOfGetAggresiveCreature = TURN_OF_MONSTER;
					pCreature = reinterpret_cast<CAggresiveCreature *>(GetFirstMonster());
				}
			}

			return pCreature;
		}
		break;

	case TURN_OF_SIEGE_OBJECT:
		{
			pCreature = reinterpret_cast<CAggresiveCreature *>(GetNextSiegeObject());
			if (NULL == pCreature)
			{
				m_cTurnOfGetAggresiveCreature = TURN_OF_MONSTER;
				pCreature = reinterpret_cast<CAggresiveCreature *>(GetFirstMonster());
			}

			return pCreature;
		}
		break;

	case TURN_OF_MONSTER:
		{
			pCreature = reinterpret_cast<CAggresiveCreature *>(GetNextMonster());
			return pCreature;
		}
		break;
	}

	return NULL;
}


void CCell::KillAll(CCharacter* lpAttacker)
{
	if (true == m_lstMonster.empty()) 
	{ 
		return; 
	}

	const int MAX_BUFFER = sizeof(PktAtAck) + AtNode::MAX_DEFENDER_NUM * sizeof(DefenserNode);
	char szBuffer[MAX_BUFFER];

	PktAtAck* lpPktAtAck = reinterpret_cast<PktAtAck*>(szBuffer);
	DefenserNode* lpDefenserNode = reinterpret_cast<DefenserNode*>(lpPktAtAck + 1); 		
	CMonster* lpDeadMonster[AtNode::MAX_DEFENDER_NUM] = { NULL, };

	MonsterList::iterator it = m_lstMonster.begin();
	unsigned char cDefenderNum = 0;

	for (; it != m_lstMonster.end() && cDefenderNum < AtNode::MAX_DEFENDER_NUM; ++it, ++cDefenderNum)
	{
		lpDeadMonster[cDefenderNum] = *it;

		unsigned short usNowHP = lpDeadMonster[cDefenderNum]->GetStatus().m_nNowHP;
		lpDeadMonster[cDefenderNum]->GetStatus().m_nNowHP = 0;

		lpDefenserNode[cDefenderNum].m_wDamage = usNowHP;
		lpDefenserNode[cDefenderNum].m_dwCharID = lpDeadMonster[cDefenderNum]->GetCID();
		lpDefenserNode[cDefenderNum].m_sCurrHP = lpDeadMonster[cDefenderNum]->GetStatus().m_nNowHP;
		lpDefenserNode[cDefenderNum].m_sCurrMP = lpDeadMonster[cDefenderNum]->GetStatus().m_nNowMP;
		lpDefenserNode[cDefenderNum].m_wMaxHP = lpDeadMonster[cDefenderNum]->GetStatus().m_StatusInfo.m_nMaxHP;
		lpDefenserNode[cDefenderNum].m_wMaxMP = lpDeadMonster[cDefenderNum]->GetStatus().m_StatusInfo.m_nMaxMP;
		lpDefenserNode[cDefenderNum].m_wMPHeal = 0;
		lpDefenserNode[cDefenderNum].m_cJudge = ClientConstants::Judge_Front;
	}

	for (int nIndex = 0; nIndex < cDefenderNum; ++nIndex)
	{
		if (NULL == lpDeadMonster[nIndex]) { break; }

		lpDeadMonster[nIndex]->GetThreat().AddToThreatList(lpAttacker, 1L);
		lpDeadMonster[nIndex]->Dead(lpAttacker);
		lpDeadMonster[nIndex]->GetThreat().ClearAll();		
	}

	lpPktAtAck->m_dwCharID = lpAttacker->GetCID();

	AtType attackType;
	attackType.m_wType = AtType::RIGHT_MELEE;
	lpPktAtAck->m_AtType = attackType;

	lpPktAtAck->m_wHP = lpAttacker->GetStatus().m_nNowHP;
	lpPktAtAck->m_wMP = lpAttacker->GetStatus().m_nNowMP;
	lpPktAtAck->m_wMPHeal = 0;

	lpPktAtAck->m_cJudge = ClientConstants::Judge_Front;
	lpPktAtAck->m_cDefenserNum = cDefenderNum;

    CCell* lpCell = lpAttacker->GetCellPos().m_lpCell;

	CGameClientDispatch* lpDispatch = lpAttacker->GetDispatcher();
	if (0 != lpDispatch && 
        lpDispatch->GetSendStream().WrapCompress(szBuffer, 
        sizeof(PktAtAck) + cDefenderNum * sizeof(DefenserNode), CmdCharAttack, 0, 0) &&
        0 != lpCell)
	{
		lpCell->SendAttackInfo(lpAttacker->GetCID(), 
            attackType, cDefenderNum, lpDefenserNode);
	}
}

void	CCell::UpgradeByEmblem(unsigned long dwCID)
{
	m_dwCastleEmblemCID = dwCID;

	CSiegeObject* lpEmblem = CSiegeObjectMgr::GetInstance().GetSiegeObject(m_dwCastleEmblemCID);
	if (lpEmblem)
	{
		CCharacter* lpCharacter = GetFirstCharacter();
		while (lpCharacter)
		{
			if (EnemyCheck::EC_ENEMY == lpEmblem->IsEnemy(lpCharacter))
			{
				// 해당 캐릭터에게 스텔스를 사용할수 없다는 정보를 보낸다.
                SendStealthInfo(*lpCharacter, false);

				if (true == lpCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::Stealth))
				{
					DetectionAttack(lpEmblem, lpCharacter);
				}
			}

			lpCharacter = GetNextCharacter();
		}
	}
}

void	CCell::DegradeByEmblem()
{
	CSiegeObject* lpEmblem = CSiegeObjectMgr::GetInstance().GetSiegeObject(m_dwCastleEmblemCID);
	if (lpEmblem)
	{
		CCharacter* lpCharacter = GetFirstCharacter();
		while (lpCharacter)
		{
			if (EnemyCheck::EC_ENEMY == lpEmblem->IsEnemy(lpCharacter))
			{
				// 해당 캐릭터에게 스텔스를 사용 가능 정보를 보낸다.
				SendStealthInfo(*lpCharacter, true);
			}

			lpCharacter = GetNextCharacter();
		}
	}

	m_dwCastleEmblemCID = 0;
}

void	CCell::DetectionAttack(CSiegeObject* lpEmblem, CAggresiveCreature* lpTargetCreature)
{
	if (NULL == lpEmblem || NULL == lpTargetCreature) return;

	AtType attackType;
	attackType.m_cSkillLockCount = 0;
	attackType.m_cSkillLevel = 1;
	attackType.m_cAtCount = 0;
	attackType.m_cCasting = 0;
	attackType.m_wType = 0x8201;	// Detection

	CAggresiveCreature*	lpAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };
	unsigned char	cDefenserJudge[AtNode::MAX_DEFENDER_NUM] = {0, };
	unsigned short	wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM] = {0, };
	lpAggresiveCreature[0] = lpTargetCreature;

	lpEmblem->SetFullMP();
	lpEmblem->Attack(attackType, 1, lpAggresiveCreature, cDefenserJudge, wDefenserMPHeal);
}

bool	CCell::SendStealthInfo(CCharacter& character, bool bUseStealth)
{
    CGameClientDispatch* lpDispatch = 0;

	if (0 != (lpDispatch = character.GetDispatcher()) &&
        GameClientSendPacket::SendStealthInfo(lpDispatch->GetSendStream(), bUseStealth))
	{
		return true;
	}

	ERRLOG1(g_Log, "CID:0x%08x 캐릭터에게 스텔스 사용 유무 정보 전송 실패", character.GetCID());
	return false;
}

///////////////////////////////////////////////////////////////////////////////////
// Function : CCell::SetCreature
//
// Description : Creature(몬스터나 유저)를 Cell에 넣음
//
// Inputs : dwCID - Creature의 ID
//			pObject - Creature의 포인터
//			eCellMoveType - 이동 타입(CCell::SendCellLogin 참고)
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CCell::SetCreature(unsigned long dwCID, CCreature* lpCreature, CCell* lpLastLogoutCell)
{
    if (0 == lpCreature)
    {
        ERRLOG1(g_Log, "Creature 포인터가 NULL입니다. CID: 0x%08x", dwCID);
        return;
    }

	if (NULL != GetCreature(dwCID)) 
	{
		ERRLOG1(g_Log, "이 셀에는 이미 이 크리쳐가 존재합니다. CID: 0x%08x", dwCID);
		return;
	}

	CAggresiveCreature* lpAggresiveCreature = 
        static_cast<CAggresiveCreature*>(lpCreature);
    
	switch (Creature::GetCreatureType(dwCID))
	{
	case Creature::CT_PC:
		{
			CCharacter* lpCharacter = static_cast<CCharacter*>(lpCreature);
            m_lstCharacter.push_back(lpCharacter);

			// 성 소유 상징물의 디텍션 영역안에 있는 셀이라면
			if (0 != m_dwCastleEmblemCID)
			{
				CSiegeObject* lpEmblem = CSiegeObjectMgr::GetInstance().GetSiegeObject(m_dwCastleEmblemCID);
				if (lpEmblem && EnemyCheck::EC_ENEMY == lpEmblem->IsEnemy(lpCharacter))
				{
					// 해당 캐릭터에게 스텔스를 사용하지 못한다는 정보를 보낸다.
					SendStealthInfo(*lpCharacter, false);

					if (lpCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::Stealth))
					{
						DetectionAttack(lpEmblem, lpCharacter);
					}
				}
			}
			else
			{
				// 해당 캐릭터에게 스텔스 사용 가능 정보를 보낸다.
				SendStealthInfo(*lpCharacter, true);
			}
		}
		break;

	case Creature::CT_MONSTER:
	case Creature::CT_SUMMON:
	case Creature::CT_STRUCT:
		m_lstMonster.push_back(static_cast<CMonster*>(lpCreature));
		break;

	case Creature::CT_SIEGE_OBJECT:
		m_lstSiegeObject.push_back(static_cast<CSiegeObject*>(lpCreature));
		break;
	}

	SendCellLogin(lpAggresiveCreature, lpLastLogoutCell);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCell::GetCreature
//
// Description : 셀에 속한 Creature의 포인터를 얻음
//
// Inputs : dwCID - 얻고자하는 녀석의 CID
//
// Outputs : None.
//
// Returns : LPCCreature - 얻는 녀석의 포인터
///////////////////////////////////////////////////////////////////////////////////
CCreature* CCell::GetCreature(unsigned long dwCID)
{
	CFindCreatureFromCID findCID(dwCID);

	switch (Creature::GetCreatureType(dwCID))
	{
	case Creature::CT_PC:
		{
			CharacterList::iterator itr = std::find_if(m_lstCharacter.begin(), m_lstCharacter.end(), findCID);
			if (itr != m_lstCharacter.end())
			{
				return (CCreature *)*itr;
			}
		}
		break;

	case Creature::CT_MONSTER:
	case Creature::CT_SUMMON:
	case Creature::CT_STRUCT:
		{
			MonsterList::iterator itr = std::find_if(m_lstMonster.begin(), m_lstMonster.end(), findCID);
			if (itr != m_lstMonster.end())
			{
				return (CCreature *)*itr;
			}
		}
		break;

	case Creature::CT_SIEGE_OBJECT:
		{
			SiegeObjectList::iterator itr = std::find_if(m_lstSiegeObject.begin(), m_lstSiegeObject.end(), findCID);
			if (itr != m_lstSiegeObject.end())
			{
				return (CCreature *)*itr;
			}
		}
		break;
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCell::DeleteCreature
//
// Description : 셀에서 원하는 Creature을 지움
//
// Inputs : dwCID - 지우고자 하는 Creature의 CID
//			eCellMoveType - 이동 타입(CCell::SendCellLogout 참고)
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CCell::DeleteCreature(unsigned long dwCID, CCell* lpPrepareLoginCell)
{
	CFindCreatureFromCID findCID(dwCID);

	switch (Creature::GetCreatureType(dwCID))
	{
	case Creature::CT_PC:
		{
			CharacterList::iterator itr = 
                std::find_if(m_lstCharacter.begin(), m_lstCharacter.end(), findCID);

			if (itr != m_lstCharacter.end()) 
			{
				SendCellLogout(*itr, lpPrepareLoginCell);;
				m_lstCharacter.erase(itr);
			} 
			else 
			{
				ERRLOG1(g_Log, "이 셀에는 이 캐릭터가 존재하지 않습니다. CID: 0x%08x", dwCID);
			}
		}
		break;

	case Creature::CT_MONSTER:
	case Creature::CT_SUMMON:
	case Creature::CT_STRUCT:
		{
			MonsterList::iterator itr = 
                std::find_if(m_lstMonster.begin(), m_lstMonster.end(), findCID);

			if (itr != m_lstMonster.end()) 
			{
				SendCellLogout(*itr, lpPrepareLoginCell);;
				m_lstMonster.erase(itr);
			}
			else 
			{
				ERRLOG1(g_Log, "이 셀에는 이 몬스터가 존재하지 않습니다. CID: 0x%08x", dwCID);
			}
		}
		break;

	case Creature::CT_SIEGE_OBJECT:
		{
			SiegeObjectList::iterator itr = 
                std::find_if(m_lstSiegeObject.begin(), m_lstSiegeObject.end(), findCID);

			if (itr != m_lstSiegeObject.end()) 
			{
				m_lstSiegeObject.erase(itr);
			}
			else 
			{
				ERRLOG1(g_Log, "이 셀에는 이 해당 공성관련 오브젝트가 존재하지 않습니다. CID: 0x%08x", dwCID);
			}
		}
		break;
	}	
}


void CCell::SetItem(const Position& Pos, Item::CItem* lpItem, unsigned long dwGold, 
					unsigned long dwOwnerID, unsigned char cAutoRouting, CCell::ItemInfo& cellItemInfo)
{
	cellItemInfo.m_Pos					= Pos;

    cellItemInfo.UID.m_Field.dwUID     	= CCellObjectUIDMgr::GetInstance().GetNewObjectUID();
    cellItemInfo.UID.m_Field.wMapIndex 	= m_wMapIndex;
    cellItemInfo.UID.m_Field.cCellX    	= m_cIndexX;
    cellItemInfo.UID.m_Field.cCellZ    	= m_cIndexZ;

	cellItemInfo.m_lpItem				= lpItem;
	cellItemInfo.m_dwGold				= dwGold;
        
	cellItemInfo.m_dwOwnerID			= dwOwnerID;
	cellItemInfo.m_cAutoRouting			= cAutoRouting;

	cellItemInfo.m_wPulse = (NONE == cAutoRouting) ? ITEM_RIGHT_TIME : ITEM_LIFE_TIME;

    m_lstItem.push_front(cellItemInfo);

    SendPullDownInfo(dwOwnerID, cellItemInfo);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCell::GetItem
//
// Description : 셀에 떨어져 있는 아이템을 얻음
//
// Inputs : dwCreatureID - 아이템을 얻으려는 크리쳐의 ID
//			nItemInfoID - 아이템의 유니크 ID
//
// Outputs : lppItem - 아이템의 포인터. 없는 아이템이거나 소유권이 없는 크리쳐가 요청하면 NULL.
//			 dwMoney_Out - 금액
//
// Returns : CCell::ErrorCode - 에러 코드
///////////////////////////////////////////////////////////////////////////////////
CCell::ErrorCode CCell::GetItem(unsigned long dwCreatureID, unsigned __int64 nItemInfoID, 
                                Item::CItem** lppItem, unsigned long& dwMoney_Out)
{
    *lppItem = NULL;
    dwMoney_Out = 0;

	CAggresiveCreature* lpPickkingCreature = CCreatureManager::GetInstance().GetAggresiveCreature(dwCreatureID);
	if (NULL == lpPickkingCreature)
	{
		return E_NOT_OWNER_OF_ITEM;
	}

	if (0 == m_lstItem.size()) 
	{ 
		return E_NOT_ITEM;
	}

	ItemList::iterator pos = m_lstItem.begin();
	ItemList::iterator end = m_lstItem.end();

	for (; pos != end; ++pos)
	{
		if (pos->UID.m_nUniqueID == nItemInfoID)
		{
			break;
		}
	}

	if (pos == end) 
	{ 
		return E_NOT_ITEM;  
	}

	ItemInfo& itemInfo = *pos;

	// 소유권 체크
	if (itemInfo.m_dwOwnerID != 0)
	{
		if (GUILD == itemInfo.m_cAutoRouting)
		{
			Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(itemInfo.m_dwOwnerID);
			if (!lpGuild)
			{
				return E_NOT_EXIST_GUILD;
			}

			if (lpPickkingCreature->GetGID() != lpGuild->GetGID())
			{
				return E_NOT_OWNER_OF_ITEM;
			}
		}
		else if (dwCreatureID != itemInfo.m_dwOwnerID)
		{
			// 유저가 떨어뜨렸거나 해서 소유권이 남에게 있는 경우
			if (NONE == itemInfo.m_cAutoRouting)
			{
				return E_NOT_OWNER_OF_ITEM;
			}

			// 몬스터가 떨어뜨린 아이템 (오토루팅 전)
			CAggresiveCreature* lpOwnerCreature = 
				CCreatureManager::GetInstance().GetAggresiveCreature(itemInfo.m_dwOwnerID);

			if (NULL != lpOwnerCreature && NULL != lpPickkingCreature) 
			{
				CParty* lpOwnerParty = lpOwnerCreature->GetParty();
	
				if (NULL == lpOwnerParty || lpOwnerParty != lpPickkingCreature->GetParty())
				{ 
					return E_NOT_OWNER_OF_ITEM;
				}
			}
		}
	}

	Item::CItem* lpItem = NULL;         

	if (0 != itemInfo.m_dwGold)
	{
		dwMoney_Out = itemInfo.m_dwGold;

		if (Creature::CT_PC == Creature::GetCreatureType(dwCreatureID) &&
			reinterpret_cast<CCharacter*>(lpPickkingCreature)->GetGold() > ULONG_MAX - dwMoney_Out)
		{
			return E_GOLD_OVERFLOW;
		}
	}
	else
	{
		lpItem = itemInfo.m_lpItem; 
		if(lpItem == NULL)
		{
			return E_NOT_ITEM; 			            
		}

		if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
		{
			return E_NOT_ITEM; 			            
		}
	}

	ErrorCode eErrorCode = S_SUCCESS;

	if (PARTY == itemInfo.m_cAutoRouting)
	{
		// 오토 루팅
		CParty* lpParty = lpPickkingCreature->GetParty();
		if (NULL != lpParty)
		{
			if (true == lpParty->AutoRouting(lpPickkingCreature, lpItem, dwMoney_Out))
			{
				eErrorCode = S_AUTO_ROUTING;
			}					
		}
	}
	else if (GUILD == itemInfo.m_cAutoRouting)
	{
		// 오토 루팅
		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(itemInfo.m_dwOwnerID);
		if (NULL != lpGuild)
		{
			if (true == lpGuild->AutoRouting(lpPickkingCreature, lpItem, dwMoney_Out))
			{
				eErrorCode = S_AUTO_ROUTING;
			}
		}
	}

	// TODO : 추후에 쓰이게 되면 수정이 필요합니다. (오토루팅까지 고려해서...)
/*
	if (NULL != lpItem && Creature::CT_PC == Creature::GetCreatureType(lpPickkingCreature->GetCID()))
	{
		if (Item::ItemType::EVENT_DROP == lpItem->GetItemInfo().m_DetailData.m_cItemType)
		{							
			char szChatMessage[PktChat::PktChatMaxSize];

			CCharacter* lpCharacter = static_cast<CCharacter*>(lpPickkingCreature);
			int nSize = _snprintf(szChatMessage, PktChat::PktChatMaxSize - 1, 
				"[알림] 축하합니다~! %s님께서 %s을(를) 얻으셨습니다~!", 
				lpCharacter->GetCharacterName(), lpItem->GetItemInfo().m_SpriteData.m_szName);

			szChatMessage[PktChat::PktChatMaxSize - 1] = 0;

			CChatPacket chatPacket(szChatMessage, 0, PktChat::NOTICE);

			if (chatPacket.IsValid())
			{
				CCreatureManager::GetInstance().SendAllCharacter(
					chatPacket.GetCompressedPacket(), chatPacket.GetCompressedSize(), CmdCharChat);
			}

			DETLOG3(g_Log, "CID:0x%08x 이벤트 로그 : %s(이)가 %s을(를) 얻었습니다.", 
				lpCharacter->GetCID(), lpCharacter->GetCharacterName(), lpItem->GetItemInfo().m_SpriteData.m_szName);
		}
	}
*/

	*lppItem = lpItem;
	m_lstItem.erase(pos);

	SendPickUpInfo(dwCreatureID, nItemInfoID);

	return eErrorCode;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCell::CheckDeleteItem
//
// Description : 맵 위에서의 아이템 지속 시간을 체크한다.
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CCell::CheckDeleteItem(void)
{
	if (true == m_lstItem.empty()) 
	{ 
		return; 
	}

	for (ItemList::iterator itr = m_lstItem.begin(); itr != m_lstItem.end();) 
	{
		ItemInfo& itemInfo = *itr;

		itemInfo.m_wPulse -= CHECK_TIME;
		if (itemInfo.m_wPulse <= NO_OWNER_TIME)
		{
			itemInfo.m_dwOwnerID = 0;
		}

		if (itemInfo.m_wPulse <= 0)
		{
			// '땅이 아이템이 줍는다'라는 생각...
			SendPickUpInfo(0, itemInfo.UID.m_nUniqueID);

            if (itemInfo.m_lpItem != NULL) 
			{
				DELETE_ITEM(itemInfo.m_lpItem);				
			}

			m_lstItem.erase(itr++);
			continue;
		}
		
		++itr;
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCell::DeleteAllItem
//
// Description : 맵 위의 아이템을 모두 지운다.
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CCell::DeleteAllItem(void)
{
	if (true == m_lstItem.empty()) 
	{ 
		return; 
	}

	for (ItemList::iterator itr = m_lstItem.begin(); itr != m_lstItem.end();) 
	{
		ItemInfo& itemInfo = *itr;

		itemInfo.m_dwOwnerID = 0;

		// '땅이 아이템이 줍는다'라는 생각...
		SendPickUpInfo(0, itemInfo.UID.m_nUniqueID);

		if (itemInfo.m_lpItem != NULL) 
		{
			DELETE_ITEM(itemInfo.m_lpItem);			
		}

		m_lstItem.erase(itr++);
	}
}


void CCell::SendPullDownInfo(unsigned long dwOwnerID, ItemInfo& itemInfo)
{
    PktPDInfo pktPDInfo;
    memset(&pktPDInfo, 0, sizeof(PktPDInfo));

    pktPDInfo.m_dwCharID = dwOwnerID;
    itemInfo.MakeFieldObject(pktPDInfo.m_FieldObject);

    // 내부에서 Wrap을 알아서 함.
    SendAllNearCellCharacter(&pktPDInfo, sizeof(PktPDInfo), CmdCharPullDownInfo);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CCell::SendPickUpInfo
//
// Description : 아이템을 누군가 주웠다는 정보를 주위 캐릭터들에게 보냄
//
// Inputs : dwCreatureID - 주운 녀석의 아이디
//			nItemInfoID - 없어진 아이템의 아이디
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CCell::SendPickUpInfo(unsigned long dwCreatureID, unsigned __int64 nItemInfoID)
{
    PktPUInfo pktPUInfo;
    memset(&pktPUInfo, 0, sizeof(PktPUInfo));

    pktPUInfo.m_dwCharID  = dwCreatureID;
    pktPUInfo.m_nObjectID = nItemInfoID;

    // 내부에서 Wrap을 알아서 함.
    SendAllNearCellCharacter(&pktPUInfo, sizeof(PktPUInfo), CmdCharPickUpInfo);
}

//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// 공격 정보
//
// Parameter :
//	1st : 
//	2st : 
//
// Do :
//	공격한 경우 주변 셀의 캐릭터들에게 AttackInfo 를 보낸다. (UDP -> TCP : 2004-04-20)
//		
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
void CCell::SendAttackInfo(unsigned long AttackerID_In, const AtType &AtType_In, 
                           unsigned char DefenserNum_In, DefenserNode* lpNode_In)
{
	const int MAX_BUFFER = sizeof(PktAtAck) + AtNode::MAX_DEFENDER_NUM * sizeof(DefenserNode);
	char szBuffer[MAX_BUFFER];
    memset(szBuffer, 0, MAX_BUFFER);

    // 방어자 개수 재조정(최대 개수 넘을 수 없게)
    DefenserNum_In = std::min(DefenserNum_In, unsigned char(AtNode::MAX_DEFENDER_NUM));

	unsigned short wPacketSize = 
        sizeof(PktAtInfo) + sizeof(DefenserNode) * DefenserNum_In;

	PktAtInfo*		lpPktAtInfo		= reinterpret_cast<PktAtInfo*>(szBuffer);
	DefenserNode*	lpDefenserNode	= reinterpret_cast<DefenserNode*>(lpPktAtInfo + 1);

    lpPktAtInfo->m_dwCharID			= AttackerID_In;
	lpPktAtInfo->m_AtType			= AtType_In;
	lpPktAtInfo->m_cDefenserNum		= DefenserNum_In;

	CopyMemory(lpDefenserNode, lpNode_In, sizeof(DefenserNode) * DefenserNum_In);

    // 내부에서 Wrap을 알아서 한다.
	SendAllNearCellCharacter(lpPktAtInfo, wPacketSize, CmdCharAttackInfo);
}

// MON_TODO : by Vincent - 2004 : 3 : 3
//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// 몬스터 스킬 캐스트 정보
//
// Parameter :
//	1st : 
//	2st : 
//
// Do :
//	몬스터가 스킬을 사용한 경우 주변 셀의 캐릭터들에게 CastObjectInfo 를 보낸다.
//		
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
void CCell::SendCastObjectInfo(unsigned long SenderID, unsigned long ReceiverID, CastObject& CastObject_In)
{
	PktCOInfo pktCOInfo;
    memset(&pktCOInfo, 0, sizeof(PktCOInfo));

	pktCOInfo.m_dwSenderID		= SenderID;
	pktCOInfo.m_dwReceiverID	= ReceiverID;
	pktCOInfo.m_sCastObject		= CastObject_In;

	SendAllNearCellCharacter(&pktCOInfo, sizeof(PktCOInfo), CmdCharCastObjectInfo);
}

//------------------------------------------------------------------------------------------------

void CCell::SendAllNearCellCharacter(const PktBase* lpPktBase, unsigned short usLength, unsigned char cCMD)
{
	__declspec(thread) static char szPacket[PktMaxLen];

	unsigned long dwPacket = PktMaxLen;
    
	if (PacketWrap::WrapCompress(szPacket, dwPacket, 
		reinterpret_cast<const char*>(lpPktBase), usLength, cCMD, 0, 0))
	{
        CCell** lppConnectCellBound = m_lpConnectCell + CCell::CONNECT_NUM;
        for (CCell** lppCell = m_lpConnectCell; 
            lppCell != lppConnectCellBound; ++lppCell)
        {
            CCell* lpCell = *lppCell;

            if (NULL != lpCell)
            {
				lpCell->SendNowAllCharacter(
					reinterpret_cast<const char*>(szPacket), 
					static_cast<unsigned short>(dwPacket), cCMD);
            }
        }
    }
}

void CCell::SendAllCharacter(const PktBase* lpPktBase, unsigned short usLength, unsigned char cCMD)
{
	__declspec(thread) static char szPacket[PktMaxLen];

	unsigned long dwPacket = PktMaxLen;

	if (PacketWrap::WrapCompress(szPacket, dwPacket, 
		reinterpret_cast<const char*>(lpPktBase), usLength, cCMD, 0, 0))
	{
		SendNowAllCharacter(
			reinterpret_cast<const char*>(szPacket), 
			static_cast<unsigned short>(dwPacket), cCMD);
	}


	/*
    if (sizeof(PktBase) <= usLength)
    {
        if (0 == m_lpSendAllBuffer ||         
            m_lpSendAllBuffer->remain() < usLength)
        {
            // 버퍼가 없거나, 버퍼 길이가 부족하거나, 패킷 카운트가 많이 쌓였으면 패킷을 전송한다.
            FlushSendAllBuffer();

            // 버퍼를 새로 생성한다.
            m_lpSendAllBuffer = CREATE_BUFFER(
                CCellManager::GetInstance().GetBufferFactory(), PktMaxLen - 1024);

            if (0 != m_lpSendAllBuffer)
            {
                // 버퍼 생성후, 헤더 부분을 비워둔다.
                m_lpSendAllBuffer->wr_ptr(sizeof(PktBase));
            }
        }
        
        if (0 != m_lpSendAllBuffer)
        {
            // 압축 헤더를 붙인다. 압축 헤더 포맷은 길이 2byte, 커맨드 1byte이다.
            // 길이는 헤더 포함 패킷 길이이다.
            
            unsigned short usDataLength = usLength - sizeof(PktBase);
            unsigned short usPacketLength = usDataLength + sizeof(usPacketLength) + sizeof(cCMD);
                        
            m_lpSendAllBuffer->push(&usPacketLength, sizeof(usPacketLength));
            m_lpSendAllBuffer->push(&cCMD, sizeof(cCMD));

            // 데이터에서 헤더를 제거하고 복사한다.
            m_lpSendAllBuffer->push(lpPktBase + 1, usDataLength);
        }
    }
	*/
}

void CCell::SendNowAllNearCellCharacter(const char* szData, unsigned short usLength, unsigned char cCMD)
{
    CCell** lppConnectCellBound = m_lpConnectCell + CCell::CONNECT_NUM;

    for (CCell** lppCell = m_lpConnectCell; 
        lppCell != lppConnectCellBound; ++lppCell)
    {
        CCell* lpCell = *lppCell;

        if (NULL != lpCell)
        {
            lpCell->SendNowAllCharacter(szData, usLength, cCMD);
        }
    }
}

void CCell::SendNowAllCharacter(const char* szData, unsigned short usLength, unsigned char cCMD)
{
    CharacterList::iterator pos = m_lstCharacter.begin();
    CharacterList::iterator end = m_lstCharacter.end();

    CCharacter* lpCharacter = 0;
    CGameClientDispatch* lpDispatch = 0;

    for (; pos != end; ++pos)            
    {
        lpCharacter = *pos;

        if (0 != lpCharacter &&
            0 != (lpDispatch = lpCharacter->GetDispatcher()))
        {
            lpDispatch->GetSendStream().PutBuffer(szData, usLength, cCMD);
        }
    }
}



CCell* CCell::GetConnectCell(unsigned int nDir) 
{
	if (nDir <= CONNECT_NUM)
    {
        return m_lpConnectCell[nDir];
	}
	
	ERRLOG1(g_Log, "Cannot get incorrect direction cell : %d", nDir);
	return NULL;	
}

void CCell::SetConnectCell(unsigned int nDir, CCell* lpConnectedCell) 
{ 
    if (nDir <= CONNECT_NUM) 
    {
        m_lpConnectCell[nDir] = lpConnectedCell;
    }
    else
    {
		ERRLOG1(g_Log, "Cannot set incorrect direction cell : %d", nDir);
	}
}

bool CCell::IsNearCell(CCell* lpNearCell)
{
    CCell** lppConnectedCellPastEnd = m_lpConnectCell + CONNECT_NUM;

    for (CCell** lppCell = m_lpConnectCell;
		lppCell != lppConnectedCellPastEnd; ++lppCell)
    {
		if (*lppCell == lpNearCell) 
		{
			return true;
		}
	}

	return false;
}


size_t CCell::GetNearCellCharacterNum() const
{
    size_t nCharacterNum = 0;

    CCell* const* lppConnectCellBound = m_lpConnectCell + CCell::CONNECT_NUM;

    for (CCell* const* lppCell = m_lpConnectCell; lppCell != lppConnectCellBound; ++lppCell)
    {
        CCell* lpCell = *lppCell;

        if (NULL != lpCell)
        {
            nCharacterNum += lpCell->GetCharacterNum();
        }
    }

    return nCharacterNum;
};


void CCell::RespawnAllCharacter(unsigned char cExceptNation)
{
	if (0 == m_lstCharacter.size()) return;

	CharacterList::iterator itr = m_lstCharacter.begin();
	CCharacter* lpCharacter = NULL;

	while (itr != m_lstCharacter.end())
	{
		lpCharacter = (*itr);
		if (lpCharacter && cExceptNation != lpCharacter->GetNation())
		{
			lpCharacter->Respawn();

            if (0 != lpCharacter->GetPID())
            {		
                // 파티원 리스폰을 알린다.
                GameClientSendPacket::SendCharDeadToParty(lpCharacter, 0, PktDeadInfo::RESPAWN);
            }
		}

		++itr;
	}
}