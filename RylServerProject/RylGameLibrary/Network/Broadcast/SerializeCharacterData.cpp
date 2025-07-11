#include "stdafx.h"

#include "SerializeCharacterData.h"

#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/PatternMonster.h>

#include <Network/Dispatch/Chat/ChatDispatch.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include <Utility/Setup/ServerSetup.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Community/Party/PartyMgr.h>

#include <GameTime/GameTimeMgr.h>

namespace Broadcast2nd
{

CSerializeCharacterData::CSerializeCharacterData()
:   m_LastChatData(0, 0, 0),
    m_usBroadcastDataLen(0), 
    m_usDeltaBroadcastDataLen(0), 
    m_dwPreparedCount(0)
{



}

CSerializeCharacterData::~CSerializeCharacterData()
{


}

void CSerializeCharacterData::InitializeData(CCharacter& character)
{
    unsigned short usEquipments[Item::EquipmentPos::MAX_EQUPMENT_POS];
    character.GetEquipments().GetEquipmentView(usEquipments, 0, Item::EquipmentPos::MAX_EQUPMENT_POS);

    const size_t nNameLen = std::min(size_t(CCharacterData::MAX_NAME), size_t(CHAR_INFOST::MAX_NAME_LEN));
    const char* szCharacterName = character.GetCharacterName();

    const size_t nStallNameLen = 
		std::min(size_t(StallNode::MAX_STALL_NAME), size_t(Item::CCharacterShopContainer::MAX_STALL_NAME_LEN));
    const char* szStallName = character.GetStall().GetStallName();

    unsigned long dwGID = character.GetGID();
    unsigned long dwPID = character.GetPID();
    unsigned long dwFame = character.GetFame();
    unsigned long dwMileage = character.GetMileage();

    bcStatusInfo statusInfo;

    statusInfo.m_BodyInfo.m_cHair           = character.GetHair();                  //! �Ӹ�Į
    statusInfo.m_BodyInfo.m_cFace           = character.GetFace();                  //! ��
    statusInfo.m_BodyInfo.m_cNationality    = character.GetNation();                //! ����
    statusInfo.m_BodyInfo.m_cRace           = character.GetRace();                  //! ����
    statusInfo.m_BodyInfo.m_cAdminLevel     = character.GetAdminLevel();            //! ��� ���� ( 0 : �Ϲ�, 1���� ũ�� ���..)
    statusInfo.m_BodyInfo.m_cSex            = character.GetSex();                   //! 0 : ����,     1 : ����
	statusInfo.m_BodyInfo.m_bPeaceMode      = character.IsPeaceMode() ? 1 : 0;		//! 0 : ������, 1 : ��ȭ���
    statusInfo.m_BodyInfo.m_cHandPos        = character.GetHand();                  //! 0 : 1�� ��,   1 : 2�� ��
    statusInfo.m_BodyInfo.m_cRideFlag       = character.GetRide();                  //! 0 : x,   1 : ��Ž
    statusInfo.m_BodyInfo.m_cReserved       = 0;

    statusInfo.m_cClass = static_cast<unsigned char>(character.GetClass());
    statusInfo.m_cLevel = character.GetLevel();

    const Position& characterPos = character.GetCurrentPos();
    CNetworkPos netPos(characterPos.m_fPointX, characterPos.m_fPointY, characterPos.m_fPointZ);

    m_LastCharacterData.SetEquipmentsID(usEquipments, Item::EquipmentPos::MAX_EQUPMENT_POS);
    m_LastCharacterData.SetName(szCharacterName, nNameLen);
    m_LastCharacterData.SetStallName(szStallName, nStallNameLen);
    m_LastCharacterData.SetGuildID(dwGID);
	m_LastCharacterData.SetTacticsFlag(character.GetTacticsFlag());
	
	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( dwGID );
	
	if ( lpGuild )
	{						
		// ���� ����ڴ� ��Ʈ ������ �ؼ� �����ش�.
		if (Guild::JOIN_WAIT == lpGuild->GetTitle( character.GetCID() ))
		{
			m_LastCharacterData.SetGuildID(dwGID | Guild::MEMBER_WAIT_BIT);
		}		
	}
	
    m_LastCharacterData.SetPartyID(dwPID);
    m_LastCharacterData.SetFame(dwFame);
    m_LastCharacterData.SetMileage(dwMileage);
    m_LastCharacterData.SetBodyInfo(statusInfo.m_dwBodyInfo);
    m_LastCharacterData.SetClass(statusInfo.m_cClass);
    m_LastCharacterData.SetLevel(statusInfo.m_cLevel);
    m_LastCharacterData.SetEnchantInfo(character.GetEnchantInfo());
    m_LastCharacterData.SetNetworkPos(netPos);
    m_LastCharacterData.SetHP(character.GetStatus().m_nNowHP);
	m_LastCharacterData.SetGuildWarFlag(character.GetGuildWarFlag());
	m_LastCharacterData.SetRealmWarFlag(character.GetRealmWarFlag());

	m_LastCharacterData.SetRealmPoint(character.GetRealmPoint());

    PrepareData(character);
}

void CSerializeCharacterData::PrepareData(CCharacter& character)
{
    PrepareBroadcastData(character);
    PrepareDeltaData(character);
}


void CSerializeCharacterData::PrepareBroadcastData(CCharacter& character)
{
    assert(m_LastCharacterData.EstimateBufferSize(0xFFFFFFFF) + sizeof(unsigned long) * 2 < MAX_CHARACTER_DATA && 
        "CSerializeCharacterData error! - MAX_CHARACTER_DATA �� �÷��ּ���!");

    m_usBroadcastDataLen = 0;    
    char* szDataPos = m_aryBroadcastData;

    // CID ����
    unsigned long dwCID = character.GetCID();
    COPY_AND_ADVANCE_DST(szDataPos, &dwCID, sizeof(unsigned long));

    // UpdateFlag ���� (���� �� ������!)
    unsigned long dwUpdateFlag = 0xFFFFFFFF;    
    COPY_AND_ADVANCE_DST(szDataPos, &dwUpdateFlag, sizeof(unsigned long));

    //! ��� ����
	unsigned long dwEquipments[Broadcast2nd::CCharacterData::HALF_EQUIP_POS];
	memset(dwEquipments, 0, sizeof(dwEquipments));

    character.GetEquipments().GetEquipmentView(
		reinterpret_cast<unsigned short*>(dwEquipments), 0, Item::EquipmentPos::MAX_EQUPMENT_POS);

	if(character.GetGMModelFlag())
	{
		// GM ĳ���� ���� �Ǿ����� ��� //
		unsigned short* wEquip = reinterpret_cast<unsigned short*>(dwEquipments);
		wEquip[0] = Item::GM_MODEL_ITEM;
	}

    const size_t nEquipmentLen = size_t(Broadcast2nd::CCharacterData::HALF_EQUIP_POS * 2);
    COPY_AND_ADVANCE_DST(szDataPos, dwEquipments, sizeof(unsigned short) * nEquipmentLen);

    //! �̸� ����
    const size_t nNameLen = std::min(size_t(CCharacterData::MAX_NAME), size_t(CHAR_INFOST::MAX_NAME_LEN));
    const char* szCharacterName = character.GetCharacterName();
    memcpy(szDataPos, szCharacterName, sizeof(char) * nNameLen);
    szDataPos += nNameLen;

    //! ������ �̸� ����
    const size_t nStallNameLen = 
		std::min(size_t(StallNode::MAX_STALL_NAME), size_t(Item::CCharacterShopContainer::MAX_STALL_NAME_LEN));
    const char* szStallName = character.GetStall().GetStallName();
    memcpy(szDataPos, szStallName, sizeof(char) * nStallNameLen);
    szDataPos += nStallNameLen;

    // ��� ID ����
    unsigned long dwGID			= character.GetGID();
	unsigned long dwTempGID		= 0;	
	
	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( dwGID );
	
	if ( lpGuild )
	{				
		// ���� ����ڴ� ��Ʈ ������ �ؼ� �����ش�.
		if (Guild::JOIN_WAIT == lpGuild->GetTitle( dwCID ))
		{
			dwTempGID = dwGID | Guild::MEMBER_WAIT_BIT;
			COPY_AND_ADVANCE_DST(szDataPos, &dwTempGID, sizeof(unsigned long));			
		}
		else
		{
			COPY_AND_ADVANCE_DST(szDataPos, &dwGID, sizeof(unsigned long));			
		}	
	}
	else
	{
		COPY_AND_ADVANCE_DST(szDataPos, &dwTempGID, sizeof(unsigned long));		
	}
	
	unsigned char cFlag = character.GetTacticsFlag();
	COPY_AND_ADVANCE_DST(szDataPos, &cFlag, sizeof(unsigned char));
	
    // ��Ƽ ID ����
    unsigned long dwPID = character.GetPID();
    COPY_AND_ADVANCE_DST(szDataPos, &dwPID, sizeof(unsigned long));   		

    // �� ����
    unsigned long dwFame = character.GetFame();
    COPY_AND_ADVANCE_DST(szDataPos, &dwFame, sizeof(unsigned long));   

    // ���ϸ��� ����
    unsigned long dwMileage = character.GetMileage();
    COPY_AND_ADVANCE_DST(szDataPos, &dwMileage, sizeof(unsigned long));   

    bcStatusInfo statusInfo;

    statusInfo.m_BodyInfo.m_cHair           = character.GetHair();                  //! �Ӹ�Į
    statusInfo.m_BodyInfo.m_cFace           = character.GetFace();                  //! ��
    statusInfo.m_BodyInfo.m_cNationality    = character.GetNation();                //! ����
    statusInfo.m_BodyInfo.m_cRace           = character.GetRace();                  //! ����
    statusInfo.m_BodyInfo.m_cAdminLevel     = character.GetAdminLevel();            //! ��� ���� ( 0 : �Ϲ�, 1���� ũ�� ���..)
    statusInfo.m_BodyInfo.m_cSex            = character.GetSex() - 1;               //! 0 : ����,     1 : ����
	statusInfo.m_BodyInfo.m_bPeaceMode      = character.IsPeaceMode() ? 1 : 0;		//! 0 : ������, 1 : ��ȭ���
    statusInfo.m_BodyInfo.m_cHandPos        = character.GetHand();                  //! 0 : 1�� ��,   1 : 2�� ��
    statusInfo.m_BodyInfo.m_cRideFlag       = character.GetRide();                  //! 0 : x,   1 : ��Ž
    statusInfo.m_BodyInfo.m_cReserved       = 0;

    statusInfo.m_cClass = static_cast<unsigned char>(character.GetClass());
    statusInfo.m_cLevel = character.GetLevel();

    COPY_AND_ADVANCE_DST(szDataPos, &statusInfo.m_dwBodyInfo, sizeof(unsigned long));   //! �Ż� ���� ����    
    COPY_AND_ADVANCE_DST(szDataPos, &statusInfo.m_cClass, sizeof(unsigned char));       //! Ŭ���� ����    
    COPY_AND_ADVANCE_DST(szDataPos, &statusInfo.m_cLevel, sizeof(unsigned char));       //! ���� ����    

    //! EnchantInfo ���� (chant, enchant flag)
	const EnchantInfo& enchantInfo = character.GetEnchantInfo();
    COPY_AND_ADVANCE_DST(szDataPos, &enchantInfo, sizeof(EnchantInfo));

    //! ��ġ ����
    const Position& characterPos = character.GetCurrentPos();
    CNetworkPos netPos(characterPos.m_fPointX, characterPos.m_fPointY, characterPos.m_fPointZ);
    COPY_AND_ADVANCE_DST(szDataPos, &netPos, sizeof(CNetworkPos));

    //! ���� HP ����
    unsigned short usHP = character.GetStatus().m_nNowHP;
    COPY_AND_ADVANCE_DST(szDataPos, &usHP, sizeof(unsigned short));

	//! ��� �׷��̵� ����
	CNetworkEquipGrade equipGrade = character.GetNetworkEquipGrade();
	COPY_AND_ADVANCE_DST(szDataPos, &equipGrade, sizeof(CNetworkEquipGrade));

	//! �����, ������ ���� �÷��� ����
	WarFlagInfo warFlagInfo;
	warFlagInfo.m_cGuildWarFlag	= character.GetGuildWarFlag();
	warFlagInfo.m_cRealmWarFlag	= character.GetRealmWarFlag();
	COPY_AND_ADVANCE_DST(szDataPos, &warFlagInfo, sizeof(WarFlagInfo));

	// �������� �������� ����Ʈ.
	unsigned char cRealmPoint = character.GetRealmPoint();
	COPY_AND_ADVANCE_DST(szDataPos, &cRealmPoint, sizeof(unsigned char));

    //! ������ ���̸� ���Ѵ�.
    m_usBroadcastDataLen = static_cast<unsigned short>(szDataPos - m_aryBroadcastData);    
}

void CSerializeCharacterData::PrepareDeltaData(CCharacter& character)
{
    assert(m_LastCharacterData.EstimateBufferSize(0xFFFFFFFF) + sizeof(unsigned long) * 2 < MAX_CHARACTER_DATA && 
        "CSerializeCharacterData error! - MAX_CHARACTER_DATA �� �÷��ּ���!");
    
    //! �����͸� ���ؼ� Delta�� ���Ѵ�.    
    m_usDeltaBroadcastDataLen = 0;
    char* szDataPos = m_aryDeltaBroadcastData;

    unsigned long dwCID = character.GetCID();

	unsigned long dwEquipments[Broadcast2nd::CCharacterData::HALF_EQUIP_POS];
	memset(dwEquipments, 0, sizeof(dwEquipments));

    character.GetEquipments().GetEquipmentView(
		reinterpret_cast<unsigned short*>(dwEquipments), 0, Item::EquipmentPos::MAX_EQUPMENT_POS);

	if(character.GetGMModelFlag())
	{
		// GM ĳ���� ���� �Ǿ����� ��� //
		unsigned short* wEquip = reinterpret_cast<unsigned short*>(dwEquipments);
		wEquip[0] = Item::GM_MODEL_ITEM;
	}

    const size_t nNameLen = std::min(size_t(CCharacterData::MAX_NAME), size_t(CHAR_INFOST::MAX_NAME_LEN));
    const char* szCharacterName = character.GetCharacterName();

    const size_t nStallNameLen = 
		std::min(size_t(StallNode::MAX_STALL_NAME), size_t(Item::CCharacterShopContainer::MAX_STALL_NAME_LEN));
    const char* szStallName = character.GetStall().GetStallName();

	unsigned char cRealmPoint = character.GetRealmPoint();

    unsigned long		dwGID			= character.GetGID();
    unsigned long		dwPID			= character.GetPID();
    unsigned long		dwFame			= character.GetFame();
    unsigned long		dwMileage		= character.GetMileage();
	CNetworkEquipGrade	equipGrade		= character.GetNetworkEquipGrade();
	unsigned char		cTacticsFlag	= character.GetTacticsFlag();

	WarFlagInfo warFlagInfo;
	warFlagInfo.m_cGuildWarFlag = character.GetGuildWarFlag();
	warFlagInfo.m_cRealmWarFlag = character.GetRealmWarFlag();

    bcStatusInfo statusInfo;

    statusInfo.m_BodyInfo.m_cHair           = character.GetHair();                  //! �Ӹ�Į
    statusInfo.m_BodyInfo.m_cFace           = character.GetFace();                  //! ��
    statusInfo.m_BodyInfo.m_cNationality    = character.GetNation();                //! ����
    statusInfo.m_BodyInfo.m_cRace           = character.GetRace();                  //! ����
    statusInfo.m_BodyInfo.m_cAdminLevel     = character.GetAdminLevel();            //! ��� ���� ( 0 : �Ϲ�, 1���� ũ�� ���..)
    statusInfo.m_BodyInfo.m_cSex            = character.GetSex() - 1;               //! 0 : ����,     1 : ����
    statusInfo.m_BodyInfo.m_bPeaceMode      = character.IsPeaceMode() ? 1 : 0;		//! 0 : ������, 1 : ��ȭ���
    statusInfo.m_BodyInfo.m_cHandPos        = character.GetHand();                  //! 0 : 1�� ��,   1 : 2�� ��
    statusInfo.m_BodyInfo.m_cRideFlag       = character.GetRide();                  //! 0 : x,   1 : ��Ž
    statusInfo.m_BodyInfo.m_cReserved       = 0;

    statusInfo.m_cClass = static_cast<unsigned char>(character.GetClass());
    statusInfo.m_cLevel = character.GetLevel();

    const EnchantInfo& enchantInfo = character.GetEnchantInfo();
    const Position& characterPos = character.GetCurrentPos();
    CNetworkPos netPos(characterPos.m_fPointX, characterPos.m_fPointY, characterPos.m_fPointZ);
    unsigned short usHP = character.GetStatus().m_nNowHP;


    unsigned long dwDWORD = character.GetCID();
    COPY_AND_ADVANCE_DST(szDataPos, &dwDWORD, sizeof(unsigned long));

    // UpdateFlag�� ���߿� �� �����ϰ�, �������� OR���� �� �Ŀ� �����Ѵ�. �ϴ� ��ġ�� ����� ���´�.
    unsigned long dwDeltaUpdateFlag = 0;
    char* szUpdateFlagCopyPos = szDataPos;
    szDataPos += sizeof(unsigned long);

    //! ��� �� �� ����.
    unsigned long dwLastEquipments[Broadcast2nd::CCharacterData::HALF_EQUIP_POS];
	memset(dwLastEquipments, 0, sizeof(dwLastEquipments));

    m_LastCharacterData.GetEquipmentsID(
		reinterpret_cast<unsigned short*>(dwLastEquipments), Item::EquipmentPos::MAX_EQUPMENT_POS);

	int nCount = 0;

    for (; nCount < Broadcast2nd::CCharacterData::HALF_EQUIP_POS; ++nCount)
    {
		if (dwEquipments[nCount] != dwLastEquipments[nCount])
		{
			COPY_AND_ADVANCE_DST(szDataPos, dwEquipments + nCount, sizeof(unsigned long));
			dwDeltaUpdateFlag |= (1 << nCount);

			m_LastCharacterData.SetEquipmentsID(dwEquipments[nCount], nCount);
		}
    }

    //! �̸� ����.
    if (0 != memcmp(m_LastCharacterData.GetName(), szCharacterName, nNameLen))
    {
        COPY_AND_ADVANCE_DST(szDataPos, szCharacterName, nNameLen);
        dwDeltaUpdateFlag |= CHAR_DELTA_CHAR_NAME;

        m_LastCharacterData.SetName(szCharacterName, nNameLen);
    }

    /*
    ������ �̸��� Delta�� ���� �ʿ��� ����. �߰��� �����ϰų� �����
    StallOpen / StallClose ��Ŷ�� ����.

    //! ������ �̸� ����.
    if (0 != memcmp(m_LastCharacterData.GetStallName(), szStallName, nStallNameLen))
    {
		COPY_AND_ADVANCE_DST(szDataPos, szStallName, nStallNameLen);
		dwDeltaUpdateFlag |= DELTA_STALL_NAME;

		m_LastCharacterData.SetStallName(szStallName, nStallNameLen);
    }
    */

    //! ��� ID����
    if (dwGID != m_LastCharacterData.GetGuildID() || cTacticsFlag != m_LastCharacterData.GetTacticsFlag())
    {
		dwDeltaUpdateFlag |= CHAR_DELTA_GUILD_ID;
		m_LastCharacterData.SetGuildID(dwGID);
		m_LastCharacterData.SetTacticsFlag(cTacticsFlag);

		unsigned long dwTempGID = 0;		

		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( dwGID );

		if ( lpGuild )
		{
			// ���� ����ڴ� ��Ʈ ������ �ؼ� �����ش�.
			if (Guild::JOIN_WAIT == lpGuild->GetTitle( dwCID ))
			{
				dwTempGID = dwGID | Guild::MEMBER_WAIT_BIT;
				COPY_AND_ADVANCE_DST(szDataPos, &dwTempGID, sizeof(unsigned long));
				m_LastCharacterData.SetGuildID(dwTempGID);				
			}
			else
			{
				COPY_AND_ADVANCE_DST(szDataPos, &dwGID, sizeof(unsigned long));
				m_LastCharacterData.SetGuildID(dwGID);				
			}
		}
		else
		{
			COPY_AND_ADVANCE_DST(szDataPos, &dwTempGID, sizeof(unsigned long));
			m_LastCharacterData.SetGuildID(dwTempGID);			
		}

		COPY_AND_ADVANCE_DST(szDataPos, &cTacticsFlag, sizeof(unsigned char));
		m_LastCharacterData.SetTacticsFlag(cTacticsFlag);
	}

    //! ��ƼID ����.
	CParty* lpParty = CPartyMgr::GetInstance().GetParty(dwPID);

    if(dwPID != m_LastCharacterData.GetPartyID())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &dwPID, sizeof(unsigned long));
        m_LastCharacterData.SetPartyID(dwPID);
		
