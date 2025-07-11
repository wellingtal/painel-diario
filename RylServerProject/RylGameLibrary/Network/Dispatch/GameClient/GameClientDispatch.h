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

// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
//#include <GameGuardLib/CSAuth.h>
#include <GameGuardLib/ggsrv.h>

#include "CheckSpeedHack.h"

// ���� ����
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
	// ���� ���� ���� ���� �������̽�

	// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
//	unsigned long GetAuthCode(void)					{ return m_CSAuth.GetAuthDword();							}
//	bool CheckAuthCode(unsigned long dwReturnCode)	{ return (TRUE == m_CSAuth.CheckAuthDword(dwReturnCode));	}
//	bool IsAuth(void)								{ return (0 != m_CSAuth.IsAuth());							}
	unsigned long GetAuthCode(void)					{ return 0; }

	bool GetAuthQuery(GG_AUTH_DATA** lpAuthData_Out);
	void SetAuthAnswer(const GG_AUTH_DATA& authData)	{ m_CSAuth2.m_AuthAnswer = authData;					}
	bool CheckAuthAnswer(void);

	// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
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

    // �ʹ� ���� ��Ŷ�� ������ ���, ��Ŷ ���뿡 ���ؼ� �α׸� ����ų� ��Ŷ�� �����Ѵ�.
    virtual void ProcessTooManyPacket(CBufferQueue& bufferQueue);

private:
    
    CCharacter*                 m_lpCharacter;              // ������ ĳ����
    unsigned long               m_dwUID;                    // ĳ���� UID
    CGameClientDispatchTable&   m_GameClientDispatchTable;  // Dispatch Table
    
    std::deque<unsigned long>   m_DBRequestQueue;           // DBRequestQueue

    CCheckPing                  m_CheckPing;                // �� üũ
    CSpeedHackCheck             m_CheckSpeedHack;           // ���� üũ

	// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
//	CCSAuth						m_CSAuth;					// GameGuard ����
	CCSAuth2					m_CSAuth2;					// GameGuard ����2

    INET_Addr                   m_MoveAddress;
    unsigned long               m_dwMoveServerID;    
};

// ��Ŷ ó�� ���̺�
class CGameClientDispatchTable : 
    public CPacketDispatchTable<CGameClientDispatch::FnProcess>
{
public:

    static CGameClientDispatchTable& GetInstance();

protected:

    CGameClientDispatchTable();
};

#endif