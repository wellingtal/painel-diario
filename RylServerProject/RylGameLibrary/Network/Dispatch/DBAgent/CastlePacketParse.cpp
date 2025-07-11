
#include "stdafx.h"

#include "CastlePacketParse.h"
#include "DBAgentDispatch.h"

#include <Utility/Setup/ServerSetup.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/Camp.h>
#include <Creature/Siege/MiningCamp.h>
#include <Creature/Siege/CampShop.h>
#include <Creature/Siege/WorldWeapon.h>
#include <Creature/Siege/CastleGate.h>
#include <Creature/Siege/CastleArms.h>
#include <Creature/Siege/SiegeArms.h>
#include <Creature/Siege/SiegeObjectMgr.h>
#include <Creature/Character/CharRespawnMgr.h>
#include <Creature/EnemyCheck.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Map/FieldMap/MineralVeinMgr.h>

#include <Skill/Spell/Spell.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/GlobalSpellMgr.h>
#include <Skill/Spell/SpellKind.h>
#include <Skill/Spell/SpellUtil.h>
#include <Skill/SkillStructure.h>
#include <Skill/SkillTable.h>

using namespace Castle;

// �� ������ ������Ʈ
bool DBAgentPacketParse::ParseCastleInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	// edith ���� (������ �κ�)
	PktCastleInfo* lpPktCastleInfo = static_cast<PktCastleInfo*>(lpPktBase);

	unsigned char cCastleObjectNum = lpPktCastleInfo->m_cCastleObjectNum;
	unsigned short wSize = lpPktCastleInfo->m_wSize;

	if (lpPktCastleInfo->m_bStartFlag)
	{
		Castle::CCastleMgr::GetInstance().Destroy();
	}

	Castle::CCastleMgr::GetInstance().SerializeIn(reinterpret_cast<char*>(lpPktCastleInfo + 1),	wSize, cCastleObjectNum);
	return true;
}

// ��� ��� ������ ������Ʈ
bool DBAgentPacketParse::ParseCampInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCampInfo* lpPktCampInfo = static_cast<PktCampInfo*>(lpPktBase);

	unsigned short wSize = lpPktCampInfo->m_wSize;
	unsigned long dwCID = lpPktCampInfo->m_dwCID;

	if (lpPktCampInfo->m_bStartFlag)
	{
		CSiegeObjectMgr::GetInstance().DestroyAllCamp();
	}

	CSiegeObjectMgr::GetInstance().SerializeIn(reinterpret_cast<char*>(lpPktCampInfo + 1), wSize, dwCID);

	return true;
}

// ���� ������ ������Ʈ
bool DBAgentPacketParse::ParseFertilityInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktFertilityInfo* lpPktFertilityInfo = static_cast<PktFertilityInfo*>(lpPktBase);

	unsigned short wSize = lpPktFertilityInfo->m_wSize;
	unsigned short wNum = lpPktFertilityInfo->m_wNum;

	CMineralVeinMgr::GetInstance().SetNowFertility(wNum, reinterpret_cast<unsigned long*>(lpPktFertilityInfo+1));
	return true;
}

// ���� ���� ������Ʈ
bool DBAgentPacketParse::ParseWorldWeaponInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktWorldWeaponInfo* lpPktWeaponInfo = static_cast<PktWorldWeaponInfo* >(lpPktBase);

	CSiegeObjectMgr::GetInstance().SetWorldWeaponInfo(lpPktWeaponInfo->m_cKarRemainSiegeTime, lpPktWeaponInfo->m_cMerRemainSiegeTime);
	return true;
}

// ���� �������� ��� ���
bool DBAgentPacketParse::ParseCreateCamp(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCreateCamp* lpPktCreateCamp = static_cast<PktCreateCamp*>(lpPktBase);
	CSiegeObject* lpCamp = NULL;

	// ������ �����ߴٸ�...
	if (PktCreateCamp::ERROR_EXIST_ANOTHER_WEAPON == lpPktCreateCamp->GetError())
	{
		// ���ÿ� �ΰ��� ���� ó���� ���� ���ؼ�...
		CSiegeObjectMgr::GetInstance().ReleaseWorldWeaponCreation();
	}

	switch ( lpPktCreateCamp->m_wObjectType )
	{
	case Siege::CAMP:
		{
			lpCamp = CSiegeObjectMgr::GetInstance().CreateCamp(lpPktCreateCamp->m_dwCID, lpPktCreateCamp->m_dwCampID, lpPktCreateCamp->m_dwGID,
															   lpPktCreateCamp->m_dwHP, lpPktCreateCamp->m_cState, lpPktCreateCamp->m_cUpgradeStep,
															   CampRight(), lpPktCreateCamp->m_Pos, lpPktCreateCamp->m_cMaterial, true);
		}
		break;

	case Siege::KARTERANT_WEAPON:
	case Siege::MERKADIA_WEAPON:
		{
			if (NULL == CSiegeObjectMgr::GetInstance().GetWorldWeapon())
			{
				lpCamp = CSiegeObjectMgr::GetInstance().CreateWorldWeapon(lpPktCreateCamp->m_dwCID, lpPktCreateCamp->m_dwCampID, lpPktCreateCamp->m_dwGID,
																		lpPktCreateCamp->m_dwHP, lpPktCreateCamp->m_cState, lpPktCreateCamp->m_cUpgradeStep,
																		CampRight(), lpPktCreateCamp->m_Pos, lpPktCreateCamp->m_cMaterial, true,
																		lpPktCreateCamp->m_wObjectType);

				// ���ÿ� �ΰ��� ���� ó���� ���� ���ؼ�...
				CSiegeObjectMgr::GetInstance().ReleaseWorldWeaponCreation();
			}
		}
		break;
	}
	
	if (lpCamp)
	{
        const int MAX_BUFFER = sizeof(PktCreateCamp);
		char szBuffer[MAX_BUFFER];

		PktCreateCamp* lpPktCC = reinterpret_cast<PktCreateCamp*>(szBuffer);

		lpPktCC->m_dwActorCID	= lpPktCreateCamp->m_dwActorCID;
		lpPktCC->m_dwCID		= lpCamp->GetCID();
		lpPktCC->m_dwCampID		= lpCamp->GetCampID();
		lpPktCC->m_dwGID		= lpCamp->GetGID();
		lpPktCC->m_dwHP			= lpCamp->GetNowHP();
		lpPktCC->m_wObjectType	= lpCamp->GetObjectType();
		lpPktCC->m_cZone		= CServerSetup::GetInstance().GetServerZone();
		lpPktCC->m_cChannel		= CServerSetup::GetInstance().GetServerChannel();
		lpPktCC->m_cState		= lpCamp->GetState();
		lpPktCC->m_cSubState	= lpCamp->GetSubState();
		lpPktCC->m_cUpgradeStep	= lpCamp->GetUpgradeStep();
		lpPktCC->m_CampRight	= reinterpret_cast<CCamp*>(lpCamp)->GetCampRight();
		lpPktCC->m_Pos.fPointX	= lpCamp->GetPosition().m_fPointX;
		lpPktCC->m_Pos.fPointY	= lpCamp->GetPosition().m_fPointY;
		lpPktCC->m_Pos.fPointZ	= lpCamp->GetPosition().m_fPointZ;
		lpPktCC->m_cMaterial	= lpCamp->GetMaterialNum();

		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCamp->GetGID() );
		if (lpGuild)
		{
			lpPktCC->m_cNation	= lpGuild->GetNation();
		}
		else
		{
			// ��� ������ ���� �� ���ﱹ������ �ӽ� ���
			lpPktCC->m_cNation	= Creature::ALMIGHTY_PIRATE;
		}

		if (true == PacketWrap::WrapCrypt(szBuffer, MAX_BUFFER, CmdCreateCamp, 0 ,0))
		{
			CCreatureManager::GetInstance().SendAllCharacter(szBuffer, MAX_BUFFER, CmdCreateCamp);
		}
	}

	return true;
}

