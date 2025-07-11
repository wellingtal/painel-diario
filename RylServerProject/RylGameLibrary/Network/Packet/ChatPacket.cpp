#include "stdafx.h"
#include "ChatPacket.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Utility/Setup/ServerSetup.h>

CChatPacket::CChatPacket(const char* szMessage, unsigned long dwSenderCID, 
                         PktChat::PktChatCmd ctCmd, unsigned short usLang,
                         unsigned short usState, unsigned short usError)
                         :  m_usPacketSize(0), m_usCompressedSize(0)
{
    PktChat* lpPktChat = reinterpret_cast<PktChat*>(m_szPacketData);

    lpPktChat->m_dwSenderCID    = dwSenderCID;  
	lpPktChat->m_usLang			= usLang;
    lpPktChat->m_cCmd           = ctCmd;        // 커맨드
    lpPktChat->m_cNum           = 0;

    char* szMessageWritePos = reinterpret_cast<char*>(lpPktChat + 1);        
    
    int nMessageLen = _snprintf(szMessageWritePos, 
        CChatPacket::MAX_CHAT_DATA_LEN - sizeof(PktChat), "%s", szMessage);

    if (0 < nMessageLen)
    {
        szMessageWritePos[nMessageLen] = 0;
        szMessageWritePos += nMessageLen + 1;

        unsigned long dwCompressedSize = CChatPacket::MAX_CHAT_DATA_LEN;
        m_usPacketSize = static_cast<unsigned short>(szMessageWritePos - m_szPacketData);

        if (!PacketWrap::WrapCompress(m_szCompressedPacket, dwCompressedSize, m_szPacketData, 
            m_usPacketSize, CmdCharChat, usState, usError))
        {
            m_usPacketSize = 0;
            m_usCompressedSize = 0;
        }
        else
        {
            m_usCompressedSize = static_cast<unsigned short>(dwCompressedSize);            
            lpPktChat->InitPtHead(m_usPacketSize, CmdCharChat, usState, usError);
        }
    }
}

CChatRequestPacket::CChatRequestPacket(const char* szMessage, PktChat::PktChatCmd ctCmd, unsigned short usLang, 
                                       unsigned long dwUID, unsigned long dwCID, 
                                       unsigned short usXPos, unsigned short usYPos, unsigned short usZPos,
                                       const char* szCharNames, unsigned char cNamesNum,
                                       unsigned short usState, unsigned short usError)
                                       :    m_dwPacketSize(0)
{
    char szSrcBuffer[CServerChatPacket::MAX_CHAT_DATA_LEN];
    memset(szSrcBuffer, 0, sizeof(char) * CServerChatPacket::MAX_CHAT_DATA_LEN);

    PktChatRequest* lpPktChatReq = reinterpret_cast<PktChatRequest*>(szSrcBuffer);

    lpPktChatReq->m_dwUID   = dwUID;    // 유저 ID
    lpPktChatReq->m_dwCID   = dwCID;    // 캐릭터 ID    

	lpPktChatReq->m_usLang	= usLang;	// 로컬언어

    lpPktChatReq->m_usXPos  = usXPos;   // 캐릭터 좌표
    lpPktChatReq->m_usYPos  = usYPos;   // 캐릭터 좌표
    lpPktChatReq->m_usZPos  = usZPos;   // 캐릭터 좌표

	lpPktChatReq->m_cZone   = CServerSetup::GetInstance().GetServerZone();

    lpPktChatReq->m_cCmd    = ctCmd;    // 커맨드
    lpPktChatReq->m_cNum    = 0;

    char* szMessageWritePos = reinterpret_cast<char*>(lpPktChatReq + 1);
    size_t nNamesLen = sizeof(char) * CHAR_INFOST::MAX_NAME_LEN * cNamesNum;

    if(0 != szCharNames && 0 != cNamesNum)
    {            
        memcpy(szMessageWritePos, szCharNames, nNamesLen);
        lpPktChatReq->m_cNum = cNamesNum;        // 데이터 갯수

        szMessageWritePos += nNamesLen;
    }

    int nMessageLen = _snprintf(szMessageWritePos, 
        CServerChatPacket::MAX_CHAT_DATA_LEN - nNamesLen - sizeof(PktChatRequest), "%s", szMessage);

    if (0 < nMessageLen)
    {
        szMessageWritePos[nMessageLen] = 0;
        szMessageWritePos += nMessageLen + 1;

        m_dwPacketSize = CServerChatPacket::MAX_CHAT_DATA_LEN;

        if(!PacketWrap::WrapCompress(m_szBuffer, m_dwPacketSize, szSrcBuffer, 
            static_cast<unsigned short>(szMessageWritePos - szSrcBuffer), CmdCharChat, usState, usError))
        {
            m_dwPacketSize = 0;
        }
    }
}

