#include "stdafx.h"
#include "CharRespawnMgr.h"

#include <Utility/Math/Math.h>
#include <Utility/DelimitedFile.h>
#include <Utility/Setup/ServerSetup.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/Siege/SiegeObjectMgr.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/Camp.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <GameTime/GameTimeMgr.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>

const char* CCharRespawnMgr::ms_szRespawnScriptFileName = "./Script/Game/RespawnScript.txt";

CCharRespawnMgr&	CCharRespawnMgr::GetInstance()
{
	static CCharRespawnMgr	ms_this;
	return ms_this;
}

CCharRespawnMgr::CCharRespawnMgr()
{
}

CCharRespawnMgr::~CCharRespawnMgr()
{
	m_ProtoTypeList.clear();
	m_AllZoneTownList.clear();
}

bool CCharRespawnMgr::LoadRespawnFromFile(const char* szFileName)
{
	int nLineCount = 0;
	char strTemp[MAX_PATH];

	CDelimitedFile		DelimitedFile;			// 객체 소멸시, 자동 Close.
	RespawnProtoType    tempProtoType;
	Position			tempRespawnPos;

	m_ProtoTypeList.clear();
	m_AllZoneTownList.clear();

	// 매크로에 로그 코드 삽입을 잊지 말 것.
	// 매크로에서 \뒤에 공백이나 문자 삽입되지 않도록 주의할 것.
	// ( '이스케이프 시퀀스가 잘못되었습니다' 에러 발생 )
	#define READ_DATA(ColumnName, Argument) \
		if (!DelimitedFile.ReadData(Argument)) { \
			ERRLOG2(g_Log, "리스폰 스크립트 읽기 실패 : %d행 %s컬럼에서 에러 발생!", nLineCount, #ColumnName); \
			return false; \
		}

	#define READ_STRING(ColumnName, Buffer, BufferSize) \
		if (!DelimitedFile.ReadString(Buffer, BufferSize)) { \
			ERRLOG2(g_Log, "리스폰 스크립트 읽기 실패 : %d행 %s컬럼에서 에러 발생!", nLineCount, #ColumnName); \
			return false; \
		}

	if (!DelimitedFile.Open(szFileName ? szFileName : ms_szRespawnScriptFileName)) 
	{
		ERRLOG1(g_Log, "%s 파일을 열 수 없습니다.", szFileName ? szFileName : ms_szRespawnScriptFileName);
		return false;
	}

	while (DelimitedFile.ReadLine()) 
	{
		++nLineCount;

		// 순서가 바뀌면 곤란하다니깐~!!! (버럭!)
		tempProtoType.m_dwGID = 0;
		READ_STRING("TownID", strTemp, MAX_PATH);
		tempProtoType.m_dwTownID = Math::Convert::Atoi(strTemp);

		READ_DATA("Nation", tempProtoType.m_cNation);
		READ_DATA("Zone", tempProtoType.m_cZone);

		int iRespawnPosCount = 0;
		READ_DATA("RespawnPosCount", iRespawnPosCount);

		if (CServerSetup::GetInstance().GetServerZone() == tempProtoType.m_cZone)
		{
			for (int i=0; i<iRespawnPosCount; ++i)
			{
				READ_DATA("RespawnPosX", tempRespawnPos.m_fPointX);
				READ_DATA("RespawnPosY", tempRespawnPos.m_fPointY);
				READ_DATA("RespawnPosZ", tempRespawnPos.m_fPointZ);

				tempProtoType.m_RespawnPosList.push_back(tempRespawnPos);
			}

			RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.find(tempProtoType.m_dwTownID);
			if (itr == m_ProtoTypeList.end())
			{
				m_ProtoTypeList.insert(std::make_pair(tempProtoType.m_dwTownID, tempProtoType));

				// 마을일 경우 모든 존의 마을 리스폰 정보 맵에 집어 넣는다.
				if (0 != (tempProtoType.m_dwTownID & Castle::TOWN_BIT))
				{
					m_AllZoneTownList.insert(std::make_pair(tempProtoType.m_dwTownID, tempProtoType));
				}
			}
			else
			{
				ERRLOG1(g_Log, "리스폰 스크립트에 마을 혹은 성의 ID가 같은 것이 두개이상 존재합니다. TownID : %d", tempProtoType.m_dwTownID);
				return false;
			}

			RespawnCharMap::iterator CharItr = m_CharList.find(tempProtoType.m_dwTownID);
			if (CharItr == m_CharList.end())
			{
				RespawnCharList tempRespawnCharList;
				tempRespawnCharList.clear();
				m_CharList.insert(std::make_pair(tempProtoType.m_dwTownID, tempRespawnCharList));
			}
		}
		else
		{
			// 마을일 경우 모든 존의 마을 리스폰 정보 맵에 집어 넣는다.
			if (0 != (tempProtoType.m_dwTownID & Castle::TOWN_BIT))
			{
				for (int i=0; i<iRespawnPosCount; ++i)
				{
					READ_DATA("RespawnPosX", tempRespawnPos.m_fPointX);
					READ_DATA("RespawnPosY", tempRespawnPos.m_fPointY);
					READ_DATA("RespawnPosZ", tempRespawnPos.m_fPointZ);

					tempProtoType.m_RespawnPosList.push_back(tempRespawnPos);
				}

				RespawnProtoTypeMap::iterator itr = m_AllZoneTownList.find(tempProtoType.m_dwTownID);
				if (itr == m_AllZoneTownList.end())
				{
					m_AllZoneTownList.insert(std::make_pair(tempProtoType.m_dwTownID, tempProtoType));
				}
				else
				{
					ERRLOG1(g_Log, "리스폰 스크립트에 마을 혹은 성의 ID가 같은 것이 두개이상 존재합니다. TownID : %d", tempProtoType.m_dwTownID);
					return false;
				}
			}
		}

		tempProtoType.m_RespawnPosList.clear();
	}

	return true;
}

bool CCharRespawnMgr::AddCampRespawnPoint(unsigned long dwCampID, unsigned long dwGID, const Position& RespawnPos)
{
	RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.find(dwCampID);
	if (itr != m_ProtoTypeList.end())
	{
		ERRLOG1(g_Log, "CampID : 0x%08x 이미 존재하는 리스폰 포인트(길드 요새) 입니다.", dwCampID);
		return false;
	}

	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(dwGID);
	if (lpGuild)
	{
		RespawnProtoType tempProtoType;
		tempProtoType.m_dwTownID = dwCampID;
		tempProtoType.m_dwGID = dwGID;
		tempProtoType.m_cNation = lpGuild->GetNation();
		tempProtoType.m_cZone = CServerSetup::GetInstance().GetServerZone();
		tempProtoType.m_RespawnPosList.push_back(RespawnPos);
		m_ProtoTypeList.insert(std::make_pair(tempProtoType.m_dwTownID, tempProtoType));

		RespawnCharList tempCharList;
		tempCharList.clear();
		m_CharList.insert(std::make_pair(tempProtoType.m_dwTownID, tempCharList));

		return true;
	}
    
	return false;
}

bool CCharRespawnMgr::DeleteCampRespawnPoint(unsigned long dwCampID)
{
    RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.find(dwCampID);
	if (itr != m_ProtoTypeList.end())
	{
		m_ProtoTypeList.erase(itr);
		return true;
	}

	ERRLOG1(g_Log, "CampID : 0x%08x 존재하지 않는 리스폰 포인트(길드 요새) 입니다.", dwCampID);
	return false;
}

bool CCharRespawnMgr::AddRealmStatueRespawnPoint(unsigned long dwIndex, unsigned char cNation, const Position& RespawnPos)
{
	unsigned long dwStatueID = Castle::STATUE_BIT | (dwIndex << 16);
    RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.find(dwStatueID);
	if (itr != m_ProtoTypeList.end())
	{
		ERRLOG2(g_Log, "Statue ID : 0x%08x (0x%08x) 이미 존재하는 리스폰 포인트(다크 카나번 석상) 입니다.", dwStatueID, dwIndex);
		return false;
	}

	RespawnProtoType tempProtoType;
	tempProtoType.m_dwTownID	= dwStatueID;
	tempProtoType.m_dwGID		= 0;
	tempProtoType.m_cNation		= cNation;
	tempProtoType.m_cZone		= CServerSetup::GetInstance().GetServerZone();
	tempProtoType.m_RespawnPosList.push_back(RespawnPos);
	m_ProtoTypeList.insert(std::make_pair(tempProtoType.m_dwTownID, tempProtoType));

	RespawnCharList tempCharList;
	tempCharList.clear();
	m_CharList.insert(std::make_pair(tempProtoType.m_dwTownID, tempCharList));

	return true;
}

bool CCharRespawnMgr::DeleteRealmStatueRespawnPoint(unsigned long dwIndex)
{
	unsigned long dwStatueID = Castle::STATUE_BIT | (dwIndex << 16);
	RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.find(dwStatueID);
	if (itr != m_ProtoTypeList.end())
	{
		m_ProtoTypeList.erase(itr);
		return true;
	}

	ERRLOG2(g_Log, "Statue ID : 0x%08x (0x%08x) 존재하지 않는 리스폰 포인트(다크 카나번 석상) 입니다.", dwStatueID, dwIndex);
	return false;
}

bool CCharRespawnMgr::SetCastleRespawnPointNation(unsigned long dwCastleNameID, unsigned char cNation)
{
	RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.begin();
	RespawnProtoTypeMap::iterator end = m_ProtoTypeList.end();
	bool bRetFound = false;

	while (itr != end)
	{
		RespawnProtoType& protoType = itr->second;
		if (0 != (Castle::CASTLE_BIT & protoType.m_dwTownID))
		{
			unsigned long dwMask = dwCastleNameID << Castle::CASTLE_NAME_BIT_SHIFT;
			if (dwMask == (dwMask & protoType.m_dwTownID))
			{
				if (0 == (Castle::SIEGE_BIT & protoType.m_dwTownID))
				{
					// 수성
					bRetFound = true;
					protoType.m_cNation = cNation;
				}
				else
				{
					// 공성
					switch (cNation)
					{
						case Creature::KARTERANT:
							protoType.m_cNation = Creature::MERKADIA;
							break;

						case Creature::MERKADIA:
							protoType.m_cNation = Creature::KARTERANT;
							break;

						case Creature::ALMIGHTY_PIRATE:
						case Creature::STATELESS:
						default:
							protoType.m_cNation = Creature::STATELESS;
							break;
					}
				}
			}
		}

		++itr;
	}

	if (false == bRetFound)
	{
		ERRLOG1(g_Log, "CastleNameID : %d 존재하지 않는 리스폰 포인트(성) 입니다.", dwCastleNameID);
	}

	return bRetFound;
}
/*
void CCharRespawnMgr::DelCharacter(unsigned long dwTownID, unsigned long dwCID)
{
	RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.find(dwTownID);
	if (itr != m_ProtoTypeList.end())
	{
		RespawnProtoType& tempProtoType = itr->second;

		RespawnCharMap::iterator it = m_CharList.find(dwTownID);
		if (it != m_CharList.end())
		{
			RespawnCharList& tempCharList = it->second;

			RespawnCharList::iterator CharListItr = tempCharList.begin();
			while (CharListItr != tempCharList.end())
			{
				RespawnCharInfo& tempCharInfo = *CharListItr;

				if(tempCharInfo.m_dwCID == dwCID)
				{
					CharListItr = tempCharList.erase(CharListItr);
					return;
				}

				++CharListItr;
			}

		}

	}
}
*/
CCharRespawnMgr::eReturn CCharRespawnMgr::AddCharacter(unsigned long dwTownID, unsigned long dwCID)
{
	RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.find(dwTownID);
	if (itr != m_ProtoTypeList.end())
	{
		RespawnProtoType& tempProtoType = itr->second;

		if (0 != (dwTownID & Castle::ABTOWN_BIT))
		{
			return RET_TOWN;
		}

		if (0 != (dwTownID & Castle::TOWN_BIT))
		{
			return RET_TOWN;
		}

		RespawnCharMap::iterator it = m_CharList.find(dwTownID);
		if (it != m_CharList.end())
		{
			unsigned long dwRemainSec = 0;
			RespawnCharList& tempCharList = it->second;
			if (0 == tempCharList.size())
			{
				dwRemainSec = static_cast<unsigned long>(RESPAWN_DELAY_SEC);
			}
			else
			{
				RespawnCharList::reverse_iterator CharInfoItr = tempCharList.rbegin();
				RespawnCharInfo& tempCharInfo = *CharInfoItr;
				dwRemainSec = static_cast<unsigned long>(tempCharInfo.m_dwRemainSec + RESPAWN_DELAY_SEC);
			}
			
			tempCharList.push_back(RespawnCharInfo(dwCID, dwRemainSec));
			return RET_TRUE;
		}

		++itr;
	}

	return RET_FALSE;
}

void CCharRespawnMgr::ProcessRespawn()
{
	RespawnCharMap::iterator itr = m_CharList.begin();
	while (itr != m_CharList.end())
	{
		if (0 != (itr->first & Castle::TOWN_BIT))
		{
			// 마을은 그냥 넘어간다.
			++itr;
			continue;
		}

		RespawnCharList& tempCharList = itr->second;
		if (tempCharList.size() > 0)
		{
			RespawnCharList::iterator CharListItr = tempCharList.begin();
			while (CharListItr != tempCharList.end())
			{
				RespawnCharInfo& tempCharInfo = *CharListItr;

                if (0 < tempCharInfo.m_dwRemainSec)
                {
    				--tempCharInfo.m_dwRemainSec;

                    // 2000초(약 30분). 이 숫자를 넘으면 로그를 찍는다
                    if (2000 < tempCharInfo.m_dwRemainSec)
                    {
                        ERRLOG2(g_Log, "CID:0x%08x/RemainSec:%u/리스폰 시간 이상",
                            tempCharInfo.m_dwCID, tempCharInfo.m_dwRemainSec);
                    }
                }
                else if (0 == tempCharInfo.m_dwRemainSec)				
				{
					// 리스폰 시키고, 하나를 빼낸다.
					CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(tempCharInfo.m_dwCID);
					if (lpCharacter)
					{
						if(lpCharacter->GetRespawnTownID() == 0)
						{
							CharListItr = tempCharList.erase(CharListItr);
							continue;
						}

						RespawnProtoTypeMap::iterator ProtoTypeItr = m_ProtoTypeList.find(itr->first);
						if (ProtoTypeItr != m_ProtoTypeList.end())
						{
							RespawnProtoType& tempProtoType = ProtoTypeItr->second;
							size_t iIndex = Math::Random::ComplexRandom(static_cast<int>(tempProtoType.m_RespawnPosList.size()));
							Position resapwnPos = tempProtoType.m_RespawnPosList[iIndex];
							resapwnPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 10) - 5;
							resapwnPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 10) - 5;

							lpCharacter->Respawn(CCharRespawnMgr::RST_FORTRESS, resapwnPos);

                            if (0 != lpCharacter->GetPID())
                            {		
                                // 파티원 리스폰을 알린다.
                                GameClientSendPacket::SendCharDeadToParty(lpCharacter, 0, PktDeadInfo::RESPAWN);
                            }
						}
						else
						{
							Position resapwnPos = GetDefaultRespawnPos(lpCharacter->GetNation());
							resapwnPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 10) - 5;
							resapwnPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 10) - 5;

							lpCharacter->Respawn(CCharRespawnMgr::RST_FORTRESS, resapwnPos);

                            if (0 != lpCharacter->GetPID())
                            {		
                                // 파티원 리스폰을 알린다.
                                GameClientSendPacket::SendCharDeadToParty(lpCharacter, 0, PktDeadInfo::RESPAWN);
                            }
						}

						CharListItr = tempCharList.erase(CharListItr);
						continue;
					}
				}

				++CharListItr;
			}
		}

		++itr;
	}
}