// ���� ������ ���� ����
bool DBAgentPacketParse::ParseCreateSiegeArms(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCreateSiegeArms* lpPktCreateSiegeArms = static_cast<PktCreateSiegeArms*>(lpPktBase);

	CSiegeObject* lpSiegeAmrs = CSiegeObjectMgr::GetInstance().CreateSiegeArms(lpPktCreateSiegeArms->m_dwCID, lpPktCreateSiegeArms->m_dwOwnerID,
																			   lpPktCreateSiegeArms->m_cNation, lpPktCreateSiegeArms->m_dwHP,
																			   lpPktCreateSiegeArms->m_wObjectType, lpPktCreateSiegeArms->m_cState,
																			   lpPktCreateSiegeArms->m_cUpgradeStep, lpPktCreateSiegeArms->m_Pos);

	if (lpSiegeAmrs)
	{
		const int MAX_BUFFER = sizeof(PktCreateSiegeArms);
		char szBuffer[MAX_BUFFER];

		PktCreateSiegeArms* lpPktCSA = reinterpret_cast<PktCreateSiegeArms*>(szBuffer);

		lpPktCSA->m_dwCID			= lpSiegeAmrs->GetCID();
		lpPktCSA->m_dwOwnerID		= lpSiegeAmrs->GetOwnerID();
		lpPktCSA->m_cNation			= lpSiegeAmrs->GetNation();
		lpPktCSA->m_wObjectType		= lpSiegeAmrs->GetObjectType();
		lpPktCSA->m_dwHP			= lpSiegeAmrs->GetNowHP();
		lpPktCSA->m_cState			= lpSiegeAmrs->GetState();
		lpPktCSA->m_cUpgradeStep	= lpSiegeAmrs->GetUpgradeStep();
		lpPktCSA->m_cZone			= CServerSetup::GetInstance().GetServerZone();
		lpPktCSA->m_cChannel		= CServerSetup::GetInstance().GetServerChannel();
		lpPktCSA->m_Pos.fPointX		= lpSiegeAmrs->GetPosition().m_fPointX;
		lpPktCSA->m_Pos.fPointY		= 0.0f;
		lpPktCSA->m_Pos.fPointZ		= lpSiegeAmrs->GetPosition().m_fPointZ;

		if (true == PacketWrap::WrapCrypt(szBuffer, MAX_BUFFER, CmdCreateSiegeArms, 0 ,0))
		{
			// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
			//lpSiegeAmrs->SendToRadiusCell(szBuffer, MAX_BUFFER, CmdCreateSiegeArms);
			lpSiegeAmrs->SendToRange(Siege::BROADCAST_RADIUS, szBuffer, MAX_BUFFER, CmdCreateSiegeArms);
		}
	}

	return true;
}

