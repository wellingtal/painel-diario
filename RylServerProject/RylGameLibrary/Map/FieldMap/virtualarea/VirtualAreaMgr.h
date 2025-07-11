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
	// 전방 참조

	class CBGServerMgr;
	class CDuelMgr;
	class CDungeonMgr;
	class CVirtualArea;

	
	class CVirtualAreaMgr
	{
	public:

		enum Const
		{
			VIRTUALAREA_PULSE	= 20		// 2초에 한번씩 처리
		};

		static CVirtualAreaMgr& GetInstance();

		// Script 파일 로드
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
		// BattleGround Server 관련 함수
		bool CreateBGServer();	// Battle Ground Server Map 과 Cell 생성

		bool SendBGServerMapList(CCharacter* lpCharacter);
		bool SendBGServerResultList(CCharacter* lpCharacter);


		// --------------------------------------------------------------------------------------------
		// Duel 관련 함수



		// --------------------------------------------------------------------------------------------
		// Dungeon 관련 함수



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
		// BattleGround Server 변수


		// --------------------------------------------------------------------------------------------
		// Duel 변수



		// --------------------------------------------------------------------------------------------
		// Dungeon 변수
	};
}

#endif	// _VIRTUAL_AREA_MANAGER_H_