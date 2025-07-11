#include "stdafx.h"
#include "BGServerMgr.h"

#include <Utility/Math/Math.h>
#include <Creature/Character/Character.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/BGServerPacket.h>

#include <Network/Dispatch/GameClient/SendCharBGServer.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/BGServer/BGServerMap.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/SpellUtil.h>


using namespace VirtualArea;



CBGServerMgr::CBGServerMgr()
{
}


CBGServerMgr::~CBGServerMgr()
{
	DestroyBGServerMap();
}


CBGServerMgr& CBGServerMgr::GetInstance()
{
	static CBGServerMgr ms_this;
	return ms_this;
}

bool	CBGServerMgr::CreateBGServerMap()
{
	if (m_lstBGServerMap.size() != 0)
	{
		DestroyBGServerMap();
	}

	// Game Room 가상 맵 생성
	m_lstBGServerMap.push_back( new CBGServerMap( CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType("BG_FRAG"), 1 )	);
    m_lstBGServerMap.push_back( new CBGServerMap( CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType("BG_FRAG"), 2 )	);
	m_lstBGServerMap.push_back( new CBGServerMap( CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType("BG_FRAG"), 3 )	);
	m_lstBGServerMap.push_back( new CBGServerMap( CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType("BG_FRAG"), 4 )	);
	m_lstBGServerMap.push_back( new CBGServerMap( CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType("BG_FRAG"), 5 )	);
	m_lstBGServerMap.push_back( new CBGServerMap( CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType("BG_FRAG"), 6 )	);
	m_lstBGServerMap.push_back( new CBGServerMap( CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType("BG_FRAG"), 7 )	);
	m_lstBGServerMap.push_back( new CBGServerMap( CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType("BG_STATUE"), 8 )	);

	LoginAllMonster();
	GameAllStart();

	return true;
}


void	CBGServerMgr::DestroyBGServerMap()
{
	BGServerMapList::iterator pos = m_lstBGServerMap.begin();
	BGServerMapList::iterator end = m_lstBGServerMap.end();

	for(; pos != end; ++pos)
	{
		if (NULL != (*pos))
		{
			delete (*pos);
			(*pos) = NULL;
		}
	}

	m_lstBGServerMap.clear();
}


bool	CBGServerMgr::Enter(CCharacter* lpCharacter, unsigned short wMapIndex, unsigned char cMoveType)
{
	if (NULL == lpCharacter) { return false; }

	unsigned short wIndex = (wMapIndex & ~VirtualArea::BGSERVERMAP);
	if (0 == wIndex || wIndex > m_lstBGServerMap.size()) { return false; }

	CBGServerMap* lpBGServerMap = m_lstBGServerMap[wIndex - 1];
	if (NULL == lpBGServerMap) { return false; }

	unsigned short wError = lpBGServerMap->Enter(lpCharacter, cMoveType);
	if (PktBase::NO_SERVER_ERR == wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			if (false == GameClientSendPacket::SendCharBGServerMoveZone(lpDispatch->GetSendStream(), lpBGServerMap->GetVirtualZone(), cMoveType, wError))
			{
				lpBGServerMap->Leave(lpCharacter);
				ERRLOG3(g_Log, "CID:0x%08x 캐릭터가 가상 존(MapIndex : %d)에서 가상 존(MapIndex : %d)으로 이동하는데 실패하였습니다.",
						lpCharacter->GetCID(), lpCharacter->GetMapIndex(), wMapIndex);
				return false;
			}

			// 존 이동 (실제로는 MapIndex 와 좌표가 바뀔 뿐이다.)
			lpCharacter->SetMapIndex(wMapIndex);
			lpCharacter->MoveTo(lpBGServerMap->GetStartPosition(lpCharacter->GetRace()), false);

			// HIDE 명령 처리
			if (VirtualArea::TYPE_SPECTATOR == cMoveType)
			{
				lpCharacter->GetEnchantInfo().SetFlag(Skill::SpellID::Hide);
			}

			// 방 정보를 보내준다.
			// (지금은 방의 모든 사람에게 다 보내주지만, 구경꾼이 들어올경우에는 그 구경꾼에게만 보내주도록 수정할수 있다.)
			lpBGServerMap->SendMapInfo();
		}
		else
		{
			lpBGServerMap->Leave(lpCharacter);
			ERRLOG3(g_Log, "CID:0x%08x 캐릭터가 가상 존(MapIndex : %d)에서 가상 존(MapIndex : %d)으로 이동하는데 실패하였습니다.",
				lpCharacter->GetCID(), lpCharacter->GetMapIndex(), wMapIndex);
			return false;
		}
	}
	else
	{
		if (PktBase::SERVER_ERROR == wError)
		{
			ERRLOG3(g_Log, "CID:0x%08x 캐릭터가 가상 존(MapIndex : %d)에서 가상 존(MapIndex : %d)으로 이동하는데 실패하였습니다.",
				lpCharacter->GetCID(), lpCharacter->GetMapIndex(), wMapIndex);
		}
		else
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				return GameClientSendPacket::SendCharBGServerMoveZone(lpDispatch->GetSendStream(), 
					lpBGServerMap->GetVirtualZone(), cMoveType, wError);
			}
		}
	}

	return true;
}