// �� ���� ��� ��Ŷ
bool DBAgentPacketParse::ParseCastleCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCastleCmd* lpPktCastleCmd = static_cast<PktCastleCmd*>(lpPktBase);

	CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle(lpPktCastleCmd->m_dwCastleID);
	CSiegeObject* lpCastleObject = CSiegeObjectMgr::GetInstance().GetSiegeObject(lpPktCastleCmd->m_dwCastleObjectID);
	unsigned long dwCID = lpPktCastleCmd->m_dwCID;
	unsigned long dwValue1 = lpPktCastleCmd->m_dwValue1;
	unsigned long dwValue2 = lpPktCastleCmd->m_dwValue2;
	unsigned char cState = lpPktCastleCmd->m_cState;
	unsigned char cSubCmd = lpPktCastleCmd->m_cSubCmd;

	if (lpCastle && lpCastleObject)
	{
		switch (cSubCmd)
		{
			// ------------------------------------------------------------------------
			// �� ��¡��
			// [In/Out] dwValue1 : ���� Ÿ��
			// [In/Out] dwValue2 : ����
			case PktCastleCmd::CASTLE_SET_TAX:	// ���� ����
			{
				lpCastle->SetTax(static_cast<unsigned char>(dwValue1), static_cast<unsigned char>(dwValue2));
			}
			break;

			case PktCastleCmd::CASTLE_UPGRADE_EMBLEM:			// ��¡�� ���׷��̵�	[In/Out] dwValue1 : ���׷��̵� Ÿ��
			case PktCastleCmd::CASTLE_UPDATE_EMBLEM:			// �� ��¡�� �ı�		[In] dwValue1 : ��ȯ���� ��¡���� �Ʊ������� ���������� ����
																//						[Out] dwValue1 : ���� ��¡���� GID
			case PktCastleCmd::CASTLE_SUMMON_EMBLEM_COMPLETE:	// �� ��¡�� ��ȯ �Ϸ�	[In/Out] dwValue1 : �Ʊ��� ��ȯ�Ϸ�
			{
				lpCastleObject->Update(cState, dwValue1, 0, 0, cSubCmd);				
			}
			break;

			// [In/Out] dwValue1 : ���׷��̵� �ܰ�
			case PktCastleCmd::CASTLE_UPGRADE_EMBLEM_COMPLETE:	// ��¡�� ���׷��̵� �Ϸ�
			{
				lpCastleObject->Upgrade(static_cast<unsigned char>(dwValue1));
			}
			break;

			// [In/Out] dwValue1 : ���ο� ���� ���� ����
			case PktCastleCmd::CASTLE_CHANGE_MASTER:	// ���� ����
			{
				// ���� ����� ������ �˷��ش�.
				lpCastle->ChangeCastleMaster(static_cast<unsigned char>(dwValue1));

				// ������ ����Ʈ ���� ����
				CCharRespawnMgr::GetInstance().SetCastleRespawnPointNation(lpCastle->GetNameID(), static_cast<unsigned char>(dwValue1));

				// NPC ���� GID ����
				CCreatureManager::GetInstance().SetNationToCastleNPC(lpCastle->GetCastleID(), static_cast<unsigned char>(dwValue1));

				// ������ �� �α����� ��� �������� ������ �������� �̵���Ų��.
				if(CServerSetup::GetInstance().GetServerZone() == SERVER_ID::CAPITAL)
				{
					if(CGameTimeMgr::GetInstance().IsSiegeWarTime())
					{
						CCreatureManager::GetInstance().ProcessAllCharacter(SiegeMovePos);
					}
				}
			}
			break;

			// [In/Out] dwValue1 : ���� Ÿ��
			// [In/Out] dwValue2 : ȸ���� ����
			case PktCastleCmd::CASTLE_TAKE_TAXMONEY:	// ���� ȸ��
			{
				lpCastle->TakeTaxMoney(static_cast<unsigned char>(dwValue1), dwValue2);
			}
			break;

			case PktCastleCmd::CASTLE_LOSE_OWNERSHIP:	// �� ���� �߸�ȭ
			{
				lpCastle->LoseOwnership();

				// ������ ����Ʈ ����
				CCharRespawnMgr::GetInstance().SetCastleRespawnPointNation(lpCastle->GetNameID(), Creature::STATELESS);

				// NPC ���� GID ����
				CCreatureManager::GetInstance().SetNationToCastleNPC(lpCastle->GetCastleID(), Creature::STATELESS);
			}
			break;

			// [In/Out] dwValue1 : ����ID
			// [In] dwValue2 : ȹ�淮
			// [Out] dwValue2 : ����������
			case PktCastleCmd::CASTLE_GAIN_MINERAL:		// ���� ȹ��
			{
				unsigned short wMineralID = static_cast<unsigned short>(dwValue1);
				unsigned short wAmount = static_cast<unsigned short>(dwValue2);

				lpCastle->GainMineral(wMineralID, wAmount);
			}
			break;

			// ------------------------------------------------------------------------
			// ����
			// [Out] dwValue1 : SubState (OPENED)
			case PktCastleCmd::CASTLE_GATE_OPEN:	// ���� ����
			{
				if (Siege::GATE == lpCastleObject->GetObjectType())
				{
					reinterpret_cast<CCastleGate*>(lpCastleObject)->Open();
				}
			}
			break;

			// [Out] dwValue1 : SubState (CLOSED)
			case PktCastleCmd::CASTLE_GATE_CLOSE:	// ���� �ݱ�
			{
				if (Siege::GATE == lpCastleObject->GetObjectType())
				{
					reinterpret_cast<CCastleGate*>(lpCastleObject)->Close();
				}
			}
			break;

			case PktCastleCmd::CASTLE_UPGRADE_GATE:		// ���� ���׷��̵�	[In/Out] dwValue1 : ���׷��̵� ��� (����� ����)
			case PktCastleCmd::CASTLE_REPAIR_GATE:		// ���� ����		[In/Out] dwValue1 : ���� ���       (����� ����)
			case PktCastleCmd::CASTLE_RESTORE_GATE:		// ���� ����		[In/Out] dwValue1 : ���� ���       (����� ����)
			{
				lpCastleObject->Update(cState, dwValue1, 0, 0, cSubCmd);
			}
			break;

			// [In/Out] dwValue1 : ���׷��̵� �ܰ�
			case PktCastleCmd::CASTLE_UPGRADE_GATE_COMPLETE:	// ���� ���׷��̵� �Ϸ�
			{
				lpCastleObject->Upgrade(static_cast<unsigned char>(dwValue1));
			}
			break;

			// [In] dwValue1 : �����Ǿ� �߰��� HP
			// [Out] dwValue1 : ������ ���� HP (���� HP)
			case PktCastleCmd::CASTLE_REPAIR_GATE_COMPLETE:		// ���� ���� �Ϸ�
			{
				lpCastleObject->Repair(static_cast<unsigned short>(dwValue1));
			}
			break;

			// [In] dwValue1 : ������ HP ���������.. ���� 0 �̴�. (���� �ȵ�)
			// [Out] dwValue1 : ���� HP
			case PktCastleCmd::CASTLE_RESTORE_GATE_COMPLETE:	// ���� ���� �Ϸ�
			{
				lpCastleObject->Restore();
			}
			break;

			case PktCastleCmd::CASTLE_DESTROY_GATE:	// ���� �ı�
			{
				lpCastleObject->Destroy(0, false);
			}
			break;

			// ------------------------------------------------------------------------
			// ���� ����
			case PktCastleCmd::CASTLE_CREATE_ARMS:				// ���� ���� ����		[In/Out] dwValue1 : ���� ���		[In] dwValue2 : ���� 16��Ʈ : ����ð� / ���� 16��Ʈ : ���� Ÿ��	[Out] dwValue2 : ���� Ÿ��
			case PktCastleCmd::CASTLE_UPGRADE_ARMS:				// ���� ���� ���׷��̵�	[In/Out] dwValue1 : ���׷��̵� ���	[In/Out] dwValue2 : ���׷��̵� �ð� (��)
			case PktCastleCmd::CASTLE_REPAIR_ARMS:				// ���� ���� ����		[In/Out] dwValue1 : ���� ���		[In/Out] dwValue2 : ������ HP ��
			{
				lpCastleObject->Update(cState, dwValue1, dwValue2, dwCID, cSubCmd);
			}
			break;
			
			// [In/Out] dwValue1 : ���� Ÿ��
			case PktCastleCmd::CASTLE_CREATE_ARMS_COMPLETE:		// ���� ���� ���� �Ϸ�
			{
				if (lpCastleObject->IsCastleArms())
				{
					reinterpret_cast<CCastleArms*>(lpCastleObject)->ChangeType(static_cast<unsigned short>(dwValue1));
				}
			}
			break;

			// [In/Out] dwValue1 : ���׷��̵� �ܰ�
			case PktCastleCmd::CASTLE_UPGRADE_ARMS_COMPLETE:	// ���� ���� ���׷��̵� �Ϸ�
			{
				lpCastleObject->Upgrade(static_cast<unsigned char>(dwValue1));
			}
			break;

			// [In/Out] dwValue1 : ������ ȸ���� HP
            // [Out] dwValue2 : ������ ȸ���� ���� HP
			case PktCastleCmd::CASTLE_REPAIR_ARMS_COMPLETE:		// ���� ���� ���� �Ϸ�
			{
				lpCastleObject->Repair(static_cast<unsigned short>(dwValue1));
			}
			break;

			// [In] dwValue1 : 0 : �ı�, 1 : ������� NPC�� ����( ���� ��� ������ ������ )
			// [Out] dwValue1 : ���� ���� NPC Type ID
			case PktCastleCmd::CASTLE_DESTROY_ARMS:				// ���� ���� �ı�
			{
				lpCastleObject->AllGetOff();
				lpCastleObject->Destroy(0, (0 != dwValue1));
			}
			break;


			// [Out] dwValue1 : Full HP ( To DBAgentServer )
			case PktCastleCmd::CASTLE_GATE_FORCE_OPEN:			// ������ ������ ���� ���·� �����.
			{
				if (lpCastleObject->IsGate())
				{
					static_cast<CCastleGate* >(lpCastleObject)->ForceOpen();
				}
			}
			break;

			// [Out] dwValue1 : Full HP ( To DBAgentServer )
			case PktCastleCmd::CASTLE_GATE_FORCE_CLOSE:			// ������ ������ ���� ���·� �����.
			{
				if (lpCastleObject->IsGate())
				{
					static_cast<CCastleGate* >(lpCastleObject)->ForceClose();
				}
			}
			break;
		}
	}

	return true;
}

