#include "stdafx.h"
#include "ParseCastle.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Castle/CastleDB.h>
#include <Castle/CastleDBMgr.h>
#include <Creature/CreatureStructure.h>
#include <Creature/Siege/SiegeObjectDB.h>
#include <Creature/Siege/CastleArmsDB.h>
#include <Creature/Siege/CampDB.h>
#include <Creature/Siege/MiningCampDB.h>
#include <Creature/Siege/CampShopDB.h>
#include <Creature/Siege/WorldWeaponDB.h>
#include <Creature/Siege/SiegeObjectDBMgr.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <DataStorage/CharacterData.h>
#include <DataStorage/CharacterDataMgr.h>

#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>

#include <Map/FieldMap/MineralVeinDBMgr.h>

#include <GameTime/GameTimeConstants.h>

#include <Creature/StatueDBMgr.h>

#include <Log/ServerLog.h>

// 길드 요새 메세지 처리시 필요한 캐릭명을 찾는 함수
const char*		GetCharacterName(unsigned long dwCID);
unsigned char	GetCharacterNation(unsigned long dwCID);

namespace DBAgent
{
namespace ParseCastle
{

bool CastleCmd(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktCastleCmd* lpPktCastleCmd = static_cast<PktCastleCmd*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned long dwCID				= lpPktCastleCmd->m_dwCID;
    unsigned long dwCastleID		= lpPktCastleCmd->m_dwCastleID;
    unsigned long dwCastleObjectID	= lpPktCastleCmd->m_dwCastleObjectID;
    unsigned long dwValue1			= lpPktCastleCmd->m_dwValue1;
    unsigned long dwValue2			= lpPktCastleCmd->m_dwValue2;
    unsigned char cSubCmd			= lpPktCastleCmd->m_cSubCmd;

    CSiegeObjectDB* lpCastleObject = CSiegeObjectDBMgr::GetInstance().GetSiegeObject(dwCastleObjectID);
    CCastleDB* lpCastle = CCastleDBMgr::GetInstance().GetCastle(dwCastleID);

    if (NULL == lpCastleObject || NULL == lpCastle)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
		// 수성 병기의 마지막 사용 시간을 업데이트
		if (cSubCmd != PktCastleCmd::CASTLE_UPDATE_HP && lpCastleObject->IsCastleArms())
		{
			static_cast<CCastleArmsDB*>(lpCastleObject)->UpdateLastUseTime();
		}

        switch (cSubCmd)
        {
            // 세율 변경
			// [In/Out] dwValue1 : 세율 타입
			// [In/Out] dwValue2 : 세율
			case PktCastleCmd::CASTLE_SET_TAX:
			{
				lpCastle->SetTax(static_cast<unsigned char>(dwValue1), static_cast<unsigned char>(dwValue2));
			}
            break;

            // 성 상징물 업그레이드
			// [In/Out] dwValue1 : 업그레이드 타입
			// [In/Out] dwValue2 : 업그레이드 시간 (분)
			case PktCastleCmd::CASTLE_UPGRADE_EMBLEM:
            {
                CSiegeObjectDB* lpEmblem = lpCastle->GetCastleEmblem();
                if (lpEmblem)
                {
                    lpEmblem->Upgrade(static_cast<unsigned char>(dwValue2), 0, static_cast<unsigned char>(dwValue1));
					lpCastle->SetUpgradeItemInfo( 0, 0 );
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

			// 성 상징물의 보석 정보 업데이트
			// [In] dwValue1 : 보석 아이템 ID
			// [In] dwValue2 : 총 갯수
			case PktCastleCmd::CASTLE_UPGRADE_ITEM_INFO:
			{
				lpCastle->SetUpgradeItemInfo( static_cast<unsigned short>(dwValue1), static_cast<unsigned char>(dwValue2) );
				return true;
			}
			break;

            // 성 상징물이 부셔질때마다
			// [In] dwValue1 : 상징물을 부순 적의 국적
			// [Out] dwValue1 : 소환중인 상징물이 아군것인지 적군것인지 정보
			case PktCastleCmd::CASTLE_UPDATE_EMBLEM:
            {
                CSiegeObjectDB* lpEmblem = lpCastle->GetCastleEmblem();
                if (lpEmblem)
                {
                    lpEmblem->Destroy(static_cast<unsigned char>(dwValue1));
                    dwValue1 = lpEmblem->GetSubState();
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

			// 세금 회수
			// [In/Out] dwValue1 : 세금 타입
			// [In/Out] dwValue2 : 회수할 세금
			case PktCastleCmd::CASTLE_TAKE_TAXMONEY:
            {
				lpCastle->TakeTaxMoney(static_cast<unsigned char>(dwValue1), dwValue2);
            }
            break;

			// 광물 획득
			// [In/Out] dwValue1 : 광물ID
			// [In/Out] dwValue2 : 획득량
			case PktCastleCmd::CASTLE_GAIN_MINERAL:
			{
				unsigned short wMineralID = static_cast<unsigned short>( dwValue1 );
				unsigned short wAmount = static_cast<unsigned short>( dwValue2 );
				
				if (!lpCastle->GainMineral(wMineralID, wAmount))
				{
					ERRLOG3(g_Log, "성 누적 광물 세금 회수 실패, CastleID : 0x%10u, MineralID : %d, Amount : %d",
						lpCastle->GetCastleID(), wMineralID, wAmount);
					return true;
				}
			}
			break;

            // 성문 열기 / 닫기
			case PktCastleCmd::CASTLE_GATE_OPEN:
			case PktCastleCmd::CASTLE_GATE_CLOSE:
            break;

            // 성문 업그레이드
			// [In/Out] dwValue1 : 업그레이드 비용
			// [In/Out] dwValue2 : 업그레이드 시간 (분)
			case PktCastleCmd::CASTLE_UPGRADE_GATE:
            {
                CSiegeObjectDB* lpGate = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpGate)
                {
                    lpGate->Upgrade(static_cast<unsigned char>(dwValue2), dwValue1);
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // 성문 수리
			// [In/Out] dwValue1 : 수리 비용
			// [In/Out] dwValue2 : 수리할 HP 양
			case PktCastleCmd::CASTLE_REPAIR_GATE:
            {
                CSiegeObjectDB* lpGate = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpGate)
                {
                    lpGate->Repair(dwValue2, dwValue1);
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // 성문 복구
			// [In/Out] dwValue1 : 성문 복구 비용
			// [In/Out] dwValue2 : 성문 복구 시간 (분)
			case PktCastleCmd::CASTLE_RESTORE_GATE:
            {
                CSiegeObjectDB* lpGate = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpGate)
                {
                    lpGate->Restore(static_cast<unsigned char>(dwValue2), dwValue1);
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // 성문 파괴
			case PktCastleCmd::CASTLE_DESTROY_GATE:
            {
                CSiegeObjectDB* lpGate = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpGate)
                {
                    lpGate->Destroy();
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // 수성 병기 개발
			// [In/Out] dwValue1 : 개발 비용
			// [In/Out] dwValue2 : 상위 16비트 : 생산시간 / 하위 16비트 : 병기 타입
			case PktCastleCmd::CASTLE_CREATE_ARMS:
            {
                CSiegeObjectDB* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpCastleArms)
                {
                    unsigned short wArmsType = static_cast<unsigned short>(dwValue2 & 0x0000FFFF);
                    unsigned char cDevelopTime = static_cast<unsigned char>( (dwValue2 >> 16) );

                    lpCastleArms->Build(wArmsType, cDevelopTime, dwValue1, dwCID);
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

			// 수성 병기 탑승
			case PktCastleCmd::CASTLE_RIDE_ARMS:
			{
				CSiegeObjectDB* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
				if (lpCastleArms)
				{
					// 수성 병기가 자동 파괴되지 않도록 마지막 사용 시간을 초기화 시킨다.
					lpCastleArms->UpdateLastUseTime(true);
				}

				return true;
			}
			break;

            // 수성 병기 내림
			case PktCastleCmd::CASTLE_GETOFF_ARMS:
			{
				return true;
			}
			break;

            // 수성 병기 업그레이드
			// [In/Out] dwValue1 : 업그레이드 비용
			// [In/Out] dwValue2 : 업그레이드 시간 (분)
			case PktCastleCmd::CASTLE_UPGRADE_ARMS:
            {
                CSiegeObjectDB* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpCastleArms)
                {
                    lpCastleArms->Upgrade(static_cast<unsigned char>(dwValue2), dwValue1);
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // 수성 병기 수리
			// [In/Out] dwValue1 : 수리 비용
			// [In/Out] dwValue2 : 수리할 HP 양
			case PktCastleCmd::CASTLE_REPAIR_ARMS:
            {
                CSiegeObjectDB* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpCastleArms)
                {
                    lpCastleArms->Repair(dwValue2, dwValue1);
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // 수성 병기 파괴
			// [In/Out] dwValue1 : 0 : 파괴, 1 : 병기관리 NPC로 변형( 생산 비용 절반을 돌려줌 )
			case PktCastleCmd::CASTLE_DESTROY_ARMS:
            {
                CSiegeObjectDB* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpCastleArms)
                {
                    lpCastleArms->Destroy(0, static_cast<bool>(dwValue1));
                }
                else
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // HP 업데이트
			case PktCastleCmd::CASTLE_UPDATE_HP:
            {
                CSiegeObjectDB* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
                if (lpCastleArms)
                {
                    lpCastleArms->UpdateHP(dwValue1);
                }
                return true;
            }
            break;

			default:
				ERRLOG1(g_Log, "Cmd:0x%02X / 존재 하지 않는 성 관련 명령을 요청하였습니다.", cSubCmd);
				return true;
        }
    }

    PktCastleCmd pktCastleCmd;
    memset(&pktCastleCmd, 0, sizeof(PktCastleCmd));

    if (0 == wError)
    {
		pktCastleCmd.m_dwCID			= dwCID;
        pktCastleCmd.m_dwCastleID		= lpCastle->GetCastleID();
        pktCastleCmd.m_dwCastleObjectID	= lpCastleObject->GetCID();
        pktCastleCmd.m_cState			= lpCastleObject->GetState();
        pktCastleCmd.m_dwValue1			= dwValue1;
        pktCastleCmd.m_dwValue2			= dwValue2;
        pktCastleCmd.m_cSubCmd			= cSubCmd;

        if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCastleCmd),
            sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktCastleCmd), sizeof(PktCastleCmd), CmdCastleCmd));
        }
    }
    else
    {
        if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCastleCmd),
            sizeof(PktCastleCmd), CmdCastleCmd, 0, wError))
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktCastleCmd), sizeof(PktCastleCmd), CmdCastleCmd));
        }

