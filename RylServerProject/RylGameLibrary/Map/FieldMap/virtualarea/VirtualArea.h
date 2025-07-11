#ifndef _VIRTUAL_AREA_H_
#define _VIRTUAL_AREA_H_

#pragma once

#include <Map/FieldMap/Cell.h>

// 전방 참조
class CCharacter;
class CMonster;
class CVirtualMonsterMgr;


namespace VirtualArea
{
	// 전방 참조
	struct ProtoType;
	class CVirtualAreaMgr;


	class CVirtualArea
	{
	public:
		virtual void			Process()												{				}
		virtual unsigned short	Enter(CCharacter* lpCharacter, unsigned char cMoveType)	{ return 0;		}
		virtual bool			Leave(CCharacter* lpCharacter)							{ return false; }

		
		unsigned long	GetVID()			{ return m_dwVID;	}
		CCell*			GetCell(int nIndex)	{ return &m_CellData[nIndex]; }
		unsigned short	GetVirtualType()	{ return static_cast<unsigned short>(m_dwVID >> 16); }
		unsigned short	GetMapIndex()		{ return m_wMapIndex; }

		void			CreateVirtualMonsterManager();
		CVirtualMonsterMgr*	GetMonsterManager()	{ return m_pVirtualMonsterMgr;	}
		
		
		// 스크립트에서 읽은 프로토 타입의 값을 리턴해주는 함수들
		unsigned short	GetStartX();
		unsigned short	GetStartZ();
		unsigned short	GetWidth();
		unsigned short	GetHeight();
		unsigned char	GetVirtualZone();
		Position		GetStartPosition(unsigned char cNation);
		Position		GetRespawnPosition(unsigned char cNation, int nIndex);
		unsigned char	GetMaxRespawnPos();
		const char*		GetMapTypeName();

		// Type 정의
		typedef std::list<CCharacter* >	CharacterList;
		typedef std::list<CMonster* >	MonsterList;

		// 몬스터 처리
		void	ProcessAllMonster();
		void	ProcessMonsterRegenHPAndMP();
		void	ProcessSummonMonsterDead();

		// Item 처리
		void	ProcessDeleteItem();

		// Cell BroadCasting
		bool	ProcessAllCellPrepareBroadCast();
		bool	ProcessAllCellBroadCast(unsigned long dwCurrentPulse);
		
	protected:

		CVirtualArea(const VirtualArea::ProtoType* lpProtoType, unsigned short wMapIndex);
		virtual ~CVirtualArea();

		virtual unsigned short	AddCharacter(CCharacter* lpCharacter)	{ return 0; }
		virtual unsigned short	AddSpectator(CCharacter* lpSpectator)	{ return 0; }

		bool			CreateCell(unsigned short wWidth, unsigned short wHeight, unsigned short wMapIndex);

		CCell*			m_CellData;
		unsigned long	m_dwVID;
		unsigned short	m_wMapIndex;


		CharacterList		m_CharacterList;
		CharacterList		m_SpectatorList;

		CVirtualMonsterMgr*	m_pVirtualMonsterMgr;

		friend class	CVirtualAreaMgr;
	};
}

#endif	// _VIRTUAL_AREA_H_