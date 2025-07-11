#ifndef _CLIENT_TO_AUTH_SERVER_PACKET_H_
#define _CLIENT_TO_AUTH_SERVER_PACKET_H_

#include <DB/DBDefine.h>
#include <Network/Packet/PacketBase.h>


#pragma pack(1)


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� ���� ��û (Client to AuthServer)
//	- ���� �̸�
//	- ���� �н�����
//	- Ŭ���̾�Ʈ ����
//
//	: ����� �������� �α����Ͽ� ���� ���̵�� �α��� ������ ���� ����Ʈ���� ��´�.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktAU* LPPktAU;
struct PktAU : public PktBase 
{
    enum 
	{ 
		// edith 2008.03.17 ID,PASS ��������
		ID_LEN		= 24, 
		PASS_LEN	= 36
	};

	char			m_UserAccount[ID_LEN];
	char			m_UserPassword[PASS_LEN];

    unsigned long	m_dwSessionID;
	unsigned long	m_dwClientVer;
	unsigned long	m_dwCheckSum;
	char			m_dwDiskOk;//hz added this for our client only fix.

	unsigned short  m_usFlag;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// �Ϻ��� ����� ���� ��û (Client to AuthServer)
//
//	: ����� �������� �α����Ͽ� ���� ���̵�� �α��� ������ ���� ����Ʈ���� ��´�.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktJPAU* LPPktJPAU;
struct PktJPAU : public PktBase
{
	enum { NAME_LEN = 16 };

	char            m_szUserAccount[NAME_LEN];

	unsigned long   m_dwUserID;
	unsigned long   m_dwSessionID;
	unsigned long   m_dwClientVer;
	unsigned long   m_dwCheckSum;

	unsigned short  m_usFlag;
};



////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� ���� ����	(AuthServer to Client)
//	- ���� ���̵�
//
//	: ����� �������� �α����Ͽ� ���� ���̵� ��´�.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktAUAck* LPPktAUAck;
struct PktAUAck : public PktBase
{
	unsigned long   m_dwUserID;	
    unsigned char   m_cAgentServerType;
    unsigned char   m_cFirstLogin;   
	char			m_ClientA;//hz added this for client kick if not ok
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ���� �α��� (Client to AuthServer)
//	- ���� ���̵�
//	- üũ��
//	- ���
//
//	: ���� �α��� �Ŀ� ĳ���� ����Ʈ ���� ������ ���´�.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktULi* LPPktULi;
struct PktULi : public PktBase 
{
	enum LoginType
	{
		USERLOGIN_FIRST			= 0,	// ���� �α�
		USERLOGIN_CHAR_SELECT	= 1		// ĳ���� ����ȭ������ �̵�
	};

 	unsigned long   m_dwUserID;
	unsigned long   m_dwCheckSum;

	unsigned char	m_cLoginType;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ���� �α��� Ack (AuthServer to Client)
//	- ���� ���̵�
//	- ĳ���� ����
//
//	(1 = ���� ����, 2 = ������ ��� ����)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktULiAck* LPPktULiAck;
struct PktULiAck : public PktBase 
{
	enum Const
	{
		MAX_CHAR_VIEW = 5
	};

	enum PktULiAckErr
	{ 
		FAIL_GET_DATA = 2 
	};

	unsigned long	m_dwSessionID;
	unsigned long	m_dwUserID;
	CHAR_VIEW		m_CharView[MAX_CHAR_VIEW];
	sGuildData		m_GuildData[MAX_CHAR_VIEW];

	unsigned long	m_dwTabFlag;

	// WORK_LIST 2.1 ���� ���� �߰�
	unsigned char	m_cAccountNation;	// ���� ����

    time_t          m_dwCurrentTime;    // ���� �ð�
    int             m_nPlayTime;        // BillingType�� �����̳� ������ ���� ���� ��¥. ������ ���� ���� �ð�(��)
    unsigned short	m_usAdminLevel;     // ��� ����.
    unsigned char   m_cBillingType;     // D : ����     T : ����    N : ����
    unsigned char   m_cBillingUser;     // 0 : ����     1 : �ǽù�

