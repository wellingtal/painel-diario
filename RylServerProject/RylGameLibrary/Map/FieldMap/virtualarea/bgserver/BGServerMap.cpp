
#include "stdafx.h"

#include <mmsystem.h>

#include <Utility/Math/Math.h>

#include <Creature/Character/Character.h>
#include <Creature/Monster/PatternMonster.h>
#include <Creature/Monster/VirtualMonsterMgr.h>

#include <Network/Dispatch/GameClient/SendCharBGServer.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/BGServerPacket.h>

#include "BGServerMap.h"

using namespace VirtualArea;

CBGServerMap::CBGServerMap(const VirtualArea::ProtoType* lpProtoType, unsigned short wMapNumber)
: CVirtualArea(lpProtoType, VirtualArea::BGSERVERMAP | wMapNumber), m_MapInfo(lpProtoType->m_cMapType), m_cStatus(START_WAITING)
{
}


CBGServerMap::~CBGServerMap(void)
{
}



unsigned short	CBGServerMap::Enter(CCharacter* lpCharacter, unsigned char cMoveType)
{
	if (false == IsPlaying())
	{
		return PktBase::SERVER_ERROR;
	}

	switch (cMoveType)
	{
		case TYPE_PLAYER:
		{
			return AddCharacter(lpCharacter);
		}

		case TYPE_SPECTATOR:
		{
			return AddSpectator(lpCharacter);
		}
	}

	return PktBase::SERVER_ERROR;
}


bool	CBGServerMap::Leave(CCharacter* lpCharacter)
{
	if (NULL == lpCharacter) return false;

	char szNation[8];
	if (lpCharacter->GetRace() == CClass::HUMAN) strcpy(szNation, "HUMAN");
	else strcpy(szNation, "AKHAN");

	CharacterList::iterator pos = std::find(m_CharacterList.begin(), m_CharacterList.end(), lpCharacter);
	if (pos != m_CharacterList.end())
	{
		--m_MapInfo.m_cCurrentCharNum[ lpCharacter->GetRace() ];
		m_CharacterList.erase(pos);

		m_MapInfo.m_PersonalInfoMap.erase( lpCharacter->GetCID() );

		// 로그
		DETLOG5(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x 캐릭터(%s, %s)가 게임에서 나가셨습니다.",
				(m_wMapIndex & ~VirtualArea::BGSERVERMAP), GetMapTypeName(),
				lpCharacter->GetCID(), lpCharacter->GetCharacterName(), szNation);

		return true;
	}

	pos = std::find(m_SpectatorList.begin(), m_SpectatorList.end(), lpCharacter);
	if (pos != m_SpectatorList.end())
	{
		m_SpectatorList.erase(pos);

		// 로그
		DETLOG5(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x 캐릭터(%s, %s)가 게임에서 나가셨습니다.",
			(m_wMapIndex & ~VirtualArea::BGSERVERMAP), GetMapTypeName(),
			lpCharacter->GetCID(), lpCharacter->GetCharacterName(), szNation);

		return true;
	}

	return false;
}


