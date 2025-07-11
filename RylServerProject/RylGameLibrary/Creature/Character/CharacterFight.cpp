#include "stdafx.h"

#include <Utility/Math/Math.h>
#include <Utility/Setup/ServerSetup.h>

#include <Log/LogStruct.h>
#include <Log/CharacterLog.h>

//hz added for pvp drop
#include <Item/Item.h>
#include <Item/ItemFactory.h>
//hz end

//hz added this for killstreak spell
#include <Skill/SkillMgr.h>
#include <Skill/Spell/SpellMgr.h>
//hz end
#include <Item/Item.h>
#include <Item/Container/ItemContainer.h>
#include <Item/Container/EquipmentsContainer.h>

#include <Network/ClientSocket/ClientConstants.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/GlobalSpellMgr.h>

#include <Creature/CreatureManager.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/PatternMonster.h>
#include <Creature/Monster/VirtualMonsterMgr.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/Camp.h>
#include <Creature/Siege/CastleGate.h>
#include <Creature/Siege/SiegeObjectMgr.h>

#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>
#include <Map/FieldMap/VirtualArea/BGServer/BGServerMap.h>
#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/Cell.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>
#include <Community/Party/Party.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include "ExpTable.h"
#include "Character.h"
#include "CharacterCreate.h"
#include "CharRespawnMgr.h"
#include <Creature/EnemyCheck.h>



const int CCharacter::CalculateFixLevelGap(CAggresiveCreature *pDefender)
{
	Creature::CreatureType eCreatureType = Creature::GetCreatureType(pDefender->GetCID());
	if (Creature::CT_MONSTER == eCreatureType ||
		Creature::CT_SUMMON == eCreatureType ||
		Creature::CT_STRUCT == eCreatureType)
	{
		CMonster* lpMonster = reinterpret_cast<CMonster *>(pDefender);
		return (lpMonster->CalculateFixLevelGap(this) * -1);
	}

	return CAggresiveCreature::CalculateLevelGap(pDefender);
}

void CCharacter::Casting(AtType attackType, AtNode& attackNode)
{
	// ĳ���ý� Ǯ���� ��æƮ��
	m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Invincible);
	m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::CounterAttack);
}

bool CCharacter::AttackCID(AtType attackType, AtNode& attackNode, unsigned short& wError)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	if (true == m_bLogout || m_CreatureStatus.m_nNowHP == 0) 
	{
		wError = PktAtAck::FAIL_ALREADY_DEAD;
		return true;
	}

	unsigned char cDefenderNum = attackNode.m_wDefenserNum > AtNode::MAX_DEFENDER_NUM ?
		AtNode::MAX_DEFENDER_NUM : attackNode.m_wDefenserNum; // �ִ� ����� �� ����

	CAggresiveCreature*	lpAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };
	unsigned short wDefenderMPHeal[AtNode::MAX_DEFENDER_NUM] = {0, };
	char cTargetType = Skill::Target::ENEMY;
	Skill::Target::Type eSkillTargetType = Skill::Target::ENEMY;

	// ĳ���ÿ� ������ ��ų�� ��� cDefenderNum �� 0 ���� �Ѿ�´�.
	if (0 == cDefenderNum)
	{
		return Attack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, wDefenderMPHeal);
	}

	// �Ҽȿɼ��̸�
	if (0x4000 < attackType.m_wType && attackType.m_wType < 0x5000)
	{
		const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (0 == pThisSkill) 
		{ 
			ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
			return false;
		}

		const unsigned short wLockCount = GetSkillLockCount(attackType.m_wType);
		if (wLockCount < 0 || wLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT)
		{
			ERRLOG3(g_Log, "CID:0x%08x ������ ��ų�� ��ī��Ʈ�� �̻��մϴ�. SkillType : 0x%04x, LockCount : %d", 
				m_dwCID, attackType.m_wType, wLockCount);
			return false;
		}

		eSkillTargetType = pThisSkill[attackType.m_cSkillLockCount].m_eTargetType;
		cTargetType = Skill::Target::FRIEND;
	}
	else if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (0 == pThisSkill) 
		{ 
			ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
			return false;
		}

		unsigned short wLockCount = GetSkillLockCount(attackType.m_wType);

		// �Դ� ��ų�� ��ų�� ��ϵ��� �ʾƵ� ��밡���ϴ�.
		if(0x9E00 < attackType.m_wType && attackType.m_wType <= 0x9EFF)//(0x9E00 < attackType.m_wType && attackType.m_wType <= 0x9EFF)
		{
			wLockCount = attackType.m_cSkillLockCount;
		}

		if (wLockCount < 0 || wLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT)
		{
			ERRLOG3(g_Log, "CID:0x%08x ������ ��ų�� ��ī��Ʈ�� �̻��մϴ�. SkillType : 0x%04x, LockCount : %d", 
				m_dwCID, attackType.m_wType, wLockCount);
			return false;
		}

		if (0 == pThisSkill[wLockCount].m_fMaxRange && 0 == pThisSkill[attackType.m_cSkillLockCount].m_fEffectExtent) 
		{
			if (m_dwCID != attackNode.m_dwDefenser[0])
			{
				ERRLOG2(g_Log, "CID:0x%08x �ڱ��ڽſ��Ը� �� �� �ִ� ��ų�Դϴ�. SkillID:0x%04x", 
					m_dwCID, attackType.m_wType);
				return false;
			}
		}

		// ��ų �Ÿ� üũ
		CAggresiveCreature* lpTargetCreature = 0;

		// Target Creature ������
		if (0 != GetMapIndex())
		{
			// Ÿ���� �����ΰ�?
			Creature::CreatureType eCreatureType = Creature::GetCreatureType(attackNode.m_dwDefenser[0]);

			if (Creature::CT_MONSTER == eCreatureType ||
				Creature::CT_SUMMON == eCreatureType ||
				Creature::CT_STRUCT == eCreatureType)
			{
				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
				if (0 != lpVirtualArea)
				{
					CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
					if (0 != lpVirtualMonsterMgr)
					{
						lpTargetCreature = lpVirtualMonsterMgr->GetAggresiveCreature(attackNode.m_dwDefenser[0]);
					}
				}
			}
			else
			{
				lpTargetCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[0]);
				if (0 != lpTargetCreature)
				{
					if (lpTargetCreature->GetMapIndex() != GetMapIndex()) 
					{
						lpTargetCreature = 0;
					}
				}
			}
		}
		else
		{
			lpTargetCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[0]);
		}

		// Target Creature ó���ϱ�
		if (0 != lpTargetCreature)
		{
			float fSquareTargetDistance = (m_CurrentPos.m_fPointX - lpTargetCreature->GetCurrentPos().m_fPointX) * 
				(m_CurrentPos.m_fPointX - lpTargetCreature->GetCurrentPos().m_fPointX) + 
				(m_CurrentPos.m_fPointZ - lpTargetCreature->GetCurrentPos().m_fPointZ) * 
				(m_CurrentPos.m_fPointZ - lpTargetCreature->GetCurrentPos().m_fPointZ);

			float fSquareEffectDistance = (pThisSkill[wLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
				(pThisSkill[wLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

			if (fSquareTargetDistance > fSquareEffectDistance)
			{
				wError = PktAtAck::FAIL_TOO_FAR;
				return false;
			}
		}

		if (pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::FRIEND || 
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::DEAD_FRIEND ||
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::FRIEND_EXCEPT_SELF ||
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::FRIEND_OBJECT ||
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::PARTY || 
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::SUMMON)
		{
			eSkillTargetType = pThisSkill[attackType.m_cSkillLockCount].m_eTargetType;
			cTargetType = Skill::Target::FRIEND;
		}
	}

	// Ŭ���̾�Ʈ�� �Ѱ��� Ÿ�ٵ��� üũ�Ѵ�. (���� ������ �ɸ��� Ÿ���� ���� üũ)
	for (unsigned char cDefender = 0; cDefender < cDefenderNum; ++cDefender) 
	{
		// Target Creature ���
		CAggresiveCreature* lpCreature = 0;
		if (0 != GetMapIndex())
		{
			Creature::CreatureType eCreatureType = Creature::GetCreatureType(attackNode.m_dwDefenser[cDefender]);

			if (Creature::CT_MONSTER == eCreatureType ||
				Creature::CT_SUMMON == eCreatureType ||
				Creature::CT_STRUCT == eCreatureType)
			{
				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
				if (0 != lpVirtualArea)
				{
					CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
					if (0 != lpVirtualMonsterMgr)
					{
						lpCreature = lpVirtualMonsterMgr->GetAggresiveCreature(attackNode.m_dwDefenser[cDefender]);
					}
				}
			}
			else
			{
				lpCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[cDefender]);
				if (lpCreature && lpCreature->GetMapIndex() != GetMapIndex())	
				{
					lpCreature = 0;
				}
			}
		}
		else
		{
			lpCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[cDefender]);
		}

		if (0 != lpCreature)
		{
			// �������� ����(-_-)
			if (Skill::Target::FRIEND == cTargetType)
			{
				if (Creature::CT_PC == Creature::GetCreatureType(lpCreature->GetCID()))
				{
					CCharacter *lpCharacter = reinterpret_cast<CCharacter *>(lpCreature);

					if (true == lpCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::Hide)) 
					{ 
						continue; 
					}

					if (false == lpCharacter->IsPeaceMode() && true == IsPeaceMode())
					{
						wError = PktAtAck::FAIL_PEACE_TO_WAR;
						return false;
					}

					if (lpCharacter->GetCellPos().m_lpCell)
					{
						if (lpCharacter->GetCellPos().m_lpCell->IsDetectionCell() &&
							Skill::IsStealthSkill(attackType.m_wType))
						{
							wError = PktAtAck::FAIL_NOT_STEALTH;
							return false;
						}
					}
				}
				else if (Creature::CT_SIEGE_OBJECT == Creature::GetCreatureType(lpCreature->GetCID()))
				{
					wError = PktAtAck::FAIL_TO_SIEGE_OBJECT;
					return false;
				}
				else
				{
					CMonster *lpMonster = reinterpret_cast<CMonster *>(lpCreature);
					if (lpMonster->GetPattern() == MonsterInfo::PATTERN_STRUCTURE) { continue; }
				}

				EnemyCheck::EnemyType eEnemyType = IsEnemy(lpCreature);
				switch (eSkillTargetType)
				{
				case Skill::Target::FRIEND:
				case Skill::Target::FRIEND_EXCEPT_SELF:
				case Skill::Target::DEAD_FRIEND:
					{
						if (EnemyCheck::EC_FRIEND == eEnemyType)
						{
							lpAggresiveCreature[cDefender] = lpCreature;
						}
					}
					break;

				case Skill::Target::PARTY:
					{
						if (EnemyCheck::EC_FRIEND == eEnemyType)
						{
							if (NULL != GetParty() &&
								NULL != lpCreature->GetParty() &&
								GetParty()->GetUID() == lpCreature->GetParty()->GetUID())
							{
								lpAggresiveCreature[cDefender] = lpCreature;
							}
							else if (GetGID() == lpCreature->GetGID())
							{
								if (NULL == GetParty() ||
									(NULL != GetParty() && NULL == reinterpret_cast<CCharacterParty* >(GetParty())->GetHostileParty()))
								{
									// Vincent : ��尣 �� ��Ʋ ���϶����� üũ�ؾ��Ѵ�.
									// if ( ��尣 �� ��Ʋ �� )
									lpAggresiveCreature[cDefender] = lpCreature;
								}
							}
						}
					}
					break;
				}
			}
			// �������� ����(��¥ ����)
			else
			{
				// �ڱ⸦ �߽����� �ϴ� ������ ��ų�� ��� Ÿ���� �ڽ����� �����մϴ�.
				// (�� ��� this�� MultiAttack() �Լ��� Ÿ�Ͽ��� ���ܽ��� �ݴϴ�.)
				if (EnemyCheck::EC_ENEMY == IsEnemy(lpCreature) || lpCreature == this)
				{
					lpAggresiveCreature[cDefender] = lpCreature;
				}
			}
		}
	}

	if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		if (0 == cDefenderNum || 0 == lpAggresiveCreature[0]) 
		{
			return Attack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, wDefenderMPHeal);
		}

		const Skill::ProtoType* pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (0 == pSkillProtoType) 
		{
			ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
			return false;
		}

		//hz added this for blast aoe Gunner
		if(attackType.m_wType == 0x9506)
		{
		return MultiAttack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, 
					lpAggresiveCreature[0]->GetCurrentPos(), 0, 6.0f,
					Math::Const::PI * 2, cTargetType);
		}
		//hz end

		//avalansa added this for AimedShot aoe Archer
		if(attackType.m_wType == 0x8803)
		{
		return MultiAttack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, 
					lpAggresiveCreature[0]->GetCurrentPos(), 0, 6.0f,
					Math::Const::PI * 2, cTargetType);
		}
		//avalansa end

		// ���� ���� üũ
		if (0 != pSkillProtoType[attackType.m_cSkillLockCount].m_fEffectExtent)
		{
			if (Skill::Target::PARTY == pSkillProtoType[attackType.m_cSkillLockCount].m_eTargetType)
			{
				CParty* lpParty = lpAggresiveCreature[0]->GetParty();
				if (0 != lpParty)
				{
					return lpParty->Attack(attackType, lpAggresiveCreature, attackNode.m_cDefenserJudge,
						this, pSkillProtoType[attackType.m_cSkillLockCount].m_fEffectExtent, cTargetType);
				}
			}
			else
			{
				return MultiAttack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, 
					lpAggresiveCreature[0]->GetCurrentPos(), 0, pSkillProtoType[attackType.m_cSkillLockCount].m_fEffectExtent,
					Math::Const::PI * 2, cTargetType);
			}
		}
	}
		return Attack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, wDefenderMPHeal);//this one
}