        pktCastleCmd = *lpPktCastleCmd;
        SendStream.WrapCompress(reinterpret_cast<char*>(&pktCastleCmd), 
            sizeof(PktCastleCmd), CmdCastleCmd, 0, wError);        
    }

    return true;
}

bool SiegeArmsCmd(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktSiegeArmsCmd* lpPktSiegeArmsCmd = static_cast<PktSiegeArmsCmd*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    unsigned long dwCID				= lpPktSiegeArmsCmd->m_dwCID;
    unsigned long dwArmsID			= lpPktSiegeArmsCmd->m_dwArmsID;
    unsigned long dwValue			= lpPktSiegeArmsCmd->m_dwValue;
    unsigned char cSubCmd			= lpPktSiegeArmsCmd->m_cSubCmd;

	CSiegeObjectDB* lpSiegeArms = CSiegeObjectDBMgr::GetInstance().GetSiegeObject(dwArmsID);
    if (NULL == lpSiegeArms)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
		// 공성 병기를 마지막으로 사용한 시간을 저장
		if (cSubCmd != PktSiegeArmsCmd::SIEGE_UPDATE_HP)
		{
			lpSiegeArms->UpdateLastUseTime();
		}

        switch (cSubCmd)
        {
            // 공성 병기 생성 취소
        case PktSiegeArmsCmd::SIEGE_CANCEL:
            {
                lpSiegeArms->SendSiegeArmsCmd(0, PktSiegeArmsCmd::SIEGE_CANCEL);

                lpSiegeArms->Destroy();
                CSiegeObjectDBMgr::GetInstance().DeleteSiegeObject(lpSiegeArms->GetCID());
                return true;
            }
            break;

            // 공성 병기 탑승
        case PktSiegeArmsCmd::SIEGE_RIDE_ARMS:
            {
                // 공성 병기가 파괴되지 않게 하기 위해
                lpSiegeArms->UpdateLastUseTime(true);
            }
            return true;

            // 공성 병기 내림
        case PktSiegeArmsCmd::SIEGE_GETOFF_ARMS:
            return true;

            // 공성 병기 수리
        case PktSiegeArmsCmd::SIEGE_REPAIR_ARMS:
            {
                if (false == lpSiegeArms->Repair(dwValue))
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // 공성 병기를 스타트킷으로
        case PktSiegeArmsCmd::SIEGE_TO_STARTKIT:
            {
                if (false == lpSiegeArms->ToStartKit())
                {
                    wError = PktBase::SERVER_ERROR;
                }
            }
            break;

            // 공성 병기 파괴
        case PktSiegeArmsCmd::SIEGE_DESTROY_ARMS:
            {
                lpSiegeArms->SendSiegeArmsCmd(0, PktSiegeArmsCmd::SIEGE_DESTROY_ARMS);

                lpSiegeArms->Destroy();
                CSiegeObjectDBMgr::GetInstance().DeleteSiegeObject(lpSiegeArms->GetCID());
                return true;
            }
            break;

            // HP 업데이트
        case PktSiegeArmsCmd::SIEGE_UPDATE_HP:
            {
                lpSiegeArms->UpdateHP(dwValue);
                return true;
            }
            break;

        default:
            ERRLOG1(g_Log, "Cmd:0x%02X / 존재 하지 않는 공성 병기 명령을 요청하였습니다.", cSubCmd);
            return true;
        }
    }

    PktSiegeArmsCmd* lpPktSiegeArmsCmdAck = 
        reinterpret_cast<PktSiegeArmsCmd*>(SendStream.GetBuffer(sizeof(PktSiegeArmsCmd)));

    if (NULL != lpPktSiegeArmsCmdAck)
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            lpPktSiegeArmsCmdAck->m_dwCID		= lpSiegeArms->GetOwnerID();
            lpPktSiegeArmsCmdAck->m_dwArmsID	= lpSiegeArms->GetCID();
            lpPktSiegeArmsCmdAck->m_cState		= lpSiegeArms->GetState();
            lpPktSiegeArmsCmdAck->m_dwValue	    = dwValue;
            lpPktSiegeArmsCmdAck->m_cSubCmd	    = cSubCmd;
        }
        else
        {
            *lpPktSiegeArmsCmd = *lpPktSiegeArmsCmd;
        }

        SendStream.WrapHeader(sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, wError);
    }

    return true;
}

bool SetCastleRight(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktCastleRight* lpPktCastleRight = static_cast<PktCastleRight*>(lpPktBase);

    unsigned short  wError      = PktBase::NO_SERVER_ERR;
    unsigned long	dwCastleID	= lpPktCastleRight->m_dwCastleID;
    CastleRight		castleRight	= lpPktCastleRight->m_CastleRight;

    CCastleDB* lpCastle = CCastleDBMgr::GetInstance().GetCastle(dwCastleID);

    if (NULL == lpCastle)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        lpCastle->SetRight(castleRight);
    }

    PktCastleRight pktCastleRight;
    memset(&pktCastleRight, 0, sizeof(PktCastleRight));

    pktCastleRight.m_dwCastleID     = dwCastleID;
    pktCastleRight.m_CastleRight    = castleRight;

    if (PktBase::NO_SERVER_ERR == wError)
    {
		if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCastleRight), 
            sizeof(PktCastleRight), CmdCastleRight, 0, 0))
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktCastleRight), sizeof(PktCastleRight), CmdCastleRight));
        }
    }
    else
    {
        SendStream.WrapCompress(reinterpret_cast<char*>(&pktCastleRight), 
            sizeof(PktCastleRight), CmdCastleRight, 0, wError);
    }

    return true;
}