		dwDeltaUpdateFlag |= CHAR_DELTA_PARTY_ID;
    }

    //! �� ����, ! ���ϸ��� ����.
    if (dwFame != m_LastCharacterData.GetFame() || dwMileage != m_LastCharacterData.GetMileage())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &dwFame, sizeof(unsigned long));        
        m_LastCharacterData.SetFame(dwFame);

		COPY_AND_ADVANCE_DST(szDataPos, &dwMileage, sizeof(unsigned long));
		m_LastCharacterData.SetMileage(dwMileage);

		dwDeltaUpdateFlag |= CHAR_DELTA_FAME;
    }
    
    //! �Ż� ���� ����
    if (statusInfo.m_dwBodyInfo != m_LastCharacterData.GetBodyInfo())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &statusInfo.m_dwBodyInfo, sizeof(unsigned long));
        dwDeltaUpdateFlag |= CHAR_DELTA_BODYINFO;

        m_LastCharacterData.SetBodyInfo(statusInfo.m_dwBodyInfo);
    }

    //! Ŭ���� ����
    if (statusInfo.m_cClass != m_LastCharacterData.GetClassType())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &statusInfo.m_cClass, sizeof(unsigned char));
        dwDeltaUpdateFlag |= CHAR_DELTA_CLASS;

        m_LastCharacterData.SetClass(statusInfo.m_cClass);
    }

    //! ���� ����
    if (statusInfo.m_cLevel != m_LastCharacterData.GetLevel())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &statusInfo.m_cLevel, sizeof(unsigned char));
        dwDeltaUpdateFlag |= CHAR_DELTA_LEVEL;

        m_LastCharacterData.SetLevel(statusInfo.m_cLevel);
    }

	//! æƮ/��æƮ ���� ����
	const EnchantInfo& lastEnchantInfo = m_LastCharacterData.GetEnchantInfo();

	for (nCount = 0; nCount < EnchantInfo::MAX_ARRAY; ++nCount)
    {
        if (enchantInfo.m_dwStatusFlag[nCount] != lastEnchantInfo.m_dwStatusFlag[nCount])
        {
            COPY_AND_ADVANCE_DST(szDataPos, enchantInfo.m_dwStatusFlag + nCount, sizeof(unsigned long));
            dwDeltaUpdateFlag |= (CHAR_DELTA_ENCHANT_INFO << nCount);

			m_LastCharacterData.SetEnchantInfo(nCount, enchantInfo.m_dwStatusFlag[nCount]);
        }
    }

    //! ��ġ ���� - �ٸ� ��������� �ְų�, 2ȸ ����(2��..) ���� �ѹ��� ������.
    if ((0 != dwDeltaUpdateFlag || 0 == (m_dwPreparedCount % 2)) 
        && netPos != m_LastCharacterData.GetNetworkPos())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &netPos, sizeof(CNetworkPos));
        dwDeltaUpdateFlag |= CHAR_DELTA_NETWORK_POS;

        m_LastCharacterData.SetNetworkPos(netPos);
    }

    //! ĳ���� HP���� - �ٸ� ��������� �ְų� 2ȸ ����(2��..) ���� �ѹ��� ������.
    if ((0 != dwDeltaUpdateFlag || 0 == (m_dwPreparedCount % 2)) && usHP != m_LastCharacterData.GetHP())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &usHP, sizeof(unsigned short));
        dwDeltaUpdateFlag |= CHAR_DELTA_HP;

        m_LastCharacterData.SetHP(usHP);
    }

	//! ��� �׷��̵� ����
	if (equipGrade != m_LastCharacterData.GetNetworkEquipGrade())
	{
		COPY_AND_ADVANCE_DST(szDataPos, &equipGrade, sizeof(CNetworkEquipGrade));
		dwDeltaUpdateFlag |= CHAR_DELTA_EQUIP_GRADE;

		m_LastCharacterData.SetNetworkEquipGrade(equipGrade);
	}

	//! �����, ������ ���� �÷��� ����
	if (m_LastCharacterData.GetGuildWarFlag() != warFlagInfo.m_cGuildWarFlag ||
		m_LastCharacterData.GetRealmWarFlag() != warFlagInfo.m_cRealmWarFlag)
	{
		COPY_AND_ADVANCE_DST(szDataPos, &warFlagInfo, sizeof(WarFlagInfo));
		dwDeltaUpdateFlag |= CHAR_DELTA_WAR_FLAG;

		m_LastCharacterData.SetGuildWarFlag(warFlagInfo.m_cGuildWarFlag);
		m_LastCharacterData.SetRealmWarFlag(warFlagInfo.m_cRealmWarFlag);
	}

	//! �������� �������� ����Ʈ ����.
	if(m_LastCharacterData.GetRealmPoint() != cRealmPoint)		
	{
		COPY_AND_ADVANCE_DST(szDataPos, &cRealmPoint, sizeof(unsigned char));
		dwDeltaUpdateFlag |= CHAR_DELTA_REALM_POINT;

		m_LastCharacterData.SetRealmPoint(cRealmPoint);		
	}

    //! ���� �Ϸ� �� ������ ���� ���.
    memcpy(szUpdateFlagCopyPos, &dwDeltaUpdateFlag, sizeof(unsigned long));
    m_usDeltaBroadcastDataLen = static_cast<unsigned short>(szDataPos - m_aryDeltaBroadcastData);

    // ä�ü����� ����Ǵ� �����͸� ��Ƽ� ������. - 2�ʿ� �ѹ�
    // m_dwPreparedCount �� ¦���� ����, CID�� ¦���� ��츸 üũ
    // m_dwPreparedCount �� Ȧ���� ����, CID�� Ȧ���� ��츸 üũ
    if ((m_dwPreparedCount % 2) == (dwCID % 2) && 
        m_LastChatData.IsSendLoginData())
    {
        SendDeltaChatData(character);
    }

    ++m_dwPreparedCount;    
}


