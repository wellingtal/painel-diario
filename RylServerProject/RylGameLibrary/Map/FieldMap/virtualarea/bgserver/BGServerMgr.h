#ifndef _BATTLEGROUND_SERVER_MANAGER_H_
#define _BATTLEGROUND_SERVER_MANAGER_H_

#pragma once

#include <vector>

// 전방 참조
class CCharacter;


namespace VirtualArea
{
	// 전방 참조
	class CVirtualArea;
	class CBGServerMap;

	
	class CBGServerMgr
	{
	public:

		static CBGServerMgr& GetInstance();

		bool	CreateBGServerMap();

		void	Process();
		bool	Enter(CCharacter* lpCharacter, unsigned short wMapIndex, unsigned char cMoveType);
		bool	Leave(CCharacter* lpCharacter);

		CVirtualArea* GetVirtualArea(unsigned short wMapIndex);

		bool	SendMapList(CCharacter* lpCharacter);
		bool	SendResultList(CCharacter* lpCharacter);
		bool	SendMapListToAllCharacter();

		typedef	std::vector<CBGServerMap* > BGServerMapList;

		void	ProcessAllMonster();
		void	ProcessMonsterRegenHPAndMP();
		void	ProcessSummonMonsterDead();
		void	ProcessDeleteItem();

		// Cell BroadCasting
		bool	ProcessAllCellPrepareBroadCast();
		bool	ProcessAllCellBroadCast(unsigned long dwCurrentPulse);
				
	private:
		CBGServerMgr();
		~CBGServerMgr();

		void	DestroyBGServerMap();
		bool	LoginAllMonster();
		void	GameAllStart();


		BGServerMapList	m_lstBGServerMap;
	};
}

#endif	// _BATTLEGROUND_SERVER_MANAGER_H_