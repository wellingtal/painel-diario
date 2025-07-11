#include "stdafx.h"

#include <fstream>

#include <Utility/Math/Math.h>
#include <Utility/Compress/MiniLZO/MiniLZOWrapper.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>

#include <Creature/Monster/PatternMonster.h>
#include <Creature/Monster/VirtualMonsterMgr.h>
#include <Creature/NPC/NPC.h>

#include <Quest/QuestMgr.h>

#include <Community/Party/PartyMgr.h>

#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include "CellManager.h"


CCellManager::CCellManager(void)
:	m_CellData(0), m_wNumMoving(0), m_usSummonCount(0), 
	m_bMoving(false), m_bAvoid(false), m_bLoadComplete(false)
{
}

CCellManager::~CCellManager(void)
{
    Destroy();
}

void CCellManager::Destroy()
{
    for (vector<SafetyZoneInfo *>::iterator infoItr = m_vecSafetyZone.begin(); 
        infoItr != m_vecSafetyZone.end(); ++infoItr)
    {
        SafetyZoneInfo* lpInfo = *infoItr;
        delete lpInfo;
        lpInfo = NULL;
    }
    m_vecSafetyZone.clear();

	if (NULL != m_WeatherRate)
	{
		delete [] m_WeatherRate;
		m_WeatherRate = NULL;
	}

    if (NULL != m_CellData)
    {
        delete [] m_CellData;
        m_CellData = NULL;
    }
}


CCellManager& CCellManager::GetInstance()
{
	static CCellManager cellManager;
	return cellManager;
}

bool CCellManager::CheckPositionInZone(Position Pos)
{
	const Position ZoneMinPos[SERVER_ID::MAX_ZONE_NUM] =
	{
		Position(0, 0, 0),			// NONE
		//Position(473, 0, 184),		// ZONE1 (�׷��� �ڽ�Ʈ)
		Position(325, 0, 325),		// ZONE12 (���� ���)
		Position(315, 0, 315),		// ZONE2 (���� ����)
		Position(591, 0, 575),		// ZONE3
		Position(484, 0, 452),		// ZONE4 (���̳� ���Ϸ���)
		Position(450, 0, 450),		// ZONE5 (���Ϸ��� ���� ����)
		Position(591, 0, 575),		// BATTLE_ZONE (��� ����)
		Position(0, 0, 0),			// ADMIN_TOOL_ZONE
		Position(630, 0, 630),		// CAPITAL 
		Position(630, 0, 630),		// ZONE9
		Position(945, 0, 945),		// BATTLE_SERVER::FRAG_ZONE
		Position(1574, 0, 954),		// BATTLE_SERVER
		Position(325, 0, 325),		// ZONE12 (���� ���)
		Position(645, 0, 641),		// BATTLE_SERVER::STATUE_ZONE
		Position(3090, 0, 3170),	// ���� (���� �ý���)
		Position(0, 0, 0),			// CHAR_SELECT (ĳ���� ���� ȭ��)
		Position(880, 0, 890),		// STONE_WAR1 
		Position(880, 0, 890),		// STONE_WAR2
		Position(880, 0, 890)		// STONE_WAR3 
	};

	const Position ZoneMaxPos[SERVER_ID::MAX_ZONE_NUM] =
	{
		Position(0, 0, 0),			// NONE
		//Position(3387, 0, 3064),	// ZONE1 (�׷��� �ڽ�Ʈ)
		Position(3769, 0, 3774),	// ZONE12 (���� ���)
		Position(3781, 0, 3781),	// ZONE2 (���� ����)
		Position(2227, 0, 2250),	// ZONE3
		Position(2705, 0, 2706),	// ZONE4 (���̳� ���Ϸ���)
		Position(3326, 0, 3347),	// ZONE5 (���Ϸ��� ���� ����)
		Position(2227, 0, 2250),	// BATTLE_ZONE (��� ����)
		Position(0, 0, 0),			// ADMIN_TOOL_ZONE
		Position(3465, 0, 3465),	// CAPITAL 
		Position(3780, 0, 3780),	// ZONE9
		Position(1259, 0, 1237),	// BATTLE_SERVER::FRAG_ZONE
		Position(2250, 0, 1884),	// BATTLE_SERVER   // was  Position(2250, 0, 1884),
		Position(3769, 0, 3774),	// ZONE12 (���� ���)
		Position(1523, 0, 1529),	// BATTLE_SERVER::STATUE_ZONE
		Position(3360, 0, 3250),	// ���� (���� �ý���)
		Position(0, 0, 0),			// CHAR_SELECT (ĳ���� ���� ȭ��)
		Position(1970, 0, 1980),	// STONE_WAR1
		Position(1970, 0, 1980),	// STONE_WAR2
		Position(1970, 0, 1980)		// STONE_WAR3
	};

	char cZoneNumber = CServerSetup::GetInstance().GetServerZone();
	return (Pos.m_fPointX >= ZoneMinPos[cZoneNumber].m_fPointX && Pos.m_fPointX <= ZoneMaxPos[cZoneNumber].m_fPointX && 
		Pos.m_fPointZ >= ZoneMinPos[cZoneNumber].m_fPointZ && Pos.m_fPointZ <= ZoneMaxPos[cZoneNumber].m_fPointZ) ? true : false;
}

void CCellManager::Load(void)
{
	// ���� : NPC�� ��ũ�� �Ŵ� ���͵�(��ȯ �Ϸ�� ���� ����)�� �����Ƿ� NPC�� ���� �ε��Ǿ�� �Ѵ�.
	//		  NPC�� Quest�� ����ϱ� ������ Quest�� ���� �ε��ؾ� �Ѵ�.

	DETLOG0(g_Log, "Quest ������ �ε��մϴ�.");
	CQuestMgr::LoadQuestInfo();

	DETLOG0(g_Log, "NPC ������ �ε��մϴ�.");
	CNPC::LoadNPCInfo();

	DETLOG0(g_Log, "���� ������ �ε��մϴ�.");
	CMonsterMgr::GetInstance().LoadMonstersFromFile();
	InitAI();

	DETLOG0(g_Log, "VirtualArea ������ �ε��մϴ�.");
	VirtualArea::CVirtualAreaMgr::GetInstance().LoadVirtualAreaProtoType();
	
	DETLOG0(g_Log, "�� ���� �ʱ�ȭ�մϴ�.");
	CreateCell();

	DETLOG0(g_Log, "�� ���� ������ �ʱ�ȭ�մϴ�.");
	char szWeather[MAX_PATH];
	sprintf(szWeather, "./Script/Game/Weather/Weather%d.bmp", CServerSetup::GetInstance().GetServerZone());
	SetWeather(szWeather);

	DETLOG0(g_Log, "���͸� �α��ŵ�ϴ�.");
	char szArrangement[MAX_PATH];
	sprintf(szArrangement, "./Script/Game/Arrangement/Arrangement%d.txt", CServerSetup::GetInstance().GetServerZone());
	LoginMonster(szArrangement);

	DETLOG0(g_Log, "������ ������Ƽ ���� �����մϴ�.");
	char szSafetyZone[MAX_PATH];
	sprintf(szSafetyZone, "./Script/Game/SafetyZone/Zone%d.bgm", CServerSetup::GetInstance().GetServerZone());
	LoadSafetyZone(szSafetyZone);

	m_bLoadComplete = true;
}

