///////////////////////////////////////////////////////////////////////////////////
//
// Purpose : 몬스터 정보를 저장 & 관리하는 클래스
//
///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//hz autoloot fix
#include <Item/Item.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Item/Container/ItemContainer.h>
#include <Item/Container/ExchangeContainer.h>

#include <Creature/Character/Character.h>


//hz end
#include <Utility/Registry/RegFunctions.h>
#include <Utility/Math/Math.h>
#include <Utility/Time/Pulse/Pulse.h>
#include <Utility/DelimitedFile.h>
#include <Utility/Setup/ServerSetup.h>

#include <Network/ClientSocket/ClientConstants.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>

#include <Creature/CreatureManager.h>
#include <Creature/Monster/PatternMonster.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/EnemyCheck.h>

#include <Community/Party/Party.h>
#include <Community/Party/PartyMgr.h>
#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <quest/QuestMgr.cpp>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include <Map/FieldMap/CellManager.h>
#include <Skill/Spell/Spell.h>
#include <Log/CharacterLog.h>

#include "AwardTable.h"
#include "Monster.h"
#include "MonsterShout.h"


LONG CMonster::ms_NormalBehaviorSendCount = 0; 
LONG CMonster::ms_AttackBehaviorSendCount = 0;
LONG CMonster::ms_ReturnBehaviorSendCount = 0;
LONG CMonster::ms_EscapeBehaviorSendCount = 0;
LONG CMonster::ms_DeadBehaviorSendCount = 0;


