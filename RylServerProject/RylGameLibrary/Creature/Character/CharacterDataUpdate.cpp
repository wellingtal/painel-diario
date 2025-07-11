#include "stdafx.h"
#include "Character.h"

#include <Community/Party/Party.h>
#include <Community/Guild/GuildConstants.h>
#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>
#include <Utility/Math/Math.h>
#include <Log/LogCommands.h>
#include <Log/CharacterLog.h>
#include <Item/Container/ItemContainer.h>
#include <Item/ItemFactory.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacket.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <RylChatServer/ChatGameServerDispatch.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

bool CCharacter::DBUpdate(DBUpdateData::UpdateType eUpdateType)
{
	if(!IsOperationFlagSet(CHAR_INFO_LOADED) || (m_bLogout && DBUpdateData::LOGOUT != eUpdateType))
	{
		return false;
	}

	if(--m_nDBUpdateCount > 0)
	{
		return false;
	}

	m_nDBUpdateCount = DBUPDATE_COUNT;

	int		nTotalSize = sizeof(PktDBUpdate) + DBUpdateData::MAX_DBUPDATE_SIZE;
    char	szCharInfo[sizeof(PktDBUpdate) + DBUpdateData::MAX_DBUPDATE_SIZE];
    char*	lpCharacterInfo = szCharInfo + sizeof(PktDBUpdate);
    
	PktDBUpdate* lpPktDBUpdate = reinterpret_cast<PktDBUpdate*>(szCharInfo);
    memset(lpPktDBUpdate, 0, sizeof(PktDBUpdate));
	
    unsigned short usLogError = 0;
    unsigned char  cLogCMD    = (DBUpdateData::LOGOUT == eUpdateType) ? 
        GAMELOG::CMD::CHAR_LOGOUT : GAMELOG::CMD::CHAR_DBUPDATE;

	unsigned char cAdmin = (true == IsAdmin()) ? 1 : 0;

	if(!GetCharacterInfo(lpCharacterInfo, &nTotalSize, lpPktDBUpdate->m_usUpdate))
	{
        nTotalSize = 0; usLogError = 1;
		ERRLOG1(g_Log, "CID:0x%08x DBUpdate실패 : 데이터를 복사해 올 수 없습니다.", m_dwCID);
	}
    else
    {
		GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
			CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

		if (0 == lpDBAgentDispatch)
        {
            usLogError = 2;
            ERRLOG1(g_Log, "CID:0x%08x DBUpdate실패 : DBAgentDispatch 를 얻을 수 없습니다.", m_dwCID);
        }
        else
	    {
            CSendStream& AgentSendStream    = lpDBAgentDispatch->GetSendStream();

            // 창고 데이터 업데이트(창고가 열려 있으면 업데이트한다.)
            // 순서 주의! DB에 Update할 때는 창고 업데이트 후 캐릭터를 업데이트한다.
            if(!m_Deposit.DBUpdate(AgentSendStream))
            {
                ERRLOG1(g_Log, "CID:0x%08x 창고 업데이트 실패", m_dwCID);
            }

            lpPktDBUpdate->m_dlItemSerial	= Item::CItemFactory::GetInstance().GetItemUID();
            lpPktDBUpdate->m_dwSessionID    = m_dwSessionID;
		    lpPktDBUpdate->m_dwUserID		= m_dwUID;
		    lpPktDBUpdate->m_dwCharID		= m_dwCID;
            lpPktDBUpdate->m_TypeCode		= eUpdateType;

		    lpPktDBUpdate->m_dwRequestKey   = 0;
            lpPktDBUpdate->m_Address.S_un.S_addr = 0;
            lpPktDBUpdate->m_cAdminLevel    = 0;
			
            // WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현            
			lpPktDBUpdate->m_cAccountNation		= 0;
            lpPktDBUpdate->m_cNameChangeCount	= 0;
			lpPktDBUpdate->m_cGuildWarFlag		= 0;
			lpPktDBUpdate->m_cRealmWarFlag		= 0;
			lpPktDBUpdate->m_cRealmPoint		= 0;
			lpPktDBUpdate->m_cTacticsFlag		= 0;
        
            if(!AgentSendStream.WrapCompress(reinterpret_cast<char*>(lpPktDBUpdate),
                static_cast<unsigned short>(sizeof(PktDBUpdate) + nTotalSize), CmdDBUpdateData, 0, 0))
            {
                usLogError = 3;
                ERRLOG1(g_Log, "CID:0x%08x DBUpdate실패 : WrapCompress를 실패했습니다.", m_dwCID);
            }
        }
	}

	if(DBUpdateData::LOGOUT == eUpdateType || 0 != usLogError)
	{
        SOCKADDR_IN remoteAddr;
        if (0 != m_lpGameClientDispatch)
        {
            remoteAddr = m_lpGameClientDispatch->GetRemoteAddr().get_addr_in();
        }
        else
        {
            memset(&remoteAddr, 0, sizeof(SOCKADDR_IN));
        }

		GAMELOG::LogCharLoginOut(m_dwUID, this, &remoteAddr, lpCharacterInfo, nTotalSize, 
			lpPktDBUpdate->m_usUpdate, cLogCMD, usLogError);
	}

	LOG_INOUT(

		char szExp[64];
        const char* szUpdateType = "Unknown";
        
        switch (eUpdateType)
        {
        case DBUpdateData::LOGIN:         szUpdateType = "Login";         break;
        case DBUpdateData::LOGOUT:        szUpdateType = "Logout";        break;
        case DBUpdateData::UPDATE:        szUpdateType = "Update";        break;
        case DBUpdateData::ADMIN_LOGIN:   szUpdateType = "AdminLogin";    break;
        case DBUpdateData::ZONEMOVE:      szUpdateType = "ZoneMove";      break;
        }

        unsigned long dwDispatchUID = (NULL != m_lpGameClientDispatch) ? 
            m_lpGameClientDispatch->GetUID() : 0;

		Math::Convert::Hex64ToStr(szExp, m_DBData.m_Info.Exp);
        DETLOG8(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchUID:%d 의 캐릭터 정보를 DBAgent에 업데이트합니다. "
            " 업데이트 타입은 %s입니다. 몇가지 기본 정보를 찍습니다. %s(lev:%2d, exp:%s)",
			m_dwUID, m_dwCID, this, dwDispatchUID, szUpdateType, m_DBData.m_Info.Name, 
            m_DBData.m_Info.Level, szExp)
	);

	return true;
}