bool	CBGServerMgr::Leave(CCharacter* lpCharacter)
{
	if (NULL == lpCharacter) return false;

	unsigned short wIndex = (lpCharacter->GetMapIndex() & ~VirtualArea::BGSERVERMAP);
	if (0 == wIndex || wIndex > m_lstBGServerMap.size()) return false;

	CBGServerMap* lpBGServerMap = m_lstBGServerMap[wIndex - 1];
	if (NULL == lpBGServerMap) return false;

	lpCharacter->SetMapIndex(0);
	Position RespawnPos(VirtualArea::RespawnPos[ lpCharacter->GetRace() ][ Math::Random::ComplexRandom(VirtualArea::MAX_LOBBY_RESPAWN_POS) ]);
	RespawnPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
	RespawnPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
	lpCharacter->MoveTo(RespawnPos, false);
	lpCharacter->GetEnchantInfo().ResetFlag(Skill::SpellID::Hide);
	lpBGServerMap->Leave(lpCharacter);

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		if (false == GameClientSendPacket::SendCharBGServerMoveZone(lpDispatch->GetSendStream(), SERVER_ID::BATTLE_SERVER, VirtualArea::TYPE_PLAYER, PktBase::NO_SERVER_ERR))
		{
			ERRLOG2(g_Log, "CID:0x%08x 캐릭터가 가상 존(MapIndex : %d)에서 나가는데 실패하였습니다.", lpCharacter->GetCID(), lpCharacter->GetMapIndex());
			return false;
		}

		// 플레이어가 나간경우 방 정보를 보내준다.
		if (lpBGServerMap->IsPlayer(lpCharacter))
		{
			lpBGServerMap->SendMapInfo();
		}
	}

	return true;
}


void	CBGServerMgr::Process()
{
	BGServerMapList::iterator pos = m_lstBGServerMap.begin();
	BGServerMapList::iterator end = m_lstBGServerMap.end();

	while (pos != end)
	{
		(*pos)->Process();
		++pos;
	}

	static unsigned long s_dwLastTime = 0;
	unsigned long dwCurrentTime = timeGetTime();
	if (dwCurrentTime - s_dwLastTime > VirtualArea::MILLISEC_PER_MINUTE)
	{
		s_dwLastTime = dwCurrentTime;
		SendMapListToAllCharacter();
	}
}


CVirtualArea* CBGServerMgr::GetVirtualArea(unsigned short wMapIndex)
{
	unsigned short wIndex = (wMapIndex & ~VirtualArea::BGSERVERMAP);

	if (0 == wIndex || wIndex > m_lstBGServerMap.size())
		return NULL;

	return reinterpret_cast<CVirtualArea*>(m_lstBGServerMap[wIndex - 1]);
}


