#ifndef _CHAT_GAME_SERVER_DISPATCH_H_
#define _CHAT_GAME_SERVER_DISPATCH_H_

#include <vector>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>
#include <Network/Stream/SendStream.h>

// ���� ����
struct PktSL;               // ���� �α���
struct PktChatData;         // ĳ���� ���� �����
struct PktFriendDB;         // ģ�� ���� �����
struct PktChatRequest;      // ä�� ��Ŷ
struct PktChatBan;			// ä�� ����
struct PktChatBanInfo;		// ä�� ���� ���� ������.

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

    bool ProcessServerLogin(PktSL* lpPktSL);						// ���� �α���
    bool ProcessCharInfoChange(PktChatData* lpChatInfoChange);		// ĳ���� ���� �����
    bool ProcessUpdateFriendDB(PktFriendDB* lpFriendDB);            // ģ�� ���� �ֱ�
    bool ProcessChat(PktChatRequest* lpPktChatReq);					// ä��
	bool ProcessChatBan(PktChatBan* lpPktChatBan);					// ä�� ���� 
	bool ProcessChatBanInfo(PktChatBanInfo* lpPktChatBanInfo);		// ä�� ���� ����.

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