unsigned short	CBGServerMap::AddCharacter(CCharacter* lpCharacter)
{
	if (NULL == lpCharacter)
	{
		return PktBase::SERVER_ERROR;
	}

	if (m_MapInfo.m_cMaxCharNumOfNation <= m_MapInfo.m_cCurrentCharNum[ lpCharacter->GetRace() ])
	{
		return PktBGServerMoveZone::FAIL_FULL_MAP;
	}

	// 진입 인원 제한 걸기
	if (m_MapInfo.m_cCurrentCharNum[ lpCharacter->GetRace() ] >= 10)
	{
		if ( (lpCharacter->GetRace() == CClass::HUMAN && m_MapInfo.m_cCurrentCharNum[CClass::HUMAN] >= m_MapInfo.m_cCurrentCharNum[CClass::AKHAN] * 1.5) ||
			 (lpCharacter->GetRace() == CClass::AKHAN && m_MapInfo.m_cCurrentCharNum[CClass::AKHAN] >= m_MapInfo.m_cCurrentCharNum[CClass::HUMAN] * 1.5) )
		{
			return PktBGServerMoveZone::FAIL_FIX_RATE;
		}
	}

	++m_MapInfo.m_cCurrentCharNum[ lpCharacter->GetRace() ];
	m_CharacterList.push_back(lpCharacter);

	// 들어왔을때 남은 경기시간을 저장해둔다. (남은 경기시간에 따른 차등 점수를 주기위해서...)
	MapInfo::PersonalInfo personalInfo(m_MapInfo.m_cRemainPlayMin);
	m_MapInfo.m_PersonalInfoMap.insert( make_pair(lpCharacter->GetCID(), personalInfo) );

	// 로그 남기기
	char szNation[8];
	if (lpCharacter->GetRace() == CClass::HUMAN) 
	{
		strcpy(szNation, "HUMAN");
	}
	else 
	{
		strcpy(szNation, "AKHAN");
	}

	DETLOG5(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x 캐릭터(%s, %s)가 Player 로 들어왔습니다.", 
		(m_wMapIndex & ~VirtualArea::BGSERVERMAP), GetMapTypeName(), 
		lpCharacter->GetCID(), lpCharacter->GetCharacterName(), szNation);

	return PktBase::NO_SERVER_ERR;
}


unsigned short	CBGServerMap::AddSpectator(CCharacter* lpSpectator)
{
	if (NULL == lpSpectator)
	{
		return PktBase::SERVER_ERROR;
	}

	m_SpectatorList.push_back(lpSpectator);

	// 로그 남기기
	char szNation[8];
	if (lpSpectator->GetRace() == CClass::HUMAN) strcpy(szNation, "HUMAN");
	else strcpy(szNation, "AKHAN");

	DETLOG5(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x 캐릭터(%s, %s)가 Spectator 로 들어왔습니다.",
			(m_wMapIndex & ~VirtualArea::BGSERVERMAP), GetMapTypeName(),
			lpSpectator->GetCID(), lpSpectator->GetCharacterName(), szNation);

	return PktBase::NO_SERVER_ERR;
}

bool	CBGServerMap::IsPlayer(CCharacter* lpCharacter)
{
	if (NULL == lpCharacter) return false;

	CharacterList::iterator pos = std::find(m_CharacterList.begin(), m_CharacterList.end(), lpCharacter);
	if (pos != m_CharacterList.end()) return true;

	return false;
}

bool	CBGServerMap::IsSpectator(CCharacter* lpCharacter)
{
	if (NULL == lpCharacter) return false;

	CharacterList::iterator pos = std::find(m_SpectatorList.begin(), m_SpectatorList.end(), lpCharacter);
	if (pos != m_SpectatorList.end()) return true;

	return false;
}

