
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

		// �α�
		DETLOG5(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x ĳ����(%s, %s)�� ���ӿ��� �����̽��ϴ�.",
				(m_wMapIndex & ~VirtualArea::BGSERVERMAP), GetMapTypeName(),
				lpCharacter->GetCID(), lpCharacter->GetCharacterName(), szNation);

		return true;
	}

	pos = std::find(m_SpectatorList.begin(), m_SpectatorList.end(), lpCharacter);
	if (pos != m_SpectatorList.end())
	{
		m_SpectatorList.erase(pos);

		// �α�
		DETLOG5(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x ĳ����(%s, %s)�� ���ӿ��� �����̽��ϴ�.",
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

	// ���� �ο� ���� �ɱ�
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

	// �������� ���� ���ð��� �����صд�. (���� ���ð��� ���� ���� ������ �ֱ����ؼ�...)
	MapInfo::PersonalInfo personalInfo(m_MapInfo.m_cRemainPlayMin);
	m_MapInfo.m_PersonalInfoMap.insert( make_pair(lpCharacter->GetCID(), personalInfo) );

	// �α� �����
	char szNation[8];
	if (lpCharacter->GetRace() == CClass::HUMAN) 
	{
		strcpy(szNation, "HUMAN");
	}
	else 
	{
		strcpy(szNation, "AKHAN");
	}

	DETLOG5(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x ĳ����(%s, %s)�� Player �� ���Խ��ϴ�.", 
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

	// �α� �����
	char szNation[8];
	if (lpSpectator->GetRace() == CClass::HUMAN) strcpy(szNation, "HUMAN");
	else strcpy(szNation, "AKHAN");

	DETLOG5(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x ĳ����(%s, %s)�� Spectator �� ���Խ��ϴ�.",
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

// ��� ���Ƿ� �̵�
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
				ERRLOG2(g_Log, "CID:0x%08x ĳ���Ͱ� ���� ��(MapIndex : %d)���� �����µ� �����Ͽ����ϴ�.", (*pos)->GetCID(), (*pos)->GetMapIndex());
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
			ERRLOG2(g_Log, "CID:0x%08x ĳ���Ͱ� ���� ��(MapIndex : %d)���� �����µ� �����Ͽ����ϴ�.", (*pos)->GetCID(), (*pos)->GetMapIndex());
			bResult = false;
		}

		++pos;
	}

	// ���������� ó��
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
				ERRLOG2(g_Log, "CID:0x%08x ĳ���Ͱ� ���� ��(MapIndex : %d)���� �����µ� �����Ͽ����ϴ�.", (*pos)->GetCID(), (*pos)->GetMapIndex());
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
			ERRLOG2(g_Log, "CID:0x%08x ĳ���Ͱ� ���� ��(MapIndex : %d)���� �����µ� �����Ͽ����ϴ�.", (*pos)->GetCID(), (*pos)->GetMapIndex());
			bResult = false;
		}

		++pos;
	}

	// ����Ʈ Ŭ����
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
		// ���� Start ���� ���� ���¶�� �ƹ��͵� ���� �ʴ´�.
		case START_WAITING: return;

		case GAME_PLAYING:
		{
			// 1�п� �ѹ��� ���� �ð��� �����ش�.
			if (nMin < m_MapInfo.m_cRemainPlayMin)
			{
				// ���� �ð� ����
				m_MapInfo.m_cRemainPlayMin = static_cast<unsigned char>(nMin);

				// �� ���� ����
				SendMapInfo();
			}

			// ���� �ð��� �� �Ǿ��ٸ�,
			if (0 == m_MapInfo.m_cRemainPlayMin)
			{
				// ���и� üũ �� ��� ����
				m_ResultInfo.m_cWinRace = RuleCheck(true);
				std::copy(&m_MapInfo.m_wScore[0], &m_MapInfo.m_wScore[CClass::MAX_RACE], &m_ResultInfo.m_wScore[0]);

				// ��� ����
				// �̱� ����, Ȥ�� ���º� ĳ���͵鿡�� ������ �ش�.
				SendResultInfo();
			}
			else
			{
				m_ResultInfo.m_cWinRace = RuleCheck();

				// �� ������ ��ǥ ������ �޼����� ���
				if (CClass::MAX_RACE != m_ResultInfo.m_cWinRace)
				{
					// ��� ���� ����
					std::copy(&m_MapInfo.m_wScore[0], &m_MapInfo.m_wScore[CClass::MAX_RACE], &m_ResultInfo.m_wScore[0]);

					// ��� ����
					// �̱� ������ ĳ���͵鿡�� ������ �ش�.
					SendResultInfo();
				}
			}

			break;
		}

		case GAME_RESTING:
		{
			// ���� �ð� ����
			m_MapInfo.m_cRemainRestMin = static_cast<unsigned char>(nMin);

			// ���� �ð��� �� �Ǿ��ٸ�,
			if (0 == m_MapInfo.m_cRemainRestMin)
			{
				GameStart();
			}

			break;
		}

		case MOVEZONE_WAITING:
		{
			// ���̵� ��ų �ð��� �� �Ǿ��ٸ�,
			if (m_dwRemainTime <= timeGetTime())
			{
				SetStatus(GAME_RESTING);
				m_dwRemainTime = timeGetTime() + m_MapInfo.m_cRestMin * MILLISEC_PER_MINUTE;

				// ĳ���͵��� �� �̵� ��Ų��. (�κ�� ������)
				AllRespawn();

				// Item�� ��� �����.
				DeleteAllItem();

				// �� ���� �ʱ�ȭ
				m_MapInfo.Initialize();
			}

			break;
		}
	}
}


