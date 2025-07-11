#ifndef _VIRTUAL_AREA_MANAGER_H_
#define _VIRTUAL_AREA_MANAGER_H_

#pragma once

#include <vector>
#include <map>

#include <Map/FieldMap/VirtualArea/VirtualAreaStructure.h>


class CCell;
class CCharacter;

namespace VirtualArea
{
	// ���� ����

	class CBGServerMgr;
	class CDuelMgr;
	class CDungeonMgr;
	class CVirtualArea;

	
	class CVirtualAreaMgr
	{
	public:

		enum Const
		{
			VIRTUALAREA_PULSE	= 20		// 2�ʿ� �ѹ��� ó��
		};

		static CVirtualAreaMgr& GetInstance();

		// Script ���� �ε�
		bool LoadVirtualAreaProtoType(const char* szFileName = 0);

		CVirtualArea* GetVirtualArea(unsigned short wMapIndex);

		const VirtualArea::ProtoType* GetVirtualAreaProtoType(unsigned long dwVID);
		const VirtualArea::ProtoType* GetVirtualAreaProtoType(char* szMapType);

		bool EnterVirtualArea(CCharacter* lpCharacter, unsigned short wMapIndex, unsigned char cMoveType);
		bool LeaveVirtualArea(CCharacter* lpCharacter);

		void ProcessAllVirtualArea();
		void ProcessAllMonster();
		void ProcessMonsterRegenHPAndMP();
		void ProcessSummonMonsterDead();
		void ProcessDeleteItem();

		// Cell BroadCasting
		bool ProcessAllCellPrepareBroadCast();
		bool ProcessAllCellBroadCast(unsigned long dwCurrentPulse);

		// --------------------------------------------------------------------------------------------
		// BattleGround Server ���� �Լ�
		bool CreateBGServer();	// Battle Ground Server Map �� Cell ����

		bool SendBGServerMapList(CCharacter* lpCharacter);
		bool SendBGServerResultList(CCharacter* lpCharacter);


		// --------------------------------------------------------------------------------------------
		// Duel ���� �Լ�



		// --------------------------------------------------------------------------------------------
		// Dungeon ���� �Լ�



	private:
		CVirtualAreaMgr();
		~CVirtualAreaMgr();

		
		void DestroyVirtualAreaProtoTypeArray();

		
		// --------------------------------------------------------------------------------------------
		// member variable

		CBGServerMgr&	m_BGServerMgr;
		CDuelMgr&		m_DuelMgr;
		CDungeonMgr&	m_DungeonMgr;

		
		VirtualArea::ProtoType*		m_VirtualAreaProtoTypeArray;
		size_t						m_VirtualAreaProtoTypeNum;

		const	VirtualArea::MapTypeMatching	m_MapTypeMatching;


		// --------------------------------------------------------------------------------------------
		// BattleGround Server ����


		// --------------------------------------------------------------------------------------------
		// Duel ����



		// --------------------------------------------------------------------------------------------
		// Dungeon ����
	};
}

#endif	// _VIRTUAL_AREA_MANAGER_H_