// 모두 대기실로 이동
bool	CBGServerMap::AllRespawn()
{
	if (0 == m_CharacterList.size() && 0 == m_SpectatorList.size()) return true;

	bool bResult = true;
	CharacterList::iterator pos = m_CharacterList.begin();
	CharacterList::iterator end = m_CharacterList.end();

	while (pos != end)
	{
		if (NULL == (*pos)) continue;

		CGameClientDispatch* lpDispatch = (*pos)->GetDispatcher();
		if (NULL != lpDispatch)
		{
			if (false == GameClientSendPacket::SendCharBGServerMoveZone(lpDispatch->GetSendStream(), SERVER_ID::BATTLE_SERVER, VirtualArea::TYPE_PLAYER, PktBase::NO_SERVER_ERR))
			{
				ERRLOG2(g_Log, "CID:0x%08x 캐릭터가 가상 존(MapIndex : %d)에서 나가는데 실패하였습니다.", (*pos)->GetCID(), (*pos)->GetMapIndex());
				bResult = false;
			}

			Position RespawnPos(VirtualArea::RespawnPos[ (*pos)->GetRace() ][ Math::Random::ComplexRandom(VirtualArea::MAX_LOBBY_RESPAWN_POS) ]);
			RespawnPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
			RespawnPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;

			(*pos)->SetMapIndex(0);
			(*pos)->GetEnchantInfo().ResetFlag(Skill::SpellID::Hide);
			(*pos)->MoveTo(RespawnPos, false);
		}
		else
		{
			ERRLOG2(g_Log, "CID:0x%08x 캐릭터가 가상 존(MapIndex : %d)에서 나가는데 실패하였습니다.", (*pos)->GetCID(), (*pos)->GetMapIndex());
			bResult = false;
		}

		++pos;
	}

	// 스펙테이터 처리
	pos = m_SpectatorList.begin();
	end = m_SpectatorList.end();

	while (pos != end)
	{
		if (NULL == (*pos)) continue;

		CGameClientDispatch* lpDispatch = (*pos)->GetDispatcher();
		if (NULL != lpDispatch)
		{
			if (false == GameClientSendPacket::SendCharBGServerMoveZone(lpDispatch->GetSendStream(), SERVER_ID::BATTLE_SERVER, VirtualArea::TYPE_PLAYER, PktBase::NO_SERVER_ERR))
			{
				ERRLOG2(g_Log, "CID:0x%08x 캐릭터가 가상 존(MapIndex : %d)에서 나가는데 실패하였습니다.", (*pos)->GetCID(), (*pos)->GetMapIndex());
				bResult = false;
			}

			Position RespawnPos(VirtualArea::RespawnPos[ (*pos)->GetRace() ][ Math::Random::ComplexRandom(VirtualArea::MAX_LOBBY_RESPAWN_POS) ]);
			RespawnPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
			RespawnPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;

			(*pos)->SetMapIndex(0);
			(*pos)->GetEnchantInfo().ResetFlag(Skill::SpellID::Hide);
			(*pos)->MoveTo(RespawnPos, false);
		}
		else
		{
			ERRLOG2(g_Log, "CID:0x%08x 캐릭터가 가상 존(MapIndex : %d)에서 나가는데 실패하였습니다.", (*pos)->GetCID(), (*pos)->GetMapIndex());
			bResult = false;
		}

		++pos;
	}

	// 리스트 클리어
	m_CharacterList.clear();
	m_SpectatorList.clear();
	
	return bResult;
}




void	CBGServerMap::Process()
{
	unsigned long dwCurrentTime = timeGetTime();
    int nMin = 0, nSec = 0;

	if (m_dwRemainTime < dwCurrentTime) nMin = 0;
	else
	{
		nMin = (m_dwRemainTime - dwCurrentTime) / MILLISEC_PER_MINUTE;
		nSec = (m_dwRemainTime - dwCurrentTime) % MILLISEC_PER_MINUTE;
		if (nSec > 0) ++nMin;
	}
	
	switch (m_cStatus)
	{
		// 아직 Start 되지 않은 상태라면 아무것도 하지 않는다.
		case START_WAITING: return;

		case GAME_PLAYING:
		{
			// 1분에 한번씩 남은 시간을 보내준다.
			if (nMin < m_MapInfo.m_cRemainPlayMin)
			{
				// 남은 시간 갱신
				m_MapInfo.m_cRemainPlayMin = static_cast<unsigned char>(nMin);

				// 방 정보 전송
				SendMapInfo();
			}

			// 제한 시간이 다 되었다면,
			if (0 == m_MapInfo.m_cRemainPlayMin)
			{
				// 승패를 체크 및 결과 저장
				m_ResultInfo.m_cWinRace = RuleCheck(true);
				std::copy(&m_MapInfo.m_wScore[0], &m_MapInfo.m_wScore[CClass::MAX_RACE], &m_ResultInfo.m_wScore[0]);

				// 결과 전송
				// 이긴 종족, 혹은 무승부 캐릭터들에게 보상을 준다.
				SendResultInfo();
			}
			else
			{
				m_ResultInfo.m_cWinRace = RuleCheck();

				// 한 종족이 목표 점수를 달성했을 경우
				if (CClass::MAX_RACE != m_ResultInfo.m_cWinRace)
				{
					// 결과 점수 저장
					std::copy(&m_MapInfo.m_wScore[0], &m_MapInfo.m_wScore[CClass::MAX_RACE], &m_ResultInfo.m_wScore[0]);

					// 결과 전송
					// 이긴 종족의 캐릭터들에게 보상을 준다.
					SendResultInfo();
				}
			}

			break;
		}

		case GAME_RESTING:
		{
			// 남은 시간 갱신
			m_MapInfo.m_cRemainRestMin = static_cast<unsigned char>(nMin);

			// 쉬는 시간이 다 되었다면,
			if (0 == m_MapInfo.m_cRemainRestMin)
			{
				GameStart();
			}

			break;
		}

		case MOVEZONE_WAITING:
		{
			// 존이동 시킬 시간이 다 되었다면,
			if (m_dwRemainTime <= timeGetTime())
			{
				SetStatus(GAME_RESTING);
				m_dwRemainTime = timeGetTime() + m_MapInfo.m_cRestMin * MILLISEC_PER_MINUTE;

				// 캐릭터들을 존 이동 시킨다. (로비로 리스폰)
				AllRespawn();

				// Item을 모두 지운다.
				DeleteAllItem();

				// 방 정보 초기화
				m_MapInfo.Initialize();
			}

			break;
		}
	}
}