bool CCellManager::InitAI(void)
{
	const int StateNum = 3;
	const int TriggerNum = 4;

	const int aryStateIDandTriggerNum[StateNum * 2] = {	
		STATE_ID_NORMAL, 4,
		STATE_ID_ATTACK, 3,
		STATE_ID_RETURN, 3,
	};
	
	const int aryTriggerandNextState[StateNum][TriggerNum * 2] = { 
		// STATE_ID_NOMAL
		{	
			INPUT_ID_SEEN_PLAYER,		STATE_ID_ATTACK,
			INPUT_ID_ATTACKED_PLAYER,	STATE_ID_ATTACK,
			INPUT_ID_ZERO_HP,			STATE_ID_DIE,		

			INPUT_ID_CMD_ATTACK,		STATE_ID_ATTACK,
		},

		// STATE_ID_ATTACK
		{	
			INPUT_ID_LEAVE_PLAYER,		STATE_ID_RETURN,
			INPUT_ID_ZERO_HP,			STATE_ID_DIE,	

			INPUT_ID_CMD_ESCAPE,		STATE_ID_RETURN,

			STATE_ID_NULL,				STATE_ID_NULL
		},

		// STATE_ID_RETURN
		{	
			INPUT_ID_ARRIVAL_SITE,		STATE_ID_NORMAL,
			INPUT_ID_ATTACKED_PLAYER,	STATE_ID_ATTACK,
			INPUT_ID_ZERO_HP,			STATE_ID_DIE, 	

			STATE_ID_NULL,				STATE_ID_NULL
		}
	};

	int nIndexState;
	int nTriggerIndex, nTransitionIndex;
	
	for (nTriggerIndex = 0, nIndexState = 0; nIndexState < StateNum; nTriggerIndex += 2, ++nIndexState)
	{
        CFSMState* lpFSMstate = new CFSMState(aryStateIDandTriggerNum[nTriggerIndex], 
            aryStateIDandTriggerNum[nTriggerIndex + 1]);

		if (0 == lpFSMstate) 
        {
			ERRLOG0(g_Log, "�޸� �Ҵ翡 �����Ͽ����ϴ�.");
			return false;
		}

		for (nTransitionIndex = 0;
			nTransitionIndex < aryStateIDandTriggerNum[nTriggerIndex + 1] * 2; nTransitionIndex += 2) 
		{
			lpFSMstate->AddTransition(aryTriggerandNextState[nIndexState][nTransitionIndex], 
				aryTriggerandNextState[nIndexState][nTransitionIndex + 1]);
		}

		if (false == CFSM::GetInstance().AddState(lpFSMstate)) 
        {
			ERRLOG0(g_Log, "State�� �ʹ� �����ϴ�.");
			delete lpFSMstate;
			break;
		}
	}

	return true;
}

bool CCellManager::CreateCell(void)
{
	int nX = 0;
	int nZ = 0;

	// ��Ʋ �׶��� ���� �������� ��� BGServerMap �� Cell ���� �����ؾ� �Ѵ�.
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		CCell::ms_CellSize = CCell::CELL_SIZE_BATTLE_SERVER;

		// BGServerMap �� ���� �����Ѵ�.
		VirtualArea::CVirtualAreaMgr::GetInstance().CreateBGServer();
	}

	m_CellData = new CCell[CCell::ms_CellSize * CCell::ms_CellSize];
	if (NULL == m_CellData) 
	{
		ERRLOG0(g_Log, "���� �Ҵ��ϴµ� �����߽��ϴ�.");
		return false;
	}

    CCell* lpCell       = m_CellData;
    CCell* lpUpperCell  = 0;
    CCell* lpLowerCell  = 0;

    const int nCellSize = CCell::ms_CellSize;

	// Make Cell Link
	for (nZ = 0; nZ < nCellSize; ++nZ) 
	{
		for (nX = 0; nX < nCellSize; ++nX, ++lpCell) 
		{
			// UP, UpperLeft, UpperRight
			if (nZ > 0)	
			{
                lpUpperCell = lpCell - nCellSize;

				lpCell->SetConnectCell(CCell::UP, lpUpperCell);

				if (nX > 0)
				{
                    lpCell->SetConnectCell(CCell::UPPERLEFT, lpUpperCell - 1);
				}
			
				if (nX < CCell::ms_CellSize-1)
				{
					lpCell->SetConnectCell(CCell::UPPERRIGHT, lpUpperCell + 1);
				}
			}

			// Down, DnLeft, DnRight
			if (nZ < nCellSize - 1)		
			{
                lpLowerCell = lpCell + nCellSize;

				lpCell->SetConnectCell(CCell::DOWN, lpLowerCell);

				if (nX > 0)
				{
                    lpCell->SetConnectCell(CCell::LOWERLEFT, lpLowerCell - 1);
				}

				if (nX < CCell::ms_CellSize - 1)
				{
					lpCell->SetConnectCell(CCell::LOWERRIGHT, lpLowerCell + 1);
				}
			}

			// Left
			if (nX > 0)
			{
				lpCell->SetConnectCell(CCell::LEFT, lpCell - 1);
			}

			// Right
			if (nX < CCell::ms_CellSize - 1)	
			{
                lpCell->SetConnectCell(CCell::RIGHT, lpCell + 1);			
			}

			// itself
			lpCell->SetConnectCell(CCell::NO, lpCell);
		}
	}

    // �� ��ũ�� ���� ���� �ʱ�ȭ�� �ؾ� �Ѵ�.
    lpCell = m_CellData;
    for (nZ = 0; nZ < nCellSize; ++nZ)
    {
        for (nX = 0; nX < nCellSize; ++nX, ++lpCell) 
        {
            lpCell->Initialize(nX, nZ);
        }
    }

    return true;
}