const Position CCharRespawnMgr::GetDefaultRespawnPos(unsigned char cNation)
{
	RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.begin();
	while (itr != m_ProtoTypeList.end())
	{
		RespawnProtoType& tempProtoType = itr->second;

		if (0 != (tempProtoType.m_dwTownID & Castle::TOWN_BIT))
		{
			if (Creature::STATELESS == tempProtoType.m_cNation || cNation == tempProtoType.m_cNation)
			{
				int iRespawnPosCount = static_cast<int>( tempProtoType.m_RespawnPosList.size() );
				int iIndex = Math::Random::SimpleRandom(GetTickCount(), iRespawnPosCount);
				Position resapwnPos = tempProtoType.m_RespawnPosList[iIndex];
				resapwnPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 10) - 5;
				resapwnPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 10) - 5;
				return resapwnPos;
			}
		}

		++itr;
	}

	ERRLOG2(g_Log, "존의 기본 리스폰 위치를 찾을수 없습니다. Zone : %d,  Nation : %d",
			CServerSetup::GetInstance().GetServerZone(), cNation);

	return Position(0, 0, 0);
}

const Position CCharRespawnMgr::GetTownRespawnPos(unsigned char cZone, unsigned char cNation)
{
	RespawnProtoTypeMap::iterator itr = m_AllZoneTownList.begin();
	while (itr != m_AllZoneTownList.end())
	{
		RespawnProtoType& tempProtoType = itr->second;

		if (tempProtoType.m_cZone == cZone)
		{
			if (Creature::STATELESS == tempProtoType.m_cNation || cNation == tempProtoType.m_cNation)
			{
				int iRespawnPosCount = static_cast<int>( tempProtoType.m_RespawnPosList.size() );
				int iIndex = Math::Random::SimpleRandom(GetTickCount(), iRespawnPosCount);
				Position resapwnPos = tempProtoType.m_RespawnPosList[iIndex];
				resapwnPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
				resapwnPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
				return resapwnPos;
			}
		}

		++itr;
	}

	ERRLOG2(g_Log, "존의 기본 리스폰 위치를 찾을수 없습니다. Zone : %d,  Nation : %d", cZone, cNation);
	return Position(0, 0, 0);
}

