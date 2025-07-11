#ifndef _CHARACTER_RESPAWN_MANAGER_H_
#define _CHARACTER_RESPAWN_MANAGER_H_

#pragma once

#include <Creature/CreatureStructure.h>

class CCharRespawnMgr
{
public:
	enum Const
	{
		RESPAWN_DELAY_SEC	= 5,

		SEC_PER_HOUR		= 3600,
		SEC_PER_MIN			= 60,
		MIN_PER_HOUR		= 60
	};

	enum eReturn
	{
		RET_FALSE			= 0,
		RET_TRUE			= 1,
		RET_TOWN			= 2
	};

	enum RespawnType
	{
		RST_TOWN			= 0,	// �װų� ��Ż �̿����� �������� ��Ȱ (�Ϲ����� ������)
		RST_FORTRESS		= 1,	// �װų� ��Ż �̿����� ����� �� �� ��ũ ī���� ���󿡼� ��Ȱ
		RST_RESURRECTION	= 2,	// ������������ ��Ȱ
		RST_BATTLE			= 3		// ��Ʋ �׶��峪 ��Ʋ ���ѿ��� ������ ��
	};

	struct RespawnProtoType
	{
		unsigned long			m_dwTownID;			// ����, ��� ���, �� ID, ��ũ ī���� ���� ID(0x800x0000)
		unsigned long			m_dwGID;			// ��� ���
		unsigned char			m_cNation;			// ���� (�޸�, ��ĭ, ��3����)
		unsigned char			m_cZone;			// ��
		std::vector<Position>	m_RespawnPosList;	// ������ ��ġ
	};

	struct RespawnCharInfo
	{
		unsigned long	m_dwCID;
		unsigned long	m_dwRemainSec;

		RespawnCharInfo(unsigned long dwCID, unsigned long dwSec)
			: m_dwCID(dwCID), m_dwRemainSec(dwSec) { }
	};

	typedef std::list<RespawnCharInfo>					RespawnCharList;
	typedef std::map<unsigned long, RespawnProtoType>	RespawnProtoTypeMap;
	typedef std::map<unsigned long, RespawnCharList>	RespawnCharMap;
	
	~CCharRespawnMgr();

	static CCharRespawnMgr&	GetInstance();

	// ----------------------------------------------------------------------------
	// ������ ��ũ��Ʈ (RespawnScript.txt)
	// ��ũ��Ʈ�� 0x40001000 �� Castle::CASTLE_BIT | (���� NameID << Castle::CASTLE_NAME_BIT_SHIFT) �Դϴ�.
	// ���Ŀ� �� ������ DB�߰� �����κ��� �޾ƿͼ� Map ������ ����ε� CastleID �� key �� ���ϴ�.
	bool LoadRespawnFromFile(const char* szFileName = 0);

	
	// ��� ������ ����Ʈ ���
	bool AddCampRespawnPoint(unsigned long dwCampID, unsigned long dwGID, const Position& RespawnPos);
	bool DeleteCampRespawnPoint(unsigned long dwCampID);


	// ��ũ ī���� ���� ������ ����Ʈ ���
	// TownID �� 0x80010000 ó�� Castle::STATUE_BIT | (���� Index) << 16) �Դϴ�.
	bool AddRealmStatueRespawnPoint(unsigned long dwIndex, unsigned char cNation, const Position& RespawnPos);
	bool DeleteRealmStatueRespawnPoint(unsigned long dwIndex);


	// �� ������ ����Ʈ ���
	// ������ �� ������ ����Ʈ Ű   : CASTLE_BIT | (NameID << Castle::CASTLE_NAME_BIT_SHIFT)
	// ������ �ӽ� ������ ����Ʈ Ű : CASTLE_BIT | SIEGE_BIT | (NameID << Castle::CASTLE_NAME_BIT_SHIFT) | ID(������)
	bool SetCastleRespawnPointNation(unsigned long dwCastleNameID, unsigned char cNation);

	
	eReturn AddCharacter(unsigned long dwTownID, unsigned long dwCID);
//	void DelCharacter(unsigned long dwTownID, unsigned long dwCID);

	void ProcessRespawn();

	const Position GetDefaultRespawnPos(unsigned char cNation);
	const Position GetRespawnPos(unsigned long dwTownID);
	const Position GetTownRespawnPos(unsigned char cZone, unsigned char cNation);
	
	// ----------------------------------------------------------------------------
	// Send �Լ�
	bool SendRespawnInfo(unsigned long dwCID);
	bool SendRespawnAreaInfo(unsigned long dwCID, unsigned long dwTownID, bool bCount=false);
	
	
private:
	CCharRespawnMgr();

	RespawnProtoTypeMap		m_ProtoTypeList;	// ���� ���� ������ ����Ʈ ����
	RespawnProtoTypeMap		m_AllZoneTownList;	// �� ���� ���� ������ ����Ʈ ����
	RespawnCharMap			m_CharList;
    
	static const char*		ms_szRespawnScriptFileName;
};

#endif