bool CCellManager::SetWeather(const char* szFileName)
{
	FILE *fpFile = fopen(szFileName, "rb");
	if (NULL == fpFile)
	{
		ERRLOG1(g_Log, "%s ������ �� �� �����ϴ�.", szFileName);
		return false;
	}

	BITMAPFILEHEADER bitmapHeader;
	if (NULL == fread(&bitmapHeader, sizeof(BITMAPFILEHEADER), 1, fpFile))
	{
		ERRLOG1(g_Log, "%s ������ ����� �� �� �����ϴ�.", szFileName);
		return false;
	}

	BITMAPINFOHEADER bitmapInfoHeader;  
	if (NULL == fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fpFile))
	{
		ERRLOG1(g_Log, "%s ������ ���� ����� �� �� �����ϴ�.", szFileName);
		return false;
	}

	int nWidth  = bitmapInfoHeader.biWidth;
	int nHeight = abs(bitmapInfoHeader.biHeight);

	// 4����Ʈ ������ �Ǿ� �����Ƿ�...
	int nRealWidth = (nWidth * (bitmapInfoHeader.biBitCount / 8) + 3) & ~3;  

	m_WeatherRate = new unsigned char[nHeight * nRealWidth];

	// �̹����� �Ųٷ� �Ǿ� �����Ƿ� �޺κк��� �д´�.
	for (int y = nHeight - 1; y > -1; --y)
	{
		fread(m_WeatherRate + (y * nRealWidth), nRealWidth,  1, fpFile);
	}

	fclose(fpFile);
	return true;
}

