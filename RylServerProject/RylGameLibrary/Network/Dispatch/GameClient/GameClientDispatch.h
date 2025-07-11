#ifndef _GAME_CLIENT_DISPATCH_H_
#define _GAME_CLIENT_DISPATCH_H_

#include <mmsystem.h>

#include <vector>
#include <deque>

#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/CheckPing.h>
#include <Network/Address/INET_Addr.h>
#include <Network/Packet/PacketDispatchTable.h>

// edith 2009.08.11 게임가드 2.5 업그레이드
//#include <GameGuardLib/CSAuth.h>
#include <GameGuardLib/ggsrv.h>

#include "CheckSpeedHack.h"

// 전방 참조
class CCharacter;
class CGameClientDispatch;
class CGameClientDispatchTable;

class CGameClientDispatch : public CRylServerDispatch
{
public:

    enum 
	{
		MAX_PACKET_DISPATCH_PER_PULSE = 10,		
	};

    typedef bool (*FnProcess)(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

    CGameClientDispatch(CSession& Session, CGameClientDispatchTable& GameClientDispatchTable);
    virtual ~CGameClientDispatch();

    virtual bool Dispatch(void);    
    virtual bool DispatchPacket(PktBase* lpPktBase);

    virtual void Connected(void);
	virtual void Disconnected(void);

    void Disconnect(void);

	// -----------------------------------------------------------------------------------------------------------
	// 게임 가드 인증 관련 인터페이스

	// edith 2009.08.11 게임가드 2.5 업그레이드
//	unsigned long GetAuthCode(void)					{ return m_CSAuth.GetAuthDword();							}
//	bool CheckAuthCode(unsigned long dwReturnCode)	{ return (TRUE == m_CSAuth.CheckAuthDword(dwReturnCode));	}
//	bool IsAuth(void)								{ return (0 != m_CSAuth.IsAuth());							}
	unsigned long GetAuthCode(void)					{ return 0; }

	bool GetAuthQuery(GG_AUTH_DATA** lpAuthData_Out);
	void SetAuthAnswer(const GG_AUTH_DATA& authData)	{ m_CSAuth2.m_AuthAnswer = authData;					}
	bool CheckAuthAnswer(void);

	// edith 2009.08.11 게임가드 2.5 업그레이드
//	unsigned long DecryptPacket(void* lpData, unsigned long dwLength)
//	{
//		return m_CSAuth.DecryptPacket(lpData, dwLength);
//	}

	void PrintGameGuardError(void);
	void PrintGameGuardError(unsigned long dwGGErrCode);

	// -----------------------------------------------------------------------------------------------------------

    CCharacter* GetCharacter(void)		{ return m_lpCharacter; }
    void        SetCharacter(CCharacter* lpCharacter);

    unsigned long GetUID(void)			{ return m_dwUID;		}
    void SetUID(unsigned long dwUID)	{ m_dwUID = dwUID;		}

    void SetMoveAddress(unsigned long dwMoveServerID, const INET_Addr& moveAddress) 
    {
        m_dwMoveServerID = dwMoveServerID;
        m_MoveAddress = moveAddress;
    }

    INET_Addr& GetMoveAddress() { return m_MoveAddress; }
    unsigned long GetMoveServerID() { return m_dwMoveServerID; }

    unsigned long PopRequestKey(void);
    void PushRequestKey(unsigned long dwRequestKey);

protected:

    // 너무 많은 패킷이 들어왔을 경우, 패킷 내용에 대해서 로그를 남기거나 패킷을 삭제한다.
    virtual void ProcessTooManyPacket(CBufferQueue& bufferQueue);

private:
    
    CCharacter*                 m_lpCharacter;              // 연관된 캐릭터
    unsigned long               m_dwUID;                    // 캐릭터 UID
    CGameClientDispatchTable&   m_GameClientDispatchTable;  // Dispatch Table
    
    std::deque<unsigned long>   m_DBRequestQueue;           // DBRequestQueue

    CCheckPing                  m_CheckPing;                // 핑 체크
    CSpeedHackCheck             m_CheckSpeedHack;           // 스핵 체크

	// edith 2009.08.11 게임가드 2.5 업그레이드
//	CCSAuth						m_CSAuth;					// GameGuard 인증
	CCSAuth2					m_CSAuth2;					// GameGuard 인증2

    INET_Addr                   m_MoveAddress;
    unsigned long               m_dwMoveServerID;    
};

// 패킷 처리 테이블
class CGameClientDispatchTable : 
    public CPacketDispatchTable<CGameClientDispatch::FnProcess>
{
public:

    static CGameClientDispatchTable& GetInstance();

protected:

    CGameClientDispatchTable();
};

#endif