bool	CBGServerMgr::SendMapList(CCharacter* lpCharacter)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	if (NULL == lpCharacter)
	{
		return false;
	}

	const int   MAX_BUFFER = sizeof(PktBGServerMapList) + PktBGServerMapList::MAX_MAP_NUM * sizeof(BGServerMapInfoNode);
	char        szBuffer[MAX_BUFFER];

	PktBGServerMapList*		lpPktBGSMLAck = reinterpret_cast<PktBGServerMapList *>(szBuffer);
	BGServerMapInfoNode*	lpMapInfoNode = reinterpret_cast<BGServerMapInfoNode *>(lpPktBGSMLAck + 1);

	lpPktBGSMLAck->m_bAll = true;
	lpPktBGSMLAck->m_cMapInfoNodeNum = 0;

	for (unsigned char cIndex = 0; cIndex < PktBGServerMapList::MAX_MAP_NUM && cIndex < m_lstBGServerMap.size();
		++cIndex, ++lpMapInfoNode, ++lpPktBGSMLAck->m_cMapInfoNodeNum)
	{
		CBGServerMap* lpBGServerMap = m_lstBGServerMap[cIndex];
		if (NULL != lpBGServerMap)
		{
			lpMapInfoNode->m_bPlaying				= lpBGServerMap->IsPlaying();
			lpMapInfoNode->m_cMapType				= lpBGServerMap->GetMapInfo().m_cMapType;
			lpMapInfoNode->m_cMaxCharNumOfNation	= lpBGServerMap->GetMapInfo().m_cMaxCharNumOfNation;
			lpMapInfoNode->m_wTargetScore			= lpBGServerMap->GetMapInfo().m_wTargetScore;
			lpMapInfoNode->m_cRemainMin				= (lpBGServerMap->IsPlaying() == true) ? lpBGServerMap->GetMapInfo().m_cRemainPlayMin : lpBGServerMap->GetMapInfo().m_cRemainRestMin;

			lpMapInfoNode->m_cCurrentCharNum[CClass::HUMAN] = lpBGServerMap->GetMapInfo().m_cCurrentCharNum[CClass::HUMAN];
			lpMapInfoNode->m_cCurrentCharNum[CClass::AKHAN] = lpBGServerMap->GetMapInfo().m_cCurrentCharNum[CClass::AKHAN];
			lpMapInfoNode->m_wScore[CClass::HUMAN]	= lpBGServerMap->GetMapInfo().m_wScore[CClass::HUMAN];
			lpMapInfoNode->m_wScore[CClass::AKHAN]	= lpBGServerMap->GetMapInfo().m_wScore[CClass::AKHAN];
		}
	}

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		return lpDispatch->GetSendStream().WrapCompress(szBuffer, sizeof(PktBGServerMapList) + 
			lpPktBGSMLAck->m_cMapInfoNodeNum * sizeof(BGServerMapInfoNode), CmdBGServerMapList, 0, 0);
	}

	return true;
}