bool CCharacter::Attack(AtType attackType, unsigned char cDefenderNum,
	CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenderMPHeal)
{	
	if (0 == (attackType.m_wType & AtType::SKILL_BIT) && 0 == cDefenderNum) 
	{
		ERRLOG0(g_Log, "��ų�� �ƴ� �Ϲ� ������, �ݵ�� Ÿ���� ���� ��쿡�� ������ ������ �մϴ�.");
		return false;
	}

	bool bBrokenCounter = true;
	bool bBrokenStealth = true;
	bool bCheckDeadTarget = true;

	// ��ų ������ ���� ó��
	if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		const Skill::ProtoType* pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (0 == pSkillProtoType) 
		{
			ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
			return false;
		}

		// æƮ ���ÿ��� ī���� ���ð� ���ڽ��� Ǯ���� �ʴ´�.
		if (Skill::Type::CHANT == pSkillProtoType->m_eSkillType)
		{
			bBrokenCounter = false;
			bBrokenStealth = false;
		}
		else
		{
			// ī���� ������ Ǯ���� �ʴ� ���� (ī���� ����)
			if (0x8605 == attackType.m_wType)
			{
				bBrokenCounter = false;
			}

			// ���������� ���ڽ��� Ǯ���� �ʴ� ����(���ڽ�, ĳ���ö���, �ϵ齺��, ������̾�, �� ��)
			if (0x8704 == attackType.m_wType || 0x8805 == attackType.m_wType || 
				0x9504 == attackType.m_wType || 0x9804 == attackType.m_wType || 
				0x8205 == attackType.m_wType || 0x9805 == attackType.m_wType ||	
				0x8707 == attackType.m_wType)
			{
				bBrokenStealth = false;
			}

			// Ȯ�������� ���ڽ��� Ǯ���� �ʴ� ����(�麻, ���� ����Ʈ)
			//if (0x8702 == attackType.m_wType)
			//{
			//	CAggresiveCreature* lpMySelf = reinterpret_cast<CAggresiveCreature*>(CCreatureManager::GetInstance().GetCreature(m_dwCID));
			//	if (4 == lpMySelf->GetSkillLockCount(0x8704))
			//	{
			//		unsigned char cSuccessRate = static_cast<unsigned char>(20 + (attackType.m_cSkillLevel * 5));
			//		if (cSuccessRate > Math::Random::ComplexRandom(100))
			//		{
			//			bBrokenStealth = false;
			//		}
			//	}
			//}

			//if (0x9802 == attackType.m_wType)
			//{
			//	CAggresiveCreature* lpMySelf = reinterpret_cast<CAggresiveCreature*>(CCreatureManager::GetInstance().GetCreature(m_dwCID));
			//	if (4 == lpMySelf->GetSkillLockCount(0x9804))
			//	{
			//		unsigned char cSuccessRate = static_cast<unsigned char>(20 + (attackType.m_cSkillLevel * 5));
			//		if (cSuccessRate > Math::Random::ComplexRandom(100))
			//		{
			//			bBrokenStealth = false;
			//		}
			//	}
			//}
			if (0x8702 == attackType.m_wType || 0x9802 == attackType.m_wType)
			{
				CSpell* pSpell = m_SpellMgr.GetAffectedInfo().GetSpell(Skill::SpellID::Stealth);
				if (NULL != pSpell)
				{
					if (4 <= pSpell->GetSkillLockCount())
					{
						unsigned char cSuccessRate = 20 + (pSpell->GetSkillLevel() * 5);
						if (cSuccessRate > Math::Random::ComplexRandom(100))
						{
							bBrokenStealth = false;
						}
					}
				}
			}
		}

		// ���� Ÿ�ٿ��� �� �� �ִ� ��ų
		if (Skill::Target::DEAD_FRIEND == pSkillProtoType->m_eTargetType || 
			Skill::Target::DEAD_ENEMY == pSkillProtoType->m_eTargetType)
		{
			bCheckDeadTarget = false;
		}
	}

	unsigned char cOffencerJudge = 0;
	unsigned short wOffencerMPHeal = 0;
	unsigned short wOffencerPrevHP = GetStatus().m_nNowHP;
	unsigned short wOffencerPrevMP = GetStatus().m_nNowMP;
	unsigned short wError = PktAtAck::NO_SERVER_ERR;

	const int   MAX_BUFFER = sizeof(PktAtAck) + AtNode::MAX_DEFENDER_NUM * sizeof(DefenserNode);
	char        szBuffer[MAX_BUFFER];

	PktAtAck*		lpPktAtAck		= reinterpret_cast<PktAtAck*>(szBuffer);
	DefenserNode*	lpDefenserNode	= reinterpret_cast<DefenserNode*>(lpPktAtAck + 1);                

	m_cConsumeMPCount = std::min(cDefenderNum, unsigned char(AtNode::MAX_MONSTER_DEFENDER_NUM));

	unsigned char nMonsterDefenderNum = 0;
	unsigned char cDefender = 0;
	unsigned char cIndex = 0;
	/*//hz added for safe zone multi attack bug:
		if(cDefenderNum != 0)
			if (true == CCellManager::GetInstance().IsSafetyZone(ppDefenders[0]->GetCurrentPos()))
				cDefenderNum=1;
				
		//hz end*/
		
	for (; cIndex < cDefenderNum; ++cIndex) 
	{
		// MP �Ҹ� Ÿ�ֱ̹����� ī��Ʈ (���� ������ �� ���� MP �Ҹ�)
		--m_cConsumeMPCount;

		if (0 == ppDefenders[cIndex])
		{ 
			continue; 
		}
		if (true == bCheckDeadTarget && 0 == ppDefenders[cIndex]->GetStatus().m_nNowHP)
		{ 
			continue; 
		}

		// �ִ� ����� �� ���� (���ʹ� ĳ���Ϳʹ� ���� ó��)
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(ppDefenders[cIndex]->GetCID()); 

		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			//hz added this for no gather attack gather:
			bool found=false;
			bool Gather=false;
			if((attackType.m_wType == 0x9E04)||(attackType.m_wType == 0x9E03))
				Gather=true;

			if(!Gather)
			{
				unsigned long dwMonID = ppDefenders[cIndex]->GetCID() & 0x0000FFFF;

				const unsigned short usMonID[13] = { 3100, 3150, 3200, 3225, 3250, 
														3275, 3300, 3325, 3350, 3375, 
														3400, 3425, 3450 };
				for(int i=0;i<13;i++)
				{
					if(usMonID[i]==dwMonID)
					{
						found=true;
						break;
					}
				}
				if(found)
					continue;
			}
			nMonsterDefenderNum++;
			if (nMonsterDefenderNum > AtNode::MAX_MONSTER_DEFENDER_NUM)
			{
				continue;
			}
		}

		// ĳ���� �Ϲ� ���ݽ� �Ÿ� üũ
		if (0 == (attackType.m_wType & AtType::SKILL_BIT))
		{
			const Position& targetPos = ppDefenders[cIndex]->GetCurrentPos();

			float fSquareTargetDistance = 
				(m_CurrentPos.m_fPointX - targetPos.m_fPointX) * (m_CurrentPos.m_fPointX - targetPos.m_fPointX) + 
				(m_CurrentPos.m_fPointZ - targetPos.m_fPointZ) * (m_CurrentPos.m_fPointZ - targetPos.m_fPointZ);

			// ���� ����
			if (AtType::RIGHT_MELEE == attackType.m_wType || AtType::LEFT_MELEE == attackType.m_wType)
			{
				if (fSquareTargetDistance > MELEE_ATTACK_RANGE * MELEE_ATTACK_RANGE)
				{
					// edith 2008.07.11 �������� �Ÿ� ���� �α� ����.
					//					ERRLOG2(g_Log, "CID:%10u ���� �������� �ʹ� �ָ� �ִ� Ÿ���� �����Ϸ� �մϴ�. �Ÿ�:%.2f(m)", 
					//						m_dwCID, fSquareTargetDistance);
					continue;
				}
			}
			// ���Ÿ� ����
			else if (AtType::BOW == attackType.m_wType || AtType::CROSSBOW == attackType.m_wType)
			{
				if (fSquareTargetDistance > RANGE_ATTACK_RANGE * RANGE_ATTACK_RANGE)
				{
					ERRLOG2(g_Log, "CID:%10u ���Ÿ� �������� �ʹ� �ָ� �ִ� Ÿ���� �����Ϸ� �մϴ�. �Ÿ�:%.2f(m)", 
						m_dwCID, fSquareTargetDistance);
					continue;
				}
			}
		}

		// TODO : ���� ������ �������ݽô�.
		cDefenserJudges[cDefender] = ClientConstants::Judge_Front;
		wDefenderMPHeal[cDefender] = 0;

		const unsigned short nPrevHP = ppDefenders[cIndex]->GetStatus().m_nNowHP;
		const unsigned short nPrevMP = ppDefenders[cIndex]->GetStatus().m_nNowMP;

		const unsigned short wPrevAttackerHP = m_CreatureStatus.m_nNowHP;

		// ����� �ݿ�
		lpDefenserNode[cDefender].m_wDamage = ppDefenders[cIndex]->ApplyDamage(attackType, this, 
			cOffencerJudge, cDefenserJudges[cDefender], wOffencerMPHeal, wDefenderMPHeal[cDefender], wError);

		const unsigned short nNowHP	= ppDefenders[cIndex]->GetStatus().m_nNowHP;
		const unsigned short nNowMP	= ppDefenders[cIndex]->GetStatus().m_nNowMP;

		// ��ų�� ���� �ڻ� ����
		if (0 == m_CreatureStatus.m_nNowHP)
		{
			m_CreatureStatus.m_nNowHP = wPrevAttackerHP;
			wError = PktAtAck::FAIL_SUICIDE;
			break;
		}
		else
		{			
			if (Creature::CT_PC == Creature::GetCreatureType(ppDefenders[cIndex]->GetCID()))
			{
				CCharacter* lpDefendCharacter = (CCharacter *)ppDefenders[cIndex];

				CMonster* lpSummonee = lpDefendCharacter->GetSummonee();
				if (0 != lpSummonee)
				{
					lpSummonee->GuardMe(this, lpDefenserNode[cDefender].m_wDamage);
				}

				if(ClientConstants::Judge_Chant == cDefenserJudges[cDefender] && lpDefenserNode[cDefender].m_wDamage == 0)
				{
					// edith 2009.06.20 ���������� æƮ����ϸ� ������0�ְ� ����� �������� ��� ����
					// æƮ ��ų�̰� �������� 0�϶� �������� �ް����� �ʴ´�.
				}
				else
				{
					lpDefendCharacter->CalculateEquipDurability((ClientConstants::Judge_Guard == cDefenserJudges[cDefender]) ? 
						AtType::GUARD : AtType::DEFENCE);
				}

				CGameClientDispatch* lpDispatch = lpDefendCharacter->GetDispatcher();
				if (0 != lpDispatch)
				{
					GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), this, lpDefendCharacter, 
						attackType, m_MotionInfo.m_fDirection, lpDefenserNode[cDefender].m_wDamage, cDefenserJudges[cDefender], 
						wDefenderMPHeal[cDefender], PktBase::NO_SERVER_ERR);
				}
			}

			// ���� ��Ŷ �����
			lpDefenserNode[cDefender].m_dwCharID = ppDefenders[cIndex]->GetCID();
			lpDefenserNode[cDefender].m_sCurrHP = nNowHP;
			lpDefenserNode[cDefender].m_sCurrMP = nNowMP;
			lpDefenserNode[cDefender].m_wMaxHP = ppDefenders[cIndex]->GetStatus().m_StatusInfo.m_nMaxHP;
			lpDefenserNode[cDefender].m_wMaxMP = ppDefenders[cIndex]->GetStatus().m_StatusInfo.m_nMaxMP;
			lpDefenserNode[cDefender].m_wMPHeal = wDefenderMPHeal[cDefender];

			lpDefenserNode[cDefender].m_cJudge = cDefenserJudges[cDefender];
		}

		++cDefender;
	}

	// --------------------------------------------------------------------------------------
	// ���ݽ� Ǯ���� ��æƮ��

	m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Invincible);

	if (true == bBrokenCounter)
	{
		m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::CounterAttack);
	}

	if (true == bBrokenStealth) 
	{
		CSpell* pSpell = m_SpellMgr.GetAffectedInfo().GetSpell(Skill::SpellID::Stealth);
		if (NULL != pSpell)
		{
			if (4 <= pSpell->GetSkillLockCount())
			{
				m_SpellMgr.GetCastingInfo().ClearChant();;
			}
		}

		m_SpellMgr.GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Stealth);
	}

	// --------------------------------------------------------------------------------------

	if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		if (0 == cDefender) 
		{ 
			Skill::CFunctions::ConsumeMP(attackType, this, 0);
		}
	}

	lpPktAtAck->m_dwCharID = m_dwCID;
	lpPktAtAck->m_AtType = attackType;

	lpPktAtAck->m_wHP = m_CreatureStatus.m_nNowHP;
	lpPktAtAck->m_wMP = m_CreatureStatus.m_nNowMP;
	lpPktAtAck->m_wMPHeal = wOffencerMPHeal;

	lpPktAtAck->m_cJudge = cOffencerJudge;
	lpPktAtAck->m_cDefenserNum = cDefender;

	if (0 != m_lpGameClientDispatch)
	{
		CSendStream& SendStream = m_lpGameClientDispatch->GetSendStream();
		if (true == SendStream.WrapCompress(
			szBuffer, sizeof(PktAtAck) + cDefender * sizeof(DefenserNode), CmdCharAttack, 0, wError) && 
			PktBase::NO_SERVER_ERR == wError)
		{
			CCell* lpCell = GetCellPos().m_lpCell;
			if (0 != lpCell)
			{
				lpCell->SendAttackInfo(m_dwCID, attackType, cDefender, lpDefenserNode);
			}

			// �����ڰ� ���̳� �������� ��� �������� ���� ������ AttackInfo �� �����ش�.
			if (ClientConstants::Judge_Heal == cOffencerJudge || ClientConstants::Judge_ManaHeal == cOffencerJudge)
			{
				if (0 != lpCell)
				{
					DefenserNode node;

					node.m_dwCharID	= m_dwCID;
					node.m_wMaxHP	= GetStatus().m_StatusInfo.m_nMaxHP;
					node.m_wMaxMP	= GetStatus().m_StatusInfo.m_nMaxMP;
					node.m_sCurrHP	= GetStatus().m_nNowHP;
					node.m_sCurrMP	= GetStatus().m_nNowMP;
					node.m_wMPHeal	= wOffencerMPHeal;
					node.m_wDamage	= 0;
					node.m_cJudge	= cOffencerJudge;

					if (ClientConstants::Judge_Heal == cOffencerJudge && node.m_sCurrHP > wOffencerPrevHP)
					{
						node.m_wDamage	= node.m_sCurrHP - wOffencerPrevHP;
					}
					else if (ClientConstants::Judge_ManaHeal == cOffencerJudge)
					{
						node.m_wDamage = wOffencerMPHeal;
					}

					lpCell->SendAttackInfo(m_dwCID, attackType, 1, &node);
				}
			}

			return true;
		}
	}

	return false;
}