	unsigned char	m_cLoginType;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ����	( Char Select )
//	- ���� ���̵�
//	- ĳ���� ���̵�
//
//	: ĳ���� ���� �� �ش� ĳ������ ������ ���� �����Ϳ� �ּҸ� �Ѱ� �޴´�.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktCS* LPPktCS;
struct PktCS : public PktBase 
{
	unsigned long		m_dwUserID;	
	unsigned long		m_dwCharID;
};

// ĳ���� ���� Ack
typedef struct PktCSAck* LPPktCSAck;
struct PktCSAck : public PktBase 
{
	enum { MAX_CHANNEL_NUM = 5 };

	unsigned char		m_cZone;							// ��
	unsigned short		m_wChannelNum[MAX_CHANNEL_NUM];		// ä��
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ����	( Char Create )
//	- ���� ���̵�
//	- ���� ��ȣ
//	- ĳ���� ���� ������
//	- ���� (����, ����, ����)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktCC* LPPktCC;
struct PktCC : public PktBase
{
	unsigned long		m_dwUserID;		
	unsigned long		m_dwSlotNum;		
	CHAR_CREATE			m_CreateChar;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� Ack
//	- ĳ���� ���̵�
//	- ���� ��ȣ
//	- ĳ���� ������
//
//	(1 = ���� ����, 2 = ������ ���� ����, 3 = �����ϴ� ĳ���� �̸�, 4 = ��� ���� ���� ����, 
//		5 = �������� ���� �̸�, 6 = �߸��� ���� ������)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktCCAck* LPPktCCAck;
struct PktCCAck : public PktBase
{
	unsigned long		m_dwCharID;
	unsigned long		m_dwSlotNum;		
	CHAR_VIEW			m_CharView;

	enum 
    { 
        SERVER_ERROR            = 1, 
        FAIL_INSERT_DATA        = 2, 
        EXIST_CHAR_NAME         = 3, 
        EXIST_SLOT              = 4,
        WRONG_CHAR_NAME         = 5, 
        WRONG_CHAR_DATA         = 6,
        SLANG_FILTERED          = 7,
        NULL_SESSION            = 8,
        VIEW_RELOAD_FAILED      = 9,
        SUPRESSED_CHAR_CREATE   = 10
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ����	( Char Delete )
//	- ���� ���̵�
//	- ĳ���� ���̵�
//	- ���� ��ȣ
//	- ��Ƽ ���̵�
//	- ��� ���̵�
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktCD* LPPktCD;
struct PktCD : public PktBase 
{
	unsigned long		m_dwUserID;	
	unsigned long		m_dwCharID;		
	unsigned long		m_dwSlotNum;	

	unsigned char		m_szPassword[STORE_INFO::MAX_PASS_LEN];
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� Ack
//	- ���� ���̵�
//	- ���� ��ȣ
//
//	(1 = ���� ����, 2 = ������ ���� ����)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktCDAck* LPPktCDAck;
struct PktCDAck : public PktBase 
{
	unsigned long		m_dwUserID;			
	unsigned long		m_dwSlotNum;		
	enum 
    { 
        SERVER_ERROR        = 1, 
        FAIL_DELETE_DATA    = 2,
    
        BLOCKED_DELETE_DATA = 6,

		FAIL_PASSWORD		= 7
    };
};

// WORK_LIST 2.1 ���� ���� �߰�
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ���� ���� ���� ( Select Account Nation )
//	- ���� ���̵�
//	- ���� ����
//
//	(1 = ���� ����, 2 = ���� ����)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktSelectAccountNation* LPPktSelectAccountNation;
struct PktSelectAccountNation : public PktBase
{
	unsigned long		m_dwUserID;
	unsigned char		m_cType;
	unsigned char		m_cAccountNation;

	enum eType
	{
		TYPE_SET		= 1,	// �� ����
		TYPE_CHANGE		= 2		// ���� ���� ����
	};

	enum { SERVER_ERROR = 1, FAIL_SET_NATION = 2 };
};

// WORK_LIST 2.3 ���� ���� ���� ��� ����
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ���� ���� �������� ���� ĳ���͵� ��� ����
//	- ���� ���̵�
//	- CID, GID, Fame
//
//	(1 = ���� ���� )
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktNationChangeResult* LPPktNationChangeResult;
struct PktNationChangeResult : public PktBase
{
	enum 
	{
		MAX_CHAR_VIEW	= 5
	};

	unsigned long		m_dwUserID;
	unsigned long		m_dwGID[ MAX_CHAR_VIEW ];
	unsigned long		m_dwFame[ MAX_CHAR_VIEW ];
};


#pragma pack()


#endif