bool	CBGServerMap::GameStart()						// 게임을 시작한다.
{
	m_dwRemainTime = timeGetTime() + m_MapInfo.m_cLimitMin * MILLISEC_PER_MINUTE;
	InitializeGameObject();

	SetStatus(GAME_PLAYING);
	return true;
}


bool	CBGServerMap::InitializeGameObject()
{
	m_CharacterList.clear();
	m_SpectatorList.clear();

	m_MapInfo.Initialize();
	m_ResultInfo.Initialize();

	if (STATUE == m_MapInfo.m_cMapType && NULL != GetMonsterManager())
	{
		// 모든 석상을 다 죽이고, 아무것도 소환하지 않는다.
		CVirtualMonsterMgr::MonsterMap::iterator pos = m_pVirtualMonsterMgr->GetMonsterMap().begin();
		CVirtualMonsterMgr::MonsterMap::iterator end = m_pVirtualMonsterMgr->GetMonsterMap().end();

		while (pos != end)
		{
			CMonster* lpMonster = pos->second;
			if (NULL == lpMonster) continue;

			CStatue* lpStatue = lpMonster->DowncastToStatue();
			if (NULL != lpStatue)
			{
				lpStatue->Rest();
			}

			++pos;
		}


		// 휴먼, 중립, 아칸 석상을 순서대로 소환한다.
		enum { HUMAN_STATUE = 0, NUETRALITY_STATUE1 = 1, AKHAN_STATUE = 2, NUETRALITY_STATUE2 = 3, NUETRALITY_STATUE3 = 4, NONE = 100 };
		DWORD dwKind, dwOldKind = NONE;

		pos = m_pVirtualMonsterMgr->GetMonsterMap().begin();
		end = m_pVirtualMonsterMgr->GetMonsterMap().end();

		while (pos != end)
		{
			CMonster* lpMonster = pos->second;
			if (NULL == lpMonster) continue;

			CStatue* lpStatue = lpMonster->DowncastToStatue();
			if (NULL != lpStatue)
			{
				// 석상 초기화 시키기
				// !!주의!! 스크립트와 관련이 있다. 순서주의
				// 종류를 얻어온다. (HUMAN_STATUE = 0, NUETRALITY_STATUE1 = 1, AKHAN_STATUE = 2, NUETRALITY_STATUE2 = 3, NUETRALITY_STATUE3 = 4)
				dwKind = (lpStatue->GetCID() & ~(Creature::MONSTER_BIT | Creature::MONSTER_KIND_BIT)) >> 16;
				if (dwOldKind != dwKind)
				{
					switch (dwKind)
					{
					case HUMAN_STATUE:
						lpStatue = lpStatue->GetLinkStatue(MonsterInfo::BG_STATUE_HUMAN_COMPLETE1);
						lpStatue->InitMonster(lpStatue->GetOriginalPos());
						break;

					case NUETRALITY_STATUE1:
					case NUETRALITY_STATUE2:
					case NUETRALITY_STATUE3:
						lpStatue = lpStatue->GetLinkStatue(MonsterInfo::BG_STATUE_NEUTRALITY1);
						lpStatue->InitMonster(lpStatue->GetOriginalPos());
						break;

					case AKHAN_STATUE:
						lpStatue = lpStatue->GetLinkStatue(MonsterInfo::BG_STATUE_AKHAN_COMPLETE1);
						lpStatue->InitMonster(lpStatue->GetOriginalPos());
						break;
					}

					dwOldKind = dwKind;
				}
			}

			++pos;
		}

		// 석상 점령전일때의 초기 점수 계산
		CalculateScore();

		// 로그 남기기
		DETLOG4(g_Log, "Battle Server Log :: (Channel : %d, %s) - 석상전을 초기화 합니다. (현재 Score - HM: %d  AK: %d)",
				(m_wMapIndex & ~VirtualArea::BGSERVERMAP), GetMapTypeName(),
				m_MapInfo.m_wScore[CClass::HUMAN], m_MapInfo.m_wScore[CClass::AKHAN]);
	}

	return true;
}