bool CCharacter::Dead(CAggresiveCreature* pOffencer)
{
	if (0 != m_lpSummonee)
	{
		m_lpSummonee->Dead(0);
	}

	// ������ ���� �ִ� ���̾��ٸ�... ���� ���⸦ ����Ѵ�.
	if (0 != m_dwProtectGateCID)
	{
		CCastleGate* lpGate = reinterpret_cast<CCastleGate*>( CSiegeObjectMgr::GetInstance().GetSiegeObject(m_dwProtectGateCID) );
		if (lpGate)
		{
			lpGate->DeleteProtectGate(this);

			// Ŭ���̾�Ʈ���� ����
			CGameClientDispatch* lpDispatch = GetDispatcher();
			if (0 != lpDispatch)
			{
				GameClientSendPacket::SendCharCastleCmd(lpDispatch->GetSendStream(), lpGate->GetCastleID(),
					lpGate->GetCID(), 0, 0,
					PktCastleCmd::CASTLE_GATE_PROTECT_CANCEL,
					PktCastleCmd::NO_SERVER_ERR);
			}
		}
	}

	if (0 != pOffencer)
	{
		if (0 != pOffencer->GetMapIndex())
		{
			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(pOffencer->GetMapIndex());
			if (lpVirtualArea)
			{
				CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
				if (lpVirtualMonsterMgr && true == lpVirtualMonsterMgr->IsSummonee(pOffencer->GetCID()))
				{
					CSummonMonster* lpSummonee = reinterpret_cast<CSummonMonster *>(pOffencer);
					pOffencer = lpSummonee->GetMaster();
				}
			}
		}
		else
		{
			if (true == CCreatureManager::GetInstance().IsSummonee(pOffencer->GetCID()))
			{
				CSummonMonster* lpSummonee = reinterpret_cast<CSummonMonster *>(pOffencer);
				pOffencer = lpSummonee->GetMaster();
			}
		}
	}

	GAMELOG::ERRType eReturnCode = 0;
	__int64	dwPrevExp = m_CreatureStatus.m_nExp;

	m_FightInfo.m_nRestoreExp = 0;

	if (0 != pOffencer)
	{
		switch (Creature::GetCreatureType(pOffencer->GetCID()))
		{
		case Creature::CT_MONSTER:
		case Creature::CT_STRUCT:
			break;

		case Creature::CT_PC:
		case Creature::CT_SUMMON:

			m_eLastDeadType = DEAD_BY_CHARACTER;
			if (EnemyCheck::EC_ENEMY == IsEnemy(pOffencer))
			{	

				CalculateFame(reinterpret_cast<CCharacter *>(pOffencer));

				// ��Ʋ �׶��� ������ ��� Flag ���϶� ������ ó�����ش�. (���� ���� ����϶��� ����)
				if (0 != pOffencer && pOffencer->GetMapIndex() != 0)
				{
					CCharacter* lpOffencerChar = reinterpret_cast<CCharacter*>(pOffencer);
					if (GetRace() != lpOffencerChar->GetRace())
					{
						VirtualArea::CVirtualArea* lpVirtualArea = 
							VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(pOffencer->GetMapIndex());

						if (0 != lpVirtualArea && lpVirtualArea->GetVirtualType() == VirtualArea::BGSERVERMAP)
						{
							VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);
							if (VirtualArea::FRAG == lpBGServerMap->GetMapType())
							{
								lpBGServerMap->KillChar(GetCID(), lpOffencerChar);

								// �α� �����
								DETLOG10(g_Log, "Battle Server Log :: (Channel : %d, %s) - "
									"CID : 0x%08x ĳ����(%s, %s)�� CID : 0x%08x ĳ����(%s, %s)�� �׿����ϴ�. (���� Score - HM: %d  AK: %d)", 
									(lpBGServerMap->GetMapIndex() & ~VirtualArea::BGSERVERMAP), lpBGServerMap->GetMapTypeName(), 
									lpOffencerChar->GetCID(), lpOffencerChar->GetCharacterName(), 
									(lpOffencerChar->GetRace() == CClass::HUMAN) ? "HUMAN" : "AKHAN", 
									GetCID(), GetCharacterName(), 
									(GetRace() == CClass::HUMAN) ? "HUMAN" : "AKHAN", 
									lpBGServerMap->GetMapInfo().m_wScore[CClass::HUMAN], 
									lpBGServerMap->GetMapInfo().m_wScore[CClass::AKHAN]);
							}

						}
					}
				}

			}
			break;
		}
	}
	else
	{
		m_eLastDeadType = DEAD_BY_SUICIDE;
	}
	//you dont die in duel now hz
	CCharacter* lpLoserCharacter = reinterpret_cast<CCharacter *>(this);

	if(lpLoserCharacter->GetDuelOpponent() == NULL)
		SetDead(TRUE);
	else
	{
		m_CreatureStatus.m_nNowHP=1;
		m_SpellMgr.GetAffectedInfo().Disenchant(Skill::SpellType::NONE,
			Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::DEAD, 1, Skill::Disenchant::INFINITE_NUM);
		m_SpellMgr.GetAffectedInfo().Disenchant(Skill::SpellType::ETERNAL_SPELL,
			Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::NONE, 1, Skill::Disenchant::INFINITE_NUM);
		m_SpellMgr.GetCastingInfo().ClearEnchant();
		m_SpellMgr.GetCastingInfo().ClearChant();
	}

	DuelInit(PktDuC::DUC_DEAD);

	GAMELOG::LogCharDead(*this, dwPrevExp, m_CreatureStatus.m_nExp, 
		pOffencer, m_CreatureStatus.m_nLevel, eReturnCode);

	// ĳ���Ϳ� ���� �ɷ��ִ� ������� ���������� ����Ǿ� �� ������� �����Ѵ�.
	const SPELL spell = m_SpellMgr.GetAffectedInfo().GetSpellInfo(TRUE);
	SetSpell(spell);

	setKillSpreeLevel(0);

	/*CCharacter* lpLoserCharacter = reinterpret_cast<CCharacter *>(this);
	if (lpLoserCharacter != NULL) 
	{
	if (m_CreatureStatus.m_nNowHP < 2){//hayzohar
	m_CreatureStatus.m_nNowHP=1;
	};
	};*/

	m_SpellMgr.GetAffectedInfo().Disenchant(Skill::SpellType::NONE,
		Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::DEAD, 1, Skill::Disenchant::INFINITE_NUM);
	m_SpellMgr.GetAffectedInfo().Disenchant(Skill::SpellType::ETERNAL_SPELL,
		Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::NONE, 1, Skill::Disenchant::INFINITE_NUM);

	// ĳ���� ���� ��æ�� Ŭ���� �Ѵ�.
	m_SpellMgr.GetCastingInfo().ClearEnchant();
	m_SpellMgr.GetCastingInfo().ClearChant();

	// �̺κ��� �׾����� ��ų�� ��æƮ ��Ű�� ���̴� ����.
	//	CAggresiveCreature::Dead(pOffencer);

	if (0 != GetParty())
	{		
		// ��Ƽ�� ����� �˸���.
		GameClientSendPacket::SendCharDeadToParty(
			this, pOffencer, PktDeadInfo::DEAD);
	}	
	return (0 == eReturnCode);
}

void CCharacter::SaveSpell(BOOL bDead)
{
	// ĳ���Ϳ� ���� �ɷ��ִ� ������� ���������� ����Ǿ� �� ������� �����Ѵ�.
	const SPELL spell = m_SpellMgr.GetAffectedInfo().GetSpellInfo(bDead);
	SetSpell(spell);
}

