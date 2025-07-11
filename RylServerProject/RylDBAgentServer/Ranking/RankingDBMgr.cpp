#include "stdafx.h"
#include "RankingDBMgr.h"

#include <DB/DBComponent.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/RankingPacket.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Utility/Setup/ServerSetup.h>
#include <Utility/Compress/MiniLZO/miniLZOWrapper.h>

#include <Log/ServerLog.h>

static CRankingDBMgr* rankingMgr = NULL;

CRankingDBMgr& CRankingDBMgr::GetInstance()
{
//    static CRankingDBMgr rankingMgr;
	if(!rankingMgr)
		rankingMgr = new CRankingDBMgr();

    return *rankingMgr;
}

VOID CRankingDBMgr::ReleaseInstance()
{
	if(rankingMgr)
	{
		delete rankingMgr;
		rankingMgr = NULL;
	}
}

CRankingDBMgr::CRankingDBMgr()
{
    memset(m_aryRanking, 0, sizeof(RankingNode) * CClass::MAX_CLASS * MAX_DECLARE_RANK_NUM);
	SERLOG0(g_Log, "CRankingDBMgr::CRankingDBMgr()");
}

CRankingDBMgr::~CRankingDBMgr()
{

}

bool CRankingDBMgr::Initialize()
{
	const unsigned char MAX_RANKING_CLASS_NUM = 14;
	const int nClassList[MAX_RANKING_CLASS_NUM] =
	{
		// 인간 1st클래스
		CClass::Defender,
		CClass::Warrior,
		CClass::Assassin,//wasnt commented out hz
		CClass::Archer,
		CClass::Sorcerer,
		CClass::Enchanter,//wasnt commented out hz
		CClass::Priest,//wasnt commented out hz
		CClass::Cleric,

		// 아칸 1st클래스
		CClass::Templar,
		CClass::Attacker,
		CClass::Gunner,//wasnt commented out hz
		CClass::RuneOff,
		CClass::LifeOff,
		CClass::ShadowOff
	};

    char szQuery[CDBComponent::QUERY_BUFFER_LEN] = "";
	int nListNum = 0;

	memset(m_aryRanking, 0, sizeof(RankingNode) * CClass::MAX_CLASS * MAX_DECLARE_RANK_NUM);

	// 클래스 별로 MAX_DECLARE_RANK_NUM(= 100)만큼 명성 별로 리스트에 채워 넣는다.
	
	for (unsigned char cCount = 0; cCount < MAX_RANKING_CLASS_NUM; ++cCount)
	{
		int nClass = nClassList[cCount];
		
        _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
            "SELECT top %d CharInfo.Name, CharInfo.Fame, CharInfo.LEVEL, CharInfo.Class "
            "FROM CharInfo, TblUnifiedCharList "
            "WHERE TblUnifiedCharList.OldServerGroupID = %d AND "
            "TblUnifiedCharList.NewCID = CharInfo.CID AND "
            "CharInfo.Class = %d AND CharInfo.Fame != 0 ORDER BY CharInfo.Fame DESC, CharInfo.LEVEL DESC",
            MAX_DECLARE_RANK_NUM, CServerSetup::GetInstance().GetAgentServerType(), nClass);

        szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

        if (!CDBSingleObject::GetInstance().Select(szQuery, 
			(void **)m_aryRanking[nClass], sizeof(RankingNode), 0, MAX_DECLARE_RANK_NUM, &nListNum))
		{
            ERRLOG3(g_Log, "Class:%d / Ranking 얻기 쿼리를 실패했습니다 : %s : %s",
                nClass, CDBSingleObject::GetInstance().GetErrorString(), szQuery);
			return false;
		}

	}

	return true;
}

void CRankingDBMgr::DeleteRanking(RankingNode& node)
{
	if (CClass::JOB_CHANGE_1ST == CClass::GetJobLevel(node.m_cClass)) 
	{ 
		bool bUpdate = false;

		for (int nRankIndex = 0; nRankIndex < MAX_DECLARE_RANK_NUM; nRankIndex++)
		{
			if (0 == strcmp("", m_aryRanking[node.m_cClass][nRankIndex].m_szCharName))
			{
				break;
			}

			if (0 == strcmp(node.m_szCharName, m_aryRanking[node.m_cClass][nRankIndex].m_szCharName))
			{			
				memset(&m_aryRanking[node.m_cClass][nRankIndex], 0, sizeof(RankingNode));
				break;
			}
		}		
	}
}

void CRankingDBMgr::UpdateRanking(RankingNode& node)
{
	if (CClass::JOB_CHANGE_1ST == CClass::GetJobLevel(node.m_cClass)) 
	{ 
	    bool bUpdate = false;

	    for (int nRankIndex = 0; nRankIndex < MAX_DECLARE_RANK_NUM; nRankIndex++)
	    {
		    if (0 == strcmp("", m_aryRanking[node.m_cClass][nRankIndex].m_szCharName))
		    {
			    break;
		    }

		    if (0 == strcmp(node.m_szCharName, m_aryRanking[node.m_cClass][nRankIndex].m_szCharName))
		    {
			    if (node.m_dwPoint == m_aryRanking[node.m_cClass][nRankIndex].m_dwPoint && 
				    node.m_cLevel == m_aryRanking[node.m_cClass][nRankIndex].m_cLevel && 
				    node.m_cClass == m_aryRanking[node.m_cClass][nRankIndex].m_cClass)
			    {
				    return;
			    }

			    // 이미 랭커인 경우 최신 정보로 업데이트
			    m_aryRanking[node.m_cClass][nRankIndex] = node;
			    bUpdate = true;
			    break;
		    }
	    }

	    if (!bUpdate)
	    {
		    if (m_aryRanking[node.m_cClass][MAX_DECLARE_RANK_NUM-1] < node)
		    {
			    // 랭킹 리스트에 진입 (최하위보다 수치가 높음)
			    m_aryRanking[node.m_cClass][MAX_DECLARE_RANK_NUM-1] = node;
			    bUpdate = true;
		    }
	    }

	    if (bUpdate)
	    {
		    std::sort(&m_aryRanking[node.m_cClass][0], 
                &m_aryRanking[node.m_cClass][MAX_DECLARE_RANK_NUM], 
			    RankingNode::ComparePoint());

		    SendRankingInfo(node);
	    }
    }
}

