#ifndef _BATTLEGROUND_SERVER_MAP_H_
#define _BATTLEGROUND_SERVER_MAP_H_

#pragma once

#include <Map/FieldMap/VirtualArea/VirtualAreaConstants.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaStructure.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>

class CCharacter;
class CMonster;

namespace VirtualArea
{
	// 전방 참조
	struct ProtoType;

	class CBGServerMap : public CVirtualArea
	{
	public:
		CBGServerMap(const VirtualArea::ProtoType* lpProtoType, unsigned short wMapNumber);
		~CBGServerMap(void);

		void			Process();
		unsigned short	Enter(CCharacter* lpCharacter, unsigned char cMoveType);
		bool			Leave(CCharacter* lpCharacter);

		bool			AllRespawn();						// 모두 대기실로 이동

		// 게임 룰 관련
		bool			GameStart();						// 게임을 시작한다.
		unsigned char	RuleCheck(bool bTimeout = false);	// 룰을 체크해서 이긴종족을 리턴
		void			AwardToWinner();
		void			KillChar(unsigned long dwDeadCID, CCharacter* lpOffencer);
		
		void			UpdateKillInfo(unsigned long dwDeadCID, unsigned long dwKillerCID);
		void			AddScore(unsigned char cNation, short wScore);
		void			CalculateScore();					// 석상 점령전일때 점수 계산

		void			ResetEnteringMin(unsigned char cMin);


		// Get/Set 함수
		unsigned char	GetMapType()								{ return m_MapInfo.m_cMapType;	}
		MapInfo&		GetMapInfo()								{ return m_MapInfo;				}
		ResultInfo&		GetResultInfo()								{ return m_ResultInfo;			}

		unsigned char	GetStatus()									{ return m_cStatus;		}
		void			SetStatus(unsigned char cStatus)			{ m_cStatus = cStatus;	}

		void			SetRemainTime(unsigned long dwRemainTime)	{ m_dwRemainTime = dwRemainTime;	}

		bool			IsPlaying()									{ return (GAME_PLAYING == m_cStatus); }
		bool			IsResting()									{ return (GAME_RESTING == m_cStatus); }
		bool			IsPlayer(CCharacter* lpCharacter);
		bool			IsSpectator(CCharacter* lpCharacter);


		// 자기 방의 정보를 보내주는 함수
		bool			SendMapInfo();
		bool			SendResultInfo();
		
	private:

		bool			InitializeGameObject();

		void			FindWinner(CharacterList& winnerList);

		unsigned short	AddCharacter(CCharacter* lpCharacter);
		unsigned short	AddSpectator(CCharacter* lpSpectator);

		// 바닥에 떨어진 Item 모두 지우기
		void			DeleteAllItem();

		// 게임 방 정보
		MapInfo			m_MapInfo;
		ResultInfo		m_ResultInfo;

		unsigned char	m_cStatus;
		unsigned long	m_dwRemainTime;			// 시간에 관련된 모든 부분에서 남은 시간으로 사용
	};
}

#endif // _BATTLEGROUND_SERVER_MAP_H_