bool CCharacter::Respawn(unsigned char cType, const Position& Pos)
{
	Position newPos;

	// ���� ���°� �ƴѵ� �������� �ϸ�. ������ �����س��� ������ �Ѵ�.
	if(!IsDead())
	{
		// ĳ���Ϳ� ���� �ɷ��ִ� ������� ���������� ����Ǿ� �� ������� �����Ѵ�.
		const SPELL spell = m_SpellMgr.GetAffectedInfo().GetSpellInfo();
		SetSpell(spell);
	}

	SetDead(FALSE);

	// edith 2009.08.26 ������ �޴������� ĳ���� ���� ����ȭ�� ���� Ÿ����̵� �ʱ�ȭ��.
	// �̷��� Ÿ�� ���̵� �ʱ�ȭ �Ǿ��ٸ� ������ ���ν������� �����Ѵ�.
	SetRespawnTownID(0);

	if (0 == Pos.m_fPointX && 0 == Pos.m_fPointY && 0 == Pos.m_fPointZ)
	{
		newPos = CCharRespawnMgr::GetInstance().GetDefaultRespawnPos(GetNation());
		m_DBData.m_Pos.LastPoint.fPointX = newPos.m_fPointX;
		m_DBData.m_Pos.LastPoint.fPointY = newPos.m_fPointY;
		m_DBData.m_Pos.LastPoint.fPointZ = newPos.m_fPointZ;
	}
	else
	{
		newPos = Pos;
	}

	if (0 == CCellManager::GetInstance().GetCell(m_CellPos.m_wMapIndex, Pos.PositionToPOS())) 
	{
		ERRLOG1(g_Log, "CID : 0x%08x, �� ���� �ۿ��� �������Ͽ����ϴ�. ĳ���� �ʱ� ���� ��ġ�� ������ŵ�ϴ�.", m_dwCID);

		if (0 != GetMapIndex())
		{
			VirtualArea::CVirtualArea* lpVirtualArea = 
				VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());

			if (lpVirtualArea)
			{
				m_CurrentPos = lpVirtualArea->GetRespawnPosition( 
					GetRace(), Math::Random::ComplexRandom( lpVirtualArea->GetMaxRespawnPos() ) );
			}
		}
		else
		{
			m_CurrentPos = CCharRespawnMgr::GetInstance().GetDefaultRespawnPos(GetNation());
		}

		m_DBData.m_Pos.LastPoint.fPointX = newPos.m_fPointX;
		m_DBData.m_Pos.LastPoint.fPointY = newPos.m_fPointY;
		m_DBData.m_Pos.LastPoint.fPointZ = newPos.m_fPointZ;
	}

	__int64	dwPrevExp = m_CreatureStatus.m_nExp;

	// 1. ���� �ɸ� ��� ��æƮ�� ����������.
	// ĳ���Ͱ� �׾����� ��æƮ�� �����־ Ŭ���̾�Ʈ���� ��� ��æƮ ����Ʈ�� �����ϱ� ������ ��� ��æƮ�� �����ߴ� �ٽðǴ�.
	m_SpellMgr.GetAffectedInfo().Disenchant(Skill::SpellType::NONE,
		Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::NONE, 1, Skill::Disenchant::INFINITE_NUM);
	m_SpellMgr.GetAffectedInfo().Disenchant(Skill::SpellType::ETERNAL_SPELL,
		Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::NONE, 1, Skill::Disenchant::INFINITE_NUM);
	m_SpellMgr.GetCastingInfo().ClearEnchant();

	//	Chant��, ���� �� ��ų�� Disable�Ѵ�. ( ������ �ɸ��� ���� Disable�� �ʿ� ����. )
	m_SpellMgr.GetCastingInfo().ClearChant();

	// edith 2008.06.03 ���� ��þƮȿ��
	if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		bool bAddRealmStatueEnchant = false;
		//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//hayzohar statue war fix
		//{
		// ���� ���� �ð��� �ƴ϶��, ���� ��æƮ ȿ���� �ɾ��ش�.
		if (!CGameTimeMgr::GetInstance().IsRealmWarTime())
		{
			bAddRealmStatueEnchant = true;
		}
		//}
		/*
		// edith 2008.07.17 �������� ���� �������� ���� �������� ������ �ش� ������ �ʿ����.
		if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
		{
		// ������ ������ ���Խ� ������ �ð��� �ƴ϶��, ���� ��æƮ ȿ���� �ɾ��ش�.
		if (CGameTimeMgr::GetInstance().IsSiegeWarTime())
		{
		bAddRealmStatueEnchant = false;
		}
		}
		*/
		/*
		// ���� ��æƮ ȿ���� ������ �� �ִ� ��Ȳ�̶��....
		if (bAddRealmStatueEnchant)
		{
		// edith 2008.06.03 �������� ������ 11�ð� ������ �ɾ��ش�.
		CCreatureManager::GetInstance().AddRealmStatueEnchant(this);
		}
		*/
		// �������� �������� ����Ʈ ȿ��.
		if (CGameTimeMgr::GetInstance().IsRealmWarTime() &&
			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))
		{
			RealmSkill::RealmInchantAdd(this);
		}
	}

	// edith 2009.06.13 16�� �� / 17�� ������ �����ð� ����
	int InvincibleTime = 30;

	if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
		InvincibleTime = 15; //how long you are in G mode hayzohar


	switch (cType)
	{
	case CCharRespawnMgr::RST_RESURRECTION :	// ������������ ��Ȱ

		if (0 == m_CreatureStatus.m_nNowHP)
		{
			m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP * RESURRECTION_RECOVERY_PERCENTAGE / 100;
			m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP * RESURRECTION_RECOVERY_PERCENTAGE / 100;

			m_CreatureStatus.m_nExp += m_FightInfo.m_nRestoreExp;
			m_FightInfo.m_nRestoreExp = 0;
		}
		break;				   

	case CCharRespawnMgr::RST_BATTLE :			// ��Ʋ ���ѿ��� ������ ��

		if (0 == m_CreatureStatus.m_nNowHP)
		{
			m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
			m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP;
		}

		if (!CServerSetup::GetInstance().IsBattleGameServer())
		{
			// 30�ʰ� ����
			Skill::CAddSpell<CInvincibleSpell>(
				CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, this, 
				Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Invincible, 1, InvincibleTime))(this);
		}
		break;

	case CCharRespawnMgr::RST_TOWN :			// �װų� ��Ż �̿����� �������� ��Ȱ (�Ϲ����� ������)

		if (0 == m_CreatureStatus.m_nNowHP)
		{
			m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP * TOWN_RECOVERY_PERCENTAGE / 100;
			m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP * TOWN_RECOVERY_PERCENTAGE / 100;

			if (!CServerSetup::GetInstance().IsBattleGameServer())
			{
				// 30�ʰ� ����
				Skill::CAddSpell<CInvincibleSpell>(
					CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, this, 
					Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Invincible, 1, InvincibleTime))(this);
			}
		}
		break;

	case CCharRespawnMgr::RST_FORTRESS :		// �װų� ��Ż �̿����� ����� ������ ��Ȱ

		if (0 == m_CreatureStatus.m_nNowHP)
		{
			m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP * FORTRESS_RECOVERY_PERCENTAGE / 100;
			m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP * FORTRESS_RECOVERY_PERCENTAGE / 100;

			if (!CServerSetup::GetInstance().IsBattleGameServer())
			{
				// 30�ʰ� ����
				Skill::CAddSpell<CInvincibleSpell>(
					CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, this, 
					Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Invincible, 1, InvincibleTime))(this);
			}
		}
		break;
	}

	// edith 2008.07.08 ��Ʋ�׶��弭������ ������ ������ �ִ��..
	if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		// ���߿� 30�ʰ� ���� �ɷ��� ����.
		m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}


	if (0 != m_lpGameClientDispatch)
	{
		CSendStream& SendStream = m_lpGameClientDispatch->GetSendStream();
		char* lpBuffer = SendStream.GetBuffer(sizeof(PktRsAck));

		if (0 != lpBuffer)
		{
			PktRsAck* lpPktRsAck = reinterpret_cast<PktRsAck*>(lpBuffer);

			lpPktRsAck->m_dwCharID = m_dwCID;
			lpPktRsAck->m_Position = newPos.PositionToPOS();

			lpPktRsAck->m_wHP   = m_CreatureStatus.m_nNowHP;
			lpPktRsAck->m_wMP   = m_CreatureStatus.m_nNowMP;
			lpPktRsAck->m_dlExp = m_CreatureStatus.m_nExp;

			lpPktRsAck->m_dwGold = m_DBData.m_Info.Gold;

			SendStream.WrapCrypt(sizeof(PktRsAck), CmdCharRespawn, 0, 0);
		}
	}

	// 2. ĳ���Ͱ� �׾����� �����ص� ��æƮ�� �ٽðǴ�.
	// ĳ���Ͱ� �׾����� ��æƮ�� �����־ Ŭ���̾�Ʈ���� ��� ��æƮ ����Ʈ�� �����ϱ� ������ ��� ��æƮ�� �����ߴ� �ٽðǴ�.
	const SPELL spell = GetSpell();
	m_SpellMgr.SetSpell(spell);

	// �������� ���� ������ �������� ������ ������ �ٽ� �ɾ��ش�.
	if(IsRealmWarBuffer())
		CCreatureManager::GetInstance().AddRealmStatueEnchant(this);

	SetRealmWarBuffer(FALSE);


	if (cType != CCharRespawnMgr::RST_RESURRECTION)
	{
		switch(m_eLastDeadType)
		{
		case DEAD_BY_NONE:      // �� �� ����
		case DEAD_BY_CHARACTER: // ĳ���Ϳ� ���� ���            
			break;

		case DEAD_BY_SUICIDE:   // �ڻ�

			// �ڻ�� ������ ����
			CalculateAllEquipDurability(DURABILITY_DECREASE_PERSENT_BY_SUICIDE);
			break;

		case DEAD_BY_MONSTER:   // ���Ϳ� ���� ��� 

			if (!CServerSetup::GetInstance().GetDeathPenaltyEvent())
			{
				unsigned char cDecreasePersent = 0;

				if(m_AbilityValue[Skill::Type::AB_ENDUR_SHILD] != 0)
					cDecreasePersent = DURABILITY_DECREASE_PERSENT_BY_MONSTER*m_AbilityValue[Skill::Type::AB_ENDUR_SHILD]/100;

				// ���Ϳ��� �׾��� �� ������ ����
				CalculateAllEquipDurability(DURABILITY_DECREASE_PERSENT_BY_MONSTER-cDecreasePersent);
			}
			break;
		}
	}        

	// �ٽ� ��Ƴ�����, ��� Ÿ���� NONE���� �ʱ�ȭ�Ѵ�
	m_eLastDeadType = DEAD_BY_NONE;

	// ĳ���� ������ �α�
	GAMELOG::LogCharRespawn(*this, dwPrevExp, m_CreatureStatus.m_nExp, 0);
	return true;
}


// ��Ʋ �׶��忡�� �ʱ� ���� ������ �ڵ� ������ �����ִ� �Լ�
bool CCharacter::AutoRespawn()
{
	if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
	{
		// ������ ��ǥ
		const POS respawnPos[CClass::MAX_RACE] =
		{
			{ 1403.06f, 18.25f, 1958.41f },
			{ 2014.29f, 63.50f, 1648.47f }
		};

		// �ڵ����� ������ �ʱ� �� ������ �̵���Ų��.
		unsigned char cZone = SERVER_ID::NONE;
		switch (GetRace())
		{
		case CClass::HUMAN:
			cZone = SERVER_ID::ZONE1;//was zone1
			break;

		case CClass::AKHAN:
			cZone = SERVER_ID::ZONE2;//was zone2
			break;

		default:
			ERRLOG2(g_Log, "CID:0x%08x ĳ������ ������ �̻��մϴ�. ����:%d", GetCID(), GetRace());
			return false;
		}

		GetStatus().m_nNowHP = GetStatus().m_StatusInfo.m_nMaxHP;
		GetStatus().m_nNowMP = GetStatus().m_StatusInfo.m_nMaxMP;

		SetDead(FALSE);

		// ä���� 2�� �̻� �ִ� ��� ä��â�� ����ְ� ���̵��� ���� �ʽ��ϴ�. 
		// �� ��� ä��â�� ������� ��Ƽ�� ���� ���� ���� �����Ƿ� 1ä�η� ���� �̵���Ű�� �����Ͽ����ϴ�. (2004-09-17 by �ε�)
		if (false == MoveZone(respawnPos[ GetRace() ], cZone, 0))
		{
			ERRLOG1(g_Log, "CID:0x%08x ��Ʋ�׶��� Ż�⿡ �����Ͽ����ϴ�.", GetCID());
			return false;
		}

		return true;
	}

	return false;
}


bool CCharacter::AttackUsingBow(unsigned short wType)
{	
	Item::CEquipment* lpEquipment = m_Equipments.GetLeftHand();
	if (0 == lpEquipment) 
	{ 
		return true;
	}

	switch (wType)
	{
	case AtType::BOW:		
	case AtType::CROSSBOW:
		{
			if (wType == AtType::BOW && lpEquipment->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::ARROW) 
			{
				return false;
			}
			if (wType == AtType::CROSSBOW && lpEquipment->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::BOLT) 
			{
				return false;
			}

			if (false == UseAmmo(reinterpret_cast<Item::CUseItem *>(lpEquipment)))
			{
				ERRLOG1(g_Log, "CID:0x%08x ȭ��(��Ʈ) ��뿡 �����Ͽ����ϴ�.", m_dwCID);
				return false;
			}

			if (0 != m_lpGameClientDispatch)
			{
				GameClientSendPacket::SendCharEquipDurability(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
					lpEquipment->GetPos().m_cIndex, lpEquipment->GetNumOrDurability(), 0, PktBase::NO_SERVER_ERR);
			}

		} break;

	default:
		{
			if (lpEquipment->GetItemInfo().m_DetailData.m_cItemType == Item::ItemType::ARROW || 
				lpEquipment->GetItemInfo().m_DetailData.m_cItemType == Item::ItemType::BOLT) 
			{
				return false;
			}

		} break;
	}

	if (0 == lpEquipment->GetNumOrDurability() && true == lpEquipment->IsSet(Item::DetailData::STACKABLE))
	{
		CalculateStatusData(false);
		if (false == m_Equipments.RemoveItem(lpEquipment->GetPos()))
		{ 
			ERRLOG1(g_Log, "CID:0x%08x ���� �������� ��񿡼� ����� �� �����߽��ϴ�.", m_dwCID);
		}		
	}

	return true;
}


