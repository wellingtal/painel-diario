
#include "stdafx.h"

#include <Network/Session/Session.h>
#include <Network/Winsock/SocketFactory.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharBGServer.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/BGServerPacket.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

#include <Utility/Setup/ServerSetup.h>
#include <Creature/Character/TempCharacter.h>
#include <Item/ItemFactory.h>

#include <DB/DBComponent.h>
#include <DB/AuthDBComponent.h>

#include <Log/LogStruct.h>
#include <Log/ItemLog.h>

#include "RegularAgentDispatch.h"

RylServerInfo CRegularAgentDispatch::ms_AgentServerInfo[SERVER_ID::MAX_GROUP_NUM];

CMultiDispatch& CRegularAgentDispatch::GetDispatchTable(void)
{
	static CMultiDispatch multiDispatch;
	return multiDispatch;
}	

CTempCharacterMgr& CRegularAgentDispatch::GetTempCharacterMgr(void)
{
	static CTempCharacterMgr tempCharacterMgr;
	return tempCharacterMgr;
}	


CRegularAgentDispatch::CRegularAgentDispatch(CSession& Session)
:	CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),	m_cGroup(0)
{

}

CRegularAgentDispatch::~CRegularAgentDispatch()
{

}

bool CRegularAgentDispatch::Initialize(void)
{
	// 파일 열기
	const char szGroupFileName[MAX_PATH] = "./Script/Server/ServerGroup.txt";
	const char szIPFileName[MAX_PATH] = "./Script/Server/ServerIP.txt";

	FILE *lpGroupFile = fopen(szGroupFileName, "rt");
	FILE *lpIPFile = fopen(szIPFileName, "rt");

	const int MAX_BUFFERSIZE = 256;
	char szGroupBuffer[MAX_BUFFERSIZE];
	char szIPBuffer[MAX_BUFFERSIZE];

	if (NULL == lpGroupFile || NULL == lpIPFile)
	{
		ERRLOG2(g_Log, "Cannot load server group/IP file  : %s or %s", szGroupFileName, szIPFileName);
		return false;
	}

	for (int nGroup = 0; nGroup < SERVER_ID::MAX_GROUP_NUM; )
	{
		if (NULL == fgets(szGroupBuffer, MAX_BUFFERSIZE, lpGroupFile) || 
			NULL == fgets(szIPBuffer, MAX_BUFFERSIZE, lpIPFile))
		{
			break;
		}

		nGroup = atoi(strtok(szIPBuffer, "\n:"));
		ms_AgentServerInfo[nGroup].m_ServerUID.sID.Group = nGroup;
		ms_AgentServerInfo[nGroup].m_ServerAddress.S_un.S_addr = inet_addr(strtok(NULL, "\n:"));

		ms_AgentServerInfo[nGroup].m_usChannelClientNum[0] = 1;
		ms_AgentServerInfo[nGroup].m_usChannelClientNum[1] = 2;
		ms_AgentServerInfo[nGroup].m_usChannelNum = 2;

		strncpy(ms_AgentServerInfo[nGroup].m_szGroupName, strtok(szGroupBuffer, "\n"), 
			RylServerInfo::GROUP_NAME_LENGTH - 1);
		ms_AgentServerInfo[nGroup].m_szGroupName[RylServerInfo::GROUP_NAME_LENGTH - 1] = '\0';
		ms_AgentServerInfo[nGroup].m_nGroupNameLen = strlen(ms_AgentServerInfo[nGroup].m_szGroupName);
	}

	fclose(lpGroupFile);
	fclose(lpIPFile);

	return true;
}