bool SetCampRight(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktCampRight* lpPktCampRight = static_cast<PktCampRight*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    unsigned long	dwCampID		= lpPktCampRight->m_dwCampID;
    CampRight		campRight		= lpPktCampRight->m_CampRight;
	CampRight		diffCampRight;
	std::fill_n(&diffCampRight.m_aryCampRight[0], int(CampRight::MAX_RIGHT_NUM), 0);

    CCampDB* lpCamp = CSiegeObjectDBMgr::GetInstance().GetCamp(dwCampID);

    if (NULL == lpCamp)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
		const CampRight& oldCampRight = lpCamp->GetRight();
		for (int i=0; i<CampRight::MAX_RIGHT_NUM; ++i)
		{
			if (oldCampRight.m_aryCampRight[i] != campRight.m_aryCampRight[i])
			{
				diffCampRight.m_aryCampRight[i] = campRight.m_aryCampRight[i];
			}
		}

        lpCamp->SetRight(campRight);
    }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 메세지 보내기
	if (PktBase::NO_SERVER_ERR == wError)
	{
		PktCampMessage pktCampMessage;
		memset(&pktCampMessage, 0, sizeof(PktCampMessage));

		if (lpCamp->IsWorldWeapon())
		{
			// 월드 웨폰 권한 변경
			// 메세지 종류, 존 번호, 권한 정보, 공지 아님
			pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_SET_RIGHT;
			pktCampMessage.m_dwCampID	= lpCamp->GetCampID();
			pktCampMessage.m_dwGID		= lpCamp->GetGID();
			pktCampMessage.m_cZone		= lpCamp->GetZone();
			pktCampMessage.m_bNotify	= false;

			pktCampMessage.m_Right.m_cCampControl = diffCampRight.m_aryCampRight[CampRight::CAMP_CONTROL];

			if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
			{
				DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
					reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
			}
		}
		else
		{
			// 길드 요새 권한 변경
			// 메세지 종류, 존 번호, 권한 정보, 공지 아님
			pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_SET_RIGHT;
			pktCampMessage.m_dwCampID	= lpCamp->GetCampID();
			pktCampMessage.m_dwGID		= lpCamp->GetGID();
			pktCampMessage.m_cZone		= lpCamp->GetZone();
			pktCampMessage.m_bNotify	= false;

			pktCampMessage.m_Right.m_cCampControl = diffCampRight.m_aryCampRight[CampRight::CAMP_CONTROL];

			if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
			{
				DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
					reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PktCampRight* lpPktCampRightAck = reinterpret_cast<PktCampRight*>(
        SendStream.GetBuffer(sizeof(PktCampRight)));

    if (NULL != lpPktCampRightAck)
    {
        lpPktCampRightAck->m_dwCampID	= dwCampID;
        lpPktCampRightAck->m_CampRight	= campRight;

        if (SendStream.WrapHeader(sizeof(PktCampRight), CmdCampRight, 0, wError))
        {
            return true;
        }
    }

    return false;    
}

bool CreateCamp(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktCreateCamp* lpPktCreateCamp = static_cast<PktCreateCamp*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;
	CSiegeObjectDB* lpCamp = NULL;

	switch (lpPktCreateCamp->m_wObjectType)
	{
		case Siege::CAMP:
		{			
			lpCamp = CSiegeObjectDBMgr::GetInstance().CreateCamp(lpPktCreateCamp->m_dwGID, lpPktCreateCamp->m_cZone,
				lpPktCreateCamp->m_cChannel, lpPktCreateCamp->m_Pos);
		}
		break;

		case Siege::KARTERANT_WEAPON:
		case Siege::MERKADIA_WEAPON:
		{
			if (Creature::STATELESS == CSiegeObjectDBMgr::GetInstance().GetWorldWeaponNation(lpPktCreateCamp->m_cZone, lpPktCreateCamp->m_cChannel))
			{
				lpCamp = CSiegeObjectDBMgr::GetInstance().CreateWorldWeapon(lpPktCreateCamp->m_dwGID, lpPktCreateCamp->m_wObjectType,
								lpPktCreateCamp->m_cZone, lpPktCreateCamp->m_cChannel, lpPktCreateCamp->m_Pos);
			}
			else
			{
				wError = PktCreateCamp::ERROR_EXIST_ANOTHER_WEAPON;
			}
		}
		break;
	}

    PktCreateCamp* lpPktCreateCampAck = reinterpret_cast<PktCreateCamp*>(
        SendStream.GetBuffer(sizeof(PktCreateCamp)));

    if (NULL != lpPktCreateCampAck)
    {
        if (lpCamp)
        {
			lpPktCreateCampAck->m_dwActorCID	= lpPktCreateCamp->m_dwActorCID;
            lpPktCreateCampAck->m_dwCID			= lpCamp->GetCID();
            lpPktCreateCampAck->m_dwCampID		= lpCamp->GetCampID();
            lpPktCreateCampAck->m_dwGID			= lpCamp->GetGID();
            lpPktCreateCampAck->m_dwHP			= 0;
			lpPktCreateCampAck->m_wObjectType	= lpCamp->GetObjectType();
            lpPktCreateCampAck->m_cState		= lpCamp->GetState();
			lpPktCreateCampAck->m_cSubState		= lpCamp->GetSubState();
            lpPktCreateCampAck->m_cUpgradeStep	= 0;
			lpPktCreateCampAck->m_cNation		= 0;
            lpPktCreateCampAck->m_cZone			= lpCamp->GetZone();
            lpPktCreateCampAck->m_cChannel		= lpCamp->GetChannel();
            lpPktCreateCampAck->m_Pos.fPointX	= lpCamp->GetPosition().m_fPointX;
            lpPktCreateCampAck->m_Pos.fPointY	= lpCamp->GetPosition().m_fPointY;
            lpPktCreateCampAck->m_Pos.fPointZ	= lpCamp->GetPosition().m_fPointZ;
			lpPktCreateCampAck->m_cMaterial		= 0;

			SendStream.WrapHeader(sizeof(PktCreateCamp), CmdCreateCamp, 0, wError);

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 메세지 보내기
			PktCampMessage pktCampMessage;
			memset(&pktCampMessage, 0, sizeof(PktCampMessage));

			if (lpCamp->IsWorldWeapon())
			{
				// 월드 웨폰 구축 시작 
				// 메세지 종류, 길드 이름, 길드 마스터 이름, 존 번호, 구축 시간, 공지
				pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_WEAPON_ENCAMP_START;
				pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
				pktCampMessage.m_dwGID			= lpCamp->GetGID();
				pktCampMessage.m_cZone			= lpCamp->GetZone();
				pktCampMessage.m_cRemainTime	= Siege::CAMP_ENCAMPING_TIME;
				pktCampMessage.m_bNotify		= true;

				Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(Guild::CGuildDBMgr::GetInstance().GetGuild( lpCamp->GetGID() ));
				if ( lpGuild )
				{
					strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());		// 길드 이름
					const Guild::MemberInfo& masterInfo = lpGuild->GetMaster();
					strcpy(pktCampMessage.m_szMasterName, masterInfo.m_strName);	// 길드 마스터 이름
				}

				if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
				{
					DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
						reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
				}
			}
			else
			{
				// 길드 요새 구축 시작 
				// 메세지 종류, 존 번호, 길드 마스터 이름, 구축 시간, 공지 아님
				pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_ENCAMP_START;
				pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
				pktCampMessage.m_dwGID			= lpCamp->GetGID();
				pktCampMessage.m_cZone			= lpCamp->GetZone();
				pktCampMessage.m_cRemainTime	= Siege::CAMP_ENCAMPING_TIME;
				pktCampMessage.m_bNotify		= false;

				Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(Guild::CGuildDBMgr::GetInstance().GetGuild( lpCamp->GetGID() ));
				if ( lpGuild )
				{
					const Guild::MemberInfo& masterInfo = lpGuild->GetMaster();
					strcpy(pktCampMessage.m_szMasterName, masterInfo.m_strName);	// 길드 마스터 이름
				}

				if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
				{
					DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
						reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
				}
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			return true;
        }
        else
        {
			if (0 == wError)
			{
				wError = PktCreateCamp::SERVER_ERROR;
			}

            ::memset(lpPktCreateCampAck, 0, sizeof(PktCreateCamp));
			return SendStream.WrapHeader(sizeof(PktCreateCamp), CmdCreateCamp, 0, wError);
        }
    }

    return false;
}

bool CreateSiegeArms(CSendStream& SendStream, PktBase* lpPktBase)
{    
    PktCreateSiegeArms* lpPktCreateSiegeArms = static_cast<PktCreateSiegeArms*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    CSiegeObjectDB* lpSiegeArms = CSiegeObjectDBMgr::GetInstance().CreateSiegeArms(
		lpPktCreateSiegeArms->m_dwOwnerID,
		lpPktCreateSiegeArms->m_cNation,
        lpPktCreateSiegeArms->m_cZone,
		lpPktCreateSiegeArms->m_cChannel,
        lpPktCreateSiegeArms->m_wObjectType,
		lpPktCreateSiegeArms->m_cUpgradeStep,
        lpPktCreateSiegeArms->m_Pos);

    
    PktCreateSiegeArms* lpPktCreateSiegeArmsAck = 
        reinterpret_cast<PktCreateSiegeArms*>(SendStream.GetBuffer(sizeof(PktCreateSiegeArms)));

    if (NULL != lpPktCreateSiegeArmsAck)
    {
        if (lpSiegeArms)
        {
            lpPktCreateSiegeArmsAck->m_dwCID		= lpSiegeArms->GetCID();
            lpPktCreateSiegeArmsAck->m_dwOwnerID	= lpSiegeArms->GetOwnerID();
            lpPktCreateSiegeArmsAck->m_wObjectType	= lpSiegeArms->GetObjectType();
            lpPktCreateSiegeArmsAck->m_cNation		= (unsigned char)lpSiegeArms->GetNation();
            lpPktCreateSiegeArmsAck->m_dwHP			= lpSiegeArms->GetNowHP();
            lpPktCreateSiegeArmsAck->m_cState		= lpSiegeArms->GetState();
            lpPktCreateSiegeArmsAck->m_cUpgradeStep	= lpSiegeArms->GetUpgradeStep();
            lpPktCreateSiegeArmsAck->m_cZone		= lpSiegeArms->GetZone();
            lpPktCreateSiegeArmsAck->m_cChannel		= lpSiegeArms->GetChannel();
            lpPktCreateSiegeArmsAck->m_Pos.fPointX	= lpSiegeArms->GetPosition().m_fPointX;
            lpPktCreateSiegeArmsAck->m_Pos.fPointY	= lpSiegeArms->GetPosition().m_fPointY;
            lpPktCreateSiegeArmsAck->m_Pos.fPointZ	= lpSiegeArms->GetPosition().m_fPointZ;
        }
        else
        {
			wError = PktCreateSiegeArms::SERVER_ERROR;
            ::memset(lpPktCreateSiegeArmsAck, 0, sizeof(PktCreateSiegeArms));
        }

        return SendStream.WrapHeader(sizeof(PktCreateSiegeArms), CmdCreateSiegeArms, 0, wError);
    }

    return false;
}

bool CampCmd(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktCampCmd* lpPktCampCmd = static_cast<PktCampCmd*>(lpPktBase);

    unsigned short wError = PktBase::NO_SERVER_ERR;

    unsigned long dwCID			= lpPktCampCmd->m_dwCID;
    unsigned long dwCampID		= lpPktCampCmd->m_dwCampID;
    unsigned char cState		= lpPktCampCmd->m_cState;
    unsigned long dwValue1		= lpPktCampCmd->m_dwValue1;
    unsigned long dwValue2		= lpPktCampCmd->m_dwValue2;
    unsigned char cSubCmd		= lpPktCampCmd->m_cSubCmd;

    CCampDB* lpCamp = CSiegeObjectDBMgr::GetInstance().GetCamp(dwCampID);
    if (NULL == lpCamp)
    {
        wError = PktBase::SERVER_ERROR;
    }
    else
    {
        lpCamp->UpdateLastUseTime();

        switch (cSubCmd)
        {
			// 길드 요새 구축 해제
			case PktCampCmd::CAMP_CANCEL:	
            {
                if (false == lpCamp->CancelBuild())
                {
                    wError = PktBase::SERVER_ERROR;
                }

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 메세지 보내기
				if (PktBase::NO_SERVER_ERR == wError)
				{
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (lpCamp->IsWorldWeapon())
					{
						// 월드 웨폰 구축 취소
						// 메세지 종류, 존 번호, 길드 이름, 길드 마스터 이름, 구축 취소 시간, 공지
						pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_WEAPON_CANCEL_START;
						pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
						pktCampMessage.m_dwGID			= lpCamp->GetGID();
						pktCampMessage.m_cZone			= lpCamp->GetZone();
						pktCampMessage.m_cRemainTime	= Siege::CAMP_CANCELING_TIME;
						pktCampMessage.m_bNotify		= true;

						Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(Guild::CGuildDBMgr::GetInstance().GetGuild( lpCamp->GetGID() ));
						if (NULL != lpGuild)
						{
							strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());		// 길드 이름
							const Guild::MemberInfo& masterInfo = lpGuild->GetMaster();
							strcpy(pktCampMessage.m_szMasterName, masterInfo.m_strName);	// 길드 마스터 이름
						}

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 구축 취소
						// 메세지 종류, 존 번호, 길드 마스터 이름, 구축 취소 시간, 공지 아님
						pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_CANCEL_START;
						pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
						pktCampMessage.m_dwGID			= lpCamp->GetGID();
						pktCampMessage.m_cZone			= lpCamp->GetZone();
						pktCampMessage.m_cRemainTime	= Siege::CAMP_CANCELING_TIME;
						pktCampMessage.m_bNotify		= false;

						Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(Guild::CGuildDBMgr::GetInstance().GetGuild( lpCamp->GetGID() ));
						if (NULL != lpGuild)
						{
							const Guild::MemberInfo& masterInfo = lpGuild->GetMaster();
							strcpy(pktCampMessage.m_szMasterName, masterInfo.m_strName);	// 길드 마스터 이름
						}

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            break;

			// Value1[In]  : 업그레이드 시간
			// Value2[In]  : 남은 자재수	
			// Value1[Out] : 업그레이드 단계
	        case PktCampCmd::CAMP_UPGRADE:
            {
                if (false == lpCamp->Upgrade(static_cast<unsigned char>(dwValue1)))
                {
                    wError = PktBase::SERVER_ERROR;
                }

                dwValue1 = lpCamp->GetUpgradeStep();

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 메세지 보내기
				if (PktBase::NO_SERVER_ERR == wError)
				{
					lpCamp->SetMaterialNum(static_cast<unsigned char>(dwValue2));

					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (lpCamp->IsWorldWeapon())
					{
						// 월드 웨폰 업그레이드
						// 메세지 종류, 존 번호, 업그레이드한 PC 이름, 업그레이드 시간, 공지 아님
						pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_WEAPON_UPGRADE_START;
						pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
						pktCampMessage.m_dwGID			= lpCamp->GetGID();
						pktCampMessage.m_cZone			= lpCamp->GetZone();
						pktCampMessage.m_cRemainTime	= lpCamp->GetUpgradeTime();
						pktCampMessage.m_bNotify		= false;

						strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));		// PC 이름

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 업그레이드
						// 메세지 종류, 존 번호, 업그레이드한 PC 이름, 업그레이드 시간, 공지 아님
						pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_UPGRADE_START;
						pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
						pktCampMessage.m_dwGID			= lpCamp->GetGID();
						pktCampMessage.m_cZone			= lpCamp->GetZone();
						pktCampMessage.m_cRemainTime	= lpCamp->GetUpgradeTime();
						pktCampMessage.m_bNotify		= false;

						strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));		// PC 이름

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            break;

			// Value1[In]  : RepairHP 양
			// Value2[In]  : 남은 자재수
		    case PktCampCmd::CAMP_REPAIR:
            {
                if (false == lpCamp->Repair(dwValue1))
                {
                    wError = PktBase::SERVER_ERROR;
                }

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 메세지 보내기
				if (PktBase::NO_SERVER_ERR == wError)
				{
					lpCamp->SetMaterialNum(static_cast<unsigned char>(dwValue2));

					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (lpCamp->IsWorldWeapon())
					{
						// 월드 웨폰 수리
						// 메세지 종류, 존 번호, 수리한 PC 이름, 수리 시간, 공지 아님
						pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_WEAPON_REPAIR_START;
						pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
						pktCampMessage.m_dwGID			= lpCamp->GetGID();
						pktCampMessage.m_cZone			= lpCamp->GetZone();
						pktCampMessage.m_cRemainTime	= lpCamp->GetRepairTime();
						pktCampMessage.m_bNotify		= false;

						strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));		// PC 이름

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 수리
						// 메세지 종류, 존 번호, 수리한 PC 이름, 수리 시간, 공지 아님
						pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_REPAIR_START;
						pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
						pktCampMessage.m_dwGID			= lpCamp->GetGID();
						pktCampMessage.m_cZone			= lpCamp->GetZone();
						pktCampMessage.m_cRemainTime	= lpCamp->GetRepairTime();
						pktCampMessage.m_bNotify		= false;

						strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));		// PC 이름

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            break;

			case PktCampCmd::CAMP_CARE:
				return true;

	        case PktCampCmd::CAMP_TO_STARTKIT:
            {
                if (false == lpCamp->ToStartKit())
                {
                    wError = PktCampCmd::SERVER_ERROR;
                }

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 메세지 보내기
				if (PktBase::NO_SERVER_ERR == wError)
				{
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (lpCamp->IsWorldWeapon())
					{
						// 월드 웨폰 스스로 파괴
						// 메세지 종류, 존 번호, 스스로 파괴한 PC 이름, 파괴 시간, 공지
						pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_WEAPON_SELF_DESTROY_START;
						pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
						pktCampMessage.m_dwGID			= lpCamp->GetGID();
						pktCampMessage.m_cZone			= lpCamp->GetZone();
						pktCampMessage.m_cRemainTime	= Siege::CAMP_TO_STARTKIT_TIME;
						pktCampMessage.m_bNotify		= true;

						strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));		// PC 이름

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 스스로 파괴
						// 메세지 종류, 존 번호, 스스로 파괴한 PC 이름, 파괴 시간, 공지 아님
						pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_SELF_DESTROY_START;
						pktCampMessage.m_dwCampID		= lpCamp->GetCampID();
						pktCampMessage.m_dwGID			= lpCamp->GetGID();
						pktCampMessage.m_cZone			= lpCamp->GetZone();
						pktCampMessage.m_cRemainTime	= Siege::CAMP_TO_STARTKIT_TIME;
						pktCampMessage.m_bNotify		= false;

						strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));		// PC 이름

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            break;

			// Value1[In]  : 변형할 타입
			// Value2[In]  : 남은 자재수
			// Value1[Out] : 변형할 타입
			case PktCampCmd::CAMP_CHANGE_TYPE:
			{
				if (false == lpCamp->ChangeType( static_cast<unsigned short>(dwValue1) ))
				{
					wError = PktCampCmd::SERVER_ERROR;
				}

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 메세지 보내기
				if (PktBase::NO_SERVER_ERR == wError)
				{
					lpCamp->SetMaterialNum( static_cast<unsigned char>(dwValue2) );

					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					// 길드 요새 타입 변형
					// 메세지 종류, 존 번호, 변형을 시도한 캐릭터 이름, 기존 타입, 변형할 타입, 공지 아님
					pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_CHANGE_TYPE_START;
					pktCampMessage.m_dwCampID	= lpCamp->GetCampID();
					pktCampMessage.m_dwGID		= lpCamp->GetGID();
					pktCampMessage.m_cZone		= lpCamp->GetZone();
					pktCampMessage.m_nValue1	= dwValue1;
					pktCampMessage.m_nValue2	= lpCamp->GetObjectType();
					pktCampMessage.m_bNotify	= false;

					strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));	// 변형을 시도한 캐릭터 이름

					if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
					{
						DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
							reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
					}
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
			break;

			// Value1[In] : 파괴한 길드 아이디
			// Value1[Out] : 파괴한 길드 아이디
	        case PktCampCmd::CAMP_DESTROY:
            {
				unsigned char cTime = 0;

				// --------------------------------------------------------------------------------------------------------------
				// 명성 처리
				int nFamePerMember = 0;

				// 파괴당한 길드
				Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB *>(
					Guild::CGuildDBMgr::GetInstance().GetGuild(lpCamp->GetGID()));

				// 파괴한 길드
				Guild::CGuildDB* lpOffenceGuild = static_cast<Guild::CGuildDB *>(
					Guild::CGuildDBMgr::GetInstance().GetGuild(dwValue1));

				if (lpGuild) 
				{
					lpGuild->AddAllMemberFame( Guild::TYPE_VALUE, Siege::FAME_FOR_DESTROYED_CAMP, lpOffenceGuild );
				}

				if (lpGuild && lpOffenceGuild)
				{
					TIME* pTime = lpOffenceGuild->GetGuildPointTime();

					if(pTime)
					{
						CTime nowTime = CTime::GetCurrentTime();
						CTimeSpan timeSpan(0);

						CTime GuildPointTime(pTime->Year, pTime->Month, pTime->Day, pTime->Hour, pTime->Minute, pTime->Second);

						timeSpan = nowTime - GuildPointTime;

						// Minbobo

						if(timeSpan.GetTotalMinutes() >= Guild::TIME_GUILD_POINT)
						{
							int nTotalFame = lpGuild->GetCurrentMemberNum() * (-Siege::FAME_FOR_DESTROYED_CAMP);
							int nMemberNum = lpOffenceGuild->GetLoginedCurrentMemberNum();
							if (0 < nMemberNum)
							{
								nFamePerMember = nTotalFame / nMemberNum;
								if (0 < nFamePerMember)
								{
                                  	lpOffenceGuild->AddLoginedMemberFame( Guild::TYPE_VALUE, nFamePerMember, lpGuild );
								}
							}

							lpOffenceGuild->SetGuildPointFlag(1);

							SYSTEMTIME sysTime;
							GetLocalTime(&sysTime);

							pTime->Year		= sysTime.wYear;
							pTime->Month	= sysTime.wMonth;
							pTime->Day		= sysTime.wDay;
							pTime->Hour		= sysTime.wHour;
							pTime->Minute	= sysTime.wMinute;
							pTime->Second	= sysTime.wSecond;
						}
						else
						{
							cTime = (unsigned char)timeSpan.GetTotalMinutes();
						}															
					}
				}
				// --------------------------------------------------------------------------------------------------------------

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 메세지 보내기
				PktCampMessage pktCampMessage;
				memset(&pktCampMessage, 0, sizeof(PktCampMessage));

				if (lpCamp->IsWorldWeapon())
				{
					// 월드 웨폰 파괴 공지
					// 메세지 종류, 존 번호, 파괴한 PC의 국가, 파괴한 PC의 이름, 파괴당한 길드 이름, 파괴당한 국가, 공지
					pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_DESTROYED;
					pktCampMessage.m_dwCampID	= lpCamp->GetCampID();
					pktCampMessage.m_dwGID		= lpCamp->GetGID();
					pktCampMessage.m_cZone		= lpCamp->GetZone();
					pktCampMessage.m_cNation	= GetCharacterNation(dwCID);
					pktCampMessage.m_bNotify	= true;

					strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));		// 파괴한 캐릭터 이름

					if ( lpGuild )
					{
						strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());	// 파괴당한 길드 이름
						pktCampMessage.m_nValue1 = lpGuild->GetNation();			// 파괴당한 국가
					}

					if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
					{
						DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
							reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
					}
				}
				else
				{
					// 길드 요새 파괴 공지
					// 메세지 종류, 존 번호, 파괴한 PC의 국가, 파괴한 PC의 이름, 파괴당한 길드 이름, 공지
					pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_DESTROYED;
					pktCampMessage.m_dwCampID	= lpCamp->GetCampID();
					pktCampMessage.m_dwGID		= lpCamp->GetGID();
					pktCampMessage.m_cZone		= lpCamp->GetZone();
					pktCampMessage.m_cNation	= GetCharacterNation(dwCID);
					pktCampMessage.m_bNotify	= true;

					if(GetCharacterName(dwCID))
					{
						strcpy(pktCampMessage.m_szPCName, GetCharacterName(dwCID));
					}
					
					
					if ( lpGuild )
					{
						strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());
					}

					if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
					{
						DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
							reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
					}

					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					// 길드 요새 파괴 알림 (파괴당한 길드 요새의 길드원에게)
					// 메세지 종류, 존 번호, 명성, 공지 아님
					pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_DESTROYED;
					pktCampMessage.m_dwCampID	= lpCamp->GetCampID();
					pktCampMessage.m_dwGID		= lpCamp->GetGID();
					pktCampMessage.m_cZone		= lpCamp->GetZone();
					pktCampMessage.m_nValue1	= -(Siege::FAME_FOR_DESTROYED_CAMP);
					pktCampMessage.m_bNotify	= false;

					if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
					{
						DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
							reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
					}

					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					// 길드 요새 파괴 알림 (파괴한 PC의 길드원에게)
					// 메세지 종류, 존 번호, 파괴된 길드 요새의 국가, 파괴된 길드 요새의 길드 이름, 명성, 공지 아님
					pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_DESTROY;
					pktCampMessage.m_dwCampID	= lpCamp->GetCampID();
					pktCampMessage.m_dwGID		= dwValue1;
					pktCampMessage.m_cZone		= lpCamp->GetZone();
					pktCampMessage.m_nValue1	= nFamePerMember;
					pktCampMessage.m_bNotify	= false;
					pktCampMessage.m_cTimeChk	= cTime;

					if ( lpGuild )
					{
						pktCampMessage.m_cNation = lpGuild->GetNation();
						strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());
					}

					if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
					{
						DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
							reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
					}
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                lpCamp->SendCampCmd(PktCampCmd::CAMP_DESTROY, dwValue1);
                lpCamp->Destroy();

				CSiegeObjectDBMgr::GetInstance().DeleteCamp( lpCamp->GetCampID() );
                return true;
            }
            break;

            // HP 업데이트
			// Value1[In] : 현재 HP
			case PktCampCmd::CAMP_UPDATE_HP:
            {
                lpCamp->UpdateHP(dwValue1);
                return true;
            }
            break;

			// 채굴기 상태 On / Off
			// Value1[In] : 바꿀 상태 (On / Off)
			// Value1[Out] : 바뀐 상태 (On / Ready / Off)
			case PktCampCmd::MINING_CAMP_ON_OFF:
			{
				if ( Siege::MINING_CAMP == lpCamp->GetObjectType() )
				{
					CMiningCampDB* lpMiningCamp = reinterpret_cast<CMiningCampDB*>(lpCamp);
					if ( lpMiningCamp )
					{
						if ( !lpMiningCamp->OnOff( dwValue1 ) )
						{
							ERRLOG3(g_Log, "채굴기 켜기/끄기 실패, CampID : 0x%10u, CurrSubState : %d, ChangeState : %d",
									lpCamp->GetCampID(), lpCamp->GetSubState(), dwValue1);
							return true;
						}
					}
				}
				else
				{
					ERRLOG1(g_Log, "채굴기 타입으로 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
					return true;
				}
			}
			break;

			// 채굴기 누적 광물 획득
			// dwValue1[In] : Mineral Item ID
			// dwValue2[In] : Amount
			case PktCampCmd::MINING_CAMP_GAIN_MINERAL:
			{
				if ( Siege::MINING_CAMP == lpCamp->GetObjectType() )
				{
					CMiningCampDB* lpMiningCamp = reinterpret_cast<CMiningCampDB*>(lpCamp);
					if ( lpMiningCamp )
					{
						unsigned short wMineralID = static_cast<unsigned short>( dwValue1 );
						unsigned short wAmount = static_cast<unsigned short>( dwValue2 );

						if ( !lpMiningCamp->GainMineral(wMineralID, wAmount) )
						{
							ERRLOG3(g_Log, "채굴기 누적 광물 획득 실패, CampID : 0x%10u, MineralID : %d, Amount : %d",
								lpCamp->GetCampID(), wMineralID, wAmount);
							return true;
						}
					}
				}
				else
				{
					ERRLOG1(g_Log, "채굴기 타입으로 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
					return true;
				}
			}
			break;

			// 세율 변경 (길드 요새 상점)
			// dwValue1[In] : 변경 후 세율
			case PktCampCmd::CAMP_SHOP_CHANGE_TAX:
			{
				if (Siege::CAMP_SHOP == lpCamp->GetObjectType())
				{
					CCampShopDB* lpCampShop = reinterpret_cast<CCampShopDB*>(lpCamp);
					if (NULL != lpCampShop)
					{
						unsigned char cTax = static_cast<unsigned char>(dwValue1);
						lpCampShop->SetTax(cTax);
					}
				}
				else
				{
					ERRLOG1(g_Log, "상점 타입으로 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
					return true;
				}
			}
			break;

			// 월드 웨폰 무기 장전
			// dwValue1[In] : 장전 시간
			// dwValue2[In] : 남은 자재수
			case PktCampCmd::WORLDWEAPON_CHARGE:
			{
				if ( Siege::KARTERANT_WEAPON == lpCamp->GetObjectType() || Siege::MERKADIA_WEAPON == lpCamp->GetObjectType() )
				{
					CWorldWeaponDB* lpWorldWeapon = reinterpret_cast<CWorldWeaponDB*>(lpCamp);
					if (NULL != lpWorldWeapon)
					{
						if (false == lpWorldWeapon->UpdateWeaponState(Siege::WEAPON_CHARGE, dwValue1, dwValue2))
						{
							ERRLOG2(g_Log, "월드 웨폰 무기 장전 실패, CampID : 0x%10u, SubState : %d",
								lpWorldWeapon->GetCampID(), lpWorldWeapon->GetSubState());
							return true;
						}
						else
						{
							lpWorldWeapon->SetMaterialNum( static_cast<unsigned char>(dwValue2) );

							//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							// 메세지 보내기
							PktCampMessage pktCampMessage;
							memset(&pktCampMessage, 0, sizeof(PktCampMessage));

							// 월드 웨폰 무기 장전
							// 메세지 종류, 존 번호, 월드 웨폰 국가, 공지 아님
							pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_WEAPON_CHARGE_START;
							pktCampMessage.m_dwCampID		= lpWorldWeapon->GetCampID();
							pktCampMessage.m_cNation		= GetCharacterNation(dwCID);
							pktCampMessage.m_dwGID			= lpWorldWeapon->GetGID();
							pktCampMessage.m_cZone			= lpWorldWeapon->GetZone();
							pktCampMessage.m_bNotify		= false;

							if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
							{
								DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
									reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
							}
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						}
					}
				}
				else
				{
					ERRLOG1(g_Log, "월드 웨폰 타입으로 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
					return true;
				}
			}
			break;

			// 월드 웨폰 무기 발사
			// dwValue1[In] : X 좌표
			// dwValue2[In] : Z 좌표
			case PktCampCmd::WORLDWEAPON_FIRE:
			{
				if ( Siege::KARTERANT_WEAPON == lpCamp->GetObjectType() || Siege::MERKADIA_WEAPON == lpCamp->GetObjectType() )
				{
					CWorldWeaponDB* lpWorldWeapon = reinterpret_cast<CWorldWeaponDB*>(lpCamp);
					if (NULL != lpWorldWeapon)
					{
						if (false == lpWorldWeapon->UpdateWeaponState(Siege::WEAPON_FIRE, dwValue1, dwValue2))
						{
							ERRLOG2(g_Log, "월드 웨폰 무기 발사 실패, CampID : 0x%10u, SubState : %d",
								lpWorldWeapon->GetCampID(), lpWorldWeapon->GetSubState());
							return true;
						}
					}
				}
				else
				{
					ERRLOG1(g_Log, "월드 웨폰 타입으로 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
					return true;
				}
			}
			break;

			// 월드 웨폰 무기 발사 완료 (Ack 보내지 않는다.)
			case PktCampCmd::WORLDWEAPON_FIRE_COMPLETE:
			{
				if ( Siege::KARTERANT_WEAPON == lpCamp->GetObjectType() || Siege::MERKADIA_WEAPON == lpCamp->GetObjectType() )
				{
					CWorldWeaponDB* lpWorldWeapon = reinterpret_cast<CWorldWeaponDB*>(lpCamp);
					if (NULL != lpWorldWeapon)
					{
						lpWorldWeapon->Fire();

						CSiegeObjectDBMgr::GetInstance().DeleteCamp(lpWorldWeapon->GetCampID());
						return true;
					}
				}
				else
				{
					ERRLOG1(g_Log, "월드 웨폰 타입으로 캐스팅 실패 CampID : 0x%10u", lpCamp->GetCampID());
					return true;
				}
			}
			break;

			default:
				ERRLOG1(g_Log, "Cmd:0x%02X / 존재 하지 않는 길드 요새 명령을 요청하였습니다.", cSubCmd);
				return true;
        }
    }

    PktCampCmd* lpPktCampCmdAck = 
        reinterpret_cast<PktCampCmd*>(SendStream.GetBuffer(sizeof(PktCampCmd)));

    if (NULL != lpPktCampCmdAck)
    {
        if (PktBase::NO_SERVER_ERR == wError)
        {
            lpPktCampCmdAck->m_dwCID	= lpCamp->GetCID();
            lpPktCampCmdAck->m_dwCampID	= lpCamp->GetCampID();
            lpPktCampCmdAck->m_cState	= lpCamp->GetState();
            lpPktCampCmdAck->m_dwValue1	= dwValue1;
            lpPktCampCmdAck->m_dwValue2	= dwValue2;
            lpPktCampCmdAck->m_cSubCmd	= cSubCmd;
        }
        else
        {
            *lpPktCampCmdAck = *lpPktCampCmd;
        }

        return SendStream.WrapHeader(sizeof(PktCampCmd), CmdCampCmd, 0, wError);
    }

    return false;
}

bool CampMessage(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktCampMessage* lpPktCampMessage = static_cast<PktCampMessage*>(lpPktBase);
	
	unsigned long dwCampID		= lpPktCampMessage->m_dwCampID;
	unsigned char cMsgCmd		= lpPktCampMessage->m_cMsgCmd;
	
	CCampDB* lpCamp = CSiegeObjectDBMgr::GetInstance().GetCamp(dwCampID);
	if (NULL == lpCamp)
	{
		return false;
	}

	if (cMsgCmd == PktCampMessage::MSGCMD_TAKE_IN_MATERIAL ||
		cMsgCmd == PktCampMessage::MSGCMD_TAKE_OUT_MATERIAL)
	{
		lpCamp->SetMaterialNum( static_cast<unsigned char>(lpPktCampMessage->m_nValue2) );
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 메세지 보내기
	PktCampMessage pktCampMessage;
	memcpy(&pktCampMessage, lpPktCampMessage, sizeof(PktCampMessage));

	// 메세지 종류, 존 번호, 공지 아님
	pktCampMessage.m_dwGID		= lpCamp->GetGID();
	pktCampMessage.m_cZone		= lpCamp->GetZone();
	
	if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
	{
		DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
			reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));

		return true;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return false;
}


bool MiningCampMineralInfo(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktMiningCampMineralInfo* lpPktMI = reinterpret_cast<PktMiningCampMineralInfo* >(lpPktBase);
	CampMineralInfo* lpMineralInfo = reinterpret_cast<CampMineralInfo* >( lpPktMI + 1 );

	CMiningCampDB* lpMiningCamp = reinterpret_cast<CMiningCampDB* >( CSiegeObjectDBMgr::GetInstance().GetCamp(lpPktMI->m_dwCampID) );
	if ( lpMiningCamp )
	{
		return lpMiningCamp->UpdateMineralInfo(lpPktMI->m_cMineralType, lpPktMI->m_cNum, lpMineralInfo);
	}

	return false;
}

bool FertilityInfo(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktFertilityInfo* lpPktFI = reinterpret_cast<PktFertilityInfo* >(lpPktBase);
	ZoneChannelInfo* lpZoneChannelInfo = reinterpret_cast<ZoneChannelInfo* >(lpPktFI + 1);
	unsigned long* lpFertilityData = reinterpret_cast<unsigned long* >(lpZoneChannelInfo + 1);

	return CMineralVeinDBMgr::GetInstance().UpdateFertilityInfo(lpZoneChannelInfo->m_cZone, lpZoneChannelInfo->m_cChannel,
																lpPktFI->m_wNum, lpFertilityData);
}

bool CampShopInfo(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktCampShopInfo* lpCampShopInfo = reinterpret_cast<PktCampShopInfo* >(lpPktBase);
	char* lpItemBuffer = reinterpret_cast<char* >(lpCampShopInfo + 1);

	CCampShopDB* lpCampDB = reinterpret_cast<CCampShopDB* >(
		CSiegeObjectDBMgr::GetInstance().GetCamp(lpCampShopInfo->m_dwCampID));
	if (NULL == lpCampDB)
	{
		ERRLOG1(g_Log, "게임 서버에서 업데이트하려는 길드 요새 상점이 존재하지 않습니다. CampID:0x%08x", 
			lpCampShopInfo->m_dwCampID);
		return false;
	}

	lpCampDB->SetItemContainer(lpItemBuffer, lpCampShopInfo->m_CampShopInfo.m_dwBufferSize, 
		lpCampShopInfo->m_CampShopInfo.m_cItemNum);

	lpCampDB->SetTempSafe(lpCampShopInfo->m_CampShopInfo.m_dwTempSafe);
	lpCampDB->SetTax(lpCampShopInfo->m_CampShopInfo.m_cTax);

	return lpCampDB->UpdateCampInfo(
		lpItemBuffer, lpCampShopInfo->m_CampShopInfo.m_dwBufferSize, lpCampShopInfo->m_CampShopInfo.m_cItemNum, 
		lpCampShopInfo->m_CampShopInfo.m_dwTempSafe, lpCampShopInfo->m_CampShopInfo.m_cTax);
}

// 임시 세금 정보를 게임 서버가 보낸 정보로 업데이트한다.
bool CastleTaxMove(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktCastleTaxMove* lpTempTaxInfo = reinterpret_cast<PktCastleTaxMove* >(lpPktBase);

	Castle::CCastleDB* lpCastle = Castle::CCastleDBMgr::GetInstance().GetCastle(lpTempTaxInfo->m_dwCastleID);
	if (NULL == lpCastle)
	{
		ERRLOG1(g_Log, "게임 서버에서 업데이트하려는 성이 존재하지 않습니다. CastleID:0x%08x", lpTempTaxInfo->m_dwCastleID);
		return false;
	}

	return lpCastle->SetTempTaxMoney(lpTempTaxInfo->m_dwMoveTempMoney, lpTempTaxInfo->m_dwResultTaxMoney);
}

// 광물 세금 정보를 게임 서버가 보낸 정보로 업데이트한다.
bool CastleMineralInfo(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktCastleMineralInfo* lpPktMI = reinterpret_cast<PktCastleMineralInfo* >(lpPktBase);
	CastleMineral*	lpMineralInfo = reinterpret_cast<CastleMineral* >( lpPktMI + 1 );
    
	Castle::CCastleDB* lpCastle = Castle::CCastleDBMgr::GetInstance().GetCastle(lpPktMI->m_dwCastleID);
	if (NULL == lpCastle)
	{
		ERRLOG1(g_Log, "게임 서버에서 업데이트하려는 성이 존재하지 않습니다. CastleID:0x%08x", lpPktMI->m_dwCastleID);
		return false;
	}

	return lpCastle->SetMineralInfo(lpPktMI->m_cMineralType, lpPktMI->m_cNum, lpMineralInfo);
}

bool WarOnOff(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktWarOnOff* lpPktWarOnOff = reinterpret_cast<PktWarOnOff*>(lpPktBase);
	unsigned long dwCID = lpPktWarOnOff->m_dwCID;
	unsigned char cType = lpPktWarOnOff->m_cType;
	unsigned char cFlag = lpPktWarOnOff->m_cFlag;

	using namespace DBAgent::DataStorage;

	CSessionData*   lpSessionData = 
		CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

	CCharacterData* lpCharacterData = 0;
	if (NULL != lpSessionData)
	{
		lpCharacterData = lpSessionData->GetCharacterData();
		if (NULL != lpCharacterData && CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
		{
			switch (cType)
			{
				case GameTime::GUILD:
				{
					lpCharacterData->SetGuildWarFlag(cFlag);
				}
				break;

				case GameTime::REALM:
				{
					lpCharacterData->SetRealmWarFlag(cFlag);
				}
				break;

				default:
				{
					ERRLOG2(g_Log, "업데이트 하려는 전쟁 플래그의 전쟁 타입이 부적절합니다. CID : %10u, Type : %d", dwCID, cType);
					return false;
				}
			}
		}
	}

	PktWarOnOff* lpPktWarOnOffAck = reinterpret_cast<PktWarOnOff*>(SendStream.GetBuffer(sizeof(PktWarOnOff)));
	if (NULL != lpPktWarOnOffAck)
	{
		lpPktWarOnOffAck->m_dwCID	= dwCID;
		lpPktWarOnOffAck->m_cType	= cType;
		lpPktWarOnOffAck->m_cFlag	= cFlag;

		return SendStream.WrapHeader(sizeof(PktWarOnOff), CmdWarOnOff, 0, 0);
	}

	return true;
}

bool StatueCmd(CSendStream& SendSream, PktBase* lpPktBase)
{
	PktStatueCmd* lpPktStatueCmd = reinterpret_cast<PktStatueCmd*>(lpPktBase);
	StatueInfo* lpStatueInfo = reinterpret_cast<StatueInfo*>(lpPktStatueCmd + 1);

	unsigned short wSubCmd = lpPktStatueCmd->m_wSubCmd;
	unsigned char cChannel = static_cast<unsigned char>(lpPktStatueCmd->m_dwValue);

	switch (wSubCmd)
	{
		case PktStatueCmd::CMD_UPDATE_KID:
		{
			CStatueDBMgr::GetInstance().UpdateKID(lpStatueInfo->m_dwIndex, cChannel, lpStatueInfo->m_wKID);
		}
		break;

		case PktStatueCmd::CMD_UPDATE_LOST_HP:
		{
			CStatueDBMgr::GetInstance().UpdateLostHP(lpStatueInfo->m_dwIndex, cChannel, lpStatueInfo->m_dwLostHP);
		}
		break;

		default:
			return false;
	}

	return true;
}

bool RealmPoint(CSendStream& SendSream, PktBase* lpPktBase)
{
	using namespace DBAgent::DataStorage;

	PktRealmPoint* lpPktRealmPoint = reinterpret_cast<PktRealmPoint*>(lpPktBase);

	CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(lpPktRealmPoint->m_dwCID);

	if(lpSessionData)
	{
		DBAgent::DataStorage::CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();

		if(lpCharacterData)
		{
			lpCharacterData->SetRealmPoint(lpPktRealmPoint->m_cRealmPoint);

			if(PacketWrap::WrapHeader(reinterpret_cast<char*>(lpPktRealmPoint), sizeof(PktRealmPoint), CmdRealmPoint, 0, 0))
			{
				DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
					reinterpret_cast<char*>(lpPktRealmPoint), sizeof(PktRealmPoint), CmdRealmPoint));
			}
		}		
	}	
	return true;
}


}
}

