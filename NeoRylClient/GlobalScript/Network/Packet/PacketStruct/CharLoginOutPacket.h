#ifndef _CHAR_LOGIN_OUT_PACKET_H_
#define _CHAR_LOGIN_OUT_PACKET_H_

// CharLoginOutPacket.h
#include <winsock2.h>
#include <windows.h>

#include <DB/DBDefine.h>

#include <GameGuardLib/ggsrv.h>
#include <Network/Packet/PacketBase.h>

#include "CharLoginOutPacketStruct.h"

// ���� ����
class CSession;

#pragma pack(1)

typedef struct PktDBUpdate* LPPktDBUpdate;
struct PktDBUpdate : public PktBase 
{
	unsigned __int64	m_dlItemSerial;     // ���� ���������� ������Ʈ�� ������ �ø���
	unsigned long	    m_dwSessionID;      // ���� ID
	unsigned long	    m_dwUserID;         // ���� ID
	unsigned long	    m_dwCharID;         // ĳ���� ID
	unsigned long       m_dwRequestKey;     // DB��û Ű

    char                m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN];  // ���� ������

	IN_ADDR			    m_Address;	        // �ּ�

    unsigned char       m_cAdminLevel;      // ��� ����

	unsigned char		m_cAdminFlag;
	
	// WORK_LIST 2.4 ���� ������ ���Ӽ����� ĳ���Ͱ� �������� ����
	unsigned char		m_cAccountNation;	// ���� ���� (�߰��ϸ鼭 �е����� ��������)

    unsigned char       m_cNameChangeCount; // �̸� �ٲ� �� �ִ� Ƚ��

	unsigned char		m_cGuildWarFlag;	// ��� ���� ���� �÷���
	unsigned char		m_cRealmWarFlag;	// ���� ���� ���� �÷���
	unsigned char		m_cTacticsFlag;		// �麴 �÷���.

	unsigned char		m_cRealmPoint;		// ���� ���� ������� ����Ʈ.

	unsigned short	    m_TypeCode;         // ������Ʈ Ÿ��

	unsigned short	    m_usUpdate[DBUpdateData::MAX_UPDATE_DB];  // ������Ʈ ������ ũ��

	unsigned int		m_PlayTime;			// ������ ���� ����̳� �ߴ��� (�߱����� ���� ��Ŷ)
	long				m_PremiumTime;		// �����̾� ���񽺸� ���� ����
	int					m_PremiumType;		// �����̾� ���񽺸� ���� ����
};

// ĳ���� �α���	(Char Login)
typedef struct PktCLi* LPPktCLi;
struct PktCLi : public PktBase 
{
	unsigned long	m_dwUserID;
	unsigned long	m_dwCharID;
	unsigned long	m_dwSessionID;	
};

// ĳ���� �α��� Ack (1 = ���� ����, 2 = ������ ��� ����)
typedef struct PktCLiAck* LPPktCLiAck;
struct PktCLiAck : public PktBase 
{
    enum { MAX_ADDRESS_LEN = 16 };

    sockaddr_in     m_GameServerUDPAddr;    // �� �̻� �����¡ ���� (UDP Removed 2004/09/24)
    sockaddr_in     m_ChatServerTCPAddr;    // �� �̻� �����¡ ���� (ChatServer Connection Removed 2004/09/24)

    unsigned long   m_dwUID;
    unsigned long   m_dwServerID;
	unsigned long	m_dwServerTime;

	bool			m_bPeaceMode;			// ����/���� ���
	bool			m_bCheckRelation;		// ��û/ ��û ���� ��� ���� üũ

    unsigned char   m_cNameChangeCount;     // �̸� ���� ������ Ƚ��

	unsigned char	m_cAdminFlag;			// GM �� ����.

	unsigned char	m_cGuildWarFlag;		// ��� ���� ���� �÷���
	unsigned char	m_cRealmWarFlag;		// ���� ���� ���� �÷���
	unsigned char	m_cRealmPoint;			// ���� ���� �������� ����Ʈ.
	unsigned char	m_cTacticsFlag;			// �뺴 �÷���.

	unsigned short	m_usUpdate[DBUpdateData::MAX_UPDATE_DB];

	unsigned int	m_PlayTime;			// ������ ���� ����̳� �ߴ��� (�߱����� ���� ��Ŷ)
	long			m_PremiumTime;		// �����̾� ���񽺸� ���� ����
	int				m_PremiumType;		// �����̾� ���񽺸� ���� ����
};

typedef struct PktCSAuth* LPPktCSAuth;
struct PktCSAuth : public PktBase
{
	unsigned long	m_dwCharID;				// ĳ���� ���̵�		
	unsigned long 	m_AuthCodeType;			// �����ڵ� Ÿ��. Ÿ�Կ� ���� �ʴ� ���� ������ ���� ����. (1Ȥ�� 2)

	unsigned long	m_dwAuthCode;			// ���� �ڵ� (���� ����)
	GG_AUTH_DATA	m_AuthCode2;			// ���� �ڵ�2 (���� ����)
};

// ĳ���� �α׾ƿ�	(Char Logout)
typedef struct PktCLo* LPPktCLo;
struct PktCLo : public PktBase 
{
	unsigned long	m_dwCharID;				// ĳ���� ���̵�			
};

// ĳ���� �α׾ƿ�	(Char Logout)
typedef struct PktCLoAck* LPPktCLoAck;
struct PktCLoAck : public PktBase 
{
	unsigned long	m_dwCharID;				// ĳ���� ���̵�			
};

#pragma pack()

#endif