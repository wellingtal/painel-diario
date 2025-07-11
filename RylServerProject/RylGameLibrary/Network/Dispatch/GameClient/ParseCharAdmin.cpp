
#include "stdafx.h"

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/Character/CharacterClass.h>
#include <Creature/Character/CharacterCreate.h>
#include <Creature/Character/ExpTable.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/AwardTable.h>
#include <Creature/Monster/VirtualMonsterMgr.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharAdmin.h>
#include <Network/Dispatch/GameClient/SendCharQuest.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/GameClient/SendCharLevelUp.h>
#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/GlobalSpellMgr.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>
#include <Map/FieldMap/VirtualArea/BGServer/BGServerMap.h>

#include <Log/LogStruct.h>
#include <Log/ItemLog.h>

#include <Community/Party/Party.h>
#include <Quest/QuestMgr.h>

#include <Item/Item.h>
#include <Item/ItemFactory.h>

#include <Utility/Filter/Filter.h>
#include <Utility/Setup/ServerSetup.h>

#include "GameClientDispatch.h"
#include "ParseCharAdmin.h"

#include <Network/Packet/PacketStruct/CastlePacket.h>

struct Respawn
{
	bool operator() (CCharacter* lpCharacter)
	{
		if (NULL != lpCharacter)
		{
			lpCharacter->Respawn();

            if (0 != lpCharacter->GetPID())
            {		
                // 파티원 리스폰을 알린다.
                GameClientSendPacket::SendCharDeadToParty(lpCharacter, 0, PktDeadInfo::RESPAWN);
            }
		}

		return true;
	}
};

bool GameClientParsePacket::ParseCharAdminCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktAdmin), GameClientDispatch);

	PktAdmin*   lpPktAdmin  = static_cast<PktAdmin*>(lpPktBase);
	CCharacter*	lpAdmin	= GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpAdmin, GameClientDispatch, lpPktBase->GetCmd());
	if (false == lpAdmin->IsAdmin())
	{
		ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : 어드민 캐릭터가 아닙니다.", lpAdmin->GetCID());
		return true;
	}

	BOOL bResult = FALSE;

	switch(lpAdmin->GetAdminLevel())
	{
/*	case 4:
		switch(lpPktAdmin->m_usCmd)
		{
		case PktAdmin::DBAGENT_KILL:
			bResult = TRUE;
			break;
		}
*/
	case 5:	// 개발자
	case 4:	// 어드민3
		switch(lpPktAdmin->m_usCmd)
		{
		case PktAdmin::RESPAWN_CHAR:		//= 5,	// 캐릭터 리스폰
		case PktAdmin::DUEL_INIT:			//= 9,	// 듀얼 초기화
		case PktAdmin::KILL_CHAR:			//= 7,	// 캐릭터 킬
		case PktAdmin::CREATE_ITEM:			//= 10,	// 아이템 드롭
		case PktAdmin::CREATE_MON:			//= 11,	// 몬스터 생성
		case PktAdmin::GET_EXP:				//= 13,	// 경험치 얻기
		case PktAdmin::GET_GOLD:			//= 14,	// 돈 얻기
		case PktAdmin::LEVEL_UP:			//= 15,	// 레벨업
		case PktAdmin::GET_FAME:			//= 18,	// 명성 얻기
		case PktAdmin::GET_MILEAGE:			//= 19,	// 마일리지 얻기
		case PktAdmin::APOCALYPSE:			//= 26,	// 주위 생물체 모두 킬
		case PktAdmin::CREATE_EQUIP:		//= 27,	// 랜덤 옵션이 붙는 장비 드롭
		case PktAdmin::ELITE_BONUS:			//= 28,	// 엘리트 보너스 조절
		case PktAdmin::STATUE_INIT:			//= 29,	// 명예의 석상 초기화
		case PktAdmin::RESPAWN_ALL:			//= 30,	// 모든 캐릭터 리스폰
		case PktAdmin::CHANGE_CLASS:		//= 31,	// 전직 하기
		case PktAdmin::LEARN_SKILL:			//= 32,	// 해당 스킬 배우기
		case PktAdmin::ERASE_SKILL:			//= 33,	// 해당 스킬 지우기
		case PktAdmin::LEVEL_DOWN:			//= 34,	// 레벨 다운
		case PktAdmin::GET_QUEST:			//= 35,	// 퀘스트 받음
		case PktAdmin::ADD_QUEST:			//= 36,	// 완료 퀘스트에 추가
		case PktAdmin::DELETE_QUEST:		//= 37,	// 수행/완료 퀘스트를 삭제
		case PktAdmin::SET_ITEM_TEMP_INVEN:	//= 45,	// 임시 인벤토리에 아이템 넣기
		case PktAdmin::ADD_SPELL:			//= 46,	// 해당 스펠을 부여
		case PktAdmin::REALM_POINT:			//= 58,	// 국가 전쟁 공헌 훈장.

		case PktAdmin::LEARN_ABILITY:		//= 60,	// 해당 스킬 배우기.
		case PktAdmin::ERASE_ABILITY:		//= 61,	// 해당 스킬 지우기


		// BG
		case PktAdmin::FINISH_GAME:			//= 38,	// 해당 방의 경기 종료 (승패 결정)
		case PktAdmin::DRAW_GAME:			//= 39,	// 해당 방의 경기 종료 (무승부)
		case PktAdmin::IGNORE_GAME:			//= 40,	// 해당 방의 경기 종료 (승패 무시)
		case PktAdmin::SET_MAX:				//= 41,	// 해당 방의 최대 인원수 설정
		case PktAdmin::SET_SCORE:			//= 42,	// 해당 방의 목표 점수 설정
		case PktAdmin::SET_LIMIT_TIME:		//= 43,	// 해당 방의 시간 제한 설정
		case PktAdmin::SET_REST_TIME:		//= 44,	// 해당 방의 쉬는 시간 설정
			bResult = TRUE;
			break;
		}
	
	case 3:	// 어드민2
		switch(lpPktAdmin->m_usCmd)
		{
		case PktAdmin::MOVE_ZONE_CHAR:		//= 1,	// 캐릭터 존 이동
		case PktAdmin::MOVE_ZONE_PARTY:		//= 2,	// 파티 존 이동
		case PktAdmin::MOVE_CHAR:			//= 3,	// 캐릭터 이동
		case PktAdmin::MOVE_PARTY:			//= 4,	// 파티 이동

		case PktAdmin::DELETE_SPELL:		//= 47,	// 부여된 해당 스펠을 파괴
		case PktAdmin::STOP:				//= 48,	// 해당 캐릭터 묶어두기
		case PktAdmin::STOP_OFF:			//= 49,	// 묶어둔 해당 캐릭터를 풀어주기
			bResult = TRUE;
			break;
		}

	case 2:	// 어드민1
		switch(lpPktAdmin->m_usCmd)
		{
		case PktAdmin::HIDE_CHAR:			//= 21,	// 캐릭터 투명
		case PktAdmin::SHOW_CHAR:			//= 22,	// 캐릭터 투명 해제
		case PktAdmin::INFO_CHAR:			//= 12,	// 캐릭터 정보
		case PktAdmin::PEACE_MODE:			//= 16,	// 반전단체 가입
		case PktAdmin::WAR_MODE:			//= 17,	// 반전단체 탈퇴
		case PktAdmin::TRACE_CHAR:			//= 20,	// 캐릭터 추적
		case PktAdmin::INVINCIBLE_CHAR:		//= 23,	// 캐릭터 무적
		case PktAdmin::LOGOUT_CHAR:			//= 25,	// 캐릭터 강제 로그아웃
		case PktAdmin::NORMAL_CHAR:			//= 24,	// 캐릭터 무적 해제
		case PktAdmin::CHAT_BAN:			//= 50,	// 채팅 금지
		case PktAdmin::CHAT_OK:				//= 51,	// 채금 해제
		case PktAdmin::NEO:					//= 52, // 네오
		case PktAdmin::SMITH:				//= 53, // 스미스모드
		case PktAdmin::RELATION:			//= 55,	// 적아군 식별.
		case PktAdmin::GM_FORM:				//= 56,	// PC 에서 GM 으로.
		case PktAdmin::PC_FORM:				//= 57,	// GM 에서 PC 로.
			bResult = TRUE;
			break;
		}
	}

	if(bResult == FALSE)
	{
		ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %d 레벨 어드민 유저입니다.", lpAdmin->GetCID(), lpAdmin->GetAdminLevel());
		return true;
	}

/*    
	// 서포트 이면 리턴
	if(lpAdmin->GetAdminLevel() <= 1)
	{
		ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : 서포트 유저입니다.", lpAdmin->GetCID());
		return true;
	}
*/
	// 해당 캐릭터가 현재 게임 서버에 없다면.. 중계 서버로 패킷을 보낸다.
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(lpPktAdmin->m_stName);
	if (NULL == lpCharacter)
	{
		// edith 이패킷은 무시
		switch(lpPktAdmin->m_usCmd)
		{
		case PktAdmin::INFO_CHAR:
		case PktAdmin::DBAGENT_KILL:
			return true;
		}

		ERRLOG2(g_Log, "Admin:0x%08x 현재 서버에 %s 캐릭터가 존재하지 않습니다.\n다른 게임 서버로 패킷을 전송합니다.",
				lpAdmin->GetCID(), lpPktAdmin->m_stName);

		GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
			CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

		if (NULL == lpDBAgentDispatch)
		{
			ERRLOG0(g_Log, "에이전트 얻기 실패.");                     
		}
		else
		{
			DETLOG3(g_Log, "어드민 명령 요청 - 어드민:0x%08x 캐릭터:%s Cmd:%d",
					lpAdmin->GetCID(), lpPktAdmin->m_stName, lpPktAdmin->GetCmd());

			// Admin 캐릭터의 CID 를 기억해둔다.
			lpPktAdmin->m_dwAdminCID = lpAdmin->GetCID();

			// 중계 서버로 전송
			GameClientSendPacket::SendCharAdminCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), lpPktBase);
		}

		return true;
	}

	return ProcessAdminCmd(lpAdmin, lpCharacter, lpPktAdmin);
}