const char*	GetCharacterName(unsigned long dwCID)
{
	using namespace DBAgent::DataStorage;

	CSessionData*   lpSessionData = 
		CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);


	CCharacterData* lpCharacterData = 0;

	if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
	{
		// 캐릭터가 안매달려있는 경우, 캐릭터를 불러온다.
		lpCharacterData = static_cast<CCharacterData*>(
			CCharacterDataMgr::GetInstance().GetLogoutData(dwCID));

		if (NULL != lpCharacterData)
		{
			return lpCharacterData->GetInfo().Name;
		}
	}		
	else if (CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
	{
		if (NULL != lpCharacterData)
		{
			return lpCharacterData->GetInfo().Name;
		}
	}

	return NULL;
}

unsigned char	GetCharacterNation(unsigned long dwCID)
{
	using namespace DBAgent::DataStorage;

	CSessionData*   lpSessionData = 
		CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

	CCharacterData* lpCharacterData = 0;

	if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
	{
		// 캐릭터가 안매달려있는 경우, 캐릭터를 불러온다.
		lpCharacterData = static_cast<CCharacterData*>(
			CCharacterDataMgr::GetInstance().GetLogoutData(dwCID));

		if (NULL != lpCharacterData)
		{
			Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
				Guild::CGuildDBMgr::GetInstance().GetGuild( lpCharacterData->GetInfo().GID ));

			if ( lpGuild )
			{
				return lpGuild->GetNation();
			}
			else
			{
				switch ( lpCharacterData->GetInfo().Race )
				{
					case CClass::HUMAN :	return Creature::KARTERANT;		break;
					case CClass::AKHAN :	return Creature::MERKADIA;		break;
				}
			}
		}
	}		
	else if (CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
	{
		Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
			Guild::CGuildDBMgr::GetInstance().GetGuild( lpCharacterData->GetInfo().GID ));

		if ( lpGuild )
		{
			return lpGuild->GetNation();
		}
		else
		{
			switch ( lpCharacterData->GetInfo().Race )
			{
				case CClass::HUMAN :	return Creature::KARTERANT;		break;
				case CClass::AKHAN :	return Creature::MERKADIA;		break;
			}
		}
	}

	return Creature::STATELESS;
}