const Position CCharRespawnMgr::GetRespawnPos(unsigned long dwTownID)
{
	RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.find(dwTownID);
	if (itr != m_ProtoTypeList.end())
	{
		RespawnProtoType& tempProtoType = itr->second;
		size_t iIndex = Math::Random::ComplexRandom(static_cast<int>(tempProtoType.m_RespawnPosList.size()));
		Position resapwnPos = tempProtoType.m_RespawnPosList[iIndex];
		resapwnPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 10) - 5;
		resapwnPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 10) - 5;
		return resapwnPos;
	}

	ERRLOG2(g_Log, "존의 해당 리스폰 위치를 찾을수 없습니다. Zone : %d,  dwTownID : %d",
			CServerSetup::GetInstance().GetServerZone(), dwTownID);

	return Position(0, 0, 0);
}

bool CCharRespawnMgr::SendRespawnInfo(unsigned long dwCID)
{
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpCharacter)
	{
		ERRLOG1(g_Log, "CID : 0x%08x 리스폰 하려는 캐릭터가 존재하지 않습니다.", dwCID);
		return false;
	}

    const unsigned short BufferSize = sizeof(PktRsInfoAck) + sizeof(RespawnArea) * PktRsInfoAck::MAX_RESPAWN_POINT;
	char szBuffer[BufferSize];

	unsigned short wBufferSize = 0;
	unsigned char cRespawnAreaNum = 0;		// 리스폰 포인트 수
	unsigned char cNation = lpCharacter->GetNation();
	RespawnArea* lpRespawnArea = reinterpret_cast<RespawnArea*>(szBuffer + sizeof(PktRsInfoAck));
	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpCharacter->GetGID());

	RespawnProtoTypeMap::iterator itr = m_ProtoTypeList.begin();
	while (itr != m_ProtoTypeList.end())
	{
		RespawnProtoType& tempProtoType = itr->second;
		
		if (0 != (tempProtoType.m_dwTownID & Castle::ABTOWN_BIT))
		{
			if( lpCharacter->GetAbilityValue(Skill::Type::AB_RESPAWN_EX))
			{
				// 자신의 국가가 속한 마을이라면 패킷에 추가한다.
				if (Creature::STATELESS == tempProtoType.m_cNation || cNation == tempProtoType.m_cNation)
				{
					lpRespawnArea->m_dwTownID = tempProtoType.m_dwTownID;
					lpRespawnArea->m_RespawnPos.fPointX = tempProtoType.m_RespawnPosList[0].m_fPointX;
					lpRespawnArea->m_RespawnPos.fPointY = tempProtoType.m_RespawnPosList[0].m_fPointY;
					lpRespawnArea->m_RespawnPos.fPointZ = tempProtoType.m_RespawnPosList[0].m_fPointZ;

					wBufferSize += sizeof(RespawnArea);
					++cRespawnAreaNum;
					++lpRespawnArea;
				}
			}
		}
		else if (0 != (tempProtoType.m_dwTownID & Castle::TOWN_BIT))
		{
			// 자신의 국가가 속한 마을이라면 패킷에 추가한다.
			if (Creature::STATELESS == tempProtoType.m_cNation || cNation == tempProtoType.m_cNation)
			{
				lpRespawnArea->m_dwTownID = tempProtoType.m_dwTownID;
				lpRespawnArea->m_RespawnPos.fPointX = tempProtoType.m_RespawnPosList[0].m_fPointX;
				lpRespawnArea->m_RespawnPos.fPointY = tempProtoType.m_RespawnPosList[0].m_fPointY;
				lpRespawnArea->m_RespawnPos.fPointZ = tempProtoType.m_RespawnPosList[0].m_fPointZ;

				wBufferSize += sizeof(RespawnArea);
				++cRespawnAreaNum;
				++lpRespawnArea;
			}
		}
		else if (lpGuild && Guild::JOIN_WAIT != lpGuild->GetTitle(dwCID) && 0 != (tempProtoType.m_dwTownID & Castle::CAMP_BIT))
		{
			// 진지라면 자기 길드의 곳이라면 패킷에 추가한다.
			if (lpGuild->GetGID() == tempProtoType.m_dwGID)
			{
				lpRespawnArea->m_dwTownID = tempProtoType.m_dwTownID;
				lpRespawnArea->m_RespawnPos.fPointX = tempProtoType.m_RespawnPosList[0].m_fPointX;
				lpRespawnArea->m_RespawnPos.fPointY = tempProtoType.m_RespawnPosList[0].m_fPointY;
				lpRespawnArea->m_RespawnPos.fPointZ = tempProtoType.m_RespawnPosList[0].m_fPointZ;

				wBufferSize += sizeof(RespawnArea);
				++cRespawnAreaNum;
				++lpRespawnArea;
			}
		}
		else if (0 != (tempProtoType.m_dwTownID & Castle::CASTLE_BIT))
		{
			// 자신과 같은 국적 수성/공성 리스폰 포인트라면 패킷에 추가한다.
			if (cNation == tempProtoType.m_cNation)
			{
				// 공성측 리스폰 포인트는 공성 시간에만 활성화 된다.
				bool bAddRespawnPoint = true;
				if (0 != (tempProtoType.m_dwTownID & Castle::SIEGE_BIT))
				{
					if (!CGameTimeMgr::GetInstance().IsSiegeWarTime())
					{
						bAddRespawnPoint = false;
					}
				}

				if (bAddRespawnPoint)
				{
					lpRespawnArea->m_dwTownID = tempProtoType.m_dwTownID;
					lpRespawnArea->m_RespawnPos.fPointX = tempProtoType.m_RespawnPosList[0].m_fPointX;
					lpRespawnArea->m_RespawnPos.fPointY = tempProtoType.m_RespawnPosList[0].m_fPointY;
					lpRespawnArea->m_RespawnPos.fPointZ = tempProtoType.m_RespawnPosList[0].m_fPointZ;

					wBufferSize += sizeof(RespawnArea);
					++cRespawnAreaNum;
					++lpRespawnArea;
				}
			}
		}
		else if (0 != (tempProtoType.m_dwTownID & Castle::STATUE_BIT))
		{
			// 다크 카나번 석상이라면 자신의 국가와 일치(종족과 일치)하면 패킷에 추가한다.
			if (Creature::STATELESS == tempProtoType.m_cNation || cNation == tempProtoType.m_cNation)
			{
				lpRespawnArea->m_dwTownID = tempProtoType.m_dwTownID;
				lpRespawnArea->m_RespawnPos.fPointX = tempProtoType.m_RespawnPosList[0].m_fPointX;
				lpRespawnArea->m_RespawnPos.fPointY = tempProtoType.m_RespawnPosList[0].m_fPointY;
				lpRespawnArea->m_RespawnPos.fPointZ = tempProtoType.m_RespawnPosList[0].m_fPointZ;

				wBufferSize += sizeof(RespawnArea);
				++cRespawnAreaNum;
				++lpRespawnArea;
			}
		}

		++itr;
	}

	// edith 2009.07.21 워프나 포탈 사용시 현재 좌표갱신하기 (불법이동 방지 검출 피하기 위해서)
	if(lpCharacter)
		lpCharacter->SetCurrentPos(lpRespawnArea->m_RespawnPos.fPointX, lpRespawnArea->m_RespawnPos.fPointY, lpRespawnArea->m_RespawnPos.fPointZ);

	PktRsInfoAck* lpRsInfoAck = reinterpret_cast<PktRsInfoAck*>(szBuffer);
	lpRsInfoAck->m_dwCharID = dwCID;
	lpRsInfoAck->m_cRsAreaNum = cRespawnAreaNum;
	lpRsInfoAck->m_wSize = wBufferSize;

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		CSendStream& SendStream = lpDispatch->GetSendStream();
		return SendStream.WrapCompress(szBuffer, sizeof(PktRsInfoAck) + wBufferSize, CmdCharRespawnInfo, 0, 0);
	}

	return false;
}