bool CCharacter::UseAmmo(Item::CUseItem* pUseItem)
{	
	using namespace Item::ItemType;

	// �˸��� ������ źȯ���� üũ
	switch (pUseItem->GetItemInfo().m_DetailData.m_cItemType)
	{
	case ARROW:
		{	// ȭ��
			Item::CEquipment* pUsingWeapon = m_Equipments.GetRightHand();
			if (0 == pUsingWeapon) 
			{
				return false;
			}

			if (pUsingWeapon->GetItemInfo().m_DetailData.m_cItemType != BOW) 
			{
				return false;
			}

			pUseItem->SetNumOrDurability(pUseItem->GetNumOrDurability()); /*			pUseItem->SetNumOrDurability(pUseItem->GetNumOrDurability() - 1); hayzohar*/

		} break;

	case BOLT:
		{	// ��Ʈ
			Item::CEquipment* pUsingWeapon = m_Equipments.GetRightHand();
			if (0 == pUsingWeapon) 
			{
				return false;
			}

			if (pUsingWeapon->GetItemInfo().m_DetailData.m_cItemType != CROSSBOW) 
			{
				return false;
			}

			pUseItem->SetNumOrDurability(pUseItem->GetNumOrDurability()); /*			pUseItem->SetNumOrDurability(pUseItem->GetNumOrDurability() - 1); hayzohar*/

		} break;

	case AMMO:
		{	// źȯ
			Item::CEquipment* lpSkillArm = m_Equipments.GetLeftHand();
			if (0 == lpSkillArm) 
			{
				return false;
			}

			if (lpSkillArm->GetItemInfo().m_DetailData.m_cItemType != SKILL_A_GUN)
			{
				return false;
			}

		} break;
	}

	return true;
}

bool CCharacter::IsPeaceMode(void)						
{ 
	// MichaelBless �Ͻ� �ǽ����� ����
	// ���� ������ ������ ��ȭ ���
	if (SERVER_ID::ZONE1 == CServerSetup::GetInstance().GetServerZone())//(SERVER_ID::ZONE12 == CServerSetup::GetInstance().GetServerZone())
	{
		if(GetLevel() < 51)
		{
			if(GetEnchantInfo().GetFlag(Skill::SpellID::MichaelBless))
				return true;
		}
	}
	/*
	// ���� ������ ������ ��ȭ ���
	if (SERVER_ID::ZONE12 == CServerSetup::GetInstance().GetServerZone())
	{
	return true;
	}
	*/
	/*
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))
	{
	// ī������ �ִ��� ����� �ð��� �ƴ϶�� ��ȭ ���
	if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone() && 
	!CGameTimeMgr::GetInstance().IsGuildWarTime())
	{
	return true;
	}
	}
	*/

	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(GetGID());
	if (NULL != lpGuild) 
	{
		if (Guild::JOIN_WAIT != lpGuild->GetTitle(m_dwCID))
		{
			return lpGuild->IsPeaceMode();
		}
	}

	return false;
}

char CCharacter::GetEliteBonus(void)
{
	EliteBonus::EliteBonusData eliteBonus = CCreatureManager::GetInstance().GetEliteBonus();
	char cBonus = eliteBonus.m_cLevel * ((GetRace() == eliteBonus.m_cNation) ? 1 : (-1));

	// �߷��� ��ȣå
	const char cServerZone = CServerSetup::GetInstance().GetServerZone();
	if ((Creature::KARTERANT == GetNation() && SERVER_ID::ZONE5 == cServerZone) || 
		(Creature::MERKADIA == GetNation() && SERVER_ID::ZONE4 == cServerZone))
	{
		cBonus -= 7;
	}

	// ���� ���� ó��
	cBonus += GetCaste();

	if (cBonus > EliteBonus::MAX_BONUS_LEVEL)
	{
		return EliteBonus::MAX_BONUS_LEVEL;
	}

	if (cBonus < EliteBonus::MAX_BONUS_LEVEL * (-1))
	{
		return EliteBonus::MAX_BONUS_LEVEL * (-1);
	}

	return cBonus;
}

char CCharacter::GetCaste(void)
{
	if (m_DBData.m_Info.Fame <= 2000)	{ return 0; }
	if (m_DBData.m_Info.Fame <= 10000)	{ return 1; }
	if (m_DBData.m_Info.Fame <= 25000)	{ return 2; }
	if (m_DBData.m_Info.Fame <= 50000)	{ return 3; }
	if (m_DBData.m_Info.Fame <= 75000)	{ return 4; }
	if (m_DBData.m_Info.Fame <= 100000)	{ return 5; }
	if (m_DBData.m_Info.Fame <= 150000)	{ return 6; }
	if (m_DBData.m_Info.Fame <= 250000)	{ return 7; }
	if (m_DBData.m_Info.Fame <= 500000)	{ return 8; }

	return 9;
}

unsigned long CCharacter::KillSpreeMultiplier(CCharacter* lpWinnerCharacter,CCharacter* lpLoserCharacter)
{

	//unsigned long RightNow = timeGetTime();

	lpWinnerCharacter->m_dwLastKilledID =lpLoserCharacter->GetCID();

	if (((timeGetTime() - lpWinnerCharacter->m_dwLastKillTime)  > 5000 ) && lpWinnerCharacter->m_dwLastKillTime != 0){
		lpWinnerCharacter->m_dwLastKillTime = 0;//reset time
		lpWinnerCharacter->m_dwKilingCount = 1;	//reset count
		lpWinnerCharacter->m_dwLastKilledID = 0;//reset id;
		return 1;
	}
	/*if (lpLoserCharacter->GetCID() == lpWinnerCharacter->m_dwLastKilledID){
	lpWinnerCharacter->m_dwLastKillTime = RightNow;//reset time
	lpWinnerCharacter->m_dwKilingCount = 0;	//reset count
	lpWinnerCharacter->m_dwLastKilledID = 0;//reset id;
	return 1;
	}*/

	lpWinnerCharacter->m_dwKilingCount++;
	if((lpWinnerCharacter->m_dwKilingCount > 0)&&(lpWinnerCharacter->m_dwKilingCount <13))
	{
		return (lpWinnerCharacter->m_dwKilingCount);
	}
	else
	{
		lpWinnerCharacter->m_dwLastKillTime = 0;//reset time
		lpWinnerCharacter->m_dwKilingCount = 1;	//reset count
		lpWinnerCharacter->m_dwLastKilledID = 0;//reset id;
		return (lpWinnerCharacter->m_dwKilingCount);
	}
}
int CCharacter::GetPositionOfSkillInBigArr(unsigned long skillID)
{
	switch(skillID)
	{
	case 0x8101:
		return 0;
		break;

	case 0x9301:
		return 1;
		break;

	case 0x9401:
		return 2;
		break;

	case 0x8104:
		return 3;
		break;

	case 0x8201:
		return 4;
		break;

	case 0x8205:
		return 5;
		break;

	case 0x8302:
		return 6;
		break;

	case 0x8C09:
		return 7;
		break;

	case 0x8303:
		return 8;
		break;

	case 0x9215:
		return 9;
		break;

	case 0x8304:
		return 10;
		break;

	case 0x8305:
		return 11;
		break;

	case 0x8401:
		return 12;
		break;

	case 0x8404:
		return 13;
		break;

	case 0x8111:
		return 14;
		break;

	case 0x9217:
		return 15;
		break;

	case 0x8C08:
		return 16;
		break;

	case 0x8501:
		return 17;
		break;

	case 0x9308:
		return 18;
		break;

	case 0x8503:
		return 19;
		break;

	case 0x8504:
		return 20;
		break;

	case 0x8505:
		return 21;
		break;

	case 0x9309:
		return 22;
		break;

	case 0x8601:
		return 23;
		break;

	case 0x9408:
		return 24;
		break;

	case 0x9809:
		return 25;
		break;

	case 0x8603:
		return 26;
		break;

	case 0x8108:
		return 27;
		break;

	case 0x8605:
		return 28;
		break;

	case 0x9111:
		return 29;
		break;

	case 0x8109:
		return 30;
		break;

	case 0x8606:
		return 31;
		break;

	case 0x9112:
		return 32;
		break;

	case 0x8607:
		return 33;
		break;

	case 0x9409:
		return 34;
		break;

	case 0x8702:
		return 35;
		break;

	case 0x8704:
		return 36;
		break;

	case 0x9804:
		return 37;
		break;

	case 0x8707:
		return 38;
		break;

	case 0x8708:
		return 39;
		break;

	case 0x8709:
		return 40;
		break;

	case 0x8710:
		return 41;
		break;

	case 0x9807:
		return 42;
		break;

	case 0x8803:
		return 43;
		break;

	case 0x8804:
		return 44;
		break;

	case 0x8A09:
		return 45;
		break;

	case 0x8805:
		return 46;
		break;

	case 0x9504:
		return 47;
		break;

	case 0x8901:
		return 48;
		break;

	case 0x8A07:
		return 49;
		break;

	case 0x9601:
		return 50;
		break;

	case 0x8902:
		return 51;
		break;

	case 0x8808:
		return 52;
		break;

	case 0x8903:
		return 53;
		break;

	case 0x9508:
		return 54;
		break;

	case 0x8807:
		return 55;
		break;

	case 0x8904:
		return 56;
		break;

	case 0x9507:
		return 57;
		break;

	case 0x8905:
		return 58;
		break;

	case 0x8B10:
		return 59;
		break;

	case 0x8906:
		return 60;
		break;

	case 0x8907:
		return 61;
		break;

	case 0x8A01:
		return 62;
		break;

	case 0x8A02:
		return 63;
		break;

	case 0x8A04:
		return 64;
		break;

	case 0x8A05:
		return 65;
		break;

	case 0x9606:
		return 66;
		break;

	case 0x8A06:
		return 67;
		break;

	case 0x8B01:
		return 68;
		break;

	case 0x8C01:
		return 69;
		break;

	case 0x9701:
		return 70;
		break;

	case 0x990F:
		return 71;
		break;

	case 0x8B04:
		return 72;
		break;

	case 0x8B06:
		return 73;
		break;

	case 0x8B07:
		return 74;
		break;

	case 0x8B09:
		return 75;
		break;

	case 0x8C02:
		return 76;
		break;

	case 0x8C03:
		return 77;
		break;

	case 0x9704:
		return 78;
		break;

	case 0x8B05:
		return 79;
		break;

	case 0x8C04:
		return 80;
		break;

	case 0x9703:
		return 81;
		break;

	case 0x8B12:
		return 82;
		break;

	case 0x8C05:
		return 83;
		break;

	case 0x8B08:
		return 84;
		break;

	case 0x8C06:
		return 85;
		break;

	case 0x9707:
		return 86;
		break;

	case 0x8C07:
		return 87;
		break;

	case 0x9211:
		return 88;
		break;

	case 0x9106:
		return 89;
		break;

	case 0x9201:
		return 90;
		break;

	case 0x8307:
		return 91;
		break;

	case 0x9202:
		return 92;
		break;

	case 0x9204:
		return 93;
		break;

	case 0x8406:
		return 94;
		break;

	case 0x9218:
		return 95;
		break;

	case 0x9302:
		return 96;
		break;

	case 0x9303:
		return 97;
		break;

	case 0x9305://fasthit
		return 130;
		break;

	case 0x9307:
		return 99;
		break;

	case 0x9402:
		return 100;
		break;

	case 0x8809:
		return 101;
		break;

	case 0x9407:
		return 102;
		break;

	case 0x9509:
		return 103;
		break;

	case 0x9502:
		return 104;
		break;

	case 0x9506:
		return 105;
		break;

	case 0x9602:
		return 106;
		break;

	case 0x9603:
		return 107;
		break;

	case 0x9604:
		return 108;
		break;

	case 0x9605:
		return 109;
		break;

	case 0x9706:
		return 110;
		break;

	case 0x970A:
		return 111;
		break;

	case 0x9802:
		return 112;
		break;

	case 0x9805:
		return 113;
		break;

	case 0x8711:
		return 114;
		break;

	case 0x9806:
		return 115;
		break;	

	case 0x9501:
		return 116;
		break;

	case 0x8908:
		return 117;
		break;

	case 0x8A08:
		return 118;
		break;

	case 0x9801:
		return 119;
		break;

	case 0x8113:
		return 120;
		break;

	case 0x8502:
		return 121;
		break;

	case 0x9113:
		return 122;
		break;

	case 0x9702:
		return 123;
		break;

	case 0x9609:
		return 124;
		break;

	case 0x9206:
		return 125;
		break;

	case 0x9208:
		return 125;
		break;

	case 0x9104:
		return 126;
		break;


	}
	return 130;
}
bool CCharacter::IsCast(unsigned long skillID)
{
	switch(skillID)
	{
	case 0x8303:
	case 0x8304:
	case 0x8305:
	case 0x8307:
	case 0x8404:
	case 0x8707:
	case 0x8710:
	case 0x8803:
	case 0x8807:
	case 0x8808:
	case 0x8809:
	case 0x8902:
	case 0x8903:
	case 0x8904:
	case 0x8A02:
	case 0x8A04:
	case 0x8A05:
	case 0x8B04:
	case 0x8C03:
	case 0x8C07:
	case 0x8C08:
	case 0x9202:
	case 0x9208:
	case 0x9210:
	case 0x9211:
	case 0x9215:
	case 0x9302:
	case 0x9303:
	case 0x9506:
	case 0x9507:
	case 0x9508:
	case 0x9509:
	case 0x9602:
	case 0x9603:
	case 0x9604:
	case 0x9606:
	case 0x9607:
	case 0x9704:
	case 0x9808:
		return true;
		break;

	default:
		return false;

	}
	return false;
}