bool	CBGServerMap::GameStart()						// ������ �����Ѵ�.
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
		// ��� ������ �� ���̰�, �ƹ��͵� ��ȯ���� �ʴ´�.
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


		// �޸�, �߸�, ��ĭ ������ ������� ��ȯ�Ѵ�.
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
				// ���� �ʱ�ȭ ��Ű��
				// !!����!! ��ũ��Ʈ�� ������ �ִ�. ��������
				// ������ ���´�. (HUMAN_STATUE = 0, NUETRALITY_STATUE1 = 1, AKHAN_STATUE = 2, NUETRALITY_STATUE2 = 3, NUETRALITY_STATUE3 = 4)
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

		// ���� �������϶��� �ʱ� ���� ���
		CalculateScore();

		// �α� �����
		DETLOG4(g_Log, "Battle Server Log :: (Channel : %d, %s) - �������� �ʱ�ȭ �մϴ�. (���� Score - HM: %d  AK: %d)",
				(m_wMapIndex & ~VirtualArea::BGSERVERMAP), GetMapTypeName(),
				m_MapInfo.m_wScore[CClass::HUMAN], m_MapInfo.m_wScore[CClass::AKHAN]);
	}

	return true;
}



unsigned char	CBGServerMap::RuleCheck(bool bTimeout)	// ���� üũ�ؼ� �̱������� ����
{
	unsigned char cWinNation = CClass::MAX_RACE;

	// �ð��� �� �����ٸ�, Ȥ�� ��� ������� ������ ���� ��ų���
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
			// �̰���� CClass::MAX_RACE �� ���º��̴�.
		}

		SetStatus(MOVEZONE_WAITING);
		m_dwRemainTime = timeGetTime() + MOVEZONE_WAIT_TIME;
	}
	else
	{
		// ��ǥ ������ �����ߴ°� üũ
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

		// �̰���� CClass::MAX_RACE �� ������ ������ ���� �����̴�.
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

	// Kill ���� ������Ʈ
	UpdateKillInfo(dwDeadCID, lpOffencer->GetCID());

	// �� ���� ����
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


void	CBGServerMap::CalculateScore()					// ���� �������϶� ���� ���
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



// �ڱ� ���� ������ �����ִ� �Լ�
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

	// ĳ���� ó��
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

	// ���������� ó��
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

	// ĳ����(�÷��̾�) ó��
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

				RULLOG3(g_Log, "CID:0x%08x �̺�Ʈ�α� - %s, Kill : %d, Die : %d", 
					(*pos)->GetCharacterName(), which->second.m_cKill, which->second.m_cKilled);

				// ���ºγ� �¸����� ĳ������ ��� ���� ���ϸ����� �����ش�.
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


	// ���������� ó��
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