bool CCharRespawnMgr::SendRespawnAreaInfo(unsigned long dwCID, unsigned long dwTownID, bool bCount)
{
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpCharacter)
	{
		ERRLOG1(g_Log, "CID : 0x%08x 리스폰 하려는 캐릭터가 존재하지 않습니다.", dwCID);
		return false;
	}

	RespawnProtoTypeMap::iterator RespawnItr = m_ProtoTypeList.find(dwTownID);
	if (RespawnItr == m_ProtoTypeList.end())
	{
		ERRLOG1(g_Log, "TownID : 0x%08x 리스폰 지점이 존재하지 않습니다.", dwTownID);
		return false;
	}
	
	RespawnCharMap::iterator itr = m_CharList.find(dwTownID);
	if (itr != m_CharList.end())
	{
		RespawnCharList& tempCharList = itr->second;

		PktRsAreaInfoAck pktRsAreaInfoAck;
		pktRsAreaInfoAck.m_dwCharID = dwCID;
		pktRsAreaInfoAck.m_dwTownID = dwTownID;
		pktRsAreaInfoAck.m_bCount = bCount;
		pktRsAreaInfoAck.m_nWaitOrder = static_cast<unsigned short>(tempCharList.size());

		unsigned long dwSec = RESPAWN_DELAY_SEC;
		if (0 != tempCharList.size())
		{
			RespawnCharList::reverse_iterator CharInfoItr = tempCharList.rbegin();
			RespawnCharInfo& tempCharInfo = *CharInfoItr;
			dwSec += tempCharInfo.m_dwRemainSec;
		}

		// 시간 계산
		pktRsAreaInfoAck.m_cRemainHour = static_cast<unsigned char>(dwSec / SEC_PER_HOUR);
		pktRsAreaInfoAck.m_cRemainMin = static_cast<unsigned char>((dwSec / SEC_PER_MIN) / MIN_PER_HOUR);
		pktRsAreaInfoAck.m_cRemainSec = static_cast<unsigned char>((dwSec % SEC_PER_HOUR) % SEC_PER_MIN);

		char* szPacket = reinterpret_cast<char *>(&pktRsAreaInfoAck);
		if (PacketWrap::WrapCrypt(szPacket, sizeof(PktRsAreaInfoAck), CmdCharRespawnAreaInfo, 0, 0))
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				return lpDispatch->GetSendStream().PutBuffer(szPacket, sizeof(PktRsAreaInfoAck), CmdCharRespawnAreaInfo);
			}
		}
	}

	ERRLOG1(g_Log, "dwTownID : 0x%08x 해당 마을(성, 길드 요새, 다크 카나번 석상)이 리스폰 포인트로 존재하지 않습니다.", dwTownID);
	return false;
}