void CRegularAgentDispatch::Connected()
{
	int nGroup = 0;
	for (; nGroup < SERVER_ID::MAX_GROUP_NUM; ++nGroup)
	{
		if (ms_AgentServerInfo[nGroup].m_ServerAddress.S_un.S_addr == 
			GetRemoteAddr().get_addr_in().sin_addr.S_un.S_addr)
		{
			break;
		}
	}

	if (SERVER_ID::MAX_GROUP_NUM == nGroup)
	{
		ERRLOG0(g_Log, "정식 중계 서버와 연결할 수 없습니다.");
		Shutdown();
		return;
	}

	char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktSL));
	if (NULL != lpBuffer)
	{
		PktSL* lpPktSL = reinterpret_cast<PktSL*>(lpBuffer);

		const int MAX_ADDRESS = 128;
		char szAddress[MAX_ADDRESS];

		CTCPFactory tcpFactory;		
		tcpFactory.GetNetworkInfo(szAddress, MAX_ADDRESS);

		lpPktSL->m_Address.S_un.S_addr = inet_addr(szAddress);

		SERVER_ID serverID = {0,};

		serverID.sID.Type		= CServerSetup::AdminToolServer;
		serverID.sID.Group		= nGroup;
		serverID.sID.Channel	= CServerSetup::GetInstance().GetChannelFromCmdLine() + 2;
		serverID.sID.ID			= SERVER_ID::ADMIN_TOOL_ZONE;

		lpPktSL->m_dwServerID	= serverID.dwID;

		if (m_SendStream.WrapHeader(sizeof(PktSL), CmdSysServerLogin, 0, 0))
		{
			INFLOG2(g_Log, "중계세션(0x%p)에 서버(0x%08x)의 로그인패킷을 전송합니다.", this, serverID);
		}
	}
}

void CRegularAgentDispatch::Disconnected()
{
	INFLOG2(g_Log, "중계세션(0x%p)과 서버(%d)의 연결을 끊습니다.", this, m_cGroup);
	GetDispatchTable().RemoveDispatch(m_cGroup);
}

bool CRegularAgentDispatch::DispatchPacket(PktBase* lpPktBase)
{
	switch (lpPktBase->GetCmd())
	{
		case CmdSysServerLogin:		return ParseServerLogin(lpPktBase);

		case CmdBGServerCharSlot:	return RegularAgentPacketParse::ParseGetCharSlot(lpPktBase);
		case CmdAdminToolGetData:	return RegularAgentPacketParse::ParseGetCharData(lpPktBase, m_cGroup);

		default:
			// 패킷 Command가 invaild한지 본다. invalid한 경우는 다시 검색해서 vaild한 위치를 찾아낸다.		
			LogErrorPacket("패킷 헤더의 패킷 커맨드가 잘못되었습니다.", lpPktBase->GetCmd());
			break;
	}

	return true;
}

bool CRegularAgentDispatch::ParseServerLogin(PktBase* lpPktBase)
{
	PktSL* lpPktSL = static_cast<PktSL *>(lpPktBase);

	SERVER_ID serverID = {0,};
	serverID.dwID = lpPktSL->m_dwServerID;
	m_cGroup = serverID.sID.Group;

	if (false == GetDispatchTable().SetDispatch(m_cGroup, this))
	{
		ERRLOG3(g_Log, "DP:0x%p/IP:%s/Duplicated servergroup login : %d",
			this, GetRemoteAddr().get_addr_string(), m_cGroup);

		return false;
	}

	DETLOG3(g_Log, "DP:0x%p/IP:%s/Servergroup login : %d",
		this, GetRemoteAddr().get_addr_string(), m_cGroup);

	return true;
}

