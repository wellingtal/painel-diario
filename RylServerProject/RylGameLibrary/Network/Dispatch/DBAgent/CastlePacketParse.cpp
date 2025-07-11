
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

// 성 데이터 업데이트
bool DBAgentPacketParse::ParseCastleInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	// edith 세금 (성생성 부분)
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

// 길드 요새 데이터 업데이트
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

// 지력 데이터 업데이트
bool DBAgentPacketParse::ParseFertilityInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktFertilityInfo* lpPktFertilityInfo = static_cast<PktFertilityInfo*>(lpPktBase);

	unsigned short wSize = lpPktFertilityInfo->m_wSize;
	unsigned short wNum = lpPktFertilityInfo->m_wNum;

	CMineralVeinMgr::GetInstance().SetNowFertility(wNum, reinterpret_cast<unsigned long*>(lpPktFertilityInfo+1));
	return true;
}

// 월드 웨폰 업데이트
bool DBAgentPacketParse::ParseWorldWeaponInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktWorldWeaponInfo* lpPktWeaponInfo = static_cast<PktWorldWeaponInfo* >(lpPktBase);

	CSiegeObjectMgr::GetInstance().SetWorldWeaponInfo(lpPktWeaponInfo->m_cKarRemainSiegeTime, lpPktWeaponInfo->m_cMerRemainSiegeTime);
	return true;
}

// 새로 구축중인 길드 요새
bool DBAgentPacketParse::ParseCreateCamp(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktCreateCamp* lpPktCreateCamp = static_cast<PktCreateCamp*>(lpPktBase);
	CSiegeObject* lpCamp = NULL;

	// 생성에 실패했다면...
	if (PktCreateCamp::ERROR_EXIST_ANOTHER_WEAPON == lpPktCreateCamp->GetError())
	{
		// 동시에 두개의 생성 처리를 막기 위해서...
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

				// 동시에 두개의 생성 처리를 막기 위해서...
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
			// 길드 정보가 없을 때 제삼국적으로 임시 사용
			lpPktCC->m_cNation	= Creature::ALMIGHTY_PIRATE;
		}

		if (true == PacketWrap::WrapCrypt(szBuffer, MAX_BUFFER, CmdCreateCamp, 0 ,0))
		{
			CCreatureManager::GetInstance().SendAllCharacter(szBuffer, MAX_BUFFER, CmdCreateCamp);
		}
	}

	return true;
}

// 새로 생성된 공성 병기
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
			// Vincent - 브로드 캐스트 테스트 코드
			//lpSiegeAmrs->SendToRadiusCell(szBuffer, MAX_BUFFER, CmdCreateSiegeArms);
			lpSiegeAmrs->SendToRange(Siege::BROADCAST_RADIUS, szBuffer, MAX_BUFFER, CmdCreateSiegeArms);
		}
	}

	return true;
}