int CCharacter::GetPositionOfSkillInArr(unsigned long skillID)
{
	switch(skillID)
	{
	case 0x8D01:
		return 0;
		break;

	case 0x8D02:
		return 1;
		break;

	case 0x8D03:
		return 2;
		break;

	case 0x8D04:
		return 3;
		break;

	case 0x8D05:
		return 4;
		break;

	case 0x8D06:
		return 5;
		break;

	case 0x8D11:
		return 6;
		break;

	case 0x8D12:
		return 7;
		break;

	case 0x8D13:
		return 8;
		break;

	case 0x8D14:
		return 9;
		break;

	case 0x8D15:
		return 10;
		break;

	default:
		return 12;
		break;
	}
	return 12;
}

void CCharacter::CalculateFame(CCharacter* lpWinnerCharacter)
{
	CGameClientDispatch* lpDispatch = 0;

	CCharacter* lpLoserCharacter = reinterpret_cast<CCharacter *>(this);

	unsigned char cWinnerCmd = PktFIAck::FAME_WIN;
	unsigned char cLoserCmd = PktFIAck::FAME_LOSE;

	// �о��ֱ� ���� (�̱��� ��ʸ�Ʈ ������������ PvP���� �� ���¡ ����å�� ��ȭ�ȴ�.)
	const unsigned long dwLimitFame = 1000;
		
	if (0 == lpLoserCharacter->GetFame() && dwLimitFame <= lpWinnerCharacter->GetFame())
	{
		cWinnerCmd = PktFIAck::FAME_WIN_LARGE_GAP;
	}

	// ���� ����
	if (true == lpLoserCharacter->GetThreat().SaveEnemy(lpWinnerCharacter->GetCID()))
	{
		cWinnerCmd = PktFIAck::FAME_WIN_CONTINUE;
		cLoserCmd = PktFIAck::FAME_LOSE_CONTINUE;
	}
	else
	{
		GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			
		if(lpDBAgentDispatch)
		{
			CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();

			char* lpBuffer = SendStream.GetBuffer(sizeof(PktSaveEnemy));

			if(lpBuffer)
			{
				PktSaveEnemy* lpPktSaveEnemy = reinterpret_cast<PktSaveEnemy*>(lpBuffer);
				lpPktSaveEnemy->m_dwCID			= m_dwCID;
				lpPktSaveEnemy->m_dwAttackedCID = lpWinnerCharacter->GetCID();
				lpPktSaveEnemy->m_dwTick		= 0;
				
				if(!SendStream.WrapCrypt(sizeof(PktSaveEnemy), CmdSaveEnemy, 0, 0))
				{
					ERRLOG1(g_Log, "CID:0x%08x ���� ���� ��Ŷ�� ������ ���߽��ϴ�.", m_dwCID);
				}
			}
		}	
	}

	// edith 2009.07.16 ������϶� ���� �����̴��� ���̸� ����� �ް��Ѵ�.
	if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
	{
		// ��� ���� �ð��̸�
		if(CGameTimeMgr::GetInstance().IsGuildWarTime())
		{
			// �����Ѵ�.
		}
		else
		{
			// edith 2008.08.18 ���� �����̸� �����Ѵ�. ���� �� ��Ÿ ��Ȳ�� ����
			if(lpWinnerCharacter->GetRace() == lpLoserCharacter->GetRace())
			{
				cWinnerCmd = PktFIAck::FAME_INFO;
				cLoserCmd = PktFIAck::FAME_INFO;
			}
		}
	}
	else
	{
		// edith 2008.08.18 ���� �����̸� �����Ѵ�. ���� �� ��Ÿ ��Ȳ�� ����
		if(lpWinnerCharacter->GetRace() == lpLoserCharacter->GetRace())
		{
			cWinnerCmd = PktFIAck::FAME_INFO;
			cLoserCmd = PktFIAck::FAME_INFO;
		}
	}


	// ��� ����
	if (lpLoserCharacter->GetDuelOpponent() == lpWinnerCharacter)
	{
		cWinnerCmd = PktFIAck::FAME_WIN_DUEL;
		cLoserCmd = PktFIAck::FAME_LOSE_DUEL;
	}

	// ����Ʋ ����
	CCharacterParty* lpLoserParty = reinterpret_cast<CCharacterParty*>(lpLoserCharacter->GetParty());
	CCharacterParty* lpWinnerParty = reinterpret_cast<CCharacterParty*>(lpWinnerCharacter->GetParty());
	if (NULL != lpLoserParty && NULL != lpWinnerParty)
	{
		if (lpLoserParty->GetHostileParty() == lpWinnerParty)
		{
			cWinnerCmd = PktFIAck::FAME_WIN_DUEL;
			cLoserCmd = PktFIAck::FAME_LOSE_DUEL;
		}
	}
	
	// edith 2009.12.26 ���� 20�̻��̸� �׾����� ��ó���� ���� �ʴ´�.
	// ���� ���ڿ� ������ ������ 20������ ������ ��ó���� ���� �ʴ´�
	if (NULL == lpWinnerParty)
	{
		int iLevelGap = lpWinnerCharacter->GetLevel() - lpLoserCharacter->GetLevel();
		if(iLevelGap >= 20)
		{
			cWinnerCmd = PktFIAck::FAME_WIN_LEVELMIN;
			cLoserCmd = PktFIAck::FAME_LOSE_LEVELMIN;
		}
	}

	GAMELOG::ERRType eReturnCode = 0;
    // -----------------------------------------------------------------------------------------------------
	// ���� ���� ��Ģ
	
	if (PktFIAck::FAME_LOSE == cLoserCmd)
	{
		unsigned long dwPrevFame = lpLoserCharacter->GetFame();
        unsigned long lReduceFame = std::min(static_cast<unsigned long>(10), lpLoserCharacter->GetFame());
		unsigned long dwNextFame = dwPrevFame - lReduceFame;
		lpLoserCharacter->SetFame(dwNextFame);

		GAMELOG::LogFameLoseBattle(*lpLoserCharacter, lpWinnerCharacter, 
			lpLoserParty, dwPrevFame, dwNextFame, eReturnCode);

		CCharacterParty* lpLoserParty = reinterpret_cast<CCharacterParty *>(lpLoserCharacter->GetParty());
		if (NULL != lpLoserParty)
		{
			lpLoserParty->SendDivisionFame(lpWinnerCharacter, lpLoserCharacter, cLoserCmd, 0);
		}
	}

	lpDispatch = lpLoserCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpLoserCharacter, 
			lpWinnerCharacter->GetCharacterName(), lpLoserCharacter->GetCharacterName(),
			lpWinnerCharacter->GetNation(), static_cast<unsigned char>(lpWinnerCharacter->GetClass()),
			cLoserCmd, PktBase::NO_SERVER_ERR);
	}
   
	// -----------------------------------------------------------------------------------------------------
	// ���� ���� ��Ģ

	unsigned long dwAcquireAmount = 10;

	// ��Ƽ�� �ƴҶ�
	if (NULL == lpWinnerParty)
	{
		unsigned long dwPrevFame = 0;
		unsigned long dwNextFame = 0;
		unsigned long dwPrevMileage = 0;
		unsigned long dwNextMileage = 0;

		if (PktFIAck::FAME_WIN == cWinnerCmd)
		{
			// ���� ���� ó��
			dwPrevFame = lpWinnerCharacter->GetFame();
			unsigned long dwGetFame = std::min(dwAcquireAmount * static_cast<unsigned long>(CServerSetup::GetInstance().GetFameDefault() / 100.0f), 
											   UINT_MAX - lpWinnerCharacter->GetFame());
			dwNextFame = dwPrevFame + dwGetFame;

			lpWinnerCharacter->SetFame(dwNextFame);
		}

		if (PktFIAck::FAME_WIN == cWinnerCmd || PktFIAck::FAME_WIN_LEVELMIN == cWinnerCmd )
		{
			// �޴޿� ���� ó��
			dwPrevMileage = lpWinnerCharacter->GetMileage();
			dwNextMileage = dwPrevMileage;

			// edith 2009.06.27 �޴��� ������, ��������� ���� ���϶��� ��´�.
			if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
			{
				// ��� ���� �ð��̸�
				if (CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					unsigned long dwGetMileage = std::min(dwAcquireAmount * static_cast<unsigned long>(CServerSetup::GetInstance().GetMileageDefault() / 100.0f), 
														UINT_MAX - lpWinnerCharacter->GetMileage());
					dwNextMileage = dwPrevMileage + dwGetMileage;
				}
			}
			else if (SERVER_ID::STONE_WAR1 <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
			{
				// ���� ���� �ð��̸�
				if (CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					unsigned long dwGetMileage = std::min(dwAcquireAmount * static_cast<unsigned long>(CServerSetup::GetInstance().GetMileageDefault() / 100.0f), 
														UINT_MAX - lpWinnerCharacter->GetMileage());
					dwNextMileage = dwPrevMileage + dwGetMileage;
				}
			}

			lpWinnerCharacter->SetMileage(dwNextMileage);

			GAMELOG::LogFameGetBattle(*lpWinnerCharacter, lpLoserCharacter, lpWinnerParty, 
				dwPrevFame, dwNextFame, dwPrevMileage, dwNextMileage, eReturnCode);
		}

		lpDispatch = lpWinnerCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpWinnerCharacter, 
				lpWinnerCharacter->GetCharacterName(), lpLoserCharacter->GetCharacterName(),
				lpLoserCharacter->GetNation(), (unsigned char)lpLoserCharacter->GetClass(),
				cWinnerCmd, PktBase::NO_SERVER_ERR);
		}
	}
	else
	{
		// ��Ƽ�϶�
		lpWinnerParty->SendDivisionFame(lpWinnerCharacter, lpLoserCharacter, 
			cWinnerCmd, static_cast<unsigned char>(dwAcquireAmount));
	}
}

