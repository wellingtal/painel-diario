#include "stdafx.h"

#include <Utility/Math/Math.h>
#include <Utility/Setup/ServerSetup.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/Spell.h>

#include <Network/ClientSocket/ClientConstants.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharStatusPacketStruct.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap//VirtualArea/VirtualAreaMgr.h>
#include <Map/DuelMap/DuelCellManager.h>

#include <Community/Party/PartyMgr.h>
#include <Log/BugPlayLog.h>

#include "./Character/Character.h"
#include "./Monster/Monster.h"
#include "./Siege/SiegeObject.h"
#include "./Siege/Camp.h"
#include "./Siege/CastleGate.h"
#include "./Siege/CastleEmblem.h"
#include "CreatureManager.h"
#include "AggresiveCreature.h"
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Packet/PacketCommand.h>


CellPosition::CellPosition()
:	m_lpCell(NULL), m_wMapIndex(0), m_cCellX(0), m_cCellZ(0), m_wInX(0), m_wInZ(0)
{

}

CellPosition::CellPosition(const Position& WorldPos) 
{ 
	MoveTo(WorldPos); 
}

bool CellPosition::MoveTo(const Position& WorldPos) 
{
	// ���� ©���� �ʵ��� ����!
	int nCellX = 0;
	int nCellZ = 0;

	Position pos = WorldPos;
	if (0 != m_wMapIndex)
	{
		VirtualArea::CVirtualArea* lpVirtualArea = 
            VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(m_wMapIndex);

		pos.m_fPointX -= lpVirtualArea->GetStartX();
		pos.m_fPointZ -= lpVirtualArea->GetStartZ();
	}

	nCellX = static_cast<int>(pos.m_fPointX) >> CCell::CELL_RESOLUTION;
	nCellZ = static_cast<int>(pos.m_fPointZ) >> CCell::CELL_RESOLUTION;

	m_wInX = static_cast<int>((pos.m_fPointX - (nCellX << CCell::CELL_RESOLUTION)) * CCell::CELL_DISTANCE);
	m_wInZ = static_cast<int>((pos.m_fPointZ - (nCellZ << CCell::CELL_RESOLUTION)) * CCell::CELL_DISTANCE);

	m_cCellX = static_cast<unsigned char>(nCellX);
	m_cCellZ = static_cast<unsigned char>(nCellZ);

    m_lpCell = CCellManager::GetInstance().GetCell(m_wMapIndex, m_cCellX, m_cCellZ);

	return (NULL != m_lpCell) ? true : false;
}


CAggresiveCreature::CAggresiveCreature(unsigned long dwCID)
:	m_dwLastTime(0), m_pParty(NULL), CCreature(dwCID), m_bLogout(false), m_bSitDown(false)
{
	m_Threat.SetOwner(this);
	m_SpellMgr.SetOwner(this);

	std::fill_n(m_aryEnchantLevel, int(Skill::SpellID::MAX_SPELL_TYPE), 0);
}

CAggresiveCreature::~CAggresiveCreature()
{
	m_Threat.ClearAll();
}

CAggresiveCreature::MoveInfo CAggresiveCreature::MoveTo(const Position& NewPosition, const bool bSitDown)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	if (m_bLogout || 
		true == GetEnchantInfo().GetFlag(Skill::SpellID::Hold) || 
		true == GetEnchantInfo().GetFlag(Skill::SpellID::Stun) || 
		true == GetEnchantInfo().GetFlag(Skill::SpellID::StoneForm))
	{
        return F_CELL_CANNOTCHANGE;
    }

	// ��� ���¶�� ��� ���� ó��
	if (CServerSetup::GetInstance().GetDuelModeCheck() && NULL != GetDuelOpponent())
	{
		m_CurrentPos = NewPosition;
		m_bSitDown = bSitDown;

		return S_CELL_UNCHANGED;
	}
	else
	{
		// TODO : ��ġ�� ���ڱ� ���� Ƣ���� �� �����ϴ� �۾� �ʿ�.

        /* ��ġ�� Ƣ�� ��� �α� Ȯ��
        if (49 < m_CurrentPos.GetSquaredDistance(Position(0, 0, 0)) &&
            49 < m_CurrentPos.GetSquaredDistance(NewPosition))
        {
            const int MAX_BUFFER = 256;
            char szBuffer[MAX_BUFFER];

            if (0 < _snprintf(szBuffer, MAX_BUFFER - 1, 
                "MoveTo : CID:0x%08x : �����̵��ϳ�?.. %d�Ÿ��� ���տ��� �̵��ߴ�!"
                "/CurrentPulse:%d (%f,%f,%f)->(%f,%f,%f)\n", 
                m_dwCID, m_CurrentPos.GetSquaredDistance(NewPosition), 
                CPulse::GetInstance().GetCurrentPulse(),
                m_CurrentPos.m_fPointX, m_CurrentPos.m_fPointY, m_CurrentPos.m_fPointZ,
                NewPosition.m_fPointX, NewPosition.m_fPointY, NewPosition.m_fPointZ))
            {
                szBuffer[MAX_BUFFER - 1] = 0;
                OutputDebugString(szBuffer);
            }        
        }
        */

		m_CurrentPos = NewPosition;
		m_bSitDown = bSitDown;
        		
		CCell* lpLastCell = m_CellPos.m_lpCell;
		
        if (!m_CellPos.MoveTo(m_CurrentPos))
		{
			ERRLOG4(g_Log, "CID:0x%08x �̻��� ��ġ�� �� �ֽ��ϴ�. (%f, %f, %f)",
				m_dwCID, m_CurrentPos.m_fPointX, m_CurrentPos.m_fPointY, m_CurrentPos.m_fPointZ);
		}

		if (m_CellPos.m_lpCell == lpLastCell)
		{
			// ���� �� �ȿ� �ִ� ���.
			return S_CELL_UNCHANGED;
		}
		else 
        {
            if (0 != lpLastCell)
		    {
			    // �ϴ� �̵��� ������, �� �α׾ƿ��� �Ѵ�.
			    lpLastCell->DeleteCreature(m_dwCID, m_CellPos.m_lpCell);
		    }
            
            if (NULL == m_CellPos.m_lpCell)
		    {
			    // ��ü �� ������ ������ ���
			    return F_CELL_CANNOTCHANGE;
		    }
        }

		m_CellPos.m_lpCell->SetCreature(m_dwCID, this, lpLastCell);
		return S_CELL_CHANGED;	
	}
}


