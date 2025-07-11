
#include "stdafx.h"

#include <algorithm>

#include <Log/ServerLog.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/RankingPacket.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

#include "RankingMgr.h"

CRankingMgr& CRankingMgr::GetInstance()
{
    static CRankingMgr rankingMgr;
    return rankingMgr;
}

CRankingMgr::CRankingMgr()
{

}


CRankingMgr::~CRankingMgr()
{

}


void CRankingMgr::UpdateRanking(const RankingNode& node)
{
	if (CClass::JOB_CHANGE_1ST != CClass::GetJobLevel(node.m_cClass)) 
	{ 
		return; 
	}

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

			// �̹� ��Ŀ�� ��� �ֽ� ������ ������Ʈ
			m_aryRanking[node.m_cClass][nRankIndex] = node;
			bUpdate = true;
			break;
		}
	}

	if (!bUpdate)
	{
		if (m_aryRanking[node.m_cClass][MAX_DECLARE_RANK_NUM-1] < node)
		{
			// ��ŷ ����Ʈ�� ���� (���������� ��ġ�� ����)
			m_aryRanking[node.m_cClass][MAX_DECLARE_RANK_NUM-1] = node;
			bUpdate = true;
		}
	}

	if (bUpdate)
	{
		std::sort(&m_aryRanking[node.m_cClass][0], &m_aryRanking[node.m_cClass][MAX_DECLARE_RANK_NUM], 
			RankingNode::ComparePoint());

		//  �߰� ������ ������Ʈ ��� ���� ������ �˷�������, ���� ������ Ŭ���̾�Ʈ�� ��û�� �ÿ��� �Ѱ��ش�.
        //  SendRankingInfo(node);//was commented out hz rank
	}
}

bool CRankingMgr::SendRankingInfo(unsigned long dwCID, unsigned char cClass, unsigned char cPage)
{
	// Ŭ������ 0�� ��� ��ü ������ �������Ѵ�.
	if (0 != cClass && CClass::JOB_CHANGE_1ST != CClass::GetJobLevel(cClass)) 
	{ 
        ERRLOG2(g_Log, "CID:%10u / Class:%d / �߸��� Ŭ������ ��ŷ�� ��û�Ͽ����ϴ�.", dwCID, cClass);
		return true; 
	}

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpCharacter)
	{
		ERRLOG1(g_Log, "CID:%10u / ������ ���������ʴ� ĳ���Ͱ� ��ŷ ����Ʈ�� ��û�Ͽ����ϴ�.", dwCID);
		return true;
	}

	const int MAX_BUFFER = sizeof(PktRankingInfo) + 
        PktRankingInfo::MAX_NUM_PER_PAGE * sizeof(RankingNode);

	char szBuffer[MAX_BUFFER];

	PktRankingInfo* lpRankingInfo	= reinterpret_cast<PktRankingInfo*>(szBuffer);
	RankingNode*	lpRankingNode	= reinterpret_cast<RankingNode*>(lpRankingInfo + 1);                

	lpRankingInfo->m_dwCharID = dwCID;
	lpRankingInfo->m_cClass = cClass;
	lpRankingInfo->m_cPage = cPage;

	lpRankingInfo->m_wNum = 0;

	unsigned short wError = PktBase::NO_SERVER_ERR;

	if (0 == cClass)
	{
		unsigned char cNation = lpCharacter->GetRace();
       // switch (cNation)//was CClass::HUMAN: and CClass::AKHAN:
		//{
			//case CClass::HUMAN://all together rank fix hz
				std::partial_sort_copy(&m_aryRanking[CClass::Defender][0], &m_aryRanking[CClass::ShadowOff/*CClass::Cleric*/][MAX_DECLARE_RANK_NUM], 
					&m_aryRanking[0][0], &m_aryRanking[0][MAX_DECLARE_RANK_NUM], RankingNode::ComparePoint());
				//break;

			/*case CClass::AKHAN:
				std::partial_sort_copy(&m_aryRanking[CClass::Templar][0], &m_aryRanking[CClass::ShadowOff][MAX_DECLARE_RANK_NUM], 
					&m_aryRanking[0][0], &m_aryRanking[0][MAX_DECLARE_RANK_NUM], RankingNode::ComparePoint());
				break;*///wasnt commented out
		//}
	}

	for (int nIndex = 0; nIndex < PktRankingInfo::MAX_NUM_PER_PAGE; ++nIndex)
	{
		unsigned char cCurrentIndex = cPage * PktRankingInfo::MAX_NUM_PER_PAGE + nIndex;
		if (0 > cCurrentIndex || cCurrentIndex >= MAX_DECLARE_RANK_NUM)
		{
			wError = PktRankingInfo::FAIL_INVALID_PAGE;
			ERRLOG2(g_Log, "��ŷ ���� Class : %d / %d.", cClass, cCurrentIndex);
			break;
		}

		if (0 == strcmp("", m_aryRanking[cClass][cCurrentIndex].m_szCharName))
		{
			if (0 == nIndex)
			{
				wError = PktRankingInfo::FAIL_INVALID_PAGE;
				ERRLOG3(g_Log, "��ŷ ���� Class : %d / %d, Name : %s", cClass, cCurrentIndex, m_aryRanking[cClass][cCurrentIndex].m_szCharName);
			}
			break;
		}

		*lpRankingNode = m_aryRanking[cClass][cCurrentIndex];
		++lpRankingNode;
		++lpRankingInfo->m_wNum;
	}

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		CSendStream& SendStream = lpDispatch->GetSendStream();
		return SendStream.WrapCompress(szBuffer, sizeof(PktRankingInfo) + lpRankingInfo->m_wNum * sizeof(RankingNode), 
			CmdRankingInfo, 0, wError);
	}

	return true;
}

unsigned char CRankingMgr::GetRank(const char* szCharName, unsigned char cClass)
{
	if(cClass >= CClass::MAX_CLASS)
		cClass = 0;

	for (int nIndex = 0; nIndex < MAX_DECLARE_RANK_NUM; ++nIndex)
	{
		if (0 == strcmp(szCharName, m_aryRanking[cClass][nIndex].m_szCharName))
		{
			return (nIndex + 1);
		}
	}

	// ���� ��
	return 0;
}