bool	CBGServerMgr::SendResultList(CCharacter* lpCharacter)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	if (NULL == lpCharacter)
	{
		return false;
	}

	const int   MAX_BUFFER = sizeof(PktBGServerResultList) + PktBGServerResultList::MAX_MAP_NUM * sizeof(BGServerResultInfoNode);
	char        szBuffer[MAX_BUFFER];

	PktBGServerResultList*	lpPktBGSRLAck = reinterpret_cast<PktBGServerResultList *>(szBuffer);
	BGServerResultInfoNode*	lpResultInfoNode = reinterpret_cast<BGServerResultInfoNode *>(lpPktBGSRLAck + 1);

	lpPktBGSRLAck->m_bAll = true;
	lpPktBGSRLAck->m_cResultInfoNodeNum = 0;

	for (unsigned char cIndex = 0; cIndex < PktBGServerResultList::MAX_MAP_NUM && cIndex < m_lstBGServerMap.size();
		++cIndex, ++lpResultInfoNode, ++lpPktBGSRLAck->m_cResultInfoNodeNum)
	{
		CBGServerMap* lpBGServerMap = m_lstBGServerMap[cIndex];
		if (NULL != lpBGServerMap)
		{
			lpResultInfoNode->m_cWinRace			  = lpBGServerMap->GetResultInfo().m_cWinRace;
			lpResultInfoNode->m_cPlayMin			  = 0;
			lpResultInfoNode->m_wAward				  = 0;
			lpResultInfoNode->m_wScore[CClass::HUMAN] = lpBGServerMap->GetResultInfo().m_wScore[CClass::HUMAN];
			lpResultInfoNode->m_wScore[CClass::AKHAN] = lpBGServerMap->GetResultInfo().m_wScore[CClass::AKHAN];
		}
	}

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		return lpDispatch->GetSendStream().WrapCompress(szBuffer, sizeof(PktBGServerResultList) + 
			lpPktBGSRLAck->m_cResultInfoNodeNum * sizeof(BGServerResultInfoNode), CmdBGServerResultList, 0, 0);
	}

	return true;
}

bool	CBGServerMgr::SendMapListToAllCharacter()
{
	PERFORMANCE_CHECK(FunctionTimingCheck);
	
	if (0 == CCreatureManager::GetInstance().GetCharacterNum()) return true;

	const int   MAX_BUFFER = sizeof(PktBGServerMapList) + PktBGServerMapList::MAX_MAP_NUM * sizeof(BGServerMapInfoNode);
	char        szSrcBuffer[MAX_BUFFER], szDstBuffer[MAX_BUFFER];

	PktBGServerMapList*		lpPktBGSMLAck = reinterpret_cast<PktBGServerMapList *>(szSrcBuffer);
	BGServerMapInfoNode*	lpMapInfoNode = reinterpret_cast<BGServerMapInfoNode *>(lpPktBGSMLAck + 1);

	lpPktBGSMLAck->m_bAll = true;
	lpPktBGSMLAck->m_cMapInfoNodeNum = 0;

	for (unsigned char cIndex = 0; cIndex < PktBGServerMapList::MAX_MAP_NUM && cIndex < m_lstBGServerMap.size();
		++cIndex, ++lpMapInfoNode, ++lpPktBGSMLAck->m_cMapInfoNodeNum)
	{
		CBGServerMap* lpBGServerMap = m_lstBGServerMap[cIndex];
		if (NULL != lpBGServerMap)
		{
			lpMapInfoNode->m_bPlaying				= lpBGServerMap->IsPlaying();
			lpMapInfoNode->m_cMapType				= lpBGServerMap->GetMapInfo().m_cMapType;
			lpMapInfoNode->m_cMaxCharNumOfNation	= lpBGServerMap->GetMapInfo().m_cMaxCharNumOfNation;
			lpMapInfoNode->m_wTargetScore			= lpBGServerMap->GetMapInfo().m_wTargetScore;
			lpMapInfoNode->m_cRemainMin				= (lpBGServerMap->IsPlaying() == true) ? lpBGServerMap->GetMapInfo().m_cRemainPlayMin : lpBGServerMap->GetMapInfo().m_cRemainRestMin;

			lpMapInfoNode->m_cCurrentCharNum[CClass::HUMAN] = lpBGServerMap->GetMapInfo().m_cCurrentCharNum[CClass::HUMAN];
			lpMapInfoNode->m_cCurrentCharNum[CClass::AKHAN] = lpBGServerMap->GetMapInfo().m_cCurrentCharNum[CClass::AKHAN];
			lpMapInfoNode->m_wScore[CClass::HUMAN]	= lpBGServerMap->GetMapInfo().m_wScore[CClass::HUMAN];
			lpMapInfoNode->m_wScore[CClass::AKHAN]	= lpBGServerMap->GetMapInfo().m_wScore[CClass::AKHAN];
		}
	}

	unsigned long dwDstLength = MAX_BUFFER;
	unsigned short wSrcLength = sizeof(PktBGServerMapList) + lpPktBGSMLAck->m_cMapInfoNodeNum * sizeof(BGServerMapInfoNode);

	if (PacketWrap::WrapCompress(szDstBuffer, dwDstLength, 
        szSrcBuffer, wSrcLength, CmdBGServerMapList, 0, 0))
	{
		// 대기실에 있는 사람들에게만 방정보를 전송해준다.
		CCreatureManager::GetInstance().SendAllCharacter(
            szDstBuffer, dwDstLength, CmdBGServerMapList, false);
	}

	return true;
}