// 성 관련 명령 패킷
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
			// 성 상징물
			// [In/Out] dwValue1 : 세율 타입
			// [In/Out] dwValue2 : 세율
			case PktCastleCmd::CASTLE_SET_TAX:	// 세율 조정
			{
				lpCastle->SetTax(static_cast<unsigned char>(dwValue1), static_cast<unsigned char>(dwValue2));
			}
			break;

			case PktCastleCmd::CASTLE_UPGRADE_EMBLEM:			// 상징물 업그레이드	[In/Out] dwValue1 : 업그레이드 타입
			case PktCastleCmd::CASTLE_UPDATE_EMBLEM:			// 성 상징물 파괴		[In] dwValue1 : 소환중인 상징물이 아군것인지 적군것인지 정보
																//						[Out] dwValue1 : 현재 상징물의 GID
			case PktCastleCmd::CASTLE_SUMMON_EMBLEM_COMPLETE:	// 성 상징물 소환 완료	[In/Out] dwValue1 : 아군이 소환완료
			{
				lpCastleObject->Update(cState, dwValue1, 0, 0, cSubCmd);				
			}
			break;

			// [In/Out] dwValue1 : 업그레이드 단계
			case PktCastleCmd::CASTLE_UPGRADE_EMBLEM_COMPLETE:	// 상징물 업그레이드 완료
			{
				lpCastleObject->Upgrade(static_cast<unsigned char>(dwValue1));
			}
			break;

			// [In/Out] dwValue1 : 새로운 성의 소유 국가
			case PktCastleCmd::CASTLE_CHANGE_MASTER:	// 성주 변경
			{
				// 성주 변경된 정보를 알려준다.
				lpCastle->ChangeCastleMaster(static_cast<unsigned char>(dwValue1));

				// 리스폰 포인트 국적 변경
				CCharRespawnMgr::GetInstance().SetCastleRespawnPointNation(lpCastle->GetNameID(), static_cast<unsigned char>(dwValue1));

				// NPC 들의 GID 변경
				CCreatureManager::GetInstance().SetNationToCastleNPC(lpCastle->GetCastleID(), static_cast<unsigned char>(dwValue1));

				// 공성전 중 로그인할 경우 공성측은 리스폰 지역으로 이동시킨다.
				if(CServerSetup::GetInstance().GetServerZone() == SERVER_ID::CAPITAL)
				{
					if(CGameTimeMgr::GetInstance().IsSiegeWarTime())
					{
						CCreatureManager::GetInstance().ProcessAllCharacter(SiegeMovePos);
					}
				}
			}
			break;

			// [In/Out] dwValue1 : 세금 타입
			// [In/Out] dwValue2 : 회수할 세금
			case PktCastleCmd::CASTLE_TAKE_TAXMONEY:	// 세금 회수
			{
				lpCastle->TakeTaxMoney(static_cast<unsigned char>(dwValue1), dwValue2);
			}
			break;

			case PktCastleCmd::CASTLE_LOSE_OWNERSHIP:	// 성 소유 중립화
			{
				lpCastle->LoseOwnership();

				// 리스폰 포인트 삭제
				CCharRespawnMgr::GetInstance().SetCastleRespawnPointNation(lpCastle->GetNameID(), Creature::STATELESS);

				// NPC 들의 GID 변경
				CCreatureManager::GetInstance().SetNationToCastleNPC(lpCastle->GetCastleID(), Creature::STATELESS);
			}
			break;

			// [In/Out] dwValue1 : 광물ID
			// [In] dwValue2 : 획득량
			// [Out] dwValue2 : 최종남은양
			case PktCastleCmd::CASTLE_GAIN_MINERAL:		// 광물 획득
			{
				unsigned short wMineralID = static_cast<unsigned short>(dwValue1);
				unsigned short wAmount = static_cast<unsigned short>(dwValue2);

				lpCastle->GainMineral(wMineralID, wAmount);
			}
			break;

			// ------------------------------------------------------------------------
			// 성문
			// [Out] dwValue1 : SubState (OPENED)
			case PktCastleCmd::CASTLE_GATE_OPEN:	// 성문 열기
			{
				if (Siege::GATE == lpCastleObject->GetObjectType())
				{
					reinterpret_cast<CCastleGate*>(lpCastleObject)->Open();
				}
			}
			break;

			// [Out] dwValue1 : SubState (CLOSED)
			case PktCastleCmd::CASTLE_GATE_CLOSE:	// 성문 닫기
			{
				if (Siege::GATE == lpCastleObject->GetObjectType())
				{
					reinterpret_cast<CCastleGate*>(lpCastleObject)->Close();
				}
			}
			break;

			case PktCastleCmd::CASTLE_UPGRADE_GATE:		// 성문 업그레이드	[In/Out] dwValue1 : 업그레이드 비용 (사용은 안함)
			case PktCastleCmd::CASTLE_REPAIR_GATE:		// 성문 수리		[In/Out] dwValue1 : 수리 비용       (사용은 안함)
			case PktCastleCmd::CASTLE_RESTORE_GATE:		// 성문 복구		[In/Out] dwValue1 : 복구 비용       (사용은 안함)
			{
				lpCastleObject->Update(cState, dwValue1, 0, 0, cSubCmd);
			}
			break;

			// [In/Out] dwValue1 : 업그레이드 단계
			case PktCastleCmd::CASTLE_UPGRADE_GATE_COMPLETE:	// 성문 업그레이드 완료
			{
				lpCastleObject->Upgrade(static_cast<unsigned char>(dwValue1));
			}
			break;

			// [In] dwValue1 : 수리되어 추가될 HP
			// [Out] dwValue1 : 수리된 최종 HP (현재 HP)
			case PktCastleCmd::CASTLE_REPAIR_GATE_COMPLETE:		// 성문 수리 완료
			{
				lpCastleObject->Repair(static_cast<unsigned short>(dwValue1));
			}
			break;

			// [In] dwValue1 : 복구된 HP 라고하지만.. 실은 0 이다. (적용 안됨)
			// [Out] dwValue1 : 현재 HP
			case PktCastleCmd::CASTLE_RESTORE_GATE_COMPLETE:	// 성문 복구 완료
			{
				lpCastleObject->Restore();
			}
			break;

			case PktCastleCmd::CASTLE_DESTROY_GATE:	// 성문 파괴
			{
				lpCastleObject->Destroy(0, false);
			}
			break;

			// ------------------------------------------------------------------------
			// 수성 병기
			case PktCastleCmd::CASTLE_CREATE_ARMS:				// 수성 병기 개발		[In/Out] dwValue1 : 개발 비용		[In] dwValue2 : 상위 16비트 : 생산시간 / 하위 16비트 : 병기 타입	[Out] dwValue2 : 병기 타입
			case PktCastleCmd::CASTLE_UPGRADE_ARMS:				// 수성 병기 업그레이드	[In/Out] dwValue1 : 업그레이드 비용	[In/Out] dwValue2 : 업그레이드 시간 (분)
			case PktCastleCmd::CASTLE_REPAIR_ARMS:				// 수성 병기 수리		[In/Out] dwValue1 : 수리 비용		[In/Out] dwValue2 : 수리할 HP 양
			{
				lpCastleObject->Update(cState, dwValue1, dwValue2, dwCID, cSubCmd);
			}
			break;
			
			// [In/Out] dwValue1 : 병기 타입
			case PktCastleCmd::CASTLE_CREATE_ARMS_COMPLETE:		// 수성 병기 개발 완료
			{
				if (lpCastleObject->IsCastleArms())
				{
					reinterpret_cast<CCastleArms*>(lpCastleObject)->ChangeType(static_cast<unsigned short>(dwValue1));
				}
			}
			break;

			// [In/Out] dwValue1 : 업그레이드 단계
			case PktCastleCmd::CASTLE_UPGRADE_ARMS_COMPLETE:	// 수성 병기 업그레이드 완료
			{
				lpCastleObject->Upgrade(static_cast<unsigned char>(dwValue1));
			}
			break;

			// [In/Out] dwValue1 : 수리로 회복될 HP
            // [Out] dwValue2 : 수리로 회복된 현재 HP
			case PktCastleCmd::CASTLE_REPAIR_ARMS_COMPLETE:		// 수성 병기 수리 완료
			{
				lpCastleObject->Repair(static_cast<unsigned short>(dwValue1));
			}
			break;

			// [In] dwValue1 : 0 : 파괴, 1 : 병기관리 NPC로 변형( 생산 비용 절반을 돌려줌 )
			// [Out] dwValue1 : 병기 관리 NPC Type ID
			case PktCastleCmd::CASTLE_DESTROY_ARMS:				// 수성 병기 파괴
			{
				lpCastleObject->AllGetOff();
				lpCastleObject->Destroy(0, (0 != dwValue1));
			}
			break;


			// [Out] dwValue1 : Full HP ( To DBAgentServer )
			case PktCastleCmd::CASTLE_GATE_FORCE_OPEN:			// 성문을 강제로 열린 상태로 만든다.
			{
				if (lpCastleObject->IsGate())
				{
					static_cast<CCastleGate* >(lpCastleObject)->ForceOpen();
				}
			}
			break;

			// [Out] dwValue1 : Full HP ( To DBAgentServer )
			case PktCastleCmd::CASTLE_GATE_FORCE_CLOSE:			// 성문을 강제로 닫힌 상태로 만든다.
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

