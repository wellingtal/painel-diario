#ifndef _DBAGENT_SERVER_PARSE_CASTLE_H_
#define _DBAGENT_SERVER_PARSE_CASTLE_H_

class CSendStream;
struct PktBase;

namespace DBAgent
{
namespace ParseCastle
{
    bool CastleCmd(CSendStream& SendStream, PktBase* lpPktBase);
    bool SiegeArmsCmd(CSendStream& SendStream, PktBase* lpPktBase);
    bool SetCastleRight(CSendStream& SendStream, PktBase* lpPktBase);
    bool SetCampRight(CSendStream& SendStream, PktBase* lpPktBase);
    bool CreateCamp(CSendStream& SendStream, PktBase* lpPktBase);
    bool CreateSiegeArms(CSendStream& SendStream, PktBase* lpPktBase);
    bool CampCmd(CSendStream& SendStream, PktBase* lpPktBase);
	bool CampMessage(CSendStream& SendStream, PktBase* lpPktBase);
	bool MiningCampMineralInfo(CSendStream& SendStream, PktBase* lpPktBase);
	bool FertilityInfo(CSendStream& SendStream, PktBase* lpPktBase);
	bool CampShopInfo(CSendStream& SendStream, PktBase* lpPktBase);
	bool CastleTaxMove(CSendStream& SendStream, PktBase* lpPktBase);
	bool CastleMineralInfo(CSendStream& SendStream, PktBase* lpPktBase);
	bool WarOnOff(CSendStream& SendStream, PktBase* lpPktBase);
	bool StatueCmd(CSendStream& SendSream, PktBase* lpPktBase);
	bool RealmPoint(CSendStream& SendSream, PktBase* lpPktBase);
}
}



#endif