// ��� ��� ��� ��Ŷ
bool DBAgentPacketParse::ParseCampCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCampCmd* lpPktCampCmd = static_cast<PktCampCmd*>(lpPktBase);

	CCamp* lpCamp = CSiegeObjectMgr::GetInstance().GetCamp(lpPktCampCmd->m_dwCampID);
	unsigned char cState = lpPktCampCmd->m_cState;
	unsigned long dwValue1 = lpPktCampCmd->m_dwValue1;
	unsigned long dwValue2 = lpPktCampCmd->m_dwValue2;
	unsigned char cSubCmd = lpPktCampCmd->m_cSubCmd;

	if (lpCamp)
	{
		switch (cSubCmd)
		{
			// ��� ��� ���� �Ϸ�
			case PktCampCmd::CAMP_BUILD_COMPLETE:
			{
				lpCamp->Build();

				// ������ ����Ʈ �߰�
				CCharRespawnMgr::GetInstance().AddCampRespawnPoint(lpCamp->GetCampID(), lpCamp->GetGID(), lpCamp->GetPosition());
			}
			break;

			// ��� ��� ���� ������
			// ��� ��� ��ŸƮŶ���� ��ȯ��
			// ��� ��� ���׷��̵���
			// ��� ��� ������
			// ��� ��� ������
			case PktCampCmd::CAMP_CANCEL:
			case PktCampCmd::CAMP_TO_STARTKIT:
			case PktCampCmd::CAMP_UPGRADE:			// dwValue1[In] : ���׷��̵� �ܰ�
			case PktCampCmd::CAMP_REPAIR:			// dwValue1[In] : ������ ȸ���� HP
			case PktCampCmd::CAMP_CHANGE_TYPE:		// dwValue1[In] : ������ Ÿ��
			{
				lpCamp->Update(cState, dwValue1, dwValue2, 0, cSubCmd);
			}
			break;

			// ��� ��� ���� ���� �Ϸ�
			case PktCampCmd::CAMP_CANCEL_COMPLETE:
			{
				// ������ ����Ʈ ����
				CCharRespawnMgr::GetInstance().DeleteCampRespawnPoint(lpCamp->GetCampID());

				lpCamp->Cancel();
				CSiegeObjectMgr::GetInstance().DeleteCamp(lpCamp->GetCampID());
			}
			break;

			// ��� ��� ���׷��̵� �Ϸ�
			case PktCampCmd::CAMP_UPGRADE_COMPLETE:	// dwValue1[In] : ���׷��̵� �ܰ�
			{
				lpCamp->Upgrade(static_cast<unsigned char>(dwValue1));
			}
			break;

			// ��� ��� ���� �Ϸ�
			case PktCampCmd::CAMP_REPAIR_COMPLETE:	// dwValue1[In] : ������ ȸ���� HP
			{
				lpCamp->Repair(static_cast<unsigned short>(dwValue1));
			}
			break;

			// ��ŸƮŶ����
			case PktCampCmd::CAMP_TO_STARTKIT_COMPLETE:
			{
				// ������ ����Ʈ ����
				CCharRespawnMgr::GetInstance().DeleteCampRespawnPoint(lpCamp->GetCampID());

				lpCamp->Destroy( lpCamp->GetGID() );
				CSiegeObjectMgr::GetInstance().DeleteCamp( lpCamp->GetCampID() );
			}
			break;

			// ��� ��� �ı��� / 7�ϰ� ķ�� �̻��� �ı�
			case PktCampCmd::CAMP_DESTROY:			// dwValue1[In] : �ı��� ����� ID
			{
				// ������ ����Ʈ ����
				CCharRespawnMgr::GetInstance().DeleteCampRespawnPoint(lpCamp->GetCampID());

				lpCamp->Destroy( dwValue1 );
				CSiegeObjectMgr::GetInstance().DeleteCamp( lpCamp->GetCampID() );
			}
			break;

			// ��� ��� ���� �Ϸ�
			case PktCampCmd::CAMP_CHANGE_COMPLETE:	// dwValue1[In] : ������ Ÿ��
			{
				lpCamp->ChangeType( static_cast<unsigned short>(dwValue1) );

				CSiegeObjectMgr::GetInstance().ChangeCampType( lpCamp->GetCampID(), static_cast<unsigned short>(dwValue1) );
			}
			break;

			// ä���� ���� ����
			// dwValue1[In] : ä���� ���� On / Ready / Off
			case PktCampCmd::MINING_CAMP_ON_OFF:
			{
				CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp*>(lpCamp);
				if ( NULL != lpMiningCamp )
				{
					lpMiningCamp->OnOff( static_cast<unsigned char>(dwValue1) );
				}
				else
				{
					ERRLOG1(g_Log, "ä���� ĳ���� ���� CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// ä���� ���� ���� ȹ��
			// dwValue1[In] : ���� ���� ������ ���̵�
			// dwValue2[In] : ���� ���� ȹ�淮
			case PktCampCmd::MINING_CAMP_GAIN_MINERAL:
			{
				CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp*>(lpCamp);
				if ( NULL != lpMiningCamp )
				{
					unsigned short wMineralID = static_cast<unsigned short>(dwValue1);
					unsigned short wAmount = static_cast<unsigned short>(dwValue2);

					lpMiningCamp->GainMineral( wMineralID, wAmount );
				}
				else
				{
					ERRLOG1(g_Log, "ä���� ĳ���� ���� CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// ���� ���� (��� ��� ����)
			// dwValue1[In] : ���� �� ����
			case PktCampCmd::CAMP_SHOP_CHANGE_TAX:
			{
				CCampShop* lpCampShop = reinterpret_cast<CCampShop*>(lpCamp);
				if (NULL != lpCampShop)
				{
					unsigned char cTax = static_cast<unsigned char>(dwValue1);
					lpCampShop->SetTax(cTax);
				}
				else
				{
					ERRLOG1(g_Log, "���� Ÿ�� ĳ���� ���� CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// ���ͱ� ȹ�� (��� ��� ����)
			// dwValue1[In] : ���ͱ�
			case PktCampCmd::CAMP_SHOP_GAIN_TAX:
			{
				CCampShop* lpCampShop = reinterpret_cast<CCampShop*>(lpCamp);
				if (NULL != lpCampShop)
				{
					unsigned long dwGold = dwValue1;
					lpCampShop->DeductGold(dwGold);

					Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpCampShop->GetGID());
					if (NULL != lpGuild)
					{
						lpGuild->AddGold(dwGold);
						lpGuild->SendGuildSafe(lpCampShop->GetCID(), "��� ��� ����", PktGuildSafe::SC_CAMPSHOP);
					}
				}
				else
				{
					ERRLOG1(g_Log, "���� Ÿ�� ĳ���� ���� CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// ���� ���� ���� ����
			case PktCampCmd::WORLDWEAPON_CHARGE:
			{
				CWorldWeapon* lpWorldWeapon = reinterpret_cast<CWorldWeapon*>(lpCamp);
				if ( NULL != lpWorldWeapon )
				{
					lpWorldWeapon->UpdateWeaponState(Siege::WEAPON_CHARGE);
				}
				else
				{
					ERRLOG1(g_Log, "���� ���� ĳ���� ���� CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// ���� ���� ���� ���� �Ϸ�
			case PktCampCmd::WORLDWEAPON_CHARGE_COMPLETE:
			{
				CWorldWeapon* lpWorldWeapon = reinterpret_cast<CWorldWeapon*>(lpCamp);
				if ( NULL != lpWorldWeapon )
				{
					lpWorldWeapon->UpdateWeaponState(Siege::WEAPON_READY);
				}
				else
				{
					ERRLOG1(g_Log, "���� ���� ĳ���� ���� CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// ���� ���� ���� �߻�
			// dwValue1[In] : X ��ǥ
			// dwValue2[In] : Z ��ǥ
			case PktCampCmd::WORLDWEAPON_FIRE:
			{
				CWorldWeapon* lpWorldWeapon = reinterpret_cast<CWorldWeapon*>(lpCamp);
				if ( NULL != lpWorldWeapon )
				{
					lpWorldWeapon->UpdateWeaponState(Siege::WEAPON_FIRE, dwValue1, dwValue2);
				}
				else
				{
					ERRLOG1(g_Log, "���� ���� ĳ���� ���� CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;
		}
	}

	return true;
}

// ���� ���� ��� ��Ŷ
bool DBAgentPacketParse::ParseSiegeArmsCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktSiegeArmsCmd* lpPktSACmd = static_cast<PktSiegeArmsCmd*>(lpPktBase);

	unsigned char cState = lpPktSACmd->m_cState;
	unsigned long dwValue = lpPktSACmd->m_dwValue;
	unsigned char cSubCmd = lpPktSACmd->m_cSubCmd;

	CSiegeArms* lpSiegeArms = reinterpret_cast<CSiegeArms*>( CSiegeObjectMgr::GetInstance().GetSiegeObject(lpPktSACmd->m_dwArmsID) );

    if (lpSiegeArms)
	{
		switch (cSubCmd)
		{
			// ���� ���� ���� �Ϸ�
		case PktSiegeArmsCmd::SIEGE_CREATE_ARMS_COMPLETE:
			{
				lpSiegeArms->Build(static_cast<unsigned char>(dwValue));
			}
			break;

			// ���� ���� ���� ��� (�ٷε�)
			// ���� ���⸦ ��ŸƮŶ���� �Ϸ�
		case PktSiegeArmsCmd::SIEGE_CANCEL:			
		case PktSiegeArmsCmd::SIEGE_TO_STARTKIT_COMPLETE:
			{
				bool bMaterial = false;

				if (PktSiegeArmsCmd::SIEGE_CANCEL==cSubCmd)					
				{
					bMaterial = true;
				}

				lpSiegeArms->AllGetOff();
				lpSiegeArms->ToStarterKit(bMaterial);
				lpSiegeArms->Destroy(0, false);
				CSiegeObjectMgr::GetInstance().DeleteSiegeObject(lpSiegeArms->GetCID());
			}
			break;

			// ���� ���� ����
			// ���� ���� ��ŸƮ Ŷ���� ����
		case PktSiegeArmsCmd::SIEGE_REPAIR_ARMS:	// [In/Out] dwValue1 : RepairHP
		case PktSiegeArmsCmd::SIEGE_TO_STARTKIT:
			{
				lpSiegeArms->Update(cState, dwValue, 0, 0, cSubCmd);
			}
			break;

			// ���� ���� ���� �Ϸ�
		case PktSiegeArmsCmd::SIEGE_REPAIR_ARMS_COMPLETE:
			{
				lpSiegeArms->Repair(static_cast<unsigned short>(dwValue));
			}
			break;

			
			// ���� ���� �ı�
		case PktSiegeArmsCmd::SIEGE_DESTROY_ARMS:
			{
				lpSiegeArms->AllGetOff();
				lpSiegeArms->Destroy(0, false);
				CSiegeObjectMgr::GetInstance().DeleteSiegeObject(lpSiegeArms->GetCID());
			}
			break;
		}
	}

	return true;
}

// �� ���� ���� ����
bool DBAgentPacketParse::ParseCastleRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCastleRight* lpPktCastleRight = static_cast<PktCastleRight*>(lpPktBase);

	CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle(lpPktCastleRight->m_dwCastleID);

	if (lpCastle)
	{
		lpCastle->SetRight(lpPktCastleRight->m_CastleRight);
	}

	return true;
}

// ��� ��� ���� ���� ����
bool DBAgentPacketParse::ParseCampRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCampRight* lpPktCampRight = static_cast<PktCampRight*>(lpPktBase);

	CCamp* lpCamp = CSiegeObjectMgr::GetInstance().GetCamp(lpPktCampRight->m_dwCampID);

	if (lpCamp)
	{
		lpCamp->SetRight(lpPktCampRight->m_CampRight);
	}

	return true;
}

// ���� �ð� ���� ��Ŷ
bool DBAgentPacketParse::ParseGameTimeInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktGameTimeInfo* lpPktGTI = static_cast<PktGameTimeInfo*>(lpPktBase);
	CastleSiegeInfo* lpCastleSiegeInfo = reinterpret_cast<CastleSiegeInfo*>(lpPktGTI + 1);
	unsigned char cType = lpPktGTI->m_cType;
	unsigned short wSize = lpPktGTI->m_wSize;
	bool bGuildUpdated = lpPktGTI->m_GuildTimeInfo.m_bUpdated;
	bool bRealmUpdated = lpPktGTI->m_RealmTimeInfo.m_bUpdated;
	bool bSiegeUpdated = lpPktGTI->m_SiegeTimeInfo.m_bUpdated;
	bool bEventUpdated = lpPktGTI->m_EventTimeInfo.m_bUpdated;

	// �� ������ ���� ���� ���� ����
	for (int i=0; i<lpPktGTI->m_cCastleNum; ++i)
	{
		if (lpCastleSiegeInfo)
		{
			CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle(lpCastleSiegeInfo->m_dwCastleID);
			if (lpCastle)
			{
				lpCastle->SetEnableSiege(lpCastleSiegeInfo->m_bEnableSiege);
			}
		}

		++lpCastleSiegeInfo;
	}

	unsigned char cOldGuildTime = CGameTimeMgr::GetInstance().GetCurrentGuildTime();
	unsigned char cOldRealmTime = CGameTimeMgr::GetInstance().GetCurrentRealmTime();
	unsigned char cOldSiegeTime = CGameTimeMgr::GetInstance().GetCurrentSiegeTime();
	unsigned char cOldEventTime = CGameTimeMgr::GetInstance().GetCurrentEventTime();

	// GameTimeMgr �� �� ����
	switch (cType)
	{
		case GameTime::TYPE_GAMESERVER_LOGIN:
		{
			CGameTimeMgr::GetInstance().SetTimeValue(GameTime::GUILD, lpPktGTI->m_GuildTimeInfo.m_cNowGuildTime,
				lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cMonth, lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cDay,
				lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cHour, lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cMinute,
				lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_dwTime);

			lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_dwTime = CGameTimeMgr::GetInstance().GetDetailRemainSec(GameTime::GUILD);

			CGameTimeMgr::GetInstance().SetTimeValue(GameTime::REALM, lpPktGTI->m_RealmTimeInfo.m_cNowRealmTime,
				lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cMonth, lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cDay,
				lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cHour, lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cMinute,
				lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_dwTime);

			lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_dwTime = CGameTimeMgr::GetInstance().GetDetailRemainSec(GameTime::REALM);

			CGameTimeMgr::GetInstance().SetTimeValue(GameTime::SIEGE, lpPktGTI->m_SiegeTimeInfo.m_cNowSiegeTime,
				lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cMonth, lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cDay,
				lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cHour, lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cMinute,
				lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_dwTime);

			lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_dwTime = CGameTimeMgr::GetInstance().GetDetailRemainSec(GameTime::SIEGE);

			CGameTimeMgr::GetInstance().SetTimeValue(GameTime::EVENT, lpPktGTI->m_EventTimeInfo.m_cNowEventTime,
				lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cMonth, lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cDay,
				lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cHour, lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cMinute,
				lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_dwTime);

			lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_dwTime = CGameTimeMgr::GetInstance().GetDetailRemainSec(GameTime::EVENT);
		}
		break;

		case GameTime::TYPE_CHANGE_GAME_TIME:
		{
			if (bGuildUpdated)
			{
				CGameTimeMgr::GetInstance().SetTimeValue(GameTime::GUILD, lpPktGTI->m_GuildTimeInfo.m_cNowGuildTime,
					lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cMonth, lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cDay,
					lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cHour, lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cMinute,
					lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_dwTime);

				lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_dwTime = CGameTimeMgr::GetInstance().GetDetailRemainSec(GameTime::GUILD);
			}

			if (bRealmUpdated)
			{
				CGameTimeMgr::GetInstance().SetTimeValue(GameTime::REALM, lpPktGTI->m_RealmTimeInfo.m_cNowRealmTime,
					lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cMonth, lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cDay,
					lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cHour, lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cMinute,
					lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_dwTime);

				lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_dwTime = CGameTimeMgr::GetInstance().GetDetailRemainSec(GameTime::REALM);
			}

			if (bSiegeUpdated)
			{
				CGameTimeMgr::GetInstance().SetTimeValue(GameTime::SIEGE, lpPktGTI->m_SiegeTimeInfo.m_cNowSiegeTime,
					lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cMonth, lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cDay,
					lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cHour, lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cMinute,
					lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_dwTime);

				lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_dwTime = CGameTimeMgr::GetInstance().GetDetailRemainSec(GameTime::SIEGE);				
			}

			if (bEventUpdated)
			{
				CGameTimeMgr::GetInstance().SetTimeValue(GameTime::EVENT, lpPktGTI->m_EventTimeInfo.m_cNowEventTime,
					lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cMonth, lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cDay,
					lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cHour, lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cMinute,
					lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_dwTime);

				lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_dwTime = CGameTimeMgr::GetInstance().GetDetailRemainSec(GameTime::EVENT);				
			}
		}
		break;

		case GameTime::TYPE_CHARACTER_LOGIN:
		case GameTime::TYPE_ENABLE_CASTLE_SIEGE:
			break;
	}

	unsigned char cNowGuildTime = CGameTimeMgr::GetInstance().GetCurrentGuildTime();
	unsigned char cNowRealmTime = CGameTimeMgr::GetInstance().GetCurrentRealmTime();
	unsigned char cNowSiegeTime = CGameTimeMgr::GetInstance().GetCurrentSiegeTime();
	unsigned char cNowEventTime = CGameTimeMgr::GetInstance().GetCurrentEventTime();

	// ���̽ĺ� �� ����
	switch (CServerSetup::GetInstance().GetServerZone())
	{

			case SERVER_ID::CAPITAL :
		{
			
			if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
			{
				if (CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_GUILD_WAR ) ;
				}
				else if (CGameTimeMgr::GetInstance().IsSiegeWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_SIEGE_WAR ) ;
				}
			
				else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}
			
			}
			else
			{
				if (CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_GUILD_WAR ) ;
				}
				else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}
			}


			{
			if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented hayzohar statue war fix
			{
				if (CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_STATUE_WAR ) ;
				}
				else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}
			}
			else //wasnt commented hayzohar statue war fix
			{
				EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
			}
		}


	}
		break;

	//						case 100/*SERVER_ID::CAPITAL*/ :
	/*	{
			// ������ : ������
			if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
			{
				if (CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_GUILD_WAR ) ;
				}
				else if (CGameTimeMgr::GetInstance().IsSiegeWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_SIEGE_WAR ) ;
				}
				/*else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}*/
			
		/*		else if (CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_STATUE_WAR ) ;
				}
				else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}
			
			
			}
			else
			{
				if (CGameTimeMgr::GetInstance().IsGuildWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_GUILD_WAR ) ;
				}
				else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}
			}
		}
		break;
		*/
		case SERVER_ID::STONE_WAR1 :
		case SERVER_ID::STONE_WAR2 :
		case SERVER_ID::STONE_WAR3 :
		{
			//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented hayzohar statue war fix
			//{
				if (CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_STATUE_WAR ) ;
				}
				else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}
			//}
			//else //wasnt commented hayzohar statue war fix
			//{
				//EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
			//}
		}
		break ;

		case SERVER_ID::ZONE12:
		{
			EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
		}
		break ;

		case SERVER_ID::ZONE1:
		case SERVER_ID::ZONE2:
		case SERVER_ID::ZONE4:
		case SERVER_ID::ZONE5:
		{
			EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_HUNTING ) ;
		}
		break ;

		default :
		{
			EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
//			EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_HUNTING ) ;
		}
		break ;
	}

	// ����� �ð��� ���� ��� ������� �ı����� ����
	if (GameTime::TYPE_CHANGE_GAME_TIME == cType &&
		CGameTimeMgr::GetInstance().IsGuildWarTime(cOldGuildTime) && !CGameTimeMgr::GetInstance().IsGuildWarTime())
	{
		CSiegeObjectMgr::GetInstance().DecreaseWeaponRemainSiegeTime();
	}

    // �������� ���� ��� 
	if (GameTime::TYPE_CHANGE_GAME_TIME == cType &&
		CGameTimeMgr::GetInstance().IsSiegeWarTime(cOldSiegeTime) && !CGameTimeMgr::GetInstance().IsSiegeWarTime())
	{
		// �� ���� ���⸦ ���� ���� NPC �� �ٲ۴�.
		CCastleMgr::GetInstance().DestroyAllCastleArms();

		// �� ���� ���� ���� ���� ����
		CCastleMgr::GetInstance().EnableTaxChange();
	}

	// Ŭ���̾�Ʈ���� ����
	char* szPacket = reinterpret_cast<char *>(lpPktGTI);
	if (PacketWrap::WrapCrypt(szPacket, wSize, CmdGameTimeInfo, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, wSize, CmdGameTimeInfo);
	}

	// Instance On �� ��� �÷��� ���ֱ�
	if (cType == GameTime::TYPE_CHANGE_GAME_TIME)
	{
		if (!CGameTimeMgr::GetInstance().IsGuildWarTime())
		{
			CCreatureManager::GetInstance().ClearGuildWarInstanceFlag();
		}

		// ������ : ��ũ ī���� ���� ����
		//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
		//{
			if (!CGameTimeMgr::GetInstance().IsRealmWarReadyTime() &&
				!CGameTimeMgr::GetInstance().IsRealmWarTime())
			{
				CCreatureManager::GetInstance().ClearRealmWarInstanceFlag();
			}
		//}
	}

	if (cType == GameTime::TYPE_CHANGE_GAME_TIME)
	{

		// ������� ����Ǹ� ������ �̵��ϰ� �ϴ� ���� �̰� ���ҽ����� ���ƴ�.
		if (bGuildUpdated && cOldGuildTime != cNowGuildTime && GameTime::GT_GUILD_WAR == cNowGuildTime)
		{
			// ���̵��� �ʿ��� ��� ���̵� ó��
			CCreatureManager::GetInstance().MoveCharacterToGuildWarZone(GameTime::GUILD);
		}

		// ������ : ��ũ ī���� ���� ����
		//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
		//{
			if (bRealmUpdated && cOldRealmTime != cNowRealmTime && GameTime::RT_PREPARE_START_10 == cNowRealmTime)
			{
				// ���̵��� �ʿ��� ��� ���̵� ó��
				CCreatureManager::GetInstance().MoveCharacterToRealmWarZone(GameTime::REALM);
			}

			if (bRealmUpdated && cOldRealmTime != cNowRealmTime && GameTime::RT_REALM_WAR == cNowRealmTime &&
				(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))
			{
				// ���̵��� �ʿ��� ��� ���̵� ó��
				CCreatureManager::GetInstance().MoveCharacterToRealmWarZone(GameTime::REALM, false);
			}
		//}
	}

	// ������ : ��ũ ī���� ���� ����
	//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
	//{
		if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
		{
			// ��ũ ī������ �������� �ð����� �������� ȿ���� �����Ѵ�.
			if (GameTime::TYPE_CHANGE_GAME_TIME == cType)
			{
				if (CGameTimeMgr::GetInstance().IsRealmWarReadyTime(cOldRealmTime) && CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					// ��þƮ�� �������� ȿ�� �߰�.
					CCreatureManager::GetInstance().ProcessAllCharacter(RealmSkill::RealmInchantAdd);
				}
				else if (CGameTimeMgr::GetInstance().IsRealmWarTime(cOldRealmTime) && !CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					// ��þƮ�� �������� ȿ�� ����.
					CCreatureManager::GetInstance().ProcessAllCharacter(RealmSkill::RealmInchantRemove);
				}
			}

			// ��ũ ī������ ���� ���� 10�� ���� ���� �ʱ�ȭ
			if (cOldRealmTime != cNowRealmTime && GameTime::RT_PREPARE_START_10 == cNowRealmTime)
			{
				SERLOG0(g_Log, "���� ���� �ð� 10�� ���̹Ƿ� ���� ������ �ʱ�ȭ �մϴ�.");
				CCreatureManager::GetInstance().InitRealmStatue();

				// ������ ǥ�� ���� ������
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();

				// ���� ��æƮ ȿ�� ������ ����Ѵ�.
				CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();

				// edith 2008.06.04 ��þƮ�� ���� ����� 2�ð� ¥���� �ɸ��� ������ ����
				// ���� ���� ���� ��æƮ ����
//				CCreatureManager::GetInstance().ClearRealmStatueEnchant();
			}

			// ��ũ ī������ ���� ���� ����� �ε����� ���� �߸�ȭ
			if (CGameTimeMgr::GetInstance().IsRealmWarTime(cOldRealmTime) && !CGameTimeMgr::GetInstance().IsRealmWarTime())
			{
				SERLOG0(g_Log, "���� ���� �ð��� ����Ǿ����Ƿ� ��ȯ���� ������ �߸�ȭ �մϴ�.");
				CCreatureManager::GetInstance().InitRealmLoadingStatue();

				// ������ ǥ�� ���� ������
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();

				// ���� ��æƮ ȿ�� ������ ����Ѵ�.
				CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();

				// ���� ���� ���� ��æƮ ���ֱ�
				// edith 2008.06.04 ��þƮ�� ���� ����� 2�ð� ¥���� �ɸ���
				// �������� ���������� 2�ð� ¥�� ������ �ɾ��ش�.
				CCreatureManager::GetInstance().AddRealmStatueEnchant();
			}
		}
	//}
/*
	// edith 2008.06.03 �̰� ������ �����ʾ�??
	// ������ �������� �������� �ư� �и��� �Ʊ⶧���� �Ʒ� ������ �ʿ䰡 ����.
	// ������ : ������
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
		{
			// ��ũ ī���� ������ ���۽�
			if (!CGameTimeMgr::GetInstance().IsSiegeWarTime(cOldSiegeTime) && CGameTimeMgr::GetInstance().IsSiegeWarTime())
			{
				// ���� ��æƮ ȿ�� ������ ����Ѵ�.
				CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();

				// edith 2008.06.04 ��þƮ�� ���� ����� 2�ð� ¥���� �ɸ��� ������ ����
				// ���� ���� ���� ��æƮ ����
				CCreatureManager::GetInstance().ClearRealmStatueEnchant();
			}

			// ��ũ ī������ ������ �����
			if (CGameTimeMgr::GetInstance().IsSiegeWarTime(cOldSiegeTime) && !CGameTimeMgr::GetInstance().IsSiegeWarTime())
			{
				// ���� ��æƮ ȿ�� ������ ����Ѵ�.
				CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();

				// ���� ���� ���� ��æƮ ���ֱ�
				CCreatureManager::GetInstance().AddRealmStatueEnchant();
			}
		}
	}
*/
	// ī������ ���. �������� ������ �������� �̵���Ų�� //			
	if(CServerSetup::GetInstance().GetServerZone() == SERVER_ID::CAPITAL)
	{
		if(cType==GameTime::TYPE_CHANGE_GAME_TIME)
		{
			if((CGameTimeMgr::GetInstance().IsSiegeWarTime(cOldSiegeTime) && !CGameTimeMgr::GetInstance().IsSiegeWarTime()) || cNowSiegeTime==GameTime::ST_SIEGE_WAR)
			{
				CCreatureManager::GetInstance().ProcessAllCharacter(SiegeMovePos);
			}
		}		
	}

	return true;
}

// �� �ӽ� ���� -> ���� ���� �̵� ���� ������Ʈ
bool DBAgentPacketParse::ParseCastleTaxMove(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCastleTaxMove* lpPktCTM = static_cast<PktCastleTaxMove*>(lpPktBase);

	CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle(lpPktCTM->m_dwCastleID);
	if (lpCastle)
	{
		lpCastle->UpdateTaxMoveInfo(lpPktCTM->m_dwMoveTempMoney, lpPktCTM->m_dwResultTaxMoney);
	}

	return true;
}

// ���� �ð� �� ���� ���� ���� Ƚ�� ���� ������Ʈ
bool DBAgentPacketParse::ParseCastleSiegeCount(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCastleSiegeCount* lpPktCSC = static_cast<PktCastleSiegeCount*>(lpPktBase);

	CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle(lpPktCSC->m_dwCastleID);
	if (lpCastle)
	{
		lpCastle->UpdateSiegeCountInfo(lpPktCSC->m_cSiegeCount, lpPktCSC->m_cInvincibleCount, lpPktCSC->m_wTotalSiegeCount);
	}

	return true;
}

// ��� ��� ���� �޼��� ��Ŷ ����
bool DBAgentPacketParse::ParseCampMessage(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCampMessage* lpPktCM = static_cast<PktCampMessage*>(lpPktBase);

	bool			bNotify = lpPktCM->m_bNotify;
	unsigned long	dwGID = lpPktCM->m_dwGID;
	
	char* szPacket = reinterpret_cast<char *>(lpPktCM);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampMessage), CmdCampMessage, 0, 0))
	{
        if (bNotify)
		{
			CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCampMessage), CmdCampMessage);
		}
		else
		{
			Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( dwGID );
			if ( lpGuild )
			{
				lpGuild->SendCurrentMember(szPacket, sizeof(PktCampMessage), CmdCampMessage);
			}
		}
	}

	return true;
}