unsigned char	CBGServerMap::RuleCheck(bool bTimeout)	// 룰을 체크해서 이긴종족을 리턴
{
	unsigned char cWinNation = CClass::MAX_RACE;

	// 시간이 다 지났다면, 혹은 운영자 명령으로 게임을 중지 시킬경우
	if (true == bTimeout)
	{
		if (m_MapInfo.m_wScore[CClass::HUMAN] > m_MapInfo.m_wScore[CClass::AKHAN])
		{
			cWinNation = CClass::HUMAN;
		}
		else if (m_MapInfo.m_wScore[CClass::HUMAN] < m_MapInfo.m_wScore[CClass::AKHAN])
		{
			cWinNation = CClass::AKHAN;
		}
		else
		{
			// 이경우의 CClass::MAX_RACE 은 무승부이다.
		}

		SetStatus(MOVEZONE_WAITING);
		m_dwRemainTime = timeGetTime() + MOVEZONE_WAIT_TIME;
	}
	else
	{
		// 목표 점수에 도달했는가 체크
		if (m_MapInfo.m_wScore[CClass::HUMAN] >= m_MapInfo.m_wTargetScore)
		{
			cWinNation = CClass::HUMAN;

			SetStatus(MOVEZONE_WAITING);
			m_dwRemainTime = timeGetTime() + MOVEZONE_WAIT_TIME;
		}
		else if (m_MapInfo.m_wScore[CClass::AKHAN] >= m_MapInfo.m_wTargetScore)
		{
			cWinNation = CClass::AKHAN;

			SetStatus(MOVEZONE_WAITING);
			m_dwRemainTime = timeGetTime() + MOVEZONE_WAIT_TIME;
		}

		// 이경우의 CClass::MAX_RACE 은 게임이 끝나지 않은 상태이다.
	}

	return cWinNation;
}


void	CBGServerMap::AwardToWinner()
{
    unsigned short wAwardBase, wRealPlayMin;
	unsigned short wPlayMin = m_MapInfo.m_cLimitMin - m_MapInfo.m_cRemainPlayMin;
	if (0 == wPlayMin) wPlayMin = 1;

	CharacterList winnerList;
	CharacterList::iterator pos;
	CharacterList::iterator end;

	if (CClass::MAX_RACE == m_ResultInfo.m_cWinRace)
	{
		wAwardBase = m_MapInfo.m_cLimitMin * MILEAGE_PER_MINUTE_FOR_DRAW;
		
		pos = m_CharacterList.begin();
		end = m_CharacterList.end();
	}
	else
	{
		wAwardBase = m_MapInfo.m_cLimitMin * MILEAGE_PER_MINUTE_FOR_WIN;

		FindWinner(winnerList);
		pos = winnerList.begin();
		end = winnerList.end();
	}

	
	while (pos != end)
	{
		if (NULL != (*pos))
		{
			MapInfo::PersonalInfoMap::iterator which = m_MapInfo.m_PersonalInfoMap.find( (*pos)->GetCID() );
			if (which != m_MapInfo.m_PersonalInfoMap.end())
			{
				wRealPlayMin = which->second.m_cEnteringMin - m_MapInfo.m_cRemainPlayMin;
				unsigned short wAward = wAwardBase * wRealPlayMin / wPlayMin;

				(*pos)->SetMileage((*pos)->GetMileage() + wAward);

				CGameClientDispatch* lpDispatch = (*pos)->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), (*pos), 
						"", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);
				}
			}
		}

		++pos;
	}
}


