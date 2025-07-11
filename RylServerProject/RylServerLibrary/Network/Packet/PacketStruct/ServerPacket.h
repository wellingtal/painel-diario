#ifndef _DBAGENT_TO_LOGIN_
#define _DBAGENT_TO_LOGIN_

#include <DB/DBDefine.h>
#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketStruct/DataPacket.h>


#pragma pack(1)


// �� ��Ŷ ( Ŭ���̾�Ʈ -> ������ 5�ʸ���.. ���� - ������ ��ŵ�, �����ϴ� �ʿ��� Ping�� �ش�. )
typedef struct PktSyP* LPPktSyP;
struct PktSyP : public PktBase
{
    unsigned long m_dwTickTime;
};


// ���� �α��� ��û ��Ŷ
typedef struct PktSL* LPPktSL;
struct PktSL : public PktBase 
{
	unsigned long	m_dwServerID;
	IN_ADDR			m_Address;	
};


// ���� �α��� ���� ��Ŷ
typedef struct PktSLAck* LPPktSLAck;
struct PktSLAck : public PktBase 
{
	enum { PATCH_ADDRESS_LENGTH = 100 };

	unsigned __int64    m_dlItemUID;
	unsigned long	    m_dwServerID;
	
	unsigned long	    m_dwClientVer;
	unsigned long	    m_dwCheckSum;	

	char			    m_PatchAddress[PATCH_ADDRESS_LENGTH];
};


// ���� ���� ������Ʈ ����
typedef struct PktSVU* LPPktSVU;
struct PktSVU : public PktBase 
{
	enum { PATCH_ADDRESS_LENGTH = 100 };

	unsigned long	m_dwServerID;
	IN_ADDR			m_ServerAddr;	
	unsigned long	m_dwClientVer;
	unsigned long	m_dwCheckSum;	
	char			m_PatchAddress[PATCH_ADDRESS_LENGTH];
};


// ä�� ������Ʈ ����
typedef struct PktSCInfo* LPPktSCInfo;
struct PktSCInfo : public PktBase 
{
	enum { MAX_CHANNEL_NUM = 5 };

	unsigned long		m_dwServerID;
	unsigned char		m_cChannelNum;
	unsigned short		m_usChannelClientNum[MAX_CHANNEL_NUM];
};


// ---------------------------------------------------------------------------
// ��ġ ���� ���� ��Ŷ


// ��ġ ���� ��û ��Ŷ
typedef struct PktSPI* LPPktSPI;
struct PktSPI : public PktBase 
{
	unsigned long	m_dwServerID;
};

// ��ġ ���� ���� ��Ŷ
typedef struct PktSPIAck* LPPktSPIAck;
struct PktSPIAck : public PktBase 
{
	enum { PATCH_ADDRESS_LENGTH = 100 };
	unsigned long	m_dwClientVer;
	char			m_PatchAddress[PATCH_ADDRESS_LENGTH];
};


// ---------------------------------------------------------------------------
// Zone ���� ��Ŷ

// ������ ����
typedef struct PktSZ* LPPktSZ;
struct PktSZ : public PktBase 
{
    unsigned short		m_cChannel;
	unsigned char		m_cZone;	
};

// ������ ���� Ack
typedef struct PktSZAck* LPPktSZAck;
struct PktSZAck : public PktBase 
{
	unsigned long		m_dwServerID;
    sockaddr_in         m_GameServerTCPAddr;	
};

// ������ �̵�
typedef struct PktSZMv* LPPktSZMv;
struct PktSZMv : public PktBase 
{
    POS		m_NewPos;
	char	m_cZone;
    char    m_cChannel;
};

// ������ ���� Ack
typedef struct PktSZMvAck* LPPktSZMvAck;
struct PktSZMvAck : public PktBase 
{
	enum { MAX_CHANNEL_NUM = 5 };

    unsigned short	m_wChannelNum[MAX_CHANNEL_NUM];
	unsigned char	m_cZone;	
};

typedef struct PktSZMvD* LPPktSZMvD;
struct PktSZMvD : public PktDD 
{
	unsigned long	m_dwUserID;
	char	m_cZone;		
	char	m_cChannel;

	POS		m_NewPos;			
};

typedef struct PktSZMvDAck* LPPktSZMvDAck;
struct PktSZMvDAck : public PktDD 
{
	enum 
    { 
        MAX_CHANNEL_NUM = 5 
    };

	enum PktSZMvDAckErr
	{
		FAIL_INVALID_ZONE	= 3,
		FAIL_INVALID_CHAR	= 5,
		FAIL_PERSONNEL_OVER	= 6,
		FAIL_LIMIT_LEVEL	= 7
	};