bool CCharacter::GetCharacterInfo(char* pBuffer, int* nBufferSize_InOut, unsigned short* lpUpdateLen)
{
	unsigned long dwSize = 0;
	unsigned short usTotalSize = 0;	

    if(!IsOperationFlagSet(CHAR_INFO_LOADED))
    {
        ERRLOG1(g_Log, "CID:0x%08x 캐릭터 정보가 세팅되지 않은 상태에서 캐릭터 정보를 얻으려 하였습니다.",
            m_dwCID);
        return false;
    }

    SaveToDBData();

	for (int nCount = 0; nCount < DBUpdateData::MAX_UPDATE_DB; ++nCount) 
	{
		switch (nCount)
		{
			case DBUpdateData::STATUS_UPDATE:
				{
					*reinterpret_cast<CHAR_INFOST*>(pBuffer) = m_DBData.m_Info;
					lpUpdateLen[nCount] = sizeof(CHAR_INFOST);
				}
				break;

			case DBUpdateData::POSITION_UPDATE:
				*reinterpret_cast<CHAR_POS*>(pBuffer) = m_DBData.m_Pos;
				lpUpdateLen[nCount] = sizeof(CHAR_POS);
				break;

			case DBUpdateData::SKILL_UPDATE:
				*reinterpret_cast<SKILL*>(pBuffer) = m_DBData.m_Skill;
				lpUpdateLen[nCount] = sizeof(SKILL);
				break;

			case DBUpdateData::QUICKSLOT_UPDATE:
				*reinterpret_cast<QUICK*>(pBuffer) = m_DBData.m_Quick;	
				lpUpdateLen[nCount] = sizeof(QUICK);
				break;

			case DBUpdateData::SPELL_UPDATE:
				*reinterpret_cast<SPELL*>(pBuffer) = m_DBData.m_Spell;	
				lpUpdateLen[nCount] = sizeof(SPELL);
				break;

			case DBUpdateData::ITEM_EQUIP_UPDATE:
				dwSize = *nBufferSize_InOut;	m_Equipments.SerializeOut(pBuffer, dwSize);
				lpUpdateLen[nCount] = static_cast<unsigned short>(dwSize);
				break;

			case DBUpdateData::ITEM_INVEN_UPDATE:
				dwSize = *nBufferSize_InOut;	m_Inventory.SerializeOut(pBuffer, dwSize);
				lpUpdateLen[nCount] = static_cast<unsigned short>(dwSize);
				break;
	
			case DBUpdateData::ITEM_EXTRA_UPDATE:
				dwSize = *nBufferSize_InOut;	m_ExtraSpace.SerializeOut(pBuffer, dwSize);
				lpUpdateLen[nCount] = static_cast<unsigned short>(dwSize);
				break;
	
			case DBUpdateData::ITEM_EXCHANGE_UPDATE:
				dwSize = *nBufferSize_InOut;	m_Exchange.SerializeOut(pBuffer, dwSize);
				lpUpdateLen[nCount] = static_cast<unsigned short>(dwSize);
				break;

			case DBUpdateData::ITEM_TEMPINVEN_UPDATE:
				dwSize = *nBufferSize_InOut;	m_TempInven.SerializeOut(pBuffer, dwSize);
				lpUpdateLen[nCount] = static_cast<unsigned short>(dwSize);
				break;		
		}

		pBuffer += lpUpdateLen[nCount];
		usTotalSize += lpUpdateLen[nCount];
		*nBufferSize_InOut -= dwSize;
	}

	*nBufferSize_InOut = usTotalSize;
	return true;
}