void CRankingDBMgr::SendRankingInfo(const RankingNode& node)
{
    const unsigned short MAX_RANKING_PACKET_SIZE = sizeof(PktRankingInfo) + sizeof(RankingNode);
    char szBuffer[MAX_RANKING_PACKET_SIZE];
    memset(szBuffer, 0, sizeof(char) * MAX_RANKING_PACKET_SIZE);

    PktRankingInfo* lpPktRankingInfo = 
        reinterpret_cast<PktRankingInfo*>(szBuffer);

    RankingNode* lpRankingNode = 
        reinterpret_cast<RankingNode*>(lpPktRankingInfo + 1);

    lpPktRankingInfo->m_wNum = 1;
    *lpRankingNode = node;

    if(PacketWrap::WrapHeader(szBuffer, MAX_RANKING_PACKET_SIZE, CmdRankingInfo, 0, 0))
    {
        DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
            szBuffer, MAX_RANKING_PACKET_SIZE, CmdRankingInfo));
    }
}

void CRankingDBMgr::SendRankingInfo(CSendStream& SendStream)
{
	SERLOG0(g_Log, "CRankingDBMgr::SendRankingInfo : Start");

	const unsigned short MAX_RANKING_PACKET_SIZE = 
        sizeof(PktRankingInfo) + sizeof(RankingNode) * NODE_NUM_PER_SENDING;
    
    const unsigned short MAX_COMPRESSED_RANKING_PACKET_SIZE = 
        static_cast<unsigned short>(CMiniLZO::GetLeastCompressBuffer(MAX_RANKING_PACKET_SIZE));

    char* szDataBuffer = new char[MAX_RANKING_PACKET_SIZE];
    char* szCompressedBuffer = new char[MAX_COMPRESSED_RANKING_PACKET_SIZE];
	    
    if(0 != szDataBuffer && 0 != szCompressedBuffer)
    {
	    PktRankingInfo* lpRankingInfo = reinterpret_cast<PktRankingInfo*>(szDataBuffer);
	    RankingNode*    lpRankingNode = reinterpret_cast<RankingNode*>(lpRankingInfo + 1);
	    unsigned short  wNum = 0;

	    for (int nClassIndex = 0; nClassIndex < CClass::MAX_CLASS; ++nClassIndex)
	    {
		    for (int nRankIndex = 0; nRankIndex < MAX_DECLARE_RANK_NUM; ++nRankIndex)
		    {
			    if (0 == strcmp("", m_aryRanking[nClassIndex][nRankIndex].m_szCharName)) 
				    break; 

			    *lpRankingNode = m_aryRanking[nClassIndex][nRankIndex];
			    ++lpRankingNode;
			    ++wNum;

			    if (NODE_NUM_PER_SENDING <= wNum)
			    {
				    // NODE_NUM_PER_SENDING의 단위로 끊어서 전송한다. (패킷 크기 제한 때문)
				    lpRankingInfo->InitPtHead(
                        static_cast<unsigned short>(sizeof(PktRankingInfo) + sizeof(RankingNode) * wNum), 
					    CmdRankingInfo, 0, 0);

				    lpRankingInfo->m_wNum = wNum;

                    unsigned long dwCompressedSize = MAX_COMPRESSED_RANKING_PACKET_SIZE;
                    
                    if(PacketWrap::WrapCompress(szCompressedBuffer, dwCompressedSize, szDataBuffer, 
                        static_cast<unsigned short>(sizeof(PktRankingInfo) + sizeof(RankingNode) * wNum), 
                        CmdRankingInfo, 0, 0))
                    {
                        DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                            szCompressedBuffer, dwCompressedSize, CmdRankingInfo));
                    }
                    
					SERLOG2(g_Log, "CRankingDBMgr::SendRankingInfo : %d, Class : %d", wNum, nClassIndex);

				    lpRankingInfo = reinterpret_cast<PktRankingInfo*>(szDataBuffer);
				    lpRankingNode = reinterpret_cast<RankingNode*>(lpRankingInfo + 1);
				    wNum = 0;
			    }
		    }
	    }

	    // 남은 리스트를 마저 보낸다.
	    lpRankingInfo->m_wNum = wNum;
        unsigned long dwCompressedSize = MAX_COMPRESSED_RANKING_PACKET_SIZE;

        if(PacketWrap::WrapCompress(szCompressedBuffer, dwCompressedSize, szDataBuffer, 
            static_cast<unsigned short>(sizeof(PktRankingInfo) + sizeof(RankingNode) * wNum), 
            CmdRankingInfo, 0, 0))
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                szCompressedBuffer, dwCompressedSize, CmdRankingInfo));
        }

		SERLOG1(g_Log, "CRankingDBMgr::SendRankingInfo : %d End", wNum);
    }

    if(0 == szDataBuffer)
    {
        delete [] szDataBuffer;
        szDataBuffer = 0;
    }
    
    if(0 == szCompressedBuffer)
    {
        delete [] szCompressedBuffer;
        szCompressedBuffer = 0;
    }    
}