	unsigned char		m_cZone;
	unsigned short		m_wChannelNum[MAX_CHANNEL_NUM];		
};

// ���� �ּ� ���
typedef struct PktSA* LPPktSA;
struct PktSA : public PktDD 
{
	unsigned long		m_dwUserID;     // ���� ���̵�
	char				m_cZone;		// �� ��ȣ
	char				m_cChannel;		// ä�� ��ȣ
};

// ���� �ּ� ��� Ack
typedef struct PktSAAck* LPPktSAAck;
struct PktSAAck : public PktDD 
{
    enum PktSAAckErr
    {
        NO_SERVER_ERR	= 0,
        SERVER_ERROR	= 1,
        FAIL_USER_LIMIT	= 4
    };

	unsigned long		m_dwServerID;		
	IN_ADDR				m_GameAddress;	
};

// ---------------------------------------------------------------------------
// ���� ����

// ���� ���̺� ������Ʈ
typedef struct PktUUT *LPPktUUT;
struct PktUUT : public PktBase 
{
	enum UpdateType
    {
		UpdateUIDTableNone			= 0, 	// ����
		UpdateUIDTableUserLogin		= 1,	// ���� �α���
		UpdateUIDTableUserLogout	= 2, 	// ���� �α׾ƿ�
		UpdateUIDTableUserMove		= 3, 	// ���� �̵�
		UpdateUIDTableCharLogin		= 4,	// ĳ���� �α���
		UpdateUIDTableCharLogout	= 5, 	// ĳ���� �α׾ƿ�
		UpdateUIDTableCharMove		= 6, 	// ĳ���� �̵�

		UpdateUIDTableBillingCheck	= 7, 	// ���� üũ

        MAX_UPDATE_TYPE             = 8
	};

    enum Err
    {
        DISCONNECT_USER     = 1
    };

	enum
    {
		// edith 2008.03.17 ID,PASS ��������
        MaxAccountLen       = 24,	
        MaxPasswordLen       = 36
    };

	unsigned char	m_cCmd;
	char			m_strAccount[MaxAccountLen];

	// edith 2008.01.15 �н����� �߰��۾�
	char			m_strPassword[MaxPasswordLen];

	IN_ADDR			m_IPAddress;	

	unsigned long	m_dwSessionID;
	unsigned long	m_dwServerID;
	unsigned long	m_dwUserID;
	unsigned long	m_dwCharID;
};

typedef struct PktUUTAck *LPPktUUTAck;
struct PktUUTAck : public PktBase
{
	unsigned char	m_cCmd;

	unsigned long	m_dwUserID;
	unsigned long	m_dwFlag;				// Success(=0)
	int             m_nPlayTime;			// ���� ���� �ð� (��, ��)
    unsigned long   m_dwCRMIndex1;          // �ǽù� ID(0�̸� ��������)
	char			m_cstrBillingType;		// D, T(N: �������)
};

// ���� ���̱�.
typedef struct PktUK* LPPktUK;
struct PktUK : public PktBase
{
	unsigned long	m_dwUserID;
	unsigned long	m_dwCharID;
	unsigned long	m_dwServerID;
};

// ���� Ÿ�Ӿƿ� ����. (Billing Timeout Notify)
typedef struct PktBTN* LPPktBTN;
struct PktBTN : public PktBase 
{
    unsigned long	m_dwUserID;
    unsigned long	m_dwCharID;
    unsigned long	m_dwServerID;
    unsigned char   m_cRemainMinute;    // ���� ��..
    unsigned char   m_cBillingType;     // 'D' ������    'T' ������
};

// �Ѱ��� ���պ��� ��� �޽���
typedef struct PktHanBTN* LPPktHanBTN;
struct PktHanBTN : public PktBase
{
    enum
    {
        MAX_HAN_BTN = 70
    };

    unsigned long   m_dwIP;
    unsigned long   m_dwUID;
    unsigned long   m_dwCID;
    char    m_szAccount[PktUUT::MaxAccountLen];
    char    m_szMsg[MAX_HAN_BTN];
};

// �Ѱ��� ���պ��� ���� ���� �޽���(������ ��, IP��)
typedef struct PktHanUserKill* LPPktHanUserKill;
struct PktHanUserKill : public PktBase
{
    enum
    {
        DISCONN_ALL_IP  = 0xFFFFFFFF
    };   

    unsigned long   m_dwIP;
    char    m_szAccount[PktUUT::MaxAccountLen];
};

#pragma pack()

#endif