const int CAggresiveCreature::CalculateLevelGap(CAggresiveCreature *pDefender) 
{ 
	return CalculateLevelGap(m_CreatureStatus.m_nLevel, pDefender->GetStatus().m_nLevel);
}

const int CAggresiveCreature::CalculateLevelGap(int nOffencerLevel, int nDefenderLevel) 
{ 
	// edith 2008.02.13 ���� �����W�� ���� �����˻�
	// ���ͷ��� - �˻���
	// ���� ������ �� ũ�� nLevelGap �� ���
	// ���� ������ �� ������ �������̳��´�.
	int nLevelGap = nOffencerLevel - nDefenderLevel;

	// -20�� �������� �ִ밪
	nLevelGap = max(MAX_LEVEL_GAP * (-1), nLevelGap);
	
	// ���Ͱ� 20���� �� ���� ������ �ִ� 20���� ��´�.
	nLevelGap = min(nLevelGap, int(MAX_LEVEL_GAP));

	return nLevelGap;
}

const float CAggresiveCreature::CalculateLevelGapAffect(CAggresiveCreature *pDefender) 
{ 
    float fFinalAffect = 1.0f;

	// ���Ϳ��� ����
	Creature::CreatureType eCreatureType = Creature::GetCreatureType(m_dwCID); 
	Creature::CreatureType eDefenderCreatureType = Creature::GetCreatureType(pDefender->GetCID());

	if (Creature::CT_MONSTER == eCreatureType ||
		Creature::CT_SUMMON == eCreatureType ||
		Creature::CT_STRUCT == eCreatureType ||
		Creature::CT_MONSTER == eDefenderCreatureType ||
		Creature::CT_SUMMON == eDefenderCreatureType ||
		Creature::CT_STRUCT == eDefenderCreatureType)
	{
/*
		// ROW��
		static const float fHuntingAffect[AGGRAVATION_NUM] = { 
			0.1f,	0.105f,	0.11f,	0.12f,	0.13f,	0.14f,	0.15f,	0.17f,	0.18f,	0.2f,	
			0.22f,	0.25f,	0.27f,	0.40f,	0.52f,	0.63f,	0.73f,	0.82f,	0.89f,	0.95f, 
			1.0f, 
			1.01f,	1.02f,	1.03f,	1.04f,	1.05f,	1.06f,	1.07f,	1.08f,	1.09f,	1.1f,	
			1.11f,	1.12f,	1.13f,	1.14f,	1.15f,	1.16f,	1.17f,	1.18f,	1.19f,	1.2f
		};
*/

		// ROW + RYL2
		static const float fHuntingAffect[AGGRAVATION_NUM] = { 
			0.1f,	0.105f,	0.11f,	0.12f,	0.13f,	0.14f,	0.15f,	0.17f,	0.18f,	0.2f,	
			0.22f,	0.25f,	0.27f,	0.40f,	0.52f,	0.63f,	0.73f,	0.82f,	0.89f,	0.95f, 
			1.0f, 
			1.015f, 1.03f,	1.045f, 1.06f,	1.075f, 1.09f,	1.105f, 1.12f,	1.135f, 1.15f,	
			1.165f, 1.18f,	1.195f, 1.21f,	1.225f, 1.24f,	1.255f, 1.27f,	1.285f, 1.3f

//			1.025f, 1.05f,	1.075f, 1.10f,	1.125f, 1.15f,	1.175f, 1.20f,	1.225f, 1.25f,	
//			1.275f, 1.30f,	1.325f, 1.35f,	1.375f, 1.40f,	1.425f, 1.45f,	1.475f, 1.5f

//			1.05f,	1.10f,	1.15f,	1.20f,	1.25f,	1.30f,	1.35f,	1.40f,	1.45f,	1.5f,	
//			1.55f,	1.60f,	1.65f,	1.70f,	1.75f,	1.80f,	1.85f,	1.90f,	1.95f,	2.0f
		};

/*
		static const float fHuntingAffect[AGGRAVATION_NUM] = { 
			0.1f,	0.105f,	0.11f,	0.12f,	0.13f,	0.14f,	0.15f,	0.17f,	0.18f,	0.2f,	
			0.22f,	0.25f,	0.27f,	0.33f,	0.45f,	0.5f,	0.6f,	0.7f,	0.8f,	0.9f, 
			1.0f, 
			1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	
			1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f
		};
*/
/*
		// ����
		static const float fHuntingAffect[AGGRAVATION_NUM] = { 
			0.01f,	0.02f,	0.03f,	0.04f,	0.05f,	0.06f,	0.07f,	0.08f,	0.09f,	0.1f,	
			0.15f,	0.2f,	0.25f,	0.3f,	0.4f,	0.5f,	0.6f,	0.7f,	0.8f,	0.9f, 
			1.0f, 
			1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	
			1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f
		};
*/
		fFinalAffect = fHuntingAffect[CalculateFixLevelGap(pDefender) + MAX_LEVEL_GAP];
	}
	// PvP
	else
	{
		// ----------------------------------------------------------------------------------
		// ������ ���� ����

		CCharacter* lpOffencer = reinterpret_cast<CCharacter *>(this);
		CCharacter* lpDefender = reinterpret_cast<CCharacter *>(pDefender);

//		static const float fPvPAffect[AGGRAVATION_NUM] = {
//			0.6f,	0.6f,	0.6f,	0.6f,	0.6f,	0.6f,	0.6f,	0.6f,	0.6f,	0.6f,	
//			0.6f,	0.64f,	0.68f,	0.72f,	0.76f,	0.8f,	0.84f,	0.88f,	0.92f,	0.96f,	
//			1.0f,	
//			1.08f,	1.16f,	1.24f,	1.32f,	1.4f,	1.48f,	1.56f,	1.64f,	1.72f,	1.8f,
//			1.8f,	1.8f,	1.8f,	1.8f,	1.8f,	1.8f,	1.8f,	1.8f,	1.8f,	1.8f
//		};
/*
		static const float fPvPAffect[AGGRAVATION_NUM] = {
			0.60f,	0.62f,	0.64f,	0.66f,	0.68f,	0.70f,	0.72f,	0.74f,	0.76f,	0.78f,	
			0.80f,	0.82f,	0.84f,	0.86f,	0.88f,	0.90f,	0.92f,	0.94f,	0.96f,	0.98f,	
			1.0f,	
			1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,
			1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f
		};
*/
		// ���翬 PVP ������
/*		// ��� �� PVP�϶� ������ ���� ���� ����. 0.9->0.8�� .
		static const float fPvPAffect[AGGRAVATION_NUM] = {
				0.52f,	0.54f,	0.56f,	0.58f,	0.60f,	0.62f,	0.64f,	0.66f,	0.67f,	0.69f,	
				0.71f,	0.72f,	0.73f,	0.74f,	0.75f,	0.76f,	0.77f,	0.78f,	0.79f,	0.80f,	
				0.81f,	
				0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,
				0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f,	0.81f
		};
*/
		// edith 2009.12.26 PVP ����
        static const float fPvPAffect[AGGRAVATION_NUM] = {
                0.49f,     0.51f,     0.53f,     0.55f,     0.57f,     0.59f,     0.61f,     0.63f,     0.64f,     0.66f,     
                0.67f,     0.68f,     0.69f,     0.70f,     0.71f,     0.72f,     0.73f,     0.74f,     0.75f,     0.76f,     
                0.77f,     
                0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,
                0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f,     0.77f
        };


/*
		// ����.
		static const float fPvPAffect[AGGRAVATION_NUM] = {
			0.30f,	0.32f,	0.34f,	0.36f,	0.38f,	0.30f,	0.32f,	0.34f,	0.36f,	0.38f,	
			0.40f,	0.41f,	0.42f,	0.43f,	0.44f,	0.45f,	0.46f,	0.47f,	0.48f,	0.49f,	
			0.50f,	
			0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,
			0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f,	0.50f
		};
*/

		fFinalAffect = fPvPAffect[CalculateLevelGap(m_CreatureStatus.m_nLevel, pDefender->GetStatus().m_nLevel) + MAX_LEVEL_GAP];

/*
		// ----------------------------------------------------------------------------------
		// ����Ʈ���ʽ� ���� ����

		CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(this);

		static const float fEliteBonusAffect[EliteBonus::MAX_BONUS_LEVEL * 2 + 1] = {
			0.6f,	0.64f,	0.68f,	0.72f,	0.76f,	0.8f,	0.84f,	0.88f,	0.92f,	0.96f,
			1.0f,
			1.08f,	1.16f,	1.24f,	1.32f,	1.4f,	1.48f,	1.56f,	1.64f,	1.72f,	1.8f
		};

		fFinalAffect = fEliteBonusAffect[lpCharacter->GetEliteBonus() + EliteBonus::MAX_BONUS_LEVEL];
*/

/*
		// ----------------------------------------------------------------------------------
		// ��� ȿ�� ���� ����

		CCharacter* lpOffencer = reinterpret_cast<CCharacter *>(this);
		CCharacter* lpDefender = reinterpret_cast<CCharacter *>(pDefender);

		static const float fCasteAffect[MAX_CASTE + 1] = {
			1.0f,	0.96f,	0.92f,	0.88f,	0.84f,	0.8f,	0.76f,	0.72f,	0.68f,	0.64f,	0.6f
		};

		char cCasteGap = lpDefender->GetCaste() - lpOffencer->GetCaste();
		cCasteGap = max(cCasteGap, char(0));
		cCasteGap = min(cCasteGap, char(MAX_CASTE));
		fFinalAffect = fCasteAffect[cCasteGap];
*/
	}

	return fFinalAffect;
}

