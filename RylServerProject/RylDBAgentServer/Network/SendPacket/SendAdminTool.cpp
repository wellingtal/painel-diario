#include "stdafx.h"
#include "SendAdminTool.h"

#include <Network/Packet/PacketStruct/CharAdminPacket.h>
#include <Network/Packet/PacketStruct/GameEventPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/Dispatch/AdminToolDispatch.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>

#include <Utility/Setup/ServerSetup.h>

#include <DataStorage/StoreData.h>
#include <DataStorage/CharacterData.h>

#include <Log/ServerLog.h>

namespace DBAgent
{
namespace SendPacket
{

bool ItemQtyCheck(unsigned long dwItemTypeID, unsigned long dwItemQty, 
    unsigned long dwCurrentItemQty, time_t tStartTime, time_t tEndTime, 
    unsigned char cType, unsigned short usError)
{
    PktItemQtyControl pktItemQtyControl;
    memset(&pktItemQtyControl, 0, sizeof(PktItemQtyControl));

    pktItemQtyControl.m_dwItemTypeID        = dwItemTypeID;
    pktItemQtyControl.m_dwItemQty           = dwItemQty;
    pktItemQtyControl.m_dwCurrentItemQty    = dwCurrentItemQty;
    pktItemQtyControl.m_tStartTime          = tStartTime;
    pktItemQtyControl.m_tEndTime            = tEndTime;

    pktItemQtyControl.m_cType               = cType;
    pktItemQtyControl.m_cGroup              = CServerSetup::GetInstance().GetServerGroup();

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktItemQtyControl), 
        sizeof(PktItemQtyControl), CmdItemQtyCheck, 0, usError))
    {        
        DBAgent::CAdminToolDispatch::GetDispatchTable().Process(
            CSendPacketAllServer(reinterpret_cast<char*>(&pktItemQtyControl), 
            sizeof(PktItemQtyControl), CmdItemQtyCheck));

        return true;
    }

    return false;
}

bool AdminToolGetDataAck(CSendStream& SendStream, PktAdminToolGetData::GetDataType dataType, 
                         unsigned long dwRequestKey, unsigned long dwUID, unsigned long dwCID, 
                         const char* lpData, unsigned long dwDataSize, unsigned short usError,
						 unsigned char cOldServerGroupID)
{
    // �ִ� ��Ŷ ũ�⺸�� ũ�� ����
    if(sizeof(PktAdminToolGetDataAck) + dwDataSize <= MAX_PACKET_LEN)
    {
        char szPacket[PktMaxLen];

        PktAdminToolGetDataAck* lpPktAdminToolGetDataAck =
            reinterpret_cast<PktAdminToolGetDataAck*>(szPacket);

        lpPktAdminToolGetDataAck->m_dwRequestKey   		= dwRequestKey;
        lpPktAdminToolGetDataAck->m_dwUID          		= dwUID;
        lpPktAdminToolGetDataAck->m_dwCID          		= dwCID;
        lpPktAdminToolGetDataAck->m_cType          		= dataType;
		lpPktAdminToolGetDataAck->m_cOldServerGroupID	= cOldServerGroupID; // Part2Selectable Ÿ�Կ��� �ʿ�.

        if(0 != lpData && 0 < dwDataSize)
        {            
            memcpy(lpPktAdminToolGetDataAck + 1, lpData, dwDataSize);
        }
        else
        {
            dwDataSize = 0;
        }

        return SendStream.WrapCompress(szPacket, 
            static_cast<unsigned short>(sizeof(PktAdminToolGetDataAck) + dwDataSize),
            CmdAdminToolGetData, 0, usError);
    }

    ERRLOG4(g_Log, "UID:%10u / CID:%10u / dwSize:%u / DataType:%d "
        "����� ������ ���� ���� : ������ + ��� ũ�Ⱑ �ִ� ��Ŷ ũ�� �̻��Դϴ�",
        dwUID, dwCID, dwDataSize, dataType);

    return false;
}