/*void CCharacter::CalculateFame(CCharacter* lpWinnerCharacter)
{
	bool changed=false;
	CGameClientDispatch* lpDispatch = 0;

	CCharacter* lpLoserCharacter = reinterpret_cast<CCharacter *>(this);
	int currentLoserSpree = lpLoserCharacter->getKillSpreeLevel();

	unsigned char cWinnerCmd = PktFIAck::FAME_WIN;
	unsigned char cLoserCmd = PktFIAck::FAME_LOSE;

	// �о��ֱ� ���� (�̱��� ��ʸ�Ʈ ������������ PvP���� �� ���¡ ����å�� ��ȭ�ȴ�.)
	const unsigned long dwLimitFame = 1000;

	if (0 == lpLoserCharacter->GetFame() && dwLimitFame <= lpWinnerCharacter->GetFame())
	{
		cWinnerCmd = PktFIAck::FAME_WIN_LARGE_GAP;
	}

	// ���� ����
	if (true == lpLoserCharacter->GetThreat().SaveEnemy(lpWinnerCharacter->GetCID()))
	{
		cWinnerCmd = PktFIAck::FAME_WIN_CONTINUE;
		cLoserCmd = PktFIAck::FAME_LOSE_CONTINUE;
	}
	else
	{
		GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

		if(lpDBAgentDispatch)
		{
			CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();

			char* lpBuffer = SendStream.GetBuffer(sizeof(PktSaveEnemy));

			if(lpBuffer)
			{
				PktSaveEnemy* lpPktSaveEnemy = reinterpret_cast<PktSaveEnemy*>(lpBuffer);
				lpPktSaveEnemy->m_dwCID			= m_dwCID;
				lpPktSaveEnemy->m_dwAttackedCID = lpWinnerCharacter->GetCID();
				lpPktSaveEnemy->m_dwTick		= 0;

				if(!SendStream.WrapCrypt(sizeof(PktSaveEnemy), CmdSaveEnemy, 0, 0))
				{
					ERRLOG1(g_Log, "CID:0x%08x ���� ���� ��Ŷ�� ������ ���߽��ϴ�.", m_dwCID);
				}
			}
		}	
	}

	// edith 2009.07.16 ������϶� ���� �����̴��� ���̸� ����� �ް��Ѵ�.
	if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
	{
		// ��� ���� �ð��̸�
		if(CGameTimeMgr::GetInstance().IsGuildWarTime())
		{
			// �����Ѵ�.
		}
		else
		{
			// edith 2008.08.18 ���� �����̸� �����Ѵ�. ���� �� ��Ÿ ��Ȳ�� ����
			if(lpWinnerCharacter->GetRace() == lpLoserCharacter->GetRace())
			{
				cWinnerCmd = PktFIAck::FAME_WIN;
				cLoserCmd = PktFIAck::FAME_LOSE;
			}
		}
	}
	else
	{
		// edith 2008.08.18 ���� �����̸� �����Ѵ�. ���� �� ��Ÿ ��Ȳ�� ����
		if(lpWinnerCharacter->GetRace() == lpLoserCharacter->GetRace())
		{
			cWinnerCmd = PktFIAck::FAME_WIN;
			cLoserCmd = PktFIAck::FAME_LOSE;
		}
	}


	// ��� ����
	if (lpLoserCharacter->GetDuelOpponent() == lpWinnerCharacter)
	{
		cWinnerCmd = PktFIAck::FAME_WIN_DUEL;
		cLoserCmd = PktFIAck::FAME_LOSE_DUEL;
	}

	// ����Ʋ ����
	CCharacterParty* lpLoserParty = reinterpret_cast<CCharacterParty*>(lpLoserCharacter->GetParty());
	CCharacterParty* lpWinnerParty = reinterpret_cast<CCharacterParty*>(lpWinnerCharacter->GetParty());
	if (NULL != lpLoserParty && NULL != lpWinnerParty)
	{
		if (lpLoserParty->GetHostileParty() == lpWinnerParty)
		{
			cWinnerCmd = PktFIAck::FAME_WIN_DUEL;
			cLoserCmd = PktFIAck::FAME_LOSE_DUEL;
		}
	}

	// edith 2009.12.26 ���� 20�̻��̸� �׾����� ��ó���� ���� �ʴ´�.
	// ���� ���ڿ� ������ ������ 20������ ������ ��ó���� ���� �ʴ´�
	if (NULL == lpWinnerParty)
	{
		int iLevelGap = lpWinnerCharacter->GetLevel() - lpLoserCharacter->GetLevel();
		if(iLevelGap >= 20)
		{
			cWinnerCmd = PktFIAck::FAME_WIN_LEVELMIN;
			cLoserCmd = PktFIAck::FAME_LOSE_LEVELMIN;
		}
	}

	GAMELOG::ERRType eReturnCode = 0;
	// -----------------------------------------------------------------------------------------------------
	// ���� ���� ��Ģ

	if (PktFIAck::FAME_LOSE == cLoserCmd)
	{
		unsigned long dwPrevFame = lpLoserCharacter->GetFame();
		unsigned long lReduceFame = std::min(static_cast<unsigned long>(5), lpLoserCharacter->GetFame());//was (10)
		unsigned long dwNextFame = dwPrevFame - lReduceFame;
		lpLoserCharacter->SetFame(dwNextFame);

		GAMELOG::LogFameLoseBattle(*lpLoserCharacter, lpWinnerCharacter, 
			lpLoserParty, dwPrevFame, dwNextFame, eReturnCode);

		CCharacterParty* lpLoserParty = reinterpret_cast<CCharacterParty *>(lpLoserCharacter->GetParty());
		if (NULL != lpLoserParty)
		{
			lpLoserParty->SendDivisionFame(lpWinnerCharacter, lpLoserCharacter, cLoserCmd, 0);
		}
	}

	lpDispatch = lpLoserCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpLoserCharacter, 
			lpWinnerCharacter->GetCharacterName(), lpLoserCharacter->GetCharacterName(),
			lpWinnerCharacter->GetNation(), static_cast<unsigned char>(lpWinnerCharacter->GetClass()),
			cLoserCmd, PktBase::NO_SERVER_ERR);
	}

	// -----------------------------------------------------------------------------------------------------
	// START SYSTEM FAME EWARD + PACK EACH KILL BY AVALANSA

	time_t t = time(0);   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT
    struct tm * now = localtime( & t );   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT

	unsigned long multiplier = KillSpreeMultiplier(lpWinnerCharacter,lpLoserCharacter); 
	
	lpWinnerCharacter->m_dwLastKillTime=timeGetTime();

	unsigned long dwAcquireAmount = ((10 + multiplier * 5) -5); /// ACI DE LUNI PANA JOI +20 FAME //w as ((10 + multiplier * 5) -5);
           //if weekend (0 = sunday , 6 =saturday)
			if (now->tm_wday ==0 || now->tm_wday==6){   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT
				dwAcquireAmount = 20;   /////// <------- AVALANSA SAMBATA DUMINICA  100% FAME EVENT  +40 FAME // was dwAcquireAmount = ((20 + multiplier * 5) -5);
			} //// ADD BY AVALANSA

	// FINIS SYSTEM FAME EWARD + PACK EACH KILL BY AVALANSA
    // -----------------------------------------------------------------------------------------------------

	// ��Ƽ�� �ƴҶ�
	if (NULL == lpWinnerParty)
	{
		if((cWinnerCmd != PktFIAck::FAME_WIN_CONTINUE)&&(cWinnerCmd != PktFIAck::FAME_WIN_DUEL)&&(cWinnerCmd != PktFIAck::FAME_WIN_LEVELMIN))
		{


		    time_t t = time(0);   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT
            struct tm * now = localtime( & t );   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT



			Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(9226); //AVALANSA ADD THIS FOR PVP REWARD // CODE ITEMS
			lpWinnerCharacter->GiveItem(lpItem); //AVALANSA ADD THIS FOR PVP REWARD


		
			if (now->tm_wday ==0 || now->tm_wday==6){   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT
				
            Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(65103);//AVALANSA ADD THIS FOR PVP REWARD // CODE ITEMS
			lpWinnerCharacter->GiveItem(lpItem);  //AVALANSA ADD THIS FOR PVP REWARD

			} //// ADD BY AVALANSA

			if (now->tm_wday ==0 || now->tm_wday==6){   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT
				
            Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(65107);//AVALANSA ADD THIS FOR PVP REWARD // CODE ITEMS
			lpWinnerCharacter->GiveItem(lpItem);  //AVALANSA ADD THIS FOR PVP REWARD

			} //// ADD BY AVALANSA

			if (now->tm_wday ==0 || now->tm_wday==6){   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT
				
            Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(65111);//AVALANSA ADD THIS FOR PVP REWARD // CODE ITEMS
			lpWinnerCharacter->GiveItem(lpItem);  //AVALANSA ADD THIS FOR PVP REWARD

			} //// ADD BY AVALANSA

            if (now->tm_wday ==0 || now->tm_wday==6){   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT
				
            Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(65115);//AVALANSA ADD THIS FOR PVP REWARD // CODE ITEMS
			lpWinnerCharacter->GiveItem(lpItem);  //AVALANSA ADD THIS FOR PVP REWARD

			} //// ADD BY AVALANSA

			if (now->tm_wday ==0 || now->tm_wday==6){   /////// AVALANSA SAMBATA DUMINICA  100% FAME EVENT
				
            Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(65119);//AVALANSA ADD THIS FOR PVP REWARD // CODE ITEMS
			lpWinnerCharacter->GiveItem(lpItem);  //AVALANSA ADD THIS FOR PVP REWARD

			} //// ADD BY AVALANSA


			bool stole=false;
			int currentWinnerSpree = lpWinnerCharacter->getKillSpreeLevel();
			currentLoserSpree/=2;
			if( currentLoserSpree > currentWinnerSpree)
			{
				currentWinnerSpree = currentLoserSpree;
				stole=true;
			}
			else
				currentWinnerSpree+=1;

			unsigned long KillCountLevel = static_cast<unsigned long>(currentWinnerSpree);
			changed=true;
			lpLoserCharacter->setKillSpreeLevel(0);//hz added for killspreelevel
			lpWinnerCharacter->setKillSpreeLevel(currentWinnerSpree);//hz added for killspreelevel
		//	dwAcquireAmount=dwAcquireAmount * + KillCountLevel;
			dwAcquireAmount=dwAcquireAmount + KillCountLevel;
			if(!stole)
			{
				switch(currentWinnerSpree)
				{
				case 2: // was 5
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 3: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 4: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 5: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 6: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff4, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 7: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff5, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 8: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff6, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				default:
					break;

				}
			}
			else
			{
				switch(currentWinnerSpree)
				{
				case 2: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 3: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 4: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 5: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 6: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff4, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 7: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff4, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff5, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 8: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff4, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff5, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff6, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				default:
					break;

				}
			}
		}
		

		unsigned long dwPrevFame = lpWinnerCharacter->GetFame();
		unsigned long dwNextFame = 0;
		unsigned long dwPrevMileage = 0;
		unsigned long dwNextMileage = 0;

		if (PktFIAck::FAME_WIN == cWinnerCmd)
		{
			// ���� ���� ó��

			time_t t = time(0);   // get time now
			struct tm * now = localtime( & t );

			//unsigned long multiplier = KillSpreeMultiplier(lpWinnerCharacter,lpLoserCharacter); 

			//lpWinnerCharacter->m_dwLastKillTime=timeGetTime();

			unsigned long dwGetFame = std::min(dwAcquireAmount * static_cast<unsigned long>(CServerSetup::GetInstance().GetFameDefault() / 500.0f), 
				UINT_MAX - lpWinnerCharacter->GetFame());
			dwNextFame = dwPrevFame + (dwGetFame);
			unsigned long dwGetMedal = lpWinnerCharacter->GetMileage();
			float add=10;
			//if weekend (0 = sunday , 6 =saturday)
			if (now->tm_wday ==0 || now->tm_wday==6){
				add*= 2.0; // increase by 100%
			}
			unsigned long dwNextMedal = dwGetMedal + add;

			lpWinnerCharacter->SetFame(dwNextFame);
			lpWinnerCharacter->SetMileage(dwNextMedal);
		}

		if (PktFIAck::FAME_WIN == cWinnerCmd || PktFIAck::FAME_WIN_LEVELMIN == cWinnerCmd )
		{
			// �޴޿� ���� ó��
			dwPrevMileage = lpWinnerCharacter->GetMileage();
			dwNextMileage = dwPrevMileage;

			time_t t = time(0);   // get time now
			struct tm * now = localtime( & t );

			// edith 2009.06.27 �޴��� ������, ��������� ���� ���϶��� ��´�.
			if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
			{
				// ��� ���� �ð��̸�
				if (CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					unsigned long dwGetMileage = std::min(dwAcquireAmount * static_cast<unsigned long>(CServerSetup::GetInstance().GetMileageDefault() / 500.0f), 
						UINT_MAX - lpWinnerCharacter->GetMileage());
					//if weekend (0 = sunday , 6 =saturday)
					if (now->tm_wday ==0 || now->tm_wday==6){
						dwGetMileage = dwGetMileage * 2.0; // increase by 50%
					}
					dwNextMileage = dwPrevMileage + dwGetMileage;
				}
			}
			else if (/*SERVER_ID::STONE_WAR1*/
/*SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
			{
				// ���� ���� �ð��̸�
				if (CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					unsigned long dwGetMileage = std::min(dwAcquireAmount * static_cast<unsigned long>(CServerSetup::GetInstance().GetMileageDefault() / 500.0f), 
						UINT_MAX - lpWinnerCharacter->GetMileage());
					//if weekend (0 = sunday , 6 =saturday)
					if (now->tm_wday ==0 || now->tm_wday==6){
						dwGetMileage = dwGetMileage * 2.0; // increase by 50% medal shower
					}
					dwNextMileage = dwPrevMileage + dwGetMileage;
				}
			}

			lpWinnerCharacter->SetMileage(dwNextMileage);

			GAMELOG::LogFameGetBattle(*lpWinnerCharacter, lpLoserCharacter, lpWinnerParty, 
				dwPrevFame, dwNextFame, dwPrevMileage, dwNextMileage, eReturnCode);
		}

		lpDispatch = lpWinnerCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpWinnerCharacter, 
				lpWinnerCharacter->GetCharacterName(), lpLoserCharacter->GetCharacterName(),
				lpLoserCharacter->GetNation(), (unsigned char)lpLoserCharacter->GetClass(),
				cWinnerCmd, PktBase::NO_SERVER_ERR);
		}
	}
	else
	{
		if((cWinnerCmd != PktFIAck::FAME_WIN_CONTINUE)&&(cWinnerCmd != PktFIAck::FAME_WIN_DUEL)&&(cWinnerCmd != PktFIAck::FAME_WIN_LEVELMIN))
		{
			unsigned long dwGold=0;
			bool stole=false;
			Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(9226);
			CParty* lpParty = lpWinnerCharacter->GetParty();
			if(lpParty)
			{
				lpParty->AutoRouting(lpWinnerCharacter, lpItem, dwGold);
			}
			int currentWinnerSpree = lpWinnerCharacter->getKillSpreeLevel();
			if(!changed)
				currentLoserSpree/=2;

			if( currentLoserSpree > currentWinnerSpree)
			{
				stole=true;
				currentWinnerSpree = currentLoserSpree;
			}
			else
				currentWinnerSpree+=1;

			lpWinnerCharacter->setKillSpreeLevel(currentWinnerSpree);//hz added for killspreelevel
			lpLoserCharacter->setKillSpreeLevel(0);//hz added for killspreelevel
			unsigned long KillCountLevel = static_cast<unsigned long>(currentWinnerSpree);
			dwAcquireAmount=dwAcquireAmount + KillCountLevel;
			
			if(!stole)
			{
				switch(currentWinnerSpree)
				{
				case 2: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 3: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 4: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 5: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 6: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff4, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 7: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff5, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 8: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff6, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				default:
					break;

				}
			}
			else
			{
				switch(currentWinnerSpree)
				{
				case 2: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 3: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 4: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 5: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 6: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff4, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 7: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff4, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff5, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				case 8: // was
					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff1, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff2, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff3, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff4, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff5, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);

					Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWinnerCharacter, 
						Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::KillStreakBuff6, currentWinnerSpree, CSpell::INFINITE_DURATION))(lpWinnerCharacter);
					break;

				default:
					break;

				}
			}
		}

		// ��Ƽ�϶�
		lpWinnerParty->SendDivisionFame(lpWinnerCharacter, lpLoserCharacter, 
			cWinnerCmd, static_cast<unsigned char>(dwAcquireAmount));
	}
}*/