CServerChatPacket::CServerChatPacket(const char* szMessage, PktChat::PktChatCmd ctCmd, unsigned short usLang, 
                                     BattleInclination::CharData& charData, const char* szSenderName,
                                     unsigned short usState, unsigned short usError)
                                     :    m_dwPacketSize(0)
{
    const unsigned char MAX_TARGETS_NUM = 200;

    char szSrcBuffer[CServerChatPacket::MAX_CHAT_DATA_LEN];
    char* szDataWritePos = szSrcBuffer + sizeof(PktBase);

    memset(szSrcBuffer, 0, sizeof(char) * CServerChatPacket::MAX_CHAT_DATA_LEN);
    SetHeaderData(szDataWritePos, ctCmd, usLang, charData, szSenderName);

    // 개수.
    *szDataWritePos = 0;    
    ++szDataWritePos;

    WrapMessage(szMessage, szDataWritePos, szSrcBuffer, 
        CServerChatPacket::MAX_CHAT_DATA_LEN, usState, usError);
}

CServerChatPacket::CServerChatPacket(const char* szMessage, PktChat::PktChatCmd ctCmd, unsigned short usLang, 
                                     BattleInclination::CharData& charData, const char* szSenderName,
                                     unsigned long* lpdwTargets, unsigned char cTargetsNum, 
                                     unsigned short usState, unsigned short usError)
                                     :    m_dwPacketSize(0)
{
    const unsigned char MAX_TARGETS_NUM = 200;

    char szSrcBuffer[CServerChatPacket::MAX_CHAT_DATA_LEN];
    char* szDataWritePos = szSrcBuffer + sizeof(PktBase);

    memset(szSrcBuffer, 0, sizeof(char) * CServerChatPacket::MAX_CHAT_DATA_LEN);
    SetHeaderData(szDataWritePos, ctCmd, usLang, charData, szSenderName);

    char* lpCIDNums = szDataWritePos;
    ++szDataWritePos;

    unsigned char nTargetCount = 0;
    unsigned long* lpdwTargetEnd = lpdwTargets + cTargetsNum;

    for(; lpdwTargets != lpdwTargetEnd && nTargetCount < MAX_TARGETS_NUM;
        ++lpdwTargets, ++nTargetCount, szDataWritePos += sizeof(unsigned long))
    {
        *reinterpret_cast<unsigned long*>(szDataWritePos) = *lpdwTargets;
    }

    *lpCIDNums = nTargetCount;

    WrapMessage(szMessage, szDataWritePos, szSrcBuffer, 
        CServerChatPacket::MAX_CHAT_DATA_LEN, usState, usError);
}