bool CCellManager::LoginMonster(const char* szFileName, unsigned short wMapIndex)
{
	CDelimitedFile DelimitedFile;
	if (false == DelimitedFile.Open(szFileName, 1)) 
	{
		ERRLOG1(g_Log, "%s ������ �� �� �����ϴ�.", szFileName);
		return false;
	}

    char strTemp[MAX_PATH];
	int iLine = 0;
    CMonster::MonsterCreateInfo tempInfo;

    CMonster*           lpNewMonster    = NULL;
    CCreatureManager&   CreatureManager = CCreatureManager::GetInstance();

	while (DelimitedFile.ReadLine()) 
	{		
		++iLine;

		DelimitedFile.ReadString(strTemp, MAX_PATH);
        tempInfo.m_dwCID = Math::Convert::Atoi(strTemp);

		DelimitedFile.ReadData(tempInfo.m_nKID);
		
		DelimitedFile.ReadString(strTemp, MAX_PATH);
		tempInfo.m_dwPID = Math::Convert::Atoi(strTemp);
		
		DelimitedFile.ReadData(tempInfo.m_Pos.m_fPointX);
		DelimitedFile.ReadData(tempInfo.m_Pos.m_fPointY);
		DelimitedFile.ReadData(tempInfo.m_Pos.m_fPointZ);
		DelimitedFile.ReadData(tempInfo.m_bScout);
		DelimitedFile.ReadData(tempInfo.m_nMovingPattern);
		DelimitedFile.ReadData(tempInfo.m_wRespawnArea);
		
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(tempInfo.m_dwCID);
		if (Creature::CT_MONSTER != eCreatureType &&
			Creature::CT_SUMMON != eCreatureType &&
			Creature::CT_STRUCT != eCreatureType)
		{
			ERRLOG2(g_Log, "�߸��� CID(%d)�� ���� ���͸� �����Ϸ� �մϴ�. Line %d", tempInfo.m_dwCID, iLine);
			continue;
		}

		if ((tempInfo.m_dwCID & Creature::MONSTER_KIND_BIT) != tempInfo.m_nKID) 
        {
			ERRLOG2(g_Log, "�߸��� KID(%d)�� ���� ���͸� �����Ϸ� �մϴ�(CID(%d) ������ Ʋ���ϴ�).", tempInfo.m_nKID, tempInfo.m_dwCID);
			continue;
		}

		switch (tempInfo.m_nKID)
		{
			// 1����
			case MonsterInfo::STATUE_NEUTRALITY1:
			case MonsterInfo::STATUE_HUMAN_LOADING1:
			case MonsterInfo::STATUE_HUMAN_COMPLETE1:
			case MonsterInfo::STATUE_AKHAN_LOADING1:
			case MonsterInfo::STATUE_AKHAN_COMPLETE1:
			// 2����
			case MonsterInfo::STATUE_NEUTRALITY2:
			case MonsterInfo::STATUE_HUMAN_LOADING2:
			case MonsterInfo::STATUE_HUMAN_COMPLETE2:
			case MonsterInfo::STATUE_AKHAN_LOADING2:
			case MonsterInfo::STATUE_AKHAN_COMPLETE2:
			{
				lpNewMonster = new CStatue(tempInfo, 0);
				lpNewMonster->SetMapIndex(wMapIndex);

				if (0 != lpNewMonster)
				{
					CStatue* lpStatue = reinterpret_cast<CStatue *>(lpNewMonster);

					// 1����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::STATUE_HUMAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::STATUE_HUMAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::STATUE_AKHAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::STATUE_AKHAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					// 2����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::STATUE_HUMAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::STATUE_HUMAN_COMPLETE2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::STATUE_AKHAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::STATUE_AKHAN_COMPLETE2))		{ delete lpNewMonster; return false; }
				}

				break;
			}

			// 1����
			case MonsterInfo::BG_STATUE_NEUTRALITY1:
			case MonsterInfo::BG_STATUE_HUMAN_LOADING1:
			case MonsterInfo::BG_STATUE_HUMAN_COMPLETE1:
			case MonsterInfo::BG_STATUE_AKHAN_LOADING1:
			case MonsterInfo::BG_STATUE_AKHAN_COMPLETE1:
			// 2����
			case MonsterInfo::BG_STATUE_NEUTRALITY2:
			case MonsterInfo::BG_STATUE_HUMAN_LOADING2:
			case MonsterInfo::BG_STATUE_HUMAN_COMPLETE2:
			case MonsterInfo::BG_STATUE_AKHAN_LOADING2:
			case MonsterInfo::BG_STATUE_AKHAN_COMPLETE2:
			{
				lpNewMonster = new CStatue(tempInfo, 0);
				lpNewMonster->SetMapIndex(wMapIndex);

				if (0 != lpNewMonster)
				{
					CStatue* lpStatue = reinterpret_cast<CStatue *>(lpNewMonster);

					// 1����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::BG_STATUE_HUMAN_LOADING1))			{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::BG_STATUE_HUMAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::BG_STATUE_AKHAN_LOADING1))			{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::BG_STATUE_AKHAN_COMPLETE1))		{ delete lpNewMonster; return false; }

					//2 ����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::BG_STATUE_HUMAN_LOADING2))			{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::BG_STATUE_HUMAN_COMPLETE2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::BG_STATUE_AKHAN_LOADING2))			{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::BG_STATUE_AKHAN_COMPLETE2))		{ delete lpNewMonster; return false; }
				}

				break;
			}

			// 1����
			case MonsterInfo::POWER_STATUE_NEUTRALITY1:
			case MonsterInfo::POWER_STATUE_HUMAN_LOADING1:
			case MonsterInfo::POWER_STATUE_HUMAN_COMPLETE1:
			case MonsterInfo::POWER_STATUE_AKHAN_LOADING1:
			case MonsterInfo::POWER_STATUE_AKHAN_COMPLETE1:
			// 2����
			case MonsterInfo::POWER_STATUE_NEUTRALITY2:
			case MonsterInfo::POWER_STATUE_HUMAN_LOADING2:
			case MonsterInfo::POWER_STATUE_HUMAN_COMPLETE2:
			case MonsterInfo::POWER_STATUE_AKHAN_LOADING2:
			case MonsterInfo::POWER_STATUE_AKHAN_COMPLETE2:
			{
				lpNewMonster = new CStatue(tempInfo, 0);
				lpNewMonster->SetMapIndex(wMapIndex);

				if (0 != lpNewMonster)
				{
					CStatue* lpStatue = reinterpret_cast<CStatue *>(lpNewMonster);

					// 1����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::POWER_STATUE_HUMAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::POWER_STATUE_HUMAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::POWER_STATUE_AKHAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::POWER_STATUE_AKHAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					// 2����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::POWER_STATUE_HUMAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::POWER_STATUE_HUMAN_COMPLETE2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::POWER_STATUE_AKHAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::POWER_STATUE_AKHAN_COMPLETE2))		{ delete lpNewMonster; return false; }
				}

				break;
			}

			// 1����
			case MonsterInfo::INT_STATUE_NEUTRALITY1:
			case MonsterInfo::INT_STATUE_HUMAN_LOADING1:
			case MonsterInfo::INT_STATUE_HUMAN_COMPLETE1:
			case MonsterInfo::INT_STATUE_AKHAN_LOADING1:
			case MonsterInfo::INT_STATUE_AKHAN_COMPLETE1:
			// 2����
			case MonsterInfo::INT_STATUE_NEUTRALITY2:
			case MonsterInfo::INT_STATUE_HUMAN_LOADING2:
			case MonsterInfo::INT_STATUE_HUMAN_COMPLETE2:
			case MonsterInfo::INT_STATUE_AKHAN_LOADING2:
			case MonsterInfo::INT_STATUE_AKHAN_COMPLETE2:
			{
				lpNewMonster = new CStatue(tempInfo, 0);
				lpNewMonster->SetMapIndex(wMapIndex);

				if (0 != lpNewMonster)
				{
					CStatue* lpStatue = reinterpret_cast<CStatue *>(lpNewMonster);

					// 1����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::INT_STATUE_HUMAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::INT_STATUE_HUMAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::INT_STATUE_AKHAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::INT_STATUE_AKHAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					// 2����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::INT_STATUE_HUMAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::INT_STATUE_HUMAN_COMPLETE2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::INT_STATUE_AKHAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::INT_STATUE_AKHAN_COMPLETE2))		{ delete lpNewMonster; return false; }
				}

				break;
			}

			// 1����
			case MonsterInfo::EXP_STATUE_NEUTRALITY1:
			case MonsterInfo::EXP_STATUE_HUMAN_LOADING1:
			case MonsterInfo::EXP_STATUE_HUMAN_COMPLETE1:
			case MonsterInfo::EXP_STATUE_AKHAN_LOADING1:
			case MonsterInfo::EXP_STATUE_AKHAN_COMPLETE1:
			// 2����
			case MonsterInfo::EXP_STATUE_NEUTRALITY2:
			case MonsterInfo::EXP_STATUE_HUMAN_LOADING2:
			case MonsterInfo::EXP_STATUE_HUMAN_COMPLETE2:
			case MonsterInfo::EXP_STATUE_AKHAN_LOADING2:
			case MonsterInfo::EXP_STATUE_AKHAN_COMPLETE2:
			{
				lpNewMonster = new CStatue(tempInfo, 0);
				lpNewMonster->SetMapIndex(wMapIndex);

				if (0 != lpNewMonster)
				{
					CStatue* lpStatue = reinterpret_cast<CStatue *>(lpNewMonster);

					// 1����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::EXP_STATUE_HUMAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::EXP_STATUE_HUMAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::EXP_STATUE_AKHAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::EXP_STATUE_AKHAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					// 2����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::EXP_STATUE_HUMAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::EXP_STATUE_HUMAN_COMPLETE2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::EXP_STATUE_AKHAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::EXP_STATUE_AKHAN_COMPLETE2))		{ delete lpNewMonster; return false; }
				}

				break;
			}

			// 1����
			case MonsterInfo::WEALTH_STATUE_NEUTRALITY1:
			case MonsterInfo::WEALTH_STATUE_HUMAN_LOADING1:
			case MonsterInfo::WEALTH_STATUE_HUMAN_COMPLETE1:
			case MonsterInfo::WEALTH_STATUE_AKHAN_LOADING1:
			case MonsterInfo::WEALTH_STATUE_AKHAN_COMPLETE1:
			// 2����
			case MonsterInfo::WEALTH_STATUE_NEUTRALITY2:
			case MonsterInfo::WEALTH_STATUE_HUMAN_LOADING2:
			case MonsterInfo::WEALTH_STATUE_HUMAN_COMPLETE2:
			case MonsterInfo::WEALTH_STATUE_AKHAN_LOADING2:
			case MonsterInfo::WEALTH_STATUE_AKHAN_COMPLETE2:
			{
				lpNewMonster = new CStatue(tempInfo, 0);
				lpNewMonster->SetMapIndex(wMapIndex);

				if (0 != lpNewMonster)
				{
					CStatue* lpStatue = reinterpret_cast<CStatue *>(lpNewMonster);

					// 1����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::WEALTH_STATUE_HUMAN_LOADING1))			{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::WEALTH_STATUE_HUMAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::WEALTH_STATUE_AKHAN_LOADING1))			{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::WEALTH_STATUE_AKHAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					// 2����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::WEALTH_STATUE_HUMAN_LOADING2))			{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::WEALTH_STATUE_HUMAN_COMPLETE2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::WEALTH_STATUE_AKHAN_LOADING2))			{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::WEALTH_STATUE_AKHAN_COMPLETE2))		{ delete lpNewMonster; return false; }
				}

				break;
			}

			// 1����
			case MonsterInfo::LIFE_EXTRACT_NEUTRALITY1:
			case MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING1:
			case MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE1:
			case MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING1:
			case MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE1:
			// 2����
			case MonsterInfo::LIFE_EXTRACT_NEUTRALITY2:
			case MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING2:
			case MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE2:
			case MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING2:
			case MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE2:
			{
				lpNewMonster = new CStatue(tempInfo, 0);
				lpNewMonster->SetMapIndex(wMapIndex);

				if (0 != lpNewMonster)
				{
					CStatue* lpStatue = reinterpret_cast<CStatue *>(lpNewMonster);

					// 1����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING1))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE1))		{ delete lpNewMonster; return false; }
					// 2����
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING2))		{ delete lpNewMonster; return false; }
					if (false == lpStatue->CreateLinkStatue(MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE2))		{ delete lpNewMonster; return false; }
				}

				break;
			}

			default:
			{
				// MON_TODO : ���� Ÿ�Կ� �°� ����
				const CMonsterMgr::MonsterProtoType* pProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(tempInfo.m_nKID);
				if (NULL == pProtoType)
				{
					ERRLOG1(g_Log, "KID : %d �� �ش��ϴ� ���� ������Ÿ���� �����ϴ�.", tempInfo.m_nKID);
					continue;
				}

				switch (pProtoType->m_MonsterInfo.m_cSkillPattern)
				{
					case MonsterInfo::PATTERN_DEFENDER:	lpNewMonster = new CDefenderMonster(tempInfo);	break;
					case MonsterInfo::PATTERN_WARRIOR:	lpNewMonster = new CWarriorMonster(tempInfo);	break;
					case MonsterInfo::PATTERN_ACOLYTE:	lpNewMonster = new CAcolyteMonster(tempInfo);	break;
					case MonsterInfo::PATTERN_MAGE:		lpNewMonster = new CMageMonster(tempInfo);		break;
					case MonsterInfo::PATTERN_BOSS:		lpNewMonster = new CBossMonster(tempInfo);		break;
					case MonsterInfo::PATTERN_NAMED:	lpNewMonster = new CNamedMonster(tempInfo);		break;
					case MonsterInfo::PATTERN_CHIEF:	lpNewMonster = new CChiefMonster(tempInfo);		break;
					case MonsterInfo::PATTERN_OBJECT:	lpNewMonster = new CObjectMonster(tempInfo);	break;
					case MonsterInfo::PATTERN_GATHER:	
						lpNewMonster = new CGatherMonster(tempInfo);	
						break;
					case MonsterInfo::PATTERN_GUARD:	
						lpNewMonster = new CGuardMonster(tempInfo);		
						break;

					default:							lpNewMonster = new CMonster(tempInfo);			break;
				}

				lpNewMonster->SetMapIndex(wMapIndex);
				break;
			}
		}

		if (NULL == lpNewMonster) 
        {
			ERRLOG0(g_Log, "���� ������ �����Ͽ����ϴ�.");
			return false;
		}

		if (false == lpNewMonster->InitMonster(tempInfo.m_Pos))
		{
			ERRLOG0(g_Log, "���� �ʱ�ȭ�� �����Ͽ����ϴ�.");
			return false;
		}

		if (0 != wMapIndex)
		{
			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
			if (NULL != lpVirtualArea)
			{
				CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
				if (lpVirtualMonsterMgr)
				{
					lpVirtualMonsterMgr->AddMonster(lpNewMonster);
				}
			}
		}
		else
		{
			CreatureManager.AddCreature(lpNewMonster);
		}

		// MON_TODO : ���� ��Ƽ
		// ��Ƽ ����
		if (0 != lpNewMonster->GetPID())
		{
			// ��Ƽ�� ���ٸ� �����Ѵ�.
			CParty* lpParty = CPartyMgr::GetInstance().GetParty( lpNewMonster->GetPID() );
			if (0 == lpParty)
			{
				lpParty = new CMonsterParty();
				lpParty->SetUID( lpNewMonster->GetPID() );
				CPartyMgr::GetInstance().AddParty(lpParty);
			}

			// ��Ƽ�� ���� �� ��Ƽ�� �߰�
			lpNewMonster->SetParty(lpParty);
			lpParty->Join(lpNewMonster->GetCID(), 0, 0, wMapIndex);
		}
		// --------------------------------
	}

	DelimitedFile.Close();
	DETLOG0(g_Log, "���� �α����� �Ϸ��߽��ϴ�.");
	return true;
}