//hz added for safe zone attack fix
bool CAggresiveCreature::IsSafeZones( CAggresiveCreature* lpSkillUser , CAggresiveCreature* lpVictim )
{
	CCharacter* lpMe = reinterpret_cast<CCharacter*>(lpSkillUser);
	CCharacter* lpHim = reinterpret_cast<CCharacter*>(lpVictim);
	Creature::CreatureType eCreatureType2 = Creature::GetCreatureType(lpHim->GetCID());
	Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpMe->GetCID());
	
	if(lpMe->GetNation() == lpHim->GetNation())
		return false;

	if (Creature::CT_MONSTER == eCreatureType2 )//if skilluser is a monster.
		return false;

	if (Creature::CT_MONSTER == eCreatureType )//if victim is a monster.
		return false;
	
					if (Creature::CT_MONSTER != eCreatureType &&
						Creature::CT_SUMMON != eCreatureType &&
						Creature::CT_STRUCT != eCreatureType)
						{
						if((lpMe->GetDuelOpponent() == NULL))//if im not in duel.
						{
							Position currPos = lpMe->GetCurrentPos();
	
							if (SERVER_ID::ZONE1 == CServerSetup::GetInstance().GetServerZone() || 
								SERVER_ID::ZONE2 == CServerSetup::GetInstance().GetServerZone())
							{
								return true;
							}
/*
							if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
							{
								if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189)))
									return true;
							}
							Position currPos2 = lpHim->GetCurrentPos();
							if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
							{
								if(((currPos2.m_fPointX >= 1824)&&(currPos2.m_fPointX <= 2250))&&((currPos2.m_fPointZ >= 2817)&&(currPos2.m_fPointZ <= 3189)))
									return true;
							}
*/
						}
						else if(lpMe->GetDuelOpponent() != lpVictim )//duel not against him
							return true;
					}
	return false;//duel against me
}
//hz end

