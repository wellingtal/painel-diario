#ifndef _AUTHSERVER_TO_DBAGENT_SERVER_PACKET_H_
#define _AUTHSERVER_TO_DBAGENT_SERVER_PACKET_H_

#include <winsock2.h>
#include <windows.h>
#include <DB/DBDefine.h>
#include "DataPacket.h"

#pragma pack(1)

// ---------------------------------------------------------
// ���� �α��� ��û     AuthServer -> DBAgentServer
typedef struct PktULD* LPPktULD;
struct PktULD : public PktDD 
{
	unsigned long	m_dwSessionID;
	unsigned long	m_dwUserID;

	IN_ADDR			m_Address;	

	unsigned char	m_cLoginType;
};

// ���� �α��� ����     DBAgentServer -> AuthServer
typedef struct PktULDAck* LPPktULDAck;
struct PktULDAck : public PktDD 
{
	enum Const
	{
		MAX_CHAR_VIEW = 5
	};

	unsigned long		m_dwUserID;
	sGuildData			m_GuildData[MAX_CHAR_VIEW];
	CHAR_VIEW			m_CharView[MAX_CHAR_VIEW];

	STORE_INFO			m_sStoreInfo;

	// WORK_LIST 2.1 ���� ���� �߰�
	unsigned char		m_cAccountNation;	// ���� ����
	
	int					m_nPlayTime;        // BillingType�� ������ ���� ���� ��¥. ������ ���� ���� �ð�(��)
	unsigned short		m_usAdminLevel;
	unsigned char		m_cBillingType;     // D : ���� T : ����  N : ����
	unsigned char		m_cBillingUser;     // 0 : ���� 1 : �ǽù�

	unsigned char	m_cLoginType;	
};


// ���� �α׾ƿ�	AuthServer -> DBAgentServer
typedef struct PktULoD *LPPktULoD;
struct PktULoD : public PktDD 
{
	unsigned long	m_dwSessionID;
	unsigned long	m_dwUserID;
};

// ���� �̵�
typedef struct PktUMvD *LPPktUMvD;
struct PktUMvD : public PktDD 
{
	unsigned long	m_dwSessionID;
	unsigned long	m_dwUserID;
};


// ���� ����		DBAgentServer -> AuthServer
typedef struct PktSSD *LPPktSSD;
struct PktSSD : public PktDD 
{
	enum 
	{ 
		// edith 2008.03.17 ID,PASS ��������
		MaxName = 24,
		MaxPass = 36,
	};

	unsigned long	m_dwSessionID;
	unsigned long	m_dwUserID;
	char			m_AccountName[MaxName];
	// edith 2008.01.15 �н����� �߰��۾�
	char			m_Password[MaxPass];

    unsigned char   m_cAgentServerType;    
    unsigned char   m_cFirstLogin;

	IN_ADDR			m_Address;
};

/*

* cAgentServerType ���
CServerSetup::AgentServerType �� ���� ������ ��.

    Part1           = 20,
    Part1Unified    = 21,
    Part2           = 22,
    Part2Unified    = 23,
    Part2Selectable = 24,

* cFirstLogin ���

�������� 0 �̻��̸� �����Ѵ�. 

cAgentServerType���� Part2Unified/Part2Selectable�� ��츸 ���ǹ��� ���̴�.

Part2Unified
    â�� ������ �� ���� : 0
    Part1 ���տ��� â�� ������ �� �ִ� : 1
    Part2 ���տ��� â�� ������ �� �ִ� : 2

Part2Selectable

    �α����� ���� ���� : 0
    �α����� ���� �ִ� : 2        
*/


// ---------------------------------------------------------


// ---------------------------------------------------------
// ĳ���� ���� ��û     AuthServer -> DBAgentServer
typedef struct PktCCD* LPPktCCD;
struct PktCCD : public PktDD 
{
	unsigned long	m_dwUserID;

	unsigned long	m_dwSlot;
	unsigned long	m_dwGold;
	POS				m_Pos;
	CHAR_CREATE		m_CharCreate;

	unsigned short	m_wSkill;
};


// ĳ���� ���� ����     DBAgentServer -> AuthServer
typedef struct PktCCDAck* LPPktCCDAck;
struct PktCCDAck : public PktDD 
{
	unsigned long	m_dwCharID;
	unsigned long	m_dwSlot;
	CHAR_VIEW		m_CharView;
};
// ---------------------------------------------------------



// ---------------------------------------------------------
// ĳ���� ���� ���� (������)
typedef struct PktCSD* LPPktCSD;
struct PktCSD : public PktDD 
{
	unsigned long		m_dwUserID;	
	unsigned long		m_dwCharID;		
};

typedef struct PktCSDAck* LPPktCSDAck;
struct PktCSDAck : public PktDD 
{
	enum { MAX_CHANNEL_NUM = 5 };

	unsigned char		m_cZone;
	unsigned short		m_wChannelNum[MAX_CHANNEL_NUM];		
};

// ---------------------------------------------------------


// ---------------------------------------------------------
// ĳ���� ���� ���� (������)
typedef struct PktCDD* LPPktCDD;
struct PktCDD : public PktDD 
{
	unsigned long	m_dwUserID;
	unsigned long	m_dwSlotNum;
	unsigned long	m_dwCharID;
};
// ---------------------------------------------------------


// WORK_LIST 2.1 ���� ���� �߰�
// ---------------------------------------------------------
// ���� ���� ���� ���� (������)
typedef struct PktSAND* LPPktSAND;
struct PktSAND : public PktDD 
{
	enum eType { TYPE_SET = 1, TYPE_CHANGE = 2 };

	unsigned long	m_dwUserID;
	unsigned char	m_cType;
	unsigned char	m_cAccountNation;
};

// ���� ���� ���濡 ���� ��� ��Ŷ
typedef struct PktANCAck* LPPktANCAck;
struct PktANCAck : public PktDD
{
	enum
	{
		MAX_CHAR_VIEW	= 5
	};

	unsigned long	m_dwUserID;
	unsigned long	m_dwGID[ MAX_CHAR_VIEW ];
	unsigned long	m_dwFame[ MAX_CHAR_VIEW ];
};
// ---------------------------------------------------------

// ---------------------------------------------------------
// ĳ���� ���� ������ ���� (������)
typedef struct PktCCID* LPPktCCID;
struct PktCCID : public PktDD 
{
    unsigned __int64    m_dwItemSerial;
	unsigned long	    m_dwCharID;
    unsigned long	    m_dwSize;
};
// ---------------------------------------------------------


// ---------------------------------------------------------
// ĳ���� �α��� ���� (������)
typedef struct PktCLiD* LPPktCLiD;
struct PktCLiD : public PktDD 
{
	unsigned long	m_dwUserID;
	unsigned long	m_dwCharID;
	unsigned long	m_dwSerial;	
};
// ---------------------------------------------------------

#pragma pack()

#endif