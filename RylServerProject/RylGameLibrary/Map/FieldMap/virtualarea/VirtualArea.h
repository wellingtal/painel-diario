#ifndef _VIRTUAL_AREA_H_
#define _VIRTUAL_AREA_H_

#pragma once

#include <Map/FieldMap/Cell.h>

// ���� ����
class CCharacter;
class CMonster;
class CVirtualMonsterMgr;


namespace VirtualArea
{
	// ���� ����
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
		
		
		// ��ũ��Ʈ���� ���� ������ Ÿ���� ���� �������ִ� �Լ���
		unsigned short	GetStartX();
		unsigned short	GetStartZ();
		unsigned short	GetWidth();
		unsigned short	GetHeight();
		unsigned char	GetVirtualZone();
		Position		GetStartPosition(unsigned char cNation);
		Position		GetRespawnPosition(unsigned char cNation, int nIndex);
		unsigned char	GetMaxRespawnPos();
		const char*		GetMapTypeName();

		// Type ����
		typedef std::list<CCharacter* >	CharacterList;
		typedef std::list<CMonster* >	MonsterList;

		// ���� ó��
		void	ProcessAllMonster();
		void	ProcessMonsterRegenHPAndMP();
		void	ProcessSummonMonsterDead();

		// Item ó��
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