bool CCellManager::LoadSafetyZone(const char* szFileName)
{
	vector<SafetyZoneInfo *>::iterator itr; 

	FILE* lpFile = fopen(szFileName, "rb");	
	if (NULL == lpFile)
	{
		ERRLOG1(g_Log, "������Ƽ�� �ε��� �����Ͽ����ϴ�. ���ϸ�:%s", szFileName);
		return false; 
	}

	unsigned long dwTriggerNum;  
	fread(&dwTriggerNum, sizeof(unsigned long), 1, lpFile); 

	for (unsigned long dwTriggerIndex = 0; dwTriggerIndex < dwTriggerNum; ++dwTriggerIndex)
	{
		SafetyZoneInfo* lpInfo = new SafetyZoneInfo; 

		fread(&(lpInfo->m_dwSectorX), sizeof(unsigned long), 1, lpFile); 
		fread(&(lpInfo->m_dwSectorY), sizeof(unsigned long), 1, lpFile); 
		fread(lpInfo->m_szFilename, sizeof(char), MAX_PATH, lpFile); 
		fread(&(lpInfo->m_dwEventNum), sizeof(unsigned long), 1, lpFile); 
		fread(lpInfo->m_aryColorTable[0], sizeof(unsigned long), BGM_TEXTURE_SIZE * BGM_TEXTURE_SIZE, lpFile); 

		unsigned long dwIndex = 0;
		unsigned long dwEventKey = 0;
		for (dwIndex = 0; dwIndex < lpInfo->m_dwEventNum; ++dwIndex)
		{ 
			fread(&dwEventKey, sizeof(unsigned long), 1, lpFile); 
			lpInfo->m_vecEventKey.push_back(dwEventKey); 
		}

		unsigned long dwColorKey = 0;
		for (dwIndex = 0; dwIndex < lpInfo->m_dwEventNum; ++dwIndex)
		{
			fread(&dwColorKey, sizeof(unsigned long), 1, lpFile ); 
			lpInfo->m_vecBGMColorKey.push_back(dwColorKey); 
		}
		for (dwIndex = 0; dwIndex < lpInfo->m_dwEventNum; ++dwIndex)
		{
			char szFilePath[MAX_PATH]; 
			fread(szFilePath, sizeof(char), MAX_PATH, lpFile);
		}

		m_vecSafetyZone.push_back(lpInfo); 
	}

	fclose(lpFile);
	return true;
}

