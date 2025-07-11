#ifndef _RYL_DBAGENT_CASTLE_DB_MANAGER_H_
#define _RYL_DBAGENT_CASTLE_DB_MANAGER_H_

#pragma once

#include <map>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Creature/CreatureStructure.h>

#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>

// 전방 참조
class CDBComponent;
class CSendStream;
class CSiegeObjectDB;
class CSiegeObjectDBMgr;

namespace Castle
{
	// 전방 참조
	class CCastleDB;

	class CCastleDBMgr
	{
	public:

        static CCastleDBMgr& GetInstance();

        // <CastleID, lpCastle>
        typedef	std::map<unsigned long, CCastleDB*, std::less<unsigned long>,
            boost::fast_pool_allocator<std::pair<unsigned long, CCastleDB*> > > CastleMap;  

		bool Initialize(CDBComponent& DBComponent);
		void Destroy();
		
		void CheckEnableSiegeForAllCastle(bool bIsSiegeTime);
		void UpdateSiegeCount();
		void DestroyAllCastleArms();

		void CloseCastleGate();
		void OpenCastleGate();

		CCastleDB* GetCastle(unsigned long dwCastleID);
		void GetCastleSiegeInfo(CastleSiegeInfo* lpCastleSiegeInfo, unsigned char& cNum, unsigned short& wSize);
        
		// GameServer 로의 Send 함수
		void SendCastleInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel);
        
	private:

        CCastleDBMgr();
		~CCastleDBMgr();

        bool ReadCastleInfoDB();
		bool ReadCastleTaxInfoDB();
		bool ReadCastleMineralInfoDB();
        
		CCastleDB* CreateCastle(const CastleInfoDB& CastleInfo);    // DB 에서 읽어와서 생성

        bool SerializeOut(CCastleDB* lpCastle_In, char* lpBuffer_Out,
            unsigned short& wBufferSize_Out, unsigned char& cDefensiveArmsNum_Out);

        CCastleDB* CreateCastleDB(const CastleInfoDB& CastleInfo);
        void DeleteCastleDB(CCastleDB* lpCastleDB);

        CDBComponent*   m_lpDBComponent;

        boost::pool<>   m_CastlePool;
		CastleMap	    m_CastleMap;
	};

};

#endif	// _CASTLE_MANAGER_H_