void	CBGServerMap::KillChar(unsigned long dwDeadCID, CCharacter* lpOffencer)
{
	if (NULL == lpOffencer) return;

	AddScore(lpOffencer->GetRace(), FRAG_SCORE);

	// Kill 정보 업데이트
	UpdateKillInfo(dwDeadCID, lpOffencer->GetCID());

	// 방 정보 전송
	SendMapInfo();
}



void	CBGServerMap::UpdateKillInfo(unsigned long dwDeadCID, unsigned long dwKillerCID)
{
	MapInfo::PersonalInfoMap::iterator pos = m_MapInfo.m_PersonalInfoMap.find( dwDeadCID );
	if (pos != m_MapInfo.m_PersonalInfoMap.end()) ++pos->second.m_cKilled;

	pos = m_MapInfo.m_PersonalInfoMap.find( dwKillerCID );
	if (pos != m_MapInfo.m_PersonalInfoMap.end()) ++pos->second.m_cKill;
}


void	CBGServerMap::AddScore(unsigned char cNation, short wScore)
{
	if (cNation >= CClass::MAX_CLASS) return;
	m_MapInfo.m_wScore[cNation] += wScore;
}


void	CBGServerMap::CalculateScore()					// 석상 점령전일때 점수 계산
{
	if (STATUE == m_MapInfo.m_cMapType && NULL != GetMonsterManager())
	{
		std::fill_n(m_MapInfo.m_wScore, int(CClass::MAX_RACE), 0);

		CVirtualMonsterMgr::MonsterMap::iterator pos = m_pVirtualMonsterMgr->GetMonsterMap().begin();
		CVirtualMonsterMgr::MonsterMap::iterator end = m_pVirtualMonsterMgr->GetMonsterMap().end();

		while (pos != end)
		{
			CMonster* lpMonster = pos->second;
            if (NULL != lpMonster && lpMonster->GetCurrentState() != STATE_ID_DIE)
			{
				switch (lpMonster->GetCID() & Creature::MONSTER_KIND_BIT)
				{
				case MonsterInfo::BG_STATUE_HUMAN_LOADING1:
					{
						AddScore(CClass::HUMAN, FRIENDLY_LOADING_STATUE_SCORE);
						break;
					}

				case MonsterInfo::BG_STATUE_HUMAN_COMPLETE1:
					{
						AddScore(CClass::HUMAN, FRIENDLY_STATUE_SCORE);
						break;
					}

				case MonsterInfo::BG_STATUE_AKHAN_LOADING1:
					{
						AddScore(CClass::AKHAN, FRIENDLY_LOADING_STATUE_SCORE);
						break;
					}

				case MonsterInfo::BG_STATUE_AKHAN_COMPLETE1:
					{
						AddScore(CClass::AKHAN, FRIENDLY_STATUE_SCORE);
						break;
					}

				case MonsterInfo::BG_STATUE_NEUTRALITY1:
					{
						AddScore(CClass::AKHAN, NEUTRALITY_STATUE_SCORE);
						AddScore(CClass::HUMAN, NEUTRALITY_STATUE_SCORE);
						break;
					}
				}
			}

			++pos;
		}
	}
}



