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
	// ���� ����
	struct ProtoType;

	class CBGServerMap : public CVirtualArea
	{
	public:
		CBGServerMap(const VirtualArea::ProtoType* lpProtoType, unsigned short wMapNumber);
		~CBGServerMap(void);

		void			Process();
		unsigned short	Enter(CCharacter* lpCharacter, unsigned char cMoveType);
		bool			Leave(CCharacter* lpCharacter);

		bool			AllRespawn();						// ��� ���Ƿ� �̵�

		// ���� �� ����
		bool			GameStart();						// ������ �����Ѵ�.
		unsigned char	RuleCheck(bool bTimeout = false);	// ���� üũ�ؼ� �̱������� ����
		void			AwardToWinner();
		void			KillChar(unsigned long dwDeadCID, CCharacter* lpOffencer);
		
		void			UpdateKillInfo(unsigned long dwDeadCID, unsigned long dwKillerCID);
		void			AddScore(unsigned char cNation, short wScore);
		void			CalculateScore();					// ���� �������϶� ���� ���

		void			ResetEnteringMin(unsigned char cMin);


		// Get/Set �Լ�
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


		// �ڱ� ���� ������ �����ִ� �Լ�
		bool			SendMapInfo();
		bool			SendResultInfo();
		
	private:

		bool			InitializeGameObject();

		void			FindWinner(CharacterList& winnerList);

		unsigned short	AddCharacter(CCharacter* lpCharacter);
		unsigned short	AddSpectator(CCharacter* lpSpectator);

		// �ٴڿ� ������ Item ��� �����
		void			DeleteAllItem();

		// ���� �� ����
		MapInfo			m_MapInfo;
		ResultInfo		m_ResultInfo;

		unsigned char	m_cStatus;
		unsigned long	m_dwRemainTime;			// �ð��� ���õ� ��� �κп��� ���� �ð����� ���
	};
}

#endif // _BATTLEGROUND_SERVER_MAP_H_