bool AdminToolSetDataAck(CSendStream& SendStream, unsigned long dwRequestKey, 
                         unsigned long dwUID, unsigned long dwCID,
                         unsigned char cType, unsigned short eErrorCode)
{
    PktNewAdminToolSetDataAck* lpSetDataAck = reinterpret_cast<PktNewAdminToolSetDataAck*>(
        SendStream.GetBuffer(sizeof(PktNewAdminToolSetDataAck)));

    if(0 != lpSetDataAck)
    {
        lpSetDataAck->m_dwRequestKey    = dwRequestKey;
        lpSetDataAck->m_dwUID           = dwUID;
        lpSetDataAck->m_dwCID           = dwCID;
        lpSetDataAck->m_cType           = cType;

        return SendStream.WrapHeader(sizeof(PktNewAdminToolSetDataAck), 
            CmdAdminToolSetData, 0, eErrorCode);
    }

    return false;
}


inline void CopyAndAdvanceDst(char*& lpWritePos, const void* lpData, unsigned long dwSize)
{
    memcpy(lpWritePos, lpData, dwSize);
    lpWritePos += dwSize;        
}


// ĳ���� �����͸� ����� �����Ѵ�.
bool TotalDataToAdminTool(CSendStream& SendStream, unsigned long dwRequestKey, IN_ADDR peerAddr,
                          DataStorage::CStoreData& storeData, DataStorage::CCharacterData& characterData)
{
    unsigned long dwUID = storeData.GetUID();
    unsigned long dwCID = characterData.GetCID();
    
    const unsigned long MAX_CHAR_DATA_SIZE = DBUpdateData::MAX_DBUPDATE_SIZE + 
        sizeof(unsigned short) * DBUpdateData::MAX_UPDATE_DB;

    const unsigned long MAX_CHAR_EXTRA_DATA_SIZE = sizeof(CHAR_INFOEX) + 
        sizeof(QUEST) + sizeof(HISTORY) + sizeof(CONFIG) + sizeof(STORE_INFO);

    const unsigned long MAX_FRIEND_BAN_DATA_SIZE = 
        std::max(CFriendList::MAX_FRIENDS_NUM * sizeof(FriendInfo),
        CBanList::MAX_BAN_NUM * sizeof(BanInfo));

    const unsigned long MAX_ADMIN_TOOL_DATA_SIZE = std::max(
        std::max(MAX_CHAR_DATA_SIZE, MAX_CHAR_EXTRA_DATA_SIZE), MAX_FRIEND_BAN_DATA_SIZE);

    char* szDataBuffer =  new char[MAX_ADMIN_TOOL_DATA_SIZE];

    if(0 == szDataBuffer)
    {
        return false;
    }

    const int MAX_ADDR_LEN = 32;
    char szAdminToolAddr[MAX_ADDR_LEN];
    _snprintf(szAdminToolAddr, MAX_ADDR_LEN - 1, "%s", inet_ntoa(peerAddr));
    szAdminToolAddr[MAX_ADDR_LEN - 1] = 0;

    // ------------------------------------------------------------------------------------------
    // CHAR_BASIC_DATA ����

    unsigned long	dwCharDataWritten = MAX_ADMIN_TOOL_DATA_SIZE - 
        sizeof(unsigned short) * DBUpdateData::MAX_UPDATE_DB;

    char*           szCharData  = szDataBuffer + sizeof(unsigned short) * DBUpdateData::MAX_UPDATE_DB;
    unsigned short* usSizeArray = reinterpret_cast<unsigned short*>(szDataBuffer);

    if(characterData.SerializeOut(szCharData, usSizeArray, 
        dwCharDataWritten, DBUpdateData::MAX_UPDATE_DB))
    {
        dwCharDataWritten += sizeof(unsigned short) * DBUpdateData::MAX_UPDATE_DB;

        if(SendPacket::AdminToolGetDataAck(SendStream, PktAdminToolGetData::CHAR_BASIC_DATA, 
            dwRequestKey, dwUID, dwCID, szDataBuffer, dwCharDataWritten, 0))
        {
            INFLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ����� CHAR_BASIC_DATA ���� ����", dwUID, dwCID, szAdminToolAddr);
        }
    }

    // ------------------------------------------------------------------------------------------
    // CHAR_EXTRA_DATA ����

    char* szExtraDataPos = szDataBuffer;

    CopyAndAdvanceDst(szExtraDataPos, &characterData.GetInfoEx(), sizeof(CHAR_INFOEX));
    CopyAndAdvanceDst(szExtraDataPos, &characterData.GetQuest(), sizeof(QUEST));
    CopyAndAdvanceDst(szExtraDataPos, &characterData.GetHistory(), sizeof(HISTORY));
    CopyAndAdvanceDst(szExtraDataPos, &characterData.GetConfig(), sizeof(CONFIG));
    CopyAndAdvanceDst(szExtraDataPos, &storeData.GetStoreInfo(), sizeof(STORE_INFO));

    if(SendPacket::AdminToolGetDataAck(SendStream, 
        PktAdminToolGetData::CHAR_EXTRA_DATA, dwRequestKey, dwUID, dwCID, szDataBuffer, 
        static_cast<unsigned long>(szExtraDataPos - szDataBuffer), 0))
    {
        INFLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ����� CHAR_EXTRA_DATA ���� ����", dwUID, dwCID, szAdminToolAddr);
    }

    // ---------------------------------------------------------------------------------------------
    // CHAR_FRIEND_DATA
    // CHAR_BAN_DATA

    unsigned long dwFriendBanWritten = MAX_FRIEND_BAN_DATA_SIZE;

    if(characterData.GetFriendList().SerializeOut(szDataBuffer, dwFriendBanWritten))
    {
        if(SendPacket::AdminToolGetDataAck(SendStream, PktAdminToolGetData::CHAR_FRIEND_DATA,
            dwRequestKey, dwUID, dwCID, szDataBuffer, dwFriendBanWritten, 0))
        {
            INFLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ����� CHAR_FRIEND_DATA ���� ����", dwUID, dwCID, szAdminToolAddr);
        }
    }

    dwFriendBanWritten = MAX_FRIEND_BAN_DATA_SIZE;
    if(characterData.GetBanList().SerializeOut(szDataBuffer, dwFriendBanWritten))
    {
        if(SendPacket::AdminToolGetDataAck(SendStream, PktAdminToolGetData::CHAR_BAN_DATA,
            dwRequestKey, dwUID, dwCID, szDataBuffer, dwFriendBanWritten, 0))
        {
            INFLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ����� CHAR_BAN_DATA ���� ����", dwUID, dwCID, szAdminToolAddr);
        }
    }

    // ---------------------------------------------------------------------------------------------
    // STORE_12_DATA

    if(SendPacket::AdminToolGetDataAck(SendStream, PktAdminToolGetData::STORE_12_DATA, 
        dwRequestKey, dwUID, dwCID, reinterpret_cast<const char*>(&storeData.GetStore1()), sizeof(STORE), 0))
    {
        INFLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ����� STORE_12_DATA ���� ����", dwUID, dwCID, szAdminToolAddr);
    }

    // ---------------------------------------------------------------------------------------------
    // STORE_34_DATA

    if(SendPacket::AdminToolGetDataAck(SendStream, PktAdminToolGetData::STORE_34_DATA, 
        dwRequestKey, dwUID, dwCID, reinterpret_cast<const char*>(&storeData.GetStore2()), sizeof(STORE), 0))
    {
        INFLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ����� STORE_34_DATA ���� ����", dwUID, dwCID, szAdminToolAddr);
    }

    delete [] szDataBuffer;
    return true;
}




}
}