bool CCharacter::SetCharacterInfo(char* pBuffer, unsigned short usUpdateLen[DBUpdateData::MAX_UPDATE_DB])
{
	for (int nCount = 0; nCount < DBUpdateData::MAX_UPDATE_DB; ++nCount)
	{
        unsigned long dwUpdateLen = usUpdateLen[nCount];

		switch (nCount)
		{            
			case DBUpdateData::STATUS_UPDATE:		    m_DBData.m_Info = *reinterpret_cast<CHAR_INFOST*>(pBuffer);	break;
			case DBUpdateData::POSITION_UPDATE:		    m_DBData.m_Pos = *reinterpret_cast<CHAR_POS*>(pBuffer);		break;
			case DBUpdateData::SKILL_UPDATE:			m_DBData.m_Skill = *reinterpret_cast<SKILL*>(pBuffer);		break;
			case DBUpdateData::QUICKSLOT_UPDATE:		m_DBData.m_Quick = *reinterpret_cast<QUICK*>(pBuffer);		break;
			case DBUpdateData::SPELL_UPDATE:			m_DBData.m_Spell = *reinterpret_cast<SPELL*>(pBuffer);		break;
			case DBUpdateData::ITEM_EQUIP_UPDATE:	    m_Equipments.SerializeIn(pBuffer, dwUpdateLen);	            break;
			case DBUpdateData::ITEM_INVEN_UPDATE:	    m_Inventory.SerializeIn(pBuffer, dwUpdateLen);	            break;
			case DBUpdateData::ITEM_EXTRA_UPDATE:	    m_ExtraSpace.SerializeIn(pBuffer, dwUpdateLen);       	    break;
			case DBUpdateData::ITEM_EXCHANGE_UPDATE:	m_Exchange.SerializeIn(pBuffer, dwUpdateLen);		        break;
			case DBUpdateData::ITEM_TEMPINVEN_UPDATE:	m_TempInven.SerializeIn(pBuffer, dwUpdateLen);				break;
		}

		pBuffer += dwUpdateLen;
	}

	m_CreatureStatus.Init(m_DBData.m_Info);	
	m_CurrentPos.m_fPointX = m_DBData.m_Pos.LastPoint.fPointX;
	m_CurrentPos.m_fPointY = m_DBData.m_Pos.LastPoint.fPointY;
	m_CurrentPos.m_fPointZ = m_DBData.m_Pos.LastPoint.fPointZ;

	return CalculateStatusData(false);
}