// ä���� ���μ��� ����
bool DBAgentPacketParse::ParseProcessMining(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	// ������ �Ǵ� �� ���������� �����ϴ�.
	char cZone = CServerSetup::GetInstance().GetServerZone();
	if (SERVER_ID::CAPITAL == cZone)
	{
		PktProcessMining* lpPktPM = static_cast<PktProcessMining*>(lpPktBase);
		unsigned long dwProcessType = lpPktPM->m_dwProcessType;

		// ä���� ���μ��� ����
		CSiegeObjectMgr::GetInstance().ProcessMiningCamp( dwProcessType );
	}

	return true;
}

// �����, ������ �÷��� ó��
bool DBAgentPacketParse::ParseWarOnOff(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	// Game -> DB�� ����.
	PktWarOnOff* lpPktWarOnOff = static_cast<PktWarOnOff*>(lpPktBase);
	unsigned long	dwCID	= lpPktWarOnOff->m_dwCID;
	unsigned char	cType	= lpPktWarOnOff->m_cType;
	unsigned char	cFlag	= lpPktWarOnOff->m_cFlag;
	unsigned short	wError	= PktWarOnOff::NO_SERVER_ERR;

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL != lpCharacter)
	{
		if (GameTime::GUILD == cType)
		{
			switch (cFlag)
			{
				case Creature::WAR_OFF:			lpCharacter->SetGuildWarFlag(cFlag);	break;
				case Creature::WAR_ON:			lpCharacter->SetGuildWarFlag(cFlag);	break;
				case Creature::WAR_INSTANCE:	lpCharacter->SetGuildWarFlag(cFlag);	break;
				default:	return true;
			}
		}
		else if (GameTime::REALM == cType)
		{
			switch (cFlag)
			{
				case Creature::WAR_OFF:			lpCharacter->SetRealmWarFlag(cFlag);	break;
				case Creature::WAR_ON:			lpCharacter->SetRealmWarFlag(cFlag);	break;
				case Creature::WAR_INSTANCE:	lpCharacter->SetRealmWarFlag(cFlag);	break;
				default:	return true;
			}
		}
		else
		{
			wError = PktWarOnOff::E_INVALID_TYPE;	
		}

		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		
		if (NULL != lpDispatch)
		{
			GameClientSendPacket::SendWarOnOff(lpDispatch->GetSendStream(), lpCharacter->GetCID(), cType, cFlag, wError);

			if (GameTime::GUILD == cType)
			{
				if (CServerSetup::GetInstance().GetServerZone() != SERVER_ID::CAPITAL)
				{
					// ���� ��ư Ŭ������ �ν��Ͻ��� �Ǹ� �� �̵�
					if (CGameTimeMgr::GetInstance().IsGuildWarTime() &&	lpCharacter->GetGuildWarFlag() != Creature::WAR_OFF)
					{
						lpCharacter->MoveToGuildWarZone();
					}								
				}
			}
			else if (GameTime::REALM == cType)
			{
				if ((CGameTimeMgr::GetInstance().IsRealmWarReadyTime() || CGameTimeMgr::GetInstance().IsRealmWarTime()) &&
					lpCharacter->GetRealmWarFlag() == Creature::WAR_INSTANCE)
				{
				//	if(CGameTimeMgr::GetInstance().IsRealmWarReadyTime())//hz wasnt here automaticly respawn at racewar fix.
						lpCharacter->MoveToRealmWarZone();
				}
			}
		}
	}

	return true;
}