bool CAggresiveCreature::MultiAttack(AtType attackType, unsigned char nDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, 
									 Position CenterPos, float fDir, float nRange, float fAngle, char cTargetType)
{
	if (NULL == ppDefenders[0])
	{
		return false;
	}

	// �ڱ� �߽����� ���� ���� ����
	if (this == ppDefenders[0])
	{
		nDefenderNum = 0;
		ppDefenders[0] = NULL;
	}

	CCell* lpCell = NULL;

	// ��� ���¶�� ��� ���� ó��
	if (CServerSetup::GetInstance().GetDuelModeCheck() && NULL != GetDuelOpponent())
	{
		lpCell = CDuelCellManager::GetInstance().GetCell(GetCID());
	}
	else
	{
		lpCell = CCellManager::GetInstance().GetCell(m_CellPos.m_wMapIndex, 
			static_cast<unsigned long>(CenterPos.m_fPointX), 
            static_cast<unsigned long>(CenterPos.m_fPointY), 
			static_cast<unsigned long>(CenterPos.m_fPointZ));
    }

    if (NULL == lpCell)
    {
        ERRLOG0(g_Log, "CID:0x%08x ���� �߽� ĳ���Ͱ� ���� �������� �ʽ��ϴ�.");
        return false; 
    }

	unsigned short wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM] = {0, };

    for (int nDirection = 0; nDirection < CCell::CONNECT_NUM && nDefenderNum < AtNode::MAX_DEFENDER_NUM; ++nDirection)
    {
        CCell* lpConnectCell = lpCell->GetConnectCell(nDirection);
        if (NULL == lpConnectCell) 
        {
            continue;
        }

        CAggresiveCreature* lpTempTarget = lpConnectCell->GetFirstAggresiveCreature();
        // Ÿ���� ���ų�, ����� ���� �ִ밡 �Ǹ�, ������ ���������ϴ�.
        while (NULL != lpTempTarget && nDefenderNum < AtNode::MAX_DEFENDER_NUM)
        {
            // ���ݿ� ���� ���ܻ�Ȳ
            bool bException = false;

			// ���⿡ ž���� ĳ���ʹ� �������� �ʴ´�. ��� ���⸦ �����Ѵ�.
			if (Creature::CT_PC == Creature::GetCreatureType(lpTempTarget->GetCID()))
			{
				CCharacter* lpRideChar = reinterpret_cast<CCharacter*>(lpTempTarget);
				if (true == lpRideChar->IsRideArms())
				{
					bException = true;
				}
			}

			EnemyCheck::EnemyType eTargetType = IsEnemy(lpTempTarget);
			if ((EnemyCheck::EC_NEUTRAL == eTargetType) ||
				(Skill::Target::FRIEND == cTargetType && EnemyCheck::EC_ENEMY == eTargetType) || 
				(Skill::Target::ENEMY == cTargetType && EnemyCheck::EC_FRIEND == eTargetType))
			{
				bException = true;
			}

			// �Ʊ��� ������� �� ��ų�� ���� ������Ʈ, ���󿡰Դ� ����Ҽ� ������ ó��
			if (Skill::Target::FRIEND == cTargetType)
			{
				if (Creature::CT_SIEGE_OBJECT == Creature::GetCreatureType(lpTempTarget->GetCID()))
				{
					bException = true;
				}
				else if (Creature::CT_STRUCT == Creature::GetCreatureType(lpTempTarget->GetCID()))
				{
					CStatue* lpStatue = reinterpret_cast<CMonster*>(lpTempTarget)->DowncastToStatue();
					if (NULL != lpStatue)
					{
						bException = true;
					}
				}
			}

			// ���� �����̸� ó������ �ʴ´�.
			if (true == lpTempTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Invincible))
			{
				bException = true;
			}

            // ��ġ�� �� ������ ó������ �ʴ´�.
            for (int nIndex = 0; nIndex < nDefenderNum; nIndex++)
            {
                if (ppDefenders[nIndex] == lpTempTarget)
                {
                    bException = true;
                    break;
                }
            }

			if (false == bException)
            {
                const float fDX = lpTempTarget->GetCurrentPos().m_fPointX - CenterPos.m_fPointX;
                const float fDZ = lpTempTarget->GetCurrentPos().m_fPointZ - CenterPos.m_fPointZ;				
                const float fDistance = (fDX * fDX) + (fDZ * fDZ);

                const float fSquareAttackRange = nRange * nRange;

                if (fDistance <= fSquareAttackRange)
                {
                    const float fTempfDir = CalcDir2D(CenterPos.m_fPointX, CenterPos.m_fPointZ, 
                        lpTempTarget->GetCurrentPos().m_fPointX, lpTempTarget->GetCurrentPos().m_fPointZ);
                    const float fDifference = (fTempfDir >= fDir) ? (fTempfDir-fDir) : (fDir-fTempfDir);		

                    if (fDifference <= fAngle && 0 < lpTempTarget->GetStatus().m_nNowHP)
                    {
                        ppDefenders[nDefenderNum]       = lpTempTarget;	
                        cDefenserJudges[nDefenderNum]   = ClientConstants::Judge_Front;
						wDefenserMPHeal[nDefenderNum]	= 0;
                        ++nDefenderNum;
                    }
                }
            }

            lpTempTarget = lpConnectCell->GetNextAggresiveCreature();
        }
    }

    if (AtNode::MAX_DEFENDER_NUM < nDefenderNum) 
    {
        SERLOG0(g_Log, "���� ������ : ����ڼ��� �ִ�ġ�� �Ѿ�̽��ϴ�.");
    }

	return Attack(attackType, nDefenderNum, ppDefenders, cDefenserJudges, wDefenserMPHeal);
}


unsigned short CAggresiveCreature::MeleeAttack(CAggresiveCreature* lpTarget, Creature::StatusType eHandType, 
											   const float fLevelGap, unsigned char &cDefenserJudge, unsigned int ExType)
{
	FightStatus resultStatus;

	// A. �޼�/������ ����
	if (Creature::RIGHT_PASSIVE_TYPE == eHandType)
	{
		resultStatus = m_CreatureStatus.m_StatusInfo;
	}
	else
	{
		unsigned short aryEnchantLevel[Skill::SpellID::MAX_SPELL_TYPE] = { 0, };
		GetEnchantLevel(aryEnchantLevel);
		GetEtcTypeStatus(eHandType).CalculateEnchantInfo(aryEnchantLevel, resultStatus);
	}

	// �κ��� ����
	CSpell* lpSpell = m_SpellMgr.GetAffectedInfo().GetSpell(Skill::SpellID::Envenom);
	if (NULL != lpSpell)
	{
		unsigned short wResult = Skill::CAddSpell<CPoisonedSpell>(
			CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, this,
			Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Poisoned, 
			lpSpell->GetSpellLevel(), static_cast<unsigned long>(20 * CalculateLevelGapAffect(lpTarget))))(lpTarget);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}

	// ������ ����
	if (Creature::LEFT_PASSIVE_TYPE == eHandType || 
		Creature::LEFT_NON_PASSIVE_TYPE == eHandType)
	{
		CalculateEquipDurability(AtType::LEFT_MELEE);
	}
	else
	{
		CalculateEquipDurability(AtType::RIGHT_MELEE);
	}

	// ������ ���
	return lpTarget->CalculateDamage(resultStatus, fLevelGap, cDefenserJudge, ExType);
}


