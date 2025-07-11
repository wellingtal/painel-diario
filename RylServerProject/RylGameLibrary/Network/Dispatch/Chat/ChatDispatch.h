#ifndef _GAME_SERVER_CHAT_DISPATCH_H_
#define _GAME_SERVER_CHAT_DISPATCH_H_

#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>

class CCharacter;

struct PktFriendDB;
struct PktFriendAck;
struct PktChatBan;

class CChatDispatch : public CRylServerDispatch
{
public:

	static CSingleDispatch& GetDispatchTable();

    enum Const
    {
        MAX_PACKET_DISPATCH_PER_PULSE   = 100,
        MAX_STREAM_BUFFER_SIZE          = 16000
    };

    CChatDispatch(CSession& Session);
    virtual ~CChatDispatch();

	virtual void Connected();
	virtual void Disconnected();
    virtual bool DispatchPacket(PktBase* lpPktBase);

    static bool SendChatServerLogin(CSendStream& SendStream);

private:

    bool ParseFriendDB(PktFriendDB* lpPktFriendDB);
    bool ParseFriendAck(PktFriendAck* lpPktFriendAck);
    bool ParseCharChat(PktBase* lpPktBase);
	bool ParseChatBan(PktChatBan* lpPktChatBan);
};


#endif