bool GameClientParsePacket::ProcessAdminCmd(CCharacter* lpAdmin, CCharacter* lpTarget, PktAdmin* lpPktAdmin)
{
	// 클라이언트에서 온 패킷 / 중계 서버에서 온 패킷
	unsigned long dwAdminCID = (NULL != lpAdmin) ? lpAdmin->GetCID() : lpPktAdmin->m_dwAdminCID;

	POS& DestPos = lpPktAdmin->m_Position;
	unsigned long dwAmount = lpPktAdmin->m_dwAmount;

	char szCommand[PktAdminCommandLog::MAX_COMMAND_SIZE] = {0, };

	switch (lpPktAdmin->m_usCmd)
	{
		case PktAdmin::NEO:					//= 52, // 네오
			{
					lpAdmin->GetSpellMgr().GetAffectedInfo().ClearAll();
					lpAdmin->GetSpellMgr().GetCastingInfo().ClearAll();

					lpAdmin->GetEnchantInfo().ClearFlag();
					lpAdmin->GetEnchantInfo().SetFlag(Skill::SpellID::Hide);

					sprintf(szCommand, "/neo");

					// 해당 메시지 들은 다시 클라이언트로 되돌려보낸다
					CGameClientDispatch* lpDispatch = lpTarget->GetDispatcher();
					if (NULL != lpDispatch)
					{
						GameClientSendPacket::SendCharAdmin(lpDispatch->GetSendStream(), lpPktAdmin, 0);
					}
			}
			break;

		case PktAdmin::SMITH:				//= 53, // 스미스모드
			{
					sprintf(szCommand, "/smith");

					// 해당 메시지 들은 다시 클라이언트로 되돌려보낸다
					CGameClientDispatch* lpDispatch = lpTarget->GetDispatcher();
					if (NULL != lpDispatch)
					{
						GameClientSendPacket::SendCharAdmin(lpDispatch->GetSendStream(), lpPktAdmin, 0);
					}
			}
			break;

		case PktAdmin::MOVE_ZONE_CHAR:
		{

				if (false == lpTarget->MoveZone(DestPos, lpPktAdmin->m_ZoneInfo.m_cZone, lpPktAdmin->m_ZoneInfo.m_cChannel))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s를 존 이동시키는데 실패하였습니다.", 
						dwAdminCID, lpPktAdmin->m_stName);
					return true;
				}
			sprintf(szCommand, "/move %s %d %f %f %f", lpTarget->GetCharacterName(), lpPktAdmin->m_ZoneInfo.m_cZone, DestPos.fPointX, DestPos.fPointY, DestPos.fPointZ);

			break;
		} 

		case PktAdmin::MOVE_ZONE_PARTY:
		{

				CParty* lpParty = lpTarget->GetParty();
				if (NULL == lpParty)
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s의 파티 얻기에 실패하였습니다.", 
						dwAdminCID, lpPktAdmin->m_stName);
					return true;
				}
				lpParty->MoveZone(DestPos, lpPktAdmin->m_ZoneInfo.m_cZone, lpPktAdmin->m_ZoneInfo.m_cChannel);
				sprintf(szCommand, "/moveparty %s %d %f %f %f", lpTarget->GetCharacterName(), lpPktAdmin->m_ZoneInfo.m_cZone, DestPos.fPointX, DestPos.fPointY, DestPos.fPointZ);
			
			break;
		} 

		case PktAdmin::MOVE_CHAR:
		{

				if (false == lpTarget->MovePos(DestPos, lpPktAdmin->m_ZoneInfo.m_cZone, false))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s를 위치 이동시키는데 실패하였습니다.", 
						dwAdminCID, lpPktAdmin->m_stName);
					return true;
				}

				sprintf(szCommand, "/moveit %s %f %f %f", lpTarget->GetCharacterName(), DestPos.fPointX, DestPos.fPointY, DestPos.fPointZ);
			
			break;
		} 
	
		case PktAdmin::MOVE_PARTY:
		{

				CParty* lpParty = lpTarget->GetParty();
				if (NULL == lpParty)
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s의 파티 얻기에 실패하였습니다.", 
						dwAdminCID, lpPktAdmin->m_stName);
					return true;
				}

				lpParty->MovePos(DestPos, lpPktAdmin->m_ZoneInfo.m_cZone, false);

				sprintf(szCommand, "/moveitparty %s %f %f %f", lpTarget->GetCharacterName(), DestPos.fPointX, DestPos.fPointY, DestPos.fPointZ);
			
			break;
		} 

		case PktAdmin::RESPAWN_CHAR:
		{

				if (false == lpTarget->Respawn()) 
				{	
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s를 리스폰시키는데 실패하였습니다.", 
						dwAdminCID, lpPktAdmin->m_stName);
					return true;
				}
				else
				{
					if (0 != lpTarget->GetPID())
					{		
						// 파티원 리스폰을 알린다.
						GameClientSendPacket::SendCharDeadToParty(lpTarget, 0, PktDeadInfo::RESPAWN);
					}

					sprintf(szCommand, "/respawn %s", lpTarget->GetCharacterName());
				}
			
				break;
		} 

		case PktAdmin::RESPAWN_PARTY:		break;
		case PktAdmin::KILL_CHAR:
		{

				if (false == lpTarget->Kill(lpAdmin))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s를 죽이는데 실패하였습니다.", 
						dwAdminCID, lpPktAdmin->m_stName);
					return true;
				}

				sprintf(szCommand, "/kick %s", lpTarget->GetCharacterName());
			
			break;
		} 

		case PktAdmin::KILL_PARTY:			break;
		case PktAdmin::DUEL_INIT:	
		{

				if (false == lpTarget->DuelInit(PktDuC::DUC_CANCEL))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s의 듀얼 초기화에 실패하였습니다.", 
						dwAdminCID, lpPktAdmin->m_stName);
					return true;
				}

				sprintf(szCommand, "/duelinit %s", lpTarget->GetCharacterName());
			
			break;
		} 
	
		case PktAdmin::CREATE_ITEM:
		{

				if (false == lpTarget->DropItem(lpPktAdmin->m_usProtoTypeID, static_cast<unsigned char>(dwAmount)))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s 앞에 아이템을 생성하여 떨구는데 실패하였습니다.", 
						dwAdminCID, lpPktAdmin->m_stName);
					return true;
				}

				sprintf(szCommand, /*"/dropitem %s %d %d"*/"/drop %s %d %d" , lpTarget->GetCharacterName(), lpPktAdmin->m_usProtoTypeID, dwAmount);

				DETLOG3(g_Log, "CID:0x%08x 어드민이 아이템을 생성하였습니다. 아이템(ID:%d, 갯수:%d)", 
					lpTarget->GetCID(), lpPktAdmin->m_usProtoTypeID, dwAmount);
			
			break;
		}

		case PktAdmin::CREATE_MON:	
		{

				// 배틀 그라운드 서버군의 가상존에서는 몬스터를 만들어 낼수 없다!!
 				if (0 != lpAdmin->GetMapIndex())
				{
	/*				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(lpAdmin->GetMapIndex());
					if (lpVirtualArea)
					{
						CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
						if (NULL == lpVirtualMonsterMgr || 
							false == lpVirtualMonsterMgr->AdminSummonMonster(lpPktAdmin->m_usProtoTypeID, DestPos, lpAdmin->GetMapIndex()))
						{
							ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : 몬스터를 생성하는데 실패하였습니다.", dwAdminCID);
							return true;
						}
					}
	*/
				}
				else
				{
					if (false == CCellManager::GetInstance().AdminSummonMonster(lpPktAdmin->m_usProtoTypeID, DestPos))
					{
						ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : 몬스터를 생성하는데 실패하였습니다.", dwAdminCID);
						return true;
					}
				}

				sprintf(szCommand, "/createmob %d %f %f %f", lpPktAdmin->m_usProtoTypeID, DestPos.fPointX, DestPos.fPointY, DestPos.fPointZ);

				DETLOG5(g_Log, "CID:0x%08x 어드민이 몬스터를 소환하였습니다. 몬스터(ID:%d, (%.1f, %.1f, %.1f))", 
						dwAdminCID, lpPktAdmin->m_usProtoTypeID, DestPos.fPointX, DestPos.fPointY, DestPos.fPointZ);
			
			break;
		}

		// PC 에서 GM 으로.
		case PktAdmin::GM_FORM:
		{
			
			CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwAdminCID);
			
			if(lpCharacter)
			{
				if(lpCharacter->IsAdmin())
				lpCharacter->SetGMModelFlag(1);
			}
			if(lpCharacter->IsAdmin())
			sprintf(szCommand, "/gmform");

			break;
		}

			// GM 에서 PC 로.
		case PktAdmin::PC_FORM:
		{
			CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwAdminCID);

				if(lpCharacter)
				{
					lpCharacter->SetGMModelFlag(0);
				}

				sprintf(szCommand, "/pcform");
			
			break;
		}

		case PktAdmin::INFO_CHAR:		
		{
			if (false == lpTarget->NotifyInfo(dwAdminCID))
			{
				ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s의 정보 전달에 실패하였습니다.", 
					dwAdminCID, lpPktAdmin->m_stName);
				return true;
			}

			sprintf(szCommand, "/hp %s", lpTarget->GetCharacterName());

			break;
		} 
		
		case PktAdmin::RELATION:
		{
			unsigned char cResult = 0;
			unsigned long dwResult = lpAdmin->IsEnemy(lpTarget, &cResult);

			char szMessage[PktChat::PktChatMaxSize] = "";
			char szType[MAX_PATH] = { 0, };	

			int nLen = _snprintf(szMessage, PktChat::PktChatMaxSize, 
				"[Client] Type : %d, Relation : %d, Result : %d, [Server] Type : %d, Relation : %d, Result : %d", 
				lpPktAdmin->m_usProtoTypeID, lpPktAdmin->m_dwAmount, lpPktAdmin->m_ZoneInfo.m_cZone, 
				EnemyCheck::CCheckTable::GetInstance().GetEnemyCheckRule(), dwResult, cResult);
				
			if (0 < nLen)
			{
				szMessage[PktChat::PktChatMaxSize - 1] = 0;

				// 채팅 서버로 정보를 보낸다.
				GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, 
					CChatDispatch::GetDispatchTable());

				if (0 != lpChatDispatch)
				{					
					const Position& pos = lpTarget->GetCurrentPos();
					char strAdminCID[CHAR_INFOST::MAX_NAME_LEN] = "";
					strcpy(strAdminCID, "0x");
					char* strHexPos = (strAdminCID + 2);
					Math::Convert::Hex32ToStr(strHexPos, dwAdminCID);

					CChatRequestPacket chatReqPacket(szMessage, 
						PktChat::ENEMY_CHECK, 0, lpTarget->GetUID(), lpTarget->GetCID(), 0, 0, 0, strAdminCID, 1);

					if (chatReqPacket.IsValid())
					{
						return lpChatDispatch->GetSendStream().PutBuffer(
							chatReqPacket.GetCompressedPacket(),
							chatReqPacket.GetCompressedSize(), CmdCharChat);
					}
				}
			}

			sprintf(szCommand, "/relation %s", lpTarget->GetCharacterName());
			
			break;
		}

		case PktAdmin::GET_EXP:
		{

				lpTarget->IncrementExp(dwAmount);

				sprintf(szCommand, "/giveexp %s %d", lpTarget->GetCharacterName(), dwAmount);
			
			break;
		} 

		case PktAdmin::GET_GOLD:
		{

				unsigned long dwSrcGold = lpTarget->GetGold();
				lpTarget->AddGold(dwAmount, true);
				GAMELOG::LogTakeGold(*lpTarget, dwSrcGold, lpTarget->GetGold(), dwAmount, 
					TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::ADMIN_GIVEGOLD, 0);

				sprintf(szCommand, "/givegold %s %d", lpTarget->GetCharacterName(), dwAmount);
			
			break;
		} 

		case PktAdmin::LEVEL_UP:
		{

				if (dwAmount < 1 || dwAmount > EXP::GetUsingMaxLevel())
				{
					break;
				}

				while (static_cast<unsigned long>(lpTarget->GetStatus().m_nLevel) < dwAmount)
				{
					if (false == lpTarget->IncrementExp(
						static_cast<unsigned long>(EXP::ExpTable[lpTarget->GetStatus().m_nLevel - 1])))
					{
						break;
					}
				}

				sprintf(szCommand, "/levelup %s %d", lpTarget->GetCharacterName(), dwAmount);
			
			break;
		} 