// 길드 요새 명령 패킷
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
			// 길드 요새 구축 완료
			case PktCampCmd::CAMP_BUILD_COMPLETE:
			{
				lpCamp->Build();

				// 리스폰 포인트 추가
				CCharRespawnMgr::GetInstance().AddCampRespawnPoint(lpCamp->GetCampID(), lpCamp->GetGID(), lpCamp->GetPosition());
			}
			break;

			// 길드 요새 구축 해제중
			// 길드 요새 스타트킷으로 변환중
			// 길드 요새 업그레이드중
			// 길드 요새 수리중
			// 길드 요새 변형중
			case PktCampCmd::CAMP_CANCEL:
			case PktCampCmd::CAMP_TO_STARTKIT:
			case PktCampCmd::CAMP_UPGRADE:			// dwValue1[In] : 업그레이드 단계
			case PktCampCmd::CAMP_REPAIR:			// dwValue1[In] : 수리로 회복될 HP
			case PktCampCmd::CAMP_CHANGE_TYPE:		// dwValue1[In] : 변형할 타입
			{
				lpCamp->Update(cState, dwValue1, dwValue2, 0, cSubCmd);
			}
			break;

			// 길드 요새 구축 해제 완료
			case PktCampCmd::CAMP_CANCEL_COMPLETE:
			{
				// 리스폰 포인트 삭제
				CCharRespawnMgr::GetInstance().DeleteCampRespawnPoint(lpCamp->GetCampID());

				lpCamp->Cancel();
				CSiegeObjectMgr::GetInstance().DeleteCamp(lpCamp->GetCampID());
			}
			break;

			// 길드 요새 업그레이드 완료
			case PktCampCmd::CAMP_UPGRADE_COMPLETE:	// dwValue1[In] : 업그레이드 단계
			{
				lpCamp->Upgrade(static_cast<unsigned char>(dwValue1));
			}
			break;

			// 길드 요새 수리 완료
			case PktCampCmd::CAMP_REPAIR_COMPLETE:	// dwValue1[In] : 수리로 회복된 HP
			{
				lpCamp->Repair(static_cast<unsigned short>(dwValue1));
			}
			break;

			// 스타트킷으로
			case PktCampCmd::CAMP_TO_STARTKIT_COMPLETE:
			{
				// 리스폰 포인트 삭제
				CCharRespawnMgr::GetInstance().DeleteCampRespawnPoint(lpCamp->GetCampID());

				lpCamp->Destroy( lpCamp->GetGID() );
				CSiegeObjectMgr::GetInstance().DeleteCamp( lpCamp->GetCampID() );
			}
			break;

			// 길드 요새 파괴시 / 7일간 캠프 미사용시 파괴
			case PktCampCmd::CAMP_DESTROY:			// dwValue1[In] : 파괴한 길드의 ID
			{
				// 리스폰 포인트 삭제
				CCharRespawnMgr::GetInstance().DeleteCampRespawnPoint(lpCamp->GetCampID());

				lpCamp->Destroy( dwValue1 );
				CSiegeObjectMgr::GetInstance().DeleteCamp( lpCamp->GetCampID() );
			}
			break;

			// 길드 요새 변형 완료
			case PktCampCmd::CAMP_CHANGE_COMPLETE:	// dwValue1[In] : 변형된 타입
			{
				lpCamp->ChangeType( static_cast<unsigned short>(dwValue1) );

				CSiegeObjectMgr::GetInstance().ChangeCampType( lpCamp->GetCampID(), static_cast<unsigned short>(dwValue1) );
			}
			break;

			// 채굴기 상태 변경
			// dwValue1[In] : 채굴기 상태 On / Ready / Off
			case PktCampCmd::MINING_CAMP_ON_OFF:
			{
				CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp*>(lpCamp);
				if ( NULL != lpMiningCamp )
				{
					lpMiningCamp->OnOff( static_cast<unsigned char>(dwValue1) );
				}
				else
				{
					ERRLOG1(g_Log, "채굴기 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// 채굴기 누적 광물 획득
			// dwValue1[In] : 누적 광물 아이템 아이디
			// dwValue2[In] : 누적 광물 획득량
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
					ERRLOG1(g_Log, "채굴기 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// 세율 변경 (길드 요새 상점)
			// dwValue1[In] : 변경 후 세율
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
					ERRLOG1(g_Log, "상점 타입 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// 수익금 획득 (길드 요새 상점)
			// dwValue1[In] : 수익금
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
						lpGuild->SendGuildSafe(lpCampShop->GetCID(), "길드 요새 상점", PktGuildSafe::SC_CAMPSHOP);
					}
				}
				else
				{
					ERRLOG1(g_Log, "상점 타입 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// 월드 웨폰 무기 장전
			case PktCampCmd::WORLDWEAPON_CHARGE:
			{
				CWorldWeapon* lpWorldWeapon = reinterpret_cast<CWorldWeapon*>(lpCamp);
				if ( NULL != lpWorldWeapon )
				{
					lpWorldWeapon->UpdateWeaponState(Siege::WEAPON_CHARGE);
				}
				else
				{
					ERRLOG1(g_Log, "월드 웨폰 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// 월드 웨폰 무기 장전 완료
			case PktCampCmd::WORLDWEAPON_CHARGE_COMPLETE:
			{
				CWorldWeapon* lpWorldWeapon = reinterpret_cast<CWorldWeapon*>(lpCamp);
				if ( NULL != lpWorldWeapon )
				{
					lpWorldWeapon->UpdateWeaponState(Siege::WEAPON_READY);
				}
				else
				{
					ERRLOG1(g_Log, "월드 웨폰 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;

			// 월드 웨폰 무기 발사
			// dwValue1[In] : X 좌표
			// dwValue2[In] : Z 좌표
			case PktCampCmd::WORLDWEAPON_FIRE:
			{
				CWorldWeapon* lpWorldWeapon = reinterpret_cast<CWorldWeapon*>(lpCamp);
				if ( NULL != lpWorldWeapon )
				{
					lpWorldWeapon->UpdateWeaponState(Siege::WEAPON_FIRE, dwValue1, dwValue2);
				}
				else
				{
					ERRLOG1(g_Log, "월드 웨폰 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
				}
			}
			break;
		}
	}

	return true;
}

// 공성 병기 명령 패킷
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
			// 공성 병기 생성 완료
		case PktSiegeArmsCmd::SIEGE_CREATE_ARMS_COMPLETE:
			{
				lpSiegeArms->Build(static_cast<unsigned char>(dwValue));
			}
			break;

			// 공성 병기 생성 취소 (바로됨)
			// 공성 병기를 스타트킷으로 완료
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

			// 공성 병기 수리
			// 공성 병기 스타트 킷으로 변형
		case PktSiegeArmsCmd::SIEGE_REPAIR_ARMS:	// [In/Out] dwValue1 : RepairHP
		case PktSiegeArmsCmd::SIEGE_TO_STARTKIT:
			{
				lpSiegeArms->Update(cState, dwValue, 0, 0, cSubCmd);
			}
			break;

			// 공성 병기 수리 완료
		case PktSiegeArmsCmd::SIEGE_REPAIR_ARMS_COMPLETE:
			{
				lpSiegeArms->Repair(static_cast<unsigned short>(dwValue));
			}
			break;

			
			// 공성 병기 파괴
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

// 성 관리 권한 변경
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

// 길드 요새 관리 권한 변경
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

// 공성 시간 관련 패킷
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

	// 각 성들의 공성 가능 정보 셋팅
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

	// GameTimeMgr 에 값 셋팅
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

	// 파이식별 룰 설정
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
			// 컨텐츠 : 공성전
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

	// 길드전 시간이 끝난 경우 월드웨폰 파괴정보 갱신
	if (GameTime::TYPE_CHANGE_GAME_TIME == cType &&
		CGameTimeMgr::GetInstance().IsGuildWarTime(cOldGuildTime) && !CGameTimeMgr::GetInstance().IsGuildWarTime())
	{
		CSiegeObjectMgr::GetInstance().DecreaseWeaponRemainSiegeTime();
	}

    // 공성전이 끝난 경우 
	if (GameTime::TYPE_CHANGE_GAME_TIME == cType &&
		CGameTimeMgr::GetInstance().IsSiegeWarTime(cOldSiegeTime) && !CGameTimeMgr::GetInstance().IsSiegeWarTime())
	{
		// 성 수성 병기를 병기 관리 NPC 로 바꾼다.
		CCastleMgr::GetInstance().DestroyAllCastleArms();

		// 성 세율 변경 가능 정보 갱신
		CCastleMgr::GetInstance().EnableTaxChange();
	}

	// 클라이언트에게 전송
	char* szPacket = reinterpret_cast<char *>(lpPktGTI);
	if (PacketWrap::WrapCrypt(szPacket, wSize, CmdGameTimeInfo, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, wSize, CmdGameTimeInfo);
	}

	// Instance On 인 경우 플래그 꺼주기
	if (cType == GameTime::TYPE_CHANGE_GAME_TIME)
	{
		if (!CGameTimeMgr::GetInstance().IsGuildWarTime())
		{
			CCreatureManager::GetInstance().ClearGuildWarInstanceFlag();
		}

		// 컨텐츠 : 다크 카나번 국가 전쟁
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

		// 길드전이 실행되면 강제로 이동하게 하는 로직 이게 원소스에서 사용됐다.
		if (bGuildUpdated && cOldGuildTime != cNowGuildTime && GameTime::GT_GUILD_WAR == cNowGuildTime)
		{
			// 존이동이 필요한 경우 존이동 처리
			CCreatureManager::GetInstance().MoveCharacterToGuildWarZone(GameTime::GUILD);
		}

		// 컨텐츠 : 다크 카나번 국가 전쟁
		//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
		//{
			if (bRealmUpdated && cOldRealmTime != cNowRealmTime && GameTime::RT_PREPARE_START_10 == cNowRealmTime)
			{
				// 존이동이 필요한 경우 존이동 처리
				CCreatureManager::GetInstance().MoveCharacterToRealmWarZone(GameTime::REALM);
			}

			if (bRealmUpdated && cOldRealmTime != cNowRealmTime && GameTime::RT_REALM_WAR == cNowRealmTime &&
				(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))
			{
				// 존이동이 필요한 경우 존이동 처리
				CCreatureManager::GetInstance().MoveCharacterToRealmWarZone(GameTime::REALM, false);
			}
		//}
	}

	// 컨텐츠 : 다크 카나번 국가 전쟁
	//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
	//{
		if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
		{
			// 다크 카나번에 국가전쟁 시간에만 공헌훈장 효과를 적용한다.
			if (GameTime::TYPE_CHANGE_GAME_TIME == cType)
			{
				if (CGameTimeMgr::GetInstance().IsRealmWarReadyTime(cOldRealmTime) && CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					// 인첸트에 공헌훈장 효과 추가.
					CCreatureManager::GetInstance().ProcessAllCharacter(RealmSkill::RealmInchantAdd);
				}
				else if (CGameTimeMgr::GetInstance().IsRealmWarTime(cOldRealmTime) && !CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					// 인첸트에 공헌훈장 효과 삭제.
					CCreatureManager::GetInstance().ProcessAllCharacter(RealmSkill::RealmInchantRemove);
				}
			}

			// 다크 카나번에 국가 전쟁 10분 전에 석상 초기화
			if (cOldRealmTime != cNowRealmTime && GameTime::RT_PREPARE_START_10 == cNowRealmTime)
			{
				SERLOG0(g_Log, "국가 전쟁 시간 10분 전이므로 석상 정보를 초기화 합니다.");
				CCreatureManager::GetInstance().InitRealmStatue();

				// 라지맵 표시 정보 보내기
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();

				// 석상 인챈트 효과 정보를 계산한다.
				CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();

				// edith 2008.06.04 인첸트가 국가 전쟁시 2시간 짜리로 걸리기 때문에 제외
				// 국가 전쟁 석상 인챈트 제거
//				CCreatureManager::GetInstance().ClearRealmStatueEnchant();
			}

			// 다크 카나번에 국가 전쟁 종료시 로딩중인 석상 중립화
			if (CGameTimeMgr::GetInstance().IsRealmWarTime(cOldRealmTime) && !CGameTimeMgr::GetInstance().IsRealmWarTime())
			{
				SERLOG0(g_Log, "국가 전쟁 시간이 종료되었으므로 소환중인 석상을 중립화 합니다.");
				CCreatureManager::GetInstance().InitRealmLoadingStatue();

				// 라지맵 표시 정보 보내기
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();

				// 석상 인챈트 효과 정보를 계산한다.
				CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();

				// 국가 전쟁 석상 인챈트 켜주기
				// edith 2008.06.04 인첸트가 국가 전쟁시 2시간 짜리로 걸리기
				// 석상전이 종료했으니 2시간 짜리 버프를 걸어준다.
				CCreatureManager::GetInstance().AddRealmStatueEnchant();
			}
		}
	//}
/*
	// edith 2008.06.03 이거 문제가 있지않아??
	// 공선전 컨텐츠는 석상전용 맥과 분리가 됐기때문에 아래 로직은 필요가 없다.
	// 컨텐츠 : 공성전
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
		{
			// 다크 카나번 공성전 시작시
			if (!CGameTimeMgr::GetInstance().IsSiegeWarTime(cOldSiegeTime) && CGameTimeMgr::GetInstance().IsSiegeWarTime())
			{
				// 석상 인챈트 효과 정보를 계산한다.
				CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();

				// edith 2008.06.04 인첸트가 국가 전쟁시 2시간 짜리로 걸리기 때문에 제외
				// 국가 전쟁 석상 인챈트 제거
				CCreatureManager::GetInstance().ClearRealmStatueEnchant();
			}

			// 다크 카나번에 공성전 종료시
			if (CGameTimeMgr::GetInstance().IsSiegeWarTime(cOldSiegeTime) && !CGameTimeMgr::GetInstance().IsSiegeWarTime())
			{
				// 석상 인챈트 효과 정보를 계산한다.
				CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();

				// 국가 전쟁 석상 인챈트 켜주기
				CCreatureManager::GetInstance().AddRealmStatueEnchant();
			}
		}
	}
*/
	// 카나번일 경우. 공성측은 리스폰 지역으로 이동시킨다 //			
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

// 성 임시 세금 -> 누적 세금 이동 정보 업데이트
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

// 공성 시간 후 성의 남은 공성 횟수 정보 업데이트
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

// 길드 요새 관련 메세지 패킷 전송
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

// 채굴기 프로세스 실행
bool DBAgentPacketParse::ParseProcessMining(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	// 게임이 되는 존 서버에서만 가능하다.
	char cZone = CServerSetup::GetInstance().GetServerZone();
	if (SERVER_ID::CAPITAL == cZone)
	{
		PktProcessMining* lpPktPM = static_cast<PktProcessMining*>(lpPktBase);
		unsigned long dwProcessType = lpPktPM->m_dwProcessType;

		// 채굴기 프로세스 실행
		CSiegeObjectMgr::GetInstance().ProcessMiningCamp( dwProcessType );
	}

	return true;
}

// 길드전, 국가전 플래그 처리
bool DBAgentPacketParse::ParseWarOnOff(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	// Game -> DB로 받음.
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
					// 참가 버튼 클릭으로 인스턴스가 되면 존 이동
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
				// 컨텐츠 : 다크 카나번 국가 전쟁
				//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
				//{
					StatueInfo* lpStatueInfo = reinterpret_cast<StatueInfo*>(lpPktStatueCmd + 1);
					for (unsigned long i=0; i<dwValue; ++i, ++lpStatueInfo)
					{
						CCreatureManager::GetInstance().SetRealmStatueInfo(lpStatueInfo->m_dwIndex, lpStatueInfo->m_wKID, lpStatueInfo->m_dwLostHP);
					}

					// 석상에 의한 인챈드 값을 계산한다. ( 성의 의한 보너스까지 계산한다. )
					CCreatureManager::GetInstance().CalculateRealmStatueEnchantInfo();
				//}
/*
				// 컨텐츠 : 신규존
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