bool RegularAgentPacketParse::ParseGetCharSlot(PktBase* lpPktBase)
{
	PktBGServerCharSlotAck* lpPktCharSlotAck = static_cast<PktBGServerCharSlotAck *>(lpPktBase);

	// TODO : 현재는 임의의 캐릭터 하나를 골라 바로 정보를 요청합니다. (창고 정보만 얻으면 되므로...)
	//	추후 환전소에 캐릭터 선택 기능이 추가되면 클라이언트에 PktBGServerCharSlotAck를 보내서 선택할 수 있도록 수정해야 합니다.

	unsigned long dwCID = lpPktCharSlotAck->m_dwCID;
	unsigned char cGroup = lpPktCharSlotAck->m_cGroup;

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpCharacter)
	{
		ERRLOG1(g_Log, "CID:0x%08x 캐릭터가 없습니다.", dwCID);
		return false;
	}

    CTempCharacter* lpTempCharacter = CRegularAgentDispatch::GetTempCharacterMgr().GetCharacter(dwCID, cGroup);
    if(0 != lpTempCharacter)
    {
	    unsigned long dwTempUID = 0;
        if(DBComponent::AuthDB::GetUIDFromBattleUID(
            CDBSingleObject::GetInstance(), lpCharacter->GetUID(), &dwTempUID))
        {
			int nCount = 0;
            for(; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
            {
                if(0 != lpPktCharSlotAck->m_dwSlotCID[nCount])
                {
                    lpTempCharacter->SetUID(dwTempUID);
                    lpTempCharacter->SetCID(lpPktCharSlotAck->m_dwSlotCID[nCount]);
                    lpTempCharacter->SetCharacterName(lpPktCharSlotAck->m_szSlotName[nCount]);

                    lpTempCharacter->SetData(CTempCharacter::SLOT_INFO_SET);

                    DETLOG5(g_Log, "UID:%10u/CID:%10u/정섭 UID얻어오기 성공 : 정섭에 캐릭터 데이터 요청 : UID:%10u/CID:%10u/Group:%d/", 
                        lpCharacter->GetUID(), dwCID, dwTempUID, lpPktCharSlotAck->m_dwSlotCID[nCount], cGroup);

                    if(SendGetCharData(dwTempUID, lpPktCharSlotAck->m_dwSlotCID[nCount], 
                        lpPktCharSlotAck->m_szSlotName[nCount], dwCID, cGroup))
                    {
                        lpTempCharacter->AddDataRequestCount();
                    }

                    break;
                }
            }

            // 캐릭터가 한마리도 없는 경우.
            if(nCount == USER_INFO::MAX_CHAR_NUM)
            {
                CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
                if (NULL != lpDispatch)
                {
                    GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), dwCID, cGroup, 
                        PktBGServerMileageChange::MC_INFO, 0, lpCharacter->GetMileage(), PktBGServerMileageChange::NONE_CHARACTER);
                }
            }
        }
        else
        {
            ERRLOG3(g_Log, "UID:%10u/CID:%10u/정섭 UID얻어오기 실패 : %s", 
                lpCharacter->GetUID(), dwCID, CDBSingleObject::GetInstance().GetErrorString());
        }
    }
    else
    {
        ERRLOG2(g_Log, "UID:%10u/CID:%10u/정섭 UID얻어오기 실패 : 임시 캐릭터 생성 실패", lpCharacter->GetUID(), dwCID);
    }

	return true;
}

bool RegularAgentPacketParse::ParseGetCharData(PktBase* lpPktBase, unsigned char cGroup)
{
	PktAdminToolGetDataAck* lpPktGetDataAck = static_cast<PktAdminToolGetDataAck *>(lpPktBase);
	char* lpDataBuffer = reinterpret_cast<char *>(lpPktGetDataAck + 1);

	unsigned long dwUID = lpPktGetDataAck->m_dwUID;
	unsigned long dwCID = lpPktGetDataAck->m_dwCID;
	unsigned long dwRequestKey = lpPktGetDataAck->m_dwRequestKey;
	unsigned char cType = lpPktGetDataAck->m_cType;
	
	unsigned long dwGold = 0;

	CTempCharacter* lpTempCharacter = 
		CRegularAgentDispatch::GetTempCharacterMgr().GetCharacter(dwRequestKey, cGroup);

	if (NULL != lpTempCharacter)
	{
		if (PktAdminToolGetData::CHAR_EXTRA_DATA == cType)
		{
			lpTempCharacter->SetUID(dwUID);
			lpTempCharacter->SetCID(dwCID);

			lpTempCharacter->GetCharInfoEx() = *reinterpret_cast<CHAR_INFOEX *>(lpDataBuffer);	
			lpDataBuffer += sizeof(CHAR_INFOEX);
			
			lpTempCharacter->GetQuest() = *reinterpret_cast<QUEST *>(lpDataBuffer);	
			lpDataBuffer += sizeof(QUEST);
			
			lpTempCharacter->GetHistory() = *reinterpret_cast<HISTORY *>(lpDataBuffer);	
			lpDataBuffer += sizeof(HISTORY);
			
			lpTempCharacter->GetConfig() = *reinterpret_cast<CONFIG *>(lpDataBuffer);	
			lpDataBuffer += sizeof(CONFIG);

			// STORE_INFO	------------------------------------------------------------------------------------

			STORE_INFO& storeInfo = lpTempCharacter->GetStoreInfo() = *reinterpret_cast<STORE_INFO*>(lpDataBuffer);

			//--------------------------------------------------------------------------------------------------	

            lpTempCharacter->SetData(CTempCharacter::CHAR_DATA_SET);
            lpTempCharacter->ReleaseDataRequestCount();

			dwGold = storeInfo.Gold;
            
            CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwRequestKey);
            if (NULL != lpCharacter)
            {
                DETLOG6(g_Log, "UID:%10u/CID:%10u/정섭UID:%10u/정섭CID:%10u/Group:%d/Gold:%10u/정섭에서 돈을 얻어왔습니다.",
                    lpCharacter->GetUID(), lpCharacter->GetCID(), dwUID, dwCID, cGroup, dwGold);

                CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
                if (NULL != lpDispatch)
                {
                    GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), dwRequestKey, cGroup, 
                        PktBGServerMileageChange::MC_INFO, dwGold, lpCharacter->GetMileage(), PktBase::NO_SERVER_ERR);

                    return true;
                }
            }
		}
	}

	return true;
}