unsigned short CAggresiveCreature::ApplyDamage(AtType attackType, CAggresiveCreature* pOffencer, 
											   unsigned char &cOffencerJudge, unsigned char &cDefenserJudge,
											   unsigned short& wOffencerMPHeal, unsigned short& wDefenserMPHeal, 
											   unsigned short &wError)
{	
	PERFORMANCE_CHECK(FunctionTimingCheck)



	unsigned short nDamage = 0;
	const unsigned short nPreviousHP = m_CreatureStatus.m_nNowHP;
	const unsigned short nPreviousMP = m_CreatureStatus.m_nNowMP;

	bool bCounter = true;
	bool bInterrupt = true;
	bool bProtection = true;

	Creature::CreatureType eCreatureType = Creature::GetCreatureType(m_dwCID);

	if (0x4000 < attackType.m_wType && attackType.m_wType < 0x5000)
	{
		bCounter = false;
		bInterrupt = false;
		bProtection = false;
	}
	else if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		const Skill::ProtoType* lpSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (NULL == lpSkillProtoType) 
		{
			ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", 
				pOffencer->GetCID(), attackType.m_wType);
			return 0;
		}

		if (false == lpSkillProtoType[attackType.m_cSkillLockCount].m_bCounter)		{ bCounter = false;		}
		if (false == lpSkillProtoType[attackType.m_cSkillLockCount].m_bInterrupt)	{ bInterrupt = false;	}
		if (false == lpSkillProtoType[attackType.m_cSkillLockCount].m_bProtection)	{ bProtection = false;	}
	}

	// ī���� �ߵ�
	if (true == bCounter)
	{
		if (true == GetEnchantInfo().GetFlag(Skill::SpellID::CounterAttack))
		{
			// ���ʹ� ī���͸� �� �� ����. (����Ʈ�� �������� �ʴ´�.)

			cDefenserJudge = ClientConstants::Judge_Counter;
			return 0;
		}
	}

	// ������ ���� �ı�
	if (true == bInterrupt)
	{
		m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Stun);
	}

	// B. ������, �̺����� ����
	if (true == bProtection)
	{
		CSpell *pSpell = NULL;

		// ������ ������ ���� 5 ����
		pSpell = m_SpellMgr.GetAffectedInfo().GetSpell(Skill::SpellID::ManaShell);
		if (NULL != pSpell)
		{
			if (0 == pSpell->DecreaseSkillLevel(5)) 
			{
				m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::ManaShell);
			}

			m_Threat.AddToThreatList(pOffencer, 1);		

			cDefenserJudge = ClientConstants::Judge_ManaShell;
			return 0;
		}


		// ������ �÷�������Ƽ ���� 5 ����
		pSpell = m_SpellMgr.GetAffectedInfo().GetSpell(Skill::SpellID::Flexibility);
		if (NULL != pSpell)
		{

			if (0 == pSpell->DecreaseSkillLevel(5)) 
			{
				m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Flexibility);
			}

			m_Threat.AddToThreatList(pOffencer, 1);		

			cDefenserJudge = ClientConstants::Judge_Evade;
			return 0;
		}
	}

	if (0x4000 < attackType.m_wType && attackType.m_wType < 0x5000)
	{
		nDamage = Skill::CProcessTable::GetInstance().UseSkill(attackType, pOffencer, this, 
			cOffencerJudge, cDefenserJudge, wOffencerMPHeal, wDefenserMPHeal, wError);
	}//1 hz
	else if (0 != (attackType.m_wType & AtType::SKILL_BIT)) 
	{
		if(0x9E00 < attackType.m_wType && attackType.m_wType <= 0x9EFF)//(0x9E00 < attackType.m_wType && attackType.m_wType <= 0x9EFF)
		{
			;	// �Դ��� ��ų�� �˻縦 �����Ѵ�.
		}
		// ��ų ������ ���� ��ų�� ���� ���θ� ����
		else if (ClientConstants::Judge_SkillItem != cOffencerJudge)
		{
			// ���Ⱑ ���� ������ ��ų�� �ִ��� �˻��ϴ� �κ�
			if (false == pOffencer->HasSkill(attackType.m_wType, attackType.m_cSkillLockCount, attackType.m_cSkillLevel)) 
			{ 
				return 0; 
			}
		}

		// ���⿡�� �̹� ���� �Ȱ��� ��ų�� �ִ��� �˻�??

		// ���Ⱑ ������ ���� ��ų�� ���Ǵ� �κ�.
		nDamage = Skill::CProcessTable::GetInstance().UseSkill(attackType, pOffencer, this, 
			cOffencerJudge, cDefenserJudge, wOffencerMPHeal, wDefenserMPHeal, wError);
	} 
	else 
	{
		if (false == pOffencer->AttackUsingBow(attackType.m_wType)) 
		{
			return 0;
		}

		// edith 2008.05.20 ������� �߰�
//		unsigned long dwRate = Math::Random::ComplexRandom(pOffencer->m_CreatureStatus.m_StatusInfo.m_wHitRate + m_CreatureStatus.m_StatusInfo.m_wEvade + m_CreatureStatus.m_StatusInfo.m_wBlock);

		float evade = m_CreatureStatus.m_StatusInfo.m_wEvade*1.05f;
		float block = m_CreatureStatus.m_StatusInfo.m_wBlock*1.05f;
		unsigned long dwRate = Math::Random::ComplexRandom(pOffencer->m_CreatureStatus.m_StatusInfo.m_wHitRate + (int)(evade + block));

		// �������
		if (dwRate >= static_cast<unsigned short>(pOffencer->m_CreatureStatus.m_StatusInfo.m_wHitRate + m_CreatureStatus.m_StatusInfo.m_wEvade))
		{
			m_Threat.AddToThreatList(pOffencer, 1);	
	
			if (Creature::CT_MONSTER == eCreatureType ||
				Creature::CT_SUMMON == eCreatureType ||
				Creature::CT_STRUCT == eCreatureType)
			{
				CMonster* lpMonster = reinterpret_cast<CMonster *>(this);
				lpMonster->Attacked();
			}

			cDefenserJudge = ClientConstants::Judge_Guard;
			return 0;
		}
		// ȸ������
		if (dwRate >= static_cast<unsigned short>(pOffencer->m_CreatureStatus.m_StatusInfo.m_wHitRate))
		{
			m_Threat.AddToThreatList(pOffencer, 1);
			
			if (Creature::CT_MONSTER == eCreatureType ||
				Creature::CT_SUMMON == eCreatureType ||
				Creature::CT_STRUCT == eCreatureType)
			{
				CMonster* lpMonster = reinterpret_cast<CMonster *>(this);
				lpMonster->Attacked();
			}

			cDefenserJudge = ClientConstants::Judge_Miss;
			return 0;
		}

/*
		// edith 2008.05.19 ����, ��� ���� ����.
		// D. ���� ����
		if (Math::Random::ComplexRandom(pOffencer->m_CreatureStatus.m_StatusInfo.m_wHitRate + m_CreatureStatus.m_StatusInfo.m_wEvade) >= 
			static_cast<unsigned short>(pOffencer->m_CreatureStatus.m_StatusInfo.m_wHitRate))
		{
			m_Threat.AddToThreatList(pOffencer, 1);
			
			if (Creature::CT_MONSTER == eCreatureType ||
				Creature::CT_SUMMON == eCreatureType ||
				Creature::CT_STRUCT == eCreatureType)
			{
				CMonster* lpMonster = reinterpret_cast<CMonster *>(this);
				lpMonster->Attacked();
			}

			cDefenserJudge = ClientConstants::Judge_Miss;
			return 0;
		}

		// E. ��� ����
		if (Math::Random::ComplexRandom(10000) < m_CreatureStatus.m_StatusInfo.m_fBlockRate * 100)
		{
			m_Threat.AddToThreatList(pOffencer, 1);	
	
			if (Creature::CT_MONSTER == eCreatureType ||
				Creature::CT_SUMMON == eCreatureType ||
				Creature::CT_STRUCT == eCreatureType)
			{
				CMonster* lpMonster = reinterpret_cast<CMonster *>(this);
				lpMonster->Attacked();
			}

			cDefenserJudge = ClientConstants::Judge_Guard;
			return 0;
		}
*/
		// A. �޼�/������ ����
		Creature::StatusType eHandType = Creature::RIGHT_PASSIVE_TYPE;
		if (AtType::LEFT_MELEE == attackType.m_wType)
		{
			eHandType = Creature::LEFT_PASSIVE_TYPE;
		}

		nDamage = pOffencer->MeleeAttack(this, eHandType, pOffencer->CalculateLevelGapAffect(this), cDefenserJudge);
	}

	CSpell* pSpell = m_SpellMgr.GetAffectedInfo().GetSpell(Skill::SpellID::Guard);

	if (0 < nDamage && NULL != pSpell)
	{
		if (4 <= pSpell->GetSkillLockCount())
		{
			if (0 != (attackType.m_wType & AtType::SKILL_BIT))
			{
				const Skill::ProtoType* lpSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
				if (NULL == lpSkillProtoType) 
				{
					ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", 
						pOffencer->GetCID(), attackType.m_wType);
					return 0;
				}
				else if (-5 < CalculateFixLevelGap(pOffencer))
				{
					// �ǰ��ڿ��� ���� 5�ܰ� ��ų�̻��� ���� ������ �ߵ��Ǿ� ���� �� ��ų �ǰ� �������� ���� Ȯ���� �ݻ�
					unsigned char cSuccessRate = 6 + (pSpell->GetSkillLevel() * 4);
					if (cSuccessRate > Math::Random::ComplexRandom(100))
					{
						unsigned short nRefrectionDamage = nDamage;

						if (5000 < nRefrectionDamage)
						{
							nRefrectionDamage = 5000;
						}

						unsigned short wNowHP = pOffencer->GetStatus().m_nNowHP;
						const unsigned short wThreatAmount = (wNowHP < nRefrectionDamage) ? wNowHP : nRefrectionDamage;
						pOffencer->GetThreat().AddToThreatList(this, wThreatAmount);
						pOffencer->GetStatus().m_nNowHP = (wNowHP > nRefrectionDamage) ? wNowHP - nRefrectionDamage : 1;

						DefenserNode node;
						node.m_wDamage = nRefrectionDamage;
						node.m_dwCharID = pOffencer->GetCID();
						node.m_sCurrHP = pOffencer->GetStatus().m_nNowHP;
						node.m_sCurrMP = pOffencer->GetStatus().m_nNowMP;
						node.m_wMaxHP = pOffencer->GetStatus().m_StatusInfo.m_nMaxHP;
						node.m_wMaxMP = pOffencer->GetStatus().m_StatusInfo.m_nMaxMP;
						node.m_wMPHeal = 0;
						node.m_cJudge = ClientConstants::Judge_Front;

						AtType attackType;
						attackType.m_wType = AtType::RIGHT_MELEE;

						const int   MAX_BUFFER = sizeof(PktAtAck) + AtNode::MAX_DEFENDER_NUM * sizeof(DefenserNode);
						char        szBuffer[MAX_BUFFER];

						PktAtAck*		lpPktAtAck		= reinterpret_cast<PktAtAck*>(szBuffer);
						DefenserNode*	lpDefenserNode	= reinterpret_cast<DefenserNode*>(lpPktAtAck + 1);
						lpDefenserNode = &node;

						lpPktAtAck->m_dwCharID = m_dwCID;
						lpPktAtAck->m_AtType = attackType;
						lpPktAtAck->m_wHP = m_CreatureStatus.m_nNowHP;
						lpPktAtAck->m_wMP = m_CreatureStatus.m_nNowMP;
						lpPktAtAck->m_wMPHeal = 0;
						lpPktAtAck->m_cJudge = ClientConstants::Judge_Front;
						lpPktAtAck->m_cDefenserNum = 1;

						CGameClientDispatch* lpDispatch = NULL;

						if (Creature::CT_PC == Creature::GetCreatureType(m_dwCID))
						{
							CCharacter* pDeffencerCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
							if (NULL != pDeffencerCharacter)
							{
								lpDispatch = pDeffencerCharacter->GetDispatcher();
							}
						}
						else if (Creature::CT_PC == Creature::GetCreatureType(pOffencer->GetCID()))
						{
							CCharacter* pOffencerCharacter = CCreatureManager::GetInstance().GetCharacter(pOffencer->GetCID());
							if (NULL != pOffencerCharacter)
							{
								lpDispatch = pOffencerCharacter->GetDispatcher();
							}
						}
						else
						{
							CCell* lpCell = pOffencer->GetCellPos().m_lpCell;
							if (NULL != lpCell)
							{
								lpCell->SendAttackInfo(m_dwCID, attackType, 1, &node);
							}
						}

						if (NULL != lpDispatch)
						{
							GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), this, pOffencer,
								attackType, 0, nRefrectionDamage, ClientConstants::Judge_Front, 0,  PktBase::NO_SERVER_ERR);

							CSendStream& SendStream = lpDispatch->GetSendStream();
							if (true == SendStream.WrapCompress(
								szBuffer, sizeof(PktAtAck) + sizeof(DefenserNode), CmdCharAttack, 0, wError) &&
								PktBase::NO_SERVER_ERR == wError)
							{
								CCell* lpCell = pOffencer->GetCellPos().m_lpCell;
								if (NULL != lpCell)
								{
									lpCell->SendAttackInfo(m_dwCID, attackType, 1, &node);
								}
							}
						}

						this->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Guard);
					}
				}
			}
			else
			{
				// �ǰ��ڿ��� ���� 5�ܰ� ��ų�̻��� ���� ������ �ߵ��Ǿ� ���� �� ��Ÿ �ǰ� ������ ����
				float fDecreaseRate = static_cast<unsigned short>(5 + (pSpell->GetSkillLevel() * 2.5f)) / 100.0f;
				nDamage = nDamage - static_cast<unsigned short>(nDamage * fDecreaseRate);
			}

		}
	}

	// Threat �� ���� ó��
	if (nDamage > 0) 
	{ 
		// ������ ���� ���⸦ ���ؼ� ������ ���̰�, ���� �ִ� ĳ���Ϳ��� �������� ����������.
		if (Creature::CT_SIEGE_OBJECT == eCreatureType)
		{
			CSiegeObject* lpSiegeObject = reinterpret_cast<CSiegeObject *>(this);
			if (lpSiegeObject && lpSiegeObject->IsGate())
			{
				reinterpret_cast<CCastleGate*>( lpSiegeObject )->DivideDamage(pOffencer, nDamage);
				reinterpret_cast<CCastleGate*>( lpSiegeObject )->SendProtectGateInfo();
			}
		}

		const unsigned short nThreatAmount = (m_CreatureStatus.m_nNowHP < nDamage) ? m_CreatureStatus.m_nNowHP : nDamage;
		m_Threat.AddToThreatList(pOffencer, nThreatAmount);		

		m_CreatureStatus.m_nNowHP = (m_CreatureStatus.m_nNowHP > nDamage) ? m_CreatureStatus.m_nNowHP - nDamage : 0;

		CMonster* pMonsterDefender = NULL;
		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			pMonsterDefender = reinterpret_cast<CMonster *>(this);
			pMonsterDefender->Attacked();

			// ������ : ��ũ ī���� ���� ����
			//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//hayzohar statue war fix
			//{
				// ��ũ ī���� ������ ���� ���ϸ� HP ������ ������Ʈ �Ѵ�.
				if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
				{
					CStatue* lpStatue = pMonsterDefender->DowncastToStatue();
					if (NULL != lpStatue)
					{
						lpStatue->SendHPUpdate();
						lpStatue->SendAttacked();                    
					}
				}
			//}
/*
			// ������ : �ű���
			if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
			{
				// ��������Ⱑ ���� ���ϸ� HP ������ ������Ʈ �Ѵ�.
				if (CServerSetup::GetInstance().GetServerZone() == SERVER_ID::ZONE9)
				{
					CStatue* lpStatue = pMonsterDefender->DowncastToStatue();
					if (NULL != lpStatue)
					{
						lpStatue->SendHPUpdate();
						lpStatue->SendAttacked();                    
					}
				}
			}
*/
		}

		// ���� ������Ʈ��� HP ������ DB�߰� ������ ������Ʈ�Ѵ�.
		if (Creature::CT_SIEGE_OBJECT == eCreatureType)
		{
			CSiegeObject* lpSiegeObject = reinterpret_cast<CSiegeObject *>(this);
			if (lpSiegeObject)
			{
				lpSiegeObject->SendHPUpdateToDBAgent();

				if (lpSiegeObject->IsCamp())
				{
					// ���� ���ϰ� �ִٴ� �޼����� �����ش�.
					reinterpret_cast<CCamp*>(lpSiegeObject)->SendAttackedMessage();
				}
				else if (lpSiegeObject->IsEmblem())
				{
					// ���� ���ϰ� �ִٴ� �޼����� �����ش�.
					reinterpret_cast<CCastleEmblem*>(lpSiegeObject)->SendAttackedMessage();
				}
			}
		}

		// edith 2008.07.11 ������ ������ �˻����� �ʴ´�. ����.. �������� ������ Ǯ�ȴ�.. 
		// 1���� 100�� �������� ������ ������ �����ϴ�. ������ ������ ���ǹ� �ϴ�.
