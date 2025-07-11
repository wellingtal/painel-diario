#ifndef _CHAT_SERVER_DISPATCH_H_
#define _CHAT_SERVER_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>

// ���� ����

struct PktSLAck;            // ���� �α��� Ack
struct PktHanBTN;

/*  sphawk : oldchat
struct PktChatLogin;        // ĳ���� �α���
struct PktChatLogout;       // ĳ���� �α׾ƿ�
struct PktFriendDB;
struct PktConfigInfo;
*/

class CChatAgentServerDispatch : public CRylServerDispatch
{
protected:

    enum 
    { 
        MAX_PACKET_DISPATCH_PER_PULSE   = 30,
        MAX_STREAM_BUFFER_SIZE          = 16000
    };

public:

	static CSingleDispatch& GetDispatchTable();
	
    CChatAgentServerDispatch(CSession& Session);

	virtual void Connected();
    virtual void Disconnected();

    bool SendServerLoginToAgentServer(unsigned long dwServerID);

private:

    virtual bool DispatchPacket(PktBase* lpPktBase);

    bool ProcessServerLoginAck(PktSLAck* lpPktSLAck);                   // �߰輭���� �α��� Ack
    bool ProcessHanBTNWarning(PktHanBTN* lpPktHanBTN);                  // �Ѱ��� ���� ���.

/*  sphawk : oldchat
    bool ProcessCharLogin(PktChatLogin* lpChatCharLogin);               // ĳ���� �α���
    bool ProcessCharLogout(PktChatLogout* lpChatCharLogout);            // ĳ���� �α׾ƿ�
    bool ProcessConfigInfo(PktConfigInfo* lpConfigInfo);                // ���� ���� �ޱ�
    bool ProcessUpdateFriendDB(PktFriendDB* lpFriendDB);                // ģ�� ���� �ޱ�
*/
};


#endif