bool CCellManager::AdminSummonMonster(int nKID, Position Pos)
{
	CMonster::MonsterCreateInfo tempInfo;

	CMonster*           lpNewMonster    = NULL;
	CCreatureManager&   CreatureManager = CCreatureManager::GetInstance();

	// nKID �� �������� üũ
	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);
	if (0 == lpProtoType)
	{
		ERRLOG1(g_Log, "KID : %d  ���͸� ��ȯ�ϴµ� �����Ͽ����ϴ�.", nKID);
		return false;
	}

	// ���� �ִ� ���� ���� �ʰ��ϴ��� üũ
	if (CreatureManager.GetMonsterNum() >= CMonster::MAX_MONSTER_UID)
	{
		ERRLOG0(g_Log, "���� ���� ���� ���� �ִ��Դϴ�. ���̻� ���͸� ������ �� �����ϴ�.");
		return false;
	}

	// ���� �ʿ��� ������ ���ڸ� �Ѱ��ش�. (CID �� ����ũ�ϰ� �ϱ� ���ؼ�)
	unsigned long dwUID = CreatureManager.GetAvailableMonsterUID(static_cast<unsigned short>(nKID));
	if (CCreatureManager::NO_BLANK_UID == dwUID)
	{
		ERRLOG1(g_Log, "KindID:%d �ش� ������ ���� ���� �ִ��Դϴ�. ���̻� ���͸� ������ �� �����ϴ�.", nKID);
		return false;
	}

	tempInfo.m_dwCID = (dwUID << 16) + nKID;
	tempInfo.m_nKID = nKID;
	tempInfo.m_dwPID = 0;
	tempInfo.m_Pos = Pos;
	tempInfo.m_bScout = false;
	tempInfo.m_nMovingPattern = 0;
	tempInfo.m_wRespawnArea = CCell::CELL_DISTANCE;

	// MON_TODO : ���� Ÿ�Կ� �°� ����
	switch (lpProtoType->m_MonsterInfo.m_cSkillPattern)
	{
		case MonsterInfo::PATTERN_DEFENDER:	lpNewMonster = new CDefenderMonster(tempInfo, true);	break;
		case MonsterInfo::PATTERN_WARRIOR:	lpNewMonster = new CWarriorMonster(tempInfo, true);		break;
		case MonsterInfo::PATTERN_ACOLYTE:	lpNewMonster = new CAcolyteMonster(tempInfo, true);		break;
		case MonsterInfo::PATTERN_MAGE:		lpNewMonster = new CMageMonster(tempInfo, true);		break;
		case MonsterInfo::PATTERN_BOSS:		lpNewMonster = new CBossMonster(tempInfo, true);		break;
		case MonsterInfo::PATTERN_NAMED:	lpNewMonster = new CNamedMonster(tempInfo, true);		break;
		case MonsterInfo::PATTERN_CHIEF:	lpNewMonster = new CChiefMonster(tempInfo, true);		break;
		case MonsterInfo::PATTERN_OBJECT:	lpNewMonster = new CObjectMonster(tempInfo, true);		break;
		case MonsterInfo::PATTERN_GATHER:	
			lpNewMonster = new CGatherMonster(tempInfo);	
			break;
		case MonsterInfo::PATTERN_GUARD:	
			lpNewMonster = new CGuardMonster(tempInfo, true);		
			break;

		default:							lpNewMonster = new CMonster(tempInfo, true);			break;
	}
	
	if (NULL == lpNewMonster) 
	{
		ERRLOG0(g_Log, "���� ������ �����Ͽ����ϴ�.");
		return false;
	}

	if (false == lpNewMonster->InitMonster(tempInfo.m_Pos))
	{
		ERRLOG0(g_Log, "���� �ʱ�ȭ�� �����Ͽ����ϴ�.");
		return false;
	}

	return CreatureManager.AddCreature(lpNewMonster);
}


bool CCellManager::SummonMonster(int nKID, Position Pos, CCharacter* lpMaster)
{
	CMonster::MonsterCreateInfo tempInfo;

	tempInfo.m_dwCID = Creature::SUMMON_MONSTER_BIT + (m_usSummonCount << 16) + nKID;
	if (0 != CCreatureManager::GetInstance().GetCreature(tempInfo.m_dwCID))
	{
		ERRLOG0(g_Log, "���� ��ȯ�� �����Ͽ����ϴ�.");
		return false;
	}

	tempInfo.m_nKID = nKID;
	tempInfo.m_Pos = Pos;

	CMonster* lpSummonMonster = 0;
	if (0 != lpMaster)
	{
		CAggresiveCreature* lpSummonee = lpMaster->GetSummonee();
		if (0 != lpSummonee)
		{
			lpSummonee->Dead(0);
		}
	}

	lpSummonMonster = new CSummonMonster(tempInfo, lpMaster);
	if (0 == lpSummonMonster) 
	{
		ERRLOG0(g_Log, "���� ��ȯ�� �����Ͽ����ϴ�.");
		return false;
	}
	if (false == lpSummonMonster->InitMonster(tempInfo.m_Pos))
	{
		delete lpSummonMonster;
		lpSummonMonster = NULL;
		ERRLOG0(g_Log, "��ȯ ���� �ʱ�ȭ�� �����Ͽ����ϴ�.");
		return false;
	}

	CCreatureManager::GetInstance().AddCreature(lpSummonMonster);

	m_usSummonCount++;
	if (CMonster::MAX_MONSTER_UID == m_usSummonCount) { m_usSummonCount = 0; }

	if (0 != lpMaster)
	{
		lpMaster->SetSummonee(lpSummonMonster);

		GameClientSendPacket::SendCharSummon(lpMaster->GetCID(), lpSummonMonster);
	}
	else
	{
		lpSummonMonster->SendMove(CMonster::RUN_ANI_LIMIT_MIN);
	}

	return true;
}

CCell* CCellManager::GetCell(unsigned short wMapIndex, unsigned char cCellX, unsigned char cCellZ) 
{ 
	CCell* lpCell = 0;

	if (0 != wMapIndex)
	{
		VirtualArea::CVirtualArea* lpArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
		if (NULL == lpArea)
		{
			ERRLOG1(g_Log, "MapIndex:0x%04x, �ε����� ���� ������ ã�� �� �����ϴ�.", wMapIndex);
			return lpCell;
		}

		if (cCellX < 0 || cCellX >= lpArea->GetWidth() || cCellZ < 0 || cCellZ >= lpArea->GetHeight())
		{
			ERRLOG5(g_Log, "MapIndex:0x%04x, X:%d(Max:%d), Z:%d(Max:%d) �ε����� ���� ã�� �� �����ϴ�.", 
				wMapIndex, cCellX, lpArea->GetWidth(), cCellZ, lpArea->GetHeight());
			return lpCell;
		} 

		lpCell = lpArea->GetCell(cCellX + cCellZ * lpArea->GetWidth());
	}
	else
	{
		if (cCellX < 0 || cCellX >= CCell::ms_CellSize || cCellZ < 0 || cCellZ >= CCell::ms_CellSize)
		{
			ERRLOG2(g_Log, "X:%d, Z:%d �ε����� ���� ã�� �� �����ϴ�.", cCellX, cCellZ);
			return lpCell;
		} 

		lpCell = &m_CellData[cCellX + cCellZ * CCell::ms_CellSize];
	}

	return lpCell;
}