/*
		if (0 == (attackType.m_wType & AtType::SKILL_BIT))
		{
			if (Creature::CT_PC == Creature::GetCreatureType(pOffencer->GetCID()))
			{
				CCharacter* pCharacter = (CCharacter *)pOffencer;
				if (NULL != pMonsterDefender) 
				{
					if (false == GAMELOG::LogDamagePumping(*pCharacter, *pMonsterDefender, nDamage))
					{
						ERRLOG1(g_Log, "CID:0x%08x ������ ������ �߻��Ͽ����ϴ�.", pOffencer->GetCID());
					}
				}
			}
		}
*/
	} 

	// Ư�� ���̽��� ó��
	switch (cDefenserJudge)
	{
		case ClientConstants::Judge_Heal:		return m_CreatureStatus.m_nNowHP - nPreviousHP;
		case ClientConstants::Judge_ManaHeal:	return m_CreatureStatus.m_nNowMP - nPreviousMP;

		case ClientConstants::Judge_Guard:	
			if(pOffencer)
			m_Threat.AddToThreatList(pOffencer, 1);			
			break;
	}

	// �׾��� ��� (�ڻ� ��ų�� ��ȿ)
	// �̹� �׾��ִ� ��쿡 �������� ��ų�� �޾������� Dead() �Լ��� �ٽ� ȣ������ �ʴ´�.
	if (0 != nPreviousHP && 0 == m_CreatureStatus.m_nNowHP && pOffencer != this)
	{
		if(pOffencer)
		Dead(pOffencer);
		m_Threat.ClearAll();		
	} 

	return nDamage;
}


