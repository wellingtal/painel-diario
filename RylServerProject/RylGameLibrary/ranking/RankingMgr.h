#ifndef _RYL_GAME_LIBRARY_RANKING_MGR_H_
#define _RYL_GAME_LIBRARY_RANKING_MGR_H_

#include <Network/Packet/PacketStruct/RankingPacket.h>
#include <RylGameLibrary/Creature/Character/CharacterClass.h>


// 전방 참조
class CSendStream;

class CRankingMgr
{
public:

	enum Const
	{
		MAX_DECLARE_RANK_NUM	= 100
	};

    static CRankingMgr& GetInstance();

	void UpdateRanking(const RankingNode& node);

	bool SendRankingInfo(unsigned long dwCID, unsigned char cClass, unsigned char cPage);
	unsigned char GetRank(const char* szCharName, unsigned char cClass);


private:

    CRankingMgr();
    ~CRankingMgr();

	RankingNode	m_aryRanking[CClass::MAX_CLASS][MAX_DECLARE_RANK_NUM];
};


#endif