CCell* CCellManager::GetCell(unsigned short wMapIndex, unsigned long dwPosX, unsigned long dwPosY, unsigned long dwPosZ) 
{ 
	CCell* lpCell = 0;

	if (0 != wMapIndex)
	{
		VirtualArea::CVirtualArea* lpArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(wMapIndex);
		if (NULL == lpArea)
		{
			ERRLOG1(g_Log, "MapIndex:0x%04x, �ε����� ���� ������ ã�� �� �����ϴ�.", wMapIndex);
			return lpCell;
		}

		if (dwPosX < lpArea->GetStartX() || 
			dwPosZ < lpArea->GetStartZ() || 
			dwPosX > static_cast<unsigned long>((lpArea->GetWidth() << CCell::CELL_RESOLUTION) + lpArea->GetStartX()) || 
			dwPosZ > static_cast<unsigned long>((lpArea->GetHeight() << CCell::CELL_RESOLUTION) + lpArea->GetStartZ())) 
		{
			ERRLOG3(g_Log, "X:%d, Y:%d, Z:%d ��ġ�� Cell �� ã�� �� �����ϴ�.", dwPosX, dwPosY, dwPosZ);
			return 0;
		}

		dwPosX -= lpArea->GetStartX();
		dwPosZ -= lpArea->GetStartZ();

		lpCell = lpArea->GetCell( (dwPosX >> CCell::CELL_RESOLUTION) + (dwPosZ >> CCell::CELL_RESOLUTION) * lpArea->GetWidth() );
	}
	else
	{
		if (dwPosX < 0 || dwPosX > static_cast<unsigned long>(CCell::ms_CellSize << CCell::CELL_RESOLUTION) || 
			dwPosZ < 0 || dwPosZ > static_cast<unsigned long>(CCell::ms_CellSize << CCell::CELL_RESOLUTION)) 
		{
			ERRLOG3(g_Log, "X:%d, Y:%d, Z:%d ��ġ�� Cell �� ã�� �� �����ϴ�.", dwPosX, dwPosY, dwPosZ);
			return 0;
		}

		lpCell = &m_CellData[(dwPosX >> CCell::CELL_RESOLUTION) + (dwPosZ >> CCell::CELL_RESOLUTION) * CCell::ms_CellSize];
	}

	return lpCell;
}


// -------------------------------------------------------------------------------------
// �α׿� �Լ���

struct CheckInfo
{	
	int m_nCellX;
	int m_nCellZ;
	CheckInfo(int nIndex) : m_nCellX(nIndex % CCell::ms_CellSize), m_nCellZ(nIndex / CCell::ms_CellSize) { }
};

typedef std::map<unsigned long, CheckInfo>			CIDInCell;
typedef std::multimap<unsigned long, CheckInfo>	    DUPCID;

class CheckMap
{
public:
	CheckMap(CIDInCell& Map) : m_Map(Map) { }
	bool operator() (CAggresiveCreature* lpCreature) { m_Map.erase(lpCreature->GetCID()); return true; }		
protected:
	CIDInCell& m_Map;
};


bool CCellManager::CheckCellAggresiveCreatures()
{
	if (0 == m_CellData)
	{
		return false;
	}

	CIDInCell	AggresiveCreatureMap;
	DUPCID		DuplicateCID;

	unsigned long		dwCID = 0;

	for (int nIndex = 0; nIndex < CCell::ms_CellSize * CCell::ms_CellSize; ++nIndex)
	{
		CCell& Cell = m_CellData[nIndex];

		CCharacter* lpCharacter = Cell.GetFirstCharacter();
		while (NULL != lpCharacter)
		{
			dwCID = lpCharacter->GetCID();
			if (false == AggresiveCreatureMap.insert(CIDInCell::value_type(dwCID, CheckInfo(nIndex))).second)
			{
				DuplicateCID.insert(DUPCID::value_type(dwCID, CheckInfo(nIndex)));
			}

			lpCharacter = Cell.GetNextCharacter();
		}

		CMonster* lpMonster = Cell.GetFirstMonster();
		while (NULL != lpMonster)
		{
			dwCID = lpMonster->GetCID();
			if (false == AggresiveCreatureMap.insert(CIDInCell::value_type(dwCID, CheckInfo(nIndex))).second)
			{
				DuplicateCID.insert(DUPCID::value_type(dwCID, CheckInfo(nIndex)));
			}

			lpMonster = Cell.GetNextMonster();
		}
	}

	CheckMap	check(AggresiveCreatureMap);
	CCreatureManager::GetInstance().ProcessAllCharacter(check);
	CCreatureManager::GetInstance().ProcessAllMonster(check);
	
	FILE* fFile = fopen("CellLog.txt", "wt");
	if (NULL == fFile) { return false; }

	if (DuplicateCID.empty())
	{
		fprintf(fFile, "�� ������ �ߺ��Ǵ� CID�� �����ϴ�.");
	}
	else
	{
		for (DUPCID::iterator itr = DuplicateCID.begin(); itr != DuplicateCID.end(); ++itr)
		{
			fprintf(fFile, "�� �ȿ��� �ߺ��Ǵ� CID:0x%08x - CellX:%d, CellZ:%d", itr->first, 
				itr->second.m_nCellX, itr->second.m_nCellZ);
		}
	}

	if (AggresiveCreatureMap.empty())
	{
		fprintf(fFile, "ũ���� �ʿ� �ִ� ���̵� ���Դϴ�.");
	}
	else
	{
		for (CIDInCell::iterator itr = AggresiveCreatureMap.begin(); itr != AggresiveCreatureMap.end(); ++itr)
		{
			fprintf(fFile, "ũ���� �ʿ� ���� CID:0x%08x - CellX:%d, CellZ:%d", itr->first,
				itr->second.m_nCellX, itr->second.m_nCellZ);
		}
	}

	fclose(fFile);
	return true;
}


bool CCellManager::CheckCellStatus(void)
{
	if (NULL == m_CellData)
	{
		return false;
	}
    
    std::fstream file("CellStat.txt", ios_base::out);

    file << "X/Z/�����/�ֺ� �����/���ͼ�/�ֺ� ���ͼ�/"
         << "������ ��/�ֺ� ������ ��/���� ũ���� ��/�ֺ� ũ���� ��/���� ��ü��/�ֺ� ��ü��"
         << std::endl;

	for (int nIndex = 0; nIndex < CCell::ms_CellSize * CCell::ms_CellSize; ++nIndex)
	{
		const int nCellX = nIndex % CCell::ms_CellSize;
		const int nCellZ = nIndex / CCell::ms_CellSize;

        CCell& CurrentCell = m_CellData[nIndex];

		unsigned long   nNearCharacters  = 0;
        unsigned long   nNearMonsters    = 0;
        unsigned long   nNearItems       = 0;

        unsigned long   nCurrentCharacter   = static_cast<unsigned long>(CurrentCell.GetCharacterNum());
        unsigned long   nCurrentMonster     = static_cast<unsigned long>(CurrentCell.GetMonsterNum());
        unsigned long   nCurrentItem        = static_cast<unsigned long>(CurrentCell.GetItemNum());
       
		for (int nCell = 0; nCell < CCell::CONNECT_NUM; ++nCell)
		{
			CCell* lpCell = CurrentCell.GetConnectCell(nCell);
			if (0 != lpCell)
			{
                nNearCharacters += static_cast<unsigned long>(lpCell->GetCharacterNum());
                nNearMonsters   += static_cast<unsigned long>(lpCell->GetMonsterNum());
                nNearItems      += static_cast<unsigned long>(lpCell->GetItemNum());
			}
		}

        file    << nCellX << '/'
                << nCellZ << '/'
                << nCurrentCharacter << '/'
                << nNearCharacters << '/'
                << nCurrentMonster << '/'
                << nNearMonsters << '/'
                << nCurrentItem << '/'
                << nNearItems << '/'
                << nCurrentCharacter + nCurrentMonster << '/'
                << nNearCharacters + nNearMonsters << '/'

                << nCurrentCharacter + nCurrentMonster + nCurrentItem << '/'
                << nNearCharacters + nNearMonsters + nNearItems << '/'
                << std::endl;
	}

    return true;
}