// 자기 방의 정보를 보내주는 함수
bool	CBGServerMap::SendMapInfo()
{
	if (0 == m_CharacterList.size() && 0 == m_SpectatorList.size()) return true;

	const int   MAX_BUFFER = sizeof(PktBGServerMapList) + sizeof(BGServerMapInfoNode);
	char        szBuffer[MAX_BUFFER];

	PktBGServerMapList*		lpPktBGSMLAck = reinterpret_cast<PktBGServerMapList *>(szBuffer);
	BGServerMapInfoNode*	lpMapInfoNode = reinterpret_cast<BGServerMapInfoNode *>(lpPktBGSMLAck + 1);

	lpPktBGSMLAck->m_bAll = false;
	lpPktBGSMLAck->m_cMapInfoNodeNum = 1;

	lpMapInfoNode->m_bPlaying				= IsPlaying();
	lpMapInfoNode->m_cMapType				= m_MapInfo.m_cMapType;
	lpMapInfoNode->m_cMaxCharNumOfNation	= m_MapInfo.m_cMaxCharNumOfNation;
	lpMapInfoNode->m_wTargetScore			= m_MapInfo.m_wTargetScore;
	//lpMapInfoNode->m_cRemainMin				= (IsPlaying() == true) ? m_MapInfo.m_cRemainPlayMin : m_MapInfo.m_cRemainRestMin;
	lpMapInfoNode->m_cRemainMin				= (IsPlaying() == true) ? m_MapInfo.m_cRemainPlayMin : 0;

	lpMapInfoNode->m_cCurrentCharNum[CClass::HUMAN] = m_MapInfo.m_cCurrentCharNum[CClass::HUMAN];
	lpMapInfoNode->m_cCurrentCharNum[CClass::AKHAN] = m_MapInfo.m_cCurrentCharNum[CClass::AKHAN];
	lpMapInfoNode->m_wScore[CClass::HUMAN]	= m_MapInfo.m_wScore[CClass::HUMAN];
	lpMapInfoNode->m_wScore[CClass::AKHAN]	= m_MapInfo.m_wScore[CClass::AKHAN];

	// 캐릭터 처리
	CharacterList::iterator pos = m_CharacterList.begin();
	CharacterList::iterator end = m_CharacterList.end();
	CGameClientDispatch* lpDispatch = NULL;

	while (pos != end)
	{
		if (NULL == (*pos)) continue;
		lpDispatch = (*pos)->GetDispatcher();

		if (NULL != lpDispatch)
		{
			CSendStream& SendStream = lpDispatch->GetSendStream();
			SendStream.WrapCompress(szBuffer, sizeof(PktBGServerMapList) + sizeof(BGServerMapInfoNode), CmdBGServerMapList, 0, 0);
		}

		++pos;
	}

	// 스펙테이터 처리
	pos = m_SpectatorList.begin();
	end = m_SpectatorList.end();

	while (pos != end)
	{
		if (NULL == (*pos)) continue;
		lpDispatch = (*pos)->GetDispatcher();

		if (NULL != lpDispatch)
		{
			CSendStream& SendStream = lpDispatch->GetSendStream();
			SendStream.WrapCompress(szBuffer, sizeof(PktBGServerMapList) + sizeof(BGServerMapInfoNode), CmdBGServerMapList, 0, 0);
		}

		++pos;
	}

	return true;
}