bool DBAgentPacketParse::ParseRealmPoint(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktRealmPoint* lpPktRealmPoint = static_cast<PktRealmPoint*>(lpPktBase);

	unsigned char cRealmPoint	= lpPktRealmPoint->m_cRealmPoint;
	unsigned long dwCID			= lpPktRealmPoint->m_dwCID;
	unsigned char cType			= lpPktRealmPoint->m_cType;

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);

	if(lpCharacter)
	{
		lpCharacter->SetRealmPoint(cRealmPoint);

		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		
		if(lpDispatch)
		{
			GameClientSendPacket::SendRealmPoint(lpDispatch->GetSendStream(), dwCID, cRealmPoint, cType, 0);

			return true;
		}
	}
	return false;	
}


bool DBAgentPacketParse::ParseStatueCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktStatueCmd* lpPktStatueCmd = static_cast<PktStatueCmd*>(lpPktBase);

	unsigned short wSubCmd = lpPktStatueCmd->m_wSubCmd;
	unsigned short wSize = lpPktStatueCmd->m_wSize;
	unsigned long dwValue = lpPktStatueCmd->m_dwValue;

	switch (wSubCmd)
	{
		case PktStatueCmd::CMD_INFO:
			{
				// ������ : ��ũ ī���� ���� ����
				//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
				//{
					StatueInfo* lpStatueInfo = reinterpret_cast<StatueInfo*>(lpPktStatueCmd + 1);
					for (unsigned long i=0; i<dwValue; ++i, ++lpStatueInfo)
					{
						CCreatureManager::GetInstance().SetRealmStatueInfo(lpStatueInfo->m_dwIndex, lpStatueInfo->m_wKID, lpStatueInfo->m_dwLostHP);
					}

					// ���� ���� ��æ�� ���� ����Ѵ�. ( ���� ���� ���ʽ����� ����Ѵ�. )
					CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();
				//}
/*
				// ������ : �ű���
				else if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
				{
					StatueInfo* lpStatueInfo = reinterpret_cast<StatueInfo*>(lpPktStatueCmd + 1);
					for (unsigned long i=0; i<dwValue; ++i, ++lpStatueInfo)
					{
						CCreatureManager::GetInstance().SetRealmStatueInfo(lpStatueInfo->m_dwIndex, lpStatueInfo->m_wKID, lpStatueInfo->m_dwLostHP);
					}
				}
*/
			}
			break;

		case PktStatueCmd::CMD_UPDATE_LOST_HP:
		case PktStatueCmd::CMD_UPDATE_KID:
		default:
			return false;
			break;
	}

	return true;
}