CCell* CCellManager::GetCell(unsigned __int64 nItemID) 
{
	CCell::ItemInfo itemInfo;
	itemInfo.UID.m_nUniqueID = nItemID;

	return GetCell(itemInfo.UID.m_Field.wMapIndex, itemInfo.UID.m_Field.cCellX, itemInfo.UID.m_Field.cCellZ);
}

CCell* CCellManager::GetCell(unsigned short wMapIndex, POS& Pos) 
{ 
	return GetCell(wMapIndex, 
		static_cast<unsigned long>(Pos.fPointX), 
		static_cast<unsigned long>(Pos.fPointY), 
		static_cast<unsigned long>(Pos.fPointZ));
}

void CCellManager::LowerResolution(int nHighX, int nHighZ, int *nLowX, int *nLowZ) 
{
	*nLowX = nHighX >> CCell::CELL_RESOLUTION;
	*nLowZ = nHighZ >> CCell::CELL_RESOLUTION;
}

void CCellManager::HigherResolution(int nLowX, int nLowZ, int *nHighX, int *nHighZ) 
{
	*nHighX = nLowX << CCell::CELL_RESOLUTION;
	*nHighZ = nLowZ << CCell::CELL_RESOLUTION;
}

bool CCellManager::IsCampCreateArea(Position Pos)
{
	unsigned char cSectorX = static_cast<unsigned char>((Pos.m_fPointX * 100) / SECTOR_SIZE);
	unsigned char cSectorZ = static_cast<unsigned char>((Pos.m_fPointZ * 100) / SECTOR_SIZE);

	float fMagnification = SECTOR_SIZE / static_cast<float>(BGM_TEXTURE_SIZE); 
	int nDestX = static_cast<int>(static_cast<int>(Pos.m_fPointX * 100) % SECTOR_SIZE / fMagnification);
	int nDestZ = static_cast<int>(BGM_TEXTURE_SIZE - 
		(static_cast<int>(Pos.m_fPointZ * 100) % SECTOR_SIZE / fMagnification) - 1);

	for (vector<SafetyZoneInfo *>::iterator infoItr = m_vecSafetyZone.begin(); 
		infoItr != m_vecSafetyZone.end(); ++infoItr)
	{
		SafetyZoneInfo* lpInfo = *infoItr;

		if (lpInfo->m_dwSectorX != cSectorX || lpInfo->m_dwSectorY != cSectorZ)
		{
			continue;
		}

		unsigned long dwBGMColor = lpInfo->m_aryColorTable[nDestZ][nDestX] & 0x00FFFFFF;
		vector<unsigned long>::iterator eventkeyItr = lpInfo->m_vecEventKey.begin();

		for (vector<unsigned long>::iterator colorkeyItr = lpInfo->m_vecBGMColorKey.begin(); 
			colorkeyItr != lpInfo->m_vecBGMColorKey.end(); ++colorkeyItr, ++eventkeyItr)
		{
			if (dwBGMColor == *colorkeyItr)
			{
				// ������Ƽ���� ǥ���ϴ� �÷��׵� (���� ��� ��ũ��Ʈ�� �Բ� ���Ƿ�...)
				if (SafetyZoneInfo::EK_CAMP_UNCREATE == *eventkeyItr)
				{
					return false;
				}
			}
		}
	}

	return true; 	
}


bool CCellManager::IsSafetyZone(Position Pos)
{
	// �ʺ����� ������ ������Ƽ���Դϴ�.
	/*if (SERVER_ID::ZONE1 == CServerSetup::GetInstance().GetServerZone() || //was commented out added for edin multi attack bug
		SERVER_ID::ZONE2 == CServerSetup::GetInstance().GetServerZone())
	{
		return true;
	}
	if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
	{
		if(((Pos.m_fPointX >= 1844)&&(Pos.m_fPointX <= 2316))&&((Pos.m_fPointZ >= 2836)&&(Pos.m_fPointZ <= 3265)))
			return true;
	}*/


	unsigned char cSectorX = static_cast<unsigned char>((Pos.m_fPointX * 100) / SECTOR_SIZE);
	unsigned char cSectorZ = static_cast<unsigned char>((Pos.m_fPointZ * 100) / SECTOR_SIZE);

	float fMagnification = SECTOR_SIZE / static_cast<float>(BGM_TEXTURE_SIZE); 
	int nDestX = static_cast<int>(static_cast<int>(Pos.m_fPointX * 100) % SECTOR_SIZE / fMagnification);
	int nDestZ = static_cast<int>(BGM_TEXTURE_SIZE - 
		(static_cast<int>(Pos.m_fPointZ * 100) % SECTOR_SIZE / fMagnification) - 1);

	for (vector<SafetyZoneInfo *>::iterator infoItr = m_vecSafetyZone.begin(); 
		infoItr != m_vecSafetyZone.end(); ++infoItr)
	{
		SafetyZoneInfo* lpInfo = *infoItr;

		if (lpInfo->m_dwSectorX != cSectorX || lpInfo->m_dwSectorY != cSectorZ)
		{
			continue;
		}

		unsigned long dwBGMColor = lpInfo->m_aryColorTable[nDestZ][nDestX] & 0x00FFFFFF;
		vector<unsigned long>::iterator eventkeyItr = lpInfo->m_vecEventKey.begin();

		for (vector<unsigned long>::iterator colorkeyItr = lpInfo->m_vecBGMColorKey.begin(); 
			colorkeyItr != lpInfo->m_vecBGMColorKey.end(); ++colorkeyItr, ++eventkeyItr)
		{
			if (dwBGMColor == *colorkeyItr)
			{
				// ������Ƽ���� ǥ���ϴ� �÷��׵� (���� ��� ��ũ��Ʈ�� �Բ� ���Ƿ�...)
				if (SafetyZoneInfo::EK_BGM_ONCE_SAFE == *eventkeyItr || 
					SafetyZoneInfo::EK_BGM_LOOP_SAFE == *eventkeyItr || 
					SafetyZoneInfo::EK_NOTBGM_SAFE == *eventkeyItr || 
					SafetyZoneInfo::EK_ESF_SAFE == *eventkeyItr || 
					SafetyZoneInfo::EK_BGM_TURN_AMB_SAFE == *eventkeyItr)
				{
					return true;
				}
			}
		}
	}

	return false; 	
}