bool RegularAgentPacketParse::SendGetCharSlot(unsigned long dwCID, unsigned char cGroup)
{
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpCharacter)
	{
		ERRLOG1(g_Log, "CID:0x%08x 캐릭터가 없습니다.", dwCID);
		return false;
	}

/*
    // Test : 환전소 사용 제한    
    if (10000002 != lpCharacter->GetUID() && 
        10000012 != lpCharacter->GetUID() &&
        10000006 != lpCharacter->GetUID() &&
        10000015 != lpCharacter->GetUID() &&
        10000310 != lpCharacter->GetUID() &&
        10000027 != lpCharacter->GetUID() &&
        10001591 != lpCharacter->GetUID() &&
        10000020 != lpCharacter->GetUID() &&
        10000601 != lpCharacter->GetUID() &&
        10000013 != lpCharacter->GetUID())
    {
        return true;
    }
*/

	GET_MULTI_DISPATCH(lpDispatch, cGroup, 
		CRegularAgentDispatch, CRegularAgentDispatch::GetDispatchTable());

	if (NULL == lpDispatch) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 그룹:%d 연결세션 구하기 실패", dwCID, cGroup);
		return false;
	}

    unsigned long dwTempUID = 0;

    // 캐릭터 슬롯 정보를 뒤져 봐서. 있으면, 그 UID로 요청을 보내고, 없으면 Slot으로 요청해 본다.
    CTempCharacter* lpTempCharacter = CRegularAgentDispatch::GetTempCharacterMgr().GetCharacter(dwCID, cGroup);

    if(0 == lpTempCharacter)
    {
        // 캐릭터 생성 실패.
        

    }
    else if(lpTempCharacter->IsSetData(CTempCharacter::SLOT_INFO_SET))
    {
        // 캐릭터 데이터를 뒤져 봐서. 있으면 그 정보로 데이터를 보내고, 없으면 데이터 요청한다.
        /*
            운영툴로 창고 건들 시 문제 생길 수 있음..
        if(lpTempCharacter->IsSetData(CTempCharacter::CHAR_DATA_SET))
        {
            CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
            if (NULL != lpDispatch)
            {
                GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), 
                    dwCID, cGroup, PktBGServerMileageChange::MC_INFO, 
                    lpTempCharacter->GetStoreInfo().Gold, lpCharacter->GetMileage(), 
                    PktBase::NO_SERVER_ERR);
            }
        }
        else*/
        {
            if(SendGetCharData(lpTempCharacter->GetUID(), lpTempCharacter->GetCID(), 
                lpTempCharacter->GetCharacterName(), dwCID, cGroup))
            {
                lpTempCharacter->AddDataRequestCount();
            }
        }
    }
    else if(DBComponent::AuthDB::GetUIDFromBattleUID(
        CDBSingleObject::GetInstance(), lpCharacter->GetUID(), &dwTempUID))
    {
        // 캐릭터 슬롯 정보가 없으므로, 정섭에 캐릭터 슬롯 데이터 요청.

        DETLOG4(g_Log, "UID:%10u/CID:%10u/정섭 UID얻어오기 성공 : 정섭 UID:%10u/Group:%d", 
            lpCharacter->GetUID(), dwCID, dwTempUID, cGroup);        

        CSendStream& SendStream = lpDispatch->GetSendStream();	
        char* lpBuffer = SendStream.GetBuffer(sizeof(PktBGServerCharSlot));	

        if (NULL != lpBuffer)
        {		
            PktBGServerCharSlot* lpCharSlot = reinterpret_cast<PktBGServerCharSlot*>(lpBuffer);

            lpCharSlot->m_dwCID  = dwCID;
            lpCharSlot->m_dwUID  = dwTempUID;
            lpCharSlot->m_cGroup = cGroup;

            return SendStream.WrapHeader(sizeof(PktBGServerCharSlot), CmdBGServerCharSlot, 0, 0);
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u/CID:%10u/정섭 UID얻어오기 실패 : %s", 
            lpCharacter->GetUID(), dwCID, CDBSingleObject::GetInstance().GetErrorString());
    }

	return false;
}