///////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////////
CMonster::CMonster() 
:	m_lpTarget(NULL), m_dwLastBehaviorTick(0), m_lCurrentFrame(0), m_bAttacking(false), m_nCurrentState(0), m_wSearchRange(0), 
	m_nNormalMovingDelay(0), m_nLeaveMovingNum(0), m_bAvoid(false), m_bLongRangeAttacked(false), m_bAdminCmdSummon(false),
	m_bScout(false), m_nMovingPattern(0), m_OriginalPosition(), CAggresiveCreature(0), m_dwPID(0), m_wRespawnArea(0)
{
	m_wDefaultSearchRange = MONSTER_SEARCH_RANGE;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::CMonster
//
// Description : 사용자 생성자
//
// Inputs : MonsterCreate - 몬스터 생성 정보
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
CMonster::CMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
:	m_lpTarget(NULL), m_dwLastBehaviorTick(0), m_lCurrentFrame(0), m_bAttacking(false), m_nCurrentState(0), m_wSearchRange(0), 
	m_nNormalMovingDelay(0), m_nLeaveMovingNum(0), m_bAvoid(false), m_bLongRangeAttacked(false), m_bAdminCmdSummon(bAdminCmdSummon),
	m_bScout(MonsterCreate.m_bScout), m_nMovingPattern(MonsterCreate.m_nMovingPattern), m_wRespawnArea(MonsterCreate.m_wRespawnArea),
	m_OriginalPosition(MonsterCreate.m_Pos), CAggresiveCreature(MonsterCreate.m_dwCID), m_dwPID(MonsterCreate.m_dwPID)
{
	const CMonsterMgr::MonsterProtoType* pProtoType = 
		CMonsterMgr::GetInstance().GetMonsterProtoType(MonsterCreate.m_nKID);
	
	if (NULL == pProtoType) 
	{
		ERRLOG1(g_Log, "알맞은 프로토타입이 없습니다. MonsterProtoType.txt를 확인해주십시오. KID:%d", MonsterCreate.m_nKID);
		return;
	}

	m_CreatureStatus = pProtoType->m_CreatureStatus;
	m_MonsterInfo = pProtoType->m_MonsterInfo;

	m_CreatureStatus.m_StatusInfo.CalculateSubStatus();

	// 게임중에 챈트 효과 계산을 위해 존재합니다.
	m_EquipStatus = pProtoType->m_CreatureStatus.m_StatusInfo;
	m_EquipStatus.m_cCalculateState = FightStatus::CS_EQUIP_INFO;

	m_wDefaultSearchRange = MONSTER_SEARCH_RANGE;
}


CMonster::~CMonster()
{
	m_SpellMgr.GetAffectedInfo().ClearAll();
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::InitMonster
//
// Description : 몬스터 초기화
//
// Inputs : Pos   - 몬스터의 위치
//			bDead - 처음 로긴 때는 true, 리스폰 시엔 false
//
// Outputs : None.
//
// Returns : 성공 여부.
///////////////////////////////////////////////////////////////////////////////////
bool CMonster::InitMonster(Position &Pos, bool bDead)
{
	m_CellPos.MoveTo(Pos);
	if (NULL == m_CellPos.m_lpCell)
	{
		ERRLOG4(g_Log, "CID:0x%08x 범위를 벗어난 셀에서 몬스터가 로긴하였습니다. X:%.1f, Y:%.1f, Z:%.1f", 
			m_dwCID, Pos.m_fPointX, Pos.m_fPointY, Pos.m_fPointZ);
		return false;
	}

	m_CurrentPos = Pos;
	m_wSearchRange = m_wDefaultSearchRange;

	m_lpTarget = NULL;

	m_lCurrentFrame = 0;
	m_bAttacking = false;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();

	ZeroMemory(&m_MotionInfo, sizeof(m_MotionInfo));

	if (bDead)
	{
		m_nCurrentState = STATE_ID_DIE;
		m_CreatureStatus.m_nNowHP = 0;
		m_CreatureStatus.m_nNowMP = 0;
	}
	else
	{
		m_CellPos.m_lpCell->SetCreature(m_dwCID, this);

		m_nCurrentState = STATE_ID_NORMAL;
		m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::GetMotion
//
// Description : 모션 정보를 얻음
//
// Inputs : MotionID - 모션 ID
//
// Outputs : Motion - 모션 정보
//
// Returns : bool - 정보가 없는 ID면 false.
///////////////////////////////////////////////////////////////////////////////////
bool CMonster::GetMotion(unsigned long MotionID, MotionInfo &Motion)
{
	int nIndex = 0;

	switch (MotionID) 
	{
		case MonsterInfo::Z3D_CA_WALK:		nIndex = 0;	break;
		case MonsterInfo::Z3D_CA_RUN:		nIndex = 1;	break;
		case MonsterInfo::Z3D_CA_ATTACK:	nIndex = 2;	break;
		case MonsterInfo::Z3D_CA_CASTING:	nIndex = 3;	break;

		default: return false;
	}

	// 방향은 복사하면 안 된다.
	Motion.m_wAction = m_MonsterInfo.m_MonsterMotions[nIndex].m_wAction;
	Motion.m_dwFrame = m_MonsterInfo.m_MonsterMotions[nIndex].m_dwFrame;
	Motion.m_fVelocity = m_MonsterInfo.m_MonsterMotions[nIndex].m_fVelocity;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::IsOverlap
//
// Description : 몬스터가 겹쳐져 있는가?
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : bool - 위 질문에 대한 Yes/No
///////////////////////////////////////////////////////////////////////////////////
bool CMonster::IsOverlap(void)
{
	for (int nCellCount = 0; nCellCount < CCell::CONNECT_NUM; ++nCellCount)
	{
		CCell* pCell = m_CellPos.m_lpCell->GetConnectCell(nCellCount);
		if (NULL == pCell || false == pCell->IsMonster())
		{
			continue;
		}

		CMonster* lpTempMonster = pCell->GetFirstMonster();

		while (NULL != lpTempMonster)
		{
			if (this != lpTempMonster)
			{
				const float fDX = lpTempMonster->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
				const float fDZ = lpTempMonster->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;

				if (fDX * fDX + fDZ * fDZ <= 1) {
					return true;
				}
			}

			lpTempMonster = pCell->GetNextMonster();
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::UpdateBehavior
//
// Description : 몬스터의 행동 설정
//
// Inputs : dwTick - 현재 틱카운트
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CMonster::UpdateBehavior(unsigned long dwTick)
{	
	// 네임드 몬스터는 스턴/석화에 걸리지 않는다.
	if (MonsterInfo::PATTERN_NAMED == m_MonsterInfo.m_cSkillPattern)
	{
		m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Stun);
		m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::StoneForm);		
	}

	// 스턴/석화시엔 아무 행동도 하지 않는다.
	if (GetEnchantInfo().GetFlag(Skill::SpellID::Stun) || 
        GetEnchantInfo().GetFlag(Skill::SpellID::StoneForm))
	{
		m_lCurrentFrame = FPS;
		return;
	}
	
	// 상태에 따른 몬스터 행동 처리를 한다.
	switch (m_nCurrentState) 
	{	
		case STATE_ID_NULL: break;
		case STATE_ID_NORMAL:	NormalBehavior(dwTick);		break;	// 보통 상태
		case STATE_ID_ATTACK:	AttackBehavior(dwTick);		break;	// 공격 상태 (거리가 멀어지면 쫓아가고...)
		case STATE_ID_RETURN:	ReturnBehavior(dwTick);		break;	// 리턴 상태 (제 위치로 돌아가는...)		
		case STATE_ID_ESCAPE:	EscapeBehavior();			break;	// 도망 상태		
		case STATE_ID_DIE:		DeadBehavior(dwTick);		break;	// 죽은 상태 (리스폰 시간이 지나면 리스폰 하지~)
		
	}

/*
	// 몬스터 정보 로그
	char logString[MAX_PATH];
	switch (m_nCurrentState)
	{
		case STATE_ID_NORMAL:	strcpy(logString, "Normal");			break;
		case STATE_ID_ATTACK:	strcpy(logString, "Attack");			break;
		case STATE_ID_RETURN:	strcpy(logString, "Return");			break;
		case STATE_ID_ESCAPE:	strcpy(logString, "Escape");			break;
		case STATE_ID_DIE:		strcpy(logString, "Die");				break;
	}

	DETLOG3(g_Log, "몬스터 정보 로그 - * State : %s\t* fX : %f\t* fZ : %f", 
		logString, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ);
*/
}



///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::Process
//
// Description : 몬스터 프로세싱
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : bool - 모션 출력 중엔 false
///////////////////////////////////////////////////////////////////////////////////
bool CMonster::Process()
{	
	const unsigned long dwTick = CPulse::GetInstance().GetLastTick();

	if (dwTick - m_dwLastBehaviorTick < FPS) {
		return false;
	}

	if ((GetSerialNumber() % 3) != (CPulse::GetInstance().GetCurrentPulse() % 3)) 
    {
		return false;
	}

	const unsigned long dwFrame = ((dwTick - m_dwLastBehaviorTick) / FPS);

	// 공격 모션은 따로 처리합니다. (이동하면서 공격 가능)
	if (m_lCurrentFrame <= 0 || m_bAttacking == true) 
	{
		if (m_lCurrentFrame <= 0 && m_bAttacking == true) {
			m_bAttacking = false;
		}

		UpdateBehavior(dwTick);
	}

	// 틱 갱신
	m_dwLastBehaviorTick = dwTick;

	unsigned long dwSlowlyRate = (true == GetEnchantInfo().GetFlag(Skill::SpellID::Frozen)) ? 2 : 1;

	m_lCurrentFrame -= (dwFrame / dwSlowlyRate);

	return true;
}



///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::MultiAttack
//
// Description : 몬스터 범위 공격 시 범위 내 타겟을 검색
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
bool CMonster::MultiAttack(void)
{
	unsigned char cDefenderNum = 1;
	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM];
	unsigned char nDefenserJudges[AtNode::MAX_DEFENDER_NUM];

	ppAggresiveCreature[0] = m_lpTarget;

	// TODO : 공격하는 방향을 설정합시다. (현재는 무조건 정면)
	short nMaxDefenderNum = AtNode::MAX_DEFENDER_NUM;
	std::fill_n(&nDefenserJudges[0], nMaxDefenderNum, ClientConstants::Judge_Front);

	float fDir = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
		m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

	AtType attackType;
	attackType.m_wType = AtType::RIGHT_MELEE;
	char cTargetType = Skill::Target::ENEMY;

	return CAggresiveCreature::MultiAttack(attackType, cDefenderNum, ppAggresiveCreature, nDefenserJudges, 
		GetCurrentPos(), fDir, m_MonsterInfo.m_wAttackRange / 100.0f, m_MonsterInfo.m_fAttackAngle, cTargetType);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::CancelTarget
//
// Description : 타겟의 취소
//
// Inputs : bool bSaveThreat - 맞은 쓰레트 값을 유지하는가?
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CMonster::CancelTarget(bool bSaveThreat)
{
	if (NULL != m_lpTarget) 
	{
		if (!bSaveThreat)
		{
			m_lpTarget->GetThreat().DeleteThreatened(this);
			m_Threat.DeleteThreat(m_lpTarget);
		}

		// 파티가 있었다면.. 파티의 타겟에서 제거한다.
		CMonsterParty* lpParty = reinterpret_cast<CMonsterParty*>(GetParty());
		if (NULL != lpParty)
		{
			CMonsterParty::PartyTargetSet::iterator itr = lpParty->GetPartyTargetSet().find(m_lpTarget->GetCID());
			if (itr != lpParty->GetPartyTargetSet().end())
			{
				lpParty->GetPartyTargetSet().erase(itr);
			}
		}
	}

	m_lpTarget = NULL;
	m_bAttacking = false;
	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_LEAVE_PLAYER);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::Dead
//
// Description : 몬스터의 사망 처리
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : 성공 여부. (false면 타겟도 없는 주제에 죽은 경우)
///////////////////////////////////////////////////////////////////////////////////
bool CMonster::Dead(CAggresiveCreature* pOffencer)
{
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	
	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);
	
	// 어워드 처리
	unsigned long aryItemID[AwardTable::MAX_DROP_ITEM + EliteBonus::MAX_BONUS_DROP_ITEM] = { 0, };

	Item::CItem* aryItem[AwardTable::MAX_DROP_ITEM + EliteBonus::MAX_BONUS_DROP_ITEM] = { 0, };

	unsigned long dwOwnerID = 0;
	unsigned long dwMonI = this->GetCID() & 0x0000FFFF;
	unsigned char cItemNum=0;
	if((dwMonI >= 3100) && (dwMonI <= 3450))
		cItemNum = m_Threat.GetAward(aryItemID, aryItem, &dwOwnerID, true);
	else
		cItemNum = m_Threat.GetAward(aryItemID, aryItem, &dwOwnerID);

	Position		SetPosition;
	CCell::ItemInfo itemInfo;

    CCell* lpDropCell =  GetCellPos().m_lpCell;

    if (NULL != lpDropCell)
    {
	    for (unsigned char cDropIndex = 0; cDropIndex < cItemNum; ++cDropIndex)//was (unsigned char cDropIndex = 0; cDropIndex < cItemNum; ++cDropIndex)
	    {
		    if (0 != aryItemID[cDropIndex]) 
            {
		        SetPosition = GetCurrentPos();
		        SetPosition.m_fPointX += cDropIndex;
		        if (cItemNum / 2 <= cDropIndex) 
		        { 
			        SetPosition.m_fPointX -= cItemNum / 2;
			        SetPosition.m_fPointZ += 1.0f; 
		        }

				unsigned long dwGold = (NULL == aryItem[cDropIndex]) ? (aryItemID[cDropIndex] & ~CCell::TYPE_CHECK_BIT) : 0;

				if(pOffencer->GetStatus().m_nLevel<=GOLD_INC_LEVEL_LIMIT)
				{
					dwGold = static_cast<unsigned long>( dwGold * 1.5f );
				}
				//hayzohar- no autoloot line:
				//lpDropCell->SetItem(SetPosition, aryItem[cDropIndex], dwGold, dwOwnerID, CCell::PARTY, itemInfo);

				CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter( dwOwnerID );
				if (NULL != aryItem[cDropIndex])
					{
						if((aryItem[cDropIndex]->GetItemInfo().m_DetailData.m_cItemType == 44)
							||(aryItem[cDropIndex]->GetItemInfo().m_DetailData.m_cItemType == 54))//44 is potion, 54 is gem
							continue;	
					}
				CParty* lpParty = lpMaster->GetParty();
				if(lpParty)
				{
					lpParty->AutoRouting(lpMaster, aryItem[cDropIndex], dwGold);
				}
				else if (NULL != aryItem[cDropIndex])
					{
						lpMaster->GiveItem(aryItem[cDropIndex]);
					}
				else if(dwGold != 0)
					lpMaster->AddGold(dwGold,true);
					
            }
	    }
    }

    // 경험치를 분배한다.
	const long lMaxThreat = m_Threat.GetMaxThreatAmount();
	m_Threat.DivisionExp();

    // 셀에서 몬스터를 일단 제거한다.
	m_CellPos.m_lpCell->DeleteCreature(m_dwCID);

	CAggresiveCreature *pCreature = m_Threat.GetMaxThreatCreature();
	if (NULL != pCreature)
	{
		if (m_CreatureStatus.m_nLevel - pCreature->GetStatus().m_nLevel >= 3 && 
			lMaxThreat > m_CreatureStatus.m_StatusInfo.m_nMaxHP * 0.8f)
		{	// 레벨차가 3 이상 나는 몬스터를 잡았을 경우 로그를 찍음
			RULLOG5(g_Log, "몬스터께서 돌아가셨습니다... Monster : 0x%08x (%d), Player : 0x%08x (%d), 획득 경험치 비율 : %.1f%%", 
				m_dwCID, m_CreatureStatus.m_nLevel, m_Threat.GetMaxThreatCreature()->GetCID(),
				m_Threat.GetMaxThreatCreature()->GetStatus().m_nLevel, lMaxThreat * 100.0f / m_CreatureStatus.m_StatusInfo.m_nMaxHP);
		}

		CCharacter* lpOffencerCharacter = 
			(Creature::CT_PC == Creature::GetCreatureType(pCreature->GetCID()))
			? static_cast<CCharacter*>(pCreature) : 0;

		if (0 != lpOffencerCharacter)
		{
			// 퀘스트 트리거 발동
			lpOffencerCharacter->CheckTrigger(Quest::TRIGGER_KILL, m_MonsterInfo.m_dwKID, m_CurrentPos, 1);

			// 몬스터 살해 로그를 남긴다. 남기는 항목은 몬스터 레벨/떨군 아이템ID 및 등급, 개수등이다
			GAMELOG::LogMonsterDead(*lpOffencerCharacter, GetCID(), 
				m_CreatureStatus.m_nLevel, aryItemID, cItemNum);
		}
	}

	// 파티에서 제거
	CParty* lpParty = GetParty();
	if (NULL != lpParty)
	{
		lpParty->Leave(GetCID(), 0, GetMapIndex());
	}

	return CAggresiveCreature::Dead(pOffencer);
}


EnemyCheck::EnemyType CMonster::IsEnemy(CCreature* lpTarget, unsigned char* cResult)
{
	if (NULL != lpTarget)
	{
		switch (Creature::GetCreatureType(lpTarget->GetCID()))
		{
			case Creature::CT_PC:		
			case Creature::CT_SUMMON:
			case Creature::CT_SIEGE_OBJECT:
			{				
				return lpTarget->IsEnemy(this);
			}

			case Creature::CT_NPC:
			case Creature::CT_MONSTER:		
			case Creature::CT_STRUCT:
			{
				if (GetNation() == lpTarget->GetNation())
				{
					return EnemyCheck::EC_FRIEND;
				}

				return EnemyCheck::EC_ENEMY;
			}
		}
	}

	ERRLOG1(g_Log, "CID:0x%08x 피아식별할 타겟이 없습니다.", m_dwCID);
	return EnemyCheck::EC_NEUTRAL;
}


void CMonster::Respawn(unsigned long dwTick)
{
	Position RespawnPos(m_OriginalPosition.m_fPointX, m_OriginalPosition.m_fPointY, m_OriginalPosition.m_fPointZ);

	if (m_nMovingPattern != FIXED && m_wRespawnArea > 0)
	{
		RespawnPos.m_fPointX += static_cast<float>(Math::Random::SimpleRandom(dwTick, m_wRespawnArea*2) - m_wRespawnArea);
		RespawnPos.m_fPointY += m_OriginalPosition.m_fPointY;
		RespawnPos.m_fPointZ += static_cast<float>(Math::Random::SimpleRandom(dwTick, m_wRespawnArea*2) - m_wRespawnArea);
	}

	InitMonster(RespawnPos);
	m_nCurrentState = STATE_ID_NORMAL;

	// 파티에 추가
	CParty* lpParty = CPartyMgr::GetInstance().GetParty(GetPID());
	if (NULL != lpParty)
	{
		SetParty(lpParty);
		lpParty->Join(GetCID(), 0, NULL, GetMapIndex());
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::SendMove
//
// Description : 몬스터 이동 패킷 보내기
//
// Inputs : None.
//
// Outputs : Send를 보낸 횟수.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
LONG CMonster::SendMove(unsigned short nAniNum)
{
	// 길드 요새와 상징물은 MonMove 를 보내지 않는다. (공격시 방향 전환을 하지않도록...)
	if (Creature::CT_SIEGE_OBJECT == Creature::GetCreatureType(m_dwCID))
	{
		CSiegeObject* lpSiegeObject = reinterpret_cast<CSiegeObject* >(this);
		if (lpSiegeObject && (lpSiegeObject->IsCamp() || lpSiegeObject->IsEmblem()))
		{
			return 0;
		}
	}

	LONG nSendCount = 0;

	if (true == GetEnchantInfo().GetFlag(Skill::SpellID::Hold) || 
		true == GetEnchantInfo().GetFlag(Skill::SpellID::Stun) || 
		true == GetEnchantInfo().GetFlag(Skill::SpellID::StoneForm)) 
	{
		m_MotionInfo.m_fVelocity = 0;
	}

	if (NULL == m_CellPos.m_lpCell) 
	{
		ERRLOG0(g_Log, "몬스터 이동 패킷 보내기 실패 : 몬스터가 셀을 벗어나 있습니다.");
		return nSendCount;
	}

	PktMM pktMM;
    memset(&pktMM, 0, sizeof(PktMM));

	pktMM.m_dwMonID = m_dwCID;

	pktMM.m_NetworkPos = CNetworkPos(m_CurrentPos.m_fPointX, m_CurrentPos.m_fPointY, m_CurrentPos.m_fPointZ, 
		m_MotionInfo.m_fDirection, (0 == m_MotionInfo.m_dwFrame) ? 0.0f : m_MotionInfo.m_fVelocity / m_MotionInfo.m_dwFrame);
	
	pktMM.m_cAct    = static_cast<unsigned char>(m_MotionInfo.m_wAction);
	pktMM.m_cAniNum = static_cast<unsigned char>(nAniNum);

	// BroadCasting 한 번을 기준으로 카운트
    if (0 != m_CellPos.m_lpCell)
    {
        ++nSendCount;
        m_CellPos.m_lpCell->SendAllNearCellCharacter(&pktMM, sizeof(PktMM), CmdMonMove);
    }
    else
    {
        ERRLOG4(g_Log, "CID:0x%08x 이상한 위치에 몬스터가 있습니다. (%f,%f,%f)", m_dwCID, 
            m_CurrentPos.m_fPointX, m_CurrentPos.m_fPointY, m_CurrentPos.m_fPointZ);
    }
		
	m_nLeaveMovingNum = nAniNum;

	return nSendCount;
}


const int CMonster::CalculateFixLevelGap(CAggresiveCreature *pDefender)
{
	// << 고정 레벨 갭 장치 >>
	// - 캐릭터와 몬스터의 레벨에 상관없이 스크립트에 정의된 수치만큼 몬스터의 레벨이 높은 걸로 취급한다. 
	// - 보스몹 등에 이용한다.

	if (true == m_MonsterInfo.m_bFixLevelGap)
	{
		const unsigned char cFixLevelGap = m_CreatureStatus.m_nLevel - m_MonsterInfo.m_cFixLevelGap;
		if (cFixLevelGap < pDefender->GetStatus().m_nLevel)
		{
			return m_MonsterInfo.m_cFixLevelGap;
		}
	}

	return CAggresiveCreature::CalculateLevelGap(pDefender);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::Attack
//
// Description : 몬스터 공격 패킷 보내기
//
// Inputs : pDefender - 방어자의 포인터
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
bool CMonster::Attack(AtType attackType, unsigned char cDefenderNum,
		CAggresiveCreature** ppDefenders, unsigned char* cDefenderJudges, unsigned short* wDefenderMPHeal)
{
	if (m_CreatureStatus.m_nNowHP == 0) 
	{
		ERRLOG1(g_Log, "CID:0x%08x 죽은 몬스터가 공격하려고 하였습니다.", m_dwCID);
		return false;
	}

	if (cDefenderNum > AtNode::MAX_DEFENDER_NUM) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 몬스터가 공격할 때, 방어자의 숫자가 최대 방어자 숫자를 넘었습니다. 방어자수 : %d", 
			m_dwCID, cDefenderNum);
		cDefenderNum = AtNode::MAX_DEFENDER_NUM;
	}

	// MON_TODO : by Vincent - 2004 : 2 : 25
	DefenserNode Defenser[AtNode::MAX_DEFENDER_NUM] = {0, };
	int nDefenserCount = 0;
	// ------------

	for (unsigned char cDefender = 0; cDefender < cDefenderNum; ++cDefender) 
	{
		if (NULL == ppDefenders[cDefender]) { continue; }

		CCharacter* lpCharacter = NULL;
		CMonster* lpSummonee = NULL;

		// 타켓이 캐릭터인 경우
		if (Creature::CT_PC == Creature::GetCreatureType(ppDefenders[cDefender]->GetCID()))
		{
			lpCharacter = reinterpret_cast<CCharacter *>(ppDefenders[cDefender]);
			lpSummonee = lpCharacter->GetSummonee();
		}
		else
		{
			// 타겟이 소환수인 경우
			if (Creature::IsSummonMonster(ppDefenders[cDefender]->GetCID()))
			{
				lpCharacter = reinterpret_cast<CSummonMonster *>(ppDefenders[cDefender])->GetMaster();
			}
		}

		if (NULL == lpCharacter) { continue; }

		unsigned char cOffencerJudge = 0;
		unsigned short wOffencerMPHeal = 0;
		unsigned short wError = PktBase::NO_SERVER_ERR;

		const unsigned short wDamage = 
			ppDefenders[cDefender]->ApplyDamage(attackType, this, cOffencerJudge, cDefenderJudges[cDefender], wOffencerMPHeal, wDefenderMPHeal[cDefender], wError);

		if (NULL != lpSummonee)
		{
			lpSummonee->GuardMe(this, wDamage);
		}

		lpCharacter->CalculateEquipDurability((ClientConstants::Judge_Guard == cDefenderJudges[cDefender]) ? 
			AtType::GUARD : AtType::DEFENCE);

		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), this, ppDefenders[cDefender], 
				attackType, m_MotionInfo.m_fDirection, wDamage, cDefenderJudges[cDefender], wDefenderMPHeal[cDefender], wError);
		}

		Defenser[nDefenserCount].m_cJudge	= cDefenderJudges[nDefenserCount];
		Defenser[nDefenserCount].m_dwCharID	= ppDefenders[nDefenserCount]->GetCID();
		Defenser[nDefenserCount].m_wMaxHP	= ppDefenders[nDefenserCount]->GetStatus().m_StatusInfo.m_nMaxHP;
		Defenser[nDefenserCount].m_wMaxMP	= ppDefenders[nDefenserCount]->GetStatus().m_StatusInfo.m_nMaxMP;
		Defenser[nDefenserCount].m_sCurrHP	= ppDefenders[nDefenserCount]->GetStatus().m_nNowHP;
		Defenser[nDefenserCount].m_sCurrMP	= ppDefenders[nDefenserCount]->GetStatus().m_nNowMP;
		Defenser[nDefenserCount].m_wMPHeal	= wDefenderMPHeal[nDefenserCount];
		Defenser[nDefenserCount].m_wDamage	= wDamage;
		++nDefenserCount;
	}

	CCell* lpCell = m_CellPos.m_lpCell;
	if (NULL != lpCell)
	{
		lpCell->SendAttackInfo(GetCID(), attackType, static_cast<unsigned char>(nDefenserCount), &Defenser[0]);
	}

	return true;
}


bool CMonster::HasSkill()
{
	for (int i=0; i<MonsterInfo::MAX_SKILL_PATTERN; ++i)
	{
		if (0 != m_MonsterInfo.m_wSkillID[i]) return true;
	}

	return false;
}


int	CMonster::GetUseSkillNum()	// 몇가지의 스킬을 가지고 있는가?
{
	int count = 0;
	for (int i=0; i<MonsterInfo::MAX_SKILL_PATTERN; ++i)
	{
		if (0 != m_MonsterInfo.m_wSkillID[i]) ++count;
	}

	return count;
}


unsigned long CMonster::GetDropRate(unsigned char cIndex)
{ 
	switch (cIndex)
	{
		case MonsterInfo::LOTTERY:
		{
			if (false == CServerSetup::GetInstance().GetLotteryEvent())
			{
				return 0;
			}
			break;
		}

		case MonsterInfo::F_RING:
		case MonsterInfo::D_RING:
		case MonsterInfo::C_RING:
		case MonsterInfo::B_RING:
		case MonsterInfo::A_RING:
		case MonsterInfo::F_NECKLACE:
		case MonsterInfo::D_NECKLACE:
		case MonsterInfo::C_NECKLACE:
		case MonsterInfo::B_NECKLACE:
		case MonsterInfo::A_NECKLACE:
		{
			if (false == CServerSetup::GetInstance().UseContents(GameRYL::ACCESSORY))
			{
				return 0;
			}
			break;
		}

		case MonsterInfo::F_RUNE:
		case MonsterInfo::D_RUNE:
		case MonsterInfo::C_RUNE:
		case MonsterInfo::B_RUNE:
		case MonsterInfo::A_RUNE:
		case MonsterInfo::DESTRUCTION_RUNE:
		{
			if (false == CServerSetup::GetInstance().UseContents(GameRYL::RUNE))
			{
				return 0;
			}
			break;
		}
	}

	time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );
	
	if (now->tm_mday >=8 && now->tm_mday <= 14 && cIndex== MonsterInfo::METAL ){
		return m_MonsterInfo.m_aryDropRate[cIndex] * 1.2; // extra 20% metal drop rate
	}

	if (now->tm_mday >=15 && now->tm_mday <= 21 && cIndex== MonsterInfo::GEM  ){
		return m_MonsterInfo.m_aryDropRate[cIndex] * 1.2; // extra 20% Gem drop rate
	}

	if (now->tm_mday >=22  && cIndex== MonsterInfo::COIN ){
		return m_MonsterInfo.m_aryDropRate[cIndex] * 1.2; // extra 20% Gold drop rate
	}

	return m_MonsterInfo.m_aryDropRate[cIndex]; 
}	


bool CMonster::IsDeadSummonMonster(void)
{
	if (m_bAdminCmdSummon)
	{
		return (STATE_ID_DIE == m_nCurrentState);
	}

	return false;
}


void CMonster::LogMonsterMoveCount()
{
	SERLOG5(g_Log, "몬스터의 이동 전송 회수 로그를 출력합니다. "
		"NormalBehaviorSendCount:%10d, AttackBehaviorSendCount:%10d, ReturnBehaviorSendCount:%10d, "
		"EscapeBehaviorSendCount:%10d, DeadBehaviorSendCount:%10d ", 
        ms_NormalBehaviorSendCount, ms_AttackBehaviorSendCount, ms_ReturnBehaviorSendCount,
        ms_EscapeBehaviorSendCount, ms_DeadBehaviorSendCount);

	ms_NormalBehaviorSendCount = ms_AttackBehaviorSendCount = ms_ReturnBehaviorSendCount = 
		ms_EscapeBehaviorSendCount = ms_DeadBehaviorSendCount = 0;
}


unsigned short CMonster::ApplyDamage(AtType attackType, CAggresiveCreature* pOffencer, unsigned char &cOffencerJudge,
									 unsigned char &cDefenserJudge, unsigned short& wOffencerMPHeal, unsigned short& wDefenserMPHeal,
									 unsigned short &wError)
{
    unsigned short usDamage = CAggresiveCreature::ApplyDamage(
        attackType, pOffencer, cOffencerJudge, cDefenserJudge, wOffencerMPHeal, wDefenserMPHeal, wError);

    // 몬스터 외치기 타입 찾기
    CMonsterShout::Behavior eBehavior = CMonsterShout::NORMAL_ATTACK;
    unsigned short usShoutSkill_ID = 0;

    if (0 == (attackType.m_wType & AtType::SKILL_BIT))
    {
        // 스킬 공격이 아니다.
        if (cDefenserJudge == ClientConstants::Judge_Critical)
        {
            eBehavior = CMonsterShout::CRITICAL_ATTACKED;
        }
    }
    else
    {
        eBehavior = CMonsterShout::SKILL_ATTACKED;
        usShoutSkill_ID = attackType.m_wType;
    }
    
    const char* szCharacterName = 0;   
	//hz added this:
	if(!pOffencer)
		return usDamage;

    if (0 != pOffencer && Creature::CT_PC == Creature::GetCreatureType(pOffencer->GetCID()))//
    {
        szCharacterName = static_cast<CCharacter*>(pOffencer)->GetCharacterName();
    }

    // 외치기 검색해서 외침.
    CMonsterShout::GetInstance().Shout(m_dwCID, m_MonsterInfo.m_dwKID, 
        static_cast<unsigned short>(m_CurrentPos.m_fPointX),
        static_cast<unsigned short>(m_CurrentPos.m_fPointZ),
        eBehavior, szCharacterName, usShoutSkill_ID);
    
    return usDamage;

}