bool	CBGServerMap::SendResultInfo()
{
	const int   MAX_BUFFER = sizeof(PktBGServerResultList) + sizeof(BGServerResultInfoNode);
	char        szBuffer[MAX_BUFFER];

	PktBGServerResultList*	lpPktBGSRLAck = reinterpret_cast<PktBGServerResultList *>(szBuffer);
	BGServerResultInfoNode*	lpResultInfoNode = reinterpret_cast<BGServerResultInfoNode *>(lpPktBGSRLAck + 1);

	lpPktBGSRLAck->m_bAll = false;
	lpPktBGSRLAck->m_cResultInfoNodeNum = 1;

	lpResultInfoNode->m_cWinRace			  = m_ResultInfo.m_cWinRace;
	lpResultInfoNode->m_wScore[CClass::HUMAN] = m_ResultInfo.m_wScore[CClass::HUMAN];
	lpResultInfoNode->m_wScore[CClass::AKHAN] = m_ResultInfo.m_wScore[CClass::AKHAN];

	// 캐릭터(플레이어) 처리
	CharacterList::iterator pos = m_CharacterList.begin();
	CharacterList::iterator end = m_CharacterList.end();
	CGameClientDispatch* lpDispatch = NULL;

	unsigned short wAwardBase;
	if (CClass::MAX_RACE == m_ResultInfo.m_cWinRace) wAwardBase = m_MapInfo.m_cLimitMin * MILEAGE_PER_MINUTE_FOR_DRAW;
	else wAwardBase = m_MapInfo.m_cLimitMin * MILEAGE_PER_MINUTE_FOR_WIN;
	
	unsigned short wPlayMin = m_MapInfo.m_cLimitMin - m_MapInfo.m_cRemainPlayMin;
	if (0 == wPlayMin) wPlayMin = 1;
	
	while (pos != end)
	{
		if (NULL == (*pos)) continue;
		lpDispatch = (*pos)->GetDispatcher();

		if (NULL != lpDispatch)
		{
			MapInfo::PersonalInfoMap::iterator which = m_MapInfo.m_PersonalInfoMap.find( (*pos)->GetCID() );
			if (which != m_MapInfo.m_PersonalInfoMap.end())
			{
				lpResultInfoNode->m_cPlayMin	= which->second.m_cEnteringMin - m_MapInfo.m_cRemainPlayMin;
				lpResultInfoNode->m_cKill		= which->second.m_cKill;
				lpResultInfoNode->m_cKilled		= which->second.m_cKilled;

				RULLOG3(g_Log, "CID:0x%08x 이벤트로그 - %s, Kill : %d, Die : %d", 
					(*pos)->GetCharacterName(), which->second.m_cKill, which->second.m_cKilled);

				// 무승부나 승리팀의 캐릭터인 경우 얻은 마일리지를 보내준다.
				if (CClass::MAX_RACE == m_ResultInfo.m_cWinRace || (*pos)->GetRace() == m_ResultInfo.m_cWinRace)
				{
					unsigned short wAward = wAwardBase * lpResultInfoNode->m_cPlayMin / wPlayMin;
					lpResultInfoNode->m_wAward = wAward;

					(*pos)->SetMileage((*pos)->GetMileage() + wAward);
					CGameClientDispatch* lpDispatch = (*pos)->GetDispatcher();
					if (NULL != lpDispatch)
					{
						GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), (*pos), 
							"", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);
					}
				}
				else lpResultInfoNode->m_wAward = 0;
			
				CSendStream& SendStream = lpDispatch->GetSendStream();
				SendStream.WrapCompress(szBuffer, sizeof(PktBGServerResultList) + sizeof(BGServerResultInfoNode), CmdBGServerResultList, 0, 0);
			}
		}

		++pos;
	}


	// 스펙테이터 처리
	lpResultInfoNode->m_cPlayMin = 0;
	lpResultInfoNode->m_wAward = 0;
	lpResultInfoNode->m_cKill = 0;
	lpResultInfoNode->m_cKilled = 0;

	pos = m_SpectatorList.begin();
	end = m_SpectatorList.end();
	
	while (pos != end)
	{
		if (NULL == (*pos)) continue;
		lpDispatch = (*pos)->GetDispatcher();

		if (NULL != lpDispatch)
		{
			CSendStream& SendStream = lpDispatch->GetSendStream();
			SendStream.WrapCompress(szBuffer, sizeof(PktBGServerResultList) + sizeof(BGServerResultInfoNode), CmdBGServerResultList, 0, 0);
		}

		++pos;
	}

	return true;
}



void	CBGServerMap::FindWinner(CharacterList& winnerList)
{
	CharacterList::iterator pos = m_CharacterList.begin();
	CharacterList::iterator end = m_CharacterList.end();

	while (pos != end)
	{
		if (NULL != (*pos))
		{
			if (m_ResultInfo.m_cWinRace == (*pos)->GetRace())
			{
				winnerList.push_back((*pos));
			}
		}

		++pos;
	}
}


void	CBGServerMap::DeleteAllItem()
{
	if (NULL == m_CellData) 
	{ 
		return;
	}

	CCell* lpCellPastEnd = m_CellData + GetWidth() * GetHeight();

	for (CCell* lpCell = m_CellData; lpCell != lpCellPastEnd; ++lpCell)
	{
		lpCell->DeleteAllItem();
	}
}


void	CBGServerMap::ResetEnteringMin(unsigned char cMin)
{
	if (m_MapInfo.m_PersonalInfoMap.empty()) return;

	MapInfo::PersonalInfoMap::iterator pos = m_MapInfo.m_PersonalInfoMap.begin();
	MapInfo::PersonalInfoMap::iterator end = m_MapInfo.m_PersonalInfoMap.end();

	while (pos != end)
	{
		pos->second.m_cEnteringMin = cMin;
		++pos;
	}
}
