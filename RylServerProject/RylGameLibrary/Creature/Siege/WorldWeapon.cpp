//--------------------------------------------------------------------------------------------------------------------------
// File Name: WorldWeapon.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 월드 웨폰
//
// Date: 2004. 12. 14. (화)
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "WorldWeapon.h"

#include <Creature/Siege/SiegeObjectMgr.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CWorldWeapon
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------
// Constrcutor, Destructor
//--------------------------------------------------------------------------------------------------------------------------
CWorldWeapon::CWorldWeapon(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, unsigned long dwHP,
						   unsigned short wObjectType, unsigned char cState, unsigned char cSubState, unsigned char cUpgradeStep,
						   unsigned char cMaterial, unsigned char cSiegeCount, const CampRight& campRight, bool bFullHP)
: CCamp(MonsterCreate, dwCampID, dwGID, dwHP, wObjectType, cState, cSubState, cUpgradeStep, cMaterial, cSiegeCount, campRight, bFullHP),
  m_iFireX(0), m_iFireZ(0)
{
}

CWorldWeapon::~CWorldWeapon()
{
}

//--------------------------------------------------------------------------------------------------------------------------
// Member Functions
//--------------------------------------------------------------------------------------------------------------------------
bool	CWorldWeapon::Build(unsigned char cUpgradeStep)
{
	CCamp::Build(cUpgradeStep);

	// 월드 웨폰 인챈트 적용
	unsigned char cNation = Creature::STATELESS;
	switch (m_wObjectType)
	{
		case Siege::KARTERANT_WEAPON:	cNation = Creature::KARTERANT;	break;
		case Siege::MERKADIA_WEAPON:	cNation = Creature::MERKADIA;	break;
	}

	CCreatureManager::GetInstance().AddWorldWeaponEnchant(reinterpret_cast<CAggresiveCreature*>(this), cNation);
	return true;
}

bool	CWorldWeapon::Destroy(unsigned long dwOffencerGID)
{
	// 월드 웨폰일 경우 파괴 정보 업데이트
	if (Siege::KARTERANT_WEAPON == m_wObjectType)
	{
		CSiegeObjectMgr::GetInstance().UpdateWorldWeaponInfo(Creature::KARTERANT, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}
	else if (Siege::MERKADIA_WEAPON == m_wObjectType)
	{
		CSiegeObjectMgr::GetInstance().UpdateWorldWeaponInfo(Creature::MERKADIA, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}

	return CCamp::Destroy(dwOffencerGID);
}

bool	CWorldWeapon::UpdateWeaponState(unsigned char cWeaponState, int iX, int iZ)
{
	m_iFireX = iX;
	m_iFireZ = iZ;
	m_cSubState = cWeaponState;
	m_iWorldWeaponCount = Siege::WEAPON_FIRE_WARNING_COUNT;
    
	// 해당 진지가 있는 반경 5셀 이내에 전송
	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= iX;
	pktCC.m_dwValue2		= iZ;
	pktCC.m_cSubCmd			= 0;

	switch (cWeaponState)
	{
		case Siege::WEAPON_CHARGE:
		{
			pktCC.m_cSubCmd	= PktCampCmd::WORLDWEAPON_CHARGE;
			pktCC.m_dwValue1 = m_cMaterial;
			pktCC.m_dwValue2 = 0;
		}
		break;

		case Siege::WEAPON_READY:	pktCC.m_cSubCmd	= PktCampCmd::WORLDWEAPON_CHARGE_COMPLETE;	break;
		case Siege::WEAPON_FIRE:	pktCC.m_cSubCmd	= PktCampCmd::WORLDWEAPON_FIRE;				break;
	}
	
	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	return true;
}

bool	CWorldWeapon::FireWarning()
{
	// 경고 메세지를 모든 캐릭터에게 전송
	PktCampMessage pktCampMessage;
	memset(&pktCampMessage, 0, sizeof(PktCampMessage));

	pktCampMessage.m_dwCampID		= GetCampID();
	pktCampMessage.m_dwGID			= GetGID();
	pktCampMessage.m_cZone			= CServerSetup::GetInstance().GetServerZone();
	pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_WEAPON_FIRE_WARNING;
	pktCampMessage.m_cRemainTime	= static_cast<unsigned char>(m_iWorldWeaponCount);
	pktCampMessage.m_cNation		= GetNation();
	pktCampMessage.m_bNotify		= true;
	pktCampMessage.m_nValue1		= m_iFireX;
	pktCampMessage.m_nValue2		= m_iFireZ;

	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
	if (NULL != lpGuild)
	{
		strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());
	}

	char* szPacket = reinterpret_cast<char* >(&pktCampMessage);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampMessage), CmdCampMessage, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCampMessage), CmdCampMessage);
	}

	--m_iWorldWeaponCount;
	return true;
}

bool	CWorldWeapon::Fire()
{
	// 월드 웨폰일 경우 파괴 정보 업데이트
	if (Siege::KARTERANT_WEAPON == m_wObjectType)
	{
		CSiegeObjectMgr::GetInstance().UpdateWorldWeaponInfo(Creature::KARTERANT, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}
	else if (Siege::MERKADIA_WEAPON == m_wObjectType)
	{
		CSiegeObjectMgr::GetInstance().UpdateWorldWeaponInfo(Creature::MERKADIA, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}

	// DB 중계 서버에 월드 웨폰 객체 삭제 명령 전송 (발사 완료, 시간 기록)
	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (NULL != lpDBAgentDispatch)
	{
		GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), 0, GetCampID(), 
			0, 0, PktCampCmd::WORLDWEAPON_FIRE_COMPLETE, PktBase::NO_SERVER_ERR);
	}

	// 모든 캐릭터에게 월드 웨폰 발사 명령 전송 (이펙트 처리 및 월드 웨폰 객체 삭제를 위해)
	PktCampCmd pktCampCmd;
	pktCampCmd.m_dwCID		= m_dwCID;
	pktCampCmd.m_dwCampID	= GetCampID();
	pktCampCmd.m_cState		= GetState();
	pktCampCmd.m_dwValue1	= m_iFireX;
	pktCampCmd.m_dwValue2	= m_iFireZ;
	pktCampCmd.m_cSubCmd	= PktCampCmd::WORLDWEAPON_FIRE_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCampCmd);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	// 모든 캐릭터 및 공성 오브젝트에 대해서 데미지 처리
	CCreatureManager::GetInstance().ProcessWorldWeaponDamage(m_iFireX, m_iFireZ, m_cUpgradeStep);

	// 모든 캐릭터의 월드 웨폰 인챈트 해제
	CCreatureManager::GetInstance().ClearWorldWeaponEnchant();

	return true;
}