CServerChatPacket::CServerChatPacket(const char* szMessage, PktChat::PktChatCmd ctCmd, unsigned short usLang, 
                                     BattleInclination::CharData& charData, 
                                     const char* szSenderName,
                                     TargetList::iterator& target_start, 
                                     TargetList::iterator& target_pastend,
                                     unsigned short usState, unsigned short usError)
                                     :    m_dwPacketSize(0)
{
    const unsigned char MAX_TARGETS_NUM = 200;

    char szSrcBuffer[CServerChatPacket::MAX_CHAT_DATA_LEN];
    char* szDataWritePos = szSrcBuffer + sizeof(PktBase);

    memset(szSrcBuffer, 0, sizeof(char) * CServerChatPacket::MAX_CHAT_DATA_LEN);
    SetHeaderData(szDataWritePos, ctCmd, usLang, charData, szSenderName);

    char* lpCIDNums = szDataWritePos;
    ++szDataWritePos;

    unsigned char nTargetCount = 0;
    for(; target_start != target_pastend && nTargetCount < MAX_TARGETS_NUM;
        ++target_start, ++nTargetCount, szDataWritePos += sizeof(unsigned long))
    {
        *reinterpret_cast<unsigned long*>(szDataWritePos) = target_start->second;
    }

    *lpCIDNums = nTargetCount;

    WrapMessage(szMessage, szDataWritePos, szSrcBuffer, 
        CServerChatPacket::MAX_CHAT_DATA_LEN, usState, usError);
}

bool CServerChatPacket::HandleServerChatPacket(PktBase* lpPktBase, char*& szMessage, 
                                               PktChat::PktChatCmd& ctCmd, unsigned short& usLang,
                                               BattleInclination::CharData& charData, 
                                               char*& szSenderName,
                                               unsigned long*& lpdwTargetCIDs, unsigned char& cCIDNum)
{
    // 패킷 길이 체크 후, 올바른 데이터를 넘겨준다.

    unsigned short usPacketLen = lpPktBase->GetLen();

    if(usPacketLen < sizeof(PktBase) + 
        sizeof(BattleInclination::CharData) + sizeof(unsigned char) * 2)
    {
        return false;
    }

    char* szStartPos = reinterpret_cast<char*>(lpPktBase);
    char* szDataPos  = reinterpret_cast<char*>(lpPktBase + 1);

    memcpy(&charData, szDataPos, sizeof(BattleInclination::CharData));
    szDataPos += sizeof(BattleInclination::CharData);

    szSenderName = szDataPos;
    szSenderName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

    szDataPos += sizeof(char) * CHAR_INFOST::MAX_NAME_LEN;
    
	usLang = static_cast<unsigned short>(*szDataPos);
	szDataPos += sizeof(unsigned short);

    ctCmd = static_cast<PktChat::PktChatCmd>(*szDataPos);
    ++szDataPos;

    cCIDNum = *szDataPos;
    ++szDataPos;

    lpdwTargetCIDs = reinterpret_cast<unsigned long*>(szDataPos);
    szDataPos += sizeof(unsigned long) * cCIDNum;

    if(szStartPos + usPacketLen <= szDataPos)
    {
        return false;
    }

    szMessage = szDataPos;
    szStartPos[usPacketLen - 1] = 0;

    return true;
}

