#include "stdafx.h"
#include "VirtualAreaMgr.h"

#include <Utility/Math/Math.h>
#include <Utility/DelimitedFile.h>

#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/BGServer/BGServerMgr.h>
#include <Map/FieldMap/VirtualArea/Duel/DuelMgr.h>
#include <Map/FieldMap/VirtualArea/Dungeon/DungeonMgr.h>

#include <Creature/Character/Character.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>


using namespace VirtualArea;


CVirtualAreaMgr& CVirtualAreaMgr::GetInstance()
{
	static CVirtualAreaMgr ms_this;
	return ms_this;
};


CVirtualAreaMgr::CVirtualAreaMgr()
: m_BGServerMgr(CBGServerMgr::GetInstance()),
  m_DuelMgr(CDuelMgr::GetInstance()),
  m_DungeonMgr(CDungeonMgr::GetInstance()),
  m_VirtualAreaProtoTypeArray(NULL), m_VirtualAreaProtoTypeNum(0)
{
}


CVirtualAreaMgr::~CVirtualAreaMgr()
{
	DestroyVirtualAreaProtoTypeArray();
}


// Script 파일 로드
bool CVirtualAreaMgr::LoadVirtualAreaProtoType(const char* szFileName)
{
	int nIndex = 0;
	int nLineCount = 0;
	char strTemp[MAX_PATH];

	CDelimitedFile	DelimitedFile;	// 객체 소멸시, 자동 Close.
	std::vector<VirtualArea::ProtoType> virtualAreaProtoTypeVector;

	virtualAreaProtoTypeVector.reserve(10);
	VirtualArea::ProtoType    tempProtoType;

	// 매크로에 로그 코드 삽입을 잊지 말 것.
	// 매크로에서 \뒤에 공백이나 문자 삽입되지 않도록 주의할 것.
	// ( '이스케이프 시퀀스가 잘못되었습니다' 에러 발생 )
	#define READ_DATA(ColumnName, Argument) \
		if (!DelimitedFile.ReadData(Argument)) { \
			ERRLOG2(g_Log, "Virtual Area 스크립트 읽기 실패 : %d행 %s컬럼에서 에러 발생!", nLineCount, #ColumnName); \
			return false; \
		}

	#define READ_STRING(ColumnName, Buffer, BufferSize) \
		if (!DelimitedFile.ReadString(Buffer, BufferSize)) { \
			ERRLOG2(g_Log, "Virtual Area 스크립트 읽기 실패 : %d행 %s컬럼에서 에러 발생!", nLineCount, #ColumnName); \
			return false; \
		}

	#define READ_POSITION(ColumnName, Pos) \
		if (!DelimitedFile.ReadData(Pos.m_fPointX)) { \
			ERRLOG2(g_Log, "Virtual Area 스크립트 읽기 실패 : %d행 %s컬럼에서 에러 발생!", nLineCount, #ColumnName); \
			return false; \
		} \
		if (!DelimitedFile.ReadData(Pos.m_fPointY)) { \
			ERRLOG2(g_Log, "Virtual Area 스크립트 읽기 실패 : %d행 %s컬럼에서 에러 발생!", nLineCount, #ColumnName); \
			return false; \
		} \
		if (!DelimitedFile.ReadData(Pos.m_fPointZ)) { \
			ERRLOG2(g_Log, "Virtual Area 스크립트 읽기 실패 : %d행 %s컬럼에서 에러 발생!", nLineCount, #ColumnName); \
			return false; \
		}

	#define READ_DATA_ARRAY(ColumnName, Argument, ArgumentNum) \
		for (nIndex=0; nIndex < ArgumentNum; ++nIndex) { \
			READ_DATA(ColumnName, Argument[nIndex]); \
		}

	#define READ_DATA_BOOL(ColumnName, Argument) \
		if (!DelimitedFile.ReadString(strTemp, MAX_PATH)) { \
			ERRLOG2(g_Log, "Virtual Area 스크립트 읽기 실패 : %d행 %s컬럼에서 에러 발생!", nLineCount, #ColumnName); \
			return false; \
		} \
		Argument = (!strcmp(strTemp, "O")) ? true : false;


	if (!DelimitedFile.Open(szFileName ? szFileName : ms_szVirtualAreaScriptFileName))
	{
		ERRLOG1(g_Log, "%s 파일을 열 수 없습니다.", szFileName ? szFileName : ms_szVirtualAreaScriptFileName);
		return false;
	}

	while (DelimitedFile.ReadLine()) 
	{
		++nLineCount;

		// 순서가 바뀌면 곤란하다니깐~!!! (버럭!)
		READ_STRING("VID", strTemp, MAX_PATH);
		tempProtoType.m_dwVID = Math::Convert::Atoi(strTemp);

		READ_STRING("MapType", tempProtoType.m_szMapType, VirtualArea::MAX_MAP_TYPE_NAME);
		tempProtoType.m_cMapType = m_MapTypeMatching.m_matchMap.find(tempProtoType.m_szMapType)->second;

		READ_DATA("Zone", tempProtoType.m_cZone);
		READ_DATA("StartX", tempProtoType.m_wStartX);
		READ_DATA("StartZ", tempProtoType.m_wStartZ);
		READ_DATA("Width", tempProtoType.m_wWidth);
		READ_DATA("Height", tempProtoType.m_wHeight);

		READ_STRING("ArrangementFile", strTemp, MAX_PATH);
		if (0 == stricmp(strTemp, "N/A"))
		{
			tempProtoType.m_szArrangementFileName[0] = '\0';
		}
		else
		{
			strcpy(tempProtoType.m_szArrangementFileName, strTemp);
		}

		READ_POSITION("StartPosition(HUMAN)", tempProtoType.m_StartPos[CClass::HUMAN]);
		READ_POSITION("StartPosition(AKHAN)", tempProtoType.m_StartPos[CClass::AKHAN]);

		tempProtoType.m_cMaxRespawnPos = 1;
		READ_POSITION("RespawnPos1(HUMAN)", tempProtoType.m_RespawnPos[CClass::HUMAN][0]);
		READ_POSITION("RespawnPos2(HUMAN)", tempProtoType.m_RespawnPos[CClass::HUMAN][1]);
		READ_POSITION("RespawnPos3(HUMAN)", tempProtoType.m_RespawnPos[CClass::HUMAN][2]);
		READ_POSITION("RespawnPos1(AKHAN)", tempProtoType.m_RespawnPos[CClass::AKHAN][0]);
		READ_POSITION("RespawnPos2(AKHAN)", tempProtoType.m_RespawnPos[CClass::AKHAN][1]);
		READ_POSITION("RespawnPos3(AKHAN)", tempProtoType.m_RespawnPos[CClass::AKHAN][2]);

		for (int i=1; i<VirtualArea::MAX_VIRTUAL_AREA_RESPAWN_POINT; ++i)
		{
			if (tempProtoType.m_RespawnPos[CClass::HUMAN][i].m_fPointX != 0 &&
				tempProtoType.m_RespawnPos[CClass::HUMAN][i].m_fPointZ != 0)
			{
				++tempProtoType.m_cMaxRespawnPos;
			}
		}

		virtualAreaProtoTypeVector.push_back(tempProtoType);
	}

	std::sort(virtualAreaProtoTypeVector.begin(), virtualAreaProtoTypeVector.end());

	for (std::vector<VirtualArea::ProtoType>::iterator itr = virtualAreaProtoTypeVector.begin();
		 itr != virtualAreaProtoTypeVector.end() - 1; ++itr)
	{
		if (itr->m_dwVID == (itr+1)->m_dwVID)
		{
			ERRLOG1(g_Log, "겹치는 VirtualArea ID가 있습니다. VID:%d", itr->m_dwVID);
			return false;
		}
	}

	m_VirtualAreaProtoTypeNum = virtualAreaProtoTypeVector.size();
	m_VirtualAreaProtoTypeArray = new VirtualArea::ProtoType[m_VirtualAreaProtoTypeNum];
	if (NULL == m_VirtualAreaProtoTypeArray) 
	{
		ERRLOG0(g_Log, "VirtualArea 스크립트 초기화 실패 : 메모리 부족");
		return false;
	}

	std::copy(virtualAreaProtoTypeVector.begin(), virtualAreaProtoTypeVector.end(), m_VirtualAreaProtoTypeArray);
	return true;
}


void CVirtualAreaMgr::DestroyVirtualAreaProtoTypeArray()
{
	m_VirtualAreaProtoTypeNum = 0;

	if (0 != m_VirtualAreaProtoTypeArray)
	{
		delete [] m_VirtualAreaProtoTypeArray;
		m_VirtualAreaProtoTypeArray = 0;
	}
}


CVirtualArea* CVirtualAreaMgr::GetVirtualArea(unsigned short wMapIndex)
{
	if (VirtualArea::BGSERVERMAP == (wMapIndex & VirtualArea::BGSERVERMAP))
	{
		return m_BGServerMgr.GetVirtualArea(wMapIndex);
	}
	else if (VirtualArea::DUELMAP == (wMapIndex & VirtualArea::DUELMAP))
	{
	}
	else if (VirtualArea::DUNGEON == (wMapIndex & VirtualArea::DUNGEON))
	{
	}

	return NULL;
}


class CFindProtoTypeFromVID : public std::unary_function<VirtualArea::ProtoType, bool>
{
public:

	explicit CFindProtoTypeFromVID(unsigned long dwVID) 
		:	m_dwVID(dwVID) 
	{ }		

	bool operator() (VirtualArea::ProtoType& protoType) 
	{ 
		return (m_dwVID == protoType.m_dwVID); 
	}


private:

	const unsigned long m_dwVID;
};


const VirtualArea::ProtoType* CVirtualAreaMgr::GetVirtualAreaProtoType(unsigned long dwVID)
{
	VirtualArea::ProtoType* lpProtoType = NULL;

	if (0 != dwVID)
	{
		CFindProtoTypeFromVID findVID(dwVID);
		lpProtoType = std::find_if(&m_VirtualAreaProtoTypeArray[0], &m_VirtualAreaProtoTypeArray[m_VirtualAreaProtoTypeNum], findVID);
	}

	return lpProtoType;
}


const VirtualArea::ProtoType* CVirtualAreaMgr::GetVirtualAreaProtoType(char* szMapType)
{
	for (size_t nIndex = 0; nIndex < m_VirtualAreaProtoTypeNum; nIndex++)
	{
		if (0 == strncmp(szMapType, m_VirtualAreaProtoTypeArray[nIndex].m_szMapType, VirtualArea::MAX_MAP_TYPE_NAME))
		{
			return m_VirtualAreaProtoTypeArray + nIndex;
		}
	}

	return NULL;
}


bool CVirtualAreaMgr::EnterVirtualArea(CCharacter* lpCharacter, unsigned short wMapIndex, unsigned char cMoveType)
{
	if (NULL == lpCharacter)
	{
		return false;
	}

	CVirtualArea* lpVirtualArea = GetVirtualArea(wMapIndex);
	if (NULL == lpVirtualArea)
	{
		ERRLOG2(g_Log, "CID:0x%08x 캐릭터가 존재하지 않는 가상 존(MapIndex : %d)으로 이동하려고 합니다.", lpCharacter->GetCID(), wMapIndex);
		return false;
	}

	if (VirtualArea::BGSERVERMAP == (wMapIndex & VirtualArea::BGSERVERMAP))
	{
		return m_BGServerMgr.Enter(lpCharacter, wMapIndex, cMoveType);
	}
	else if (VirtualArea::DUELMAP == (wMapIndex & VirtualArea::DUELMAP))
	{
	}
	else if (VirtualArea::DUNGEON == (wMapIndex & VirtualArea::DUNGEON))
	{
	}
	
	return false;
}


bool CVirtualAreaMgr::LeaveVirtualArea(CCharacter* lpCharacter)
{
	if (NULL == lpCharacter)
	{
		return false;
	}

	unsigned short wCurrentMapIndex = lpCharacter->GetMapIndex();
	CVirtualArea* lpVirtualArea = GetVirtualArea(wCurrentMapIndex);
	if (NULL == lpVirtualArea)
	{
		ERRLOG2(g_Log, "CID:0x%08x 캐릭터가 존재하지 않는 가상 존(MapIndex : %d)에서 나가려고 합니다.", lpCharacter->GetCID(), wCurrentMapIndex);
		return false;
	}

	if (VirtualArea::BGSERVERMAP == (wCurrentMapIndex & VirtualArea::BGSERVERMAP))
	{
		return m_BGServerMgr.Leave(lpCharacter);
	}
	else if (VirtualArea::DUELMAP == (wCurrentMapIndex & VirtualArea::DUELMAP))
	{
	}
	else if (VirtualArea::DUNGEON == (wCurrentMapIndex & VirtualArea::DUNGEON))
	{
	}

	return false;
}


void CVirtualAreaMgr::ProcessAllVirtualArea()
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		m_BGServerMgr.Process();
	}
	else
	{
	//	m_DeulMgr.Process();
	//	m_DungeonMgr.Process();
	}
}

void CVirtualAreaMgr::ProcessAllMonster()
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		m_BGServerMgr.ProcessAllMonster();
	}
	else
	{
	//	m_DeulMgr.ProcessAllMonster();
	//	m_DungeonMgr.ProcessAllMonster();
	}
}

void CVirtualAreaMgr::ProcessMonsterRegenHPAndMP()
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		m_BGServerMgr.ProcessMonsterRegenHPAndMP();
	}
	else
	{
	//	m_DeulMgr.ProcessMonsterRegenHPAndMP();
	//	m_DungeonMgr.ProcessMonsterRegenHPAndMP();
	}
}

