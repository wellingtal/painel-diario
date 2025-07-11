#ifndef _CHAT_GAME_SERVER_DISPATCH_H_
#define _CHAT_GAME_SERVER_DISPATCH_H_

#include <vector>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>
#include <Network/Stream/SendStream.h>

// 전방 참조
struct PktSL;               // 서버 로그인
struct PktChatData;         // 캐릭터 정보 변경됨
struct PktFriendDB;         // 친구 정보 변경시
struct PktChatRequest;      // 채팅 패킷
struct PktChatBan;			// 채팅 금지
struct PktChatBanInfo;		// 채팅 금지 정보 얻어오기.

// forward decl.
class CServerChatPacket;


class CChatGameServerDispatch : public CRylServerDispatch
{
protected:

    enum 
    { 
        MAX_PACKET_DISPATCH_PER_PULSE   = 30,
        MAX_STREAM_BUFFER_SIZE          = 16000
    };

public:

    static CMultiDispatch& GetDispatchTable();

    CChatGameServerDispatch(CSession& Session);

	virtual void Connected();
    virtual void Disconnected();

    unsigned long GetServerID() const { return m_dwServerID; }

private:

    virtual bool DispatchPacket(PktBase* lpPktBase);

    bool ProcessServerLogin(PktSL* lpPktSL);						// 서버 로그인
    bool ProcessCharInfoChange(PktChatData* lpChatInfoChange);		// 캐릭터 정보 변경됨
    bool ProcessUpdateFriendDB(PktFriendDB* lpFriendDB);            // 친구 정보 주기
    bool ProcessChat(PktChatRequest* lpPktChatReq);					// 채팅
	bool ProcessChatBan(PktChatBan* lpPktChatBan);					// 채팅 금지 
	bool ProcessChatBanInfo(PktChatBanInfo* lpPktChatBanInfo);		// 채팅 금지 정보.

    unsigned long   m_dwServerID;    
};

class CSendChatAllGameServer
{
public:

    CSendChatAllGameServer(CServerChatPacket& serverChatPacket);
    bool operator () (unsigned long dwServerID, CPacketDispatch& dispatch);

private:

    CServerChatPacket& m_ServerChatPacket;
};

#endif