bool RegularAgentPacketParse::SendGetCharData(unsigned long dwUID, unsigned long dwSlotCID, 
                                              const char* szSlotName, unsigned long dwCID, unsigned char cGroup)
{
	GET_MULTI_DISPATCH(lpDispatch, cGroup, 
		CRegularAgentDispatch, CRegularAgentDispatch::GetDispatchTable());

	if (NULL == lpDispatch) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 그룹:%d 연결세션 구하기 실패", dwCID, cGroup);
		return false;
	}

    CSendStream& SendStream = lpDispatch->GetSendStream();	
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktAdminToolGetData));	

	if (NULL != lpBuffer)
	{		
		PktAdminToolGetData* lpGetData = reinterpret_cast<PktAdminToolGetData *>(lpBuffer);

		lpGetData->m_cType = PktAdminToolGetData::GET_CHAR_DATA;
		lpGetData->m_dwUID = dwUID;
		lpGetData->m_dwCID = dwSlotCID;
		strncpy(lpGetData->m_szName, szSlotName, CHAR_INFOST::MAX_NAME_LEN);

		// 정섭의 정보를 요청한 캐릭터의 아이디를 보관해둡니다.
		lpGetData->m_dwRequestKey = dwCID;		
       
        if(SendStream.WrapHeader(sizeof(PktAdminToolGetData), CmdAdminToolGetData, 0, 0))
        {
			return true;
		}
	}

	return false;
}