// DELETE_ME : 더이상 클라이언트가 반전 모드로의 변경은 불가능하다. 개념만 남아있을 뿐. (2005-05-31 by 로딘)
/*
		case PktAdmin::PEACE_MODE:
		{
			PeaceModeInfo peaceModeInfo = lpTarget->GetPeaceMode();
			peaceModeInfo.m_bPeace = true;
			lpTarget->SetPeaceMode(peaceModeInfo, false);

			CGameClientDispatch* lpDispatch = lpTarget->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharPeaceMode(lpDispatch->GetSendStream(), 
					lpTarget->GetCID(), 0, true, 0);
			}
			break;
		} 

		case PktAdmin::WAR_MODE:
		{
			PeaceModeInfo peaceModeInfo = lpTarget->GetPeaceMode();
			peaceModeInfo.m_bPeace = false;
			lpTarget->SetPeaceMode(peaceModeInfo, false);

			CGameClientDispatch* lpDispatch = lpTarget->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharPeaceMode(lpDispatch->GetSendStream(), 
					lpTarget->GetCID(), 0, false, 0);
			}
			break;
		} 
*/

		case PktAdmin::GET_FAME:
		{

				lpTarget->SetFame(dwAmount);

				CGameClientDispatch* lpDispatch = lpTarget->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpTarget, 
						"", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);
				}
			
				sprintf(szCommand, /*"/setfame %s %d"*/"/setfame %s %d", lpTarget->GetCharacterName(), dwAmount);
			
			break;
		} 

		case PktAdmin::GET_MILEAGE:
		{
				lpTarget->SetMileage(dwAmount);

				CGameClientDispatch* lpDispatch = lpTarget->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpTarget, 
						"", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);
				}

				sprintf(szCommand, /*"/setmileage %s %d"*/"/setmileage %s %d", lpTarget->GetCharacterName(), dwAmount);
			break;
		} 

		case PktAdmin::TRACE_CHAR:
		{
			
			// 목표와 같은 존에 있는 경우
			if (NULL != lpAdmin)
			{
					Position DestPostion = lpTarget->GetCurrentPos();

					// 약간 옆으로 이동시켜준다.
					DestPostion.m_fPointX += 5;

					sprintf(szCommand, "/trace %s", lpTarget->GetCharacterName());

					if (false == lpAdmin->MovePos(DestPostion, lpPktAdmin->m_ZoneInfo.m_cZone, false))
					{
						ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s의 위치로 이동하는데 실패하였습니다.", 
							dwAdminCID, lpPktAdmin->m_stName);
						return true;
					}			
			}
			// 목표와 다른  존에 있는 경우
			else
			{
				// Admin 캐릭터를 목표 캐릭터가 있는 존과 위치로 이동 시킨다.
				CCharacter* lpAdmin = CCreatureManager::GetInstance().GetCharacter(dwAdminCID);
				if (NULL == lpAdmin)
				{
					ERRLOG1(g_Log, "어드민 명령 오류 : %s 캐릭터를 추적하는데 실패하였습니다. 어드민 캐릭터가 NULL 입니다.", 
						lpPktAdmin->m_stName);
					return true;
				}
					sprintf(szCommand, "/trace %s", lpAdmin->GetCharacterName());

					if (0 == DestPos.fPointX && 0 == DestPos.fPointY && 0 == DestPos.fPointZ)
					{
						CGameClientDispatch* lpDispatch = lpAdmin->GetDispatcher();
						if (NULL != lpDispatch)
						{
							GameClientSendPacket::SendCharAdmin(lpDispatch->GetSendStream(), lpPktAdmin, PktAdmin::E_LOGOUT_CHAR);
						}

						return true;
					}
					else
					{
						// 약간 옆으로 이동시켜준다.
						DestPos.fPointX += 5;

						if (false == lpAdmin->MoveZone(DestPos, lpPktAdmin->m_ZoneInfo.m_cZone, lpPktAdmin->m_ZoneInfo.m_cChannel))
						{
							ERRLOG1(g_Log, "어드민 명령 오류 : %s 캐릭터를 추적하는데 실패하였습니다.", lpPktAdmin->m_stName);
							return true;
						}
					}
			}

			break;
		} 

		case PktAdmin::REQUEST_TRACE_POS:
		{
				if (NULL != lpPktAdmin)
				{
					lpPktAdmin->m_Position.fPointX = lpTarget->GetCurrentPos().m_fPointX;
					lpPktAdmin->m_Position.fPointY = lpTarget->GetCurrentPos().m_fPointY;
					lpPktAdmin->m_Position.fPointZ = lpTarget->GetCurrentPos().m_fPointZ;

					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
					if (NULL != lpDBAgentDispatch)
					{
						GameClientSendPacket::SendCharAdminCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), lpPktAdmin);
					}
				}
			break;
		}

		case PktAdmin::HIDE_CHAR:		
		{
			if(lpAdmin->IsAdmin())
			{
				lpAdmin->GetSpellMgr().GetAffectedInfo().ClearAll();
				lpAdmin->GetSpellMgr().GetCastingInfo().ClearAll();

				lpAdmin->GetEnchantInfo().ClearFlag();
				lpAdmin->GetEnchantInfo().SetFlag(Skill::SpellID::Hide);

				sprintf(szCommand, "/hide");
			}
			break;
		} 

		case PktAdmin::SHOW_CHAR:		
		{
			if(lpAdmin->IsAdmin())
			{
				lpAdmin->GetEnchantInfo().ResetFlag(Skill::SpellID::Hide);

				sprintf(szCommand, "/show");
			}
			break;
		} 

		case PktAdmin::INVINCIBLE_CHAR:
		{
			if(lpAdmin->IsAdmin())
			{
				Skill::CAddSpell<CInvincibleSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpAdmin, 
					Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Invincible, 1, CSpell::INFINITE_DURATION))(lpAdmin);

				sprintf(szCommand, "/god");
			}
			break;
		} 

		case PktAdmin::NORMAL_CHAR:
		{
			if(lpAdmin->IsAdmin())
			{
				lpAdmin->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Invincible);
			
				sprintf(szCommand, "/human");
			}
			break;
		} 

		case PktAdmin::LOGOUT_CHAR:
		{
				CGameClientDispatch* lpDispatch = lpTarget->GetDispatcher();
				if (NULL != lpDispatch)
				{
					DETLOG2(g_Log, "CID:0x%08x 어드민이 강제 로그아웃 시켰습니다. 어드민CID:0x%08x", 
						lpTarget->GetCID(), dwAdminCID);

					lpDispatch->Disconnect();
					lpDispatch->Disconnected();
				}

			break;
		} 

		case PktAdmin::APOCALYPSE:
		{
			if(lpAdmin->IsAdmin())
			{
				CCell* lpCell = lpAdmin->GetCellPos().m_lpCell;
				if (NULL == lpCell) 
				{ 
					ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : 어드민 캐릭터가 있는 셀이 이상합니다.", dwAdminCID);
					break; 
				}

				lpCell->KillAll(lpAdmin);

				sprintf(szCommand, "/killall");
			}
			break;
		} 

		case PktAdmin::CREATE_EQUIP:
		{
				CCell* lpCell = lpTarget->GetCellPos().m_lpCell;
				if (NULL == lpCell)
				{
					ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : 캐릭터가 있는 셀이 이상합니다.", lpTarget->GetCID());
					break;
				}

				lpPktAdmin->m_usProtoTypeID = std::max(lpPktAdmin->m_usProtoTypeID, static_cast<unsigned short>(Item::EquipType::AAA_GRADE));
				lpPktAdmin->m_usProtoTypeID = std::min(lpPktAdmin->m_usProtoTypeID, static_cast<unsigned short>(Item::EquipType::F_GRADE));
				Item::EquipType::Grade eGrade = static_cast<Item::EquipType::Grade>(lpPktAdmin->m_usProtoTypeID);

				unsigned char cMonsterLevel = 
					std::min(static_cast<unsigned char>(Creature::MONSTER_MAX_LEVEL), static_cast<unsigned char>(lpPktAdmin->m_dwAmount));

				unsigned short wItemKind = AwardTable::CAward::GetInstance().GetAwardEquipment(
					Item::EquipType::STANDARD_OPTION, cMonsterLevel, lpTarget, true);

				Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wItemKind);
				if (NULL != lpItem)
				{
					Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
					if (NULL != lpEquip)
					{
						// 매직찬스 50프로 확율 테스트.
						if(lpEquip->AddRandomOption(eGrade, (unsigned char)lpPktAdmin->m_dwAmount2, 50))
							lpEquip->SetNewEquip(2);
						else
							lpEquip->SetNewEquip();
					}

					lpTarget->GiveItem(lpItem);

					// GievItem 으로 스택된 경우
					if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
					{
						DELETE_ITEM(lpItem);
					}
				}

				sprintf(szCommand, /*"/createitem %s %d %d"*/"/create %s %d %d", lpTarget->GetCharacterName(), lpPktAdmin->m_dwAmount, lpPktAdmin->m_usProtoTypeID);
			break;
		} 

		case PktAdmin::ELITE_BONUS:
		{
				CCreatureManager& CreatureManager = CCreatureManager::GetInstance();

				EliteBonus::EliteBonusData eliteBonus(lpPktAdmin->m_EliteBonusInfo.m_cNation, lpPktAdmin->m_EliteBonusInfo.m_cLevel);
				if ((eliteBonus.m_cNation == CClass::HUMAN || eliteBonus.m_cNation == CClass::AKHAN) && 
					(eliteBonus.m_cLevel >= 0 && eliteBonus.m_cLevel <= EliteBonus::MAX_BONUS_LEVEL))
				{
					CreatureManager.SetAutoBalance(false);
					CreatureManager.SetEliteBonus(eliteBonus);
				}
				else
				{
					CreatureManager.SetAutoBalance(true);
				}

				sprintf(szCommand, "/elitebonus %d %d", lpPktAdmin->m_EliteBonusInfo.m_cNation, lpPktAdmin->m_EliteBonusInfo.m_cLevel);
			
			break;
		} 

		case PktAdmin::STATUE_INIT:
		{
			if(lpAdmin->IsAdmin())
			{
				if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
				{
					CCreatureManager::GetInstance().ProcessAllMonster(
						std::bind2nd(std::mem_fun1(&CMonster::Dead), reinterpret_cast<CAggresiveCreature *>(NULL)));
				}

				sprintf(szCommand, "/resstatue");
			}
			break;
		} 

		case PktAdmin::RESPAWN_ALL:
		{
				CCreatureManager::GetInstance().ProcessAllCharacter(Respawn());

				sprintf(szCommand, "/resmen");
			
			break;
		}

		case PktAdmin::CHANGE_CLASS:
		{

				if (lpTarget->GetStatus().m_nLevel < 10)
				{
					ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : 레벨이 20보다 작습니다.",	dwAdminCID);
					break;
				}

				unsigned char cClassType = static_cast<unsigned char>(lpPktAdmin->m_usProtoTypeID);
				if (lpTarget->GetClass() == cClassType)
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : 같은 클래스(%d)로 전직할려고 합니다.",
							dwAdminCID, cClassType);
					break;
				}

				/*if (lpTarget->GetRace() != CClass::GetRace(cClassType))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : 체인지 할려는 클래스(%d)와 종족이 다릅니다.", 
							dwAdminCID, cClassType);
					break;
				}*/

				// 기본 클래스에서 해당 상위 클래스로 전직하는 경우
				if (CClass::GetJobLevel(static_cast<unsigned char>(lpTarget->GetClass())) == CClass::DEFAULT_CLASS &&
					CClass::GetPreviousJob(cClassType) == lpTarget->GetClass())
				{
					if (false == lpTarget->ChangeClass(cClassType))
					{
						ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %d 로 클래스 체인지를 실패하였습니다.", 
								dwAdminCID, lpPktAdmin->m_usProtoTypeID);
					}
				}

				// 같은 종족의 다른 클래스로 전직하는 경우
				else
				{
					unsigned char cPrevClassType = CClass::GetPreviousJob(cClassType);
					int nCurrentLevel = lpTarget->GetStatus().m_nLevel;

					// 1 Step
					// 레벨 1의 초기 캐릭터로 만들면서, 모든 스킬을 지운다.
					if (false == lpTarget->InitLevel1Char(cPrevClassType))
					{
						ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : CID:0x%08x 캐릭터를 1레벨로 만드는데 실패하였습니다.",
								dwAdminCID, lpTarget->GetCID());
						break;
					}

					// 2 Step
					// 해당 레벨까지 level up
					while (lpTarget->GetStatus().m_nLevel < nCurrentLevel)
					{
						if (false == lpTarget->IncrementExp(
							static_cast<unsigned long>(EXP::ExpTable[lpTarget->GetStatus().m_nLevel - 1])))
						{
							break;
						}
					}

					// 3 Step
					// 해당 클래스로 전직
					if (lpTarget->GetStatus().m_nLevel >= 10 && cClassType != cPrevClassType)
					{
						if (false == lpTarget->ChangeClass(cClassType))
						{
							ERRLOG4(g_Log, "CID:0x%08x 어드민 명령 오류 : CID:0x%08x 캐릭터 클래스 체인지 실패. 현재 캐릭터의 클래스는 %d, 바꾸고자 하는 클래스는 %d입니다.",
								dwAdminCID, lpTarget->GetCID(), lpTarget->GetClass(), cClassType);
							break;
						}
					}
				}

				sprintf(szCommand, "/class %s %d", lpTarget->GetCharacterName(), cClassType);
			
			break;
		}

		case PktAdmin::LEARN_SKILL:
		{

				unsigned short nSkillID		= lpPktAdmin->m_usProtoTypeID;
				unsigned char cLockCount	= static_cast<unsigned char>(lpPktAdmin->m_Position.fPointX) - 1;	// 1부터 시작으로 입력 받음
				unsigned char cSkillLevel	= static_cast<unsigned char>(lpPktAdmin->m_Position.fPointY);		// 0부터 시작으로 입력 받음

				// 이미 배워둔 스킬인지 체크
				if (cLockCount < lpTarget->GetSkillLockCount(nSkillID) ||
					(cLockCount == lpTarget->GetSkillLockCount(nSkillID) && cSkillLevel <= lpTarget->GetSkillLevel(nSkillID)))
				{
					ERRLOG4(g_Log, "CID:0x%08x 어드민 명령 오류 : SkillID:0x%08x LockCount:%d SkillLevel:%d 스킬을 배우는데 실패하였습니다.", 
							dwAdminCID, nSkillID, cLockCount, cSkillLevel);
					break;
				}

				// 스킬 배우기
				int count = cLockCount * CSkillMgr::MAX_SKILL_LEVEL + cSkillLevel + 1;
				cLockCount = lpTarget->GetSkillLockCount(nSkillID) == -1 ? 0 : lpTarget->GetSkillLockCount(nSkillID);
				cSkillLevel = lpTarget->GetSkillLevel(nSkillID) == -1 ? 0 : lpTarget->GetSkillLevel(nSkillID);
				count -= cLockCount * CSkillMgr::MAX_SKILL_LEVEL + cSkillLevel + 1;
			
				using namespace Item;

				ItemInfo tempItemInfo(nSkillID);
				tempItemInfo.m_UseItemInfo.m_usSkill_ID         = nSkillID;
				tempItemInfo.m_UseItemInfo.m_usSkill_LockCount  = cLockCount;
				tempItemInfo.m_DetailData.m_dwFlags |= DetailData::USE_ITEM;

				// ItemSerial 이 0 인 임시 아이템을 생성한다.
				CUseItem* lpUseItem = CUseItem::DowncastToUseItem(
					CItemFactory::GetInstance().CreateTempItem(tempItemInfo));

				for (int i=0; i<count; ++i)
				{
					if (!lpTarget->SkillCreate(lpUseItem))
					{
						ERRLOG4(g_Log, "CID:0x%08x 어드민 명령 오류 : SkillID:0x%08x LockCount:%d SkillLevel:%d 스킬을 배우는데 실패하였습니다.", 
								dwAdminCID, nSkillID, cLockCount, cSkillLevel);
					}

					++cSkillLevel;
					if (cLockCount != CSkillMgr::MAX_SKILL_LOCKCOUNT-1 && cSkillLevel == CSkillMgr::MAX_SKILL_LEVEL)
					{
						cSkillLevel = 0;
						++cLockCount;

						DELETE_ITEM(lpUseItem);

						tempItemInfo.m_usProtoTypeID = nSkillID;
						tempItemInfo.m_UseItemInfo.m_usSkill_ID = nSkillID;
						tempItemInfo.m_UseItemInfo.m_usSkill_LockCount = cLockCount;

						lpUseItem = CUseItem::DowncastToUseItem(
							CItemFactory::GetInstance().CreateTempItem(tempItemInfo));
					}
				}

				sprintf(szCommand, "/learnskill 0x%08x %d %d", nSkillID, cLockCount+1, cSkillLevel);

				DELETE_ITEM(lpUseItem);
			
			break;
		}

		case PktAdmin::ERASE_SKILL:
		{

				unsigned short nSkillID		= lpPktAdmin->m_usProtoTypeID;
				unsigned char cLockCount	= static_cast<unsigned char>(lpPktAdmin->m_Position.fPointX) - 1;	// 1부터 시작으로 입력 받음
				unsigned char cSkillLevel	= static_cast<unsigned char>(lpPktAdmin->m_Position.fPointY);		// 0부터 시작으로 입력 받음

				if (cLockCount > lpTarget->GetSkillLockCount(nSkillID) ||
					(cLockCount == lpTarget->GetSkillLockCount(nSkillID) && cSkillLevel >= lpTarget->GetSkillLevel(nSkillID)))
				{
					ERRLOG4(g_Log, "CID:0x%08x 어드민 명령 오류 : SkillID:0x%08x LockCount:%d SkillLevel:%d 스킬을 지우는데 실패하였습니다.", 
							dwAdminCID, nSkillID, cLockCount, cSkillLevel);
					break;
				}

				unsigned char cIndex = static_cast<unsigned char>(lpTarget->GetSkillSlotIndex(nSkillID));
				int count = lpTarget->GetSkillLockCount(nSkillID) * CSkillMgr::MAX_SKILL_LEVEL + lpTarget->GetSkillLevel(nSkillID) + 1;
				count -= cLockCount * CSkillMgr::MAX_SKILL_LEVEL + cSkillLevel + 1;

				for (int i=0; i<count; ++i)
				{
					Item::ItemPos itemPos(TakeType::TS_ADMIN, 0);

					if (false == lpTarget->SkillErase(cIndex, itemPos))
					{
						ERRLOG4(g_Log, "CID:0x%08x 어드민 명령 오류 : SkillID:0x%08x LockCount:%d SkillLevel:%d 스킬을 지우는데 실패하였습니다.", 
								dwAdminCID, nSkillID, cLockCount, cSkillLevel);
					}

					--cSkillLevel;
					if (cSkillLevel < 0)
					{
						cSkillLevel = 0;
						--cLockCount;
					}
				}

				sprintf(szCommand, "/eraseskill 0x%08x %d %d", nSkillID, cLockCount+1, cSkillLevel);
			
			break;
		}

		case PktAdmin::LEARN_ABILITY:
		{

				unsigned short	wAbilityID  = lpPktAdmin->m_usProtoTypeID;
				unsigned short	wAbilityLV	= static_cast<unsigned char>(lpPktAdmin->m_Position.fPointX) - 1;

				// 이미 배워둔 어빌리티인지 체크
				if ( wAbilityLV <= lpTarget->GetSkillLockCount(wAbilityID) )
				{
					ERRLOG3(g_Log, "CID:0x%08x 어드민 명령 오류 : AbilityID:0x%08x wAbilityLV:%d 어빌리티를 배우는데 실패하였습니다.", 
							dwAdminCID, wAbilityID, wAbilityLV);
					break;
				}

				using namespace Item;

				ItemInfo tempItemInfo(wAbilityID);
				tempItemInfo.m_UseItemInfo.m_usSkill_ID         = wAbilityID;
				tempItemInfo.m_UseItemInfo.m_usSkill_LockCount  = wAbilityLV;
				tempItemInfo.m_DetailData.m_dwFlags |= DetailData::USE_ITEM;

				// ItemSerial 이 0 인 임시 아이템을 생성한다.
				CUseItem* lpUseItem = CUseItem::DowncastToUseItem(
					CItemFactory::GetInstance().CreateTempItem(tempItemInfo));

				if (!lpTarget->AbilityCreate(lpUseItem))
				{
					ERRLOG3(g_Log, "CID:0x%08x 어드민 명령 오류 : AbilityID:0x%08x wAbilityLV:%d 어빌리티를 배우는데 실패하였습니다.", 
							dwAdminCID, wAbilityID, wAbilityLV);
				}

				sprintf(szCommand, "/learnability 0x%08x %d", wAbilityID, wAbilityLV+1);

				DELETE_ITEM(lpUseItem);
			
			break;
		}

		case PktAdmin::ERASE_ABILITY:
		{

				unsigned short	wAbilityID  = lpPktAdmin->m_usProtoTypeID;
				unsigned short	wAbilityLV	= static_cast<unsigned char>(lpPktAdmin->m_Position.fPointX) - 1;

				// 이미 배워둔 어빌리티인지 체크
				if ( wAbilityLV > lpTarget->GetSkillLockCount(wAbilityID) )
				{
					ERRLOG3(g_Log, "CID:0x%08x 어드민 명령 오류 : AbilityID:0x%08x wAbilityLV:%d 어빌리티를 지우는데 실패하였습니다.", 
							dwAdminCID, wAbilityID, wAbilityLV);
					break;
				}

				unsigned char cIndex = static_cast<unsigned char>(lpTarget->GetSkillSlotIndex(wAbilityID));
				int count = lpTarget->GetSkillLockCount(wAbilityID)-wAbilityLV;

				for (int i=0; i<count; ++i)
				{
					Item::ItemPos itemPos(TakeType::TS_ADMIN, 0);

					if (false == lpTarget->AbilityErase(cIndex, itemPos))
					{
						ERRLOG3(g_Log, "CID:0x%08x 어드민 명령 오류 : SkillID:0x%08x LockCount:%d 어빌리티를 지우는데 실패하였습니다.", 
								dwAdminCID, wAbilityID, wAbilityLV);
					}

					--wAbilityLV;
				}

				sprintf(szCommand, "/eraseability 0x%08x %d", wAbilityID, wAbilityLV+1);
			
			break;
		}

		case PktAdmin::LEVEL_DOWN:
		{

				if (dwAmount < 1 || dwAmount > EXP::GetUsingMaxLevel())
				{
					break;
				}

				unsigned char cClassType = static_cast<unsigned char>(lpTarget->GetClass());
				unsigned char cPrevClassType = cClassType;
				if (CClass::GetJobLevel(cClassType) == CClass::JOB_CHANGE_1ST)
				{
					cPrevClassType = CClass::GetPreviousJob(cClassType);
				}
	/*
				// edith 100 렙 이팩트
				// 100레벨 컨텐츠를 사용할경우 만랩 인챈트를 제거한다.
				if (true == CServerSetup::GetInstance().UseContents(GameRYL::LEVEL_LIMIT_100))
				{
					lpTarget->GetSpellMgr().GetAffectedInfo().RemoveChantBySpellType(Skill::SpellID::HundredLevel);
					lpTarget->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::HundredLevel);
				}
	*/			
				// 1 Step
				// 레벨 1의 초기 캐릭터로 만들면서, 모든 스킬을 지운다.
				if (false == lpTarget->InitLevel1Char(cPrevClassType))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : CID:0x%08x 캐릭터를 1레벨로 만드는데 실패하였습니다.",
							dwAdminCID, lpTarget->GetCID());
					break;
				}

				// 2 Step
				// 해당 레벨까지 level up
				while (static_cast<unsigned long>(lpTarget->GetStatus().m_nLevel) < dwAmount)
				{
					if (false == lpTarget->IncrementExp(
						static_cast<unsigned long>(EXP::ExpTable[lpTarget->GetStatus().m_nLevel - 1])))
					{
						break;
					}
				}
	
				// 3 Step
				// 해당 클래스로 전직
				if (lpTarget->GetStatus().m_nLevel >= 10 && cClassType != cPrevClassType)
				{
					if (false == lpTarget->ChangeClass(cClassType))
					{
						ERRLOG4(g_Log, "CID:0x%08x 어드민 명령 오류 : CID:0x%08x 캐릭터 클래스 체인지 실패. "
							"현재 캐릭터의 클래스는 %d, 바꾸고자 하는 클래스는 %d입니다.", 
							dwAdminCID, lpTarget->GetCID(), lpTarget->GetClass(), cClassType);
						break;
					}
				}

				sprintf(szCommand, "/leveldown %s %d", lpTarget->GetCharacterName(), dwAmount);
			
			break;
		}

		// 퀘스트 관련 어드민 명령
		case PktAdmin::GET_QUEST:	// 퀘스트 받기
		{

				// 캐릭터 이름, 퀘스트 ID
				unsigned short wQuestID = lpPktAdmin->m_usProtoTypeID;
				Quest::QuestNode* lpQuest = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
				if (NULL == lpQuest)
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 퀘스트가 존재하지 않습니다.", 
						lpTarget->GetCID(), wQuestID);
					break;
				}

				if (true == lpTarget->HasQuest(wQuestID))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 를 이미 가지고 있습니다.", lpTarget->GetCID(), wQuestID);
					break;
				}

				if (false == lpTarget->GiveQuest(lpQuest))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 퀘스트를 주는데 실패했습니다.", 
						lpTarget->GetCID(), wQuestID);
					break;
				}

				CGameClientDispatch* lpGameClientDispatch = lpTarget->GetDispatcher();
				if (NULL != lpGameClientDispatch)
				{
					GameClientSendPacket::SendCharStartQuest(lpGameClientDispatch->GetSendStream(), lpTarget->GetCID(),
															0, wQuestID, PktBase::NO_SERVER_ERR);
				}

				if (false == lpTarget->StartPhase(wQuestID, 1))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 퀘스트의 첫번째 단계를 실행하는데 실패했습니다.",
							lpTarget->GetCID(), wQuestID);

					// 해당 퀘스트 제거
					lpTarget->CancelQuest(wQuestID);
					GameClientSendPacket::SendCharCancelQuest(lpGameClientDispatch->GetSendStream(),
															  lpTarget->GetCID(), wQuestID, PktAdmin::NO_SERVER_ERR);
					break;
				}

				sprintf(szCommand, "/getquest %s 0x%08x", lpTarget->GetCharacterName(), wQuestID);
			
			break;
		}

		case PktAdmin::ADD_QUEST:	// 완료 퀘스트에 추가
		{

				unsigned short wQuestID = lpPktAdmin->m_usProtoTypeID;
				Quest::QuestNode* lpQuest = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
				if (NULL == lpQuest)
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 퀘스트가 존재하지 않습니다.", 
						lpTarget->GetCID(), wQuestID);
					break;
				}

				CGameClientDispatch* lpGameClientDispatch = lpTarget->GetDispatcher();
				if (NULL == lpGameClientDispatch)
				{
					ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : GameClientDispatch 가 NULL 입니다.", 
						lpTarget->GetCID());
					break;
				}

				// 이미 완료 퀘스트에 존재하는지 체크한다.
				if (true == lpTarget->HasHistoryQuest(wQuestID))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 퀘스트를 이미 완료하였습니다.", 
						lpTarget->GetCID(), wQuestID);
					break;
				}

				// 현재 수행중인 퀘스트에 존재하는지 체크한다.
				if (true == lpTarget->HasExecutingQuest(wQuestID))
				{
					// 수행중이던 퀘스트를 없앤다.
					lpTarget->CancelQuest(wQuestID);

					GameClientSendPacket::SendCharCancelQuest(lpGameClientDispatch->GetSendStream(),
												lpTarget->GetCID(), wQuestID, PktAdmin::NO_SERVER_ERR);
				}

				if (false == lpTarget->InsertHistoryQuest(wQuestID))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 퀘스트를 완료된 퀘스트로 넣는데 실패했습니다.", 
						lpTarget->GetCID(), wQuestID);
					break;
				}

				// 퀘스트 정보를 보낸다.
				GameClientSendPacket::SendCharQuestInfo(lpGameClientDispatch->GetSendStream(), lpTarget);

				sprintf(szCommand, "/addquest %s 0x%08x", lpTarget->GetCharacterName(), wQuestID);
			
			break;
		}

		case PktAdmin::DELETE_QUEST:	// 퀘스트 삭제
		{

				unsigned short wQuestID = lpPktAdmin->m_usProtoTypeID;
				Quest::QuestNode* lpQuest = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
				if (NULL == lpQuest)
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 퀘스트가 존재하지 않습니다.", lpTarget->GetCID(), wQuestID);
					break;
				}

				CGameClientDispatch* lpGameClientDispatch = lpTarget->GetDispatcher();
				if (NULL == lpGameClientDispatch)
				{
					ERRLOG1(g_Log, "CID:0x%08x 어드민 명령 오류 : GameClientDispatch 가 NULL 입니다.", lpTarget->GetCID());
					break;
				}

				if (false == lpTarget->HasQuest(wQuestID))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : QuestID:0x%x 퀘스트를 가지고 있지 않습니다.",	lpTarget->GetCID(), wQuestID);
					break;
				}

				if (true == lpTarget->HasExecutingQuest(wQuestID))
				{
					lpTarget->CancelQuest(wQuestID);
					GameClientSendPacket::SendCharCancelQuest(lpGameClientDispatch->GetSendStream(), 
						lpTarget->GetCID(), wQuestID, PktAdmin::NO_SERVER_ERR);
					break;
				}

				if (true == lpTarget->HasHistoryQuest(wQuestID))
				{
					if (true == lpTarget->DeleteHistoryQuest(wQuestID))
					{
						// 퀘스트에 의해 받는 영향을 계산
						lpTarget->CalculateStatusData(false);
						if (true == lpTarget->CalculateMaxSkillSlot())
						{
							GameClientSendPacket::SendCharStateRedistribution(lpGameClientDispatch->GetSendStream(), 
								lpTarget->GetCID(), lpTarget->GetState(), lpTarget->GetSkill(), PktBase::NO_SERVER_ERR);
						}
					}
				}

				GameClientSendPacket::SendCharQuestInfo(lpGameClientDispatch->GetSendStream(), lpTarget);
			
				sprintf(szCommand, "/deletequest %s 0x%08x", lpTarget->GetCharacterName(), wQuestID);
				
			break;
		}

		// BATTLEGROUND_SERVER 운영자 명령 
		case PktAdmin::FINISH_GAME:		// 해당 방의 경기 종료 (승패 결정)
		{
				unsigned short wMapNumber = lpPktAdmin->m_usProtoTypeID;
				unsigned short wMapIndex = static_cast<unsigned short>(VirtualArea::BGSERVERMAP | wMapNumber);

				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
				if (NULL != lpVirtualArea)
				{
					VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);

					VirtualArea::MapInfo&		mapInfo		= lpBGServerMap->GetMapInfo();
					VirtualArea::ResultInfo&	resultInfo	= lpBGServerMap->GetResultInfo();

					resultInfo.m_cWinRace = lpBGServerMap->RuleCheck(true);
					std::copy(&mapInfo.m_wScore[0], &mapInfo.m_wScore[CClass::MAX_RACE], &resultInfo.m_wScore[0]);

					lpBGServerMap->SendResultInfo();

					sprintf(szCommand, "/finish %d", wMapNumber);
				}
			
			break;
		}

		case PktAdmin::DRAW_GAME:			// 해당 방의 경기 종료 (무승부)
		{

				unsigned short wMapNumber = lpPktAdmin->m_usProtoTypeID;
				unsigned short wMapIndex = static_cast<unsigned short>(VirtualArea::BGSERVERMAP | wMapNumber);

				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
				if (NULL != lpVirtualArea)
				{
					VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);

					VirtualArea::MapInfo&		mapInfo		= lpBGServerMap->GetMapInfo();
					VirtualArea::ResultInfo&	resultInfo	= lpBGServerMap->GetResultInfo();

					resultInfo.m_cWinRace = lpBGServerMap->RuleCheck(true);
					resultInfo.m_cWinRace = CClass::MAX_RACE;
					std::copy(&mapInfo.m_wScore[0], &mapInfo.m_wScore[CClass::MAX_RACE], &resultInfo.m_wScore[0]);

					lpBGServerMap->SendResultInfo();

					sprintf(szCommand, "/draw %d", wMapNumber);
				}
			
			break;
		}

		case PktAdmin::IGNORE_GAME:		// 해당 방의 경기 종료 (승패 무시)
		{
			unsigned short wMapNumber = lpPktAdmin->m_usProtoTypeID;
			unsigned short wMapIndex = static_cast<unsigned short>(VirtualArea::BGSERVERMAP | wMapNumber);

			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
			if (NULL != lpVirtualArea)
			{
				VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);

				lpBGServerMap->RuleCheck(true);

				sprintf(szCommand, "/ignore %d", wMapNumber);
			}

			break;
		}

		case PktAdmin::SET_MAX:			// 해당 방의 최대 인원수 설정
		{

				unsigned short wMapNumber = lpPktAdmin->m_usProtoTypeID;
				unsigned short wMapIndex = static_cast<unsigned short>(VirtualArea::BGSERVERMAP | wMapNumber);
				unsigned char cMaxCharNum = static_cast<unsigned char>(dwAmount);

				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
				if (NULL != lpVirtualArea)
				{
					VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);

					lpBGServerMap->GetMapInfo().m_cMaxCharNumOfNation = cMaxCharNum;
					lpBGServerMap->SendMapInfo();

					sprintf(szCommand, "/max %d %d", wMapNumber, cMaxCharNum);
				}
			
			break;
		}

		case PktAdmin::SET_SCORE:			// 해당 방의 목표 점수 설정
		{

				unsigned short wMapNumber = lpPktAdmin->m_usProtoTypeID;
				unsigned short wMapIndex = static_cast<unsigned short>(VirtualArea::BGSERVERMAP | wMapNumber);
				unsigned short wTargetScore = static_cast<unsigned short>(dwAmount);

				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
				if (NULL != lpVirtualArea)
				{
					VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);

					lpBGServerMap->GetMapInfo().m_wTargetScore = wTargetScore;
					lpBGServerMap->SendMapInfo();

					sprintf(szCommand, "/score %d %d", wMapNumber, wTargetScore);
				}
			
			break;
		}

		case PktAdmin::SET_LIMIT_TIME:		// 해당 방의 시간 제한 설정
		{

				unsigned short wMapNumber = lpPktAdmin->m_usProtoTypeID;
				unsigned short wMapIndex = static_cast<unsigned short>(VirtualArea::BGSERVERMAP | wMapNumber);
				unsigned char cLimitMin = static_cast<unsigned char>(dwAmount);

				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
				if (NULL != lpVirtualArea)
				{
					VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);

					lpBGServerMap->GetMapInfo().m_cLimitMin = cLimitMin;
					lpBGServerMap->GetMapInfo().m_cRemainPlayMin = cLimitMin;
					lpBGServerMap->ResetEnteringMin(cLimitMin);	// 들어올 시점의 남은 시간을 cLimit 로 전부 리셋
					if (lpBGServerMap->IsPlaying())
					{
						lpBGServerMap->SetRemainTime( timeGetTime() + cLimitMin * VirtualArea::MILLISEC_PER_MINUTE );
					}
					lpBGServerMap->SendMapInfo();

					sprintf(szCommand, "/time %d %d", wMapNumber, cLimitMin);
				}
			
			break;
		}

		case PktAdmin::SET_REST_TIME:		// 해당 방의 쉬는 시간 설정
		{

				unsigned short wMapNumber = lpPktAdmin->m_usProtoTypeID;
				unsigned short wMapIndex = static_cast<unsigned short>(VirtualArea::BGSERVERMAP | wMapNumber);
				unsigned char cRestMin = static_cast<unsigned char>(dwAmount);

				VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
				if (NULL != lpVirtualArea)
				{
					VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);

					lpBGServerMap->GetMapInfo().m_cRestMin = cRestMin;
					lpBGServerMap->GetMapInfo().m_cRemainRestMin = cRestMin;
					if (lpBGServerMap->IsResting())
					{
						lpBGServerMap->SetRemainTime( timeGetTime() + cRestMin * VirtualArea::MILLISEC_PER_MINUTE );
					}
					lpBGServerMap->SendMapInfo();

					sprintf(szCommand, "/rest %d %d", wMapNumber, cRestMin);
				}
			
			break;
		}

		case PktAdmin::SET_ITEM_TEMP_INVEN:
		{

				Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(lpPktAdmin->m_usProtoTypeID);
				if (NULL != lpItem)
				{
					if (lpItem->IsSet(Item::DetailData::STACKABLE))
					{
						lpItem->SetNumOrDurability(static_cast<unsigned char>(dwAmount));
					}

					if (false == lpTarget->SetItem(Item::ItemPos(TakeType::TS_TEMPINVEN, 0), lpItem))
					{
						ERRLOG2(g_Log, "CID:0x%08x 어드민 명령 오류 : %s의 임시 인벤토리에 아이템을 넣어주는데 실패하였습니다.", 
							dwAdminCID, lpPktAdmin->m_stName);

						DELETE_ITEM(lpItem);
						return true;
					}

					sprintf(szCommand, "/addtempinven %s %d %d", lpTarget->GetCharacterName(), lpPktAdmin->m_usProtoTypeID, dwAmount);
				
					DETLOG3(g_Log, "CID:0x%08x 어드민이 아이템을 생성하여 임시 인벤토리에 넣어주었습니다. 아이템(ID:%d, 갯수:%d)", 
						lpTarget->GetCID(), lpPktAdmin->m_usProtoTypeID, dwAmount);
				}
			
			break;
		}

		// 스펠 관련 운영자 명렁
		case PktAdmin::ADD_SPELL:
		{

				unsigned short wSpellID		= static_cast<unsigned short>(min(static_cast<long>(lpPktAdmin->m_dwAmount), USHRT_MAX));
				unsigned short wSpellLevel	= static_cast<unsigned short>(min(static_cast<long>(lpPktAdmin->m_dwAmount2), USHRT_MAX));
				unsigned long dwDurationSec	= lpPktAdmin->m_dwAmount3;

				dwDurationSec = dwDurationSec == 0 ? CSpell::INFINITE_DURATION : dwDurationSec;

				if (false == AddSpell(lpAdmin, lpTarget, wSpellID, wSpellLevel, dwDurationSec))
				{
					ERRLOG2(g_Log, "CID:0x%08x 어드민 명령으로 스펠 부여 실패 (SpellID : %d)", lpTarget->GetCID(), wSpellID);
					return true;
				}

				sprintf(szCommand, "/addspell %s 0x%08x %d %d", lpTarget->GetCharacterName(), wSpellID, wSpellLevel, dwDurationSec);
			
			break;
		}

		case PktAdmin::DELETE_SPELL:
		{

				unsigned char cSpellType = static_cast<unsigned char>(lpPktAdmin->m_usProtoTypeID);

				lpTarget->GetSpellMgr().GetAffectedInfo().RemoveChantBySpellType(cSpellType);
				lpTarget->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(cSpellType);

				sprintf(szCommand, "/deletespell %s %d", lpTarget->GetCharacterName(), cSpellType);
			
			break;
		}

		// 캐릭터 묶어두기 관련 운영자 명령
		case PktAdmin::STOP:
		{
	
				if (false == AddSpell(lpAdmin, lpTarget, Skill::SpellID::Hold, 1, Skill::Disenchant::INFINITE_NUM))
				{
					ERRLOG1(g_Log, "CID:0x%08x 어드민 명령으로 캐릭터 묶어두기 실패", lpTarget->GetCID());
					return true;
				}

				sprintf(szCommand, "/stop %s", lpTarget->GetCharacterName());
			
			break;
		}

		case PktAdmin::STOP_OFF:
		{

				lpTarget->GetSpellMgr().GetAffectedInfo().RemoveChantBySpellType(Skill::SpellID::Hold);
				lpTarget->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Hold);

				sprintf(szCommand, "/stopoff %s", lpTarget->GetCharacterName());
			
			break;
		}

		case PktAdmin::CHAT_BAN:
		{

				GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, CChatDispatch::GetDispatchTable());
				if (NULL != lpChatDispatch)
				{
					GameClientSendPacket::SendCharChatBan(lpChatDispatch->GetSendStream(), dwAdminCID, lpTarget->GetCID(), dwAmount);
				}
			
			break;
		}

		case PktAdmin::CHAT_OK:
		{

				GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, CChatDispatch::GetDispatchTable());
				if (NULL != lpChatDispatch)
				{
					GameClientSendPacket::SendCharChatBan(lpChatDispatch->GetSendStream(), dwAdminCID, lpTarget->GetCID(), 0);
				}
			
			break;
		}

		// 공헌훈장 포인트.
		case PktAdmin::REALM_POINT:
		{

				if(dwAmount>=0 && dwAmount<=5)
				{
					// DB Agent 보낸다.
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					if(lpDBAgentDispatch)
					{					
						CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();
						char* lpBuffer = SendStream.GetBuffer(sizeof(PktRealmPoint));

						if (lpBuffer)
						{
							PktRealmPoint* lpPktRealmPoint = reinterpret_cast<PktRealmPoint*>(lpBuffer);

							lpPktRealmPoint->m_dwCID		= lpTarget->GetCID();
							lpPktRealmPoint->m_cRealmPoint	= static_cast<unsigned char>(dwAmount);
							lpPktRealmPoint->m_cType		= PktRealmPoint::REALM_JOIN;

							return SendStream.WrapCrypt(sizeof(PktRealmPoint), CmdRealmPoint, 0, 0);
						}								
					}

					sprintf(szCommand, "/realmpoint %s %d", lpTarget->GetCharacterName(), dwAmount);
				
			}
			break;
		}


		default:			
			break;
	}

	// Admin 명령어 로그 패킷 //
	if(lpAdmin && lpTarget)
	{
		GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

		if(lpDBAgentDispatch)
		{
			char* lpBuffer = lpDBAgentDispatch->GetSendStream().GetBuffer(sizeof(PktAdminCommandLog));

			if(lpBuffer)
			{
				PktAdminCommandLog* lpPktAdminCommandLog = reinterpret_cast<PktAdminCommandLog*>(lpBuffer);

				lpPktAdminCommandLog->m_dwAdminCID	= lpAdmin->GetCID();		
				lpPktAdminCommandLog->m_dwCID		= lpTarget->GetCID();

				memcpy(lpPktAdminCommandLog->m_szCommand, szCommand, PktAdminCommandLog::MAX_COMMAND_SIZE);

				lpDBAgentDispatch->GetSendStream().WrapHeader(sizeof(PktAdminCommandLog), CmdAdminCommandLog, 0, PktBase::NO_SERVER_ERR);
			}
		}		
	}	

	return true;
}

