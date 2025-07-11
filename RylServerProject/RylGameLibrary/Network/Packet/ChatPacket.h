#ifndef _GAMA_RYL_CHAT_PACKET_H_
#define _GAMA_RYL_CHAT_PACKET_H_

#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>
#include <Community/FriendList.h>
#include <Community/BanList.h>

#include <map>
#include <boost/pool/pool_alloc.hpp>

class CSendStream;

class CChatPacket
{
public:

    enum
    {
        MAX_CHAT_DATA_LEN = 1024
    };

    CChatPacket(const char* szMessage, unsigned long dwSenderCID,
        PktChat::PktChatCmd ctCmd, unsigned short usLang, unsigned short usState = 0, unsigned short usError = 0);
    
    bool IsValid() const { return (0 != m_usPacketSize || 0 != m_usCompressedSize); }
    
    unsigned short  GetPacketSize() const { return m_usPacketSize; }
    const PktBase*  GetPacketData() const { return (0 != m_usPacketSize) ? reinterpret_cast<const PktBase*>(m_szPacketData) : 0; }

    unsigned short  GetCompressedSize() const   { return m_usCompressedSize; }
    const char*     GetCompressedPacket() const { return (0 != m_usCompressedSize) ? m_szCompressedPacket : 0; }
    
private:

    unsigned short  m_usCompressedSize;
    unsigned short  m_usPacketSize;

    char            m_szPacketData[CChatPacket::MAX_CHAT_DATA_LEN];
    char            m_szCompressedPacket[CChatPacket::MAX_CHAT_DATA_LEN];
};

class CServerChatPacket
{
public:

    enum
    {
        MAX_CHAT_DATA_LEN = 8192
    };

    // Key : ServerID / Value : TargetCID
    typedef std::multimap<unsigned long, unsigned long, std::less<unsigned long>,
        boost::fast_pool_allocator< std::pair<unsigned long, unsigned long> > > TargetList;

    CServerChatPacket(const char* szMessage, PktChat::PktChatCmd ctCmd, unsigned short usLang, 
        BattleInclination::CharData& charData, const char* szSenderName,
        unsigned short usState = 0, unsigned short usError = 0);

    CServerChatPacket(const char* szMessage, PktChat::PktChatCmd ctCmd, unsigned short usLang, 
        BattleInclination::CharData& charData, const char* szSenderName,
        unsigned long* lpdwTargets, unsigned char cTargetsNum, 
        unsigned short usState = 0, unsigned short usError = 0);

    CServerChatPacket(const char* szMessage, PktChat::PktChatCmd ctCmd, unsigned short usLang, 
        BattleInclination::CharData& charData, const char* szSenderName,
        TargetList::iterator& target_start, TargetList::iterator& target_pastend,
        unsigned short usState = 0, unsigned short usError = 0);

    static bool HandleServerChatPacket(PktBase* lpPktBase, char*& szMessage, 
        PktChat::PktChatCmd& ctCmd, unsigned short& usLang, BattleInclination::CharData& charData, 
        char*& szSenderName, unsigned long*& lpdwTargetCIDs, unsigned char& cCIDNum);

    bool IsValid() const { return (0 != m_dwPacketSize); }
    unsigned long   GetCompressedSize() const { return m_dwPacketSize; }
    const char*     GetCompressedPacket() const { return (0 != m_dwPacketSize) ? m_szBuffer : NULL; }

private:

    static void SetHeaderData(char*& szDataWritePos, PktChat::PktChatCmd ctCmd, unsigned short usLang,
        BattleInclination::CharData& charData, const char* szSenderName);

    void WrapMessage(const char* szMessage, 
        char* szDataWritePos, char* szSrcBuffer, const int nSrcBufferLen, 
        unsigned short usState, unsigned short usError); 

    unsigned long   m_dwPacketSize;    
    char            m_szBuffer[CServerChatPacket::MAX_CHAT_DATA_LEN];
};

class CChatRequestPacket
{
public:

    CChatRequestPacket(const char* szMessage, PktChat::PktChatCmd ctCmd, unsigned short usLang, 
        unsigned long dwUID, unsigned long dwCID, 
        unsigned short usXPos, unsigned short usYPos, unsigned short usZPos,
        const char* szCharNames = 0, unsigned char cNamesNum = 0,
        unsigned short usState = 0, unsigned short usError = 0);

    bool IsValid() const { return (0 != m_dwPacketSize); }
    unsigned long   GetCompressedSize() const { return m_dwPacketSize; }
    const char*     GetCompressedPacket() const { return (0 != m_dwPacketSize) ? m_szBuffer : NULL; }

private:

    unsigned long   m_dwPacketSize;    
    char            m_szBuffer[CServerChatPacket::MAX_CHAT_DATA_LEN];
};

class CCommunityListSend
{
public:

    CCommunityListSend(CFriendList& FriendList);
    CCommunityListSend(CBanList& BanList);
    ~CCommunityListSend() { }

    bool IsValid() { return m_bIsValid; }

	bool Send(unsigned long dwOwnerUID, unsigned long dwOwnerCID, unsigned long dwGID, unsigned short wClass,
				char cLevel, unsigned long dwServerID, CSendStream& SendStream);    

    enum
    {
		FRIEND_BUFFER_SIZE	= sizeof(FriendInfo) * CFriendList::MAX_FRIENDS_NUM,
		BAN_BUFFER_SIZE		= sizeof(BanInfo) * CBanList::MAX_BAN_NUM,
		LIST_BUFFER_SIZE	= (sizeof(PktFriendDB) + (FRIEND_BUFFER_SIZE > BAN_BUFFER_SIZE ? FRIEND_BUFFER_SIZE : BAN_BUFFER_SIZE)) * 2
    };

private:

    char    m_szData[LIST_BUFFER_SIZE];    
    bool    m_bIsValid;
};


#endif