bool CSerializeCharacterData::SendChatLogin(CCharacter& character)
{    
    GET_SINGLE_DISPATCH(lpChatDispatch, 
        CChatDispatch, CChatDispatch::GetDispatchTable());

    if (0 != lpChatDispatch)
    {
        CSendStream& sendStream = lpChatDispatch->GetSendStream();

        char* szBuffer = sendStream.GetBuffer(sizeof(PktChatData) + MAX_CHARACTER_DATA);

        if (0 != szBuffer)
        {
            PktChatData* lpPktChatData = reinterpret_cast<PktChatData*>(szBuffer);

            const unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

            lpPktChatData->m_dwUID = character.GetUID();
            lpPktChatData->m_dwCID = character.GetCID();
            lpPktChatData->m_cType = PktChatData::LOGIN;

            BattleInclination::CharData charData;
            memset(&charData, 0, sizeof(BattleInclination::CharData));
            BattleInclination::SetCharData(character, charData, true);

            char* szDataPos = szBuffer + sizeof(PktChatData);
            unsigned short usUpdateFlag = 0xFFFF;

            COPY_AND_ADVANCE_DST(szDataPos, &usUpdateFlag, sizeof(unsigned short));

			// edith 2008.02.29 ĳ���� �̸�����
            char szName[CHAR_INFOST::MAX_NAME_LEN];
            strncpy(szName, character.GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
            szName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

            COPY_AND_ADVANCE_DST(szDataPos, szName, CHAR_INFOST::MAX_NAME_LEN);
            m_LastChatData.SetName(szName);
            
            const unsigned long dwPID = character.GetPID();
            COPY_AND_ADVANCE_DST(szDataPos, &dwPID, sizeof(unsigned long));
            m_LastChatData.SetPID(dwPID);			

			unsigned long dwTempGID = 0;

			Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( charData.m_dwGID );

			unsigned char cTacticsFlag = character.GetTacticsFlag();

			if ( lpGuild )
			{
				m_LastChatData.SetGID(charData.m_dwGID);

				// ���� ����ڴ� ��Ʈ ������ �ؼ� �����ش�.
				if (Guild::JOIN_WAIT == lpGuild->GetTitle( character.GetCID() ))
				{
					dwTempGID = charData.m_dwGID | Guild::MEMBER_WAIT_BIT;
					COPY_AND_ADVANCE_DST(szDataPos, &dwTempGID, sizeof(unsigned long));
					m_LastChatData.SetGID(dwTempGID);
				}
				else
				{
					COPY_AND_ADVANCE_DST(szDataPos, &charData.m_dwGID, sizeof(unsigned long));					
				}
			}					
			else
			{
				COPY_AND_ADVANCE_DST(szDataPos, &charData.m_dwGID, sizeof(unsigned long));				
			}

			COPY_AND_ADVANCE_DST(szDataPos, &cTacticsFlag, sizeof(unsigned char));
			m_LastChatData.SetTacticsFlag(cTacticsFlag);

			COPY_AND_ADVANCE_DST(szDataPos, &charData.m_cRealmWarFlag, sizeof(unsigned char));
			m_LastChatData.SetRealmWarFlag(charData.m_cRealmWarFlag);

			COPY_AND_ADVANCE_DST(szDataPos, &charData.m_cGuildWarFlag, sizeof(unsigned char));
			m_LastChatData.SetGuildWarFlag(charData.m_cGuildWarFlag);

            COPY_AND_ADVANCE_DST(szDataPos, &charData.m_dwCastleID, sizeof(unsigned long));
            m_LastChatData.SetCastleID(charData.m_dwCastleID);

            *szDataPos = charData.m_cSiegeState;
            ++szDataPos;

            m_LastChatData.SetSiegeState(charData.m_cSiegeState);

            *szDataPos = charData.m_cNation;
            ++szDataPos;

            m_LastChatData.SetNation(charData.m_cNation);

            *szDataPos = charData.m_cFlags;
            ++szDataPos;

            m_LastChatData.SetFlags(charData.m_cFlags);

            const RejectOption rejectOption = character.GetRejectOption();
            COPY_AND_ADVANCE_DST(szDataPos, &rejectOption, sizeof(RejectOption));
            m_LastChatData.SetRejectOption(rejectOption);

            const unsigned char cAdminLevel = character.GetAdminLevel();
            *szDataPos = cAdminLevel;
            ++szDataPos;

            m_LastChatData.SetAdminLevel(cAdminLevel);

            char szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN];
            strncpy(szAccountName, character.GetAccountName(), CHAR_INFOST::MAX_ACCOUNT_LEN);
            szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN - 1] = 0;

            COPY_AND_ADVANCE_DST(szDataPos, szAccountName, CHAR_INFOST::MAX_ACCOUNT_LEN);
            m_LastChatData.SetAccountName(szAccountName);

			*szDataPos = charData.m_cCreatureType;
			++szDataPos;

			m_LastChatData.SetCreatureType(charData.m_cCreatureType);

			COPY_AND_ADVANCE_DST(szDataPos, &charData.m_wObjectType, sizeof(unsigned short));
			m_LastChatData.SetObjectType(charData.m_wObjectType);

			COPY_AND_ADVANCE_DST(szDataPos, &charData.m_wKind, sizeof(unsigned short));
			m_LastChatData.SetKind(charData.m_wKind);

            if (sendStream.WrapHeader(
                static_cast<unsigned short>(szDataPos - szBuffer), CmdChatInfoChanged, 0, 0))
            {
                m_LastChatData.SetSendLoginData(true);
                return true;
            }
        }
    }

    return false;    
}