unsigned short CAggresiveCreature::CalculateDamage(const FightStatus& OffencerStatusInfo, 
												   const float fLevelGap, unsigned char &cDefenserJudge, unsigned int ExType) 
{
	// C. ������ ����
	long lDamage = static_cast<long>(Math::Random::ComplexRandom(OffencerStatusInfo.m_lMaxDamage, OffencerStatusInfo.m_lMinDamage) * 
		(100 - m_CreatureStatus.m_StatusInfo.m_fDefenceRate) / 100.0f);

	// G. ũ��Ƽ�� ����
	float fCriticalRate = OffencerStatusInfo.m_fCriticalRate;
	
	// edith 2009.12.26 PVP�϶� ���ӵ弦, ����Ʈ�� ��� 1/5 ũ�� Ȯ���� ����
	if(ExType != 0)
		fCriticalRate = fCriticalRate/ExType;

	if (Math::Random::ComplexRandom(10000) < fCriticalRate * 100)
	{
		lDamage = static_cast<long>(lDamage * OffencerStatusInfo.m_fCriticalMagnification);
		cDefenserJudge = ClientConstants::Judge_Critical;
	}

	// H. ������, ����Ʈ ���ʽ� ȿ��
	lDamage = static_cast<long>(lDamage * fLevelGap);

	lDamage = max(lDamage, long(0));
	lDamage = min(lDamage, USHRT_MAX);

	return static_cast<unsigned short>(lDamage);
}