bool RegularAgentPacketParse::SendSetCharData(unsigned long dwCID, unsigned long dwMileage, unsigned char cGroup)
{
	CTempCharacter* lpTempCharacter = CRegularAgentDispatch::GetTempCharacterMgr().GetCharacter(dwCID, cGroup);
	CCharacter*     lpCharacter     = CCreatureManager::GetInstance().GetCharacter(dwCID);

    if (NULL != lpTempCharacter && NULL != lpCharacter && lpTempCharacter->IsSetData(CTempCharacter::CHAR_DATA_SET))
	{
/*
		// Test : 환전소 사용 제한
		if (10000002 != lpCharacter->GetUID() && 
			10000012 != lpCharacter->GetUID() &&
			10000006 != lpCharacter->GetUID() &&
            10000015 != lpCharacter->GetUID() &&
            10000310 != lpCharacter->GetUID() &&
            10000027 != lpCharacter->GetUID() &&
            10001591 != lpCharacter->GetUID() &&
            10000020 != lpCharacter->GetUID() &&
            10000601 != lpCharacter->GetUID() &&
            10000013 != lpCharacter->GetUID())
		{
			return true;
		}
*/

        CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();


        if(!(lpTempCharacter->GetStoreInfo().Flag & PktDepositUpdateDB::USED_DEPOSIT))
        {
            // 창고를 사용한 적이 없다!
            ERRLOG2(g_Log, "CID:0x%08x/DataRequestCount:0x%08x/창고를 사용한 적이 없어서 데이터를 업데이트할 수 없습니다.", 
                dwCID, lpTempCharacter->GetStoreInfo().Flag);
           
            if (NULL != lpDispatch)
            {
                GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), dwCID, cGroup, 
                    PktBGServerMileageChange::MC_RESULT, 0, lpCharacter->GetMileage(), PktBGServerMileageChange::NOT_USED_STORE);
            }

            return true;
        }

        if(0 != lpTempCharacter->GetDataRequestCount())
        {
            ERRLOG2(g_Log, "CID:0x%08x/DataRequestCount:%u/데이터를 업데이트 할 수 없습니다. 데이터 요청중입니다.", 
                dwCID, lpTempCharacter->GetDataRequestCount());

            if (NULL != lpDispatch)
            {
                GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), dwCID, cGroup, 
                    PktBGServerMileageChange::MC_RESULT, 0, lpCharacter->GetMileage(), PktBGServerMileageChange::DATA_REQUESTING);
            }

            return true;
        }

        unsigned char cGroup = lpTempCharacter->GetGroup();
        GET_MULTI_DISPATCH(lpAgentDispatch, cGroup, 
            CRegularAgentDispatch, CRegularAgentDispatch::GetDispatchTable());

        if (NULL == lpAgentDispatch) 
        {
            ERRLOG2(g_Log, "CID:0x%08x 그룹:%d 연결세션 구하기 실패", dwCID, cGroup);

            if (NULL != lpDispatch)
            {
                GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), dwCID, cGroup, 
                    PktBGServerMileageChange::MC_RESULT, 0, lpCharacter->GetMileage(), PktBGServerMileageChange::SERVER_ERROR);
            }

            return true;
        }

		if (lpCharacter->GetMileage() < dwMileage)
		{
			ERRLOG1(g_Log, "CID:0x%08x 공헌 메달이 충분하지 않습니다.", dwCID);

            if (NULL != lpDispatch)
            {
                GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), dwCID, cGroup, 
                    PktBGServerMileageChange::MC_RESULT, 0, lpCharacter->GetMileage(), PktBGServerMileageChange::NOT_ENOUGH_MEDAL);
            }

            return true;
		}

        unsigned long dwAddGold = dwMileage * 10;
        unsigned long dwSrcGold = lpTempCharacter->GetStoreInfo().Gold;
        if(ULONG_MAX - dwAddGold < dwSrcGold)
        {  
            // 돈 오버플로우 발생

            if (NULL != lpDispatch)
            {
                GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), dwCID, cGroup, 
                    PktBGServerMileageChange::MC_RESULT, 0, lpCharacter->GetMileage(), PktBGServerMileageChange::MONEY_OVERFLOW);
            }

            return true;
        }
        
        unsigned long dwDstGold = dwSrcGold + dwAddGold;

        GAMELOG::LogTakeGold(*lpCharacter, dwSrcGold, dwDstGold, dwAddGold, 
            TakeType::TS_DEPOSIT, TakeType::TS_DEPOSIT, GAMELOG::sTakeGoldLogV2::BATTLE_DEPOSIT, 0);

        lpCharacter->SetMileage(lpCharacter->GetMileage() - dwMileage);

        lpTempCharacter->GetStoreInfo().Gold = dwDstGold;

        CSendStream& SendStream = lpAgentDispatch->GetSendStream();
		
		PktAdminToolSetData baseData;
		memset(&baseData, 0, sizeof(PktAdminToolSetData));

		baseData.m_dwSerial = Item::CItemFactory::GetInstance().GetItemUID();
		baseData.m_dwUID = lpTempCharacter->GetUID();
		baseData.m_dwCID = lpTempCharacter->GetCID();
		baseData.m_dwRequestKey = dwCID;

		// -------------------------------------------------------------------------------------------------
		// OPEN_UPDATE_DATA

		char* lpOpenBuffer = SendStream.GetBuffer(sizeof(PktAdminToolSetData));	
		if (NULL != lpOpenBuffer)
		{		
			PktAdminToolSetData* lpOpenSetData = reinterpret_cast<PktAdminToolSetData *>(lpOpenBuffer);

			*lpOpenSetData = baseData;
			lpOpenSetData->m_cType = PktAdminToolSetData::OPEN_UPDATE_DATA;

            SendStream.WrapHeader(sizeof(PktAdminToolSetData), 
                CmdAdminToolSetData, 0, PktBase::NO_SERVER_ERR);
		}

		// -------------------------------------------------------------------------------------------------
		// CHAR_EXTRA_DATA

		unsigned short usBufferSize = sizeof(PktAdminToolSetData) + 
			sizeof(CHAR_INFOEX) + sizeof(QUEST) + sizeof(HISTORY) + sizeof(CONFIG) + sizeof(STORE_INFO);

		char* lpExtraBuffer = SendStream.GetBuffer(usBufferSize);	
		if (NULL != lpExtraBuffer)
		{
			PktAdminToolSetData* lpExtraSetData = reinterpret_cast<PktAdminToolSetData *>(lpExtraBuffer);
			char* In_DataBuffer	= reinterpret_cast<char *>(lpExtraSetData + 1);
			
			CopyMemory(In_DataBuffer, &lpTempCharacter->GetCharInfoEx(), sizeof(CHAR_INFOEX)); 
			In_DataBuffer += sizeof(CHAR_INFOEX);

			CopyMemory(In_DataBuffer, &lpTempCharacter->GetQuest(), sizeof(QUEST)); 
			In_DataBuffer += sizeof(QUEST);

			CopyMemory(In_DataBuffer, &lpTempCharacter->GetHistory(), sizeof(HISTORY)); 
			In_DataBuffer += sizeof(HISTORY);

			CopyMemory(In_DataBuffer, &lpTempCharacter->GetConfig(), sizeof(CONFIG)); 
			In_DataBuffer += sizeof(CONFIG);

			*reinterpret_cast<STORE_INFO *>(In_DataBuffer) = lpTempCharacter->GetStoreInfo();

            *lpExtraSetData = baseData;
            lpExtraSetData->m_cType = PktAdminToolSetData::CHAR_EXTRA_DATA;

            SendStream.WrapHeader(usBufferSize, CmdAdminToolSetData, 0, PktBase::NO_SERVER_ERR);
        }

        // -------------------------------------------------------------------------------------------------
        // FINISH_UPDATE_DATA

        char* lpFinishBuffer = SendStream.GetBuffer(sizeof(PktAdminToolSetData));	
        if (NULL != lpFinishBuffer)
        {
            PktAdminToolSetData* lpFinishSetData = reinterpret_cast<PktAdminToolSetData *>(lpFinishBuffer);

            *lpFinishSetData = baseData;
            lpFinishSetData->m_cType = PktAdminToolSetData::FINISH_UPDATE_DATA;

            SendStream.WrapHeader(sizeof(PktAdminToolSetData), CmdAdminToolSetData, 0, PktBase::NO_SERVER_ERR);
        }

        //--------------------------------------------------------------------------------------------------	

        if (NULL != lpDispatch)
        {
            DETLOG10(g_Log, "UID:%10u/CID:%10u/정섭UID:%10u/정섭CID:%10u/Group:%d/"
                "입금전:%10u/입금후:%10u/입금액:%10u/사용마일리지:%10u/현재마일리지:%10u/"
                "정섭에 돈을 업데이트했습니다.",
                lpCharacter->GetUID(), lpCharacter->GetCID(), baseData.m_dwUID, baseData.m_dwCID, 
                cGroup, dwSrcGold, dwDstGold, dwAddGold, dwMileage, lpCharacter->GetMileage());

            GameClientSendPacket::SendCharBGServerMileageChange(lpDispatch->GetSendStream(), 
                dwCID, cGroup, PktBGServerMileageChange::MC_RESULT, lpTempCharacter->GetStoreInfo().Gold, 
                lpCharacter->GetMileage(), PktBase::NO_SERVER_ERR);
        }
    }

    return true;
}

