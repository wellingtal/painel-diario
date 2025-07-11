#ifndef _RANKING_MGR_H_
#define _RANKING_MGR_H_

#pragma once

#include <Network/Packet/PacketStruct/RankingPacket.h>
#include <Creature/Character/CharacterClass.h>

// forward decl.
class CSendStream;

class CRankingDBMgr
{
public:
	enum Const
	{
		MAX_DECLARE_RANK_NUM	= 100,
		NODE_NUM_PER_SENDING	= 400
	};

    bool Initialize(void);
	void UpdateRanking(RankingNode& node);
	
	void SendRankingInfo(const RankingNode& node);
	void SendRankingInfo(CSendStream& SendStream);

	void DeleteRanking(RankingNode& node);

    static CRankingDBMgr& GetInstance();
    static VOID ReleaseInstance();

public:
    CRankingDBMgr();
    virtual ~CRankingDBMgr();

private:

	RankingNode	m_aryRanking[CClass::MAX_CLASS][MAX_DECLARE_RANK_NUM];
};

#endif