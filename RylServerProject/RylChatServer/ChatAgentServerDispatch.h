#ifndef _CHAT_SERVER_DISPATCH_H_
#define _CHAT_SERVER_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>

// 전방 참조

struct PktSLAck;            // 서버 로그인 Ack
struct PktHanBTN;

/*  sphawk : oldchat
struct PktChatLogin;        // 캐릭터 로그인
struct PktChatLogout;       // 캐릭터 로그아웃
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

    bool ProcessServerLoginAck(PktSLAck* lpPktSLAck);                   // 중계서버로 로그인 Ack
    bool ProcessHanBTNWarning(PktHanBTN* lpPktHanBTN);                  // 한게임 빌링 경고.

/*  sphawk : oldchat
    bool ProcessCharLogin(PktChatLogin* lpChatCharLogin);               // 캐릭터 로그인
    bool ProcessCharLogout(PktChatLogout* lpChatCharLogout);            // 캐릭터 로그아웃
    bool ProcessConfigInfo(PktConfigInfo* lpConfigInfo);                // 설정 정보 받기
    bool ProcessUpdateFriendDB(PktFriendDB* lpFriendDB);                // 친구 정보 받기
*/
};


#endif