void BattleInclination::SetCharData(CCreature& creature, BattleInclination::CharData& charData, bool bSetRideArmsInfo)
{
	charData.m_dwCID			= creature.GetCID();          // ĳ���� CID
	charData.m_dwGID			= creature.GetGID();          // ĳ���� GID    
	charData.m_dwCastleID		= 0;                          // ���� ������ ���, ���� ���� ID�� ����.
	charData.m_cNation			= creature.GetNation();       // ĳ���� ����
	charData.m_cSiegeState		= 0;                          // ���� ������ ���, ���� ���� ���¸� ����.
	charData.m_cFlags			= 0;                          // ��Ÿ ����
	charData.m_cAdminLevel		= 0;
	charData.m_dwPID			= 0;
	charData.m_wObjectType		= 0;
	charData.m_wKind			= 0;
	charData.m_cCreatureType	= Creature::GetCreatureType(creature.GetCID());	
	charData.m_cRealmWarFlag	= 0;
	charData.m_cGuildWarFlag	= 0;
	charData.m_cTacticsFlag		= 0;
	charData.m_cRide			= creature.IsRideArms();

	// ��� ���� ����ڴ� GID �� 0 �� ����.
	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( charData.m_dwGID );
	if ( lpGuild )
	{
		if(Creature::GetCreatureType(charData.m_dwCID)==Creature::CT_PC)
		{
			CCharacter&		character		= static_cast<CCharacter&>(creature);
			unsigned char	cTacticsFlag	= character.GetTacticsFlag();

			if (Guild::JOIN_WAIT == lpGuild->GetTitle( charData.m_dwCID ))
			{
				charData.m_dwGID		= 0;				
			}			

			charData.m_cTacticsFlag	= cTacticsFlag;
		}
	}

	CParty* lpParty = creature.GetParty();
	if (lpParty)
	{
		charData.m_dwPID		= lpParty->GetUID();		
	}

	charData.SetPeaceMode(creature.IsPeaceMode());
	charData.SetSafetyZone(CCellManager::GetInstance().IsSafetyZone(creature.GetCurrentPos()));
	charData.SetGODMode(creature.GetEnchantInfo().GetFlag(Skill::SpellID::Invincible));

	bool bHasDevelopingCamp, bHasCompleteCamp;
	CSiegeObjectMgr::GetInstance().HasCampByGID(charData.m_dwGID, bHasDevelopingCamp, bHasCompleteCamp);
	charData.SetHasDevelopingCamp(bHasDevelopingCamp);
	charData.SetHasCamp(bHasCompleteCamp);

	switch (Creature::GetCreatureType(charData.m_dwCID))
	{
	case Creature::CT_SIEGE_OBJECT:
		{
			CSiegeObject& siegeObject = static_cast<CSiegeObject&>(creature);

			charData.m_dwGID = siegeObject.GetGID();
			charData.SetCastleObject(siegeObject.IsCastleObject());
			charData.SetCastleNPC(siegeObject.IsCastleNPC());
			charData.m_dwCastleID	= siegeObject.GetCastleID();
			charData.m_cSiegeState	= siegeObject.GetState();
			charData.m_wObjectType	= siegeObject.GetObjectType();

			// ��� ����� ������ ����� �������� �����̴�.
			charData.m_cGuildWarFlag = Creature::WAR_ON ;
		}
		break;

	case Creature::CT_PC:
		{
			CCharacter& character		= static_cast<CCharacter&>(creature);
			charData.m_cAdminLevel		= character.GetAdminLevel();
			charData.m_cRealmWarFlag	= character.GetRealmWarFlag();
			charData.m_cGuildWarFlag	= character.GetGuildWarFlag();

			// ����Ʋ ���� (���� ��尣 ��Ʋ�� �߰��Ǹ� ���� �ʿ�)
			if (NULL != character.GetParty())
			{
				if (NULL != reinterpret_cast<CCharacterParty*>(character.GetParty())->GetHostileParty())
				{
					charData.SetTeamBattle(true);
				}
			}

			if(bSetRideArmsInfo && character.IsRideArms())
			{
				CSiegeObject* lpSiegeObject = 
					CSiegeObjectMgr::GetInstance().GetSiegeObject(character.GetRideArmsCID());

				if(lpSiegeObject)
				{
					charData.m_dwGID = lpSiegeObject->GetGID();
					charData.SetCastleObject(lpSiegeObject->IsCastleObject());
					charData.SetCastleNPC(lpSiegeObject->IsCastleNPC());
					charData.m_dwCastleID	= lpSiegeObject->GetCastleID();
					charData.m_cSiegeState	= lpSiegeObject->GetState();
					charData.m_wObjectType	= lpSiegeObject->GetObjectType();					
				}
			}
		}
		break;

	case Creature::CT_MONSTER:
	case Creature::CT_SUMMON:
	case Creature::CT_STRUCT:
		{
			CMonster& monster = static_cast<CMonster&>(creature);

			charData.m_wKind = monster.GetKID();
		}
		break;
	}
}

void BattleInclination::SetRelationData(CCreature& ownerCreature, CCreature& targetCreature, 
	BattleInclination::RelationData& relationData)
{
	relationData.m_cVSFlag			= 0;
	relationData.m_bHostileGuild	= false;

	if (Creature::CT_PC == Creature::GetCreatureType(ownerCreature.GetCID()) && 
		Creature::CT_PC == Creature::GetCreatureType(targetCreature.GetCID()))
	{
		CCharacter& ownerCharacter	= static_cast<CCharacter&>(ownerCreature);
		CCharacter& targetCharacter	= static_cast<CCharacter&>(targetCreature);

		if (NULL != ownerCharacter.GetDuelOpponent())
		{
			if (ownerCharacter.GetDuelOpponent()->GetCID() == targetCharacter.GetCID())
			{
				relationData.m_cVSFlag |= BattleInclination::VS_CHARACTER;
			}
		}

		if (NULL != ownerCharacter.GetParty() && NULL != targetCharacter.GetParty())
		{
			CCharacterParty* lpHostileParty = 
				reinterpret_cast<CCharacterParty* >(ownerCharacter.GetParty())->GetHostileParty();

			if (NULL != lpHostileParty)
			{
				if (lpHostileParty->GetUID() == targetCharacter.GetParty()->GetUID())
				{
					relationData.m_cVSFlag |= BattleInclination::VS_PARTY;
				}
			}
		}
	}

	// Rodin : ��尣 ��Ʋ ����
	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(ownerCreature.GetGID());
	if (NULL != lpGuild)
	{
		if (true == lpGuild->IsEnemyGuild(targetCreature.GetGID()))
		{			
			relationData.m_bHostileGuild = true;
		}
	}
	lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(targetCreature.GetGID());
	if (NULL != lpGuild)
	{
		if (true == lpGuild->IsEnemyGuild(ownerCreature.GetGID()))
		{			
			relationData.m_bHostileGuild = true;
		}
	}
}


EnemyCheck::EnemyType CCharacter::IsEnemy(CCreature* lpTarget, unsigned char* lpResult)
{
	if (NULL == lpTarget)
	{
		ERRLOG1(g_Log, "CID:0x%08x �Ǿƽĺ��� Ÿ���� �����ϴ�.", m_dwCID);
		return EnemyCheck::EC_NEUTRAL;
	}

	BattleInclination::CharData     ownerInfo;
	BattleInclination::CharData     targetInfo;

	BattleInclination::SetCharData(*this, ownerInfo);
	BattleInclination::SetCharData(*lpTarget, targetInfo);

	BattleInclination::RelationData	relationInfo;
	BattleInclination::SetRelationData(*this, *lpTarget, relationInfo);

	unsigned char cResultFromStruct = 0;
	unsigned long dwResult = EnemyCheck::CCheckTable::GetInstance().IsEnemyFromStruct(
		ownerInfo, targetInfo, relationInfo, cResultFromStruct);

	if (NULL != lpResult)
	{
		*lpResult = cResultFromStruct;
	}
	return static_cast<EnemyCheck::EnemyType>(dwResult);
}


void CCharacter::SendAttackedToParty(unsigned long dwAttackerID, AtType atType, unsigned short wDamage, unsigned char cDefenserJudge, unsigned short wMPHeal)
{
	if (0 != m_pParty)
	{
		bool bLastSendTimeCheck = true;

		if(atType.m_wType & AtType::SKILL_BIT)
		{
			// (�Ϲݰ����̳� Ÿ�� Ÿ���� Enemy, Melee�� ��ų�� ���� ��� - �ð� üũ)
			// Friend, Frexme, Frdead, Party �ð� üũ ���� ����.
			const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(atType.m_wType);
			if(0 != lpProtoType)
			{
				switch(lpProtoType[atType.m_cSkillLockCount].m_eTargetType)
				{
				case Skill::Target::FRIEND:
				case Skill::Target::FRIEND_EXCEPT_SELF:
				case Skill::Target::DEAD_FRIEND:
				case Skill::Target::PARTY:

					bLastSendTimeCheck = false;
					break;
				}
			}
		}

		unsigned long dwCurrentTime = 0;
		if(!bLastSendTimeCheck || 500 <= (dwCurrentTime = timeGetTime()) - m_dwLastSendPartyAttackInfoTime)
		{
			m_dwLastSendPartyAttackInfoTime = dwCurrentTime;

			CCharacterParty* lpParty = static_cast<CCharacterParty*>(m_pParty);

			if(!lpParty)
			{
				return;
			}

			const int   MAX_BUFFER = sizeof(PktAtAck) + sizeof(DefenserNode);
			char szPacket[MAX_BUFFER];

			unsigned short  wPacketSize = sizeof(PktAtInfo) + sizeof(DefenserNode);

			PktAtInfo*		lpPktAtInfo		= reinterpret_cast<PktAtInfo*>(szPacket);
			DefenserNode*	lpDefenserNode	= reinterpret_cast<DefenserNode*>(lpPktAtInfo + 1);

			lpPktAtInfo->m_dwCharID			= dwAttackerID;
			lpPktAtInfo->m_AtType			= atType;
			lpPktAtInfo->m_cDefenserNum		= 1;

			lpDefenserNode->m_dwCharID      = m_dwCID;     // ����� ĳ���� ���̵�

			lpDefenserNode->m_wMaxHP        = m_CreatureStatus.m_StatusInfo.m_nMaxHP;	// �ִ� HP
			lpDefenserNode->m_sCurrHP       = m_CreatureStatus.m_nNowHP;		        // ���� HP
			lpDefenserNode->m_wMaxMP        = m_CreatureStatus.m_StatusInfo.m_nMaxMP;	// �ִ� MP
			lpDefenserNode->m_sCurrMP       = m_CreatureStatus.m_nNowMP;                // ���� MP

			lpDefenserNode->m_wMPHeal		= wMPHeal;			// MP ȸ����
			lpDefenserNode->m_wDamage       = wDamage;          // ������ ��
			lpDefenserNode->m_cJudge        = cDefenserJudge;   // ���� ��

			if (PacketWrap::WrapCrypt(szPacket, wPacketSize, CmdCharAttackInfo, 0, 0))
			{
				// �ֺ� ������ ���� ������, �ָ� ������ ��Ƽ ������Ը� ������ �ȴ�.
				lpParty->SendNotNearCellMember(szPacket, wPacketSize, m_CellPos.m_lpCell, CmdCharAttackInfo);
			}
		}
	}
}

void CCharacter::SkillClear()
{
	CAffectedSpell& cAffectedInfo = m_SpellMgr.GetAffectedInfo();
	cAffectedInfo.ClearAll();	

	CCastingSpell& cCastingInfo = m_SpellMgr.GetCastingInfo();
	cCastingInfo.ClearAll();		
}