bool CCharacter::MoveZoneProcess(unsigned long dwServerID) 
{
    // 파티에 존이동했음을 보냄    
    if (0 != m_pParty)
    {
        CCharacterParty* lpParty = static_cast<CCharacterParty*>(m_pParty);
        lpParty->SendPartyMemberDataToDBAgent(m_dwCID, 0, 0, dwServerID, 0,
            GetCharacterName(), PktDD::SCmdMoveZonePartyMem);
    }	

	// 친구 리스트에 존이동 메세지를 보낸다 //
	FriendInfoUpdate(GetUID(), GetCID(), GetGID(), GetClass(), GetLevel(), dwServerID);
	
    m_bLogout = true; 
    SetOperationFlag(CHAR_ZONE_MOVED);
    return Logout(DBUpdateData::ZONEMOVE);
}

void CCharacter::FriendInfoUpdate(unsigned long dwUID, unsigned long dwCID, unsigned long dwGID, unsigned short wClass,
								  char cLevel, unsigned long dwServerID)
{
	// 친구 리스트에 업데이트 메세지를 보낸다 //

	GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, CChatDispatch::GetDispatchTable());

	if(lpChatDispatch)
	{
		char* lpBuffer = lpChatDispatch->GetSendStream().GetBuffer(sizeof(PktFriendDB));

		if(lpBuffer)
		{
			PktFriendDB* lpPktFriendDB = reinterpret_cast<PktFriendDB*>(lpBuffer);

			lpPktFriendDB->m_dwOwnerUID		= dwUID;
			lpPktFriendDB->m_dwOwnerCID		= dwCID;
			lpPktFriendDB->m_dwReferenceUID	= 0;
			lpPktFriendDB->m_dwReferenceCID	= 0;
			lpPktFriendDB->m_dwData			= 0;
			lpPktFriendDB->m_cCmd			= PktFriendDB::FRIEND_INFO_UPDATE;
			lpPktFriendDB->m_dwGID			= dwGID;
			lpPktFriendDB->m_wClass			= wClass;
			lpPktFriendDB->m_cLevel			= cLevel;
			lpPktFriendDB->m_dwServerID		= dwServerID;

			lpChatDispatch->GetSendStream().WrapCrypt(sizeof(PktFriendDB), CmdFriendDB, 0, 0);
		}		
	}
}

bool CCharacter::ItemDump(char* pBuffer, int* nBufferSize_InOut) const
{
	using namespace GAMELOG;

	sItemDump*	lpItemDump = reinterpret_cast<sItemDump*>(pBuffer);
	char*		lpItems = reinterpret_cast<char*>(&lpItemDump[1]);

	std::fill_n(lpItemDump->m_usDataSize, int(sItemDump::MAX_DUMP), 0);

	unsigned long   dwSize = 0;
	unsigned short	usTotalSize = sizeof(sItemDump);	

	for (int nCount = 0; nCount < sItemDump::MAX_DUMP; ++nCount)
	{
		switch (nCount)
		{
			case sItemDump::EQUIP_DUMP:
				dwSize = *nBufferSize_InOut;
				m_Equipments.SerializeOut(lpItems, dwSize);
				lpItemDump->m_usDataSize[nCount] = static_cast<unsigned short>(dwSize);
				break;

			case sItemDump::INVEN_DUMP:
				dwSize = *nBufferSize_InOut;
				m_Inventory.SerializeOut(lpItems, dwSize);
				lpItemDump->m_usDataSize[nCount] = static_cast<unsigned short>(dwSize);
				break;

			case sItemDump::EXTRA_DUMP:
				dwSize = *nBufferSize_InOut;
				m_ExtraSpace.SerializeOut(lpItems, dwSize);
				lpItemDump->m_usDataSize[nCount] = static_cast<unsigned short>(dwSize);
				break;

			case sItemDump::EXCHANGE_DUMP:
				dwSize = *nBufferSize_InOut;
				m_Exchange.SerializeOut(lpItems, dwSize);
				lpItemDump->m_usDataSize[nCount] = static_cast<unsigned short>(dwSize);
				break;
		}

		lpItems += lpItemDump->m_usDataSize[nCount];
		usTotalSize += lpItemDump->m_usDataSize[nCount];
		*nBufferSize_InOut -= dwSize;
	}

	*nBufferSize_InOut = usTotalSize;
	return true;
}
