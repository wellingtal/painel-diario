
#include "stdafx.h"

#include <Log/ServerLog.h>
#include <DB/DBComponent.h>
#include <Castle/CastleDBComponent.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Community/Guild/GuildDBMgr.h>
#include <Community/Guild/Guild.h>

#include "CampShopDB.h"

CCampShopDB::CCampShopDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo)
: CCampDB(DBComponent, CampInfo), m_dwItemContainerSize(0), m_cItemNum(0), m_dwTempSafe(0), m_cTax(0)
{
	std::fill_n(m_aryItemContainer, int(CampShopInfoDB::MAX_CONTAINER_SIZE), 0);
}

CCampShopDB::~CCampShopDB()
{
}

Siege::eReturn CCampShopDB::Process()
{
	if (m_cSiegeCount >= Siege::CAMP_SHOP_TRANSFER_COUNT)
	{
		Guild::CGuild* lpGuild = Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID);
		if (NULL != lpGuild)
		{
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 메세지 전송
			PktCampMessage pktCampMessage;
			::memset(&pktCampMessage, 0, sizeof(PktCampMessage));

			// 길드 요새 상점 세금 수익금 획득
			// 메세지 종류, 존 번호, 공지 아님, 수익금
			pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_SHOP_GAIN_TAX;
			pktCampMessage.m_dwCampID	= GetCampID();
			pktCampMessage.m_dwGID		= GetGID();
			pktCampMessage.m_cZone		= GetZone();
			pktCampMessage.m_bNotify	= false;
			pktCampMessage.m_nValue1	= m_dwTempSafe;

			if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), 
				CmdCampMessage, 0, PktBase::NO_SERVER_ERR))
			{
				DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
					reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			SendCampCmd(PktCampCmd::CAMP_SHOP_GAIN_TAX, m_dwTempSafe);

            DETLOG3(g_Log, "GID:%10u / CurrentGuildGold:%u / TempSafeGold:%u / 길드 요새 상점 : 그동안 모인 수익금을 길드 요새에 입금합니다",
                lpGuild->GetGID(), lpGuild->GetGold(), m_dwTempSafe);
                
			lpGuild->AddGold(m_dwTempSafe);
			m_dwTempSafe = 0;

			SetSiegeCount(0);
			DBComponent::CastleDB::UpdateCampSiegeCount(m_DBComponent, GetCampID(), GetSiegeCount());
		}
	}
		
	return CCampDB::Process();
}

bool CCampShopDB::UpdateCampInfo(const char* aryItemBuffer, unsigned long dwItemBufferSize, unsigned char cItemNum, 
								 unsigned long dwTempSafe, unsigned char cTax)
{
	return DBComponent::CastleDB::UpdateCampShopInfo(m_DBComponent, GetCampID(), 
		aryItemBuffer, dwItemBufferSize, cItemNum, dwTempSafe, cTax);
}

bool CCampShopDB::SerializeOut(char* szBuffer_Out, unsigned short& usBufferSize_Out, unsigned long& dwCID_Out)
{
	if (true == CCampDB::SerializeOut(szBuffer_Out, usBufferSize_Out, dwCID_Out))
	{
		char* lpBuffer = szBuffer_Out + usBufferSize_Out;
		if (NULL == lpBuffer)
		{
			return false;
		}

		// 길드 요새 상점 정보
		CampShopInfo* lpCampShopInfo = reinterpret_cast<CampShopInfo*>(lpBuffer);

		lpCampShopInfo->m_dwTempSafe	= m_dwTempSafe;
		lpCampShopInfo->m_cTax			= m_cTax;
		lpCampShopInfo->m_dwBufferSize	= m_dwItemContainerSize;
		lpCampShopInfo->m_cItemNum		= m_cItemNum;

		unsigned short wSize = static_cast<unsigned short>(m_dwItemContainerSize + sizeof(unsigned long) * m_cItemNum);

		// 아이템 버퍼
		char* lpItemBuffer = reinterpret_cast<char*>(lpCampShopInfo + 1);
		std::copy(m_aryItemContainer, m_aryItemContainer + wSize, lpItemBuffer);

		usBufferSize_Out += sizeof(CampShopInfo) + wSize;

		return true;
	}

	return false;
}

