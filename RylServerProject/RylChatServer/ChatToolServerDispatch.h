#ifndef _GAMA_CHATSERVER_CHAT_TOOL_SERVER_DISPATCH_H_
#define _GAMA_CHATSERVER_CHAT_TOOL_SERVER_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>
#include <Network/Packet/ChatToolPacketCmd.h>

#include <set>
#include <string>
#include <boost/pool/pool_alloc.hpp>

// forward decl.
class CChatCharInfo;
class CChatPosition;

namespace ChatData
{
    class CCharInfo;
}


class CChatAdminNames
{
public:

    static CChatAdminNames& GetInstance();

    bool RegisterName(const char* szAdminName);
    bool RemoveName(const char* szAdminName);
    bool HasName(const char* szAdminName);

private:
    
    typedef std::set<std::string, std::less<std::string>, 
        boost::fast_pool_allocator<std::string> >   NameSet;
    
    NameSet     m_NameSet;
};



class CChatToolServerDispatch : public CRylServerDispatch
{
protected:

    enum 
    { 
        MAX_PACKET_DISPATCH_PER_PULSE   = 5,
        MAX_STREAM_BUFFER_SIZE          = 16000
    };

public:

    static CMultiDispatch& GetDispatchTable();

    CChatToolServerDispatch(CSession& Session);

	virtual void Connected();
    virtual void Disconnected();
    
    //! 인증되었는지 여부를 리턴.
    bool IsAuthorized() const { return m_bAuthorized; }

    bool SendChatMessage(ChatData::CCharInfo& charInfo, 
        const char* szChatMsg, const char* szTargetName,
        unsigned short usXPos, unsigned short usZPos, 
        unsigned char cChatMsgType, unsigned short usLang, bool bTargetIsToolAdmin);

    bool SendChatMessage(unsigned long dwTargetUID, unsigned long dwTargetCID,
		const char* szSenderName, const char* szTargetName,
		const char* szChatMsg, unsigned char cChatMsgType, unsigned short usLang);

private:

    virtual bool Dispatch();
    virtual bool DispatchPacket(PktBase* lpPktBase);

    bool Authorize(PktBase* lpPktBase);
    bool ChangeOption(PktBase* lpPktBase);
    bool ChatRequest(PktBase* lpPktBase);
    bool CheckPing(PktBase* lpPktBase);
    bool ChatAdminStatus(PktBase* lpPktBase);

    //! 변수 선언.
    char m_szUserID[ChatToolPkt::MAX_USER_ID];
    char m_szPassword[ChatToolPkt::MAX_PASSWORD];
    char m_cChatOptions[ChatToolPkt::ChatOption::MAX_OPTION];

    unsigned long m_dwLastPingRecvTime;

    bool m_bAuthorized;
};


class CSendAllDispatch
{
public:

    CSendAllDispatch(ChatData::CCharInfo& chatInfo, 
        const char* szChatMsg, const char* szTargetName,
        unsigned short usXPos, unsigned short usZPos,
        unsigned char cChatMsgType, unsigned short usLang, bool bTargetIsToolAdmin)
        :   m_chatInfo(chatInfo), 
            m_usXPos(usXPos), 
            m_usZPos(usZPos),
            m_cChatMsgType(cChatMsgType),
            m_szChatMsg(szChatMsg),
            m_szTargetName(szTargetName),
			m_usLang(usLang),
            m_bTargetIsToolAdmin(bTargetIsToolAdmin)
    {

    }

    bool operator() (unsigned long dwDispatchKey, CPacketDispatch& dispatch)
    {        
        return static_cast<CChatToolServerDispatch&>(dispatch).SendChatMessage(
            m_chatInfo, m_szChatMsg, m_szTargetName, 
            m_usXPos, m_usZPos, m_cChatMsgType, m_usLang, m_bTargetIsToolAdmin);
    }

private:

    ChatData::CCharInfo&    m_chatInfo;
    const char*             m_szChatMsg;
    const char*             m_szTargetName;
    unsigned short          m_usXPos;
    unsigned short          m_usZPos;

	unsigned short          m_usLang;

    unsigned char           m_cChatMsgType;
    unsigned char           m_bTargetIsToolAdmin;        
};

class CSendAdminDispatch
{
public:

    CSendAdminDispatch(unsigned long dwTargetUID, unsigned long dwTargetCID,
		const char* szSenderName, const char* szTargetName, 
		const char* szChatMsg, unsigned char cChatMsgType, unsigned short usLang)
        :   m_dwTargetUID(dwTargetUID), m_dwTargetCID(dwTargetCID),
			m_szSenderName(szSenderName), m_szTargetName(szTargetName),
			m_szChatMsg(szChatMsg), m_cChatMsgType(cChatMsgType), m_usLang(usLang)
    {

    }

    bool operator() (unsigned long dwDispatchKey, CPacketDispatch& dispatch)
    {        
        return static_cast<CChatToolServerDispatch&>(dispatch).SendChatMessage(
			m_dwTargetUID, m_dwTargetCID, m_szSenderName, m_szTargetName, m_szChatMsg, m_cChatMsgType, m_usLang);
    }

private:

	unsigned long			m_dwTargetUID;
	unsigned long			m_dwTargetCID;
	const char*				m_szSenderName;
	const char*				m_szTargetName;
    const char*             m_szChatMsg;
    unsigned char           m_cChatMsgType;
	unsigned short          m_usLang;
};


#endif