bool GameClientParsePacket::AddSpell(CCharacter* lpCaster, CCharacter* lpVictim, 
									 unsigned short wSpellID, unsigned short wSpellLevel, unsigned long dwDurationSec)
{
	if (!lpCaster || !lpVictim)
	{
		return false;
	}

	CSpell* pSpell = NULL ;
	unsigned short wResult = CSpell::NO_ENCHANT_ERROR ;

	switch (wSpellID)
	{
		// Chant
		case Skill::SpellID::BattleSong :
		{
			pSpell = new CBattleSongSpell(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec), 0);
		}
		break;

		case Skill::SpellID::StealHand :
		{
			pSpell = new CStealHandSpell(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec), 0);
		}
		break;

		case Skill::SpellID::MaintenanceChant :
		{
			pSpell = new CMaintenanceChantSpell(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec), 0);
		}
		break;

		case Skill::SpellID::AccelerationChant :
		{
			pSpell = new CAccelerationChantSpell(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec), 0);
		}
		break;

		case Skill::SpellID::LifeAura :
		{
			pSpell = new CLifeAuraSpell(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec), 0);
		}
		break;

		case Skill::SpellID::GuardAura :
		{
			pSpell = new CGuardAuraSpell(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec), 0);
		}
		break;

		case Skill::SpellID::SpeedBuff :
		{
			pSpell = new CSpeedBuffChantSpell(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec), 0);
		}
		break;


		// Enchant
		case Skill::SpellID::ExpOrb :
		{
			//CExpOrbSpell
			wResult = Skill::CAddSpell<CExpOrbSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::PAYBUFF_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::LuckyOrb :
		{
			//CLuckyOrbSpell
			wResult = Skill::CAddSpell<CLuckyOrbSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster, 
				Skill::SpellType::PAYBUFF_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::PowerStatue :
		{
			wResult = Skill::CAddSpell<CPowerStatueSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::STATUE_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);

			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;
		case Skill::SpellID::IntStatue :
		{
			wResult = Skill::CAddSpell<CIntStatueSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::STATUE_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);

			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::ExpStatue :
		{
			wResult = Skill::CAddSpell<CExpStatueSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::STATUE_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);

			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;
		case Skill::SpellID::WealthStatue :
		{
			wResult = Skill::CAddSpell<CWealthStatueSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::STATUE_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);

			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::CD_8D12:
		case Skill::SpellID::CD_8D14:
		case Skill::SpellID::CD_8D16:
		case Skill::SpellID::CD_99A1:
		case Skill::SpellID::CD_99A2:
		case Skill::SpellID::CD_99A3:

		case Skill::SpellID::DemagePotion:
		case Skill::SpellID::ArmorPotion:
		case Skill::SpellID::HitRatePotion:
		case Skill::SpellID::EvadePotion:
		case Skill::SpellID::MaxHPPotion:
		case Skill::SpellID::MaxMPPotion:
		case Skill::SpellID::MPRegenPotion:
		case Skill::SpellID::HPRegenPotion:
		case Skill::SpellID::CriticalPotion:
		case Skill::SpellID::BlockPotion:
		case Skill::SpellID::SpeedPotion:
		case Skill::SpellID::DeCoolDownPotion:
		case Skill::SpellID::MagicPowerPotion:
		case Skill::SpellID::MagicResistPotion:
		case Skill::SpellID::GreenPotion:   ////////////////
		{
			wResult = Skill::CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::BUFF_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Regeneration :
		{
			wResult = Skill::CAddSpell<CRegenerationSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Slow :
		{
			wResult = Skill::CAddSpell<CSlowSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::ArmorBroken :
		{
			wResult = Skill::CAddSpell<CArmorBrokenSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Blaze :
		{
			wResult = Skill::CAddSpell<CBlazeSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Charging :
		{
			wResult = Skill::CAddSpell<CChargingSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Stealth :
		{
			wResult = Skill::CAddSpell<CStealthSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::ManaShell :
		{
			wResult = Skill::CAddSpell<CManaShellSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Encourage :
		{
			wResult = Skill::CAddSpell<CEncourageSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::EnchantWeapon :
		{
			wResult = Skill::CAddSpell<CEnchantWeaponSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::DeEnchant :
		{
			wResult = Skill::CAddSpell<CDeEnchantSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::KillStreakBuff ://hz added for killstreak buff.
		{
			wResult = Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::KillStreakBuff1 ://hz added for killstreak buff.
		{
			wResult = Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::KillStreakBuff2 ://hz added for killstreak buff.
		{
			wResult = Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::KillStreakBuff3 ://hz added for killstreak buff.
		{
			wResult = Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::KillStreakBuff4 ://hz added for killstreak buff.
		{
			wResult = Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::KillStreakBuff5 ://hz added for killstreak buff.
		{
			wResult = Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::KillStreakBuff6 ://hz added for killstreak buff.
		{
			wResult = Skill::CAddSpell<CKillStreakBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::BrightArmor :
		{
			wResult = Skill::CAddSpell<CBrightArmorSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::HardenSkin :
		{
			wResult = Skill::CAddSpell<CHardenSkinSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Flexibility :
		{
			wResult = Skill::CAddSpell<CFlexibilitySpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Guard :
		{
			wResult = Skill::CAddSpell<CGuardSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Hold :
		{
			wResult = Skill::CAddSpell<CHoldSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::PHYSICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Stun :
		{
			wResult = Skill::CAddSpell<CStunSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Frozen :
		{
			wResult = Skill::CAddSpell<CFrozenSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Poisoned :
		{
			wResult = Skill::CAddSpell<CPoisonedSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::LowerStrength :
		{
			wResult = Skill::CAddSpell<CLowerStrengthSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType,	lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Invincible :
		{
			wResult = Skill::CAddSpell<CInvincibleSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster, 
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Hide :
		{
			return true;
		}
		break;

		case Skill::SpellID::StoneForm :
		{
			wResult = Skill::CAddSpell<CStoneFormSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster, 
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::CounterAttack :
		{
			return true;
		}
		break;

		case Skill::SpellID::Envenom :
		{
			wResult = Skill::CAddSpell<CEnvenomSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster, 
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::BombSet :
		{
			wResult = Skill::CAddSpell<CBombSetSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster, 
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
			return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
		}
		break;

		case Skill::SpellID::Fired :
			{
				wResult = Skill::CAddSpell<CFiredSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
					Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
				return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
			}
			break;

		case Skill::SpellID::CurseOfBlind :
			{
				wResult = Skill::CAddSpell<CCurseOfBlindSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType,	lpCaster,
					Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
				return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
			}
			break;

		case Skill::SpellID::HundredLevel :
			return true;
/*
		// edith 100랩달성 이팩트
		case Skill::SpellID::HundredLevel :
			{
				wResult = Skill::CAddSpell<CHundredLevelSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType,	lpCaster,
					Skill::SpellType::ETERNAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
				return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
			}
			break;
*/
		case Skill::SpellID::DeCoolDown :
			{
				wResult = Skill::CAddSpell<CDeCoolDownSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType,	lpCaster,
					Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
				return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
			}
			break;

		case Skill::SpellID::MichaelBless :
			{
				wResult = Skill::CAddSpell<CMichaelBlessSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType,	lpCaster,
					Skill::SpellType::PAYBUFF_SPELL, wSpellID, wSpellLevel, dwDurationSec))(lpVictim);
				return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
			}
			break;

		case Skill::SpellID::Clairvoyance :
		{
			pSpell = new CClairvoyanceChantSpell(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCaster,
				Skill::SpellType::MAGICAL_SPELL, wSpellID, wSpellLevel, dwDurationSec), 0);

		}
		break;
	}

	// Chant Spell
	if (pSpell)
	{
		CParty* pParty = lpVictim->GetParty();
		if (pParty)
		{
			pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpCaster->GetMapIndex());
		}
		else
		{
			unsigned short wError = CSpell::NO_ENCHANT_ERROR;
			pSpell->AddAffected(lpVictim, wError);
		}

		CGlobalSpellMgr::GetInstance().Add(pSpell);

		return true;
	}

	return false;
}


bool GameClientParsePacket::ParseCharNameChange(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCharNameChange), GameClientDispatch);

    PktCharNameChange* lpPktCharNameChange = 
        reinterpret_cast<PktCharNameChange*>(lpPktBase);    
/*
// 패킷 진행순서
-Game
bool GameClientParsePacket::ParseCharNameChange(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)

-DBAgent
bool CGameDispatch::ChangeName(PktBase* lpPktBase)

-Game
bool DBAgentPacketParse::ParseCharNameChangeAck(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)

*/

    const char*     szChangeName    = lpPktCharNameChange->m_szCharName;
    unsigned long   dwUID           = lpPktCharNameChange->m_dwUID;
    unsigned long   dwCID           = lpPktCharNameChange->m_dwCID;
	Item::ItemPos   ItemPos			= lpPktCharNameChange->m_ItemPos;         // 허가서 위치
    
    unsigned short  usError         = 0;

    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

    if (0 == lpCharacter || lpCharacter->GetUID() != dwUID || lpCharacter->GetCID() != dwCID)
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / CharacterPointer:0x%08X / 캐릭터 이름 변경 실패 : 일치하는 캐릭터 없음",
            dwUID, dwCID, lpCharacter);

        usError = PktCharNameChange::ERR_CANNOT_FIND_CHARACTER;
    }   
/*
	// 삭제 아이템 기능이 들어감으로서 해당 로직은 제외
    else if (0 == lpCharacter->GetNameChangeCount())
    {
        ERRLOG2(g_Log, "UID:%10u / CID:%10u / 이름 변경 카운트가 0입니다", dwUID, dwCID);

        usError = PktCharNameChange::ERR_NAMECHANGE_COUNT;
    }
*/
    else if (!CharCreate::CheckCharCreateName(szChangeName, CServerSetup::GetInstance().GetHanCheck()))
    {
        // 생성 캐릭터 이름 검사        
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / 잘못된 캐릭터 이름 %s", dwUID, dwCID, szChangeName);

        usError = PktCharNameChange::ERR_INVALID_NAME;
    }
    else if (!Filter::NameCheck(szChangeName))
    {
        // 생성 캐릭터 이름 검사        
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / 사용할 수 없는 캐릭터 이름 %s", dwUID, dwCID, szChangeName);

        usError = PktCharNameChange::ERR_INVALID_NAME;
    }
    else if(!lpCharacter->CheckRenameWarrant(ItemPos, false))
	{
		// 생성 캐릭터 이름 검사        
		ERRLOG2(g_Log, "UID:%10u / CID:%10u / 계명허가서 아이템에 문제가있다.", dwUID, dwCID);
		usError = PktCharNameChange::ERR_NAMECHANGE_ITEM;
	}
	else
	{
        GET_SINGLE_DISPATCH(lpAgentDispatch, CDBAgentDispatch, 
            CDBAgentDispatch::GetDispatchTable());

        if(0 == lpAgentDispatch || !GameClientSendPacket::SendCharNameChange(
            lpAgentDispatch->GetSendStream(), dwUID, dwCID, szChangeName, 0, &ItemPos, 0))
        {
            usError = PktCharNameChange::ERR_SERVER;
        }
    }

    if(0 != usError)
    {
        // 클라이언트로 에러코드를 보낸다.
        GameClientSendPacket::SendCharNameChange(GameClientDispatch.GetSendStream(),
            dwUID, dwCID, szChangeName, 0, &ItemPos, usError);
    }

    return true;
}