float CAggresiveCreature::CalcDir2D(const float fSrcX, const float fSrcY, const float fDstX, const float fDstY)
{
	float fResult;
	const float fDX = fDstX - fSrcX;
	const float fDY = fDstY - fSrcY;

	if (!fDX && !fDY) {
		return 0.0f;
	}

	if (!fDX) {
		return (fDY > 0.0f) ? Math::Const::PI * 0.5f : Math::Const::PI * 1.5f;
	}

	if (!fDY) {
		return (fDX > 0.0f) ? 0.0f : Math::Const::PI;
	}

	const float ufDX = (fDX > 0.0f) ? fDX : fDX * -1.0f;
	const float ufDY = (fDY > 0.0f) ? fDY : fDY * -1.0f;

	fResult = atanf(ufDY / ufDX);

	if (fDX < 0.0f && fDY >= 0.0f) {
		fResult = Math::Const::PI - fResult;
	}
	else if (fDX < 0.0f && fDY < 0.0f) {
		fResult += Math::Const::PI;
	}
	else if (fDX >= 0.0f && fDY < 0.0f) {
		fResult = (Math::Const::PI * 2) - fResult;
	}

	return fResult;
}

bool CAggresiveCreature::RegenHPAndMP(unsigned short usAdditionalHP, unsigned short usAdditionalMP, bool bAddDefaultRegenValue)
{
	if (0 == m_CreatureStatus.m_nNowHP) { return false; }

	short usHPRegen = usAdditionalHP;
	short usMPRegen = usAdditionalMP;

	if (true == bAddDefaultRegenValue) 
	{ 
		usHPRegen += m_CreatureStatus.m_StatusInfo.m_wHPRegen;
		usMPRegen += m_CreatureStatus.m_StatusInfo.m_wMPRegen;

		if (true == m_bSitDown)
		{
			// edith 2009.08.07 �ɾ������� �������� ���δ�.
			usHPRegen += (m_CreatureStatus.m_StatusInfo.m_wHPRegen*2);//hz rest hp&mp while sit was 2
			usMPRegen += (m_CreatureStatus.m_StatusInfo.m_wMPRegen*2);

//			usHPRegen += m_CreatureStatus.m_StatusInfo.m_nMaxHP / 10;
//			usMPRegen += m_CreatureStatus.m_StatusInfo.m_nMaxMP / 10;
		}
	}
    
	m_CreatureStatus.m_nNowHP = (m_CreatureStatus.m_nNowHP + usHPRegen < m_CreatureStatus.m_StatusInfo.m_nMaxHP) ? 
        m_CreatureStatus.m_nNowHP + usHPRegen : m_CreatureStatus.m_StatusInfo.m_nMaxHP;

	m_CreatureStatus.m_nNowMP = (m_CreatureStatus.m_nNowMP + usMPRegen < m_CreatureStatus.m_StatusInfo.m_nMaxMP) ?
        m_CreatureStatus.m_nNowMP + usMPRegen : m_CreatureStatus.m_StatusInfo.m_nMaxMP;

    return true;
}

bool CAggresiveCreature::GuardAura(unsigned short usAdditionalBlock, unsigned short usAdditionalDefense, bool bAddDefaultRegenValue)
{
	if (0 == m_CreatureStatus.m_nNowHP) { return false; }

	short usBlockRegen = usAdditionalBlock;
	short usDefRegen = usAdditionalDefense;
	
	if (true == bAddDefaultRegenValue) 
	{ 
		usBlockRegen += m_CreatureStatus.m_StatusInfo.m_fBlockRate;
		usDefRegen += m_CreatureStatus.m_StatusInfo.m_fDefenceRate;
	}

    m_CreatureStatus.m_StatusInfo.m_fBlockRate += usBlockRegen;
	m_CreatureStatus.m_StatusInfo.m_fDefenceRate += usDefRegen;
    return true;
}