void CServerChatPacket::SetHeaderData(char*& szDataWritePos, 
                                      PktChat::PktChatCmd ctCmd, unsigned short usLang,
                                      BattleInclination::CharData& charData,
                                      const char* szSenderName)
{
    memcpy(szDataWritePos, &charData, sizeof(BattleInclination::CharData));
    szDataWritePos += sizeof(BattleInclination::CharData);

    if(0 != szSenderName)
    {
        strncpy(szDataWritePos, szSenderName, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
        szDataWritePos[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
    }
    else
    {
        memset(szDataWritePos, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    }

    szDataWritePos += sizeof(char) * CHAR_INFOST::MAX_NAME_LEN;

	memcpy(szDataWritePos, &usLang, sizeof(unsigned short));
	szDataWritePos += sizeof(unsigned short);

    *szDataWritePos = static_cast<unsigned char>(ctCmd);
    ++szDataWritePos;

    *szDataWritePos = 0;
}

void CServerChatPacket::WrapMessage(const char* szMessage, 
                                    char* szDataWritePos, char* szSrcBuffer, 
                                    const int nSrcBufferLen,
                                    unsigned short usState, unsigned short usError)
{
    unsigned short usPacketSize = static_cast<unsigned short>(szDataWritePos - szSrcBuffer);

    int nMessageLen = _snprintf(szDataWritePos, 
        nSrcBufferLen - usPacketSize, "%s", szMessage);

    if(0 < nMessageLen)
    {
        szDataWritePos[nMessageLen] = 0;
        szDataWritePos += nMessageLen + 1;

        usPacketSize = static_cast<unsigned short>(szDataWritePos - szSrcBuffer);
        m_dwPacketSize = nSrcBufferLen;

        if(!PacketWrap::WrapCompress(m_szBuffer, m_dwPacketSize, szSrcBuffer, 
            usPacketSize, CmdCharChat, usState, usError))
        {
            m_dwPacketSize = 0;
        }
    }    
}



CCommunityListSend::CCommunityListSend(CFriendList& FriendList)
{
    memset(m_szData, 0, sizeof(char) * LIST_BUFFER_SIZE);

    PktFriendDB* lpPktFriendDB_Client = reinterpret_cast<PktFriendDB*>(m_szData);

    lpPktFriendDB_Client->m_dwOwnerUID      = 0;
    lpPktFriendDB_Client->m_dwOwnerCID      = 0;
    lpPktFriendDB_Client->m_dwReferenceUID  = 0;
    lpPktFriendDB_Client->m_dwReferenceCID  = 0;
    lpPktFriendDB_Client->m_dwData          = LIST_BUFFER_SIZE;
    lpPktFriendDB_Client->m_cCmd            = PktFriendDB::FRIEND_LIST;

    m_bIsValid = FriendList.SerializeOut(m_szData + sizeof(PktFriendDB), 
        lpPktFriendDB_Client->m_dwData);
}

CCommunityListSend::CCommunityListSend(CBanList& BanList)
{
    memset(m_szData, 0, sizeof(char) * LIST_BUFFER_SIZE);

    PktFriendDB* lpPktFriendDB_Client = reinterpret_cast<PktFriendDB*>(m_szData);

    lpPktFriendDB_Client->m_dwOwnerUID      = 0;
    lpPktFriendDB_Client->m_dwOwnerCID      = 0;
    lpPktFriendDB_Client->m_dwReferenceUID  = 0;
    lpPktFriendDB_Client->m_dwReferenceCID  = 0;
    lpPktFriendDB_Client->m_dwData          = LIST_BUFFER_SIZE;
    lpPktFriendDB_Client->m_cCmd            = PktFriendDB::BAN_LIST;

    m_bIsValid = BanList.SerializeOut(m_szData + sizeof(PktFriendDB), 
        lpPktFriendDB_Client->m_dwData);
}


bool CCommunityListSend::Send(unsigned long dwOwnerUID, unsigned long dwOwnerCID, unsigned long dwGID, unsigned short wClass,
							  char cLevel, unsigned long dwServerID, CSendStream& SendStream)
{
    if(m_bIsValid)
    {
        PktFriendDB* lpPktFriendDB_Client = reinterpret_cast<PktFriendDB*>(m_szData);

        lpPktFriendDB_Client->m_dwOwnerUID      = dwOwnerUID;
        lpPktFriendDB_Client->m_dwOwnerCID      = dwOwnerCID;
		lpPktFriendDB_Client->m_dwGID			= dwGID;
		lpPktFriendDB_Client->m_wClass			= wClass;
		lpPktFriendDB_Client->m_cLevel			= cLevel;
		lpPktFriendDB_Client->m_dwServerID		= dwServerID;

        return SendStream.WrapCompress(m_szData, 
            static_cast<unsigned short>(lpPktFriendDB_Client->m_dwData) + sizeof(PktFriendDB), CmdFriendDB, 0, 0);
    }

    return false;
}