void CVirtualAreaMgr::ProcessSummonMonsterDead()
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		m_BGServerMgr.ProcessSummonMonsterDead();
	}
	else
	{
	//	m_DuelMgr.ProcessSummonMonsterDead();
	//	m_DungeonMgr.ProcessSummonMonsterDead();
	}
}

bool CVirtualAreaMgr::ProcessAllCellPrepareBroadCast()
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		m_BGServerMgr.ProcessAllCellPrepareBroadCast();
	}
	else
	{
	//	m_DuelMgr.ProcessAllCellPrepareBroadCast();
	//	m_DungeonMgr.ProcessAllCellPrepareBroadCast();
	}

	return true;
}

bool CVirtualAreaMgr::ProcessAllCellBroadCast(unsigned long dwCurrentPulse)
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		m_BGServerMgr.ProcessAllCellBroadCast(dwCurrentPulse);
	}
	else
	{
	//	m_DuelMgr.ProcessAllCellBroadCast();
	//	m_DungeonMgr.ProcessAllCellBroadCast();
	}

	return true;
}

void CVirtualAreaMgr::ProcessDeleteItem()
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		m_BGServerMgr.ProcessDeleteItem();
	}
	else
	{
	//	m_DuelMgr.ProcessDeleteItem();
	//	m_DungeonMgr.ProcessDeleteItem();
	}
}


// --------------------------------------------------------------------------------------------
// BattleGround Server 관련 함수

bool CVirtualAreaMgr::CreateBGServer()	// Battle Ground Server Map 과 Cell 생성
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		return m_BGServerMgr.CreateBGServerMap();
	}

	return false;
}

bool CVirtualAreaMgr::SendBGServerMapList(CCharacter* lpCharacter)
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		return m_BGServerMgr.SendMapList(lpCharacter);
	}

	return false;
}

bool CVirtualAreaMgr::SendBGServerResultList(CCharacter* lpCharacter)
{
	if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
	{
		return m_BGServerMgr.SendResultList(lpCharacter);
	}

	return false;
}



