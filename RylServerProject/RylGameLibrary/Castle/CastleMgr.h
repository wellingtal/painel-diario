#ifndef _CASTLE_MANAGER_H_
#define _CASTLE_MANAGER_H_

#pragma once

#include <map>
#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include <Creature/CreatureStructure.h>

// ���� ����
class CSiegeObject;
class CSiegeObjectMgr;

namespace Castle
{
	// ���� ����
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
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���Ƶд�.
//		CCastle*		GetCastleByGID(unsigned long dwGID);
		CCastle*		GetCastleInBlessArea(const Position& Pos);
		void			GetCastleSiegeInfo(CastleSiegeInfo* lpCastleSiegeInfo, unsigned char& cNum, unsigned short& wSize);

		
		// ���� ������ ����
		unsigned char	GetCastleNum() const;
		
		// �ش� ��, �ش� ������ ���� ������ ����
		unsigned char	GetCastleNum(unsigned char cNation, unsigned char cZone) const;
		
		// �ش� ��, �ش� ������ ���� �ݰ泻�� �ִ� ���� ������ ����
		unsigned char	GetCastleNum(unsigned char cNation, unsigned char cZone, const Position& Pos) const;		

		// Pos �ݰ濡 ���� �����ϴ��� üũ
		bool		ExistCastleInRadius(const Position& Pos);

		// Pos �ݰ濡 ���� ���⸦ ������ �� �ִ��� üũ
		bool		ExistSiegeInRadius(const Position& Pos);

		
		// ��Ŷ ����
		bool		SendCastleInfo(CSendStream& SendStream);

		// DBAgentServer �κ��� �� ������ ����
		bool		SerializeIn(char* lpBuffer_In, unsigned short wBufferSize_In, unsigned char cObjectNum);

		
		// �� ��¡�� HP Regen ó��
		void		ProcessEmblemRegenHPAndMP();

		// ���� �ð��� ������ ���� ���� ���� ���� �ʱ�ȭ
		void		EnableTaxChange();

		// ���� ���⸦ ���� ���� NPC �� ����
		void		DestroyAllCastleArms();

		// ���� ���� üũ.
		bool		HasCastleArms(unsigned long dwCID);

		// �� ������.
		unsigned char GetNation();

	private:
		CCastleMgr();
		
		CastleMap			m_CastleMap;
	};
}

#endif	// _CASTLE_MANAGER_H_