void DBAgentPacketParse::SiegeMovePos(CCharacter* pCharacter)
{
	if(!pCharacter)
	{
		return;
	}

	if(pCharacter->IsRideArms())
	{
		return;
	}

	unsigned char cNation = Castle::CCastleMgr::GetInstance().GetNation();

	Position Pos;

	float fPosX = static_cast<float>(Math::Random::ComplexRandom(6));
	float fPosZ = static_cast<float>(Math::Random::ComplexRandom(6));

	if(cNation!=pCharacter->GetNation())
	{
		const Position currPos = pCharacter->GetCurrentPos();

		if(currPos.m_fPointX>=1890 && currPos.m_fPointX<=2205 &&
			currPos.m_fPointZ>=2835 && currPos.m_fPointZ<=3150)
		{
			if(cNation==Creature::MERKADIA)
			{
				Pos.m_fPointX = 1094.0f + fPosX;
				Pos.m_fPointY = 70.0f;
				Pos.m_fPointZ = 2594.0f + fPosZ;
			}
			else
			{
				Pos.m_fPointX = 3294.0f + fPosX;
				Pos.m_fPointY = 70.0f;
				Pos.m_fPointZ = 2994.0f + fPosZ;
			}

			pCharacter->MoveTo(Pos, false);

			POS sendPos;
			sendPos.fPointX = Pos.m_fPointX;
			sendPos.fPointY = Pos.m_fPointY;
			sendPos.fPointZ = Pos.m_fPointZ;

			GameClientSendPacket::SendCharBindPosition(*pCharacter, 0, PktBP::BP_WARP, sendPos, CServerSetup::GetInstance().GetServerZone(), 0);
		}
	}	
}