bool CSerializeCharacterData::SendChatLogout(CCharacter& character)
{
    GET_SINGLE_DISPATCH(lpChatDispatch, 
        CChatDispatch, CChatDispatch::GetDispatchTable());

    if (0 != lpChatDispatch)
    {
        CSendStream& sendStream = lpChatDispatch->GetSendStream();

        char* szBuffer = sendStream.GetBuffer(sizeof(PktChatData));

        if (0 != szBuffer)
        {
            PktChatData* lpPktChatData = reinterpret_cast<PktChatData*>(szBuffer);

            lpPktChatData->m_dwUID = character.GetUID();
            lpPktChatData->m_dwCID = character.GetCID();            
            lpPktChatData->m_cType = PktChatData::LOGOUT;

            return sendStream.WrapHeader(sizeof(PktChatData), CmdChatInfoChanged, 0, 0);
        }
    }

    return false;
}


void CSerializeCharacterData::SendDeltaChatData(CCharacter& character)
{
    assert(ChatData::CCharInfo::EstimateBufferSize(0xFFFF) + sizeof(unsigned long) * 2 < MAX_CHARACTER_DATA && 
        "MAX_CHARACTER_DATA �� �÷��ּ���!");

    GET_SINGLE_DISPATCH(lpChatDispatch, 
        CChatDispatch, CChatDispatch::GetDispatchTable());

    if (0 != lpChatDispatch)
    {
        // ������ �����.
        char szBuffer[MAX_CHARACTER_DATA + sizeof(PktChatData)];

        char* szDataPos = szBuffer;
        PktChatData* lpPktChatData = reinterpret_cast<PktChatData*>(szDataPos);
        szDataPos += sizeof(PktChatData);

        unsigned short* lpusUpdateFlag = reinterpret_cast<unsigned short*>(szDataPos);
        szDataPos += sizeof(unsigned short);

        unsigned short usUpdateFlag = 0;

        BattleInclination::CharData charData;
        memset(&charData, 0, sizeof(BattleInclination::CharData));
		BattleInclination::SetCharData(character, charData, true);

        if (0 != strncmp(character.GetCharacterName(), 
            m_LastChatData.GetName(), sizeof(char) * CHAR_INFOST::MAX_NAME_LEN))
        { 
            char szName[CHAR_INFOST::MAX_NAME_LEN];
            
            strncpy(szName, character.GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
            szName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

            COPY_AND_ADVANCE_DST(szDataPos, szName, CHAR_INFOST::MAX_NAME_LEN);

            m_LastChatData.SetName(szName);
            usUpdateFlag |= ChatData::DELTA_NAME;
        }

        const unsigned long dwPID = character.GetPID();		

        if (dwPID != m_LastChatData.GetPID())
        {
            COPY_AND_ADVANCE_DST(szDataPos, &dwPID, sizeof(unsigned long));
            m_LastChatData.SetPID(dwPID);			

            usUpdateFlag |= ChatData::DELTA_PID;
        }
        
        if (charData.m_dwGID != m_LastChatData.GetGID() || charData.m_cTacticsFlag != m_LastChatData.GetTacticsFlag())
        {
			usUpdateFlag |= ChatData::DELTA_GID;

			unsigned long dwTempGID = 0;
			unsigned char cTempTacticsFlag = 0;
							
			Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( charData.m_dwGID );
			if ( lpGuild )
			{
				m_LastChatData.SetGID(charData.m_dwGID);
				
				unsigned char cTacticsFlag = character.GetTacticsFlag();

				// ���� ����ڴ� ��Ʈ ������ �ؼ� �����ش�.
				if (Guild::JOIN_WAIT == lpGuild->GetTitle( character.GetCID() ))
				{
					dwTempGID = charData.m_dwGID | Guild::MEMBER_WAIT_BIT;
					COPY_AND_ADVANCE_DST(szDataPos, &dwTempGID, sizeof(unsigned long));
					m_LastChatData.SetGID(dwTempGID);
				}
				else
				{
					COPY_AND_ADVANCE_DST(szDataPos, &charData.m_dwGID, sizeof(unsigned long));						
				}

				COPY_AND_ADVANCE_DST(szDataPos, &cTacticsFlag, sizeof(unsigned char));
				m_LastChatData.SetTacticsFlag(cTacticsFlag);
			}
			else
			{
				COPY_AND_ADVANCE_DST(szDataPos, &dwTempGID, sizeof(unsigned long));
				m_LastChatData.SetGID(dwTempGID);

				COPY_AND_ADVANCE_DST(szDataPos, &cTempTacticsFlag, sizeof(unsigned char));
				m_LastChatData.SetTacticsFlag(cTempTacticsFlag);
			}
        }
        
		if(charData.m_cRealmWarFlag!=m_LastChatData.GetRealmWarFlag() || charData.m_cGuildWarFlag!=m_LastChatData.GetGuildWarFlag())
		{
			usUpdateFlag |= ChatData::DELTA_WARFLAG;
			
			COPY_AND_ADVANCE_DST(szDataPos, &charData.m_cRealmWarFlag, sizeof(unsigned char));
			m_LastChatData.SetRealmWarFlag(charData.m_cRealmWarFlag);

			COPY_AND_ADVANCE_DST(szDataPos, &charData.m_cGuildWarFlag, sizeof(unsigned char));
			m_LastChatData.SetGuildWarFlag(charData.m_cGuildWarFlag);
		}

        if (charData.m_dwCastleID != m_LastChatData.GetCastleID())
        {
            COPY_AND_ADVANCE_DST(szDataPos, &charData.m_dwCastleID, sizeof(unsigned long));
            m_LastChatData.SetCastleID(charData.m_dwCastleID);

            usUpdateFlag |= ChatData::DELTA_CASTLEID;
        }

        if (charData.m_cSiegeState != m_LastChatData.GetSiegeState())
        {
            *szDataPos = charData.m_cSiegeState;
            ++szDataPos;

            m_LastChatData.SetSiegeState(charData.m_cSiegeState);

            usUpdateFlag |= ChatData::DELTA_SIEGESTATE;
        }

        if (charData.m_cNation != m_LastChatData.GetNation())
        {
            *szDataPos = charData.m_cNation;
            ++szDataPos;

            m_LastChatData.SetNation(charData.m_cNation);

            usUpdateFlag |= ChatData::DELTA_NATION;
        }

        if (charData.m_cFlags != m_LastChatData.GetFlags())
        {
            *szDataPos = charData.m_cFlags;
            ++szDataPos;

            m_LastChatData.SetFlags(charData.m_cFlags);

            usUpdateFlag |= ChatData::DELTA_ETCFLAGS;
        }

        const RejectOption rejectOption = character.GetRejectOption();
        if (0 != memcmp(&rejectOption, &m_LastChatData.GetRejectOption(), sizeof(RejectOption)))
        {
            COPY_AND_ADVANCE_DST(szDataPos, &rejectOption, sizeof(RejectOption));
            m_LastChatData.SetRejectOption(rejectOption);

            usUpdateFlag |= ChatData::DELTA_REJECTOPTION;
        }

        const unsigned char cAdminLevel = character.GetAdminLevel();
        if (cAdminLevel != m_LastChatData.GetAdminLevel())
        {
            *szDataPos = cAdminLevel;
            ++szDataPos;

            m_LastChatData.SetAdminLevel(cAdminLevel);

            usUpdateFlag |= ChatData::DELTA_ADMINLEVEL;
        }		

		if(charData.m_cCreatureType!=m_LastChatData.GetCreatureType())
		{
			*szDataPos = charData.m_cCreatureType;
			++szDataPos;

			m_LastChatData.SetCreatureType(charData.m_cCreatureType);

			usUpdateFlag |= ChatData::DELTA_CREATURETYPE;			
		}		

		if(charData.m_wObjectType!=m_LastChatData.GetObjectType())
		{
			COPY_AND_ADVANCE_DST(szDataPos, &charData.m_wObjectType, sizeof(unsigned short));
			m_LastChatData.SetObjectType(charData.m_wObjectType);

			usUpdateFlag |= ChatData::DELTA_OBJECTTYPE;			
		}
		
		if(charData.m_wKind!=m_LastChatData.GetKind())
		{
			COPY_AND_ADVANCE_DST(szDataPos, &charData.m_wKind, sizeof(unsigned short));
			m_LastChatData.SetKind(charData.m_wKind);

			usUpdateFlag |= ChatData::DELTA_KID;
		}		

        *lpusUpdateFlag = usUpdateFlag;

        if (0 != usUpdateFlag)
        {
            lpPktChatData->m_dwUID = character.GetUID();
            lpPktChatData->m_dwCID = character.GetCID();
            lpPktChatData->m_cType = PktChatData::DELTA;

            lpChatDispatch->GetSendStream().WrapCompress(szBuffer, 
                static_cast<unsigned short>(szDataPos - szBuffer), CmdChatInfoChanged, 0, 0);
        }
    }
}

CSerializeMonsterData::CSerializeMonsterData()
: m_dwPreparedCount(0)
, m_usBroadcastDataLen(0)
, m_usDeltaBroadcastDataLen(0)
{

}

CSerializeMonsterData::~CSerializeMonsterData()
{

}

//! �ʱ� �����͸� �����Ѵ�.
void CSerializeMonsterData::InitializeData(CMonster& monster)
{
    const Position& monsterPos = monster.GetCurrentPos();
        
    m_LastMonsterData.SetEnchantInfo(monster.GetEnchantInfo());

    m_LastMonsterData.SetNetworkPos(
        CNetworkPos(monsterPos.m_fPointX, monsterPos.m_fPointY, monsterPos.m_fPointZ));
    
    m_LastMonsterData.SetHP(monster.GetStatus().m_nNowHP);

    PrepareData(monster);
}

//! �����͸� �����ϰ�, �غ��ϰ�, Delta�� ���Ѵ�.
void CSerializeMonsterData::PrepareData(CMonster& monster)
{
    PrepareBroadcastData(monster);
    PrepareDeltaData(monster);
}
       
void CSerializeMonsterData::PrepareBroadcastData(CMonster& monster)
{
    assert(m_LastMonsterData.EstimateBufferSize(0xFF) + 
        sizeof(unsigned long) + sizeof(unsigned char) < MAX_MONSTER_DATA && 
        "CSerializeMonsterData error! - MAX_MONSTER_DATA �� �÷��ּ���!");

    m_usBroadcastDataLen = 0;    
    char* szDataPos = m_aryBroadcastData;

    // CID ����
    unsigned long dwCID = monster.GetCID();
    COPY_AND_ADVANCE_DST(szDataPos, &dwCID, sizeof(unsigned long));

    // UpdateFlag ��ġ ����.
    unsigned char  cUpdateFlag = 0;
    unsigned char* lpUpdateFlagPos = 
        reinterpret_cast<unsigned char*>(szDataPos);

    ++szDataPos;

    //! EnchantInfo ���� (chant, enchant flag)
	const EnchantInfo& enchantInfo = monster.GetEnchantInfo();
	for (int nCount = 0; nCount < EnchantInfo::MAX_ARRAY; ++nCount)
    {
        COPY_AND_ADVANCE_DST(szDataPos, enchantInfo.m_dwStatusFlag + nCount, sizeof(unsigned long));
        cUpdateFlag |= (MON_DELTA_ENCHANT_INFO << nCount);
    }

    //! ��ġ ����
    const Position& monsterPos = monster.GetCurrentPos();
    CNetworkPos netPos(monsterPos.m_fPointX, monsterPos.m_fPointY, monsterPos.m_fPointZ);
    COPY_AND_ADVANCE_DST(szDataPos, &netPos, sizeof(CNetworkPos));
    cUpdateFlag |= MON_DELTA_NETWORK_POS;

    //! ���� HP ����
    unsigned short usHP = monster.GetStatus().m_nNowHP;
    COPY_AND_ADVANCE_DST(szDataPos, &usHP, sizeof(unsigned short));
    cUpdateFlag |= MON_DELTA_HP;

    //! ���� ���� ID����
    CCharacter* lpMaster = 0;
    
    unsigned long dwMasterCID =
        (Creature::CT_SUMMON == Creature::GetCreatureType(monster.GetCID()) &&
        0 != (lpMaster = static_cast<CSummonMonster&>(monster).GetMaster()))
        ? lpMaster->GetCID() : 0;

    if (0 != dwMasterCID)
    {
        COPY_AND_ADVANCE_DST(szDataPos, &dwMasterCID, sizeof(unsigned long));
        cUpdateFlag |= MON_DELTA_MASTERCID;
    }

    *lpUpdateFlagPos = cUpdateFlag;
    
    //! ������ ���̸� ���Ѵ�.
    m_usBroadcastDataLen = static_cast<unsigned short>(szDataPos - m_aryBroadcastData);
}

void CSerializeMonsterData::PrepareDeltaData(CMonster& monster)
{
    assert(m_LastMonsterData.EstimateBufferSize(0xFF) + 
        sizeof(unsigned long) + sizeof(unsigned char) < MAX_MONSTER_DATA && 
        "CSerializeMonsterData error! - MAX_MONSTER_DATA �� �÷��ּ���!");
    
    //! �����͸� ���ؼ� Delta�� ���Ѵ�.    
    m_usDeltaBroadcastDataLen = 0;
    char* szDataPos = m_aryDeltaBroadcastData;

    unsigned long dwCID = monster.GetCID();

    const EnchantInfo& enchantInfo = monster.GetEnchantInfo();
    const Position& monsterPos = monster.GetCurrentPos();
    CNetworkPos netPos(monsterPos.m_fPointX, monsterPos.m_fPointY, monsterPos.m_fPointZ);
    unsigned short usHP = monster.GetStatus().m_nNowHP;

    unsigned long dwDWORD = monster.GetCID();
    COPY_AND_ADVANCE_DST(szDataPos, &dwDWORD, sizeof(unsigned long));

    // UpdateFlag�� ���߿� �� �����ϰ�, �������� OR���� �� �Ŀ� �����Ѵ�. �ϴ� ��ġ�� ����� ���´�.
    unsigned char cDeltaUpdateFlag = 0;
    char* szUpdateFlagCopyPos = szDataPos;
    szDataPos += sizeof(unsigned char);

    //! æƮ/��æƮ ���� ����
	const EnchantInfo& lastEnchantInfo = m_LastMonsterData.GetEnchantInfo();

	for (int nCount = 0; nCount < EnchantInfo::MAX_ARRAY; ++nCount)
    {
        if (enchantInfo.m_dwStatusFlag[nCount] != lastEnchantInfo.m_dwStatusFlag[nCount])
        {
            COPY_AND_ADVANCE_DST(szDataPos, enchantInfo.m_dwStatusFlag + nCount, sizeof(unsigned long));
            cDeltaUpdateFlag |= (MON_DELTA_ENCHANT_INFO << nCount);

			m_LastMonsterData.SetEnchantInfo(nCount, enchantInfo.m_dwStatusFlag[nCount]);
        }
    }

    //! ��ġ ���� - �ٸ� ��������� �ְų�, 2ȸ ����(2��..) ���� �ѹ��� ������.
    if ((0 != cDeltaUpdateFlag || 0 == (m_dwPreparedCount % 2)) && 
        netPos != m_LastMonsterData.GetNetworkPos())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &netPos, sizeof(CNetworkPos));
        cDeltaUpdateFlag |= MON_DELTA_NETWORK_POS;

        m_LastMonsterData.SetNetworkPos(netPos);
    }

    //! ĳ���� HP���� - �ٸ� ��������� �ְų� 2ȸ ����(2��..) ���� �ѹ��� ������.
    if ((0 != cDeltaUpdateFlag || 0 == (m_dwPreparedCount % 2)) && 
        usHP != m_LastMonsterData.GetHP())
    {
        COPY_AND_ADVANCE_DST(szDataPos, &usHP, sizeof(unsigned short));
        cDeltaUpdateFlag |= MON_DELTA_HP;

        m_LastMonsterData.SetHP(usHP);
    }

    //! ���� �Ϸ� �� ������ ���� ���.
    *szUpdateFlagCopyPos = cDeltaUpdateFlag;
    m_usDeltaBroadcastDataLen = 
        static_cast<unsigned short>(szDataPos - m_aryDeltaBroadcastData);

    ++m_dwPreparedCount;
}

}