bool	CBGServerMgr::LoginAllMonster()
{
	for (size_t nIndex=0; nIndex<m_lstBGServerMap.size(); ++nIndex)
	{
		if (m_lstBGServerMap[nIndex])
		{
			const VirtualArea::ProtoType* lpProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType( m_lstBGServerMap[nIndex]->GetVID() );
			if (lpProtoType && '\0' != lpProtoType->m_szArrangementFileName[0])
			{
				m_lstBGServerMap[nIndex]->CreateVirtualMonsterManager();
				unsigned short wMapIndex = static_cast<unsigned short>(VirtualArea::BGSERVERMAP | (nIndex + 1));
				CCellManager::GetInstance().LoginMonster(lpProtoType->m_szArrangementFileName, wMapIndex);
			}
		}
	}

	return true;
}

void	CBGServerMgr::GameAllStart()
{
	std::for_each(m_lstBGServerMap.begin(), m_lstBGServerMap.end(), std::mem_fun<bool, CBGServerMap>(&CBGServerMap::GameStart));
}


void	CBGServerMgr::ProcessAllMonster()
{
	BGServerMapList::iterator pos = m_lstBGServerMap.begin();
	BGServerMapList::iterator end = m_lstBGServerMap.end();

	while (pos != end)
	{
		if (NULL != (*pos))
		{
			(*pos)->ProcessAllMonster();
		}

		++pos;
	}
}

void	CBGServerMgr::ProcessMonsterRegenHPAndMP()
{
	BGServerMapList::iterator pos = m_lstBGServerMap.begin();
	BGServerMapList::iterator end = m_lstBGServerMap.end();

	while (pos != end)
	{
		if (NULL != (*pos))
		{
			(*pos)->ProcessMonsterRegenHPAndMP();
		}

		++pos;
	}
}

void	CBGServerMgr::ProcessSummonMonsterDead()
{
	BGServerMapList::iterator pos = m_lstBGServerMap.begin();
	BGServerMapList::iterator end = m_lstBGServerMap.end();

	while (pos != end)
	{
		if (NULL != (*pos))
		{
			(*pos)->ProcessSummonMonsterDead();
		}

		++pos;
	}
}

void	CBGServerMgr::ProcessDeleteItem()
{
	BGServerMapList::iterator pos = m_lstBGServerMap.begin();
	BGServerMapList::iterator end = m_lstBGServerMap.end();

	while (pos != end)
	{
		if (NULL != (*pos))
		{
			(*pos)->ProcessDeleteItem();
		}

		++pos;
	}
}

bool	CBGServerMgr::ProcessAllCellPrepareBroadCast()
{
	BGServerMapList::iterator pos = m_lstBGServerMap.begin();
	BGServerMapList::iterator end = m_lstBGServerMap.end();

	while (pos != end)
	{
		if (NULL != (*pos))
		{
			(*pos)->ProcessAllCellPrepareBroadCast();
		}

		++pos;
	}

	return true;
}

bool	CBGServerMgr::ProcessAllCellBroadCast(unsigned long dwCurrentPulse)
{
	BGServerMapList::iterator pos = m_lstBGServerMap.begin();
	BGServerMapList::iterator end = m_lstBGServerMap.end();

	while (pos != end)
	{
		if (NULL != (*pos))
		{
			(*pos)->ProcessAllCellBroadCast(dwCurrentPulse);
		}

		++pos;
	}

	return true;
}