
/*
#ifndef _CCHAT_DISPATCH_H_
#define _CCHAT_DISPATCH_H_

#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/CheckPing.h>

#include "ChatPosition.h"

// 패킷 전방 참조
struct PktChatClientLogin;
struct PktChat;
struct PktChat;
struct PktWh;
struct PktMU;


class CChatClientDispatch : public CRylServerDispatch
{
protected:

	enum 
    {
        MAX_PACKET_DISPATCH_PER_PULSE   = 5,
        MAX_STREAM_BUFFER_SIZE          = 16000
    };

public:

    typedef std::map<unsigned long, CChatClientDispatch*>   ChatClientDispatchMap;

	CChatClientDispatch(CSession& Session);
	
	virtual void Disconnected();

    static ChatClientDispatchMap& GetDispatchMap();

    unsigned long   GetUID() const { return m_dwUID; }
    unsigned long   GetCID() const { return m_dwCID; }
    unsigned long   GetServerID() const { return m_dwServerID; }
    
    CChatPosition&  GetChatPos() { return m_ChatPos; }

protected:

    enum Flags
    {
        CHAR_CONNECTED      = (1 << 0),
        CHAR_DISCONNECTED   = (1 << 1)
    };

    bool Dispatch();

    void SetFlag(unsigned short usFlag) { m_usFlag |= usFlag; }
    void ResetFlag(unsigned short usFlag) { m_usFlag &= ~usFlag; }
    void ClearFlag() { m_usFlag = 0; }

    bool IsSetFlag(unsigned short usFlag) { return usFlag == (m_usFlag & usFlag); }
    
private:

	virtual bool DispatchPacket(PktBase* lpPktBase);

	// Packet Dispatch Functions
    bool ParseCharChatLogin(PktChatClientLogin* lpPktChatClientLogin);
    bool ParseCharMoveUpdate(PktMU* lpPktMU);
	bool ParseCharChat(PktChat* lpPktChat);
	bool ParseCharWhisper(PktWh* lpPktWh);

    unsigned long   m_dwUID;        // UID
    unsigned long   m_dwCID;        // CID
    unsigned long   m_dwServerID;   // ServerID

    unsigned short  m_usFlag;       // Flag

    CChatPosition   m_ChatPos;      // 채팅하는 위치
    CCheckPing      m_CheckPing;    // 핑 체크
};


#endif

*/