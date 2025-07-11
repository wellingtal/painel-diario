#ifndef _CASTLE_MANAGER_H_
#define _CASTLE_MANAGER_H_

#pragma once

#include <map>
#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include <Creature/CreatureStructure.h>

// 전방 참조
class CSiegeObject;
class CSiegeObjectMgr;

namespace Castle
{
	// 전방 참조
	class CCastle;

	class CCastleMgr
	{
	public:
		~CCastleMgr();
		static CCastleMgr& GetInstance();

		typedef	std::map<unsigned long, CCastle*>			CastleMap;				// <CastleID, lpCastle>
		
		bool		Initialize();
		void		Destroy();

		void		Process();

		CCastle*		GetCastle(unsigned long dwCastleID);
		CCastle*		GetCastleByNameID(unsigned long dwCastleNameID);
// CASTLE_TODO : 성이 길드 소유가 아니므로 막아둔다.
//		CCastle*		GetCastleByGID(unsigned long dwGID);
		CCastle*		GetCastleInBlessArea(const Position& Pos);
		void			GetCastleSiegeInfo(CastleSiegeInfo* lpCastleSiegeInfo, unsigned char& cNum, unsigned short& wSize);

		
		// 성의 갯수를 리턴
		unsigned char	GetCastleNum() const;
		
		// 해당 존, 해당 국가의 성의 갯수를 리턴
		unsigned char	GetCastleNum(unsigned char cNation, unsigned char cZone) const;
		
		// 해당 존, 해당 국가의 보상 반경내에 있는 성의 갯수를 리턴
		unsigned char	GetCastleNum(unsigned char cNation, unsigned char cZone, const Position& Pos) const;		

		// Pos 반경에 성이 존재하는지 체크
		bool		ExistCastleInRadius(const Position& Pos);

		// Pos 반경에 공성 병기를 생성할 수 있는지 체크
		bool		ExistSiegeInRadius(const Position& Pos);

		
		// 패킷 전송
		bool		SendCastleInfo(CSendStream& SendStream);

		// DBAgentServer 로부터 성 정보를 얻어옴
		bool		SerializeIn(char* lpBuffer_In, unsigned short wBufferSize_In, unsigned char cObjectNum);

		
		// 성 상징물 HP Regen 처리
		void		ProcessEmblemRegenHPAndMP();

		// 공성 시간후 성들의 세율 변경 가능 정보 초기화
		void		EnableTaxChange();

		// 수성 병기를 병기 관리 NPC 로 변경
		void		DestroyAllCastleArms();

		// 수성 병기 체크.
		bool		HasCastleArms(unsigned long dwCID);

		// 성 소유주.
		unsigned char GetNation();

	private:
		CCastleMgr();
		
		CastleMap			m_CastleMap;
	};
}

#endif	// _CASTLE_MANAGER_H_