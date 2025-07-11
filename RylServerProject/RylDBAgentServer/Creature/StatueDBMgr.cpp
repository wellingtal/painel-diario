#include "stdafx.h"

#include "StatueDBMgr.h"

#include <DB/DBComponent.h>
#include <Log/ServerLog.h>

#include <Utility/Setup/ServerSetup.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Creature/Monster/MonsterStructure.h>

CStatueDBMgr& CStatueDBMgr::GetInstance()
{
	static CStatueDBMgr ms_this;
	return ms_this;
}

CStatueDBMgr::CStatueDBMgr()
:	m_lpDBComponent(NULL)
{
}

CStatueDBMgr::~CStatueDBMgr()
{
	ClearStatueMap();
}

bool	CStatueDBMgr::Initialize(CDBComponent& DBComponent)
{
	ClearStatueMap();
	m_lpDBComponent = &DBComponent;

	// 컨텐츠 : 다크 카나번 국가 전쟁 or 신규존
	if (16 <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= 18)//wasnt commented hayzohar fix statue war bug was (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))
//		|| true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
	{
		const int MAX_ROWS = 1000;
		int nGetRows = 0;

		char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
		_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
			"SELECT "
			"TblStatueInfo.nIndex, "
			"TblStatueInfo.tnChannel, "
			"TblStatueInfo.snKID, "
			"TblStatueInfo.nLostHP FROM TblStatueInfo ");

		szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
		if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
		{
			ERRLOG0(g_Log, "Statue DB 얻어오기 실패");
			return false;
		}

		StatueInfoDB* statueInfo = new StatueInfoDB[MAX_ROWS];
		memset(statueInfo, 0, sizeof(StatueInfoDB) * MAX_ROWS);

		while (m_lpDBComponent->GetData((void**)statueInfo, sizeof(StatueInfoDB), MAX_ROWS, &nGetRows))
		{   
			if (0 == nGetRows) { break; }

			for (StatueInfoDB* lpStatueInfo = statueInfo; nGetRows > 0; --nGetRows, ++lpStatueInfo)
			{
				StatueInfoDB* lpStatue = CreateStatue(*lpStatueInfo);
				if (NULL != lpStatue)
				{
					unsigned long dwKey = CalcKeyValue(lpStatue->m_dwIndex, lpStatue->m_cChannel);
					if (!m_StatueMap.insert(std::make_pair(dwKey, lpStatue)).second)
					{
						ERRLOG2(g_Log, "Statue DB에 중복된 키값의 석상이 존재합니다. Index: %d / Channel: %d",
							lpStatue->m_dwIndex, lpStatue->m_cChannel);

						memset(statueInfo, 0, sizeof(StatueInfoDB) * MAX_ROWS);
						delete [] statueInfo;

						return false;
					}
				}
				else
				{
					ERRLOG2(g_Log, "Statue DB 값으로 석상정보를 생성하는데 실패했습니다. Index: %d / Channel: %d",
							lpStatueInfo->m_dwIndex, lpStatueInfo->m_cChannel);

					memset(statueInfo, 0, sizeof(StatueInfoDB) * MAX_ROWS);
					delete [] statueInfo;

					return false;
				}
			}

			memset(statueInfo, 0, sizeof(StatueInfoDB) * MAX_ROWS);
		}

		delete [] statueInfo;

		SERLOG0(g_Log, "석상 정보 Initalize 성공.");
	}

	return true;
}

unsigned long	CStatueDBMgr::CalcKeyValue(unsigned long dwIndex, unsigned char cChannel) const
{
	return static_cast<unsigned long>( (dwIndex << 16) | (cChannel) );
}

bool	CStatueDBMgr::UpdateKID(unsigned long dwIndex, unsigned char cChannel, unsigned short wNewKID)
{

	unsigned long dwKey = CalcKeyValue(dwIndex, cChannel);
	StatueMap::iterator itr = m_StatueMap.find(dwKey);
	if (itr == m_StatueMap.end())
	{
		return false;
	}

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
		"UPDATE TblStatueInfo SET snKID=%d WHERE nIndex=%d and tnChannel=%d",
		wNewKID, dwIndex, cChannel);
	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "Statue KID DB Update 실패");
		return false;
	}

	StatueInfoDB* lpStatue = itr->second;
	if (0 != lpStatue)
	{
		lpStatue->m_wKID = wNewKID;
	}
	else
	{
		ERRLOG0(g_Log, "Statue KID Update 실패");
		return false;
	}

	return true;
}

