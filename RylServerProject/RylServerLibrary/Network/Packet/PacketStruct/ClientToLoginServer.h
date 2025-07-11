#ifndef _CLIENT_TO_LOGIN_SERVER_PACKET_H_
#define _CLIENT_TO_LOGIN_SERVER_PACKET_H_

#include "ServerInfo.h"
#include <Network/Packet/PacketBase.h>

#pragma pack(1)

// -----------------------------------------------------------------------------
// �α��� Ŭ���̾�Ʈ ��Ŷ.


// -----------------------------------------------------------------------------
// ���� ����Ʈ ��û ��Ŷ : Launcher to LoginServer
typedef struct PktSvL* LPPktSvL;
struct PktSvL : public PktBase 
{
	unsigned long		m_dwClientVer;
};

// ���� ����Ʈ ���� ��Ŷ : LoginServer to Launcher
typedef struct PktSvLAck* LPPktSvLAck;
struct PktSvLAck : public PktBase 
{
	enum { PATCH_ADDRESS_LENGTH = 100 };

	unsigned long		m_dwClientVer;
	char				m_PatchAddress[PATCH_ADDRESS_LENGTH];

	SERVER_LIST			m_ServerList;				// ���� ����Ʈ
};
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// ������ ���� ��û ��Ŷ : Launcher to LoginServer
typedef struct PktSG* LPPktSG;
struct PktSG : public PktBase 
{
    char				m_cGroup;
};

// ������ ���� ���� ��Ŷ : LoginServer to Launcher
typedef struct PktSGAck* LPPktSGAck;
struct PktSGAck : public PktBase
{
	unsigned long		m_dwServerID;
	IN_ADDR				m_AuthAddress;
};
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// ����� ���� ��û ��Ŷ(OLD) : Launcher to LoginServer
typedef struct PktAUO* LPPktAUO;
struct PktAUO : public PktBase 
{
	enum 
	{ 
		// edith 2008.03.17 ID,PASS ��������
		NAME_LEN = 24,
		PASS_LEN = 36
	};

	char				m_UserAccount[NAME_LEN];	
	char				m_UserPassword[PASS_LEN];	

	unsigned long		m_ClientVerInfo;
	unsigned short		m_cFlag;
};

// ����� ���� ���� ��Ŷ : LoginServer to Launcher
typedef struct PktAUOAck* LPPktAUOAck;
struct PktAUOAck : public PktBase 
{
	unsigned long		m_dwUserID;					// ���� ���̵�
	SERVER_LIST			m_ServerList;				// ���� ����Ʈ
};

// -----------------------------------------------------------------------------


#pragma pack()

#endif