//--------------------------------------------------------------------------------------------------------------------------
// File Name: WorldWeapon.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: ���� ����
//
// Date: 2004. 12. 14. (ȭ)
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

	// ���� ���� ��æƮ ����
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
	// ���� ������ ��� �ı� ���� ������Ʈ
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
    
	// �ش� ������ �ִ� �ݰ� 5�� �̳��� ����
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
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	return true;
}

bool	CWorldWeapon::FireWarning()
{
	// ��� �޼����� ��� ĳ���Ϳ��� ����
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
	// ���� ������ ��� �ı� ���� ������Ʈ
	if (Siege::KARTERANT_WEAPON == m_wObjectType)
	{
		CSiegeObjectMgr::GetInstance().UpdateWorldWeaponInfo(Creature::KARTERANT, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}
	else if (Siege::MERKADIA_WEAPON == m_wObjectType)
	{
		CSiegeObjectMgr::GetInstance().UpdateWorldWeaponInfo(Creature::MERKADIA, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}

	// DB �߰� ������ ���� ���� ��ü ���� ��� ���� (�߻� �Ϸ�, �ð� ���)
	// �߰� ������ ��Ŷ ����
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (NULL != lpDBAgentDispatch)
	{
		GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), 0, GetCampID(), 
			0, 0, PktCampCmd::WORLDWEAPON_FIRE_COMPLETE, PktBase::NO_SERVER_ERR);
	}

	// ��� ĳ���Ϳ��� ���� ���� �߻� ��� ���� (����Ʈ ó�� �� ���� ���� ��ü ������ ����)
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

	// ��� ĳ���� �� ���� ������Ʈ�� ���ؼ� ������ ó��
	CCreatureManager::GetInstance().ProcessWorldWeaponDamage(m_iFireX, m_iFireZ, m_cUpgradeStep);

	// ��� ĳ������ ���� ���� ��æƮ ����
	CCreatureManager::GetInstance().ClearWorldWeaponEnchant();

	return true;
}