bool	CStatueDBMgr::UpdateLostHP(unsigned long dwIndex, unsigned char cChannel, unsigned long dwLostHP)
{
	unsigned long dwKey = CalcKeyValue(dwIndex, cChannel);
	StatueMap::iterator itr = m_StatueMap.find(dwKey);
	if (itr == m_StatueMap.end())
	{
		return false;
	}

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
		"UPDATE TblStatueInfo SET nLostHP=%d WHERE nIndex=%d and tnChannel=%d",
		dwLostHP, dwIndex, cChannel);
	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "Statue LostHP DB Update 실패");
		return false;
	}

	StatueInfoDB* lpStatue = itr->second;
	if (0 != lpStatue)
	{
		lpStatue->m_dwLostHP = dwLostHP;
	}
	else
	{
		ERRLOG0(g_Log, "Statue LostHP DB Update 실패");
		return false;
	}

	return true;
}

void	CStatueDBMgr::SendStatueInfo(CSendStream& SendStream, unsigned char cChannel)
{
	const unsigned int MAX_STATUE_INFO = sizeof(PktStatueCmd) + sizeof(StatueInfo) * 100;
	char szBuffer[MAX_STATUE_INFO];
	PktStatueCmd* lpPktStatueCmd = reinterpret_cast<PktStatueCmd*>(szBuffer);
	StatueInfo* lpStatueInfo = reinterpret_cast<StatueInfo*>(lpPktStatueCmd + 1);

	unsigned short wSize = 0;
	unsigned short wStatueNum = 0;

	StatueMap::const_iterator itr = m_StatueMap.begin();
	StatueMap::const_iterator end = m_StatueMap.end();

	while (itr != end)
	{
		const StatueInfoDB* lpStatue = itr->second;
		if (0 != lpStatue)
		{
			if (lpStatue->m_cChannel == cChannel)
			{
				lpStatueInfo->m_dwIndex		= lpStatue->m_dwIndex;
				lpStatueInfo->m_wKID		= lpStatue->m_wKID;
				lpStatueInfo->m_dwLostHP	= lpStatue->m_dwLostHP;

				++lpStatueInfo;
				++wStatueNum;
				wSize += sizeof(StatueInfo);
			}
		}
		else
		{
			ERRLOG0( g_Log, "Null StatueInfo 포인터가 포함되어 있습니다." ) ;
		}

		++itr;
	}

	if (wStatueNum > 0)
	{
		lpPktStatueCmd->m_wSubCmd	= PktStatueCmd::CMD_INFO;
		lpPktStatueCmd->m_dwValue	= wStatueNum;
		lpPktStatueCmd->m_wSize		= wSize;

		SendStream.WrapCompress(szBuffer, 
			static_cast<unsigned short>(sizeof(PktStatueCmd) + wSize), CmdStatueCmd, 0, 0);
	}
}

StatueInfoDB*	CStatueDBMgr::CreateStatue( const StatueInfoDB& statueInfo )
{
	StatueInfoDB* lpStatue = new StatueInfoDB;
	if (0 != lpStatue)
	{
		lpStatue->m_dwIndex		= statueInfo.m_dwIndex;
		lpStatue->m_cChannel	= statueInfo.m_cChannel;
		lpStatue->m_wKID		= statueInfo.m_wKID;
		lpStatue->m_dwLostHP	= statueInfo.m_dwLostHP;
	}

	return lpStatue;
}

void	CStatueDBMgr::ClearStatueMap()
{
	if (!m_StatueMap.empty())
	{
		StatueMap::iterator itr = m_StatueMap.begin();
		StatueMap::iterator end = m_StatueMap.end();

		while (itr != end)
		{
			if (itr->second)
			{
				delete itr->second;
                itr->second = NULL;
			}

			++itr;
		}

		m_StatueMap.clear();
	}
}
